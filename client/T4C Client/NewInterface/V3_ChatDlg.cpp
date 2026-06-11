// V3_ChatDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_ChatDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
 
#include "V3_StatDlg.h"
#include "V3_MainBarDlg.h"
#include "V3_AskValueDlg.h"
#include "V3_ChatLogDlg.h"

extern CSaveGame g_SaveGame;

void ParseText( char *pText);

HANDLE g_CCRefreshThreadHandle = NULL;
DWORD g_dwLastMacroCallChat = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_ChatDlg::V3_ChatDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_ListCC( channelListEvent ),
m_ListUser( userListEvent ),
m_btnAddCC(btnAddCCEvent,g_GUILocalString[322],g_DefColorH),
m_btnDelCC(btnDelCCEvent,g_GUILocalString[323],g_DefColorH),
m_btnTogglePage(btnTogglePageEvent,g_GUILocalString[320],g_DefColorH),
m_btnRequete(btnRequeteEvent,g_GUILocalString[353],g_DefColorH)
{
   int i=0;

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogChatLastXPos();
   m_iYPos = g_Global.GetDialogChatLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_ChatBack",true,"V3_ChatBackMask");

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

   m_stHCC     .SetText(g_GUILocalString[2],g_DefColorH);
   m_stHUName  .SetText(g_GUILocalString[3],g_DefColorH);
   m_stHUTitle .SetText(g_GUILocalString[4],g_DefColorH);
   m_stHUGuild .SetText(g_GUILocalString[248],g_DefColorH);
  
   

   SetHelpId( m_ListCC, 316 );
   m_ListCC.SetupList    ( 2, 20, FramePos( 0, 0, 0, 2 ) );
   m_ListCC.SetColumnSize( 0, 132  );
   m_ListCC.SetColumnSize( 1, 18  );
   m_ListCC.CenterColumn( 0 );
   m_ListCC.CenterColumn( 1 );
   m_ListCC.SetScrollUpDownUI( &m_ListCCScrollUp );
   m_ListCC.SetScrollDownDownUI( &m_ListCCScrollDown );
   
   m_ListCCSelectionGraphPatch.SetPos( FramePos( 0, 0, 152, 20 ) );
   m_ListCCSelectionGraphPatch.SetSpriteId( "V3_ChatCCListPatch" );
   m_ListCCSelectionGraphPatch.Show();
   m_ListCC.SetSelectionSprite( &m_ListCCSelectionGraphPatch);
   channelListEvent.SetUI( this );


   SetHelpId( m_ListUser, 317 );
   m_ListUser.SetupList    ( 4, 20, FramePos( 0, 0, 2, 1 ) );
   m_ListUser.SetColumnSize( 0, 140  );
   m_ListUser.SetColumnSize( 1, 188  );
   m_ListUser.SetColumnSize( 2, 140  );
   m_ListUser.SetColumnSize( 3, 18  );
   m_ListUser.CenterColumn( 0 );
   m_ListUser.CenterColumn( 1 );
   m_ListUser.CenterColumn( 2 );
   m_ListUser.CenterColumn( 3 );
   m_ListUser.SetScrollUpDownUI( &m_ListUserScrollUp );
   m_ListUser.SetScrollDownDownUI( &m_ListUserScrollDown );
   
   m_ListUserSelectionGraphPatch.SetPos( FramePos( 0, 0, 492, 20 ) );
   m_ListUserSelectionGraphPatch.SetSpriteId( "V3_ChatUserListPatch" );
   m_ListUserSelectionGraphPatch.Show();
   m_ListUser.SetSelectionSprite( &m_ListUserSelectionGraphPatch);
   userListEvent.SetUI( this );

   
   selectedChannel = "Main"; 


   m_btnAddCC.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnAddCC.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnAddCC.SetHighlightUI( "V3_Btn72x24_H" );
   btnAddCCEvent.SetUI( this );

   m_btnDelCC.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnDelCC.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnDelCC.SetHighlightUI( "V3_Btn72x24_H" );
   btnDelCCEvent.SetUI( this );

   m_btnTogglePage.SetSpriteId   ( "V3_Btn130x24_N" );
   m_btnTogglePage.SetDownUI     ( "V3_Btn130x24_D" );
   m_btnTogglePage.SetHighlightUI( "V3_Btn130x24_H" );
   btnTogglePageEvent.SetUI( this );

   m_btnRequete.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnRequete.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnRequete.SetHighlightUI( "V3_Btn92x24_H" );
   btnRequeteEvent.SetUI( this );


   


   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_stHCC     );
   AddChild( &m_stHUName  );
   AddChild( &m_stHUTitle );
   AddChild( &m_stHUGuild );
   AddChild( &m_stHInfo   );
   AddChild( &m_ListCC    );
   AddChild( &m_ListUser  );
   AddChild( &m_btnAddCC);
   AddChild( &m_btnDelCC);
   AddChild( &m_btnTogglePage);
   AddChild( &m_btnRequete);

   AddChild( &Drag_Button);

 
   RootBoxUI::GetInstance()->AddChild( this );

   


   if( g_CCRefreshThreadHandle == NULL )
   {
      // Start the channel refreshing thread.
      unsigned int threadId = 0;
      g_CCRefreshThreadHandle = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, ChannelRefreshThread, NULL, 0, &threadId ));
   }
} 

