// V3_InteractionPVPDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_InteractionPvp.h"
#include "V3_ChatLogDlg.h"
#include "V3_PanierDlg.h"
#include "V3_ConfirmDlg.h"
#include "V3_InvDlg.h"
#include "V3_DisplaySpecialDlg.h"
#include "V3_AskValueDlg.h"
#include "V3_ArenePointsDlg.h"
#include "..\SaveGame.h"
#include "..\FormatText.h"
#include "..\Bitmap.h"
extern CSaveGame g_SaveGame; 

extern Bitmap GridID;
extern T3VSBSound SoundFX[250];
extern Font *fSmallNormal_12; 



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_InteractionPVPDlg::V3_InteractionPVPDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
AreneList( areneListEvent ),
RejoindreBtn( rejoindreButtonEvent  ,g_LocalString[750],g_DefColorH ),
AfficherBtn( afficherButtonEvent  ,g_LocalString[795],g_DefColorH )
{
   int i=0;
   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastIPVPXPos();
   m_iYPos = g_Global.GetDialogLastIPVPYPos();
   
   //SetHelpId( this, 755 );
  
   m_BackgroundS.SetSpriteId("InteractionPVPBack2",true,"InteractionPVPBack2Mask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_StatIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
   RejoindreBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   RejoindreBtn.SetDownUI     ( "V3_Btn130x24_D" );
   RejoindreBtn.SetHighlightUI( "V3_Btn130x24_H" );
   rejoindreButtonEvent.SetUI( this );
   SetHelpId( RejoindreBtn, 782 );

   AfficherBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   AfficherBtn.SetDownUI     ( "V3_Btn130x24_D" );
   AfficherBtn.SetHighlightUI( "V3_Btn130x24_H" );
   afficherButtonEvent.SetUI( this );
   SetHelpId( AfficherBtn, 794 );

   
     
   m_stHArene1 .SetCenter( true );
   m_stHArene2 .SetCenter( true );
  

   m_stHArene1  .SetText(g_LocalString[790],g_DefColorH);
   m_stHArene2  .SetText(g_LocalString[796],g_DefColorH);
  
   AreneList.SetupList( 2, 20, FramePos( 0, 0, 2, 1 ) );
   AreneList.SetColumnSize( 0, 228 );
   AreneList.SetColumnSize( 1, 106 );
   AreneSelect.SetPos( FramePos( 0, 0, 336, 20 ) );
   AreneSelect.SetSpriteId( "InteractionPVPSelect2" );
   AreneSelect.Show();
   AreneList.SetSelectionSprite( &AreneSelect, 0);
   areneListEvent.SetUI( this );
   AreneList.SetScrollDownDownUI( &AreneListScrollDown );
   AreneList.SetScrollUpDownUI( &AreneListScrollUp );

   m_IconArene01.SetSpriteId( "AreneIcon01");
   m_IconArene02.SetSpriteId( "AreneIcon02");

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &RejoindreBtn);
   AddChild( &AfficherBtn);
   AddChild( &AreneList );
   AddChild( &m_stHArene1);
   AddChild( &m_stHArene2);
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   ::InitializeCriticalSection(&crLockList);

   m_strSelectedAreneName = "";

   m_bNeedUpdateList     = false;
   m_bNeedUpdateListStat = false;

   RejoindreBtn .Hide();
   AfficherBtn  .Hide();
} 

V3_InteractionPVPDlg::~V3_InteractionPVPDlg()
{
   
   ::DeleteCriticalSection(&crLockList);
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_InteractionPVPDlg *V3_InteractionPVPDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_InteractionPVPDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_InteractionPVPDlg::Display( void *null)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   // If inventory is already shown.
   if( GetInstance()->IsShown())
   {
      // Hide box
      GetInstance()->Hide();
      GetInstance()->m_TimerTools.KillNMTimer(1);
   }
   else
   {
      GetInstance()->m_iXPos = g_Global.GetDialogLastIPVPXPos();
      GetInstance()->m_iYPos = g_Global.GetDialogLastIPVPYPos();
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      GetInstance()->Show();

      GetInstance()->m_TimerTools.SetNMTimer(1,1000,TimerCallback,GetInstance());
   }
   return 0;
}

