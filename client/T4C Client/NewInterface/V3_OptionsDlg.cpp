// V3_OptionsDlg.cpp:
//

#include "../pch.h"

#include "V3_OptionsDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"

#include "V3_StatDlg.h"
#include "V3_MacroDlg.h"
#include "V3_SpellDlg.h"
#include "V3_GroupeDlg.h"
#include "V3_TMIDlg.h"
#include "V3_EffectStatusDlg.h"
#include "V3_GroupOSDlg.h"
#include "V3_GuildDlg.h"
#include "V3_ProfessionDlg.h"
#include "V3_InvDlg.h"
#include "V3_Mp3Dlg.h"

#include "V3_ChatLogDlg.h"
#include "V3_BoutiqueDlg.h"
#include "V3_PanierDlg.h"
#include "V3_MainBarDlg.h"


extern T3VSBSound SoundFX[250];
extern CSaveGame g_SaveGame;

extern DWORD FreezeID;

#define MAX_BRIGHTNESS 7

DWORD g_dwLastMacroCallOpt = 0;

extern HANDLE  g_hMp3EventDisable;

#define V2IMAGE_GMNMS_DATA       "\\Game Files\\gmnmb.bin"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_OptionsDlg::V3_OptionsDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
MOnglet01Button(tabStatsEvent            ,g_GUILocalString[309],g_DefColorH ),
MOnglet02Button(tabInvEvent              ,g_GUILocalString[310],g_DefColorH ),
MOnglet03Button(tabSpellEvent            ,g_GUILocalString[311],g_DefColorH ),
MOnglet04Button(tabProfessionEvent       ,g_GUILocalString[312],g_DefColorH ),
MOnglet05Button(tabGuildeEvent           ,g_GUILocalString[313],g_DefColorH ),
MOnglet06Button(tabGroupEvent            ,g_GUILocalString[314],g_DefColorH ),
MOnglet07Button(tabMacroEvent            ,g_GUILocalString[315],g_DefColorH ),
MOnglet08Button(EmptyEvent::GetInstance(),g_GUILocalString[316],g_DefColorH ),
m_sldMusic ( musicVolumeEvent ),
m_sldEffect( effectEvent      ),
m_sldAmbiant( ambiantVolumeEvent ),
m_sldBright( brightnessEvent  ),
optionsList( optionListEvent  ),
reloadList ( reloadListEvent  ),
m_btnCDMusicE( cdMusicEvent ),
m_btnCDMusicD( cdMusicEvent ),
m_btnLogFileName( fileNameEvent ),
//m_btnQuitter( quitButtonEvent ,g_GUILocalString[66],g_DefColorH ),
m_btnResetGUI( resetGUIButtonEvent ,g_GUILocalString[347],g_DefColorH ),
m_btnLowCPU( lowCpuButtonEvent ,g_LocalString[580],g_DefColorH),
m_btnDefault( defaultButtonEvent ,g_LocalString[740],g_DefColorH),
m_btnReload( reloadButtonEvent ,g_LocalString[599],g_DefColorH),
m_btnNMSGold( nmsGOLDButtonEvent ,g_LocalString[655],g_DefColorH),
m_btnPanier( nmsPanierButtonEvent ,g_LocalString[669],g_DefColorH)

