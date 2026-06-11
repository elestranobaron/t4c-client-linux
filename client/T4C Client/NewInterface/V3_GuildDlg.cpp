// V3_GuildDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_GuildDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "..\Bitmap.h"

#include "V3_StatDlg.h"
#include "V3_OptionsDlg.h"
#include "V3_SpellDlg.h"
#include "V3_GroupeDlg.h"
#include "V3_ChatDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_ProfessionDlg.h"
#include "V3_InvDlg.h"


extern Bitmap GridID;
extern DWORD TargetID;
extern CSaveGame g_SaveGame;

DWORD g_dwLastMacroCallGuild = 0;

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

V3_GuildDlg::V3_GuildDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
MOnglet01Button(tabStatsEvent            ,g_GUILocalString[309],g_DefColorH ),
MOnglet02Button(tabInvEvent              ,g_GUILocalString[310],g_DefColorH ),
MOnglet03Button(tabSpellEvent            ,g_GUILocalString[311],g_DefColorH ),
MOnglet04Button(tabProfessionEvent       ,g_GUILocalString[312],g_DefColorH ),
MOnglet05Button(EmptyEvent::GetInstance(),g_GUILocalString[313],g_DefColorH ),
MOnglet06Button(tabGroupEvent            ,g_GUILocalString[314],g_DefColorH ),
MOnglet07Button(tabMacroEvent            ,g_GUILocalString[315],g_DefColorH ),
MOnglet08Button(tabOptionEvent           ,g_GUILocalString[316],g_DefColorH ),
m_bDragDialog (false),
m_GuildList   ( guildListEvent),
selectedUser (""),
m_btnInviter(inviteButtonEvent,g_GUILocalString[337],g_DefColorH),
m_btnQuitter(leaveButtonEvent,g_GUILocalString[338],g_DefColorH),
m_btnKick(kickButtonEvent,g_GUILocalString[339],g_DefColorH),
m_btnChangeNote(changeNoteButtonEvent,g_GUILocalString[340],g_DefColorH),
m_ListPerm( permListEvent ),
m_ListTitre( titreListEvent ),
m_btnChangeSettings(changeSettingsButtonEvent,g_GUILocalString[17],g_DefColorH),
m_btnViewLog(viewLogButtonEvent,g_GUILocalString[341],g_DefColorH)
{
   m_strGuildName[0] = 0x00;
   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 602 );
  
   m_BackgroundS.SetSpriteId("V3_GuildBack",true,"V3_GuildBackMask");

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
   //tabGuildeEvent    .SetUI(this);
   tabGroupEvent     .SetUI(this);
   tabMacroEvent     .SetUI(this);
   tabOptionEvent    .SetUI(this);

   m_strGuildName[0]   = 0x00;
   m_strLastNote[0]    = 0x00;
   m_strRangName[0][0] = 0x00;
   m_strRangName[1][0] = 0x00;
   m_strRangName[2][0] = 0x00;
   m_strRangName[3][0] = 0x00;
   m_strRangName[4][0] = 0x00;
   m_dwUserPermission.dwVal = 0;


   m_GuildList.SetupList    ( 2, 20, FramePos( 0, 0, 2, 1 ) );
   m_GuildList.SetColumnSize( 0, 140  );
   m_GuildList.SetColumnSize( 1, 76  );
   m_GuildList.CenterColumn( 1 );
   m_GuildList.SetScrollUpDownUI( &m_GuildListScrollUp );
   m_GuildList.SetScrollDownDownUI( &m_GuildListScrollDown );
   
   m_GuildListingSelect.SetPos( FramePos( 0, 0, 218, 20 ) );
   m_GuildListingSelect.SetSpriteId( "V3_GuildListSelect" );
   m_GuildListingSelect.Show();
   m_GuildList.SetSelectionSprite( &m_GuildListingSelect);
   guildListEvent.SetUI( this );


   m_strNotes.SetCenter( false );
   m_strNotes.SetHelpText( FontManager::GetInstance()->GetSmallFont());
   m_strNotes.SetText( "", g_DefColorD, 445,0,false,2);


   m_btnInviter.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnInviter.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnInviter.SetHighlightUI( "V3_Btn72x24_H" );
   inviteButtonEvent.SetUI( this );
   SetHelpId( m_btnInviter, 611 );

   m_btnQuitter.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnQuitter.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnQuitter.SetHighlightUI( "V3_Btn72x24_H" );
   leaveButtonEvent.SetUI( this );
   SetHelpId( m_btnQuitter, 618 );
   m_btnQuitter.Hide();

   m_btnKick.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnKick.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnKick.SetHighlightUI( "V3_Btn72x24_H" );
   kickButtonEvent.SetUI( this );
   SetHelpId( m_btnKick, 612 );

   m_btnChangeNote.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnChangeNote.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnChangeNote.SetHighlightUI( "V3_Btn72x24_H" );
   changeNoteButtonEvent.SetUI( this );
   SetHelpId( m_btnChangeNote, 614 );

   m_btnChangeSettings.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnChangeSettings.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnChangeSettings.SetHighlightUI( "V3_Btn92x24_H" );
   changeSettingsButtonEvent.SetUI( this );
   m_btnChangeSettings.Hide();

   m_btnViewLog.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnViewLog.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnViewLog.SetHighlightUI( "V3_Btn92x24_H" );
   viewLogButtonEvent.SetUI( this );
   SetHelpId( m_btnViewLog, 615 );
   


   m_strNom    .SetText(g_GUILocalString[  3],g_DefColorH);
   m_strRang   .SetText(g_GUILocalString[249],g_DefColorH);
   m_strSetting.SetText(g_GUILocalString[253],g_DefColorH);
   m_strTitre  .SetText(g_GUILocalString[249],g_DefColorH);

   //Liste des Permission
   m_ListPerm.SetupList( 2, 20, FramePos( 0, 0, 2, 1 ) );
   m_ListPerm.SetColumnSize( 0, 171 );
   m_ListPerm.SetColumnSize( 1, 20  );
   m_ListPerm.CenterColumn( 1 );
   m_ListPerm.HideTick();
   permListEvent.SetUI( this );
   
   
   //Liste des Permission
   m_ListTitre.SetupList( 2, 20, FramePos( 0, 0, 2, 1 ) );
   m_ListTitre.SetColumnSize( 0, 68 );
   m_ListTitre.SetColumnSize( 1, 20  );
   m_ListTitre.CenterColumn( 1 );
   m_ListTitre.HideTick();
   titreListEvent.SetUI( this );
  

   UpdateAllFramePos(m_iXPos,m_iYPos);


  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_GuildList);
   AddChild( &m_btnInviter);
   AddChild( &m_btnQuitter);
   AddChild( &m_btnKick);
   AddChild( &m_btnChangeNote);
   AddChild( &m_btnChangeSettings);
   
   AddChild( &MOnglet01Button );
   AddChild( &MOnglet02Button );
   AddChild( &MOnglet03Button );
   AddChild( &MOnglet04Button );
   AddChild( &MOnglet05Button );
   AddChild( &MOnglet06Button );
   AddChild( &MOnglet07Button );
   AddChild( &MOnglet08Button );

   AddChild( &Drag_Button);
   AddChild( &m_strNotes );
   AddChild( &m_strNom);
   AddChild( &m_strRang);
   AddChild( &m_strSetting);
   AddChild( &m_strTitre);
   AddChild( &m_ListPerm);
   AddChild( &m_ListTitre);
   AddChild( &m_btnViewLog);
   
  
   RootBoxUI::GetInstance()->AddChild( this );

   
   InitializeCriticalSection(&crLock);
} 

