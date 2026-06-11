// ************************************************************************
// ***                                                                  ***
//      File Name: IDirectXDraw.Cpp
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
// *** Copyright (c) 1997-1998 Vircom.             All rights reserved. ***

#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include "IDirectXDraw.h"
#include "GUILocalString.h"
#include "App.h"
#include "Global.h"
#include "MacroHandler.h"


extern Global g_Global;
extern bool IsNbrT4cGraterThanOne(BOOL bUpdate);


#ifdef _DEBUG
int DebugDC = 0;
#endif

LPDIRECTDRAW7        lpdd;


//DirectX Draw Global Unique Identifier (GUID)
static LPDIRECTDRAWSURFACE7  lpDXComplexSurface = NULL;
static LPDIRECTDRAWSURFACE7  lpDXComplexSurfaceWindowed   = NULL;
static LPDIRECTDRAWSURFACE7  lpDXDummySurface = NULL;
LPDIRECTXDRAW                lpDXDirectDraw = NULL;

BYTE  *m_pLutAlphaR   = NULL;
BYTE  *m_pLutAlphaG   = NULL;
BYTE  *m_pLutAlphaB   = NULL;
BYTE  *m_pLutAlphareR = NULL;
BYTE  *m_pLutAlphareG = NULL;
BYTE  *m_pLutAlphareB = NULL;
BYTE  *m_pAlphaI      = NULL;
float *m_pfIntensity  = NULL;

bool primaryDrawBuffer = true;

IDirectXDraw::IDirectXDraw(void) {
/* Initialize the DirectX Environment.
   */                      
   lpddpal = 0;
   lpdd = 0;
   lpFrontBuffer = 0;
   lpBackBufferF = 0;
   lpBackBufferWindowed = 0;
   DXCompleteInit = FALSE;

   m_pLutAlphaR   = new BYTE[0xFFFF];
   m_pLutAlphaG   = new BYTE[0xFFFF];
   m_pLutAlphaB   = new BYTE[0xFFFF];
   m_pLutAlphareR = new BYTE[0xFFFF];
   m_pLutAlphareG = new BYTE[0xFFFF];
   m_pLutAlphareB = new BYTE[0xFFFF];

   m_pAlphaI      = new BYTE[0xFF];
   m_pfIntensity  = new float[0xFF];

}


IDirectXDraw::~IDirectXDraw(void) 
{
/* Close the DirectX Draw Environment.
   */                      
   Release();

   delete []m_pLutAlphaR;
   delete []m_pLutAlphaG;
   delete []m_pLutAlphaB;
   delete []m_pLutAlphareR;
   delete []m_pLutAlphareG;
   delete []m_pLutAlphareB;
   delete []m_pAlphaI;
   delete []m_pfIntensity;
}


LONG CALLBACK HookMessageCallback( int message, WPARAM wParam, LPARAM lParam );

