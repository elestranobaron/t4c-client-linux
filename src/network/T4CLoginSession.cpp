/**
 * Session login / monde T4C 1.72 — stack COMM + NMPacketManager (pas CommCenter 1.68).
 */
#include "network/T4CLoginSession.h"
#include "network/T4CNpcDialog.h"
#include "network/T4CPlayerInventory.h"

#include "audio/T4CGameMusic.h"
#include "game/T4CMovementBaseline.h"
#include "Comm.h"
#include "PacketTypes.h"
#include "TFCPacket.h"
#include "TFCSocket.h"
#include "network/T4CCommBridge.h"
#include "network/T4CClientStubs.h"
#include "network/T4CNetworkDebugLog.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include "game/T4CUnitSpriteNames.h"

#include <cstdio>
#include <cstring>
#include <iterator>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef RQ_SafePlug
/** Opcode antiplug (serveur Linux PacketManager_linux.cpp). */
#define RQ_SafePlug 123
#endif

bool T4CLoginSessionStart(const std::string &hostField, const std::string &portField, const std::string &login,
                          const std::string &password);

namespace {

constexpr short kClientVersion = 1720;
constexpr std::uint16_t kStillConnected = 10;
constexpr std::uint16_t kPacketPopup = 10004;

std::mutex g_mutex;
std::string g_login;
std::string g_password;
std::string g_selectedPlayer;
long g_sessionKey = 0;

std::atomic<int> g_pipelineStep{0};
std::atomic<bool> g_pendingCharacterList{false};
std::atomic<bool> g_pendingEnterWorld{false};
std::atomic<bool> g_waitingPutPlayer{false};
std::atomic<Uint32> g_putPlayerSentAt{0};
std::atomic<int> g_putPlayerRetryCount{0};
std::atomic<bool> g_waitingCreate{false};
std::atomic<Uint32> g_createSentAt{0};
std::atomic<int> g_createRetryCount{0};
unsigned char g_pendingCreateStats[5]{};
unsigned char g_pendingCreateSex{0};
std::atomic<bool> g_waitingDelete{false};
std::atomic<Uint32> g_deleteSentAt{0};
std::atomic<int> g_deleteRetryCount{0};
std::atomic<bool> g_pendingDeleteAfterAuth{false};
std::atomic<bool> g_pendingCreateAfterAuth{false};
std::string g_pendingDeleteName;
std::atomic<bool> g_waitingReroll{false};
std::atomic<bool> g_inCreateRerollPhase{false};
std::atomic<bool> g_autoEnterWorldAfterCreate{false};
std::atomic<bool> g_pendingCreateSuccess{false};
std::atomic<bool> g_rolledStatsPending{false};
std::atomic<bool> g_inGame{false};
std::atomic<bool> g_attackMode{false};

std::string g_pendingCreateName;
T4CCharacterRolledStats g_rolledStats{};
std::mutex g_rolledMutex;

std::unordered_map<std::int32_t, std::uint16_t> g_remoteAppearance;

std::unordered_map<std::int32_t, T4CPuppetDress> g_remotePuppet;
std::unordered_map<std::int32_t, std::pair<unsigned int, unsigned int>> g_pendingPuppetRequest;

/* Noms reels des unites (RQ_GetUnitName 35) : « Dark Fang », « Mithrand »… + couleur serveur. */
struct T4CUnitNameEntry {
    std::string name;
    std::uint32_t color{0};
};
std::unordered_map<std::int32_t, T4CUnitNameEntry> g_unitNames;
/* Requete en cours → date d'envoi : permet le retry (PNJ qui a bouge, reponse perdue). */
std::unordered_map<std::int32_t, std::chrono::steady_clock::time_point> g_pendingUnitName;

std::mutex g_charMutex;
std::vector<T4CCharacterSlot> g_characters;
int g_maxCharsPerAccount = 4;

std::mutex g_errMutex;
std::string g_putPlayerError;
std::string g_createError;
std::string g_deleteError;
std::string g_authError;

T4CEnterWorldSpawn g_spawn{};
T4CActivePlayer g_activePlayer{};
std::mutex g_activeMutex;

std::mutex g_remoteMutex;
std::vector<T4CRemoteUnitEvent> g_remoteEvents;
std::vector<T4CGroundObjectMarker> g_groundMarkers;
std::mutex g_localMoveMutex;
unsigned int g_localMoveX{0};
unsigned int g_localMoveY{0};
std::uint16_t g_localMoveOpcode{0};
std::atomic<bool> g_localMovePending{false};

std::atomic<bool> g_statusDirty{false};
std::atomic<bool> g_popupPending{false};
std::mutex g_statusMutex;
T4CPlayerStatus g_playerStatus{};

std::mutex g_inventoryMutex;
T4CPlayerBackpack g_backpack{};
T4CPlayerEquipment g_equipment{};
T4CPlayerSkillBook g_skillBook{};
T4CPlayerSpellBook g_spellBook{};
std::atomic<bool> g_inventoryDirty{false};

std::mutex g_chatMutex;
std::vector<T4CChatMessage> g_chatMessages;

std::mutex g_timeMutex;
T4CGameTime g_gameTime{};

void PushChatMessage(T4CChatMessage msg) {
    std::lock_guard<std::mutex> lock(g_chatMutex);
    if (g_chatMessages.size() > 200) {
        g_chatMessages.erase(g_chatMessages.begin(), g_chatMessages.begin() + 100);
    }
    g_chatMessages.push_back(std::move(msg));
}
std::atomic<bool> g_playerDead{false};
std::atomic<bool> g_deathPending{false};
std::mutex g_deathMutex;
T4CDeathState g_deathState{};
std::atomic<bool> g_deathResurrectSent{false};

std::mutex g_damageMutex;
std::vector<T4CFloatingDamage> g_floatingDamage{};

/* Attaque locale (10001/10002 avec le joueur comme attaquant) — consommee par l'ecran monde
 * pour jouer l'animation de swing du perso. */
std::mutex g_localAttackMutex;
bool g_localAttackPending = false;
unsigned g_localAttackTargetX = 0;
unsigned g_localAttackTargetY = 0;

std::mutex g_talkMutex;
T4CTalkState g_talkState{};
T4CNpcSpeech g_pendingSpeech{};
std::atomic<bool> g_speechPending{false};
T4CShopList g_shopList{};
std::atomic<bool> g_shopDirty{false};

/** Delai minimal avant nouvel essai Register (ms, horloge SDL_GetTicks). */
std::atomic<Uint32> g_reconnectAllowedAfterTick{0};
std::atomic<bool> g_logoutInProgress{false};
std::atomic<bool> g_pendingAutoRegister{false};
std::atomic<Uint32> g_autoRegisterAt{0};
std::atomic<Uint32> g_registerSentAt{0};
std::atomic<int> g_registerRetryCount{0};
/** Horodatage dernier envoi etape auth 2 (99) ou 3 (26). */
std::atomic<Uint32> g_authPhaseSentAt{0};
std::atomic<int> g_authPhaseRetryCount{0};
std::string g_lastHost;
std::string g_lastPort;

constexpr int kPostLogoutReconnectCooldownSec = 3;

std::mutex g_logoutThreadMutex;
std::thread g_logoutThread;
std::atomic<bool> g_logoutSentThisSession{false};
std::atomic<bool> g_waitingFromPreInGame{false};
std::atomic<int> g_fromPreInGameResult{-1};
std::atomic<Uint32> g_enterWorld46SentAt{0};
std::atomic<int> g_enterWorld46RetryCount{0};
std::atomic<bool> g_pendingNearItems{false};
/* True entre l'envoi de RQ_GetNearItems (60) et la reponse inview (opcode 16) : permet de
 * traiter ce 16 comme un rafraichissement complet (purge sol + unites disparues), contrairement
 * aux bandes peripheriques poussees par le serveur pendant le deplacement. */
std::atomic<bool> g_expectInviewRefresh{false};
std::atomic<bool> g_pendingTeleport{false};
T4CPlayerTeleport g_teleportPayload{};
std::mutex g_teleportMutex;

/* Keep-alive : horodatage du dernier paquet envoye au serveur. Le port Linux (SDL3) ne
 * demarrait jamais le thread Windows COMM.StartSendAlive() — sans keep-alive, la connexion
 * UDP serveur (PACKET_BACKLOG_TIMEOUT = 15 s) expire pendant l'inactivite et tout paquet
 * suivant (les moves) est jete avant PacketInterpret. On renvoie l'opcode 10 (RQ_Ack)
 * toutes les ~3 s d'inactivite, comme SendAliveDataThread cote Windows. */
std::atomic<Uint32> g_lastClientSendTick{0};
constexpr Uint32 kKeepAliveIdleMs = 3000;

void SendEnterWorldFollowUpPackets();
void SendOpcodeOnly(short opcode);

bool IsWorldSessionReady() {
    return g_inGame.load() && g_fromPreInGameResult.load() >= 0;
}

void TryFlushPendingNearItems() {
    if (!g_pendingNearItems.load() || !IsWorldSessionReady()) {
        return;
    }
    g_pendingNearItems.store(false);
    g_expectInviewRefresh.store(true);
    SendOpcodeOnly(static_cast<short>(RQ_GetNearItems));
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                           "[PHASE] -> RQ_GetNearItems (60) apres opcode 46 OK");
}

bool WorldSessionReceiving() {
    return g_inGame.load() || g_waitingFromPreInGame.load();
}

void ReapFinishedLogoutThread() {
    std::lock_guard<std::mutex> tlock(g_logoutThreadMutex);
    if (g_logoutThread.joinable()) {
        g_logoutThread.join();
    }
}

bool SameLoginEndpoint(const std::string &hostField, const std::string &portField, short resolvedPort) {
    if (hostField != g_lastHost) {
        return false;
    }
    if (!portField.empty() && portField != g_lastPort) {
        return false;
    }
    if (portField.empty() && !g_lastPort.empty()) {
        const short saved = static_cast<short>(std::atoi(g_lastPort.c_str()));
        if (saved > 0 && saved != resolvedPort) {
            return false;
        }
    }
    return T4CCommBridgeIsOpen();
}

void MarkAuthPhaseSent() {
    g_authPhaseSentAt.store(SDL_GetTicks());
}

void FailAuthPhase(const char *message) {
    {
        std::lock_guard<std::mutex> lock(g_errMutex);
        g_authError = message;
    }
    g_pipelineStep.store(0);
    g_authPhaseSentAt.store(0);
    g_authPhaseRetryCount.store(0);
    g_registerSentAt.store(0);
}

void SendPacket(TFCPacket &pkt);
void SendPutPlayerInGame(const std::string &name);
void SendCreatePlayerPacket(const std::string &name, unsigned char sex, const unsigned char stats[5]);
void SendDeletePlayerPacket(const std::string &name);
void SendOpcodeOnly(short opcode);

/** Comme finalstep/client : RQ_SafePlug (123) + octet 0 — amorce deconnexion antiplug. */
void SendSafePlugLogout() {
    TFCPacket p;
    p << static_cast<short>(RQ_SafePlug);
    p << static_cast<char>(0);
    SendPacket(p);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Net, "[NET] Logout : RQ_SafePlug (123) envoye.");
}

void WaitForServerWorldLogoutDrain() {
    /* Laisser le serveur traiter ExitGame in_game avant un nouveau Register (meme port). */
    SDL_Delay(1200);
    for (int i = 0; i < 24; ++i) {
        T4CCommBridgePoll();
        SDL_Delay(50);
    }
}

static void LogoutWorker(int capturedPipelineStep) {
    if (capturedPipelineStep >= 5) {
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Net,
                               "[NET] Attente liberation compte serveur (~2 s apres ExitGame)…");
        SendOpcodeOnly(static_cast<short>(RQ_ExitGame));
        WaitForServerWorldLogoutDrain();
    } else if (capturedPipelineStep >= 2) {
        for (int i = 0; i < 8; ++i) {
            T4CCommBridgePoll();
            SDL_Delay(100);
        }
    }
    T4CCommBridgeStop();
    g_reconnectAllowedAfterTick.store(SDL_GetTicks() +
                                     static_cast<Uint32>(kPostLogoutReconnectCooldownSec) * 1000U);
    g_logoutSentThisSession.store(false);
    g_logoutInProgress.store(false);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Net,
                           "[NET] Deconnexion arriere-plan terminee — attendez ~%d s avant Connect "
                           "(le serveur libere la session sur l'ancien port UDP).",
                           kPostLogoutReconnectCooldownSec);
}

/** ExitGame ecran persos : fire-and-forget, socket UDP conserve pour reconnexion immediate. */
void SendCharSelectLogoutToServer() {
    SendOpcodeOnly(static_cast<short>(RQ_ExitGame));
    for (int i = 0; i < 6; ++i) {
        T4CCommBridgePoll();
    }
}

/** App.cpp AsyncClose : ExitGame + poll (~1 s). Uniquement en jeu (pas ecran persos). */
void SendGracefulExitToServer() {
    if (!g_inGame.load()) {
        return;
    }
    g_boQuitApp = false;
    for (int i = 0; i < 3; ++i) {
        SendOpcodeOnly(static_cast<short>(RQ_ExitGame));
        T4CCommBridgePoll();
        SDL_Delay(100);
    }
    SDL_Delay(500);
    for (int i = 0; i < 20; ++i) {
        T4CCommBridgePoll();
        SDL_Delay(50);
    }
}

void ResetLoginSessionState() {
    g_sessionKey = 0;
    g_selectedPlayer.clear();
    g_pendingEnterWorld.store(false);
    g_waitingPutPlayer.store(false);
    g_putPlayerSentAt.store(0);
    g_putPlayerRetryCount.store(0);
    g_waitingCreate.store(false);
    g_createSentAt.store(0);
    g_createRetryCount.store(0);
    g_waitingDelete.store(false);
    g_deleteSentAt.store(0);
    g_deleteRetryCount.store(0);
    g_pendingDeleteAfterAuth.store(false);
    g_pendingCreateAfterAuth.store(false);
    g_waitingReroll.store(false);
    g_inCreateRerollPhase.store(false);
    g_inGame.store(false);
    g_attackMode.store(false);
    g_pendingCharacterList.store(false);
    g_pendingAutoRegister.store(false);
    g_registerRetryCount.store(0);
    g_registerSentAt.store(0);
    g_authPhaseRetryCount.store(0);
    g_authPhaseSentAt.store(0);
    g_popupPending.store(false);
    g_localMovePending.store(false);
    g_waitingFromPreInGame.store(false);
    g_fromPreInGameResult.store(-1);
    g_enterWorld46SentAt.store(0);
    g_enterWorld46RetryCount.store(0);
    g_pendingNearItems.store(false);
    g_expectInviewRefresh.store(false);
    {
        std::lock_guard<std::mutex> lock(g_activeMutex);
        g_activePlayer = {};
    }
    {
        std::lock_guard<std::mutex> lock(g_charMutex);
        g_characters.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_inventoryMutex);
        g_backpack = {};
        g_equipment = {};
        g_skillBook = {};
        g_spellBook = {};
    }
    {
        std::lock_guard<std::mutex> lock(g_chatMutex);
        g_chatMessages.clear();
    }
    g_inventoryDirty.store(false);
    g_playerDead.store(false);
    g_deathPending.store(false);
    g_deathResurrectSent.store(false);
    {
        std::lock_guard<std::mutex> lock(g_deathMutex);
        g_deathState = {};
    }
    {
        std::lock_guard<std::mutex> lock(g_damageMutex);
        g_floatingDamage.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_talkMutex);
        g_talkState = {};
        g_pendingSpeech = {};
        g_shopList = {};
    }
    g_speechPending.store(false);
    g_shopDirty.store(false);
    T4CLoginSessionClearRemoteUnits();
}

void ApplyRaceToPuppetAppearance(T4CActivePlayer *player) {
    if (!player) {
        return;
    }
    if (player->race >= 10001 && player->race <= 10004) {
        player->appearance = 10011;
        player->classIndex = static_cast<std::uint8_t>(player->race - 10001);
        player->female = false;
    } else if (player->race >= 15001 && player->race <= 15004) {
        player->appearance = 10012;
        player->classIndex = static_cast<std::uint8_t>(player->race - 15001);
        player->female = true;
    }
}

void SkipOpcode(TFCPacket *msg) {
    msg->Seek(0, 0);
    short ignored = 0;
    msg->Get(&ignored);
}

bool IsFemaleAppearance(std::uint16_t appearance) {
    return appearance == 10012 || appearance == 15012 || (appearance >= 15001 && appearance <= 15004);
}

bool IsActualPlayerAppearance(std::uint16_t appearance) {
    return (appearance >= 10001 && appearance <= 10004) || (appearance >= 15001 && appearance <= 15004);
}

bool IsPuppetAppearance(std::uint16_t appearance) {
    return appearance == 10011 || appearance == 10012 || appearance == 15011 || appearance == 15012;
}

/** Remap apparences joueur template -> modele creature (Packet.cpp TFCMoveID / TFCAddObject). */
std::uint16_t NormalizeServerAppearance(std::uint16_t appearance) {
    switch (appearance) {
        case 10002:
            return 20039;
        case 15002:
            return 25039;
        case 10004:
            return 20042;
        case 15004:
            return 25042;
        case 10001:
            return 20043;
        case 15001:
            return 25043;
        case 10003:
            return 20044;
        case 15003:
            return 25044;
        default:
            return appearance;
    }
}

void RequestPuppetInformation(std::int32_t unitId, unsigned int x, unsigned int y) {
    if (unitId == 0) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_remoteMutex);
        const auto it = g_remotePuppet.find(unitId);
        if (it != g_remotePuppet.end() && it->second.known) {
            return;
        }
        g_pendingPuppetRequest[unitId] = {x, y};
    }
    TFCPacket p;
    p << static_cast<short>(RQ_PuppetInformation);
    p << static_cast<long>(unitId);
    p << static_cast<short>(x);
    p << static_cast<short>(y);
    SendPacket(p);
}

void RequestUnitName(std::int32_t unitId, unsigned int x, unsigned int y) {
    if (unitId == 0) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_remoteMutex);
        if (g_unitNames.count(unitId) != 0) {
            return;
        }
        const auto now = std::chrono::steady_clock::now();
        const auto it = g_pendingUnitName.find(unitId);
        if (it != g_pendingUnitName.end() && now - it->second < std::chrono::seconds(3)) {
            return;
        }
        g_pendingUnitName[unitId] = now;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_GetUnitName);
    p << static_cast<long>(unitId);
    p << static_cast<short>(x);
    p << static_cast<short>(y);
    SendPacket(p);
}

void ApplyPuppetDressToPlayer(T4CActivePlayer *player, const T4CPuppetDress &dress) {
    if (!player) {
        return;
    }
    player->puppetBody = dress.body;
    player->puppetFeet = dress.feet;
    player->puppetGloves = dress.gloves;
    player->puppetHelm = dress.helm;
    player->puppetLegs = dress.legs;
    player->puppetWeaponR = dress.weaponR;
    player->puppetWeaponL = dress.weaponL;
    player->puppetCape = dress.cape;
    player->puppetKnown = dress.known;
}

