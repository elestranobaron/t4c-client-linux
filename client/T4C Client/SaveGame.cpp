// SaveGame.cpp: implementation of the CSaveGame class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning( disable:4786 )
#include "../pch.h"
#include "SaveGame.h"
#include "App.h"
#include "Zlib/zlib.h"
#include <memory.h>
#include <stdio.h>
#include <shlobj.h>
#include "NewInterface/RootBoxUI.h"

#include "NewInterface/V3_InvDlg.h"
#include "NewInterface/V3_ChestDlg.h"
#include "NewInterface/V3_GuildChestDlg.h"
#include "NewInterface/V3_ChatDlg.h"

#define  MEM_COMPRESS_NEED(ActualLen) (ULONG)(ActualLen *1.1 + 12);
#define  V2IMAGE_GMNMS_DATA       "\\Game Files\\gmnmb.bin"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSaveGame g_SaveGame;

CSaveGame::CSaveGame()
{
   m_bNMSDisplay = FALSE;
   char strPathFile[1024];
   ClientConfig *pConfig = g_Global.GetClientConfig();
   sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGE_GMNMS_DATA);
   FILE *pfGMD = NULL;
   fopen_s(&pfGMD,strPathFile,"rb");
   if(pfGMD)
   {
      fclose(pfGMD);
      m_bNMSDisplay = TRUE;
   }

   m_bIsLoaded         = FALSE;

   m_bSaveThread       = FALSE;
   m_hSaveThread       = NULL;
   m_hSaveThreadExit   = NULL;

   m_pOptionParams.bPageEnable             = 1;
   
   // Option Sound
   m_pOptionParams.dwMusicVol              = 18;
   m_pOptionParams.dwSoundVol              = 18;
   m_pOptionParams.dwAmbiantVol            = 18;
   m_pOptionParams.bUseCD                  = 0;
   m_pOptionParams.bStatusMp3              = MP3_DISABLE;
   m_pOptionParams.bMp3Random              = 0;
   m_pOptionParams.bDisplayMp3             = 0;
   m_pOptionParams.iMp3Volume              = 100;
   
   // Option Graphic
   
   m_pOptionParams.bLightLowSize           = 1;
   m_pOptionParams.bMeteoEffect            = 1;
   m_pOptionParams.bShowAnimDecorsLight    = 1;


   m_pOptionParams.bDitherGraph            = 1;
   m_pOptionParams.bSeraphAnim             = 1;
   m_pOptionParams.bDisplayStatus          = 1;
   m_pOptionParams.bDisplayChatLog         = 1;
   m_pOptionParams.bDisplayGroupInfo       = 1;
   m_pOptionParams.bDisplayRadar           = 1;
   m_pOptionParams.bMacroOnClick           = 1;
   m_pOptionParams.bLockTarget             = 0;
   m_pOptionParams.bLockResize             = 1;
   m_pOptionParams.bShowItemSpec           = 1;
   m_pOptionParams.bShowNewLife            = 1;
   m_pOptionParams.bDisplayDamage          = 1;
   m_pOptionParams.bDisplayHealing         = 1;
   m_pOptionParams.bChatLogFonce           = 1;
   m_pOptionParams.bDisplayTarget          = 1;
   m_pOptionParams.bDisplayYouTMI          = 0;
   m_pOptionParams.bPlayAt32FPS            = 1;
   m_pOptionParams.bShowNewOmbrage         = 0;
   m_pOptionParams.bGUIDynamic             = 1;
   m_pOptionParams.bNotDisplayMacro        = 0;
   m_pOptionParams.bDisplayRestant         = 1;
   m_pOptionParams.bAncienneMusique        = 0;
   m_pOptionParams.bChestListe             = 0;
   m_pOptionParams.dwBrightnessNew         = 6;
   if(m_bNMSDisplay)
      m_pOptionParams.bAncienDisplayOH        = 0;
   else 
      m_pOptionParams.bAncienDisplayOH        = 1;

   //option Log
   m_pOptionParams.blogNPC      = 1;
   m_pOptionParams.blogNPCPerso = 1;
   m_pOptionParams.bLogUser     = 1;
   m_pOptionParams.bLogEnable   = 0;

   //RTMap
   for(int w=0;w<10;w++)
   {
      for(int y=0;y<TR_MAP_Y;y++)
      {
         for(int x=0;x<TR_MAP_X;x++)
         {
            m_uchRTMap[w][y][x] = 0x00;
         }
      }
   }

   ::InitializeCriticalSection(&m_crLockSave);

   SYSTEMTIME sysTime; 
	GetLocalTime(&sysTime);
   sprintf_s(m_pOptionParams.LogName,48,"%02d%02d%02d_log.txt",sysTime.wMonth,sysTime.wDay,sysTime.wYear-2000);

   if(!m_hSaveThread)
   {
      m_bSaveSuspended  = TRUE; //on demarre le save suspendu... 
      m_bSaveThread     = TRUE;
      m_hSaveThreadExit = CreateEvent(NULL,FALSE, FALSE, NULL);
      m_hSaveThread     = (HANDLE)_beginthread( SaveGameThread  , 0, this );   
      
   }

   
}

CSaveGame::~CSaveGame()
{
   if(m_hSaveThreadExit && m_hSaveThread)
   {
      ::SetEvent(m_hSaveThreadExit);
      if( ::WaitForSingleObject( m_hSaveThread, 1000 ) == WAIT_TIMEOUT ) 
         ::TerminateThread( m_hSaveThread, 0x69 );
      ::CloseHandle(m_hSaveThreadExit);
      m_hSaveThreadExit   = NULL;
      m_hSaveThread = NULL;
   }

   ::DeleteCriticalSection(&m_crLockSave);
}

