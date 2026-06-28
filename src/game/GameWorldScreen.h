#pragma once

#include <SDL3/SDL.h>

#include "assets/map/T4CTmiWorldMap.h"
#include "assets/map/T4CZoneMap.h"
#include "assets/map/T4CV2WorldMap.h"
#include "assets/map/T4CV2SpriteAtlas.h"
#include "assets/map/T4CMapObjectSprites.h"
#include "game/T4CGameHud.h"
#include "game/T4CCharacterWindow.h"
#include "network/T4CLoginSession.h"
#include "network/T4CNpcDialog.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class LauncherChrome;

/** Vue monde minimale SDL3 apres PutPlayerInGame (sans moteur TnC). */
class GameWorldScreen {
   public:
    static constexpr int kLogicalWidth = 1280;
    static constexpr int kLogicalHeight = 720;
    static constexpr int kTileSize = 32;
    static constexpr int kIsoTileW = T4CV2WorldMap::kIsoTileWidth;
    static constexpr int kIsoTileH = T4CV2WorldMap::kIsoTileHeight;

    GameWorldScreen() = default;

    bool Init(SDL_Renderer *renderer, SDL_Window *window, unsigned int spawnX, unsigned int spawnY,
              unsigned short world);

    /** Ouvre cartes + prepare preload sprites (non bloquant). */
    bool BeginLoad(SDL_Renderer *renderer, SDL_Window *window, unsigned int spawnX, unsigned int spawnY,
                   unsigned short world);

    /** Decode sprites avec budget temps (ms) ; retourne true quand le monde est pret. */
    bool PollLoadForMs(int maxMs = 12);

    float GetLoadProgress() const { return loadProgress_; }

    bool IsLoading() const { return loading_; }

    void RenderLoading(SDL_Renderer *renderer, LauncherChrome *chrome) const;

    void Shutdown();

    bool IsReady() const { return ready_; }

    /** Zone musique monde (spawn) — disponible une fois IsReady(). */
    std::uint16_t GetWorldId() const { return world_; }
    unsigned int GetSpawnX() const { return playerX_; }
    unsigned int GetSpawnY() const { return playerY_; }

    const std::string &GetLastError() const { return lastError_; }

    /** false = quitter l'application (SDL_EVENT_QUIT). */
    bool HandleEvent(const SDL_Event &event);

    bool ConsumeReturnToLogin();

    bool ConsumeQuitApp();

    void Update();

    void Render(SDL_Renderer *renderer, LauncherChrome *chrome);

   private:
    static constexpr Uint32 kMoveCooldownMs = 100;
    static constexpr Uint32 kMoveServerTimeoutMs = 450;
    static constexpr std::uint16_t kObjGroupPortal = 297;

    static bool moveDeltaFromOpcode(std::uint16_t opcode, int &dx, int &dy);
    static std::uint16_t moveOpcodeFromArrows(bool left, bool right, bool up, bool down);
    static std::uint16_t moveOpcodeFromKey(const SDL_Event &event);
    static std::uint16_t moveOpcodeToward(unsigned int fromX, unsigned int fromY, unsigned int toX,
                                          unsigned int toY);
    static bool isMeleeRange(unsigned int ax, unsigned int ay, unsigned int bx, unsigned int by);

