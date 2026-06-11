// Global.h: interface for the Global class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBAL_H__BE9CC3E6_9953_49D5_86F2_D3E2D40E7912__INCLUDED_)
#define AFX_GLOBAL_H__BE9CC3E6_9953_49D5_86F2_D3E2D40E7912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <vector>
#include "INITools.h"

#define NBR_ZONE_MAP 8


#define NMS_TAG_MAX_TAG         7
#define NMS_SMILES_MAX         27
#define NMS_SMILES_MAX_ALPHA  200


#define SYSTEM_COLOR       CL_GREEN_LIGHT2
#define SYSTEM_OUTLINE     RGB(150,150,255)


#define CL_BLACK           RGB(   0,   0,   0 )
#define CL_WHITE           RGB( 255, 255, 255 )
#define CL_GRAY            RGB( 128, 128, 128 )
#define CL_RED				   RGB( 255,   0,   0 )
#define CL_GREEN   		   RGB(   0, 255,   0 )
#define CL_BLUE			   RGB(   0,   0, 255 )
#define CL_YELLOW 		   RGB( 255, 220,   0 )
#define CL_ORANGE			   RGB( 255, 128,   0 )
#define CL_CYAN      	   RGB(   0, 192, 192 )
#define CL_PINK            RGB( 255, 130, 255 )
#define CL_PURPLE          RGB( 150,  25, 200 )
#define CL_MAGENTA         RGB( 255,   0, 255 )
#define CL_GUI_GREEN       RGB(  10,  60,  50 )
#define CL_GUI_GREEN_LIGHT RGB(  90, 175, 165 )

#define CL_BLUE_LIGHT  	   RGB(   0, 100, 255 )
#define CL_BLUE_LIGHT2 	   RGB( 119, 180, 253 )
#define CL_GREEN_LIGHT2 	RGB( 141, 227,   0 )
#define CL_GREEN_LIGHT     RGB(  50, 255,  50 )
#define CL_CYAN_LIGHT  	   RGB(   0, 255, 255 )
#define CL_GRAY_LIGHT      RGB( 192, 192, 192 )
#define CL_GRAY_LIGHT_MID  RGB( 150, 150, 150 )
#define CL_RED_LIGHT  	   RGB( 255,  50,  50 )
#define CL_WHITE_LIGHT     RGB( 190, 180, 140 )
#define CL_YELLOW_LIGHT    RGB( 255, 255, 100 )



#define CL_GREEN_DARK      RGB(  50, 170,  10 )
#define CL_RED_DARK        RGB( 140,  30,  30 )
#define CL_ORANGE_DARK     RGB( 128,  64,   0 )
#define CL_BLUE_DARK       RGB(  32,  70, 128 )
#define CL_YELLOW_DARK     RGB( 127, 127,   0 )

#define CL_DISPLAY_BOX_GOLD  RGB( 197, 170, 107 )

#define CL_DISPLAY_BOX_LINK    RGB( 180,255,0 )
#define CL_DISPLAY_BOX_LINKO   RGB( 59 ,108,9 )
#define CL_DISPLAY_BOX_GREY    RGB( 132,133,127 )
#define CL_DISPLAY_BOX_WHITE   RGB( 230,230,230 )

#define CL_ITEM_GREEN   RGB(0,192,0)
#define CL_ITEM_YELLOW  RGB(225,209,35)
#define CL_ITEM_PURPLE  RGB(159,80,230)
#define CL_ITEM_BLUE    RGB(30,130,255)


//define des cc color par default
#define CC_COL_0     RGB(17,181,205)
#define CC_COL_1     RGB(255,33,75)
#define CC_COL_2     RGB(90,119,216)
#define CC_COL_3     RGB(220,202,6)
#define CC_COL_4     RGB(5,197,37)
#define CC_COL_5     RGB(199,10,204)
#define CC_COL_6     RGB(192,192,192)
#define CC_COL_7     RGB(224,100,10)


const DWORD g_DefColorH  =  RGB(140,120,90);
const DWORD g_DefColorD  =  RGB(180,160,140);
const DWORD g_DefColorDL =  RGB(140,120,100);
const DWORD g_DefColorDS =  RGB(8  ,120,110);


