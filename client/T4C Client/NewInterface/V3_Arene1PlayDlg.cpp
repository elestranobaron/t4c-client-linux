// V3_Arene1PlayDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_Arene1PlayDlg.h"
#include "V3_ArenePointsDlg.h"
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

V3_Arene1PlayDlg::V3_Arene1PlayDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
PlayerListB( playerListEvent ),
PlayerListR( playerListEvent ),
QuitterBtn        ( quitterButtonEvent        ,g_LocalString[753],g_DefColorH ),
RejoindreBtn      ( rejoindreButtonEvent      ,g_LocalString[750],g_DefColorH )
{
   int i=0;
   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastArene1PXPos();
   m_iYPos = g_Global.GetDialogLastArene1PYPos();
   
   SetHelpId( this, 787 );
  
   m_BackgroundS.SetSpriteId("Arene1PlayBack",true,"Arene1PlayBackMask");

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
   
   RejoindreBtn.SetSpriteId   ( "V3_Btn92x24_N" );
   RejoindreBtn.SetDownUI     ( "V3_Btn92x24_D" );
   RejoindreBtn.SetHighlightUI( "V3_Btn92x24_H" );
   rejoindreButtonEvent.SetUI( this );
   SetHelpId( RejoindreBtn, 782 );
   
   QuitterBtn.SetSpriteId   ( "V3_Btn92x24_N" );
   QuitterBtn.SetDownUI     ( "V3_Btn92x24_D" );
   QuitterBtn.SetHighlightUI( "V3_Btn92x24_H" );
   quitterButtonEvent.SetUI( this );
   SetHelpId( QuitterBtn, 781 );

   stStayTime.SetCenter( true );
   stBPoint  .SetCenter( true );
   stBName   .SetCenter( true );
   stBFlag   .SetCenter( true );
   stBKill   .SetCenter( true );
   stBDead   .SetCenter( true );
   stBPts    .SetCenter( true );
   stRPoint  .SetCenter( true );
   stRName   .SetCenter( true );
   stRFlag   .SetCenter( true );
   stRKill   .SetCenter( true );
   stRDead   .SetCenter( true );
   stRPts    .SetCenter( true );

   stBName.SetText(g_GUILocalString[379],g_DefColorH);
   stBFlag.SetText(g_GUILocalString[380],g_DefColorH);
   stBKill.SetText(g_GUILocalString[381],g_DefColorH);
   stBDead.SetText(g_GUILocalString[382],g_DefColorH);
   stBPts .SetText(g_GUILocalString[391],g_DefColorH);

   stRName.SetText(g_GUILocalString[379],g_DefColorH);
   stRFlag.SetText(g_GUILocalString[380],g_DefColorH);
   stRKill.SetText(g_GUILocalString[381],g_DefColorH);
   stRDead.SetText(g_GUILocalString[382],g_DefColorH);
   stRPts .SetText(g_GUILocalString[391],g_DefColorH);
   
 
   PlayerListB.SetupList(5, 20, FramePos( 0, 0, 2, 1 ) );
   PlayerListB.SetColumnSize( 0, 167 );
   PlayerListB.SetColumnSize( 1, 70 );
   PlayerListB.SetColumnSize( 2, 70 );
   PlayerListB.SetColumnSize( 3, 61 );
   PlayerListB.SetColumnSize( 4, 61 );
   PlayerSelectB.SetPos( FramePos( 0, 0, 440, 20 ) );
   PlayerSelectB.SetSpriteId( "Arene1PlaySelect" );
   PlayerSelectB.Show();
   PlayerListB.SetSelectionSprite( &PlayerSelectB, 0);
   playerListEvent.SetUI( this );
   PlayerListB.SetScrollDownDownUI( &PlayerListScrollDownB );
   PlayerListB.SetScrollUpDownUI( &PlayerListScrollUpB );

   PlayerListR.SetupList(5, 20, FramePos( 0, 0, 2, 1 ) );
   PlayerListR.SetColumnSize( 0, 167 );
   PlayerListR.SetColumnSize( 1, 70 );
   PlayerListR.SetColumnSize( 2, 70 );
   PlayerListR.SetColumnSize( 3, 61 );
   PlayerListR.SetColumnSize( 4, 61 );
   PlayerSelectR.SetPos( FramePos( 0, 0, 440, 20 ) );
   PlayerSelectR.SetSpriteId( "Arene1PlaySelect" );
   PlayerSelectR.Show();
   PlayerListR.SetSelectionSprite( &PlayerSelectR, 0);
   playerListEvent.SetUI( this );
   PlayerListR.SetScrollDownDownUI( &PlayerListScrollDownR );
   PlayerListR.SetScrollUpDownUI( &PlayerListScrollUpR );

   m_FlagB.SetSpriteId( "Arene1PlayFlagBleu");
   m_FlagR.SetSpriteId( "Arene1PlayFlagRed");


   UpdateAllFramePos(m_iXPos,m_iYPos);

  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &QuitterBtn  );
   AddChild( &RejoindreBtn);
   AddChild( &PlayerListB );
   AddChild( &PlayerListR );
   AddChild( &m_FlagB);
   AddChild( &m_FlagR);
   AddChild( &stStayTime );
   AddChild( &stBPoint   );
   AddChild( &stBName    );
   AddChild( &stBFlag    );
   AddChild( &stBKill    );
   AddChild( &stBDead    );
   AddChild( &stBPts     );
   AddChild( &stRPoint   ); 
   AddChild( &stRName    );
   AddChild( &stRFlag    );
   AddChild( &stRKill    );
   AddChild( &stRDead    );
   AddChild( &stRPts     );
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   m_iLevelMin     = 0;
   m_iLevelMax     = 0;
   m_iSecTotal     = 0;
   m_shTimeStayMin = 0;
   m_shTimeStaySec = 0;
   m_iPointB       = 0;
   m_iPointR       = 0;

   QuitterBtn.Hide();
   RejoindreBtn.Hide();

   ::InitializeCriticalSection(&crLockList);
} 

