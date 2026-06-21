// RootBoxUI.cpp
//
#include "../pch.h"
#include "RootBoxUI.h"
#include <mmsystem.h>


#include "V3_Mp3Dlg.h"
#include "V3_ArenePointsDlg.h"
#include "V3_TMIDlg.h"
#include "V3_LifeDlg.h"
#include "V3_TargetDlg.h"
#include "V3_GroupOSDlg.h"
#include "V3_EffectStatusDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_MainBarDlg.h"
#include "V3_ChatDlg.h"
#include "V3_GMDlg.h"
#include "V3_RTMapDlg.h"
#include "V3_ItemInfoDlg.h"
#include "V3_DisplayHelpDlg.h"
#include "V3_AHVendreDlg.h"
#include "V3_InvDlg.h"

#include "..\TFCPlayer.h"
#include "..\SaveGame.h"
#include "..\App.h"

extern CSaveGame g_SaveGame;
extern bool g_UiInit;
extern bool g_pendingInventoryRefresh;
extern TFCPlayer Player;
extern DWORD TargetID;
extern DWORD SelectedID;


RootBoxUI::RootBoxUI() 
{
    // Always show the root box IU.
   m_puiDragging = NULL;

   Show();
    
}


//////////////////////////////////////////////////////////////////////////////////////////
RootBoxUI *RootBoxUI::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
// Returns the sole RootBoxUI instance.
//////////////////////////////////////////////////////////////////////////////////////////
{
    static RootBoxUI rootBox;

    return &rootBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RootBoxUI::LeftMouseDown
//////////////////////////////////////////////////////////////////////////////////////////
//  Left mouse down. Thread safe.
// 
(
 MousePos mousePos
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );
   
    
    // If this click happens shorter than the click-aware time.
    if( timeGetTime() - lastClickTime <= ClickTime )
    {
        if( currentMouseState == MouseClick )
        {

            currentMouseState = MouseDblClick;
        }
    }
    else
    {
        currentMouseState = MouseNone;        
    }

    leftMouseDown = true;
    lastClickTime = timeGetTime();

    //Ajout de la validation MAIN GUI V3...
    if( V3_MainBarDlg::GetInstance()->IsShown() && V3_MainBarDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_MainBarDlg::GetInstance()->LeftMouseDown( mousePos );
       
       mouseOwned = true;
       return true;
    }

    if( V3_TMIDlg::GetInstance()->IsShown() && V3_TMIDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_TMIDlg::GetInstance()->LeftMouseDown( mousePos );

        mouseOwned = true;
        return true;
    }

    if( V3_LifeDlg::GetInstance()->IsShown() && V3_LifeDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_LifeDlg::GetInstance()->LeftMouseDown( mousePos );
       
       mouseOwned = true;
       return true;
    }

    if( V3_TargetDlg::GetInstance()->IsShown() && V3_TargetDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_TargetDlg::GetInstance()->LeftMouseDown( mousePos );
       
       mouseOwned = true;
       return true;
    }

    if( V3_GroupOSDlg::GetInstance()->IsShown() && V3_GroupOSDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_GroupOSDlg::GetInstance()->LeftMouseDown( mousePos );
       
       mouseOwned = true;
       return true;
    }


    if( V3_EffectStatusDlg::GetInstance()->IsShown() && V3_EffectStatusDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_EffectStatusDlg::GetInstance()->LeftMouseDown( mousePos );
       mouseOwned = true;
       return true;
    }

    

    if( V3_ChatLogDlg::GetInstance()->IsShown() && V3_ChatLogDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ChatLogDlg::GetInstance()->LeftMouseDown( mousePos );
       
       mouseOwned = true;
       return true;
    }

    if( V3_Mp3Dlg::GetInstance()->IsShown() && V3_Mp3Dlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_Mp3Dlg::GetInstance()->LeftMouseDown( mousePos );
       
       mouseOwned = true;
       return true;
    }

    if( V3_ArenePointsDlg::GetInstance()->IsShown() && V3_ArenePointsDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ArenePointsDlg::GetInstance()->LeftMouseDown( mousePos );

       mouseOwned = true;
       return true;
    }

    

    

    if( BoxUI::LeftMouseDown( mousePos ) )
    {
        mouseOwned = true;
        return true;
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool RootBoxUI::LeftMouseUp
//////////////////////////////////////////////////////////////////////////////////////////
//  Left mouse up. Thread safe.
// 
(
 MousePos mousePos
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );

    if( timeGetTime() - lastClickTime <= ClickTime )
    {
        if( currentMouseState == MouseNone )
        {
            currentMouseState = MouseClick;
        }
    }
    else
    {
        currentMouseState = MouseNone;
    }

    leftMouseDown = false;

    //Ajout de la validation MAIN GUI V3...
    if( V3_MainBarDlg::GetInstance()->IsShown() && V3_MainBarDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_MainBarDlg::GetInstance()->LeftMouseUp( mousePos );
       
       mouseOwned = true;
       return true;
    }
    
    if( V3_TMIDlg::GetInstance()->IsShown() && V3_TMIDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_TMIDlg::GetInstance()->LeftMouseUp( mousePos );

       mouseOwned = true;
       return true;
    }

    if( V3_LifeDlg::GetInstance()->IsShown() && V3_LifeDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_LifeDlg::GetInstance()->LeftMouseUp( mousePos );
       
       mouseOwned = true;
       return true;
    }

    if( V3_TargetDlg::GetInstance()->IsShown() && V3_TargetDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_TargetDlg::GetInstance()->LeftMouseUp( mousePos );
       
       mouseOwned = true;
       return true;
    }

    if( V3_GroupOSDlg::GetInstance()->IsShown() && V3_GroupOSDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_GroupOSDlg::GetInstance()->LeftMouseUp( mousePos );
       
       mouseOwned = true;
       return true;
    }
    

    if( V3_EffectStatusDlg::GetInstance()->IsShown() && V3_EffectStatusDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_EffectStatusDlg::GetInstance()->LeftMouseUp( mousePos );
       
       mouseOwned = true;
       return true;
    }

    

    if( V3_ChatLogDlg::GetInstance()->IsShown() && V3_ChatLogDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ChatLogDlg::GetInstance()->LeftMouseUp( mousePos );
       
       mouseOwned = true;
       return true;
    }

    if( V3_Mp3Dlg::GetInstance()->IsShown() && V3_Mp3Dlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_Mp3Dlg::GetInstance()->LeftMouseUp( mousePos );
       
       mouseOwned = true;
       return true;
    }

    if( V3_ArenePointsDlg::GetInstance()->IsShown() && V3_ArenePointsDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ArenePointsDlg::GetInstance()->LeftMouseUp( mousePos );

       mouseOwned = true;
       return true;
    }

    

    

    if( BoxUI::LeftMouseUp( mousePos ) ){
        mouseOwned = true;
        return true;
    }
    return false;

}
//////////////////////////////////////////////////////////////////////////////////////////
bool RootBoxUI::RightMouseDown
//////////////////////////////////////////////////////////////////////////////////////////
// Right mouse down. Thread safe.
// 
(
 MousePos mousePos
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );

    
    // If this click happens shorter than the click-aware time.
    if( timeGetTime() - lastClickTime <= ClickTime )
    {
        if( currentMouseState == MouseClick )
        {
            currentMouseState = MouseDblClick;
        }
    }
    else
    {
        currentMouseState = MouseNone;        
    }


    lastClickTime = timeGetTime();

    //Ajout de la validation MAIN GUI V3...
    if( V3_MainBarDlg::GetInstance()->IsShown() && V3_MainBarDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_MainBarDlg::GetInstance()->RightMouseDown( mousePos );
       
       return true;
    }

    if( V3_TMIDlg::GetInstance()->IsShown() && V3_TMIDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_TMIDlg::GetInstance()->RightMouseDown( mousePos );

       return true;
    }

    if( V3_LifeDlg::GetInstance()->IsShown() && V3_LifeDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_LifeDlg::GetInstance()->RightMouseDown( mousePos );
       
       return true;
    }

    if( V3_TargetDlg::GetInstance()->IsShown() && V3_TargetDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_TargetDlg::GetInstance()->RightMouseDown( mousePos );
       
       return true;
    }

    if( V3_GroupOSDlg::GetInstance()->IsShown() && V3_GroupOSDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_GroupOSDlg::GetInstance()->RightMouseDown( mousePos );
       
       return true;
    }

    if( V3_EffectStatusDlg::GetInstance()->IsShown() && V3_EffectStatusDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_EffectStatusDlg::GetInstance()->RightMouseDown( mousePos );
       
       return true;
    }

    if( V3_ChatLogDlg::GetInstance()->IsShown() && V3_ChatLogDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ChatLogDlg::GetInstance()->RightMouseDown( mousePos );
       
       return true;
    }

    if( V3_Mp3Dlg::GetInstance()->IsShown() && V3_Mp3Dlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_Mp3Dlg::GetInstance()->RightMouseDown( mousePos );
       
       return true;
    }

    if( V3_ArenePointsDlg::GetInstance()->IsShown() && V3_ArenePointsDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ArenePointsDlg::GetInstance()->RightMouseDown( mousePos );

       return true;
    }

    

    

    return BoxUI::RightMouseDown( mousePos );
}
//////////////////////////////////////////////////////////////////////////////////////////
bool RootBoxUI::RightMouseUp
//////////////////////////////////////////////////////////////////////////////////////////
//  Right mouse up. Thread safe.
// 
(
 MousePos mousePos
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );

    if( timeGetTime() - lastClickTime <= ClickTime )
    {
        if( currentMouseState == MouseNone )
        {

            currentMouseState = MouseClick;
        }
    }
    else
    {
        currentMouseState = MouseNone;
    }

    //Ajout de la validation MAIN GUI V3...
    if( V3_MainBarDlg::GetInstance()->IsShown() && V3_MainBarDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_MainBarDlg::GetInstance()->RightMouseUp( mousePos );
       
       return true;
    }
    
    if( V3_TMIDlg::GetInstance()->IsShown() && V3_TMIDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_TMIDlg::GetInstance()->RightMouseUp( mousePos );

       return true;
    }

    if( V3_LifeDlg::GetInstance()->IsShown() && V3_LifeDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_LifeDlg::GetInstance()->RightMouseUp( mousePos );
       
       return true;
    }

    if( V3_TargetDlg::GetInstance()->IsShown() && V3_TargetDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_TargetDlg::GetInstance()->RightMouseUp( mousePos );
       
       return true;
    }

    if( V3_GroupOSDlg::GetInstance()->IsShown() && V3_GroupOSDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_GroupOSDlg::GetInstance()->RightMouseUp( mousePos );
       
       return true;
    }
    

    if( V3_EffectStatusDlg::GetInstance()->IsShown() && V3_EffectStatusDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_EffectStatusDlg::GetInstance()->RightMouseUp( mousePos );
       
       return true;
    }

    if( V3_ChatLogDlg::GetInstance()->IsShown() && V3_ChatLogDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ChatLogDlg::GetInstance()->RightMouseUp( mousePos );
       
       return true;
    }

    if( V3_Mp3Dlg::GetInstance()->IsShown() && V3_Mp3Dlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_Mp3Dlg::GetInstance()->RightMouseUp( mousePos );
       
       return true;
    }

    if( V3_ArenePointsDlg::GetInstance()->IsShown() && V3_ArenePointsDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ArenePointsDlg::GetInstance()->RightMouseUp( mousePos );

       return true;
    }

    



    return BoxUI::RightMouseUp( mousePos );
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RootBoxUI::DragCycle
//////////////////////////////////////////////////////////////////////////////////////////
//  Drag cycle, thread safe.
// 
(
 MousePos mousePos
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );

    //Ajout de la validation MAIN GUI V3...
    if( V3_MainBarDlg::GetInstance()->IsShown() && V3_MainBarDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       return true;
    }

    if( V3_TMIDlg::GetInstance()->IsShown() && V3_TMIDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       return true;
    }

    if( V3_LifeDlg::GetInstance()->IsShown() && V3_LifeDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       return true;
    }

    if( V3_TargetDlg::GetInstance()->IsShown() && V3_TargetDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       return true;
    }

    if( V3_GroupOSDlg::GetInstance()->IsShown() && V3_GroupOSDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       return true;
    }

    if( V3_EffectStatusDlg::GetInstance()->IsShown() && V3_EffectStatusDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       return true;
    }

    if( V3_ChatLogDlg::GetInstance()->IsShown() && V3_ChatLogDlg::GetInstance()->GetPos().MousePosInFrame( mousePos ) )
    {
       return true;
    }

    return BoxUI::DragCycle( mousePos );
}