#define PL_FACTION_NONE 						   0x00000001 //   0
#define PL_FACTION_01  							   0x00000002 //   1
#define PL_FACTION_02						      0x00000004 //   2
#define PL_FACTION_03				      	   0x00000008 //   3
#define PL_FACTION_04							   0x00000010 //   4
#define PL_FACTION_05							   0x00000020 //   5
#define PL_FACTION_06					         0x00000040 //   6
#define PL_FACTION_07         				   0x00000080 //   7
#define PL_FACTION_08			      		   0x00000100 //   8
#define PL_FACTION_09					         0x00000200 //   9 
#define PL_FACTION_10						      0x00000400 //   10
#define PL_FACTION_11         				   0x00000800 //   11
#define PL_FACTION_12			      		   0x00001000 //   12
#define PL_FACTION_13	               	   0x00002000 //   13
#define PL_FACTION_14		            		0x00004000 //   14
#define PL_FACTION_15         					0x00008000 //   15
#define PL_FACTION_16	                     0x00010000 //   16
#define PL_FACTION_17	         			   0x00020000 //   17
#define PL_FACTION_18	         			   0x00040000 //   18
#define PL_FACTION_19            				0x00080000 //   19
#define PL_FACTION_20      					   0x00100000 //   20
#define PL_FACTION_21	         				0x00200000 //   21
#define PL_FACTION_22            				0x00400000 //   22
#define PL_FACTION_23	         			   0x00800000 //   23
#define PL_FACTION_24         				   0x01000000 //   24
#define PL_FACTION_25	                     0x02000000 //   25
#define PL_FACTION_26      					   0x04000000 //   26
#define PL_FACTION_27		      			   0x08000000 //   27
#define PL_FACTION_28	   					   0x10000000 //   28
#define PL_FACTION_29	         				0x20000000 //   29
#define PL_FACTION_30							   0x40000000 //   30
#define PL_FACTION_31							   0x80000000 //   31
 

typedef struct _sItemBoust
{
   char chBoustStat;// see below for codes 
   long lMinVal;
   long lMaxVal;
}sItemBoust;

typedef struct _sItemInfo
{
   bool   bSet;
   unsigned char *Name;
   short  shAppearance;
   char   chRadiance;
   short  shAC;
   short  shDodge;
   short  shEnd;
   long   lMinDamage;
   long   lMaxDamage;
   short  shAttack;
   short  shStr;
   short  shAgi;
   short  shWis;
   short  shInt;
   ULONG  ulID;
   bool   bInventory;
   std::vector<sItemBoust> vBoust;
   //short  shNbrBoust; // number of boosts of the object 
   //sItemBoust *pBoust;
}sItemInfo;

typedef struct _ClientConfig
{
   char strSavePath[1024];
   char strPath[512];
   char strAccountIP[50 ];
   char strAccountSvrName[128];
   int  dwAfkStatus;
   char strAfkMessage[2048];
   int  dwNMShowFPS;
   int  FirstTimeAddon;
}ClientConfig;


typedef struct _sLinkAreaWDA
{
   unsigned short dwSrcX;
   unsigned short dwSrcY;
   unsigned short dwSrcW;
   unsigned short dwDesX;
   unsigned short dwDesY;
   unsigned short dwDesW;
}sLinkAreaWDA;

const int g_OptionDlgW       = 608;
const int g_OptionDlgH       = 455;

const int g_ChatDlgW         = 780;
const int g_ChatDlgH         = 450;

const int g_ChestDlgW        = 780;
const int g_ChestDlgH        = 450;

const int g_AskValueDlgW     = 412;
const int g_AskValueDlgH     = 197;

const int g_ComboDlgW        = 176;
const int g_ComboDlgH        = 141;

const int g_SmileDlgW        = 192;
const int g_SmileDlgH        = 105;

const int g_MainBarDlgW      = 700;
const int g_MainBarDlgH      = 85;

const int g_TMIDlgW          = 148;
const int g_TMIDlgH          = 152;
const int g_TMIDlgMAPWH      =  96;
const int g_TMIDlgMAPWHDiv2  = g_TMIDlgMAPWH/2;

const int g_LifeDlgW          = 216;
const int g_LifeDlgH          = 74;

const int g_TargetDlgW        = 212;
const int g_TargetDlgH        = 79;

