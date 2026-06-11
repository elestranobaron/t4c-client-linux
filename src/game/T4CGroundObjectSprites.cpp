#include "game/T4CGroundObjectSprites.h"

#include <algorithm>
#include <iterator>

namespace {

struct GroundSpriteRow {
    std::uint16_t appearance;
    const char *sprite;
};

constexpr GroundSpriteRow kGroundSprites[] = {
#include "game/T4CGroundObjectSpriteTable.inc"
};

constexpr int kGroundSpriteCount = static_cast<int>(std::size(kGroundSprites));

const char *lookup(const std::uint16_t appearance) {
    const GroundSpriteRow *begin = kGroundSprites;
    const GroundSpriteRow *end = kGroundSprites + kGroundSpriteCount;
    const GroundSpriteRow *it =
        std::lower_bound(begin, end, appearance,
                         [](const GroundSpriteRow &row, const std::uint16_t value) { return row.appearance < value; });
    if (it != end && it->appearance == appearance) {
        return it->sprite;
    }
    return nullptr;
}

}  // namespace

const char *T4CGroundObjectSpriteName(const std::uint16_t appearance) {
    return lookup(appearance);
}
