// V3_Mp3Dlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_Mp3Dlg.h"
#include "RootBoxUI.h"
#include "V3_StatDlg.h"
#include "V3_ChatLogDlg.h"
#include "..\NMPlayVideo.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;


extern HANDLE  g_hMp3EventNext;
extern HANDLE  g_hMp3EventPrev;
extern HANDLE  g_hMp3EventPlay;
extern HANDLE  g_hMp3EventPaused;
extern HANDLE  g_hMp3EventVol;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_Mp3Dlg::V3_Mp3Dlg():
Drag_ButtonL(dragEvent),
Drag_ButtonR(dragEvent),
PlayButton( playButtonEvent ),
PauseButton( pauseButtonEvent ),
NextButton( nextButtonEvent ),
PrevButton( prevButtonEvent ),
RndButtonOn(rndButtonEvent),
RndButtonOf(rndButtonEvent),
VolPButton(volPButtonEvent),
VolMButton(volMButtonEvent)
{
   int i=0;


   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastMP3XPos();
   m_iYPos = g_Global.GetDialogLastMP3YPos();
   
 
   m_Background .SetSpriteId("V3_Mp3ABack",true,"V3_Mp3ABackMask");

   
   Drag_ButtonL.SetSpriteId   ( "" );
   Drag_ButtonL.SetDownUI     ( "" );
   Drag_ButtonL.SetHighlightUI( "" ); 
   Drag_ButtonR.SetSpriteId   ( "" );
   Drag_ButtonR.SetDownUI     ( "" );
   Drag_ButtonR.SetHighlightUI( "" );
   dragEvent.SetUI    ( this ); 


   PlayButton.SetSpriteId   ( "V3_Mp3A_PlayN",true,"V3_Mp3A_PlayNMask" );
   PlayButton.SetDownUI     ( "V3_Mp3A_PlayN",true,"V3_Mp3A_PlayNMask" );
   PlayButton.SetHighlightUI( "V3_Mp3A_PlayH",true,"V3_Mp3A_PlayNMask" );

   PauseButton.SetSpriteId   ( "V3_Mp3A_PauseN",true,"V3_Mp3A_PlayNMask" );
   PauseButton.SetDownUI     ( "V3_Mp3A_PauseN",true,"V3_Mp3A_PlayNMask" );
   PauseButton.SetHighlightUI( "V3_Mp3A_PauseH",true,"V3_Mp3A_PlayNMask" );

   
   PrevButton.SetSpriteId   ( "V3_mp3A_PrevN",true,"V3_mp3A_Mask" );
   PrevButton.SetDownUI     ( "V3_mp3A_PrevN",true,"V3_mp3A_Mask" );
   PrevButton.SetHighlightUI( "V3_mp3A_PrevH",true,"V3_mp3A_Mask" );
   
   NextButton.SetSpriteId   ( "V3_mp3A_NextN",true,"V3_mp3A_Mask" );
   NextButton.SetDownUI     ( "V3_mp3A_NextN",true,"V3_mp3A_Mask" );
   NextButton.SetHighlightUI( "V3_mp3A_NextH",true,"V3_mp3A_Mask" );

   RndButtonOn.SetSpriteId   ( "V3_Mp3A_RndN",true,"V3_mp3A_Mask" );
   RndButtonOn.SetDownUI     ( "V3_Mp3A_RndN",true,"V3_mp3A_Mask" );
   RndButtonOn.SetHighlightUI( "V3_Mp3A_RndH",true,"V3_mp3A_Mask" );

   RndButtonOf.SetSpriteId   ( "V3_Mp3A_RndOffN",true,"V3_mp3A_Mask" );
   RndButtonOf.SetDownUI     ( "V3_Mp3A_RndOffN",true,"V3_mp3A_Mask" );
   RndButtonOf.SetHighlightUI( "V3_Mp3A_RndH",true,"V3_mp3A_Mask" );

   VolPButton.SetSpriteId   ( "V3_mp3A_VolMoreN",true,"V3_mp3A_Mask" );
   VolPButton.SetDownUI     ( "V3_mp3A_VolMoreN",true,"V3_mp3A_Mask" );
   VolPButton.SetHighlightUI( "V3_mp3A_VolMoreH",true,"V3_mp3A_Mask" );

   VolMButton.SetSpriteId   ( "V3_mp3A_VolLessN",true,"V3_mp3A_Mask" );
   VolMButton.SetDownUI     ( "V3_mp3A_VolLessN",true,"V3_mp3A_Mask" );
   VolMButton.SetHighlightUI( "V3_mp3A_VolLessH",true,"V3_mp3A_Mask" );

   playButtonEvent .SetUI( this );
   pauseButtonEvent.SetUI( this );
   nextButtonEvent .SetUI( this );
   prevButtonEvent .SetUI( this );
   rndButtonEvent  .SetUI( this );
   volPButtonEvent .SetUI( this );
   volMButtonEvent .SetUI( this );
    

   UpdateAllFramePos(m_iXPos,m_iYPos);

   AddChild( &m_Background);
   AddChild( &PlayButton );
   AddChild( &PauseButton );
   AddChild( &PrevButton );
   AddChild( &NextButton );
   AddChild( &RndButtonOn );
   AddChild( &RndButtonOf );
   AddChild( &VolPButton );
   AddChild( &VolMButton );
   AddChild( &m_VolVal);
   AddChild( &Drag_ButtonL);
   AddChild( &Drag_ButtonR);


   RndButtonOn.Hide();
   RndButtonOf.Hide();

   m_iVolume10 = g_SaveGame.GetOptionsParam()->iMp3Volume/10;
   if(m_iVolume10 < 0)
      m_iVolume10 = 0;
   else if(m_iVolume10 >10)
      m_iVolume10 = 10;


   RootBoxUI::GetInstance()->AddChild( this );
} 

