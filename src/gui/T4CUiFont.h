#pragma once

#include <SDL3/SDL.h>

struct TTF_Font;

/** Police launcher T4C (SDL3_ttf, t4cbeaulieux.ttf sous T4C_DATA/fonts/). */
class T4CUiFont {
   public:
    T4CUiFont() = default;
    ~T4CUiFont();

    T4CUiFont(const T4CUiFont &) = delete;
    T4CUiFont &operator=(const T4CUiFont &) = delete;

    bool init(float pointSize = 18.f);
    void shutdown();

    bool isReady() const { return font_ != nullptr; }

    void drawText(SDL_Renderer *renderer, const char *text, float x, float y, SDL_Color color) const;

    /** Surface RGBA32 — le caller doit SDL_DestroySurface. */
    SDL_Surface *renderTextSurface(const char *text, SDL_Color color) const;

    void blitText(SDL_Surface *dest, int x, int y, const char *text, SDL_Color color) const;

    void measureText(const char *text, int *outW, int *outH) const;

    TTF_Font *handle() const { return font_; }

   private:
    TTF_Font *font_{nullptr};
    float pointSize_{18.f};
};
