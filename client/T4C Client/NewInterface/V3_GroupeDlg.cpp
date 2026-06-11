// V3_GroupeDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_GroupeDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "..\Bitmap.h"

#include "V3_StatDlg.h"
#include "V3_SpellDlg.h"
#include "V3_OptionsDlg.h"
#include "V3_MacroDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_ChatDlg.h"
#include "V3_GuildDlg.h"
#include "V3_ProfessionDlg.h"
#include "V3_InvDlg.h"

extern Bitmap GridID;
extern DWORD TargetID;
extern CSaveGame g_SaveGame;
DWORD g_dwLastMacroCallGroup = 0;

namespace
{
   const COLORREF g_NearMemberColor = CL_WHITE;
   const COLORREF g_NearLeaderColor = CL_CYAN_LIGHT;
   const COLORREF g_FarMemberColor  = CL_GRAY;
   const COLORREF g_FarLeaderColor  = CL_CYAN;
};

namespace{
   T3VSBSound *naSound = NULL;
   
   void InitSound(){
      if( naSound == NULL ){
         naSound = new T3VSBSound;
         naSound->Create( "General NA sound", TS_MEMORY );
      }
   }
   
   
   enum TargetPriority{
      monsterPriority = 0,
         equalPriority   = 1,
         playerPriority  = 2
   };
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_GroupeDlg::V3_GroupeDlg():
m_iMemberSize (0),
m_bAutoShare( false ),    
m_bWeAreTheLeader( false ),
X_Button( hideEvent ),
Drag_Button(dragEvent),
MOnglet01Button(tabStatsEvent            ,g_GUILocalString[309],g_DefColorH ),
MOnglet02Button(tabInvEvent              ,g_GUILocalString[310],g_DefColorH ),
MOnglet03Button(tabSpellEvent            ,g_GUILocalString[311],g_DefColorH ),
MOnglet04Button(tabProfessionEvent       ,g_GUILocalString[312],g_DefColorH ),
MOnglet05Button(tabGuildeEvent           ,g_GUILocalString[313],g_DefColorH ),
MOnglet06Button(EmptyEvent::GetInstance(),g_GUILocalString[314],g_DefColorH ),
MOnglet07Button(tabMacroEvent            ,g_GUILocalString[315],g_DefColorH ),
MOnglet08Button(tabOptionEvent           ,g_GUILocalString[316],g_DefColorH ),
m_bDragDialog (false),
m_ListMember( EmptyEvent::GetInstance() ),
m_btnInvite   (inviteButtonEvent,g_GUILocalString[116],g_DefColorH),
m_btnQuitter  (leaveButtonEvent,g_GUILocalString[117],g_DefColorH),
m_btnJeter    (expelButtonEvent,g_GUILocalString[118],g_DefColorH),
m_btnSeparer  (autoShareButtonEvent,g_GUILocalString[119],g_DefColorH),
m_btnInviteD  (EmptyEvent::GetInstance(),g_GUILocalString[116],g_DefColorH),
m_btnQuitterD (EmptyEvent::GetInstance(),g_GUILocalString[117],g_DefColorH),
m_btnJeterD   (EmptyEvent::GetInstance(),g_GUILocalString[118],g_DefColorH),
m_btnSeparerD (EmptyEvent::GetInstance(),g_GUILocalString[119],g_DefColorH)

{ 
   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_GroupeBack",true,"V3_GroupeBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_GuildIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
   
  

   MOnglet01Button.SetHighlightUI( "V3_MainOnglet_77p_H");
   MOnglet02Button.SetHighlightUI( "V3_MainOnglet_77p_H");
   MOnglet03Button.SetHighlightUI( "V3_MainOnglet_48p_H");
   MOnglet04Button.SetHighlightUI( "V3_MainOnglet_77p_H");
   MOnglet05Button.SetHighlightUI( "V3_MainOnglet_50p_H");
   MOnglet06Button.SetHighlightUI( "V3_MainOnglet_50p_H");
   MOnglet07Button.SetHighlightUI( "V3_MainOnglet_50p_H");
   MOnglet08Button.SetHighlightUI( "V3_MainOnglet_50p_H");

   tabStatsEvent     .SetUI(this);
   tabInvEvent       .SetUI(this);
   tabSpellEvent     .SetUI(this);
   tabProfessionEvent.SetUI(this);
   tabGuildeEvent    .SetUI(this);
   //tabGroupEvent     .SetUI(this);
   tabMacroEvent     .SetUI(this);
   tabOptionEvent    .SetUI(this);


   // Member list.
   m_ListMember.SetupList( 3, 20, FramePos( 0, 0, 2, 2 ) );
   m_ListMember.SetColumnSize( 0, 157 );
   m_ListMember.SetColumnSize( 1, 105 );
   m_ListMember.SetColumnSize( 2, 80 );
   m_ListMember.CenterColumn( 2 );
   memberSelectionGraphPatch.SetSpriteId( "V3_GroupeListSelect" );
   m_ListMember.SetSelectionSprite( &memberSelectionGraphPatch, -3, 0 );
   SetHelpId( m_ListMember, 454 );
   m_ListMember.HideTick();


   inviteButtonEvent.SetUI( this );
   leaveButtonEvent.SetUI( this );
   expelButtonEvent.SetUI( this );
   autoShareButtonEvent.SetUI( this );

   m_btnInvite.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnInvite.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnInvite.SetHighlightUI( "V3_Btn72x24_H" );
   m_btnInviteD.SetSpriteId( "V3_Btn72x24_D" );
   SetHelpId( m_btnInvite, 324 );
   SetHelpId( m_btnInviteD, 324 );

   m_btnQuitter.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnQuitter.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnQuitter.SetHighlightUI( "V3_Btn72x24_H" );
   m_btnQuitterD.SetSpriteId( "V3_Btn72x24_D" );
   SetHelpId( m_btnQuitter, 325 );
   SetHelpId( m_btnQuitterD, 325 );

   m_btnJeter.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnJeter.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnJeter.SetHighlightUI( "V3_Btn72x24_H" );
   m_btnJeterD.SetSpriteId( "V3_Btn72x24_D" );
   SetHelpId( m_btnJeter, 326 );
   SetHelpId( m_btnJeterD, 326 );

   m_btnSeparer.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnSeparer.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnSeparer.SetHighlightUI( "V3_Btn72x24_H" );
   m_btnSeparerD.SetSpriteId( "V3_Btn72x24_D" );
   SetHelpId( m_btnSeparer, 327 );
   SetHelpId( m_btnSeparerD, 327 );


   m_BtnSplitDisabled.SetSpriteId("V3_Btn72x24_Off");
   m_bShowDisableSplit = FALSE;

   m_strTitre[0].SetText(g_GUILocalString[39],g_DefColorH);
   m_strTitre[1].SetText(g_GUILocalString[40],g_DefColorH);
   m_strTitre[2].SetText(g_GUILocalString[120],g_DefColorH);
   
   
   

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_ListMember );
   AddChild( &m_btnInviteD  );
   AddChild( &m_btnQuitterD );
   AddChild( &m_btnJeterD   );  
   AddChild( &m_btnSeparerD ); 
   AddChild( &m_btnInvite   );
   AddChild( &m_btnQuitter  );
   AddChild( &m_btnJeter    ); 
   AddChild( &m_btnSeparer  ); 
   AddChild( &MOnglet01Button );
   AddChild( &MOnglet02Button );
   AddChild( &MOnglet03Button );
   AddChild( &MOnglet04Button );
   AddChild( &MOnglet05Button );
   AddChild( &MOnglet06Button );
   AddChild( &MOnglet07Button );
   AddChild( &MOnglet08Button );

