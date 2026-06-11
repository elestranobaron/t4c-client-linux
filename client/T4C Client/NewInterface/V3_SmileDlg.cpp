// V3_SmileDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_SmileDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"

#include "V3_StatDlg.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_SmileDlg::V3_SmileDlg():

cbXButton (xEvent),
btnSmilex01y01(smilex01y01Event),
btnSmilex02y01(smilex02y01Event),
btnSmilex03y01(smilex03y01Event),
btnSmilex04y01(smilex04y01Event),
btnSmilex05y01(smilex05y01Event),
btnSmilex06y01(smilex06y01Event),
btnSmilex07y01(smilex07y01Event),
btnSmilex08y01(smilex08y01Event),
btnSmilex09y01(smilex09y01Event),
btnSmilex01y02(smilex01y02Event),
btnSmilex02y02(smilex02y02Event),
btnSmilex03y02(smilex03y02Event),
btnSmilex04y02(smilex04y02Event),
btnSmilex05y02(smilex05y02Event),
btnSmilex06y02(smilex06y02Event),
btnSmilex07y02(smilex07y02Event),
btnSmilex08y02(smilex08y02Event),
btnSmilex09y02(smilex09y02Event),
btnSmilex01y03(smilex01y03Event),
btnSmilex02y03(smilex02y03Event),
btnSmilex03y03(smilex03y03Event),
btnSmilex04y03(smilex04y03Event),
btnSmilex05y03(smilex05y03Event),
btnSmilex06y03(smilex06y03Event),
btnSmilex07y03(smilex07y03Event),
btnSmilex08y03(smilex08y03Event),
btnSmilex09y03(smilex09y03Event),

m_pParent (NULL)
{
   int i=0;

   m_iXPos         = 0;
   m_iYPos         = 0;
   m_SelectedSmile = 0;
  

   m_BackgroundS.SetSpriteId("V3_SmileBack",true,"V3_SmileBackMask");

   
   
   cbXButton.SetSpriteId   ( "V3_CheckX18_ON_N" );
   cbXButton.SetDownUI     ( "V3_CheckX18_ON_N" );
   cbXButton.SetHighlightUI( "V3_CheckX18_ON_H" ); 
   xEvent.SetUI    ( this );

   smilex01y01Event.SetUI    ( this );
   smilex02y01Event.SetUI    ( this );
   smilex03y01Event.SetUI    ( this );
   smilex04y01Event.SetUI    ( this );
   smilex05y01Event.SetUI    ( this );
   smilex06y01Event.SetUI    ( this );
   smilex07y01Event.SetUI    ( this );
   smilex08y01Event.SetUI    ( this );
   smilex09y01Event.SetUI    ( this );
   smilex01y02Event.SetUI    ( this );
   smilex02y02Event.SetUI    ( this );
   smilex03y02Event.SetUI    ( this );
   smilex04y02Event.SetUI    ( this );
   smilex05y02Event.SetUI    ( this );
   smilex06y02Event.SetUI    ( this );
   smilex07y02Event.SetUI    ( this );
   smilex08y02Event.SetUI    ( this );
   smilex09y02Event.SetUI    ( this );
   smilex01y03Event.SetUI    ( this );
   smilex02y03Event.SetUI    ( this );
   smilex03y03Event.SetUI    ( this );
   smilex04y03Event.SetUI    ( this );
   smilex05y03Event.SetUI    ( this );
   smilex06y03Event.SetUI    ( this );
   smilex07y03Event.SetUI    ( this );
   smilex08y03Event.SetUI    ( this );
   smilex09y03Event.SetUI    ( this );

   statNom.SetText(g_GUILocalString[377]);


   UpdateAllFramePos(m_iXPos,m_iYPos);
  
   AddChild( &m_BackgroundS ); 
   AddChild( &cbXButton);

   AddChild( &btnSmilex01y01);
   AddChild( &btnSmilex02y01);
   AddChild( &btnSmilex03y01);
   AddChild( &btnSmilex04y01);
   AddChild( &btnSmilex05y01);
   AddChild( &btnSmilex06y01);
   AddChild( &btnSmilex07y01);
   AddChild( &btnSmilex08y01);
   AddChild( &btnSmilex09y01);
   AddChild( &btnSmilex01y02);
   AddChild( &btnSmilex02y02);
   AddChild( &btnSmilex03y02);
   AddChild( &btnSmilex04y02);
   AddChild( &btnSmilex05y02);
   AddChild( &btnSmilex06y02);
   AddChild( &btnSmilex07y02);
   AddChild( &btnSmilex08y02);
   AddChild( &btnSmilex09y02);
   AddChild( &btnSmilex01y03);
   AddChild( &btnSmilex02y03);
   AddChild( &btnSmilex03y03);
   AddChild( &btnSmilex04y03);
   AddChild( &btnSmilex05y03);
   AddChild( &btnSmilex06y03);
   AddChild( &btnSmilex07y03);
   AddChild( &btnSmilex08y03);
   AddChild( &btnSmilex09y03);
   AddChild( &statNom );


  
   RootBoxUI::GetInstance()->AddChild( this );

} 

