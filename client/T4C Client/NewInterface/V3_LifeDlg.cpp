// V3_LifeDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_LifeDlg.h"

// Guard anti-deadlock (cf. V3_StatsDlg / V2PalManager) : GetInstance() depuis le thread de rendu
// pendant que le thread reseau construit l'instance.
static V3_LifeDlg* g_pLifeDlgInstance = NULL;

static bool  s_pendingDeadStatus     = false;
static BYTE  s_pendingDeadStatusVal  = 0;
static bool  s_pendingDeadInfo       = false;
static WORD  s_pendingDeadCur        = 0;
static WORD  s_pendingDeadMax        = 0;
static BYTE  s_pendingDeadCanResurect = 0;

static void ApplyPendingDeathUI(V3_LifeDlg *dlg)
{
   if (s_pendingDeadStatus)
   {
      s_pendingDeadStatus = false;
      dlg->UpdateDeadStatus(s_pendingDeadStatusVal, true);
   }
   if (s_pendingDeadInfo)
   {
      s_pendingDeadInfo = false;
      dlg->UpdateDeadInfo(s_pendingDeadCur, s_pendingDeadMax, s_pendingDeadCanResurect);
   }
}
#include "RootBoxUI.h"
#include "V3_StatDlg.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"
#include "..\SaveGame.h"
extern CSaveGame g_SaveGame;

extern Font *fSystem_22;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_LifeDlg::V3_LifeDlg():
Drag_Button(dragEvent),
m_DeathBtnRestore( m_DeathBtnRestoreEvent,"Resurrect",g_DefColorH ),
m_invBtnPV(EmptyEvent::GetInstance()),
m_invBtnPM(EmptyEvent::GetInstance())
{
   g_pLifeDlgInstance = this;

   int i=0;

   m_dwDeadStatus = 0;

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastLifeXPos();
   m_iYPos = g_Global.GetDialogLastLifeYPos();
   
 
   m_Background .SetSpriteId("V3_LifeBackF",true,"V3_LifeBackFMask");
   m_BackgroundD.SetSpriteId("V3_LifeBackFD",true,"V3_LifeBackFDMask");
   m_PVBar.SetSpriteId("V3_PVBar");
   m_PMBar.SetSpriteId("V3_PMBar");
   m_DTBar.SetSpriteId("V3_DeathBar");

   m_DeathImage.SetSpriteId("V3_lifeDeadImage");

   char strTmp[10];
   for(i=0;i<iNBR_SKIN_IMAGE_MAX;i++)
   {
      sprintf_s(strTmp,10,"%04d",i);
      m_TargetImg[i].SetSpriteId(strTmp);
   }

   
   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this ); 

   m_DeathBtnRestore.SetSpriteId   ( "V3_Btn72x16_N" );
   m_DeathBtnRestore.SetDownUI     ( "V3_Btn72x16_D" );
   m_DeathBtnRestore.SetHighlightUI( "V3_Btn72x16_H" ); 
   dragEvent.SetUI    ( this ); 
   
   
  
   m_stPV.SetText(g_GUILocalString[5],g_DefColorD,g_Global.GetScreenW(),0,false,2);
   m_stPM.SetText(g_GUILocalString[6],g_DefColorD,g_Global.GetScreenW(),0,false,2);
   

   m_invBtnPV.SetHelpText( FontManager::GetInstance()->GetSmallFont() );
   m_invBtnPM.SetHelpText( FontManager::GetInstance()->GetSmallFont() );
 

   m_stAttack.SetSpriteId("V3_AttackStatic",true,"V3_AttackStaticMask");
   m_stXPScroll.SetSpriteId("64kIconScrollXP");
   m_stORScroll.SetSpriteId("64kIconScrollGold");
   m_stRPDisable.SetSpriteId("64kIconXPOff");

   UpdateAllFramePos(m_iXPos,m_iYPos);

   AddChild( &m_Background);
   AddChild( &m_BackgroundD);
   AddChild( &m_stPV);
   AddChild( &m_stPM);
   AddChild( &m_stLv);
   AddChild( &m_stNom);
   AddChild( &Drag_Button);
   AddChild( &m_invBtnPV);
   AddChild( &m_invBtnPM);
   AddChild( &m_DeathBtnRestore);
   

   m_DTBar.Hide();
   m_DeathBtnRestore.Hide();
   m_DeathBtnRestore.Hide();
   m_BackgroundD.Hide();
  
   RootBoxUI::GetInstance()->AddChild( this );
} 

