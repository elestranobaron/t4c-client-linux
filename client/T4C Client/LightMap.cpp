#include "pch.h"
#include <emmintrin.h>
#include "LightMap.h"
#include "IDirectXDraw.h"
#include "stdio.h"
#include "MacroHandler.h"
#include "Global.h"
#include "SaveGame.h"
#include "NewInterface/V3_LifeDlg.h"
#include  <math.h> 
extern CSaveGame g_SaveGame;

//#define USE_CPP_LIGHT_CODE


   
LightMap LightMapOBJ;

#define _RGB16BIT565_L(r,g,b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))

static WORD *lpwLightMap = NULL;
//static WORD *lpwLightMap = new WORD[g_Global.GetScreenWDiv2() * g_Global.GetScreenHDiv2()];

//WORD LightMap::lpwLightMap[512 * 384];

LightMap::LightMap(void) 
{
   lpwMap         = NULL;
   m_uiGrayStatus    = 0;
   m_uiGrayStatusCnt = 0;
}

LightMap::~LightMap(void) 
{
   Release();
}

void LightMap::Release(void) {
   if (lpwMap) 
   {
      delete lpwMap;
      lpwMap = NULL;
   }
   if(lpwLightMap)
   {
      delete []lpwLightMap;
      lpwLightMap = NULL;
   }
}

void LightMap::Create(LPTSTR lpszID, int w, int h) 
{
   //on valid si la light existe et est en bonne taille...
   if(lpwLightMap == NULL)
      lpwLightMap   = new WORD[g_Global.GetScreenW() * g_Global.GetScreenH()];
   memset(lpwLightMap,0x00,g_Global.GetScreenW() * g_Global.GetScreenH()*sizeof(WORD));
   

   WORD wRed;
   WORD wGreen;
   WORD wBlue;
   WORD wColor;
   char szTemp[256];

   BYTE *bTempBuffer = new BYTE[w*h*3];
   
   wWidth = w;
   wHeight = h;

   lpwMap = new WORD [wWidth*wHeight];


   strcpy_s(szTemp,256, "Game Files\\");
   strcat_s(szTemp,256, lpszID);
   
   FILE *f = NULL;
   fopen_s(&f,szTemp, "rb");
   int dwtoto = fread(bTempBuffer, 1, w*h*3, f);
   fclose(f);

   for (int i = 0; i < w*h; i++) 
   {
      wRed = bTempBuffer[(i*3)] >> 3;
      wGreen = bTempBuffer[(i*3)+1] >> 3;
      wBlue = bTempBuffer[(i*3)+2] >> 3;

      // Brightness Correction
      wRed = wRed + (g_SaveGame.GetOptionsParam()->dwBrightnessNew*1);
      if (wRed > 31)
         wRed = 31;
      wGreen = wGreen + (g_SaveGame.GetOptionsParam()->dwBrightnessNew-1);
      if (wGreen > 31)
         wGreen = 31;
      wBlue = wBlue + (g_SaveGame.GetOptionsParam()->dwBrightnessNew-1);
      if (wBlue > 31)
         wBlue = 31;
      wColor = (wRed << 10) + (wGreen << 5) + (wBlue);
      lpwMap[i] = wColor;
   }
   delete bTempBuffer;
}

void LightMap::MergeLightMap(int x, int y) 
{
   if(Custom.gDebugNoLight == 1)
      return;

   #ifdef USE_CPP_LIGHT_CODE
      if (g_SaveGame.GetOptionsParam()->bLightLowSize) 
         MergeLightMap_BaseLow(x, y);
      else
         MergeLightMap_Base(x, y);

   #else
      if(Custom.gdwUSEMMX)
      {
         if (g_SaveGame.GetOptionsParam()->bLightLowSize) 
            MergeLightMap_MMXLow(x, y);
         else
            MergeLightMap_MMX(x, y);
      }
      else
      {
         if (g_SaveGame.GetOptionsParam()->bLightLowSize) 
            MergeLightMap_SSE2Low(x, y);
         else
            MergeLightMap_SSE2(x, y);
      }
      
   #endif
}


void LightMap::MakeBaseLightMap(int r, int g, int b) 
{
   // Brightness Correction
   r = r + ((g_SaveGame.GetOptionsParam()->dwBrightnessNew-1));
   if (r > 31)
      r = 31;
   g = g + ((g_SaveGame.GetOptionsParam()->dwBrightnessNew-1));
   if (g > 31)
      g = 31;
   b = b + ((g_SaveGame.GetOptionsParam()->dwBrightnessNew-1));
   if (b > 31)
      b = 31;

   int i, j;
   DWORD dwColor = (r << 26) + (g << 21) + (b << 16) + (r << 10) + (g << 5) + (b);
   LPDWORD lpdwLightMap = (LPDWORD)lpwLightMap;

   if (g_SaveGame.GetOptionsParam()->bLightLowSize) 
   {
      for (j = 0; j < g_Global.GetScreenHDiv2(); j++) 
      {
         for (i = 0; i < g_Global.GetScreenWDiv2()/2; i++) 
         {
            *lpdwLightMap = dwColor;
            lpdwLightMap++;
         }
      }
   }
   else
   {
      for (j = 0; j < g_Global.GetScreenH(); j++) 
      {
         for (i = 0; i < g_Global.GetScreenW()/2; i++) 
         {
            *lpdwLightMap = dwColor;
            lpdwLightMap++;
         }
      }
   }
}



void LightMap::MakeLightingFX(LPBYTE lpbDest, LPBYTE lpbSource, long lDPitch, long lSPitch, int xPos, int yPos) 
{

   #ifdef USE_CPP_LIGHT_CODE
      if (g_SaveGame.GetOptionsParam()->bLightLowSize) 
         MakeLightingFX_BaseLow(lpbDest, lpbSource, lDPitch, lSPitch, xPos, yPos);
      else
         MakeLightingFX_Base(lpbDest, lpbSource, lDPitch, lSPitch, xPos, yPos);
   
   #else
      if(Custom.gdwUSEMMX)
      {
         if (g_SaveGame.GetOptionsParam()->bLightLowSize) 
            MakeLightingFX_MMXLow(lpbDest, lpbSource, lDPitch, lSPitch, xPos, yPos);
         else
            MakeLightingFX_MMX(lpbDest, lpbSource, lDPitch, lSPitch, xPos, yPos);
      }
      else
      {
         if (g_SaveGame.GetOptionsParam()->bLightLowSize) 
            MakeLightingFX_SSE2Low(lpbDest, lpbSource, lDPitch, lSPitch, xPos, yPos);
         else
            MakeLightingFX_SSE2(lpbDest, lpbSource, lDPitch, lSPitch, xPos, yPos);
      }
   #endif
}




void LightMap::MergeLightMap_Base(int x, int y) 
{
   // Set 1 Mini Buffer
   if ((x+wWidth) <= 0)
      return;
   if (x >= g_Global.GetScreenW())
      return;
   if ((y+wHeight) <= 0)
      return;
   if (y >= g_Global.GetScreenH())
      return;
   
   int j;
   int  xCut = x < 0 ? -x : 0;
   int  yCut = y < 0 ? -y : 0;
   int  xEnd = (x+wWidth)  > g_Global.GetScreenW() ? (x+wWidth) -g_Global.GetScreenW() : 0;
   int  yEnd = (y+wHeight) > g_Global.GetScreenH() ? (y+wHeight)-g_Global.GetScreenH() : 0;
   
   DWORD dwW = (DWORD)wWidth-(xEnd+xCut);
   WORD wH = wHeight;
   
   
   UINT dwOffset = ((x+xCut) + ((y+yCut)*g_Global.GetScreenW()));
   //if(dwOffset > (UINT)((g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight)))
   //   dwOffset = (g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight);
   
   if(dwOffset > (UINT)((g_Global.GetScreenW()*g_Global.GetScreenH())-(wWidth*(wH-yEnd-yCut))))
      dwOffset = (g_Global.GetScreenW()*g_Global.GetScreenH())-(wWidth*(wH-yEnd-yCut));
   
   LPWORD lpwL = (lpwLightMap + dwOffset);
   LPWORD lpwM = (lpwMap + xCut + yCut*wWidth);
   
   int dwWDiv2 = g_Global.GetScreenW();
   for (j = 0; j < wH-yEnd-yCut; j++) 
   {
      for (DWORD i = 0; i < dwW; ++i)
      {
         WORD wDCol = 0;
         
         WORD wSRed = *lpwM & 0x7C00;
         WORD wDRed = *lpwL & 0x7C00;
         
         wDCol = (wSRed > wDRed) ? wDCol | wSRed : wDCol | wDRed;
         
         WORD wSGreen = *lpwM & 0x03E0;
         WORD wDGreen = *lpwL & 0x03E0;
         
         wDCol = (wSGreen > wDGreen) ? wDCol | wSGreen : wDCol | wDGreen;
         
         WORD wSBlue = *lpwM & 0x001F;
         WORD wDBlue = *lpwL & 0x001F;
         
         wDCol = (wSBlue > wDBlue) ? wDCol | wSBlue : wDCol | wDBlue;
         
         *lpwL = wDCol;
         
         ++lpwM;
         ++lpwL;
      }
      lpwM += (xEnd + xCut);
      lpwL += (dwWDiv2 - dwW);
   }
}


