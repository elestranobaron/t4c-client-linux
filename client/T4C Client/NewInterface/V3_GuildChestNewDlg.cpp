// V3_GuildChestNewDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "V3_GuildChestNewDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_AskValueDlg.h"
#include "V3_AskValueSldDlg.h"

extern CSaveGame g_SaveGame;

const int g_AskChestToBackpack = 0;
const int g_AskBackpackToChest = 1;



namespace
{
   T3VSBSound *dropSound = NULL;

   void InitSound()
   {
      if( dropSound == NULL )
      {
         dropSound = new T3VSBSound;
         dropSound->Create( "Generic drop item", TS_MEMORY );
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
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_GuildChestNewDlg::V3_GuildChestNewDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
ChestList( chestListEvent ),
//EmptyEvent::GetInstance()
m_Onglet00(tabEvent00,g_GUILocalString[269],g_DefColorH ),
m_Onglet01(tabEvent01,g_GUILocalString[270],g_DefColorH ),
m_Onglet02(tabEvent02,g_GUILocalString[271],g_DefColorH ),
m_Onglet03(tabEvent03,g_GUILocalString[272],g_DefColorH ),
m_Onglet04(tabEvent04,g_GUILocalString[273],g_DefColorH ),
m_Onglet05(tabEvent05,g_GUILocalString[274],g_DefColorH ),
m_Onglet06(tabEvent06,g_GUILocalString[275],g_DefColorH ),
m_Onglet07(tabEvent07,g_GUILocalString[276],g_DefColorH ),
m_Onglet08(tabEvent08,g_GUILocalString[277],g_DefColorH ),
m_Onglet09(tabEvent09,g_GUILocalString[278],g_DefColorH ),
m_Onglet10(tabEvent10,g_GUILocalString[279],g_DefColorH ),
m_Onglet11(tabEvent11,g_GUILocalString[280],g_DefColorH ),
m_Onglet12(tabEvent12,g_GUILocalString[281],g_DefColorH ),
m_Onglet13(tabEvent13,g_GUILocalString[343],g_DefColorH ),
m_Onglet14(tabEvent14,g_GUILocalString[282],g_DefColorH ),
m_Onglet15(tabEvent15,g_GUILocalString[283],g_DefColorH ),
statGoldChestBtn(statGoldChestBtnEvent),
m_dwCurrentOngletSelect(0),
m_pDisplayBagItem(NULL),
m_bNeedToUpdateList(FALSE),
mainDropZoneAddChest( chestGridEvent ),
mainDropZoneRemChest( chestGridEvent ),
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
   m_iXPos = g_Global.GetDialogLastChestNewXPos();
   m_iYPos = g_Global.GetDialogLastChestNewYPos();
   
 
   m_BackgroundS.SetSpriteId("V3_NewChestBack",true,"V3_NewChestBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_ChestIcon");

   SetHelpId( this, 297 );

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
    
  
   //Liste des metiers...
   ChestList.SetupList( 1, 28, FramePos( 0, 0, 1, 0 ) );
   ChestList.SetColumnSize( 0, 230 );
   
   ChestListSelGraph.SetPos( FramePos( 0, 0, 230, 28 ) );
   ChestListSelGraph.SetSpriteId( "V3_NewChestBackSelect" );
   ChestListSelGraph.Show();
   ChestList.SetSelectionSprite( &ChestListSelGraph, 0);
   ChestList.SetScrollDownDownUI( &ChestListScrollDown );
   ChestList.SetScrollUpDownUI( &ChestListScrollUp );
   chestListEvent.SetUI( this );

     
   m_Onglet00.SetHighlightUI( "V3_MainOnglet_60p_H");
   m_Onglet01.SetHighlightUI( "V3_MainOnglet_60p_H");
   m_Onglet02.SetHighlightUI( "V3_MainOnglet_60p_H");
   m_Onglet03.SetHighlightUI( "V3_MainOnglet_60p_H");
   m_Onglet04.SetHighlightUI( "V3_MainOnglet_60p_H");
   m_Onglet05.SetHighlightUI( "V3_MainOnglet_70p_H");
   m_Onglet06.SetHighlightUI( "V3_MainOnglet_70p_H");
   m_Onglet07.SetHighlightUI( "V3_MainOnglet_60p_H");
   m_Onglet08.SetHighlightUI( "V3_MainOnglet_60p_H");
   m_Onglet09.SetHighlightUI( "V3_MainOnglet_60p_H");
   m_Onglet10.SetHighlightUI( "V3_MainOnglet_90p_H");
   m_Onglet11.SetHighlightUI( "V3_MainOnglet_90p_H");
   m_Onglet12.SetHighlightUI( "V3_MainOnglet_90p_H");
   m_Onglet13.SetHighlightUI( "V3_MainOnglet_70p_H");
   m_Onglet14.SetHighlightUI( "V3_MainOnglet_70p_H");
   m_Onglet15.SetHighlightUI( "V3_MainOnglet_70p_H");

   tabEvent00.SetUI(this);
   tabEvent01.SetUI(this);
   tabEvent02.SetUI(this);
   tabEvent03.SetUI(this);
   tabEvent04.SetUI(this);
   tabEvent05.SetUI(this);
   tabEvent06.SetUI(this);
   tabEvent07.SetUI(this);
   tabEvent08.SetUI(this);
   tabEvent09.SetUI(this);
   tabEvent10.SetUI(this);
   tabEvent11.SetUI(this);
   tabEvent12.SetUI(this);
   tabEvent13.SetUI(this);
   tabEvent14.SetUI(this);
   tabEvent15.SetUI(this);

   statGoldChestBtnEvent.SetUI(this);

   //SetHelpId( mainDropZoneAddChest,   275 );
   //SetHelpId( mainDropZoneRemChest,   275 );

   mainDropZoneRemChest.SetBypassRClick(TRUE);

    
   statQty.SetText("");
   statGoldChest.SetText("");

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);

   AddChild( &ChestList );

   AddChild( &m_Onglet00);
   AddChild( &m_Onglet01);
   AddChild( &m_Onglet02);
   AddChild( &m_Onglet03);
   AddChild( &m_Onglet04);
   AddChild( &m_Onglet05);
   AddChild( &m_Onglet06);
   AddChild( &m_Onglet07);
   AddChild( &m_Onglet08);
   AddChild( &m_Onglet09);
   AddChild( &m_Onglet10);
   AddChild( &m_Onglet11);
   AddChild( &m_Onglet12);
   AddChild( &m_Onglet13);
   AddChild( &m_Onglet14);
   AddChild( &m_Onglet15);

   AddChild( &mainDropZoneAddChest  );
   AddChild( &mainDropZoneRemChest  );

   AddChild( inventoryGrid );
   AddChild( junkButton);
   AddChild( dropButton);
   AddChild( iconGold );
   AddChild( statGold );
   
   AddChild( &statGoldChest );
   AddChild( &statGoldChestBtn);
  
   AddChild( &statQty );
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );


