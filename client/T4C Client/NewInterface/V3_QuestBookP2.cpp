// V3_QuestBookP2.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "V3_QuestBookP2.h"
#include "V3_QuestBookP1.h"
#include "V3_ChatLogDlg.h"
#include "V3_ConfirmDlg.h"
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

V3_QuestBookP2::V3_QuestBookP2():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
QuestList       ( questListEvent ),
QuestStepList   ( questStepListEvent ),
btnPage1        ( page1ButtonEvent,g_LocalString[800],g_DefColorH ),
btnPage2        ( page2ButtonEvent,g_LocalString[801],g_DefColorH ),
btnPage3        ( page3ButtonEvent,g_LocalString[802],g_DefColorH ),
btnAbandonner   ( abandonnerButtonEvent,g_LocalString[803],g_DefColorH )
{
   int i=0;
   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 755 );
  
   m_BackgroundS.SetSpriteId("V3_QuestBookP2",true,"V3_QuestBookMask");

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
   

   btnAbandonner.SetSpriteId   ( "V3_Btn92x24_N" );
   btnAbandonner.SetDownUI     ( "V3_Btn92x24_D" );
   btnAbandonner.SetHighlightUI( "V3_Btn92x24_H" );
   
   page1ButtonEvent.SetUI( this );
   page2ButtonEvent.SetUI( this );
   page3ButtonEvent.SetUI( this );
   abandonnerButtonEvent.SetUI( this );
   
  
   QuestList.SetupList( 1, 20, FramePos( 0, 0, 2, 1 ) );
   QuestList.SetColumnSize( 0, 213 );
   QuestSelect.SetPos( FramePos( 0, 0, 216, 20 ) );
   QuestSelect.SetSpriteId( "V3_QuestBookSelect" );
   QuestSelect.Show();
   QuestList.SetSelectionSprite( &QuestSelect, 0);
   questListEvent.SetUI( this );
   QuestList.SetScrollDownDownUI( &QuestListScrollDown );
   QuestList.SetScrollUpDownUI( &QuestListScrollUp );

   QuestStepList.SetupList( 1, 41, FramePos( 0, 0, 2, 1 ) );
   QuestStepList.SetColumnSize( 0, 213 );
   QuestStepSelect.SetPos( FramePos( 0, 0, 216, 41 ) );
   QuestStepSelect.SetSpriteId( "V3_QuestBookP2Select" );
   QuestStepSelect.Show();
   QuestStepList.SetSelectionSprite( &QuestStepSelect, 0);
   questStepListEvent.SetUI( this );
   QuestStepList.SetScrollDownDownUI( &QuestStepListScrollDown );
   QuestStepList.SetScrollUpDownUI( &QuestStepListScrollUp );

   m_stStep1.SetText("",g_DefColorH,210,0,false,2);
   m_stStep2.SetText("",g_DefColorH,210,0,false,2);
   m_stStep3.SetText("",g_DefColorH,210,0,false,2);
   m_stStep4.SetText("",g_DefColorH,210,0,false,2);
   m_stStep5.SetText("",g_DefColorH,210,0,false,2);
   m_stStep6.SetText("",g_DefColorH,210,0,false,2);


   m_stStep1.SetCenter(false);
   m_stStep2.SetCenter(false);
   m_stStep3.SetCenter(false);
   m_stStep4.SetCenter(false);
   m_stStep5.SetCenter(false);
   m_stStep6.SetCenter(false);

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_backIconsL);
   AddChild( &m_backIconsP);
   AddChild( &btnPage1);
   AddChild( &btnPage2);
   AddChild( &btnPage3);
   AddChild( &btnAbandonner);
   
   AddChild( &m_stStep1 );
   AddChild( &m_stStep2 );
   AddChild( &m_stStep3 );
   AddChild( &m_stStep4 );
   AddChild( &m_stStep5 );
   AddChild( &m_stStep6 );

   AddChild( &QuestList );
   AddChild( &QuestStepList );
   AddChild( &m_stHList );
   AddChild( &m_stFList);

   
   
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );


   btnAbandonner.Hide();

   m_bNeedUpdateList   = false;
   m_bNeedUpdateSelect = false;
   m_iSelQuest  = -1;
   m_iSelQuestL = -1;
   ::InitializeCriticalSection(&crLockList);
} 

