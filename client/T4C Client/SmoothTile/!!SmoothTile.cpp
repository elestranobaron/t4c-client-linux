//#include "stdafx.h"
#include "SmoothTile.h"
#include <algorithm>
#include <assert.h>

SmoothTile::SmoothTile(void)
{
}

SmoothTile::~SmoothTile(void)
{
}

int SmoothTile::Load( ifstream& rInputStream )
{
   rInputStream.read(reinterpret_cast<char*>(&m_ID), sizeof(m_ID));

   unsigned char NumLayers;
   rInputStream.read(reinterpret_cast<char*>(&NumLayers), sizeof(NumLayers));

   m_Layers.resize(NumLayers);
   m_Offsets.resize(NumLayers);

   for (unsigned int i = 0; i < m_Layers.size(); ++i)
   {
      m_Layers[i].Load(rInputStream);
      m_Offsets[i].Load(rInputStream);
   }

   return 0;
}

int SmoothTile::Save( ofstream& rOutputStream ) const
{
   rOutputStream.write(reinterpret_cast<const char*>(&m_ID), sizeof(m_ID));

   assert(m_Layers.size() == m_Offsets.size());

   unsigned char NumLayers = m_Layers.size();
   rOutputStream.write(reinterpret_cast<const char*>(&NumLayers), sizeof(NumLayers));

   for (unsigned int i = 0; i < m_Layers.size(); ++i)
   {
      m_Layers[i].Save(rOutputStream);
      m_Offsets[i].Save(rOutputStream);
   }

   return 0;
}

SmoothTile::TileDefinition::TileDefinition( std::vector<unsigned short>& rTiles, unsigned short SmoothSize )
{
   CreateFrom(rTiles, SmoothSize);
}

int SmoothTile::TileDefinition::CreateFrom(const std::vector<unsigned short>& rTiles, unsigned short SmoothSize)
{
   static const TileOffsets sTilesOffsetsMaj[] = 
   {
      TileOffsets(-1, -1),
      TileOffsets( 0, -1),
      TileOffsets( 1, -1),
      TileOffsets(-1,  0),
      TileOffsets( 0,  0),
      TileOffsets( 1,  0),
      TileOffsets(-1,  1),
      TileOffsets( 0,  1),
      TileOffsets( 1,  1)
   };

   static const TileOffsets sTilesOffsetsMin[] = 
   {
      TileOffsets(-1, -1),
      TileOffsets( 1, -1),
      TileOffsets(-1,  1),
      TileOffsets( 1,  1),
      TileOffsets( 0,  0)
   };

   assert(rTiles.size() == eTileSignatureSize);
   m_Tiles.reserve(eTileSignatureSize + 1);
   std::map<unsigned short, int> MapTilesToCount;
   for (size_t i = 0; i < rTiles.size(); ++i)
   {
      MapTilesToCount[rTiles[i]]++;
   }
   std::vector<TilesToCount> VectTilesToCount;
   std::map<unsigned short, int>::iterator itCountCur = MapTilesToCount.begin();
   std::map<unsigned short, int>::iterator itCountEnd = MapTilesToCount.end();
   for (itCountCur; itCountCur != itCountEnd; ++itCountCur)
   {
      VectTilesToCount.push_back(TilesToCount(itCountCur->first, itCountCur->second));
   }
   std::sort(VectTilesToCount.begin(), VectTilesToCount.end());

   std::map<unsigned short, int> MapTilesToID;
   for (i = 0; i < VectTilesToCount.size(); ++i)
   {
      MapTilesToID[VectTilesToCount[i].TileID] = MapTilesToID.size() + 1;
   }
   m_Tiles.resize(eTileSignatureSize+1);
   for (i = 0; i < rTiles.size(); ++i)
   {
      m_Tiles[i] = MapTilesToID[rTiles[i]];
   }

   m_MapDefinitionIDToTileID.clear();
   std::map<unsigned short, int>::iterator itCurID = MapTilesToID.begin();
   std::map<unsigned short, int>::iterator itEndID = MapTilesToID.end();
   for (itCurID; itCurID != itEndID; ++itCurID)
   {
      m_MapDefinitionIDToTileID[itCurID->second] = itCurID->first;
      m_Colors.push_back(itCurID->second);

      bool Found = false;
      // Check center tile
      if (m_Tiles[24] == itCurID->second)
      {
         m_TilesOffset.push_back(sTilesOffsetsMaj[24 / 5]);
         Found = true;
      }

      if (Found == false)
      {
            // Scan full tiles
         for (size_t i = 4; i < m_Tiles.size(); i += 5)
         {
            if (m_Tiles[i] == itCurID->second)
            {
               m_TilesOffset.push_back(sTilesOffsetsMaj[i / 5]);
               Found = true;
               break;
            }
         }
      }

      if (Found == false)
      {
         for (size_t i = 0; i < m_Tiles.size(); i++)
         {
            if (m_Tiles[i] == itCurID->second)
            {
               // 0...4
               int PosMaj = (i / 5);
               int PosMin = (i % 5);

               TileOffsets Offset(sTilesOffsetsMaj[PosMaj].m_OffsetX + sTilesOffsetsMin[PosMin].m_OffsetX,
                  sTilesOffsetsMaj[PosMaj].m_OffsetY + sTilesOffsetsMin[PosMin].m_OffsetY);
               m_TilesOffset.push_back(Offset);
               Found = true;
               break;
            }
         }
      }
      assert(Found);
   }

   m_Tiles.push_back(SmoothSize);

   return 0;
}

int SmoothTile::TileDefinition::Load( ifstream& rInputStream )
{
   m_Tiles.resize(eTileSignatureSize + 1);
   for (unsigned int i = 0; i < m_Tiles.size(); ++i)
   {
      unsigned char Val;
      rInputStream.read(reinterpret_cast<char*>(&Val), sizeof(Val));
      m_Tiles[i] = Val;
   }

   return 0;
}

int SmoothTile::TileDefinition::Save( ofstream& rOutputStream ) const
{
   assert(m_Tiles.size() == eTileSignatureSize + 1);

   for (unsigned int i = 0; i < m_Tiles.size(); ++i)
   {
      assert(m_Tiles[i] <= 255);
      unsigned char Val = static_cast<unsigned char>(m_Tiles[i]);
      rOutputStream.write(reinterpret_cast<char*>(&Val), sizeof(Val));
   }

   return 0;
}