V3_ChatDlg::~V3_ChatDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_ChatDlg *V3_ChatDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_ChatDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_ChatDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallChat > 500)
      {
         g_dwLastMacroCallChat =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}

unsigned int V3_ChatDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   // Send request for list of channels on server.
   //TFCPacket sending;
   //sending << (short)RQ_GetChatterChannelList;
   //SEND_PACKET( sending );
   
   // If a channel is selected
   if( !GetInstance()->selectedChannel.empty() )
   {
      // Get its user list immediatly.
      TFCPacket sending;           
      sending << (short)RQ_GetChatterUserList2;
      sending << (short)GetInstance()->selectedChannel.size();
      sending << (char *)GetInstance()->selectedChannel.c_str();
      SEND_PACKET( sending );
   }

   GetInstance()->m_iXPos = g_Global.GetDialogChatLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogChatLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);
      
   GetInstance()->Show();

   return 0;
}

void V3_ChatDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_ChatDlgW, iPosY+g_ChatDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_ChatDlgW, iPosY+g_ChatDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+727, iPosY+27, iPosX+727+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+690, iPosY+57 ) );

  
   m_stHCC     .SetPos( FramePos( iPosX+38 , iPosY+94 , iPosX+186, iPosY+111 ) );
   m_stHUName  .SetPos( FramePos( iPosX+233, iPosY+94 , iPosX+370, iPosY+111 ) );
   m_stHUTitle .SetPos( FramePos( iPosX+375, iPosY+94 , iPosX+560, iPosY+111 ) );
   m_stHUGuild .SetPos( FramePos( iPosX+565, iPosY+94 , iPosX+702, iPosY+111 ) );
   m_stHInfo   .SetPos( FramePos( iPosX+233, iPosY+375, iPosX+702, iPosY+392 ) );

   m_ListCC  .SetListDef(iPosX,iPosY,20,37 ,118,210,313,&m_ListCCScrollUp,&m_ListCCScrollDown);
   m_ListUser.SetListDef(iPosX,iPosY,20,232,118,747,368,&m_ListCCScrollUp,&m_ListCCScrollDown);

   m_btnAddCC     .SetPos( FramePos( iPosX+40 , iPosY+329, iPosX+40+72 , iPosY+329+24 ) );
   m_btnDelCC     .SetPos( FramePos( iPosX+136, iPosY+329, iPosX+136+72, iPosY+329+24 ) );
   m_btnTogglePage.SetPos( FramePos( iPosX+59 , iPosY+374, iPosX+59+130, iPosY+374+24 ) );
   m_btnRequete   .SetPos( FramePos( iPosX+344, iPosY+403, iPosX+344+92, iPosY+403+24 ) );

}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_ChatDlg::Draw(V2SPRITEFX *vsfFX)
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
      
      if(iNewPosX+g_ChatDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_ChatDlgW;
      if(iNewPosY+g_ChatDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_ChatDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   //backGame.Draw(vsfFX);
   GameUI::Draw( vsfFX );

   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   m_btnAddCC.DrawTextButtonExt(hdc);
   m_btnDelCC.DrawTextButtonExt(hdc);
   m_btnTogglePage.DrawTextButtonExt(hdc);
   m_btnRequete.DrawTextButtonExt(hdc);
   
   // Draw les texte...
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[1],strlen(g_GUILocalString[1]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}

void V3_ChatDlg::Hide()
{
   BoxUI::Hide();
}

void V3_ChatDlg::Show()
{
   UpdateAllFramePos(g_Global.GetDialogChatLastXPos(),g_Global.GetDialogChatLastYPos());
   BoxUI::Show();
}




void V3_ChatDlg::StartDragDlg( MousePos mousePos )
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

void V3_ChatDlg::StopDragDlg()
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
      
      if(m_iXPos+g_ChatDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_ChatDlgW;
      if(m_iYPos+g_ChatDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_ChatDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogChatLastXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_ChatDlg::DragEvent::LeftClicked( void )
{
   V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_ChatDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}



void V3_ChatDlg::EnterChatterChannel(string channelID, string pwd,DWORD  color,bool   bEnable,bool   bUpdateList,bool   bClearCCList)
{
    // If text was typed in the channel box.
    if( !channelID.empty() )
    {
        // Send a joins packet
        TFCPacket sending;

        BYTE chUpdateList = 0;
        if(bUpdateList)
           chUpdateList = 1;

        BYTE chClearCCList = 0;
        if(bClearCCList)
           chClearCCList = 1;

        sending << (RQ_SIZE)RQ_AddRemoveChatterChannel;
        sending << (char)chClearCCList;
        sending << (char)chUpdateList;
        sending << (short)channelID.size();
        sending << (char *)channelID.c_str();

        // If a password was provided.
        if( !pwd.empty() )
        {
            sending << (short)pwd.size();
            sending << (char *)pwd.c_str();
        }
        else
        {
            sending << (short)0;
        }
        
        // Add to the list of channels to save.
        RegChannel regChannel;
        regChannel.channelID = channelID;
        regChannel.pwd       = pwd;
        regChannel.color     = color;
        regChannel.bActive   = bEnable;
        regChannels.insert( regChannel );

        SEND_PACKET( sending );

        // Preemtively add the channel
        m_ListCC.AddNewItem();

        ChannelUI *channelUI = new ChannelUI;
        channelUI->channelColor = static_cast< ChannelUI::color >( color );
        channelUI->SetText( channelID.c_str(), channelUI->channelColor, 640, 0, 0 );
        channelUI->SetCenter( false );
        channelUI->SetPos( FramePos( 0, 0, channelUI->GetWidth(), channelUI->GetHeight() ) );
        channelUI->channelID = channelID;
        channelUI->listenState = bEnable;
        m_ListCC.AddColumnItem( 0, channelUI );

        if( channelID == selectedChannel )
        {
            m_ListCC.SetSelection( channelUI );
        }

        ChannelToggleUI *channelToggleUI = new ChannelToggleUI;
        if(bEnable)
        {
            channelToggleUI->SetSpriteId   ( "V3_Check18_ON_N");
            channelToggleUI->SetDownUI     ( "V3_Check18_ON_N");
            channelToggleUI->SetHighlightUI( "V3_Check18_ON_H");
        }
        else
        {
            channelToggleUI->SetSpriteId   ( "V3_Check18_OFF_N" );
            channelToggleUI->SetDownUI     ( "V3_Check18_OFF_N" );
            channelToggleUI->SetHighlightUI( "V3_Check18_OFF_H" );

            TFCPacket sending;
            sending << (short)RQ_ToggleChatterListening;
            sending << (short)channelUI->channelID.size();
            sending << (char *)channelUI->channelID.c_str();
            sending << (char)( 0);
            SEND_PACKET( sending );
        }

        //channelToggleUI->SetCenter( false );
        channelToggleUI->SetPos( FramePos( 0, 0, 18, 18 ) );
        channelToggleUI->channelID = channelID;
        channelToggleUI->listenState = bEnable;
        m_ListCC.AddColumnItem( 1, channelToggleUI );

        //savedColor [ channelUI->channelID ] = channelUI->channelColor;
        //savedStatus[ channelUI->channelID ] = channelUI->listenState;
    }
}


//  Called when the user selects a channel in the list.
void V3_ChatDlg::ChannelListEvent::LeftClicked( void )
{
    V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );

    int col = me->m_ListCC.GetSelectedColumn();

    string channelId;
    
    // Click in channel names listing
    if( col == 0 )
    {
       ChannelUI *channelUI = static_cast< ChannelUI * >( me->m_ListCC.GetSelection() );
       
       if( channelUI == NULL )
          return;
       
       V3_MainBarDlg::GetInstance()->ComboReturn(0,(char*)channelUI->channelID.c_str());

       if(me->selectedChannel == channelUI->channelID)
          return;

       me->selectedChannel = channelUI->channelID;
       
       TFCPacket sending;           
       sending << (short)RQ_GetChatterUserList2;
       sending << (short)channelUI->channelID.size();
       sending << (char *)channelUI->channelID.c_str();
       
       SEND_PACKET( sending );
    }
    // Clicked on the channel toggle button
    else
    {
        ChannelUI *channelUI = static_cast< ChannelUI * >( me->m_ListCC.GetSelection() );

        // If nothing was selected.
        if( channelUI == NULL )
            return;

        // If we double clicked on the toggle listening icon.
        if( me->m_ListCC.GetSelectedColumn() == 1 )
        {
           TFCPacket sending;
           sending << (short)RQ_ToggleChatterListening;
           sending << (short)channelUI->channelID.size();
           sending << (char *)channelUI->channelID.c_str();
           sending << (char)( channelUI->listenState == true ? 0 : 1 );
           SEND_PACKET( sending );

           channelUI->listenState = !channelUI->listenState;

           //me->savedStatus[ channelUI->channelID ] = channelUI->listenState;
           
           // Scroll through the list of reg Channels to save.
           set< RegChannel >::iterator j;
           for( j = me->regChannels.begin(); j != me->regChannels.end(); j++ )
           {
              if( _stricmp( (*j).channelID.c_str(), channelUI->channelID.c_str() ) == 0 )
              {
                 (*j).bActive = channelUI->listenState;
              }
           }
        }
    }   
}

//  Called when the user selects a channel in the list.
void V3_ChatDlg::ChannelListEvent::LeftDblClicked( void )
{
    V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );

    int col = me->m_ListCC.GetSelectedColumn();

    string channelId;

    
    
    // Cycle the channel's colors
    if( col == 0 )
    {

        ChannelUI *channelUI = static_cast< ChannelUI * >( me->m_ListCC.GetSelection() );

        if( channelUI == NULL )
            return;


        if(Custom.g_pNewCCColor)
        {
           int dwIndex = -1;
           for(int i=0;i<Custom.g_dwNbrCCColor;i++)
           {
              if(Custom.g_pNewCCColor[i] == channelUI->channelColor)
              {
                 dwIndex = i;
                 i = Custom.g_dwNbrCCColor;
              }
           }
           if(dwIndex >=0)
           {
               dwIndex++;
               if(dwIndex >= Custom.g_dwNbrCCColor)
                  dwIndex = 0;
               channelUI->channelColor = (ChannelUI::color)Custom.g_pNewCCColor[dwIndex];
           }
           else
              channelUI->channelColor = ChannelUI::cyan;
        }
        else
           channelUI->channelColor = ChannelUI::cyan;

        
        channelUI->SetText( 
            channelUI->channelID.c_str(), 
            channelUI->channelColor, 
            640, 
            0, 
            0 
        );

        //me->savedColor[ channelUI->channelID ] = channelUI->channelColor;

        // Scroll through the list of reg Channels to save.
        set< RegChannel >::iterator j;
        for( j = me->regChannels.begin(); j != me->regChannels.end(); j++ )
        {
            if( _stricmp( (*j).channelID.c_str(), channelUI->channelID.c_str() ) == 0 )
            {
                (*j).color = channelUI->channelColor;
            }
        }
    }
}

//  Saves all information into the registry.

//NM:ValidSave
void V3_ChatDlg::CCToSave()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   IgnoreSet::iterator i;
   for( i = ignoreList.begin(); i != ignoreList.end(); i++ )
   {
      sIgnoreList sNewIgnore;
      sprintf_s(sNewIgnore.strNameIgnore,256,"%s",(*i).c_str());
      g_SaveGame.AddIgnore(sNewIgnore);
   }
   
   //Save CCUser
   set< RegChannel >::iterator c;
   for( c = regChannels.begin(); c != regChannels.end(); c++ )
   {
      sChannel sNewCC;
      sprintf_s(sNewCC.strNameCC,256,"%s",(*c).channelID.c_str());
      sprintf_s(sNewCC.strPasswordCC,256,"%s",(*c).pwd.c_str());
      sNewCC.dwColor = (*c).color;
      if((*c).bActive)
         sNewCC.chEnable = 1;
      else
         sNewCC.chEnable = 0;
      
      g_SaveGame.AddChannelS(sNewCC);
   }
}

void V3_ChatDlg::ClearChannelList()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   regChannels.clear();
}

