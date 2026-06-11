#include "audio/T4CGameMusicZone.h"

namespace {

#define Track45(a, b, c, d) ((Y >= -X + (a)) && (Y >= X + (b)) && (Y <= -X + (c)) && (Y <= X + (d)))
#define Track90(a, b, c, d) ((X >= (a) && X <= (b)) && (Y >= (c) && Y <= (d)))

constexpr int kMainWorld = 0;
constexpr int kDungeonWorld = 1;
constexpr int kCavernWorld = 2;
constexpr int kEditorWorld = 3;

}  // namespace

int T4CGameMusicPickTrack(const unsigned short world, const unsigned int x, const unsigned int y,
                          const int playerLevel) {
    const int X = static_cast<int>(x);
    const int Y = static_cast<int>(y);
    int music = T4C_MUSIC_FOREST;

    switch (world) {
        case kMainWorld:
            music = T4C_MUSIC_FOREST;
            break;
        case kDungeonWorld:
            music = T4C_MUSIC_DUNGEON;
            break;
        case kCavernWorld:
            music = T4C_MUSIC_CAVERN;
            break;
        default:
            music = T4C_MUSIC_FOREST;
            break;
    }

    if (world == kEditorWorld) {
        music = T4C_MUSIC_FOREST;
        if (Track90(0, 1536, 1535, 3071)) {
            music = T4C_MUSIC_DUNGEON;
        }
        if (Track90(1536, 1536, 3071, 3071)) {
            music = T4C_MUSIC_CAVERN;
        }
    }

    if (world == kDungeonWorld) {
        if (((X >= 135 && X <= 285) && (Y >= 410 && Y <= 560)) ||
            ((X >= 960 && X <= 1185) && (Y >= 75 && Y <= 300))) {
            music = T4C_MUSIC_BOSS;
        }
    }

    if (world == kMainWorld) {
        if (Track45(823, 407, 1135, 723) || Track45(1136, 406, 1208, 664) || Track45(568, 248, 712, 312) ||
            Track90(993, 1023, 945, 951) || Track90(952, 1029, 952, 957) || Track90(939, 1034, 958, 963) ||
            Track90(904, 1040, 964, 969) || Track90(885, 1063, 970, 978) || Track90(873, 1070, 979, 987) ||
            Track90(865, 1093, 988, 1001) || Track90(853, 1240, 1002, 1087) || Track90(844, 1255, 1088, 1320)) {
            music = T4C_MUSIC_OUTDOORS;
        }
        if (Track45(555, -115, 625, -45)) {
            music = T4C_MUSIC_BOSS;
        }
        if (Track45(1571, 807, 1699, 933)) {
            music = T4C_MUSIC_SADNESS;
        }
        if (Track45(879, -777, 1017, -547) || Track45(1018, -744, 1072, -602) || Track45(1720, -624, 1818, -612) ||
            Track45(1437, -695, 1883, -625) || Track45(1460, -732, 1890, -696) ||
            Track45(1521, -761, 1891, -733) || Track45(1546, -802, 1908, -762) ||
            Track45(1575, -833, 1915, -803)) {
            music = T4C_MUSIC_CAVERN;
        }
        if (Track45(1222, -134, 1394, 36) || Track90(2552, 2975, 1874, 2138)) {
            music = T4C_MUSIC_NOISES;
        }
        if (Track90(1622, 1814, 1745, 1938) || Track90(2696, 2847, 2193, 2607)) {
            music = T4C_MUSIC_DUNGEON;
        }
        if (((X >= 2928 && X <= 3022) && (Y >= 908 && Y <= 966)) ||
            ((X >= 2896 && X <= 3022) && (Y >= 967 && Y <= 1020)) ||
            ((X >= 2785 && X <= 3022) && (Y >= 1021 && Y <= 1026)) ||
            ((X >= 2779 && X <= 3022) && (Y >= 1027 && Y <= 1030)) ||
            ((X >= 2761 && X <= 3022) && (Y >= 1031 && Y <= 1046)) ||
            ((X >= 2744 && X <= 3022) && (Y >= 1047 && Y <= 1060)) ||
            ((X >= 2730 && X <= 3022) && (Y >= 1061 && Y <= 1075)) ||
            ((X >= 2725 && X <= 3022) && (Y >= 1076 && Y <= 1275))) {
            music = T4C_MUSIC_OUTDOORS;
        }
        if (((Y >= -X + 2767) && (Y >= X - 567) && (Y <= -X + 2947) && (Y <= X - 317)) ||
            ((Y >= -X + 2948) && (Y >= X - 566) && (Y <= -X + 3112) && (Y <= X - 302))) {
            music = T4C_MUSIC_OUTDOORS;
        }
        if ((Y >= -X + 3848) && (Y >= X - 1722) && (Y <= -X + 3902) && (Y <= X - 1662)) {
            music = T4C_MUSIC_CAVERN;
        }
        if ((Y >= -X + 3903) && (Y >= X - 1701) && (Y <= -X + 3907) && (Y <= X - 1687)) {
            music = T4C_MUSIC_CAVERN;
        }
        if ((Y >= -X + 3673) && (Y >= X + 841) && (Y <= -X + 3791) && (Y <= X + 1103)) {
            music = T4C_MUSIC_OUTDOORS;
        }
        if ((Y >= -X + 3792) && (Y >= X + 778) && (Y <= -X + 3858) && (Y <= X + 1104)) {
            music = T4C_MUSIC_OUTDOORS;
        }
        if ((Y >= -X + 3859) && (Y >= X + 759) && (Y <= -X + 3951) && (Y <= X + 1103)) {
            music = T4C_MUSIC_OUTDOORS;
        }
        if ((Y >= -X + 3952) && (Y >= X + 796) && (Y <= -X + 4036) && (Y <= X + 1104)) {
            music = T4C_MUSIC_OUTDOORS;
        }
        if ((Y >= -X + 4037) && (Y >= X + 821) && (Y <= -X + 4311) && (Y <= X + 1105)) {
            music = T4C_MUSIC_OUTDOORS;
        }
        if ((Y >= -X + 1950) && (Y >= X + 1410) && (Y <= -X + 1970) && (Y <= X + 1430)) {
            music = T4C_MUSIC_SADNESS;
        }
        if ((Y >= -X + 1950) && (Y >= X + 1260) && (Y <= -X + 1970) && (Y <= X + 1280)) {
            music = T4C_MUSIC_SADNESS;
        }
        if ((Y >= -X + 2090) && (Y >= X + 1260) && (Y <= -X + 2110) && (Y <= X + 1280)) {
            music = T4C_MUSIC_SADNESS;
        }
        if ((Y >= -X + 2090) && (Y >= X + 1410) && (Y <= -X + 2110) && (Y <= X + 1430)) {
            music = T4C_MUSIC_SADNESS;
        }
        if ((Y >= -X + 1961) && (Y >= X + 1281) && (Y <= -X + 2099) && (Y <= X + 1409)) {
            music = T4C_MUSIC_SADNESS;
        }
        if ((Y >= -X + 2535) && (Y >= X + 1645) && (Y <= -X + 2605) && (Y <= X + 1715)) {
            music = T4C_MUSIC_BOSS;
        }
    }

    if (world == kDungeonWorld && playerLevel <= 1) {
        (void)playerLevel;
    }

    return music;
}

const char *T4CGameMusicTrackBaseName(const int trackId) {
    switch (trackId) {
        case T4C_MUSIC_BOSS:
            return "Boss Music";
        case T4C_MUSIC_OUTDOORS:
            return "Outdoors Music";
        case T4C_MUSIC_FOREST:
            return "Forest Music";
        case T4C_MUSIC_DUNGEON:
            return "Dungeons Music";
        case T4C_MUSIC_CAVERN:
            return "Caverns Music";
        case T4C_MUSIC_SADNESS:
            return "Sadness Music";
        case T4C_MUSIC_NOISES:
            return "Noises Music";
        default:
            return nullptr;
    }
}
