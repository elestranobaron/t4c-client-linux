#pragma once

#include <SDL3/SDL.h>

#include "network/T4CNetworkDebugLog.h"

#include <string>
#include <vector>

class LauncherChrome;

enum class LoginActiveField : int {
    Ip = 0,
    Port = 1,
    Login = 2,
    Password = 3,
    None = -1,
};

/** Écran de connexion minimaliste en coordonnées logiques 800×600 (voir logical presentation SDL). */
class LoginScreen {
   public:
    static constexpr int kLogicalWidth = 800;
    static constexpr int kLogicalHeight = 600;

    explicit LoginScreen(SDL_Renderer *renderer, LauncherChrome *chrome);

    /** false = quitter l'application (ex. Escape sur l'écran login). */
    bool HandleEvent(const SDL_Event &event, SDL_Window *window);

    void Update();

    void Render(SDL_Renderer *renderer);

    const std::string &GetIp() const { return ipStr_; }
    const std::string &GetPort() const { return portStr_; }
    const std::string &GetLogin() const { return loginStr_; }
    const std::string &GetPassword() const { return passwordStr_; }

   private:
    void tryConnect(SDL_Window *window);
    void applyBackspace();
    void appendTextInput(const char *utf8Text);
    static bool PointInRect(float x, float y, const SDL_FRect &rect);
    std::string *mutableActiveString();
    void refreshMatrixLogLines();

    void drawUiText(SDL_Renderer *renderer, const char *text, float x, float y, SDL_Color color) const;

    SDL_Renderer *renderer_{nullptr};
    LauncherChrome *chrome_{nullptr};

    std::string ipStr_;
    std::string portStr_;
    std::string loginStr_;
    std::string passwordStr_;

    LoginActiveField activeField_{LoginActiveField::Ip};

    SDL_FRect ipFieldRect_{};
    SDL_FRect portFieldRect_{};
    SDL_FRect loginFieldRect_{};
    SDL_FRect passwordFieldRect_{};
    SDL_FRect connectButtonRect_{};
    SDL_FRect matrixConsoleRect_{};

    std::vector<T4CMatrixLogLine> matrixLogLines_;
    std::string authStatusLine_;

    static constexpr size_t kMaxFieldLen = 256;
    static constexpr size_t kMaxPortFieldLen = 8;
};