V3_GuildDlg::~V3_GuildDlg()
{
   DeleteCriticalSection(&crLock);
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_GuildDlg *V3_GuildDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_GuildDlg instance;
    return &instance;
    
} 

unsigned int V3_GuildDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallGuild > 500)
      {
         g_dwLastMacroCallGuild =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_GuildDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   g_Global.SetLastDialogOpen(4);

   EnterCriticalSection(&GetInstance()->crLock);
   GetInstance()->m_strGuildName[0]   = 0x00;
   GetInstance()->m_strLastNote[0]    = 0x00;
   GetInstance()->m_strRangName[0][0] = 0x00;
   GetInstance()->m_strRangName[1][0] = 0x00;
   GetInstance()->m_strRangName[2][0] = 0x00;
   GetInstance()->m_strRangName[3][0] = 0x00;
   GetInstance()->m_strRangName[4][0] = 0x00;
   GetInstance()->m_dwUserPermission.dwVal = 0;
   GetInstance()->m_GuildList.DeleteList();
   GetInstance()->m_strNotes.SetText( "", g_DefColorD, 445,0,false,2);
   LeaveCriticalSection(&GetInstance()->crLock);


   
   TFCPacket sending;	
   sending << (RQ_SIZE)RQ_NM_GetGuildList;	
   SEND_PACKET( sending );

   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);
   GetInstance()->Show();

   return 0;
}

