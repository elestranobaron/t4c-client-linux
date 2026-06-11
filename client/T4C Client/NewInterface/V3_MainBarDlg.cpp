// V3_MainBarDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_MainBarDlg.h"
#include "RootBoxUI.h"
#include "V3_ChatLogDlg.h"
#include "V3_StatDlg.h"
#include "V3_InvDlg.h"
#include "V3_SpellDlg.h"
#include "V3_ProfessionDlg.h"
#include "V3_GuildDlg.h"
#include "V3_GroupeDlg.h"
#include "V3_MacroDlg.h"
#include "V3_OptionsDlg.h"
#include "V3_ConfirmDlg.h"
#include "V3_LifeDlg.h"
#include "V3_ComboDlg.h"
#include "V3_SmileDlg.h"
#include "V3_ChatDlg.h"
#include "V3_AHDlg.h"
#include "V3_AHVendreDlg.h"
#include "V3_BoutiqueDlg.h"
#include "V3_InteractionRP.h"
#include "V3_InteractionPVP.h"
#include "V3_QuestBookP1.h"
#include "V3_QuestBookP2.h"
#include "V3_QuestBookP3.h"
#include "V3_BuyDlg.h"
#include "V3_ChestDlg.h"
#include "V3_ChestNewDlg.h"
#include "V3_DisplayHelpDlg.h"
#include "V3_DisplaySpecialDlg.h"
#include "V3_GuildChestDlg.h"
#include "V3_GuildChestNewDlg.h"
#include "V3_ItemInfoDlg.h"
#include "V3_RTMapDlg.h"
#include "V3_TradeDlg.h"
#include "V3_Arene1WaitDlg.h"
#include "V3_Arene1PlayDlg.h"
#include "V3_GMDlg.h"
#include "V3_TrainDlg.h"
#include "V3_TrainProfDlg.h"
#include "V3_VolDlg.h"
#include "V3_PanierDlg.h"
#include "V3_TMIDlg.h"
#include "V3_AskValueSldDlg.h"
#include "V3_ArenePointsDlg.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"

extern T3VSBSound SoundFX[250];
extern CSaveGame g_SaveGame;

namespace{
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

void ParseText( char *pText);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_MainBarDlg::V3_MainBarDlg():
Drag_ButtonL(dragEvent),
Drag_ButtonR(dragEvent),
m_BtnHome(btnHomeEvent),
m_BtnExit(btnExitEvent),
m_BtnCCDlg( btnCCDlgEvent ),
m_BtnCC( btnCCEvent ),
m_BtnDD( btnDDEvent ),
m_BtnWW( btnWWEvent ),
m_BtnCB( btnCBEvent),
m_TextInputState( SendToGame ),
m_iCurrentMP (0),
m_btnMPUp (mpUpEvent),
m_btnMPDn (mpDnEvent),
m_BtnChatLog (btnChatBoxEvent),
m_BtnQuestBook( btnQuestBookEvent),
m_BtnSmile( btnSmileEvent),
m_BtnPVP( btnPVPEvent)
{
   int x,y;
   InitSound();

   m_bDragDialog = false;

   m_iXPos = g_Global.GetDialogLastMainBarXPos();
   m_iYPos = g_Global.GetDialogLastMainBarYPos();
   
 
   m_Background.SetSpriteId("V3_MainBarBack",true,"V3_MainBarBackMask");
  
   
   //Buttons
   m_BtnHome.SetDownUI     ( "V3_MainBtnHome_D" );
   m_BtnHome.SetHighlightUI( "V3_MainBtnHome_H" ); 
   btnHomeEvent.SetUI( this );
   SetHelpId( m_BtnHome, 726 );

   m_BtnExit.SetDownUI     ( "V3_MainBtnExit_D" );
   m_BtnExit.SetHighlightUI( "V3_MainBtnExit_H" ); 
   btnExitEvent.SetUI( this );
   SetHelpId( m_BtnExit, 392 );

   m_XPBar.SetSpriteId("V3_MainXPBar");


   m_InputPatch.SetSpriteId("V3_MainInputPatch");
   m_InputPatch.Hide();
   

   
   //SetHelpId( m_BtnCCDlg, 256 );
   m_BtnCCDlg.SetSpriteId   ( "V3_MainBtnIconCC_N" );
   m_BtnCCDlg.SetDownUI     ( "V3_MainBtnIconCC_D" );
   m_BtnCCDlg.SetHighlightUI( "V3_MainBtnIconCC_H" );
   btnCCDlgEvent.SetUI( this );
   //m_BtnCCDlg.Hide();



   SetHelpId( m_BtnCC, 255 );
   m_BtnCC.SetSpriteId   ( "V3_MainBtnIconCC_N" );
   m_BtnCC.SetDownUI     ( "V3_MainBtnIconCC_D" );
   m_BtnCC.SetHighlightUI( "V3_MainBtnIconCC_H" );
   btnCCEvent.SetUI( this );
   m_BtnCC.Hide();

   SetHelpId( m_BtnDD, 256 );
   m_BtnDD.SetSpriteId   ( "V3_MainBtnIconD_N" );
   m_BtnDD.SetDownUI     ( "V3_MainBtnIconD_D" );
   m_BtnDD.SetHighlightUI( "V3_MainBtnIconD_H" );
   btnDDEvent.SetUI( this );
   //m_BtnDD.Hide();  on commence tj en direct talk par default
   
   SetHelpId( m_BtnWW, 257 );
   m_BtnWW.SetSpriteId   ( "V3_MainBtnIconW_N" );
   m_BtnWW.SetDownUI     ( "V3_MainBtnIconW_D" );
   m_BtnWW.SetHighlightUI( "V3_MainBtnIconW_H" );
   btnWWEvent.SetUI( this );
   m_BtnWW.Hide();

   //SetHelpId( m_BtnCB, 257 );
   m_BtnCB.SetSpriteId   ( "V3_MainBtnIconCB_N" );
   m_BtnCB.SetDownUI     ( "V3_MainBtnIconCB_D" );
   m_BtnCB.SetHighlightUI( "V3_MainBtnIconCB_H" );
   btnCBEvent.SetUI( this );
   m_BtnCB.Hide();



   m_BtnChatLog.SetSpriteId   ( "V3_MainBtnIconCB_N" );
   m_BtnChatLog.SetDownUI     ( "V3_MainBtnIconCB_D" );
   m_BtnChatLog.SetHighlightUI( "V3_MainBtnIconCB_H" );
   btnChatBoxEvent.SetUI( this );

   m_BtnQuestBook.SetSpriteId   ( "V3_MainBtnIconQB_N" );
   m_BtnQuestBook.SetDownUI     ( "V3_MainBtnIconQB_D" );
   m_BtnQuestBook.SetHighlightUI( "V3_MainBtnIconQB_H" );
   btnQuestBookEvent.SetUI( this );

   m_BtnSmile.SetSpriteId   ( "V3_MainBtnIconSmile_N" );
   m_BtnSmile.SetDownUI     ( "V3_MainBtnIconSmile_D" );
   m_BtnSmile.SetHighlightUI( "V3_MainBtnIconSmile_H" );
   btnSmileEvent.SetUI( this );

   m_BtnPVP.SetSpriteId   ( "V3_MainBtnIconPVP_N" );
   m_BtnPVP.SetDownUI     ( "V3_MainBtnIconPVP_D" );
   m_BtnPVP.SetHighlightUI( "V3_MainBtnIconPVP_H" );
   btnPVPEvent.SetUI( this );

   
   


   


   m_EditInput.EnableCursor( true );
   m_EditInput.EnableCursorF( true );
   m_EditInput.SetParent( this );
   SetHelpId( m_EditInput, 259 );
   

   m_EditPageInput.EnableCursor( true );
   m_EditPageInput.SetParent( this );
   SetHelpId( m_EditPageInput, 258 );
   m_EditPageInput.Hide();

   m_strLastCCSelected = "";
   m_stCCname.SetText(m_strLastCCSelected,g_DefColorD);
   m_stCCname.Hide();


   int iIndex = 0;
   for(y=0;y<g_iNbrMacroY;y++)
   {
      for(x=0;x<g_iNbrMacroX;x++)
      {
         m_MacroButton[y][x].iIndex = iIndex;
         iIndex++;
         if(y == 0)
            m_MacroButton[y][x].Show();
         else
            m_MacroButton[y][x].Hide();
      }
   }

   m_btnMPUp.SetSpriteId   ( "V3_SpinUP_N" );
   m_btnMPUp.SetHighlightUI( "V3_SpinUP_H" );
   
   m_btnMPDn.SetSpriteId   ( "V3_SpinDN_N" );
   m_btnMPDn.SetHighlightUI( "V3_SpinDN_H" );

   Drag_ButtonL.SetSpriteId   ( "" );
   Drag_ButtonL.SetDownUI     ( "" );
   Drag_ButtonL.SetHighlightUI( "" ); 
   Drag_ButtonR.SetSpriteId   ( "" );
   Drag_ButtonR.SetDownUI     ( "" );
   Drag_ButtonR.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this ); 
     

