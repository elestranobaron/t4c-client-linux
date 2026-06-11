// V3_PanierDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_PanierDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_ConfirmDlg.h"
#include "V3_BoutiqueDlg.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;

extern Font *fSmallNormal_12;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_PanierDlg::V3_PanierDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
achatList( achatListEvent ),
UtiliserButton( utiliserButtonEvent ,g_LocalString[678],g_DefColorH),
CrediterButton( crediterButtonEvent ,g_LocalString[674],g_DefColorH),
BoutiqueButton( boutiqueButtonEvent ,g_LocalString[655],g_DefColorH)
{
   int i=0;

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 680 );
  
   m_BackgroundS.SetSpriteId("V3_PanierBack",true,"V3_PanierBackMask");

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
   
   SanctionJailIcon.SetSpriteId( "V3_SanctionJailIcon" );

   for(i=0;i<4;i++)
   {
      backSanctionL1[i].SetSpriteId( "Sanction_Light1" );
      backSanctionL2[i].SetSpriteId( "Sanction_Light2" );
      backSanctionL3[i].SetSpriteId( "Sanction_Light3" );
   }


   UtiliserButton.SetSpriteId   ( "V3_Btn130x24_N" );
   UtiliserButton.SetDownUI     ( "V3_Btn130x24_D" );
   UtiliserButton.SetHighlightUI( "V3_Btn130x24_H" );
   utiliserButtonEvent.SetUI( this );
   SetHelpId( UtiliserButton, 671 );
   
   CrediterButton.SetSpriteId   ( "V3_Btn130x24_N" );
   CrediterButton.SetDownUI     ( "V3_Btn130x24_D" );
   CrediterButton.SetHighlightUI( "V3_Btn130x24_H" );
   crediterButtonEvent.SetUI( this );
   SetHelpId( CrediterButton, 672 );
   
   BoutiqueButton.SetSpriteId   ( "V3_Btn92x24_N" );
   BoutiqueButton.SetDownUI     ( "V3_Btn92x24_D" );
   BoutiqueButton.SetHighlightUI( "V3_Btn92x24_H" );
   boutiqueButtonEvent.SetUI( this );
   SetHelpId( BoutiqueButton, 670 );
   
  
   m_stHEcu.SetText(g_LocalString[661],g_DefColorH);
   achatDescription.SetCenter( false );

   m_stHSanction.SetText(g_LocalString[681],g_DefColorH);

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
   AddChild( &achatList );
   AddChild( &UtiliserButton );
   AddChild( &CrediterButton );
   AddChild( &BoutiqueButton );
   AddChild( &m_stHSanction );
   AddChild( &m_stHEcu );
   AddChild( &m_stEcu );
   AddChild( &m_sthCol1);
   AddChild( &m_sthCol2);



   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );


   UtiliserButton.Hide();
   CrediterButton.Hide();
   
   
   m_strDescription[0] = 0x00;
} 

V3_PanierDlg::~V3_PanierDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_PanierDlg *V3_PanierDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_PanierDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_PanierDlg::Display( void *null)
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