V3_LifeDlg::~V3_LifeDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_LifeDlg *V3_LifeDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{
    if (g_pLifeDlgInstance)
       return g_pLifeDlgInstance;

    static V3_LifeDlg instance;
    return &instance;
    
}

void V3_LifeDlg::QueueDeadStatus(BYTE chStatus)
{
   s_pendingDeadStatusVal = chStatus;
   s_pendingDeadStatus    = true;
}

void V3_LifeDlg::QueueDeadInfo(WORD wTimeCurrent, WORD wTimeTotal, BYTE chCanResurect)
{
   s_pendingDeadCur         = wTimeCurrent;
   s_pendingDeadMax         = wTimeTotal;
   s_pendingDeadCanResurect = chCanResurect;
   s_pendingDeadInfo        = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_LifeDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iXPos = g_Global.GetDialogLastLifeXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastLifeYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->Show();

   return 0;
}

void V3_LifeDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_LifeDlgW, iPosY+g_LifeDlgH ) );
   m_Background .SetPos( FramePos( iPosX, iPosY, iPosX+g_LifeDlgW, iPosY+g_LifeDlgH ) );
   m_BackgroundD.SetPos( FramePos( iPosX, iPosY, iPosX+g_LifeDlgW, iPosY+g_LifeDlgH ) );
   Drag_Button .SetPos( FramePos( iPosX+149, iPosY, iPosX+g_LifeDlgW, iPosY+g_LifeDlgH ) );
   m_stPV      .SetPos( FramePos( iPosX+30, iPosY+33,iPosX+45, iPosY+40) );
   m_stPM      .SetPos( FramePos( iPosX+30, iPosY+50,iPosX+45, iPosY+57) );
   m_PVBar     .SetPos( FramePos( iPosX+46, iPosY+30,iPosX+146, iPosY+43) );
   m_PMBar     .SetPos( FramePos( iPosX+46, iPosY+47,iPosX+146, iPosY+60) );
   m_DTBar     .SetPos( FramePos( iPosX+46, iPosY+30,iPosX+146, iPosY+43) );
   m_stLv      .SetPos( FramePos( iPosX+162,iPosY+62,iPosX+200, iPosY+71) );
   m_stNom     .SetPos( FramePos( iPosX+38 ,iPosY+8 ,iPosX+151, iPosY+19) );
   m_invBtnPV  .SetPos( FramePos( iPosX+46, iPosY+30,iPosX+149, iPosY+43) );
   m_invBtnPM  .SetPos( FramePos( iPosX+46, iPosY+47,iPosX+149, iPosY+60) );

   m_DeathImage.SetPos( FramePos( iPosX+163,iPosY+5 ,iPosX+198, iPosY+59) );
   m_DeathBtnRestore.SetPos( FramePos( iPosX+58,iPosY+47 ,iPosX+58+72, iPosY+47+16) );

   for(int i=0;i<iNBR_SKIN_IMAGE_MAX;i++)
      m_TargetImg[i].SetPos( FramePos( iPosX+154,iPosY+7 ,iPosX+189, iPosY+61) );
   
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_LifeDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   ApplyPendingDeathUI(this);

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
      
      if(iNewPosX+g_LifeDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_LifeDlgW;
      if(iNewPosY+g_LifeDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_LifeDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }


   char strTmp[255];
   sprintf_s(strTmp,255,"%d",Player.Level);
   m_stLv.SetText(strTmp,g_DefColorD,g_Global.GetScreenW(),0,false,true);

   sprintf_s(strTmp,255,"%s",Player.Name);
   m_stNom.SetText(strTmp,g_DefColorD,g_Global.GetScreenW(),0,false,true);

   GameUI::Draw( vsfFX );
 
   

   if(m_dwDeadStatus == 0)
   {
      //Dessine els bar pard essus le back...
      //PV Bar
      int percent = Player.MaxHp == 0 ? 0 : Player.Hp * 100 / Player.MaxHp;
      DrawSingleStatusItem( m_PVBar, percent );
      //PM Bar
      percent = Player.MaxMana == 0 ? 0 : Player.Mana * 100 / Player.MaxMana;
      DrawSingleStatusItem( m_PMBar, percent );

      
      int iType3D = Objects.GetMainObject()->Type3D;
      if(Objects.Set == 71)
      {
         Objects.Lock(0);
         RECT rcClip;
         rcClip.left   = iOffX+153;
         rcClip.top    = 0;//iOffY;
         rcClip.right  = g_Global.GetScreenW();//iOffX+g_LifeDlgW;
         rcClip.bottom = iOffY+60;
         Objects.DrawPuppetClip( iOffX+158, iOffY+80,1 ,&rcClip,Objects.GetMainObject());
         Objects.Unlock(0);
      }
      else
      {
         if(Objects.Set >=0 || Objects.Set < iNBR_SKIN_IMAGE_MAX)
         {
            V2SPRITEFX fx;
            memset( &fx, 0, sizeof( fx ) );
            fx.dwFX |= FX_LEFTRIGHTMIRROR;

            m_TargetImg[Objects.Set].Draw(&fx);
         }
      }
   }
   else
   {
      int percent = m_dwDeadMax == 0 ? 0 : m_dwDeadCur * 100 / m_dwDeadMax;
      DrawSingleStatusItem( m_DTBar, percent );

      m_DeathImage.Draw(vsfFX);

      if(m_Background.IsShown())
      {
         UpdateDeadStatus(1,false);
      }
   }

   int iIconOffsetX = 140;
   if (g_Var.wRPStatus > 0) 
   {
      FramePos HAttackP   = FramePos(iOffX+iIconOffsetX,iOffY+69,iOffX+iIconOffsetX+14,iOffY+69+19);
      m_stRPDisable.SetPos( HAttackP );
      m_stRPDisable.Draw( vsfFX );
      
      iIconOffsetX-=30;
   }

   //draw epee mode combar
   if (Player.AttackMode > 0) 
   {
      FramePos HAttackP   = FramePos(iOffX+iIconOffsetX,iOffY+69,iOffX+iIconOffsetX+14,iOffY+69+19);
      m_stAttack.SetPos( HAttackP );
      m_stAttack.Draw( vsfFX );

      iIconOffsetX-=30;
   }
   //draw scroll XP...
   if(g_Var.wDisplayXPScroll >0)
   {
      FramePos HXPScrollP   = FramePos(iOffX+iIconOffsetX,iOffY+69,iOffX+iIconOffsetX+24,iOffY+69+29);
      m_stXPScroll.SetPos( HXPScrollP );
      m_stXPScroll.Draw( vsfFX );
      
      //CV: Scroll Times....
      HDC hdc;
      DXDGetDC(&hdc, 24);
      char Buf[100];
      
      sprintf_s(Buf,100, "%d",g_Var.wDisplayXPScrollT); 
      fSystem_22->DrawFont(hdc, iOffX+iIconOffsetX+1, iOffY+69-2, CL_BLACK , 0, Buf);
      fSystem_22->DrawFont(hdc, iOffX+iIconOffsetX+3, iOffY+69-0, CL_BLACK , 0, Buf);
      fSystem_22->DrawFont(hdc, iOffX+iIconOffsetX+2, iOffY+69-1, CL_GREEN, 0, Buf);
      
      DXDReleaseDC(hdc, 24);
      
      int iMax = 160;
      if(g_SaveGame.GetOptionsParam()->bPlayAt32FPS)
         iMax = 320;
      if(++g_Var.wDisplayXPScroll >iMax) //max 10 secondes
         g_Var.wDisplayXPScroll = 0;

      iIconOffsetX-=30;
   }

   if(g_Var.wDisplayORScroll >0)
   {
      FramePos HORScrollP   = FramePos(iOffX+iIconOffsetX,iOffY+69,iOffX+iIconOffsetX+24,iOffY+69+29);
      m_stORScroll.SetPos( HORScrollP );
      m_stORScroll.Draw( vsfFX );
      
      //CV: Scroll Times....
      HDC hdc;
      DXDGetDC(&hdc, 24);
      char Buf[100];
      
      sprintf_s(Buf,100, "%d",g_Var.wDisplayORScrollT); 
      fSystem_22->DrawFont(hdc, iOffX+iIconOffsetX+1, iOffY+69-2, CL_BLACK , 0, Buf);
      fSystem_22->DrawFont(hdc, iOffX+iIconOffsetX+3, iOffY+69-0, CL_BLACK , 0, Buf);
      fSystem_22->DrawFont(hdc, iOffX+iIconOffsetX+2, iOffY+69-1, CL_GREEN, 0, Buf);
      
      DXDReleaseDC(hdc, 24);
      int iMax = 160;
      if(g_SaveGame.GetOptionsParam()->bPlayAt32FPS)
         iMax = 320;

      if(++g_Var.wDisplayORScroll >iMax) //max 10 secondes
         g_Var.wDisplayORScroll = 0;

       iIconOffsetX-=30;
   }



   if(m_DeathBtnRestore.IsShown())
   {
      HDC hdc;
      DXDGetDC(&hdc, 86);
      m_DeathBtnRestore.DrawTextButtonExt(hdc);
      DXDReleaseDC(hdc, 86);
   }
   

   
}


