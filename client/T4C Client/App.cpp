// ************************************************************************
// ***                                                                  ***
//      File Name: App.cpp
//      Project:   The 4th Coming
//      Creation:  25 Mars, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***
#include "pch.h"
// Include     
#include "App.h"
#include "Wingdi.h"
#include "TFCPacket.h" // vector.
#include "COMM.h"
#include "IDirectXDraw.h" 

#include "GameMusic.h"
#include "V2Database.h"
#include "Disp.h"
#include "GUILocalString.h"   
#include "NewInterface/RootBoxUI.h"
#include "NewFont.h"
#include "Lock.h"

#include "Global.h"
#include "SaveGame.h"

// Extern Global.

extern Font *V3_BtnFontB;
extern Font *fSmallNormal_12;
extern Font *fSmallNormal_13;
extern Font *fSmallNormal_14;

extern Font *fT4CDef_18;
extern Font *fT4CDef_21;
extern Font *fT4CDef_22;
extern Font *fT4CDef_23B;
extern Font *fT4CDef_30B;
extern Font *fT4CDef_50B;
extern Font *fSystem_22;





extern CV2Sprite **lplpCursor;
extern CV2Sprite **lplpCursorA;
extern HANDLE InterPacketEvent;
extern char *MsgDay;
//extern char *MsgDayLast;
extern HANDLE hMaintenanceThread;
extern HANDLE hDrawThread;
extern HANDLE hMouseActionThread;

extern CSaveGame g_SaveGame;

extern bool IsNbrT4cGraterThanOne(BOOL bUpdate);

// Global Var.
AppManagement g_App;
AppVariable   g_Var;
bool          g_boQuitApp = false;
bool          g_bSendCloseToServer = true;
bool          g_bDrawThreadPaused = false;
UINT          g_uiReloadForceTeleport = 0;
UINT          g_uiReloadForce = 0;
bool          g_UiInit = false;
bool          g_pendingInventoryRefresh = false;
bool          g_boWaitForShutdown = true;

unsigned char *g_ListData  = NULL;
int            g_ListSize  = 0;

static CLock  drawLock;
static CLock  mainLock;
static CLock  mouseLock;
static CLock  packetLock;

static bool loading = false;

//*********************************************************************************
AppManagement::AppManagement( void )
/**********************************************************************************
 * AppManager Constructor
 */
{
   // Constructor
   SMT_Exit = true;
   SCT_Exit = true;
   DT_Exit  = true;
   MT_Exit  = true;
   MS_Exit  = true;
   CD_Exit  = true;
   
   boFocus   = true;
   AddFontResourceEx("Fonts\\t4cbeaulieuxV2.ttf",FR_PRIVATE  ,0);
}

//*********************************************************************************
AppManagement::~AppManagement( void )
/**********************************************************************************
 * App Manager Desctructor
 */
{

   
}


