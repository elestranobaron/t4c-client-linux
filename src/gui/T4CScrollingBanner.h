#pragma once

#include <SDL3/SDL.h>

#include <atomic>
#include <string>
#include <thread>

class T4CUiFont;

/** Texte horizontal defilant (style credits launcher Windows). */
class T4CScrollingBanner {
   public:
    T4CScrollingBanner();
    ~T4CScrollingBanner();

    T4CScrollingBanner(const T4CScrollingBanner &) = delete;
    T4CScrollingBanner &operator=(const T4CScrollingBanner &) = delete;

    void setText(std::string text);
    void setBandRect(float x, float y, float w, float h);
    void setSpeed(float pixelsPerSecond);

    /** Repart de zero (ex. re-init launcher). */
    void resetScroll();

    /** Demarre le ticker independant du thread principal (defilement fluide pendant chargement lourd). */
    void startTicker();
    void stopTicker();

    void update(float deltaSeconds);
    void render(SDL_Renderer *renderer, const T4CUiFont &font, SDL_Color color) const;

   private:
    void tickerLoop();

    std::string text_;
    SDL_FRect band_{0.f, 540.f, 800.f, 36.f};
    std::atomic<float> speed_{48.f};
    std::atomic<float> scrollPx_{0.f};
    std::atomic<bool> tickerRunning_{false};
    std::thread ticker_;
};
