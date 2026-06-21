// V3_InvDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include <set>

#include "V3_StatDlg.h"
#include "V3_SpellDlg.h"
#include "V3_GuildDlg.h"
#include "V3_GroupeDlg.h"
#include "V3_MacroDlg.h"
#include "V3_InvDlg.h"

// Guard anti-deadlock (cf. V3_StatsDlg) : construction concurrente thread reseau / rendu.
static V3_InvDlg* g_pInvDlgInstance = NULL;
#include "V3_OptionsDlg.h"
#include "V3_ProfessionDlg.h"
#include "V3_SpellDlg.h"

#include "V3_ChatLogDlg.h"
#include "V3_MainBarDlg.h"

#include "V3_AskValueSldDlg.h"
#include "V3_DisplaySpecialDlg.h"

#include "V3_ChestDlg.h"
#include "V3_ChestNewDlg.h"
#include "V3_GuildChestDlg.h"
#include "V3_GuildChestNewDlg.h"
#include "V3_AHVendreDlg.h"
#include "V3_TradeDlg.h"

extern DWORD TargetID;
extern DWORD SelectedID;
extern CSaveGame g_SaveGame;

#define V2IMAGEUSENAME_HEADER   "\\Game Files\\UseItemBDI.dat"
#define V2IMAGEUSENAME_DATA     "\\Game Files\\UseItemBDD.dat"


const int g_AskDrop = 0;
const int g_AskJunk = 1;

DWORD g_dwLastMacroCallInv = 0;

namespace{
    T3VSBSound *dropSound = NULL;
    T3VSBSound *naSound = NULL;

