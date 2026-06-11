#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT}/build/linux"

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

python3 "${ROOT}/scripts/gen_puppet_tables.py"
python3 "${ROOT}/scripts/gen_creature_appearance_table.py"

cmake "${ROOT}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE:-Debug}"
cmake --build . -j"$(nproc 2>/dev/null || echo 4)"

echo ""
echo "=== Tests non-regression ==="
"${ROOT}/scripts/run_regression_tests.sh"

echo ""
echo "Build OK: ${BUILD_DIR}/t4c_client"
echo "Run: T4C_RUNTIME=${ROOT}/runtime ${BUILD_DIR}/t4c_client [server_ip] [port]"
