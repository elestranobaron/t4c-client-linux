/**
 * Sonde E2E headless : auth 1.72 (14→99→26) sans GUI SDL3.
 * Usage: t4c_e2e_auth_probe [host] [port] [login] [password]
 *
 * Variables d'environnement (cumulables) :
 *   T4C_E2E_DOUBLE_CONNECT=1   double appel Start (1 socket attendu)
 *   T4C_E2E_RECONNECT=1        reconnect depuis l'ecran persos
 *   T4C_E2E_ENTER_WORLD=1      entre dans le monde (PutPlayerInGame)
 *   T4C_E2E_MOVE_TEST=1        en jeu, envoie des moves et exige une confirmation serveur
 *   T4C_E2E_WORLD_RECONNECT=1  depuis le monde, retour login + reconnexion
 *                              (+ MOVE_TEST = re-entre dans le monde et re-teste les moves)
 */
#include "game/T4CMovementBaseline.h"
#include "network/T4CLoginSession.h"
#include "network/T4CNetworkDebugLog.h"
#include "network/T4CNpcDialog.h"

#include <SDL3/SDL.h>

#include <unistd.h>

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace {

void LogLine(const char *fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    std::fprintf(stdout, "%s\n", buf);
    std::fflush(stdout);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "%s", buf);
}

bool EnvOn(const char *name) {
    const char *v = std::getenv(name);
    return v && v[0] == '1';
}

/** Attend la fin du LogoutWorker et le cooldown post-SafePlug (~7 s). */
bool WaitForReconnectReady(int timeoutSec) {
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSec);
    while (std::chrono::steady_clock::now() < deadline) {
        T4CLoginSessionPollBackgroundTasks();
        if (!T4CLoginSessionIsLogoutInProgress() &&
            T4CLoginSessionGetReconnectCooldownSeconds() == 0) {
            return true;
        }
        SDL_Delay(50);
    }
    return !T4CLoginSessionIsLogoutInProgress() &&
           T4CLoginSessionGetReconnectCooldownSeconds() == 0;
}

/** Attend la liste persos (opcode 26). Retourne false si reseau coupe / timeout. */
bool WaitCharacterList(std::vector<T4CCharacterSlot> *outSlots, int timeoutSec) {
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSec);
    while (std::chrono::steady_clock::now() < deadline) {
        T4CLoginSessionPollBackgroundTasks();
        if (T4CLoginSessionConsumeCharacterListReady()) {
            int maxPerAccount = 0;
            T4CLoginSessionCopyCharacterList(outSlots, &maxPerAccount);
            return true;
        }
        if (!T4CLoginSessionIsNetworkActive()) {
            return false;
        }
        SDL_Delay(50);
    }
    return false;
}

/** PutPlayerInGame + attente spawn. Retourne false sur erreur/timeout. */
bool EnterWorld(const std::string &name, T4CEnterWorldSpawn *outSpawn, int timeoutSec) {
    LogLine("[E2E] -> PutPlayerInGame « %s »", name.c_str());
    if (!T4CLoginSessionRequestPutPlayerInGame(name)) {
        LogLine("[E2E] FAIL envoi PutPlayerInGame");
        return false;
    }
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSec);
    bool spawnReady = false;
    while (std::chrono::steady_clock::now() < deadline) {
        T4CLoginSessionPollBackgroundTasks();
        if (!spawnReady && T4CLoginSessionConsumeEnterWorldReady(outSpawn)) {
            LogLine("[E2E] OK PutPlayerInGame @ %u,%u w=%u", outSpawn->x, outSpawn->y,
                    static_cast<unsigned>(outSpawn->world));
            spawnReady = true;
        }
        if (spawnReady && T4CLoginSessionIsWorldSessionReady()) {
            if (T4CLoginSessionRequestNearItems()) {
                LogLine("[E2E] -> RQ_GetNearItems (60) envoye (post-46)");
            }
            return true;
        }
        if (T4CLoginSessionHasPutPlayerInGameError()) {
            LogLine("[E2E] FAIL PutPlayerInGame: %s",
                    T4CLoginSessionGetPutPlayerInGameErrorMessage().c_str());
            return false;
        }
        if (!T4CLoginSessionIsNetworkActive()) {
            LogLine("[E2E] FAIL reseau coupe avant entree monde complete");
            return false;
        }
        SDL_Delay(50);
    }
    if (spawnReady) {
        LogLine("[E2E] FAIL timeout opcode 46 (FromPreInGameToInGame) %ds", timeoutSec);
    } else {
        LogLine("[E2E] FAIL timeout PutPlayerInGame %ds", timeoutSec);
    }
    return false;
}

