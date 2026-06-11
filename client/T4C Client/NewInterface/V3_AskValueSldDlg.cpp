// V3_AskValueSldDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_AskValueSldDlg.h"
#include "V3_ChatLogDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"

#include "V3_StatDlg.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_AskValueSldDlg::V3_AskValueSldDlg():
m_btnCancel( cancelEvent),
m_btnOK( okEvent),
m_pParent (NULL),
m_SldValue( sliderUpdateQtyEvent )
{
   int i=0;

   m_iXPos       = 0;
   m_iYPos       = 0;
   m_uiShowCodeP = 0;
  

   m_BackgroundS.SetSpriteId("V3_AskValueBack",true,"V3_AskValueBackMask");
   m_BackAsk.SetSpriteId("V3_AskValueSld");

   m_btnOK.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnOK.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnOK.SetHighlightUI( "V3_Btn92x24_H" );
   okEvent.SetUI( this );

  
   m_btnCancel.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnCancel.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnCancel.SetHighlightUI( "V3_Btn92x24_H" );
   cancelEvent.SetUI( this );

   m_editValue.EnableCursor( true );
   m_editValue.SetFilter( &amountEditFilter );
   m_editValue.SetMaxTextLength( 9 );
   m_editValue.SetParent( this );

   sliderUpdateQtyEvent.SetUI( this );

   m_stTag.SetText("");


   UpdateAllFramePos(m_iXPos,m_iYPos);
  
   AddChild( &m_BackgroundS ); 
   AddChild( &m_BackAsk );
   AddChild( &m_btnOK );
   AddChild( &m_btnCancel );
   AddChild( &m_stTag );
   AddChild( &m_editValue );
   AddChild( &m_SldValue  );

  
   RootBoxUI::GetInstance()->AddChild( this );

} 

V3_AskValueSldDlg::~V3_AskValueSldDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_AskValueSldDlg *V3_AskValueSldDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_AskValueSldDlg instance;
    return &instance;
    
} 

void V3_AskValueSldDlg::SetXYPos(int iX,int iY)
{
   m_iXPos = iX;
   m_iYPos = iY;
}

void V3_AskValueSldDlg::SetDialogText(std::string strTagName, int iMaxLenght,int iHelpID,int iDefaultVal)
{
   m_iMaxQty = iMaxLenght;

   m_stTag.SetText("");

   lastClickedItem = &m_editValue;
   m_stTag    .SetText( strTagName,g_DefColorH);
   
   char strTmp[100];
   sprintf_s(strTmp,100,"%d",iDefaultVal);

   m_editValue.SetText( strTmp);

   m_editValue.SetMaxTextLength( 20 );
   if(iHelpID != 0)
      SetHelpId( m_editValue, iHelpID );
} 


void V3_AskValueSldDlg::SetButtonText( std::string strOK, std::string strCancel,int iOKHelpID,int iCancelHelpID)
{
   m_btnOK     .SetBtnInfo(strOK,g_DefColorH);
   m_btnCancel .SetBtnInfo(strCancel,g_DefColorH);

   SetHelpId( m_btnOK    , iOKHelpID     );
   SetHelpId( m_btnCancel, iCancelHelpID );
}

void V3_AskValueSldDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_AskValueDlgW, iPosY+g_AskValueDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX    , iPosY     , iPosX+g_AskValueDlgW   , iPosY+g_AskValueDlgH    ) );
   m_BackAsk    .SetPos( FramePos( iPosX+44 , iPosY+74  , iPosX+366   , iPosY+123    ) );
   

   m_btnOK      .SetPos( FramePos( iPosX+104, iPosY+146  , iPosX+104+92, iPosY+146+24 ) );
   m_btnCancel  .SetPos( FramePos( iPosX+216, iPosY+146  , iPosX+216+92, iPosY+146+24 ) );

   m_editValue  .SetPos( FramePos( iPosX+214, iPosY+102+2, iPosX+365   , iPosY+122    ) );
   m_stTag      .SetPos( FramePos( iPosX+45 , iPosY+75   , iPosX+365   , iPosY+95     ) );
   m_SldValue   .SetSliderDef(1,m_iMaxQty ,iPosX,iPosY,20,44,102,198,121);
   m_SldValue   .SetSliderPos(1);
 }

//////////////////////////////////////////////////////////////////////////////////////////
void V3_AskValueSldDlg::Draw(V2SPRITEFX *vsfFX)
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


void V3_AskValueSldDlg::Hide()
{
   BoxUI::Hide();

   if(m_pParent)
      m_pParent->AskValueReturnSld(false,0,NULL,0);
}

void V3_AskValueSldDlg::HideNoCB()
{
   BoxUI::Hide();
}



void V3_AskValueSldDlg::Show(UINT uiShowCodeP,int iDefaultVal)
{
   m_uiShowCodeP = uiShowCodeP;
   UpdateAllFramePos(m_iXPos,m_iYPos);
   m_SldValue.SetSliderPos(iDefaultVal); //CV: Ajoute le slider a la meme position que le edit...
   BoxUI::Show();
}


void V3_AskValueSldDlg::OkEvent::LeftClicked( void )
{
	V3_AskValueSldDlg *me = static_cast< V3_AskValueSldDlg * >( GetUI() );

   if (GetTickCount() < me->dwDelayUntill) 
      return;

	
   DWORD qty = atoi( me->m_editValue.GetText().c_str() );

   me->HideNoCB();
	if(me->m_pParent)
      me->m_pParent->AskValueReturnSld(true,me->m_uiShowCodeP,(char *)me->m_editValue.GetText().c_str(),qty);
}

void V3_AskValueSldDlg::CancelEvent::LeftClicked( void )
{
   V3_AskValueSldDlg *me = static_cast< V3_AskValueSldDlg * >( GetUI() );
   
   if (GetTickCount() < me->dwDelayUntill) 
      return;
   
   
   me->Hide();
}


//  Keyboard hit
bool V3_AskValueSldDlg::VKeyInput(VKey ivKey)
{
   if (ivKey == VK_ESCAPE) 
   {
      cancelEvent.LeftClicked();
      return true;
   } 
   if (ivKey == VK_RETURN) 
   {
      okEvent.LeftClicked();
      return true;
   }
   return true;
}



//////////////////////////////////////////////////////////////////////////////////////////
void V3_AskValueSldDlg::SliderUpdateQtyEvent::LeftClicked( void )
{
   V3_AskValueSldDlg *me = static_cast< V3_AskValueSldDlg * >( GetUI() );
   
   char buf[ 256 ];
   _itoa_s( me->m_SldValue.GetSliderPos(), buf,256, 10 );
   // Update the edit box to the current slider's position.
   me->m_editValue.SetText(buf);
}

//  Filters the text input box and only lets numbers pass through.
bool V3_AskValueSldDlg::AmountEditFilter::IsOK(char nextCh, const std::string curText)
{
   return isdigit( nextCh ) != 0 ? true : false;
}

//  Sets the junk drop popup's current state.
void V3_AskValueSldDlg::SetDelay(DWORD dwDelay_ms)
{
   dwDelayUntill = GetTickCount() + dwDelay_ms;
}

void V3_AskValueSldDlg::SetParent(BoxUI *pParent)
{
   dwDelayUntill = GetTickCount(); //set default delais to now...
   m_pParent = pParent;
}