//  Updates the public channel list.
void V3_ChatDlg::UpdateChannels(const std::vector< Channel > &publicChannels)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   int dwScrollPos = 0;
   if(m_ListCC.GetGrid()->size())
      dwScrollPos = m_ListCC.GetScrollPos();
   m_ListCC.DeleteList();
   //savedColor.clear();
   //savedStatus.clear();
   
   set< RegChannel >::iterator j;
   vector< Channel >::const_iterator i;
   for( i = publicChannels.begin(); i != publicChannels.end(); i++ )
   {
      DWORD color   = ChannelUI::cyan;
      bool  bStatus = true;
      
      // Add to the list of channels to save.
      bool found = false;
      for( j = regChannels.begin(); j != regChannels.end(); j++ )
      {
         if( _stricmp( (*j).channelID.c_str(), (*i).channelID.c_str() ) == 0 )
         {                
            found = true;
            break;
         }
      }
      
      if( !found )
      {
         RegChannel regChannel;
         regChannel.channelID = (*i).channelID;
         regChannel.color     = color;
         regChannel.bActive   = (*i).listenState;
         
         regChannels.insert( regChannel );
      }
      else
      {
         
         color = (*j).color;
         bStatus = (*j).bActive;
      }
      
      //if( savedColor.find( (*i).channelID ) != savedColor.end() )
      //   color = savedColor[ (*i).channelID ];    
      //if( savedStatus.find( (*i).channelID ) != savedStatus.end() )
      //   bStatus = savedStatus[ (*i).channelID ];    
      
      m_ListCC.AddNewItem();

      std::string strTmp = (*i).channelID.c_str();
      
      ChannelUI *channelUI = new ChannelUI;
      channelUI->channelColor = static_cast< ChannelUI::color >( color );
      channelUI->SetText( (*i).channelID.c_str(), color, 640, 0, 0 );
      channelUI->SetCenter( false );
      channelUI->SetPos( FramePos( 0, 0, channelUI->GetWidth(), channelUI->GetHeight() ) );
      channelUI->channelID = (*i).channelID;
      channelUI->listenState = bStatus;        
      m_ListCC.AddColumnItem( 0, channelUI );
      
      if( (*i).channelID == selectedChannel )
      {
         m_ListCC.SetSelection( channelUI );
      }
      
      ChannelToggleUI *channelToggleUI = new ChannelToggleUI;
      if( bStatus )
      {
         channelToggleUI->SetSpriteId   ( "V3_Check18_ON_N" );
         channelToggleUI->SetDownUI     ( "V3_Check18_ON_N" );
         channelToggleUI->SetHighlightUI( "V3_Check18_ON_H" );
      }else
      {
         channelToggleUI->SetSpriteId   ( "V3_Check18_OFF_N" );
         channelToggleUI->SetDownUI     ( "V3_Check18_OFF_N" );
         channelToggleUI->SetHighlightUI( "V3_Check18_OFF_H" );
      }
      //channelToggleUI->SetCenter( false ); 
      channelToggleUI->SetPos( FramePos( 0, 0, 18, 18 ) );
      channelToggleUI->channelID = (*i).channelID;
      channelToggleUI->listenState = bStatus;
      m_ListCC.AddColumnItem( 1, channelToggleUI );
   }
   
   // Scroll through the list of reg Channels to save.    
   for( j = regChannels.begin(); j != regChannels.end(); j++ )
   {
      bool found = false;
      // Make sure the channel to save is in the provided list of channels.
      for( i = publicChannels.begin(); i != publicChannels.end() && !found; i++ )    
      {
         // If the channel was found.
         if( _stricmp( (*i).channelID.c_str(), (*j).channelID.c_str() ) == 0 )
         {
            found = true;
         }            
      }
      
      // If the channel was not found.
      if( !found )
      {
         regChannels.erase( j );
         j =regChannels.begin();
      }
   }
   m_ListCC.SetScrollPos( dwScrollPos );
}


