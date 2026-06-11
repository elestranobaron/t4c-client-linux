// V3_QuestBookP3.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_QuestBookP3.h"
#include "V3_QuestBookP1.h"
#include "V3_ChatLogDlg.h"
#include "..\FormatText.h"
#include "..\Bitmap.h"
#include "..\SaveGame.h"

extern CSaveGame g_SaveGame; 
extern Bitmap GridID;
extern T3VSBSound SoundFX[250];
extern Font *fSmallNormal_12; 
extern Font *fT4CDef_18;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_QuestBookP3::V3_QuestBookP3():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
QuestList( questListEvent ),
btnPage1        ( page1ButtonEvent,g_LocalString[800],g_DefColorH ),
btnPage2        ( page2ButtonEvent,g_LocalString[801],g_DefColorH ),
btnPage3        ( page3ButtonEvent,g_LocalString[802],g_DefColorH )
{
   int i=0;
   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 755 );
  
   m_BackgroundS.SetSpriteId("V3_QuestBookP3",true,"V3_QuestBookMask");

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

   m_stHList   .SetText(g_LocalString[797],g_DefColorH,g_Global.GetScreenW(),0,false,3);
   
   
   page1ButtonEvent.SetUI( this );
   page2ButtonEvent.SetUI( this );
   page3ButtonEvent.SetUI( this );
   
  
   QuestList.SetupList( 1, 20, FramePos( 0, 0, 2, 1 ) );
   QuestList.SetColumnSize( 0, 213 );
   QuestSelect.SetPos( FramePos( 0, 0, 216, 20 ) );
   QuestSelect.SetSpriteId( "V3_QuestBookSelect" );
   QuestSelect.Show();
   QuestList.SetSelectionSprite( &QuestSelect, 0);
   questListEvent.SetUI( this );
   QuestList.SetScrollDownDownUI( &QuestListScrollDown );
   QuestList.SetScrollUpDownUI( &QuestListScrollUp );

  
   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_backIconsL);
   AddChild( &m_backIconsP);
   AddChild( &btnPage1);
   AddChild( &btnPage2);
   AddChild( &btnPage3);

   AddChild( &QuestList );
   AddChild( &m_stHList );
   AddChild( &m_stFList);
   
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   m_bNeedUpdateList   = false;
   ::InitializeCriticalSection(&crLockList);
} 

V3_QuestBookP3::~V3_QuestBookP3()
{
   
   ::DeleteCriticalSection(&crLockList);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
V3_QuestBookP3 *V3_QuestBookP3::GetInstance( void )
{
    static V3_QuestBookP3 instance;
    return &instance;
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_QuestBookP3::Display( void *null)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->Show();

   return 0;
}

void V3_QuestBookP3::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon  .SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button     .SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button  .SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   m_backIconsL .SetPos( FramePos( iPosX+44 , iPosY+71, iPosX+44+28 ,iPosY+71+32));
   m_backIconsP .SetPos( FramePos( iPosX+214, iPosY+71, iPosX+214+28,iPosY+71+32));
 
   btnPage1     .SetPos( FramePos( iPosX+33 , iPosY+63 , iPosX+134, iPosY+84 ) );
   btnPage2     .SetPos( FramePos( iPosX+136, iPosY+63 , iPosX+237, iPosY+84 ) );
   btnPage3     .SetPos( FramePos( iPosX+239, iPosY+63 , iPosX+340, iPosY+84 ) );

   m_stHList    .SetPos( FramePos( iPosX+67  , iPosY+105, iPosX+275, iPosY+120 ) );
   m_stFList    .SetPos( FramePos( iPosX+67  , iPosY+369, iPosX+275, iPosY+384 ) );
 
   QuestList    .SetListDef(iPosX,iPosY,20,65,130,299,359,&QuestListScrollUp,&QuestListScrollDown);
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_QuestBookP3::Draw(V2SPRITEFX *vsfFX)
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
      m_vQuestListL.clear();

      std::sort(m_vQuestList.begin(), m_vQuestList.end(), sQuestCompleteList::compareMyDataPredicate);

      for(UINT i=0;i<m_vQuestList.size();i++)
      {
         sQuestCompleteList newQuest;
         newQuest.ushUniqueID   = m_vQuestList[i].ushUniqueID;
         newQuest.ushQuestLevel = m_vQuestList[i].ushQuestLevel;
         newQuest.strName       = m_vQuestList[i].strName;
         m_vQuestListL.push_back(newQuest);
      }
      ListToControlDisplay();
      m_bNeedUpdateList = false;

      char strTmp[100];
      sprintf_s(strTmp,100,"%d %s",m_vQuestListL.size(),g_LocalString[806]);
      m_stFList   .SetText(strTmp,g_DefColorH,g_Global.GetScreenW(),0,false,3);
      UnlockList();
   }


   GameUI::Draw( vsfFX );

   HDC hdc;
   DXDGetDC(&hdc, 86);

   btnPage1.DrawTextButtonExt(hdc);
   btnPage2.DrawTextButtonExt(hdc);
   btnPage3.DrawTextButtonExt(hdc);

  
   // Draw les texte...
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_LocalString[799],strlen(g_LocalString[799]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);

   //Dessine liste Active quest
   char strTmp[1024];
   unsigned int indexStart = QuestList.GetScrollPos();
   int iYStart = 132;
   for(int i=0;i<11;i++)
   {
      if(indexStart < m_vQuestListL.size())
      {
         COLORREF crText = g_DefColorD;
         // on chnagera couleur selon level... ou faite ou pas plus tard
     
         sprintf_s(strTmp,1024  ,"%s",m_vQuestListL[indexStart].strName.c_str());
         fT4CDef_18->DrawFont(hdc, iOffX+ 67 , iOffY+iYStart+(i*21) , crText, 0, strTmp);      

         sprintf_s(strTmp,1024  ,"Nv: %d",m_vQuestListL[indexStart].ushQuestLevel);
         fT4CDef_18->DrawFont(hdc, iOffX+ 232 , iOffY+iYStart+(i*21) , crText, 0, strTmp);      
      }
      indexStart++;
   }
   
   DXDReleaseDC(hdc, 86);
}