#pragma optimize( "", off ) 
//////////////////////////////////////////////////////////////////////////////////////////
UINT AppManagement::AsyncClose
//////////////////////////////////////////////////////////////////////////////////////////
//  Asynchronous closing function
// 
(
 LPVOID theApp // Unused
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    AppManagement *app = static_cast< AppManagement * >( theApp );

    // Are we in quitting mode ? No then...
    int i; // Loop value
  
  
    // Set the flag that we are currenlty closing the application.    
    //NM:Close
    try
    {
       Sleep(250);


       // Sending the Close packet (20) to the Server.
	    // NMNMNMNM Already Send to server and wait antiplug delay if avaiable
       // meme si c hacker et que le client se ferme immediatement il restera present
       // en jeux le temps du closes...
       if(g_bSendCloseToServer)
       {
          TFCPacket Send;
          Send << (short)RQ_ExitGame;
          
          if (COMM.Ctr) 
          {
             // Was socket created ?
             SEND_PACKET(Send);
             Sleep(50);
          }
       }

      g_GameMusic.Reset();
      if( g_UiInit ){
        // Notify all UI of a shutdown.
        RootBoxUI::GetInstance()->ClientShutdown();
      }

   
      if( siGetMainWndHook() != 0 ){
          UnhookWindowsHookEx( siGetMainWndHook() );
      }

   

	  // Allow time for the close packet to be sent.
	  Sleep( 1000 );
      // Close the COMM Center
      COMM.StopSendAlive();
      COMM.Close();

      // Delete all Fonts.
      FontManager::GetInstance()->Release();

  
      // Closing Thread Stuff.
      if ( 1/*app->boFocus*/ ) {
         // Are we in focus ?
         // Sound Manager Thread. Set the Event to make the loop exit.
         for (i = 0; i < 200; i++) {
            if (app->SMT_Exit)
               i = 200;
            Sleep(15);
         }
     
		 // Sound Control Thread.
         for (i = 0; i < 200; i++) {
            if (app->SCT_Exit)
               i = 200;
            Sleep(15);
         }
     
		 // Drawing Thread.
         for (i = 0; i < 200; i++) {
            if (app->DT_Exit)
               i = 200;
			
            Sleep(15);
         }
		if( WaitForSingleObject( hDrawThread, 0 ) == WAIT_TIMEOUT ){
            SuspendThread( hDrawThread );
		}

     
		 // Maintenance Thread.
         for (i = 0; i < 200; i++) {
            if (app->MT_Exit)
               i = 200;
			Sleep(15);
         }

		 if( WaitForSingleObject( hMaintenanceThread, 0 ) == WAIT_TIMEOUT ){
             Sleep( 5000 );
			 //SuspendThread( hMaintenanceThread );
		 }
     
		 // Mouse Thread.
         for (i = 0; i < 200; i++) {
            if (app->MS_Exit)
               i = 200;
			Sleep(15);
         }

		 if( WaitForSingleObject( hMouseActionThread, 0 ) == WAIT_TIMEOUT ){
			 SuspendThread( hMouseActionThread );
		 }

     
		 // CD Thread.
         for (i = 0; i < 200; i++) {
            if (app->CD_Exit)
               i = 200;
			Sleep(15);
         }
      } 

 

      try{
      // Music Section. We are currently Stopping and closing the CDAudio
      //CloseCDAudio();
      if (ug_T3SoundManager) {
         if (ug_T3SoundManager->hEvent)
            SetEvent(ug_T3SoundManager->hEvent);
         Sleep(50);
         if (ug_T3SoundManager)
            delete ug_T3SoundManager;
         ug_T3SoundManager = NULL;
         
      }
      }
      catch(...)
      {

      }
	
	  

      // Delete Mouse Cursor
      delete [] lplpCursor;
      delete [] lplpCursorA;
  
      // Restore Mouse Cursor
      int h = ShowCursor(true);
      while (!h) {
         h = ShowCursor(true);
      }

  
      // Delete InterpretPacket Event.
      if (InterPacketEvent) {
         CloseHandle(InterPacketEvent);
         InterPacketEvent = NULL;
      }
  

      	// New delete
      g_V2Database.Release();

      
       

      // Delete font handler and all fonts
      if (UniqueFontHandler)
         delete UniqueFontHandler;

     
      
      if (V3_BtnFontB)
         delete V3_BtnFontB;
      if (fSmallNormal_12)
         delete fSmallNormal_12;
      if (fSmallNormal_13)
         delete fSmallNormal_13;
      if (fSmallNormal_14)
         delete fSmallNormal_14;

      

      if (fT4CDef_21)
         delete fT4CDef_21;
      if (fT4CDef_22)
         delete fT4CDef_22;
      if (fT4CDef_23B)
         delete fT4CDef_23B;
      if (fT4CDef_30B)
         delete fT4CDef_30B;
      if (fT4CDef_50B)
         delete fT4CDef_50B;
      if (fSystem_22)
         delete fSystem_22;
      if (fT4CDef_18)
         delete fT4CDef_18;

      
      



      
	  // If there is a Message of the Day, delete it!
      if (MsgDay)
         delete MsgDay;
      //if (MsgDayLast)
      //   delete MsgDayLast;
    }catch( ... ){
    }

    try{
		//ShowWindow( siGethWnd(), SW_HIDE );        

        // Display the Custom Error Message
        switch (app->dwErrNb) {
		case 1: {
         MessageBox(NULL, app->lpszErrStr, g_LocalString[410], MB_OK | MB_TOPMOST );
              } break;
        }

    }catch(...){
    }


    // Delete all local String.
    g_LocalString   .Release();
    g_GUILocalString.Release();

    if(g_ListData)
       delete []g_ListData;
    g_ListData = NULL;


    
   

    // Close every unclose file handle.
    _fcloseall();
   

    // Closing DirectDraw Stuff.
    if (lpDXDirectDraw) 
    {
       
       // Was directdraw initialized ?
       if (lpDXDirectDraw->lpdd && lpDXDirectDraw->DXCompleteInit) 
       {
          // Was directdraw fully initialized ?
          if (app->boFocus) 
          {
             // Are we in focus ? If so fade the screen before closing.
             
             if (DXDClear() == DD_OK) 
             {
                // This prevent some error of Surface Lost (or other....)
                
                DXDFlip(); //exit game
                DXDClear();
             }
          }
       }
       
       
       // Release everyting.
       World.ReleaseSurface();
       lpDXDirectDraw->Release();
       delete lpDXDirectDraw;
    }

    g_boWaitForShutdown = false;	
	Sleep( 20000 );


  exit( -1236 ); 

    return 0;
}
#pragma optimize( "", on ) 


