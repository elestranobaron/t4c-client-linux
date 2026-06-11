#include "gui/CharacterCreateScreen.h"

#include "gui/LauncherChrome.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>

#if defined(LINUX_PORT)
#include "network/T4CLoginSession.h"
#endif

namespace {

constexpr float kPaddingX = 48.0f;
constexpr float kFieldX = 240.0f;
constexpr float kFieldW = 480.0f;
constexpr float kFieldH = 36.0f;
constexpr float kNameY = 160.0f;
constexpr float kSexY = 240.0f;
constexpr float kQuestionY = 130.0f;
constexpr float kAnswerStartY = 200.0f;
constexpr float kAnswerRowH = 36.0f;
constexpr float kRerollTitleY = 120.0f;
constexpr float kRerollStatsY = 170.0f;
constexpr float kRerollRowH = 28.0f;

bool KeyDown(const SDL_Event &event, SDL_Scancode sc) {
    return event.type == SDL_EVENT_KEY_DOWN && event.key.down && !event.key.repeat &&
           event.key.scancode == sc;
}

void AppendAsciiPrintable(std::string &dst, const char *text) {
    if (!text) {
        return;
    }
    for (const unsigned char *p = reinterpret_cast<const unsigned char *>(text); *p; ++p) {
        const unsigned char c = *p;
        if (c >= 32 && c <= 126) {
            dst.push_back(static_cast<char>(c));
        }
    }
}

bool IsValidCharacterName(const std::string &name) {
    constexpr size_t kMaxNameLen = 20;
    if (name.size() < 3 || name.size() > kMaxNameLen) {
        return false;
    }
    for (unsigned char c : name) {
        if (!std::isalnum(c)) {
            return false;
        }
    }
    return true;
}

}  // namespace

CharacterCreateScreen::CharacterCreateScreen(SDL_Renderer *renderer, LauncherChrome *chrome)
    : renderer_(renderer), chrome_(chrome) {
    resetFlow();
}

void CharacterCreateScreen::resetFlow() {
#if defined(LINUX_PORT)
    T4CLoginSessionPrepareForCreateScreen();
#endif
    stay_ = true;
    step_ = CharacterCreateStep::Name;
    nameStr_.clear();
    male_ = true;
    statusLocked_ = false;
    waitingReroll_ = false;
    rolledStats_ = {};
    questionnaire_.reset();
    statusLine_ = "Nom (3-20 alphanum) — Entree : suivant — Esc : retour";
}

void CharacterCreateScreen::drawUiText(SDL_Renderer *renderer, const char *text, const float x,
                                       const float y, const SDL_Color color) const {
    if (chrome_ && chrome_->font().isReady()) {
        chrome_->font().drawText(renderer, text, x, y, color);
        return;
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDebugText(renderer, x, y, text);
}

void CharacterCreateScreen::appendTextInput(const char *utf8Text) {
    if (step_ != CharacterCreateStep::Name) {
        return;
    }
    std::string chunk;
    AppendAsciiPrintable(chunk, utf8Text);
    if (chunk.empty()) {
        return;
    }
    std::string filtered;
    for (char c : chunk) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            filtered.push_back(c);
        }
    }
    if (filtered.empty()) {
        return;
    }
    if (nameStr_.size() + filtered.size() > kMaxNameLen) {
        filtered.resize(kMaxNameLen - nameStr_.size());
    }
    nameStr_.append(filtered);
}

void CharacterCreateScreen::applyBackspace() {
    if (step_ == CharacterCreateStep::Name && !nameStr_.empty()) {
        nameStr_.pop_back();
    }
}