bool IsRemoteDrawableUnit(std::uint16_t appearance) {
    // Plage des unites (joueurs 10001-10012, PNJ 10005-10010, cadavres 15xxx,
    // monstres 20xxx, cadavres monstres 25xxx). Les objets au sol (< 10000) sont
    // exclus. Apparence mappee -> sprite ; sinon marqueur (au moins visible).
    return appearance >= 10001 && appearance <= 29999;
}

bool IsGroundWorldObject(std::uint16_t appearance) {
    return appearance > 0 && appearance < 10001;
}

void UpsertGroundMarker(unsigned int x, unsigned int y, std::uint16_t appearance, std::int32_t unitId) {
    if (unitId == 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    for (auto &marker : g_groundMarkers) {
        if (marker.unitId == unitId) {
            marker.x = x;
            marker.y = y;
            marker.appearance = appearance;
            return;
        }
    }
    g_groundMarkers.push_back({unitId, appearance, x, y});
}

/** Met a jour l'apparence d'un marqueur sol existant (porte ouverte/fermee via opcode 12). */
bool UpdateGroundMarkerAppearance(std::int32_t unitId, std::uint16_t appearance) {
    if (unitId == 0) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    for (auto &marker : g_groundMarkers) {
        if (marker.unitId == unitId) {
            marker.appearance = appearance;
            return true;
        }
    }
    return false;
}

/* Borne la croissance de g_groundMarkers : retire le sol trop loin du joueur (le serveur n'envoie
 * pas toujours un retrait explicite quand on s'eloigne). Marge large > cull rendu (90). */
void PruneFarGroundMarkers() {
    unsigned int px = 0;
    unsigned int py = 0;
    {
        std::lock_guard<std::mutex> lock(g_activeMutex);
        if (!g_activePlayer.valid) {
            return;
        }
        px = g_activePlayer.serverX;
        py = g_activePlayer.serverY;
    }
    constexpr int kPruneRange = 130;
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    g_groundMarkers.erase(
        std::remove_if(g_groundMarkers.begin(), g_groundMarkers.end(),
                       [px, py](const T4CGroundObjectMarker &m) {
                           const int dx = static_cast<int>(m.x) - static_cast<int>(px);
                           const int dy = static_cast<int>(m.y) - static_cast<int>(py);
                           return std::abs(dx) > kPruneRange || std::abs(dy) > kPruneRange;
                       }),
        g_groundMarkers.end());
}

void RemoveGroundMarker(std::int32_t unitId) {
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    g_groundMarkers.erase(
        std::remove_if(g_groundMarkers.begin(), g_groundMarkers.end(),
                       [unitId](const T4CGroundObjectMarker &m) { return m.unitId == unitId; }),
        g_groundMarkers.end());
}

void QueueLocalPlayerMove(const unsigned int x, const unsigned int y, const std::uint16_t moveOpcode) {
    {
        std::lock_guard<std::mutex> lock(g_localMoveMutex);
        g_localMoveX = x;
        g_localMoveY = y;
        g_localMoveOpcode = moveOpcode;
    }
    g_localMovePending.store(true);
}

bool IsLocalPlayerUnitId(std::int32_t unitId) {
    std::lock_guard<std::mutex> lock(g_activeMutex);
    if (!g_activePlayer.valid || g_activePlayer.unitId == 0) {
        return false;
    }
    return static_cast<std::uint32_t>(unitId) ==
           static_cast<std::uint32_t>(g_activePlayer.unitId);
}

bool IsLocalPlayerMoveTarget(const int sx, const int sy) {
    std::lock_guard<std::mutex> lock(g_activeMutex);
    if (!g_activePlayer.valid) {
        return false;
    }
    const int dx = sx - static_cast<int>(g_activePlayer.serverX);
    const int dy = sy - static_cast<int>(g_activePlayer.serverY);
    return dx >= -1 && dx <= 1 && dy >= -1 && dy <= 1;
}

void PushRemoteEvent(T4CRemoteUnitEvent ev) {
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    g_remoteEvents.push_back(ev);
}

void RememberRemoteAppearance(std::int32_t unitId, std::uint16_t appearance) {
    if (unitId != 0 && appearance != 0) {
        std::lock_guard<std::mutex> lock(g_remoteMutex);
        g_remoteAppearance[unitId] = appearance;
    }
}

void QueueRemoteSpawn(unsigned int x, unsigned int y, std::uint16_t appearance, std::int32_t unitId,
                      char hpPercent) {
    appearance = NormalizeServerAppearance(appearance);
    if (!IsRemoteDrawableUnit(appearance) ||
        T4CLoginSessionShouldSkipRemoteUnit(appearance, unitId, x, y)) {
        return;
    }
    RememberRemoteAppearance(unitId, appearance);
    /* Nom reel demande des l'apparition : label correct au 1er clic (Mithrand, Garde…). */
    if (!IsLocalPlayerUnitId(unitId)) {
        RequestUnitName(unitId, x, y);
    }
    if (IsPuppetAppearance(appearance)) {
        RequestPuppetInformation(unitId, x, y);
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                               "[PHASE] puppet spawn id=%d app=%u @ %u,%u", unitId,
                               static_cast<unsigned>(appearance), x, y);
    }
    T4CRemoteUnitEvent ev{};
    ev.kind = T4CRemoteUnitEventKind::Spawn;
    ev.unitId = unitId;
    ev.appearance = appearance;
    ev.x = x;
    ev.y = y;
    ev.hpPercent = hpPercent;
    PushRemoteEvent(ev);
}

void QueueRemoteMove(unsigned int x, unsigned int y, std::uint16_t unitId, std::uint16_t moveOpcode,
                     std::uint16_t appearanceHint) {
    if (IsLocalPlayerUnitId(unitId)) {
        return;
    }
    if (appearanceHint != 0) {
        appearanceHint = NormalizeServerAppearance(appearanceHint);
        RememberRemoteAppearance(unitId, appearanceHint);
    }
    std::uint16_t appearance = appearanceHint;
    bool known = appearanceHint != 0;
    if (!known) {
        std::lock_guard<std::mutex> lock(g_remoteMutex);
        const auto it = g_remoteAppearance.find(unitId);
        if (it != g_remoteAppearance.end()) {
            appearance = it->second;
            known = true;
        }
    }
    // Apparence connue mais non dessinable (objet au sol) : on ignore. Apparence
    // inconnue : on laisse passer (sera resolue au prochain popup/update).
    if (known && !IsRemoteDrawableUnit(appearance)) {
        return;
    }
    T4CRemoteUnitEvent ev{};
    ev.kind = T4CRemoteUnitEventKind::Move;
    ev.unitId = unitId;
    ev.appearance = appearance;
    ev.x = x;
    ev.y = y;
    ev.moveOpcode = moveOpcode;
    PushRemoteEvent(ev);
}

void QueueRemoteRemove(std::int32_t unitId) {
    if (unitId == 0 || IsLocalPlayerUnitId(unitId)) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_remoteMutex);
        g_remoteAppearance.erase(unitId);
        g_remotePuppet.erase(unitId);
        g_pendingPuppetRequest.erase(unitId);
    }
    T4CRemoteUnitEvent ev{};
    ev.kind = T4CRemoteUnitEventKind::Remove;
    ev.unitId = unitId;
    PushRemoteEvent(ev);
}

bool ParseUnitInfo(TFCPacket *msg, std::uint16_t *appearance, std::int32_t *unitId, char *hpPercent) {
    if (!msg || !appearance || !unitId) {
        return false;
    }
    // Layout serveur (Unit::PacketUnitInformation) :
    //   long  dwFVal(4) | short friendlyID(2) | short appearance(2) | long id(4)
    //   | char radiance(1) | char status(1) | char hp%(1) | char combatMode(1) | char hiddenInv2(1)
    long fval = 0;
    short friendlyId = 0;
    short app = 0;
    long id = 0;
    char rad = 0;
    char status = 0;
    char hp = 0;
    char combatMode = 0;
    char hiddenInv2 = 0;
    msg->Get(&fval);
    msg->Get(&friendlyId);
    msg->Get(&app);
    msg->Get(&id);
    msg->Get(&rad);
    msg->Get(&status);
    msg->Get(&hp);
    msg->Get(&combatMode);
    msg->Get(&hiddenInv2);
    *appearance = static_cast<std::uint16_t>(app);
    *unitId = static_cast<std::int32_t>(id);
    if (hpPercent) {
        *hpPercent = hp;
    }
    return true;
}

bool ReadPacketString(TFCPacket *msg, std::string *out) {
    if (!msg || !out) {
        return false;
    }
    short len = 0;
    msg->Get(&len);
    if (len < 0) {
        return false;
    }
    out->clear();
    out->reserve(static_cast<size_t>(len));
    for (int i = 0; i < len; ++i) {
        char ch = 0;
        msg->Get(&ch);
        out->push_back(ch);
    }
    return true;
}

bool ParseBagItem(TFCPacket *msg, T4CBagItem *out) {
    if (!msg || !out) {
        return false;
    }
    short appearance = 0;
    long objectId = 0;
    short baseId = 0;
    long qty = 0;
    long charges = 0;
    char chtarget = 0;
    char chAttack = 0;
    char chPvp = 0;
    msg->Get(&appearance);
    msg->Get(&objectId);
    msg->Get(&baseId);
    msg->Get(&qty);
    msg->Get(&charges);
    msg->Get(&chtarget);
    msg->Get(&chAttack);
    msg->Get(&chPvp);
    out->appearance = static_cast<std::uint16_t>(appearance);
    out->objectId = static_cast<std::int32_t>(objectId);
    out->baseId = static_cast<std::uint16_t>(baseId);
    out->qty = static_cast<std::uint32_t>(qty);
    out->charges = static_cast<std::int32_t>(charges);
    return true;
}

bool ParseEquipItem(TFCPacket *msg, T4CEquippedItem *out, const T4CEquipSlot slot) {
    if (!msg || !out) {
        return false;
    }
    long itemId = 0;
    short appear = 0;
    short baseId = 0;
    short qty = 0;
    long charges = 0;
    msg->Get(&itemId);
    msg->Get(&appear);
    msg->Get(&baseId);
    msg->Get(&qty);
    msg->Get(&charges);
    std::string name;
    ReadPacketString(msg, &name);
    out->slot = slot;
    out->objectId = static_cast<std::int32_t>(itemId);
    out->appearance = static_cast<std::uint16_t>(appear);
    out->baseId = static_cast<std::uint16_t>(baseId);
    out->qty = static_cast<std::uint16_t>(qty);
    out->charges = static_cast<std::int32_t>(charges);
    out->name = std::move(name);
    return true;
}

void ApplyEquipmentToLocalPuppet(const T4CPlayerEquipment &equipment) {
    T4CPuppetDress dress{};
    for (const T4CEquippedItem &item : equipment.items) {
        if (item.objectId == 0) {
            continue;
        }
        switch (item.slot) {
            case T4CEquipSlot::Body:
                dress.body = item.appearance;
                break;
            case T4CEquipSlot::Gloves:
                dress.gloves = item.appearance;
                break;
            case T4CEquipSlot::Helm:
                dress.helm = item.appearance;
                break;
            case T4CEquipSlot::Legs:
                dress.legs = item.appearance;
                break;
            case T4CEquipSlot::Feet:
                dress.feet = item.appearance;
                break;
            case T4CEquipSlot::WeaponRight:
                dress.weaponR = item.appearance;
                break;
            case T4CEquipSlot::WeaponLeft:
                dress.weaponL = item.appearance;
                break;
            case T4CEquipSlot::Cape:
                dress.cape = item.appearance;
                break;
            default:
                break;
        }
    }
    dress.known = true;
    std::lock_guard<std::mutex> lock(g_activeMutex);
    if (g_activePlayer.valid) {
        ApplyPuppetDressToPlayer(&g_activePlayer, dress);
        ApplyRaceToPuppetAppearance(&g_activePlayer);
    }
}

void QueueRemoteAttack(std::int32_t unitId, unsigned int x, unsigned int y, char hpPercent, unsigned int targetX,
                       unsigned int targetY, const bool hasTarget) {
    if (unitId == 0 || IsLocalPlayerUnitId(unitId)) {
        return;
    }
    T4CRemoteUnitEvent ev{};
    ev.kind = T4CRemoteUnitEventKind::Attack;
    ev.unitId = unitId;
    ev.x = x;
    ev.y = y;
    ev.hpPercent = hpPercent;
    if (hasTarget) {
        ev.targetX = targetX;
        ev.targetY = targetY;
        ev.hasAttackTarget = true;
    }
    PushRemoteEvent(ev);
}

static std::uint16_t LookupRemoteAppearance(const std::int32_t unitId) {
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    const auto it = g_remoteAppearance.find(unitId);
    return it != g_remoteAppearance.end() ? it->second : 0;
}

static bool IsRemoteUnitTracked(const std::int32_t unitId) {
    return LookupRemoteAppearance(unitId) != 0;
}

/** Demande au serveur les infos d'une unite absente du client (Windows Packet.cpp opcode 71). */
void RequestMissingUnit(const std::int32_t unitId, const short x, const short y) {
    if (unitId == 0 || IsLocalPlayerUnitId(unitId)) {
        return;
    }
    TFCPacket p;
    p << static_cast<short>(71);
    p << static_cast<long>(unitId);
    p << x;
    p << y;
    SendPacket(p);
}

void HandleViewBackpack(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    char display = 0;
    long containerId = 0;
    short nbObjects = 0;
    msg->Get(&display);
    msg->Get(&containerId);
    msg->Get(&nbObjects);
    T4CPlayerBackpack backpack{};
    backpack.showUi = display != 0;
    backpack.containerId = static_cast<std::int32_t>(containerId);
    backpack.items.reserve(static_cast<size_t>(nbObjects));
    for (int i = 0; i < nbObjects; ++i) {
        T4CBagItem item{};
        if (!ParseBagItem(msg, &item)) {
            break;
        }
        backpack.items.push_back(item);
    }
    backpack.valid = true;
    {
        std::lock_guard<std::mutex> lock(g_inventoryMutex);
        g_backpack = std::move(backpack);
    }
    g_inventoryDirty.store(true);
}

void HandleViewEquipped(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    char rangedAttack = 0;
    msg->Get(&rangedAttack);
    T4CPlayerEquipment equipment{};
    equipment.rangedAttack = rangedAttack != 0;
    const T4CEquipSlot slots[] = {
        T4CEquipSlot::Body,        T4CEquipSlot::Gloves,      T4CEquipSlot::Helm,
        T4CEquipSlot::Legs,        T4CEquipSlot::Bracelets,   T4CEquipSlot::Necklace,
        T4CEquipSlot::WeaponRight, T4CEquipSlot::WeaponLeft,  T4CEquipSlot::Ring1,
        T4CEquipSlot::Ring2,       T4CEquipSlot::Belt,        T4CEquipSlot::Cape,
        T4CEquipSlot::Feet,
    };
    equipment.items.reserve(13);
    for (const T4CEquipSlot slot : slots) {
        T4CEquippedItem item{};
        if (!ParseEquipItem(msg, &item, slot)) {
            break;
        }
        if (item.objectId != 0) {
            equipment.items.push_back(item);
        }
    }
    {
        T4CEquippedItem orb{};
        ParseEquipItem(msg, &orb, T4CEquipSlot::Ring1);
    }
    equipment.valid = true;
    {
        std::lock_guard<std::mutex> lock(g_inventoryMutex);
        g_equipment = std::move(equipment);
        ApplyEquipmentToLocalPuppet(g_equipment);
    }
    g_inventoryDirty.store(true);
}

void HandleDamageOrHeal(TFCPacket *msg, const bool healing) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long unitId = 0;
    short len = 0;
    msg->Get(&unitId);
    msg->Get(&len);
    std::string text;
    const int safeLen = std::min<int>(len, 99);
    text.reserve(static_cast<size_t>(safeLen));
    for (int i = 0; i < safeLen; ++i) {
        char ch = 0;
        msg->Get(&ch);
        text.push_back(ch);
    }
    for (int i = safeLen; i < len; ++i) {
        char ch = 0;
        msg->Get(&ch);
    }
    unsigned long color = 0;
    msg->Get(reinterpret_cast<long *>(&color));
    if (!text.empty()) {
        T4CFloatingDamage dmg{};
        dmg.unitId = static_cast<std::int32_t>(unitId);
        dmg.text = healing ? ("+" + text) : text;
        dmg.color = static_cast<std::uint32_t>(color);
        std::lock_guard<std::mutex> lock(g_damageMutex);
        g_floatingDamage.push_back(std::move(dmg));
    }
}

void HandleAttackResult(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long idAttack = 0;
    long idDefend = 0;
    char typeAttack = 0;
    char typeDefend = 0;
    char hpPercent = 0;
    char combatMode = 0;
    short ax = 0;
    short ay = 0;
    short dx = 0;
    short dy = 0;
    msg->Get(&idAttack);
    msg->Get(&idDefend);
    msg->Get(&typeAttack);
    msg->Get(&typeDefend);
    msg->Get(&hpPercent);
    msg->Get(&combatMode);
    msg->Get(&ax);
    msg->Get(&ay);
    msg->Get(&dx);
    msg->Get(&dy);
    (void)typeAttack;
    (void)typeDefend;
    (void)combatMode;
    const unsigned uax = static_cast<unsigned>(ax);
    const unsigned uay = static_cast<unsigned>(ay);
    const unsigned udx = static_cast<unsigned>(dx);
    const unsigned udy = static_cast<unsigned>(dy);
    const std::uint16_t attackerAppearance = LookupRemoteAppearance(static_cast<std::int32_t>(idAttack));
    const std::uint16_t defenderAppearance = LookupRemoteAppearance(static_cast<std::int32_t>(idDefend));
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                           "[PHASE] <- attaque (10001) att=%ld(app=%u) def=%ld(app=%u) hp=%d",
                           static_cast<long>(idAttack), static_cast<unsigned>(attackerAppearance),
                           static_cast<long>(idDefend), static_cast<unsigned>(defenderAppearance),
                           static_cast<int>(hpPercent));

    const char *attackerSprite = T4CSpriteNameFromAppearance(attackerAppearance);
    const char *defenderSprite = T4CSpriteNameFromAppearance(defenderAppearance);
    if (!IsLocalPlayerUnitId(idAttack)) {
        if (!IsRemoteUnitTracked(static_cast<std::int32_t>(idAttack))) {
            RequestMissingUnit(static_cast<std::int32_t>(idAttack), ax, ay);
        }
        T4CGameMusic::PlayCreatureAttackSfx(attackerSprite);
        QueueRemoteAttack(static_cast<std::int32_t>(idAttack), uax, uay, hpPercent, udx, udy, true);
    } else {
        /* Le joueur attaque : whoosh + animation de swing locale (vers le defenseur). */
        T4CGameMusic::PlayCreatureAttackSfx(nullptr);
        std::lock_guard<std::mutex> lock(g_localAttackMutex);
        g_localAttackPending = true;
        g_localAttackTargetX = udx;
        g_localAttackTargetY = udy;
    }
    if (!IsLocalPlayerUnitId(idDefend) && idDefend != 0 &&
        !IsRemoteUnitTracked(static_cast<std::int32_t>(idDefend))) {
        RequestMissingUnit(static_cast<std::int32_t>(idDefend), dx, dy);
    }
    if (IsLocalPlayerUnitId(idDefend)) {
        T4CGameMusic::PlayPlayerHitSfx();
    } else if (defenderSprite) {
        T4CGameMusic::PlayCreatureHitSfx(defenderSprite);
    }
    if (!IsLocalPlayerUnitId(idDefend)) {
        T4CRemoteUnitEvent ev{};
        ev.kind = T4CRemoteUnitEventKind::Update;
        ev.unitId = static_cast<std::int32_t>(idDefend);
        ev.x = udx;
        ev.y = udy;
        ev.hpPercent = hpPercent;
        PushRemoteEvent(ev);
    }
}

