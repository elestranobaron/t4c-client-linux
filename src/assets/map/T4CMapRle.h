#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

/** Décompression RLE des chunks carte T4C (Tileset.cpp). */
namespace T4CMapRle {

/** Couche sol / decor : dwLimit = 16384 (LASTONE). */
std::size_t DecompressFloor(const std::uint16_t *compressed, std::size_t compressedWords,
                            std::uint16_t *out, std::size_t outCount,
                            std::uint16_t limit = 16384);

/** Couches alpha (mapa/mapm/mapl/mapx) : dwLimit = 0x8000. */
std::size_t DecompressAlpha(const std::uint16_t *compressed, std::size_t compressedWords,
                            std::uint16_t *out, std::size_t outCount,
                            std::uint16_t limit = 0x8000);

}  // namespace T4CMapRle
