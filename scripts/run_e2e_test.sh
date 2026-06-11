#!/usr/bin/env bash
# Test E2E T4C 1.72 : preflight → serveur → sonde auth headless → journal horodate.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVER_DIR="${T4C_SERVER_DIR:-${ROOT}/../FinalStEp0/T4Serv3}"
TS="$(date '+%Y%m%d_%H%M%S')"
LOG_DIR="${T4C_E2E_LOG_DIR:-${ROOT}/logs/e2e/${TS}}"
HOST="${T4C_E2E_HOST:-127.0.0.1}"
PORT="${T4C_E2E_PORT:-11677}"
LOGIN="${T4C_E2E_LOGIN:-test}"
PASS="${T4C_E2E_PASS:-test}"
SERVER_WAIT_SEC="${T4C_E2E_SERVER_WAIT:-300}"

mkdir -p "${LOG_DIR}"

log() {
  local line="[$(date '+%Y-%m-%d %H:%M:%S')] $*"
  echo "${line}" | tee -a "${LOG_DIR}/e2e_journal.log"
}

fail() {
  log "FAIL: $*"
  exit 1
}

SERVER_PID=""
cleanup() {
  if [[ -n "${SERVER_PID}" ]] && kill -0 "${SERVER_PID}" 2>/dev/null; then
    log "Arret serveur PID ${SERVER_PID}"
    kill "${SERVER_PID}" 2>/dev/null || true
    wait "${SERVER_PID}" 2>/dev/null || true
  fi
}
trap cleanup EXIT INT TERM

log "=== E2E T4C 1.72 — session ${TS} ==="
log "Journal: ${LOG_DIR}/e2e_journal.log"

# --- Preflight ---
log "--- Preflight ---"

[[ -x "${SERVER_DIR}/build/T4CServer" ]] || fail "T4CServer absent — build_server.sh"
[[ -d "${SERVER_DIR}/runtime/WDA" ]] || fail "runtime serveur absent — copy_server_runtime.sh"

PROBE="${ROOT}/build/linux/t4c_e2e_auth_probe"
if [[ ! -x "${PROBE}" ]]; then
  log "Sonde absente — build client (t4c_e2e_auth_probe)"
  "${ROOT}/scripts/build_client.sh" 2>&1 | tee "${LOG_DIR}/build_client.log"
fi
[[ -x "${PROBE}" ]] || fail "t4c_e2e_auth_probe introuvable apres build"

for dsn in "T4C Server Authentication" "T4C Server"; do
  if ! echo "SELECT 1;" | isql -v "${dsn}" t4cuser T4Cpass2026! -b >/dev/null 2>&1; then
    fail "ODBC DSN [${dsn}] inaccessible"
  fi
  log "ODBC OK [${dsn}]"
done

# --- Arret instance precedente sur le port ---
if ss -ulnp 2>/dev/null | grep -q ":${PORT} "; then
  log "Port ${PORT} deja utilise — arret T4CServer existant"
  pkill -f "${SERVER_DIR}/build/T4CServer" 2>/dev/null || true
  sleep 2
fi

# --- Sync runtime → build ---
if command -v rsync >/dev/null 2>&1; then
  rsync -a "${SERVER_DIR}/runtime/" "${SERVER_DIR}/build/"
else
  cp -a "${SERVER_DIR}/runtime/." "${SERVER_DIR}/build/"
fi

# --- Serveur ---
log "--- Demarrage serveur ---"
mkdir -p "${SERVER_DIR}/build/logs"
(
  cd "${SERVER_DIR}/build"
  export ODBCINI="${ODBCINI:-${HOME}/.odbc.ini}"
  unset T4C_SKIP_CREATURES
  if [[ -n "${T4C_E2E_SKIP_CREATURES:-}" ]]; then
    export T4C_SKIP_CREATURES=1
    log "T4C_SKIP_CREATURES=1 (dev rapide — pas pour prod)"
  fi
  if command -v stdbuf >/dev/null 2>&1; then
    exec stdbuf -oL -eL ./T4CServer
  else
    exec ./T4CServer
  fi
) >"${LOG_DIR}/server_stdout.log" 2>&1 &
SERVER_PID=$!
log "Serveur PID ${SERVER_PID} (stdout → server_stdout.log)"

