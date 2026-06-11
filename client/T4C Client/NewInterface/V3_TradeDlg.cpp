// V3_TradeDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "..\Bitmap.h"
#include "V3_TradeDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_AskValueSldDlg.h"
#include "V3_ConfirmDlg.h"
#include "V3_ChatDlg.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;

extern Bitmap GridID;

using namespace std;
DWORD g_dwLastMacroCallTrade = 0;

namespace
{
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
            }
            else
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
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_TradeDlg::V3_TradeDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
junkButton( &(V3_InvDlg::GetInstance()->junkButton) ),
dropButton( &(V3_InvDlg::GetInstance()->dropButton) ),
inventoryGrid( &(V3_InvDlg::GetInstance()->inventoryGrid) ),
invGridScrollDownGraph( &(V3_InvDlg::GetInstance()->gridScrollDownGraph) ),
invGridScrollUpGraph( &(V3_InvDlg::GetInstance()->gridScrollUpGraph) ),
iconGold( &(V3_InvDlg::GetInstance()->iconGold) ),
statGold( &(V3_InvDlg::GetInstance()->statGold) ),
instantItemInfo( &(V3_InvDlg::GetInstance()->instantItemInfo) ),
tradeMyGrid( tradeGridEvent ),
tradeOtherGrid( EmptyEvent::GetInstance() ),
inviteButton( inviteButtonEvent,g_GUILocalString[116],g_DefColorH ),
cancelButton( cancelButtonEvent ),
mReadyButtonON( readyButtonEvent ),
mReadyButtonOFF( readyButtonEvent ),
mConfirmButtonON( confirmButtonEvent ),
mConfirmButtonOFF( confirmButtonEvent ),
disabledConfirmButton( EmptyEvent::GetInstance() ),
clearButton( clearButtonEvent ),
mOtherReadyButton( EmptyEvent::GetInstance() ),
mOtherConfirmButton( EmptyEvent::GetInstance() )
{
   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_TradeBack",true,"V3_TradeBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_MarchandIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
   
  
   gridSelectedTile.SetSpriteId( "GUI_BackInvGridSelect" );
   
   // Setup my grid
   tradeMyGrid.SetGrid( 8, 4, FramePos( 0, 0, 26, 26 ), FramePos( 0, 0, 3, 3 ) );
   tradeMyGrid.SetScrollUpDownUI( &tradeMyGridScrollUpGraph );
   tradeMyGrid.SetScrollDownDownUI( &tradeMyGridScrollDownGraph );
   tradeMyGrid.SetSelectedTileUI( &gridSelectedTile );
   SetHelpId(tradeMyGrid, 569);
   
   // Setup other party's grid
   tradeOtherGrid.SetGrid( 8, 4, FramePos( 0, 0, 26, 26 ), FramePos( 0, 0, 3, 3 ) );
   tradeOtherGrid.SetScrollUpDownUI( &tradeOtherGridScrollUpGraph );
   tradeOtherGrid.SetScrollDownDownUI( &tradeOtherGridScrollDownGraph );
   tradeOtherGrid.AllowDrag( false );
	SetHelpId(tradeOtherGrid, 570);

     
   
   inviteButton.SetSpriteId   ( "V3_Btn72x24_N" );     
   inviteButton.SetDownUI     ( "V3_Btn72x24_D" );
   inviteButton.SetHighlightUI( "V3_Btn72x24_H" );
   inviteButtonEvent.SetUI( this );
   SetHelpId( inviteButton, 566 );     
   
   cancelButton.SetSpriteId   ( "V3_BtnXX_N" );     
   cancelButton.SetDownUI     ( "V3_BtnXX_D" );
   cancelButton.SetHighlightUI( "V3_BtnXX_H" );
   cancelButtonEvent.SetUI( this );
	SetHelpId( cancelButton, 565 );

   // Setup the different control positions.
   
   mReadyButtonON.SetSpriteId   ( "V3_BtnChk_N" );
   mReadyButtonON.SetDownUI     ( "V3_BtnChk_D" );
   mReadyButtonON.SetHighlightUI( "V3_BtnChk_H" );
   SetHelpId( mReadyButtonON, 562 );
   mReadyButtonOFF.SetSpriteId   ( "V3_BtnDblArrow_N" );
   mReadyButtonOFF.SetDownUI     ( "V3_BtnDblArrow_D" );
   mReadyButtonOFF.SetHighlightUI( "V3_BtnDblArrow_H" );
   SetHelpId( mReadyButtonOFF, 562 ); 
   readyButtonEvent.SetUI( this );
   
   mConfirmButtonON.SetSpriteId   ( "V3_BtnChk_N" );
   mConfirmButtonON.SetDownUI     ( "V3_BtnChk_D" );
   mConfirmButtonON.SetHighlightUI( "V3_BtnChk_H" );
   SetHelpId( mConfirmButtonON, 563 );     
   mConfirmButtonOFF.SetSpriteId   ( "V3_BtnDblArrow_N" );
   mConfirmButtonOFF.SetDownUI     ( "V3_BtnDblArrow_D" );
   mConfirmButtonOFF.SetHighlightUI( "V3_BtnDblArrow_H" );
   SetHelpId( mConfirmButtonOFF, 563 );     
   confirmButtonEvent.SetUI( this );
   disabledConfirmButton.SetSpriteId( "V3_BtnD_N" );
   disabledConfirmButton.EnableSound(false);
   SetHelpId( disabledConfirmButton, 563 );

   clearButton.SetSpriteId   ( "V3_BtnBArrow_N" );
   clearButton.SetDownUI     ( "V3_BtnBArrow_D" );
   clearButton.SetHighlightUI( "V3_BtnBArrow_H" );
   clearButtonEvent.SetUI( this );
   SetHelpId( clearButton, 564 ); 
   
   mOtherReadyButton.SetSpriteId( "V3_BtnD_N" );
   mOtherReadyButton.EnableSound(false);
   SetHelpId( mOtherReadyButton, 567 );
   
   mOtherConfirmButton.SetSpriteId( "V3_BtnD_N" );
   mOtherConfirmButton.EnableSound(false);
	SetHelpId( mOtherConfirmButton, 568 );

   
   m_stMyInfo   .SetText("",g_DefColorH);
   m_stOtherInfo.SetText("",g_DefColorH);
   
   
  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &tradeMyGrid);
   AddChild( &tradeOtherGrid);
   AddChild( &inviteButton);
   AddChild( &cancelButton);
   AddChild( &mReadyButtonON);
   AddChild( &mReadyButtonOFF);
   AddChild( &mConfirmButtonON);
   AddChild( &mConfirmButtonOFF);
   AddChild( &disabledConfirmButton);
   AddChild( &clearButton);
   AddChild( &mOtherReadyButton);
   AddChild( &mOtherConfirmButton);

   AddChild( &mReadyButtonON);
   AddChild( &mReadyButtonOFF);
   AddChild( &mConfirmButtonON);
   AddChild( &mConfirmButtonOFF);
   AddChild( &disabledConfirmButton);
   AddChild( &clearButton);
   AddChild( &mOtherReadyButton);
   AddChild( &mOtherConfirmButton);
   
   AddChild( &m_stMyInfo);
   AddChild( &m_stOtherInfo);

   AddChild( inventoryGrid );
   AddChild( junkButton);
   AddChild( dropButton);
   AddChild( iconGold );
	AddChild( statGold );
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   mMyStatus = mOtherStatus = CS_EDITINGITEMS;

   trading = FALSE;
	UpdateInterfaceStatus();


   
} 