    void InitSound()
    {
        if( dropSound == NULL ){
            dropSound = new T3VSBSound;
            dropSound->Create( "Generic drop item", TS_MEMORY );

            naSound = new T3VSBSound;
            naSound->Create( "General NA sound", TS_MEMORY );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    //  Drops back the given item UI in its original ui.
    bool DropBack(GameUI *itemUI)
    {
       GameUI *src = GameUI::GetDragParentUI();
       if( src != NULL )
       {
          // If the source is a grid UI
          if( typeid( *src ) == typeid( GridUI ) )
          {
             // Try to see if the grid has space left.
             GridUI *srcGrid = static_cast< GridUI * >( src );
             // If the grid spot isn't taken
             if( srcGrid->CanDrop( GameUI::GetDragInitialPos() ) )
             {
                // Redrop the item
                src->LeftMouseDown( GameUI::GetDragInitialPos() );
             }else
             {
                // Otherwise add the item.
                srcGrid->AddItem( itemUI );
                GameUI::CancelDrag();
                CMouseCursor::GetInstance()->SetCustomCursor( NULL );
             }
             
          }
          else
          {
             // Drop the item where it was first fetched.
             src->LeftMouseDown( GameUI::GetDragInitialPos() );
          }
          return true;
       }
       return false;
    };
    
    //////////////////////////////////////////////////////////////////////////////////////////
    //  Drops back the given item UI in its original ui.
    // 
    //////////////////////////////////////////////////////////////////////////////////////////
    bool DropBack2(GameUI *itemUI ,bool bResetMouseCursor)
    {
       GameUI *src = GameUI::GetDragParentUI();
       if( src != NULL )
       {
          // If the source is a grid UI
          if( typeid( *src ) == typeid( GridUI ) )
          {
             
             // Try to see if the grid has space left.
             GridUI *srcGrid = static_cast< GridUI * >( src );
             if( srcGrid->CanDrop( GameUI::GetDragInitialPos() ) )
             {
                srcGrid->DropItem(GameUI::GetDragInitialPos(),bResetMouseCursor);//LastPos();
                GameUI::CancelDrag();
                if(bResetMouseCursor)
                  CMouseCursor::GetInstance()->SetCustomCursor( NULL );
             }
             else
             {
                srcGrid->AddItem( itemUI );
                GameUI::CancelDrag();
                if(bResetMouseCursor)
                  CMouseCursor::GetInstance()->SetCustomCursor( NULL );
             }
          }
          return true;
       }
       return false;
    };

	struct ItemCallbackData
	{
		ItemCallbackData(DWORD dwID,LPBAG_ITEM bagItem)
			: dwItemID(dwID),bagItemPtr(bagItem)
		{};
		DWORD dwItemID;
		LPBAG_ITEM bagItemPtr;
	};
}

//  Returns a string which displays boosted and normal stats (for help).
string GetStatBoostString(int boost,int stat)
{
   char buf[ 256 ];
   _itoa_s( boost, buf,256, 10 );
   string str( buf );
   str += " / ";
   _itoa_s( stat, buf,256, 10 );
   str += buf;
   return str;
}

//  Returns the color according to the given variables.
DWORD GetColor(int stat,int boostedStat,int upg)
{
    DWORD color;
    // If stat is boosted.
    if( boostedStat > stat )
    {
        // If the stat was upgraded. (stat train).
        if( upg != 0 )
            color = CL_GREEN_LIGHT; // Pale green
        else
            color = CL_GREEN; // Green
    }
    else if( boostedStat < stat )// If stat is under normal.
    {
        // If the stat was upgraded.
        if( upg != 0 )
            color = CL_RED_LIGHT; // Pink
        else
            color = CL_RED; // Red
    }
    else // If stat is normal
    {
        // If the stat was upgraded. (stat train).
        if( upg != 0 )
            color = CL_WHITE; // Bright white.
        else
            color = g_DefColorD; // Light grey.
    }
   
    return color;
}


void FormatChargesLeft( char *buf, LPBAG_ITEM bagItem )
{
   if( bagItem->iCharges != 0 )
   {
      if( bagItem->iCharges > 0 )
      {
         bagItem->iCharges--;
         sprintf_s( buf,512, g_LocalString[ 433 ], bagItem->chIName, bagItem->iCharges );
         
         static_cast< V3_InvDlg::ItemUI * >( bagItem->associatedUI )->UpdateName2();
      }
      else
      {
         sprintf_s( buf,512, g_LocalString[ 534 ], bagItem->chIName );
      }
   }
   else
   {
      sprintf_s( buf,512, g_LocalString[ 433 ], bagItem->chIName, bagItem->dwQty-1 );
   }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_InvDlg::V3_InvDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
MOnglet01Button(tabStatsEvent            ,g_GUILocalString[309],g_DefColorH ),
MOnglet02Button(EmptyEvent::GetInstance(),g_GUILocalString[310],g_DefColorH ),
MOnglet03Button(tabSpellEvent            ,g_GUILocalString[311],g_DefColorH ),
MOnglet04Button(tabProfessionEvent       ,g_GUILocalString[312],g_DefColorH ),
MOnglet05Button(tabGuildeEvent           ,g_GUILocalString[313],g_DefColorH ),
MOnglet06Button(tabGroupEvent            ,g_GUILocalString[314],g_DefColorH ),
MOnglet07Button(tabMacroEvent            ,g_GUILocalString[315],g_DefColorH ),
MOnglet08Button(tabOptionEvent           ,g_GUILocalString[316],g_DefColorH ),
m_bDragDialog (false),
iconSTR( EmptyEvent::GetInstance() ),
iconEND( EmptyEvent::GetInstance() ),
iconAGI( EmptyEvent::GetInstance() ),
iconWIS( EmptyEvent::GetInstance() ),
iconINT( EmptyEvent::GetInstance() ),
iconAC( EmptyEvent::GetInstance() ),
iconGold( EmptyEvent::GetInstance() ),
mainDropZone( equipEvent ),
headDropZone( equipEvent ),
bodyDropZone( equipEvent ),
leftDropZone( equipEvent ),
rightDropZone( equipEvent ),
legsDropZone( equipEvent ),
feetDropZone( equipEvent ),
capeDropZone( equipEvent ),
glovesDropZone( equipEvent ),
neckDropZone( equipEvent ),
wristDropZone( equipEvent ),
beltDropZone( equipEvent ),
ring1DropZone( equipEvent ),
ring2DropZone( equipEvent ),
Orbe1DropZone( equipEvent ),
inventoryGrid( inventoryGridEvent ),
macroButton( macroButtonEvent ),
TradeButton( tradeButtonEvent ),
junkButton( junkButtonEvent ),
dropButton( dropButtonEvent )
{
   g_pInvDlgInstance = this;

   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_InvBack",true,"V3_InvBackMask");
   m_backIconsS.SetSpriteId("V3_InvBackIcons");
   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_InvIcon");

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
   //tabInvEvent       .SetUI(this);
   tabSpellEvent     .SetUI(this);
   tabProfessionEvent.SetUI(this);
   tabGuildeEvent    .SetUI(this);
   tabGroupEvent     .SetUI(this);
   tabMacroEvent     .SetUI(this);
   tabOptionEvent    .SetUI(this);


   statSTR.SetHelpText ( FontManager::GetInstance()->GetSmallFont() );
   statEND.SetHelpText ( FontManager::GetInstance()->GetSmallFont() );
   statAGI.SetHelpText ( FontManager::GetInstance()->GetSmallFont() );
   statWIS.SetHelpText ( FontManager::GetInstance()->GetSmallFont() );
   statINT.SetHelpText ( FontManager::GetInstance()->GetSmallFont() );
   statAC .SetHelpText ( FontManager::GetInstance()->GetSmallFont() );
  
   iconSTR.EnableSound( false );
   iconEND.EnableSound( false );
   iconAGI.EnableSound( false );
   iconWIS.EnableSound( false );
   iconINT.EnableSound( false );
   iconAC .EnableSound( false );
   iconGold.EnableSound( false );
   
   SetHelpId( iconSTR, 262 );
   SetHelpId( iconEND, 263 );
   SetHelpId( iconAGI, 264 );
   SetHelpId( iconWIS, 266 );
   SetHelpId( iconINT, 265 );
   SetHelpId( iconAC , 448 );
   SetHelpId( iconGold,450 );

   SetHelpId( capeDropZone,   267 );
   SetHelpId( leftDropZone,   271 );
   SetHelpId( headDropZone,   269 );
   SetHelpId( bodyDropZone,   270 );
   SetHelpId( rightDropZone,  268 );
   SetHelpId( legsDropZone,   272 );
   SetHelpId( feetDropZone,   273 );
   SetHelpId( glovesDropZone, 274 );
   SetHelpId( mainDropZone,   275 );
   SetHelpId( neckDropZone,   276 );
   SetHelpId( wristDropZone,  277 );
   SetHelpId( beltDropZone,   278 );
   SetHelpId( ring1DropZone,  279 );
   SetHelpId( ring2DropZone,  279 );
   SetHelpId( Orbe1DropZone,  653 );
   equipEvent.SetUI( this );


   inventoryGrid.SetGrid( 8, 8, FramePos( 0, 0, 26, 26 ), FramePos( 0, 0, 3, 3 ) );
   inventoryGrid.AllowHelp(false);
   SetHelpId( inventoryGrid, 284 );
   inventoryGrid.SetScrollUpDownUI( &gridScrollUpGraph );
   inventoryGrid.SetScrollDownDownUI( &gridScrollDownGraph );
   gridSelectedTile.SetSpriteId( "GUI_BackInvGridSelect" );
   inventoryGrid.SetSelectedTileUI( &gridSelectedTile );


   inventoryGridEvent.SetUI( this );

   macroButton.SetSpriteId   ( "V3_MacroBtn_N",true,"V3_MacroBtn_A" );
   macroButton.SetDownUI     ( "V3_MacroBtn_D",true,"V3_MacroBtn_A" );
   macroButton.SetHighlightUI( "V3_MacroBtn_H",true,"V3_MacroBtn_A" );
   SetHelpId( macroButton, 449 );
   macroButtonEvent.SetUI( this );

   TradeButton.SetSpriteId   ( "V3_TradeBtn_N",true,"V3_TradeBtn_A");
   TradeButton.SetDownUI     ( "V3_TradeBtn_D",true,"V3_TradeBtn_A");
   TradeButton.SetHighlightUI( "V3_TradeBtn_H",true,"V3_TradeBtn_A");
   SetHelpId( TradeButton, 574 );
   tradeButtonEvent.SetUI( this );

   junkButton.SetSpriteId   ( "V3_TrashBtn_N",true,"V3_TrashBtn_A");
   junkButton.SetDownUI     ( "V3_TrashBtn_N",true,"V3_TrashBtn_A");
   junkButton.SetHighlightUI( "V3_TrashBtn_H",true,"V3_TrashBtn_A");
   SetHelpId( junkButton, 281 );
   junkButtonEvent.SetUI( this );

   dropButton.SetSpriteId   ( "V3_DropBtn_N",true,"V3_DropBtn_A");
   dropButton.SetDownUI     ( "V3_DropBtn_D",true,"V3_DropBtn_A");
   dropButton.SetHighlightUI( "V3_DropBtn_H",true,"V3_DropBtn_A");
   SetHelpId( dropButton, 280 );
   dropButtonEvent.SetUI( this );
   
 

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_backIconsS); 
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

   AddChild( &statSTR);
   AddChild( &statEND);
   AddChild( &statAGI);
   AddChild( &statWIS);
   AddChild( &statINT);
   AddChild( &statAC );
   AddChild( &statGold);

   AddChild( &iconSTR );
   AddChild( &iconEND );
   AddChild( &iconAGI );
   AddChild( &iconWIS );
   AddChild( &iconINT );
   AddChild( &iconAC  );
   AddChild( &iconGold);

   AddChild( &capeDropZone  );
   AddChild( &leftDropZone  );
   AddChild( &headDropZone  );
   AddChild( &bodyDropZone  );
   AddChild( &rightDropZone );
   AddChild( &legsDropZone  );
   AddChild( &feetDropZone  );
   AddChild( &glovesDropZone);
   AddChild( &mainDropZone  );
   AddChild( &neckDropZone  );
   AddChild( &wristDropZone );
   AddChild( &beltDropZone  );
   AddChild( &ring1DropZone );
   AddChild( &ring2DropZone );
   AddChild( &Orbe1DropZone );
   AddChild( &inventoryGrid );

   AddChild( &macroButton);
   AddChild( &TradeButton);
   AddChild( &junkButton);
   AddChild( &dropButton);

   AddChild( &Drag_Button);

   LoadBackpack();
   InitializeMacro();
  
   RootBoxUI::GetInstance()->AddChild( this );

   ReadDisplayBDImage();

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

} 

V3_InvDlg::~V3_InvDlg()
{
   m_vImageDisplay.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_InvDlg *V3_InvDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{
    if (g_pInvDlgInstance)
       return g_pInvDlgInstance;

    static V3_InvDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_InvDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallInv > 500)
      {
         g_dwLastMacroCallInv =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}

unsigned int V3_InvDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   g_Global.SetLastDialogOpen(1);

   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   // Get the stats equipment and Inv
   TFCPacket sending;	
   sending << (RQ_SIZE)RQ_ViewInv;	
   SEND_PACKET( sending );
   
   /*
   TFCPacket sending;	
   // Get the stats.
   sending << (RQ_SIZE)RQ_GetStatus;	
   SEND_PACKET( sending );

   sending.Destroy();
   // Get the equipment.
   sending << (RQ_SIZE)RQ_ViewEquiped;
   SEND_PACKET( sending );

   sending.Destroy();
   sending << (RQ_SIZE)RQ_ViewBackpack2;
   SEND_PACKET( sending );
   */

   GetInstance()->Show();

   return 0;
}

void V3_InvDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_backIconsS.SetPos( FramePos( iPosX+73, iPosY+402 , iPosX+391,iPosY+429));
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


   statSTR .SetPos ( FramePos( iPosX+96 , iPosY+407 , iPosX+140, iPosY+424) );
   statEND .SetPos ( FramePos( iPosX+170, iPosY+407 , iPosX+213, iPosY+424) );
   statAGI .SetPos ( FramePos( iPosX+244, iPosY+407 , iPosX+287, iPosY+424) );
   statINT .SetPos ( FramePos( iPosX+318, iPosY+407 , iPosX+361, iPosY+424) );
   statWIS .SetPos ( FramePos( iPosX+392, iPosY+407 , iPosX+435, iPosY+424) );
   statAC  .SetPos ( FramePos( iPosX+490, iPosY+407 , iPosX+533, iPosY+424) );
   statGold.SetPos ( FramePos( iPosX+448, iPosY+367 , iPosX+548, iPosY+387) );
   
   iconSTR .SetPos ( FramePos( iPosX+73 , iPosY+402 , iPosX+95 , iPosY+429) );
   iconEND .SetPos ( FramePos( iPosX+146, iPosY+402 , iPosX+169, iPosY+429) );
   iconAGI .SetPos ( FramePos( iPosX+220, iPosY+402 , iPosX+243, iPosY+429) );
   iconWIS .SetPos ( FramePos( iPosX+294, iPosY+402 , iPosX+317, iPosY+429) );
   iconINT .SetPos ( FramePos( iPosX+368, iPosY+402 , iPosX+391, iPosY+429) );
   iconAC  .SetPos ( FramePos( iPosX+466, iPosY+402 , iPosX+489, iPosY+429) );
   iconGold.SetPos ( FramePos( iPosX+543, iPosY+366 , iPosX+567, iPosY+388) );

   mainDropZone  .SetPos(  FramePos( iPosX+114, iPosY+183, iPosX+212 , iPosY+305 ) );

   headDropZone  .SetPos(  FramePos( iPosX+137, iPosY+97 , iPosX+192, iPosY+152 ) );
   bodyDropZone  .SetPos(  FramePos( iPosX+43 , iPosY+152, iPosX+98 , iPosY+234 ) );
   legsDropZone  .SetPos(  FramePos( iPosX+136, iPosY+322, iPosX+191, iPosY+388 ) );
   
   rightDropZone .SetPos(  FramePos( iPosX+43 , iPosY+243, iPosX+98 , iPosY+325 ) );
   leftDropZone  .SetPos(  FramePos( iPosX+229, iPosY+243, iPosX+284, iPosY+325 ) );
   

   feetDropZone  .SetPos(  FramePos( iPosX+195, iPosY+334, iPosX+238, iPosY+377 ) );
   capeDropZone  .SetPos(  FramePos( iPosX+229, iPosY+152, iPosX+284, iPosY+234 ) );
   glovesDropZone.SetPos(  FramePos( iPosX+43 , iPosY+334, iPosX+86 , iPosY+377 ) );
   beltDropZone  .SetPos(  FramePos( iPosX+89 , iPosY+334, iPosX+132, iPosY+377 ) );
   neckDropZone  .SetPos(  FramePos( iPosX+57, iPosY+116,  iPosX+86,  iPosY+145 ) );
   wristDropZone .SetPos(  FramePos( iPosX+97, iPosY+109,  iPosX+126, iPosY+138 ) );
   ring1DropZone .SetPos(  FramePos( iPosX+202, iPosY+109, iPosX+231, iPosY+138 ) );
   ring2DropZone .SetPos(  FramePos( iPosX+241, iPosY+116, iPosX+270, iPosY+145 ) );
   Orbe1DropZone .SetPos(  FramePos( iPosX+241, iPosY+334, iPosX+284, iPosY+377 ) );

   instantItemInfo.SetPos( FramePos( iPosX+306, iPosY+93, iPosX+540, iPosY+118 ) );
   instantItemInfo.SetCenter( false );
   
   inventoryGrid  .SetGridDef(iPosX,iPosY,20,309 ,127,567,358,&gridScrollUpGraph,&gridScrollDownGraph);


   macroButton.SetPos( FramePos( iPosX+416, iPosY+367, iPosX+416+22, iPosY+367+22 ) );
   TradeButton.SetPos( FramePos( iPosX+384, iPosY+367, iPosX+384+22, iPosY+367+22 ) );
   junkButton .SetPos( FramePos( iPosX+309, iPosY+367, iPosX+309+22, iPosY+367+22 ) );
   dropButton .SetPos( FramePos( iPosX+352, iPosY+367, iPosX+352+22, iPosY+367+22 ) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_InvDlg::Draw(V2SPRITEFX *vsfFX)
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
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[23],strlen(g_GUILocalString[23]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);


   // Get the current mouse coordinates.
   MousePos mousePos( MouseDevice::xPos, MouseDevice::yPos );
   
   ItemUI * itemUI = NULL;
   if( inventoryGrid.GetPos().MousePosInFrame( mousePos ) )
   {
      itemUI = static_cast< ItemUI * >( inventoryGrid.GetGridItemAt( mousePos ) );
   }
   
   // Display the instant help text.
   if( itemUI != NULL )
   {
      //TextObject *to = itemUI->GetHelpText();
      LPBAG_ITEM bagItem = itemUI->bagItem;
      
      // If the item doesn't have a name.
      if(/* to == NULL && */bagItem->chIName[ 0 ] == 0 )
      {
         // Send a query item's name.
         TFCPacket sending;
         sending << (RQ_SIZE)RQ_QueryItemName;
         sending << (char)PL_SEARCHBACKPACK;
         sending << (long)bagItem->dwID;
         SEND_PACKET( sending );
         
         // Avoid the drawing thread to send continuous QueryItemName packets.
         bagItem->chIName[ 0 ] = ' ';
         bagItem->chIName[ 1 ] = 0;
      }
      else
      {
         //if( to != NULL )
         {
            //instantItemInfo.SetText(to->GetText(),CL_ORANGE,246,0,false,1);
            instantItemInfo.SetText(itemUI->strName2,CL_ORANGE,246,0,false,1);
            instantItemInfo.Draw( vsfFX );
         }
      }        
   }
   
   //0 ==    
   //1 == /  bas gauche
   //2 == |  de face 
   //3 == \  bas droit
   //4 == <  vers la gauche
   //5 == 
   //6 == >  vers la droite
   //7 == \  haut gauche
   //8 == |  de dos
   //9 == /  haut droit
   
   static int dwCnt     = 0;
   static int dwDir     = 0;
   static int dwRealDir = 2;
   dwCnt++;
   static int dwOffset = 0;
   static int dwIncr   = 1;
   if(dwCnt >8)
   {
      dwCnt = 0;
      
      
      //dwDir++; 
      //if(dwDir >7)
      //   dwDir = 0;
      
      
      dwDir+=dwIncr; 
      if(dwDir >4)
      {
         dwDir = 4;
         dwIncr = -1;
      }
      else if(dwDir < 0)
      {
         dwDir = 0;
         dwIncr = 1;
      }
      
      switch(dwDir)
      {
         case 0: dwRealDir = 2; dwOffset = 0;break;
         case 1: dwRealDir = 1; dwOffset = 0;break;
         case 2: dwRealDir = 4; dwOffset = 0;break;
         case 3: dwRealDir = 7; dwOffset = 0;break;
         case 4: dwRealDir = 8; dwOffset = 0;break;
         case 5: dwRealDir = 9; dwOffset = 5;break;
         case 6: dwRealDir = 6; dwOffset = 5;break;
         case 7: dwRealDir = 3; dwOffset = 5;break;
      }

   }
   
   
   Objects.Lock(0);
   Objects.DrawPuppet( iOffX+148+dwOffset, iOffY+273,dwRealDir );
   Objects.Unlock(0);
}

void V3_InvDlg::Hide()
{
   BoxUI::Hide();
}




void V3_InvDlg::StartDragDlg( MousePos mousePos )
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

void V3_InvDlg::StopDragDlg()
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
void V3_InvDlg::DragEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_InvDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}






void V3_InvDlg::TabStatsEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
   