//  Gets the color of the given channel
DWORD V3_ChatDlg::GetChannelColor(std::string channelId)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );   
   
   vector< vector< ListUI::GridItem > > &grid = *m_ListCC.GetGrid();
   
   UINT i;
   for( i = 0; i < grid[ 0 ].size(); i++ )
   {
      ChannelUI *channel = static_cast< ChannelUI * >( grid[ 0 ][ i ] );
      if( channel != NULL && _stricmp( channel->channelID.c_str(), channelId.c_str() ) == 0 )
      {
         return channel->channelColor;
      }
   }
   return CL_WHITE;
}

void V3_ChatDlg::ClientInitialize( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   ignoreList.clear();
   m_ListCC.DeleteList();
}



void V3_ChatDlg::SetupServerAllParam()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   ignoreList.clear();
   m_ListCC.DeleteList();
   //savedColor.clear();
   //savedStatus.clear();
   
   
   //Load les ignore List
   int dwNbrIgnore = g_SaveGame.GetNbrIgnore();
   if(dwNbrIgnore > 0)
   {
      for(int i =0;i<dwNbrIgnore; i++ )
      {
         sIgnoreList *pNewIgnore = g_SaveGame.GetIgnore(i);
         ignoreList.insert( pNewIgnore->strNameIgnore );
      }
   }
   
   //Load la Liste des CC
   int dwNbrCC = g_SaveGame.GetNbrChannel();
   if(dwNbrCC > 0)
   {
      for(int i =0;i<dwNbrCC; i++ )
      {
         sChannel *pNewCC = g_SaveGame.GetChannel(i);
         bool bEnable = true;
         if(pNewCC->chEnable==0)
         {
            bEnable = false;
            // on dois envoyer au serveur que le CC ets desactiver...
         }
         bool bUpdateList = false;
         if(i == dwNbrCC-1)
            bUpdateList = true;
         
         bool bClearPlayerCC = false;
         if(i == 0)
            bClearPlayerCC = true;
         
         EnterChatterChannel( pNewCC->strNameCC, pNewCC->strPasswordCC, pNewCC->dwColor,bEnable,bUpdateList,bClearPlayerCC);
      }
   }

   OptionParam *pOption = g_SaveGame.GetOptionsParam();
   // If pages are now allowed.
   if(pOption->bPageEnable )
   {
      m_btnTogglePage.SetBtnInfo(g_GUILocalString[321],g_DefColorH);
      // Tell server to turn pages on.
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_TogglePage;
      sending << (char)1;
      SEND_PACKET( sending );
   }
   else
   {
      m_btnTogglePage.SetBtnInfo(g_GUILocalString[320],g_DefColorH);
      // Tell server to turn pages off.
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_TogglePage;
      sending << (char)0;
      
      SEND_PACKET( sending );
    }
}




