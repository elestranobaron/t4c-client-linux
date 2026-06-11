// V3_ChatLogDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_ChatLogDlg.h"
#include "RootBoxUI.h"
#include "V3_StatDlg.h"
#include "V3_ChatDlg.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "..\SysMsg.h"

extern CSaveGame g_SaveGame;


const int g_iMinSizeY = 91;  //20+20+20+20+11   //Pin ScrollUP + Tick + ScrollDown + GoBottom 
const int g_iMinSizeX = 200; 


const int g_iSizeYOff = 15;
const int g_iSizeXOff = 15;

extern TemplateList <SysMsg> tlSystemMessage;
void ShowSystemMessage(char *szText, int iColor, int iTime, bool allowNewLine ) 
{ 
   tlSystemMessage.Lock("Add Message");
   if(!V3_ChatLogDlg::GetInstance()->IsShown())
   {
      SysMsg *Msg = new SysMsg; 
      Msg->SetText( szText, iColor, iTime, allowNewLine ,483);
      tlSystemMessage.AddToTail(Msg); 
      
      if(tlSystemMessage.GetNbObjects() >5) 
      {
         tlSystemMessage.ToHead();
         SysMsg *pMsgDel = tlSystemMessage.GetObject();
         delete pMsgDel;
         pMsgDel = NULL;
         tlSystemMessage.RemoveObject();
      }
      
   }
   else
   {
      tlSystemMessage.DestroyList(); 
   }
   tlSystemMessage.Unlock("Add Message");
   
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_ChatLogDlg::V3_ChatLogDlg():
Drag_Button(dragEvent),
UPSize_Button(sizeEvent),
RightSize_Button(sizeEvent),
m_ListEnd(listEndEvent),
m_ListPinE(pinEvent),
m_ListPinD(pinEvent)
{
   int i=0;

   m_bDragDialog  = false;
   m_bSizeDialogX = false;
   m_bSizeDialogY = false;
   m_iXPos = g_Global.GetDialogLastChatLogXPos();
   m_iYPos = g_Global.GetDialogLastChatLogYPos();
   m_iHVal = g_Global.GetDialogLastChatLogH();
   m_iWVal = g_Global.GetDialogLastChatLogW();
   
 
   m_Background.SetSpriteId("V3_ChatLogBack",true,"V3_ChatLogBackMask");
   m_BackgroundF.SetSpriteId("V3_ChatLogBack",true,"V3_ChatLogBackMask2");
   m_RightScroll.SetSpriteId("V3_ChatLogScroll",true,"V3_ChatLogScrollMask");
   m_RightScrollUPBtn.SetSpriteId("V3_BtnSliderUP_N");
   m_RightScrollDNBtn.SetSpriteId("V3_BtnSliderDN_N");
      
 
   
   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );

   UPSize_Button.SetSpriteId   ( "" );
   UPSize_Button.SetDownUI     ( "" );
   UPSize_Button.SetHighlightUI( "" ); 
   RightSize_Button.SetSpriteId   ( "" );
   RightSize_Button.SetDownUI     ( "" );
   RightSize_Button.SetHighlightUI( "" ); 
   sizeEvent.SetUI    ( this );

   
   //m_ListScroll.SetSpriteId( "V3_VSliderTick" );
   m_ListScroll.SetSpriteId( "V3_VSliderTickA",true,"V3_VSliderTickAMask" );
   m_ListScroll.SetPos( FramePos( 0, 0, 16, 20) );
   
   SetHelpId( m_ListText, 254 );
   m_ListText.SetScrollDownDownUI( &m_ListScrollDown );
   m_ListText.SetScrollUpDownUI( &m_ListScrollUp );
   m_ListText.SetScrollUI( &m_ListScroll );

   m_ListEnd.SetSpriteId   ( "V3_SpinDN_H" );
   listEndEvent.SetUI    ( this );

   m_ListPinE.SetSpriteId   ( "V3_BtnPinOn_N" );
   m_ListPinE.SetHighlightUI( "V3_BtnPinOn_H" ); 
   m_ListPinD.SetSpriteId   ( "V3_BtnPinOff_N" );
   m_ListPinD.SetHighlightUI( "V3_BtnPinOff_H" );
   pinEvent.SetUI    ( this );
   
  
  

   UpdateAllFramePos(m_iXPos,m_iYPos,m_iWVal,m_iHVal);

   //AddChild( &m_Background);
   AddChild( &m_RightScroll);
   AddChild( &m_RightScrollUPBtn);
   AddChild( &m_RightScrollDNBtn);
   AddChild( &m_ListText );
   AddChild( &Drag_Button);
   AddChild( &UPSize_Button);
   AddChild( &RightSize_Button);
   AddChild( &m_ListEnd);
   AddChild( &m_ListPinE);
   AddChild( &m_ListPinD);


   m_iPinnedStatus = g_Global.GetDialogLastChatLogP();
   if(m_iPinnedStatus == 0)
   {
      m_ListPinD.Show();
      m_ListPinE.Hide();
   }
   else
   {
      m_ListPinE.Show();
      m_ListPinD.Hide();
   }

   
  
   RootBoxUI::GetInstance()->AddChild( this );
} 