void V3_PanierDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   SanctionJailIcon.SetPos( FramePos( iPosX+430, iPosY+268, iPosX+455, iPosY+297 ) );
   for(int i=0;i<4;i++)
   {
      backSanctionL1[i].SetPos( FramePos( iPosX+427, iPosY+300+(i*7), iPosX+427+48, iPosY+300+5+(i*7) ) );
      backSanctionL2[i].SetPos( FramePos( iPosX+475, iPosY+300+(i*7), iPosX+475+48, iPosY+300+5+(i*7) ) );
      backSanctionL3[i].SetPos( FramePos( iPosX+523, iPosY+300+(i*7), iPosX+523+48, iPosY+300+5+(i*7) ) );
   }

   UtiliserButton.SetPos( FramePos( iPosX+434, iPosY+176, iPosX+434+130, iPosY+176+24 ) );
   CrediterButton.SetPos( FramePos( iPosX+434, iPosY+205, iPosX+434+130, iPosY+205+24 ) );
   BoutiqueButton.SetPos( FramePos( iPosX+257, iPosY+405, iPosX+257+92, iPosY+405+24 ) );

   m_stHSanction.SetPos( FramePos( iPosX+426, iPosY+245, iPosX+570, iPosY+262 ) );
   m_stHEcu.SetPos( FramePos( iPosX+432, iPosY+119, iPosX+563, iPosY+136 ) );
   m_stEcu .SetPos( FramePos( iPosX+432, iPosY+140, iPosX+563, iPosY+157 ) );
   achatDescription.SetPos( FramePos( iPosX+44, iPosY+342, iPosX+564, iPosY+393 ) );
   m_sthCol1.SetPos( FramePos( iPosX+42 , iPosY+94, iPosX+309, iPosY+111 ) );
   m_sthCol2.SetPos( FramePos( iPosX+314, iPosY+94, iPosX+391, iPosY+111 ) );


   achatList.SetListDef(iPosX,iPosY,20,41,118,415,326,&achatListScrollUp,&achatListScrollDown);
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_PanierDlg::Draw(V2SPRITEFX *vsfFX)
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

   if(m_ushNbrSanction>0)
      backSanctionL1[0].Draw(vsfFX);
   if(m_ushNbrSanction >1)
      backSanctionL1[1].Draw(vsfFX);
   if(m_ushNbrSanction >2)
      backSanctionL1[2].Draw(vsfFX);
   if(m_ushNbrSanction >3)
      backSanctionL1[3].Draw(vsfFX);
   
   if(m_ushNbrSanction >4)
      backSanctionL2[0].Draw(vsfFX);
   if(m_ushNbrSanction >5)
      backSanctionL2[1].Draw(vsfFX);
   if(m_ushNbrSanction >6)
      backSanctionL2[2].Draw(vsfFX);
   if(m_ushNbrSanction >7)
      backSanctionL2[3].Draw(vsfFX);
   
   if(m_ushNbrSanction >8)
      backSanctionL3[0].Draw(vsfFX);
   if(m_ushNbrSanction >9)
      backSanctionL3[1].Draw(vsfFX);
   if(m_ushNbrSanction >10)
      backSanctionL3[2].Draw(vsfFX);
   if(m_ushNbrSanction >11)
      backSanctionL3[3].Draw(vsfFX);
   
   if(m_dwNbrSecRemain >0)
      SanctionJailIcon.Draw(vsfFX);
   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   UtiliserButton.DrawTextButtonExt(hdc);
   CrediterButton.DrawTextButtonExt(hdc);
   BoutiqueButton.DrawTextButtonExt(hdc);
   
   // Draw les texte...

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_LocalString[675],strlen(g_LocalString[675]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);


   if(m_dwNbrSecRemain >0)
   {
      int iNbrHour = m_dwNbrSecRemain/3600;
      int inbrMin  = (m_dwNbrSecRemain-(iNbrHour*3600))/60;
      
      char strTmp[100];
      sprintf_s(strTmp,100  ,g_LocalString[682],iNbrHour,inbrMin);
      fSmallNormal_12->DrawFont(hdc, iOffX+TxtOff(fSmallNormal_12,hdc,strTmp ,114)+ 456, iOffY+273 , g_DefColorD   , 0, strTmp);
   }

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
            
            sprintf_s(strInfo,255  ,"%s",m_vListDraw[indexStart].strName);
            fSmallNormal_12->DrawFont(hdc, iOffX+ 44 , iOffY+iYStart+(i*21) , crText, 0, strInfo);      
            
            sprintf_s(strInfo,255  ,"%s",m_vListDraw[indexStart].strInf);
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

void V3_PanierDlg::Hide()
{
   m_dwSelectedItemList = 0;
   
   achatList.DeleteList();
   m_vAchatList.clear();
   m_vListDraw.clear();

   BoxUI::Hide();
}





void V3_PanierDlg::StartDragDlg( MousePos mousePos )
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

void V3_PanierDlg::StopDragDlg()
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
void V3_PanierDlg::DragEvent::LeftClicked( void )
{
   V3_PanierDlg *me = static_cast< V3_PanierDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_PanierDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_PanierDlg *me = static_cast< V3_PanierDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}




void V3_PanierDlg::UtiliserButtonEvent::LeftClicked( void )
{
   V3_PanierDlg *me = static_cast< V3_PanierDlg * >( GetUI() );

   int Row = me->achatList.GetSelectedRow();
   if(Row >=0)
   {
      V3_ConfirmDlg::GetInstance()->SetDialogText(g_GUILocalString[305]);
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

void V3_PanierDlg::CrediterButtonEvent::LeftClicked( void )
{
   V3_PanierDlg *me = static_cast< V3_PanierDlg * >( GetUI() );
      
   int Row = me->achatList.GetSelectedRow();
   if(Row >=0)
   {
      V3_ConfirmDlg::GetInstance()->SetDialogText(g_GUILocalString[305]);
      V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
      V3_ConfirmDlg::GetInstance()->SetParent(me);
      V3_ConfirmDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
      V3_ConfirmDlg::GetInstance()->Show(1);
      
      me->RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
   }
   else
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[668], CL_RED, true);
   }
}