V3_TradeDlg::~V3_TradeDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_TradeDlg *V3_TradeDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_TradeDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_TradeDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallTrade > 500)
      {
         g_dwLastMacroCallTrade =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}

unsigned int V3_TradeDlg::Display( void *null)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->UpdateInterfaceStatus();
   GetInstance()->Show();
   return 0;
}

void V3_TradeDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   tradeMyGrid   .SetGridDef(iPosX,iPosY,20,36 ,119,292,234,&tradeMyGridScrollUpGraph,&tradeOtherGridScrollUpGraph);
   tradeOtherGrid.SetGridDef(iPosX,iPosY,20,36 ,275,292,390,&tradeMyGridScrollUpGraph,&tradeOtherGridScrollDownGraph);

   inviteButton.SetPos( FramePos( iPosX+267, iPosY+407, iPosX+267+72, iPosY+407+24 ) );
   cancelButton.SetPos ( FramePos(iPosX+269, iPosY+86 , iPosX+269+24, iPosY+86 +24 ) );

   mReadyButtonON .SetPos ( FramePos(iPosX+182, iPosY+86 , iPosX+182+24, iPosY+86 +24 ) );
   mReadyButtonOFF.SetPos ( FramePos(iPosX+182, iPosY+86 , iPosX+182+24, iPosY+86 +24 ) );

   mConfirmButtonON     .SetPos ( FramePos(iPosX+211, iPosY+86 , iPosX+211+24, iPosY+86 +24 ) );
   mConfirmButtonOFF    .SetPos ( FramePos(iPosX+211, iPosY+86 , iPosX+211+24, iPosY+86 +24 ) );
   disabledConfirmButton.SetPos ( FramePos(iPosX+211, iPosY+86 , iPosX+211+24, iPosY+86 +24 ) );

   clearButton .SetPos ( FramePos(iPosX+240, iPosY+86 , iPosX+240+24, iPosY+86 +24 ) );

   mOtherReadyButton  .SetPos ( FramePos(iPosX+182, iPosY+242 , iPosX+182+24, iPosY+242 +24 ) );
   mOtherConfirmButton.SetPos ( FramePos(iPosX+211, iPosY+242 , iPosX+211+24, iPosY+242 +24 ) );

   statGold->SetPos ( FramePos( iPosX+448, iPosY+367 , iPosX+548, iPosY+387) );
   iconGold->SetPos ( FramePos( iPosX+543, iPosY+366 , iPosX+567, iPosY+388) );
   instantItemInfo->SetPos( FramePos( iPosX+306, iPosY+93, iPosX+540, iPosY+118 ) );
   instantItemInfo->SetCenter( false );
   inventoryGrid  ->SetGridDef(iPosX,iPosY,20,309 ,127,567,358,&(V3_InvDlg::GetInstance()->gridScrollUpGraph) ,&(V3_InvDlg::GetInstance()->gridScrollDownGraph) );
   junkButton ->SetPos( FramePos( iPosX+309, iPosY+367, iPosX+309+22, iPosY+367+22 ) );
   dropButton ->SetPos( FramePos( iPosX+352, iPosY+367, iPosX+352+22, iPosY+367+22 ) );

   m_stMyInfo   .SetPos( FramePos( iPosX+32, iPosY+87 , iPosX+177, iPosY+108) );
   m_stOtherInfo.SetPos( FramePos( iPosX+32, iPosY+243, iPosX+177, iPosY+264) );

}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_TradeDlg::Draw(V2SPRITEFX *vsfFX)
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

   inviteButton.DrawTextButtonExt(hdc);
   
   // Draw les texte...

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[25],strlen(g_GUILocalString[25]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);

   // Get the current mouse coordinates.
   MousePos mousePos( MouseDevice::xPos, MouseDevice::yPos );
   
   ItemUI * itemUI = NULL;
   ePlacesToSearch whereToSearch = PL_SEARCHBACKPACK;
   if( inventoryGrid->GetPos().MousePosInFrame( mousePos ) )
   {
      itemUI = static_cast< ItemUI * >( inventoryGrid->GetGridItemAt( mousePos ) );
      whereToSearch = PL_SEARCHBACKPACK;
   } 
   else if( tradeMyGrid.GetPos().MousePosInFrame( mousePos ) ) 
   {
      itemUI = static_cast< ItemUI * >( tradeMyGrid.GetGridItemAt( mousePos ) );
      whereToSearch = PL_SEARCHMYTRADE;
   } 
   else if( tradeOtherGrid.GetPos().MousePosInFrame( mousePos ) ) 
   {
      itemUI = static_cast< ItemUI * >( tradeOtherGrid.GetGridItemAt( mousePos ) );
      whereToSearch = PL_SEARCHOTHERTRADE;
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
         sending << (char)whereToSearch;
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
            //instantItemInfo->SetText(to->GetText(),CL_ORANGE,246);
            instantItemInfo->SetText(itemUI->strName2,CL_ORANGE,246);
            instantItemInfo->Draw( vsfFX );
         }
      }        
   }
}

