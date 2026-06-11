#include "assets/map/T4CV2WorldMap.h"

#include "assets/map/T4CMapObjectSprites.h"
#include "assets/map/T4CMapRle.h"
#include "assets/map/T4CTmiWorldMap.h"
#include "assets/map/T4CV2SpriteAtlas.h"
#include "runtime/T4CRuntimePaths.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace {

constexpr int kIndexEntryCount = T4CV2WorldMap::kChunksPerAxis * T4CV2WorldMap::kChunksPerAxis;

bool ReadChunkIndex(const std::string &path, std::vector<std::uint32_t> &outOffsets) {
    outOffsets.clear();
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }
    outOffsets.resize(static_cast<std::size_t>(kIndexEntryCount));
    in.read(reinterpret_cast<char *>(outOffsets.data()),
            static_cast<std::streamsize>(outOffsets.size() * sizeof(std::uint32_t)));
    return static_cast<bool>(in);
}

}  // namespace

const char *T4CV2WorldMap::MapBaseName(unsigned short worldIndex) {
    switch (worldIndex) {
        case 0:
            return "v2_worldmap.map";
        case 1:
            return "v2_dungeonmap.map";
        case 2:
            return "v2_cavernmap.map";
        case 3:
            return "v2_underworld.map";
        case 4:
            return "v2_leoworld.map";
        case 5:
            return "v2_extension01.map";
        case 6:
            return "v2_extension02.map";
        case 7:
            return "v2_extension03.map";
        default:
            return nullptr;
    }
}

void T4CV2WorldMap::TileIdToRgb(std::uint16_t tileId, std::uint8_t &r, std::uint8_t &g, std::uint8_t &b) {
    if (tileId == 0) {
        r = 8;
        g = 8;
        b = 12;
        return;
    }
    const std::uint32_t h = static_cast<std::uint32_t>(tileId) * 2654435761u;
    r = static_cast<std::uint8_t>(64 + ((h >> 16) & 0x7F));
    g = static_cast<std::uint8_t>(64 + ((h >> 8) & 0x7F));
    b = static_cast<std::uint8_t>(64 + (h & 0x7F));
}

void T4CV2WorldMap::Clear() {
    open_ = false;
    objectLayerOpen_ = false;
    worldIndex_ = 0;
    mapPath_.clear();
    map2Path_.clear();
    chunkOffsets_.clear();
    objectChunkOffsets_.clear();
    chunkCache_.clear();
    objectChunkCache_.clear();
    lastError_.clear();
}

bool T4CV2WorldMap::OpenWorld(unsigned short worldIndex) {
    Clear();

    const char *base = MapBaseName(worldIndex);
    if (!base) {
        lastError_ = "Indice monde carte V2 invalide";
        return false;
    }

    const std::string path1 = T4CGameFilesPath((std::string(base) + "1").c_str());
    const std::string path2 = T4CGameFilesPath((std::string(base) + "2").c_str());

    std::error_code ec;
    if (!fs::is_regular_file(path1, ec)) {
        lastError_ = "Fichier carte introuvable : " + path1;
        return false;
    }

    if (!ReadChunkIndex(path1, chunkOffsets_)) {
        lastError_ = "Index carte incomplet : " + path1;
        chunkOffsets_.clear();
        return false;
    }

    mapPath_ = path1;
    open_ = true;
    worldIndex_ = worldIndex;

    if (fs::is_regular_file(path2, ec) && ReadChunkIndex(path2, objectChunkOffsets_)) {
        map2Path_ = path2;
        objectLayerOpen_ = true;
        SDL_Log("[T4CV2WorldMap] map2 OK — %s", map2Path_.c_str());
    } else {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[T4CV2WorldMap] map2 absent : %s", path2.c_str());
    }

    SDL_Log("[T4CV2WorldMap] monde %u OK — %s (%d chunks index)", static_cast<unsigned>(worldIndex), mapPath_.c_str(),
            kIndexEntryCount);
    return true;
}

bool T4CV2WorldMap::loadChunkFromPath(const std::string &path, const std::vector<std::uint32_t> &offsets,
                                      const std::uint16_t chunkX, const std::uint16_t chunkY,
                                      std::vector<std::uint16_t> &out) const {
    out.clear();
    if (path.empty() || chunkX >= kChunksPerAxis || chunkY >= kChunksPerAxis) {
        return false;
    }

    const std::size_t indexPos = static_cast<std::size_t>(chunkX) +
                               static_cast<std::size_t>(chunkY) * static_cast<std::size_t>(kChunksPerAxis);
    if (indexPos >= offsets.size()) {
        return false;
    }

    const std::uint32_t offset = offsets[indexPos];
    if (offset == 0) {
        out.assign(static_cast<std::size_t>(kTilesPerChunk), 0);
        return true;
    }

    std::vector<unsigned char> compressed(static_cast<std::size_t>(kCompressedChunkBytes));
    {
        std::ifstream in(path, std::ios::binary);
        if (!in) {
            return false;
        }
        in.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
        in.read(reinterpret_cast<char *>(compressed.data()), static_cast<std::streamsize>(compressed.size()));
        if (static_cast<std::size_t>(in.gcount()) != compressed.size()) {
            return false;
        }
    }

    out.resize(static_cast<std::size_t>(kTilesPerChunk));
    const auto *words = reinterpret_cast<const std::uint16_t *>(compressed.data());
    const std::size_t wordCount = compressed.size() / 2;
    const std::size_t decoded = T4CMapRle::DecompressFloor(words, wordCount, out.data(), out.size());
    if (decoded < out.size()) {
        std::fill(out.begin() + static_cast<std::ptrdiff_t>(decoded), out.end(), 0);
    }
    return true;
}

