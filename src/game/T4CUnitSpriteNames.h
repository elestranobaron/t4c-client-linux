#pragma once

#include "game/T4CMovementBaseline.h"

#include <string>
#include <vector>

/** Vue 3D (angle + miroir) pour une direction T4C 1–9 (Icon3D.cpp). */
struct T4CUnitSpriteView {
    int angleDegrees{0};
    bool mirror{false};
};

/** Frames cycle attaque Icon3D (GoblinA045-a … GoblinA045-i) — 9 frames pour la plupart des unites. */
constexpr int kT4CAttackAnimFrames = 9;

/** Cadavre Icon3D (GoblinC-a …) — FacesC, pas d'angle. */
constexpr int kT4CCorpseAnimMaxFrames = 6;
constexpr int kT4CCorpseAnimTickMs = 150;
constexpr int kT4CDyingPhaseMs = 500;
constexpr int kT4CCorpseLifetimeMs = 1500;

/** Nom frame V2 debout / marche : « Warrio045-b » (Icon3D View*, ST_MOVING). */
std::string T4CUnitSpriteFrameName(const char *npcBase, const T4CUnitSpriteView &view, int frameIndex,
                                   int maxFrameIndex = kT4CWalkAnimFrames - 1);

/** Nom frame attaque : « GoblinStAtt045-a » (DrawSprite3DA). */
std::string T4CUnitAttackSpriteFrameName(const char *npcBase, const T4CUnitSpriteView &view, int frameIndex);

/** Nom frame cadavre : « GoblinC-a » (Icon3D DrawCorpse). */
std::string T4CUnitCorpseSpriteFrameName(const char *npcBase, int frameIndex);

/** Apparence cadavre creature (plages 15001–19999 / 25001–29999, cf. ChangeType Windows). */
bool T4CIsCreatureCorpseAppearance(std::uint16_t appearance);

/** Remap TYPE opcode 12 (Packet.cpp RQ_DepositObject) avant ChangeType. */
std::uint16_t T4CNormalizeDepositObjectType(std::uint16_t type);

/** Cadavre serveur depuis apparence vivante (Bat 20002 → 25002, ObjectListing +5000). */
std::uint16_t T4CCreatureCorpseAppearanceFromLive(std::uint16_t liveAppearance);

/** Cle palette CV2PalManager (ViewID) depuis un nom complet « PupWhiteRobe090-a ». */
std::string T4CUnitSpritePaletteKey(const std::string &spriteName);

/** Nom frame idle debout : « GoblinStMov045-a » (Icon3D StMov*, ST_STANDING). */
std::string T4CUnitSpriteIdleFrameName(const char *npcBase, const T4CUnitSpriteView &view, int frameIndex,
                                       int maxFrameIndex = kT4CIdleAnimFrames - 1);

/** Frame suivante cycle PNJ (0 … kT4CWalkAnimFrames-1). */
int T4CWalkAnimNextFrame(int frame);

/** Noms a precharger pour un sprite unitaire (StMov idle + 9 frames marche par angle). */
void T4CAppendUnitSpritePreload(const char *base, std::vector<std::string> *queue);

/** Preload puppet : 13 frames View par angle (Icon3D faces=13). */
void T4CAppendPuppetSpritePreload(const char *base, std::vector<std::string> *queue);

/** Preload minimal entree monde : pose debout frame 0 par angle (5 angles). */
void T4CAppendUnitSpritePreloadMinimal(const char *base, std::vector<std::string> *queue);