void V3_GuildDlg::UpdateAllFramePos(int iPosX,int iPosY)
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


   m_GuildList.SetListDef(iPosX,iPosY,20,37,122,276,309,&m_GuildListScrollUp,&m_GuildListScrollDown);
   m_strNotes.SetPos( FramePos( iPosX+42, iPosY+355, iPosX+484, iPosY+392 ) );

   m_strNom.SetPos     ( FramePos( iPosX+38 , iPosY+100, iPosX+175, iPosY+117 ) );
   m_strRang.SetPos    ( FramePos( iPosX+180, iPosY+100, iPosX+253, iPosY+117 ) );
   m_strSetting.SetPos ( FramePos( iPosX+288, iPosY+100, iPosX+476, iPosY+117 ) );
   m_strTitre.SetPos   ( FramePos( iPosX+482, iPosY+100, iPosX+567, iPosY+117 ) );

   m_btnInviter   .SetPos ( FramePos( iPosX+38 , iPosY+317, iPosX+38+72 , iPosY+317+24 ) );
   m_btnQuitter   .SetPos ( FramePos( iPosX+111, iPosY+317, iPosX+111+72, iPosY+317+24 ) );
   m_btnKick      .SetPos ( FramePos( iPosX+184, iPosY+317, iPosX+184+72, iPosY+317+24 ) );
   m_btnChangeNote.SetPos ( FramePos( iPosX+494, iPosY+362, iPosX+494+72, iPosY+362+24 ) );
   m_btnViewLog   .SetPos ( FramePos( iPosX+257, iPosY+404, iPosX+267+92, iPosY+404+24 ) );
   m_btnChangeSettings.SetPos ( FramePos( iPosX+388, iPosY+317, iPosX+388+92, iPosY+317+24 ) );
   

   m_ListPerm .SetPos( FramePos( iPosX+287, iPosY+122, iPosX+477, iPosY+288 ) );    
   m_ListTitre.SetPos( FramePos( iPosX+481, iPosY+122, iPosX+568, iPosY+204 ) );    
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_GuildDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   m_btnQuitter    .Hide();
   m_btnInviter    .Hide();
   m_btnKick       .Hide();
   m_btnChangeNote .Hide();
   m_btnViewLog    .Hide();
   
   if(m_dwUserPermission.dwVal != GUILD_FONDATEUR_RIGHT && strcmp(m_strGuildName,"") != NULL)
      m_btnQuitter.Show(); 
   
   
   if(m_dwUserPermission.Perm.CanSeeLog)
      m_btnViewLog.Show();
   if(m_dwUserPermission.Perm.CanWriteNote)
      m_btnChangeNote.Show(); 
   if(m_dwUserPermission.Perm.CanKick)
      m_btnKick.Show();
   if(m_dwUserPermission.Perm.CanInvite)
      m_btnInviter.Show();

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
   m_btnInviter.DrawTextButtonExt(hdc);
   m_btnQuitter.DrawTextButtonExt(hdc);
   m_btnKick.DrawTextButtonExt(hdc);
   m_btnChangeNote.DrawTextButtonExt(hdc);
   m_btnChangeSettings.DrawTextButtonExt(hdc);
   m_btnViewLog.DrawTextButtonExt(hdc);
   
   // Draw les texte...
   char strTitle[256];
   sprintf_s(strTitle,256  ,"%s - %s",g_GUILocalString[248],m_strGuildName);

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,strTitle,strlen(strTitle));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}

void V3_GuildDlg::Hide()
{
   BoxUI::Hide();
}

void V3_GuildDlg::Show()
{
   m_ListPerm.DeleteList();
   m_ListTitre.DeleteList();
   m_btnChangeSettings.Hide();
   BoxUI::Show();
}




void V3_GuildDlg::StartDragDlg( MousePos mousePos )
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

void V3_GuildDlg::StopDragDlg()
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
void V3_GuildDlg::DragEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_GuildDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}







void V3_GuildDlg::TabStatsEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   V3_StatsDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GuildDlg::TabInvEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   V3_InvDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GuildDlg::TabSpellEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   V3_SpellDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GuildDlg::TabProfessionEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );

   V3_ProfessionDlg::GetInstance()->Display(NULL);
   me->Hide();
}

/*
// void V3_GuildDlg::TabGuildeEvent::LeftClicked( void )
// {
//    V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
// }
*/

void V3_GuildDlg::TabGroupEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   V3_GroupeDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GuildDlg::TabMacroEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   V3_MacroDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_GuildDlg::TabOptionEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   V3_OptionsDlg::GetInstance()->Display(NULL);
   me->Hide();
}