{
   int i=0;

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_OptionBack",true,"V3_OptionBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_OptionIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );

   // Setup the Options list.
   
   optionsList.SetupList    ( 2, 20, FramePos( 0, 0, 0, 2 ) );
   optionsList.SetColumnSize( 0, 220  );
   optionsList.SetColumnSize( 1, 18  );
   optionsList.CenterColumn( 0 );
   optionsList.CenterColumn( 1 );
   optionsList.SetScrollUpDownUI( &optionsScrollUp );
   optionsList.SetScrollDownDownUI( &optionsScrollDown );
   
   optionsListSelect.SetPos( FramePos( 0, 0, 238, 20 ) );
   optionsListSelect.SetSpriteId( "V3_OptionsListSelect" );
   optionsListSelect.Show();
   optionsList.SetSelectionSprite( &optionsListSelect);
   optionListEvent.SetUI( this );

   reloadList.SetupList    ( 1, 20, FramePos( 0, 0, 0, 2 ) );
   reloadList.SetColumnSize( 0, 197  );
   reloadList.CenterColumn( 0 );
   reloadList.SetScrollUpDownUI( &reloadScrollUp );
   reloadList.SetScrollDownDownUI( &reloadScrollDown );
   
   reloadListSelect.SetPos( FramePos( 0, 0, 197, 20 ) );
   reloadListSelect.SetSpriteId( "V3_OptionsReloadListSelect" );
   reloadListSelect.Show();
   reloadList.SetSelectionSprite( &reloadListSelect);
   reloadListEvent.SetUI( this );

   m_stHParam     .SetText( g_GUILocalString[319] ,g_DefColorH);
   m_stHSon       .SetText( g_GUILocalString[42 ] ,g_DefColorH);
   m_stHReload    .SetText( g_LocalString[598]    ,g_DefColorH);
   m_stLuminosite .SetText( g_GUILocalString[65]  ,g_DefColorH);
   m_stLogFileName.SetText( g_GUILocalString[51]  ,g_DefColorH);
   m_stMusic      .SetText( g_GUILocalString[44]  ,g_DefColorD);
   m_stEffect     .SetText( g_GUILocalString[45]  ,g_DefColorD);
   m_stAmbiant    .SetText( g_GUILocalString[372] ,g_DefColorD);
   m_stCDMusic    .SetText( g_GUILocalString[46]  ,g_DefColorD);

   m_stMusic      .SetCenter(false);
   m_stEffect     .SetCenter(false);
   m_stAmbiant    .SetCenter(false);
   m_stCDMusic    .SetCenter(false);
   m_stLuminosite .SetCenter(false);
   m_stLogFileName.SetCenter(false);
   
  

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
   tabMacroEvent     .SetUI(this);
   //tabOptionEvent    .SetUI(this);


   //Option Sliders
   musicVolumeEvent.SetUI( this );
   SetHelpId( m_sldMusic, 388 );
   
   effectEvent.SetUI( this );
   SetHelpId( m_sldEffect, 389 );

   ambiantVolumeEvent.SetUI( this );
   SetHelpId( m_sldAmbiant, 747 );
   
   
   
   brightnessEvent.SetUI( this );
   SetHelpId( m_sldBright, 390 );


   m_btnCDMusicE.SetSpriteId   ( "V3_Check18_ON_N" );
   m_btnCDMusicE.SetDownUI     ( "V3_Check18_ON_N" );
   m_btnCDMusicE.SetHighlightUI( "V3_Check18_ON_H" );
   cdMusicEvent.SetUI( this );
   SetHelpId( m_btnCDMusicE, 391 );
   
   m_btnCDMusicD.SetSpriteId    ( "V3_Check18_OFF_N" );
   m_btnCDMusicD.SetDownUI      ( "V3_Check18_OFF_N" );
   m_btnCDMusicD.SetHighlightUI ( "V3_Check18_OFF_H" );
   SetHelpId( m_btnCDMusicD, 391 );

   SetHelpId( m_stLogFileNameD, 474 );

   
   fileNameEvent.SetUI( this );


   /*
   m_btnQuitter.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnQuitter.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnQuitter.SetHighlightUI( "V3_Btn72x24_H" );
   quitButtonEvent.SetUI( this );
   SetHelpId( m_btnQuitter, 392 );
   */

   
   m_btnResetGUI.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnResetGUI.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnResetGUI.SetHighlightUI( "V3_Btn92x24_H" );
   resetGUIButtonEvent.SetUI( this );

   
   m_btnLowCPU.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnLowCPU.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnLowCPU.SetHighlightUI( "V3_Btn72x24_H" );
   lowCpuButtonEvent.SetUI( this );
   SetHelpId( m_btnLowCPU, 579 );

   m_btnDefault.SetSpriteId   ( "V3_Btn72x24_N" );
   m_btnDefault.SetDownUI     ( "V3_Btn72x24_D" );
   m_btnDefault.SetHighlightUI( "V3_Btn72x24_H" );
   defaultButtonEvent.SetUI( this );
   SetHelpId( m_btnDefault, 741 );

  
   //reload
   
   m_btnReload.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnReload.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnReload.SetHighlightUI( "V3_Btn92x24_H" );
   reloadButtonEvent.SetUI( this );
   SetHelpId( m_btnReload, 597 );
    
   //nmsGold
   
   m_btnNMSGold.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnNMSGold.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnNMSGold.SetHighlightUI( "V3_Btn92x24_H" );
   nmsGOLDButtonEvent.SetUI( this );
   SetHelpId( m_btnNMSGold, 654 );
   
   //nmspanier
   m_btnPanier.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnPanier.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnPanier.SetHighlightUI( "V3_Btn92x24_H" );
   nmsPanierButtonEvent.SetUI( this );
   SetHelpId( m_btnPanier, 670 );

  
   m_btnReload.Hide();

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

   AddChild( &Drag_Button);
   AddChild( &optionsList);
   AddChild( &reloadList);
   AddChild( &m_stHParam);
   AddChild( &m_stHSon);
   AddChild( &m_stHReload);
   AddChild( &m_stLuminosite);
   AddChild( &m_stLogFileName);
   AddChild( &m_stLogFileNameD);
   AddChild( &m_stMusic);
   AddChild( &m_stEffect);
   AddChild( &m_stAmbiant);
   AddChild( &m_stCDMusic);

   AddChild( &m_sldMusic  );
   AddChild( &m_sldEffect );
   AddChild( &m_sldAmbiant);
   AddChild( &m_sldBright );

   AddChild( &m_btnCDMusicE );
   AddChild( &m_btnCDMusicD );
   AddChild( &m_btnLogFileName );

   //AddChild( &m_btnQuitter);
   AddChild( &m_btnResetGUI);
   AddChild( &m_btnLowCPU);
   AddChild( &m_btnDefault);
   AddChild( &m_btnReload);

   AddChild( &m_btnNMSGold);
   AddChild( &m_btnPanier);
   

   m_bNMSDisplay = false;
   char strPathFile[1024];
   ClientConfig *pConfig = g_Global.GetClientConfig();
   sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGE_GMNMS_DATA);
   FILE *pfGMD = NULL;
   fopen_s(&pfGMD,strPathFile,"rb");
   if(pfGMD)
   {
      fclose(pfGMD);
      m_bNMSDisplay = true;
   }


   if(g_SaveGame.GetOptionsParam()->dwBrightnessNew > MAX_BRIGHTNESS)
      g_SaveGame.GetOptionsParam()->dwBrightnessNew = MAX_BRIGHTNESS;
   m_sldBright  .SetSliderPos(  g_SaveGame.GetOptionsParam()->dwBrightnessNew );

   m_sldEffect  .SetSliderPos(  g_SaveGame.GetOptionsParam()->dwSoundVol );
   m_sldMusic   .SetSliderPos(  g_SaveGame.GetOptionsParam()->dwMusicVol );
   m_sldAmbiant .SetSliderPos(  g_SaveGame.GetOptionsParam()->dwAmbiantVol );

   if( g_SaveGame.GetOptionsParam()->bUseCD )
   {
	   m_btnCDMusicE.Show();
	   m_btnCDMusicD.Hide();
   }
   else
   {
	   m_btnCDMusicE.Hide();
	   m_btnCDMusicD.Show();
	   
   }
  
   RootBoxUI::GetInstance()->AddChild( this );

   UpdateOptionsList(false);
   m_stLogFileNameD.SetText( g_SaveGame.GetOptionsParam()->LogName,g_DefColorD );


  

} 