BOOL CSaveGame::bSetFileName(char *pstrAccountName,char *pstrPlayerName)
{
   ClientConfig *pConfig = g_Global.GetClientConfig();

   ::EnterCriticalSection(&m_crLockSave);
   // create directory savegame if not exist...
   char strSaveGameDir[MAX_PATH];
   if(Player.PortCom == 11677)
	   sprintf_s(strSaveGameDir,MAX_PATH,"%s\\SaveGame\\%s_%s_%s\\",pConfig->strSavePath,pConfig->strAccountIP,pstrAccountName,pstrPlayerName);
   else
	   sprintf_s(strSaveGameDir,MAX_PATH,"%s\\SaveGame\\%s.%d_%s_%s\\",pConfig->strSavePath,pConfig->strAccountIP,Player.PortCom,pstrAccountName,pstrPlayerName);

   
   CreateDirectory( strSaveGameDir, NULL );//Create folder to ma sure he exist...

   sprintf_s(m_strSavFileNameI,512,"%sGrids.bin",strSaveGameDir);
   sprintf_s(m_strSavFileNameM,512,"%sMacros.bin",strSaveGameDir);
   sprintf_s(m_strSavFileNameC,512,"%sCCI.bin",strSaveGameDir);
   sprintf_s(m_strSavFileNameRT,512,"%sRTMap.Bin",strSaveGameDir);
   sprintf_s(m_strSavFileNameINI,512,"%sOptions.ini",strSaveGameDir);

   m_iniOptions.SetIniFileName(m_strSavFileNameINI);
   ::LeaveCriticalSection(&m_crLockSave);
   return TRUE;
}