bool T4CV2WorldMap::loadFloorChunk(const std::uint16_t chunkX, const std::uint16_t chunkY,
                                   std::vector<std::uint16_t> &out) const {
    return loadChunkFromPath(mapPath_, chunkOffsets_, chunkX, chunkY, out);
}

bool T4CV2WorldMap::loadObjectChunk(const std::uint16_t chunkX, const std::uint16_t chunkY,
                                    std::vector<std::uint16_t> &out) const {
    return loadChunkFromPath(map2Path_, objectChunkOffsets_, chunkX, chunkY, out);
}

const std::vector<std::uint16_t> *T4CV2WorldMap::getCachedFloorChunk(const std::uint16_t chunkX,
                                                                     const std::uint16_t chunkY) const {
    const ChunkKey key{chunkX, chunkY};
    auto it = chunkCache_.find(key);
    if (it != chunkCache_.end()) {
        return &it->second;
    }

    std::vector<std::uint16_t> chunk;
    if (!loadFloorChunk(chunkX, chunkY, chunk)) {
        return nullptr;
    }

    if (chunkCache_.size() >= 64) {
        chunkCache_.clear();
    }
    const auto inserted = chunkCache_.emplace(key, std::move(chunk));
    return &inserted.first->second;
}

const std::vector<std::uint16_t> *T4CV2WorldMap::getCachedObjectChunk(const std::uint16_t chunkX,
                                                                       const std::uint16_t chunkY) const {
    if (!objectLayerOpen_) {
        return nullptr;
    }

    const ChunkKey key{chunkX, chunkY};
    auto it = objectChunkCache_.find(key);
    if (it != objectChunkCache_.end()) {
        return &it->second;
    }

    std::vector<std::uint16_t> chunk;
    if (!loadObjectChunk(chunkX, chunkY, chunk)) {
        return nullptr;
    }

    if (objectChunkCache_.size() >= 64) {
        objectChunkCache_.clear();
    }
    const auto inserted = objectChunkCache_.emplace(key, std::move(chunk));
    return &inserted.first->second;
}

std::uint16_t T4CV2WorldMap::GetFloorTileId(const unsigned int worldX, const unsigned int worldY) const {
    if (!open_ || worldX >= static_cast<unsigned>(kWorldSize) || worldY >= static_cast<unsigned>(kWorldSize)) {
        return 0;
    }

    const std::uint16_t chunkX = static_cast<std::uint16_t>(worldX / static_cast<unsigned>(kChunkSize));
    const std::uint16_t chunkY = static_cast<std::uint16_t>(worldY / static_cast<unsigned>(kChunkSize));
    const std::vector<std::uint16_t> *chunk = getCachedFloorChunk(chunkX, chunkY);
    if (!chunk || chunk->size() != static_cast<std::size_t>(kTilesPerChunk)) {
        return 0;
    }

    const unsigned lx = worldX % static_cast<unsigned>(kChunkSize);
    const unsigned ly = worldY % static_cast<unsigned>(kChunkSize);
    return (*chunk)[static_cast<std::size_t>(ly) * static_cast<std::size_t>(kChunkSize) + lx];
}

std::uint16_t T4CV2WorldMap::GetObjectTileId(const unsigned int worldX, const unsigned int worldY) const {
    if (!objectLayerOpen_ || worldX >= static_cast<unsigned>(kWorldSize) ||
        worldY >= static_cast<unsigned>(kWorldSize)) {
        return 0;
    }

    const std::uint16_t chunkX = static_cast<std::uint16_t>(worldX / static_cast<unsigned>(kChunkSize));
    const std::uint16_t chunkY = static_cast<std::uint16_t>(worldY / static_cast<unsigned>(kChunkSize));
    const std::vector<std::uint16_t> *chunk = getCachedObjectChunk(chunkX, chunkY);
    if (!chunk || chunk->size() != static_cast<std::size_t>(kTilesPerChunk)) {
        return 0;
    }

    const unsigned lx = worldX % static_cast<unsigned>(kChunkSize);
    const unsigned ly = worldY % static_cast<unsigned>(kChunkSize);
    return (*chunk)[static_cast<std::size_t>(ly) * static_cast<std::size_t>(kChunkSize) + lx];
}