V3_ChatLogDlg::~V3_ChatLogDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_ChatLogDlg *V3_ChatLogDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_ChatLogDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_ChatLogDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iPinnedStatus = g_Global.GetDialogLastChatLogP();
   if(GetInstance()->m_iPinnedStatus == 0)
   {
      GetInstance()->m_ListPinD.Show();
      GetInstance()->m_ListPinE.Hide();
   }
   else
   {
      GetInstance()->m_ListPinE.Show();
      GetInstance()->m_ListPinD.Hide();
   }
   
   GetInstance()->m_iXPos = g_Global.GetDialogLastChatLogXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastChatLogYPos();
   GetInstance()->m_iWVal = g_Global.GetDialogLastChatLogW();
   GetInstance()->m_iHVal = g_Global.GetDialogLastChatLogH();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos,GetInstance()->m_iWVal,GetInstance()->m_iHVal);

   GetInstance()->Show();
   
   return 0;
}

void V3_ChatLogDlg::UpdateAllFramePos(int iPosX,int iPosY, int iW, int iH)
{
   
   


   if(m_iPinnedStatus == 0)
   {
      
      SetPos( FramePos( iPosX, iPosY, iPosX+iW, iPosY+iH ) );
      m_Background.SetPos( FramePos( iPosX, iPosY, iPosX+iW, iPosY+iH ) );
      m_BackgroundF.SetPos( FramePos( iPosX, iPosY, iPosX+iW, iPosY+iH ) );
      
     
      //left scroll
      Drag_Button      .SetPos( FramePos( iPosX+25, iPosY+g_iSizeYOff, iPosX+iW-g_iSizeXOff, iPosY+iH ) );
      UPSize_Button    .SetPos( FramePos( iPosX+25, iPosY-g_iSizeYOff, iPosX+iW-g_iSizeXOff, iPosY+g_iSizeYOff ) );
      RightSize_Button .SetPos( FramePos( iPosX+iW-g_iSizeXOff, iPosY+g_iSizeYOff, iPosX+iW+g_iSizeXOff, iPosY+iH ) );
      
      m_RightScroll.SetPos( FramePos( iPosX, iPosY, iPosX+20, iPosY+iH ));
      m_ListEnd.SetPos( FramePos( iPosX+2, iPosY, iPosX+18, iPosY+11) );
      m_RightScrollUPBtn.SetPos( FramePos( iPosX, iPosY+11, iPosX+20, iPosY+11+20) );
      m_RightScrollDNBtn.SetPos( FramePos( iPosX, iPosY+iH-40, iPosX+20, iPosY+iH-20) );
      m_ListPinE.SetPos( FramePos( iPosX, iPosY+iH-20, iPosX+20, iPosY+iH) );
      m_ListPinD.SetPos( FramePos( iPosX, iPosY+iH-20, iPosX+20, iPosY+iH) );
      m_ListText.SetPageDef(iPosX,iPosY,20,0,0,iW,iH,&m_ListScrollUp,&m_ListScrollDown,11,20,false,true);
   }
   else
   {
      SetPos( FramePos( iPosX, iPosY, iPosX+20, iPosY+iH ) );
      m_Background.SetPos( FramePos( iPosX, iPosY, iPosX+iW, iPosY+iH ) );
      m_BackgroundF.SetPos( FramePos( iPosX, iPosY, iPosX+iW, iPosY+iH ) );
      
      
      //left scroll
      Drag_Button      .SetPos( FramePos( 0, 0, 0, 0 ) ); //pas de drag button
      UPSize_Button    .SetPos( FramePos( 0, 0, 0, 0 ) ); //pas de drag button
      RightSize_Button .SetPos( FramePos( 0, 0, 0, 0 ) ); //pas de drag button
      
      m_RightScroll.SetPos( FramePos( iPosX, iPosY, iPosX+20, iPosY+iH ));
      m_ListEnd.SetPos( FramePos( iPosX+2, iPosY, iPosX+18, iPosY+11) );
      m_RightScrollUPBtn.SetPos( FramePos( iPosX, iPosY+11, iPosX+20, iPosY+11+20) );
      m_RightScrollDNBtn.SetPos( FramePos( iPosX, iPosY+iH-40, iPosX+20, iPosY+iH-20) );
      m_ListPinE.SetPos( FramePos( iPosX, iPosY+iH-20, iPosX+20, iPosY+iH) );
      m_ListPinD.SetPos( FramePos( iPosX, iPosY+iH-20, iPosX+20, iPosY+iH) );
      m_ListText.SetPageDef(iPosX,iPosY,20,0,0,iW,iH,&m_ListScrollUp,&m_ListScrollDown,11,20,false,true);
   }

   

    //right scroll
   /*
   Drag_Button .SetPos( FramePos( iPosX, iPosY, iPosX+g_ChatLogDlgW-25, iPosY+iH ) );
   m_RightScroll.SetPos( FramePos( iPosX+g_ChatLogDlgW-20, iPosY, iPosX+g_ChatLogDlgW, iPosY+iH ));
   m_RightScrollUPBtn.SetPos( FramePos( iPosX+g_ChatLogDlgW-20, iPosY+11, iPosX+g_ChatLogDlgW, iPosY+11+20) );
   m_RightScrollDNBtn.SetPos( FramePos( iPosX+g_ChatLogDlgW-20, iPosY+iH-20, iPosX+g_ChatLogDlgW, iPosY+iH) );
   m_ListEnd.SetPos( FramePos( iPosX+g_ChatLogDlgW-18, iPosY, iPosX+g_ChatLogDlgW-2, iPosY+11) );
   m_ListText.SetPageDef(iPosX,iPosY,20,0,0,g_ChatLogDlgW,iH,&m_ListScrollUp,&m_ListScrollDown,11,20,false,false);
   
   */
   
}

