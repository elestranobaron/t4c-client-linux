#pragma once

#include <cstdint>
#include <string>
#include <vector>

/** Bulle de dialogue PNJ (opcode 27 RQ_IndirectTalk). */
struct T4CNpcSpeech {
    std::int32_t unitId{0};
    std::string text;
    std::string speakerName;
    int direction{2};
    bool isNpc{true};
    /** Liens [Buy]/[Train] extraits du texte T4C. */
    std::vector<std::string> links;
};

/** Article boutique (opcodes 41 buy / 56 sell). */
struct T4CShopEntry {
    std::int32_t objectId{0};
    std::uint16_t appearance{0};
    std::uint32_t price{0};
    std::uint32_t maxQty{0};
    std::string name;
    std::string requirements;
    bool canEquip{false};
};

enum class T4CShopMode : std::uint8_t {
    None,
    Buy,
    Sell,
    Train,
    Learn,
};

struct T4CShopList {
    T4CShopMode mode{T4CShopMode::None};
    std::uint32_t gold{0};
    std::uint16_t skillPoints{0};
    std::vector<T4CShopEntry> items;
    bool valid{false};
};

/** Etat conversation active (TalkTo* Windows). */
struct T4CTalkState {
    std::int32_t unitId{0};
    unsigned int x{0};
    unsigned int y{0};
    bool active{false};
};