// Updates the list of users in the currently selected channel
void V3_ChatDlg::UpdateChannelUsers(const std::vector< User > &channelUsers)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   DWORD oldPos = m_ListUser.GetScrollPos();
   m_ListUser.DeleteList();
   
   vector< User >::const_iterator i;
   for( i = channelUsers.begin(); i != channelUsers.end(); i++ )
   {
      m_ListUser.AddNewItem();
      bool bIgnored = IsUserIgnored((*i).userName);

      DWORD dwColor = g_DefColorD;

      if(bIgnored)
      {
         if( (*i).UserState )
            dwColor = CL_RED;
         else
            dwColor = CL_RED_DARK;
      }
      else
      {
         if( !(*i).UserState )
            dwColor = g_DefColorDL;
      }

      if((*i).chGodLevel ==3)
         dwColor = CL_RED;
      else if((*i).chGodLevel ==2)
         dwColor = CL_GREEN;
      else if((*i).chGodLevel ==1)
         dwColor = CL_BLUE_LIGHT2;



      //Colonne 1 NOM
      UserUI *userUI = new UserUI;

	  char nameTmp[100];
	  if((*i).chHidden ==1)
		sprintf_s(nameTmp,100,"*%s",(*i).userName.c_str());
	  else
		sprintf_s(nameTmp,100,"%s",(*i).userName.c_str());

      userUI->SetText( nameTmp, dwColor, 640, 0, 0 );
      userUI->SetCenter( false );
      userUI->SetPos( FramePos( 0, 0, userUI->GetWidth(), userUI->GetHeight() ) );
      userUI->userID = (*i).userName;
      m_ListUser.AddColumnItem( 0, userUI );
      if( (*i).userName == selectedUser )
      {
         m_ListUser.SetSelection( userUI );
      }

      //Colonne 2 Titre
      userUI = new UserUI;  
      userUI->SetText( (*i).title.c_str(), dwColor, 640, 0, 0 );
      userUI->SetCenter( false );
      userUI->SetPos( FramePos( 0, 0, userUI->GetWidth(), userUI->GetHeight() ) );
      userUI->userID = (*i).title;
      m_ListUser.AddColumnItem( 1, userUI );
      
      //Colonne 3 Guild
      userUI = new UserUI;
      userUI->SetText( (*i).Guild.c_str(), dwColor, 640, 0, 0 );
      userUI->SetCenter( false );
      userUI->SetPos( FramePos( 0, 0, userUI->GetWidth(), userUI->GetHeight() ) );
      userUI->userID = (*i).Guild;
      m_ListUser.AddColumnItem( 2, userUI );

      //Colonne 4 Check si Ignorer ou pas...
      ChannelToggleUI *channelToggleUI = new ChannelToggleUI;
      if( bIgnored )
      {
         channelToggleUI->SetSpriteId   ( "V3_CheckX18_ON_N" );
         channelToggleUI->SetDownUI     ( "V3_CheckX18_ON_N" );
         channelToggleUI->SetHighlightUI( "V3_CheckX18_ON_H" );
      }
      else
      {
         channelToggleUI->SetSpriteId   ( "V3_CheckX18_OFF_N" );
         channelToggleUI->SetDownUI     ( "V3_CheckX18_OFF_N" );
         channelToggleUI->SetHighlightUI( "V3_CheckX18_OFF_H" );
      }
      //channelToggleUI->SetCenter( false ); 
      channelToggleUI->SetPos( FramePos( 0, 0, 18, 18 ) );
      channelToggleUI->channelID = (*i).userName;
      channelToggleUI->listenState = bIgnored;
      m_ListUser.AddColumnItem( 3, channelToggleUI );
   }
   
   if( channelUsers.size() == 1 )
   {
      m_stHInfo.SetText( g_LocalString[ 528 ],g_DefColorD );
   }
   else
   {
      char buf[ 1024 ];
      sprintf_s( buf,1024, g_LocalString[ 527 ], channelUsers.size() );
      m_stHInfo.SetText( buf ,g_DefColorD);
   }
   
   m_ListUser.SetScrollPos( oldPos );
   
}


