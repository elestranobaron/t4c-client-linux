// V3_ConfirmDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_ConfirmDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"

#include "V3_StatDlg.h"
#include "V3_ComboDlg.h"



/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_ConfirmDlg::V3_ConfirmDlg():
m_btnCancel( hideEvent),
m_btnOK( okEvent),
m_pParent (NULL)
{
   int i=0;

   m_iXPos       = 0;
   m_iYPos       = 0;
   m_uiShowCodeP = 0;
  

   m_BackgroundS.SetSpriteId("V3_ConfirmBack",true,"V3_ConfirmBackMask");
   

   m_btnOK.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnOK.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnOK.SetHighlightUI( "V3_Btn92x24_H" );
   okEvent.SetUI( this );
   
   m_btnCancel.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnCancel.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnCancel.SetHighlightUI( "V3_Btn92x24_H" );
   hideEvent.SetUI( this );


   m_stTag.SetText("");


   UpdateAllFramePos(m_iXPos,m_iYPos);
  
   AddChild( &m_BackgroundS ); 
   AddChild( &m_btnOK );
   AddChild( &m_btnCancel );
   AddChild( &m_stTag );
  
   RootBoxUI::GetInstance()->AddChild( this );

} 

V3_ConfirmDlg::~V3_ConfirmDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_ConfirmDlg *V3_ConfirmDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_ConfirmDlg instance;
    return &instance;
    
} 

void V3_ConfirmDlg::SetXYPos(int iX,int iY)
{
   m_iXPos = iX;
   m_iYPos = iY;
}

void V3_ConfirmDlg::SetDialogText( std::string strTagName)
{
   m_stTag    .SetText( strTagName,g_DefColorH,319);
} 

void V3_ConfirmDlg::SetButtonText( std::string strOK, std::string strCancel,int iOKHelpID,int iCancelHelpID)
{
   m_btnOK    .SetBtnInfo(strOK,g_DefColorH);
   m_btnCancel.SetBtnInfo(strCancel,g_DefColorH);

   SetHelpId( m_btnOK    , iOKHelpID     );
   SetHelpId( m_btnCancel, iCancelHelpID );
}

void V3_ConfirmDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_AskValueDlgW, iPosY+g_AskValueDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX    , iPosY     , iPosX+g_AskValueDlgW   , iPosY+g_AskValueDlgH    ) );
   m_btnOK      .SetPos( FramePos( iPosX+104, iPosY+146 , iPosX+104+92, iPosY+146+24 ) );
   m_btnCancel  .SetPos( FramePos( iPosX+216, iPosY+146 , iPosX+216+92, iPosY+146+24 ) );
   m_stTag      .SetPos( FramePos( iPosX+47 , iPosY+79  , iPosX+363   , iPosY+117    ) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_ConfirmDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

   //backGame.Draw(vsfFX);
   GameUI::Draw( vsfFX );

   HDC hdc;
   DXDGetDC(&hdc, 86);
   m_btnOK    .DrawTextButtonExt(hdc);
   m_btnCancel.DrawTextButtonExt(hdc);
   DXDReleaseDC(hdc, 86);
}


void V3_ConfirmDlg::Hide()
{
   BoxUI::Hide();

   if(m_pParent)
      m_pParent->ConfirmReturn(false,m_uiShowCodeP);
}

void V3_ConfirmDlg::HideNoCB()
{
   BoxUI::Hide();
}

void V3_ConfirmDlg::Show(UINT uiShowCodeP)
{
   if(V3_ComboDlg::GetInstance()->IsShown())
      V3_ComboDlg::GetInstance()->Hide();

   m_uiShowCodeP = uiShowCodeP;
   UpdateAllFramePos(m_iXPos,m_iYPos);
   BoxUI::Show();
}


void V3_ConfirmDlg::OkEvent::LeftClicked( void )
{
	V3_ConfirmDlg *me = static_cast< V3_ConfirmDlg * >( GetUI() );
	
   me->HideNoCB();

   if(me->m_pParent)
      me->m_pParent->ConfirmReturn(true,me->m_uiShowCodeP);
}


bool V3_ConfirmDlg::VKeyInput(VKey ivKey)
{
   if (ivKey == VK_ESCAPE) 
   {
      hideEvent.LeftClicked();
      return true;
   } 
   if (ivKey == VK_RETURN) 
   {
      okEvent.LeftClicked();
      return true;
   }
   return true;
}
