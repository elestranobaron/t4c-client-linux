// V3_AskValueDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_AskValueDlg.h"
#include "V3_ChatLogDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"

#include "V3_StatDlg.h"

//////////////////////////////////////////////////////////////////////////////////////////
//  Determines if a VK_ key is a valid macro used in conjunction with CTRL
// 
// Return: bool, true if its valid.
//////////////////////////////////////////////////////////////////////////////////////////
inline bool IsKeyValidWithCtrl(WORD key)

{
    if( ( key >= 'A' && key <= 'Z' ) ||
        ( key >= '0' && key <= '9' ) ){
        return true;
    }
    switch( key ){
    case VK_BACK:
    case VK_TAB:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_END:
    case VK_HOME:
    case VK_INSERT:
    case VK_DELETE:
    case VK_NUMPAD0:
    case VK_NUMPAD1:
    case VK_NUMPAD2:
    case VK_NUMPAD3:
    case VK_NUMPAD4:
    case VK_NUMPAD5:
    case VK_NUMPAD6:
    case VK_NUMPAD7:
    case VK_NUMPAD8:
    case VK_NUMPAD9:
    case VK_MULTIPLY:
    case VK_ADD:
    case VK_SEPARATOR:
    case VK_SUBTRACT:
    case VK_DECIMAL:
    case VK_DIVIDE:
    case VK_F2:
    case VK_F3:
    case VK_F4:
    case VK_F5:
    case VK_F6:
    case VK_F7:
    case VK_F8:
    case VK_F9:
    case VK_F10:
    case VK_F11:
    case VK_F12:
    case VK_F13:
    case VK_F14:
    case VK_F15:
    case VK_F16:
    case VK_F17:
    case VK_F18:
    case VK_F19:
    case VK_F20:
    case VK_F21:
    case VK_F22:
    case VK_F23:
    case VK_F24:
        return true;
    };

    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Determines if a VK_ is a valid macro used in conjucnction with SHIFT or no key
//  combination at all.
//
// Return: bool, true if its valid.
//////////////////////////////////////////////////////////////////////////////////////////
inline bool IsKeyValidWithShiftOrNothing( WORD key )
{
    switch( key ){
    case VK_F2:
    case VK_F3:
    case VK_F4:
    case VK_F5:
    case VK_F6:
    case VK_F7:
    case VK_F8:
    case VK_F9:
    case VK_F10:
    case VK_F11:
    case VK_F12:
    case VK_F13:
    case VK_F14:
    case VK_F15:
    case VK_F16:
    case VK_F17:
    case VK_F18:
    case VK_F19:
    case VK_F20:
    case VK_F21:
    case VK_F22:
    case VK_F23:
    case VK_F24:
        return true;
    };

    return false;
}



namespace
{
   T3VSBSound *naSound = NULL;
   
   void InitSound()
   {
      if( naSound == NULL )
      {
         naSound = new T3VSBSound;
         naSound->Create( "General NA sound", TS_MEMORY );
      }
   }
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_AskValueDlg::V3_AskValueDlg():
m_btnCancel( hideEvent),
m_btnOK( okEvent),
m_btnEffacer( effacerEvent),
m_bUseAsk2 (false),
m_bEffacerBtn (false),
m_bUseVKey (false),
m_pParent (NULL)
{
   int i=0;
   InitSound();

   m_iXPos       = 0;
   m_iYPos       = 0;
   m_uiShowCodeP = 0;
  

   m_BackgroundS.SetSpriteId("V3_AskValueBack",true,"V3_AskValueBackMask");
   m_BackAsk1.SetSpriteId("V3_AskValueLine");
   m_BackAsk2.SetSpriteId("V3_AskValueLine");;
   m_BackAsk2.Hide();

   m_btnOK.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnOK.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnOK.SetHighlightUI( "V3_Btn92x24_H" );
   okEvent.SetUI( this );

   m_btnEffacer.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnEffacer.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnEffacer.SetHighlightUI( "V3_Btn92x24_H" );
   m_btnEffacer.Hide();
   effacerEvent.SetUI( this );

   
   
   
   m_btnCancel.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnCancel.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnCancel.SetHighlightUI( "V3_Btn92x24_H" );
   hideEvent.SetUI( this );

   m_editValue1.EnableCursor( true );
   m_editValue1.SetMaxTextLength( 20 );
   m_editValue2.EnableCursor( true );
   m_editValue2.SetMaxTextLength( 20 );
   m_editValue2.Hide();

   m_stTag1.SetText("");
   m_stTag2.SetText("");
   m_stTag2.Hide();

   m_editValue1.SetParent( this );
   m_editValue2.SetParent( this );


   UpdateAllFramePos(m_iXPos,m_iYPos);
  
   AddChild( &m_BackgroundS ); 
   AddChild( &m_BackAsk1 );
   AddChild( &m_BackAsk2 );
   AddChild( &m_btnOK );
   AddChild( &m_btnEffacer);
   AddChild( &m_btnCancel );
   AddChild( &m_stTag1 );
   AddChild( &m_stTag2 );
   AddChild( &m_editValue1 );
   AddChild( &m_editValue2 );

  
   RootBoxUI::GetInstance()->AddChild( this );

} 

V3_AskValueDlg::~V3_AskValueDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_AskValueDlg *V3_AskValueDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_AskValueDlg instance;
    return &instance;
    
} 

void V3_AskValueDlg::SetXYPos(int iX,int iY)
{
   m_iXPos = iX;
   m_iYPos = iY;
}

void V3_AskValueDlg::SetDialogText1(bool bUseVKey, VKey vkValue,std::string strTagName, std::string strFileName,int iMaxLenght,int iHelpID)
{
   m_bUseVKey = bUseVKey;
   m_bUseAsk2 = false;
   vKey       = vkValue;

   m_stTag1.SetText("");
   m_stTag2.SetText("");
   m_stTag2    .Hide();
   m_editValue2.Hide();
   m_BackAsk2  .Hide();
   lastClickedItem = &m_editValue1;

   m_stTag1    .SetText( strTagName,g_DefColorH);
   
   if(m_bUseVKey)
      m_editValue1.SetText( vKey.GetName() );
   else
      m_editValue1.SetText( strFileName);

   m_editValue1.SetMaxTextLength( iMaxLenght );
   if(iHelpID != 0)
      SetHelpId( m_editValue1, iHelpID );
} 

void V3_AskValueDlg::SetDialogText2( std::string strTagName, std::string strFileName,int iMaxLenght,int iHelpID)
{
   m_bUseAsk2 = true;
   m_stTag2.SetText("");
   m_stTag2    .Show();
   m_editValue2.Show();
   m_BackAsk2  .Show();
   
   m_stTag2    .SetText( strTagName,g_DefColorH);
   m_editValue2.SetText( strFileName);
   m_editValue2.SetMaxTextLength( iMaxLenght );
   if(iHelpID != 0)
      SetHelpId( m_editValue2, iHelpID );
} 

void V3_AskValueDlg::SetButtonText( std::string strOK, std::string strCancel,int iOKHelpID,int iCancelHelpID,bool bShowEffacer)
{
   m_bEffacerBtn = bShowEffacer;
   m_btnOK     .SetBtnInfo(strOK,g_DefColorH);
   m_btnCancel .SetBtnInfo(strCancel,g_DefColorH);
   if(m_bEffacerBtn)
   {
      m_btnEffacer.SetBtnInfo(g_GUILocalString[97],g_DefColorH);
      m_btnEffacer.Show();
   }
   else
      m_btnEffacer.Hide();
   

   SetHelpId( m_btnOK    , iOKHelpID     );
   SetHelpId( m_btnCancel, iCancelHelpID );
}

void V3_AskValueDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_AskValueDlgW, iPosY+g_AskValueDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX    , iPosY     , iPosX+g_AskValueDlgW   , iPosY+g_AskValueDlgH    ) );
   m_BackAsk1   .SetPos( FramePos( iPosX+44 , iPosY+74  , iPosX+366   , iPosY+96    ) );
   m_BackAsk2   .SetPos( FramePos( iPosX+44 , iPosY+101 , iPosX+366   , iPosY+123   ) );
   

   if(m_bEffacerBtn)
   {
      m_btnOK      .SetPos( FramePos( iPosX+160, iPosY+146  , iPosX+160+92, iPosY+146+24 ) );
      m_btnCancel  .SetPos( FramePos( iPosX+263, iPosY+146  , iPosX+263+92, iPosY+146+24 ) );
      m_btnEffacer .SetPos( FramePos( iPosX+57 , iPosY+146  , iPosX+57 +92, iPosY+146+24 ) );
   }
   else
   {
      m_btnOK      .SetPos( FramePos( iPosX+104, iPosY+146  , iPosX+104+92, iPosY+146+24 ) );
      m_btnCancel  .SetPos( FramePos( iPosX+216, iPosY+146  , iPosX+216+92, iPosY+146+24 ) );
   }
   


   m_editValue1 .SetPos( FramePos( iPosX+173, iPosY+75+2 , iPosX+365   , iPosY+95     ) );
   m_editValue2 .SetPos( FramePos( iPosX+173, iPosY+102+2, iPosX+365   , iPosY+122     ) );
   m_stTag1     .SetPos( FramePos( iPosX+45 , iPosY+75   , iPosX+162   , iPosY+95     ) );
   m_stTag2     .SetPos( FramePos( iPosX+45 , iPosY+102  , iPosX+162   , iPosY+122     ) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_AskValueDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

   //backGame.Draw(vsfFX);
   GameUI::Draw( vsfFX );

   HDC hdc;
   DXDGetDC(&hdc, 86);
   m_btnOK     .DrawTextButtonExt(hdc);
   m_btnCancel .DrawTextButtonExt(hdc);
   m_btnEffacer.DrawTextButtonExt(hdc);
   DXDReleaseDC(hdc, 86);
}


