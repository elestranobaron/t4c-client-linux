// V3_SpellDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_SpellDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"

#include "V3_StatDlg.h"
#include "V3_OptionsDlg.h"
#include "V3_MacroDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_GroupeDlg.h"
#include "V3_GuildDlg.h"
#include "V3_ProfessionDlg.h"
#include "V3_InvDlg.h"

extern DWORD TargetID;
extern DWORD SelectedID;
extern CSaveGame g_SaveGame;
 
DWORD g_dwLastMacroCallSpell = 0;

namespace
{
    struct SpellCallbackData
    {
        SpellCallbackData( const V3_SpellDlg::Spell &ispell, bool allowSelf )
            : spell( ispell ), allowSelf( allowSelf ), targetPriority( V3_SpellDlg::equalPriority )
        {};
        V3_SpellDlg::Spell spell;
        bool     allowSelf;
        int      targetPriority;
    };

    T3VSBSound *turnPageSound = NULL;
    T3VSBSound *noMorePageSound = NULL;
    T3VSBSound *selectSound = NULL;

    void InitSound()
    {
        if( turnPageSound == NULL )
        {
            turnPageSound = new T3VSBSound;
            turnPageSound->Create( "Page turning sound", TS_MEMORY );
            
            noMorePageSound = new T3VSBSound;
            noMorePageSound->Create( "General NA sound", TS_MEMORY );
            
            selectSound = new T3VSBSound;
            selectSound->Create( "Generic Drop Item", TS_MEMORY );
        }
    }
};

