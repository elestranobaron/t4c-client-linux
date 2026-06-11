// V3_InteractionRPDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_InteractionRP.h"
#include "V3_ChatLogDlg.h"
#include "V3_PanierDlg.h"
#include "V3_ConfirmDlg.h"
#include "V3_InvDlg.h"
#include "V3_DisplaySpecialDlg.h"
#include "V3_AskValueDlg.h"
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

V3_InteractionRPDlg::V3_InteractionRPDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
PlayerList( playerListEvent ),
PhraseList( phraseListEvent ),
RefreshBtn        ( refreshButtonEvent        ,g_LocalString[769],g_DefColorH ),
CreerBtn          ( crerButtonEvent           ,g_LocalString[749],g_DefColorH ),
RejoindreBtn      ( rejoindreButtonEvent      ,g_LocalString[750],g_DefColorH ),
InviterBtn        ( inviterButtonEvent        ,g_LocalString[751],g_DefColorH ),
QuitterBtn        ( quitterButtonEvent        ,g_LocalString[753],g_DefColorH ),
TerminerBtn       ( terminerButtonEvent       ,g_LocalString[758],g_DefColorH ),
SignalerBtn       ( signalerButtonEvent       ,g_LocalString[754],g_DefColorH ),
ExpulserBtn       ( expulserButtonEvent       ,g_LocalString[752],g_DefColorH )
{
   int i=0;
   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 755 );
  
   m_BackgroundS.SetSpriteId("InteractionRPBack",true,"InteractionRPBackMask");

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
   
   RefreshBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   RefreshBtn.SetDownUI     ( "V3_Btn130x24_D" );
   RefreshBtn.SetHighlightUI( "V3_Btn130x24_H" );
   refreshButtonEvent.SetUI( this );
   SetHelpId( RefreshBtn, 771 );

   CreerBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   CreerBtn.SetDownUI     ( "V3_Btn130x24_D" );
   CreerBtn.SetHighlightUI( "V3_Btn130x24_H" );
   crerButtonEvent.SetUI( this );
   SetHelpId( CreerBtn, 756 );
   
   RejoindreBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   RejoindreBtn.SetDownUI     ( "V3_Btn130x24_D" );
   RejoindreBtn.SetHighlightUI( "V3_Btn130x24_H" );
   rejoindreButtonEvent.SetUI( this );
   SetHelpId( RejoindreBtn, 757 );
   
   QuitterBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   QuitterBtn.SetDownUI     ( "V3_Btn130x24_D" );
   QuitterBtn.SetHighlightUI( "V3_Btn130x24_H" );
   quitterButtonEvent.SetUI( this );
   SetHelpId( QuitterBtn, 759 );

   TerminerBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   TerminerBtn.SetDownUI     ( "V3_Btn130x24_D" );
   TerminerBtn.SetHighlightUI( "V3_Btn130x24_H" );
   terminerButtonEvent.SetUI( this );
   SetHelpId( TerminerBtn, 760 );

   SignalerBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   SignalerBtn.SetDownUI     ( "V3_Btn130x24_D" );
   SignalerBtn.SetHighlightUI( "V3_Btn130x24_H" );
   signalerButtonEvent.SetUI( this );
   SetHelpId( SignalerBtn, 761 );

   ExpulserBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   ExpulserBtn.SetDownUI     ( "V3_Btn130x24_D" );
   ExpulserBtn.SetHighlightUI( "V3_Btn130x24_H" );
   expulserButtonEvent.SetUI( this );
   SetHelpId( ExpulserBtn, 762 );

   InviterBtn.SetSpriteId   ( "V3_Btn130x24_N" );
   InviterBtn.SetDownUI     ( "V3_Btn130x24_D" );
   InviterBtn.SetHighlightUI( "V3_Btn130x24_H" );
   inviterButtonEvent.SetUI( this );
   SetHelpId( InviterBtn, 766 );


   m_backIconsL.SetSpriteId( "RPIconL");
   
   SetHelpId(m_backIconsL,774);
   SetHelpId(m_stLevel   ,774);

   SetHelpId(stPhraseActive  ,778);

   
   

     
   stPhraseActive.SetCenter( false );
   stPhraseComplete.SetCenter( false );
  

   m_stHNom   .SetText(g_LocalString[763],g_DefColorH);
   m_stHPhrase.SetText(g_LocalString[764],g_DefColorH);
  
   PhraseList.SetupList( 2, 20, FramePos( 0, 0, 2, 1 ) );
   PhraseList.SetColumnSize( 0, 167 );
   PhraseList.SetColumnSize( 1, 339 );
   PhraseSelect.SetPos( FramePos( 0, 0, 508, 20 ) );
   PhraseSelect.SetSpriteId( "InteractionRPBackSelectPhrase" );
   PhraseSelect.Show();
   PhraseList.SetSelectionSprite( &PhraseSelect, 0);
   phraseListEvent.SetUI( this );
   PhraseList.SetScrollDownDownUI( &PhraseListScrollDown );
   PhraseList.SetScrollUpDownUI( &PhraseListScrollUp );

   PlayerList.SetupList(1, 20, FramePos( 0, 0, 2, 1 ) );
   PlayerList.SetColumnSize( 0, 167 );
   PlayerSelect.SetPos( FramePos( 0, 0, 168, 20 ) );
   PlayerSelect.SetSpriteId( "InteractionRPBackSelectPlayer" );
   PlayerSelect.Show();
   PlayerList.SetSelectionSprite( &PlayerSelect, 0);
   playerListEvent.SetUI( this );
   PlayerList.SetScrollDownDownUI( &PlayerListScrollDown );
   PlayerList.SetScrollUpDownUI( &PlayerListScrollUp );
   SetHelpId(PlayerList  ,779);


   m_RPBar.SetSpriteId("V3_RPBar");
  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_backIconsL);
   AddChild( &RefreshBtn    );
   AddChild( &CreerBtn    );
   AddChild( &RejoindreBtn);
   AddChild( &InviterBtn  );
   AddChild( &QuitterBtn  );
   AddChild( &TerminerBtn );
   AddChild( &SignalerBtn );
   AddChild( &ExpulserBtn );

   AddChild( &PlayerList );
   AddChild( &PhraseList );
   //AddChild( &stPhraseActive );
   //AddChild( &stPhraseComplete );
   AddChild( &m_stHNom);
   AddChild( &m_stHPhrase);
   AddChild( &m_stLevel);



   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   m_chHaveRP   = 0;
   m_strSubject = "";
   m_chHaveRPL  = 0;
   m_strSubjectL= "";
   m_bNeedUpdateList = false;

   m_iRPlevelPC = 0;
   m_iPoints    = 0;

   
   QuitterBtn.Hide();
   TerminerBtn.Hide();
   ExpulserBtn.Hide();
   InviterBtn.Hide();


   ::InitializeCriticalSection(&crLockList);
} 

