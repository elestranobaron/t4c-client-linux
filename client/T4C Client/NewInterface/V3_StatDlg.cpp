// V3_StatsDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_StatDlg.h"
#include "RootBoxUI.h"
#include "V3_ChatLogDlg.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"

#include "V3_MacroDlg.h"
#include "V3_OptionsDlg.h"
#include "V3_SpellDlg.h"
#include "V3_GroupeDlg.h"
#include "V3_GuildDlg.h"
#include "V3_ProfessionDlg.h"
#include "V3_InvDlg.h"
#include "V3_InteractionRP.h"
#include "V3_InteractionPVP.h"
#include "V3_ArenePointsDlg.h"

extern DWORD TargetID;
extern CSaveGame g_SaveGame;

DWORD g_dwLastMacroCallStat = 0;

// Pose des le debut du constructeur pour qu'un GetInstance() concurrent (thread de rendu/HUD pendant
// que le thread reseau construit l'instance, ou inversement) renvoie l'instance-en-cours-de-construction
// au lieu de bloquer sur le guard d'init statique thread-safe de VS2022 (/Zc:threadSafeInit, absent du
// build VC6/VC8 d'origine). Sans ca, ce guard inter-bloque avec le verrou UI (RootBoxUI) -> deadlock du
// thread reseau lors d'un RQ_HPchanged recu avant la fin de l'entree (serveur Linux) -> blocage au loading.
static V3_StatsDlg* g_pStatsDlgInstance = NULL;