HRESULT IDirectXDraw::CreateDirectX(HWND hwnd, int ScreenX, int ScreenY, int ScreenBPP, int forceVideoMode) 
{
   HRESULT hresult;

   //RestoreDisplayModeDD();
   //Release();
   

   // Initialize the DirectX Draw Environment.
   
   try 
   { 
      hresult = DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL);

   } catch (...) 
   {
      g_App.SetError(1, g_LocalString[154]);
      g_App.Close(g_LocalString[154],true);
   }

   // Unknow random initialization error, this may help the system to stabilize.
   //Sleep(500);
      
   DXDErrorHandle(hresult, "DirectDraw Init");
   if (hresult != DD_OK) {
      g_App.SetError(1, g_LocalString[396]);
      g_App.Close(g_LocalString[396],true);
   }
    

   SetWindowPos(hwnd,NULL,0,0,g_Global.GetDisplayW(),g_Global.GetDisplayH(),SWP_DRAWFRAME);
   ShowWindow(hwnd,SW_SHOW);
   RECT ClientRect; 
   GetClientRect(hwnd,&ClientRect);
   SetWindowPos(hwnd,NULL,0,0,2*g_Global.GetDisplayW()-ClientRect.right,2*g_Global.GetDisplayH()-ClientRect.bottom,SWP_NOMOVE);

    
   ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
   ddsd.dwSize            = sizeof(ddsd);
   lpdd->GetDisplayMode(&ddsd);
   OldScreenX   = ddsd.dwWidth;
   OldScreenY   = ddsd.dwHeight;
   OldScreenBPP = ddsd.ddpfPixelFormat.dwRGBBitCount;
   if (Custom.gWindowed)
   { 
      //WINDOWED16 
      
      if(Custom.gdwUSEVBOX)
      {
         if (OldScreenBPP != 16)
         {
            if ((hresult = lpdd->SetDisplayMode(ddsd.dwWidth, ddsd.dwHeight, 16,0,0)) != DD_OK)
               return hresult;
         }
      }
      if ((hresult = lpdd->SetCooperativeLevel(hwnd, DDSCL_NORMAL )) != DD_OK) 
        return hresult;// Can't set the Cooperative Level.

   }
   else 
   {
      if ((hresult = lpdd->SetCooperativeLevel(hwnd, DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT)) != DD_OK) 
         return hresult;// Can't set the Cooperative Level.

      if ((hresult = lpdd->SetDisplayMode(g_Global.GetDisplayW(), g_Global.GetDisplayH(), 16,0,0)) != DD_OK)
         return hresult;
   }   
   
   if (Custom.gWindowed)
   {
      
      memset( &ddsd, 0, sizeof(ddsd) );
      ddsd.dwSize            = sizeof(ddsd);
      ddsd.dwFlags           = DDSD_CAPS;
      ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE;
      
      if ((hresult = lpdd->CreateSurface(&ddsd, &lpFrontBuffer, NULL)) != DD_OK)
         return hresult; // Can't create the Surface.
      
      
      lpDXPrimarySurface = lpFrontBuffer; //affectation apres creation
      
      if (hresult = lpdd->CreateClipper( 0, &lpClipper, NULL )!= DD_OK)
      {
         lpFrontBuffer->Release();
         lpdd->Release();
         return hresult;
      }
      
      if (hresult = lpClipper->SetHWnd( 0, hwnd )!= DD_OK)
      {
         lpClipper-> Release();
         lpFrontBuffer->Release();
         lpdd->Release();
         return hresult;
      }
      
      if (hresult = lpDXPrimarySurface->SetClipper( lpClipper )!= DD_OK) //creation du clipper...
      {
         lpClipper-> Release();
         lpFrontBuffer->Release();
         lpdd->Release();
         return hresult;
      }

	  memset( &ddsd, 0, sizeof(ddsd) );
	  ddsd.dwSize = sizeof( ddsd );
	  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
	  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	  ddsd.dwWidth = g_Global.GetDisplayW();
	  ddsd.dwHeight = g_Global.GetDisplayH();

      if (hresult = lpdd->CreateSurface( &ddsd, &lpBackBufferF, NULL )!= DD_OK)
      {
         lpClipper-> Release();
         lpFrontBuffer->Release();
         lpdd->Release();
         return hresult;
      }
     
      lpDXComplexSurface = lpBackBufferF;

      
      //cree le buffer de travail 16 bits qui sera utiliser en windowed pour pas chnager le mode video...
      memset( &ddsd, 0, sizeof(ddsd) );
      ddsd.dwSize = sizeof( ddsd );
      ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT | DDSD_PIXELFORMAT;
      ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
      ddsd.dwWidth = g_Global.GetDisplayW();
      ddsd.dwHeight = g_Global.GetDisplayH();
      ddsd.ddpfPixelFormat.dwSize        = sizeof(DDPIXELFORMAT);
      ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
      ddsd.ddpfPixelFormat.dwFlags       = 64 ;
      ddsd.ddpfPixelFormat.dwRBitMask    = 63488;
      ddsd.ddpfPixelFormat.dwGBitMask    = 2016;
      ddsd.ddpfPixelFormat.dwBBitMask    = 31;


      if (hresult = lpdd->CreateSurface( &ddsd, &lpBackBufferWindowed, NULL )!= DD_OK)
      {
         lpClipper-> Release();
         lpFrontBuffer->Release();
         lpdd->Release();
         return hresult;
      }

      lpDXComplexSurfaceWindowed = lpBackBufferWindowed;
      

      
      wRMask = (WORD)ddsd.ddpfPixelFormat.dwRBitMask;
      wGMask = (WORD)ddsd.ddpfPixelFormat.dwGBitMask;
      wBMask = (WORD)ddsd.ddpfPixelFormat.dwBBitMask;



      switch (forceVideoMode) 
      {
         case 5:
   			wRMask = 5;
            wGMask = 6;
            wBMask = 5;
         break;
         case 6:
            wRMask = 5;
            wGMask = 6;
            wBMask = 5;
         break;
         default:
         break;
      }
	  
      int i;
      for (i = 0; i < 16; i++) 
      {
         if (wRMask & (1<<i)) 
         {
            bRUp = i;
            i = 16;
         }
      }
      for (i = 0; i < 16; i++) 
      {
         if (wGMask & (1<<i)) 
         {
            bGUp = i;
            i = 16;
         }
      }
      for (i = 0; i < 16; i++) 
      {
         if (wBMask & (1<<i)) 
         {
            bBUp = i;
            i = 16;
         }
      }
      for (i = 15; i >= 0; i--) 
      {
         if (wRMask & (1 << i)) 
         {
            bRDown = 8-(i-bRUp+1);
            i = -1;
         }
      }
      for (i = 15; i >= 0; i--) 
      {
         if (wGMask & (1 << i)) 
         {
            bGDown = 8-(i-bGUp+1);
            i = -1;
         }
      }
      for (i = 15; i >= 0; i--) 
      {
         if (wBMask & (1 << i)) 
         {
            bBDown = 8-(i-bBUp+1);
            i = -1;
         }
      }
	  


      DDBLTFX ddbltfx;
      ddbltfx.dwSize = sizeof(ddbltfx);
      ddbltfx.dwFillColor = 0;
      
      lpDXPrimarySurface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);//Clear dans initialisation
      lpDXComplexSurface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);//Clear dans initialisation
	   lpDXComplexSurfaceWindowed->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); //Ajouter bufferTmp si windowed
   }
   else 
   {
	
     
      // Create a PrimarySurface

      ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
      ddsd.dwSize            = sizeof(ddsd);
      ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
      ddsd.dwBackBufferCount = 1;
      ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX |DDSCAPS_VIDEOMEMORY;
      
      if ((hresult = lpdd->CreateSurface(&ddsd, &lpFrontBuffer, NULL)) != DD_OK) 
      {
		  ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		  ddsd.dwSize            = sizeof(ddsd);
		  ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		  ddsd.dwBackBufferCount = 1;
		  ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

		  if ((hresult = lpdd->CreateSurface(&ddsd, &lpFrontBuffer, NULL)) != DD_OK) 
		  {
			  return hresult;
		  }
      }

      DDSURFACEDESC2 Sprite;  
      memset( &Sprite, 0, sizeof( DDSURFACEDESC2 ) );
      Sprite.dwSize            = sizeof(Sprite);
      Sprite.dwFlags           = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
      Sprite.ddsCaps.dwCaps    = DDSCAPS_SYSTEMMEMORY;
      Sprite.dwHeight          = g_Global.GetDisplayH();
      Sprite.dwWidth           = g_Global.GetDisplayW();
      hresult = lpDXDirectDraw->lpdd->CreateSurface(&Sprite, &lpDXDummySurface, NULL);




	  //cree le buffer de travail 16 bits qui sera utiliser en windowed pour pas chnager le mode video...
	  memset( &ddsd, 0, sizeof(ddsd) );
	  ddsd.dwSize = sizeof( ddsd );
	  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT | DDSD_PIXELFORMAT;
	  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	  ddsd.dwWidth = g_Global.GetDisplayW();
	  ddsd.dwHeight = g_Global.GetDisplayH();
	  ddsd.ddpfPixelFormat.dwSize        = sizeof(DDPIXELFORMAT);
	  ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
	  ddsd.ddpfPixelFormat.dwFlags       = 64 ;
	  ddsd.ddpfPixelFormat.dwRBitMask    = 63488;
	  ddsd.ddpfPixelFormat.dwGBitMask    = 2016;
	  ddsd.ddpfPixelFormat.dwBBitMask    = 31;


	  if (hresult = lpdd->CreateSurface( &ddsd, &lpBackBufferWindowed, NULL )!= DD_OK)
	  {
		  lpClipper-> Release();
		  lpFrontBuffer->Release();
		  lpdd->Release();
		  return hresult;
	  }

	  lpDXComplexSurfaceWindowed = lpBackBufferWindowed;




      
	  lpdd->GetDisplayMode(&ddsd);
      wRMask = (WORD)ddsd.ddpfPixelFormat.dwRBitMask;
      wGMask = (WORD)ddsd.ddpfPixelFormat.dwGBitMask;
      wBMask = (WORD)ddsd.ddpfPixelFormat.dwBBitMask;

      switch (forceVideoMode) {
         case 5:
            wRMask = 31744;
            wGMask = 992;
            wBMask = 31;
         break;
         case 6:
			   wRMask = 5;
            wGMask = 6;
            wBMask = 5;
         break;
         default:
         break;
      };


      int i;
      for (i = 0; i < 16; i++) {
         if (wRMask & (1<<i)) {
            bRUp = i;
            i = 16;
         }
      }
      for (i = 0; i < 16; i++) {
         if (wGMask & (1<<i)) {
            bGUp = i;
            i = 16;
         }
      }
      for (i = 0; i < 16; i++) {
         if (wBMask & (1<<i)) {
            bBUp = i;
            i = 16;
         }
      }
      for (i = 15; i >= 0; i--) {
         if (wRMask & (1 << i)) {
            bRDown = 8-(i-bRUp+1);
            i = -1;
         }
      }
      for (i = 15; i >= 0; i--) {
         if (wGMask & (1 << i)) {
            bGDown = 8-(i-bGUp+1);
            i = -1;
         }
      }
      for (i = 15; i >= 0; i--) {
         if (wBMask & (1 << i)) {
            bBDown = 8-(i-bBUp+1);
            i = -1;
         }
      }


       
      
      // Create the ComplexSurface
      DDSCAPS2 ddscaps;
      ZeroMemory(&ddscaps, sizeof(ddscaps));
      ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
      if ((hresult = lpFrontBuffer->GetAttachedSurface(&ddscaps, &lpBackBufferF)) != DD_OK)// Can't create the Surface
		return hresult;
      

	  lpDXPrimarySurface = lpFrontBuffer;  //FULLSCREEN
	  lpDXComplexSurface = lpBackBufferF;  //FULLSCREEN
      
     
      
      // Clear both PrimarySuface and ComplexSurface.
      DDBLTFX ddbltfx;
      ddbltfx.dwSize = sizeof(ddbltfx);
      ddbltfx.dwFillColor = 0;
      
      lpDXPrimarySurface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx); //FULLSCREEN
      lpDXComplexSurface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx); //FULLSCREEN
   }
   DXCompleteInit = TRUE;


   return DD_OK;
}