void V3_PanierDlg::BoutiqueButtonEvent::LeftClicked( void )
{

   V3_PanierDlg *me = static_cast< V3_PanierDlg * >( GetUI() );
   me->Hide();
   
   V3_BoutiqueDlg *pNMSGoldDlg = V3_BoutiqueDlg::GetInstance();
   pNMSGoldDlg->CallMainNMSAchat();
   pNMSGoldDlg->Display(NULL);
   
}


void V3_PanierDlg::RequestAchats()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   m_dwNbrNMSGold     = 0;
   
   m_ushNbrSanction   = 0;
   m_dwNbrSecRemain   = 0;
   
   
   TFCPacket Send;
   Send << (RQ_SIZE)RQ_NM_NMSGOLD_ListPanier;
   SEND_PACKET(Send);  
   
   TFCPacket Send2;
   Send2 << (RQ_SIZE)RQ_NM_NMSGOLD_Sanction;
   SEND_PACKET(Send2);  
}

void V3_PanierDlg::AchatListEvent::LeftClicked( void )
{
   V3_PanierDlg *me = static_cast< V3_PanierDlg * >( GetUI() );
   
   int iRow = me->achatList.GetSelectedRow();
   
   me->m_dwSelectedItemList = me->m_vListDraw[iRow].dwID;//Flag ID on this list
   if(me->m_vListDraw[iRow].chCanCrediter >0)
      me->CrediterButton.Show();
   else
      me->CrediterButton.Hide();
   
   me->UtiliserButton.Show();
}

void V3_PanierDlg::SetSanction(USHORT ushNbrSanction, DWORD dwNbrSecRemain)
{
   m_ushNbrSanction   = ushNbrSanction;
   m_dwNbrSecRemain   = dwNbrSecRemain;
}

void V3_PanierDlg::ClearAchatList()
{
   m_vAchatList.clear();
   m_strDescription[0] = 0x00;
}

void V3_PanierDlg::AddAchatOption(char *pstrName,char *pstrInf, DWORD dwID, char chCanCrediter)
{
   sVosAchatT newOpt;
   sprintf_s(newOpt.strName,128,"%s",pstrName);
   sprintf_s(newOpt.strInf ,32,"%s",pstrInf);
   newOpt.dwID          = dwID;
   newOpt.chCanCrediter = chCanCrediter;
   
   m_vAchatList.push_back(newOpt);
}


void V3_PanierDlg::ListToControl()
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
      
      sVosAchatT newI;
      sprintf_s(newI.strName,128,"%s",m_vAchatList[i].strName);
      sprintf_s(newI.strInf,32 ,"%s",m_vAchatList[i].strInf);
      newI.dwID          = m_vAchatList[i].dwID;
      newI.chCanCrediter = m_vAchatList[i].chCanCrediter;
      m_vListDraw.push_back(newI);
   }
   
   achatList.SetScrollPos( 0 );
   m_bLockDrawList = false;
   
   if(m_vAchatList.size() == 0)
   {
      sprintf_s(m_strDescription,512,"%s",g_LocalString[679]);
   }
   
   UtiliserButton.Hide();
   CrediterButton.Hide();
}


void V3_PanierDlg::SetNbrNMSGold(DWORD dwNbr)
{
   m_dwNbrNMSGold = dwNbr;
   char strTemp[100];
   sprintf_s(strTemp,100,"%d",m_dwNbrNMSGold);
   m_stEcu.SetText(strTemp,g_DefColorD);
}


int V3_PanierDlg::TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise)
{
   int dwSize = (int)pFont->GetFLen(hdc,pStr);
   return((dwBoxZise-dwSize)/2);
}

void V3_PanierDlg::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   switch(uiShowCodeP)
   {
      case 0:
      {
         if(bOK)
         {
            DWORD dwFlagID = GetSelectedItem();

            Hide();
            //Packet de Credit
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_NM_NMSGOLD_UtiliserPanier;
            sending << (long)dwFlagID;
            SEND_PACKET( sending );
            
         }
      }
      break;
      case 1:
      {
         if(bOK)
         {
            DWORD dwFlagID = GetSelectedItem();
            //Packet de Credit
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_NM_NMSGOLD_CrediterPanier;
            sending << (long)dwFlagID;
            SEND_PACKET( sending );
         }
      }
      break;
   }
}