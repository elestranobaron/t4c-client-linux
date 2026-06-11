// V3_AHDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"
 
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "V3_AHDlg.h" 
#include "V3_ChatLogDlg.h" 
#include "V3_AskValueDlg.h"
#include "V3_AskValueSldDlg.h" 
#include "V3_AHVendreDlg.h"
 
extern CSaveGame g_SaveGame; 
#define V2IMAGE_GMNMS_DATA       "\\Game Files\\gmnmb.bin"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_AHDlg::V3_AHDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
SoldList( soldListEvent ),
vendreButton( vendreButtonEvent,g_GUILocalString[292],g_DefColorH ),
buyNowButton( buyNowButtonEvent,g_GUILocalString[388],g_DefColorH ),
buyNowNMSButton( buyNowNMSButtonEvent,g_GUILocalString[389],g_DefColorH ),
bidButton   ( bidButtonEvent   ,g_GUILocalString[287],g_DefColorH ),
cancelButton( cancelButtonEvent,g_GUILocalString[286],g_DefColorH ),
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
m_dwCurrentOngletSelect(0)
{
	m_bNMSOptions = FALSE;
	char strPathFile[1024];
	ClientConfig *pConfig = g_Global.GetClientConfig();
	sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGE_GMNMS_DATA);
	FILE *pfGMD = NULL;
	fopen_s(&pfGMD,strPathFile,"rb");
	if(pfGMD)
	{
		fclose(pfGMD);
		m_bNMSOptions = TRUE;
	}

   int i=0;
   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastAHXPos();
   m_iYPos = g_Global.GetDialogLastAHYPos();
   
   m_BackgroundS.SetSpriteId("V3_AH3Back",true,"V3_AH3BackMask");
   

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_AHIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
  

   buyNowButton.SetSpriteId   ( "V3_Btn130x24_N" );
   buyNowButton.SetDownUI     ( "V3_Btn130x24_D" );
   buyNowButton.SetHighlightUI( "V3_Btn130x24_H" );
   buyNowButtonEvent.SetUI( this );
   SetHelpId( buyNowButton, 807 );

   buyNowNMSButton.SetSpriteId   ( "V3_Btn130x24_N" );
   buyNowNMSButton.SetDownUI     ( "V3_Btn130x24_D" );
   buyNowNMSButton.SetHighlightUI( "V3_Btn130x24_H" );
   buyNowNMSButtonEvent.SetUI( this );
   SetHelpId( buyNowNMSButton, 808 );
   
   cancelButton.SetSpriteId   ( "V3_Btn72x24_N" );
   cancelButton.SetDownUI     ( "V3_Btn72x24_D" );
   cancelButton.SetHighlightUI( "V3_Btn72x24_H" );
   cancelButtonEvent.SetUI( this );
   SetHelpId( cancelButton, 649 );
   
   bidButton.SetSpriteId   ( "V3_Btn72x24_N" );
   bidButton.SetDownUI     ( "V3_Btn72x24_D" );
   bidButton.SetHighlightUI( "V3_Btn72x24_H" );
   bidButtonEvent.SetUI( this );
   SetHelpId( bidButton, 650 );
   
   vendreButton.SetSpriteId   ( "V3_Btn72x24_N" );
   vendreButton.SetDownUI     ( "V3_Btn72x24_D" );
   vendreButton.SetHighlightUI( "V3_Btn72x24_H" );
   vendreButtonEvent.SetUI( this );
   SetHelpId( vendreButton, 641 );


   buyNowButton.Hide();
   buyNowNMSButton.Hide();
   bidButton   .Hide();
   cancelButton.Hide();

   //Liste des metiers...
   SoldList.SetupList( 7, 29, FramePos( 0, 0, 1, 0 ) );
   SoldList.SetColumnSize( 0, 249 );
   SoldList.SetColumnSize( 1, 79 ); 
   SoldList.SetColumnSize( 2, 67 ); 
   SoldList.SetColumnSize( 3, 81 ); 
   SoldList.SetColumnSize( 4, 43 );
   SoldList.SetColumnSize( 5, 81 ); 
   SoldList.SetColumnSize( 6, 81 ); 
   
   SoldListSelGraph.SetPos( FramePos( 0, 0, 687, 29 ) );
   SoldListSelGraph.SetSpriteId( "V3_AH3Select" );
   SoldListSelGraph.Show();
   SoldList.SetSelectionSprite( &SoldListSelGraph, 0);
   SoldList.SetScrollDownDownUI( &SoldListScrollDown );
   SoldList.SetScrollUpDownUI( &SoldListScrollUp );
   soldListEvent.SetUI( this );

     
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

   m_stCol1.SetText(g_GUILocalString[297],g_DefColorH);
   m_stCol2.SetText(g_GUILocalString[298],g_DefColorH);
   m_stCol3.SetText(g_GUILocalString[299],g_DefColorH);
   m_stCol4.SetText(g_GUILocalString[300],g_DefColorH);

   if(m_bNMSOptions)
      m_stCol5.SetText(g_GUILocalString[387],g_DefColorH);
   else
      m_stCol5.SetText("",g_DefColorH);

   m_stCol6.SetText(g_GUILocalString[301],g_DefColorH);
   m_stCol7.SetText(g_GUILocalString[302],g_DefColorH);

  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);

   AddChild( &SoldList );
   AddChild( &buyNowButton);
   if(m_bNMSOptions)
      AddChild( &buyNowNMSButton);
   AddChild( &cancelButton);
   AddChild( &bidButton   );
   AddChild( &vendreButton);


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
   AddChild( &m_stPlayerGold);

   AddChild( &m_stCol1);
   AddChild( &m_stCol2);
   AddChild( &m_stCol3);
   AddChild( &m_stCol4);
   AddChild( &m_stCol5);
   AddChild( &m_stCol6);
   AddChild( &m_stCol7);




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

} 

