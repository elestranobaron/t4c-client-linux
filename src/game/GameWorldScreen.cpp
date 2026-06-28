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
#include <unordered_set>
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

bool tryDrawNpcBase(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const char *base, const float screenX,
                    const float screenY, const int direction, const int animFrame, const bool attacking,
                    const bool idleStanding) {
    if (!base || base[0] == '\0' || !renderer) {
        return false;
    }
    const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(direction);
    const int maxWalkFrame = kT4CNpcWalkAnimFrames - 1;
    if (attacking) {
        const std::string attackFrame = T4CUnitAttackSpriteFrameName(base, view, animFrame);
        if (!attackFrame.empty() && atlas.TryDrawSpriteByName(renderer, attackFrame, screenX, screenY, view.mirror)) {
            return true;
        }
    }
    /* StMov absent du port V2 (V2DataI.did) : a l'arret, frame 0 du cycle marche (Windows pose statique). */
    const int frameIndex = idleStanding ? 0 : animFrame;
    const std::string frame = T4CUnitSpriteFrameName(base, view, frameIndex, maxWalkFrame);
    if (!frame.empty() && atlas.TryDrawSpriteByName(renderer, frame, screenX, screenY, view.mirror)) {
        return true;
    }
    if (!idleStanding && frameIndex != 0) {
        const std::string frame0 = T4CUnitSpriteFrameName(base, view, 0, maxWalkFrame);
        if (!frame0.empty() && atlas.TryDrawSpriteByName(renderer, frame0, screenX, screenY, view.mirror)) {
            return true;
        }
    }
    return false;
}

bool tryDrawNpcOutline(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const char *base, const float screenX,
                       const float screenY, const int direction, const int animFrame, const bool attacking,
                       const bool idleStanding) {
    if (!base || base[0] == '\0' || !renderer) {
        return false;
    }
    const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(direction);
    const int maxWalkFrame = kT4CNpcWalkAnimFrames - 1;
    if (attacking) {
        const std::string attackFrame = T4CUnitAttackSpriteFrameName(base, view, animFrame);
        if (!attackFrame.empty() &&
            atlas.TryDrawSpriteOutline(renderer, attackFrame, screenX, screenY, 255, 255, 255, view.mirror)) {
            return true;
        }
    }
    const int frameIndex = idleStanding ? 0 : animFrame;
    const std::string frame = T4CUnitSpriteFrameName(base, view, frameIndex, maxWalkFrame);
    if (!frame.empty() &&
        atlas.TryDrawSpriteOutline(renderer, frame, screenX, screenY, 255, 255, 255, view.mirror)) {
        return true;
    }
    return false;
}

bool tryDrawCorpse(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const char *base, const float screenX,
                   const float screenY, const int corpseFrame) {
    if (!base || base[0] == '\0' || !renderer) {
        return false;
    }
    const std::string frame = T4CUnitCorpseSpriteFrameName(base, corpseFrame);
    if (!frame.empty() && atlas.TryDrawSpriteByName(renderer, frame, screenX, screenY)) {
        return true;
    }
    /* Pas de fallback sur le sprite vivant : un cadavre sans sprite dedie doit
       disparaitre, sinon le monstre "mort" reste affiche vivant (clone fige). */
    return false;
}

void LogUnitDrawFailOnce(const std::int32_t unitId, const std::uint16_t appearance, const char *reason) {
    static std::unordered_set<std::uint32_t> logged;
    const std::uint32_t key =
        (static_cast<std::uint32_t>(unitId) << 16) | static_cast<std::uint32_t>(appearance);
    if (!logged.insert(key).second) {
        return;
    }
    const char *sprite = T4CRemoteUnitSpriteName(unitId, appearance);
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "[GameWorld] drawUnitSprite KO id=%d appearance=%u sprite=%s (%s)", unitId,
                static_cast<unsigned>(appearance), sprite != nullptr ? sprite : "?", reason);
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

bool tryDrawAnimatedGloveCursor(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const float x,
                                const float y) {
    const int frame = static_cast<int>((SDL_GetTicks() / 90u) % 12u);
    char gloveName[16];
    std::snprintf(gloveName, sizeof(gloveName), "glove%04d", frame);
    if (atlas.TryDrawSpriteByName(renderer, gloveName, x, y)) {
        return true;
    }
    return atlas.TryDrawSpriteByName(renderer, "glove0000", x, y);
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
    cancelPendingGroundAction();
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

void GameWorldScreen::finishAssetLoad() {
    loading_ = false;
    preloadQueue_.clear();
    preloadIndex_ = 0;
    loadProgress_ = 1.f;
}

void GameWorldScreen::completeSessionEnter() {
    if (ready_) {
        return;
    }
    ready_ = true;
    canMove_ = true;

    if (!T4CLoginSessionRequestNearItems()) {
        lastError_ = "T4CLoginSessionRequestNearItems a echoue";
    }
    T4CLoginSessionRequestPlayerStatus();
    T4CLoginSessionRequestViewEquipped();
    T4CLoginSessionRequestViewBackpack();
    T4CLoginSessionRequestSkillList();
    T4CLoginSessionRequestSpellList();
    T4CLoginSessionUpdateActivePlayerPosition(playerX_, playerY_);

    zoneMap_.LoadWorld(world_);
    lastZoneId_ = zoneMap_.IsLoaded() ? zoneMap_.ZoneIdAt(playerX_, playerY_) : -1;
}

void GameWorldScreen::tryCompleteSessionEnter() {
    if (loading_ || ready_ || !T4CLoginSessionIsWorldSessionReady()) {
        return;
    }
    completeSessionEnter();
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
    canMove_ = false;
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
            T4CActivePlayer active{};
            T4CLoginSessionGetActivePlayer(&active);
            T4CPuppetDress dress{};
            dress.body = 285;
            dress.legs = 284;
            dress.known = true;
            if (active.valid && active.puppetKnown) {
                dress.body = active.puppetBody;
                dress.feet = active.puppetFeet;
                dress.gloves = active.puppetGloves;
                dress.helm = active.puppetHelm;
                dress.legs = active.puppetLegs;
                dress.weaponR = active.puppetWeaponR;
                dress.weaponL = active.puppetWeaponL;
                dress.cape = active.puppetCape;
                dress.known = true;
            }
            T4CPuppetAppendDressPreload(dress, active.valid && active.female, &preloadQueue_);
            if (active.valid) {
                if (const char *base = T4CPlayerSpriteNpcName(active); base != nullptr) {
                    T4CAppendUnitSpritePreload(base, &preloadQueue_);
                }
            }
            gameHud_.preloadSprites(&spriteAtlas_);
            preloadQueue_.push_back("V3_LifeBackF");
            preloadQueue_.push_back("V3_PVBar");
            preloadQueue_.push_back("V3_PMBar");
            preloadQueue_.push_back("StaticAttackCursor");
            for (int f = 0; f <= 11; ++f) {
                char gloveName[16];
                char gloveMask[20];
                std::snprintf(gloveName, sizeof(gloveName), "glove%04d", f);
                std::snprintf(gloveMask, sizeof(gloveMask), "glove%04dMask", f);
                preloadQueue_.push_back(gloveName);
                preloadQueue_.push_back(gloveMask);
            }
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
            deferredPreloadQueue_.clear();
            deferredPreloadIndex_ = 0;
            SDL_Log("[GameWorld] preload bloquant : %zu sprites (creatures a la demande au spawn)",
                    preloadQueue_.size());
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
        finishAssetLoad();
    }
    return true;
}

bool GameWorldScreen::PollLoadForMs(const int maxMs) {
    if (!loading_ || ready_) {
        return ready_;
    }

    if (spritesLoaded_ && preloadIndex_ < static_cast<int>(preloadQueue_.size())) {
        const int processed =
            spriteAtlas_.PreloadSpritesForMs(preloadQueue_, preloadIndex_, std::max(1, maxMs));
        preloadIndex_ += processed;
        if (!preloadQueue_.empty()) {
            loadProgress_ = static_cast<float>(preloadIndex_) /
                            static_cast<float>(preloadQueue_.size());
        }
    }

    if (preloadIndex_ >= static_cast<int>(preloadQueue_.size())) {
        finishAssetLoad();
    }
    tryCompleteSessionEnter();
    return ready_;
}

