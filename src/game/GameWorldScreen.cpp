#include "game/GameWorldScreen.h"
#include "game/T4CGameHud.h"
#include "game/T4CGroundObjectSprites.h"
#include "game/T4CPuppetDraw.h"
#include "game/T4CMovementBaseline.h"
#include "game/T4CUnitSpriteNames.h"

#include "audio/T4CGameMusic.h"
#include "gui/LauncherChrome.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>

namespace {

constexpr float kCenterX = static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f;
constexpr float kCenterY = static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f;

/* Au-dela de cette distance (tuiles) une unite/objet est hors champ serveur (_DEFAULT_RANGE=80) :
 * on la masque pour eviter les sprites fantomes qui trainent. */
constexpr int kRemoteUnitCullRange = 90;

/* Familles de portes (objets serveur) : valeur CLOSED puis OPENED + frames d'anim contigues.
 * Cf. client/T4C Client/Apparence.h (__OBJGROUP_*_DOOR*). */
bool isDoorAppearance(const std::uint16_t a) {
    return a == 15 || a == 16 || a == 297 || (a >= 769 && a <= 781) || (a >= 791 && a <= 803) ||
           (a >= 804 && a <= 816) || (a >= 817 && a <= 829) || (a >= 830 && a <= 842) ||
           (a >= 843 && a <= 855) || (a >= 856 && a <= 868) || (a >= 1065 && a <= 1077);
}

/* Etat "ferme" = valeur CLOSED exacte de chaque famille (tout le reste = ouverte/en ouverture). */
bool isDoorClosedAppearance(const std::uint16_t a) {
    return a == 15 || a == 769 || a == 791 || a == 804 || a == 817 || a == 830 || a == 843 ||
           a == 856 || a == 1065;
}

/* Nom lisible derive du nom de sprite (faute de table de noms d'objets cote client). */
std::string prettyGroundObjectName(const std::uint16_t appearance) {
    const char *sprite = T4CGroundObjectSpriteName(appearance);
    if (!sprite || !sprite[0]) {
        return "Objet";
    }
    std::string s(sprite);
    /* Retire les prefixes techniques connus. */
    for (const char *prefix : {"64kItemGr", "Ground_V2_", "Ground_", "64kItem", "Item"}) {
        const std::size_t n = std::char_traits<char>::length(prefix);
        if (s.compare(0, n, prefix) == 0) {
            s.erase(0, n);
            break;
        }
    }
    /* Retire un suffixe numerique de frame (ex. "Door1", "Hammer01"). */
    while (!s.empty() && (std::isdigit(static_cast<unsigned char>(s.back())) || s.back() == ' ')) {
        s.pop_back();
    }
    if (s.empty()) {
        return "Objet";
    }
    return s;
}

float WorldToScreenX(unsigned int worldX, unsigned int playerX) {
    return kCenterX + static_cast<float>(static_cast<int>(worldX) - static_cast<int>(playerX)) *
                          static_cast<float>(GameWorldScreen::kTileSize);
}

float WorldToScreenY(unsigned int worldY, unsigned int playerY) {
    return kCenterY + static_cast<float>(static_cast<int>(worldY) - static_cast<int>(playerY)) *
                          static_cast<float>(GameWorldScreen::kTileSize);
}

void appendUnitSpritePreload(const char *base, std::vector<std::string> *queue) {
    T4CAppendUnitSpritePreload(base, queue);
}

bool tryDrawNpcBase(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const char *base, const float screenX,
                    const float screenY, const int direction, const int animFrame, const bool attacking) {
    if (!base || base[0] == '\0' || !renderer) {
        return false;
    }
    const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(direction);
    if (attacking) {
        const std::string attackFrame = T4CUnitAttackSpriteFrameName(base, view, animFrame);
        if (!attackFrame.empty() && atlas.TryDrawSpriteByName(renderer, attackFrame, screenX, screenY, view.mirror)) {
            return true;
        }
    }
    const std::string frame = T4CUnitSpriteFrameName(base, view, animFrame);
    if (!frame.empty() && atlas.TryDrawSpriteByName(renderer, frame, screenX, screenY, view.mirror)) {
        return true;
    }
    return atlas.TryDrawSpriteByName(renderer, base, screenX, screenY);
}

}  // namespace

/*
 * Fleches = direction a l'ecran. Projection rectangulaire : worldX -> ecran X,
 * worldY -> ecran Y (meme signe). On choisit l'opcode boussole dont le delta
 * monde donne le mouvement ecran voulu (opcodes : 1=N 2=NE 3=E 4=SE 5=S 6=SO
 * 7=O 8=NO, cf. moveDeltaFromOpcode).
 */
std::uint16_t GameWorldScreen::moveOpcodeFromArrows(const bool left, const bool right, const bool up,
                                                    const bool down) {
    return T4CMoveOpcodeFromArrows(left, right, up, down);
}

std::uint16_t GameWorldScreen::moveOpcodeToward(const unsigned int fromX, const unsigned int fromY,
                                                const unsigned int toX, const unsigned int toY) {
    const int dx = static_cast<int>(toX) - static_cast<int>(fromX);
    const int dy = static_cast<int>(toY) - static_cast<int>(fromY);
    if (dx == 0 && dy == 0) {
        return 0;
    }
    return moveOpcodeFromArrows(dx < 0, dx > 0, dy < 0, dy > 0);
}

static const char *kHoldMoveCursorSprite(int direction) {
    static const char *const kSprites[8] = {"V3_North Cursor",       "V3_North-East Cursor",
                                            "V3_East Cursor",        "V3_South-East Cursor",
                                            "V3_South Cursor",       "V3_South-West Cursor",
                                            "V3_West Cursor",        "V3_North-West Cursor"};
    if (direction < 1 || direction > 8) {
        return nullptr;
    }
    return kSprites[direction - 1];
}

int GameWorldScreen::moveCursorDirectionFromScreen(const float screenX, const float screenY) {
    /* Aligne Mouse.cpp / Pf.cpp (base 1024x768, centre 512x384). */
    const int xPos = static_cast<int>(screenX * 1024.f / static_cast<float>(kLogicalWidth));
    const int yPos = static_cast<int>(screenY * 768.f / static_cast<float>(kLogicalHeight));
    const int dwMouseDistance = 512 * 3;
    int a = xPos * 3;
    int b = yPos * 4;
    if (b > dwMouseDistance) {
        if (a > dwMouseDistance) {
            a -= dwMouseDistance;
            b -= dwMouseDistance;
            if (a > b * 2) {
                return 3;
            }
            if (b > a * 2) {
                return 5;
            }
            return 4;
        }
        b -= dwMouseDistance;
        a = dwMouseDistance - a;
        if (a > b * 2) {
            return 7;
        }
        if (b > a * 2) {
            return 5;
        }
        return 6;
    }
    if (a > dwMouseDistance) {
        a -= dwMouseDistance;
        b = dwMouseDistance - b;
        if (a > b * 2) {
            return 3;
        }
        if (b > a * 2) {
            return 1;
        }
        return 2;
    }
    a = dwMouseDistance - a;
    b = dwMouseDistance - b;
    if (a > b * 2) {
        return 7;
    }
    if (b > a * 2) {
        return 1;
    }
    return 8;
}

void GameWorldScreen::updateHoldMoveCursor(const float screenX, const float screenY) {
    holdMoveDirection_ = moveCursorDirectionFromScreen(screenX, screenY);
    unsigned int wx = 0;
    unsigned int wy = 0;
    if (screenToWorldTile(screenX, screenY, wx, wy)) {
        holdMoveTargetX_ = wx;
        holdMoveTargetY_ = wy;
    }
}

void GameWorldScreen::beginHoldMove(const float screenX, const float screenY) {
    leftMouseHeld_ = true;
    cancelPendingAttack();
    updateHoldMoveCursor(screenX, screenY);
}

void GameWorldScreen::endHoldMove() {
    leftMouseHeld_ = false;
    holdMoveDirection_ = 0;
}

bool GameWorldScreen::isMeleeRange(const unsigned int ax, const unsigned int ay, const unsigned int bx,
                                   const unsigned int by) {
    const int dx = std::abs(static_cast<int>(bx) - static_cast<int>(ax));
    const int dy = std::abs(static_cast<int>(by) - static_cast<int>(ay));
    return dx <= 1 && dy <= 1 && (dx + dy) > 0;
}

bool GameWorldScreen::moveDeltaFromOpcode(const std::uint16_t opcode, int &dx, int &dy) {
    return T4CMoveDeltaFromOpcode(opcode, dx, dy);
}

std::uint16_t GameWorldScreen::moveOpcodeFromKey(const SDL_Event &event) {
    if (event.type != SDL_EVENT_KEY_DOWN || !event.key.down) {
        return 0;
    }

    const SDL_Scancode sc = event.key.scancode;
    const bool isMoveKey =
        sc == SDL_SCANCODE_LEFT || sc == SDL_SCANCODE_RIGHT || sc == SDL_SCANCODE_UP ||
        sc == SDL_SCANCODE_DOWN || sc == SDL_SCANCODE_KP_4 || sc == SDL_SCANCODE_KP_6 ||
        sc == SDL_SCANCODE_KP_8 || sc == SDL_SCANCODE_KP_2;
    if (!isMoveKey) {
        return 0;
    }

    const bool *const keys = SDL_GetKeyboardState(nullptr);
    if (!keys) {
        return 0;
    }

    const bool left = keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_KP_4];
    const bool right = keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_KP_6];
    const bool up = keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_KP_8];
    const bool down = keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_KP_2];

    return moveOpcodeFromArrows(left, right, up, down);
}

int GameWorldScreen::isoViewRadius() const {
    return std::min(kLogicalWidth / kIsoTileW, kLogicalHeight / kIsoTileH) + 8;
}

void GameWorldScreen::finishEnterWorld() {
    loading_ = false;
    preloadQueue_.clear();
    preloadIndex_ = 0;
    loadProgress_ = 1.f;
    ready_ = true;

    if (!T4CLoginSessionRequestNearItems()) {
        lastError_ = "T4CLoginSessionRequestNearItems a echoue";
    }
    T4CLoginSessionRequestPlayerStatus();
    T4CLoginSessionRequestViewEquipped();
    T4CLoginSessionRequestViewBackpack();
    T4CLoginSessionRequestSkillList();
    T4CLoginSessionRequestSpellList();
    T4CLoginSessionUpdateActivePlayerPosition(playerX_, playerY_);

    /* Zones nommees : charge la carte de zones du monde (bandeau « Vous entrez dans... »). */
    zoneMap_.LoadWorld(world_);
    lastZoneId_ = zoneMap_.IsLoaded() ? zoneMap_.ZoneIdAt(playerX_, playerY_) : -1;
}

void GameWorldScreen::updateZoneBanner() {
    if (!zoneMap_.IsLoaded()) {
        return;
    }
    const int zone = zoneMap_.ZoneIdAt(playerX_, playerY_);
    if (zone == lastZoneId_) {
        return;
    }
    lastZoneId_ = zone;
    const std::string name = zoneMap_.ZoneNameAt(playerX_, playerY_);
    if (name.empty()) {
        return;
    }
    zoneBanner_ = name;
    zoneBannerUntil_ = SDL_GetTicks() + 4000;
    T4CLoginSessionPushSystemMessage("Vous entrez dans : " + name);
}

bool GameWorldScreen::BeginLoad(SDL_Renderer *renderer, SDL_Window *window, const unsigned int spawnX,
                                const unsigned int spawnY, const unsigned short world) {
    Shutdown();

    if (!renderer || !window) {
        lastError_ = "Init: renderer ou fenetre null";
        return false;
    }

    renderer_ = renderer;
    window_ = window;
    playerX_ = spawnX;
    playerY_ = spawnY;
    snapPlayerViewToServer();
    world_ = world;
    canMove_ = true;
    moveCooldownUntil_ = 0;
    returnToLogin_ = false;
    quitApp_ = false;
    showGameMenu_ = false;
    confirmReturnToLogin_ = false;
    nearItemsIdleRefreshAt_ = 0;
    remoteUnits_.clear();
    lastError_.clear();
    loading_ = true;
    preloadQueue_.clear();
    preloadIndex_ = 0;
    loadProgress_ = 0.f;
    ready_ = false;

    tmiMap_.Clear();
    tmiLoaded_ = false;
    v2Map_.Clear();
    v2Loaded_ = false;
    spriteAtlas_.Clear();
    spritesLoaded_ = false;

    viewTilesW_ = kLogicalWidth / kTileSize;
    viewTilesH_ = kLogicalHeight / kTileSize;
    mapOriginX_ = 0.f;
    mapOriginY_ = static_cast<float>((kLogicalHeight - viewTilesH_ * kTileSize) / 2);

    if (v2Map_.OpenWorld(world)) {
        v2Loaded_ = true;
        tmiMap_.LoadWorld(world);
        tmiLoaded_ = tmiMap_.IsLoaded();
        if (renderer && spriteAtlas_.Init(renderer)) {
            spritesLoaded_ = true;
            T4CV2SpriteAtlas::CollectViewportSpriteNames(v2Map_, spawnX, spawnY, isoViewRadius(),
                                                         preloadQueue_);
            for (int cls = 0; cls < 4; ++cls) {
                T4CActivePlayer probe{};
                probe.classIndex = static_cast<std::uint8_t>(cls);
                probe.valid = true;
                const char *base = T4CPlayerSpriteNpcName(probe);
                appendUnitSpritePreload(base, &preloadQueue_);
            }
            T4CPuppetDress defaultDress{};
            defaultDress.body = 285;
            defaultDress.legs = 284;
            defaultDress.known = true;
            T4CPuppetAppendDressPreload(defaultDress, false, &preloadQueue_);
            std::vector<std::string> creatureBases;
            T4CCollectCreaturePreloadBases(&creatureBases);
            for (const std::string &base : creatureBases) {
                appendUnitSpritePreload(base.c_str(), &preloadQueue_);
            }
            gameHud_.preloadSprites(&spriteAtlas_);
            preloadQueue_.push_back("V3_LifeBackF");
            preloadQueue_.push_back("V3_PVBar");
            preloadQueue_.push_back("V3_PMBar");
            preloadQueue_.push_back("StaticAttackCursor");
            preloadQueue_.push_back("glove0000");
            preloadQueue_.push_back("V3_TalkCursor");
            for (int f = 0; f <= 22; f += 2) {
                char swordName[16];
                std::snprintf(swordName, sizeof(swordName), "sword%04d", f);
                preloadQueue_.push_back(swordName);
            }
            preloadQueue_.push_back("V3_North Cursor");
            preloadQueue_.push_back("V3_North-East Cursor");
            preloadQueue_.push_back("V3_East Cursor");
            preloadQueue_.push_back("V3_South-East Cursor");
            preloadQueue_.push_back("V3_South Cursor");
            preloadQueue_.push_back("V3_South-West Cursor");
            preloadQueue_.push_back("V3_West Cursor");
            preloadQueue_.push_back("V3_North-West Cursor");
            spriteAtlas_.PreloadBanksForNames(preloadQueue_);
            SDL_Log("[GameWorld] preload viewport : %zu sprites uniques", preloadQueue_.size());
        } else {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] sprites V2 : %s",
                        spriteAtlas_.GetLastError().c_str());
        }
        const std::uint16_t probe = v2Map_.GetFloorTileId(spawnX, spawnY);
        SDL_Log("[GameWorld] carte V2 monde %u OK — tuile sol @ %u,%u = %u (TMI=%s sprites=%s)",
                static_cast<unsigned>(world), spawnX, spawnY, static_cast<unsigned>(probe),
                tmiLoaded_ ? "oui" : "non", spritesLoaded_ ? "oui" : "non");
    } else {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] V2 indisponible : %s — fallback TMI",
                    v2Map_.GetLastError().c_str());
        if (tmiMap_.LoadWorld(world)) {
            tmiLoaded_ = true;
            SDL_Log("[GameWorld] carte TMI monde %u OK", static_cast<unsigned>(world));
        } else {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] TMI indisponible : %s — grille fallback",
                        tmiMap_.GetLastError().c_str());
        }
    }

    if (!spritesLoaded_ || preloadQueue_.empty()) {
        finishEnterWorld();
    }
    return true;
}