V3_AHDlg::~V3_AHDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_AHDlg *V3_AHDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_AHDlg instance;
    return &instance;
    
} 

unsigned int V3_AHDlg::Display( void *null)
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
      GetInstance()->m_iXPos = g_Global.GetDialogLastAHXPos();
      GetInstance()->m_iYPos = g_Global.GetDialogLastAHYPos();
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      //tout ce qui est relier au inventaire vas aller dans la page vendre uniquement...


      GetInstance()->Show();
   }
   return 0;
}

void V3_AHDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_AHDlgW, iPosY+g_AHDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_AHDlgW, iPosY+g_AHDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+727, iPosY+27, iPosX+727+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+690, iPosY+57 ) );



   SoldList.SetListDef(iPosX,iPosY,20,36,137,744,397,&SoldListScrollUp,&SoldListScrollDown);
   vendreButton.SetPos( FramePos( iPosX+238, iPosY+405, iPosX+238+72, iPosY+405+24 ) );
   cancelButton.SetPos( FramePos( iPosX+320, iPosY+405, iPosX+320+72, iPosY+405+24 ) );
   buyNowButton   .SetPos( FramePos( iPosX+459, iPosY+405, iPosX+459+130, iPosY+405+24 ) );
   buyNowNMSButton.SetPos( FramePos( iPosX+594, iPosY+405, iPosX+594+130, iPosY+405+24 ) );
   bidButton   .SetPos( FramePos( iPosX+382, iPosY+405, iPosX+382+72, iPosY+405+24 ) );
   

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

   m_stPlayerGold.SetPos( FramePos( iPosX+91,iPosY+408, iPosX+189, iPosY+425 ) );

   m_stCol1.SetPos( FramePos( iPosX+37 ,iPosY+115, iPosX+283, iPosY+132 ) );
   m_stCol2.SetPos( FramePos( iPosX+287,iPosY+115, iPosX+363, iPosY+132 ) );
   m_stCol3.SetPos( FramePos( iPosX+367,iPosY+115, iPosX+431, iPosY+132 ) );
   m_stCol4.SetPos( FramePos( iPosX+435,iPosY+115, iPosX+513, iPosY+132 ) );
   m_stCol5.SetPos( FramePos( iPosX+517,iPosY+115, iPosX+557, iPosY+132 ) );
   m_stCol6.SetPos( FramePos( iPosX+561,iPosY+115, iPosX+639, iPosY+132 ) );
   m_stCol7.SetPos( FramePos( iPosX+643,iPosY+115, iPosX+721, iPosY+132 ) );
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_AHDlg::Draw(V2SPRITEFX *vsfFX)
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
      
      if(iNewPosX+g_AHDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_AHDlgW;
      if(iNewPosY+g_AHDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_AHDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   char strTmp[100];
   sprintf_s(strTmp,100,"%d",Player.Gold);
   m_stPlayerGold.SetText(strTmp,g_DefColorD);

   GameUI::Draw( vsfFX );

   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   vendreButton.DrawTextButtonExt(hdc);
   buyNowButton.DrawTextButtonExt(hdc);
   buyNowNMSButton.DrawTextButtonExt(hdc);
   bidButton.DrawTextButtonExt(hdc);
   cancelButton.DrawTextButtonExt(hdc);
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
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[268],strlen(g_GUILocalString[268]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);

   m_bDrawInProgress = false;
}

void V3_AHDlg::Hide()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   BoxUI::Hide();
}





