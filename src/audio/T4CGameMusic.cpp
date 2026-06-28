#include "audio/T4CGameMusic.h"

#include "audio/T4CGameMusicZone.h"
#include "runtime/T4CRuntimePaths.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace {

struct MusicStreamState {
    SDL_AudioStream *stream{nullptr};
    Uint8 *wavData{nullptr};
    Uint32 wavLen{0};
    size_t readPos{0};
    SDL_AudioSpec spec{};
};

std::uint32_t ReadLE32(const Uint8 *p) {
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}
std::uint16_t ReadLE16(const Uint8 *p) {
    return static_cast<std::uint16_t>(p[0]) | (static_cast<std::uint16_t>(p[1]) << 8);
}

/* Chargeur WAV tolerant : les FX Files T4C ont un header PCM correct sauf `block align`/`byte rate`
 * non conformes (ex. block align=2 pour du 8-bit mono) que SDL_LoadWAV rejette ("Unsupported block
 * alignment"). On parse RIFF/fmt/data nous-memes en ignorant ces champs. Buffer alloue via SDL_malloc
 * (liberable par SDL_free comme SDL_LoadWAV). */
bool LoadWavLenient(const char *path, SDL_AudioSpec *outSpec, Uint8 **outData, Uint32 *outLen) {
    size_t fileLen = 0;
    Uint8 *file = static_cast<Uint8 *>(SDL_LoadFile(path, &fileLen));
    if (!file) {
        return false;
    }
    bool ok = false;
    if (fileLen >= 12 && std::memcmp(file, "RIFF", 4) == 0 && std::memcmp(file + 8, "WAVE", 4) == 0) {
        std::uint16_t channels = 1;
        std::uint32_t freq = 22050;
        std::uint16_t bits = 8;
        std::uint16_t fmtTag = 1;
        const Uint8 *data = nullptr;
        std::uint32_t dataLen = 0;
        size_t pos = 12;
        while (pos + 8 <= fileLen) {
            const Uint8 *id = file + pos;
            const std::uint32_t chunkLen = ReadLE32(file + pos + 4);
            const size_t body = pos + 8;
            if (std::memcmp(id, "fmt ", 4) == 0 && body + 16 <= fileLen) {
                fmtTag = ReadLE16(file + body);
                channels = ReadLE16(file + body + 2);
                freq = ReadLE32(file + body + 4);
                bits = ReadLE16(file + body + 14);
            } else if (std::memcmp(id, "data", 4) == 0) {
                data = file + body;
                dataLen = static_cast<std::uint32_t>(std::min<size_t>(chunkLen, fileLen - body));
            }
            pos = body + chunkLen + (chunkLen & 1);  // chunks alignes sur 2 octets
        }
        if (fmtTag == 1 && data && dataLen > 0 && channels > 0) {
            SDL_AudioSpec spec{};
            spec.format = (bits == 16) ? SDL_AUDIO_S16 : SDL_AUDIO_U8;
            spec.channels = channels;
            spec.freq = static_cast<int>(freq > 0 ? freq : 22050);
            Uint8 *copy = static_cast<Uint8 *>(SDL_malloc(dataLen));
            if (copy) {
                std::memcpy(copy, data, dataLen);
                *outSpec = spec;
                *outData = copy;
                *outLen = dataLen;
                ok = true;
            }
        }
    }
    SDL_free(file);
    return ok;
}

std::mutex g_musicMutex;
MusicStreamState g_music{};
int g_currentTrackId = -1;
int g_oldTrackId = -1;
float g_volumeGain = 0.75f;
float g_sfxVolumeGain = 0.75f;
bool g_audioReady = false;

void FreeWav(MusicStreamState &st) {
    if (st.wavData) {
        SDL_free(st.wavData);
        st.wavData = nullptr;
    }
    st.wavLen = 0;
    st.readPos = 0;
}

void CloseStream(MusicStreamState &st) {
    if (st.stream) {
        SDL_DestroyAudioStream(st.stream);
        st.stream = nullptr;
    }
}

