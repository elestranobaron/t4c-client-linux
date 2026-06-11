// V3_EffectStatusDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_EffectStatusDlg.h"
#include "RootBoxUI.h"
#include "V3_StatDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_SpellDlg.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"


extern CSaveGame g_SaveGame;

void FormatChargesLeft( char *buf, LPBAG_ITEM bagItem );

//const int iOffsetY        = 80;
const int iTrackSizeMY      = 24;
const int iTrackSizeSY      = 10;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_EffectStatusDlg::V3_EffectStatusDlg():
 m_bScrollEnabled( false ),
 m_BtnScrollUp( scrollUpEvent ),
 m_BtnScrollDn( scrollDownEvent ),
 Drag_Button(dragEvent),
 UPSize_Button(sizeEvent),
 m_iMaxListSize(0),
 m_iListPos(0),
 m_ListPinE(pinEvent),
 m_ListPinD(pinEvent),
 displayedEffects( 30 )  //11
{
   UINT i=0;

   m_bDragDialog  = false;
   m_bSizeDialogY = false;
   m_iXPos = g_Global.GetDialogLastStatusXPos();
   m_iYPos = g_Global.GetDialogLastStatusYPos();
   m_iNbrVisibleEffect = g_Global.GetDialogLastStatusNbrEffect();
   

   
  
   // 27x 17y
   m_BtnScrollUp.SetSpriteId( "V3_StatusBarUp" );
   m_BtnScrollUp.SetHelpText( FontManager::GetInstance()->GetSmallFont() );
   scrollUpEvent.SetUI( this );
   
   
   
   m_BtnScrollDn.SetSpriteId( "V3_StatusBarDown" );
   m_BtnScrollDn.SetHelpText( FontManager::GetInstance()->GetSmallFont() );
   scrollDownEvent.SetUI( this );

  
   // Add all displayed effects
   for( i = 0; i < displayedEffects.size(); i++ )
   {
      displayedEffects[ i ].SetSpriteId( "V3_EffectStatusBar" );
      displayedEffects[ i ].SetHelpText( FontManager::GetInstance()->GetSmallFont("" ) );
      displayedEffects[ i ].emptyGauge.SetSpriteId( "V3_EffectStatusEmptyBar" );
      AddChild( &displayedEffects[ i ] );
   }

   Drag_Button.SetSpriteId   ( "V3_BtnMoveOn_N" );
   Drag_Button.SetDownUI     ( "V3_BtnMoveOn_N" );
   Drag_Button.SetHighlightUI( "V3_BtnMoveOn_H" ); 
   dragEvent.SetUI    ( this ); 
   
   UPSize_Button.SetSpriteId   ( "V3_EffectTrack" );
   UPSize_Button.SetDownUI     ( "V3_EffectTrack" );
   UPSize_Button.SetHighlightUI( "V3_EffectTrack" ); 
   sizeEvent.SetUI    ( this ); 

   m_ListPinE.SetSpriteId   ( "V3_BtnPinOn_N" );
   m_ListPinE.SetHighlightUI( "V3_BtnPinOn_H" ); 
   m_ListPinD.SetSpriteId   ( "V3_BtnPinOff_N" );
   m_ListPinD.SetHighlightUI( "V3_BtnPinOff_H" );
   pinEvent.SetUI    ( this );

   m_iPinnedStatus = g_Global.GetDialogLastStatusP();
   if(m_iPinnedStatus == 0)
   {
      m_ListPinD.Show();
      m_ListPinE.Hide();
      Drag_Button  .Show();
      UPSize_Button.Show();
   }
   else
   {
      m_ListPinE.Show();
      m_ListPinD.Hide();
      Drag_Button  .Hide();
      UPSize_Button.Hide();
   }

   UpdateAllFramePos(m_iXPos,m_iYPos);
   
   m_BtnScrollUp.Hide();
   m_BtnScrollDn.Hide();
   
   AddChild( &Drag_Button);
   AddChild( &UPSize_Button);
   AddChild( &m_BtnScrollUp );
   AddChild( &m_BtnScrollDn );
   AddChild( &m_ListPinE);
   AddChild( &m_ListPinD);

  

   RootBoxUI::GetInstance()->AddChild( this );

   
} 

