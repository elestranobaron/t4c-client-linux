// V3_MacroDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_MacroDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include <set>

#include "V3_InvDlg.h"
#include "V3_AskValueDlg.h"
#include "V3_StatDlg.h"
#include "V3_OptionsDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_MainBarDlg.h"
#include "V3_SpellDlg.h"
#include "V3_GroupeDlg.h"
#include "V3_GuildDlg.h"
#include "V3_ProfessionDlg.h"
#include "V3_InvDlg.h"

extern CSaveGame g_SaveGame;

DWORD g_dwLastMacroCallMacro = 0;
DWORD g_dwLastMacroCallMacroKey = 0;

namespace
{
   set< MacroEvent * > eventSet;

   T3VSBSound *naSound = NULL;
   
   void InitSound()
   {
      if( naSound == NULL )
      {
         naSound = new T3VSBSound;
         naSound->Create( "General NA sound", TS_MEMORY );
      }
   }
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_MacroDlg::V3_MacroDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
MOnglet01Button(tabStatsEvent            ,g_GUILocalString[309],g_DefColorH ),
MOnglet02Button(tabInvEvent              ,g_GUILocalString[310],g_DefColorH ),
MOnglet03Button(tabSpellEvent            ,g_GUILocalString[311],g_DefColorH ),
MOnglet04Button(tabProfessionEvent       ,g_GUILocalString[312],g_DefColorH ),
MOnglet05Button(tabGuildeEvent           ,g_GUILocalString[313],g_DefColorH ),
MOnglet06Button(tabGroupEvent            ,g_GUILocalString[314],g_DefColorH ),
MOnglet07Button(EmptyEvent::GetInstance(),g_GUILocalString[315],g_DefColorH ),
MOnglet08Button(tabOptionEvent           ,g_GUILocalString[316],g_DefColorH ),
macroList( macroListEvent  ),
m_btnDelMacro (delMacroButtonEvent,g_GUILocalString[324],g_DefColorH),
m_bDisableMacro (false),
m_iCurrentMP (0),
m_btnMPUp (mpUpEvent),
m_btnMPDn (mpDnEvent)

{
   int y,x;
   InitSound();

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_MacroBack",true,"V3_MacroBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_MacroIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
   
  

   MOnglet01Button.SetHighlightUI( "V3_MainOnglet_77p_H");
   MOnglet02Button.SetHighlightUI( "V3_MainOnglet_77p_H");
   MOnglet03Button.SetHighlightUI( "V3_MainOnglet_48p_H");
   MOnglet04Button.SetHighlightUI( "V3_MainOnglet_77p_H");
   MOnglet05Button.SetHighlightUI( "V3_MainOnglet_50p_H");
   MOnglet06Button.SetHighlightUI( "V3_MainOnglet_50p_H");
   MOnglet07Button.SetHighlightUI( "V3_MainOnglet_50p_H");
   MOnglet08Button.SetHighlightUI( "V3_MainOnglet_50p_H");

   tabStatsEvent     .SetUI(this);
   tabInvEvent       .SetUI(this);
   tabSpellEvent     .SetUI(this);
   tabProfessionEvent.SetUI(this);
   tabGuildeEvent    .SetUI(this);
   tabGroupEvent     .SetUI(this);
   //tabMacroEvent     .SetUI(this);
   tabOptionEvent    .SetUI(this);



   macroList.SetupList    ( 3, 38, FramePos( 0, 0, 2, 1 ) );
   macroList.SetColumnSize( 0, 40  );
   macroList.SetColumnSize( 1, 198  );
   macroList.SetColumnSize( 2, 148 );
   macroList.CenterColumn( 0 );
   macroList.CenterColumn( 1 );
   macroList.CenterColumn( 2 );
   macroList.SetScrollUpDownUI( &macroScrollUp );
   macroList.SetScrollDownDownUI( &macroScrollDown );
   SetHelpId( macroList, 378 );

   macroListSelect.SetPos( FramePos( 0, 0, 390, 38 ) );
   macroListSelect.SetSpriteId( "V3_MacroListPatch" );
   macroListSelect.Show();
   macroList.SetSelectionSprite( &macroListSelect);
   macroListEvent.SetUI( this );


   m_strListNom.SetText(g_GUILocalString[34],g_DefColorH);
   m_strListCle.SetText(g_GUILocalString[35],g_DefColorH);
   


   int iIndex = 0;
   for(y=0;y<g_iNbrMacroY;y++)
   {
      for(x=0;x<g_iNbrMacroX;x++)
      {
         m_MacroButton[y][x].iIndex = iIndex;
         iIndex++;
         if(y == 0)
            m_MacroButton[y][x].Show();
         else
            m_MacroButton[y][x].Hide();
      }
   }
  

   m_btnDelMacro.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnDelMacro.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnDelMacro.SetHighlightUI( "V3_Btn92x24_H" );
   delMacroButtonEvent.SetUI( this );
   SetHelpId( m_btnDelMacro, 383 );


   m_btnMPUp.SetSpriteId   ( "V3_SpinUP_N" );
   m_btnMPUp.SetHighlightUI( "V3_SpinUP_H" );

   m_btnMPDn.SetSpriteId   ( "V3_SpinDN_N" );
   m_btnMPDn.SetHighlightUI( "V3_SpinDN_H" );


   

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   
   
   AddChild( &MOnglet01Button );
   AddChild( &MOnglet02Button );
   AddChild( &MOnglet03Button );
   AddChild( &MOnglet04Button );
   AddChild( &MOnglet05Button );
   AddChild( &MOnglet06Button );
   AddChild( &MOnglet07Button );
   AddChild( &MOnglet08Button );
   AddChild( &macroList );

   for(y=0;y<g_iNbrMacroY;y++)
   {
      for(x=0;x<g_iNbrMacroX;x++)
      {
         AddChild( &m_MacroButton[y][x]);
      }
   }
 
   AddChild( &m_btnDelMacro);
   AddChild( &Drag_Button);
   AddChild( &m_strListNom);
   AddChild( &m_strListCle);
   AddChild( &m_strMP );
   AddChild( &m_btnMPUp);
   AddChild( &m_btnMPDn);
   RootBoxUI::GetInstance()->AddChild( this );

   
} 

V3_MacroDlg::~V3_MacroDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_MacroDlg *V3_MacroDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_MacroDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////

unsigned int V3_MacroDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallMacro > 500)
      {
         g_dwLastMacroCallMacro =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}

