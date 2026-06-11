// Include Files.
#include "pch.h"
#include "MacroHandler.h"
#include "Global.h"


UserGeneralConfig Custom;

static BOOL boMacroEnabled = TRUE;
extern BOOL DoNotMove;

//*******************************************************************************************//
MacroHandler::MacroHandler( void )
//*******************************************************************************************//
// Default Constructor.
// 
{
   //Saved
   Custom.g_pNewCCColor      = NULL;
   Custom.gstrMp3PlayList[0] = 0x00;
   Custom.gdwUSEMMX          = 0;
   Custom.gdwUSEVBOX         = 0;
   Custom.gDisConMusic       = 0;
   Custom.gWindowed          = TRUE;


   //Unsaved
   Custom.gTextColor       = CL_YELLOW;
   Custom.gWindowedSizing  = false;
   Custom.g_dwNbrCCColor   = 0;
   Custom.gTakeScreenShot  = 0;
   Custom.gUpdateFPSOption = FALSE;
   Custom.gWinterrize      = 0;
   Custom.gHIDEAllDecors   = 0;
   Custom.gDebugLightTime  = 0;
   Custom.gDebugNoLight    = 0;
   Custom.gDebugLightDay   = 0;
   Custom.gDebugLightNight = 0;
   Custom.gDebugGUIHidden  = 0;
   Custom.gTIME_R          = 31;
   Custom.gTIME_G          = 31;
   Custom.gTIME_B          = 31;
   Custom.gbEnableGameSnow = false;
   Custom.gbEnableGameFog  = false;  
   Custom.gbEnableGameRain = false;   
   Custom.gWebUpdateEnabled= true;     //WEBPATCH2 a TRUE HERE to ENABLE IT...
   Custom.gdwCurrentSongIndex = 0;  
    
   
   //Config
   Custom.gfirstTimeAddOn = false;
   Custom.gShowFPS        = false; 

    
   Custom.dwLastMp3PlayerState = 0;
}

//*******************************************************************************************//
MacroHandler::~MacroHandler( void )
//*******************************************************************************************//
// Destructor
// 
{
	HandlerDesc *hd;

	ToHead();
	while(QueryNext()) {
		hd = GetObject();
      RemoveObject();
      delete hd;
	}

   //Saved
   if(Custom.g_pNewCCColor)
      delete []Custom.g_pNewCCColor;
   Custom.g_pNewCCColor = NULL;
}


void MacroHandler::DeleteMacro( VKey vKey ){
	HandlerDesc *hd;

    Lock(0);
    ToHead();
    while( QueryNext() ){
        hd = GetObject();
        if (hd->vKey == vKey) {
            DeleteObject();
			SetQueryState(STOP_QUERY);
		}
    }
    Unlock(0);
}

void MacroHandler::AddNewMacro( VKey vKey, UINT (* CallBack)(LPVOID), LPVOID lpParam) {
    // Delete any previous key binded to the new macro call
    DeleteMacro( vKey );

    // Create a new handler desc for this macro.
    HandlerDesc *hd = new HandlerDesc;
    
    // Put the Key.
	hd->vKey = vKey;
	// Call back
	hd->CallBack = CallBack;
	// Param Pointer
	hd->lpParam = lpParam;

	Lock(0);
    AddToTail(hd);
    Unlock(0);
}

BOOL MacroHandler::MacroExists( VKey vKey ){
    BOOL boFound = FALSE;

    HandlerDesc *hd;

    Lock(0);
    ToHead();
    while( QueryNext() ){
        hd = GetObject();
        if (hd->vKey == vKey) {
            boFound = TRUE;
			SetQueryState(STOP_QUERY);
		}
    }
    Unlock(0);
    return boFound;
}

//*******************************************************************************************//
BOOL MacroHandler::CallMacro
//*******************************************************************************************//
// Set CallBack Function
// 
(
  VKey vKey
)
//*******************************************************************************************//
{
    if( !boMacroEnabled )
    {
        return FALSE;
    }

	HandlerDesc *hd;

    BOOL boFound = FALSE;

    Lock(0);

    if( GetNbObjects() == 0 ){
    }

    ToHead();
    while(QueryNext()) 
    {
       hd = GetObject();
       if (hd->vKey == vKey && !DoNotMove) 
       {

          hd->CallBack(hd->lpParam); 
          //Sleep(10);
          boFound = TRUE;
          break;
       }
    }
    Unlock(0);

    return boFound;
}

void MacroHandler::EnableMacroCall(){
    boMacroEnabled = TRUE;
}

void MacroHandler::DisableMacroCall(){
    boMacroEnabled = FALSE;
}