void V3_AskValueDlg::Hide()
{
   BoxUI::Hide();

   if(m_pParent)
      m_pParent->AskValueReturn(false,0,NULL,NULL,0);
}

void V3_AskValueDlg::HideNoCB()
{
   BoxUI::Hide();
}



void V3_AskValueDlg::Show(UINT uiShowCodeP)
{
   m_uiShowCodeP = uiShowCodeP;
   UpdateAllFramePos(m_iXPos,m_iYPos);
   BoxUI::Show();
}


void V3_AskValueDlg::OkEvent::LeftClicked( void )
{
	V3_AskValueDlg *me = static_cast< V3_AskValueDlg * >( GetUI() );

   me->HideNoCB();

	if(me->m_pParent)
      me->m_pParent->AskValueReturn(true,me->m_uiShowCodeP,(char *)me->m_editValue1.GetText().c_str(),(char *)me->m_editValue2.GetText().c_str(),me->vKey);
}

void V3_AskValueDlg::EffacerEvent::LeftClicked( void )
{
   V3_AskValueDlg *me = static_cast< V3_AskValueDlg * >( GetUI() );
   
   me->m_editValue1.SetText("");
   me->vKey = 0;
   
}



// Keyboard hit .
bool V3_AskValueDlg::TextInput(char ch)
{
   if(m_bUseVKey)
      return false;

   // Enter pressed.        
   if( ch == '\r' )
   {
      if(m_pParent)
         m_pParent->AskValueReturn(true,m_uiShowCodeP,(char *)m_editValue1.GetText().c_str(),(char *)m_editValue2.GetText().c_str(),vKey);
      Hide();
   }
   else
   {
      // Tab pressed
      if( ch == '\t' )
      {
         if(m_bUseAsk2)
         {
            if( GetLastClickedItem() == &m_editValue1 )
               lastClickedItem = &m_editValue2;
            else
               lastClickedItem = &m_editValue1;
         }
         else
         {
            lastClickedItem = &m_editValue1;
         }
      }
      else if( ch != 0 )
      {
         if( GetLastClickedItem() != NULL )
         {
            GetLastClickedItem()->TextInput( ch );
         }
      }
   }
   return true;
}