   m_iEquipIdx[ 0] = EP_BODY;
   m_iEquipIdx[ 1] = EP_LEG;
   m_iEquipIdx[ 2] = EP_FEET;
   m_iEquipIdx[ 3] = EP_GLOVE;
   m_iEquipIdx[ 4] = EP_HELM;
   m_iEquipIdx[ 5] = EP_BELT;
   m_iEquipIdx[ 6] = EP_WRIST;
   m_iEquipIdx[ 7] = EP_CAPE;
   m_iEquipIdx[ 8] = EP_FINGER;
   m_iEquipIdx[ 9] = EP_NECK;
   m_iEquipIdx[10] = EP_WEAPON_RIGHT;
   m_iEquipIdx[11] = EP_WEAPON_LEFT;
   m_iEquipIdx[12] = EP_WEAPON_TWO;
   m_iEquipIdx[13] = EP_ORBE;
   m_iEquipIdx[14] = EP_NONE;
   m_iEquipIdx[15] = EP_SEARCH;

   m_bDrawInProgress = false;
   m_bLockDraw       = false;

   m_strLastSearch[0] = 0x00;

} 

V3_GuildChestNewDlg::~V3_GuildChestNewDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_GuildChestNewDlg *V3_GuildChestNewDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_GuildChestNewDlg instance;
    return &instance;
    
} 