   V3_StatsDlg::GetInstance()->Display(NULL);
   me->Hide();
}

// void V3_InvDlg::TabInvEvent::LeftClicked( void )
// {
//    V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
//    
// }

void V3_InvDlg::TabSpellEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
   
   V3_SpellDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_InvDlg::TabProfessionEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
   
   V3_ProfessionDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_InvDlg::TabGuildeEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );

   V3_GuildDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_InvDlg::TabGroupEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
   
   V3_GroupeDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_InvDlg::TabMacroEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );

   V3_MacroDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_InvDlg::TabOptionEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
   
   V3_OptionsDlg::GetInstance()->Display(NULL);
   me->Hide();
}






















// Query wether the item can be dragged away from the equipment box. Some items are
// fixed in the equip boxes, such as Seraph wings.
// Return: bool, true if the item can be dragged.
bool V3_InvDlg::EquipEvent::CanDrag(GameUI *idragUI)
{
    ItemUI *dragUI = static_cast< ItemUI * >( idragUI );
    return true;
}

// Called when an item was successfully dropped from the equipment box.
void V3_InvDlg::EquipEvent::Dropped(EventVisitor *sourceEvent)
{
    ItemUI *dragUI = static_cast< ItemUI * >( GetDragItem() );
    
    // Must drop an item.
    if( dragUI == NULL )
    {
       return;
    }
    
    // Send an unequip packet.
    TFCPacket sending;
    sending << (RQ_SIZE)RQ_UnequipObject;
    bool proceed = true;
    V3_InvDlg *invUI = static_cast< V3_InvDlg * >( GetUI() );
    DropZoneUI *dropZone = static_cast< DropZoneUI * >( GetDragParentUI() );
    char equipPos = 0;
    
    // Determine from which event came this item.
    if( dropZone == &invUI->bodyDropZone )        
       equipPos = body;
    else if( dropZone == &invUI->headDropZone )
       equipPos = helm;
    else if( dropZone == &invUI->leftDropZone )
       equipPos = weapon_left;
    else if( dropZone == &invUI->rightDropZone )
       equipPos = weapon_right;
    else if( dropZone == &invUI->legsDropZone )
       equipPos = legs;
    else if( dropZone == &invUI->feetDropZone )
       equipPos = feet;
    else if( dropZone == &invUI->capeDropZone )
       equipPos = sleeves;
    else if( dropZone == &invUI->glovesDropZone )
       equipPos = gloves;
    else if( dropZone == &invUI->neckDropZone )
       equipPos = necklace;
    else if( dropZone == &invUI->wristDropZone )
       equipPos = bracelets;
    else if( dropZone == &invUI->beltDropZone )
       equipPos = belt;
    else if( dropZone == &invUI->ring1DropZone )
       equipPos = ring1;
    else if( dropZone == &invUI->ring2DropZone )
       equipPos = ring2;
    else if( dropZone == &invUI->Orbe1DropZone )
       equipPos = Orbe1;
    else
       proceed = false;
    
    
    
    if( proceed )
    {
       // Add the item to the backpack.
       Player.tlBackpack.Lock( "EquipEvent::Dropped" );
       
       bool found = false;
       // Try to find the item in the backpack.
       Player.tlBackpack.ToHead();
       while( Player.tlBackpack.QueryNext() )
       {
          if( Player.tlBackpack.GetObject()->associatedUI == dragUI )
          {
             found = true;
          }
       }
       
       dragUI->bagItem->associatedUI = dragUI;
       
       // If it wasn't in the backpack.
       if( !found )
          Player.tlBackpack.AddToHead( dragUI->bagItem );// Add it.

       Player.tlBackpack.Unlock( "EquipEvent::Dropped" );
       
       dropSound->Play( TRUE );
       
       Player.lpbEquipped[ equipPos ] = NULL;  // Remove the bag item from its equip position.
       sending << (char)equipPos;
       SEND_PACKET( sending );
    }
}

//  Called when an item is dropped in the equipment box.
bool V3_InvDlg::EquipEvent::Dropping( void )
{
    bool resetDrag = true;
    
    // If the source event is the equipment event.
    if( GetUI()->GetDragSourceEvent() == this )
    {
        // Re-sync the equipment box.
        static_cast< V3_InvDlg * >( GetUI() )->SyncEquipment();
    }
    else
    {        
        ItemUI *itemUI = static_cast< ItemUI * >( GetDragItem() );
        bool destroyItem = true;
        // If the item is a stack
        if( itemUI->bagItem->dwQty > 1 )
        {
            // Drop the item back to its original position
            if( DropBack( itemUI ) )
            {
                // Do not destroy the item anymore
                destroyItem = false;
            }
        }
        
        // Retreive the bag item ID.
        DWORD itemID = itemUI->bagItem->dwID;
        
        if( destroyItem )
        { 
            // De-associate the UI from the bag item.
            itemUI->bagItem->associatedUI = NULL;        
            itemUI->UpdateName();
            // Delete the itemUI.
            delete itemUI;
        }
        dropSound->Play( TRUE );
        // Send an equip item packet.
        TFCPacket sending;

        sending << (RQ_SIZE)RQ_EquipObject;
        sending << (long)itemID;
        SEND_PACKET( sending );
    }

    if( resetDrag )
    {
        // Reset mouse cursor.
        CMouseCursor::GetInstance()->SetCustomCursor( NULL );
        // Cancel drag mode.
        CancelDrag();
    }
    
    // Never let an item be dropped in the equipment box, its handling will
    // be done by the packets.
    return false;
}

//  Dragged something from the equip dropzone
void V3_InvDlg::EquipEvent::Dragged( void )
{
   ItemUI *item;
   if( GetDragItem() == NULL )
   {
      return;
   }
   else
   {
      item = static_cast< ItemUI * >( GetDragItem() );
   }
   ItemDragSounds( item->bagItem->Type )->Play( TRUE );
}





