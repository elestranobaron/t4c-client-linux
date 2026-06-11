// V3_PacmanDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_PacmanDlg.h"
#include "V3_ChatLogDlg.h"

extern Font *fT4CDef_23B;

namespace
{
   T3VSBSound *PacOpen     = NULL;
   T3VSBSound *PacEat      = NULL;
   T3VSBSound *PacEatGhost = NULL;
   T3VSBSound *PacEatBonus = NULL;
   T3VSBSound *PacDie      = NULL;
   T3VSBSound *PacEatGob   = NULL;
   
   
   void InitSound()
   {
      if( PacOpen == NULL )
      {
         PacOpen = new T3VSBSound;
         PacOpen->Create( "PAC_Open", TS_MEMORY );
         
         PacEat = new T3VSBSound;
         PacEat->Create( "PAC_Eat", TS_MEMORY );
         
         PacEatGhost = new T3VSBSound;
         PacEatGhost->Create( "PAC_EatGhost", TS_MEMORY );
         
         PacEatBonus = new T3VSBSound;
         PacEatBonus->Create( "PAC_EatBonus", TS_MEMORY );
         
         PacDie = new T3VSBSound;
         PacDie->Create( "PAC_Die", TS_MEMORY );
         
         PacEatGob = new T3VSBSound;
         PacEatGob->Create( "PAC_EatGob", TS_MEMORY );
         
      }
   }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_PacmanDlg::V3_PacmanDlg():
X_Button( hideEvent ),
PlayButton( playButtonEvent,"Play",g_DefColorH ),
PauseButton( pauseButtonEvent,"Pause",g_DefColorH )

{
   int i=0;
   int iv=0;
   InitSound();
   m_iXPos = (g_Global.GetScreenW()-g_PacDlgW)/2;;
   m_iYPos = (((g_Global.GetScreenH()-g_PacDlgH)/2)-(g_MainBarDlgH/2)-20);
   
   SetHelpId( this, 285 );
  
   m_BackgroundS.SetSpriteId("V3_PacmanBack",true,"V3_PacmanBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_StatIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

  
   backGame.SetSpriteId( "V3_PacGameBack" );
   backGame.DrawAlpha  (false);
  

   PlayButton.SetSpriteId   ( "V3_Btn72x24_N" );
   PlayButton.SetDownUI     ( "V3_Btn72x24_D" );
   PlayButton.SetHighlightUI( "V3_Btn72x24_H" );
   playButtonEvent.SetUI    ( this );
   
   PauseButton.SetSpriteId   ( "V3_Btn72x24_N" );
   PauseButton.SetDownUI     ( "V3_Btn72x24_D" );
   PauseButton.SetHighlightUI( "V3_Btn72x24_H" );
   pauseButtonEvent.SetUI    ( this );
     

   
  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &backGame);
   AddChild( &PlayButton);
   AddChild( &PauseButton);



 
   RootBoxUI::GetInstance()->AddChild( this );



   m_hPenCrayon = CreatePen(PS_SOLID,3,CL_BLUE);
   m_hBrosse    = CreateSolidBrush(CL_BLUE_DARK);
   
   
   
   m_RCp.left   = m_iXPos  + 46;
   m_RCp.top    = m_iYPos  + 51;
   m_RCp.right  = m_RCp.left + 576;
   m_RCp.bottom = m_RCp.top  + 468;
   
   
   m_pLabyrinthe    = new CPathFinder(m_RCp);
   m_pPacman        = new CPacman(0);
   
   for(i=0,iv=3;i<4;i++,iv--)
   {
      m_pFantome[i]  = new CFantome(iv);
      m_iCptA[i]     = 0;
   }
   
   
   //Init game....
   Pause          = false;
   PauseSauvegarde= false;
   DejaBonus      = false;
   DejaDessiner   = false;
   m_Fin          = true;
   PacmanDeplace  = true;
   DepParticulier = false;
   m_GameDessineBonnus = false;
   m_bgob1 = true;
   m_bgob2 = true;
   m_bgob3 = true;
   m_bgob4 = true;
   
   m_Niveau    = 1;
   CompteurFin = 0;
   m_Vie       = 6;
   m_Signal    = 0;
   time8       = 0;
   m_Score     = 1;
   
   PacCpt        =  0;
   PacDx         =  0;
   PacDy         = -1;
   AttenteX      =  0;
   AttenteY      =  0;
   
   m_iPacmanID   = -1;
   for(i=0;i<4;i++)
      m_iFantomeID[4] = -1;
   
   
   PacmanTargetY = EN_DEHORS;
   PacmanTargetX = EN_DEHORS;
   
   m_pLabyrinthe->SetCase(m_pPacman->GetPosX(),m_pPacman->GetPosY(),CASE_VIDE);
   
	NbTotalPoint = NbPointsActuel();

} 

V3_PacmanDlg::~V3_PacmanDlg()
{

   if(m_pLabyrinthe)
      delete m_pLabyrinthe;
   m_pLabyrinthe = NULL;
   
   if(m_pPacman)
      delete m_pPacman;
   m_pPacman = NULL;
   
   for(int i=0;i<4;i++)
   {
      if(m_pFantome[i])
         delete m_pFantome[i];
      m_pFantome[i] = NULL;
   }
   
   DeleteObject(m_hPenCrayon);
   DeleteObject(m_hBrosse);


   m_TimerTools.KillNMTimer(-1);
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_PacmanDlg *V3_PacmanDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_PacmanDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_PacmanDlg::Display( void *null)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   // If inventory is already shown.
   if( GetInstance()->IsShown())
   {
      // Hide box
      GetInstance()->Hide();
   }
   else
   {
      GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
      GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      GetInstance()->Show();
   }
   return 0;
}

void V3_PacmanDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_PacDlgW, iPosY+g_PacDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_PacDlgW, iPosY+g_PacDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+614, iPosY+27, iPosX+614+27, iPosY+27+24 ) );

   backGame.SetPos( FramePos( iPosX+46, iPosY+51, iPosX+621, iPosY+518 ) );
   PlayButton.SetPos( FramePos( iPosX+450, iPosY+27, iPosX+450+72, iPosY+27+29 ) );
   PauseButton.SetPos( FramePos( iPosX+526, iPosY+27, iPosX+526+72, iPosY+27+24 ) );

}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_PacmanDlg::Draw(V2SPRITEFX *vsfFX)
{
   static bool first = true;
   static bool deja  = false;

   if(!IsShown())
      return;

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

  

   GameUI::Draw( vsfFX );

   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   PlayButton .DrawTextButtonExt(hdc);
   PauseButton.DrawTextButtonExt(hdc);
   
   // Draw les texte...
   char strTitle[100];
   sprintf_s(strTitle  ,100,"NMS T4CMan Mouaaa!!!");

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,strTitle,strlen(strTitle));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);


   if(!DejaDessiner) 
      DejaDessiner=true;
   
   //m_pLabyrinthe->DessineLabyrinthe(hdc);
   
   int i,j;
   for(i=0;i<MAXX;i++)
   {
      for(j=0;j<MAXY;j++)
      {
         if(m_pLabyrinthe->GetCase(i,j)==PAC_POINT)
            m_pLabyrinthe->DessinePoint(i,j,true,hdc);
      };
   }
   
   if(m_bgob1) 
      m_pLabyrinthe->DessineGob(9,1,m_bgob1,hdc);
   
   if(m_bgob2) 
      m_pLabyrinthe->DessineGob(25,1,m_bgob2,hdc);
   
   if(m_bgob3) 
      m_pLabyrinthe->DessineGob(9,24,m_bgob3,hdc);
   
   if(m_bgob4) 
      m_pLabyrinthe->DessineGob(25,24,m_bgob4,hdc);
   
   
   char strTmp[100];
   if(m_Fin && m_Vie <1)
   {
      sprintf_s(strTmp,100  ,"Game Over");
      fT4CDef_23B->DrawFont(hdc, iOffX+TxtOff(fT4CDef_23B,hdc,strTmp ,69)+273, iOffY+440 , g_DefColorH, 0, strTmp);   
      fT4CDef_23B->DrawFont(hdc, iOffX+TxtOff(fT4CDef_23B,hdc,strTmp ,69)+381, iOffY+440 , g_DefColorH, 0, strTmp);   
   }
   else
   {
      
      sprintf_s(strTmp,100  ,"Niveau");
      fT4CDef_23B->DrawFont(hdc, iOffX+TxtOff(fT4CDef_23B,hdc,strTmp ,69)+273, iOffY+70 , g_DefColorH    , 0, strTmp);
      
      sprintf_s(strTmp,100  ,"%d",m_Niveau);
      fT4CDef_23B->DrawFont(hdc, iOffX+TxtOff(fT4CDef_23B,hdc,strTmp ,69)+273, iOffY+90 , g_DefColorD    , 0, strTmp);
      
      
      sprintf_s(strTmp,100  ,"Score");
      fT4CDef_23B->DrawFont(hdc, iOffX+TxtOff(fT4CDef_23B,hdc,strTmp ,69)+381, iOffY+70 , g_DefColorH    , 0, strTmp);
      
      sprintf_s(strTmp,100  ,"%d",m_Score);
      fT4CDef_23B->DrawFont(hdc, iOffX+TxtOff(fT4CDef_23B,hdc,strTmp ,69)+381, iOffY+90 , g_DefColorD    , 0, strTmp);
      
      sprintf_s(strTmp,100  ,"%d",m_Vie);
      fT4CDef_23B->DrawFont(hdc, iOffX+314, iOffY+127 , g_DefColorD , 0, strTmp);
      
      
      
      if(Pause)
      {
         sprintf_s(strTmp,100  ,"Pause");
         fT4CDef_23B->DrawFont(hdc, iOffX+TxtOff(fT4CDef_23B,hdc,strTmp ,69)+273, iOffY+440 , g_DefColorH, 0, strTmp);   
         fT4CDef_23B->DrawFont(hdc, iOffX+TxtOff(fT4CDef_23B,hdc,strTmp ,69)+381, iOffY+440 , g_DefColorH, 0, strTmp);   
      }
   }
   DXDReleaseDC(hdc, 86);


   if(Pause || m_iPacmanID == -1 || m_iFantomeID[0] == -1 ||
      m_iFantomeID[1] == -1 ||
      m_iFantomeID[2] == -1 ||
      m_iFantomeID[3] == -1    ) 
      return;
   
   ManageGame_S01();
   
   if(m_Fin && m_Vie<1)
      return;
   
   if(m_GameDessineBonnus)
      DessineBonus(m_pLabyrinthe->GetCase(13,14)!=CASE_VIDE);
   
   
   V2SPRITEFX VsfFX;
   ZeroMemory(&VsfFX, sizeof(V2SPRITEFX));
   VsfFX.lpClipRect = new RECT;
   VsfFX.lpClipRect->left = 0;
   VsfFX.lpClipRect->top = 0;
   VsfFX.lpClipRect->right = g_Global.GetScreenW();
   VsfFX.lpClipRect->bottom = g_Global.GetScreenH();
   
   
   for(i=0;i<4;i++)
      g_Var.vsfGame01[m_iFantomeID[i]].DrawSpriteN(m_iFantomeXP[i],m_iFantomeYP[i],&VsfFX);
   
   g_Var.vsfGame01[m_iPacmanID].DrawSpriteN(m_iPacmanXP,m_iPacmanYP,&VsfFX);
   
   
   delete VsfFX.lpClipRect;
   
   
   if(PacmanCanMove(AttenteX,AttenteY))
   {
      PacDx=AttenteX;
      PacDy=AttenteY;
      AttenteX=0;
      AttenteY=0;
   };
   
   TestCollisions();
   
   
   
   if( (first)&&(!deja) )
   {
      bool tmp = Pause;
      Pause=true;
      deja = true;
      first = false;
      Pause = tmp;
	};
}