unsigned int V3_GuildChestNewDlg::Display( void *null)
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   // If inventory is already shown.
   if( GetInstance()->IsShown())
   {
      // Hide box
      GetInstance()->Hide();
   }
   else
   {
      GetInstance()->m_iXPos = g_Global.GetDialogLastChestNewXPos();
      GetInstance()->m_iYPos = g_Global.GetDialogLastChestNewYPos();
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      //tout ce qui est relier au inventaire vas aller dans la page vendre uniquement...


      GetInstance()->Show();
   }
   return 0;
}

void V3_GuildChestNewDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_ChestNewDlgW, iPosY+g_ChestNewDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_ChestNewDlgW, iPosY+g_ChestNewDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+727, iPosY+27, iPosX+727+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+690, iPosY+57 ) );



   ChestList.SetListDef(iPosX,iPosY,20,62,180,313,403,&ChestListScrollUp,&ChestListScrollDown);
  

   m_Onglet00.SetPos( FramePos( iPosX+29 , iPosY+62, iPosX+29 +60, iPosY+62+21 ) );
   m_Onglet01.SetPos( FramePos( iPosX+91 , iPosY+62, iPosX+91 +60, iPosY+62+21 ) );
   m_Onglet02.SetPos( FramePos( iPosX+153, iPosY+62, iPosX+153+60, iPosY+62+21 ) );
   m_Onglet03.SetPos( FramePos( iPosX+215, iPosY+62, iPosX+215+60, iPosY+62+21 ) );
   m_Onglet04.SetPos( FramePos( iPosX+277, iPosY+62, iPosX+277+60, iPosY+62+21 ) );
   m_Onglet05.SetPos( FramePos( iPosX+339, iPosY+62, iPosX+339+70, iPosY+62+21 ) );
   m_Onglet06.SetPos( FramePos( iPosX+411, iPosY+62, iPosX+411+70, iPosY+62+21 ) );
   m_Onglet07.SetPos( FramePos( iPosX+483, iPosY+62, iPosX+483+60, iPosY+62+21 ) );
   m_Onglet08.SetPos( FramePos( iPosX+545, iPosY+62, iPosX+545+60, iPosY+62+21 ) );
   m_Onglet09.SetPos( FramePos( iPosX+607, iPosY+62, iPosX+607+60, iPosY+62+21 ) );

   m_Onglet10.SetPos( FramePos( iPosX+31 , iPosY+88, iPosX+31 +90, iPosY+88+21 ) );
   m_Onglet11.SetPos( FramePos( iPosX+123, iPosY+88, iPosX+123+90, iPosY+88+21 ) );
   m_Onglet12.SetPos( FramePos( iPosX+215, iPosY+88, iPosX+215+90, iPosY+88+21 ) );
   m_Onglet13.SetPos( FramePos( iPosX+307, iPosY+88, iPosX+307+70, iPosY+88+21 ) );
   m_Onglet14.SetPos( FramePos( iPosX+379, iPosY+88, iPosX+379+70, iPosY+88+21 ) );
   m_Onglet15.SetPos( FramePos( iPosX+451, iPosY+88, iPosX+451+70, iPosY+88+21 ) );

   statQty.SetPos( FramePos( iPosX+320, iPosY+384, iPosX+433, iPosY+400 ) );
   statGoldChest.SetPos( FramePos( iPosX+317, iPosY+183, iPosX+417, iPosY+201 ) );
   statGoldChestBtn.SetPos( FramePos( iPosX+317, iPosY+183, iPosX+417, iPosY+201 ) );

   statGold->SetPos ( FramePos( iPosX+621, iPosY+367+36 , iPosX+721, iPosY+387+36) );
   iconGold->SetPos ( FramePos( iPosX+716, iPosY+366+36 , iPosX+740, iPosY+388+36) );
   instantItemInfo->SetPos( FramePos( iPosX+479, iPosY+93+36, iPosX+713, iPosY+118+36 ) );
   instantItemInfo->SetCenter( false );

   inventoryGrid  ->SetGridDef(iPosX,iPosY,20,482 ,127+36,740,358+36,&(V3_InvDlg::GetInstance()->gridScrollUpGraph) ,&(V3_InvDlg::GetInstance()->gridScrollDownGraph) );
   junkButton ->SetPos( FramePos( iPosX+482, iPosY+367+36, iPosX+482+22, iPosY+367+22+36 ) );
   dropButton ->SetPos( FramePos( iPosX+525, iPosY+367+36, iPosX+525+22, iPosY+367+22+36 ) );

   mainDropZoneAddChest  .SetPos(  FramePos( iPosX+317, iPosY+205, iPosX+436 , iPosY+261 ) );
   mainDropZoneRemChest  .SetPos(  FramePos( iPosX+317, iPosY+263, iPosX+436 , iPosY+380 ) );

}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_GuildChestNewDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   if(m_bLockDraw)
      return;
   
   m_bDrawInProgress = true;
   
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
      
      if(iNewPosX+g_ChestNewDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_ChestNewDlgW;
      if(iNewPosY+g_ChestNewDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_ChestNewDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   if(m_bNeedToUpdateList)
   {
      m_bNeedToUpdateList = FALSE;
      if(m_dwCurrentOngletSelect == EP_SEARCH && m_strLastSearch[0] != 0x00)
         UpdateSelectedEquip(m_dwCurrentOngletSelect,true,m_strLastSearch);
      else 
         UpdateSelectedEquip(m_dwCurrentOngletSelect);
   }

  
   GameUI::Draw( vsfFX );

   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   m_Onglet00.DrawTextButtonExt(hdc);
   m_Onglet01.DrawTextButtonExt(hdc);
   m_Onglet02.DrawTextButtonExt(hdc);
   m_Onglet03.DrawTextButtonExt(hdc);
   m_Onglet04.DrawTextButtonExt(hdc);
   m_Onglet05.DrawTextButtonExt(hdc);
   m_Onglet06.DrawTextButtonExt(hdc);
   m_Onglet07.DrawTextButtonExt(hdc);
   m_Onglet08.DrawTextButtonExt(hdc);
   m_Onglet09.DrawTextButtonExt(hdc);
   m_Onglet10.DrawTextButtonExt(hdc);
   m_Onglet11.DrawTextButtonExt(hdc);
   m_Onglet12.DrawTextButtonExt(hdc);
   m_Onglet13.DrawTextButtonExt(hdc);
   m_Onglet14.DrawTextButtonExt(hdc);
   m_Onglet15.DrawTextButtonExt(hdc);
   
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

   // Display the instant help text.
   if( itemUI != NULL )
   {
      //TextObject *to = itemUI->GetHelpText();
      //if( to != NULL )
      {
         
         //instantItemInfo->SetText(to->GetText(),CL_ORANGE,246);
         instantItemInfo->SetText(itemUI->strName2,CL_ORANGE,246);
         instantItemInfo->Draw( vsfFX );
      }
   }

   m_bDrawInProgress = false;
}

void V3_GuildChestNewDlg::Hide()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   BoxUI::Hide();
}





