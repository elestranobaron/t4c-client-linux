#pragma once



#include <SDL3/SDL.h>



#include <cstdint>

#include <string>

#include <unordered_map>

#include <vector>



class T4CTmiWorldMap;

class T4CV2SpriteAtlas;



/** Lecteur chunks v2_*map.map1 (sol) + map2 (objets) — Phase 2b/2d. */

class T4CV2WorldMap {

   public:

    static constexpr int kWorldSize = 3072;

    static constexpr int kChunkSize = 128;

    static constexpr int kChunksPerAxis = kWorldSize / kChunkSize;  // 24

    static constexpr int kCompressedChunkBytes = 128 * 128;         // VirtualWidth*VirtualHeight/2

    static constexpr int kTilesPerChunk = kChunkSize * kChunkSize;



    static constexpr int kIsoTileWidth = 32;

    static constexpr int kIsoTileHeight = 16;

    /*
     * Decalage pour centrer la cellule du joueur a l'ecran. Projection
     * rectangulaire : on ancre au coin haut-gauche de la cellule (sprite sol
     * 32x16), donc on recentre d'une demi-tuile.
     */
    static constexpr float kIsoAnchorBiasX = -static_cast<float>(kIsoTileWidth) / 2.f;

    static constexpr float kIsoAnchorBiasY = -static_cast<float>(kIsoTileHeight) / 2.f;



    T4CV2WorldMap() = default;



    bool OpenWorld(unsigned short worldIndex);

    void Clear();



    bool IsOpen() const { return open_; }



    bool HasObjectLayer() const { return objectLayerOpen_; }



    /** ID tuile sol (map1) a coordonnees monde ; 0 si hors limites / indisponible. */

    std::uint16_t GetFloorTileId(unsigned int worldX, unsigned int worldY) const;



    /** ID objet decor (map2) ; 0 si absent. */

    std::uint16_t GetObjectTileId(unsigned int worldX, unsigned int worldY) const;



    /** Dessine la vue isometrique (sprites V2 si atlas, sinon couleurs TMI). */

    void RenderIsoViewport(SDL_Renderer *renderer, float centerX, float centerY, float screenCenterX,

                           float screenCenterY, int viewRadiusTiles, const T4CTmiWorldMap *tmiColors,

                           const T4CV2SpriteAtlas *sprites) const;



    std::string GetLastError() const { return lastError_; }



   private:

    struct ChunkKey {

        std::uint16_t cx;

        std::uint16_t cy;



        bool operator==(const ChunkKey &o) const { return cx == o.cx && cy == o.cy; }

    };



    struct ChunkKeyHash {

        std::size_t operator()(const ChunkKey &k) const {

            return (static_cast<std::size_t>(k.cy) << 16) | k.cx;

        }

    };



    static const char *MapBaseName(unsigned short worldIndex);

    static void TileIdToRgb(std::uint16_t tileId, std::uint8_t &r, std::uint8_t &g, std::uint8_t &b);



    bool loadChunkFromPath(const std::string &path, const std::vector<std::uint32_t> &offsets, std::uint16_t chunkX,

                           std::uint16_t chunkY, std::vector<std::uint16_t> &out) const;

    bool loadFloorChunk(std::uint16_t chunkX, std::uint16_t chunkY, std::vector<std::uint16_t> &out) const;

    bool loadObjectChunk(std::uint16_t chunkX, std::uint16_t chunkY, std::vector<std::uint16_t> &out) const;

    const std::vector<std::uint16_t> *getCachedFloorChunk(std::uint16_t chunkX, std::uint16_t chunkY) const;

    const std::vector<std::uint16_t> *getCachedObjectChunk(std::uint16_t chunkX, std::uint16_t chunkY) const;



    void drawIsoDiamond(SDL_Renderer *renderer, float cx, float cy, std::uint8_t r, std::uint8_t g,

                        std::uint8_t b) const;



    bool open_{false};

    bool objectLayerOpen_{false};

    unsigned short worldIndex_{0};

    std::string mapPath_;

    std::string map2Path_;

    std::vector<std::uint32_t> chunkOffsets_;

    std::vector<std::uint32_t> objectChunkOffsets_;

    mutable std::unordered_map<ChunkKey, std::vector<std::uint16_t>, ChunkKeyHash> chunkCache_;

    mutable std::unordered_map<ChunkKey, std::vector<std::uint16_t>, ChunkKeyHash> objectChunkCache_;

    mutable std::string lastError_;

};