bool GameWorldScreen::PollLoad(const int batchSize) {
    if (!loading_ || ready_) {
        return ready_;
    }

    if (spritesLoaded_ && preloadIndex_ < static_cast<int>(preloadQueue_.size())) {
        const int processed =
            spriteAtlas_.PreloadSprites(preloadQueue_, preloadIndex_, std::max(1, batchSize));
        preloadIndex_ += processed;
        if (preloadQueue_.empty()) {
            loadProgress_ = 1.f;
        } else {
            loadProgress_ = static_cast<float>(preloadIndex_) /
                            static_cast<float>(preloadQueue_.size());
        }
    }

    if (preloadIndex_ >= static_cast<int>(preloadQueue_.size())) {
        finishEnterWorld();
    }
    return ready_;
}

void GameWorldScreen::RenderLoading(SDL_Renderer *renderer, LauncherChrome *chrome) const {
    if (!renderer) {
        return;
    }

    if (chrome) {
        chrome->renderBackground(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 12, 14, 20, 255);
        SDL_RenderClear(renderer);
    }

    const float barW = 480.f;
    const float barH = 18.f;
    const float barX = (static_cast<float>(kLogicalWidth) - barW) * 0.5f;
    const float barY = static_cast<float>(kLogicalHeight) * 0.55f;

    SDL_FRect frame{barX - 2.f, barY - 2.f, barW + 4.f, barH + 4.f};
    SDL_SetRenderDrawColor(renderer, 40, 44, 56, 255);
    SDL_RenderFillRect(renderer, &frame);

    SDL_FRect fill{barX, barY, barW * std::clamp(loadProgress_, 0.f, 1.f), barH};
    SDL_SetRenderDrawColor(renderer, 72, 140, 220, 255);
    SDL_RenderFillRect(renderer, &fill);

    char pct[64];
    const int percent = static_cast<int>(std::clamp(loadProgress_, 0.f, 1.f) * 100.f);
    std::snprintf(pct, sizeof(pct), "Chargement du monde… %d%%", percent);

    const SDL_Color white{230, 230, 240, 255};
    if (chrome && chrome->font().isReady()) {
        chrome->font().drawText(renderer, "Chargement du monde", barX, barY - 36.f, white);
        chrome->font().drawText(renderer, pct, barX, barY + 28.f, white);
    } else {
        SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
        SDL_RenderDebugText(renderer, barX, barY - 24.f, "Chargement du monde");
        SDL_RenderDebugText(renderer, barX, barY + 28.f, pct);
    }
}

bool GameWorldScreen::Init(SDL_Renderer *renderer, SDL_Window *window, const unsigned int spawnX,
                           const unsigned int spawnY, const unsigned short world) {
    if (!BeginLoad(renderer, window, spawnX, spawnY, world)) {
        return false;
    }
    while (loading_ && !ready_) {
        PollLoad(512);
    }
    return ready_;
}

void GameWorldScreen::Shutdown() {
    if (ready_) {
        T4CLoginSessionClearRemoteUnits();
    }
    if (chatInputActive_) {
        closeChatInput();
    }
    chatLog_.clear();
    overheadSpeech_.clear();
    selectedBagIndex_ = -1;
    gameHud_.shutdown();
    if (nightHaloTexture_) {
        SDL_DestroyTexture(nightHaloTexture_);
        nightHaloTexture_ = nullptr;
    }
    nextTimeRequestAt_ = 0;
    zoneMap_.Clear();
    lastZoneId_ = -1;
    zoneBanner_.clear();
    zoneBannerUntil_ = 0;
    remoteUnits_.clear();
    groundObjects_.clear();
    selectedGroundId_ = 0;
    tmiMap_.Clear();
    tmiLoaded_ = false;
    v2Map_.Clear();
    v2Loaded_ = false;
    spriteAtlas_.Clear();
    spritesLoaded_ = false;
    loading_ = false;
    preloadQueue_.clear();
    preloadIndex_ = 0;
    loadProgress_ = 0.f;
    renderer_ = nullptr;
    window_ = nullptr;
    ready_ = false;
    canMove_ = true;
    moveCooldownUntil_ = 0;
    returnToLogin_ = false;
    quitApp_ = false;
    showGameMenu_ = false;
    confirmReturnToLogin_ = false;
    playerStatus_ = {};
    pendingAttack_ = {};
    moveServerTimeoutUntil_ = 0;
    nearItemsIdleRefreshAt_ = 0;
    endHoldMove();
}

// Pipeline deplacement joueur — BASELINE IMMUABLE (CHANGELOG.md, ne pas modifier sans relire).
bool GameWorldScreen::tryMovePlayer(const std::uint16_t moveOpcode) {
    if (!ready_ || !canMove_ || isPlayerScrolling()) {
        return false;
    }

    int dx = 0;
    int dy = 0;
    if (!moveDeltaFromOpcode(moveOpcode, dx, dy)) {
        return false;
    }

    const int nx = static_cast<int>(playerX_) + dx;
    const int ny = static_cast<int>(playerY_) + dy;
    if (nx < 0 || ny < 0 || nx > 3071 || ny > 3071) {
        return false;
    }

    /* Pas de blocage client sur map2 : le serveur tranche (escaliers, portes, liens).
     * Windows envoie le move sans test IsBlocking cote client. */

    if (!T4CLoginSessionSendMove(moveOpcode)) {
        return false;
    }

    // Direction visuelle immediate (opcode envoye). Le serveur repond toujours avec
    // __EVENT_OBJECT_MOVED (= opcode 1), pas la direction reelle — cf. MoveToPosition.
    const Uint32 now = SDL_GetTicks();
    const int newDir = T4CDirectionFromMoveOpcode(moveOpcode);
    if (T4CWalkAnimShouldResetFrame(playerMoving_, playerDirection_, newDir)) {
        playerAnimFrame_ = 0;
        playerAnimTick_ = now;
    }
    playerDirection_ = newDir;
    playerMoving_ = true;
    playerAnimUntil_ = now + 450;
    canMove_ = false;
    moveCooldownUntil_ = now + kMoveCooldownMs;
    moveServerTimeoutUntil_ = now + kMoveServerTimeoutMs;
    scheduleNearItemsRefresh();
    return true;
}

void GameWorldScreen::scheduleNearItemsRefresh() {
    nearItemsIdleRefreshAt_ = SDL_GetTicks() + 300;
}

void GameWorldScreen::refreshZoneMusic(const bool forceRestart) {
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    const unsigned level =
        active.valid && active.level > 0 ? static_cast<unsigned>(active.level) : 1u;
    if (forceRestart) {
        T4CGameMusic::Reset();
    }
    T4CGameMusic::LoadNewSound(world_, playerX_, playerY_, level);
}

void GameWorldScreen::applyServerPlayerMove(const unsigned int x, const unsigned int y,
                                            const std::uint16_t moveOpcode) {
    const int dx = static_cast<int>(x) - static_cast<int>(playerX_);
    const int dy = static_cast<int>(y) - static_cast<int>(playerY_);
    const int adx = std::abs(dx);
    const int ady = std::abs(dy);

    /* Windows Packet.cpp TFCMoveID : |delta|>10 = teleport (escaliers/liens), |delta|>1 = 1 tuile. */
    if (adx > 10 || ady > 10) {
        playerX_ = x;
        playerY_ = y;
        snapPlayerViewToServer();
        playerMoving_ = false;
        showGameMenu_ = false;
        showInventory_ = false;
        hasNpcSpeech_ = false;
        shopList_ = {};
        T4CLoginSessionSendBreakConversation();
        T4CLoginSessionClearRemoteUnits();
        remoteUnits_.clear();
        groundObjects_.clear();
        T4CLoginSessionUpdateActivePlayerPosition(playerX_, playerY_);
        T4CLoginSessionRequestNearItems();
        refreshZoneMusic(true);
        canMove_ = true;
        moveServerTimeoutUntil_ = 0;
        return;
    }

    unsigned int stepX = x;
    unsigned int stepY = y;
    if (adx > 1) {
        stepX = static_cast<unsigned>(static_cast<int>(playerX_) + (dx > 0 ? 1 : -1));
    }
    if (ady > 1) {
        stepY = static_cast<unsigned>(static_cast<int>(playerY_) + (dy > 0 ? 1 : -1));
    }

    const int sdx = static_cast<int>(stepX) - static_cast<int>(playerX_);
    const int sdy = static_cast<int>(stepY) - static_cast<int>(playerY_);

    /* Move refuse (meme case) : Windows pfStopMovement + bCanMovePL. */
    if (sdx == 0 && sdy == 0) {
        snapPlayerViewToServer();
        playerMoving_ = false;
        canMove_ = true;
        moveServerTimeoutUntil_ = 0;
        return;
    }

    playerX_ = stepX;
    playerY_ = stepY;
    T4CLoginSessionUpdateActivePlayerPosition(playerX_, playerY_);

    if (sdx != 0 || sdy != 0) {
        const int newDir = T4CDirectionFromServerMoveConfirm(sdx, sdy, moveOpcode);
        if (T4CWalkAnimShouldResetFrame(playerMoving_, playerDirection_, newDir)) {
            playerAnimFrame_ = 0;
            playerAnimTick_ = SDL_GetTicks();
        }
        playerDirection_ = newDir;
        playerMoving_ = true;
        playerAnimUntil_ = SDL_GetTicks() + 450;
        refreshZoneMusic();
    }
    canMove_ = true;
    moveServerTimeoutUntil_ = 0;
    scheduleNearItemsRefresh();
}

void GameWorldScreen::syncGroundObjectsFromNetwork() {
    std::vector<T4CGroundObjectMarker> markers;
    T4CLoginSessionCopyGroundObjectMarkers(&markers);
    std::unordered_map<std::int32_t, T4CGroundObjectMarker> previous;
    previous.swap(groundObjects_);
    for (const T4CGroundObjectMarker &marker : markers) {
        if (marker.unitId == 0) {
            continue;
        }
        /* Filet anti-fantome : ignore tout objet hors du rayon serveur (_DEFAULT_RANGE=80). */
        const int dx = static_cast<int>(marker.x) - static_cast<int>(playerX_);
        const int dy = static_cast<int>(marker.y) - static_cast<int>(playerY_);
        if (std::abs(dx) > kRemoteUnitCullRange || std::abs(dy) > kRemoteUnitCullRange) {
            continue;
        }
        /* Porte : son d'ouverture/fermeture quand le serveur change l'apparence de l'objet
         * (ferme <-> ouvert), comme le client Windows d'origine. */
        if (const auto prev = previous.find(marker.unitId);
            prev != previous.end() && prev->second.appearance != marker.appearance &&
            isDoorAppearance(prev->second.appearance) && isDoorAppearance(marker.appearance)) {
            const bool wasClosed = isDoorClosedAppearance(prev->second.appearance);
            const bool nowClosed = isDoorClosedAppearance(marker.appearance);
            if (wasClosed && !nowClosed) {
                T4CGameMusic::PlaySfx("Open Wooden Door");
            } else if (!wasClosed && nowClosed) {
                T4CGameMusic::PlaySfx("Close Wooden Door");
            }
        }
        groundObjects_[marker.unitId] = marker;
    }
}

bool GameWorldScreen::isMapTileBlocking(const unsigned int worldX, const unsigned int worldY) const {
    if (!v2Loaded_) {
        return false;
    }
    const std::uint16_t objectId = v2Map_.GetObjectTileId(worldX, worldY);
    return T4CMapObjectSprites::IsBlockingMapObject(objectId);
}

bool GameWorldScreen::screenToWorldTile(const float screenX, const float screenY, unsigned int &outX,
                                        unsigned int &outY) const {
    const float anchorX = kCenterX + T4CV2WorldMap::kIsoAnchorBiasX;
    const float anchorY = kCenterY + T4CV2WorldMap::kIsoAnchorBiasY;
    const int wx = static_cast<int>(std::lround(playerDisplayX_)) +
                   static_cast<int>(std::lround((screenX - anchorX) / static_cast<float>(kIsoTileW)));
    const int wy = static_cast<int>(std::lround(playerDisplayY_)) +
                   static_cast<int>(std::lround((screenY - anchorY) / static_cast<float>(kIsoTileH)));
    if (wx < 0 || wy < 0 || wx > 3071 || wy > 3071) {
        return false;
    }
    outX = static_cast<unsigned>(wx);
    outY = static_cast<unsigned>(wy);
    return true;
}

static bool isDoorLikeAppearance(const std::uint16_t appearance) {
    return isDoorAppearance(appearance);
}

bool GameWorldScreen::tryUseAtScreen(const float screenX, const float screenY) {
    const std::int32_t groundId = pickGroundObjectAtScreen(screenX, screenY);
    if (groundId != 0) {
        const auto it = groundObjects_.find(groundId);
        if (it != groundObjects_.end()) {
            selectedGroundId_ = groundId;
            if (isDoorLikeAppearance(it->second.appearance)) {
                T4CGameMusic::PlaySfx("Open Wooden Door");
            }
            return T4CLoginSessionSendUseObject(it->second.x, it->second.y, groundId);
        }
    }

    unsigned int wx = 0;
    unsigned int wy = 0;
    if (!screenToWorldTile(screenX, screenY, wx, wy) || !v2Loaded_) {
        return false;
    }
    const std::uint16_t objectId = v2Map_.GetObjectTileId(wx, wy);
    if (!T4CMapObjectSprites::IsInteractiveMapObject(objectId)) {
        return false;
    }
    if (T4CMapObjectSprites::IsDoorMapObject(objectId)) {
        SDL_Log("[Door] use map-object id=%u @ %u,%u (SFX Open Wooden Door)", objectId, wx, wy);
        T4CGameMusic::PlaySfx("Open Wooden Door");
    }
    return T4CLoginSessionSendUseObject(wx, wy, 0);
}

void GameWorldScreen::applyLocalMoveDelta(const int dx, const int dy) {
    playerX_ = static_cast<unsigned int>(static_cast<int>(playerX_) + dx);
    playerY_ = static_cast<unsigned int>(static_cast<int>(playerY_) + dy);
    T4CLoginSessionUpdateActivePlayerPosition(playerX_, playerY_);
}