V3_OptionsDlg::~V3_OptionsDlg()
{
}

//  Returns the side menu instance.
V3_OptionsDlg *V3_OptionsDlg::GetInstance( void )
{

    static V3_OptionsDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////

unsigned int V3_OptionsDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallOpt > 500)
      {
         g_dwLastMacroCallOpt =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}
unsigned int V3_OptionsDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   g_Global.SetLastDialogOpen(7);

   if(GetInstance()->m_bNMSDisplay)
   {
      GetInstance()->m_btnNMSGold.Show();
      GetInstance()->m_btnPanier.Show();
   }
   else
   {
      GetInstance()->m_btnNMSGold.Hide();
      GetInstance()->m_btnPanier.Hide();
   }
   
   GetInstance()->UpdatePlayerList();
   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);
   GetInstance()->Show();

   return 0;
}

void V3_OptionsDlg::UpdateAllFramePos(int iPosX,int iPosY)
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


   optionsList.SetListDef(iPosX,iPosY,20,295,129,556,324,&optionsScrollUp,&optionsScrollDown);
   reloadList .SetListDef(iPosX,iPosY,20,50 ,252,270,359,&reloadScrollUp ,&reloadScrollDown); 

   

   m_sldBright .SetSliderDef(1,MAX_BRIGHTNESS ,iPosX,iPosY,20,414,348,556,367);
   m_sldMusic  .SetSliderDef(0,20,iPosX,iPosY,20,165,129,269,148);
   m_sldEffect .SetSliderDef(0,20,iPosX,iPosY,20,165,151,269,170);
   m_sldAmbiant.SetSliderDef(0,20,iPosX,iPosY,20,165,173,269,192);

   

 
   m_stHParam     .SetPos ( FramePos( iPosX+296  , iPosY+105  , iPosX+532, iPosY+122) );
   m_stHSon       .SetPos ( FramePos( iPosX+51   , iPosY+105  , iPosX+268, iPosY+122) );
   m_stHReload    .SetPos ( FramePos( iPosX+51   , iPosY+228  , iPosX+245, iPosY+245) );
   m_stLuminosite .SetPos ( FramePos( iPosX+296+3, iPosY+349+2, iPosX+403, iPosY+366) );
   m_stLogFileName.SetPos ( FramePos( iPosX+296+3, iPosY+371+2, iPosX+403, iPosY+388) );
   m_stMusic      .SetPos ( FramePos( iPosX+51+3 , iPosY+130+2, iPosX+158, iPosY+147) );
   m_stEffect     .SetPos ( FramePos( iPosX+51+3 , iPosY+152+2, iPosX+158, iPosY+169) );
   m_stAmbiant    .SetPos ( FramePos( iPosX+51+3 , iPosY+174+2, iPosX+158, iPosY+191) );
   m_stCDMusic    .SetPos ( FramePos( iPosX+51+3 , iPosY+196+2, iPosX+250, iPosY+213) );

   m_btnCDMusicE  .SetPos ( FramePos( iPosX+251 , iPosY+196, iPosX+268, iPosY+213) );
   m_btnCDMusicD  .SetPos ( FramePos( iPosX+251 , iPosY+196, iPosX+268, iPosY+213) );

   m_btnLogFileName.SetPos ( FramePos( iPosX+413 , iPosY+370, iPosX+556, iPosY+389) );
   m_stLogFileNameD.SetPos ( FramePos( iPosX+414 , iPosY+371, iPosX+555, iPosY+388) );

   //m_btnQuitter.SetPos ( FramePos( iPosX+339, iPosY+404, iPosX+339+72, iPosY+404+24 ) );
   m_btnResetGUI.SetPos ( FramePos( iPosX+286, iPosY+404, iPosX+286+92, iPosY+404+24 ) );
   m_btnLowCPU  .SetPos ( FramePos( iPosX+393, iPosY+404, iPosX+393+72, iPosY+404+24 ) );
   m_btnDefault .SetPos ( FramePos( iPosX+480, iPosY+404, iPosX+480+72, iPosY+404+24 ) );
   m_btnReload  .SetPos ( FramePos( iPosX+114, iPosY+364, iPosX+114+92, iPosY+364+24 ) );
   m_btnNMSGold .SetPos ( FramePos( iPosX+58 , iPosY+404, iPosX+58 +92, iPosY+404+24 ) );
   m_btnPanier  .SetPos ( FramePos( iPosX+170, iPosY+404, iPosX+170+92, iPosY+404+24 ) );

}
 