const int g_GroupOSDlgW       = 136;//105;
const int g_GroupOSDlgH       = 46;//18;

const int g_ChatLogDlgDefW    = 482;
const int g_ChatLogDlgDefH    = 120;

const int g_GuildLogDlgW      = 780;
const int g_GuildLogDlgH      = 450;

const int g_GMDlgW            = 668;
const int g_GMDlgH            = 540;

const int g_Arene1WDlgW       = 316;
const int g_Arene1WDlgH       = 404;

const int g_Arene1PDlgW       = 608;
const int g_Arene1PDlgH       = 404;

const int g_VolDlgW           = 608;
const int g_VolDlgH           = 251;

const int g_AHDlgW            = 780;
const int g_AHDlgH            = 450;

const int g_ChestNewDlgW      = 780;
const int g_ChestNewDlgH      = 490;

const int g_IPVPDlgW          = 780;
const int g_IPVPDlgH          = 490;


const int g_AHVDlgW           = 608;
const int g_AHVDlgH           = 428;

const int g_IFDlgW            = 408;
const int g_IFDlgH            = 428;

const int g_iEffetW           = 44;
const int g_iEffetH           = 42;

const int g_PacDlgW           = 668;
const int g_PacDlgH           = 540;

const int g_MP3DlgW           = 182;
const int g_MP3DlgH           = 38;

const int g_ArenePointDlgW    = 212;
const int g_ArenePointDlgH    = 40;

const int g_DSDlgW            = 608;
const int g_DSDlgH            = 455;

const int g_DSHDlgW           = 668;
const int g_DSHDlgH           = 455;

class Global  
{
public:
	Global();
	virtual ~Global();
   void InitZoneFile();


   std::vector <sLinkAreaWDA>  m_vArealinkVector[NBR_ZONE_MAP];



protected:
   int  m_dwDisplayW;
   int  m_dwDisplayH;
   int  m_dwDisplayWDiv2;
   int  m_dwDisplayHDiv2;

   int  m_dwScreenW;
   int  m_dwScreenH;
   int  m_dwScreenWDiv2;
   int  m_dwScreenHDiv2;

   int  m_dwNbrTileX;
   int  m_dwNbrTileY;
   int  m_dwNbrTileXDiv2;
   int  m_dwNbrTileYDiv2;

   int  m_dwMainTorchW;
   int  m_dwMainTorchH;

   unsigned long m_crBtnEText;
   unsigned long m_crBtnDText;

   int m_dwAlphaValue;

   int m_dwLastScrollStatus;


   bool          m_bCanDisplayChange;
   bool          m_bDisplayChangeZone;
   bool          m_bForceDisplayZone;
   int           m_dwCurMapsZone;
   int           m_dwCurPosZone;
   int           m_dwChangeX;
   int           m_dwChangeY;
   unsigned char *m_pchZoneMaps;

   //int   m_dwLoadZoneXS;
   //int   m_dwLoadZoneYS;
   
   typedef struct _ZoneMapInfo
   {
      //char strMapName[20];
      bool bEnable;
   }ZoneMapInfo;

   typedef struct _ZoneInfo
   {
      char strZoneName[30];
      bool bEnable;
   }ZoneInfo;

   ZoneInfo     m_ZoneInfo[NBR_ZONE_MAP][256];
   ZoneMapInfo  m_ZoneMapInfo[NBR_ZONE_MAP];

   char m_strPath[512];
   char m_ZoneFile[512];
   char m_ZoneFileC[512];

   sItemInfo *m_pItemInfo;

   int m_dwLastWorldX;
   int m_dwLastWorldY;

   int m_iDialoglastXPos;
   int m_iDialoglastYPos;
   int m_iDialogChatlastXPos;
   int m_iDialogChatlastYPos;
   int m_iDialogGMlastXPos;
   int m_iDialogGMlastYPos;
   int m_iDialogArene1WlastXPos;
   int m_iDialogArene1WlastYPos;
   int m_iDialogArene1PlastXPos;
   int m_iDialogArene1PlastYPos;
   int m_iDialogVollastXPos;
   int m_iDialogVollastYPos;
   int m_iDialogAHlastXPos;
   int m_iDialogAHlastYPos;
   int m_iDialogIFlastXPos;
   int m_iDialogIFlastYPos;
   int m_iDialogChestNewlastXPos;
   int m_iDialogChestNewlastYPos;
   int m_iDialogIPVPlastXPos;
   int m_iDialogIPVPlastYPos;