bool V3_SpellDlg::Spell::operator < ( const Spell &spell2 ) const
{
   if( element < spell2.element )
   {
      return true;
   }
   else if( element > spell2.element )
   {
      return false;
   }
   return( level < spell2.level );
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_SpellDlg::V3_SpellDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_btnLArrow(lArrowEvent),
m_btnRArrow(rArrowEvent),
MOnglet01Button(tabStatsEvent            ,g_GUILocalString[309],g_DefColorH ),
MOnglet02Button(tabInvEvent              ,g_GUILocalString[310],g_DefColorH ),
MOnglet03Button(EmptyEvent::GetInstance(),g_GUILocalString[311],g_DefColorH ),
MOnglet04Button(tabProfessionEvent       ,g_GUILocalString[312],g_DefColorH ),
MOnglet05Button(tabGuildeEvent           ,g_GUILocalString[313],g_DefColorH ),
MOnglet06Button(tabGroupEvent            ,g_GUILocalString[314],g_DefColorH ),
MOnglet07Button(tabMacroEvent            ,g_GUILocalString[315],g_DefColorH ),
MOnglet08Button(tabOptionEvent           ,g_GUILocalString[316],g_DefColorH ),
m_iCurrentPage (0),
spellEvent0( 0 ),
spellEvent1( 1 ),
spellEvent2( 2 ),
spellEvent3( 3 ),
spellEvent4( 4 ),
spellEvent5( 5 ),
spellEvent6( 6 ),
spellEvent7( 7 ),
spellEvent8( 8 ),
spellEvent9( 9 ),
PageSpell0(spellEvent0),
PageSpell1(spellEvent1),
PageSpell2(spellEvent2),
PageSpell3(spellEvent3),
PageSpell4(spellEvent4),
PageSpell5(spellEvent5),
PageSpell6(spellEvent6),
PageSpell7(spellEvent7),
PageSpell8(spellEvent8),
PageSpell9(spellEvent9),
m_btnM0 (m0ButtonEvent),
m_btnM1 (m1ButtonEvent),
m_btnM2 (m2ButtonEvent),
m_btnM3 (m3ButtonEvent),
m_btnM4 (m4ButtonEvent),
m_btnM5 (m5ButtonEvent),
m_btnM6 (m6ButtonEvent),
m_btnM7 (m7ButtonEvent),
m_btnM8 (m8ButtonEvent),
m_btnM9 (m9ButtonEvent),
airEvent( Spell::air ),
waterEvent( Spell::water ),
earthEvent( Spell::earth ),
fireEvent( Spell::fire ),
darkEvent( Spell::dark ),
lightEvent( Spell::light ),
normalEvent( Spell::normal ),
m_btnAir   ( airEvent ),
m_btnFire  ( fireEvent ),
m_btnEarth ( earthEvent ),
m_btnWater ( waterEvent ),
m_btnLight ( lightEvent ),
m_btnDark  ( darkEvent ),
m_btnNormal( normalEvent )
{
   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_SpellBack",true,"V3_SpellBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_SpellIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );

   m_btnLArrow.SetSpriteId(   "V3_SpellLArrow_N");
   m_btnLArrow.SetHighlightUI("V3_SpellLArrow_H");
   m_btnRArrow.SetSpriteId(   "V3_SpellRArrow_N");
   m_btnRArrow.SetHighlightUI("V3_SpellRArrow_H");
   SetHelpId( m_btnLArrow, 346 );
   SetHelpId( m_btnRArrow, 347 );
   
   lArrowEvent.SetUI    ( this );
   rArrowEvent.SetUI    ( this );
   
  

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
   //tabSpellEvent     .SetUI(this);
   tabProfessionEvent.SetUI(this);
   tabGuildeEvent    .SetUI(this);
   tabGroupEvent     .SetUI(this);
   tabMacroEvent     .SetUI(this);
   tabOptionEvent    .SetUI(this);



   SetHelpId( PageSpell0, 349 );
   SetHelpId( PageSpell1, 349 );
   SetHelpId( PageSpell2, 349 );
   SetHelpId( PageSpell3, 349 );
   SetHelpId( PageSpell4, 349 );
   SetHelpId( PageSpell5, 349 );
   SetHelpId( PageSpell6, 349 );
   SetHelpId( PageSpell7, 349 );
   SetHelpId( PageSpell8, 349 );
   SetHelpId( PageSpell9, 349 );
   
   
   
   spellEvent0.SetUI( this );
   spellEvent1.SetUI( this );
   spellEvent2.SetUI( this );
   spellEvent3.SetUI( this );
   spellEvent4.SetUI( this );
   spellEvent5.SetUI( this );
   spellEvent6.SetUI( this );
   spellEvent7.SetUI( this );
   spellEvent8.SetUI( this );
   spellEvent9.SetUI( this );



   V3CastCursor.CreateSpriteMain( "Handmg0000","Handmg0000Mask");
   V3CastCursor.AddFrame(         "Handmg0001","Handmg0001Mask");
   V3CastCursor.AddFrame(         "Handmg0002","Handmg0002Mask");
   V3CastCursor.AddFrame(         "Handmg0003","Handmg0003Mask");
   V3CastCursor.AddFrame(         "Handmg0004","Handmg0004Mask");
   V3CastCursor.AddFrame(         "Handmg0005","Handmg0005Mask");
   V3CastCursor.AddFrame(         "Handmg0006","Handmg0006Mask");
   V3CastCursor.AddFrame(         "Handmg0007","Handmg0007Mask");
   V3CastCursor.AddFrame(         "Handmg0008","Handmg0008Mask");
   V3CastCursor.AddFrame(         "Handmg0009","Handmg0009Mask");
   V3CastCursor.AddFrame(         "Handmg0010","Handmg0010Mask");
   V3CastCursor.AddFrame(         "Handmg0011","Handmg0011Mask");
   V3CastCursor.AddFrame(         "Handmg0012","Handmg0012Mask");
   V3CastCursor.AddFrame(         "Handmg0013","Handmg0013Mask");
   V3CastCursor.AddFrame(         "Handmg0014","Handmg0014Mask");
   V3CastCursor.AddFrame(         "Handmg0015","Handmg0015Mask");
   V3CastCursor.AddFrame(         "Handmg0016","Handmg0016Mask");
   V3CastCursor.AddFrame(         "Handmg0017","Handmg0017Mask");
   V3CastCursor.AddFrame(         "Handmg0018","Handmg0018Mask");
   V3CastCursor.AddFrame(         "Handmg0019","Handmg0019Mask");
   V3CastCursor.AddFrame(         "Handmg0020","Handmg0020Mask");
   V3CastCursor.AddFrame(         "Handmg0021","Handmg0021Mask");
   V3CastCursor.AddFrame(         "Handmg0022","Handmg0022Mask");
   V3CastCursor.AddFrame(         "Handmg0023","Handmg0023Mask");

   /*
   V3CastCursor.CreateSprite( "64kCursorSpell-a" );
   castCursor.AddFrame( "64kCursorSpell-b" );
   castCursor.AddFrame( "64kCursorSpell-c" );
   castCursor.AddFrame( "64kCursorSpell-d" );
   castCursor.AddFrame( "64kCursorSpell-e" );
   castCursor.AddFrame( "64kCursorSpell-f" );
   castCursor.AddFrame( "64kCursorSpell-g" );
   castCursor.AddFrame( "64kCursorSpell-h" );
   castCursor.AddFrame( "64kCursorSpell-i" );
   castCursor.AddFrame( "64kCursorSpell-j" );
   castCursor.AddFrame( "64kCursorSpell-k" );
   castCursor.AddFrame( "64kCursorSpell-l" );
   */


   m_btnAir   .SetSpriteId( "V3_SignetAir_N" );
   m_btnFire  .SetSpriteId( "V3_SignetFire_N" );
   m_btnEarth .SetSpriteId( "V3_SignetTerre_N" );
   m_btnWater .SetSpriteId( "V3_SignetWater_N" );
   m_btnLight .SetSpriteId( "V3_SignetLight_N" );
   m_btnDark  .SetSpriteId( "V3_SignetNeutre_H" );
   m_btnNormal.SetSpriteId( "V3_SignetDark_N" );

   m_btnAir   .SetHighlightUI( "V3_SignetAir_H" );
   m_btnFire  .SetHighlightUI( "V3_SignetFire_H" );
   m_btnEarth .SetHighlightUI( "V3_SignetTerre_H" );
   m_btnWater .SetHighlightUI( "V3_SignetWater_H" );
   m_btnLight .SetHighlightUI( "V3_SignetLight_H" );
   m_btnDark  .SetHighlightUI( "V3_SignetNeutre_N" );
   m_btnNormal.SetHighlightUI( "V3_SignetDark_H" );


   m_btnFire.Hide();

   SetHelpId( m_btnAir   , 510 );
   SetHelpId( m_btnFire  , 507 );
   SetHelpId( m_btnEarth , 509 );
   SetHelpId( m_btnWater , 508 );
   SetHelpId( m_btnLight , 511 );
   SetHelpId( m_btnDark  , 512 );
   SetHelpId( m_btnNormal, 513 );
   
   airEvent.SetUI( this );
   earthEvent.SetUI( this );
   fireEvent.SetUI( this );
   waterEvent.SetUI( this );
   lightEvent.SetUI( this );
   darkEvent.SetUI( this );
   normalEvent.SetUI( this );
   
   
   m_SignetAir   .SetSpriteId("V3_SignetBack",true,"V3_SignetBackMask");
   m_SignetFire  .SetSpriteId("V3_SignetBack",true,"V3_SignetBackMask");
   m_SignetEarth .SetSpriteId("V3_SignetBack",true,"V3_SignetBackMask");
   m_SignetWater .SetSpriteId("V3_SignetBack",true,"V3_SignetBackMask");
   m_SignetLight .SetSpriteId("V3_SignetBack",true,"V3_SignetBackMask");
   m_SignetDark  .SetSpriteId("V3_SignetBack",true,"V3_SignetBackMask");
   m_SignetNormal.SetSpriteId("V3_SignetBack",true,"V3_SignetBackMask");
   
   m_SignetFire.Hide();
   
   
   m_btnM0.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM0.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM0.SetHighlightUI( "V3_BtnPP_H" );
   m_btnM1.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM1.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM1.SetHighlightUI( "V3_BtnPP_H" );
   m_btnM2.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM2.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM2.SetHighlightUI( "V3_BtnPP_H" );
   m_btnM3.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM3.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM3.SetHighlightUI( "V3_BtnPP_H" );
   m_btnM4.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM4.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM4.SetHighlightUI( "V3_BtnPP_H" );
   m_btnM5.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM5.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM5.SetHighlightUI( "V3_BtnPP_H" );
   m_btnM6.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM6.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM6.SetHighlightUI( "V3_BtnPP_H" );
   m_btnM7.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM7.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM7.SetHighlightUI( "V3_BtnPP_H" );
   m_btnM8.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM8.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM8.SetHighlightUI( "V3_BtnPP_H" );
   m_btnM9.SetSpriteId   ( "V3_BtnPP_N" );
   m_btnM9.SetDownUI     ( "V3_BtnPP_D" );
   m_btnM9.SetHighlightUI( "V3_BtnPP_H" );
   
   
   m0ButtonEvent.SetUI( this );
   m1ButtonEvent.SetUI( this );
   m2ButtonEvent.SetUI( this );
   m3ButtonEvent.SetUI( this );
   m4ButtonEvent.SetUI( this );
   m5ButtonEvent.SetUI( this );
   m6ButtonEvent.SetUI( this );
   m7ButtonEvent.SetUI( this );
   m8ButtonEvent.SetUI( this );
   m9ButtonEvent.SetUI( this );

      
   SetHelpId( m_btnM0, 345 );
   SetHelpId( m_btnM1, 345 );
   SetHelpId( m_btnM2, 345 );
   SetHelpId( m_btnM3, 345 );
   SetHelpId( m_btnM4, 345 );
   SetHelpId( m_btnM5, 345 );
   SetHelpId( m_btnM6, 345 );
   SetHelpId( m_btnM7, 345 );
   SetHelpId( m_btnM8, 345 );
   SetHelpId( m_btnM9, 345 );
   
   
   UpdateAllFramePos(m_iXPos,m_iYPos);
   
   
   
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   
   AddChild( &MOnglet01Button );
   AddChild( &MOnglet02Button );
   AddChild( &MOnglet03Button );
   AddChild( &MOnglet04Button );
   AddChild( &MOnglet05Button );
   AddChild( &MOnglet06Button );
   AddChild( &MOnglet07Button );
   AddChild( &MOnglet08Button );

   AddChild( &PageSpell0);
   AddChild( &PageSpell1);
   AddChild( &PageSpell2);
   AddChild( &PageSpell3);
   AddChild( &PageSpell4);
   AddChild( &PageSpell5);
   AddChild( &PageSpell6);
   AddChild( &PageSpell7);
   AddChild( &PageSpell8);
   AddChild( &PageSpell9);

   AddChild( &m_SignetAir    );
   AddChild( &m_SignetFire   );
   AddChild( &m_SignetEarth  );
   AddChild( &m_SignetWater  );
   AddChild( &m_SignetLight  );    
   AddChild( &m_SignetDark   );
   AddChild( &m_SignetNormal );

   AddChild( &m_btnAir    );
   AddChild( &m_btnFire   );
   AddChild( &m_btnEarth  );
   AddChild( &m_btnWater  );
   AddChild( &m_btnLight  );    
   AddChild( &m_btnDark   );
   AddChild( &m_btnNormal );

   
   AddChild( &m_btnM0);
   AddChild( &m_btnM1);
   AddChild( &m_btnM2);
   AddChild( &m_btnM3);
   AddChild( &m_btnM4);
   AddChild( &m_btnM5);
   AddChild( &m_btnM6);
   AddChild( &m_btnM7);
   AddChild( &m_btnM8);
   AddChild( &m_btnM9);

   AddChild( &m_btnLArrow);
   AddChild( &m_btnRArrow);
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   InitializeMacro();

} 

