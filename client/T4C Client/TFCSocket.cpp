// **************************************************************************
// ***                                                                    ***
//      File Name: TFCSocket.Cpp
//      Project:   The Fourth Coming
//      Creation:  May 30th, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                    ***
// **************************************************************************
// ***                                                                    ***
//      Change History
//
//         Date            Ver.	  Author	     Purpose
//         ----            ----    ------      -------
//         06-27-1997      1.0     TH          Initial development.
//
//		  Fonctions:
//			  Provide Socket Receive Function
//
// ***                                                                    ***
// **************************************************************************
// *** Copyright (c) 1996-2003 Vircom inc.         All rights reserved.   ***
#include "pch.h"
#pragma warning( disable : 4291 )

#define uMinus(a, b) (b > a ? (0) : (a - b))

#pragma warning (disable:4786)

#include <windows.h>
#include "App.H"
#include "Bitmap.h"
#include "cs.h"
#include "DirectXInput.h"
#include "Disp.h"
#include <eh.h>
#include "FormatText.h"
#include "MacroHandler.h"
#include "Mouse.h"
#include "MouseAction.h"
#include "NewFont.h"
#include "NTime.h"
#include "ObjectListing.h"
#include "old.h"
#include "Packet.h"
#include "pf.h"
#include <process.h>
#include "Random.h"
#include <stdio.h>
#include "TFCFlag.h"
#include "TFCLandDef.h"
#include "TFCPacket.h"
#include "TFCPlayer.h"
#include "TFCSocket.h"
#include "TileSet.h"
#include "VisualObjectList.h"
#include "GUILocalString.h"
#include "NewInterface/RootBoxUI.h"
#include "NewInterface/V3_ChatLogDlg.h"
#include "NewInterface/V3_PacmanDlg.h"
#include "NewInterface/V3_TradeDlg.h"
#include "NewInterface/V3_MainBarDlg.h"
#include "NewInterface/V3_ChatDlg.h"
#include "CombatCursor.h"
#include "SysMsg.h"
#include "SaveGame.h"
#include "NMVideoCapture.h"
#include "VideoCapture.h"
#include "version.h"
#include "Resource.h"
#include "RegKeyHandler.h"
#include "version.h"

extern CSaveGame g_SaveGame;
extern NMVideoCapture g_NMVideoCapture;

extern unsigned char *g_ListData;
extern int            g_ListSize;
extern char           ServerIP[100];

extern int g_DONE;
extern int g_MOVX;
extern int g_MOVY;


#define MOUSE_CLICK(a, b, c, d) (x >= a && x <= a+c && y >= b && y <= b+d && Chose == 1)


extern HWND GlobalHwnd;

DWORD g_dwTextOnFullScreenCnt = 0;
void ProcessThreadDraw(bool bIncreaseObj);
void ProcessEventDraw();

unsigned char Connect_key[]={ 0xE1 ,0xC4 ,0x1B ,0xC0 ,0xC5 ,0x0D ,0x58 ,0xB7, 0x01 ,0xCE ,0x33 ,0xD3 ,0xA9 ,0x3F ,0xBA ,0x99,
						            0xC5 ,0x0D ,0x58 ,0xB7 ,0x01 ,0xCE ,0x33 ,0xD3, 0x1B ,0xC0 ,0xC5 ,0x0D ,0x58 ,0xB7 ,0x01 ,0xCE,
							         0x1B ,0xC0 ,0xC5 ,0x0D ,0x58 ,0xB7 ,0x01 ,0xCE, 0x33 ,0x1B ,0xC0 ,0xC5 ,0x0D ,0x58 ,0xB7 ,0x01,
							         0x0D ,0x58 ,0xB7 ,0x01 ,0xCE ,0xB7 ,0x01 ,0xCE ,0x33 ,0xD3, 0x1B ,0xCE, 0x33 ,0x1B ,0xC0 ,0x01   };

BOOL ReAsk = FALSE;

BOOL DBL = FALSE;

DWORD ThreadATimeStamp = 0;
DWORD ThreadBTimeStamp = 0;

DWORD LastCall;
BOOL CreateFlag = FALSE;

extern BOOL QuitFirstLoop;


DWORD TOTALOUT = 0;
DWORD OUTREALPEEK = 0;
DWORD OUTREALLOW = -1;
DWORD OUTPEEK = 0;
DWORD TIMESTART = 0;
DWORD TOTALIN = 0;
DWORD INREALPEEK = 0;
DWORD INREALLOW = -1;
DWORD INPEEK = 0;
DWORD OUTLASTGET = 0;
DWORD INLASTGET = 0;

DWORD LAG = 0;
DWORD LAG2 = 0;

int SlowMeDown = 0;

extern HANDLE hDrawThread;
extern HANDLE hMaintenanceThread;
extern HANDLE hMouseActionThread;
extern BOOL DoNotMove;

extern BOOL MESSAGE_THREAD;
extern BOOL MENU_THREAD;

extern CombatCursorVSF AttackCursorIcon;

int ResizeMax = 624;

extern BOOL INGAME;
char MaxCharactersPerAccount = 3;
unsigned char key;


unsigned char Direction;

extern LPDIRECTINPUTDEVICE lpDIMouseDevice; // Direct Input Device

char MenuName[100];

extern void LoadMusic(void);
extern void ReleaseMusic(void);

DWORD GetThreadTime(HANDLE h) {
    FILETIME Dumb;
    FILETIME Time;
    FILETIME Kernel;
    GetThreadTimes(h, &Dumb, &Dumb, &Kernel, &Time);
    DWORD dwT = (Time.dwHighDateTime << 19) + (Time.dwLowDateTime >> 13);
    dwT += (Kernel.dwHighDateTime << 19) + (Kernel.dwLowDateTime >> 13);
    return dwT;
}

inline int Abs(int x) {
    return ((x < 0) ? (-x) : (x));
}
bool Retrace = false;


extern T3VSBSound SoundFX[250];
T3VSBSound *Victim = NULL;

DWORD LastID;

extern bool INFINITELOOP;
//int Chose = 0;

extern HANDLE InterPacketEvent;

class InterPacket : public TemplateList <unsigned char *> {
public:
    void Set(void) {
        SetEvent(InterPacketEvent);
    }
};

extern InterPacket ug_InterPacket;

extern Font *fT4CDef_21;
extern Font *fT4CDef_22;
extern Font *fT4CDef_23B;
extern Font *fT4CDef_30B;
extern Font *fT4CDef_50B;
extern Font *fSystem_22;


extern Font *fSmallNormal_12;
extern Font *fSmallNormal_13;
extern Font *fSmallNormal_14;

extern Font *V3_BtnFontB;




int GeX = 0, GeY = 0;

extern bool WannaPos;
extern int SkillX;
extern int SkillY;

bool Shit = false;
bool LevelUp = false;
extern HWND g_hwnd;

DWORD DoubleClickTemp2 = -10000;
bool DoubleClick = false;

char Force = 0;

bool            Draw;
//char Text[1000];

bool Follow = false;
int FollowID = 0;
bool Err = true;

// TODO : TFC CLIENT JUNK
bool MenuThreadFinished = false;
//NMNMNM Connection
bool ConnectionThreadFinished = false;


//++NMNMNM --[NEW GUI CV2Sprite Sprite]
//extern Sprite IntroP[NM_CONNECT_SCREEN_NBR_BACK_ANIM];
//NMNMNM Connection
extern BOOL         g_bSecondLoadComplete;
extern BOOL         g_bSmoothFloorLoading;
extern void         SecondInitObject(LPVOID pParam);
extern void         SmoothInitObject(LPVOID pParam);
extern void         DrawSecondLoadingText();

extern bool MessageThreadFinished;
extern Random rnd;
//extern Sprite SplashP;
extern CV2Sprite    Splash;
extern volatile unsigned long Round;
extern char TFC_State;
char LastState = 0;
//extern CString ServerIP;
//extern unsigned int ServerPort;
extern bool Register;
bool bCanMovePL = true;
bool Move2 = true;
bool NMAutoMove = false;
int  NMAutoDir  = 0;

int xResize = 20;
int yResize = 30;
int ResizeCorrection = 0;

extern bool MOUSEINUSE;
extern VisualObjectList Objects;

bool SocketUsed = false;

extern Bitmap Grid;
extern Bitmap GridID;
extern Bitmap GridBlocking;

extern char TFC_State;

bool MOUSEINUSE = false;
DirectXInput KeyBoard;


extern CV2Sprite V3MouseCursor1;
extern CV2Sprite V3MouseCursor1A;
extern AnimV2Sprite V3MouseCursor2;
extern AnimV2Sprite V3MouseCursor3;
extern CV2Sprite V3MouseCursor5;
extern CV2Sprite V3MouseCursor5A;
extern CV2Sprite V3MouseCursor6;
extern CV2Sprite V3MouseCursor6A;
extern CV2Sprite V3MouseCursor7;
extern CV2Sprite V3MouseCursor7A;
extern CV2Sprite V3MouseCursor8;
extern CV2Sprite V3MouseCursor8A;
extern CV2Sprite V3MouseCursor9;
extern CV2Sprite V3MouseCursor9A;
extern CV2Sprite V3MouseCursor10;
extern CV2Sprite V3MouseCursor10A;
extern CV2Sprite V3MouseCursor11;
extern CV2Sprite V3MouseCursor11A;
extern CV2Sprite V3MouseCursor12;
extern CV2Sprite V3MouseCursor12A;
extern AnimV2Sprite V3MouseCursor16;
extern CV2Sprite V3MouseCursor17;
extern CV2Sprite V3MouseCursor17A;

extern CV2Sprite V3MouseCursor19;
extern CV2Sprite V3MouseCursor19A;
extern CV2Sprite V3MouseCursor20;
extern CV2Sprite V3MouseCursor20A;
extern CV2Sprite V3MouseCursor21;
extern CV2Sprite V3MouseCursor21A;


extern AnimV2Sprite MouseCursor18;

extern int Tick;

int  DrawLastMoving    = 0;
bool NeedRedraw = false;
bool WantGame = false;
bool WantPreGame = false;
int  g_chAlphaZone = 0;

void    FPS_Calculator(LPVOID pParam);
HANDLE  g_hExitFPSCalc = NULL;
HANDLE  g_hFPSThread   = NULL;
int     g_iDrawingFPSCnt  = 0;
int     g_iDrawingFPSLast = 0;
UINT    g_uiDrawPTimeCnt  = 0; 
UINT    g_uiDrawPTimeLast = 0;

extern bool ScreenDisplayThreadFinished;

extern int NBCURSOR;

//DIRECTDRAWSURFACE *lplpCursorSurface = NULL;
CV2Sprite **lplpCursor  = NULL;
CV2Sprite **lplpCursorA = NULL;

LPDIRECTDRAWSURFACE7 lpSafe;
BOOL bSafe = FALSE;


extern char *MsgDay;
//extern char *MsgDayLast;
UINT g_dwConnectionState = 0;
char g_strMessageStatus[5][512];

// t4c Update
DWORD g_dwVersion;
char g_lpszIP[256];
char g_lpszUser[256];
char g_lpszPath[256];
char g_lpszPassword[256];


#define PATCH_MOVE_MS 0


extern BOOL     g_bMp3ThreadRunning;
extern BOOL     g_bMp3ForceExit;
extern HANDLE  g_hMp3EventN;
extern HANDLE  g_hMp3EventP;
extern void    Mp3PlayerThread(LPVOID pParam);
extern HANDLE  g_hMp3EventPlay;


//Socket Items
char   g_ConnectionIP[100];
int    g_ConnectionPort = 0;
BOOL   g_InSocket           = FALSE;
UINT   g_SocketState        = 0;
UINT   g_SocketThreadID     = NULL;
HANDLE g_SocketThreadHandle = NULL;
UINT WINAPI SocketFunction(LPVOID pParam);



void COMMCallBack(COMM_INTR_PROTOTYPE);
void    CloseGlobalCOMM(LPVOID pParam);




TFCSocket::TFCSocket() {
    // Constructeur
    InitializeCriticalSection(&csTFCSocket);
}

TFCSocket::~TFCSocket() {
    // Destructeur
    DeleteCriticalSection(&csTFCSocket);
}

const int MAXMOV = 20;

bool Attack = false;
bool Get = false;
bool Use = false;
int XAttack;
int YAttack;
int XGet;
int YGet;
int XUse;
int YUse;

bool Patente = false;
sockaddr_in ReceiveIPaddr;
extern TFCSocket Remote;

BACK Backpack[24];

bool stShow = false;
int x = 0, y = 0, button[4];
//	int newbutton = 0; 
DWORD monSleep = 0;

bool bDisplayList = false;
char DisplayList[25][100];
int  DisplayInt[25];
int  nbList = 0;


DWORD Try = 0;

TemplateList <SysMsg> tlSystemMessage;


void ManageAutoMove(char* strKeyInput);


//##NMNMNM --[Main Draw Thread ZONE....]
UINT WINAPI DrawThread(LPVOID pParam) 
{
   g_App.StartThread(APP_DRAWING_THREAD);
   g_App.SetLoading( false );
   
   while (!g_Var.inGame)
   {
      Sleep(50);
   }


   OptionParam *pOption = g_SaveGame.GetOptionsParam();

   LONGLONG cur_time;

   double dFPSWanted32 = 32;
   double dFPSWanted16 = 16;

   DWORD    time_count32=(DWORD)(1000.00/dFPSWanted32);
   DWORD    time_count16=(DWORD)(1000.00/dFPSWanted16);


   LONGLONG perf_cnt;
   LONGLONG next_time=0;

   LONGLONG DrawTimeStart=0;
   LONGLONG DrawTimeStop =0;

   int iDrawingCnt = 0;
   DWORD dwDrawCount32 = 0;
  

   if (QueryPerformanceFrequency((LARGE_INTEGER *) &perf_cnt)) 
   {
      QueryPerformanceCounter((LARGE_INTEGER *) &next_time);
      time_count32= (DWORD)(perf_cnt/dFPSWanted32);
      time_count16= (DWORD)(perf_cnt/dFPSWanted16);
   } 

   g_DONE = 8;
   g_MOVX = 4;
   g_MOVY = 2;
   DWORD    time_count = time_count32;
   if(!g_SaveGame.GetOptionsParam()->bPlayAt32FPS)
   {
      time_count = time_count16;
      g_DONE = 4;
      g_MOVX = 8;
      g_MOVY = 4;
   }

   //Start FPS Calculation Thread
   g_hExitFPSCalc = CreateEvent(NULL,TRUE,FALSE,"");
   g_hFPSThread = (HANDLE)_beginthread(FPS_Calculator, 0, NULL );
   
   bool bIncreaseMove = true;
   while (!g_boQuitApp) 
   {
      

      while (!g_Var.inGame || g_uiReloadForceTeleport)
      {
         g_bDrawThreadPaused = true;
         Sleep(200);
         //OutputDebugString("********** g_Var.inGame || g_uiReloadForceTeleport\n");
      }
      g_bDrawThreadPaused = false;
      
      if(g_boQuitApp)
      {
         g_App.ExitThread(APP_DRAWING_THREAD);
         _endthreadex(1);
         return 1;
      }


      g_App.LockDrawThread();

      ProcessEventDraw();

      QueryPerformanceCounter((LARGE_INTEGER *) &cur_time);

      if (cur_time>next_time) 
      {
         if(Custom.gUpdateFPSOption)
         {
            Custom.gUpdateFPSOption = FALSE;
            time_count = time_count32;
            g_DONE = 8;
            g_MOVX = 4;
            g_MOVY = 2;
            if(!g_SaveGame.GetOptionsParam()->bPlayAt32FPS)
            {
               time_count = time_count16;
               g_DONE = 4;
               g_MOVX = 8;
               g_MOVY = 4;
            }
            dwDrawCount32 = 0;
         }

         if(g_SaveGame.GetOptionsParam()->bPlayAt32FPS)
         {
            if(++dwDrawCount32%2)
               bIncreaseMove = true;
            else
               bIncreaseMove = false;
         }
         else
            bIncreaseMove = true;
         

      
         DrawTimeStart = timeGetTime();
         ProcessThreadDraw(bIncreaseMove);
         DrawTimeStop = timeGetTime();

         //Look ending process
         if (stShow) 
         {
            Objects.Lock(121);
            Objects.SetTotalGrid();
            Objects.Unlock(121);
            stShow = false;
         }

         g_uiDrawPTimeCnt += (UINT)(DrawTimeStop-DrawTimeStart);

        
         
         if (DrawLastMoving)
         {
            DrawLastMoving--;
            if(!DrawLastMoving)
            {
               bCanMovePL = true;
            }
         }
         else
         {
            Draw = false;
         }
         
         next_time += time_count;
         
         // If we get more than a frame ahead, allow us to drop one
         // Otherwise, we will never catch up if we let the error
         // accumulate, and message handling will suffer
         
         if (next_time < cur_time)
            next_time = cur_time + time_count;

         g_iDrawingFPSCnt++;
      }
      else
         Sleep(1);


      g_App.UnlockDrawThread();

   }


   SetEvent(g_hExitFPSCalc);
   if(::WaitForSingleObject(g_hFPSThread, 500) == WAIT_TIMEOUT ) 
      TerminateThread( g_hFPSThread, 0x666 );
   CloseHandle(g_hFPSThread);
   CloseHandle(g_hExitFPSCalc);



   g_App.ExitThread(APP_DRAWING_THREAD);
   _endthreadex(1);
   return 1;
}


int Looping = 0;
int TalkToX = 0, TalkToY = 0, TalkToW = 0;
short TalkToOffset = 0;
unsigned long TalkToID = 0;
int NBCURSOR = 6;
DWORD LastMacro = 0;
DWORD tLastTime = 0;
int   tLastMinute = 0;
int   tLastHour = 0;


DWORD LastAsk3 = 0;

bool ForceSpell = false;
bool ForceSkill = false;
//


BOOL boKeyProcess = FALSE;

unsigned char *ucPacket = NULL;

