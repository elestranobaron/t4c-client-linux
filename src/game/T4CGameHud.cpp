#include "game/T4CGameHud.h"

#include <algorithm>
#include <cstdio>
#include <vector>

namespace {

// Sprites V3 (résolus depuis l'atlas V2 par identifiant string, comme le client Windows).
constexpr const char *kLifeBack = "V3_LifeBackF";
constexpr const char *kHpBar = "V3_PVBar";
constexpr const char *kMpBar = "V3_PMBar";
constexpr const char *kMainBack = "V3_MainBarBack";
constexpr const char *kXpBar = "V3_MainXPBar";
constexpr const char *kTmiBack = "V3_TMIBack";
constexpr const char *kPlayerPos = "V3_PlayerPos";

int clampPercent(const int value) { return std::clamp(value, 0, 100); }

constexpr int kRadarPx = 96;     // zone carte V3_TMIDlg (26,27)-(122,123)
constexpr int kRadarRange = 48;  // ±48 tuiles autour du joueur (1 px = 1 tuile)

}  // namespace

SDL_FRect T4CGameHud::homeButtonRect(const int screenW, const int screenH) {
    const float ox = static_cast<float>((screenW - kMainBarW) / 2);
    const float oy = static_cast<float>(screenH - kMainBarH);
    return SDL_FRect{ox + 82.f, oy + 44.f, 28.f, 28.f};
}

SDL_FRect T4CGameHud::exitButtonRect(const int screenW, const int screenH) {
    const float ox = static_cast<float>((screenW - kMainBarW) / 2);
    const float oy = static_cast<float>(screenH - kMainBarH);
    return SDL_FRect{ox + 590.f, oy + 44.f, 28.f, 28.f};
}

SDL_FRect T4CGameHud::macroSlotRect(const int screenW, const int screenH, const int slot) {
    const float ox = static_cast<float>((screenW - kMainBarW) / 2);
    const float oy = static_cast<float>(screenH - kMainBarH);
    return SDL_FRect{ox + 134.f + static_cast<float>(slot) * 36.f, oy + 40.f, 34.f, 32.f};
}

SDL_FRect T4CGameHud::chatInputRect(const int screenW, const int screenH) {
    const float ox = static_cast<float>((screenW - kMainBarW) / 2);
    const float oy = static_cast<float>(screenH - kMainBarH);
    return SDL_FRect{ox + 110.f, oy + 19.f, 400.f, 17.f};
}

void T4CGameHud::shutdown() {
    if (radarTexture_) {
        SDL_DestroyTexture(radarTexture_);
        radarTexture_ = nullptr;
    }
    radarValid_ = false;
    radarWorld_ = 0xFFFF;
}

void T4CGameHud::preloadSprites(T4CV2SpriteAtlas *atlas) const {
    if (!atlas) {
        return;
    }
    const std::vector<std::string> names = {
        kLifeBack, kHpBar, kMpBar, kMainBack, kXpBar, kTmiBack, kPlayerPos,
        kDefaultItemMacros[0].iconSprite, kDefaultItemMacros[1].iconSprite, kDefaultItemMacros[2].iconSprite,
    };
    atlas->PreloadBanksForNames(names);
}

void T4CGameHud::drawText(const T4CUiFont *font, SDL_Renderer *renderer, const char *text, const float x,
                          const float y, const SDL_Color color) {
    if (font && font->isReady() && text && text[0] != '\0') {
        font->drawText(renderer, text, x, y, color);
    }
}

void T4CGameHud::drawFrameOrFill(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const char *spriteName,
                                 const float x, const float y, const float w, const float h,
                                 const SDL_Color fallback) {
    if (atlas.TryDrawSpriteByName(renderer, spriteName, x, y)) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, fallback.r, fallback.g, fallback.b, fallback.a);
    SDL_FRect frame{x, y, w, h};
    SDL_RenderFillRect(renderer, &frame);
    // liseré clair pour lisibilité
    SDL_SetRenderDrawColor(renderer, 90, 84, 64, 255);
    SDL_RenderRect(renderer, &frame);
}