V3_SpellDlg::~V3_SpellDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_SpellDlg *V3_SpellDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_SpellDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_SpellDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallSpell > 500)
      {
         g_dwLastMacroCallSpell =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}

unsigned int V3_SpellDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
 
   g_Global.SetLastDialogOpen(2);

   GetInstance()->RequestSpellList();
   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);
   GetInstance()->Show();
   return 0;
}

void V3_SpellDlg::RequestSpellList()
{
   TFCPacket sending;
   sending << (RQ_SIZE)RQ_SendSpellList;
   sending << (char)1;
	SEND_PACKET( sending );
}


void V3_SpellDlg::UpdateAllFramePos(int iPosX,int iPosY)
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

   m_btnLArrow.SetPos( FramePos( iPosX+44 , iPosY+372, iPosX+71 , iPosY+389 ) );
   m_btnRArrow.SetPos( FramePos( iPosX+534, iPosY+372, iPosX+561, iPosY+389 ) );


   PageSpell0.SetPos( FramePos( iPosX+46, iPosY+135, iPosX+46+222, iPosY+135+40 ) );
   PageSpell1.SetPos( FramePos( iPosX+46, iPosY+182, iPosX+46+222, iPosY+182+40 ) );
   PageSpell2.SetPos( FramePos( iPosX+46, iPosY+229, iPosX+46+222, iPosY+229+40 ) );
   PageSpell3.SetPos( FramePos( iPosX+46, iPosY+276, iPosX+46+222, iPosY+276+40 ) );
   PageSpell4.SetPos( FramePos( iPosX+46, iPosY+323, iPosX+46+222, iPosY+323+40 ) );

   PageSpell5.SetPos( FramePos( iPosX+314, iPosY+135, iPosX+314+222, iPosY+135+40 ) );
   PageSpell6.SetPos( FramePos( iPosX+314, iPosY+182, iPosX+314+222, iPosY+182+40 ) );
   PageSpell7.SetPos( FramePos( iPosX+314, iPosY+229, iPosX+314+222, iPosY+229+40 ) );
   PageSpell8.SetPos( FramePos( iPosX+314, iPosY+276, iPosX+314+222, iPosY+276+40 ) );
   PageSpell9.SetPos( FramePos( iPosX+314, iPosY+323, iPosX+314+222, iPosY+323+40 ) );


   m_btnM0.SetPos( FramePos( iPosX+274, iPosY+146, iPosX+274+20, iPosY+146+20 ) );
   m_btnM1.SetPos( FramePos( iPosX+274, iPosY+193, iPosX+274+20, iPosY+193+20 ) );
   m_btnM2.SetPos( FramePos( iPosX+274, iPosY+240, iPosX+274+20, iPosY+240+20 ) );
   m_btnM3.SetPos( FramePos( iPosX+274, iPosY+287, iPosX+274+20, iPosY+287+20 ) );
   m_btnM4.SetPos( FramePos( iPosX+274, iPosY+334, iPosX+274+20, iPosY+334+20 ) );

   m_btnM5.SetPos( FramePos( iPosX+542, iPosY+146, iPosX+542+20, iPosY+146+20 ) );
   m_btnM6.SetPos( FramePos( iPosX+542, iPosY+193, iPosX+542+20, iPosY+193+20 ) );
   m_btnM7.SetPos( FramePos( iPosX+542, iPosY+240, iPosX+542+20, iPosY+240+20 ) );
   m_btnM8.SetPos( FramePos( iPosX+542, iPosY+287, iPosX+542+20, iPosY+287+20 ) );
   m_btnM9.SetPos( FramePos( iPosX+542, iPosY+334, iPosX+542+20, iPosY+334+20 ) );



   Spell::Element firstElement = Spell::fire;
   if( PageSpell0.spell.Id != 0 )
   {
      firstElement = PageSpell0.spell.element;        
   }
   m_btnFire      .SetPos( FramePos( iPosX+250 , iPosY+85, iPosX+250+32 , iPosY+85+40 ) );
   m_SignetFire   .SetPos( FramePos( iPosX+250 , iPosY+85, iPosX+250+38 , iPosY+85+49 ) );
   m_btnNormal    .SetPos( FramePos( iPosX+527 , iPosY+85, iPosX+527+32 , iPosY+85+40 ) );
   m_SignetNormal .SetPos( FramePos( iPosX+527 , iPosY+85, iPosX+527+38 , iPosY+85+49 ) );
  
   // If water is before the first element.
   if( Spell::water < firstElement )
   {
      m_btnWater   .SetPos( FramePos( iPosX+210 , iPosY+85, iPosX+210+32 , iPosY+85+40 ) );		
      m_SignetWater.SetPos( FramePos( iPosX+210 , iPosY+85, iPosX+210+38 , iPosY+85+49 ) );		
   }
   else
   {
      m_btnWater   .SetPos( FramePos( iPosX+327 , iPosY+85, iPosX+327+32 , iPosY+85+40 ) );
      m_SignetWater.SetPos( FramePos( iPosX+327 , iPosY+85, iPosX+327+38 , iPosY+85+49 ) );
   }
   
   // If air is before the first element.
   if( Spell::air < firstElement )
   {
      m_btnAir   .SetPos( FramePos( iPosX+170 , iPosY+85, iPosX+170+32 , iPosY+85+40 ) );
      m_SignetAir.SetPos( FramePos( iPosX+170 , iPosY+85, iPosX+170+38 , iPosY+85+49 ) );
   }
   else
   {
      m_btnAir   .SetPos( FramePos( iPosX+367 , iPosY+85, iPosX+367+32 , iPosY+85+40 ) );
      m_SignetAir.SetPos( FramePos( iPosX+367 , iPosY+85, iPosX+367+38 , iPosY+85+49 ) );
   }
   
   // If earth is before the first element.
   if( Spell::earth < firstElement )
   {
      m_btnEarth   .SetPos( FramePos( iPosX+130 , iPosY+85, iPosX+130+32 , iPosY+85+40 ) );
      m_SignetEarth.SetPos( FramePos( iPosX+130 , iPosY+85, iPosX+130+38 , iPosY+85+49 ) );
   }
   else
   {
      m_btnEarth   .SetPos( FramePos( iPosX+407 , iPosY+85, iPosX+407+32 , iPosY+85+40 ) );
      m_SignetEarth.SetPos( FramePos( iPosX+407 , iPosY+85, iPosX+407+38 , iPosY+85+49 ) );
   }
   
   // If light is before the first element.
   if( Spell::light < firstElement )
   {
      m_btnLight   .SetPos( FramePos( iPosX+90 , iPosY+85, iPosX+90+32 , iPosY+85+40 ) );
      m_SignetLight.SetPos( FramePos( iPosX+90 , iPosY+85, iPosX+90+38 , iPosY+85+49 ) );
   }
   else
   {
      m_btnLight   .SetPos( FramePos( iPosX+447, iPosY+85, iPosX+447+32, iPosY+85+40 ) );
      m_SignetLight.SetPos( FramePos( iPosX+447, iPosY+85, iPosX+447+38, iPosY+85+49 ) );
   }
   
   // If dark is before the first element.
   if( Spell::dark < firstElement )
   {
      m_btnDark   .SetPos( FramePos( iPosX+50 , iPosY+85, iPosX+50+32 , iPosY+85+40 ) );
      m_SignetDark.SetPos( FramePos( iPosX+50 , iPosY+85, iPosX+50+38 , iPosY+85+49 ) );
   }
   else
   {
      m_btnDark   .SetPos( FramePos( iPosX+487, iPosY+85, iPosX+487+32, iPosY+85+40 ) );
      m_SignetDark.SetPos( FramePos( iPosX+487, iPosY+85, iPosX+487+38, iPosY+85+49 ) );
   }
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_SpellDlg::Draw(V2SPRITEFX *vsfFX)
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

   //left arrow...
   if(m_iCurrentPage==0)
      m_btnLArrow.Hide();
   else
      m_btnLArrow.Show();

   //right arrow...
   if((UINT)m_iCurrentPage+10 >= spellList.size())
      m_btnRArrow.Hide();
   else
      m_btnRArrow.Show();
   


   //backGame.Draw(vsfFX);
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
   
   // Draw les texte...

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[28],strlen(g_GUILocalString[28]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}