void LightMap::MergeLightMap_BaseLow(int x, int y) 
{
   // Set 1 Mini Buffer
   if ((x+wWidth) <= 0)
      return;
   if (x >= g_Global.GetScreenWDiv2())
      return;
   if ((y+wHeight) <= 0)
      return;
   if (y >= g_Global.GetScreenHDiv2())
      return;
   
   int j;
   int  xCut = x < 0 ? -x : 0;
   int  yCut = y < 0 ? -y : 0;
   int  xEnd = (x+wWidth)  > g_Global.GetScreenWDiv2() ? (x+wWidth) -g_Global.GetScreenWDiv2() : 0;
   int  yEnd = (y+wHeight) > g_Global.GetScreenHDiv2() ? (y+wHeight)-g_Global.GetScreenHDiv2() : 0;
   
   DWORD dwW = (DWORD)wWidth-(xEnd+xCut);
   WORD wH = wHeight;
   
   
   UINT dwOffset = ((x+xCut) + ((y+yCut)*g_Global.GetScreenWDiv2()));
   
   if(dwOffset > (UINT)((g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*(wH-yEnd-yCut))))
      dwOffset = (g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*(wH-yEnd-yCut));
   
   LPWORD lpwL = (lpwLightMap + dwOffset);
   LPWORD lpwM = (lpwMap + xCut + yCut*wWidth);
   
   int dwWDiv2 = g_Global.GetScreenWDiv2();
   for (j = 0; j < wH-yEnd-yCut; j++) 
   {
      for (DWORD i = 0; i < dwW; ++i)
      {
         WORD wDCol = 0;
         
         WORD wSRed = *lpwM & 0x7C00;
         WORD wDRed = *lpwL & 0x7C00;
         
         wDCol = (wSRed > wDRed) ? wDCol | wSRed : wDCol | wDRed;
         
         WORD wSGreen = *lpwM & 0x03E0;
         WORD wDGreen = *lpwL & 0x03E0;
         
         wDCol = (wSGreen > wDGreen) ? wDCol | wSGreen : wDCol | wDGreen;
         
         WORD wSBlue = *lpwM & 0x001F;
         WORD wDBlue = *lpwL & 0x001F;
         
         wDCol = (wSBlue > wDBlue) ? wDCol | wSBlue : wDCol | wDBlue;
         
         *lpwL = wDCol;
         
         ++lpwM;
         ++lpwL;
      }
      lpwM += (xEnd + xCut);
      lpwL += (dwWDiv2 - dwW);
   }
}


void LightMap::MergeLightMap_SSE2(int x, int y) 
{
   // Set 1 Mini Buffer
   if ((x+wWidth) <= 0)
      return;
   if (x >= g_Global.GetScreenW())
      return;
   if ((y+wHeight) <= 0)
      return;
   if (y >= g_Global.GetScreenH())
      return;

   int j;
   int  xCut = x < 0 ? -x : 0;
   int  yCut = y < 0 ? -y : 0;
   int  xEnd = (x+wWidth)  > g_Global.GetScreenW() ? (x+wWidth) -g_Global.GetScreenW() : 0;
   int  yEnd = (y+wHeight) > g_Global.GetScreenH() ? (y+wHeight)-g_Global.GetScreenH() : 0;

   DWORD dwW = (DWORD)wWidth-(xEnd+xCut);
   WORD wH = wHeight;


   UINT dwOffset = ((x+xCut) + ((y+yCut)*g_Global.GetScreenW()));
   //if(dwOffset > (UINT)((g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight)))
   //   dwOffset = (g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight);

   if(dwOffset > (UINT)((g_Global.GetScreenW()*g_Global.GetScreenH())-(wWidth*(wH-yEnd-yCut))))
      dwOffset = (g_Global.GetScreenW()*g_Global.GetScreenH())-(wWidth*(wH-yEnd-yCut));

   LPWORD lpwL = (lpwLightMap + dwOffset);
   LPWORD lpwM = (lpwMap + xCut + yCut*wWidth);

   int dwWDiv2 = g_Global.GetScreenW();

   int SizeX = dwW;

   int Remainder = (SizeX & 0x07); 
   int NumBlocks = (SizeX & ~0x07) / 8;

   const unsigned int mu1_555 = 0x7c007c00;
   const unsigned int mu2_555 = 0x03e003e0;
   const unsigned int mu3_555 = 0x001f001f;

   // Loading masks
   __m128i REDMASK_555   = _mm_set_epi32 (mu1_555, mu1_555, mu1_555, mu1_555);
   __m128i GREENMASK_555 = _mm_set_epi32 (mu2_555, mu2_555, mu2_555, mu2_555);
   __m128i BLUEMASK_555  = _mm_set_epi32 (mu3_555, mu3_555, mu3_555, mu3_555);
   
   for (j = 0; j < wH-yEnd-yCut; j++) 
   {
      __asm
      {
         push        edi
         push        esi

         push        ecx

         mov         ecx, NumBlocks
         mov         edi, lpwL
         mov         esi, lpwM

         cmp         ecx, 0
         je          done_pixels_blocks

do_next_block:

         movdqu      xmm0, byte ptr [edi]  // dst color
         movdqu      xmm3, byte ptr [esi]  // src color
         movdqa      xmm1, xmm0
         movdqa      xmm4, xmm3
         movdqa      xmm2, xmm0
         movdqa      xmm5, xmm3

         pand        xmm0, REDMASK_555
         pand        xmm1, GREENMASK_555
         pand        xmm2, BLUEMASK_555
         pand        xmm3, REDMASK_555
         pand        xmm4, GREENMASK_555
         pand        xmm5, BLUEMASK_555

         movdqa      xmm6, xmm0    // copy
         movdqa      xmm7, xmm2
         pcmpgtw     xmm6, xmm3    // create mask over copy
         pcmpgtw     xmm7, xmm5
         pand        xmm0, xmm6    // keep mm0 parts from mask in mm6
         pand        xmm2, xmm7
         pandn       xmm6, xmm3    // keep mm3 parts from ~mask in mm6 
         pandn       xmm7, xmm5

         movdqa      xmm3, xmm1
         por         xmm0, xmm2
         pcmpgtw     xmm3, xmm4
         por         xmm6, xmm7
         pand        xmm1, xmm3
         por         xmm0, xmm6
         pandn       xmm3, xmm4
         por         xmm0, xmm1
         por         xmm0, xmm3

         add         esi, 16      // Advance to the next eight pixels.
         movdqu      [edi], xmm0

         add         edi, 16      // Advance to the next eight pixels.
         
         //
         // Loop again or break.
         //
         dec			ecx
         jnz			do_next_block

done_pixels_blocks:
         
         pop         ecx
            
         pop         edi
         pop         esi
      }

      // Increment pointers to remainder positions
      lpwM += NumBlocks * 8;
      lpwL += NumBlocks * 8;

      for (int i = 0; i < Remainder; ++i)
      {
         WORD wDCol = 0;

         WORD wSRed = *lpwM & 0x7C00;
         WORD wDRed = *lpwL & 0x7C00;

         wDCol = (wSRed > wDRed) ? wDCol | wSRed : wDCol | wDRed;

         WORD wSGreen = *lpwM & 0x03E0;
         WORD wDGreen = *lpwL & 0x03E0;

         wDCol = (wSGreen > wDGreen) ? wDCol | wSGreen : wDCol | wDGreen;

         WORD wSBlue = *lpwM & 0x001F;
         WORD wDBlue = *lpwL & 0x001F;

         wDCol = (wSBlue > wDBlue) ? wDCol | wSBlue : wDCol | wDBlue;

         *lpwL = wDCol;

         ++lpwM;
         ++lpwL;
      }
      lpwM += (xEnd + xCut);
      lpwL += (dwWDiv2 - dwW);
   }
}

