#include "gui/T4CScrollingBanner.h"

#include "gui/T4CUiFont.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>
#include <string>

void T4CScrollingBanner::setText(std::string text) {
    text_ = std::move(text);
}

void T4CScrollingBanner::setBandRect(const float x, const float y, const float w, const float h) {
    band_ = SDL_FRect{x, y, w, h};
}

void T4CScrollingBanner::setSpeed(const float pixelsPerSecond) {
    speed_ = pixelsPerSecond;
}

void T4CScrollingBanner::resetScroll() {
    scrollStartMs_ = SDL_GetTicks();
}

void T4CScrollingBanner::update(const float /*deltaSeconds*/) {
    if (scrollStartMs_ == 0) {
        scrollStartMs_ = SDL_GetTicks();
    }
}

void T4CScrollingBanner::render(SDL_Renderer *renderer, const T4CUiFont &font, const SDL_Color color) const {
    if (!renderer || text_.empty() || !font.isReady()) {
        return;
    }

    int tw = 0;
    int th = 0;
    font.measureText(text_.c_str(), &tw, &th);
    if (tw <= 0) {
        return;
    }

    Uint64 startMs = scrollStartMs_;
    if (startMs == 0) {
        startMs = SDL_GetTicks();
    }
    const float scrollX = speed_ * static_cast<float>(SDL_GetTicks() - startMs) * 0.001f;
    const float loop = static_cast<float>(tw) + 80.f;
    float x = band_.x - std::fmod(scrollX, loop);
    const float y = band_.y + (band_.h - static_cast<float>(th)) * 0.5f;

    SDL_Rect clip{static_cast<int>(band_.x), static_cast<int>(band_.y), static_cast<int>(band_.w),
                  static_cast<int>(band_.h)};
    SDL_SetRenderClipRect(renderer, &clip);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 140);
    SDL_RenderFillRect(renderer, &band_);
    SDL_SetRenderDrawColor(renderer, 90, 70, 40, 200);
    SDL_RenderRect(renderer, &band_);

    for (int pass = 0; pass < 3; ++pass) {
        font.drawText(renderer, text_.c_str(), x + static_cast<float>(pass) * loop, y, color);
    }
    SDL_SetRenderClipRect(renderer, nullptr);
}
