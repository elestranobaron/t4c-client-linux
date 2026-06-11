#pragma once

#include <string>
#include <vector>

/** Vue 3D (angle + miroir) pour une direction T4C 1–9 (Icon3D.cpp). */
struct T4CUnitSpriteView {
    int angleDegrees{0};
    bool mirror{false};
};

/** Frames cycle attaque Icon3D (GoblinA045-a … GoblinA045-i) — 9 frames pour la plupart des unites. */
constexpr int kT4CAttackAnimFrames = 9;

/** Nom frame V2 debout / marche : « Warrio045-b » (Icon3D View*, pas StMov pour les puppets). */
std::string T4CUnitSpriteFrameName(const char *npcBase, const T4CUnitSpriteView &view, int frameIndex);

/** Nom frame attaque : « GoblinStAtt045-a » (DrawSprite3DA). */
std::string T4CUnitAttackSpriteFrameName(const char *npcBase, const T4CUnitSpriteView &view, int frameIndex);

/** Cle palette CV2PalManager (ViewID) depuis un nom complet « PupWhiteRobe090-a ». */
std::string T4CUnitSpritePaletteKey(const std::string &spriteName);

/** Noms a precharger pour un sprite unitaire (debout + 3 frames marche par angle). */
void T4CAppendUnitSpritePreload(const char *base, std::vector<std::string> *queue);
