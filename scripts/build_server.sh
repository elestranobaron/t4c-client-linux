#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVER_DIR="${T4C_SERVER_DIR:-${ROOT}/../FinalStEp0/T4Serv3}"

if [[ ! -f "${SERVER_DIR}/build.sh" ]]; then
  echo "Serveur introuvable: ${SERVER_DIR}" >&2
  echo "Definissez T4C_SERVER_DIR ou placez FinalStEp0/T4Serv3 a cote de FinalStEp2." >&2
  exit 1
fi

sed -i 's/\r$//' "${SERVER_DIR}/build.sh" 2>/dev/null || true
chmod +x "${SERVER_DIR}/build.sh"
"${SERVER_DIR}/build.sh" "$@"

echo ""
echo "Serveur OK: ${SERVER_DIR}/build/T4CServer"