void V3_SpellDlg::Hide()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   PageSpell0.Select( false );
   PageSpell1.Select( false );
   PageSpell2.Select( false );
   PageSpell3.Select( false );
   PageSpell4.Select( false );
   PageSpell5.Select( false );
   PageSpell6.Select( false );
   PageSpell7.Select( false );
   PageSpell8.Select( false );
   PageSpell9.Select( false );
   UpdateIndexes();
   
	BoxUI::Hide();
}


void V3_SpellDlg::StartDragDlg( MousePos mousePos )
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

void V3_SpellDlg::StopDragDlg()
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





//  Updates the current spell list.
void V3_SpellDlg::UpdateSpells(const list< Spell > &newSpellList)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   spellList = newSpellList;
   spellList.sort();
   UpdateSpellPages();
}
 
//  Updates the spell pages according to the current page.
void V3_SpellDlg::UpdateSpellPages( void )
{
   int kp=0;
   // Fetch the first spell's iterator.
   list< Spell >::iterator i = spellList.begin();
   int c;
   for( c = 0; c < m_iCurrentPage; c++ )
   {
      i++;
      // If we reached the end of the spell list before the offset.
      if( i == spellList.end() )
      {
         PageSpell0.ClearPage();
         PageSpell1.ClearPage();
         PageSpell2.ClearPage();
         PageSpell3.ClearPage();
         PageSpell4.ClearPage();

         PageSpell5.ClearPage();
         PageSpell6.ClearPage();
         PageSpell7.ClearPage();
         PageSpell8.ClearPage();
         PageSpell9.ClearPage();
         UpdateIndexes();
         return;
      }
   }
   
   if( i == spellList.end() )
   {
      PageSpell0.ClearPage();
      PageSpell1.ClearPage();
      PageSpell2.ClearPage();
      PageSpell3.ClearPage();
      PageSpell4.ClearPage();
      PageSpell5.ClearPage();
      PageSpell6.ClearPage();
      PageSpell7.ClearPage();
      PageSpell8.ClearPage();
      PageSpell9.ClearPage();
      UpdateIndexes();
      return;
   }

   PageSpell0.ClearPage();
   PageSpell1.ClearPage();
   PageSpell2.ClearPage();
   PageSpell3.ClearPage();
   PageSpell4.ClearPage();
   PageSpell5.ClearPage();
   PageSpell6.ClearPage();
   PageSpell7.ClearPage();
   PageSpell8.ClearPage();
   PageSpell9.ClearPage();
   
   for(kp=0;kp<10;kp++)
   {
      // Update the first spell.
      switch(kp)
      {
         case 0: PageSpell0.FillSpellPage( *i ); break;
         case 1: PageSpell1.FillSpellPage( *i ); break;
         case 2: PageSpell2.FillSpellPage( *i ); break;
         case 3: PageSpell3.FillSpellPage( *i ); break;
         case 4: PageSpell4.FillSpellPage( *i ); break;
         case 5: PageSpell5.FillSpellPage( *i ); break;
         case 6: PageSpell6.FillSpellPage( *i ); break;
         case 7: PageSpell7.FillSpellPage( *i ); break;
         case 8: PageSpell8.FillSpellPage( *i ); break;
         case 9: PageSpell9.FillSpellPage( *i ); break;
      }
      
      i++;
      if( i == spellList.end() )
      {
         /*
         for(int kpt=kp+1;kpt<10;kpt++)
            PageSpell[kpt].ClearPage();
         */
         UpdateIndexes();
         return; 
      };
   }
   UpdateIndexes();
}