   UpdateAllFramePos(m_iXPos,m_iYPos);

   AddChild( &m_Background);
   AddChild( &Drag_ButtonL);
   AddChild( &Drag_ButtonR);
   AddChild( &m_BtnHome);
   AddChild( &m_BtnExit);
   AddChild( &m_stXP);
   AddChild( &m_BtnCCDlg);
   AddChild( &m_BtnCC);
   AddChild( &m_BtnDD);
   AddChild( &m_BtnWW);
   AddChild( &m_BtnCB);
   AddChild( &m_InputPatch);
   AddChild( &m_EditInput);
   AddChild( &m_EditPageInput);
   AddChild( &m_stCCname);
   AddChild( &m_strMP );
   AddChild( &m_btnMPUp );
   AddChild( &m_btnMPDn );
   AddChild( &m_BtnChatLog );
   AddChild( &m_BtnSmile );
   AddChild( &m_BtnQuestBook );
   AddChild( &m_BtnPVP );
   
   

   for(y=0;y<g_iNbrMacroY;y++)
   {
      for(x=0;x<g_iNbrMacroX;x++)
      {
         AddChild( &m_MacroButton[y][x]);
      }
   }
   
   RootBoxUI::GetInstance()->AddChild( this );

   //Initialize the typed history list
   rollbackTyped.push_back("");
	ii = rollbackTyped.end();

   m_BtnPVP.Hide();

   
} 

V3_MainBarDlg::~V3_MainBarDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_MainBarDlg *V3_MainBarDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_MainBarDlg instance;
    return &instance;
    
} 

void V3_MainBarDlg::ClearChannelList()
{
   m_ChannelList.clear();
}

void V3_MainBarDlg::AddChannel(std::string strName, bool bListen)
{
   sChannel nC;
   nC.strName = strName;
   nC.bListen = bListen;

   m_ChannelList.push_back(nC);
}

void V3_MainBarDlg::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   switch(uiShowCodeP)
   {
      case 0:
      {
         if(bOK)
         {
            T3VSBSound PopupSound;
            PopupSound.CopyWave(&SoundFX[110]);
            PopupSound.Play(FALSE);
         
            Sleep(10);
         
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_ExitGame;
            SEND_PACKET( sending );
        
            if(!COMM.isHalf())
               PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
         }
      }
      break;
   }
}

void V3_MainBarDlg::AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey)
{
   if(!bOK)
      return;

   switch(uiShowCodeP)
   {
      case 0:
      {
         if(pStrValue1)
         {
            TFCPacket Send2;
            Send2 << (RQ_SIZE)RQ_AskCompagnonName;
            Send2 << (char)strlen(pStrValue1);
            Send2 << (char *)pStrValue1;
            SEND_PACKET(Send2);
         }
      }
      break;
   }
}