V3_QuestBookP2::~V3_QuestBookP2()
{
   
   ::DeleteCriticalSection(&crLockList);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
V3_QuestBookP2 *V3_QuestBookP2::GetInstance( void )
{
    static V3_QuestBookP2 instance;
    return &instance;
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_QuestBookP2::Display( void *null)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->Show();

   return 0;
}

void V3_QuestBookP2::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon  .SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button     .SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button  .SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   m_backIconsL .SetPos( FramePos( iPosX+44 , iPosY+71, iPosX+44+28 ,iPosY+71+32));
   m_backIconsP .SetPos( FramePos( iPosX+214, iPosY+71, iPosX+214+28,iPosY+71+32));

   QuestStepList.SetListDef(iPosX,iPosY,20,313,119,547,369,&QuestStepListScrollUp,&QuestStepListScrollDown);
   QuestList    .SetListDef(iPosX,iPosY,20,65,130,299,359,&QuestListScrollUp,&QuestListScrollDown);
    
   btnPage1     .SetPos( FramePos( iPosX+33 , iPosY+63 , iPosX+134, iPosY+84 ) );
   btnPage2     .SetPos( FramePos( iPosX+136, iPosY+63 , iPosX+237, iPosY+84 ) );
   btnPage3     .SetPos( FramePos( iPosX+239, iPosY+63 , iPosX+340, iPosY+84 ) );
   btnAbandonner.SetPos( FramePos( iPosX+380, iPosY+403, iPosX+380+92, iPosY+403+24 ) );

   m_stStep1    .SetPos( FramePos( iPosX+316 , iPosY+121, iPosX+522, iPosY+121+37 ) );
   m_stStep2    .SetPos( FramePos( iPosX+316 , iPosY+163, iPosX+522, iPosY+163+37 ) );
   m_stStep3    .SetPos( FramePos( iPosX+316 , iPosY+205, iPosX+522, iPosY+205+37 ) );
   m_stStep4    .SetPos( FramePos( iPosX+316 , iPosY+247, iPosX+522, iPosY+247+37 ) );
   m_stStep5    .SetPos( FramePos( iPosX+316 , iPosY+289, iPosX+522, iPosY+289+37 ) );
   m_stStep6    .SetPos( FramePos( iPosX+316 , iPosY+331, iPosX+522, iPosY+331+37 ) );

   m_stHList    .SetPos( FramePos( iPosX+67  , iPosY+105, iPosX+275, iPosY+120 ) );
   m_stFList    .SetPos( FramePos( iPosX+67  , iPosY+369, iPosX+275, iPosY+384 ) );
  
  
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_QuestBookP2::Draw(V2SPRITEFX *vsfFX)
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
      for(UINT i=0;i<m_vQuestListL.size();i++)
         m_vQuestListL[i].aStepMsg.clear();
      m_vQuestListL.clear();

      std::sort(m_vQuestList.begin(), m_vQuestList.end(), sQuestActiveList::compareMyDataPredicate);


      for(UINT i=0;i<m_vQuestList.size();i++)
      {
         sQuestActiveList newQuest;
         newQuest.ushUniqueID   = m_vQuestList[i].ushUniqueID;
         newQuest.ushQuestLevel = m_vQuestList[i].ushQuestLevel;
         newQuest.chStep        = m_vQuestList[i].chStep;
         newQuest.strName       = m_vQuestList[i].strName;

         for(UINT s=0;s<m_vQuestList[i].aStepMsg.size();s++)
         {
            newQuest.aStepMsg.push_back(m_vQuestList[i].aStepMsg[s]);
         }
         m_vQuestListL.push_back(newQuest);
      }
      ListToControlDisplay();
      m_bNeedUpdateList = false;
      m_iSelQuest = -1;
      m_bNeedUpdateSelect = true;

      char strTmp[100];
      sprintf_s(strTmp,100,"%d %s",m_vQuestListL.size(),g_LocalString[805]);
      m_stFList   .SetText(strTmp,g_DefColorH,g_Global.GetScreenW(),0,false,3);
      UnlockList();
   }

   if(m_bNeedUpdateSelect)
   {
      LockList();
      m_iSelQuestL = m_iSelQuest;
      ListToControlDisplayStep();
      m_bNeedUpdateSelect = false;
      UnlockList();
   }

   GameUI::Draw( vsfFX );

   HDC hdc;
   DXDGetDC(&hdc, 86);

   btnPage1.DrawTextButtonExt(hdc);
   btnPage2.DrawTextButtonExt(hdc);
   btnPage3.DrawTextButtonExt(hdc);
   btnAbandonner.DrawTextButtonExt(hdc);

  
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
         COLORREF crText = QuestColoybyLevel(m_vQuestListL[indexStart].ushQuestLevel);/*g_DefColorD*/;
         // on chnagera couleur selon level... ou faite ou pas plus tard
     
         sprintf_s(strTmp,1024  ,"%s",m_vQuestListL[indexStart].strName.c_str());
         fT4CDef_18->DrawFont(hdc, iOffX+ 67 , iOffY+iYStart+(i*21) , crText, 0, strTmp);      

         sprintf_s(strTmp,1024  ,"Nv: %d",m_vQuestListL[indexStart].ushQuestLevel);
         fT4CDef_18->DrawFont(hdc, iOffX+ 232 , iOffY+iYStart+(i*21) , crText, 0, strTmp);      
      }
      indexStart++;
   }
   
   DXDReleaseDC(hdc, 86);

   //Dessine la liste des step...
   indexStart = QuestStepList.GetScrollPos();
   iYStart = 122;
   m_stStep1.SetText("",g_DefColorH,210,0,false,2);
   m_stStep2.SetText("",g_DefColorH,210,0,false,2);
   m_stStep3.SetText("",g_DefColorH,210,0,false,2);
   m_stStep4.SetText("",g_DefColorH,210,0,false,2);
   m_stStep5.SetText("",g_DefColorH,210,0,false,2);
   m_stStep6.SetText("",g_DefColorH,210,0,false,2);
   
   if(m_iSelQuestL >=0)
   {
      //if(!btnAbandonner.IsShown())
      //   btnAbandonner.Show();
      for(int i=0;i<6;i++)
      {

         if(indexStart < m_vQuestListL[m_iSelQuestL].aStepMsg.size())
         {
            COLORREF crText = g_DefColorH;
            if(indexStart+1 == m_vQuestListL[m_iSelQuestL].chStep)
               crText = RGB(50,168,30); //en cours
            else if(indexStart+1 > m_vQuestListL[m_iSelQuestL].chStep)
               crText = g_DefColorD; //completer

                 if(i == 0)  m_stStep1.SetText(m_vQuestListL[m_iSelQuestL].aStepMsg[indexStart].c_str(),crText,210,0,false,2);
            else if(i == 1)  m_stStep2.SetText(m_vQuestListL[m_iSelQuestL].aStepMsg[indexStart].c_str(),crText,210,0,false,2);
            else if(i == 2)  m_stStep3.SetText(m_vQuestListL[m_iSelQuestL].aStepMsg[indexStart].c_str(),crText,210,0,false,2);
            else if(i == 3)  m_stStep4.SetText(m_vQuestListL[m_iSelQuestL].aStepMsg[indexStart].c_str(),crText,210,0,false,2);
            else if(i == 4)  m_stStep5.SetText(m_vQuestListL[m_iSelQuestL].aStepMsg[indexStart].c_str(),crText,210,0,false,2);
            else if(i == 5)  m_stStep6.SetText(m_vQuestListL[m_iSelQuestL].aStepMsg[indexStart].c_str(),crText,210,0,false,2);

         }
         indexStart++;
      }
   }
   else
   {
      //if(btnAbandonner.IsShown())
      //   btnAbandonner.Hide();
   }

   m_stStep1.Draw( vsfFX );
   m_stStep2.Draw( vsfFX );
   m_stStep3.Draw( vsfFX );
   m_stStep4.Draw( vsfFX );
   m_stStep5.Draw( vsfFX );
   m_stStep6.Draw( vsfFX );
}

