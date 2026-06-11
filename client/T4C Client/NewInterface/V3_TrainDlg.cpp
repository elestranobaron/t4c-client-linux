// V3_TrainDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_TrainDlg.h"
#include "V3_ChatLogDlg.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;



extern int TalkToX, TalkToY,TalkToW;
extern DWORD TalkToID;

namespace
{
    T3VSBSound *qtySound = NULL;
    T3VSBSound *naSound = NULL;
    
    void InitSound()
    {
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

V3_TrainDlg::V3_TrainDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
m_ListItem( itemListEvent ),
m_btnT( tButtonEvent,g_GUILocalString[81],g_DefColorH ),
m_btnL( tButtonEvent,g_GUILocalString[78],g_DefColorH )

{
   int i=0;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_TrainBack",true,"V3_TrainBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_TrainIcon");

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
   m_btnT.Hide();

   m_btnL.SetSpriteId   ( "V3_Btn92x24_N"   );
   m_btnL.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnL.SetHighlightUI( "V3_Btn92x24_H"  );
   tButtonEvent.SetUI( this );
   SetHelpId( m_btnL, 342 );
   m_btnL.Hide();

   

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
   m_stHSkill.SetText(g_GUILocalString[114],g_DefColorH);


   SetHelpId( m_stHTot , 333 );
   SetHelpId( m_stTot  , 333 );
   SetHelpId( m_stHReq , 334 ); 
   SetHelpId( m_stReq  , 334 ); 
   SetHelpId( m_stHLeft, 335 );
   SetHelpId( m_stLeft , 335 );
   SetHelpId( m_stHSkill,341 );
   SetHelpId( m_stSkill ,341 );

   
   m_stHCol1.SetText(g_GUILocalString[79],g_DefColorH);
   m_stHCol2.SetText(g_GUILocalString[72],g_DefColorH);
   m_stHCol3.SetText(g_GUILocalString[80],g_DefColorH);

  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_btnT);
   AddChild( &m_btnL);
   AddChild( &m_ListItem);
   AddChild( &m_stHCol1 );
   AddChild( &m_stHCol2 );
   AddChild( &m_stHCol3 );
   
   AddChild( &m_stHOr );
   AddChild( &m_stHReq );
   AddChild( &m_stHTot );
   AddChild( &m_stHLeft );
   AddChild( &m_stHSkill);
   AddChild( &m_stReq );
   AddChild( &m_stTot );
   AddChild( &m_stLeft );
   AddChild( &m_stSkill);

   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

} 

V3_TrainDlg::~V3_TrainDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_TrainDlg *V3_TrainDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_TrainDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_TrainDlg::Display( void *null ,UINT uiMode)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   GetInstance()->m_uiDisplayMode = uiMode;
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

      if(uiMode == 0)
      {
         GetInstance()->m_btnT.Show();
         GetInstance()->m_btnL.Hide();
      }
      else if(uiMode == 1)
      {
         GetInstance()->m_btnT.Hide();
         GetInstance()->m_btnL.Show();
      }
      else if(uiMode == 2)
      {
         
      }

      GetInstance()->Show();
   }
   return 0;
}