//  Called when the user selects a player in the list.
void V3_ChatDlg::UserListEvent::LeftClicked( void )
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );
   
   int col = me->m_ListUser.GetSelectedColumn();
   if(col == 3)
   { 
      ChannelToggleUI *userUI = static_cast< ChannelToggleUI * >( me->m_ListUser.GetSelectionRC() );    
      
      if( userUI != NULL )
      {
         if(userUI->listenState)
         {
            me->GetIgnoreList()->erase( userUI->channelID );
         }
         else
         {
            if(_stricmp( userUI->channelID.c_str(), Player.Name ) != 0 )
            {
               me->GetIgnoreList()->insert( userUI->channelID );
            }
         }

         if( !GetInstance()->selectedChannel.empty() )
         {
            TFCPacket sending;           
            sending << (short)RQ_GetChatterUserList2;
            sending << (short)GetInstance()->selectedChannel.size();
            sending << (char *)GetInstance()->selectedChannel.c_str();
            
            SEND_PACKET( sending );
         }
      } 
      
   }
   else
   {
      UserUI *userUI = static_cast< UserUI * >( me->m_ListUser.GetSelection() );    
   
      if( userUI != NULL )
      {
         // If the chat ui is in page mode.
         if( V3_MainBarDlg::GetInstance()->GetTextInputState() == SendToPage )
         {
            // Set the page name to the clicked user name.
            V3_MainBarDlg::GetInstance()->SetInputtextUI( userUI->userID );            
         }
      
         me->selectedUser = userUI->userID;
      }
      else
      {
         me->selectedUser = "";
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_ChatDlg::UserListEvent::LeftDblClicked( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Set page mode with the selected user.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
   V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );
   
   int col = me->m_ListUser.GetSelectedColumn();
   if(col == 3)
   {
      UserUI *userUI = static_cast< UserUI * >( me->m_ListUser.GetSelection() );    
      
      if( userUI != NULL )
      {
      
      } 
   }
   else
   {
      UserUI *userUI = static_cast< UserUI * >( me->m_ListUser.GetSelection() );    
      
      if( userUI != NULL )
      {
         
         V3_MainBarDlg::GetInstance()->SetInputtextUI( userUI->userID ); 
         V3_MainBarDlg::GetInstance()->ForceSetSendCC();
         me->selectedUser = userUI->userID;
      }
      else
      {
         me->selectedUser = "";
      }
   }
}



void V3_ChatDlg::BtnAddCCEvent::LeftClicked( void )
{
   V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );
   
   V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,g_GUILocalString[8] ,"",20,461);
   V3_AskValueDlg::GetInstance()->SetDialogText2(g_GUILocalString[9] ,"",16,462);
   V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueDlg::GetInstance()->SetParent(me);
   V3_AskValueDlg::GetInstance()->SetXYPos(((g_ChatDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_ChatDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueDlg::GetInstance()->Show(0);
   
   me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );
   
}

void V3_ChatDlg::BtnDelCCEvent::LeftClicked( void )
{
   V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );
  
   string selectedChannel = me->GetSelectedChannel();
   
   // If there is a selected channel.
   if( !selectedChannel.empty() )
   {
      
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_RemoveFromChatterChannel;
      sending << (short)selectedChannel.size();
      sending << (char *)selectedChannel.c_str();
     
      g_SaveGame.RemoveChannel((char *)selectedChannel.c_str());
        
      SEND_PACKET( sending );
    }
}