void GameWorldScreen::syncRemoteUnitsFromNetwork() {
    std::vector<T4CRemoteUnitEvent> events;
    T4CLoginSessionDrainRemoteUnitEvents(&events);
    for (const T4CRemoteUnitEvent &ev : events) {
        if (T4CLoginSessionShouldSkipRemoteUnit(ev.appearance, ev.unitId, ev.x, ev.y)) {
            remoteUnits_.erase(ev.unitId);
            continue;
        }

        switch (ev.kind) {
            case T4CRemoteUnitEventKind::Remove:
                remoteUnits_.erase(ev.unitId);
                break;
            case T4CRemoteUnitEventKind::Spawn:
            case T4CRemoteUnitEventKind::Move:
            case T4CRemoteUnitEventKind::Update:
            case T4CRemoteUnitEventKind::Attack: {
                RemoteUnitVisual &unit = remoteUnits_[ev.unitId];
                const bool firstSpawn = !unit.initialized;
                const int dx = static_cast<int>(ev.x) - static_cast<int>(unit.serverX);
                const int dy = static_cast<int>(ev.y) - static_cast<int>(unit.serverY);
                if (unit.initialized && (dx != 0 || dy != 0)) {
                    unit.direction = T4CDirectionFromServerMoveConfirm(dx, dy, ev.moveOpcode);
                } else if (ev.moveOpcode >= 1 && ev.moveOpcode <= 8) {
                    unit.direction = T4CDirectionFromMoveOpcode(ev.moveOpcode);
                }
                if (firstSpawn) {
                    unit.displayX = static_cast<float>(ev.x);
                    unit.displayY = static_cast<float>(ev.y);
                    unit.initialized = true;
                } else if (ev.kind == T4CRemoteUnitEventKind::Spawn || std::abs(dx) > 3 ||
                           std::abs(dy) > 3) {
                    /* Re-spawn (bande peripherique / GetNearItems) ou grand saut : snap sur la
                     * position serveur, pas de sprite fantome fige a l'ancienne tuile. */
                    unit.displayX = static_cast<float>(ev.x);
                    unit.displayY = static_cast<float>(ev.y);
                    unit.moving = false;
                    unit.animFrame = 0;
                } else if (dx != 0 || dy != 0) {
                    unit.moving = true;
                    unit.animTick = SDL_GetTicks();
                }
                unit.serverX = ev.x;
                unit.serverY = ev.y;
                if (ev.appearance != 0) {
                    unit.appearance = ev.appearance;
                }
                if (ev.kind == T4CRemoteUnitEventKind::Spawn || ev.kind == T4CRemoteUnitEventKind::Update ||
                    ev.kind == T4CRemoteUnitEventKind::Attack) {
                    if (ev.hpPercent > 0) {
                        unit.hpPercent = ev.hpPercent;
                    }
                }
                if (ev.kind == T4CRemoteUnitEventKind::Attack) {
                    const Uint32 now = SDL_GetTicks();
                    unit.attacking = true;
                    unit.attackAnimFrame = 0;
                    unit.attackAnimTick = now;
                    unit.attackUntil = now + 70 * kT4CAttackAnimFrames;
                    unit.moving = false;
                    if (ev.hasAttackTarget) {
                        const int tdx = static_cast<int>(ev.targetX) - static_cast<int>(ev.x);
                        const int tdy = static_cast<int>(ev.targetY) - static_cast<int>(ev.y);
                        if (tdx != 0 || tdy != 0) {
                            unit.direction = T4CDirectionFromWorldDelta(tdx, tdy);
                        }
                    }
                }
                break;
            }
        }
    }

    /* Filet anti-fantome : retire les unites hors du rayon serveur (au cas ou un opcode 11 de
     * retrait serait perdu en UDP). */
    for (auto it = remoteUnits_.begin(); it != remoteUnits_.end();) {
        const int dx = static_cast<int>(it->second.serverX) - static_cast<int>(playerX_);
        const int dy = static_cast<int>(it->second.serverY) - static_cast<int>(playerY_);
        if (std::abs(dx) > kRemoteUnitCullRange || std::abs(dy) > kRemoteUnitCullRange) {
            it = remoteUnits_.erase(it);
        } else {
            ++it;
        }
    }
}

void GameWorldScreen::noteBagItemUsed(const T4CBagItem &item) {
    /* Torche (ID statique 40015 « Torch ») : bascule l'eclairage local. */
    std::string lower = item.name;
    for (char &c : lower) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    const bool isTorch = item.baseId == 40015 || lower.find("torch") != std::string::npos;
    if (!isTorch) {
        return;
    }
    torchLit_ = !torchLit_;
    T4CLoginSessionPushSystemMessage(torchLit_ ? "Votre torche eclaire les alentours."
                                               : "Votre torche est eteinte.");
}

bool GameWorldScreen::isNpcUnit(const std::int32_t unitId, const std::uint16_t appearance) {
    if (unitId == 0) {
        return false;
    }
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    if (active.valid && unitId == active.unitId) {
        return false;
    }
    if ((appearance >= 10001 && appearance <= 10004) || (appearance >= 15001 && appearance <= 15004)) {
        return false;
    }
    return true;
}

bool GameWorldScreen::HandleEvent(const SDL_Event &event) {
    if (!ready_) {
        return true;
    }

    if (event.type == SDL_EVENT_QUIT) {
        quitApp_ = true;
        return false;
    }

    /* Saisie chat : priorite absolue sur les autres raccourcis clavier. */
    if (handleChatEvent(event)) {
        return true;
    }

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.down &&
        (event.key.scancode == SDL_SCANCODE_ESCAPE || event.key.key == SDLK_ESCAPE)) {
        if (shopList_.valid || hasNpcSpeech_) {
            T4CLoginSessionSendBreakConversation();
            shopList_ = {};
            hasNpcSpeech_ = false;
            return true;
        }
        if (showInventory_) {
            showInventory_ = false;
            return true;
        }
        if (confirmReturnToLogin_) {
            confirmReturnToLogin_ = false;
            return true;
        }
        /* Windows V3_MainBarDlg::ManageESCGUIAll — Esc cycle HUD, pas deconnexion directe. */
        showGameMenu_ = !showGameMenu_;
        return true;
    }

    /* Ctrl+C : bascule mode attaque (macro Windows VKey('C', ctrl) → AttackMode). Permet
     * d'attaquer les PNJ a statut neutre (garde Olin Haad…) qui ripostent si frappes. */
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.down &&
        (event.key.key == SDLK_C || event.key.scancode == SDL_SCANCODE_C) &&
        (event.key.mod & SDL_KMOD_CTRL) != 0) {
        T4CLoginSessionSendAttackMode(!attackMode_);
        return true;
    }

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.down &&
        (event.key.key == SDLK_I || event.key.scancode == SDL_SCANCODE_I)) {
        showInventory_ = !showInventory_;
        T4CGameMusic::PlaySfx("Button release sound");
        if (showInventory_) {
            charTab_ = T4CCharacterWindow::Tab::Inventory;
            T4CLoginSessionRequestPlayerStatus();
            T4CLoginSessionRequestViewBackpack();
            T4CLoginSessionRequestViewEquipped();
            T4CLoginSessionRequestSkillList();
            T4CLoginSessionRequestSpellList();
        }
        return true;
    }

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.down && hasNpcSpeech_) {
        const int linkIdx = event.key.key - SDLK_1;
        if (linkIdx >= 0 && linkIdx < 9 &&
            static_cast<size_t>(linkIdx) < npcSpeech_.links.size()) {
            T4CLoginSessionSendDirectedTalkLink(npcSpeech_.links[static_cast<size_t>(linkIdx)]);
            return true;
        }
    }

    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        SDL_Event converted = event;
        if (renderer_) {
            SDL_ConvertEventToRenderCoordinates(renderer_, &converted);
        }
        mouseX_ = converted.motion.x;
        mouseY_ = converted.motion.y;
        mouseValid_ = true;
        if (leftMouseHeld_ && !playerDead_ && !showGameMenu_ && !confirmReturnToLogin_) {
            updateHoldMoveCursor(converted.motion.x, converted.motion.y);
        }
        hoveredUnitId_ = pickUnitAtScreen(converted.motion.x, converted.motion.y);
        hoveredGroundId_ = pickGroundObjectAtScreen(converted.motion.x, converted.motion.y);
        return true;
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        SDL_Event converted = event;
        if (renderer_) {
            SDL_ConvertEventToRenderCoordinates(renderer_, &converted);
        }
        if (converted.button.button == SDL_BUTTON_LEFT) {
            endHoldMove();
            return true;
        }
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        SDL_Event converted = event;
        if (renderer_) {
            SDL_ConvertEventToRenderCoordinates(renderer_, &converted);
        }
        const float mx = converted.button.x;
        const float my = converted.button.y;
        const std::int32_t picked = pickUnitAtScreen(mx, my);

        if (converted.button.button == SDL_BUTTON_LEFT) {
            if (confirmReturnToLogin_ && handleReturnConfirmClick(mx, my)) {
                return true;
            }
            if (showGameMenu_ && handleGameMenuClick(mx, my)) {
                return true;
            }
            if (showInventory_) {
                const T4CCharacterWindow::ClickResult cr =
                    charWindow_.handleClick(kLogicalWidth, kLogicalHeight, mx, my, charTab_, equipment_,
                                            backpack_, skillBook_, spellBook_, selectedBagIndex_);
                if (cr.close) {
                    showInventory_ = false;
                    selectedBagIndex_ = -1;
                }
                if (cr.tabChanged) {
                    charTab_ = cr.tab;
                    selectedBagIndex_ = -1;
                }
                switch (cr.action) {
                    case T4CCharacterWindow::Action::SelectBagItem:
                        /* Double-clic sur un objet du sac = l'utiliser (torche → eclairage…). */
                        if (converted.button.clicks >= 2 && backpack_.valid && cr.index >= 0 &&
                            static_cast<size_t>(cr.index) < backpack_.items.size()) {
                            const T4CBagItem &item = backpack_.items[static_cast<size_t>(cr.index)];
                            T4CLoginSessionSendUseBagItem(item.objectId);
                            noteBagItemUsed(item);
                            T4CLoginSessionRequestViewBackpack();
                            T4CLoginSessionRequestViewEquipped();
                            T4CLoginSessionRequestPlayerStatus();
                        }
                        selectedBagIndex_ = cr.index;
                        break;
                    case T4CCharacterWindow::Action::UnequipSlot:
                        T4CLoginSessionSendUnequipObject(cr.slot);
                        T4CGameMusic::PlaySfx("Equip");
                        T4CLoginSessionRequestViewBackpack();
                        T4CLoginSessionRequestViewEquipped();
                        T4CLoginSessionRequestPlayerStatus();
                        break;
                    case T4CCharacterWindow::Action::EquipSelected:
                    case T4CCharacterWindow::Action::UseSelected:
                    case T4CCharacterWindow::Action::DropSelected:
                    case T4CCharacterWindow::Action::JunkSelected: {
                        if (backpack_.valid && cr.index >= 0 &&
                            static_cast<size_t>(cr.index) < backpack_.items.size()) {
                            const T4CBagItem &item = backpack_.items[static_cast<size_t>(cr.index)];
                            const std::uint32_t qty = item.qty > 0 ? item.qty : 1u;
                            if (cr.action == T4CCharacterWindow::Action::EquipSelected) {
                                T4CLoginSessionSendEquipObject(item.objectId);
                                T4CGameMusic::PlaySfx("Equip");
                                selectedBagIndex_ = -1;
                            } else if (cr.action == T4CCharacterWindow::Action::UseSelected) {
                                T4CLoginSessionSendUseBagItem(item.objectId);
                                noteBagItemUsed(item);
                            } else if (cr.action == T4CCharacterWindow::Action::DropSelected) {
                                T4CLoginSessionSendDropObject(item.objectId, qty);
                                T4CGameMusic::PlaySfx("Generic drop item");
                                selectedBagIndex_ = -1;
                            } else {
                                T4CLoginSessionSendJunkItems(item.objectId, qty);
                                T4CGameMusic::PlaySfx("Generic drop item");
                                selectedBagIndex_ = -1;
                            }
                            T4CLoginSessionRequestViewBackpack();
                            T4CLoginSessionRequestViewEquipped();
                            T4CLoginSessionRequestPlayerStatus();
                            scheduleNearItemsRefresh();
                        }
                        break;
                    }
                    case T4CCharacterWindow::Action::UseSkill:
                        if (skillBook_.valid && cr.index >= 0 &&
                            static_cast<size_t>(cr.index) < skillBook_.skills.size()) {
                            const T4CPlayerSkill &sk = skillBook_.skills[static_cast<size_t>(cr.index)];
                            if (sk.useMode == 1) {
                                T4CLoginSessionSendUseSkill(sk.skillId);
                                T4CLoginSessionPushSystemMessage("Vous utilisez " + sk.name + ".");
                            } else {
                                T4CLoginSessionPushSystemMessage(
                                    sk.useMode == 0 ? "Cette competence ne s'utilise pas directement."
                                                    : "Competence a cible : non supportee ici.");
                            }
                        }
                        break;
                    case T4CCharacterWindow::Action::CastSpell:
                        if (castSpellAtIndex(cr.index)) {
                            showInventory_ = false;
                        }
                        break;
                    case T4CCharacterWindow::Action::None:
                        break;
                }
                if (cr.consumed) {
                    return true;
                }
            }
            if (shopList_.valid && handleShopClick(mx, my)) {
                return true;
            }
            if (handleHudClick(mx, my)) {
                return true;
            }
            selectedUnitId_ = picked;
            selectedGroundId_ = pickGroundObjectAtScreen(mx, my);
            if (picked != 0) {
                const auto it = remoteUnits_.find(picked);
                const std::uint16_t appearance = it != remoteUnits_.end() ? it->second.appearance : 0;
                if (it != remoteUnits_.end()) {
                    /* Nom reel (« Dark Fang », « Mithrand »…) pour l'etiquette de cible. */
                    T4CLoginSessionRequestUnitName(picked, it->second.serverX, it->second.serverY);
                }
                if (attackMode_) {
                    /* Mode attaque (Ctrl+C) : tout clic sur une unite = attaque, PNJ inclus. */
                    tryAttackUnit(picked);
                } else if (converted.button.clicks >= 2) {
                    if (isNpcUnit(picked, appearance)) {
                        tryTalkUnit(picked);
                    } else {
                        tryAttackUnit(picked);
                    }
                } else if (isNpcUnit(picked, appearance)) {
                    tryTalkUnit(picked);
                }
            } else if (selectedGroundId_ != 0) {
                const auto it = groundObjects_.find(selectedGroundId_);
                if (it != groundObjects_.end()) {
                    if (isDoorLikeAppearance(it->second.appearance)) {
                        /* Porte : on l'utilise — le son suit le changement d'apparence serveur. */
                        T4CLoginSessionSendUseObject(it->second.x, it->second.y, selectedGroundId_);
                    } else {
                        /* Objet au sol : ramassage direct (Windows Grid==5 → RQ_GetObject). */
                        T4CLoginSessionSendGetObject(it->second.x, it->second.y, selectedGroundId_);
                        T4CGameMusic::PlaySfx("Generic pickup item");
                        scheduleNearItemsRefresh();
                        T4CLoginSessionRequestViewBackpack();
                    }
                }
            } else if (!playerDead_ && !showGameMenu_ && !confirmReturnToLogin_) {
                beginHoldMove(mx, my);
            } else {
                cancelPendingAttack();
            }
            return true;
        }
        if (converted.button.button == SDL_BUTTON_RIGHT) {
            /* Clic droit = IDENTIFIER seulement (Windows MouseAction UseOb → Objects.Identify).
             * Aucune action (pas d'attaque, pas d'usage de porte) : ca affiche le nom. */
            if (picked != 0) {
                selectedUnitId_ = picked;
                const auto it = remoteUnits_.find(picked);
                if (it != remoteUnits_.end()) {
                    T4CLoginSessionRequestUnitName(picked, it->second.serverX, it->second.serverY);
                }
                return true;
            }
            const std::int32_t groundId = pickGroundObjectAtScreen(mx, my);
            if (groundId != 0) {
                const auto it = groundObjects_.find(groundId);
                if (it != groundObjects_.end()) {
                    /* Examine : affiche le nom de l'objet pres de lui (pas de ramassage). */
                    examineLabel_ = prettyGroundObjectName(it->second.appearance);
                    examineLabelWorldX_ = static_cast<float>(it->second.x);
                    examineLabelWorldY_ = static_cast<float>(it->second.y);
                    examineLabelUntil_ = SDL_GetTicks() + 3000;
                }
            }
            return true;
        }
    }

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.down &&
        event.key.scancode == SDL_SCANCODE_U) {
        if (selectedGroundId_ != 0) {
            const auto it = groundObjects_.find(selectedGroundId_);
            if (it != groundObjects_.end()) {
                T4CLoginSessionSendUseObject(it->second.x, it->second.y, selectedGroundId_);
            }
        }
        return true;
    }

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.down &&
        event.key.scancode == SDL_SCANCODE_G) {
        if (selectedGroundId_ != 0) {
            const auto it = groundObjects_.find(selectedGroundId_);
            if (it != groundObjects_.end()) {
                T4CLoginSessionSendGetObject(it->second.x, it->second.y, selectedGroundId_);
            }
        }
        return true;
    }

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.down &&
        (event.key.key == SDLK_A || event.key.scancode == SDL_SCANCODE_A)) {
        if (selectedUnitId_ != 0) {
            tryAttackUnit(selectedUnitId_);
        }
        return true;
    }

    const std::uint16_t opcode = moveOpcodeFromKey(event);
    if (opcode != 0) {
        tryMovePlayer(opcode);
    }

    return true;
}

