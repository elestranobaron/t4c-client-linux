#pragma once

#include <cstdint>

/** Musique de fond T4C — equivalent minimal de g_GameMusic (DirectSound → SDL3 audio). */
class T4CGameMusic {
   public:
    static bool Init();
    static void Shutdown();

    /** Ecran selection / creation perso — « Sadness Music » (TFCSocket menu loop). */
    static void StartCharacterSelect();

    /** Piste selon monde + coords ; ne recharge pas si la piste est deja active. */
    static void LoadNewSound(std::uint16_t world, std::uint32_t x, std::uint32_t y, std::uint32_t playerLevel);

    static void Stop();
    /** Force le prochain LoadNewSound a recharger (teleport Windows : Reset avant LoadNewSound). */
    static void Reset();

    static void SetVolume(float gain0to1);
    static float GetVolume();

    static void SetSfxVolume(float gain0to1);
    static float GetSfxVolume();

    /** Effet one-shot depuis runtime/FX Files/<name>.wav (combat, portes, etc.). */
    static void PlaySfx(const char *baseName);

    /** Whoosh attaque monstre — sprite base Icon3D (Goblin, Bat, …). */
    static void PlayCreatureAttackSfx(const char *spriteBase);

    /** Hit monstre touche. */
    static void PlayCreatureHitSfx(const char *spriteBase);

    /** Joueur local touche — Male/Female Hit (SetAttack Victim). */
    static void PlayPlayerHitSfx();

   private:
    T4CGameMusic() = delete;
};