void LightMap::MergeLightMap_MMX(int x, int y) 
{
   // Set 1 Mini Buffer
   if ((x+wWidth) <= 0)
      return;
   if (x >= g_Global.GetScreenW())
      return;
   if ((y+wHeight) <= 0)
      return;
   if (y >= g_Global.GetScreenH())
      return;

   int j;
   int  xCut = x < 0 ? -x : 0;
   int  yCut = y < 0 ? -y : 0;
   int  xEnd = (x+wWidth)  > g_Global.GetScreenW() ? (x+wWidth) -g_Global.GetScreenW() : 0;
   int  yEnd = (y+wHeight) > g_Global.GetScreenH() ? (y+wHeight)-g_Global.GetScreenH() : 0;

   DWORD dwW = (DWORD)wWidth-(xEnd+xCut);
   WORD wH = wHeight;


   UINT dwOffset = ((x+xCut) + ((y+yCut)*g_Global.GetScreenW()));
   //if(dwOffset > (UINT)((g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight)))
   //   dwOffset = (g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight);

   if(dwOffset > (UINT)((g_Global.GetScreenW()*g_Global.GetScreenH())-(wWidth*(wH-yEnd-yCut))))
      dwOffset = (g_Global.GetScreenW()*g_Global.GetScreenH())-(wWidth*(wH-yEnd-yCut));

   LPWORD lpwL = (lpwLightMap + dwOffset);
   LPWORD lpwM = (lpwMap + xCut + yCut*wWidth);

   int dwWDiv2 = g_Global.GetScreenW();

   int SizeX = dwW;

   int Remainder = (SizeX & 0x03); 
   int NumBlocks = (SizeX & ~0x03) / 4;

   const unsigned int mu1_555 = 0x7c007c00;
   const unsigned int mu2_555 = 0x03e003e0;
   const unsigned int mu3_555 = 0x001f001f;

   const unsigned int ou1_555 = 0x001f001f;
   const unsigned int ou2_555 = 0x001f001f;
   const unsigned int ou3_555 = 0x001f001f;

   // Loading masks
   unsigned int REDMASK_555[2]   = { mu1_555, mu1_555 };
   unsigned int GREENMASK_555[2] = { mu2_555, mu2_555 };
   unsigned int BLUEMASK_555[2]  = { mu3_555, mu3_555 };

   for (j = 0; j < wH-yEnd-yCut; j++) 
   {
      __asm
      {
         push        edi
            push        esi

            push        ecx

            mov         ecx, NumBlocks
            mov         edi, lpwL
            mov         esi, lpwM

            cmp         ecx, 0
            je          done_pixels_blocks

do_next_block:

         movq        mm0, byte ptr [edi]  // dst color
         movq        mm3, byte ptr [esi]  // src color
         movq        mm1, mm0
            movq        mm4, mm3
            movq        mm2, mm0
            movq        mm5, mm3

            pand        mm0, REDMASK_555
            pand        mm1, GREENMASK_555
            pand        mm2, BLUEMASK_555
            pand        mm3, REDMASK_555
            pand        mm4, GREENMASK_555
            pand        mm5, BLUEMASK_555

            movq        mm6, mm0    // copy
            movq        mm7, mm2
            pcmpgtw     mm6, mm3    // create mask over copy
            pcmpgtw     mm7, mm5
            pand        mm0, mm6    // keep mm0 parts from mask in mm6
            pand        mm2, mm7
            pandn       mm6, mm3    // keep mm3 parts from ~mask in mm6 
            pandn       mm7, mm5

            movq        mm3, mm1
            por         mm0, mm2
            pcmpgtw     mm3, mm4
            por         mm6, mm7
            pand        mm1, mm3
            por         mm0, mm6
            pandn       mm3, mm4
            por         mm0, mm1
            por         mm0, mm3

            add			esi, 8      // Advance to the next height pixels.
            movq	      [edi], mm0

            add			edi, 8      // Advance to the next height pixels.

            //
            // Loop again or break.
            //
            dec			ecx
            jnz			do_next_block

done_pixels_blocks:

         pop         ecx

            pop         edi
            pop         esi
            emms
      }

      // Increment pointers to remainder positions
      lpwM += NumBlocks * 4;
      lpwL += NumBlocks * 4;

      for (int i = 0; i < Remainder; ++i)
      {
         WORD wDCol = 0;

         WORD wSRed = *lpwM & 0x7C00;
         WORD wDRed = *lpwL & 0x7C00;

         wDCol = (wSRed > wDRed) ? wDCol | wSRed : wDCol | wDRed;

         WORD wSGreen = *lpwM & 0x03E0;
         WORD wDGreen = *lpwL & 0x03E0;

         wDCol = (wSGreen > wDGreen) ? wDCol | wSGreen : wDCol | wDGreen;

         WORD wSBlue = *lpwM & 0x001F;
         WORD wDBlue = *lpwL & 0x001F;

         wDCol = (wSBlue > wDBlue) ? wDCol | wSBlue : wDCol | wDBlue;

         *lpwL = wDCol;

         ++lpwM;
         ++lpwL;
      }
      lpwM += (xEnd + xCut);
      lpwL += (dwWDiv2 - dwW);
   }
}