void CharacterCreateScreen::tryAdvance(SDL_Window *window) {
    if (step_ == CharacterCreateStep::Name) {
        if (!IsValidCharacterName(nameStr_)) {
            statusLocked_ = true;
            statusLine_ = "Nom invalide (3-20 caracteres alphanumeriques).";
            return;
        }
        step_ = CharacterCreateStep::Sex;
        statusLocked_ = false;
        statusLine_ = "Tab : sexe — Entree : questionnaire — Esc : nom";
        return;
    }
    if (step_ == CharacterCreateStep::Sex) {
        questionnaire_.reset();
        step_ = CharacterCreateStep::Questionnaire;
        statusLocked_ = false;
        statusLine_ = "Question 1/4 — Fleches ou 1-5 — Entree : valider — Esc : sexe";
        return;
    }
    if (step_ == CharacterCreateStep::Questionnaire) {
        if (questionnaire_.isComplete()) {
            tryCreate(window);
            return;
        }
        if (questionnaire_.confirmChoice()) {
            statusLocked_ = true;
            statusLine_ = "Questionnaire termine — creation en cours (attente opcode 25)…";
            tryCreate(window);
        } else {
            char buf[96];
            std::snprintf(buf, sizeof(buf), "Question %d/4 — Fleches ou 1-5 — Entree : valider — Esc : sexe",
                          questionnaire_.questionNumber() + 1);
            statusLine_ = buf;
        }
    }
}

void CharacterCreateScreen::tryCreate(SDL_Window * /*window*/) {
#if defined(LINUX_PORT)
    if (T4CLoginSessionIsWaitingCreatePlayer()) {
        statusLocked_ = true;
        statusLine_ = "Creation en cours (attente opcode 25)…";
        return;
    }
    T4CLoginSessionClearCreatePlayerError();
    statusLocked_ = false;
    const unsigned char sex = male_ ? 0 : 1;
    unsigned char stats[5];
    questionnaire_.fillPacketStats(stats);
    if (!T4CLoginSessionRequestCreatePlayer(nameStr_, sex, stats)) {
        statusLocked_ = true;
#if defined(LINUX_PORT)
        if (T4CLoginSessionIsInCreateRerollPhase()) {
            statusLine_ = "Creation precedente en cours — Esc (reroll) ou retour selection.";
        } else if (T4CLoginSessionIsWaitingDeletePlayer()) {
            statusLine_ = "Annulation perso precedente en cours — reessayez dans quelques secondes.";
        } else {
            statusLine_ = "Envoi RQ_CreatePlayer (25) impossible (reseau occupe).";
        }
#else
        statusLine_ = "Envoi RQ_CreatePlayer (25) impossible.";
#endif
        return;
    }
    statusLocked_ = true;
    statusLine_ = "Creation en cours (attente opcode 25)…";
    SDL_Log("[CharacterCreate] RQ_CreatePlayer « %s » (%s) stats=%u,%u,%u,%u,%u", nameStr_.c_str(),
            male_ ? "M" : "F", static_cast<unsigned>(stats[0]), static_cast<unsigned>(stats[1]),
            static_cast<unsigned>(stats[2]), static_cast<unsigned>(stats[3]),
            static_cast<unsigned>(stats[4]));
#endif
}

