// TFCPlayer.cpp: implementation of the TFCPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "TFCPlayer.h"
#include "Global.h"
#include "Packet.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "VisualObjectList.h"

TFCPlayer Player;

TFCPlayer::TFCPlayer()
{
    CanRunScripts = false;
    CanSlayUsers = false;
	LastAttack = 0;
   LastObject = 0;
   lKey = 0;
	ViewObjectFlag = 0;
   Light = 0;
	AttackMode = 0;
	SetGridMode = 0;
	Dead = false;
   rangedAttack = false;
   Level = 10000;
   isSeraph = false;
   AlreadyPlayHisAnim = false;
//	Hp = MaxHp = Mana = MaxMana = 0;
	
	for( int i = 0; i < NBR_EQUIPED_MAX; i++ ){
		lpbEquipped[ i ] = NULL;
	}


   iPAir = iPFeu = iPEau = iPTerre = iPNoir = iPLumiere = 0;
   iPBAir= iPBFeu= iPBEau= iPBTerre= iPBNoir= iPBLumiere= 0;

   iRAir = iRFeu = iREau = iRTerre = iRNoir = iRLumiere = 0;
   iRBAir= iRBFeu= iRBEau= iRBTerre= iRBNoir= iRBLumiere= 0;

	InitializeCriticalSection( &csThreadLock );
}

TFCPlayer::~TFCPlayer()
{
	tlBackpack.ToHead();
   while (tlBackpack.QueryNext())
      tlBackpack.DeleteObject();

   tlUserSkill.ToHead();
   while (tlUserSkill.QueryNext()) 
   {
      tlUserSkill.DeleteObject();
   }

   for (int i = 0; i < NBR_EQUIPED_MAX; i++)
      if (Player.lpbEquipped[i])
         delete Player.lpbEquipped[i];

   DeleteProfession();

 

   DeleteCriticalSection( &csThreadLock );
}

void TFCPlayer::DeleteProfession( void )
{
   //delete les profession...
   tlProfessionAll.ToHead();
   while (tlProfessionAll.QueryNext())
   {
      LPUSER_PROFESSION p1 = tlProfessionAll.GetObject();
      //delete les profesison name.,..
      p1->tlProfession.ToHead();
      while (p1->tlProfession.QueryNext())
         p1->tlProfession.DeleteObject();

      //delete les Formule Info.,..
      p1->tlFormule.ToHead();
      while (p1->tlFormule.QueryNext())
      {
         LPFORMULE_INFO pf = p1->tlFormule.GetObject();   
         if(pf->pName)
            delete []pf->pName;
         //Delete la liste des requis
         pf->tlFormuleReq.ToHead();
         while (pf->tlFormuleReq.QueryNext())
         {
            LPFORMULE_REQ pr = pf->tlFormuleReq.GetObject();   
            if(pr->pName)
               delete []pr->pName;
            pf->tlFormuleReq.DeleteObject();
         }
         //pf->tlFormuleReq.ToHead();
         //pf->tlFormuleReq.DeleteObject();

         p1->tlFormule.DeleteObject();
      }
      //p1->tlFormule.ToHead();
      //p1->tlFormule.DeleteObject();


      //tlProfessionAll.ToHead();
      tlProfessionAll.DeleteObject();
   }
   
}

void TFCPlayer::Lock( void ){
	EnterCriticalSection( &csThreadLock );
}

void TFCPlayer::Unlock( void ){
	LeaveCriticalSection( &csThreadLock );
}


//////////////////////////////////////////////////////////////////////////////////////////
void TFCPlayer::ScreenPosToWL
//////////////////////////////////////////////////////////////////////////////////////////
// This function converts pixel screen coordinates into world coordinates.
// 
(
 LPPOINT lppScreenPos,	// Pointer to screen position
 LPPOINT lppWorldPos	// Pointer to world position
)
//////////////////////////////////////////////////////////////////////////////////////////
{
   lppWorldPos->x = ((lppScreenPos->x-(g_Global.GetScreenWDiv2()-16))/32) + xPos + ((lppScreenPos->x < (g_Global.GetScreenWDiv2()-16)) ? -1 : 0);
	lppWorldPos->y = ((lppScreenPos->y-(g_Global.GetScreenHDiv2()+24))/16) + yPos + ((lppScreenPos->y < (g_Global.GetScreenHDiv2()+24)) ? -1 : 0);
}
