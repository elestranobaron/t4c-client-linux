// V3_BuyDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_BuyDlg.h"
#include "V3_ChatLogDlg.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;


extern int TalkToX, TalkToY,TalkToW;
extern DWORD TalkToID;

namespace
{
    T3VSBSound *qtySound = NULL;
    T3VSBSound *naSound = NULL;
    
    void InitSound(){
        if( qtySound == NULL )
        {
            qtySound = new T3VSBSound;
            qtySound->Create( "Quantity Buttons", TS_MEMORY );

            naSound = new T3VSBSound;
            naSound->Create( "General NA sound", TS_MEMORY );
        }
    };

    const int ItemNameSize = 198;
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_BuyDlg::V3_BuyDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
m_ListItem( itemListEvent ),
m_btnBuy( buyButtonEvent,g_GUILocalString[70],g_DefColorH ),
m_btnSell( buyButtonEvent,g_GUILocalString[77],g_DefColorH ),
m_btnTrade( buyButtonEvent,g_GUILocalString[390],g_DefColorH )

{
   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_BuyBack",true,"V3_BuyBackMask");

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
   
   
   m_btnBuy.SetSpriteId   ( "V3_Btn92x24_N"   );
   m_btnBuy.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnBuy.SetHighlightUI( "V3_Btn92x24_H"  );
   buyButtonEvent.SetUI( this );
   SetHelpId( m_btnBuy, 336 );
   m_btnBuy.Hide();

   m_btnTrade.SetSpriteId   ( "V3_Btn92x24_N"   );
   m_btnTrade.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnTrade.SetHighlightUI( "V3_Btn92x24_H"  );
   buyButtonEvent.SetUI( this );
   SetHelpId( m_btnTrade, 810 );
   m_btnTrade.Hide();

   

   m_btnSell.SetSpriteId   ( "V3_Btn92x24_N"   );
   m_btnSell.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnSell.SetHighlightUI( "V3_Btn92x24_H"  );
   buyButtonEvent.SetUI( this );
   SetHelpId( m_btnSell, 336 );
   m_btnSell.Hide();

   

   m_ListItem.SetupList( 5, 38, FramePos( 0, 0, 2, 0 ) );
   m_ListItem.CenterColumn( 0 );
   m_ListItem.CenterColumn( 1 );
   m_ListItem.CenterColumn( 2 );
   m_ListItem.CenterColumn( 3 );
   m_ListItem.CenterColumn( 4 );
   m_ListItem.SetColumnSize( 0, 40 );
   m_ListItem.SetColumnSize( 1, 198 );
   m_ListItem.SetColumnSize( 2, 70 );
   m_ListItem.SetColumnSize( 3, 44 );
   m_ListItem.SetColumnSize( 4, 24 );
   itemListEvent.SetUI( this );
   selectionSprite.SetPos( FramePos( 0, 0, 384, 38 ) );
   selectionSprite.SetSpriteId( "V3_BuyBackListSelect" );
   selectionSprite.Show();
   m_ListItem.SetSelectionSprite( &selectionSprite);


   m_ListItem.SetScrollDownDownUI( &listArrowDown );
   m_ListItem.SetScrollUpDownUI( &listArrowUp );

   m_stHOr.SetText(g_GUILocalString[24],g_DefColorH);
   m_stHReq.SetText(g_GUILocalString[74],g_DefColorH);
   m_stHTot.SetText(g_GUILocalString[75],g_DefColorH);
   m_stHLeft.SetText(g_GUILocalString[76],g_DefColorH);


   SetHelpId( m_stHTot , 333 );
   SetHelpId( m_stTot  , 333 );
   SetHelpId( m_stHReq , 334 ); 
   SetHelpId( m_stReq  , 334 ); 
   SetHelpId( m_stHLeft, 335 );
   SetHelpId( m_stLeft , 335 );

   
   m_stHCol1.SetText(g_GUILocalString[71],g_DefColorH);
   m_stHCol2.SetText(g_GUILocalString[72],g_DefColorH);
   m_stHCol3.SetText(g_GUILocalString[73],g_DefColorH);

  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_btnBuy);
   AddChild( &m_btnTrade);
   AddChild( &m_btnSell);
   AddChild( &m_ListItem);
   AddChild( &m_stHCol1 );
   AddChild( &m_stHCol2 );
   AddChild( &m_stHCol3 );
   
   AddChild( &m_stHOr );
   AddChild( &m_stHReq );
   AddChild( &m_stHTot );
   AddChild( &m_stHLeft );
   AddChild( &m_stReq );
   AddChild( &m_stTot );
   AddChild( &m_stLeft );

   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );


   m_ulCredits = 0;

} 