void V3_OptionsDlg::Draw(V2SPRITEFX *vsfFX)
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

   //backGame.Draw(vsfFX);
   GameUI::Draw( vsfFX );

   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   MOnglet01Button.DrawTextButtonExt(hdc);
   MOnglet02Button.DrawTextButtonExt(hdc);
   MOnglet03Button.DrawTextButtonExt(hdc);
   MOnglet04Button.DrawTextButtonExt(hdc);
   MOnglet05Button.DrawTextButtonExt(hdc);
   MOnglet06Button.DrawTextButtonExt(hdc);
   MOnglet07Button.DrawTextButtonExt(hdc);
   MOnglet08Button.DrawTextButtonExt(hdc);
   m_btnResetGUI.DrawTextButtonExt(hdc);
   m_btnLowCPU.DrawTextButtonExt(hdc);
   m_btnDefault.DrawTextButtonExt(hdc);
   m_btnReload.DrawTextButtonExt(hdc);
   if(m_btnNMSGold.IsShown())
      m_btnNMSGold.DrawTextButtonExt(hdc);
   if(m_btnPanier.IsShown())
      m_btnPanier.DrawTextButtonExt(hdc);
   
   // Draw les texte...
   char strTitle[100];
   sprintf_s(strTitle,100,"Options");

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,strTitle,strlen(strTitle));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}

void V3_OptionsDlg::Hide()
{
   BoxUI::Hide();
}




void V3_OptionsDlg::StartDragDlg( MousePos mousePos )
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

void V3_OptionsDlg::StopDragDlg()
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
void V3_OptionsDlg::DragEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_OptionsDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


void V3_OptionsDlg::UpdateOptionsList(bool bResize)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   int dwScrollPos = 0;
   if(optionsList.GetGrid()->size())
      dwScrollPos = optionsList.GetScrollPos();
   optionsList.DeleteList();
   
   
   OptionParam *pOption = g_SaveGame.GetOptionsParam();


   //AJoute les 3 option de log en debut de liste
   AddOptionsParameters(g_GUILocalString[48],0, pOption->blogNPC);
   AddOptionsParameters(g_GUILocalString[367],1, pOption->blogNPCPerso);
   AddOptionsParameters(g_GUILocalString[49],2, pOption->bLogUser);
   AddOptionsParameters(g_GUILocalString[50],3, pOption->bLogEnable);

   
   //AJoute les options graphique ou du jeux lui meme


   AddOptionsParameters(g_GUILocalString[332],4 , pOption->bDisplayRadar);
   AddOptionsParameters(g_GUILocalString[57] ,5 , pOption->bDisplayStatus);
   AddOptionsParameters(g_GUILocalString[333],6 , pOption->bDisplayChatLog);
   AddOptionsParameters(g_GUILocalString[335],7 , pOption->bDisplayGroupInfo);
   AddOptionsParameters(g_GUILocalString[361],8 , pOption->bDisplayTarget);
   AddOptionsParameters(g_GUILocalString[362],9 , pOption->bDisplayYouTMI);
   AddOptionsParameters(g_GUILocalString[350],10 ,pOption->bDisplayMp3);
   AddOptionsParameters(g_LocalString   [733],11, pOption->bNotDisplayMacro);
   AddOptionsParameters(g_LocalString   [732],12, pOption->bDisplayRestant);
   AddOptionsParameters(g_GUILocalString[376],13 ,pOption->bChestListe);
   AddOptionsParameters(g_GUILocalString[349],14, pOption->bGUIDynamic);
   AddOptionsParameters(g_GUILocalString[334],15, pOption->bMacroOnClick);
   AddOptionsParameters(g_GUILocalString[60] ,16, pOption->bLockResize);
   AddOptionsParameters(g_GUILocalString[61] ,17, pOption->bLockTarget);
   AddOptionsParameters(g_GUILocalString[56] ,18, pOption->bSeraphAnim);
   AddOptionsParameters(g_GUILocalString[52] ,19, pOption->bLightLowSize);
   AddOptionsParameters(g_GUILocalString[331],20, pOption->bMeteoEffect);
   AddOptionsParameters(g_GUILocalString[217],21, pOption->bShowAnimDecorsLight);
   AddOptionsParameters(g_GUILocalString[54] ,22, pOption->bDitherGraph);
   AddOptionsParameters(g_GUILocalString[115],23, pOption->bShowItemSpec);
   AddOptionsParameters(g_GUILocalString[171],24, pOption->bShowNewLife);
   AddOptionsParameters(g_GUILocalString[246],25, pOption->bDisplayDamage);
   AddOptionsParameters(g_GUILocalString[247],26, pOption->bDisplayHealing);
   AddOptionsParameters(g_GUILocalString[360],27, pOption->bChatLogFonce);
   AddOptionsParameters(g_GUILocalString[363],28, pOption->bPlayAt32FPS);
   AddOptionsParameters(g_GUILocalString[364],29, pOption->bAncienneMusique);
   if(m_bNMSDisplay)
      AddOptionsParameters(g_GUILocalString[392],30, pOption->bAncienDisplayOH);
   
   
   

   //AddOptionsParameters(g_GUILocalString[173],19, pOption->bOldStatBar);


   if(pOption->bLockResize && bResize)
   {
      SetWindowPos(siGethWnd(),NULL,0,0,g_Global.GetDisplayW(),g_Global.GetDisplayH(),SWP_DRAWFRAME);
      ShowWindow(siGethWnd(),SW_SHOW);
      RECT ClientRect; 
      GetClientRect(siGethWnd(),&ClientRect);
      SetWindowPos(siGethWnd(),NULL,ClientRect.left,ClientRect.top,2*g_Global.GetDisplayW()-ClientRect.right,2*g_Global.GetDisplayH()-ClientRect.bottom,SWP_NOMOVE);
   }
   optionsList.SetScrollPos(dwScrollPos );
   
   m_stLogFileNameD.SetText( g_SaveGame.GetOptionsParam()->LogName );
}

