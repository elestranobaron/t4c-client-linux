// ************************************************************************
// ***                                                                  ***
//      File Name: Main.cpp
//      Project:   The 4th Coming
//      Creation:  25 Mars, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***
#include "pch.h"
#include "NMPlayVideo.h"
 
#pragma warning( disable : 4786 )
#pragma warning( disable : 4291 )

// Number used to enable Debug
#define MAGIC_DEBUG_NUMBER   8030


#define DEF_WINDOW_W 800  //1024
#define DEF_WINDOW_H 600  // 768

     
// Include Files
#ifndef  _WIN32_WINNT
#define  _WIN32_WINNT 0x0600 // for SignalObjectAndWait
#endif
#include <windows.h>
#include "vfw.h"
#include <shlobj.h>
#include "App.h"
#include "Bitmap.h"
#include "V2Database.h"
#include "DirectXInput.h"
#include "Disp.h"
#include "Ddraw.h"
#include <eh.h>
#include "NewFont.h" 
#include "Franck.h"
#include "Icon3D.h"
#include "IDirectXDraw.h"
#include "IDirectXFont.h"
#include "IDirectXImage.h"
#include "GUILocalString.h"
#include "MacroHandler.h"
#include "Mouse.h"
#include <new.h>
#include "NTime.h"
#include "ObjectListing.h"
#include "old.h"
#include "Packet.h"
#include <process.h> 
#include "Random.h"
#include <ras.h>
#include "Resource.h"
//#include "Sprite.h"
#include <stdio.h>
#include "System.h"
#include "SystemAction.h"
#include "SystemInfo.h"
#include "TFCFlag.h"
#include "TFCObject.h"
#include "TFCPlayer.h"
#include "TFCSocket.h"
#include "TileSet.h"
#include "VisualObjectList.h"
//#include "WarningBox.h"
#include <wininet.h>
#include "Global.h"
#include "C2.h"
#include "version.h"
#include "CombatCursor.h"
#include "SaveGame.h"
#include "VideoCapture.h"
#include "NMVideoCapture.h"
#include "Zlib/zlib.h"


#include "NewInterface/RootBoxUI.h"
#include "NewInterface/V3_PacmanDlg.h"
#include "NewInterface/V3_StatDlg.h"
#include "NewInterface/V3_TMIDlg.h"
#include "NewInterface/V3_GroupOSDlg.h"
#include "NewInterface/V3_EffectStatusDlg.h"
#include "NewInterface/V3_ChatLogDlg.h"
#include "NewInterface/V3_MainBarDlg.h"
#include "NewInterface/V3_ChatDlg.h"
#include "NewInterface/V3_OptionsDlg.h"
#include "NewInterface/V3_MacroDlg.h"
#include "NewInterface/V3_SpellDlg.h"
#include "NewInterface/V3_GroupeDlg.h"
#include "NewInterface/V3_InvDlg.h"
#include "NewInterface/V3_TradeDlg.h"
#include "NewInterface/V3_GuildDlg.h"
#include "NewInterface/V3_ProfessionDlg.h"
#include "NewInterface/V3_QuestBookP1.h"
#include "NewInterface/V3_QuestBookP2.h"
#include "NewInterface/V3_QuestBookP3.h"




// Scotch: Server history vars
std::vector<std::string>vServerHistory;

// Static Global
// Alt Tab
static BOOL inGameTab = FALSE; // Verify si le Code de ALT-TAB est utiliser pour un 
// ATL-TAB ou pour entrer dans le jeux.
static BOOL bGetID = FALSE;	 // Stock de ALT-TAB que j'vas modifer 
static DWORD dwGetID = 0;		 // Stock de ALT-TAB que j'vas modifier
int Tabbing = 1;					 // Le Tabbing c le nb de fois que t'a fait ALT-TAB
// Thread
HANDLE hMaintenanceThread = NULL; // Ca sert a-peu-pres juste pour le alt-tab.
extern DWORD TargetID;

// Global                      
// Exception Filter
// Unsorted
extern unsigned char key;
static MMRESULT timerid;
extern HANDLE hDrawThread;
extern HANDLE hMouseThread;
extern HANDLE hCDThread;
extern bool MenuThreadFinished;
int CTRL_State = 0;
bool SHIFT_State = false;
HWND g_hwnd;
Random rnd(10, 20);
static Random rnd2(0, 5);
extern BACK Backpack[24];
Bitmap Grid;
Bitmap GridID;
Bitmap GridPriority;
Bitmap GridBlocking;
T3VSBSound SoundFX[250];
T3VSBSound ConnectSoundFX;
TFCSocket     Remote;
unsigned char msglist = 0;
volatile unsigned long Round = 0;
int Mx, My;
double    CriticalSection::OldPriority = 9999999;
char      CriticalSection::OldName[100];
char      CriticalSection::OldID[100];
char ServerIP[100];
char ServerResol[100];
char *      ServerPort;
bool Register = false;
char TFC_State = TFC_CONNECT;
static char Try = 0;


Font *V3_BtnFontB     = NULL;
Font *fSmallNormal_12 = NULL;
Font *fSmallNormal_13 = NULL;
Font *fSmallNormal_14 = NULL;
Font *fT4CDef_18      = NULL;
Font *fT4CDef_21      = NULL;
Font *fT4CDef_22      = NULL;
Font *fT4CDef_23B     = NULL;
Font *fT4CDef_30B     = NULL;
Font *fT4CDef_50B     = NULL;
Font *fSystem_22      = NULL;


DWORD g_dwLastMacroCallScreenshot  = 0;
DWORD g_dwLastMacroCallIdentifyAll = 0;
DWORD g_dwLastMacroCallPaste       = 0;

bool ScreenDisplayThreadFinished = false;
bool MessageThreadFinished = false;
extern DirectXInput KeyBoard;
extern LPDIRECTINPUTDEVICE lpDIMouseDevice; // Direct Input Device
BOOL QuitFirstLoop = FALSE;
//extern char Text[1000];
char LastText[1000];
extern BOOL INGAME;
extern unsigned char Direction;
extern short TalkToOffset;
HWND GlobalHwnd;
char RegistrationCode[80];
BOOL MESSAGE_THREAD = TRUE;
BOOL MENU_THREAD = TRUE;
bool NoKey;
extern BOOL boKeyProcess;
extern HANDLE InterPacketEvent;
extern unsigned char *Packet;
char *MsgDay = NULL;
//char *MsgDayLast = NULL;
char  g_strGeneralMessage[100];
extern QWORD qwPacketUseCounter[150];
extern QWORD qwPacketUseTime[150];
extern QWORD qwPacketUsePeek[150];
DWORD ThreadITimeStamp = 0;
bool g_ForceDecompression = false;
static bool boMusicDecompressionCancelled = false;
extern int TalkToX, TalkToY, TalkToW;
extern DWORD TalkToID;

extern Global g_Global;
extern CSaveGame g_SaveGame;
extern NMVideoCapture g_NMVideoCapture;

extern unsigned char *g_ListData;
extern int            g_ListSize;

/// TEMP
extern bool g_DisplayLetter;
extern bool g_boWaitForShutdown;
// END TEMP

//New loading One Shoot Thread...
BOOL    g_FirstInitCreateComplete = FALSE;
void    FirstInitCreate(LPVOID pParam);

BOOL    g_bFirstLoadComplete = FALSE;
void    DrawFisrtLoadingText();
void    DrawSecondLoadingText();
void    FirstInitObject(LPVOID pParam);

BOOL    g_bSecondLoadComplete = FALSE;
void    SecondInitObject(LPVOID pParam);

BOOL    g_bSmoothFloorLoading = FALSE;
void    SmoothInitObject(LPVOID pParam);

BOOL    g_FloodCCThreadActif = FALSE;
ULONG WINAPI FloodCCThread(LPVOID pParam);


HANDLE  m_hMp3Thread       = NULL;
HANDLE  g_hMp3EventExit    = NULL;
HANDLE  g_hMp3EventNext    = NULL;
HANDLE  g_hMp3EventPrev    = NULL;
HANDLE  g_hMp3EventPlay    = NULL;
HANDLE  g_hMp3EventPaused  = NULL;
HANDLE  g_hMp3EventVol     = NULL;
HANDLE  g_hMp3EventDisable = NULL;

void    Mp3PlayerThread(LPVOID pParam);
bool    PlayMp3NewSong(int iMode,HWND &hMp3);

DWORD g_dwNbrt4cInstance = 0;
// Function
bool    WndClassInit   ( void ); // Initialisation de la Class Window.
bool    CreateWnd      ( void ); // Creation du Window.
void    InitializeTFCData(void); // Initialize all T4C Data
bool    IsNbrT4cGraterThanOne(BOOL bUpdate);


// Unsorted
BOOL DecriptVSB(BOOL, int);
DWORD GetThreadTime(HANDLE);
void ParseText(char *pText);






BOOL Save = FALSE;
BOOL Only = FALSE;
BOOL Graph = FALSE;
//BOOL Windowed = FALSE;

BYTE DlgState;

char *MsgErr = NULL;
char MsgUrl[100];

bool boolGOA = false;

char TryNb = 0;

BOOL InSocket = FALSE;
int SocketState = 0;

UINT SocketThreadID = NULL;
HANDLE SocketThreadHandle = NULL;

int StLine = 0;
int EndLine = 0;
int NbLine = 0;

extern DWORD SpellID;


//NightMare -- Add 2005-02-15
HCURSOR m_hMouseTransCursor; //Contain Window Empty (transparent) Mouse Cursor

static BOOL    g_bExitComplete = FALSE;
void    ExitThreadGame(LPVOID pParam);

static BOOL    g_bRunCommand = FALSE;
static char    g_szFileName[512];
void    RunCommandThread(LPVOID pParam);


// Code 
UINT WINAPI MaintenanceThread(LPVOID pParam)  
{
   Remote.MainThread();// Thread de Maintenance.
   _endthreadex(1);
   return 1;
}

// Used to lock some macro keys.
static unsigned int EmptyCallback( void *null )
{
    return 0;
}

static UINT StartCapture( void *null )
{
   g_NMVideoCapture.StartCapture();
   V3_ChatLogDlg::GetInstance()->AddListLogs( "", "Starting VIDEO Capture...", SYSTEM_COLOR, true, false );
   return 0;
}

static UINT EndCapture( void *null )
{
    g_NMVideoCapture.StopCapture();
    V3_ChatLogDlg::GetInstance()->AddListLogs( "", "END of VIDEO Capture", SYSTEM_COLOR, true, false );
    return 0;
}
static UINT CommitCapture( void *null )
{
    return 0;
}
static UINT ScreenShot( void *null )
{
   if(Custom.gTakeScreenShot)
      return 0;

   if(timeGetTime()-g_dwLastMacroCallScreenshot > 100)
   {
      g_dwLastMacroCallScreenshot =timeGetTime();
      Custom.gTakeScreenShot = TRUE;
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 558 ], SYSTEM_COLOR, true, false );
   }
   return 0;
} 

static UINT TakeItemReservedID( void *null )
{
	TFCPacket sending;
	sending << (RQ_SIZE)RQ_GetObjectLinkedID;
	SEND_PACKET( sending );
	return 0;
}

static UINT IdentifyAllObject( void *null )
{
   if(timeGetTime()-g_dwLastMacroCallIdentifyAll > 500)
   {
      g_dwLastMacroCallIdentifyAll =timeGetTime();
      Objects.IdentifyAll();
   }
   return 0;
}

static UINT PasteClipboard( void *null )
{
   static BOOL bAlreadyUSE = FALSE;
   if(bAlreadyUSE)
      return 0;

   if(timeGetTime()-g_dwLastMacroCallPaste > 250)
   {
      g_dwLastMacroCallPaste =timeGetTime();
      bAlreadyUSE = TRUE;
      if (OpenClipboard(NULL)) 
      {
         if (::IsClipboardFormatAvailable(CF_TEXT) || ::IsClipboardFormatAvailable(CF_OEMTEXT))
         {
            // Retrieve the Clipboard data (specifying that 
            // we want ANSI text (via the CF_TEXT value).
            HANDLE hClipboardData = GetClipboardData(CF_TEXT);
            
            // Call GlobalLock so that to retrieve a pointer
            // to the data associated with the handle returned
            // from GetClipboardData.
            char *pchData = (char*)GlobalLock(hClipboardData);
            
            //make sure the text are not more longer than 250 character
            int iLenMax = strlen(pchData)+1;
            if(iLenMax > 249)
               iLenMax = 250;

            char *strText = new char[iLenMax];
            memcpy(strText,pchData,iLenMax);

            for(UINT i=0;i<strlen(strText);i++)
            {
               if(strText[i] == '\n' || strText[i] == '\r')
                  strText[i] = 0x00;
               else
                  V3_MainBarDlg::GetInstance()->TextInput( pchData[i] );
            }
            delete []strText;
            strText = NULL;
            GlobalUnlock(hClipboardData);
         }
         CloseClipboard();
      }
      bAlreadyUSE = FALSE;
   }

   return 0;
}


void RestartT4C()
{
   char thisFile[ MAX_PATH ];
   GetModuleFileName( NULL, thisFile, MAX_PATH );
   
   string params( "" );
   
   char appDir[ MAX_PATH ];
   GetCurrentDirectory( MAX_PATH, appDir );
   
   SHELLEXECUTEINFO shellInfo;
   shellInfo.cbSize = sizeof( SHELLEXECUTEINFO );
   shellInfo.fMask  = SEE_MASK_NOCLOSEPROCESS;
   shellInfo.hwnd   = NULL;
   shellInfo.lpVerb = "open";
   shellInfo.lpFile = thisFile; 
   shellInfo.lpParameters = params.c_str();
   shellInfo.lpDirectory  = appDir; 
   shellInfo.nShow = SW_SHOW;
   
   // If the shell executed correctly.
   ShellExecuteEx( &shellInfo );
}


UINT WINAPI Decryption(LPVOID pParam) 
{
   if (DecriptVSB(FALSE, FALSE)) 
   {
      EndDialog(GlobalHwnd, 0);
      return 0;
   } 
   else 
   {
      char Temp[1000];
      DWORD Sector;
      DWORD Cluster;
      DWORD Byte;
      DWORD TotalCluster;
      GetDiskFreeSpace(NULL, &Sector, &Byte, &Cluster, &TotalCluster);
      sprintf_s(Temp,1000, g_LocalString[182], (Byte*Sector*Cluster) / 1000000);
      SetDlgItemText(GlobalHwnd, IDC_MESSAGE, Temp );
      while (TRUE) 
         Sleep(15);
      return 0;
   }
}