void LightMap::MergeLightMap_SSE2Low(int x, int y) 
{
   // Set 1 Mini Buffer
   if ((x+wWidth) <= 0)
      return;
   if (x >= g_Global.GetScreenWDiv2())
      return;
   if ((y+wHeight) <= 0)
      return;
   if (y >= g_Global.GetScreenHDiv2())
      return;

   int j;
   int  xCut = x < 0 ? -x : 0;
   int  yCut = y < 0 ? -y : 0;
   int  xEnd = (x+wWidth)  > g_Global.GetScreenWDiv2() ? (x+wWidth) -g_Global.GetScreenWDiv2() : 0;
   int  yEnd = (y+wHeight) > g_Global.GetScreenHDiv2() ? (y+wHeight)-g_Global.GetScreenHDiv2() : 0;

   DWORD dwW = (DWORD)wWidth-(xEnd+xCut);
   WORD wH = wHeight;


   UINT dwOffset = ((x+xCut) + ((y+yCut)*g_Global.GetScreenWDiv2()));
   //if(dwOffset > (UINT)((g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight)))
   //   dwOffset = (g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight);

   if(dwOffset > (UINT)((g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*(wH-yEnd-yCut))))
      dwOffset = (g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*(wH-yEnd-yCut));

   LPWORD lpwL = (lpwLightMap + dwOffset);
   LPWORD lpwM = (lpwMap + xCut + yCut*wWidth);

   int dwWDiv2 = g_Global.GetScreenWDiv2();

   int SizeX = dwW;

   int Remainder = (SizeX & 0x07); 
   int NumBlocks = (SizeX & ~0x07) / 8;

   const unsigned int mu1_555 = 0x7c007c00;
   const unsigned int mu2_555 = 0x03e003e0;
   const unsigned int mu3_555 = 0x001f001f;

   // Loading masks
   __m128i REDMASK_555   = _mm_set_epi32 (mu1_555, mu1_555, mu1_555, mu1_555);
   __m128i GREENMASK_555 = _mm_set_epi32 (mu2_555, mu2_555, mu2_555, mu2_555);
   __m128i BLUEMASK_555  = _mm_set_epi32 (mu3_555, mu3_555, mu3_555, mu3_555);
   
   for (j = 0; j < wH-yEnd-yCut; j++) 
   {
      __asm
      {
         push        edi
         push        esi

         push        ecx

         mov         ecx, NumBlocks
         mov         edi, lpwL
         mov         esi, lpwM

         cmp         ecx, 0
         je          done_pixels_blocks

do_next_block:

         movdqu      xmm0, byte ptr [edi]  // dst color
         movdqu      xmm3, byte ptr [esi]  // src color
         movdqa      xmm1, xmm0
         movdqa      xmm4, xmm3
         movdqa      xmm2, xmm0
         movdqa      xmm5, xmm3

         pand        xmm0, REDMASK_555
         pand        xmm1, GREENMASK_555
         pand        xmm2, BLUEMASK_555
         pand        xmm3, REDMASK_555
         pand        xmm4, GREENMASK_555
         pand        xmm5, BLUEMASK_555

         movdqa      xmm6, xmm0    // copy
         movdqa      xmm7, xmm2
         pcmpgtw     xmm6, xmm3    // create mask over copy
         pcmpgtw     xmm7, xmm5
         pand        xmm0, xmm6    // keep mm0 parts from mask in mm6
         pand        xmm2, xmm7
         pandn       xmm6, xmm3    // keep mm3 parts from ~mask in mm6 
         pandn       xmm7, xmm5

         movdqa      xmm3, xmm1
         por         xmm0, xmm2
         pcmpgtw     xmm3, xmm4
         por         xmm6, xmm7
         pand        xmm1, xmm3
         por         xmm0, xmm6
         pandn       xmm3, xmm4
         por         xmm0, xmm1
         por         xmm0, xmm3

         add         esi, 16      // Advance to the next eight pixels.
         movdqu      [edi], xmm0

         add         edi, 16      // Advance to the next eight pixels.
         
         //
         // Loop again or break.
         //
         dec			ecx
         jnz			do_next_block

done_pixels_blocks:
         
         pop         ecx
            
         pop         edi
         pop         esi
      }

      // Increment pointers to remainder positions
      lpwM += NumBlocks * 8;
      lpwL += NumBlocks * 8;

      for (int i = 0; i < Remainder; ++i)
      {
         WORD wDCol = 0;

         WORD wSRed = *lpwM & 0x7C00;
         WORD wDRed = *lpwL & 0x7C00;

         wDCol = (wSRed > wDRed) ? wDCol | wSRed : wDCol | wDRed;

         WORD wSGreen = *lpwM & 0x03E0;
         WORD wDGreen = *lpwL & 0x03E0;

         wDCol = (wSGreen > wDGreen) ? wDCol | wSGreen : wDCol | wDGreen;

         WORD wSBlue = *lpwM & 0x001F;
         WORD wDBlue = *lpwL & 0x001F;

         wDCol = (wSBlue > wDBlue) ? wDCol | wSBlue : wDCol | wDBlue;

         *lpwL = wDCol;

         ++lpwM;
         ++lpwL;
      }
      lpwM += (xEnd + xCut);
      lpwL += (dwWDiv2 - dwW);
   }
}

void LightMap::MergeLightMap_MMXLow(int x, int y) 
{
   // Set 1 Mini Buffer
   if ((x+wWidth) <= 0)
      return;
   if (x >= g_Global.GetScreenWDiv2())
      return;
   if ((y+wHeight) <= 0)
      return;
   if (y >= g_Global.GetScreenHDiv2())
      return;

   int j;
   int  xCut = x < 0 ? -x : 0;
   int  yCut = y < 0 ? -y : 0;
   int  xEnd = (x+wWidth)  > g_Global.GetScreenWDiv2() ? (x+wWidth) -g_Global.GetScreenWDiv2() : 0;
   int  yEnd = (y+wHeight) > g_Global.GetScreenHDiv2() ? (y+wHeight)-g_Global.GetScreenHDiv2() : 0;

   DWORD dwW = (DWORD)wWidth-(xEnd+xCut);
   WORD wH = wHeight;


   UINT dwOffset = ((x+xCut) + ((y+yCut)*g_Global.GetScreenWDiv2()));
   //if(dwOffset > (UINT)((g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight)))
   //   dwOffset = (g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*wHeight);

   if(dwOffset > (UINT)((g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*(wH-yEnd-yCut))))
      dwOffset = (g_Global.GetScreenWDiv2()*g_Global.GetScreenHDiv2())-(wWidth*(wH-yEnd-yCut));

   LPWORD lpwL = (lpwLightMap + dwOffset);
   LPWORD lpwM = (lpwMap + xCut + yCut*wWidth);

   int dwWDiv2 = g_Global.GetScreenWDiv2();

   int SizeX = dwW;

   int Remainder = (SizeX & 0x03); 
   int NumBlocks = (SizeX & ~0x03) / 4;

   const unsigned int mu1_555 = 0x7c007c00;
   const unsigned int mu2_555 = 0x03e003e0;
   const unsigned int mu3_555 = 0x001f001f;

   const unsigned int ou1_555 = 0x001f001f;
   const unsigned int ou2_555 = 0x001f001f;
   const unsigned int ou3_555 = 0x001f001f;

   // Loading masks
   unsigned int REDMASK_555[2]   = { mu1_555, mu1_555 };
   unsigned int GREENMASK_555[2] = { mu2_555, mu2_555 };
   unsigned int BLUEMASK_555[2]  = { mu3_555, mu3_555 };

   for (j = 0; j < wH-yEnd-yCut; j++) 
   {
      __asm
      {
         push        edi
            push        esi

            push        ecx

            mov         ecx, NumBlocks
            mov         edi, lpwL
            mov         esi, lpwM

            cmp         ecx, 0
            je          done_pixels_blocks

do_next_block:

         movq        mm0, byte ptr [edi]  // dst color
         movq        mm3, byte ptr [esi]  // src color
         movq        mm1, mm0
            movq        mm4, mm3
            movq        mm2, mm0
            movq        mm5, mm3

            pand        mm0, REDMASK_555
            pand        mm1, GREENMASK_555
            pand        mm2, BLUEMASK_555
            pand        mm3, REDMASK_555
            pand        mm4, GREENMASK_555
            pand        mm5, BLUEMASK_555

            movq        mm6, mm0    // copy
            movq        mm7, mm2
            pcmpgtw     mm6, mm3    // create mask over copy
            pcmpgtw     mm7, mm5
            pand        mm0, mm6    // keep mm0 parts from mask in mm6
            pand        mm2, mm7
            pandn       mm6, mm3    // keep mm3 parts from ~mask in mm6 
            pandn       mm7, mm5

            movq        mm3, mm1
            por         mm0, mm2
            pcmpgtw     mm3, mm4
            por         mm6, mm7
            pand        mm1, mm3
            por         mm0, mm6
            pandn       mm3, mm4
            por         mm0, mm1
            por         mm0, mm3

            add			esi, 8      // Advance to the next height pixels.
            movq	      [edi], mm0

            add			edi, 8      // Advance to the next height pixels.

            //
            // Loop again or break.
            //
            dec			ecx
            jnz			do_next_block

done_pixels_blocks:

         pop         ecx

            pop         edi
            pop         esi
            emms
      }

      // Increment pointers to remainder positions
      lpwM += NumBlocks * 4;
      lpwL += NumBlocks * 4;

      for (int i = 0; i < Remainder; ++i)
      {
         WORD wDCol = 0;

         WORD wSRed = *lpwM & 0x7C00;
         WORD wDRed = *lpwL & 0x7C00;

         wDCol = (wSRed > wDRed) ? wDCol | wSRed : wDCol | wDRed;

         WORD wSGreen = *lpwM & 0x03E0;
         WORD wDGreen = *lpwL & 0x03E0;

         wDCol = (wSGreen > wDGreen) ? wDCol | wSGreen : wDCol | wDGreen;

         WORD wSBlue = *lpwM & 0x001F;
         WORD wDBlue = *lpwL & 0x001F;

         wDCol = (wSBlue > wDBlue) ? wDCol | wSBlue : wDCol | wDBlue;

         *lpwL = wDCol;

         ++lpwM;
         ++lpwL;
      }
      lpwM += (xEnd + xCut);
      lpwL += (dwWDiv2 - dwW);
   }
}


