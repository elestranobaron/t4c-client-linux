#include "assets/map/T4CTmiWorldMap.h"

#include "runtime/T4CRuntimePaths.h"

#include <SDL3/SDL.h>
#include <zlib.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace {

constexpr std::size_t kIndexBytes = static_cast<std::size_t>(T4CTmiWorldMap::kMapSize) *
                                  static_cast<std::size_t>(T4CTmiWorldMap::kMapSize);
constexpr std::size_t kPaletteBytes = 256 * 3;
constexpr std::size_t kHeaderBytes = 4 + T4CTmiWorldMap::kWorldCount * 4 + T4CTmiWorldMap::kWorldCount * 4;

std::uint32_t ReadLeU32(const unsigned char *p) {
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}

}  // namespace

void T4CTmiWorldMap::Clear() {
    loaded_ = false;
    worldIndex_ = 0;
    indices_.clear();
    paletteRgb_.clear();
    lastError_.clear();
}

bool T4CTmiWorldMap::LoadWorld(unsigned short worldIndex) {
    Clear();

    if (worldIndex >= kWorldCount) {
        lastError_ = "Indice monde TMI invalide";
        return false;
    }

    const std::string path = T4CGameFilesPath("TMI_Map.dat");
    std::error_code ec;
    if (!fs::is_regular_file(path, ec)) {
        lastError_ = "TMI_Map.dat introuvable : " + path + " (runtime=" + ResolveT4CRuntimeRoot() + ")";
        return false;
    }

    const auto fileSize = fs::file_size(path, ec);
    if (ec || fileSize < kHeaderBytes + 64) {
        lastError_ = "TMI_Map.dat trop petit : " + path;
        return false;
    }

    std::vector<unsigned char> file(static_cast<std::size_t>(fileSize));
    {
        std::ifstream in(path, std::ios::binary);
        if (!in || !in.read(reinterpret_cast<char *>(file.data()), static_cast<std::streamsize>(fileSize)) ||
            static_cast<std::size_t>(in.gcount()) != static_cast<std::size_t>(fileSize)) {
            lastError_ = "Lecture TMI_Map.dat echouee : " + path;
            return false;
        }
    }

    const std::uint32_t uncompressedSize = ReadLeU32(file.data());
    const auto *compressedSizes = reinterpret_cast<const std::uint32_t *>(file.data() + 4);
    const auto *fileOffsets = reinterpret_cast<const std::uint32_t *>(file.data() + 4 + kWorldCount * 4);

    const std::uint32_t compSize = compressedSizes[worldIndex];
    const std::uint32_t offset = fileOffsets[worldIndex];

    if (compSize == 0) {
        lastError_ = "Monde TMI vide (taille compresse 0)";
        return false;
    }

    const std::size_t end = static_cast<std::size_t>(offset) + static_cast<std::size_t>(compSize);
    if (end > file.size()) {
        char buf[512];
        std::snprintf(buf, sizeof(buf),
                      "Bloc TMI hors fichier : off=%u len=%u fin=%zu fichier=%zu path=%s runtime=%s",
                      offset, compSize, end, file.size(), path.c_str(), ResolveT4CRuntimeRoot().c_str());
        lastError_ = buf;
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[T4CTmiWorldMap] %s", buf);
        return false;
    }

    std::vector<unsigned char> raw(uncompressedSize);
    uLongf destLen = uncompressedSize;
    const int zrc =
        uncompress(raw.data(), &destLen, file.data() + offset, static_cast<uLong>(compSize));
    if (zrc != Z_OK) {
        char buf[256];
        std::snprintf(buf, sizeof(buf), "Decompression zlib TMI code=%d (off=%u len=%u)", zrc, offset, compSize);
        lastError_ = buf;
        return false;
    }
    if (destLen < kIndexBytes + kPaletteBytes) {
        lastError_ = "TMI decompresse trop petit";
        return false;
    }

    indices_.assign(raw.begin(), raw.begin() + static_cast<std::ptrdiff_t>(kIndexBytes));
    paletteRgb_.assign(raw.begin() + static_cast<std::ptrdiff_t>(kIndexBytes),
                       raw.begin() + static_cast<std::ptrdiff_t>(kIndexBytes + kPaletteBytes));

    loaded_ = true;
    worldIndex_ = worldIndex;
    SDL_Log("[T4CTmiWorldMap] monde %u OK — %s (%zu octets, off=%u comp=%u)", static_cast<unsigned>(worldIndex),
            path.c_str(), indices_.size(), offset, compSize);
    return true;
}

bool T4CTmiWorldMap::SampleRgb(unsigned int worldX, unsigned int worldY, std::uint8_t &r, std::uint8_t &g,
                               std::uint8_t &b) const {
    if (!loaded_ || worldX >= static_cast<unsigned>(kMapSize) || worldY >= static_cast<unsigned>(kMapSize) ||
        indices_.size() != kIndexBytes || paletteRgb_.size() != kPaletteBytes) {
        return false;
    }
    const std::size_t idx =
        static_cast<std::size_t>(worldY) * static_cast<std::size_t>(kMapSize) + static_cast<std::size_t>(worldX);
    const unsigned char palIndex = indices_[idx];
    const std::size_t p = static_cast<std::size_t>(palIndex) * 3;
    r = paletteRgb_[p];
    g = paletteRgb_[p + 1];
    b = paletteRgb_[p + 2];
    return true;
}

void T4CTmiWorldMap::RenderViewport(SDL_Renderer *renderer, unsigned int centerX, unsigned int centerY,
                                    int viewTilesW, int viewTilesH, int tilePx, float originX,
                                    float originY) const {
    if (!renderer || !loaded_ || indices_.size() != kIndexBytes || paletteRgb_.size() != kPaletteBytes) {
        return;
    }

    const int halfW = viewTilesW / 2;
    const int halfH = viewTilesH / 2;
    const int startX = static_cast<int>(centerX) - halfW;
    const int startY = static_cast<int>(centerY) - halfH;

    for (int dy = 0; dy < viewTilesH; ++dy) {
        for (int dx = 0; dx < viewTilesW; ++dx) {
            const int wx = startX + dx;
            const int wy = startY + dy;
            if (wx < 0 || wy < 0 || wx >= kMapSize || wy >= kMapSize) {
                continue;
            }
            const std::size_t idx =
                static_cast<std::size_t>(wy) * static_cast<std::size_t>(kMapSize) + static_cast<std::size_t>(wx);
            const unsigned char palIndex = indices_[idx];
            const std::size_t p = static_cast<std::size_t>(palIndex) * 3;
            SDL_SetRenderDrawColor(renderer, paletteRgb_[p], paletteRgb_[p + 1], paletteRgb_[p + 2], 255);
            SDL_FRect rect{originX + static_cast<float>(dx * tilePx), originY + static_cast<float>(dy * tilePx),
                           static_cast<float>(tilePx), static_cast<float>(tilePx)};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
