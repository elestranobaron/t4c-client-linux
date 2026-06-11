// ************************************************************************
// ***                                                                  ***
//      File Name: GameMusic.h
//      Project:   The 4th Coming
//      Creation:  25 Mars, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***

#ifndef GAMEMUSIC_H
#define GAMEMUSIC_H

// Include File
#include "NewSound.h"

class GameMusic {
   // Class that manage the Music of T4C.
private:	
   CRITICAL_SECTION 	csLock;				   // The Lock.
   DWORD					m_dwMusicNumber;		// The Music Number current playing.
   DWORD					m_dwOldMusicNumber;  // The Old Music Number playing.
   DWORD					m_dwFXNumber;		// The Music Number current playing.
   DWORD					m_dwOldFXNumber;  // The Old Music Number playing.
   BOOL              m_bThreadRunning;

public:
   GameMusic(void);		// Constuctor.
   ~GameMusic(void);		// Destructor.

   void SetNewVolume(void); // Set a New music volume.
   void Start(void);			 // Start the Music.
   void LoadNewSound(char *pstrReason);     // Load a new sound for the new area.
   void Reset(void);			 // Reset the Music.
   void FocusPlay(void);
   void FocusPause(void);
protected:
   void PlayMusicID(int iID);
   void PlayFXID(int iID);
   void CheckLooping(void);

private:
   static void Mp3MusicPlayCheckLoop(LPVOID pParameter);
}; 

// Global Var
extern GameMusic g_GameMusic;

#endif