void V3_ChatLogDlg::AddListLogs(string fromUser, string newText, DWORD  baseColor, bool screenEcho, bool allowNewLine)
{
   if( screenEcho )
   {
      std::string newStr = newText;
      ShowSystemMessage( (char*)newStr.c_str(), baseColor, 7500, allowNewLine );
   } 

   //Doit valider si le user est dans la liste des IGNORE... et ajouter le log...
   
   if(!V3_ChatDlg::GetInstance()->IsUserIgnored(fromUser))
   {
      m_ListText.AddText( newText.c_str(), baseColor,true);
      
      OptionParam *pOption = g_SaveGame.GetOptionsParam();
       
      if( pOption->bLogEnable)
      {
         ClientConfig *pConfig = g_Global.GetClientConfig(); 
         
         char strLogFile[1024];
         
         sprintf_s(strLogFile,1024,"%s\\Logs\\%s",pConfig->strSavePath,pOption->LogName);
        
         FILE *f = NULL;
         fopen_s(&f, strLogFile, "ab" ); //oki logs
         if( f != NULL )
         {
            SYSTEMTIME curTime;
            GetLocalTime( &curTime );
            
            char buf[ 1024 ];
            sprintf_s( buf,1024, "%02u/%02u/%u %02u:%02u:%02u-- ", curTime.wMonth,curTime.wDay,curTime.wYear,curTime.wHour,curTime.wMinute,curTime.wSecond);
            fputs( buf, f );
            fputs( newText.c_str(), f );
            fputs( "\r\n", f );
            fclose( f );
         }
      }
   }
}