V3_BuyDlg::~V3_BuyDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_BuyDlg *V3_BuyDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_BuyDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_BuyDlg::Display( void *null ,UINT uiWhat)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   GetInstance()->m_uiBuyMode = uiWhat;
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

      if(uiWhat == 0)
      {
         GetInstance()->m_btnBuy.Show();
         GetInstance()->m_btnSell.Hide();
         GetInstance()->m_btnTrade.Hide();
         GetInstance()->m_stHTot.SetText(g_GUILocalString[75],g_DefColorH);
      }
      else if(uiWhat == 1)
      {
         GetInstance()->m_btnBuy.Hide();
         GetInstance()->m_btnSell.Show();
         GetInstance()->m_btnTrade.Hide();
         GetInstance()->m_stHTot.SetText(g_GUILocalString[113],g_DefColorH);
      }
      else if(uiWhat == 2)
      {
         GetInstance()->m_btnBuy.Hide();
         GetInstance()->m_btnSell.Hide();
         GetInstance()->m_btnTrade.Show();
         GetInstance()->m_stHTot.SetText(g_GUILocalString[75],g_DefColorH);
      }

      GetInstance()->Show();
   }
   return 0;
}

void V3_BuyDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   m_btnBuy.SetPos ( FramePos( iPosX+468, iPosY+366, iPosX+468+92, iPosY+366+24 ) );
   m_btnSell.SetPos ( FramePos( iPosX+468, iPosY+366, iPosX+468+92, iPosY+366+24 ) );
   m_btnTrade.SetPos ( FramePos( iPosX+468, iPosY+366, iPosX+468+92, iPosY+366+24 ) );
   
   

   m_ListItem.SetListDef(iPosX,iPosY,20,40,92,446,395,&listArrowUp,&listArrowDown);

   m_stHCol1.SetPos ( FramePos( iPosX+78 , iPosY+68, iPosX+278, iPosY+85 ) );
   m_stHCol2.SetPos ( FramePos( iPosX+283, iPosY+68, iPosX+350, iPosY+85 ) );
   m_stHCol3.SetPos ( FramePos( iPosX+355, iPosY+68, iPosX+396, iPosY+85 ) );

   m_stHOr  .SetPos ( FramePos( iPosX+461, iPosY+105, iPosX+565, iPosY+122 ) );
   m_stHReq .SetPos ( FramePos( iPosX+461, iPosY+135, iPosX+565, iPosY+152 ) );
   m_stHTot .SetPos ( FramePos( iPosX+461, iPosY+185, iPosX+565, iPosY+202 ) );
   m_stHLeft.SetPos ( FramePos( iPosX+461, iPosY+235, iPosX+565, iPosY+252 ) );
   
   m_stReq  .SetPos ( FramePos( iPosX+461, iPosY+205, iPosX+565, iPosY+222 ) );
   m_stTot  .SetPos ( FramePos( iPosX+461, iPosY+155, iPosX+565, iPosY+172 ) );
   m_stLeft .SetPos ( FramePos( iPosX+461, iPosY+255, iPosX+565, iPosY+272 ) );
   

}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_BuyDlg::Draw(V2SPRITEFX *vsfFX)
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

   m_btnBuy.DrawTextButtonExt(hdc);
   m_btnSell.DrawTextButtonExt(hdc);
   m_btnTrade.DrawTextButtonExt(hdc);

   
   // Draw les texte...
   char strTitle[100];
   if(m_uiBuyMode == 0) //Acheter
      sprintf_s(strTitle,100  ,g_GUILocalString[70]);
   else if(m_uiBuyMode == 1) //Vendre
      sprintf_s(strTitle,100  ,g_GUILocalString[77]);
   else if(m_uiBuyMode == 2) //Echanger
      sprintf_s(strTitle,100  ,g_GUILocalString[390]);
   


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

void V3_BuyDlg::Hide()
{
   BoxUI::Hide();
}

void V3_BuyDlg::Show()
{
   UpdateAllFramePos(g_Global.GetDialogLastXPos(),g_Global.GetDialogLastYPos());
   BoxUI::Show();
}




void V3_BuyDlg::StartDragDlg( MousePos mousePos )
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

void V3_BuyDlg::StopDragDlg()
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
void V3_BuyDlg::DragEvent::LeftClicked( void )
{
   V3_BuyDlg *me = static_cast< V3_BuyDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_BuyDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_BuyDlg *me = static_cast< V3_BuyDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}



