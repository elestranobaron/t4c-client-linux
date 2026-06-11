#!/usr/bin/env bash
# E2E combat/mort/temple : regression + auth + reconnect + verifications journal.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TS="$(date '+%Y%m%d_%H%M%S')"
LOG_DIR="${ROOT}/logs/e2e/combat_${TS}"
mkdir -p "${LOG_DIR}"

log() { echo "[$(date '+%H:%M:%S')] $*" | tee -a "${LOG_DIR}/combat_e2e.log"; }

log "=== E2E combat / mort / temple ==="

"${ROOT}/scripts/build_client.sh" 2>&1 | tee "${LOG_DIR}/build_client.log"
"${ROOT}/scripts/run_regression_tests.sh" 2>&1 | tee "${LOG_DIR}/regression.log"

export T4C_E2E_ENTER_WORLD=1
export T4C_E2E_MOVE_TEST=1
"${ROOT}/scripts/run_e2e_test.sh" 2>&1 | tee "${LOG_DIR}/auth_world.log"

log "Checks manuels requis en jeu :"
log "  1. Mort gobs -> overlay mort -> auto RQ_NM_DeathResurect (202) -> temple ~2948,1041"
log "  2. SFX combat (Whooshh/Goblin Hit) sans erreur block alignment"
log "  3. Contour survol epouse sprite (pas rectangle)"
log "  4. Mithrand visible banque (log puppet spawn id/app)"
log "  5. Porte -> Open Wooden Door.wav"
log "  6. Menu Esc : sac, stats, mute audio, retour persos"
log "  7. Curseur gant StaticAttackCursor"
log "  8. Garde Olin Haad : pas de doublon fige apres GetNearItems"
log "PASS pipeline E2E automatise"