bool CharacterCreateScreen::HandleEvent(const SDL_Event &event, SDL_Window *window) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (KeyDown(event, SDL_SCANCODE_ESCAPE)) {
                if (step_ == CharacterCreateStep::Reroll) {
#if defined(LINUX_PORT)
                    T4CLoginSessionCancelCreateReroll();
#endif
                    SDL_Log("[CharacterCreate] Esc — annulation reroll, retour selection");
                    stay_ = false;
                    statusLocked_ = false;
                    return false;
                }
                if (step_ == CharacterCreateStep::Questionnaire) {
                    step_ = CharacterCreateStep::Sex;
                    questionnaire_.reset();
                    statusLocked_ = false;
                    statusLine_ = "Tab : sexe — Entree : questionnaire — Esc : nom";
                    return true;
                }
                if (step_ == CharacterCreateStep::Sex) {
                    step_ = CharacterCreateStep::Name;
                    statusLocked_ = false;
                    statusLine_ = "Nom (3-20 alphanum) — Entree : suivant — Esc : retour";
                    return true;
                }
                SDL_Log("[CharacterCreate] Esc — retour selection");
                stay_ = false;
                statusLocked_ = false;
                return false;
            }
            if (step_ == CharacterCreateStep::Sex && KeyDown(event, SDL_SCANCODE_TAB)) {
                male_ = !male_;
                return true;
            }
            if (step_ == CharacterCreateStep::Reroll) {
                if (KeyDown(event, SDL_SCANCODE_R)) {
#if defined(LINUX_PORT)
                    if (T4CLoginSessionRequestCreateReroll()) {
                        waitingReroll_ = true;
                        statusLocked_ = true;
                        statusLine_ = "Relance des des (opcode 31)…";
                    } else {
                        statusLocked_ = true;
                        statusLine_ = "Relance des des impossible.";
                    }
#endif
                    return true;
                }
                if (KeyDown(event, SDL_SCANCODE_RETURN) || KeyDown(event, SDL_SCANCODE_KP_ENTER)) {
#if defined(LINUX_PORT)
                    if (T4CLoginSessionConfirmCreateReroll()) {
                        statusLocked_ = true;
                        statusLine_ = "Entree en monde en cours (opcode 26 puis 13)…";
                    } else {
                        statusLocked_ = true;
                        statusLine_ = "Validation impossible.";
                    }
#endif
                    return true;
                }
            }
            if (step_ == CharacterCreateStep::Questionnaire) {
                if (KeyDown(event, SDL_SCANCODE_UP)) {
                    int choice = questionnaire_.selectedChoice() - 1;
                    if (choice < 1) {
                        choice = T4CCharacterQuestionnaire::kAnswerCount;
                    }
                    questionnaire_.setSelectedChoice(choice);
                    return true;
                }
                if (KeyDown(event, SDL_SCANCODE_DOWN)) {
                    int choice = questionnaire_.selectedChoice() + 1;
                    if (choice > T4CCharacterQuestionnaire::kAnswerCount) {
                        choice = 1;
                    }
                    questionnaire_.setSelectedChoice(choice);
                    return true;
                }
                for (int digit = 1; digit <= T4CCharacterQuestionnaire::kAnswerCount; ++digit) {
                    if (KeyDown(event, static_cast<SDL_Scancode>(SDL_SCANCODE_1 + digit - 1))) {
                        questionnaire_.setSelectedChoice(digit);
                        tryAdvance(window);
                        return true;
                    }
                }
            }
            if (KeyDown(event, SDL_SCANCODE_BACKSPACE)) {
                applyBackspace();
                statusLocked_ = false;
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_RETURN) || KeyDown(event, SDL_SCANCODE_KP_ENTER)) {
                tryAdvance(window);
                return true;
            }
            return true;
        case SDL_EVENT_TEXT_INPUT:
            appendTextInput(event.text.text);
            statusLocked_ = false;
            return true;
        default:
            return true;
    }
}

