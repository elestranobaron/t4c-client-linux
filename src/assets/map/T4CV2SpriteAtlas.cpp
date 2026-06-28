#include "assets/map/T4CV2SpriteAtlas.h"
#include "game/T4CUnitSpriteNames.h"

#include "assets/map/T4CMapObjectSprites.h"
#include "assets/map/T4CV2WorldMap.h"
#include "runtime/T4CRuntimePaths.h"

#include <SDL3/SDL.h>
#include <zlib.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <strings.h>
#include <unordered_set>
#include <vector>

namespace {

void LogSpriteIssueOnce(const char *kind, const std::string &spriteName, const char *detail = nullptr) {
    static std::unordered_set<std::string> logged;
    std::string key = kind;
    key += ':';
    key += spriteName;
    if (detail != nullptr) {
        key += ':';
        key += detail;
    }
    if (!logged.insert(key).second) {
        return;
    }
    if (detail != nullptr) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[SpriteAtlas] %s '%s' (%s)", kind, spriteName.c_str(), detail);
    } else {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[SpriteAtlas] %s '%s'", kind, spriteName.c_str());
    }
}

#pragma pack(push, 1)
struct V2DidHeaderFile {
    char strNom[64];
    char strPath[256];
    std::uint32_t dwFileOffset;
    std::uint32_t dwDataFileIndex;
    std::uint32_t dwThisPosIndex;
};

struct V2SpriteHeaderDisk {
    std::uint16_t dwCompType;
    std::uint16_t flag1;
    std::uint16_t dwWidth;
    std::uint16_t dwHeight;
    std::int16_t shOffX1;
    std::int16_t shOffY1;
    std::int16_t shOffX2;
    std::int16_t shOffY2;
    std::uint16_t ushTransparency;
    std::uint16_t ushTransColor;
    std::uint32_t dwDataUnpack;
    std::uint32_t dwDataPack;
};

struct PalInfoDisk {
    char lpszID[64];
    char lpSpritePal[256 * 3];
};
#pragma pack(pop)

constexpr std::uint16_t kCompDd = 1;
constexpr std::uint16_t kCompNck = 2;
constexpr std::uint16_t kCompNull = 3;  /* Windows COMP_NULL : frame vide (pas d'erreur). */
constexpr std::uint16_t kCompZip = 9;

bool ReadFileBytes(const std::string &path, std::vector<std::uint8_t> &out) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }
    in.seekg(0, std::ios::end);
    const auto sz = in.tellg();
    if (sz <= 0) {
        return false;
    }
    out.resize(static_cast<std::size_t>(sz));
    in.seekg(0, std::ios::beg);
    in.read(reinterpret_cast<char *>(out.data()), sz);
    return static_cast<std::size_t>(in.gcount()) == out.size();
}

bool ReadGameFileBytes(const char *subpath, std::vector<std::uint8_t> &out) {
    const std::string resolved = T4CResolveGameFilesPath(subpath);
    if (resolved.empty()) {
        return false;
    }
    return ReadFileBytes(resolved, out);
}

void XorBuffer(std::vector<std::uint8_t> &buf, std::uint8_t key) {
    for (auto &b : buf) {
        b ^= key;
    }
}

V2SpriteHeaderDisk DecodeHeader(const V2SpriteHeaderDisk &raw) {
    V2SpriteHeaderDisk h = raw;
    h.dwCompType ^= 0xAAAA;
    h.flag1 ^= 0x1458;
    h.dwWidth ^= 0x1234;
    h.dwHeight ^= 0x6242;
    h.shOffX1 ^= 0x2355;
    h.shOffY1 ^= 0xF6C3;
    h.shOffX2 ^= 0xAAF3;
    h.shOffY2 ^= 0xAAAA;
    h.ushTransparency ^= 0x4321;
    h.ushTransColor ^= 0x1234;
    h.dwDataUnpack ^= 0xDDCCBBAA;
    h.dwDataPack ^= 0xAABBCCDD;
    return h;
}

bool DecompressNckIndices(const std::uint8_t *packed, std::size_t packedSize, int width, int height,
                          std::uint8_t transColor, std::vector<std::uint8_t> &out,
                          std::vector<std::uint8_t> &shadowMask, int &strideOut) {
    const int lg = (width + 3) & ~3;
    const int ht = height;
    strideOut = lg;
    out.assign(static_cast<std::size_t>(lg * ht), transColor);
    shadowMask.assign(static_cast<std::size_t>(lg * ht), 0);

    const std::uint8_t *pt = packed;
    const std::uint8_t *const end = packed + packedSize;
    int x = 0;
    int y = 0;

    while (pt + 4 <= end && y < ht) {
        x = static_cast<int>(pt[0]) | (static_cast<int>(pt[1]) << 8);
        pt += 2;
        const int nbPix = (static_cast<int>(pt[0]) * 4) + static_cast<int>(pt[1]);
        pt += 2;
        if (pt >= end) {
            break;
        }

        if (*pt == 1) {
            /* NCK_SHADOW : damier Windows (LoadSprite_NCK / DrawNCK) → noir, pas transparent. */
            if (nbPix > 1) {
                for (int i = 0; i < nbPix; ++i) {
                    const int px = i + x;
                    if (px < 0 || px >= lg || y < 0 || y >= ht) {
                        continue;
                    }
                    bool shadowPixel = false;
                    if ((y % 2) == 0) {
                        if ((px % 2) != 0) {
                            shadowPixel = true;
                        }
                    } else if ((px % 2) == 0) {
                        shadowPixel = true;
                    }
                    if (shadowPixel) {
                        const std::size_t off = static_cast<std::size_t>(px + y * lg);
                        out[off] = 0;
                        shadowMask[off] = 1;
                    }
                }
            }
        } else if (*pt == 0) {
            for (int i = 0; i < nbPix && pt + 1 < end; ++i) {
                ++pt;
                const int px = i + x;
                if (px >= 0 && px < lg && y >= 0 && y < ht) {
                    out[static_cast<std::size_t>(px + y * lg)] = *pt;
                }
            }
        }
        ++pt;
        if (pt >= end) {
            break;
        }
        if (*pt == 0) {
            break;
        }
        if (*pt == 1) {
            ++pt;
        } else if (*pt == 2) {
            ++y;
            ++pt;
        }
    }
    return true;
}

}  // namespace