void V3_OptionsDlg::AddOptionsParameters(char *pstrName,int dwID, int dwStatus)
{
   bool bStatus = false;
   if(dwStatus)
      bStatus = true;
   
   optionsList.AddNewItem();
   OptDlgUI *OptUI = new OptDlgUI;
   
   OptUI->SetText( pstrName, g_DefColorD, 640, 0, 0 );
   OptUI->SetCenter( false );
   OptUI->SetPos( FramePos( 0, 0, OptUI->GetWidth(), OptUI->GetHeight() ) );
   OptUI->optionID = pstrName;
   OptUI->dwState  = bStatus;   
   OptUI->dwID     = dwID;
   optionsList.AddColumnItem( 0, OptUI );
   
   
   OptDlgToggleUI *optDlgToggleUI = new OptDlgToggleUI;
   if( bStatus )
   {
      optDlgToggleUI->SetSpriteId   ( "V3_Check18_ON_N" );
      optDlgToggleUI->SetDownUI     ( "V3_Check18_ON_N" );
      optDlgToggleUI->SetHighlightUI( "V3_Check18_ON_H" );
   }else
   {
      optDlgToggleUI->SetSpriteId   ( "V3_Check18_OFF_N" );
      optDlgToggleUI->SetDownUI     ( "V3_Check18_OFF_N" );
      optDlgToggleUI->SetHighlightUI( "V3_Check18_OFF_H" );
   }
   //channelToggleUI->SetCenter( false ); 
   optDlgToggleUI->SetPos( FramePos( 0, 0, 18, 18 ) );
   optDlgToggleUI->optionID = pstrName;
   optDlgToggleUI->bState   = bStatus;
   optionsList.AddColumnItem( 1, optDlgToggleUI );
}

void V3_OptionsDlg::AddPlayer(char *pstrName,UINT uiID,int dwStatus)
{
   reloadList.AddNewItem();
   OptDlgUI *OptUI = new OptDlgUI;
   
   if(dwStatus == 0)
      OptUI->SetText( pstrName, g_DefColorD, 640, 0, 0 );
   else if(dwStatus == 1)
      OptUI->SetText( pstrName, CL_GREEN, 640, 0, 0 );
   else if(dwStatus == 2)
      OptUI->SetText( pstrName, CL_RED, 640, 0, 0 );
   OptUI->SetCenter( false );
   OptUI->SetPos( FramePos( 0, 0, OptUI->GetWidth(), OptUI->GetHeight() ) );
   OptUI->optionID = pstrName;
   OptUI->dwState  = dwStatus; 
   OptUI->dwID     = uiID;
   reloadList.AddColumnItem( 0, OptUI );
}