    bool tryMovePlayer(std::uint16_t moveOpcode);
    void applyLocalMoveDelta(int dx, int dy);
    std::int32_t pickUnitAtScreen(float screenX, float screenY) const;
    std::int32_t pickGroundObjectAtScreen(float screenX, float screenY) const;
    bool screenToWorldTile(float screenX, float screenY, unsigned int &outX, unsigned int &outY) const;
    bool isMapTileBlocking(unsigned int worldX, unsigned int worldY) const;
    bool tryUseAtScreen(float screenX, float screenY);
    bool tryAttackUnit(std::int32_t unitId);
    bool tryTalkUnit(std::int32_t unitId);
    void handleUnitLeftClick(std::int32_t unitId, std::uint16_t appearance, char friendStatus);
    bool tryPickupGroundObject(std::int32_t groundId);
    void updatePendingGroundAction();
    void cancelPendingGroundAction();
    bool isNpcUnit(std::int32_t unitId, std::uint16_t appearance, char friendStatus) const;
    bool isMonsterUnit(std::int32_t unitId, std::uint16_t appearance, char friendStatus) const;
    bool isAnimatedGloveGroundObject(std::uint16_t appearance) const;
    bool isUiPanelBlockingWorldCursor() const;
    /** Effets locaux d'objets utilises (torche → eclairage). */
    void noteBagItemUsed(const T4CBagItem &item);
    bool useBagItemByBaseId(std::uint16_t baseId);
    void updatePendingAttack();
    void cancelPendingAttack();
    void drawTargetHpBar(SDL_Renderer *renderer, float sx, float sy, int hpPercent) const;
    void drawUnitOutline(SDL_Renderer *renderer, float sx, float sy, std::int32_t unitId,
                         std::uint16_t appearance) const;
    void renderDialoguePanel(SDL_Renderer *renderer) const;
    void renderShopPanel(SDL_Renderer *renderer) const;
    void renderChestPanel(SDL_Renderer *renderer) const;
    void renderInventoryPanel(SDL_Renderer *renderer) const;
    void renderChatLog(SDL_Renderer *renderer) const;
    void renderOverheadSpeech(SDL_Renderer *renderer);
    bool handleShopClick(float mx, float my);
    bool handleChestClick(float mx, float my);
    bool handleChatEvent(const SDL_Event &event);
    void openChatInput();
    void closeChatInput();
    void sendChatLine(const std::string &line);
    void pumpChatMessages();
    bool handleHudClick(float mx, float my);
    bool castSpellAtIndex(int index);
    void triggerLocalCastAnim(unsigned targetX, unsigned targetY);
    void renderNightOverlay(SDL_Renderer *renderer);
    void renderGameMenu(SDL_Renderer *renderer) const;
    void renderReturnConfirm(SDL_Renderer *renderer) const;
    bool handleGameMenuClick(float mx, float my);
    bool handleReturnConfirmClick(float mx, float my);
    void refreshZoneMusic(bool forceRestart = false);
    void drawGroundObjectMarker(SDL_Renderer *renderer, float sx, float sy, std::uint16_t appearance) const;
    void syncGroundObjectsFromNetwork();
    void applyServerPlayerMove(unsigned int x, unsigned int y, std::uint16_t moveOpcode);
    void syncRemoteUnitsFromNetwork();
    void preloadUnitSpritesForAppearance(std::uint16_t appearance);
    void drawHudText(SDL_Renderer *renderer, const char *text, float x, float y) const;
    void drawWrappedHudText(SDL_Renderer *renderer, const char *text, float x, float y, float maxWidth,
                            float lineHeight, int maxLines) const;
    void drawFilledCircle(SDL_Renderer *renderer, float cx, float cy, float radius) const;
    bool drawUnitSprite(SDL_Renderer *renderer, float screenX, float screenY, std::int32_t unitId,
                        std::uint16_t appearance, int direction, int animFrame, bool attacking,
                        bool idleStanding) const;
    bool drawPlayerSprite(SDL_Renderer *renderer, float screenX, float screenY) const;
    float worldToScreenX(unsigned int worldX) const;
    float worldToScreenY(unsigned int worldY) const;
    float worldToScreenX(float worldX, float worldY) const;
    float worldToScreenY(float worldX, float worldY) const;
    void updateRemoteUnitMotion(Uint32 now);
    void updatePlayerScroll();
    bool isPlayerWalkAnimActive() const;
    bool isPlayerScrolling() const;
    void snapPlayerViewToServer();
    int isoViewRadius() const;
    void finishAssetLoad();
    void completeSessionEnter();
    void tryCompleteSessionEnter();
    void pollDeferredSpritePreload(int maxMs);

    SDL_Renderer *renderer_{nullptr};
    SDL_Window *window_{nullptr};

