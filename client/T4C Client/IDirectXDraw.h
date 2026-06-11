// ************************************************************************
// ***                                                                  ***
//      File Name: IDirectXDraw.h
//      Project:   The Fourth Coming
//      Creation:  25 Mars, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***
//      Change History
//
//         Date            Ver.	  Author	     Purpose
//         ----            ----    ------      -------
//         03-25-1997      1.0     TH          Initial development
//
//      Description
//         Provide Simplified class for DirectDraw, DirectSurface and 
//         DirectPalette.
// ***                                                                  ***
//	************************************************************************
// *** Copyright (c) 1997-1998 Vircom. All rights reserved.             ***

#ifndef __DIRECTXDRAW
#define __DIRECTXDRAW

#include <ddraw.h>
#include "SystemInfo.h"
#include "Global.h"
                       
extern Global g_Global;   

#define DXDCreate(x, y)  lpDXDirectDraw->CreateDirectX(x, g_Global.GetScreenW(), g_Global.GetScreenH(), 16, y)
#define DXDRelease()     lpDXDirectDraw->Release()

extern bool primaryDrawBuffer;
																																																																																				
#define DXDGetDC lpDXDirectDraw->GetDC
#define DXDGetDCConnect lpDXDirectDraw->GetDC
#define DXDReleaseDC lpDXDirectDraw->ReleaseDC
#define DXDFlip lpDXDirectDraw->Flip
#define DXDBlt lpDXDirectDraw->Blt
#define DXDRestoreSurface lpDXDirectDraw->RestoreSurface
#define DXDRestoreSurfaceF lpDXDirectDraw->RestoreSurfaceF


inline bool IsPrimaryDrawBufferOn(){
    return primaryDrawBuffer;
}
																                  


// DirectDraw, DirectSurface and DirectPallette simplified class.
class IDirectXDraw {
	private:
	public:
	   LPDIRECTDRAWSURFACE7 lpFrontBuffer;	          // Pointer to the Primary Surface
	   LPDIRECTDRAWSURFACE7 lpBackBufferF;	          // Pointer to the Back Surface
	   LPDIRECTDRAWSURFACE7 lpBackBufferWindowed;	  // Pointer to the Back Surface

      int OldScreenX;
      int OldScreenY;
      int OldScreenBPP;
	  int NewScreenX;
	  int NewScreenY;
	  int NewScreenBPP;

		BOOL DXCompleteInit;

		LPDIRECTDRAWCLIPPER lpClipper;

														   
	public:
	   DDSURFACEDESC2       ddsd;				  // Surface Description
		LPDIRECTDRAW7        lpdd;				  // DirectDraw
	   LPDIRECTDRAWPALETTE lpddpal;		     // Complex Palette
		LPDIRECTDRAWPALETTE lpddfrontpal;     // Primary Palette
		BOOL					fEnabled;		// DirectDraw enabled

      WORD                wRMask;
      WORD                wGMask;
      WORD                wBMask;
      BYTE                bRUp;
      BYTE                bGUp;
      BYTE                bBUp;
      BYTE                bRDown;
      BYTE                bGDown;
      BYTE                bBDown;

		IDirectXDraw(void);				        // Class Constructor
	   ~IDirectXDraw(void);			           // Class Destructor

		HRESULT CreateDirectX(HWND, int ScreenX = g_Global.GetScreenW(), int ScreenY = g_Global.GetScreenH(), 
			            int ScreenBPP= 16, int forceVideoMode = 0);    // Class Initialisation 
		void Release(void);                   // Class Closing
		void RestoreDisplayModeDD(void);

        LPDIRECTDRAWSURFACE7 PicklockBackBuffer( LPRECT lpRect, LPDDSURFACEDESC2 sur, DWORD flags, HANDLE hEvent );
        LPDIRECTDRAWSURFACE7 GetBackBufferDirect();

        void GetDC( HDC *x, int a );
        void ReleaseDC( HDC x, int a );
        void Flip();
        void Blt( LPDIRECTDRAWSURFACE7 x );
        void RestoreSurface();
        void RestoreSurfaceF();

		HRESULT CreateSurfaceNM( LPDDSURFACEDESC2 dds2, LPDIRECTDRAWSURFACE7*pSurface, IUnknown *Unknow);    // Class Initialisation 
};

typedef class IDirectXDraw FAR *LPDIRECTXDRAW;

void DXDErrorHandle(HRESULT, char *);
void DXDSetView(void);
void DXDFadeEntry(LPPALETTEENTRY, LPPALETTEENTRY, int MaxRed = 100, int MaxGreen = 100, int MaxBlue = 100, int Spare = 6);
HRESULT DXDClear(void);

// DirectX Draw Global Unique Identifier (GUID)
extern LPDIRECTXDRAW        lpDXDirectDraw;

extern BYTE *m_pLutAlphaR;
extern BYTE *m_pLutAlphaG;
extern BYTE *m_pLutAlphaB;
extern BYTE *m_pLutAlphareR;
extern BYTE *m_pLutAlphareG;
extern BYTE *m_pLutAlphareB;

extern BYTE  *m_pAlphaI;
extern float *m_pfIntensity;



static LPDIRECTDRAWSURFACE7	lpBackBufferF      = NULL;
static LPDIRECTDRAWSURFACE7	lpDXPrimarySurface = NULL;


#endif