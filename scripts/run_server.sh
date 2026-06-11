#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVER_DIR="${T4C_SERVER_DIR:-${ROOT}/../FinalStEp0/T4Serv3}"
RUNTIME="${T4C_SERVER_RUNTIME:-${SERVER_DIR}/runtime}"
BIN="${SERVER_DIR}/build/T4CServer"

if [[ ! -x "${BIN}" ]]; then
  echo "Binaire absent — lancez: ${ROOT}/scripts/build_server.sh" >&2
  exit 1
fi

if [[ ! -d "${RUNTIME}/WDA" ]]; then
  echo "Runtime serveur absent — lancez: ${SERVER_DIR}/scripts/copy_server_runtime.sh" >&2
  exit 1
fi

# Sync runtime → build (WDA, ini, lng…) puis lance depuis build/
if command -v rsync >/dev/null 2>&1; then
  rsync -a "${RUNTIME}/" "${SERVER_DIR}/build/"
else
  cp -a "${RUNTIME}/." "${SERVER_DIR}/build/"
fi

cd "${SERVER_DIR}/build"
exec "./T4CServer" "$@"
