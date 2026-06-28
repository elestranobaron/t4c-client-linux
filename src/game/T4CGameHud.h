#pragma once

#include "assets/map/T4CTmiWorldMap.h"
#include "assets/map/T4CV2SpriteAtlas.h"
#include "gui/T4CUiFont.h"
#include "network/T4CLoginSession.h"

#include <SDL3/SDL.h>

/**
 * HUD in-game complet, calqué sur le client Windows (NewInterface/V3_*Dlg).
 * Trois overlays persistants, positionnés comme à l'origine :
 *  - Life (216×74)   : haut-droite  — V3_LifeDlg  (cadre + barres PV/PM + nom + niveau + portrait)
 *  - MainBar (700×85): bas-centre   — V3_MainBarDlg (fond + barre XP + 12 slots macro + boutons)
 *  - TMI (148×152)   : bas-droite   — V3_TMIDlg   (radar + position + coords)
 * Coordonnées internes = relatives à l'origine du panneau (cf. dialogs Windows).
 */
class T4CGameHud {
   public:
    static constexpr int kLifeW = 216;
    static constexpr int kLifeH = 74;
    static constexpr int kMainBarW = 700;
    static constexpr int kMainBarH = 85;
    static constexpr int kTmiW = 148;
    static constexpr int kTmiH = 152;

    void preloadSprites(T4CV2SpriteAtlas *atlas) const;

    void render(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font, int screenW,
                int screenH, const T4CActivePlayer &player, const T4CPlayerStatus &status,
                unsigned int world, const T4CTmiWorldMap *tmiMap = nullptr);

    void shutdown();

    /** Macros objet par defaut (V3_InvDlg::SetDefaultMacro — F2/F3/F4). */
    struct DefaultItemMacro {
        int slot;
        std::uint16_t baseId;
        const char *iconSprite;
    };
    static constexpr DefaultItemMacro kDefaultItemMacros[] = {
        {1, 40623, "64kInvPotion 2"},
        {2, 40004, "64kInvPotion 1"},
        {3, 40015, "64kInvTorch"},
    };
    static constexpr int kDefaultItemMacroCount = 3;

    /** Zones cliquables MainBar (V3_MainBarDlg Home/Exit + 12 slots macro). */
    static SDL_FRect homeButtonRect(int screenW, int screenH);
    static SDL_FRect exitButtonRect(int screenW, int screenH);
    static SDL_FRect macroSlotRect(int screenW, int screenH, int slot);
    /** Zone saisie chat (m_EditInput 110,19)-(510,36) relative MainBar. */
    static SDL_FRect chatInputRect(int screenW, int screenH);

   private:
    static void drawBar(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const char *spriteName,
                        float x, float y, float w, float h, int percent, SDL_Color fallback);
    static void drawFrameOrFill(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const char *spriteName,
                                float x, float y, float w, float h, SDL_Color fallback);
    static void drawText(const T4CUiFont *font, SDL_Renderer *renderer, const char *text, float x, float y,
                         SDL_Color color);

    void renderLife(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font,
                    float originX, float originY, const T4CActivePlayer &player,
                    const T4CPlayerStatus &status) const;
    void renderMainBar(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font,
                       float originX, float originY, const T4CPlayerStatus &status) const;
    void renderTmi(SDL_Renderer *renderer, const T4CV2SpriteAtlas &atlas, const T4CUiFont *font,
                   float originX, float originY, const T4CActivePlayer &player, unsigned int world,
                   const T4CTmiWorldMap *tmiMap);

    /** Radar TMI : texture 96×96 reconstruite quand la tuile joueur change. */
    SDL_Texture *radarTexture_{nullptr};
    unsigned int radarCenterX_{0};
    unsigned int radarCenterY_{0};
    unsigned short radarWorld_{0xFFFF};
    bool radarValid_{false};
};