void V3_ChatDlg::BtnTogglePageEvent::LeftClicked( void )
{
   V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );
   
   // Switch paging mode.
   OptionParam *pOption = g_SaveGame.GetOptionsParam();
   pOption->bPageEnable = !pOption->bPageEnable;
   
   // If pages are now allowed.
   if(pOption->bPageEnable )
   {
      me->m_btnTogglePage.SetBtnInfo(g_GUILocalString[321],g_DefColorH);
      
      // Tell server to turn pages on.
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_TogglePage;
      sending << (char)1;
      SEND_PACKET( sending );
   }
   else
   {
      me->m_btnTogglePage.SetBtnInfo(g_GUILocalString[320],g_DefColorH);
      
      // Tell server to turn pages off.
      TFCPacket sending;
      sending << (RQ_SIZE)RQ_TogglePage;
      sending << (char)0;
      
      SEND_PACKET( sending );
    }
}


void V3_ChatDlg::AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey)
{
   if(!bOK)
      return;

   switch(uiShowCodeP)
   {
      case 0:
      {
         CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
         EnterChatterChannel( pStrValue1, pStrValue2, ChannelUI::cyan ,true,true,false);
      }
      break;
      case 1:
      {
         if(strlen(pStrValue1) >= 10)
         {
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_GMMSG_Post;
            sending << (short)strlen( pStrValue1);
            sending << (char *)pStrValue1;
            SEND_PACKET( sending );
         }
         else
         {
            V3_ChatLogDlg::GetInstance()->AddListLogs("", g_GUILocalString[354], CL_RED, true);
         }
      }
      break;
   }
}


