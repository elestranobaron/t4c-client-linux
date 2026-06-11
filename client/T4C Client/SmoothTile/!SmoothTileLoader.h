#pragma once

#include "SmoothTile.h"

class SmoothTileLoader
{
public:
   SmoothTileLoader(void);
   ~SmoothTileLoader(void);

   static int LoadExceptionsFromFile(const std::string& rPathName,std::set<unsigned short>& rSetExceptions);
   static int LoadFromFile(const std::string& rPathName, std::map<SmoothTile::TileDefinition, SmoothTile>& rMapTiles);
   static int SaveToFile(const std::string& rPathName, std::map<SmoothTile::TileDefinition, SmoothTile>& rMapTiles);
   static int MapToVector(const std::map<SmoothTile::TileDefinition, SmoothTile>& rMap, std::vector<SmoothTile>& rVector);
   static int MapToVector(const std::map<SmoothTile::TileDefinition, SmoothTile>& rMap, std::vector<SmoothTile::TileDefinition>& rVector);
};