void V3_TradeDlg::Hide()
{
   if(trading)
   {
      V3_TradeDlg::RequestTradeCancel();
   }
   ResetTrade();
   BoxUI::Hide();
}





void V3_TradeDlg::StartDragDlg( MousePos mousePos )
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

void V3_TradeDlg::StopDragDlg()
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
void V3_TradeDlg::DragEvent::LeftClicked( void )
{
   V3_TradeDlg *me = static_cast< V3_TradeDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_TradeDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_TradeDlg *me = static_cast< V3_TradeDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}




//  Dragged something from the trade
void V3_TradeDlg::TradeGridEvent::Dragged( void )
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

// Verify that the dropped item is ok.
// Return: bool, true if dropped item is ok, false otherwise.
bool V3_TradeDlg::TradeGridEvent::Dropping( void )
{
   
   // If the source event is the trade event.
   if( GetUI()->GetDragSourceEvent() == this )
   {
      return true;
   }
   else
   {        
      ItemUI *itemUI = static_cast< ItemUI * >( GetDragItem() );
      V3_TradeDlg *me = V3_TradeDlg::GetInstance();
      
      // Retreive the bag item ID.
      DWORD itemID = itemUI->bagItem->dwID;
      
      dropSound->Play( TRUE );

      me->m_pItemUIReq = itemUI;
      
      DropBack( GetDragItem() );
      
      if( itemUI->bagItem->dwQty == 1 )
      {
         me->AskValueReturnSld(true,g_AskFromBPToTrade,NULL,1);
         return false;
      }
      
      V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[38] ,itemUI->bagItem->dwQty,0);
      V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
      V3_AskValueSldDlg::GetInstance()->SetParent(me);
      V3_AskValueSldDlg::GetInstance()->SetDelay(500);
      V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
      V3_AskValueSldDlg::GetInstance()->Show(g_AskFromBPToTrade,0);
      
      //Avait un delais, 
      me->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );
   }
   return false;
}