void GameWorldScreen::cancelPendingAttack() {
    pendingAttack_.active = false;
    pendingAttack_.unitId = 0;
    pendingAttack_.talk = false;
}

bool GameWorldScreen::tryTalkUnit(const std::int32_t unitId) {
    if (unitId == 0 || playerDead_) {
        return false;
    }
    const auto it = remoteUnits_.find(unitId);
    if (it == remoteUnits_.end()) {
        return false;
    }
    const int dx = static_cast<int>(it->second.serverX) - static_cast<int>(playerX_);
    const int dy = static_cast<int>(it->second.serverY) - static_cast<int>(playerY_);
    /* Le serveur refuse (« You are too far ! ») si dist^2 >= 120 (~11 tuiles). Comme le client
     * Windows, on marche d'abord vers le PNJ s'il est loin, puis on parle (marge anti-desync). */
    if (dx * dx + dy * dy > 36) {
        pendingAttack_.active = true;
        pendingAttack_.unitId = unitId;
        pendingAttack_.talk = true;
        return true;
    }
    cancelPendingAttack();
    const int direction = T4CDirectionFromWorldDelta(dx, dy);
    return T4CLoginSessionSendDirectedTalk(it->second.serverX, it->second.serverY, unitId, direction);
}

bool GameWorldScreen::tryAttackUnit(const std::int32_t unitId) {
    if (unitId == 0) {
        return false;
    }

    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    if (active.valid && active.unitId != 0 && unitId == active.unitId) {
        return false;
    }

    const auto it = remoteUnits_.find(unitId);
    if (it == remoteUnits_.end()) {
        return false;
    }

    const unsigned tx = it->second.serverX;
    const unsigned ty = it->second.serverY;

    if (isMeleeRange(playerX_, playerY_, tx, ty)) {
        cancelPendingAttack();
        return T4CLoginSessionSendAttack(tx, ty, unitId);
    }

    pendingAttack_.active = true;
    pendingAttack_.unitId = unitId;
    pendingAttack_.talk = false;
    return true;
}

void GameWorldScreen::updatePendingAttack() {
    if (!pendingAttack_.active || !canMove_) {
        return;
    }

    const auto it = remoteUnits_.find(pendingAttack_.unitId);
    if (it == remoteUnits_.end()) {
        cancelPendingAttack();
        return;
    }

    const unsigned tx = it->second.serverX;
    const unsigned ty = it->second.serverY;

    if (pendingAttack_.talk) {
        const int dx = static_cast<int>(tx) - static_cast<int>(playerX_);
        const int dy = static_cast<int>(ty) - static_cast<int>(playerY_);
        if (dx * dx + dy * dy <= 36) {
            const std::int32_t targetId = pendingAttack_.unitId;
            cancelPendingAttack();
            T4CLoginSessionSendDirectedTalk(tx, ty, targetId, T4CDirectionFromWorldDelta(dx, dy));
            return;
        }
    } else if (isMeleeRange(playerX_, playerY_, tx, ty)) {
        const std::int32_t targetId = pendingAttack_.unitId;
        cancelPendingAttack();
        T4CLoginSessionSendAttack(tx, ty, targetId);
        return;
    }

    const std::uint16_t opcode = moveOpcodeToward(playerX_, playerY_, tx, ty);
    if (opcode != 0) {
        tryMovePlayer(opcode);
    }
}

std::int32_t GameWorldScreen::pickUnitAtScreen(const float screenX, const float screenY) const {
    // Les unites sont ancrees aux pieds ; le sprite s'etend vers le haut. On teste
    // une boite (~40 large, du sol jusqu'a ~64px au-dessus) et on prend la plus proche.
    constexpr float kHalfW = 22.f;
    constexpr float kUp = 64.f;
    constexpr float kDown = 10.f;
    std::int32_t best = 0;
    float bestDist = 1e9f;
    for (const auto &entry : remoteUnits_) {
        const float sx = worldToScreenX(entry.second.displayX, entry.second.displayY);
        const float sy = worldToScreenY(entry.second.displayX, entry.second.displayY);
        if (screenX < sx - kHalfW || screenX > sx + kHalfW || screenY < sy - kUp || screenY > sy + kDown) {
            continue;
        }
        const float cx = screenX - sx;
        const float cy = screenY - (sy - 24.f);
        const float dist = cx * cx + cy * cy;
        if (dist < bestDist) {
            bestDist = dist;
            best = entry.first;
        }
    }
    return best;
}

std::int32_t GameWorldScreen::pickGroundObjectAtScreen(const float screenX, const float screenY) const {
    /* Sprites sol 64kItemGr* : ancre tuile + offset vertical negatif. */
    constexpr float kHalfW = 24.f;
    constexpr float kUp = 40.f;
    constexpr float kDown = 6.f;
    std::int32_t best = 0;
    float bestDist = 1e9f;
    for (const auto &entry : groundObjects_) {
        const float sx = worldToScreenX(entry.second.x, entry.second.y);
        const float sy = worldToScreenY(entry.second.x, entry.second.y);
        if (screenX < sx - kHalfW || screenX > sx + kHalfW || screenY < sy - kUp || screenY > sy + kDown) {
            continue;
        }
        const float cx = screenX - sx;
        const float cy = screenY - (sy - 20.f);
        const float dist = cx * cx + cy * cy;
        if (dist < bestDist) {
            bestDist = dist;
            best = entry.first;
        }
    }
    return best;
}

void GameWorldScreen::drawGroundObjectMarker(SDL_Renderer *renderer, const float sx, const float sy,
                                               const std::uint16_t appearance) const {
    if (!renderer) {
        return;
    }
    if (spritesLoaded_) {
        if (const char *spriteName = T4CGroundObjectSpriteName(appearance); spriteName != nullptr) {
            if (spriteAtlas_.TryDrawSpriteByName(renderer, spriteName, sx, sy)) {
                return;
            }
        }
    }
    SDL_Color color{180, 140, 60, 255};
    if (appearance == kObjGroupPortal) {
        color = {120, 180, 255, 255};
    } else if (appearance >= 10 && appearance <= 20) {
        color = {200, 160, 80, 255};
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_FRect body{sx - 8.f, sy - 10.f, 16.f, 10.f};
    SDL_RenderFillRect(renderer, &body);
    SDL_SetRenderDrawColor(renderer, 255, 255, 220, 255);
    SDL_RenderRect(renderer, &body);
}

void GameWorldScreen::drawUnitOutline(SDL_Renderer *renderer, const float sx, const float sy,
                                        const std::int32_t unitId, const std::uint16_t appearance) const {
    if (!renderer || !spritesLoaded_) {
        return;
    }
    int direction = 2;
    int frame = 0;
    bool attacking = false;
    if (const auto it = remoteUnits_.find(unitId); it != remoteUnits_.end()) {
        direction = it->second.direction;
        attacking = it->second.attacking;
        frame = attacking ? it->second.attackAnimFrame
                          : (it->second.moving ? it->second.animFrame : 0);
    }
    const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(direction);
    if (T4CIsPuppetAppearance(appearance)) {
        T4CPuppetDress dress{};
        if (!T4CLoginSessionGetRemotePuppetDress(unitId, &dress)) {
            dress.body = 425;
            dress.legs = 284;
            dress.feet = 285;
            dress.known = true;
        }
        const char *fallback = T4CPuppetFallbackSpriteName(dress, appearance == 10012 || appearance == 15012);
        if (fallback) {
            const std::string frameName =
                attacking ? T4CUnitAttackSpriteFrameName(fallback, view, frame)
                          : T4CUnitSpriteFrameName(fallback, view, frame);
            if (!frameName.empty() &&
                spriteAtlas_.TryDrawSpriteOutline(renderer, frameName, sx, sy, 100, 220, 255, view.mirror)) {
                return;
            }
        }
    }
    const char *base = T4CRemoteUnitSpriteName(unitId, appearance);
    if (!base) {
        return;
    }
    const std::string frameName = attacking ? T4CUnitAttackSpriteFrameName(base, view, frame)
                                            : T4CUnitSpriteFrameName(base, view, frame);
    if (!frameName.empty() &&
        spriteAtlas_.TryDrawSpriteOutline(renderer, frameName, sx, sy, 100, 220, 255, view.mirror)) {
        return;
    }
    if (spriteAtlas_.TryDrawSpriteOutline(renderer, base, sx, sy, 100, 220, 255, view.mirror)) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, 100, 220, 255, 255);
    SDL_FRect box{sx - 20.f, sy - 60.f, 40.f, 68.f};
    SDL_RenderRect(renderer, &box);
}

void GameWorldScreen::renderDialoguePanel(SDL_Renderer *renderer) const {
    if (!renderer || !hasNpcSpeech_) {
        return;
    }
    SDL_FRect panel{20.f, static_cast<float>(kLogicalHeight) - 170.f,
                    static_cast<float>(kLogicalWidth) - 40.f, 150.f};
    SDL_SetRenderDrawColor(renderer, 10, 20, 40, 220);
    SDL_RenderFillRect(renderer, &panel);
    char title[128];
    std::snprintf(title, sizeof(title), "%s:",
                    npcSpeech_.speakerName.empty() ? "PNJ" : npcSpeech_.speakerName.c_str());
    drawHudText(renderer, title, 28.f, static_cast<float>(kLogicalHeight) - 162.f);
    drawHudText(renderer, npcSpeech_.text.c_str(), 28.f, static_cast<float>(kLogicalHeight) - 142.f);
    float linkY = static_cast<float>(kLogicalHeight) - 110.f;
    for (size_t i = 0; i < npcSpeech_.links.size() && i < 9; ++i) {
        char line[160];
        std::snprintf(line, sizeof(line), "%zu: [%s]", i + 1, npcSpeech_.links[i].c_str());
        drawHudText(renderer, line, 28.f, linkY);
        linkY += 16.f;
    }
}