BOOL CSaveGame::Load_SaveGame(char *pstrAccountName,char *pstrPlayerName)
{
   OutputDebugString("Load_SaveGame***\n");  
  

   ClientConfig *pConfig = g_Global.GetClientConfig();

   // create directory savegame if not exist...
   char strSaveGameDir[MAX_PATH];

   if(Player.PortCom == 11677)
	  sprintf_s(strSaveGameDir,MAX_PATH,"%s\\SaveGame\\%s_%s_%s\\",pConfig->strSavePath,pConfig->strAccountIP,pstrAccountName,pstrPlayerName);
   else
	  sprintf_s(strSaveGameDir,MAX_PATH,"%s\\SaveGame\\%s.%d_%s_%s\\",pConfig->strSavePath,pConfig->strAccountIP,Player.PortCom,pstrAccountName,pstrPlayerName);

   CreateDirectory( strSaveGameDir, NULL );//Create folder to ma sure he exist...

   sprintf_s(m_strSavFileNameI,512,"%sGrids.bin",strSaveGameDir);
   sprintf_s(m_strSavFileNameM,512,"%sMacros.bin",strSaveGameDir);
   sprintf_s(m_strSavFileNameC,512,"%sCCI.bin",strSaveGameDir);
   sprintf_s(m_strSavFileNameRT,512,"%sRTMap.Bin",strSaveGameDir);
   sprintf_s(m_strSavFileNameINI,512,"%sOptions.ini",strSaveGameDir);

   m_iniOptions.SetIniFileName(m_strSavFileNameINI);
   

   
   bFreeAll();

   FILE *pfI = NULL;
   fopen_s(&pfI,m_strSavFileNameI,"rb");
   if(pfI)
   {
      unsigned short ushVersion;
      fread(&ushVersion,2,1,pfI);
      if(ushVersion == SAVEGAME_VERSION)
      {
         int i=0;
         int dwNbrInv;
         int dwNbrChest;
         int dwNbrChestG;
         sInventory  pInvItem;
         fread(&dwNbrInv,1,4,pfI);
         fread(&dwNbrChest,1,4,pfI);
         fread(&dwNbrChestG,1,4,pfI);

         //Inventory
         for(i=0;i<dwNbrInv;i++)
         {
            fread(&pInvItem,sizeof(sInventory),1,pfI);
            AddInv(pInvItem);
         }
         //chest
         for(i=0;i<dwNbrChest;i++)
         {
            fread(&pInvItem,sizeof(sInventory),1,pfI);
            AddChest(pInvItem);
         }
         //chest guild     
         for(i=0;i<dwNbrChestG;i++)
         {
            fread(&pInvItem,sizeof(sInventory),1,pfI);
            AddChestG(pInvItem);
         }
      }
      else
      {
         m_pInventoryL .clear();
         m_pChestL     .clear();
         m_pChestLG    .clear();
         g_Var.InvalideSaveGameI = true;
      }
      fclose(pfI);
   }


   FILE *pfM = NULL;
   fopen_s(&pfM,m_strSavFileNameM,"rb");
   if(pfM)
   {
      unsigned short ushVersion;
      fread(&ushVersion,2,1,pfM);
      if(ushVersion == SAVEGAME_VERSION)
      {
         int i=0;
         int dwNbrMacroItem;
         int dwNbrMacroSpell;
         int dwNbrMacroSkill;
         
         sMacro  pMacro;
         fread(&dwNbrMacroItem,1,4,pfM);
         fread(&dwNbrMacroSpell,1,4,pfM);
         fread(&dwNbrMacroSkill,1,4,pfM);

         for(i=0;i<dwNbrMacroItem;i++)
         {
            fread(&pMacro,sizeof(sMacro),1,pfM);
            AddMacroItem(pMacro);
         }
         for(i=0;i<dwNbrMacroSpell;i++)
         {
            fread(&pMacro,sizeof(sMacro),1,pfM);
            AddMacroSpell(pMacro);
         }
         for(i=0;i<dwNbrMacroSkill;i++)
         {
            fread(&pMacro,sizeof(sMacro),1,pfM);
            AddMacroSkill(pMacro);
         }
      }
      else
      {
         m_pMacroItemL .clear();
         m_pMacroSpellL.clear();
         m_pMacroSkillL.clear();
         g_Var.InvalideSaveGameM = true;
      }
      fclose(pfM);
   }

   FILE *pfCC = NULL;
   fopen_s(&pfCC,m_strSavFileNameC,"rb");
   if(pfCC)
   {
      unsigned short ushVersion;
      fread(&ushVersion,2,1,pfCC);
      if(ushVersion == SAVEGAME_VERSION)
      {
         int i=0;
         int dwNbrChannel; 
         int dwNbrIgnore;
         sChannel    pChannel;
         sIgnoreList pIgnore;

         fread(&dwNbrChannel,1,4,pfCC);
         fread(&dwNbrIgnore,1,4,pfCC);

         for(i=0;i<dwNbrChannel;i++)
         {
            fread(&pChannel,sizeof(sChannel),1,pfCC);
            AddChannelS(pChannel);
         }
        
         for(i=0;i<dwNbrIgnore;i++)
         {
            fread(&pIgnore,sizeof(sIgnoreList),1,pfCC);
            AddIgnore(pIgnore);
         }
      }
      else
      {
         m_pChannelL   .clear();
         m_pIgnoreL    .clear();
         g_Var.InvalideSaveGameC = true;
      }

      fclose(pfCC);
   }

   FILE *pfRT = NULL;
   fopen_s(&pfRT,m_strSavFileNameRT,"rb");
   if(pfRT)
   {
      unsigned short ushVersion;
      fread(&ushVersion,2,1,pfRT);
      if(ushVersion == SAVEGAME_VERSION)
      {
         // read les info des RT MAp...
         int dwRSize = fread(&m_uchRTMap,1,10*TR_MAP_Y*TR_MAP_X,pfRT);
         if(dwRSize != 10*TR_MAP_Y*TR_MAP_X)
         {
            for(int w=0;w<10;w++)
            {
               for(int y=0;y<TR_MAP_Y;y++)
               {
                  for(int x=0;x<TR_MAP_X;x++)
                  {
                     m_uchRTMap[w][y][x] = 0x00;
                  }
               }
            }
         }
      }
      else
      {
         for(int w=0;w<10;w++)
         {
            for(int y=0;y<TR_MAP_Y;y++)
            {
               for(int x=0;x<TR_MAP_X;x++)
               {
                  m_uchRTMap[w][y][x] = 0x00;
               }
            }
         }
      }

      fclose(pfRT);
   }


   

   if(m_iniOptions.CheckSection("SETTINGS"))
   {
      m_pOptionParams.bPageEnable         = m_iniOptions.GetINIValueInt("SETTINGS","PAGEENABLE"        ,1);
      m_pOptionParams.dwMusicVol          = m_iniOptions.GetINIValueInt("SETTINGS","MUSICVOLUME"       ,18);
      m_pOptionParams.dwSoundVol          = m_iniOptions.GetINIValueInt("SETTINGS","SOUNDVOLUME"       ,18);
      m_pOptionParams.dwAmbiantVol        = m_iniOptions.GetINIValueInt("SETTINGS","AMBIANTFXVOLUME"   ,18);
      m_pOptionParams.bUseCD              = m_iniOptions.GetINIValueInt("SETTINGS","USECD"             ,0);
      m_pOptionParams.iMp3Volume          = m_iniOptions.GetINIValueInt("SETTINGS","MP3VOLUME"         ,100);
      m_pOptionParams.bStatusMp3          = m_iniOptions.GetINIValueInt("SETTINGS","MP3STATUS"         ,MP3_DISABLE);
      m_pOptionParams.bMp3Random          = m_iniOptions.GetINIValueInt("SETTINGS","MP3RANDOM"         ,0);
      m_pOptionParams.bLightLowSize       = m_iniOptions.GetINIValueInt("SETTINGS","LOWLIGHTSIZE"      ,1);
      m_pOptionParams.bMeteoEffect        = m_iniOptions.GetINIValueInt("SETTINGS","METEOEFFECT"       ,1);
      m_pOptionParams.bDitherGraph        = m_iniOptions.GetINIValueInt("SETTINGS","DIETERGRAPH"       ,1);
      m_pOptionParams.bSeraphAnim         = m_iniOptions.GetINIValueInt("SETTINGS","SHOWANIMSERAPH"    ,1);
      m_pOptionParams.bDisplayStatus      = m_iniOptions.GetINIValueInt("SETTINGS","DISPLAYSPELLSTATUS",1);
      m_pOptionParams.bDisplayRadar       = m_iniOptions.GetINIValueInt("SETTINGS","DISPLAYRADAR"      ,1);
      m_pOptionParams.bDisplayChatLog     = m_iniOptions.GetINIValueInt("SETTINGS","DISPLAYCHATLOG"    ,1);
      m_pOptionParams.bDisplayGroupInfo   = m_iniOptions.GetINIValueInt("SETTINGS","DISPLAYGROUPINFO"  ,1);
      m_pOptionParams.bLockTarget         = m_iniOptions.GetINIValueInt("SETTINGS","USECLOCKTARGET"    ,0);
      m_pOptionParams.bMacroOnClick       = m_iniOptions.GetINIValueInt("SETTINGS","MACROONCLICK"      ,1);
      m_pOptionParams.bGUIDynamic         = m_iniOptions.GetINIValueInt("SETTINGS","GUIDYNAMIC"        ,1);
      m_pOptionParams.bLockResize         = m_iniOptions.GetINIValueInt("SETTINGS","LOCKRESIZE"        ,1);
      m_pOptionParams.bShowItemSpec       = m_iniOptions.GetINIValueInt("SETTINGS","SHOWITEMINFO"      ,1);
      m_pOptionParams.bShowNewLife        = m_iniOptions.GetINIValueInt("SETTINGS","SHOWNEWLIFE"       ,1);
      m_pOptionParams.bShowNewOmbrage     = m_iniOptions.GetINIValueInt("SETTINGS","SHOWNEWOMBRAGE"    ,0);
      m_pOptionParams.bShowAnimDecorsLight= m_iniOptions.GetINIValueInt("SETTINGS","SHOWANIMATEDDECORS",1);
      m_pOptionParams.bDisplayMp3         = m_iniOptions.GetINIValueInt("SETTINGS","SHOWMP3"           ,0);
      m_pOptionParams.bDisplayDamage      = m_iniOptions.GetINIValueInt("SETTINGS","DISPLAYDAMAGE"     ,1);
      m_pOptionParams.bDisplayHealing     = m_iniOptions.GetINIValueInt("SETTINGS","DISPLAYHEALING"    ,1);
      m_pOptionParams.dwBrightnessNew     = m_iniOptions.GetINIValueInt("SETTINGS","BRIGHTNESS"        ,6);
      m_pOptionParams.bChatLogFonce       = m_iniOptions.GetINIValueInt("SETTINGS","CHATBACKFONCE"     ,1);
      m_pOptionParams.bDisplayTarget      = m_iniOptions.GetINIValueInt("SETTINGS","DISPLAYTARGET"     ,1);
      m_pOptionParams.bDisplayYouTMI      = m_iniOptions.GetINIValueInt("SETTINGS","DISPLAYYOUTMI"     ,0);
      m_pOptionParams.bPlayAt32FPS        = m_iniOptions.GetINIValueInt("SETTINGS","RUN323FPS"         ,1);
      m_pOptionParams.bNotDisplayMacro    = m_iniOptions.GetINIValueInt("SETTINGS","HIDEMACRO"         ,0);
      m_pOptionParams.bDisplayRestant     = m_iniOptions.GetINIValueInt("SETTINGS","DISPLAYRESTANT"    ,1);
      m_pOptionParams.bAncienneMusique    = m_iniOptions.GetINIValueInt("SETTINGS","ANCIENNEMUSIC"     ,0);
      m_pOptionParams.blogNPC             = m_iniOptions.GetINIValueInt("SETTINGS","LOGNPC"            ,1);
      m_pOptionParams.blogNPCPerso        = m_iniOptions.GetINIValueInt("SETTINGS","LOGPERSONPC"       ,1);
      m_pOptionParams.bLogUser            = m_iniOptions.GetINIValueInt("SETTINGS","LOGUSER"           ,1);
      m_pOptionParams.bLogEnable          = m_iniOptions.GetINIValueInt("SETTINGS","LOGENABLE"         ,0);
      m_pOptionParams.bChestListe         = m_iniOptions.GetINIValueInt("SETTINGS","CHESTLISTE"        ,0);
      m_pOptionParams.bAncienDisplayOH    = m_iniOptions.GetINIValueInt("SETTINGS","OLDDISPLAYTEXT"    ,0);
      if(!m_bNMSDisplay)
         m_pOptionParams.bAncienDisplayOH = 1;
      sprintf_s(m_pOptionParams.LogName,256,"%s",m_iniOptions.GetINIValue("SETTINGS","LOGNAME",""));
   }
   else
   {
      m_iniOptions.AddSection("SETTINGS");
      m_iniOptions.AddKey("SETTINGS","PAGEENABLE");
      m_iniOptions.AddKey("SETTINGS","MUSICVOLUME");
      m_iniOptions.AddKey("SETTINGS","SOUNDVOLUME");
      m_iniOptions.AddKey("SETTINGS","AMBIANTFXVOLUME");
      m_iniOptions.AddKey("SETTINGS","USECD");
      m_iniOptions.AddKey("SETTINGS","MP3VOLUME");
      m_iniOptions.AddKey("SETTINGS","MP3STATUS");
      m_iniOptions.AddKey("SETTINGS","MP3RANDOM");
      m_iniOptions.AddKey("SETTINGS","LOWLIGHTSIZE");
      m_iniOptions.AddKey("SETTINGS","METEOEFFECT");
      m_iniOptions.AddKey("SETTINGS","DIETERGRAPH");
      m_iniOptions.AddKey("SETTINGS","SHOWANIMSERAPH");
      m_iniOptions.AddKey("SETTINGS","DISPLAYSPELLSTATUS");
      m_iniOptions.AddKey("SETTINGS","DISPLAYRADAR");
      m_iniOptions.AddKey("SETTINGS","DISPLAYCHATLOG");
      m_iniOptions.AddKey("SETTINGS","DISPLAYGROUPINFO");
      m_iniOptions.AddKey("SETTINGS","USECLOCKTARGET");
      m_iniOptions.AddKey("SETTINGS","MACROONCLICK");
      m_iniOptions.AddKey("SETTINGS","GUIDYNAMIC");
      m_iniOptions.AddKey("SETTINGS","LOCKRESIZE");
      m_iniOptions.AddKey("SETTINGS","SHOWITEMINFO");
      m_iniOptions.AddKey("SETTINGS","SHOWNEWLIFE");
      m_iniOptions.AddKey("SETTINGS","SHOWNEWOMBRAGE");
      m_iniOptions.AddKey("SETTINGS","SHOWANIMATEDDECORS");
      m_iniOptions.AddKey("SETTINGS","SHOWMP3");
      m_iniOptions.AddKey("SETTINGS","DISPLAYDAMAGE");
      m_iniOptions.AddKey("SETTINGS","DISPLAYHEALING");
      m_iniOptions.AddKey("SETTINGS","BRIGHTNESS");
      m_iniOptions.AddKey("SETTINGS","CHATBACKFONCE");
      m_iniOptions.AddKey("SETTINGS","DISPLAYTARGET");
      m_iniOptions.AddKey("SETTINGS","DISPLAYYOUTMI");
      m_iniOptions.AddKey("SETTINGS","RUN323FPS");
      m_iniOptions.AddKey("SETTINGS","HIDEMACRO");
      m_iniOptions.AddKey("SETTINGS","DISPLAYRESTANT");
      m_iniOptions.AddKey("SETTINGS","ANCIENNEMUSIC");
      m_iniOptions.AddKey("SETTINGS","LOGNPC");
      m_iniOptions.AddKey("SETTINGS","LOGUSER");
      m_iniOptions.AddKey("SETTINGS","LOGENABLE");
      m_iniOptions.AddKey("SETTINGS","LOGNAME");
      m_iniOptions.AddKey("SETTINGS","CHESTLISTE");
      

      m_iniOptions.WriteINIValueInt("SETTINGS","PAGEENABLE"        ,m_pOptionParams.bPageEnable);
      m_iniOptions.WriteINIValueInt("SETTINGS","MUSICVOLUME"       ,m_pOptionParams.dwMusicVol);
      m_iniOptions.WriteINIValueInt("SETTINGS","SOUNDVOLUME"       ,m_pOptionParams.dwSoundVol);
      m_iniOptions.WriteINIValueInt("SETTINGS","AMBIANTFXVOLUME"   ,m_pOptionParams.dwAmbiantVol);
      m_iniOptions.WriteINIValueInt("SETTINGS","USECD"             ,m_pOptionParams.bUseCD);
      m_iniOptions.WriteINIValueInt("SETTINGS","MP3VOLUME"         ,m_pOptionParams.iMp3Volume);
      m_iniOptions.WriteINIValueInt("SETTINGS","MP3STATUS"         ,m_pOptionParams.bStatusMp3);
      m_iniOptions.WriteINIValueInt("SETTINGS","MP3RANDOM"         ,m_pOptionParams.bMp3Random);
      m_iniOptions.WriteINIValueInt("SETTINGS","LOWLIGHTSIZE"      ,m_pOptionParams.bLightLowSize);
      m_iniOptions.WriteINIValueInt("SETTINGS","METEOEFFECT"       ,m_pOptionParams.bMeteoEffect);
      m_iniOptions.WriteINIValueInt("SETTINGS","DIETERGRAPH"       ,m_pOptionParams.bDitherGraph);
      m_iniOptions.WriteINIValueInt("SETTINGS","SHOWANIMSERAPH"    ,m_pOptionParams.bSeraphAnim);
      m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYSPELLSTATUS",m_pOptionParams.bDisplayStatus);
      m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYRADAR"      ,m_pOptionParams.bDisplayRadar);
      m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYCHATLOG"    ,m_pOptionParams.bDisplayChatLog);
      m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYGROUPINFO"  ,m_pOptionParams.bDisplayGroupInfo);
      m_iniOptions.WriteINIValueInt("SETTINGS","USECLOCKTARGET"    ,m_pOptionParams.bLockTarget);
      m_iniOptions.WriteINIValueInt("SETTINGS","MACROONCLICK"      ,m_pOptionParams.bMacroOnClick);
      m_iniOptions.WriteINIValueInt("SETTINGS","GUIDYNAMIC"        ,m_pOptionParams.bGUIDynamic);
      m_iniOptions.WriteINIValueInt("SETTINGS","LOCKRESIZE"        ,m_pOptionParams.bLockResize);
      m_iniOptions.WriteINIValueInt("SETTINGS","SHOWITEMINFO"      ,m_pOptionParams.bShowItemSpec);
      m_iniOptions.WriteINIValueInt("SETTINGS","SHOWNEWLIFE"       ,m_pOptionParams.bShowNewLife);
      m_iniOptions.WriteINIValueInt("SETTINGS","SHOWNEWOMBRAGE"    ,m_pOptionParams.bShowNewOmbrage);
      m_iniOptions.WriteINIValueInt("SETTINGS","SHOWANIMATEDDECORS",m_pOptionParams.bShowAnimDecorsLight);
      m_iniOptions.WriteINIValueInt("SETTINGS","SHOWMP3"           ,m_pOptionParams.bDisplayMp3);
      m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYDAMAGE"     ,m_pOptionParams.bDisplayDamage);
      m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYHEALING"    ,m_pOptionParams.bDisplayHealing);
      m_iniOptions.WriteINIValueInt("SETTINGS","BRIGHTNESS"        ,m_pOptionParams.dwBrightnessNew);
      m_iniOptions.WriteINIValueInt("SETTINGS","CHATBACKFONCE"     ,m_pOptionParams.bChatLogFonce);
      m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYTARGET"     ,m_pOptionParams.bDisplayTarget);
      m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYYOUTMI"     ,m_pOptionParams.bDisplayYouTMI);
      m_iniOptions.WriteINIValueInt("SETTINGS","RUN323FPS"         ,m_pOptionParams.bPlayAt32FPS);
      m_iniOptions.WriteINIValueInt("SETTINGS","HIDEMACRO"         ,m_pOptionParams.bNotDisplayMacro);
      m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYRESTANT"    ,m_pOptionParams.bDisplayRestant);
      m_iniOptions.WriteINIValueInt("SETTINGS","ANCIENNEMUSIC"     ,m_pOptionParams.bAncienneMusique);
      m_iniOptions.WriteINIValueInt("SETTINGS","OLDDISPLAYTEXT"    ,m_pOptionParams.bAncienDisplayOH);
      m_iniOptions.WriteINIValueInt("SETTINGS","LOGNPC"            ,m_pOptionParams.blogNPC);
      m_iniOptions.WriteINIValueInt("SETTINGS","LOGPERSONPC"       ,m_pOptionParams.blogNPCPerso);
      m_iniOptions.WriteINIValueInt("SETTINGS","LOGUSER"           ,m_pOptionParams.bLogUser);
      m_iniOptions.WriteINIValueInt("SETTINGS","LOGENABLE"         ,m_pOptionParams.bLogEnable);
      m_iniOptions.WriteINIValueInt("SETTINGS","CHESTLISTE"        ,m_pOptionParams.bChestListe);
      m_iniOptions.WriteINIValue("SETTINGS","LOGNAME",m_pOptionParams.LogName);
   }

   SYSTEMTIME sysTime; 
   GetLocalTime(&sysTime);
   sprintf_s(m_pOptionParams.LogName,48,"%02d%02d%02d_log.txt",sysTime.wMonth,sysTime.wDay,sysTime.wYear-2000);

   m_bIsLoaded = TRUE;
   return m_bIsLoaded;
}


