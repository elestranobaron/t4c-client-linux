#pragma once

#pragma warning( disable : 4786 )
#include "LayeredTile.h"
#include <vector> 
#include <map>
#include <fstream.h>

class SmoothTile : public LayeredTile
{
public:
   class TileDefinition
   {
   public:
      TileDefinition() { };
      TileDefinition(std::vector<unsigned short>& rTiles);
      virtual ~TileDefinition() { }

      struct TilesToCount
      {
         TilesToCount(unsigned short ID, unsigned int Count) : TileID(ID), TileCount(Count) { }
         unsigned short TileID;
         unsigned int   TileCount;
         bool operator<(const TilesToCount& rOther) const
         {
            if (TileCount < rOther.TileCount)
            {
               return true;
            }
            if (TileCount > rOther.TileCount)
            {
               return false;
            }
            return (TileID < rOther.TileID);
         }
      };

      struct TileOffsets
      {
         TileOffsets() : m_OffsetX(0), m_OffsetY(0) { }
         TileOffsets(char X, char Y) : m_OffsetX(X), m_OffsetY(Y) { }
         char m_OffsetX;
         char m_OffsetY;

         int Save(ofstream& rOutputStream) const
         {
            rOutputStream.write(reinterpret_cast<const char*>(&m_OffsetX), sizeof(m_OffsetX));
            rOutputStream.write(reinterpret_cast<const char*>(&m_OffsetY), sizeof(m_OffsetY));
            return 0;
         }
         int Load(ifstream& rInputStream)
         {
            rInputStream.read(reinterpret_cast<char*>(&m_OffsetX), sizeof(m_OffsetX));
            rInputStream.read(reinterpret_cast<char*>(&m_OffsetY), sizeof(m_OffsetY));
            return 0;
         }
      };

      int CreateFrom(const std::vector<unsigned short>& rTiles);

      bool operator<(const TileDefinition& rOther) const
      {
         return m_Tiles < rOther.m_Tiles;
      }
      bool operator==(const TileDefinition& rOther) const
      {
         return m_Tiles == rOther.m_Tiles;
      }

      int Load(ifstream& rInputStream);
      int Save(ofstream& rOutputStream) const;

      // 9 Tiles (the center one + 8 others around) x 5
      std::vector<unsigned short> m_Tiles;

      std::vector<unsigned short> m_Colors;

      std::vector<TileOffsets> m_TilesOffset;

      std::map<unsigned short, int> m_MapTilesToID;
      std::map<int, unsigned short> m_MapIDToTiles;
   };

   SmoothTile();
   virtual ~SmoothTile();

   LayeredTile::Layer& AddLayer(unsigned short CurrentColor, TileDefinition::TileOffsets& rOffsets)
   {
      m_Offsets.push_back(rOffsets);
      LayeredTile::Layer NewLayer(CurrentColor, 0);
      m_Layers.push_back(NewLayer);
      return m_Layers.back();
   }

   int Load(ifstream& rInputStream);
   int Save(ofstream& rOutputStream) const;

   std::vector<TileDefinition::TileOffsets> m_Offsets;

   unsigned short m_ID;
};