void V3_OptionsDlg::OptionsListEvent::LeftClicked( void )
{
   bool bResize = false;
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
  
   int col = me->optionsList.GetSelectedColumn();
   
   string channelId;
   
   // Click in names listing
   if( col == 0 )
   {
      OptDlgUI *optUI = static_cast< OptDlgUI * >( me->optionsList.GetSelection() );
      
      if( optUI == NULL )
         return;
   }
   // Clicked on the toggle button
   else if (col == 1)
   {
      OptDlgUI *optUI = static_cast< OptDlgUI * >( me->optionsList.GetSelection() );
      
      if( optUI == NULL )
         return;
      
      OptionParam *pOption = g_SaveGame.GetOptionsParam();
      switch(optUI->dwID)
      {
         //Option Logs
         case 0:pOption->blogNPC        = !pOption->blogNPC        ; break;
         case 1:pOption->blogNPCPerso   = !pOption->blogNPCPerso   ; break;
         case 2:pOption->bLogUser       = !pOption->bLogUser       ; break;
         case 3:pOption->bLogEnable     = !pOption->bLogEnable     ; break;

         //All autres options
            
         case 4 :
            pOption->bDisplayRadar           = !pOption->bDisplayRadar       ; 
            if(pOption->bDisplayRadar)
               V3_TMIDlg::GetInstance()->Show();
            else
               V3_TMIDlg::GetInstance()->Hide();
         break;
         case 5 :
            pOption->bDisplayStatus          = !pOption->bDisplayStatus      ; 
            if(pOption->bDisplayStatus)
               V3_EffectStatusDlg::GetInstance()->Show();
            else
               V3_EffectStatusDlg::GetInstance()->Hide();
         break;
         case 6 :
            pOption->bDisplayChatLog          = !pOption->bDisplayChatLog      ; 
            if(pOption->bDisplayChatLog)
               V3_ChatLogDlg::GetInstance()->Display(NULL);
            else
               V3_ChatLogDlg::GetInstance()->Hide();
         break;
         case 7 :
            pOption->bDisplayGroupInfo          = !pOption->bDisplayGroupInfo      ; 
            if(pOption->bDisplayGroupInfo)
               V3_GroupOSDlg::GetInstance()->Show();
            else
               V3_GroupOSDlg::GetInstance()->Hide();
         break;
         
         case 8 :pOption->bDisplayTarget               = !pOption->bDisplayTarget         ; break;
         case 9 :pOption->bDisplayYouTMI               = !pOption->bDisplayYouTMI         ; break;

         case 10:
            pOption->bDisplayMp3          = !pOption->bDisplayMp3      ; 
            if(pOption->bDisplayMp3)
            {
               V3_Mp3Dlg::GetInstance()->Show();
               g_GameMusic.Reset();
            }
            else
            {
               V3_Mp3Dlg::GetInstance()->Hide();
			   ::SetEvent(g_hMp3EventDisable);
            }
            
         break;

         case 11 :
            pOption->bNotDisplayMacro = !pOption->bNotDisplayMacro; 
            V3_MainBarDlg::GetInstance()->Display(NULL);
         break;
         case 12 :pOption->bDisplayRestant           = !pOption->bDisplayRestant     ; break;


         case 13 :
         {
            pOption->bChestListe               = !pOption->bChestListe         ; 

            TFCPacket Send;
            if(pOption->bChestListe)
               Send << (RQ_SIZE)RQ_ChestListe;
            else
               Send << (RQ_SIZE)RQ_ChestNormal;
            SEND_PACKET(Send); 
         }
         break;

         case 14 :pOption->bGUIDynamic               = !pOption->bGUIDynamic         ; break;
         case 15 :pOption->bMacroOnClick             = !pOption->bMacroOnClick       ; break;
         case 16 :
            pOption->bLockResize             = !pOption->bLockResize         ; 
            bResize = true;
         break;
         case 17 :
            pOption->bLockTarget             = !pOption->bLockTarget         ; 
            if(!pOption->bLockTarget)
               FreezeID   = NULL;
         break;
         case 18 :pOption->bSeraphAnim             = !pOption->bSeraphAnim         ; break;
         case 19 :pOption->bLightLowSize           = !pOption->bLightLowSize    ; 
            World.Lock(30); //NM:OK:Changement de light
            Objects.Lock(30);
            Objects.CreateTorch();
            Objects.UpdateLight(TRUE);
            Objects.Unlock(30);
            World.Unlock(30);
         break;
         case 20 :pOption->bMeteoEffect            = !pOption->bMeteoEffect        ; break;
         case 21:
            pOption->bShowAnimDecorsLight    = !pOption->bShowAnimDecorsLight    ; 
            World.Lock(0);//NM:OK:Changement de decors animee
            Objects.Lock(0);
            World.SetPosition(Player.xPos, Player.yPos, Player.World, TRUE,FALSE);
            Objects.Unlock(0);
            World.Unlock(0);
         break;
         case 22:pOption->bDitherGraph            = !pOption->bDitherGraph              ; break;
         case 23:pOption->bShowItemSpec           = !pOption->bShowItemSpec             ; break;
         case 24:pOption->bShowNewLife            = !pOption->bShowNewLife              ; break;
         case 25:pOption->bDisplayDamage          = !pOption->bDisplayDamage            ; break;
         case 26:pOption->bDisplayHealing         = !pOption->bDisplayHealing           ; break;
         case 27:pOption->bChatLogFonce           = !pOption->bChatLogFonce             ; break;
         case 28:
         {
            pOption->bPlayAt32FPS = !pOption->bPlayAt32FPS; 
            Custom.gUpdateFPSOption = TRUE;
         }
         break;
         case 29:
         {
            pOption->bAncienneMusique = !pOption->bAncienneMusique; 
            g_GameMusic.Reset();
            g_GameMusic.LoadNewSound("change option");
         }
         break;
         case 30: 
            if(me->m_bNMSDisplay)
               pOption->bAncienDisplayOH = !pOption->bAncienDisplayOH; 
         break;
      }
      me->UpdateOptionsList(bResize);
    }   
}


void V3_OptionsDlg::MusicVolumeEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   g_SaveGame.GetOptionsParam()->dwMusicVol = me->m_sldMusic.GetSliderPos();
   g_GameMusic.SetNewVolume();
}

void V3_OptionsDlg::AmbiantVolumeEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   g_SaveGame.GetOptionsParam()->dwAmbiantVol = me->m_sldAmbiant.GetSliderPos();
   g_GameMusic.SetNewVolume();
}

void V3_OptionsDlg::EffectEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   g_SaveGame.GetOptionsParam()->dwSoundVol = me->m_sldEffect.GetSliderPos();
   g_GameMusic.SetNewVolume();
}


void V3_OptionsDlg::BrightnessEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   g_SaveGame.GetOptionsParam()->dwBrightnessNew = me->m_sldBright.GetSliderPos();
   

   Objects.CreateTorch();
   Objects.Lock(30);
   Objects.UpdateLight(TRUE);
   Objects.Unlock(30);
}

// CD Music state changed
void V3_OptionsDlg::CdMusicEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );

   
   g_SaveGame.GetOptionsParam()->bUseCD = !g_SaveGame.GetOptionsParam()->bUseCD;
   g_GameMusic.Reset();
   if( g_SaveGame.GetOptionsParam()->bUseCD )
   {
      me->m_btnCDMusicE.Show();
      me->m_btnCDMusicD.Hide();
   }
   else
   {
      me->m_btnCDMusicE.Hide();
      me->m_btnCDMusicD.Show();
      g_GameMusic.LoadNewSound("Disable CD Audio"); 
   }
}


void V3_OptionsDlg::FileNameEvent::LeftClicked( void )
{   
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );

   V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,g_GUILocalString[99] ,g_SaveGame.GetOptionsParam()->LogName,20,475);
   V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueDlg::GetInstance()->SetParent(me);
   V3_AskValueDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueDlg::GetInstance()->Show(0);

   me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );
}