const int STRid = 1;
const int ENDid = 2;
const int AGIid = 3;
const int WILid = 4;
const int INTid = 5;
const int WISid = 6;
const int LCKid = 7;

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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_StatsDlg::V3_StatsDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
upgSTR_Button( upgSTRevent ),
upgEND_Button( upgENDevent ),
upgAGI_Button( upgAGIevent ),
upgWIS_Button( upgWISevent ),
upgINT_Button( upgINTevent ),
dngSTR_Button( dngSTRevent ),
dngEND_Button( dngENDevent ),
dngAGI_Button( dngAGIevent ),
dngWIS_Button( dngWISevent ),
dngINT_Button( dngINTevent ),
statsCommit_Button( statsCommitEvent,g_GUILocalString[17],g_DefColorH ),
IteractionRPButton( iteractionRPEvent,g_GUILocalString[374],g_DefColorH ),
IteractionPVPButton( iteractionPVPEvent,g_GUILocalString[383],g_DefColorH ),
addMacro_Button( addMacroEvent,g_GUILocalString[308],g_DefColorH ),
OngletStat_Button(ongletStatEvent,g_GUILocalString[317],g_DefColorH ),
OngletPR_Button(ongletPREvent,g_GUILocalString[318],g_DefColorH ),
MOnglet01Button(EmptyEvent::GetInstance(),g_GUILocalString[309],g_DefColorH ),
MOnglet02Button(tabInvEvent              ,g_GUILocalString[310],g_DefColorH ),
MOnglet03Button(tabSpellEvent            ,g_GUILocalString[311],g_DefColorH ),
MOnglet04Button(tabProfessionEvent       ,g_GUILocalString[312],g_DefColorH ),
MOnglet05Button(tabGuildeEvent           ,g_GUILocalString[313],g_DefColorH ),
MOnglet06Button(tabGroupEvent            ,g_GUILocalString[314],g_DefColorH ),
MOnglet07Button(tabMacroEvent            ,g_GUILocalString[315],g_DefColorH ),
MOnglet08Button(tabOptionEvent           ,g_GUILocalString[316],g_DefColorH ),
skillList( skillEvent )
{
   g_pStatsDlgInstance = this;

   int i=0;
   InitSound();

   m_iSelectDisplay = 0;
 
   m_iUpgSTR      = 0;
   m_iUpgEND      = 0;
   m_iUpgAGI      = 0;
   m_iUpgINT      = 0;
   m_iUpgWIS      = 0;
   m_usedStatPts  = 0;


   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_StatBack8n",true,"V3_StatBack8nMask");
   m_BackgroundPR.SetSpriteId("V3_StatBack8n2",true,"V3_StatBack8nMask");
   m_backIconsS.SetSpriteId(  "V3_StatBackIconsn");
   m_backIconsPR.SetSpriteId( "V3_StatPuisBackIconsn");

   m_TitleIcon  .SetSpriteId( "V3_StatIcon");
   m_backIconsSH.SetSpriteId( "V3_StatIconH");
   m_backIconsSF.SetSpriteId( "V3_StatIconF");



   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
   
   

   upgSTRevent.stat = STRid;
   upgENDevent.stat = ENDid;
   upgAGIevent.stat = AGIid;
   upgWISevent.stat = WISid;
   upgINTevent.stat = INTid;
   dngSTRevent.stat = STRid;
   dngENDevent.stat = ENDid;
   dngAGIevent.stat = AGIid;
   dngWISevent.stat = WISid;
   dngINTevent.stat = INTid;
   
   upgSTRevent.SetUI( this );
   upgENDevent.SetUI( this );
   upgAGIevent.SetUI( this );
   upgWISevent.SetUI( this );
   upgINTevent.SetUI( this );
   dngSTRevent.SetUI( this );
   dngENDevent.SetUI( this );
   dngAGIevent.SetUI( this );
   dngWISevent.SetUI( this );
   dngINTevent.SetUI( this );

   // Setup the upgrade/downgrade buttons.
   upgSTR_Button.SetSpriteId   ( "V3_SpinUP_N" );
   upgSTR_Button.SetHighlightUI( "V3_SpinUP_H" );
   upgSTR_Button.SetEnableDragCycle();
   upgEND_Button.SetSpriteId   ( "V3_SpinUP_N" );
   upgEND_Button.SetHighlightUI( "V3_SpinUP_H" );
   upgEND_Button.SetEnableDragCycle();
   upgAGI_Button.SetSpriteId   ( "V3_SpinUP_N" );
   upgAGI_Button.SetHighlightUI( "V3_SpinUP_H" );
   upgAGI_Button.SetEnableDragCycle();
   upgWIS_Button.SetSpriteId   ( "V3_SpinUP_N" );
   upgWIS_Button.SetHighlightUI( "V3_SpinUP_H" );
   upgWIS_Button.SetEnableDragCycle();
   upgINT_Button.SetSpriteId   ( "V3_SpinUP_N" );
   upgINT_Button.SetHighlightUI( "V3_SpinUP_H" );
   upgINT_Button.SetEnableDragCycle();
   
   dngSTR_Button.SetSpriteId   ( "V3_SpinDN_N" );
   dngSTR_Button.SetHighlightUI( "V3_SpinDN_H" );
   dngSTR_Button.SetEnableDragCycle();
   dngEND_Button.SetSpriteId   ( "V3_SpinDN_N" );
   dngEND_Button.SetHighlightUI( "V3_SpinDN_H" );
   dngEND_Button.SetEnableDragCycle();
   dngAGI_Button.SetSpriteId   ( "V3_SpinDN_N" );
   dngAGI_Button.SetHighlightUI( "V3_SpinDN_H" );
   dngAGI_Button.SetEnableDragCycle();
   dngWIS_Button.SetSpriteId   ( "V3_SpinDN_N" );
   dngWIS_Button.SetHighlightUI( "V3_SpinDN_H" );
   dngWIS_Button.SetEnableDragCycle();
   dngINT_Button.SetSpriteId   ( "V3_SpinDN_N" );
   dngINT_Button.SetHighlightUI( "V3_SpinDN_H" );
   dngINT_Button.SetEnableDragCycle();

   OngletStat_Button.SetHighlightUI( "V3_StatOngletSPR_H");
   OngletPR_Button.SetHighlightUI( "V3_StatOngletSPR_H");

   MOnglet01Button.SetHighlightUI( "V3_MainOnglet_77p_H");
   MOnglet02Button.SetHighlightUI( "V3_MainOnglet_77p_H");
   MOnglet03Button.SetHighlightUI( "V3_MainOnglet_48p_H");
   MOnglet04Button.SetHighlightUI( "V3_MainOnglet_77p_H");
   MOnglet05Button.SetHighlightUI( "V3_MainOnglet_50p_H");
   MOnglet06Button.SetHighlightUI( "V3_MainOnglet_50p_H");
   MOnglet07Button.SetHighlightUI( "V3_MainOnglet_50p_H");
   MOnglet08Button.SetHighlightUI( "V3_MainOnglet_50p_H");

   //tabStatsEvent     .SetUI(this);
   tabInvEvent       .SetUI(this);
   tabSpellEvent     .SetUI(this);
   tabProfessionEvent.SetUI(this);
   tabGuildeEvent    .SetUI(this);
   tabGroupEvent     .SetUI(this);
   tabMacroEvent     .SetUI(this);
   tabOptionEvent    .SetUI(this);

   statsCommitEvent.SetUI( this );
   statsCommit_Button.SetSpriteId     ( "V3_Btn92x24_N" );
   statsCommit_Button.SetDownUI       ( "V3_Btn92x24_D" );
   statsCommit_Button.SetHighlightUI  ( "V3_Btn92x24_H" );
   SetHelpId( statsCommit_Button, 295 );

   iteractionRPEvent.SetUI( this );
   IteractionRPButton.SetSpriteId     ( "V3_Btn92x24_N" );
   IteractionRPButton.SetDownUI       ( "V3_Btn92x24_D" );
   IteractionRPButton.SetHighlightUI  ( "V3_Btn92x24_H" );
   SetHelpId( IteractionRPButton, 748 );

   iteractionPVPEvent.SetUI( this );
   IteractionPVPButton.SetSpriteId     ( "V3_Btn92x24_N" );
   IteractionPVPButton.SetDownUI       ( "V3_Btn92x24_D" );
   IteractionPVPButton.SetHighlightUI  ( "V3_Btn92x24_H" );
   SetHelpId( IteractionPVPButton, 792 );

   

   addMacroEvent.SetUI( this );
   addMacro_Button.SetSpriteId     ( "V3_Btn92x24_N" );
   addMacro_Button.SetDownUI       ( "V3_Btn92x24_D" );
   addMacro_Button.SetHighlightUI  ( "V3_Btn92x24_H" );
   SetHelpId( addMacro_Button, 447 );

   


   UpdateAllFramePos(m_iXPos,m_iYPos);

   m_statR[2].SetHelpText( FontManager::GetInstance()->GetSmallFont() );
   m_statR[3].SetHelpText( FontManager::GetInstance()->GetSmallFont() );

 

   SetHelpId( m_hlpL[0], 262 );
   SetHelpId( m_hlpL[1], 263 );
   SetHelpId( m_hlpL[2], 264 );
   SetHelpId( m_hlpL[3], 266 );
   SetHelpId( m_hlpL[4], 265 );
   SetHelpId( m_hlpL[5], 581 );

   SetHelpId( m_hlpR[0], 445 );
   SetHelpId( m_hlpR[1], 596 );
   SetHelpId( m_hlpR[2], 286 );
   SetHelpId( m_hlpR[3], 287 );
   SetHelpId( m_hlpR[4], 486 );
   SetHelpId( m_hlpR[5], 582 );


  
   // Setup the skill list.

   skillList.SetupList    ( 3, 38, FramePos( 0, 0, 0, 3 ) );
   skillList.SetColumnSize( 0, 40  );
   skillList.SetColumnSize( 1, 30  );
   skillList.SetColumnSize( 2, 146 );
   skillList.CenterColumn( 0 );
   skillList.CenterColumn( 1 );
   skillList.CenterColumn( 2 );
   skillList.SetScrollUpDownUI( &skillScrollUp );
   skillList.SetScrollDownDownUI( &skillScrollDown );
   
   skillListSelect.SetPos( FramePos( 0, 0, 220, 38 ) );
   skillListSelect.SetSpriteId( "V3_StatSkillListSelect" );
   skillListSelect.Show();
   skillList.SetSelectionSprite( &skillListSelect);
   skillEvent.SetUI( this );
   SetHelpId( skillList, 296 );

   ongletStatEvent.SetUI( this );
   ongletPREvent.SetUI( this );

   m_backIconsIP.SetSpriteId( "V3_IconInterPoints");
   SetHelpId(m_backIconsIP,811);
   SetHelpId(m_ptsInter,811);

   m_backIconsIPT.SetSpriteId( "V3_IconInterPoints");
   SetHelpId(m_backIconsIPT,812);
   SetHelpId(m_ptsInterT,812);
   

   
   AddChild( &m_BackgroundS); 
   AddChild( &m_BackgroundPR); 
   AddChild( &m_backIconsS); 
   AddChild( &m_backIconsPR);
   AddChild( &m_TitleIcon ); 
   AddChild( &m_backIconsSH );
   AddChild( &m_backIconsSF );
   AddChild( &X_Button);
   AddChild( &m_backIconsIP);
   AddChild( &m_ptsInter);
   AddChild( &m_backIconsIPT);
   AddChild( &m_ptsInterT);
   

   AddChild( &m_XP[0] ); 
   AddChild( &m_XP[1] ); 
   AddChild( &m_XPT[0] ); 
   AddChild( &m_XPT[1] ); 
   AddChild( &m_NomLvl[0] ); 
   AddChild( &m_NomLvl[1] ); 
   AddChild( &m_PtsStat[0] ); 
   AddChild( &m_PtsStat[1] ); 
   AddChild( &m_PtsSKill[0] ); 
   AddChild( &m_PtsSKill[1] ); 
   AddChild( &m_Karma[0] ); 
   AddChild( &m_Karma[1] ); 


   AddChild( &OngletStat_Button );
   AddChild( &OngletPR_Button );

   AddChild( &MOnglet01Button );
   AddChild( &MOnglet02Button );
   AddChild( &MOnglet03Button );
   AddChild( &MOnglet04Button );
   AddChild( &MOnglet05Button );
   AddChild( &MOnglet06Button );
   AddChild( &MOnglet07Button );
   AddChild( &MOnglet08Button );

   
   AddChild( &Drag_Button);
   AddChild( &upgSTR_Button );
   AddChild( &upgEND_Button );
   AddChild( &upgAGI_Button );
   AddChild( &upgWIS_Button );
   AddChild( &upgINT_Button );
   AddChild( &dngSTR_Button );
   AddChild( &dngEND_Button );
   AddChild( &dngAGI_Button );
   AddChild( &dngWIS_Button );
   AddChild( &dngINT_Button );
   AddChild( &statsCommit_Button );
   AddChild( &IteractionRPButton );
   AddChild( &IteractionPVPButton );
   AddChild( &addMacro_Button );
   AddChild( &skillList );

   

   for(i=0;i<6;i++)
   {
      AddChild( &m_statL[i] ); 
      AddChild( &m_statR[i] );
      AddChild( &m_hlpL[i] ); 
      AddChild( &m_hlpR[i] );
   }

   RootBoxUI::GetInstance()->AddChild( this );

   // Hide all upgrade buttons by default.
   upgSTR_Button.Hide();
   upgEND_Button.Hide();
   upgAGI_Button.Hide();
   upgWIS_Button.Hide();
   upgINT_Button.Hide();
   dngSTR_Button.Hide();
   dngEND_Button.Hide();
   dngAGI_Button.Hide();
   dngWIS_Button.Hide();
   dngINT_Button.Hide();

   statsCommit_Button.Hide();
   addMacro_Button.Hide();
   m_backIconsSH.Hide();
   m_backIconsSF.Hide();

   m_BackgroundPR.Hide();
   m_backIconsPR.Hide();

   


   // Get the skill list.
   TFCPacket sending;
   sending << (RQ_SIZE)RQ_GetSkillList;	
   SEND_PACKET( sending );

   InitializeMacro();


   
} 

V3_StatsDlg::~V3_StatsDlg()
{
   
}