void V3_TrainDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   m_btnT.SetPos ( FramePos( iPosX+468, iPosY+366, iPosX+468+92, iPosY+366+24 ) );
   m_btnL.SetPos ( FramePos( iPosX+468, iPosY+366, iPosX+468+92, iPosY+366+24 ) );
   

   m_ListItem.SetListDef(iPosX,iPosY,20,40,92,446,395,&listArrowUp,&listArrowDown);

   m_stHCol1.SetPos ( FramePos( iPosX+78 , iPosY+68, iPosX+278, iPosY+85 ) );
   m_stHCol2.SetPos ( FramePos( iPosX+283, iPosY+68, iPosX+350, iPosY+85 ) );
   m_stHCol3.SetPos ( FramePos( iPosX+355, iPosY+68, iPosX+396, iPosY+85 ) );

   m_stHOr   .SetPos ( FramePos( iPosX+461, iPosY+105, iPosX+565, iPosY+122 ) );
   m_stHTot  .SetPos ( FramePos( iPosX+461, iPosY+135, iPosX+565, iPosY+152 ) );
   m_stHReq  .SetPos ( FramePos( iPosX+461, iPosY+185, iPosX+565, iPosY+202 ) );
   m_stHLeft .SetPos ( FramePos( iPosX+461, iPosY+235, iPosX+565, iPosY+252 ) );
   m_stHSkill.SetPos ( FramePos( iPosX+461, iPosY+294, iPosX+565, iPosY+311 ) );
   
   m_stReq  .SetPos ( FramePos( iPosX+461, iPosY+155, iPosX+565, iPosY+172 ) );
   m_stTot  .SetPos ( FramePos( iPosX+461, iPosY+205, iPosX+565, iPosY+222 ) );
   m_stLeft .SetPos ( FramePos( iPosX+461, iPosY+255, iPosX+565, iPosY+272 ) );
   m_stSkill.SetPos ( FramePos( iPosX+461, iPosY+314, iPosX+565, iPosY+331 ) );
   

}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_TrainDlg::Draw(V2SPRITEFX *vsfFX)
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
   m_btnL.DrawTextButtonExt(hdc);
   
   // Draw les texte...
   char strTitle[100];
   if(m_uiDisplayMode == 0)
      sprintf_s(strTitle,100  ,g_GUILocalString[81]);
   else if(m_uiDisplayMode == 1)
      sprintf_s(strTitle,100  ,g_GUILocalString[78]);
   else if(m_uiDisplayMode == 2)
      sprintf_s(strTitle,100  ,g_GUILocalString[77]);


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

void V3_TrainDlg::Hide()
{
   BoxUI::Hide();
}

void V3_TrainDlg::Show()
{
   UpdateAllFramePos(g_Global.GetDialogLastXPos(),g_Global.GetDialogLastYPos());
   BoxUI::Show();
}




void V3_TrainDlg::StartDragDlg( MousePos mousePos )
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

