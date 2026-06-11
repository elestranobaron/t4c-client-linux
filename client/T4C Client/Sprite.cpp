// ************************************************************************
// ***                                                                  ***
//      File Name: Sprite.cpp
//      Project:   TFC Client
//      Creation:  29 June, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***
//      Change History
//
//         Date            Ver.	  Author	     Purpose
//         ----            ----    ------      -------
//         06-29-1997      1.0     TH          Initial development
//
//      Description
//         Provide fonction to load a Sprite from a Image or an ID
// ***                                                                  ***
//	************************************************************************
// *** Copyright (c) 1997-1998 Vircom. All rights reserved.             ***      
 
#include "pch.h"
#pragma warning( disable: 4786 )
#include "Sprite.h"
#include "V2Database.h"
#include "MacroHandler.h"
#include "App.h"
#include "Global.h"
#include "V2PalManager.h"
#include "MemMapFile.h"
#include "SaveGame.h"
#include "Zlib/zlib.h"
extern CSaveGame g_SaveGame;


// Function Member
Sprite::Sprite(void) {
// Constructeur
	lpSpriteSurface  = NULL;
   
   lpSpritePal      = NULL;

   Dither = FALSE;


}

Sprite::~Sprite(void) {
// Destructeur
   Release();
	
}

void Sprite::Release(void) {
// Release Memory
   if (lpSpriteSurface && !g_boQuitApp) 
   {
		lpSpriteSurface->Release(); // Ligne Succeptible de causer de serieux bug
		lpSpriteSurface = NULL;
	}

   if (lpSpritePal)
      delete lpSpritePal;
   lpSpritePal = NULL;

}

LPWORD Sprite::GetSurfaceMemory(VOID) {
   DDSURFACEDESC2 ddsd;
   ZeroMemory(&ddsd, sizeof(ddsd));
   ddsd.dwSize = sizeof(ddsd);

   lpSpriteSurface->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

   return (LPWORD)(ddsd.lpSurface);
}

void Sprite::ReleaseSurfaceMemory(LPWORD lpb) {
   lpSpriteSurface->Unlock(NULL);
}

void Sprite::Create(int W, int H, BOOL f, int Color, int Xc, int Yc, int XFc, int YFc) {
   DDSURFACEDESC2 Sprite;
	LPDDCOLORKEY lpddColorKey;

	Width = W;
	Height = H;

	fTransparent = f;
   TransColor = Color;
	xCor = Xc;
	yCor = Yc;
	FlipxCor = XFc;
	FlipyCor = YFc;

	SpriteRect.left            = 0;
	SpriteRect.top             = 0;
   SpriteRect.right           = Width;
	SpriteRect.bottom          = Height;

   ZeroMemory(&Sprite, sizeof(Sprite));
   Sprite.dwSize            = sizeof(Sprite);
   Sprite.dwFlags           = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	Sprite.ddsCaps.dwCaps    = DDSCAPS_SYSTEMMEMORY;
   Sprite.dwHeight          = Height;
	Sprite.dwWidth           = Width;
   
   try {
   DXDErrorHandle(lpDXDirectDraw->CreateSurfaceNM(&Sprite, &lpSpriteSurface, NULL), "Create Sprite");
   } catch (...) {

   exit(0);
   }

	lpddColorKey = new DDCOLORKEY;
   lpddColorKey->dwColorSpaceHighValue = TransColor;
   lpddColorKey->dwColorSpaceLowValue  = TransColor;
	
   if(lpSpriteSurface)
   {
      DXDErrorHandle(lpSpriteSurface->SetColorKey(DDCKEY_SRCBLT, lpddColorKey), "Create Sprite 2");

      DDSURFACEDESC2 Sprite2;
      memset(&Sprite2, 0, sizeof(Sprite2));
      Sprite2.dwSize = sizeof(Sprite2);	
      DXDErrorHandle(lpSpriteSurface->Lock(NULL, &Sprite2, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL), "Draw Overlap");

      if(Sprite2.lpSurface)
      {
         for (int i = 0; i < Sprite2.lPitch*Height/2; i++)
            ((LPWORD)Sprite2.lpSurface)[i] = (WORD)TransColor;
         lpSpriteSurface->Unlock(NULL);
      }
   }

   delete lpddColorKey;
}
	