unsigned int V3_MacroDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   g_Global.SetLastDialogOpen(6);

   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);
   GetInstance()->Show();

   return 0;
}

void V3_MacroDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_OptionDlgW, iPosY+g_OptionDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+520, iPosY+57 ) );

   //les 8 onglet
   MOnglet01Button.SetPos ( FramePos( iPosX+33  , iPosY+63  , iPosX+109, iPosY+83 ) );
   MOnglet02Button.SetPos ( FramePos( iPosX+112 , iPosY+63  , iPosX+188, iPosY+83 ) );
   MOnglet03Button.SetPos ( FramePos( iPosX+191 , iPosY+63  , iPosX+236, iPosY+83 ) );
   MOnglet04Button.SetPos ( FramePos( iPosX+239 , iPosY+63  , iPosX+315, iPosY+83 ) );
   MOnglet05Button.SetPos ( FramePos( iPosX+318 , iPosY+63  , iPosX+367, iPosY+83 ) );
   MOnglet06Button.SetPos ( FramePos( iPosX+370 , iPosY+63  , iPosX+419, iPosY+83 ) );
   MOnglet07Button.SetPos ( FramePos( iPosX+422 , iPosY+63  , iPosX+471, iPosY+83 ) );
   MOnglet08Button.SetPos ( FramePos( iPosX+474 , iPosY+63  , iPosX+523, iPosY+83 ) );


   macroList.SetListDef(iPosX,iPosY,20,39,118,452,350,&macroScrollUp,&macroScrollDown);
   m_strListNom.SetPos ( FramePos( iPosX+78  , iPosY+96  , iPosX+278, iPosY+113 ) );
   m_strListCle.SetPos ( FramePos( iPosX+283 , iPosY+96  , iPosX+428, iPosY+113 ) );
   m_strMP     .SetPos ( FramePos( iPosX+519+2 , iPosY+372 , iPosX+534+2, iPosY+386 ) );

   m_btnMPUp   .SetPos ( FramePos( iPosX+519 , iPosY+361 , iPosX+534, iPosY+371 ) );
   m_btnMPDn   .SetPos ( FramePos( iPosX+519 , iPosY+386 , iPosX+534, iPosY+396 ) );

   m_btnDelMacro.SetPos ( FramePos( iPosX+471 , iPosY+138  , iPosX+471+92, iPosY+138+24 ) );


   int iXS = 87;
   int iXW = 34;
   int iXO = 36;
   
   for(int y=0;y<g_iNbrMacroY;y++)
   {
      for(int x=0;x<g_iNbrMacroX;x++)
      {
         int iXPS = iXS+(x*iXO);
         m_MacroButton[y][x].SetPos( FramePos( iPosX+iXPS, iPosY+362, iPosX+iXPS+iXW, iPosY+394 ) );
      }
   }
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_MacroDlg::Draw(V2SPRITEFX *vsfFX)
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

   

   char strTmp[50];
   sprintf_s(strTmp,50,"%d",m_iCurrentMP+1);
   m_strMP.SetText(strTmp,g_DefColorD,g_Global.GetScreenW(),0,false,2);
   GameUI::Draw( vsfFX );
   
   HDC hdc;
   DXDGetDC(&hdc, 86);

   //dessine tous les texte des bouton...

   MOnglet01Button.DrawTextButtonExt(hdc);
   MOnglet02Button.DrawTextButtonExt(hdc);
   MOnglet03Button.DrawTextButtonExt(hdc);
   MOnglet04Button.DrawTextButtonExt(hdc);
   MOnglet05Button.DrawTextButtonExt(hdc);
   MOnglet06Button.DrawTextButtonExt(hdc);
   MOnglet07Button.DrawTextButtonExt(hdc);
   MOnglet08Button.DrawTextButtonExt(hdc);
   m_btnDelMacro.DrawTextButtonExt(hdc);

   
   // Draw les texte...

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_GUILocalString[16],strlen(g_GUILocalString[16]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}