void V3_StatsDlg::Reload()
{
   InitializeMacro();
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_StatsDlg *V3_StatsDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{
    // Renvoie l'instance en cours de construction lors d'un appel concurrent/re-entrant,
    // avant que le guard magic-static ci-dessous ne se (self-)bloque (cf. g_pStatsDlgInstance).
    if (g_pStatsDlgInstance)
       return g_pStatsDlgInstance;

    static V3_StatsDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_StatsDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallStat > 500)
      {
         g_dwLastMacroCallStat =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}

unsigned int V3_StatsDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   //GetInstance()->UpdateCharacterSheet_Dis("V3_StatsDlg::Display");

   g_Global.SetLastDialogOpen(0);

   TFCPacket Send,SendA, Send2;	

   // Get the skill list.
   Send2 << (RQ_SIZE)RQ_GetSkillList;	
   SEND_PACKET(Send2);
   // Show box.

   //Get Puissance et resistance
   SendA << (RQ_SIZE)RQ_GetStatus2;	
   SEND_PACKET( SendA );

   // Get the stats
   Send << (RQ_SIZE)RQ_GetStatus;	
   SEND_PACKET(Send);
 

   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);
   GetInstance()->Show();
   return 0;
}

void V3_StatsDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundPR.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_backIconsS.SetPos( FramePos( iPosX+40, iPosY+181, iPosX+40+176,iPosY+181+212));
   m_backIconsPR.SetPos( FramePos( iPosX+40, iPosY+181, iPosX+40+176,iPosY+181+212));

   m_TitleIcon  .SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   m_backIconsSH.SetPos( FramePos( iPosX+38, iPosY+92 , iPosX+38+12 ,iPosY+92+15));
   m_backIconsSF.SetPos( FramePos( iPosX+38, iPosY+92 , iPosX+38+12 ,iPosY+92+15));
   
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );
   
   int iUpSY = 185;
   int iDnSY = 198;
   upgSTR_Button.SetPos( FramePos( iPosX+163, iPosY+ iUpSY, iPosX+163+16, iPosY+ iUpSY+11) );
   dngSTR_Button.SetPos( FramePos( iPosX+163, iPosY+ iDnSY, iPosX+163+16, iPosY+ iDnSY+11) );
   iUpSY+=36;
   iDnSY+=36;
   upgEND_Button.SetPos( FramePos( iPosX+163, iPosY+ iUpSY, iPosX+163+16, iPosY+ iUpSY+11) );
   dngEND_Button.SetPos( FramePos( iPosX+163, iPosY+ iDnSY, iPosX+163+16, iPosY+ iDnSY+11) );
   iUpSY+=36;
   iDnSY+=36;
   upgAGI_Button.SetPos( FramePos( iPosX+163, iPosY+ iUpSY, iPosX+163+16, iPosY+ iUpSY+11) );  
   dngAGI_Button.SetPos( FramePos( iPosX+163, iPosY+ iDnSY, iPosX+163+16, iPosY+ iDnSY+11) );  
   iUpSY+=36;
   iDnSY+=36;
   upgINT_Button.SetPos( FramePos( iPosX+163, iPosY+ iUpSY, iPosX+163+16, iPosY+ iUpSY+11) );
   dngINT_Button.SetPos( FramePos( iPosX+163, iPosY+ iDnSY, iPosX+163+16, iPosY+ iDnSY+11) );
   iUpSY+=36;
   iDnSY+=36;
   upgWIS_Button.SetPos( FramePos( iPosX+163, iPosY+ iUpSY, iPosX+163+16, iPosY+ iUpSY+11) );
   dngWIS_Button.SetPos( FramePos( iPosX+163, iPosY+ iDnSY, iPosX+163+16, iPosY+ iDnSY+11) );
   
   statsCommit_Button.SetPos( FramePos( iPosX+125, iPosY+404, iPosX+125+92, iPosY+404+24) );
   addMacro_Button.SetPos( FramePos( iPosX+474, iPosY+126, iPosX+474+92, iPosY+126+24) );

   IteractionRPButton.SetPos( FramePos( iPosX+440, iPosY+404, iPosX+440+92, iPosY+404+24) );
   IteractionPVPButton.SetPos( FramePos( iPosX+330, iPosY+404, iPosX+330+92, iPosY+404+24) );

   m_backIconsIP.SetPos( FramePos( iPosX+315 , iPosY+88, iPosX+335 ,iPosY+110));
   m_ptsInter.SetPos( FramePos( iPosX+337  , iPosY+91 , iPosX+429, iPosY+105) );
   m_backIconsIPT.SetPos( FramePos( iPosX+439 , iPosY+88, iPosX+459 ,iPosY+110));
   m_ptsInterT.SetPos( FramePos( iPosX+463  , iPosY+91 , iPosX+558, iPosY+105) );
   
   
   skillList.SetListDef(iPosX,iPosY,20,324,178,566,338,&skillScrollUp,&skillScrollDown);



   
   //Set Stat Pos
   const int iSYs = 184;
   const int iEYs = 209;
   const int iSYh = 180;
   const int iEYh = 213;
   const int iOY  = 36;
   
   for(int i=0;i<6;i++)
   {
      m_statL[i].SetPos ( FramePos( iPosX+69 , iPosY+iSYs+(i*iOY) , iPosX+159, iPosY+iEYs+(i*iOY) ) );
      m_statR[i].SetPos ( FramePos( iPosX+213, iPosY+iSYs+(i*iOY) , iPosX+303, iPosY+iEYs+(i*iOY) ) );
      
      m_hlpL[i].SetPos ( FramePos( iPosX+39 , iPosY+iSYh+(i*iOY) , iPosX+68, iPosY+iEYh+(i*iOY) ) );
      m_hlpR[i].SetPos ( FramePos( iPosX+183, iPosY+iSYh+(i*iOY) , iPosX+212,iPosY+iEYh+(i*iOY) ) );
   }
   
   m_XP[0]      .SetPos ( FramePos( iPosX+392 , iPosY+355 , iPosX+573, iPosY+372) );
   m_XP[1]      .SetPos ( FramePos( iPosX+392 , iPosY+377 , iPosX+573, iPosY+394) );
   m_XPT[0]     .SetPos ( FramePos( iPosX+312 , iPosY+355 , iPosX+389, iPosY+372) );
   m_XPT[1]     .SetPos ( FramePos( iPosX+312 , iPosY+377 , iPosX+389, iPosY+394) );
   m_NomLvl[0]  .SetPos ( FramePos( iPosX+68  , iPosY+90  , iPosX+127, iPosY+107) );
   m_NomLvl[1]  .SetPos ( FramePos( iPosX+127 , iPosY+90  , iPosX+309, iPosY+107) );
   m_PtsStat[0] .SetPos ( FramePos( iPosX+40  , iPosY+115 , iPosX+159, iPosY+129) );
   m_PtsStat[1] .SetPos ( FramePos( iPosX+40  , iPosY+131 , iPosX+159, iPosY+148) );
   m_PtsSKill[0].SetPos ( FramePos( iPosX+325 , iPosY+115 , iPosX+444, iPosY+129) );
   m_PtsSKill[1].SetPos ( FramePos( iPosX+325 , iPosY+131 , iPosX+444, iPosY+148) );
   m_Karma[0]   .SetPos ( FramePos( iPosX+184 , iPosY+115 , iPosX+303, iPosY+129) );
   m_Karma[1]   .SetPos ( FramePos( iPosX+184 , iPosY+131 , iPosX+303, iPosY+148) );

   //les 8 onglet
   MOnglet01Button.SetPos ( FramePos( iPosX+33  , iPosY+63  , iPosX+109, iPosY+83 ) );
   MOnglet02Button.SetPos ( FramePos( iPosX+112 , iPosY+63  , iPosX+188, iPosY+83 ) );
   MOnglet03Button.SetPos ( FramePos( iPosX+191 , iPosY+63  , iPosX+236, iPosY+83 ) );
   MOnglet04Button.SetPos ( FramePos( iPosX+239 , iPosY+63  , iPosX+315, iPosY+83 ) );
   MOnglet05Button.SetPos ( FramePos( iPosX+318 , iPosY+63  , iPosX+367, iPosY+83 ) );
   MOnglet06Button.SetPos ( FramePos( iPosX+370 , iPosY+63  , iPosX+419, iPosY+83 ) );
   MOnglet07Button.SetPos ( FramePos( iPosX+422 , iPosY+63  , iPosX+471, iPosY+83 ) );
   MOnglet08Button.SetPos ( FramePos( iPosX+474 , iPosY+63  , iPosX+523, iPosY+83 ) );


   OngletStat_Button.SetPos ( FramePos( iPosX+37  , iPosY+153  , iPosX+117, iPosY+173 ) );
   OngletPR_Button.SetPos ( FramePos( iPosX+120 , iPosY+153  , iPosX+200, iPosY+173 ) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_StatsDlg::Draw(V2SPRITEFX *vsfFX)
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

   

   HDC hdc;
   DXDGetDC(&hdc, 84);

   statsCommit_Button.DrawTextButtonExt(hdc);
   IteractionRPButton.DrawTextButtonExt(hdc);
   IteractionPVPButton.DrawTextButtonExt(hdc);
   addMacro_Button.DrawTextButtonExt(hdc);
   OngletStat_Button.DrawTextButtonExt(hdc);
   OngletPR_Button.DrawTextButtonExt(hdc);
   MOnglet01Button.DrawTextButtonExt(hdc);
   MOnglet02Button.DrawTextButtonExt(hdc);
   MOnglet03Button.DrawTextButtonExt(hdc);
   MOnglet04Button.DrawTextButtonExt(hdc);
   MOnglet05Button.DrawTextButtonExt(hdc);
   MOnglet06Button.DrawTextButtonExt(hdc);
   MOnglet07Button.DrawTextButtonExt(hdc);
   MOnglet08Button.DrawTextButtonExt(hdc);
   
   // Draw les texte...

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_LocalString[712],strlen(g_LocalString[712]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 84);
}