void V3_QuestBookP3::Hide()
{
   BoxUI::Hide();
}





void V3_QuestBookP3::StartDragDlg( MousePos mousePos )
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

void V3_QuestBookP3::StopDragDlg()
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
void V3_QuestBookP3::DragEvent::LeftClicked( void )
{
   V3_QuestBookP3 *me = static_cast< V3_QuestBookP3 * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_QuestBookP3::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_QuestBookP3 *me = static_cast< V3_QuestBookP3 * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


void V3_QuestBookP3::Page1ButtonEvent::LeftClicked( void )
{
   V3_QuestBookP3 *me = static_cast< V3_QuestBookP3 * >( GetUI() );

   TFCPacket packet;
   packet << (RQ_SIZE)RQ_QB_GetQuestList;
   SEND_PACKET( packet );
}

void V3_QuestBookP3::Page2ButtonEvent::LeftClicked( void )
{
   V3_QuestBookP3 *me = static_cast< V3_QuestBookP3 * >( GetUI() );

   TFCPacket packet;
   packet << (RQ_SIZE)RQ_QB_GetActiveQuest;
   SEND_PACKET( packet );
}

void V3_QuestBookP3::Page3ButtonEvent::LeftClicked( void )
{
   V3_QuestBookP3 *me = static_cast< V3_QuestBookP3 * >( GetUI() );
}


void V3_QuestBookP3::QuestListEvent::LeftClicked( void )
{
   V3_QuestBookP3 *me = static_cast< V3_QuestBookP3 * >( GetUI() );
}



void V3_QuestBookP3::ClearAllList()
{
   m_vQuestList.clear();

   QuestList.ResetSelectedRow();
   QuestList.DeleteList();
}


void V3_QuestBookP3::UpdateQuestList(USHORT ushUniqueID,USHORT ushLevel,char *pstrName)
{
   sQuestCompleteList newQuest;

   newQuest.ushUniqueID   = ushUniqueID;
   newQuest.ushQuestLevel = ushLevel;
   newQuest.strName       = pstrName;
   m_vQuestList.push_back(newQuest);
}



void V3_QuestBookP3::UpdateDisplayList()
{
   m_bNeedUpdateList = true;
}

void V3_QuestBookP3::ListToControlDisplay()
{
   QuestList.ResetSelectedRow();

   QuestList.DeleteList();
   for(unsigned int i=0;i<m_vQuestListL.size();i++)
   {
      QuestList.AddNewItem();

      StrListTmp *pTmp = new StrListTmp;
      pTmp->strTmp[0] = 0;
      QuestList.AddColumnItem( 0, pTmp );
   }
   QuestList.SetScrollPos( 0 );
}