//  Called when a sell packet comes in.
void V3_BuyDlg::Buy(const list< SellItemInfo > &sellItemList)
{       
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   
   m_ulCredits = Player.Gold;
   m_stHOr.SetText(g_GUILocalString[24],g_DefColorH);

   // If there are items in the sell list.
   if( sellItemList.size() != 0 )
   {
      
      // Remove previous items from grid
      m_ListItem.DeleteList();
      
      // For each items in the list.
      list< SellItemInfo >::const_iterator i;
      for( i = sellItemList.begin(); i != sellItemList.end(); i++)
      {
         char buf[ 512 ];
         m_ListItem.AddNewItem();
         
         // Create the item's icon.
         SellItemUI *newGraphItem = new SellItemUI;
         CV2Sprite *lpVSF = ItemIcons( (*i).appearance );
         newGraphItem->itemInfo = *i;
         newGraphItem->SetVSF( lpVSF );
         newGraphItem->SetPos( FramePos( 0, 0, lpVSF->GetWidth(), lpVSF->GetHeight() ) );
         sprintf_s(buf,512, "%s. %s", (*i).name.c_str(), (*i).reqs.c_str());
         newGraphItem->SetHelpText( FontManager::GetInstance()->GetSmallFont(buf ) );
         
         m_ListItem.AddColumnItem( 0, newGraphItem );
         
         DWORD textColor;
         if( (*i).canEquip )
         {
            textColor = g_DefColorD;
         }
         else
         {
            textColor = CL_RED;
         }
         
         // Create a new text objects.
         // The name
         StaticTextUI *newTextItem = new StaticTextUI;
         newTextItem->SetText( (*i).name, textColor );
         newTextItem->AutoTruncate( ItemNameSize );
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( false );
         m_ListItem.AddColumnItem( 1, newTextItem );
         
         // The price
         newTextItem = new StaticTextUI;            
		   _itoa_s( (*i).price, buf,256, 10 );
         newTextItem->SetText( buf, textColor );
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( true );
         m_ListItem.AddColumnItem( 2, newTextItem );
         
         // The current qty.
         newTextItem = new StaticTextUI;
         newTextItem->SetText( "0", textColor );
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( true );
         m_ListItem.AddColumnItem( 3, newTextItem );
         
         ListUI *newItemData = new ListUI( EmptyEvent::GetInstance() );
         ButtonUI *upButton = new ButtonUI( EmptyEvent::GetInstance() );
         ButtonUI *downButton = new ButtonUI( EmptyEvent::GetInstance() );
         
         upButton->SetSpriteId   ( "V3_SpinUP_N" );
         upButton->SetHighlightUI( "V3_SpinUP_H" );
         upButton->SetPos( FramePos( 0, 0, 16, 11 ) );
         SetHelpId( upButton, 337 );
         downButton->SetSpriteId   ( "V3_SpinDN_N" );
         downButton->SetHighlightUI( "V3_SpinDN_H" );
         downButton->SetPos( FramePos( 0, 0, 16, 11 ) );
         SetHelpId( downButton, 338 );
         
         newItemData->SetPos( FramePos( 0, 0, 16, 21 ) );
         newItemData->SetupList( 1, 10, FramePos( 0, 0, 0, 1 ) );
         newItemData->SetColumnSize( 0, 16 );
         newItemData->AddNewItem();
         newItemData->AddColumnItem( 0, upButton );
         newItemData->AddNewItem();
         newItemData->AddColumnItem( 0, downButton );
         
         m_ListItem.AddColumnItem( 4, newItemData );
         
         m_stReq.SetText( "0" ,g_DefColorD);
		   _itoa_s( m_ulCredits, buf, 10 );
         m_stTot.SetText(buf ,g_DefColorD);
         m_stLeft.SetText( buf ,g_DefColorD);
      }
      
      // Display sell box.
      Display(NULL,0); //Acheter
   }   
}