void V3_AHDlg::StartDragDlg( MousePos mousePos )
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

void V3_AHDlg::StopDragDlg()
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
      
      if(m_iXPos+g_AHDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_AHDlgW;
      if(m_iYPos+g_AHDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_AHDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastAHXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_AHDlg::DragEvent::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_AHDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}




void V3_AHDlg::UpdateItemList(DWORD dwPID,const std::vector< ItemSold > &ItemList)
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   m_AllItemList.clear();
   
   m_dwPlayerID = dwPID;
   
   vector< ItemSold >::const_iterator i;
   int iCnt = 0;
   for( i = ItemList.begin(); i != ItemList.end(); i++ )
   {
      V3_AHDlg::ItemSold ItemS;
      ItemS.strName    = i->strName;
      ItemS.dwIndex    = i->dwIndex;
      ItemS.dwOwnerID  = i->dwOwnerID;
      ItemS.dwQty      = i->dwQty;
      ItemS.dwTime     = i->dwTime;
      ItemS.dwBuy      = i->dwBuy;
      ItemS.dwBuyNMS   = i->dwBuyNMS;
      ItemS.dwBid      = i->dwBid;
      ItemS.dwCurBid   = i->dwCurBid;
      ItemS.dwCurBidID = i->dwCurBidID;
      ItemS.dwEquipPos = i->dwEquipPos;
      ItemS.dwTimeEnter= i->dwTimeEnter;
      m_AllItemList.push_back(ItemS);
      iCnt++;
      
   }
   
   if(m_dwCurrentOngletSelect >= 0)
      UpdateSelectedEquip(m_dwCurrentOngletSelect);
   else
      UpdateSelectedEquip(-1);
   
}