void T4CV2WorldMap::drawIsoDiamond(SDL_Renderer *renderer, float cx, float cy, std::uint8_t r, std::uint8_t g,
                                   std::uint8_t b) const {
    const float hw = static_cast<float>(kIsoTileWidth) * 0.5f;
    const float hh = static_cast<float>(kIsoTileHeight) * 0.5f;

    SDL_SetRenderDrawColor(renderer, r, g, b, 220);
    for (int dy = static_cast<int>(-hh); dy <= static_cast<int>(hh); ++dy) {
        const float fy = static_cast<float>(dy);
        const float t = 1.f - std::abs(fy) / hh;
        const float halfW = hw * t;
        SDL_FRect row{cx - halfW, cy + fy, halfW * 2.f, 1.f};
        SDL_RenderFillRect(renderer, &row);
    }
}

void T4CV2WorldMap::RenderIsoViewport(SDL_Renderer *renderer, float centerX, float centerY,
                                      float screenCenterX, float screenCenterY, int viewRadiusTiles,
                                      const T4CTmiWorldMap *tmiColors, const T4CV2SpriteAtlas *sprites) const {
    if (!renderer || !open_) {
        return;
    }

    (void)viewRadiusTiles;
    const int pcx = static_cast<int>(std::floor(centerX));
    const int pcy = static_cast<int>(std::floor(centerY));

    /*
     * Projection T4C rectangulaire (Tileset.cpp / LoadVirtualMap) :
     *   screenX = screenCenterX + (worldX - centerX) * 32
     *   screenY = screenCenterY + (worldY - centerY) * 16
     * centerX/Y flottants = scroll fluide entre tuiles (Windows Done/g_DONE).
     */
    const int halfCols = static_cast<int>(screenCenterX / static_cast<float>(kIsoTileWidth)) + 2;
    const int rowsUp = static_cast<int>(screenCenterY / static_cast<float>(kIsoTileHeight)) + 3;
    const int rowsDown = static_cast<int>(screenCenterY / static_cast<float>(kIsoTileHeight)) + 13;

    const auto screenX = [&](const int wx) {
        return screenCenterX + (static_cast<float>(wx) - centerX) * static_cast<float>(kIsoTileWidth) +
               kIsoAnchorBiasX;
    };
    const auto screenY = [&](const int wy) {
        return screenCenterY + (static_cast<float>(wy) - centerY) * static_cast<float>(kIsoTileHeight) +
               kIsoAnchorBiasY;
    };

    // Couche sol : ordre haut->bas (worldY croissant) pour un recouvrement correct.
    for (int dy = -rowsUp; dy <= rowsDown; ++dy) {
        const int wy = pcy + dy;
        if (wy < 0 || wy >= kWorldSize) {
            continue;
        }
        const float sy = screenY(wy);
        for (int dx = -halfCols; dx <= halfCols; ++dx) {
            const int wx = pcx + dx;
            if (wx < 0 || wx >= kWorldSize) {
                continue;
            }

            const unsigned uwx = static_cast<unsigned>(wx);
            const unsigned uwy = static_cast<unsigned>(wy);
            const std::uint16_t tileId = GetFloorTileId(uwx, uwy);
            const float sx = screenX(wx);

            if (sprites && sprites->IsReady() && sprites->TryDrawTile(renderer, tileId, uwx, uwy, sx, sy)) {
                continue;
            }

            std::uint8_t r = 0;
            std::uint8_t g = 0;
            std::uint8_t b = 0;
            if (!tmiColors || !tmiColors->SampleRgb(uwx, uwy, r, g, b)) {
                TileIdToRgb(tileId, r, g, b);
            }
            drawIsoDiamond(renderer, sx, sy, r, g, b);
        }
    }

    if (!objectLayerOpen_ || !sprites || !sprites->IsReady()) {
        return;
    }

    const auto drawObjectLayer = [&](const bool specialOverlapPass) {
        for (int dy = -rowsUp; dy <= rowsDown; ++dy) {
            const int wy = pcy + dy;
            if (wy < 0 || wy >= kWorldSize) {
                continue;
            }
            const float sy = screenY(wy);
            for (int dx = -halfCols; dx <= halfCols; ++dx) {
                const int wx = pcx + dx;
                if (wx < 0 || wx >= kWorldSize) {
                    continue;
                }

                const unsigned uwx = static_cast<unsigned>(wx);
                const unsigned uwy = static_cast<unsigned>(wy);
                const std::uint16_t objectId = GetObjectTileId(uwx, uwy);
                if (objectId == 0 || !T4CMapObjectSprites::ShouldDrawMapObject(objectId)) {
                    continue;
                }

                const bool isSpecial = T4CMapObjectSprites::IsSpecialOverlapObject(objectId);
                if (isSpecial != specialOverlapPass) {
                    continue;
                }

                sprites->TryDrawMapObject(renderer, objectId, uwx, uwy, screenX(wx), sy);
            }
        }
    };

    /* Passe 1 : decors normaux ; passe 2 : overlap / chateau (Tileset.cpp CompiledView2). */
    drawObjectLayer(false);
    drawObjectLayer(true);
}