#pragma optimize( "", off ) 
//*********************************************************************************
void AppManagement::Close( char *szReason ,bool bSendExitToServer)
/**********************************************************************************
* Close the application.
*
* @REMARK This function should always be call in order to terminate the 
* application and make the proper clean up.
*/
{
   // Close Application       
   if (!g_boQuitApp) 
   {
      g_Var.inGame = false;

      g_SaveGame.SuspendSaveThread(TRUE);
      g_SaveGame.Save_SaveGame(); //On Quit, on Save avant de quitter...


      while(!g_bDrawThreadPaused)
      {
         Sleep(100);
      }
      //Kill le font si necessaire
      if(!IsNbrT4cGraterThanOne(TRUE))
      {
         RemoveFontResourceEx("Fonts\\t4cbeaulieuxV2.ttf",FR_PRIVATE  ,0);
      }

      


      g_boQuitApp = true;
      g_bSendCloseToServer = bSendExitToServer;
      unsigned int nil;
      HANDLE hCloseThread = (HANDLE)_beginthreadex( NULL, 0, AsyncClose, this, 0, &nil );

      if( hCloseThread == NULL )
      {
         exit( -1234 );
      }
      // Wait 30 seconds for the object to close.
      //if( WaitForSingleObject( hCloseThread, 30000 ) == WAIT_TIMEOUT )
      //   exit( -1235 );

   }
}
#pragma optimize( "", on ) 
//*********************************************************************************
void AppManagement::SetError
/**********************************************************************************
 * This function Set andError Number and an Error String to be displayed by the 
 * Close Method.
 */
(
 DWORD dwErr,    // Error Number
 char *lpszErr   // Error String
)
//*********************************************************************************
{
   if (!g_boQuitApp) {
		// Set Error # and Message.
		dwErrNb = dwErr;
		lpszErrStr = new char [strlen(lpszErr)+1];
		strcpy_s(lpszErrStr,strlen(lpszErr)+1, lpszErr);
   }
}

//*********************************************************************************
void AppManagement::ExitThread
/**********************************************************************************
 * This method notify the Application Managr than a Core Thread has terminated. 
 * It is not currently supported to add custom thread to this method.
 */
(
 DWORD dwThread  // Internal Thread ID (Not the Window Thread ID)
)
//*********************************************************************************
{
   // Set the Flag to Exit Thread
   switch (dwThread) {
   case APP_MOUSE_THREAD:			MS_Exit  = true;	break;
   case APP_CD_THREAD:				CD_Exit  = true;	break;
   case APP_DRAWING_THREAD:		DT_Exit  = true;	break;
   case APP_MAINTENANCE_THREAD:	MT_Exit  = true;	break;
   case APP_SOUND_THREAD:			SMT_Exit = true;	break;
   case APP_VOLUME_THREAD:			SCT_Exit = true;	break;
      
   default:					  
      SetError(1, g_LocalString[393]); 
      Close(g_LocalString[393],true); 
      break;
   }
}

//*********************************************************************************
void AppManagement::StartThread
/**********************************************************************************
 * This method notify the Application manager that a Core Thread has started. 
 * It is not currently supported to add custom thread to this method.
 */
(
 DWORD dwThread  // Internal Thread ID (Not the Window Thread ID)
)
//*********************************************************************************
{
   // Set the Flag to Start Thread
   switch (dwThread) {
   case APP_MOUSE_THREAD:			MS_Exit  = false;	break;
   case APP_CD_THREAD:				CD_Exit  = false;	break;
   case APP_DRAWING_THREAD:		DT_Exit  = false;	break;
   case APP_MAINTENANCE_THREAD:	MT_Exit  = false;	break;
   case APP_SOUND_THREAD:			SMT_Exit = false;	break;
   case APP_VOLUME_THREAD:			SCT_Exit = false;	break;
      
   default:					  
      SetError(1, g_LocalString[394]); 
      Close(g_LocalString[394],true); 
      break;
   }
}

//*********************************************************************************
bool AppManagement::isFocus( void )
/**********************************************************************************
 * This method return the application focus state (set by the SetFocus method). 
 *
 * @RETURN <b>true</b> if the application has the focus, <b>false</b> if the 
 * application hasn't the focus.
 */
{
   // is the App on Focus ?
   return boFocus;
}



//*********************************************************************************
AppVariable::AppVariable( void )
/**********************************************************************************
 * Constructor. Initialize global application variable to default state.
 */
{
   inGame           = false; 
   InvalideSaveGameI= false;
   InvalideSaveGameM= false;
   InvalideSaveGameC= false;
   bMemory          = false; 
   minimizeState    = false;

   wRPStatus         = 0;

   wDisplayXPScroll  = 0;
   wDisplayXPScrollT = 0;

   wDisplayORScroll  = 0;
   wDisplayORScrollT = 0;

   dw1stAprilCnt     = 0;
   wBubbleAnimCnt01  = 1;
   wBubbleAnimCnt02  = 1;
   wBubbleAnimCnt03  = 1;
   iFishXPos         = 0;
   iFishYPos         = 0;
   
   
  
}

void AppManagement::LockDrawThread(){     drawLock.Lock(); }
void AppManagement::UnlockDrawThread(){   drawLock.Unlock(); }
void AppManagement::LockMainThread(){     mainLock.Lock(); }
void AppManagement::UnlockMainThread(){   mainLock.Unlock(); }
void AppManagement::LockPacketThread(){   packetLock.Lock(); }
void AppManagement::UnlockPacketThread(){ packetLock.Unlock(); }

//////////////////////////////////////////////////////////////////////////////////////////
void AppManagement::SetLoading
//////////////////////////////////////////////////////////////////////////////////////////
// 
// 
(
 bool loadValue
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    if( loadValue ){
        
    }else{
        
    }
    loading = loadValue;
}

bool AppManagement::IsLoading(){ return loading; };