/* Opcode 10002 __EVENT_MISS : attaque ratee (tres frequente). On joue quand meme l'animation
 * + le whoosh de l'attaquant pour que le combat reste vivant (le client Windows fait pareil). */
void HandleAttackMiss(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long idAttack = 0;
    long idDefend = 0;
    short ax = 0;
    short ay = 0;
    short dx = 0;
    short dy = 0;
    msg->Get(&idAttack);
    msg->Get(&idDefend);
    msg->Get(&ax);
    msg->Get(&ay);
    msg->Get(&dx);
    msg->Get(&dy);
    const std::uint16_t attackerAppearance = LookupRemoteAppearance(static_cast<std::int32_t>(idAttack));
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- attaque ratee (10002) att=%ld def=%ld",
                           static_cast<long>(idAttack), static_cast<long>(idDefend));
    if (!IsLocalPlayerUnitId(idAttack)) {
        if (!IsRemoteUnitTracked(static_cast<std::int32_t>(idAttack))) {
            RequestMissingUnit(static_cast<std::int32_t>(idAttack), ax, ay);
        }
        T4CGameMusic::PlayCreatureAttackSfx(T4CSpriteNameFromAppearance(attackerAppearance));
        /* hpPercent=0 => l'animation d'attaque se joue sans modifier la barre de vie (miss). */
        QueueRemoteAttack(static_cast<std::int32_t>(idAttack), static_cast<unsigned>(ax),
                          static_cast<unsigned>(ay), 0, static_cast<unsigned>(dx),
                          static_cast<unsigned>(dy), true);
    } else {
        T4CGameMusic::PlayCreatureAttackSfx(nullptr);
        std::lock_guard<std::mutex> lock(g_localAttackMutex);
        g_localAttackPending = true;
        g_localAttackTargetX = static_cast<unsigned>(dx);
        g_localAttackTargetY = static_cast<unsigned>(dy);
    }
    if (!IsLocalPlayerUnitId(idDefend) && idDefend != 0 &&
        !IsRemoteUnitTracked(static_cast<std::int32_t>(idDefend))) {
        RequestMissingUnit(static_cast<std::int32_t>(idDefend), dx, dy);
    }
}

void HandleYouDied(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    g_playerDead.store(true);
    g_deathPending.store(true);
    T4CGameMusic::StartCharacterSelect();
    {
        std::lock_guard<std::mutex> lock(g_deathMutex);
        g_deathState.active = true;
        g_deathState.canResurrect = false;
    }
}

void HandleNmDeathStatus(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    char chStatus = 0;
    msg->Get(&chStatus);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- RQ_NM_DeathStatus (200) status=%d",
                           static_cast<int>(chStatus));
    if (chStatus == 0) {
        g_playerDead.store(false);
        g_deathPending.store(false);
        g_deathResurrectSent.store(false);
        std::lock_guard<std::mutex> lock(g_deathMutex);
        g_deathState = {};
    } else {
        g_playerDead.store(true);
        g_deathPending.store(true);
        std::lock_guard<std::mutex> lock(g_deathMutex);
        g_deathState.active = true;
        g_deathState.canResurrect = false;
    }
}

void HandleNmDeathProgress(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    short wtCurrent = 0;
    short wtTotal = 0;
    char chCanResurrect = 0;
    msg->Get(&wtCurrent);
    msg->Get(&wtTotal);
    msg->Get(&chCanResurrect);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                           "[PHASE] <- RQ_NM_DeathProgress (201) %d/%d canRes=%d",
                           static_cast<int>(wtCurrent), static_cast<int>(wtTotal),
                           static_cast<int>(chCanResurrect));
    {
        std::lock_guard<std::mutex> lock(g_deathMutex);
        g_deathState.active = true;
        g_deathState.timeCurrent = static_cast<std::uint16_t>(wtCurrent);
        g_deathState.timeTotal = static_cast<std::uint16_t>(wtTotal);
        g_deathState.canResurrect = chCanResurrect != 0;
    }
    if (chCanResurrect != 0 && !g_deathResurrectSent.exchange(true)) {
        TFCPacket packet;
        packet << static_cast<short>(RQ_NM_DeathResurect);
        SendPacket(packet);
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] -> RQ_NM_DeathResurect (202)");
    }
}

void ExtractBracketLinks(const std::string &text, std::vector<std::string> *outLinks) {
    if (!outLinks) {
        return;
    }
    std::size_t pos = 0;
    while ((pos = text.find('[', pos)) != std::string::npos) {
        const std::size_t end = text.find(']', pos);
        if (end == std::string::npos) {
            break;
        }
        if (end > pos + 1) {
            outLinks->push_back(text.substr(pos + 1, end - pos - 1));
        }
        pos = end + 1;
    }
}

void SetTalkTarget(const std::int32_t unitId, const unsigned int x, const unsigned int y) {
    std::lock_guard<std::mutex> lock(g_talkMutex);
    g_talkState.unitId = unitId;
    g_talkState.x = x;
    g_talkState.y = y;
    g_talkState.active = unitId != 0;
}

void HandleIndirectTalk(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long unitId = 0;
    char direction = 0;
    long color = 0;
    char isNpc = 0;
    msg->Get(&unitId);
    msg->Get(&direction);
    msg->Get(&color);
    msg->Get(&isNpc);
    short textLen = 0;
    msg->Get(&textLen);
    std::string text;
    const int safeLen = std::min<int>(textLen, 4095);
    text.reserve(static_cast<size_t>(safeLen));
    for (int i = 0; i < safeLen; ++i) {
        char ch = 0;
        msg->Get(&ch);
        text.push_back(ch);
    }
    for (int i = safeLen; i < textLen; ++i) {
        char ch = 0;
        msg->Get(&ch);
    }
    short nameLen = 0;
    msg->Get(&nameLen);
    std::string name;
    const int safeName = std::min<int>(nameLen, 255);
    for (int i = 0; i < safeName; ++i) {
        char ch = 0;
        msg->Get(&ch);
        name.push_back(ch);
    }
    for (int i = safeName; i < nameLen; ++i) {
        char ch = 0;
        msg->Get(&ch);
    }
    unsigned long nameColor = 0;
    msg->Get(reinterpret_cast<long *>(&nameColor));
    (void)color;
    (void)nameColor;

    T4CNpcSpeech speech{};
    speech.unitId = static_cast<std::int32_t>(unitId);
    speech.text = std::move(text);
    speech.speakerName = std::move(name);
    speech.direction = static_cast<int>(direction);
    speech.isNpc = isNpc != 0;
    ExtractBracketLinks(speech.text, &speech.links);

    {
        std::lock_guard<std::mutex> lock(g_talkMutex);
        g_pendingSpeech = speech;
        if (speech.isNpc && speech.unitId != 0) {
            g_talkState.unitId = speech.unitId;
            g_talkState.active = true;
        }
    }
    g_speechPending.store(true);

    /* Historique chat (V3_ChatLogDlg Windows) : toute parole locale, PNJ ou joueur. */
    T4CChatMessage chat{};
    chat.kind = T4CChatKind::Local;
    chat.speaker = speech.speakerName;
    chat.text = speech.text;
    chat.color = static_cast<std::uint32_t>(color);
    chat.unitId = speech.unitId;
    PushChatMessage(std::move(chat));
}

/** Opcode 29 RQ_Page entrant : message prive. */
void HandlePageMessage(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    std::string name;
    std::string text;
    ReadPacketString(msg, &name);
    ReadPacketString(msg, &text);
    if (text.empty()) {
        return;
    }
    T4CChatMessage chat{};
    chat.kind = T4CChatKind::Page;
    chat.speaker = name.empty() ? std::string("(page)") : name;
    chat.text = std::move(text);
    chat.color = 0x80C0FFu;
    PushChatMessage(std::move(chat));
}

/** Opcode 63 RQ_ServerMessage : texte serveur (position ignoree, va au log). */
void HandleServerMessage(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    short wx = 0;
    short wy = 0;
    msg->Get(&wx);
    msg->Get(&wy);
    std::string text;
    ReadPacketString(msg, &text);
    unsigned long color = 0x00FF80u;
    if (!msg->isEnd()) {
        msg->Get(reinterpret_cast<long *>(&color));
    }
    if (text.empty()) {
        return;
    }
    T4CChatMessage chat{};
    chat.kind = T4CChatKind::System;
    chat.text = std::move(text);
    chat.color = static_cast<std::uint32_t>(color);
    PushChatMessage(std::move(chat));
}

/** Opcode 102 RQ_InfoMessage : info popup serveur. */
void HandleInfoMessage(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long type = 0;
    long color = 0;
    msg->Get(&type);
    msg->Get(&color);
    std::string text;
    ReadPacketString(msg, &text);
    (void)type;
    if (text.empty()) {
        return;
    }
    T4CChatMessage chat{};
    chat.kind = T4CChatKind::System;
    chat.text = std::move(text);
    chat.color = static_cast<std::uint32_t>(color);
    PushChatMessage(std::move(chat));
}

/** Opcode 45 RQ_GetTime : heure du monde (Packet.cpp L2883). */
void HandleGetTime(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    char sec = 0;
    char minute = 0;
    char hour = 0;
    char day = 0;
    char week = 0;
    char month = 0;
    short year = 0;
    msg->Get(&sec);
    msg->Get(&minute);
    msg->Get(&hour);
    msg->Get(&day);
    msg->Get(&week);
    msg->Get(&month);
    msg->Get(&year);
    std::lock_guard<std::mutex> lock(g_timeMutex);
    g_gameTime.second = static_cast<unsigned char>(sec);
    g_gameTime.minute = static_cast<unsigned char>(minute);
    g_gameTime.hour = static_cast<unsigned char>(hour);
    g_gameTime.day = static_cast<unsigned char>(day);
    g_gameTime.week = static_cast<unsigned char>(week);
    g_gameTime.month = static_cast<unsigned char>(month);
    g_gameTime.year = static_cast<unsigned short>(year);
    g_gameTime.valid = true;
}

/** Opcode 198 RQ_AttackMode : confirmation mode combat (Packet.cpp L5650 — Ctrl+C Windows). */
void HandleAttackModeReply(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long ask = 0;
    long set = 0;
    msg->Get(&ask);
    if (!msg->isEnd()) {
        msg->Get(&set);
    }
    if (set > 0) {
        const bool enabled = ask != 0;
        g_attackMode.store(enabled);
        T4CChatMessage chat{};
        chat.kind = T4CChatKind::System;
        chat.text = enabled ? "Mode attaque ACTIVE — clic = attaquer (PNJ inclus)."
                            : "Mode attaque desactive.";
        chat.color = enabled ? 0xFF6040u : 0x80FF80u;
        PushChatMessage(std::move(chat));
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- RQ_AttackMode (198) ask=%ld set=%ld",
                               ask, set);
    }
}

/** Opcode 39 RQ_GetSkillList : livre de competences (Packet.cpp L2919). */
void HandleSkillListReply(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    short count = 0;
    msg->Get(&count);
    T4CPlayerSkillBook book{};
    book.skills.reserve(static_cast<size_t>(std::max<short>(count, 0)));
    for (int i = 0; i < count; ++i) {
        T4CPlayerSkill skill{};
        short skillId = 0;
        char useMode = 0;
        short points = 0;
        short truePoints = 0;
        msg->Get(&skillId);
        msg->Get(&useMode);
        msg->Get(&points);
        msg->Get(&truePoints);
        ReadPacketString(msg, &skill.name);
        ReadPacketString(msg, &skill.description);
        skill.skillId = static_cast<std::uint16_t>(skillId);
        skill.useMode = static_cast<unsigned char>(useMode);
        skill.points = static_cast<std::uint16_t>(points);
        skill.truePoints = static_cast<std::uint16_t>(truePoints);
        book.skills.push_back(std::move(skill));
    }
    book.valid = true;
    {
        std::lock_guard<std::mutex> lock(g_inventoryMutex);
        g_skillBook = std::move(book);
    }
    g_inventoryDirty.store(true);
}

/** Opcode 62 RQ_SendSpellList : grimoire (Packet.cpp L3096). */
void HandleSpellListReply(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    char update = 0;
    short mana = 0;
    short maxMana = 0;
    short count = 0;
    msg->Get(&update);
    msg->Get(&mana);
    msg->Get(&maxMana);
    msg->Get(&count);
    (void)update;
    T4CPlayerSpellBook book{};
    book.mana = static_cast<unsigned short>(mana);
    book.maxMana = static_cast<unsigned short>(maxMana);
    for (int i = 0; i < count; ++i) {
        short spellId = 0;
        msg->Get(&spellId);
        if (spellId == 0) {
            continue;
        }
        T4CPlayerSpell spell{};
        char targetType = 0;
        char attackSpell = 0;
        short manaCost = 0;
        long duration = 0;
        short level = 0;
        short element = 0;
        short damageType = 0;
        long icon = 0;
        msg->Get(&targetType);
        msg->Get(&attackSpell);
        msg->Get(&manaCost);
        msg->Get(&duration);
        msg->Get(&level);
        msg->Get(&element);
        msg->Get(&damageType);
        msg->Get(&icon);
        ReadPacketString(msg, &spell.description);
        ReadPacketString(msg, &spell.name);
        (void)attackSpell;
        spell.spellId = static_cast<std::uint16_t>(spellId);
        spell.targetType = static_cast<unsigned char>(targetType);
        spell.manaCost = static_cast<std::uint16_t>(manaCost);
        spell.duration = static_cast<std::int32_t>(duration);
        spell.level = static_cast<std::uint16_t>(level);
        spell.element = static_cast<std::uint16_t>(element);
        spell.damageType = static_cast<std::uint16_t>(damageType);
        spell.icon = static_cast<std::int32_t>(icon);
        book.spells.push_back(std::move(spell));
    }
    book.valid = true;
    {
        std::lock_guard<std::mutex> lock(g_inventoryMutex);
        g_spellBook = std::move(book);
    }
    g_inventoryDirty.store(true);
}

/** Opcode 53 RQ_GoldChange : or transporte. */
void HandleGoldChange(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long gold = 0;
    msg->Get(&gold);
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_playerStatus.gold = gold < 0 ? 0u : static_cast<std::uint32_t>(gold);
        g_playerStatus.valid = true;
    }
    g_statusDirty.store(true);
}

/** Opcode 92 RQ_UpdateWeight : poids transporte / max. */
void HandleUpdateWeight(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long weight = 0;
    long maxWeight = 0;
    msg->Get(&weight);
    msg->Get(&maxWeight);
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_playerStatus.weight = static_cast<unsigned short>(std::max<long>(weight, 0));
        g_playerStatus.maxWeight = static_cast<unsigned short>(std::max<long>(maxWeight, 0));
        g_playerStatus.valid = true;
    }
    g_statusDirty.store(true);
}

/** Opcode 123 RQ_UpdateBackpackAfterUse : maj qty/charges d'un objet du sac. */
void HandleBackpackAfterUse(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    char display = 0;
    long containerId = 0;
    short appearance = 0;
    long itemId = 0;
    short baseId = 0;
    long qty = 0;
    long charges = 0;
    msg->Get(&display);
    if (msg->isEnd()) {
        /* Opcode 123 court (RQ_SafePlug : 123 + char 1) = ack de sortie volontaire serveur,
         * pas une maj d'inventaire. */
        return;
    }
    msg->Get(&containerId);
    msg->Get(&appearance);
    msg->Get(&itemId);
    msg->Get(&baseId);
    msg->Get(&qty);
    msg->Get(&charges);
    (void)display;
    (void)containerId;
    bool found = false;
    {
        std::lock_guard<std::mutex> lock(g_inventoryMutex);
        for (T4CBagItem &item : g_backpack.items) {
            if (item.objectId == itemId ||
                (item.objectId == 0 && item.baseId == static_cast<std::uint16_t>(baseId))) {
                item.objectId = itemId;
                item.qty = qty < 0 ? 0u : static_cast<std::uint32_t>(qty);
                item.charges = static_cast<std::int32_t>(charges);
                if (appearance != 0) {
                    item.appearance = static_cast<std::uint16_t>(appearance);
                }
                found = true;
                break;
            }
        }
        if (found) {
            std::erase_if(g_backpack.items, [](const T4CBagItem &it) { return it.qty == 0; });
        }
    }
    if (!found) {
        /* Objet inconnu localement : resynchronise le sac complet. */
        SendOpcodeOnly(static_cast<short>(RQ_ViewInv));
    }
    g_inventoryDirty.store(true);
}

bool ParseShopEntryName(TFCPacket *msg, std::string *outName, std::string *outReqs) {
    if (!ReadPacketString(msg, outName)) {
        return false;
    }
    if (outReqs) {
        *outReqs = {};
    }
    return true;
}

void HandleBuyItemList(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long gold = 0;
    short nbItems = 0;
    msg->Get(&gold);
    msg->Get(&nbItems);
    T4CShopList shop{};
    shop.mode = T4CShopMode::Buy;
    shop.gold = static_cast<std::uint32_t>(gold);
    shop.items.reserve(static_cast<size_t>(nbItems));
    for (int i = 0; i < nbItems; ++i) {
        T4CShopEntry entry{};
        short itemId = 0;
        short appearance = 0;
        long price = 0;
        char canEquip = 0;
        msg->Get(&itemId);
        msg->Get(&appearance);
        msg->Get(&price);
        msg->Get(&canEquip);
        std::string name;
        std::string reqs;
        ParseShopEntryName(msg, &name, &reqs);
        entry.objectId = itemId;
        entry.appearance = static_cast<std::uint16_t>(appearance);
        entry.price = static_cast<std::uint32_t>(price);
        entry.canEquip = canEquip != 0;
        entry.name = std::move(name);
        entry.requirements = std::move(reqs);
        shop.items.push_back(std::move(entry));
    }
    shop.valid = true;
    {
        std::lock_guard<std::mutex> lock(g_talkMutex);
        g_shopList = std::move(shop);
    }
    g_shopDirty.store(true);
}