    unsigned int playerX_{0};
    unsigned int playerY_{0};
    float playerDisplayX_{0.f};
    float playerDisplayY_{0.f};
    unsigned short world_{0};

    bool ready_{false};
    bool returnToLogin_{false};
    bool quitApp_{false};
    bool canMove_{true};
    Uint32 moveCooldownUntil_{0};

    int playerDirection_{1};
    int playerAnimFrame_{0};
    int playerIdleAnimFrame_{0};
    Uint32 playerIdleAnimTick_{0};
    bool playerMoving_{false};
    /* Animation de swing du joueur (10001/10002 avec soi comme attaquant). */
    bool playerAttacking_{false};
    int playerAttackFrame_{0};
    Uint32 playerAttackTick_{0};
    Uint32 playerAttackUntil_{0};
    Uint32 playerAnimUntil_{0};
    Uint32 playerAnimTick_{0};

    struct RemoteUnitVisual {
        unsigned int serverX{0};
        unsigned int serverY{0};
        float displayX{0.f};
        float displayY{0.f};
        std::uint16_t appearance{0};
        char hpPercent{100};
        char friendStatus{kT4CVolCannotTalk};
        int direction{2};
        int animFrame{0};
        int idleAnimFrame{0};
        bool moving{false};
        bool initialized{false};
        Uint32 animTick{0};
        bool attacking{false};
        int attackAnimFrame{0};
        Uint32 attackAnimTick{0};
        Uint32 attackUntil{0};
        /* Mort creature (opcode 12, ChangeType Windows) : swing ~500 ms puis cadavre ~5 s. */
        bool dying{false};
        bool showCorpse{false};
        std::uint16_t liveAppearance{0};
        std::uint16_t corpseAppearance{0};
        Uint32 dieStartedAt{0};
        int corpseFrame{0};
        Uint32 corpseAnimTick{0};
    };

    bool isRemoteUnitSelectable(const RemoteUnitVisual &unit, std::int32_t unitId) const;

    struct PendingAttack {
        bool active{false};
        std::int32_t unitId{0};
        /* true = on marche vers le PNJ pour lui PARLER (serveur exige dist^2 < 120). */
        bool talk{false};
    };

    struct PendingGroundAction {
        bool active{false};
        bool pickup{false};
        std::int32_t groundId{0};
        unsigned int x{0};
        unsigned int y{0};
    };

    PendingGroundAction pendingGround_{};
    std::unordered_map<std::int32_t, RemoteUnitVisual> remoteUnits_;
    std::unordered_map<std::int32_t, T4CGroundObjectMarker> groundObjects_;
    std::int32_t selectedUnitId_{0};
    std::int32_t selectedGroundId_{0};
    std::int32_t hoveredUnitId_{0};
    std::int32_t hoveredGroundId_{0};
    /* Etiquette nom affichee au clic droit sur un objet/unite (examine). */
    std::string examineLabel_{};
    float examineLabelWorldX_{0.f};
    float examineLabelWorldY_{0.f};
    Uint32 examineLabelUntil_{0};
    bool playerDead_{false};
    /* Mode attaque (Ctrl+C, opcode 198) : clic = attaque, PNJ neutres inclus. */
    bool attackMode_{false};
    unsigned int backpackItemCount_{0};
    bool equipmentKnown_{false};
    T4CNpcSpeech npcSpeech_{};
    bool hasNpcSpeech_{false};
    T4CShopList shopList_{};
    T4CChestList chestList_{};
    bool showInventory_{false};
    bool showGameMenu_{false};
    bool confirmReturnToLogin_{false};
    float mouseX_{0.f};
    float mouseY_{0.f};
    bool mouseValid_{false};
    bool leftMouseHeld_{false};
    int holdMoveDirection_{0};
    unsigned int holdMoveTargetX_{0};
    unsigned int holdMoveTargetY_{0};
    T4CPlayerBackpack backpack_{};
    T4CPlayerEquipment equipment_{};
    T4CPlayerSkillBook skillBook_{};
    T4CPlayerSpellBook spellBook_{};
    T4CCharacterWindow charWindow_{};
    T4CCharacterWindow::Tab charTab_{T4CCharacterWindow::Tab::Inventory};
    struct FloatingDamageVisual {
        std::int32_t unitId{0};
        std::string text;
        Uint32 expireAt{0};
        Uint32 spawnAt{0};
        std::uint32_t color{0xFFFFFFu};
    };
    std::vector<FloatingDamageVisual> floatingDamage_;