void V3_TrainDlg::StopDragDlg()
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
void V3_TrainDlg::DragEvent::LeftClicked( void )
{
   V3_TrainDlg *me = static_cast< V3_TrainDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_TrainDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_TrainDlg *me = static_cast< V3_TrainDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


//  Player wants to sell the items it has selected.
void V3_TrainDlg::TButtonEvent::LeftClicked( void )
{
   V3_TrainDlg *me = static_cast< V3_TrainDlg * >( GetUI() );
   
   if(me->m_uiDisplayMode == 0)
   {
      // Fetch all items to sell.
      vector< vector< ListUI::GridItem > > *items = me->m_ListItem.GetGrid();
      // Get all items from the 4th column (SellItemUIs).
      vector< ListUI::GridItem >::iterator i;
      vector< SellItemInfo > bought;
      
      for( i = (*items)[0].begin(); i != (*items)[0].end(); i++ )
      {
         SellItemUI *sellItem = static_cast< SellItemUI * >( *i );
         // If we sell this item
         if( sellItem->itemInfo.qty != sellItem->itemInfo.initialQty )
            bought.push_back( sellItem->itemInfo );            
      }
       
      if( bought.size() > 0 )
      {
         TFCPacket sending;
         
         sending << (short)RQ_SendTrainSkillList;
         sending << (short)TalkToX;
         sending << (short)TalkToY;
         sending << (long)TalkToID;
         sending << (short)bought.size();
         
         vector< SellItemInfo >::iterator i;
         for( i = bought.begin(); i != bought.end(); i++ )
         {					
            sending << (short)(*i).skillID;	// ID of skill to train.													
            sending << (short)( (*i).qty - (*i).initialQty ); // Quantity of skill points to train
         }
         
         SEND_PACKET( sending );
      }
   }
   else if(me->m_uiDisplayMode == 1)
   {
      // Fetch all items to sell.
      vector< vector< ListUI::GridItem > > &items = *me->m_ListItem.GetGrid();
      // Get all items from the 4th column (SellItemUIs).
      vector< ListUI::GridItem >::iterator i;
      vector< SellItemInfo > bought;
      // If there is only one item to teach
      if( items[0].size() == 1 )
      {
         // Teach this single skill.
         SellItemUI *sellItem = static_cast< SellItemUI * >( *items[0].begin() ); 
         bought.push_back( sellItem->itemInfo );
      }
      else
      {
         for( i = items[0].begin(); i != items[0].end(); i++ )
         {
            SellItemUI *sellItem = static_cast< SellItemUI * >( *i ); 
            // If we teach this item
            if( sellItem->itemInfo.teach )
               bought.push_back( sellItem->itemInfo );            
         }
      }
      
      if( bought.size() > 0 )
      {
         TFCPacket sending;
         
         sending << (short)RQ_SendTeachSkillList;
         sending << (short)TalkToX;
         sending << (short)TalkToY;
         sending << (long)TalkToID;
         
         short shNbrSkill = bought.size();
         sending << (short)shNbrSkill;
         
         vector< SellItemInfo >::iterator i;
         for( i = bought.begin(); i != bought.end(); i++ )
         {					
            sending << (short)(*i).skillID;	// ID of skill to train.													
         }
         
         SEND_PACKET( sending );
      }
   }
   else if(me->m_uiDisplayMode == 2)
   {
      
   }
   
   me->Hide();
}


//  Players selected a new item from the list.
void V3_TrainDlg::ItemListEvent::LeftClicked( void )
{
   V3_TrainDlg *me = static_cast< V3_TrainDlg * >( GetUI() );
   
   int col = me->m_ListItem.GetSelectedColumn();
   
   // If the selected column is the +/- column
   if( col == 4 )
   {        
      const GameUI *selection = me->m_ListItem.GetSelection();
      
      if( selection == NULL )
      {
         return;
      }
     
      if(me->m_uiDisplayMode == 0)
      {
         SellItemUI *item = static_cast< SellItemUI * >( const_cast< GameUI * >( selection ) );
         
         // If clicked on the upper + part of the arrow column
         if( MouseDevice::yPos < me->m_ListItem.GetSelectedRowItem(4)->GetPos().y + me->m_ListItem.GetSelectedRowItem(4)->GetPos().Height() / 2 )
         {
            int iSpinNbr = me->m_ListItem.GetSpinCnt();
            static int iStepUP  = 1;
            static int iStepCnt = 0;
            if(iSpinNbr == 0)
            {
               iStepUP = 1;
               iStepCnt= 0;
            }
            else if(++iStepCnt >10)
            {
               iStepCnt= 0;
               iStepUP*=2;
               if(iStepUP >100)
                  iStepUP = 100;
            }
            
            if(item->itemInfo.qty+iStepUP >= item->itemInfo.maxQty)
               iStepUP = item->itemInfo.maxQty-item->itemInfo.qty;
            if(me->m_wSkillPnts < iStepUP )
               iStepUP = me->m_wSkillPnts;
            
            // If the player can spend more skill points.
            if( item->itemInfo.canHave && (item->itemInfo.qty+iStepUP) <= item->itemInfo.maxQty &&me->m_wSkillPnts >= iStepUP )
            {
               int currentPrice = atoi( me->m_stReq.GetText()->GetText() );
               // If the price still is enough.
               if( currentPrice + (item->itemInfo.price*iStepUP) <= Player.Gold )
               {
                  char buf[ 256 ];
                  item->itemInfo.qty+=iStepUP;
                  StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
				  _itoa_s( item->itemInfo.qty, buf,256, 10 );
                  qtyUI->SetText( buf );
                  currentPrice += (item->itemInfo.price*iStepUP);
                  
				   _itoa_s( currentPrice, buf,256, 10 ); 
                  me->m_stReq.SetText(buf);
				  _itoa_s( Player.Gold - currentPrice, buf,256, 10 );
                  me->m_stLeft.SetText( buf );
                  me->m_wSkillPnts-=iStepUP;
				  _itoa_s( me->m_wSkillPnts, buf,256, 10 );
                  me->m_stSkill.SetText( buf );
                  
                  qtySound->Play( TRUE );
               }
               else
               {
                  if( currentPrice + item->itemInfo.price > Player.Gold )
                  {
                     V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 536 ],SYSTEM_COLOR,false);
                  }
                  
               }
            }
            else
            {
               if( !item->itemInfo.canHave )
               {
                  V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 539 ],SYSTEM_COLOR,false);
               }
               else if( item->itemInfo.qty >= item->itemInfo.maxQty )
               {
                  V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 540 ],SYSTEM_COLOR,false);
               }
               else if( me->m_wSkillPnts == 0 )
               {
                  V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 538 ],SYSTEM_COLOR,false);
               }
               naSound->Play( TRUE );
            }
         }
         // If clicked on the lower - part of the arrow column
         else
         {
            if( item->itemInfo.qty > item->itemInfo.initialQty )
            {
               char buf[ 256 ];
               
               int currentPrice = atoi( me->m_stReq.GetText()->GetText() );
               
               currentPrice -= item->itemInfo.price;
               
			      _itoa_s( currentPrice, buf,256, 10 );
               me->m_stReq.SetText(buf  );
			      _itoa_s( Player.Gold - currentPrice, buf,256, 10 );
               me->m_stLeft.SetText( buf );
               
               item->itemInfo.qty--;
               me->m_wSkillPnts++;
               
               StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
			   _itoa_s( item->itemInfo.qty, buf,256, 10 ) ;
               qtyUI->SetText( buf);
               
			   _itoa_s( me->m_wSkillPnts, buf,256, 10 );
               me->m_stSkill.SetText( buf );
               
               qtySound->Play( TRUE );
            }
            else
            {
               naSound->Play( TRUE );
            }                
         }
      }
      else if(me->m_uiDisplayMode == 1)
      {
         SellItemUI *item = static_cast< SellItemUI * >( const_cast< GameUI * >( selection ) );
         
         // If clicked on the upper + part of the arrow column
         if( MouseDevice::yPos < me->m_ListItem.GetSelectedRowItem(4)->GetPos().y + me->m_ListItem.GetSelectedRowItem(4)->GetPos().Height() / 2 )
         {
            // If the player can spend more skill points.
            if( !item->itemInfo.teach )
            {
               // If player doesn't have enough skill points.
               if( item->itemInfo.skillPnts > me->m_wSkillPnts )
               {
                  V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 538 ],SYSTEM_COLOR,false);                            
                  naSound->Play( TRUE );
                  return;
               }
               
               int currentPrice = atoi( me->m_stReq.GetText()->GetText() );
               // If the price still is enough.
               if( currentPrice + item->itemInfo.price <= Player.Gold && item->itemInfo.canHave )
               {
                  char buf[ 256 ];
                  
                  item->itemInfo.teach = true;
                  
                  StaticTextUI *qtyUI = static_cast< StaticTextUI * >( me->m_ListItem.GetSelectedRowItem( 3 ) );
				  _itoa_s( item->itemInfo.skillPnts, buf,256, 10 );
                  qtyUI->SetText( buf );
                  
                  currentPrice += item->itemInfo.price;
                  
                  me->m_wSkillPnts -= (WORD)item->itemInfo.skillPnts;
                  
				  _itoa_s( currentPrice, buf,256, 10 );
                  me->m_stReq.SetText( buf );
				  _itoa_s( Player.Gold - currentPrice, buf,256, 10 );
                  me->m_stLeft.SetText( buf );
				  _itoa_s( me->m_wSkillPnts, buf,256, 10 );
                  me->m_stSkill.SetText( buf );
                  
                  qtySound->Play( TRUE );
               }
               else
               {
                  if( !item->itemInfo.canHave )
                  {
                     V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[ 537 ],SYSTEM_COLOR,false);                            
                  } 
                  else if( currentPrice + item->itemInfo.price > Player.Gold )
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
               char buf[ 256 ];
               
               int currentPrice = atoi( me->m_stReq.GetText()->GetText() );
               
               currentPrice -= item->itemInfo.price;
               
               me->m_wSkillPnts += (WORD)item->itemInfo.skillPnts;
               
			   _itoa_s( currentPrice, buf,256, 10 );
               me->m_stReq.SetText( buf );
			   _itoa_s( Player.Gold - currentPrice, buf,256, 10 );
               me->m_stLeft.SetText( buf );
			   _itoa_s( me->m_wSkillPnts, buf,256, 10 );
               me->m_stSkill.SetText( buf );
               
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
      else if(me->m_uiDisplayMode == 2)
      {

      }

   }
   else
   {
   }
}


