#include "game/T4CInvItemSprites.h"

#include "game/T4CGroundObjectSprites.h"

#include <algorithm>
#include <iterator>

namespace {

struct InvSpriteRow {
    std::uint16_t appearance;
    const char *sprite;
};

constexpr InvSpriteRow kInvSprites[] = {
#include "game/T4CInvItemSpriteTable.inc"
};

constexpr int kInvSpriteCount = static_cast<int>(std::size(kInvSprites));

const char *lookup(const std::uint16_t appearance) {
    const InvSpriteRow *begin = kInvSprites;
    const InvSpriteRow *end = kInvSprites + kInvSpriteCount;
    const InvSpriteRow *it =
        std::lower_bound(begin, end, appearance,
                         [](const InvSpriteRow &row, const std::uint16_t value) { return row.appearance < value; });
    if (it != end && it->appearance == appearance) {
        return it->sprite;
    }
    return nullptr;
}

}  // namespace

const char *T4CInvItemSpriteName(const std::uint16_t appearance) {
    if (const char *inv = lookup(appearance); inv != nullptr) {
        return inv;
    }
    const std::uint16_t normalized = T4CNormalizeGroundAppearance(appearance);
    if (normalized != appearance) {
        if (const char *inv = lookup(normalized); inv != nullptr) {
            return inv;
        }
    }
    return T4CGroundObjectSpriteName(appearance);
}
