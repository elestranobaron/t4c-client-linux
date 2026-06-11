// V3_VolDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_VolDlg.h"
#include "V3_ChatLogDlg.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;


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
    };
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_VolDlg::V3_VolDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
itemGrid( itemGridEvent ),
robButton( robButtonEvent,g_GUILocalString[82],g_DefColorH )
{
   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastVolXPos();
   m_iYPos = g_Global.GetDialogLastVolYPos();
   
   SetHelpId( this, 285 );
  
   m_BackgroundS.SetSpriteId("V3_VolBack",true,"V3_VolBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_VolIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
   
   // 248, 116    x148 y176
   itemGrid.SetGrid( 17, 4, FramePos( 0, 0, 26, 26 ), FramePos( 0, 0, 3, 3 ) );
   itemGrid.SetScrollUpDownUI( &gridScrollUpGraph );
   itemGrid.SetScrollDownDownUI( &gridScrollDownGraph );
   gridSelectedTile.SetSpriteId( "GUI_BackInvGridSelect" );
   itemGrid.SetSelectedTileUI( &gridSelectedTile );
   SetHelpId( itemGrid, 522 );
   itemGridEvent.SetUI( this );
    
   instantItemInfo.SetCenter( false );
   instantItemInfo.SetText( "" );

   targetName     .SetCenter( false );
   targetName.SetText( "" );
   
   robButton.SetSpriteId   ( "V3_Btn72x24_N" );
   robButton.SetDownUI     ( "V3_Btn72x24_D" );
   robButton.SetHighlightUI( "V3_Btn72x24_H" );
   SetHelpId( &robButton, 518 );
   robButtonEvent.SetUI( this );
  
   UpdateAllFramePos(m_iXPos,m_iYPos);
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &itemGrid);
   AddChild( &targetName);
   AddChild( &robButton);
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );


} 

V3_VolDlg::~V3_VolDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_VolDlg *V3_VolDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_VolDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_VolDlg::Display( void *null)
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
      GetInstance()->m_iXPos = g_Global.GetDialogLastVolXPos();
      GetInstance()->m_iYPos = g_Global.GetDialogLastVolYPos();
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      GetInstance()->Show();
   }
   return 0;
}

void V3_VolDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_VolDlgW, iPosY+g_VolDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_VolDlgW, iPosY+g_VolDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   itemGrid       .SetGridDef(iPosX,iPosY,20,46 ,72,566,186,&gridScrollUpGraph,&gridScrollDownGraph);
   instantItemInfo.SetPos( FramePos( iPosX+218, iPosY+199, iPosX+449, iPosY+223) );
   targetName     .SetPos( FramePos( iPosX+57 , iPosY+199, iPosX+203, iPosY+223) );
   robButton      .SetPos(FramePos( iPosX+461, iPosY+199, iPosX+461+72, iPosY+199+24) );

}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_VolDlg::Draw(V2SPRITEFX *vsfFX)
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
      
      if(iNewPosX+g_VolDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_VolDlgW;
      if(iNewPosY+g_VolDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_VolDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   GameUI::Draw( vsfFX );

   // Get the current mouse coordinates.
   MousePos mousePos( MouseDevice::xPos, MouseDevice::yPos );
   
   ItemUI * itemUI = NULL;
   if( itemGrid.GetPos().MousePosInFrame( mousePos ) )
   {
      itemUI = static_cast< ItemUI * >( itemGrid.GetGridItemAt( mousePos ) );
   }
   
   // Display the instant help text.
   if( itemUI != NULL )
   {
      TextObject *to = itemUI->GetHelpText();
      
      if( to != NULL )
      {
         instantItemInfo.SetText(to->GetText(),g_DefColorD,234);
         instantItemInfo.Draw( vsfFX );
      }
   }
   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   robButton.DrawTextButtonExt(hdc);
   
   // Draw les texte...

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[342],strlen(g_GUILocalString[342]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}

void V3_VolDlg::Hide()
{
   itemGrid.DeleteGrid();
   BoxUI::Hide();
   
   // If the player had selected an item
   GameUI *itemUI = GetDragItem();
   if( itemUI != NULL )
   {
      // Reset drag.
      CancelDrag();
      CMouseCursor::GetInstance()->SetCustomCursor( NULL );
      // Delete it.
      delete itemUI;
   }
}





void V3_VolDlg::StartDragDlg( MousePos mousePos )
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

void V3_VolDlg::StopDragDlg()
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
      
      if(m_iXPos+g_VolDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_VolDlgW;
      if(m_iYPos+g_VolDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_VolDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastVolXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_VolDlg::DragEvent::LeftClicked( void )
{
   V3_VolDlg *me = static_cast< V3_VolDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_VolDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_VolDlg *me = static_cast< V3_VolDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}



//  Sets the user in rob mode.
void V3_VolDlg::Rob(bool canRob,string targetName,list< V3_VolDlg::Item > &itemList)
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   // If there is already a box in foreground control.
   if( IsForegroundControlled() )
   {
      return;
   }
   
   
   list< Item >::iterator i;
   // Destroy the previous rob.
   itemGrid.DeleteGrid();
   // Create an item UI for all items in the target's backpack.
   for( i = itemList.begin(); i != itemList.end(); i++ )
   {
      ItemUI *itemUI = new ItemUI;
      itemUI->itemData = *i;
      itemUI->SetVSF( InvItemIcons( (*i).appearance ) );
      itemUI->SetPos( FramePos( 0, 0, itemUI->GetVSF()->GetWidth(), itemUI->GetVSF()->GetHeight() ) );
      itemUI->SetHelpText( FontManager::GetInstance()->GetSmallFont((*i).itemName.c_str() ) );
      itemGrid.AddItem( itemUI );
   }
   
   V3_VolDlg::targetName.SetText( targetName.c_str(),g_DefColorD );
   
   if( canRob )
   {
      robButton.Show();
   }
   else
   {
      robButton.Hide();
   }
   
   V3_VolDlg::canRob = canRob;
   
   
   // Put this window in foreground control.
   Display(NULL);
   RequestForegroundControl( this );
}

//  Prematurly abort the robbing
void V3_VolDlg::DispellRob( void )
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   Hide();
}

//  Robs the current drag item.
void V3_VolDlg::Rob( void )
{
   ItemUI *itemUI = static_cast< ItemUI * >( GetDragItem() );   
   
   if( itemUI == NULL )
   {
      naSound->Play( TRUE );
      V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 516 ],SYSTEM_COLOR,true);
      return;
   }
   
   TFCPacket sending;
   sending << (RQ_SIZE)RQ_Rob;
   sending << (long)itemUI->itemData.id;
   SEND_PACKET( sending );
   Hide();
}

//  Double-clicked the item grid.
void V3_VolDlg::ItemGridEvent::LeftDblClicked( void )
{
   V3_VolDlg *me = static_cast< V3_VolDlg * >( GetUI() );
   if( me->canRob )
   {
      static_cast< V3_VolDlg * >( GetUI() )->Rob();
   }
}

//  Clicked the rob button.
void V3_VolDlg::RobButtonEvent::LeftClicked( void )
{
   static_cast< V3_VolDlg * >( GetUI() )->Rob();
}

//  Dropping something on the item grid.
//  Return: bool, true if the item is an ItemUI
bool V3_VolDlg::ItemGridEvent::Dropping( void )
{
   if( GetDragItem() == NULL )
   {
      return false;
   }
   if( typeid( *GetDragItem() ) == typeid( V3_VolDlg::ItemUI ) )
   {
      return true;
   }
   return false;
}