   AddChild( &m_strTitre[0] );
   AddChild( &m_strTitre[1] );
   AddChild( &m_strTitre[2] );
   
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   ResolveGraphPatches(); 

} 

V3_GroupeDlg::~V3_GroupeDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_GroupeDlg *V3_GroupeDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_GroupeDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_GroupeDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallGroup > 500)
      {
         g_dwLastMacroCallGroup =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}

unsigned int V3_GroupeDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   g_Global.SetLastDialogOpen(5);
   
   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->Show();
   return 0;
}

void V3_GroupeDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   //les 8 onglet
   MOnglet01Button.SetPos ( FramePos( iPosX+33  , iPosY+63  , iPosX+109, iPosY+83 ) );
   MOnglet02Button.SetPos ( FramePos( iPosX+112 , iPosY+63  , iPosX+188, iPosY+83 ) );
   MOnglet03Button.SetPos ( FramePos( iPosX+191 , iPosY+63  , iPosX+236, iPosY+83 ) );
   MOnglet04Button.SetPos ( FramePos( iPosX+239 , iPosY+63  , iPosX+315, iPosY+83 ) );
   MOnglet05Button.SetPos ( FramePos( iPosX+318 , iPosY+63  , iPosX+367, iPosY+83 ) );
   MOnglet06Button.SetPos ( FramePos( iPosX+370 , iPosY+63  , iPosX+419, iPosY+83 ) );
   MOnglet07Button.SetPos ( FramePos( iPosX+422 , iPosY+63  , iPosX+471, iPosY+83 ) );
   MOnglet08Button.SetPos ( FramePos( iPosX+474 , iPosY+63  , iPosX+523, iPosY+83 ) );

   m_ListMember.SetPos( FramePos( iPosX+133, iPosY+149, iPosX+478, iPosY+322 ) );    

   m_btnInvite .SetPos( FramePos( iPosX+124, iPosY+345, iPosX+124+72, iPosY+345+24 ) );
   m_btnInviteD.SetPos( FramePos( iPosX+124, iPosY+345, iPosX+124+72, iPosY+345+24 ) );

   m_btnQuitter .SetPos( FramePos( iPosX+220, iPosY+345, iPosX+220+72, iPosY+345+24 ) );
   m_btnQuitterD.SetPos( FramePos( iPosX+220, iPosY+345, iPosX+220+72, iPosY+345+24 ) );

   m_btnJeter .SetPos( FramePos( iPosX+317, iPosY+345, iPosX+317+72, iPosY+345+24 ) );
   m_btnJeterD.SetPos( FramePos( iPosX+317, iPosY+345, iPosX+317+72, iPosY+345+24 ) );

   m_btnSeparer .SetPos( FramePos( iPosX+413, iPosY+345, iPosX+413+72, iPosY+345+24 ) );
   m_btnSeparerD.SetPos( FramePos( iPosX+413, iPosY+345, iPosX+413+72, iPosY+345+24 ) );
   m_BtnSplitDisabled.SetPos( FramePos( iPosX+413, iPosY+345, iPosX+413+72, iPosY+345+24 ) );

   m_strTitre[0].SetPos( FramePos( iPosX+131, iPosY+120, iPosX+288, iPosY+137 ) );
   m_strTitre[1].SetPos( FramePos( iPosX+293, iPosY+120, iPosX+395, iPosY+137 ) );
   m_strTitre[2].SetPos( FramePos( iPosX+400, iPosY+120, iPosX+477, iPosY+137 ) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_GroupeDlg::Draw(V2SPRITEFX *vsfFX)
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
      
      if(iNewPosX+g_OptionDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_OptionDlgW;
      if(iNewPosY+g_OptionDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_OptionDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   GameUI::Draw( vsfFX );

   if(m_bShowDisableSplit)
      m_BtnSplitDisabled.Draw(vsfFX);

   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   MOnglet01Button.DrawTextButtonExt(hdc);
   MOnglet02Button.DrawTextButtonExt(hdc);
   MOnglet03Button.DrawTextButtonExt(hdc);
   MOnglet04Button.DrawTextButtonExt(hdc);
   MOnglet05Button.DrawTextButtonExt(hdc);
   MOnglet06Button.DrawTextButtonExt(hdc);
   MOnglet07Button.DrawTextButtonExt(hdc);
   MOnglet08Button.DrawTextButtonExt(hdc);
   m_btnInvite   .DrawTextButtonExt(hdc);
   m_btnQuitter  .DrawTextButtonExt(hdc);
   m_btnJeter    .DrawTextButtonExt(hdc);
   m_btnSeparer  .DrawTextButtonExt(hdc);
   m_btnInviteD  .DrawTextButtonExt(hdc);
   m_btnQuitterD .DrawTextButtonExt(hdc);
   m_btnJeterD   .DrawTextButtonExt(hdc);
   m_btnSeparerD .DrawTextButtonExt(hdc);
   
   // Draw les texte...
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[314],strlen(g_GUILocalString[314]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}


void V3_GroupeDlg::Hide()
{
    BoxUI::Hide();
}


void V3_GroupeDlg::Join(DWORD sourceID,string sourceName)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   // If there is already a box in foreground control.
   if( IsForegroundControlled() )
   {
      // Automatically decline
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_GroupLeave;
      SEND_PACKET( sending );
   }
   else
   {    
      // If the user was ignored.
      if(V3_ChatDlg::GetInstance()->IsUserIgnored( sourceName )) //invite groupe
      {
         // Decline.
         TFCPacket sending;
         sending << (RQ_SIZE)RQ_GroupLeave;
         SEND_PACKET( sending );
      }
      else
      {
         char buf[ 1024 ];
         sprintf_s( buf,1024, g_LocalString[ 354 ], sourceName.c_str() );


         V3_ConfirmDlg::GetInstance()->SetDialogText(buf);
         V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[10],g_GUILocalString[11],738,739);
         V3_ConfirmDlg::GetInstance()->SetParent(this);
         V3_ConfirmDlg::GetInstance()->SetXYPos(((g_Global.GetScreenW()-g_AskValueDlgW)/2), ((g_Global.GetScreenH()-g_AskValueDlgH)/2));
         V3_ConfirmDlg::GetInstance()->Show(0);
         
         RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
      }
      
   }
}

void V3_GroupeDlg::UpdateMembers(const list< GroupMember > &members,bool newShare, bool bDisband)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   if(bDisband && m_ListMember.GetGrid()->size() > 0)
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 519 ], SYSTEM_COLOR, true );
   }
 
   m_ListMember.DeleteList();
   list< GroupMember >::const_iterator i;
   m_bWeAreTheLeader = false;
   m_bAutoShare      = newShare;
   m_iMemberSize     = members.size();
   
   for( i = members.begin(); i != members.end(); i++ )
   {
      m_ListMember.AddNewItem();
      if( (*i).leader && (*i).id == Player.ID )
         m_bWeAreTheLeader = true;
      
      // Add the user name.
      {
         TextMemberUI *memberUI = new TextMemberUI;
         if( (*i).leader )
            memberUI->SetText( (*i).name.c_str(), g_NearLeaderColor, g_Global.GetScreenW(), 0, 0 );
         else
            memberUI->SetText( (*i).name.c_str(), g_NearMemberColor, g_Global.GetScreenW(), 0, 0 );
         memberUI->SetCenter( false );
         memberUI->SetPos( FramePos( 0, 0, memberUI->GetWidth(), memberUI->GetHeight() ) );
         memberUI->SetMember( (*i) );
         SetHelpId( memberUI, 501 );
         m_ListMember.AddColumnItem( 0, memberUI );
      }

      // Add the user HP graph bar.
      {
         HpMemberUI *hpUI = new HpMemberUI;
         hpUI->SetPos( FramePos( 0, 0, 105, 16 ) );
         hpUI->SetMember( *i );
         m_ListMember.AddColumnItem( 1, hpUI );
      }
      
      // Add the user level.
      {
         TextMemberUI *memberUI = new TextMemberUI;
         
         char buf[ 256 ];
		 _itoa_s( (*i).level, buf,256, 10 );
         
         if( (*i).leader )
            memberUI->SetText( buf, g_NearLeaderColor, 640, 0, 0 );
         else
            memberUI->SetText( buf, g_NearMemberColor, 640, 0, 0 );
         memberUI->SetCenter( false );
         memberUI->SetPos( FramePos( 0, 0, memberUI->GetWidth(), memberUI->GetHeight() ) );        
         memberUI->SetMember( *i );
         
         m_ListMember.AddColumnItem( 2, memberUI );
      }
   }
   
   ResolveGraphPatches();    
}