void V3_InteractionPVPDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_IPVPDlgW, iPosY+g_IPVPDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_IPVPDlgW, iPosY+g_IPVPDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+727, iPosY+27, iPosX+727+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+690, iPosY+57 ) );

   RejoindreBtn .SetPos( FramePos( iPosX+510, iPosY+400 , iPosX+510+130, iPosY+400+24 ) );
   AfficherBtn  .SetPos( FramePos( iPosX+510, iPosY+400 , iPosX+510+130, iPosY+400+24 ) );

   m_stHArene1.SetPos( FramePos( iPosX+41 , iPosY+74, iPosX+263, iPosY+89 ) );
   m_stHArene2.SetPos( FramePos( iPosX+270, iPosY+74, iPosX+371, iPosY+89 ) );
 
   AreneList.SetListDef(iPosX,iPosY,20,38,93,395,427,&AreneListScrollUp,&AreneListScrollDown);
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_InteractionPVPDlg::Draw(V2SPRITEFX *vsfFX)
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
      
      if(iNewPosX+g_IPVPDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_IPVPDlgW;
      if(iNewPosY+g_IPVPDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_IPVPDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }


   if(m_bNeedUpdateList)
   {
      LockList();
      m_vArListL.clear();

      for(UINT i=0;i<m_vArList.size();i++)
      {
         sAreneList newAr;
         newAr.iIDList   = m_vArList[i].iIDList;
         newAr.iIDType   = m_vArList[i].iIDType;
         newAr.iEnable   = m_vArList[i].iEnable;
         newAr.iStatus   = m_vArList[i].iStatus;
         newAr.iLevelMin = m_vArList[i].iLevelMin;
         newAr.iLevelMax = m_vArList[i].iLevelMax;
         newAr.iNbrSec   = m_vArList[i].iNbrSec;
         newAr.iNbrMin   = m_vArList[i].iNbrMin;

         sprintf_s(newAr.strARName , 50 ,"%s",m_vArList[i].strARName);
         sprintf_s(newAr.strARDesc,2048 ,"%s",m_vArList[i].strARDesc);
         m_vArListL.push_back(newAr);
      }

      ListToControlDisplay();
      m_bNeedUpdateList = false;
      UnlockList();
   }

   if(m_bNeedUpdateListStat)
   {
      LockList();

      if(m_vArListL.size() == m_vArList.size())
      {
         for(UINT i=0;i<m_vArList.size();i++)
         {
            m_vArListL[i].iEnable   = m_vArList[i].iEnable;
            m_vArListL[i].iStatus   = m_vArList[i].iStatus;
            m_vArListL[i].iLevelMin = m_vArList[i].iLevelMin;
            m_vArListL[i].iLevelMax = m_vArList[i].iLevelMax;
            m_vArListL[i].iNbrSec   = m_vArList[i].iNbrSec;
            m_vArListL[i].iNbrMin   = m_vArList[i].iNbrMin;
         }
      }
      m_bNeedUpdateListStat = false;
      UnlockList();
   }

   UINT iSel = AreneList.GetSelectedRow();
   if(iSel >=0 && iSel < m_vArListL.size())
   {
      if(m_vArListL[iSel].iStatus == 0 || m_vArListL[iSel].iStatus == 1)
      {
         RejoindreBtn.Show();
         AfficherBtn.Hide();
      }
      else if(m_vArListL[iSel].iStatus == 2)
      {
         RejoindreBtn.Hide();
         AfficherBtn.Show();
      }
   }
   else
   {
      RejoindreBtn.Hide();
      AfficherBtn.Hide();
   }
 
   GameUI::Draw( vsfFX );

   HDC hdc;
   DXDGetDC(&hdc, 86);

   RejoindreBtn  .DrawTextButtonExt(hdc);
   AfficherBtn  .DrawTextButtonExt(hdc);
   
   // Draw les texte...
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_LocalString[791],strlen(g_LocalString[791]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);

   
   int iYStart = 97;
   FormatText MessageFmt;
   MessageFmt.SetText(m_strDesc.c_str());
   MessageFmt.Format(57);
   for(int l=0;l<MessageFmt.GetnbLine();l++)
   {
      fSmallNormal_12->DrawFont(hdc, iOffX+ 405, iOffY+iYStart+(l*13) , g_DefColorD    , 0, MessageFmt.GetText(l));      
   }





   //Dessine liste player
   char strInfoN1[255];
   char strInfoN2[255];
   //Dessine liste des Arenes
   unsigned int indexStart = AreneList.GetScrollPos();
   iYStart = 97;
   for(int i=0;i<16;i++)
   {
      if(indexStart < m_vArListL.size())
      {
         COLORREF crText = g_DefColorD;
         if(m_vArListL[i].iEnable == 0)
         {
            crText = RGB(225,20,20);
            sprintf_s(strInfoN1,255  ,"%s",m_vArListL[indexStart].strARName);
            strInfoN2[0] = 0x00;
         }
         else if(m_vArListL[i].iStatus == 0)
         {
            crText = g_DefColorD;
            sprintf_s(strInfoN1,255  ,"%s",m_vArListL[indexStart].strARName);
            strInfoN2[0] = 0x00;
         }
         else if(m_vArListL[i].iStatus == 1)
         {
            crText = RGB(244,149,54); //orange
            sprintf_s(strInfoN1,255  ,"%s (nv: %d-%d)",m_vArListL[indexStart].strARName,m_vArListL[indexStart].iLevelMin,m_vArListL[indexStart].iLevelMax);
            sprintf_s(strInfoN2,255  ,"%d min. %d sec.",m_vArListL[indexStart].iNbrMin,m_vArListL[indexStart].iNbrSec);
         }
         else if(m_vArListL[i].iStatus == 2)
         {
            crText = RGB(0,164,0); //vert
            sprintf_s(strInfoN1,255  ,"%s (nv: %d-%d)",m_vArListL[indexStart].strARName,m_vArListL[indexStart].iLevelMin,m_vArListL[indexStart].iLevelMax);
            sprintf_s(strInfoN2,255  ,"%d min. %d sec.",m_vArListL[indexStart].iNbrMin,m_vArListL[indexStart].iNbrSec);
         }

         
         int yPos = iOffY+iYStart+(i*21);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 41 , yPos , crText, 0, strInfoN1);      
         fSmallNormal_12->DrawFont(hdc, iOffX+ 271, yPos , crText, 0, strInfoN2);      

      }
      indexStart++;
   }
   DXDReleaseDC(hdc, 86);

   /*
   iYStart = 104;
   for(int i=0;i<4;i++)
   {
      if(indexStart < m_vArListL.size())
      {
         
         int yPos = iOffY+iYStart+(i*21);
         RECT clipRect = { iOffX+ 41, yPos+1, iOffX+ 41+16, yPos+19 };
         V2SPRITEFX fx;
         memset( &fx, 0, sizeof( fx ) );
         fx.lpClipRect = &clipRect;
         if(m_vArListL[i].iIDType == 0)
            m_IconArene01.Draw( &fx );
         else
            m_IconArene02.Draw( &fx );
      }
      indexStart++;
   }
*/
}

