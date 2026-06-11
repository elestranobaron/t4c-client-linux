#pragma once

#include <SDL3/SDL.h>

#include <string>

#include "gui/T4CScrollingBanner.h"
#include "gui/T4CUiFont.h"

/** Fond LoadingScreen + police Beaulieux + bandeau defilant (login / selection perso). */
class LauncherChrome {
   public:
    bool init(SDL_Renderer *renderer);
    void shutdown();

    void update();
    void renderBackground(SDL_Renderer *renderer) const;
    void renderBanner(SDL_Renderer *renderer);

    /** Bande defilante en bas (800x600 login vs 1280x720 chargement monde). */
    void layoutBannerForLogicalSize(float logicalWidth, float logicalHeight);

    T4CUiFont &font() { return font_; }
    const T4CUiFont &font() const { return font_; }

   private:
    SDL_Renderer *renderer_{nullptr};
    SDL_Texture *background_{nullptr};
    T4CUiFont font_;
    T4CUiFont bannerFont_;
    T4CScrollingBanner banner_;
    Uint64 lastTickMs_{0};
};