BOOL CSaveGame::Save_SaveGame()
{

   if(!m_bIsLoaded)
      return FALSE;


   m_pInventoryL .clear();
   m_pChestL     .clear();
   m_pChestLG    .clear();

   //ALL Chest grid
   V3_InvDlg  ::GetInstance()->BackpackToSave();        //Protected
   V3_ChestDlg::GetInstance()->ChestToSave();           //Protected
   V3_GuildChestDlg::GetInstance()->GuildChestToSave(); //Protected

   FILE *pfI = NULL;
   fopen_s(&pfI,m_strSavFileNameI,"wb");
   if(pfI)
   {
      fwrite(&SAVEGAME_VERSION,2,1,pfI);
      int i=0;
      int dwNbrInv        = m_pInventoryL .size();
      int dwNbrChest      = m_pChestL     .size();
      int dwNbrChestG     = m_pChestLG    .size();

      fwrite(&dwNbrInv,1,4,pfI);
      fwrite(&dwNbrChest,1,4,pfI);
      fwrite(&dwNbrChestG,1,4,pfI);
      for(i=0;i<dwNbrInv;i++)
         fwrite(&m_pInventoryL[i],sizeof(sInventory),1,pfI);
      for(i=0;i<dwNbrChest;i++)
         fwrite(&m_pChestL[i],sizeof(sInventory),1,pfI);
      for(i=0;i<dwNbrChestG;i++)
         fwrite(&m_pChestLG[i],sizeof(sInventory),1,pfI);
      fclose(pfI);
   }
   
   //AllMacro Info
   m_pMacroItemL .clear();
   m_pMacroSpellL.clear();
   m_pMacroSkillL.clear();

   V3_MacroDlg::GetInstance()->MacroToSave(); //Protected

   FILE *pfM = NULL;
   fopen_s(&pfM,m_strSavFileNameM,"wb");
   if(pfM)
   {
      fwrite(&SAVEGAME_VERSION,2,1,pfM);
      int i=0;
      int dwNbrMacroItem  = m_pMacroItemL .size();
      int dwNbrMacroSpell = m_pMacroSpellL.size();
      int dwNbrMacroSkill = m_pMacroSkillL.size();

      fwrite(&dwNbrMacroItem,1,4,pfM);
      fwrite(&dwNbrMacroSpell,1,4,pfM);
      fwrite(&dwNbrMacroSkill,1,4,pfM);

      for(i=0;i<dwNbrMacroItem;i++)
         fwrite(&m_pMacroItemL[i],sizeof(sMacro),1,pfM);
      for(i=0;i<dwNbrMacroSpell;i++)
         fwrite(&m_pMacroSpellL[i],sizeof(sMacro),1,pfM);
      for(i=0;i<dwNbrMacroSkill;i++)
         fwrite(&m_pMacroSkillL[i],sizeof(sMacro),1,pfM);

      fclose(pfM);
   }

   //Save all CC and ignore list user
   m_pChannelL   .clear();
   m_pIgnoreL    .clear();
   V3_ChatDlg::GetInstance()->CCToSave(); //Protected:

   FILE *pfCC = NULL;
   fopen_s(&pfCC,m_strSavFileNameC,"wb");
   if(pfCC)
   {
      fwrite(&SAVEGAME_VERSION,2,1,pfCC);
      int i=0;
      int dwNbrChannel    = m_pChannelL   .size();
      int dwNbrIgnore     = m_pIgnoreL    .size();


      fwrite(&dwNbrChannel,1,4,pfCC);
      fwrite(&dwNbrIgnore,1,4,pfCC);

      for(i=0;i<dwNbrChannel;i++)
         fwrite(&m_pChannelL[i],sizeof(sChannel),1,pfCC);
      for(i=0;i<dwNbrIgnore;i++)
         fwrite(&m_pIgnoreL[i],sizeof(sIgnoreList),1,pfCC);

      fclose(pfCC);
   }


   FILE *pfRT = NULL;
   fopen_s(&pfRT,m_strSavFileNameRT,"wb");
   if(pfRT)
   {
      fwrite(&SAVEGAME_VERSION,2,1,pfRT);
      fwrite(&m_uchRTMap,10*TR_MAP_Y*TR_MAP_X,1,pfRT);
      fclose(pfRT);
   }

   m_iniOptions.WriteINIValueInt("SETTINGS","PAGEENABLE"        ,m_pOptionParams.bPageEnable);
   m_iniOptions.WriteINIValueInt("SETTINGS","MUSICVOLUME"       ,m_pOptionParams.dwMusicVol);
   m_iniOptions.WriteINIValueInt("SETTINGS","SOUNDVOLUME"       ,m_pOptionParams.dwSoundVol);
   m_iniOptions.WriteINIValueInt("SETTINGS","AMBIANTFXVOLUME"   ,m_pOptionParams.dwAmbiantVol);
   m_iniOptions.WriteINIValueInt("SETTINGS","USECD"             ,m_pOptionParams.bUseCD);
   m_iniOptions.WriteINIValueInt("SETTINGS","MP3VOLUME"         ,m_pOptionParams.iMp3Volume);
   m_iniOptions.WriteINIValueInt("SETTINGS","MP3STATUS"         ,m_pOptionParams.bStatusMp3);
   m_iniOptions.WriteINIValueInt("SETTINGS","MP3RANDOM"         ,m_pOptionParams.bMp3Random);
   m_iniOptions.WriteINIValueInt("SETTINGS","LOWLIGHTSIZE"      ,m_pOptionParams.bLightLowSize);
   m_iniOptions.WriteINIValueInt("SETTINGS","METEOEFFECT"       ,m_pOptionParams.bMeteoEffect);
   m_iniOptions.WriteINIValueInt("SETTINGS","DIETERGRAPH"       ,m_pOptionParams.bDitherGraph);
   m_iniOptions.WriteINIValueInt("SETTINGS","SHOWANIMSERAPH"    ,m_pOptionParams.bSeraphAnim);
   m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYSPELLSTATUS",m_pOptionParams.bDisplayStatus);
   m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYRADAR"      ,m_pOptionParams.bDisplayRadar);
   m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYCHATLOG"    ,m_pOptionParams.bDisplayChatLog);
   m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYGROUPINFO"  ,m_pOptionParams.bDisplayGroupInfo);
   m_iniOptions.WriteINIValueInt("SETTINGS","USECLOCKTARGET"    ,m_pOptionParams.bLockTarget);
   m_iniOptions.WriteINIValueInt("SETTINGS","MACROONCLICK"      ,m_pOptionParams.bMacroOnClick);
   m_iniOptions.WriteINIValueInt("SETTINGS","GUIDYNAMIC"        ,m_pOptionParams.bGUIDynamic);
   m_iniOptions.WriteINIValueInt("SETTINGS","LOCKRESIZE"        ,m_pOptionParams.bLockResize);
   m_iniOptions.WriteINIValueInt("SETTINGS","SHOWITEMINFO"      ,m_pOptionParams.bShowItemSpec);
   m_iniOptions.WriteINIValueInt("SETTINGS","SHOWNEWLIFE"       ,m_pOptionParams.bShowNewLife);
   m_iniOptions.WriteINIValueInt("SETTINGS","SHOWNEWOMBRAGE"    ,m_pOptionParams.bShowNewOmbrage);
   m_iniOptions.WriteINIValueInt("SETTINGS","SHOWANIMATEDDECORS",m_pOptionParams.bShowAnimDecorsLight);
   m_iniOptions.WriteINIValueInt("SETTINGS","SHOWMP3"           ,m_pOptionParams.bDisplayMp3);
   m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYDAMAGE"     ,m_pOptionParams.bDisplayDamage);
   m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYHEALING"    ,m_pOptionParams.bDisplayHealing);
   m_iniOptions.WriteINIValueInt("SETTINGS","BRIGHTNESS"        ,m_pOptionParams.dwBrightnessNew);
   m_iniOptions.WriteINIValueInt("SETTINGS","CHATBACKFONCE"     ,m_pOptionParams.bChatLogFonce);
   m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYTARGET"     ,m_pOptionParams.bDisplayTarget);
   m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYYOUTMI"     ,m_pOptionParams.bDisplayYouTMI);
   m_iniOptions.WriteINIValueInt("SETTINGS","RUN323FPS"         ,m_pOptionParams.bPlayAt32FPS);
   m_iniOptions.WriteINIValueInt("SETTINGS","HIDEMACRO"         ,m_pOptionParams.bNotDisplayMacro);
   m_iniOptions.WriteINIValueInt("SETTINGS","DISPLAYRESTANT"    ,m_pOptionParams.bDisplayRestant);
   m_iniOptions.WriteINIValueInt("SETTINGS","ANCIENNEMUSIC"     ,m_pOptionParams.bAncienneMusique);
   m_iniOptions.WriteINIValueInt("SETTINGS","OLDDISPLAYTEXT"    ,m_pOptionParams.bAncienDisplayOH);
   m_iniOptions.WriteINIValueInt("SETTINGS","LOGNPC"            ,m_pOptionParams.blogNPC);
   m_iniOptions.WriteINIValueInt("SETTINGS","LOGPERSONPC"       ,m_pOptionParams.blogNPCPerso);
   m_iniOptions.WriteINIValueInt("SETTINGS","LOGUSER"           ,m_pOptionParams.bLogUser);
   m_iniOptions.WriteINIValueInt("SETTINGS","LOGENABLE"         ,m_pOptionParams.bLogEnable);
   m_iniOptions.WriteINIValueInt("SETTINGS","CHESTLISTE"        ,m_pOptionParams.bChestListe);
   m_iniOptions.WriteINIValue("SETTINGS","LOGNAME",m_pOptionParams.LogName);

  
  
   return TRUE;
} 