T4CV2SpriteAtlas::~T4CV2SpriteAtlas() {
    Clear();
}

void T4CV2SpriteAtlas::Clear() {
    for (auto &kv : spriteCache_) {
        if (kv.second.tex) {
            SDL_DestroyTexture(kv.second.tex);
        }
        if (kv.second.outlineTex) {
            SDL_DestroyTexture(kv.second.outlineTex);
        }
    }
    spriteCache_.clear();
    didEntries_.clear();
    didByName_.clear();
    ddaBanks_.clear();
    ddaBankMissing_.clear();
    palettes_.clear();
    hasDefaultPalette_ = false;
    defaultPalette_.fill(0);
    ready_ = false;
    renderer_ = nullptr;
    lastError_.clear();
}

int T4CV2SpriteAtlas::appendPalettesFromFile(const char *path) {
    std::vector<std::uint8_t> compressed;
    if (!ReadGameFileBytes(path, compressed) || compressed.size() < 24) {
        return 0;
    }

    const std::size_t headerSkip = 16 + 4 + 4 + 17;
    const std::uint32_t uncompSize =
        static_cast<std::uint32_t>(compressed[16]) | (static_cast<std::uint32_t>(compressed[17]) << 8) |
        (static_cast<std::uint32_t>(compressed[18]) << 16) | (static_cast<std::uint32_t>(compressed[19]) << 24);
    const std::uint32_t compSize =
        static_cast<std::uint32_t>(compressed[20]) | (static_cast<std::uint32_t>(compressed[21]) << 8) |
        (static_cast<std::uint32_t>(compressed[22]) << 16) | (static_cast<std::uint32_t>(compressed[23]) << 24);

    if (headerSkip + compSize + 1 > compressed.size() || uncompSize < sizeof(PalInfoDisk)) {
        return 0;
    }

    const std::uint8_t *compData = compressed.data() + headerSkip;
    std::vector<std::uint8_t> raw(uncompSize);
    uLongf destLen = uncompSize;
    if (uncompress(raw.data(), &destLen, compData, compSize) != Z_OK) {
        return 0;
    }
    XorBuffer(raw, 0x66);

    const std::size_t count = raw.size() / sizeof(PalInfoDisk);
    const auto *entries = reinterpret_cast<const PalInfoDisk *>(raw.data());
    int added = 0;
    for (std::size_t i = 0; i < count; ++i) {
        const PalInfoDisk &disk = entries[i];
        const std::size_t idLen = strnlen(disk.lpszID, sizeof(disk.lpszID));
        if (idLen == 0) {
            continue;
        }
        PaletteEntry entry;
        entry.id.assign(disk.lpszID, idLen);
        if (!entry.id.empty() && entry.id.back() == 'P') {
            entry.id.pop_back();
        }
        std::memcpy(entry.rgb.data(), disk.lpSpritePal, entry.rgb.size());
        palettes_.push_back(std::move(entry));
        ++added;
    }
    return added;
}

bool T4CV2SpriteAtlas::loadDidFile(const char *path, bool nmsBank) {
    std::vector<std::uint8_t> file;
    if (!ReadGameFileBytes(path, file) || file.size() < 38) {
        return false;
    }

    const std::size_t headerSkip = 16 + 4 + 4 + 17;
    const std::uint32_t uncompSize =
        static_cast<std::uint32_t>(file[16]) | (static_cast<std::uint32_t>(file[17]) << 8) |
        (static_cast<std::uint32_t>(file[18]) << 16) | (static_cast<std::uint32_t>(file[19]) << 24);
    const std::uint32_t compSize =
        static_cast<std::uint32_t>(file[20]) | (static_cast<std::uint32_t>(file[21]) << 8) |
        (static_cast<std::uint32_t>(file[22]) << 16) | (static_cast<std::uint32_t>(file[23]) << 24);

    if (headerSkip + compSize + 1 > file.size() || uncompSize == 0 ||
        uncompSize % sizeof(V2DidHeaderFile) != 0) {
        return false;
    }

    const std::uint8_t *compData = file.data() + headerSkip;
    std::vector<std::uint8_t> raw(uncompSize);
    uLongf destLen = uncompSize;
    if (uncompress(raw.data(), &destLen, compData, compSize) != Z_OK) {
        return false;
    }
    XorBuffer(raw, 0x99);

    const std::size_t count = raw.size() / sizeof(V2DidHeaderFile);
    const auto *entries = reinterpret_cast<const V2DidHeaderFile *>(raw.data());
    for (std::size_t i = 0; i < count; ++i) {
        const V2DidHeaderFile &e = entries[i];
        const std::size_t nameLen = strnlen(e.strNom, sizeof(e.strNom));
        if (nameLen == 0) {
            continue;
        }
        DidEntry de;
        de.name.assign(e.strNom, nameLen);
        de.fileOffset = e.dwFileOffset;
        de.dataFileIndex = e.dwDataFileIndex + (nmsBank ? static_cast<std::uint32_t>(kNbFilesOri) : 0U);
        didEntries_.push_back(std::move(de));
        const std::size_t idx = didEntries_.size() - 1;
        didByName_[NormalizeDidKey(didEntries_[idx].name)] = idx;
    }
    return true;
}