# Attente fin init (port UDP + message boot)
READY_MARK="${T4C_E2E_READY_MARK:-[BOOT] SERVER_READY}"
log "Attente boot serveur « ${READY_MARK} » (max ${SERVER_WAIT_SEC}s)…"
ready=0
for ((i = 1; i <= SERVER_WAIT_SEC; i++)); do
  if grep -qF "${READY_MARK}" "${LOG_DIR}/server_stdout.log" 2>/dev/null \
     && ss -ulnp 2>/dev/null | grep -q ":${PORT} "; then
    ready=1
    log "Serveur pret (${i}s) — port ${PORT} + init complete"
    break
  fi
  if ! kill -0 "${SERVER_PID}" 2>/dev/null; then
    log "Serveur termine prematurement — voir server_stdout.log"
    tail -40 "${LOG_DIR}/server_stdout.log" | tee -a "${LOG_DIR}/e2e_journal.log"
    fail "serveur crash au demarrage"
  fi
  sleep 1
done
[[ "${ready}" -eq 1 ]] || fail "timeout boot serveur (marqueur « ${READY_MARK} » ou port ${PORT})"

# Copie logs serveur si presents
for f in logs/Debug.log Debug.log logs/Death.log; do
  if [[ -f "${SERVER_DIR}/build/${f}" ]]; then
    cp -a "${SERVER_DIR}/build/${f}" "${LOG_DIR}/server_$(basename "${f}")" 2>/dev/null || true
  fi
done

sleep 2

# --- Sonde auth headless ---
log "--- Sonde auth (14→99→26) ---"
set +e
(
  cd "${ROOT}/build/linux"
  export T4C_RUNTIME="${ROOT}/runtime"
  ./t4c_e2e_auth_probe "${HOST}" "${PORT}" "${LOGIN}" "${PASS}"
) 2>&1 | tee "${LOG_DIR}/auth_probe.log"
PROBE_RC=${PIPESTATUS[0]}
set -e

if [[ -f "${ROOT}/build/linux/debug/t4c_network_session.log" ]]; then
  cp -a "${ROOT}/build/linux/debug/t4c_network_session.log" "${LOG_DIR}/client_network.log"
elif [[ -f "${ROOT}/debug/t4c_network_session.log" ]]; then
  cp -a "${ROOT}/debug/t4c_network_session.log" "${LOG_DIR}/client_network.log"
fi

# --- Resume ---
log "--- Resultat ---"
if [[ "${PROBE_RC}" -eq 0 ]]; then
  log "PASS auth E2E (${LOGIN}@${HOST}:${PORT})"
  RESULT=PASS
else
  log "FAIL auth E2E code=${PROBE_RC}"
  RESULT=FAIL
  tail -20 "${LOG_DIR}/auth_probe.log" | tee -a "${LOG_DIR}/e2e_journal.log"
  tail -30 "${LOG_DIR}/server_stdout.log" | tee -a "${LOG_DIR}/e2e_journal.log"
fi

{
  echo ""
  echo "## ${TS} — Test E2E auth"
  echo ""
  echo "- **Resultat** : ${RESULT}"
  echo "- **Host** : ${HOST}:${PORT}"
  echo "- **Compte** : ${LOGIN}"
  echo "- **Journal** : \`logs/e2e/${TS}/e2e_journal.log\`"
  echo "- **Serveur stdout** : \`logs/e2e/${TS}/server_stdout.log\`"
  echo "- **Sonde auth** : \`logs/e2e/${TS}/auth_probe.log\`"
  echo ""
} >>"${ROOT}/logs/e2e/E2E_INDEX.md"

log "Index mis a jour: logs/e2e/E2E_INDEX.md"
log "=== Fin session ${TS} ==="

exit "${PROBE_RC}"