BOOL WINAPI Dlg_Procd2 (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
   
   if (uMsg == WM_COMMAND) 
   {
      if ((wParam & 0xFFFF) == IDCANCEL) 
      {
         boMusicDecompressionCancelled = true;
         EndDialog(hwnd, 0);
      }
   }
   
   if (uMsg == WM_PAINT) 
   {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
   }
   
   if (uMsg == WM_INITDIALOG) 
   {
      GlobalHwnd = hwnd;
      InvalidateRect(hwnd, NULL, FALSE);
      UpdateWindow(hwnd);
      UINT ID;
      SetDlgItemText(hwnd, IDC_MESSAGE, g_LocalString[175] );
      SetDlgItemText(hwnd, IDC_BOX, g_LocalString[176] );
      SetDlgItemText(hwnd, IDCANCEL, g_LocalString[160] );
      SetWindowText(hwnd, g_LocalString[9]);
      _beginthreadex(NULL, 0, Decryption, NULL, NULL, &ID);
   }
   
   return FALSE;
}

void CorrectText(char *Name)          {
   unsigned int k = 0;
   for (unsigned int j = 0; j < strlen(Name); j++) {
      if (Name[j] != 10 && Name[j] != 13) {
         Name[j] = Name[k];
         k++;
      }
   }
   Name[k] = 0;
}

char RealName[200][500];
char Name[200][500];
char PatchV[200][500];
char Desc[200][500];
char Button[200][500];
char Web[200][500];
char LastOne[500];
char Login[500];
char Login2[500];
//char ProxyPatch[500];

int iGok = 0;

bool bSearchIntoVector(std::string str, std::vector<std::string>vStr)
{
	bool bRet = false;
	for(std::vector<std::string>::iterator i = vStr.begin(); i != vStr.end(); i++)
	{
		if( *i == str ) { bRet = true; }
	}
	return bRet;
}

WORD bColor = 0;

static long FAR PASCAL WindowMessageInput( 
                                          HWND hWnd, 
                                          UINT message, 
                                          WPARAM wParam, 
                                          LPARAM lParam 
                                          ) 
{

   ThreadITimeStamp = timeGetTime();
      
   switch ( message ) 
   {

     case WM_SIZE :
     {
        if(Custom.gWindowed)
        {
        }
     }
     break;
	  // NightMare -- Add 2005-02-17
	  // When resizing in WindowedMode Keep Aspect ratio
	  // Valide All king of resizing...
	  // 1.2756 == ratio factor Width / Height  648 / 508
	  // 648 == 480 + 8pixels of border
	  // 508 == 480 + 8pixels of border +20 pixels tilebars
      case WM_SIZING :
         {
            if(Custom.gWindowed)
            {
               if(g_SaveGame.GetOptionsParam()->bLockResize)
               {
                   RECT rc;
                   GetWindowRect(siGethWnd(),&rc);
                   LPRECT pRect  = (LPRECT)lParam;

                   pRect->left   = rc.left;
                   pRect->right  = rc.right;
                   pRect->top    = rc.top;
                   pRect->bottom = rc.bottom;
                   break;
               }

               UINT   fwSide = wParam;
               LPRECT pRect  = (LPRECT)lParam;
               long lScrWidth  = GetSystemMetrics(SM_CXSCREEN);
               long lScrHeight = GetSystemMetrics(SM_CYSCREEN);
               switch(fwSide)
               {
                  case WMSZ_BOTTOM :
                     pRect->right = (int)(pRect->left +((pRect->bottom-pRect->top)*1.2756));
                     if(pRect->right > lScrWidth)
                     {
                        pRect->right  = lScrWidth;
                        pRect->bottom = (int)(pRect->top+((pRect->right-pRect->left)/1.2756));
                     }
                  break;
                  case WMSZ_RIGHT :
                     pRect->bottom = (int)(pRect->top+((pRect->right-pRect->left)/1.2756));
                     if(pRect->bottom > lScrHeight)
                     {
                        pRect->bottom  = lScrHeight;
                        pRect->right = (int)(pRect->left +((pRect->bottom-pRect->top) *1.2756));
                     }
                  break;
                  case WMSZ_BOTTOMRIGHT :
                     pRect->right = (int)(pRect->left +((pRect->bottom-pRect->top) *1.2756));
                     if(pRect->right > lScrWidth)
                     {
                        pRect->right  = lScrWidth;
                        pRect->bottom = (int)(pRect->top +((pRect->right-pRect->left)/1.2756));
                     }
                     pRect->bottom = (int)(pRect->top +((pRect->right-pRect->left)/1.2756));
                     if(pRect->bottom > lScrHeight)
                     {
                        pRect->bottom  = lScrHeight;
                        pRect->right = (int)(pRect->left +((pRect->bottom-pRect->top) *1.2756));
                     }
                  break;
                  case WMSZ_TOP :
                     pRect->left = (int)(pRect->right -((pRect->bottom-pRect->top) *1.2756));
                     if(pRect->left < 0)
                     {
                        pRect->left   = 0;
                        pRect->top = (int)(pRect->bottom -((pRect->right-pRect->left)/1.2756));
                     }
                  break;
                  case WMSZ_LEFT :
                     pRect->top = (int)(pRect->bottom -((pRect->right-pRect->left)/1.2756));
                     if(pRect->top < 0)
                     {
                        pRect->top  = 0;
                        pRect->left = (int)(pRect->right -((pRect->bottom-pRect->top) *1.2756));
                     }
                  break;
                  case WMSZ_TOPLEFT :
                     pRect->left = (int)(pRect->right -((pRect->bottom-pRect->top)*1.2756));
                     if(pRect->left < 0)
                     {
                        pRect->left   = 0;
                        pRect->top = (int)(pRect->bottom -((pRect->right-pRect->left)/1.2756));
                     }
                     pRect->top = (int)(pRect->bottom -((pRect->right-pRect->left)/1.2756));
                     if(pRect->top < 0)
                     {
                        pRect->top  = 0;
                        pRect->left = (int)(pRect->right -((pRect->bottom-pRect->top) *1.2756));
                     }
                  break;
                  case WMSZ_BOTTOMLEFT :
                     pRect->left = (int)(pRect->right -((pRect->bottom-pRect->top) *1.2756));
                     if(pRect->left < 0)
                     {
                        pRect->left   = 0;
                        pRect->bottom = (int)(pRect->top +((pRect->right-pRect->left)/1.2756));
                     }
                     pRect->bottom = (int)(pRect->top +((pRect->right-pRect->left)/1.2756));
                     if(pRect->bottom > lScrHeight)
                     {
                        pRect->bottom  = lScrHeight;
                        pRect->left = (int)(pRect->right -((pRect->bottom-pRect->top) *1.2756));
                     }  
                  break;
                  case WMSZ_TOPRIGHT :
                     pRect->right = (int)(pRect->left +((pRect->bottom-pRect->top)*1.2756));
                     if(pRect->right > lScrWidth)
                     {
                        pRect->right  = lScrWidth;
                        pRect->top = (int)(pRect->bottom -((pRect->right-pRect->left)/1.2756));
                     }
                     pRect->top = (int)(pRect->bottom 
                        -((pRect->right-pRect->left)/1.2756));
                     if(pRect->top < 0)
                     {
                        pRect->top  = 0;
                        pRect->right = (int)(pRect->left +((pRect->bottom-pRect->top) *1.2756));
                     }
                  break;
               }
            }
         }
         break;
	  // END NightMare
	  //////////////////////////////

      // NightMare -- Add 2005-02-15
      // Verify windows Mouse change message and if Message 
	  // not on Client we Set Transparent Cursor and return TRUE;
	  // return TRUE not apply cursor modification...
	  // NightMare -- Update 2005-02-17
	  // remove variable bWindowedCursorStatus and add Custom.WindowedSizing
	  // to keep sizing status to block player when resizing
      case WM_SETCURSOR:
		 // Only on WindowedMode...
         if(Custom.gWindowed)
         {
			// keep the last stat of cursor 
			// do not change cursor if already transparent...
            int dwHitTestCode = LOWORD(lParam); 
            if(g_SaveGame.GetOptionsParam()->bLockResize && (dwHitTestCode >=10 && dwHitTestCode <=17))
               return FALSE;
            int yPos = HIWORD(lParam); 
            if(dwHitTestCode == HTCLIENT  )
            {
               if(Custom.gWindowedSizing)
               {
                  SetCursor(m_hMouseTransCursor);
                  Custom.gWindowedSizing = false;
               }
               return TRUE;
            }
            else
            {
			   // Here Cursor can change... suppose to be on title bar or Border
               Custom.gWindowedSizing = true;
            }
         }
	  break;
	  // END NightMare
	  //////////////////////////

      case WM_DESTROY:
      case WM_QUIT:
         {
            g_App.Close("DESTROY",true);
         } break;

   case MM_MCINOTIFY: 
      {
         if (wParam == MCI_NOTIFY_SUCCESSFUL)
            PlayCDAudio(GlobalCDTrack, TRUE);
         
      } break;
      
      
      /*2-2
      1
      
        2-2
        3
        2-1
      */
      
   case WM_SYSCOMMAND:
      {
        

         if(wParam == SC_MINIMIZE)
         {
            if(g_Var.inGame)//BLBL m�me si on est pas encore INGAME !
            {
               g_Var.minimizeState = true;
            }
         }
 
         /*
         else if(wParam == SC_MAXIMIZE || wParam == SC_RESTORE || wParam == 0xF032) //0xF032 == AMximize with dbl click header
         {
            if(Custom.gWindowed)
            {

               if(g_SaveGame.GetOptionsParam()->bLockResize)
               {
                  RECT rc;
                  GetWindowRect(siGethWnd(),&rc);
                  LPRECT pRect  = (LPRECT)lParam;

                  pRect->left   = rc.left;
                  pRect->right  = rc.right;
                  pRect->top    = rc.top;
                  pRect->bottom = rc.bottom;
                  return 0;
               }

            }
         }
         */
         else// if(wParam == SC_MINIMIZE)
         {
            g_Var.minimizeState = false;
         }
      }
      break;
      
   case WM_ACTIVATE:
      {
         BOOL fActive = wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE;
         if (fActive) 
         {
            if (KeyBoard.IsCreated()) 
            {
               if (lpDIMouseDevice) 
               {
                  if (siGetSystemOS() == SI_WIN_NT_40) 
                  {
                     Tabbing+=2;
                  }
                  lpDIMouseDevice->Acquire();
               }
               KeyBoard.Acquire();
            }
         }
      } break;
      
      case WM_SETFOCUS:
      {
         g_GameMusic.FocusPlay();
         if (inGameTab && !g_boQuitApp && hMaintenanceThread) 
         {
            //if( !g_App.isFocus() )
            {
               if(!Custom.gWindowed)
               {
                  DXDSetView();
                  DXDRestoreSurfaceF();
                  DXDRestoreSurface();
               }

               if (KeyBoard.IsCreated()) 
               {
                  if (lpDIMouseDevice) 
                  {
                     if (siGetSystemOS() == SI_WIN_NT_40) 
                     {
                        Tabbing+=2;
                     }
                     lpDIMouseDevice->Acquire();
                  }
                  KeyBoard.Acquire();
               }
            }

            CTRL_State  = 0;
            SHIFT_State = 0;

            /*
            // SET FOCUS BLOCK
            if ((!wParam || wParam == dwGetID) && !g_boQuitApp) 
            {
               if (hMaintenanceThread) 
               {
                  CTRL_State = 0;
               }
            }
            // END SET FOCUS BLOCK
            */
         }
      } break;
      case WM_KILLFOCUS: 
      {
         g_GameMusic.FocusPause();
         if (inGameTab) 
         {			
            //if (g_App.isFocus()) 
            {
               if (hMaintenanceThread && !g_boQuitApp)
               {                         
                  if (lpDIMouseDevice) 
                  {
                     lpDIMouseDevice->Unacquire();
                  }
                  KeyBoard.Unacquire();
               }
            } 
         } 
         // END KILL FOCUS BLOCK
      } break;

      
   case WM_KEYUP:
      {
         // ESCAPE depressed
         if (wParam == 17) {
            CTRL_State = 0;
            
         }
         // SHIFT depressed.
         if( wParam == VK_SHIFT ){
            SHIFT_State = false;
         }         
      } break;
      
   case WM_SYSKEYDOWN:

      /*
      if (wParam == VK_F10) 
      {
         PostMessage(hWnd, WM_KEYDOWN, VK_F10, NULL);
         PostMessage(hWnd, WM_KEYUP, VK_F10, NULL);
      }
      */

      if (wParam == 17) 
      {
         CTRL_State = 1;
         
      }
   break; 

   case WM_SYSKEYUP:
         if (wParam == 17) {
            CTRL_State = 0;
         }
   break;

   case WM_KEYDOWN:
      {
         // Disable TAB key.
         //if (wParam == 9) 
         //{
         //   return DefWindowProc(hWnd, message, wParam, lParam);            
         //}

         if (wParam == 17) {
            CTRL_State = 1;
            
         }
         if( wParam == VK_SHIFT )
         {
            SHIFT_State = true;
         }

         NoKey = false;
         switch (TFC_State) 
         {
            case TFC_MENU:                  
            case TFC_INTRODUCTION: 
            case TFC_CHOOSE_RACE: 
            case TFC_MAKE_REROLL:           
            case TFC_CHOOSE_PLAYER:
            case TFC_WARNING: 
            case TFC_OPTION:        
            case TFC_VIEW_CREDITS: 
            case TFC_CONFIRM_DELETE:       
            case TFC_MSGDAY: 
            case TFC_ASK_CREATE_CHARACTER:
            case TFC_CHOOSE_SEXX:   
            case TFC_WARNING_2: 
            case TFC_CONNECT:
            {
               if (wParam == VK_DOWN)
                  key = 200;
               if (wParam == VK_UP)
                  key = 201;
               if (wParam == VK_TAB)
                  key = 202;
               if (wParam == VK_F10) 
                  return 1; // disable f10 key
            } break;
            
            case TFC_PLAY:
            {
               if( 1)
               {
                  if (g_DisplayLetter) 
                  {
                     g_DisplayLetter = false;
                  } 
                  else 
                  {
                     VKey vKey;
                     vKey.SetKey( wParam, CTRL_State != 0, SHIFT_State );
                     // If that's a valid vkey.
                     if( vKey.GetKey() != 0 )
                     {
                        if( !Custom.gMacro.CallMacro( vKey ) && !V3_MacroDlg::GetInstance()->CallMacro( vKey ) )
                        {
                           RootBoxUI::GetInstance()->VKeyInput( vKey );
                        }
                        else
                           g_Global.ResetItemInfo();
                     }
                  }
               }
               
               switch (wParam) 
               {
                  //case VK_NUMPAD1: case VK_NUMPAD2: case VK_NUMPAD3: case VK_NUMPAD4: case VK_NUMPAD5: 
                  //case VK_NUMPAD6: case VK_NUMPAD7: case VK_NUMPAD8: case VK_NUMPAD9: 
                  //case VK_END: case VK_NEXT: case VK_HOME:  case VK_PRIOR:   
                  
                  case VK_UP: case VK_DOWN: case VK_LEFT:    case VK_RIGHT: case VK_ESCAPE:
                  {
  
                     NoKey = true;
                  } 
                  break;
                  /*
                  case VK_F1: 
                  break; 
                  case VK_F2: 
                  break;
                  case VK_F3: 
                  break;
                  case VK_F4: 
                  break;
                  case VK_F5: 
                  break; 
                  case VK_F6: 
                  break;
                  case VK_F7: 
                  break;
                  case VK_F8: 
                  {
                     #if 0
                        Objects.Lock(0);
                        Objects.DeleteAll();
                        Objects.Unlock(0);
                     #endif
                  } 
                  break;
                  case VK_F9: 
                  break;
                  */
                  case VK_F10: 
                     return 1; // Disable F10 Key
                  break;
                  
                  
               }
             } break;
        }
        } break;
        
      case WM_CHAR:
         {
            // Disable TAB key.
            if (wParam == 9)
               return DefWindowProc(hWnd, message, wParam, lParam);

            switch (TFC_State) {
               
            case TFC_PLAY: 
               {
                  if (NoKey) 
                     break;
                  if ( !CTRL_State || ( isprint( wParam ) || ( wParam > 128 && wParam < 256 ) ) ) 
                  {
                     if (INGAME) 
                     {
                        
                        if( RootBoxUI::GetInstance()->TextInput( wParam ) )
                        {
                           NoKey = true;
                           break;
                        }
                       
                        V3_ChatLogDlg::GetInstance()->TextInput( wParam );
                        NoKey = true;
                     } 
                     /*                  #ifdef GRBX_CHAR
                     BoxHandler.PostBoxMessage(GWIN_MSG_CHAR, wParam, NULL, NULL); break;
#endif  */
                  }
               } break;
          
          case TFC_MENU: 
          case TFC_INTRODUCTION: 
          case TFC_CHOOSE_RACE: 
          case TFC_MAKE_REROLL: 
          case TFC_CHOOSE_PLAYER: 
          case TFC_WARNING:
          case TFC_OPTION: 
          case TFC_VIEW_CREDITS: 
          case TFC_CONFIRM_DELETE:
          case TFC_MSGDAY: 
          case TFC_ASK_CREATE_CHARACTER:
          case TFC_WARNING_2: 
          case TFC_CHOOSE_SEXX: 
             {
                if (!CTRL_State || !isalpha(wParam))
                   key = tolower(wParam);
             } break;
             
          case TFC_CHOOSE_NAME:
          case TFC_CONNECT:
             {
                key = wParam;
             } break;
         } 
         
         } break;
         
      case WM_CLOSE:
         {
            if(g_Var.inGame)
            {
               int caca = 1;
               int dwVal  = lParam;

               if(lParam == 0x6969)
               {
                  if(m_hMp3Thread)
                  {
                     SetEvent(g_hMp3EventExit);
                     if( WaitForSingleObject( m_hMp3Thread, 1500) == WAIT_TIMEOUT ) 
                        TerminateThread( m_hMp3Thread, 1 );
                     m_hMp3Thread = NULL;
                  }
                  g_App.Close(g_LocalString[403],false);
               }
               else
               {
                  g_Global.ResetItemInfo();
                  V3_MainBarDlg::GetInstance()->ShowQuitWindows();
                  return FALSE;
               }
               
            }
            else
            {
                  g_boQuitApp = 1;
                  //PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x1111);
                  //Sleep(2000);
            }
         } break;
         
         //      case WM_KILLFOCUS:
         //         exit(1);
         //      break;
   }
   
   ThreadITimeStamp = 0;
   
   return DefWindowProc(hWnd, message, wParam, lParam);
}