std::string T4CV2SpriteAtlas::NormalizeDidKey(const std::string &name) {
    std::string key = name;
    for (char &c : key) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return key;
}

bool T4CV2SpriteAtlas::ensureBankLoaded(const std::uint32_t bankIndex) const {
    if (bankIndex >= ddaBanks_.size()) {
        return false;
    }
    if (!ddaBanks_[bankIndex].empty()) {
        return true;
    }
    if (ddaBankMissing_[bankIndex]) {
        return false;
    }

    const char *pathTemplate = nullptr;
    int fileIndex = 0;
    if (bankIndex < static_cast<std::uint32_t>(kNbFilesOri)) {
        pathTemplate = "V2Data";
        fileIndex = static_cast<int>(bankIndex);
    } else {
        pathTemplate = "v2nmsdata";
        fileIndex = static_cast<int>(bankIndex - kNbFilesOri);
    }

    char path[128];
    std::snprintf(path, sizeof(path), "%s%02d.dda", pathTemplate, fileIndex);
    std::vector<std::uint8_t> bytes;
    if (!ReadGameFileBytes(path, bytes)) {
        ddaBankMissing_[bankIndex] = true;
        LogSpriteIssueOnce("banque absente", path);
        return false;
    }
    if (bytes.size() > 4) {
        bytes.erase(bytes.begin(), bytes.begin() + 4);
    }
    ddaBanks_[bankIndex] = std::move(bytes);
    return true;
}

void T4CV2SpriteAtlas::BeginRenderFrame() const {
    decodeBudget_ = kDecodeBudgetPerFrame;
}

bool T4CV2SpriteAtlas::Init(SDL_Renderer *renderer) {
    Clear();
    renderer_ = renderer;
    if (!renderer_) {
        lastError_ = "Renderer null";
        return false;
    }

    const int palOri = appendPalettesFromFile("V2ColorI.dpd");
    const int palNms = appendPalettesFromFile("v2nmscolori.dpd");
    if (palOri + palNms <= 0) {
        lastError_ = "Palettes V2 introuvables";
        return false;
    }
    defaultPalette_ = palettes_.front().rgb;
    for (const PaletteEntry &pal : palettes_) {
        if (strcasecmp(pal.id.c_str(), "Bright1") == 0 || strcasecmp(pal.id.c_str(), "bright1") == 0) {
            defaultPalette_ = pal.rgb;
            break;
        }
    }
    hasDefaultPalette_ = true;

    const std::size_t oriEntriesBefore = didEntries_.size();
    if (!loadDidFile("V2DataI.did", false)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "[T4CV2SpriteAtlas] V2DataI.did introuvable ou invalide — sprites creatures/UI ORI "
                    "indisponibles");
    } else {
        SDL_Log("[T4CV2SpriteAtlas] V2DataI.did : %zu entrees",
                didEntries_.size() - oriEntriesBefore);
    }
    if (!loadDidFile("v2nmsdatai.did", true)) {
        lastError_ = "Index v2nmsdatai.did introuvable";
        return false;
    }

    ddaBanks_.assign(static_cast<std::size_t>(kTotalBanks), {});
    ddaBankMissing_.assign(static_cast<std::size_t>(kTotalBanks), false);

    ready_ = true;
    SDL_Log("[T4CV2SpriteAtlas] OK — %zu entrees index, %zu palettes (banques .dda a la demande)",
            didEntries_.size(), palettes_.size());
    return true;
}

const T4CV2SpriteAtlas::DidEntry *T4CV2SpriteAtlas::findEntry(const std::string &name) const {
    const auto it = didByName_.find(NormalizeDidKey(name));
    if (it == didByName_.end()) {
        return nullptr;
    }
    return &didEntries_[it->second];
}

const std::uint8_t *T4CV2SpriteAtlas::paletteForSprite(const std::string &spriteName,
                                                         const int paletteVariant) const {
    if (!hasDefaultPalette_) {
        return nullptr;
    }

    const auto lookupPal = [this](const std::string &viewId, const int palNb) -> const std::uint8_t * {
        if (palNb == 1) {
            const std::uint8_t *best = defaultPalette_.data();
            std::size_t bestLen = 0;
            for (const PaletteEntry &pal : palettes_) {
                if (pal.id.size() <= bestLen || pal.id.size() > viewId.size()) {
                    continue;
                }
                if (strncasecmp(pal.id.c_str(), viewId.c_str(), pal.id.size()) == 0) {
                    best = pal.rgb.data();
                    bestLen = pal.id.size();
                }
            }
            return best;
        }

        if (palNb >= 0 && palNb < 10) {
            const char suffix = static_cast<char>('0' + palNb);
            const PaletteEntry *best = nullptr;
            std::size_t bestLen = 0;
            for (const PaletteEntry &pal : palettes_) {
                if (pal.id.size() < 2 || pal.id.back() != suffix) {
                    continue;
                }
                const std::string temp = pal.id.substr(0, pal.id.size() - 1);
                if (temp.size() <= bestLen || temp.size() > viewId.size()) {
                    continue;
                }
                if (strncasecmp(temp.c_str(), viewId.c_str(), temp.size()) == 0) {
                    best = &pal;
                    bestLen = temp.size();
                }
            }
            if (best) {
                return best->rgb.data();
            }
        }
        return defaultPalette_.data();
    };

    const auto pickIfSpecific = [this](const std::uint8_t *pal) -> const std::uint8_t * {
        return (pal != nullptr && pal != defaultPalette_.data()) ? pal : nullptr;
    };

    std::string full = spriteName;
    if (const std::size_t p = full.find(" ("); p != std::string::npos) {
        full.resize(p);
    }

    const std::string viewKey = T4CUnitSpritePaletteKey(spriteName);

    if (paletteVariant >= 0 && paletteVariant < 10) {
        if (const std::uint8_t *pal = pickIfSpecific(lookupPal(viewKey, paletteVariant))) {
            return pal;
        }
    }

    /* CreateSprite Windows : GetPal(lpszID, 1) puis repli Bright1. */
    if (const std::uint8_t *pal = pickIfSpecific(lookupPal(full, 1))) {
        return pal;
    }
    if (const std::uint8_t *pal = pickIfSpecific(lookupPal(viewKey, 1))) {
        return pal;
    }

    /* Icon3D puppet : GetPal(ViewID, palID) avec palID=0. */
    if (const std::uint8_t *pal = pickIfSpecific(lookupPal(viewKey, 0))) {
        return pal;
    }

    return defaultPalette_.data();
}

