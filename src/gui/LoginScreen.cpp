#include "gui/LoginScreen.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>

#include "gui/LoginClientConfig.h"
#include "gui/LauncherChrome.h"

#if defined(LINUX_PORT)
#include "network/T4CLoginSession.h"
#endif

namespace {

constexpr float kPaddingX = 48.0f;
constexpr float kFieldX = 240.0f;
constexpr float kFieldW = 480.0f;
constexpr float kFieldIpW = 352.0f;
constexpr float kPortFieldW = 112.0f;
constexpr float kPortFieldX = kFieldX + kFieldIpW + 18.0f;
constexpr float kFieldH = 36.0f;
constexpr float kRow1Y = 120.0f;
constexpr float kRow2Y = 200.0f;
constexpr float kRow3Y = 280.0f;

constexpr float kButtonW = 280.0f;
constexpr float kButtonH = 44.0f;
constexpr float kButtonX = (static_cast<float>(LoginScreen::kLogicalWidth) - kButtonW) * 0.5f;
constexpr float kButtonY = 400.0f;

/** Ajoute uniquement les caractères imprimables ASCII (SDL_RenderDebugText est limité à l’ASCII). */
void AppendAsciiPrintable(std::string &dst, const char *text) {
    if (!text) {
        return;
    }
    for (const unsigned char *p = reinterpret_cast<const unsigned char *>(text); *p; ++p) {
        unsigned char c = *p;
        if (c >= 32 && c <= 126) {
            dst.push_back(static_cast<char>(c));
        }
    }
}

std::string PasswordStars(size_t length) {
    return std::string(std::min(length, static_cast<size_t>(128)), '*');
}

}  // namespace

LoginScreen::LoginScreen(SDL_Renderer *renderer, LauncherChrome *chrome)
    : renderer_(renderer),
      chrome_(chrome),
      portStr_{"11677"},
      ipFieldRect_{kFieldX, kRow1Y - 8.0f, kFieldIpW, kFieldH},
      portFieldRect_{kPortFieldX, kRow1Y - 8.0f, kPortFieldW, kFieldH},
      loginFieldRect_{kFieldX, kRow2Y - 8.0f, kFieldW, kFieldH},
      passwordFieldRect_{kFieldX, kRow3Y - 8.0f, kFieldW, kFieldH},
      connectButtonRect_{kButtonX, kButtonY, kButtonW, kButtonH},
      matrixConsoleRect_{kPaddingX,
                         kButtonY + kButtonH + 10.0f,
                         static_cast<float>(LoginScreen::kLogicalWidth) - 2.0f * kPaddingX,
                         548.0f - (kButtonY + kButtonH + 10.0f)} {
    std::string lip;
    std::string lport;
    std::string llogin;
    if (LoadLoginClientConfig(&lip, &lport, &llogin)) {
        if (!lip.empty()) {
            ipStr_ = std::move(lip);
        }
        if (!lport.empty()) {
            portStr_ = std::move(lport);
        }
        if (!llogin.empty()) {
            loginStr_ = std::move(llogin);
        }
    }
}

bool LoginScreen::PointInRect(float x, float y, const SDL_FRect &rect) {
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}

std::string *LoginScreen::mutableActiveString() {
    switch (activeField_) {
        case LoginActiveField::Ip:
            return &ipStr_;
        case LoginActiveField::Port:
            return &portStr_;
        case LoginActiveField::Login:
            return &loginStr_;
        case LoginActiveField::Password:
            return &passwordStr_;
        default:
            return nullptr;
    }
}

