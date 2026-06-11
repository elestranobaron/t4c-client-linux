#pragma once

#include <SDL3/SDL.h>

#include <string>

class T4CUiFont;

/** Texte horizontal defilant (style credits launcher Windows). */
class T4CScrollingBanner {
   public:
    void setText(std::string text);
    void setBandRect(float x, float y, float w, float h);
    void setSpeed(float pixelsPerSecond);

    /** Repart de zero (ex. re-init launcher). */
    void resetScroll();

    void update(float deltaSeconds);
    void render(SDL_Renderer *renderer, const T4CUiFont &font, SDL_Color color) const;

   private:
    std::string text_;
    SDL_FRect band_{0.f, 540.f, 800.f, 36.f};
    float speed_{48.f};
    Uint64 scrollStartMs_{0};
};
