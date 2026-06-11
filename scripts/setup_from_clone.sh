#!/usr/bin/env bash
# Réintègre les fichiers manquants après un clone git et prépare le client.
# Usage :
#   export T4C_ORIGINAL_CLIENT="/chemin/vers/Original Client 1.72"
#   ./scripts/setup_from_clone.sh            # setup complet (assets + build + tests)
#   ./scripts/setup_from_clone.sh --check    # diagnostic seul, ne modifie RIEN
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CHECK_ONLY=0
[[ "${1:-}" == "--check" ]] && CHECK_ONLY=1

ok()   { printf '  [OK]      %s\n' "$*"; }
miss() { printf '  [MANQUE]  %s\n' "$*"; MISSING=1; }
MISSING=0

# Fichiers critiques attendus dans runtime/ (échantillon représentatif).
CRITICAL_ASSETS=(
  "Game Files/V2DataI.did"
  "Game Files/TMI_Map.dat"
  "Game Files/RT_Map.dat"
  "Game Files/UseMapBDD.dat"
  "english.elng"
  "EnglishGUI.elng"
  "Images/load800600.pcx"
  "FX Files"
  "Music Files"
)

echo "=== [1/5] Outils ==="
for tool in cmake g++ python3; do
  if command -v "${tool}" >/dev/null 2>&1; then ok "${tool}"; else miss "${tool} (installer via le gestionnaire de paquets)"; fi
done
if pkg-config --exists sdl2 2>/dev/null; then ok "SDL2 (dev)"; else miss "SDL2 dev (libsdl2-dev)"; fi

echo "=== [2/5] Assets runtime/ ==="
if [[ ${CHECK_ONLY} -eq 0 ]]; then
  if [[ ! -f "${ROOT}/runtime/english.elng" ]]; then
    if [[ -z "${T4C_ORIGINAL_CLIENT:-}" ]]; then
      echo "  runtime/ absent et T4C_ORIGINAL_CLIENT non défini." >&2
      echo "  export T4C_ORIGINAL_CLIENT=\"/chemin/vers/Original Client 1.72\"" >&2
      exit 1
    fi
    "${ROOT}/scripts/copy_runtime_assets.sh"
  else
    echo "  runtime/ déjà présent — copie sautée (relancer copy_runtime_assets.sh pour rafraîchir)."
  fi
fi

echo "=== [3/5] Fichiers critiques ==="
for f in "${CRITICAL_ASSETS[@]}"; do
  if [[ -e "${ROOT}/runtime/${f}" ]]; then ok "runtime/${f}"; else miss "runtime/${f}"; fi
done

if [[ ${CHECK_ONLY} -eq 1 ]]; then
  echo ""
  if [[ ${MISSING} -eq 0 ]]; then
    echo "Diagnostic : tout est présent. Lancer ./scripts/setup_from_clone.sh (sans --check)."
  else
    echo "Diagnostic : éléments manquants ci-dessus. Assets → définir T4C_ORIGINAL_CLIENT puis relancer."
  fi
  exit "${MISSING}"
fi

if [[ ${MISSING} -ne 0 ]]; then
  echo ""
  echo "Des fichiers critiques manquent (liste ci-dessus) — vérifier la source T4C_ORIGINAL_CLIENT." >&2
  exit 1
fi

echo "=== [4/5] Build client (+ tables générées) ==="
"${ROOT}/scripts/build_client.sh"

echo "=== [5/5] Terminé ==="
echo ""
echo "Lancer le client :"
echo "  T4C_RUNTIME=${ROOT}/runtime ${ROOT}/build/linux/t4c_client <ip_serveur> 11677"
echo ""
echo "Test E2E complet (serveur requis, voir SETUP.md) :"
echo "  ${ROOT}/scripts/run_e2e_test.sh"