V3_Arene1PlayDlg::~V3_Arene1PlayDlg()
{
   
   ::DeleteCriticalSection(&crLockList);
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_Arene1PlayDlg *V3_Arene1PlayDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_Arene1PlayDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_Arene1PlayDlg::Display( void *null)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   // If inventory is already shown.
   /*
   if( GetInstance()->IsShown())
   {
      // Hide box
      GetInstance()->Hide();
   }
   else
   {*/
      GetInstance()->m_iXPos = g_Global.GetDialogLastArene1PXPos();
      GetInstance()->m_iYPos = g_Global.GetDialogLastArene1PYPos();
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      GetInstance()->Show();
   //}
   return 0;
}

void V3_Arene1PlayDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_Arene1PDlgW, iPosY+g_Arene1PDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_Arene1PDlgW, iPosY+g_Arene1PDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );


   QuitterBtn  .SetPos( FramePos( iPosX+430, iPosY+353, iPosX+430+92, iPosY+353+24 ) );
   RejoindreBtn.SetPos( FramePos( iPosX+328, iPosY+353, iPosX+328+92, iPosY+353+24 ) );

 
   stStayTime  .SetPos( FramePos( iPosX+73 , iPosY+353, iPosX+313, iPosY+353+24 ) );
   stBPoint    .SetPos( FramePos( iPosX+40 , iPosY+171, iPosX+105, iPosY+197    ) );
   stBName     .SetPos( FramePos( iPosX+112, iPosY+77 , iPosX+278, iPosY+92     ) );
   stBFlag     .SetPos( FramePos( iPosX+281, iPosY+77 , iPosX+350, iPosY+92     ) );
   stBKill     .SetPos( FramePos( iPosX+353, iPosY+77 , iPosX+422, iPosY+92     ) );
   stBDead     .SetPos( FramePos( iPosX+425, iPosY+77 , iPosX+485, iPosY+92     ) );
   stBPts      .SetPos( FramePos( iPosX+488, iPosY+77 , iPosX+548, iPosY+92     ) );

   stRPoint    .SetPos( FramePos( iPosX+40 , iPosY+310, iPosX+105, iPosY+336    ) );
   stRName     .SetPos( FramePos( iPosX+112, iPosY+217, iPosX+278, iPosY+232    ) );
   stRFlag     .SetPos( FramePos( iPosX+281, iPosY+217, iPosX+350, iPosY+232    ) );
   stRKill     .SetPos( FramePos( iPosX+353, iPosY+217, iPosX+422, iPosY+232    ) );
   stRDead     .SetPos( FramePos( iPosX+425, iPosY+217, iPosX+485, iPosY+232    ) );
   stRPts      .SetPos( FramePos( iPosX+488, iPosY+217, iPosX+548, iPosY+232    ) );

   m_FlagB     .SetPos( FramePos( iPosX+39, iPosY+72 , iPosX+98, iPosY+167    ) );
   m_FlagR     .SetPos( FramePos( iPosX+39, iPosY+212, iPosX+98, iPosY+307    ) );

   PlayerListB.SetListDef(iPosX,iPosY,20,112,96 ,570,199,&PlayerListScrollUpB,&PlayerListScrollDownB);
   PlayerListR.SetListDef(iPosX,iPosY,20,112,236,570,339,&PlayerListScrollUpR,&PlayerListScrollDownR);
}
  