V3_Mp3Dlg::~V3_Mp3Dlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_Mp3Dlg *V3_Mp3Dlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_Mp3Dlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_Mp3Dlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iXPos = g_Global.GetDialogLastMP3XPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastMP3YPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->Show();

   return 0;
}

void V3_Mp3Dlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_MP3DlgW, iPosY+g_MP3DlgH ) );
   m_Background .SetPos( FramePos( iPosX, iPosY, iPosX+g_MP3DlgW, iPosY+g_MP3DlgH ) );
   Drag_ButtonL .SetPos( FramePos( iPosX, iPosY, iPosX+14, iPosY+g_MP3DlgH ) );
   Drag_ButtonR .SetPos( FramePos( iPosX+163, iPosY, iPosX+g_MP3DlgW, iPosY+g_MP3DlgH ) );

   PrevButton .SetPos( FramePos( iPosX+14, iPosY+9, iPosX+14+16, iPosY+9+16 ) );
   PlayButton .SetPos( FramePos( iPosX+33, iPosY+7, iPosX+33+20, iPosY+7+20 ) );
   PauseButton.SetPos( FramePos( iPosX+33, iPosY+7, iPosX+33+20, iPosY+7+20 ) );
   NextButton .SetPos( FramePos( iPosX+56, iPosY+9, iPosX+56+16, iPosY+9+16 ) );

   VolMButton .SetPos( FramePos( iPosX+75 , iPosY+9, iPosX+75+16 , iPosY+9+16 ) );
   VolPButton .SetPos( FramePos( iPosX+128, iPosY+9, iPosX+128+16, iPosY+9+16 ) );
   RndButtonOn.SetPos( FramePos( iPosX+147, iPosY+9, iPosX+147+16, iPosY+9+16 ) );
   RndButtonOf.SetPos( FramePos( iPosX+147, iPosY+9, iPosX+147+16, iPosY+9+16 ) );

   m_VolVal.SetPos( FramePos( iPosX+91, iPosY+11,iPosX+127, iPosY+22) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_Mp3Dlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   if(!g_SaveGame.GetOptionsParam()->bDisplayMp3)
   {
      Hide();
      return;
   }

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
      
      if(iNewPosX+g_MP3DlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_MP3DlgW;
      if(iNewPosY+g_MP3DlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_MP3DlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   int iState = Stopped;
   
   iState = Custom.dwLastMp3PlayerState;
   
   bool bShow = PlayButton.IsShown();
   
   if(iState ==  Running &&  PlayButton.IsShown())
   {
      PlayButton.Hide();
      PauseButton.Show();
   }
   else if((iState ==  Paused || iState   ==  Stopped )&&  PauseButton.IsShown())
   {
      PlayButton.Show();
      PauseButton.Hide();
   }

   if(g_SaveGame.GetOptionsParam()->bMp3Random == 0 && (!RndButtonOf.IsShown() || RndButtonOn.IsShown()))
   {
      RndButtonOf.Show();
      RndButtonOn.Hide();
   }

   if(g_SaveGame.GetOptionsParam()->bMp3Random == 1 && (!RndButtonOn.IsShown() || RndButtonOf.IsShown()))
   {
      RndButtonOf.Hide();
      RndButtonOn.Show();
   }

   char strPos[512];
   sprintf_s(strPos,512,"%d %%",g_SaveGame.GetOptionsParam()->iMp3Volume);
   m_VolVal.SetText(strPos,g_DefColorD,g_Global.GetScreenW(),0,false,2);

   GameUI::Draw( vsfFX );
  
}


void V3_Mp3Dlg::Hide()
{ 
   BoxUI::Hide();
}



void V3_Mp3Dlg::StartDragDlg( MousePos mousePos )
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

         
         RootBoxUI::GetInstance()->SetDragUI((GameUI*)&Drag_ButtonL);
         g_Global.SetDraggingDialog(true);
      }
   }
}

