// SaveGame.h: interface for the CSaveGame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVEGAME_H__BE9CC3E6_9953_49D5_86F2_D3E2D40E7912__INCLUDED_)
#define AFX_SAVEGAME_H__BE9CC3E6_9953_49D5_86F2_D3E2D40E7912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef  _WIN32_WINNT
#define  _WIN32_WINNT 0x0600 // for SignalObjectAndWait
#endif
#include <windows.h>
#include <direct.h>
#include <vector>

#include "INITools.h"


#define TR_MAP_Y 192
#define TR_MAP_X 192

#define TR_MAP_YDIV (3072/TR_MAP_Y)
#define TR_MAP_XDIV (3072/TR_MAP_X)


const int g_iNbrMacroX = 12;
const int g_iNbrMacroY = 9;
const int g_iNbrMacro  = g_iNbrMacroX*g_iNbrMacroY;

const unsigned short SAVEGAME_VERSION  = 0x0100;   //2 bytes


#define MP3_DISABLE 0
#define MP3_PLAY    0xAA
#define MP3_PAUSED  0xBB




typedef struct _sInventory
{
   int dwID;
   int dwType;
   int dwX;
   int dwY;
}sInventory;

typedef struct _sMacro
{
   int  dwIcon;
   int  dwID;
   int  dwKey;
   char chFastPos[g_iNbrMacroX*g_iNbrMacroY];
   char strMacroName[256]; 
}sMacro;

typedef struct _sChannel
{
   char   strNameCC[256];
   char   strPasswordCC[256];
   DWORD  dwColor;
   DWORD  chEnable;
}sChannel;

typedef struct _sIgnoreList
{
   char strNameIgnore[256];
}sIgnoreList;

typedef struct _sOptionParam
{
   // General Config
   int  bPageEnable;  //1
   // Option Sound
   int  dwMusicVol;   //18/20
   int  dwSoundVol;   //18/20
   int  dwAmbiantVol;   //18/20
   int  bUseCD;       //0
   // Option Graphic
   int  bLightLowSize;//1
   int  bMeteoEffect; //1
   int  bDitherGraph; //1
   int  bSeraphAnim;  //1
   int  bDisplayStatus;//1
   int  bDisplayRadar; //1
   int  bDisplayChatLog;//1
   int  bDisplayGroupInfo;//1
   int  bLockTarget;//0
   short  bMacroOnClick;//1
   short  bGUIDynamic;//1

   //option Log
   int  blogNPC;//1
   int  blogNPCPerso;//1
   int  bLogUser;//1
   int  bLogEnable;//0
   char LogName[256];

   //new option
   int  bLockResize;//1
   int  bShowItemSpec;//1
   int  bShowNewLife;//1
   int  bShowNewOmbrage;//0
   int  bShowAnimDecorsLight;//1
   int  bStatusMp3;// MP3_DISABLE;
   int  bMp3Random;//0
   int  bDisplayMp3;//0
   int  bDisplayDamage;//1
   int  bDisplayHealing;//1
   int  dwBrightnessNew;//6
   int  iMp3Volume;//100
   int  bChatLogFonce;//1
   int  bDisplayTarget;//1
   int  bDisplayYouTMI;//0
   int  bPlayAt32FPS;//1
   int  bNotDisplayMacro;//0
   int  bDisplayRestant;//1
   int  bAncienneMusique;//0
   int  bChestListe;//0
   int  bAncienDisplayOH;//0
}OptionParam;

 

class CSaveGame  
{
public:
	CSaveGame();
	virtual ~CSaveGame();
   BOOL bSetFileName(char *pstrAccountName,char *pstrPlayerName);
   BOOL Load_SaveGame(char *pstrAccountName,char *pstrPlayerName);
   BOOL Save_SaveGame();
   void SuspendSaveThread(BOOL bSuspend);

   
   BOOL bFreeAll();

  
   BOOL AddInv       (sInventory  pInventory ); //OK
   BOOL AddChest     (sInventory  pChest );     //OK
   BOOL AddChestG    (sInventory  pChest );     //OK
   BOOL AddMacroItem (sMacro      pMacroItem ); //OK
   BOOL AddMacroSpell(sMacro      pMacroSpell); //OK
   BOOL AddMacroSkill(sMacro      pMacroSkill); //OK


   BOOL AddChannelS  (sChannel    pChannel   );
   BOOL AddIgnore    (sIgnoreList pIgnore    );
   

   void RemoveChannel(char *pStrName);

   int GetNbrInv    (){return m_pInventoryL.size();}        //OK
   int GetNbrChest  (){return m_pChestL.size();}            //OK
   int GetNbrChestG (){return m_pChestLG.size();}           //OK
   int GetNbrMItem  (){return m_pMacroItemL.size();}        //OK
   int GetNbrMSpell (){return m_pMacroSpellL.size();}       //OK
   int GetNbrMSkill (){return m_pMacroSkillL.size();}       //OK
   int GetNbrChannel(){return m_pChannelL.size();}          //OK
   int GetNbrIgnore (){return m_pIgnoreL.size();}           //OK
   
   sInventory  *GetInv    (UINT dwIndex);  //OK
   sInventory  *GetChest  (UINT dwIndex);  //OK
   sInventory  *GetChestG (UINT dwIndex);  //OK
   sMacro      *GetMItem  (UINT dwIndex);  //OK
   sMacro      *GetMSpell (UINT dwIndex);  //OK
   sMacro      *GetMSkill (UINT dwIndex);  //OK
   sChannel    *GetChannel(UINT dwIndex);  //OK
   sIgnoreList *GetIgnore (UINT dwIndex);  //OK
   
   
   
   OptionParam* GetOptionsParam(){return &m_pOptionParams;}
 
   
   
  
  

   void SetRTMapVal(USHORT ushX,USHORT ushY,USHORT ushW );
   BOOL GetRTMapVal(USHORT ushX,USHORT ushY,USHORT ushW , int *pdwX,int *pdwY);
   BOOL GetRTMapValCase(USHORT ushX,USHORT ushY,USHORT ushW );

protected:
   BOOL m_bIsLoaded;
   char  m_strSavFileNameI[MAX_PATH];
   char  m_strSavFileNameM[MAX_PATH];
   char  m_strSavFileNameC[MAX_PATH];
   char  m_strSavFileNameRT[MAX_PATH];
   char  m_strSavFileNameINI[MAX_PATH];

   unsigned char m_uchRTMap[10][TR_MAP_Y][TR_MAP_X];

  
   std::vector <sInventory>  m_pInventoryL;
   std::vector <sMacro>      m_pMacroItemL;
   std::vector <sMacro>      m_pMacroSpellL;
   std::vector <sMacro>      m_pMacroSkillL;
   std::vector <sChannel>    m_pChannelL;
   std::vector <sIgnoreList> m_pIgnoreL;
   std::vector <sInventory>  m_pChestL;
   std::vector <sInventory>  m_pChestLG;

   OptionParam  m_pOptionParams;

   CINITools  m_iniOptions;

  


public:
   
protected:
   BOOL       m_bNMSDisplay;
   BOOL       m_bSaveSuspended;
   BOOL       m_bSaveThread;
   HANDLE     m_hSaveThread;
   HANDLE     m_hSaveThreadExit;

   CRITICAL_SECTION m_crLockSave;

private:
    static void  SaveGameThread( LPVOID lpParam );


};

#endif // !defined(AFX_SAVEGAME_H__BE9CC3E6_9953_49D5_86F2_D3E2D40E7912__INCLUDED_)

