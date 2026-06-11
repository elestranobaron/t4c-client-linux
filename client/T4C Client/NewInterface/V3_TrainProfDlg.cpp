// V3_TrainProfDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "V3_TrainProfDlg.h"
#include "V3_ChatLogDlg.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;

extern int TalkToX, TalkToY, TalkToW;
extern DWORD TalkToID;

namespace
{
    T3VSBSound *qtySound = NULL;
    T3VSBSound *naSound = NULL;
    
    void InitSound(){
        if( qtySound == NULL ){
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

V3_TrainProfDlg::V3_TrainProfDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
m_ListItem( itemListEvent ),
m_btnT( tButtonEvent,g_GUILocalString[78],g_DefColorH )

{
   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_TrailProfBack",true,"V3_TrailProfBackMask");
   m_Icons      .SetSpriteId("V3_TrainProfIcons");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_ProfessionIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
   
   m_btnT.SetSpriteId   ( "V3_Btn92x24_N"   );
   m_btnT.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnT.SetHighlightUI( "V3_Btn92x24_H"  );
   tButtonEvent.SetUI( this );
   SetHelpId( m_btnT, 342 );

     

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

   m_stHOr.SetText(g_GUILocalString[371],g_DefColorH);
   
   m_stHCol1.SetText(g_GUILocalString[237],g_DefColorH);
   m_stHCol2.SetText(g_GUILocalString[72],g_DefColorH);
   m_stHCol3.SetText(g_GUILocalString[73],g_DefColorH);

  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_Icons);
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_btnT);
   AddChild( &m_ListItem);
   AddChild( &m_stHCol1 );
   AddChild( &m_stHCol2 );
   AddChild( &m_stHCol3 );
   
   AddChild( &m_stHOr );
   AddChild( &m_stOr );

   AddChild( &m_stStatApo);
   AddChild( &m_stStatBij);
   AddChild( &m_stStatCou);
   AddChild( &m_stStatArm);
   AddChild( &m_stStatFor);
   AddChild( &m_stStatEbe);

   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   ushStatApo  = 0;
   ushStatBij  = 0;
   ushStatCou  = 0;
   ushStatArm  = 0;
   ushStatFor  = 0;
   ushStatEbe  = 0;
   dwStatGold  = 0;
} 

V3_TrainProfDlg::~V3_TrainProfDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_TrainProfDlg *V3_TrainProfDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_TrainProfDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_TrainProfDlg::Display( void *null)
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

      GetInstance()->Show();
   }
   return 0;
}