void V3_Mp3Dlg::StopDragDlg()
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
      
      if(m_iXPos+g_MP3DlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_MP3DlgW;
      if(m_iYPos+g_MP3DlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_MP3DlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastMP3XYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      
      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_Mp3Dlg::DragEvent::LeftClicked( void )
{
   V3_Mp3Dlg *me = static_cast< V3_Mp3Dlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_Mp3Dlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_Mp3Dlg *me = static_cast< V3_Mp3Dlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}

void V3_Mp3Dlg::PlayButtonEvent::LeftClicked( void )
{
   if(Custom.gMp3List.size() <=0)
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[684], CL_RED, true);
      return;
   }

   ::SetEvent(g_hMp3EventPlay);
}


void V3_Mp3Dlg::PauseButtonEvent::LeftClicked( void )
{
   if(Custom.gMp3List.size() >=0)
      ::SetEvent(g_hMp3EventPaused);
}

void V3_Mp3Dlg::PrevButtonEvent::LeftClicked( void )
{
   if(Custom.gMp3List.size() >=0)
      ::SetEvent(g_hMp3EventPrev);
}

void V3_Mp3Dlg::NextButtonEvent::LeftClicked( void )
{
   if(Custom.gMp3List.size() >=0)
      ::SetEvent(g_hMp3EventNext);
}

void V3_Mp3Dlg::RndButtonEvent::LeftClicked( void )
{
   g_SaveGame.GetOptionsParam()->bMp3Random = 1-g_SaveGame.GetOptionsParam()->bMp3Random;
}

void V3_Mp3Dlg::VolMButtonEvent::LeftClicked( void )
{
   V3_Mp3Dlg *me = static_cast< V3_Mp3Dlg * >( GetUI() );

   me->m_iVolume10--;
   if(me->m_iVolume10 < 0)
      me->m_iVolume10 = 0;
   else if(me->m_iVolume10 >10)
      me->m_iVolume10 = 10;

   g_SaveGame.GetOptionsParam()->iMp3Volume = me->m_iVolume10*10;

   ::SetEvent(g_hMp3EventVol);
}

void V3_Mp3Dlg::VolPButtonEvent::LeftClicked( void )
{
   V3_Mp3Dlg *me = static_cast< V3_Mp3Dlg * >( GetUI() );

   me->m_iVolume10++;
   if(me->m_iVolume10 < 0)
      me->m_iVolume10 = 0;
   else if(me->m_iVolume10 >10)
      me->m_iVolume10 = 10;
   
   g_SaveGame.GetOptionsParam()->iMp3Volume = me->m_iVolume10*10;
   ::SetEvent(g_hMp3EventVol);
}