namespace {

constexpr SDL_FRect kShopPanel{static_cast<float>(GameWorldScreen::kLogicalWidth) - 340.f, 80.f, 320.f,
                               372.f};
constexpr float kShopRowY = kShopPanel.y + 46.f;
constexpr float kShopRowH = 18.f;
constexpr int kShopMaxRows = 16;
constexpr SDL_FRect kShopClose{kShopPanel.x + kShopPanel.w - 26.f, kShopPanel.y + 4.f, 22.f, 18.f};

bool PointInRect(const float x, const float y, const SDL_FRect &r) {
    return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

}  // namespace

void GameWorldScreen::renderShopPanel(SDL_Renderer *renderer) const {
    if (!renderer || !shopList_.valid) {
        return;
    }
    const char *title = "Boutique";
    const char *hint = "";
    switch (shopList_.mode) {
        case T4CShopMode::Buy:
            title = "Achat";
            hint = "Clic: acheter 1";
            break;
        case T4CShopMode::Sell:
            title = "Vente";
            hint = "Clic: vendre 1";
            break;
        case T4CShopMode::Train:
            title = "Entrainement";
            hint = "Clic: +1 point";
            break;
        case T4CShopMode::Learn:
            title = "Apprentissage";
            hint = "Clic: apprendre";
            break;
        default:
            break;
    }
    SDL_SetRenderDrawColor(renderer, 30, 30, 50, 235);
    SDL_RenderFillRect(renderer, &kShopPanel);
    SDL_SetRenderDrawColor(renderer, 110, 100, 70, 255);
    SDL_RenderRect(renderer, &kShopPanel);
    char header[96];
    if (shopList_.mode == T4CShopMode::Train || shopList_.mode == T4CShopMode::Learn) {
        std::snprintf(header, sizeof(header), "%s (pts:%u)", title,
                      static_cast<unsigned>(shopList_.skillPoints));
    } else {
        std::snprintf(header, sizeof(header), "%s (or:%u)", title, shopList_.gold);
    }
    drawHudText(renderer, header, kShopPanel.x + 8.f, kShopPanel.y + 8.f);
    drawHudText(renderer, hint, kShopPanel.x + 8.f, kShopPanel.y + 26.f);
    SDL_SetRenderDrawColor(renderer, 90, 40, 36, 255);
    SDL_RenderFillRect(renderer, &kShopClose);
    drawHudText(renderer, "X", kShopClose.x + 7.f, kShopClose.y + 3.f);

    const bool hovered = mouseValid_;
    for (size_t i = 0; i < shopList_.items.size() && i < static_cast<size_t>(kShopMaxRows); ++i) {
        const T4CShopEntry &item = shopList_.items[i];
        const float ry = kShopRowY + static_cast<float>(i) * kShopRowH;
        SDL_FRect row{kShopPanel.x + 4.f, ry, kShopPanel.w - 8.f, kShopRowH};
        if (hovered && mouseX_ >= row.x && mouseX_ < row.x + row.w && mouseY_ >= row.y &&
            mouseY_ < row.y + row.h) {
            SDL_SetRenderDrawColor(renderer, 60, 58, 90, 255);
            SDL_RenderFillRect(renderer, &row);
        }
        char line[192];
        if (shopList_.mode == T4CShopMode::Train) {
            std::snprintf(line, sizeof(line), "%s — %u (max %u)",
                          item.name.empty() ? "?" : item.name.c_str(), item.price, item.maxQty);
        } else if (shopList_.mode == T4CShopMode::Sell) {
            std::snprintf(line, sizeof(line), "%s — %u or (x%u)",
                          item.name.empty() ? "?" : item.name.c_str(), item.price, item.maxQty);
        } else {
            std::snprintf(line, sizeof(line), "%s — %u or", item.name.empty() ? "?" : item.name.c_str(),
                          item.price);
        }
        drawHudText(renderer, line, row.x + 4.f, ry + 4.f);
    }
}

bool GameWorldScreen::handleShopClick(const float mx, const float my) {
    if (!shopList_.valid || !PointInRect(mx, my, kShopPanel)) {
        return false;
    }
    if (PointInRect(mx, my, kShopClose)) {
        T4CLoginSessionSendBreakConversation();
        shopList_ = {};
        hasNpcSpeech_ = false;
        return true;
    }
    const int row = static_cast<int>((my - kShopRowY) / kShopRowH);
    if (my < kShopRowY || row < 0 || static_cast<size_t>(row) >= shopList_.items.size() ||
        row >= kShopMaxRows) {
        return true;
    }
    const T4CShopEntry &item = shopList_.items[static_cast<size_t>(row)];
    switch (shopList_.mode) {
        case T4CShopMode::Buy:
            T4CLoginSessionSendBuyItems({{item.objectId, 1}});
            T4CLoginSessionPushSystemMessage("Achat : " + item.name);
            T4CGameMusic::PlaySfx("Gold sound");
            break;
        case T4CShopMode::Sell:
            T4CLoginSessionSendSellItems({{item.objectId, 1u}});
            T4CLoginSessionPushSystemMessage("Vente : " + item.name);
            T4CGameMusic::PlaySfx("Gold sound");
            break;
        case T4CShopMode::Train:
            T4CLoginSessionSendTrainSkills({{static_cast<std::uint16_t>(item.objectId), 1}});
            T4CLoginSessionPushSystemMessage("Entrainement : " + item.name);
            break;
        case T4CShopMode::Learn:
            T4CLoginSessionSendTeachSkills({static_cast<std::uint16_t>(item.objectId)});
            T4CLoginSessionPushSystemMessage("Apprentissage : " + item.name);
            break;
        default:
            break;
    }
    /* Refresh sac/or/skills apres transaction (le serveur traite en sequence). */
    T4CLoginSessionRequestViewBackpack();
    T4CLoginSessionRequestPlayerStatus();
    if (shopList_.mode == T4CShopMode::Train || shopList_.mode == T4CShopMode::Learn) {
        T4CLoginSessionRequestSkillList();
        T4CLoginSessionRequestSpellList();
    }
    return true;
}

bool GameWorldScreen::handleHudClick(const float mx, const float my) {
    const SDL_FRect home = T4CGameHud::homeButtonRect(kLogicalWidth, kLogicalHeight);
    if (PointInRect(mx, my, home)) {
        showInventory_ = true;
        T4CGameMusic::PlaySfx("Button release sound");
        charTab_ = T4CCharacterWindow::Tab::Stats;
        T4CLoginSessionRequestPlayerStatus();
        T4CLoginSessionRequestViewBackpack();
        T4CLoginSessionRequestViewEquipped();
        T4CLoginSessionRequestSkillList();
        T4CLoginSessionRequestSpellList();
        return true;
    }
    const SDL_FRect exit = T4CGameHud::exitButtonRect(kLogicalWidth, kLogicalHeight);
    if (PointInRect(mx, my, exit)) {
        showGameMenu_ = true;
        T4CGameMusic::PlaySfx("Button release sound");
        return true;
    }
    /* Slots macro 1-12 : sort n du grimoire (defaut Windows : macros remplies par drag). */
    for (int slot = 0; slot < 12; ++slot) {
        const SDL_FRect r = T4CGameHud::macroSlotRect(kLogicalWidth, kLogicalHeight, slot);
        if (PointInRect(mx, my, r)) {
            if (spellBook_.valid && static_cast<size_t>(slot) < spellBook_.spells.size()) {
                castSpellAtIndex(slot);
            }
            return true;
        }
    }
    /* Fond des panneaux HUD : consomme le clic (pas de marche sous la MainBar/TMI/Life). */
    const SDL_FRect mainBar{static_cast<float>((kLogicalWidth - T4CGameHud::kMainBarW) / 2),
                            static_cast<float>(kLogicalHeight - T4CGameHud::kMainBarH),
                            static_cast<float>(T4CGameHud::kMainBarW),
                            static_cast<float>(T4CGameHud::kMainBarH)};
    const SDL_FRect tmi{static_cast<float>(kLogicalWidth - T4CGameHud::kTmiW),
                        static_cast<float>(kLogicalHeight - T4CGameHud::kTmiH - T4CGameHud::kMainBarH),
                        static_cast<float>(T4CGameHud::kTmiW), static_cast<float>(T4CGameHud::kTmiH)};
    const SDL_FRect life{static_cast<float>(kLogicalWidth - T4CGameHud::kLifeW), 0.f,
                         static_cast<float>(T4CGameHud::kLifeW), static_cast<float>(T4CGameHud::kLifeH)};
    if (PointInRect(mx, my, mainBar) || PointInRect(mx, my, tmi) || PointInRect(mx, my, life)) {
        return true;
    }
    return false;
}

bool GameWorldScreen::castSpellAtIndex(const int index) {
    if (!spellBook_.valid || index < 0 || static_cast<size_t>(index) >= spellBook_.spells.size()) {
        return false;
    }
    const T4CPlayerSpell &spell = spellBook_.spells[static_cast<size_t>(index)];
    if (selectedUnitId_ != 0) {
        if (const auto it = remoteUnits_.find(selectedUnitId_); it != remoteUnits_.end()) {
            T4CLoginSessionSendCastSpell(spell.spellId, it->second.serverX, it->second.serverY,
                                         selectedUnitId_);
            T4CLoginSessionPushSystemMessage("Sort " + spell.name + " sur la cible.");
            return true;
        }
    }
    /* Pas de cible : lancer sur soi (V3_SpellDlg coords 0,0 id 0). */
    T4CLoginSessionSendCastSpell(spell.spellId, 0, 0, 0);
    T4CLoginSessionPushSystemMessage("Sort " + spell.name + ".");
    return true;
}

namespace {

/* Couleur COLORREF Windows (0x00BBGGRR) → composantes SDL. */
void ColorrefToRgb(const std::uint32_t c, Uint8 &r, Uint8 &g, Uint8 &b) {
    r = static_cast<Uint8>(c & 0xFF);
    g = static_cast<Uint8>((c >> 8) & 0xFF);
    b = static_cast<Uint8>((c >> 16) & 0xFF);
    if (r < 40 && g < 40 && b < 40) {
        r = g = b = 220;  // texte noir illisible sur fond sombre
    }
}

void Utf8PopBack(std::string &s) {
    while (!s.empty() && (static_cast<unsigned char>(s.back()) & 0xC0) == 0x80) {
        s.pop_back();
    }
    if (!s.empty()) {
        s.pop_back();
    }
}

}  // namespace

void GameWorldScreen::openChatInput() {
    chatInputActive_ = true;
    chatInput_.clear();
    if (window_) {
        SDL_StartTextInput(window_);
    }
}

void GameWorldScreen::closeChatInput() {
    chatInputActive_ = false;
    chatInput_.clear();
    if (window_) {
        SDL_StopTextInput(window_);
    }
}

bool GameWorldScreen::handleChatEvent(const SDL_Event &event) {
    if (chatInputActive_) {
        if (event.type == SDL_EVENT_TEXT_INPUT) {
            if (event.text.text != nullptr && chatInput_.size() < 240) {
                chatInput_ += event.text.text;
            }
            return true;
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.down) {
            if (event.key.key == SDLK_BACKSPACE) {
                Utf8PopBack(chatInput_);
            } else if (event.key.key == SDLK_RETURN || event.key.key == SDLK_KP_ENTER) {
                const std::string line = chatInput_;
                closeChatInput();
                sendChatLine(line);
            } else if (event.key.key == SDLK_ESCAPE) {
                closeChatInput();
            }
            /* Consomme toutes les touches pendant la saisie (pas de raccourcis I/A/G/U). */
            return true;
        }
        return false;
    }
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.down &&
        (event.key.key == SDLK_RETURN || event.key.key == SDLK_KP_ENTER) && !showGameMenu_ &&
        !confirmReturnToLogin_) {
        openChatInput();
        return true;
    }
    return false;
}

void GameWorldScreen::sendChatLine(const std::string &rawLine) {
    std::string line = rawLine;
    while (!line.empty() && (line.back() == ' ' || line.back() == '\t')) {
        line.pop_back();
    }
    size_t start = 0;
    while (start < line.size() && line[start] == ' ') {
        ++start;
    }
    line = line.substr(start);
    if (line.empty()) {
        return;
    }
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    if (line[0] == ':' && line.size() > 1) {
        /* Cri de zone (main2.cpp prefixe ':'). */
        const std::string text = line.substr(1);
        if (T4CLoginSessionSendShout(text)) {
            T4CLoginSessionPushSystemMessage("Vous criez : " + text);
        }
        return;
    }
    if (line[0] == '/' && line.size() > 1) {
        /* Message prive « /Nom message ». */
        const size_t space = line.find(' ');
        if (space != std::string::npos && space > 1 && space + 1 < line.size()) {
            const std::string target = line.substr(1, space - 1);
            const std::string text = line.substr(space + 1);
            if (T4CLoginSessionSendPage(target, text)) {
                T4CLoginSessionPushSystemMessage("Vers " + target + " : " + text);
            }
        } else {
            T4CLoginSessionPushSystemMessage("Usage : /Nom message");
        }
        return;
    }
    if (T4CLoginSessionSendChatLocal(line)) {
        /* Echo local immediat (le serveur rebroadcast en 27 pour les autres). */
        const Uint32 now = SDL_GetTicks();
        ChatLine echo{};
        echo.text = (active.valid && !active.name.empty() ? active.name : "Vous") + std::string(": ") + line;
        echo.color = 0xFFFFFFu;
        echo.recvAt = now;
        chatLog_.push_back(std::move(echo));
        if (active.valid && active.unitId != 0) {
            overheadSpeech_[active.unitId] = OverheadSpeech{line, now + 5000};
        }
    }
}

void GameWorldScreen::pumpChatMessages() {
    std::vector<T4CChatMessage> messages;
    T4CLoginSessionDrainChatMessages(&messages);
    if (messages.empty()) {
        return;
    }
    const Uint32 now = SDL_GetTicks();
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    for (T4CChatMessage &msg : messages) {
        /* Echo serveur de notre propre parole : deja affiche localement. */
        if (msg.kind == T4CChatKind::Local && active.valid && msg.unitId == active.unitId) {
            continue;
        }
        ChatLine line{};
        switch (msg.kind) {
            case T4CChatKind::Local:
            case T4CChatKind::Shout:
                line.text = msg.speaker.empty() ? msg.text : msg.speaker + ": " + msg.text;
                break;
            case T4CChatKind::Page:
                line.text = "[" + msg.speaker + "] " + msg.text;
                break;
            case T4CChatKind::System:
                line.text = msg.text;
                break;
        }
        line.color = msg.color;
        line.recvAt = now;
        chatLog_.push_back(std::move(line));
        if (msg.kind == T4CChatKind::Local && msg.unitId != 0) {
            overheadSpeech_[msg.unitId] = OverheadSpeech{msg.text, now + 5000};
        }
    }
    if (chatLog_.size() > 80) {
        chatLog_.erase(chatLog_.begin(), chatLog_.begin() + static_cast<long>(chatLog_.size() - 60));
    }
}

void GameWorldScreen::renderChatLog(SDL_Renderer *renderer) const {
    if (!renderer) {
        return;
    }
    const Uint32 now = SDL_GetTicks();
    constexpr int kMaxLines = 8;
    constexpr Uint32 kLineTtlMs = 14000;
    const float baseY = static_cast<float>(kLogicalHeight) - 110.f;

    int shown = 0;
    for (auto it = chatLog_.rbegin(); it != chatLog_.rend() && shown < kMaxLines; ++it) {
        if (!chatInputActive_ && now - it->recvAt > kLineTtlMs) {
            break;
        }
        const float y = baseY - static_cast<float>(shown + 1) * 16.f;
        Uint8 r = 220;
        Uint8 g = 220;
        Uint8 b = 220;
        ColorrefToRgb(it->color, r, g, b);
        const float w = static_cast<float>(it->text.size()) * 8.f + 8.f;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 130);
        SDL_FRect bg{8.f, y - 2.f, w, 15.f};
        SDL_RenderFillRect(renderer, &bg);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDebugText(renderer, 12.f, y, it->text.c_str());
        ++shown;
    }

    if (chatInputActive_) {
        SDL_FRect box{8.f, baseY, 620.f, 20.f};
        SDL_SetRenderDrawColor(renderer, 10, 10, 24, 220);
        SDL_RenderFillRect(renderer, &box);
        SDL_SetRenderDrawColor(renderer, 140, 130, 90, 255);
        SDL_RenderRect(renderer, &box);
        const std::string prompt = "> " + chatInput_ + ((now / 400) % 2 == 0 ? "_" : "");
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(renderer, box.x + 6.f, box.y + 5.f, prompt.c_str());
        SDL_SetRenderDrawColor(renderer, 160, 150, 130, 255);
        SDL_RenderDebugText(renderer, box.x + 6.f, box.y + 24.f,
                            "Entree: parler   : crier   /Nom: prive   Echap: fermer");
    }
}

namespace {

/* Obscurite 0..1 selon l'heure T4C (jour 7h-17h, nuit 19h-5h, transitions 2 h). */
float NightDarkness(const float hour) {
    if (hour >= 7.f && hour <= 17.f) {
        return 0.f;
    }
    if (hour >= 19.f || hour <= 5.f) {
        return 1.f;
    }
    if (hour > 17.f && hour < 19.f) {
        return (hour - 17.f) * 0.5f;
    }
    return (7.f - hour) * 0.5f;  // 5h-7h aube
}

}  // namespace

