#include "gui/CharacterSelectScreen.h"

#include "gui/LauncherChrome.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdio>
#include <string>

#if defined(LINUX_PORT)
#include "network/T4CLoginSession.h"
#endif

namespace {

constexpr float kPaddingX = 48.0f;
constexpr float kListY = 120.0f;
constexpr float kRowH = 28.0f;
constexpr float kFooterRowH = 26.0f;
constexpr float kFooterBottomPad = 48.0f;

bool KeyDown(const SDL_Event &event, SDL_Scancode sc) {
    return event.type == SDL_EVENT_KEY_DOWN && event.key.down && !event.key.repeat &&
           event.key.scancode == sc;
}

int HitTestCharacterRow(float mx, float my, std::size_t rowCount) {
    if (rowCount == 0 || my < kListY || my >= kListY + static_cast<float>(rowCount) * kRowH) {
        return -1;
    }
    const int row = static_cast<int>((my - kListY) / kRowH);
    if (row < 0 || static_cast<std::size_t>(row) >= rowCount) {
        return -1;
    }
    if (mx < kPaddingX - 8.0f || mx > kPaddingX + 704.0f) {
        return -1;
    }
    return row;
}

}  // namespace

CharacterSelectScreen::CharacterSelectScreen(SDL_Renderer *renderer, LauncherChrome *chrome)
    : renderer_(renderer), chrome_(chrome) {
    resetFlow();
}

void CharacterSelectScreen::resetFlow() {
    stay_ = true;
    wantCreate_.store(false);
    confirmDelete_ = false;
    pendingDeleteName_.clear();
    selectedIndex_ = 0;
    statusLocked_ = false;
    statusLine_ = "Chargement liste…";
    lastTickMs_ = 0;
    introduction_.close();
#if defined(LINUX_PORT)
    T4CLoginSessionClearPutPlayerInGameError();
    T4CLoginSessionClearDeletePlayerError();
#endif
}

