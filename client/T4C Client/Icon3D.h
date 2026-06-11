// ************************************************************************
// ***                                                                  ***
//      File Name: Sprite3D.h
//      Project:   TFC Client
//      Creation:  8 July, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***
//      Change History
//
//         Date            Ver.	  Author	     Purpose
//         ----            ----    ------      -------
//         07-08-1997      1.0     TH          Initial development
//
//      Description
//         Provide fonction to load a set of Sprite to form a 3D Sprite
// ***                                                                  ***
//	************************************************************************
// *** Copyright (c) 1997-1998 Vircom. All rights reserved.             ***

#ifndef __Vircom_Sprite3D_105113070897
#define __Vircom_Sprite3D_105113070897


#define FRAMING 1

struct INFO {
   int Width;
   int Height;
   int CX;
   int CY; 
	BOOL DrawOverlap;
};

#include "V2Sprite.h"
#include "BasicSprite.h"

#define ST_STANDING  1
#define ST_MOVING  2
#define ST_ATTACK  3
#define ST_ATTACK_STANDING 4

class Sprite3D : public BasicSprite {
	public:
      BOOL bAlreadyLoad;
		int bColorG2R[10];
      DWORD LastAccess;
      
      Sprite3D();
		virtual ~Sprite3D();

		CV2Sprite *View000, *View045, *View090, *View135, 
				 *View180;
		CV2Sprite *Att000, *Att045, *Att090, *Att135, 
				 *Att180;
		CV2Sprite *Rng000, *Rng045, *Rng090, *Rng135, 
				 *Rng180;
		CV2Sprite *StAtt000, *StAtt045, *StAtt090, *StAtt135, 
				 *StAtt180;
		CV2Sprite *StMov000, *StMov045, *StMov090, *StMov135, 
				 *StMov180;
		CV2Sprite *Corpse;

      
      void LoadSprite3D(int, int, int, int, int, char *, int, int, int, int = 1);
	   void LoadSprite3D2(int, int, int, int, int, char *, int, int, int, int = 1);
      void LoadSprite3D(int, int, int, int, int, int, char *, int, int, int, int = 1);
	   void LoadSprite3D(int, int, int, char *, int, int, int = 1);
      void LoadSprite3D(int, char *, int = 1);
      void LoadSpriteC(int, int, int, char *, int = 1);
		void DrawSprite3D(int Direction, int Frame, int X, int Y, BYTE Stand, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *,Sprite3D *pMask = NULL);
		void DrawSprite3DA(int Direction, int Frame, int X, int Y, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *,Sprite3D *pMask = NULL);
		void DrawSprite3DR(int Direction, int Frame, int X, int Y, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *,Sprite3D *pMask = NULL);
		void DrawCorpse(int, int X, int Y, RECT *Clip, RECT *OutBound, RECT *Wrap, LPDIRECTDRAWSURFACE7 Surface, signed char Format, int AbsX, int AbsY, TFCObject *,Sprite3D *pMask = NULL);
		void Release(void);
      BOOL Release(DWORD);
      int  GetTextCorrection(void);
      INFO *GetCurrentVSF(int Direction, int Frame, BYTE Stand);
};

#endif