void CALLBACK TimerFunc( UINT nID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD d2);


// ******************************************************************************************* //
int WINAPI WinMain 
// ******************************************************************************************* //
// La Procedure Principale.
// 
(
 HINSTANCE hInstance,      // The Application Instance handle.
 HINSTANCE hPrevInstance,  // Instance of the same Application currently running.
 LPSTR     lpCmdLine,      // The Command line.
 int       nShowCmd        // Specified how the application should be shown.
 )
{
   char Path[256];
   GetModuleFileName(NULL, Path, 256);
   int len = strlen(Path) - 1;
   while (len && Path[len] != '\\')
      len--;
   Path[len] = 0;
   SetCurrentDirectory(Path); 


  
   ClientConfig *pConfig = g_Global.ReadClientConfig();
 
   //verifie config in first...
   char strConfig[512];
   sprintf_s(strConfig,512,"%s\\T4CConfig.ini",pConfig->strSavePath);
   FILE *pfConfig = NULL;
   fopen_s(&pfConfig,strConfig,"rb");
   if(!pfConfig)
   { 
      /*
      MessageBox(NULL,"Welcome on T4C !\nPlease proceed to the basic configuration before launching the game.\n\n"\
                      "Bienvenue sur T4C !\nVeuillez proc�der � la configuration de base avant le lancement du jeu.\n\n"\
                      "Willkommen T4C !\nBitte fahren Sie mit der Grundkonfiguration vor dem Start des Spiels"
         ,"Information!", MB_OK+MB_ICONINFORMATION);
      ShellExecute(NULL, "open", "T4CConfig.exe", "", "", SW_SHOW);
      return true;
      */
      Custom.NoConfigFileFound = TRUE;
   }
   else
   {
      fseek(pfConfig, 0, SEEK_END);
      int sizeBuf = ftell(pfConfig);
      fclose(pfConfig);
      
      if(sizeBuf == 0)
          Custom.NoConfigFileFound = TRUE;
      else
         Custom.NoConfigFileFound = FALSE;
   }

   //Setup Tmp with config value...
   Custom.gShowFPS       = pConfig->dwNMShowFPS;




 
   // TRY to Update DLL patchw32.DLL 
   CopyFile("NEWDLL.DLL", "PATCHW32.DLL", FALSE);

  
   // Variables.
   MSG msg;
   // System Information Initialisation.
   siCreate();
   siSethInstance(hInstance);
   siSetSystemOS();

 
   // Activate Memory Leak Logging.
   g_Var.bMemory = true;
   
   WndClassInit();      // Register the Window Class.
   CreateWnd();         // Create the Window.
   
   OutputDebugString("Init!\r\n");
   bGetID = TRUE;

   if (!g_boQuitApp)
      InitializeTFCData();
   if(g_boQuitApp)
      return true;
   
   
  
   // - Correction of small bug on LoadCursor Parameters...
   m_hMouseTransCursor = LoadCursor(siGethInstance(), MAKEINTRESOURCE(IDC_EMPTY_CURSOR)); 
   SetCursor(m_hMouseTransCursor);
   
   while ( !g_boQuitApp ) 
   {
      if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
      {
         if ( !GetMessage( &msg, NULL, 0, 0 ) )
         {
            return msg.wParam;
         } 
         
         if( msg.message == WM_ACTIVATEAPP )
         {
            SetCursor(m_hMouseTransCursor);
         }
         
         if(msg.hwnd)
         {
            if (!TranslateAccelerator(msg.hwnd, NULL, &msg)) 
            {
               if(msg.message == WM_SYSKEYDOWN && msg.wParam == VK_F10)
               {
                  PostMessage(msg.hwnd, WM_KEYDOWN, VK_F10, NULL);
                  PostMessage(msg.hwnd, WM_KEYUP, VK_F10, NULL);
               }
               else
               {
                  TranslateMessage(&msg);
                  DispatchMessage(&msg);
               }
            }
         }
      } 
      else 
      {
         WaitMessage();
      }
   }

      
   ::CloseHandle(g_hMp3EventExit);
   ::CloseHandle(g_hMp3EventNext);
   ::CloseHandle(g_hMp3EventPrev);
   ::CloseHandle(g_hMp3EventPlay);
   ::CloseHandle(g_hMp3EventPaused);
   ::CloseHandle(g_hMp3EventVol);
   ::CloseHandle(g_hMp3EventDisable);
   
   

  

   g_hMp3EventExit   = NULL;
   g_hMp3EventNext   = NULL;
   g_hMp3EventPrev   = NULL;
   g_hMp3EventPlay   = NULL;
   g_hMp3EventPaused = NULL;
   g_hMp3EventVol    = NULL;
   g_hMp3EventDisable= NULL;
   
  
   OutputDebugString( "\r\nWaiting for shutdown.\n\n" );
   int dwQuitCnt = 0;
   while( g_boWaitForShutdown && dwQuitCnt <200)
   {
      Sleep( 200 );
      dwQuitCnt++;
   }
    
   if(dwQuitCnt>=200)
   {
      OutputDebugString( "\r\n***********Pas eu le temps de fermer.....\n\n\n" );
      exit(5678);
      
   }
   g_boWaitForShutdown = 0;
   OutputDebugString( "\r\nClosing app.\n\n\n" );
   
   Sleep(500);
   siRelease();
   Sleep(500);
   //exit(0);
   return true;
}

// ******************************************************************************************* //
// Initialisation de la Class Window.
// Return:
//	  true   Initialisation Succefull.
//    false  Error with Initialisation.
//
// ******************************************************************************************* //
bool WndClassInit ( void )

{
   WNDCLASS wcex; // Window Class
   
   //	wcex.cbSize        = sizeof(WNDCLASSEX);
   wcex.style         = CS_VREDRAW; // No Close, Redraw on Change.
   wcex.lpfnWndProc   = WindowMessageInput; // Callback function.
   wcex.cbClsExtra    = 0;
   wcex.cbWndExtra    = 0;
   wcex.hInstance     = siGethInstance(); // Window Instance
   wcex.hIcon         = LoadIcon( siGethInstance(), MAKEINTRESOURCE(IDI_ICON2) ); // Application Icon //NightMare COrrect The name of Icons
   wcex.hCursor       = LoadCursor( NULL, IDC_ARROW ); // Application Arrow
   wcex.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH ); 
   wcex.lpszMenuName  = NULL;
   wcex.lpszClassName = "AppClass"; // Class Name
   //	wcex.hIconSm       = NULL; //LoadIcon( ug_siApp->GethInstance(), MAKEINTRESOURCE(IDI_APP_ICON) ); // Application Small Icon
   
   if ( !RegisterClass( &wcex ) ) 
   {
      GetLastError();
   }
   // Success.
   return true;
}


// ******************************************************************************************* //
// Creation du Window.
// Return:
//		true   Creation Succefull.
//    false  Error with Creation.
//
// ******************************************************************************************* //
bool CreateWnd ( void )
{
   HWND hWnd; // Window Handle
   
   hWnd = CreateWindow ( 
      "AppClass", // Window Class Name
      "The 4th Coming", // Application Name
      WS_POPUP,// | WS_MAXIMIZE, // Style
      (GetSystemMetrics(SM_CXSCREEN)-g_Global.GetScreenW())/2, // left
      (GetSystemMetrics(SM_CYSCREEN)-g_Global.GetScreenH())/2, // top
      0, // left
      0, // top
      //		GetSystemMetrics(SM_CXSCREEN), // right
      //		GetSystemMetrics(SM_CYSCREEN), // bottom
      NULL,
      NULL,
      siGethInstance(), // Handle Instance
      NULL 
      );
   //ShowWindow(hWnd, SW_SHOW);
   
   if ( !hWnd ) 
   {
      // Cannot create the Window.
      GetLastError();
   }
   siSethWnd(hWnd);
   return true;
}


// True cursors!

CV2Sprite V3MouseCursor1;
CV2Sprite V3MouseCursor1A;
AnimV2Sprite V3MouseCursor2;
AnimV2Sprite V3MouseCursor3;
CV2Sprite V3MouseCursor5;
CV2Sprite V3MouseCursor5A;
CV2Sprite V3MouseCursor6;
CV2Sprite V3MouseCursor6A;
CV2Sprite V3MouseCursor7;
CV2Sprite V3MouseCursor7A;
CV2Sprite V3MouseCursor8;
CV2Sprite V3MouseCursor8A;
CV2Sprite V3MouseCursor9;
CV2Sprite V3MouseCursor9A;
CV2Sprite V3MouseCursor10;
CV2Sprite V3MouseCursor10A;
CV2Sprite V3MouseCursor11;
CV2Sprite V3MouseCursor11A;
CV2Sprite V3MouseCursor12;
CV2Sprite V3MouseCursor12A;

AnimV2Sprite V3MouseCursor16;
CV2Sprite V3MouseCursor17;
CV2Sprite V3MouseCursor17A;

CV2Sprite V3MouseCursor19;
CV2Sprite V3MouseCursor19A;
CV2Sprite V3MouseCursor20;
CV2Sprite V3MouseCursor20A;
CV2Sprite V3MouseCursor21;
CV2Sprite V3MouseCursor21A;
CV2Sprite V3MouseCursor22;
CV2Sprite V3MouseCursor22A;






AnimV2Sprite MouseCursor18;


CombatCursorVSF AttackCursorIcon;

//CV2Sprite SplashP;
CV2Sprite    Splash;


BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam) 
{
   char title[500];
   GetWindowText(hWnd, title, sizeof(title)); 
   if(strstr(title, "The 4th Coming")) 
   {
      g_dwNbrt4cInstance++;
      //return FALSE;
   } 
   return TRUE;
}