    /* Chat (V3_ChatLogDlg + saisie MainBar simplifiees). */
    struct ChatLine {
        std::string text;
        std::uint32_t color{0xFFFFFFu};
        Uint32 recvAt{0};
    };
    std::vector<ChatLine> chatLog_;
    bool chatInputActive_{false};
    std::string chatInput_;

    /* Bulles overhead (paroles au-dessus des unites, ~5 s comme Windows). */
    struct OverheadSpeech {
        std::string text;
        Uint32 until{0};
    };
    std::unordered_map<std::int32_t, OverheadSpeech> overheadSpeech_;

    int selectedBagIndex_{-1};

    /** Drag inventaire (V3_InvDlg : clic = prendre l'objet dans le curseur). */
    struct InventoryDrag {
        bool active{false};
        bool fromEquip{false};
        int bagIndex{-1};
        T4CEquipSlot equipSlot{T4CEquipSlot::Body};
        std::int32_t objectId{0};
        std::uint16_t appearance{0};
        std::uint32_t qty{0};
    };
    InventoryDrag invDrag_{};
    void startInventoryDragFromBag(int index);
    void startInventoryDragFromEquip(T4CEquipSlot slot);
    void cancelInventoryDrag();
    void finishInventoryDrop(float mx, float my);
    void applyInventoryDragAction(T4CCharacterWindow::Action action);
    void refreshInventoryViews();
    const T4CEquippedItem *findEquippedItem(T4CEquipSlot slot) const;
    T4CCharacterWindow::InvDragView invDragView() const;
    void drawInventoryDragCursor(SDL_Renderer *renderer) const;

    /* Jour/nuit (opcode 45) : voile nocturne + halo lumiere joueur (LightMap simplifie). */
    SDL_Texture *nightHaloTexture_{nullptr};
    Uint32 nextTimeRequestAt_{0};
    /* Torche allumee (double-clic sur la torche du sac) : halo plus large la nuit/donjon. */
    bool torchLit_{false};

    /* Zones nommees (Zone_Map.dat) : bandeau « Vous entrez dans... ». */
    T4CZoneMap zoneMap_;
    int lastZoneId_{-1};
    std::string zoneBanner_;
    Uint32 zoneBannerUntil_{0};
    void updateZoneBanner();
    PendingAttack pendingAttack_{};
    Uint32 moveServerTimeoutUntil_{0};
    /** Apres un move : GetNearItems (rayon 80) une fois a l'arret — peripheral (16) = bande directionnelle seulement. */
    Uint32 nearItemsIdleRefreshAt_{0};
    void scheduleNearItemsRefresh();
    static int moveCursorDirectionFromScreen(float screenX, float screenY);
    void updateHoldMoveCursor(float screenX, float screenY);
    void beginHoldMove(float screenX, float screenY);
    void endHoldMove();

    T4CV2WorldMap v2Map_;
    bool v2Loaded_{false};
    T4CV2SpriteAtlas spriteAtlas_;
    bool spritesLoaded_{false};
    T4CTmiWorldMap tmiMap_;
    bool tmiLoaded_{false};
    int viewTilesW_{40};
    int viewTilesH_{22};
    float mapOriginX_{0.f};
    float mapOriginY_{0.f};

    bool loading_{false};
    std::vector<std::string> preloadQueue_;
    int preloadIndex_{0};
    std::vector<std::string> deferredPreloadQueue_;
    int deferredPreloadIndex_{0};
    float loadProgress_{0.f};

    T4CGameHud gameHud_{};
    T4CPlayerStatus playerStatus_{};
    const T4CUiFont *uiFont_{nullptr};

    std::string lastError_;
};