void CSaveGame::SuspendSaveThread(BOOL bSuspend)
{
   ::EnterCriticalSection(&m_crLockSave);
   m_bSaveSuspended = bSuspend;
   if(m_bSaveSuspended)
      OutputDebugString("***** SAVE THREAD SUSPENDED !!!!!!\n");
   else
      OutputDebugString("***** SAVE THREAD ACTIVATED !!!!!!\n");
   ::LeaveCriticalSection(&m_crLockSave);
}


BOOL CSaveGame::bFreeAll()
{
   //Inv
   m_pInventoryL .clear();
   m_pChestL     .clear();
   m_pChestLG    .clear();
   
   //Macro
   m_pMacroItemL .clear();
   m_pMacroSpellL.clear();
   m_pMacroSkillL.clear();
   //CC
   m_pChannelL   .clear();
   m_pIgnoreL    .clear();
   
   m_bIsLoaded = FALSE;
   return TRUE;
}


BOOL CSaveGame::AddInv       (sInventory  pInventory)
{
   m_pInventoryL.push_back(pInventory);
   return TRUE;
}

BOOL CSaveGame::AddMacroItem (sMacro      pMacroItem)
{
   m_pMacroItemL.push_back(pMacroItem);
   return TRUE;
}

BOOL CSaveGame::AddMacroSpell(sMacro      pMacroSpell)
{
   m_pMacroSpellL.push_back(pMacroSpell);
   return TRUE;
}