void V3_MacroDlg::Hide()
{
   BoxUI::Hide();
}




void V3_MacroDlg::StartDragDlg( MousePos mousePos )
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

void V3_MacroDlg::StopDragDlg()
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

//////////////////////////////////////////////////////////////////////////////////////////
void V3_MacroDlg::MacroToSave()
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   //sinon on sync les macro avec la list de save pour les enregistrer...
   typedef vector< vector< ListUI::GridItem > > Grid;
   Grid *gridList = macroList.GetGrid();
   // For each event
   set< MacroEvent * >::iterator z;
   for( z = eventSet.begin(); z != eventSet.end(); z++ )
   {
      MacroEvent *macroEvent = *z;
      list< MacroDataUI * > macroList;
      
      UINT i;
      // Scan the list of macros.
      for( i = 0; i != (*gridList)[0].size(); i++ )
      {
         MacroItemUI *macroItemUI = static_cast< MacroItemUI * >( (*gridList)[ 0 ][ i ] );
         
         // If the macro is owned by this macro event
         if( typeid( *macroItemUI->macroEvent ) == typeid( *macroEvent ) )
         {
            // Add it to the list.
            macroList.push_back( macroItemUI->macroUI );
         }
      }
      // Save all macros owned by this event
      macroEvent->SaveMacro( macroList );
   }
   
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_MacroDlg::ResetMacro( void )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   for(int y=0;y<g_iNbrMacroY;y++)
   {
      for(int x=0;x<g_iNbrMacroX;x++)
      {
         m_MacroButton[y][x].macroItem = NULL;
      }
   }
   
   macroList.DeleteList();
   V3_InvDlg::GetInstance()->SetDefaultMacro();
   V3_MainBarDlg::GetInstance()->UpdateMacroBtn();
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_MacroDlg::ReloadReset( void )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   for(int y=0;y<g_iNbrMacroY;y++)
   {
      for(int x=0;x<g_iNbrMacroX;x++)
      {
         m_MacroButton[y][x].macroItem = NULL;
      }
   }
   macroList.DeleteList();
}

