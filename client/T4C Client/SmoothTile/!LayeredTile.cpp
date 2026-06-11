#include "LayeredTile.h"
#include <assert.h>

LayeredTile::Layer::Layer(unsigned int Color, unsigned char Intensity)
{
   m_Color = Color;
   //m_Intensity.Create(LayerWidth, LayerHeight, 1, 1);
   for (unsigned int j = 0; j < LayerHeight; ++j)
   {
      for (unsigned int i = 0; i < LayerWidth; ++i)
      {
         //m_Intensity.At<unsigned char>(i, j, 0) = Intensity;
         m_Intensity[j][i] = 0;
      }
   }
}

LayeredTile::Layer::Layer()
{
   m_Color = 0;
   //m_Intensity.Create(LayerWidth, LayerHeight, 1, 1);
   for (unsigned int j = 0; j < LayerHeight; ++j)
   {
      for (unsigned int i = 0; i < LayerWidth; ++i)
      {
         //m_Intensity.At<unsigned char>(i, j, 0) = 0;
         m_Intensity[j][i] = 0;
      }
   }
}

unsigned char LayeredTile::Layer::GetIntensity(int x, int y) const
{
   //return m_Intensity.At<unsigned char>(x, y, 0);
   return m_Intensity[y][x];
}

void LayeredTile::Layer::SetIntensity(int x, int y, unsigned char chVal)
{
   //return m_Intensity.At<unsigned char>(x, y, 0);
   m_Intensity[y][x] = chVal;
}

int LayeredTile::Layer::Save( ofstream& rOutputStream ) const 
{
   for (unsigned int j = 0; j < LayerHeight; ++j)
   {
      for (unsigned int i = 0; i < LayerWidth; ++i)
      {
         unsigned char Value = GetIntensity(i, j);
         rOutputStream.write(reinterpret_cast<char*>(&Value), sizeof(Value));
      }
   }
   return 0;
}

int LayeredTile::Layer::Load( ifstream& rInputStream )
{
   for (unsigned int j = 0; j < LayerHeight; ++j)
   {
      for (unsigned int i = 0; i < LayerWidth; ++i)
      {
         unsigned char Value;
         rInputStream.read(reinterpret_cast<char*>(&Value), sizeof(Value));
         SetIntensity(i, j, Value);
      }
   }
   return 0;
}

LayeredTile::LayeredTile(void)
{
}

LayeredTile::~LayeredTile(void)
{
}

float LayeredTile::GetPixelCoef(int x, int y, unsigned int Color)
{
   Layer& rLayer = GetLayer(Color);
   return rLayer.GetIntensity(x, y) / 255.0f;
}

LayeredTile::Layer& LayeredTile::GetLayer(unsigned int Color)
{
   for (unsigned int i = 0; i < m_Layers.size(); ++i)
   {
      if (m_Layers[i].GetColor() == Color)
      {
         return m_Layers[i];
      }
   }

   Layer NewLayer(Color, 0);

   m_Layers.push_back(NewLayer);

   return m_Layers.back();
}

int LayeredTile::SetBaseLayer(const Layer& rNewLayer)
{
   assert(m_Layers.size() == 0);
   m_Layers.push_back(rNewLayer);

   return 0;
}