#include "gui/LauncherChrome.h"

#include "runtime/T4CRuntimePaths.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace {

constexpr const char *kBannerText =
    "  The 4th Coming  —  markshptang  —  Bienvenue, aventurier  —  "
    "Selectionnez votre personnage ou creez-en un  —  ";

bool LoadBackgroundTexture(SDL_Renderer *renderer, SDL_Texture **outTex) {
    const std::string path = T4CRuntimePath("Images/start800600.pcx");
    if (path.empty() || !fs::is_regular_file(path)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[LauncherChrome] LoadingScreen.bmp introuvable.");
        return false;
    }
    SDL_Surface *surf = SDL_LoadBMP(path.c_str());
    if (!surf) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[LauncherChrome] SDL_LoadBMP: %s", SDL_GetError());
        return false;
    }
    *outTex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    if (!*outTex) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[LauncherChrome] texture fond: %s", SDL_GetError());
        return false;
    }
    SDL_SetTextureScaleMode(*outTex, SDL_SCALEMODE_LINEAR);
    return true;
}

}  // namespace

bool LauncherChrome::init(SDL_Renderer *renderer) {
    shutdown();
    renderer_ = renderer;

    if (!TTF_Init()) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[LauncherChrome] TTF_Init: %s", SDL_GetError());
    }

    font_.init(18.f);
    bannerFont_.init(15.f);

    LoadBackgroundTexture(renderer, &background_);

    banner_.setText(kBannerText);
    layoutBannerForLogicalSize(800.f, 600.f);
    banner_.setSpeed(42.f);
    banner_.resetScroll();

    lastTickMs_ = SDL_GetTicks();
    return true;
}

void LauncherChrome::shutdown() {
    if (background_) {
        SDL_DestroyTexture(background_);
        background_ = nullptr;
    }
    bannerFont_.shutdown();
    font_.shutdown();
    renderer_ = nullptr;
    TTF_Quit();
}

void LauncherChrome::update() {
    const Uint64 now = SDL_GetTicks();
    const float dt = static_cast<float>(now - lastTickMs_) * 0.001f;
    lastTickMs_ = now;
    banner_.update(dt);
}

void LauncherChrome::renderBackground(SDL_Renderer *renderer) const {
    if (background_) {
        SDL_FRect dst{0.f, 0.f, 800.f, 600.f};
        SDL_RenderTexture(renderer, background_, nullptr, &dst);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 8, 10, 18, 120);
        SDL_RenderFillRect(renderer, &dst);
        return;
    }
    SDL_SetRenderDrawColor(renderer, 18, 20, 26, 255);
    SDL_RenderClear(renderer);
}

void LauncherChrome::renderBanner(SDL_Renderer *renderer) {
    const SDL_Color gold{230, 200, 120, 255};
    banner_.render(renderer, bannerFont_.isReady() ? bannerFont_ : font_, gold);
    SDL_SetRenderClipRect(renderer, nullptr);
    SDL_SetRenderColorScale(renderer, 1.f);
}

void LauncherChrome::layoutBannerForLogicalSize(const float logicalWidth, const float logicalHeight) {
    constexpr float kBandH = 40.f;
    banner_.setBandRect(0.f, logicalHeight - kBandH - 8.f, logicalWidth, kBandH);
}