void CharacterCreateScreen::Update() {
#if defined(LINUX_PORT)
    T4CLoginSessionPollBackgroundTasks();

    T4CCharacterRolledStats freshStats{};
    if (T4CLoginSessionConsumeRolledStatsUpdate(&freshStats)) {
        rolledStats_ = freshStats;
        waitingReroll_ = false;
        step_ = CharacterCreateStep::Reroll;
        statusLocked_ = false;
        statusLine_ = "R : relancer les des — Entree : valider — Esc : annuler";
    } else if (T4CLoginSessionIsInCreateRerollPhase() && step_ == CharacterCreateStep::Questionnaire) {
        step_ = CharacterCreateStep::Reroll;
        statusLocked_ = false;
        statusLine_ = "R : relancer les des — Entree : valider — Esc : annuler";
    }

    if (T4CLoginSessionHasCreatePlayerError()) {
        statusLocked_ = true;
        statusLine_ = T4CLoginSessionGetCreatePlayerErrorMessage();
        waitingReroll_ = false;
    } else if (T4CLoginSessionHasPutPlayerInGameError()) {
        statusLocked_ = true;
        statusLine_ = T4CLoginSessionGetPutPlayerInGameErrorMessage();
    } else if (step_ == CharacterCreateStep::Reroll) {
        if (T4CLoginSessionIsWaitingPutPlayerInGame()) {
            statusLocked_ = true;
            statusLine_ = "Entree en monde en cours (opcode 13)…";
        } else if (waitingReroll_) {
            statusLocked_ = true;
            statusLine_ = "Relance des des (opcode 31)…";
        } else if (!statusLocked_) {
            statusLine_ = "R : relancer les des — Entree : valider — Esc : annuler";
        }
    } else if (T4CLoginSessionIsWaitingPutPlayerInGame()) {
        statusLocked_ = true;
        statusLine_ = "Entree en monde en cours (opcode 13)…";
    } else if (T4CLoginSessionIsWaitingCreatePlayer()) {
        statusLocked_ = true;
        statusLine_ = "Creation en cours (attente opcode 25)…";
    } else if (waitingReroll_) {
        statusLocked_ = true;
        statusLine_ = "Relance des des (opcode 31)…";
    } else if (!statusLocked_) {
        if (step_ == CharacterCreateStep::Name) {
            statusLine_ = "Nom (3-20 alphanum) — Entree : suivant — Esc : retour";
        } else if (step_ == CharacterCreateStep::Sex) {
            statusLine_ = "Tab : sexe — Entree : questionnaire — Esc : nom";
        } else if (step_ == CharacterCreateStep::Reroll) {
            statusLine_ = "R : relancer les des — Entree : valider — Esc : annuler";
        } else if (step_ == CharacterCreateStep::Questionnaire) {
            if (questionnaire_.isComplete()) {
                statusLine_ = "Questionnaire termine — creation en cours (attente opcode 25)…";
            } else {
                char buf[96];
                std::snprintf(buf, sizeof(buf), "Question %d/4 — Fleches ou 1-5 — Entree : valider — Esc : sexe",
                              questionnaire_.questionNumber() + 1);
                statusLine_ = buf;
            }
        }
    }
#endif
    if (chrome_) {
        chrome_->update();
    }
}

void CharacterCreateScreen::renderNameStep(SDL_Renderer *renderer, const SDL_Color textMain,
                                           const SDL_Color textMuted, const SDL_Color textField) {
    drawUiText(renderer, "Nom :", kPaddingX, kNameY, textMuted);
    SDL_FRect nameRect{kFieldX, kNameY - 8.0f, kFieldW, kFieldH};
    SDL_SetRenderDrawColor(renderer, 40, 44, 52, 255);
    SDL_RenderFillRect(renderer, &nameRect);
    SDL_SetRenderDrawColor(renderer, 100, 120, 140, 255);
    SDL_RenderRect(renderer, &nameRect);
    const std::string nameDisplay = nameStr_.empty() ? "_" : nameStr_;
    drawUiText(renderer, nameDisplay.c_str(), kFieldX + 8.0f, kNameY, textField);
    (void)textMain;
}

void CharacterCreateScreen::renderSexStep(SDL_Renderer *renderer, const SDL_Color textMain,
                                          const SDL_Color textMuted) {
    drawUiText(renderer, "Sexe :", kPaddingX, kSexY, textMuted);
    char sexLine[64];
    std::snprintf(sexLine, sizeof(sexLine), "%s  (Tab pour changer)", male_ ? "Homme" : "Femme");
    drawUiText(renderer, sexLine, kFieldX, kSexY, textMain);
}

