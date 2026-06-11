// V3_BoutiqueDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_BoutiqueDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_PanierDlg.h"
#include "V3_ConfirmDlg.h"
#include "V3_InvDlg.h"
#include "V3_DisplaySpecialDlg.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;

extern T3VSBSound SoundFX[250];
extern Font *fSmallNormal_12;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_BoutiqueDlg::V3_BoutiqueDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
achatList( achatListEvent ),
AchatOpt1Button( achatOpt1ButtonEvent,g_LocalString[656],g_DefColorH ),
AchatOpt2Button( achatOpt2ButtonEvent,g_LocalString[658],g_DefColorH ),
AchatOpt3Button( achatOpt3ButtonEvent,g_LocalString[657],g_DefColorH ),
AchatOpt4Button( achatOpt4ButtonEvent,g_LocalString[659],g_DefColorH ),
AchatShowMinionSkin( achatShowMinionSkinEvent,g_LocalString[745],g_DefColorH ),
AcheterButton( acheterButtonEvent,g_LocalString[660],g_DefColorH ),
PanierButton( panierButtonEvent,g_LocalString[669],g_DefColorH )
{
   int i=0;
   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_BoutiqueBack",true,"V3_BoutiqueBackMask");

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
   


   AchatOpt1Button.SetSpriteId   ( "V3_Btn130x24_N" );
   AchatOpt1Button.SetDownUI     ( "V3_Btn130x24_D" );
   AchatOpt1Button.SetHighlightUI( "V3_Btn130x24_H" );
   achatOpt1ButtonEvent.SetUI( this );
   SetHelpId( AchatOpt1Button, 662 );
   
   AchatOpt2Button.SetSpriteId   ( "V3_Btn130x24_N" );
   AchatOpt2Button.SetDownUI     ( "V3_Btn130x24_D" );
   AchatOpt2Button.SetHighlightUI( "V3_Btn130x24_H" );
   achatOpt2ButtonEvent.SetUI( this );
   SetHelpId( AchatOpt2Button, 664 );
   
   AchatOpt3Button.SetSpriteId   ( "V3_Btn130x24_N" );
   AchatOpt3Button.SetDownUI     ( "V3_Btn130x24_D" );
   AchatOpt3Button.SetHighlightUI( "V3_Btn130x24_H" );
   achatOpt3ButtonEvent.SetUI( this );
   SetHelpId( AchatOpt3Button, 665 );
   
   AchatOpt4Button.SetSpriteId   ( "V3_Btn130x24_N" );
   AchatOpt4Button.SetDownUI     ( "V3_Btn130x24_D" );
   AchatOpt4Button.SetHighlightUI( "V3_Btn130x24_H" );
   achatOpt4ButtonEvent.SetUI( this );
   SetHelpId( AchatOpt4Button, 663 );
   AchatOpt4Button.Hide();

   AchatShowMinionSkin.SetSpriteId   ( "V3_Btn130x24_N" );
   AchatShowMinionSkin.SetDownUI     ( "V3_Btn130x24_D" );
   AchatShowMinionSkin.SetHighlightUI( "V3_Btn130x24_H" );
   achatShowMinionSkinEvent.SetUI( this );
   SetHelpId( AchatShowMinionSkin, 746 );

   
   
   
   AcheterButton.SetSpriteId   ( "V3_Btn92x24_N" );
   AcheterButton.SetDownUI     ( "V3_Btn92x24_D" );
   AcheterButton.SetHighlightUI( "V3_Btn92x24_H" );
   acheterButtonEvent.SetUI( this );
   SetHelpId( AcheterButton, 673 );
   
   PanierButton.SetSpriteId   ( "V3_Btn92x24_N" );
   PanierButton.SetDownUI     ( "V3_Btn92x24_D" );
   PanierButton.SetHighlightUI( "V3_Btn92x24_H" );
   panierButtonEvent.SetUI( this );
   SetHelpId( PanierButton, 670 );
   
  
   m_stHEcu.SetText(g_LocalString[661],g_DefColorH);
   achatDescription.SetCenter( false );

   m_sthCol1.SetText(g_LocalString[666],g_DefColorH);
   m_sthCol2.SetText(g_LocalString[667],g_DefColorH);

   achatList.SetupList( 2, 20, FramePos( 0, 0, 2, 1 ) );
   achatList.SetColumnSize( 0, 270 );
   achatList.SetColumnSize( 1, 80 );
   achatListingSelect.SetPos( FramePos( 0, 0, 352, 20 ) );
   achatListingSelect.SetSpriteId( "V3_BoutiqueSelect" );
   achatListingSelect.Show();
   achatList.SetSelectionSprite( &achatListingSelect, 0);
   achatListEvent.SetUI( this );
   achatList.SetScrollDownDownUI( &achatListScrollDown );
   achatList.SetScrollUpDownUI( &achatListScrollUp );
  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &AchatOpt1Button );
   AddChild( &AchatOpt2Button );
   AddChild( &AchatOpt3Button );
   AddChild( &AchatOpt4Button );
   AddChild( &AchatShowMinionSkin);
   AddChild( &achatList );
   AddChild( &AcheterButton );
   AddChild( &PanierButton );
   AddChild( &m_stHEcu );
   AddChild( &m_stEcu );
   AddChild( &m_sthCol1);
   AddChild( &m_sthCol2);



   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );


   m_strDescription[0] = 0x00;
} 