bool T4CV2SpriteAtlas::decodeSprite(const DidEntry &entry, DecodedSprite &out) const {
    if (!ensureBankLoaded(entry.dataFileIndex)) {
        return false;
    }
    if (entry.dataFileIndex >= ddaBanks_.size()) {
        return false;
    }
    const std::vector<std::uint8_t> &bank = ddaBanks_[entry.dataFileIndex];
    if (entry.fileOffset + sizeof(V2SpriteHeaderDisk) > bank.size()) {
        return false;
    }

    V2SpriteHeaderDisk raw{};
    std::memcpy(&raw, bank.data() + entry.fileOffset, sizeof(raw));
    const V2SpriteHeaderDisk h = DecodeHeader(raw);
    if (h.dwCompType == kCompNull) {
        out = {};
        return true;
    }
    if (h.dwWidth == 0 || h.dwHeight == 0) {
        return false;
    }

    const std::size_t dataOff = entry.fileOffset + sizeof(V2SpriteHeaderDisk);
    if (dataOff + h.dwDataPack > bank.size()) {
        return false;
    }

    std::vector<std::uint8_t> packed(h.dwDataPack);
    std::memcpy(packed.data(), bank.data() + dataOff, h.dwDataPack);

    if (h.dwWidth > 180 || h.dwHeight > 180) {
        std::vector<std::uint8_t> outer(h.dwDataUnpack);
        uLongf destLen = h.dwDataUnpack;
        if (uncompress(outer.data(), &destLen, packed.data(), static_cast<uLong>(packed.size())) != Z_OK) {
            return false;
        }
        packed = std::move(outer);
    }

    out.width = static_cast<int>(h.dwWidth);
    out.height = static_cast<int>(h.dwHeight);
    out.offX = h.shOffX1;
    out.offY = h.shOffY1;
    out.offX2 = h.shOffX2;
    out.offY2 = h.shOffY2;
    out.transIndex = static_cast<std::uint8_t>(h.ushTransColor & 0xFF);
    out.colorKeyed = h.ushTransparency != 0;
    const int stride = (out.width + 3) & ~3;
    out.stride = stride;

    switch (h.dwCompType) {
        case kCompZip: {
            if (packed.size() < 4) {
                return false;
            }
            const std::uint32_t innerSize =
                static_cast<std::uint32_t>(packed[0]) | (static_cast<std::uint32_t>(packed[1]) << 8) |
                (static_cast<std::uint32_t>(packed[2]) << 16) | (static_cast<std::uint32_t>(packed[3]) << 24);
            if (innerSize == 0 || packed.size() < 5) {
                return false;
            }
            std::vector<std::uint8_t> rawIndices(innerSize);
            uLongf destLen = innerSize;
            if (uncompress(rawIndices.data(), &destLen, packed.data() + 4,
                           static_cast<uLong>(packed.size() - 4)) != Z_OK) {
                return false;
            }
            out.indices = std::move(rawIndices);
            return out.indices.size() >= static_cast<std::size_t>(stride * out.height);
        }
        case kCompNck: {
            return DecompressNckIndices(packed.data(), packed.size(), out.width, out.height, out.transIndex,
                                        out.indices, out.shadowMask, out.stride);
        }
        case kCompDd: {
            out.indices.assign(static_cast<std::size_t>(stride * out.height), out.transIndex);
            const std::size_t copy =
                std::min(packed.size(), static_cast<std::size_t>(stride * out.height));
            std::memcpy(out.indices.data(), packed.data(), copy);
            return true;
        }
        default:
            return false;
    }
}

