// V3_GuildChestDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include <set>


#include "V3_GuildChestDlg.h"
#include "V3_ChatLogDlg.h"

#include "V3_AskValueDlg.h"
#include "V3_AskValueSldDlg.h"

#include "V3_InvDlg.h"

extern DWORD TargetID;
extern CSaveGame g_SaveGame;


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
    bool DropBack2(GameUI *itemUI )
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
                srcGrid->DropItem(GameUI::GetDragInitialPos());//LastPos();
                GameUI::CancelDrag();
                CMouseCursor::GetInstance()->SetCustomCursor( NULL );
             }
             else
             {
                srcGrid->AddItem( itemUI );
                GameUI::CancelDrag();
                CMouseCursor::GetInstance()->SetCustomCursor( NULL );
             }
          }
          return true;
       }
       return false;
    };
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_GuildChestDlg::V3_GuildChestDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
chestGrid( chestGuildGridEvent ),
instantItemInfo       (&(V3_InvDlg::GetInstance()->instantItemInfo) ),
statGold              (&(V3_InvDlg::GetInstance()->statGold) ),
iconGold              (&(V3_InvDlg::GetInstance()->iconGold)  ),
inventoryGrid         (&(V3_InvDlg::GetInstance()->inventoryGrid) ),
invGridScrollDownGraph(&(V3_InvDlg::GetInstance()->gridScrollDownGraph) ),
invGridScrollUpGraph  (&(V3_InvDlg::GetInstance()->gridScrollUpGraph) ),
junkButton            (&(V3_InvDlg::GetInstance()->junkButton)),
dropButton            (&(V3_InvDlg::GetInstance()->dropButton)  )
{
   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogChestLastXPos();
   m_iYPos = g_Global.GetDialogChestLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_ChestBigBack",true,"V3_ChestBigBackMask");
   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_ChestIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   



   chestGrid.SetGrid( 12, 7, FramePos( 0, 0, 26, 26 ), FramePos( 0, 0, 3, 3 ) );
   chestGrid.AllowHelp(false);
   SetHelpId( chestGrid, 559 );
   chestGrid.SetScrollUpDownUI( &chestGridScrollUpGraph );
   chestGrid.SetScrollDownDownUI( &chestGridScrollDownGraph );
   gridSelectedTile.SetSpriteId( "GUI_BackInvGridSelect" );
   chestGrid.SetSelectedTileUI( &gridSelectedTile );



     
 

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &chestGrid );   

   AddChild( inventoryGrid );
   AddChild( junkButton);
   AddChild( dropButton);
   AddChild( iconGold );
	AddChild( statGold );

   AddChild( &Drag_Button);

   LoadChest();
  
   RootBoxUI::GetInstance()->AddChild( this );


} 

V3_GuildChestDlg::~V3_GuildChestDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_GuildChestDlg *V3_GuildChestDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_GuildChestDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_GuildChestDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   // If inventory is already shown.
   if( GetInstance()->IsShown())
   {
      // Hide box
      GetInstance()->Hide();
   }
   else
   {
      GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
      GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      TFCPacket sending;
      sending << (RQ_SIZE)RQ_ViewBackpack2;
      SEND_PACKET( sending );

      GetInstance()->Show();
   }
   return 0;
}