void V3_StatsDlg::Hide()
{
   BoxUI::Hide();
}


void V3_StatsDlg::UpdateCharacterSheetHP()
{
   char buf[ 256 ];
   //HP
   if( Player.TrueMaxHp != Player.MaxHp )
   {
      _itoa_s( Player.Hp, buf,256, 10 );
      string str = buf;
      str += " / ";
      _itoa_s( Player.MaxHp, buf,256, 10 );
      str += buf;
      str += "(";
      _itoa_s( Player.TrueMaxHp, buf,256, 10 );
      str += buf;
      str += ")";
      m_statR[2].GetHelpText()->SetText( str.c_str(),g_DefColorD );
   }
   else
   {
      m_statR[2].GetHelpText()->ResetText("");
   }
}



void V3_StatsDlg::UpdateCharacterSheetMP()
{
   char buf[ 256 ];
   //MP
   if( Player.TrueMaxMana != Player.MaxMana )
   {
      _itoa_s( Player.Mana, buf,256, 10 );
      string str = buf;
      str += " / ";
      _itoa_s( Player.MaxMana, buf,256, 10 );
      str += buf;
      str += "(";
      _itoa_s( Player.TrueMaxMana, buf,256, 10 );
      str += buf;
      str += ")";
      m_statR[3].GetHelpText()->SetText( str.c_str(),g_DefColorD );
   }
   else
   {
      m_statR[3].GetHelpText()->ResetText("");
   }
}

void V3_StatsDlg::UpdateCharacterSheetWeight()
{
   char strTmp[256];
   sprintf_s(strTmp,256,"%d/%d",Player.Weight ,Player.MaxWeight );
   m_statR[4].SetText( strTmp,g_DefColorD);
}

void V3_StatsDlg::UpdateCharacterSheetStat()
{
   char buf[ 256 ];
   char strTmp[6][ 256 ];

   //Left Stats
   sprintf_s(strTmp[0],256,"%d/%d",Player.bStr + m_iUpgSTR,Player.Str + m_iUpgSTR);
   sprintf_s(strTmp[1],256,"%d/%d",Player.bEnd + m_iUpgEND,Player.End + m_iUpgEND);
   sprintf_s(strTmp[2],256,"%d/%d",Player.bAgi + m_iUpgAGI,Player.Agi + m_iUpgAGI);
   sprintf_s(strTmp[3],256,"%d/%d",Player.bInt + m_iUpgINT,Player.Int + m_iUpgINT);
   sprintf_s(strTmp[4],256,"%d/%d",Player.bWis + m_iUpgWIS,Player.Wis + m_iUpgWIS);

   m_statL[0].SetText( strTmp[0], GetColor( Player.Str, Player.bStr, m_iUpgSTR ) );
   m_statL[1].SetText( strTmp[1], GetColor( Player.End, Player.bEnd, m_iUpgEND ) );
   m_statL[2].SetText( strTmp[2], GetColor( Player.Agi, Player.bAgi, m_iUpgAGI ) );
   m_statL[3].SetText( strTmp[3], GetColor( Player.Int, Player.bInt, m_iUpgINT ) );
   m_statL[4].SetText( strTmp[4], GetColor( Player.Wis, Player.bWis, m_iUpgWIS ) );

   m_PtsStat[0].SetText( g_GUILocalString[ 19 ],g_DefColorH );
   _itoa_s( Player.StatsPts - m_usedStatPts, buf,256, 10 );
   m_PtsStat[1].SetText( buf,g_DefColorD );
}

