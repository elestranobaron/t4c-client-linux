#pragma once

#include <SDL3/SDL.h>

#include <cstdint>
#include <string>
#include <vector>

/** Carte monde 3072×3072 (indices palette) depuis TMI_Map.dat — etape 2a Phase 2. */
class T4CTmiWorldMap {
   public:
    static constexpr int kMapSize = 3072;
    static constexpr int kWorldCount = 8;

    T4CTmiWorldMap() = default;

    bool LoadWorld(unsigned short worldIndex);
    void Clear();

    bool IsLoaded() const { return loaded_; }
    unsigned short LoadedWorld() const { return worldIndex_; }

    /** Dessine une fenetre autour de (centerX,centerY) ; origine ecran (originX,originY). */
    void RenderViewport(SDL_Renderer *renderer, unsigned int centerX, unsigned int centerY, int viewTilesW,
                        int viewTilesH, int tilePx, float originX, float originY) const;

    /** Couleur RGB depuis l'indice palette (monde charge). */
    bool SampleRgb(unsigned int worldX, unsigned int worldY, std::uint8_t &r, std::uint8_t &g,
                   std::uint8_t &b) const;

    std::string GetLastError() const { return lastError_; }

   private:
    bool loaded_{false};
    unsigned short worldIndex_{0};
    std::vector<unsigned char> indices_;
    std::vector<unsigned char> paletteRgb_;
    std::string lastError_;
};