V3_EffectStatusDlg::~V3_EffectStatusDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_EffectStatusDlg *V3_EffectStatusDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_EffectStatusDlg instance;
    return &instance;
    
} 

void V3_EffectStatusDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   if(m_iPinnedStatus == 0)
   {
      SetPos( FramePos( iPosX, iPosY, iPosX + g_iEffetW, iPosY + (m_iNbrVisibleEffect*g_iEffetH)+iTrackSizeMY+iTrackSizeSY ) );
   }
   else
   {
      SetPos( FramePos( iPosX, iPosY, iPosX + g_iEffetW, iPosY + g_iEffetH+iTrackSizeMY+iTrackSizeSY ) );
   }

   int size = m_ListEffects.size() - m_iNbrVisibleEffect;
   if( size > 0 )
   {
      m_bScrollEnabled = true;
      SetListSize(size + 1);
   }
   else
   {
      m_bScrollEnabled = false;
      SetListSize(0);
   }
   
   CalcVisibleEffects();
   if(m_iPinnedStatus == 0)
   {
      //on valide si deja plus grand...
      int iSize = (int)m_ListEffects.size() - m_iNbrVisibleEffect;
      if(iSize <= 0 )
         SetPos( FramePos( iPosX, iPosY, iPosX + g_iEffetW, iPosY + (m_iNbrVisibleEffect*g_iEffetH)+iTrackSizeMY+iTrackSizeSY ));
   }
   
   m_BtnScrollUp.SetPos( FramePos( iPosX+11, iPosY - 19, iPosX+38, iPosY-2 ) );
   m_BtnScrollDn.SetPos( FramePos( iPosX+11, 
                                   iPosY+iTrackSizeMY+iTrackSizeSY + g_iEffetH * ( m_iNbrVisibleEffect ),
                                   iPosX+38,
                                   iPosY+iTrackSizeMY+iTrackSizeSY + g_iEffetH * ( m_iNbrVisibleEffect + 1 ) ));

   Drag_Button .SetPos( FramePos( iPosX+22, iPosY+2, iPosX + 42, iPosY+22 ) );
   m_ListPinE  .SetPos( FramePos( iPosX   , iPosY+2, iPosX + 20, iPosY+22 ) );
   m_ListPinD  .SetPos( FramePos( iPosX   , iPosY+2, iPosX + 20, iPosY+22 ) );

   int iySizeBtn = iPosY+iTrackSizeMY+2 + g_iEffetH * ( m_iNbrVisibleEffect );
   UPSize_Button .SetPos( FramePos( iPosX, iySizeBtn, iPosX + g_iEffetW, iySizeBtn+6 ) );

   

   

   for(UINT i = 0; i < displayedEffects.size(); i++ )
   {
      displayedEffects[ i ].SetPos( FramePos( iPosX, iPosY+iTrackSizeMY + ( g_iEffetH * i ), iPosX + g_iEffetW, iPosY+iTrackSizeMY + (g_iEffetH * (i + 1) ) ));
   }
   
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_EffectStatusDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iNbrVisibleEffect = g_Global.GetDialogLastStatusNbrEffect();
   GetInstance()->m_iPinnedStatus     = g_Global.GetDialogLastStatusP();
   if(GetInstance()->m_iPinnedStatus == 0)
   {
      GetInstance()->m_ListPinD.Show();
      GetInstance()->m_ListPinE.Hide();
      GetInstance()->Drag_Button  .Show();
      GetInstance()->UPSize_Button.Show();
   }
   else
   {
      GetInstance()->m_ListPinE.Show();
      GetInstance()->m_ListPinD.Hide();
      GetInstance()->Drag_Button  .Hide();
      GetInstance()->UPSize_Button.Hide();
   }

   GetInstance()->m_iXPos = g_Global.GetDialogLastStatusXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastStatusYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);
   GetInstance()->Show();
   
   return 0;
}


 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_EffectStatusDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   if(!g_SaveGame.GetOptionsParam()->bDisplayStatus)
   {
      Hide();
      return;
   }

   CalcEffectInfo();

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
      
      if(iNewPosX+g_iEffetW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_iEffetW;
      if(iNewPosY+g_iEffetH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_iEffetH;
      
      
      UpdateAllFramePos(iNewPosX,iNewPosY);
      
      iOffX = iNewPosX;
      iOffY = iNewPosY;
      
   }
   if(m_bSizeDialogY)
   {
      //on essaie de calculer le nombre de visible selon la positiond e la souris

      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      int iNewPosY = (mousePos.y-m_iYPosSR);
      if(iNewPosY <0)
      {
         //on diminue le nbr de visible...
         int nbrChange = (abs(iNewPosY)+24)/42;

         m_iNbrVisibleEffect = m_iNbrVisibleEffectSR-nbrChange;
         if(m_iNbrVisibleEffect <2)
            m_iNbrVisibleEffect = 2;
      }
      else
      {
         //on diminue le nbr de visible...
         int nbrChange = (iNewPosY+24)/42;
         
         m_iNbrVisibleEffect = m_iNbrVisibleEffectSR+nbrChange;
         if(m_iNbrVisibleEffect >20)
            m_iNbrVisibleEffect = 20;
      }
      

      if(iNewPosY < 0)
         iNewPosY = 0;

      UpdateAllFramePos(m_iXPos,m_iYPos);
      

   }
   GameUI::Draw( vsfFX );
 
}


