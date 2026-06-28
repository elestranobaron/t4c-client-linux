#include "gui/T4CScrollingBanner.h"

#include "gui/T4CUiFont.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <thread>

T4CScrollingBanner::T4CScrollingBanner() = default;

T4CScrollingBanner::~T4CScrollingBanner() {
    stopTicker();
}

void T4CScrollingBanner::setText(std::string text) {
    text_ = std::move(text);
}

void T4CScrollingBanner::setBandRect(const float x, const float y, const float w, const float h) {
    band_ = SDL_FRect{x, y, w, h};
}

void T4CScrollingBanner::setSpeed(const float pixelsPerSecond) {
    speed_.store(pixelsPerSecond);
}

void T4CScrollingBanner::resetScroll() {
    scrollPx_.store(0.f);
}

void T4CScrollingBanner::startTicker() {
    if (tickerRunning_.load()) {
        return;
    }
    tickerRunning_.store(true);
    ticker_ = std::thread([this] { tickerLoop(); });
}

void T4CScrollingBanner::stopTicker() {
    if (!tickerRunning_.exchange(false)) {
        return;
    }
    if (ticker_.joinable()) {
        ticker_.join();
    }
}

void T4CScrollingBanner::tickerLoop() {
    using clock = std::chrono::steady_clock;
    auto last = clock::now();
    while (tickerRunning_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        const auto now = clock::now();
        const float dt = std::chrono::duration<float>(now - last).count();
        last = now;
        scrollPx_.store(scrollPx_.load() + speed_.load() * dt);
    }
}

void T4CScrollingBanner::update(const float /*deltaSeconds*/) {
    /* Defilement pilote par tickerLoop() — update() conserve pour compat LauncherChrome. */
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

    const float scrollX = scrollPx_.load();
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
