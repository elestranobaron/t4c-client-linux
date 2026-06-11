// V3_GroupOSDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_GroupOSDlg.h"
#include "RootBoxUI.h"
#include "V3_StatDlg.h"
#include "V3_GroupeDlg.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"


extern CSaveGame g_SaveGame;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_GroupOSDlg::V3_GroupOSDlg():
Drag_Button(dragEvent)
{
   int i=0;

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogGroupOSXPos();
   m_iYPos = g_Global.GetDialogGroupOSYPos();
   
 
   BackGround.SetSpriteId("V3_GroupDisplay2Back",true,"V3_GroupDisplay2BackMask");
   
   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this ); 
   
  
   fullBar.SetSpriteId( "V3_PVBar" );
 

   UpdateAllFramePos(m_iXPos,m_iYPos);

   AddChild( &Drag_Button);
   
  
   RootBoxUI::GetInstance()->AddChild( this );
} 

V3_GroupOSDlg::~V3_GroupOSDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_GroupOSDlg *V3_GroupOSDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_GroupOSDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_GroupOSDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   GetInstance()->m_iXPos = g_Global.GetDialogGroupOSXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogGroupOSYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);
   GetInstance()->Show();

   return 0;
}

void V3_GroupOSDlg::UpdateAllFramePos(int iPosX,int iPosY)
{

   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   // Get the list's grid
   std::vector< std::vector< ListUI::GridItem > > &grid = *V3_GroupeDlg::GetInstance()->m_ListMember.GetGrid();
   int iSize = grid[0].size();
   if(iSize >1)
      iSize--;



   SetPos( FramePos( iPosX, iPosY, iPosX+g_GroupOSDlgW, iPosY+(iSize*g_GroupOSDlgH) ) );
   Drag_Button .SetPos( GetPos() );


}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_GroupOSDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   if(!g_SaveGame.GetOptionsParam()->bDisplayGroupInfo)
   {
      Hide();
      return;
   }

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
      
      if(iNewPosX+g_GroupOSDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_GroupOSDlgW;
      if(iNewPosY+g_GroupOSDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_GroupOSDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }
   else
   {
      UpdateAllFramePos(iOffX,m_iYPos);
   }

   //GameUI::Draw( vsfFX );

   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   std::vector< std::vector< ListUI::GridItem > > &grid = *V3_GroupeDlg::GetInstance()->m_ListMember.GetGrid();

   UINT i;
   int dwNbrGrp = 0;  
   for( i = 0; i < grid[0].size(); i++ )
   {
      // Do not display the user's HP twice.        
      TextMemberUI *playerName = static_cast< TextMemberUI * >( grid[ 0 ][ i ] );
      if( _stricmp( playerName->GetText()->GetText(), Player.Name ) == 0 )
      {
         continue;
      }
            
      HpMemberUI *hpUI = static_cast< HpMemberUI * >( grid[ 1 ][ i ] );
      
      int YPos = iOffY+(dwNbrGrp*g_GroupOSDlgH);

      FramePos HPPosB    = FramePos(iOffX,YPos,iOffX+g_GroupOSDlgW,YPos+g_GroupOSDlgH);
      FramePos HPPosPV   = FramePos(iOffX+31,YPos+24,iOffX+130,YPos+37);
      FramePos HPPosPVst = FramePos(iOffX+15,YPos+27,iOffX+28,YPos+35);
      FramePos HPPosPN   = FramePos(iOffX+27,YPos+2,iOffX+135,YPos+15);
      
      BackGround.SetPos( HPPosB );
      fullBar .SetPos( HPPosPV );
      BackGround.Draw( vsfFX );
     
      
      // Create a clipping area according to the group member's hp.
      long newWidth = HPPosPV.Width() * ( 100 - hpUI->hpPercent ) / 100;
      RECT clipRect = { HPPosPV.x, HPPosPV.y, HPPosPV.maxx - newWidth, HPPosPV.maxy };
      V2SPRITEFX fx;
      memset( &fx, 0, sizeof( fx ) );
      fx.lpClipRect = &clipRect;
      fullBar.Draw( &fx );

      m_stPV  .SetText(g_GUILocalString[5],g_DefColorD,g_Global.GetScreenW(),0,false,2);
      m_stPV  .SetPos( HPPosPVst );
      m_stPV  .Draw( vsfFX );

      m_stNom  .SetText(playerName->GetText()->GetText(),g_DefColorD,g_Global.GetScreenW(),0,false,2);
      m_stNom  .SetPos( HPPosPN );
      m_stNom  .Draw( vsfFX );
      dwNbrGrp++;
    }
}


void V3_GroupOSDlg::Hide()
{ 
   BoxUI::Hide();
}

void V3_GroupOSDlg::Show()
{
   UpdateAllFramePos(g_Global.GetDialogGroupOSXPos(),g_Global.GetDialogGroupOSYPos());
   BoxUI::Show();
}




void V3_GroupOSDlg::StartDragDlg( MousePos mousePos )
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

void V3_GroupOSDlg::StopDragDlg()
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
      
      if(m_iXPos+g_GroupOSDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_GroupOSDlgW;
      if(m_iYPos+g_GroupOSDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_GroupOSDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogGroupOSXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_GroupOSDlg::DragEvent::LeftClicked( void )
{
   V3_GroupOSDlg *me = static_cast< V3_GroupOSDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_GroupOSDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_GroupOSDlg *me = static_cast< V3_GroupOSDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}



//  Draws a single item
void V3_GroupOSDlg::DrawSingleStatusItem(GraphUI &fullBar,int percent)
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