//////////////////////////////////////////////////////////////////////////////////////////
void V3_Arene1PlayDlg::Draw(V2SPRITEFX *vsfFX)
{
   char strTmp[100];

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
      
      if(iNewPosX+g_Arene1PDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_Arene1PDlgW;
      if(iNewPosY+g_Arene1PDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_Arene1PDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   if(m_bNeedUpdateList)
   {
      LockList();
      m_vPLListLB.clear();
      m_vPLListLR.clear();

      for(UINT i=0;i<m_vPLListB.size();i++)
      {
         sAr1PLay newPl;
         newPl.iPlID     = m_vPLListB[i].iPlID;
         newPl.strPlName = m_vPLListB[i].strPlName;
         newPl.iFlag     = m_vPLListB[i].iFlag;
         newPl.iKill     = m_vPLListB[i].iKill;
         newPl.iDead     = m_vPLListB[i].iDead;
         newPl.iPlPoints = m_vPLListB[i].iPlPoints;
         m_vPLListLB.push_back(newPl);
      }

      for(UINT i=0;i<m_vPLListR.size();i++)
      {
         sAr1PLay newPl;
         newPl.iPlID     = m_vPLListR[i].iPlID;
         newPl.strPlName = m_vPLListR[i].strPlName;
         newPl.iFlag     = m_vPLListR[i].iFlag;
         newPl.iKill     = m_vPLListR[i].iKill;
         newPl.iDead     = m_vPLListR[i].iDead;
         newPl.iPlPoints = m_vPLListR[i].iPlPoints;
         m_vPLListLR.push_back(newPl);
      }

      ListToControlDisplay();
      m_bNeedUpdateList = false;
      UnlockList();
   }
  
   

   if(!m_bOnArene) //pas rp
   {
      QuitterBtn.Hide();
      //RejoindreBtn.Show();
   }
   else
   {
      QuitterBtn.Show();
      RejoindreBtn.Hide();
   }

   if(m_shTimeStayMin >0 || m_shTimeStaySec >0)
   {
      sprintf_s(strTmp,100,"%02d min.  %02d sec.",m_shTimeStayMin,m_shTimeStaySec);
      stStayTime.SetText(strTmp, g_DefColorD, 0, 0, 0 );
   }
   else
   {
      stStayTime.SetText("", g_DefColorD, 0, 0, 0 );
   }

   sprintf_s(strTmp,100,"%d",m_iPointB);
   stBPoint.SetText(strTmp);
   sprintf_s(strTmp,100,"%d",m_iPointR);
   stRPoint.SetText(strTmp);

   GameUI::Draw( vsfFX );


   HDC hdc;
   DXDGetDC(&hdc, 86);

   RejoindreBtn.DrawTextButtonExt(hdc);
   QuitterBtn  .DrawTextButtonExt(hdc);
   
   // Draw les texte...
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   char buf[ 1024 ];
   sprintf_s( buf,1024, g_LocalString[ 785 ], V3_InteractionPVPDlg::GetInstance()->GetSelectedAreneName().c_str(),m_iLevelMin,m_iLevelMax );
   TextOut(hdc,iOffX+68, iOffY+30,buf,strlen(buf));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);


   //Dessine liste player
   char strInfoN[50];
   unsigned int indexStart = PlayerListB.GetScrollPos();
   int iYStart = 99;
   for(int i=0;i<5;i++)
   {
      if(indexStart < m_vPLListLB.size())
      {
         COLORREF crText = g_DefColorD;
         sprintf_s(strInfoN,50  ,"%s",m_vPLListLB[indexStart].strPlName.c_str());
         fSmallNormal_12->DrawFont(hdc, iOffX+ 115 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);   
         sprintf_s(strInfoN,50  ,"%d",m_vPLListLB[indexStart].iFlag);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 284 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);   
         sprintf_s(strInfoN,50  ,"%d",m_vPLListLB[indexStart].iKill);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 356 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);   
         sprintf_s(strInfoN,50  ,"%d",m_vPLListLB[indexStart].iDead);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 428 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);   
         sprintf_s(strInfoN,50  ,"%d",m_vPLListLB[indexStart].iPlPoints);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 491 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);   
      }
      indexStart++;
   }

   indexStart = PlayerListR.GetScrollPos();
   iYStart = 239;
   for(int i=0;i<5;i++)
   {
      if(indexStart < m_vPLListLR.size())
      {
         COLORREF crText = g_DefColorD;
         sprintf_s(strInfoN,50  ,"%s",m_vPLListLR[indexStart].strPlName.c_str());
         fSmallNormal_12->DrawFont(hdc, iOffX+ 115 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);   
         sprintf_s(strInfoN,50  ,"%d",m_vPLListLR[indexStart].iFlag);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 284 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);   
         sprintf_s(strInfoN,50  ,"%d",m_vPLListLR[indexStart].iKill);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 356 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);   
         sprintf_s(strInfoN,50  ,"%d",m_vPLListLR[indexStart].iDead);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 428 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);  
         sprintf_s(strInfoN,50  ,"%d",m_vPLListLR[indexStart].iPlPoints);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 491 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);   
      }
      indexStart++;
   }

   DXDReleaseDC(hdc, 86);
}