void V3_GuildDlg::UpdateGuildList(BYTE chShow,const std::vector< User > &GuildUsers, char *pstrGuildName, char *pstrNotes, DWORD dwPermission,
                                  char *pstrR1,char *pstrR2,char *pstrR3,char *pstrR4,char *pstrR5)
{

   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() ); //normalement devrait remplacer le lock de la fonction...

   sprintf_s(m_strGuildName,50,"%s",pstrGuildName);
   sprintf_s(m_strLastNote ,255,"%s",pstrNotes);
   sprintf_s(m_strRangName[0],100,"%s",pstrR1);
   sprintf_s(m_strRangName[1],100,"%s",pstrR2);
   sprintf_s(m_strRangName[2],100,"%s",pstrR3);
   sprintf_s(m_strRangName[3],100,"%s",pstrR4);
   sprintf_s(m_strRangName[4],100,"%s",pstrR5);
   m_dwUserPermission.dwVal= dwPermission;
   
   m_strNotes.SetText( m_strLastNote, CL_ORANGE, 444,0,false,2 );
   
   
   EnterCriticalSection(&crLock);
   m_GuildList.DeleteList();
   
   vector< User >::const_iterator i;
   for( i = GuildUsers.begin(); i != GuildUsers.end(); i++ )
   {
      m_GuildList.AddNewItem();
      
      // Add the user name.
      {
         GuildMemberUI *memberUI = new GuildMemberUI;
         //le leader en orange...
         //les online en very
         //les offline en gris...
         
         if( (*i).dwPermission == GUILD_FONDATEUR_RIGHT )
         {
            if((*i).bOnline)
               memberUI->SetText( (*i).strUserName.c_str(), CL_ORANGE, g_Global.GetScreenW(), 0, 0 );
            else
               memberUI->SetText( (*i).strUserName.c_str(), CL_ORANGE_DARK, g_Global.GetScreenW(), 0, 0 );
         }
         else if((*i).bOnline)
            memberUI->SetText( (*i).strUserName.c_str(), CL_GREEN, g_Global.GetScreenW(), 0, 0 );
         else
            memberUI->SetText( (*i).strUserName.c_str(), CL_GRAY, g_Global.GetScreenW(), 0, 0 );
         
         memberUI->SetCenter( false );
         memberUI->SetPos( FramePos( 0, 0, memberUI->GetWidth(), memberUI->GetHeight() ) );
         memberUI->SetMember( (*i) );
         m_GuildList.AddColumnItem( 0, memberUI );
      }
      // Add the user title.
      {
         GuildMemberUI *memberUI = new GuildMemberUI;
         //le leader en orange...
         //les online en very
         //les offline en gris...
         
         if( (*i).dwPermission == GUILD_FONDATEUR_RIGHT )
         {
            if((*i).bOnline)
               memberUI->SetText( m_strRangName[(*i).dwTitle-1], CL_ORANGE, g_Global.GetScreenW(), 0, 0 );
            else
               memberUI->SetText( m_strRangName[(*i).dwTitle-1], CL_ORANGE_DARK, g_Global.GetScreenW(), 0, 0 );
         }
         else if((*i).bOnline)
            memberUI->SetText( m_strRangName[(*i).dwTitle-1], CL_GREEN, g_Global.GetScreenW(), 0, 0 );
         else
            memberUI->SetText( m_strRangName[(*i).dwTitle-1], CL_GRAY, g_Global.GetScreenW(), 0, 0 );
         
         
         memberUI->SetCenter( false );
         memberUI->SetPos( FramePos( 0, 0, memberUI->GetWidth(), memberUI->GetHeight() ) );
         memberUI->SetMember( (*i) );
         m_GuildList.AddColumnItem( 1, memberUI );
      }
   }
   LeaveCriticalSection(&crLock);

   if(GuildUsers.size() == 0)
   {
      m_strGuildName[0] = 0x00;
   }
}


void V3_GuildDlg::GuildListEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   EnterCriticalSection(&me->crLock);
   UserUI *userUI = static_cast< UserUI * >( me->m_GuildList.GetSelection() );    
   
   if( userUI != NULL )
   {
      me->selectedUser = userUI->userID;
   }
   else
   {
      me->selectedUser = "";
   }

 
   // All stocked items are GroupMembers by multiple inheritance.
   GuildMemberUI *member = static_cast< GuildMemberUI * >( me->m_GuildList.GetSelection() );
   
   // If no selection.
   if( member == NULL )
   {
      me->m_ListTitre.DeleteList();
      me->m_ListPerm.DeleteList();
      me->m_btnChangeSettings.Hide();

      LeaveCriticalSection(&me->crLock);
      return;
   }
   
   if(member->dwPermission == GUILD_FONDATEUR_RIGHT)
   {
      //on envoit p[lus de message etant donne que c<est la meme fenetre...
      //V3_ChatLogDlg::GetInstance()->AddListLogs("", g_GUILocalString[ 254 ], 0x0080FF, true);

      //on reset kles permission au MAX sans le bouton de changement
      me->m_ListTitre.DeleteList();
      me->m_ListPerm.DeleteList();
      me->m_btnChangeSettings.Hide();

      LeaveCriticalSection(&me->crLock);
      return;
   }

   if(!me->m_dwUserPermission.Perm.CanChPerm)
   {
      // le joueur a pas access au permission donc on ne lui permet pas de changer...

      me->m_ListTitre.DeleteList();
      me->m_ListPerm.DeleteList();
      me->m_btnChangeSettings.Hide();

      LeaveCriticalSection(&me->crLock);
      return;
   }
   

   
   me->m_dwUserID = member->dwID;
   me->UpdateRangDisplay(member->dwTitle-2,me->m_strRangName[1],me->m_strRangName[2],me->m_strRangName[3],me->m_strRangName[4],false);
   me->UpdatePermDisplay(member->dwPermission,false);
   me->m_btnChangeSettings.Show();

   LeaveCriticalSection(&me->crLock);
}