void V3_GuildChestNewDlg::StartDragDlg( MousePos mousePos )
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

void V3_GuildChestNewDlg::StopDragDlg()
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
      
      if(m_iXPos+g_ChestNewDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_ChestNewDlgW;
      if(m_iYPos+g_ChestNewDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_ChestNewDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastChestNewXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_GuildChestNewDlg::DragEvent::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_GuildChestNewDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}




void V3_GuildChestNewDlg::UpdateItemList(std::vector< cItemChest > &ItemList)
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   m_AllItemList.clear();
   vector< cItemChest >::const_iterator i;
   for( i = ItemList.begin(); i != ItemList.end(); i++ )
   {
      V3_GuildChestNewDlg::cItemChest ItemC;
      ItemC.strName       = i->strName;
      ItemC.dwAppearance  = i->dwAppearance;
      ItemC.dwID          = i->dwID;
      ItemC.dwStaticRef   = i->dwStaticRef;
      ItemC.dwQty         = i->dwQty;
      ItemC.lCharge       = i->lCharge;
      ItemC.lEquipPos     = i->lEquipPos;

      //scan to insert sorted...
      m_AllItemList.push_back(ItemC);

      if(ItemC.dwAppearance == __OBJGROUP_COINS_PILE)
      {
         //l'or on set la quantite a l'or
         char strQty[100];
         sprintf_s(strQty,"x %d",ItemC.dwQty);
         statGoldChest.SetText(strQty);
      }
      
   }
   if(m_dwCurrentOngletSelect >= 0)
      UpdateSelectedEquip(m_dwCurrentOngletSelect);
   else
      UpdateSelectedEquip(-1);
}