void V3_InteractionPVPDlg::Hide()
{
   m_TimerTools.KillNMTimer(1);
   BoxUI::Hide();
}





void V3_InteractionPVPDlg::StartDragDlg( MousePos mousePos )
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

void V3_InteractionPVPDlg::StopDragDlg()
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
      
      if(m_iXPos+g_IPVPDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_IPVPDlgW;
      if(m_iYPos+g_IPVPDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_IPVPDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastIPVPXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_InteractionPVPDlg::DragEvent::LeftClicked( void )
{
   V3_InteractionPVPDlg *me = static_cast< V3_InteractionPVPDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_InteractionPVPDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_InteractionPVPDlg *me = static_cast< V3_InteractionPVPDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


void V3_InteractionPVPDlg::RejoindreButtonEvent::LeftClicked( void )
{
   V3_InteractionPVPDlg *me = static_cast< V3_InteractionPVPDlg * >( GetUI() );

   int iSel = me->AreneList.GetSelectedRow();

   if(iSel >= 0)
   {
      int iAreneID   = me->m_vArListL[iSel].iIDList;
      int iAreneType = me->m_vArListL[iSel].iIDType;
      me->m_strSelectedAreneName = me->m_vArListL[iSel].strARName;
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_ARENA1_Join;
      sending << (long)iAreneID;
      sending << (long)iAreneType;
      SEND_PACKET( sending );
      me->Hide();


      /*
      if(V3_ArenePointsDlg::GetInstance()->IsShown())
      {
         V3_ChatLogDlg::GetInstance()->AddListLogs("", g_GUILocalString[793], CL_RED, true);
         return;
      }
      int iAreneID   = me->m_vArListL[iSel].iIDList;
      int iAreneType = me->m_vArListL[iSel].iIDType;
      me->m_strSelectedAreneName = me->m_vArListL[iSel].strARName;
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_ARENA1_GetWaitPlayerList;
      sending << (long)iAreneID;
      sending << (long)iAreneType;
      SEND_PACKET( sending );
      me->Hide();
      */
   }
   else
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs("", g_GUILocalString[384], CL_RED, true);
   }
}

void V3_InteractionPVPDlg::AfficherButtonEvent::LeftClicked( void )
{
   V3_InteractionPVPDlg *me = static_cast< V3_InteractionPVPDlg * >( GetUI() );

   int iSel = me->AreneList.GetSelectedRow();

   if(iSel >= 0)
   {
      int iAreneID   = me->m_vArListL[iSel].iIDList;
      int iAreneType = me->m_vArListL[iSel].iIDType;

      TFCPacket sending;
      sending << (RQ_SIZE)RQ_ARENA1_GetWaitPlayerList;
      sending << (long)iAreneID;
      sending << (long)iAreneType;
      SEND_PACKET( sending );
   }
   else
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs("", g_GUILocalString[384], CL_RED, true);
   }
}



void V3_InteractionPVPDlg::AreneListEvent::LeftClicked( void )
{
   V3_InteractionPVPDlg *me = static_cast< V3_InteractionPVPDlg * >( GetUI() );

   int iSel = me->AreneList.GetSelectedRow();

   if(iSel >=0)
   {
      me->m_strDesc = me->m_vArListL[iSel].strARDesc;
   }
   else
   {
      me->m_strDesc = "";
   }
}

void V3_InteractionPVPDlg::ClearAllList()
{
   m_vArList.clear();
   m_strDesc = "";
}


void V3_InteractionPVPDlg::UpdateAreneList(int iID,int iType,int iEnable,char *pName,char *pstrDesc)
{
   sAreneList newAR;
   newAR.iIDList   = iID;
   newAR.iIDType   = iType;
   newAR.iEnable   = iEnable;
   newAR.iStatus   = 0;
   newAR.iLevelMin = 0;
   newAR.iLevelMax = 0;
   newAR.iNbrSec   = 0;
   newAR.iNbrMin   = 0;
   sprintf_s(newAR.strARName ,50  ,"%s",pName);
   sprintf_s(newAR.strARDesc,2048 ,"%s",pstrDesc);
   m_vArList.push_back(newAR);
}


void V3_InteractionPVPDlg::UpdateAreneListStat(int iIndex,int iEnable,int iStatus,int  iLevelMin,int  iLevelMax,int  iNbrSec,int  iNbrMin)
{
   if((UINT)iIndex < m_vArList.size())
   {
      m_vArList[iIndex].iEnable   = iEnable;
      m_vArList[iIndex].iStatus   = iStatus;
      m_vArList[iIndex].iLevelMin = iLevelMin;
      m_vArList[iIndex].iLevelMax = iLevelMax;
      m_vArList[iIndex].iNbrSec   = iNbrSec;
      m_vArList[iIndex].iNbrMin   = iNbrMin;
   }
}

void V3_InteractionPVPDlg::UpdateDisplayList()
{
   m_bNeedUpdateList = true;
}

void V3_InteractionPVPDlg::UpdateDisplayListStat()
{
   m_bNeedUpdateListStat = true;
}

void V3_InteractionPVPDlg::ListToControlDisplay()
{
   AreneList.ResetSelectedRow();

   AreneList.DeleteList();
   for(unsigned int i=0;i<m_vArListL.size();i++)
   {
      AreneList.AddNewItem();

      StrListTmp *pTmp = new StrListTmp;
      pTmp->strTmp[0] = 0;
      AreneList.AddColumnItem( 0, pTmp );
   }
   AreneList.SetScrollPos( 0 );
}


int V3_InteractionPVPDlg::TimerCallback(void* pThis,unsigned int dwCode)
{
   V3_InteractionPVPDlg *me = (V3_InteractionPVPDlg*)pThis;

   me->OnTimer(dwCode);
   return 0;
}

void V3_InteractionPVPDlg::OnTimer(unsigned int nIDEvent)
{
   if(nIDEvent==1)
   {
      TFCPacket packet;
      packet << (RQ_SIZE)RQ_RP_BroadCastPVPStat;
      SEND_PACKET( packet );
   }
}