void V3_GroupeDlg::UpdateMemberHp(DWORD id,WORD hpPercent)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   // Get the list's grid
   vector< vector< ListUI::GridItem > > *groupGrid = m_ListMember.GetGrid();
   
   // Scroll through all the graph hp bars in the list.
   UINT i;
   for( i = 0; i != (*groupGrid)[ 1 ].size(); i++ )
   {
      // Get the HpMemberUI.
      HpMemberUI *hpUI = static_cast< HpMemberUI * >( (*groupGrid)[ 1 ][ i ] );
      
      // If this is the player we are looking for.
      if( hpUI->id == id )
      {
         // Update its hp percentage.
         hpUI->hpPercent = hpPercent;
      }
   }
}


//////////////////////////////////////////////////////////////////////////////////////////
//  Updates the auto-split status of the group.
//
//////////////////////////////////////////////////////////////////////////////////////////
void V3_GroupeDlg::UpdateAutoShare(bool newShare)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   m_bAutoShare = newShare;
   
   if( m_bAutoShare )
      V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 523 ],SYSTEM_COLOR,true);
   else
      V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 524 ],SYSTEM_COLOR,true);
   
   ResolveGraphPatches();
}

//////////////////////////////////////////////////////////////////////////////////////////
//  This functions simply hide or shows the right graph patches/controls
// 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_GroupeDlg::ResolveGraphPatches( void )
{
   if( m_bWeAreTheLeader )
   {
      // Enable leader controls.
      m_btnQuitter.Show();
      m_btnJeter  .Show();
      m_btnInvite .Show();
      m_btnSeparer.Show();
      
      if( m_bAutoShare )
      {
         m_bShowDisableSplit = FALSE;
      }
      else
      {
         m_bShowDisableSplit = TRUE;
      }
      
      m_btnQuitterD.Hide();
      m_btnJeterD.Hide();
      m_btnInviteD.Hide();
      m_btnSeparerD.Hide();
   }
   else
   {
      // If there are members in the group.
      if( m_iMemberSize != 0 )
      {
         // Enable member functions.
         m_btnQuitter.Show();
         m_btnJeter  .Hide();
         m_btnInvite .Hide();
         
         if( m_bAutoShare )
         {
            m_btnSeparerD.Show();
            m_btnSeparer.Hide();
            m_bShowDisableSplit = FALSE;
         }
         else
         {
            m_btnSeparerD.Hide();
            m_btnSeparer.Show();
            m_bShowDisableSplit = TRUE;
         }                
         m_btnQuitterD.Hide();
         m_btnJeterD  .Show();
         m_btnInviteD .Show();            
      }
      else
      {
         // Enable non-group functions.
         m_btnQuitter.Hide();
         m_btnJeter.Hide();
         m_btnInvite.Show();
         m_btnSeparer.Hide();
         
         m_btnQuitterD.Show();
         m_btnJeterD.Show();
         m_btnInviteD.Hide();
         m_btnSeparerD.Show();
         m_bShowDisableSplit = TRUE;
      }
   }
}