void V3_GuildChestNewDlg::UpdateSelectedEquip(int iID,bool bSearch, std::string strSearch)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   ResetOngletSelect();
   SetOngletSelect(iID);


  ChestList.DeleteList();

  UpdateEquipDropZone(-1);



   if(iID == -1)
   {
      return;
   }

   // Sort the vector using predicate and std::sort
   std::sort(m_AllItemList.begin(), m_AllItemList.end(), cItemChest::compareMyDataPredicate);


   char strTmp[255];
   vector< cItemChest >::const_iterator i;
   int iListPos = 0;
   COLORREF cr    = g_DefColorD;
   for( i = m_AllItemList.begin(); i != m_AllItemList.end(); i++ )
   {
      bool bAdd = false;
      if(i->lEquipPos == (unsigned int)iID && !bSearch)
      {
         bAdd = true;
      }
      else if(bSearch)
      {
         //compare les string.
         std::string strLN = i->strName;

         std::transform(strLN.begin(),strLN.end(),strLN.begin(),tolower);
         std::transform(strSearch.begin(),strSearch.end(),strSearch.begin(),tolower);
         if(strstr(strLN.c_str(),strSearch.c_str())!= NULL)
            bAdd = true;
      }
 
      if(bAdd)
      {
         ChestList.AddNewItem();

         sprintf_s(strTmp,255,"%s",i->strName.c_str());
         ListDlgUI *ListUI = new ListDlgUI;
         ListUI->SetText( strTmp, cr, 225, 0, 0 );
         ListUI->SetCenter( false );
         ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
         ListUI->optionID = strTmp;
         ListUI->dwState  = 1; 
         ListUI->dwID     = iListPos;
         ChestList.AddColumnItem( 0, ListUI );
      }
      iListPos++;
   }
   ChestList.SetScrollPos(0 );
}










void V3_GuildChestNewDlg::ChestListEvent::LeftClicked( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   
   ListDlgUI *ListUI = static_cast< ListDlgUI * >( me->ChestList.GetSelection() );
   
   if( ListUI != NULL && ListUI->dwState >0)
   {
       me->UpdateEquipDropZone(ListUI->dwID);
   }
   else
   {
       me->UpdateEquipDropZone(-1);
   }
}

