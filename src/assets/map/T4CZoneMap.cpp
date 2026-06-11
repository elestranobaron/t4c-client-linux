#include "assets/map/T4CZoneMap.h"

#include "runtime/T4CRuntimePaths.h"

#include <SDL3/SDL.h>
#include <zlib.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace {

/* Cle XOR de zonemapinfo.zfn (Global.cpp ZONE_key). */
const unsigned char kZoneKey[16] = {0xE1, 0xC4, 0x1B, 0xC0, 0xC5, 0x0D, 0x58, 0xB7,
                                    0x01, 0xCE, 0x33, 0xD3, 0xA9, 0x3F, 0xBA, 0x99};

std::uint32_t ReadLE32(const unsigned char *p) {
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}

std::string GameFilePath(const char *name) {
    std::string path = T4CRuntimePath("Game Files");
    if (path.empty()) {
        return {};
    }
    path += '/';
    path += name;
    return path;
}

}  // namespace

bool T4CZoneMap::loadZoneNames() {
    if (namesLoaded_) {
        return true;
    }
    const std::string path = GameFilePath("zonemapinfo.zfn");
    size_t len = 0;
    unsigned char *data = static_cast<unsigned char *>(SDL_LoadFile(path.c_str(), &len));
    if (!data) {
        return false;
    }
    size_t lineStart = 0;
    for (size_t i = 0; i <= len; ++i) {
        if (i < len && data[i] != '\n') {
            continue;
        }
        size_t lineLen = i - lineStart;
        if (lineLen > 0 && lineLen < 1024) {
            char line[1024];
            std::memcpy(line, data + lineStart, lineLen);
            line[lineLen] = '\0';
            if (lineLen > 0 && line[lineLen - 1] == '\r') {
                line[--lineLen] = '\0';
            }
            for (size_t k = 0; k < lineLen; ++k) {
                line[k] = static_cast<char>(static_cast<unsigned char>(line[k]) ^ kZoneKey[k % 16]);
            }
            /* Format Windows : 4 chars monde, 4 chars index, puis nom. */
            if (lineLen > 8) {
                char wbuf[5] = {line[0], line[1], line[2], line[3], 0};
                char ibuf[5] = {line[4], line[5], line[6], line[7], 0};
                const int w = std::atoi(wbuf);
                const int idx = std::atoi(ibuf);
                if (w >= 0 && w < kWorldCount && idx >= 0 && idx <= 255 &&
                    w == static_cast<int>(worldIndex_)) {
                    zoneNames_[idx] = &line[8];
                }
            }
        }
        lineStart = i + 1;
    }
    SDL_free(data);
    namesLoaded_ = true;
    return true;
}

bool T4CZoneMap::LoadWorld(const unsigned short worldIndex) {
    if (loaded_ && worldIndex_ == worldIndex) {
        return true;
    }
    Clear();
    if (worldIndex >= kWorldCount) {
        return false;
    }
    worldIndex_ = worldIndex;

    const std::string path = GameFilePath("Zone_Map.dat");
    size_t fileLen = 0;
    unsigned char *file = static_cast<unsigned char *>(SDL_LoadFile(path.c_str(), &fileLen));
    if (!file) {
        return false;
    }
    /* Header : ulong origSize, 8 tailles compressees, 8 offsets (Global::LoadZoneMapWorld). */
    constexpr size_t kHeader = 4 + 8 * 4 + 8 * 4;
    bool ok = false;
    if (fileLen >= kHeader) {
        const std::uint32_t origSize = ReadLE32(file);
        const std::uint32_t compSize = ReadLE32(file + 4 + worldIndex * 4);
        const std::uint32_t filePos = ReadLE32(file + 4 + 8 * 4 + worldIndex * 4);
        if (origSize >= static_cast<std::uint32_t>(kMapSize) * kMapSize && filePos < fileLen &&
            static_cast<size_t>(filePos) + compSize <= fileLen) {
            std::vector<unsigned char> raw(origSize);
            uLongf outLen = origSize;
            if (uncompress(raw.data(), &outLen, file + filePos, compSize) == Z_OK &&
                outLen >= static_cast<uLongf>(kMapSize) * kMapSize) {
                zoneIds_.assign(reinterpret_cast<const char *>(raw.data()),
                                static_cast<size_t>(kMapSize) * kMapSize);
                ok = true;
            }
        }
    }
    SDL_free(file);
    if (!ok) {
        SDL_Log("[ZoneMap] echec chargement %s (monde %u)", path.c_str(), worldIndex);
        return false;
    }
    loadZoneNames();
    loaded_ = true;
    SDL_Log("[ZoneMap] monde %u charge (%zu noms)", worldIndex,
            [this] {
                size_t n = 0;
                for (const std::string &s : zoneNames_) {
                    if (!s.empty()) {
                        ++n;
                    }
                }
                return n;
            }());
    return true;
}

void T4CZoneMap::Clear() {
    loaded_ = false;
    zoneIds_.clear();
    for (std::string &s : zoneNames_) {
        s.clear();
    }
    namesLoaded_ = false;
}

int T4CZoneMap::ZoneIdAt(const unsigned int worldX, const unsigned int worldY) const {
    if (!loaded_ || worldX >= kMapSize || worldY >= kMapSize) {
        return 0xFF;
    }
    return static_cast<unsigned char>(
        zoneIds_[static_cast<size_t>(worldY) * kMapSize + worldX]);
}

std::string T4CZoneMap::ZoneNameAt(const unsigned int worldX, const unsigned int worldY) const {
    const int id = ZoneIdAt(worldX, worldY);
    if (id < 0 || id > 255) {
        return {};
    }
    return zoneNames_[id];
}
