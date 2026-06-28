#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <csignal>

#include "audio/T4CGameMusic.h"
#include "game/GameWorldScreen.h"
#include "gui/CharacterCreateScreen.h"
#include "gui/CharacterSelectScreen.h"
#include "gui/LauncherChrome.h"
#include "gui/LoginScreen.h"
#include "network/T4CLoginSession.h"

namespace {

enum class AppPhase { Login, CharacterSelect, CharacterCreate, WorldLoading, World };

/** Ctrl+C : terminaison propre (RQ_ExitGame avant fermeture UDP). */
volatile std::sig_atomic_t g_sigintRequested = 0;

void OnSigInt(int) {
    g_sigintRequested = 1;
}

/** Demarre le chargement carte/sprites (ecran loading). Retourne false si echec. */
bool TryBeginWorldLoad(AppPhase &phase, GameWorldScreen &world, LauncherChrome &launcherChrome,
                       SDL_Renderer *renderer, SDL_Window *window, AppPhase fallbackPhase) {
    T4CEnterWorldSpawn spawn{};
    if (!T4CLoginSessionConsumeEnterWorldReady(&spawn)) {
        return false;
    }

    SDL_StopTextInput(window);
    SDL_SetWindowTitle(window, "T4C — chargement");
    SDL_SetWindowSize(window, 1280, 720);
    SDL_SetRenderLogicalPresentation(renderer, GameWorldScreen::kLogicalWidth, GameWorldScreen::kLogicalHeight,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    if (!world.BeginLoad(renderer, window, spawn.x, spawn.y, spawn.world)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "T4C", world.GetLastError().c_str(), window);
        phase = fallbackPhase;
        if (fallbackPhase != AppPhase::World) {
            SDL_StartTextInput(window);
        }
        return false;
    }

    launcherChrome.layoutBannerForLogicalSize(static_cast<float>(GameWorldScreen::kLogicalWidth),
                                            static_cast<float>(GameWorldScreen::kLogicalHeight));

    phase = AppPhase::WorldLoading;
    T4CGameMusic::Stop();
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    SDL_Log("[Main] chargement monde — %s @ %u,%u w=%u", active.valid ? active.name.c_str() : "?", spawn.x,
            spawn.y, static_cast<unsigned>(spawn.world));
    return true;
}

}  // namespace

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        return 1;
    }

    std::signal(SIGINT, OnSigInt);
    std::signal(SIGTERM, OnSigInt);

    SDL_Window *window = SDL_CreateWindow("T4C Client 1.72 Linux", 800, 600, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderLogicalPresentation(renderer, LoginScreen::kLogicalWidth, LoginScreen::kLogicalHeight,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

    LauncherChrome launcherChrome;
    launcherChrome.init(renderer);

    T4CGameMusic::Init();

    LoginScreen login(renderer, &launcherChrome);
    CharacterSelectScreen characterSelect(renderer, &launcherChrome);
    CharacterCreateScreen characterCreate(renderer, &launcherChrome);
    GameWorldScreen world;

    SDL_StartTextInput(window);

    AppPhase phase = AppPhase::Login;
    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || g_sigintRequested) {
                running = false;
                break;
            }
            if (phase == AppPhase::Login) {
                if (!login.HandleEvent(event, window)) {
                    running = false;
                }
            } else if (phase == AppPhase::CharacterSelect) {
                if (!characterSelect.HandleEvent(event, window)) {
                    T4CLoginSessionAbortLogin();
                    T4CLoginSessionResetAfterReturnToLogin();
                    T4CGameMusic::Stop();
                    phase = AppPhase::Login;
                    SDL_SetWindowTitle(window, "T4C Client 1.72 Linux");
                    SDL_StartTextInput(window);
                }
            } else if (phase == AppPhase::CharacterCreate) {
                if (!characterCreate.HandleEvent(event, window)) {
                    phase = AppPhase::CharacterSelect;
                    SDL_SetWindowTitle(window, "T4C — personnages");
                    SDL_StopTextInput(window);
                }
            } else {
                world.HandleEvent(event);
                if (world.ConsumeQuitApp()) {
                    running = false;
                }
                if (world.ConsumeReturnToLogin()) {
                    T4CLoginSessionDisconnectInGame();
                    world.Shutdown();
                    T4CLoginSessionResetAfterReturnToLogin();
                    T4CGameMusic::Stop();
                    SDL_ShowCursor();
                    phase = AppPhase::Login;
                    SDL_SetWindowTitle(window, "T4C Client 1.72 Linux");
                    SDL_SetWindowSize(window, 800, 600);
                    SDL_SetRenderLogicalPresentation(renderer, LoginScreen::kLogicalWidth,
                                                     LoginScreen::kLogicalHeight,
                                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
                    launcherChrome.layoutBannerForLogicalSize(static_cast<float>(LoginScreen::kLogicalWidth),
                                                              static_cast<float>(LoginScreen::kLogicalHeight));
                    SDL_StartTextInput(window);
                }
            }
        }

        if (!running) {
            break;
        }

        T4CLoginSessionPollBackgroundTasks();

        if (phase == AppPhase::Login) {
            login.Update();
            if (T4CLoginSessionConsumeCharacterListReady()) {
                characterSelect.resetFlow();
                phase = AppPhase::CharacterSelect;
                SDL_StopTextInput(window);
                SDL_SetWindowTitle(window, "T4C — personnages");
                T4CGameMusic::StartCharacterSelect();
            }
            SDL_SetRenderDrawColor(renderer, 18, 20, 26, 255);
            SDL_RenderClear(renderer);
            login.Render(renderer);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        } else if (phase == AppPhase::CharacterSelect) {
            characterSelect.Update();
            if (characterSelect.ConsumeWantCreateCharacter()) {
                characterCreate.resetFlow();
                phase = AppPhase::CharacterCreate;
                SDL_SetWindowTitle(window, "T4C — creation perso");
                SDL_StartTextInput(window);
            }
            TryBeginWorldLoad(phase, world, launcherChrome, renderer, window, AppPhase::CharacterSelect);
            if (phase == AppPhase::CharacterSelect) {
                SDL_SetRenderDrawColor(renderer, 18, 20, 26, 255);
                SDL_RenderClear(renderer);
                characterSelect.Render(renderer);
                SDL_RenderPresent(renderer);
                SDL_Delay(16);
            }
        } else if (phase == AppPhase::CharacterCreate) {
            characterCreate.Update();
            if (!TryBeginWorldLoad(phase, world, launcherChrome, renderer, window, AppPhase::CharacterCreate)) {
                if (T4CLoginSessionConsumeCreatePlayerSuccess()) {
                    phase = AppPhase::CharacterSelect;
                    T4CGameMusic::StartCharacterSelect();
                }
            }
            if (phase == AppPhase::CharacterCreate) {
                SDL_SetRenderDrawColor(renderer, 18, 20, 26, 255);
                SDL_RenderClear(renderer);
                characterCreate.Render(renderer);
                SDL_RenderPresent(renderer);
                SDL_Delay(16);
            }
        } else if (phase == AppPhase::WorldLoading) {
            const Uint64 frameStart = SDL_GetTicks();

            SDL_Event loadEvent;
            while (SDL_PollEvent(&loadEvent)) {
                if (loadEvent.type == SDL_EVENT_QUIT || g_sigintRequested) {
                    running = false;
                    break;
                }
            }
            if (!running) {
                break;
            }

            T4CLoginSessionPollBackgroundTasks();
            launcherChrome.update();
            world.RenderLoading(renderer, &launcherChrome);
            launcherChrome.renderBanner(renderer);
            SDL_RenderPresent(renderer);

            world.PollLoadForMs(12);

            if (world.IsReady()) {
                T4CActivePlayer active{};
                T4CLoginSessionGetActivePlayer(&active);
                const unsigned level =
                    active.valid && active.level > 0 ? static_cast<unsigned>(active.level) : 1u;
                T4CGameMusic::LoadNewSound(world.GetWorldId(), world.GetSpawnX(), world.GetSpawnY(), level);
                phase = AppPhase::World;
                SDL_HideCursor();
                SDL_SetWindowTitle(window, "T4C — monde");
                SDL_Log("[Main] entree monde OK — %s", active.valid ? active.name.c_str() : "?");
            }

            SDL_RenderPresent(renderer);
            const Uint64 frameMs = SDL_GetTicks() - frameStart;
            if (frameMs < 16) {
                SDL_Delay(static_cast<Uint32>(16 - frameMs));
            }
        } else {
            world.Update();
            world.Render(renderer, &launcherChrome);
            SDL_RenderPresent(renderer);
            SDL_Delay(5);
        }
    }

    world.Shutdown();
    T4CLoginSessionShutdown();
    T4CGameMusic::Shutdown();
    launcherChrome.shutdown();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