void HandleSellItemList(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long gold = 0;
    short nbItems = 0;
    msg->Get(&gold);
    msg->Get(&nbItems);
    T4CShopList shop{};
    shop.mode = T4CShopMode::Sell;
    shop.gold = static_cast<std::uint32_t>(gold);
    for (int i = 0; i < nbItems; ++i) {
        T4CShopEntry entry{};
        long itemId = 0;
        short appearance = 0;
        long price = 0;
        long maxQty = 0;
        msg->Get(&itemId);
        msg->Get(&appearance);
        msg->Get(&price);
        msg->Get(&maxQty);
        std::string name;
        ParseShopEntryName(msg, &name, nullptr);
        entry.objectId = static_cast<std::int32_t>(itemId);
        entry.appearance = static_cast<std::uint16_t>(appearance);
        entry.price = static_cast<std::uint32_t>(price);
        entry.maxQty = static_cast<std::uint32_t>(maxQty);
        entry.name = std::move(name);
        shop.items.push_back(std::move(entry));
    }
    shop.valid = true;
    {
        std::lock_guard<std::mutex> lock(g_talkMutex);
        g_shopList = std::move(shop);
    }
    g_shopDirty.store(true);
}

void HandleTrainSkillList(TFCPacket *msg, const bool teach) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    short skillPnts = 0;
    short nbItems = 0;
    msg->Get(&skillPnts);
    msg->Get(&nbItems);
    T4CShopList shop{};
    shop.mode = teach ? T4CShopMode::Learn : T4CShopMode::Train;
    shop.skillPoints = static_cast<std::uint16_t>(skillPnts);
    for (int i = 0; i < nbItems; ++i) {
        T4CShopEntry entry{};
        char canHave = 0;
        short skillId = 0;
        short qty = 0;
        short maxQty = 0;
        long price = 0;
        msg->Get(&canHave);
        msg->Get(&skillId);
        msg->Get(&qty);
        if (teach) {
            msg->Get(&price);
            std::string name;
            std::string reqs;
            ParseShopEntryName(msg, &name, &reqs);
            entry.name = std::move(name);
            entry.requirements = std::move(reqs);
            long icon = 0;
            msg->Get(&icon);
            (void)icon;
            long sp = 0;
            msg->Get(&sp);
            entry.maxQty = static_cast<std::uint32_t>(sp);
        } else {
            msg->Get(&maxQty);
            msg->Get(&price);
            std::string name;
            ParseShopEntryName(msg, &name, nullptr);
            entry.name = std::move(name);
            entry.maxQty = static_cast<std::uint32_t>(maxQty);
        }
        entry.objectId = skillId;
        entry.price = static_cast<std::uint32_t>(price);
        entry.canEquip = canHave != 0;
        shop.items.push_back(std::move(entry));
    }
    shop.valid = true;
    {
        std::lock_guard<std::mutex> lock(g_talkMutex);
        g_shopList = std::move(shop);
    }
    g_shopDirty.store(true);
}

bool ParseRolledStats(TFCPacket *msg, T4CCharacterRolledStats *out) {
    if (!msg || !out) {
        return false;
    }
    unsigned char agi = 0;
    unsigned char end = 0;
    unsigned char intel = 0;
    unsigned char luck = 0;
    unsigned char str = 0;
    unsigned char wil = 0;
    unsigned char wis = 0;
    long maxHp = 0;
    long hp = 0;
    unsigned short maxMana = 0;
    unsigned short mana = 0;
    msg->Get(reinterpret_cast<char *>(&agi));
    msg->Get(reinterpret_cast<char *>(&end));
    msg->Get(reinterpret_cast<char *>(&intel));
    msg->Get(reinterpret_cast<char *>(&luck));
    msg->Get(reinterpret_cast<char *>(&str));
    msg->Get(reinterpret_cast<char *>(&wil));
    msg->Get(reinterpret_cast<char *>(&wis));
    msg->Get(&maxHp);
    msg->Get(&hp);
    msg->Get(reinterpret_cast<short *>(&maxMana));
    msg->Get(reinterpret_cast<short *>(&mana));
    out->agi = agi;
    out->end = end;
    out->intel = intel;
    out->luck = luck;
    out->str = str;
    out->wil = wil;
    out->wis = wis;
    out->maxHp = maxHp < 0 ? 0u : static_cast<unsigned>(maxHp);
    out->hp = hp < 0 ? 0u : static_cast<unsigned>(hp);
    out->maxMana = maxMana;
    out->mana = mana;
    out->valid = true;
    return true;
}

void StoreRolledStats(T4CCharacterRolledStats stats) {
    {
        std::lock_guard<std::mutex> lock(g_rolledMutex);
        g_rolledStats = stats;
    }
    g_rolledStatsPending.store(true);
}

void SetCreatePlayerError(unsigned char code) {
    std::lock_guard<std::mutex> lock(g_errMutex);
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Creation refusee (code %u)", static_cast<unsigned>(code));
    g_createError = buf;
    g_waitingCreate.store(false);
    g_createSentAt.store(0);
    g_createRetryCount.store(0);
    g_pendingCreateAfterAuth.store(false);
}

void SetCreatePlayerErrorMessage(const std::string &msg) {
    std::lock_guard<std::mutex> lock(g_errMutex);
    g_createError = msg;
    g_waitingCreate.store(false);
    g_createSentAt.store(0);
    g_createRetryCount.store(0);
    g_pendingCreateAfterAuth.store(false);
}

void SetDeletePlayerError(unsigned char code) {
    std::lock_guard<std::mutex> lock(g_errMutex);
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Suppression refusee (code %u)", static_cast<unsigned>(code));
    g_deleteError = buf;
    g_waitingDelete.store(false);
    g_deleteSentAt.store(0);
    g_deleteRetryCount.store(0);
    g_pendingDeleteName.clear();
    g_pendingDeleteAfterAuth.store(false);
}

void SetDeletePlayerErrorMessage(const std::string &msg) {
    std::lock_guard<std::mutex> lock(g_errMutex);
    g_deleteError = msg;
    g_waitingDelete.store(false);
    g_deleteSentAt.store(0);
    g_deleteRetryCount.store(0);
    g_pendingDeleteName.clear();
    g_pendingDeleteAfterAuth.store(false);
}

void TryAutoEnterWorldAfterCreateList() {
    if (!g_autoEnterWorldAfterCreate.exchange(false)) {
        return;
    }
    std::string name = g_pendingCreateName;
    if (name.empty()) {
        g_pendingCreateSuccess.store(true);
        return;
    }
    g_selectedPlayer = name;
    {
        std::lock_guard<std::mutex> lock(g_activeMutex);
        g_activePlayer = {};
        g_activePlayer.name = name;
        g_activePlayer.valid = true;
    }
    /* Reset dedup NM uniquement apres create/reroll (rafale 25/31/26 avant le 13). */
    if (COMM.Ctr) {
        COMM.Ctr->ClearAllReceivedPacketIDs();
    }
    SendPutPlayerInGame(name);
}

void SendPacket(TFCPacket &pkt) {
    COMM.SendPacket(&pkt);
    g_lastClientSendTick.store(SDL_GetTicks());
}

void SendOpcodeOnly(short opcode) {
    TFCPacket p;
    p << opcode;
    SendPacket(p);
}

void SendRegister(const std::string &login, const std::string &password) {
    std::string acc = login.substr(0, 255);
    std::string pwd = password.substr(0, 255);
    TFCPacket p;
    p.SetPacketSeedID(99999);
    p << static_cast<short>(RQ_RegisterAccount);
    p << static_cast<char>(acc.size());
    p << const_cast<char *>(acc.c_str());
    p << static_cast<char>(pwd.size());
    p << const_cast<char *>(pwd.c_str());
    p << kClientVersion;
    p << static_cast<short>(0);
    SendPacket(p);
    g_registerSentAt.store(SDL_GetTicks());
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth,
                           "[AUTH] -> RQ_RegisterAccount (14) version=%d login_len=%zu", kClientVersion,
                           acc.size());
}

void SendAuthVersion() {
    TFCPacket p;
    p << static_cast<short>(RQ_AuthenticateServerVersion);
    p << static_cast<long>(kClientVersion);
    SendPacket(p);
    MarkAuthPhaseSent();
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] -> RQ_AuthenticateServerVersion (99) long=%d",
                           kClientVersion);
}

void SendCharacterListRequest() {
    SendOpcodeOnly(static_cast<short>(RQ_GetPersonnalPClist));
    MarkAuthPhaseSent();
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] -> RQ_GetPersonnalPClist (26)");
}

void SendPutPlayerInGame(const std::string &name) {
    std::string n = name.substr(0, 255);
    TFCPacket p;
    p << static_cast<short>(RQ_PutPlayerInGame);
    p << static_cast<char>(n.size());
    p << const_cast<char *>(n.c_str());
    p << static_cast<long>(g_sessionKey);
    SendPacket(p);
    g_waitingPutPlayer.store(true);
    g_putPlayerSentAt.store(SDL_GetTicks());
    g_putPlayerRetryCount.store(0);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] -> RQ_PutPlayerInGame (13) « %s » key=%ld",
                           n.c_str(), static_cast<long>(g_sessionKey));
}

void SendCreatePlayerPacket(const std::string &name, unsigned char sex, const unsigned char stats[5]) {
    TFCPacket p;
    p << static_cast<short>(RQ_CreatePlayer);
    for (int i = 0; i < 5; ++i) {
        p << static_cast<char>(stats[i]);
    }
    p << static_cast<char>(sex);
    std::string n = name.substr(0, 255);
    p << static_cast<char>(n.size());
    p << const_cast<char *>(n.c_str());
    SendPacket(p);
    g_createSentAt.store(SDL_GetTicks());
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                           "[PHASE] -> RQ_CreatePlayer (25) « %s » (%s) stats=%u,%u,%u,%u,%u",
                           n.c_str(), sex ? "F" : "M", static_cast<unsigned>(stats[0]),
                           static_cast<unsigned>(stats[1]), static_cast<unsigned>(stats[2]),
                           static_cast<unsigned>(stats[3]), static_cast<unsigned>(stats[4]));
}

void SendDeletePlayerPacket(const std::string &name) {
    std::string n = name.substr(0, 255);
    TFCPacket p;
    p << static_cast<short>(RQ_DeletePlayer);
    p << static_cast<char>(n.size());
    p << const_cast<char *>(n.c_str());
    SendPacket(p);
    g_deleteSentAt.store(SDL_GetTicks());
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] -> RQ_DeletePlayer (15) « %s »", n.c_str());
}

void SendPostEnterWorldPackets() {
    g_waitingFromPreInGame.store(true);
    g_fromPreInGameResult.store(-1);
    g_enterWorld46SentAt.store(SDL_GetTicks());
    g_enterWorld46RetryCount.store(0);
    SendOpcodeOnly(static_cast<short>(RQ_FromPreInGameToInGame));
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                           "[PHASE] -> 46 (entree monde — attente reponse avant 60/skill/moves)");
}

/** Apres RQ_TeleportPlayer (57) le serveur repasse boPreInGame — comme Windows (opcode 60 + 46). */
void NotifyPlayerTeleportResync() {
    g_fromPreInGameResult.store(-1);
    g_waitingFromPreInGame.store(true);
    g_enterWorld46SentAt.store(SDL_GetTicks());
    g_enterWorld46RetryCount.store(0);
    g_pendingNearItems.store(true);
    SendPostEnterWorldPackets();
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                           "[PHASE] teleport → resync opcode 46 + GetNearItems differe");
}

void SendEnterWorldFollowUpPackets() {
    /* 60 envoye par finishEnterWorld() apres preload — eviter double 60 qui bloque le thread UDP. */
    SendOpcodeOnly(static_cast<short>(RQ_GetSkillList));
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] -> skill list (39, apres 46 OK — 60 au chargement carte)");
}

void ParseCharacterList(TFCPacket *msg) {
    SkipOpcode(msg);
    unsigned char count = 0;
    msg->Get(reinterpret_cast<char *>(&count));
    std::vector<T4CCharacterSlot> slots;
    slots.reserve(count);
    for (unsigned i = 0; i < count; ++i) {
        char nameLen = 0;
        msg->Get(&nameLen);
        T4CCharacterSlot slot;
        for (int c = 0; c < nameLen; ++c) {
            char ch = 0;
            msg->Get(&ch);
            slot.name.push_back(ch);
        }
        unsigned short race = 0;
        unsigned short level = 0;
        msg->Get(reinterpret_cast<short *>(&race));
        msg->Get(reinterpret_cast<short *>(&level));
        short extra = 0;
        msg->Get(&extra);
        slot.race = race;
        slot.level = level;
        slots.push_back(std::move(slot));
    }
    {
        std::lock_guard<std::mutex> lock(g_charMutex);
        g_characters = std::move(slots);
    }
    g_pendingCharacterList.store(true);
    g_pipelineStep.store(4);
    g_authPhaseSentAt.store(0);
    g_authPhaseRetryCount.store(0);
    TryAutoEnterWorldAfterCreateList();
    if (g_waitingDelete.load()) {
        g_waitingDelete.store(false);
        g_deleteSentAt.store(0);
        g_deleteRetryCount.store(0);
        g_pendingDeleteName.clear();
    }
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- liste persos (%u)", count);
}

void ParseCharacterEquipSkin(TFCPacket *msg) {
    SkipOpcode(msg);
    char count = 0;
    msg->Get(&count);
    short tmp = 0;
    for (int i = 0; i < count; ++i) {
        for (int ee = 0; ee < 17; ++ee) {
            msg->Get(&tmp);
        }
        msg->Get(&tmp);
    }
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- equipement persos (%d)", static_cast<int>(count));
}

void HandleMaxCharactersPerAccount(TFCPacket *msg) {
    SkipOpcode(msg);
    char maxChars = 3;
    msg->Get(&maxChars);
    if (maxChars > 0) {
        g_maxCharsPerAccount = maxChars;
    }
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- max persos/compte = %d",
                           g_maxCharsPerAccount);
}

bool ParsePutPlayerInGame(TFCPacket *msg) {
    SkipOpcode(msg);
    char err = 0;
    msg->Get(&err);
    if (err != 0) {
        if (err == 7 && !g_selectedPlayer.empty() && g_putPlayerRetryCount.load() < 4) {
            const int n = g_putPlayerRetryCount.fetch_add(1) + 1;
            g_putPlayerSentAt.store(SDL_GetTicks() - 7500);
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                                   "[PHASE] PutPlayerInGame busy (7) — retry %d/4 dans ~1 s", n);
            return false;
        }
        std::lock_guard<std::mutex> lock(g_errMutex);
        char buf[128];
        std::snprintf(buf, sizeof(buf), "PutPlayerInGame erreur %u", static_cast<unsigned>(err));
        g_putPlayerError = buf;
        g_waitingPutPlayer.store(false);
        g_putPlayerSentAt.store(0);
        return false;
    }

    long factionId = 0;
    long playerId = 0;
    short x = 0;
    short y = 0;
    short world = 0;
    msg->Get(&factionId);
    msg->Get(&playerId);
    msg->Get(&x);
    msg->Get(&y);
    msg->Get(&world);

    // Le reste du paquet (stats, or, temps…) n'est pas necessaire ici : chaque paquet
    // possede son propre buffer, on extrait x/y/world/playerId et on ignore la suite.

    g_spawn.x = x < 0 ? 0u : static_cast<unsigned>(x);
    g_spawn.y = y < 0 ? 0u : static_cast<unsigned>(y);
    g_spawn.world = world < 0 ? 0 : static_cast<unsigned short>(world);
    g_spawn.valid = true;

    {
        std::lock_guard<std::mutex> lock(g_activeMutex);
        g_activePlayer.name = g_selectedPlayer;
        g_activePlayer.serverX = g_spawn.x;
        g_activePlayer.serverY = g_spawn.y;
        g_activePlayer.unitId = static_cast<std::int32_t>(playerId);
        g_activePlayer.valid = true;
        ApplyRaceToPuppetAppearance(&g_activePlayer);
    }

    g_waitingPutPlayer.store(false);
    g_putPlayerSentAt.store(0);
    g_pendingEnterWorld.store(true);
    g_pipelineStep.store(5);
    SendPostEnterWorldPackets();
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- PutPlayerInGame OK @ %u,%u w=%u", g_spawn.x,
                           g_spawn.y, static_cast<unsigned>(g_spawn.world));
    return true;
}

void HandleRegisterReply(TFCPacket *msg) {
    SkipOpcode(msg);
    unsigned char status = 0;
    msg->Get(reinterpret_cast<char *>(&status));
    if (status == TFCRegisterAccount_Registred) {
        {
            std::lock_guard<std::mutex> lock(g_errMutex);
            g_authError.clear();
        }
        short msgLen = 0;
        msg->Get(&msgLen);
        for (int i = 0; i < msgLen; ++i) {
            char ch = 0;
            msg->Get(&ch);
        }
        long key = 0;
        msg->Get(&key);
        g_sessionKey = key;
        g_registerRetryCount.store(0);
        g_pipelineStep.store(2);
        g_authPhaseRetryCount.store(0);
        SendAuthVersion();
    } else {
        std::string errMsg = "Authentification refusee.";
        if (!msg->isEnd()) {
            short msgLen = 0;
            msg->Get(&msgLen);
            std::string text;
            for (int i = 0; i < msgLen; ++i) {
                char ch = 0;
                msg->Get(&ch);
                if (ch != '\r' && ch != '\n') {
                    text.push_back(ch);
                }
            }
            if (!text.empty()) {
                errMsg = text;
            }
        }
        const bool retryable = status == 1 || status == 2;
        if (retryable && g_registerRetryCount.load() < 6) {
            const int n = g_registerRetryCount.fetch_add(1) + 1;
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                                   "[AUTH] Register code=%u — retry %d/6 dans ~2 s (%s)",
                                   static_cast<unsigned>(status), n, errMsg.c_str());
            g_autoRegisterAt.store(SDL_GetTicks() + 2000);
            g_pendingAutoRegister.store(true);
            return;
        }
        {
            std::lock_guard<std::mutex> lock(g_errMutex);
            g_authError = errMsg;
        }
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn, "[AUTH] Register refuse code=%u — %s",
                               static_cast<unsigned>(status), errMsg.c_str());
        g_pipelineStep.store(0);
        T4CCommBridgeStop();
    }
}