//////////////////////////////////////////////////////////////////////////////////////////
// Uses an item in the inventory grid.
// 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_InvDlg::InventoryGridEvent::LeftDblClicked()
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );

   ItemUI *item;
   
   OutputDebugString("Inventory DblClick.\n");
   
   if( GetDragItem() == NULL )
   {
      return;
   }
   else
   {
      item = static_cast< ItemUI * >( GetDragItem() );
   }

   if (!strcmp( (const char *)item->bagItem->chIName, "Unsealed letter from Owain" )    )
   {
      DropBack2( item,true);
      V3_DisplaySpecialDlg::GetInstance()->ShowLetter(1,"Unsealed letter from Owain");
      me->RequestForegroundControl( V3_DisplaySpecialDlg::GetInstance() );
   }
   else if (!strcmp( (const char *)item->bagItem->chIName, "Lettre d'Owain" )    )
   {
      DropBack2( item,true);
      V3_DisplaySpecialDlg::GetInstance()->ShowLetter(1,"Lettre d'Owain");
      me->RequestForegroundControl( V3_DisplaySpecialDlg::GetInstance() );
   }
   else if (!strcmp( (const char *)item->bagItem->chIName, "Crimsonscale's letter" ))
   {
      DropBack2( item,true);
      V3_DisplaySpecialDlg::GetInstance()->ShowLetter(0,"Crimsonscale's letter");
      me->RequestForegroundControl( V3_DisplaySpecialDlg::GetInstance() );
   }
   else if (!strcmp( (const char *)item->bagItem->chIName, "Lettre de Crimsonscale" ))
   {
      DropBack2( item,true);
      V3_DisplaySpecialDlg::GetInstance()->ShowLetter(0,"Lettre de Crimsonscale");
      me->RequestForegroundControl( V3_DisplaySpecialDlg::GetInstance() );
   } 
   else if (!strcmp( (const char *)item->bagItem->chIName, "Carte du labyrinthe" ))
   {
      DropBack2( item,true);
      V3_DisplaySpecialDlg::GetInstance()->ShowLetter(2,"Carte du labyrinthe");
      me->RequestForegroundControl( V3_DisplaySpecialDlg::GetInstance() );
   }
   else 
   {
	   if (!strcmp( (const char *)item->bagItem->chIName, "D?blocage 7?me renaissance" ))
	   {
		   if(g_Var.dw1stAprilCnt == 0)
		   {
			   g_Var.wBubbleAnimCnt01 = 1;
			   g_Var.wBubbleAnimCnt01R= 0;
			   g_Var.wBubbleAnimCnt02 = 5;
			   g_Var.wBubbleAnimCnt02R= 0;
			   g_Var.wBubbleAnimCnt03 = 10;
			   g_Var.wBubbleAnimCnt03R= 0;
			   g_Var.dw1stAprilCnt = 1;
			   g_Var.iFishXPos     = -g_Var.vsfFish.GetWidth();
			   g_Var.iFishYPos     = g_Global.GetScreenH()-(rand()%100)-(2*g_Var.vsfFish.GetHeight());
			   g_Var.bDisplayedMessage = false;
			   sprintf_s(g_Var.strFishMessage,1024,"%s","");
		   }
	   }

      bool bFound = false;
      int  iIndex = 0;
      
      for(unsigned long i=0;i<V3_InvDlg::GetInstance()->m_vImageDisplay.size();i++)
      {
         if (!_stricmp( (const char *)item->bagItem->chIName, V3_InvDlg::GetInstance()->m_vImageDisplay[i].strName ))
         {
            bFound = true;
            iIndex = i;
            i = V3_InvDlg::GetInstance()->m_vImageDisplay.size()+1;
         } 
      }

      if(bFound)
      {
         unsigned char *pImage = new unsigned char[g_DSDlgW*g_DSDlgH];
         unsigned char *pPal   = new unsigned char[256*3];
         V3_InvDlg::GetInstance()->ReadImageFile(V3_InvDlg::GetInstance()->m_vImageDisplay[iIndex].iOffset,pImage,pPal);

         DropBack2( item,true);

         V3_DisplaySpecialDlg::GetInstance()->ShowLoadedBmp(pImage,pPal,g_DSDlgW,g_DSDlgH,V3_InvDlg::GetInstance()->m_vImageDisplay[iIndex].strName);
         me->RequestForegroundControl( V3_DisplaySpecialDlg::GetInstance() );

         delete []pImage;
         delete []pPal;

      }
      else
      {
		  me->UseItemDirect(item,item->bagItem);
      }
   }

   if (!strcmp( (const char *)item->bagItem->chIName, "D?blocage 7?me renaissance" ))
   {
	  V3_InvDlg::GetInstance()->Hide();
   }
}

// Verify that the dropped item is ok.
// 
// Return: bool, true if dropped item is ok, false otherwise.

bool V3_InvDlg::InventoryGridEvent::Dropping( void )
{
   GameUI *dragItem = GetDragItem();
   
   if( dragItem == NULL )
   {
      return false;
   }
   
   // If the dropped item is an ItemUI
   if( typeid( *dragItem ) == typeid( ItemUI )                      || 
       typeid( *dragItem ) == typeid( V3_ChestDlg::ItemUI )         || 
       typeid( *dragItem ) == typeid( V3_ChestNewDlg::ItemUI )      || 
       typeid( *dragItem ) == typeid( V3_GuildChestDlg::ItemUI )    || 
       typeid( *dragItem ) == typeid( V3_GuildChestNewDlg::ItemUI ) || 
       typeid( *dragItem ) == typeid( V3_TradeDlg::ItemUI )           )
   {
      // V3_TradeDlg items are handled by packets, so no dropping handling allowed for them.
      if ( GetUI()->GetDragSourceEvent() == &(V3_TradeDlg::GetInstance()->tradeGridEvent) ) 
      {
         V3_TradeDlg::GetInstance()->EventDroppingItemUIFromTradeToBackpack();
         return false;
      }
      
      // V3_ChestDlg items are handled by packets, so no dropping handling allowed for them.
      if ( GetUI()->GetDragSourceEvent() == &(V3_ChestDlg::GetInstance()->chestGridEvent) ) 
      {
         ItemUI *iuiItem = (ItemUI*)dragItem;
         V3_ChestDlg *chestUI = V3_ChestDlg::GetInstance();
         

         chestUI->m_pItemUIReq = iuiItem;


         DropBack(dragItem);
         if(iuiItem->bagItem->dwQty > 0)
         {
            if( iuiItem->bagItem->dwQty == 1 )
            {
               chestUI->m_pItemUIReq = iuiItem;   
               chestUI->AskValueReturnSld(true,g_AskChestToBackpack,NULL,1);
               return false;
            }

            V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[38] ,iuiItem->bagItem->dwQty,0);
            V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
            V3_AskValueSldDlg::GetInstance()->SetParent(chestUI);
            V3_AskValueSldDlg::GetInstance()->SetDelay(500);
            V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+chestUI->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+chestUI->m_iYPos);
            V3_AskValueSldDlg::GetInstance()->Show(g_AskChestToBackpack,0);

            chestUI->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );
         }
         return false;
      }

      // V3_ChestNewDlg items are handled by packets, so no dropping handling allowed for them.
      if ( GetUI()->GetDragSourceEvent() == &(V3_ChestNewDlg::GetInstance()->chestGridEvent) ) 
      {
         ItemUI *iuiItem = (ItemUI*)dragItem;
         V3_ChestNewDlg *chestNewUI = V3_ChestNewDlg::GetInstance();


         chestNewUI->m_pItemUIReq = iuiItem;


         DropBack(dragItem);

         if( iuiItem->bagItem->dwQty == 1 )
         {
            chestNewUI->m_pItemUIReq = iuiItem;   
            chestNewUI->AskValueReturnSld(true,g_AskChestToBackpack,NULL,1);
            return false;
         }

         V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[38] ,iuiItem->bagItem->dwQty,0);
         V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
         V3_AskValueSldDlg::GetInstance()->SetParent(chestNewUI);
         V3_AskValueSldDlg::GetInstance()->SetDelay(500);
         V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+chestNewUI->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+chestNewUI->m_iYPos);
         V3_AskValueSldDlg::GetInstance()->Show(g_AskChestToBackpack,0);

         chestNewUI->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );


         return false;
      }
      
      if ( GetUI()->GetDragSourceEvent() == &(V3_GuildChestDlg::GetInstance()->chestGuildGridEvent) ) 
      {
         ItemUI *iuiItem = (ItemUI*)dragItem;
         V3_GuildChestDlg *chestUI = V3_GuildChestDlg::GetInstance();

         chestUI->m_pItemUIReq = iuiItem;
         
         
         DropBack(dragItem);
         
         if( iuiItem->bagItem->dwQty == 1 )
         {
            chestUI->m_pItemUIReq = iuiItem;   
            chestUI->AskValueReturnSld(true,g_AskChestToBackpackGuild,NULL,1);
            return false;
         }
         
         V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[38] ,iuiItem->bagItem->dwQty,0);
         V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
         V3_AskValueSldDlg::GetInstance()->SetParent(chestUI);
         V3_AskValueSldDlg::GetInstance()->SetDelay(500);
         V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+chestUI->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+chestUI->m_iYPos);
         V3_AskValueSldDlg::GetInstance()->Show(g_AskChestToBackpackGuild,0);
         
         chestUI->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );
         
         DropBack(dragItem);
         
         return false;
      }

      if ( GetUI()->GetDragSourceEvent() == &(V3_GuildChestNewDlg::GetInstance()->chestGridEvent) ) 
      {
         ItemUI *iuiItem = (ItemUI*)dragItem;
         V3_GuildChestNewDlg *chestUI = V3_GuildChestNewDlg::GetInstance();

         chestUI->m_pItemUIReq = iuiItem;


         DropBack(dragItem);

         if( iuiItem->bagItem->dwQty == 1 )
         {
            chestUI->m_pItemUIReq = iuiItem;   
            chestUI->AskValueReturnSld(true,g_AskChestToBackpackGuild,NULL,1);
            return false;
         }

         V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[38] ,iuiItem->bagItem->dwQty,0);
         V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
         V3_AskValueSldDlg::GetInstance()->SetParent(chestUI);
         V3_AskValueSldDlg::GetInstance()->SetDelay(500);
         V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+chestUI->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+chestUI->m_iYPos);
         V3_AskValueSldDlg::GetInstance()->Show(g_AskChestToBackpackGuild,0);

         chestUI->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );

         DropBack(dragItem);

         return false;
      }
      
      return true;
   }
   
   
   return false;
}