void V3_EffectStatusDlg::Hide()
{ 
   BoxUI::Hide();
}

void V3_EffectStatusDlg::Show()
{
   BoxUI::Show();
}













// Adds an effect to the status UI.
// DWORD id,          // A unique identifier for the effect.
// DWORD time,        // The time which the effect lasts.
// DWORD totalTime,   // Total duration of effect
// DWORD iconId,      // The icon to display.
// std::string desc   // The effect's description.
//////////////////////////////////////////////////////////////////////////////////////////
void V3_EffectStatusDlg::AddEffect(DWORD id,DWORD time,DWORD totalTime,DWORD iconId, std::string desc)
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   
   if( time == 0 )
      return;
   
   if( totalTime < time )
      totalTime = time;
   
   // Try to find a corresponding effect in the m_ListEffects list.
   list< Effect >::iterator i;
   for( i = m_ListEffects.begin(); i != m_ListEffects.end(); i++ )
   {
      // If one was found
      if( (*i).id == id )
      {
         if( time == 0xFFFFFFFF )
         {
            (*i).infinite = true;
            (*i).expireTime = 0xFFFFFFFF;
         }
         else
         {
            (*i).infinite = false;
            (*i).expireTime = GetTickCount() + time;
         }
         // Update its expireTime;            
         (*i).initialTime = totalTime;
         
         CalcVisibleEffects();
         return;
      }
   }
   
   Effect effect;
   
   effect.id = id;
   if( time == 0xFFFFFFFF )
   {
      effect.infinite = true;
      effect.expireTime = 0xFFFFFFFF;
   }
   else
   {
      effect.infinite = false;
      effect.expireTime = GetTickCount() + time;
   }
   
   effect.initialTime = totalTime;
   effect.desc = desc;
   effect.SetVSF( SpellIcons( iconId ) );
   effect.SetPos( FramePos( 0, 0, effect.GetVSF()->GetWidth(), effect.GetVSF()->GetHeight() ) );
   
   m_ListEffects.push_back( effect );
   
   int size = m_ListEffects.size() - m_iNbrVisibleEffect;
   if( size > 0 )
   {
      m_bScrollEnabled = true;
      SetListSize(size + 1);
   }
   else
   {
      m_bScrollEnabled = false;
      SetListSize(0);
   }
   
   CalcVisibleEffects();
}




