#!/usr/bin/env bash
# Tests de non-regression — a lancer apres chaque build / avant commit.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT}/build/linux"
PROBE="${BUILD_DIR}/t4c_regression_tests"

if [[ ! -x "${PROBE}" ]]; then
  echo "Binaire absent — build client d'abord."
  "${ROOT}/scripts/build_client.sh"
fi

[[ -x "${PROBE}" ]] || { echo "ERREUR: t4c_regression_tests introuvable"; exit 1; }

echo "=== t4c_regression_tests (baseline immuable) ==="
"${PROBE}"
RC=$?

if [[ ${RC} -ne 0 ]]; then
  echo ""
  echo "ECHEC regression — ne pas avancer sans corriger."
  echo "Voir CHANGELOG.md « BASELINE IMMUABLE » et src/tests/t4c_regression_tests.cpp"
  exit ${RC}
fi

echo ""
echo "Regression OK (${PROBE})"

if [[ "${T4C_RUN_E2E:-0}" == "1" ]]; then
  echo ""
  echo "=== E2E reseau (optionnel T4C_RUN_E2E=1) ==="
  "${ROOT}/scripts/run_e2e_test.sh"
fi

exit 0
