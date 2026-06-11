// ************************************************************************
// ***                                                                  ***
//      File Name: GameMusic.Cpp
//      Project:   The 4th Coming
//      Creation:  25 Mars, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// *** Copyright (c) 1997-1999 Vircom. All rights reserved.             ***

// Include File
#include "pch.h"
#include "GameMusic.h"
#include "TFCPlayer.h"
#include "Global.h"
#include "App.h"
#include "SaveGame.h"
#include "NMPlayVideo.h"
#include "TileSet.h"
extern CSaveGame g_SaveGame;
extern TileSet World;

#define Track45(a,b,c,d) ((Y >= -X + a) && (Y >= X + b) && (Y <= -X + c) && (Y <= X + d))
#define Track90(a,b,c,d) ((X >= a && X <= b) && (Y >= c && Y <= d))

// Gloval Var
GameMusic g_GameMusic;
CNMPlayVideo  *m_pMp3GameMusic;
CNMPlayVideo  *m_pMp3GameFX;



static int const BOSS_MUSIC			= 0;//RGB (255,255,255)
static int const OUTDOORS_MUSIC     = 1;//RGB (255,  0,  0)
static int const FOREST_MUSIC	    	= 2;//RGB (  0,255,  0)
static int const DUNGEON_MUSIC		= 3;//RGB (255,255,  0)
static int const CAVERN_MUSIC	   	= 4;//RGB (  0,255,255)
static int const SADNESS_MUSIC		= 5;//RGB (255,  0,255)
static int const NOISES_MUSIC		   = 6;//RGB (  0,  0,255)
static int const HALOWEEN_MUSIC     = 7;//RGB (255,128,  0)
static int const NOEL_MUSIC         = 8;//RGB (  0,128,255)
static int const SILENCE_MUSIC		= 9;

static int const FX_NONE    			= 0;//RGB (0  ,  0,  0)
static int const FX_GROTTE_EFFECT01 = 1;//RGB (255,  0,  0)
static int const FX_CRYPT_EFFECT01  = 2;//RGB (128,  0,  0)

GameMusic::GameMusic(void) 
{
	m_dwMusicNumber	 = INFINITE;
	m_dwOldMusicNumber = -1;
   m_dwFXNumber	    = INFINITE;
   m_dwOldFXNumber    = -1;

   m_pMp3GameMusic = NULL;
   m_pMp3GameFX    = NULL;
	InitializeCriticalSection(&csLock);

   m_bThreadRunning = TRUE;
   _beginthread(Mp3MusicPlayCheckLoop, 0, this );
}

GameMusic::~GameMusic(void) 
{
   Reset();
   m_bThreadRunning = FALSE;
   Sleep(100);
   
	DeleteCriticalSection(&csLock);
}

void GameMusic::Mp3MusicPlayCheckLoop(LPVOID pParameter)
{
   GameMusic* me = reinterpret_cast<GameMusic*>(pParameter);
   while(me->m_bThreadRunning)
   {
      me->CheckLooping();
      Sleep(10);
   }
   

}

// Set a New Music Volume
void GameMusic::SetNewVolume(void) 
{

	// Lock
	EnterCriticalSection(&csLock);
   if(m_pMp3GameMusic)
      m_pMp3GameMusic  ->SetVolumePourcent(g_SaveGame.GetOptionsParam()->dwMusicVol*5,true);

   if(m_pMp3GameFX)
      m_pMp3GameFX  ->SetVolumePourcent(g_SaveGame.GetOptionsParam()->dwAmbiantVol*5,true);

	// Unlock
	LeaveCriticalSection(&csLock);
}