// Called when an item was successfully dropped from the trade
void V3_TradeDlg::TradeGridEvent::Dropped ( EventVisitor *sourceEvent )
{
   return;
}

void V3_TradeDlg::InviteButtonEvent::LeftClicked() 
{
   V3_TradeDlg *me = static_cast< V3_TradeDlg * >( GetUI() );
   
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

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TradeDlg::CancelButtonEvent::LeftClicked() 
{
   V3_TradeDlg *me = static_cast< V3_TradeDlg * >( GetUI() );
   V3_TradeDlg::RequestTradeCancel();
   me->UpdateInterfaceStatus();
}

void V3_TradeDlg::ReadyButtonEvent::LeftClicked() 
{
   int myStatus;
   myStatus = V3_TradeDlg::GetInstance()->mMyStatus;
   if (myStatus == CS_READY || myStatus == CS_CONFIRMED) 
   {
      V3_TradeDlg::RequestTradeSetStatus(CS_EDITINGITEMS);
   }
   else 
   {
      V3_TradeDlg::RequestTradeSetStatus(CS_READY);
   }
}

void V3_TradeDlg::ConfirmButtonEvent::LeftClicked() 
{
   int myStatus;
   myStatus = V3_TradeDlg::GetInstance()->mMyStatus;
   if (myStatus == CS_READY) 
   {
      V3_TradeDlg::RequestTradeSetStatus(CS_CONFIRMED);
   } 
   else if (myStatus == CS_CONFIRMED) 
   {
      V3_TradeDlg::RequestTradeSetStatus(CS_READY);
   }
}

void V3_TradeDlg::ClearButtonEvent::LeftClicked() 
{
   V3_TradeDlg::RequestTradeClear();
}

void V3_TradeDlg::EventDroppingItemUIFromTradeToBackpack()
{
   GameUI *dragItem = GetDragItem();
   ItemUI *iuiItem = (ItemUI*)dragItem;
   V3_TradeDlg *me = V3_TradeDlg::GetInstance();

   me->m_pItemUIReq = iuiItem;
   
   DropBack( dragItem );
   
   if( iuiItem->bagItem->dwQty == 1 )
   {
      me->AskValueReturnSld(true,g_AskFromTradeToBP,NULL,1);
      return ;
   }
   
   V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[38] ,iuiItem->bagItem->dwQty,0);
   V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueSldDlg::GetInstance()->SetParent(me);
   V3_AskValueSldDlg::GetInstance()->SetDelay(500);
   V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueSldDlg::GetInstance()->Show(g_AskFromTradeToBP,0);
   
   me->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TradeDlg::EventPlayerInvited(char *pCharacterName, DWORD pInvitorID, short pX, short pY)
{
   if (pCharacterName == NULL) 
      return;

   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   // If there is already a box in foreground control.
   if( IsForegroundControlled() )
   {
      // Decline.
   }
   else
   {    
      // If the user was ignored.
      if(V3_ChatDlg::GetInstance()->IsUserIgnored( pCharacterName )) //invite Trade
      {
         // Decline.
      }
      else
      {
         char buf[ 1024 ];
         sprintf_s( buf,1024, g_LocalString[ 573 ], pCharacterName );
         

         m_dwInvitorID = pInvitorID;
         m_dwInvitorPX = pX;
         m_dwInvitorPY = pY;

         
         V3_ConfirmDlg::GetInstance()->SetDialogText(buf);
         V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[10],g_GUILocalString[11],738,739);
         V3_ConfirmDlg::GetInstance()->SetParent(this);
         V3_ConfirmDlg::GetInstance()->SetXYPos(((g_Global.GetScreenW()-g_AskValueDlgW)/2), ((g_Global.GetScreenH()-g_AskValueDlgH)/2));
         V3_ConfirmDlg::GetInstance()->Show(0);
         
         RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
      }
      
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TradeDlg::EventTradeStarted(char *pCharacterName)
{
   if (pCharacterName == NULL)
   {
      m_stMyInfo   .SetText("",g_DefColorH);
      m_stOtherInfo.SetText("",g_DefColorH);
      return;
   }
   m_stMyInfo   .SetText(g_LocalString[ 683 ],g_DefColorH);
   m_stOtherInfo.SetText(pCharacterName,g_DefColorH);

   mMyStatus = mOtherStatus = CS_EDITINGITEMS;
   trading = true;
   Display(NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TradeDlg::EventTradeCanceled() 
{
   ResetTrade();
   Hide();
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TradeDlg::EventTradeFinished()
{
   ResetTrade();
   Hide();
}

void V3_TradeDlg::EventTradeStatusChanged(int pMyStatus, int pOtherStatus)
{
   // new status MUST be in a valid range!
   if (pMyStatus <= pOtherStatus && pOtherStatus < CS_EDITINGITEMS && pMyStatus >= pOtherStatus && pOtherStatus > CS_CONFIRMED) 
   {
      return;
   }
   mMyStatus = pMyStatus;
   mOtherStatus = pOtherStatus;
   UpdateInterfaceStatus();
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TradeDlg::UpdateTrade( TemplateList< BAG_ITEM > *newTrade, int pAffected )
{
   
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   TemplateList< BAG_ITEM > *tlTheTrade;
   GridUI *theGrid;
   ePlacesToSearch whereToSearch;
   
   if (pAffected == AC_MYSELF) 
   {
      tlTheTrade = &tlMyTrade;
      theGrid		= &tradeMyGrid;
      whereToSearch = PL_SEARCHMYTRADE;
   } 
   else if (pAffected == AC_OTHER) 
   {
      tlTheTrade = &tlOtherTrade;
      theGrid = &tradeOtherGrid;
      whereToSearch = PL_SEARCHOTHERTRADE;
   } else return;
   
   BOOL boDraggingItemComesFromTrade = FALSE;
   GameUI *draggingItem = GetDragItem(); // If there is an item being dragged, save it for comparition
   
   tlTheTrade->Lock( "V3_TradeDlg::UpdateTrade" );
   newTrade->Lock( "V3_TradeDlg::UpdateTrade" );
   
   // Removes the deleted items from the container list and update qty and charges for the others
   tlTheTrade->ToHead();
   while ( tlTheTrade->QueryNext() ) 
   {
      BOOL boCureentTradeItemIsStillOnTheList = FALSE;
      BAG_ITEM *bagItem = tlTheTrade->GetObject();
      
      newTrade->ToHead();
      while ( newTrade->QueryNext() ) 
      {
         BAG_ITEM *newBagItem = newTrade->GetObject();
         
         // If the item doesn't have an ID yet and an item of the same
         // type is found.
         if( bagItem->dwID == 0 && bagItem->associatedUI != NULL && bagItem->wBaseID == newBagItem->wBaseID )
         { 
            bagItem->chIName[0] = 0x00;
            // Use this new item's ID as the item's ID.
            bagItem->dwID = newBagItem->dwID;
         }
         
         if( newBagItem->dwID == bagItem->dwID )
         {
            // Update the bag item's item count
            bagItem->dwQty = newBagItem->dwQty;
            bagItem->iCharges = newBagItem->iCharges;
            bagItem->chTarget = newBagItem->chTarget;
            bagItem->chAttack = newBagItem->chAttack;
            bagItem->chPVP    = newBagItem->chPVP;

            // Update the associatedUI's name.
            if( bagItem->associatedUI != NULL )
            {
               static_cast< V3_InvDlg::ItemUI * >( bagItem->associatedUI)->UpdateName2();
            }
            
            boCureentTradeItemIsStillOnTheList = TRUE;
            break;
         }
      }
      
      if (boCureentTradeItemIsStillOnTheList == FALSE) 
      { // If the object is not on the pack anymore, purge it!
         theGrid->RemoveItem( bagItem->associatedUI ); // Remove its ItemUI from the grid.
         delete bagItem->associatedUI;					// Delete the ItemUI
         tlTheTrade->DeleteObject();							// Delete the bag_info data
      } 
      else 
      { // If the object is still on the list, keep it there and remove it from the newList. In the end, new list will have only new added items
         newTrade->DeleteObject();
         if ( draggingItem != NULL && draggingItem == bagItem->associatedUI ) 
         { // If this item is being dragged, allow client to keep dragging
            boDraggingItemComesFromTrade = TRUE;
         }
      }
   }
   
   // newTrade list now contains only the items that werent on the list before
   // So, now adds those new items to the trade list
   newTrade->ToHead();
   while ( newTrade->QueryNext() ) 
   {
      BAG_ITEM *newBagItem = newTrade->GetObject();
      
      tlTheTrade->AddToTail( newBagItem );
      
      // If a sprite was specified.
      if( newBagItem->lpSprite != NULL )
      {
         // Create a new GameUI.
         ItemUI *newItemUI = new ItemUI;
         
         // Associate both bag item and item UI.
         newItemUI->bagItem = newBagItem;
         newBagItem->associatedUI = newItemUI;
         
         newItemUI->Setup();
         
         // Add the UI to the inventory grid.
         theGrid->AddItem( newItemUI );
         
         // If the item does not have a name, request it! :)
         if (!newBagItem->chIName[0]) 
         {
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_QueryItemName;
            sending << (char)whereToSearch;
            sending << (long)newBagItem->dwID;
            SEND_PACKET( sending );
         }
      }
   }
   
   // If there is currently an item being dragged.
   if( draggingItem != NULL )
   {
      // Receiving an update of my own tradepack and im dragging an item from it
      if (GetDragSourceEvent() == &tradeGridEvent && pAffected == AC_MYSELF) 
      {
         
         // If the drag item could not be found in the trade list nor on the inventory.
         if( boDraggingItemComesFromTrade == FALSE )
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
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TradeDlg::UpdateItemsName(DWORD dwItemID,LPBYTE lpbItemName,ePlacesToSearch pWhereToLook)
{
   TemplateList<BAG_ITEM> *theTrade;
   if (pWhereToLook == PL_SEARCHMYTRADE) 
   {
      theTrade = &tlMyTrade;
   } 
   else if (pWhereToLook == PL_SEARCHOTHERTRADE) 
   {
      theTrade = &tlOtherTrade;
   } else return;
   
   theTrade->Lock("V3_TradeDlg::UpdateItemsName");
   theTrade->ToHead();
   while (theTrade->QueryNext()) 
   {
      LPBAG_ITEM lpBagItem = theTrade->GetObject();
      
      // Update each item with the same base ID.
      if( lpBagItem->dwID == dwItemID )
      {
         strcpy_s((char *)lpBagItem->chIName,512, (char *)lpbItemName);
         
         // If there is an associated UI.
         if( lpBagItem->associatedUI != NULL )
         {
            // Set its help text according to the UI's text.
            static_cast< V3_TradeDlg::ItemUI * >( lpBagItem->associatedUI )->UpdateName2();
         }
         break;
      }
   }
   theTrade->Unlock("V3_TradeDlg::UpdateItemsName");
}


// Update the interface buttons/grids according to the current status of the trade
void V3_TradeDlg::UpdateInterfaceStatus()
{
   if (trading == TRUE) 
   {
      if (mMyStatus == CS_EDITINGITEMS) 
      {
         mReadyButtonON.Hide();
         mConfirmButtonON.Hide();
         mConfirmButtonOFF.Hide();
         mReadyButtonOFF.Show();
         disabledConfirmButton.Show();
      } 
      else if (mMyStatus == CS_READY) 
      {
         mReadyButtonOFF.Hide();
         mReadyButtonON.Show();
         if (mOtherStatus == CS_READY || mOtherStatus == CS_CONFIRMED) 
         {
            mConfirmButtonON.Hide();
            disabledConfirmButton.Hide();
            mConfirmButtonOFF.Show();
         } 
         else 
         {
            mConfirmButtonON.Hide();
            mConfirmButtonOFF.Hide();
            disabledConfirmButton.Show();
         }
      } 
      else if (mMyStatus == CS_CONFIRMED) 
      {
         mReadyButtonOFF.Hide();
         mConfirmButtonOFF.Hide();
         disabledConfirmButton.Hide();
         mReadyButtonON.Show();
         mConfirmButtonON.Show();
      }

      if (mOtherStatus == CS_EDITINGITEMS) 
      {
         mOtherReadyButton.SetSpriteId("V3_BtnD_N");
         mOtherConfirmButton.SetSpriteId("V3_BtnD_N");
      } 
      else if (mOtherStatus == CS_READY) 
      {
         mOtherReadyButton.SetSpriteId("V3_BtnChk_N");
         mOtherConfirmButton.SetSpriteId("V3_BtnD_N");
      } 
      else if (mOtherStatus == CS_CONFIRMED) 
      {
         mOtherReadyButton.SetSpriteId("V3_BtnChk_N");
         mOtherConfirmButton.SetSpriteId("V3_BtnChk_N");
      }
      
      inviteButton.Hide();
      tradeMyGrid.Show();
      tradeOtherGrid.Show();
      cancelButton.Show();
      clearButton.Show();
      
      
      mOtherReadyButton.Hide(); // Hide first so it updates the sprite
      mOtherConfirmButton.Hide(); // Hide first so it updates the sprite
      mOtherReadyButton.Show();
      mOtherConfirmButton.Show();
   } 
   else 
   {
      tradeMyGrid.Hide();
      tradeOtherGrid.Hide();
      clearButton.Hide();
      mConfirmButtonON.Hide();
      mConfirmButtonOFF.Hide();
      mReadyButtonON.Hide();
      mReadyButtonOFF.Hide();
      cancelButton.Hide();
      mOtherReadyButton.Hide();
      mOtherConfirmButton.Hide();
      
      disabledConfirmButton.Hide();
      inviteButton.Show();
      
      m_stMyInfo   .SetText("",g_DefColorH);
      m_stOtherInfo.SetText("",g_DefColorH);
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TradeDlg::ResetTrade()
//////////////////////////////////////////////////////////////////////////////////////////
// Reset the trade status variables and update the interface
//////////////////////////////////////////////////////////////////////////////////////////
{
	trading = false;
	mMyStatus = mOtherStatus = CS_EDITINGITEMS;
	UpdateInterfaceStatus();
}

// When the player clicks the invite button, the trade interface closes and the player
// can click on some other player to ask him to trade.
// When he clicks, this function is called to make the invite to the clicked player
unsigned int V3_TradeDlg::InvitePlayer(void *data )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   V3_TradeDlg *me = reinterpret_cast< V3_TradeDlg * >( data );
   
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
      
      RequestTradeInvite(dwID, xPos + Player.xPos, yPos + Player.yPos);
   }
   
   me->Display(NULL);
   
   return 0;
}

// Send a packet to the server asking to trade with someone
void V3_TradeDlg::RequestTradeInvite( DWORD dwID, short xPos, short yPos )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_TradeInvite;
   packet << (long)dwID;
   packet << (short)xPos;
   packet << (short)yPos;
   SEND_PACKET( packet );
}

// Send a packet to the server asking to cancel the trade
void V3_TradeDlg::RequestTradeCancel()
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_TradeCancel;
   SEND_PACKET( packet );
}

// Send a packet to the server asking to change status
void V3_TradeDlg::RequestTradeSetStatus( int Status )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_TradeSetStatus;
   packet << (short)Status;
   SEND_PACKET( packet );
}

// Send a packet to the server asking to clear all items from the tradepack
void V3_TradeDlg::RequestTradeClear()
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_TradeClear;
   SEND_PACKET( packet );
}

// Send a packet to the server asking to move the item from backpack to trade
void V3_TradeDlg::RequestMoveItemFromBackpackToTrade( DWORD dwItemID, DWORD dwQty )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_TradeAddItemFromBackpack;
   packet << (long)dwItemID;
   packet << (long)dwQty;
   SEND_PACKET( packet );
}

// Send a packet to the server asking to move the item from trade to backpack
void V3_TradeDlg::RequestMoveItemFromTradeToBackpack( DWORD dwItemID, DWORD dwQty )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_TradeRemoveItemToBackpack;
   packet << (long)dwItemID;
   packet << (long)dwQty;
   SEND_PACKET( packet );
}

//////////////////////////////////////////////////////////////////////////////////////////



void V3_TradeDlg::AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal)
{
   if(!bOK || iSldVal == 0)
      return;
   
   DWORD dwQty = iSldVal;
   
   switch(uiShowCodeP)
   {
      case g_AskFromBPToTrade:
      {
         V3_TradeDlg::GetInstance()->RequestMoveItemFromBackpackToTrade(m_pItemUIReq->bagItem->dwID, dwQty);
      }      
      break;
      case g_AskFromTradeToBP:
      {
         V3_TradeDlg::GetInstance()->RequestMoveItemFromTradeToBackpack(m_pItemUIReq->bagItem->dwID, dwQty);
      }      
      break;
      
     
   }
}

void V3_TradeDlg::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   switch(uiShowCodeP)
   {
      case 0:
      {
         if(bOK)
         {
           V3_TradeDlg::RequestTradeInvite(m_dwInvitorID, (short)m_dwInvitorPX, (short)m_dwInvitorPY);
         }
      }
      break;
   }
}