void V3_TrainProfDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_Icons.SetPos ( FramePos( iPosX+460 , iPosY+94, iPosX+491, iPosY+302 ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   m_btnT.SetPos ( FramePos( iPosX+468, iPosY+366, iPosX+468+92, iPosY+366+24 ) );
   

   m_ListItem.SetListDef(iPosX,iPosY,20,40,92,446,395,&listArrowUp,&listArrowDown);

   m_stHCol1.SetPos ( FramePos( iPosX+78 , iPosY+68, iPosX+278, iPosY+85 ) );
   m_stHCol2.SetPos ( FramePos( iPosX+283, iPosY+68, iPosX+350, iPosY+85 ) );
   m_stHCol3.SetPos ( FramePos( iPosX+355, iPosY+68, iPosX+396, iPosY+85 ) );

   m_stHOr   .SetPos ( FramePos( iPosX+461, iPosY+316, iPosX+565, iPosY+333 ) );
   m_stOr    .SetPos ( FramePos( iPosX+461, iPosY+336, iPosX+565, iPosY+353 ) );
   
   m_stStatApo.SetPos ( FramePos( iPosX+491, iPosY+98 , iPosX+565, iPosY+123 ) );
   m_stStatBij.SetPos ( FramePos( iPosX+491, iPosY+133, iPosX+565, iPosY+158 ) );
   m_stStatCou.SetPos ( FramePos( iPosX+491, iPosY+168, iPosX+565, iPosY+193 ) );
   m_stStatArm.SetPos ( FramePos( iPosX+491, iPosY+203, iPosX+565, iPosY+228 ) );
   m_stStatFor.SetPos ( FramePos( iPosX+491, iPosY+238, iPosX+565, iPosY+263 ) );
   m_stStatEbe.SetPos ( FramePos( iPosX+491, iPosY+273, iPosX+565, iPosY+298 ) );

}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_TrainProfDlg::Draw(V2SPRITEFX *vsfFX)
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

   m_btnT.DrawTextButtonExt(hdc);
   
   // Draw les texte...

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[228],strlen(g_GUILocalString[228]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}

void V3_TrainProfDlg::Hide()
{
   BoxUI::Hide();
}

void V3_TrainProfDlg::Show()
{
   UpdateAllFramePos(g_Global.GetDialogLastXPos(),g_Global.GetDialogLastYPos());
   BoxUI::Show();
}




void V3_TrainProfDlg::StartDragDlg( MousePos mousePos )
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

void V3_TrainProfDlg::StopDragDlg()
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
void V3_TrainProfDlg::DragEvent::LeftClicked( void )
{
   V3_TrainProfDlg *me = static_cast< V3_TrainProfDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_TrainProfDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_TrainProfDlg *me = static_cast< V3_TrainProfDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


//  Player wants to sell the items it has selected.
void V3_TrainProfDlg::TButtonEvent::LeftClicked( void )
{
   V3_TrainProfDlg *me = static_cast< V3_TrainProfDlg * >( GetUI() );
   
   // Fetch all items to sell.
   vector< vector< ListUI::GridItem > > &items = *me->m_ListItem.GetGrid();
   // Get all items from the 4th column (ProfItemUI).
   vector< ListUI::GridItem >::iterator i;
   vector< ProfItemInfo > bought;

   // If there is only one item to teach
   if( items[0].size() == 1 )
   {
      // Teach this single skill.
      ProfItemUI *sellItem = static_cast< ProfItemUI * >( *items[0].begin() ); 
      bought.push_back( sellItem->itemInfo );
   }
   else
   {
      for( i = items[0].begin(); i != items[0].end(); i++ )
      {
         ProfItemUI *sellItem = static_cast< ProfItemUI * >( *i ); 
         
         // If we teach this item
         if( sellItem->itemInfo.teach )
         {
            bought.push_back( sellItem->itemInfo );            
         }
      }
   }
   
   if( bought.size() > 0 )
   {
      TFCPacket sending;
      
      sending << (short)RQ_NM_SendTeachFormuleList;
      sending << (short)TalkToX;
      sending << (short)TalkToY;
      sending << (long)TalkToID;
      
      short shNbrSkill = bought.size();
      sending << (short)shNbrSkill;
      
      vector< ProfItemInfo >::iterator i;
      for( i = bought.begin(); i != bought.end(); i++ )
      {					
         sending << (short)(*i).ushID;	// ID of skill to train.													
      }
      
      SEND_PACKET( sending );
   }
   me->Hide();
}


//  Players selected a new item from the list.
void V3_TrainProfDlg::ItemListEvent::LeftClicked( void )
{
   V3_TrainProfDlg *me = static_cast< V3_TrainProfDlg * >( GetUI() );
   
   int col = me->m_ListItem.GetSelectedColumn();
   
   // If the selected column is the +/- column
   if( col == 4 )
   {        
      const GameUI *selection = me->m_ListItem.GetSelection();
      
      if( selection == NULL )
      {
         return;
      }
      
      ProfItemUI *item = static_cast< ProfItemUI * >( const_cast< GameUI * >( selection ) );
      
      // If clicked on the upper + part of the arrow column
      if( MouseDevice::yPos < me->m_ListItem.GetSelectedRowItem(4)->GetPos().y + me->m_ListItem.GetSelectedRowItem(4)->GetPos().Height() / 2 )
      {
         if( !item->itemInfo.teach )
         {
            // If player doesn't have enough gold
            if( item->itemInfo.dwCost > me->dwGoldValue )
            {
               V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 536 ],SYSTEM_COLOR,false);                            
               naSound->Play( TRUE );
               return;
            }
            
            int currentPrice = me->dwGoldBuyPrice;
            // If the price still is enough.
            if( currentPrice + item->itemInfo.dwCost <= me->dwGoldValue && item->itemInfo.canLearn == 1 )
            {
               
               item->itemInfo.teach = true;
               
               StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
               qtyUI->SetText( "1" );
               
               currentPrice += item->itemInfo.dwCost;
               me->dwGoldBuyPrice = currentPrice;   
               char buf[256];
               _itoa_s( me->dwStatGold-me->dwGoldBuyPrice, buf,256, 10 );
               me->m_stOr     .SetText( buf );
               qtySound->Play( TRUE );
            }
            else
            {
               if( item->itemInfo.canLearn ==0)
               {
                  V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 607 ],SYSTEM_COLOR,false);                            
               } 
               else if( item->itemInfo.canLearn ==2)
               {
                  V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 608 ],SYSTEM_COLOR,false);                            
               } 
               else if(currentPrice + item->itemInfo.dwCost > me->dwGoldValue )
               {
                  V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 536 ],SYSTEM_COLOR,false);
               }
               naSound->Play( TRUE );
            }
         }
         else
         {
            
         }
      }
      // If clicked on the lower - part of the arrow column
      else
      {
         if( item->itemInfo.teach )
         {
            int currentPrice = me->dwGoldBuyPrice;
            currentPrice -= item->itemInfo.dwCost;
            me->dwGoldBuyPrice = currentPrice;
            char buf[256];
            _itoa_s( me->dwStatGold-me->dwGoldBuyPrice, buf,256, 10 );
            me->m_stOr     .SetText( buf );
            
            
            item->itemInfo.teach = false;
            
            StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
            qtyUI->SetText( "0" );
            
            qtySound->Play( TRUE );
         }
         else
         {
            naSound->Play( TRUE );
         }                
      }

   }
   else
   {
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TrainProfDlg::ItemListEvent::LeftDblClicked( void )
{
   V3_TrainProfDlg *me = static_cast< V3_TrainProfDlg * >( GetUI() );
   
   // If the selecti
   int col = me->m_ListItem.GetSelectedColumn();
   // If the player dbl-clicked the up/down arrow
   if( col == 4 )
      return;
   
   
   const GameUI *selection = me->m_ListItem.GetSelection();
   if( selection == NULL )
      return;
   
   ProfItemUI *item = static_cast< ProfItemUI * >( const_cast< GameUI * >( selection ) );
   
   if( item->itemInfo.canLearn == 0)
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 607 ],SYSTEM_COLOR,false);                            
      return;
   }
   else if( item->itemInfo.canLearn == 2)
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 608 ],SYSTEM_COLOR,false);                            
      return;
   }
   
   TFCPacket sending;
   sending << (short)RQ_NM_SendTeachFormuleList;
   sending << (short)TalkToX;
   sending << (short)TalkToY;
   sending << (long)TalkToID;
   short shNbrSkill = 1;
   sending << (short)shNbrSkill;
   
   sending << (short)item->itemInfo.ushID;	// ID of Formule to learn
   
   SEND_PACKET( sending );
   
   me->Hide();
}