//  Adds a macro to the macro UI.
void V3_MacroDlg::AddMacro(MacroDataUI *macroUI,string macroName,MacroEvent *macroEvent,bool echo)
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   // Search the macro list for the macro.
   vector< vector< ListUI::GridItem > > &grid = *macroList.GetGrid();
   
   // Register this event.
   eventSet.insert( macroEvent );
   
   bool exists = false;
   vector< ListUI::GridItem >::iterator i;
   for( i = grid[0].begin(); i != grid[0].end(); i++ )
   {
      MacroItemUI *macro = static_cast< MacroItemUI * >( *i );
      // If the macro name and the specified event are the same (coming from the same source).
      if( macro->macroName == macroName &&typeid( *macro->macroEvent ) == typeid( *macroEvent ) )
      {
         exists = true;
         break;
      }
   }
   
   char str[ 512 ];
   
   if( exists )
   {
      if( echo )
      {
         naSound->Play( TRUE );
         sprintf_s( str,512, g_LocalString[ 355 ], macroName.c_str() );
         V3_ChatLogDlg::GetInstance()->AddListLogs( "", str, SYSTEM_COLOR, false );
      }
      return;
   }
   
   if( echo )
   {
      sprintf_s( str,512, g_LocalString[ 356 ], macroName.c_str() );
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", str, SYSTEM_COLOR, false );
   }
   
   // Macro item UI's
   MacroItemUI *macroItemUI = new MacroItemUI( macroEvent, macroUI, macroName );
   macroItemUI->SetPos( macroUI->GetPos() );
   
   macroItemUI->SetHelpText( FontManager::GetInstance()->GetSmallFont(macroName.c_str() ) );
   macroList.AddNewItem();
   macroList.AddColumnItem( 0, macroItemUI );
   
   for(int k=0;k<g_iNbrMacro;k++)
   {
      if( macroUI->chGridPos[k] != 0 )
      {
         int y = k/g_iNbrMacroX;
         int x = k%g_iNbrMacroX;

         m_MacroButton[y][x].macroItem = macroItemUI;
      }
   }
    
   
   StaticTextUI *macroText = new StaticTextUI;
   macroText->SetText( macroName.c_str() );
   macroText->AutoTruncate( 190 );
   macroText->SetPos( FramePos( 0, 0, macroText->GetWidth(), macroText->GetHeight() ) );    
   macroList.AddColumnItem( 1, macroText );
  
   
   macroText = new StaticTextUI;
   macroText->SetText( macroUI->vKey.GetName() );
   macroText->SetPos( FramePos( 0, 0, 148, 38 ) );
   macroList.AddColumnItem( 2, macroText );
}