void V3_GroupeDlg::StartDragDlg( MousePos mousePos )
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

void V3_GroupeDlg::StopDragDlg()
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
      
      if(m_iXPos+g_OptionDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_OptionDlgW;
      if(m_iYPos+g_OptionDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_OptionDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_GroupeDlg::DragEvent::LeftClicked( void )
{
   V3_GroupeDlg *me = static_cast< V3_GroupeDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_GroupeDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_GroupeDlg *me = static_cast< V3_GroupeDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}






void V3_GroupeDlg::TabStatsEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
  
   V3_StatsDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GroupeDlg::TabInvEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   V3_InvDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GroupeDlg::TabSpellEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   V3_SpellDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GroupeDlg::TabProfessionEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );

   V3_ProfessionDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GroupeDlg::TabGuildeEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   V3_GuildDlg::GetInstance()->Display(NULL);
   me->Hide();
}

/*
// void V3_GroupeDlg::TabGroupEvent::LeftClicked( void )
// {
//    V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
//    
// }
*/

void V3_GroupeDlg::TabMacroEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   V3_MacroDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GroupeDlg::TabOptionEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   V3_OptionsDlg::GetInstance()->Display(NULL);
   me->Hide(); 
}







//  Invite button got pressed.
void V3_GroupeDlg::InviteButtonEvent::LeftClicked( void )
{
   V3_GroupeDlg *me = static_cast< V3_GroupeDlg * >( GetUI() );
   
   MouseDevice md;
   if( md.LockNextEvent( DM_CLICK, me, InvitePlayer, 0 ) )
   {
      me->Hide();
   }
   else
   {
      // Could not lock mouse.
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 444 ], SYSTEM_COLOR, true);
   }
}

