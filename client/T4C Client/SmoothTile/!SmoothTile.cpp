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

   assert(NumLayers <= 9);
   m_Layers.resize(NumLayers);
   m_Offsets.resize(NumLayers);

   for (unsigned int i = 0; i < m_Layers.size(); ++i)
   {
      m_Layers[i].Load(rInputStream);
      m_Offsets[i].Load(rInputStream);
   }

   return 0;
}

int SmoothTile::Save(ofstream& rOutputStream ) const
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

SmoothTile::TileDefinition::TileDefinition( std::vector<unsigned short>& rTiles )
{
   CreateFrom(rTiles);
}

int SmoothTile::TileDefinition::CreateFrom(const std::vector<unsigned short>& rTiles)
{
   static const TileOffsets sTilesOffsets[] = 
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

   size_t i = 0;

   assert(rTiles.size() == 9*5);
   std::map<unsigned short, int> MapTilesToCount;
   for (i = 0; i < rTiles.size(); ++i)
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

   m_MapTilesToID.clear();
   for (i = 0; i < VectTilesToCount.size(); ++i)
   {
      m_MapTilesToID[VectTilesToCount[i].TileID] = m_MapTilesToID.size() + 1;
   }
   m_Tiles.resize(9*5);
   for (i = 0; i < rTiles.size(); ++i)
   {
      m_Tiles[i] = m_MapTilesToID[rTiles[i]];
   }

   m_MapIDToTiles.clear();
   std::map<unsigned short, int>::iterator itCurID = m_MapTilesToID.begin();
   std::map<unsigned short, int>::iterator itEndID = m_MapTilesToID.end();
   for (itCurID; itCurID != itEndID; ++itCurID)
   {
      m_MapIDToTiles[itCurID->second] = itCurID->first;
      m_Colors.push_back(itCurID->second);
      //             for (size_t i = 4; i < m_Tiles.size(); i += 5)
      //             {
      //                if (m_Tiles[i] == itCurID->second)
      //                {
      //                   m_TilesOffset.push_back(sTilesOffsets[i / 5]);
      //                   break;
      //                }
      //             }

      bool Found = false;
      // Check center tile
      if (m_Tiles[24] == itCurID->second)
      {
         m_TilesOffset.push_back(sTilesOffsets[24 / 5]);
         Found = true;
      }

      if (Found == false)
      {
            // Scan full tiles
         for (size_t i = 4; i < m_Tiles.size(); i += 5)
         {
            if (m_Tiles[i] == itCurID->second)
            {
               m_TilesOffset.push_back(sTilesOffsets[i / 5]);
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

               TileOffsets Offset(sTilesOffsets[PosMaj].m_OffsetX + sTilesOffsetsMin[PosMin].m_OffsetX,
                  sTilesOffsets[PosMaj].m_OffsetY + sTilesOffsetsMin[PosMin].m_OffsetY);
               m_TilesOffset.push_back(Offset);
               Found = true;
               break;
            }
         }
      }
      assert(Found);
   }

   return 0;
}

int SmoothTile::TileDefinition::Load( ifstream& rInputStream )
{
   m_Tiles.resize(45);
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
   assert(m_Tiles.size() == 45);

   for (unsigned int i = 0; i < m_Tiles.size(); ++i)
   {
      assert(m_Tiles[i] <= 255);
      unsigned char Val = static_cast<unsigned char>(m_Tiles[i]);
      rOutputStream.write(reinterpret_cast<char*>(&Val), sizeof(Val));
   }

   return 0;
}