V3_InteractionRPDlg::~V3_InteractionRPDlg()
{
   
   ::DeleteCriticalSection(&crLockList);
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_InteractionRPDlg *V3_InteractionRPDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_InteractionRPDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_InteractionRPDlg::Display( void *null)
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

void V3_InteractionRPDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   m_backIconsL.SetPos( FramePos( iPosX+44 , iPosY+71, iPosX+44+28 ,iPosY+71+32));
 
   RefreshBtn    .SetPos( FramePos( iPosX+416, iPosY+210, iPosX+416+130, iPosY+210+24 ) );

   CreerBtn    .SetPos( FramePos( iPosX+416, iPosY+154 , iPosX+416+130, iPosY+154 +24 ) );

   QuitterBtn  .SetPos( FramePos( iPosX+239, iPosY+154, iPosX+239+130, iPosY+154+24 ) );
   TerminerBtn .SetPos( FramePos( iPosX+239, iPosY+154, iPosX+239+130, iPosY+154+24 ) );
   InviterBtn  .SetPos( FramePos( iPosX+239, iPosY+182, iPosX+239+130, iPosY+182+24 ) );
   ExpulserBtn .SetPos( FramePos( iPosX+239, iPosY+210, iPosX+239+130, iPosY+210+24 ) );

   RejoindreBtn.SetPos( FramePos( iPosX+82 , iPosY+403, iPosX+82 +130, iPosY+403+24 ) );
   SignalerBtn .SetPos( FramePos( iPosX+405, iPosY+403, iPosX+405+130, iPosY+403+24 ) );

   m_RPBar     .SetPos( FramePos( iPosX+88, iPosY+88,iPosX+100+88, iPosY+88+11) );
   
   
   stPhraseActive  .SetPos( FramePos( iPosX+41 , iPosY+116, iPosX+542, iPosY+139 ) );
   stPhraseComplete.SetPos( FramePos( iPosX+41 , iPosY+366, iPosX+542, iPosY+390 ) );

   m_stHNom   .SetPos( FramePos( iPosX+40 , iPosY+260, iPosX+201, iPosY+275 ) );
   m_stHPhrase.SetPos( FramePos( iPosX+210, iPosY+260, iPosX+542, iPosY+275 ) );
   m_stLevel  .SetPos( FramePos( iPosX+86 , iPosY+74 , iPosX+190, iPosY+86 ) );
   
   PlayerList.SetListDef(iPosX,iPosY,20,38,143,226,246,&PlayerListScrollUp,&PlayerListScrollDown);
   PhraseList.SetListDef(iPosX,iPosY,20,38,279,567,361,&PhraseListScrollUp,&PhraseListScrollDown);
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_InteractionRPDlg::Draw(V2SPRITEFX *vsfFX)
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

   if(m_bNeedUpdateList)
   {
      LockList();
      m_vRPListL.clear();
      m_vPLListL.clear();

      for(UINT i=0;i<m_vRPList.size();i++)
      {
         sRPList newRP;
         newRP.iMessageID = m_vRPList[i].iMessageID;
         sprintf_s(newRP.strPLName ,50  ,"%s",m_vRPList[i].strPLName);
         sprintf_s(newRP.strMessage,256 ,"%s",m_vRPList[i].strMessage);
         m_vRPListL.push_back(newRP);
      }

      for(UINT i=0;i<m_vPLList.size();i++)
      {
         sPLList newPl;
         newPl.iPlID     = m_vPLList[i].iPlID;
         newPl.strPlName = m_vPLList[i].strPlName;
         m_vPLListL.push_back(newPl);
      }

      m_chHaveRPL   = m_chHaveRP;
      m_strSubjectL = m_strSubject;


      ListToControlDisplay();
      m_bNeedUpdateList = false;
      UnlockList();
   }
  
   

   if(m_chHaveRPL == 0) //pas rp
   {
      QuitterBtn.Hide();
      TerminerBtn.Hide();
      ExpulserBtn.Hide();
      InviterBtn.Hide();

      CreerBtn.Show();
      RejoindreBtn.Show();
      SignalerBtn.Show();
   }
   else if(m_chHaveRPL == 1) //son RP
   {
      CreerBtn.Hide();
      RejoindreBtn.Hide();
      QuitterBtn.Hide();

      TerminerBtn.Show();
      ExpulserBtn.Show();
      InviterBtn.Show();
      SignalerBtn.Show();
   }
   else if(m_chHaveRPL == 2) //dans un RP
   {
      CreerBtn.Hide();
      RejoindreBtn.Hide();
      TerminerBtn.Hide();
      ExpulserBtn.Hide();
      InviterBtn.Hide();

      QuitterBtn.Show();
      SignalerBtn.Show();
   }

   GameUI::Draw( vsfFX );


   DrawSingleStatusItem( m_RPBar, m_iRPlevelPC );
   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   RefreshBtn  .DrawTextButtonExt(hdc);
   CreerBtn    .DrawTextButtonExt(hdc);
   RejoindreBtn.DrawTextButtonExt(hdc);
   InviterBtn  .DrawTextButtonExt(hdc);
   QuitterBtn  .DrawTextButtonExt(hdc);
   TerminerBtn .DrawTextButtonExt(hdc);
   SignalerBtn .DrawTextButtonExt(hdc);
   ExpulserBtn .DrawTextButtonExt(hdc);
   
   // Draw les texte...
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_LocalString[765],strlen(g_LocalString[765]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);


   //Dessine liste player
   char strInfoN[50];
   char strInfoM[256];
   unsigned int indexStart = PlayerList.GetScrollPos();
   int iYStart = 146;
   for(int i=0;i<5;i++)
   {
      if(indexStart < m_vPLListL.size())
      {
         COLORREF crText = g_DefColorD;

         if(indexStart == 0) //premier membre toujours le createur
            crText = CL_RED;
     


         sprintf_s(strInfoN,50  ,"%s",m_vPLListL[indexStart].strPlName.c_str());
         fSmallNormal_12->DrawFont(hdc, iOffX+ 41 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);      
      }
      indexStart++;
   }

   //Dessine liste des RP
   
   indexStart = PhraseList.GetScrollPos();
   iYStart = 282;
   for(int i=0;i<4;i++)
   {
      if(indexStart < m_vRPListL.size())
      {
         COLORREF crText = g_DefColorD;

         sprintf_s(strInfoN,50  ,"%s",m_vRPListL[indexStart].strPLName);
         sprintf_s(strInfoM,256 ,"%s",m_vRPListL[indexStart].strMessage);
         fSmallNormal_12->DrawFont(hdc, iOffX+ 41 , iOffY+iYStart+(i*21) , crText, 0, strInfoN);      

         FormatText MessageFmt;
         MessageFmt.SetText(strInfoM);
         MessageFmt.Format(50);
         if(MessageFmt.GetnbLine() >0)
         {
            char strTmp[1024];
            if(MessageFmt.GetnbLine() > 1)
               sprintf_s(strTmp,1024,"%s...",MessageFmt.GetText(0));
            else
               sprintf_s(strTmp,1024,"%s",MessageFmt.GetText(0));

            fSmallNormal_12->DrawFont(hdc, iOffX+ 210, iOffY+iYStart+(i*21) , crText, 0, strTmp);      
         }
         //fSmallNormal_12->DrawFont(hdc, iOffX+ 210, iOffY+iYStart+(i*21) , crText, 0, strInfoM);      
      }
      indexStart++;
   }
   DXDReleaseDC(hdc, 86);

   if(m_chHaveRPL)
   {
      if(m_chHaveRPL == 1)
         stPhraseActive.SetText(m_strSubjectL.c_str(),CL_GREEN_DARK,520,0,false,2);
      else
         stPhraseActive.SetText(m_strSubjectL.c_str(),g_DefColorD,520,0,false,2);

      stPhraseActive.Draw( vsfFX );
   }
 
   int Row = PhraseList.GetSelectedRow();
   if (Row >= 0 && m_vRPListL.size() >0)
   {
      stPhraseComplete.SetText(m_vRPListL[Row].strMessage,g_DefColorD,540,0,false,2);
      stPhraseComplete.Draw( vsfFX );
   }

   /*
   if(strcmp(m_strDescription,"")!=0)
   {
      achatDescription.SetText(m_strDescription,g_DefColorD,520,0,false,2);
      achatDescription.Draw( vsfFX );
   }
   */
}