void GameWorldScreen::renderNightOverlay(SDL_Renderer *renderer) {
    if (!renderer) {
        return;
    }
    float darkness = 0.f;
    if (world_ != 0) {
        /* Sous-sols/donjons (monde != 0) : toujours sombres, quelle que soit l'heure. */
        darkness = 1.f;
    } else {
        T4CGameTime time{};
        T4CLoginSessionGetGameTime(&time);
        if (!time.valid) {
            return;
        }
        const float hour = static_cast<float>(time.hour) + static_cast<float>(time.minute) / 60.f;
        darkness = NightDarkness(hour);
    }
    if (darkness <= 0.01f) {
        return;
    }
    const Uint8 alpha = static_cast<Uint8>(165.f * darkness);

    /* Halo : texture radiale 256×256 (alpha 0 centre → 255 bord), construite une fois. */
    constexpr int kHaloPx = 256;
    if (!nightHaloTexture_) {
        std::vector<Uint8> pixels(static_cast<size_t>(kHaloPx) * kHaloPx * 4);
        const float half = static_cast<float>(kHaloPx) * 0.5f;
        for (int y = 0; y < kHaloPx; ++y) {
            for (int x = 0; x < kHaloPx; ++x) {
                const float dx = (static_cast<float>(x) + 0.5f - half) / half;
                const float dy = (static_cast<float>(y) + 0.5f - half) / half;
                float d = std::sqrt(dx * dx + dy * dy);
                /* Plateau lumineux jusqu'a 0.45, fondu vers l'obscurite au bord. */
                float a = (d - 0.45f) / 0.55f;
                a = std::clamp(a, 0.f, 1.f);
                Uint8 *out = pixels.data() + (static_cast<size_t>(y) * kHaloPx + x) * 4;
                out[0] = 0;
                out[1] = 0;
                out[2] = 8;
                out[3] = static_cast<Uint8>(a * 255.f);
            }
        }
        nightHaloTexture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                              SDL_TEXTUREACCESS_STATIC, kHaloPx, kHaloPx);
        if (nightHaloTexture_) {
            SDL_UpdateTexture(nightHaloTexture_, nullptr, pixels.data(), kHaloPx * 4);
            SDL_SetTextureBlendMode(nightHaloTexture_, SDL_BLENDMODE_BLEND);
        }
    }

    const float cx = kCenterX + T4CV2WorldMap::kIsoAnchorBiasX;
    const float cy = kCenterY + T4CV2WorldMap::kIsoAnchorBiasY - 24.f;
    /* Torche allumee : halo nettement plus large (STAT_RADIANCE cote serveur). */
    const float radius = torchLit_ ? 480.f : 300.f;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 8, alpha);
    const SDL_FRect bands[4] = {
        {0.f, 0.f, static_cast<float>(kLogicalWidth), cy - radius},
        {0.f, cy + radius, static_cast<float>(kLogicalWidth),
         static_cast<float>(kLogicalHeight) - (cy + radius)},
        {0.f, cy - radius, cx - radius, radius * 2.f},
        {cx + radius, cy - radius, static_cast<float>(kLogicalWidth) - (cx + radius), radius * 2.f},
    };
    for (const SDL_FRect &band : bands) {
        if (band.w > 0.f && band.h > 0.f) {
            SDL_RenderFillRect(renderer, &band);
        }
    }
    if (nightHaloTexture_) {
        SDL_SetTextureAlphaMod(nightHaloTexture_, alpha);
        SDL_FRect dst{cx - radius, cy - radius, radius * 2.f, radius * 2.f};
        SDL_RenderTexture(renderer, nightHaloTexture_, nullptr, &dst);
    }
}

void GameWorldScreen::renderOverheadSpeech(SDL_Renderer *renderer) {
    if (!renderer || overheadSpeech_.empty()) {
        return;
    }
    const Uint32 now = SDL_GetTicks();
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    for (auto it = overheadSpeech_.begin(); it != overheadSpeech_.end();) {
        if (now >= it->second.until) {
            it = overheadSpeech_.erase(it);
            continue;
        }
        float sx = 0.f;
        float sy = 0.f;
        bool visible = false;
        if (active.valid && it->first == active.unitId) {
            sx = kCenterX + T4CV2WorldMap::kIsoAnchorBiasX;
            sy = kCenterY + T4CV2WorldMap::kIsoAnchorBiasY;
            visible = true;
        } else if (const auto unit = remoteUnits_.find(it->first); unit != remoteUnits_.end()) {
            sx = worldToScreenX(unit->second.displayX, unit->second.displayY);
            sy = worldToScreenY(unit->second.displayX, unit->second.displayY);
            visible = true;
        }
        if (visible) {
            std::string text = it->second.text;
            if (text.size() > 60) {
                text = text.substr(0, 57) + "...";
            }
            const float w = static_cast<float>(text.size()) * 8.f + 10.f;
            SDL_FRect bg{sx - w * 0.5f, sy - 100.f, w, 16.f};
            SDL_SetRenderDrawColor(renderer, 16, 16, 28, 200);
            SDL_RenderFillRect(renderer, &bg);
            SDL_SetRenderDrawColor(renderer, 120, 110, 80, 220);
            SDL_RenderRect(renderer, &bg);
            SDL_SetRenderDrawColor(renderer, 250, 250, 235, 255);
            SDL_RenderDebugText(renderer, bg.x + 5.f, bg.y + 4.f, text.c_str());
        }
        ++it;
    }
}

namespace {

constexpr SDL_FRect kGameMenuPanel{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f - 150.f,
                                   static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f - 130.f, 300.f,
                                   260.f};
constexpr SDL_FRect kMenuBtnInventory{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f - 130.f,
                                      static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f - 90.f, 260.f,
                                      24.f};
constexpr SDL_FRect kMenuBtnStats{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f - 130.f,
                                  static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f - 58.f, 260.f, 24.f};
constexpr SDL_FRect kMenuBtnChat{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f - 130.f,
                                 static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f - 26.f, 260.f, 24.f};
constexpr SDL_FRect kMenuBtnAudio{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f - 130.f,
                                  static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f + 6.f, 260.f, 24.f};
constexpr SDL_FRect kMenuBtnReturn{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f - 130.f,
                                   static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f + 38.f, 260.f, 24.f};
constexpr SDL_FRect kMenuBtnClose{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f - 130.f,
                                  static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f + 70.f, 260.f, 24.f};
constexpr SDL_FRect kConfirmPanel{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f - 160.f,
                                  static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f - 50.f, 320.f, 100.f};
constexpr SDL_FRect kConfirmYes{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f - 130.f,
                                static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f + 10.f, 110.f, 26.f};
constexpr SDL_FRect kConfirmNo{static_cast<float>(GameWorldScreen::kLogicalWidth) * 0.5f + 20.f,
                               static_cast<float>(GameWorldScreen::kLogicalHeight) * 0.5f + 10.f, 110.f, 26.f};

}  // namespace

bool GameWorldScreen::handleGameMenuClick(const float mx, const float my) {
    if (PointInRect(mx, my, kMenuBtnInventory) || PointInRect(mx, my, kMenuBtnStats)) {
        showGameMenu_ = false;
        showInventory_ = true;
        charTab_ = PointInRect(mx, my, kMenuBtnStats) ? T4CCharacterWindow::Tab::Stats
                                                      : T4CCharacterWindow::Tab::Inventory;
        T4CLoginSessionRequestPlayerStatus();
        T4CLoginSessionRequestViewBackpack();
        T4CLoginSessionRequestViewEquipped();
        T4CLoginSessionRequestSkillList();
        T4CLoginSessionRequestSpellList();
        return true;
    }
    if (PointInRect(mx, my, kMenuBtnChat)) {
        showGameMenu_ = false;
        openChatInput();
        return true;
    }
    if (PointInRect(mx, my, kMenuBtnAudio)) {
        const float vol = T4CGameMusic::GetVolume();
        T4CGameMusic::SetVolume(vol > 0.1f ? 0.f : 0.75f);
        T4CGameMusic::SetSfxVolume(vol > 0.1f ? 0.f : 0.75f);
        return true;
    }
    if (PointInRect(mx, my, kMenuBtnReturn)) {
        showGameMenu_ = false;
        confirmReturnToLogin_ = true;
        return true;
    }
    if (PointInRect(mx, my, kMenuBtnClose)) {
        showGameMenu_ = false;
        return true;
    }
    if (!PointInRect(mx, my, kGameMenuPanel)) {
        showGameMenu_ = false;
    }
    return true;
}

bool GameWorldScreen::handleReturnConfirmClick(const float mx, const float my) {
    if (PointInRect(mx, my, kConfirmYes)) {
        confirmReturnToLogin_ = false;
        returnToLogin_ = true;
        return true;
    }
    if (PointInRect(mx, my, kConfirmNo) || !PointInRect(mx, my, kConfirmPanel)) {
        confirmReturnToLogin_ = false;
        return true;
    }
    return true;
}

void GameWorldScreen::renderGameMenu(SDL_Renderer *renderer) const {
    if (!renderer || !showGameMenu_) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 140);
    SDL_FRect dim{0.f, 0.f, static_cast<float>(kLogicalWidth), static_cast<float>(kLogicalHeight)};
    SDL_RenderFillRect(renderer, &dim);
    SDL_SetRenderDrawColor(renderer, 35, 40, 55, 240);
    SDL_RenderFillRect(renderer, &kGameMenuPanel);
    drawHudText(renderer, "Menu (Esc)", kGameMenuPanel.x + 12.f, kGameMenuPanel.y + 12.f);
    drawHudText(renderer, "Sac / Inventaire", kMenuBtnInventory.x + 8.f, kMenuBtnInventory.y + 4.f);
    drawHudText(renderer, "Stats / Equipement", kMenuBtnStats.x + 8.f, kMenuBtnStats.y + 4.f);
    drawHudText(renderer, "Chat (Entree)", kMenuBtnChat.x + 8.f, kMenuBtnChat.y + 4.f);
    const bool muted = T4CGameMusic::GetVolume() < 0.05f;
    drawHudText(renderer, muted ? "Musique/SFX: OFF" : "Musique/SFX: ON", kMenuBtnAudio.x + 8.f,
                kMenuBtnAudio.y + 4.f);
    drawHudText(renderer, "Retour personnages...", kMenuBtnReturn.x + 8.f, kMenuBtnReturn.y + 4.f);
    drawHudText(renderer, "Fermer", kMenuBtnClose.x + 8.f, kMenuBtnClose.y + 4.f);
}

void GameWorldScreen::renderReturnConfirm(SDL_Renderer *renderer) const {
    if (!renderer || !confirmReturnToLogin_) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_FRect dim{0.f, 0.f, static_cast<float>(kLogicalWidth), static_cast<float>(kLogicalHeight)};
    SDL_RenderFillRect(renderer, &dim);
    SDL_SetRenderDrawColor(renderer, 50, 30, 30, 245);
    SDL_RenderFillRect(renderer, &kConfirmPanel);
    drawHudText(renderer, "Quitter le monde ?", kConfirmPanel.x + 12.f, kConfirmPanel.y + 12.f);
    drawHudText(renderer, "Oui", kConfirmYes.x + 40.f, kConfirmYes.y + 5.f);
    drawHudText(renderer, "Non", kConfirmNo.x + 40.f, kConfirmNo.y + 5.f);
}

void GameWorldScreen::renderInventoryPanel(SDL_Renderer *renderer) const {
    if (!renderer || !showInventory_) {
        return;
    }
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    charWindow_.render(renderer, spriteAtlas_, uiFont_, kLogicalWidth, kLogicalHeight, charTab_, active,
                       playerStatus_, equipment_, backpack_, skillBook_, spellBook_);
}

void GameWorldScreen::drawTargetHpBar(SDL_Renderer *renderer, const float sx, const float sy,
                                       const int hpPercent) const {
    if (!renderer) {
        return;
    }
    const int pct = std::clamp(hpPercent, 0, 100);
    constexpr float kBarW = 48.f;
    constexpr float kBarH = 6.f;
    const float x = sx - kBarW * 0.5f;
    const float y = sy - 72.f;
    SDL_SetRenderDrawColor(renderer, 40, 20, 20, 220);
    SDL_FRect back{x, y, kBarW, kBarH};
    SDL_RenderFillRect(renderer, &back);
    SDL_SetRenderDrawColor(renderer, pct > 25 ? 200 : 220, pct > 25 ? 40 : 30, 30, 255);
    SDL_FRect fill{x, y, kBarW * static_cast<float>(pct) / 100.f, kBarH};
    SDL_RenderFillRect(renderer, &fill);
    SDL_SetRenderDrawColor(renderer, 255, 220, 80, 255);
    SDL_RenderRect(renderer, &back);
}

bool GameWorldScreen::ConsumeReturnToLogin() {
    return std::exchange(returnToLogin_, false);
}

bool GameWorldScreen::ConsumeQuitApp() {
    return std::exchange(quitApp_, false);
}