//  Dragged something from the inventory grid.
void V3_InvDlg::InventoryGridEvent::Dragged( void )
{
   ItemUI *item;
   if( GetDragItem() == NULL )
   {
      return;
   }
   else
   {
      item = static_cast< ItemUI * >( GetDragItem() );
   }
   ItemDragSounds( item->bagItem->Type )->Play( TRUE );
}

//  Dropped something in the inventory grid
void V3_InvDlg::InventoryGridEvent::Dropped(EventVisitor *sourceEvent)
{
   dropSound->Play( TRUE ); 
}



//  Reset the Macro.
void V3_InvDlg::SetDefaultMacro( void )
{
   // Add the default hardcoded macros.
   // Provided for backward compatibility with the original F2 to F4 macros.
   ItemMacroUI *macroUI = new ItemMacroUI;
   
   // Light healing potions at F2
   macroUI->vKey.SetKey( VK_F2 );
   macroUI->iconId = 241;
   macroUI->name   = g_LocalString[ 434 ];
   macroUI->baseId = 40623;
   macroUI->SetVSF( ItemIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, macroUI->name.c_str(), &itemMacroEvent,false);
   
   // Mana potion at F3
   macroUI = new ItemMacroUI;
   macroUI->vKey.SetKey( VK_F3 );
   macroUI->iconId = 240;
   macroUI->name   = g_LocalString[ 436 ];
   macroUI->baseId = 40004;
   macroUI->SetVSF( ItemIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, macroUI->name.c_str(), &itemMacroEvent,false);
   
   // Torch at F4
   macroUI = new ItemMacroUI;
   macroUI->vKey.SetKey( VK_F4 );
   macroUI->iconId = 244;
   macroUI->name   = g_LocalString[ 435 ];
   macroUI->baseId = 40015;
   macroUI->SetVSF( ItemIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, macroUI->name.c_str(), &itemMacroEvent,false);
}

//  Updates the character stats.
void V3_InvDlg::UpdateCharacterSheet( void )
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   
   char buf[ 256 ];
   
   _itoa_s( Player.bStr, buf,256, 10 );
   statSTR.SetText( buf, GetColor( Player.Str, Player.bStr, 0 ) );
   _itoa_s( Player.bEnd, buf,256, 10 );
   statEND.SetText( buf, GetColor( Player.End, Player.bEnd, 0 ) );
   _itoa_s( Player.bAgi, buf,256, 10 );
   statAGI.SetText( buf, GetColor( Player.Agi, Player.bAgi, 0 ) );
   _itoa_s( Player.bWis, buf,256, 10 );
   statWIS.SetText( buf, GetColor( Player.Wis, Player.bWis, 0 ) );
   _itoa_s( Player.bInt, buf,256, 10 );
   statINT.SetText( buf, GetColor( Player.Int, Player.bInt, 0 ) );
   
   _itoa_s( Player.bAC, buf,256, 10 );
   statAC.SetText( buf, GetColor( Player.AC, Player.bAC, 0 ) );
   _itoa_s( Player.Gold, buf,256, 10 );
   statGold.SetText( buf, CL_ORANGE );
   
   FontManager *font = FontManager::GetInstance();
   
   statSTR.GetHelpText()->SetText( GetStatBoostString( Player.bStr, Player.Str ).c_str() );
   statEND.GetHelpText()->SetText( GetStatBoostString( Player.bEnd, Player.End ).c_str() );
   statAGI.GetHelpText()->SetText( GetStatBoostString( Player.bAgi, Player.Agi ).c_str() );
   statWIS.GetHelpText()->SetText( GetStatBoostString( Player.bWis, Player.Wis ).c_str() );
   statINT.GetHelpText()->SetText( GetStatBoostString( Player.bInt, Player.Int ).c_str() );
   statAC.GetHelpText()->SetText(  GetStatBoostString( Player.bAC,  Player.AC ).c_str() );
}

//  Updates the gold on the user.
void V3_InvDlg::UpdateGold( void )
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   char buf[ 256 ];
   _itoa_s( Player.Gold, buf,256, 10 );
   statGold.SetText(buf , CL_ORANGE );
}

//  Synchronizes the equipment content.
void V3_InvDlg::SyncEquipment( void )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   UpdateEquipDropZone( body,          &bodyDropZone );
   UpdateEquipDropZone( feet,          &feetDropZone );
   UpdateEquipDropZone( gloves,        &glovesDropZone );
   UpdateEquipDropZone( helm,          &headDropZone );
   UpdateEquipDropZone( legs,          &legsDropZone );	
   UpdateEquipDropZone( bracelets,     &wristDropZone );
   UpdateEquipDropZone( necklace,      &neckDropZone );
   UpdateEquipDropZone( weapon_right,  &rightDropZone );
   UpdateEquipDropZone( weapon_left,   &leftDropZone );
   UpdateEquipDropZone( ring1,         &ring1DropZone );
   UpdateEquipDropZone( ring2,         &ring2DropZone );	
   UpdateEquipDropZone( belt,          &beltDropZone );
   UpdateEquipDropZone( sleeves,       &capeDropZone );
   UpdateEquipDropZone( Orbe1,         &Orbe1DropZone );
}

//  Updates one equip position.
void V3_InvDlg::UpdateEquipDropZone(int bodyPos,DropZoneUI *dropZone)
{
   if( dropZone->GetItem() != NULL )
   {
      // Reset the drop zone's UI.
      delete dropZone->GetItem();            
      dropZone->SetItem( NULL );            
   }

   // If there is an equipped item at this position.
   if( Player.lpbEquipped[ bodyPos ] != NULL )
   {
      // If there is already an item
      if( dropZone->GetItem() != NULL )
      {
         ItemUI *item = static_cast< ItemUI * >( dropZone->GetItem() );
         // Update its bag item.
         item->bagItem = Player.lpbEquipped[ bodyPos ];
         Player.lpbEquipped[ bodyPos ]->associatedUI = item;
         item->UpdateName();
         item->UpdateName2();
      }
      else
      {
         // Create a new UI.
         ItemUI *newUI = new ItemUI;
         // Set its bag item.
         newUI->bagItem = Player.lpbEquipped[ bodyPos ];
         newUI->bagItem->associatedUI = newUI;
         newUI->SetVSF( Player.lpbEquipped[ bodyPos ]->lpSprite );
         newUI->SetPos( FramePos( 0, 0, Player.lpbEquipped[ bodyPos ]->lpSprite->GetWidth(),Player.lpbEquipped[ bodyPos ]->lpSprite->GetHeight() ));
         newUI->UpdateName();
         newUI->UpdateName2();
         // Set the drop zone's item
         dropZone->SetItem( newUI );
      }
   }
   else
   {
      // If the drop zone already has the item.
      if( dropZone->GetItem() != NULL )
      {
         // Reset the drop zone's UI.
         delete dropZone->GetItem();            
         dropZone->SetItem( NULL );            
      }
   }
}


//  Udpates the user's inventory.
void V3_InvDlg::UpdateInventory(TemplateList< BAG_ITEM > *newInventory)
{
   vector< GameUI * >   exceptList;
   vector< BAG_ITEM * > newList;
   
   // Scroll through the new inventory.
   newInventory->Lock( "InventoryUI::UpdateInventory" );
   newInventory->ToHead();
   while( newInventory->QueryNext() )
   {
      BAG_ITEM *bagItem = newInventory->GetObject();
      // If this bag already has an associated bag item.
      if( bagItem->associatedUI != NULL )
      {
         // Add it to the except list.
         exceptList.push_back( bagItem->associatedUI );
      }
      else
      {
         // Otherwise add it to the new list.
         newList.push_back( bagItem );
      }
   }
   newInventory->Unlock( "InventoryUI::UpdateInventory" );
   
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   // Delete all items in the grid which are not in the exceptList.
   inventoryGrid.DeleteGridExcept( exceptList );
   
   // If there is currently an item being dragged.
   if( GetDragItem() != NULL )
   {    
      
      if (GetDragSourceEvent() == &inventoryGridEvent || GetDragSourceEvent() == &equipEvent) 
      {
         // Add all equipped items to the except list.
         if( Player.lpbEquipped[ body ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ body ]->associatedUI );
         }
         if( Player.lpbEquipped[ feet ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ feet ]->associatedUI );
         }
         if( Player.lpbEquipped[ gloves ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ gloves ]->associatedUI );
         }
         if( Player.lpbEquipped[ helm ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ helm ]->associatedUI );
         }
         if( Player.lpbEquipped[ legs ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ legs ]->associatedUI );
         }
         if( Player.lpbEquipped[ bracelets ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ bracelets ]->associatedUI );
         }
         if( Player.lpbEquipped[ necklace ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ necklace ]->associatedUI );
         }
         if( Player.lpbEquipped[ weapon_right ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ weapon_right ]->associatedUI );
         }
         if( Player.lpbEquipped[ weapon_left ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ weapon_left ]->associatedUI );
         }
         if( Player.lpbEquipped[ ring1 ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ ring1 ]->associatedUI );
         }
         if( Player.lpbEquipped[ ring2 ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ ring2 ]->associatedUI );
         }
         if( Player.lpbEquipped[ belt ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ belt ]->associatedUI );
         }
         if( Player.lpbEquipped[ sleeves ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ sleeves ]->associatedUI );
         }
         if( Player.lpbEquipped[ Orbe1 ] != NULL ){
            exceptList.push_back( Player.lpbEquipped[ Orbe1 ]->associatedUI );
         }
         
         // If the drag item could not be found in the except list.
         if( find( exceptList.begin(), exceptList.end(), GetDragItem() ) == exceptList.end() )
         {
            // Stop dragging.
            delete GetDragItem();
            // Reset mouse cursor.
            CMouseCursor::GetInstance()->SetCustomCursor( NULL );
            // Cancel drag operation.
            CancelDrag();
         }
      }
   }

   // Insert all the newly found items.    
   UINT i;
   for( i = 0; i < newList.size(); i++ )
   {
      
      // If a sprite was specified.
      if( newList[ i ]->lpSprite != NULL )
      {
         // Create a new GameUI.
         ItemUI *newItem = new ItemUI;
         
         // Associate both bag item and item UI.
         newItem->bagItem = newList[ i ];
         newList[ i ]->associatedUI = newItem;
         
         newItem->Setup();
         
         // Add the UI to the inventory grid.
         inventoryGrid.AddItem( newItem );
      }
   }
}