void V3_ChatLogDlg::ClearListLogs()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   m_ListText.ClearList();
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_ChatLogDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   if(!g_SaveGame.GetOptionsParam()->bDisplayChatLog)
   {
      Hide();
      return;
   }

   if(m_bDragDialog)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );

      int iNewPosX = m_iXPos + (mousePos.x-m_iXPosSD);
      int iNewPosY = m_iYPos + (mousePos.y-m_iYPosSD);

      
      //on valide si proche de la main barre dans le cendre si oui, on vas attacjer a la main barre
      int iXM1Pos  = g_Global.GetDialogLastMainBarXPos()+108;
      int iXM2Pos  = g_Global.GetDialogLastMainBarXPos()+592;
      int iYMPos   = g_Global.GetDialogLastMainBarYPos()+14;
      int iChatPX  = iNewPosX;
      int iChatPX2 = iNewPosX+m_iWVal;
      int iChatPY  = iNewPosY+m_iHVal;


      //valide en Y first...
      if(iChatPY > iYMPos-g_iSizeYOff && iChatPY < iYMPos+g_iSizeYOff)
      {
         //oki now on valide en X avec le coin gauche et sinon si entre les 2 limites...
         //on commence par si il est entre les 2 limites
         if(iChatPX >=iXM1Pos && iChatPX+m_iWVal <=iXM2Pos )
         {
            //on est entre les range on attack seulement en Y
            iNewPosY = iYMPos-m_iHVal;
         }
         if(iChatPX > iXM1Pos-g_iSizeXOff && iChatPX < iXM1Pos+g_iSizeXOff)
         {
            //on valide coin inferieur gauche et si oui on attach egalement en X
            iNewPosX = iXM1Pos;
            iNewPosY = iYMPos-m_iHVal;
         }
         else if(iChatPX2 > iXM2Pos-g_iSizeXOff && iChatPX2 < iXM2Pos+g_iSizeXOff)
         {
            //on valide coin inferieur gauche et si oui on attach egalement en X
            iNewPosX = iXM2Pos-m_iWVal;
            iNewPosY = iYMPos-m_iHVal;
         }
      }


      
      
      if(iNewPosX < 0)
         iNewPosX = 0;

      if(iNewPosY < 0)
         iNewPosY = 0;
      
      if(iNewPosX+m_iWVal > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-m_iWVal;
      if(iNewPosY+m_iHVal > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-m_iHVal;

      UpdateAllFramePos(iNewPosX, iNewPosY,m_iWVal,m_iHVal);
   }
   else if(m_bSizeDialogX)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      int iMouseOffsetX = (mousePos.x-m_iXPosSR);
      
      int iNewW;
      if(iMouseOffsetX <0)
         iNewW = m_iWVal - abs(iMouseOffsetX);
      else
         iNewW = m_iWVal + iMouseOffsetX;

      if(iNewW <g_iMinSizeX)
         iNewW  = g_iMinSizeX;

      if(m_iXPos + iNewW >= g_Global.GetScreenW())
         iNewW -= ((m_iXPos + iNewW)-g_Global.GetScreenW());
     
     
      UpdateAllFramePos(m_iXPos,m_iYPos,iNewW,m_iHVal);
   }
   else if(m_bSizeDialogY)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      int iMouseOffsetY = (mousePos.y-m_iYPosSR);

      int iNewH;
      if(iMouseOffsetY <0)
         iNewH = m_iHVal + abs(iMouseOffsetY);
      else
         iNewH = m_iHVal - iMouseOffsetY;
      
      int iOYPatch = 0;
      if(iNewH <g_iMinSizeY)
      {
         iOYPatch = g_iMinSizeY-iNewH;
         iNewH  = g_iMinSizeY;
      }

      int iNewPosY = m_iYPos + (iMouseOffsetY-iOYPatch);
      if(iNewPosY < 0)
         iNewPosY = 0;
      

      
      if(iNewPosY+iNewH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-iNewH;
      
      
      UpdateAllFramePos(m_iXPos,iNewPosY,m_iWVal,iNewH);
      
   }
   

   


   FramePos frmPos  = GetPos();
   FramePos frmPosB = m_Background.GetPos();

   
   RECT rect  = { frmPos.x , frmPos.y , frmPos.maxx , frmPos.maxy };
   RECT rectB = { frmPosB.x, frmPosB.y, frmPosB.maxx, frmPosB.maxy};
   V2SPRITEFX fx;
   memset( &fx, 0, sizeof( fx ) );
   fx.lpClipRect = &rectB;
   fx.dwFX |= FX_NOCORRECTION;
   
   if(g_SaveGame.GetOptionsParam()->bChatLogFonce)
      m_BackgroundF.Draw(&fx);
   else
      m_Background.Draw(&fx);
   

   fx.lpClipRect = &rect;
   GameUI::Draw( &fx/*vsfFX*/ );

  
}


