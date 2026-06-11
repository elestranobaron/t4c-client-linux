#pragma once
#include <vector>
// #include ".\MyClasses\Image.h"
// #include ".\MyClasses\ImageReadWrite.h"
#include <fstream>

class LayeredTile
{
public:
   enum
   {
      LayerWidth = 32,
      LayerHeight = 16
   };

   class Layer
   {
   public:
      Layer(unsigned int Color, unsigned char Intensity);
      Layer();

      //const MD::Image& GetIntensity() const { return m_Intensity; }
      unsigned char GetIntensity(int x, int y) const;
      void SetIntensity(int x, int y,unsigned char chVal);
      unsigned short GetColor() { return m_Color; }

      int Save(std::ofstream& rOutputStream) const;
      int Load(std::ifstream& rInputStream);

   protected:
      unsigned short m_Color;
      //MD::Image      m_Intensity;
      unsigned char  m_Intensity[LayerHeight][LayerWidth];
   };

   LayeredTile();
   ~LayeredTile();

   float GetPixelCoef(int x, int y, unsigned int Color);

   Layer& GetLayer(unsigned int Color);

   int SetBaseLayer(const Layer& rNewLayer);

   std::vector<Layer> m_Layers;
};