//  Updates the indexes according to the currently displayed spells.
void V3_SpellDlg::UpdateIndexes( void )
{
   m_btnAir   .Hide();
   m_btnFire  .Hide();
   m_btnEarth .Hide();
   m_btnWater .Hide();
   m_btnLight .Hide();
   m_btnDark  .Hide();
   m_btnNormal.Hide(); 

   m_SignetAir   .Hide();
   m_SignetFire  .Hide();
   m_SignetEarth .Hide();
   m_SignetWater .Hide();
   m_SignetLight .Hide();
   m_SignetDark  .Hide();
   m_SignetNormal.Hide(); 
   
   UpdateAllFramePos(m_iXPos,m_iYPos);
   
   
   m_btnAir   .Show(); 
   m_btnFire  .Show();
   m_btnEarth .Show();
   m_btnWater .Show(); 
   m_btnLight .Show(); 
   m_btnDark  .Show(); 
   m_btnNormal.Show();
   
   m_SignetAir   .Show();
   m_SignetFire  .Show();
   m_SignetEarth .Show();
   m_SignetWater .Show();
   m_SignetLight .Show();
   m_SignetDark  .Show();
   m_SignetNormal.Show(); 
   
   // Remove all indexes which are currently displayed.
   if( PageSpell0.spell.Id != 0 )
   {
      ClearIndex( PageSpell0.spell.element );
   }
}

//  Clears the index associated with an element.
void V3_SpellDlg::ClearIndex(const Spell::Element e)
 {
   switch( e )
   {
      case Spell::air:    m_btnAir   .Hide();  m_SignetAir   .Hide();  break;
      case Spell::water:  m_btnWater .Hide();  m_SignetWater .Hide();  break;
      case Spell::fire:   m_btnFire  .Hide();  m_SignetFire  .Hide();  break;
      case Spell::earth:  m_btnEarth .Hide();  m_SignetEarth .Hide();  break;
      case Spell::light:  m_btnLight .Hide();  m_SignetLight .Hide();  break;
      case Spell::dark:   m_btnDark  .Hide();  m_SignetDark  .Hide();  break;
      case Spell::normal: m_btnNormal.Hide();  m_SignetNormal.Hide();  break;
   }
}












///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_SpellDlg::DragEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_SpellDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}





void V3_SpellDlg::TabStatsEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );

   V3_StatsDlg::GetInstance()->Display(NULL);
   me->Hide();
   
}

void V3_SpellDlg::TabInvEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );

   V3_InvDlg::GetInstance()->Display(NULL);
   me->Hide();
}

/*
// void V3_SpellDlg::TabSpellEvent::LeftClicked( void )
// {
//    V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
//    
// }
*/

void V3_SpellDlg::TabProfessionEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
 
   V3_ProfessionDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_SpellDlg::TabGuildeEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );

   V3_GuildDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_SpellDlg::TabGroupEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   V3_GroupeDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_SpellDlg::TabMacroEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   V3_MacroDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_SpellDlg::TabOptionEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );

   V3_OptionsDlg::GetInstance()->Display(NULL);
   me->Hide();
}




//////////////////////////////////////////////////////////////////////////////////////////
void V3_SpellDlg::SpellClickEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   me->PageSpell0.Select( false );
   me->PageSpell1.Select( false );
   me->PageSpell2.Select( false );
   me->PageSpell3.Select( false );
   me->PageSpell4.Select( false );
   me->PageSpell5.Select( false );
   me->PageSpell6.Select( false );
   me->PageSpell7.Select( false );
   me->PageSpell8.Select( false );
   me->PageSpell9.Select( false );
   
   
   // Get the spell according to which event got triggered.
   switch( spellNum )
   {
      case 0 : 
         if(me->PageSpell0.spell.Id != 0)
            me->PageSpell0.Select( true ); 
      break;
      case 1 : 
         if(me->PageSpell1.spell.Id != 0)
            me->PageSpell1.Select( true ); 
      break;
      case 2 : 
         if(me->PageSpell2.spell.Id != 0)
            me->PageSpell2.Select( true ); 
      break;
      case 3 : 
         if(me->PageSpell3.spell.Id != 0)
            me->PageSpell3.Select( true ); 
      break;
      case 4 : 
         if(me->PageSpell4.spell.Id != 0)
            me->PageSpell4.Select( true ); 
      break;
      case 5 :
         if(me->PageSpell5.spell.Id != 0)
            me->PageSpell5.Select( true ); 
      break;
      case 6 : 
         if(me->PageSpell6.spell.Id != 0)
            me->PageSpell6.Select( true ); 
      break;
      case 7 : 
         if(me->PageSpell7.spell.Id != 0)
            me->PageSpell7.Select( true ); 
      break;
      case 8 : 
         if(me->PageSpell8.spell.Id != 0)
            me->PageSpell8.Select( true ); 
      break;
      case 9 : 
         if(me->PageSpell9.spell.Id != 0)
            me->PageSpell9.Select( true ); 
      break;
      default: return;
   }
}

//  If one of the spells got double-clicked.
void V3_SpellDlg::SpellClickEvent::LeftDblClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   
   
   // Get the spell according to which event got triggered.
   switch( spellNum )
   {
      case 0 : spell = me->PageSpell0.spell; break;
      case 1 : spell = me->PageSpell1.spell; break;
      case 2 : spell = me->PageSpell2.spell; break;
      case 3 : spell = me->PageSpell3.spell; break;
      case 4 : spell = me->PageSpell4.spell; break;
      case 5 : spell = me->PageSpell5.spell; break;
      case 6 : spell = me->PageSpell6.spell; break;
      case 7 : spell = me->PageSpell7.spell; break;
      case 8 : spell = me->PageSpell8.spell; break;
      case 9 : spell = me->PageSpell9.spell; break;
      
      default: return;
   }
   
   if( spell.Id == 0 )
   {
      noMorePageSound->Play( TRUE );
      return;
   }
   
   me->CastSpell( spell, false );
}



//////////////////////////////////////////////////////////////////////////////////////////
 V3_SpellDlg::SpellPageUI::SpellPageUI(EventVisitor &event)
: spellEvent( event ),
m_bSelectState(false)
{
    m_stName.SetCenter( false );
    for(int i=0;i<2;i++)
    {
       m_stLevel[i].SetCenter( false );
       m_stDuration[i].SetCenter( false );
       m_stType[i].SetCenter( false );
       m_strManaCost[i].SetCenter( false );
    }

    m_stLevel[0]    .SetText( g_GUILocalString[ 330 ], g_DefColorH ,g_Global.GetScreenW(),0,false,2);
    m_stDuration[0] .SetText( g_GUILocalString[ 328 ], g_DefColorH ,g_Global.GetScreenW(),0,false,2);
    m_stType[0]     .SetText( g_GUILocalString[ 327 ], g_DefColorH ,g_Global.GetScreenW(),0,false,2);
    m_strManaCost[0].SetText( g_GUILocalString[ 329 ], g_DefColorH ,g_Global.GetScreenW(),0,false,2);
}

