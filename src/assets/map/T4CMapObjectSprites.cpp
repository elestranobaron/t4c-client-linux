#include "assets/map/T4CMapObjectSprites.h"

#include <algorithm>
#include <cstring>
#include <iterator>

namespace T4CMapObjectSprites {

namespace {

constexpr std::uint16_t kSpecialS = 499;
constexpr std::uint16_t kSpecialE = 13999;
constexpr std::uint16_t kSpecialS2 = 15000;
constexpr std::uint16_t kSpecialE2 = 16384;

/*
 * Table valeur objet -> sprite, generee depuis le client Windows
 * (Tileset.cpp giant switch + LoadSprite + enum TFCLanddef.h).
 *   - n[0..3] : variante mur LEFT/RIGHT selon (worldX-1)%4 (identique x4 si fixe).
 *   - mirror  : DrawSprite(...,1) cote Windows = retournement horizontal.
 * Couvre chateau, temples, maisons (hsw, wdh), murs, ponts, decors, etc.
 */
struct ObjEntry {
    std::uint16_t value;
    bool mirror;
    const char *n[4];
};

constexpr ObjEntry kObjTable[] = {
#include "assets/map/T4CMapObjectTable.inc"
};

constexpr int kObjTableCount = static_cast<int>(std::size(kObjTable));

const ObjEntry *findEntry(const std::uint16_t objectId) {
    const ObjEntry *begin = kObjTable;
    const ObjEntry *end = kObjTable + kObjTableCount;
    const ObjEntry *it =
        std::lower_bound(begin, end, objectId,
                         [](const ObjEntry &e, const std::uint16_t v) { return e.value < v; });
    if (it != end && it->value == objectId) {
        return it;
    }
    return nullptr;
}

}  // namespace

bool ShouldDrawMapObject(const std::uint16_t objectId) {
    return objectId != 0 && findEntry(objectId) != nullptr;
}

bool IsSpecialOverlapObject(const std::uint16_t objectId) {
    if (objectId == 0) {
        return false;
    }
    if (objectId > kSpecialS && objectId <= kSpecialE) {
        return true;
    }
    if (objectId > kSpecialS2 && objectId <= kSpecialE2) {
        return true;
    }
    return false;
}

std::string ObjectSpriteName(const std::uint16_t objectId, const unsigned int worldX,
                             const unsigned int worldY, bool *mirror) {
    (void)worldY;
    const ObjEntry *e = findEntry(objectId);
    if (!e) {
        if (mirror) {
            *mirror = false;
        }
        return {};
    }
    if (mirror) {
        *mirror = e->mirror;
    }
    /* Variante mur left/right : (worldX-1)%4 (cf. Tileset.cpp). */
    const unsigned int variant = (worldX + 3U) % 4U;
    const char *name = e->n[variant];
    return name ? std::string(name) : std::string{};
}

namespace {

bool spriteNameHas(const char *name, const char *token) {
    if (!name || !token) {
        return false;
    }
    return std::strstr(name, token) != nullptr;
}

bool classifySpriteName(const char *name, bool *blocking, bool *interactive) {
    if (!name || name[0] == '\0') {
        return false;
    }
    const bool isDoor = spriteNameHas(name, "Door") || spriteNameHas(name, "door");
    const bool isPortal = spriteNameHas(name, "Portal") || spriteNameHas(name, "portal");
    const bool isChest = spriteNameHas(name, "Chest") || spriteNameHas(name, "chest");
    const bool isWall = spriteNameHas(name, "Wall") || spriteNameHas(name, "wall");
    if (interactive) {
        *interactive = isDoor || isPortal || isChest;
    }
    if (blocking) {
        *blocking = isWall && !isDoor;
    }
    return true;
}

}  // namespace

bool IsBlockingMapObject(const std::uint16_t objectId) {
    const ObjEntry *e = findEntry(objectId);
    if (!e) {
        return false;
    }
    bool blocking = false;
    classifySpriteName(e->n[0], &blocking, nullptr);
    return blocking;
}

bool IsDoorMapObject(const std::uint16_t objectId) {
    const ObjEntry *e = findEntry(objectId);
    if (!e) {
        return false;
    }
    for (const char *name : e->n) {
        if (name && (spriteNameHas(name, "Door") || spriteNameHas(name, "door"))) {
            return true;
        }
    }
    return false;
}

bool IsInteractiveMapObject(const std::uint16_t objectId) {
    const ObjEntry *e = findEntry(objectId);
    if (!e) {
        return false;
    }
    bool interactive = false;
    classifySpriteName(e->n[0], nullptr, &interactive);
    return interactive;
}

}  // namespace T4CMapObjectSprites