void V3_BuyDlg::Sell(const list< SellItemInfo > &sellItemList)
{       
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   m_stHOr.SetText(g_GUILocalString[24],g_DefColorH);

   m_ulCredits = Player.Gold;
   // If there are items in the sell list.
   if( sellItemList.size() != 0 )
   {
      
      // Remove previous items from grid
      m_ListItem.DeleteList();
      
      // For each items in the list.
      list< SellItemInfo >::const_iterator i;
      for( i = sellItemList.begin(); i != sellItemList.end(); i++ )
      {
         char buf[ 256 ];
         m_ListItem.AddNewItem();
         
         // Create the item's icon.
         SellItemUI *newGraphItem = new SellItemUI;
         CV2Sprite *lpVSF = ItemIcons( (*i).appearance );
         newGraphItem->itemInfo = *i;
         newGraphItem->SetVSF( lpVSF );
         newGraphItem->SetPos( FramePos( 0, 0, lpVSF->GetWidth(), lpVSF->GetHeight() ) );
         newGraphItem->SetHelpText( FontManager::GetInstance()->GetSmallFont((*i).name.c_str() ) );
         
         m_ListItem.AddColumnItem( 0, newGraphItem );
         
         // Create a new text objects.
         // The name
         StaticTextUI *newTextItem = new StaticTextUI;            
         newTextItem->SetText( (*i).name);
         newTextItem->AutoTruncate( ItemNameSize );
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( false );
         m_ListItem.AddColumnItem( 1, newTextItem );
         
         // The price
         newTextItem = new StaticTextUI;            
		   _itoa_s( (*i).price, buf,256, 10 );
         newTextItem->SetText( buf);
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( true );
         m_ListItem.AddColumnItem( 2, newTextItem );
         
         // The current qty.
         newTextItem = new StaticTextUI;
         newTextItem->SetText( "0");
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( true );
         m_ListItem.AddColumnItem( 3, newTextItem );
         
         ListUI *newItemData = new ListUI( EmptyEvent::GetInstance() );
         ButtonUI *upButton = new ButtonUI( EmptyEvent::GetInstance() );
         ButtonUI *downButton = new ButtonUI( EmptyEvent::GetInstance() );
         
         upButton->SetSpriteId   ( "V3_SpinUP_N" );
         upButton->SetHighlightUI( "V3_SpinUP_H" );
         upButton->SetPos( FramePos( 0, 0, 16, 11 ) );
         SetHelpId( upButton, 337 );
         downButton->SetSpriteId   ( "V3_SpinDN_N" );
         downButton->SetHighlightUI( "V3_SpinDN_H" );
         downButton->SetPos( FramePos( 0, 0, 16, 11 ) );
         SetHelpId( downButton, 338 );
         
         newItemData->SetPos( FramePos( 0, 0, 16, 21 ) );
         newItemData->SetupList( 1, 10, FramePos( 0, 0, 0, 1 ) );
         newItemData->SetColumnSize( 0, 16 );
         newItemData->AddNewItem();
         newItemData->AddColumnItem( 0, upButton );
         newItemData->AddNewItem();
         newItemData->AddColumnItem( 0, downButton );
         
         m_ListItem.AddColumnItem( 4, newItemData );

         m_stReq.SetText( "0" ,g_DefColorD);

		 _itoa_s( m_ulCredits, buf,256, 10 );
         m_stTot.SetText( buf ,g_DefColorD);
         m_stLeft.SetText( buf ,g_DefColorD);
      }
      // Display sell box.
      Display(NULL,1); //Vendre
   }   
}