COLORREF  V3_QuestBookP2::QuestColoybyLevel(int iQL)
{
   int iLevel = Player.Level;

   int iVertMin  = iLevel- (1+(Player.Level/5));
   int iVertMax  = iLevel+ (1+(Player.Level/10));
   int iJauneMin = iLevel+ (1+(Player.Level/5));

   if(iQL >=iVertMin && iQL < iVertMax)
      return CL_GREEN;
   if(iQL <=iVertMin)
      return CL_GRAY_LIGHT;
   if(iQL <=iJauneMin)
      return CL_YELLOW;
   else
      return CL_RED;
}

void V3_QuestBookP2::Hide()
{
   BoxUI::Hide();
}





void V3_QuestBookP2::StartDragDlg( MousePos mousePos )
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

void V3_QuestBookP2::StopDragDlg()
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
void V3_QuestBookP2::DragEvent::LeftClicked( void )
{
   V3_QuestBookP2 *me = static_cast< V3_QuestBookP2 * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_QuestBookP2::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_QuestBookP2 *me = static_cast< V3_QuestBookP2 * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


void V3_QuestBookP2::Page1ButtonEvent::LeftClicked( void )
{
   V3_QuestBookP2 *me = static_cast< V3_QuestBookP2 * >( GetUI() );

   TFCPacket packet;
   packet << (RQ_SIZE)RQ_QB_GetQuestList;
   SEND_PACKET( packet );
}

void V3_QuestBookP2::Page2ButtonEvent::LeftClicked( void )
{
   V3_QuestBookP2 *me = static_cast< V3_QuestBookP2 * >( GetUI() );
}

void V3_QuestBookP2::Page3ButtonEvent::LeftClicked( void )
{
   V3_QuestBookP2 *me = static_cast< V3_QuestBookP2 * >( GetUI() );

   TFCPacket packet;
   packet << (RQ_SIZE)RQ_QB_GetQuestListComplete;
   SEND_PACKET( packet );
}

void V3_QuestBookP2::AbandonnerButtonEvent::LeftClicked( void )
{
   V3_QuestBookP2 *me = static_cast< V3_QuestBookP2 * >( GetUI() );

   if(me->m_iSelQuestL >=0)
   {
      me->m_ushRemoveQuestID =  me->m_vQuestListL[me->m_iSelQuestL].ushUniqueID;

      V3_ConfirmDlg::GetInstance()->SetDialogText(g_GUILocalString[386]);
      V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
      V3_ConfirmDlg::GetInstance()->SetParent(me);
      V3_ConfirmDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
      V3_ConfirmDlg::GetInstance()->Show(0);

      me->RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
   }
   else
   {
      V3_ChatLogDlg::GetInstance()->AddListLogs("", g_LocalString[ 804 ], CL_YELLOW, true);
      //me->btnAbandonner.Hide();
   }
}


void V3_QuestBookP2::QuestListEvent::LeftClicked( void )
{
   V3_QuestBookP2 *me = static_cast< V3_QuestBookP2 * >( GetUI() );

   me->LockList();
   me->m_bNeedUpdateSelect = true;
   

   int Row = me->QuestList.GetSelectedRow();
   if (Row >= 0 && me->m_vQuestListL.size() >0)
   {
      me->m_iSelQuest = Row;
   }
   me->UnlockList();
}

void V3_QuestBookP2::QuestStepListEvent::LeftClicked( void )
{
   V3_QuestBookP2 *me = static_cast< V3_QuestBookP2 * >( GetUI() );
}




void V3_QuestBookP2::ClearAllList()
{
   m_stStep1.SetText("",g_DefColorH,210,0,false,2);
   m_stStep2.SetText("",g_DefColorH,210,0,false,2);
   m_stStep3.SetText("",g_DefColorH,210,0,false,2);
   m_stStep4.SetText("",g_DefColorH,210,0,false,2);
   m_stStep5.SetText("",g_DefColorH,210,0,false,2);
   m_stStep6.SetText("",g_DefColorH,210,0,false,2);

   //btnAbandonner.Hide();

   m_iSelQuest  = -1;
   m_iSelQuestL = -1;

   for(UINT i=0;i<m_vQuestList.size();i++)
      m_vQuestList[i].aStepMsg.clear();
   m_vQuestList.clear();

   for(UINT i=0;i<m_vQuestListL.size();i++)
      m_vQuestListL[i].aStepMsg.clear();
   m_vQuestListL.clear();

   QuestList.ResetSelectedRow();
   QuestList.DeleteList();

   QuestStepList.ResetSelectedRow();
   QuestStepList.DeleteList();

  
}


void V3_QuestBookP2::UpdateQuestList(USHORT ushUniqueID,USHORT ushLevel,BYTE chStep,char *pstrName,std::vector<std::string> &aStepMsg)
{
   sQuestActiveList newQuest;

   newQuest.ushUniqueID   = ushUniqueID;
   newQuest.ushQuestLevel = ushLevel;
   newQuest.chStep        = chStep;
   newQuest.strName       = pstrName;

   for(UINT i=0;i<aStepMsg.size();i++)
   {
      newQuest.aStepMsg.push_back(aStepMsg[i]);
   }
   m_vQuestList.push_back(newQuest);
}



void V3_QuestBookP2::UpdateDisplayList()
{
   m_bNeedUpdateList = true;
}

void V3_QuestBookP2::ListToControlDisplay()
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

void V3_QuestBookP2::ListToControlDisplayStep()
{
   QuestStepList.ResetSelectedRow();
   QuestStepList.DeleteList();
  
   if(m_iSelQuestL >=0 && m_iSelQuestL < (int)m_vQuestListL.size())
   {
      for(unsigned int i=0;i<m_vQuestListL[m_iSelQuestL].aStepMsg.size();i++)
      {
         QuestStepList.AddNewItem();
         StrListTmp *pTmp = new StrListTmp;
         pTmp->strTmp[0] = 0;
         QuestStepList.AddColumnItem( 0, pTmp );
      }
   }
   else
   {
      m_iSelQuestL = -1;
   }

   
   QuestStepList.SetScrollPos( 0 );
}

void V3_QuestBookP2::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   switch(uiShowCodeP)
   {
      case 0:
      {
         if(bOK)
         {
           //Send packet
           TFCPacket packet;
           packet << (RQ_SIZE)RQ_QB_StopQuest;
           packet << (short)m_ushRemoveQuestID;
           SEND_PACKET( packet );

           m_ushRemoveQuestID = 0;

         }
      }
      break;
   }
}