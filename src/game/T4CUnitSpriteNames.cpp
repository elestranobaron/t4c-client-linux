#include "game/T4CUnitSpriteNames.h"

#include "game/T4CMovementBaseline.h"

#include <cstdio>
#include <vector>

#include <algorithm>

static int clampAnimFrame(const int frameIndex, const int maxFrame) {
    if (frameIndex < 0) {
        return 0;
    }
    return frameIndex > maxFrame ? maxFrame : frameIndex;
}

std::string T4CUnitSpriteFrameName(const char *npcBase, const T4CUnitSpriteView &view, const int frameIndex) {
    if (!npcBase || npcBase[0] == '\0') {
        return {};
    }
    const int clampedFrame = clampAnimFrame(frameIndex, kT4CWalkAnimFrames - 1);
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%s%03d-%c", npcBase, view.angleDegrees,
                  static_cast<char>('a' + clampedFrame));
    return buf;
}

std::string T4CUnitAttackSpriteFrameName(const char *npcBase, const T4CUnitSpriteView &view, const int frameIndex) {
    if (!npcBase || npcBase[0] == '\0') {
        return {};
    }
    /* Icon3D LoadSprite3D : frames d'attaque nommees « <Base>A<angle>-<lettre> »
     * (ex. GoblinA045-a). StAtt n'est que le COMPTE de frames, pas le nom. */
    const int clampedFrame = clampAnimFrame(frameIndex, kT4CAttackAnimFrames - 1);
    char buf[80];
    std::snprintf(buf, sizeof(buf), "%sA%03d-%c", npcBase, view.angleDegrees,
                  static_cast<char>('a' + clampedFrame));
    return buf;
}

std::string T4CUnitSpritePaletteKey(const std::string &spriteName) {
    std::string key = spriteName;
    if (const std::size_t p = key.find(" ("); p != std::string::npos) {
        key.resize(p);
    }
    if (key.size() >= 2 && key[key.size() - 2] == '-') {
        key.resize(key.size() - 2);
    }
    static const char *kAngles[] = {"000", "045", "090", "135", "180"};
    for (const char *angle : kAngles) {
        const std::size_t alen = 3;
        if (key.size() >= alen && key.compare(key.size() - alen, alen, angle) == 0) {
            key.resize(key.size() - alen);
            break;
        }
    }
    return key;
}

void T4CAppendUnitSpritePreload(const char *base, std::vector<std::string> *queue) {
    if (!base || base[0] == '\0' || !queue) {
        return;
    }
    const auto appendUnique = [&](const std::string &name) {
        if (name.empty()) {
            return;
        }
        if (std::find(queue->begin(), queue->end(), name) == queue->end()) {
            queue->push_back(name);
        }
    };
    appendUnique(base);
    for (const int angle : {0, 45, 90, 135, 180}) {
        T4CUnitSpriteView view{};
        view.angleDegrees = angle;
        for (const int frame : {0, 1, 2}) {
            appendUnique(T4CUnitSpriteFrameName(base, view, frame));
        }
        /* PAS de preload des frames d'attaque (<Base>A<angle>-a..i) : 45 sprites de plus par
         * base = chargement monde tres long. Elles se decodent a la demande au 1er swing. */
    }
}