   int m_iDialoglastMainBarXPos;
   int m_iDialoglastMainBarYPos;
   int m_iDialoglastTMIXPos;
   int m_iDialoglastTMIYPos;
   int m_iDialoglastLifeXPos;
   int m_iDialoglastLifeYPos;
   int m_iDialoglastGroupOSXPos;
   int m_iDialoglastGroupOSYPos;
   int m_iDialoglastChatLogXPos;
   int m_iDialoglastChatLogYPos;
   int m_iDialoglastChatLogH;
   int m_iDialoglastChatLogW;
   int m_iDialoglastChatLogPinned;
   int m_iDialoglastTargetXPos;
   int m_iDialoglastTargetYPos;
   int m_iDialoglastMP3XPos;
   int m_iDialoglastMP3YPos;
   int m_iDialoglastArenePointXPos;
   int m_iDialoglastArenePointYPos;
   int m_iDialoglastStatusXPos;
   int m_iDialoglastStatusYPos;
   int m_iDialoglastStatusPinned;
   int m_iDialoglastStatusNbrEffect;
   int m_iDialogChestlastXPos;
   int m_iDialogChestlastYPos;
   int m_iLastDialogOpenned;
   bool m_bDraggingDialog;

public:
   void Lock(void); 
   void Unlock(void);
   void SetDisplaySize(int dwW, int dwH);

   int GetDisplayW(){return m_dwDisplayW;}
   int GetDisplayH(){return m_dwDisplayH;}
   int GetDisplayWDiv2(){return m_dwDisplayWDiv2;}
   int GetDisplayHDiv2(){return m_dwDisplayHDiv2;}

   int GetScreenW(){return m_dwScreenW;}
   int GetScreenH(){return m_dwScreenH;}
   int GetScreenWDiv2(){return m_dwScreenWDiv2;}
   int GetScreenHDiv2(){return m_dwScreenHDiv2;}

   int GetNbrTileX(){return m_dwNbrTileX;}
   int GetNbrTileY(){return m_dwNbrTileY;}
   int GetNbrTileXDiv2(){return m_dwNbrTileXDiv2;}
   int GetNbrTileYDiv2(){return m_dwNbrTileYDiv2;}


   unsigned long GetBtnTCE(){return m_crBtnEText;}
   unsigned long GetBtnTCD(){return m_crBtnDText;}
   int           GetAlphaValue(){return m_dwAlphaValue;}
   void          SetlastScrollStatus(int dwVal){m_dwLastScrollStatus = dwVal;}
   int           GetlastScrollStatus(){return m_dwLastScrollStatus;}

  
   void          LoadZoneMapWorld(int dwWorld, int dwX, int dwY);
   void          ValidMapZonePosition(int dwPosX, int dwPosY);
   void          ForceDisplayZone(int dwPosX, int dwPosY);
   int           GetDisplayZone();
   char*         GetDisplayZoneName();

   unsigned long GetPLFactionMask(unsigned long ulVal);

   sItemInfo *   GetItemInfo(){return m_pItemInfo;};
   void          ResetItemInfo();
   void          SetItemInfoID(ULONG ulID);
   void          SetItemInfoInventory(bool bInventory);
   bool          IsItemInfoAvailable(){return m_pItemInfo->bSet;}


   ClientConfig *ReadClientConfig();
   ClientConfig *GetClientConfig();
   void          WriteClientConfig();

   int           GetDialogLastMainBarXPos();
   int           GetDialogLastMainBarYPos();
   void          SetDialogLastMainBarXYPos(int X, int Y);

   int           GetDialogLastXPos();
   int           GetDialogLastYPos();
   void          SetDialogLastXYPos(int X, int Y);

   int           GetDialogChestLastXPos();
   int           GetDialogChestLastYPos();
   void          SetDialogChestLastXYPos(int X, int Y);

   int           GetDialogChatLastXPos();
   int           GetDialogChatLastYPos();
   void          SetDialogChatLastXYPos(int X, int Y);

   int           GetDialogLastGMXPos();
   int           GetDialogLastGMYPos();
   void          SetDialogLastGMXYPos(int X, int Y);