void GameWorldScreen::Update() {
    if (!ready_) {
        return;
    }

    T4CLoginSessionPollBackgroundTasks();

    const Uint32 now = SDL_GetTicks();

    if (nearItemsIdleRefreshAt_ != 0 && now >= nearItemsIdleRefreshAt_ && canMove_ &&
        !isPlayerScrolling()) {
        nearItemsIdleRefreshAt_ = 0;
        T4CLoginSessionRequestNearItems();
    }

    T4CActivePlayer popup{};
    if (T4CLoginSessionConsumePlayerPopupUpdate(&popup)) {
        // Correction de position envoyee par le serveur (popup) : on s'y aligne.
        playerX_ = popup.serverX;
        playerY_ = popup.serverY;
        snapPlayerViewToServer();
        T4CLoginSessionUpdateActivePlayerPosition(playerX_, playerY_);
        refreshZoneMusic();
        canMove_ = true;
        moveServerTimeoutUntil_ = 0;
    }

    T4CPlayerTeleport teleport{};
    if (T4CLoginSessionConsumePlayerTeleport(&teleport)) {
        if (teleport.x != 0 || teleport.y != 0) {
            playerX_ = teleport.x;
            playerY_ = teleport.y;
        }
        if (teleport.hasWorld && teleport.world != world_) {
            /* Changement de monde (sous-sol temple/crypte = monde 1 dungeonmap) :
             * recharge la carte V2 + TMI sinon on dessine le terrain de l'ancien monde. */
            world_ = teleport.world;
            if (v2Map_.OpenWorld(world_)) {
                v2Loaded_ = true;
                tmiMap_.LoadWorld(world_);
                tmiLoaded_ = tmiMap_.IsLoaded();
                SDL_Log("[GameWorld] teleport monde %u : carte V2 rechargee",
                        static_cast<unsigned>(world_));
            } else {
                v2Loaded_ = false;
                tmiLoaded_ = tmiMap_.LoadWorld(world_) && tmiMap_.IsLoaded();
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                            "[GameWorld] teleport monde %u : carte V2 indisponible (%s)",
                            static_cast<unsigned>(world_), v2Map_.GetLastError().c_str());
            }
            zoneMap_.LoadWorld(world_);
            lastZoneId_ = -1;
        }
        snapPlayerViewToServer();
        playerMoving_ = false;
        playerDead_ = false;
        showGameMenu_ = false;
        showInventory_ = false;
        hasNpcSpeech_ = false;
        shopList_ = {};
        T4CLoginSessionClearPlayerDeath();
        T4CLoginSessionUpdateActivePlayerPosition(playerX_, playerY_);
        T4CLoginSessionClearRemoteUnits();
        remoteUnits_.clear();
        groundObjects_.clear();
        endHoldMove();
        T4CLoginSessionRequestNearItems();
        scheduleNearItemsRefresh();
        refreshZoneMusic(true);
        canMove_ = true;
        moveServerTimeoutUntil_ = 0;
    }

    syncRemoteUnitsFromNetwork();
    syncGroundObjectsFromNetwork();
    updatePlayerScroll();
    updateRemoteUnitMotion(now);

    unsigned int serverX = 0;
    unsigned int serverY = 0;
    std::uint16_t serverMoveOp = 0;
    if (T4CLoginSessionConsumeLocalPlayerMove(&serverX, &serverY, &serverMoveOp)) {
        applyServerPlayerMove(serverX, serverY, serverMoveOp);
    } else if (!canMove_ && moveServerTimeoutUntil_ != 0 && now >= moveServerTimeoutUntil_) {
        canMove_ = true;
        moveServerTimeoutUntil_ = 0;
    }

    T4CPlayerStatus updated{};
    if (T4CLoginSessionConsumePlayerStatusUpdate(&updated)) {
        playerStatus_ = updated;
    }

    updatePendingAttack();

    {
        unsigned atx = 0;
        unsigned aty = 0;
        if (T4CLoginSessionConsumeLocalAttack(&atx, &aty)) {
            const Uint32 now = SDL_GetTicks();
            playerAttacking_ = true;
            playerAttackFrame_ = 0;
            playerAttackTick_ = now;
            playerAttackUntil_ = now + 70 * kT4CAttackAnimFrames;
            const int ddx = static_cast<int>(atx) - static_cast<int>(playerX_);
            const int ddy = static_cast<int>(aty) - static_cast<int>(playerY_);
            if (ddx != 0 || ddy != 0) {
                playerDirection_ = T4CDirectionFromWorldDelta(ddx, ddy);
            }
        }
    }
    if (playerAttacking_) {
        const Uint32 now = SDL_GetTicks();
        if (now >= playerAttackUntil_) {
            playerAttacking_ = false;
            playerAttackFrame_ = 0;
        } else if (now - playerAttackTick_ >= 70) {
            playerAttackFrame_ = (playerAttackFrame_ + 1) % kT4CAttackAnimFrames;
            playerAttackTick_ = now;
        }
    }

    if (T4CLoginSessionConsumePlayerDeath()) {
        playerDead_ = true;
        canMove_ = false;
        cancelPendingAttack();
    }
    T4CDeathState deathState{};
    T4CLoginSessionGetDeathState(&deathState);
    if (!deathState.active && !T4CLoginSessionIsPlayerDead()) {
        playerDead_ = false;
    } else if (deathState.active) {
        playerDead_ = true;
        canMove_ = false;
    }
    if (deathState.canResurrect && playerDead_) {
        T4CLoginSessionSendDeathResurrect();
    }

    std::vector<T4CFloatingDamage> damageEvents;
    T4CLoginSessionDrainFloatingDamage(&damageEvents);
    for (const T4CFloatingDamage &dmg : damageEvents) {
        FloatingDamageVisual visual{};
        visual.unitId = dmg.unitId;
        visual.text = dmg.text;
        visual.expireAt = now + 1800;
        visual.spawnAt = now;
        visual.color = dmg.color;
        floatingDamage_.push_back(std::move(visual));
    }
    floatingDamage_.erase(
        std::remove_if(floatingDamage_.begin(), floatingDamage_.end(),
                       [now](const FloatingDamageVisual &v) { return now >= v.expireAt; }),
        floatingDamage_.end());

    T4CPlayerBackpack backpack{};
    T4CPlayerEquipment equipment{};
    if (T4CLoginSessionConsumeInventoryUpdate()) {
        T4CLoginSessionGetBackpack(&backpack);
        T4CLoginSessionGetEquipment(&equipment);
        backpack_ = backpack;
        equipment_ = equipment;
        backpackItemCount_ = backpack.valid ? static_cast<unsigned int>(backpack.items.size()) : 0;
        equipmentKnown_ = equipment.valid;
        T4CPlayerSkillBook skills{};
        T4CPlayerSpellBook spells{};
        T4CLoginSessionGetSkillBook(&skills);
        T4CLoginSessionGetSpellBook(&spells);
        skillBook_ = std::move(skills);
        spellBook_ = std::move(spells);
    }

    T4CNpcSpeech speech{};
    if (T4CLoginSessionConsumeNpcSpeech(&speech)) {
        /* Panneau dialogue : seulement pour les PNJ — les paroles joueurs vont au chat/bulles. */
        if (speech.isNpc) {
            npcSpeech_ = std::move(speech);
            hasNpcSpeech_ = true;
        }
    }
    T4CShopList shop{};
    if (T4CLoginSessionConsumeShopList(&shop)) {
        shopList_ = std::move(shop);
    }

    pumpChatMessages();
    attackMode_ = T4CLoginSessionGetAttackMode();
    updateZoneBanner();

    /* Heure monde (jour/nuit) : refresh ~60 s comme le client Windows. */
    if (now >= nextTimeRequestAt_) {
        T4CLoginSessionRequestGameTime();
        nextTimeRequestAt_ = now + 60000;
    }

    const bool *const keys = SDL_GetKeyboardState(nullptr);
    std::uint16_t held = 0;
    if (keys) {
        const bool left = keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_KP_4];
        const bool right = keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_KP_6];
        const bool up = keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_KP_8];
        const bool down = keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_KP_2];
        held = moveOpcodeFromArrows(left, right, up, down);
    }

    // Garder l'anim marche tant que la fleche est enfoncee (pas seulement 320 ms par pas).
    if (held != 0) {
        playerAnimUntil_ = now + 450;
    }

    if (playerMoving_ && now > playerAnimUntil_) {
        playerMoving_ = false;
    }
    if (!playerMoving_) {
        playerAnimFrame_ = 0;
    }

    if (keys && canMove_ && !playerDead_ && held != 0) {
        tryMovePlayer(held);
    } else if (leftMouseHeld_ && canMove_ && !playerDead_ && !showGameMenu_ && !confirmReturnToLogin_) {
        std::uint16_t moveOp = 0;
        if (holdMoveDirection_ >= 1 && holdMoveDirection_ <= 8) {
            moveOp = static_cast<std::uint16_t>(holdMoveDirection_);
        } else if (holdMoveTargetX_ != playerX_ || holdMoveTargetY_ != playerY_) {
            moveOp = moveOpcodeToward(playerX_, playerY_, holdMoveTargetX_, holdMoveTargetY_);
        }
        if (moveOp != 0) {
            tryMovePlayer(moveOp);
        }
    }

}

void GameWorldScreen::drawHudText(SDL_Renderer *renderer, const char *text, const float x,
                                  const float y) const {
    if (!renderer || !text) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, 220, 220, 200, 255);
    SDL_RenderDebugText(renderer, x, y, text);
}

void GameWorldScreen::drawFilledCircle(SDL_Renderer *renderer, const float cx, const float cy,
                                       const float radius) const {
    if (!renderer || radius <= 0.f) {
        return;
    }
    const int ir = static_cast<int>(radius);
    for (int dy = -ir; dy <= ir; ++dy) {
        const float w = std::sqrt(radius * radius - static_cast<float>(dy * dy));
        SDL_FRect row{cx - w, cy + static_cast<float>(dy), w * 2.f, 1.f};
        SDL_RenderFillRect(renderer, &row);
    }
}

float GameWorldScreen::worldToScreenX(const unsigned int worldX) const {
    if (v2Loaded_) {
        return kCenterX + (static_cast<float>(worldX) - playerDisplayX_) * static_cast<float>(kIsoTileW) +
               T4CV2WorldMap::kIsoAnchorBiasX;
    }
    if (tmiLoaded_) {
        const int halfW = viewTilesW_ / 2;
        return mapOriginX_ + static_cast<float>(static_cast<int>(worldX) - static_cast<int>(playerX_) + halfW) *
                                 static_cast<float>(kTileSize) +
               static_cast<float>(kTileSize) * 0.5f;
    }
    return WorldToScreenX(worldX, playerX_);
}

float GameWorldScreen::worldToScreenY(const unsigned int worldY) const {
    if (v2Loaded_) {
        return kCenterY + (static_cast<float>(worldY) - playerDisplayY_) * static_cast<float>(kIsoTileH) +
               T4CV2WorldMap::kIsoAnchorBiasY;
    }
    if (tmiLoaded_) {
        const int halfH = viewTilesH_ / 2;
        return mapOriginY_ + static_cast<float>(static_cast<int>(worldY) - static_cast<int>(playerY_) + halfH) *
                                 static_cast<float>(kTileSize) +
               static_cast<float>(kTileSize) * 0.5f;
    }
    return WorldToScreenY(worldY, playerY_);
}

void GameWorldScreen::snapPlayerViewToServer() {
    T4CPlayerViewState st{playerX_, playerY_, playerDisplayX_, playerDisplayY_};
    T4CPlayerViewSnapDisplay(&st);
    playerDisplayX_ = st.displayX;
    playerDisplayY_ = st.displayY;
}

bool GameWorldScreen::isPlayerScrolling() const {
    return T4CPlayerViewIsScrolling({playerX_, playerY_, playerDisplayX_, playerDisplayY_});
}

void GameWorldScreen::updatePlayerScroll() {
    T4CPlayerViewState st{playerX_, playerY_, playerDisplayX_, playerDisplayY_};
    const float prevX = st.displayX;
    const float prevY = st.displayY;
    T4CPlayerScrollStep(&st, kT4CPlayerScrollSteps);
    playerDisplayX_ = st.displayX;
    playerDisplayY_ = st.displayY;
    if (playerMoving_ && (playerDisplayX_ != prevX || playerDisplayY_ != prevY)) {
        playerAnimFrame_ = T4CWalkAnimNextFrame(playerAnimFrame_);
    }
}

void GameWorldScreen::updateRemoteUnitMotion(const Uint32 now) {
    for (auto &entry : remoteUnits_) {
        RemoteUnitVisual &unit = entry.second;
        if (unit.attacking) {
            if (now >= unit.attackUntil) {
                unit.attacking = false;
                unit.attackAnimFrame = 0;
            } else if (now - unit.attackAnimTick >= 70) {
                unit.attackAnimFrame = (unit.attackAnimFrame + 1) % kT4CAttackAnimFrames;
                unit.attackAnimTick = now;
            }
            continue;
        }
        const float targetX = static_cast<float>(unit.serverX);
        const float targetY = static_cast<float>(unit.serverY);
        const float dx = targetX - unit.displayX;
        const float dy = targetY - unit.displayY;
        const float distSq = dx * dx + dy * dy;
        if (distSq < 0.0025f) {
            unit.displayX = targetX;
            unit.displayY = targetY;
            unit.moving = false;
            unit.animFrame = 0;
            continue;
        }
        unit.moving = true;
        const float dist = std::sqrt(distSq);
        const float step = std::min(dist, 0.18f);
        unit.displayX += dx / dist * step;
        unit.displayY += dy / dist * step;
        if (now - unit.animTick >= 90) {
            unit.animFrame = T4CWalkAnimNextFrame(unit.animFrame);
            unit.animTick = now;
        }
    }
}

float GameWorldScreen::worldToScreenX(const float worldX, const float worldY) const {
    if (v2Loaded_) {
        (void)worldY;
        return kCenterX + (worldX - playerDisplayX_) * static_cast<float>(kIsoTileW) +
               T4CV2WorldMap::kIsoAnchorBiasX;
    }
    if (tmiLoaded_) {
        const int halfW = viewTilesW_ / 2;
        return mapOriginX_ + (worldX - static_cast<float>(playerX_) + static_cast<float>(halfW)) *
                                 static_cast<float>(kTileSize) +
               static_cast<float>(kTileSize) * 0.5f;
    }
    return kCenterX + (worldX - static_cast<float>(playerX_)) * static_cast<float>(kTileSize);
}

float GameWorldScreen::worldToScreenY(const float worldX, const float worldY) const {
    if (v2Loaded_) {
        (void)worldX;
        return kCenterY + (worldY - playerDisplayY_) * static_cast<float>(kIsoTileH) +
               T4CV2WorldMap::kIsoAnchorBiasY;
    }
    if (tmiLoaded_) {
        const int halfH = viewTilesH_ / 2;
        return mapOriginY_ + (worldY - static_cast<float>(playerY_) + static_cast<float>(halfH)) *
                                 static_cast<float>(kTileSize) +
               static_cast<float>(kTileSize) * 0.5f;
    }
    return kCenterY + (worldY - static_cast<float>(playerY_)) * static_cast<float>(kTileSize);
}

bool GameWorldScreen::drawUnitSprite(SDL_Renderer *renderer, const float screenX, const float screenY,
                                     const std::int32_t unitId, const std::uint16_t appearance, const int direction,
                                     const int animFrame, const bool attacking) const {
    if (!spritesLoaded_ || !renderer || appearance == 0) {
        return false;
    }

    if (T4CIsPuppetAppearance(appearance)) {
        T4CPuppetDress dress{};
        if (!T4CLoginSessionGetRemotePuppetDress(unitId, &dress)) {
            /* Puppet PNJ (Mithrand/Nobleman…) : fallback WHITEROBE → sprite Cleric si opcode 70 pas encore recu. */
            dress.body = 425;
            dress.legs = 284;
            dress.feet = 285;
            dress.known = true;
        }
        const bool female = appearance == 10012 || appearance == 15012;
        if (T4CPuppetTryDraw(spriteAtlas_, renderer, dress, female, direction, animFrame, screenX, screenY,
                             attacking)) {
            return true;
        }
        if (tryDrawNpcBase(spriteAtlas_, renderer, T4CPuppetFallbackSpriteName(dress, female), screenX, screenY,
                           direction, animFrame, attacking)) {
            return true;
        }
    }

    T4CActivePlayer probe{};
    probe.appearance = appearance;
    probe.valid = true;
    if (appearance >= 10001 && appearance <= 10004) {
        probe.classIndex = static_cast<std::uint8_t>(appearance - 10001);
    } else if (appearance >= 15001 && appearance <= 15004) {
        probe.classIndex = static_cast<std::uint8_t>(appearance - 15001);
    }

    if (!T4CIsPuppetAppearance(appearance) && appearance >= 10001 && appearance <= 15004) {
        const std::string standing = T4CPlayerUnitSpriteName(probe, direction, animFrame);
        if (!standing.empty()) {
            const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(direction);
            if (spriteAtlas_.TryDrawSpriteByName(renderer, standing, screenX, screenY, view.mirror)) {
                return true;
            }
        }
    }

    const char *npcBase = T4CRemoteUnitSpriteName(unitId, appearance);
    if (tryDrawNpcBase(spriteAtlas_, renderer, npcBase, screenX, screenY, direction, animFrame, attacking)) {
        return true;
    }

    return false;
}

bool GameWorldScreen::drawPlayerSprite(SDL_Renderer *renderer, const float screenX, const float screenY) const {
    if (!spritesLoaded_ || !renderer) {
        return false;
    }

    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    if (!active.valid) {
        return false;
    }

    const int frame = playerAttacking_ ? playerAttackFrame_ : (playerMoving_ ? playerAnimFrame_ : 0);
    if (T4CPuppetTryDrawPlayer(spriteAtlas_, renderer, active, playerDirection_, frame, screenX, screenY,
                               playerAttacking_)) {
        return true;
    }

    const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(playerDirection_);
    if (playerAttacking_) {
        const char *base = T4CPlayerSpriteNpcName(active);
        const std::string attackName = T4CUnitAttackSpriteFrameName(base, view, frame);
        if (!attackName.empty() &&
            spriteAtlas_.TryDrawSpriteByName(renderer, attackName, screenX, screenY, view.mirror)) {
            return true;
        }
    }

    const std::string spriteName = T4CPlayerUnitSpriteName(active, playerDirection_, playerAttacking_ ? 0 : frame);
    if (spriteName.empty()) {
        return false;
    }

    return spriteAtlas_.TryDrawSpriteByName(renderer, spriteName, screenX, screenY, view.mirror);
}