void HandleAuthVersionReply(TFCPacket *msg) {
    SkipOpcode(msg);
    long valid = 0;
    msg->Get(&valid);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                           "[PHASE] <- RQ_AuthenticateServerVersion (99) valid=%ld", valid);
    if (valid == 1) {
        g_authPhaseRetryCount.store(0);
        if (g_pendingDeleteAfterAuth.exchange(false) && !g_pendingDeleteName.empty()) {
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                                   "[PHASE] re-auth OK — renvoi RQ_DeletePlayer (15)");
            SendDeletePlayerPacket(g_pendingDeleteName);
            return;
        }
        if (g_pendingCreateAfterAuth.exchange(false) && !g_pendingCreateName.empty()) {
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                                   "[PHASE] re-auth OK — renvoi RQ_CreatePlayer (25)");
            SendCreatePlayerPacket(g_pendingCreateName, g_pendingCreateSex, g_pendingCreateStats);
            return;
        }
        g_pipelineStep.store(3);
        SendCharacterListRequest();
    } else {
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn, "[AUTH] Version refusee (99 long=0, attendu %d)",
                               kClientVersion);
        FailAuthPhase("Version client refusee par le serveur.");
    }
}

void HandleCreatePlayerReply(TFCPacket *msg) {
    g_waitingCreate.store(false);
    g_createSentAt.store(0);
    g_createRetryCount.store(0);
    SkipOpcode(msg);
    char err = 0;
    msg->Get(&err);
    if (err != 0) {
        SetCreatePlayerError(static_cast<unsigned char>(err));
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn, "[PHASE] RQ_CreatePlayer (25) refuse code=%d",
                               static_cast<int>(err));
        return;
    }
    T4CCharacterRolledStats stats{};
    if (ParseRolledStats(msg, &stats)) {
        StoreRolledStats(stats);
    }
    g_inCreateRerollPhase.store(true);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] RQ_CreatePlayer (25) OK — ecran reroll");
}

void HandleRerollReply(TFCPacket *msg) {
    g_waitingReroll.store(false);
    SkipOpcode(msg);
    T4CCharacterRolledStats stats{};
    if (ParseRolledStats(msg, &stats)) {
        StoreRolledStats(stats);
    }
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] RQ_Reroll (31) OK");
}

void HandleDeletePlayerReply(TFCPacket *msg) {
    SkipOpcode(msg);
    char code = 0;
    msg->Get(&code);
    if (code != 0 && msg->isEnd()) {
        SetDeletePlayerError(static_cast<unsigned char>(code));
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn, "[PHASE] RQ_DeletePlayer (15) refuse code=%d",
                               static_cast<int>(code));
        return;
    }
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- RQ_DeletePlayer (15) OK — refresh liste");
    SendCharacterListRequest();
}

void ParsePeripheralObjectList(TFCPacket *msg, const bool replaceGroundList) {
    if (replaceGroundList) {
        std::lock_guard<std::mutex> lock(g_remoteMutex);
        g_groundMarkers.clear();
    }
    unsigned short count = 0;
    msg->Get(reinterpret_cast<short *>(&count));
    unsigned drawn = 0;
    unsigned ground = 0;
    std::unordered_set<std::int32_t> inviewUnitIds;
    for (unsigned i = 0; i < count && !msg->isEnd(); ++i) {
        short sx = 0;
        short sy = 0;
        msg->Get(&sx);
        msg->Get(&sy);
        std::uint16_t appearance = 0;
        std::int32_t unitId = 0;
        char hpPercent = 0;
        if (!ParseUnitInfo(msg, &appearance, &unitId, &hpPercent)) {
            break;
        }
        appearance = NormalizeServerAppearance(appearance);
        if (appearance >= 10001 && count > 4) {
            /* Diagnostic visibilite PNJ : trace les unites (pas le sol) des grosses listes. */
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                                   "[PHASE]    unite %u/%u : app=%u id=%d @ %d,%d", i + 1,
                                   static_cast<unsigned>(count), static_cast<unsigned>(appearance), unitId,
                                   static_cast<int>(sx), static_cast<int>(sy));
        }
        if (sx >= 0 && sy >= 0) {
            if (IsGroundWorldObject(appearance)) {
                UpsertGroundMarker(static_cast<unsigned>(sx), static_cast<unsigned>(sy), appearance, unitId);
                ++ground;
            } else if (IsRemoteDrawableUnit(appearance)) {
                if (!T4CLoginSessionShouldSkipRemoteUnit(appearance, unitId, static_cast<unsigned>(sx),
                                                         static_cast<unsigned>(sy))) {
                    ++drawn;
                    inviewUnitIds.insert(unitId);
                }
                QueueRemoteSpawn(static_cast<unsigned>(sx), static_cast<unsigned>(sy), appearance, unitId,
                                 hpPercent);
            }
        }
    }
    if (replaceGroundList && count > 0) {
        std::vector<std::int32_t> stale;
        {
            std::lock_guard<std::mutex> lock(g_remoteMutex);
            for (const auto &entry : g_remoteAppearance) {
                if (IsLocalPlayerUnitId(entry.first)) {
                    continue;
                }
                if (inviewUnitIds.find(entry.first) == inviewUnitIds.end()) {
                    stale.push_back(entry.first);
                }
            }
        }
        for (const std::int32_t id : stale) {
            QueueRemoteRemove(id);
        }
    }
    PruneFarGroundMarkers();
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                           "[PHASE] <- liste peripherique (%u entree(s), %u PNJ, %u sol)", count, drawn,
                           ground);
}

void HandleTeleportPlayer(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    T4CPlayerTeleport tp{};
    if (!msg->isEnd()) {
        short tx = 0;
        short ty = 0;
        short tw = 0;
        msg->Get(&tx);
        msg->Get(&ty);
        msg->Get(&tw);
        if (tx >= 0 && ty >= 0 && tw >= 0) {
            tp.x = static_cast<unsigned>(tx);
            tp.y = static_cast<unsigned>(ty);
            tp.world = static_cast<unsigned short>(tw);
            tp.hasWorld = true;
        }
    }
    {
        std::lock_guard<std::mutex> lock(g_teleportMutex);
        g_teleportPayload = tp;
    }
    g_pendingTeleport.store(true);
    NotifyPlayerTeleportResync();
    T4CLoginSessionSendBreakConversation();
    if (tp.x >= 2900 && tp.x <= 3000 && tp.y >= 1000 && tp.y <= 1100) {
        g_playerDead.store(false);
        g_deathPending.store(false);
        g_deathResurrectSent.store(false);
        std::lock_guard<std::mutex> lock(g_deathMutex);
        g_deathState = {};
    }
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                           "[PHASE] <- RQ_TeleportPlayer (57) @ %u,%u w=%u",
                           tp.x, tp.y, static_cast<unsigned>(tp.world));
}

void HandleObjectAppearedList(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    /* IMPORTANT : l'inview complet (reponse a GetNearItems 60) ET les bandes peripheriques poussees
     * a chaque deplacement (Character.cpp packet_peripheral_units) arrivent tous deux en opcode 16,
     * impossibles a distinguer. On traite donc TOUJOURS en additif (jamais de purge) : les retraits
     * passent par l'opcode 11 (BCObjectRemoved) + le cull par distance cote rendu. Purger ici sur une
     * bande d'1 entree effacait tout le sol (ex. porte qui disparait). */
    g_expectInviewRefresh.store(false);
    ParsePeripheralObjectList(msg, false);
}

void HandleGetNearItemsReply(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    g_expectInviewRefresh.store(false);
    if (msg->isEnd()) {
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] RQ_GetNearItems (60) vide");
        return;
    }
    ParsePeripheralObjectList(msg, false);
}

void HandleRemoteMove(TFCPacket *msg, const std::uint16_t moveOpcode) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    short sx = 0;
    short sy = 0;
    msg->Get(&sx);
    msg->Get(&sy);
    std::uint16_t appearance = 0;
    std::int32_t unitId = 0;
    if (!ParseUnitInfo(msg, &appearance, &unitId, nullptr)) {
        return;
    }
    if (sx < 0 || sy < 0) {
        return;
    }
    appearance = NormalizeServerAppearance(appearance);
    if (IsLocalPlayerUnitId(unitId) ||
        (unitId == 0 && IsLocalPlayerMoveTarget(static_cast<int>(sx), static_cast<int>(sy)))) {
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- move ack @ %d,%d (op=%u unit=%ld)",
                               static_cast<int>(sx), static_cast<int>(sy),
                               static_cast<unsigned>(moveOpcode), static_cast<long>(unitId));
        QueueLocalPlayerMove(static_cast<unsigned>(sx), static_cast<unsigned>(sy), moveOpcode);
        return;
    }
    RememberRemoteAppearance(unitId, appearance);
    if (IsPuppetAppearance(appearance)) {
        RequestPuppetInformation(unitId, static_cast<unsigned>(sx), static_cast<unsigned>(sy));
    }
    QueueRemoteMove(static_cast<unsigned>(sx), static_cast<unsigned>(sy), unitId, moveOpcode, appearance);
}

void HandlePuppetInformation(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long unitId = 0;
    short body = 0;
    short feet = 0;
    short gloves = 0;
    short helm = 0;
    short legs = 0;
    short wRight = 0;
    short wLeft = 0;
    short cape = 0;
    short hair = 0;
    short tag = 0;
    msg->Get(&unitId);
    msg->Get(&body);
    msg->Get(&feet);
    msg->Get(&gloves);
    msg->Get(&helm);
    msg->Get(&legs);
    msg->Get(&wRight);
    msg->Get(&wLeft);
    msg->Get(&cape);
    msg->Get(&hair);
    msg->Get(&tag);
    if (unitId == 0) {
        return;
    }
    bool isLocal = false;
    {
        std::lock_guard<std::mutex> lock(g_activeMutex);
        isLocal = g_activePlayer.valid && g_activePlayer.unitId != 0 && unitId == g_activePlayer.unitId;
    }
    if (isLocal) {
        T4CPuppetDress dress{};
        dress.body = static_cast<std::uint16_t>(body);
        dress.feet = static_cast<std::uint16_t>(feet);
        dress.gloves = static_cast<std::uint16_t>(gloves);
        dress.helm = static_cast<std::uint16_t>(helm);
        dress.legs = static_cast<std::uint16_t>(legs);
        dress.weaponR = static_cast<std::uint16_t>(wRight);
        dress.weaponL = static_cast<std::uint16_t>(wLeft);
        dress.cape = static_cast<std::uint16_t>(cape);
        dress.known = true;
        std::lock_guard<std::mutex> lock(g_activeMutex);
        ApplyPuppetDressToPlayer(&g_activePlayer, dress);
        ApplyRaceToPuppetAppearance(&g_activePlayer);
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_remoteMutex);
        T4CPuppetDress &stored = g_remotePuppet[unitId];
        stored.body = static_cast<std::uint16_t>(body);
        stored.feet = static_cast<std::uint16_t>(feet);
        stored.gloves = static_cast<std::uint16_t>(gloves);
        stored.helm = static_cast<std::uint16_t>(helm);
        stored.legs = static_cast<std::uint16_t>(legs);
        stored.weaponR = static_cast<std::uint16_t>(wRight);
        stored.weaponL = static_cast<std::uint16_t>(wLeft);
        stored.cape = static_cast<std::uint16_t>(cape);
        stored.known = true;
        g_pendingPuppetRequest.erase(unitId);
    }
}

/* RQ_GetUnitName (35) : long id, short len + nom, short len + guilde, ulong couleur,
 * ulong couleur guilde (Packet.cpp Windows). */
void HandleGetUnitName(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long unitId = 0;
    msg->Get(&unitId);
    short len = 0;
    msg->Get(&len);
    std::string name;
    for (int i = 0; i < len && i < 100; ++i) {
        char c = 0;
        msg->Get(&c);
        name.push_back(c);
    }
    short guildLen = 0;
    msg->Get(&guildLen);
    std::string guild;
    for (int i = 0; i < guildLen && i < 100; ++i) {
        char c = 0;
        msg->Get(&c);
        guild.push_back(c);
    }
    unsigned long color = 0;
    unsigned long guildColor = 0;
    msg->Get(&color);
    msg->Get(&guildColor);
    (void)guildColor;
    if (unitId == 0 || name.empty()) {
        return;
    }
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- nom unite (35) id=%ld « %s »",
                           static_cast<long>(unitId), name.c_str());
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    T4CUnitNameEntry &entry = g_unitNames[static_cast<std::int32_t>(unitId)];
    entry.name = std::move(name);
    entry.color = static_cast<std::uint32_t>(color);
    g_pendingUnitName.erase(static_cast<std::int32_t>(unitId));
}

void HandleUnitUpdate(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    std::uint16_t appearance = 0;
    std::int32_t unitId = 0;
    char hpPercent = 0;
    if (!ParseUnitInfo(msg, &appearance, &unitId, &hpPercent) || IsLocalPlayerUnitId(unitId)) {
        return;
    }
    RememberRemoteAppearance(unitId, appearance);
    T4CRemoteUnitEvent ev{};
    ev.kind = T4CRemoteUnitEventKind::Update;
    ev.unitId = unitId;
    ev.appearance = appearance;
    ev.hpPercent = hpPercent;
    PushRemoteEvent(ev);
}

void HandleMissingUnit(TFCPacket *msg) {
    SkipOpcode(msg);
    long unitId = 0;
    short type = 0;
    msg->Get(&unitId);
    if (!msg->isEnd()) {
        msg->Get(&type);
    }
    /* Windows Packet.cpp : TYPE==11 = unite disparue ; autres types = requete attaque / refresh. */
    if (type == 11) {
        RemoveGroundMarker(static_cast<std::int32_t>(unitId));
        QueueRemoteRemove(static_cast<std::int32_t>(unitId));
    }
}

/* Serveur __EVENT_OBJECT_REMOVED (opcode 11) : char(effet) + long ID. Retire l'unite distante
 * (sinon le sprite reste fige = doublon garde Olin Haad). */
void HandleObjectRemoved(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    char how = 0;
    long unitId = 0;
    msg->Get(&how);
    msg->Get(&unitId);
    (void)how;
    RemoveGroundMarker(static_cast<std::int32_t>(unitId));
    QueueRemoteRemove(static_cast<std::int32_t>(unitId));
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- retrait unite (11) id=%ld",
                           static_cast<long>(unitId));
}

/* Serveur __EVENT_OBJECT_CHANGED (opcode 12) : short type + long ID + long dead. Mort d'une
 * creature : on retire l'unite vivante (le cadavre reviendra comme objet sol via GetNearItems). */
void HandleObjectChanged(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    short objectType = 0;
    long unitId = 0;
    long dead = 0;
    msg->Get(&objectType);
    msg->Get(&unitId);
    msg->Get(&dead);
    if (dead != 0) {
        QueueRemoteRemove(static_cast<std::int32_t>(unitId));
        return;
    }
    /* dead==0 : changement d'apparence (porte ouverte/fermee, etc.). On applique tout de suite
     * sur le marqueur sol ; le son de porte suit la transition dans syncGroundObjectsFromNetwork. */
    if (UpdateGroundMarkerAppearance(static_cast<std::int32_t>(unitId),
                                     static_cast<std::uint16_t>(objectType))) {
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                               "[PHASE] <- objet change (12) id=%ld app=%u", static_cast<long>(unitId),
                               static_cast<unsigned>(static_cast<std::uint16_t>(objectType)));
    }
}

void HandlePacketPopup(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    short sx = 0;
    short sy = 0;
    msg->Get(&sx);
    msg->Get(&sy);
    std::uint16_t appearance = 0;
    std::int32_t unitId = 0;
    char hpPercent = 0;
    if (!ParseUnitInfo(msg, &appearance, &unitId, &hpPercent)) {
        return;
    }
    if (sx < 0 || sy < 0) {
        return;
    }
    const unsigned ux = static_cast<unsigned>(sx);
    const unsigned uy = static_cast<unsigned>(sy);

    if (IsLocalPlayerUnitId(unitId)) {
        std::lock_guard<std::mutex> lock(g_activeMutex);
        g_activePlayer.serverX = ux;
        g_activePlayer.serverY = uy;
        if (appearance != 0) {
            if (IsActualPlayerAppearance(appearance)) {
                g_activePlayer.appearance = appearance;
                g_activePlayer.female = IsFemaleAppearance(appearance);
                if (appearance >= 10001 && appearance <= 10004) {
                    g_activePlayer.classIndex = static_cast<std::uint8_t>(appearance - 10001);
                } else if (appearance >= 15001 && appearance <= 15004) {
                    g_activePlayer.classIndex = static_cast<std::uint8_t>(appearance - 15001);
                }
            } else if (IsPuppetAppearance(appearance)) {
                g_activePlayer.appearance = appearance;
                g_activePlayer.female = IsFemaleAppearance(appearance);
            }
        }
        g_activePlayer.unitId = unitId;
        g_activePlayer.valid = true;
        g_popupPending.store(true);
        if (IsPuppetAppearance(g_activePlayer.appearance) && !g_activePlayer.puppetKnown) {
            RequestPuppetInformation(unitId, ux, uy);
        }
        return;
    }

    if (IsActualPlayerAppearance(appearance)) {
        bool isLocal = false;
        {
            std::lock_guard<std::mutex> lock(g_activeMutex);
            if (g_activePlayer.unitId != 0 && unitId == g_activePlayer.unitId) {
                isLocal = true;
            }
        }
        if (isLocal) {
            std::lock_guard<std::mutex> lock(g_activeMutex);
            g_activePlayer.serverX = ux;
            g_activePlayer.serverY = uy;
            if (appearance != 0) {
                g_activePlayer.appearance = appearance;
                g_activePlayer.female = IsFemaleAppearance(appearance);
            }
            g_activePlayer.unitId = unitId;
            g_activePlayer.valid = true;
            g_popupPending.store(true);
            return;
        }
    }
    QueueRemoteSpawn(ux, uy, appearance, unitId, hpPercent);
}