//  Calculates which effects are currently visibible on screen and setups the 
//  displayedEffects vector accordingly.
void V3_EffectStatusDlg::CalcVisibleEffects( void )
{
   // First sort the effect list.
   m_ListEffects.sort();
   
   int j, ySize = iTrackSizeMY;
   list< Effect >::iterator i;
   // Move the the scroll offset
   i = m_ListEffects.begin();
   j = 0;
   while( i != m_ListEffects.end() && j < m_iListPos )
   {
      i++;
      j++;
   }
   
   // Setup the displayable effects while there is displayable space.
   j = 0;
   while( i != m_ListEffects.end() && j < (int)m_iNbrVisibleEffect )
   {
      // Point the displayed effect to this effect.
      displayedEffects[ j ].effect = &(*i);
      
      ySize += 42;
      j++;
      i++;
   }
   
   // All non-displayed effects
   for( ; j < (int)displayedEffects.size(); j++ )
   {
      displayedEffects[ j ].effect = NULL;
   }
   
   DWORD yOffset = m_iYPos;
   
   
   if( m_iListPos == 0 )
   {
      m_BtnScrollUp.Hide();
   }
   else
   {
      m_BtnScrollUp.Show();
      yOffset -= 19;
   }
   
   if( m_iListPos >= m_iMaxListSize - 1 || m_iMaxListSize == 0 )
   {
      m_BtnScrollDn.Hide();
   }
   else
   {
      ySize += 19;
      m_BtnScrollDn.Show();
   }
   
   FramePos thisPos = GetPos();
   thisPos.maxy = m_iYPos + ySize+iTrackSizeSY;
   thisPos.y = yOffset;
   SetPos( thisPos );
}

//  Updates the effect information
void V3_EffectStatusDlg::CalcEffectInfo( void )
{
   DWORD curTick = GetTickCount();
   
   list< DWORD > expireList;
   
   // Check for any expired m_ListEffects    
   list< Effect >::iterator k;
   for( k = m_ListEffects.begin(); k != m_ListEffects.end(); k++ )
   {
      if( !(*k).infinite && curTick >= (*k).expireTime )
      {
         expireList.push_back( (*k).id );            
      }
   }
   // Dispell all m_ListEffects which have expired.
   list< DWORD >::iterator l;
   for( l = expireList.begin(); l != expireList.end(); l++ )
   {
      DispellEffect( *l );
   }
   
   
   // Update the displayed effects info.
   int i;
   for( i = 0; i < m_iNbrVisibleEffect; i++ )
   {
      EffectDisplay *e = &displayedEffects[ i ];
      
      if( e->effect == NULL )
      {
         continue;
      }
      
      // If the information is currently being displayed
      if( e->IsHelpDisplayed() || !e->bTextInitFirstTime)
      {
         e->bTextInitFirstTime = true;
         // Update it.
         string helpText = e->effect->desc;
         
         // Get the remaining time in friendly format.
         int secondsRemaining = ( e->effect->expireTime - curTick ) / 1000;
         
         int sec, min, hour,day;
         
         day   = secondsRemaining / 86400;
         sec   = (secondsRemaining % 60);
         min   = (secondsRemaining % 3600) / 60;
         hour  = (secondsRemaining % 86400) / 3600;
         
         char buf[ 256 ];
         
         // INTL
         helpText += g_LocalString[ 442 ];
         
         if( e->effect->infinite )
         {
            helpText += g_LocalString[ 531 ];
         }
         else
         {
            if( day != 0 )
            {
               sprintf_s( buf,256, "%d %s, ", day, g_LocalString[601]);
               helpText += buf;
            }
            
            if( hour != 0 )
            {
               sprintf_s( buf,256, "%02u:", hour );
               helpText += buf;
            }
            
            if( hour != 0 || min != 0 ){
               sprintf_s( buf,256, "%02u:", min );
               helpText += buf;
            }
            if( hour != 0 || min != 0 || sec != 0 ){
               sprintf_s( buf,256, "%02u", sec );
               helpText += buf;
            }
         }
         
         e->GetHelpText()->SetText( helpText.c_str() );

         e->ResyncHelpText();
      }
   }
}