//  Update all relative control positions.
void V3_SpellDlg::SpellPageUI::SetPos(FramePos framePos)
{
    GameUI::SetPos( framePos );

    int x = framePos.x;
    int y = framePos.y;

 
    m_stName        .SetPos(FramePos(  x+40,  y+0, x+220,  y+12 ) );

    m_stLevel[0]    .SetPos(FramePos(  x+160, y+26,x+190, y+38 ) );
    m_stDuration[0] .SetPos(FramePos(  x+40,  y+26,x+72,  y+38 ) );
    m_stType[0]     .SetPos(FramePos(  x+40,  y+14,x+72,  y+26 ) );
    m_strManaCost[0].SetPos(FramePos(  x+160, y+14,x+190, y+26 ) );

    m_stLevel[1]    .SetPos(FramePos(  x+195, y+26,x+220, y+38 ) );
    m_stDuration[1] .SetPos(FramePos(  x+80,  y+26,x+150, y+38 ) );
    m_stType[1]     .SetPos(FramePos(  x+80,  y+14,x+150, y+26 ) );
    m_strManaCost[1].SetPos(FramePos(  x+195, y+14,x+220, y+26 ) );

    icon.SetPos(FramePos(  x+4, y+4,x+4+32, y+4+32 ) );
}

//  Sets the new selection state of the page.
void V3_SpellDlg::SpellPageUI::Select(bool selectState)
{
    if( selectState )
    {
        selectSound->Play( TRUE );
        m_bSelectState = true;

        m_stLevel[0]    .SetText( g_GUILocalString[ 330 ], g_DefColorDS ,g_Global.GetScreenW(),0,false,2);
        m_stDuration[0] .SetText( g_GUILocalString[ 328 ], g_DefColorDS ,g_Global.GetScreenW(),0,false,2);
        m_stType[0]     .SetText( g_GUILocalString[ 327 ], g_DefColorDS ,g_Global.GetScreenW(),0,false,2);
        m_strManaCost[0].SetText( g_GUILocalString[ 329 ], g_DefColorDS ,g_Global.GetScreenW(),0,false,2);
    }
    else
    {
        m_bSelectState = false;

        m_stLevel[0]    .SetText( g_GUILocalString[ 330 ], g_DefColorH ,g_Global.GetScreenW(),0,false,2);
        m_stDuration[0] .SetText( g_GUILocalString[ 328 ], g_DefColorH ,g_Global.GetScreenW(),0,false,2);
        m_stType[0]     .SetText( g_GUILocalString[ 327 ], g_DefColorH ,g_Global.GetScreenW(),0,false,2);
        m_strManaCost[0].SetText( g_GUILocalString[ 329 ], g_DefColorH ,g_Global.GetScreenW(),0,false,2);
    }
    if(spell.Id != 0)
      FillSpellPage(spell); 
}

//  Clears the spell page.
void V3_SpellDlg::SpellPageUI::ClearPage( void )
{
    spell.Id = 0;
    m_stName        .SetText( "", g_DefColorD);
    m_stLevel[1]    .SetText( "", g_DefColorD ,g_Global.GetScreenW(),0,false,2 );
    m_stDuration[1] .SetText( "", g_DefColorD ,g_Global.GetScreenW(),0,false,2 );
    m_stType[1]     .SetText( "", g_DefColorD ,g_Global.GetScreenW(),0,false,2 );
    m_strManaCost[1].SetText( "", g_DefColorD ,g_Global.GetScreenW(),0,false,2 );
    icon.SetVSF( NULL );
}