V3_SmileDlg::~V3_SmileDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_SmileDlg *V3_SmileDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_SmileDlg instance;
    return &instance;
    
} 

void V3_SmileDlg::SetXYPos(int iX,int iY)
{
   m_iXPos = iX;
   m_iYPos = iY;
}

void V3_SmileDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_SmileDlgW, iPosY+g_SmileDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX    , iPosY     , iPosX+g_SmileDlgW   , iPosY+g_SmileDlgH    ) );

   int iXP = 2;
   int iYP = 20;
   btnSmilex01y01.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex02y01.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex03y01.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex04y01.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex05y01.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex06y01.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex07y01.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex08y01.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex09y01.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   iXP = 2;
   iYP += 21;
   btnSmilex01y02.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex02y02.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex03y02.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex04y02.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex05y02.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex06y02.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex07y02.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex08y02.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex09y02.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   iXP = 2;
   iYP += 21;
   btnSmilex01y03.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex02y03.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex03y03.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex04y03.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex05y03.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex06y03.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex07y03.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex08y03.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;
   btnSmilex09y03.SetPos( FramePos( iPosX+iXP, iPosY+iYP, iPosX+iXP+20   , iPosY+iYP+20) );iXP+=21;



   cbXButton.SetPos( FramePos( iPosX+174, iPosY, iPosX+174+18   , iPosY+18) );

   statNom.SetPos( FramePos( iPosX+3, iPosY+2, iPosX+170, iPosY+17 ) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_SmileDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

   GameUI::Draw( vsfFX );

}


void V3_SmileDlg::Hide()
{
   BoxUI::Hide();
}

void V3_SmileDlg::Show()
{
   UpdateAllFramePos(m_iXPos,m_iYPos);
   BoxUI::Show();
}



void V3_SmileDlg::Smilex01y01Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(1);
   me->Hide();
}

void V3_SmileDlg::Smilex02y01Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(2);
   me->Hide();
}

void V3_SmileDlg::Smilex03y01Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(3);
   me->Hide();
}

void V3_SmileDlg::Smilex04y01Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(4);
   me->Hide();
}

void V3_SmileDlg::Smilex05y01Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(5);
   me->Hide();
}

void V3_SmileDlg::Smilex06y01Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(6);
   me->Hide();
}

void V3_SmileDlg::Smilex07y01Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(7);
   me->Hide();
}

void V3_SmileDlg::Smilex08y01Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(8);
   me->Hide();
}

void V3_SmileDlg::Smilex09y01Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(9);
   me->Hide();
}

void V3_SmileDlg::Smilex01y02Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(10);
   me->Hide();
}

void V3_SmileDlg::Smilex02y02Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(11);
   me->Hide();
}

void V3_SmileDlg::Smilex03y02Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(12);
   me->Hide();
}

void V3_SmileDlg::Smilex04y02Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(13);
   me->Hide();
}

void V3_SmileDlg::Smilex05y02Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(14);
   me->Hide();
}

void V3_SmileDlg::Smilex06y02Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(15);
   me->Hide();
}

void V3_SmileDlg::Smilex07y02Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(16);
   me->Hide();
}

void V3_SmileDlg::Smilex08y02Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(17);
   me->Hide();
}

void V3_SmileDlg::Smilex09y02Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(18);
   me->Hide();
}





void V3_SmileDlg::Smilex01y03Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(19);
   me->Hide();
}

void V3_SmileDlg::Smilex02y03Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(20);
   me->Hide();
}

void V3_SmileDlg::Smilex03y03Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(21);
   me->Hide();
}

void V3_SmileDlg::Smilex04y03Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(22);
   me->Hide();
}

void V3_SmileDlg::Smilex05y03Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(23);
   me->Hide();
}

void V3_SmileDlg::Smilex06y03Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(24);
   me->Hide();
}

void V3_SmileDlg::Smilex07y03Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(25);
   me->Hide();
}

void V3_SmileDlg::Smilex08y03Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(26);
   me->Hide();
}

void V3_SmileDlg::Smilex09y03Event::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   if(me->m_pParent) me->m_pParent->SmileReturn(27);
   me->Hide();
}






void V3_SmileDlg::XEvent::LeftClicked( void )
{
   V3_SmileDlg *me = static_cast< V3_SmileDlg * >( GetUI() );
   
   me->Hide();
}