void T4CGameHud::drawBar(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const char *spriteName,
                         const float x, const float y, const float w, const float h, const int percent,
                         const SDL_Color fallback) {
    const int pct = clampPercent(percent);
    const float fillW = w * static_cast<float>(pct) / 100.f;
    // Clip à gauche sur la largeur remplie (le client Windows clippe la partie vide à droite).
    const SDL_Rect clip{static_cast<int>(x), static_cast<int>(y), static_cast<int>(fillW),
                        static_cast<int>(h)};
    SDL_SetRenderClipRect(renderer, &clip);
    if (!atlas.TryDrawSpriteByName(renderer, spriteName, x, y)) {
        SDL_SetRenderDrawColor(renderer, fallback.r, fallback.g, fallback.b, fallback.a);
        SDL_FRect fill{x, y, fillW, h};
        SDL_RenderFillRect(renderer, &fill);
    }
    SDL_SetRenderClipRect(renderer, nullptr);
}

void T4CGameHud::renderLife(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font,
                            const float ox, const float oy, const T4CActivePlayer &player,
                            const T4CPlayerStatus &status) const {
    drawFrameOrFill(renderer, atlas, kLifeBack, ox, oy, static_cast<float>(kLifeW),
                    static_cast<float>(kLifeH), {20, 18, 28, 220});

    const SDL_Color textColor{230, 224, 200, 255};
    const SDL_Color white{255, 255, 255, 255};

    if (player.valid && !player.name.empty()) {
        drawText(font, renderer, player.name.c_str(), ox + 38.f, oy + 8.f, textColor);
    }

    const std::uint16_t level = status.valid && status.level != 0 ? status.level : player.level;
    if (level != 0) {
        char lv[16];
        std::snprintf(lv, sizeof(lv), "%u", static_cast<unsigned>(level));
        drawText(font, renderer, lv, ox + 162.f, oy + 60.f, textColor);
    }

    const unsigned int hp = status.valid ? status.hp : 0;
    const unsigned int maxHp = status.valid ? status.maxHp : 0;
    const unsigned short mana = status.valid ? status.mana : 0;
    const unsigned short maxMana = status.valid ? status.maxMana : 0;

    drawText(font, renderer, "PV", ox + 30.f, oy + 31.f, textColor);
    drawText(font, renderer, "PM", ox + 30.f, oy + 48.f, textColor);

    const int hpPct = maxHp == 0 ? 0 : static_cast<int>(hp * 100 / maxHp);
    const int mpPct = maxMana == 0 ? 0 : static_cast<int>(mana * 100 / maxMana);
    // Rectangles V3_LifeDlg : PV (46,30)-(146,43), PM (46,47)-(146,60) → 100×13.
    drawBar(renderer, atlas, kHpBar, ox + 46.f, oy + 30.f, 100.f, 13.f, hpPct, {180, 40, 40, 255});
    drawBar(renderer, atlas, kMpBar, ox + 46.f, oy + 47.f, 100.f, 13.f, mpPct, {52, 84, 200, 255});

    if (status.valid && maxHp > 0) {
        char nums[48];
        std::snprintf(nums, sizeof(nums), "%u/%u", hp, maxHp);
        drawText(font, renderer, nums, ox + 60.f, oy + 30.f, white);
    }
    if (status.valid && maxMana > 0) {
        char nums[48];
        std::snprintf(nums, sizeof(nums), "%u/%u", static_cast<unsigned>(mana),
                      static_cast<unsigned>(maxMana));
        drawText(font, renderer, nums, ox + 60.f, oy + 47.f, white);
    }

    if (T4CLoginSessionGetAttackMode()) {
        drawFrameOrFill(renderer, atlas, "StaticAttackCursor", ox + 162.f, oy + 54.f, 14.f, 19.f,
                        {80, 20, 20, 255});
    }
}