//  Dispells a given effect.
void V3_EffectStatusDlg::DispellEffect(DWORD id)
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   
   list< Effect >::iterator i;
   for( i = m_ListEffects.begin(); i != m_ListEffects.end(); i++ )
   {
      if( (*i).id == id )
      {
         m_ListEffects.erase( i );
         break;
      }
   }
   
   int size = m_ListEffects.size() - m_iNbrVisibleEffect;
   if( size > 0 )
   {
      m_bScrollEnabled = true;
      SetListSize(size + 1);
   }
   else
   {
      m_bScrollEnabled = false;
      SetListSize(0);
   }
   
   CalcVisibleEffects();
}

void V3_EffectStatusDlg::DispellAllEffect()
{
   CAutoLock autoLock( RootBoxUI::GetInstance()->GetLock() );
   
   m_ListEffects.clear();
   
   int size = m_ListEffects.size() - m_iNbrVisibleEffect;
   if( size > 0 )
   {
      m_bScrollEnabled = true;
      SetListSize(size + 1);
   }
   else
   {
      m_bScrollEnabled = false;
      SetListSize(0);
   }
   
   CalcVisibleEffects();
}













//////////////////////////////////////////////////////////////////////////////////////////
//  Draws the displayed effect status.
// 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_EffectStatusDlg::EffectDisplay::Draw(V2SPRITEFX *vsfFX)
{
   if( effect != NULL )
   {
      FramePos thisPos = GetPos();
      
      GameUI::Draw( vsfFX );
      
      // Center the icon
      FramePos itemPos = effect->GetPos();
      itemPos.SetNewOffset( MousePos( ( thisPos.x + 10 ) - itemPos.Width() / 2 + ( thisPos.Width()-10) / 2,thisPos.y - itemPos.Height() / 2+ thisPos.Height() / 2));
      effect->SetPos( itemPos );
      
      // Draw the effect's icon
      bool drawIcon = true;
      DWORD curTick = GetTickCount();
      
      // If there is less than 15 seconds remaining for the effect.
      if( ( effect->expireTime - curTick ) / 1000 <= 15 )
      {
         // Make it blink (undisplayed 300ms per second).        
         if( ( effect->expireTime - curTick ) % 1000 < 300 )
         {
            drawIcon = false;    
         }
      }
      
      if( drawIcon )
      {
         // Draw the icon.
         effect->Draw( vsfFX );
      }
      
      const int gaugeHeight = 39;
      
      // Query the emptyGauge's total size.
      int emptyGaugeSize = gaugeHeight - ( effect->expireTime - curTick ) * gaugeHeight / effect->initialTime;
      
      if( effect->infinite )
      {
         emptyGaugeSize = 0;
      }
      
      // Set the gauge's pos.
      emptyGauge.SetPos( FramePos( thisPos.x + 2, thisPos.y + 1, thisPos.maxx, thisPos.maxy ) );
      
      // Clip the empty bar's according to its current size.
      RECT clipRect = { thisPos.x, thisPos.y, thisPos.maxx, thisPos.y + 1 + emptyGaugeSize};
      
      V2SPRITEFX fx;
      memset( &fx, 0, sizeof( fx ) );
      fx.lpClipRect = &clipRect;
      emptyGauge.Draw( &fx );
   }
}