/**
 * En jeu : envoie des moves (meme direction) et exige une confirmation serveur
 * (__EVENT_OBJECT_MOVED pour le joueur local). Retourne true si confirme.
 */
bool MoveTest(const char *tag, int timeoutSec) {
    LogLine("[E2E] -> test deplacement (%s) : envoi moves + attente confirmation serveur", tag);
    const std::uint16_t dir = 1;  // RQ_MoveNorth
    bool confirmed = false;
    int sent = 0;
    auto nextSend = std::chrono::steady_clock::now();
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSec);
    while (std::chrono::steady_clock::now() < deadline) {
        T4CLoginSessionPollBackgroundTasks();
        unsigned int mx = 0;
        unsigned int my = 0;
        std::uint16_t mop = 0;
        if (T4CLoginSessionConsumeLocalPlayerMove(&mx, &my, &mop)) {
            LogLine("[E2E] OK move confirme serveur (%s) @ %u,%u (op=%u) apres %d envois", tag, mx, my,
                    static_cast<unsigned>(mop), sent);
            confirmed = true;
            break;
        }
        if (std::chrono::steady_clock::now() >= nextSend && sent < 12) {
            if (T4CLoginSessionSendMove(dir)) {
                ++sent;
                LogLine("[E2E] move #%d envoye dir=%u (%s)", sent, static_cast<unsigned>(dir), tag);
            }
            nextSend = std::chrono::steady_clock::now() + std::chrono::milliseconds(400);
        }
        if (!T4CLoginSessionIsNetworkActive()) {
            LogLine("[E2E] FAIL reseau coupe pendant test move (%s)", tag);
            return false;
        }
        SDL_Delay(30);
    }
    if (!confirmed) {
        LogLine("[E2E] FAIL aucun move confirme par le serveur (%s, %d envois) — bug reproduit", tag, sent);
    }
    return confirmed;
}

const char *Arg(int argc, char **argv, int idx, const char *fallback) {
    return (idx < argc && argv[idx] && argv[idx][0]) ? argv[idx] : fallback;
}

}  // namespace