HRESULT IDirectXDraw::CreateSurfaceNM( LPDDSURFACEDESC2 dds2, LPDIRECTDRAWSURFACE7 *pSurface, IUnknown *Unknow)
{
	if(Custom.gWindowed)
	{
		dds2->dwFlags =  dds2->dwFlags | DDSD_PIXELFORMAT;

		dds2->ddsCaps.dwCaps  = dds2->ddsCaps.dwCaps |DDSCAPS_OFFSCREENPLAIN ;

		dds2->ddpfPixelFormat.dwSize        = sizeof(DDPIXELFORMAT);
		dds2->ddpfPixelFormat.dwRGBBitCount = 16;
		dds2->ddpfPixelFormat.dwFlags       = 64 ;
		dds2->ddpfPixelFormat.dwRBitMask    = 63488;
		dds2->ddpfPixelFormat.dwGBitMask    = 2016;
		dds2->ddpfPixelFormat.dwBBitMask    = 31;
	}

	return lpdd->CreateSurface(dds2, pSurface, Unknow);
}
 
void IDirectXDraw::RestoreDisplayModeDD(void) 
{
	if (DXCompleteInit)
		lpdd->RestoreDisplayMode();
}

extern AppManagement g_App;

void IDirectXDraw::Release(void) {
/* Close the DirectX Draw Environment.
 */    

	if (lpBackBufferWindowed && g_App.isFocus() ) 
	{
		// Chk si le lpBackBuffer n'a pas deja ete deleter.
		if( lpBackBufferWindowed->IsLost() == DD_OK )
			lpBackBufferWindowed->Release();

		lpBackBufferWindowed = 0;
		lpDXComplexSurfaceWindowed = 0; //le release du directX
	}
	else
	{

	}

   if (lpBackBufferF && g_App.isFocus() ) 
   {
   // Chk si le lpBackBuffer n'a pas deja ete deleter.

      if( lpBackBufferF->IsLost() == DD_OK )
          lpBackBufferF->Release();
      lpBackBufferF = 0;
      lpDXComplexSurface = 0; //le release du directX
   }
   else
   {

   }

   if (lpFrontBuffer && g_App.isFocus() ) 
   { 
	   // Chk si le lpFrontBuffer n'a pas deja ete deleter.
	   if( lpFrontBuffer->IsLost() == DD_OK )
		   lpFrontBuffer->Release();

	   lpFrontBuffer = 0;
	   lpDXPrimarySurface = 0; //le release du directX
   }
   else
   {
   }

             
   if (lpdd) 
   {

   // Verifi si le lpdd n'a pas deja ete deleter.


      lpdd->Release();
		lpdd = 0;   

	  if(IsNbrT4cGraterThanOne(FALSE))
	  {
		  exit(666);
	  }
   }
}

