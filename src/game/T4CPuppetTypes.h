#pragma once

#include <cstdint>

#include "game/T4CPuppetDraw.h"

/** Slots calques — Puppet.h client Windows. */
enum T4CPupSlot : std::uint8_t {
    kPupHandLeft = 0,
    kPupArmLeft = 1,
    kPupFoot = 2,
    kPupLegs = 3,
    kPupBody = 4,
    kPupHead = 5,
    kPupHandRight = 6,
    kPupArmRight = 7,
    kPupWeapon = 8,
    kPupShield = 9,
    kPupBoot = 10,
    kPupHat = 11,
    kPupCape = 12,
    kPupBackBody = 13,
    kPupCape2 = 14,
    kPupHair = 15,
    kPupRobeLegs = 16,
    kPupMask = 17,
    kPupWeapon2 = 18,
    kPupSlotCount = 19,
};

/** Equipement resolu par slot (PuppetInfo[19] apres Puppetize). */
struct T4CPuppetInfo {
    std::uint8_t slot[kPupSlotCount]{};
};

/** Convertit OBJGROUP serveur -> PuppetInfo (VisualObjectList::Puppetize). */
void T4CPuppetizeFromDress(const T4CPuppetDress &dress, bool male, T4CPuppetInfo *out);

/** Resout un sprite 3D par slot ; nullptr si partie masquee. */
const char *T4CPuppetResolveSprite(T4CPupSlot slot, std::uint8_t pupeq, bool female);
