// V3_ArenePointsDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_ArenePointsDlg.h"
#include "RootBoxUI.h"
#include "V3_StatDlg.h"
#include "V3_ChatLogDlg.h"
#include "..\NMPlayVideo.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_ArenePointsDlg::V3_ArenePointsDlg():
Drag_Button(dragEvent)
{
   int i=0;

   m_chlastCode    = 0;
   m_chlastCodeCnt = 0;
   m_chProgressPC  = 0;

   m_lAreneID      = 0;
   m_lAreneType    = 0;


   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastArenePointXPos();
   m_iYPos = g_Global.GetDialogLastArenePointYPos();
   
 
   m_Background .SetSpriteId("V3_ArenePlayBack1",true,"V3_ArenePlayBack1Mask");

   
   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this ); 


   m_FlagBar.SetSpriteId("FlagTakeProgress");
   UpdateAllFramePos(m_iXPos,m_iYPos);

   AddChild( &m_Background);
   AddChild( &m_PointB);
   AddChild( &m_PointR);
   AddChild( &Drag_Button);

   RootBoxUI::GetInstance()->AddChild( this );
} 

V3_ArenePointsDlg::~V3_ArenePointsDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_ArenePointsDlg *V3_ArenePointsDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_ArenePointsDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_ArenePointsDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iXPos = g_Global.GetDialogLastArenePointXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastArenePointYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->Show();

   return 0; 
}

void V3_ArenePointsDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_ArenePointDlgW, iPosY+g_ArenePointDlgH ) );
   m_Background .SetPos( FramePos( iPosX, iPosY, iPosX+g_ArenePointDlgW, iPosY+g_ArenePointDlgH ) );
   Drag_Button .SetPos( FramePos( iPosX, iPosY, iPosX+g_ArenePointDlgW, iPosY+g_ArenePointDlgH ) );

   m_PointB.SetPos( FramePos( iPosX+42 , iPosY+14,iPosX+60, iPosY+26) );
   m_PointR.SetPos( FramePos( iPosX+199, iPosY+14,iPosX+219,iPosY+26) );
   m_Time  .SetPos( FramePos( iPosX+75 , iPosY+14,iPosX+185,iPosY+26) );

   m_FlagBar     .SetPos( FramePos( iPosX+72, iPosY+13,iPosX+72+118, iPosY+72+14) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_ArenePointsDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   /*
   if(!g_SaveGame.GetOptionsParam()->bDisplayMp3)
   {
      Hide();
      return;
   }
   */

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

   if(m_bDragDialog)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );

      int iNewPosX = m_iXPos + (mousePos.x-m_iXPosSD);
      int iNewPosY = m_iYPos + (mousePos.y-m_iYPosSD);
      
      if(iNewPosX < 0)
         iNewPosX = 0;

      if(iNewPosY < 0)
         iNewPosY = 0;
      
      if(iNewPosX+g_ArenePointDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_ArenePointDlgW;
      if(iNewPosY+g_ArenePointDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_ArenePointDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   char strPos[50];
   sprintf_s(strPos,50,"%d",m_iPointBleu);
   m_PointB.SetText(strPos,g_DefColorD);
   sprintf_s(strPos,50,"%d",m_iPointRed);
   m_PointR.SetText(strPos,g_DefColorD);

   if(m_chlastCode == 1 && m_chlastCodeCnt >0)
   {
      m_chlastCodeCnt--;
      sprintf_s(strPos,50,"%s",g_GUILocalString[385]);
      m_Time.SetText(strPos,g_DefColorD);
   }
   else
   {
      m_chlastCode = 0;
      m_chlastCodeCnt = 0;

      sprintf_s(strPos,50,"%d min %02d sec",m_iNbrMin,m_iNbrSec);
      m_Time.SetText(strPos,g_DefColorD);
   }

   

   GameUI::Draw( vsfFX );

   if(m_chProgressPC > 0)
      DrawSingleStatusItem( m_FlagBar, m_chProgressPC );
   m_Time.Draw(vsfFX);
}


void V3_ArenePointsDlg::Hide()
{ 
   BoxUI::Hide();
}



void V3_ArenePointsDlg::StartDragDlg( MousePos mousePos )
{
   if(!g_SaveGame.GetOptionsParam()->bGUIDynamic)
      return;
   
   if(!m_bDragDialog)
   {
      //si premier start drag, on prend la position de depart du drag
      //pour en calculer le deltat X et Y du deplacement de la fenetre...
      {
         
         m_bDragDialog = true;
         m_iXPosSD = mousePos.x;
         m_iYPosSD = mousePos.y;

         
         RootBoxUI::GetInstance()->SetDragUI((GameUI*)&Drag_Button);
         g_Global.SetDraggingDialog(true);
      }
   }
}

void V3_ArenePointsDlg::StopDragDlg()
{
   if(m_bDragDialog)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      m_iXPos = m_iXPos + (mousePos.x-m_iXPosSD);
      m_iYPos = m_iYPos + (mousePos.y-m_iYPosSD);
      
      if(m_iXPos < 0)
         m_iXPos = 0;
      if(m_iYPos < 0)
         m_iYPos = 0;
      
      if(m_iXPos+g_ArenePointDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_ArenePointDlgW;
      if(m_iYPos+g_ArenePointDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_ArenePointDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastArenePointXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      
      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_ArenePointsDlg::DragEvent::LeftClicked( void )
{
   V3_ArenePointsDlg *me = static_cast< V3_ArenePointsDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_ArenePointsDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_ArenePointsDlg *me = static_cast< V3_ArenePointsDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}

void V3_ArenePointsDlg::UpdateFlagProgress(char chCode ,char chPC)
{
   m_chlastCode    = chCode;
   m_chlastCodeCnt = 0;
   if(m_chlastCode == 1)
      m_chlastCodeCnt = 90;

   m_chProgressPC  = chPC;
}

void V3_ArenePointsDlg::UpdateInfoPlay(long lAreneID,long lAreneType,int iPtsB, int iPtsR, int iNbrMin, int iNbrSec)
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   m_lAreneID   = lAreneID;
   m_lAreneType = lAreneType;
   m_iPointBleu = iPtsB;
   m_iPointRed  = iPtsR;
   m_iNbrMin    = iNbrMin;
   m_iNbrSec    = iNbrSec;
   if(m_iNbrMin < 0)
      m_iNbrMin = 0;
   if(m_iNbrSec < 0)
      m_iNbrSec = 0;

   if(m_iNbrMin > 0 || m_iNbrSec > 0)
   {
      if(!IsShown())
         Display(NULL);
   }
   else
   {
      if(IsShown())
         Hide();
   }
}

void V3_ArenePointsDlg::DrawSingleStatusItem(GraphUI &fullBar,int percent)
{
   FramePos fullPos = fullBar.GetPos();
   FramePos emptyPos = fullBar.GetPos();

   // Create a clipping area according to the group member's hp.
   long newWidth = fullPos.Width() * ( 100 - percent ) / 100;

   RECT clipRect = { fullPos.x, fullPos.y, fullPos.maxx - newWidth, fullPos.maxy };

   V2SPRITEFX fx;
   memset( &fx, 0, sizeof( fx ) );

   fx.lpClipRect = &clipRect;
   fullBar.Draw( &fx );
}