//  Wheel up. Thread safe.
bool RootBoxUI::WheelUp(MousePos mousePos)
{
    CAutoLock autoLock( &threadLock );

    if( V3_ChatLogDlg::GetInstance()->IsShown() && V3_ChatLogDlg::GetInstance()->m_Background.GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ChatLogDlg::GetInstance()->m_ListText.ScrollUp( 1 );
       return true;
    }

    return BoxUI::WheelUp( mousePos );
}

//  Wheel down. Thread safe.
bool RootBoxUI::WheelDown(MousePos mousePos)
{
    CAutoLock autoLock( &threadLock );

    if( V3_ChatLogDlg::GetInstance()->IsShown() && V3_ChatLogDlg::GetInstance()->m_Background.GetPos().MousePosInFrame( mousePos ) )
    {
       V3_ChatLogDlg::GetInstance()->m_ListText.ScrollDown( 1 );
       return true;
    }

    return BoxUI::WheelDown( mousePos );
}
//////////////////////////////////////////////////////////////////////////////////////////
bool RootBoxUI::TextInput
//////////////////////////////////////////////////////////////////////////////////////////
//  KbHit, thread safe.
// 
(
 char ch
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );

    if( foregroundChild != NULL ){
        // Send the keyboard event to it.
        foregroundChild->TextInput( ch );        
        return true;
    }

    return BoxUI::TextInput( ch );
}    