void V3_Arene1PlayDlg::Hide()
{
   m_TimerTools.KillNMTimer(1);
   BoxUI::Hide();
}


void V3_Arene1PlayDlg::StartDragDlg( MousePos mousePos )
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

void V3_Arene1PlayDlg::StopDragDlg()
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
      
      if(m_iXPos+g_Arene1PDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_Arene1PDlgW;
      if(m_iYPos+g_Arene1PDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_Arene1PDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastArene1PXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_Arene1PlayDlg::DragEvent::LeftClicked( void )
{
   V3_Arene1PlayDlg *me = static_cast< V3_Arene1PlayDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_Arene1PlayDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_Arene1PlayDlg *me = static_cast< V3_Arene1PlayDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


void V3_Arene1PlayDlg::RejoindreButtonEvent::LeftClicked( void )
{
   V3_Arene1PlayDlg *me = static_cast< V3_Arene1PlayDlg * >( GetUI() );
   
   
}

void V3_Arene1PlayDlg::QuitterButtonEvent::LeftClicked( void )
{
   V3_Arene1PlayDlg *me = static_cast< V3_Arene1PlayDlg * >( GetUI() );

   TFCPacket sending;
   sending << (RQ_SIZE)RQ_ARENA1_Leave;
   sending << (long)me->m_iArenaID;
   sending << (long)me->m_iArenaType;
   SEND_PACKET( sending );

   me->Hide();

}



void V3_Arene1PlayDlg::PlayerListEvent::LeftClicked( void )
{
   V3_Arene1PlayDlg *me = static_cast< V3_Arene1PlayDlg * >( GetUI() );
   
     
}


void V3_Arene1PlayDlg::ClearAllList()
{
   m_vPLListB.clear();
   m_vPLListR.clear();
}

void V3_Arene1PlayDlg::UpdateInfo(int iArenaID, int iArenaType,short shStayMin,short shStaySec,bool bOnList,int iLevelMin,int iLevelMax,int iPB, int iPR,bool bSetTimer)
{
   m_iArenaID   = iArenaID;
   m_iArenaType = iArenaType;
   m_shTimeStayMin = shStayMin;
   m_shTimeStaySec = shStaySec;

   if(m_shTimeStayMin < 0)
      m_shTimeStayMin = 0;
   if(m_shTimeStaySec < 0)
      m_shTimeStaySec = 0;

   m_bOnArene  = bOnList;
   m_iLevelMin = iLevelMin;
   m_iLevelMax = iLevelMax;
   m_iPointB = iPB;
   m_iPointR = iPR;



   
   if(bSetTimer)
      m_TimerTools.SetNMTimer(1,5000,TimerCallback,this);
}

void V3_Arene1PlayDlg::UpdatePLList(int iPlID,char *pName,int iFlag, int iKill, int iDead,int iTeamID,int iPlPoints)
{
   sAr1PLay newPL;
   newPL.iPlID     = iPlID;
   newPL.strPlName = pName;
   newPL.iFlag     = iFlag;
   newPL.iKill     = iKill;
   newPL.iDead     = iDead;
   newPL.iPlPoints = iPlPoints;
   
   if(iTeamID == 1)
      m_vPLListB.push_back(newPL);
   else
      m_vPLListR.push_back(newPL);
}


void V3_Arene1PlayDlg::UpdateDisplayList()
{
   m_bNeedUpdateList = true;
}

void V3_Arene1PlayDlg::ListToControlDisplay()
{
   PlayerListB.ResetSelectedRow();
   PlayerListB.DeleteList();
   for(unsigned int i=0;i<m_vPLListLB.size();i++)
   {
      PlayerListB.AddNewItem();

      StrListTmp *pTmp = new StrListTmp;
      pTmp->strTmp[0] = 0;
      PlayerListB.AddColumnItem( 0, pTmp );
   }
   PlayerListB.SetScrollPos( 0 );

   PlayerListR.ResetSelectedRow();
   PlayerListR.DeleteList();
   for(unsigned int i=0;i<m_vPLListLR.size();i++)
   {
      PlayerListR.AddNewItem();

      StrListTmp *pTmp = new StrListTmp;
      pTmp->strTmp[0] = 0;
      PlayerListR.AddColumnItem( 0, pTmp );
   }
   PlayerListR.SetScrollPos( 0 );
}


int V3_Arene1PlayDlg::TimerCallback(void* pThis,unsigned int dwCode)
{
   V3_Arene1PlayDlg *me = (V3_Arene1PlayDlg*)pThis;

   me->OnTimer(dwCode);
   return 0;
}

void V3_Arene1PlayDlg::OnTimer(unsigned int nIDEvent)
{
   if(nIDEvent==1)
   {
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_ARENA1_UpdatePlayStat;
      sending << (long)m_iArenaID;
      sending << (long)m_iArenaType;
      SEND_PACKET( sending );
   }
}