BOOL CSaveGame::AddMacroSkill(sMacro      pMacroSkill)
{
   m_pMacroSkillL.push_back(pMacroSkill);
   return TRUE;
}

BOOL CSaveGame::AddChannelS(sChannel    pChannel)
{
   m_pChannelL.push_back(pChannel);
   return TRUE;
}

void CSaveGame::RemoveChannel(char *pStrName)
{
   std::vector <sChannel>::iterator i;
   for( i = m_pChannelL.begin(); i != m_pChannelL.end(); i++ )
   {
      if(strcmp((*i).strNameCC,pStrName) == 0)
      {
          m_pChannelL.erase(i);
          return;
      }
   }
}

BOOL CSaveGame::AddIgnore    (sIgnoreList pIgnore)
{
   m_pIgnoreL.push_back(pIgnore);
   return TRUE;
}

BOOL CSaveGame::AddChest (sInventory  pChest)
{
   m_pChestL.push_back(pChest);
   return TRUE;
}

BOOL CSaveGame::AddChestG (sInventory  pChest)
{
   m_pChestLG.push_back(pChest);
   return TRUE;
}


sInventory  *CSaveGame::GetInv(UINT dwIndex)
{
   if(dwIndex < m_pInventoryL.size())
      return &m_pInventoryL[dwIndex];
   return NULL;
}

