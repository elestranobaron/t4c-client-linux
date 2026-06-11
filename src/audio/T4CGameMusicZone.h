#pragma once

/** Identifiants pistes — ordre aligne GameMusic.cpp Windows. */
enum T4CGameMusicTrack {
    T4C_MUSIC_BOSS = 0,
    T4C_MUSIC_OUTDOORS = 1,
    T4C_MUSIC_FOREST = 2,
    T4C_MUSIC_DUNGEON = 3,
    T4C_MUSIC_CAVERN = 4,
    T4C_MUSIC_SADNESS = 5,
    T4C_MUSIC_SILENCE = 6,
    T4C_MUSIC_NOISES = 7,
};

/** Choisit la piste selon monde + coords (copie GameMusic.cpp). */
int T4CGameMusicPickTrack(unsigned short world, unsigned int x, unsigned int y, int playerLevel);

/** Nom VSB / fichier WAV sans extension (« Forest Music », …). */
const char *T4CGameMusicTrackBaseName(int trackId);