void DXDErrorHandle(HRESULT hresult, char *Method) {
/* Handle Errors.
 */
	switch (hresult) {
	case DD_OK: 
	// Everything is fine.
		break;
	
	case DDERR_ALREADYINITIALIZED: 
	// The object has already been initialized.
		break;

	case DDERR_BLTFASTCANTCLIP:														
	// A DirectDrawClipper object is attached to a source surface that has 
	// passed into a call to the IDirectDrawSurface2::BltFast method. 
		break;

	case DDERR_CANNOTATTACHSURFACE:
	// A surface cannot be attached to another requested surface. 
		break;

	case DDERR_CANNOTDETACHSURFACE:
	// A surface cannot be detached from another requested surface.
		break;

	case DDERR_CANTCREATEDC:
	// Windows cannot create any more device contexts (DCs).
		break;

	case DDERR_CANTDUPLICATE:
	// Primary and 3D surfaces, or surfaces that are implicitly created, 
	// cannot be duplicated.
		break;

	case DDERR_CANTLOCKSURFACE:															
	// Access to this surface is refused because an attempt was made to lock
	// the primary surface without DCI support. 
		break;

	case DDERR_CANTPAGELOCK:																
	// An attempt to page lock a surface failed. Page lock will not work on a 
	// display-memory surface or an emulated primary surface.
		break;

	case DDERR_CANTPAGEUNLOCK:																
	// An attempt to page unlock a surface failed. Page unlock will not work
	// on a display-memory surface or an emulated primary surface.
		break;

	case DDERR_CLIPPERISUSINGHWND:														
	// An attempt was made to set a clip list for a DirectDrawClipper object
	// that is already monitoring a window handle.
		break;

	case DDERR_COLORKEYNOTSET:
	// No source color key is specified for this operation.
		break;

	case DDERR_CURRENTLYNOTAVAIL:
	// No support is currently available. 
		break;

	case DDERR_DCALREADYCREATED:														
	// A device context (DC) has already been returned for this surface. Only
	// one DC can be retrieved for each surface. 
		break;

	case DDERR_DIRECTDRAWALREADYCREATED:												
	// A DirectDraw object representing this driver has already been created
	// for this process. 
		break;

	case DDERR_EXCEPTION:																	
	// An exception was encountered while performing the requested operation.
		break;

	case DDERR_EXCLUSIVEMODEALREADYSET:													
	// An attempt was made to set the cooperative level when it was already 
	// set to exclusive. 
		break;

	case DDERR_GENERIC:
	// There is an undefined error condition. 
		break;

	case DDERR_HEIGHTALIGN:																	
	// The height of the provided rectangle is not a multiple of the required
	// alignment. 
		break;

	case DDERR_HWNDALREADYSET:																
	// The DirectDraw cooperative level window handle has already been set. 
	// It cannot be reset while the process has surfaces or palettes created. 
		break;

	case DDERR_HWNDSUBCLASSED:																
	// DirectDraw is prevented from restoring state because the DirectDraw 
	// cooperative level window handle has been subclassed. 
		break;

	case DDERR_IMPLICITLYCREATED:														
	// The surface cannot be restored because it is an implicitly created 
	// surface.
		break;

	case DDERR_INCOMPATIBLEPRIMARY:														
	// The primary surface creation request does not match with the existing 
	// primary surface. 
		break;

	case DDERR_INVALIDCAPS:
	// One or more of the capability bits passed to the callback function are
	// incorrect. 
		break;

	case DDERR_INVALIDCLIPLIST:
	// DirectDraw does not support the provided clip list.
		break;

	case DDERR_INVALIDDIRECTDRAWGUID:													
	// The globally unique identifier (GUID) passed to the DirectDrawCreate 
	// function is not a valid DirectDraw driver identifier.
		break;

	case DDERR_INVALIDMODE:
	// DirectDraw does not support the requested mode. 
		break;

	case DDERR_INVALIDOBJECT:
	// DirectDraw received a pointer that was an invalid DirectDraw object.
		break;

	case DDERR_INVALIDPARAMS:
	// One or more of the parameters passed to the method are incorrect. 
		break;

	case DDERR_INVALIDPIXELFORMAT:
	// The pixel format was invalid as specified. 
		break;

	case DDERR_INVALIDPOSITION:
	// The position of the overlay on the destination is no longer legal. 
		break;
	
	case DDERR_INVALIDRECT:
	// The provided rectangle was invalid. 
	break;


	case DDERR_INVALIDSURFACETYPE:
	// The requested operation could not be performed because the surface was of the wrong type. 
	break;

	case DDERR_LOCKEDSURFACES:
	// One or more surfaces are locked, causing the failure of the requested operation. 
	break;

	case DDERR_NO3D:
	// No 3D hardware or emulation is present. 
	break;

	case DDERR_NOALPHAHW:
	// No alpha acceleration hardware is present or available, causing the failure of the requested operation. 
	break;
	

	case DDERR_NOBLTHW:
	// No blitter hardware is present. 
	break;

	case DDERR_NOCLIPLIST:
	// No clip list is available. 
	break;

	case DDERR_NOCLIPPERATTACHED:
	// No DirectDrawClipper object is attached to the surface object. 
	break;

	case DDERR_NOCOLORCONVHW:
	// The operation cannot be carried out because no color-conversion hardware is present or available. 
	break;

	case DDERR_NOCOLORKEY:
	// The surface does not currently have a color key. 
	break;

	case DDERR_NOCOLORKEYHW:
	// The operation cannot be carried out because there is no hardware support for the destination color key. 
	break;

	case DDERR_NOCOOPERATIVELEVELSET:
	// A create function is called without the IDirectDraw2::SetCooperativeLevel method being called. 
		break;

	case DDERR_NODC:
	// No DC has ever been created for this surface. 
	break;

	case DDERR_NODDROPSHW:
	// No DirectDraw raster operation (ROP) hardware is available. 
	break;

	case DDERR_NODIRECTDRAWHW:
	// Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware. 
	break;

	case DDERR_NODIRECTDRAWSUPPORT:
	// DirectDraw support is not possible with the current display driver. 
	break;

	case DDERR_NOEMULATION:
	// Software emulation is not available. 
	break;

	case DDERR_NOEXCLUSIVEMODE:
	// The operation requires the application to have exclusive mode, but the application does not have exclusive mode. 
	break;

	case DDERR_NOFLIPHW:
	// Flipping visible surfaces is not supported. 
	break;

	case DDERR_NOGDI:
	// No GDI is present. 
	break;

	case DDERR_NOHWND:
	// Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle. 
	break;

	case DDERR_NOMIPMAPHW:
	// The operation cannot be carried out because no mipmap texture mapping hardware is present or available. 
	break;

	case DDERR_NOMIRRORHW:
	// The operation cannot be carried out because no mirroring hardware is present or available. 
	break;

	case DDERR_NOOVERLAYDEST:
	// The IDirectDrawSurface2::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface2::UpdateOverlay method has not been called on to establish a destination. 
	break;

	case DDERR_NOOVERLAYHW:
	// The operation cannot be carried out because no overlay hardware is present or available. 
	break;

	case DDERR_NOPALETTEATTACHED:
	// No palette object is attached to this surface. 
	break;

	case DDERR_NOPALETTEHW:
	// There is no hardware support for 16- or 256-color palettes. 
	break;

	case DDERR_NORASTEROPHW:
	// The operation cannot be carried out because no appropriate raster operation hardware is present or available. 
	break;

	case DDERR_NOROTATIONHW:
	// The operation cannot be carried out because no rotation hardware is present or available. 
	break;

	case DDERR_NOSTRETCHHW:
	// The operation cannot be carried out because there is no hardware support for stretching. 
	break;

	case DDERR_NOT4BITCOLOR:
	// The DirectDrawSurface object is not using a 4-bit color palette and the requested operation requires a 4-bit color palette. 
	break;

	case DDERR_NOT4BITCOLORINDEX:
	// The DirectDrawSurface object is not using a 4-bit color index palette and the requested operation requires a 4-bit color index palette. 
	break;

	case DDERR_NOT8BITCOLOR:
	// The DirectDrawSurface object is not using an 8-bit color palette and the requested operation requires an 8-bit color palette. 
	break;

	case DDERR_NOTAOVERLAYSURFACE:
	// An overlay component is called for a non-overlay surface. 
	break;

	case DDERR_NOTEXTUREHW:
	// The operation cannot be carried out because no texture-mapping hardware is present or available. 
	break;

	case DDERR_NOTFLIPPABLE:
	// An attempt has been made to flip a surface that cannot be flipped. 
	break;

	case DDERR_NOTFOUND:
	// The requested item was not found. 
	break;

	case DDERR_NOTINITIALIZED:
	// An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized. 
	break;

	case DDERR_NOTLOCKED:
	// An attempt is made to unlock a surface that was not locked. 
	break;

	case DDERR_NOTPAGELOCKED:
	// An attempt is made to page unlock a surface with no outstanding page locks. 
	break;

	case DDERR_NOTPALETTIZED:
	// The surface being used is not a palette-based surface. 
	break;

	case DDERR_NOVSYNCHW:
	// The operation cannot be carried out because there is no hardware support for vertical blank synchronized operations. 
	break;

	case DDERR_NOZBUFFERHW:
	// The operation to create a z-buffer in display memory or to perform a blit using a z-buffer cannot be carried out because there is no hardware support for z-buffers. 
	break;

	case DDERR_NOZOVERLAYHW:
	// The overlay surfaces cannot be z-layered based on the z-order because the hardware does not support z-ordering of overlays. 
	break;

	case DDERR_OUTOFCAPS:
	// The hardware needed for the requested operation has already been allocated. 
	break;

	case DDERR_OUTOFMEMORY:
	// DirectDraw does not have enough memory to perform the operation. 
	break;

	case DDERR_OUTOFVIDEOMEMORY:
	// DirectDraw does not have enough display memory to perform the operation. 
	break;

   case DDERR_OVERLAYCANTCLIP:
   // The hardware does not support clipped overlays. 
	break;

   case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
   // An attempt was made to have more than one color key active on an overlay. 
	break;

   case DDERR_OVERLAYNOTVISIBLE :
   // The IDirectDrawSurface2::GetOverlayPosition method is called on a hidden overlay. 
	break;

   case DDERR_PALETTEBUSY :
   // Access to this palette is refused because the palette is locked by another thread. 
	break;

   case DDERR_PRIMARYSURFACEALREADYEXISTS:
	// This process has already created a primary surface. 
	break;

   case DDERR_REGIONTOOSMALL :
   // The region passed to the IDirectDrawClipper::GetClipList method is too small. 
	break;

   case DDERR_SURFACEALREADYATTACHED :
   // An attempt was made to attach a surface to another surface to which it is already attached. 
	break;

   case DDERR_SURFACEALREADYDEPENDENT :
   // An attempt was made to make a surface a dependency of another surface to which it is already dependent. 
	break;

   case DDERR_SURFACEBUSY :
   // Access to the surface is refused because the surface is locked by another thread. 
	break;

   case DDERR_SURFACEISOBSCURED :
   // Access to the surface is refused because the surface is obscured. 
	break;

   case DDERR_SURFACELOST :
   // Access to the surface is refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have the IDirectDrawSurface2::Restore method called on it. 
	break;

   case DDERR_SURFACENOTATTACHED :
   // The requested surface is not attached. 
	break;

   case DDERR_TOOBIGHEIGHT :
   // The height requested by DirectDraw is too large. 
	break;

   case DDERR_TOOBIGSIZE :
   // The size requested by DirectDraw is too large. However, the individual height and width are OK. 
	break;

   case DDERR_TOOBIGWIDTH :
   // The width requested by DirectDraw is too large. 
	break;

	case DDERR_UNSUPPORTED:
   // The operation is not supported. 
	break;

   case DDERR_UNSUPPORTEDFORMAT :
   // The FourCC format requested is not supported by DirectDraw. 
	break;

   case DDERR_UNSUPPORTEDMASK :
   // The bitmask in the pixel format requested is not supported by DirectDraw. 
	break;

   case DDERR_UNSUPPORTEDMODE :
   // The display is currently in an unsupported mode. 
	break;

   case DDERR_VERTICALBLANKINPROGRESS :
   // A vertical blank is in progress. 
	break;

   case DDERR_WASSTILLDRAWING :
   // The previous blit operation that is transferring information to or from this surface is incomplete. 
	break;

   case DDERR_WRONGMODE :
   // This surface cannot be restored because it was created in a different mode. 
	break;

   case DDERR_XALIGN :
   // The provided rectangle was not horizontally aligned on a required boundary.
	break;
	
	default:
	// Unknown error
		break;
	}

   if (hresult != DD_OK) 
   {

	}
}