void SDLCALL MusicStreamCallback(void *userdata, SDL_AudioStream *stream, int additional_amount,
                                 int /*total_amount*/) {
    auto *st = static_cast<MusicStreamState *>(userdata);
    if (!st || !st->wavData || additional_amount <= 0) {
        return;
    }

    int remaining = additional_amount;
    while (remaining > 0) {
        if (st->readPos >= st->wavLen) {
            st->readPos = 0;
        }
        const int avail = static_cast<int>(st->wavLen - st->readPos);
        if (avail <= 0) {
            break;
        }
        const int chunk = std::min(remaining, avail);
        if (!SDL_PutAudioStreamData(stream, st->wavData + st->readPos, chunk)) {
            SDL_Log("[GameMusic] SDL_PutAudioStreamData: %s", SDL_GetError());
            break;
        }
        st->readPos += static_cast<size_t>(chunk);
        remaining -= chunk;
    }
}

std::string SonsWavPath(const char *baseName) {
    if (!baseName || !baseName[0]) {
        return {};
    }
    std::string path = T4CRuntimePath("Music Files");
    if (path.empty()) {
        return {};
    }
    path += '/';
    path += baseName;
    path += ".wav";
    return path;
}

bool LoadWavFile(const std::string &path, MusicStreamState &st) {
    FreeWav(st);
    if (path.empty()) {
        return false;
    }
    if (!SDL_LoadWAV(path.c_str(), &st.spec, &st.wavData, &st.wavLen) &&
        !LoadWavLenient(path.c_str(), &st.spec, &st.wavData, &st.wavLen)) {
        SDL_Log("[GameMusic] SDL_LoadWAV echoue: %s (%s)", path.c_str(), SDL_GetError());
        return false;
    }
    st.readPos = 0;
    return true;
}

bool OpenMusicStream(MusicStreamState &st) {
    if (!st.wavData || st.wavLen == 0) {
        return false;
    }
    st.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &st.spec,
                                          MusicStreamCallback, &st);
    if (!st.stream) {
        SDL_Log("[GameMusic] SDL_OpenAudioDeviceStream: %s", SDL_GetError());
        return false;
    }
    SDL_SetAudioStreamGain(st.stream, g_volumeGain);
    if (!SDL_ResumeAudioStreamDevice(st.stream)) {
        SDL_Log("[GameMusic] SDL_ResumeAudioStreamDevice: %s", SDL_GetError());
        CloseStream(st);
        return false;
    }
    return true;
}

void PlayTrackId(int trackId) {
    CloseStream(g_music);

    if (trackId == T4C_MUSIC_SILENCE) {
        FreeWav(g_music);
        g_currentTrackId = T4C_MUSIC_SILENCE;
        SDL_Log("[GameMusic] silence");
        return;
    }

    const char *baseName = T4CGameMusicTrackBaseName(trackId);
    if (!baseName) {
        SDL_Log("[GameMusic] piste inconnue id=%d", trackId);
        return;
    }

    const std::string path = SonsWavPath(baseName);
    if (!LoadWavFile(path, g_music)) {
        return;
    }
    if (!OpenMusicStream(g_music)) {
        FreeWav(g_music);
        return;
    }

    g_currentTrackId = trackId;
    SDL_Log("[GameMusic] lecture %s", path.c_str());
}

}  // namespace

bool T4CGameMusic::Init() {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    if (g_audioReady) {
        return true;
    }
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        SDL_Log("[GameMusic] SDL_INIT_AUDIO: %s", SDL_GetError());
        return false;
    }
    g_audioReady = true;
    g_currentTrackId = -1;
    g_oldTrackId = -1;
    SDL_Log("[GameMusic] init OK");
    return true;
}

void T4CGameMusic::StartCharacterSelect() {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    if (!g_audioReady) {
        return;
    }
    g_oldTrackId = -1;
    PlayTrackId(T4C_MUSIC_SADNESS);
    g_oldTrackId = g_currentTrackId;
}

void T4CGameMusic::LoadNewSound(const std::uint16_t world, const std::uint32_t x, const std::uint32_t y,
                                const std::uint32_t playerLevel) {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    if (!g_audioReady) {
        return;
    }

    const int track = T4CGameMusicPickTrack(world, x, y, static_cast<int>(playerLevel));
    if (track == g_oldTrackId) {
        return;
    }

    g_oldTrackId = track;
    PlayTrackId(track);
}

void T4CGameMusic::Stop() {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    CloseStream(g_music);
    FreeWav(g_music);
    g_currentTrackId = -1;
    g_oldTrackId = -1;
}

void T4CGameMusic::Reset() {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    g_oldTrackId = -1;
}