struct V2Sprite_sprite_header
{		
   unsigned short	dwCompType;	 // 1=unpack 2=pack 9 =Zip
   unsigned short	flag1;		 
   unsigned short	dwWidth;		 
   unsigned short	dwHeight; 	 
   short			   shOffX1;		 
   short			   shOffY1;		 
   short			   shOffX2;		 
   short			   shOffY2;		 
   unsigned short	ushTransparency;
   unsigned short	ushTransColor;	// Index de la couleur transparente
   unsigned long	dwDataUnpack;
   unsigned long	dwDataPack;
};

void Sprite::LoadSprite(char *ID) 
{
// Load a Sprite rom a SpriteID
	// Initialisation de Variable.
	DDSURFACEDESC2 Sprite;
	LPDDCOLORKEY lpddColorKey;

   signed long offset;
   char database;
	
   g_V2Database.GetOffsetAndDB(ID, offset, database);
   
   V2Sprite_sprite_header Junk;

   g_V2Database.m_pDataBuffer[database].CpyMemory(&Junk,sizeof(Junk),offset);
   Junk.dwCompType      ^= 0xAAAA;
   Junk.flag1           ^= 0x1458;
   Junk.dwWidth         ^= 0x1234;
   Junk.dwHeight        ^= 0x6242;
   Junk.shOffX1         ^= 0x2355;
   Junk.shOffY1         ^= 0xF6C3;
   Junk.shOffX2         ^= 0xAAF3;
   Junk.shOffY2         ^= 0xAAAA;
   Junk.ushTransparency ^= 0x4321;
   Junk.ushTransColor   ^= 0x1234;
   Junk.dwDataUnpack    ^= 0xDDCCBBAA;
   Junk.dwDataPack      ^= 0xAABBCCDD;
   
   int dwOffsetData = offset+sizeof(V2Sprite_sprite_header);
   
   
   // Read Header
   Width = Junk.dwWidth;
   Height = Junk.dwHeight;


   xCor = Junk.shOffX1;
   yCor = Junk.shOffY1;
   FlipxCor = Junk.shOffX2;
   FlipyCor = Junk.shOffY2;
   fTransparent = Junk.ushTransparency;
   TransColor = Junk.ushTransColor;
   WORD wPal, wRed, wGreen, wBlue, wColor;
   LPBYTE lpPal = CV2PalManager::GetInstance()->GetPal("", 1);
   wRed = (lpPal[(TransColor*3)])>>lpDXDirectDraw->bRDown;
   wGreen = (lpPal[(TransColor*3)+1])>>lpDXDirectDraw->bGDown;
   wBlue = (lpPal[(TransColor*3)+2])>>lpDXDirectDraw->bBDown;
   wColor = (wRed<<lpDXDirectDraw->bRUp)+(wGreen<<lpDXDirectDraw->bGUp)+(wBlue<<lpDXDirectDraw->bBUp);
   TransColor = wColor;

   SpriteRect.left            = 0;
	SpriteRect.top             = 0;
   SpriteRect.right           = Width;
	SpriteRect.bottom          = Height;

   ZeroMemory(&Sprite, sizeof(Sprite));
	Sprite.dwSize            = sizeof(Sprite);
   Sprite.dwFlags           = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	Sprite.ddsCaps.dwCaps    = DDSCAPS_SYSTEMMEMORY;
   Sprite.dwHeight          = Height;
	Sprite.dwWidth           = Width;

	DXDErrorHandle(lpDXDirectDraw->CreateSurfaceNM(&Sprite, &lpSpriteSurface, NULL), "Load Sprite");
   if(!lpSpriteSurface)
      return;

	lpddColorKey = new DDCOLORKEY;
   lpddColorKey->dwColorSpaceHighValue = TransColor;
   lpddColorKey->dwColorSpaceLowValue = TransColor;
	
	DXDErrorHandle(lpSpriteSurface->SetColorKey(DDCKEY_SRCBLT, lpddColorKey), "Load Sprite 2");

	memset(&Sprite, 0, sizeof(Sprite));
	Sprite.dwSize = sizeof(Sprite);
	DXDErrorHandle(lpSpriteSurface->Lock(NULL, &Sprite, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL), "Load Sprite 3");

   if(!Sprite.lpSurface)
   {
      if(lpddColorKey)
         delete lpddColorKey;
      lpddColorKey = NULL;

      lpSpriteSurface->Unlock(NULL);
      return;
   }


   

   unsigned char *Temp2 = new unsigned char [Junk.dwDataUnpack];
   unsigned char *Temp = Temp2;

   if(Width >180 || Height >180)//:CR_NMNMNM Valid
   {
      byte *pchpack = new BYTE[Junk.dwDataPack];
      g_V2Database.m_pDataBuffer[database].CpyMemory(pchpack,Junk.dwDataPack,dwOffsetData);
      //Unpack Data
      uncompress(Temp,&Junk.dwDataUnpack,pchpack,Junk.dwDataPack);
      delete []pchpack;
      pchpack = NULL;
   }
   else
   {
      g_V2Database.m_pDataBuffer[database].CpyMemory(Temp,Junk.dwDataPack,dwOffsetData);
   }
  

	char *p = (char *)Sprite.lpSurface;
   long lPitch = Sprite.lPitch;

   for (int i = 0; i < Height; i++) {
      for (int j = 0; j < Width; j++) {
         wPal = (*(BYTE *)(Temp+j))*3;
         wRed = (lpPal[wPal])>>lpDXDirectDraw->bRDown;
         wGreen = (lpPal[wPal+1])>>lpDXDirectDraw->bGDown;
         wBlue = (lpPal[wPal+2])>>lpDXDirectDraw->bBDown;
         
         wColor = (wRed<<lpDXDirectDraw->bRUp)+(wGreen<<lpDXDirectDraw->bGUp)+(wBlue<<lpDXDirectDraw->bBUp);
         
         p[(j<<1)+1] = (BYTE)(wColor >> 8);
         p[(j<<1)] = (BYTE)(wColor & 0xFF);
      }
      p += lPitch;
      Temp += Width;
   }
	delete Temp2;
	
	lpSpriteSurface->Unlock(NULL);

   if(lpddColorKey)
	   delete lpddColorKey;
   lpddColorKey = NULL;
}