void V3_GuildDlg::Join(DWORD sourceID,string sourceName, std::string GuildName )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   // If there is already a box in foreground control.
   if( IsForegroundControlled() )
   {
      // Automatically decline
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_NM_GuildLeave;
      SEND_PACKET( sending );
   }
   else
   {    
      // If the user was ignored.
      if(V3_ChatDlg::GetInstance()->IsUserIgnored( sourceName )) //invite guild
      {
         // Decline.
         TFCPacket sending;
         sending << (RQ_SIZE)RQ_NM_GuildLeave;
         SEND_PACKET( sending );
      }
      else
      {
         char buf[ 1024 ];
         sprintf_s( buf,1024, g_LocalString[ 616 ], sourceName.c_str(),GuildName.c_str() );
         
         
         V3_ConfirmDlg::GetInstance()->SetDialogText(buf);
         V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[10],g_GUILocalString[11],738,739);
         V3_ConfirmDlg::GetInstance()->SetParent(this);
         V3_ConfirmDlg::GetInstance()->SetXYPos(((g_Global.GetScreenW()-g_AskValueDlgW)/2), ((g_Global.GetScreenH()-g_AskValueDlgH)/2));
         V3_ConfirmDlg::GetInstance()->Show(g_CONFIRM_ACCEPT_INVITE);
         
         RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
      }
      
   }
}

void V3_GuildDlg::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   switch(uiShowCodeP)
   {
      case g_CONFIRM_ACCEPT_INVITE:
      {
         if(bOK)
         {
            // Send a GroupJoin packet.
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_NM_GuildJoin;
            SEND_PACKET( sending );
         }
         else
         {
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_NM_GuildLeave;
            SEND_PACKET( sending );
         }
      }
      break;
      case g_CONFIRM_LEAVE_GUILD:
      {
         if(bOK)
         {
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_NM_GuildLeave;
            SEND_PACKET( sending );
            m_strGuildName[0]   = 0x00;
            m_strLastNote[0]    = 0x00;
            m_strRangName[0][0] = 0x00;
            m_strRangName[1][0] = 0x00;
            m_strRangName[2][0] = 0x00;
            m_strRangName[3][0] = 0x00;
            m_strRangName[4][0] = 0x00;
            m_dwUserPermission.dwVal = 0;

            Hide();
         }
      }
      break;
      case g_CONFIRM_KICK_GUILD:
         {
            if(bOK)
            {
               // Send a group kick packet.
               TFCPacket sending;
               sending << (RQ_SIZE)RQ_NM_GuildKick;
               sending << (long)m_iKickoutID;
               SEND_PACKET( sending );
            }
         }
      break;
   }
}

void V3_GuildDlg::AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey)
{
   if(!bOK)
      return;
   
   switch(uiShowCodeP)
   {
      case 0:
      {
         char strNote[1024];
         sprintf_s(strNote,1024,"%s",pStrValue1);
         if(strlen(strNote) > 255)
            strNote[254] = 0x00;
         
         
         // Send a group kick packet.
         TFCPacket sending;
         sending << (RQ_SIZE)RQ_NM_GuildChangeNote;
         sending << (short)strlen( strNote);
         sending << (char *)strNote;
         SEND_PACKET( sending );
      }
      break;
   }
}

void V3_GuildDlg::InviteButtonEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   MouseDevice md;
   if( md.LockNextEvent( DM_CLICK, me, InvitePlayer, 0 ) )
   {
      me->Hide();
   }
   else
   {
      // Could not lock mouse.
      V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[ 444 ], SYSTEM_COLOR, true);
   }
}

unsigned int V3_GuildDlg::InvitePlayer(void *data )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   V3_GuildDlg *me = reinterpret_cast< V3_GuildDlg * >( data );
   
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
      sending << (RQ_SIZE)RQ_NM_GuildInvite;
      sending << (long)dwID;
      sending << (short)( xPos + Player.xPos );
      sending << (short)( yPos + Player.yPos );        
      
      SEND_PACKET( sending );
   }
   
   me->Show();
   
   return 0;
}

void V3_GuildDlg::LeaveButtonEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   char strText[1000];
   sprintf_s(strText,1000,"%s",g_GUILocalString[250]);
    
   V3_ConfirmDlg::GetInstance()->SetDialogText(strText);
   V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
   V3_ConfirmDlg::GetInstance()->SetParent(me);
   V3_ConfirmDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_ConfirmDlg::GetInstance()->Show(g_CONFIRM_LEAVE_GUILD);
   
   me->RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
}

void V3_GuildDlg::KickButtonEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   EnterCriticalSection(&me->crLock);
   // All stocked items are GroupMembers by multiple inheritance.
   GuildMemberUI *member = static_cast< GuildMemberUI * >( me->m_GuildList.GetSelection() );
   
   // If no selection.
   if( member == NULL )
   {
      LeaveCriticalSection(&me->crLock);
      return;
   }

   char strText[1000];
   sprintf_s(strText,1000,g_GUILocalString[251],member->strUserName.c_str());
   
   V3_ConfirmDlg::GetInstance()->SetDialogText(strText);
   V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
   V3_ConfirmDlg::GetInstance()->SetParent(me);
   V3_ConfirmDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_ConfirmDlg::GetInstance()->Show(g_CONFIRM_KICK_GUILD);
   
   me->RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
   
   me->m_iKickoutID = member->dwID;
   LeaveCriticalSection(&me->crLock);
}

