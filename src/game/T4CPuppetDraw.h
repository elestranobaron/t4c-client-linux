#pragma once

#include "assets/map/T4CV2SpriteAtlas.h"

#include <SDL3/SDL.h>

#include <cstdint>
#include <string>
#include <vector>

/** Equipement puppet tel qu'envoye par RQ_PuppetInformation (68) — __OBJGROUP_* serveur. */
struct T4CPuppetDress {
    std::uint16_t body{0};
    std::uint16_t feet{0};
    std::uint16_t gloves{0};
    std::uint16_t helm{0};
    std::uint16_t legs{0};
    std::uint16_t weaponR{0};
    std::uint16_t weaponL{0};
    std::uint16_t cape{0};
    bool known{false};
};

struct T4CActivePlayer;

/** Dessine un puppet 3D (19 calques, BodyOrderR) — tables generees depuis Puppet.cpp. */
/** Contour FX_OUTLINE sur les 19 calques puppet (meme BodyOrderR que le corps). */
bool T4CPuppetTryDrawOutline(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const T4CPuppetDress &dress,
                             const bool female, const int direction, const int frameIndex, const float screenX,
                             const float screenY, const bool attacking, const std::uint8_t r, const std::uint8_t g,
                             const std::uint8_t b);

bool T4CPuppetTryDraw(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const T4CPuppetDress &dress,
                      bool female, int direction, int frameIndex, float screenX, float screenY,
                      bool attacking = false);

bool T4CPuppetTryDrawPlayer(const T4CV2SpriteAtlas &atlas, SDL_Renderer *renderer, const T4CActivePlayer &player,
                            int direction, int frameIndex, float screenX, float screenY, bool attacking = false);

/** Noms de sprites uniques a precharger (kPuppetResolveRows) — eviter au chargement monde. */
void T4CPuppetCollectPreloadBases(std::vector<std::string> *out);

/** Precharge uniquement les calques d'une tenue (joueur actif / defaut). */
void T4CPuppetAppendDressPreload(const T4CPuppetDress &dress, bool female, std::vector<std::string> *out);

/** Sprite NPC simple si le puppet 3D echoue (opcode 68 pas encore recu, budget decode, etc.). */
const char *T4CPuppetFallbackSpriteName(const T4CPuppetDress &dress, bool female);