bool IsNbrT4cGraterThanOne(BOOL bUpdate)
{
	if(bUpdate)
	{
		g_dwNbrt4cInstance = 0;
		EnumWindows(MyEnumProc, 0);
	}

   if(g_dwNbrt4cInstance > 1)
      return true;
   else
      return false;
}

void InitializeTFCData(void) 
{
   char Path[256];
   GetModuleFileName(NULL, Path, 256);
   int len = strlen(Path) - 1;
   while (len && Path[len] != '\\')
      len--;
   Path[len] = 0;

   ClientConfig *pConfig = g_Global.GetClientConfig();
   

   //Load le launcher Config pour loader les macro voulu...
   char strConfig[512];
   sprintf_s(strConfig,512,"%s\\T4CConfig.ini",pConfig->strSavePath);
   Custom.gIniTools.SetIniFileName(strConfig);

   /*
   if(!Custom.gIniTools.CheckSection("SETTINGS") || 
      !Custom.gIniTools.CheckSection("MACRO")    ||
      !Custom.gIniTools.CheckSection("COLOR")          )
   {
      MessageBox(siGethWnd(), "Invalid config file... Use T4CConfig.exe.\n\n"
         "Fichier de configuration invalid... Utiliser T4CConfig.exe.\n\n"
         "Ung�ltige Konfigurationsdatei... Verwenden T4CConfig.exe.\n\n", "Error / Erreur / Fehler", MB_OK);
      exit(777);
   }
   */

   //read Settings...
   DWORD dwCLanguage;
   DWORD dwCResolution;
   dwCResolution           = Custom.gIniTools.GetINIValueInt("SETTINGS","GAME_RESOLUTION",2);
   Custom.gWindowed        = Custom.gIniTools.GetINIValueInt("SETTINGS","GAME_WINDOWED",1);
   Custom.gdwUSEMMX        = Custom.gIniTools.GetINIValueInt("SETTINGS","GAME_OLD_MMX",0);
   Custom.gdwUSEVBOX       = Custom.gIniTools.GetINIValueInt("SETTINGS","GAME_VBOX",0);
   dwCLanguage             = Custom.gIniTools.GetINIValueInt("SETTINGS","GAME_LANGUAGE",0);
   Custom.gDisConMusic     = Custom.gIniTools.GetINIValueInt("SETTINGS","GAME_CONNECT_MUSIC_OFF",0);
   sprintf_s(Custom.gstrMp3PlayList,255,"%s",Custom.gIniTools.GetINIValue("SETTINGS","GAME_MP3_PLAYLIST"  ,""));


   //ReadMacro
   BOOL bMacro[12];
   BYTE chMacroKey[12];
   chMacroKey[0]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","INVENTORY_KEY",'I');
   chMacroKey[1]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","CHARACTER_KEY",'S');
   chMacroKey[2]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","CHATTER_KEY",'L');
   chMacroKey[3]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","GROUPPLAY_KEY",'G');
   chMacroKey[4]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","SPELL_KEY",'P');
   chMacroKey[5]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","MACRO_KEY",'M');
   chMacroKey[6]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","TRADE_KEY",'T');
   chMacroKey[7]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","OPTION_KEY",'O');
   chMacroKey[8]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","SCREENSHOT_KEY",'H');
   chMacroKey[9]  = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","GUILD_KEY",'D');
   chMacroKey[10] = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","PROFESSION_KEY",'J');
   chMacroKey[11] = (BYTE)Custom.gIniTools.GetINIValueInt("MACRO","TAKEITEMID_KEY",'K');
   bMacro[0]      = Custom.gIniTools.GetINIValueInt("MACRO","INVENTORY_ENABLE",1);
   bMacro[1]      = Custom.gIniTools.GetINIValueInt("MACRO","CHARACTER_ENABLE",1);
   bMacro[2]      = Custom.gIniTools.GetINIValueInt("MACRO","CHATTER_ENABLE",1);
   bMacro[3]      = Custom.gIniTools.GetINIValueInt("MACRO","GROUPPLAY_ENABLE",1);
   bMacro[4]      = Custom.gIniTools.GetINIValueInt("MACRO","SPELL_ENABLE",1);
   bMacro[5]      = Custom.gIniTools.GetINIValueInt("MACRO","MACRO_ENABLE",1);
   bMacro[6]      = Custom.gIniTools.GetINIValueInt("MACRO","TRADE_ENABLE",1);
   bMacro[7]      = Custom.gIniTools.GetINIValueInt("MACRO","OPTION_ENABLE",1);
   bMacro[8]      = Custom.gIniTools.GetINIValueInt("MACRO","SCREENSHOT_ENABLE",1);
   bMacro[9]      = Custom.gIniTools.GetINIValueInt("MACRO","GUILD_ENABLE",1);
   bMacro[10]     = Custom.gIniTools.GetINIValueInt("MACRO","PROFESSION_ENABLE",1);
   bMacro[11]     = Custom.gIniTools.GetINIValueInt("MACRO","TAKEITEMID_ENABLE",1);

   //Read All CC Color
   Custom.g_dwNbrCCColor = 0;
   char strColorID[30];
   char strColorEnable[30];
   DWORD dwColor[50];
   for(int i=0;i<50;i++)
   {
      sprintf_s(strColorID,30,"COLORID%02d",i);
      sprintf_s(strColorEnable,30,"COLORENABLE%02d",i);

      DWORD dwColorStatus = Custom.gIniTools.GetINIValueInt("COLOR",strColorEnable,0);
      if(dwColorStatus)
      {
         Custom.g_dwNbrCCColor++;
         dwColor[i] = Custom.gIniTools.GetINIValueInt("COLOR",strColorID    ,0);
      }
   }
   if(Custom.g_dwNbrCCColor >0)
   {
      Custom.g_pNewCCColor = new int[Custom.g_dwNbrCCColor];
      for(int i=0;i<Custom.g_dwNbrCCColor;i++)
      {
         Custom.g_pNewCCColor[i] = dwColor[i];
      }
   }
   else
   {
      Custom.g_dwNbrCCColor = 8;
      Custom.g_pNewCCColor = new int[8];
      Custom.g_pNewCCColor[0] = RGB(17,181,205);
      Custom.g_pNewCCColor[1] = RGB(255,33,75);
      Custom.g_pNewCCColor[2] = RGB(90,119,216);
      Custom.g_pNewCCColor[3] = RGB(220,202,6);
      Custom.g_pNewCCColor[4] = RGB(5,197,37);
      Custom.g_pNewCCColor[5] = RGB(199,10,204);
      Custom.g_pNewCCColor[6] = RGB(192,192,192);
      Custom.g_pNewCCColor[7] = RGB(224,100,10);
   }

   //Setup Macro
   if(bMacro[0])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[0], true ), V3_InvDlg::DisplayM, NULL );
   if(bMacro[1])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[1], true ), V3_StatsDlg::DisplayM, NULL );
   if(bMacro[2])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[2], true ), V3_ChatDlg::DisplayM, NULL );
   if(bMacro[3])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[3], true ), V3_GroupeDlg::DisplayM, NULL );
   if(bMacro[4])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[4], true ), V3_SpellDlg::DisplayM, NULL );
   if(bMacro[5])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[5], true ), V3_MacroDlg::DisplayM, NULL );
   if(bMacro[6])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[6], true ), V3_TradeDlg::DisplayM, NULL );
   if(bMacro[7])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[7], true ), V3_OptionsDlg::DisplayM, NULL );
   if(bMacro[8])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[8], true ), ScreenShot, NULL );
   if(bMacro[9])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[9], true ), V3_GuildDlg::DisplayM, NULL );
   if(bMacro[10])
      Custom.gMacro.AddNewMacro( VKey( chMacroKey[10], true ), V3_ProfessionDlg::DisplayM, NULL );
   if(bMacro[11])
	   Custom.gMacro.AddNewMacro( VKey( chMacroKey[11], true ), TakeItemReservedID, NULL );



   Custom.gMacro.AddNewMacro( VKey( 'C', true),  AttackMode       , NULL );
   Custom.gMacro.AddNewMacro( VKey( 'W', true ), ShowWorldMap     , NULL );
   Custom.gMacro.AddNewMacro( VKey( 'N', true ), IdentifyAllObject, NULL );
   Custom.gMacro.AddNewMacro( VKey( 'V', true ), PasteClipboard   , NULL );
   //Custom.gMacro.AddNewMacro( VKey( 'B', true ), StartCapture, NULL );
   //Custom.gMacro.AddNewMacro( VKey( 'N', true ), EndCapture, NULL );

   //Setup language files.
   switch (dwCLanguage) 
   {
      case 0:  strcpy_s(Player.szLanguage,256, "English"); break; 
      case 1:  strcpy_s(Player.szLanguage,256, "French"); break; 
      case 2:  strcpy_s(Player.szLanguage,256, "German"); break; 
      default: strcpy_s(Player.szLanguage,256, "English"); break; 
   }

   //Setup Resolution
   if(dwCResolution == 0)
      g_Global.SetDisplaySize(640,480);
   else if(dwCResolution == 1)
      g_Global.SetDisplaySize(800,600);
   else if(dwCResolution == 2)
      g_Global.SetDisplaySize(1024,768);
   else if(dwCResolution == 3)
      g_Global.SetDisplaySize(1280,768);
   else if(dwCResolution == 4)
      g_Global.SetDisplaySize(1280,720);
   else if(dwCResolution == 5)
      g_Global.SetDisplaySize(1280,800);
   else
      g_Global.SetDisplaySize(DEF_WINDOW_W,DEF_WINDOW_H);

   //Setup Mp3 Play List if selected...
   FILE *pf = NULL;
   if(Custom.gstrMp3PlayList[0] != 0) // on a un mp3 files...
      fopen_s(&pf,Custom.gstrMp3PlayList,"rt");
   if(pf)
   {
      char strFLine[4096];
      char *pRetRead;
      char *pRetScan1;
      char *pRetScan2;
      do 
      {
         TCHAR szPath[MAX_PATH]; // Buffer correctly sized
         if(FAILED(SHGetFolderPath(NULL,CSIDL_MYMUSIC, NULL, 0, szPath))) 
         { 
            
         } 

         pRetRead = fgets(strFLine,4096,pf);
         if(pRetRead)
         {
            pRetScan1 = strstr(strFLine,"media src=\"");
            if(pRetScan1!= 0)
            {
               pRetScan1+= 11; // skip header line
               pRetScan2 = strchr(pRetScan1,'"');
               pRetScan1[pRetScan2-pRetScan1] = NULL;

               std::string S = pRetScan1;
               int iPos = S.find("&amp;");
               if( iPos >=0)
                  S.replace(S.find("&amp;"), 5, "&");

               iPos = S.find("&lt;");
               if( iPos >=0)
                  S.replace(S.find("&lt;"), 4, "<");

               iPos = S.find("&gt;");
               if( iPos >=0)
                  S.replace(S.find("&gt;"), 4, ">");

               iPos = S.find("&quot;");
               if( iPos >=0)
                  S.replace(S.find("&quot;"), 6, "\"");

               iPos = S.find("&apos;");
               if( iPos >=0)
                  S.replace(S.find("&apos;"), 6, "'");


              



               sMp3List newMp3;
               sprintf_s(newMp3.strName,1024,"%s",S.c_str());
               if( ( newMp3.strName[strlen(newMp3.strName)-4] == '.'                                                     ) &&
                  ( newMp3.strName[strlen(newMp3.strName)-3] == 'M'  || newMp3.strName[strlen(newMp3.strName)-3] == 'm' ) &&
                  ( newMp3.strName[strlen(newMp3.strName)-2] == 'M'  || newMp3.strName[strlen(newMp3.strName)-2] == 'p' ) &&
                  ( newMp3.strName[strlen(newMp3.strName)-1] == '3'                                                     )    )
               {
                  FILE *pf=NULL;
                  fopen_s(&pf,newMp3.strName,"rb");
                  if(pf)
                  {
                     fclose(pf);
                     Custom.gMp3List.push_back(newMp3);
                  }
                  else
                  {
                     sprintf_s(newMp3.strName,1024,"%s%s",szPath,S.c_str()+2);
                     pf=NULL;
                     fopen_s(&pf,newMp3.strName,"rb");
                     if(pf)
                     {
                        fclose(pf);
                        Custom.gMp3List.push_back(newMp3);
                     }
                  }
               }


            }
         }
      } while(pRetRead);
      fclose(pf);

   }
      
   g_LocalString   .Release();
   g_LocalString   .CreateLang(dwCLanguage,false);
   g_GUILocalString.Release();
   g_GUILocalString.CreateLang(dwCLanguage,true);

   Custom.iLanguageID = dwCLanguage;
   
   if(pConfig->FirstTimeAddon == 1)
      Custom.gfirstTimeAddOn = true;
   pConfig->FirstTimeAddon = 0;
      
   GlobalCDHandle = siGethWnd();
   OpenCDAudio();
   
   if (FALSE && QueryCDAudio()) 
   {
   }
   
   
   if (g_SaveGame.GetOptionsParam()->bUseCD) 
   {
      PlayCDAudio(7, TRUE); // bye default play track 7
   }
   
   unsigned char CodeC[5] = { 120, 87, 123, 98, 8 };
   
     
     
     char Temp[100];
     UINT i = 0;
     for (i = 0; i < strlen(ServerIP); i++) {
        Temp[i] = ServerIP[i];
     }
     Temp[i] = 0;
     
     
     ug_T3SoundManager = new T3SoundManager;   
     
     int iMusicRetry = 0;
     do 
     {
        std::string errMsg;                    
        if (!ug_T3SoundManager->Create(siGethWnd(), 44100, 16, 2, errMsg)) 
        {
           if (siGetSystemOS() == SI_WIN_NT_40) 
           {
              iMusicRetry = MessageBox(siGethWnd(), g_LocalString[543], g_LocalString[504], MB_OKCANCEL);
           } 
           else 
           {
              iMusicRetry = MessageBox(siGethWnd(), g_LocalString[503], g_LocalString[504], MB_RETRYCANCEL);
           }
        }
     } while (iMusicRetry == IDRETRY);
     if (iMusicRetry == IDCANCEL && siGetSystemOS() == SI_WIN_NT_40) 
     {
        g_App.Close("User abort on NT4 (Music not initiazed).",true);
        exit(0);
     }
     
     
     if (g_ForceDecompression) 
     {
        DialogBox(siGethInstance(), MAKEINTRESOURCE(IDD_DIALOG2), siGethWnd(), Dlg_Procd2);
        
        if( !boMusicDecompressionCancelled ){
           if (g_ForceDecompression)
              exit(777);
           
           MessageBox(siGethWnd(), g_LocalString[438], g_LocalString[437], MB_OK);
           RestartT4C();
        }
        SendMessage(siGethWnd(), WM_CLOSE, 0, 0x6969);
        return;
     }		
     
     
     if (g_ForceDecompression)
        exit(777);
     
     
     
     //FILE *index = NULL;
     //fopen_s(&index,"Game Files\\MD.MD", "rb");
     if (/*index  ||*/ g_ForceDecompression) 
     {
        /*
        if (index)
           fclose(index);
        */
        DialogBox(siGethInstance(), MAKEINTRESOURCE(IDD_DIALOG2), siGethWnd(), Dlg_Procd2);
        
        if( !boMusicDecompressionCancelled )
        {
           if (g_ForceDecompression)
              exit(777);
           MessageBox(siGethWnd(), g_LocalString[438], g_LocalString[437], MB_OK);
           RestartT4C();
        }
        SendMessage(siGethWnd(), WM_CLOSE, 0, 0x6969);
        return;
     } 
     else
     {
        FILE *hFile=NULL;
        //Now on lit de mes document car le client necrit plus aucun fichier dans son repertoire UAC oblige
        ClientConfig *pConfig = g_Global.GetClientConfig();
        char strFileVSB[MAX_PATH];
        sprintf_s(strFileVSB,MAX_PATH,"%s\\Sounds\\T4CGameFile.VSB",pConfig->strSavePath);

        fopen_s(&hFile, strFileVSB, "rb");
        FILE *hFile2 = NULL;
        fopen_s(&hFile2, "Game Files\\VSBInfo.Txt", "rb");
        int fileSize = 0;
        BYTE bBuffer[256];
        
        if (hFile != NULL && hFile2 != NULL) 
        {
           
           fgets((char *)bBuffer, 256, hFile2);
           
           int iValue = atoi((char *)bBuffer);
           if (iValue) 
           {
              
              fseek(hFile, 0, SEEK_END);
              fileSize = ftell(hFile);
              
              fclose(hFile);
              fclose(hFile2);
              
              if (fileSize != iValue) 
              {                  
                 DialogBox(siGethInstance(), MAKEINTRESOURCE(IDD_DIALOG2), siGethWnd(), Dlg_Procd2);
                 if( !boMusicDecompressionCancelled )
                 {
                    if (g_ForceDecompression)
                       exit(777);
                    
                    MessageBox(siGethWnd(), g_LocalString[438], g_LocalString[437], MB_OK);
                    RestartT4C();                    
                 }
                 SendMessage(siGethWnd(), WM_CLOSE, 0, 0x6969);
                 return;
              } 
           } 
           else 
           {
              fclose(hFile);
              fclose(hFile2);
           }
        } 
        else 
        {
           if (hFile != NULL)
              fclose(hFile);
           if (hFile2 != NULL)
              fclose(hFile2);
           
           DialogBox(siGethInstance(), MAKEINTRESOURCE(IDD_DIALOG2), siGethWnd(), Dlg_Procd2);
           if( !boMusicDecompressionCancelled )
           {
              if (g_ForceDecompression)
                 exit(777);
              MessageBox(siGethWnd(), g_LocalString[438], g_LocalString[437], MB_OK);
              RestartT4C();
           }            
           SendMessage(siGethWnd(), WM_CLOSE, 0, 0x6969);
           return;
        }
        
        LPBYTE MemTest = new BYTE [32000000];
        if (!MemTest) 
        {
           DialogBox(siGethInstance(), MAKEINTRESOURCE(IDD_DIALOG2), siGethWnd(), Dlg_Procd2);
           if (g_boQuitApp)
              return;
        }
        delete MemTest;
        if (g_boQuitApp) 
        {
           SendMessage(siGethWnd(), WM_CLOSE, 0, 0x6969);
           return;
        }
     }	
     
    
     if (g_ForceDecompression)
        exit(777);
     
     if (MsgDay) 
     {
        delete MsgDay;
        MsgDay = NULL;
     }
  
     //prepare Mp3 process...
     g_hMp3EventExit    = CreateEvent(NULL,FALSE, FALSE, NULL);
     g_hMp3EventNext    = CreateEvent(NULL,FALSE, FALSE, NULL);
     g_hMp3EventPrev    = CreateEvent(NULL,FALSE, FALSE, NULL);
     g_hMp3EventPlay    = CreateEvent(NULL,FALSE, FALSE, NULL);
     g_hMp3EventPaused  = CreateEvent(NULL,FALSE, FALSE, NULL);
     g_hMp3EventVol     = CreateEvent(NULL,FALSE, FALSE, NULL);
	 g_hMp3EventDisable = CreateEvent(NULL,FALSE, FALSE, NULL);
     
     
     m_hMp3Thread = (HANDLE)_beginthread(Mp3PlayerThread, 0, NULL );


     
     //Setup WindowedMode windows
     if (Custom.gWindowed)
     {
        SetWindowLong( siGethWnd(), GWL_STYLE, WS_OVERLAPPEDWINDOW ); // Set titlebar, min/max/restore buttons and sysmenu
     }

     
     g_Global.LoadGUI();

     time_t CurrentTime;
     time(&CurrentTime);
     if(!Custom.gWindowed)
     {
        int h = ShowCursor(false);
        while (h > 0)
           h = ShowCursor(false);
     }
     // END NightMare
     //////////////////////////////


     
     lpDXDirectDraw = new IDirectXDraw; // Init DirectDraw ptr.
     // Init DirectX Draw
     if (DXDCreate(siGethWnd(), 0) != DD_OK) {
        g_App.SetError(1, g_LocalString[154]);
        g_App.Close("Error in initialization",true);
        return;
     }

   g_FirstInitCreateComplete = FALSE;
   _beginthread( FirstInitCreate, 0, NULL );
}
 