void ParseGetStatusPacket(TFCPacket *msg) {
    long hp = 0;
    long maxHp = 0;
    short mana = 0;
    short maxMana = 0;
    long expHigh = 0;
    long expLow = 0;
    short bAc = 0;
    short ac = 0;
    short bStr = 0;
    short bEnd = 0;
    short bAgi = 0;
    short bWil = 0;
    short bWis = 0;
    short bInt = 0;
    short bLck = 0;
    short statsPts = 0;
    short str = 0;
    short end = 0;
    short agi = 0;
    short wil = 0;
    short wis = 0;
    short intel = 0;
    short lck = 0;
    short level = 0;
    short skillPts = 0;
    short weight = 0;
    short maxWeight = 0;
    short karma = 0;
    short trueMaxHp = 0;
    short trueMaxMana = 0;
    short crime = 0;
    short honor = 0;
    long interactionPts = 0;
    long interactionPtsT = 0;

    msg->Get(&hp);
    msg->Get(&maxHp);
    msg->Get(&mana);
    msg->Get(&maxMana);
    msg->Get(&expHigh);
    msg->Get(&expLow);
    msg->Get(&bAc);
    msg->Get(&ac);
    msg->Get(&bStr);
    msg->Get(&bEnd);
    msg->Get(&bAgi);
    msg->Get(&bWil);
    msg->Get(&bWis);
    msg->Get(&bInt);
    msg->Get(&bLck);
    msg->Get(&statsPts);
    msg->Get(&str);
    msg->Get(&end);
    msg->Get(&agi);
    msg->Get(&wil);
    msg->Get(&wis);
    msg->Get(&intel);
    msg->Get(&lck);
    msg->Get(&level);
    msg->Get(&skillPts);
    msg->Get(&weight);
    msg->Get(&maxWeight);
    msg->Get(&karma);
    msg->Get(&trueMaxHp);
    msg->Get(&trueMaxMana);
    msg->Get(&crime);
    msg->Get(&honor);
    msg->Get(&interactionPts);
    msg->Get(&interactionPtsT);
    (void)bAc;
    (void)bStr;
    (void)bEnd;
    (void)bAgi;
    (void)bWil;
    (void)bWis;
    (void)bInt;
    (void)bLck;
    (void)statsPts;
    (void)wil;
    (void)lck;
    (void)skillPts;
    (void)karma;
    (void)trueMaxHp;
    (void)trueMaxMana;
    (void)crime;
    (void)honor;
    (void)interactionPts;
    (void)interactionPtsT;

    T4CPlayerStatus next{};
    next.hp = hp < 0 ? 0u : static_cast<unsigned>(hp);
    next.maxHp = maxHp < 0 ? 0u : static_cast<unsigned>(maxHp);
    next.mana = mana < 0 ? 0 : static_cast<unsigned short>(mana);
    next.maxMana = maxMana < 0 ? 0 : static_cast<unsigned short>(maxMana);
    next.level = level < 0 ? 0 : static_cast<std::uint16_t>(level);
    next.ac = ac < 0 ? 0 : static_cast<unsigned short>(ac);
    next.str = str < 0 ? 0 : static_cast<unsigned short>(str);
    next.end = end < 0 ? 0 : static_cast<unsigned short>(end);
    next.agi = agi < 0 ? 0 : static_cast<unsigned short>(agi);
    next.wis = wis < 0 ? 0 : static_cast<unsigned short>(wis);
    next.intel = intel < 0 ? 0 : static_cast<unsigned short>(intel);
    next.weight = weight < 0 ? 0 : static_cast<unsigned short>(weight);
    next.maxWeight = maxWeight < 0 ? 0 : static_cast<unsigned short>(maxWeight);
    next.xp = (static_cast<std::uint64_t>(static_cast<std::uint32_t>(expHigh)) << 32) |
              static_cast<std::uint64_t>(static_cast<std::uint32_t>(expLow));
    next.valid = true;

    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_playerStatus = next;
    }
    {
        std::lock_guard<std::mutex> lock(g_activeMutex);
        if (g_activePlayer.valid && level > 0) {
            g_activePlayer.level = static_cast<std::uint16_t>(level);
        }
    }
    g_statusDirty.store(true);
}

void HandleGetStatus(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    ParseGetStatusPacket(msg);
}

void HandleHpChanged(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long hp = 0;
    msg->Get(&hp);
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_playerStatus.hp = hp < 0 ? 0u : static_cast<unsigned>(hp);
        g_playerStatus.valid = true;
    }
    g_statusDirty.store(true);
}

void HandleManaChanged(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    short mana = 0;
    msg->Get(reinterpret_cast<short *>(&mana));
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_playerStatus.mana = mana < 0 ? 0 : static_cast<unsigned short>(mana);
        g_playerStatus.valid = true;
    }
    g_statusDirty.store(true);
}

void HandleXpChanged(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    long high = 0;
    long low = 0;
    msg->Get(&high);
    msg->Get(&low);
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_playerStatus.xp = (static_cast<std::uint64_t>(static_cast<std::uint32_t>(high)) << 32) |
                            static_cast<std::uint64_t>(static_cast<std::uint32_t>(low));
        g_playerStatus.valid = true;
    }
    g_statusDirty.store(true);
}

void HandleLevelUp(TFCPacket *msg) {
    if (!WorldSessionReceiving()) {
        return;
    }
    SkipOpcode(msg);
    short level = 0;
    long expHigh = 0;
    long expLow = 0;
    long hp = 0;
    long maxHp = 0;
    short mana = 0;
    short maxMana = 0;
    msg->Get(&level);
    msg->Get(&expHigh);
    msg->Get(&expLow);
    msg->Get(&hp);
    msg->Get(&maxHp);
    msg->Get(&mana);
    msg->Get(&maxMana);
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_playerStatus.level = level < 0 ? 0 : static_cast<std::uint16_t>(level);
        g_playerStatus.hp = hp < 0 ? 0u : static_cast<unsigned>(hp);
        g_playerStatus.maxHp = maxHp < 0 ? 0u : static_cast<unsigned>(maxHp);
        g_playerStatus.mana = mana < 0 ? 0 : static_cast<unsigned short>(mana);
        g_playerStatus.maxMana = maxMana < 0 ? 0 : static_cast<unsigned short>(maxMana);
        g_playerStatus.xpToNextLevel =
            (static_cast<std::uint64_t>(static_cast<std::uint32_t>(expHigh)) << 32) |
            static_cast<std::uint64_t>(static_cast<std::uint32_t>(expLow));
        g_playerStatus.valid = true;
    }
    {
        std::lock_guard<std::mutex> lock(g_activeMutex);
        if (g_activePlayer.valid && level > 0) {
            g_activePlayer.level = static_cast<std::uint16_t>(level);
        }
    }
    g_statusDirty.store(true);
}

}  // namespace

void T4CLoginSessionHandlePacket(TFCPacket *msg) {
    if (!msg) {
        return;
    }
    const short opcode = msg->GetPacketID();
    msg->Seek(0, 0);

    try {
        switch (opcode) {
        case RQ_RegisterAccount:
            HandleRegisterReply(msg);
            break;
        case RQ_AuthenticateServerVersion:
            HandleAuthVersionReply(msg);
            break;
        case RQ_GetPersonnalPClist:
            ParseCharacterList(msg);
            break;
        case RQ_GetPersonnalPClistEquitSkin:
            ParseCharacterEquipSkin(msg);
            break;
        case RQ_MaxCharactersPerAccountInfo:
            HandleMaxCharactersPerAccount(msg);
            break;
        case RQ_PutPlayerInGame:
            ParsePutPlayerInGame(msg);
            break;
        case RQ_CreatePlayer:
            HandleCreatePlayerReply(msg);
            break;
        case RQ_Reroll:
            HandleRerollReply(msg);
            break;
        case RQ_DeletePlayer:
            HandleDeletePlayerReply(msg);
            break;
        case RQ_TeleportPlayer:
            HandleTeleportPlayer(msg);
            break;
        case RQ_SendPeriphericObjects:
            HandleObjectAppearedList(msg);
            break;
        case RQ_GetNearItems:
            HandleGetNearItemsReply(msg);
            break;
        case RQ_UnitUpdate:
            HandleUnitUpdate(msg);
            break;
        case RQ_PuppetInformation:
            HandlePuppetInformation(msg);
            break;
        case RQ_GetUnitName:
            HandleGetUnitName(msg);
            break;
        case RQ_MissingUnit:
            HandleMissingUnit(msg);
            break;
        case RQ_GetStatus:
            HandleGetStatus(msg);
            break;
        case RQ_HPchanged:
            HandleHpChanged(msg);
            break;
        case RQ_ManaChanged:
            HandleManaChanged(msg);
            break;
        case RQ_XPchanged:
            HandleXpChanged(msg);
            break;
        case RQ_LevelUp:
            HandleLevelUp(msg);
            break;
        case kPacketPopup:
            HandlePacketPopup(msg);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            HandleRemoteMove(msg, static_cast<std::uint16_t>(opcode));
            break;
        case 11:  // __EVENT_OBJECT_REMOVED (serveur) — pas RQ_GetObject (client→serveur)
            HandleObjectRemoved(msg);
            break;
        case 12:  // __EVENT_OBJECT_CHANGED (serveur) — mort / changement d'objet
            HandleObjectChanged(msg);
            break;
        case RQ_ExitGame:
            COMM.SetAlive(15000);
            g_reconnectAllowedAfterTick.store(0);
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- RQ_ExitGame (20) session liberee");
            break;
        case kStillConnected:
            SendOpcodeOnly(static_cast<short>(RQ_Ack));
            break;
        case RQ_ViewEquiped:
            HandleViewEquipped(msg);
            break;
        case RQ_ViewBackpack2:
            HandleViewBackpack(msg);
            break;
        case RQ_DamageUnit:
            HandleDamageOrHeal(msg, false);
            break;
        case RQ_HealingUnit:
            HandleDamageOrHeal(msg, true);
            break;
        case RQ_YouDied:
            HandleYouDied(msg);
            break;
        case RQ_NM_DeathStatus:
            HandleNmDeathStatus(msg);
            break;
        case RQ_NM_DeathProgress:
            HandleNmDeathProgress(msg);
            break;
        case 10001:
            HandleAttackResult(msg);
            break;
        case 10002:  // __EVENT_MISS : attaque ratee
            HandleAttackMiss(msg);
            break;
        case RQ_IndirectTalk:
            HandleIndirectTalk(msg);
            break;
        case RQ_Page:
            HandlePageMessage(msg);
            break;
        case RQ_ServerMessage:
            HandleServerMessage(msg);
            break;
        case RQ_InfoMessage:
            HandleInfoMessage(msg);
            break;
        case RQ_GetTime:
            HandleGetTime(msg);
            break;
        case RQ_GetSkillList:
            HandleSkillListReply(msg);
            break;
        case RQ_AttackMode:
            HandleAttackModeReply(msg);
            break;
        case RQ_SendSpellList:
            HandleSpellListReply(msg);
            break;
        case RQ_GoldChange:
            HandleGoldChange(msg);
            break;
        case RQ_UpdateWeight:
            HandleUpdateWeight(msg);
            break;
        case RQ_UpdateBackpackAfterUse:
            HandleBackpackAfterUse(msg);
            break;
        case RQ_SendBuyItemList:
            HandleBuyItemList(msg);
            break;
        case RQ_SendSellItemList:
            HandleSellItemList(msg);
            break;
        case RQ_SendTrainSkillList:
            HandleTrainSkillList(msg, false);
            break;
        case RQ_SendTeachSkillList:
            HandleTrainSkillList(msg, true);
            break;
        case RQ_FromPreInGameToInGame: {
            SkipOpcode(msg);
            char code = 0;
            if (!msg->isEnd()) {
                msg->Get(&code);
            }
            g_enterWorld46SentAt.store(0);
            if (code == 0) {
                g_waitingFromPreInGame.store(false);
                g_fromPreInGameResult.store(0);
                g_enterWorld46RetryCount.store(0);
                g_inGame.store(true);
                g_pipelineStep.store(6);
                {
                    std::lock_guard<std::mutex> lock(g_activeMutex);
                    ApplyRaceToPuppetAppearance(&g_activePlayer);
                    if (g_activePlayer.unitId != 0) {
                        RequestPuppetInformation(g_activePlayer.unitId, g_activePlayer.serverX,
                                                 g_activePlayer.serverY);
                    }
                }
                SendEnterWorldFollowUpPackets();
                TryFlushPendingNearItems();
            } else if ((code == 1 || code == 7) && g_enterWorld46RetryCount.load() < 8) {
                const int n = g_enterWorld46RetryCount.fetch_add(1) + 1;
                g_waitingFromPreInGame.store(true);
                g_fromPreInGameResult.store(-1);
                g_enterWorld46SentAt.store(SDL_GetTicks() - 2000);
                T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                                       "[PHASE] FromPreInGameToInGame (46) code=%d — retry %d/8",
                                       static_cast<int>(code), n);
            } else {
                g_waitingFromPreInGame.store(false);
                g_fromPreInGameResult.store(static_cast<int>(code));
            }
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] <- FromPreInGameToInGame (46) code=%d",
                                   static_cast<int>(code));
            break;
        }
        default:
            T4CNetworkDebugLog("[NET] opcode %d (%d octets)", static_cast<int>(opcode), msg->GetPos());
            break;
        }
    } catch (TFCPacketException *e) {
        int bufSize = 0;
        LPBYTE buf = nullptr;
        msg->GetBuffer(buf, bufSize);
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                               "[NET] TFCPacketException opcode=%d pos=%d size=%d cause=%u",
                               static_cast<int>(opcode), msg->GetPos(), bufSize,
                               e ? static_cast<unsigned>(e->m_cause) : 0U);
        delete e;
        FailAuthPhase("Erreur protocole reseau (paquet invalide).");
    }
}

bool T4CLoginSessionStart(const std::string &hostField, const std::string &portField, const std::string &login,
                          const std::string &password) {
    if (g_logoutInProgress.load()) {
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                               "[NET] Connexion refusee : deconnexion SafePlug encore en cours.");
        return false;
    }
    ReapFinishedLogoutThread();

    short port = PORT_SERVER;
    if (!portField.empty()) {
        port = static_cast<short>(std::atoi(portField.c_str()));
    }
    if (port <= 0) {
        port = PORT_SERVER;
    }

    const int curStep = g_pipelineStep.load();
    if (curStep >= 1 && curStep <= 3 && SameLoginEndpoint(hostField, portField, port)) {
        g_login = login;
        if (!password.empty()) {
            g_password = password;
        }
        T4CCommBridgePoll();
        g_authPhaseRetryCount.store(0);
        switch (curStep) {
        case 1:
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth,
                                   "[AUTH] Connect — renvoi Register (14, step=1, meme socket)");
            SendRegister(login, password);
            break;
        case 2:
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth,
                                   "[AUTH] Connect — renvoi AuthenticateServerVersion (99, step=2, meme socket)");
            SendAuthVersion();
            break;
        case 3:
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth,
                                   "[AUTH] Connect — renvoi GetPersonnalPClist (26, step=3, meme socket)");
            SendCharacterListRequest();
            break;
        default:
            break;
        }
        return true;
    }

    const Uint32 now = SDL_GetTicks();
    const Uint32 allowed = g_reconnectAllowedAfterTick.load();
    if (allowed != 0 && now < allowed) {
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                               "[NET] Connexion refusee : encore ~%d s avant reconnexion (liberation compte serveur).",
                               static_cast<int>((allowed - now + 999) / 1000));
        return false;
    }

    g_boQuitApp = false;
    T4CNetworkDebugBeginSession();
    g_login = login;
    if (!password.empty()) {
        g_password = password;
    } else if (g_password.empty()) {
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                               "[AUTH] Connexion refusee : mot de passe vide (saisir le mot de passe).");
        return false;
    }
    g_lastHost = hostField;
    g_lastPort = portField;
    g_registerRetryCount.store(0);
    g_authPhaseRetryCount.store(0);
    g_authPhaseSentAt.store(0);
    g_pendingAutoRegister.store(false);
    g_logoutSentThisSession.store(false);
    g_pipelineStep.store(0);
    g_pendingCharacterList.store(false);
    g_pendingEnterWorld.store(false);
    g_waitingPutPlayer.store(false);
    g_inGame.store(false);
    {
        std::lock_guard<std::mutex> lock(g_errMutex);
        g_authError.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_charMutex);
        g_characters.clear();
    }

    if (!T4CCommBridgeStart(hostField.c_str(), port)) {
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn, "[AUTH] T4CCommBridgeStart failed");
        return false;
    }

    T4CCommBridgePoll();
    SendRegister(login, password);
    g_pipelineStep.store(1);
    return true;
}

void T4CLoginSessionShutdown() {
    {
        std::lock_guard<std::mutex> tlock(g_logoutThreadMutex);
        if (g_logoutThread.joinable()) {
            g_logoutThread.join();
        }
    }
    const int step = g_pipelineStep.load();
    if (g_inGame.load()) {
        SendGracefulExitToServer();
    } else if (step >= 2) {
        SendCharSelectLogoutToServer();
    }
    T4CCommBridgeStop();
    ResetLoginSessionState();
    g_pipelineStep.store(0);
    g_boQuitApp = true;
    g_reconnectAllowedAfterTick.store(0);
    g_logoutInProgress.store(false);
}

void T4CLoginSessionDisconnectInGame() {
    std::lock_guard<std::mutex> tlock(g_logoutThreadMutex);
    if (g_logoutThread.joinable()) {
        return;
    }
    const int capturedStep = g_pipelineStep.load();
    if (capturedStep < 2) {
        return;
    }
    if (capturedStep >= 4 && !g_logoutSentThisSession.load()) {
        SendSafePlugLogout();
        g_logoutSentThisSession.store(true);
    }
    /* Attendre la reponse opcode 46 avant ExitGame — sinon le serveur reste preInGame et bloque Register. */
    if (capturedStep >= 5) {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
        while (g_fromPreInGameResult.load() < 0 &&
               std::chrono::steady_clock::now() < deadline) {
            T4CCommBridgePoll();
            SDL_Delay(20);
        }
    }
    if (capturedStep >= 6) {
        SendOpcodeOnly(static_cast<short>(RQ_ExitGame));
    }
    ResetLoginSessionState();
    g_pipelineStep.store(0);
    g_reconnectAllowedAfterTick.store(0);
    g_logoutInProgress.store(true);
    g_logoutThread = std::thread(LogoutWorker, capturedStep);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Net,
                           "[NET] Logout monde : SafePlug+ExitGame immediat, fermeture UDP en arriere-plan.");
}

bool T4CLoginSessionIsNetworkActive() {
    return g_pipelineStep.load() >= 1;
}

bool T4CLoginSessionIsLogoutInProgress() {
    return g_logoutInProgress.load();
}

int T4CLoginSessionGetReconnectCooldownSeconds() {
    const Uint32 now = SDL_GetTicks();
    const Uint32 allowed = g_reconnectAllowedAfterTick.load();
    if (allowed == 0 || now >= allowed) {
        return 0;
    }
    return static_cast<int>((allowed - now + 999) / 1000);
}