V3_BoutiqueDlg::~V3_BoutiqueDlg()
{
   
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_BoutiqueDlg *V3_BoutiqueDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_BoutiqueDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_BoutiqueDlg::Display( void *null)
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

void V3_BoutiqueDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );


   AchatOpt1Button    .SetPos( FramePos( iPosX+433, iPosY+176, iPosX+433+130, iPosY+176+24 ) );
   AchatOpt2Button    .SetPos( FramePos( iPosX+433, iPosY+205, iPosX+433+130, iPosY+205+24 ) );
   AchatOpt3Button    .SetPos( FramePos( iPosX+433, iPosY+234, iPosX+433+130, iPosY+234+24 ) );
   AchatOpt4Button    .SetPos( FramePos( iPosX+433, iPosY+263, iPosX+433+130, iPosY+263+24 ) );
   AchatShowMinionSkin.SetPos( FramePos( iPosX+433, iPosY+300, iPosX+433+130, iPosY+300+24 ) );
   
   AcheterButton  .SetPos( FramePos( iPosX+201, iPosY+405, iPosX+201+92, iPosY+405+24 ) );
   PanierButton   .SetPos( FramePos( iPosX+313, iPosY+405, iPosX+313+92, iPosY+405+24 ) );

   m_stHEcu.SetPos( FramePos( iPosX+432, iPosY+119, iPosX+563, iPosY+136 ) );
   m_stEcu .SetPos( FramePos( iPosX+432, iPosY+140, iPosX+563, iPosY+157 ) );
   achatDescription.SetPos( FramePos( iPosX+44, iPosY+342, iPosX+564, iPosY+393 ) );
   m_sthCol1.SetPos( FramePos( iPosX+42 , iPosY+94, iPosX+309, iPosY+111 ) );
   m_sthCol2.SetPos( FramePos( iPosX+314, iPosY+94, iPosX+391, iPosY+111 ) );


   achatList.SetListDef(iPosX,iPosY,20,41,118,415,326,&achatListScrollUp,&achatListScrollDown);
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_BoutiqueDlg::Draw(V2SPRITEFX *vsfFX)
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

   AchatOpt1Button.DrawTextButtonExt(hdc);
   AchatOpt2Button.DrawTextButtonExt(hdc);
   AchatOpt3Button.DrawTextButtonExt(hdc);
   AchatOpt4Button.DrawTextButtonExt(hdc);
   AchatShowMinionSkin.DrawTextButtonExt(hdc);

   AcheterButton.DrawTextButtonExt(hdc);
   PanierButton.DrawTextButtonExt(hdc);
   
   // Draw les texte...
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_LocalString[655],strlen(g_LocalString[655]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);


   if(!m_bLockDrawList)
   {
      char strInfo[255];
      
      unsigned int indexStart = achatList.GetScrollPos();
      int iYStart = 122;
      for(int i=0;i<10;i++)
      {
         if(indexStart < m_vListDraw.size())
         {
            COLORREF crText = g_DefColorD;
            if((DWORD)m_vListDraw[indexStart].iCost > m_dwNbrNMSGold)
               crText = CL_RED;
            
            sprintf_s(strInfo,255  ,"%s",m_vListDraw[indexStart].strName);
            fSmallNormal_12->DrawFont(hdc, iOffX+ 44 , iOffY+iYStart+(i*21) , crText, 0, strInfo);      
            
            sprintf_s(strInfo,255  ,"%-6d",m_vListDraw[indexStart].iCost);
            fSmallNormal_12->DrawFont(hdc, iOffX+ 316, iOffY+iYStart+(i*21) , crText, 0, strInfo);      
         }
         indexStart++;
      }
   }


   DXDReleaseDC(hdc, 86);

   if(strcmp(m_strDescription,"")!=0)
   {
      achatDescription.SetText(m_strDescription,g_DefColorD,520,0,false,2);
      achatDescription.Draw( vsfFX );
   }
}