void ParseText(char *pText) 
{
   Objects.Lock(28);
   BOOL boNPCisThere = Objects.FoundID(TalkToID);
   Objects.Unlock(28);
   TFCPacket Send;

   ClientConfig *pConfig = g_Global.GetClientConfig();
   
   char strText[1000];
   //char strTextTmp[1024];
   int dwlength = strlen(pText);
   if(dwlength >999)
      dwlength = 999;

   memcpy(strText,pText,dwlength+1);


   strcpy_s(LastText,1000, strText);

   char *pRet = strstr(strText,"$mp");
   if(pRet != NULL)
   {
      int total = strlen(strText);
      int Last  = strlen(pRet);

      strText[total-Last] = 0x00;
      char strPos[100];
      sprintf_s(strPos,100,"%d,%d,%d",g_Global.GetLastMouseWorldX(),g_Global.GetLastMouseWorldY(),Player.World);
      strcat_s(strText,1000,strPos);

   }
   
   char *next_token1 = NULL;
   if (strText[0] == '!') 
   { 
      char *szCmd = strtok_s(strText, " ", &next_token1);
      if (!_stricmp(szCmd, "!RUN") && Player.CanRunScripts && g_bRunCommand == FALSE) 
      {
         g_bRunCommand = TRUE;
         sprintf_s(g_szFileName,512,"%s",strtok_s(NULL, " ", &next_token1));
         _beginthread(RunCommandThread, 0, NULL );
         
      } 
      if (!_stricmp(szCmd, "!STOP") && Player.CanRunScripts && g_bRunCommand == TRUE) 
      {
         g_bRunCommand = FALSE;
         Sleep(100);
      } 
      else if (!_stricmp(szCmd, "!SLEEP")) 
      {
         char *szTime = strtok_s(NULL, " ", &next_token1);
         if (szTime)
            Sleep(atoi(szTime));
      }

      else if (!_stricmp(szCmd, "!AFK")) 
      {
         
         char *szAFKCmd = strtok_s(NULL, "", &next_token1);
         char szTemp[256];
         
         if(szAFKCmd)
         {
            
            if (!_stricmp(szAFKCmd, "VIEW"))
            {
               char szAFK[5];
               if(pConfig->dwAfkStatus)
                  sprintf_s(szAFK,5,"on");
               else
                  sprintf_s(szAFK,5,"off");
               
               sprintf_s(szTemp,256, g_LocalString[628], szAFK, pConfig->strAfkMessage);
               V3_ChatLogDlg::GetInstance()->AddListLogs("", szTemp, CL_YELLOW, true);
            }
            else if (!_stricmp(szAFKCmd, "ON"))
            {
               pConfig->dwAfkStatus = 1;
               sprintf_s(szTemp,256, g_LocalString[625]);
               V3_ChatLogDlg::GetInstance()->AddListLogs("", szTemp, CL_YELLOW, true);
            } 
            else if (!_stricmp(szAFKCmd, "OFF"))
            {
               pConfig->dwAfkStatus = 0;               
               sprintf_s(szTemp,256, g_LocalString[626]);
               V3_ChatLogDlg::GetInstance()->AddListLogs("", szTemp, CL_YELLOW, true);
            } 
            else
            {
               sprintf_s(pConfig->strAfkMessage,2048,"%s",szAFKCmd);
               sprintf_s(szTemp,256, g_LocalString[627], szAFKCmd);
               V3_ChatLogDlg::GetInstance()->AddListLogs("", szTemp, CL_YELLOW, true);
            }
         }
         else
         {
            V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[619], CL_YELLOW, true);
            V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[620], CL_YELLOW, true);
            V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[621], CL_YELLOW, true);
            V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[622], CL_YELLOW, true);
            V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[623], CL_YELLOW, true);
            V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[624], CL_YELLOW, true);
         }
         
      }


      

      else if (!_stricmp(szCmd, "!Pac") && Player.CanRunScripts) 
      {
         //V3_PacmanDlg::GetInstance()->StartGame();
         //A Mettre en PACKET...
         //TFCPacket sending;
         //sending << (RQ_SIZE)RQ_NM_ GetAHList;
         //SEND_PACKET( sending );
      }

      else if (!_stricmp(szCmd, "!AH") && Player.CanRunScripts) 
      {
         TFCPacket sending;
         sending << (RQ_SIZE)RQ_NM_GetAHList;
         SEND_PACKET( sending );
      }
      /*
      else if (!_stricmp(szCmd, "!Questbook") && Player.CanRunScripts) 
      {
         //si deja afficher on ne redemande pas car sa ne chnagera jamais sauf au reboot serveur
         TFCPacket packet;
         packet << (RQ_SIZE)RQ_QB_GetActiveQuest;
         SEND_PACKET( packet );
      }
      */

      else if (!_stricmp(szCmd, "!Clear")) 
      {
         V3_ChatLogDlg::GetInstance()->ClearListLogs();
      } 
      else if (!_stricmp(szCmd, "!1April")&& Player.CanRunScripts) 
      {
         if(g_Var.dw1stAprilCnt == 0)
         {
            g_Var.wBubbleAnimCnt01 = 1;
            g_Var.wBubbleAnimCnt01R= 0;
            g_Var.wBubbleAnimCnt02 = 5;
            g_Var.wBubbleAnimCnt02R= 0;
            g_Var.wBubbleAnimCnt03 = 10;
            g_Var.wBubbleAnimCnt03R= 0;
            g_Var.dw1stAprilCnt = 1;
            g_Var.iFishXPos     = -g_Var.vsfFish.GetWidth();
            g_Var.iFishYPos     = g_Global.GetScreenH()-(rand()%100)-(2*g_Var.vsfFish.GetHeight());
            g_Var.bDisplayedMessage = false;
            sprintf_s(g_Var.strFishMessage,1024,"%s","Start 1st Apris fish demo.");
         }
      } 
      
      else if (!_stricmp(szCmd, "!FPS") /*&& Player.CanRunScripts*/) 
      {
         char *szFPSTime = strtok_s(NULL, " ", &next_token1);

         
         if(Custom.gShowFPS == 0)
         {
            Custom.gShowFPS = 1;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Show FPS.", CL_YELLOW, true);
         }
         else
         {
            Custom.gShowFPS = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Hide FPS.", CL_YELLOW, true);
         }
         pConfig->dwNMShowFPS = Custom.gShowFPS; 
      }
      else if (!_stricmp(szCmd, "!NoLight") && Player.CanRunScripts) 
      {
         char *szFPSTime = strtok_s(NULL, " ", &next_token1);


         if(Custom.gDebugNoLight == 0)
         {
            Custom.gDebugNoLight = 1;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Hide All Light.", CL_YELLOW, true);
         }
         else
         {
            Custom.gDebugNoLight = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Show All Light.", CL_YELLOW, true);
         }
      }

      else if (!_stricmp(szCmd, "!LightColor") && Player.CanRunScripts) 
      {
         char *szFPSTime = strtok_s(NULL, " ", &next_token1);
         
           
         if(Custom.gDebugLightTime == 0)
         {
            Custom.gDebugLightTime = 1;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Custom Time RGB Enable (!NMR 1-31)(!NMG 1-31)(!NMB 1-31)", CL_YELLOW, true);
         }
         else
         {
            Custom.gDebugLightTime = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Custom Time RGB Disabled", CL_YELLOW, true);
         }
      }
      else if (!_stricmp(szCmd, "!Day") && Player.CanRunScripts) 
      {
         char *szFPSTime = strtok_s(NULL, " ", &next_token1);
         if(Custom.gDebugLightDay == 0)
         {
            Custom.gDebugLightDay   = 1;
            Custom.gDebugLightNight = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Day Always!", CL_YELLOW, true);
         }
         else
         {
            Custom.gDebugLightDay = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Use server time", CL_YELLOW, true);
         }
      }
      else if (!_stricmp(szCmd, "!Night") && Player.CanRunScripts) 
      {
         char *szFPSTime = strtok_s(NULL, " ", &next_token1);
         if(Custom.gDebugLightNight == 0)
         {
            Custom.gDebugLightNight = 1;
            Custom.gDebugLightDay  = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Night Always!", CL_YELLOW, true);
         }
         else
         {
            Custom.gDebugLightNight = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Use server time", CL_YELLOW, true);
         }
      }
