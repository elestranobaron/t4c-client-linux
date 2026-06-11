#pragma once

#include <SDL3/SDL.h>

#include "gui/T4CCharacterQuestionnaire.h"
#include "network/T4CLoginSession.h"

#include <string>

class LauncherChrome;

enum class CharacterCreateStep { Name, Sex, Questionnaire, Reroll };

/** Creation de personnage : nom, sexe, questionnaire (4 questions), opcode 25. */
class CharacterCreateScreen {
   public:
    static constexpr int kLogicalWidth = 800;
    static constexpr int kLogicalHeight = 600;

    explicit CharacterCreateScreen(SDL_Renderer *renderer, LauncherChrome *chrome);

    /** false = retour selection persos (Esc depuis l'etape nom). */
    bool HandleEvent(const SDL_Event &event, SDL_Window *window);

    void Update();

    void Render(SDL_Renderer *renderer);

    bool ShouldStay() const { return stay_; }

    const std::string &GetStatusLine() const { return statusLine_; }

    /** Reinitialise le flux (depuis la selection perso). */
    void resetFlow();

   private:
    void tryAdvance(SDL_Window *window);
    void tryCreate(SDL_Window *window);
    void applyBackspace();
    void appendTextInput(const char *utf8Text);
    void drawUiText(SDL_Renderer *renderer, const char *text, float x, float y, SDL_Color color) const;
    void renderNameStep(SDL_Renderer *renderer, SDL_Color textMain, SDL_Color textMuted, SDL_Color textField);
    void renderSexStep(SDL_Renderer *renderer, SDL_Color textMain, SDL_Color textMuted);
    void renderQuestionnaireStep(SDL_Renderer *renderer, SDL_Color textMain, SDL_Color textMuted, SDL_Color textSel);
    void renderRerollStep(SDL_Renderer *renderer, SDL_Color textMain, SDL_Color textMuted, SDL_Color textValue);

    SDL_Renderer *renderer_{nullptr};
    LauncherChrome *chrome_{nullptr};
    bool stay_{true};
    CharacterCreateStep step_{CharacterCreateStep::Name};
    T4CCharacterQuestionnaire questionnaire_;
    std::string nameStr_;
    bool male_{true};
    std::string statusLine_;
    bool statusLocked_{false};
    T4CCharacterRolledStats rolledStats_{};
    bool waitingReroll_{false};

    static constexpr size_t kMaxNameLen = 20;
};