void V3_LifeDlg::Hide()
{ 
   BoxUI::Hide();
}



void V3_LifeDlg::StartDragDlg( MousePos mousePos )
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

void V3_LifeDlg::StopDragDlg()
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
      
      if(m_iXPos+g_LifeDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_LifeDlgW;
      if(m_iYPos+g_LifeDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_LifeDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastLifeXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      
      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_LifeDlg::DragEvent::LeftClicked( void )
{
   V3_LifeDlg *me = static_cast< V3_LifeDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_LifeDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_LifeDlg *me = static_cast< V3_LifeDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}



//  Draws a single item
void V3_LifeDlg::DrawSingleStatusItem(GraphUI &fullBar,int percent)
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



void V3_LifeDlg::UpdateDeadStatus(BYTE chStatus,bool bProcessHide)
{
   m_dwDeadStatus   = chStatus;
   m_dwDeadCur      = 0;
   m_dwDeadMax      = 0;
   m_dwCanRestore   = 0;

   if(bProcessHide)
      m_DeathBtnRestore.Hide();
   
   if(m_dwDeadStatus == 1)
   {
      m_PVBar      .Hide();
      m_PMBar      .Hide();
      m_stPV       .Hide();
      m_stPM       .Hide();
      m_Background .Hide();

      m_DTBar      .Show();
      m_BackgroundD.Show();
   }
   else
   {
      m_PVBar      .Show();
      m_PMBar      .Show();
      m_stPV       .Show();
      m_stPM       .Show();
      m_Background .Show();

      m_DTBar      .Hide();
      m_BackgroundD.Hide();
   }
   
}

void V3_LifeDlg::UpdateDeadInfo(WORD wTimeCurrent,WORD wTimeTotal,BYTE chCanResurect)
{
   if(m_dwDeadStatus == 0)
   {
      m_dwDeadStatus = 1;
   }

   m_dwDeadCur      = wTimeCurrent;
   m_dwDeadMax      = wTimeTotal;
   m_dwCanRestore   = chCanResurect;

   if(m_dwCanRestore && !m_DeathBtnRestore.IsShown())
   {
      m_DeathBtnRestore.Show();
   }
   else if(!m_dwCanRestore && m_DeathBtnRestore.IsShown())
   {
      m_DeathBtnRestore.Hide();
   }
   
   
}


void V3_LifeDlg::DeathRestoreEvent::LeftClicked( void )
{
   V3_LifeDlg *me = static_cast< V3_LifeDlg * >( GetUI() );    
   
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_NM_DeathResurect;
   SEND_PACKET( packet );
}


bool V3_LifeDlg::RightMouseUp( MousePos mousePos )
{
  char strTmp[255] ;
  if(m_PVBar.GetPos().MousePosInFrame(mousePos))
  {
      sprintf_s(strTmp,"%d/%d",Player.Hp ,Player.MaxHp );
      m_invBtnPV.GetHelpText()->SetText( strTmp,g_DefColorD );
      if( IsClick() )
         m_invBtnPV.DisplayHelp( true, mousePos.x, mousePos.y );
  }
  if(m_PMBar.GetPos().MousePosInFrame(mousePos))
  {
     
     sprintf_s(strTmp,255,"%d/%d",Player.Mana ,Player.MaxMana );
     m_invBtnPV.GetHelpText()->SetText( strTmp,g_DefColorD );
     if( IsClick() )
       m_invBtnPV.DisplayHelp( true, mousePos.x, mousePos.y );
  }
   return true;
}