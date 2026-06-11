#include "game/T4CPuppetDraw.h"
#include "game/T4CPuppetTypes.h"
#include "game/T4CUnitSpriteNames.h"
#include "network/T4CLoginSession.h"

#include "T4CPuppetBodyOrder.inc"
#include "T4CPuppetResolve.inc"
#include "T4CPuppetize.inc"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr std::uint16_t kDefaultBodyCloth1 = 285;
constexpr std::uint16_t kDefaultLegCloth1 = 284;

void setSlot(T4CPuppetInfo *info, const std::uint8_t slot, const std::uint8_t eq) {
    if (info && slot < kPupSlotCount) {
        info->slot[slot] = eq;
    }
}

int clampDirection(const int direction) {
    switch (direction) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 6:
        case 7:
        case 8:
        case 9:
            return direction;
        default:
            return 2;
    }
}

const T4CPuppetResolveRow *resolveRow(const T4CPuppetInfo &info, const T4CPupSlot slot, const bool female) {
    return T4CPuppetResolveLookup(static_cast<std::uint8_t>(slot), info.slot[slot], female);
}

const char *resolveSprite(const T4CPuppetInfo &info, const T4CPupSlot slot, const bool female,
                          int *paletteVariant) {
    const T4CPuppetResolveRow *row = resolveRow(info, slot, female);
    if (!row || row->hidden || !row->sprite) {
        return nullptr;
    }
    if (paletteVariant) {
        *paletteVariant = row->variant;
    }
    return row->sprite;
}

bool drawLayer(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const char *base, const int direction,
               const int frameIndex, const float screenX, const float screenY, const int paletteVariant,
               const bool attacking) {
    if (!base || base[0] == '\0') {
        return false;
    }
    const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(direction);
    if (attacking) {
        /* Pose d'attaque puppet : « PupChainMailBodyA045-a » etc. Repli sur la pose debout. */
        const std::string attFrame = T4CUnitAttackSpriteFrameName(base, view, frameIndex);
        if (!attFrame.empty() &&
            atlas.TryDrawSpriteByName(renderer, attFrame, screenX, screenY, view.mirror, paletteVariant)) {
            return true;
        }
    }
    const std::string frame = T4CUnitSpriteFrameName(base, view, attacking ? 0 : frameIndex);
    if (frame.empty()) {
        return atlas.TryDrawSpriteByName(renderer, base, screenX, screenY, view.mirror, paletteVariant);
    }
    return atlas.TryDrawSpriteByName(renderer, frame, screenX, screenY, view.mirror, paletteVariant);
}

bool drawPuppetInternal(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const T4CPuppetInfo &info,
                        const bool female, const int direction, const int frameIndex, const float screenX,
                        const float screenY, const bool attacking) {
    const int dir = clampDirection(direction);
    bool any = false;
    for (int layer = 0; layer < 19; ++layer) {
        const auto slot = static_cast<T4CPupSlot>(kPuppetBodyOrderR[dir][layer]);
        int paletteVariant = 0;
        const char *sprite = resolveSprite(info, slot, female, &paletteVariant);
        if (sprite && drawLayer(atlas, renderer, sprite, direction, frameIndex, screenX, screenY, paletteVariant,
                                attacking)) {
            any = true;
        }
    }
    return any;
}

}  // namespace

void T4CPuppetizeFromDress(const T4CPuppetDress &dress, const bool /*male*/, T4CPuppetInfo *out) {
    if (!out) {
        return;
    }
    for (int i = 0; i < kPupSlotCount; ++i) {
        out->slot[i] = 0;
    }

    T4CPuppetDress effective = dress;
    if (!effective.known) {
        effective.body = kDefaultBodyCloth1;
        effective.legs = kDefaultLegCloth1;
        effective.known = true;
    }

    static const struct {
        std::uint16_t T4CPuppetDress::*member;
    } kFields[] = {
        &T4CPuppetDress::cape,
        &T4CPuppetDress::weaponR,
        &T4CPuppetDress::weaponL,
        &T4CPuppetDress::helm,
        &T4CPuppetDress::gloves,
        &T4CPuppetDress::legs,
        &T4CPuppetDress::feet,
        &T4CPuppetDress::body,
    };

    for (const auto &f : kFields) {
        const std::uint16_t objgroup = effective.*f.member;
        if (objgroup == 0) {
            continue;
        }
        for (int i = 0; i < kPuppetizeRowCount; ++i) {
            const T4CPuppetizeRow &row = kPuppetizeRows[i];
            if (row.objgroup == objgroup) {
                setSlot(out, row.slot, row.pupeq);
            }
        }
    }
}

const char *T4CPuppetResolveSprite(const T4CPupSlot slot, const std::uint8_t pupeq, const bool female) {
    const T4CPuppetResolveRow *row = T4CPuppetResolveLookup(static_cast<std::uint8_t>(slot), pupeq, female);
    if (!row || row->hidden) {
        return nullptr;
    }
    return row->sprite;
}

void T4CPuppetCollectPreloadBases(std::vector<std::string> *out) {
    if (!out) {
        return;
    }
    for (int i = 0; i < kPuppetResolveRowCount; ++i) {
        const char *sp = kPuppetResolveRows[i].sprite;
        if (!sp || sp[0] == '\0') {
            continue;
        }
        const std::string name(sp);
        if (std::find(out->begin(), out->end(), name) == out->end()) {
            out->push_back(name);
        }
    }
}

void T4CPuppetAppendDressPreload(const T4CPuppetDress &dress, const bool female, std::vector<std::string> *out) {
    if (!out) {
        return;
    }
    T4CPuppetInfo info{};
    T4CPuppetizeFromDress(dress, !female, &info);
    for (int slot = 0; slot < kPupSlotCount; ++slot) {
        const char *sprite = resolveSprite(info, static_cast<T4CPupSlot>(slot), female, nullptr);
        if (!sprite) {
            continue;
        }
        T4CAppendUnitSpritePreload(sprite, out);
    }
}

const char *T4CPuppetFallbackSpriteName(const T4CPuppetDress &dress, const bool female) {
    if (dress.known) {
        if (dress.body == 425) {
            return "Cleric";
        }
        if (dress.body == 269) {
            return "GuardModel1";
        }
    }
    return female ? "PaysanneModel1" : "Warrio";
}

bool T4CPuppetTryDraw(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const T4CPuppetDress &dress,
                      const bool female, const int direction, const int frameIndex, const float screenX,
                      const float screenY, const bool attacking) {
    if (!renderer) {
        return false;
    }
    T4CPuppetInfo info{};
    T4CPuppetizeFromDress(dress, !female, &info);
    return drawPuppetInternal(atlas, renderer, info, female, direction, frameIndex, screenX, screenY, attacking);
}

bool T4CPuppetTryDrawPlayer(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const T4CActivePlayer &player,
                            const int direction, const int frameIndex, const float screenX, const float screenY,
                            const bool attacking) {
    T4CPuppetDress dress{};
    if (player.puppetKnown) {
        dress.body = player.puppetBody;
        dress.feet = player.puppetFeet;
        dress.gloves = player.puppetGloves;
        dress.helm = player.puppetHelm;
        dress.legs = player.puppetLegs;
        dress.weaponR = player.puppetWeaponR;
        dress.weaponL = player.puppetWeaponL;
        dress.cape = player.puppetCape;
        dress.known = true;
    }
    return T4CPuppetTryDraw(atlas, renderer, dress, player.female, direction, frameIndex, screenX, screenY,
                            attacking);
}