void V3_StatsDlg::UpdateCharacterSheet( char *pFromWho )
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );

   //char strTmpMsg[1000];
   //sprintf_s(strTmpMsg,1000,"UpdateCharCheet from %s\n",pFromWho);
   //OutputDebugString(strTmpMsg);


    char buf[ 256 ];
    char buf2[ 256 ];
    char strTmp[6][ 256 ];


    if(m_iSelectDisplay == 0)
    {
       m_BackgroundS.Show();
       m_backIconsS.Show();
       m_BackgroundPR.Hide();
       m_backIconsPR.Hide();

       //Left Stats
       sprintf_s(strTmp[0],256,"%d/%d",Player.bStr + m_iUpgSTR,Player.Str + m_iUpgSTR);
       sprintf_s(strTmp[1],256,"%d/%d",Player.bEnd + m_iUpgEND,Player.End + m_iUpgEND);
       sprintf_s(strTmp[2],256,"%d/%d",Player.bAgi + m_iUpgAGI,Player.Agi + m_iUpgAGI);
       sprintf_s(strTmp[3],256,"%d/%d",Player.bInt + m_iUpgINT,Player.Int + m_iUpgINT);
       sprintf_s(strTmp[4],256,"%d/%d",Player.bWis + m_iUpgWIS,Player.Wis + m_iUpgWIS);
       sprintf_s(strTmp[5],256,"%d",Player.Honor);
       
       m_statL[0].SetText( strTmp[0], GetColor( Player.Str, Player.bStr, m_iUpgSTR ) );
       m_statL[1].SetText( strTmp[1], GetColor( Player.End, Player.bEnd, m_iUpgEND ) );
       m_statL[2].SetText( strTmp[2], GetColor( Player.Agi, Player.bAgi, m_iUpgAGI ) );
       m_statL[3].SetText( strTmp[3], GetColor( Player.Int, Player.bInt, m_iUpgINT ) );
       m_statL[4].SetText( strTmp[4], GetColor( Player.Wis, Player.bWis, m_iUpgWIS ) );
       m_statL[5].SetText( strTmp[5],g_DefColorD);
       
       //Right Stats
       sprintf_s(strTmp[0],256,"%d/%d",Player.bAC ,Player.AC );
       sprintf_s(strTmp[1],256,"%d/%d",Player.bLck ,Player.Lck );
       sprintf_s(strTmp[2],256,"%d/%d",Player.Hp ,Player.MaxHp );
       sprintf_s(strTmp[3],256,"%d/%d",Player.Mana ,Player.MaxMana );
       sprintf_s(strTmp[4],256,"%d/%d",Player.Weight ,Player.MaxWeight );
       sprintf_s(strTmp[5],256,"%d",Player.Crime);
       
       m_statR[0].SetText( strTmp[0], GetColor( Player.AC, Player.bAC, 0 ) );
       m_statR[1].SetText( strTmp[1], GetColor( Player.Lck, Player.bLck, 0 ) );
       m_statR[2].SetText( strTmp[2], GetColor( Player.TrueMaxHp, Player.MaxHp, 0 ) );
       m_statR[3].SetText( strTmp[3], GetColor( Player.TrueMaxMana, Player.MaxMana, 0 ) );
       m_statR[4].SetText( strTmp[4],g_DefColorD);
       m_statR[5].SetText( strTmp[5],g_DefColorD);

       SetHelpId( m_hlpL[0], 262 );
       SetHelpId( m_hlpL[1], 263 );
       SetHelpId( m_hlpL[2], 264 );
       SetHelpId( m_hlpL[3], 266 );
       SetHelpId( m_hlpL[4], 265 );
       SetHelpId( m_hlpL[5], 581 );
       
       SetHelpId( m_hlpR[0], 445 );
       SetHelpId( m_hlpR[1], 596 );
       SetHelpId( m_hlpR[2], 286 );
       SetHelpId( m_hlpR[3], 287 );
       SetHelpId( m_hlpR[4], 486 );
       SetHelpId( m_hlpR[5], 582 );
    }
    else
    {
       m_BackgroundPR.Show();
       m_backIconsPR.Show();
       m_BackgroundS.Hide();
       m_backIconsS.Hide();
       
       
       
       

       //Left Stats
       sprintf_s(strTmp[0],256,"%d/%d",Player.iPBAir     ,Player.iPAir     );
       sprintf_s(strTmp[1],256,"%d/%d",Player.iPBFeu     ,Player.iPFeu     );
       sprintf_s(strTmp[2],256,"%d/%d",Player.iPBEau     ,Player.iPEau     );
       sprintf_s(strTmp[3],256,"%d/%d",Player.iPBTerre   ,Player.iPTerre   );
       sprintf_s(strTmp[4],256,"%d/%d",Player.iPBNoir    ,Player.iPNoir    );
       sprintf_s(strTmp[5],256,"%d/%d",Player.iPBLumiere ,Player.iPLumiere );
       
       m_statL[0].SetText( strTmp[0],GetColor( Player.iPAir    , Player.iPBAir    , 0 ));
       m_statL[1].SetText( strTmp[1],GetColor( Player.iPFeu    , Player.iPBFeu    , 0 ));
       m_statL[2].SetText( strTmp[2],GetColor( Player.iPEau    , Player.iPBEau    , 0 ));
       m_statL[3].SetText( strTmp[3],GetColor( Player.iPTerre  , Player.iPBTerre  , 0 ));
       m_statL[4].SetText( strTmp[4],GetColor( Player.iPNoir   , Player.iPBNoir   , 0 ));
       m_statL[5].SetText( strTmp[5],GetColor( Player.iPLumiere, Player.iPBLumiere, 0 ));

       
       //Right Stats
       sprintf_s(strTmp[0],256,"%d/%d",Player.iRBAir     ,Player.iRAir     );
       sprintf_s(strTmp[1],256,"%d/%d",Player.iRBFeu     ,Player.iRFeu     );
       sprintf_s(strTmp[2],256,"%d/%d",Player.iRBEau     ,Player.iREau     );
       sprintf_s(strTmp[3],256,"%d/%d",Player.iRBTerre   ,Player.iRTerre   );
       sprintf_s(strTmp[4],256,"%d/%d",Player.iRBNoir    ,Player.iRNoir    );
       sprintf_s(strTmp[5],256,"%d/%d",Player.iRBLumiere ,Player.iRLumiere );
       
       m_statR[0].SetText( strTmp[0],GetColor( Player.iRAir    , Player.iRBAir    , 0 ));
       m_statR[1].SetText( strTmp[1],GetColor( Player.iRFeu    , Player.iRBFeu    , 0 ));
       m_statR[2].SetText( strTmp[2],GetColor( Player.iREau    , Player.iRBEau    , 0 ));
       m_statR[3].SetText( strTmp[3],GetColor( Player.iRTerre  , Player.iRBTerre  , 0 ));
       m_statR[4].SetText( strTmp[4],GetColor( Player.iRNoir   , Player.iRBNoir   , 0 ));
       m_statR[5].SetText( strTmp[5],GetColor( Player.iRLumiere, Player.iRBLumiere, 0 ));
       
       SetHelpId( m_hlpL[0], 714 );
       SetHelpId( m_hlpL[1], 715 );
       SetHelpId( m_hlpL[2], 716 );
       SetHelpId( m_hlpL[3], 717 );
       SetHelpId( m_hlpL[4], 718 );
       SetHelpId( m_hlpL[5], 719 );
       
       SetHelpId( m_hlpR[0], 720 );
       SetHelpId( m_hlpR[1], 721 );
       SetHelpId( m_hlpR[2], 722 );
       SetHelpId( m_hlpR[3], 723 );
       SetHelpId( m_hlpR[4], 724 );
       SetHelpId( m_hlpR[5], 725 );
    }
    

    
    //HP
    if( Player.TrueMaxHp != Player.MaxHp )
    {
	   _itoa_s( Player.Hp, buf,256, 10 );
       string str = buf;
       str += " / ";
	   _itoa_s( Player.MaxHp, buf,256, 10 );
       str += buf;
       str += "(";
	   _itoa_s( Player.TrueMaxHp, buf,256, 10 );
       str += buf;
       str += ")";
       m_statR[2].GetHelpText()->SetText( str.c_str(),g_DefColorD );
    }
    else
    {
       m_statR[2].GetHelpText()->ResetText("");
    }

    //MP
    if( Player.TrueMaxMana != Player.MaxMana )
    {
	   _itoa_s( Player.Mana, buf,256, 10 );
       string str = buf;
       str += " / ";
	   _itoa_s( Player.MaxMana, buf,256, 10 );
       str += buf;
       str += "(";
	   _itoa_s( Player.TrueMaxMana, buf,256, 10 );
       str += buf;
       str += ")";
       m_statR[3].GetHelpText()->SetText( str.c_str(),g_DefColorD );
    }
    else
    {
       m_statR[3].GetHelpText()->ResetText("");
    }
    

    //XP
	_i64toa_s( Player.Exp2Go-Player.Exp, buf,256, 10 );
   StringToSpacesString(buf,buf2);
    m_XP[0].SetText( buf2 ,g_DefColorD);
  
	_i64toa_s( Player.Exp, buf,256, 10 );
   StringToSpacesString(buf,buf2);
    m_XP[1].SetText( buf2 ,g_DefColorD);
    

    m_XPT[0].SetText( g_GUILocalString[306] ,g_DefColorH);
    m_XPT[1].SetText( g_GUILocalString[307] ,g_DefColorH);

    //Playername et level
	_itoa_s( Player.Level, buf,256, 10 );
    string str = g_GUILocalString[368];
    str += buf;

    m_NomLvl[0].SetText( str  ,g_DefColorD);
    m_NomLvl[1].SetText( Player.Name  ,g_DefColorD);


    _itoa_s( Player.InteractionPts, buf,256, 10 );
    m_ptsInter.SetText( buf  ,g_DefColorD);

    _itoa_s( Player.InteractionPtsT, buf,256, 10 );
    m_ptsInterT.SetText( buf  ,g_DefColorD);

    //Sexe
    if(!Player.Sexx)
    {
       m_backIconsSH.Show();
       m_backIconsSF.Hide();
    }
    else
    {
       m_backIconsSH.Hide();
       m_backIconsSF.Show();
    }
    
    
    //Karma
    m_Karma[0].SetText( g_GUILocalString[ 22 ],g_DefColorH );

    if( Player.Karma >= 400 )
       m_Karma[1].SetText( g_LocalString[ 300 ],g_DefColorD );
    else if( Player.Karma >= 300 )
       m_Karma[1].SetText( g_LocalString[ 301 ],g_DefColorD );
    else if( Player.Karma >= 200 )
       m_Karma[1].SetText( g_LocalString[ 302 ],g_DefColorD );
    else if( Player.Karma >= 100 )
       m_Karma[1].SetText( g_LocalString[ 303 ],g_DefColorD );
    else if( Player.Karma >= -100 )
       m_Karma[1].SetText( g_LocalString[ 304 ],g_DefColorD );
    else if( Player.Karma >= -200 )
       m_Karma[1].SetText( g_LocalString[ 305 ],g_DefColorD );
    else if( Player.Karma >= -300 )
       m_Karma[1].SetText( g_LocalString[ 306 ],g_DefColorD );
    else if( Player.Karma >= -400 )
       m_Karma[1].SetText( g_LocalString[ 307 ],g_DefColorD );
    else
       m_Karma[1].SetText( g_LocalString[ 308 ],g_DefColorD );

    //
        
    
    m_PtsStat[0].SetText( g_GUILocalString[ 19 ],g_DefColorH );
	_itoa_s( Player.StatsPts - m_usedStatPts, buf,256, 10 );
    m_PtsStat[1].SetText( buf,g_DefColorD );

   
    m_PtsSKill[0].SetText( g_GUILocalString[ 18 ],g_DefColorH );
	_itoa_s( Player.SkillPts, buf,256, 10 );
    m_PtsSKill[1].SetText( buf,g_DefColorD );
    
     
    if( Player.StatsPts != 0 && m_iSelectDisplay == 0)
    {
        upgSTR_Button.Show();
        upgEND_Button.Show();
        upgAGI_Button.Show();
        upgWIS_Button.Show();
        upgINT_Button.Show();
        dngSTR_Button.Show();
        dngEND_Button.Show();
        dngAGI_Button.Show();
        dngWIS_Button.Show();
        dngINT_Button.Show();

        statsCommit_Button.Show();
    }
    else
    {
        upgSTR_Button.Hide();
        upgEND_Button.Hide();
        upgAGI_Button.Hide();
        upgWIS_Button.Hide();
        upgINT_Button.Hide();
        dngSTR_Button.Hide();
        dngEND_Button.Hide();
        dngAGI_Button.Hide();
        dngWIS_Button.Hide();
        dngINT_Button.Hide();

        statsCommit_Button.Hide();
    }
}