// Fills a spell list
void V3_SpellDlg::SpellPageUI::FillSpellPage(const Spell &ispell)
{
    char buf[ 256 ];

    spell = ispell;

    if(m_bSelectState)
       m_stName.SetText( spell.name.c_str(), g_DefColorDS);
    else
       m_stName.SetText( spell.name.c_str(), g_DefColorD);

    

	_itoa_s( spell.level, buf,256, 10 );
    m_stLevel[1].SetText( buf, g_DefColorD ,g_Global.GetScreenW(),0,false,2);
    
            
    if( spell.duration > 1000 * 60 )
    {
        DWORD min = spell.duration / ( 60 * 1000 );
        sprintf_s( buf,256, g_LocalString[ 364 ], spell.duration / ( 60 * 1000 ) );
        
        // Get the seconds part
        int sec = spell.duration / 1000 - min * 60;

        if( sec > 0 )
        {
            sprintf_s( buf,256, g_LocalString[ 365 ], sec );
        }
    }
    else if( spell.duration > 1000 )
    {
        sprintf_s( buf,256, g_LocalString[ 365 ], spell.duration / 1000 );
    }
    else if( spell.duration == 0 )
    {
        strcpy_s( buf,256, g_LocalString[ 366 ] );
    }
    else
    {
        sprintf_s( buf,256, g_LocalString[ 367 ], spell.duration );
    }
    
    m_stDuration[1].SetText( buf, g_DefColorD ,g_Global.GetScreenW(),0,false,2);
    
    if( spell.type == Spell::physical )
    {
        m_stType[1].SetText( g_LocalString[ 514 ], g_DefColorD ,g_Global.GetScreenW(),0,false,2);
    }
    else
    {
        m_stType[1].SetText( g_LocalString[ 515 ], g_DefColorD ,g_Global.GetScreenW(),0,false,2);        
    }

	_itoa_s( spell.manaCost, buf,256, 10 );
    m_strManaCost[1].SetText( buf, g_DefColorD ,g_Global.GetScreenW(),0,false,2);

    
    CV2Sprite *lpVSF = SpellIcons( spell.icon );    
    icon.SetVSF( lpVSF );
    GetHelpText()->SetText( spell.desc.c_str() );
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_SpellDlg::SpellPageUI::Draw(V2SPRITEFX *vsfFX)
{
    // Draw the control's controls.

   m_stName        .Draw( vsfFX );
   for(int i=0;i<2;i++)
   {
      m_stLevel[i]    .Draw( vsfFX );
      m_stDuration[i] .Draw( vsfFX );
      m_stType[i]     .Draw( vsfFX );
      m_strManaCost[i].Draw( vsfFX );
   }
   icon  .Draw( vsfFX );
}

// Mouse click events on the spell control.
bool V3_SpellDlg::SpellPageUI::LeftMouseUp(MousePos mousePos)
{
    if( IsDblClick() )
    {
        spellEvent.LeftDblClicked();
    }
    else
    {
        spellEvent.LeftClicked();
    }
    return true;
}



//  Casts the given spell
void V3_SpellDlg::CastSpell(const Spell &spell,bool autoTargetSelf,bool noCallback)
{
    // Whether the spell can be cast on self or not.
    bool allowSelf = false;

    lastSpell = spell.Id;

    if( autoTargetSelf && spell.targetType != TARGET_SELF && spell.targetType != TARGET_GROUP_SELF &&spell.targetType != TARGET_GROUP_POSITION)
    {
        TFCPacket sending;

        sending << (RQ_SIZE)RQ_CastSpell;
        sending << (short)spell.Id;
        sending << (short)Player.xPos;
        sending << (short)Player.yPos;
        sending << (long)Player.ID;
		
	      SEND_PACKET( sending );
        return;        
    }


    switch( spell.targetType )
    {
       // Target Self
       case TARGET_GROUP_SELF:
       case TARGET_NONGROUP_SELF:
       case TARGET_SELF:
       {
          TFCPacket sending;
          
          sending << (RQ_SIZE)RQ_CastSpell;
          sending << (short)spell.Id;
          sending << (short)0;
          sending << (short)0;
          sending << (long)0;
          
          SEND_PACKET( sending );
       }break;

       // Target Position
       case TARGET_GROUP_POSITION:
       case TARGET_NONGROUP_POSITION:
       case TARGET_POSITION:
       {
          // Spell cast using a world position.			
          SpellCallbackData *mouseData = new SpellCallbackData( spell, false );
          
          if( noCallback )
          {
             CastSpellPosition( mouseData );
          }
          else
          {
             // Set a lock on the mouse
             if( !mouse.LockNextEvent( DM_CLICK, mouseData, CastSpellPosition, 0 ) )
             {
                // Could not lock mouse.
                V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 444 ], SYSTEM_COLOR, true);
                delete mouseData;
             }
             else
             {
                CMouseCursor::GetInstance()->SetCustomCursor( &V3CastCursor, 0, 0, true );
             }
          }
       }break;

       // Favor NPC targetting
       case TARGET_NONGROUP_UNIT:
       case TARGET_UNIT_OBJECT:
       case TARGET_UNIT_NPC: 
       case TARGET_UNIT_LIVING_MONSTER: 
         allowSelf = true;
       case TARGET_UNIT_LIVING_MONSTER_NONSELF: 
       {
          SpellCallbackData *mouseData = new SpellCallbackData( spell,allowSelf );
       
          if( noCallback )
          {
             CastSpellUnit( mouseData );
          }
          else
          {
             // Set a lock on the mouse        
             if( !mouse.LockNextEvent( DM_CLICK, mouseData, CastSpellUnit, 0 ) )
             {
                // Could not lock mouse.
                V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 444 ], SYSTEM_COLOR, true);
                delete mouseData;
             }
             else
             {
                CMouseCursor::GetInstance()->SetCustomCursor( &V3CastCursor, 0, 0, true );
             }
          }
       }break;

       ////////////////////////////////
       // Favor PC targetting
       case TARGET_GROUP_UNIT:
       case TARGET_UNIT_LIVING_PLAYER:
       case TARGET_UNIT_ANY_PLAYER:
       case TARGET_UNIT_PC: 
          allowSelf = true;
       case TARGET_UNIT_PC_NONSEL: 
       {
          Objects.Lock(98);
          Objects.SetGridMode( playerPriority );
          Objects.Unlock(98);
          SpellCallbackData *mouseData = new SpellCallbackData( spell, allowSelf );
          if( noCallback )
          {
             CastSpellUnit( mouseData );
          }
          else
          {
             // Set a lock on the mouse
             if( !mouse.LockNextEvent( DM_CLICK, mouseData, CastSpellUnit, 0 ) )
             {
                // Could not lock mouse.
                V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 444 ], SYSTEM_COLOR, true);
                delete mouseData;
             }
             else
             {
                CMouseCursor::GetInstance()->SetCustomCursor( &V3CastCursor, 0, 0, true );
             }
          }
       } break;

       ////////////////////////////////
       // Favor any unit.
       case TARGET_UNIT_ANY:
       case TARGET_UNIT_LIVING:
          allowSelf = true;
       case TARGET_UNIT_LIVING_NONSELF:
       case TARGET_UNIT_ANY_NONSELF: 
       {
          Objects.Lock(99);
          Objects.SetGridMode( equalPriority );
          Objects.Unlock(99);
          
          SpellCallbackData *mouseData = new SpellCallbackData( spell, allowSelf );
          
          if( noCallback )
          {
             CastSpellUnit( mouseData );
          }
          else
          {
             // Set a lock on the mouse
             if( !mouse.LockNextEvent( DM_CLICK, mouseData, CastSpellUnit, 0 ) )
             {
                // Could not lock mouse.
                V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 444 ], SYSTEM_COLOR, true);
                delete mouseData;
             }
             else
             {
                CMouseCursor::GetInstance()->SetCustomCursor( &V3CastCursor, 0, 0, true );
             }
          }
       }break;
    }

    if( IsShown() )
    {
        Hide();
    }
}




// Callback fonction called after choosing a world position for using a skill
UINT V3_SpellDlg::CastSpellPosition(LPVOID rawData)
{	
   SpellCallbackData *mouseData = reinterpret_cast< SpellCallbackData * >( rawData );
   
   CMouseCursor::GetInstance()->SetCustomCursor( NULL );
   
   // Send the use skill with a world position but without a unit ID.
   TFCPacket sending;
   POINT mousePos;
      
   // Get the mouse pos.
   MouseDevice md;
   md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
   
   // Then convert the mouse coordinates into world coordinates
   Player.ScreenPosToWL( &mousePos, &mousePos );   
   
   sending << (RQ_SIZE)RQ_CastSpell;
   sending << (short)mouseData->spell.Id;	
   
   bool bSetID = false;

   signed char xPos, yPos;
   Objects.Lock(100);
   if (TargetID == Player.ID) 
   {
      sending << (short)( Player.xPos );
      sending << (short)( Player.yPos );
      sending << (long)TargetID;
   } 
   else if (Objects.FoundValidID( &xPos, &yPos, TargetID )) 
   {
      sending << (short)( xPos+Player.xPos );
      sending << (short)( yPos+Player.yPos );
      sending << (long)TargetID;
      SelectedID = TargetID;
      bSetID = true;
   } 
   else 
   {
      sending << (short)( mousePos.x );
      sending << (short)( mousePos.y );
      sending << (long)NULL;
   }
   Objects.Unlock(100);
   SEND_PACKET( sending );
   delete mouseData;


   return 0;
}

// Callback function called after choosing a unit.
UINT V3_SpellDlg::CastSpellUnit(LPVOID rawData)
{
   
   CMouseCursor::GetInstance()->SetCustomCursor( NULL );
   POINT mousePos;
   SpellCallbackData *mouseData = reinterpret_cast< SpellCallbackData * >( rawData );
   
   TFCPacket sending;
   sending << (RQ_SIZE)RQ_CastSpell;
   sending << (short)mouseData->spell.Id;
   
   // Get the mouse pos.
   MouseDevice md;
   md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
   
   // Get the world pos.
   Player.ScreenPosToWL( &mousePos, &mousePos );
   
   bool bSetID = false;
   signed char xPos, yPos;
   Objects.Lock(100);
   if (TargetID == Player.ID) 
   {
      sending << (short)( Player.xPos );
      sending << (short)( Player.yPos );
      sending << (long)TargetID;
      SEND_PACKET( sending );
   }
   else if (Objects.FoundValidID( &xPos, &yPos, TargetID )) 
   {
      sending << (short)( xPos+Player.xPos );
      sending << (short)( yPos+Player.yPos );
      sending << (long)TargetID;
      SelectedID = TargetID;
      bSetID = true;

      SEND_PACKET( sending );
   }
   else 
   {
      //      sending << (short)( mousePos.x );
      //      sending << (short)( mousePos.y );
      //      sending << (long)NULL;
   }
   Objects.Unlock(100);
   delete mouseData;


   CMouseCursor::GetInstance()->SetCustomCursor( NULL );
   
   return 0;
}