//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_MainBarDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iXPos = g_Global.GetDialogLastMainBarXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastMainBarYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->UpdateMacroBtn();
   GetInstance()->Show();
   return 0;
}

void V3_MainBarDlg::UpdateAllFramePos(int iPosX,int iPosY)
{ 
   SetPos( FramePos( iPosX, iPosY+13, iPosX+g_MainBarDlgW, iPosY+g_MainBarDlgH ) );
   m_Background.SetPos( FramePos( iPosX, iPosY, iPosX+g_MainBarDlgW, iPosY+g_MainBarDlgH ) );
   Drag_ButtonL .SetPos( FramePos( iPosX, iPosY, iPosX+80, iPosY+g_MainBarDlgH ) );
   Drag_ButtonR .SetPos( FramePos( iPosX+620, iPosY, iPosX+g_MainBarDlgW, iPosY+g_MainBarDlgH ) );

   m_BtnHome.SetPos( FramePos( iPosX+82 , iPosY+44, iPosX+82+28 , iPosY+44+28 ) );
   m_BtnExit.SetPos( FramePos( iPosX+590, iPosY+44, iPosX+590+28, iPosY+44+28 ) );
   m_XPBar  .SetPos( FramePos( iPosX+79 , iPosY+75, iPosX+622,    iPosY+82) );
   m_stXP   .SetPos( FramePos( iPosX+315, iPosY+75, iPosX+384,    iPosY+82) );

   m_BtnCCDlg.SetPos( FramePos( iPosX+598, iPosY+17, iPosX+617,    iPosY+36) );
   m_BtnCC.SetPos( FramePos( iPosX+82, iPosY+17, iPosX+101,    iPosY+36) );
   m_BtnDD.SetPos( FramePos( iPosX+82, iPosY+17, iPosX+101,    iPosY+36) );
   m_BtnWW.SetPos( FramePos( iPosX+82, iPosY+17, iPosX+101,    iPosY+36) );
   m_BtnCB.SetPos( FramePos( iPosX+214,iPosY+16, iPosX+230,    iPosY+36) );

   m_BtnChatLog.SetPos( FramePos( iPosX+576,iPosY+16, iPosX+592,    iPosY+36) );
   m_BtnQuestBook.SetPos( FramePos( iPosX+556,iPosY+16, iPosX+576,    iPosY+36) );
   m_BtnSmile    .SetPos( FramePos( iPosX+536,iPosY+16, iPosX+556,    iPosY+36) );
   m_BtnPVP      .SetPos( FramePos( iPosX+516,iPosY+16, iPosX+536,    iPosY+36) );

   

   m_InputPatch.SetPos( FramePos( iPosX+228, iPosY+16, iPosX+230,    iPosY+36) );

   m_EditPageInput.SetPos( FramePos( iPosX+110, iPosY+19, iPosX+226,    iPosY+36) );
   m_stCCname     .SetPos( FramePos( iPosX+110, iPosY+19, iPosX+213,    iPosY+36) );

   m_strMP     .SetPos ( FramePos( iPosX+568+3, iPosY+51 , iPosX+579+3, iPosY+62 ) );
   m_btnMPUp   .SetPos ( FramePos( iPosX+566  , iPosY+39 , iPosX+581   ,iPosY+49 ) );
   m_btnMPDn   .SetPos ( FramePos( iPosX+566  , iPosY+64 , iPosX+581   ,iPosY+74 ) );

   switch(m_TextInputState)
   {
      case SendToGame:
         m_EditInput.SetPos( FramePos( iPosX+110, iPosY+19, iPosX+510,    iPosY+36) );
      break;
      case SendToPage:
      case SendToChannel:
         m_EditInput.SetPos( FramePos( iPosX+236, iPosY+19, iPosX+510,    iPosY+36) );
      break;
   }

   int iXS = 134;
   int iXW = 34;
   int iXO = 36;
   
   for(int y=0;y<g_iNbrMacroY;y++)
   {
      for(int x=0;x<g_iNbrMacroX;x++)
      {
         int iXPS = iXS+(x*iXO);
         m_MacroButton[y][x].SetPos( FramePos( iPosX+iXPS, iPosY+40, iPosX+iXPS+iXW, iPosY+72 ) );
      }
   }
   
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_MainBarDlg::Draw(V2SPRITEFX *vsfFX)
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
      
      if(iNewPosX < 0)
         iNewPosX = 0;
      
      if(iNewPosX+g_MainBarDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_MainBarDlgW;
      
      
      UpdateAllFramePos(iNewPosX,iOffY);
      
      iOffX = iNewPosX;
      
   }
 
   int percent = 0;
   //if(Player.Level == 1) 
   //{ 
   //   percent = (int)(Player.ExpLastLevel == 0 ? 0 : ( Player.Exp * 100 / Player.ExpLastLevel ));
   //}
   //else
   {
      double curXP = (double)(Player.Exp - Player.ExpLastLevel);
      double maxXP = (double)(Player.Exp2Go - Player.ExpLastLevel);
      percent = (int)(maxXP == 0 ? 0 : ( curXP * 100 / maxXP ));
   }


   if(V3_ArenePointsDlg::GetInstance()->IsShown())
      m_BtnPVP.Show();
   else
      m_BtnPVP.Hide();


   
   char strTmp[30];
   sprintf_s(strTmp,30,"%d %%",percent);
   m_stXP.SetText(strTmp,g_DefColorD,g_Global.GetScreenW(),0,false,2);
   sprintf_s(strTmp,30,"%d",m_iCurrentMP+1);
   m_strMP.SetText(strTmp,g_DefColorD,g_Global.GetScreenW(),0,false,2);

   GameUI::Draw( vsfFX );

   
   V3_LifeDlg::GetInstance()->DrawSingleStatusItem( m_XPBar, percent );

}


void V3_MainBarDlg::Hide()
{ 
   BoxUI::Hide();
}