void V3_OptionsDlg::AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey)
{
   if(!bOK)
      return;

   switch(uiShowCodeP)
   {
      case 0:
      {
         OptionParam *pOption = g_SaveGame.GetOptionsParam();
         sprintf_s(pOption ->LogName,48,"%s",(char *)pStrValue1);
         m_stLogFileNameD.SetText( pStrValue1,g_DefColorD );
      }
      break;
   }
}

void V3_OptionsDlg::ConfirmReturn(bool bOK,UINT uiShowCodeP)
{
   switch(uiShowCodeP)
   {
      case 0:
      {
         if(bOK)
         {
            T3VSBSound PopupSound;
            PopupSound.CopyWave(&SoundFX[110]);
            PopupSound.Play(FALSE);
         
            Sleep(10);
         
            TFCPacket sending;
            sending << (RQ_SIZE)RQ_ExitGame;
            SEND_PACKET( sending );
         
            Hide();
         
            if(!COMM.isHalf())
               PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
         }
      }
      break;
      case 1:
      {
         if(bOK)
         {
            //Close all GUI first...
            Hide();


            //reset toute les position des GUI...
            g_Global.ResetAllGUIPositionAndSize();
            RootBoxUI::GetInstance()->SetSideMenuState(false);
            V3_ChatLogDlg::GetInstance()->m_ListText.ReconstructList();
         }
      }
      break;
   }
}





void V3_OptionsDlg::QuitEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   V3_ConfirmDlg::GetInstance()->SetDialogText(g_GUILocalString[83]);
   V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
   V3_ConfirmDlg::GetInstance()->SetParent(me);
   V3_ConfirmDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_ConfirmDlg::GetInstance()->Show(0);
   
   me->RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
}

void V3_OptionsDlg::ResetGUIButtonEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   V3_ConfirmDlg::GetInstance()->SetDialogText(g_GUILocalString[348]);
   V3_ConfirmDlg::GetInstance()->SetButtonText(g_GUILocalString[84] ,g_GUILocalString[85],738,739);
   V3_ConfirmDlg::GetInstance()->SetParent(me);
   V3_ConfirmDlg::GetInstance()->SetXYPos(((g_OptionDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_OptionDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_ConfirmDlg::GetInstance()->Show(1);
   
   me->RequestForegroundControl( V3_ConfirmDlg::GetInstance() );
}





void V3_OptionsDlg::LowCpuButtonEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   OptionParam *pOption = g_SaveGame.GetOptionsParam();

  
   pOption->bLightLowSize           = 1;
   pOption->bMeteoEffect            = 0;
   World.Lock(0); //NM:OK:Changement de decors LOW CPU
   Objects.Lock(0);
   Objects.CreateTorch();
   Objects.UpdateLight(TRUE);
   World.SetPosition(Player.xPos, Player.yPos, Player.World, TRUE,FALSE);
   Objects.Unlock(0);
   World.Unlock(0);
   
   pOption->bDitherGraph            = 0;
   pOption->bSeraphAnim             = 1;
   pOption->bDisplayStatus          = 1;
   pOption->bDisplayRadar           = 1;
   pOption->bDisplayChatLog         = 1;
   pOption->bDisplayGroupInfo       = 1;
   pOption->bMacroOnClick           = 0;
   pOption->bLockResize             = 1;
   pOption->bLockTarget             = 0;
   
   pOption->bPlayAt32FPS            = 0;
   pOption->bAncienneMusique        = 0;
   if(me->m_bNMSDisplay)
      pOption->bAncienDisplayOH        = 0;
   else 
      pOption->bAncienDisplayOH        = 1;
   
  
  
   pOption->bShowItemSpec           = 0; 
   pOption->bShowNewLife            = 0; 
   pOption->bShowNewOmbrage         = 0;
   pOption->bShowAnimDecorsLight    = 0;
   
   me->UpdateOptionsList(true);
   me->m_stLogFileNameD.SetText( g_SaveGame.GetOptionsParam()->LogName,g_DefColorD );
}


void V3_OptionsDlg::DefaultButtonEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );

   OptionParam *pOption = g_SaveGame.GetOptionsParam();
   pOption->blogNPC              = 1;
   pOption->blogNPCPerso         = 1;
   pOption->bLogUser             = 1;
   pOption->bLogEnable           = 0;
   pOption->bDisplayRadar        = 1;
   pOption->bDisplayStatus       = 1;
   pOption->bDisplayChatLog      = 1;
   pOption->bDisplayGroupInfo    = 1;
   pOption->bDisplayTarget       = 1;
   pOption->bDisplayYouTMI       = 0;
   pOption->bDisplayMp3          = 0;
   pOption->bNotDisplayMacro     = 0;
   pOption->bDisplayRestant      = 1;
   pOption->bChestListe          = 0;
   pOption->bGUIDynamic          = 1;
   pOption->bMacroOnClick        = 1;
   pOption->bLockResize          = 1;
   pOption->bLockTarget          = 0;
   pOption->bSeraphAnim          = 1;
   pOption->bLightLowSize        = 1;
   pOption->bMeteoEffect         = 1;
   pOption->bShowAnimDecorsLight = 1;
   pOption->bDitherGraph         = 1;
   pOption->bShowItemSpec        = 1;
   pOption->bShowNewLife         = 1;
   pOption->bDisplayDamage       = 1;
   pOption->bDisplayHealing      = 1;
   pOption->bChatLogFonce        = 1;
   pOption->bPlayAt32FPS         = 1;
   pOption->bAncienneMusique     = 0;
   if(me->m_bNMSDisplay)
      pOption->bAncienDisplayOH     = 0;
   else
      pOption->bAncienDisplayOH     = 1;

   me->Reload();
}