#if 0
      else if (!_stricmp(szCmd, "!Oups")) 
      {
         char *pstrTmp = new char[6000];
         memset(pstrTmp,0x00,6000);
         for(int i=0;i<5998;i++)
            pstrTmp[i] = ' ';

         for(int k=0;k<5;k++)
         { 
            Send << (short)RQ_Shout;
            Send << (short)strlen(Player.Name);
            Send << (char *)Player.Name;
            Send << (long)Custom.gTextColor;
            Send << (short)(strlen(pstrTmp));
            Send << (char *)&pstrTmp[0];
            SEND_PACKET(Send);
         }
      }
      else if (!_stricmp(szCmd, "!Oups2")) 
      {
         char *szAFKCmd = strtok_s(NULL, "", &next_token1);
         if(szAFKCmd)
         {
            char *pstrTmp = new char[6000];
            memset(pstrTmp,0x00,6000);
            for(int i=0;i<5998;i++)
               pstrTmp[i] = ' ';

            for(int k=0;k<5;k++)
            { 
               TFCPacket sending;
               sending << (RQ_SIZE)RQ_Page;
               sending << (short)strlen(szAFKCmd);
               sending << (char *)szAFKCmd;
               sending << (short)(strlen(pstrTmp));
               sending << (char *)&pstrTmp[0];;
               SEND_PACKET( sending );
            }
         }
      }
      else if (!_stricmp(szCmd, "!Oups3")) 
      {
         char *pstrTmp = new char[6000];
         memset(pstrTmp,0x00,6000);
         for(int i=0;i<5998;i++)
            pstrTmp[i] = ' ';

         Direction = 1;
         for(int k=0;k<5;k++)
         { 
            Send << (short)RQ_IndirectTalk;
            Send << (long)Player.ID;
            Send << (char)0;
            Send << (long)0xFF55AD;
            Send << (long)Player.lKey;
            Send << (short)strlen(pstrTmp);
            Send << (char *)pstrTmp;
            SEND_PACKET(Send); 
         }
      }
      else if (!_stricmp(szCmd, "!Oups4")) 
      {
         char strTmp[6000];
         for(int i=0;i<6000;i++)
            strTmp[i] = ' ';

         TFCPacket sending;           
         sending << (short)48;
         sending << (short)strlen(strTmp);
         sending << (char *)strTmp;
         sending << (short)strlen(strTmp);
         sending << (char *)strTmp;
         SEND_PACKET(sending); 
      }
#endif
      else if (!_stricmp(szCmd, "!HideGUI") && Player.CanRunScripts) 
      {
         char *szFPSTime = strtok_s(NULL, " ", &next_token1);
         if(Custom.gDebugGUIHidden == 0)
         {
            Custom.gDebugGUIHidden = 1;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "User Interface Hidden.", CL_YELLOW, true);
         }
         else
         {
            Custom.gDebugGUIHidden = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "User Interface showed.", CL_YELLOW, true);
         }
      }
      else if (!_stricmp(szCmd, "!ShowHideDecors") && Player.CanRunScripts) 
      {
         char *szFPSTime = strtok_s(NULL, " ", &next_token1);
         
         
         if(Custom.gHIDEAllDecors == 0)
         {
            Custom.gHIDEAllDecors = 1;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Hide all decors Done!", CL_YELLOW, true);
         } 
         else
         {
            Custom.gHIDEAllDecors = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "Show all decors Done!", CL_YELLOW, true);
         }
      }

      else if (!_stricmp(szCmd, "!Winterrize") && Player.CanRunScripts) 
      {
         char *szFPSTime = strtok_s(NULL, " ", &next_token1);
         
         
         if(Custom.gWinterrize == 0)
         {
            Custom.gWinterrize = 1;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "LH in Winter mode // teleport to refrest effect", CL_YELLOW, true);
         }
         else
         {
            Custom.gWinterrize = 0;
            V3_ChatLogDlg::GetInstance()->AddListLogs("", "LH in Summer mode // teleport to refrest effect", CL_YELLOW, true);
         }
      }

      else if (!_stricmp(szCmd, "!ClearArealink") && Player.CanRunScripts) 
      {
        
         for(int i=0;i<NBR_ZONE_MAP;i++)
            g_Global.m_vArealinkVector[i].clear();
         
         V3_ChatLogDlg::GetInstance()->AddListLogs("", "Done !", CL_YELLOW, true);
         
      }

      else if (!_stricmp(szCmd, "!SMILE")&& Player.CanRunScripts) 
      {

         char *szSmileID = strtok_s(NULL, "", &next_token1);

         if(szSmileID)
         {
            short shSmileID = atoi(szSmileID);
            if(shSmileID <1 || shSmileID >NMS_SMILES_MAX)
               shSmileID = 1;

            Send << (short)RQ_UpdateSmile;
            Send << (short)shSmileID;
            SEND_PACKET(Send);
         }
      }

      
       //CV:NM Draw TEST
      /*
      else if (!_stricmp(szCmd, "!FloodCC")) 
      {
         DWORD ThreadID;
         CreateThread(NULL, 0, FloodCCThread, NULL, 0, &ThreadID);
         //_beginthread( FloodCCThread, 0, NULL );
        
      }
      */
      
      
         

      

      else if (!_stricmp(szCmd, "!NMR")&& Player.CanRunScripts) 
      {
         
         char *szAFKCmd = strtok_s(NULL, "", &next_token1);
         
         if(szAFKCmd)
         {
            int iVal = atoi(szAFKCmd);
            if(iVal <1 || iVal >31)
               iVal = 31;
            Custom.gTIME_R = iVal;
         }
      }
      else if (!_stricmp(szCmd, "!NMG")&& Player.CanRunScripts) 
      {
         
         char *szAFKCmd = strtok_s(NULL, "", &next_token1);
         
         if(szAFKCmd)
         {
            int iVal = atoi(szAFKCmd);
            if(iVal <1 || iVal >31)
               iVal = 31;
            Custom.gTIME_G = iVal;
         }
      }
      else if (!_stricmp(szCmd, "!NMB")&& Player.CanRunScripts) 
      {
         
         char *szAFKCmd = strtok_s(NULL, "", &next_token1);
         
         if(szAFKCmd)
         {
            int iVal = atoi(szAFKCmd);
            if(iVal <1 || iVal >31)
               iVal = 31;
            Custom.gTIME_B = iVal;
         }
      }
      
      //the strlen>2 avoids smileys :), :p to be displayed in main channel as ) or p
      //exception is for the word : k often used as Okay
     } else if ( strText[0] == ':' && ( (strlen(strText)>2)||(strText[1]=='k')||(strText[1]=='K') ) ) { 
        
        Send << (short)RQ_Shout;
        Send << (short)strlen(Player.Name);
        Send << (char *)Player.Name;
        Send << (long)Custom.gTextColor;
        Send << (short)(strlen(strText)-1);
        Send << (char *)&strText[1];
        SEND_PACKET(Send);
        
        
        
        //								} else if (strText[0] == ';') {
        //									u_Chatter.ParseCommand(&strText[1]);
     } 
     else if (strText[0] == '/') 
     {
        OptionParam *pOption = g_SaveGame.GetOptionsParam();
        if (pOption->bPageEnable) 
        {
           BOOL MultiName = FALSE;
           Send << (short)RQ_Page;
           char Token[2500];
           for (UINT i = 1; i < strlen(strText); i++) {
              if (strText[i] == '\"' && i == 1) {
                 MultiName = TRUE;
              } else if (strText[i] == ' ' && !MultiName) {
                 Token[i-(MultiName ? 2 : 1)] = 0;
                 i = strlen(strText);
              } else if (strText[i] == '\"' && MultiName) {
                 Token[i-(MultiName ? 2 : 1)] = 0;
                 i = strlen(strText);
              } else {
                 Token[i-(MultiName ? 2 : 1)] = strText[i];
              }
           }         
            
           
           Send << (short)(strlen(Token));
           Send << (char *)Token;
           if (strlen(&strText[strlen(Token)+1+(MultiName ? 2 : 0)])) 
           {

              Send << (short)strlen(&strText[strlen(Token)+1+(MultiName ? 2 : 0)]);            
              Send << (char *)&strText[strlen(Token)+1+(MultiName ? 2 : 0)];
              //str += &strText[strlen(Token)+1+(MultiName ? 2 : 0)];
           } else {
              Send << (short)1;
              Send << (char *)" ";
              //str += " ";
           }
           // To avoid paging while AFK.
           if(!pConfig->dwAfkStatus)
           {
              if (!V3_ChatDlg::GetInstance()->IsUserIgnored( Token ))  //page a ignorer user
              {
                 SEND_PACKET(Send);
              } 
              else 
              {
                 V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[517], CL_YELLOW, true, false );
              }
           } 
           else 
           {
              V3_ChatLogDlg::GetInstance()->AddListLogs( "", "You cannot page while AFK!", CL_YELLOW, true, false);
           }
           
           
        } 
        else 
        {
           V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[100],CL_RED,true);
        }
        //the strlen>2 avoids smileys :), :p to be displayed in main channel as ) or p
        //exception is for the word : k often used as Okay
     }
     else if ( strText[0] == ';'  && ( (strlen(strText)>2)||(strText[1]=='k')||(strText[1]=='K') ) ) 
     { 
        V3_MainBarDlg::GetInstance()->SendMessageToCurrentChannel(&strText[1]);
     } 
     else if ((TalkToX || TalkToY) && boNPCisThere) {
        if (strText[0]) {
           Send << (short)RQ_DirectedTalk;
           Send << (short)(TalkToX);
           Send << (short)(TalkToY);
           TalkToOffset = 0;
           Send << (long)TalkToID;
           Direction = 1;
           int a = ((TalkToX-Player.xPos)+11)*3;
           int b = ((TalkToY-Player.yPos)+16)*2;
           if	(b > 30) {
              if (a > 30) {
                 a -= 30;
                 b -= 30;
                 if (a > b*2) {
                    Direction = 6;
                 } else if (b > a*2) {
                    Direction = 2;
                 } else {
                    Direction = 3;
                 }
              } else {
                 b -= 30;
                 a = 30 - a;
                 if (a > b*2) {
                    Direction = 4;
                 } else if (b > a*2) {
                    Direction = 2;
                 } else {
                    Direction = 1;
                 }
              }
           } else {
              if (a > 30) {
                 a -= 30;
                 b = 30 - b;
                 if (a > b*2) {
                    Direction = 6;
                 } else if (b > a*2) {
                    Direction = 8;
                 } else {
                    Direction = 9;
                 }	
              } else {
                 a = 30 - a;
                 b = 30 - b;
                 if (a > b*2) {
                    Direction = 4;
                 } else if (b > a*2) {
                    Direction = 8;
                 } else {
                    Direction = 7;
                 }
              }			
           }
           Objects.Lock(28);
           Objects.Unlock(28);
           Send << (char)Direction;
           Send << (long)Custom.gTextColor;
           Send << (long)Player.lKey;
           Send << (short)strlen(strText);
           Send << (char *)strText;
           SEND_PACKET(Send);
        } else {
           Send << (short)RQ_BroadcastTextChange;
           Send << (long)TalkToID;
           Send << (short)TalkToOffset;
           SEND_PACKET(Send);
        } 
     } else { 
        Send << (short)RQ_IndirectTalk;
        Send << (long)Player.ID;
        Send << (char)0;
        Send << (long)Custom.gTextColor;
        Send << (long)Player.lKey;
        Send << (short)strlen(strText);
        if (strText[0])
           Send << (char *)strText;
        SEND_PACKET(Send); 
     } 
     strText[0] = 0;
}


void DrawFisrtLoadingText()
{
   static int dwCntFLT = 2;
   HDC hdc;
   DXDGetDCConnect(&hdc, 28);
     
   int dwX = (g_Global.GetScreenW()-fT4CDef_22->GetFLen(hdc,g_LocalString[1]))/2;
   //int dwY = 20;
   int dwY = g_Global.GetScreenH()-80;
   fT4CDef_22->DrawFont(hdc, dwX,dwY , CL_WHITE, 0, g_LocalString[1]);

   char strVersion[50];
   sprintf_s(strVersion,50,"%.02f %s",Version::GetVersion()/1000.00,Version::GetSubVersion());
    
   char *Text  = new char [100];
   char *Text2 = new char [100];
   char *Text3 = new char [100];
   strcpy_s(Text,100, g_LocalString[9]);
   strcat_s(Text,100, " ");
   strcat_s(Text,100, strVersion/*g_LocalString[10]*/);
   
   int dwCnt = 0;
   int i=0;
   for(i=0;i<dwCntFLT;i+=2)
   {
      Text3[i+0] = '.';
      Text3[i+1] = ' ';
   }
   Text3[i] = 0x00;
   dwCntFLT++;
   if(dwCntFLT >= 60)
      dwCntFLT = 2;
   
   dwX = (g_Global.GetScreenW()-fT4CDef_22->GetFLen(hdc,Text))/2;
   dwY = dwY+5+fT4CDef_22->GetFHeight(hdc);
   fT4CDef_22->DrawFont(hdc, dwX, dwY, CL_WHITE, 0, Text);

   dwY = g_Global.GetScreenH()-60;
   dwX = (g_Global.GetScreenW()-fSystem_22->GetFLen(hdc,Text3))/2;
   fSystem_22->DrawFont(hdc, dwX, dwY+15 +fSystem_22->GetFHeight(hdc), CL_WHITE, 0, Text3);
   
   delete Text;
   delete Text2;
   delete Text3;
   
   DXDReleaseDC(hdc, 28);
}