//  Saves the backpack into the system's registry.
void V3_InvDlg::BackpackToSave( void )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   list< GridUI::GridItem > itemList;
   list< GridUI::GridItem >::iterator i;
   inventoryGrid.GetGridItemList( itemList );
   for( i = itemList.begin(); i != itemList.end(); i++ )
   {
      GridUI::GridItem &gridItem = *i;
      ItemUI *itemUI = static_cast< ItemUI * >( gridItem.ui );
      
      sInventory sNewInv;
      
      sNewInv.dwID         = itemUI->bagItem->wBaseID;
      sNewInv.dwX          = gridItem.gridPos.x;
      sNewInv.dwY          = gridItem.gridPos.y;
      sNewInv.dwType       = itemUI->bagItem->Type;
      g_SaveGame.AddInv(sNewInv);
   }
}

void V3_InvDlg::ClearItem()
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   inventoryGrid.DeleteGrid();
}

void V3_InvDlg::Reload()
{
   LoadBackpack();
   InitializeMacro();
}

//  Loads the backpack from the system's registry.
void V3_InvDlg::LoadBackpack( void )
{
   int dwNbrInv = g_SaveGame.GetNbrInv();
   
   set< DWORD > addedIds;
   if(dwNbrInv > 0)
   {
      for(int i =0;i<dwNbrInv; i++ )
      {
         //sInventory *pNewInv = NULL;
         sInventory *pNewInv = g_SaveGame.GetInv(i);
         
         GridUI::GridPos gridPos;
         gridPos.x = pNewInv->dwX;
         gridPos.y = pNewInv->dwY;
         
         
         // Create a new UI.
         ItemUI *itemUI = new ItemUI;
         
         LPBAG_ITEM bagItem = new BAG_ITEM;
         
         bagItem->associatedUI = itemUI;
         bagItem->wBaseID      = (WORD)pNewInv->dwID;
         bagItem->dwID         = 0; // Assign a null ID.
         bagItem->Type         = (WORD)pNewInv->dwType;
         bagItem->lpSprite     = InvItemIcons( pNewInv->dwType );
         bagItem->chIName[ 0 ] = 0;
         
         itemUI->bagItem = bagItem; // Associate the BAG_ITEM to the UI
         itemUI->Setup(); // Setup the item UI.
         
         // If the item ID was already added or the grid cannot accept the item.
         if( addedIds.find( bagItem->wBaseID ) != addedIds.end() || !inventoryGrid.AddGridItemAtPos( gridPos, itemUI ) )
         {
            // If the spot wasn't valid, junk this item UI.
            delete itemUI;
            delete bagItem;
         }
         else
         {
            addedIds.insert( bagItem->wBaseID );
            Player.tlBackpack.Lock( "EquipEvent::Load" );
            Player.tlBackpack.AddToTail( bagItem );
            Player.tlBackpack.Unlock( "EquipEvent::Load" );
         }
      }
   }
}

//  Client initializes the spell UI.
//  Load the spell macros.
void V3_InvDlg::InitializeMacro( void )
{
   int dwNbrItemMacro  = g_SaveGame.GetNbrMItem();
   int dwNbrSkillMacro = g_SaveGame.GetNbrMSkill();
   int dwNbrSpellMacro = g_SaveGame.GetNbrMSpell();
      
   int dwNbrMacro = dwNbrItemMacro+dwNbrSkillMacro+dwNbrSpellMacro;
   
   if(dwNbrMacro == 0)
      SetDefaultMacro();
   else
   {
      for(int i =0;i<dwNbrItemMacro; i++ )
      {
         ItemMacroUI *macroUI = new ItemMacroUI;
         
         sMacro *pNewMacro = g_SaveGame.GetMItem(i);
         
         macroUI->baseId    = pNewMacro->dwID;
         macroUI->iconId    = pNewMacro->dwIcon;
         memcpy(macroUI->chGridPos,pNewMacro->chFastPos,g_iNbrMacro);
         macroUI->vKey.SetLong(pNewMacro->dwKey);
         macroUI->name      = pNewMacro->strMacroName;
         
         macroUI->SetVSF( ItemIcons( macroUI->iconId ) );
         macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
         
         if( macroUI->baseId == 0 || macroUI->iconId == 0 )
            delete macroUI;
         else // Add the macro
            V3_MacroDlg::GetInstance()->AddMacro(macroUI, macroUI->name, &itemMacroEvent, false);
      }
   }
}

void V3_InvDlg::ReadImageFile(int dwOffset, BYTE *pImage, BYTE *pPal)
{
   ClientConfig *pConfig = g_Global.GetClientConfig();
   char strPathFile[1024];
   char strPathFileC[1024];
   sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGEUSENAME_DATA);
   sprintf_s(strPathFileC,1024,"%s\\%s%s",pConfig->strPath,Player.ServerName,V2IMAGEUSENAME_DATA);
   
   FILE *pfD = NULL;
   fopen_s(&pfD,strPathFileC,"rb");
   if(pfD == NULL)
      fopen_s(&pfD,strPathFile,"rb");
   if(pfD)
   {
      fseek( pfD, dwOffset, SEEK_SET);
      
      unsigned long dwData       = (g_DSDlgW*g_DSDlgH)+(256*3);
      unsigned char *pData       = new unsigned char[dwData];
      
      
      unsigned long dwDataC      = 0;
      fread(&dwDataC  ,4,1,pfD);
      unsigned char *pDataC      = new unsigned char[dwDataC];
      
      fread(pDataC    ,dwDataC,1,pfD);
      
      fclose(pfD);
      
      if( uncompress(pData, &dwData, pDataC, dwDataC) != Z_OK )
      {
         if(pData)
            delete []pData;
         if(pDataC)
            delete []pDataC;
         
         return;
      }
      
      BYTE *pchImageByte = new BYTE[g_DSDlgW*g_DSDlgH];
      
      int dwPosDest = 0;
      
      for(int y=g_DSDlgH-1;y>=0;y--)
      {
         for(int x=0;x<g_DSDlgW;x++)
         {
            pchImageByte[(y*g_DSDlgW)+x] = (BYTE) pData[(dwPosDest*g_DSDlgW)+x];
         }
         dwPosDest++;
      }
      
      memcpy(pImage,pchImageByte,g_DSDlgW*g_DSDlgH);
      memcpy(pPal  ,pData+(g_DSDlgW*g_DSDlgH),256*3);
      
      if(pData)
         delete []pData;
      if(pDataC)
         delete []pDataC;
      if(pchImageByte)
         delete []pchImageByte;
   }
}



//  Setups the given item UI according to its binded BAG_ITEM data.
void V3_InvDlg::ItemUI::Setup( void )
{
   // Set a 0, 0 pos to tell the grid to select a position.
   SetPos( FramePos( 0, 0, bagItem->lpSprite->GetWidth(), bagItem->lpSprite->GetHeight() ) );
   SetVSF( bagItem->lpSprite );
   UpdateName2();
}

//  Help requested for that object.
bool V3_InvDlg::ItemUI::RightMouseUp(MousePos mousePos)
{
   // If the item doesn't have a name.
   if( bagItem->chIName[ 0 ] == 0 )
   {
      // Send a query item's name.
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_QueryItemName;
      sending << (char)PL_SEARCHBACKPACK;
      sending << (long)bagItem->dwID;
      SEND_PACKET( sending );
   }
   
   g_Global.ResetItemInfo();
   g_Global.SetItemInfoID(bagItem->dwID);
   g_Global.SetItemInfoInventory(true);
   TFCPacket sending2;
   sending2 << (RQ_SIZE)RQ_QueryItemInfo;
   sending2 << (long)bagItem->dwID;
   SEND_PACKET( sending2 );
   return true;
}  

//  Updates the name of this item UI
void V3_InvDlg::ItemUI::UpdateName( void )
{
   if( strlen( reinterpret_cast< const char * >( bagItem->chIName ) ) == 0 )
   {
      return;
   }
   
   char name[ 512 ];
   if( bagItem->dwQty <= 1 )
   {
      if( bagItem->iCharges > 0 )
      {
         sprintf_s( name,512, "%s (x%u)", bagItem->chIName, bagItem->iCharges );
      }
      else if( bagItem->iCharges < 0 )
      {
         sprintf_s( name,512, g_LocalString[ 535 ], bagItem->chIName );            
      }
      else
      {
         sprintf_s( name,512, "%s", bagItem->chIName );
      }

   }
   else
   {
      sprintf_s( name,512, "%s x%u", bagItem->chIName, bagItem->dwQty );
   }
 
   SetHelpText( FontManager::GetInstance()->GetSmallFont(reinterpret_cast< const char * >( name ) ) );
}