void V3_GuildChestNewDlg::ChestListEvent::RightClicked( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_GuildChestNewDlg *AHUI = static_cast< V3_GuildChestNewDlg * >( GetUI() );
}

void V3_GuildChestNewDlg::TabEvent00::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[0];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent01::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[1];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent02::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[2];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent03::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[3];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent04::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[4];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent05::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[5];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent06::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[6];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent07::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[7];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent08::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[8];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent09::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[9];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent10::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[10];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent11::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[11];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent12::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[12];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent13::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[13];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent14::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[14];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_GuildChestNewDlg::TabEvent15::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );
   
   V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,g_GUILocalString[283] ,"",40,475);
   V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueDlg::GetInstance()->SetParent(me);
   V3_AskValueDlg::GetInstance()->SetXYPos(((g_ChestNewDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_ChestNewDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueDlg::GetInstance()->Show(0);
   
   me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );
}

void V3_GuildChestNewDlg::StatGoldChestBtnEvent::LeftClicked( void )
{
   V3_GuildChestNewDlg *me = static_cast< V3_GuildChestNewDlg * >( GetUI() );

   for(UINT i=0;i<me->m_AllItemList.size();i++)
   {
      if(me->m_AllItemList[i].dwAppearance == __OBJGROUP_COINS_PILE)
      {
         me->UpdateEquipDropZone(i);
         return;
      }
   }

   me->UpdateEquipDropZone(-1);



   

}

void V3_GuildChestNewDlg::ResetOngletSelect()
{
   m_Onglet00.SetSpriteId( "");
   m_Onglet01.SetSpriteId( "");
   m_Onglet02.SetSpriteId( "");
   m_Onglet03.SetSpriteId( "");
   m_Onglet04.SetSpriteId( "");
   m_Onglet05.SetSpriteId( "");
   m_Onglet06.SetSpriteId( "");
   m_Onglet07.SetSpriteId( "");
   m_Onglet08.SetSpriteId( "");
   m_Onglet09.SetSpriteId( "");
   m_Onglet10.SetSpriteId( "");
   m_Onglet11.SetSpriteId( "");
   m_Onglet12.SetSpriteId( "");
   m_Onglet13.SetSpriteId( "");
   m_Onglet14.SetSpriteId( "");
   m_Onglet15.SetSpriteId( "");
}

void V3_GuildChestNewDlg::SetOngletSelect(int iID)
{
   switch(iID)
   {
      case  EP_BODY :         m_Onglet00.SetSpriteId( "V3_MainOnglet_60p_H"); break;
      case  EP_LEG :          m_Onglet01.SetSpriteId( "V3_MainOnglet_60p_H"); break;
      case  EP_FEET :         m_Onglet02.SetSpriteId( "V3_MainOnglet_60p_H"); break;
      case  EP_GLOVE :        m_Onglet03.SetSpriteId( "V3_MainOnglet_60p_H"); break;
      case  EP_HELM :         m_Onglet04.SetSpriteId( "V3_MainOnglet_60p_H"); break;
      case  EP_BELT :         m_Onglet05.SetSpriteId( "V3_MainOnglet_70p_H"); break;
      case  EP_WRIST :        m_Onglet06.SetSpriteId( "V3_MainOnglet_70p_H"); break;
      case  EP_CAPE :         m_Onglet07.SetSpriteId( "V3_MainOnglet_60p_H"); break;
      case  EP_FINGER :       m_Onglet08.SetSpriteId( "V3_MainOnglet_60p_H"); break;
      case  EP_NECK :         m_Onglet09.SetSpriteId( "V3_MainOnglet_60p_H"); break;
      case  EP_WEAPON_RIGHT:  m_Onglet10.SetSpriteId( "V3_MainOnglet_90p_H"); break;
      case  EP_WEAPON_LEFT:   m_Onglet11.SetSpriteId( "V3_MainOnglet_90p_H"); break;
      case  EP_WEAPON_TWO:    m_Onglet12.SetSpriteId( "V3_MainOnglet_90p_H"); break;
      case  EP_ORBE:          m_Onglet13.SetSpriteId( "V3_MainOnglet_70p_H"); break;
      case  EP_NONE:          m_Onglet14.SetSpriteId( "V3_MainOnglet_70p_H"); break;
      case  EP_SEARCH:        m_Onglet15.SetSpriteId( "V3_MainOnglet_70p_H"); break;
   }
}

void V3_GuildChestNewDlg::AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey)
{
   if(!bOK)
      return;
   
   switch(uiShowCodeP)
   {
      case 0:
      {
         m_dwCurrentOngletSelect = m_iEquipIdx[15];
         sprintf_s(m_strLastSearch,512,"%s",pStrValue1);
         UpdateSelectedEquip(m_dwCurrentOngletSelect,true,m_strLastSearch);
      }
      break;
   }
}