void V3_GuildDlg::ChangeNoteButtonEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,g_GUILocalString[263] ,me->m_strLastNote,255,0);
   V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueDlg::GetInstance()->SetParent(me);
   V3_AskValueDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueDlg::GetInstance()->Show(0);
   
   me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );
}



//////////////////////////////////////////////////////////////////////////////////////////
void V3_GuildDlg::TitreListEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   int col = me->m_ListTitre.GetSelectedColumn();
   
   string channelId;
   
   // Click in names listing
   if( col == 0 )
   {
      RangUI *optUI = static_cast< RangUI * >( me->m_ListTitre.GetSelection() );
      
      if( optUI == NULL )
         return;
   }
   // Clicked on the toggle button
   else if (col == 1)
   {
      
      RangUI *optUI = static_cast< RangUI * >( me->m_ListTitre.GetSelection() );
      
      if( optUI == NULL )
         return;
      
      int iSelectID = optUI->dwID;
      
      me->UpdateRang(iSelectID,false);
   }   
}

void V3_GuildDlg::PermListEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   int col = me->m_ListPerm.GetSelectedColumn();
   
   string channelId;
   
   // Click in names listing
   if( col == 0 )
   {
      RangUI *optUI = static_cast< RangUI * >( me->m_ListPerm.GetSelection() );
      
      if( optUI == NULL )
         return;
   }
   // Clicked on the toggle button
   else if (col == 1)
   {
      
      RangUI *optUI = static_cast< RangUI * >( me->m_ListPerm.GetSelection() );
      
      if( optUI == NULL )
         return;
      
      switch(optUI->dwID)
      {
         case 0: me->m_dwCurrentPermission.Perm.CanChPerm    = !me->m_dwCurrentPermission.Perm.CanChPerm; break;
         case 1: me->m_dwCurrentPermission.Perm.CanInvite    = !me->m_dwCurrentPermission.Perm.CanInvite; break;
         case 2: me->m_dwCurrentPermission.Perm.CanKick      = !me->m_dwCurrentPermission.Perm.CanKick; break;
         case 3: me->m_dwCurrentPermission.Perm.ViewChest    = !me->m_dwCurrentPermission.Perm.ViewChest; break;
         case 4: me->m_dwCurrentPermission.Perm.CanDeposit   = !me->m_dwCurrentPermission.Perm.CanDeposit; break;
         case 5: me->m_dwCurrentPermission.Perm.CanTake      = !me->m_dwCurrentPermission.Perm.CanTake; break;
         case 6: me->m_dwCurrentPermission.Perm.CanWriteNote = !me->m_dwCurrentPermission.Perm.CanWriteNote; break;
         case 7: me->m_dwCurrentPermission.Perm.CanSeeLog    = !me->m_dwCurrentPermission.Perm.CanSeeLog; break;
      }
      
      me->UpdatePermDisplay(me->m_dwCurrentPermission.dwVal,false);
   }    
}


void V3_GuildDlg::UpdateRangDisplay(int iRandSelect,char*pR1,char*pR2,char*pR3,char*pR4,bool bReset)
{
   
   
   sprintf_s(m_strRangN[0],100,"%s",pR1);
   sprintf_s(m_strRangN[1],100,"%s",pR2);
   sprintf_s(m_strRangN[2],100,"%s",pR3);
   sprintf_s(m_strRangN[3],100,"%s",pR4);
   
   UpdateRang(iRandSelect,bReset);
}


void V3_GuildDlg::UpdateRang(int iRandSelect,bool bReset)
{
   m_ListTitre.DeleteList();
   
   for(int i=0;i<4;i++)
      m_iSelect[i] = 0;
   
   if(!bReset)
   {
      m_iSelect[iRandSelect] = 1;
      m_iCurrentTitle = iRandSelect;
   }
   else
   {
      m_iCurrentTitle = 0;
   }
   
   
   
   AddRangParam(m_strRangN[0],0, m_iSelect[0]);
   AddRangParam(m_strRangN[1],1, m_iSelect[1]);
   AddRangParam(m_strRangN[2],2, m_iSelect[2]);
   AddRangParam(m_strRangN[3],3, m_iSelect[3]);
}