void RootBoxUI::SetSideMenuState( bool bLoadCC ) 
{
   V3_Mp3Dlg::GetInstance()->Display(NULL);
   V3_TMIDlg::GetInstance()->Display(NULL);
   V3_LifeDlg::GetInstance()->Display(NULL);
   V3_TargetDlg::GetInstance()->Display(NULL);
   V3_GroupOSDlg::GetInstance()->Display(NULL);
   V3_EffectStatusDlg::GetInstance()->Display(NULL);
   V3_ChatLogDlg::GetInstance()->Display(NULL);
   if(bLoadCC)
      V3_ChatDlg::GetInstance()->SetupServerAllParam();
   V3_MainBarDlg::GetInstance()->Display(NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RootBoxUI::VKeyInput(VKey vKey)
{
   CAutoLock autoLock( &threadLock );
   
   bool sendChar = true;
   
   // If there is a defined foreground child.
   if( foregroundChild != NULL )
   {
      // Send the keyboard event to it.
      foregroundChild->VKeyInput( vKey );
      return true;
   }
   
   
   if ( vKey.CTRL() == true && (vKey.GetKey() == VK_UP || vKey.GetKey() == VK_DOWN) ) 
   {
      if ( V3_ChatLogDlg::GetInstance()->VKeyInput( vKey ) ) 
         return true;
   } 
   else if( vKey.GetKey() == VK_ESCAPE )
   {

   }
   else if( vKey.GetKey() == VK_TAB )
   {
      sendChar = false;
      V3_RTMapDlg *pRTMap  = V3_RTMapDlg::GetInstance();

      if(pRTMap->IsShown())
         pRTMap->Hide();
      else
         pRTMap->Display(NULL);
   }
   else if( vKey.GetKey() == VK_F1)
   {
      sendChar = false;

      if(V3_DisplayHelpDlg::GetInstance()->IsShown())
         V3_DisplayHelpDlg::GetInstance()->Hide();
      else
         V3_DisplayHelpDlg::GetInstance()->ShowHelp(1);
   }
   else if( vKey.GetKey() == VK_F9 && Player.CanRunScripts)
   {
      sendChar = false;
      V3_GMDlg   *pV3_GMDlg  = V3_GMDlg::GetInstance();
      
      if(pV3_GMDlg->IsShown())
         pV3_GMDlg->Hide();
      else
      {
         pV3_GMDlg->Display(NULL,true,0,Player.World);
      }
   }
   

   if(g_Global.IsItemInfoAvailable())
   {
      V3_ItemInfoDlg::GetInstance()->Hide();
      g_Global.ResetItemInfo();
      sendChar = false;
   }
   
   if( sendChar )
   {
      return BoxUI::VKeyInput( vKey );
   }
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void RootBoxUI::Draw( V2SPRITEFX *vsfFX )
//////////////////////////////////////////////////////////////////////////////////////////
//  Drawing thread safe.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
   CAutoLock autoLock( &threadLock );
   
   if( !IsLeftMouseDown() )
   {
      mouseOwned = false;        
   }
   
   vector< GameUI * > drawList;
   
   bool fullscreenBoxAdded = false;
   
   // Check all children to query those that need to be drawn.
   ChildQueue::iterator i;
   for( i = GetChildList()->begin(); i != GetChildList()->end(); i++ )
   {
      
      //NMNMNM_DeathNMNMNM
      //Ajout de la validation MAIN GUI V3...
      if( (*i)->IsShown() &&      (*i) != V3_TMIDlg::GetInstance()             && 
                                  (*i) != V3_LifeDlg::GetInstance()            && 
                                  (*i) != V3_Mp3Dlg::GetInstance()             && 
                                  (*i) != V3_ArenePointsDlg::GetInstance()     && 
                                  (*i) != V3_TargetDlg::GetInstance()          && 
                                  (*i) != V3_GroupOSDlg::GetInstance()         && 
                                  (*i) != V3_EffectStatusDlg::GetInstance()    && 
                                  (*i) != V3_ChatLogDlg::GetInstance()         &&
                                  (*i) != V3_MainBarDlg::GetInstance()    )
      {
         // If the UI is either minimized, or is the first
         // fullscreen box to be added.
         if( ( !fullscreenBoxAdded && (*i) != foregroundChild ) )
         {
            // If at least one box is not minimized
            if(1)
            {
               fullscreenBoxAdded = true;
               // Add the ui to the list of UIs to draw.
               drawList.push_back( *i );
            }
         }
         else
         {
            if( (*i) != foregroundChild)
            {
               (*i)->Hide();
            }
         }
      }
   }

   
   vector< GameUI * >::iterator j;
   // Draw everything in the draw list.
   for( j = drawList.begin(); j != drawList.end(); j++ )
   {
      (*j)->Draw( NULL );        
   }

   
   
   
   // Draw the side menu on top of everyting else.
  
   
   //Ajout de la validation MAIN GUI V3...

   if(V3_Mp3Dlg::GetInstance()->IsShown())
      V3_Mp3Dlg::GetInstance()->Draw(NULL);

   if(V3_ArenePointsDlg::GetInstance()->IsShown())
      V3_ArenePointsDlg::GetInstance()->Draw(NULL);

   if(V3_TMIDlg::GetInstance()->IsShown())
      V3_TMIDlg::GetInstance()->Draw(NULL);

   if(V3_LifeDlg::GetInstance()->IsShown())
      V3_LifeDlg::GetInstance()->Draw(NULL);



   Objects.Lock(666);
   if(SelectedID)
   {
      if(!V3_TargetDlg::GetInstance()->IsShown())
         V3_TargetDlg::GetInstance()->Display(NULL);

      BOOL bForceHide = TRUE;
      TFCObject* pObj = Objects.GetObjectByID(SelectedID);
      bool bOK = FALSE;
      if(pObj && pObj->Type3D !=-1)
         bOK = TRUE;
      else if(pObj)
      {
         if((pObj->Type >= 23000 && pObj->Type <=23500)            )
            bOK = TRUE; //object De type Monster
      }
      if(bOK)
      {
         int iHP = pObj->pHp;
         if(!pObj->m_pName)
         {
            //Send identify...
            short shIdn = 0;
            TFCPacket Send; 
            Send << (short)RQ_GetUnitName2;
            Send << (long)pObj->ID;
            Send << (short)(pObj->OX+Player.xPos);
            Send << (short)(pObj->OY+Player.yPos);
            Send << (short)shIdn;
            SEND_PACKET(Send);	
         }
         if(Objects.CheckDeadDirect(pObj))
            iHP = 0;
         V3_TargetDlg::GetInstance()->UpdateInfo(pObj->m_pName,iHP,pObj->wLevel);
         V3_TargetDlg::GetInstance()->Draw(NULL,pObj);
         bForceHide = FALSE;
      }

      if(bForceHide) 
      {
         SelectedID = NULL;
         V3_TargetDlg::GetInstance()->Hide();
      }
   }
   else if(!SelectedID && V3_TargetDlg::GetInstance()->IsShown())
      V3_TargetDlg::GetInstance()->Hide();
   Objects.Unlock(666);

   if(V3_GroupOSDlg::GetInstance()->IsShown())
      V3_GroupOSDlg::GetInstance()->Draw(NULL);

   if(V3_EffectStatusDlg::GetInstance()->IsShown())
      V3_EffectStatusDlg::GetInstance()->Draw(NULL);

   if(V3_ChatLogDlg::GetInstance()->IsShown())
      V3_ChatLogDlg::GetInstance()->Draw(NULL);
   
   if(V3_MainBarDlg::GetInstance()->IsShown())
      V3_MainBarDlg::GetInstance()->Draw(NULL);

  

   if(g_Global.IsItemInfoAvailable() && !V3_AHVendreDlg::GetInstance()->IsShown())
   {
      sItemInfo *pInfo = g_Global.GetItemInfo();
      if(pInfo->shAC     == 0 && pInfo->shDodge == 0 && pInfo->shEnd == 0 && pInfo->lMinDamage == 0 && pInfo->lMaxDamage == 0 && 
         pInfo->shAttack == 0 && pInfo->shStr   == 0 && pInfo->shAgi == 0 && pInfo->shWis      == 0 && pInfo->shInt      == 0 && 
         pInfo->vBoust.size() == 0)
      {
         g_Global.ResetItemInfo();
         Objects.Lock(40);
         Objects.Identify(pInfo->ulID);
         Objects.Unlock(40);
      }
      else
      {
         if(!V3_ItemInfoDlg::GetInstance()->IsShown() )
         {
            V3_ItemInfoDlg::GetInstance()->Display(NULL);
            RequestForegroundControl( V3_ItemInfoDlg::GetInstance() );
         }
      }
   }
   else
   {
      if(g_Global.IsItemInfoAvailable())
         g_Global.ResetItemInfo();
      if(V3_ItemInfoDlg::GetInstance()->IsShown())
         V3_ItemInfoDlg::GetInstance()->Hide();

   }
 

   // If there is a foreground child.
   if( foregroundChild != NULL )
   {
      // Draw it
      foregroundChild->Draw( NULL );
   }

   // Rafraichissement inventaire differe (ViewBackpack2 depuis le thread reseau, ex. mort).
   if (g_UiInit && g_pendingInventoryRefresh)
   {
      g_pendingInventoryRefresh = false;
      V3_InvDlg::GetInstance()->UpdateInventory( &Player.tlBackpack );
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void RootBoxUI::DrawHelp
//////////////////////////////////////////////////////////////////////////////////////////
//  Draws the help texts
// 
(
 V2SPRITEFX *vsfFX
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );
    
    GameUI::DrawHelp( vsfFX );
}


//////////////////////////////////////////////////////////////////////////////////////////
void RootBoxUI::AddChild
//////////////////////////////////////////////////////////////////////////////////////////
//  Children adding, thread safe.
// 
(
 GameUI *child
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );

    GameUI::AddChild( child );
}

//////////////////////////////////////////////////////////////////////////////////////////
void RootBoxUI::RemoveChild
//////////////////////////////////////////////////////////////////////////////////////////
//  Children removing. Thread safe.
// 
(
 GameUI *child
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );

    GameUI::RemoveChild( child );
}
//////////////////////////////////////////////////////////////////////////////////////////
void RootBoxUI::DeleteChild
//////////////////////////////////////////////////////////////////////////////////////////
//  Children deletion. Thread safe.
// 
(
 GameUI *child
)
//////////////////////////////////////////////////////////////////////////////////////////
{
     CAutoLock autoLock( &threadLock );

     GameUI::DeleteChild( child );
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RootBoxUI::IsMouseOwned
//////////////////////////////////////////////////////////////////////////////////////////
// Determines if the mouse is owned by the UI.
// 
(
 MousePos mousePos // The mouse pos.
)
// Return: bool, true if the mouse is owned by the UI.
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );

    if( mouseOwned ){
        return true;
    }

  
    // If a child box was hit by the mouse position.
    if( GetHitChild( mousePos ) != NULL ){
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RootBoxUI::IsGUIMaximized( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Determines whether any UIs are maximized.
// 
// Return: bool, true if any box is maximized.
//////////////////////////////////////////////////////////////////////////////////////////
{
    CAutoLock autoLock( &threadLock );
    
    // Check all children to query those that need to be drawn.
    ChildQueue::iterator i;
    for( i = GetChildList()->begin(); i != GetChildList()->end(); i++ ){
        if( (*i)->IsShown() &&
            (*i) != foregroundChild
         ){
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
MousePos RootBoxUI::GetRootScreenOffset( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the screen offset.
//////////////////////////////////////////////////////////////////////////////////////////
{
    MousePos offset( 0, 0 );
    
    return offset;
}