void V3_GuildChestNewDlg::Reload()
{
   UpdateEquipDropZone(-1);

   inventoryGrid          =  &(V3_InvDlg::GetInstance()->inventoryGrid) ;
   invGridScrollDownGraph =  &(V3_InvDlg::GetInstance()->gridScrollDownGraph) ;
   invGridScrollUpGraph   =  &(V3_InvDlg::GetInstance()->gridScrollUpGraph) ;
   iconGold               =  &(V3_InvDlg::GetInstance()->iconGold) ;
   statGold               =  &(V3_InvDlg::GetInstance()->statGold) ;
   instantItemInfo        =  &(V3_InvDlg::GetInstance()->instantItemInfo);

}

void V3_GuildChestNewDlg::ClearItem()
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   UpdateEquipDropZone(-1);

   
   m_AllItemList.clear();
   ChestList.DeleteList();
}


//  Dragged something from the chest
void V3_GuildChestNewDlg::ChestGridEvent::Dragged( void )
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

void V3_GuildChestNewDlg::ChestGridEvent::RightClicked(void)
{
   V3_GuildChestNewDlg *me = V3_GuildChestNewDlg::GetInstance();

   ItemUI *pItem = static_cast< ItemUI * >( me->mainDropZoneRemChest.GetItem() );

   if(pItem)
   {
      g_Global.ResetItemInfo();
      g_Global.SetItemInfoID(pItem->bagItem->dwID);
      g_Global.SetItemInfoInventory(true);
      TFCPacket sending2;
      sending2 << (RQ_SIZE)RQ_QueryItemInfo;
      sending2 << (long)pItem->bagItem->dwID;
      SEND_PACKET( sending2 );
   }
   
}  

// Verify that the dropped item is ok.
// Return: bool, true if dropped item is ok, false otherwise.
bool V3_GuildChestNewDlg::ChestGridEvent::Dropping( void )
{

   // If the source event is the chest event.
   if( GetUI()->GetDragSourceEvent() == this )
   {
      return true;
   }
   else
   {        
      V3_GuildChestNewDlg *me = V3_GuildChestNewDlg::GetInstance();


      ItemUI *itemUI = static_cast< ItemUI * >( GetDragItem() );

      // Retreive the bag item ID.
      DWORD itemID = itemUI->bagItem->dwID;
      dropSound->Play( TRUE );

      me->m_pItemUIReq = itemUI;

      DropBack( GetDragItem() );

      if( itemUI->bagItem->dwQty == 1 )
      {
         me->m_pItemUIReq = itemUI;   
         me->AskValueReturnSld(true,g_AskBackpackToChest,NULL,1);
         return false;
      }

      V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[38] ,itemUI->bagItem->dwQty,0);
      V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
      V3_AskValueSldDlg::GetInstance()->SetParent(me);
      V3_AskValueSldDlg::GetInstance()->SetDelay(500);
      V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_ChestDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_ChestDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
      V3_AskValueSldDlg::GetInstance()->Show(g_AskBackpackToChest,0);

      //Avait un delais, 
      me->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );


   }


   // Never let an item be dropped in the chest box, its handling will
   // be done by the packets.

   return false;
}