void V3_MainBarDlg::Show()
{
   UpdateAllFramePos(g_Global.GetDialogLastMainBarXPos(),g_Global.GetDialogLastMainBarYPos());
   BoxUI::Show();
}



void V3_MainBarDlg::BtnHomeEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );


   //ICI on se souvien de la derniere fenetre ouverte et on ouvre cette fenetre

   switch(g_Global.GetLastDialogOpen())
   {
      case 0: V3_StatsDlg::GetInstance()->Display(NULL);        break;
      case 1: V3_InvDlg::GetInstance()->Display(NULL);          break;
      case 2: V3_SpellDlg::GetInstance()->Display(NULL);        break;
      case 3: V3_ProfessionDlg::GetInstance()->Display(NULL);   break;
      case 4: V3_GuildDlg::GetInstance()->Display(NULL);        break;
      case 5: V3_GroupeDlg::GetInstance()->Display(NULL);       break;
      case 6: V3_MacroDlg::GetInstance()->Display(NULL);        break;
      case 7: V3_OptionsDlg::GetInstance()->Display(NULL);      break;

      default:V3_StatsDlg::GetInstance()->Display(NULL);        break;
   }
}


void V3_MainBarDlg::BtnExitEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );
   
   V3_ConfirmDlg::GetInstance()->SetDialogText(g_GUILocalString[83]);
   V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
   V3_ConfirmDlg::GetInstance()->SetParent(me);
   V3_ConfirmDlg::GetInstance()->SetXYPos(((g_Global.GetScreenW()-g_AskValueDlgW)/2), ((g_Global.GetScreenH()-g_AskValueDlgH)/2));
   V3_ConfirmDlg::GetInstance()->Show(0);
   
   me->RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
}



void V3_MainBarDlg::BtnCCDlgEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );

   V3_ChatDlg::GetInstance()->Display(NULL);
}


void V3_MainBarDlg::BtnDDEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );
   
   me->m_TextInputState = SendToChannel;
   me->m_BtnDD.Hide();  // Hide the speak toggle button.
   me->m_BtnCC.Show(); // Show the channel
   me->m_BtnCB.Show();
   me->m_stCCname.Show();
   
   
   me->UpdateAllFramePos(me->m_iXPos,me->m_iYPos);
}


void V3_MainBarDlg::BtnCCEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );
   
   me->m_TextInputState = SendToPage;
   
   // Hide the channel toggle button.
   me->m_BtnCC.Hide();
   me->m_BtnCB.Hide();
   me->m_stCCname.Hide();

   // Show the page and graph patch.
   me->m_BtnWW.Show();
   me->m_InputPatch.Show();
   me->m_EditPageInput.Show();
   me->m_EditInput.SetParent( me );
   
   me->UpdateAllFramePos(me->m_iXPos,me->m_iYPos);
   
}



void V3_MainBarDlg::BtnWWEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );
   
   me->m_TextInputState = SendToGame;
   
   // Hide the page and graph patch.
   me->m_BtnWW.Hide();
   me->m_InputPatch.Hide();
   me->m_EditPageInput.Hide();
   
   
   me->m_EditInput.SetParent( NULL );
   me->m_BtnDD.Show(); // Show the speak toggle button.
   
   me->UpdateAllFramePos(me->m_iXPos,me->m_iYPos);
   
}

void V3_MainBarDlg::BtnCBEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );
   
   V3_ComboDlg::GetInstance()->ClearCombo();

   int iScrollPos = 0;
   int iSelectedCCPos = 0;
   //AddCC to Combo...
   for(UINT i=0;i<me->m_ChannelList.size();i++)
   {
      V3_ComboDlg::GetInstance()->AddComboItem(me->m_ChannelList[i].strName);

      std::string strName = me->m_ChannelList[i].strName;
      if(strcmp(me->m_strLastCCSelected.c_str(),strName.c_str()) == 0)
         iSelectedCCPos = i;
   }

   

   //le CC selectionner est pas visible on le mets premierd e la liste si possible...
   //sinon il sera a sa position dan sla liste...

   if(iSelectedCCPos > 3)
   {
      iScrollPos = iSelectedCCPos-7+4;
      if(iScrollPos > (int)me->m_ChannelList.size()-7)
      {
         
         iScrollPos -= (me->m_ChannelList.size()-7-iScrollPos);
      }
   }

   
   V3_ComboDlg::GetInstance()->SetParent(me);
   V3_ComboDlg::GetInstance()->SetXYPos(me->m_iXPos+109 ,me->m_iYPos+13-g_ComboDlgH);
   V3_ComboDlg::GetInstance()->Show(0,iScrollPos);
   
   
   me->RequestForegroundControl( V3_ComboDlg::GetInstance() );
}

void V3_MainBarDlg::BtnChatBoxEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );

   g_SaveGame.GetOptionsParam()->bDisplayChatLog = !g_SaveGame.GetOptionsParam()->bDisplayChatLog;

   if(g_SaveGame.GetOptionsParam()->bDisplayChatLog)
      V3_ChatLogDlg::GetInstance()->Display(NULL);
   else
      V3_ChatLogDlg::GetInstance()->Hide();
}

void V3_MainBarDlg::BtnQuestBookEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );

   TFCPacket packet;
   packet << (RQ_SIZE)RQ_QB_GetActiveQuest;
   SEND_PACKET( packet );
}

void V3_MainBarDlg::BtnSmileEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );

   int XPos = (me->m_iXPos+576)-g_SmileDlgW;

   V3_SmileDlg::GetInstance()->SetParent(me);
   V3_SmileDlg::GetInstance()->SetXYPos(XPos ,me->m_iYPos+13-g_SmileDlgH);
   V3_SmileDlg::GetInstance()->Show();

   me->RequestForegroundControl( V3_SmileDlg::GetInstance() );
}