void V3_BoutiqueDlg::Hide()
{
   achatList.DeleteList();
   m_vAchatList.clear();
   m_vListDraw.clear();

   BoxUI::Hide();
}





void V3_BoutiqueDlg::StartDragDlg( MousePos mousePos )
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

void V3_BoutiqueDlg::StopDragDlg()
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
void V3_BoutiqueDlg::DragEvent::LeftClicked( void )
{
   V3_BoutiqueDlg *me = static_cast< V3_BoutiqueDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_BoutiqueDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_BoutiqueDlg *me = static_cast< V3_BoutiqueDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}




void V3_BoutiqueDlg::AchatOpt1ButtonEvent::LeftClicked( void )
{
   V3_BoutiqueDlg *me = static_cast< V3_BoutiqueDlg * >( GetUI() );
   
   me->m_bLockDrawList = true;
   me->m_vListDraw.clear();
   me->m_strDescription[0] = 0x00;
   me->m_dwSelectedAchatOpt = 0; 
   me->m_dwSelectedItemList = 0;
   me->m_bLockDrawList = false;
   
   TFCPacket Send;
   Send << (RQ_SIZE)RQ_NM_NMSGOLD_AchatOpt1;
   SEND_PACKET(Send);  
}

void V3_BoutiqueDlg::AchatOpt2ButtonEvent::LeftClicked( void )
{
   
   V3_BoutiqueDlg *me = static_cast< V3_BoutiqueDlg * >( GetUI() );
   
   me->m_bLockDrawList = true;
   me->m_vListDraw.clear();
   me->m_strDescription[0] = 0x00;
   me->m_dwSelectedAchatOpt = 0; 
   me->m_dwSelectedItemList = 0;
   me->m_bLockDrawList = false;
   
   TFCPacket Send;
   Send << (RQ_SIZE)RQ_NM_NMSGOLD_AchatOpt2;
   SEND_PACKET(Send);  
}


void V3_BoutiqueDlg::AchatOpt3ButtonEvent::LeftClicked( void )
{
   V3_BoutiqueDlg *me = static_cast< V3_BoutiqueDlg * >( GetUI() );
   
   me->m_bLockDrawList = true;
   me->m_vListDraw.clear();
   me->m_strDescription[0] = 0x00;
   me->m_dwSelectedAchatOpt = 0; 
   me->m_dwSelectedItemList = 0;
   me->m_bLockDrawList = false;
   
   TFCPacket Send;
   Send << (RQ_SIZE)RQ_NM_NMSGOLD_AchatOpt3;
   SEND_PACKET(Send);  
}


void V3_BoutiqueDlg::AchatOpt4ButtonEvent::LeftClicked( void )
{
   
}

void V3_BoutiqueDlg::AchatShowMinionSkinEvent::LeftClicked( void )
{
   V3_BoutiqueDlg *me = static_cast< V3_BoutiqueDlg * >( GetUI() );

   bool bFound = false;
   int  iIndex = 0;

   for(unsigned long i=0;i<V3_InvDlg::GetInstance()->m_vImageDisplay.size();i++)
   {
      if (!_stricmp( "Aperçu Compagnons", V3_InvDlg::GetInstance()->m_vImageDisplay[i].strName ))
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

      V3_DisplaySpecialDlg::GetInstance()->ShowLoadedBmp(pImage,pPal,g_DSDlgW,g_DSDlgH,V3_InvDlg::GetInstance()->m_vImageDisplay[iIndex].strName);
      me->RequestForegroundControl( V3_DisplaySpecialDlg::GetInstance() );

      delete []pImage;
      delete []pPal;

   }
}