/*void DXDFadeEntry(LPPALETTEENTRY Dest, LPPALETTEENTRY Source, int MaxRed, int MaxGreen, int MaxBlue, int Spare) {
   for (int i=0; i < DEFAULT_COLORS; i++) {
		Dest[i].peRed   = Source[i].peRed   * MaxRed / 100;
		Dest[i].peGreen = Source[i].peGreen * MaxGreen / 100;
		Dest[i].peBlue  = Source[i].peBlue  * MaxBlue / 100;
	    Dest[i].peFlags = PC_NOCOLLAPSE;
	}

   for (i = DEFAULT_COLORS-Spare; i < DEFAULT_COLORS; i++) {
		Dest[i].peRed   = Source[i].peRed;
		Dest[i].peGreen = Source[i].peGreen;
		Dest[i].peBlue  = Source[i].peBlue;
	    Dest[i].peFlags = PC_NOCOLLAPSE;
   }
}*/

void DXDSetView(void) 
{
	   DXDErrorHandle(lpDXDirectDraw->lpdd->SetCooperativeLevel(siGethWnd(), DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN), "SetCooperative"); 
	   DXDErrorHandle(lpDXDirectDraw->lpdd->RestoreDisplayMode(), "Restore View"); 
	   DXDErrorHandle(lpDXDirectDraw->lpdd->SetDisplayMode(g_Global.GetDisplayW(), g_Global.GetDisplayH(), 16,0,0), "Set View"); 
}