void V3_MainBarDlg::BtnPVPEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );

   TFCPacket sending;
   sending << (RQ_SIZE)RQ_ARENA1_GetWaitPlayerList;
   sending << (long)V3_ArenePointsDlg::GetInstance()->GetArenaID();
   sending << (long)V3_ArenePointsDlg::GetInstance()->GetArenaType();
   SEND_PACKET( sending );
}



bool V3_MainBarDlg::TextInput(char ch)
{       
   bool analyzeCh = true;
   
   // Escape.
   if( ch == 27 )
   {
      return true;
   }

 
   // If the input is SendToPage.
   if( m_TextInputState == SendToPage )
   {
      // If the last clicked item is the page name edit control.
      if( GetLastClickedItem() == &m_EditPageInput )
      {
         analyzeCh = false;
         
         if( ch == '\r' || ch == '\t' )
         {
            lastClickedItem = &m_EditInput;
         }
         else
         {
            m_EditPageInput.TextInput( ch );
         }
      }        
   }

    if( analyzeCh )
    {

        if( ch == '\r' )
        {   
            // Determine where to send the text according to the UI's current state.
            switch( m_TextInputState )
            {
               case SendToGame:
               {
                  char *theText = new char[ m_EditInput.GetText().size() + 1 ];
                  strcpy_s( theText,m_EditInput.GetText().size() + 1, m_EditInput.GetText().c_str() );
                  ParseText( theText );
                  delete theText;
               }
               break;
               case SendToPage:
               {
                  ClientConfig *pConfig = g_Global.GetClientConfig();
                  if(!pConfig->dwAfkStatus)
                  {
                     OptionParam *pOption = g_SaveGame.GetOptionsParam();
                     if (!pOption->bPageEnable) 
                     {
                        // user ignore all page.
                        V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[100],CL_RED,true);
                     } 
                     else 
                     {
                        if( m_EditInput.GetText().empty() || m_EditPageInput.GetText().empty() )
                        {
                           naSound->Play( TRUE );
                           break;
                        }
                        
                        TFCPacket sending;
                        sending << (RQ_SIZE)RQ_Page;
                        sending << (short)m_EditPageInput.GetText().size();
                        sending << (char *)m_EditPageInput.GetText().c_str();
                        sending << (short)m_EditInput.GetText().size();
                        sending << (char *)m_EditInput.GetText().c_str();
                        
                        if (!V3_ChatDlg::GetInstance()->IsUserIgnored( m_EditPageInput.GetText().c_str())) //Page un ignorer
                        {
                           SEND_PACKET( sending );
                        } 
                        else 
                        {
                           V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[517], CL_RED, true, false );
                        }
                     }
                  } 
                  else 
                  {
                     V3_ChatLogDlg::GetInstance()->AddListLogs( "", "You cannot page while AFK!", CL_RED, true, false);
                  }
               }
               break;
               case SendToChannel:
               {
                  
                  // If there is no selected channel
                  if( m_strLastCCSelected.empty() )
                  {
                     naSound->Play( TRUE );
                     V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 487 ],SYSTEM_COLOR,false);
                     break;
                  }                           
                  
                  string msg = m_EditInput.GetText();
                  
                  // If the text input isn't empty.
                  if( !msg.empty() )
                  {
                    
                     TFCPacket sending;
                     // Create a send chatter message packet.
                     sending << (RQ_SIZE)RQ_SendChatterChannelMessage;
                     sending << (short)m_strLastCCSelected.size();
                     sending << (char *)m_strLastCCSelected.c_str();
                     sending << (short)msg.size();
                     sending << (char *)msg.c_str();
                     
                     SEND_PACKET( sending );
                  }
                  else
                  {
                     naSound->Play( TRUE );
                  }
               }
               break;
            }
            
            if ( m_EditInput.GetText().size() > 0 ) 
            {
               
               rollbackTyped.push_back(m_EditInput.GetText());
               ii = rollbackTyped.end();
               if (rollbackTyped.size() > 128) 
                  rollbackTyped.pop_front();
               m_EditInput.SetText( "" );
               
            }
        }
        else
        {    
           m_EditInput.TextInput( ch );
        }
    }

    m_EditInput.SetParent( this );
    return true;
}