// Called when the mouse event has selected a player.
unsigned int V3_GroupeDlg::InvitePlayer(void *data)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   V3_GroupeDlg *me = static_cast< V3_GroupeDlg * >( data );
   
   POINT pntMousePos;
   POINT pntWorldPos;
   
   // Get the mouse pos.
   MouseDevice md;
   md.GetPosition( (int *)&pntMousePos.x, (int *)&pntMousePos.y );
   
   Player.ScreenPosToWL( &pntMousePos, &pntWorldPos );
   
   int nX = ( pntMousePos.x + 48 ) / 32;
   int nY = ( pntMousePos.y - 8 ) / 16;
   
   DWORD dwID = GridID( nX, nY );
   // If found an ID.
   if( dwID != 0 )
   {
      signed char xPos, yPos;
      Objects.Lock(101);
      Objects.Found( &xPos, &yPos, dwID );
      Objects.Unlock(101);
      
      TFCPacket sending;
      // Send an invitation to the user.
      sending << (RQ_SIZE)RQ_GroupInvite;
      sending << (long)dwID;
      sending << (short)( xPos + Player.xPos );
      sending << (short)( yPos + Player.yPos );        
      
      SEND_PACKET( sending );
   }
   
   me->Show();
   
   return 0;
}

//  Leave button got pressed.
void V3_GroupeDlg::LeaveButtonEvent::LeftClicked( void )
{
   // Send a GroupLeave request.
   TFCPacket sending;
   sending << (RQ_SIZE)RQ_GroupLeave;
   SEND_PACKET( sending );
}