void V3_StatsDlg::UpdateSkills( void )
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   
   skillList.DeleteList();
   
   Player.tlUserSkill.Lock( "V3_StatsDlg::UpdateSkills" );
   Player.tlUserSkill.ToHead();
   while( Player.tlUserSkill.QueryNext() )
   {
      USER_SKILL *userSkill = Player.tlUserSkill.GetObject();
      SkillUI *skillUI = new SkillUI;
      DWORD color;
      
      if( userSkill->dwStrength == userSkill->dwTrueStrength )
         color = g_DefColorD; 
      else if( userSkill->dwStrength > userSkill->dwTrueStrength )
         color = CL_GREEN_DARK; // Dark green
      else
         color = CL_RED; // Dark red.
      
      char buf[ 1024 ];
      string skillDesc( " " );
      skillDesc += userSkill->desc;
      
      skillUI->SetVSF( SkillIconsV3( userSkill->wID ) );
      skillUI->userSkill = userSkill;
      // Set the pos equal to the text's pos.
      skillUI->SetPos( FramePos( 0, 0, skillUI->GetVSF()->GetWidth(), skillUI->GetVSF()->GetHeight() ) );
      skillUI->SetHelpText( FontManager::GetInstance()->GetSmallFont(skillDesc.c_str() ) );
      skillList.AddNewItem();
      skillList.AddColumnItem( 0, skillUI );
      
      string skillVal( " " );
	  _itoa_s( userSkill->dwStrength, buf,1024, 10 );
      skillVal  =  buf;
      skillVal += " / ";
	  _itoa_s( userSkill->dwTrueStrength, buf,1024, 10 );
      skillVal += buf;   
      
      StaticTextUI *skillUIuse = new StaticTextUI;
      if( userSkill->bUse == 0 )
      {
         skillUIuse->SetText( "", color );
      }
      else
      {
         skillUIuse->SetText( "", color );
      }
      // Set the pos equal to the text's pos.
      skillUIuse->SetPos( FramePos( 0, 0, skillUIuse->GetWidth(), skillUIuse->GetHeight() ) );
      // The list UI will be responsible for centering this item.
      skillUIuse->SetCenter( false );
      skillUIuse->SetHelpText( FontManager::GetInstance()->GetSmallFont(skillDesc.c_str() ) );
      skillList.AddColumnItem( 1, skillUIuse );

      StaticTextUI *skillUIst = new StaticTextUI;
      skillUIst->SetText( skillVal, color );
      // Set the pos equal to the text's pos.
      skillUIst->SetPos( FramePos( 0, 0, skillUIst->GetWidth(), skillUIst->GetHeight() ) );
      // The list UI will be responsible for centering this item.
      skillUIst->SetCenter( false );
      skillUIst->SetHelpText( FontManager::GetInstance()->GetSmallFont(skillDesc.c_str() ) );
      skillList.AddColumnItem( 2, skillUIst );
   }
   Player.tlUserSkill.Unlock( "V3_StatsDlg::UpdateSkills" );
}




DWORD V3_StatsDlg::GetColor(int iStat,int iStatB,int iUpg)
{
    DWORD color;
    
    if( iStatB > iStat )// If stat is boosted.
    {
        if( iUpg != 0 )
            color = CL_GREEN_LIGHT;
        else
            color = CL_GREEN; //Green
    }
    else if( iStatB < iStat )// If stat is under normal.
    {
        if( iUpg != 0 )
            color = CL_RED_LIGHT;
        else
            color = CL_RED; // Red
    }
    else // If stat is normal
    {
        // If the stat was upgraded. (stat train).
        if( iUpg != 0 )
            color = CL_WHITE; // Bright white.
        else
            color = g_DefColorD; 
    }
   
    return color;
}


void V3_StatsDlg::StartDragDlg( MousePos mousePos )
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

void V3_StatsDlg::StopDragDlg()
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


void V3_StatsDlg::ManageSkillMacroButton(SkillUI *skillUI)
{
   
   LPUSER_SKILL skillItem = skillUI->userSkill;
   if(skillItem->bUse )
   {
      addMacro_Button.Show();
   }
   else
   {
      addMacro_Button.Hide();
   }
}

void V3_StatsDlg::UseSkill(SkillUI *skillUI)
{
   TFCPacket sending;
   
   LPUSER_SKILL skillItem = skillUI->userSkill;
   m_dwLastSkill = skillItem->wID;
   char buf[ 256 ];
   
   switch( skillItem->bUse )
   {
      case 0: // Skill cannot be used.
         naSound->Play( TRUE );
         V3_ChatLogDlg::GetInstance()->AddListLogs( "",g_LocalString[ 440 ],SYSTEM_COLOR,true);
      break;
      case 1: // Skill used without any parameters.
         sprintf_s( buf,256, g_LocalString[ 439 ], skillItem->SkillName.c_str() );  
         V3_ChatLogDlg::GetInstance()->AddListLogs( "",buf,SYSTEM_COLOR,true);

         // Just use the skill. Just do it!
         sending << (RQ_SIZE)RQ_UseSkill;
         sending << (short)skillItem->wID;
         sending << (short)0;
         sending << (short)0;
         sending << (long) 0;
      
         SEND_PACKET( sending );
      break;
      case 2: // Skill used using a world position.
      {
         // Set a lock on the mouse
         MouseDevice md;
         if( md.LockNextEvent( DM_CLICK, skillItem, UseSkillPosition, 0 ) )
         {
            if(skillItem->wID != 22)
            {
               sprintf_s( buf,256, g_LocalString[ 441 ], skillItem->SkillName.c_str() );
               V3_ChatLogDlg::GetInstance()->AddListLogs( "",buf,SYSTEM_COLOR,true);
            }
            
            if( IsShown() )
               Hide();
         }
         else
         {
            // Could not lock mouse
            naSound->Play( TRUE );
            V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 444 ], SYSTEM_COLOR, true);
         }
      
      }
      break;
      
      case 3: // Skill used using a unit.
      {
         // Set a lock on the mouse
         MouseDevice md;
         if( md.LockNextEvent( DM_CLICK, skillItem, UseSkillUnit, 0 ) )
         {
            if(skillItem->wID != 22)
            {
               sprintf_s( buf,256, g_LocalString[ 441 ], skillItem->SkillName.c_str() );
               V3_ChatLogDlg::GetInstance()->AddListLogs( "",buf,SYSTEM_COLOR,true);
            }
            if( IsShown() )
               Hide();
         }
         else
         {
            // Could not lock mouse.
            naSound->Play( TRUE );
            V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 444 ], SYSTEM_COLOR, true);
         }
      }
      break;
   }        
}