void V3_GuildDlg::UpdatePermDisplay(DWORD dwPermission,bool bReset)
{
   m_ListPerm.DeleteList();
   if(!bReset)
      m_dwCurrentPermission.dwVal = dwPermission;
   else
      m_dwCurrentPermission.dwVal = 0;
   
   AddPermParam(g_GUILocalString[261],0,m_dwCurrentPermission.Perm.CanChPerm);
   AddPermParam(g_GUILocalString[255],1,m_dwCurrentPermission.Perm.CanInvite);
   AddPermParam(g_GUILocalString[256],2,m_dwCurrentPermission.Perm.CanKick);
   AddPermParam(g_GUILocalString[257],3,m_dwCurrentPermission.Perm.ViewChest);
   AddPermParam(g_GUILocalString[258],4,m_dwCurrentPermission.Perm.CanDeposit);
   AddPermParam(g_GUILocalString[259],5,m_dwCurrentPermission.Perm.CanTake);
   AddPermParam(g_GUILocalString[260],6,m_dwCurrentPermission.Perm.CanWriteNote);
   AddPermParam(g_GUILocalString[262],7,m_dwCurrentPermission.Perm.CanSeeLog);
}


void V3_GuildDlg::AddRangParam(char *pstrName,int dwID, int dwStatus)
{
   bool bStatus = false;
   if(dwStatus)
      bStatus = true;
   
   m_ListTitre.AddNewItem();
   RangUI *pRangUI = new RangUI;
   
   pRangUI->SetText( pstrName, g_DefColorD, 640, 0, 0 );
   pRangUI->SetCenter( false );
   pRangUI->SetPos( FramePos( 0, 0, pRangUI->GetWidth(), pRangUI->GetHeight() ) );
   pRangUI->optionID = pstrName;
   pRangUI->bState = bStatus;   
   pRangUI->dwID   = dwID;
   m_ListTitre.AddColumnItem( 0, pRangUI );
   
   
   SettingRangToggle *pToggleUI = new SettingRangToggle;
   if( bStatus )
   {
      pToggleUI->SetSpriteId   ( "V3_Check18_ON_N" );
      pToggleUI->SetDownUI     ( "V3_Check18_ON_N" );
      pToggleUI->SetHighlightUI( "V3_Check18_ON_H" );
   }else
   {
      pToggleUI->SetSpriteId   ( "V3_Check18_OFF_N" );
      pToggleUI->SetDownUI     ( "V3_Check18_OFF_N" );
      pToggleUI->SetHighlightUI( "V3_Check18_OFF_H" );
   }
   //channelToggleUI->SetCenter( false ); 
   pToggleUI->SetPos( FramePos( 0, 0, 18, 18 ) );
   pToggleUI->optionID = pstrName;
   pToggleUI->bState = bStatus;
   m_ListTitre.AddColumnItem( 1, pToggleUI );
}

void V3_GuildDlg::AddPermParam(char *pstrName,int dwID, int dwStatus)
{
   bool bStatus = false;
   if(dwStatus)
      bStatus = true;
   
   m_ListPerm.AddNewItem();
   RangUI *pRangUI = new RangUI;
   
   pRangUI->SetText( pstrName, g_DefColorD, 640, 0, 0 );
   pRangUI->SetCenter( false );
   pRangUI->SetPos( FramePos( 0, 0, pRangUI->GetWidth(), pRangUI->GetHeight() ) );
   pRangUI->optionID = pstrName;
   pRangUI->bState = bStatus;   
   pRangUI->dwID   = dwID;
   m_ListPerm.AddColumnItem( 0, pRangUI );
   
   
   SettingRangToggle *pToggleUI = new SettingRangToggle;
   if( bStatus )
   {
      pToggleUI->SetSpriteId   ( "V3_Check18_ON_N" );
      pToggleUI->SetDownUI     ( "V3_Check18_ON_N" );
      pToggleUI->SetHighlightUI( "V3_Check18_ON_H" );
   }
   else
   {
      pToggleUI->SetSpriteId   ( "V3_Check18_OFF_N" );
      pToggleUI->SetDownUI     ( "V3_Check18_OFF_N" );
      pToggleUI->SetHighlightUI( "V3_Check18_OFF_H" );
   }
   //channelToggleUI->SetCenter( false ); 
   pToggleUI->SetPos( FramePos( 0, 0, 18, 18 ) );
   pToggleUI->optionID = pstrName;
   pToggleUI->bState = bStatus;
   m_ListPerm.AddColumnItem( 1, pToggleUI );
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_GuildDlg::ChangeSettingsButtonEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   int iTitre = me->m_iCurrentTitle+2;
   // Send a group kick packet.
   TFCPacket sending;
   sending << (RQ_SIZE)RQ_NM_GuildChangeSetting;
   sending << (long)me->m_dwUserID;
   sending << (long)iTitre;
   sending << (long)me->m_dwCurrentPermission.dwVal;
   
   SEND_PACKET( sending );
}

void V3_GuildDlg::ViewLogButtonEvent::LeftClicked( void )
{
   V3_GuildDlg *me = static_cast< V3_GuildDlg * >( GetUI() );
   
   me->logsPopup.ClearLogList();
   // Send a group kick packet.
   TFCPacket sending;
   sending << (RQ_SIZE)RQ_NM_GuildGetLogs;
   SEND_PACKET( sending );
}