void T4CGameHud::renderMainBar(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font,
                               const float ox, const float oy, const T4CPlayerStatus &status) const {
    drawFrameOrFill(renderer, atlas, kMainBack, ox, oy, static_cast<float>(kMainBarW),
                    static_cast<float>(kMainBarH), {26, 22, 18, 235});

    // Barre d'XP — V3_MainBarDlg (79,75)-(622,82) : 543×7.
    int xpPct = 0;
    if (status.valid && status.xpToNextLevel > 0 && status.xp <= status.xpToNextLevel) {
        xpPct = static_cast<int>(status.xp * 100 / status.xpToNextLevel);
    }
    drawBar(renderer, atlas, kXpBar, ox + 79.f, oy + 75.f, 543.f, 7.f, xpPct, {200, 170, 40, 255});
    {
        char pct[16];
        std::snprintf(pct, sizeof(pct), "%d%%", xpPct);
        drawText(font, renderer, pct, ox + 330.f, oy + 73.f, {255, 255, 255, 255});
    }

    // Boutons Home / Exit (zones V3_MainBarDlg) — Home = fiche perso, Exit = menu.
    const SDL_Color btn{60, 52, 40, 255};
    drawFrameOrFill(renderer, atlas, "V3_MainBtnHome_N", ox + 82.f, oy + 44.f, 28.f, 28.f, btn);
    drawFrameOrFill(renderer, atlas, "V3_MainBtnExit_N", ox + 590.f, oy + 44.f, 28.f, 28.f, btn);

    // Or + poids transportes (Windows V3_InvDlg/V3_StatsDlg, maj opcodes 53/92).
    if (status.valid) {
        char buf[48];
        std::snprintf(buf, sizeof(buf), "Or %u", status.gold);
        drawText(font, renderer, buf, ox + 8.f, oy + 8.f, {255, 200, 70, 255});
        std::snprintf(buf, sizeof(buf), "Pds %u/%u", status.weight, status.maxWeight);
        drawText(font, renderer, buf, ox + 8.f, oy + 24.f, {210, 205, 185, 255});
    }

    // 12 slots macro — y=40, x=134+col*36, 34×32 (V3_MainBarDlg).
    const SDL_Color slot{40, 36, 30, 220};
    for (int col = 0; col < 12; ++col) {
        const float sx = ox + 134.f + static_cast<float>(col) * 36.f;
        SDL_FRect cell{sx, oy + 40.f, 34.f, 32.f};
        SDL_SetRenderDrawColor(renderer, slot.r, slot.g, slot.b, slot.a);
        SDL_RenderFillRect(renderer, &cell);
        SDL_SetRenderDrawColor(renderer, 80, 72, 56, 255);
        SDL_RenderRect(renderer, &cell);
        for (int m = 0; m < kDefaultItemMacroCount; ++m) {
            if (kDefaultItemMacros[m].slot != col) {
                continue;
            }
            const float iconX = sx + 17.f;
            const float iconY = oy + 56.f;
            if (!atlas.TryDrawSpriteByName(renderer, kDefaultItemMacros[m].iconSprite, iconX, iconY)) {
                drawText(font, renderer, "?", sx + 12.f, oy + 48.f, {255, 255, 255, 255});
            }
            break;
        }
    }
}