unsigned short Sprite::GetWidth(void) {
// Return Sprite Width
   return Width;
}

unsigned short Sprite::GetHeight(void) {
// Return Sprite Height
   return Height;
}

unsigned short Sprite::GetTransColor(void) {
// Return Sprite TransColor
   return TransColor;
}

LPDIRECTDRAWSURFACE7 Sprite::GetSurface(void) {
// Return SpirteSurface
   return lpSpriteSurface;
}

RECT *Sprite::GetRect(void) {
    return &SpriteRect;
}

void Sprite::GetSpriteDC(HDC *hdc) {
    lpSpriteSurface->GetDC(hdc);
}

void Sprite::ReleaseSpriteDC(HDC hdc) {
    lpSpriteSurface->ReleaseDC(hdc);
}

void Sprite::DrawSprite(int xPos, int yPos, RECT *Clip, RECT *OutBound, RECT *Wrap, LPDIRECTDRAWSURFACE7 ToBlit, char FX, signed char Format, BOOL ParamDither) {
    // Draw a Bitmap to Screen
    if (!lpSpriteSurface){       
        return;
    }
   memcpy(&sourceRect, &SpriteRect, sizeof(RECT));

	if (FX == 1) {
		destRect.left          = xPos+FlipxCor;
		destRect.top           = yPos+FlipyCor;
		destRect.right         = xPos+Width+FlipxCor;
		destRect.bottom        = yPos+Height+FlipyCor;
	} else {
		destRect.left          = xPos+xCor;
		destRect.top           = yPos+yCor;
		destRect.right         = xPos+Width+xCor;
		destRect.bottom        = yPos+Height+yCor;
	}

	if (Clip) {
	// Clip Border ?
		LeftBorder   = Clip->left;
		RightBorder  = Clip->right;
		TopBorder    = Clip->top;
		BottomBorder = Clip->bottom;

        if (destRect.left > RightBorder){
            return;
        }
		if (destRect.right > RightBorder) {
			sourceRect.right  = SpriteRect.right - (destRect.right   - RightBorder);
			destRect  .right  = RightBorder;
		}
        if (destRect.top > BottomBorder){
            return;
        }
		if (destRect.bottom > BottomBorder) {
			sourceRect.bottom = SpriteRect.bottom - (destRect.bottom - BottomBorder);
			destRect  .bottom = BottomBorder;
		}
        if (destRect.right < LeftBorder){
            return;
        }
		if (destRect.left < LeftBorder) {
			sourceRect.left  = SpriteRect.left + (LeftBorder - destRect.left);
			destRect  .left  = LeftBorder;
		}
        if (destRect.bottom < TopBorder){
            return;
        }
		if (destRect.top < TopBorder) {
			sourceRect.top  = SpriteRect.top + (TopBorder - destRect.top);
			destRect  .top  = TopBorder;
		}
	}

	if (Wrap) {
	// Wrap Sprite around the Screen ?
		LeftBorder   = Wrap->left;
		RightBorder  = Wrap->right;
		TopBorder    = Wrap->top;
		BottomBorder = Wrap->bottom;

		memset(Dest,   0, sizeof(RECT)*4);
		memset(Source, 0, sizeof(RECT)*4);

   	// Out of Bound ?
		if (destRect.left >= RightBorder) {
			destRect.left  -= RightBorder;
			destRect.right -= RightBorder;
		}
		if (destRect.right < LeftBorder) {
			destRect.left  += RightBorder;
			destRect.right += RightBorder;
		}

		if (destRect.top >= BottomBorder) {
			destRect.top    -= BottomBorder;
			destRect.bottom -= BottomBorder;
		}
		if (destRect.bottom < TopBorder) {
			destRect.top    += BottomBorder;
			destRect.bottom += BottomBorder;
		}

		if (destRect.left < LeftBorder) {
			Dest[0].left  = Dest[2].left  = RightBorder + destRect.left;
			Dest[0].right = Dest[2].right = RightBorder;
			Dest[1].left  = Dest[3].left  = LeftBorder;
			Dest[1].right = Dest[3].right = destRect.right;

			Source[0].left  = Source[2].left  = sourceRect.left;
			Source[0].right = Source[2].right = sourceRect.left - destRect.left;
			Source[1].left  = Source[3].left  = Source[0].right;
			Source[1].right = Source[3].right = sourceRect.right;
		} else if (destRect.right > RightBorder) {
			Dest[0].left  = Dest[2].left  = destRect.left;
			Dest[0].right = Dest[2].right = RightBorder;
			Dest[1].left  = Dest[3].left  = LeftBorder;
			Dest[1].right = Dest[3].right = destRect.right - RightBorder;

			Source[0].left  = Source[2].left  = sourceRect.left;
			Source[0].right = Source[2].right = sourceRect.left+RightBorder-destRect.left;
			Source[1].left  = Source[3].left  = Source[0].right;
			Source[1].right = Source[3].right = sourceRect.right;
		} else {
			Dest[0].left  = destRect.left;
			Dest[0].right = destRect.right;
			Dest[2].left  = destRect.left;
			Dest[2].right = destRect.right;

			Source[0].left  = sourceRect.left;
			Source[0].right = sourceRect.right;
			Source[2].left  = sourceRect.left;
			Source[2].right = sourceRect.right;
		}

		if (destRect.top < TopBorder) {
			Dest[0].top    = Dest[1].top    = BottomBorder + destRect.top;
			Dest[0].bottom = Dest[1].bottom = BottomBorder;
			Dest[2].top    = Dest[3].top    = TopBorder;
			Dest[2].bottom = Dest[3].bottom = destRect.bottom;

			Source[0].top    = Source[1].top    = sourceRect.top;
			Source[0].bottom = Source[1].bottom = sourceRect.top - destRect.top;
			Source[2].top    = Source[3].top    = Source[0].bottom;
			Source[2].bottom = Source[3].bottom = sourceRect.bottom;
		} else if (destRect.bottom > BottomBorder) {
			Dest[0].top    = Dest[1].top    = destRect.top;
			Dest[0].bottom = Dest[1].bottom = BottomBorder;
			Dest[2].top    = Dest[3].top    = TopBorder;
			Dest[2].bottom = Dest[3].bottom = destRect.bottom - BottomBorder;

			Source[0].top    = Source[1].top    = sourceRect.top;
			Source[0].bottom = Source[1].bottom = sourceRect.top+BottomBorder - destRect.top;
			Source[2].top    = Source[3].top    = Source[0].bottom;
			Source[2].bottom = Source[3].bottom = sourceRect.bottom;
		} else {
			Dest[0].top    = destRect.top;
			Dest[0].bottom = destRect.bottom;
			Dest[1].top    = destRect.top;
			Dest[1].bottom = destRect.bottom;

			Source[0].top    = sourceRect.top;
			Source[0].bottom = sourceRect.bottom;
			Source[1].top    = sourceRect.top;
			Source[1].bottom = sourceRect.bottom;
		}

		if (FX == 1) {
			LPDIRECTDRAWSURFACE7 lpNewSurface;
			DDSURFACEDESC2 Sprite;
			LPDDCOLORKEY lpddColorKey;
			DDBLTFX DDBltFx;

         ZeroMemory(&Sprite, sizeof(Sprite));
			Sprite.dwSize            = sizeof(Sprite);
			Sprite.dwFlags           = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			Sprite.ddsCaps.dwCaps    = DDSCAPS_SYSTEMMEMORY;
			Sprite.dwHeight          = Height;
			Sprite.dwWidth           = Width;

			DXDErrorHandle(lpDXDirectDraw->CreateSurfaceNM(&Sprite, &lpNewSurface, NULL), "Draw Sprite");

			lpddColorKey = new DDCOLORKEY;
			memset(lpddColorKey, TransColor, sizeof(DDCOLORKEY));
	
			DXDErrorHandle(lpNewSurface->SetColorKey(DDCKEY_SRCBLT, lpddColorKey), "Draw Sprite 2");

			memset(&DDBltFx, 0, sizeof(DDBltFx));
			DDBltFx.dwSize = sizeof(DDBltFx);
			DDBltFx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;
            
			lpNewSurface->Blt(NULL, lpSpriteSurface, NULL, DDBLT_WAIT | DDBLT_DDFX, &DDBltFx);

			delete lpddColorKey;

			for (int i = 0; i < 4; i++) {
				if (Format < 0) {
					int Left   = Dest[i].left;
					int Right  = Dest[i].right;
					int Top    = Dest[i].top;
					int Bottom = Dest[i].bottom;
					Dest[i].left   = Left + ((Right-Left)*(100+Format)/200); // ((100 - format) / 2) / 100
					Dest[i].right  = Left + ((Right-Left)*(100+Format)*3/200); // (((100 - format) */ 2) * 3) / 100
					Dest[i].top    = Top  + ((Bottom-Top)*(100+Format)/200); 
					Dest[i].bottom = Top  + ((Bottom-Top)*(100+Format)*3/200);
				}
				ToBlit->Blt(&Dest[i], lpNewSurface, &Source[i], DDBLT_WAIT | (fTransparent ? DDBLT_KEYSRC : NULL), NULL);
			}

			lpNewSurface->Release();
		} else {
			for (int i = 0; i < 4; i++) {
				if (Format < 0) {
					int Left   = Dest[i].left;
					int Right  = Dest[i].right;
					int Top    = Dest[i].top;
					int Bottom = Dest[i].bottom;
					Dest[i].left   = Left + ((Right-Left)*(100+Format)/200); // ((100 - format) / 2) / 100
					Dest[i].right  = Left + ((Right-Left)*(100+Format)*3/200); // (((100 - format) / 2) * 3) / 100
					Dest[i].top    = Top  + ((Bottom-Top)*(100+Format)/200); 
					Dest[i].bottom = Top  + ((Bottom-Top)*(100+Format)*3/200);
				}
                
				ToBlit->Blt(&Dest[i], lpSpriteSurface, &Source[i], DDBLT_WAIT | (fTransparent ? DDBLT_KEYSRC : NULL), NULL);
			}
		}
	} else {
		if (FX == 1) {
			LPDIRECTDRAWSURFACE7 lpNewSurface;
			DDSURFACEDESC2 Sprite;
			LPDDCOLORKEY lpddColorKey;
			DDBLTFX DDBltFx;

         ZeroMemory(&Sprite, sizeof(Sprite));
			Sprite.dwSize            = sizeof(Sprite);
			Sprite.dwFlags           = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			Sprite.ddsCaps.dwCaps    = DDSCAPS_SYSTEMMEMORY;
			Sprite.dwHeight          = Height;
			Sprite.dwWidth           = Width;
	
            DXDErrorHandle(lpDXDirectDraw->CreateSurfaceNM(&Sprite, &lpNewSurface, NULL), "Draw Sprite 3");
            
            lpddColorKey = new DDCOLORKEY;
            memset(lpddColorKey, TransColor, sizeof(DDCOLORKEY));
            
            DXDErrorHandle(lpNewSurface->SetColorKey(DDCKEY_SRCBLT, lpddColorKey), "Draw Sprite 4");
            
            memset(&DDBltFx, 0, sizeof(DDBltFx));
            DDBltFx.dwSize = sizeof(DDBltFx);
            DDBltFx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;
            
			lpNewSurface->Blt(NULL, lpSpriteSurface, NULL, DDBLT_WAIT | DDBLT_DDFX, &DDBltFx);

			delete lpddColorKey;

			if (Format < 0) {
				int Left   = destRect.left;
				int Right  = destRect.right;
				int Top    = destRect.top;
				int Bottom = destRect.bottom;
				destRect.left   = Left + ((Right-Left)*(100+Format)/200); // ((100 - format) / 2) / 100
				destRect.right  = Left + ((Right-Left)*(100+Format)*3/200); // (((100 - format) / 2) * 3) / 100
				destRect.top    = Top  + ((Bottom-Top)*(100+Format)/200); 
				destRect.bottom = Top  + ((Bottom-Top)*(100+Format)*3/200);
			}
            if (Format < 0){
                ToBlit->Blt(&destRect, lpNewSurface, &sourceRect, DDBLT_WAIT | (fTransparent ? DDBLT_KEYSRC : NULL), NULL);
            }else{
                ToBlit->BltFast(destRect.left, destRect.top, lpNewSurface, &sourceRect, DDBLTFAST_WAIT | (fTransparent ? DDBLTFAST_SRCCOLORKEY : NULL));
            }
            lpNewSurface->Release();
        } else  {
            if (Format < 0) {
                int Left   = destRect.left;
                int Right  = destRect.right;
                int Top    = destRect.top;
                int Bottom = destRect.bottom;
                destRect.left   = Left + ((Right-Left)*(100+Format)/200); // ((100 - format) / 2) / 100
                destRect.right  = Left + ((Right-Left)*(100+Format)*3/200); // (((100 - format) / 2) * 3) / 100
                destRect.top    = Top  + ((Bottom-Top)*(100+Format)/200); 
                destRect.bottom = Top  + ((Bottom-Top)*(100+Format)*3/200);
            }
            if (Format < 0) {

			   ToBlit->Blt(&destRect, lpSpriteSurface, &sourceRect, DDBLT_WAIT | (fTransparent ? DDBLT_KEYSRC : NULL), NULL);
         } 
            else 
            {
               if ((g_SaveGame.GetOptionsParam()->bDitherGraph && Dither) || ParamDither) 
               {
                  if((destRect.right -destRect.left) >1 && (destRect.bottom -destRect.top) >0)
                  {
                     LPDIRECTDRAWSURFACE7 lpNewSurface;
                     DDSURFACEDESC2 Sprite;
                  

                     //Construit un sprite temporaire, on blit la zone du back dans ce sprite...
                     ZeroMemory(&Sprite, sizeof(Sprite));
                     Sprite.dwSize            = sizeof(Sprite);
                     Sprite.dwFlags           = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
                     Sprite.ddsCaps.dwCaps    = DDSCAPS_SYSTEMMEMORY;
                     Sprite.dwHeight          = destRect.bottom-destRect.top;
                     Sprite.dwWidth           = destRect.right -destRect.left;
                  
                     DXDErrorHandle(lpDXDirectDraw->CreateSurfaceNM(&Sprite, &lpNewSurface, NULL), "Draw Sprite 3");
                  
                  
                     RECT destRectTmp;
                     destRectTmp.left   = 0;
                     destRectTmp.top    = 0;
                     destRectTmp.right  = destRect.right -destRect.left;
                     destRectTmp.bottom = destRect.bottom-destRect.top;
                  
                  
                     lpNewSurface->BltFast(0,0, ToBlit, &destRect, DDBLTFAST_WAIT);
                  
                     // COntient la zone du back en dessous du sprite a blitter en alpha...
                  
                     DDCOLORKEY ddColorKey;
                     lpSpriteSurface->GetColorKey(DDCKEY_SRCBLT, &ddColorKey); 
                   
                     // si on clip la surface a dessiner...
                     ddColorKey.dwColorSpaceLowValue = TransColor;
                  
                     TransAlphaImprovedOldSpr(lpSpriteSurface,lpNewSurface,0,0,0,0, sourceRect, 210, ddColorKey.dwColorSpaceLowValue,16);
                     ToBlit->BltFast(destRect.left, destRect.top, lpNewSurface, &destRectTmp, DDBLTFAST_WAIT);
                  
                     lpNewSurface->Release();
                  }
               } 
               else 
               {                
                   ToBlit->Blt(&destRect, lpSpriteSurface, &sourceRect, DDBLT_WAIT | (fTransparent ? DDBLT_KEYSRC : NULL), NULL);
                   //ToBlit->BltFast(destRect.left, destRect.top, lpSpriteSurface, &sourceRect, DDBLTFAST_WAIT | (fTransparent ? DDBLTFAST_SRCCOLORKEY : NULL));
               }
         }
        }	
    }
}

