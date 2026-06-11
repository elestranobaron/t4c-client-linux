#pragma once

#include <cstdint>

/** Constantes baseline — voir CHANGELOG « BASELINE IMMUABLE ». */
constexpr int kT4CPlayerScrollSteps = 8;
/** FacesStM puppet / Icon3D — cycle marche Windows (StMov000-a … StMov000-i). */
constexpr int kT4CWalkAnimFrames = 9;
constexpr std::uint16_t kT4CEventObjectMovedOpcode = 1;

/** Position serveur (tuile) + centre visuel carte (float). */
struct T4CPlayerViewState {
    unsigned int serverX{0};
    unsigned int serverY{0};
    float displayX{0.f};
    float displayY{0.f};
};

bool T4CMoveDeltaFromOpcode(std::uint16_t opcode, int &dx, int &dy);
std::uint16_t T4CMoveOpcodeFromArrows(bool left, bool right, bool up, bool down);

/** Direction numpad 1–9 depuis delta monde (TileSet::MoveToPosition). */
int T4CDirectionFromWorldDelta(int dx, int dy);

/** Direction depuis opcode **envoye** client 1–8 (tryMovePlayer). */
int T4CDirectionFromMoveOpcode(std::uint16_t moveOpcode);

/**
 * Direction apres confirmation serveur : delta position uniquement.
 * packetOpcode est toujours kT4CEventObjectMovedOpcode (=1) — ne jamais l'utiliser pour la direction.
 */
int T4CDirectionFromServerMoveConfirm(int dx, int dy, std::uint16_t packetOpcode);

bool T4CPlayerViewIsScrolling(const T4CPlayerViewState &state);

/** Un pas de lerp display → serveur (1/kT4CPlayerScrollSteps tuile). Retourne true si scroll en cours. */
bool T4CPlayerScrollStep(T4CPlayerViewState *state, int scrollSteps = kT4CPlayerScrollSteps);

void T4CPlayerViewSnapDisplay(T4CPlayerViewState *state);

/** Applique position serveur sans snap display (baseline applyServerPlayerMove). */
void T4CPlayerViewApplyServerMove(T4CPlayerViewState *state, unsigned int newX, unsigned int newY);

/**
 * Politique animation : reset frame si debut marche ou changement direction.
 * Retourne true si frame/tick doivent etre remis a zero.
 */
bool T4CWalkAnimShouldResetFrame(bool wasMoving, int oldDirection, int newDirection);

/** Frame suivante du cycle StMov (0 … kT4CWalkAnimFrames-1). */
int T4CWalkAnimNextFrame(int frame);
