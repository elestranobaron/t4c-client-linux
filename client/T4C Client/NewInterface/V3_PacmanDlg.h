// V3_PacmanDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "..\NMTimerUtils.h"
#include "..\Game01\PathFinder.h"



#define NBR_MAX_TIMER   20

typedef int (*NM_TIMER_CallbackFct)(void* pThis,unsigned int dwCode);

class V3_PacmanDlg : public BoxUI  
{
public:	
	virtual ~V3_PacmanDlg();


    static V3_PacmanDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    int TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise);
    void Hide();

    void StartGame();
    bool VKeyInput( VKey vKey );
        

protected:
   CNMTimerUtils m_TimerTools;

private:
   // List containers.
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;

   void OnTimer(unsigned int nIDEvent);
    
   
   RECT m_RCp;
   //PackMan Game variables.....
   bool Pause;
   bool PauseSauvegarde;
   bool DejaBonus;
   
   int NbTotalPoint;
   
   //bool DejaNouveau;
   bool DejaDessiner;
   int m_Vie;
   int m_PrixFantome;
   int time8;
   int m_Signal;
   bool DepParticulier;
   int PacDx;
   int PacDy;
   int m_Niveau;
   int AttenteX;
   int CompteurFin;
   int AttenteY;
   int PacCpt;
   bool m_Fin;
   bool PacmanDeplace;
   int PacmanTargetY;
   int PacmanTargetX;
   long int m_Score;
   int m_iCptA[4];
   
   bool m_bgob1 ;
   bool m_bgob2 ;
   bool m_bgob3 ;
   bool m_bgob4 ;
   
   
   
   int  NbPointsActuel();
   bool PlusDePoints();
   int ValeurAbsolue(int a);
   void TuerPacman();
   bool Collision(CFantome * sp);
   void ChargerLesXetLesY(void);
   bool PacmanCanMove(int dx,int dy);
   bool CanMoveFantome(CSprite * sp,int dx,int dy);
   void TestCollisions();
   
   
   void CreeBonus();
   
   void NouveauPacman(bool bForce);
   
   
   void DessineBonus(bool comment);
   void DeplaceFantome(CFantome * sp);
   void DeplaceFantomeHasard(CFantome * sp);
   void DeplacePacman(int dx,int dy);
   
   
   
   void ManageGame_S01();
   
   
   
   bool  m_GameDessineBonnus;
   int   m_iPacmanID;
   int   m_iPacmanXP;
   int   m_iPacmanYP;
   
   int   m_iFantomeID[4];
   int   m_iFantomeXP[4];
   int   m_iFantomeYP[4];




private:
   V3_PacmanDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;

   GraphUI  backGame;
   ButtonUI  PlayButton;
   ButtonUI  PauseButton;



   class PlayButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } playButtonEvent;
   
   class PauseButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } pauseButtonEvent;
   
   
   friend PlayButtonEvent;
   friend PauseButtonEvent;

   HPEN m_hPenCrayon;
   HBRUSH m_hBrosse;
   
   
   
   CPathFinder *m_pLabyrinthe;
   CPacman     *m_pPacman;
   
   CFantome    *m_pFantome[4];
   

   static int    TimerCallback(void* pThis,unsigned int nIDEvent);

};