void T4CGameMusic::SetVolume(const float gain0to1) {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    g_volumeGain = std::clamp(gain0to1, 0.0f, 1.0f);
    if (g_music.stream) {
        SDL_SetAudioStreamGain(g_music.stream, g_volumeGain);
    }
}

float T4CGameMusic::GetVolume() {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    return g_volumeGain;
}

void T4CGameMusic::SetSfxVolume(const float gain0to1) {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    g_sfxVolumeGain = std::clamp(gain0to1, 0.0f, 1.0f);
}

float T4CGameMusic::GetSfxVolume() {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    return g_sfxVolumeGain;
}

namespace {

struct SfxStreamState {
    SDL_AudioStream *stream{nullptr};
    Uint8 *wavData{nullptr};
    Uint32 wavLen{0};
    size_t readPos{0};
    SDL_AudioSpec spec{};
    bool finished{false};
};

std::vector<std::unique_ptr<SfxStreamState>> g_sfxStreams;

void FreeSfxWav(SfxStreamState &st) {
    if (st.wavData) {
        SDL_free(st.wavData);
        st.wavData = nullptr;
    }
    st.wavLen = 0;
    st.readPos = 0;
}

void CloseSfxStream(SfxStreamState &st) {
    if (st.stream) {
        SDL_DestroyAudioStream(st.stream);
        st.stream = nullptr;
    }
}

void SDLCALL SfxStreamCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int /*total_amount*/) {
    auto *st = static_cast<SfxStreamState *>(userdata);
    if (!st || !st->wavData || additional_amount <= 0 || st->finished) {
        return;
    }

    int remaining = additional_amount;
    while (remaining > 0) {
        if (st->readPos >= st->wavLen) {
            st->finished = true;
            SDL_PauseAudioStreamDevice(stream);
            break;
        }
        const int avail = static_cast<int>(st->wavLen - st->readPos);
        const int chunk = std::min(remaining, avail);
        if (!SDL_PutAudioStreamData(stream, st->wavData + st->readPos, chunk)) {
            break;
        }
        st->readPos += static_cast<size_t>(chunk);
        remaining -= chunk;
    }
}

void CleanupFinishedSfx() {
    for (auto it = g_sfxStreams.begin(); it != g_sfxStreams.end();) {
        if ((*it) && (*it)->finished) {
            CloseSfxStream(**it);
            FreeSfxWav(**it);
            it = g_sfxStreams.erase(it);
        } else {
            ++it;
        }
    }
}

std::string SfxWavPath(const char *baseName) {
    if (!baseName || !baseName[0]) {
        return {};
    }
    std::string path = T4CRuntimePath("FX Files");
    if (path.empty()) {
        return {};
    }
    path += '/';
    path += baseName;
    path += ".wav";
    return path;
}

const char *PickCreatureAttackSfx(const char *spriteBase) {
    if (!spriteBase || !spriteBase[0]) {
        return "Whooshh 2";
    }
    if (std::strcmp(spriteBase, "Goblin") == 0 || std::strcmp(spriteBase, "GoblinBoss") == 0 ||
        std::strcmp(spriteBase, "NMS_GoblinNoel") == 0) {
        return (SDL_GetTicks() & 1) ? "Whooshh 2" : "Whooshm 6";
    }
    static const struct {
        const char *base;
        const char *sfx;
    } kAttackTable[] = {
        {"Bat", "Bat Attack"},       {"Beast", "Beast Attack"},   {"Beholder", "Beholder Attack"},
        {"Demon", "Demon Attack"},   {"Minotaur", "Minotaur Attack"}, {"Mummy", "Mummy Attack"},
        {"Kraanian", "Kraanian Attack"}, {"Atrocity", "Atrocity Attack"}, {"Elemear", "Elemear Attack"},
    };
    for (const auto &row : kAttackTable) {
        if (std::strcmp(spriteBase, row.base) == 0) {
            return row.sfx;
        }
    }
    return "Whooshh 2";
}