bool V3_MainBarDlg::ManageESCGUIAll()
{
   UINT uiNbrShow = 0;

   if(V3_StatsDlg::GetInstance() && V3_StatsDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_InvDlg::GetInstance() && V3_InvDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_SpellDlg::GetInstance() && V3_SpellDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_ProfessionDlg::GetInstance() && V3_ProfessionDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_GuildDlg::GetInstance() && V3_GuildDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_GroupeDlg::GetInstance() && V3_GroupeDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_MacroDlg::GetInstance() && V3_MacroDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_OptionsDlg::GetInstance() && V3_OptionsDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_ChatDlg::GetInstance() && V3_ChatDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_ComboDlg::GetInstance() && V3_ComboDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_AHDlg::GetInstance() && V3_AHDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_BoutiqueDlg::GetInstance() && V3_BoutiqueDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_InteractionRPDlg::GetInstance() && V3_InteractionRPDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_QuestBookP1::GetInstance() && V3_QuestBookP1::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_QuestBookP2::GetInstance() && V3_QuestBookP2::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_QuestBookP3::GetInstance() && V3_QuestBookP3::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_InteractionPVPDlg::GetInstance() && V3_InteractionPVPDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_Arene1WaitDlg::GetInstance() && V3_Arene1WaitDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_Arene1PlayDlg::GetInstance() && V3_Arene1PlayDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_BuyDlg::GetInstance() && V3_BuyDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_DisplayHelpDlg::GetInstance() && V3_DisplayHelpDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_DisplaySpecialDlg::GetInstance() && V3_DisplaySpecialDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_GMDlg::GetInstance() && V3_GMDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_ItemInfoDlg::GetInstance() && V3_ItemInfoDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_PanierDlg::GetInstance() && V3_PanierDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_ProfessionDlg::GetInstance() && V3_ProfessionDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_RTMapDlg::GetInstance() && V3_RTMapDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_TradeDlg::GetInstance() && V3_TradeDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_TrainDlg::GetInstance() && V3_TrainDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_TrainProfDlg::GetInstance() && V3_TrainProfDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_VolDlg::GetInstance() && V3_VolDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_GuildChestDlg::GetInstance() && V3_GuildChestDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_ConfirmDlg::GetInstance() && V3_ConfirmDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_AHVendreDlg::GetInstance() && V3_AHVendreDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_AskValueDlg::GetInstance() && V3_AskValueDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_AskValueSldDlg::GetInstance() && V3_AskValueSldDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_ChestDlg::GetInstance() && V3_ChestDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_ChestNewDlg::GetInstance() && V3_ChestNewDlg::GetInstance()->IsShown())
      uiNbrShow++;
   if(V3_GuildChestNewDlg::GetInstance() && V3_GuildChestNewDlg::GetInstance()->IsShown())
      uiNbrShow++;
  
   if(uiNbrShow == 0)
   {

      //oki on peu manager le ESCAPE
      //on essaie de detecter la sequence... si inconnu on reaffiche tout...
      //Option 0: TMI Visible, chat log visible et macro visible
         //g_SaveGame.GetOptionsParam()->bDisplayChatLog = true;
         //g_SaveGame.GetOptionsParam()->bDisplayRadar   = true
         //g_SaveGame.GetOptionsParam()->bNotDisplayMacro= false
      //Option 1: TMI Visible, chat log INVISIBLE et macro visible
         //g_SaveGame.GetOptionsParam()->bDisplayChatLog = false;
         //g_SaveGame.GetOptionsParam()->bDisplayRadar   = true
         //g_SaveGame.GetOptionsParam()->bNotDisplayMacro= false
      //Option 2: TMI INVISIBLE, chat log INVISIBLE et macro visible
         //g_SaveGame.GetOptionsParam()->bDisplayChatLog = false;
         //g_SaveGame.GetOptionsParam()->bDisplayRadar   = false
         //g_SaveGame.GetOptionsParam()->bNotDisplayMacro= false
      //Option 3: TMI INVISIBLE, chat log INVISIBLE et macro INVISIBLE
         //g_SaveGame.GetOptionsParam()->bDisplayChatLog = false;
         //g_SaveGame.GetOptionsParam()->bDisplayRadar   = false
         //g_SaveGame.GetOptionsParam()->bNotDisplayMacro= true
      //Option 4: TMI Visible, chat log INVISIBLE et macro INVISIBLE
         //g_SaveGame.GetOptionsParam()->bDisplayChatLog = false;
         //g_SaveGame.GetOptionsParam()->bDisplayRadar   = true
         //g_SaveGame.GetOptionsParam()->bNotDisplayMacro= true


      int iOption     = 0;
      int iOptionNext = 0;
      if(g_SaveGame.GetOptionsParam()->bDisplayChatLog && g_SaveGame.GetOptionsParam()->bDisplayRadar && !g_SaveGame.GetOptionsParam()->bNotDisplayMacro)
      {
         iOption = 0;
         iOptionNext = 1;
      }
      else if(!g_SaveGame.GetOptionsParam()->bDisplayChatLog && g_SaveGame.GetOptionsParam()->bDisplayRadar && !g_SaveGame.GetOptionsParam()->bNotDisplayMacro)
      {
         iOption     = 1;
         iOptionNext = 2;
      }
      else if(!g_SaveGame.GetOptionsParam()->bDisplayChatLog && !g_SaveGame.GetOptionsParam()->bDisplayRadar && !g_SaveGame.GetOptionsParam()->bNotDisplayMacro)
      {
         iOption     = 2;
         iOptionNext = 3;
      }
      else if(!g_SaveGame.GetOptionsParam()->bDisplayChatLog && !g_SaveGame.GetOptionsParam()->bDisplayRadar && g_SaveGame.GetOptionsParam()->bNotDisplayMacro)
      {
         iOption     = 3;
         iOptionNext = 4;
      }
      else if(!g_SaveGame.GetOptionsParam()->bDisplayChatLog && g_SaveGame.GetOptionsParam()->bDisplayRadar && g_SaveGame.GetOptionsParam()->bNotDisplayMacro)
      {
         iOption     = 4;
         iOptionNext = 0;
      }
      else
      {
         iOptionNext = 0;
      }

      //on valide si on a le chat log attacjer a la main barre...
      //on valide si proche de la main barre dans le cendre si oui, on vas attacjer a la main barre
      int iXM1Pos  = g_Global.GetDialogLastMainBarXPos()+108;
      int iXM2Pos  = g_Global.GetDialogLastMainBarXPos()+592;
      int iYMPos   = g_Global.GetDialogLastMainBarYPos()+14;
      int iChatPX  = g_Global.GetDialogLastChatLogXPos();
      int iChatPX2 = iChatPX+g_Global.GetDialogLastChatLogW();
      int iChatPY  = g_Global.GetDialogLastChatLogYPos()+g_Global.GetDialogLastChatLogH();

      bool bLogAttach = FALSE;

      //valide en Y first...
      if(iChatPY > iYMPos-15 && iChatPY < iYMPos+15)
      {
         //oki attacher en Y, on look si un des 2 point est attacher en X
         if(iChatPX >=iXM1Pos && iChatPX+g_Global.GetDialogLastChatLogW() <=iXM2Pos )
            bLogAttach = TRUE;
         if(iChatPX > iXM1Pos-15 && iChatPX < iXM1Pos+15)
             bLogAttach = TRUE;
         else if(iChatPX2 > iXM2Pos-15 && iChatPX2 < iXM2Pos+15)
             bLogAttach = TRUE;
      }

      BYTE chOldmacroStatus = g_SaveGame.GetOptionsParam()->bNotDisplayMacro;

      switch(iOptionNext)
      {
         case 0:
         {
            g_SaveGame.GetOptionsParam()->bDisplayChatLog = true;
            g_SaveGame.GetOptionsParam()->bDisplayRadar   = true;
            g_SaveGame.GetOptionsParam()->bNotDisplayMacro= false;
         }break;
         case 1:
         {
            g_SaveGame.GetOptionsParam()->bDisplayChatLog = false;
            g_SaveGame.GetOptionsParam()->bDisplayRadar   = true;
            g_SaveGame.GetOptionsParam()->bNotDisplayMacro= false;
         }break;
         case 2:
         {
            g_SaveGame.GetOptionsParam()->bDisplayChatLog = false;
            g_SaveGame.GetOptionsParam()->bDisplayRadar   = false;
            g_SaveGame.GetOptionsParam()->bNotDisplayMacro= false;
         }break;
         case 3: 
         {
            g_SaveGame.GetOptionsParam()->bDisplayChatLog = false;
            g_SaveGame.GetOptionsParam()->bDisplayRadar   = false;
            g_SaveGame.GetOptionsParam()->bNotDisplayMacro= true;
         }break;
         case 4:
         {
            g_SaveGame.GetOptionsParam()->bDisplayChatLog = false;
            g_SaveGame.GetOptionsParam()->bDisplayRadar   = true;
            g_SaveGame.GetOptionsParam()->bNotDisplayMacro= true;
         }break;

      }

      if(g_SaveGame.GetOptionsParam()->bDisplayChatLog)
         V3_ChatLogDlg::GetInstance()->Display(NULL);
      else
         V3_ChatLogDlg::GetInstance()->Hide();

      if(g_SaveGame.GetOptionsParam()->bDisplayRadar)
         V3_TMIDlg::GetInstance()->Show();
      else
         V3_TMIDlg::GetInstance()->Hide();

      //on look si le chat log est attacher si oui on le change de position lui aussi...
      if(chOldmacroStatus != g_SaveGame.GetOptionsParam()->bNotDisplayMacro)
      {
         if(bLogAttach)
         {
            int iNewY = g_Global.GetDialogLastChatLogYPos();
            if(!chOldmacroStatus)
               iNewY+=47;
            else
               iNewY-=47;
            g_Global.SetDialogLastChatLogXYPos(g_Global.GetDialogLastChatLogXPos(),iNewY,g_Global.GetDialogLastChatLogW(),g_Global.GetDialogLastChatLogH());
            V3_ChatLogDlg::GetInstance()->Display(NULL);
         }
      }
      V3_MainBarDlg::GetInstance()->Display(NULL);
  

      OutputDebugString("receive ESACPE OK\n");

      return true;
   }
   OutputDebugString("receive ESACPE FALSE\n");
   return false;
}

