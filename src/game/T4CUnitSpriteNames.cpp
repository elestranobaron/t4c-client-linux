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

std::string T4CUnitSpriteIdleFrameName(const char *npcBase, const T4CUnitSpriteView &view, const int frameIndex,
                                       const int maxFrameIndex) {
    if (!npcBase || npcBase[0] == '\0') {
        return {};
    }
    const int clampedFrame = clampAnimFrame(frameIndex, maxFrameIndex);
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%sStMov%03d-%c", npcBase, view.angleDegrees,
                  static_cast<char>('a' + clampedFrame));
    return buf;
}

std::string T4CUnitSpriteFrameName(const char *npcBase, const T4CUnitSpriteView &view, const int frameIndex,
                                   const int maxFrameIndex) {
    if (!npcBase || npcBase[0] == '\0') {
        return {};
    }
    const int clampedFrame = clampAnimFrame(frameIndex, maxFrameIndex);
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

std::string T4CUnitCorpseSpriteFrameName(const char *npcBase, const int frameIndex) {
    if (!npcBase || npcBase[0] == '\0') {
        return {};
    }
    const int clampedFrame = clampAnimFrame(frameIndex, kT4CCorpseAnimMaxFrames - 1);
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%sC-%c", npcBase, static_cast<char>('a' + clampedFrame));
    return buf;
}

bool T4CIsCreatureCorpseAppearance(const std::uint16_t appearance) {
    return (appearance > 15000 && appearance < 20000) || (appearance > 25000 && appearance < 30000);
}

std::uint16_t T4CNormalizeDepositObjectType(std::uint16_t type) {
    switch (type) {
        case 10002:
            return 20039;
        case 15002:
            return 25039;
        case 10004:
            return 20042;
        case 15004:
            return 25042;
        case 10001:
            return 20043;
        case 15001:
            return 25043;
        case 10003:
            return 20044;
        case 15003:
            return 25044;
        default:
            break;
    }
    if ((type >= 26000 && type < 30000) || (type >= 16000 && type < 20000)) {
        return static_cast<std::uint16_t>(type - 1000);
    }
    return type;
}

std::uint16_t T4CCreatureCorpseAppearanceFromLive(const std::uint16_t liveAppearance) {
    if (liveAppearance >= 10001 && liveAppearance <= 14999) {
        return static_cast<std::uint16_t>(liveAppearance + 5000);
    }
    if (liveAppearance >= 20001 && liveAppearance <= 24999) {
        return static_cast<std::uint16_t>(liveAppearance + 5000);
    }
    return liveAppearance;
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

void T4CAppendUnitSpritePreloadMinimal(const char *base, std::vector<std::string> *queue) {
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
    for (const int angle : {0, 45, 90, 135, 180}) {
        T4CUnitSpriteView view{};
        view.angleDegrees = angle;
        appendUnique(T4CUnitSpriteFrameName(base, view, 0, kT4CNpcWalkAnimFrames - 1));
    }
}

void T4CAppendPuppetSpritePreload(const char *base, std::vector<std::string> *queue) {
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
    for (const int angle : {0, 45, 90, 135, 180}) {
        T4CUnitSpriteView view{};
        view.angleDegrees = angle;
        for (int frame = 0; frame < kT4CPuppetWalkAnimFrames; ++frame) {
            appendUnique(T4CUnitSpriteFrameName(base, view, frame, kT4CPuppetWalkAnimFrames - 1));
        }
    }
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
    /* Joueur classe (Warrio…) : marche 9 frames par angle. StMov/corpse/nom nu absents du .did. */
    for (const int angle : {0, 45, 90, 135, 180}) {
        T4CUnitSpriteView view{};
        view.angleDegrees = angle;
        for (int frame = 0; frame < kT4CWalkAnimFrames; ++frame) {
            appendUnique(T4CUnitSpriteFrameName(base, view, frame));
        }
    }
}