void V3_InteractionRPDlg::Hide()
{
   BoxUI::Hide();
}





void V3_InteractionRPDlg::StartDragDlg( MousePos mousePos )
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

void V3_InteractionRPDlg::StopDragDlg()
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
void V3_InteractionRPDlg::DragEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_InteractionRPDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


void V3_InteractionRPDlg::RefreshButtonEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );

   TFCPacket packet;
   packet << (RQ_SIZE)RQ_RP_BroadCastRP;
   SEND_PACKET( packet );
}


void V3_InteractionRPDlg::CrerButtonEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );

   V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,g_LocalString[773] ,"",160,0);
   V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[355] ,g_GUILocalString[11],738,739);
   V3_AskValueDlg::GetInstance()->SetParent(me);
   V3_AskValueDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueDlg::GetInstance()->Show(0);

   me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );
}

void V3_InteractionRPDlg::RejoindreButtonEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );

   int Row = me->PhraseList.GetSelectedRow();

   if (Row >= 0)
   {
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_RP_RejoindreRP;
      sending << (long)me->m_vRPList[Row].iMessageID;
      SEND_PACKET( sending );
   }
}

void V3_InteractionRPDlg::InviterButtonEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );

   MouseDevice md;
   if( md.LockNextEvent( DM_CLICK, me, InvitePlayer, 0 ) )
   {
      me->Hide();
   }
   else
   {
      // Could not lock mouse.
      V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[ 444 ], SYSTEM_COLOR, true);
   }
}