// Virtual key inputs
bool V3_MainBarDlg::VKeyInput(VKey vKey)
{
   if( vKey.ALT() )
   {
      return false;
   }
   if( vKey.CTRL() )
   {
      switch (vKey.GetKey()) 
      {
         case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
         break;
         default:
         return false;
      }
   }

   switch( vKey.GetKey() )
   {
      case VK_ESCAPE:
      break;
      case VK_UP:
         if (vKey.CTRL()) 
         {
            if (ii != rollbackTyped.begin()) 
            {
               ii--;
               m_EditInput.SetText(*ii);
            }
         } 
      break;
      case VK_DOWN:
         if (vKey.CTRL()) 
         {
            try 
            {
               if (ii == rollbackTyped.end())
                  m_EditInput.SetText("");
               else 
               {
                  ii++;
                  if (ii == rollbackTyped.end()) 
                     m_EditInput.SetText("");
                  else 
                     m_EditInput.SetText(*ii);
               }
            } 
            catch (...) 
            {
            
            }
         }
         break;
         
      case VK_LEFT:
         if (vKey.CTRL()) 
         {
            m_EditInput.MoveCursorPos(-1);
         } 
         else if (vKey.SHIFT()) 
         {
            m_EditInput.MoveCursorPos(-1,1);
         }
      break;
      case VK_RIGHT:
         if (vKey.CTRL()) 
         {
            m_EditInput.MoveCursorPos(1);
         } 
         else if (vKey.SHIFT()) 
         {
            m_EditInput.MoveCursorPos(1,1);
         }
      break;
      case VK_HOME:
         if (vKey.SHIFT()) 
         {
            m_EditInput.MoveCursorPos(-10000);
         }
      break;
      case VK_END:
         if (vKey.SHIFT()) 
         {
            m_EditInput.MoveCursorPos(10000);
         }
      break;
      case VK_DELETE:
         if (vKey.SHIFT()) 
         {
            m_EditInput.SetText("");
         }
         else
         {
            m_EditInput.DeleteChar();
         }
      break;
   }

   m_EditInput.SetParent( this );
   return true;
}


void V3_MainBarDlg::ComboReturn(UINT uiShowCodeP,char *pStrValue)
{
   m_strLastCCSelected = pStrValue;
   m_stCCname.SetText(m_strLastCCSelected,g_DefColorD);
}

void V3_MainBarDlg::SmileReturn(UINT uiSmileID)
{
   TFCPacket sending;
   sending << (short)RQ_UpdateSmile;
   sending << (short)uiSmileID;
   SEND_PACKET(sending);
}

void V3_MainBarDlg::ForceSetSendCC()
{
   m_TextInputState = SendToPage;
   
   // Hide the channel toggle button.
   m_BtnDD.Hide();
   m_BtnCC.Hide();
   m_BtnCB.Hide();
   m_stCCname.Hide();

   // Show the page and graph patch.
   m_BtnWW.Show();
   m_InputPatch.Show();
   m_EditPageInput.Show();
   m_EditInput.SetParent( this );
   
   UpdateAllFramePos(m_iXPos,m_iYPos);

   lastClickedItem = &m_EditInput;
}