void GameWorldScreen::Render(SDL_Renderer *renderer, LauncherChrome *chrome) {
    if (!renderer || !ready_) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 12, 12, 16, 255);
    SDL_RenderClear(renderer);

    if (v2Loaded_) {
        const int radius = isoViewRadius();
        const T4CTmiWorldMap *tmi = tmiLoaded_ ? &tmiMap_ : nullptr;
        const T4CV2SpriteAtlas *sprites = spritesLoaded_ ? &spriteAtlas_ : nullptr;
        if (sprites && sprites->IsReady()) {
            spriteAtlas_.BeginRenderFrame();
        }
        v2Map_.RenderIsoViewport(renderer, playerDisplayX_, playerDisplayY_, kCenterX, kCenterY, radius, tmi,
                                 sprites);
    } else if (tmiLoaded_) {
        tmiMap_.RenderViewport(renderer, playerX_, playerY_, viewTilesW_, viewTilesH_, kTileSize, mapOriginX_,
                               mapOriginY_);
    } else {
        const int phaseX = static_cast<int>(playerX_ % 32);
        const int phaseY = static_cast<int>(playerY_ % 32);
        const float gridOriginX = kCenterX - static_cast<float>(phaseX * kTileSize);
        const float gridOriginY = kCenterY - static_cast<float>(phaseY * kTileSize);

        SDL_SetRenderDrawColor(renderer, 32, 96, 40, 255);
        for (float x = gridOriginX; x < static_cast<float>(kLogicalWidth) + kTileSize; x += kTileSize) {
            SDL_RenderLine(renderer, x, 0.f, x, static_cast<float>(kLogicalHeight));
        }
        for (float y = gridOriginY; y < static_cast<float>(kLogicalHeight) + kTileSize; y += kTileSize) {
            SDL_RenderLine(renderer, 0.f, y, static_cast<float>(kLogicalWidth), y);
        }
    }

    SDL_SetRenderDrawColor(renderer, 180, 200, 255, 255);
    bool selectionVisible = false;
    float selSx = 0.f;
    float selSy = 0.f;
    std::uint16_t selAppearance = 0;
    for (const auto &entry : groundObjects_) {
        const float sx = worldToScreenX(entry.second.x, entry.second.y);
        const float sy = worldToScreenY(entry.second.x, entry.second.y);
        drawGroundObjectMarker(renderer, sx, sy, entry.second.appearance);
        if (entry.first == hoveredGroundId_ && entry.first != selectedGroundId_) {
            if (const char *spriteName = T4CGroundObjectSpriteName(entry.second.appearance);
                spriteName != nullptr &&
                spriteAtlas_.TryDrawSpriteOutline(renderer, spriteName, sx, sy, 120, 200, 255)) {
                /* contour sprite */
            } else {
                SDL_SetRenderDrawColor(renderer, 100, 180, 255, 180);
                SDL_FRect hoverBox{sx - 11.f, sy - 13.f, 22.f, 16.f};
                SDL_RenderRect(renderer, &hoverBox);
            }
        }
    }
    for (const auto &entry : remoteUnits_) {
        const float sx = worldToScreenX(entry.second.displayX, entry.second.displayY);
        const float sy = worldToScreenY(entry.second.displayX, entry.second.displayY);
        const std::uint16_t appearance = entry.second.appearance;
        if (entry.first == hoveredUnitId_ && entry.first != selectedUnitId_) {
            drawUnitOutline(renderer, sx, sy, entry.first, appearance);
        }
        if (entry.first == selectedUnitId_) {
            selectionVisible = true;
            selSx = sx;
            selSy = sy;
            selAppearance = appearance;
        }
        const int frame = entry.second.attacking
                              ? entry.second.attackAnimFrame
                              : (entry.second.moving ? entry.second.animFrame : 0);
        if (!drawUnitSprite(renderer, sx, sy, entry.first, appearance, entry.second.direction, frame,
                            entry.second.attacking)) {
            SDL_FRect dot{sx - 4.f, sy - 4.f, 8.f, 8.f};
            SDL_RenderFillRect(renderer, &dot);
        }
    }

    // Encadre + libelle de l'unite selectionnee (V3_TargetDlg simplifie).
    if (selectionVisible) {
        drawUnitOutline(renderer, selSx, selSy, selectedUnitId_, selAppearance);
        if (const auto selIt = remoteUnits_.find(selectedUnitId_); selIt != remoteUnits_.end()) {
            drawTargetHpBar(renderer, selSx, selSy, static_cast<int>(selIt->second.hpPercent));
        }
        SDL_FRect labelBg{selSx - 48.f, selSy - 82.f, 96.f, 18.f};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer, &labelBg);
        const char *base = T4CRemoteUnitSpriteName(selectedUnitId_, selAppearance);
        const bool isPlayer = (selAppearance >= 10001 && selAppearance <= 10004) ||
                              (selAppearance >= 15001 && selAppearance <= 15004);
        char label[96];
        std::string realName;
        if (T4CLoginSessionGetUnitName(selectedUnitId_, &realName, nullptr)) {
            /* Nom serveur (RQ_GetUnitName 35) : « Dark Fang », « Mithrand », « Olin Haad guard »… */
            std::snprintf(label, sizeof(label), "%s", realName.c_str());
        } else if (isPlayer) {
            std::snprintf(label, sizeof(label), "Joueur #%d", selectedUnitId_);
        } else if (T4CIsPuppetAppearance(selAppearance)) {
            T4CPuppetDress dress{};
            if (T4CLoginSessionGetRemotePuppetDress(selectedUnitId_, &dress) && dress.body == 425) {
                std::snprintf(label, sizeof(label), "Pretre");
            } else if (dress.body == 269) {
                std::snprintf(label, sizeof(label), "Garde");
            } else {
                std::snprintf(label, sizeof(label), "PNJ");
            }
        } else if (base) {
            std::snprintf(label, sizeof(label), "%s", base);
        } else {
            std::snprintf(label, sizeof(label), "PNJ #%d", selectedUnitId_);
        }
        const float labelW = static_cast<float>(std::strlen(label)) * 8.f + 8.f;
        if (labelW > labelBg.w) {
            labelBg.x = selSx - labelW * 0.5f;
            labelBg.w = labelW;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
            SDL_RenderFillRect(renderer, &labelBg);
        }
        drawHudText(renderer, label, labelBg.x + 4.f, selSy - 80.f);
    } else {
        selectedUnitId_ = 0;
    }

    /* Etiquette nom au clic droit (examine) — affichee pres de l'objet pendant 3 s. */
    if (!examineLabel_.empty() && SDL_GetTicks() < examineLabelUntil_) {
        const float lx = worldToScreenX(examineLabelWorldX_, examineLabelWorldY_);
        const float ly = worldToScreenY(examineLabelWorldX_, examineLabelWorldY_);
        const float w = static_cast<float>(examineLabel_.size()) * 8.f + 12.f;
        SDL_FRect bg{lx - w * 0.5f, ly - 34.f, w, 18.f};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer, &bg);
        SDL_SetRenderDrawColor(renderer, 120, 200, 255, 255);
        SDL_RenderRect(renderer, &bg);
        drawHudText(renderer, examineLabel_.c_str(), lx - w * 0.5f + 6.f, ly - 30.f);
    } else if (!examineLabel_.empty()) {
        examineLabel_.clear();
    }

    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    const float playerAnchorX = kCenterX + T4CV2WorldMap::kIsoAnchorBiasX;
    const float playerAnchorY = kCenterY + T4CV2WorldMap::kIsoAnchorBiasY;
    if (spritesLoaded_ && active.valid) {
        if (!drawPlayerSprite(renderer, playerAnchorX, playerAnchorY)) {
            SDL_SetRenderDrawColor(renderer, 240, 210, 40, 255);
            drawFilledCircle(renderer, kCenterX, kCenterY, 14.f);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            drawFilledCircle(renderer, kCenterX, kCenterY, 4.f);
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 240, 210, 40, 255);
        drawFilledCircle(renderer, kCenterX, kCenterY, 14.f);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawFilledCircle(renderer, kCenterX, kCenterY, 4.f);
    }

    const Uint32 renderNow = SDL_GetTicks();
    for (const FloatingDamageVisual &dmg : floatingDamage_) {
        float sx = 0.f;
        float sy = 0.f;
        if (const auto it = remoteUnits_.find(dmg.unitId); it != remoteUnits_.end()) {
            sx = worldToScreenX(it->second.displayX, it->second.displayY);
            sy = worldToScreenY(it->second.displayX, it->second.displayY);
        } else if (active.valid && dmg.unitId == active.unitId) {
            sx = playerAnchorX;
            sy = playerAnchorY;
        } else {
            continue;
        }
        /* Texte montant + couleur serveur (rouge degats / vert soin, Windows DamageUnit). */
        const float rise = static_cast<float>(renderNow - dmg.spawnAt) * 0.022f;
        Uint8 r = 255;
        Uint8 g = 80;
        Uint8 b = 60;
        if (dmg.color != 0) {
            ColorrefToRgb(dmg.color, r, g, b);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_RenderDebugText(renderer, sx - 11.f, sy - 77.f - rise, dmg.text.c_str());
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDebugText(renderer, sx - 12.f, sy - 78.f - rise, dmg.text.c_str());
    }

    renderNightOverlay(renderer);
    renderOverheadSpeech(renderer);

    /* Bandeau zone (« Vous entrez dans... ») — haut centre, 4 s. */
    if (!zoneBanner_.empty() && SDL_GetTicks() < zoneBannerUntil_) {
        const float w = static_cast<float>(zoneBanner_.size()) * 8.f + 28.f;
        SDL_FRect banner{(static_cast<float>(kLogicalWidth) - w) * 0.5f, 40.f, w, 24.f};
        SDL_SetRenderDrawColor(renderer, 14, 14, 26, 210);
        SDL_RenderFillRect(renderer, &banner);
        SDL_SetRenderDrawColor(renderer, 180, 160, 90, 255);
        SDL_RenderRect(renderer, &banner);
        SDL_SetRenderDrawColor(renderer, 255, 232, 160, 255);
        SDL_RenderDebugText(renderer, banner.x + 14.f, banner.y + 8.f, zoneBanner_.c_str());
    } else if (!zoneBanner_.empty() && SDL_GetTicks() >= zoneBannerUntil_) {
        zoneBanner_.clear();
    }

    /* Indicateur mode attaque (Ctrl+C) — bandeau discret haut gauche. */
    if (attackMode_) {
        SDL_FRect badge{8.f, 8.f, 130.f, 20.f};
        SDL_SetRenderDrawColor(renderer, 60, 8, 8, 200);
        SDL_RenderFillRect(renderer, &badge);
        SDL_SetRenderDrawColor(renderer, 220, 60, 40, 255);
        SDL_RenderRect(renderer, &badge);
        SDL_SetRenderDrawColor(renderer, 255, 120, 90, 255);
        SDL_RenderDebugText(renderer, badge.x + 10.f, badge.y + 6.f, "MODE ATTAQUE");
    }

    const T4CUiFont *font = chrome ? &chrome->font() : nullptr;
    uiFont_ = font;
    if (spritesLoaded_) {
        gameHud_.render(renderer, spriteAtlas_, font, kLogicalWidth, kLogicalHeight, active, playerStatus_,
                        static_cast<unsigned int>(world_), tmiLoaded_ ? &tmiMap_ : nullptr);
    }

    renderDialoguePanel(renderer);
    renderShopPanel(renderer);
    renderChatLog(renderer);
    renderInventoryPanel(renderer);
    renderGameMenu(renderer);
    renderReturnConfirm(renderer);

    if (mouseValid_ && spritesLoaded_) {
        if (leftMouseHeld_ && holdMoveDirection_ >= 1 && holdMoveDirection_ <= 8) {
            if (const char *cursor = kHoldMoveCursorSprite(holdMoveDirection_); cursor != nullptr) {
                spriteAtlas_.TryDrawSpriteByName(renderer, cursor, mouseX_, mouseY_);
            }
        } else {
            /* Curseur contextuel Windows : gant par defaut, epee animee (oscillante) sur un
             * ennemi, curseur parole sur un PNJ amical (main2.cpp FirstInitObject). */
            bool overEnemy = false;
            bool overNpc = false;
            if (hoveredUnitId_ != 0) {
                const auto hovIt = remoteUnits_.find(hoveredUnitId_);
                if (hovIt != remoteUnits_.end()) {
                    const std::uint16_t app = hovIt->second.appearance;
                    T4CActivePlayer me{};
                    T4CLoginSessionGetActivePlayer(&me);
                    const bool isSelf = me.valid && me.unitId != 0 && hoveredUnitId_ == me.unitId;
                    const bool isCorpse = app >= 15001 && app <= 15012;
                    const bool isMonster = app >= 20000 && app <= 24999;
                    if (!isSelf && !isCorpse) {
                        if (isMonster || attackMode_) {
                            overEnemy = true;
                        } else if (isNpcUnit(hoveredUnitId_, app)) {
                            overNpc = true;
                        }
                    }
                }
            }
            if (overEnemy) {
                /* 12 frames sword0000..sword0022 (pas de 2) — V3MouseCursor2 Windows. */
                const int frame = static_cast<int>((SDL_GetTicks() / 90u) % 12u) * 2;
                char swordName[16];
                std::snprintf(swordName, sizeof(swordName), "sword%04d", frame);
                if (!spriteAtlas_.TryDrawSpriteByName(renderer, swordName, mouseX_, mouseY_)) {
                    spriteAtlas_.TryDrawSpriteByName(renderer, "StaticAttackCursor", mouseX_, mouseY_);
                }
            } else if (overNpc) {
                if (!spriteAtlas_.TryDrawSpriteByName(renderer, "V3_TalkCursor", mouseX_, mouseY_)) {
                    spriteAtlas_.TryDrawSpriteByName(renderer, "glove0000", mouseX_, mouseY_);
                }
            } else if (!spriteAtlas_.TryDrawSpriteByName(renderer, "glove0000", mouseX_, mouseY_)) {
                spriteAtlas_.TryDrawSpriteByName(renderer, "StaticAttackCursor", mouseX_, mouseY_);
            }
        }
    }

    if (playerDead_ || T4CLoginSessionIsPlayerDead()) {
        SDL_SetRenderDrawColor(renderer, 80, 0, 0, 180);
        SDL_FRect deathOverlay{0.f, static_cast<float>(kLogicalHeight) * 0.35f,
                               static_cast<float>(kLogicalWidth), 100.f};
        SDL_RenderFillRect(renderer, &deathOverlay);
        drawHudText(renderer, "VOUS ETES MORT", static_cast<float>(kLogicalWidth) * 0.5f - 60.f,
                    static_cast<float>(kLogicalHeight) * 0.35f + 20.f);
        T4CDeathState death{};
        T4CLoginSessionGetDeathState(&death);
        if (death.canResurrect) {
            drawHudText(renderer, "Ressurrection au temple...",
                        static_cast<float>(kLogicalWidth) * 0.5f - 90.f,
                        static_cast<float>(kLogicalHeight) * 0.35f + 44.f);
        } else if (death.timeTotal > 0) {
            char timer[64];
            std::snprintf(timer, sizeof(timer), "Attente %u / %u", death.timeCurrent, death.timeTotal);
            drawHudText(renderer, timer, static_cast<float>(kLogicalWidth) * 0.5f - 50.f,
                        static_cast<float>(kLogicalHeight) * 0.35f + 44.f);
        }
    }
}