sInventory  *CSaveGame::GetChest(UINT dwIndex)
{
   if(dwIndex < m_pChestL.size())
      return &m_pChestL[dwIndex];
   return NULL;
}

sInventory  *CSaveGame::GetChestG(UINT dwIndex)
{
   if(dwIndex < m_pChestLG.size())
     return &m_pChestLG[dwIndex];
   return NULL;
}

sMacro *CSaveGame::GetMItem  (UINT dwIndex)
{
   if(dwIndex < m_pMacroItemL.size())
      return &m_pMacroItemL[dwIndex];
   return NULL;
}

sMacro *CSaveGame::GetMSpell (UINT dwIndex)
{
   if(dwIndex < m_pMacroSpellL.size())
      return &m_pMacroSpellL[dwIndex];
   return NULL;
}

sMacro *CSaveGame::GetMSkill (UINT dwIndex)
{
   if(dwIndex < m_pMacroSkillL.size())
      return &m_pMacroSkillL[dwIndex];
   return NULL;
}



sChannel *CSaveGame::GetChannel(UINT dwIndex)
{
   if(dwIndex < m_pChannelL.size())
      return &m_pChannelL[dwIndex];
   return NULL;
}

sIgnoreList *CSaveGame::GetIgnore (UINT dwIndex)
{
   if(dwIndex < m_pIgnoreL.size())
      return &m_pIgnoreL[dwIndex];
   return NULL;
}