void LightMap::MakeLightingFX_Base(LPBYTE lpbDest, LPBYTE lpbSource, long lDPitch, long lSPitch, int xPos, int yPos) 
{
	int j;
	LPWORD  lpwDest    = (LPWORD)lpbDest;
	LPWORD  lpwSource  = (LPWORD)lpbSource;
	LPWORD  lpwLight   = lpwLightMap;
	WORD wRMask = lpDXDirectDraw->wRMask;
	WORD wGMask = lpDXDirectDraw->wGMask;
	WORD wBMask = lpDXDirectDraw->wBMask;
	int dwWDiv2 = g_Global.GetScreenW();

	int iSOffset = (lSPitch/2)-(g_Global.GetScreenW());
	int iDOffset = (lDPitch/2)-(g_Global.GetScreenW());

	for (j = 0; j < g_Global.GetScreenH(); j++) 
	{
		for (int i = 0; i < dwWDiv2; ++i)
		{
			WORD wLight = *lpwLight;
			WORD wLightR = wLight & 0x7C00;
			WORD wLightG = wLight & 0x03E0;
			WORD wLightB = wLight & 0x001F;

			WORD wSrc  = *lpwSource;
			WORD wSrcR = wSrc & wRMask;
			WORD wSrcG = wSrc & wGMask;
			WORD wSrcB = wSrc & wBMask;

			WORD wColor = (((wSrcR * wLightR) >> 15) & wRMask) +
				(((wSrcG * wLightG) >> 10) & wGMask) +
				(((wSrcB * wLightB) >>  5) & wBMask);
			*lpwDest = wColor;

			++lpwSource;
			++lpwDest;

			++lpwLight;
		}
		lpwSource += iSOffset;
		lpwDest   += iDOffset;
	}
}

void LightMap::MakeLightingFX_BaseLow(LPBYTE lpbDest, LPBYTE lpbSource, long lDPitch, long lSPitch, int xPos, int yPos) 
{
   int j;
   LPWORD  lpwDest    = (LPWORD)lpbDest;
   LPWORD  lpwSource  = (LPWORD)lpbSource;
   LPWORD  lpwLight   = lpwLightMap;
   WORD wRMask = lpDXDirectDraw->wRMask;
   WORD wGMask = lpDXDirectDraw->wGMask;
   WORD wBMask = lpDXDirectDraw->wBMask;
   
   int dwWDiv2 = g_Global.GetScreenWDiv2();

   int iSOffset = (lSPitch/2)-(g_Global.GetScreenW());
   int iDOffset = (lDPitch/2)-(g_Global.GetScreenW());
   
   for (j = 0; j < g_Global.GetScreenH()+yPos; j++) 
   {
      for (int i = 0; i < dwWDiv2; ++i)
      {
         WORD wLight = *lpwLight;
         WORD wLightR = wLight & 0x7C00;
         WORD wLightG = wLight & 0x03E0;
         WORD wLightB = wLight & 0x001F;
         
         WORD wSrc  = *lpwSource;
         WORD wSrcR = wSrc & wRMask;
         WORD wSrcG = wSrc & wGMask;
         WORD wSrcB = wSrc & wBMask;
         
         WORD wColor = (((wSrcR * wLightR) >> 15) & wRMask) +
            (((wSrcG * wLightG) >> 10) & wGMask) +
            (((wSrcB * wLightB) >>  5) & wBMask);
         *lpwDest = wColor;
         
         ++lpwSource;
         ++lpwDest;
         
         wSrc  = *lpwSource;
         wSrcR = wSrc & wRMask;
         wSrcG = wSrc & wGMask;
         wSrcB = wSrc & wBMask;
         
         wColor = (((wSrcR * wLightR) >> 15) & wRMask) +
            (((wSrcG * wLightG) >> 10) & wGMask) +
            (((wSrcB * wLightB) >>  5) & wBMask);
         *lpwDest = wColor;
         
         ++lpwSource;
         ++lpwDest;
         
         ++lpwLight;
      }
      lpwLight -= (1 - (j & 0x1)) * dwWDiv2;
	  lpwSource += iSOffset;
	  lpwDest   += iDOffset;
   } 
}

void LightMap::MakeLightingFX_SSE2(LPBYTE lpbDest, LPBYTE lpbSource, long lDPitch, long lSPitch, int xPos, int yPos) 
{
   int j;
   LPWORD  lpwDest    = (LPWORD)lpbDest;
   LPWORD  lpwSource  = (LPWORD)lpbSource;
   LPWORD  lpwLight   = lpwLightMap;
   WORD wRMask = lpDXDirectDraw->wRMask;
   WORD wGMask = lpDXDirectDraw->wGMask;
   WORD wBMask = lpDXDirectDraw->wBMask;

   lpwDest += (xPos);
   lpwSource += (xPos/2) + ((lSPitch/2)*(-yPos/2));
   lpwLight += (xPos/2) + (g_Global.GetScreenW()*(-yPos/2));

   int iSOffset = (lSPitch/2)-(g_Global.GetScreenW());
   int iDOffset = (lDPitch/2)-(g_Global.GetScreenW());
   int SizeX = g_Global.GetScreenW();

   int Remainder = (SizeX & 0x07); 
   int NumBlocks = (SizeX & ~0x07) / 8;

   const unsigned int mu1_565 = 0xf800f800;
   const unsigned int mu2_565 = 0x07e007e0;
   const unsigned int mu3_565 = 0x001f001f;

   const unsigned int mu1_555 = 0x7c007c00;
   const unsigned int mu2_555 = 0x03e003e0;
   const unsigned int mu3_555 = 0x001f001f;
   
   const unsigned int ou1_555 = 0x001f001f;
   const unsigned int ou2_555 = 0x001f001f;
   const unsigned int ou3_555 = 0x001f001f;
   
   // Loading masks
   __m128i REDMASK_565   = _mm_set_epi32(mu1_565, mu1_565, mu1_565, mu1_565);
   __m128i GREENMASK_565 = _mm_set_epi32(mu2_565, mu2_565, mu2_565, mu2_565);
   __m128i BLUEMASK_565  = _mm_set_epi32(mu3_565, mu3_565, mu3_565, mu3_565);
   
   __m128i REDMASK_555   = _mm_set_epi32(mu1_555, mu1_555, mu1_555, mu1_555);
   __m128i GREENMASK_555 = _mm_set_epi32(mu2_555, mu2_555, mu2_555, mu2_555);
   __m128i BLUEMASK_555  = _mm_set_epi32(mu3_555, mu3_555, mu3_555, mu3_555);
   
   __m128i OFFSETRED_555   = _mm_set_epi32(ou1_555, ou1_555, ou1_555, ou1_555);
   __m128i OFFSETGREEN_555 = _mm_set_epi32(ou2_555, ou2_555, ou2_555, ou2_555);
   __m128i OFFSETBLUE_555  = _mm_set_epi32(ou3_555, ou3_555, ou3_555, ou3_555);

   for (j = 0; j < g_Global.GetScreenH()+yPos; j++) 
   {
      __asm
      {
         push        edi
         push        esi
         push        edx

         push        ecx

         mov         ecx, NumBlocks
         mov         edi, lpwDest
         mov         esi, lpwSource
         mov         edx, lpwLight

         cmp         ecx, 0
         je          done_pixels_blocks

do_next_block:

         movdqu      xmm0, byte ptr [edx]  // light
         movdqu      xmm3, byte ptr [esi]  // src color
         movdqu      xmm1, xmm0
         movdqu      xmm4, xmm3
         movdqu      xmm2, xmm0
         movdqu      xmm5, xmm3

         pand        xmm0, REDMASK_555     // no need if 24bpp light map, but some unpacking still needed...
         pand        xmm1, GREENMASK_555   // no need if 24bpp light map, ...
         pand        xmm2, BLUEMASK_555    // no need if 24bpp light map, ...
         pand        xmm3, REDMASK_565
         pand        xmm4, GREENMASK_565
         pand        xmm5, BLUEMASK_565

         psrlw       xmm0, 10              // no need if 24bpp light map, ...
         psrlw       xmm1, 5               // no need if 24bpp light map, ...
         
         psrlw       xmm3, 11
         psrlw       xmm4, 5

         pmullw      xmm0, xmm3
         pmullw      xmm1, xmm4
         pmullw      xmm2, xmm5
         
         paddw       xmm0, OFFSETRED_555
         paddw       xmm1, OFFSETGREEN_555
         paddw       xmm2, OFFSETBLUE_555

         // Divide by 32
         psraw       xmm0, 5
         psraw       xmm1, 5
         psraw       xmm2, 5

         psllw       xmm0, 11
         psllw       xmm1, 5
         por         xmm0, xmm2
         add         edx, 16      // Advance to the next eight pixels.
         por         xmm0, xmm1
         add         esi, 16      // Advance to the next eight pixels.

         movdqu      [edi], xmm0

         add         edi, 16      // Advance to the next eight pixels.
         
         //
         // Loop again or break.
         //
         dec			ecx
         jnz			do_next_block

done_pixels_blocks:
         
         pop         ecx
            
         pop         edx
         pop         edi
         pop         esi
      }

      // Increment pointers to remainder positions
      lpwSource += NumBlocks * 8;
      lpwDest   += NumBlocks * 8;
      lpwLight  += NumBlocks * 8;

      for (int i = 0; i < Remainder; ++i)
      {
         WORD wLight = *lpwLight;
         WORD wLightR = wLight & 0x7C00;
         WORD wLightG = wLight & 0x03E0;
         WORD wLightB = wLight & 0x001F;

         WORD wSrc  = *lpwSource;
         WORD wSrcR = wSrc & wRMask;
         WORD wSrcG = wSrc & wGMask;
         WORD wSrcB = wSrc & wBMask;

         WORD wColor = (((wSrcR * wLightR) >> 15) & wRMask) +
                       (((wSrcG * wLightG) >> 10) & wGMask) +
                       (((wSrcB * wLightB) >>  5) & wBMask);
         *lpwDest = wColor;

         ++lpwSource;
         ++lpwDest;

         ++lpwLight;
      }
	  lpwSource += iSOffset;
	  lpwDest   += iDOffset;
   } 
}