//////////////////////////////////////////////////////////////////////////////////////////
// Called by the mouse event handler when the user has clicked on a position.
// 
// Return: static UINT, 0
//////////////////////////////////////////////////////////////////////////////////////////
UINT V3_StatsDlg::UseSkillPosition(LPVOID lpData)
{
   LPUSER_SKILL userSkill = reinterpret_cast< LPUSER_SKILL >( lpData );
   
   CMouseCursor::GetInstance()->SetCustomCursor( NULL );
  
   // Send the use skill with a world position but without a unit ID.
   TFCPacket sending;
   POINT mousePos;
   
   // Get the mouse pos.
   MouseDevice md;
   md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
   
   // then convert the mouse coordinates into world coordinates
   Player.ScreenPosToWL( &mousePos, &mousePos );	
   
   sending << (RQ_SIZE)RQ_UseSkill;
   sending << (short)userSkill->wID;
   
   signed char xPos, yPos;	
   Objects.Lock(2000);
   if (TargetID == Player.ID) 
   {
      sending << (short)( Player.xPos );
      sending << (short)( Player.yPos );
      sending << (long)TargetID;
   } 
   else if (Objects.Found( &xPos, &yPos, TargetID )) 
   {
      sending << (short)( xPos+Player.xPos );
      sending << (short)( yPos+Player.yPos );
      sending << (long)TargetID;
   } 
   else 
   {
      sending << (short)( mousePos.x );
      sending << (short)( mousePos.y );
      sending << (long)NULL;
   }
   Objects.Unlock(2000);
   
   SEND_PACKET( sending );
   return 0;
}
	
//////////////////////////////////////////////////////////////////////////////////////////
// Called by the mouse event handler when the user has clicked on a unit.
// 
// Return: 	static UINT, 0
//////////////////////////////////////////////////////////////////////////////////////////
UINT V3_StatsDlg::UseSkillUnit(LPVOID lpData)
{
   CMouseCursor::GetInstance()->SetCustomCursor( NULL );
   POINT mousePos;
   LPUSER_SKILL userSkill = reinterpret_cast< LPUSER_SKILL >( lpData );
   
  
   TFCPacket sending;
   sending << (RQ_SIZE)RQ_UseSkill;
   sending << (short)userSkill->wID;
   
   // Get the mouse pos.
   MouseDevice md;
   md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
   
   // Get the world pos.
   Player.ScreenPosToWL( &mousePos, &mousePos );
   
   signed char xPos, yPos;	
   Objects.Lock(2000);
   if (TargetID == Player.ID) 
   {
      sending << (short)( Player.xPos );
      sending << (short)( Player.yPos );
      sending << (long)TargetID;
      SEND_PACKET( sending );
   } 
   else if (Objects.Found( &xPos, &yPos, TargetID )) 
   {
      sending << (short)( xPos+Player.xPos );
      sending << (short)( yPos+Player.yPos );
      sending << (long)TargetID;
      SEND_PACKET( sending );
   } 
   else 
   {
      //      sending << (short)( mousePos.x );
      //      sending << (short)( mousePos.y );
      //      sending << (long)NULL;
   }
   Objects.Unlock(2000);
   return 0;
}


void V3_StatsDlg::CastLastSkill( void )
{
   vector< vector< ListUI::GridItem > > &skillGrid = *skillList.GetGrid();
   
   // Search for the skill in the skill list.
   for(UINT i = 0; i < skillGrid[ 0 ].size(); i++ )
   {
      SkillUI *skillUI = static_cast< SkillUI * >( skillGrid[ 0 ][ i ] );
      
      if( skillUI->userSkill->wID == m_dwLastSkill)
      {
         if(skillUI->userSkill->bUse == 2 || skillUI->userSkill->bUse == 3)
         {
            LPUSER_SKILL skillItem = skillUI->userSkill;
            UseSkillPosition( skillItem );
         }
         else
         {
            UseSkill( skillUI );
         }
         
         return;
      }
   }
}



///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_StatsDlg::DragEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_StatsDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


///////////////////////////////////////////////////////
// UPGRADE UP / DOWN SPIN BUTTON
///////////////////////////////////////////////////////
void V3_StatsDlg::StatUpgEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   int iSpinNbr =  me->upgSTR_Button.GetSpinCnt();
   iSpinNbr += me->upgEND_Button.GetSpinCnt();
   iSpinNbr += me->upgAGI_Button.GetSpinCnt();
   iSpinNbr += me->upgWIS_Button.GetSpinCnt();
   iSpinNbr += me->upgINT_Button.GetSpinCnt();
   
   
   static int iStepUPStat  = 1;
   static int iStepCntStat = 0;
   if(iSpinNbr == 0)
   {
      iStepUPStat = 1;
      iStepCntStat= 0;
   }
   else if(++iStepCntStat >10)
   {
      iStepCntStat= 0;
      iStepUPStat*=2;
      if(iStepUPStat >100)
         iStepUPStat = 100;
   }
   
   if(me->m_usedStatPts+iStepUPStat >  Player.StatsPts)
      iStepUPStat =  Player.StatsPts-me->m_usedStatPts;
   
   
   if( me->m_usedStatPts+iStepUPStat <= Player.StatsPts )
   {
      
      int *statPtr;
      
      switch( stat )
      {
         case STRid:  statPtr = &me->m_iUpgSTR; break;
         case ENDid:  statPtr = &me->m_iUpgEND; break;
         case AGIid:  statPtr = &me->m_iUpgAGI; break;
         case INTid:  statPtr = &me->m_iUpgINT; break;
         case WISid:  statPtr = &me->m_iUpgWIS; break;
         default: return;
      }
      
      (*statPtr)+=iStepUPStat;
      
      me->m_usedStatPts+=iStepUPStat;
      
      me->UpdateCharacterSheetStat();
   }
   else
   {
      naSound->Play( TRUE );
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Left clicked on a stat downgrade event. ANd COmmit Button
// 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_StatsDlg::StatDngEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   int *statPtr;
   
   switch( stat )
   {
      case STRid:  statPtr = &me->m_iUpgSTR; break;
      case ENDid:  statPtr = &me->m_iUpgEND; break;
      case AGIid:  statPtr = &me->m_iUpgAGI; break;
      case INTid:  statPtr = &me->m_iUpgINT; break;
      case WISid:  statPtr = &me->m_iUpgWIS; break;
      default: return;
   }
   
   if( *statPtr > 0 )
   {
      (*statPtr)--;
      
      me->m_usedStatPts--;
      me->UpdateCharacterSheetStat();
   }
   else
   {
      naSound->Play( TRUE );
   }
}



//////////////////////////////////////////////////////////////////////////////////////////
void V3_StatsDlg::IteractionRPEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   me->Hide();

   TFCPacket packet;
   packet << (RQ_SIZE)RQ_RP_BroadCastRP;
   SEND_PACKET( packet );

   V3_InteractionRPDlg *pIntRPDlg = V3_InteractionRPDlg::GetInstance();
   pIntRPDlg->Display(NULL);

}