void V3_BuyDlg::Echanger(const list< SellItemInfo > &sellItemList,unsigned long ulPoints)
{       
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );

   m_ulCredits = ulPoints;
   m_stHOr.SetText(g_GUILocalString[391],g_DefColorH);
   // If there are items in the sell list.
   if( sellItemList.size() != 0 )
   {

      // Remove previous items from grid
      m_ListItem.DeleteList();

      // For each items in the list.
      list< SellItemInfo >::const_iterator i;
      for( i = sellItemList.begin(); i != sellItemList.end(); i++)
      {
         char buf[ 512 ];
         m_ListItem.AddNewItem();

         // Create the item's icon.
         SellItemUI *newGraphItem = new SellItemUI;
         CV2Sprite *lpVSF = ItemIcons( (*i).appearance );
         newGraphItem->itemInfo = *i;
         newGraphItem->SetVSF( lpVSF );
         newGraphItem->SetPos( FramePos( 0, 0, lpVSF->GetWidth(), lpVSF->GetHeight() ) );
         sprintf_s(buf,512, "%s. %s", (*i).name.c_str(), (*i).reqs.c_str());
         newGraphItem->SetHelpText( FontManager::GetInstance()->GetSmallFont(buf ) );

         m_ListItem.AddColumnItem( 0, newGraphItem );

         DWORD textColor;
         if( (*i).canEquip )
         {
            textColor = g_DefColorD;
         }
         else
         {
            textColor = CL_RED;
         }

         // Create a new text objects.
         // The name
         StaticTextUI *newTextItem = new StaticTextUI;
         newTextItem->SetText( (*i).name, textColor );
         newTextItem->AutoTruncate( ItemNameSize );
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( false );
         m_ListItem.AddColumnItem( 1, newTextItem );

         // The price
         newTextItem = new StaticTextUI;            
         _itoa_s( (*i).price, buf,256, 10 );
         newTextItem->SetText( buf, textColor );
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( true );
         m_ListItem.AddColumnItem( 2, newTextItem );

         // The current qty.
         newTextItem = new StaticTextUI;
         newTextItem->SetText( "0", textColor );
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( true );
         m_ListItem.AddColumnItem( 3, newTextItem );

         ListUI *newItemData = new ListUI( EmptyEvent::GetInstance() );
         ButtonUI *upButton = new ButtonUI( EmptyEvent::GetInstance() );
         ButtonUI *downButton = new ButtonUI( EmptyEvent::GetInstance() );

         upButton->SetSpriteId   ( "V3_SpinUP_N" );
         upButton->SetHighlightUI( "V3_SpinUP_H" );
         upButton->SetPos( FramePos( 0, 0, 16, 11 ) );
         SetHelpId( upButton, 337 );
         downButton->SetSpriteId   ( "V3_SpinDN_N" );
         downButton->SetHighlightUI( "V3_SpinDN_H" );
         downButton->SetPos( FramePos( 0, 0, 16, 11 ) );
         SetHelpId( downButton, 338 );

         newItemData->SetPos( FramePos( 0, 0, 16, 21 ) );
         newItemData->SetupList( 1, 10, FramePos( 0, 0, 0, 1 ) );
         newItemData->SetColumnSize( 0, 16 );
         newItemData->AddNewItem();
         newItemData->AddColumnItem( 0, upButton );
         newItemData->AddNewItem();
         newItemData->AddColumnItem( 0, downButton );

         m_ListItem.AddColumnItem( 4, newItemData );

         m_stReq.SetText( "0" ,g_DefColorD);
         _itoa_s( m_ulCredits, buf, 10 );
         m_stTot.SetText(buf ,g_DefColorD);
         m_stLeft.SetText( buf ,g_DefColorD);
      }

      // Display sell box.
      Display(NULL,2); //Echanger
   }   
}


//  Player wants to sell the items it has selected.
void V3_BuyDlg::BuyButtonEvent::LeftClicked( void )
{
   V3_BuyDlg *me = static_cast< V3_BuyDlg * >( GetUI() );
   
   if(me->m_uiBuyMode == 0) //ACHETER
   {
      // Fetch all items to sell.
      vector< vector< ListUI::GridItem > > *items = me->m_ListItem.GetGrid();
   
      bool send = false;
   
      // Get all items from the 1st column (SellItemUIs).    
      vector< ListUI::GridItem >::iterator i;
      vector< SellItemInfo > bought;
   
      for( i = (*items)[0].begin(); i != (*items)[0].end(); i++ )
      {
         SellItemUI *sellItem = static_cast< SellItemUI * >( *i );
      
         // If we bought this item
         if( sellItem->itemInfo.qty != 0 )
         {
            bought.push_back( sellItem->itemInfo );
         }
      }
      // If any items were bought.
      if( bought.size() != 0 )
      {
         TFCPacket sending;
         sending << (short)RQ_SendBuyItemList;
         sending << (short)TalkToX;
         sending << (short)TalkToY;
         sending << (long)TalkToID;      
      
         sending << (short)bought.size();
         vector< SellItemInfo >::iterator j;
         for( j = bought.begin(); j != bought.end(); j++ )
         {
            sending << (short)( (*j).dwID );
            sending << (short)( (*j).qty );
         }
         SEND_PACKET( sending );
      }
   }
   else if(me->m_uiBuyMode == 1) //VENDRE
   {
      // Fetch all items to sell.
      vector< vector< ListUI::GridItem > > *items = me->m_ListItem.GetGrid();
      
      bool send = false;
      TFCPacket sending;
      sending << (short)RQ_SendSellItemList;
      sending << (short)TalkToX;
      sending << (short)TalkToY;
      sending << (long)TalkToID;      
      

      // Get all items from the 4th column (SellItemUIs).
      unsigned short ushNbrItem = 0;
      vector< ListUI::GridItem >::iterator i;
      for( i = (*items)[0].begin(); i != (*items)[0].end(); i++ )
      {
         SellItemUI *sellItem = static_cast< SellItemUI * >( *i );

         // If we sell this item
         if( sellItem->itemInfo.qty != 0 )
            ushNbrItem++;
      }


      unsigned short ushQty = (*items)[0].size();
      sending << (short)ushNbrItem;
      
      // Get all items from the 4th column (SellItemUIs).
      for( i = (*items)[0].begin(); i != (*items)[0].end(); i++ )
      {
         SellItemUI *sellItem = static_cast< SellItemUI * >( *i );
         
         // If we sell this item
         if( sellItem->itemInfo.qty != 0 )
         {
            send = true;
            
            sending << (long)sellItem->itemInfo.dwID;
            sending << (long)sellItem->itemInfo.qty;
         }
      }
      
      if( send )
      {
         SEND_PACKET( sending );
      }
   }
   else if(me->m_uiBuyMode == 2) //ECHANGER
   {
      // Fetch all items to sell.
      vector< vector< ListUI::GridItem > > *items = me->m_ListItem.GetGrid();

      bool send = false;

      // Get all items from the 1st column (SellItemUIs).    
      vector< ListUI::GridItem >::iterator i;
      vector< SellItemInfo > bought;

      for( i = (*items)[0].begin(); i != (*items)[0].end(); i++ )
      {
         SellItemUI *sellItem = static_cast< SellItemUI * >( *i );

         // If we bought this item
         if( sellItem->itemInfo.qty != 0 )
         {
            bought.push_back( sellItem->itemInfo );
         }
      }
      // If any items were bought.
      if( bought.size() != 0 )
      {
         TFCPacket sending;
         sending << (short)RQ_SendPointsItemList;
         sending << (short)TalkToX;
         sending << (short)TalkToY;
         sending << (long)TalkToID;      

         sending << (short)bought.size();
         vector< SellItemInfo >::iterator j;
         for( j = bought.begin(); j != bought.end(); j++ )
         {
            sending << (short)( (*j).dwID );
            sending << (short)( (*j).qty );
         }
         SEND_PACKET( sending );
      }
   }
   
   me->Hide();
}