void T4CGameHud::renderTmi(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font,
                           const float ox, const float oy, const T4CActivePlayer &player,
                           const unsigned int world, const T4CTmiWorldMap *tmiMap) {
    drawFrameOrFill(renderer, atlas, kTmiBack, ox, oy, static_cast<float>(kTmiW),
                    static_cast<float>(kTmiH), {18, 20, 16, 230});

    // Zone carte (26,27)-(122,123) : 96×96 — radar terrain TMI (1 px = 1 tuile, V3_TMIDlg DrawTMI).
    SDL_FRect mapArea{ox + 26.f, oy + 27.f, 96.f, 96.f};
    SDL_SetRenderDrawColor(renderer, 10, 14, 10, 255);
    SDL_RenderFillRect(renderer, &mapArea);

    const bool haveMap = tmiMap != nullptr && tmiMap->IsLoaded() && player.valid;
    if (haveMap) {
        const bool dirty = !radarValid_ || radarCenterX_ != player.serverX ||
                           radarCenterY_ != player.serverY ||
                           radarWorld_ != static_cast<unsigned short>(world);
        if (!radarTexture_) {
            radarTexture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                              SDL_TEXTUREACCESS_STREAMING, kRadarPx, kRadarPx);
        }
        if (radarTexture_ && dirty) {
            static std::uint8_t pixels[kRadarPx * kRadarPx * 4];
            for (int py = 0; py < kRadarPx; ++py) {
                for (int px = 0; px < kRadarPx; ++px) {
                    const long wx = static_cast<long>(player.serverX) - kRadarRange + px;
                    const long wy = static_cast<long>(player.serverY) - kRadarRange + py;
                    std::uint8_t r = 8;
                    std::uint8_t g = 10;
                    std::uint8_t b = 8;
                    if (wx >= 0 && wy >= 0) {
                        tmiMap->SampleRgb(static_cast<unsigned>(wx), static_cast<unsigned>(wy), r, g, b);
                    }
                    std::uint8_t *out = pixels + (py * kRadarPx + px) * 4;
                    out[0] = r;
                    out[1] = g;
                    out[2] = b;
                    out[3] = 255;
                }
            }
            SDL_UpdateTexture(radarTexture_, nullptr, pixels, kRadarPx * 4);
            radarCenterX_ = player.serverX;
            radarCenterY_ = player.serverY;
            radarWorld_ = static_cast<unsigned short>(world);
            radarValid_ = true;
        }
        if (radarTexture_ && radarValid_) {
            SDL_RenderTexture(renderer, radarTexture_, nullptr, &mapArea);
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 28, 40, 28, 120);
        for (int i = 1; i < 4; ++i) {
            const float gx = mapArea.x + 24.f * static_cast<float>(i);
            const float gy = mapArea.y + 24.f * static_cast<float>(i);
            SDL_RenderLine(renderer, gx, mapArea.y, gx, mapArea.y + 96.f);
            SDL_RenderLine(renderer, mapArea.x, gy, mapArea.x + 96.f, gy);
        }
    }

    // Point joueur centré (72,74)-(76,78).
    if (!atlas.TryDrawSpriteByName(renderer, kPlayerPos, ox + 70.f, oy + 72.f)) {
        SDL_SetRenderDrawColor(renderer, 255, 240, 90, 255);
        SDL_FRect dot{ox + 71.f, oy + 73.f, 5.f, 5.f};
        SDL_RenderFillRect(renderer, &dot);
    }

    // Coords « x,y,monde » sous la carte (33,131).
    if (player.valid) {
        char coords[48];
        std::snprintf(coords, sizeof(coords), "%u,%u,%u", player.serverX, player.serverY, world);
        drawText(font, renderer, coords, ox + 33.f, oy + 129.f, {210, 220, 210, 255});
    }
}

void T4CGameHud::render(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font,
                        const int screenW, const int screenH, const T4CActivePlayer &player,
                        const T4CPlayerStatus &status, const unsigned int world,
                        const T4CTmiWorldMap *tmiMap) {
    if (!renderer) {
        return;
    }

    // Positions par défaut du client Windows (Global.cpp).
    const float lifeX = static_cast<float>(screenW - kLifeW);  // haut-droite (W-216, 0)
    const float lifeY = 0.f;
    const float mainX = static_cast<float>((screenW - kMainBarW) / 2);  // bas-centre
    const float mainY = static_cast<float>(screenH - kMainBarH);
    const float tmiX = static_cast<float>(screenW - kTmiW);  // bas-droite
    const float tmiY = static_cast<float>(screenH - kTmiH - kMainBarH);

    renderMainBar(renderer, atlas, font, mainX, mainY, status);
    renderLife(renderer, atlas, font, lifeX, lifeY, player, status);
    renderTmi(renderer, atlas, font, tmiX, tmiY, player, world, tmiMap);
}