unsigned int V3_InteractionRPDlg::InvitePlayer(void *data )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_InteractionRPDlg *me = reinterpret_cast< V3_InteractionRPDlg * >( data );

   POINT pntMousePos;
   POINT pntWorldPos;

   // Get the mouse pos.
   MouseDevice md;
   md.GetPosition( (int *)&pntMousePos.x, (int *)&pntMousePos.y );

   Player.ScreenPosToWL( &pntMousePos, &pntWorldPos );

   int nX = ( pntMousePos.x + 48 ) / 32;
   int nY = ( pntMousePos.y - 8 ) / 16;

   DWORD dwID = GridID( nX, nY );
   // If found an ID.
   if( dwID != 0 )
   {
      signed char xPos, yPos;
      Objects.Lock(101);
      Objects.Found( &xPos, &yPos, dwID );
      Objects.Unlock(101);

      TFCPacket sending;

      // Send an invitation to the user.
      sending << (RQ_SIZE)RQ_RP_InviteRP;
      sending << (long)dwID;
      sending << (short)( xPos + Player.xPos );
      sending << (short)( yPos + Player.yPos );        

      SEND_PACKET( sending );
   }

   md.ResetStatus();
   me->Display(NULL);

   return 0;
}

void V3_InteractionRPDlg::ExpulserButtonEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );

   int Row = me->PlayerList.GetSelectedRow();

   if (Row >= 0)
   {
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_RP_ExpluserRP;
      sending << (long)me->m_vPLList[Row].iPlID;
      SEND_PACKET( sending );
   }

}

