// V3_TargetDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_TargetDlg.h"
#include "RootBoxUI.h"
#include "V3_StatDlg.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_TargetDlg::V3_TargetDlg():
Drag_Button(dragEvent)
{
   int i=0;


   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastTargetXPos();
   m_iYPos = g_Global.GetDialogLastTargetYPos();
   
 
   m_Background .SetSpriteId("V3_TargetBack2",true,"V3_TargetBack2Mask");
   m_PVBar.SetSpriteId("V3_PVBar");

   
   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this ); 

   char strTmp[10];
   for(i=0;i<iNBR_TARGET_IMAGE_MAX;i++)
   {
      sprintf_s(strTmp,10,"%04d",i);
      m_TargetImg[i].SetSpriteId(strTmp);
   }
     
   
  
   m_stPV.SetText(g_GUILocalString[5],g_DefColorD,g_Global.GetScreenW(),0,false,2);
      

   

   UpdateAllFramePos(m_iXPos,m_iYPos);

   AddChild( &m_Background);
   AddChild( &m_stPV);
   AddChild( &m_stNom);
   AddChild( &Drag_Button);
      
   RootBoxUI::GetInstance()->AddChild( this );
} 

V3_TargetDlg::~V3_TargetDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_TargetDlg *V3_TargetDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_TargetDlg instance;
    return &instance;
    
}  

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_TargetDlg::Display( void *null )
{
   if(!g_SaveGame.GetOptionsParam()->bDisplayTarget)
      return 0;
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iXPos = g_Global.GetDialogLastTargetXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastTargetYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->Show();

   return 0;
}

void V3_TargetDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_TargetDlgW, iPosY+g_TargetDlgH ) );
   m_Background .SetPos( FramePos( iPosX, iPosY, iPosX+g_TargetDlgW, iPosY+g_TargetDlgH ) );
   Drag_Button .SetPos( FramePos( iPosX, iPosY, iPosX+70, iPosY+g_TargetDlgH ) );
   m_stPV      .SetPos( FramePos( iPosX+179, iPosY+42,iPosX+187, iPosY+50) );
   m_PVBar     .SetPos( FramePos( iPosX+76 , iPosY+39,iPosX+175, iPosY+52) );
   m_stNom     .SetPos( FramePos( iPosX+73 , iPosY+16,iPosX+200, iPosY+30) );

   for(int i=0;i<iNBR_TARGET_IMAGE_MAX;i++)
      m_TargetImg[i].SetPos( FramePos( iPosX+12,iPosY+12 ,iPosX+67, iPosY+65) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_TargetDlg::Draw(V2SPRITEFX *vsfFX,TFCObject *pObject)
{
   if(!IsShown())
      return;

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
      
      if(iNewPosX+g_TargetDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_TargetDlgW;
      if(iNewPosY+g_TargetDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_TargetDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }


   m_stNom.SetText(m_strName,g_DefColorD,g_Global.GetScreenW(),0,false,2);
   m_stNom.SetCenter(false);

   GameUI::Draw( vsfFX );
 
   //Dessine els bar pard essus le back...
   //PV Bar
   DrawSingleStatusItem( m_PVBar, m_iPCHP );

   //char strTmp[100];
   //sprintf_s(strTmp,100,"Draw PC target %d/100\n",m_iPCHP);
   //OutputDebugString(strTmp);

   Objects.Lock(0);
   RECT rcClip;
   rcClip.left   = 0;
   rcClip.top    = 0;//iOffY;
   rcClip.right  = iOffX+100/*62*/;//iOffX+g_TargetDlgW;
   rcClip.bottom = iOffY+60;
   if(pObject->Type3D == 71)
      Objects.DrawPuppetClip( iOffX+20, iOffY+80,2 ,&rcClip,pObject); //3 ok \ 2 ==  de face
   else
   {
      if(pObject->Type3D >=0 && pObject->Type3D < iNBR_TARGET_IMAGE_MAX)
         m_TargetImg[pObject->Type3D].Draw(vsfFX);
      else
      {
         //m_TargetImg[4].Draw(vsfFX);
      }
      //on dessine juste limage fake de la target...
   }
   Objects.Unlock(0);
}


void V3_TargetDlg::Hide()
{ 
   BoxUI::Hide();
}



void V3_TargetDlg::StartDragDlg( MousePos mousePos )
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

void V3_TargetDlg::StopDragDlg()
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
      
      if(m_iXPos+g_TargetDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_TargetDlgW;
      if(m_iYPos+g_TargetDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_TargetDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastTargetXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      
      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_TargetDlg::DragEvent::LeftClicked( void )
{
   V3_TargetDlg *me = static_cast< V3_TargetDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_TargetDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_TargetDlg *me = static_cast< V3_TargetDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}



//  Draws a single item
void V3_TargetDlg::DrawSingleStatusItem(GraphUI &fullBar,int percent)
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



void V3_TargetDlg::UpdateInfo(char *pStrName, int iPCHP, WORD wLevel)
{
   if(pStrName)
      sprintf_s(m_strName,1024,"%s",pStrName);
   else
      m_strName[0] = 0x00;

   m_iPCHP = iPCHP;
   m_wLevel = wLevel;
}