void V3_AHDlg::UpdateSelectedEquip(int iID,bool bSearch, std::string strSearch)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   ResetOngletSelect();
   SetOngletSelect(iID);

   buyNowButton.Hide();
   buyNowNMSButton.Hide();
   bidButton.Hide();
   cancelButton.Hide();

   SoldList.DeleteList();

   if(iID == -1)
   {
      return;
   }

   char strTmp[255];
   vector< ItemSold >::const_iterator i;
   int iListPos = 0;
   for( i = m_AllItemList.begin(); i != m_AllItemList.end(); i++ )
   {
      COLORREF cr    = g_DefColorD;
      COLORREF crBid = g_DefColorD;

      bool bAdd = false;
      if(i->dwEquipPos == (unsigned int)iID && !bSearch)
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
         if(i->dwOwnerID == m_dwPlayerID)
            cr = CL_GREEN;
         if(i->dwCurBidID == m_dwPlayerID)
            crBid = CL_GREEN;

         SoldList.AddNewItem();

         sprintf_s(strTmp,255,"%s",i->strName.c_str());
         ListDlgUI *ListUI = new ListDlgUI;
         ListUI->SetText( strTmp, cr, 245, 0, 0 );
         ListUI->SetCenter( false );
         ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
         ListUI->optionID = strTmp;
         ListUI->dwState  = 1; 
         ListUI->dwID     = iListPos;
         SoldList.AddColumnItem( 0, ListUI );

         sprintf_s(strTmp,255,"%d",i->dwQty);
         ListUI = new ListDlgUI;
         ListUI->SetText( strTmp, cr, 640, 0, 0 );
         ListUI->SetCenter( false );
         ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
         ListUI->optionID = strTmp;
         ListUI->dwState  = 1; 
         ListUI->dwID     = iListPos;
         SoldList.AddColumnItem( 1, ListUI );

         int iNbrDay = i->dwTime/86400;
         int iNbrH   = (i->dwTime-(iNbrDay*86400))/3600;
         int iNbrM   = ((i->dwTime-(iNbrDay*86400))-(iNbrH*3600))/60;

         sprintf_s(strTmp,255,"%dd %dh%d",iNbrDay,iNbrH,iNbrM);
         ListUI = new ListDlgUI;
         ListUI->SetText( strTmp, cr, 640, 0, 0 );
         ListUI->SetCenter( false );
         ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
         ListUI->optionID = strTmp;
         ListUI->dwState  = 1; 
         ListUI->dwID     = iListPos;
         SoldList.AddColumnItem( 2, ListUI );

         sprintf_s(strTmp,255,"%d",i->dwBuy);
         ListUI = new ListDlgUI;
         ListUI->SetText( strTmp, cr, 640, 0, 0 );
         ListUI->SetCenter( false );
         ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
         ListUI->optionID = strTmp;
         ListUI->dwState  = 1; 
         ListUI->dwID     = iListPos;
         SoldList.AddColumnItem( 3, ListUI );

         if(m_bNMSOptions)
            sprintf_s(strTmp,255,"%d",i->dwBuyNMS);
         else
            sprintf_s(strTmp,255,"");
         ListUI = new ListDlgUI;
         ListUI->SetText( strTmp, cr, 640, 0, 0 );
         ListUI->SetCenter( false );
         ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
         ListUI->optionID = strTmp;
         ListUI->dwState  = 1; 
         ListUI->dwID     = iListPos;
         SoldList.AddColumnItem( 4, ListUI );

         sprintf_s(strTmp,255,"%d",i->dwBid);
         ListUI = new ListDlgUI;
         ListUI->SetText( strTmp, cr, 640, 0, 0 );
         ListUI->SetCenter( false );
         ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
         ListUI->optionID = strTmp;
         ListUI->dwState  = 1; 
         ListUI->dwID     = iListPos;
         SoldList.AddColumnItem( 5, ListUI );

         sprintf_s(strTmp,255,"%d",i->dwCurBid);
         ListUI = new ListDlgUI;
         ListUI->SetText( strTmp, crBid, 640, 0, 0 );
         ListUI->SetCenter( false );
         ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
         ListUI->optionID = strTmp;
         ListUI->dwState  = 1; 
         ListUI->dwID     = iListPos;
         SoldList.AddColumnItem( 6, ListUI );
      }
      iListPos++;
   }

   SoldList.SetScrollPos(0 );
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_AHDlg::VendreButtonEvent::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   
   V3_AHVendreDlg::GetInstance()->Display(NULL);
   me->RequestForegroundControl( V3_AHVendreDlg::GetInstance() );
     
}

void V3_AHDlg::CancelButtonEvent::LeftClicked( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   
   ListDlgUI *ListUI = static_cast< ListDlgUI * >( me->SoldList.GetSelection() );
   
   if( ListUI != NULL && ListUI->dwState >0)
   {
      if(me->m_AllItemList[ListUI->dwID].dwOwnerID == me->m_dwPlayerID)
      {
         TFCPacket sending;
         sending << (RQ_SIZE)RQ_NM_CancelAHItems;
         sending << (long)me->m_AllItemList[ListUI->dwID].dwIndex;
         sending << (long)me->m_AllItemList[ListUI->dwID].dwTimeEnter;
         // Send the packet.
         SEND_PACKET( sending );
      }
   }
}