HRESULT DXDClear(void) 
{
	DDBLTFX ddbltfx;
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.dwFillColor = 0;
      
	if(Custom.gWindowed)
		lpDXComplexSurfaceWindowed->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); //Ajouter bufferTmp si windowed
   return lpDXComplexSurface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); //Ajouter bufferTmp si windowed
}

//////////////////////////////////////////////////////////////////////////////////////////
// Gets a lock on the back buffer. If the surface as lost, the fonction
// will try to restore it only if the application currently has the focus. 
// 
// Return: LPDIRECTDRAWSURFACE7, A valid surface or NULL if a lock couldn't be aquired.
//////////////////////////////////////////////////////////////////////////////////////////
LPDIRECTDRAWSURFACE7 IDirectXDraw::PicklockBackBuffer(LPRECT lpRect,LPDDSURFACEDESC2 sur,DWORD flags,HANDLE hEvent)

{
	//CV:32 test CHANGERT LE CODE PAR CELUI CI...

	if(!Custom.gWindowed)
	{
		const int MaxTries = 5;
		int tries = 0;

		do 
		{
			while( lpBackBufferF->IsLost() != DD_OK )
			{
				lpFrontBuffer->Restore();
				Sleep(5) ;
				if( tries++ >= 5 ){
					return NULL;
				}          
			}

			HRESULT hr = lpBackBufferF->Lock(
				lpRect, sur, flags, hEvent
				);

			if( hr == DDERR_SURFACELOST ){
				Sleep(5);
				if( !g_App.isFocus() ){
					return NULL;
			 }
				lpBackBufferF->Restore(); 
			}else{
				break;
			}
			if( tries++ > MaxTries ){
				return NULL;
			}
		} while (1);
		return lpBackBufferF;
	}
	else
	{


		const int MaxTries = 5;
		int tries = 0;

		do 
		{
			while( lpBackBufferWindowed->IsLost() != DD_OK )
			{
				lpFrontBuffer->Restore();
				Sleep(5) ;
				if( tries++ >= 5 )
				{
					return NULL;
				}          
			}

			HRESULT hr = lpBackBufferWindowed->Lock(lpRect, sur, flags, hEvent);

			if( hr == DDERR_SURFACELOST )
			{
				Sleep(5);
				if( !g_App.isFocus() )
				{
					return NULL;
				}
				lpBackBufferWindowed->Restore(); 
			}
			else
			{
				break;
			}
			if( tries++ > MaxTries )
			{
				return NULL;
			}
		} while (1);
		return lpBackBufferWindowed;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
LPDIRECTDRAWSURFACE7 IDirectXDraw::GetBackBufferDirect( void )
//////////////////////////////////////////////////////////////////////////////////////////
// Returns the backbuffer
//////////////////////////////////////////////////////////////////////////////////////////
{
   
	if (!Custom.gWindowed)
   {
		if( !g_App.isFocus() )
		{
           return lpDXDummySurface;
		}
   }
   

   if (Custom.gWindowed)
	  return lpBackBufferWindowed;
   else
	  return lpBackBufferF;

}


// Accessor functions
void IDirectXDraw::GetDC( HDC *x, int a )
{
    if( (!g_App.isFocus() || lpDXComplexSurface->IsLost())&& !Custom.gWindowed) //Fullscreen
    {
        lpDXDummySurface->GetDC( x );
    }
    else
    {
		if(Custom.gWindowed)
			lpDXComplexSurfaceWindowed->GetDC(x); //remplacer par le buffer 16 Tmp
		else
			lpDXComplexSurface->GetDC(x); //remplacer par le buffer 16 Tmp
    }
}
void IDirectXDraw::ReleaseDC( HDC x, int a ){
	if( (!g_App.isFocus() || lpDXComplexSurface->IsLost())&& !Custom.gWindowed) //Fullscreen
   {
		lpDXDummySurface->ReleaseDC( x );
   }
   else
   {
	   if(Custom.gWindowed)
	       lpDXComplexSurfaceWindowed->ReleaseDC(x); //remplacer par le buffer 16 Tmp
	   else
		   lpDXComplexSurface->ReleaseDC(x); //remplacer par le buffer 16 Tmp

   }
}


void IDirectXDraw::Flip()
{
   if (Custom.gWindowed)
   {
      RECT rcRectDest;
      POINT p;
      p.x = 0; p.y = 0;
      ClientToScreen(siGethWnd(), &p);
      GetClientRect(siGethWnd(), &rcRectDest);
      OffsetRect(&rcRectDest, p.x, p.y);

      HRESULT hr1 = lpDXPrimarySurface->SetClipper( lpClipper ); //le clipper avant le draw
      HRESULT hr2 = lpDXPrimarySurface->Blt(&rcRectDest, lpDXComplexSurfaceWindowed, NULL, DDBLT_WAIT ,NULL); //a changer ici pour dessiner dans le 32 bits
      if(hr1 != S_OK)
         lpDXPrimarySurface->Restore();
      if(hr2 != S_OK)
      {
         lpDXComplexSurfaceWindowed->Restore();
         lpDXPrimarySurface->Restore();
      }



      int toto = 1;
   }
   else if( g_App.isFocus() && !Custom.gWindowed )
   {
      primaryDrawBuffer = !primaryDrawBuffer; 
      lpDXPrimarySurface->Flip(NULL, DDFLIP_WAIT);  //FULLSCREEN ONLY
   }
   else
   {
      Sleep(0);
   }
}
void IDirectXDraw::Blt( LPDIRECTDRAWSURFACE7 x )
{
    if( !g_App.isFocus() && !Custom.gWindowed)
	{
        Sleep(0);
        return;
    }


	if(!Custom.gWindowed)
	{
		if( lpDXComplexSurface->IsLost() != DD_OK )                //remplacer par le buffer 16 Tmp
			lpDXComplexSurface->Restore();                         //remplacer par le buffer 16 Tmp
		lpDXComplexSurface->Blt(NULL, x, NULL, DDBLT_WAIT, NULL);  //remplacer par le buffer 16 Tmp
	}
	else
	{
		if( lpDXComplexSurfaceWindowed->IsLost() != DD_OK )                //remplacer par le buffer 16 Tmp
			lpDXComplexSurfaceWindowed->Restore();                         //remplacer par le buffer 16 Tmp
		lpDXComplexSurfaceWindowed->Blt(NULL, x, NULL, DDBLT_WAIT, NULL);  //remplacer par le buffer 16 Tmp
	}

}



void IDirectXDraw::RestoreSurfaceF(){
    DXDErrorHandle(lpDXPrimarySurface->Restore(), "Primary Get Focus"); //FullScreen Only
}
void IDirectXDraw::RestoreSurface(){
    DXDErrorHandle(lpDXComplexSurface->Restore(), "Complex Get Focus"); //FullScreen Only
}