void DrawSecondLoadingText()
{
   static int dwCntFLT = 2;
   HDC hdc;
   DXDGetDCConnect(&hdc, 29);
   
   int dwX = (g_Global.GetScreenW()-fT4CDef_22->GetFLen(hdc,g_LocalString[1]))/2;
   int dwY = g_Global.GetScreenH()-40;
   fT4CDef_22->DrawFont(hdc, dwX,dwY , CL_WHITE, 0, g_LocalString[1]);
   
   char *Text2 = new char [100];
   char *Text3 = new char [100];
   
   int dwCnt = 0;
   int i=0;
   for(i=0;i<dwCntFLT;i+=2)
   {
      Text3[i+0] = '.';
      Text3[i+1] = ' ';
   }
   Text3[i] = 0x00;
   dwCntFLT++;
   if(dwCntFLT >= 60)
      dwCntFLT = 2;
   
   dwX = (g_Global.GetScreenW()-fSystem_22->GetFLen(hdc,Text3))/2;
   fSystem_22->DrawFont(hdc, dwX, g_Global.GetScreenH()-20 , CL_WHITE, 0, Text3);
   
   delete Text2;
   delete Text3;
   
   DXDReleaseDC(hdc, 29);
}



ULONG WINAPI  FloodCCThread(LPVOID pParam)
{
   if(g_FloodCCThreadActif)
      return 0;

   g_FloodCCThreadActif = TRUE;
   static int iCnt = 0;
   
   char strMesage[1024];
   for(int i=0;i<100;i++)
   {
      sprintf_s(strMesage,1024,"Test d'un flood Multi ligne, ben ouais je c pas trop koi ecrire, mais bon c assez bizarre, mais pas le choix nek me demande le vilain :P Test d'un flood Multi ligne, ben ouais je c pas trop koi ecrire, mais bon c assez bizarre, mais pas le choix nek me demande le vilain :P [%05d] [%03d]",iCnt,i);

      V3_ChatLogDlg::GetInstance()->AddListLogs("yo man", strMesage, CL_YELLOW, true,true);
      Sleep(1);
   }
   iCnt++;

   g_FloodCCThreadActif = FALSE;
   return 0;
}
 

void FirstInitCreate(LPVOID pParam)
{
   IDirectXImage diLoading;
   HRESULT hrLoading;
   try 
   {
      //Isp.PCX for 800 or 1024...
      char strISPName[128];
      sprintf_s(strISPName,128,"Images\\start%d%d.PCX",g_Global.GetDisplayW(),g_Global.GetDisplayH());
      hrLoading = diLoading.LoadImage(strISPName,g_Global.GetDisplayW(),g_Global.GetDisplayH());
      if (hrLoading) 
      {
         DXDBlt(diLoading);
         DXDFlip(); //first init
         //diLoading.Release();
      }
   } catch (...) 
   {
   };
   
   

   
   FontHandler *fh = new FontHandler;
   
   UniqueFontHandler = fh;
   
   fh->Initialize();
   FontManager::GetInstance()->Initialize();
   
 
   
   double TempFontHeight = 0.00;

   std::string strFont = "T4C BeaulieuxV2";
   //std::string strFont = "courrier";


   //calcule la hauteau d<un font ???
   Font *pFontTmp = fh->GetFont((char*)strFont.c_str());
   HDC hdc;
   pFontTmp->CreateF();
   DXDGetDCConnect(&hdc, 30);
   TempFontHeight = (double)pFontTmp->GetFHeight(hdc);
   DXDReleaseDC(hdc, 30);


   V3_BtnFontB     = fh->GetFont("Verdana");
   fSmallNormal_12 = fh->GetFont("Verdana");
   fSmallNormal_13 = fh->GetFont("Verdana");
   fSmallNormal_14 = fh->GetFont("Verdana");
   fT4CDef_21	   = fh->GetFont("Verdana");
   fT4CDef_22	   = fh->GetFont((char*)strFont.c_str());
   fT4CDef_23B     = fh->GetFont((char*)strFont.c_str());
   fT4CDef_30B     = fh->GetFont((char*)strFont.c_str());
   fT4CDef_50B     = fh->GetFont((char*)strFont.c_str());
   fSystem_22      = fh->GetFont("System");
   fT4CDef_18      = fh->GetFont((char*)strFont.c_str());

  
   
   //cv: Mod pas besoin de creer ici le sera dans le FromPreInGameToInGame
   //Objects.CreateTorch();

   

   V3_BtnFontB->SetSize(6,13,400);
   V3_BtnFontB->CreateF();
   fSmallNormal_12->SetSize(6,12);
   fSmallNormal_12->CreateF();
   fSmallNormal_13->SetSize(6,13);
   fSmallNormal_13->CreateF();
   fSmallNormal_14->SetSize(6,14);
   fSmallNormal_14->CreateF();
   
   fT4CDef_21->SetSize(5,16,600);
   fT4CDef_21->CreateF();
   fT4CDef_22->SetSize((22.0)/TempFontHeight);
   fT4CDef_22->CreateF();
   fT4CDef_23B->SetSize(5,23,600);
   fT4CDef_23B->CreateF();
   fT4CDef_30B->SetSize(10,32,600);
   fT4CDef_30B->CreateF();
   fT4CDef_50B->SetSize(20,54,600);
   fT4CDef_50B->CreateF();

   fSystem_22->SetSize((8.0)/TempFontHeight);
   fSystem_22->CreateF();

   fT4CDef_18->SetSize(6,16,400);
   fT4CDef_18->CreateF();
  
   
  
   
   
   
   
   
 
   
   
   g_bFirstLoadComplete = FALSE;
   if (hrLoading)
   {
      DXDBlt(diLoading);
      DrawFisrtLoadingText();
      DXDFlip();
   }
   // Load assets on this thread: parallel LoadSprite + DXDFlip deadlocks DirectDraw on Win10+.
   FirstInitObject(NULL);
   
   // NMNMNM_Size_ 18 megs deja...
   int iCntMin = 30;
   while(iCntMin > 0)
   { 
      Sleep(100);
      if(hrLoading)
         DXDBlt(diLoading);
      DrawFisrtLoadingText();
      DXDFlip(); //attente first init
      iCntMin--;
   }

 
   //STI
   // NMNMNM_Size_ 60 megs deja...
   if(hrLoading) 
      diLoading.Release();


   
  

   
    
   TIMECAPS caps;
   timeGetDevCaps( &caps, sizeof(caps) );
   timeBeginPeriod( caps.wPeriodMin );
   
   
   ZeroMemory(qwPacketUseCounter, 150*sizeof(QWORD));
   ZeroMemory(qwPacketUseTime, 150*sizeof(QWORD));
   
   inGameTab = TRUE;

   

   // Start le thread de GetMessage.
   unsigned int iID;
   hMaintenanceThread = (HANDLE) _beginthreadex(NULL, 0, MaintenanceThread, NULL, NULL, &iID);

   g_FirstInitCreateComplete = TRUE;
}

void FirstInitObject(LPVOID pParam)
{
   SoundFX[6].Create("Open Box", TS_MEMORY);
   SoundFX[110].Create("Equip", TS_MEMORY);
   SoundFX[44].Create("Vampire Dying", TS_MEMORY);

  
   AttackCursorIcon.LoadSprite("StaticAttackCursor");    //dessiner quand mode combat activer...


   V3MouseCursor1.LoadSprite("V3_InterfaceCursorS");
   V3MouseCursor1A.LoadSprite("V3_InterfaceCursorSMask");

   V3MouseCursor2.CreateSpriteMain( "sword0000","sword0000Mask");
   V3MouseCursor2.AddFrame(         "sword0002","sword0002Mask");
   V3MouseCursor2.AddFrame(         "sword0004","sword0004Mask");
   V3MouseCursor2.AddFrame(         "sword0006","sword0006Mask");
   V3MouseCursor2.AddFrame(         "sword0008","sword0008Mask");
   V3MouseCursor2.AddFrame(         "sword0010","sword0010Mask");
   V3MouseCursor2.AddFrame(         "sword0012","sword0012Mask");
   V3MouseCursor2.AddFrame(         "sword0014","sword0014Mask");
   V3MouseCursor2.AddFrame(         "sword0016","sword0016Mask");
   V3MouseCursor2.AddFrame(         "sword0018","sword0018Mask");
   V3MouseCursor2.AddFrame(         "sword0020","sword0020Mask");
   V3MouseCursor2.AddFrame(         "sword0022","sword0022Mask");

   V3MouseCursor3.CreateSpriteMain( "bowc0000","bowc0000Mask");
   V3MouseCursor3.AddFrame(         "bowc0002","bowc0002Mask");
   V3MouseCursor3.AddFrame(         "bowc0004","bowc0004Mask");
   V3MouseCursor3.AddFrame(         "bowc0006","bowc0006Mask");
   V3MouseCursor3.AddFrame(         "bowc0008","bowc0008Mask");
   V3MouseCursor3.AddFrame(         "bowc0010","bowc0010Mask");
   V3MouseCursor3.AddFrame(         "bowc0012","bowc0012Mask");
   V3MouseCursor3.AddFrame(         "bowc0014","bowc0014Mask");
   V3MouseCursor3.AddFrame(         "bowc0016","bowc0016Mask");
   V3MouseCursor3.AddFrame(         "bowc0018","bowc0018Mask");
   V3MouseCursor3.AddFrame(         "bowc0020","bowc0020Mask");
   V3MouseCursor3.AddFrame(         "bowc0022","bowc0022Mask");


   V3MouseCursor5.LoadSprite("V3_North Cursor");
   V3MouseCursor5A.LoadSprite("V3_North CursorMask");
   V3MouseCursor6.LoadSprite("V3_North-East Cursor");
   V3MouseCursor6A.LoadSprite("V3_North-East CursorMask");
   V3MouseCursor7.LoadSprite("V3_East Cursor");
   V3MouseCursor7A.LoadSprite("V3_East CursorMask");
   V3MouseCursor8.LoadSprite("V3_South-East Cursor");
   V3MouseCursor8A.LoadSprite("V3_South-East CursorMask");
   V3MouseCursor9.LoadSprite("V3_South Cursor");
   V3MouseCursor9A.LoadSprite("V3_South CursorMask");
   V3MouseCursor10.LoadSprite("V3_South-West Cursor");
   V3MouseCursor10A.LoadSprite("V3_South-West CursorMask");
   V3MouseCursor11.LoadSprite("V3_West Cursor");
   V3MouseCursor11A.LoadSprite("V3_West CursorMask");
   V3MouseCursor12.LoadSprite("V3_North-West Cursor");
   V3MouseCursor12A.LoadSprite("V3_North-West CursorMask");
   V3MouseCursor17.LoadSprite("V3_TalkCursor");
   V3MouseCursor17A.LoadSprite("V3_TalkCursorMask");
   V3MouseCursor19.LoadSprite("V3_MoveCursor");
   V3MouseCursor19A.LoadSprite("V3_MoveCursorMask");
   V3MouseCursor20.LoadSprite("V3_SizeUP");
   V3MouseCursor20A.LoadSprite("V3_SizeUPMask");
   V3MouseCursor21.LoadSprite("V3_SizeSide");
   V3MouseCursor21A.LoadSprite("V3_SizeSideMask");
   V3MouseCursor22.LoadSprite("glove0000");
   V3MouseCursor22A.LoadSprite("glove0000Mask");


   //cursor animer
   V3MouseCursor16.CreateSpriteMain("glove0000","glove0000Mask");
   V3MouseCursor16.AddFrame( "glove0001", "glove0001Mask");
   V3MouseCursor16.AddFrame( "glove0002", "glove0002Mask");
   V3MouseCursor16.AddFrame( "glove0003", "glove0003Mask");
   V3MouseCursor16.AddFrame( "glove0004", "glove0004Mask");
   V3MouseCursor16.AddFrame( "glove0005", "glove0005Mask");
   V3MouseCursor16.AddFrame( "glove0006", "glove0006Mask");
   V3MouseCursor16.AddFrame( "glove0007", "glove0007Mask");
   V3MouseCursor16.AddFrame( "glove0008" ,"glove0008Mask");
   V3MouseCursor16.AddFrame( "glove0009" ,"glove0009Mask");
   V3MouseCursor16.AddFrame( "glove0010" ,"glove0010Mask");
   V3MouseCursor16.AddFrame( "glove0011" ,"glove0011Mask");



   
   /*
   MouseCursor3.CreateSprite("64kCursorBow-a");
   MouseCursor3.AddFrame( "64kCursorBow-b" );
   MouseCursor3.AddFrame( "64kCursorBow-c" );
   MouseCursor3.AddFrame( "64kCursorBow-d" );
   MouseCursor3.AddFrame( "64kCursorBow-e" );
   MouseCursor3.AddFrame( "64kCursorBow-f" );
   MouseCursor3.AddFrame( "64kCursorBow-g" );
   MouseCursor3.AddFrame( "64kCursorBow-h" );
   MouseCursor3.AddFrame( "64kCursorBow-i" );
   MouseCursor3.AddFrame( "64kCursorBow-j" );
   MouseCursor3.AddFrame( "64kCursorBow-k" );
   */
   
   
   
   
   
   
  
   
   
  
   MouseCursor18.CreateSprite("DoorCursor00");
   MouseCursor18.AddFrame( "DoorCursor01" );
   MouseCursor18.AddFrame( "DoorCursor02" );
   MouseCursor18.AddFrame( "DoorCursor03" );
   MouseCursor18.AddFrame( "DoorCursor04" );
   MouseCursor18.AddFrame( "DoorCursor05" );
   MouseCursor18.AddFrame( "DoorCursor06" );
   MouseCursor18.AddFrame( "DoorCursor07" );
   MouseCursor18.AddFrame( "DoorCursor08" );
   MouseCursor18.AddFrame( "DoorCursor09" );
   MouseCursor18.AddFrame( "DoorCursor10" );
   MouseCursor18.AddFrame( "DoorCursor11" );

   
   
   
   
   
   g_Var.vsfDisk     .CreateSprite("DiskAccess");
   g_Var.vsfMap      .CreateSprite("MapLoading");
   g_Var.vsfNetwork  .CreateSprite("NetworkAccess");
   g_Var.vsfProgressB[0].CreateSprite("V3_EnterProgressBar");
   g_Var.vsfProgressB[1].CreateSprite("V3_EnterProgressBarMask");
   g_Var.vsfProgressT[0].CreateSprite("V3_EnterProgressTick");
   g_Var.vsfProgressT[1].CreateSprite("V3_EnterProgressTickMask");
   g_Var.vsfXpScroll .CreateSprite("64kIconScrollXP");
   g_Var.vsfORScroll .CreateSprite("64kIconScrollGold");
   
   g_Var.wDisplayDisk    = 0;
   g_Var.wDisplayMap     = 0;
   g_Var.wProgressCnt    = 0;
   g_Var.wDisplayMapZone = 0;
   g_Var.wProgressInc = 1;


   g_Var.vsfGame01[ 0].CreateSprite("PacGame00");
   g_Var.vsfGame01[ 1].CreateSprite("PacGame01");
   g_Var.vsfGame01[ 2].CreateSprite("PacGame02");
   g_Var.vsfGame01[ 3].CreateSprite("PacGame03");
   g_Var.vsfGame01[ 4].CreateSprite("PacGame04");
   g_Var.vsfGame01[ 5].CreateSprite("PacGame05");
   g_Var.vsfGame01[ 6].CreateSprite("PacGame06");
   g_Var.vsfGame01[ 7].CreateSprite("PacGame07");
   g_Var.vsfGame01[ 8].CreateSprite("PacGame08");
   g_Var.vsfGame01[ 9].CreateSprite("PacGame09");
   g_Var.vsfGame01[10].CreateSprite("PacGame10");
   g_Var.vsfGame01[11].CreateSprite("PacGame11");
   g_Var.vsfGame01[12].CreateSprite("PacGame12");
   g_Var.vsfGame01[13].CreateSprite("PacGame13");
   g_Var.vsfGame01[14].CreateSprite("PacGame14");
   g_Var.vsfGame01[15].CreateSprite("PacGame15");
   g_Var.vsfGame01[16].CreateSprite("PacGame16");
   g_Var.vsfGame01[17].CreateSprite("PacGame17");
   g_Var.vsfGame01[18].CreateSprite("PacGame18");
   g_Var.vsfGame01[19].CreateSprite("PacGame19");
   g_Var.vsfGame01[20].CreateSprite("PacGame20");
   g_Var.vsfGame01[21].CreateSprite("PacGame21");
   g_Var.vsfGame01[22].CreateSprite("PacGame22");
   g_Var.vsfGame01[23].CreateSprite("PacGame23");
   g_Var.vsfGame01[24].CreateSprite("PacGame24");
   g_Var.vsfGame01[25].CreateSprite("PacGame25");
   g_Var.vsfGame01[26].CreateSprite("PacGame26");
   g_Var.vsfGame01[27].CreateSprite("PacGame27");
   g_Var.vsfGame01[28].CreateSprite("PacGame28");
   g_Var.vsfGame01[29].CreateSprite("PacGame29");

   g_Var.vsfBubble[ 0].CreateSprite("BubbleBack");
   g_Var.vsfBubble[ 1].CreateSprite("Bubble-a");
   g_Var.vsfBubble[ 2].CreateSprite("Bubble-b");
   g_Var.vsfBubble[ 3].CreateSprite("Bubble-c");
   g_Var.vsfBubble[ 4].CreateSprite("Bubble-d");
   g_Var.vsfBubble[ 5].CreateSprite("Bubble-e");
   g_Var.vsfBubble[ 6].CreateSprite("Bubble-f");
   g_Var.vsfBubble[ 7].CreateSprite("Bubble-g");
   g_Var.vsfBubble[ 8].CreateSprite("Bubble-h");
   g_Var.vsfBubble[ 9].CreateSprite("Bubble-i");
   g_Var.vsfBubble[10].CreateSprite("Bubble-j");
   g_Var.vsfBubble[11].CreateSprite("Bubble-k");
   g_Var.vsfBubble[12].CreateSprite("Bubble-l");
   g_Var.vsfBubble[13].CreateSprite("Bubble-m");
   g_Var.vsfBubble[14].CreateSprite("Bubble-n");
   g_Var.vsfBubble[15].CreateSprite("Bubble-o");
   g_Var.vsfBubble[16].CreateSprite("Bubble-p");
   g_Var.vsfBubble[17].CreateSprite("Bubble-q");

   g_Var.vsfFish.CreateSprite("1StAprilFish");



   
   

   char strNameTmp2[128];
   sprintf_s(strNameTmp2,128,"T4COption%d",g_Global.GetScreenW());
   Splash.LoadSprite(strNameTmp2);
   
   
   
   
    // the connection menu will wait the end of loading...
   
  
   // Set World Information.
   World.SetTileDimension   (32,   16  );
   World.SetViewDimension   (g_Global.GetNbrTileX(),   g_Global.GetNbrTileY());
   Grid.SetBitmap           (g_Global.GetNbrTileX()+1, g_Global.GetNbrTileY()+1  );
   GridID.SetBitmap         (g_Global.GetNbrTileX()+1, g_Global.GetNbrTileY()+1  );
   GridPriority.SetBitmap	 (g_Global.GetNbrTileX()+1, g_Global.GetNbrTileY()+1  );
   GridBlocking.SetBitmap   (g_Global.GetNbrTileX()+1, g_Global.GetNbrTileY()+1  );
   World.SetVirtualDimension(256,  256);
   World.SetWorldDimension  (3072, 3072);
   World.SetNumberofWorld   (8);	//Was set to 4
   
   g_bFirstLoadComplete = TRUE;
}