void LightMap::MakeLightingFX_MMX(LPBYTE lpbDest, LPBYTE lpbSource, long lDPitch, long lSPitch, int xPos, int yPos) 
{
   int j;
   LPWORD  lpwDest    = (LPWORD)lpbDest;
   LPWORD  lpwSource  = (LPWORD)lpbSource;
   LPWORD  lpwLight   = lpwLightMap;
   WORD wRMask = lpDXDirectDraw->wRMask;
   WORD wGMask = lpDXDirectDraw->wGMask;
   WORD wBMask = lpDXDirectDraw->wBMask;

   lpwDest += (xPos);
   lpwSource += (xPos/2) + ((lSPitch/2)*(-yPos/2));
   lpwLight += (xPos/2) + (g_Global.GetScreenW()*(-yPos/2));

   int iSOffset = (lSPitch/2)-(g_Global.GetScreenW());
   int iDOffset = (lDPitch/2)-(g_Global.GetScreenW());
   int SizeX = g_Global.GetScreenW();

   int Remainder = (SizeX & 0x03); 
   int NumBlocks = (SizeX & ~0x03) / 4;

   const unsigned int mu1_565 = 0xf800f800;
   const unsigned int mu2_565 = 0x07e007e0;
   const unsigned int mu3_565 = 0x001f001f;

   const unsigned int ou1_565 = 0x001f001f;
   const unsigned int ou2_565 = 0x003f003f;
   const unsigned int ou3_565 = 0x001f001f;

   const unsigned int mu1_555 = 0x7c007c00;
   const unsigned int mu2_555 = 0x03e003e0;
   const unsigned int mu3_555 = 0x001f001f;

   const unsigned int ou1_555 = 0x001f001f;
   const unsigned int ou2_555 = 0x001f001f;
   const unsigned int ou3_555 = 0x001f001f;

   // Loading masks
   unsigned int REDMASK_565[2]   = { mu1_565, mu1_565 };
   unsigned int GREENMASK_565[2] = { mu2_565, mu2_565 };
   unsigned int BLUEMASK_565[2]  = { mu3_565, mu3_565 };

   unsigned int REDMASK_555[2]   = { mu1_555, mu1_555 };
   unsigned int GREENMASK_555[2] = { mu2_555, mu2_555 };
   unsigned int BLUEMASK_555[2]  = { mu3_555, mu3_555 };

   unsigned int OFFSETRED_555[2]   = { ou1_555, ou1_555 };
   unsigned int OFFSETGREEN_555[2] = { ou2_555, ou2_555 };
   unsigned int OFFSETBLUE_555[2]  = { ou3_555, ou3_555 };

   for (j = 0; j < g_Global.GetScreenH()+yPos; j++) 
   {
      __asm
      {
         push        edi
            push        esi
            push        edx

            push        ecx

            mov         ecx, NumBlocks
            mov         edi, lpwDest
            mov         esi, lpwSource
            mov         edx, lpwLight

            cmp         ecx, 0
            je          done_pixels_blocks

do_next_block:

         movq        mm0, byte ptr [edx]  // light
         movq        mm3, byte ptr [esi]  // src color
         movq        mm1, mm0
            movq        mm4, mm3
            movq        mm2, mm0
            movq        mm5, mm3

            pand        mm0, REDMASK_555     // no need if 24bpp light map, but some unpacking still needed...
            pand        mm1, GREENMASK_555   // no need if 24bpp light map, ...
            pand        mm2, BLUEMASK_555    // no need if 24bpp light map, ...
            pand        mm3, REDMASK_565
            pand        mm4, GREENMASK_565
            pand        mm5, BLUEMASK_565

            psrlw       mm0, 10              // no need if 24bpp light map, ...
            psrlw       mm1, 5               // no need if 24bpp light map, ...

            psrlw       mm3, 11
            psrlw       mm4, 5

            pmullw      mm0, mm3
            pmullw      mm1, mm4
            pmullw      mm2, mm5

            paddw       mm0, OFFSETRED_555
            paddw       mm1, OFFSETGREEN_555
            paddw       mm2, OFFSETBLUE_555

            // Divide by 32
            psraw		   mm0, 5
            psraw		   mm1, 5
            psraw		   mm2, 5

            psllw       mm0, 11
            psllw       mm1, 5
            por         mm0, mm2
            add         edx, 8      // Advance to the next height pixels.
            por         mm0, mm1
            add			esi, 8      // Advance to the next height pixels.

            movq	      [edi], mm0

            add			edi, 8      // Advance to the next height pixels.

            //
            // Loop again or break.
            //
            dec			ecx
            jnz			do_next_block

done_pixels_blocks:

         pop         ecx

            pop         edx
            pop         edi
            pop         esi
            emms
      }

      // Increment pointers to remainder positions
      lpwSource += NumBlocks * 4;
      lpwDest   += NumBlocks * 4;
      lpwLight  += NumBlocks * 4;

      for (int i = 0; i < Remainder; ++i)
      {
         WORD wLight = *lpwLight;
         WORD wLightR = wLight & 0x7C00;
         WORD wLightG = wLight & 0x03E0;
         WORD wLightB = wLight & 0x001F;

         WORD wSrc  = *lpwSource;
         WORD wSrcR = wSrc & wRMask;
         WORD wSrcG = wSrc & wGMask;
         WORD wSrcB = wSrc & wBMask;

         WORD wColor = (((wSrcR * wLightR) >> 15) & wRMask) +
            (((wSrcG * wLightG) >> 10) & wGMask) +
            (((wSrcB * wLightB) >>  5) & wBMask);
         *lpwDest = wColor;

         ++lpwSource;
         ++lpwDest;

         ++lpwLight;
      }
      lpwSource += iSOffset;
      lpwDest   += iDOffset;
   } 
}


