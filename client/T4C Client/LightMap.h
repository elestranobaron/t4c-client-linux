#ifndef __LIGHTMAP_H
#define __LIGHTMAP_H

#include <windows.h>
#include "Global.h"

//#define LM_MAX_X  SCREEN_W_DIV2
//#define LM_MAX_Y  SCREEN_H_DIV2


class LightMap {

   public:
      LightMap(void);
      ~LightMap(void); 
      void Create(LPTSTR, int, int);
      void Release(void);
      void MergeLightMap(int, int);
      void MakeBaseLightMap(int, int, int);
      void MakeLightingFX(LPBYTE, LPBYTE, long, long, int, int);

      WORD GetWidth(){return wWidth;}
      WORD GetHeight(){return wHeight;}

   protected:
      void MergeLightMap_Base(int, int);
      void MergeLightMap_SSE2(int, int);
      void MergeLightMap_MMX(int, int);
      void MergeLightMap_BaseLow(int, int);
      void MergeLightMap_SSE2Low(int, int);
      void MergeLightMap_MMXLow(int, int);

      void MakeLightingFX_Base(LPBYTE, LPBYTE, long, long, int, int);
      void MakeLightingFX_SSE2(LPBYTE, LPBYTE, long, long, int, int);
      void MakeLightingFX_MMX(LPBYTE, LPBYTE, long, long, int, int);
      void MakeLightingFX_BaseLow(LPBYTE, LPBYTE, long, long, int, int);
      void MakeLightingFX_SSE2Low(LPBYTE, LPBYTE, long, long, int, int);
      void MakeLightingFX_MMXLow(LPBYTE, LPBYTE, long, long, int, int);

   protected:
      int    m_uiGrayStatus;
      int    m_uiGrayStatusCnt;

   private:
      
      LPWORD lpwMap;

      WORD wWidth;
      WORD wHeight;
};
extern LightMap LightMapOBJ;

#endif