void Sprite::LoadPalette(char *ID) 
{
   lpSpritePal = CV2PalManager::GetInstance()->GetPalIndex(ID,1);
}


//////////////////////////////////////////////////////////////////////////////////
// TransAlphaImproved - 
//////////////////////////////////////////////////////////////////////////////////
HRESULT Sprite::TransAlphaImprovedOldSpr(LPDIRECTDRAWSURFACE7 src, LPDIRECTDRAWSURFACE7 dest,LONG lDestX, LONG lDestY,LONG lDestW,LONG lDestH, RECT srcRect, WORD ALPHA, DWORD ColorKey, WORD BPP)
{
   int register i,j;
   int height,width;
   BYTE* lpSprite;
   BYTE* lpDest;
   WORD dPitch, SpritePitch;
   DWORD sColorKey;
   DWORD sTemp,dTemp;
   DWORD sr,sg,sb,dr,dg,db;
   WORD sbuf,dbuf;
   DWORD Result;
   BOOL oddWidth = FALSE;
   DDSURFACEDESC2 srcDDSD, destDDSD;
   DWORD REDC,GREENC, BLUEC;
   DWORD PLUS64;
   DWORD ALPHABY4;
   DWORD doubleColorKey;

   // Check the ALPHA value
   if (ALPHA < 0)
      ALPHA = 0;
   else if (ALPHA > 256)
      ALPHA = 256;
   
   // Set height and width of SPRITE
   height = srcRect.bottom - srcRect.top;
   width = srcRect.right - srcRect.left; 
   
   // Lock down both surfaces for read and write
   ZeroMemory(&srcDDSD, sizeof(srcDDSD));
   srcDDSD.dwSize = sizeof(srcDDSD);
	src->Lock(NULL, &srcDDSD, DDLOCK_WAIT, NULL);
    
	ZeroMemory(&destDDSD, sizeof(destDDSD));
   destDDSD.dwSize = sizeof(destDDSD);
   dest->Lock(NULL, &destDDSD, DDLOCK_WAIT, NULL);

      
   // Get the color key for sprite surface
   sColorKey = ColorKey;
   
   // Set the pitch for both surfaces
   SpritePitch = (WORD)srcDDSD.lPitch;
   dPitch      = (WORD)destDDSD.lPitch;
   
   // Initialize the pointers to the upper left hand corner of surface
   lpSprite = (BYTE*)srcDDSD.lpSurface;
   lpDest   = (BYTE*)destDDSD.lpSurface;

   if(!lpSprite || !lpDest)
   {
      src->Unlock(NULL);
      dest->Unlock(NULL);
      return FALSE;
   }
   
   // Do some work outside of the loop
   PLUS64         = 64 | (64 << 16);
   ALPHABY4      = (ALPHA / 4) | ((ALPHA / 4) << 16);
   doubleColorKey = ColorKey | (ColorKey << 16);
   
   switch(BPP)
   {
      case 16:
      
         // Initialize the pointers to the first pixel in the rectangle
         lpSprite += (srcRect.top * SpritePitch) + (srcRect.left * 2);
         lpDest   += (lDestY * dPitch) + (lDestX * 2);
      
         // Set the horizontal padding
         sbuf = (WORD)(SpritePitch - (2 * width));
         dbuf = (WORD)(dPitch - (2 * width));
      
         // Is the Sprite width odd or even?
         if (width % 2 == 1)
         {
            oddWidth = TRUE;
            width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
         }
         else
            width = width / 2;  //div by 2, processing 2 pixels at a time.
      
      
         i = height;
         do
         {
            if (oddWidth)
            {
               sTemp = *((WORD*)lpSprite);
            
               if (sTemp != ColorKey)
               {
                  dTemp = *((WORD*)lpDest);
                  sb = sTemp & 0x1f;
                  db = dTemp & 0x1f;
                  sg = (sTemp >> 5) & 0x3f;
                  dg = (dTemp >> 5) & 0x3f;
                  sr = (sTemp >> 11) & 0x1f;
                  dr = (dTemp >> 11) & 0x1f;
               
                  *((WORD*)lpDest) = (WORD)((ALPHA * (sb - db) >> 8) + db |
                     ((ALPHA * (sg - dg) >> 8) + dg) << 5 |
                     ((ALPHA * (sr - dr) >> 8) + dr) << 11);
               }
            
               lpDest   += 2;
               lpSprite += 2;
            }
            j = width;
            do
            {
               sTemp = *((DWORD*)lpSprite);
            
               if ( sTemp != doubleColorKey )
               {
                  dTemp = *((DWORD*)lpDest);
               
                  sb = sTemp & 0x001F001F;
                  db = dTemp & 0x001F001F;
                  sg = (sTemp >> 5)  & 0x003F003F;
                  dg = (dTemp >> 5)  & 0x003F003F;
                  sr = (sTemp >> 11) & 0x001F001F;
                  dr = (dTemp >> 11) & 0x001F001F;
               
                  BLUEC  = ((((ALPHA * ((sb + PLUS64) - db)) >> 8) + db) - ALPHABY4) & 0x001F001F;
                  GREENC = (((((ALPHA * ((sg + PLUS64) - dg)) >> 8) + dg) - ALPHABY4) & 0x003F003F) << 5;
                  REDC   = (((((ALPHA * ((sr + PLUS64) - dr)) >> 8) + dr) - ALPHABY4) & 0x001F001F) << 11;
               
                  Result = BLUEC | GREENC | REDC;
               
                  if ( (sTemp >> 16) == ColorKey )
                     Result = (Result & 0xFFFF) | (dTemp & 0xFFFF0000);
                  else if ( (sTemp & 0xFFFF) == ColorKey )
                     Result = (Result & 0xFFFF0000) | (dTemp & 0xFFFF);
               
                  *((DWORD*)lpDest) = Result;
               }
               lpDest    += 4;
               lpSprite  += 4;
            
            }while (--j > 0);
         
            lpDest   += dbuf;
            lpSprite += sbuf;
         
         }while (--i > 0);
      break;

	} // End _RGB Format switch statement
	src->Unlock(NULL);
	dest->Unlock(NULL);

	return DD_OK;
}