void V3_GuildChestDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_ChestDlgW, iPosY+g_ChestDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_ChestDlgW, iPosY+g_ChestDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+727, iPosY+27, iPosX+727+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+693, iPosY+57 ) );

   chestGrid.SetGridDef(iPosX,iPosY,20,64 ,147,440,349,&chestGridScrollUpGraph,&chestGridScrollDownGraph);
  
   statGold->SetPos ( FramePos( iPosX+621, iPosY+367 , iPosX+721, iPosY+387) );
   iconGold->SetPos ( FramePos( iPosX+716, iPosY+366 , iPosX+740, iPosY+388) );
   instantItemInfo->SetPos( FramePos( iPosX+479, iPosY+93, iPosX+713, iPosY+118 ) );
   instantItemInfo->SetCenter( false );

   inventoryGrid  ->SetGridDef(iPosX,iPosY,20,482 ,127,740,358,&(V3_InvDlg::GetInstance()->gridScrollUpGraph) ,&(V3_InvDlg::GetInstance()->gridScrollDownGraph) );
   junkButton ->SetPos( FramePos( iPosX+482, iPosY+367, iPosX+482+22, iPosY+367+22 ) );
   dropButton ->SetPos( FramePos( iPosX+525, iPosY+367, iPosX+525+22, iPosY+367+22 ) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_GuildChestDlg::Draw(V2SPRITEFX *vsfFX)
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
      
      if(iNewPosX+g_ChestDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_ChestDlgW;
      if(iNewPosY+g_ChestDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_ChestDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   GameUI::Draw( vsfFX );

   

   HDC hdc;
   DXDGetDC(&hdc, 86);
   
   // Draw les texte...

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[264],strlen(g_GUILocalString[264]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);


   // Get the current mouse coordinates.
   MousePos mousePos( MouseDevice::xPos, MouseDevice::yPos );
   
   ItemUI * itemUI = NULL;
   if( inventoryGrid->GetPos().MousePosInFrame( mousePos ) )
      itemUI = static_cast< ItemUI * >( inventoryGrid->GetGridItemAt( mousePos ) );
   else if( chestGrid.GetPos().MousePosInFrame( mousePos ) ) 
      itemUI = static_cast< ItemUI * >( chestGrid.GetGridItemAt( mousePos ) );
   
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
         sending << (char)PL_SEARCHGUILDCHEST;
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

void V3_GuildChestDlg::Hide()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   BoxUI::Hide();
}




void V3_GuildChestDlg::StartDragDlg( MousePos mousePos )
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

void V3_GuildChestDlg::StopDragDlg()
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
      
      if(m_iXPos+g_ChestDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_ChestDlgW;
      if(m_iYPos+g_ChestDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_ChestDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogChestLastXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_GuildChestDlg::DragEvent::LeftClicked( void )
{
   V3_GuildChestDlg *me = static_cast< V3_GuildChestDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_GuildChestDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_GuildChestDlg *me = static_cast< V3_GuildChestDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}

void V3_GuildChestDlg::AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal)
{
   if(!bOK || iSldVal == 0)
      return;

   DWORD dwQty = iSldVal;

   switch(uiShowCodeP)
   {
      case g_AskChestToBackpackGuild:
      {
         RequestMoveItemFromChestToBackpack(m_pItemUIReq->bagItem->dwID, dwQty);
      }      
      break;
      case g_AskBackpackToChestGuild:
      {
         RequestMoveItemFromBackpackToChest(m_pItemUIReq->bagItem->dwID, dwQty);
      }      
      break;
   }
}


// Updates the chest
void V3_GuildChestDlg::UpdateChest( TemplateList< BAG_ITEM > *newChest )
{

   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   BOOL boDraggingItemComesFromChest = FALSE;
   GameUI *draggingItem = GetDragItem(); // If there is an item being dragged, save it for comparition

   tlChest.Lock( "V3_GuildChestDlg::UpdateChest" );
   newChest->Lock( "V3_GuildChestDlg::UpdateChest" );

   // Removes the deleted items from the chest list and update qty and charges for the others
   tlChest.ToHead();
   while ( tlChest.QueryNext() ) 
   {
      BOOL boCureentChestItemIsStillOnTheList = FALSE;
      BAG_ITEM *bagItem = tlChest.GetObject();

      newChest->ToHead();
      while ( newChest->QueryNext() ) 
      {
         BAG_ITEM *newBagItem = newChest->GetObject();

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
               static_cast< V3_InvDlg::ItemUI * >( bagItem->associatedUI)->UpdateName2();

            boCureentChestItemIsStillOnTheList = TRUE;
            break;
         }
      }

      if (boCureentChestItemIsStillOnTheList == FALSE) 
      { // If the object is not on the chest anymore, purge it!
         chestGrid.RemoveItem( bagItem->associatedUI ); // Remove its ItemUI from the grid.
         delete bagItem->associatedUI;					// Delete the ItemUI
         tlChest.DeleteObject();							// Delete the bag_info data
      } 
      else 
      { // If the object is still on the list, keep it there and remove it from the newList. In the end, new list will have only new added items
         newChest->DeleteObject();
         // If the item being dragged comes from chest, allow client to keep dragging
         if ( draggingItem != NULL && draggingItem == bagItem->associatedUI ) 
         { 
            boDraggingItemComesFromChest = TRUE;
         }
      }
   }


   // newChest list now contains only the items that werent on the list before
   // So, now adds those new items to the chest list
   newChest->ToHead();
   while ( newChest->QueryNext() ) 
   {
      BAG_ITEM *newBagItem = newChest->GetObject();
      // If a sprite was specified.
      if( newBagItem->lpSprite != NULL )
      {
         tlChest.AddToTail( newBagItem );
         // Create a new GameUI.
         ItemUI *newItemUI = new ItemUI;

         // Associate both bag item and item UI.
         newItemUI->bagItem = newBagItem;
         newBagItem->associatedUI = newItemUI;

         newItemUI->Setup();

         // Add the UI to the inventory grid.
         chestGrid.AddItem( newItemUI );

         // If the item does not have a name, request it! :)
         if (!newBagItem->chIName[0]) 
         {
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_QueryItemName;
            sending << (char)PL_SEARCHGUILDCHEST;
            sending << (long)newBagItem->dwID;
            SEND_PACKET( sending );
         }
      }
   }

   // If there is currently an item being dragged.
   if( draggingItem != NULL )
   {

      if (GetDragSourceEvent() == &chestGuildGridEvent) 
      {
         // Gets the vector with all inventory items to check if the dragged items is not from the inventory
         std::vector< GameUI * > vInventoryItemsList;
         inventoryGrid->GetGridItems(vInventoryItemsList);

         // If the drag item could not be found in the chest list nor on the inventory.
         if( boDraggingItemComesFromChest == FALSE
            && find( vInventoryItemsList.begin(), vInventoryItemsList.end(), GetDragItem() ) == vInventoryItemsList.end())
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

   tlChest.Unlock("V3_GuildChestDlg::UpdateChest");
   newChest->Unlock("V3_GuildChestDlg::UpdateChest");
}

void V3_GuildChestDlg::Reload()
{
   LoadChest();
   inventoryGrid          =  &(V3_InvDlg::GetInstance()->inventoryGrid) ;
   invGridScrollDownGraph =  &(V3_InvDlg::GetInstance()->gridScrollDownGraph) ;
   invGridScrollUpGraph   =  &(V3_InvDlg::GetInstance()->gridScrollUpGraph) ;
   iconGold               =  &(V3_InvDlg::GetInstance()->iconGold) ;
   statGold               =  &(V3_InvDlg::GetInstance()->statGold) ;
   instantItemInfo        =  &(V3_InvDlg::GetInstance()->instantItemInfo);
   
}

void V3_GuildChestDlg::ClearItem()
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   chestGrid.DeleteGrid();
   
   tlChest.Lock("reload");
   tlChest.ToHead();
   while( tlChest.QueryNext() )
   {
      tlChest.DeleteObject();
   }
   tlChest.Unlock("reload");
}

//  Setups the given item UI according to its binded BAG_ITEM data.
BOOL V3_GuildChestDlg::UpdateItemsName(DWORD dwItemID,LPBYTE lpbItemName)
{
   BOOL returnValue = FALSE;
   tlChest.Lock("V3_GuildChestDlg::UpdateItemsName");
   tlChest.ToHead();
   while (tlChest.QueryNext()) 
   {
      LPBAG_ITEM lpBagItem = tlChest.GetObject();
      
      // Update each item with the same base ID.
      if( lpBagItem->dwID == dwItemID )
      {
         strcpy_s((char *)lpBagItem->chIName,512, (char *)lpbItemName);
         
         // If there is an associated UI.
         if( lpBagItem->associatedUI != NULL )
         {
            // Set its help text according to the UI's text.
            static_cast< V3_GuildChestDlg::ItemUI * >( lpBagItem->associatedUI )->UpdateName2();
         }
         returnValue = TRUE;
         break;
      }
   }
   tlChest.Unlock("V3_GuildChestDlg::UpdateItemsName");
  	return returnValue;
}

// Virtual key inputs
bool V3_GuildChestDlg::VKeyInput(VKey vKey)
{
   if (vKey.GetKey() == VK_ESCAPE) 
      Hide();
   return true;
}

//  Setups the given item UI according to its binded BAG_ITEM data.
void V3_GuildChestDlg::RequestMoveItemFromChestToBackpack(DWORD dwItemID,DWORD dwQty)
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_NM_GUILDChestRemoveItem;
   packet << (long)dwItemID;
   packet << (long)dwQty;
   SEND_PACKET( packet );
}


//  Setups the given item UI according to its binded BAG_ITEM data.
void V3_GuildChestDlg::RequestMoveItemFromBackpackToChest(DWORD dwItemID,DWORD dwQty)
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_NM_GUILDChestAddItem;
   packet << (long)dwItemID;
   packet << (long)dwQty;
   SEND_PACKET( packet );
}

//  Saves the chest into the system's registry.
void V3_GuildChestDlg::GuildChestToSave( void )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   list< GridUI::GridItem > itemList;
   list< GridUI::GridItem >::iterator i;
   chestGrid.GetGridItemList( itemList );
   for( i = itemList.begin(); i != itemList.end(); i++ )
   {
      GridUI::GridItem &gridItem = *i;
      ItemUI *itemUI = static_cast< ItemUI * >( gridItem.ui );
      
      sInventory sNewChest;
      
      sNewChest.dwID         = itemUI->bagItem->wBaseID;
      sNewChest.dwX          = gridItem.gridPos.x;
      sNewChest.dwY          = gridItem.gridPos.y;
      sNewChest.dwType       = itemUI->bagItem->Type;
      g_SaveGame.AddChestG(sNewChest);
   }
}

