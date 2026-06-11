#pragma once

#include <cstdint>
#include <string>
#include <vector>

/** Objet inventaire / coffre (opcode 18 / 106, aligne BAG_ITEM Windows). */
struct T4CBagItem {
    std::uint16_t appearance{0};
    std::int32_t objectId{0};
    std::uint16_t baseId{0};
    std::uint32_t qty{0};
    std::int32_t charges{0};
    /** Nom serveur (opcode 59) ; vide tant que non recu. */
    std::string name;
    /** Requete 59 en vol pour cet objectId. */
    bool nameQueryPending{false};
};

/** Ou chercher l'objet cote serveur (PacketTypes.h PL_SEARCH*). */
enum class T4CItemSearchPlace : unsigned char {
    Backpack = 0,
    BankChest = 1,
};

struct T4CPlayerSkill {
    std::uint16_t skillId{0};
    unsigned char useMode{0};
    std::uint16_t points{0};
    std::uint16_t truePoints{0};
    std::string name;
    std::string description;
};

struct T4CPlayerSpell {
    std::uint16_t spellId{0};
    unsigned char targetType{0};
    std::uint16_t manaCost{0};
    std::int32_t duration{0};
    std::uint16_t level{0};
    std::uint16_t element{0};
    std::uint16_t damageType{0};
    std::int32_t icon{0};
    std::string name;
    std::string description;
};

struct T4CPlayerBackpack {
    bool showUi{false};
    std::int32_t containerId{0};
    std::vector<T4CBagItem> items;
    bool valid{false};
};

struct T4CPlayerBankChest {
    bool uiVisible{false};
    std::vector<T4CBagItem> items;
    bool valid{false};
};

struct T4CPlayerSkillBook {
    std::vector<T4CPlayerSkill> skills;
    bool valid{false};
};

struct T4CPlayerSpellBook {
    unsigned short mana{0};
    unsigned short maxMana{0};
    std::vector<T4CPlayerSpell> spells;
    bool valid{false};
};

/** Slot equipement (opcode 19, ordre serveur Character::packet_equiped). */
enum class T4CEquipSlot : unsigned char {
    Body = 0,
    Gloves = 1,
    Helm = 2,
    Legs = 3,
    Bracelets = 4,
    Necklace = 5,
    WeaponRight = 6,
    WeaponLeft = 7,
    Ring1 = 8,
    Ring2 = 9,
    Belt = 10,
    Cape = 11,
    Feet = 12,
};

struct T4CEquippedItem {
    T4CEquipSlot slot{T4CEquipSlot::Body};
    std::int32_t objectId{0};
    std::uint16_t appearance{0};
    std::uint16_t baseId{0};
    std::uint16_t qty{0};
    std::int32_t charges{0};
    std::string name;
};

struct T4CPlayerEquipment {
    bool rangedAttack{false};
    std::vector<T4CEquippedItem> items;
    bool valid{false};
};
