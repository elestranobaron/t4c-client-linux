#include "assets/map/T4CMapRle.h"

namespace T4CMapRle {

namespace {

std::size_t DecompressImpl(const std::uint16_t *compressed, std::size_t compressedWords, std::uint16_t *out,
                           std::size_t outCount, std::uint16_t limit) {
    std::size_t dst = 0;
    std::size_t src = 0;
    while (dst < outCount && src < compressedWords) {
        const std::uint16_t data = compressed[src++];
        if (data > limit) {
            if (src >= compressedWords) {
                break;
            }
            const std::uint16_t value = compressed[src++];
            const std::size_t run = static_cast<std::size_t>(data - limit);
            for (std::size_t i = 0; i < run && dst < outCount; ++i) {
                out[dst++] = value;
            }
        } else {
            out[dst++] = data;
        }
    }
    return dst;
}

}  // namespace

std::size_t DecompressFloor(const std::uint16_t *compressed, std::size_t compressedWords, std::uint16_t *out,
                            std::size_t outCount, std::uint16_t limit) {
    return DecompressImpl(compressed, compressedWords, out, outCount, limit);
}

std::size_t DecompressAlpha(const std::uint16_t *compressed, std::size_t compressedWords, std::uint16_t *out,
                            std::size_t outCount, std::uint16_t limit) {
    return DecompressImpl(compressed, compressedWords, out, outCount, limit);
}

}  // namespace T4CMapRle