SDL_Texture *T4CV2SpriteAtlas::createTextureFromDecoded(const DecodedSprite &sprite,
                                                         const std::uint8_t *paletteRgb,
                                                         const DecodedSprite *alphaMask,
                                                         const bool omitShadow) const {
    if (!renderer_ || !paletteRgb || sprite.indices.empty()) {
        return nullptr;
    }

    /* Masque alpha jumeau « <Nom>Mask » (curseurs Windows : TransAlphaMask2 fait
     * dest*alpha + src*(255-alpha) — la valeur du masque est l'opacite du FOND).
     * alpha sprite = 255 - valeur masque ; les pixels color-key restent invisibles. */
    const bool hasAlphaMask = alphaMask && !alphaMask->indices.empty() &&
                              alphaMask->width >= sprite.width && alphaMask->height >= sprite.height;

    const bool hasShadowMask =
        !sprite.shadowMask.empty() &&
        sprite.shadowMask.size() >= static_cast<std::size_t>(sprite.stride * sprite.height);

    /* DirectDraw (V2Sprite.cpp) pose le color-key sur la VALEUR RGB de ushTransColor, pas sur
     * l'index : tout pixel de meme couleur (ex. fond noir index 0 quand transIndex=255 est aussi
     * noir) doit etre transparent — sinon carre noir derriere gant/epee/curseurs. */
    const std::size_t tpi = static_cast<std::size_t>(sprite.transIndex) * 3;
    const std::uint8_t tr = paletteRgb[tpi];
    const std::uint8_t tg = paletteRgb[tpi + 1];
    const std::uint8_t tb = paletteRgb[tpi + 2];

    std::vector<std::uint8_t> rgba(static_cast<std::size_t>(sprite.width * sprite.height * 4));
    for (int y = 0; y < sprite.height; ++y) {
        for (int x = 0; x < sprite.width; ++x) {
            const std::size_t si = static_cast<std::size_t>(x + y * sprite.stride);
            const std::uint8_t idx = sprite.indices[si];
            const std::size_t o = static_cast<std::size_t>((x + y * sprite.width) * 4);
            const std::size_t pi = static_cast<std::size_t>(idx) * 3;
            if (hasShadowMask && sprite.shadowMask[si] != 0) {
                if (omitShadow) {
                    rgba[o + 3] = 0;
                } else {
                    rgba[o + 0] = 0;
                    rgba[o + 1] = 0;
                    rgba[o + 2] = 0;
                    rgba[o + 3] = 255;
                }
            } else if (idx == sprite.transIndex ||
                       (sprite.colorKeyed && paletteRgb[pi] == tr && paletteRgb[pi + 1] == tg &&
                        paletteRgb[pi + 2] == tb)) {
                rgba[o + 3] = 0;
            } else {
                rgba[o + 0] = paletteRgb[pi];
                rgba[o + 1] = paletteRgb[pi + 1];
                rgba[o + 2] = paletteRgb[pi + 2];
                rgba[o + 3] = 255;
                if (hasAlphaMask) {
                    const std::size_t mi = static_cast<std::size_t>(x + y * alphaMask->stride);
                    if (mi < alphaMask->indices.size()) {
                        rgba[o + 3] = static_cast<std::uint8_t>(255 - alphaMask->indices[mi]);
                    }
                }
            }
        }
    }

    SDL_Surface *surface = SDL_CreateSurface(sprite.width, sprite.height, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        return nullptr;
    }

    if (!SDL_LockSurface(surface)) {
        SDL_DestroySurface(surface);
        return nullptr;
    }

    const int pitch = surface->pitch;
    auto *dst = static_cast<std::uint8_t *>(surface->pixels);
    for (int y = 0; y < sprite.height; ++y) {
        std::memcpy(dst + y * pitch, rgba.data() + static_cast<std::size_t>(y * sprite.width * 4),
                    static_cast<std::size_t>(sprite.width * 4));
    }
    SDL_UnlockSurface(surface);

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_DestroySurface(surface);
    if (tex) {
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    }
    return tex;
}

SDL_Texture *T4CV2SpriteAtlas::createOutlineTextureFromDecoded(const DecodedSprite &sprite,
                                                                const std::uint8_t *paletteRgb) const {
    if (!renderer_ || sprite.indices.empty() || sprite.width <= 0 || sprite.height <= 0) {
        return nullptr;
    }

    const bool hasShadowMask =
        !sprite.shadowMask.empty() &&
        sprite.shadowMask.size() >= static_cast<std::size_t>(sprite.stride * sprite.height);

    const std::size_t tpi = static_cast<std::size_t>(sprite.transIndex) * 3;
    const bool keyRgb = sprite.colorKeyed && paletteRgb != nullptr;

    auto isOpaque = [&](const int x, const int y) -> bool {
        if (x < 0 || y < 0 || x >= sprite.width || y >= sprite.height) {
            return false;
        }
        const std::size_t si = static_cast<std::size_t>(x + y * sprite.stride);
        if (hasShadowMask && sprite.shadowMask[si] != 0) {
            return true;
        }
        const std::uint8_t idx = sprite.indices[si];
        if (idx == sprite.transIndex) {
            return false;
        }
        if (keyRgb) {
            const std::size_t pi = static_cast<std::size_t>(idx) * 3;
            if (paletteRgb[pi] == paletteRgb[tpi] && paletteRgb[pi + 1] == paletteRgb[tpi + 1] &&
                paletteRgb[pi + 2] == paletteRgb[tpi + 2]) {
                return false;
            }
        }
        return true;
    };

    std::vector<std::uint8_t> rgba(static_cast<std::size_t>(sprite.width * sprite.height * 4), 0);
    for (int y = 0; y < sprite.height; ++y) {
        for (int x = 0; x < sprite.width; ++x) {
            if (isOpaque(x, y)) {
                continue;
            }
            bool edge = false;
            for (int dy = -1; dy <= 1 && !edge; ++dy) {
                for (int dx = -1; dx <= 1 && !edge; ++dx) {
                    if (dx == 0 && dy == 0) {
                        continue;
                    }
                    if (isOpaque(x + dx, y + dy)) {
                        edge = true;
                    }
                }
            }
            if (!edge) {
                continue;
            }
            const std::size_t o = static_cast<std::size_t>((x + y * sprite.width) * 4);
            rgba[o + 0] = 255;
            rgba[o + 1] = 255;
            rgba[o + 2] = 220;
            rgba[o + 3] = 255;
        }
    }

    SDL_Surface *surface = SDL_CreateSurface(sprite.width, sprite.height, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        return nullptr;
    }
    if (!SDL_LockSurface(surface)) {
        SDL_DestroySurface(surface);
        return nullptr;
    }
    const int pitch = surface->pitch;
    auto *dst = static_cast<std::uint8_t *>(surface->pixels);
    for (int y = 0; y < sprite.height; ++y) {
        std::memcpy(dst + y * pitch, rgba.data() + static_cast<std::size_t>(y * sprite.width * 4),
                    static_cast<std::size_t>(sprite.width * 4));
    }
    SDL_UnlockSurface(surface);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_DestroySurface(surface);
    if (tex) {
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    }
    return tex;
}