void V3_GuildDlg::ClearLogList()
{
   logsPopup.ClearLogList();
}

void V3_GuildDlg::AddLogs(char *pstrDatem ,char *pstrLogs)
{
   logsPopup.AddLogs(pstrDatem,pstrLogs);
}

void V3_GuildDlg::DisplayLog()
{
   logsPopup.DisplayLog();
}


//////////////////////////////////////////////////////////////////////////////////////////
V3_GuildDlg::LogsPopup::LogsPopup( void ) 
:
X_Button( hideEvent ),
ListLogs( logListEvent )
//////////////////////////////////////////////////////////////////////////////////////////
{
   //Back du popup box...
   int iPosX = (g_Global.GetScreenW()-g_GuildLogDlgW)/2;
   int iPosY = ((g_Global.GetScreenH()-g_GuildLogDlgH)/2)-(g_MainBarDlgH/2);
   SetPos( FramePos( iPosX, iPosY, iPosX+g_GuildLogDlgW, iPosY+g_GuildLogDlgH ) );
   
   LogsBack.SetSpriteId( "V3_GuildLogBack",true,"V3_GuildLogBackMask" );
   LogsBack.SetPos( FramePos( iPosX, iPosY, iPosX+g_GuildLogDlgW, iPosY+g_GuildLogDlgH ) );
   
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" );
   X_Button.SetPos( FramePos( iPosX+727, iPosY+27, iPosX+727+27, iPosY+27+24 ) );
   SetHelpId( X_Button, 261 );

        

   ListLogs.SetupList    ( 2, 20, FramePos( 0, 0, 2, 1 ) );
   ListLogs.SetColumnSize( 0, 160  );
   ListLogs.SetColumnSize( 1, 527  );
   ListLogs.SetScrollUpDownUI( &ListScrollUp );
   ListLogs.SetScrollDownDownUI( &ListScrollDown );

   ListLogs.SetListDef(iPosX,iPosY,20,36,118,747,389,&ListScrollUp,&ListScrollDown);

   m_strT.SetText(g_GUILocalString[266],g_DefColorH);
   m_strL.SetText(g_GUILocalString[267],g_DefColorH);
   m_strT.SetPos ( FramePos( iPosX+37 , iPosY+94, iPosX+194, iPosY+111 ) );
   m_strL.SetPos ( FramePos( iPosX+199, iPosY+94, iPosX+723, iPosY+111 ) );
  
   AddChild( &LogsBack );
   AddChild( &ListLogs );
   AddChild( &X_Button );
   AddChild( &m_strT);
   AddChild( &m_strL);
   
   
   RootBoxUI::GetInstance()->AddChild( this );
}

void V3_GuildDlg::LogsPopup::Draw(V2SPRITEFX *vsfFX)
{
   int iPosX = (g_Global.GetScreenW()-g_GuildLogDlgW)/2;
   int iPosY = ((g_Global.GetScreenH()-g_GuildLogDlgH)/2)-(g_MainBarDlgH/2);
      
   GameUI::Draw( vsfFX );

   HDC hdc;
   DXDGetDC(&hdc, 86);
   
   // Draw les texte...
  
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());
   
   //Draw All header
   TextOut(hdc,iPosX+68, iPosY+30,g_GUILocalString[265],strlen(g_GUILocalString[265]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}


//////////////////////////////////////////////////////////////////////////////////////////
void V3_GuildDlg::LogsPopup::LogListEvent::LeftClicked( void )
{
  
}




void V3_GuildDlg::LogsPopup::ClearLogList()
{
   ListLogs.DeleteList();
}

void V3_GuildDlg::LogsPopup::AddLogs(char *pstrDate ,char *pstrLogs)
{
   ListLogs.AddNewItem();

   LogsUI *pLogUI = new LogsUI;
   pLogUI->SetText( pstrDate, g_DefColorD, 640, 0, 0 );
   pLogUI->SetCenter( false );
   pLogUI->SetPos( FramePos( 0, 0, pLogUI->GetWidth(), pLogUI->GetHeight() ) );
   pLogUI->strTxt  = pstrDate;
   pLogUI->dwID   = 0;
   ListLogs.AddColumnItem( 0, pLogUI );

   pLogUI = new LogsUI;
   pLogUI->SetText( pstrLogs, g_DefColorD, 640, 0, 0 ,true);
   pLogUI->SetCenter( false );
   pLogUI->SetPos( FramePos( 0, 0, pLogUI->GetWidth(), pLogUI->GetHeight() ) );
   pLogUI->strTxt  = pstrLogs;
   pLogUI->dwID   = 1;
   ListLogs.AddColumnItem( 1, pLogUI );
}

void V3_GuildDlg::LogsPopup::DisplayLog()
{
   V3_GuildDlg::GetInstance()->logsPopup.Show();
   V3_GuildDlg::GetInstance()->RequestForegroundControl(&V3_GuildDlg::GetInstance()->logsPopup);
}