void GameWorldScreen::pollDeferredSpritePreload(const int maxMs) {
    if (!ready_ || !spritesLoaded_ || deferredPreloadQueue_.empty()) {
        return;
    }
    if (deferredPreloadIndex_ >= static_cast<int>(deferredPreloadQueue_.size())) {
        return;
    }
    deferredPreloadIndex_ +=
        spriteAtlas_.PreloadSpritesForMs(deferredPreloadQueue_, deferredPreloadIndex_, maxMs);
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
        PollLoadForMs(32);
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
    cancelInventoryDrag();
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
    deferredPreloadQueue_.clear();
    deferredPreloadIndex_ = 0;
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
        const bool isNew = previous.find(marker.unitId) == previous.end();
        groundObjects_[marker.unitId] = marker;
        if (isNew && spritesLoaded_) {
            if (const char *sp = T4CGroundObjectSpriteName(marker.appearance); sp != nullptr) {
                const std::string name(sp);
                if (std::find(deferredPreloadQueue_.begin(), deferredPreloadQueue_.end(), name) ==
                    deferredPreloadQueue_.end()) {
                    deferredPreloadQueue_.push_back(name);
                }
                spriteAtlas_.PreloadBanksForNames(deferredPreloadQueue_);
                spriteAtlas_.PreloadSprites({name}, 0, 1);
            }
        }
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

void GameWorldScreen::preloadUnitSpritesForAppearance(const std::uint16_t appearance) {
    if (!spritesLoaded_ || appearance == 0 || T4CIsPuppetAppearance(appearance)) {
        return;
    }
    const char *base = T4CSpriteNameFromAppearance(appearance);
    if (base == nullptr || base[0] == '\0') {
        return;
    }
    std::vector<std::string> names;
    T4CAppendUnitSpritePreloadMinimal(base, &names);
    if (names.empty()) {
        return;
    }
    spriteAtlas_.PreloadBanksForNames(names);
    spriteAtlas_.PreloadSpritesForMs(names, 0, 16);
}

void GameWorldScreen::syncRemoteUnitsFromNetwork() {
    std::vector<T4CRemoteUnitEvent> events;
    T4CLoginSessionDrainRemoteUnitEvents(&events);
    const auto beginDeath = [](RemoteUnitVisual &unit, const std::uint16_t corpseApp, const Uint32 now) {
        if (unit.dying) {
            return;
        }
        unit.liveAppearance = unit.appearance != 0 ? unit.appearance : corpseApp;
        unit.corpseAppearance = corpseApp;
        unit.dying = true;
        /* Windows ChangeType : Killed=true, KillTimer, Type reste VIVANT ~500 ms puis KillType. */
        unit.showCorpse = false;
        unit.displayX = static_cast<float>(unit.serverX);
        unit.displayY = static_cast<float>(unit.serverY);
        unit.dieStartedAt = now;
        unit.corpseFrame = 0;
        unit.corpseAnimTick = now;
        unit.moving = false;
        unit.attacking = false;
        unit.attackAnimFrame = 0;
        unit.animFrame = 0;
        unit.idleAnimFrame = 0;
        unit.hpPercent = 0;
    };
    for (const T4CRemoteUnitEvent &ev : events) {
        if (ev.kind != T4CRemoteUnitEventKind::Remove && ev.kind != T4CRemoteUnitEventKind::Die &&
            T4CLoginSessionIsRemoteUnitDying(ev.unitId)) {
            continue;
        }
        if (T4CLoginSessionShouldSkipRemoteUnit(ev.appearance, ev.unitId, ev.x, ev.y)) {
            remoteUnits_.erase(ev.unitId);
            continue;
        }

        switch (ev.kind) {
            case T4CRemoteUnitEventKind::Remove:
                remoteUnits_.erase(ev.unitId);
                T4CLoginSessionFinalizeRemoteUnitDeath(ev.unitId);
                if (selectedUnitId_ == ev.unitId) {
                    selectedUnitId_ = 0;
                }
                if (hoveredUnitId_ == ev.unitId) {
                    hoveredUnitId_ = 0;
                }
                break;
            case T4CRemoteUnitEventKind::Die: {
                const auto it = remoteUnits_.find(ev.unitId);
                if (it == remoteUnits_.end()) {
                    break;
                }
                RemoteUnitVisual &unit = it->second;
                const Uint32 now = SDL_GetTicks();
                beginDeath(unit, ev.appearance, now);
                /* Tuer les cadavres GetNearItems fantomes au meme tile (clone Bat 25002). */
                const unsigned deadX = unit.serverX;
                const unsigned deadY = unit.serverY;
                std::vector<std::int32_t> duplicateCorpses;
                for (const auto &other : remoteUnits_) {
                    if (other.first == ev.unitId) {
                        continue;
                    }
                    if (other.second.serverX == deadX && other.second.serverY == deadY &&
                        T4CIsCreatureCorpseAppearance(other.second.appearance)) {
                        duplicateCorpses.push_back(other.first);
                    }
                }
                for (const std::int32_t dupeId : duplicateCorpses) {
                    remoteUnits_.erase(dupeId);
                    T4CLoginSessionFinalizeRemoteUnitDeath(dupeId);
                }
                if (selectedUnitId_ == ev.unitId) {
                    selectedUnitId_ = 0;
                }
                if (hoveredUnitId_ == ev.unitId) {
                    hoveredUnitId_ = 0;
                }
                break;
            }
            case T4CRemoteUnitEventKind::Spawn:
            case T4CRemoteUnitEventKind::Move:
            case T4CRemoteUnitEventKind::Update:
            case T4CRemoteUnitEventKind::Attack: {
                const auto existing = remoteUnits_.find(ev.unitId);
                if (existing != remoteUnits_.end() && existing->second.dying) {
                    break;
                }
                RemoteUnitVisual &unit = remoteUnits_[ev.unitId];
                const bool firstSpawn = !unit.initialized;
                const bool hasPosition =
                    ev.snapDisplay || ev.kind == T4CRemoteUnitEventKind::Spawn ||
                    ev.kind == T4CRemoteUnitEventKind::Move || ev.kind == T4CRemoteUnitEventKind::Attack;
                const int dx = hasPosition ? static_cast<int>(ev.x) - static_cast<int>(unit.serverX) : 0;
                const int dy = hasPosition ? static_cast<int>(ev.y) - static_cast<int>(unit.serverY) : 0;
                if (hasPosition) {
                    if (unit.initialized && (dx != 0 || dy != 0)) {
                        unit.direction = T4CDirectionFromServerMoveConfirm(dx, dy, ev.moveOpcode);
                    } else if (ev.moveOpcode >= 1 && ev.moveOpcode <= 8) {
                        unit.direction = T4CDirectionFromMoveOpcode(ev.moveOpcode);
                    }
                    if (firstSpawn) {
                        unit.displayX = static_cast<float>(ev.x);
                        unit.displayY = static_cast<float>(ev.y);
                        unit.initialized = true;
                        if (ev.appearance != 0) {
                            preloadUnitSpritesForAppearance(ev.appearance);
                        }
                    } else if (ev.kind == T4CRemoteUnitEventKind::Move && !ev.snapDisplay &&
                               std::abs(dx) <= 3 && std::abs(dy) <= 3 && (dx != 0 || dy != 0)) {
                        /* Move UDP tuile par tuile : retard visuel borne a 1 tuile (Windows MovX/SpeedX). */
                        const float ddx = static_cast<float>(ev.x) - unit.displayX;
                        const float ddy = static_cast<float>(ev.y) - unit.displayY;
                        const float lag = std::max(std::abs(ddx), std::abs(ddy));
                        if (lag > 1.0f) {
                            unit.displayX = static_cast<float>(ev.x) - ddx / lag;
                            unit.displayY = static_cast<float>(ev.y) - ddy / lag;
                        }
                        unit.moving = true;
                        unit.animTick = SDL_GetTicks();
                    } else if (ev.kind == T4CRemoteUnitEventKind::Spawn || ev.snapDisplay ||
                               std::abs(dx) > 3 || std::abs(dy) > 3) {
                        /* GetNearItems / combat snap : snap (HEAD, pas interpolation). */
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
                    if (ev.snapDisplay || ev.kind == T4CRemoteUnitEventKind::Attack) {
                        unit.displayX = static_cast<float>(ev.x);
                        unit.displayY = static_cast<float>(ev.y);
                        unit.moving = false;
                    }
                } else if (firstSpawn) {
                    unit.initialized = true;
                }
                if (ev.appearance != 0) {
                    unit.appearance = ev.appearance;
                }
                if (ev.kind == T4CRemoteUnitEventKind::Spawn || ev.kind == T4CRemoteUnitEventKind::Update) {
                    unit.friendStatus = ev.friendStatus;
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

    /* Cadavres spawnes par GetNearItems (25xxx) = clones ; seule la sequence Die les affiche. */
    for (auto it = remoteUnits_.begin(); it != remoteUnits_.end();) {
        if (!it->second.dying && T4CIsCreatureCorpseAppearance(it->second.appearance)) {
            T4CLoginSessionFinalizeRemoteUnitDeath(it->first);
            it = remoteUnits_.erase(it);
        } else {
            ++it;
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

bool GameWorldScreen::useBagItemByBaseId(const std::uint16_t baseId) {
    T4CPlayerBackpack bp = backpack_;
    if (!bp.valid) {
        T4CLoginSessionGetBackpack(&bp);
    }
    if (!bp.valid) {
        T4CLoginSessionPushSystemMessage("Sac indisponible.");
        return false;
    }
    for (const T4CBagItem &item : bp.items) {
        if (item.baseId != baseId) {
            continue;
        }
        T4CLoginSessionSendUseBagItem(item.objectId);
        noteBagItemUsed(item);
        return true;
    }
    T4CLoginSessionPushSystemMessage("Objet introuvable dans le sac.");
    return false;
}

bool GameWorldScreen::isNpcUnit(const std::int32_t unitId, const std::uint16_t appearance,
                                const char friendStatus) const {
    if (unitId == 0) {
        return false;
    }
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    if (active.valid && unitId == active.unitId) {
        return false;
    }
    /* Windows VisualObjectList : VOL_NPC → curseur TALK / MouseAction grid 3. */
    switch (static_cast<unsigned char>(friendStatus)) {
        case static_cast<unsigned char>(kT4CVolCanTalk):
            return true;
        case static_cast<unsigned char>(kT4CVolCannotTalk):
            return false;
        case static_cast<unsigned char>(kT4CVolIsPlayer):
        case static_cast<unsigned char>(kT4CVolIsMinions):
            return false;
        default:
            break;
    }
    if ((appearance >= 10001 && appearance <= 10004) || (appearance >= 15001 && appearance <= 15004)) {
        return false;
    }
    if (appearance >= 20000 && appearance <= 24999) {
        return false;
    }
    /* PNJ humains / puppet (ex. 10011 Mithrand) quand le serveur n'a pas encore pousse Friend. */
    return appearance >= 10001 && appearance < 20000;
}

bool GameWorldScreen::isMonsterUnit(const std::int32_t unitId, const std::uint16_t appearance,
                                    const char friendStatus) const {
    if (unitId == 0 || isNpcUnit(unitId, appearance, friendStatus)) {
        return false;
    }
    if (const auto it = remoteUnits_.find(unitId); it != remoteUnits_.end() && it->second.dying) {
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
    return appearance >= 20000 ||
           static_cast<unsigned char>(friendStatus) == static_cast<unsigned char>(kT4CVolCannotTalk);
}

bool GameWorldScreen::isAnimatedGloveGroundObject(const std::uint16_t appearance) const {
    if (isDoorLikeAppearance(appearance)) {
        return true;
    }
    if (T4CIsChestGroundAppearance(appearance)) {
        return false;
    }
    return appearance != 0;
}

bool GameWorldScreen::isUiPanelBlockingWorldCursor() const {
    return showInventory_ || chestList_.valid || shopList_.valid || hasNpcSpeech_;
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
        if (shopList_.valid || chestList_.valid || hasNpcSpeech_) {
            T4CLoginSessionSendBreakConversation();
            shopList_ = {};
            T4CLoginSessionCloseChest();
            chestList_ = {};
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

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.down) {
        switch (event.key.key) {
            case SDLK_F2:
                useBagItemByBaseId(40623);
                return true;
            case SDLK_F3:
                useBagItemByBaseId(40004);
                return true;
            case SDLK_F4:
                useBagItemByBaseId(40015);
                return true;
            default:
                break;
        }
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
            if (showInventory_ && invDrag_.active) {
                finishInventoryDrop(converted.button.x, converted.button.y);
                return true;
            }
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
                const T4CCharacterWindow::HitTestResult hit =
                    charWindow_.hitTest(kLogicalWidth, kLogicalHeight, mx, my, charTab_, equipment_,
                                        backpack_, skillBook_, spellBook_, selectedBagIndex_, invDragView());
                if (hit.kind == T4CCharacterWindow::HitKind::Close) {
                    showInventory_ = false;
                    selectedBagIndex_ = -1;
                    cancelInventoryDrag();
                    return true;
                }
                if (hit.kind == T4CCharacterWindow::HitKind::Tab) {
                    if (hit.tab != charTab_) {
                        charTab_ = hit.tab;
                        selectedBagIndex_ = -1;
                        cancelInventoryDrag();
                    }
                    return true;
                }
                if (charTab_ == T4CCharacterWindow::Tab::Inventory) {
                    if (hit.kind == T4CCharacterWindow::HitKind::BagCell && hit.hasItem) {
                        if (converted.button.clicks >= 2) {
                            const T4CBagItem &item =
                                backpack_.items[static_cast<size_t>(hit.index)];
                            T4CLoginSessionSendUseBagItem(item.objectId);
                            noteBagItemUsed(item);
                            refreshInventoryViews();
                            cancelInventoryDrag();
                        } else {
                            startInventoryDragFromBag(hit.index);
                        }
                        return true;
                    }
                    if (hit.kind == T4CCharacterWindow::HitKind::EquipSlot && hit.hasItem) {
                        startInventoryDragFromEquip(hit.slot);
                        return true;
                    }
                }
                if (hit.kind == T4CCharacterWindow::HitKind::Skill) {
                    if (skillBook_.valid && hit.index >= 0 &&
                        static_cast<size_t>(hit.index) < skillBook_.skills.size()) {
                        const T4CPlayerSkill &sk = skillBook_.skills[static_cast<size_t>(hit.index)];
                        if (sk.useMode == 1) {
                            T4CLoginSessionSendUseSkill(sk.skillId);
                            T4CLoginSessionPushSystemMessage("Vous utilisez " + sk.name + ".");
                        } else {
                            T4CLoginSessionPushSystemMessage(
                                sk.useMode == 0 ? "Cette competence ne s'utilise pas directement."
                                                : "Competence a cible : non supportee ici.");
                        }
                    }
                    return true;
                }
                if (hit.kind == T4CCharacterWindow::HitKind::Spell) {
                    if (castSpellAtIndex(hit.index)) {
                        showInventory_ = false;
                    }
                    return true;
                }
                if (hit.kind != T4CCharacterWindow::HitKind::Outside) {
                    return true;
                }
            }
            if (shopList_.valid && handleShopClick(mx, my)) {
                return true;
            }
            if (chestList_.valid && handleChestClick(mx, my)) {
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
                const char friendStatus = it != remoteUnits_.end() ? it->second.friendStatus : kT4CVolCannotTalk;
                if (it != remoteUnits_.end()) {
                    /* Nom reel (« Dark Fang », « Mithrand »…) pour l'etiquette de cible. */
                    T4CLoginSessionRequestUnitName(picked, it->second.serverX, it->second.serverY);
                }
                if (attackMode_) {
                    tryAttackUnit(picked);
                } else if (isNpcUnit(picked, appearance, friendStatus)) {
                    tryTalkUnit(picked);
                } else if (isMonsterUnit(picked, appearance, friendStatus)) {
                    tryAttackUnit(picked);
                }
            } else if (selectedGroundId_ != 0) {
                const auto it = groundObjects_.find(selectedGroundId_);
                if (it != groundObjects_.end()) {
                    if (isDoorLikeAppearance(it->second.appearance)) {
                        if (isMeleeRange(playerX_, playerY_, it->second.x, it->second.y)) {
                            T4CLoginSessionSendUseObject(it->second.x, it->second.y, selectedGroundId_);
                            T4CGameMusic::PlaySfx("Open Wooden Door");
                        } else {
                            pendingGround_.active = true;
                            pendingGround_.pickup = false;
                            pendingGround_.groundId = selectedGroundId_;
                            pendingGround_.x = it->second.x;
                            pendingGround_.y = it->second.y;
                            cancelPendingAttack();
                        }
                    } else if (T4CIsChestGroundAppearance(it->second.appearance)) {
                        T4CLoginSessionSendUseObject(it->second.x, it->second.y, selectedGroundId_);
                    } else {
                        tryPickupGroundObject(selectedGroundId_);
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

void GameWorldScreen::cancelPendingGroundAction() {
    pendingGround_.active = false;
    pendingGround_.pickup = false;
    pendingGround_.groundId = 0;
}

void GameWorldScreen::handleUnitLeftClick(const std::int32_t unitId, const std::uint16_t appearance,
                                          const char friendStatus) {
    if (unitId == 0 || playerDead_) {
        return;
    }
    /* Windows : mode attaque (Ctrl+C) → tout clic sur unite Type>10000 = Combat. */
    if (attackMode_) {
        tryAttackUnit(unitId);
        return;
    }
    if (isNpcUnit(unitId, appearance, friendStatus)) {
        tryTalkUnit(unitId);
        return;
    }
    if (isMonsterUnit(unitId, appearance, friendStatus)) {
        tryAttackUnit(unitId);
    }
}

bool GameWorldScreen::tryPickupGroundObject(const std::int32_t groundId) {
    if (groundId == 0 || playerDead_) {
        return false;
    }
    const auto it = groundObjects_.find(groundId);
    if (it == groundObjects_.end()) {
        return false;
    }
    const unsigned tx = it->second.x;
    const unsigned ty = it->second.y;
    if (isMeleeRange(playerX_, playerY_, tx, ty)) {
        cancelPendingGroundAction();
        T4CLoginSessionSendGetObject(tx, ty, groundId);
        T4CGameMusic::PlaySfx("Generic pickup item");
        scheduleNearItemsRefresh();
        T4CLoginSessionRequestViewBackpack();
        return true;
    }
    pendingGround_.active = true;
    pendingGround_.pickup = true;
    pendingGround_.groundId = groundId;
    pendingGround_.x = tx;
    pendingGround_.y = ty;
    cancelPendingAttack();
    return true;
}

void GameWorldScreen::updatePendingGroundAction() {
    if (!pendingGround_.active) {
        return;
    }
    const unsigned tx = pendingGround_.x;
    const unsigned ty = pendingGround_.y;
    if (isMeleeRange(playerX_, playerY_, tx, ty)) {
        const std::int32_t groundId = pendingGround_.groundId;
        const bool pickup = pendingGround_.pickup;
        cancelPendingGroundAction();
        if (pickup && groundId != 0) {
            T4CLoginSessionSendGetObject(tx, ty, groundId);
            T4CGameMusic::PlaySfx("Generic pickup item");
            scheduleNearItemsRefresh();
            T4CLoginSessionRequestViewBackpack();
        } else if (groundId != 0) {
            T4CLoginSessionSendUseObject(tx, ty, groundId);
        }
        return;
    }
    if (!canMove_) {
        return;
    }
    const std::uint16_t opcode = moveOpcodeToward(playerX_, playerY_, tx, ty);
    if (opcode != 0) {
        tryMovePlayer(opcode);
    }
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
    if (!pendingAttack_.active) {
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

    if (!canMove_) {
        return;
    }

    const std::uint16_t opcode = moveOpcodeToward(playerX_, playerY_, tx, ty);
    if (opcode != 0) {
        tryMovePlayer(opcode);
    }
}

bool GameWorldScreen::isRemoteUnitSelectable(const RemoteUnitVisual &unit, const std::int32_t unitId) const {
    if (unit.dying || unit.showCorpse) {
        return false;
    }
    if (T4CIsPuppetAppearance(unit.appearance)) {
        T4CPuppetDress dress{};
        if (!T4CLoginSessionGetRemotePuppetDress(unitId, &dress)) {
            return false;
        }
    }
    return true;
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
        if (!isRemoteUnitSelectable(entry.second, entry.first)) {
            continue;
        }
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
    const auto it = remoteUnits_.find(unitId);
    if (it == remoteUnits_.end() || !isRemoteUnitSelectable(it->second, unitId)) {
        return;
    }
    int direction = it->second.direction;
    int frame = 0;
    bool attacking = false;
    bool moving = false;
    attacking = it->second.attacking;
    moving = it->second.moving;
    frame = attacking ? it->second.attackAnimFrame
                      : (moving ? it->second.animFrame : it->second.idleAnimFrame);
    const bool idleStanding = !moving && !attacking;
    const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(direction);
    if (T4CIsPuppetAppearance(appearance)) {
        T4CPuppetDress dress{};
        if (!T4CLoginSessionGetRemotePuppetDress(unitId, &dress)) {
            return;
        }
        const bool female = appearance == 10012 || appearance == 15012;
        const int puppetFrame = (idleStanding && !attacking) ? 0 : frame;
        /* Contour cible : CL_WHITE comme Windows (VisualObjectList.cpp dwOutlineColor = CL_WHITE). */
        if (T4CPuppetTryDrawOutline(spriteAtlas_, renderer, dress, female, direction, puppetFrame, sx, sy, attacking,
                                    255, 255, 255)) {
            return;
        }
        if (tryDrawNpcOutline(spriteAtlas_, renderer, T4CPuppetFallbackSpriteName(dress, female), sx, sy, direction,
                              puppetFrame, attacking, idleStanding)) {
            return;
        }
        return;
    }
    const char *base = T4CRemoteUnitSpriteName(unitId, appearance);
    if (!base) {
        return;
    }
    if (tryDrawNpcOutline(spriteAtlas_, renderer, base, sx, sy, direction, frame, attacking, idleStanding)) {
        return;
    }
    const int frameIndex = (idleStanding && !attacking) ? 0 : frame;
    const std::string frameName = attacking ? T4CUnitAttackSpriteFrameName(base, view, frameIndex)
                                            : T4CUnitSpriteFrameName(base, view, frameIndex);
    if (!frameName.empty() &&
        spriteAtlas_.TryDrawSpriteOutline(renderer, frameName, sx, sy, 255, 255, 255, view.mirror)) {
        return;
    }
    (void)spriteAtlas_.TryDrawSpriteOutline(renderer, base, sx, sy, 255, 255, 255, view.mirror);
}

void GameWorldScreen::renderDialoguePanel(SDL_Renderer *renderer) const {
    if (!renderer || !hasNpcSpeech_) {
        return;
    }
    const float panelX = 20.f;
    const float panelY = static_cast<float>(kLogicalHeight) - 220.f;
    const float panelW = static_cast<float>(kLogicalWidth) - 40.f;
    const float panelH = 200.f;
    SDL_FRect panel{panelX, panelY, panelW, panelH};
    SDL_SetRenderDrawColor(renderer, 10, 20, 40, 230);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 110, 100, 70, 255);
    SDL_RenderRect(renderer, &panel);

    SDL_Rect clip{static_cast<int>(panel.x + 4.f), static_cast<int>(panel.y + 4.f),
                  static_cast<int>(panel.w - 8.f), static_cast<int>(panel.h - 8.f)};
    SDL_SetRenderClipRect(renderer, &clip);

    char title[128];
    std::snprintf(title, sizeof(title), "%s:",
                    npcSpeech_.speakerName.empty() ? "PNJ" : npcSpeech_.speakerName.c_str());
    drawHudText(renderer, title, panelX + 8.f, panelY + 8.f);
    drawWrappedHudText(renderer, npcSpeech_.text.c_str(), panelX + 8.f, panelY + 28.f, panelW - 16.f,
                       16.f, 7);

    float linkY = panelY + panelH - 8.f - static_cast<float>(npcSpeech_.links.size()) * 16.f;
    if (linkY < panelY + 28.f) {
        linkY = panelY + 28.f;
    }
    for (size_t i = 0; i < npcSpeech_.links.size() && i < 9; ++i) {
        char line[160];
        std::snprintf(line, sizeof(line), "%zu: [%s]", i + 1, npcSpeech_.links[i].c_str());
        drawHudText(renderer, line, panelX + 8.f, linkY);
        linkY += 16.f;
    }
    SDL_SetRenderClipRect(renderer, nullptr);
}

namespace {

constexpr float kShopPanelW = 384.f;
constexpr float kShopPanelH = 420.f;
constexpr float kShopPanelX = (static_cast<float>(GameWorldScreen::kLogicalWidth) - kShopPanelW) * 0.5f;
constexpr float kShopPanelY = 72.f;
constexpr SDL_FRect kShopPanel{kShopPanelX, kShopPanelY, kShopPanelW, kShopPanelH};
constexpr float kShopRowY = kShopPanel.y + 64.f;
constexpr float kShopRowH = 20.f;
constexpr int kShopMaxRows = 14;
constexpr SDL_FRect kShopClose{kShopPanel.x + kShopPanel.w - 26.f, kShopPanel.y + 6.f, 22.f, 18.f};

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
            hint = "Clic sur une ligne : acheter 1";
            break;
        case T4CShopMode::Sell:
            title = "Vente";
            hint = "Clic sur une ligne : vendre 1";
            break;
        case T4CShopMode::Train:
            title = "Entrainement";
            hint = "Clic : +1 point de compétence";
            break;
        case T4CShopMode::Learn:
            title = "Apprentissage";
            hint = "Clic : apprendre";
            break;
        default:
            break;
    }
    SDL_SetRenderDrawColor(renderer, 24, 24, 42, 245);
    SDL_RenderFillRect(renderer, &kShopPanel);
    SDL_SetRenderDrawColor(renderer, 140, 120, 70, 255);
    SDL_RenderRect(renderer, &kShopPanel);
    char header[128];
    if (shopList_.mode == T4CShopMode::Train || shopList_.mode == T4CShopMode::Learn) {
        std::snprintf(header, sizeof(header), "%s — points : %u", title,
                      static_cast<unsigned>(shopList_.skillPoints));
    } else {
        std::snprintf(header, sizeof(header), "%s — or : %u — %zu article(s)", title, shopList_.gold,
                      shopList_.items.size());
    }
    drawHudText(renderer, header, kShopPanel.x + 10.f, kShopPanel.y + 10.f);
    drawHudText(renderer, hint, kShopPanel.x + 10.f, kShopPanel.y + 28.f);
    drawHudText(renderer, "Objet", kShopPanel.x + 12.f, kShopPanel.y + 46.f);
    drawHudText(renderer, "Prix", kShopPanel.x + kShopPanel.w - 72.f, kShopPanel.y + 46.f);
    SDL_SetRenderDrawColor(renderer, 80, 72, 48, 255);
    SDL_FRect headerLine{kShopPanel.x + 8.f, kShopPanel.y + 58.f, kShopPanel.w - 16.f, 1.f};
    SDL_RenderFillRect(renderer, &headerLine);
    SDL_SetRenderDrawColor(renderer, 90, 40, 36, 255);
    SDL_RenderFillRect(renderer, &kShopClose);
    drawHudText(renderer, "X", kShopClose.x + 7.f, kShopClose.y + 3.f);

    if (shopList_.items.empty()) {
        drawHudText(renderer, "(liste vide — parlez au marchand)", kShopPanel.x + 12.f, kShopRowY + 8.f);
        return;
    }

    const bool hovered = mouseValid_;
    for (size_t i = 0; i < shopList_.items.size() && i < static_cast<size_t>(kShopMaxRows); ++i) {
        const T4CShopEntry &item = shopList_.items[i];
        const float ry = kShopRowY + static_cast<float>(i) * kShopRowH;
        SDL_FRect row{kShopPanel.x + 8.f, ry, kShopPanel.w - 16.f, kShopRowH - 2.f};
        if (i % 2 == 0) {
            SDL_SetRenderDrawColor(renderer, 34, 34, 56, 255);
            SDL_RenderFillRect(renderer, &row);
        }
        if (hovered && mouseX_ >= row.x && mouseX_ < row.x + row.w && mouseY_ >= row.y &&
            mouseY_ < row.y + row.h) {
            SDL_SetRenderDrawColor(renderer, 70, 66, 110, 255);
            SDL_RenderFillRect(renderer, &row);
        }
        char line[192];
        if (shopList_.mode == T4CShopMode::Train) {
            std::snprintf(line, sizeof(line), "%s", item.name.empty() ? "?" : item.name.c_str());
            drawHudText(renderer, line, row.x + 4.f, ry + 3.f);
            std::snprintf(line, sizeof(line), "%u (max %u)", item.price, item.maxQty);
            drawHudText(renderer, line, kShopPanel.x + kShopPanel.w - 88.f, ry + 3.f);
        } else if (shopList_.mode == T4CShopMode::Sell) {
            std::snprintf(line, sizeof(line), "%s x%u", item.name.empty() ? "?" : item.name.c_str(),
                          item.maxQty);
            drawHudText(renderer, line, row.x + 4.f, ry + 3.f);
            std::snprintf(line, sizeof(line), "%u or", item.price);
            drawHudText(renderer, line, kShopPanel.x + kShopPanel.w - 88.f, ry + 3.f);
        } else {
            std::snprintf(line, sizeof(line), "%s", item.name.empty() ? "?" : item.name.c_str());
            drawHudText(renderer, line, row.x + 4.f, ry + 3.f);
            std::snprintf(line, sizeof(line), "%u or", item.price);
            drawHudText(renderer, line, kShopPanel.x + kShopPanel.w - 88.f, ry + 3.f);
        }
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

void GameWorldScreen::renderChestPanel(SDL_Renderer *renderer) const {
    if (!renderer || !chestList_.valid) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, 28, 22, 18, 245);
    SDL_RenderFillRect(renderer, &kShopPanel);
    SDL_SetRenderDrawColor(renderer, 140, 100, 50, 255);
    SDL_RenderRect(renderer, &kShopPanel);
    char header[128];
    std::snprintf(header, sizeof(header), "Coffre — %zu objet(s)", chestList_.items.size());
    drawHudText(renderer, header, kShopPanel.x + 10.f, kShopPanel.y + 10.f);
    drawHudText(renderer, "Clic sur une ligne : prendre 1", kShopPanel.x + 10.f, kShopPanel.y + 28.f);
    drawHudText(renderer, "Objet", kShopPanel.x + 12.f, kShopPanel.y + 46.f);
    drawHudText(renderer, "Qté", kShopPanel.x + kShopPanel.w - 72.f, kShopPanel.y + 46.f);
    SDL_SetRenderDrawColor(renderer, 80, 72, 48, 255);
    SDL_FRect headerLine{kShopPanel.x + 8.f, kShopPanel.y + 58.f, kShopPanel.w - 16.f, 1.f};
    SDL_RenderFillRect(renderer, &headerLine);
    drawHudText(renderer, "X", kShopClose.x + 6.f, kShopClose.y + 1.f);
    if (chestList_.items.empty()) {
        drawHudText(renderer, "(coffre vide)", kShopPanel.x + 12.f, kShopRowY + 8.f);
        return;
    }
    for (size_t i = 0; i < chestList_.items.size() && i < static_cast<size_t>(kShopMaxRows); ++i) {
        const T4CChestItem &item = chestList_.items[i];
        const float ry = kShopRowY + static_cast<float>(i) * kShopRowH;
        SDL_FRect row{kShopPanel.x + 8.f, ry, kShopPanel.w - 16.f, kShopRowH - 2.f};
        if (i % 2 == 0) {
            SDL_SetRenderDrawColor(renderer, 40, 34, 28, 220);
            SDL_RenderFillRect(renderer, &row);
        }
        char line[160];
        std::snprintf(line, sizeof(line), "%s", item.name.empty() ? "?" : item.name.c_str());
        drawHudText(renderer, line, row.x + 4.f, ry + 3.f);
        std::snprintf(line, sizeof(line), "%u", static_cast<unsigned>(item.qty > 0 ? item.qty : 1u));
        drawHudText(renderer, line, kShopPanel.x + kShopPanel.w - 88.f, ry + 3.f);
    }
}

bool GameWorldScreen::handleChestClick(const float mx, const float my) {
    if (!chestList_.valid || !PointInRect(mx, my, kShopPanel)) {
        return false;
    }
    if (PointInRect(mx, my, kShopClose)) {
        T4CLoginSessionCloseChest();
        chestList_ = {};
        return true;
    }
    const int row = static_cast<int>((my - kShopRowY) / kShopRowH);
    if (my < kShopRowY || row < 0 || static_cast<size_t>(row) >= chestList_.items.size() ||
        row >= kShopMaxRows) {
        return true;
    }
    const T4CChestItem &item = chestList_.items[static_cast<size_t>(row)];
    const std::uint32_t qty = item.qty > 0 ? item.qty : 1u;
    T4CLoginSessionSendChestTakeItem(item.objectId, qty > 1 ? 1u : qty);
    T4CGameMusic::PlaySfx("Generic pickup item");
    T4CLoginSessionRequestViewBackpack();
    T4CLoginSessionRequestPlayerStatus();
    /* Windows V3_ChestDlg : envoi 108 seulement ; le serveur pousse l'objet sol via opcode 10004. */
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
    const SDL_FRect chatInput = T4CGameHud::chatInputRect(kLogicalWidth, kLogicalHeight);
    if (PointInRect(mx, my, chatInput)) {
        openChatInput();
        return true;
    }
    /* Slots macro 1-12 : objets par defaut F2-F4 (slots 1-3), sorts pour le reste. */
    for (int slot = 0; slot < 12; ++slot) {
        const SDL_FRect r = T4CGameHud::macroSlotRect(kLogicalWidth, kLogicalHeight, slot);
        if (!PointInRect(mx, my, r)) {
            continue;
        }
        for (int m = 0; m < T4CGameHud::kDefaultItemMacroCount; ++m) {
            if (T4CGameHud::kDefaultItemMacros[m].slot == slot) {
                useBagItemByBaseId(T4CGameHud::kDefaultItemMacros[m].baseId);
                return true;
            }
        }
        if (spellBook_.valid && static_cast<size_t>(slot) < spellBook_.spells.size()) {
            castSpellAtIndex(slot);
        }
        return true;
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
            triggerLocalCastAnim(it->second.serverX, it->second.serverY);
            T4CLoginSessionPushSystemMessage("Sort " + spell.name + " sur la cible.");
            return true;
        }
    }
    T4CActivePlayer active{};
    T4CLoginSessionGetActivePlayer(&active);
    triggerLocalCastAnim(active.serverX, active.serverY);
    T4CLoginSessionSendCastSpell(spell.spellId, 0, 0, 0);
    T4CLoginSessionPushSystemMessage("Sort " + spell.name + ".");
    return true;
}

void GameWorldScreen::triggerLocalCastAnim(const unsigned targetX, const unsigned targetY) {
    const Uint32 now = SDL_GetTicks();
    playerAttacking_ = true;
    playerAttackFrame_ = 0;
    playerAttackTick_ = now;
    playerAttackUntil_ = now + 420;
    const int ddx = static_cast<int>(targetX) - static_cast<int>(playerX_);
    const int ddy = static_cast<int>(targetY) - static_cast<int>(playerY_);
    if (ddx != 0 || ddy != 0) {
        playerDirection_ = T4CDirectionFromWorldDelta(ddx, ddy);
    }
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
    T4CTalkState talk{};
    T4CLoginSessionGetTalkState(&talk);
    if (talk.active && talk.unitId != 0) {
        if (T4CLoginSessionSendDirectedTalkMessage(line)) {
            return;
        }
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
                       playerStatus_, equipment_, backpack_, skillBook_, spellBook_, selectedBagIndex_,
                       invDragView());
    drawInventoryDragCursor(renderer);
}

const T4CEquippedItem *GameWorldScreen::findEquippedItem(const T4CEquipSlot slot) const {
    for (const T4CEquippedItem &it : equipment_.items) {
        if (it.slot == slot && it.appearance != 0) {
            return &it;
        }
    }
    return nullptr;
}

T4CCharacterWindow::InvDragView GameWorldScreen::invDragView() const {
    T4CCharacterWindow::InvDragView view{};
    if (invDrag_.active) {
        view.active = true;
        view.fromEquip = invDrag_.fromEquip;
        view.bagIndex = invDrag_.bagIndex;
        view.equipSlot = invDrag_.equipSlot;
    }
    return view;
}

void GameWorldScreen::startInventoryDragFromBag(const int index) {
    if (!backpack_.valid || index < 0 || static_cast<size_t>(index) >= backpack_.items.size()) {
        return;
    }
    const T4CBagItem &item = backpack_.items[static_cast<size_t>(index)];
    invDrag_ = {};
    invDrag_.active = true;
    invDrag_.bagIndex = index;
    invDrag_.objectId = item.objectId;
    invDrag_.appearance = item.appearance;
    invDrag_.qty = item.qty > 0 ? item.qty : 1u;
    selectedBagIndex_ = index;
    T4CGameMusic::PlaySfx("Equip");
}

void GameWorldScreen::startInventoryDragFromEquip(const T4CEquipSlot slot) {
    const T4CEquippedItem *item = findEquippedItem(slot);
    if (item == nullptr) {
        return;
    }
    invDrag_ = {};
    invDrag_.active = true;
    invDrag_.fromEquip = true;
    invDrag_.equipSlot = slot;
    invDrag_.objectId = item->objectId;
    invDrag_.appearance = item->appearance;
    invDrag_.qty = item->qty > 0 ? item->qty : 1u;
    selectedBagIndex_ = -1;
    T4CGameMusic::PlaySfx("Equip");
}

void GameWorldScreen::cancelInventoryDrag() {
    invDrag_ = {};
}

void GameWorldScreen::refreshInventoryViews() {
    T4CLoginSessionRequestViewBackpack();
    T4CLoginSessionRequestViewEquipped();
    T4CLoginSessionRequestPlayerStatus();
}

void GameWorldScreen::applyInventoryDragAction(const T4CCharacterWindow::Action action) {
    if (!invDrag_.active || invDrag_.objectId == 0) {
        return;
    }
    const std::uint32_t qty = invDrag_.qty > 0 ? invDrag_.qty : 1u;
    switch (action) {
        case T4CCharacterWindow::Action::EquipSelected:
            if (!invDrag_.fromEquip) {
                T4CLoginSessionSendEquipObject(invDrag_.objectId);
                T4CGameMusic::PlaySfx("Equip");
            }
            break;
        case T4CCharacterWindow::Action::UseSelected:
            T4CLoginSessionSendUseBagItem(invDrag_.objectId);
            if (!invDrag_.fromEquip && invDrag_.bagIndex >= 0 && backpack_.valid &&
                static_cast<size_t>(invDrag_.bagIndex) < backpack_.items.size()) {
                noteBagItemUsed(backpack_.items[static_cast<size_t>(invDrag_.bagIndex)]);
            }
            break;
        case T4CCharacterWindow::Action::DropSelected:
            T4CLoginSessionSendDropObject(invDrag_.objectId, qty);
            T4CGameMusic::PlaySfx("Generic drop item");
            scheduleNearItemsRefresh();
            break;
        case T4CCharacterWindow::Action::JunkSelected:
            T4CLoginSessionSendJunkItems(invDrag_.objectId, qty);
            T4CGameMusic::PlaySfx("Generic drop item");
            break;
        default:
            return;
    }
    refreshInventoryViews();
    cancelInventoryDrag();
    selectedBagIndex_ = -1;
}

void GameWorldScreen::finishInventoryDrop(const float mx, const float my) {
    if (!invDrag_.active) {
        return;
    }
    const T4CCharacterWindow::HitTestResult hit =
        charWindow_.hitTest(kLogicalWidth, kLogicalHeight, mx, my, charTab_, equipment_, backpack_,
                            skillBook_, spellBook_, selectedBagIndex_, invDragView());

    switch (hit.kind) {
        case T4CCharacterWindow::HitKind::EquipSlot:
            if (!invDrag_.fromEquip && invDrag_.objectId != 0) {
                T4CLoginSessionSendEquipObject(invDrag_.objectId);
                T4CGameMusic::PlaySfx("Equip");
                refreshInventoryViews();
            } else if (invDrag_.fromEquip) {
                T4CLoginSessionSendUnequipObject(invDrag_.equipSlot);
                T4CGameMusic::PlaySfx("Equip");
                refreshInventoryViews();
            }
            break;
        case T4CCharacterWindow::HitKind::BagCell:
            if (invDrag_.fromEquip) {
                T4CLoginSessionSendUnequipObject(invDrag_.equipSlot);
                T4CGameMusic::PlaySfx("Equip");
                refreshInventoryViews();
            }
            break;
        case T4CCharacterWindow::HitKind::ActionButton:
            applyInventoryDragAction(hit.action);
            return;
        case T4CCharacterWindow::HitKind::Outside:
            if (!invDrag_.fromEquip && invDrag_.objectId != 0) {
                T4CLoginSessionSendDropObject(invDrag_.objectId, invDrag_.qty > 0 ? invDrag_.qty : 1u);
                T4CGameMusic::PlaySfx("Generic drop item");
                refreshInventoryViews();
                scheduleNearItemsRefresh();
            }
            break;
        default:
            break;
    }
    cancelInventoryDrag();
}

void GameWorldScreen::drawInventoryDragCursor(SDL_Renderer *renderer) const {
    if (!renderer || !invDrag_.active || invDrag_.appearance == 0 || !mouseValid_) {
        return;
    }
    if (const char *sprite = T4CGroundObjectSpriteName(invDrag_.appearance); sprite != nullptr) {
        spriteAtlas_.TryDrawSpriteByName(renderer, sprite, mouseX_, mouseY_);
    }
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
    pollDeferredSpritePreload(6);

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
        canMove_ = false;
        moveServerTimeoutUntil_ = 0;
    }

    if (ready_ && !T4CLoginSessionIsWorldSessionReady()) {
        canMove_ = false;
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
        canMove_ = T4CLoginSessionIsWorldSessionReady();
        moveServerTimeoutUntil_ = 0;
    } else if (ready_ && !canMove_ && moveServerTimeoutUntil_ == 0 && !playerDead_ &&
               T4CLoginSessionIsWorldSessionReady()) {
        canMove_ = true;
    }

    T4CPlayerStatus updated{};
    if (T4CLoginSessionConsumePlayerStatusUpdate(&updated)) {
        playerStatus_ = updated;
    }

    updatePendingAttack();
    updatePendingGroundAction();

    {
        unsigned atx = 0;
        unsigned aty = 0;
        if (T4CLoginSessionConsumeLocalAttack(&atx, &aty)) {
            triggerLocalCastAnim(atx, aty);
            playerAttackUntil_ = SDL_GetTicks() + 70 * kT4CAttackAnimFrames;
        } else if (T4CLoginSessionConsumeLocalCast(&atx, &aty)) {
            triggerLocalCastAnim(atx, aty);
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
    } else if (!isPlayerWalkAnimActive()) {
        if (now - playerIdleAnimTick_ >= static_cast<Uint32>(kT4CIdleAnimTickMs)) {
            playerIdleAnimFrame_ = T4CWalkAnimNextIdleFrame(playerIdleAnimFrame_);
            playerIdleAnimTick_ = now;
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
    } else {
        T4CLoginSessionGetShopList(&shop);
        if (shop.valid) {
            shopList_ = shop;
        }
    }
    T4CChestList chest{};
    if (T4CLoginSessionConsumeChestList(&chest)) {
        chestList_ = std::move(chest);
    } else {
        T4CLoginSessionGetChestList(&chest);
        if (chest.valid) {
            chestList_ = chest;
        }
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

    if (playerMoving_ && now > playerAnimUntil_ && !isPlayerScrolling()) {
        playerMoving_ = false;
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
    const SDL_Color color{220, 220, 200, 255};
    if (uiFont_ && uiFont_->isReady()) {
        uiFont_->drawText(renderer, text, x, y, color);
        return;
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDebugText(renderer, x, y, text);
}

void GameWorldScreen::drawWrappedHudText(SDL_Renderer *renderer, const char *text, const float x,
                                         const float y, const float maxWidth, const float lineHeight,
                                         const int maxLines) const {
    if (!renderer || !text || maxWidth <= 0.f || lineHeight <= 0.f || maxLines <= 0) {
        return;
    }
    const SDL_Color color{220, 220, 200, 255};
    std::string line;
    float cursorX = x;
    float cursorY = y;
    int lines = 0;

    const auto flushLine = [&]() {
        if (line.empty()) {
            return;
        }
        if (uiFont_ && uiFont_->isReady()) {
            uiFont_->drawText(renderer, line.c_str(), cursorX, cursorY, color);
        } else {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderDebugText(renderer, cursorX, cursorY, line.c_str());
        }
        line.clear();
        cursorY += lineHeight;
        ++lines;
    };

    const auto lineWidth = [&](const std::string &value) -> float {
        if (uiFont_ && uiFont_->isReady()) {
            int w = 0;
            int h = 0;
            uiFont_->measureText(value.c_str(), &w, &h);
            return static_cast<float>(w);
        }
        return static_cast<float>(value.size()) * 8.f;
    };

    for (const unsigned char uch : std::string(text)) {
        const char ch = static_cast<char>(uch);
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            flushLine();
            if (lines >= maxLines) {
                break;
            }
            continue;
        }
        if (ch == ' ' && line.empty()) {
            continue;
        }
        const std::string trial = line + ch;
        if (!line.empty() && lineWidth(trial) > maxWidth) {
            flushLine();
            if (lines >= maxLines) {
                break;
            }
            if (ch != ' ') {
                line = ch;
            }
        } else {
            line = trial;
        }
    }
    if (lines < maxLines && !line.empty()) {
        flushLine();
    }
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
    /* Windows : g_DONE=8 pas scroll, 1 avance SpriteNumber/frame rendu (nbSprite4Move=1). */
    if (isPlayerWalkAnimActive() && (playerDisplayX_ != prevX || playerDisplayY_ != prevY)) {
        playerAnimFrame_ = T4CWalkAnimNextPuppetFrame(playerAnimFrame_);
    }
}

bool GameWorldScreen::isPlayerWalkAnimActive() const {
    return playerMoving_ || isPlayerScrolling();
}

void GameWorldScreen::updateRemoteUnitMotion(const Uint32 now) {
    std::vector<std::int32_t> expiredCorpses;
    for (auto &entry : remoteUnits_) {
        RemoteUnitVisual &unit = entry.second;
        if (unit.dying) {
            const Uint32 elapsed = now - unit.dieStartedAt;
            if (!unit.showCorpse) {
                if (elapsed >= static_cast<Uint32>(kT4CDyingPhaseMs)) {
                    unit.showCorpse = true;
                    unit.corpseFrame = 0;
                    unit.corpseAnimTick = now;
                }
            } else {
                if (now - unit.corpseAnimTick >= static_cast<Uint32>(kT4CCorpseAnimTickMs)) {
                    if (unit.corpseFrame < kT4CCorpseAnimMaxFrames - 1) {
                        unit.corpseFrame++;
                    }
                    unit.corpseAnimTick = now;
                }
                if (elapsed >= static_cast<Uint32>(kT4CDyingPhaseMs + kT4CCorpseLifetimeMs)) {
                    expiredCorpses.push_back(entry.first);
                }
            }
            continue;
        }
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
            if (now - unit.animTick >= static_cast<Uint32>(kT4CIdleAnimTickMs)) {
                unit.idleAnimFrame = T4CWalkAnimNextIdleFrame(unit.idleAnimFrame);
                unit.animTick = now;
            }
            continue;
        }
        unit.moving = true;
        const float dist = std::sqrt(distSq);
        const float step = std::min(dist, 0.18f);
        unit.displayX += dx / dist * step;
        unit.displayY += dy / dist * step;
        if (now - unit.animTick >= static_cast<Uint32>(kT4CWalkAnimTickMs)) {
            unit.animFrame = T4CWalkAnimNextNpcFrame(unit.animFrame);
            unit.animTick = now;
        }
    }
    for (const std::int32_t unitId : expiredCorpses) {
        remoteUnits_.erase(unitId);
        T4CLoginSessionFinalizeRemoteUnitDeath(unitId);
        if (selectedUnitId_ == unitId) {
            selectedUnitId_ = 0;
        }
        if (hoveredUnitId_ == unitId) {
            hoveredUnitId_ = 0;
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
                                     const int animFrame, const bool attacking, const bool idleStanding) const {
    if (!spritesLoaded_ || !renderer || appearance == 0) {
        return false;
    }

    if (T4CIsPuppetAppearance(appearance)) {
        T4CPuppetDress dress{};
        if (!T4CLoginSessionGetRemotePuppetDress(unitId, &dress)) {
            /* Windows (VisualObjectList.cpp:15734) : un puppet joueur dont l'equipement
               (RQ_PuppetInformation/68) n'est pas recu (KnownPuppet==false) n'est PAS dessine.
               On ne dessine donc PAS de "PNJ robe blanche" : invisible jusqu'a habillage. */
            return false;
        }
        const bool female = appearance == 10012 || appearance == 15012;
        /* Puppet a l'arret : pas de cycle idle (cf. joueur) — frame statique 0. */
        const int puppetFrame = (idleStanding && !attacking) ? 0 : animFrame;
        if (T4CPuppetTryDraw(spriteAtlas_, renderer, dress, female, direction, puppetFrame, screenX, screenY,
                             attacking)) {
            return true;
        }
        if (tryDrawNpcBase(spriteAtlas_, renderer, T4CPuppetFallbackSpriteName(dress, female), screenX, screenY,
                           direction, puppetFrame, attacking, idleStanding)) {
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
        const int standingFrame = (idleStanding && !attacking) ? 0 : animFrame;
        const std::string standing = T4CPlayerUnitSpriteName(probe, direction, standingFrame);
        if (!standing.empty()) {
            const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(direction);
            if (spriteAtlas_.TryDrawSpriteByName(renderer, standing, screenX, screenY, view.mirror)) {
                return true;
            }
        }
    }

    const char *npcBase = T4CRemoteUnitSpriteName(unitId, appearance);
    if (tryDrawNpcBase(spriteAtlas_, renderer, npcBase, screenX, screenY, direction, animFrame, attacking,
                       idleStanding)) {
        return true;
    }

    LogUnitDrawFailOnce(unitId, appearance, npcBase != nullptr ? "decode/echec sprite" : "appearance inconnue");
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

    const int frame =
        playerAttacking_ ? playerAttackFrame_ : (isPlayerWalkAnimActive() ? playerAnimFrame_ : playerIdleAnimFrame_);
    /* Windows : un puppet joueur n'a PAS de cycle idle — a l'arret SpriteNumber reste a 1
       (pose statique). Le rendu puppet indexe le cycle de MARCHE avec frameIndex, donc lui
       passer une frame idle qui defile ferait "marcher sur place". On force donc frame 0
       statique a l'arret pour le puppet (l'anim idle StMov ne vaut que pour les sprites NPC). */
    const int puppetFrame = playerAttacking_ ? frame : (isPlayerWalkAnimActive() ? playerAnimFrame_ : 0);
    if (T4CPuppetTryDrawPlayer(spriteAtlas_, renderer, active, playerDirection_, puppetFrame, screenX, screenY,
                               playerAttacking_)) {
        return true;
    }

    const T4CUnitSpriteView view = T4CUnitSpriteViewFromDirection(playerDirection_);
    if (playerAttacking_) {
        const char *base = T4CPlayerSpriteNpcName(active);
        const std::string attackName = T4CUnitAttackSpriteFrameName(base, view, playerAttackFrame_);
        if (!attackName.empty() &&
            spriteAtlas_.TryDrawSpriteByName(renderer, attackName, screenX, screenY, view.mirror)) {
            return true;
        }
    }
    const int drawFrame =
        playerAttacking_ ? playerAttackFrame_ : (isPlayerWalkAnimActive() ? playerAnimFrame_ : 0);
    const std::string spriteName = T4CPlayerUnitSpriteName(active, playerDirection_, drawFrame);
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
        const std::uint16_t appearance =
            entry.second.showCorpse ? entry.second.liveAppearance : entry.second.appearance;
        if (entry.first == selectedUnitId_) {
            selectionVisible = true;
            selSx = sx;
            selSy = sy;
            selAppearance = appearance;
        }
        bool drew = false;
        if (entry.second.showCorpse) {
            const char *base = T4CSpriteNameFromAppearance(entry.second.liveAppearance);
            drew = tryDrawCorpse(spriteAtlas_, renderer, base, sx, sy, entry.second.corpseFrame);
        } else {
            const int frame = entry.second.attacking
                                  ? entry.second.attackAnimFrame
                                  : (entry.second.moving ? entry.second.animFrame : entry.second.idleAnimFrame);
            const bool idleStanding = !entry.second.moving && !entry.second.attacking;
            drew = drawUnitSprite(renderer, sx, sy, entry.first, appearance, entry.second.direction, frame,
                                  entry.second.attacking, idleStanding);
        }
        if (!drew && !entry.second.showCorpse && !entry.second.dying) {
            /* Pas de point pour un puppet sans equipement (invisible facon Windows). */
            T4CPuppetDress probeDress{};
            const bool undressedPuppet =
                T4CIsPuppetAppearance(appearance) &&
                !T4CLoginSessionGetRemotePuppetDress(entry.first, &probeDress);
            if (!undressedPuppet) {
                SDL_FRect dot{sx - 4.f, sy - 4.f, 8.f, 8.f};
                SDL_RenderFillRect(renderer, &dot);
            }
        }
    }

    // Encadre + libelle de l'unite selectionnee (V3_TargetDlg simplifie).
    if (selectionVisible) {
        if (const auto selIt = remoteUnits_.find(selectedUnitId_);
            selIt != remoteUnits_.end() && isRemoteUnitSelectable(selIt->second, selectedUnitId_)) {
            drawUnitOutline(renderer, selSx, selSy, selectedUnitId_, selAppearance);
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
    renderChestPanel(renderer);
    renderChatLog(renderer);
    renderInventoryPanel(renderer);
    renderGameMenu(renderer);
    renderReturnConfirm(renderer);

    if (mouseValid_ && spritesLoaded_) {
        if (isUiPanelBlockingWorldCursor()) {
            /* Inventaire / coffre / boutique : gant fixe decale a droite de la cible (GameUI Windows). */
            spriteAtlas_.TryDrawSpriteByName(renderer, "glove0000", mouseX_ + 24.f, mouseY_);
        } else if (leftMouseHeld_ && holdMoveDirection_ >= 1 && holdMoveDirection_ <= 8) {
            if (const char *cursor = kHoldMoveCursorSprite(holdMoveDirection_); cursor != nullptr) {
                spriteAtlas_.TryDrawSpriteByName(renderer, cursor, mouseX_, mouseY_);
            }
        } else {
            bool overEnemy = false;
            bool overNpc = false;
            if (hoveredUnitId_ != 0) {
                const auto hovIt = remoteUnits_.find(hoveredUnitId_);
                if (hovIt != remoteUnits_.end()) {
                    const std::uint16_t app = hovIt->second.appearance;
                    const char friendStatus = hovIt->second.friendStatus;
                    T4CActivePlayer me{};
                    T4CLoginSessionGetActivePlayer(&me);
                    const bool isSelf = me.valid && me.unitId != 0 && hoveredUnitId_ == me.unitId;
                    const bool isCorpse = app >= 15001 && app <= 15012;
                    if (!isSelf && !isCorpse) {
                        if (attackMode_) {
                            overEnemy = true;
                        } else if (isNpcUnit(hoveredUnitId_, app, friendStatus)) {
                            overNpc = true;
                        } else if (isMonsterUnit(hoveredUnitId_, app, friendStatus)) {
                            overEnemy = true;
                        }
                    }
                }
            }
            if (overEnemy) {
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
            } else if (hoveredGroundId_ != 0) {
                const auto gIt = groundObjects_.find(hoveredGroundId_);
                const std::uint16_t gApp =
                    gIt != groundObjects_.end() ? gIt->second.appearance : static_cast<std::uint16_t>(0);
                if (isAnimatedGloveGroundObject(gApp)) {
                    tryDrawAnimatedGloveCursor(spriteAtlas_, renderer, mouseX_, mouseY_);
                } else {
                    spriteAtlas_.TryDrawSpriteByName(renderer, "glove0000", mouseX_, mouseY_);
                }
            } else {
                spriteAtlas_.TryDrawSpriteByName(renderer, "glove0000", mouseX_, mouseY_);
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