void CharacterCreateScreen::renderQuestionnaireStep(SDL_Renderer *renderer, const SDL_Color textMain,
                                                    const SDL_Color textMuted, const SDL_Color textSel) {
    if (questionnaire_.isComplete()) {
        drawUiText(renderer, "Questionnaire termine — envoi de la creation au serveur…", kPaddingX, kQuestionY,
                   textMain);
        return;
    }
    drawUiText(renderer, questionnaire_.questionText().c_str(), kPaddingX, kQuestionY, textMain);

    float y = kAnswerStartY;
    for (int i = 0; i < T4CCharacterQuestionnaire::kAnswerCount; ++i) {
        char line[512];
        std::snprintf(line, sizeof(line), "%d) %s", i + 1, questionnaire_.answerText(i).c_str());
        if (i + 1 == questionnaire_.selectedChoice()) {
            SDL_FRect hl{kPaddingX - 8.0f, y - 4.0f, 704.0f, kAnswerRowH};
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 48, 72, 96, 200);
            SDL_RenderFillRect(renderer, &hl);
            SDL_SetRenderDrawColor(renderer, 120, 160, 200, 255);
            SDL_RenderRect(renderer, &hl);
            drawUiText(renderer, line, kPaddingX, y, textSel);
        } else {
            drawUiText(renderer, line, kPaddingX, y, textMuted);
        }
        y += kAnswerRowH;
    }
}

void CharacterCreateScreen::renderRerollStep(SDL_Renderer *renderer, const SDL_Color textMain,
                                             const SDL_Color textMuted, const SDL_Color textValue) {
    char title[128];
    std::snprintf(title, sizeof(title), "Stats de « %s » — jet de des", nameStr_.c_str());
    drawUiText(renderer, title, kPaddingX, kRerollTitleY, textMain);

    struct StatRow {
        const char *label;
        unsigned value;
    };
    const StatRow rows[] = {
        {"Force", rolledStats_.str},
        {"Endurance", rolledStats_.end},
        {"Agilite", rolledStats_.agi},
        {"Sagesse", rolledStats_.wis},
        {"Intelligence", rolledStats_.intel},
    };

    float y = kRerollStatsY;
    for (const StatRow &row : rows) {
        char line[64];
        std::snprintf(line, sizeof(line), "%-14s", row.label);
        drawUiText(renderer, line, kPaddingX, y, textMuted);
        char val[16];
        std::snprintf(val, sizeof(val), "%u", row.value);
        drawUiText(renderer, val, kFieldX, y, textValue);
        y += kRerollRowH;
    }

    char hpLine[64];
    std::snprintf(hpLine, sizeof(hpLine), "%-14s", "Points de vie");
    drawUiText(renderer, hpLine, kPaddingX, y, textMuted);
    char hpVal[32];
    std::snprintf(hpVal, sizeof(hpVal), "%u", rolledStats_.hp);
    drawUiText(renderer, hpVal, kFieldX, y, textValue);

    drawUiText(renderer, "[R] Relancer les des", kPaddingX, y + 48.0f, textMuted);
    drawUiText(renderer, "[Entree] Valider et entrer en monde", kPaddingX, y + 76.0f, textMuted);
    drawUiText(renderer, "[Esc] Annuler (supprime le perso)", kPaddingX, y + 104.0f, textMuted);
}

void CharacterCreateScreen::Render(SDL_Renderer *renderer) {
    if (chrome_) {
        chrome_->renderBackground(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 22, 26, 34, 255);
        SDL_RenderClear(renderer);
    }

    const SDL_Color textMain{230, 230, 240, 255};
    const SDL_Color textMuted{200, 210, 220, 255};
    const SDL_Color textField{255, 255, 255, 255};
    const SDL_Color textSel{180, 220, 255, 255};
    const SDL_Color textValue{255, 255, 255, 255};

    drawUiText(renderer, "The 4th Coming — Creation de personnage", kPaddingX, 48.0f, textMain);
    drawUiText(renderer, statusLine_.c_str(), kPaddingX, 80.0f, textMuted);

    switch (step_) {
        case CharacterCreateStep::Name:
            renderNameStep(renderer, textMain, textMuted, textField);
            break;
        case CharacterCreateStep::Sex:
            renderSexStep(renderer, textMain, textMuted);
            break;
        case CharacterCreateStep::Questionnaire:
            renderQuestionnaireStep(renderer, textMain, textMuted, textSel);
            break;
        case CharacterCreateStep::Reroll:
            renderRerollStep(renderer, textMain, textMuted, textValue);
            break;
    }

    if (chrome_) {
        chrome_->renderBanner(renderer);
    }
}