void T4CLoginSessionPollBackgroundTasks() {
    ReapFinishedLogoutThread();
    T4CCommBridgePoll();

    /* Keep-alive : tant que la session UDP est ouverte, renvoyer l'opcode 10 (RQ_Ack) apres
     * ~3 s sans envoi, pour empecher l'expiration cote serveur (sinon les moves apres une
     * periode d'inactivite sont jetes avant traitement). Remplace COMM.StartSendAlive() Windows. */
    if (!g_logoutInProgress.load() && T4CCommBridgeIsOpen()) {
        const Uint32 now = SDL_GetTicks();
        const Uint32 last = g_lastClientSendTick.load();
        if (last != 0 && now - last >= kKeepAliveIdleMs) {
            SendOpcodeOnly(static_cast<short>(RQ_Ack));
        }
    }

    if (g_pendingAutoRegister.load()) {
        const Uint32 now = SDL_GetTicks();
        if (now >= g_autoRegisterAt.load()) {
            g_pendingAutoRegister.store(false);
            if (g_pipelineStep.load() == 1) {
                g_registerRetryCount.store(0);
                T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth, "[AUTH] nouvel essai Register (code 1/2)");
                SendRegister(g_login, g_password);
            }
        }
        return;
    }

    const int step = g_pipelineStep.load();
    if (step == 1 && g_registerSentAt.load() != 0) {
        const Uint32 elapsed = SDL_GetTicks() - g_registerSentAt.load();
        if (elapsed >= 8000) {
            const int retries = g_registerRetryCount.fetch_add(1) + 1;
            if (retries >= 3) {
                FailAuthPhase("Serveur ne repond pas (timeout Register).");
                T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn, "[AUTH] timeout Register apres 3 essais");
                return;
            }
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth, "[AUTH] retry Register (%d/3)", retries + 1);
            SendRegister(g_login, g_password);
        }
        return;
    }

    if ((step == 2 || step == 3) && g_authPhaseSentAt.load() != 0) {
        const Uint32 elapsed = SDL_GetTicks() - g_authPhaseSentAt.load();
        if (elapsed < 5000) {
            return;
        }
        const int retries = g_authPhaseRetryCount.fetch_add(1) + 1;
        if (retries >= 6) {
            FailAuthPhase(step == 2 ? "Pas de reponse serveur (99)." : "Pas de reponse serveur (26).");
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                                   "[AUTH] timeout opcode %d apres 6 essais", step == 2 ? 99 : 26);
            return;
        }
        if (step == 2) {
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth, "[AUTH] retry AuthenticateServerVersion (99) (%d/6)",
                                   retries + 1);
            SendAuthVersion();
        } else {
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth, "[AUTH] retry GetPersonnalPClist (26) (%d/6)",
                                   retries + 1);
            SendCharacterListRequest();
        }
        return;
    }

    if (g_waitingPutPlayer.load() && g_putPlayerSentAt.load() != 0 && !g_selectedPlayer.empty()) {
        const Uint32 elapsed = SDL_GetTicks() - g_putPlayerSentAt.load();
        if (elapsed >= 8000) {
            const int retries = g_putPlayerRetryCount.fetch_add(1) + 1;
            if (retries >= 4) {
                {
                    std::lock_guard<std::mutex> lock(g_errMutex);
                    g_putPlayerError = "Pas de reponse serveur (opcode 13 PutPlayerInGame).";
                }
                g_waitingPutPlayer.store(false);
                g_putPlayerSentAt.store(0);
                T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn, "[AUTH] timeout PutPlayerInGame apres 4 essais");
                return;
            }
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth, "[AUTH] retry PutPlayerInGame (13) (%d/4)",
                                   retries + 1);
            SendPutPlayerInGame(g_selectedPlayer);
        }
        return;
    }

    if (g_waitingCreate.load() && g_createSentAt.load() != 0 && !g_pendingCreateName.empty()) {
        const Uint32 elapsed = SDL_GetTicks() - g_createSentAt.load();
        if (elapsed >= 8000) {
            const int retries = g_createRetryCount.fetch_add(1) + 1;
            if (retries >= 4) {
                SetCreatePlayerErrorMessage("Pas de reponse serveur (opcode 25 CreatePlayer).");
                T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                                       "[PHASE] timeout RQ_CreatePlayer (25) apres 4 essais");
                return;
            }
            if (retries >= 2) {
                T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth,
                                       "[AUTH] retry create — re-auth opcode 99 avant opcode 25");
                g_pendingCreateAfterAuth.store(true);
                SendAuthVersion();
                return;
            }
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] retry RQ_CreatePlayer (25) (%d/4)",
                                   retries + 1);
            SendCreatePlayerPacket(g_pendingCreateName, g_pendingCreateSex, g_pendingCreateStats);
        }
        return;
    }

    if (g_waitingDelete.load() && g_deleteSentAt.load() != 0 && !g_pendingDeleteName.empty()) {
        const Uint32 elapsed = SDL_GetTicks() - g_deleteSentAt.load();
        if (elapsed >= 8000) {
            const int retries = g_deleteRetryCount.fetch_add(1) + 1;
            if (retries >= 4) {
                SetDeletePlayerErrorMessage("Pas de reponse serveur (opcode 15 DeletePlayer).");
                T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                                       "[PHASE] timeout RQ_DeletePlayer (15) apres 4 essais");
                return;
            }
            if (retries >= 2) {
                T4CNetworkDebugLogKind(T4CMatrixLogKind::Auth,
                                       "[AUTH] retry delete — re-auth opcode 99 avant opcode 15");
                g_pendingDeleteAfterAuth.store(true);
                SendAuthVersion();
                return;
            }
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] retry RQ_DeletePlayer (15) (%d/4)",
                                   retries + 1);
            SendDeletePlayerPacket(g_pendingDeleteName);
        }
        return;
    }

    if (g_waitingFromPreInGame.load() && g_fromPreInGameResult.load() < 0 && g_enterWorld46SentAt.load() != 0) {
        const Uint32 elapsed = SDL_GetTicks() - g_enterWorld46SentAt.load();
        if (elapsed >= 2500) {
            const int retries = g_enterWorld46RetryCount.fetch_add(1) + 1;
            if (retries >= 8) {
                T4CNetworkDebugLogKind(T4CMatrixLogKind::Warn,
                                       "[PHASE] timeout FromPreInGameToInGame (46) apres 8 essais");
                g_waitingFromPreInGame.store(false);
                g_enterWorld46SentAt.store(0);
                return;
            }
            g_enterWorld46SentAt.store(SDL_GetTicks());
            SendOpcodeOnly(static_cast<short>(RQ_FromPreInGameToInGame));
            T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] retry FromPreInGameToInGame (46) (%d/8)",
                                   retries + 1);
        }
    }

    TryFlushPendingNearItems();
}

std::string T4CLoginSessionGetWorldHudLine() {
    T4CActivePlayer ap{};
    T4CLoginSessionGetActivePlayer(&ap);
    if (ap.valid) {
        char buf[160];
        std::snprintf(buf, sizeof(buf), "%s @ %u,%u | fleches=deplacement", ap.name.c_str(), ap.serverX,
                      ap.serverY);
        return buf;
    }
    return "En jeu";
}

void T4CLoginSessionAbortLogin() {
    if (g_inGame.load()) {
        SendGracefulExitToServer();
    }
    /* Login / persos / monde : conserver le socket UDP (reconnect instantanee). */
    ResetLoginSessionState();
    g_pipelineStep.store(0);
    g_reconnectAllowedAfterTick.store(0);
}

bool T4CLoginSessionIsAuthInProgress() {
    const int step = g_pipelineStep.load();
    return step >= 1 && step <= 5;
}

std::string T4CLoginSessionGetLastAuthError() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    return g_authError;
}

void T4CLoginSessionResetAfterReturnToLogin() {
    g_boQuitApp = false;
    g_pipelineStep.store(0);
    ResetLoginSessionState();
}

bool T4CLoginSessionConsumeCharacterListReady() {
    return g_pendingCharacterList.exchange(false);
}

void T4CLoginSessionCopyCharacterList(std::vector<T4CCharacterSlot> *outSlots, int *outMaxPerAccount) {
    if (!outSlots) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_charMutex);
    *outSlots = g_characters;
    if (outMaxPerAccount) {
        *outMaxPerAccount = g_maxCharsPerAccount;
    }
}

bool T4CLoginSessionRequestCreatePlayer(const std::string &name, unsigned char sex, const unsigned char stats[5]) {
    if (name.empty() || !stats || g_waitingCreate.load() || g_waitingPutPlayer.load() ||
        g_inCreateRerollPhase.load()) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_errMutex);
        g_createError.clear();
    }
    g_pendingCreateName = name;
    std::memcpy(g_pendingCreateStats, stats, 5);
    g_pendingCreateSex = sex;
    g_createRetryCount.store(0);
    SendCreatePlayerPacket(name, sex, stats);
    g_waitingCreate.store(true);
    return true;
}

bool T4CLoginSessionIsWaitingCreatePlayer() {
    return g_waitingCreate.load();
}

bool T4CLoginSessionHasCreatePlayerError() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    return !g_createError.empty();
}

std::string T4CLoginSessionGetCreatePlayerErrorMessage() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    return g_createError;
}

void T4CLoginSessionClearCreatePlayerError() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    g_createError.clear();
}

void T4CLoginSessionPrepareForCreateScreen() {
    T4CLoginSessionClearCreatePlayerError();
    g_waitingCreate.store(false);
    g_createSentAt.store(0);
    g_createRetryCount.store(0);
    g_waitingReroll.store(false);
    g_rolledStatsPending.store(false);
    g_inCreateRerollPhase.store(false);
    g_autoEnterWorldAfterCreate.store(false);
}

bool T4CLoginSessionConsumeCreatePlayerSuccess() {
    return g_pendingCreateSuccess.exchange(false);
}

bool T4CLoginSessionIsInCreateRerollPhase() {
    return g_inCreateRerollPhase.load();
}

bool T4CLoginSessionConsumeRolledStatsUpdate(T4CCharacterRolledStats *outStats) {
    if (!g_rolledStatsPending.exchange(false)) {
        return false;
    }
    if (outStats) {
        std::lock_guard<std::mutex> lock(g_rolledMutex);
        *outStats = g_rolledStats;
    }
    return true;
}

bool T4CLoginSessionRequestCreateReroll() {
    if (!g_inCreateRerollPhase.load() || g_waitingReroll.load()) {
        return false;
    }
    SendOpcodeOnly(static_cast<short>(RQ_Reroll));
    g_waitingReroll.store(true);
    return true;
}

bool T4CLoginSessionConfirmCreateReroll() {
    if (!g_inCreateRerollPhase.load()) {
        return false;
    }
    g_autoEnterWorldAfterCreate.store(true);
    g_inCreateRerollPhase.store(false);
    g_waitingReroll.store(false);
    SendCharacterListRequest();
    return true;
}

bool T4CLoginSessionCancelCreateReroll() {
    if (!g_inCreateRerollPhase.load() || g_pendingCreateName.empty()) {
        return false;
    }
    T4CLoginSessionRequestDeletePlayer(g_pendingCreateName);
    g_inCreateRerollPhase.store(false);
    g_waitingReroll.store(false);
    g_pendingCreateName.clear();
    return true;
}

bool T4CLoginSessionRequestDeletePlayer(const std::string &playerName) {
    if (playerName.empty() || g_waitingDelete.load()) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_errMutex);
        g_deleteError.clear();
    }
    g_pendingDeleteName = playerName;
    g_deleteRetryCount.store(0);
    SendDeletePlayerPacket(playerName);
    g_waitingDelete.store(true);
    return true;
}

bool T4CLoginSessionIsWaitingDeletePlayer() {
    return g_waitingDelete.load();
}

bool T4CLoginSessionHasDeletePlayerError() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    return !g_deleteError.empty();
}

std::string T4CLoginSessionGetDeletePlayerErrorMessage() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    return g_deleteError;
}

void T4CLoginSessionClearDeletePlayerError() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    g_deleteError.clear();
}

bool T4CLoginSessionRequestQueryNameExistence(const std::string &) {
    return true;
}

bool T4CLoginSessionRequestPutPlayerInGame(const std::string &playerName) {
    if (g_waitingPutPlayer.load()) {
        return false;
    }
    g_selectedPlayer = playerName;
    {
        std::lock_guard<std::mutex> lock(g_activeMutex);
        g_activePlayer = {};
        g_activePlayer.name = playerName;
        g_activePlayer.valid = true;
        std::lock_guard<std::mutex> cl(g_charMutex);
        for (const T4CCharacterSlot &slot : g_characters) {
            if (slot.name == playerName) {
                g_activePlayer.race = slot.race;
                g_activePlayer.level = slot.level;
                ApplyRaceToPuppetAppearance(&g_activePlayer);
                break;
            }
        }
    }
    SendPutPlayerInGame(playerName);
    return true;
}

bool T4CLoginSessionIsWaitingPutPlayerInGame() {
    return g_waitingPutPlayer.load();
}

bool T4CLoginSessionHasPutPlayerInGameError() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    return !g_putPlayerError.empty();
}

std::string T4CLoginSessionGetPutPlayerInGameErrorMessage() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    return g_putPlayerError;
}

void T4CLoginSessionClearPutPlayerInGameError() {
    std::lock_guard<std::mutex> lock(g_errMutex);
    g_putPlayerError.clear();
}

bool T4CLoginSessionConsumeEnterWorldReady(T4CEnterWorldSpawn *outSpawn) {
    if (!g_pendingEnterWorld.exchange(false)) {
        return false;
    }
    if (outSpawn) {
        *outSpawn = g_spawn;
    }
    return g_spawn.valid;
}

void T4CLoginSessionGetActivePlayer(T4CActivePlayer *outPlayer) {
    if (!outPlayer) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_activeMutex);
    *outPlayer = g_activePlayer;
}

void T4CLoginSessionGetPlayerStatus(T4CPlayerStatus *outStatus) {
    if (!outStatus) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_statusMutex);
    *outStatus = g_playerStatus;
}

bool T4CLoginSessionConsumePlayerStatusUpdate(T4CPlayerStatus *outStatus) {
    if (!g_statusDirty.exchange(false)) {
        return false;
    }
    if (outStatus) {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        *outStatus = g_playerStatus;
    }
    return true;
}

bool T4CLoginSessionRequestPlayerStatus() {
    SendOpcodeOnly(static_cast<short>(RQ_GetStatus));
    return true;
}

void T4CLoginSessionGetBackpack(T4CPlayerBackpack *out) {
    if (!out) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_inventoryMutex);
    *out = g_backpack;
}

void T4CLoginSessionGetSkillBook(T4CPlayerSkillBook *out) {
    if (!out) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_inventoryMutex);
    *out = g_skillBook;
}

void T4CLoginSessionGetSpellBook(T4CPlayerSpellBook *out) {
    if (!out) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_inventoryMutex);
    *out = g_spellBook;
}

void T4CLoginSessionGetBankChest(T4CPlayerBankChest *out) {
    if (out) {
        *out = {};
    }
}

void T4CLoginSessionGetEquipment(T4CPlayerEquipment *out) {
    if (!out) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_inventoryMutex);
    *out = g_equipment;
}

bool T4CLoginSessionConsumeInventoryUpdate() {
    return g_inventoryDirty.exchange(false);
}

bool T4CLoginSessionRequestSkillList() {
    SendOpcodeOnly(static_cast<short>(RQ_GetSkillList));
    return true;
}

bool T4CLoginSessionRequestSpellList() {
    /* V3_SpellDlg::RequestSpellList : opcode 62 + octet 1 (sinon ValidPacket FAIL serveur). */
    TFCPacket p;
    p << static_cast<short>(RQ_SendSpellList);
    p << static_cast<char>(1);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionRequestViewBackpack() {
    SendOpcodeOnly(static_cast<short>(RQ_ViewInv));
    return true;
}

bool T4CLoginSessionRequestViewEquipped() {
    SendOpcodeOnly(static_cast<short>(RQ_ViewEquiped));
    return true;
}

void T4CLoginSessionPollItemNameRequests(T4CItemSearchPlace, int) {}

bool T4CLoginSessionIsBankChestUiVisible() {
    return false;
}

bool T4CLoginSessionConsumePlayerPopupUpdate(T4CActivePlayer *outPlayer) {
    if (!g_popupPending.exchange(false)) {
        return false;
    }
    T4CLoginSessionGetActivePlayer(outPlayer);
    return true;
}

bool T4CLoginSessionConsumePlayerTeleport(T4CPlayerTeleport *outTeleport) {
    if (!g_pendingTeleport.exchange(false)) {
        return false;
    }
    if (outTeleport) {
        std::lock_guard<std::mutex> lock(g_teleportMutex);
        *outTeleport = g_teleportPayload;
    }
    return true;
}

bool T4CLoginSessionIsWorldSessionReady() {
    return IsWorldSessionReady();
}

bool T4CLoginSessionRequestNearItems() {
    if (!IsWorldSessionReady()) {
        g_pendingNearItems.store(true);
        T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase,
                               "[PHASE] -> RQ_GetNearItems (60) differe (attente opcode 46)");
        return true;
    }
    g_expectInviewRefresh.store(true);
    SendOpcodeOnly(static_cast<short>(RQ_GetNearItems));
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] -> RQ_GetNearItems (60)");
    return true;
}

bool T4CLoginSessionSendMove(std::uint16_t moveOpcode) {
    const int phase46 = g_fromPreInGameResult.load();
    if (!g_inGame.load() || phase46 < 0 || moveOpcode < 1 || moveOpcode > 8) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(moveOpcode);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendAttack(const unsigned int targetX, const unsigned int targetY,
                                 const std::int32_t targetUnitId) {
    if (!g_inGame.load()) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_Attack);
    p << static_cast<short>(targetX);
    p << static_cast<short>(targetY);
    p << static_cast<long>(targetUnitId);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendDirectedTalk(const unsigned int targetX, const unsigned int targetY,
                                       const std::int32_t targetUnitId, const int direction) {
    if (!g_inGame.load() || targetUnitId == 0) {
        return false;
    }
    SetTalkTarget(targetUnitId, targetX, targetY);
    TFCPacket p;
    p << static_cast<short>(RQ_DirectedTalk);
    p << static_cast<short>(targetX);
    p << static_cast<short>(targetY);
    p << static_cast<long>(targetUnitId);
    p << static_cast<char>(direction);
    p << static_cast<long>(0);
    p << static_cast<long>(g_sessionKey);
    p << static_cast<short>(0);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendDirectedTalkLink(const std::string &linkText) {
    if (!g_inGame.load() || linkText.empty()) {
        return false;
    }
    T4CTalkState talk{};
    T4CLoginSessionGetTalkState(&talk);
    if (!talk.active || talk.unitId == 0) {
        return false;
    }
    const short len = static_cast<short>(std::min<std::size_t>(linkText.size(), 255));
    TFCPacket p;
    p << static_cast<short>(RQ_DirectedTalkNoFeed);
    p << static_cast<short>(talk.x);
    p << static_cast<short>(talk.y);
    p << static_cast<long>(talk.unitId);
    p << static_cast<char>(2);
    p << static_cast<long>(0);
    p << static_cast<long>(g_sessionKey);
    p << len;
    for (int i = 0; i < len; ++i) {
        p << linkText[static_cast<size_t>(i)];
    }
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendBreakConversation() {
    if (!g_inGame.load()) {
        return false;
    }
    T4CTalkState talk{};
    T4CLoginSessionGetTalkState(&talk);
    if (!talk.active) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_BreakConversation);
    p << static_cast<long>(talk.unitId);
    p << static_cast<short>(talk.x);
    p << static_cast<short>(talk.y);
    SendPacket(p);
    SetTalkTarget(0, 0, 0);
    T4CLoginSessionCloseShop();
    return true;
}

void T4CLoginSessionGetTalkState(T4CTalkState *outTalk) {
    if (!outTalk) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_talkMutex);
    *outTalk = g_talkState;
}

bool T4CLoginSessionConsumeNpcSpeech(T4CNpcSpeech *outSpeech) {
    if (!g_speechPending.exchange(false)) {
        return false;
    }
    if (outSpeech) {
        std::lock_guard<std::mutex> lock(g_talkMutex);
        *outSpeech = g_pendingSpeech;
    }
    return true;
}

bool T4CLoginSessionConsumeShopList(T4CShopList *outShop) {
    if (!g_shopDirty.exchange(false)) {
        return false;
    }
    if (outShop) {
        std::lock_guard<std::mutex> lock(g_talkMutex);
        *outShop = g_shopList;
    }
    return true;
}

void T4CLoginSessionGetShopList(T4CShopList *outShop) {
    if (!outShop) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_talkMutex);
    *outShop = g_shopList;
}