const T4CV2SpriteAtlas::CachedSprite *T4CV2SpriteAtlas::decodeAndCache(const std::string &spriteName,
                                                                         const bool respectBudget,
                                                                         const int paletteVariant,
                                                                         const bool omitShadow) const {
    std::string cacheKey = spriteName;
    if (paletteVariant >= 0) {
        cacheKey += "#p";
        cacheKey += std::to_string(paletteVariant);
    }
    if (omitShadow) {
        cacheKey += "#noshad";
    }

    auto it = spriteCache_.find(cacheKey);
    if (it != spriteCache_.end()) {
        return it->second.tex ? &it->second : nullptr;
    }

    const DidEntry *entry = findEntry(spriteName);
    if (!entry) {
        LogSpriteIssueOnce("index absent", spriteName);
        spriteCache_[cacheKey] = {};
        return nullptr;
    }

    if (respectBudget && decodeBudget_ <= 0) {
        LogSpriteIssueOnce("budget epuise", spriteName);
        return nullptr;
    }

    DecodedSprite decoded;
    if (!decodeSprite(*entry, decoded)) {
        char detail[48];
        std::snprintf(detail, sizeof(detail), "banque %u off %u", entry->dataFileIndex, entry->fileOffset);
        LogSpriteIssueOnce("decode KO", spriteName, detail);
        spriteCache_[cacheKey] = {};
        return nullptr;
    }
    if (decoded.width <= 0 || decoded.height <= 0) {
        spriteCache_[cacheKey] = {};
        return nullptr;
    }
    if (respectBudget) {
        decodeBudget_ -= 1;
    }

    /* Masque alpha jumeau « <Nom>Mask » (curseurs : V3_TalkCursor + V3_TalkCursorMask…) :
     * applique la translucidite par pixel comme DrawSpriteN(pSprMask) sous Windows. */
    DecodedSprite maskDecoded;
    const DecodedSprite *alphaMask = nullptr;
    if (spriteName.size() < 4 || spriteName.compare(spriteName.size() - 4, 4, "Mask") != 0) {
        if (const DidEntry *maskEntry = findEntry(spriteName + "Mask")) {
            if (decodeSprite(*maskEntry, maskDecoded)) {
                alphaMask = &maskDecoded;
            }
        }
    }

    const std::uint8_t *pal = paletteForSprite(spriteName, paletteVariant);
    SDL_Texture *tex = createTextureFromDecoded(decoded, pal, alphaMask, omitShadow);
    CachedSprite cached;
    cached.tex = tex;
    cached.outlineTex = tex ? createOutlineTextureFromDecoded(decoded, pal) : nullptr;
    cached.offX = decoded.offX;
    cached.offY = decoded.offY;
    cached.offX2 = decoded.offX2;
    cached.offY2 = decoded.offY2;
    spriteCache_[cacheKey] = cached;
    return tex ? &spriteCache_[cacheKey] : nullptr;
}

const T4CV2SpriteAtlas::CachedSprite *T4CV2SpriteAtlas::getOrCreateSprite(const std::string &spriteName) const {
    return decodeAndCache(spriteName, true);
}

int T4CV2SpriteAtlas::PreloadSprites(const std::vector<std::string> &names, const int startIndex,
                                     const int maxCount) const {
    if (maxCount <= 0 || startIndex < 0 || static_cast<std::size_t>(startIndex) >= names.size()) {
        return 0;
    }

    int examined = 0;
    while (startIndex + examined < static_cast<int>(names.size()) && examined < maxCount) {
        const std::string &name = names[static_cast<std::size_t>(startIndex + examined)];
        if (spriteCache_.find(name) == spriteCache_.end()) {
            decodeAndCache(name, false);
        }
        examined += 1;
    }
    return examined;
}

int T4CV2SpriteAtlas::PreloadSpritesForMs(const std::vector<std::string> &names, const int startIndex,
                                          const int maxMs) const {
    if (maxMs <= 0 || startIndex < 0 || static_cast<std::size_t>(startIndex) >= names.size()) {
        return 0;
    }
    const Uint64 deadline = SDL_GetTicks() + static_cast<Uint64>(maxMs);
    int examined = 0;
    while (startIndex + examined < static_cast<int>(names.size()) && SDL_GetTicks() < deadline) {
        const std::string &name = names[static_cast<std::size_t>(startIndex + examined)];
        if (spriteCache_.find(name) == spriteCache_.end()) {
            decodeAndCache(name, false);
        }
        examined += 1;
    }
    return examined;
}

void T4CV2SpriteAtlas::PreloadBanksForNames(const std::vector<std::string> &names) const {
    std::unordered_set<std::uint32_t> banks;
    banks.reserve(8);
    for (const std::string &name : names) {
        const DidEntry *entry = findEntry(name);
        if (entry) {
            banks.insert(entry->dataFileIndex);
        }
    }
    for (const std::uint32_t bank : banks) {
        ensureBankLoaded(bank);
    }
}