   int           GetDialogLastArene1WXPos();
   int           GetDialogLastArene1WYPos();
   void          SetDialogLastArene1WXYPos(int X, int Y);

   int           GetDialogLastArene1PXPos();
   int           GetDialogLastArene1PYPos();
   void          SetDialogLastArene1PXYPos(int X, int Y);

   int           GetDialogLastVolXPos();
   int           GetDialogLastVolYPos();
   void          SetDialogLastVolXYPos(int X, int Y);

   int           GetDialogLastAHXPos();
   int           GetDialogLastAHYPos();
   void          SetDialogLastAHXYPos(int X, int Y);

   int           GetDialogLastChestNewXPos();
   int           GetDialogLastChestNewYPos();
   void          SetDialogLastChestNewXYPos(int X, int Y);

   int           GetDialogLastIPVPXPos();
   int           GetDialogLastIPVPYPos();
   void          SetDialogLastIPVPXYPos(int X, int Y);

   int           GetDialogLastIFXPos();
   int           GetDialogLastIFYPos();
   void          SetDialogLastIFXYPos(int X, int Y);

   int           GetDialogLastTMIXPos();
   int           GetDialogLastTMIYPos();
   void          SetDialogLastTMIXYPos(int X, int Y);

   int           GetDialogLastLifeXPos();
   int           GetDialogLastLifeYPos();
   void          SetDialogLastLifeXYPos(int X, int Y);

   int           GetDialogLastStatusXPos();
   int           GetDialogLastStatusYPos();
   void          SetDialogLastStatusXYPos(int X, int Y);
   int           GetDialogLastStatusP();
   void          SetDialogLastStatusP(int P);
   int           GetDialogLastStatusNbrEffect();
   void          SetDialogLastStatusNbrEffect(int iNbr);

   int           GetDialogLastTargetXPos();
   int           GetDialogLastTargetYPos();
   void          SetDialogLastTargetXYPos(int X, int Y);

   int           GetDialogGroupOSXPos();
   int           GetDialogGroupOSYPos();
   void          SetDialogGroupOSXYPos(int X, int Y);

   int           GetDialogLastMP3XPos();
   int           GetDialogLastMP3YPos();
   void          SetDialogLastMP3XYPos(int X, int Y);

   int           GetDialogLastArenePointXPos();
   int           GetDialogLastArenePointYPos();
   void          SetDialogLastArenePointXYPos(int X, int Y);


   int           GetDialogLastChatLogXPos();
   int           GetDialogLastChatLogYPos();
   int           GetDialogLastChatLogH();
   int           GetDialogLastChatLogW();
   int           GetDialogLastChatLogP();
   void          SetDialogLastChatLogXYPos(int X, int Y, int W,int H);
   void          SetDialogLastChatLogP(int P);

   void          SetLastDialogOpen(int iVal);
   int           GetLastDialogOpen();

   void          SetDraggingDialog(bool bVal);
   bool          GetDraggingDialog();


   void          ResetAllGUIPositionAndSize();


   void          SetMouseLastWorld(int dwX, int dwY)
   {
      m_dwLastWorldX = dwX;
      m_dwLastWorldY = dwY;
   }
   int           GetLastMouseWorldX(){return m_dwLastWorldX;}
   int           GetLastMouseWorldY(){return m_dwLastWorldY;}
   
   int SaveCurrentImage8(BYTE *pshImage,BYTE *pPal,int dwWidth, int dwHeight,char *pstrName,BOOL bFlipV);


   void SaveGUI();
   void LoadGUI();

   HRESULT LoadImageDataToBmpFile2(LPCTSTR lpszFileName,BYTE **pData,BYTE **pPal,int *pdwWidth,int *pdwHeight, int *pdwImageBitsPixel);

protected:
   int LoadImageDataToBmpFile(char * lpszFileName,unsigned char *pData);
   
   
   ClientConfig m_ClientConfig;
   BOOL m_bConfigRead;

   CINITools  m_iniT4C;


private:
   CRITICAL_SECTION csInternal;

   static UINT  WINAPI SaveGUIThread( void *nil );


};

#endif // !defined(AFX_GLOBAL_H__BE9CC3E6_9953_49D5_86F2_D3E2D40E7912__INCLUDED_)