void V3_AHDlg::BuyNowButtonEvent::LeftClicked( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   
   ListDlgUI *ListUI = static_cast< ListDlgUI * >( me->SoldList.GetSelection() );
   if( ListUI != NULL && ListUI->dwState >0)
   {
      if(me->m_AllItemList[ListUI->dwID].dwOwnerID != me->m_dwPlayerID)
      {
         TFCPacket sending;
         
         DWORD dwPrix = me->m_AllItemList[ListUI->dwID].dwBuy; //pas besoin du prix en achat c le montant demander...
         // sera valider par le serveur...
         BYTE  chBuy = 1;
         sending << (RQ_SIZE)RQ_NM_BuyAHItems;
         sending << (long)me->m_AllItemList[ListUI->dwID].dwIndex;
         sending << (char)chBuy;
         sending << (long)dwPrix;
         sending << (long)0; //Prix NMS
         sending << (long)me->m_AllItemList[ListUI->dwID].dwTimeEnter;
         
         // Send the packet.
         SEND_PACKET( sending );
      }
   }
}

void V3_AHDlg::BuyNowNMSButtonEvent::LeftClicked( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );

   ListDlgUI *ListUI = static_cast< ListDlgUI * >( me->SoldList.GetSelection() );
   if( ListUI != NULL && ListUI->dwState >0)
   {
      if(me->m_AllItemList[ListUI->dwID].dwOwnerID != me->m_dwPlayerID)
      {
         TFCPacket sending;

         DWORD dwPrix = me->m_AllItemList[ListUI->dwID].dwBuyNMS; //pas besoin du prix en achat c le montant demander...
         // sera valider par le serveur...
         BYTE  chBuy = 1;
         sending << (RQ_SIZE)RQ_NM_BuyAHItems;
         sending << (long)me->m_AllItemList[ListUI->dwID].dwIndex;
         sending << (char)chBuy;
         sending << (long)0;
         sending << (long)dwPrix; //Prix NMS
         sending << (long)me->m_AllItemList[ListUI->dwID].dwTimeEnter;

         // Send the packet.
         SEND_PACKET( sending );
      }
   }
}

