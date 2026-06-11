// V3_Arene1WaitDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_Arene1WaitDlg.h"
#include "V3_InteractionPVP.h"
#include "V3_InvDlg.h"
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

V3_Arene1WaitDlg::V3_Arene1WaitDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
PlayerList( playerListEvent ),
QuitterBtn        ( quitterButtonEvent        ,g_LocalString[753],g_DefColorH )
{
   int i=0;
   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastArene1WXPos();
   m_iYPos = g_Global.GetDialogLastArene1WYPos();
   
   SetHelpId( this, 786 );
  
   m_BackgroundS.SetSpriteId("Arene1WaitBack",true,"Arene1WaitBackMask");

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
   
   QuitterBtn.SetSpriteId   ( "V3_Btn92x24_N" );
   QuitterBtn.SetDownUI     ( "V3_Btn92x24_D" );
   QuitterBtn.SetHighlightUI( "V3_Btn92x24_H" );
   quitterButtonEvent.SetUI( this );
   SetHelpId( QuitterBtn, 781 );

   stStartTime.SetCenter( true );
   stListTitle.SetCenter( true );
   stListTitle.SetText(g_GUILocalString[378],g_DefColorH);
 
   PlayerList.SetupList(1, 20, FramePos( 0, 0, 2, 1 ) );
   PlayerList.SetColumnSize( 0, 217 );
   PlayerSelect.SetPos( FramePos( 0, 0, 217, 20 ) );
   PlayerSelect.SetSpriteId( "Arene1WaitSelect" );
   PlayerSelect.Show();
   PlayerList.SetSelectionSprite( &PlayerSelect, 0);
   playerListEvent.SetUI( this );
   PlayerList.SetScrollDownDownUI( &PlayerListScrollDown );
   PlayerList.SetScrollUpDownUI( &PlayerListScrollUp );

   UpdateAllFramePos(m_iXPos,m_iYPos);

  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &QuitterBtn  );
   AddChild( &PlayerList );
   AddChild( &stStartTime);
   AddChild( &stListTitle);
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   m_iLevelMin = 0;
   m_iLevelMax = 0;
   m_iSecTotal     = 0;
   m_shTimeStayMin = 0;
   m_shTimeStaySec = 0;

   m_lArenaID   = 0;
   m_lArenaType = 0;

   ::InitializeCriticalSection(&crLockList);
} 

V3_Arene1WaitDlg::~V3_Arene1WaitDlg()
{
   
   ::DeleteCriticalSection(&crLockList);
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_Arene1WaitDlg *V3_Arene1WaitDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_Arene1WaitDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_Arene1WaitDlg::Display( void *null)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   GetInstance()->m_iXPos = g_Global.GetDialogLastArene1WXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastArene1WYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->Show();

   return 0;
}

void V3_Arene1WaitDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_Arene1WDlgW, iPosY+g_Arene1WDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_Arene1WDlgW, iPosY+g_Arene1WDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+263, iPosY+27, iPosX+263+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+263, iPosY+57 ) );


   QuitterBtn  .SetPos( FramePos( iPosX+112, iPosY+317, iPosX+112+92, iPosY+317+24 ) );
   
   stStartTime  .SetPos( FramePos( iPosX+72 , iPosY+352, iPosX+244, iPosY+374 ) );
   stListTitle  .SetPos( FramePos( iPosX+42 , iPosY+75 , iPosX+253, iPosY+90  ) );

   PlayerList.SetListDef(iPosX,iPosY,20,39,94,277,302,&PlayerListScrollUp,&PlayerListScrollDown);
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_Arene1WaitDlg::Draw(V2SPRITEFX *vsfFX)
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
      
      if(iNewPosX+g_Arene1WDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_Arene1WDlgW;
      if(iNewPosY+g_Arene1WDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_Arene1WDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   if(m_bNeedUpdateList)
   {
      LockList();
      m_vPLListL.clear();

      for(UINT i=0;i<m_vPLList.size();i++)
      {
         sAr1PL newPl;
         newPl.iPlID     = m_vPLList[i].iPlID;
         newPl.strPlName = m_vPLList[i].strPlName;
         m_vPLListL.push_back(newPl);
      }

      ListToControlDisplay();
      m_bNeedUpdateList = false;
      UnlockList();
   }
  
   if(m_shTimeStayMin >0 || m_shTimeStaySec >0)
   {
      char strTmp[100];
      sprintf_s(strTmp,100,"%02d min.  %02d sec.",m_shTimeStayMin,m_shTimeStaySec);
      stStartTime.SetText(strTmp, g_DefColorD, 0, 0, 0 );
   }
   else
   {
      stStartTime.SetText("", g_DefColorD, 0, 0, 0 );
   }

   GameUI::Draw( vsfFX );


   HDC hdc;
   DXDGetDC(&hdc, 86);

   QuitterBtn  .DrawTextButtonExt(hdc);
   
   // Draw les texte...
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   char strTmp[ 1024 ];
   sprintf_s( strTmp,1024, g_LocalString[ 784 ], V3_InteractionPVPDlg::GetInstance()->GetSelectedAreneName().c_str());
   
   TextOut(hdc,iOffX+68, iOffY+30,strTmp,strlen(strTmp));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);


   //Dessine liste player
   char strInfoN[50];
   unsigned int indexStart = PlayerList.GetScrollPos();
   int iYStart = 98;
   for(int i=0;i<10;i++)
   {
      if(indexStart < m_vPLListL.size())
      {
         COLORREF crText = g_DefColorD;
         sprintf_s(strInfoN,50  ,"%s",m_vPLListL[indexStart].strPlName.c_str());
         fSmallNormal_12->DrawFont(hdc, iOffX+ 41 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);      
      }
      indexStart++;
   }
   DXDReleaseDC(hdc, 86);
}

