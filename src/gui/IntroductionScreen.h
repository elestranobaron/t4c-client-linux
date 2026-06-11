#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <vector>

class LauncherChrome;

/** Ecran defilant TFC_INTRODUCTION — recit de l'Haruspice (aligne French.elng 45-47, 92-93). */
class IntroductionScreen {
   public:
    static constexpr int kLogicalWidth = 800;
    static constexpr int kLogicalHeight = 600;

    bool isOpen() const { return open_; }
    void open();
    void close();

    /** false = fermer l'introduction. */
    bool HandleEvent(const SDL_Event &event);

    void Update(float deltaSeconds);

    void Render(SDL_Renderer *renderer, LauncherChrome *chrome);

   private:
    void rebuildWrappedLines(LauncherChrome *chrome);
    void drawUiText(SDL_Renderer *renderer, LauncherChrome *chrome, const char *text, float x, float y,
                    SDL_Color color) const;

    bool open_{false};
    float scrollY_{0.0f};
    float contentHeight_{0.0f};
    std::vector<std::string> wrappedLines_;
    bool linesDirty_{true};
};
