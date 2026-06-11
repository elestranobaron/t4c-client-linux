#pragma once

#include <SDL3/SDL.h>

#include "gui/IntroductionScreen.h"

#include <atomic>
#include <string>
#include <vector>

class LauncherChrome;

/** Ecran liste persos (apres auth, avant opcode 13). */
class CharacterSelectScreen {
   public:
    static constexpr int kLogicalWidth = 800;
    static constexpr int kLogicalHeight = 600;

    explicit CharacterSelectScreen(SDL_Renderer *renderer, LauncherChrome *chrome);

    bool HandleEvent(const SDL_Event &event, SDL_Window *window);

    void Update();

    void Render(SDL_Renderer *renderer);

    /** false = retour login (Esc). */
    bool ShouldStay() const { return stay_; }

    /** true une fois : ouvrir ecran creation (touche C). */
    bool ConsumeWantCreateCharacter() { return wantCreate_.exchange(false); }

    void resetFlow();

    const std::string &GetStatusLine() const { return statusLine_; }

   private:
    void refreshFromSession();
    void tryEnterWorld(SDL_Window *window);
    void tryDeleteSelected(SDL_Window *window);
    void drawUiText(SDL_Renderer *renderer, const char *text, float x, float y, SDL_Color color) const;
    void renderActionFooter(SDL_Renderer *renderer, SDL_Color textMuted) const;

    SDL_Renderer *renderer_{nullptr};
    LauncherChrome *chrome_{nullptr};
    IntroductionScreen introduction_;
    Uint64 lastTickMs_{0};
    bool stay_{true};
    std::atomic<bool> wantCreate_{false};
    bool confirmDelete_{false};
    std::string pendingDeleteName_;
    int selectedIndex_{0};
    Uint64 lastCharClickMs_{0};
    int lastCharClickRow_{-1};
    std::vector<std::string> displayLines_;
    std::string statusLine_;
    bool statusLocked_{false};
};