//  Players selected a new item from the list.
void V3_BuyDlg::ItemListEvent::LeftClicked( void )
{
   V3_BuyDlg *me = static_cast< V3_BuyDlg * >( GetUI() );
   
   int col = me->m_ListItem.GetSelectedColumn();
   
   // If the selected column is the +/- column
   if( col == 4 )
   {        
      const GameUI *selection = me->m_ListItem.GetSelection();
      
      if( selection == NULL )
      {
         return;
      }
     
      if(me->m_uiBuyMode == 0) //Acheter
      {
         SellItemUI *item = static_cast< SellItemUI * >( const_cast< GameUI * >( selection ) );
         
         // If clicked on the upper + part of the arrow column
         if( MouseDevice::yPos < me->m_ListItem.GetSelectedRowItem(4)->GetPos().y + me->m_ListItem.GetSelectedRowItem(4)->GetPos().Height() / 2 )
         {
            
            int iSpinNbr = me->m_ListItem.GetSpinCnt();
            
            static int iStepUPBuy  = 1;
            static int iStepCntBuy = 0;
            if(iSpinNbr == 0)
            {
               iStepUPBuy = 1;
               iStepCntBuy= 0;
            }
            else if(++iStepCntBuy >10)
            {
               iStepCntBuy= 0;
               iStepUPBuy*=2;
               if(iStepUPBuy >100)
                  iStepUPBuy = 100;
            }
            
            int currentPrice = atoi( me->m_stReq.GetText()->GetText() );
            
            //if( item->itemInfo.price <= m_ulCredits - currentPrice )
            if( currentPrice + (item->itemInfo.price*iStepUPBuy) <= me->m_ulCredits)
            {
               //if(item->itemInfo.qty <2000)
               {

                  char buf[ 256 ];

                  item->itemInfo.qty+=iStepUPBuy;
                  if(item->itemInfo.qty > 2000)
                  {
                     iStepUPBuy = iStepUPBuy-(item->itemInfo.qty-2000);
                     item->itemInfo.qty = 2000;
                  }


                  StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
                  _itoa_s( item->itemInfo.qty, buf,256, 10 ) ;
                  qtyUI->SetText( buf);

                  currentPrice += (item->itemInfo.price*iStepUPBuy);

                  _itoa_s( currentPrice, buf,256, 10 );
                  me->m_stReq.SetText( buf );
                  _itoa_s( me->m_ulCredits - currentPrice, buf,256, 10 );
                  me->m_stLeft.SetText( buf );

                  qtySound->Play( TRUE );
               }
            }
            else
            {
               V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 536 ],SYSTEM_COLOR,false);
               naSound->Play( TRUE );
            }
         }
         // If clicked on the lower - part of the arrow column
         else
         {
            if( item->itemInfo.qty != 0 )
            {
               char buf[ 256 ];

               item->itemInfo.qty--;

               int currentPrice = atoi( me->m_stReq.GetText()->GetText() );

               currentPrice -= item->itemInfo.price;

               _itoa_s( currentPrice, buf,256, 10 );
               me->m_stReq.SetText(buf);
               _itoa_s( me->m_ulCredits - currentPrice, buf,256, 10 ) ;
               me->m_stLeft.SetText( buf);                    

               StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
               _itoa_s( item->itemInfo.qty, buf,256, 10 );
               qtyUI->SetText( buf );

               qtySound->Play( TRUE );
            }
            else
            {
               naSound->Play( TRUE );
            }
         }
         
      }
      else if(me->m_uiBuyMode == 1) //vendre
      {
         SellItemUI *item = static_cast< SellItemUI * >( const_cast< GameUI * >( selection ) );
         
         // If clicked on the upper + part of the arrow column
         if( MouseDevice::yPos < me->m_ListItem.GetSelectedRowItem(4)->GetPos().y + me->m_ListItem.GetSelectedRowItem(4)->GetPos().Height() / 2 )
         {
            int iSpinNbr = me->m_ListItem.GetSpinCnt();

            static int iStepUPSold  = 1;
            static int iStepCntSold = 0;
            if(iSpinNbr == 0)
            {
               iStepUPSold = 1;
               iStepCntSold= 0;
            }
            else if(++iStepCntSold >10)
            {
               iStepCntSold= 0;
               iStepUPSold*=2;
               if(iStepUPSold >100)
                  iStepUPSold = 100;
            }


            if( item->itemInfo.qty < item->itemInfo.maxQty )
            {                
               int currentPrice = atoi( me->m_stReq.GetText()->GetText() );

               char buf[ 256 ];

               item->itemInfo.qty+=iStepUPSold;
               if(item->itemInfo.qty > item->itemInfo.maxQty)
               {
                  iStepUPSold = iStepUPSold-(item->itemInfo.qty-item->itemInfo.maxQty);
                  item->itemInfo.qty = item->itemInfo.maxQty;
               }
               if(item->itemInfo.qty > 2000)
               {
                  iStepUPSold = iStepUPSold-(item->itemInfo.qty-2000);
                  item->itemInfo.qty = 2000;
               }


               StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
               _itoa_s( item->itemInfo.qty, buf,256, 10 );
               qtyUI->SetText( buf );

               currentPrice += (item->itemInfo.price*iStepUPSold);

               _itoa_s( currentPrice, buf,256, 10 );
               me->m_stReq.SetText( buf );
               _itoa_s( me->m_ulCredits + currentPrice, buf,256 ,10 );
               me->m_stLeft.SetText(buf );


               qtySound->Play( TRUE );
            }
            else
            {
               naSound->Play( TRUE );
               V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 541 ],SYSTEM_COLOR,false);
            }
         }
         // If clicked on the lower - part of the arrow column
         else
         {
            if( item->itemInfo.qty > 0 )
            {
               char buf[ 256 ];
               
               int currentPrice = atoi( me->m_stReq.GetText()->GetText() );

               currentPrice -= item->itemInfo.price;

               _itoa_s( currentPrice, buf, 256,10 );
               me->m_stReq.SetText( buf );
               _itoa_s( me->m_ulCredits + currentPrice, buf,256, 10 );
               me->m_stLeft.SetText( buf );

               item->itemInfo.qty--;

               StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );

               _itoa_s( item->itemInfo.qty, buf,256, 10 );
               qtyUI->SetText( buf );

               qtySound->Play( TRUE );
            }
            else
            {
               naSound->Play( TRUE );
            }
            
         }
      }
      else if(me->m_uiBuyMode == 2) //Echanger
      {
         SellItemUI *item = static_cast< SellItemUI * >( const_cast< GameUI * >( selection ) );

         // If clicked on the upper + part of the arrow column
         if( MouseDevice::yPos < me->m_ListItem.GetSelectedRowItem(4)->GetPos().y + me->m_ListItem.GetSelectedRowItem(4)->GetPos().Height() / 2 )
         {

            int iSpinNbr = me->m_ListItem.GetSpinCnt();

            static int iStepUPBuy  = 1;
            static int iStepCntBuy = 0;
            if(iSpinNbr == 0)
            {
               iStepUPBuy = 1;
               iStepCntBuy= 0;
            }
            else if(++iStepCntBuy >10)
            {
               iStepCntBuy= 0;
               iStepUPBuy*=2;
               if(iStepUPBuy >100)
                  iStepUPBuy = 100;
            }

            int currentPrice = atoi( me->m_stReq.GetText()->GetText() );

            //if( item->itemInfo.price <= m_ulCredits - currentPrice )
            if( currentPrice + (item->itemInfo.price*iStepUPBuy) <= me->m_ulCredits)
            {
               //if(item->itemInfo.qty <2000)
               {

                  char buf[ 256 ];

                  item->itemInfo.qty+=iStepUPBuy;
                  if(item->itemInfo.qty > 2000)
                  {
                     iStepUPBuy = iStepUPBuy-(item->itemInfo.qty-2000);
                     item->itemInfo.qty = 2000;
                  }


                  StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
                  _itoa_s( item->itemInfo.qty, buf,256, 10 ) ;
                  qtyUI->SetText( buf);

                  currentPrice += (item->itemInfo.price*iStepUPBuy);

                  _itoa_s( currentPrice, buf,256, 10 );
                  me->m_stReq.SetText( buf );
                  _itoa_s( me->m_ulCredits - currentPrice, buf,256, 10 );
                  me->m_stLeft.SetText( buf );

                  qtySound->Play( TRUE );
               }
            }
            else
            {
               V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 809 ],SYSTEM_COLOR,false);
               naSound->Play( TRUE );
            }
         }
         // If clicked on the lower - part of the arrow column
         else
         {
            if( item->itemInfo.qty != 0 )
            {
               char buf[ 256 ];

               item->itemInfo.qty--;

               int currentPrice = atoi( me->m_stReq.GetText()->GetText() );

               currentPrice -= item->itemInfo.price;

               _itoa_s( currentPrice, buf,256, 10 );
               me->m_stReq.SetText(buf);
               _itoa_s( me->m_ulCredits - currentPrice, buf,256, 10 ) ;
               me->m_stLeft.SetText( buf);                    

               StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
               _itoa_s( item->itemInfo.qty, buf,256, 10 );
               qtyUI->SetText( buf );

               qtySound->Play( TRUE );
            }
            else
            {
               naSound->Play( TRUE );
            }
         }

      }
   }
   else
   {
   }
}