void V3_MacroDlg::RemoveFromGrid( MacroItemUI *macroItem )
{
   for(int y=0;y<g_iNbrMacroY;y++)
   {
      for(int x=0;x<g_iNbrMacroX;x++)
      {
         if(m_MacroButton[y][x].macroItem == macroItem)
         {
            m_MacroButton[y][x].macroItem = NULL;
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
bool V3_MacroDlg::CallMacro(VKey vKey)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   if( m_bDisableMacro)
   {
      return false;
   }
   
   // Search the corresponding macro in the macro list.
   vector< vector< ListUI::GridItem > > &macroGrid = *macroList.GetGrid();
   
   UINT row;
   for( row = 0; row != macroGrid[ 0 ].size(); row++ )
   {
      MacroItemUI *macroItem = static_cast< MacroItemUI * >( macroGrid[ 0 ][ row ] );
      if( macroItem->macroUI->vKey == vKey )
      {
         break;
      }
   }
   
   // if the macroid was found before reaching the end of the list.
   
   if( row != macroGrid[ 0 ].size() )
   {
      if(timeGetTime()-g_dwLastMacroCallMacroKey > 50)
      {
         g_dwLastMacroCallMacroKey =timeGetTime();
         MacroItemUI *macroItem = static_cast< MacroItemUI * >( macroGrid[ 0 ][ row ] );
         // Call the macro visitor
         macroItem->macroEvent->ExecMacro( macroItem->macroUI );
      }
      return true;
   }
   
   return false; 
}

//  Searches the macro database for existing keys.
bool V3_MacroDlg::KeyExists(VKey ivKey)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   // Search the corresponding macro in the macro list.
   vector< vector< ListUI::GridItem > > &macroGrid = *macroList.GetGrid();
   
   UINT row;
   for( row = 0; row != macroGrid[ 0 ].size(); row++ )
   {
      MacroItemUI *macroItem = static_cast< MacroItemUI * >( macroGrid[ 0 ][ row ] );
      
      if( macroItem->macroUI->vKey == ivKey )
      {
         return true;
      }
   }
   
   return false;
}




void V3_MacroDlg::MacroListEvent::LeftDblClicked( void )

{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );
   
   MacroItemUI *macroItem = static_cast< MacroItemUI * >( me->macroList.GetSelectedRowItem( 0 ) );

   if(macroItem->macroUI)
   {

      me->m_dwLastMacroID = macroItem->macroId;
      
      Custom.gMacro.DisableMacroCall();
      me->m_bDisableMacro = true;

      V3_AskValueDlg::GetInstance()->SetDialogText1(true,macroItem->macroUI->vKey,g_GUILocalString[98] ,g_SaveGame.GetOptionsParam()->LogName,100,465);
      V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],467,468,true);
      V3_AskValueDlg::GetInstance()->SetParent(me);
      V3_AskValueDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
      V3_AskValueDlg::GetInstance()->Show(0);
   
      me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );
   }
}

void V3_MacroDlg::AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey)
{
   if(bOK)
   {

      switch(uiShowCodeP)
      {
         case 0:
         {
            // Search the corresponding macro in the macro list.
            vector< vector< ListUI::GridItem > > &macroListP = *macroList.GetGrid();
         
            UINT row;
            for( row = 0; row != macroListP[ 0 ].size(); row++ )
            {
               MacroItemUI *macroItem = static_cast< MacroItemUI * >( macroListP[ 0 ][ row ] );
            
               if( macroItem->macroId == m_dwLastMacroID )
               {
                  break;
               }
            }
         
            // if the macroid was found before reaching the end of the list.
            if( row != macroListP[ 0 ].size() )
            {
               MacroItemUI *macroItem = static_cast< MacroItemUI * >( macroListP[ 0 ][ row ] );
               macroItem->macroUI->vKey = vKey;
            
               StaticTextUI *keyName = static_cast< StaticTextUI * >( macroListP[ 2 ][ row ] );        
               keyName->SetText( pStrValue1 );
            }
         }
         break;
      }
   }
   Custom.gMacro.EnableMacroCall();
   m_bDisableMacro = false;
}


///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_MacroDlg::DragEvent::LeftClicked( void )
{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_MacroDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}





V3_MacroDlg::MacroButtonUI::MacroButtonUI( void )
: macroItem( NULL )
{
}


//////////////////////////////////////////////////////////////////////////////////////////
bool V3_MacroDlg::MacroButtonUI::LeftMouseDown(MousePos mousePos)
{
   V3_MacroDlg *me = V3_MacroDlg::GetInstance();
      
   if( IsDblClick())
   {
      if( macroItem == NULL )
         return true;
      
      macroItem->macroEvent->ExecMacro( macroItem->macroUI );// Call the macro visitor
   }
   return true;
}  