void V3_Arene1WaitDlg::Hide()
{
   BoxUI::Hide();
}


void V3_Arene1WaitDlg::StartDragDlg( MousePos mousePos )
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

void V3_Arene1WaitDlg::StopDragDlg()
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
      
      if(m_iXPos+g_Arene1WDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_Arene1WDlgW;
      if(m_iYPos+g_Arene1WDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_Arene1WDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastArene1WXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_Arene1WaitDlg::DragEvent::LeftClicked( void )
{
   V3_Arene1WaitDlg *me = static_cast< V3_Arene1WaitDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_Arene1WaitDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_Arene1WaitDlg *me = static_cast< V3_Arene1WaitDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}

void V3_Arene1WaitDlg::QuitterButtonEvent::LeftClicked( void )
{
   V3_Arene1WaitDlg *me = static_cast< V3_Arene1WaitDlg * >( GetUI() );

   TFCPacket sending;
   sending << (RQ_SIZE)RQ_ARENA1_Leave;
   sending << (long)me->m_lArenaID;
   sending << (long)me->m_lArenaType;
   SEND_PACKET( sending );
   me->Hide();
}



void V3_Arene1WaitDlg::PlayerListEvent::LeftClicked( void )
{
   V3_Arene1WaitDlg *me = static_cast< V3_Arene1WaitDlg * >( GetUI() );
   
     
}


void V3_Arene1WaitDlg::ClearAllList()
{
   m_vPLList.clear();
}

void V3_Arene1WaitDlg::UpdateInfoWait(long lArenaID,long lArenaType,short dwStayMin,short dwStaySec,int iLevelMin,int iLevelMax)
{
   UpdateTimeStay(dwStayMin,dwStaySec);
   m_iLevelMin  = iLevelMin;
   m_iLevelMax  = iLevelMax;
   m_lArenaID   = lArenaID;
   m_lArenaType = lArenaType;
}

void V3_Arene1WaitDlg::UpdateTimeStay(short shStayMin,short shStaySec)
{
   m_iSecTotal = shStayMin*60+shStaySec;
   m_shTimeStayMin = shStayMin;
   m_shTimeStaySec = shStaySec;

   if(m_shTimeStayMin < 0)
      m_shTimeStayMin = 0;
   if(m_shTimeStaySec < 0)
      m_shTimeStaySec = 0;
}

void V3_Arene1WaitDlg::UpdatePLList(int iPlID,char *pName)
{
   sAr1PL newPL;
   newPL.iPlID     = iPlID;
   newPL.strPlName = pName;
   
   m_vPLList.push_back(newPL);
}


void V3_Arene1WaitDlg::UpdateDisplayList()
{
   m_bNeedUpdateList = true;
}

void V3_Arene1WaitDlg::ListToControlDisplay()
{
   PlayerList.ResetSelectedRow();
   PlayerList.DeleteList();
   for(unsigned int i=0;i<m_vPLListL.size();i++)
   {
      PlayerList.AddNewItem();

      StrListTmp *pTmp = new StrListTmp;
      pTmp->strTmp[0] = 0;
      PlayerList.AddColumnItem( 0, pTmp );
   }
   PlayerList.SetScrollPos( 0 );
}