//////////////////////////////////////////////////////////////////////////////////////////
UINT WINAPI V3_ChatDlg::ChannelRefreshThread(void *nothing)
{
   int iCount = 0;
   while( g_boQuitApp )
   {
      iCount++;
      Sleep( 1000 );

      if(iCount == 5)
      {
         iCount = 0;
         if(!g_boQuitApp)
         {
            if( GetInstance()->IsShown() )
            {
               // If a channel is currently selected.
               if( !GetInstance()->selectedChannel.empty() )
               {
                  TFCPacket sending;           
                  sending << (short)RQ_GetChatterUserList2;
                  sending << (short)GetInstance()->selectedChannel.size();
                  sending << (char *)GetInstance()->selectedChannel.c_str();
               
                  SEND_PACKET( sending );
               }
            }
         }
      }
   }
   
   return 0;
}


//  Determines if a user has been ignored.
bool V3_ChatDlg::IsUserIgnored(string userName)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   return( ignoreList.find( userName ) != ignoreList.end() );
}


void V3_ChatDlg::BtnRequeteEvent::LeftClicked( void )
{
   V3_ChatDlg *me = static_cast< V3_ChatDlg * >( GetUI() );
  
   V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,g_GUILocalString[353] ,"",180,0);
   V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[355] ,g_GUILocalString[11],738,739);
   V3_AskValueDlg::GetInstance()->SetParent(me);
   V3_AskValueDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueDlg::GetInstance()->Show(1);
   
   me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );
}