//  Sends a message to the currently selected channel
void V3_MainBarDlg::SendMessageToCurrentChannel(const string &msg)
{
   if( !m_strLastCCSelected.empty() )
   {
      TFCPacket sending;
      // Create a send chatter message packet.
      sending << (RQ_SIZE)RQ_SendChatterChannelMessage;
      sending << (short)m_strLastCCSelected.size();
      sending << (char *)m_strLastCCSelected.c_str();
      sending << (short)msg.size();
      sending << (char *)msg.c_str();
      
      SEND_PACKET( sending );
   }
   else
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 487 ],SYSTEM_COLOR,false);
   }
}

void V3_MainBarDlg::ShowQuitWindows()
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   V3_ConfirmDlg::GetInstance()->SetDialogText(g_GUILocalString[83]);
   V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
   V3_ConfirmDlg::GetInstance()->SetParent(this);
   V3_ConfirmDlg::GetInstance()->SetXYPos(((g_Global.GetScreenW()-g_AskValueDlgW)/2), ((g_Global.GetScreenH()-g_AskValueDlgH)/2));
   V3_ConfirmDlg::GetInstance()->Show(0);
   
   RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
}

void V3_MainBarDlg::UpdateMacroBtn()
{
   for(int y=0;y<g_iNbrMacroY;y++)
   {
      for(int x=0;x<g_iNbrMacroX;x++)
      {
         m_MacroButton[y][x].macroItem = V3_MacroDlg::GetInstance()->GetMacroItemUI(x,y);

         if(m_MacroButton[y][x].macroItem)
            m_MacroButton[y][x].SetHelpText(FontManager::GetInstance()->GetSmallFont( m_MacroButton[y][x].macroItem->macroName.c_str() ));
         else
            m_MacroButton[y][x].SetHelpText(FontManager::GetInstance()->GetSmallFont( "" ));
      }
   }
   
}

void V3_MainBarDlg::MPUpEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = V3_MainBarDlg::GetInstance();
   
   me->m_iCurrentMP--;
   if(me->m_iCurrentMP < 0)
      me->m_iCurrentMP = g_iNbrMacroY-1;
      
   for(int y=0;y<g_iNbrMacroY;y++)
   {
      for(int x=0;x<g_iNbrMacroX;x++)
      {
         if(y == me->m_iCurrentMP)
            me->m_MacroButton[y][x].Show();
         else
            me->m_MacroButton[y][x].Hide();
      }
   }
}

void V3_MainBarDlg::MPDnEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = V3_MainBarDlg::GetInstance();
   
   me->m_iCurrentMP++;
   if(me->m_iCurrentMP >= g_iNbrMacroY)
      me->m_iCurrentMP = 0;
      
      
   for(int y=0;y<g_iNbrMacroY;y++)
   {
      for(int x=0;x<g_iNbrMacroX;x++)
      {
         if(y == me->m_iCurrentMP)
            me->m_MacroButton[y][x].Show();
         else
            me->m_MacroButton[y][x].Hide();
      }
   }
}







V3_MainBarDlg::MacroButtonUI::MacroButtonUI( void )
: macroItem( NULL )
{
}


//////////////////////////////////////////////////////////////////////////////////////////
bool V3_MainBarDlg::MacroButtonUI::LeftMouseDown(MousePos mousePos)
{
   V3_MainBarDlg *me = V3_MainBarDlg::GetInstance();
   
   if(!g_SaveGame.GetOptionsParam()->bMacroOnClick)
   {
      if( IsDblClick())
      {
         if( macroItem == NULL )
            return true;
      
         macroItem->macroEvent->ExecMacro( macroItem->macroUI );// Call the macro visitor
      }
   }
   /*
   else
   {
      //if( IsClick())
      {
         if( macroItem == NULL )
            return true;
         
         macroItem->macroEvent->ExecMacro( macroItem->macroUI );// Call the macro visitor
      }
   }
   */
   return true;
} 

bool V3_MainBarDlg::MacroButtonUI::LeftMouseUp( MousePos mousePos )
{
   V3_MainBarDlg *me = V3_MainBarDlg::GetInstance();
   
   if(g_SaveGame.GetOptionsParam()->bMacroOnClick)
   {
      //if( IsClick())
      {
         if( macroItem == NULL )
            return true;
         
         macroItem->macroEvent->ExecMacro( macroItem->macroUI );// Call the macro visitor
      }
   }
   return true;
} 



void V3_MainBarDlg::StartDragDlg( MousePos mousePos )
{
   if(!g_SaveGame.GetOptionsParam()->bGUIDynamic)
      return;

   if(!m_bDragDialog)
   {
      //si premier start drag, on prend la position de depart du drag
      //pour en calculer le deltat X et Y du deplacement de la fenetre...
      {
         
         RootBoxUI::GetInstance()->SetDragUI((GameUI*)&Drag_ButtonL);
         g_Global.SetDraggingDialog(true);

         m_bDragDialog = true;
         m_iXPosSD = mousePos.x;
      }
   }
}

void V3_MainBarDlg::StopDragDlg()
{
   if(m_bDragDialog)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      m_iXPos = m_iXPos + (mousePos.x-m_iXPosSD);
      
      if(m_iXPos < 0)
         m_iXPos = 0;
      
      if(m_iXPos+g_MainBarDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_MainBarDlgW;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastMainBarXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_MainBarDlg::DragEvent::LeftClicked( void )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_MainBarDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_MainBarDlg *me = static_cast< V3_MainBarDlg * >( GetUI() );
   
   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}

void V3_MainBarDlg::AskCompagnonName()
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,g_GUILocalString[99] ,"",20,737);
   V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueDlg::GetInstance()->SetParent(this);
   V3_AskValueDlg::GetInstance()->SetXYPos(((g_Global.GetScreenW()-g_AskValueDlgW)/2), ((g_Global.GetScreenH()-g_AskValueDlgH)/2));
   V3_AskValueDlg::GetInstance()->Show(0);

   RequestForegroundControl( V3_AskValueDlg::GetInstance() );
}