void TFCSocket::MainThread(void) {
    g_App.StartThread(APP_MAINTENANCE_THREAD);
    InterPacketEvent = CreateEvent(NULL, false, false, "Socket Event");
    
    //   LPBYTE lpszCommandString = new BYTE [100];
    //   BYTE lpszReturnString[100];
    //            mciSendString("open cdaudio", (char *)lpszReturnString,
    //               sizeof(lpszReturnString), NULL);
    //            mciSendString("set cdaudio time format tmsf", (char *)lpszReturnString, 
    //               sizeof(lpszReturnString), NULL);
    //   GlobalCDTrack = 1;
    //   wsprintf((char *)lpszCommandString, "play cdaudio from %d to %d notify", GlobalCDTrack, GlobalCDTrack+1);
    //   mciSendString((char *)lpszCommandString, (char *)lpszReturnString, 
    //      sizeof(lpszReturnString), siGethWnd());
    //   
    if (TFC_State == TFC_CONNECT) 
    {
        //NMNMNM Connection
        ConnectionThread();
        if(!g_boQuitApp)
        {
            COMM.LongLive();
            COMM.StartSendAlive();
        }


        

        if(!g_boQuitApp)
        {
           char strTmp[512];
           CV2Sprite    ConnectBack;
           CV2Sprite    ConnectLogoT4cS[2];
           
           sprintf_s(strTmp,512,"%d%d",g_Global.GetDisplayW(),g_Global.GetDisplayH());
           ConnectBack.LoadSprite(strTmp);
           ConnectLogoT4cS[0].LoadSprite("V3_T4CSmall");
           ConnectLogoT4cS[1].LoadSprite("V3_T4CSmallMask");

           RECT rect;
           rect.left = 0;
           rect.top = 0;
           rect.right = g_Global.GetScreenW();
           rect.bottom = g_Global.GetScreenH();
           V2SPRITEFX vsffx2;
           ZeroMemory(&vsffx2, sizeof(V2SPRITEFX));
           vsffx2.lpClipRect = &rect;

           while(!g_bSecondLoadComplete)
           {
              Sleep(50);
              DXDClear();
              ConnectBack.DrawSpriteN(0, 0, &vsffx2);
              ConnectLogoT4cS[0].DrawSpriteN((g_Global.GetScreenW()-ConnectLogoT4cS[0].GetWidth())/2, 0, &vsffx2,&ConnectLogoT4cS[1]);


              DrawSecondLoadingText();
              DXDFlip(); //loading
              OutputDebugString("Wait Loaded first complete 01...\n");
           }

           ConnectBack.Release();
           ConnectLogoT4cS[0].Release();
           ConnectLogoT4cS[1].Release();

           /*
           BOOL bSame = TRUE;
           if((MsgDayLast==NULL || strcmp(MsgDayLast,MsgDay)!= 0)&& MsgDay)
           {
              bSame = FALSE;
              ClientConfig *pConfig = g_Global.GetClientConfig();
              //ecrit le msg of the day...
              char strFile[MAX_PATH];
              sprintf_s(strFile,MAX_PATH,"%s\\svrmsg.dat",pConfig->strSavePath);
              FILE *pfMD = NULL;
              fopen_s(&pfMD,strFile,"wb"); //oki messageoftheday
              if(pfMD)
              {
                 int dwTaille = strlen(MsgDay);
                 fwrite(&dwTaille,4,1,pfMD);
                 fwrite(MsgDay ,dwTaille,1,pfMD);
                 fclose(pfMD);
              }
           }
           */

           TFC_State = TFC_MSGDAY;
        }

        LastState = 0;
        if(!g_boQuitApp)
        {
           MenuThread();
		     KeyBoard.Acquire();
        }
    }

    
    
    
   
    Objects.Direction2 = 1;
    Backpack[0].nbObjects = 0;
    
    TIMESTART = timeGetTime();
    
    COMM.State = 2;
    
    
    
    
    // Variable Initialisation.
    int LastType = 0;
    
    BOOL bSendPreGame = FALSE;
    
    char *KBuffer = new char [256];
    memset(KBuffer, 0, 256);
    //	KeyBoard.Release();
    MouseDevice Mouse2;
    
    while (!g_boQuitApp) 
    {
       g_App.LockMainThread();
       
       Sleep(5);
       // While Main Thread
       if (!COMM.isAlive()) 
       {
          
          V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[98],CL_RED,true);          
          
          Sleep(2500);
          char Temp[100];
          sprintf_s(Temp,100, g_LocalString[98], 24, 0);
          g_App.SetError(1, Temp);
          PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
          Sleep(250);
          continue;
       } 
       
       //OutputDebugString("ici ici cii \n");
       if (WantPreGame && !bSendPreGame) 
       {
          //Change le loading ICI...
          DXDClear();
          DXDFlip();//avant enter game
          DXDClear();
          
          IDirectXImage diLoading;
          try 
          {
             // Load.PCX for 800 and 1024
             char strLOADName[128];
             sprintf_s(strLOADName,128,"Images\\load%d%d.PCX",g_Global.GetDisplayW(),g_Global.GetDisplayH());
             if (diLoading.LoadImage(strLOADName,g_Global.GetDisplayW(),g_Global.GetDisplayH())) 
             {
                DXDClear();
                DXDBlt(diLoading);
                DXDFlip();//avant enter game loading
                //DXDSetPalette(diLoading);
                diLoading.Release();
             }
          } 
          catch (...) 
          {
          };
          
          TFCPacket Send;
          bSendPreGame = TRUE;
          Send << (short)RQ_PutPlayerInGame;


          Send << (char)strlen(MenuName);
          Send << (char *)MenuName;
          Send << (long)Player.lKey;
          
          strcpy_s(Player.Name,256, MenuName);

          //++NMNMNM --[ Load Savegame Settings]
          g_SaveGame.Load_SaveGame(Player.Account,Player.Name);
          
          
          //Ici on peu starter le Mp3 si ye deja a ON...
          if(g_SaveGame.GetOptionsParam()->bStatusMp3 == MP3_PLAY)
          {
             g_GameMusic.LoadNewSound("mainThread");
             if(Custom.gMp3List.size() >0)
             {
                ::SetEvent(g_hMp3EventPlay);
             }
          }
          else
          {
             //si pas a play on disable car on supporte pas la derniere chanson paused...
             g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_DISABLE;
          }
          
          
          SEND_PACKET(Send);
          Sleep(250);
       }
       
       
       if (LevelUp) 
       {
          if (timeGetTime() - LastAsk3 > 5000) 
          {
             LevelUp = false;
          } 
       }
       
       if (timeGetTime() - tLastTime > 50)  //NMNMNMNMNMNMNM fake temnps pour aller tre stres vites
       {
          tLastTime = timeGetTime();
          g_TimeStructure.AddSeconde();
       }
       
       Objects.Lock(123);
       Objects.ChkText();
       Objects.Unlock(123); 
        
       
       if (!WantPreGame) 
       {
          
          if ((timeGetTime() - Try) > 100 && !bCanMovePL && !NeedRedraw) //NMNMNM MOVE TIME peutv etre
          {
             bCanMovePL = true;
          }
          
          if (!QuitFirstLoop) 
          {
             
             KeyBoard.KeyBuffer(KBuffer);
             ForceSpell = false;
             ForceSkill = false;
             if (KBuffer[DIK_LCONTROL] || KBuffer[DIK_RCONTROL]) 
             {
                ForceSpell = true;
             } 
             else if (KBuffer[DIK_LALT] || KBuffer[DIK_RALT]) 
             {
                ForceSkill = true;
             }
             
             
             CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
             
             //gestion des dialog qui block le mouvement
             BOOL boIsAnyInterfaceUIBlockingMove = FALSE;
             {	
                if ( (V3_PacmanDlg::GetInstance()->IsShown()) ||
                     (V3_TradeDlg ::GetInstance()->IsShown()) /*||*/
                   
                   )
                {
                   boIsAnyInterfaceUIBlockingMove = TRUE;
                }
                
             }
             
             
             ManageAutoMove(KBuffer);
             
             
             if (!boKeyProcess && !DoNotMove && Move2 && boIsAnyInterfaceUIBlockingMove == FALSE && (!(KBuffer[DIK_LCONTROL] || KBuffer[DIK_RCONTROL])) && (!(KBuffer[DIK_LSHIFT] || KBuffer[DIK_RSHIFT])) ) 
             {

                short shNLState = GetKeyState(VK_NUMLOCK);
                shNLState = 1-shNLState;
                
                if ((KBuffer[DIK_DOWN]  || (shNLState && KBuffer[DIK_NUMPAD2])) && (KBuffer[DIK_LEFT]  || (shNLState && KBuffer[DIK_NUMPAD4]))    )
                {
                   pfStopMovement("Move 5");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(139);
                   if (!GridBlocking(9, 16)) 
                   {
                      if (bCanMovePL) // move down left
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(6+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(139);
                }
                
                if ((KBuffer[DIK_DOWN]   || (shNLState && KBuffer[DIK_NUMPAD2])) &&(KBuffer[DIK_RIGHT]  || (shNLState && KBuffer[DIK_NUMPAD6]))    ) 
                {
                   pfStopMovement("Move 6");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(140);
                   if (!GridBlocking(11, 16)) 
                   {
                      if (bCanMovePL) 
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(4+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(140);
                }
                
                if ((KBuffer[DIK_UP]    || (shNLState && KBuffer[DIK_NUMPAD8])) &&(KBuffer[DIK_LEFT]  || (shNLState && KBuffer[DIK_NUMPAD4]))    )
                {
                   pfStopMovement("Move 7");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(141);
                   if (!GridBlocking(9, 14)) 
                   {
                      if (bCanMovePL)  // move up left
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(8+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(141);
                }
                
                if ((KBuffer[DIK_UP]     || (shNLState && KBuffer[DIK_NUMPAD8])) &&(KBuffer[DIK_RIGHT]  || (shNLState && KBuffer[DIK_NUMPAD6]))    )
                {
                   pfStopMovement("Move 8");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(142);
                   if (!GridBlocking(11, 14)) 
                   {
                      if (bCanMovePL) // move up right
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(2+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(142);
                }
                
                if (KBuffer[DIK_DOWN] || (shNLState && KBuffer[DIK_NUMPAD2])) 
                {
                   // Send Mouvement
                   pfStopMovement("Move 9");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(143);
                   if (!GridBlocking(10, 16)) 
                   {
                      if (bCanMovePL)  // move down 
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(5+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(143);
                }
                
                if (KBuffer[DIK_UP] || (shNLState && KBuffer[DIK_NUMPAD8])) 
                {
                   pfStopMovement("Move 10");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(144);
                   if (!GridBlocking(10, 14)) 
                   {
                      if (bCanMovePL)  // move up
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(1+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(144);
                } 
                
                if (KBuffer[DIK_LEFT] || (shNLState && KBuffer[DIK_NUMPAD4])) 
                {
                   pfStopMovement("Move 11");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(145);
                   if (!GridBlocking(9, 15)) 
                   {
                      if (bCanMovePL ) // move left
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(7+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(145);
                }
                
                if (KBuffer[DIK_RIGHT] || (shNLState && KBuffer[DIK_NUMPAD6])) 
                {
                   pfStopMovement("Move 12");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(146);
                   if (!GridBlocking(11, 15)) 
                   {
                      if (bCanMovePL) // move right
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(3+210);
                         SEND_PACKET(Send);
                         
                      }
                   }
                   Objects.Unlock(146);
                }
                
                if ((shNLState && KBuffer[DIK_HOME]) || (shNLState && KBuffer[DIK_NUMPAD7])) 
                {		
                   pfStopMovement("Move 13");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(147);
                   if (!GridBlocking(9, 14)) 
                   {
                      if (bCanMovePL) // move up left
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(8+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(147);
                } 
                
                if ((shNLState && KBuffer[DIK_END]) || (shNLState && KBuffer[DIK_NUMPAD1])) 
                {
                   pfStopMovement("Move 14");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(148);
                   if (!GridBlocking(9, 16)) 
                   {
                      if (bCanMovePL) // move bottom left
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(6+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(148);
                }
                
                if ((shNLState && KBuffer[DIK_PRIOR]) || (shNLState && KBuffer[DIK_NUMPAD9])) 
                {
                   pfStopMovement("Move 15");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(149);
                   if (!GridBlocking(11, 14)) 
                   {
                      if (bCanMovePL)  // move up right
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(2+210);
                         SEND_PACKET(Send);
                      }
                   }
                   Objects.Unlock(149);
                } 
                
                if ((shNLState && KBuffer[DIK_NEXT]) || (shNLState && KBuffer[DIK_NUMPAD3])) 
                { 
                   pfStopMovement("Move 16");
                   Follow  = false;
                   Attack = false;
                   Use = false;
                   Get = false;
                   Objects.Lock(150);
                   if (!GridBlocking(11, 16)) 
                   {
                      if (bCanMovePL)  // move down right
                      {
                         bCanMovePL = false;
                         Try = timeGetTime();
                         TFCPacket Send;
                         Send << (short)(4+210);
                         SEND_PACKET(Send); 
                      } 
                   }
                   Objects.Unlock(150);
                }
            }
      } 
      //Mouse.GetRelease(&button);
   } 
   
   g_App.UnlockMainThread();
   
   if(TFC_State == TFC_MENU)
   {
      MenuThreadFinished = false;
      MenuThread();
   }
   } //	while (!MainThreadFinished) 
   delete KBuffer;
   
   g_App.ExitThread(APP_MAINTENANCE_THREAD);
   _endthreadex(1);
}



UINT WINAPI MouseActionThread(LPVOID pParam)
{
    while( !g_boQuitApp )
    {
        if( !WantPreGame )
        {
            if(!g_uiReloadForce)
               MouseAction();
        }
        else
        {
            MouseActionReset();
        }
        Sleep( 20 ); //NMNMNM_MOUSE_ avant 50 now 20
    }
    return 0;
}



int QN[8];
int RN[8][5];
int QuestionNumber = 0;
BYTE QuestionAnswer[5];

VOID Shuffle(VOID) {
    
    Random Rnd8(0, 7, timeGetTime());
    Random Rnd5(0, 4, timeGetTime());
    
    int i, j, k;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 5; j++)
            RN[i][j] = 5;
    }
    
    for (i = 0; i < 8; i++)
        QN[i] = 6;
    
    for (i = 0; i < 8; i++) {
        QN[i] = Rnd8;
        
        BOOL Okay = TRUE;					
        
        for (j = 0; j < i; j++) {
            if (QN[i] == QN[j])
                Okay = FALSE;
        }
        
        if (!Okay)
            i--;
    }
    
    for (k = 0; k < 8; k++) {		
        for (i = 0; i < 5; i++) {
            RN[QN[k]][i] = Rnd5;
            
            BOOL Okay = TRUE;					
            
            for (j = 0; j < i; j++) {
                if (RN[QN[k]][i] == RN[QN[k]][j])
                    Okay = FALSE;
            }
            
            if (!Okay)
                i--;
        }
    }
}


BOOL IsOnButton(int Check,int x,int y,CV2Sprite *pVsf,int xS, int yS)
{
   if(!Check)
      return FALSE;

   if(x >= xS && x <= xS+pVsf->GetWidth() && y >= yS && y <= yS+pVsf->GetHeight())
      return TRUE;
   return FALSE;
}


namespace{

	T3VSBSound *GUI_BtnDOwn = NULL;

    void InitSound()
	{
        if( !GUI_BtnDOwn )
		{
			GUI_BtnDOwn = new T3VSBSound;
			GUI_BtnDOwn->Create( "Generic Drop Item", TS_MEMORY );
        }
    }
	void FreeSound()
	{
		if(GUI_BtnDOwn)
			delete GUI_BtnDOwn;
		GUI_BtnDOwn = NULL;
	}
};


//##NMNMNM --[Menu thread ZONE... Tous le loading....]


void ManageAutoMove(char* strKeyInput)
{
	short shNLState = GetKeyState(VK_NUMLOCK);
	shNLState = 1-shNLState;

	if(strKeyInput[DIK_LCONTROL] && strKeyInput[DIK_LMENU] && !NMAutoMove)
	{
		NMAutoDir = 0;
		if((strKeyInput[DIK_NUMPAD1] || strKeyInput[DIK_END]) && shNLState) //Down+Left
			NMAutoDir = 5;
		else if((strKeyInput[DIK_NUMPAD3] || strKeyInput[DIK_NEXT]) && shNLState) //Down+Right
			NMAutoDir = 6;
		else if((strKeyInput[DIK_NUMPAD7] || strKeyInput[DIK_HOME]) && shNLState) //Up+Left
			NMAutoDir = 7;
		else if((strKeyInput[DIK_NUMPAD9] || strKeyInput[DIK_PRIOR]) && shNLState) //Up+Right
			NMAutoDir = 8;
		else if((strKeyInput[DIK_NUMPAD8] || strKeyInput[DIK_UP]) && shNLState) //Up
			NMAutoDir = 1;
		else if((strKeyInput[DIK_NUMPAD6] || strKeyInput[DIK_RIGHT]) && shNLState) //Right
			NMAutoDir = 2;
		else if((strKeyInput[DIK_NUMPAD2] || strKeyInput[DIK_DOWN]) && shNLState) //Down
			NMAutoDir = 3;
		else if((strKeyInput[DIK_NUMPAD4] || strKeyInput[DIK_LEFT]) && shNLState) //Left
			NMAutoDir = 4;

		if(NMAutoDir > 0)
		{
			NMAutoMove = true;
			V3_ChatLogDlg::GetInstance()->AddListLogs("","AutoMove [Start]",CL_WHITE,true);
		}
	}
	else if(NMAutoMove)
	{
		//verifie si on dois desactiver le automove...
		if((strKeyInput[DIK_LCONTROL] && strKeyInput[DIK_LMENU] && strKeyInput[DIK_SPACE]) ||
			(shNLState && strKeyInput[DIK_NUMPAD5]) ||
			strKeyInput[DIK_F1 ] || strKeyInput[DIK_F2 ] || strKeyInput[DIK_F3 ] || strKeyInput[DIK_F4 ] ||
			strKeyInput[DIK_F5 ] || strKeyInput[DIK_F6 ] || strKeyInput[DIK_F7 ] || strKeyInput[DIK_F8 ] ||
			strKeyInput[DIK_F9 ] || strKeyInput[DIK_F10] || strKeyInput[DIK_F11] || strKeyInput[DIK_F12] )
		{
			NMAutoMove = false;
			NMAutoDir  = 0;
			V3_ChatLogDlg::GetInstance()->AddListLogs("","AutoMove [End]",CL_WHITE,true);
		}
		else
		{
			if((strKeyInput[DIK_NUMPAD1] || strKeyInput[DIK_END]) && shNLState) //Down+Left
				NMAutoDir = 5;
			else if((strKeyInput[DIK_NUMPAD3] || strKeyInput[DIK_NEXT]) && shNLState) //Down+Right
				NMAutoDir = 6;
			else if((strKeyInput[DIK_NUMPAD7] || strKeyInput[DIK_HOME]) && shNLState) //Up+Left
				NMAutoDir = 7;
			else if((strKeyInput[DIK_NUMPAD9] || strKeyInput[DIK_PRIOR]) && shNLState) //Up+Right
				NMAutoDir = 8;
			else if((strKeyInput[DIK_NUMPAD8] || strKeyInput[DIK_UP]) && shNLState) //Up
				NMAutoDir = 1;
			else if((strKeyInput[DIK_NUMPAD6] || strKeyInput[DIK_RIGHT]) && shNLState) //Right
				NMAutoDir = 2;
			else if((strKeyInput[DIK_NUMPAD2] || strKeyInput[DIK_DOWN]) && shNLState) //Down
				NMAutoDir = 3;
			else if((strKeyInput[DIK_NUMPAD4] || strKeyInput[DIK_LEFT]) && shNLState) //Left
				NMAutoDir = 4;

			if(NMAutoDir == 5)
			{
				strKeyInput[DIK_DOWN] = 1;
				strKeyInput[DIK_LEFT] = 1;
			}
			else if(NMAutoDir == 6)
			{
				strKeyInput[DIK_DOWN] = 1;
				strKeyInput[DIK_RIGHT] = 1;
			}
			else if(NMAutoDir == 7)
			{
				strKeyInput[DIK_UP] = 1;
				strKeyInput[DIK_LEFT] = 1;
			}
			else if(NMAutoDir == 8)
			{
				strKeyInput[DIK_UP] = 1;
				strKeyInput[DIK_RIGHT] = 1;
			}
			else if(NMAutoDir == 1)
			{
				strKeyInput[DIK_UP] = 1;
			}
			else if(NMAutoDir == 2)
			{
				strKeyInput[DIK_RIGHT] = 1;
			}
			else if(NMAutoDir == 3)
			{
				strKeyInput[DIK_DOWN] = 1;
			}
			else if(NMAutoDir == 4)
			{
				strKeyInput[DIK_LEFT] = 1;
			}
		}
	}
}




//NMNMNM Connection
void TFCSocket::ConnectionThread(void) 
{
/* 
   OPENFILENAME ofn;
   char szFileName[MAX_PATH] = "";

   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
   ofn.hwndOwner = g_hwnd;
   ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile = szFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT;
   ofn.lpstrDefExt = "txt";

   if(GetOpenFileName(&ofn))
   {
      // Do something usefull with the filename stored in szFileName 
   }
*/

   int x = 0, y = 0, button[4];
   ZeroMemory(button, 4*sizeof(int));

   char strAccount[50];
   char strPassword[50];
   char strAccountT[51];
   char strPasswordT[51];
   memset(strAccount,0x00,50);
   memset(strPassword,0x00,50);
   
   

   //liste des serveur...
   int  iServerSelected = 0;
   int  iNbrServerInList = 0; 
   char strServerListName[100][100];
   char strServerListIP[100][100];
   ClientConfig *pConfig = g_Global.GetClientConfig();
   char strFile[512];
   sprintf_s(strFile,512,"%s\\IPList.Txt",pConfig->strPath);
   char strLine[1000];
   char *pRet;
   FILE *pf1 = NULL;
   fopen_s(&pf1,strFile,"rt");
   if(pf1)
   {
      do 
      {
         pRet = fgets(strLine,1000,pf1);
         if(pRet)
         {
            char strN[1000];
            char strA[1000];
            strLine[strlen(strLine)-1] = 0x00;

            int iCntN = 0;
            int iCntA = 0;
            UINT iS = 0;
            for(UINT i=iS;i<strlen(strLine);i++,iS++)
            {
               if(strLine[i] != '#')
                  strN[iCntN++] = strLine[i];
               else
               {
                  iS++; //Skip the # char
                  break;
               }
            }
            for(UINT i=iS;i<strlen(strLine);i++)
            {
               strA[iCntA++] = strLine[i];
            }
            strN[iCntN] = 0x00;
            strA[iCntA] = 0x00;

            if(iCntN >0 && iCntN < 100 && iCntA >0 && iCntA < 100)
            {
               sprintf_s(strServerListName[iNbrServerInList],100,"%s",strN);
               sprintf_s(strServerListIP  [iNbrServerInList],100,"%s",strA);
               iNbrServerInList++;
            }
         }
      } while(pRet && iNbrServerInList <100);
      fclose(pf1);
   }

   /*
   if(pf1)
   {
      do 
      {
         pRet = fgets(strLine,1000,pf1);
         if(pRet)
         {
            strLine[strlen(strLine)-1] = 0x00;
            sprintf_s(strServerList[iNbrServerInList],100,"%s",strLine);
            iNbrServerInList++;
         }
      } while(pRet && iNbrServerInList <100);
      fclose(pf1);
   }
   else
   {
      sprintf_s(strServerList[0],100,"%s","Edit IPList.txt");
   }
   */ 
 

   //fichier de ip de dev pas en update
   char strFileDev[512];
   sprintf_s(strFileDev,512,"%s\\IPListDev.Txt",pConfig->strPath);
   
   FILE *pf1Dev = NULL;
   fopen_s(&pf1Dev,strFileDev,"rt");
   if(pf1Dev)
   {
      do 
      {
         pRet = fgets(strLine,1000,pf1Dev);
         if(pRet)
         {
            char strN[1000];
            char strA[1000];
            strLine[strlen(strLine)-1] = 0x00;

            int iCntN = 0;
            int iCntA = 0;
            UINT iS = 0;
            for(UINT i=iS;i<strlen(strLine);i++,iS++)
            {
               if(strLine[i] != '#')
                  strN[iCntN++] = strLine[i];
               else
               {
                  iS++; //Skip the # char
                  break;
               }
            }
            for(UINT i=iS;i<strlen(strLine);i++)
            {
               strA[iCntA++] = strLine[i];
            }
            strN[iCntN] = 0x00;
            strA[iCntA] = 0x00;

            if(iCntN >0 && iCntN < 100 && iCntA >0 && iCntA < 100)
            {
               sprintf_s(strServerListName[iNbrServerInList],100,"%s",strN);
               sprintf_s(strServerListIP  [iNbrServerInList],100,"%s",strA);
               iNbrServerInList++;
            }
         }
      } while(pRet && iNbrServerInList <100);
      fclose(pf1Dev);
   }
   /*
   if(pf1Dev)
   { 
      char *pRetDev;
      do 
      {
         pRetDev = fgets(strLine,1000,pf1Dev);
         if(pRetDev)
         {
            strLine[strlen(strLine)-1] = 0x00;
            sprintf_s(strServerList[iNbrServerInList],100,"%s",strLine);
            iNbrServerInList++;

         }
      } while(pRetDev && iNbrServerInList <100);
       fclose(pf1Dev);
   }
   */

  

   
   
   
   DWORD WaitTime;
   int Chose = 0;
   int InputSelect = 0;

   
   MouseDevice Mouse2;




   //select the last IP...
   int i=0;
   for(i=0;i<iNbrServerInList;i++)
   {
      if(strcmp(strServerListName[i],pConfig->strAccountSvrName) ==0)
      {
         iServerSelected = i;
         i = 99999;
      }
   }

   //if(strcmp(strAccount,"empty")==0)
   if(strAccount[0] == 0x00)
      InputSelect = 0;
   else
      InputSelect = 1;


   if (!KeyBoard.IsCreated()) 
   {
      KeyBoard.Create();
   }
   KeyBoard.CreateDevice(GUID_SysKeyboard);
   KeyBoard.Acquire();
   lplpCursor  = new CV2Sprite *[19];
   lplpCursorA = new CV2Sprite *[19];
   for(i=0;i<19;i++)
      lplpCursorA[i] = NULL;
   
   lplpCursor [0] = &V3MouseCursor1;
   lplpCursorA[0] = &V3MouseCursor1A;
   lplpCursor[1]  = &V3MouseCursor2;
   lplpCursorA[1] = NULL;
   lplpCursor[2]  = &V3MouseCursor3;
   lplpCursorA[2] = NULL;
   
   lplpCursor [3] = &V3MouseCursor1;
   lplpCursorA[3] = &V3MouseCursor1A;
   lplpCursor [4] = &V3MouseCursor1;
   lplpCursorA[4] = &V3MouseCursor1A;
   lplpCursor [5] = &V3MouseCursor1;
   lplpCursorA[5] = &V3MouseCursor1A;
   lplpCursor [6] = &V3MouseCursor1;
   lplpCursorA[6] = &V3MouseCursor1A;

   lplpCursor [7] = &V3MouseCursor5;
   lplpCursorA[7] = &V3MouseCursor5A;
   lplpCursor [8] = &V3MouseCursor6;
   lplpCursorA[8] = &V3MouseCursor6A;
   lplpCursor [9] = &V3MouseCursor7;
   lplpCursorA[9] = &V3MouseCursor7A;
   lplpCursor [10]= &V3MouseCursor8;
   lplpCursorA[10]= &V3MouseCursor8A;
   lplpCursor [11]= &V3MouseCursor9;
   lplpCursorA[11]= &V3MouseCursor9A;
   lplpCursor [12]= &V3MouseCursor10;
   lplpCursorA[12]= &V3MouseCursor10A;
   lplpCursor [13]= &V3MouseCursor11;
   lplpCursorA[13]= &V3MouseCursor11A;
   lplpCursor [14]= &V3MouseCursor12;
   lplpCursorA[14]= &V3MouseCursor12A;

   lplpCursor [15]= &V3MouseCursor1;
   lplpCursorA[15]= &V3MouseCursor1A;
   lplpCursor [16]= &V3MouseCursor16;
   lplpCursorA[16]= NULL;
   lplpCursor [17]= &V3MouseCursor17;
   lplpCursorA[17]= &V3MouseCursor17A;



   


   lplpCursor[18] = &MouseCursor18;

   DDSURFACEDESC2 ddSurfaceDesc;
   ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC2));
   
   ddSurfaceDesc.dwSize            = sizeof(ddSurfaceDesc);
   ddSurfaceDesc.dwFlags           = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
   ddSurfaceDesc.ddsCaps.dwCaps    = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
   ddSurfaceDesc.dwHeight          = 40;
   ddSurfaceDesc.dwWidth           = 47;
   
   DXDErrorHandle(lpDXDirectDraw->CreateSurfaceNM(&ddSurfaceDesc, &lpSafe, NULL), "Add Mouse Cursor 543");


   V2SPRITEFX vsffx;
   ZeroMemory(&vsffx, sizeof(V2SPRITEFX));
   RECT rect;
   rect.left = 0;
   rect.top = 0;
   rect.right = g_Global.GetScreenW();
   rect.bottom = g_Global.GetScreenH();
   vsffx.lpClipRect = &rect;
   vsffx.dwFX |= FX_FIT2SCREEN;

   V2SPRITEFX vsffx2;
   ZeroMemory(&vsffx2, sizeof(V2SPRITEFX));
   vsffx2.lpClipRect = &rect;
    
   if(Custom.gDisConMusic == FALSE)
      g_GameMusic.Start();

   InitSound();
   


   char strTmp[512];
   //Load Menu Images.
   
   CV2Sprite    ConnectBack;
   CV2Sprite    ConnectLogoDial[2];
   CV2Sprite    ConnectLogoDev[2];
   CV2Sprite    ConnectLogoT4cS[2];
   CV2Sprite    ConnectLogoT4cB[2];
   CV2Sprite    ConnectLoginBack[2];
   CV2Sprite    ConnectLoginBackE[2];

   CV2Sprite    ConnectLoginSpinUP[2];
   CV2Sprite    ConnectLoginSpinDN[2];
   CV2Sprite    ConnectLoginSelect;
   CV2Sprite    ConnectLoginBtn[2];
   CV2Sprite    ConnectSaveAccountPassword[2];

   sprintf_s(strTmp,512,"%d%d",g_Global.GetDisplayW(),g_Global.GetDisplayH());
   ConnectBack.LoadSprite(strTmp);

   ConnectLogoDial[0].LoadSprite("V3_DialLog");
   ConnectLogoDial[1].LoadSprite("V3_DialLogMask");

   ConnectLogoDev[0].LoadSprite("V3_t4cDevLogo");
   ConnectLogoDev[1].LoadSprite("V3_t4cDevLogoMask");

   ConnectLogoT4cS[0].LoadSprite("V3_T4CSmall");
   ConnectLogoT4cS[1].LoadSprite("V3_T4CSmallMask");

   ConnectLogoT4cB[0].LoadSprite("V3_T4CBig");
   ConnectLogoT4cB[1].LoadSprite("V3_T4CBigMask");

   ConnectLoginBack[0].LoadSprite("V3_LoginBackNew");
   ConnectLoginBack[1].LoadSprite("V3_LoginBackNewMask");

   ConnectLoginBackE[0].LoadSprite("V3_LoginBackEmptyNew");
   ConnectLoginBackE[1].LoadSprite("V3_LoginBackEmptyNewMask");

   ConnectLoginSpinUP[0].LoadSprite("V3_LoginSpinUP_N");
   ConnectLoginSpinUP[1].LoadSprite("V3_LoginSpinUP_H");

   ConnectLoginSpinDN[0].LoadSprite("V3_LoginSpinDN_N");
   ConnectLoginSpinDN[1].LoadSprite("V3_LoginSpinDN_H");

   ConnectLoginSelect.LoadSprite("V3_LoginBackSelect");

   ConnectLoginBtn[0].LoadSprite("V3_Btn92x24_N");
   ConnectLoginBtn[1].LoadSprite("V3_Btn92x24_H");

   ConnectSaveAccountPassword[0].LoadSprite("V3_Check18_OFF_N");
   ConnectSaveAccountPassword[1].LoadSprite("V3_Check18_ON_N");
  
   //cree la structure des login et pass...
   typedef struct _sLoginPass
   {
	   char strLogin[50];
	   char strPass[50];
   }sLoginPass;

	DWORD dwSAVEAccountPassword = 0;
   int  uiLastAccountConnected = -1;


    
   char strFileConnect[512];
   sprintf_s(strFileConnect,512,"%s\\Connect.bin",pConfig->strSavePath);

   std::vector<sLoginPass> vConnectList;

   long lNbrConnect = 0;
   //LoadConnectionFile
   FILE *pf = NULL;
   fopen_s(&pf,strFileConnect,"rb");
   if(pf)
   {
	   fread(&lNbrConnect,4,1,pf);
	   for(int i=0; i<lNbrConnect; i++)
	   {
		   sLoginPass sNewLogin;
			if(fread(sNewLogin.strLogin ,1,50,pf) == 50)
			{
				if(fread(sNewLogin.strPass  ,1,50,pf) == 50)
				{
					for(UINT k=0;k<strlen(sNewLogin.strLogin);k++)
					sNewLogin.strLogin[k] ^= Connect_key[k];
					for(UINT k=0;k<strlen(sNewLogin.strPass);k++)
					sNewLogin.strPass[k] ^= Connect_key[k];

					vConnectList.push_back(sNewLogin);
				}
			}
	   }
	   fclose(pf);
   }

   //faut setter selon le dernier loader...
   //strAccount    
   //strPassword

   RegKeyHandler regKey;
   if(regKey.Open( HKEY_CURRENT_USER, "Software\\Dialsoft\\T4CClient" ))
   {
      uiLastAccountConnected = regKey.GetProfileInt("LAST_CONNECTION_INDEX",-1);
      dwSAVEAccountPassword  = regKey.GetProfileInt("SAVE_PASSWORD",0);
   }
   if(vConnectList.size() > 0 && uiLastAccountConnected == -1)
      uiLastAccountConnected = 0;

   if(uiLastAccountConnected >=0 && uiLastAccountConnected < (int)vConnectList.size())
   {
      sprintf_s(strAccount ,50,"%s",vConnectList[uiLastAccountConnected].strLogin);
      sprintf_s(strPassword,50,"%s",vConnectList[uiLastAccountConnected].strPass);
   }
   else
   {
      uiLastAccountConnected = -1;
      strAccount[0] = 0x00;
      strPassword[0] = 0x00;
   }

   WaitTime = timeGetTime();
   
   #define CONNECT_STAT_MAIN               0
   #define CONNECT_STAT_STATUS             1
   #define CONNECT_STAT_STATUS_NOFEED      2
   #define CONNECT_STAT_STATUS_NOFEED_QUIT 3
   #define CONNECT_STAT_STATUS_ENTERGAME   4
   
   #define CONNECT_BTN_QUITTER       0
   #define CONNECT_BTN_CONNECT       1
   #define CONNECT_BTN_RETIRER       2
   #define CONNECT_BTN_OK            3
   #define CONNECT_BTN_SVR_UP        4
   #define CONNECT_BTN_SVR_DN        5
   #define CONNECT_BTN_LOGIN_UP      6
   #define CONNECT_BTN_LOGIN_DN      7
   #define CONNECT_BTN_SAVE_PASSWORD 8
   
   g_dwConnectionState = CONNECT_STAT_MAIN;
   
   DWORD dwLoadTime = timeGetTime();

   MOUSEINUSE = true;

   int  iCntBlinkCursor = 0;
   bool bShowCursor = false;
   while (!ConnectionThreadFinished && !g_boQuitApp) 
   {
      ++iCntBlinkCursor;
      if(iCntBlinkCursor > 10)
      {
         iCntBlinkCursor = 0;
         bShowCursor = !bShowCursor;
      }

      g_App.LockMainThread();
      Sleep(25);


      if (1 ) 
      {
         DXDClear();
         ConnectBack.DrawSpriteN(0, 0, &vsffx2);
         ConnectLogoDial[0].DrawSpriteN(0, g_Global.GetScreenH()-ConnectLogoDial[0].GetHeight(), &vsffx2,&ConnectLogoDial[1]);
         ConnectLogoDev[0].DrawSpriteN(g_Global.GetScreenW()-ConnectLogoDev[0].GetWidth(), g_Global.GetScreenH()-ConnectLogoDev[0].GetHeight(), &vsffx2,&ConnectLogoDev[1]);
         ConnectLogoT4cS[0].DrawSpriteN((g_Global.GetScreenW()-ConnectLogoT4cS[0].GetWidth())/2, 0, &vsffx2,&ConnectLogoT4cS[1]);
            
         HDC hdc;
         DXDGetDCConnect(&hdc, 19); 
         
         //dessine les textes...
         
         //SetBkMode(hdc, TRANSPARENT);
         //SetTextColor(hdc,RGB(255,255,255));

         //Copyright
         sprintf_s(strTmp,512,"2002-2012+ ©Copyright Dialsoft Inc. All right reserved.");
         //nom du server
         int iTextW = V3_BtnFontB->GetFLen(hdc, strTmp);
         V3_BtnFontB->DrawFont(hdc, (g_Global.GetScreenW()-iTextW)/2  ,  g_Global.GetScreenH()-20  , CL_WHITE        , 0, strTmp);
         
         DXDReleaseDC(hdc, 19);
      }


      switch (g_dwConnectionState) 
      {
         case CONNECT_STAT_STATUS_ENTERGAME:

            World.SetWorldFileName   ("Game Files\\V2_WorldMap.Map"    ,Player.ServerName);
            World.SetWorldFileName   ("Game Files\\V2_DungeonMap.Map"  ,Player.ServerName);
            World.SetWorldFileName   ("Game Files\\V2_CavernMap.Map"   ,Player.ServerName);
            World.SetWorldFileName   ("Game Files\\V2_Underworld.Map"  ,Player.ServerName);
            World.SetWorldFileName   ("Game Files\\V2_LeoWorld.Map"    ,Player.ServerName);
            World.SetWorldFileName   ("Game Files\\V2_Extension01.Map" ,Player.ServerName);	
            World.SetWorldFileName   ("Game Files\\V2_Extension02.Map" ,Player.ServerName);	
            World.SetWorldFileName   ("Game Files\\V2_Extension03.Map" ,Player.ServerName);

            Objects.FirstInitObjects();
            g_Global.InitZoneFile();

            //NMNMNM_SMALL_INSTALLFIX
            //Start le loading 2 ici on ets sure que la version est OK et patcher...
            g_bSmoothFloorLoading = FALSE;
            _beginthread( SmoothInitObject, 0, NULL );

            g_bSecondLoadComplete = FALSE;
            _beginthread( SecondInitObject, 0, NULL );

            ConnectionThreadFinished = true;
         break;
         case CONNECT_STAT_MAIN:
         {
            int dwVal;
            int x = 0;
            int y = 0;
            Mouse2.GetPosition(&x, &y);


            int iXBox =  (g_Global.GetScreenW()-ConnectLoginBack[0].GetWidth())/2;
            int iYBox =  ((g_Global.GetScreenH()-ConnectLoginBack[0].GetHeight())/5)*2;
            ConnectLoginBack[0].DrawSpriteN(iXBox, iYBox, &vsffx2,&ConnectLoginBack[1]);


            //la liste des IP donc spin up et down si plusieur IP
            if(iNbrServerInList >0)
            {
               int dwXup = iXBox+314;
               int dwYup = iYBox+53;
               int dwXdn = dwXup;
               int dwYdn = iYBox+64;

               if(x >= dwXup && x <= dwXup+ConnectLoginSpinUP[0].GetWidth() && y >= dwYup && y <= dwYup+ConnectLoginSpinUP[0].GetHeight()   )
                  ConnectLoginSpinUP[1] .DrawSpriteN(dwXup,dwYup,&vsffx2);
               else
                  ConnectLoginSpinUP[0] .DrawSpriteN(dwXup,dwYup,&vsffx2);
               
               if(x >= dwXdn && x <= dwXdn+ConnectLoginSpinDN[0].GetWidth() && y >= dwYdn && y <= dwYdn+ConnectLoginSpinDN[0].GetHeight()   )
                  ConnectLoginSpinDN[1] .DrawSpriteN(dwXdn,dwYdn,&vsffx2);
               else
                  ConnectLoginSpinDN[0] .DrawSpriteN(dwXdn,dwYdn,&vsffx2);
            }

            //le edit selectionner...
            if(InputSelect == 0)
               ConnectLoginSelect.DrawSpriteN(iXBox+97,iYBox+83,&vsffx2);
            else
               ConnectLoginSelect.DrawSpriteN(iXBox+97,iYBox+113,&vsffx2);

            //bouton ceooect et quitetr

            int dwXC = iXBox+55;
            int dwYC = iYBox+143+42;
            int dwXR = iXBox+152;
            int dwYR = dwYC;
            int dwXQ = iXBox+249;
            int dwYQ = dwYC;
            
            if(x >= dwXR && x <= dwXR+ConnectLoginBtn[0].GetWidth() && y >= dwYR && y <= dwYR+ConnectLoginBtn[0].GetHeight()   )
               ConnectLoginBtn[1] .DrawSpriteN(dwXR,dwYR,&vsffx2);
            else
               ConnectLoginBtn[0] .DrawSpriteN(dwXR,dwYR,&vsffx2);

            if(x >= dwXC && x <= dwXC+ConnectLoginBtn[0].GetWidth() && y >= dwYC && y <= dwYC+ConnectLoginBtn[0].GetHeight()   )
               ConnectLoginBtn[1] .DrawSpriteN(dwXC,dwYC,&vsffx2);
            else
               ConnectLoginBtn[0] .DrawSpriteN(dwXC,dwYC,&vsffx2);
            
            if(x >= dwXQ && x <= dwXQ+ConnectLoginBtn[0].GetWidth() && y >= dwYQ && y <= dwYQ+ConnectLoginBtn[0].GetHeight()   )
               ConnectLoginBtn[1] .DrawSpriteN(dwXQ,dwYQ,&vsffx2);
            else
               ConnectLoginBtn[0] .DrawSpriteN(dwXQ,dwYQ,&vsffx2);

            int dwXSP = iXBox+97;
            int dwYSP = iYBox+143;

            //dessine le checkBox
            if(dwSAVEAccountPassword)
               ConnectSaveAccountPassword[1] .DrawSpriteN(dwXSP,dwYSP,&vsffx2);
            else
               ConnectSaveAccountPassword[0] .DrawSpriteN(dwXSP,dwYSP,&vsffx2);

            {

               int dwXup = iXBox+314;
               int dwYup = iYBox+53+31;
               int dwXdn = dwXup;
               int dwYdn = iYBox+64+31;

               if(x >= dwXup && x <= dwXup+ConnectLoginSpinUP[0].GetWidth() && y >= dwYup && y <= dwYup+ConnectLoginSpinUP[0].GetHeight()   )
                  ConnectLoginSpinUP[1] .DrawSpriteN(dwXup,dwYup,&vsffx2);
               else
                  ConnectLoginSpinUP[0] .DrawSpriteN(dwXup,dwYup,&vsffx2);

               if(x >= dwXdn && x <= dwXdn+ConnectLoginSpinDN[0].GetWidth() && y >= dwYdn && y <= dwYdn+ConnectLoginSpinDN[0].GetHeight()   )
                  ConnectLoginSpinDN[1] .DrawSpriteN(dwXdn,dwYdn,&vsffx2);
               else
                  ConnectLoginSpinDN[0] .DrawSpriteN(dwXdn,dwYdn,&vsffx2);
            }

            
            //dessine les textes...
            HDC hdc;
            DXDGetDCConnect(&hdc, 19);
            //SetBkMode(hdc, TRANSPARENT);
            //SetTextColor(hdc,RGB(255,255,0));

            //dessine texte bouton...

            

            //Bouton Connect
            dwVal = V3_BtnFontB->GetFLen(hdc, g_LocalString[585]);
            dwVal = (ConnectLoginBtn[0].GetWidth()-dwVal)/2;
            V3_BtnFontB->DrawFont(hdc, dwXC+dwVal  ,  dwYC+5  , g_DefColorH        , 0, g_LocalString[585]);

            //Bouton Retirer
            dwVal = V3_BtnFontB->GetFLen(hdc, g_LocalString[743]);
            dwVal = (ConnectLoginBtn[0].GetWidth()-dwVal)/2;
            V3_BtnFontB->DrawFont(hdc, dwXR+dwVal  ,  dwYR+5  , g_DefColorH        , 0, g_LocalString[743]);

            //Bouton Quitter
            dwVal = V3_BtnFontB->GetFLen(hdc, g_LocalString[6]);
            dwVal = (ConnectLoginBtn[0].GetWidth()-dwVal)/2;
            V3_BtnFontB->DrawFont(hdc, dwXQ+dwVal  ,  dwYQ+5  , g_DefColorH        , 0, g_LocalString[6]);

            //Enregistrer password
            dwVal = V3_BtnFontB->GetFLen(hdc, g_LocalString[6]);
            dwVal = (ConnectLoginBtn[0].GetWidth()-dwVal)/2;
            V3_BtnFontB->DrawFont(hdc, iXBox+124  ,  iYBox+144  , g_DefColorH        , 0, g_LocalString[730]);

            //Nom du Serveur
            dwVal = fT4CDef_23B->GetFLen(hdc, strServerListName[iServerSelected]);
            dwVal = (217-dwVal)/2;
            fT4CDef_23B->DrawFont(hdc, iXBox+97+dwVal  ,  iYBox+53  , CL_WHITE        , 0, strServerListName[iServerSelected]);

            if(Custom.NoConfigFileFound)
            {

               dwVal = V3_BtnFontB->GetFLen(hdc, "Welcome on T4C !   Default configuration used...   You can use T4CConfig.exe to setup the game.");
               dwVal = (g_Global.GetScreenW()-dwVal)/2;
               fT4CDef_23B->DrawFont(hdc, dwVal  ,  g_Global.GetScreenH()-120  , g_DefColorD        , 0, "Welcome on T4C !   Default configuration used...   You can use T4CConfig.exe to setup the game.");

               dwVal = V3_BtnFontB->GetFLen(hdc, "Bienvenue sur T4C !   La configuration par défaut est utilisé...   Utilisez T4CConfig.exe pour configurer le jeu.");
               dwVal = (g_Global.GetScreenW()-dwVal)/2;
               fT4CDef_23B->DrawFont(hdc, dwVal  ,  g_Global.GetScreenH()-100  , g_DefColorD        , 0, "Bienvenue sur T4C !   La configuration par défaut est utilisé...   Utilisez T4CConfig.exe pour configurer le jeu.");

               dwVal = V3_BtnFontB->GetFLen(hdc, "Willkommen auf T4C !   Default-Konfiguration verwendet... Sie können T4CConfig.exe zur Einrichtung Spiels.");
               dwVal = (g_Global.GetScreenW()-dwVal)/2;
               fT4CDef_23B->DrawFont(hdc, dwVal  ,  g_Global.GetScreenH()-80  , g_DefColorD        , 0, "Willkommen auf T4C !   Default-Konfiguration verwendet... Sie können T4CConfig.exe zur Einrichtung Spiels.");


               //MessageBox(NULL,"Welcome on T4C !\nPlease proceed to the basic configuration before launching the game.\n\n"\
               //   "Bienvenue sur T4C !\nVeuillez procéder à la configuration de base avant le lancement du jeu.\n\n"\
               //   "Willkommen T4C !\nBitte fahren Sie mit der Grundkonfiguration vor dem Start des Spiels"

            }

            //ACCOUNT
            if(InputSelect == 0)
            {
               if(bShowCursor)
                  sprintf_s(strAccountT,51,"%s|",strAccount);
               else
                  sprintf_s(strAccountT,51,"%s",strAccount);
            }
            else
               sprintf_s(strAccountT,51,"%s",strAccount);
            
            
            fT4CDef_23B    ->DrawFont(hdc, iXBox+107 , iYBox+84 , CL_WHITE, 0, strAccountT);
            fSmallNormal_14->DrawFont(hdc, iXBox+75  , iYBox+84+6 , g_DefColorH, 0, g_LocalString[685]);
            


            //Password
            char strCryptPass[50];
            memset(strCryptPass,0x00,50);
            for(UINT i=0;i<strlen(strPassword);i++)
               strCryptPass[i] = 'x';
            if(InputSelect == 1)
            {
               if(bShowCursor)
                  sprintf_s(strPasswordT,51,"%s|",strCryptPass);
               else
                  sprintf_s(strPasswordT,51,"%s",strCryptPass);
            }
            else
               sprintf_s(strPasswordT,51,"%s",strCryptPass);
            
            fT4CDef_23B     ->DrawFont(hdc, iXBox+107 , iYBox+114 , CL_WHITE, 0, strPasswordT);
            fSmallNormal_14 ->DrawFont(hdc, iXBox+32  , iYBox+114+6 , g_DefColorH, 0, g_LocalString[686]);
            
            
            DXDReleaseDC(hdc, 19);
            

         }
         break;
         case CONNECT_STAT_STATUS:
         case CONNECT_STAT_STATUS_NOFEED:
         case CONNECT_STAT_STATUS_NOFEED_QUIT:
         {
            int x = 0;
            int y = 0;
            Mouse2.GetPosition(&x, &y);

            
            
            int iXBox =  (g_Global.GetScreenW()-ConnectLoginBackE[0].GetWidth())/2;
            int iYBox =  ((g_Global.GetScreenH()-ConnectLoginBackE[0].GetHeight())/5)*2;
            ConnectLoginBackE[0].DrawSpriteN(iXBox, iYBox, &vsffx2,&ConnectLoginBackE[1]);

           

            //bouton ceooect et quitetr
            
            int dwXOK = iXBox+150;
            int dwYOK = iYBox+143+42;
            
            if(g_dwConnectionState == CONNECT_STAT_STATUS)
            {
               if(x >= dwXOK && x <= dwXOK+ConnectLoginBtn[0].GetWidth() && y >= dwYOK && y <= dwYOK+ConnectLoginBtn[0].GetHeight()   )
                  ConnectLoginBtn[1] .DrawSpriteN(dwXOK,dwYOK,&vsffx2);
               else
                  ConnectLoginBtn[0] .DrawSpriteN(dwXOK,dwYOK,&vsffx2);
            }


            HDC hdc;
            DXDGetDCConnect(&hdc, 20);

            int dwVal;
            
            if(g_dwConnectionState == CONNECT_STAT_STATUS)
            {
               //Bouton 
               dwVal = V3_BtnFontB->GetFLen(hdc, g_LocalString[586]);
               dwVal = (ConnectLoginBtn[0].GetWidth()-dwVal)/2;
               V3_BtnFontB->DrawFont(hdc, dwXOK+dwVal  ,  dwYOK+5  , g_DefColorH        , 0, g_LocalString[586]);
            }

            //status texte ;
            for(int i=0;i<5;i++)
            {
               dwVal = fT4CDef_23B->GetFLen(hdc, g_strMessageStatus[i]);
               dwVal = (ConnectLoginBackE[0].GetWidth()-dwVal)/2;
               fT4CDef_23B->DrawFont(hdc, iXBox+dwVal  ,  iYBox+80+(i*20)  , CL_WHITE   , 0, g_strMessageStatus[i]);
            }


            DXDReleaseDC(hdc, 20);

         }
         break;
      
      }

      if (MOUSEINUSE) 
      {
         Mouse2.GetPosition(&x, &y);
         CMouseCursor *pMouse = CMouseCursor::GetInstance();
         pMouse->DrawCursor(x, y); // 1
      }
      
      Sleep(10);
      
      Chose = 0;
      
      
      
      
      DXDFlip(); //menu connect
      
     
      int xp, yp;
      Mouse2.GetStatus(&xp, &yp, button);
      x = xp;
      y = yp;
      if (button[0] == DM_CLICK) 
      {
          Chose = 1;
          DBL = FALSE;
      }
      if (button[0] == DM_DOUBLE_CLICK) 
      {
          Chose = 1;
          DBL = TRUE;
      }



      switch (g_dwConnectionState) 
      {
         case CONNECT_STAT_MAIN: //OKOKOK
         {
            int iXBox =  (g_Global.GetScreenW()-ConnectLoginBack[0].GetWidth())/2;
            int iYBox =  ((g_Global.GetScreenH()-ConnectLoginBack[0].GetHeight())/5)*2;
            int dwXC = iXBox+55;
            int dwYC = iYBox+143+42;
            int dwXR = iXBox+152;
            int dwYR = dwYC;
            int dwXQ = iXBox+249;
            int dwYQ = dwYC;
            

            int dwBtnMouseClick = -1;

            if(Chose == 1)
            {
               int dwXup = iXBox+314;
               int dwYup = iYBox+53;
               int dwXdn = dwXup;
               int dwYdn = iYBox+64;
               int dwXA  = iXBox+97;
               int dwYA  = iYBox+83;
               int dwXP  = iXBox+97;
               int dwYP  = iYBox+113;

               int dwXupL = iXBox+314;
               int dwYupL = iYBox+53+31;
               int dwXdnL = dwXup;
               int dwYdnL = iYBox+64+31;

               int dwXSP = iXBox+97;
               int dwYSP = iYBox+143;

               //dessine le checkBox
               if(dwSAVEAccountPassword)
                  ConnectSaveAccountPassword[1] .DrawSpriteN(dwXSP,dwYSP,&vsffx2);
               else
                  ConnectSaveAccountPassword[0] .DrawSpriteN(dwXSP,dwYSP,&vsffx2);
              
               if(x >= dwXSP && x <= dwXSP+ConnectLoginBtn[0].GetWidth() && y >= dwYSP && y <= dwYSP+ConnectLoginBtn[0].GetHeight())
               {
                  dwBtnMouseClick = CONNECT_BTN_SAVE_PASSWORD;
                  GUI_BtnDOwn->Play(2);
               }
               else if(x >= dwXQ && x <= dwXQ+ConnectLoginBtn[0].GetWidth() && y >= dwYQ && y <= dwYQ+ConnectLoginBtn[0].GetHeight())
               {
                  dwBtnMouseClick = CONNECT_BTN_QUITTER;
                  GUI_BtnDOwn->Play(2);
               }
               else if(x >= dwXC && x <= dwXC+ConnectLoginBtn[0].GetWidth() && y >= dwYC && y <= dwYC+ConnectLoginBtn[0].GetHeight())
               {
                  dwBtnMouseClick = CONNECT_BTN_CONNECT;
                  GUI_BtnDOwn->Play(2);
               }
               else if(x >= dwXR && x <= dwXR+ConnectLoginBtn[0].GetWidth() && y >= dwYR && y <= dwYR+ConnectLoginBtn[0].GetHeight())
               {
                  dwBtnMouseClick = CONNECT_BTN_RETIRER;
                  GUI_BtnDOwn->Play(2);
               }
               else if(x >= dwXup && x <= dwXup+ConnectLoginSpinUP[0].GetWidth() && y >= dwYup && y <= dwYup+ConnectLoginSpinUP[0].GetHeight() && iNbrServerInList >0)
               {
                  dwBtnMouseClick = CONNECT_BTN_SVR_UP;
                  GUI_BtnDOwn->Play(2);
               }
               else if(x >= dwXdn && x <= dwXdn+ConnectLoginSpinDN[0].GetWidth() && y >= dwYdn && y <= dwYdn+ConnectLoginSpinDN[0].GetHeight() && iNbrServerInList >0)
               {
                  dwBtnMouseClick = CONNECT_BTN_SVR_DN;
                  GUI_BtnDOwn->Play(2);
               }
               else if(x >= dwXupL && x <= dwXupL+ConnectLoginSpinUP[0].GetWidth() && y >= dwYupL && y <= dwYupL+ConnectLoginSpinUP[0].GetHeight())
               {
                  dwBtnMouseClick = CONNECT_BTN_LOGIN_UP;
                  GUI_BtnDOwn->Play(2);
               }
               else if(x >= dwXdnL && x <= dwXdnL+ConnectLoginSpinDN[0].GetWidth() && y >= dwYdnL && y <= dwYdnL+ConnectLoginSpinDN[0].GetHeight())
               {
                  dwBtnMouseClick = CONNECT_BTN_LOGIN_DN;
                  GUI_BtnDOwn->Play(2);
               }
               
               else if(x >= dwXA && x <= dwXA+ConnectLoginSelect.GetWidth() && y >= dwYA && y <= dwYA+ConnectLoginSelect.GetHeight())
               {
                  InputSelect = 0;
               }
               else if(x >= dwXP && x <= dwXP+ConnectLoginSelect.GetWidth() && y >= dwYP && y <= dwYP+ConnectLoginSelect.GetHeight())
               {
                  InputSelect = 1;
               }
            }

            

            if(dwBtnMouseClick == CONNECT_BTN_SAVE_PASSWORD)
            {
               dwSAVEAccountPassword = 1-dwSAVEAccountPassword;
            }

            if(dwBtnMouseClick == CONNECT_BTN_LOGIN_UP)
            {
               if(uiLastAccountConnected >=1)
               {
                  uiLastAccountConnected--;
                  if(uiLastAccountConnected < 0)
                     uiLastAccountConnected = 0;

                  if(vConnectList.size() >0)
                  {
                     sprintf_s(strAccount ,50,"%s",vConnectList[uiLastAccountConnected].strLogin);
                     sprintf_s(strPassword,50,"%s",vConnectList[uiLastAccountConnected].strPass);
                  }
                  
               }
               
            }
            else if(dwBtnMouseClick == CONNECT_BTN_LOGIN_DN)
            {
               if(uiLastAccountConnected >=0)
               {
                  uiLastAccountConnected++;
                  if(uiLastAccountConnected >= (int)vConnectList.size())
                     uiLastAccountConnected = vConnectList.size()-1;

                  if(vConnectList.size() >0)
                  {
                     sprintf_s(strAccount ,50,"%s",vConnectList[uiLastAccountConnected].strLogin);
                     sprintf_s(strPassword,50,"%s",vConnectList[uiLastAccountConnected].strPass);
                  }
               }
               
            }

            else if(dwBtnMouseClick == CONNECT_BTN_SVR_UP && iNbrServerInList >0)
            {
               iServerSelected--;
               if(iServerSelected < 0)
                  iServerSelected = 0;
            }
            else if(dwBtnMouseClick == CONNECT_BTN_SVR_DN  && iNbrServerInList >0)
            {
               iServerSelected++;
               if(iServerSelected >=iNbrServerInList)
                  iServerSelected = iNbrServerInList-1;
            }
            else if (key == 27 ||  dwBtnMouseClick==CONNECT_BTN_QUITTER) 
            {
               Chose = 2;
               key = 0;
               if(ucPacket)
                  delete ucPacket;
               ucPacket = NULL;
               g_boQuitApp = 1;
               //on save les chnagement a la liste...
               FILE *pfW = NULL;
               
               //Save la liste des connecter
               fopen_s(&pfW,strFileConnect,"wb"); //oki connect name 
               if(pfW)
               {
                  lNbrConnect = vConnectList.size();
                  fwrite(&lNbrConnect,4,1,pfW);
                  for(UINT i=0; i<vConnectList.size(); i++)
                  {
                     for(UINT k=0;k<strlen(vConnectList[i].strLogin);k++)
                        vConnectList[i].strLogin[k] ^= Connect_key[k];
                     for(UINT k=0;k<strlen(vConnectList[i].strPass);k++)
                        vConnectList[i].strPass[k] ^= Connect_key[k];

                     fwrite(vConnectList[i].strLogin ,1,50,pfW);
                     fwrite(vConnectList[i].strPass  ,1,50,pfW);
                  }
                  fclose(pfW);
               }

               PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x1111);
               Sleep(500);
               exit(0);
               
               return;
            }
            else if(dwBtnMouseClick==CONNECT_BTN_RETIRER)
            {
               int clearThisLogin = 0;
               vector< sLoginPass >::iterator i;
               for( i = vConnectList.begin(); i != vConnectList.end(); i++ )
               {					
                  if(strcmp(strAccount,i->strLogin)==0)
                  {
                     strAccount[0] = 0x00;
                     strPassword[0] = 0x00;
                     vConnectList.erase(i);
                     break;
                  }
               }
               clearThisLogin++;

               

            }
            else if (key == 13 ||  dwBtnMouseClick==CONNECT_BTN_CONNECT) 
            {
               //dois lancer le connect au serveur...
               if(strAccount[0] == 0x00)
               {
                  g_dwConnectionState = CONNECT_STAT_STATUS;
                  for(int r=0;r<5;r++)   
                     g_strMessageStatus[r][0] = 0x00;
                  sprintf_s(g_strMessageStatus[0],512,"%s",g_LocalString[587]);

               }
               else if(strPassword[0] == 0)
               {
                  g_dwConnectionState = CONNECT_STAT_STATUS;
                  for(int r=0;r<5;r++)   
                     g_strMessageStatus[r][0] = 0x00;
                  sprintf_s(g_strMessageStatus[0],512,"%s",g_LocalString[588]);
               }
               else
               { 
                  //Try connect....
                  if(g_ConnectionPort != 0)
                  {
                     sprintf_s(strServerListIP[iServerSelected],100,"%s:%d",strServerListIP[iServerSelected],g_ConnectionPort);
                  }
                  char *next_token1 = NULL;
                  char * ServerPort = strtok_s(strServerListIP[iServerSelected], ":",&next_token1);
                  ServerPort = strtok_s(NULL, ":",&next_token1); 
                  sprintf_s(g_ConnectionIP,100,"%s",strServerListIP[iServerSelected]);
                  if (ServerPort)
                  {
                     g_ConnectionPort = atoi(ServerPort);
                     
                  }
                  else
                     g_ConnectionPort = 11677;

                  


                  COMM.Close();
                  COMM.SetIPAddr(g_ConnectionIP);
                  COMM.SetAddrPort(g_ConnectionPort);
                  if (!COMM.Create(COMMCallBack)) 
                  {
                     g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;
                     for(int r=0;r<5;r++)   
                        g_strMessageStatus[r][0] = 0x00;
                     FormatText HelpText2;
                     HelpText2.SetText(g_LocalString[84]);
                     HelpText2.Format(50);
                     for(int i=0;i<HelpText2.GetnbLine();i++)
                     {
                        if(i <5)
                           sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
                     }
                     _beginthread( CloseGlobalCOMM, 0, NULL );
                  }
               
                  else
                  {
                     //Set connect configuration...
                     sprintf_s(Player.Account  ,256,"%s",strAccount);
                     sprintf_s(Player.Password ,256,"%s",strPassword);
					      Player.PortCom = g_ConnectionPort;
                     
                     sprintf_s(pConfig->strAccountIP     , 50,"%s",strServerListIP[iServerSelected]);
                     sprintf_s(Player.ServerName         ,256,"%s",strServerListName[iServerSelected]);
                     sprintf_s(pConfig->strAccountSvrName,128,"%s",strServerListName[iServerSelected]);
                     
                     

                     g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;
                     for(int r=0;r<5;r++)   
                        g_strMessageStatus[r][0] = 0x00;
                     FormatText HelpText2;
                     HelpText2.SetText(g_LocalString[248]);
                     HelpText2.Format(50);
                     for(int i=0;i<HelpText2.GetnbLine();i++)
                     {
                        if(i <5)
                           sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
                     }
                     g_SocketState = 1;
                     g_SocketThreadHandle = (HANDLE) _beginthreadex(NULL, 0, SocketFunction, NULL, NULL, &g_SocketThreadID);
                  }
               }  
               key = 0;
            }
            else if(key == 200 || key == 201 || key == 202)
            {
               InputSelect = 1-InputSelect;
               key = 0;
            }
            else
            {
               if (key == 8) 
               {
                  if(InputSelect == 0)
                  {
                     if (strlen(strAccount) > 0)
                        strAccount[strlen(strAccount)-1] = 0;
                  }
                  else
                  {
                     if (strlen(strPassword) > 0)
                        strPassword[strlen(strPassword)-1] = 0;
                  }
                  key = 0;
               } 
               else 
               {
                  if (key) 
                  {
                     if(InputSelect == 0)
                     {
                        if (strlen(strAccount) < 20) 
                        {
                           char Temp[2];
                           Temp[0] = key;
                           Temp[1] = 0;
                           strcat_s(strAccount,50, Temp);
                        }
                     }
                     else
                     {
                        if (strlen(strPassword) < 20) 
                        {
                           char Temp[2];
                           Temp[0] = key;
                           Temp[1] = 0;
                           strcat_s(strPassword,50, Temp);
                        }
                     }
                     key = 0;
                  }
               }
            
            }

            
         } 
         break;
         case CONNECT_STAT_STATUS:
         case CONNECT_STAT_STATUS_NOFEED:
         {
            int iXBox =  (g_Global.GetScreenW()-ConnectLoginBackE[0].GetWidth())/2;
            int iYBox =  ((g_Global.GetScreenH()-ConnectLoginBackE[0].GetHeight())/5)*2;

            int dwXOK = iXBox+150;
            int dwYOK = iYBox+143+42;

            int dwBtnMouseClick = -1;

            if(g_dwConnectionState != CONNECT_STAT_STATUS_NOFEED)
            {
               if(Chose == 1)
               {
                  if(x >= dwXOK && x <= dwXOK+ConnectLoginBtn[0].GetWidth() && y >= dwYOK && y <= dwYOK+ConnectLoginBtn[0].GetHeight())
                  {
                     dwBtnMouseClick = CONNECT_BTN_OK;
                     GUI_BtnDOwn->Play(2);
                  }
               }

               if (key == 13 || key == 27 || dwBtnMouseClick==CONNECT_BTN_OK) 
               {
                  g_dwConnectionState = CONNECT_STAT_MAIN;
                  key = 0;
               }
            }

         }
         break;
         case CONNECT_STAT_STATUS_NOFEED_QUIT:
            Chose = 2;
            key = 0;
            if(ucPacket)
               delete ucPacket;
            ucPacket = NULL;

            TFCPacket sending;
            sending << (RQ_SIZE)RQ_ExitGame;
            SEND_PACKET( sending );

            //do not quit t4c update will close client direct...

            //Sleep(2000);
            //PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x1111);
            //Sleep(250);
         break;
      } 
 

      
     
      g_App.UnlockMainThread();
   } //	while (!MainThreadFinished) 
   
   //delete [] Dest;
   if(ucPacket)
      delete ucPacket;
   ucPacket = NULL;
   FreeSound();


   //On peu deleter tous les graphieur utiliser ici...

   for(int k=0;k<2;k++)
   {
      ConnectLogoDial[k].Release();
      ConnectLogoDev[k].Release();
      ConnectLogoT4cS[k].Release();
      ConnectLogoT4cB[k].Release();
      ConnectLoginBack[k].Release();
      ConnectLoginBackE[k].Release();
      ConnectLoginSpinUP[k].Release();
      ConnectLoginSpinDN[k].Release();
      ConnectLoginBtn[k].Release();
	  ConnectSaveAccountPassword[i].Release();
   }
   ConnectBack.Release();
   ConnectLoginSelect.Release();
  

   int ilastLoginIndex = -1;
   if(g_dwConnectionState == CONNECT_STAT_STATUS_ENTERGAME)
   {
      //oki on save si le compte existe pas le new compte
	   bool bFound = false;
	   for(UINT i=0;i<vConnectList.size();i++)
	   {
		   if(strcmp(vConnectList[i].strLogin,Player.Account) == 0)
		   {
            ilastLoginIndex = (int)i;
			   bFound = true;
			   if(dwSAVEAccountPassword)
               sprintf_s(vConnectList[i].strPass,50,"%s",Player.Password);
            else
               vConnectList[i].strPass[0] = 0x00; // reset password
		   }
	   }

      if(!bFound)
      {
         sLoginPass sNewLogin;
         sprintf_s(sNewLogin.strLogin,50,"%s",Player.Account);
         if(dwSAVEAccountPassword)
            sprintf_s(sNewLogin.strPass,50,"%s",Player.Password);
         else
            sNewLogin.strPass[0] = 0x00; // reset password
         vConnectList.push_back(sNewLogin);

         ilastLoginIndex = vConnectList.size()-1;
      }

      //on save les chnagement a la liste...
      FILE *pfW = NULL;
      fopen_s(&pfW,strFileConnect,"wb"); //oki connect name 
      if(pfW)
      {
         lNbrConnect = vConnectList.size();
         fwrite(&lNbrConnect,4,1,pfW);
         for(UINT i=0; i<vConnectList.size(); i++)
         {
            for(UINT k=0;k<strlen(vConnectList[i].strLogin);k++)
               vConnectList[i].strLogin[k] ^= Connect_key[k];
            for(UINT k=0;k<strlen(vConnectList[i].strPass);k++)
               vConnectList[i].strPass[k] ^= Connect_key[k];

            fwrite(vConnectList[i].strLogin ,1,50,pfW);
            fwrite(vConnectList[i].strPass  ,1,50,pfW);
         }
         fclose(pfW);
      }
   }


 
   if(!regKey.Open( HKEY_CURRENT_USER, "Software\\Dialsoft\\T4CClient" ))
   {
      regKey.Create(HKEY_CURRENT_USER,"Software\\Dialsoft\\T4CClient" );
      regKey.WriteProfileInt("LAST_CONNECTION_INDEX",ilastLoginIndex);
      regKey.WriteProfileInt("SAVE_PASSWORD",dwSAVEAccountPassword);

   }
   else
   {
      regKey.WriteProfileInt("LAST_CONNECTION_INDEX",ilastLoginIndex);
      regKey.WriteProfileInt("SAVE_PASSWORD",dwSAVEAccountPassword);
   }
}


void CloseGlobalCOMM(LPVOID pParam)
{
   COMM.Close();
   g_dwConnectionState = CONNECT_STAT_STATUS;

}


void COMMCallBack(COMM_INTR_PROTOTYPE) 
{
   // Function callback lors du recoit d'un packet.
   TFCPacket *Msg = new TFCPacket; 					// Packet (ou Message)
   
   // Creation du Message a prtir du Buffer.
   Msg->SetBuffer(lpbBuffer, nBufferSize);
   
   
   if (COMM.State == 1) 
   {
      // COMM Center est en Mode Syncronious. Les packets sont recut avec une loop while(receive())
      // pour du backward compatibility. On mets donc les packets dans une liste qui donnera le packet
      // a notre function receive().
      COMM.Lock();
      COMM.AddPacket(Msg);
      short Type;
      Msg->Get((short *)&Type);
      Msg->Seek(-2/*-sizeof( short )*/, 1 );
      COMM.Unlock(); 
   } 
   else 
   {
      g_App.LockPacketThread();
      HandlePacket(Msg);// Handle Packet.
      g_App.UnlockPacketThread();
      delete Msg;// Delete Message.
   } 
};

UINT WINAPI SocketFunction(LPVOID pParam) 
{
   TFCPacket *Msg      = NULL;

   BOOL  bMsgDay       = FALSE;
   BOOL  bNotRegister  = TRUE;
   WORD  Type          = 0;
   DWORD dwLastTryTime = 0;
   DWORD dwTryNb       = 0;

   if (g_InSocket) 
      return 0;

   
   g_InSocket = TRUE;
   COMM.State = 1;

   if (g_SocketState == 1) 
   {
      TFCPacket Send;
      Send.SetPacketSeedID(99999);
      Send << (short)RQ_QueryPatchServerInfo2 ;
      SEND_PACKET(Send);
   }
   while (!bMsgDay && g_SocketState == 1) 
   {
      if (timeGetTime() - dwLastTryTime > 1000 && bNotRegister) 
      {
         dwLastTryTime = timeGetTime();
         dwTryNb++;
         if (dwTryNb >= 10) 
         {
            g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;

            for(int r=0;r<5;r++)   
               g_strMessageStatus[r][0] = 0x00;
            FormatText HelpText2;
            HelpText2.SetText(g_LocalString[430]);
            HelpText2.Format(50);
            for(int i=0;i<HelpText2.GetnbLine();i++)
            {
               if(i <5)
                  sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
            }

            g_InSocket           = FALSE;
            g_SocketThreadHandle = NULL;
            _beginthread( CloseGlobalCOMM, 0, NULL );
            return 0;
         }
      }

      Sleep(25);
      COMM.Lock();
      int Len = COMM.Receive(Msg);
      COMM.Unlock();
      if(Len > 0) 
      {		
         // Packet is OK.
         Msg->Get((short *)&Type);

         LPBYTE lpbBuffer2;
         int nBufferSize2;

         Msg->GetBuffer(lpbBuffer2, nBufferSize2);

         switch(Type) 
         {
            // Type of Packet
            case RQ_QueryPatchServerInfo2:
            {
               CoInitialize( NULL );
               Player.Version = (unsigned short)Version::GetVersion();

               char Temp[256];
               WORD wLen;
               WORD wLanguage;

               Msg->Get((long *)&g_dwVersion);

               Msg->Get((short *)&wLen);
               int i = 0;
               for (i = 0; i < wLen; i++)
                  Msg->Get((char *)&Temp[i]);
               Temp[wLen] = 0;
               strcpy_s(g_lpszIP,256, Temp);

               Msg->Get((short *)&wLen);
               for (i = 0; i < wLen; i++)
                  Msg->Get((char *)&Temp[i]);
               Temp[wLen] = 0;
               strcpy_s(g_lpszPath,256, Temp);

               Msg->Get((short *)&wLen);
               for (i = 0; i < wLen; i++)
                  Msg->Get((char *)&Temp[i]);
               Temp[wLen] = 0;
               strcpy_s(g_lpszUser,256, Temp);

               Msg->Get((short *)&wLen);
               for (i = 0; i < wLen; i++)
                  Msg->Get((char *)&Temp[i]);
               Temp[wLen] = 0;
               strcpy_s(g_lpszPassword,256, Temp);

               Msg->Get((short *)&wLanguage);

               //now on copie toujours le fichier de patch de download au root
               //en cas d update du patch...
               {

                  char szFullPath[1024];
                  char szDownloadDir[1024];
                  char szAppDir[1024];
                  char szFrom[1024];
                  char szTo[1024];
                  char szAppName[1024];

                  GetModuleFileName(NULL, szFullPath, 1024);      
                  strcpy_s(szAppDir,1024, szFullPath);
                  for(i = strlen(szAppDir)-1; i > 0; i--) 
                  {
                     if (szAppDir[i] == '\\') 
                     {
                        szAppDir[i] = NULL;
                        strcpy_s(szAppName,1024, szAppDir+i+1);
                        i = 0;
                     }
                  }
                  strcpy_s(szDownloadDir,1024, szAppDir);
                  strcat_s(szDownloadDir,1024, "\\Download");

                  strcpy_s(szFrom,1024, szDownloadDir);
                  strcat_s(szFrom,1024, "\\T4CUpdate.Exe");

                  strcpy_s(szTo,1024, szAppDir);
                  strcat_s(szTo,1024, "\\T4CUpdate.Exe");



                  //Sassure de tuer tous les process t4c... quon trouve...
                  int iCnt = 0;
                  PROCESS_INFORMATION  piInfo;
                  DWORD dwClientID;
                  HWND hClientWnd = NULL;
                  hClientWnd = ::FindWindow( NULL, "T4C Update" );
                  while(hClientWnd && iCnt < 10)
                  {  
                     GetWindowThreadProcessId( hClientWnd, &dwClientID );
                     piInfo.hProcess = OpenProcess( PROCESS_TERMINATE, FALSE, dwClientID );	
                     if( piInfo.hProcess != NULL )
                        TerminateProcess( piInfo.hProcess, 666 );

                     Sleep(100);
                     hClientWnd = ::FindWindow( NULL, "T4C Update" );
                     iCnt++;
                  }



                  //if(!hClientWnd) //pas reussi a killer le process on ne copie pas...
                  {
                     i = 0;
                     while (!CopyFile(szFrom, szTo, FALSE) && i < 10 )  //passe de 20 a 3 sec
                     {
                        Sleep(250);
                        i++;
                     }
                  }

               }

               g_SocketState = 2; // passe au step suivant...
            } break;
         };	
         LPBYTE lpbBuffer;
         int nBufferSize;
         Msg->GetBuffer(lpbBuffer, nBufferSize);
         delete Msg;
      }
   }

   COMM.Close();

   

   if ( Custom.gWebUpdateEnabled ) 
   {  
      char strCmdLine[1024];
      if(g_lpszUser[0] = 0x00)
         sprintf_s(strCmdLine,1024,"%d chk %s %s",Custom.iLanguageID,g_lpszIP,g_lpszPath);
      else
         sprintf_s(strCmdLine,1024,"%d chk %s %s %s %s",Custom.iLanguageID,g_lpszIP,g_lpszPath,g_lpszUser,g_lpszPassword);


      BOOL bPatchNeeded = FALSE;
      try
      {
         SHELLEXECUTEINFO SHInfo = {0};
         SHInfo.cbSize = sizeof (SHELLEXECUTEINFO);
         SHInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
         SHInfo.lpVerb = "open";
         SHInfo.lpFile = "T4CUpdate.exe";
         SHInfo.lpParameters = strCmdLine;
         SHInfo.nShow = SW_HIDE;

         ShellExecuteEx (&SHInfo);
         long timeout = WaitForSingleObject(SHInfo.hProcess, 25000);
         DWORD processExitCode; 
         GetExitCodeProcess(SHInfo.hProcess, &processExitCode); 
         if(processExitCode == 1)
         {
            if(g_lpszUser[0] = 0x00)
               sprintf_s(strCmdLine,1024,"%d update %s %s",Custom.iLanguageID,g_lpszIP,g_lpszPath);
            else
               sprintf_s(strCmdLine,1024,"%d update %s %s %s %s",Custom.iLanguageID,g_lpszIP,g_lpszPath,g_lpszUser,g_lpszPassword);

            //On lance la mise a jours immediatement...
            SHELLEXECUTEINFO SHInfoUpdate = {0};
            SHInfoUpdate.cbSize = sizeof (SHELLEXECUTEINFO);
            SHInfoUpdate.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
            SHInfoUpdate.lpVerb = "open";
            SHInfoUpdate.lpFile = "T4CUpdate.exe";
            SHInfoUpdate.lpParameters = strCmdLine;
            SHInfoUpdate.nShow = SW_HIDE;

            ShellExecuteEx (&SHInfoUpdate);

            g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED_QUIT;

            for(int r=0;r<5;r++)   
               g_strMessageStatus[r][0] = 0x00;
            FormatText HelpText2;
            HelpText2.SetText(g_LocalString[592]);
            HelpText2.Format(50);
            for(int i=0;i<HelpText2.GetnbLine();i++)
            {
               if(i <5)
                  sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
            }

            g_InSocket           = FALSE;
            g_SocketThreadHandle = NULL;
            _beginthread( CloseGlobalCOMM, 0, NULL );
            return 0;

         }
      }
      catch (...)
      {

      }
   }

   

   COMM.SetIPAddr(g_ConnectionIP);
   COMM.SetAddrPort(g_ConnectionPort);
   if (!COMM.Create(COMMCallBack)) 
   {
      g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;
      for(int r=0;r<5;r++)   
         g_strMessageStatus[r][0] = 0x00;
      FormatText HelpText2;
      HelpText2.SetText(g_LocalString[84]);
      HelpText2.Format(50);
      for(int i=0;i<HelpText2.GetnbLine();i++)
      {
         if(i <5)
            sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
      }
      _beginthread( CloseGlobalCOMM, 0, NULL );
      return 0;
   }

   dwLastTryTime = timeGetTime();
   if (g_SocketState == 2) 
   {
      TFCPacket Send;
      Send.SetPacketSeedID(99999);
      Send << (short)RQ_MessageOfDay ;
      SEND_PACKET(Send);
   }
   while (!bMsgDay && g_SocketState == 2) 
   {
      if (timeGetTime() - dwLastTryTime > 1000 && bNotRegister) 
      {
         dwLastTryTime = timeGetTime();
         dwTryNb++;
         if (dwTryNb >= 10) 
         {
            g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;

            for(int r=0;r<5;r++)   
               g_strMessageStatus[r][0] = 0x00;
            FormatText HelpText2;
            HelpText2.SetText(g_LocalString[430]);
            HelpText2.Format(50);
            for(int i=0;i<HelpText2.GetnbLine();i++)
            {
               if(i <5)
                  sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
            }
            
            g_InSocket           = FALSE;
            g_SocketThreadHandle = NULL;
            _beginthread( CloseGlobalCOMM, 0, NULL );
            return 0;
         }
      }
  
      Sleep(100);
      COMM.Lock();
      int Len = COMM.Receive(Msg);
      COMM.Unlock();
      if(Len > 0) 
      {		
         // Packet is OK.
         Msg->Get((short *)&Type);
         
         LPBYTE lpbBuffer2;
         int nBufferSize2;
         
         Msg->GetBuffer(lpbBuffer2, nBufferSize2);
         
         switch(Type) 
         {
            // Type of Packet
            case RQ_MessageOfDay: 
            {
               OutputDebugString(" ** Receive MOTD!\r\n");

               bMsgDay = TRUE;
               short Len;
               char Lettre;
               int  j = 0;
               
               Msg->Get((short *)&Len);
               MsgDay = new char [Len+2048];
               for (int i = 0; i < Len; i++) 
               {
                  Msg->Get((char *)&Lettre);
                  if (Lettre == 10) 
                  {
                  
                  } 
                  else if (Lettre == 13) 
                  {
                     MsgDay[j] = ' ';
                     j++;
                     MsgDay[j] = '<';
                     j++;
                     MsgDay[j] = '>';
                     j++;
                  } 
                  else 
                  {
                     MsgDay[j] = Lettre;
                     j++;
                  }
               }
               MsgDay[j] = 0;
            } break;
         };	
         LPBYTE lpbBuffer;
         int nBufferSize;
         Msg->GetBuffer(lpbBuffer, nBufferSize);
         delete Msg;
         g_SocketState = 3; // passe au step suivant...
      }
   }
   

   dwTryNb = 0;
   if(g_SocketState == 3)
   {
      TFCPacket Send;
      Send.SetPacketSeedID(99999);
      Send << (short)RQ_RegisterAccount;
      Send << (char)strlen(Player.Account);
      Send << (char *)Player.Account;
      Send << (char)strlen(Player.Password);
      Send << (char *)Player.Password;
      Send << (short)Player.Version;
      Send << (short)0; //future language info

      SEND_PACKET(Send);
   }
   
   while (bNotRegister && g_SocketState == 3) 
   {
      if (timeGetTime() - dwLastTryTime > 1000 && bNotRegister) 
      {
         dwLastTryTime = timeGetTime();
         dwTryNb++;
         if (dwTryNb >= 10) 
         {
            g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;

            for(int r=0;r<5;r++)   
               g_strMessageStatus[r][0] = 0x00;
            FormatText HelpText2;
            HelpText2.SetText(g_LocalString[83]);
            HelpText2.Format(50);
            for(int i=0;i<HelpText2.GetnbLine();i++)
            {
               if(i <5)
                  sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
            }
            
            g_InSocket           = FALSE;
            g_SocketThreadHandle = NULL;
            _beginthread( CloseGlobalCOMM, 0, NULL );
            return 0;
         }
         

         #if 0
         if (dwTryNb <= 1) 
         {
            OutputDebugString("Send Registration\r\n");
            dwTryNb++;
            
               TFCPacket Send;
               Send.SetPacketSeedID(99999);
               //Send << (short)RQ_QueryPatchServerInfo;
               Send << (short)RQ_QueryPatchServerInfo2;
               SEND_PACKET(Send);
         }
         #endif
      }
      Sleep(25);
      
      COMM.Lock();
      int Len = COMM.Receive(Msg);
      COMM.Unlock();
      if(Len > 0) 
      {		
         Msg->Get((short *)&Type);
         switch(Type) 
         {
            case RQ_RegisterAccount: 
            {
               OutputDebugString("Receive Registration\r\n");
               unsigned char Validation;
               Msg->Get((char *)&Validation);
               switch (Validation) 
               {
                  case TFCRegisterAccount_Registred: 
                  { // Account registred
                     OutputDebugString("Registering\r\n");
                     g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;
                     for(int r=0;r<5;r++)   
                        g_strMessageStatus[r][0] = 0x00;
                     FormatText HelpText2;
                     HelpText2.SetText(g_LocalString[593]);
                     HelpText2.Format(50);
                     for(int i=0;i<HelpText2.GetnbLine();i++)
                     {
                        if(i <5)
                           sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
                     }

                     short Len= 0;
                     char chTmp;
                     Msg->Get((short *)&Len);
                     int i = 0;
                     for (i = 0; i < Len; i++) 
                     {
                        Msg->Get((char *)&chTmp);
                     }

                     long lKey = 0;
                     Msg->Get((long *)&lKey);
                     Player.lKey = lKey;
                     
                     OutputDebugString("sendRQ_AuthenticateServerVersion\n");
                     TFCPacket Send;                     
                     Send << (RQ_SIZE)RQ_AuthenticateServerVersion;
                     Send << (long)Player.Version;
                     SEND_PACKET(Send);
                     dwTryNb = 1;
                  } 
                  break;
                  
                  case TFCRegisterAccount_WrongPassword: 
                  { // Account refused, wrong pwd
                     OutputDebugString("Receive Wrong Password\r\n");

                     g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;

                     short Len= 0;
                     int j=0;
                     char Lettre;
                     Msg->Get((short *)&Len);
                     char *MsgErr = new char [Len+1];
                     int i = 0;
                     for (i = 0; i < Len; i++) 
                     {
                        Msg->Get((char *)&Lettre);
                        if (Lettre == 10 || Lettre == 13) 
                           MsgErr[j] = ' ';
                        else 
                           MsgErr[j] = Lettre;

                        j++;
                     }
                     MsgErr[j] = 0;

                     for(int r=0;r<5;r++)   
                        g_strMessageStatus[r][0] = 0x00;
                     FormatText HelpText2;
                     HelpText2.SetText(MsgErr);
                     HelpText2.Format(50);
                     for(i=0;i<HelpText2.GetnbLine();i++)
                     {
                        if(i <5)
                           sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
                     }
                     
                     g_InSocket           = FALSE;
                     g_SocketThreadHandle = NULL;
                     _beginthread( CloseGlobalCOMM, 0, NULL );

                     delete []MsgErr;
                     MsgErr = NULL;
                     return 0;
                  } 
                  break;
                  
                  case 2: 
                  {
                     OutputDebugString("Authentification Error\r\n");

                     g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;

                     for(int r=0;r<5;r++)   
                        g_strMessageStatus[r][0] = 0x00;
                     FormatText HelpText2;
                     HelpText2.SetText(g_LocalString[591]);
                     HelpText2.Format(50);
                     for(int i=0;i<HelpText2.GetnbLine();i++)
                     {
                        if(i <5)
                           sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
                     }
                     
                     g_InSocket           = FALSE;
                     g_SocketThreadHandle = NULL;
                     _beginthread( CloseGlobalCOMM, 0, NULL );
                     return 0;
                  } 
                  break;
               } 
            } 
            break;
            
            
            case RQ_AuthenticateServerVersion:
            {
               OutputDebugString("Received server version authentication\r\n");
               unsigned long Valid = 0;
               Msg->Get((long*)&Valid);
               
               if( Valid == 1 )
               {
                  g_SocketState = 4;
               }
               else
               {
                  bNotRegister = FALSE;
                  OutputDebugString("Receive Wrong Version\r\n");
                  
                  g_dwConnectionState = CONNECT_STAT_STATUS_NOFEED;
                  
                  for(int r=0;r<5;r++)   
                     g_strMessageStatus[r][0] = 0x00;
                  FormatText HelpText2;
                  HelpText2.SetText(g_LocalString[542]);
                  HelpText2.Format(50);
                  for(int i=0;i<HelpText2.GetnbLine();i++)
                  {
                     if(i <5)
                        sprintf_s(g_strMessageStatus[i],512,"%s",HelpText2.GetText(i));
                  }
                  
                  g_InSocket           = FALSE;
                  g_SocketThreadHandle = NULL;
                  _beginthread( CloseGlobalCOMM, 0, NULL );
                  return 0;
                  
               }
            } 
            break;
            default: 
            {
               char Temp[100];
			      sprintf_s(Temp,100,"Unknow packet type [%d]\r\n",Type);
               OutputDebugString(Temp);
            } 
            break;
            case TFCStillConnected: 
            {
               TFCPacket Send;
               
               Send << (short)10;
               
               SEND_PACKET(Send);
            } 
            break;
            
         };	
         
         LPBYTE lpbBuffer;
         int nBufferSize;
         Msg->GetBuffer(lpbBuffer, nBufferSize);
         delete Msg;
      }
   };

   g_dwConnectionState = CONNECT_STAT_STATUS_ENTERGAME;

   bNotRegister = TRUE;
   g_InSocket   = FALSE;
   g_SocketThreadHandle = NULL;
   return 1;
}


void ProcessThreadDraw(bool bIncreaseObj)
{
   string lastText;
   string cachedText;
   MouseDevice Mouse2;

  
  
   MousePos &mp = RootBoxUI::GetInstance()->GetRootScreenOffset();
   bool isMaximized = RootBoxUI::GetInstance()->IsGUIMaximized();
   
   
   //Update Skin and main View...
   World.Lock(119);//NM: ProcessThreadDraw
   Objects.Lock(1010);
   Objects.IsNeedChangeSkin();
   World.UpdateView(Draw, false, mp.x, mp.y,bIncreaseObj); // Draw2
   Objects.Unlock(1010);
   World.Unlock(119);
   

      
   if (!Custom.gDebugGUIHidden) 
   {
      //Draw main GUI

      RootBoxUI::GetInstance()->Draw( NULL );
      
     
      if(!V3_ChatLogDlg::GetInstance()->IsShown()&& g_dwTextOnFullScreenCnt >16)
      {
         // draw tous les texte....
         tlSystemMessage.Lock("Draw");
         if (tlSystemMessage.GetNbObjects()/* && !mp.y*/) 
         {
            tlSystemMessage.ToTail();
            int Row = 0;
            int dwCnt = 0;

            int ixPos = V3_MainBarDlg::GetInstance()->GetPos().x+110;

            int iYOffset = 72;
            if(g_SaveGame.GetOptionsParam()->bNotDisplayMacro)
               iYOffset-=47;

            while(tlSystemMessage.QueryPrevious() && dwCnt < 5) 
            {
               SysMsg *Msg = tlSystemMessage.GetObject();
               

               Msg->Draw( ixPos, g_Global.GetScreenH()-iYOffset - Row - Msg->GetHeight() );
               
               Row += Msg->GetHeight();
               if (timeGetTime() > Msg->GetTime() && timeGetTime() - Msg->GetTime() > 4000) 
               {
                  delete Msg;
                  tlSystemMessage.RemoveObject();
               }
               dwCnt++;
            };
         }
         tlSystemMessage.Unlock("Draw"); 
         
      }
      else if(!V3_ChatLogDlg::GetInstance()->IsShown())
      {
         g_dwTextOnFullScreenCnt++;
      } 
      else
         g_dwTextOnFullScreenCnt = 0;
         
      
      // If help is visible we draw it over all other GUI...
      RootBoxUI::GetInstance()->DrawHelp( NULL );
      g_SaveGame.SetRTMapVal(Player.xPos,Player.yPos,Player.World);
   } 


   //If !FPS is enable we show the current drawing thread info
   if(Custom.gShowFPS)
   {
      HDC hdc;
      DXDGetDC(&hdc, 23); //Show FPS
      char Buf[100];
      
      //Drawing FPS
      sprintf_s(Buf,100, "Drawing at %d fps",g_iDrawingFPSLast); 
      fT4CDef_22->DrawFont(hdc, 60, 30, CL_RED, 0, Buf);
      //DrawingThread FPS
      //sprintf_s(Buf,100, "Drawing Thread at %d fps",g_iDrawPFPSLast); 
      //fT4CDef_22->DrawFont(hdc, 60, 45, CL_RED, 0, Buf);

      //DrawingThread FPS
      sprintf_s(Buf,100, "Drawing take %d ms",g_uiDrawPTimeLast); 
      fT4CDef_22->DrawFont(hdc, 60, 45, CL_RED, 0, Buf);
      DXDReleaseDC(hdc, 23);
   }

   V2SPRITEFX VsfFX;
   ZeroMemory(&VsfFX, sizeof(V2SPRITEFX));
   VsfFX.lpClipRect = new RECT;
   VsfFX.lpClipRect->left = 0;
   VsfFX.lpClipRect->top = 0;
   VsfFX.lpClipRect->right = g_Global.GetScreenW();
   VsfFX.lpClipRect->bottom = g_Global.GetScreenH();

   // if need to display LOADING ICONS...
   if(g_Var.wDisplayDisk >0)
   {
      g_Var.vsfDisk.DrawSpriteN(g_Global.GetScreenW()-g_Var.vsfDisk.GetWidth(),g_Global.GetScreenH()-g_Var.vsfDisk.GetHeight(), &VsfFX);
      
      if(++g_Var.wDisplayDisk >5)
         g_Var.wDisplayDisk = 0;
   }
      
   //if need to display MAP (loading world)
   if(g_Var.wDisplayMap >0)
   {
      g_Var.vsfMap.DrawSpriteN(g_Global.GetScreenW()-g_Var.vsfMap.GetWidth(),g_Global.GetScreenH()-g_Var.vsfMap.GetHeight(), &VsfFX);
      if(++g_Var.wDisplayMap >5)
         g_Var.wDisplayMap = 0;
   }

  
      
   //if need to diaplsy NETWORK busy icons
   if (!COMM.isHalf()) 
   {
      g_Var.vsfNetwork.DrawSpriteN(g_Global.GetScreenW()-g_Var.vsfNetwork.GetWidth(),g_Global.GetScreenH()-g_Var.vsfNetwork.GetHeight(), &VsfFX);
   }
      
   //if we have a name on this positions...
   //we setup the display name show...
   if(g_Global.GetDisplayZone() >=0)
   {
      g_Var.wDisplayMapZone = 1;
      sprintf_s(g_Var.strMapZoneName,128,"%s",g_Global.GetDisplayZoneName());

      g_Var.vsfMapZoneN   .Release();
      g_Var.vsfMapZoneN  .CreateSprite("V3_ZoneDisplayN");

      HDC hdc;
      g_Var.vsfMapZoneN.m_lpV2SpriteInfo->Data.lpDDSurface->GetDC(&hdc);
      int iOffset = (324-(fT4CDef_30B->GetFLen(hdc,g_Var.strMapZoneName)))/2;
      fT4CDef_30B->DrawFont(hdc, iOffset+ 2, 25+2 , CL_BLACK , 0, g_Var.strMapZoneName);
      fT4CDef_30B->DrawFont(hdc, iOffset   , 25   , g_DefColorD  , 0, g_Var.strMapZoneName);
      g_Var.vsfMapZoneN.m_lpV2SpriteInfo->Data.lpDDSurface->ReleaseDC(hdc);
      g_chAlphaZone = 0;
   }
   
   //if display zone setupped, we display this zone with fadein and fade out...
   if(g_Var.wDisplayMapZone >0)
   {
      V2SPRITEFX VsfFX2;
      ZeroMemory(&VsfFX2, sizeof(V2SPRITEFX));
      VsfFX2.lpClipRect = new RECT;
      VsfFX2.lpClipRect->left = (g_Global.GetScreenW()-g_Var.vsfMapZoneN.GetWidth())/2;
      VsfFX2.lpClipRect->top  = 100;
      VsfFX2.lpClipRect->right = VsfFX2.lpClipRect->left+g_Var.vsfMapZoneN.GetWidth();
      VsfFX2.lpClipRect->bottom = VsfFX2.lpClipRect->top+g_Var.vsfMapZoneN.GetHeight();
      if(g_Var.wDisplayMapZone >0 && g_Var.wDisplayMapZone < 30)
      {
         g_chAlphaZone+=20;
         if(g_chAlphaZone >= 200)
            g_chAlphaZone = 200;
         g_Var.vsfMapZoneN.DrawSpriteNSemiTrans(VsfFX2.lpClipRect->left,VsfFX2.lpClipRect->top, &VsfFX2,TRUE,g_chAlphaZone);
      }
      else if(g_Var.wDisplayMapZone >29 && g_Var.wDisplayMapZone < 70)
      {
         g_Var.vsfMapZoneN.DrawSpriteNSemiTrans(VsfFX2.lpClipRect->left,VsfFX2.lpClipRect->top, &VsfFX2,TRUE,g_chAlphaZone);
      }
      else
      {
         g_chAlphaZone-=20;
         if(g_chAlphaZone < 20)
            g_chAlphaZone = 0;
         g_Var.vsfMapZoneN.DrawSpriteNSemiTrans(VsfFX2.lpClipRect->left,VsfFX2.lpClipRect->top, &VsfFX2,TRUE,g_chAlphaZone);
      }
      delete VsfFX2.lpClipRect;

      if(++g_Var.wDisplayMapZone >100)
         g_Var.wDisplayMapZone = 0;
   } 


   if(g_Var.dw1stAprilCnt >210)
   {
      int iStepSize = g_Global.GetScreenW()/10;
      g_Var.vsfFish.DrawSpriteN(g_Var.iFishXPos,g_Var.iFishYPos,&VsfFX);
      g_Var.dw1stAprilCnt++;
      
      g_Var.iFishXPos+=2;
      if(!g_SaveGame.GetOptionsParam()->bPlayAt32FPS)
         g_Var.iFishXPos+=2;

      if(g_Var.iFishXPos >= (iStepSize*2) && g_Var.iFishXPos <=(iStepSize*3))
      {
         if(!g_Var.bDisplayedMessage)
         {
            V3_ChatLogDlg::GetInstance()->AddListLogs("", g_Var.strFishMessage, CL_ORANGE, true);
            g_Var.bDisplayedMessage = true;
         }
         g_Var.iFishYPos++;
         if(!g_SaveGame.GetOptionsParam()->bPlayAt32FPS)
            g_Var.iFishYPos++;
      }
   
      if(g_Var.iFishXPos >= (iStepSize*2) && g_Var.iFishXPos <=(iStepSize*8))
      {
         HDC hdc;
         DXDGetDC(&hdc, 26); //level UP

         int s = g_Global.GetScreenWDiv2();
         int yPos = (g_Global.GetScreenH()/3);
         s -= (fT4CDef_50B->GetFLen(hdc, g_LocalString[744])/2);

         fT4CDef_50B->DrawFont(hdc, s-1, yPos-1, 0, 0, g_LocalString[ 744 ] );
         fT4CDef_50B->DrawFont(hdc, s+1, yPos+1, 0, 0, g_LocalString[ 744 ] );
         fT4CDef_50B->DrawFont(hdc, s, yPos, CL_WHITE, 0, g_LocalString[ 744 ] );

         DXDReleaseDC(hdc, 26);
      }
      



      if(g_Var.iFishXPos >= (iStepSize*6) && g_Var.iFishXPos <=(iStepSize*7))
         g_Var.iFishYPos--;

      if(g_Var.iFishXPos > g_Global.GetScreenW())
         g_Var.dw1stAprilCnt = 0;


   }



   if(g_Var.dw1stAprilCnt >0)
   {
      int iXOffset = g_Global.GetScreenW()/4;
      g_Var.vsfBubble[0].DrawSpriteN(iXOffset-(g_Var.vsfBubble[0].GetWidth()/2),g_Global.GetScreenH()-g_Var.vsfBubble[0].GetHeight(), &VsfFX,&g_Var.vsfBubble[g_Var.wBubbleAnimCnt01]);
      g_Var.wBubbleAnimCnt01R++;
      if(g_SaveGame.GetOptionsParam()->bPlayAt32FPS && g_Var.wBubbleAnimCnt01R >= 6)
      {
         g_Var.wBubbleAnimCnt01R = 0;
         g_Var.wBubbleAnimCnt01++;
      }
      else if( g_Var.wBubbleAnimCnt01R >= 2)
      {
         g_Var.wBubbleAnimCnt01R = 0;
         g_Var.wBubbleAnimCnt01++;
      }
      if(g_Var.wBubbleAnimCnt01>=18)
         g_Var.wBubbleAnimCnt01 = 1;
      if(g_Var.dw1stAprilCnt <=70)
         g_Var.dw1stAprilCnt++;
   }

   if(g_Var.dw1stAprilCnt >70)
   {
      int iXOffset = g_Global.GetScreenW()/4;
      g_Var.vsfBubble[0].DrawSpriteN((iXOffset*2)-(g_Var.vsfBubble[0].GetWidth()/2),g_Global.GetScreenH()-g_Var.vsfBubble[0].GetHeight(), &VsfFX,&g_Var.vsfBubble[g_Var.wBubbleAnimCnt02]);
      g_Var.wBubbleAnimCnt02R++;
      if(g_SaveGame.GetOptionsParam()->bPlayAt32FPS && g_Var.wBubbleAnimCnt02R >= 6)
      {
         g_Var.wBubbleAnimCnt02R = 0;
         g_Var.wBubbleAnimCnt02++;
      }
      else if( g_Var.wBubbleAnimCnt02R >= 2)
      {
         g_Var.wBubbleAnimCnt02R = 0;
         g_Var.wBubbleAnimCnt02++;
      }
      if(g_Var.wBubbleAnimCnt02>=18)
         g_Var.wBubbleAnimCnt02 = 1;
      if(g_Var.dw1stAprilCnt <=140)
         g_Var.dw1stAprilCnt++;
   }

   if(g_Var.dw1stAprilCnt >140)
   {
      int iXOffset = g_Global.GetScreenW()/4;
      g_Var.vsfBubble[0].DrawSpriteN((iXOffset*3)-(g_Var.vsfBubble[0].GetWidth()/2),g_Global.GetScreenH()-g_Var.vsfBubble[0].GetHeight(), &VsfFX,&g_Var.vsfBubble[g_Var.wBubbleAnimCnt03]);
      g_Var.wBubbleAnimCnt03R++;
      if(g_SaveGame.GetOptionsParam()->bPlayAt32FPS && g_Var.wBubbleAnimCnt03R >= 6)
      {
         g_Var.wBubbleAnimCnt03R = 0;
         g_Var.wBubbleAnimCnt03++;
      }
      else if( g_Var.wBubbleAnimCnt03R >= 2)
      {
         g_Var.wBubbleAnimCnt03R = 0;
         g_Var.wBubbleAnimCnt03++;
      }
      if(g_Var.wBubbleAnimCnt03>=18)
         g_Var.wBubbleAnimCnt03 = 1;
      if(g_Var.dw1stAprilCnt <=210)
         g_Var.dw1stAprilCnt++;
   }

   

   delete VsfFX.lpClipRect;
    
    
      
   // Look if we need to capture this frame to make a good video...
   g_NMVideoCapture.CaptureFrame();

   //look if need to take a screenshot...
   if(Custom.gTakeScreenShot)
   {
      VideoCapture *pCap = new VideoCapture;
      pCap->TakeDesktopSnapshot();
      ClientConfig *pConfig = g_Global.GetClientConfig();
      char strScreenShotPath[1024];
      sprintf_s(strScreenShotPath,1024,"%s\\ScreenShot",pConfig->strSavePath);
      pCap->SaveToFile(strScreenShotPath);
      delete pCap;
      Custom.gTakeScreenShot = FALSE;
   }

  
   //At the end, draw mouse cursor...
   Mouse2.GetPosition(&x, &y);
   CMouseCursor::GetInstance()->DrawCursor(x, y); // 1   
   x = (x+48-16)/32;
   y = (y-8)/16;
   

   //Flip DX screen to put back buffer in the front buffer
   DXDFlip(); //***************************real DRAW
}


void ProcessEventDraw()
{
   

   if (NeedRedraw && !DrawLastMoving) 
   {
      
      DrawLastMoving    = g_DONE;
      World.Lock(115);//NM: ProcessEventDraw
      Objects.Lock(1009);
      Objects.GetMainObject()->Direction = Objects.Direction2;
      Objects.MovePlayer(World.MovX, World.MovY);
      World.Switch();
      Objects.Unlock(1009);
      World.Unlock(115);


      Draw = true;

      if (Follow) 
      {
         signed char xDif = 0;                 
         signed char yDif = 0;
         Objects.Lock(117);
         Objects.Found(&xDif, &yDif, FollowID);
         Objects.Unlock(117);

         pfSetPosition(xDif+Player.xPos, yDif+Player.yPos);
         if (Attack) 
         {
            XAttack = xDif+Player.xPos;
            YAttack = yDif+Player.yPos;
         }
         pfNearPosition();
         if (Use) 
         {
            pfNearPosition();
         }
      }

      if (!pfIsAtPosition()) 
      {
         bCanMovePL = false;
         Try = timeGetTime();
         TFCPacket Send;

         long NewID = 0;
         short Menrage = (short)pfGetNextMovement();
         Objects.Lock(118);
         switch (Menrage) 
         {
            case 1: NewID = Objects.FoundID2( 0, -1);  break;
            case 2: NewID = Objects.FoundID2( 1, -1);  break;
            case 3: NewID = Objects.FoundID2( 1,  0);  break;
            case 4: NewID = Objects.FoundID2( 1,  1);  break;
            case 5: NewID = Objects.FoundID2( 0,  1);  break;
            case 6: NewID = Objects.FoundID2( -1,  1); break;
            case 7: NewID = Objects.FoundID2( -1,  0); break;
            case 8: NewID = Objects.FoundID2( -1, -1); break;
         }; 
         Objects.Unlock(118);

         if (Menrage && (!Attack || !NewID)) 
         {
            Send << Menrage;

            SEND_PACKET(Send);
         }
      } 
      else if (Attack) 
      {
         pfStopMovement("Move 4");
         Err = false;
         Follow = false;

         TFCPacket Send; 

         Send << (short)24;
         Send << (short)XAttack;
         Send << (short)YAttack;
         Send << (long)FollowID;

         SEND_PACKET(Send);
         Attack = false;
      } 
      else if (Use) 
      {
         Err = false;
         Follow = false;

         TFCPacket Send; 

         Send << (short)23;
         Send << (short)XUse;
         Send << (short)YUse;
         Send << (long)FollowID;

         SEND_PACKET(Send);
         Use = false;
      } 
      else if (Get) 
      {
         Err = false;
         Follow = false;

         TFCPacket Send; 

         Send << (short)11;
         Send << (short)XGet;
         Send << (short)YGet;
         Send << (long)FollowID;

         SEND_PACKET(Send);
         Get = false;
      }

      if (Retrace) 
      {
         Retrace = false;
         bCanMovePL = false;
         Try = timeGetTime();

         TFCPacket Send; 
         TFCPacket Send2;

         Send << (short)RQ_GetPlayerPos;
         Send2 << (short)16;
         Send2 << (char)Objects.Direction2;
         Send2 << (short)Player.xPos;
         Send2 << (short)Player.yPos;

         SEND_PACKET(Send);
         SEND_PACKET(Send2);
      }
      NeedRedraw = false;
   }
}



void  FPS_Calculator(LPVOID pParam)
{

   bool bRunning = true;

   while(bRunning)
   {
      int iRet = WaitForSingleObject(g_hExitFPSCalc,1000);
      switch(iRet)
      {
         case WAIT_OBJECT_0 + 0 :
            bRunning = false;
         break;
         case WAIT_TIMEOUT:
            if(g_iDrawingFPSCnt >0)
            {
               g_uiDrawPTimeLast = g_uiDrawPTimeCnt/g_iDrawingFPSCnt;
               g_uiDrawPTimeCnt  = 0;
               g_iDrawingFPSLast = g_iDrawingFPSCnt;
               g_iDrawingFPSCnt  = 0;
            }
         break;
      }
   }

}




void TFCSocket::MenuThread(void) 
{
   //Mouse amnagement
   MouseDevice Mouse2;
   if (!KeyBoard.IsCreated()) 
   {
      KeyBoard.Create();
   }
   KeyBoard.CreateDevice(GUID_SysKeyboard);
   KeyBoard.Acquire();

   //creation fenetre directX pour le menu...
   DDSURFACEDESC2 ddSurfaceDesc;
   ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC2));
   ddSurfaceDesc.dwSize            = sizeof(ddSurfaceDesc);
   ddSurfaceDesc.dwFlags           = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
   ddSurfaceDesc.ddsCaps.dwCaps    = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
   ddSurfaceDesc.dwHeight          = 40;
   ddSurfaceDesc.dwWidth           = 47;
   DXDErrorHandle(lpDXDirectDraw->CreateSurfaceNM(&ddSurfaceDesc, &lpSafe, NULL), "Add Mouse Cursor 543");

   V2SPRITEFX vsffx;
   ZeroMemory(&vsffx, sizeof(V2SPRITEFX));
   RECT rect;
   rect.left = 0;
   rect.top = 0;
   rect.right = g_Global.GetScreenW();
   rect.bottom = g_Global.GetScreenH();
   vsffx.lpClipRect = &rect;
   vsffx.dwFX |= FX_FIT2SCREEN;
   
   V2SPRITEFX vsffx2;
   ZeroMemory(&vsffx2, sizeof(V2SPRITEFX));
   vsffx2.lpClipRect = &rect;


   //uinitialisation de la structure des players a 0
   int i=0;
   for(i=0;50<i;i++)
   {
      ZeroMemory(Custom.m_strName[i], 100);
      Custom.m_shRace[i]      = 0;
      Custom.m_shLevel[i]     = 0;
      Custom.m_shHairColor[i] = 0;
      for(int ee=0;ee<17;ee++)
      {
         Custom.m_wEquipList[i][ee]=0;
      }
   }

   //Initialisation des texts par default...
   FormatText Question[8];
   FormatText Reponse[8][5];
   
   for (i = 0; i < 8; i++) 
   {
      Question[i].SetText(g_LocalString[106+i*6]);
      Question[i].Format(329, fSmallNormal_13);
      Reponse[i][0].SetText(g_LocalString[107+i*6]);
      Reponse[i][1].SetText(g_LocalString[108+i*6]);
      Reponse[i][2].SetText(g_LocalString[109+i*6]);
      Reponse[i][3].SetText(g_LocalString[110+i*6]);
      Reponse[i][4].SetText(g_LocalString[111+i*6]);
      Reponse[i][0].Format(329, fSmallNormal_13);
      Reponse[i][1].Format(329, fSmallNormal_13);
      Reponse[i][2].Format(329, fSmallNormal_13);
      Reponse[i][3].Format(329, fSmallNormal_13);
      Reponse[i][4].Format(329, fSmallNormal_13);
   }

   FormatText Introduction; 
   Introduction.SetText(g_LocalString[45]);
   Introduction.SetText("<>");
   Introduction.SetText("<>");
   Introduction.SetText(g_LocalString[46]);
   Introduction.SetText("<>");
   Introduction.SetText("<>");
   Introduction.SetText(g_LocalString[47]);
   Introduction.SetText("<>");
   Introduction.SetText("<>");
   Introduction.SetText(g_LocalString[92]);
   Introduction.SetText("<>");
   Introduction.SetText("<>");
   Introduction.SetText(g_LocalString[93]);
   Introduction.SetText("<>");
   Introduction.SetText("<>"); 
   Introduction.Format(520, fT4CDef_22);


   FormatText Credits;
   char strVersion[50];
   sprintf_s(strVersion,50,"%.02f %s",Version::GetVersion()/1000.00,Version::GetSubVersion());
   Credits.SetText(g_LocalString[9]);
   Credits.SetText("<>");
   Credits.SetText(strVersion);  //name and version
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(">==T4CDEV Project Team==<");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(">--1.7x Coordination Unit--<");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("Marc Frega, Dialsoft Founder & CEO");
   Credits.SetText("<>");
   Credits.SetText("Carl [Nightmare] Vachon, Project Leader");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(">--1.7 Communication Unit--<");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("Maxime [Nekhar] Guillemot");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(">--1.7x Coding Unit--<");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("Carl [Nightmare] Vachon");
   Credits.SetText("<>");
   Credits.SetText("Quentin [Moen] Mansuy");
   Credits.SetText("<>");
   Credits.SetText("Martin [Marae] Dubé");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(">--1.7 Concept and Test--<");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("Carl [Nightmare] Vachon");
   Credits.SetText("<>");
   Credits.SetText("Maxime [Nekhar] Guillemot");
   Credits.SetText("<>");
   Credits.SetText("Quentin [Moen] Mansuy");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(">--1.7x Graphics Unit--<");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("Jonathan [Heschal] Pelletier");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(">--1.7x World Design Unit--<");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("Nadine [Lune] Pelletier");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(">--1.7x Music & Sound Unit--<");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(" [BoX] ");
   Credits.SetText("<>");
   Credits.SetText("<>"); 
   Credits.SetText("<>");
   Credits.SetText(">--1.7x Special Thanks--<");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText(" James [Onyx] Allee for English Translations");
   Credits.SetText("<>");
   Credits.SetText("<>"); 
   Credits.SetText("<>");
   Credits.SetText("-----------------------------------------------------------------------------");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.SetText("<>");
   Credits.Format(520, fT4CDef_22);

   FormatText MsgFormatText;
   MsgFormatText.SetText(MsgDay);
   MsgFormatText.Format(520, fT4CDef_22);
   bool bCanHideMsgDay = false;

   FormatText MsgCreate01;
   MsgCreate01.SetText(g_LocalString[695]);
   MsgCreate01.Format(246, fSmallNormal_12);

   FormatText MsgCreate02;
   MsgCreate02.SetText(g_LocalString[705]);
   MsgCreate02.Format(246, fSmallNormal_12);

   FormatText MsgCreate03;
   MsgCreate03.SetText(g_LocalString[706]);
   MsgCreate03.Format(246, fSmallNormal_12);

   FormatText MsgCreate04;
   MsgCreate04.SetText(g_LocalString[707]);
   MsgCreate04.Format(246, fSmallNormal_12);



   //def des image utiliser pour le menu...
   CV2Sprite    ConnectBack;
   CV2Sprite    ConnectBackFrame[2];
   CV2Sprite    ConnectBackFrameErr[2];  
   CV2Sprite    ConnectLogoDial[2];
   CV2Sprite    ConnectLogoDev[2];
   CV2Sprite    ConnectLogoT4cS[2];
   CV2Sprite    ConnectLogoT4cB[2];
   CV2Sprite    ConnectIntroBack[2];
   CV2Sprite    ConnectNext[3];
   CV2Sprite    ConnectPrev[3];
   CV2Sprite    ConnectFrameBtn[2];
   CV2Sprite    ConnectSelectPlayer[2];
   CV2Sprite    ConnectPodiumGlow[2];
   CV2Sprite    ConnectBtn80[3];
   CV2Sprite    ConnectBtn92[3];
   CV2Sprite    ConnectCreateBack01[2];
   CV2Sprite    ConnectCreateBack02[2];
   CV2Sprite    ConnectCreateBack03[2];
   CV2Sprite    ConnectCreateBack04[2];
   CV2Sprite    ConnectCreateBack05[2];
   CV2Sprite    ConnectBtnMen[3];
   CV2Sprite    ConnectBtnWomen[3];  
   CV2Sprite    QuestionBack[3];  
   CV2Sprite    StatsIcons[9];  
   CV2Sprite    StatDee[3];  
   


   char strTmp[512];
   sprintf_s(strTmp,512,"%d%d",g_Global.GetDisplayW(),g_Global.GetDisplayH());
   ConnectBack.LoadSprite(strTmp);

   ConnectBackFrame[0].LoadSprite("V3_ConnectBack");
   ConnectBackFrame[1].LoadSprite("V3_ConnectBackMask");

   ConnectBackFrameErr[0].LoadSprite("V3_ConnectBackErr");
   ConnectBackFrameErr[1].LoadSprite("V3_ConnectBackErrMask");
   
   ConnectLogoDial[0].LoadSprite("V3_DialLog");
   ConnectLogoDial[1].LoadSprite("V3_DialLogMask");
   
   ConnectLogoDev[0].LoadSprite("V3_t4cDevLogo");
   ConnectLogoDev[1].LoadSprite("V3_t4cDevLogoMask");
   
   ConnectLogoT4cS[0].LoadSprite("V3_T4CSmall");
   ConnectLogoT4cS[1].LoadSprite("V3_T4CSmallMask");
   
   ConnectLogoT4cB[0].LoadSprite("V3_T4CBig");
   ConnectLogoT4cB[1].LoadSprite("V3_T4CBigMask");

   ConnectIntroBack[0].LoadSprite("V3_IntroBack");
   ConnectIntroBack[1].LoadSprite("V3_IntroBackMask");

   ConnectNext[0].LoadSprite("V3_NextArrow_N");
   ConnectNext[1].LoadSprite("V3_NextArrow_H");
   ConnectNext[2].LoadSprite("V3_NextArrow_NMask");

   ConnectPrev[0].LoadSprite("V3_PrevArrow_N");
   ConnectPrev[1].LoadSprite("V3_PrevArrow_H");
   ConnectPrev[2].LoadSprite("V3_PrevArrow_NMask");

   ConnectFrameBtn[0].LoadSprite("V3_Btn160x24_N");
   ConnectFrameBtn[1].LoadSprite("V3_Btn160x24_H");

   sprintf_s(strTmp,"V3_SelectPlayerBack");
   ConnectSelectPlayer[0].LoadSprite("V3_SelectPlayerBack");
   sprintf_s(strTmp,"V3_SelectPlayerBackMask");
   ConnectSelectPlayer[1].LoadSprite("V3_SelectPlayerBackMask");

   ConnectPodiumGlow[0].LoadSprite("V3_PodiumGlow");
   ConnectPodiumGlow[1].LoadSprite("V3_PodiumGlowMask");

   ConnectBtn80[0].LoadSprite("V3_GUIBtn80_N");
   ConnectBtn80[1].LoadSprite("V3_GUIBtn80_H");
   ConnectBtn80[2].LoadSprite("V3_GUIBtn80_NMask");

   ConnectBtn92[0].LoadSprite("V3_GUIBtn80_N");
   ConnectBtn92[1].LoadSprite("V3_GUIBtn80_H");
   ConnectBtn92[2].LoadSprite("V3_GUIBtn80_NMask");

   ConnectCreateBack01[0].LoadSprite("V3_Create01");
   ConnectCreateBack01[1].LoadSprite("V3_Create01Mask");

   ConnectCreateBack02[0].LoadSprite("V3_Create02");
   ConnectCreateBack02[1].LoadSprite("V3_Create02Mask");

   ConnectCreateBack03[0].LoadSprite("V3_Create03");
   ConnectCreateBack03[1].LoadSprite("V3_Create03Mask");

   ConnectCreateBack04[0].LoadSprite("V3_Create04");
   ConnectCreateBack04[1].LoadSprite("V3_Create04Mask");

   ConnectCreateBack05[0].LoadSprite("V3_Create05");
   ConnectCreateBack05[1].LoadSprite("V3_Create05Mask");

   ConnectBtnMen[0].LoadSprite("V3_MenBtn_N");
   ConnectBtnMen[1].LoadSprite("V3_MenBtn_H");
   ConnectBtnMen[2].LoadSprite("V3_MenBtn_NMask");

   ConnectBtnWomen[0].LoadSprite("V3_WomenBtn_N");
   ConnectBtnWomen[1].LoadSprite("V3_WomenBtn_H");
   ConnectBtnWomen[2].LoadSprite("V3_WomenBtn_NMask");

   QuestionBack[0].LoadSprite("V3_QuestionN");
   QuestionBack[1].LoadSprite("V3_QuestionH");
   QuestionBack[2].LoadSprite("V3_QuestionNMask");

   StatsIcons[0].LoadSprite("V3_ConnectStr");
   StatsIcons[1].LoadSprite("V3_ConnectEnd");
   StatsIcons[2].LoadSprite("V3_ConnectAgi");
   StatsIcons[3].LoadSprite("V3_ConnectInt");
   StatsIcons[4].LoadSprite("V3_ConnectWis");
   StatsIcons[5].LoadSprite("V3_ConnectLck");
   StatsIcons[6].LoadSprite("V3_ConnectHP");
   StatsIcons[7].LoadSprite("V3_ConnectMp");
   StatsIcons[8].LoadSprite("V3_ConnectMask");

   StatDee[0].LoadSprite("V3_DeeBtn_N");
   StatDee[1].LoadSprite("V3_DeeBtn_H");
   StatDee[2].LoadSprite("V3_DeeBtn_NMask");


   char strErrRaisonL1[500];
   char strErrRaisonL2[500];
   

   unsigned char VSKey[15];

   VSKey[0] = ' ';
   VSKey[1] = 'ä';
   VSKey[2] = 'ï';
   VSKey[3] = 'ë';
   VSKey[4] = 'ö';
   VSKey[5] = 'ü';
   VSKey[6] = 'é';
   VSKey[7] = 'è';
   VSKey[8] = 'à';
   VSKey[9] = 'ù';
   VSKey[10] = 'â';
   VSKey[11] = 'î';
   VSKey[12] = 'ê';
   VSKey[13] = 'ô';
   VSKey[14] = 'û';


  
   


   InitSound();



   //declaration de toutes les variable temporaire...

   int iScrollYPos  = -1;
   int NM_FPVisible = 0;
   int NM_MPVisible = 11;
   int Selected     = 0;
   char NbOfPlayer  = 0;

  
   unsigned short  ushPacketID;
   TFCPacket      *Msg;
   int N = 0,x = 0, y = 0, button[4];
   ZeroMemory(button, 4*sizeof(int));
   int Chose = 0;
   MOUSEINUSE = true;
   int Frame = 0;
   Force = 9+NBCURSOR;
   DWORD WaitTime;
   BYTE QuestionChoice = 1;
   Register = true;
   
   BYTE nameIsNotOk = 0;
   DWORD dwLastUse = timeGetTime();

   //sert a faire tourner le PJ dans la selection perso...
   int dwCnt     = 0;
   int dwDir     = 0;
   int dwRealDir = 2;
   int dwOffset  = 0;
   int dwIncr    = 1;
  
  
   WaitTime = timeGetTime();
   int  iCntBlinkCursor = 0;
   bool bShowCursor = false;
   while (!MenuThreadFinished && !g_boQuitApp) 
   {
      //Gestion des blinking curseur au besoin...
      ++iCntBlinkCursor;
      if(iCntBlinkCursor > 10)
      {
         iCntBlinkCursor = 0;
         bShowCursor = !bShowCursor;
      }


      g_App.LockMainThread();
      Sleep(25);
      
      // While Main Thread
      if (timeGetTime() - WaitTime > 2000) 
      {
         WaitTime = timeGetTime();
         
         if (timeGetTime() - dwLastUse > 90000) 
         {
            TFC_State = TFC_WARNING_2;
            strcpy_s(strErrRaisonL1,500,  g_LocalString[ 497 ] ); //erreur de connection
            strcpy_s(strErrRaisonL2,500,  g_LocalString[ 498 ] ); //connexion perdu...
         }
      }

      if (TFC_State > TFC_FIRST_MENU_AFTER_LOGIN ) 
      {
         DXDClear();
         ConnectBack.DrawSpriteN(0, 0, &vsffx2);


      }

      // reagrde l<etat de la souris...
    
      // DRAW the screen for good state...
      
      switch (TFC_State) 
      {
         case TFC_MSGDAY://OKOKOK
         case TFC_INTRODUCTION: 
         case TFC_VIEW_CREDITS:
         {
            //dessine mack de Intro et msg of the day...
            int yFramePos = (g_Global.GetScreenH()-ConnectIntroBack[0].GetHeight())/2;
            ConnectIntroBack[0].DrawSpriteN((g_Global.GetScreenW()-ConnectIntroBack[0].GetWidth())/2,yFramePos, &vsffx2,&ConnectIntroBack[1]);

            //Fleche back

            int x = 0;
            int y = 0;
            Mouse2.GetPosition(&x, &y);
            
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();

            if(x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 )
               ConnectPrev[1].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);


            int iLastY = 0;
            if(iScrollYPos == -1) // premier coup on initialise le Y Start...
               iScrollYPos = (g_Global.GetScreenH()*2)-((g_Global.GetScreenH()- (yFramePos+ConnectIntroBack[0].GetHeight()-40))*2);
            else
               iScrollYPos -=2;
            
            HDC hdc;
            DXDGetDCConnect(&hdc, 10);

            int iTitleID = 0;
            if(TFC_State == TFC_MSGDAY) 
            {
               iTitleID = 687;
               for (int i = 0; i < MsgFormatText.GetnbLine(); i++) 
               {
                  iLastY = (iScrollYPos/2)+i*fT4CDef_22->GetFHeight(hdc);
                  if(iLastY < yFramePos+ConnectIntroBack[0].GetHeight()-30 && iLastY >  yFramePos)
                     fT4CDef_22->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_22->GetFLen(hdc, MsgFormatText.GetText(i)))/2, iLastY, CL_WHITE, 0, MsgFormatText.GetText(i));
               }
            }
            else if(TFC_State == TFC_INTRODUCTION) 
            {
               iTitleID = 688;
               for (int i = 0; i < Introduction.GetnbLine(); i++) 
               {
                  iLastY = (iScrollYPos/2)+i*fT4CDef_22->GetFHeight(hdc);
                  if(iLastY < yFramePos+ConnectIntroBack[0].GetHeight()-30 && iLastY >  yFramePos)
                     fT4CDef_22->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_22->GetFLen(hdc, Introduction.GetText(i)))/2, iLastY, CL_WHITE, 0, Introduction.GetText(i));
               }
            }
            else if(TFC_State == TFC_VIEW_CREDITS) 
            {
               iTitleID = 689;
               for (int i = 0; i < Credits.GetnbLine(); i++) 
               {
                  iLastY = (iScrollYPos/2)+i*fT4CDef_22->GetFHeight(hdc);
                  if(iLastY < yFramePos+ConnectIntroBack[0].GetHeight()-30 && iLastY >  yFramePos)
                     fT4CDef_22->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_22->GetFLen(hdc, Credits.GetText(i)))/2, iLastY, CL_WHITE, 0, Credits.GetText(i));
               }
            }

            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[iTitleID]))/2)-1, 20-1, CL_BLACK, 0, g_LocalString[iTitleID]);   
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[iTitleID]))/2)+1, 20+1, CL_BLACK, 0, g_LocalString[iTitleID]);   
            fT4CDef_30B->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[iTitleID]))/2, 20, CL_WHITE, 0, g_LocalString[iTitleID]);   

            DXDReleaseDC(hdc, 10);
         }
         break;

         case TFC_MENU: //OKOKOK
         {
            ConnectLogoDial[0].DrawSpriteN(0, g_Global.GetScreenH()-ConnectLogoDial[0].GetHeight(), &vsffx2,&ConnectLogoDial[1]);
            ConnectLogoDev [0].DrawSpriteN(g_Global.GetScreenW()-ConnectLogoDev[0].GetWidth(), g_Global.GetScreenH()-ConnectLogoDev[0].GetHeight(), &vsffx2,&ConnectLogoDev[1]);
            ConnectLogoT4cS[0].DrawSpriteN((g_Global.GetScreenW()-ConnectLogoT4cS[0].GetWidth())/2, 0, &vsffx2,&ConnectLogoT4cS[1]);
            
          
            int iXBox =  (g_Global.GetScreenW()-ConnectBackFrame[0].GetWidth())/2;
            int iYBox =  ((g_Global.GetScreenH()-ConnectBackFrame[0].GetHeight())/5)*2;
            ConnectBackFrame[0].DrawSpriteN(iXBox, iYBox, &vsffx2,&ConnectBackFrame[1]);
            
            int iXBtnX1 = iXBox+40;
            int iYBtnY1 = iYBox+44;
            int iXBtnX2 = iXBtnX1+ConnectFrameBtn->GetWidth();
            int iYBtnY2 = iYBtnY1+ConnectFrameBtn->GetHeight();
            int oYOff   = 29;
            int dwVal   = 0;
            
            int x = 0;
            int y = 0;
            Mouse2.GetPosition(&x, &y);
            
            int ib=0;
            for(ib=0;ib<4;ib++)
            {
               if(x >= iXBtnX1 && x <= iXBtnX2 && y >= iYBtnY1 && y <= iYBtnY2)
                   ConnectFrameBtn[1] .DrawSpriteN(iXBtnX1,iYBtnY1,&vsffx2);
               else
                  ConnectFrameBtn[0] .DrawSpriteN(iXBtnX1,iYBtnY1,&vsffx2);
               iYBtnY1+=oYOff;
               iYBtnY2+=oYOff;
            }
            
            //dessine texte sur les bouton...
            iXBtnX1 = iXBox+40;
            iYBtnY1 = iYBox+44;

            HDC hdc;
            DXDGetDCConnect(&hdc, 13);
            int dwTPos = 0;
            for(ib=0;ib<4;ib++)
            {
               //text ID des bouton a dessiner...
               if(ib == 0)
                  dwTPos = 2;
               else if(ib == 1)
                  dwTPos = 3;
               else if(ib == 2)
                  dwTPos = 5;
               else if(ib == 3)
                  dwTPos = 6;


               dwVal = V3_BtnFontB->GetFLen(hdc, g_LocalString[dwTPos]);
               dwVal = (ConnectFrameBtn[0].GetWidth()-dwVal)/2;
               V3_BtnFontB->DrawFont(hdc, iXBtnX1+dwVal  ,  iYBtnY1+5  , g_DefColorH        , 0, g_LocalString[dwTPos]);

               iYBtnY1+=oYOff;
               iYBtnY2+=oYOff;


            }

            //dessine les textes...
            
            //SetBkMode(hdc, TRANSPARENT);
            //SetTextColor(hdc,RGB(255,255,255));
            
            //Copyright
            sprintf_s(strTmp,512,"2002-2012+ ©Copyright Dialsoft Inc. All right reserved.");
            //nom du server
            int iTextW = V3_BtnFontB->GetFLen(hdc, strTmp);
            V3_BtnFontB->DrawFont(hdc, (g_Global.GetScreenW()-iTextW)/2  ,  g_Global.GetScreenH()-20  , CL_WHITE        , 0, strTmp);

            DXDReleaseDC(hdc, 13);
         } 
         break;

         case TFC_CHOOSE_PLAYER:
         {
            //dessine mack de Intro et msg of the day...
            int iXPos = (g_Global.GetScreenW()-ConnectSelectPlayer[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectSelectPlayer[0].GetHeight())/2;
            ConnectSelectPlayer[0].DrawSpriteN(iXPos,iYPos, &vsffx2,&ConnectSelectPlayer[1]);

            int x = 0;
            int y = 0;
            Mouse2.GetPosition(&x, &y);
            
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            
            if(x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 )
               ConnectPrev[1].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);

            //les boutons...
            //Create
            int iCrX1 = iXPos+24;
            int iCrY1 = iYPos+324;
            int iCrX2 = iCrX1+ConnectBtn80[0].GetWidth();
            int iCrY2 = iCrY1+ConnectBtn80[0].GetHeight();

            if(x >= iCrX1 && x <= iCrX2 && y >= iCrY1 && y <= iCrY2 )
               ConnectBtn80[1].DrawSpriteN(iCrX1,iCrY1, &vsffx2,&ConnectBtn80[2]);
            else
               ConnectBtn80[0].DrawSpriteN(iCrX1,iCrY1, &vsffx2,&ConnectBtn80[2]);

            //Delete
            int iDlX1 = iXPos+103;
            int iDlY1 = iYPos+324;
            int iDlX2 = iDlX1+ConnectBtn92[0].GetWidth();
            int iDlY2 = iDlY1+ConnectBtn92[0].GetHeight();
            
            if(x >= iDlX1 && x <= iDlX2 && y >= iDlY1 && y <= iDlY2 )
               ConnectBtn92[1].DrawSpriteN(iDlX1,iDlY1, &vsffx2,&ConnectBtn92[2]);
            else
               ConnectBtn92[0].DrawSpriteN(iDlX1,iDlY1, &vsffx2,&ConnectBtn92[2]);

            //Bouton text Enter
            int iEnterX1 = iXPos+418;
            int iEnterY1 = iYPos+326;
            int iEnterX2 = iXPos+495;
            int iEnterY2 = iYPos+343;
            
            if(x >= iEnterX1 && x <= iEnterX2 && y >= iEnterY1 && y <= iEnterY2 )
               ConnectNext[1].DrawSpriteN(iEnterX1+50,iEnterY1, &vsffx2,&ConnectNext[2]);
            else
               ConnectNext[0].DrawSpriteN(iEnterX1+50,iEnterY1, &vsffx2,&ConnectNext[2]);

            


            HDC hdc;
            DXDGetDCConnect(&hdc, 10);
            int yStart = iYPos+47;
            int yOff   = 21;
            for (i = NM_FPVisible; i < NM_FPVisible+NM_MPVisible; i++) 
            { 
               if(i < NbOfPlayer)
               {
                  sprintf_s(strTmp,512,"%u. %s",i+1,Custom.m_strName[i]);
                  //NomPlayer
                  if(i == Selected)
                     fT4CDef_21->DrawFont(hdc, iXPos+26, yStart+2+(i*yOff), CL_GUI_GREEN_LIGHT, 0, strTmp);
                  else
                     fT4CDef_21->DrawFont(hdc, iXPos+26, yStart+2+(i*yOff), CL_WHITE, 0, strTmp);

               }
            }

            if(Selected >= 0 && Custom.m_shLevel[Selected] > 0) //minimum level 1 player
            {
               //Dessine le NOM + level
               fT4CDef_23B->DrawFont(hdc, iXPos+282+(118-fT4CDef_23B->GetFLen(hdc, Custom.m_strName[Selected]))/2, iYPos+30, CL_WHITE, 0, Custom.m_strName[Selected]);
               
               sprintf_s(strTmp,512,"%d",Custom.m_shLevel[Selected]);
               fT4CDef_23B->DrawFont(hdc, iXPos+295+(94-fT4CDef_23B->GetFLen(hdc, strTmp))/2, iYPos+271, CL_GUI_GREEN_LIGHT, 0, strTmp);
            }
            

            

            //Header du menu...
            //Niveau 
            fT4CDef_23B->DrawFont(hdc, iXPos+295+(94-fT4CDef_23B->GetFLen(hdc, g_LocalString[39]))/2, iYPos+248, CL_WHITE, 0, g_LocalString[39]);
            //Liste
            fT4CDef_23B->DrawFont(hdc, iXPos+25+(153-fT4CDef_23B->GetFLen(hdc, g_LocalString[691]))/2, iYPos+25, CL_WHITE, 0, g_LocalString[691]);
            
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[690]))/2)-1, 20-1, CL_BLACK, 0, g_LocalString[690]);   
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[690]))/2)+1, 20+1, CL_BLACK, 0, g_LocalString[690]);   
            fT4CDef_30B->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[690]))/2, 20, CL_WHITE, 0, g_LocalString[690]); 
            
            //texte de bouton
            //creer
            fT4CDef_23B->DrawFont(hdc, iCrX1+(79-fT4CDef_23B->GetFLen(hdc, g_LocalString[4]))/2, iCrY1+2, CL_WHITE, 0, g_LocalString[4]);
            //delete
            fT4CDef_23B->DrawFont(hdc, iDlX1+(79-fT4CDef_23B->GetFLen(hdc, g_LocalString[7]))/2, iDlY1+2, CL_WHITE, 0, g_LocalString[7]);

            //delete
            if(x >= iEnterX1 && x <= iEnterX2 && y >= iEnterY1 && y <= iEnterY2 )
               fT4CDef_23B->DrawFont(hdc, iEnterX1, iEnterY1-2, CL_GUI_GREEN_LIGHT, 0, g_LocalString[105]);
            else
               fT4CDef_23B->DrawFont(hdc, iEnterX1, iEnterY1-2, CL_WHITE, 0, g_LocalString[105]);
            


            
            DXDReleaseDC(hdc, 10);

            if(Selected >= 0 && Custom.m_shLevel[Selected] > 0) //minimum level 1 player
            {
               Objects.Lock(0);
               
               TFCObject ObjectTmp;
               ObjectTmp.Type = Custom.m_shRace[Selected];
               if(ObjectTmp.Type != __PLAYER_HUMAN_PUPPET && ObjectTmp.Type != __PLAYER_HUMAN_FEMALE)
                  ObjectTmp.Type = __PLAYER_HUMAN_PUPPET;

               ObjectTmp.Type3D = 71;
               Objects.SetMenuPuppet(&ObjectTmp,Custom.m_wEquipList[Selected][0]
                  ,Custom.m_wEquipList[Selected][1]
                  ,Custom.m_wEquipList[Selected][2]
                  ,Custom.m_wEquipList[Selected][3]
                  ,Custom.m_wEquipList[Selected][4]
                  ,Custom.m_wEquipList[Selected][8]
                  ,Custom.m_wEquipList[Selected][9]
                  ,Custom.m_wEquipList[Selected][15]
                  ,Custom.m_shHairColor[Selected] );
               
               RECT rcClip;
               rcClip.left   = 0;
               rcClip.top    = 0;//iOffY;
               rcClip.right  = g_Global.GetScreenW();
               rcClip.bottom = g_Global.GetScreenH();

               dwCnt++;
               if(dwCnt >8)
               {
                  dwCnt = 0;
                  
                  //dwDir++; 
                  //if(dwDir >7)
                  //   dwDir = 0;
                  
                  
                  dwDir+=dwIncr;  
                  if(dwDir >4)
                  {
                     dwDir = 4;
                     dwIncr = -1;
                  }
                  else if(dwDir < 0)
                  {
                     dwDir = 0;
                     dwIncr = 1;
                  }
                  
                  switch(dwDir)
                  {
                     case 0: dwRealDir = 2; dwOffset = 0;break;
                     case 1: dwRealDir = 1; dwOffset = 0;break;
                     case 2: dwRealDir = 4; dwOffset = 0;break;
                     case 3: dwRealDir = 7; dwOffset = 0;break;
                     case 4: dwRealDir = 8; dwOffset = 0;break;
                     case 5: dwRealDir = 9; dwOffset = 5;break;
                     case 6: dwRealDir = 6; dwOffset = 5;break;
                     case 7: dwRealDir = 3; dwOffset = -5;break;
                  }
                  
               }
               
               
               Objects.DrawPuppetClip( iXPos+323+dwOffset, iYPos+199,dwRealDir ,&rcClip,&ObjectTmp);
               Objects.Unlock(0);

               ConnectPodiumGlow[0].DrawSpriteN(iXPos+285,iYPos+67, &vsffx2,&ConnectPodiumGlow[1]);
            }
         }
         break;
         case TFC_CONFIRM_DELETE:
         {
            //dessine mack de Intro et msg of the day...
            int iXPos = (g_Global.GetScreenW()-ConnectSelectPlayer[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectSelectPlayer[0].GetHeight())/2;
            ConnectSelectPlayer[0].DrawSpriteN(iXPos,iYPos, &vsffx2,&ConnectSelectPlayer[1]);

            int x = 0;
            int y = 0;
            Mouse2.GetPosition(&x, &y);
            
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            
            if(x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 )
               ConnectPrev[1].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);

            //les boutons...
            //OUI
            int iOuiX1 = iXPos+343;
            int iOuiY1 = iYPos+324;
            int iOuiX2 = iOuiX1+ConnectBtn80[0].GetWidth();
            int iOuiY2 = iOuiY1+ConnectBtn80[0].GetHeight();

            if(x >= iOuiX1 && x <= iOuiX2 && y >= iOuiY1 && y <= iOuiY2 )
               ConnectBtn80[1].DrawSpriteN(iOuiX1,iOuiY1, &vsffx2,&ConnectBtn80[2]);
            else
               ConnectBtn80[0].DrawSpriteN(iOuiX1,iOuiY1, &vsffx2,&ConnectBtn80[2]);

            //NON
            int iNonX1 = iXPos+427;
            int iNonY1 = iYPos+324;
            int iNonX2 = iNonX1+ConnectBtn80[0].GetWidth();
            int iNonY2 = iNonY1+ConnectBtn80[0].GetHeight();
            
            if(x >= iNonX1 && x <= iNonX2 && y >= iNonY1 && y <= iNonY2 )
               ConnectBtn80[1].DrawSpriteN(iNonX1,iNonY1, &vsffx2,&ConnectBtn80[2]);
            else
               ConnectBtn80[0].DrawSpriteN(iNonX1,iNonY1, &vsffx2,&ConnectBtn80[2]);

                        


            HDC hdc;
            DXDGetDCConnect(&hdc, 10);
            int yStart = iYPos+47;
            int yOff   = 21;
            for (i = NM_FPVisible; i < NM_FPVisible+NM_MPVisible; i++) 
            { 
               if(i < NbOfPlayer)
               {
                  sprintf_s(strTmp,512,"%u. %s",i+1,Custom.m_strName[i]);
                  //NomPlayer
                  if(i == Selected)
                     fT4CDef_21->DrawFont(hdc, iXPos+26, yStart+2+(i*yOff), CL_GUI_GREEN_LIGHT, 0, strTmp);
                  else
                     fT4CDef_21->DrawFont(hdc, iXPos+26, yStart+2+(i*yOff), CL_WHITE, 0, strTmp);

               }
            }

            if(Selected >= 0 && Custom.m_shLevel[Selected] > 0) //minimum level 1 player
            {
               //Dessine le NOM + level
               fT4CDef_23B->DrawFont(hdc, iXPos+282+(118-fT4CDef_23B->GetFLen(hdc, Custom.m_strName[Selected]))/2, iYPos+30, CL_WHITE, 0, Custom.m_strName[Selected]);
               
               sprintf_s(strTmp,512,"%d",Custom.m_shLevel[Selected]);
               fT4CDef_23B->DrawFont(hdc, iXPos+295+(94-fT4CDef_23B->GetFLen(hdc, strTmp))/2, iYPos+271, CL_GUI_GREEN_LIGHT, 0, strTmp);
            }
            

            

            //Header du menu...
            //Niveau 
            fT4CDef_23B->DrawFont(hdc, iXPos+295+(94-fT4CDef_23B->GetFLen(hdc, g_LocalString[39]))/2, iYPos+248, CL_WHITE, 0, g_LocalString[39]);
            //Liste
            fT4CDef_23B->DrawFont(hdc, iXPos+25+(153-fT4CDef_23B->GetFLen(hdc, g_LocalString[691]))/2, iYPos+25, CL_WHITE, 0, g_LocalString[691]);
            
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[693]))/2)-1, 20-1, CL_BLACK, 0, g_LocalString[693]);   
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[693]))/2)+1, 20+1, CL_BLACK, 0, g_LocalString[693]);   
            fT4CDef_30B->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[693]))/2, 20, CL_WHITE, 0, g_LocalString[693]); 
            
            //texte de bouton
            //creer
            fT4CDef_23B->DrawFont(hdc, iOuiX1+(79-fT4CDef_23B->GetFLen(hdc, g_LocalString[4]))/2, iOuiY1+2, CL_WHITE, 0, g_LocalString[25]);
            //delete
            fT4CDef_23B->DrawFont(hdc, iNonX1+(79-fT4CDef_23B->GetFLen(hdc, g_LocalString[7]))/2, iNonY1+2, CL_WHITE, 0, g_LocalString[26]);


            int iEnterX1 = iXPos+25;
            int iEnterY1 = iYPos+326;
            fT4CDef_23B->DrawFont(hdc, iEnterX1+(316-fT4CDef_23B->GetFLen(hdc, g_LocalString[692]))/2, iEnterY1-2, CL_RED, 0, g_LocalString[692]);

            
            DXDReleaseDC(hdc, 10);

            if(Selected >= 0 && Custom.m_shLevel[Selected] > 0) //minimum level 1 player
            {
               Objects.Lock(0);
               
               TFCObject ObjectTmp;
               ObjectTmp.Type = Custom.m_shRace[Selected];
               if(ObjectTmp.Type != __PLAYER_HUMAN_PUPPET && ObjectTmp.Type != __PLAYER_HUMAN_FEMALE)
                  ObjectTmp.Type = __PLAYER_HUMAN_PUPPET;
               ObjectTmp.Type3D = 71;
               Objects.SetMenuPuppet(&ObjectTmp,Custom.m_wEquipList[Selected][0]
                  ,Custom.m_wEquipList[Selected][1]
                  ,Custom.m_wEquipList[Selected][2]
                  ,Custom.m_wEquipList[Selected][3]
                  ,Custom.m_wEquipList[Selected][4]
                  ,Custom.m_wEquipList[Selected][8]
                  ,Custom.m_wEquipList[Selected][9]
                  ,Custom.m_wEquipList[Selected][15]
                  ,Custom.m_shHairColor[Selected] );
               
               RECT rcClip;
               rcClip.left   = 0;
               rcClip.top    = 0;//iOffY;
               rcClip.right  = g_Global.GetScreenW();
               rcClip.bottom = g_Global.GetScreenH();

              
               
               
               Objects.DrawPuppetClip( iXPos+323, iYPos+199,2 ,&rcClip,&ObjectTmp);
               Objects.Unlock(0);

               ConnectPodiumGlow[0].DrawSpriteN(iXPos+285,iYPos+57, &vsffx2,&ConnectPodiumGlow[1]);
            }
         }
         break;


         
         case TFC_CHOOSE_NAME: 
         case TFC_ASK_CREATE_CHARACTER:
         {
            //Prev general
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            if(x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 )
               ConnectPrev[1].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            
            //dessine mack de Intro et msg of the day...
            int iXPos = (g_Global.GetScreenW()-ConnectCreateBack01[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectCreateBack01[0].GetHeight())/2;
            ConnectCreateBack01[0].DrawSpriteN(iXPos,iYPos, &vsffx2,&ConnectCreateBack01[1]);

            int iNextCrX1 = iXPos+590;
            int iNextCrY1 = iYPos+411;
            int iNextCrX2 = iNextCrX1+ConnectNext[0].GetWidth();
            int iNextCrY2 = iNextCrY1+ConnectNext[0].GetHeight();
            if(x >= iNextCrX1 && x <= iNextCrX2 && y >= iNextCrY1 && y <= iNextCrY2 )
               ConnectNext[1].DrawSpriteN(iNextCrX1,iNextCrY1, &vsffx2,&ConnectNext[2]);
            else
               ConnectNext[0].DrawSpriteN(iNextCrX1,iNextCrY1, &vsffx2,&ConnectNext[2]);


            HDC hdc;
            DXDGetDCConnect(&hdc, 10);
                      
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2)-1, 20-1, CL_BLACK, 0, g_LocalString[694]);   
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2)+1, 20+1, CL_BLACK, 0, g_LocalString[694]);   
            fT4CDef_30B->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2, 20, CL_WHITE, 0, g_LocalString[694]); 
            
            // dessine le tooltips...
            int iYStart = iYPos+270;
            for (int i = 0; i < MsgCreate01.GetnbLine(); i++) 
            {
               if(i == 0)
                  fSmallNormal_12->DrawFont(hdc, iXPos+22, iYStart, CL_WHITE, 0, MsgCreate01.GetText(i));
               else
                  fSmallNormal_12->DrawFont(hdc, iXPos+14, iYStart, CL_WHITE, 0, MsgCreate01.GetText(i));
               iYStart+=13;
            }

            //Question du nom
            fT4CDef_23B->DrawFont(hdc, iXPos+333+(253-fT4CDef_23B->GetFLen(hdc, g_LocalString[696]))/2, iYPos+170, CL_WHITE, 0, g_LocalString[696]);

            //Nom msg erreur
            if (nameIsNotOk == 1) 
            {
               fT4CDef_23B->DrawFont(hdc, iXPos+303+(313-fT4CDef_23B->GetFLen(hdc, g_LocalString[499]))/2, iYPos+310, CL_RED_DARK, 0, g_LocalString[ 499 ] );
            } 
            else if (nameIsNotOk == 2) 
            {
               fT4CDef_23B->DrawFont(hdc, iXPos+303+(313-fT4CDef_23B->GetFLen(hdc, g_LocalString[521]))/2, iYPos+310, CL_RED_DARK, 0, g_LocalString[ 521 ] );
            }
            

            char Temp[100];
            if(bShowCursor)
               sprintf_s(Temp,100,"%s|", Player.Name);
            else
               sprintf_s(Temp,100,"%s", Player.Name);
            fT4CDef_23B->DrawFont(hdc, iXPos+333+(253-fT4CDef_23B->GetFLen(hdc, Player.Name))/2,  iYPos+218, CL_GUI_GREEN_LIGHT, 0, Temp);

            //dessine les 5 stat de progression

            fT4CDef_23B->DrawFont(hdc, iXPos+ 74+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[697]))/2, iYPos+406, CL_GUI_GREEN_LIGHT, 0, g_LocalString[ 697 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+174+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[698]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 698 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+274+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[699]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 699 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+374+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[700]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 700 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+474+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[701]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 701 ] );

            
            DXDReleaseDC(hdc, 10);


            //Dessine le puppet...
            Objects.Lock(0);
            
            TFCObject ObjectTmp;
            ObjectTmp.Type = __PLAYER_HUMAN_PUPPET;
            ObjectTmp.Type3D = 71;
            Objects.SetMenuPuppet(&ObjectTmp,__OBJGROUP_BODY_CLOTH1,0,0,0,__OBJGROUP_LEG_CLOTH1,0,0,0,0 );
            
            RECT rcClip;
            rcClip.left   = 0;
            rcClip.top    = 0;//iOffY;
            rcClip.right  = g_Global.GetScreenW();
            rcClip.bottom = g_Global.GetScreenH();
            
            
            Objects.DrawPuppetClip( iXPos+126, iYPos+187,2 ,&rcClip,&ObjectTmp);
            Objects.Unlock(0);
            
            ConnectPodiumGlow[0].DrawSpriteN(iXPos+88,iYPos+60, &vsffx2,&ConnectPodiumGlow[1]);
         }
         break;
         case TFC_CHOOSE_SEXX: 
         {
            //Prev general
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            if(x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 )
               ConnectPrev[1].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            
            //dessine mack de Intro et msg of the day...
            int iXPos = (g_Global.GetScreenW()-ConnectCreateBack01[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectCreateBack01[0].GetHeight())/2;
            ConnectCreateBack02[0].DrawSpriteN(iXPos,iYPos, &vsffx2,&ConnectCreateBack02[1]);

            
            int iPrevCrX1 = iXPos+10;
            int iPrevCrY1 = iYPos+411;
            int iPrevCrX2 = iPrevCrX1+ConnectPrev[0].GetWidth();
            int iPrevCrY2 = iPrevCrY1+ConnectPrev[0].GetHeight();
            if(x >= iPrevCrX1 && x <= iPrevCrX2 && y >= iPrevCrY1 && y <= iPrevCrY2 )
               ConnectPrev[1].DrawSpriteN(iPrevCrX1,iPrevCrY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevCrX1,iPrevCrY1, &vsffx2,&ConnectPrev[2]);
            

            int iNextCrX1 = iXPos+590;
            int iNextCrY1 = iYPos+411;
            int iNextCrX2 = iNextCrX1+ConnectNext[0].GetWidth();
            int iNextCrY2 = iNextCrY1+ConnectNext[0].GetHeight();
            if(x >= iNextCrX1 && x <= iNextCrX2 && y >= iNextCrY1 && y <= iNextCrY2 )
               ConnectNext[1].DrawSpriteN(iNextCrX1,iNextCrY1, &vsffx2,&ConnectNext[2]);
            else
               ConnectNext[0].DrawSpriteN(iNextCrX1,iNextCrY1, &vsffx2,&ConnectNext[2]);


            //Icon Bn SexeM
            int iSMX1 = iXPos+353;
            int iSMY1 = iYPos+211;
            int iSMX2 = iSMX1+ConnectBtnMen[0].GetWidth();
            int iSMY2 = iSMY1+ConnectBtnMen[0].GetHeight();
            if(x >= iSMX1 && x <= iSMX2 && y >= iSMY1 && y <= iSMY2 )
               ConnectBtnMen[1].DrawSpriteN(iSMX1,iSMY1, &vsffx2,&ConnectBtnMen[2]);
            else
               ConnectBtnMen[0].DrawSpriteN(iSMX1,iSMY1, &vsffx2,&ConnectBtnMen[2]);

            //Icon Bn SexeF
            int iSFX1 = iXPos+470;
            int iSFY1 = iYPos+211;
            int iSFX2 = iSFX1+ConnectBtnMen[0].GetWidth();
            int iSFY2 = iSFY1+ConnectBtnMen[0].GetHeight();
            if(x >= iSFX1 && x <= iSFX2 && y >= iSFY1 && y <= iSFY2 )
               ConnectBtnWomen[1].DrawSpriteN(iSFX1,iSFY1, &vsffx2,&ConnectBtnWomen[2]);
            else
               ConnectBtnWomen[0].DrawSpriteN(iSFX1,iSFY1, &vsffx2,&ConnectBtnWomen[2]);



            HDC hdc;
            DXDGetDCConnect(&hdc, 10);
                      
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2)-1, 20-1, CL_BLACK, 0, g_LocalString[694]);   
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2)+1, 20+1, CL_BLACK, 0, g_LocalString[694]);   
            fT4CDef_30B->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2, 20, CL_WHITE, 0, g_LocalString[694]); 
            
            // dessine le tooltips...
            int iYStart = iYPos+270;
            for (int i = 0; i < MsgCreate02.GetnbLine(); i++) 
            {
               if(i == 0)
                  fSmallNormal_12->DrawFont(hdc, iXPos+22, iYStart, CL_WHITE, 0, MsgCreate02.GetText(i));
               else
                  fSmallNormal_12->DrawFont(hdc, iXPos+14, iYStart, CL_WHITE, 0, MsgCreate02.GetText(i));
               iYStart+=13;
            }

            //Question du nom
            fT4CDef_23B->DrawFont(hdc, iXPos+333+(253-fT4CDef_23B->GetFLen(hdc, g_LocalString[702]))/2, iYPos+170, CL_WHITE, 0, g_LocalString[702]);

            //Homme
            fT4CDef_23B->DrawFont(hdc, iXPos+353+(96-fT4CDef_23B->GetFLen(hdc, g_LocalString[703]))/2, iYPos+315, CL_WHITE, 0, g_LocalString[703]);
            //Femme
            fT4CDef_23B->DrawFont(hdc, iXPos+470+(96-fT4CDef_23B->GetFLen(hdc, g_LocalString[704]))/2, iYPos+315, CL_WHITE, 0, g_LocalString[704]);

           
            //Nom du Bonhomme
            fT4CDef_23B->DrawFont(hdc, iXPos+94+(98-fT4CDef_23B->GetFLen(hdc, Player.Name))/2, iYPos+50, CL_WHITE, 0, Player.Name);
            

            
            //dessine les 5 stat de progression
            fT4CDef_23B->DrawFont(hdc, iXPos+ 74+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[697]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 697 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+174+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[698]))/2, iYPos+406, CL_GUI_GREEN_LIGHT, 0, g_LocalString[ 698 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+274+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[699]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 699 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+374+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[700]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 700 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+474+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[701]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 701 ] );

            
            DXDReleaseDC(hdc, 10);


            //Dessine le puppet...
            Objects.Lock(0);
            
            TFCObject ObjectTmp;
            if (!Player.Sexx) 
               ObjectTmp.Type  = __PLAYER_HUMAN_PUPPET;
            else 
               ObjectTmp.Type  =  __PLAYER_HUMAN_FEMALE;
            ObjectTmp.Type3D = 71;
            Objects.SetMenuPuppet(&ObjectTmp,__OBJGROUP_BODY_CLOTH1,0,0,0,__OBJGROUP_LEG_CLOTH1,0,0,0,0 );
            
            RECT rcClip;
            rcClip.left   = 0;
            rcClip.top    = 0;//iOffY;
            rcClip.right  = g_Global.GetScreenW();
            rcClip.bottom = g_Global.GetScreenH();
             
            
            Objects.DrawPuppetClip( iXPos+126, iYPos+187,2 ,&rcClip,&ObjectTmp);
            Objects.Unlock(0);
            
            ConnectPodiumGlow[0].DrawSpriteN(iXPos+88,iYPos+60, &vsffx2,&ConnectPodiumGlow[1]);
         }
         break;
         case TFC_CHOOSE_RACE: 
         {
            //Prev general
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            if(x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 )
               ConnectPrev[1].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            
            //dessine mack de Intro et msg of the day...
            int iXPos = (g_Global.GetScreenW()-ConnectCreateBack01[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectCreateBack01[0].GetHeight())/2;
            ConnectCreateBack03[0].DrawSpriteN(iXPos,iYPos, &vsffx2,&ConnectCreateBack03[1]);

            
            int iPrevCrX1 = iXPos+10;
            int iPrevCrY1 = iYPos+411;
            int iPrevCrX2 = iPrevCrX1+ConnectPrev[0].GetWidth();
            int iPrevCrY2 = iPrevCrY1+ConnectPrev[0].GetHeight();
            if(x >= iPrevCrX1 && x <= iPrevCrX2 && y >= iPrevCrY1 && y <= iPrevCrY2 )
               ConnectPrev[1].DrawSpriteN(iPrevCrX1,iPrevCrY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevCrX1,iPrevCrY1, &vsffx2,&ConnectPrev[2]);
            

            int iNextCrX1 = iXPos+590;
            int iNextCrY1 = iYPos+411;
            int iNextCrX2 = iNextCrX1+ConnectNext[0].GetWidth();
            int iNextCrY2 = iNextCrY1+ConnectNext[0].GetHeight();
            if(x >= iNextCrX1 && x <= iNextCrX2 && y >= iNextCrY1 && y <= iNextCrY2 )
               ConnectNext[1].DrawSpriteN(iNextCrX1,iNextCrY1, &vsffx2,&ConnectNext[2]);
            else
               ConnectNext[0].DrawSpriteN(iNextCrX1,iNextCrY1, &vsffx2,&ConnectNext[2]);


            //5 back de question
            int iQX1 = iXPos+283;
            int iQX2 = iQX1+332;
            int iOff = 48;
            
            
            for(int k=0;k<5;k++)
            {
               int iQY1 = iYPos+134 +(k*iOff);   
               int iQY2 = iQY1+45;
               if(QuestionChoice-1 == k)
                  QuestionBack[1].DrawSpriteN(iQX1,iQY1, &vsffx2,&QuestionBack[2]);
               else
                  QuestionBack[0].DrawSpriteN(iQX1,iQY1, &vsffx2,&QuestionBack[2]);

            }
            

               

            


            HDC hdc;
            DXDGetDCConnect(&hdc, 10);
                      
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2)-1, 20-1, CL_BLACK, 0, g_LocalString[694]);   
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2)+1, 20+1, CL_BLACK, 0, g_LocalString[694]);   
            fT4CDef_30B->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2, 20, CL_WHITE, 0, g_LocalString[694]); 
            
            // dessine le tooltips...
            int iYStart = iYPos+270;
            for (int i = 0; i < MsgCreate03.GetnbLine(); i++) 
            {
               if(i == 0)
                  fSmallNormal_12->DrawFont(hdc, iXPos+22, iYStart, CL_WHITE, 0, MsgCreate03.GetText(i));
               else
                  fSmallNormal_12->DrawFont(hdc, iXPos+14, iYStart, CL_WHITE, 0, MsgCreate03.GetText(i));
               iYStart+=13;
            }

            
           
            //Nom du Bonhomme
            fT4CDef_23B->DrawFont(hdc, iXPos+94+(98-fT4CDef_23B->GetFLen(hdc, Player.Name))/2, iYPos+50, CL_WHITE, 0, Player.Name);

            //Question tag
            sprintf_s(strTmp,512,"Question %d / %d",(QuestionNumber+1)>4?4:QuestionNumber+1,4);
            fT4CDef_23B->DrawFont(hdc, iQX1+(332-fT4CDef_23B->GetFLen(hdc, strTmp))/2, iYPos+50, CL_WHITE, 0, strTmp);

            if (QuestionNumber < 4) 
            {
               int i;
               //Question
               for (i = 0; i < Question[QN[QuestionNumber]].GetnbLine(); i++)
               {
                  if(i == 0)
                     fSmallNormal_13->DrawFont(hdc, iQX1+5, iYPos+85+i*fSmallNormal_13->GetFHeight(hdc), CL_ORANGE, 0, Question[QN[QuestionNumber]].GetText(i));
                  else
                     fSmallNormal_13->DrawFont(hdc, iQX1+5-8, iYPos+85+i*fSmallNormal_13->GetFHeight(hdc), CL_ORANGE, 0, Question[QN[QuestionNumber]].GetText(i));
               }
               
               //reponse 1
               for (i = 0; i < Reponse[QN[QuestionNumber]][RN[QuestionNumber][0]].GetnbLine(); i++)
               {
                  if(i == 0)
                     fSmallNormal_13->DrawFont(hdc, iQX1+5, (iYPos+137)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][0]].GetText(i));
                  else
                     fSmallNormal_13->DrawFont(hdc, iQX1+5-8, (iYPos+137)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][0]].GetText(i));

               }
               
               //reponse 2
               for (i = 0; i < Reponse[QN[QuestionNumber]][RN[QuestionNumber][1]].GetnbLine(); i++)
               {
                  if(i == 0)
                     fSmallNormal_13->DrawFont(hdc, iQX1+5, (iYPos+185)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][1]].GetText(i));
                  else
                     fSmallNormal_13->DrawFont(hdc, iQX1+5-8, (iYPos+185)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][1]].GetText(i));
               }
               
               //reponse 3
               for (i = 0; i < Reponse[QN[QuestionNumber]][RN[QuestionNumber][2]].GetnbLine(); i++)
               {
                  if(i == 0)
                     fSmallNormal_13->DrawFont(hdc, iQX1+5, (iYPos+233)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][2]].GetText(i));
                  else
                     fSmallNormal_13->DrawFont(hdc, iQX1+5-8, (iYPos+233)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][2]].GetText(i));
               }
               
               //reponse 4
               for (i = 0; i < Reponse[QN[QuestionNumber]][RN[QuestionNumber][3]].GetnbLine(); i++)
               {
                  if(i == 0)
                     fSmallNormal_13->DrawFont(hdc, iQX1+5, (iYPos+281)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][3]].GetText(i));
                  else
                     fSmallNormal_13->DrawFont(hdc, iQX1+5-8, (iYPos+281)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][3]].GetText(i));
               }
               
               //reponse 5
               for (i = 0; i < Reponse[QN[QuestionNumber]][RN[QuestionNumber][4]].GetnbLine(); i++)
               {
                  if(i == 0)
                     fSmallNormal_13->DrawFont(hdc, iQX1+5, (iYPos+329)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][4]].GetText(i));
                  else
                     fSmallNormal_13->DrawFont(hdc, iQX1+5-8, (iYPos+329)+(i*fSmallNormal_13->GetFHeight(hdc)), CL_WHITE, 0, Reponse[QN[QuestionNumber]][RN[QuestionNumber][4]].GetText(i));
               }
            }
            

            
            //dessine les 5 stat de progression
            fT4CDef_23B->DrawFont(hdc, iXPos+ 74+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[697]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 697 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+174+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[698]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 698 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+274+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[699]))/2, iYPos+406, CL_GUI_GREEN_LIGHT, 0, g_LocalString[ 699 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+374+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[700]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 700 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+474+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[701]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 701 ] );

            
            DXDReleaseDC(hdc, 10);


            //Dessine le puppet...
            Objects.Lock(0);
            
            TFCObject ObjectTmp;
            if (!Player.Sexx) 
               ObjectTmp.Type  = __PLAYER_HUMAN_PUPPET;
            else 
               ObjectTmp.Type  =  __PLAYER_HUMAN_FEMALE;
            ObjectTmp.Type3D = 71;
            Objects.SetMenuPuppet(&ObjectTmp,__OBJGROUP_BODY_CLOTH1,0,0,0,__OBJGROUP_LEG_CLOTH1,0,0,0,0 );
            
            RECT rcClip;
            rcClip.left   = 0;
            rcClip.top    = 0;//iOffY;
            rcClip.right  = g_Global.GetScreenW();
            rcClip.bottom = g_Global.GetScreenH();
             
            
            Objects.DrawPuppetClip( iXPos+126, iYPos+187,2 ,&rcClip,&ObjectTmp);
            Objects.Unlock(0);
            
            ConnectPodiumGlow[0].DrawSpriteN(iXPos+88,iYPos+60, &vsffx2,&ConnectPodiumGlow[1]);
         }
         break;
        
         case TFC_MAKE_REROLL: 
         {
            //Prev general
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            if(x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 )
               ConnectPrev[1].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevX1,iPrevY1, &vsffx2,&ConnectPrev[2]);
            
            //dessine mack de Intro et msg of the day...
            int iXPos = (g_Global.GetScreenW()-ConnectCreateBack01[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectCreateBack01[0].GetHeight())/2;
            ConnectCreateBack04[0].DrawSpriteN(iXPos,iYPos, &vsffx2,&ConnectCreateBack04[1]);

            
            int iPrevCrX1 = iXPos+10;
            int iPrevCrY1 = iYPos+411;
            int iPrevCrX2 = iPrevCrX1+ConnectPrev[0].GetWidth();
            int iPrevCrY2 = iPrevCrY1+ConnectPrev[0].GetHeight();
            if(x >= iPrevCrX1 && x <= iPrevCrX2 && y >= iPrevCrY1 && y <= iPrevCrY2 )
               ConnectPrev[1].DrawSpriteN(iPrevCrX1,iPrevCrY1, &vsffx2,&ConnectPrev[2]);
            else
               ConnectPrev[0].DrawSpriteN(iPrevCrX1,iPrevCrY1, &vsffx2,&ConnectPrev[2]);
            

            int iNextCrX1 = iXPos+590;
            int iNextCrY1 = iYPos+411;
            int iNextCrX2 = iNextCrX1+ConnectNext[0].GetWidth();
            int iNextCrY2 = iNextCrY1+ConnectNext[0].GetHeight();
            if(x >= iNextCrX1 && x <= iNextCrX2 && y >= iNextCrY1 && y <= iNextCrY2 )
               ConnectNext[1].DrawSpriteN(iNextCrX1,iNextCrY1, &vsffx2,&ConnectNext[2]);
            else
               ConnectNext[0].DrawSpriteN(iNextCrX1,iNextCrY1, &vsffx2,&ConnectNext[2]);


            //Dessine les 9 Icons...
            StatsIcons[0].DrawSpriteN(iXPos+299,iYPos+131, &vsffx2,&StatsIcons[8]);
            StatsIcons[1].DrawSpriteN(iXPos+299,iYPos+180, &vsffx2,&StatsIcons[8]);
            StatsIcons[2].DrawSpriteN(iXPos+299,iYPos+229, &vsffx2,&StatsIcons[8]);
            StatsIcons[3].DrawSpriteN(iXPos+299,iYPos+278, &vsffx2,&StatsIcons[8]);
            StatsIcons[4].DrawSpriteN(iXPos+299,iYPos+327, &vsffx2,&StatsIcons[8]);
            StatsIcons[5].DrawSpriteN(iXPos+459,iYPos+131, &vsffx2,&StatsIcons[8]);
            StatsIcons[6].DrawSpriteN(iXPos+459,iYPos+180, &vsffx2,&StatsIcons[8]);
            StatsIcons[7].DrawSpriteN(iXPos+459,iYPos+229, &vsffx2,&StatsIcons[8]);


            //Dessine le Dee
            int iDeX1 = iXPos+513;
            int iDeY1 = iYPos+329;
            int iDeX2 = iDeX1+StatDee[0].GetWidth();
            int iDeY2 = iDeY1+StatDee[0].GetHeight();
            if(x >= iDeX1 && x <= iDeX2 && y >= iDeY1 && y <= iDeY2 )
               StatDee[1].DrawSpriteN(iDeX1,iDeY1, &vsffx2,&StatDee[2]);
            else
               StatDee[0].DrawSpriteN(iDeX1,iDeY1, &vsffx2,&StatDee[2]);
           


            HDC hdc;
            DXDGetDCConnect(&hdc, 10);
                      
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2)-1, 20-1, CL_BLACK, 0, g_LocalString[694]);   
            fT4CDef_30B->DrawFont(hdc, ((g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2)+1, 20+1, CL_BLACK, 0, g_LocalString[694]);   
            fT4CDef_30B->DrawFont(hdc, (g_Global.GetScreenW()-fT4CDef_30B->GetFLen(hdc, g_LocalString[694]))/2, 20, CL_WHITE, 0, g_LocalString[694]); 
            
            // dessine le tooltips...
            int iYStart = iYPos+270;
            for (int i = 0; i < MsgCreate04.GetnbLine(); i++) 
            {
               if(i == 0)
                  fSmallNormal_12->DrawFont(hdc, iXPos+22, iYStart, CL_WHITE, 0, MsgCreate04.GetText(i));
               else
                  fSmallNormal_12->DrawFont(hdc, iXPos+14, iYStart, CL_WHITE, 0, MsgCreate04.GetText(i));
               iYStart+=13;
            }

            
           
            //Nom du Bonhomme
            fT4CDef_23B->DrawFont(hdc, iXPos+94+(98-fT4CDef_23B->GetFLen(hdc, Player.Name))/2, iYPos+90, CL_WHITE, 0, Player.Name);

            //Dessine Resultat
            fT4CDef_23B->DrawFont(hdc, iXPos+296+(307-fT4CDef_23B->GetFLen(hdc, g_LocalString[708]))/2, iYPos+87, CL_WHITE, 0, g_LocalString[708]);

            //Dessine jeter a nouveasu
            fT4CDef_23B->DrawFont(hdc, iXPos+456+(147-fT4CDef_23B->GetFLen(hdc, g_LocalString[710]))/2, iYPos+300, CL_WHITE, 0, g_LocalString[710]);

            //dessine les nom des caracteristique...

            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, g_LocalString[31] ))/2, iYPos+127, CL_WHITE, 0, g_LocalString[31]);
            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, g_LocalString[32] ))/2, iYPos+176, CL_WHITE, 0, g_LocalString[32]);
            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, g_LocalString[33] ))/2, iYPos+225, CL_WHITE, 0, g_LocalString[33]);
            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, g_LocalString[36] ))/2, iYPos+274, CL_WHITE, 0, g_LocalString[36]);
            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, g_LocalString[35] ))/2, iYPos+323, CL_WHITE, 0, g_LocalString[35]);
            fT4CDef_23B->DrawFont(hdc, iXPos+489+(113-fT4CDef_23B->GetFLen(hdc, g_LocalString[37] ))/2, iYPos+127, CL_WHITE, 0, g_LocalString[37]);
            fT4CDef_23B->DrawFont(hdc, iXPos+489+(113-fT4CDef_23B->GetFLen(hdc, g_LocalString[38] ))/2, iYPos+176, CL_WHITE, 0, g_LocalString[38]);
            fT4CDef_23B->DrawFont(hdc, iXPos+489+(113-fT4CDef_23B->GetFLen(hdc, g_LocalString[709]))/2, iYPos+225, CL_WHITE, 0, g_LocalString[709]);

            //Dessine les valeur des points
            sprintf_s(strTmp,512,"%d",Player.Str);
            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, strTmp ))/2, iYPos+127+20, CL_WHITE, 0, strTmp);
            sprintf_s(strTmp,512,"%d",Player.End);
            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, strTmp ))/2, iYPos+176+20, CL_WHITE, 0, strTmp);
            sprintf_s(strTmp,512,"%d",Player.Agi);
            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, strTmp ))/2, iYPos+225+20, CL_WHITE, 0, strTmp);
            sprintf_s(strTmp,512,"%d",Player.Int);
            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, strTmp ))/2, iYPos+274+20, CL_WHITE, 0, strTmp);
            sprintf_s(strTmp,512,"%d",Player.Wis);
            fT4CDef_23B->DrawFont(hdc, iXPos+329+(113-fT4CDef_23B->GetFLen(hdc, strTmp ))/2, iYPos+323+20, CL_WHITE, 0, strTmp);
            sprintf_s(strTmp,512,"%d",Player.Lck);
            fT4CDef_23B->DrawFont(hdc, iXPos+489+(113-fT4CDef_23B->GetFLen(hdc, strTmp ))/2, iYPos+127+20, CL_WHITE, 0, strTmp);
            sprintf_s(strTmp,512,"%d",Player.Hp);
            fT4CDef_23B->DrawFont(hdc, iXPos+489+(113-fT4CDef_23B->GetFLen(hdc, strTmp ))/2, iYPos+176+20, CL_WHITE, 0, strTmp);
            sprintf_s(strTmp,512,"%d",Player.Mana);
            fT4CDef_23B->DrawFont(hdc, iXPos+489+(113-fT4CDef_23B->GetFLen(hdc, strTmp ))/2, iYPos+225+20, CL_WHITE, 0, strTmp);

            
            //dessine les 5 stat de progression
            fT4CDef_23B->DrawFont(hdc, iXPos+ 74+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[697]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 697 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+174+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[698]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 698 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+274+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[699]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 699 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+374+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[700]))/2, iYPos+406, CL_GUI_GREEN_LIGHT, 0, g_LocalString[ 700 ] );
            fT4CDef_23B->DrawFont(hdc, iXPos+474+(80-fT4CDef_23B->GetFLen(hdc, g_LocalString[701]))/2, iYPos+406, CL_WHITE, 0, g_LocalString[ 701 ] );

            
            DXDReleaseDC(hdc, 10);


            //Dessine le puppet...
            Objects.Lock(0);
            
            TFCObject ObjectTmp;
            if (!Player.Sexx) 
               ObjectTmp.Type  = __PLAYER_HUMAN_PUPPET;
            else 
               ObjectTmp.Type  =  __PLAYER_HUMAN_FEMALE;
            ObjectTmp.Type3D = 71;
            Objects.SetMenuPuppet(&ObjectTmp,__OBJGROUP_BODY_CLOTH1,0,0,0,__OBJGROUP_LEG_CLOTH1,0,0,0,0 );
            
            RECT rcClip;
            rcClip.left   = 0;
            rcClip.top    = 0;//iOffY;
            rcClip.right  = g_Global.GetScreenW();
            rcClip.bottom = g_Global.GetScreenH();
             
            
            Objects.DrawPuppetClip( iXPos+126, iYPos+187,2 ,&rcClip,&ObjectTmp);
            Objects.Unlock(0);
            
            ConnectPodiumGlow[0].DrawSpriteN(iXPos+88,iYPos+60, &vsffx2,&ConnectPodiumGlow[1]);
         }
         break;
            
         case TFC_WARNING_2:
         case TFC_WARNING:
         {
            ConnectLogoDial[0].DrawSpriteN(0, g_Global.GetScreenH()-ConnectLogoDial[0].GetHeight(), &vsffx2,&ConnectLogoDial[1]);
            ConnectLogoDev [0].DrawSpriteN(g_Global.GetScreenW()-ConnectLogoDev[0].GetWidth(), g_Global.GetScreenH()-ConnectLogoDev[0].GetHeight(), &vsffx2,&ConnectLogoDev[1]);
            ConnectLogoT4cS[0].DrawSpriteN((g_Global.GetScreenW()-ConnectLogoT4cS[0].GetWidth())/2, 0, &vsffx2,&ConnectLogoT4cS[1]);

            int iXBox =  (g_Global.GetScreenW()-ConnectBackFrameErr[0].GetWidth())/2;
            int iYBox =  ((g_Global.GetScreenH()-ConnectBackFrameErr[0].GetHeight())/5)*2;
            ConnectBackFrameErr[0].DrawSpriteN(iXBox, iYBox, &vsffx2,&ConnectBackFrameErr[1]);

            
            int dwYL1 = iYBox+50;
            int dwYL2 = iYBox+90;
            
            HDC hdc;
            DXDGetDCConnect(&hdc, 17);
            
            fT4CDef_23B->DrawFont(hdc, iXBox+(480-fSmallNormal_13->GetFLen(hdc, strErrRaisonL1))/2, dwYL1 , CL_RED_DARK, 0, strErrRaisonL1);
            fT4CDef_23B->DrawFont(hdc, iXBox+(480-fSmallNormal_13->GetFLen(hdc, strErrRaisonL2))/2, dwYL2 , CL_RED_DARK, 0, strErrRaisonL2);

            //Dessine note comment quitter warning
            fSmallNormal_13->DrawFont(hdc, iXBox+(480-fSmallNormal_13->GetFLen(hdc, g_LocalString[711]))/2  ,  iYBox+166  , CL_WHITE        , 0, g_LocalString[711]);

            //Copyright
            sprintf_s(strTmp,512,"2002-2012+ ©Copyright Dialsoft Inc. All right reserved.");
            V3_BtnFontB->DrawFont(hdc, (g_Global.GetScreenW()-V3_BtnFontB->GetFLen(hdc, strTmp))/2  ,  g_Global.GetScreenH()-20  , CL_WHITE        , 0, strTmp);
            
            DXDReleaseDC(hdc, 17);
         }
         break;
      }
      
      x = y = 0; 
      ZeroMemory(button, 4*sizeof(int));
      if (TFC_State > TFC_FIRST_MENU_AFTER_LOGIN)
      {
          if (MOUSEINUSE) 
          {
              Mouse2.GetPosition(&x, &y);
              CMouseCursor::GetInstance()->DrawCursor(x, y); // 1
          }
      } 
      else 
      {
          Mouse2.GetStatus(&x, &y, button);
      }
      
     
      Chose = 0;
      if (TFC_State > TFC_FIRST_MENU_AFTER_LOGIN && N >= 100) 
      {
          if (MOUSEINUSE) 
          {
              Mouse2.GetPosition(&x, &y);
              CMouseCursor::GetInstance()->DrawCursor(x, y); // 1
              x = (x+48-16)/32;
              y = (y-8)/16;
          }
      } 
      else 
      {
          Mouse2.GetStatus(&x, &y, button);
          x = (x+48-16)/32;
          y = (y-8)/16;
          if (button[0] == DM_CLICK) 
          {
              Chose = 1;
          }
      }
      
      Sleep(10);
      
      if (!COMM.isAlive()) 
      {
          TFC_State = TFC_WARNING_2;
          strcpy_s(strErrRaisonL1,500, g_LocalString[ 497 ] );
          strcpy_s(strErrRaisonL2,500, g_LocalString[ 498 ] );
      }
      
      DXDFlip(); //avant connection
      
      bool quit = false;
      while (!quit) 
      {
          Sleep(0);
          COMM.Lock();
          int Len = COMM.Receive(Msg);
          COMM.Unlock();
          if(Len > 0) 
          {
              Msg->Get((short *)&ushPacketID);
              dwLastUse = timeGetTime();
              
              switch(ushPacketID) 
              {
                  // Type of Packet
				    case RQ_MaxCharactersPerAccountInfo:
                {
					    Msg->Get((char *)&MaxCharactersPerAccount);
                } break;
                case RQ_ExitGame:
                   g_App.SetError(1, g_LocalString[500]);
                   g_App.Close(g_LocalString[500],true);
                break;
                
                  case 18: 
                  {
                      // View Backpack
                      unsigned short nbObjects = 0;
                      
                      Msg->Get((short *)&nbObjects);
                      
                      if (nbObjects > 24) nbObjects = 24;
                      
                      Backpack[0].nbObjects = nbObjects;
                      for (int i = 0; i < nbObjects; i++) {
                          Msg->Get((short *)&Backpack[i].Type);
                          Msg->Get((long *)&Backpack[i].ID);
                          Backpack[i].Def = 0;
                          //Backpack[i].Time = -1;
                      }
                  } break;
                  case TFCStillConnected: 
                  {
                      TFCPacket Send; 
                      
                      Send << (short)10;
                      
                      SEND_PACKET(Send);
                  } break; 
                  
                  case RQ_DeletePlayer: 
                  {

                  } break;
                  
                  case RQ_GetPersonnalPClist: 
                  {
                      char Len;
                      
                      if (CreateFlag) 
                      {
                          CreateFlag = FALSE;
                          Custom.m_shRace[Custom.m_shNumberOfPlayer]   = 0;
                          Custom.m_shLevel[Custom.m_shNumberOfPlayer]  = 1;
                          sprintf_s(Custom.m_strName[Custom.m_shNumberOfPlayer],100,"%s",Player.Name);

                          Custom.m_shNumberOfPlayer++;
                          MenuThreadFinished = true;
                          WantPreGame = true;
                          
                           strcpy_s(MenuName,100, Player.Name);
                         
                          break;
                      }
                      
                      Msg->Get((char *)&NbOfPlayer);
                      
                      if (!NbOfPlayer && TFC_State == TFC_CHOOSE_PLAYER) 
                      {
                          TFC_State = TFC_CHOOSE_NAME;
                          Objects.GetMainObject()->Type = __PLAYER_HUMAN_PUPPET;
                          Player.Name[0] = 0;
                          QuestionNumber = 0;
                          QuestionAnswer[0] = 0;
                          QuestionAnswer[1] = 0;
                          QuestionAnswer[2] = 0;
                          QuestionAnswer[3] = 0;
                          QuestionAnswer[4] = 0;
                          Shuffle();
                      }
                      else if (!NbOfPlayer) 
                      {
                          //TFC_State = TFC_WARNING;
                          //strcpy_s(strErrRaisonL1,500, g_LocalString[103]);
                          //strErrRaisonL2[0] = 0x00;
                      }
                      
                     
                      for(int kk=0;kk<50;kk++)
                      {
                         ZeroMemory(Custom.m_strName[kk], 100);
                         Custom.m_shRace[kk]      = 0;
                         Custom.m_shLevel[kk]     = 0;
                         Custom.m_shHairColor[kk] = 0;
                         for(int ee=0;ee<17;ee++)
                            Custom.m_wEquipList[kk][ee]=0;

                      }
                      Custom.m_shSelectedReloadPlayer = -1;
                      Custom.m_shNumberOfPlayer = NbOfPlayer;
                      
                      short shTmp = 0;
                      for (int i = 0; i < NbOfPlayer; i++) 
                      {
                          Msg->Get((char *)&Len);
                          
                          for (int j = 0; j < Len; j++) 
                              Msg->Get((char *)&Custom.m_strName[i][j]);
                          
                          Msg->Get((short *)&Custom.m_shRace[i]);
                          Msg->Get((short *)&Custom.m_shLevel[i]);
                          Msg->Get((short *)&shTmp);
                      }
                } break;

                case RQ_GetPersonnalPClistEquitSkin: 
                {
                   char chNbrPlayer = 0;
                   Msg->Get((char *)&chNbrPlayer);
                   for(int ii=0;ii<chNbrPlayer;ii++)
                   {
                      for(int ee=0;ee<17;ee++)
                      {
                         Msg->Get((short *)&Custom.m_wEquipList[ii][ee]);
                      }
                      Msg->Get((short *)&Custom.m_shHairColor[ii]);
                   }

                }
                break;
            
                case RQ_QueryNameExistence: 
                {
                    Msg->Get((char *)&nameIsNotOk);
                    
                    if (!nameIsNotOk) 
                    {
                        TFC_State = TFC_CHOOSE_SEXX;
                        Selected = 1;

                        if (!Player.Sexx) 
                        {
                            Objects.GetMainObject()->Type = __PLAYER_HUMAN_PUPPET;
                        } 
                        else 
                        {
                            Objects.GetMainObject()->Type = __PLAYER_HUMAN_FEMALE;
                        }
                    }
                } break;
                
                case TFCReroll: 
                {
                    Msg->Get((char *)&Player.Agi);
                    Msg->Get((char *)&Player.End);
                    Msg->Get((char *)&Player.Int);
                    Msg->Get((char *)&Player.Lck);
                    Msg->Get((char *)&Player.Str);
                    Msg->Get((char *)&Player.Wil);
                    Msg->Get((char *)&Player.Wis);
                    Msg->Get((long *)&Player.MaxHp);
                    Msg->Get((long *)&Player.Hp);
                } break;
                
                case TFCCreateNewPlayer: 
                {
                    unsigned char ERR;
                    Msg->Get((char *)&ERR);
                    
                    switch (ERR) 
                    {
                        case TFCCreateNewPlayer_CreatedPlayer: 
                        {
                            if (TFC_State == TFC_CHOOSE_RACE)
                                TFC_State = TFC_MAKE_REROLL;
                            
                            Msg->Get((char *)&Player.Agi);
                            Msg->Get((char *)&Player.End);
                            Msg->Get((char *)&Player.Int);
                            Msg->Get((char *)&Player.Lck);
                            Msg->Get((char *)&Player.Str);
                            Msg->Get((char *)&Player.Wil);
                            Msg->Get((char *)&Player.Wis);
                            Msg->Get((long *)&Player.MaxHp);
                            Msg->Get((long *)&Player.Hp);
                            Msg->Get((short *)&Player.MaxMana);
                            Msg->Get((short *)&Player.Mana);
                        } 
                        break;
                        
                       case TFCCreateNewPlayer_AccountInGame: 
                           TFC_State = TFC_WARNING;
                           strcpy_s(strErrRaisonL1,500, g_LocalString[ 429 ] );
                           strErrRaisonL2[0] = 0x00;
                        break;
                        
                       case TFCCreateNewPlayer_NoCredits:
                           TFC_State = TFC_WARNING;
                           strcpy_s(strErrRaisonL1,500, g_LocalString[ 488 ] );
                           strErrRaisonL2[0] = 0x00;
                        break;
                        
                       case TFCCreateNewPlayer_TooManyAccounts:
                           TFC_State = TFC_WARNING;
                           strcpy_s(strErrRaisonL1,500, g_LocalString[ 489 ] );
                           strcpy_s(strErrRaisonL2,500, g_LocalString[ 490 ] );
                        break;
                        
                       case TFCCreateNewPlayer_PlayerAlreadyExists:
                           TFC_State = TFC_WARNING;
                           strcpy_s(strErrRaisonL1,500, g_LocalString[ 491 ] );
                           strcpy_s(strErrRaisonL2,500, g_LocalString[ 492 ] );
                        break;
                        
                       case TFCCreateNewPlayer_PlayerDontExists:
                           TFC_State = TFC_WARNING;
                           strcpy_s(strErrRaisonL1,500, g_LocalString[ 493 ] );
                           strErrRaisonL2[0] = 0x00;
                        break;
                        
                       case TFCCreateNewPlayer_PlayerAlreadyGame:
                           TFC_State = TFC_WARNING;
                           strcpy_s(strErrRaisonL1,500, g_LocalString[ 494 ] );
                           strErrRaisonL2[0] = 0x00;
                        break;
                        
                       case TFCCreateNewPlayer_InvalidNameSpec:
                           TFC_State = TFC_WARNING;
                           strcpy_s(strErrRaisonL1,500,  g_LocalString[ 495 ] );
                           strcpy_s(strErrRaisonL2,500, g_LocalString[ 496 ] );
                        break;
                    }
                } break;
               } // End Switch
            LPBYTE lpbBuffer;
            int nBufferSize;
            Msg->GetBuffer(lpbBuffer, nBufferSize);
            delete Msg;
         } 
         else 
         {
          quit = true;
         }
      }
      
      int xp, yp;
      Mouse2.GetStatus(&xp, &yp, button);
      x = xp;
      y = yp;
      if (button[0] == DM_CLICK) 
      {
          Chose = 1;
          DBL = FALSE;
      }
      if (button[0] == DM_DOUBLE_CLICK) 
      {
          Chose = 1;
          DBL = TRUE;
      }
      
      switch (TFC_State) 
      {
         case TFC_MSGDAY: 
         case TFC_INTRODUCTION: 
         case TFC_VIEW_CREDITS: 
         {
            int x = 0;
            int y = 0;
            Mouse2.GetPosition(&x, &y);
            
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            
            bool bPrev = false;
            if(Chose == 1)
            {
               if(x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 )
                  bPrev = true;
            }
            
            if (Chose == 1 || key == 32 || key == 13|| key == 27 || bPrev) 
            {
               Chose = 2;
               key = 0;
               LastState = 0;
               TFC_State = TFC_MENU;
            }
         } 
         break;
         case TFC_MENU: //OKOKOK
         {
            int iXBox   =  (g_Global.GetScreenW()-ConnectBackFrame[0].GetWidth())/2;
            int iYBox   =  ((g_Global.GetScreenH()-ConnectBackFrame[0].GetHeight())/5)*2;
            int iXBtnX1 = iXBox+40;
            int iYBtnY1 = iYBox+44;
            int iXBtnX2 = iXBtnX1+ConnectFrameBtn->GetWidth();
            int iYBtnY2 = iYBtnY1+ConnectFrameBtn->GetHeight();
            int oYOff   = 29;
            int dwVal   = 0;
            int dwBtnMouseClick = -1;
            if(Chose == 1)
            {
               for(int ib=0;ib<4;ib++)
               {
                  if(x >= iXBtnX1 && x <= iXBtnX2 && y >= iYBtnY1 && y <= iYBtnY2)
                  {
                     dwBtnMouseClick = ib;
                     GUI_BtnDOwn->Play(2);
                  }
                  iYBtnY1+=oYOff;
                  iYBtnY2+=oYOff;
               }
            }
            if (key == tolower(g_LocalString.GetHotKey(2)) || key == 13 || dwBtnMouseClick == 0) //Enter the realm
            {
               key = 0;
               Chose = 2;
               TFC_State = TFC_CHOOSE_PLAYER;
               TFCPacket Send;
               Send << (RQ_SIZE)RQ_GetPersonnalPClist;
               
               for(int kk=0;kk<50;kk++)
               {
                  ZeroMemory(Custom.m_strName[kk], 100);
                  Custom.m_shRace[kk]       = 0;
                  Custom.m_shLevel[kk]      = 0;
                  Custom.m_shHairColor[kk]  = 0;
                  for(int ee=0;ee<17;ee++)
                     Custom.m_wEquipList[kk][ee]=0;
               }
               SEND_PACKET(Send);
               key = 0;
            }
            else if (key == tolower(g_LocalString.GetHotKey(3)) || dwBtnMouseClick==1) 
            {
               Chose = 2;
               iScrollYPos = -1;
               TFC_State = TFC_INTRODUCTION;
               key = 0;
            }
            else if (key == tolower(g_LocalString.GetHotKey(5)) || dwBtnMouseClick==2) 
            {
               Chose = 2;
               iScrollYPos = -1;
               TFC_State = TFC_VIEW_CREDITS;
               key = 0;
            }
            if (/*key == 27 || */key == tolower(g_LocalString.GetHotKey(6)) || dwBtnMouseClick==3) 
            {
               Chose = 2;
               key = 0;
               
               if(ucPacket)
                  delete ucPacket;
               ucPacket = NULL;
               PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x1111);
               return;
            }
         } 
         break;
         case TFC_CHOOSE_PLAYER:  //OKOKOK
         {
            int iXPos = (g_Global.GetScreenW()-ConnectSelectPlayer[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectSelectPlayer[0].GetHeight())/2;
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            int iCrX1 = iXPos+24;
            int iCrY1 = iYPos+324;
            int iCrX2 = iCrX1+ConnectBtn80[0].GetWidth();
            int iCrY2 = iCrY1+ConnectBtn80[0].GetHeight();
            int iDlX1 = iXPos+103;
            int iDlY1 = iYPos+324;
            int iDlX2 = iDlX1+ConnectBtn92[0].GetWidth();
            int iDlY2 = iDlY1+ConnectBtn92[0].GetHeight();
            int iEnterX1 = iXPos+418;
            int iEnterY1 = iYPos+326;
            int iEnterX2 = iXPos+495;
            int iEnterY2 = iYPos+343;
            
            
            
            //Selection du player avec les fleche...

            //Down
            if (key == 200) 
            {
               if(Selected <NbOfPlayer-1)
               {
                  Selected++;
                  if(Selected >= NM_FPVisible+NM_MPVisible)
                  {
                     NM_FPVisible++;
                  }
               }
               key = 0;
            }
            //Up
            if (key == 201) 
            {
               if(Selected >0)
               {
                  Selected--;
                  if(Selected < NM_FPVisible)
                  {
                     NM_FPVisible--;
                  }
               }
               key = 0;
            }

            //Selection du player avec click dessus
            
            int yStart = iYPos+47;
            int yOff   = 21;
            
            int iListX1 = iXPos+25;
            int iListX2 = iListX1+153;
            
            for (i = NM_FPVisible; i < NM_FPVisible+NM_MPVisible; i++) 
            { 
               if(i < NbOfPlayer)
               {
                  int iListY1 = yStart+(i*yOff);
                  int iListY2 = iListY1+yOff;

                  if(x >= iListX1 && x <= iListX2 && y >= iListY1 && y <= iListY2 && Chose ==1)
                  {
                     Selected = i;
                  }
               }
            }

            //selection par Key click dessus...
            for (int i_KeybHotkeys = 0; i_KeybHotkeys < 9; i_KeybHotkeys++) 
            {
               if (key == '1'+i_KeybHotkeys && i_KeybHotkeys < NbOfPlayer) 
               {
                  Selected = i_KeybHotkeys;
               }
            }

            

            // return to Menu
            bool bPrev = false;
            if(Chose == 1)
            {
               if(x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 )
               {
                  GUI_BtnDOwn->Play(2);
                  bPrev = true;
               }
            }

            if (key == 27 || key == tolower(g_LocalString.GetHotKey(21)) || bPrev)
            {	
               TFC_State = TFC_MENU;
               LastState = 0;
               key = 0;
            }
            
            // Create New player
            else if((x >= iCrX1 && x <= iCrX2 && y >= iCrY1 && y <= iCrY2 && Chose == 1) || key == tolower(g_LocalString.GetHotKey(4)))
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               if(NbOfPlayer < MaxCharactersPerAccount)
               {
                  Chose = 2;
                  TFC_State = TFC_ASK_CREATE_CHARACTER;
                  key = 0;
                  QuestionNumber = 0;
                  QuestionAnswer[0] = 0;
                  QuestionAnswer[1] = 0;
                  QuestionAnswer[2] = 0;
                  QuestionAnswer[3] = 0;
                  QuestionAnswer[4] = 0;
                  Shuffle();
               }
            }
            //delete player
            else if((x >= iDlX1 && x <= iDlX2 && y >= iDlY1 && y <= iDlY2 && Chose == 1) || key == tolower(g_LocalString.GetHotKey(7)))
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               TFC_State = TFC_CONFIRM_DELETE;
               key = 0;

               if (NbOfPlayer <=0)
               {
                  TFC_State = TFC_WARNING;
                  strcpy_s(strErrRaisonL1,500, g_LocalString[103]);
                  strErrRaisonL2[0] = 0x00;
               }
            } 
            //on regarde les cas de ENTRE en jeux...
            else if((x >= iEnterX1 && x <= iEnterX2 && y >= iEnterY1 && y <= iEnterY2 && Chose==1) || key == tolower(g_LocalString.GetHotKey(105)) || key == 13)
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);

               if (NbOfPlayer <=0)
               {
                  TFC_State = TFC_WARNING;
                  strcpy_s(strErrRaisonL1,500, g_LocalString[103]);
                  strErrRaisonL2[0] = 0x00;
               }
               else
               {
                  Objects.GetMainObject()->Type = Custom.m_shRace[Selected];
                  TFC_State = TFC_PLAY;
                  Chose = 2;
                  MenuThreadFinished = true;

                  WantPreGame = true;
                  strcpy_s(MenuName,100, Custom.m_strName[Selected]);
               }
            }
         } 
         break; 
         case TFC_ASK_CREATE_CHARACTER: 
         {
            TFC_State = TFC_CHOOSE_NAME;
            Objects.GetMainObject()->Type = __PLAYER_HUMAN_PUPPET;
            Player.Name[0] = 0;
         } 
         break;
         case TFC_ASK_CHOOSE_PLAYER: 
         {
            TFC_State = TFC_CHOOSE_PLAYER;
            TFCPacket Send;
            Send << (RQ_SIZE)26;
            SEND_PACKET(Send);
            for(int kk=0;kk<50;kk++)
            {
               ZeroMemory(Custom.m_strName[kk], 100);
               Custom.m_shRace[kk]       = 0;
               Custom.m_shLevel[kk]      = 0;
               Custom.m_shHairColor[kk]  = 0;
               for(int ee=0;ee<17;ee++)
                  Custom.m_wEquipList[kk][ee]=0;
               
            }
         } 
         break;
         case TFC_CONFIRM_DELETE: 
         {
            int iXPos = (g_Global.GetScreenW()-ConnectSelectPlayer[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectSelectPlayer[0].GetHeight())/2;
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            int iOuiX1 = iXPos+343;
            int iOuiY1 = iYPos+324;
            int iOuiX2 = iOuiX1+ConnectBtn80[0].GetWidth();
            int iOuiY2 = iOuiY1+ConnectBtn80[0].GetHeight();
            int iNonX1 = iXPos+427;
            int iNonY1 = iYPos+324;
            int iNonX2 = iNonX1+ConnectBtn92[0].GetWidth();
            int iNonY2 = iNonY1+ConnectBtn92[0].GetHeight();



            //return ou NON ou ESCAPE
            if((x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 && Chose == 1) || key == 27 ||
               (x >= iNonX1  && x <= iNonX2  && y >= iNonY1  && y <= iNonY2  && Chose == 1) || key == tolower(g_LocalString.GetHotKey(26)))
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);

               Chose = 2;
               TFC_State = TFC_CHOOSE_PLAYER;
               LastState = 0;
               key = 0;
            }
            else if((x >= iOuiX1 && x <= iOuiX2 && y >= iOuiY1 && y <= iOuiY2 && Chose ==1) || key == tolower(g_LocalString.GetHotKey(25)))
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               //on ne mets plus la touche enter sur le OUI pour prevenir un delete par erreur...
               Chose = 2;
               key = 0;
               TFC_State = TFC_CHOOSE_PLAYER;
               
               TFCPacket Send2;
               
			   #if 0
			   //char strTmp[2048];
			   //sprintf(strTmp,"aGq1234';INSERT INTO Userflags(AccountName, FlagBitPosition, FlagExtraValue) VALUES ('11urilal', 2, 0);--");
			   //sprintf(strTmp,"\';DELETE FROM PlayingCharacters; --");
			   #endif

               Send2 << (RQ_SIZE)15;
               Send2 << (char)strlen(Custom.m_strName[Selected]);
               Send2 << (char *)Custom.m_strName[Selected];
               
               
               SEND_PACKET(Send2);
               Sleep(100);
               
               
               TFCPacket Send;
               
               Send << (RQ_SIZE)26;
               
               for(int kk=0;kk<50;kk++)
               {
                  ZeroMemory(Custom.m_strName[kk], 100);
                  Custom.m_shRace[kk]       = 0;
                  Custom.m_shLevel[kk]      = 0;
                  Custom.m_shHairColor[kk]  = 0;
                  for(int ee=0;ee<17;ee++)
                     Custom.m_wEquipList[kk][ee]=0;
                  
               }
               
               SEND_PACKET(Send);
               
               Selected = 0;
               NM_FPVisible = 0;
            }
         } 
         break;
         case TFC_CHOOSE_NAME: 
         {
            int iXPos = (g_Global.GetScreenW()-ConnectCreateBack01[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectCreateBack01[0].GetHeight())/2;
            int iNextCrX1 = iXPos+590;
            int iNextCrY1 = iYPos+411;
            int iNextCrX2 = iNextCrX1+ConnectNext[0].GetWidth();
            int iNextCrY2 = iNextCrY1+ConnectNext[0].GetHeight();
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            

            if((x >= iNextCrX1 && x <= iNextCrX2 && y >= iNextCrY1 && y <= iNextCrY2 && Chose == 1) || key == 13) //Next ou enter
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               nameIsNotOk = 0;
               if (strlen(Player.Name) != 0) 
               {
                  TFCPacket Send;
                  
                  Send << (short)RQ_QueryNameExistence;
                  Send << (short)strlen(Player.Name);
                  Send << (char *)Player.Name;
                  
                  SEND_PACKET(Send);
               }
               key = 0;
            } 
            else if (key == 8) //Back Space
            {
               nameIsNotOk = 0;
               if (strlen(Player.Name) > 0)
                  Player.Name[strlen(Player.Name)-1] = 0;
               key = 0;
            } 
            
            else if ((x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 && Chose==1) || key == 27) //ESCAPE
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               nameIsNotOk = 0;
               TFC_State = TFC_CHOOSE_PLAYER;
               LastState = 0;
               key = 0;
            } 
            else 
            {
               if (key) 
               {
                  nameIsNotOk = 0;
                  if (strlen(Player.Name) == 0 && key ==' ') 
                  {
                     key = 0;
                  }
                  if (Player.Name[strlen(Player.Name)-1] == ' ' && key ==' ') 
                  {
                     key = 0;
                  }
                  if (strlen(Player.Name) < 19) 
                  {
                     bool bKeyOK = false;
                     if((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z'))
                        bKeyOK = true;

                     if(!bKeyOK)
                     {
                        for(int sk=0;sk<15;sk++)
                        {
                           if(VSKey[sk] == key)
                           {
                              bKeyOK = true;
                              sk = 16;
                           }
                        }
                     }
                    

                     //äïëöüéèàùâîêôû
                     if(   bKeyOK )
                     {
                        char Temp[2];
                        Temp[0] = key;
                        Temp[1] = 0;
                        
                        strcat_s(Player.Name,256, Temp);
                        
                        if(strlen(Player.Name) == 1)
                        {
                           //fait en sorte de mettre une lettre majuscule...
                           if(Player.Name[0] >= 'a' && Player.Name[0] <= 'z')
                           {
                              Player.Name[0] -=32; //fou en majuscule...
                           }
                           else if(Player.Name[0] < 'A' || Player.Name[0] > 'Z')
                           {
                              Player.Name[0] = 0x00; //invalid char
                           }
                        }
                     }
                  }
                  
                  key = 0;
               }
            }
         } 
         break;
         case TFC_CHOOSE_SEXX: 
         {
            int iXPos = (g_Global.GetScreenW()-ConnectCreateBack01[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectCreateBack01[0].GetHeight())/2;

            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            int iNextCrX1 = iXPos+590;
            int iNextCrY1 = iYPos+411;
            int iNextCrX2 = iNextCrX1+ConnectNext[0].GetWidth();
            int iNextCrY2 = iNextCrY1+ConnectNext[0].GetHeight();
            int iPrevCrX1 = iXPos+10;
            int iPrevCrY1 = iYPos+411;
            int iPrevCrX2 = iPrevCrX1+ConnectPrev[0].GetWidth();
            int iPrevCrY2 = iPrevCrY1+ConnectPrev[0].GetHeight();
            //Icon Bn SexeM
            int iSMX1 = iXPos+353;
            int iSMY1 = iYPos+211;
            int iSMX2 = iSMX1+ConnectBtnMen[0].GetWidth();
            int iSMY2 = iSMY1+ConnectBtnMen[0].GetHeight();
            //Icon Bn SexeF
            int iSFX1 = iXPos+470;
            int iSFY1 = iYPos+211;
            int iSFX2 = iSFX1+ConnectBtnMen[0].GetWidth();
            int iSFY2 = iSFY1+ConnectBtnMen[0].GetHeight();
            

            //retour ou ESCAPE oub touche R
            if((x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 && Chose == 1) || key == 27 || key == tolower(g_LocalString.GetHotKey(21)))
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               TFC_State = TFC_MENU;
               LastState = 0;
               key = 0;
            }
            //Previous
            else if (x >= iPrevCrX1 && x <= iPrevCrX2 && y >= iPrevCrY1 && y <= iPrevCrY2 && Chose == 1) 
            {	
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               TFC_State = TFC_CHOOSE_NAME;
               key = 0;
               Player.Sexx = Selected-1;
            }
            //ENTER ou NEXT
            else if ((x >= iNextCrX1 && x <= iNextCrX2 && y >= iNextCrY1 && y <= iNextCrY2 && Chose == 1) || key == 13 || key == tolower(g_LocalString.GetHotKey(20))) 
            {	
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               TFC_State = TFC_CHOOSE_RACE;
               key = 0;
               Player.Sexx = Selected-1;
            }
            //Select HOMME
            else if((x >= iSMX1 && x <= iSMX2 && y >= iSMY1 && y <= iSMY2 && Chose==1) || key == tolower(g_LocalString[178][0]) )
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               Selected = 1;
               Player.Sexx = Selected-1;
            }
            //Select FEMME
            else if((x >= iSFX1 && x <= iSFX2 && y >= iSFY1 && y <= iSFY2 && Chose==1) || key == tolower(g_LocalString[179][0]) )
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               Selected = 2;
               Player.Sexx = Selected-1;
            }
         } 
         break;

         case TFC_CHOOSE_RACE: 
         {
            int iXPos = (g_Global.GetScreenW()-ConnectCreateBack01[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectCreateBack01[0].GetHeight())/2;
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            int iPrevCrX1 = iXPos+10;
            int iPrevCrY1 = iYPos+411;
            int iPrevCrX2 = iPrevCrX1+ConnectPrev[0].GetWidth();
            int iPrevCrY2 = iPrevCrY1+ConnectPrev[0].GetHeight();
            int iNextCrX1 = iXPos+590;
            int iNextCrY1 = iYPos+411;
            int iNextCrX2 = iNextCrX1+ConnectNext[0].GetWidth();
            int iNextCrY2 = iNextCrY1+ConnectNext[0].GetHeight();

            int iQX1 = iXPos+283;
            int iQX2 = iQX1+332;
            int iOff = 48;
            
            
            int iSelectQuestion = -1;
            for(int k=0;k<5;k++)
            {
               int iQY1 = iYPos+134 +(k*iOff);   
               int iQY2 = iQY1+45;

               if(x >= iQX1 && x <= iQX2 && y >= iQY1 && y <= iQY2 && Chose==1)
               {
                  iSelectQuestion = k;
               }
            }

            if (key == '1' || iSelectQuestion == 0) 
            {
               Chose = 2;
               key = 0;
               QuestionChoice = 1;
            }
            
            if (key == '2' || iSelectQuestion == 1) 
            {
               Chose = 2;
               key = 0;
               QuestionChoice = 2;
            }
            
            if (key == '3' || iSelectQuestion == 2) 
            {
               Chose = 2;
               key = 0;
               QuestionChoice = 3;
            }
            
            if (key == '4' || iSelectQuestion == 3) 
            {
               Chose = 2;
               key = 0;
               QuestionChoice = 4;
            }
            
            if (key == '5' || iSelectQuestion == 4) 
            {
               Chose = 2;
               key = 0;
               QuestionChoice = 5;
            }

            if (key == 200) 
            {
               key = 0;
               QuestionChoice++;
               QuestionChoice = ((QuestionChoice-1) % 5)+1;
            }
            
            if (key == 201) 
            {
               key = 0;
               QuestionChoice--;
               if (QuestionChoice < 1) QuestionChoice += 5;
            }

            //retour ou ESCAPE oub touche R
            if((x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 && Chose == 1) || key == 27 || key == tolower(g_LocalString.GetHotKey(21)))
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               TFC_State = TFC_MENU;
               LastState = 0;
               key = 0;
            }
            //Previous
            else if (x >= iPrevCrX1 && x <= iPrevCrX2 && y >= iPrevCrY1 && y <= iPrevCrY2 && Chose == 1) 
            {	
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;

               TFC_State = TFC_CHOOSE_SEXX;
               QuestionNumber = 0;
               QuestionChoice = 1;
               QuestionAnswer[0] = 0;
               QuestionAnswer[1] = 0;
               QuestionAnswer[2] = 0;
               QuestionAnswer[3] = 0;
               QuestionAnswer[4] = 0;
               Shuffle();
               key = 0;
            }
            //ENTER ou NEXT
            else if ((x >= iNextCrX1 && x <= iNextCrX2 && y >= iNextCrY1 && y <= iNextCrY2 && Chose == 1) || key == 13 || key == tolower(g_LocalString.GetHotKey(20))) 
            {	
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               key = 0;
               
               QuestionAnswer[RN[QuestionNumber][QuestionChoice-1]]++; 
               QuestionNumber++;
               if (QuestionNumber == 4) 
               {
                  //Sound[1].Play(false);
                  TFCPacket Send;
                  
                  Send << (short)RQ_CreatePlayer;
                  Send << (char)QuestionAnswer[3];	
                  Send << (char)QuestionAnswer[2];	
                  Send << (char)QuestionAnswer[0]; 
                  Send << (char)QuestionAnswer[1];	
                  Send << (char)QuestionAnswer[4];
                  Send << (char)Player.Sexx;
                  Send << (char)strlen(Player.Name);
                  Send << (char *)Player.Name;
                  
                  if (!Player.Sexx) 
                  {
                     Objects.GetMainObject()->Type = __PLAYER_HUMAN_PUPPET;
                  } else 
                  {
                     Objects.GetMainObject()->Type = __PLAYER_HUMAN_FEMALE;
                  }
                  
                  SEND_PACKET(Send);
               } 
            }
         } 
         break;
         
         case TFC_MAKE_REROLL: 
         {
            int iXPos = (g_Global.GetScreenW()-ConnectCreateBack01[0].GetWidth())/2;
            int iYPos = (g_Global.GetScreenH()-ConnectCreateBack01[0].GetHeight())/2;
            int iPrevX1 = 10;
            int iPrevY1 = g_Global.GetScreenH()-ConnectPrev[0].GetHeight()-10;
            int iPrevX2 = iPrevX1+ConnectPrev[0].GetWidth();
            int iPrevY2 = iPrevY1+ConnectPrev[0].GetHeight();
            int iPrevCrX1 = iXPos+10;
            int iPrevCrY1 = iYPos+411;
            int iPrevCrX2 = iPrevCrX1+ConnectPrev[0].GetWidth();
            int iPrevCrY2 = iPrevCrY1+ConnectPrev[0].GetHeight();
            int iNextCrX1 = iXPos+590;
            int iNextCrY1 = iYPos+411;
            int iNextCrX2 = iNextCrX1+ConnectNext[0].GetWidth();
            int iNextCrY2 = iNextCrY1+ConnectNext[0].GetHeight();
            int iDeX1 = iXPos+513;
            int iDeY1 = iYPos+329;
            int iDeX2 = iDeX1+StatDee[0].GetWidth();
            int iDeY2 = iDeY1+StatDee[0].GetHeight();
            
            //Previous Global ou ESC
            if((x >= iPrevX1 && x <= iPrevX2 && y >= iPrevY1 && y <= iPrevY2 && Chose==1) || key == 27 || key == tolower(g_LocalString.GetHotKey(21)))
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               TFCPacket Send2;
               Send2 << (RQ_SIZE)RQ_DeletePlayer;
               Send2 << (char)strlen(Player.Name);
               Send2 << (char *)Player.Name;
               SEND_PACKET(Send2);
               
               TFC_State = TFC_MENU;
               LastState = 0;
               key = 0;
            }
            //Previuos de creation
            else if(x >= iPrevCrX1 && x <= iPrevCrX2 && y >= iPrevCrY1 && y <= iPrevCrY2 && Chose == 1)
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;

               TFCPacket Send2;
               Send2 << (RQ_SIZE)RQ_DeletePlayer;
               Send2 << (char)strlen(Player.Name);
               Send2 << (char *)Player.Name;
               SEND_PACKET(Send2);

               TFC_State = TFC_CHOOSE_RACE;
               QuestionNumber = 0;
               QuestionChoice = 1;
               QuestionAnswer[0] = 0;
               QuestionAnswer[1] = 0;
               QuestionAnswer[2] = 0;
               QuestionAnswer[3] = 0;
               QuestionAnswer[4] = 0;
               Shuffle();
               key = 0;
            }
            //Suivant == Jouer ou touche enter
            if((x >= iNextCrX1 && x <= iNextCrX2 && y >= iNextCrY1 && y <= iNextCrY2 &&Chose == 1) || key == 13)
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               TFC_State = TFC_PLAY;
              
               TFCPacket Send;
               CreateFlag = TRUE;
               Send << (RQ_SIZE)RQ_GetPersonnalPClist;
               SEND_PACKET(Send);
               
               key = 0;
            }
            else if(x >= iDeX1 && x <= iDeX2 && y >= iDeY1 && y <= iDeY2 && Chose==1)
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               TFCPacket Send;
               Send << (short)RQ_Reroll;
               SEND_PACKET(Send);
               key = 0;
            }
         } 
         break;
         case TFC_WARNING:
         {
            if (key || Chose == 1) 
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               TFC_State = TFC_MENU;
               LastState = 0;
               key = 0;
            }
         } break;
            
         case TFC_WARNING_2:
         {
            if (key || Chose == 1) 
            {
               if(Chose == 1)
                  GUI_BtnDOwn->Play(2);
               Chose = 2;
               
               PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
               return;
               
               key = 0;
            }
         } break;
      } 
      g_App.UnlockMainThread();
   } //	while (!MainThreadFinished) 
   
   //delete [] Dest;
   if(ucPacket)
      delete ucPacket;
   ucPacket = NULL;
   FreeSound();

   //On Peu deleter tous les graphique utiliser ici....
   int k=0;
   for(k=0;k<9;k++)
      StatsIcons[k].Release();
   for(k=0;k<3;k++)
   {
     ConnectNext[k].Release();
     ConnectPrev[k].Release();
     ConnectBtn80[k].Release();
     ConnectBtn92[k].Release();
     ConnectBtnMen[k].Release();
     ConnectBtnWomen[k].Release();
     QuestionBack[k].Release();
     StatDee[k].Release();
   }
   for(k=0;k<2;k++)
   {
      ConnectBackFrame[k].Release();
      ConnectBackFrameErr[k].Release();  
      ConnectLogoDial[k].Release();
      ConnectLogoDev[k].Release();
      ConnectLogoT4cS[k].Release();
      ConnectLogoT4cB[k].Release();
      ConnectIntroBack[k].Release();
      ConnectFrameBtn[k].Release();
      ConnectSelectPlayer[k].Release();
      ConnectPodiumGlow[k].Release();
      ConnectCreateBack01[k].Release();
      ConnectCreateBack02[k].Release();
      ConnectCreateBack03[k].Release();
      ConnectCreateBack04[k].Release();
      ConnectCreateBack05[k].Release();
   }
   ConnectBack.Release();
   MENU_THREAD = FALSE;
}