//  Updates the name of this item UI
void V3_InvDlg::ItemUI::UpdateName2( void )
{
   if( strlen( reinterpret_cast< const char * >( bagItem->chIName ) ) == 0 )
   {
      return;
   }

   if( bagItem->dwQty <= 1 )
   {
      if( bagItem->iCharges > 0 )
      {
         sprintf_s( strName2,512, "%s (x%u)", bagItem->chIName, bagItem->iCharges );
      }
      else if( bagItem->iCharges < 0 )
      {
         sprintf_s( strName2,512, g_LocalString[ 535 ], bagItem->chIName );            
      }
      else
      {
         sprintf_s( strName2,512, "%s", bagItem->chIName );
      }

   }
   else
   {
      sprintf_s( strName2,512, "%s x%u", bagItem->chIName, bagItem->dwQty );
   }
}


//  Executes a spell macro.
void V3_InvDlg::ItemMacroEvent::ExecMacro(MacroDataUI *imacroUI)
{
   
   ItemMacroUI *itemMacro = static_cast< ItemMacroUI * >( imacroUI );
   
   // Check the backpack for items with this requested Id.
   Player.tlBackpack.Lock( "EquipEvent::Macro" );
   Player.tlBackpack.ToHead();
   while( Player.tlBackpack.QueryNext() )
   {
      LPBAG_ITEM bagItem = Player.tlBackpack.GetObject();
      
      if( bagItem->wBaseID == itemMacro->baseId )
      {
         
         V3_InvDlg::GetInstance()->UseItemDirect(NULL,bagItem);
        
         Player.tlBackpack.Unlock( "EquipEvent::Macro" );
         return;
      }
   }
   Player.tlBackpack.Unlock( "EquipEvent::Macro" );
   
   naSound->Play( TRUE );
   
   char buf[ 512 ];
   sprintf_s( buf,512, g_LocalString[ 432 ], itemMacro->name.c_str() );
   V3_ChatLogDlg::GetInstance()->AddListLogs( "", buf, SYSTEM_COLOR,true);
}

void V3_InvDlg::ItemMacroEvent::SaveMacro(const list< MacroDataUI * > &macroList)
{
   list< MacroDataUI * >::const_iterator i;
   for( i = macroList.begin(); i != macroList.end(); i++ )
   {
      const ItemMacroUI *macroUI = static_cast< const ItemMacroUI * >( *i );
      sMacro sNewMacro;
      
      sNewMacro.dwID       = macroUI->baseId;
      sNewMacro.dwIcon     = macroUI->iconId;
      memcpy(sNewMacro.chFastPos,macroUI->chGridPos,g_iNbrMacro);
      sNewMacro.dwKey      = macroUI->vKey.AsLong();
      sprintf_s(sNewMacro.strMacroName,256,"%s",macroUI->name.c_str());
      g_SaveGame.AddMacroItem(sNewMacro);
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_InvDlg::TradeButtonEvent::LeftClicked( void )
{
   V3_TradeDlg::GetInstance()->Display( NULL );
}

//  Left clicked on the macro button.
void V3_InvDlg::MacroButtonEvent::LeftClicked( void )
{
   // Check which item was brough to the macro box.
   ItemUI *itemUI = static_cast< ItemUI * >( GetDragItem() );
   
   if( itemUI == NULL )
   {
      // Notify the user how we must use this button.
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 431 ], SYSTEM_COLOR,false);
      naSound->Play( TRUE );
      return;
   }
   
   DropBack( itemUI );
   
   if( !ItemIcons.Exists( itemUI->bagItem->Type ) )
   {
      naSound->Play( TRUE );
      return;       
   }
   
   
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
   
   ItemMacroUI *macroUI = new ItemMacroUI;
   
   macroUI->iconId = itemUI->bagItem->Type;
   macroUI->name   = reinterpret_cast< const char * >( itemUI->bagItem->chIName );
   macroUI->baseId = itemUI->bagItem->wBaseID;
   macroUI->SetVSF( ItemIcons( macroUI->iconId ) );
   macroUI->SetPos( FramePos( 0, 0, macroUI->GetVSF()->GetWidth(), macroUI->GetVSF()->GetHeight() ) );
   
   // Add macro for spell.
   V3_MacroDlg::GetInstance()->AddMacro( macroUI, macroUI->name.c_str(), &me->itemMacroEvent );
}

// Drop button click.
void V3_InvDlg::DropButtonEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
   
   GameUI *dragItem = GetDragItem();
   if( dragItem == NULL )
   {
      return;
   }
   
   // If the dropped item is not an ItemUI
   if( typeid( *dragItem ) != typeid( ItemUI ) )
   {        
      return;
   }
   
   //If the item comes from the chest, tell the player its not allowed
   if ( GetUI()->GetDragSourceEvent() == &(V3_ChestDlg::GetInstance()->chestGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 561 ], SYSTEM_COLOR,false);
      return;
   }

   //If the item comes from the chest, tell the player its not allowed
   if ( GetUI()->GetDragSourceEvent() == &(V3_ChestNewDlg::GetInstance()->chestGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 561 ], SYSTEM_COLOR,false);
      return;
   }
   
   //If the item comes from the chest, tell the player its not allowed
   if ( GetUI()->GetDragSourceEvent() == &(V3_GuildChestDlg::GetInstance()->chestGuildGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 561 ], SYSTEM_COLOR,false);
      return;
   }

   //If the item comes from the chest, tell the player its not allowed
   if ( GetUI()->GetDragSourceEvent() == &(V3_GuildChestNewDlg::GetInstance()->chestGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 561 ], SYSTEM_COLOR,false);
      return;
   }

   //If the item comes from the chest, tell the player its not allowed
   if ( GetUI()->GetDragSourceEvent() == &(V3_TradeDlg::GetInstance()->tradeGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 561 ], SYSTEM_COLOR,false);
      return;
   }
   
   
   ItemUI *itemUI = static_cast< ItemUI * >( dragItem );
   
   GameUI *src = GameUI::GetDragParentUI();    
   bool nonGridSource = false;
   if( typeid( *src ) != typeid( GridUI ) )
   {
      nonGridSource = true;
   }
   
   DropBack( itemUI );
   
   if( nonGridSource )
   {
      // Notify the user how we must use this button.
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 505 ], SYSTEM_COLOR,false);
      
      naSound->Play( TRUE );
      return;
   }
   
   if( itemUI->bagItem->dwQty == 0 )
   {
      return;
   }
   if( itemUI->bagItem->dwQty == 1 )
   {
      me->m_pItemUIReq = itemUI;   
      me->AskValueReturnSld(true,g_AskDrop,NULL,1);
      return;
   }

   me->m_pItemUIReq = itemUI;
   V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[38] ,itemUI->bagItem->dwQty,0);
   V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueSldDlg::GetInstance()->SetParent(me);
   V3_AskValueSldDlg::GetInstance()->SetDelay(500);
   V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueSldDlg::GetInstance()->Show(g_AskDrop,0);
   
   me->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );
}

// Junk button click
void V3_InvDlg::JunkButtonEvent::LeftClicked( void )
{
   V3_InvDlg *me = static_cast< V3_InvDlg * >( GetUI() );
   
   GameUI *dragItem = GetDragItem();
   if( dragItem == NULL )
   {
      return;
   }
   
   // If the dropped item is not an ItemUI
   if( typeid( *dragItem ) != typeid( ItemUI )	)
   {        
      return;
   }
   
   //If the item comes from the chest, tell the player its not allowed
   if ( GetUI()->GetDragSourceEvent() == &(V3_ChestDlg::GetInstance()->chestGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 560 ], SYSTEM_COLOR,false);
      return;
   }

   //If the item comes from the chest, tell the player its not allowed
   if ( GetUI()->GetDragSourceEvent() == &(V3_ChestNewDlg::GetInstance()->chestGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 560 ], SYSTEM_COLOR,false);
      return;
   }
   
   if ( GetUI()->GetDragSourceEvent() == &(V3_GuildChestDlg::GetInstance()->chestGuildGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 560 ], SYSTEM_COLOR,false);
      return;
   }

   if ( GetUI()->GetDragSourceEvent() == &(V3_GuildChestNewDlg::GetInstance()->chestGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 560 ], SYSTEM_COLOR,false);
      return;
   }

   if ( GetUI()->GetDragSourceEvent() == &(V3_TradeDlg::GetInstance()->tradeGridEvent) ) 
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 560 ], SYSTEM_COLOR,false);
      return;
   }
   
   
   
   ItemUI *itemUI = static_cast< ItemUI * >( dragItem );  
   
   GameUI *src = GameUI::GetDragParentUI();    
   bool nonGridSource = false;
   if( typeid( *src ) != typeid( GridUI ) )
   {
      nonGridSource = true;
   }
   
   
   DropBack( itemUI );
   
   if( nonGridSource )
   {
      // Notify the user how we must use this button.
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 506 ], SYSTEM_COLOR,false);
      naSound->Play( TRUE );
      return;
   }
   
   if( itemUI->bagItem->dwQty == 0 )
   {
      return;
   }

   if( itemUI->bagItem->dwQty == 1 )
   {
      me->m_pItemUIReq = itemUI;   
      me->AskValueReturnSld(true,g_AskJunk,NULL,1);
      return;
   }
   
   me->m_pItemUIReq = itemUI;
   V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[36] ,itemUI->bagItem->dwQty,0);
   V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueSldDlg::GetInstance()->SetParent(me);
   V3_AskValueSldDlg::GetInstance()->SetDelay(500);
   V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueSldDlg::GetInstance()->Show(g_AskJunk,0);
   
   me->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );
}