void LoginScreen::appendTextInput(const char *utf8Text) {
    std::string *s = mutableActiveString();
    if (!s) {
        return;
    }
    std::string chunk;
    AppendAsciiPrintable(chunk, utf8Text);
    if (chunk.empty()) {
        return;
    }
    if (activeField_ == LoginActiveField::Port) {
        std::string digits;
        for (char c : chunk) {
            if (c >= '0' && c <= '9') {
                digits.push_back(c);
            }
        }
        chunk = std::move(digits);
        if (chunk.empty()) {
            return;
        }
    }
    const size_t maxLen = (activeField_ == LoginActiveField::Port) ? kMaxPortFieldLen : kMaxFieldLen;
    if (s->size() + chunk.size() > maxLen) {
        chunk.resize(maxLen - s->size());
    }
    s->append(chunk);
}

void LoginScreen::applyBackspace() {
    std::string *s = mutableActiveString();
    if (!s || s->empty()) {
        return;
    }
    s->pop_back();
}

void LoginScreen::tryConnect(SDL_Window *window) {
    (void)window;
    std::string portSave = portStr_;
    while (!portSave.empty() && std::isspace(static_cast<unsigned char>(portSave.front()))) {
        portSave.erase(0, 1);
    }
    while (!portSave.empty() && std::isspace(static_cast<unsigned char>(portSave.back()))) {
        portSave.pop_back();
    }
    if (portSave.empty()) {
        portSave = "11677";
    }

#if defined(LINUX_PORT)
    if (ipStr_.empty()) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[LoginScreen] IP vide.");
        return;
    }
    if (loginStr_.empty()) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[LoginScreen] Login vide.");
        return;
    }
#endif

    if (!SaveLoginClientConfig(ipStr_, portSave, loginStr_)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[LoginScreen] Impossible d'écrire %s",
                    LoginClientConfigPath().c_str());
    }

#if defined(LINUX_PORT)
    T4CLoginSessionPollBackgroundTasks();
    SDL_Log("[LoginScreen] Connexion demandee - IP=\"%s\" port=\"%s\" login_len=%zu password_len=%zu", ipStr_.c_str(),
            portSave.c_str(), loginStr_.size(), passwordStr_.size());
    authStatusLine_.clear();
    if (!T4CLoginSessionStart(ipStr_, portSave, loginStr_, passwordStr_)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[LoginScreen] Echec demarrage session reseau.");
    }
#else
    SDL_Log("[LoginScreen] Connexion (stub hors LINUX_PORT)");
#endif
}

void LoginScreen::refreshMatrixLogLines() {
#if defined(LINUX_PORT)
    constexpr std::size_t kMaxConsoleLines = 120;
    T4CNetworkDebugCopyRecent(matrixLogLines_, kMaxConsoleLines);
#endif
}

bool LoginScreen::HandleEvent(const SDL_Event &event, SDL_Window *window) {
    SDL_Event ev = event;
    switch (event.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            if (renderer_) {
                SDL_ConvertEventToRenderCoordinates(renderer_, &ev);
            }
            if (!ev.button.down || ev.button.button != SDL_BUTTON_LEFT) {
                return true;
            }
            const float mx = ev.button.x;
            const float my = ev.button.y;

            if (PointInRect(mx, my, ipFieldRect_)) {
                activeField_ = LoginActiveField::Ip;
                if (window) {
                    SDL_StartTextInput(window);
                }
                return true;
            }
            if (PointInRect(mx, my, portFieldRect_)) {
                activeField_ = LoginActiveField::Port;
                if (window) {
                    SDL_StartTextInput(window);
                }
                return true;
            }
            if (PointInRect(mx, my, loginFieldRect_)) {
                activeField_ = LoginActiveField::Login;
                if (window) {
                    SDL_StartTextInput(window);
                }
                return true;
            }
            if (PointInRect(mx, my, passwordFieldRect_)) {
                activeField_ = LoginActiveField::Password;
                if (window) {
                    SDL_StartTextInput(window);
                }
                return true;
            }
            if (PointInRect(mx, my, connectButtonRect_)) {
                tryConnect(window);
                return true;
            }
            activeField_ = LoginActiveField::None;
            return true;
        }

        case SDL_EVENT_TEXT_INPUT: {
            appendTextInput(event.text.text);
            return true;
        }

        case SDL_EVENT_KEY_DOWN: {
            if (event.key.scancode == SDL_SCANCODE_BACKSPACE && event.key.down) {
                applyBackspace();
                return true;
            }
            if (!event.key.down || event.key.repeat) {
                return true;
            }
            if (event.key.key == SDLK_ESCAPE) {
                return false;
            }
            if (event.key.key == SDLK_RETURN || event.key.key == SDLK_KP_ENTER) {
                tryConnect(window);
                return true;
            }
            if (event.key.scancode == SDL_SCANCODE_TAB) {
                switch (activeField_) {
                    case LoginActiveField::Ip:
                        activeField_ = LoginActiveField::Port;
                        break;
                    case LoginActiveField::Port:
                        activeField_ = LoginActiveField::Login;
                        break;
                    case LoginActiveField::Login:
                        activeField_ = LoginActiveField::Password;
                        break;
                    case LoginActiveField::Password:
                        activeField_ = LoginActiveField::Ip;
                        break;
                    default:
                        activeField_ = LoginActiveField::Ip;
                        break;
                }
                if (window) {
                    SDL_StartTextInput(window);
                }
                return true;
            }
            return true;
        }

        default:
            return true;
    }
}