void CharacterSelectScreen::drawUiText(SDL_Renderer *renderer, const char *text, const float x,
                                       const float y, const SDL_Color color) const {
    if (chrome_ && chrome_->font().isReady()) {
        chrome_->font().drawText(renderer, text, x, y, color);
        return;
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDebugText(renderer, x, y, text);
}

void CharacterSelectScreen::refreshFromSession() {
#if defined(LINUX_PORT)
    std::vector<T4CCharacterSlot> slots;
    int maxPerAccount = 0;
    T4CLoginSessionCopyCharacterList(&slots, &maxPerAccount);

    displayLines_.clear();
    if (slots.empty()) {
        displayLines_.push_back("(aucun personnage sur ce compte)");
        selectedIndex_ = 0;
        return;
    }

    for (std::size_t i = 0; i < slots.size(); ++i) {
        const T4CCharacterSlot &s = slots[i];
        char line[128];
        std::snprintf(line, sizeof(line), "%s  (race %u, niv %u)", s.name.c_str(),
                      static_cast<unsigned>(s.race), static_cast<unsigned>(s.level));
        displayLines_.push_back(line);
    }
    selectedIndex_ = std::clamp(selectedIndex_, 0, static_cast<int>(slots.size()) - 1);

    if (T4CLoginSessionHasPutPlayerInGameError()) {
        statusLocked_ = true;
        confirmDelete_ = false;
        statusLine_ = T4CLoginSessionGetPutPlayerInGameErrorMessage();
    }
    if (T4CLoginSessionHasDeletePlayerError()) {
        statusLocked_ = true;
        confirmDelete_ = false;
        statusLine_ = T4CLoginSessionGetDeletePlayerErrorMessage();
    }
    if (T4CLoginSessionIsWaitingDeletePlayer()) {
        statusLocked_ = true;
        statusLine_ = "Suppression en cours (attente opcode 15/26)…";
    } else if (!T4CLoginSessionHasDeletePlayerError() && !confirmDelete_ &&
               statusLine_.find("Suppression en cours") == 0) {
        statusLocked_ = false;
    }
#endif
}

void CharacterSelectScreen::tryEnterWorld(SDL_Window * /*window*/) {
#if defined(LINUX_PORT)
    if (T4CLoginSessionIsWaitingPutPlayerInGame()) {
        statusLocked_ = true;
        statusLine_ = "Chargement perso (attente opcode 13)…";
        return;
    }
    std::vector<T4CCharacterSlot> slots;
    int maxPerAccount = 0;
    T4CLoginSessionCopyCharacterList(&slots, &maxPerAccount);
    if (selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(slots.size())) {
        statusLocked_ = true;
        statusLine_ = "Selection invalide.";
        return;
    }
    T4CLoginSessionClearPutPlayerInGameError();
    statusLocked_ = false;
    if (!T4CLoginSessionRequestPutPlayerInGame(slots[static_cast<std::size_t>(selectedIndex_)].name)) {
        statusLocked_ = true;
        statusLine_ = "Envoi RQ_PutPlayerInGame (13) impossible.";
        return;
    }
    statusLocked_ = true;
    statusLine_ = "Chargement perso (attente opcode 13)…";
    SDL_Log("[CharacterSelect] RQ_PutPlayerInGame envoye pour %s", slots[static_cast<std::size_t>(selectedIndex_)].name.c_str());
#endif
}

void CharacterSelectScreen::tryDeleteSelected(SDL_Window * /*window*/) {
#if defined(LINUX_PORT)
    if (T4CLoginSessionIsWaitingDeletePlayer() || T4CLoginSessionIsWaitingPutPlayerInGame()) {
        return;
    }
    std::vector<T4CCharacterSlot> slots;
    T4CLoginSessionCopyCharacterList(&slots, nullptr);
    if (selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(slots.size())) {
        statusLocked_ = true;
        confirmDelete_ = false;
        statusLine_ = "Selection invalide.";
        return;
    }
    const std::string &name = slots[static_cast<std::size_t>(selectedIndex_)].name;
    T4CLoginSessionClearDeletePlayerError();
    statusLocked_ = false;
    if (!T4CLoginSessionRequestDeletePlayer(name)) {
        statusLocked_ = true;
        confirmDelete_ = false;
        statusLine_ = "Envoi RQ_DeletePlayer (15) impossible.";
        return;
    }
    confirmDelete_ = false;
    statusLocked_ = true;
    statusLine_ = "Suppression en cours (attente opcode 15/26)…";
    if (selectedIndex_ >= static_cast<int>(slots.size()) - 1) {
        selectedIndex_ = std::max(0, selectedIndex_ - 1);
    }
    SDL_Log("[CharacterSelect] RQ_DeletePlayer envoye pour %s", name.c_str());
#endif
}

bool CharacterSelectScreen::HandleEvent(const SDL_Event &event, SDL_Window *window) {
    if (introduction_.isOpen()) {
        if (!introduction_.HandleEvent(event)) {
            introduction_.close();
        }
        return true;
    }

    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (KeyDown(event, SDL_SCANCODE_ESCAPE)) {
                if (confirmDelete_) {
                    confirmDelete_ = false;
                    pendingDeleteName_.clear();
                    statusLocked_ = false;
                    return true;
                }
                SDL_Log("[CharacterSelect] Esc — retour login");
                stay_ = false;
                statusLocked_ = false;
                return false;
            }
            if (KeyDown(event, SDL_SCANCODE_UP)) {
                selectedIndex_ = std::max(0, selectedIndex_ - 1);
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_DOWN)) {
                if (!displayLines_.empty()) {
                    selectedIndex_ =
                        std::min(selectedIndex_, static_cast<int>(displayLines_.size()) - 1);
                    selectedIndex_ = std::min(selectedIndex_ + 1,
                                             static_cast<int>(displayLines_.size()) - 1);
                }
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_RETURN) || KeyDown(event, SDL_SCANCODE_KP_ENTER)) {
                if (confirmDelete_) {
                    tryDeleteSelected(window);
                    return true;
                }
                tryEnterWorld(window);
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_D) || KeyDown(event, SDL_SCANCODE_DELETE)) {
#if defined(LINUX_PORT)
                if (confirmDelete_) {
                    return true;
                }
                std::vector<T4CCharacterSlot> slots;
                T4CLoginSessionCopyCharacterList(&slots, nullptr);
                if (slots.empty() || selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(slots.size())) {
                    statusLocked_ = true;
                    statusLine_ = "Aucun personnage a supprimer.";
                    return true;
                }
                pendingDeleteName_ = slots[static_cast<std::size_t>(selectedIndex_)].name;
                confirmDelete_ = true;
                statusLocked_ = false;
                statusLine_.clear();
#endif
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_C)) {
                if (confirmDelete_) {
                    return true;
                }
#if defined(LINUX_PORT)
                std::vector<T4CCharacterSlot> slots;
                int maxPerAccount = 0;
                T4CLoginSessionCopyCharacterList(&slots, &maxPerAccount);
                if (maxPerAccount > 0 && static_cast<int>(slots.size()) >= maxPerAccount) {
                    statusLocked_ = true;
                    statusLine_ = "Nombre maximum de personnages atteint.";
                    return true;
                }
#endif
                wantCreate_.store(true);
                return true;
            }
            if (KeyDown(event, SDL_SCANCODE_I) || KeyDown(event, SDL_SCANCODE_H)) {
                if (!confirmDelete_) {
                    introduction_.open();
                }
                return true;
            }
            return true;
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            if (confirmDelete_) {
                return true;
            }
            SDL_Event ev = event;
            if (renderer_) {
                SDL_ConvertEventToRenderCoordinates(renderer_, &ev);
            }
            if (!ev.button.down || ev.button.button != SDL_BUTTON_LEFT) {
                return true;
            }
            const int row = HitTestCharacterRow(ev.button.x, ev.button.y, displayLines_.size());
            if (row >= 0) {
                const Uint64 now = SDL_GetTicks();
                constexpr Uint64 kDoubleClickMs = 400;
                if (row == lastCharClickRow_ && lastCharClickMs_ != 0 &&
                    now - lastCharClickMs_ <= kDoubleClickMs) {
                    selectedIndex_ = row;
                    lastCharClickRow_ = -1;
                    lastCharClickMs_ = 0;
                    tryEnterWorld(window);
                } else {
                    selectedIndex_ = row;
                    lastCharClickRow_ = row;
                    lastCharClickMs_ = now;
                }
            }
            return true;
        }
        default:
            return true;
    }
}

