#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class T4CV2WorldMap;

/** Chargeur sprites V2 (.did/.dda) → SDL_Texture — Phase 2c. */
class T4CV2SpriteAtlas {
   public:
    T4CV2SpriteAtlas() = default;
    ~T4CV2SpriteAtlas();

    T4CV2SpriteAtlas(const T4CV2SpriteAtlas &) = delete;
    T4CV2SpriteAtlas &operator=(const T4CV2SpriteAtlas &) = delete;

    bool Init(SDL_Renderer *renderer);
    void Clear();

    bool IsReady() const { return ready_; }

    /** Reinitialise le budget de decodes pour cette frame (tuiles hors cache). */
    void BeginRenderFrame() const;

    bool TryDrawTile(SDL_Renderer *renderer, std::uint16_t tileId, unsigned int worldX, unsigned int worldY,
                     float screenX, float screenY) const;

    bool TryDrawMapObject(SDL_Renderer *renderer, std::uint16_t objectId, unsigned int worldX, unsigned int worldY,
                          float screenX, float screenY) const;

    bool TryDrawSpriteByName(SDL_Renderer *renderer, const std::string &spriteName, float screenX, float screenY,
                             bool flipHorizontal = false, int paletteVariant = -1) const;

    /** Contour 1px autour du masque alpha (FX_OUTLINE Windows). */
    bool TryDrawSpriteOutline(SDL_Renderer *renderer, const std::string &spriteName, float screenX, float screenY,
                              std::uint8_t r = 255, std::uint8_t g = 255, std::uint8_t b = 255,
                              bool flipHorizontal = false, int paletteVariant = -1) const;

    /** Noms uniques de sprites sol visibles autour du spawn (preload entree monde). */
    static void CollectViewportSpriteNames(const T4CV2WorldMap &map, unsigned int centerX, unsigned int centerY,
                                           int viewRadiusTiles, std::vector<std::string> &out);

    /** Pre-decode jusqu'a maxCount sprites (ignore le budget frame). Retourne le nombre traite. */
    int PreloadSprites(const std::vector<std::string> &names, int startIndex, int maxCount) const;

    /** Charge en RAM les banques .dda requises par la liste de noms. */
    void PreloadBanksForNames(const std::vector<std::string> &names) const;

    std::string GetLastError() const { return lastError_; }

   private:
    static constexpr int kNbFilesOri = 20;
    static constexpr int kNbFilesNms = 20;
    static constexpr int kTotalBanks = kNbFilesOri + kNbFilesNms;
    static constexpr int kDecodeBudgetPerFrame = 2048;

    struct DidEntry {
        std::string name;
        std::uint32_t fileOffset{0};
        std::uint32_t dataFileIndex{0};
    };

    struct DecodedSprite {
        int width{0};
        int height{0};
        int offX{0};
        int offY{0};
        int offX2{0};
        int offY2{0};
        std::uint8_t transIndex{0};
        /* ushTransparency du header : si !=0, DirectDraw color-key la VALEUR RGB de transIndex. */
        bool colorKeyed{false};
        std::vector<std::uint8_t> indices;
        std::vector<std::uint8_t> shadowMask;
        int stride{0};
    };

    struct CachedSprite {
        SDL_Texture *tex{nullptr};
        SDL_Texture *outlineTex{nullptr};
        int offX{0};
        int offY{0};
        int offX2{0};
        int offY2{0};
    };

    struct PaletteEntry {
        std::string id;
        std::array<std::uint8_t, 256 * 3> rgb{};
    };

    bool loadDidFile(const char *path, bool nmsBank);
    int appendPalettesFromFile(const char *path);
    bool ensureBankLoaded(std::uint32_t bankIndex) const;
    const DidEntry *findEntry(const std::string &name) const;
    bool decodeSprite(const DidEntry &entry, DecodedSprite &out) const;
    const std::uint8_t *paletteForSprite(const std::string &spriteName, int paletteVariant = -1) const;
    const CachedSprite *getOrCreateSprite(const std::string &spriteName) const;
    const CachedSprite *decodeAndCache(const std::string &spriteName, bool respectBudget,
                                       int paletteVariant = -1) const;
    static std::string NormalizeDidKey(const std::string &name);
    SDL_Texture *createTextureFromDecoded(const DecodedSprite &sprite, const std::uint8_t *paletteRgb,
                                          const DecodedSprite *alphaMask = nullptr) const;
    SDL_Texture *createOutlineTextureFromDecoded(const DecodedSprite &sprite,
                                                 const std::uint8_t *paletteRgb) const;
    static std::string IsoSpriteName(std::uint16_t tileId, unsigned int worldX, unsigned int worldY);
    static const char *TileSpriteBase(std::uint16_t tileId);

    SDL_Renderer *renderer_{nullptr};
    bool ready_{false};
    std::vector<DidEntry> didEntries_;
    std::unordered_map<std::string, std::size_t> didByName_;
    mutable std::vector<std::vector<std::uint8_t>> ddaBanks_;
    mutable std::vector<bool> ddaBankMissing_;
    std::vector<PaletteEntry> palettes_;
    std::array<std::uint8_t, 256 * 3> defaultPalette_{};
    bool hasDefaultPalette_{false};
    mutable std::unordered_map<std::string, CachedSprite> spriteCache_;
    mutable int decodeBudget_{kDecodeBudgetPerFrame};
    std::string lastError_;
};