void V3_GuildChestNewDlg::AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal)
{
   if(!bOK || iSldVal == 0)
      return;

   DWORD dwQty = iSldVal;

   switch(uiShowCodeP)
   {
      case g_AskChestToBackpack:
      {
         RequestMoveItemFromChestToBackpack(m_pItemUIReq->bagItem->dwID, dwQty);
      }      
      break;
      case g_AskBackpackToChest:
      {
         RequestMoveItemFromBackpackToChest(m_pItemUIReq->bagItem->dwID, dwQty);
      }      
      break;
   }
}

void V3_GuildChestNewDlg::RequestMoveItemFromChestToBackpack(DWORD dwItemID,DWORD dwQty)
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_NM_GUILDChestRemoveItem;
   packet << (long)dwItemID;
   packet << (long)dwQty;
   SEND_PACKET( packet );

   UpdateEquipDropZone(-1);
}


//  Setups the given item UI according to its binded BAG_ITEM data.
void V3_GuildChestNewDlg::RequestMoveItemFromBackpackToChest(DWORD dwItemID,DWORD dwQty)
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_NM_GUILDChestAddItem;
   packet << (long)dwItemID;
   packet << (long)dwQty;
   SEND_PACKET( packet );
}


void V3_GuildChestNewDlg::UpdateEquipDropZone(int iListPos)
{
   if( mainDropZoneRemChest.GetItem() != NULL )
   {
      // Reset the drop zone's UI.
      delete mainDropZoneRemChest.GetItem();            
      mainDropZoneRemChest.SetItem( NULL );            
   }

   statQty.SetText("");

   // If there is an equipped item at this position.
   if( iListPos >= 0 )
   {
      if(m_pDisplayBagItem)
         delete m_pDisplayBagItem;
      m_pDisplayBagItem = NULL;

      m_pDisplayBagItem = new BAG_ITEM;
      m_pDisplayBagItem->nX         = -1;	// assign random pos to item
      m_pDisplayBagItem->nY         = -1;
      m_pDisplayBagItem->dwID       = m_AllItemList[iListPos].dwID;
      m_pDisplayBagItem->chIName[0] = 0;
      m_pDisplayBagItem->Type       = (WORD)m_AllItemList[iListPos].dwAppearance;
      m_pDisplayBagItem->lpSprite   = InvItemIcons(  m_AllItemList[iListPos].dwAppearance );		// TODO
      m_pDisplayBagItem->wBaseID    = (WORD)m_AllItemList[iListPos].dwStaticRef;
      m_pDisplayBagItem->dwQty      = m_AllItemList[iListPos].dwQty;
      m_pDisplayBagItem->iCharges   = m_AllItemList[iListPos].lCharge;
      m_pDisplayBagItem->chTarget   = 0;
      m_pDisplayBagItem->chAttack   = 0;
      m_pDisplayBagItem->chPVP      = 0;
 

      // Create a new UI.
      ItemUI *newUI = new ItemUI;
      // Set its bag item.
      newUI->bagItem = m_pDisplayBagItem;
      newUI->bagItem->associatedUI = newUI;
      newUI->SetVSF( m_pDisplayBagItem->lpSprite );
      newUI->SetPos( FramePos( 0, 0, m_pDisplayBagItem->lpSprite->GetWidth(),m_pDisplayBagItem->lpSprite->GetHeight() ));
      // Set the drop zone's item
      mainDropZoneRemChest.SetItem( newUI );

      char strQty[100];
      sprintf_s(strQty,"x %d",m_pDisplayBagItem->dwQty);
      statQty.SetText(strQty);
   }
}