void V3_InteractionRPDlg::QuitterButtonEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );

   TFCPacket sending;
   sending << (RQ_SIZE)RQ_RP_TerminerRP;
   SEND_PACKET( sending );
}

void V3_InteractionRPDlg::TerminerButtonEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );

   TFCPacket sending;
   sending << (RQ_SIZE)RQ_RP_TerminerRP;
   SEND_PACKET( sending );
}

void V3_InteractionRPDlg::SignalerButtonEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );

   int Row = me->PhraseList.GetSelectedRow();

   if (Row >= 0)
   {
	   TFCPacket sending;
	   sending << (RQ_SIZE)RQ_RP_SignalerRP;
	   sending << (long)me->m_vRPList[Row].iMessageID;
	   SEND_PACKET( sending );
   }
}


void V3_InteractionRPDlg::PlayerListEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );
   
     
}

void V3_InteractionRPDlg::PhraseListEvent::LeftClicked( void )
{
   V3_InteractionRPDlg *me = static_cast< V3_InteractionRPDlg * >( GetUI() );


}


void V3_InteractionRPDlg::ClearAllList()
{
   m_vRPList.clear();
   m_vPLList.clear();
   m_chHaveRP = 0;
   m_strSubject = "";
}

void V3_InteractionRPDlg::UpdateInfo(int iPC, int iLevel, int iPoints)
{
   m_iRPlevelPC = iPC;
   m_iPoints    = iPoints;

   char strTmp[20];
   sprintf_s(strTmp,20,"%d",iLevel);
   m_stLevel.SetText(strTmp,g_DefColorD);
}

void V3_InteractionRPDlg::UpdateRPList(int iID,char *pName,char *pstrMsg)
{
   sRPList newRP;
   newRP.iMessageID = iID;
   sprintf_s(newRP.strPLName ,50  ,"%s",pName);
   sprintf_s(newRP.strMessage,256 ,"%s",pstrMsg);
   m_vRPList.push_back(newRP);
}

void V3_InteractionRPDlg::UpdatePLList(int iPlID,char *pName)
{
   sPLList newPL;
   newPL.iPlID     = iPlID;
   newPL.strPlName = pName;
   
   m_vPLList.push_back(newPL);
}