void T4CV2SpriteAtlas::CollectViewportSpriteNames(const T4CV2WorldMap &map, const unsigned int centerX,
                                                  const unsigned int centerY, const int viewRadiusTiles,
                                                  std::vector<std::string> &out) {
    out.clear();
    if (!map.IsOpen()) {
        return;
    }

    const int radius = std::max(1, viewRadiusTiles);
    const int pcx = static_cast<int>(centerX);
    const int pcy = static_cast<int>(centerY);
    std::unordered_set<std::string> unique;
    unique.reserve(static_cast<std::size_t>(radius * radius * 4));

    for (int sum = -radius * 2; sum <= radius * 2; ++sum) {
        for (int dx = -radius; dx <= radius; ++dx) {
            const int dy = sum - dx;
            if (dy < -radius || dy > radius) {
                continue;
            }

            const int wx = pcx + dx;
            const int wy = pcy + dy;
            if (wx < 0 || wy < 0 || wx >= T4CV2WorldMap::kWorldSize || wy >= T4CV2WorldMap::kWorldSize) {
                continue;
            }

            const std::uint16_t tileId =
                map.GetFloorTileId(static_cast<unsigned>(wx), static_cast<unsigned>(wy));
            const std::string name = IsoSpriteName(tileId, static_cast<unsigned>(wx), static_cast<unsigned>(wy));
            if (!name.empty()) {
                unique.insert(name);
            }

            if (map.HasObjectLayer()) {
                const std::uint16_t objectId =
                    map.GetObjectTileId(static_cast<unsigned>(wx), static_cast<unsigned>(wy));
                if (T4CMapObjectSprites::ShouldDrawMapObject(objectId)) {
                    const std::string objName = T4CMapObjectSprites::ObjectSpriteName(
                        objectId, static_cast<unsigned>(wx), static_cast<unsigned>(wy));
                    if (!objName.empty()) {
                        unique.insert(objName);
                    }
                }
            }
        }
    }

    out.reserve(unique.size());
    for (const std::string &name : unique) {
        out.push_back(name);
    }
}

const char *T4CV2SpriteAtlas::TileSpriteBase(const std::uint16_t tileId) {
    switch (tileId) {
        case 5:
            return "NMRPF_NWood01";
        case 6:
            return "NMRPF_NWood02";
        case 7:
            return "NMRPF_NWood03";
        case 8:
            return "NMRPF_NWood04";
        case 51:
            return "NMRPF_OldLaveSolid";
        case 52:
            return "NMRPF_OldLaveMiSolid";
        case 53:
            return "NMRPF_OldLaveLiquid";
        case 54:
            return "NMRPF_LaveSolid";
        case 55:
            return "NMRPF_LaveMiSolid";
        case 56:
            return "NMRPF_LaveLiquid";
        case 59:
            return "NMRPF_RockTown0";
        case 60:
            return "NMRPF_RockTown1";
        case 61:
            return "NMRPF_RockTown2";
        case 62:
            return "NMRPF_RockTown3";
        case 63:
            return "NMRPF_Grass2Flower";
        case 64:
            return "NMRPF_Grass2Low";
        case 65:
            return "NMRPF_Grass2High";
        case 66:
            return "NMRPF_Grass2LowD";
        case 67:
            return "NMRPF_Grass2HighD";
        case 68:
            return "NMRPF_Earth2";
        case 69:
            return "NMRPF_Earth2Forest";
        case 70:
            return "NMRPF_Beach2";
        case 71:
            return "NMRPF_MountainHigh";
        case 72:
            return "NMRPF_MountainLow";
        case 73:
            return "NMRPF_Ocean2High";
        case 74:
            return "NMRPF_Ocean2Low";
        case 75:
            return "NMRPF_Ocean2Border";
        case 76:
            return "NMRPF_River2";
        case 77:
            return "NMRPF_Snow2";
        case 78:
            return "NMRPF_Snow2Foot";
        case 79:
            return "NMRPF_Earth2Frozen";
        case 80:
            return "NMRPF_Desert2";
        case 81:
            return "NMRPF_Ocean2Frozen";
        case 82:
            return "NMRPF_MountainGravel";
        case 83:
            return "NMRPF_Grass2Flower";
        case 84:
            return "NMRPF_Grass2High";
        case 85:
            return "NMRPF_Grass2Low";
        case 86:
            return "NMRPF_Earth2";
        case 87:
            return "WoodFloorN01";
        case 88:
            return "WoodFloorN02";
        case 89:
            return "WoodFloorN03";
        case 90:
            return "WoodFloorN04";
        case 91:
            return "WoodFloorN05";
        case 92:
            return "WoodFloorN06";
        case 93:
            return "WoodFloorN07";
        case 94:
            return "WoodFloorN08";
        case 95:
            return "NewTempleblack01";
        case 96:
            return "NewTempleblack02";
        case 97:
            return "NewTempleblack03";
        case 98:
            return "NewTempleblue01";
        case 99:
            return "NewTempleblue02";
        case 100:
            return "NewTempleblue03";
        case 101:
            return "NewTemplegreen01";
        case 102:
            return "NewTemplegreen02";
        case 103:
            return "NewTemplegreen03";
        case 104:
            return "NewTemplered01";
        case 105:
            return "NewTemplered02";
        case 106:
            return "NewTemplered03";
        case 107:
            return "NewTemplesand01";
        case 108:
            return "NewTemplesand02";
        case 109:
            return "NewTemplesand03";
        case 110:
            return "FDungeon01br";
        case 111:
            return "FDungeon01gr";
        case 112:
            return "FDungeon01brb";
        case 113:
            return "FDungeon01grb";
        case 114:
            return "FDungeon02br";
        case 115:
            return "FDungeon02gr";
        case 116:
            return "FDungeon02brb";
        case 117:
            return "FDungeon02grb";
        case 118:
            return "FDungeon03br";
        case 119:
            return "FDungeon03gr";
        case 120:
            return "FDungeon03brb";
        case 121:
            return "FDungeon03grb";
        case 122:
            return "FDungeon04br";
        case 123:
            return "FDungeon04gr";
        case 124:
            return "FDungeon04brb";
        case 125:
            return "FDungeon04grb";
        case 126:
            return "FSWP_Bouette";
        case 127:
            return "FSWP_Water";
        case 128:
            return "FSWP_WaterEarth";
        case 129:
            return "FSWP_WaterGreen";
        case 130:
            return "FSWP_WaterLGrass";
        case 131:
            return "FSWP_WaterHGrass";
        default:
            return nullptr;
    }
}