//////////////////////////////////////////////////////////////////////////////////////////
void V3_TrainProfDlg::TrainProf(DWORD newGoldVal,const list< ProfItemInfo > &ProfItemList)
{

    CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );

    dwGoldValue = newGoldVal;
    
    if( ProfItemList.size() != 0 )// If there are items in the sell list.
    {
        // Remove previous items from grid
        m_ListItem.DeleteList();
        
        // For each items in the list.
        list< ProfItemInfo >::const_iterator i;
        for( i = ProfItemList.begin(); i != ProfItemList.end(); i++ )
        {
            DWORD textColor;
            if( (*i).canLearn == 0 )  //cant
                textColor = CL_RED;
            else if( (*i).canLearn == 1 ) //can
                textColor = g_DefColorD;
            else //already learned...
                textColor = CL_GREEN;


            char buf[ 256 ];
            m_ListItem.AddNewItem();


            // Create the item's icon.
            ProfItemUI *newGraphItem = new ProfItemUI;
            CV2Sprite *lpVSF;
            lpVSF = ProfessionIcons( (*i).chProfession );

            char strtextHelp[1024];
            sprintf_s(strtextHelp,1024,"%s --> %s : %s %d %s",g_GUILocalString[229+(*i).chProfession],
                                                       (*i).name.c_str(),
                                                       g_GUILocalString[235],
                                                       (*i).ushSkillLevel,
                                                       g_GUILocalString[236]);

            newGraphItem->itemInfo = *i;
            newGraphItem->SetVSF( lpVSF );
            newGraphItem->SetPos( FramePos( 0, 0, lpVSF->GetWidth(), lpVSF->GetHeight() ) );
            newGraphItem->SetHelpText( FontManager::GetInstance()->GetSmallFont(strtextHelp ) );
            
            m_ListItem.AddColumnItem( 0, newGraphItem );

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
			   _itoa_s( (*i).dwCost, buf,256, 10 );
            newTextItem->SetText( buf, textColor );
            newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
            newTextItem->SetCenter( false );
            m_ListItem.AddColumnItem( 2, newTextItem );

            // The Qty buy (only 1 possible)
            newTextItem = new StaticTextUI;
            newTextItem->SetText( "0", textColor );
            newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
            newTextItem->SetCenter( false );
            m_ListItem.AddColumnItem( 3, newTextItem );

            
            // Up/Down arrows
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


            _itoa_s( ushStatApo, buf,256, 10 );
            m_stStatApo.SetText( buf );
            _itoa_s( ushStatBij, buf,256, 10 );
            m_stStatBij.SetText( buf );
            _itoa_s( ushStatCou, buf,256, 10 );
            m_stStatCou.SetText( buf );
            _itoa_s( ushStatArm, buf,256, 10 );
            m_stStatArm.SetText( buf );
            _itoa_s( ushStatFor, buf,256, 10 );
            m_stStatFor.SetText( buf );
            _itoa_s( ushStatEbe, buf,256, 10 );
            m_stStatEbe.SetText( buf );
            _itoa_s( dwStatGold, buf,256, 10 );
            m_stOr     .SetText( buf );

            dwGoldBuyPrice = 0;
        }

        // Display sell box.
        Display(NULL);
    }   
}