//  Casts a spell using its spell Id.
bool V3_SpellDlg::CastSpell(DWORD spellId,bool autoTargetSelf)
{
   V3_SpellDlg *me = V3_SpellDlg::GetInstance();
   
   list< Spell >::iterator i = me->spellList.begin();
   // Search the spell in the spellbook.
   for( i = me->spellList.begin(); i != me->spellList.end(); i++ )
   {    
      if( (*i).Id == spellId )
      {
         // Cast the found spell.
         me->CastSpell( *i, autoTargetSelf );
         return true;
      }
   }
   return false;
}


//  Casts the last spell
void V3_SpellDlg::CastLastSpell( void )
{
   list< Spell >::iterator i = spellList.begin();
   // Search the spell in the spellbook.
   for( i = spellList.begin(); i != spellList.end(); i++ )
   {    
      if( (*i).Id == lastSpell )
      {
         // Cast the found spell.
         CastSpell( *i, false, true );
         return;
      }
   }
}


//  Left clicked on the previous button.
void V3_SpellDlg::LArrowEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   me->m_iCurrentPage-= 10;
   if( me->m_iCurrentPage < 0 )
   {
      me->m_iCurrentPage = 0;
      noMorePageSound->Play( TRUE );
   }
   else
   {
      turnPageSound->Play( TRUE );
   }
   me->UpdateSpellPages();
}

//  Left clicked on the next button.
void V3_SpellDlg::RArrowEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   me->m_iCurrentPage += 10;
   if( me->m_iCurrentPage >= (int)me->spellList.size() )
   {
      me->m_iCurrentPage -= 10;
      noMorePageSound->Play( TRUE );
   }
   else
   {
      me->UpdateSpellPages();
      turnPageSound->Play( TRUE );
   }
}


//  Left clicked an elemental search.
void V3_SpellDlg::ElementEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   list< Spell >::iterator i = me->spellList.begin();
   
   int oldCurrentPage = me->m_iCurrentPage;
   
   // Find the first spell corresponding to the event's element.
   me->m_iCurrentPage = 0;
   while( i != me->spellList.end() )
   {
      if( (*i).element == element )
      {
         me->UpdateSpellPages();
         return;
      }
      me->m_iCurrentPage++;
      i++;
   } 
   
   // Restore page state if the element wasn't found.
   me->m_iCurrentPage = oldCurrentPage;
}


//  Left clicked on the macro button.
void V3_SpellDlg::M0ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell0.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

void V3_SpellDlg::M1ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell1.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

void V3_SpellDlg::M2ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell2.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

void V3_SpellDlg::M3ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell3.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

void V3_SpellDlg::M4ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell4.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

void V3_SpellDlg::M5ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell5.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

void V3_SpellDlg::M6ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell6.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

void V3_SpellDlg::M7ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell7.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

void V3_SpellDlg::M8ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell8.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

void V3_SpellDlg::M9ButtonEvent::LeftClicked( void )
{
   V3_SpellDlg *me = static_cast< V3_SpellDlg * >( GetUI() );
   
   Spell spell;
   spell = me->PageSpell9.spell;
   if( spell.Id == 0 ) // Check which spell is selected
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 375 ], SYSTEM_COLOR, false );
      return;
   }
   
   SpellMacroUI *macroUI = new SpellMacroUI;
   
   macroUI->iconId = (unsigned short)spell.icon;
   macroUI->spell = spell;
   macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, spell.name.c_str(), &me->spellMacroEvent );
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_SpellDlg::SpellMacroEvent::ExecMacro(MacroDataUI *imacroUI)
{
   SpellMacroUI *spellMacro = static_cast< SpellMacroUI * >( imacroUI );
   
   V3_SpellDlg *me = V3_SpellDlg::GetInstance();
   
   list< Spell >::iterator i = me->spellList.begin();
   // Search the spell in the spellbook.
   for( i = me->spellList.begin(); i != me->spellList.end(); i++ )
   {    
      if( (*i).Id == spellMacro->spell.Id )
      {
         // Cast the found spell.
         me->CastSpell( *i, spellMacro->autoTargetSelf );
         return;
      }
   }
   
   V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 376 ], SYSTEM_COLOR, true );
}

//////////////////////////////////////////////////////////////////////////////////////////
// Saves the given macro.
// 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_SpellDlg::SpellMacroEvent::SaveMacro(const list< MacroDataUI * > &macroList)
{
   list< MacroDataUI * >::const_iterator i;
   for( i = macroList.begin(); i != macroList.end(); i++ )
   {
      const SpellMacroUI *macroUI = static_cast< const SpellMacroUI * >( *i );
      sMacro sNewMacro;
      
      sNewMacro.dwID       = macroUI->spell.Id;
      sNewMacro.dwIcon     = macroUI->iconId;
      memcpy(sNewMacro.chFastPos,macroUI->chGridPos,g_iNbrMacro);
      sNewMacro.dwKey      = macroUI->vKey.AsLong();
      sprintf_s(sNewMacro.strMacroName,256,"%s",macroUI->spell.name.c_str());
      g_SaveGame.AddMacroSpell(sNewMacro);
   }
}

void V3_SpellDlg::Reload()
{
   InitializeMacro();
}

void V3_SpellDlg::InitializeMacro( void )
{
   int dwNbrSpellMacro = g_SaveGame.GetNbrMSpell();
   if(dwNbrSpellMacro > 0)
   {
      for(int i =0;i<dwNbrSpellMacro; i++ )
      {
         SpellMacroUI *macroUI = new SpellMacroUI;
         
         sMacro *pNewMacro = g_SaveGame.GetMSpell(i);
         
         macroUI->spell.Id    = pNewMacro->dwID;
         macroUI->iconId      = pNewMacro->dwIcon;
         memcpy(macroUI->chGridPos,pNewMacro->chFastPos,g_iNbrMacro);
         macroUI->spell.name  = pNewMacro->strMacroName;
         macroUI->vKey.SetLong(pNewMacro->dwKey);
         
         macroUI->SetVSF( SpellIcons( macroUI->iconId ) );
         macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
         
         if( macroUI->spell.Id == 0 || macroUI->iconId == 0 )
            delete macroUI;
         else // Add the macro
            V3_MacroDlg::GetInstance()->AddMacro(macroUI, macroUI->spell.name, &spellMacroEvent, false);
      }
   }
}