void V3_ChatLogDlg::Hide()
{ 
   BoxUI::Hide();
}

void V3_ChatLogDlg::Show()
{
   BoxUI::Show();
}




bool V3_ChatLogDlg::StartDragDlg( MousePos mousePos )
{
   if(!g_SaveGame.GetOptionsParam()->bGUIDynamic)
      return false;

   
   if(!m_bDragDialog && !m_bSizeDialogX && !m_bSizeDialogY && !m_iPinnedStatus)
   {
      //si premier start drag, on prend la position de depart du drag
      //pour en calculer le deltat X et Y du deplacement de la fenetre...
      {
         
         m_bDragDialog = true;
         m_iXPosSD = mousePos.x;
         m_iYPosSD = mousePos.y;

         RootBoxUI::GetInstance()->SetDragUI((GameUI*)&Drag_Button);
         g_Global.SetDraggingDialog(true);

         return true;
      }
   }

   return false;
}

void V3_ChatLogDlg::StopDragDlg()
{
   if(m_bDragDialog)
   {
      m_bDragDialog = false;

      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      m_iXPos = m_iXPos + (mousePos.x-m_iXPosSD);
      m_iYPos = m_iYPos + (mousePos.y-m_iYPosSD);

      //on valide si proche de la main barre dans le cendre si oui, on vas attacjer a la main barre
      int iXM1Pos  = g_Global.GetDialogLastMainBarXPos()+108;
      int iXM2Pos  = g_Global.GetDialogLastMainBarXPos()+592;
      int iYMPos   = g_Global.GetDialogLastMainBarYPos()+14;
      int iChatPX  = m_iXPos;
      int iChatPX2 = m_iXPos+m_iWVal;
      int iChatPY  = m_iYPos+m_iHVal;
      
      
      //valide en Y first...
      if(iChatPY > iYMPos-g_iSizeYOff && iChatPY < iYMPos+g_iSizeYOff)
      {
         //oki now on valide en X avec le coin gauche et sinon si entre les 2 limites...
         //on commence par si il est entre les 2 limites
         if(iChatPX >=iXM1Pos && iChatPX+m_iWVal <=iXM2Pos )
         {
            //on est entre les range on attack seulement en Y
            m_iYPos = iYMPos-m_iHVal;
         }
         if(iChatPX > iXM1Pos-g_iSizeXOff && iChatPX < iXM1Pos+g_iSizeXOff)
         {
            //on valide coin inferieur gauche et si oui on attach egalement en X
            m_iXPos = iXM1Pos;
            m_iYPos = iYMPos-m_iHVal;
         }
         else if(iChatPX2 > iXM2Pos-g_iSizeXOff && iChatPX2 < iXM2Pos+g_iSizeXOff)
         {
            //on valide coin inferieur gauche et si oui on attach egalement en X
            m_iXPos = iXM2Pos-m_iWVal;
            m_iYPos = iYMPos-m_iHVal;
         }
      }

      

      
      
      if(m_iXPos < 0)
         m_iXPos = 0;
      if(m_iYPos < 0)
         m_iYPos = 0;
      
      if(m_iXPos+m_iWVal > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-m_iWVal;
      if(m_iYPos+m_iHVal > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-m_iHVal;
      
      UpdateAllFramePos(m_iXPos,m_iYPos,m_iWVal,m_iHVal);
      g_Global.SetDialogLastChatLogXYPos(m_iXPos,m_iYPos,m_iWVal,m_iHVal);

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

bool V3_ChatLogDlg::StartSizeDlg( MousePos mousePos )
{
   if(!m_bDragDialog && !m_bSizeDialogX && !m_bSizeDialogY && !m_iPinnedStatus)
   {
      //si premier start size, on prend la position de depart du Resize

      //X
      if(mousePos.x > m_iXPos+m_iWVal-g_iSizeXOff  && mousePos.x <= m_iXPos+m_iWVal)
      {
         
         m_bSizeDialogX = true;
         m_iXPosSR = mousePos.x;
         RootBoxUI::GetInstance()->SetDragUI((GameUI*)&UPSize_Button);
         g_Global.SetDraggingDialog(true);
         return true;
      }
      //Y
      else if(mousePos.y > m_iYPos  && mousePos.y <= m_iYPos+g_iSizeYOff)
      {
         
         m_bSizeDialogY = true;
         m_iYPosSR = mousePos.y;
         RootBoxUI::GetInstance()->SetDragUI((GameUI*)&RightSize_Button);
         g_Global.SetDraggingDialog(true);
         return true;
      }

   }
   
   return false;
}

void V3_ChatLogDlg::StopSizeDlg()
{
   bool bStopped = false;
   if(m_bSizeDialogX)
   {
      m_bSizeDialogX = false;
      
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      int iMouseOffsetX = (mousePos.x-m_iXPosSR);
      if(iMouseOffsetX <0)
         m_iWVal = m_iWVal - abs(iMouseOffsetX);
      else
         m_iWVal = m_iWVal + iMouseOffsetX;

      if(m_iWVal <g_iMinSizeX)
         m_iWVal  = g_iMinSizeX;

      if(m_iXPos + m_iWVal >= g_Global.GetScreenW())
         m_iWVal -= ((m_iXPos + m_iWVal)-g_Global.GetScreenW());
      

      m_ListText.ReconstructList();
      bStopped = true;
   }

   if(m_bSizeDialogY)
   {
      m_bSizeDialogY = false;
      
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      int iMouseOffsetY = (mousePos.y-m_iYPosSR);
      if(iMouseOffsetY <0)
         m_iHVal = m_iHVal + abs(iMouseOffsetY);
      else
         m_iHVal = m_iHVal - iMouseOffsetY;
      
      int iOYPatch = 0;
      if(m_iHVal <g_iMinSizeY)
      {
         iOYPatch = g_iMinSizeY-m_iHVal;
         m_iHVal  = g_iMinSizeY;
      }
      
      
      m_iYPos = m_iYPos + (iMouseOffsetY-iOYPatch);
      if(m_iYPos < 0)
         m_iYPos = 0;
      
      
      if(m_iYPos+m_iHVal > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-m_iHVal;
      
      bStopped = true;
   }

   if(bStopped)
   {
      if(m_iXPos < 0)
         m_iXPos = 0;
      if(m_iYPos < 0)
         m_iYPos = 0;
      
      if(m_iXPos+m_iWVal > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-m_iWVal;
      if(m_iYPos+m_iHVal > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-m_iHVal;
      
      UpdateAllFramePos(m_iXPos,m_iYPos,m_iWVal,m_iHVal);
      g_Global.SetDialogLastChatLogXYPos(m_iXPos,m_iYPos,m_iWVal,m_iHVal);
      
      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
   }
   
}



///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_ChatLogDlg::DragEvent::LeftClicked( void )
{
   V3_ChatLogDlg *me = static_cast< V3_ChatLogDlg * >( GetUI() );
   
   me->StopDragDlg();
   
}

void V3_ChatLogDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_ChatLogDlg *me = static_cast< V3_ChatLogDlg * >( GetUI() );
   
   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   
   me->StartDragDlg(mp);
}


void V3_ChatLogDlg::SizeEvent::LeftClicked( void )
{
   V3_ChatLogDlg *me = static_cast< V3_ChatLogDlg * >( GetUI() );
   
   me->StopSizeDlg();
}

void V3_ChatLogDlg::SizeEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_ChatLogDlg *me = static_cast< V3_ChatLogDlg * >( GetUI() );
   
   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   
   me->StartSizeDlg(mp);
}





void V3_ChatLogDlg::ListEndEvent::LeftClicked( void )
{
   V3_ChatLogDlg *me = static_cast< V3_ChatLogDlg * >( GetUI() );
   
   me->m_ListText.SetScrollPos( 0xFFFFFFFF);
}

void V3_ChatLogDlg::PinEvent::LeftClicked( void )
{
   V3_ChatLogDlg *me = static_cast< V3_ChatLogDlg * >( GetUI() );
   
   me->m_iPinnedStatus = 1-me->m_iPinnedStatus;
   if(me->m_iPinnedStatus == 0)
   {
      me->m_ListPinD.Show();
      me->m_ListPinE.Hide();
   }
   else
   {
      me->m_ListPinE.Show();
      me->m_ListPinD.Hide();
   }
   g_Global.SetDialogLastChatLogP(me->m_iPinnedStatus);

   me->UpdateAllFramePos(me->m_iXPos,me->m_iYPos,me->m_iWVal,me->m_iHVal);
}