//  Double-clicking on an item in the list buys it.
void V3_BuyDlg::ItemListEvent::LeftDblClicked( void )
{
   V3_BuyDlg *me = static_cast< V3_BuyDlg * >( GetUI() );
   
   // If the selecti
   int col = me->m_ListItem.GetSelectedColumn();
   // If the player dbl-clicked the up/down arrow
   if( col == 4 ){
      LeftClicked();
      return;
   }
   
   const GameUI *selection = me->m_ListItem.GetSelection();
   if( selection == NULL )
   {
      return;
   }

   if(me->m_uiBuyMode == 0) //Acheter
   {
      SellItemUI *item = static_cast< SellItemUI * >( const_cast< GameUI * >( selection ) );
   
      // Buy the selected item.
      TFCPacket sending;
      sending << (short)RQ_SendBuyItemList;
      sending << (short)TalkToX;
      sending << (short)TalkToY;
      sending << (long)TalkToID;      
   
      sending << (short)1;
      sending << (short)( item->itemInfo.dwID );
      sending << (short)( 1 );
   
      SEND_PACKET( sending );
   }
   else if(me->m_uiBuyMode == 1) //vendre
   {
      SellItemUI *item = static_cast< SellItemUI * >( const_cast< GameUI * >( selection ) );
      
      // Buy the selected item.
      TFCPacket sending;
      sending << (short)RQ_SendSellItemList;
      sending << (short)TalkToX;
      sending << (short)TalkToY;
      sending << (long)TalkToID;      
      unsigned short ushNbr = 1;
      unsigned long  ulQty  = 1;
      sending << (short)ushNbr;
      
      sending << (long)( item->itemInfo.dwID );
      sending << (long)( ulQty );
      
      SEND_PACKET( sending );
   }
   else if(me->m_uiBuyMode == 2) //Echnager
   {
      SellItemUI *item = static_cast< SellItemUI * >( const_cast< GameUI * >( selection ) );

      // Buy the selected item.
      TFCPacket sending;
      sending << (short)RQ_SendPointsItemList;
      sending << (short)TalkToX;
      sending << (short)TalkToY;
      sending << (long)TalkToID;      

      sending << (short)1;
      sending << (short)( item->itemInfo.dwID );
      sending << (short)( 1 );

      SEND_PACKET( sending );
   }
   
   me->Hide();
}