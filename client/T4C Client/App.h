// ************************************************************************
// ***                                                                  ***
//      File Name: App.h
//      Project:   The 4th Coming
//      Creation:  25 Mars, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***

#ifndef APP_H
#define APP_H


#define WINDOWED 0

#include "V2Sprite.h"
#include "Lock.h"


// Const
const int APP_MOUSE_THREAD		  = 1;
const int APP_CD_THREAD			  = 2;
const int APP_DRAWING_THREAD	  = 3;
const int APP_MAINTENANCE_THREAD = 4;
const int APP_SOUND_THREAD		  = 5;
const int APP_VOLUME_THREAD		  = 6;

class AppManagement {
// Class that manage the exit of T4C.

	private:
		DWORD dwErrNb;			// Code d'erreur lors de sortie
		char  *lpszErrStr;	// String d'erreur lors de sortie
		bool  boFocus;			// Is the App on Focus ?

		bool SMT_Exit;			// Flag for the Sound Management Thread
		bool SCT_Exit;			// Flag for the Sound Control Thread
		bool DT_Exit;			// Flag for the Drawing Thread
		bool MT_Exit;			// Flag for the Maintenance Thread
		bool MS_Exit;			// Flag for the Mouse Thread
		bool CD_Exit;			// Flag for the CD Thread

        static UINT CALLBACK AsyncClose( LPVOID theApp );

	public:
        void LockDrawThread();
        void UnlockDrawThread();
        void LockMainThread();
        void UnlockMainThread();
        void LockPacketThread();
        void UnlockPacketThread();

        void SetLoading( bool loadValue );
        bool IsLoading();

		AppManagement(void);		  // Constructor
		~AppManagement(void);	  // Destructor

		void Close(char *,bool bSendExitToServer);						// Close App.
		void SetError(DWORD, char *);		// Set Exit Error.
		void ExitThread(DWORD);				// Set Exit Thread.
		void StartThread(DWORD);			// Set Starting Thread.
		bool isFocus(void);					// is the App on Focus ?
};

class AppVariable {
// Class that hold all App State Variable
   public:
      AppVariable(void);
      
      bool minimizeState;
      bool inGame;
      bool InvalideSaveGameI;
      bool InvalideSaveGameM;
      bool InvalideSaveGameC;
      bool bMemory;
      WORD wRPStatus;
      WORD wDisplayXPScroll;
      WORD wDisplayXPScrollT;
      WORD wDisplayORScroll;
      WORD wDisplayORScrollT;
      WORD wDisplayDisk;
      WORD wDisplayMap;
      WORD wDisplayMapZone;
      char strMapZoneName[128];
      CV2Sprite  vsfDisk;
      CV2Sprite  vsfNetwork;
      CV2Sprite  vsfMap;
      CV2Sprite  vsfMapZoneN;
      CV2Sprite  vsfXpScroll;
      CV2Sprite  vsfORScroll;
      WORD wProgressCnt;
      int  wProgressInc;
      CV2Sprite  vsfProgressB[2];
      CV2Sprite  vsfProgressT[2];
      CV2Sprite  vsfGame01[30];

      DWORD      dw1stAprilCnt;
      WORD       wBubbleAnimCnt01;
      WORD       wBubbleAnimCnt01R;
      WORD       wBubbleAnimCnt02;
      WORD       wBubbleAnimCnt02R;
      WORD       wBubbleAnimCnt03;
      WORD       wBubbleAnimCnt03R;
      CV2Sprite  vsfBubble[18];
      int        iFishXPos;
      int        iFishYPos;
      CV2Sprite  vsfFish;
      bool       bDisplayedMessage;
      char       strFishMessage[1024];
};


// Global Var
extern AppManagement g_App;
extern AppVariable   g_Var;
extern bool          g_boQuitApp;		// Si on est en train de sortir du jeux
extern bool          g_bDrawThreadPaused;		// Si on est en train de sortir du jeux
extern UINT          g_uiReloadForceTeleport;
extern UINT          g_uiReloadForce;


#endif // APP_H