void V3_BoutiqueDlg::AcheterButtonEvent::LeftClicked( void )
{
   V3_BoutiqueDlg *me = static_cast< V3_BoutiqueDlg * >( GetUI() );
   
   int Row = me->achatList.GetSelectedRow();
   
   if(Row >=0)
   {
      V3_ConfirmDlg::GetInstance()->SetDialogText(g_GUILocalString[303]);
      V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
      V3_ConfirmDlg::GetInstance()->SetParent(me);
      V3_ConfirmDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
      V3_ConfirmDlg::GetInstance()->Show(0);
      
      me->RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
   }
   else
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[668], CL_RED, true);
   }
   
}


void V3_BoutiqueDlg::PanierButtonEvent::LeftClicked( void )
{
   V3_BoutiqueDlg *me = static_cast< V3_BoutiqueDlg * >( GetUI() );
   
   me->Hide();
   
   V3_PanierDlg *pNMSPanierDlg = V3_PanierDlg::GetInstance();
   pNMSPanierDlg->RequestAchats();
   pNMSPanierDlg->Show();
}

void V3_BoutiqueDlg::CallMainNMSAchat()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   m_dwNbrNMSGold = 0;
   
   TFCPacket Send;
   Send << (RQ_SIZE)RQ_NM_NMSGOLD_AchatOpt1;
   SEND_PACKET(Send);  
   
}

void V3_BoutiqueDlg::AchatListEvent::LeftClicked( void )
{
   V3_BoutiqueDlg *me = static_cast< V3_BoutiqueDlg * >( GetUI() );
   
   me->m_dwSelectedItemList = me->achatList.GetSelectedRow();
   
   sprintf_s(me->m_strDescription,512,"%s", me->m_vListDraw[me->m_dwSelectedItemList].strDesc);
   
}

void V3_BoutiqueDlg::ClearAchatList()
{
   m_vAchatList.clear();
   m_strDescription[0] = 0x00;
}

void V3_BoutiqueDlg::AddAchatOption(char *pstrName,char *pstrDesc, int iCost)
{
   sAchatListT newOpt;
   sprintf_s(newOpt.strName,128,"%s",pstrName);
   if(pstrDesc)
      sprintf_s(newOpt.strDesc,512,"%s",pstrDesc);
   else
      newOpt.strDesc[0] = 0x00;
   
   newOpt.iCost = iCost;
   
   m_vAchatList.push_back(newOpt);
}

void V3_BoutiqueDlg::ListToControl()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   m_bLockDrawList = true;
   achatList.DeleteList();
   m_vListDraw.clear();
   
   int ii = 0;
   
   for(unsigned int i=0;i<m_vAchatList.size();i++)
   {
      
      achatList.AddNewItem();
      
      
      StrListTmp *pTmp = new StrListTmp;
      pTmp->strTmp[0] = 0;
      achatList.AddColumnItem( 0, pTmp );
      
      sAchatListT newI;
      sprintf_s(newI.strName,128,"%s",m_vAchatList[i].strName);
      sprintf_s(newI.strDesc,512,"%s",m_vAchatList[i].strDesc);
      newI.iCost = m_vAchatList[i].iCost;
      m_vListDraw.push_back(newI);
   }
   
   achatList.SetScrollPos( 0 );
   m_bLockDrawList = false;
}


void V3_BoutiqueDlg::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   switch(uiShowCodeP)
   {
      case 0:
      {
         if(bOK)
         {
            T3VSBSound PopupSound;
            
            PopupSound.CopyWave(&SoundFX[110]);
            PopupSound.Play(FALSE);
            
            DWORD dwOpt  = GetSelectedOpt ();
            DWORD dwItem = GetSelectedItem();
            
            //Packet d'achat
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_NM_NMSGOLD_Acheter;
            sending << (long)dwOpt;
            sending << (long)dwItem;
            SEND_PACKET( sending );
         }
      }
      break;
   }
}

void V3_BoutiqueDlg::SetNbrNMSGold(DWORD dwNbr)
{
   m_dwNbrNMSGold = dwNbr;
   char strTemp[100];
   sprintf_s(strTemp,100,"%d",m_dwNbrNMSGold);
   m_stEcu.SetText(strTemp,g_DefColorD);
}