void T4CLoginSessionCloseShop() {
    std::lock_guard<std::mutex> lock(g_talkMutex);
    g_shopList = {};
    g_shopDirty.store(false);
}

bool T4CLoginSessionSendUseObject(const unsigned int targetX, const unsigned int targetY,
                                    const std::int32_t unitId) {
    if (!g_inGame.load()) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_UseObject);
    p << static_cast<short>(targetX);
    p << static_cast<short>(targetY);
    p << static_cast<long>(unitId);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendGetObject(const unsigned int targetX, const unsigned int targetY,
                                    const std::int32_t unitId) {
    if (!g_inGame.load()) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_GetObject);
    p << static_cast<short>(targetX);
    p << static_cast<short>(targetY);
    p << static_cast<long>(unitId);
    SendPacket(p);
    return true;
}

namespace {

void AppendPacketString(TFCPacket &p, const std::string &text, const std::size_t maxLen = 1023) {
    const short len = static_cast<short>(std::min(text.size(), maxLen));
    p << len;
    for (int i = 0; i < len; ++i) {
        p << text[static_cast<size_t>(i)];
    }
}

}  // namespace

bool T4CLoginSessionSendBuyItems(const std::vector<std::pair<std::int32_t, std::uint16_t>> &items) {
    if (!g_inGame.load() || items.empty()) {
        return false;
    }
    T4CTalkState talk{};
    T4CLoginSessionGetTalkState(&talk);
    if (!talk.active || talk.unitId == 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_SendBuyItemList);
    p << static_cast<short>(talk.x);
    p << static_cast<short>(talk.y);
    p << static_cast<long>(talk.unitId);
    p << static_cast<short>(items.size());
    for (const auto &[objectId, qty] : items) {
        p << static_cast<short>(objectId);
        p << static_cast<short>(qty);
    }
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendSellItems(const std::vector<std::pair<std::int32_t, std::uint32_t>> &items) {
    if (!g_inGame.load() || items.empty()) {
        return false;
    }
    T4CTalkState talk{};
    T4CLoginSessionGetTalkState(&talk);
    if (!talk.active || talk.unitId == 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_SendSellItemList);
    p << static_cast<short>(talk.x);
    p << static_cast<short>(talk.y);
    p << static_cast<long>(talk.unitId);
    p << static_cast<short>(items.size());
    for (const auto &[objectId, qty] : items) {
        p << static_cast<long>(objectId);
        p << static_cast<long>(qty);
    }
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendTrainSkills(const std::vector<std::pair<std::uint16_t, std::uint16_t>> &skills) {
    if (!g_inGame.load() || skills.empty()) {
        return false;
    }
    T4CTalkState talk{};
    T4CLoginSessionGetTalkState(&talk);
    if (!talk.active || talk.unitId == 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_SendTrainSkillList);
    p << static_cast<short>(talk.x);
    p << static_cast<short>(talk.y);
    p << static_cast<long>(talk.unitId);
    p << static_cast<short>(skills.size());
    for (const auto &[skillId, points] : skills) {
        p << static_cast<short>(skillId);
        p << static_cast<short>(points);
    }
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendTeachSkills(const std::vector<std::uint16_t> &skillIds) {
    if (!g_inGame.load() || skillIds.empty()) {
        return false;
    }
    T4CTalkState talk{};
    T4CLoginSessionGetTalkState(&talk);
    if (!talk.active || talk.unitId == 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_SendTeachSkillList);
    p << static_cast<short>(talk.x);
    p << static_cast<short>(talk.y);
    p << static_cast<long>(talk.unitId);
    p << static_cast<short>(skillIds.size());
    for (const std::uint16_t skillId : skillIds) {
        p << static_cast<short>(skillId);
    }
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendEquipObject(const std::int32_t itemId) {
    if (!g_inGame.load() || itemId == 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_EquipObject);
    p << static_cast<long>(itemId);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendUnequipObject(const T4CEquipSlot slot) {
    if (!g_inGame.load()) {
        return false;
    }
    /* T4CEquipSlot (ordre opcode 19) -> code position Windows TFCPlayer.h (body=0, feet=1, ...). */
    char equipPos = -1;
    switch (slot) {
        case T4CEquipSlot::Body: equipPos = 0; break;
        case T4CEquipSlot::Feet: equipPos = 1; break;
        case T4CEquipSlot::Gloves: equipPos = 2; break;
        case T4CEquipSlot::Helm: equipPos = 3; break;
        case T4CEquipSlot::Legs: equipPos = 4; break;
        case T4CEquipSlot::Bracelets: equipPos = 6; break;
        case T4CEquipSlot::Necklace: equipPos = 7; break;
        case T4CEquipSlot::WeaponRight: equipPos = 8; break;
        case T4CEquipSlot::WeaponLeft: equipPos = 9; break;
        case T4CEquipSlot::Ring1: equipPos = 11; break;
        case T4CEquipSlot::Ring2: equipPos = 12; break;
        case T4CEquipSlot::Belt: equipPos = 14; break;
        case T4CEquipSlot::Cape: equipPos = 15; break;
    }
    if (equipPos < 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_UnequipObject);
    p << equipPos;
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendDropObject(const std::int32_t itemId, const std::uint32_t qty) {
    if (!g_inGame.load() || itemId == 0 || qty == 0) {
        return false;
    }
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    TFCPacket p;
    p << static_cast<short>(RQ_DepositObject);
    p << static_cast<short>(active.serverX);
    p << static_cast<short>(active.serverY);
    p << static_cast<long>(itemId);
    p << static_cast<long>(qty);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendJunkItems(const std::int32_t itemId, const std::uint32_t qty) {
    if (!g_inGame.load() || itemId == 0 || qty == 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_JunkItems);
    p << static_cast<long>(itemId);
    p << static_cast<long>(qty);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendUseBagItem(const std::int32_t itemId) {
    if (!g_inGame.load() || itemId == 0) {
        return false;
    }
    /* V3_InvDlg::UseItemDirect : RQ_UseObject coords (0,0) + itemId = usage sur soi. */
    TFCPacket p;
    p << static_cast<short>(RQ_UseObject);
    p << static_cast<short>(0);
    p << static_cast<short>(0);
    p << static_cast<long>(itemId);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendCastSpell(const std::uint16_t spellId, const unsigned int targetX,
                                  const unsigned int targetY, const std::int32_t targetId) {
    if (!g_inGame.load() || spellId == 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_CastSpell);
    p << static_cast<short>(spellId);
    p << static_cast<short>(targetX);
    p << static_cast<short>(targetY);
    p << static_cast<long>(targetId);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendUseSkill(const std::uint16_t skillId) {
    if (!g_inGame.load() || skillId == 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_UseSkill);
    p << static_cast<short>(skillId);
    p << static_cast<short>(0);
    p << static_cast<short>(0);
    p << static_cast<long>(0);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendAttackMode(const bool enable) {
    if (!g_inGame.load()) {
        return false;
    }
    /* VisualObjectList::AttackMode (Windows) : opcode 198 + long (1=activer, 0=couper). */
    TFCPacket p;
    p << static_cast<short>(RQ_AttackMode);
    p << static_cast<long>(enable ? 1 : 0);
    SendPacket(p);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] -> RQ_AttackMode (198) ask=%d",
                           enable ? 1 : 0);
    return true;
}

bool T4CLoginSessionGetAttackMode() {
    return g_attackMode.load();
}

bool T4CLoginSessionSendChatLocal(const std::string &text) {
    if (!g_inGame.load() || text.empty()) {
        return false;
    }
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    if (active.unitId == 0) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_IndirectTalk);
    p << static_cast<long>(active.unitId);
    p << static_cast<char>(0);
    p << static_cast<long>(0x00FFFFFF);
    p << static_cast<long>(g_sessionKey);
    AppendPacketString(p, text);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendShout(const std::string &text) {
    if (!g_inGame.load() || text.empty()) {
        return false;
    }
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    TFCPacket p;
    p << static_cast<short>(RQ_Shout);
    AppendPacketString(p, active.name, 255);
    p << static_cast<long>(0x00FFFFFF);
    AppendPacketString(p, text);
    SendPacket(p);
    return true;
}

bool T4CLoginSessionSendPage(const std::string &targetName, const std::string &text) {
    if (!g_inGame.load() || targetName.empty() || text.empty()) {
        return false;
    }
    TFCPacket p;
    p << static_cast<short>(RQ_Page);
    AppendPacketString(p, targetName, 255);
    AppendPacketString(p, text);
    SendPacket(p);
    return true;
}

void T4CLoginSessionDrainChatMessages(std::vector<T4CChatMessage> *outMessages) {
    if (!outMessages) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_chatMutex);
    outMessages->insert(outMessages->end(), std::make_move_iterator(g_chatMessages.begin()),
                        std::make_move_iterator(g_chatMessages.end()));
    g_chatMessages.clear();
}

void T4CLoginSessionGetGameTime(T4CGameTime *outTime) {
    if (!outTime) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_timeMutex);
    *outTime = g_gameTime;
}

bool T4CLoginSessionRequestGameTime() {
    if (!g_inGame.load()) {
        return false;
    }
    SendOpcodeOnly(static_cast<short>(RQ_GetTime));
    return true;
}

void T4CLoginSessionPushSystemMessage(const std::string &text) {
    T4CChatMessage chat{};
    chat.kind = T4CChatKind::System;
    chat.text = text;
    chat.color = 0xFFD060u;
    PushChatMessage(std::move(chat));
}

bool T4CLoginSessionConsumeLocalPlayerMove(unsigned int *outX, unsigned int *outY,
                                             std::uint16_t *outMoveOpcode) {
    if (!g_localMovePending.exchange(false)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_localMoveMutex);
    if (outX) {
        *outX = g_localMoveX;
    }
    if (outY) {
        *outY = g_localMoveY;
    }
    if (outMoveOpcode) {
        *outMoveOpcode = g_localMoveOpcode;
    }
    return true;
}

void T4CLoginSessionUpdateActivePlayerPosition(unsigned int x, unsigned int y) {
    std::lock_guard<std::mutex> lock(g_activeMutex);
    g_activePlayer.serverX = x;
    g_activePlayer.serverY = y;
}

void T4CLoginSessionDrainRemoteUnitEvents(std::vector<T4CRemoteUnitEvent> *outEvents) {
    if (!outEvents) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    *outEvents = std::move(g_remoteEvents);
    g_remoteEvents.clear();
}

#include "T4CCreatureAppearanceTable.inc"

const char *T4CSpriteNameFromAppearance(const std::uint16_t appearance) {
    // Puppets generiques : sprite dedie via equipement (T4CRemoteUnitSpriteName).
    if (IsPuppetAppearance(appearance)) {
        return nullptr;
    }
    // Table auto-generee depuis le client Windows (apparence -> sprite 3D).
    const auto *begin = std::begin(kCreatureSpriteTable);
    const auto *end = std::end(kCreatureSpriteTable);
    const auto *it = std::lower_bound(begin, end, appearance,
                                      [](const T4CCreatureSprite &e, std::uint16_t v) { return e.appearance < v; });
    if (it != end && it->appearance == appearance) {
        return it->sprite;
    }
    return nullptr;
}

void T4CCollectCreaturePreloadBases(std::vector<std::string> *out) {
    if (!out) {
        return;
    }
    for (const T4CCreatureSprite &row : kCreatureSpriteTable) {
        if (!row.sprite || row.sprite[0] == '\0') {
            continue;
        }
        const std::string base(row.sprite);
        if (std::find(out->begin(), out->end(), base) == out->end()) {
            out->push_back(base);
        }
    }
}

bool T4CIsPuppetAppearance(const std::uint16_t appearance) {
    return IsPuppetAppearance(appearance);
}

bool T4CLoginSessionGetRemotePuppetDress(const std::int32_t unitId, T4CPuppetDress *outDress) {
    if (!outDress || unitId == 0) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    const auto it = g_remotePuppet.find(unitId);
    if (it == g_remotePuppet.end() || !it->second.known) {
        return false;
    }
    *outDress = it->second;
    return true;
}

const char *T4CRemoteUnitSpriteName(const std::int32_t unitId, const std::uint16_t appearance) {
    (void)unitId;
    if (IsPuppetAppearance(appearance)) {
        return nullptr;
    }
    return T4CSpriteNameFromAppearance(appearance);
}

bool T4CLoginSessionShouldSkipRemoteUnit(std::uint16_t appearance, std::int32_t unitId, unsigned int x,
                                         unsigned int y) {
    std::lock_guard<std::mutex> lock(g_activeMutex);
    if (!g_activePlayer.valid) {
        return false;
    }
    if (g_activePlayer.unitId != 0 && unitId == g_activePlayer.unitId) {
        return true;
    }
    if (IsActualPlayerAppearance(appearance) && x == g_activePlayer.serverX && y == g_activePlayer.serverY) {
        return true;
    }
    return false;
}

void T4CLoginSessionClearRemoteUnits() {
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    g_remoteEvents.clear();
    g_groundMarkers.clear();
    g_remoteAppearance.clear();
    g_remotePuppet.clear();
    g_pendingPuppetRequest.clear();
    g_unitNames.clear();
    g_pendingUnitName.clear();
}

void T4CLoginSessionCopyGroundObjectMarkers(std::vector<T4CGroundObjectMarker> *outMarkers) {
    if (!outMarkers) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    *outMarkers = g_groundMarkers;
}

bool T4CLoginSessionConsumePlayerDeath() {
    return g_deathPending.exchange(false);
}

void T4CLoginSessionRequestUnitName(const std::int32_t unitId, const unsigned int x, const unsigned int y) {
    RequestUnitName(unitId, x, y);
}

bool T4CLoginSessionGetUnitName(const std::int32_t unitId, std::string *outName, std::uint32_t *outColor) {
    std::lock_guard<std::mutex> lock(g_remoteMutex);
    const auto it = g_unitNames.find(unitId);
    if (it == g_unitNames.end() || it->second.name.empty()) {
        return false;
    }
    if (outName) {
        *outName = it->second.name;
    }
    if (outColor) {
        *outColor = it->second.color;
    }
    return true;
}

bool T4CLoginSessionConsumeLocalAttack(unsigned *targetX, unsigned *targetY) {
    std::lock_guard<std::mutex> lock(g_localAttackMutex);
    if (!g_localAttackPending) {
        return false;
    }
    g_localAttackPending = false;
    if (targetX) {
        *targetX = g_localAttackTargetX;
    }
    if (targetY) {
        *targetY = g_localAttackTargetY;
    }
    return true;
}

bool T4CLoginSessionIsPlayerDead() {
    return g_playerDead.load();
}

void T4CLoginSessionGetDeathState(T4CDeathState *outState) {
    if (!outState) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_deathMutex);
    *outState = g_deathState;
}

void T4CLoginSessionClearPlayerDeath() {
    g_playerDead.store(false);
    g_deathPending.store(false);
    g_deathResurrectSent.store(false);
    std::lock_guard<std::mutex> lock(g_deathMutex);
    g_deathState = {};
}

bool T4CLoginSessionSendDeathResurrect() {
    if (!WorldSessionReceiving()) {
        return false;
    }
    if (g_deathResurrectSent.exchange(true)) {
        return false;
    }
    TFCPacket packet;
    packet << static_cast<short>(RQ_NM_DeathResurect);
    SendPacket(packet);
    T4CNetworkDebugLogKind(T4CMatrixLogKind::Phase, "[PHASE] -> RQ_NM_DeathResurect (202) manuel");
    return true;
}

void T4CLoginSessionDrainFloatingDamage(std::vector<T4CFloatingDamage> *outDamage) {
    if (!outDamage) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_damageMutex);
    *outDamage = std::move(g_floatingDamage);
    g_floatingDamage.clear();
}

bool T4CLoginSessionConsumeNetworkSuccessDialog() {
    return false;
}

const char *T4CPlayerSpriteNpcName(const T4CActivePlayer &player) {
    if (player.puppetKnown) {
        return nullptr;
    }
    static const char *const kClassSprites[] = {"Warrio", "Wizard", "Cleric", "Thief"};
    int cls = -1;
    if (player.appearance >= 10001 && player.appearance <= 10004) {
        cls = static_cast<int>(player.appearance - 10001);
    } else if (player.appearance >= 15001 && player.appearance <= 15004) {
        cls = static_cast<int>(player.appearance - 15001);
    } else if (player.classIndex <= 3) {
        cls = static_cast<int>(player.classIndex);
    }
    if (cls < 0 || cls > 3) {
        cls = 0;
    }
    return kClassSprites[cls];
}

std::string T4CPlayerStandingSpriteName(const T4CActivePlayer &player) {
    return T4CPlayerUnitSpriteName(player, 1, 0);
}

T4CUnitSpriteView T4CUnitSpriteViewFromDirection(const int direction) {
    T4CUnitSpriteView view{};
    switch (direction) {
        case 1:
            view.angleDegrees = 45;
            break;
        case 2:
            view.angleDegrees = 0;
            break;
        case 3:
            view.angleDegrees = 45;
            view.mirror = true;
            break;
        case 4:
            view.angleDegrees = 90;
            break;
        case 6:
            view.angleDegrees = 90;
            view.mirror = true;
            break;
        case 7:
            view.angleDegrees = 135;
            break;
        case 8:
            view.angleDegrees = 180;
            break;
        case 9:
            view.angleDegrees = 135;
            view.mirror = true;
            break;
        default:
            view.angleDegrees = 0;
            break;
    }
    return view;
}


std::string T4CPlayerUnitSpriteName(const T4CActivePlayer &player, const int direction, const int frameIndex) {
    const char *base = T4CPlayerSpriteNpcName(player);
    if (!base || base[0] == '\0') {
        return {};
    }
    return T4CUnitSpriteFrameName(base, T4CUnitSpriteViewFromDirection(direction), frameIndex);
}