//////////////////////////////////////////////////////////////////////////////////////////
//  Checks for double-clicks on the status effects
bool V3_EffectStatusDlg::EffectDisplay::LeftMouseUp(MousePos mousePos)
{

 
   if(g_SaveGame.GetOptionsParam()->bMacroOnClick)
   {
      if( !IsClick() || effect == NULL )
         return true;
   }
   else if( !IsDblClick() || effect == NULL )
      return true;
   

   std::string step = "Before try";
   
   if( !V3_SpellDlg::GetInstance()->CastSpell( effect->id, true ) )
   {
      
      // This is a way to re-use an item with the information avaiable on the client side.
      // This is not a perfect system and should be removed if it cause any troubles.
      
      // Get the name of the item from its description.
      UINT pos = effect->desc.find(":",0);
      std::string fxName = effect->desc;
      if( pos < fxName.size() )
      {                
         fxName[pos] = 0;
      }
      else
      {
         fxName = "";
      }

      char strItemName[1024];
      sprintf_s(strItemName,1024,"%s",fxName.c_str());

      //remove all smace at the end of name...
      while(strItemName[strlen(strItemName)-1] == ' ')
         strItemName[strlen(strItemName)-1] = 0x00;

      fxName = strItemName;
      
      // Compare the name of the FX to the name of the objects in the backpack.
      // If it finds a match, it use the object.
      bool bF = false;
      Player.tlBackpack.Lock("Lock backpack");
      Player.tlBackpack.ToHead();
      while (Player.tlBackpack.QueryNext()) 
      {
         BAG_ITEM *obj = Player.tlBackpack.GetObject();
         if (!strcmp(fxName.c_str(), (char *)obj->chIName)) 
         {
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_UseObject;
            sending << (short)0;
            sending << (short)0;
            sending << (long)( obj->dwID );
            
            // send the use packet
            SEND_PACKET( sending );
            Player.tlBackpack.SetQueryState(STOP_QUERY);
            bF = true;
            
            char buf[ 512 ];
            FormatChargesLeft( buf, obj );
            V3_ChatLogDlg::GetInstance()->AddListLogs( "", buf, SYSTEM_COLOR,true);
         }
      }
      Player.tlBackpack.Unlock("Lock backpack");
      if (!bF) 
      {              
         V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 376 ], SYSTEM_COLOR, true );
      }
   }
   
   return true;
}



void V3_EffectStatusDlg::ScrollUpEvent::LeftClicked( void )
{
   V3_EffectStatusDlg *me = static_cast< V3_EffectStatusDlg * >( GetUI() );    

}

void V3_EffectStatusDlg::ScrollDownEvent::LeftClicked( void )
{
   V3_EffectStatusDlg *me = static_cast< V3_EffectStatusDlg * >( GetUI() );    

}

void V3_EffectStatusDlg::DragEvent::LeftClicked( void )
{
   V3_EffectStatusDlg *me = static_cast< V3_EffectStatusDlg * >( GetUI() );

   me->StopDragDlg();

}

void V3_EffectStatusDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_EffectStatusDlg *me = static_cast< V3_EffectStatusDlg * >( GetUI() );
}

void V3_EffectStatusDlg::SizeEvent::LeftClicked( void )
{
   V3_EffectStatusDlg *me = static_cast< V3_EffectStatusDlg * >( GetUI() );

   me->StopSizeDlg();
   
   
}

void V3_EffectStatusDlg::SizeEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_EffectStatusDlg *me = static_cast< V3_EffectStatusDlg * >( GetUI() );
   
}

void V3_EffectStatusDlg::PinEvent::LeftClicked( void )
{
   V3_EffectStatusDlg *me = static_cast< V3_EffectStatusDlg * >( GetUI() );
   
}