void SecondInitObject(LPVOID pParam)
{
   
   World.CreateView();

   Objects.Lock(0);
   Objects.Create(); // Create the Game Object.
   Objects.Unlock(0);
  

   g_bSecondLoadComplete = TRUE;
   
}

void SmoothInitObject(LPVOID pParam)
{
   World.CreateSmooth();
   g_bSmoothFloorLoading = TRUE;
}


void ExitThreadGame(LPVOID pParam)
{
   //g_LocalString[ 444 ]
   char strMessage[128];
   for(int i=5;i>0;i--) //NADNADNAD
   {
      sprintf_s(strMessage,128,"%s%d sec.",g_GUILocalString[169],i);
      V3_ChatLogDlg::GetInstance()->AddListLogs("",strMessage, SYSTEM_COLOR, true);
      Sleep(1000);
   }
   PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
}  

void RunCommandThread(LPVOID pParam)
{
   FILE *hFile = NULL;
   fopen_s(&hFile,g_szFileName, "rt");
   if (!hFile) 
   {
      char szTemp[1000];
      sprintf_s(szTemp,1000, "Error: File %s cannot be opened.", g_szFileName);
      
      V3_ChatLogDlg::GetInstance()->AddListLogs("",szTemp,CL_RED,true);
   } 
   else 
   {
      char szTemp[1000];
      while (fgets(szTemp, 255, hFile) && g_bRunCommand) 
      {
         if (szTemp[strlen(szTemp)-1] == 10 || szTemp[strlen(szTemp)-1] == 13)
            szTemp[strlen(szTemp)-1] = NULL;

         ParseText(szTemp);
         Sleep(50);
      }
      fclose(hFile);
   }
   g_bRunCommand = FALSE;
} 
         
bool PlayMp3NewSong(int iMode,HWND &hMp3)
{
   if(iMode == 0)
   {
      if(g_SaveGame.GetOptionsParam()->bMp3Random)
         Custom.gdwCurrentSongIndex = rand()%Custom.gMp3List.size();
      else
         Custom.gdwCurrentSongIndex = 0;
   }
   else if(iMode == 1)
   {
      if(g_SaveGame.GetOptionsParam()->bMp3Random)
         Custom.gdwCurrentSongIndex = rand()%Custom.gMp3List.size();
      else
          Custom.gdwCurrentSongIndex++;

      if((unsigned int)Custom.gdwCurrentSongIndex >= Custom.gMp3List.size())
         Custom.gdwCurrentSongIndex = 0;
   }
   else if(iMode == 2)
   {
      if(g_SaveGame.GetOptionsParam()->bMp3Random)
         Custom.gdwCurrentSongIndex = rand()%Custom.gMp3List.size();
      else
         Custom.gdwCurrentSongIndex--;
      
      if(Custom.gdwCurrentSongIndex < 0)
         Custom.gdwCurrentSongIndex = 0;
   }

   
   if(hMp3 !=NULL)
   {
	   MCIWndStop(hMp3);
	   MCIWndDestroy(hMp3);
	   hMp3 = NULL;
   }


   hMp3 = MCIWndCreate(GlobalHwnd,NULL,
					   MCIWNDF_NOAUTOSIZEWINDOW|MCIWNDF_NOAUTOSIZEMOVIE|MCIWNDF_NOERRORDLG|
					   MCIWNDF_NOMENU|MCIWNDF_NOOPEN|MCIWNDF_NOPLAYBAR|WS_MINIMIZE,
					   Custom.gMp3List[Custom.gdwCurrentSongIndex].strName);

    if(hMp3)
	{
		::ShowWindow(hMp3,SW_HIDE);
      MCIWndSetVolume(hMp3,g_SaveGame.GetOptionsParam()->iMp3Volume*10);
		MCIWndPlay(hMp3);
		return true;
	}
	return false;
}

void  Mp3PlayerThread(LPVOID pParam)
{
	char strTextTmp[1024];

	HANDLE hEvents[7];
	hEvents[0] = g_hMp3EventExit;
	hEvents[1] = g_hMp3EventNext;
	hEvents[2] = g_hMp3EventPrev;
	hEvents[3] = g_hMp3EventPlay;
	hEvents[4] = g_hMp3EventPaused;
	hEvents[5] = g_hMp3EventVol;
	hEvents[6] = g_hMp3EventDisable;



	HWND m_hMp3 = NULL;

	BOOL bMp3ForceExit = FALSE;
	while(!bMp3ForceExit)
	{
		DWORD dwEvents = ::WaitForMultipleObjects(7,hEvents,FALSE,100);
		switch(dwEvents)
		{ 
			case WAIT_OBJECT_0+0 ://Exit
				bMp3ForceExit = TRUE;
			break;
			case WAIT_OBJECT_0+1 : //Next
			case WAIT_OBJECT_0+2 : //Prev
			{	
				 int iNextAction = 1;
				 if(dwEvents == WAIT_OBJECT_0+2)
					 iNextAction = 2;
				 if(Custom.gMp3List.size() >0)
				 {
					 if(PlayMp3NewSong(iNextAction,m_hMp3))
					 {
						 g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_PLAY;
						 Custom.dwLastMp3PlayerState = Running;
						 sprintf_s(strTextTmp,1024,"Play [%s]",Custom.gMp3List[Custom.gdwCurrentSongIndex].strName);
						 V3_ChatLogDlg::GetInstance()->AddListLogs("", strTextTmp, CL_ORANGE, true);
					 }
					 else
					 {
						 g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_DISABLE;
						 Custom.dwLastMp3PlayerState = Stopped;
					 }
				 }
			 }
			break;
			case WAIT_OBJECT_0+3 : //Play
				if(Custom.gMp3List.size() >0)
				{
					if(g_SaveGame.GetOptionsParam()->bStatusMp3 != MP3_PAUSED)
					{
						if(PlayMp3NewSong(0,m_hMp3))
						{
							g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_PLAY;
							g_GameMusic.Reset();
							Custom.dwLastMp3PlayerState = Running;
							sprintf_s(strTextTmp,1024,"Play [%s]",Custom.gMp3List[Custom.gdwCurrentSongIndex].strName);
							V3_ChatLogDlg::GetInstance()->AddListLogs("", strTextTmp, CL_ORANGE, true);
						}
						else
						{
							g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_DISABLE;
							Custom.dwLastMp3PlayerState = Stopped;
						}
					}
					else if(g_SaveGame.GetOptionsParam()->bStatusMp3 == MP3_PAUSED)
					{
						if(m_hMp3)
						{
							MCIWndResume(m_hMp3);
							g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_PLAY;
							g_GameMusic.Reset();
							Custom.dwLastMp3PlayerState = Running;
							sprintf_s(strTextTmp,1024,"Mp3 resumed.");
							V3_ChatLogDlg::GetInstance()->AddListLogs("", strTextTmp, CL_ORANGE, true);
						}
					}
				}
			break;
			case WAIT_OBJECT_0+4 ://pause
				if(Custom.gMp3List.size() >0)
				{
					if(g_SaveGame.GetOptionsParam()->bStatusMp3 == MP3_PLAY)
					{
						if(m_hMp3)
						{
							MCIWndPause(m_hMp3);
							g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_PAUSED;
							Custom.dwLastMp3PlayerState = Paused;

							sprintf_s(strTextTmp,1024,"Mp3 paused.");
							V3_ChatLogDlg::GetInstance()->AddListLogs("", strTextTmp, CL_ORANGE, true);
						}
					}
				}
			break;
			case WAIT_OBJECT_0+5 ://SetVolume
				if(m_hMp3)
					MCIWndSetVolume(m_hMp3,g_SaveGame.GetOptionsParam()->iMp3Volume*10);
			break;
			case WAIT_OBJECT_0+6 ://Disable
				if(m_hMp3!=NULL)
				{
					MCIWndStop(m_hMp3);
					MCIWndDestroy(m_hMp3);
					m_hMp3 = NULL;
				}
				Custom.dwLastMp3PlayerState = Stopped;
				g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_DISABLE;
				g_GameMusic.Reset();
				g_GameMusic.LoadNewSound("OIptions");
			break;
			case WAIT_TIMEOUT: 
			{
				if(Custom.gMp3List.size() >0)
				{
					if(m_hMp3)
					{
						if(g_SaveGame.GetOptionsParam()->bStatusMp3 == MP3_PLAY)  //look if is in play mode...
						{ 
							UINT uiLength    = MCIWndGetLength(m_hMp3);
							UINT uiPosC      = MCIWndGetPosition(m_hMp3);
							if(uiPosC >= uiLength)//SOng fini on chnage de chanson
							{
								MCIWndStop(m_hMp3);
								if(PlayMp3NewSong(1,m_hMp3))
								{
									g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_PLAY;
									g_GameMusic.Reset();
									Custom.dwLastMp3PlayerState = Running;
									sprintf_s(strTextTmp,1024,"Play [%s]",Custom.gMp3List[Custom.gdwCurrentSongIndex].strName);
									V3_ChatLogDlg::GetInstance()->AddListLogs("", strTextTmp, CL_ORANGE, true);
								}
								else
								{
									g_SaveGame.GetOptionsParam()->bStatusMp3 = MP3_DISABLE;
									Custom.dwLastMp3PlayerState = Stopped;
								}
							}
						}
					}
				}
			}
			break;
		}
	}

	if(m_hMp3!=NULL)
	{
		MCIWndStop(m_hMp3);
		MCIWndDestroy(m_hMp3);
		m_hMp3 = NULL;
	}
}