void V3_InteractionRPDlg::UpdateRPActive(char chRPType, char *pstrSubject)
{
   m_chHaveRP   = chRPType;
   if(pstrSubject)
      m_strSubject = pstrSubject;
}

void V3_InteractionRPDlg::UpdateDisplayList()
{
   m_bNeedUpdateList = true;
}

void V3_InteractionRPDlg::ListToControlDisplay()
{
   PhraseList.ResetSelectedRow();
   PlayerList.ResetSelectedRow();

   PhraseList.DeleteList();
   for(unsigned int i=0;i<m_vRPListL.size();i++)
   {
      PhraseList.AddNewItem();

      StrListTmp *pTmp = new StrListTmp;
      pTmp->strTmp[0] = 0;
      PhraseList.AddColumnItem( 0, pTmp );
   }
   PhraseList.SetScrollPos( 0 );


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

void V3_InteractionRPDlg::AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey)
{
   if(!bOK)
      return;

   switch(uiShowCodeP)
   {
      case 0:
      {
         if(strlen(pStrValue1) >= 20)
         {
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_RP_CreerRP;
            sending << (short)strlen( pStrValue1);
            sending << (char *)pStrValue1;
            SEND_PACKET( sending );
         }
         else
         {
            V3_ChatLogDlg::GetInstance()->AddListLogs("", g_GUILocalString[375], CL_RED, true);
         }
      }
      break;
   }
}


void V3_InteractionRPDlg::AcceptPlayer(char *pstrPLName)
{
   char buf[ 1024 ];
   sprintf_s( buf,1024, g_LocalString[ 767 ], pstrPLName );


   V3_ConfirmDlg::GetInstance()->SetDialogText(buf);
   V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[10],g_GUILocalString[11],738,739);
   V3_ConfirmDlg::GetInstance()->SetParent(this);
   V3_ConfirmDlg::GetInstance()->SetXYPos(((g_Global.GetScreenW()-g_AskValueDlgW)/2), ((g_Global.GetScreenH()-g_AskValueDlgH)/2));
   V3_ConfirmDlg::GetInstance()->Show(0);

   RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
}

void V3_InteractionRPDlg::InvitePlayer(char *pstrPLName)
{
   char buf[ 1024 ];
   sprintf_s( buf,1024, g_LocalString[ 780 ], pstrPLName );


   V3_ConfirmDlg::GetInstance()->SetDialogText(buf);
   V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[10],g_GUILocalString[11],738,739);
   V3_ConfirmDlg::GetInstance()->SetParent(this);
   V3_ConfirmDlg::GetInstance()->SetXYPos(((g_Global.GetScreenW()-g_AskValueDlgW)/2), ((g_Global.GetScreenH()-g_AskValueDlgH)/2));
   V3_ConfirmDlg::GetInstance()->Show(1);

   RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
}



void V3_InteractionRPDlg::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   switch(uiShowCodeP)
   {
      case 0:
      {
         char chAccept = 0;
         if(bOK)
            chAccept = 1;

         TFCPacket sending;
         sending << (RQ_SIZE)RQ_RP_RejoindreAnswerRP;
         sending << (char)chAccept;
         SEND_PACKET( sending );
      }
      break;
      case 1:
      {
            char chAccept = 0;
            if(bOK)
               chAccept = 1;

            TFCPacket sending;
            sending << (RQ_SIZE)RQ_RP_InviteAnswerRP;
            sending << (char)chAccept;
            SEND_PACKET( sending );
      }
      break;
   }
}

void V3_InteractionRPDlg::DrawSingleStatusItem(GraphUI &fullBar,int percent)
{
   FramePos fullPos = fullBar.GetPos();
   FramePos emptyPos = fullBar.GetPos();

   // Create a clipping area according to the group member's hp.
   long newWidth = fullPos.Width() * ( 100 - percent ) / 100;

   RECT clipRect = { fullPos.x, fullPos.y, fullPos.maxx - newWidth, fullPos.maxy };

   V2SPRITEFX fx;
   memset( &fx, 0, sizeof( fx ) );

   fx.lpClipRect = &clipRect;
   fullBar.Draw( &fx );
}