void V3_PacmanDlg::Hide()
{
   TuerPacman();
   m_Fin = true;
   
   m_TimerTools.KillNMTimer(-1);
   BoxUI::Hide();
}

int V3_PacmanDlg::TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise)
{
   int dwSize = (int)pFont->GetFLen(hdc,pStr);
   return((dwBoxZise-dwSize)/2);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool V3_PacmanDlg::VKeyInput(VKey vKey)
{
   switch( vKey.GetKey() )
   {
      case VK_UP:
         {
            AttenteX=0;
		      AttenteY=-1;
         }
      break;
      case VK_DOWN:
         {
            AttenteX=0;
		      AttenteY=1;
         }
      break;
      case VK_LEFT:
         {
            AttenteX=-1;
		      AttenteY=0;
         }
      break;
      case VK_RIGHT:
         {
            AttenteX=1;
		      AttenteY=0;
         }
      break;
      case VK_PAUSE:
         Pause=!Pause;
      break;
   }

   vKey.ResetAllKeyState();

   
   return false;
}



void V3_PacmanDlg::PlayButtonEvent::LeftClicked( void )
{
   V3_PacmanDlg *me = static_cast< V3_PacmanDlg * >( GetUI() );
   
   me->NouveauPacman(true);
}

void V3_PacmanDlg::PauseButtonEvent::LeftClicked( void )
{
   V3_PacmanDlg *me = static_cast< V3_PacmanDlg * >( GetUI() );
   
   
   me->Pause=!me->Pause;
   
}




void V3_PacmanDlg::StartGame()
{
   PacOpen->Play( TRUE );
   Show();
   m_Niveau =1;
   m_Vie    =0;
   m_Score  =1;
   m_Fin    =true;
   
}





void V3_PacmanDlg::OnTimer(unsigned int nIDEvent)
{

   if(Pause)
      return;

   static int cptcpt=0;

	int Xcase,Ycase,Xd,Yd,dX,dY,dd;
	int dx=0,dy=0;

   Xcase = (m_RCp.right-m_RCp.left) /(MAXX);
   Ycase = (m_RCp.bottom-m_RCp.top)/ ( MAXY);
   
   if(Xcase>Ycase) 
      Xcase=Ycase;
   else 
      Ycase=Xcase;
   
   
   dX = Xcase/2;
   dY = Ycase/2;
   
   Xd =m_RCp.left+((m_RCp.right -m_RCp.left-(2*(MAXX-1)+2)*dX)/2);
	Yd =m_RCp.top +((m_RCp.bottom-m_RCp.top -(2*(MAXY-1)+2)*dY)/2);

	dd = (2*dX)/3;

	
	


   if(nIDEvent==10)
   {
      if(m_pLabyrinthe->GetCase(13,14)!=CASE_VIDE)
         m_pLabyrinthe->SetCase(13,14,CASE_VIDE);
      
      m_GameDessineBonnus = true;
      m_TimerTools.KillNMTimer(10);
   }
   else if( (nIDEvent==9)&&(m_Fin) )
   {
      if((cptcpt>=4))
      {
         cptcpt=0;
         m_TimerTools.KillNMTimer(9);
         NouveauPacman(false);
         return;
      };
      if(CompteurFin<=1)
      {
         cptcpt=0;


         for(int i=0;i<4;i++)
         {
            m_iFantomeID[i]  = m_pLabyrinthe->GetIdBitmap(m_pFantome[i],m_pPacman,true,m_Fin,CompteurFin,m_Signal);
            m_iFantomeID[i]  = m_pFantome[i]->Dessine(m_pFantome[i]->GetLastX(),m_pFantome[i]->GetLastY(),true,false,m_RCp,NULL,m_iFantomeID[i],m_iFantomeXP[i],m_iFantomeYP[i]);
         }

         CompteurFin++;
         m_iPacmanID = m_pLabyrinthe->GetIdBitmap(m_pPacman,m_pPacman,true,m_Fin,CompteurFin,m_Signal);
         m_iPacmanID = m_pPacman->Dessine(m_pPacman->GetLastX(),m_pPacman->GetLastY(),true,false,m_RCp,NULL,m_iPacmanID,m_iPacmanXP,m_iPacmanYP);


         

         return;
      };
      if((CompteurFin<=4))
      {

         CompteurFin++;
         m_iPacmanID = m_pLabyrinthe->GetIdBitmap(m_pPacman,m_pPacman,true,m_Fin,CompteurFin,m_Signal);
         m_iPacmanID = m_pPacman->Dessine(m_pPacman->GetLastX(),m_pPacman->GetLastY(),true,false,m_RCp,NULL,m_iPacmanID,m_iPacmanXP,m_iPacmanYP);

         return;
      };
      if(CompteurFin==5)
      {

         m_iPacmanID = m_pLabyrinthe->GetIdBitmap(m_pPacman,m_pPacman,false,m_Fin,CompteurFin,m_Signal);
         m_iPacmanID = m_pPacman->Dessine(m_pPacman->GetLastX(),m_pPacman->GetLastY(),false,false,m_RCp,NULL,m_iPacmanID,m_iPacmanXP,m_iPacmanYP);


         CompteurFin=6;
         cptcpt++;
         return;
      };
      if(CompteurFin==6)
      {
         m_iPacmanID = m_pLabyrinthe->GetIdBitmap(m_pPacman,m_pPacman,false,m_Fin,CompteurFin,m_Signal);
         m_iPacmanID = m_pPacman->Dessine(m_pPacman->GetLastX(),m_pPacman->GetLastY(),true,false,m_RCp,NULL,m_iPacmanID,m_iPacmanXP,m_iPacmanYP);

         CompteurFin=5;
         cptcpt++;
         return;
      };
   }
   else if((nIDEvent==8)&&(m_pPacman->Puissance==FORT))
	{
		if( (time8<=20))
		{
         for(int i=0;i<4;i++)
         {
			   if(m_pFantome[i]->Atteint)
			   {
				   m_pFantome[i]->sensibilite=0;
				   m_pFantome[i]->dureepoursuite=0;
			   };
         }

		};

		if( (time8>20)&&(time8<30))
		{

         if(m_Signal==0) 
            m_Signal=1;
			else 
            m_Signal=0;
         for(int i=0;i<4;i++)
         {
            m_iFantomeID[i]  = m_pLabyrinthe->GetIdBitmap(m_pFantome[i],m_pPacman,true,m_Fin,CompteurFin,m_Signal);
            m_iFantomeID[i]  = m_pFantome[i]->Dessine(m_pFantome[i]->GetLastX(),m_pFantome[i]->GetLastY(),true,false,m_RCp,NULL,m_iFantomeID[i],m_iFantomeXP[i],m_iFantomeYP[i]);
         }
		};

		time8++;
		if(time8>=30)
		{
			m_Signal=0;
			time8=0;
			m_pPacman->Puissance=FAIBLE;
		};
	}
   else if(nIDEvent==3)
   {
      int ttt=25;

      for(int i=0;i<4;i++)
      {
         if(m_pFantome[i]->candeplace) 
            DeplaceFantome(m_pFantome[i]);
         if (m_pFantome[i]->sensibilite<=ttt*(m_pFantome[i]->Identifiant-30))
            m_pFantome[i]->sensibilite++;
      }
	}
   else if(nIDEvent==7 || nIDEvent==6 || nIDEvent==5 || nIDEvent==4)
   {
      CFantome *sp;
      int i=0;
      if(nIDEvent == 7)
         i = 0;
      else if(nIDEvent == 6)
         i = 1;
      else if(nIDEvent == 5)
         i = 2;
      else if(nIDEvent == 4)
         i = 3;
      sp = m_pFantome[i];

      int iCpt = 3-i;


      if((sp->candeplace==false))
      {
         m_iCptA[iCpt]++;
         if(sp->TargetX==sp->GetPosX())
         {
            if(sp->TargetY>sp->GetPosY()) dy=1;
            else dy=-1;
         };
         
         if(sp->TargetY==sp->GetPosY())
         {
            if(sp->TargetX>sp->GetPosX()) dx=1;
            else dx=-1;
         };


         m_iFantomeID[i]  = m_pLabyrinthe->GetIdBitmap(sp,m_pPacman,true,m_Fin,CompteurFin,m_Signal);
         m_iFantomeID[i]  = sp->Dessine(sp->GetLastX()+m_iCptA[iCpt]*dx*dd,sp->GetLastY()+m_iCptA[iCpt]*dy*dd,true,false,m_RCp,NULL,m_iFantomeID[i],m_iFantomeXP[i],m_iFantomeYP[i]);

         
         if(m_iCptA[iCpt]==2)
         {

            sp->SetPosX(sp->TargetX);
            sp->SetPosY(sp->TargetY);
            sp->TargetX=0;
            sp->TargetY=0;

            m_iFantomeID[i]  = m_pLabyrinthe->GetIdBitmap(sp,m_pPacman,true,m_Fin,CompteurFin,m_Signal);
            m_iFantomeID[i]  = sp->Dessine(0,0,true,true,m_RCp,NULL,m_iFantomeID[i],m_iFantomeXP[i],m_iFantomeYP[i]);

            sp->candeplace=true;
            m_iCptA[iCpt]=0;
            m_TimerTools.KillNMTimer(nIDEvent);
         };
      };
	}
   else if((nIDEvent==2)&&(m_pPacman!=NULL))
	{
		if((PacmanTargetX==17)&&(PacmanTargetY==25 || PacmanTargetY==0)&&DepParticulier)
		{
         m_pPacman->SetPosX(PacmanTargetX);
			m_pPacman->SetPosY(PacmanTargetY);


         m_iPacmanID = m_pLabyrinthe->GetIdBitmap(m_pPacman,m_pPacman,true,m_Fin,CompteurFin,m_Signal);
         m_iPacmanID = m_pPacman->Dessine(0,0,true,true,m_RCp,NULL,m_iPacmanID,m_iPacmanXP,m_iPacmanYP);

			PacmanTargetX=0;
			PacmanTargetY=0;
			PacmanDeplace=true;
			PacCpt=0;
			m_TimerTools.KillNMTimer(2);
			DepParticulier=false;
			return;
		};

		PacCpt++;
		int dx=0,dy=0;

		if(PacmanTargetX==m_pPacman->GetPosX())
		{
			if(PacmanTargetY>m_pPacman->GetPosY()) dy=1;
			else dy=-1;
		};

		if(PacmanTargetY==m_pPacman->GetPosY())
		{
			if(PacmanTargetX>m_pPacman->GetPosX()) dx=1;
			else dx=-1;
		};




      m_iPacmanID = m_pLabyrinthe->GetIdBitmap(m_pPacman,m_pPacman,true,m_Fin,CompteurFin,m_Signal);
      m_iPacmanID = m_pPacman->Dessine(m_pPacman->GetLastX()+PacCpt*dx*dd,m_pPacman->GetLastY()+PacCpt*dy*dd,true,false,m_RCp,NULL,m_iPacmanID,m_iPacmanXP,m_iPacmanYP);


		if(PacCpt==2)
		{
			m_pPacman->SetPosX(PacmanTargetX);
			m_pPacman->SetPosY(PacmanTargetY);

         m_iPacmanID = m_pLabyrinthe->GetIdBitmap(m_pPacman,m_pPacman,true,m_Fin,CompteurFin,m_Signal);
         m_iPacmanID = m_pPacman->Dessine(0,0,true,true,m_RCp,NULL,m_iPacmanID,m_iPacmanXP,m_iPacmanYP);
       

 			PacmanTargetX=0;
   		PacmanTargetY=0;
			PacmanDeplace=true;
			PacCpt=0;
			m_TimerTools.KillNMTimer(2);
		};

	}
   else if(nIDEvent==1)
	{
		
		bool avant1=m_bgob1;
		bool avant2=m_bgob2;
		bool avant3=m_bgob3;
		bool avant4=m_bgob4;

		m_bgob1= (m_pLabyrinthe->GetCase(9,1)==PAC_GOB);
		m_bgob2= (m_pLabyrinthe->GetCase(25,1)==PAC_GOB);
		m_bgob3= (m_pLabyrinthe->GetCase(9,24)==PAC_GOB);
		m_bgob4= (m_pLabyrinthe->GetCase(25,24)==PAC_GOB);

      HDC hdc;
      DXDGetDC(&hdc, 68);
		if(m_bgob1) 
         m_pLabyrinthe->DessineGob(9,1,m_bgob1,hdc);
		else if(avant1) 
         m_pLabyrinthe->DessineGob(9,1,m_bgob1,hdc);

		if(m_bgob2) 
         m_pLabyrinthe->DessineGob(25,1,m_bgob2,hdc);
		else if(avant2) 
         m_pLabyrinthe->DessineGob(25,1,m_bgob2,hdc);

		if(m_bgob3) 
         m_pLabyrinthe->DessineGob(9,24,m_bgob3,hdc);
		else if(avant3) 
         m_pLabyrinthe->DessineGob(9,24,m_bgob3,hdc);

		if(m_bgob4) 
         m_pLabyrinthe->DessineGob(25,24,m_bgob4,hdc);
		else if(avant4) 
         m_pLabyrinthe->DessineGob(25,24,m_bgob4,hdc);
      DXDReleaseDC(hdc, 68);

		m_pLabyrinthe->m_ChangeGob = !m_pLabyrinthe->m_ChangeGob;

		if(PacmanDeplace==true)
		{


         m_iPacmanID = m_pLabyrinthe->GetIdBitmap(m_pPacman,m_pPacman,true,m_Fin,CompteurFin,m_Signal);
         m_iPacmanID = m_pPacman->Dessine(m_pPacman->GetLastX(),m_pPacman->GetLastY(),true,false,m_RCp,NULL,m_iPacmanID,m_iPacmanXP,m_iPacmanYP);


         for(int i=0;i<4;i++)
         {
            m_iFantomeID[i]  = m_pLabyrinthe->GetIdBitmap(m_pFantome[i],m_pPacman,true,m_Fin,CompteurFin,m_Signal);
            m_iFantomeID[i]  = m_pFantome[i]->Dessine(m_pFantome[i]->GetLastX(),m_pFantome[i]->GetLastY(),true,false,m_RCp,NULL,m_iFantomeID[i],m_iFantomeXP[i],m_iFantomeYP[i]);
         }

		};
	};
	DeplacePacman(PacDx,PacDy);
}

int V3_PacmanDlg::TimerCallback(void* pThis,unsigned int dwCode)
{
   V3_PacmanDlg *me = (V3_PacmanDlg*)pThis;
   
   me->OnTimer(dwCode);
   return 0;
}





int V3_PacmanDlg::NbPointsActuel()
{
   int res=0;
   for(int i=0;i<MAXX;i++)
   {
      for(int j=0;j<MAXY;j++)
      {
         if(m_pLabyrinthe->GetCase(i,j)==PAC_POINT) 
            res++;
      }
   }
   return res;
}

bool V3_PacmanDlg::PlusDePoints()
{
   int i,j;
   
   for(i=0;i<MAXX;i++)
   {
      for(j=0;j<MAXY;j++)
      {
         if(m_pLabyrinthe->GetCase(i,j)==PAC_POINT)
            return false;
      };
   }
   return true;
}

int V3_PacmanDlg::ValeurAbsolue(int a)
{
   if(a>=0) 
      return a;
   else 
      return (-a);
}

bool V3_PacmanDlg::Collision(CFantome *sp)
{
   int spX,spY;
   int pcX,pcY;
   
   spX=sp->GetPosX();
   spY=sp->GetPosY();
   
   pcX=m_pPacman->GetPosX();
   pcY=m_pPacman->GetPosY();
   
   necessaire=1;
   if((ValeurAbsolue(spX-pcX)<=1)&&(ValeurAbsolue(spY-pcY)<1)) 
      return true;
   if((ValeurAbsolue(spX-pcX)<1)&&(ValeurAbsolue(spY-pcY)<=1)) 
      return true;
   
   necessaire=0;
   return false;
};

void V3_PacmanDlg::ChargerLesXetLesY()
{
   for(int i=0;i<4;i++)
   {
      if(m_pFantome[i]->TargetX!=0)
      {
         m_pLabyrinthe->m_pLesX[i]=m_pFantome[i]->TargetX;
         m_pLabyrinthe->m_pLesY[i]=m_pFantome[i]->TargetY;
      }
      else
      {
         m_pLabyrinthe->m_pLesX[i]=m_pFantome[i]->GetPosX();
         m_pLabyrinthe->m_pLesY[i]=m_pFantome[i]->GetPosY();
      }
   }
}

void V3_PacmanDlg::TuerPacman()
{
   if(m_Fin) 
      return;

   PacDie->Play( TRUE );

   CompteurFin=0;
   m_Fin=true;
   m_Vie--;
   m_TimerTools.KillNMTimer(1);
   m_TimerTools.KillNMTimer(2);
   m_TimerTools.KillNMTimer(3);
   m_TimerTools.KillNMTimer(4);
   m_TimerTools.KillNMTimer(5);
   m_TimerTools.KillNMTimer(6);
   m_TimerTools.KillNMTimer(7);
   m_TimerTools.KillNMTimer(8);
   m_TimerTools.KillNMTimer(10);
   if(m_Vie >0)
      m_TimerTools.SetNMTimer(9,200,TimerCallback,this);
}


bool V3_PacmanDlg::PacmanCanMove(int dx, int dy)
{
   if((dx==0)&&(dy==0)) 
      return false;
   if((dx!=0)&&(dy!=0)) 
      return false;
   
   int i,j;
   i=dx+m_pPacman->GetPosX();
   j=dy+m_pPacman->GetPosY();
   
   if(!PacmanDeplace)
   {
      i=dx+PacmanTargetX;
      j=dy+PacmanTargetY;
   };
   
   if((i<0)||(i>=MAXX)) 
      return false;
   if((j<0)||(j>=MAXY)) 
      return false;
   
   if(m_pLabyrinthe->GetCase(i,j)==COIN_GAUCHE_HAUT) return false;
   if(m_pLabyrinthe->GetCase(i,j)==COIN_GAUCHE_BAS)  return false;
   if(m_pLabyrinthe->GetCase(i,j)==COIN_DROIT_HAUT)  return false;
   if(m_pLabyrinthe->GetCase(i,j)==COIN_DROIT_BAS)   return false;
   if(m_pLabyrinthe->GetCase(i,j)==MUR_VERTICAL)     return false;
   if(m_pLabyrinthe->GetCase(i,j)==MUR_HORIZONTAL)   return false;
   if(m_pLabyrinthe->GetCase(i,j)==MUR_PORTE)        return false;
   
   return true;
}

bool V3_PacmanDlg::CanMoveFantome(CSprite *sp, int dx, int dy)
{
   if((dx==0)&&(dy==0)) return false;
   if((dx!=0)&&(dy!=0)) return false;
   
   int i,j;
   i=dx+sp->GetPosX();
   j=dy+sp->GetPosY();
   
   
   if((i<0)||(i>=MAXX)) return false;
   if((j<0)||(j>=MAXY)) return false;
   
   if(m_pLabyrinthe->GetCase(i,j)==COIN_GAUCHE_HAUT) return false;
   if(m_pLabyrinthe->GetCase(i,j)==COIN_GAUCHE_BAS)  return false;
   if(m_pLabyrinthe->GetCase(i,j)==COIN_DROIT_HAUT)  return false;
   if(m_pLabyrinthe->GetCase(i,j)==COIN_DROIT_BAS)   return false;
   if(m_pLabyrinthe->GetCase(i,j)==MUR_VERTICAL)     return false;
   if(m_pLabyrinthe->GetCase(i,j)==MUR_HORIZONTAL)   return false;
   if(m_pLabyrinthe->GetCase(i,j)==MUR_PORTE)        return false;
   
   return true;
}

void V3_PacmanDlg::TestCollisions()
{
   if(m_pPacman->Vivant)
   {
      if(m_pPacman->Puissance==FORT)
      {
         for(int i=0;i<4;i++)
         {
            if( (Collision(m_pFantome[i]))&&(m_pFantome[i]->Atteint)&&(m_pFantome[i]->Vivant) )
            {
               PacEatGhost->Play( TRUE );

               m_iFantomeID[i]  = m_pLabyrinthe->GetIdBitmap(m_pFantome[i],m_pPacman,false,m_Fin,CompteurFin,m_Signal);
               m_iFantomeID[i]  = m_pFantome[i]->Dessine(m_pFantome[i]->GetLastX(),m_pFantome[i]->GetLastY(),false,false,m_RCp,NULL,m_iFantomeID[i],m_iFantomeXP[i],m_iFantomeYP[i]);

               m_iPacmanID  = m_pLabyrinthe->GetIdBitmap(m_pPacman,m_pPacman,true,m_Fin,CompteurFin,m_Signal);
               m_iPacmanID  = m_pPacman->Dessine(m_pPacman->GetLastX(),m_pPacman->GetLastY(),true,false,m_RCp,NULL,m_iPacmanID,m_iPacmanXP,m_iPacmanYP);

               m_pFantome[i]->Vivant=false;
               m_Score=m_Score+m_PrixFantome;
               m_PrixFantome=2*m_PrixFantome;
            }
            else if( (Collision(m_pFantome[i]))&&(m_pFantome[i]->Vivant) ) 
               TuerPacman();
         }
      }
      else
      {
         for(int i=0;i<4;i++)
         {
            if( (Collision(m_pFantome[i]))&&(m_pFantome[i]->Vivant) )
               TuerPacman();
         }
      };
   };
}





void V3_PacmanDlg::NouveauPacman(bool bForce)
{
   if(bForce)
   {
      m_Niveau =1;
      m_Vie    =6;
      m_Score  =1;
      m_Fin    =false;
      Pause    =false;
      m_TimerTools.KillNMTimer(9);
      m_Signal=0;
      DejaDessiner=false;
      time8=0;

      if(m_pLabyrinthe)
         delete m_pLabyrinthe;
      
      m_pLabyrinthe    = new CPathFinder(m_RCp);

      CompteurFin=0;
      m_Fin=false;
      
      if(m_pPacman)
         delete m_pPacman;
      m_pPacman = new CPacman();
      
      PacmanDeplace = true;
      PacmanTargetY = EN_DEHORS;
      PacmanTargetX = EN_DEHORS;
      PacCpt        =0;
      PacDx         =0;
      PacDy         =-1;
      AttenteX      =0;
      AttenteY      =0;
      DepParticulier= false;
      m_GameDessineBonnus = false;
      m_pLabyrinthe->SetCase(m_pPacman->GetPosX(),m_pPacman->GetPosY(),CASE_VIDE);
      m_pLabyrinthe->SetCase(13,14,CASE_VIDE);
      
      for(int i=0,iv=3;i<4;i++,iv--)
      {
         if(m_pFantome[i])
            delete m_pFantome[i];
         m_pFantome[i]  = new CFantome(iv);
      }
      
      m_TimerTools.SetNMTimer(8,600,TimerCallback,this);
      m_TimerTools.SetNMTimer(1,150,TimerCallback,this);
      m_TimerTools.SetNMTimer(3,125,TimerCallback,this);
      return;
   }

   if(m_Vie>0)
   {
      m_TimerTools.KillNMTimer(9);
      m_Signal=0;
      DejaDessiner=false;
      time8=0;
      if(!m_Fin)
      {
         if(m_pLabyrinthe)
            delete m_pLabyrinthe;
         
         m_pLabyrinthe    = new CPathFinder(m_RCp);
      };
      CompteurFin=0;
      m_Fin=false;

      if(m_pPacman)
         delete m_pPacman;
      m_pPacman = new CPacman();

      PacmanDeplace = true;
      PacmanTargetY = EN_DEHORS;
      PacmanTargetX = EN_DEHORS;
      PacCpt        =0;
      PacDx         =0;
      PacDy         =-1;
      AttenteX      =0;
      AttenteY      =0;
      DepParticulier= false;
      m_GameDessineBonnus = false;
      m_pLabyrinthe->SetCase(m_pPacman->GetPosX(),m_pPacman->GetPosY(),CASE_VIDE);
      m_pLabyrinthe->SetCase(13,14,CASE_VIDE);

      for(int i=0,iv=3;i<4;i++,iv--)
      {
         if(m_pFantome[i])
            delete m_pFantome[i];
         m_pFantome[i]  = new CFantome(iv);
      }
      
      m_TimerTools.SetNMTimer(8,600,TimerCallback,this);
      m_TimerTools.SetNMTimer(1,150,TimerCallback,this);
      m_TimerTools.SetNMTimer(3,125,TimerCallback,this);
   }
}


void V3_PacmanDlg::CreeBonus()
{
   int LesBonus[6];
   int i,bonus;
   
   LesBonus[0]=BONUS1;
   for(i=1;i<6;i++)
      LesBonus[i]=LesBonus[i-1]+1;
   
   if(m_Niveau<=6) 
      bonus=LesBonus[m_Niveau-1];
   else 
      bonus=BONUS6;
   
   m_pLabyrinthe->SetCase(13,14,bonus);
   DessineBonus(true);
   m_GameDessineBonnus = true;
}



void V3_PacmanDlg::DessineBonus(bool comment)
{
   if(m_pLabyrinthe->GetCase(13,14)==PAC_POINT) 
      return;
   
   
   int Xcase,Ycase,Xd,Yd,dX,dY;
   
   int bonus;


   Xcase = (m_RCp.right-m_RCp.left) /(MAXX);
   Ycase = (m_RCp.bottom-m_RCp.top)/ ( MAXY);
   
   if(Xcase>Ycase) 
      Xcase=Ycase;
   else 
      Ycase=Xcase;
   
   
   dX = Xcase/2;
   dY = Ycase/2;
   
   Xd =m_RCp.left+((m_RCp.right -m_RCp.left-(2*(MAXX-1)+2)*dX)/2);
	Yd =m_RCp.top +((m_RCp.bottom-m_RCp.top -(2*(MAXY-1)+2)*dY)/2);
   
   bonus=m_pLabyrinthe->GetCase(13,14);
   
   int iImageID = 3; //image transparente...
   if(comment) 
   {
      switch(bonus)
      {
         case BONUS1:
            iImageID = 24;
         break;
         case BONUS2:
            iImageID = 25;
         break;
         case BONUS3:
            iImageID = 26;
         break;
         case BONUS4:
            iImageID = 27;
         break;
         case BONUS5:
            iImageID = 28;
         break;
         case BONUS6:
            iImageID = 29;
         break;
         default:
            iImageID = 6;
         break;
      };
   }
   
   
   int PosX=13;
   int PosY=14;


   V2SPRITEFX VsfFX;
   ZeroMemory(&VsfFX, sizeof(V2SPRITEFX));
   VsfFX.lpClipRect = new RECT;
   VsfFX.lpClipRect->left = 0;
   VsfFX.lpClipRect->top = 0;
   VsfFX.lpClipRect->right = g_Global.GetScreenW();
   VsfFX.lpClipRect->bottom = g_Global.GetScreenH();
   
   
   g_Var.vsfGame01[iImageID].DrawSpriteN(Xd+(2*PosX+1)*dX-dX-7,Yd+(2*PosY+1)*dY-dY-7,&VsfFX);
   
   delete VsfFX.lpClipRect;
}

void V3_PacmanDlg::DeplaceFantome(CFantome *sp)
{
	if(!sp->candeplace) 
      return;

	TestCollisions();
	
	ChargerLesXetLesY();

	if(sp->Vivant)
	{
	   if( (sp->sensibilite<=25*(sp->Identifiant-30)) )
	   {
		   if(m_pPacman->Puissance==FAIBLE)sp->Atteint=false;
		   DeplaceFantomeHasard(sp);
	   }
	   else if((sp->GetPosX()!=m_pPacman->GetPosX())||(sp->GetPosY()!=m_pPacman->GetPosY()))
	   {
		   if(m_pPacman->Puissance==FAIBLE)sp->Atteint=true;
		   if(105-sp->dureepoursuite/7>=10) 
			   sp->dureepoursuite++;

		   m_pLabyrinthe->ConstruireChemin(sp->GetPosX(),sp->GetPosY(),m_pPacman->GetPosX(),m_pPacman->GetPosY(),false);
		   
		   if(m_pLabyrinthe->m_pLaCaseSuivante!=NULL)
		   {
			   bool oui=true;
			   for(int k=0;k<4;k++)
			   {
				   if (oui)
					   if( (m_pLabyrinthe->m_pLesX[k]==m_pLabyrinthe->m_pLaCaseSuivante->m_x)&&(m_pLabyrinthe->m_pLesY[k]==m_pLabyrinthe->m_pLaCaseSuivante->m_y) )
						   oui=false;

			   };
			   if(!oui)
			   {
				   DeplaceFantomeHasard(sp);
				   if( (105-sp->dureepoursuite/7<=8) ) 
				   {
					   sp->dureepoursuite=0;
					   sp->sensibilite=0;
				   };	
				   return;
			   };
			   sp->candeplace=false;
			   sp->TargetX=m_pLabyrinthe->m_pLaCaseSuivante->m_x;
			   sp->TargetY=m_pLabyrinthe->m_pLaCaseSuivante->m_y;
			   int niv=m_Niveau;
			   if(m_Niveau>=10) m_Niveau=8;
			   m_TimerTools.SetNMTimer(38-sp->Identifiant,105-sp->dureepoursuite/7>10 ? 105-sp->dureepoursuite/7-m_Niveau : 10,TimerCallback,this);
			   m_Niveau=niv;
			   if( (105-sp->dureepoursuite/7<=8) ) 
			   {
				   sp->dureepoursuite=0;
				   sp->sensibilite=0;
			   };	
		   }
		   else
		   {
			   DeplaceFantomeHasard(sp);
		   };
	   };
	}
	else 
	{
		if( (sp->GetPosX()>14) && (sp->GetPosX()<19) && (sp->GetPosY()>10) && (sp->GetPosY()<15) )
		{
			sp->dureepoursuite=0;
			sp->sensibilite=0;
			sp->Vivant=true;
			sp->Atteint=false;
			sp->candeplace=true;
			return;
		};

		int pcX,pcY;

		if(PacmanTargetX==0)
		{
			pcX=m_pPacman->GetPosX();
			pcY=m_pPacman->GetPosY();
		}
		else
		{
			pcX=PacmanTargetX;
			pcY=PacmanTargetY;
		};

		m_pLabyrinthe->ConstruireChemin(sp->GetPosX(),sp->GetPosY(),16,13,false);
		
		if(m_pLabyrinthe->m_pLaCaseSuivante!=NULL)
		{
			bool oui=true;
			for(int k=0;k<4;k++)
			{
				if( (m_pLabyrinthe->m_pLesX[k]==m_pLabyrinthe->m_pLaCaseSuivante->m_x)&&(m_pLabyrinthe->m_pLesY[k]==m_pLabyrinthe->m_pLaCaseSuivante->m_y) )
						oui=false;
			};
			if( (pcX==m_pLabyrinthe->m_pLaCaseSuivante->m_x)&&(pcY==m_pLabyrinthe->m_pLaCaseSuivante->m_y) )
						oui=false;
			if(!oui)
			{
				DeplaceFantomeHasard(sp);
				return;
			};
			sp->candeplace=false;
			sp->TargetX=m_pLabyrinthe->m_pLaCaseSuivante->m_x;
			sp->TargetY=m_pLabyrinthe->m_pLaCaseSuivante->m_y;
			m_TimerTools.SetNMTimer(38-sp->Identifiant,35,TimerCallback,this);
		}
		else
		{
			DeplaceFantomeHasard(sp);
		};
	};
}

void V3_PacmanDlg::DeplaceFantomeHasard(CFantome *sp)
{
   int dx,dy,i,pcX,pcY;
   dx=0;
   dy=0;
   int cpt=0;
   srand( (unsigned)time( NULL ) );
   
   bool oui=false;
   
   ChargerLesXetLesY();
   
   pcX=m_pPacman->GetPosX();
   pcY=m_pPacman->GetPosY();
   
   TestCollisions();
   while( ((!CanMoveFantome(sp,dx,dy))||(!oui)) && (!m_Fin))
   {
      cpt++;
      if(cpt>=500) 
      {
         cpt=0;
         return;
      }
      i=rand();
      if((i%2)==0) dx=-1;
      else dx=1;
      i=rand();
      if((i%2)==0) dy=-1;
      else dy=1;
      i=rand();
      if((i%2)==0) dx=0;
      else dy=0;
      if((sp->GetPosX()==17)&&(sp->GetPosY()==0)&&(dx==0)&&(dy==-1))
      {
         int i=0;
         if(sp == m_pFantome[0])
            i=0;
         else if(sp == m_pFantome[1])
            i=1;
         else if(sp == m_pFantome[2])
            i=2;
         else if(sp == m_pFantome[3])
            i=3;


         sp->SetPosY(25);
         m_iFantomeID[i]  = m_pLabyrinthe->GetIdBitmap(m_pFantome[i],m_pPacman,true,m_Fin,CompteurFin,m_Signal);
         m_iFantomeID[i]  = m_pFantome[i]->Dessine(0,0,true,true,m_RCp,NULL,m_iFantomeID[i],m_iFantomeXP[i],m_iFantomeYP[i]);

        
         return;
      };


      if((sp->GetPosX()==17)&&(sp->GetPosY()==25)&&(dx==0)&&(dy==1))
      {
          int i=0;
         if(sp == m_pFantome[0])
            i=0;
         else if(sp == m_pFantome[1])
            i=1;
         else if(sp == m_pFantome[2])
            i=2;
         else if(sp == m_pFantome[3])
            i=3;


         sp->SetPosY(0);
         m_iFantomeID[i]  = m_pLabyrinthe->GetIdBitmap(m_pFantome[i],m_pPacman,true,m_Fin,CompteurFin,m_Signal);
         m_iFantomeID[i]  = m_pFantome[i]->Dessine(0,0,true,true,m_RCp,NULL,m_iFantomeID[i],m_iFantomeXP[i],m_iFantomeYP[i]);

         return;
      };
      oui=true;
      for(int k=0;k<4;k++)
      {
         if( (m_pLabyrinthe->m_pLesX[k]==dx+sp->GetPosX())&&(m_pLabyrinthe->m_pLesY[k]==dy+sp->GetPosY()) )
            oui=false;
      };
      if((dx+sp->GetPosX()==pcX)&&(dy+sp->GetPosY()==pcY))
         oui=false;
   };
   sp->candeplace=false;
   sp->TargetX=dx+sp->GetPosX();
   sp->TargetY=dy+sp->GetPosY();
   m_TimerTools.SetNMTimer(38-sp->Identifiant,77-2*m_Niveau,TimerCallback,this);
}


void V3_PacmanDlg::DeplacePacman(int dx, int dy)
{
	int i,j,timer=105;

	if(m_pPacman->Puissance==FORT) 
      timer = 75+2*m_Niveau;
	if(m_pPacman->Puissance==FAIBLE) 
      timer = 90+2*m_Niveau;

	if((dx==0)&&(dy==0)) return;
	if((dx!=0)&&(dy!=0)) return;
	if((m_pPacman!=NULL)&&(PacmanDeplace))
		if(m_pPacman->Vivant)
		{
			if((m_pPacman->GetPosX()==17)&&(m_pPacman->GetPosY()==0)&&(dy==-1))
			{
				DepParticulier=true;
				PacmanTargetX=i=17;
				PacmanTargetY=j=25;
				PacCpt       =0;
				PacmanDeplace=false;
				if(m_pLabyrinthe->GetCase(i,j)==PAC_POINT)
				{
					m_Score=m_Score+1;
					m_pLabyrinthe->SetCase(i,j,CASE_VIDE);

               PacEat->Play( TRUE );
				};

				if(m_pLabyrinthe->GetCase(i,j)==PAC_GOB)
				{
               PacEatGob->Play( TRUE );
					m_Score=m_Score+6+2*m_Niveau;
					m_pLabyrinthe->SetCase(i,j,CASE_VIDE);
					m_pPacman->Puissance=FORT;
					m_PrixFantome=20;
					m_Signal=0;
					time8=0;

               for(int i=0;i<4;i++)
               {
					   if( (m_pFantome[i]->Atteint) || m_pFantome[i]->Atteignable())
					   {
						   m_pFantome[i]->Atteint=true;
						   m_pFantome[i]->sensibilite=0;
						   m_pFantome[i]->dureepoursuite=0;
					   };
               }
				};
				m_TimerTools.SetNMTimer(2,timer,TimerCallback,this);
				return;
			};

			if((m_pPacman->GetPosX()==17)&&(m_pPacman->GetPosY()==25)&&(dy==1))
			{
				DepParticulier=true;
				PacmanTargetX=i=17;
				PacmanTargetY=j=0;
				PacCpt       =0;
				PacmanDeplace=false;
				if(m_pLabyrinthe->GetCase(i,j)==PAC_POINT)
				{
					m_Score=m_Score+1;
					m_pLabyrinthe->SetCase(i,j,CASE_VIDE);

               PacEat->Play( TRUE );
				};

				if(m_pLabyrinthe->GetCase(i,j)==PAC_GOB)
				{
               PacEatGob->Play( TRUE );

					m_Score=m_Score+6+2*m_Niveau;
					m_pLabyrinthe->SetCase(i,j,CASE_VIDE);
					m_pPacman->Puissance=FORT;
					m_PrixFantome=20;
					m_Signal=0;
					time8=0;


               for(int i=0;i<4;i++)
               {
					   if( (m_pFantome[i]->Atteint) || m_pFantome[i]->Atteignable())
					   {
						   m_pFantome[i]->Atteint=true;
						   m_pFantome[i]->sensibilite=0;
						   m_pFantome[i]->dureepoursuite=0;
					   };
               }
				};
				m_TimerTools.SetNMTimer(2,timer,TimerCallback,this);
				return;
			};

			i=dx+m_pPacman->GetPosX();
			j=dy+m_pPacman->GetPosY();
			
			if((i<0)||(i>=MAXX)) return;
			if((j<0)||(j>=MAXY)) return;

			if(m_pLabyrinthe->GetCase(i,j)==COIN_GAUCHE_HAUT) return;
			if(m_pLabyrinthe->GetCase(i,j)==COIN_GAUCHE_BAS) return;
			if(m_pLabyrinthe->GetCase(i,j)==COIN_DROIT_HAUT) return;
			if(m_pLabyrinthe->GetCase(i,j)==COIN_DROIT_BAS) return;
			if(m_pLabyrinthe->GetCase(i,j)==MUR_VERTICAL) return;
			if(m_pLabyrinthe->GetCase(i,j)==MUR_HORIZONTAL) return;
			if(m_pLabyrinthe->GetCase(i,j)==MUR_PORTE) return;

			if(m_pLabyrinthe->GetCase(i,j)==PAC_POINT)
			{
				m_Score=m_Score+1;
				m_pLabyrinthe->SetCase(i,j,CASE_VIDE);
            PacEat->Play( TRUE );

			};

			if((m_pLabyrinthe->GetCase(i,j)!=PAC_POINT)&&(m_pLabyrinthe->GetCase(i,j)!=CASE_VIDE)&&(i==13)&&(j==14))
			{
            PacEatBonus->Play( TRUE );

            m_GameDessineBonnus = false;
				m_Score=m_Score+250*m_Niveau;
				m_pLabyrinthe->SetCase(i,j,CASE_VIDE);
			};
			

			if(m_pLabyrinthe->GetCase(i,j)==PAC_GOB)
			{
            PacEatGob->Play( TRUE );

            m_Score=m_Score+6+2*m_Niveau;
            m_pLabyrinthe->SetCase(i,j,CASE_VIDE);
            m_pPacman->Puissance=FORT;
            m_PrixFantome=20;
            m_Signal=0;
            time8=0;
            
            for(int i=0;i<4;i++)
            {
               if( (m_pFantome[i]->Atteint) || m_pFantome[i]->Atteignable())
               {
                  m_pFantome[i]->Atteint=true;
                  m_pFantome[i]->sensibilite=0;
                  m_pFantome[i]->dureepoursuite=0;
               };
            }
			};
			
			if(dx==1) m_pPacman->Direction=DROITE;
			if(dx==-1) m_pPacman->Direction=GAUCHE;
			if(dy==1) m_pPacman->Direction=BAS;
			if(dy==-1) m_pPacman->Direction=HAUT;

			PacmanTargetX=i;
			PacmanTargetY=j;
			PacCpt       =0;
			PacmanDeplace=false;
			m_TimerTools.SetNMTimer(2,timer,TimerCallback,this);
		};
}



void V3_PacmanDlg::ManageGame_S01()
{
   if(PlusDePoints()) 
   {
      DejaBonus=false;
      m_Fin=false;
      m_Niveau++;
      NouveauPacman(false);
      return;
   };
   
   if(NbPointsActuel()<=NbTotalPoint/2)
   {
      if(!DejaBonus)
      {
         DejaBonus=true;
         CreeBonus();
         m_TimerTools.SetNMTimer(10,8000,TimerCallback,this);
      };
   };
   
}