void V3_AHDlg::BidButtonEvent::LeftClicked( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   
   
   ListDlgUI *ListUI = static_cast< ListDlgUI * >( me->SoldList.GetSelection() );
   if( ListUI != NULL && ListUI->dwState >0)
   {
      if(me->m_AllItemList[ListUI->dwID].dwOwnerID != me->m_dwPlayerID)
      {
         int iMiseMin = 1;
         if(me->m_AllItemList[ListUI->dwID].dwCurBid < me->m_AllItemList[ListUI->dwID].dwBid)
            iMiseMin = me->m_AllItemList[ListUI->dwID].dwBid;
         else
            iMiseMin = me->m_AllItemList[ListUI->dwID].dwCurBid+1;

         me->m_dwIndexItemBuy = me->m_AllItemList[ListUI->dwID].dwIndex;
         me->m_dwTSItemBuy    = me->m_AllItemList[ListUI->dwID].dwTimeEnter;

         V3_AskValueSldDlg::GetInstance()->SetDialogText(g_GUILocalString[344] ,Player.Gold,648,iMiseMin);
         V3_AskValueSldDlg::GetInstance()->SetButtonText(g_GUILocalString[294] ,g_GUILocalString[11],646,647);
         V3_AskValueSldDlg::GetInstance()->SetParent(me);
         V3_AskValueSldDlg::GetInstance()->SetDelay(500);
         V3_AskValueSldDlg::GetInstance()->SetXYPos(((g_AHDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_AHDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
         V3_AskValueSldDlg::GetInstance()->Show(0,iMiseMin);
         
         me->RequestForegroundControl( V3_AskValueSldDlg::GetInstance() );
      }
   }
}

 

void V3_AHDlg::SoldListEvent::LeftClicked( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   
   ListDlgUI *ListUI = static_cast< ListDlgUI * >( me->SoldList.GetSelection() );
   
   if( ListUI != NULL && ListUI->dwState >0)
   {
      if(me->m_AllItemList[ListUI->dwID].dwOwnerID == me->m_dwPlayerID)
      {
         me->buyNowButton.Hide();
         me->buyNowNMSButton.Hide();
         me->bidButton.Hide();
         me->cancelButton.Show();
      }
      else
      {
         me->cancelButton.Hide();
         
         if(me->m_AllItemList[ListUI->dwID].dwBuyNMS >0 && me->m_bNMSOptions)
            me->buyNowNMSButton.Show();
         else
            me->buyNowNMSButton.Hide();

         if(me->m_AllItemList[ListUI->dwID].dwBuy >0)
            me->buyNowButton.Show();
         else
            me->buyNowButton.Hide();

         if(me->m_AllItemList[ListUI->dwID].dwBid >0)
            me->bidButton.Show();
         else
            me->bidButton.Hide();
      }
   }
   else
   {
      me->buyNowButton.Hide();
      me->buyNowNMSButton.Hide();
      me->bidButton.Hide();
      me->cancelButton.Hide();
   }
}

void V3_AHDlg::SoldListEvent::RightClicked( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_AHDlg *AHUI = static_cast< V3_AHDlg * >( GetUI() );
   
   ListDlgUI *ListUI = static_cast< ListDlgUI * >( AHUI->SoldList.GetSelection() );
   
   if( ListUI != NULL && ListUI->dwState >0)
   {
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_NM_InfoAHItems;
      sending << (long)AHUI->m_AllItemList[ListUI->dwID].dwIndex;
      SEND_PACKET( sending );
   }
}

void V3_AHDlg::TabEvent00::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[0];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent01::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[1];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent02::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[2];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent03::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[3];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent04::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[4];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent05::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[5];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent06::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[6];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent07::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[7];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent08::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[8];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent09::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[9];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent10::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[10];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent11::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[11];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent12::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[12];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent13::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[13];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent14::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   me->m_dwCurrentOngletSelect = me->m_iEquipIdx[14];
   me->UpdateSelectedEquip(me->m_dwCurrentOngletSelect);
}

void V3_AHDlg::TabEvent15::LeftClicked( void )
{
   V3_AHDlg *me = static_cast< V3_AHDlg * >( GetUI() );
   
   V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,g_GUILocalString[283] ,"",40,475);
   V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueDlg::GetInstance()->SetParent(me);
   V3_AskValueDlg::GetInstance()->SetXYPos(((g_AHDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_AHDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueDlg::GetInstance()->Show(0);
   
   me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );
}

void V3_AHDlg::ResetOngletSelect()
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

void V3_AHDlg::SetOngletSelect(int iID)
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

void V3_AHDlg::AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey)
{
   if(!bOK)
      return;
   
   switch(uiShowCodeP)
   {
      case 0:
      {
         m_dwCurrentOngletSelect = m_iEquipIdx[15];
         UpdateSelectedEquip(m_dwCurrentOngletSelect,true,pStrValue1);
      }
      break;
   }
}


void V3_AHDlg::AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal)
{
   if(!bOK || iSldVal == 0)
      return;
   
   
   switch(uiShowCodeP)
   {
      case 0:
      {
         BYTE  chBuy = 0; //on Bid...
         
         TFCPacket sending;
         sending << (RQ_SIZE)RQ_NM_BuyAHItems;
         sending << (long)m_dwIndexItemBuy;
         sending << (char)chBuy;
         sending << (long)iSldVal;
         sending << (long)0; //Prix NMS
         sending << (long)m_dwTSItemBuy;
         // Send the packet.
         SEND_PACKET( sending );       
      }      
      break;
   }
}