//  Left mouse down on the status UI (check for scroll bar)
bool V3_EffectStatusDlg::LeftMouseDown(MousePos mousePos)
{
   if( Drag_Button.GetPos().MousePosInFrame( mousePos ) && Drag_Button.IsShown())
   {
      StartDragDlg(mousePos);
      return true;
   }

   if( UPSize_Button.GetPos().MousePosInFrame( mousePos ) && UPSize_Button.IsShown())
   {
      StartSizeDlg(mousePos);
      return true;
   }

   if( m_ListPinE.GetPos().MousePosInFrame( mousePos ))
   {
      m_iPinnedStatus = 1-m_iPinnedStatus;
      if(m_iPinnedStatus == 0)
      {
         m_ListPinD.Show();
         m_ListPinE.Hide();
         Drag_Button  .Show();
         UPSize_Button.Show();
      }
      else
      {
         m_ListPinE.Show();
         m_ListPinD.Hide();
         Drag_Button  .Hide();
         UPSize_Button.Hide();
      }
      g_Global.SetDialogLastStatusP(m_iPinnedStatus);
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      return true;
   }

   if( !m_bScrollEnabled )
   {
      return false;
   }
   
   if( m_BtnScrollUp.GetPos().MousePosInFrame( mousePos ) && m_BtnScrollUp.IsShown())
   {
      if(m_iListPos >0)
         m_iListPos--;
      CalcVisibleEffects();
      return true;
   }
   else if( m_BtnScrollDn.GetPos().MousePosInFrame( mousePos ) && m_BtnScrollDn.IsShown())
   {
      if(m_iListPos < m_iMaxListSize-1)
         m_iListPos++;
      CalcVisibleEffects();
      return true;
   }

   

   return true;
}
 
//  Left mouse down on the status UI (check for scroll bar)
bool V3_EffectStatusDlg::LeftMouseUp(MousePos mousePos)
{
   StopDragDlg();
   StopSizeDlg();
   
   UINT i;
   for( i = 0; i < displayedEffects.size(); i++ )
   {
      EffectDisplay *e = &displayedEffects[ i ];

      

      if(e && e->effect != NULL)
      {
         FramePos thisPos = e->GetPos();
         if(mousePos.x > thisPos.x && mousePos.x < thisPos.maxx &&
            mousePos.y > thisPos.y && mousePos.y < thisPos.maxy     )
         {
             return e->LeftMouseUp(mousePos);
         }

      }
   }
   return true;
}


//  Left mouse down on the status UI (check for scroll bar)
bool V3_EffectStatusDlg::RightMouseUp(MousePos mousePos)
{
   
   UINT i;
   for( i = 0; i < displayedEffects.size(); i++ )
   {
      EffectDisplay *e = &displayedEffects[ i ];
      if(e && e->effect != NULL)
      {
         FramePos thisPos = e->GetPos();
         if(mousePos.x > thisPos.x && mousePos.x < thisPos.maxx &&
            mousePos.y > thisPos.y && mousePos.y < thisPos.maxy     )
         {
            
            if( IsClick() )
            {
               e->DisplayHelp( true, mousePos.x, mousePos.y );
            }
            return true;
         }
         
      }
   }
   return true;
}

void V3_EffectStatusDlg::StartDragDlg( MousePos mousePos )
{
   if(!g_SaveGame.GetOptionsParam()->bGUIDynamic)
      return;
   
   if(!m_bDragDialog && !m_bSizeDialogY&& !m_iPinnedStatus)
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

void V3_EffectStatusDlg::StopDragDlg()
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
      
      if(m_iXPos+g_iEffetW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_iEffetW;
      if(m_iYPos+g_iEffetH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_iEffetH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastStatusXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;
      
      
      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

void V3_EffectStatusDlg::StartSizeDlg( MousePos mousePos )
{
   if(!g_SaveGame.GetOptionsParam()->bGUIDynamic)
      return;

   if(!m_bDragDialog && !m_bSizeDialogY && !m_iPinnedStatus)
   {
      
      m_bSizeDialogY = true;
      m_iYPosSR = mousePos.y;
      m_iNbrVisibleEffectSR = m_iNbrVisibleEffect;
      RootBoxUI::GetInstance()->SetDragUI((GameUI*)&UPSize_Button);
      g_Global.SetDraggingDialog(true);
   }
   
}

void V3_EffectStatusDlg::StopSizeDlg()
{
   if(m_bSizeDialogY)
   {
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastStatusNbrEffect(m_iNbrVisibleEffect);

      m_bSizeDialogY = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
   }
}