void V3_StatsDlg::IteractionPVPEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   me->Hide();

   if(V3_ArenePointsDlg::GetInstance()->IsShown())
   {
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_ARENA1_GetWaitPlayerList;
      sending << (long)V3_ArenePointsDlg::GetInstance()->GetArenaID();
      sending << (long)V3_ArenePointsDlg::GetInstance()->GetArenaType();
      SEND_PACKET( sending );
   }
   else
   {
      TFCPacket packet;
      packet << (RQ_SIZE)RQ_RP_BroadCastPVP;
      SEND_PACKET( packet );

      V3_InteractionPVPDlg *pIntPVPDlg = V3_InteractionPVPDlg::GetInstance();
      pIntPVPDlg->Display(NULL);
   }
   

  

}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_StatsDlg::StatsCommitEvent::LeftClicked( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Someone pushed the stat commit button!!.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   bool send = false;
   
   // If any of the stats changed.
   if( me->m_iUpgSTR != 0 ||
       me->m_iUpgEND != 0 ||
       me->m_iUpgAGI != 0 ||
       me->m_iUpgINT != 0 ||
       me->m_iUpgWIS != 0 )
   {
      
      // Send a stat train packet.
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_SendStatTrain;
      
      sending << (short)( me->m_iUpgSTR ); // Str
      sending << (short)( me->m_iUpgEND ); // End
      sending << (short)( me->m_iUpgAGI ); // Agi
      sending << (short)( 0 );             // Wil
      sending << (short)( me->m_iUpgWIS ); // Wis
      sending << (short)( me->m_iUpgINT ); // Int
      sending << (short)( 0 );             // Lck	    
      
      SEND_PACKET( sending );
      
      // Assume the packet will pass and update the stats before receiving an update.
      Player.Str += me->m_iUpgSTR;
      Player.End += me->m_iUpgEND;
      Player.Agi += me->m_iUpgAGI;
      Player.Int += me->m_iUpgINT;
      Player.Wis += me->m_iUpgWIS;
      
      Player.bStr += me->m_iUpgSTR;
      Player.bEnd += me->m_iUpgEND;
      Player.bAgi += me->m_iUpgAGI;
      Player.bInt += me->m_iUpgINT;
      Player.bWis += me->m_iUpgWIS;
      
      Player.StatsPts -= me->m_usedStatPts;
      
      me->m_iUpgSTR = 0;
      me->m_iUpgEND = 0;
      me->m_iUpgAGI = 0;
      me->m_iUpgWIS = 0;
      me->m_iUpgINT = 0;
      
      me->m_usedStatPts = 0;	    
      
      me->UpdateCharacterSheet("OK-StatsCommitEvent::LeftClicked");
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_StatsDlg::AddMacroEvent::LeftClicked( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Someone pushed the add Macro button!!.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   SkillUI *skillUI = static_cast< SkillUI * >( me->skillList.GetSelection() );
   
   if( skillUI == NULL )
   {
      naSound->Play( TRUE );
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 351 ], SYSTEM_COLOR, false );
      return;
   }
   
   if( skillUI->userSkill->bUse == 0 )
   {
      naSound->Play( TRUE );
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 352 ], SYSTEM_COLOR, false );
      return;
   }
   
   SkillMacroUI *macroUI = new SkillMacroUI;
   
   macroUI->skillId = skillUI->userSkill->wID;
   macroUI->name    = skillUI->userSkill->SkillName;
   
   macroUI->SetVSF( SkillIconsV3( macroUI->skillId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for skill.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, skillUI->userSkill->SkillName,&me->skillMacroEvent );
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_StatsDlg::SkillEvent::LeftDblClicked( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Left dbl clicked on skill
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
   
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   GameUI *skill = me->skillList.GetSelection();
   
   if( skill == NULL )
      return;

   
   if( typeid( *skill ) == typeid( SkillUI ) )
   {
      SkillUI *skillUI = static_cast< SkillUI * >( skill );
      me->UseSkill( skillUI );
   } 
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_StatsDlg::SkillEvent::LeftClicked( void )
//////////////////////////////////////////////////////////////////////////////////////////
//   Left clicked on skill
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   GameUI *skill = me->skillList.GetSelection();
   
   if( skill == NULL )
      return;

   if( typeid( *skill ) == typeid( SkillUI ) )
   {
      SkillUI *skillUI = static_cast< SkillUI * >( skill );
      me->ManageSkillMacroButton( skillUI );
   } 
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Executes a spell macro.
// 
void V3_StatsDlg::SkillMacroEvent::ExecMacro(MacroDataUI *imacroUI)
{
   V3_StatsDlg *me = V3_StatsDlg::GetInstance();

   SkillMacroUI *macroUI = static_cast< SkillMacroUI * >( imacroUI );
   
   vector< vector< ListUI::GridItem > > &skillGrid = *me->skillList.GetGrid();

   // Search for the skill in the skill list.
   for(UINT i = 0; i < skillGrid[ 0 ].size(); i++ )
   {
      SkillUI *skillUI = static_cast< SkillUI * >( skillGrid[ 0 ][ i ] );
      if( skillUI->userSkill->wID == macroUI->skillId )
      {
         me->UseSkill( skillUI );// Use the skill.
         return;
      }
   }
   
   naSound->Play( TRUE );
   V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 353 ], SYSTEM_COLOR, true );
}

//////////////////////////////////////////////////////////////////////////////////////////
// Saves the given macro.
// 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_StatsDlg::SkillMacroEvent::SaveMacro(const list< MacroDataUI * > &macroList)
{
   list< MacroDataUI * >::const_iterator i;
   for( i = macroList.begin(); i != macroList.end(); i++ )
   {
      const SkillMacroUI *macroUI = static_cast< const SkillMacroUI * >( *i );
      sMacro sNewMacro;
      
      sNewMacro.dwID       = macroUI->skillId;
      sNewMacro.dwIcon     = macroUI->skillId;
      memcpy(sNewMacro.chFastPos,macroUI->chGridPos,g_iNbrMacro);
      sNewMacro.dwKey      = macroUI->vKey.AsLong();
      sprintf_s(sNewMacro.strMacroName,256,"%s",macroUI->name.c_str());
      g_SaveGame.AddMacroSkill(sNewMacro);
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Client initializes, load the skill macros.
// 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_StatsDlg::InitializeMacro( void )
{
   int dwNbrSkillMacro = g_SaveGame.GetNbrMSkill();
   if(dwNbrSkillMacro > 0)
   {
      for(int i =0;i<dwNbrSkillMacro; i++ )
      {
         SkillMacroUI *macroUI = new SkillMacroUI;
         
         sMacro *pNewMacro = g_SaveGame.GetMSkill(i);
         
         macroUI->skillId     = pNewMacro->dwID;
         memcpy(macroUI->chGridPos,pNewMacro->chFastPos,g_iNbrMacro);
         macroUI->name        = pNewMacro->strMacroName;
         macroUI->vKey.SetLong(pNewMacro->dwKey);
         
         macroUI->SetVSF( SkillIconsV3( macroUI->skillId ) );
         macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
         
         if( macroUI->skillId == 0)
            delete macroUI;
         else // Add the macro
            V3_MacroDlg::GetInstance()->AddMacro(macroUI, macroUI->name, &skillMacroEvent, false);
      }
   }
}

void V3_StatsDlg::OngletStatEvent::LeftClicked( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Someone pushed the add Macro button!!.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   me->m_iSelectDisplay = 0;
   me->UpdateCharacterSheet("OK-OngletStatEvent::LeftClicked");
   
}

//  Someone pushed the add Macro button!!.
void V3_StatsDlg::OngletPREvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   me->m_iSelectDisplay = 1;
   me->UpdateCharacterSheet("OK-OngletPREvent::LeftClicked");
}

// Personnage/Inventaire/Sorts/Professions/Guilde/Groupe/Macros/Options








/*
// void V3_StatsDlg::TabStatsEvent::LeftClicked( void )
// {
//    V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
//    
// }
*/

void V3_StatsDlg::TabInvEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );

   V3_InvDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_StatsDlg::TabSpellEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   V3_SpellDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_StatsDlg::TabProfessionEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
 
   V3_ProfessionDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_StatsDlg::TabGuildeEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );

   V3_GuildDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_StatsDlg::TabGroupEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );

   V3_GroupeDlg::GetInstance()->Display(NULL);
   me->Hide();
   
}

void V3_StatsDlg::TabMacroEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );

   V3_MacroDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_StatsDlg::TabOptionEvent::LeftClicked( void )
{
   V3_StatsDlg *me = static_cast< V3_StatsDlg * >( GetUI() );
   
   V3_OptionsDlg::GetInstance()->Display(NULL);
   me->Hide();
}


void V3_StatsDlg::StringToSpacesString(char *pSource,char *pDest)
{
   int iNbrChar = strlen(pSource);
   int iReste   = iNbrChar%3;
   int iNbrS    = iNbrChar/3;

   int iD = 0;
   int iS = 0;
   //on commence par copier le reste
   for(int kk=0;kk<iReste;kk++)
   {
      pDest[iD] = pSource[iS];
      iD++; iS++;
   }
   if(iReste > 0)
   {
      pDest[iD] = ' '; 
      iD++;
   }
   for(int kk=0;kk<iNbrS;kk++)
   {
      pDest[iD] = pSource[iS];
      iD++;iS++;
      pDest[iD] = pSource[iS];
      iD++;iS++;
      pDest[iD] = pSource[iS];
      iD++;iS++;
      if(kk+1<iNbrS)
      {
         pDest[iD] = ' '; 
         iD++;
      }
      else
         pDest[iD] = 0x00;
   }
}