void LightMap::MakeLightingFX_SSE2Low(LPBYTE lpbDest, LPBYTE lpbSource, long lDPitch, long lSPitch, int xPos, int yPos) 
{
   int j;
   LPWORD  lpwDest    = (LPWORD)lpbDest;
   LPWORD  lpwSource  = (LPWORD)lpbSource;
   LPWORD  lpwLight   = lpwLightMap;
   WORD wRMask = lpDXDirectDraw->wRMask;
   WORD wGMask = lpDXDirectDraw->wGMask;
   WORD wBMask = lpDXDirectDraw->wBMask;

   lpwDest += (xPos);
   lpwSource += (xPos/2) + ((lSPitch/2)*(-yPos/2));
   lpwLight += (xPos/4) + (g_Global.GetScreenWDiv2()*(-yPos/4));

   int dwWDiv2 = g_Global.GetScreenWDiv2();
   int SizeX = g_Global.GetScreenWDiv2();
   int iSOffset = (lSPitch/2)-(g_Global.GetScreenW());
   int iDOffset = (lDPitch/2)-(g_Global.GetScreenW());

   int Remainder = (SizeX & 0x07); 
   int NumBlocks = (SizeX & ~0x07) / 8 * 2;

   const unsigned int mu1_565 = 0xf800f800;
   const unsigned int mu2_565 = 0x07e007e0;
   const unsigned int mu3_565 = 0x001f001f;
   
   const unsigned int mu1_555 = 0x7c007c00;
   const unsigned int mu2_555 = 0x03e003e0;
   const unsigned int mu3_555 = 0x001f001f;
   
   const unsigned int ou1_555 = 0x001f001f;
   const unsigned int ou2_555 = 0x001f001f;
   const unsigned int ou3_555 = 0x001f001f;
   
   // Loading masks
   __m128i REDMASK_565   = _mm_set_epi32(mu1_565, mu1_565, mu1_565, mu1_565);
   __m128i GREENMASK_565 = _mm_set_epi32(mu2_565, mu2_565, mu2_565, mu2_565);
   __m128i BLUEMASK_565  = _mm_set_epi32(mu3_565, mu3_565, mu3_565, mu3_565);

   __m128i REDMASK_555   = _mm_set_epi32(mu1_555, mu1_555, mu1_555, mu1_555);
   __m128i GREENMASK_555 = _mm_set_epi32(mu2_555, mu2_555, mu2_555, mu2_555);
   __m128i BLUEMASK_555  = _mm_set_epi32(mu3_555, mu3_555, mu3_555, mu3_555);

   __m128i OFFSETRED_555   = _mm_set_epi32(ou1_555, ou1_555, ou1_555, ou1_555);
   __m128i OFFSETGREEN_555 = _mm_set_epi32(ou2_555, ou2_555, ou2_555, ou2_555);
   __m128i OFFSETBLUE_555  = _mm_set_epi32(ou3_555, ou3_555, ou3_555, ou3_555);

   for (j = 0; j < g_Global.GetScreenH()+yPos; j++) 
   {
      __asm
      {
         push        edi
         push        esi
         push        edx

         push        ecx

         mov         ecx, NumBlocks
         mov         edi, lpwDest
         mov         esi, lpwSource
         mov         edx, lpwLight

         cmp         ecx, 0
         je          done_pixels_blocks

do_next_block:

         movq        xmm0, qword ptr [edx] // light
         movq        xmm1, qword ptr [edx] // light
         movq        xmm2, qword ptr [edx] // light
         movdqu      xmm3, byte ptr [esi]  // src color
         movdqu      xmm4, byte ptr [esi]  // src color
         movdqu      xmm5, byte ptr [esi]  // src color

         pand        xmm0, REDMASK_555     // no need if 24bpp light map, but some unpacking still needed...
         pand        xmm1, GREENMASK_555   // no need if 24bpp light map, ...
         pand        xmm2, BLUEMASK_555    // no need if 24bpp light map, ...
         punpcklwd   xmm0, xmm0            // Shuffle first 2 light pixels
         punpcklwd   xmm1, xmm1            // Shuffle first 2 light pixels
         punpcklwd   xmm2, xmm2            // Shuffle first 2 light pixels
         pand        xmm3, REDMASK_565
         pand        xmm4, GREENMASK_565
         pand        xmm5, BLUEMASK_565

         psrlw       xmm0, 10              // no need if 24bpp light map, ...
         psrlw       xmm1, 5               // no need if 24bpp light map, ...
         
         psrlw       xmm3, 11
         psrlw       xmm4, 5

         pmullw      xmm3, xmm0
         pmullw      xmm4, xmm1
         pmullw      xmm5, xmm2
         
         paddw       xmm3, OFFSETRED_555
         paddw       xmm4, OFFSETGREEN_555
         paddw       xmm5, OFFSETBLUE_555

         // Divide by 32
         psraw		 xmm3, 5
         psraw		 xmm4, 5
         psraw		 xmm5, 5

         psllw       xmm3, 11
         psllw       xmm4, 5
         por         xmm3, xmm5
         por         xmm3, xmm4
         add         esi, 16              // Advance to the next eight pixels.

         movdqu	     [edi], xmm3

         add         edx, 8      // Advance to the next height pixels.
         add         edi, 16      // Advance to the next height pixels.

         //
         // Loop again or break.
         //
         dec			ecx
         jnz			do_next_block

done_pixels_blocks:
         
         pop         ecx
            
         pop         edx
         pop         edi
         pop         esi
      }

      // Increment pointers to remainder positions
      lpwSource += NumBlocks * 8;
      lpwDest   += NumBlocks * 8;
      lpwLight  += NumBlocks * 8 / 2;

      for (int i = 0; i < Remainder; ++i)
      {
         WORD wLight = *lpwLight;
         WORD wLightR = wLight & 0x7C00;
         WORD wLightG = wLight & 0x03E0;
         WORD wLightB = wLight & 0x001F;

         WORD wSrc  = *lpwSource;
         WORD wSrcR = wSrc & wRMask;
         WORD wSrcG = wSrc & wGMask;
         WORD wSrcB = wSrc & wBMask;

         WORD wColor = (((wSrcR * wLightR) >> 15) & wRMask) +
                       (((wSrcG * wLightG) >> 10) & wGMask) +
                       (((wSrcB * wLightB) >>  5) & wBMask);
         *lpwDest = wColor;

         ++lpwSource;
         ++lpwDest;

         wSrc  = *lpwSource;
         wSrcR = wSrc & wRMask;
         wSrcG = wSrc & wGMask;
         wSrcB = wSrc & wBMask;

         wColor = (((wSrcR * wLightR) >> 15) & wRMask) +
                  (((wSrcG * wLightG) >> 10) & wGMask) +
                  (((wSrcB * wLightB) >>  5) & wBMask);
         *lpwDest = wColor;

         ++lpwSource;
         ++lpwDest;

         ++lpwLight;
      }
      lpwLight -= (1 - (j & 0x1)) * dwWDiv2;
	  lpwSource += iSOffset;
	  lpwDest   += iDOffset;
   } 
}