bool V3_MacroDlg::MacroButtonUI::RightMouseDown( MousePos mousePos )
{
   V3_MacroDlg *me = V3_MacroDlg::GetInstance();

   MacroItemUI *macroItem = static_cast< MacroItemUI * >( me->macroList.GetSelectedRowItem( 0 ) );
   if(macroItem)
   {
      if(this->macroItem)
      {
        
         this->macroItem->macroUI->chGridPos[this->iIndex] = 0;
         this->macroItem = NULL;
      }
      else
      {
         this->macroItem = macroItem;
         macroItem->macroUI->chGridPos[this->iIndex] = 1;
      }
   }
   else
   {
      if(this->macroItem)
      {

         this->macroItem->macroUI->chGridPos[this->iIndex] = 0;
         this->macroItem = NULL;
      }
   }

   V3_MainBarDlg::GetInstance()->UpdateMacroBtn();
   return true;
}



void V3_MacroDlg::DelMacroButtonEvent::LeftClicked( void )
{
   V3_MacroDlg *me = V3_MacroDlg::GetInstance();
 
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   
   MacroItemUI *macroItem = static_cast< MacroItemUI *>( me->macroList.GetSelection() );
   
   if( macroItem == NULL )
   {
      naSound->Play( TRUE );
      V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 359 ], SYSTEM_COLOR, false );
      return;
   }
   
   // Try to find the macro in the grid.
   me->RemoveFromGrid( macroItem );
   me->macroList.DeleteSelection();
   V3_MainBarDlg::GetInstance()->UpdateMacroBtn();
}

void V3_MacroDlg::MPUpEvent::LeftClicked( void )
{
   V3_MacroDlg *me = V3_MacroDlg::GetInstance();
   
   if(me->m_iCurrentMP > 0)
   {
      me->m_iCurrentMP--;

      for(int y=0;y<g_iNbrMacroY;y++)
      {
         for(int x=0;x<g_iNbrMacroX;x++)
         {
            if(y == me->m_iCurrentMP)
               me->m_MacroButton[y][x].Show();
            else
               me->m_MacroButton[y][x].Hide();
         }
      }
   }
}

void V3_MacroDlg::MPDnEvent::LeftClicked( void )
{
   V3_MacroDlg *me = V3_MacroDlg::GetInstance();
   
   if(me->m_iCurrentMP < (g_iNbrMacroY-1))
   {
      me->m_iCurrentMP++;
      
      for(int y=0;y<g_iNbrMacroY;y++)
      {
         for(int x=0;x<g_iNbrMacroX;x++)
         {
            if(y == me->m_iCurrentMP)
               me->m_MacroButton[y][x].Show();
            else
               me->m_MacroButton[y][x].Hide();
         }
      }
   }
   
}


//////////////////////////////////////////////////////////////////////////////////////////
MacroItemUI::MacroItemUI(MacroEvent *imacroEvent,MacroDataUI *imacroUI,string imacroName): 
macroUI( imacroUI ), 
macroName( imacroName ), 
macroEvent( imacroEvent )
{
   static int globalMacroId = 1;
   macroId = globalMacroId++;
}

MacroItemUI::~MacroItemUI( void )
{
}










void V3_MacroDlg::TabStatsEvent::LeftClicked( void )
{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );
  
   V3_StatsDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_MacroDlg::TabInvEvent::LeftClicked( void )
{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );
   
   V3_InvDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_MacroDlg::TabSpellEvent::LeftClicked( void )
{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );

   V3_SpellDlg::GetInstance()->Display(NULL);
   me->Hide();
   
}

void V3_MacroDlg::TabProfessionEvent::LeftClicked( void )
{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );
   
   V3_ProfessionDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_MacroDlg::TabGuildeEvent::LeftClicked( void )
{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );
   
   V3_GuildDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_MacroDlg::TabGroupEvent::LeftClicked( void )
{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );
   
   V3_GroupeDlg::GetInstance()->Display(NULL);
   me->Hide();
   
}

/*
// void V3_MacroDlg::TabMacroEvent::LeftClicked( void )
// {
//    V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );
//    
// }
*/

void V3_MacroDlg::TabOptionEvent::LeftClicked( void )
{
   V3_MacroDlg *me = static_cast< V3_MacroDlg * >( GetUI() );
   
   V3_OptionsDlg::GetInstance()->Display(NULL);
   me->Hide();
}


