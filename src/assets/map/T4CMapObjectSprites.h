#pragma once

#include <cstdint>
#include <string>

/** Resout un ID objet map2 (CompiledView2 / DrawTileSet) en nom sprite V2. */
namespace T4CMapObjectSprites {

bool ShouldDrawMapObject(std::uint16_t objectId);

/** Objet couche overlap / decor special (1300–9999, 15000–16384) — 2e passe rendu. */
bool IsSpecialOverlapObject(std::uint16_t objectId);

/**
 * Nom sprite V2 pour un ID objet. Si `mirror` non nul, indique le retournement
 * horizontal (DrawSprite(...,1) cote Windows). Renvoie {} si non mappe.
 */
std::string ObjectSpriteName(std::uint16_t objectId, unsigned int worldX, unsigned int worldY,
                             bool *mirror = nullptr);

/** Tuile map2 bloque le deplacement (mur plein, pas porte). */
bool IsBlockingMapObject(std::uint16_t objectId);

/** Porte / portail / coffre — clic envoie RQ_UseObject (23). */
bool IsInteractiveMapObject(std::uint16_t objectId);

/** Porte map2 (crypte, temple, etc.) — declenche SFX ouverture. */
bool IsDoorMapObject(std::uint16_t objectId);

}  // namespace T4CMapObjectSprites