void CharacterSelectScreen::Update() {
#if defined(LINUX_PORT)
    T4CLoginSessionPollBackgroundTasks();
#endif
    const Uint64 now = SDL_GetTicks();
    if (lastTickMs_ == 0) {
        lastTickMs_ = now;
    }
    const float delta = static_cast<float>(now - lastTickMs_) * 0.001f;
    lastTickMs_ = now;
    introduction_.Update(delta);

    if (chrome_) {
        chrome_->update();
    }
    refreshFromSession();
}

void CharacterSelectScreen::renderActionFooter(SDL_Renderer *renderer, const SDL_Color textMuted) const {
    std::vector<std::string> lines;
#if defined(LINUX_PORT)
    if (confirmDelete_) {
        lines.push_back("Supprimer « " + pendingDeleteName_ + " » ?");
        lines.emplace_back("[Entree]  Confirmer la suppression");
        lines.emplace_back("[Esc]     Annuler");
    } else {
        std::vector<T4CCharacterSlot> slots;
        int maxPerAccount = 0;
        T4CLoginSessionCopyCharacterList(&slots, &maxPerAccount);
        if (!slots.empty()) {
            lines.emplace_back("[Entree]  Jouer avec le personnage selectionne");
            lines.emplace_back("Clic x2   Entrer en jeu (double-clic sur la ligne)");
            lines.emplace_back("Clic      Selectionner un personnage dans la liste");
        }
        char createLine[96];
        std::snprintf(createLine, sizeof(createLine), "[C]       Creer un personnage (%zu/%d)",
                      slots.size(), maxPerAccount);
        lines.emplace_back(createLine);
        if (!slots.empty()) {
            lines.emplace_back("[D]       Supprimer le personnage selectionne");
        }
        lines.emplace_back("[I]       Introduction — recit de l'Haruspice");
        lines.emplace_back("[Esc]     Retour login");
    }
#else
    (void)renderer;
    (void)textMuted;
#endif
    if (lines.empty()) {
        return;
    }

    float y = static_cast<float>(kLogicalHeight) - kFooterBottomPad -
              static_cast<float>(lines.size()) * kFooterRowH;
    for (const std::string &line : lines) {
        drawUiText(renderer, line.c_str(), kPaddingX, y, textMuted);
        y += kFooterRowH;
    }
}

void CharacterSelectScreen::Render(SDL_Renderer *renderer) {
    if (introduction_.isOpen()) {
        introduction_.Render(renderer, chrome_);
        return;
    }

    if (chrome_) {
        chrome_->renderBackground(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 22, 26, 34, 255);
        SDL_RenderClear(renderer);
    }

    const SDL_Color textMain{230, 230, 240, 255};
    const SDL_Color textMuted{200, 210, 220, 255};
    const SDL_Color textSel{180, 220, 255, 255};

    drawUiText(renderer, "The 4th Coming — Selection du personnage", kPaddingX, 48.0f, textMain);
    if (statusLocked_ && !statusLine_.empty()) {
        drawUiText(renderer, statusLine_.c_str(), kPaddingX, 80.0f, textMuted);
    }

    float y = kListY;
    for (std::size_t i = 0; i < displayLines_.size(); ++i) {
        if (static_cast<int>(i) == selectedIndex_) {
            SDL_FRect hl{kPaddingX - 8.0f, y - 4.0f, 704.0f, kRowH};
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 48, 72, 96, 200);
            SDL_RenderFillRect(renderer, &hl);
            SDL_SetRenderDrawColor(renderer, 120, 160, 200, 255);
            SDL_RenderRect(renderer, &hl);
            drawUiText(renderer, displayLines_[i].c_str(), kPaddingX, y, textSel);
        } else {
            drawUiText(renderer, displayLines_[i].c_str(), kPaddingX, y, textMuted);
        }
        y += kRowH;
    }

    renderActionFooter(renderer, textMuted);

    if (chrome_) {
        chrome_->renderBanner(renderer);
    }
}