//  Keyboard hit
bool V3_AskValueDlg::VKeyInput(VKey ivKey)
{
   if(!m_bUseVKey)
      return false;

   bool isValid = false;
   if( ivKey.CTRL() )
   {
      isValid = IsKeyValidWithCtrl( ivKey.GetKey() );
   }
   else
   {
      isValid = IsKeyValidWithShiftOrNothing( ivKey.GetKey() );
   }
   
   if (!ivKey.CTRL() ) 
   {
      if (ivKey == VK_ESCAPE) 
      {
         if (vKey == 0) 
         {
            hideEvent.LeftClicked();
            return true;
         } 
         else 
         {
            vKey = 0;
            m_editValue1.SetText( vKey.GetName() );
            return true;
         }
      } 
      else if (ivKey == VK_RETURN) 
      {
         okEvent.LeftClicked();
         return true;
      }
   }
   
   if( !isValid )
   {
      naSound->Play( TRUE );
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 357 ], SYSTEM_COLOR, false );
      return true;
   }
   
   // If a custom macro of macroUI macro already exist.
   if( Custom.gMacro.MacroExists( ivKey ) || V3_MacroDlg::GetInstance()->KeyExists( ivKey ) )
   {
      naSound->Play( TRUE );
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 358 ], SYSTEM_COLOR, false );
      return true;
   }

   vKey = ivKey;
   m_editValue1.SetText( vKey.GetName() );
  
   
   return true;
}