//  Loads the chest from the system's registry.
void V3_GuildChestDlg::LoadChest( void )
{
   int dwNbrChest = g_SaveGame.GetNbrChestG();
   
   set< DWORD > addedIds;
   if(dwNbrChest > 0)
   {
      for(int i =0;i<dwNbrChest; i++ )
      {
         sInventory *pNewChest = g_SaveGame.GetChestG(i);
         
         GridUI::GridPos gridPos;
         gridPos.x = pNewChest->dwX;
         gridPos.y = pNewChest->dwY;
         
         
         // Create a new UI.
         ItemUI *itemUI = new ItemUI;
         
         LPBAG_ITEM bagItem = new BAG_ITEM;
         
         bagItem->associatedUI = itemUI;
         bagItem->wBaseID      = (WORD)pNewChest->dwID;
         bagItem->dwID         = 0; // Assign a null ID.
         bagItem->Type         = (WORD)pNewChest->dwType;
         bagItem->lpSprite     = InvItemIcons( pNewChest->dwType );
         bagItem->chIName[0] = 0;
         
         itemUI->bagItem = bagItem; // Associate the BAG_ITEM to the UI
         itemUI->Setup(); // Setup the item UI.
         
         // If the item ID was already added or the grid cannot accept the item.
         if( addedIds.find( bagItem->wBaseID ) != addedIds.end() || !chestGrid.AddGridItemAtPos( gridPos, itemUI ) )
         {
            // If the spot wasn't valid, junk this item UI.
            delete itemUI;
            delete bagItem;
         }
         else
         {
            addedIds.insert( bagItem->wBaseID );
            tlChest.AddToTail( bagItem);
         }
      }
   }
}