std::string T4CV2SpriteAtlas::IsoSpriteName(const std::uint16_t tileId, const unsigned int worldX,
                                            const unsigned int worldY) {
    const char *base = TileSpriteBase(tileId);
    if (!base) {
        return {};
    }
    char buf[128];
    const int ix = static_cast<int>(worldX % 16) + 1;
    const int iy = static_cast<int>(worldY % 16) + 1;
    std::snprintf(buf, sizeof(buf), "%s (%d, %d)", base, ix, iy);
    return buf;
}

bool T4CV2SpriteAtlas::TryDrawSpriteByName(SDL_Renderer *renderer, const std::string &spriteName,
                                           const float screenX, const float screenY,
                                           const bool flipHorizontal, const int paletteVariant,
                                           const bool omitShadow) const {
    if (!ready_ || !renderer || spriteName.empty()) {
        return false;
    }

    const CachedSprite *sprite = decodeAndCache(spriteName, true, paletteVariant, omitShadow);
    if (!sprite || !sprite->tex) {
        return false;
    }

    float tw = 0.f;
    float th = 0.f;
    if (!SDL_GetTextureSize(sprite->tex, &tw, &th)) {
        return false;
    }

    const int offX = flipHorizontal ? sprite->offX2 : sprite->offX;
    const int offY = flipHorizontal ? sprite->offY2 : sprite->offY;
    SDL_FRect dst{screenX + static_cast<float>(offX), screenY + static_cast<float>(offY), tw, th};
    if (flipHorizontal) {
        const SDL_FPoint pivot{dst.x + tw * 0.5f, dst.y + th * 0.5f};
        SDL_RenderTextureRotated(renderer, sprite->tex, nullptr, &dst, 0.0, &pivot, SDL_FLIP_HORIZONTAL);
    } else {
        SDL_RenderTexture(renderer, sprite->tex, nullptr, &dst);
    }
    return true;
}

bool T4CV2SpriteAtlas::TryDrawSpriteOutline(SDL_Renderer *renderer, const std::string &spriteName,
                                              const float screenX, const float screenY, const std::uint8_t r,
                                              const std::uint8_t g, const std::uint8_t b, const bool flipHorizontal,
                                              const int paletteVariant) const {
    if (!ready_ || !renderer || spriteName.empty()) {
        return false;
    }
    const CachedSprite *sprite = decodeAndCache(spriteName, true, paletteVariant);
    if (!sprite || !sprite->outlineTex) {
        return false;
    }
    float tw = 0.f;
    float th = 0.f;
    if (!SDL_GetTextureSize(sprite->outlineTex, &tw, &th)) {
        return false;
    }
    SDL_SetTextureColorMod(sprite->outlineTex, r, g, b);
    const int offX = flipHorizontal ? sprite->offX2 : sprite->offX;
    const int offY = flipHorizontal ? sprite->offY2 : sprite->offY;
    SDL_FRect dst{screenX + static_cast<float>(offX), screenY + static_cast<float>(offY), tw, th};
    if (flipHorizontal) {
        const SDL_FPoint pivot{dst.x + tw * 0.5f, dst.y + th * 0.5f};
        SDL_RenderTextureRotated(renderer, sprite->outlineTex, nullptr, &dst, 0.0, &pivot, SDL_FLIP_HORIZONTAL);
    } else {
        SDL_RenderTexture(renderer, sprite->outlineTex, nullptr, &dst);
    }
    return true;
}

bool T4CV2SpriteAtlas::TryDrawTile(SDL_Renderer *renderer, const std::uint16_t tileId, const unsigned int worldX,
                                   const unsigned int worldY, const float screenX, const float screenY) const {
    if (!ready_ || !renderer) {
        return false;
    }

    const std::string name = IsoSpriteName(tileId, worldX, worldY);
    if (name.empty()) {
        return false;
    }

    return TryDrawSpriteByName(renderer, name, screenX, screenY);
}

bool T4CV2SpriteAtlas::TryDrawMapObject(SDL_Renderer *renderer, const std::uint16_t objectId,
                                        const unsigned int worldX, const unsigned int worldY, const float screenX,
                                        const float screenY) const {
    if (!ready_ || !renderer || !T4CMapObjectSprites::ShouldDrawMapObject(objectId)) {
        return false;
    }

    bool mirror = false;
    const std::string name = T4CMapObjectSprites::ObjectSpriteName(objectId, worldX, worldY, &mirror);
    if (name.empty()) {
        return false;
    }

    return TryDrawSpriteByName(renderer, name, screenX, screenY, mirror);
}