//////////////////////////////////////////////////////////////////////////////////////////
void V3_TrainDlg::Train(WORD newSkillPnts,const list< SellItemInfo > &sellItemList)
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   
   m_wSkillPnts = newSkillPnts;
   
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
         
         DWORD textColor;
         if( (*i).canHave )
            textColor = g_DefColorD;
         else
            textColor = CL_RED;
         
         // Create the item's icon.
         SellItemUI *newGraphItem = new SellItemUI;
         CV2Sprite *lpVSF = SkillIcons( (*i).skillID );
         newGraphItem->itemInfo = *i;
         newGraphItem->SetVSF( lpVSF );
         newGraphItem->SetPos( FramePos( 0, 0, lpVSF->GetWidth(), lpVSF->GetHeight() ) );
         newGraphItem->SetHelpText( FontManager::GetInstance()->GetSmallFont((*i).name.c_str() ) );
         
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
		 _itoa_s( (*i).price, buf,256, 10 );
         newTextItem->SetText( buf, textColor );
         newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
         newTextItem->SetCenter( false );
         m_ListItem.AddColumnItem( 2, newTextItem );
         
         // The current qty.
         newTextItem = new StaticTextUI;
		 _itoa_s( (*i).qty, buf,256, 10 );
         newTextItem->SetText( buf, textColor );
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

         m_stReq.SetText( "0" ,g_DefColorD);
		 _itoa_s( Player.Gold, buf,256, 10 );
         m_stTot.SetText( buf ,g_DefColorD);
         m_stLeft.SetText( buf ,g_DefColorD);
		 _itoa_s(m_wSkillPnts, buf,256, 10 );
         m_stSkill.SetText( buf ,g_DefColorD);

      }
      
      // Display sell box.
      Display(NULL,0);
   }   
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_TrainDlg::Learn(WORD newSkillPnts,const list< SellItemInfo > &sellItemList)
{
    CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );

    m_wSkillPnts = newSkillPnts;
    
    // If there are items in the sell list.
    if( sellItemList.size() != 0 )
    {
        // Remove previous items from grid
        m_ListItem.DeleteList();
        
        // For each items in the list.
        list< SellItemInfo >::const_iterator i;
        for( i = sellItemList.begin(); i != sellItemList.end(); i++ )
        {
            DWORD textColor;
            if( (*i).canHave )
                textColor = g_DefColorD;
            else
                textColor = CL_RED;

            char buf[ 256 ];
            m_ListItem.AddNewItem();

            // Create the item's icon.
            SellItemUI *newGraphItem = new SellItemUI;
            CV2Sprite *lpVSF;
            if( (*i).skillID < 10000 )
                lpVSF = SkillIcons( (*i).icon );
            else
                lpVSF = SpellIcons( (*i).icon );

            string helpText( (*i).name.c_str() );
            helpText += "; ";
            helpText += (*i).reqs;

            newGraphItem->itemInfo = *i;
            newGraphItem->SetVSF( lpVSF );
            newGraphItem->SetPos( FramePos( 0, 0, lpVSF->GetWidth(), lpVSF->GetHeight() ) );
            newGraphItem->SetHelpText( FontManager::GetInstance()->GetSmallFont(helpText.c_str() ) );
            
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
			_itoa_s( (*i).price, buf,256, 10 );
            newTextItem->SetText( buf, textColor );
            newTextItem->SetPos( FramePos( 0, 0, newTextItem->GetWidth(), newTextItem->GetHeight() ) );
            newTextItem->SetCenter( false );
            m_ListItem.AddColumnItem( 2, newTextItem );

            // The skill cost in points
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
            
            m_stReq.SetText( "0" ,g_DefColorD);
			_itoa_s( Player.Gold, buf,256, 10 ) ;
            m_stTot.SetText( buf,g_DefColorD);
            m_stLeft.SetText( buf ,g_DefColorD);
			_itoa_s(m_wSkillPnts, buf,256, 10 );
            m_stSkill.SetText( buf ,g_DefColorD);
        }

        // Display sell box.
        Display(NULL,1);
    }   
}