void GameMusic::Reset(void) 
{
   // Reset the Music.
   if (g_SaveGame.GetOptionsParam()->bUseCD) 
   {
      StopCDAudio();
   } 

   EnterCriticalSection(&csLock);
   if(m_pMp3GameMusic)
      m_pMp3GameMusic  ->CmdStop();

   if(m_pMp3GameFX)
      m_pMp3GameFX  ->CmdStop();
   LeaveCriticalSection(&csLock);

   m_dwOldMusicNumber = INFINITE;
   m_dwOldFXNumber    = INFINITE;
}

 
void GameMusic::Start(void) 
{
   // Start the Music.
	EnterCriticalSection(&csLock);
   if(m_pMp3GameMusic)
   {
      delete m_pMp3GameMusic;
      m_pMp3GameMusic = NULL;
   }

   if(m_pMp3GameFX)
   {
      delete m_pMp3GameFX;
      m_pMp3GameFX = NULL;
   }

	if (!g_SaveGame.GetOptionsParam()->bUseCD && g_SaveGame.GetOptionsParam()->bStatusMp3 == MP3_DISABLE) 
   {
      m_pMp3GameMusic = new CNMPlayVideo(NULL,NULL);
      m_pMp3GameMusic  ->SetVolumePourcent(g_SaveGame.GetOptionsParam()->dwMusicVol*5,true);
      if(m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\SadnessNew.mp3", 0) != S_OK)
      {
         delete m_pMp3GameMusic;
         m_pMp3GameMusic = NULL;
      }
	}
	LeaveCriticalSection(&csLock);
}

void GameMusic::CheckLooping(void) 
{
   EnterCriticalSection(&csLock);
   if(m_pMp3GameMusic)
   {
      REFERENCE_TIME rtC = m_pMp3GameMusic->GetDVTimecodeS();
      REFERENCE_TIME rtT = m_pMp3GameMusic->GetDVTimecodeTotalS();
      if(rtC >= rtT)
         m_pMp3GameMusic->SetToStartPos();

   }

   if(m_pMp3GameFX)
   {
      REFERENCE_TIME rtC = m_pMp3GameFX->GetDVTimecodeS();
      REFERENCE_TIME rtT = m_pMp3GameFX->GetDVTimecodeTotalS();
      if(rtC >= rtT)
         m_pMp3GameFX->SetToStartPos();
   }

   
   LeaveCriticalSection(&csLock);
}

void GameMusic::FocusPlay(void)
{
   OptionParam *pOption = g_SaveGame.GetOptionsParam();
   if(pOption->bDisplayMp3)
      return;
   EnterCriticalSection(&csLock);
   if(m_pMp3GameMusic)
      m_pMp3GameMusic->CmdPlay();
   if(m_pMp3GameFX)
      m_pMp3GameFX->CmdPlay();
   LeaveCriticalSection(&csLock);
}

void GameMusic::FocusPause(void)
{
   OptionParam *pOption = g_SaveGame.GetOptionsParam();
   if(pOption->bDisplayMp3)
      return;

   EnterCriticalSection(&csLock);
   if(m_pMp3GameMusic)
      m_pMp3GameMusic->CmdPausePause();
   if(m_pMp3GameFX)
      m_pMp3GameFX->CmdPausePause();
   LeaveCriticalSection(&csLock);
}

void GameMusic::LoadNewSound(char *pstrReason) 
{
   //char strTmp[100];
   //sprintf_s(strTmp,100,"LoadNewSound-->%s\n",pstrReason);
   //OutputDebugString(strTmp);

   
   OptionParam *pOption = g_SaveGame.GetOptionsParam();
   if(pOption->bDisplayMp3)
   {
      EnterCriticalSection(&csLock);
      if(m_pMp3GameMusic)
      {
         delete m_pMp3GameMusic;
         m_pMp3GameMusic = NULL;
      }

      if(m_pMp3GameFX)
      {
         delete m_pMp3GameFX;
         m_pMp3GameFX = NULL;
      }
      LeaveCriticalSection(&csLock);
      return; //Mp3Player Actif on fait Rien...
   }

	// Temp Var (X & Y are more readable than Player.xPos & Player.yPos)
	int X = Player.xPos;
	int Y = Player.yPos;

   m_dwMusicNumber = World.GetPosViewM(X,Y);

   if (m_dwMusicNumber != m_dwOldMusicNumber) 
   {
	   // If the new Music Number is not the same as the Old Music Number (made a continuous play) AND
	   // There is some volume on on CD music playing.
		// Set the Old Music Number.
		m_dwOldMusicNumber = m_dwMusicNumber;
      if(g_SaveGame.GetOptionsParam()->bStatusMp3 >MP3_DISABLE)
      {
         EnterCriticalSection(&csLock);
         if(m_pMp3GameMusic)
         {
            delete m_pMp3GameMusic;
            m_pMp3GameMusic = NULL;
         }
         LeaveCriticalSection(&csLock);
      }
		else if (g_SaveGame.GetOptionsParam()->bUseCD) 
      {
		// CD Music Playing
			switch (m_dwMusicNumber) 
         {
			// Play the appropriate Music
				case BOSS_MUSIC:		PlayCDAudio(2, TRUE); break;
				case OUTDOORS_MUSIC: PlayCDAudio(3, TRUE); break;
				case FOREST_MUSIC:	PlayCDAudio(4, TRUE); break;
				case DUNGEON_MUSIC:	PlayCDAudio(5, TRUE); break;
				case CAVERN_MUSIC:	PlayCDAudio(6, TRUE); break;
				case SADNESS_MUSIC:  PlayCDAudio(7, TRUE); break;
            case NOISES_MUSIC:   PlayCDAudio(8, TRUE); break;
			}
		} 
      else 
      {
		   // HD Music Playing
         EnterCriticalSection(&csLock);
			if (m_dwMusicNumber != SILENCE_MUSIC) 
         {
            PlayMusicID(m_dwMusicNumber);
			} 
         else 
         {
            if(m_pMp3GameMusic)
            {
               delete m_pMp3GameMusic;
               m_pMp3GameMusic = NULL;
            }
			}
         LeaveCriticalSection(&csLock);
		}
	}


   m_dwFXNumber = World.GetPosViewX(X,Y);
   if (m_dwFXNumber != m_dwOldFXNumber) 
   {
      // If the new Music Number is not the same as the Old Music Number (made a continuous play) AND
      // There is some volume on on CD music playing.
      // Set the Old Music Number.
      m_dwOldFXNumber = m_dwFXNumber;
      if(g_SaveGame.GetOptionsParam()->bStatusMp3 >MP3_DISABLE)
      {
         EnterCriticalSection(&csLock);
         if(m_pMp3GameFX)
         {
            delete m_pMp3GameFX;
            m_pMp3GameFX = NULL;
         }
         LeaveCriticalSection(&csLock);
      }
      else 
      {
         // HD Music Playing
         EnterCriticalSection(&csLock);
         if (m_dwFXNumber != FX_NONE) 
         {
            PlayFXID(m_dwFXNumber);
         } 
         else 
         {
            if(m_pMp3GameFX)
            {
               delete m_pMp3GameFX;
               m_pMp3GameFX = NULL;
            }
         }
         LeaveCriticalSection(&csLock);
      }
   }
}

void GameMusic::PlayMusicID(int iID)
{
   if(m_pMp3GameMusic)
   {
      delete m_pMp3GameMusic;
      m_pMp3GameMusic = NULL;
   }
   m_pMp3GameMusic = new CNMPlayVideo(NULL,NULL);
   m_pMp3GameMusic  ->SetVolumePourcent(g_SaveGame.GetOptionsParam()->dwMusicVol*5,true);
   

   HRESULT hr = S_OK+1;
   switch (iID) 
   {
      // Load the New Music
      case BOSS_MUSIC:		hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\Boss.mp3", 0);		      break;
      case OUTDOORS_MUSIC: hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\Outdoors.mp3", 0);		   break;
      case FOREST_MUSIC:	
         if(g_SaveGame.GetOptionsParam()->bAncienneMusique)
            hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\Forest.mp3", 0);		   
         else
            hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\ForestNew.mp3", 0);	
         
      break;
      case DUNGEON_MUSIC:	hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\dungeons.mp3", 0);		   break;
      case CAVERN_MUSIC:	
         if(g_SaveGame.GetOptionsParam()->bAncienneMusique)
            hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\Caverns.mp3", 0);		   
         else
            hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\CavernsNew.mp3", 0);		   
      break;
      case SADNESS_MUSIC:  
         if(g_SaveGame.GetOptionsParam()->bAncienneMusique)
            hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\Sadness.mp3", 0);		
         else
            hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\SadnessNew.mp3", 0);		
      break;
      case NOISES_MUSIC:   hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\Forest noises.mp3", 0);	break;
      case HALOWEEN_MUSIC: hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\Halloween01.mp3", 0);		break;
      case NOEL_MUSIC:     hr = m_pMp3GameMusic  ->PlayMovieInWindow("Music Files\\NoelMusic01.mp3", 0);		break;
   }
   if(hr != S_OK)
   {
      if(m_pMp3GameMusic)
      {
         delete m_pMp3GameMusic;
         m_pMp3GameMusic = NULL;
      }
   }
}


void GameMusic::PlayFXID(int iID)
{
   if(m_pMp3GameFX)
   {
      delete m_pMp3GameFX;
      m_pMp3GameFX = NULL;
   }
   m_pMp3GameFX = new CNMPlayVideo(NULL,NULL);
   m_pMp3GameFX  ->SetVolumePourcent(g_SaveGame.GetOptionsParam()->dwAmbiantVol*5,true);


   HRESULT hr = S_OK+1;
   switch (iID) 
   {
      // Load the New Music
      case FX_GROTTE_EFFECT01:	 
         hr = m_pMp3GameFX->PlayMovieInWindow("FX Files\\Cavern01.mp3", 0);		      
      break;
      case FX_CRYPT_EFFECT01 :    
         hr = m_pMp3GameFX->PlayMovieInWindow("FX Files\\crypt01.mp3", 0);		      
      break;
   }
   if(hr != S_OK)
   {
      if(m_pMp3GameFX)
      {
         delete m_pMp3GameFX;
         m_pMp3GameFX = NULL;
      }
   }
}