//  Expel button got clicked.
void V3_GroupeDlg::ExpelButtonEvent::LeftClicked( void )
{
   V3_GroupeDlg *me = static_cast< V3_GroupeDlg * >( GetUI() );
   
   // All stocked items are GroupMembers by multiple inheritance.
   TextMemberUI *member = static_cast< TextMemberUI * >( me->m_ListMember.GetSelection() );
   
   // If no selection.
   if( member == NULL )
   {
      return;
   }
   
   TFCPacket sending;
   // Send a group kick packet.
   sending << (RQ_SIZE)RQ_GroupKick;
   sending << (long)member->id;
   SEND_PACKET( sending );
}

//  Clicked on the autoshare button.
void V3_GroupeDlg::AutoShareButtonEvent::LeftClicked( void )
{
   V3_GroupeDlg *me = static_cast< V3_GroupeDlg * >( GetUI() );
   
  
   TFCPacket sending;
   sending << (RQ_SIZE)RQ_GroupToggleAutoSplit;
   sending << (char)( me->m_bAutoShare ? 0 : 1 );
   SEND_PACKET( sending );
}


//////////////////////////////////////////////////////////////////////////////////////////
//  Draws the text member.
// 
//////////////////////////////////////////////////////////////////////////////////////////
void TextMemberUI::Draw(V2SPRITEFX *vsfFX )
{
   Objects.Lock(143);
   // If the user is within range.
   if( Objects.FoundID( id ) || id == Player.ID )
   {
      Objects.Unlock(143);
      
      // Set its text. The function should be very fast as it shouldn't do
      // anything else than a couple of tests if the text and color didn't change.
      if( leader )
         SetText( GetText()->GetText(), g_NearLeaderColor, g_Global.GetScreenW(), 0, 0 );
      else
         SetText( GetText()->GetText(), g_NearMemberColor, g_Global.GetScreenW(), 0, 0 );
   }
   else
   {
      Objects.Unlock(143);
      
      if( leader )
         SetText( GetText()->GetText(), g_FarLeaderColor, g_Global.GetScreenW(), 0, 0 );
      else
         SetText( GetText()->GetText(), g_FarMemberColor, g_Global.GetScreenW(), 0, 0 );
   }
   StaticTextUI::Draw( vsfFX );    
}


//////////////////////////////////////////////////////////////////////////////////////////
HpMemberUI::HpMemberUI( void )
{
   fullBar.SetSpriteId( "V3_GroupePV" );
   emptyBar.SetSpriteId( "V3_GroupePVBack" );
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Draws a member's HP.
//  
//////////////////////////////////////////////////////////////////////////////////////////
void HpMemberUI::Draw(V2SPRITEFX *vsfFX)
{ 
   // Display the full bar.
   FramePos emptyPos = GetPos();
   /*
   emptyPos.y +=1;
   emptyPos.maxy -=1;
   emptyPos.x += 2;
   emptyPos.maxx -= 2;
   */
   emptyBar.SetPos( emptyPos );    
   emptyBar.Draw( vsfFX );
   
   fullBar.SetPos( emptyPos );
   
   // Create a clipping area according to the group member's hp.
   long newWidth = emptyPos.Width() * ( 100 - hpPercent ) / 100;
   RECT clipRect = { emptyPos.x, emptyPos.y, emptyPos.maxx - newWidth, emptyPos.maxy };
   V2SPRITEFX fx;
   memset( &fx, 0, sizeof( fx ) );
   fx.lpClipRect = &clipRect;
   fullBar.Draw( &fx );
}

void V3_GroupeDlg::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   switch(uiShowCodeP)
   {
      case 0:
      {
         if(bOK)
         {
            // Send a GroupJoin packet.
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_GroupJoin;
            SEND_PACKET( sending );
         }
         else
         {
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_GroupLeave;
            SEND_PACKET( sending );
         }
      }
      break;
   }
}