void LightMap::MakeLightingFX_MMXLow(LPBYTE lpbDest, LPBYTE lpbSource, long lDPitch, long lSPitch, int xPos, int yPos) 
{
   int j;
   LPWORD  lpwDest    = (LPWORD)lpbDest;
   LPWORD  lpwSource  = (LPWORD)lpbSource;
   LPWORD  lpwLight   = lpwLightMap;
   WORD wRMask = lpDXDirectDraw->wRMask;
   WORD wGMask = lpDXDirectDraw->wGMask;
   WORD wBMask = lpDXDirectDraw->wBMask;

   lpwDest += (xPos);
   lpwSource += (xPos/2) + ((lSPitch/2)*(-yPos/2));
   lpwLight += (xPos/4) + (g_Global.GetScreenWDiv2()*(-yPos/4));

   int dwWDiv2 = g_Global.GetScreenWDiv2();
   int SizeX = g_Global.GetScreenWDiv2();
   int iSOffset = (lSPitch/2)-(g_Global.GetScreenW());
   int iDOffset = (lDPitch/2)-(g_Global.GetScreenW());

   int Remainder = (SizeX & 0x03); 
   int NumBlocks = (SizeX & ~0x03) / 4;

   const unsigned int mu1_565 = 0xf800f800;
   const unsigned int mu2_565 = 0x07e007e0;
   const unsigned int mu3_565 = 0x001f001f;

   const unsigned int ou1_565 = 0x001f001f;
   const unsigned int ou2_565 = 0x003f003f;
   const unsigned int ou3_565 = 0x001f001f;

   const unsigned int mu1_555 = 0x7c007c00;
   const unsigned int mu2_555 = 0x03e003e0;
   const unsigned int mu3_555 = 0x001f001f;

   const unsigned int ou1_555 = 0x001f001f;
   const unsigned int ou2_555 = 0x001f001f;
   const unsigned int ou3_555 = 0x001f001f;

   // Loading masks
   unsigned int REDMASK_565[2]   = { mu1_565, mu1_565 };
   unsigned int GREENMASK_565[2] = { mu2_565, mu2_565 };
   unsigned int BLUEMASK_565[2]  = { mu3_565, mu3_565 };

   unsigned int REDMASK_555[2]   = { mu1_555, mu1_555 };
   unsigned int GREENMASK_555[2] = { mu2_555, mu2_555 };
   unsigned int BLUEMASK_555[2]  = { mu3_555, mu3_555 };

   unsigned int OFFSETRED_555[2]   = { ou1_555, ou1_555 };
   unsigned int OFFSETGREEN_555[2] = { ou2_555, ou2_555 };
   unsigned int OFFSETBLUE_555[2]  = { ou3_555, ou3_555 };

   for (j = 0; j < g_Global.GetScreenH()+yPos; j++) 
   {
      __asm
      {
         push        edi
            push        esi
            push        edx

            push        ecx

            mov         ecx, NumBlocks
            mov         edi, lpwDest
            mov         esi, lpwSource
            mov         edx, lpwLight

            cmp         ecx, 0
            je          done_pixels_blocks

do_next_block:

         movq        mm0, byte ptr [edx]  // light
         movq        mm3, byte ptr [esi]  // src color
         movq        mm1, mm0
            movq        mm4, mm3
            movq        mm2, mm0
            movq        mm5, mm3
            movq        mm6, mm0             // Copy of 4 pixels light

            pand        mm0, REDMASK_555     // no need if 24bpp light map, but some unpacking still needed...
            pand        mm1, GREENMASK_555   // no need if 24bpp light map, ...
            pand        mm2, BLUEMASK_555    // no need if 24bpp light map, ...
            pshufw      mm0, mm0, 0x50       // Shuffle first 2 light pixels
            pshufw      mm1, mm1, 0x50       // Shuffle first 2 light pixels
            pshufw      mm2, mm2, 0x50       // Shuffle first 2 light pixels
            pand        mm3, REDMASK_565
            pand        mm4, GREENMASK_565
            pand        mm5, BLUEMASK_565

            psrlw       mm0, 10              // no need if 24bpp light map, ...
            psrlw       mm1, 5               // no need if 24bpp light map, ...

            psrlw       mm3, 11
            psrlw       mm4, 5

            pmullw      mm3, mm0
            pmullw      mm4, mm1
            pmullw      mm5, mm2

            paddw       mm3, OFFSETRED_555
            paddw       mm4, OFFSETGREEN_555
            paddw       mm5, OFFSETBLUE_555

            // Divide by 32
            psraw		   mm3, 5
            psraw		   mm4, 5
            psraw		   mm5, 5

            psllw       mm3, 11
            psllw       mm4, 5
            por         mm3, mm5
            por         mm3, mm4
            add			esi, 8      // Advance to the next height pixels.

            movq        mm0, mm6             // Restore 4 pixels light
            movq        mm1, mm6             // Restore 4 pixels light
            movq        mm2, mm6             // Restore 4 pixels light

            movq	      [edi], mm3

            pand        mm0, REDMASK_555     // no need if 24bpp light map, but some unpacking still needed...
            pand        mm1, GREENMASK_555   // no need if 24bpp light map, ...
            pand        mm2, BLUEMASK_555    // no need if 24bpp light map, ...
            pshufw      mm0, mm0, 0xFA       // Shuffle second 2 light pixels
            pshufw      mm1, mm1, 0xFA       // Shuffle second 2 light pixels
            pshufw      mm2, mm2, 0xFA       // Shuffle second 2 light pixels

            movq        mm3, byte ptr [esi]  // src color
         movq        mm4, mm3
            movq        mm5, mm3

            psrlw       mm0, 10              // no need if 24bpp light map, ...
            psrlw       mm1, 5               // no need if 24bpp light map, ...

            pand        mm3, REDMASK_565
            pand        mm4, GREENMASK_565
            pand        mm5, BLUEMASK_565

            psrlw       mm3, 11
            psrlw       mm4, 5

            pmullw      mm3, mm0
            pmullw      mm4, mm1
            pmullw      mm5, mm2

            paddw       mm3, OFFSETRED_555
            paddw       mm4, OFFSETGREEN_555
            paddw       mm5, OFFSETBLUE_555

            // Divide by 32
            psraw		   mm3, 5
            psraw		   mm4, 5
            psraw		   mm5, 5

            psllw       mm3, 11
            psllw       mm4, 5
            por         mm3, mm5
            add			edi, 8      // Advance to the next height pixels.
            por         mm3, mm4
            add			esi, 8      // Advance to the next height pixels.

            movq	      [edi], mm3

            add         edx, 8      // Advance to the next height pixels.
            add			edi, 8      // Advance to the next height pixels.

            //
            // Loop again or break.
            //
            dec			ecx
            jnz			do_next_block

done_pixels_blocks:

         pop         ecx

            pop         edx
            pop         edi
            pop         esi
            emms
      }

      // Increment pointers to remainder positions
      lpwSource += NumBlocks * 4 * 2;
      lpwDest   += NumBlocks * 4 * 2;
      lpwLight  += NumBlocks * 4;

      for (int i = 0; i < Remainder; ++i)
      {
         WORD wLight = *lpwLight;
         WORD wLightR = wLight & 0x7C00;
         WORD wLightG = wLight & 0x03E0;
         WORD wLightB = wLight & 0x001F;

         WORD wSrc  = *lpwSource;
         WORD wSrcR = wSrc & wRMask;
         WORD wSrcG = wSrc & wGMask;
         WORD wSrcB = wSrc & wBMask;

         WORD wColor = (((wSrcR * wLightR) >> 15) & wRMask) +
            (((wSrcG * wLightG) >> 10) & wGMask) +
            (((wSrcB * wLightB) >>  5) & wBMask);
         *lpwDest = wColor;

         ++lpwSource;
         ++lpwDest;

         wSrc  = *lpwSource;
         wSrcR = wSrc & wRMask;
         wSrcG = wSrc & wGMask;
         wSrcB = wSrc & wBMask;

         wColor = (((wSrcR * wLightR) >> 15) & wRMask) +
            (((wSrcG * wLightG) >> 10) & wGMask) +
            (((wSrcB * wLightB) >>  5) & wBMask);
         *lpwDest = wColor;

         ++lpwSource;
         ++lpwDest;

         ++lpwLight;
      }
      lpwLight -= (1 - (j & 0x1)) * dwWDiv2;
      lpwSource += iSOffset;
      lpwDest   += iDOffset;
   } 
}