void CSaveGame::SetRTMapVal(USHORT ushX,USHORT ushY,USHORT ushW )
{
   int dwRtX = ushX/TR_MAP_XDIV;
   int dwRtY = ushY/TR_MAP_YDIV;

   m_uchRTMap[ushW][dwRtY][dwRtX] = 1;
}

BOOL CSaveGame::GetRTMapVal(USHORT ushX,USHORT ushY,USHORT ushW , int *pdwX,int *pdwY)
{  
   int dwRtX = ushX/TR_MAP_XDIV;
   int dwRtY = ushY/TR_MAP_YDIV;

   *pdwX = dwRtX;
   *pdwY = dwRtY;

   if(m_uchRTMap[ushW][dwRtY][dwRtX]==1)
      return TRUE;
   else
      return FALSE;
}

BOOL CSaveGame::GetRTMapValCase(USHORT ushX,USHORT ushY,USHORT ushW )
{  
   if(ushX >= TR_MAP_X || ushY >= TR_MAP_Y)
      return FALSE;

   if(m_uchRTMap[ushW][ushY][ushX]==1)
      return TRUE;
   else
      return FALSE;
}






void CSaveGame::SaveGameThread(LPVOID lpData)
{
   CSaveGame *me = (CSaveGame *)lpData;

   while(me->m_bSaveThread)
   {                                                    
      switch( ::WaitForSingleObject(me->m_hSaveThreadExit,1000*60*5)) //timeout tous les 5 min pour enregistrer...
      { 
         case WAIT_OBJECT_0+0 :
         {
            me->m_bSaveThread = FALSE;
         }
         break;
         case WAIT_TIMEOUT: 
         {
            if(!g_boQuitApp)
            {
               //on Save...
               ::EnterCriticalSection(&me->m_crLockSave);
               if(!me->m_bSaveSuspended)
               {
                  OutputDebugString("***** START SAVE !!!!!!\n");
                  me->Save_SaveGame();  //SaveGame...
                  OutputDebugString("***** STOP SAVE !!!!!!\n");
               }
               ::LeaveCriticalSection(&me->m_crLockSave);
            }
         }
         break;
      }
   }

}