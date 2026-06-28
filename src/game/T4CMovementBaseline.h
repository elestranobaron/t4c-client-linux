#pragma once

#include <cstdint>

/** Constantes baseline — voir CHANGELOG « BASELINE IMMUABLE ». */
constexpr int kT4CPlayerScrollSteps = 8;
/** Cycle marche joueur / classes PC (warrio000-a … warrio000-i). */
constexpr int kT4CWalkAnimFrames = 9;
/** Cycle marche creature PNJ (Rat=6, Bat=8 — 6 couvre la majorite sans frame fantome). */
constexpr int kT4CNpcWalkAnimFrames = 6;
/** Cycle marche puppet joueur (Icon3D LoadSprite3D faces=13, View045-a … View045-m). */
constexpr int kT4CPuppetWalkAnimFrames = 13;
/** Intervalle tick marche PNJ distants (VisualObjectList nbSprite4Move, ~90 ms). */
constexpr int kT4CWalkAnimTickMs = 90;
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

constexpr int kT4CIdleAnimFrames = 4;
constexpr int kT4CIdleAnimTickMs = 150;

/** Frame suivante cycle idle StMov (0 … kT4CIdleAnimFrames-1). */
int T4CWalkAnimNextIdleFrame(int frame);

/** Frame suivante cycle marche creature PNJ (0 … kT4CNpcWalkAnimFrames-1). */
int T4CWalkAnimNextNpcFrame(int frame);

/**
 * Frame suivante puppet joueur — VisualObjectList.cpp Type==0, Faces=13 :
 * SpriteNumber 1..13 puis wrap (equiv. frame 0..12 modulo 13).
 */
int T4CWalkAnimNextPuppetFrame(int frame);