void V3_InvDlg::AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal)
{
   if(!bOK || iSldVal == 0)
      return;


   switch(uiShowCodeP)
   {
      case g_AskDrop:
      {
         TFCPacket sending;
         
         DWORD dwQty = iSldVal;
         // If the player put too much items.
         if( dwQty > m_pItemUIReq->bagItem->dwQty )
         {
            dwQty = m_pItemUIReq->bagItem->dwQty;
         }
         
         // Fill in a drop packet.
         sending << (RQ_SIZE)RQ_DepositObject;
         sending << (short)Player.xPos;        
         sending << (short)Player.yPos;
         sending << (long)m_pItemUIReq->bagItem->dwID;
         sending << (long)dwQty;
         
         // Send the packet.
        SEND_PACKET( sending );
      }      
      break;
      case g_AskJunk:
         m_iJunkQty = iSldVal;
         V3_ConfirmDlg::GetInstance()->SetDialogText(g_GUILocalString[37]);
         V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
         V3_ConfirmDlg::GetInstance()->SetParent(this);
         V3_ConfirmDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+m_iYPos);
         V3_ConfirmDlg::GetInstance()->Show(0);
         RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
      break;
   }
}

void V3_InvDlg::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   if(!bOK || m_iJunkQty <= 0)
      return;

   
   switch(uiShowCodeP)
   {
      case 0:
      {
         if(bOK)
         {
            TFCPacket sending;
            
            // If the player put too much items.
            if( (UINT)m_iJunkQty > m_pItemUIReq->bagItem->dwQty )
            {
               m_iJunkQty = m_pItemUIReq->bagItem->dwQty;
            }
            
            // Fill in a drop packet.
            sending << (RQ_SIZE)RQ_JunkItems;
            sending << (long)m_pItemUIReq->bagItem->dwID;
            sending << (long)m_iJunkQty;
            // Send the packet.
            SEND_PACKET( sending );
         }
      }
      break;
   }
}


void V3_InvDlg::ReadDisplayBDImage()
{
   int dwNbrImage = 0;
   
   unsigned long dwHeader       = (1000*54);
   unsigned char *pHeader       = new unsigned char[dwHeader];
   memset(pHeader,0x00,dwHeader);
   
   
   unsigned long dwHeaderC      = 0;
   unsigned char *pHeaderC      = NULL;
   
   
   ClientConfig *pConfig = g_Global.GetClientConfig();
   char strPathFile[1024];
   char strPathFileC[1024];
   sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGEUSENAME_HEADER);
   sprintf_s(strPathFileC,1024,"%s\\%s%s",pConfig->strPath,Player.ServerName,V2IMAGEUSENAME_HEADER);
   
   
   FILE *pfH = NULL;
   fopen_s(&pfH,strPathFileC,"rb");
   if(pfH == NULL)
      fopen_s(&pfH,strPathFile,"rb");
   if(pfH)
   {
      fread(&dwNbrImage ,4,1,pfH);
      fread(&dwHeaderC  ,4,1,pfH);
      pHeaderC       = new unsigned char[dwHeaderC];
      fread(pHeaderC    ,dwHeaderC,1,pfH);
      fclose(pfH);
   }
   else
   {
      if(pHeader)
         delete []pHeader;
      if(pHeaderC)
         delete []pHeaderC;
      return;
   }
   
   
   if( uncompress(pHeader, &dwHeader, pHeaderC, dwHeaderC) != Z_OK )
   {
      if(pHeader)
         delete []pHeader;
      if(pHeaderC)
         delete []pHeaderC;
      return;
   }
   
   unsigned char *pHeaderOTmp = pHeader;
   unsigned char *pHeaderNTmp = pHeader+(1000*4);
   
   for(int i=0;i<dwNbrImage;i++)
   {
      int dwOffset;
      memcpy(&dwOffset,pHeaderOTmp,4);
      
      sImagesDisplay newImage;
      sprintf_s(newImage.strName,255,"%s",pHeaderNTmp);
      newImage.iOffset = dwOffset;
      
      m_vImageDisplay.push_back(newImage);
      
      pHeaderNTmp+=50;
      pHeaderOTmp+=4;
   }
   
   int dwSize = m_vImageDisplay.size();
   
   if(pHeader)
      delete []pHeader;
   if(pHeaderC)
      delete []pHeaderC;
}


void V3_InvDlg::UseItemDirect(ItemUI *item,LPBAG_ITEM bagItemPtr)
{
	switch( bagItemPtr->chTarget )
	{
		// Target Self
		case TARGET_GROUP_SELF:
		case TARGET_NONGROUP_SELF:
		case TARGET_SELF:
		{
			char buf[ 512 ];
			FormatChargesLeft( buf, bagItemPtr );
			//V3_ChatLogDlg::GetInstance()->AddListLogs( "", buf, SYSTEM_COLOR,true);

			// Create the packet
			TFCPacket sending;
			sending << (RQ_SIZE)RQ_UseObject;
			sending << (short)0;	
			sending << (short)0;
			sending << (long)( bagItemPtr->dwID );

         if(item)
			   DropBack2( item,true);
			// send the use packet
			SEND_PACKET( sending );
			//V3_InvDlg::GetInstance()->Hide();
		}break;

		// Target Position
		case TARGET_GROUP_UNIT:
		case TARGET_UNIT_LIVING_PLAYER:
		case TARGET_UNIT_ANY_PLAYER:
		case TARGET_UNIT_PC: 
		case TARGET_UNIT_PC_NONSEL: 
		case TARGET_UNIT_ANY:
		case TARGET_UNIT_LIVING:
		case TARGET_UNIT_LIVING_NONSELF:
		case TARGET_UNIT_ANY_NONSELF: 
		case TARGET_GROUP_POSITION:
		case TARGET_NONGROUP_POSITION:
		case TARGET_POSITION:
		case TARGET_NONGROUP_UNIT:
		case TARGET_UNIT_OBJECT:
		case TARGET_UNIT_NPC: 
		case TARGET_UNIT_LIVING_MONSTER: 
		case TARGET_UNIT_LIVING_MONSTER_NONSELF: 
		{
         switch( bagItemPtr->chTarget )
         {
            case TARGET_GROUP_UNIT:
            case TARGET_UNIT_LIVING_PLAYER:
            case TARGET_UNIT_ANY_PLAYER:
            case TARGET_UNIT_PC: 
            case TARGET_UNIT_PC_NONSEL: 
               Objects.Lock(98);
               Objects.SetGridMode( playerPriority );
               Objects.Unlock(98);
            break;
            case TARGET_UNIT_ANY:
            case TARGET_UNIT_LIVING:
            case TARGET_UNIT_LIVING_NONSELF:
            case TARGET_UNIT_ANY_NONSELF: 
               Objects.Lock(99);
               Objects.SetGridMode( equalPriority );
               Objects.Unlock(99);
            break;
         }
			// Spell cast using a world position.			
			ItemCallbackData *mouseData = new ItemCallbackData( bagItemPtr->dwID, bagItemPtr );
 
			// Set a lock on the mouse
			if( !mouse.LockNextEvent( DM_CLICK, mouseData, UseItemOnTarget, 0 ) )
			{
				// Could not lock mouse.
				V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 444 ], SYSTEM_COLOR, true);
				delete mouseData;
			}
			else
			{
				CMouseCursor::GetInstance()->SetCustomCursor( &V3CastCursor, 0, 0, true );
			}
         if(item)
         {
			   DropBack2( item,false);
            if( IsShown() )
            {
               Hide();
            }
         }
		}break;
		default:
         if(item)
			   DropBack2( item,true);
		break;
	}
}

UINT V3_InvDlg::UseItemOnTarget(LPVOID rawData)
{

   CMouseCursor::GetInstance()->SetCustomCursor( NULL );
   POINT mousePos;
   ItemCallbackData *mouseData = reinterpret_cast< ItemCallbackData * >( rawData );

   TFCPacket sending;
   sending << (RQ_SIZE)RQ_UseObject2;

   // Get the mouse pos.
   MouseDevice md;
   md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );

   // Get the world pos.
   Player.ScreenPosToWL( &mousePos, &mousePos );

   bool bSendBuf = false;
   char buf[ 512 ];
   bool bSetID = false;
   signed char xPos, yPos;
   Objects.Lock(100);
   if (TargetID == Player.ID) 
   {
      sending << (short)( Player.xPos );
      sending << (short)( Player.yPos );
      sending << (long)( mouseData->dwItemID);
      sending << (long)TargetID;
      sending << (char)mouseData->bagItemPtr->chAttack;
      sending << (char)mouseData->bagItemPtr->chPVP;

      SEND_PACKET( sending );
      bSendBuf = true;
      FormatChargesLeft( buf, mouseData->bagItemPtr );
   }
   else if (Objects.FoundValidID( &xPos, &yPos, TargetID )) 
   {
      sending << (short)( xPos+Player.xPos );
      sending << (short)( yPos+Player.yPos );
      sending << (long)( mouseData->dwItemID);
      sending << (long)TargetID;
      sending << (char)mouseData->bagItemPtr->chAttack;
      sending << (char)mouseData->bagItemPtr->chPVP;
      SelectedID = TargetID;
      bSetID = true;

      SEND_PACKET( sending );

      bSendBuf = true;
      FormatChargesLeft( buf, mouseData->bagItemPtr );
   }
   else 
   {
      //cancel on fait rien... pas de cible
   }
   Objects.Unlock(100);

   //if(bSendBuf)
   //   V3_ChatLogDlg::GetInstance()->AddListLogs( "", buf, SYSTEM_COLOR,true);
   delete mouseData;



   return 0;
}