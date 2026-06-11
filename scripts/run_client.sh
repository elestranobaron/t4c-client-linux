#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="${ROOT}/build/linux/t4c_client"
RUNTIME="${T4C_RUNTIME:-${ROOT}/runtime}"

if [[ ! -x "${BIN}" ]]; then
  echo "Binaire absent — lancez d'abord: ${ROOT}/scripts/build_client.sh" >&2
  exit 1
fi

export T4C_RUNTIME="${RUNTIME}"
exec "${BIN}" "$@"
