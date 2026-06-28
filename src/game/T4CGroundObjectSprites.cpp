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

/** Variantes miroir serveur (_I) → type canonique avant lookup sprite.
 * Parité `VisualObjectList.cpp::SetMouseCursor` L156-188. */
std::uint16_t normalizeGroundAppearance(const std::uint16_t appearance) {
    switch (appearance) {
        case 18:
            return 41; /* CHEST_I → CHEST */
        case 19:
            return 42; /* CHEST_OPEN_I → CHEST_OPEN */
        case 20:
            return 238; /* CHEST2_I → CHEST2 */
        case 130:
            return 15; /* WOODEN_DOOR_CLOSED_I */
        case 131:
            return 16; /* WOODEN_DOOR_OPENED_I */
        case 132:
            return 17; /* WOODEN_CHAIR_I */
        case 133:
            return 105; /* WOODEN_CHAIR_2_I */
        case 134:
            return 106; /* WOODEN_ROUND_CHAIR_2_I */
        case 138:
            return 135; /* 2_WOODEN_CHAIR_I */
        case 139:
            return 136; /* 2_WOODEN_CHAIR_2_I */
        case 140:
            return 137; /* 2_WOODEN_ROUND_CHAIR_2_I */
        case 327:
            return 323; /* VAULT_TALK_I */
        case 459:
            return 458; /* VAULT_I — cf. Apparence.h __OBJGROUP_VAULT */
        case 547:
            return 543; /* SKAVEN_CORPSE_I1 */
        case 548:
            return 544;
        case 549:
            return 545;
        case 550:
            return 546;
        case 771:
            return 769; /* WOODEN_DOOR2_CLOSED_I */
        case 772:
            return 770;
        case 793:
            return 791;
        case 794:
            return 792;
        case 806:
            return 804;
        case 807:
            return 805;
        case 819:
            return 817;
        case 820:
            return 818;
        case 832:
            return 830;
        case 833:
            return 831;
        case 845:
            return 843;
        case 846:
            return 844;
        case 858:
            return 856;
        case 859:
            return 857;
        case 1067:
            return 1065; /* TEMPLE_DOOR1_CLOSED_I */
        case 1068:
            return 1066; /* TEMPLE_DOOR1_OPENED_I */
        default:
            return appearance;
    }
}

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

std::uint16_t T4CNormalizeGroundAppearance(const std::uint16_t appearance) {
    return normalizeGroundAppearance(appearance);
}

const char *T4CGroundObjectSpriteName(const std::uint16_t appearance) {
    return lookup(normalizeGroundAppearance(appearance));
}

bool T4CIsChestGroundAppearance(const std::uint16_t appearance) {
    const std::uint16_t t = normalizeGroundAppearance(appearance);
    switch (t) {
        case 10:   /* CHEST_TALK */
        case 41:   /* CHEST */
        case 42:   /* CHEST_OPEN */
        case 238:  /* CHEST2 */
        case 426:  /* CHEST_TROLL */
        case 1079:
        case 1080:
        case 1081:
        case 1082:
        case 1083:
        case 1084:
        case 1091:
            return true;
        default:
            return false;
    }
}