//  Dragged something from the chest
void V3_GuildChestDlg::ChestGuildGridEvent::Dragged( void )
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
bool V3_GuildChestDlg::ChestGuildGridEvent::Dropping( void )
{
   
   // If the source event is the chest event.
   if( GetUI()->GetDragSourceEvent() == this )
   {
      return true;
   }
   else
   {        
      V3_GuildChestDlg *me = V3_GuildChestDlg::GetInstance();


      ItemUI *itemUI = static_cast< ItemUI * >( GetDragItem() );
      
      // Retreive the bag item ID.
      DWORD itemID = itemUI->bagItem->dwID;
      dropSound->Play( TRUE );

      me->m_pItemUIReq = itemUI;

      DropBack( GetDragItem() );

      if( itemUI->bagItem->dwQty == 1 )
      {
         me->m_pItemUIReq = itemUI;   
         me->AskValueReturnSld(true,g_AskBackpackToChestGuild,NULL,1);
         return false;
      }

      V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[38] ,itemUI->bagItem->dwQty,0);
      V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
      V3_AskValueSldDlg::GetInstance()->SetParent(me);
      V3_AskValueSldDlg::GetInstance()->SetDelay(500);
      V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_ChestDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_ChestDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
      V3_AskValueSldDlg::GetInstance()->Show(g_AskBackpackToChestGuild,0);
      
      //Avait un delais, 
      me->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );
      
      
   }
   
   
   // Never let an item be dropped in the chest box, its handling will
   // be done by the packets.
   return false;
}

