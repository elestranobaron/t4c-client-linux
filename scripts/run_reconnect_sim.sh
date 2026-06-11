#!/usr/bin/env bash
# Simulations auth / reconnect (headless). Usage: ./scripts/run_reconnect_sim.sh [host] [port]
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVER_DIR="${T4C_SERVER_DIR:-${ROOT}/../FinalStEp0/T4Serv3}"
HOST="${1:-127.0.0.1}"
PORT="${2:-11677}"
PROBE="${ROOT}/build/linux/t4c_e2e_auth_probe"
export T4C_RUNTIME="${ROOT}/runtime"

fail() { echo "FAIL: $*"; exit 1; }

[[ -x "${PROBE}" ]] || fail "build probe: ./scripts/build_client.sh"
[[ -x "${SERVER_DIR}/build/T4CServer" ]] || fail "build server"

pkill T4CServer 2>/dev/null || true
sleep 1
rsync -a "${SERVER_DIR}/runtime/" "${SERVER_DIR}/build/" 2>/dev/null || true
(
  cd "${SERVER_DIR}/build"
  export ODBCINI="${ODBCINI:-${HOME}/.odbc.ini}"
  exec ./T4CServer
) >/tmp/t4c_reconnect_sim_srv.log 2>&1 &
SPID=$!
trap 'kill "${SPID}" 2>/dev/null || true' EXIT

for i in $(seq 1 120); do
  grep -qF '[BOOT] SERVER_READY' /tmp/t4c_reconnect_sim_srv.log 2>/dev/null && break
  sleep 1
done
grep -qF '[BOOT] SERVER_READY' /tmp/t4c_reconnect_sim_srv.log || fail "server boot timeout"

restart_server() {
  kill "${SPID}" 2>/dev/null || true
  wait "${SPID}" 2>/dev/null || true
  sleep 1
  : > /tmp/t4c_reconnect_sim_srv.log
  (
    cd "${SERVER_DIR}/build"
    export ODBCINI="${ODBCINI:-${HOME}/.odbc.ini}"
    exec ./T4CServer
  ) >>/tmp/t4c_reconnect_sim_srv.log 2>&1 &
  SPID=$!
  for i in $(seq 1 120); do
    grep -qF '[BOOT] SERVER_READY' /tmp/t4c_reconnect_sim_srv.log 2>/dev/null && return 0
    sleep 1
  done
  fail "server boot timeout"
}

run_case() {
  local name="$1"
  shift
  echo "=== ${name} ==="
  if (cd "${ROOT}/build/linux" && "$@"); then
    echo "PASS ${name}"
  else
    echo "FAIL ${name} (rc=$?)"
    strings /tmp/t4c_reconnect_sim_srv.log 2>/dev/null | grep -E '\[AUTH\]|\[PKT\]' | tail -15 || true
    exit 1
  fi
}

run_case "auth simple" \
  env ./t4c_e2e_auth_probe "${HOST}" "${PORT}" test test

restart_server

run_case "double Connect (1 socket)" \
  env T4C_E2E_DOUBLE_CONNECT=1 ./t4c_e2e_auth_probe "${HOST}" "${PORT}" test test

restart_server

run_case "Esc persos -> reconnect" \
  env T4C_E2E_RECONNECT=1 ./t4c_e2e_auth_probe "${HOST}" "${PORT}" test test

restart_server

run_case "monde -> retour login -> reconnect" \
  env T4C_E2E_ENTER_WORLD=1 T4C_E2E_WORLD_RECONNECT=1 ./t4c_e2e_auth_probe "${HOST}" "${PORT}" test test

restart_server

run_case "monde -> deplacement (move confirme serveur)" \
  env T4C_E2E_ENTER_WORLD=1 T4C_E2E_MOVE_TEST=1 ./t4c_e2e_auth_probe "${HOST}" "${PORT}" test test

restart_server

# Garde anti-regression du crash serveur opcode 68 (PacketPuppetInfo, pl NULL) :
# apres une reconnexion depuis le monde, re-entrer + bouger doit etre confirme,
# et surtout le serveur ne doit PAS crasher (sinon les moves partent dans le vide).
run_case "monde -> move -> reconnect -> monde -> move (anti-crash opcode 68)" \
  env T4C_E2E_ENTER_WORLD=1 T4C_E2E_WORLD_RECONNECT=1 T4C_E2E_MOVE_TEST=1 \
  ./t4c_e2e_auth_probe "${HOST}" "${PORT}" test test

# Le serveur doit toujours etre vivant apres le scenario (detection crash).
if ! kill -0 "${SPID}" 2>/dev/null; then
  echo "FAIL serveur crashe pendant le scenario move/reconnect"
  strings /tmp/t4c_reconnect_sim_srv.log 2>/dev/null | tail -20 || true
  exit 1
fi

echo "=== ALL PASS ==="