int main(int argc, char **argv) {
    const char *host = Arg(argc, argv, 1, "127.0.0.1");
    const char *port = Arg(argc, argv, 2, "11677");
    const char *login = Arg(argc, argv, 3, "test");
    const char *password = Arg(argc, argv, 4, "test");

    if (!SDL_Init(0)) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 10;
    }

    const bool doDouble = EnvOn("T4C_E2E_DOUBLE_CONNECT");
    const bool doReconnect = EnvOn("T4C_E2E_RECONNECT");
    const bool doEnterWorld = EnvOn("T4C_E2E_ENTER_WORLD");
    const bool doMoveTest = EnvOn("T4C_E2E_MOVE_TEST");
    const bool doWorldReconnect = EnvOn("T4C_E2E_WORLD_RECONNECT");

    LogLine("[E2E] demarrage auth host=%s port=%s login=%s", host, port, login);

    auto finish = [](int code) -> int {
        T4CLoginSessionShutdown();
        SDL_Quit();
        return code;
    };

    if (doDouble) {
        LogLine("[E2E] -> simulation double-clic Connect");
    }
    if (!T4CLoginSessionStart(host, port, login, password)) {
        LogLine("[E2E] FAIL T4CLoginSessionStart #1");
        SDL_Quit();
        return 1;
    }
    if (doDouble) {
        if (!T4CLoginSessionStart(host, port, login, password)) {
            LogLine("[E2E] FAIL T4CLoginSessionStart #2");
            return finish(12);
        }
        LogLine("[E2E] double Start OK (2e appel ignore, 1 socket)");
    }

    std::vector<T4CCharacterSlot> slots;
    if (!WaitCharacterList(&slots, 45)) {
        LogLine("[E2E] FAIL timeout 45s (pas de opcode 26) ou reseau coupe");
        return finish(3);
    }
    LogLine("[E2E] OK liste persos (%zu slots)", slots.size());
    for (const auto &s : slots) {
        LogLine("[E2E]   perso: %s lvl=%u race=%u", s.name.c_str(), static_cast<unsigned>(s.level),
                static_cast<unsigned>(s.race));
    }

    // --- Reconnexion simple depuis l'ecran persos ---
    if (doReconnect) {
        LogLine("[E2E] -> reconnexion (shutdown + restart auth)");
        T4CLoginSessionAbortLogin();
        if (!T4CLoginSessionStart(host, port, login, password)) {
            LogLine("[E2E] FAIL reconnexion T4CLoginSessionStart");
            return finish(8);
        }
        std::vector<T4CCharacterSlot> slots2;
        if (!WaitCharacterList(&slots2, 45)) {
            LogLine("[E2E] FAIL reconnexion: reseau coupe ou timeout");
            return finish(9);
        }
        LogLine("[E2E] OK reconnexion 14->26 PASS");
        return finish(0);
    }

    if (!doEnterWorld || slots.empty()) {
        return finish(0);
    }

    // --- Entree monde + (option) test move ---
    T4CEnterWorldSpawn spawn{};
    if (!EnterWorld(slots.front().name, &spawn, 30)) {
        return finish(7);
    }

    if (doMoveTest && !MoveTest("1ere connexion", 15)) {
        return finish(21);
    }

    /* Linger optionnel : reste en jeu N secondes pour recevoir l'inview push complet
     * (diagnostic visibilite PNJ — Mithrand/Araknor/marchands). */
    if (const char *lingerEnv = std::getenv("T4C_E2E_LINGER_SEC");
        lingerEnv != nullptr && lingerEnv[0] != '\0') {
        const int lingerSec = std::atoi(lingerEnv);
        if (lingerSec > 0) {
            LogLine("[E2E] linger %d s en jeu (reception inview/PNJ)", lingerSec);
            const Uint64 lingerUntil = SDL_GetTicks() + static_cast<Uint64>(lingerSec) * 1000u;
            const bool talkTest = EnvOn("T4C_E2E_TALK_TEST");
            /* Cible de marche optionnelle (ex. Mithrand 2961,1093) : on marche jusqu'a < 6
             * tuiles du point, puis on parle au PNJ le plus proche de ce point. */
            long goalX = -1;
            long goalY = -1;
            if (const char *gx = std::getenv("T4C_E2E_TALK_X")) {
                goalX = std::atol(gx);
            }
            if (const char *gy = std::getenv("T4C_E2E_TALK_Y")) {
                goalY = std::atol(gy);
            }
            struct NpcInfo {
                std::int32_t id{0};
                unsigned x{0};
                unsigned y{0};
            };
            std::vector<NpcInfo> npcs;
            bool talkSent = false;
            bool speechSeen = false;
            std::vector<T4CRemoteUnitEvent> events;
            auto nextMove = std::chrono::steady_clock::now();
            while (SDL_GetTicks() < lingerUntil) {
                T4CLoginSessionPollBackgroundTasks();
                if (talkTest) {
                    events.clear();
                    T4CLoginSessionDrainRemoteUnitEvents(&events);
                    for (const T4CRemoteUnitEvent &ev : events) {
                        if (ev.kind == T4CRemoteUnitEventKind::Spawn && ev.unitId > 1000000 &&
                            ev.appearance >= 10005) {
                            npcs.push_back({ev.unitId, ev.x, ev.y});
                        }
                    }
                    T4CActivePlayer me{};
                    T4CLoginSessionGetActivePlayer(&me);
                    /* Position courante : suit les acks serveur (GetActivePlayer ne bouge pas
                     * tout seul — c'est GameWorldScreen qui la met a jour dans le vrai client). */
                    static unsigned curX = 0;
                    static unsigned curY = 0;
                    if (curX == 0 && curY == 0 && me.valid) {
                        curX = me.serverX;
                        curY = me.serverY;
                    }
                    unsigned mx = 0;
                    unsigned my = 0;
                    std::uint16_t mop = 0;
                    while (T4CLoginSessionConsumeLocalPlayerMove(&mx, &my, &mop)) {
                        curX = mx;
                        curY = my;
                    }
                    me.serverX = curX;
                    me.serverY = curY;
                    const long refX = goalX >= 0 ? goalX : static_cast<long>(me.serverX);
                    const long refY = goalY >= 0 ? goalY : static_cast<long>(me.serverY);
                    const long pdx = refX - static_cast<long>(me.serverX);
                    const long pdy = refY - static_cast<long>(me.serverY);
                    const bool atGoal = pdx * pdx + pdy * pdy <= 25;
                    if (!talkSent && me.valid && !atGoal &&
                        std::chrono::steady_clock::now() >= nextMove) {
                        /* Anti-blocage : si la position ne change plus, alterne axe X / axe Y. */
                        static unsigned lastX = 0;
                        static unsigned lastY = 0;
                        static int stuck = 0;
                        if (me.serverX == lastX && me.serverY == lastY) {
                            ++stuck;
                        } else {
                            stuck = 0;
                            lastX = me.serverX;
                            lastY = me.serverY;
                        }
                        std::uint16_t op = 0;
                        if (stuck >= 3 && (stuck / 3) % 2 == 1 && pdx != 0) {
                            op = T4CMoveOpcodeFromArrows(pdx < 0, pdx > 0, false, false);
                        } else if (stuck >= 3 && pdy != 0) {
                            op = T4CMoveOpcodeFromArrows(false, false, pdy < 0, pdy > 0);
                        } else {
                            op = T4CMoveOpcodeFromArrows(pdx < 0, pdx > 0, pdy < 0, pdy > 0);
                        }
                        if (op != 0) {
                            T4CLoginSessionSendMove(op);
                        }
                        nextMove = std::chrono::steady_clock::now() + std::chrono::milliseconds(220);
                    }
                    if (!talkSent && me.valid && atGoal && !npcs.empty()) {
                        /* PNJ le plus proche du point cible (le serveur exige dist^2 < 120). */
                        const NpcInfo *best = nullptr;
                        long bestD = 1 << 30;
                        for (const NpcInfo &n : npcs) {
                            const long dx = static_cast<long>(n.x) - refX;
                            const long dy = static_cast<long>(n.y) - refY;
                            const long d = dx * dx + dy * dy;
                            if (d < bestD) {
                                bestD = d;
                                best = &n;
                            }
                        }
                        if (best && bestD < 100) {
                            LogLine("[E2E] -> DirectedTalk PNJ id=%d @ %u,%u (joueur @ %u,%u)",
                                    best->id, best->x, best->y, me.serverX, me.serverY);
                            T4CLoginSessionSendDirectedTalk(best->x, best->y, best->id, 1);
                            T4CLoginSessionRequestUnitName(best->id, best->x, best->y);
                            talkSent = true;
                        }
                    }
                    T4CNpcSpeech speech{};
                    if (T4CLoginSessionConsumeNpcSpeech(&speech)) {
                        LogLine("[E2E] <- parole unite id=%d npc=%d « %.120s » (locuteur « %s »)",
                                speech.unitId, speech.isNpc ? 1 : 0, speech.text.c_str(),
                                speech.speakerName.c_str());
                        speechSeen = true;
                    }
                }
                SDL_Delay(50);
            }
            if (talkTest) {
                LogLine("[E2E] talk test : envoye=%d reponse=%d", talkSent ? 1 : 0, speechSeen ? 1 : 0);
            }
        }
    }

    // Reconnexion MEME SOCKET depuis le monde (flux GUI exact : AbortLogin garde le
    // socket UDP → Start réutilise le même port → serveur « session deja auth IP/port »).
    if (EnvOn("T4C_E2E_SAME_SOCKET_RECONNECT")) {
        LogLine("[E2E] -> reconnexion MEME SOCKET depuis le monde (AbortLogin + Start)");
        T4CLoginSessionAbortLogin();
        SDL_Delay(200);
        for (int i = 0; i < 10; ++i) {
            T4CLoginSessionPollBackgroundTasks();
            SDL_Delay(30);
        }
        if (!T4CLoginSessionStart(host, port, login, password)) {
            LogLine("[E2E] FAIL reconnexion meme socket Start");
            return finish(16);
        }
        std::vector<T4CCharacterSlot> slotsSS;
        if (!WaitCharacterList(&slotsSS, 45)) {
            LogLine("[E2E] FAIL reconnexion meme socket: pas de liste persos (serveur muet ?)");
            return finish(17);
        }
        LogLine("[E2E] OK reconnexion meme socket 14->26 PASS");
        if (slotsSS.empty()) {
            slotsSS = slots;
        }
        T4CEnterWorldSpawn spawnSS{};
        if (!EnterWorld(slotsSS.front().name, &spawnSS, 30)) {
            return finish(18);
        }
        if (doMoveTest && !MoveTest("apres reconnexion meme socket", 15)) {
            return finish(23);
        }
        return finish(0);
    }

    // Fermeture brutale : quitte SANS logout propre (pas de SafePlug/ExitGame),
    // pour laisser une session/unite fantome cote serveur (simule fermeture appli / kill).
    if (EnvOn("T4C_E2E_HARD_EXIT")) {
        LogLine("[E2E] HARD_EXIT : sortie sans logout (fantome serveur attendu)");
        std::fflush(stdout);
        _exit(0);
    }

    // --- Retour login depuis le monde + reconnexion ---
    if (doWorldReconnect) {
        LogLine("[E2E] -> retour login depuis monde (DisconnectInGame + reconnect)");
        T4CLoginSessionDisconnectInGame();
        if (!WaitForReconnectReady(18)) {
            LogLine("[E2E] FAIL timeout attente logout/cooldown apres monde");
            return finish(14);
        }
        if (!T4CLoginSessionStart(host, port, login, password)) {
            LogLine("[E2E] FAIL reconnexion apres monde T4CLoginSessionStart");
            return finish(11);
        }
        std::vector<T4CCharacterSlot> slots2;
        if (!WaitCharacterList(&slots2, 45)) {
            LogLine("[E2E] FAIL reconnexion apres monde: reseau coupe ou timeout");
            return finish(12);
        }
        LogLine("[E2E] OK reconnexion apres monde 14->26 PASS");

        // Re-entre dans le monde et re-teste les moves (scenario qui echoue cote GUI).
        if (doMoveTest) {
            if (slots2.empty()) {
                slots2 = slots;
            }
            T4CEnterWorldSpawn spawn2{};
            if (!EnterWorld(slots2.front().name, &spawn2, 30)) {
                return finish(15);
            }
            if (!MoveTest("apres reconnexion", 15)) {
                return finish(22);
            }
        }
        return finish(0);
    }

    return finish(0);
}
