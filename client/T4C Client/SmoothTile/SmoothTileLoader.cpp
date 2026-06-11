#include "SmoothTileLoader.h"
#include <fstream>
#include <iostream>
#include <assert.h>

SmoothTileLoader::SmoothTileLoader(void)
{
}

SmoothTileLoader::~SmoothTileLoader(void)
{
}

int SmoothTileLoader::LoadExceptionsFromFile(const std::string& rPathName,
                                             std::set<unsigned short>& rSetExceptions)
{
   rSetExceptions.clear();

   std::ifstream InputFile;
   InputFile.open(rPathName.c_str());

   unsigned short Exception = 0;
   InputFile >> Exception;
   while (!InputFile.eof())
   {
      rSetExceptions.insert(Exception);
      InputFile >> Exception;
   }

   return 0;
}

int SmoothTileLoader::LoadFromFile(const std::string& rPathName,
                                   std::map<SmoothTile::TileDefinition, SmoothTile>& rMapTiles)
{
   rMapTiles.clear();

   std::ifstream InputFile;
   InputFile.open(rPathName.c_str(), std::ios::binary);

   char Header[4] = { 0 };
   InputFile.read(Header, sizeof(Header));
   unsigned short Version;
   InputFile.read(reinterpret_cast<char*>(&Version), sizeof(Version));

   unsigned short Count = 0;
   InputFile.read(reinterpret_cast<char*>(&Count), sizeof(Count));

   for (int i = 0; i < Count; ++i)
   {
      SmoothTile::TileDefinition TileDef;
      TileDef.Load(InputFile);
      rMapTiles[TileDef].Load(InputFile);
   }

   return 0;
}

int SmoothTileLoader::SaveToFile(const std::string& rPathName,
                                 std::map<SmoothTile::TileDefinition, SmoothTile>& rMapTiles)
{
   std::ofstream OutputFile;
   OutputFile.open(rPathName.c_str(), std::ios::binary);

   char Header[4] = { 'S', 'T', 'S', 0};
   unsigned short Version = 0x0100;
   OutputFile.write(Header, sizeof(Header));
   OutputFile.write(reinterpret_cast<char*>(&Version), sizeof(Version));

   unsigned short Count = rMapTiles.size();
   OutputFile.write(reinterpret_cast<char*>(&Count), sizeof(Count));

   std::map<SmoothTile::TileDefinition, SmoothTile>::iterator itCur = rMapTiles.begin();
   std::map<SmoothTile::TileDefinition, SmoothTile>::iterator itEnd = rMapTiles.end();
   for (itCur; itCur != itEnd; ++itCur)
   {
      itCur->first.Save(OutputFile);
      itCur->second.Save(OutputFile);
   }

   return 0;
}

int SmoothTileLoader::MapToVector( const std::map<SmoothTile::TileDefinition, SmoothTile>& rMap, std::vector<SmoothTile>& rVector )
{
   std::vector<bool> VecLoaded;
   VecLoaded.resize(rMap.size(), false);
   rVector.clear();
   rVector.resize(rMap.size());
   std::map<SmoothTile::TileDefinition, SmoothTile>::const_iterator itCur = rMap.begin();
   std::map<SmoothTile::TileDefinition, SmoothTile>::const_iterator itEnd = rMap.end();

   for (itCur; itCur != itEnd; ++itCur)
   {
      assert(itCur->second.m_ID < rVector.size());
      //assert(VecLoaded[itCur->second.m_ID] == false);
      if (VecLoaded[itCur->second.m_ID] == true)
      {
         //std::cout << "Duplicate found: " << itCur->second.m_ID << std::endl;
      }
      VecLoaded[itCur->second.m_ID] = true;
      rVector[itCur->second.m_ID] = itCur->second;
   }

   return 0;
}

int SmoothTileLoader::MapToVector( const std::map<SmoothTile::TileDefinition, SmoothTile>& rMap, std::vector<SmoothTile::TileDefinition>& rVector )
{
   std::vector<bool> VecLoaded;
   VecLoaded.resize(rMap.size(), false);
   rVector.clear();
   rVector.resize(rMap.size());
   std::map<SmoothTile::TileDefinition, SmoothTile>::const_iterator itCur = rMap.begin();
   std::map<SmoothTile::TileDefinition, SmoothTile>::const_iterator itEnd = rMap.end();

   for (itCur; itCur != itEnd; ++itCur)
   {
      assert(itCur->second.m_ID < rVector.size());
      //assert(VecLoaded[itCur->second.m_ID] == false);
      if (VecLoaded[itCur->second.m_ID] == true)
      {
         //std::cout << "Duplicate found: " << itCur->second.m_ID << std::endl;
      }
      VecLoaded[itCur->second.m_ID] = true;
      rVector[itCur->second.m_ID] = itCur->first;
   }

   return 0;
}
