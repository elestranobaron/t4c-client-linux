#pragma once

#include <string>

/**
 * Zones nommées du monde (Game Files/Zone_Map.dat + zonemapinfo.zfn).
 * Reproduit Global::LoadZoneMapWorld / ForceDisplayZone du client Windows :
 * un octet zone-id par tuile (3072×3072, zlib par monde) + table de noms XOR.
 */
class T4CZoneMap {
   public:
    static constexpr int kMapSize = 3072;
    static constexpr int kWorldCount = 8;

    /** Charge les ids de zone d'un monde + la table de noms (une fois). */
    bool LoadWorld(unsigned short worldIndex);

    void Clear();

    bool IsLoaded() const { return loaded_; }

    /** Id de zone (0-255) a une tuile ; 0xFF = aucune. */
    int ZoneIdAt(unsigned int worldX, unsigned int worldY) const;

    /** Nom de zone a une tuile ; chaine vide si zone muette. */
    std::string ZoneNameAt(unsigned int worldX, unsigned int worldY) const;

   private:
    bool loadZoneNames();

    bool loaded_{false};
    unsigned short worldIndex_{0};
    std::string zoneIds_;  // kMapSize*kMapSize octets
    std::string zoneNames_[256];
    bool namesLoaded_{false};
};