void V3_OptionsDlg::NmsGOLDButtonEvent::LeftClicked( void )
{
   
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   me->Hide();
   
   V3_BoutiqueDlg *pNMSGoldDlg = V3_BoutiqueDlg::GetInstance();
   if(pNMSGoldDlg->IsShown())
   {
      pNMSGoldDlg->Hide();
   }
   else
   {
      pNMSGoldDlg->CallMainNMSAchat();
      pNMSGoldDlg->Display(NULL);
   }
}

void V3_OptionsDlg::NmsPanierButtonEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   me->Hide();
   
   V3_PanierDlg *pNMSPanierDlg = V3_PanierDlg::GetInstance();
   if(pNMSPanierDlg->IsShown())
   {
      pNMSPanierDlg->Hide();
   }
   else
   {
      
      pNMSPanierDlg->RequestAchats();

      pNMSPanierDlg->Display(NULL);
   }
}

void V3_OptionsDlg::UpdatePlayerList()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   
   int dwScrollPos = 0;
   if(reloadList.GetGrid()->size())
      dwScrollPos = reloadList.GetScrollPos();
   reloadList.DeleteList();
   
   UINT uiID = 0;
   for(int i=0;i<Custom.m_shNumberOfPlayer;i++)
   {
      if(strcmp(Custom.m_strName[i],Player.Name)==0)
         AddPlayer(Custom.m_strName[i],uiID,1);
      else
         AddPlayer(Custom.m_strName[i],uiID,0);
      uiID++;
   }
   
   reloadList.SetScrollPos(dwScrollPos );
}

void V3_OptionsDlg::ReloadButtonEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );

   if(Custom.m_shSelectedReloadPlayer >=0 )
   {
      //Envoie le reroll.... fait en sorte que le serveur save le player et detruise le user deja connecter...
      TFCPacket Send;
      Send << (RQ_SIZE)38;
      SEND_PACKET(Send); 
      
      me->m_btnReload.Hide();
      me->Hide();
   }
}

void V3_OptionsDlg::ReloadListEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   OptDlgUI *optUI = static_cast< OptDlgUI * >( me->reloadList.GetSelection() );
   
   if( optUI == NULL || optUI->dwState >0)
   {
      if(optUI->dwState == 1)
      {
      }
      else if(optUI->dwState == 2)
      {
      }
      else
      {
      }
      
      
      Custom.m_shSelectedReloadPlayer = -1;
      me->m_btnReload.Hide();
      return;
   }
   
   Custom.m_shSelectedReloadPlayer = optUI->dwID;
   me->m_btnReload.Show();
   
}






void V3_OptionsDlg::TabStatsEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );

   V3_StatsDlg::GetInstance()->Display(NULL);
   me->Hide();
   
}

void V3_OptionsDlg::TabInvEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   V3_InvDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_OptionsDlg::TabSpellEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );

   V3_SpellDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_OptionsDlg::TabProfessionEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   V3_ProfessionDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_OptionsDlg::TabGuildeEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );

   V3_GuildDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_OptionsDlg::TabGroupEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
   
   V3_GroupeDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_OptionsDlg::TabMacroEvent::LeftClicked( void )
{
   V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );

   V3_MacroDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_OptionsDlg::Reload( void )
{
   OptionParam *pOption = g_SaveGame.GetOptionsParam();
   if(pOption->bDisplayRadar)
      V3_TMIDlg::GetInstance()->Show();
   else
      V3_TMIDlg::GetInstance()->Hide();

   if(pOption->bDisplayStatus)
      V3_EffectStatusDlg::GetInstance()->Show();
   else
      V3_EffectStatusDlg::GetInstance()->Hide();

   if(pOption->bDisplayChatLog)
      V3_ChatLogDlg::GetInstance()->Display(NULL);
   else
      V3_ChatLogDlg::GetInstance()->Hide();

   if(pOption->bDisplayGroupInfo)
      V3_GroupOSDlg::GetInstance()->Show();
   else
      V3_GroupOSDlg::GetInstance()->Hide();

   if(pOption->bDisplayMp3)
      V3_Mp3Dlg::GetInstance()->Show();
   else
      V3_Mp3Dlg::GetInstance()->Hide();

   V3_MainBarDlg::GetInstance()->Display(NULL);



   if(!pOption->bLockTarget)
      FreezeID   = NULL;

   World.Lock(0);//NM:OK:Changement de decors animee
   Objects.Lock(0);
   Objects.CreateTorch();
   Objects.UpdateLight(TRUE);
   World.SetPosition(Player.xPos, Player.yPos, Player.World, TRUE,FALSE);
   Objects.Unlock(0);
   World.Unlock(0);

   Custom.gUpdateFPSOption = TRUE;

   g_GameMusic.Reset();
   g_GameMusic.LoadNewSound("Reload");

   UpdateOptionsList(true);
}

/*
// void V3_OptionsDlg::TabOptionEvent::LeftClicked( void )
// {
//    V3_OptionsDlg *me = static_cast< V3_OptionsDlg * >( GetUI() );
//    
// }
*/


