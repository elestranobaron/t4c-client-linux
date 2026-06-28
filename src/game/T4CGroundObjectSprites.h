#pragma once

#include <cstdint>

/** Remap variantes miroir serveur (_I) → objgroup canonique (VisualObjectList.cpp). */
std::uint16_t T4CNormalizeGroundAppearance(std::uint16_t appearance);

/** Sprite sol 64kItemGr* pour un objgroup serveur (appearance < 10001). */
const char *T4CGroundObjectSpriteName(std::uint16_t appearance);

/** Coffre au sol (curseur USE Windows, pas GET). */
bool T4CIsChestGroundAppearance(std::uint16_t appearance);