const char *PickCreatureHitSfx(const char *spriteBase) {
    if (!spriteBase || !spriteBase[0]) {
        return "Male Hit 1";
    }
    if (std::strcmp(spriteBase, "Goblin") == 0 || std::strcmp(spriteBase, "GoblinBoss") == 0 ||
        std::strcmp(spriteBase, "NMS_GoblinNoel") == 0) {
        return "Goblin Hit";
    }
    static const struct {
        const char *base;
        const char *sfx;
    } kHitTable[] = {
        {"Bat", "Bat Hit"},         {"Beast", "Beast Hit"},       {"Beholder", "Beholder Hit"},
        {"Demon", "Demon Hit"},     {"Minotaur", "Minotaur Hit"}, {"Mummy", "Mummy Hit"},
        {"Kraanian", "Kraanian Hit"}, {"Atrocity", "Atrocity Hit"}, {"Kobold", "Kobold Hit"},
        {"Centaur", "Centaur Hit"}, {"Rat", "Rat Hit"},           {"Spider", "Spider Hit"},
        {"Orc", "Orc Hit"},         {"Zombie", "Zombie Hit"},     {"Goblin", "Goblin Hit"},
        {"Wolf", "Wolf Hit"},
    };
    for (const auto &row : kHitTable) {
        if (std::strcmp(spriteBase, row.base) == 0) {
            return row.sfx;
        }
    }
    return "Male Hit 1";
}

}  // namespace

void T4CGameMusic::PlaySfx(const char *baseName) {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    if (!g_audioReady || !baseName || !baseName[0]) {
        return;
    }
    CleanupFinishedSfx();
    while (g_sfxStreams.size() >= 8) {
        g_sfxStreams.erase(g_sfxStreams.begin());
    }

    auto st = std::make_unique<SfxStreamState>();
    const std::string path = SfxWavPath(baseName);
    Uint8 *rawWav = nullptr;
    Uint32 rawLen = 0;
    SDL_AudioSpec srcSpec{};
    if (path.empty() || (!SDL_LoadWAV(path.c_str(), &srcSpec, &rawWav, &rawLen) &&
                          !LoadWavLenient(path.c_str(), &srcSpec, &rawWav, &rawLen))) {
        SDL_Log("[GameMusic] SFX introuvable: %s (%s)", baseName, SDL_GetError());
        return;
    }
    SDL_AudioSpec dstSpec{};
    dstSpec.format = SDL_AUDIO_S16;
    dstSpec.channels = 1;
    dstSpec.freq = srcSpec.freq > 0 ? srcSpec.freq : 22050;
    Uint8 *converted = nullptr;
    int convertedLen = 0;
    if (!SDL_ConvertAudioSamples(&srcSpec, rawWav, static_cast<int>(rawLen), &dstSpec, &converted,
                                 &convertedLen)) {
        SDL_Log("[GameMusic] SFX conversion: %s (%s)", baseName, SDL_GetError());
        SDL_free(rawWav);
        return;
    }
    SDL_free(rawWav);
    st->spec = dstSpec;
    st->wavData = converted;
    st->wavLen = static_cast<Uint32>(convertedLen);
    SfxStreamState *raw = st.get();
    raw->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &raw->spec, SfxStreamCallback, raw);
    if (!raw->stream) {
        FreeSfxWav(*raw);
        return;
    }
    SDL_SetAudioStreamGain(raw->stream, g_sfxVolumeGain);
    if (!SDL_ResumeAudioStreamDevice(raw->stream)) {
        CloseSfxStream(*raw);
        FreeSfxWav(*raw);
        return;
    }
    g_sfxStreams.push_back(std::move(st));
}

void T4CGameMusic::PlayCreatureAttackSfx(const char *spriteBase) {
    PlaySfx(PickCreatureAttackSfx(spriteBase));
}

void T4CGameMusic::PlayCreatureHitSfx(const char *spriteBase) {
    PlaySfx(PickCreatureHitSfx(spriteBase));
}

void T4CGameMusic::PlayPlayerHitSfx() {
    PlaySfx((SDL_GetTicks() & 1) ? "Male Hit 1" : "Male Hit 2");
}

void T4CGameMusic::Shutdown() {
    std::lock_guard<std::mutex> lock(g_musicMutex);
    for (const auto &st : g_sfxStreams) {
        if (st) {
            CloseSfxStream(*st);
            FreeSfxWav(*st);
        }
    }
    g_sfxStreams.clear();
    CloseStream(g_music);
    FreeWav(g_music);
    g_currentTrackId = -1;
    g_oldTrackId = -1;
    if (g_audioReady) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        g_audioReady = false;
    }
}