void LoginScreen::drawUiText(SDL_Renderer *renderer, const char *text, const float x, const float y,
                             const SDL_Color color) const {
    if (chrome_ && chrome_->font().isReady()) {
        chrome_->font().drawText(renderer, text, x, y, color);
        return;
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDebugText(renderer, x, y, text);
}

void LoginScreen::Update() {
#if defined(LINUX_PORT)
    T4CLoginSessionPollBackgroundTasks();
    if (!T4CLoginSessionIsNetworkActive()) {
        const std::string err = T4CLoginSessionGetLastAuthError();
        if (!err.empty()) {
            authStatusLine_ = err;
        } else if (T4CLoginSessionIsLogoutInProgress()) {
            authStatusLine_ = "Deconnexion en cours (SafePlug)…";
        } else if (T4CLoginSessionGetReconnectCooldownSeconds() > 0) {
            authStatusLine_ = "Attente liberation compte serveur…";
        } else {
            authStatusLine_.clear();
        }
    } else if (T4CLoginSessionGetReconnectCooldownSeconds() > 0) {
        authStatusLine_ = "Deconnexion en cours…";
    } else {
        authStatusLine_ = "Connexion en cours… (re-cliquer Connect relance l'etape en cours)";
    }
#endif
    if (chrome_) {
        chrome_->update();
    }
    refreshMatrixLogLines();
}

void LoginScreen::Render(SDL_Renderer *renderer) {
    if (chrome_) {
        chrome_->renderBackground(renderer);
    }

    const LoginActiveField focus = activeField_;
    const SDL_Color textMain{230, 230, 240, 255};
    const SDL_Color textMuted{180, 180, 195, 255};

    auto drawFieldRect = [&](const SDL_FRect &r, bool selected) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 52, 70, 90, 210);
        } else {
            SDL_SetRenderDrawColor(renderer, 38, 42, 52, 190);
        }
        SDL_RenderFillRect(renderer, &r);
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 200, 180, 120, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 90, 95, 110, 255);
        }
        SDL_RenderRect(renderer, &r);
    };

    drawFieldRect(ipFieldRect_, focus == LoginActiveField::Ip);
    drawFieldRect(portFieldRect_, focus == LoginActiveField::Port);
    drawFieldRect(loginFieldRect_, focus == LoginActiveField::Login);
    drawFieldRect(passwordFieldRect_, focus == LoginActiveField::Password);

    SDL_SetRenderDrawColor(renderer, 55, 120, 75, 255);
    SDL_RenderFillRect(renderer, &connectButtonRect_);
    SDL_SetRenderDrawColor(renderer, 100, 200, 130, 255);
    SDL_RenderRect(renderer, &connectButtonRect_);

    constexpr float ly1 = kRow1Y;
    constexpr float ly2 = kRow2Y;
    constexpr float ly3 = kRow3Y;

    drawUiText(renderer, "IP:", kPaddingX, ly1, textMain);
    {
        std::string line = ipStr_;
        line.insert(0, " ");
        drawUiText(renderer, line.c_str(), kFieldX + 10.0f, ly1, textMain);
    }

    constexpr float kPortLabelX = kPortFieldX - 56.0f;
    drawUiText(renderer, "Port:", kPortLabelX, ly1, textMain);
    {
        std::string line = portStr_;
        line.insert(0, " ");
        drawUiText(renderer, line.c_str(), kPortFieldX + 8.0f, ly1, textMain);
    }

    drawUiText(renderer, "Login:", kPaddingX, ly2, textMain);
    {
        std::string line = loginStr_;
        line.insert(0, " ");
        drawUiText(renderer, line.c_str(), kFieldX + 10.0f, ly2, textMain);
    }

    drawUiText(renderer, "Password:", kPaddingX, ly3, textMain);
    {
        std::string line = PasswordStars(passwordStr_.size());
        line.insert(0, " ");
        drawUiText(renderer, line.c_str(), kFieldX + 10.0f, ly3, textMain);
    }

    drawUiText(renderer, "Se connecter", connectButtonRect_.x + 72.f, connectButtonRect_.y + 12.f, textMain);

    drawUiText(renderer, "The 4th Coming — Connexion", kPaddingX, 48.0f, textMuted);
    if (!authStatusLine_.empty()) {
        const SDL_Color textErr{255, 120, 100, 255};
        drawUiText(renderer, authStatusLine_.c_str(), kPaddingX, 72.0f, textErr);
    }

    /* Console Matrix (dernieres lignes en bas, defilement implicite). */
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &matrixConsoleRect_);
    SDL_SetRenderDrawColor(renderer, 0, 255, 120, 255);
    SDL_RenderRect(renderer, &matrixConsoleRect_);

    SDL_SetRenderDrawColor(renderer, 160, 170, 190, 255);
    SDL_RenderDebugText(renderer, matrixConsoleRect_.x + 6.0f, matrixConsoleRect_.y + 4.0f,
                        "Vert=UDP/crypto  Cyan=auth/version  Jaune=jalon  Rouge=alerte");

    const float rowH = static_cast<float>(SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) + 2.0f;
    const int maxVis =
        std::max(1, static_cast<int>((matrixConsoleRect_.h - 10.0f - rowH) / std::max(1.0f, rowH)));
    const std::size_t n = matrixLogLines_.size();
    std::size_t start = 0;
    if (n > static_cast<std::size_t>(maxVis)) {
        start = n - static_cast<std::size_t>(maxVis);
    }
    float y = matrixConsoleRect_.y + 6.0f + rowH;
    for (std::size_t i = start; i < n; ++i) {
        std::string line = matrixLogLines_[i].text;
        if (line.size() > 100) {
            line.resize(97);
            line += "...";
        }
        switch (matrixLogLines_[i].kind) {
            case T4CMatrixLogKind::Net:
                SDL_SetRenderDrawColor(renderer, 0, 255, 120, 255);
                break;
            case T4CMatrixLogKind::Auth:
                SDL_SetRenderDrawColor(renderer, 120, 210, 255, 255);
                break;
            case T4CMatrixLogKind::Phase:
                SDL_SetRenderDrawColor(renderer, 255, 210, 70, 255);
                break;
            case T4CMatrixLogKind::Warn:
                SDL_SetRenderDrawColor(renderer, 255, 90, 90, 255);
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                break;
        }
        SDL_RenderDebugText(renderer, matrixConsoleRect_.x + 6.0f, y, line.c_str());
        y += rowH;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    if (chrome_) {
        chrome_->renderBanner(renderer);
    }
}
