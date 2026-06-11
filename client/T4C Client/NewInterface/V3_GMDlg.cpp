// V3_GMDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "..\FormatText.h"
#include "V3_GMDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_AskValueDlg.h"
#include "V3_ComboDlg.h"

extern Font *fSmallNormal_14;
extern Font *fSmallNormal_12;
extern CSaveGame g_SaveGame;

#define V2IMAGE_GMMAPNAME_HEADER   "\\Game Files\\UseMapBDI.dat"
#define V2IMAGE_GMMAPNAME_DATA     "\\Game Files\\UseMapBDD.dat"
#define V2IMAGE_GMMDRAW_DATA       "\\Game Files\\gmtdr.bin"
#define GMMAP_WIDTH  624
#define GMMAP_HEIGHT 380


void UpdateDifered(LPVOID pParam);
void ParseText( char *pText);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_GMDlg::V3_GMDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false),
prevButton( prevButtonEvent ,"Prev.",g_DefColorH),
nextButton( nextButtonEvent ,"Next",g_DefColorH),
flagButton( flagButtonEvent ,"Flags",g_DefColorH),
skinButton( skinButtonEvent ,"Skins",g_DefColorH),
itemsButton(itemsButtonEvent ,"Items",g_DefColorH),
monstersButton(monstersButtonEvent,"Monsters",g_DefColorH),
spellsButton(spellsButtonEvent,"Spells",g_DefColorH),
npcsButton(npcsButtonEvent,"Npcs",g_DefColorH),
optionsButton(optionsButtonEvent,"Options",g_DefColorH),
eventsButton(eventsButtonEvent,"Events",g_DefColorH),
ppButton(ppButtonEvent,"Show PPos",g_DefColorH),
GMRButton(gmrButtonEvent,"GM Request",g_DefColorH),
GMDButton(gmdButtonEvent,"GM Draw",g_DefColorH),
GMDTileButton(tileEvent),
DrawBtnM( drawBtnMButtonEvent ,"Go Monster",g_DefColorH),
DrawBtnI( drawBtnIButtonEvent ,"Go Item",g_DefColorH),
m_btnMIName( miNameButtonEvent),
TeleportBtn(teleportEvent),
allButton(allButtonEvent,"All",g_DefColorH),
kewWordButton(kewWordButtonEvent,"Search",g_DefColorH),
RefreshButton(refreshButtonEvent,"Refresh",g_DefColorH),
CloseButton(closeButtonEvent,"Close",g_DefColorH),
flagListCtrl ( flagListEvent ),
optionsList ( optionsListEvent ),
eventsList ( eventsListEvent ),
gmmsgList (gmmsgListEvent ),
m_BtnCB( btnCBEvent)
{
   int i=0;

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastGMXPos();
   m_iYPos = g_Global.GetDialogLastGMYPos();
   
   SetHelpId( this, 297 );
  

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
    
   

   m_GMMapPageB   .SetSpriteId("V3_GMMapBack",true,"V3_GMMapBackMask");
   m_GMMapFlagSkin.SetSpriteId("V3_GMListBack",true,"V3_GMListBackMask");
   m_GMMapOptSkin .SetSpriteId("V3_GMOptionBack",true,"V3_GMOptionBackMask");
   m_GMMsgBack    .SetSpriteId("V3_GMListGMMsgBack",true,"V3_GMListGMMsgBackMask");
   m_GMDrawBack   .SetSpriteId("V3_GMDrawBack",true,"V3_GMDrawBackMask");
     
   
   teleportEvent.SetUI( this );
   tileEvent.SetUI( this );
   
   for(int y=0;y<46;y++)
   {
      for(int x=0;x<36;x++)
      {
         m_bTileMosaic[y][x] = FALSE;
         m_DrawTile[y][x].SetSpriteId( "V3_GMDrawTile");
      }
   }
   


   prevButton.SetSpriteId   ( "V3_Btn72x24_N" );
   prevButton.SetDownUI     ( "V3_Btn72x24_D" );
   prevButton.SetHighlightUI( "V3_Btn72x24_H" );
   prevButtonEvent.SetUI( this );
   
   nextButton.SetSpriteId   ( "V3_Btn72x24_N" );
   nextButton.SetDownUI     ( "V3_Btn72x24_D" );
   nextButton.SetHighlightUI( "V3_Btn72x24_H" );
   nextButtonEvent.SetUI( this );

  
   flagButton.SetSpriteId   ( "V3_Btn72x24_N" );
   flagButton.SetDownUI     ( "V3_Btn72x24_D" );
   flagButton.SetHighlightUI( "V3_Btn72x24_H" );
   flagButtonEvent.SetUI( this );
   
   skinButton.SetSpriteId   ( "V3_Btn72x24_N" );
   skinButton.SetDownUI     ( "V3_Btn72x24_D" );
   skinButton.SetHighlightUI( "V3_Btn72x24_H" );
   skinButtonEvent.SetUI( this );
   
   npcsButton.SetSpriteId   ( "V3_Btn72x24_N" );
   npcsButton.SetDownUI     ( "V3_Btn72x24_D" );
   npcsButton.SetHighlightUI( "V3_Btn72x24_H" );
   npcsButtonEvent.SetUI( this );
   
   itemsButton.SetSpriteId   ( "V3_Btn72x24_N" );
   itemsButton.SetDownUI     ( "V3_Btn72x24_D" );
   itemsButton.SetHighlightUI( "V3_Btn72x24_H" );
   itemsButtonEvent.SetUI( this );
   
   monstersButton.SetSpriteId   ( "V3_Btn72x24_N" );
   monstersButton.SetDownUI     ( "V3_Btn72x24_D" );
   monstersButton.SetHighlightUI( "V3_Btn72x24_H" );
   monstersButtonEvent.SetUI( this );
   
   spellsButton.SetSpriteId   ( "V3_Btn72x24_N" );
   spellsButton.SetDownUI     ( "V3_Btn72x24_D" );
   spellsButton.SetHighlightUI( "V3_Btn72x24_H" );
   spellsButtonEvent.SetUI( this );
   
   optionsButton.SetSpriteId   ( "V3_Btn72x24_N" );
   optionsButton.SetDownUI     ( "V3_Btn72x24_D" );
   optionsButton.SetHighlightUI( "V3_Btn72x24_H" );
   optionsButtonEvent.SetUI( this );

   eventsButton.SetSpriteId   ( "V3_Btn72x24_N" );
   eventsButton.SetDownUI     ( "V3_Btn72x24_D" );
   eventsButton.SetHighlightUI( "V3_Btn72x24_H" );
   eventsButtonEvent.SetUI( this );
   
   ppButton.SetSpriteId   ( "V3_Btn72x24_N" );
   ppButton.SetDownUI     ( "V3_Btn72x24_D" );
   ppButton.SetHighlightUI( "V3_Btn72x24_H" );
   ppButtonEvent.SetUI( this );

   GMRButton.SetSpriteId   ( "V3_Btn72x24_N" );
   GMRButton.SetDownUI     ( "V3_Btn72x24_D" );
   GMRButton.SetHighlightUI( "V3_Btn72x24_H" );
   gmrButtonEvent.SetUI( this );

   GMDButton.SetSpriteId   ( "V3_Btn72x24_N" );
   GMDButton.SetDownUI     ( "V3_Btn72x24_D" );
   GMDButton.SetHighlightUI( "V3_Btn72x24_H" );
   gmdButtonEvent.SetUI( this );

   DrawBtnM.SetSpriteId   ( "V3_Btn72x24_N" );
   DrawBtnM.SetDownUI     ( "V3_Btn72x24_D" );
   DrawBtnM.SetHighlightUI( "V3_Btn72x24_H" );
   drawBtnMButtonEvent.SetUI( this );

   DrawBtnI.SetSpriteId   ( "V3_Btn72x24_N" );
   DrawBtnI.SetDownUI     ( "V3_Btn72x24_D" );
   DrawBtnI.SetHighlightUI( "V3_Btn72x24_H" );
   drawBtnIButtonEvent.SetUI( this );

   miNameButtonEvent.SetUI( this );

   //SetHelpId( m_BtnCB, 257 );
   m_BtnCB.SetSpriteId   ( "V3_MainBtnIconCB_N" );
   m_BtnCB.SetDownUI     ( "V3_MainBtnIconCB_D" );
   m_BtnCB.SetHighlightUI( "V3_MainBtnIconCB_H" );
   btnCBEvent.SetUI( this );
  
   
   allButton.SetSpriteId   ( "V3_Btn72x24_N" );
   allButton.SetDownUI     ( "V3_Btn72x24_D" );
   allButton.SetHighlightUI( "V3_Btn72x24_H" );
   allButtonEvent.SetUI( this );
   
   kewWordButton.SetSpriteId   ( "V3_Btn72x24_N" );
   kewWordButton.SetDownUI     ( "V3_Btn72x24_D" );
   kewWordButton.SetHighlightUI( "V3_Btn72x24_H" );
   kewWordButtonEvent.SetUI( this );

   RefreshButton.SetSpriteId   ( "V3_Btn72x24_N" );
   RefreshButton.SetDownUI     ( "V3_Btn72x24_D" );
   RefreshButton.SetHighlightUI( "V3_Btn72x24_H" );
   refreshButtonEvent.SetUI( this );

   CloseButton.SetSpriteId   ( "V3_Btn72x24_N" );
   CloseButton.SetDownUI     ( "V3_Btn72x24_D" );
   CloseButton.SetHighlightUI( "V3_Btn72x24_H" );
   closeButtonEvent.SetUI( this );


   flagListSelectionGraphPatch.SetSpriteId( "V3_GMListSelect" );
   flagListSelectionGraphPatch.SetPos( FramePos( 0, 0, 574, 18 ) );
   flagListSelectionGraphPatch.Show();
   flagListCtrl.SetSelectionSprite( &flagListSelectionGraphPatch, 0 );
   
   // Setup the user listing
   flagListCtrl.SetupList( 2, 18, FramePos( 0, 0, 2, 0 ) );
   flagListCtrl.SetColumnSize( 0, 144 );
   flagListCtrl.SetColumnSize( 1, 407 );
   flagListCtrl.SetScrollDownDownUI( &flagListScrollDown );
   flagListCtrl.SetScrollUpDownUI( &flagListScrollUp );
   flagListEvent.SetUI( this );
   
   
   
   //Liste des option graphiques...
   optionsList.SetupList( 2, 20, FramePos( 0, 0, 0, 0 ) );
   optionsList.SetColumnSize( 0, 244 );
   optionsList.SetColumnSize( 1, 20  );
   optionsList.CenterColumn( 1 );
   optionsListingSelectionGraphPatch.SetPos( FramePos( 0, 0, 244, 20 ) );
   optionsListingSelectionGraphPatch.SetSpriteId( "V3_GMOptionSelect" );
   optionsListingSelectionGraphPatch.Show();
   optionsList.SetSelectionSprite( &optionsListingSelectionGraphPatch, 0);
   optionsListEvent.SetUI( this );
   optionsList.SetScrollDownDownUI( &optionsListScrollDown );
   optionsList.SetScrollUpDownUI( &optionsListScrollUp );

   //Liste des option graphiques...
   eventsList.SetupList( 2, 20, FramePos( 0, 0, 0, 0 ) );
   eventsList.SetColumnSize( 0, 244 );
   eventsList.SetColumnSize( 1, 20  );
   eventsList.CenterColumn( 1 );
   eventsListingSelectionGraphPatch.SetPos( FramePos( 0, 0, 244, 20 ) );
   eventsListingSelectionGraphPatch.SetSpriteId( "V3_GMOptionSelect" );
   eventsListingSelectionGraphPatch.Show();
   eventsList.SetSelectionSprite( &eventsListingSelectionGraphPatch, 0);
   eventsListEvent.SetUI( this );
   eventsList.SetScrollDownDownUI( &eventsListScrollDown );
   eventsList.SetScrollUpDownUI( &eventsListScrollUp );

   //Liste des GMMsg graphiques...
   gmmsgListingSelectionGraphPatch.SetSpriteId( "V3_GMListGMMsgSelect" );
   gmmsgListingSelectionGraphPatch.SetPos( FramePos( 0, 0, 574, 44 ) );
   gmmsgListingSelectionGraphPatch.Show();
   gmmsgList.SetSelectionSprite( &gmmsgListingSelectionGraphPatch, 0 );

   // Setup the user listing
   gmmsgList.SetupList( 2, 44, FramePos( 0, 0, 2, 0 ) );
   gmmsgList.SetColumnSize( 0, 144 );
   gmmsgList.SetColumnSize( 1, 407 );
   gmmsgList.SetScrollDownDownUI( &gmmsgListScrollDown );
   gmmsgList.SetScrollUpDownUI( &gmmsgListScrollUp );
   gmmsgListEvent.SetUI( this );
   
  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
   GetInstance()->m_stMIName.SetText("",g_DefColorD);
   sprintf_s( GetInstance()->strMIName,512,"");


   AddChild( &m_GMMapPageB);
   AddChild( &m_GMMapFlagSkin);
   m_GMMapFlagSkin.Hide();
   AddChild( &TeleportBtn);
   TeleportBtn.Hide();
   

   AddChild( &allButton);
   allButton.Hide();
   AddChild( &kewWordButton);
   kewWordButton.Hide();



   
   
   //Add All OPT Options and HIDE it NM_OPT
   AddChild( &m_GMMapOptSkin);
   m_GMMapOptSkin.Hide();
   AddChild( &optionsList );
   optionsList .Hide();

   AddChild( &eventsList );
   eventsList .Hide();


   AddChild( &m_GMMsgBack);
   m_GMMsgBack.Hide();
   AddChild( &gmmsgList);
   gmmsgList.Hide();

   AddChild( &m_GMDrawBack);
   m_GMDrawBack.Hide();
   AddChild( &GMDTileButton);
   GMDTileButton.Hide();
   AddChild( &DrawBtnM);
   DrawBtnM.Hide();
   AddChild( &DrawBtnI);
   DrawBtnI.Hide();
   AddChild( &m_BtnCB);
   m_BtnCB.Hide();
   AddChild( &m_stMIName);
   m_stMIName.Hide();
   AddChild( &m_btnMIName);
   m_btnMIName.Hide();
   
   

   
   
   

   AddChild( &RefreshButton);
   RefreshButton.Hide();
   AddChild( &CloseButton);
   CloseButton.Hide();
   
   

   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   
   /////////////////////////////////
   
   
   
   AddChild( &prevButton );
   AddChild( &nextButton );
   AddChild( &flagButton );
   AddChild( &skinButton );
   AddChild( &itemsButton );
   AddChild( &monstersButton );
   AddChild( &spellsButton );
   AddChild( &npcsButton );
   AddChild( &optionsButton );
   AddChild( &eventsButton );
   AddChild( &ppButton );
   AddChild( &GMRButton);
   AddChild( &GMDButton);
   AddChild( &X_Button );
   AddChild( &flagListCtrl );
   flagListCtrl .Hide();
   




   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   ReadGMMapBDImage();
   m_pGMMapData    = new BYTE[GMMAP_WIDTH*GMMAP_HEIGHT];
   m_pGMMapDataPal = new BYTE[256*3];

   m_bShowCurrentGMWorld    = FALSE;
   bNeedToUpdateMap         = FALSE;
   m_dwCurrentGMWorld       = 0;
   m_iLastDiaplayedImageMap = -1;


   m_dwShowWhat       = 0;
   
   m_bLockPlayerPos = false;

   m_iCurrentWaitWhat = -1;

   m_bUpdateAll      = true;
   m_bNeedUpdateList = false;
   m_bNeedUpdateListGMMsg = false;
   m_strKeyWorld[0]  = 0x00;

   

   ::InitializeCriticalSection(&crLockList);
   ::InitializeCriticalSection(&crLockListL);

} 

V3_GMDlg::~V3_GMDlg()
{
   m_vImageGMMap.clear();
   
   if(m_pGMMapData)
      delete[]m_pGMMapData;
   m_pGMMapData = NULL;

   if(m_pGMMapDataPal)
      delete []m_pGMMapDataPal;
   m_pGMMapDataPal = NULL;

   ::DeleteCriticalSection(&crLockList);
   ::DeleteCriticalSection(&crLockListL);
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_GMDlg *V3_GMDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_GMDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_GMDlg::Display( void *null,bool putForeground,int dwShowWhat,int dwValDef)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   GetInstance()->m_iXPos = g_Global.GetDialogLastGMXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastGMYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

   GetInstance()->m_bShowCurrentGMWorld = FALSE;
   GetInstance()->m_GMMapPageB.Hide();
   GetInstance()->m_GMMapFlagSkin.Hide();
   GetInstance()->flagListCtrl .Hide();
   GetInstance()->TeleportBtn.Hide();
   GetInstance()->allButton.Hide();
   GetInstance()->kewWordButton.Hide();
   
   //NM_OPT
   GetInstance()->m_GMMapOptSkin.Hide();
   GetInstance()->optionsList.Hide();

   //EVENTS
   GetInstance()->m_GMMapOptSkin.Hide();
   GetInstance()->eventsList.Hide();

   //GMMsg
   GetInstance()->m_GMMsgBack.Hide();
   GetInstance()->gmmsgList.Hide();
   GetInstance()->RefreshButton.Hide();
   GetInstance()->CloseButton.Hide();

   //GmDraw
   GetInstance()->m_GMDrawBack.Hide();
   GetInstance()->GMDTileButton.Hide();
   GetInstance()->DrawBtnM.Hide();
   GetInstance()->DrawBtnI.Hide();
   GetInstance()->m_BtnCB.Hide();
   GetInstance()->m_btnMIName.Hide();
   GetInstance()->m_stMIName.Hide();

   char strPathFile[1024];
   char strPathFileC[1024];
   ClientConfig *pConfig = g_Global.GetClientConfig();
   sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGE_GMMDRAW_DATA);
   sprintf_s(strPathFileC,1024,"%s\\%s%s",pConfig->strPath,Player.ServerName,V2IMAGE_GMMDRAW_DATA);
   FILE *pfGMD = NULL;
   fopen_s(&pfGMD,strPathFileC,"rb");
   if(pfGMD == NULL)
      fopen_s(&pfGMD,strPathFile,"rb");
   if(pfGMD)
   {
      fclose(pfGMD);
      GetInstance()->GMDButton.Show();
   }
   else
      GetInstance()->GMDButton.Hide();
 
   
   GetInstance()->m_dwShowWhat = dwShowWhat;
   if(GetInstance()->m_dwShowWhat == 0)
   {
      GetInstance()->TeleportBtn.Show();
      GetInstance()->m_dwCurrentGMWorld = dwValDef;
      GetInstance()->m_GMMapPageB.Show();
      if(GetInstance()->m_dwCurrentGMWorld < 0 || GetInstance()->m_dwCurrentGMWorld >= NBR_GM_MAP)
         GetInstance()->m_dwCurrentGMWorld = 0;
      GetInstance()->ReadImageFile(GetInstance()->m_dwCurrentGMWorld,GetInstance()->m_pGMMapData,GetInstance()->m_pGMMapDataPal);

      GetInstance()->m_bShowCurrentGMWorld = TRUE;
      if(dwValDef == 0)
      {
         GetInstance()->nextButton.Show();
         GetInstance()->prevButton.Hide();
      }
      else
      {
         GetInstance()->nextButton.Show();
         GetInstance()->prevButton.Show();
      }
   }
   else if(GetInstance()->m_dwShowWhat == 1 || GetInstance()->m_dwShowWhat == 2 || GetInstance()->m_dwShowWhat == 3 || 
           GetInstance()->m_dwShowWhat == 4 || GetInstance()->m_dwShowWhat == 5 || GetInstance()->m_dwShowWhat == 6    )
   {
      GetInstance()->allButton.Show();
      GetInstance()->kewWordButton.Show();
      GetInstance()->flagListCtrl.Show();
      GetInstance()->m_GMMapFlagSkin.Show();
   }
   else if (GetInstance()->m_dwShowWhat == 7)
   {
      //Show All opt Controll options...
      GetInstance()->m_GMMapOptSkin.Show();
      GetInstance()->optionsList.Show();
   }
   else if (GetInstance()->m_dwShowWhat == 8)
   {
      //Show All opt Controll options...
      GetInstance()->RefreshButton.Show();
      GetInstance()->CloseButton.Show();
      GetInstance()->m_GMMsgBack.Show();
      GetInstance()->gmmsgList.Show();
   }
   else if (GetInstance()->m_dwShowWhat == 9)//GmDraw
   {
      GetInstance()->m_GMDrawBack.Show();
      GetInstance()->GMDTileButton.Show();
      GetInstance()->DrawBtnM.Show();
      GetInstance()->DrawBtnI.Show();
      GetInstance()->m_BtnCB.Show();
      GetInstance()->m_btnMIName.Show();
      GetInstance()->m_stMIName.Show();
   }
   else if (GetInstance()->m_dwShowWhat == 10)
   {
      GetInstance()->m_GMMapOptSkin.Show();
      GetInstance()->eventsList.Show();
   }

   GetInstance()->ClearPosList();
   GetInstance()->ppButton.SetBtnInfo("Show PPos",g_DefColorH);
   


   
   GetInstance()->Show(putForeground);
   return 0;
}

void V3_GMDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_GMDlgW, iPosY+g_GMDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+614, iPosY+27, iPosX+614+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+580, iPosY+57 ) );


   m_GMMapPageB.SetPos( FramePos( iPosX, iPosY, iPosX+g_GMDlgW, iPosY+g_GMDlgH ) );
   m_GMMapFlagSkin.SetPos( FramePos( iPosX, iPosY, iPosX+g_GMDlgW, iPosY+g_GMDlgH ) );
   m_GMMapOptSkin.SetPos( FramePos( iPosX, iPosY, iPosX+g_GMDlgW, iPosY+g_GMDlgH ) );
   m_GMMsgBack.SetPos( FramePos( iPosX, iPosY, iPosX+g_GMDlgW, iPosY+g_GMDlgH ) );
   m_GMDrawBack.SetPos( FramePos( iPosX, iPosY, iPosX+g_GMDlgW, iPosY+g_GMDlgH ) );

   TeleportBtn   .SetPos( FramePos( iPosX+23, iPosY+59, iPosX+644, iPosY+438 ) );
   GMDTileButton .SetPos( FramePos( iPosX+46, iPosY+63, iPosX+621, iPosY+430 ) );
   DrawBtnM      .SetPos( FramePos( iPosX+450, iPosY+436, iPosX+450+72, iPosY+436+24 ) );
   DrawBtnI      .SetPos( FramePos( iPosX+350, iPosY+436, iPosX+350+72, iPosY+436+24 ) );
   m_BtnCB       .SetPos( FramePos( iPosX+28+80,iPosY+436, iPosX+44+80,    iPosY+436+20) );

   flagListCtrl.SetListDef(iPosX,iPosY,20,36,92,632,397,&flagListScrollUp,&flagListScrollDown);
   optionsList.SetListDef(iPosX,iPosY,20,202,92,486,391,&optionsListScrollUp,&optionsListScrollDown);
   eventsList.SetListDef(iPosX,iPosY,20,202,92,486,391,&eventsListScrollUp,&eventsListScrollDown);

   gmmsgList.SetListDef(iPosX,iPosY,20,36,92,632,399,&gmmsgListScrollUp,&gmmsgListScrollDown);

   prevButton    .SetPos( FramePos( iPosX+23 , iPosY+440, iPosX+23 +72, iPosY+440+24 ) );
   nextButton    .SetPos( FramePos( iPosX+573, iPosY+440, iPosX+573+72, iPosY+440+24 ) );
   flagButton    .SetPos( FramePos( iPosX+97 , iPosY+464, iPosX+97 +72, iPosY+464+24 ) );
   skinButton    .SetPos( FramePos( iPosX+173, iPosY+464, iPosX+173+72, iPosY+464+24 ) );
   npcsButton    .SetPos( FramePos( iPosX+248, iPosY+464, iPosX+248+72, iPosY+464+24 ) );
   itemsButton   .SetPos( FramePos( iPosX+97 , iPosY+490, iPosX+97 +72, iPosY+490+24 ) );
   monstersButton.SetPos( FramePos( iPosX+173, iPosY+490, iPosX+173+72, iPosY+490+24 ) );
   spellsButton  .SetPos( FramePos( iPosX+248, iPosY+490, iPosX+248+72, iPosY+490+24 ) );
   optionsButton .SetPos( FramePos( iPosX+409, iPosY+464, iPosX+409+72, iPosY+464+24 ) );

   eventsButton .SetPos( FramePos( iPosX+334, iPosY+464, iPosX+334+72, iPosY+464+24 ) );

   ppButton      .SetPos( FramePos( iPosX+409, iPosY+490, iPosX+409+72, iPosY+490+24 ) );
   GMRButton     .SetPos( FramePos( iPosX+484, iPosY+464, iPosX+484+72, iPosY+464+24 ) );
   GMDButton     .SetPos( FramePos( iPosX+484, iPosY+490, iPosX+484+72, iPosY+490+24 ) );
   m_btnMIName   .SetPos( FramePos( iPosX+51+80, iPosY+435+2 , iPosX+240+80 , iPosY+457  ) );
   m_stMIName    .SetPos( FramePos( iPosX+51+80, iPosY+435+2 , iPosX+240+80 , iPosY+457  ) );
  
   

   allButton     .SetPos( FramePos( iPosX+258, iPosY+407, iPosX+258+72, iPosY+407+24 ) );
   kewWordButton .SetPos( FramePos( iPosX+338, iPosY+407, iPosX+338+72, iPosY+407+24 ) );

   RefreshButton .SetPos( FramePos( iPosX+258, iPosY+407, iPosX+258+72, iPosY+407+24 ) );
   CloseButton   .SetPos( FramePos( iPosX+338, iPosY+407, iPosX+338+72, iPosY+407+24 ) );

   for(int y=0;y<46;y++)
   {
      for(int x=0;x<36;x++)
      {
         int iXP = iPosX+46+(x*16);
         int iYP = iPosY+63+(y*8);
         m_DrawTile[y][x].SetPos( FramePos( iXP, iYP, iXP+16, iYP+8 ) );
      }
   }

}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_GMDlg::Draw(V2SPRITEFX *vsfFX)
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
      
      if(iNewPosX+g_GMDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_GMDlgW;
      if(iNewPosY+g_GMDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_GMDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   if(m_bNeedUpdateList)
   {
      //copy la liste ori dans loc
      LockListL();
      m_vQuestFlagSkinListL.clear();
      for(UINT i=0;i<m_vQuestFlagSkinList.size();i++)
      {
         sQuestflagList newFlag;
         newFlag.ushID = m_vQuestFlagSkinList[i].ushID;
         sprintf_s(newFlag.strName,100,"%s",m_vQuestFlagSkinList[i].strName);
         m_vQuestFlagSkinListL.push_back(newFlag);
      }
      UnlockListL();
      ListToControlGM(m_bUpdateAll,m_strKeyWorld);
      m_bNeedUpdateList = false;
   }

   if(m_bNeedUpdateListGMMsg)
   {
      //copy la liste ori dans loc
      LockListL();
      m_vOptGMMsgL.clear();
      for(UINT i=0;i<m_vOptGMMsg.size();i++)
      {
         sGMMsgList newMsg;
         newMsg.dwMessageID = m_vOptGMMsg[i].dwMessageID;
         sprintf_s(newMsg.strTimeStamp,50,"%s",m_vOptGMMsg[i].strTimeStamp);
         sprintf_s(newMsg.strPlayerName,50,"%s",m_vOptGMMsg[i].strPlayerName);
         sprintf_s(newMsg.strMessage,255,"%s",m_vOptGMMsg[i].strMessage);
         m_vOptGMMsgL.push_back(newMsg);
      }
      UnlockListL();
      ListToControlGMMsg();
      m_bNeedUpdateListGMMsg = false;
   }
   


   UpdateDrawStatus();

   GameUI::Draw( vsfFX );

   
   if(bNeedToUpdateMap)
   {
      bNeedToUpdateMap = FALSE;
      m_sprGMMap.Release();
      m_sprGMMap.LoadSpriteData(m_pGMMapData,m_pGMMapDataPal,GMMAP_WIDTH,GMMAP_HEIGHT,0);
   }
   if(m_bShowCurrentGMWorld)
   {
      m_sprGMMap.DrawSpriteN( iOffX+23, iOffY+59, vsfFX,NULL);
   }
   
   if(m_dwShowWhat == 9) //GmDraw
   {
      //Dessine la grille avec les option deja selectionner...
      for(int y=0;y<46;y++)
      {
         for(int x=0;x<36;x++)
         {
            if(m_bTileMosaic[y][x])
            {
               m_DrawTile[y][x].Draw(vsfFX);
            }
         }
      }
   }

   

   HDC hdc;
   DXDGetDC(&hdc, 86);

   prevButton.DrawTextButtonExt(hdc);
   nextButton.DrawTextButtonExt(hdc);
   flagButton.DrawTextButtonExt(hdc);
   skinButton.DrawTextButtonExt(hdc);
   itemsButton.DrawTextButtonExt(hdc);
   monstersButton.DrawTextButtonExt(hdc);
   spellsButton.DrawTextButtonExt(hdc);
   npcsButton.DrawTextButtonExt(hdc);
   optionsButton.DrawTextButtonExt(hdc);
   eventsButton.DrawTextButtonExt(hdc);
   ppButton.DrawTextButtonExt(hdc);
   GMRButton.DrawTextButtonExt(hdc);
   if(GMDButton.IsShown())
      GMDButton.DrawTextButtonExt(hdc);
   allButton.DrawTextButtonExt(hdc);
   kewWordButton.DrawTextButtonExt(hdc);
   RefreshButton.DrawTextButtonExt(hdc);
   CloseButton.DrawTextButtonExt(hdc);
   DrawBtnM.DrawTextButtonExt(hdc);
   DrawBtnI.DrawTextButtonExt(hdc);
   
   // Draw les texte...
 
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,m_strHeaderText,strlen(m_strHeaderText));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);


   //Dessine les player sur la map...
   if(m_dwShowWhat == 0 && !m_bLockPlayerPos)
   {
      char strPPName[255];
      long iXD = 0;
      long iYD = 0;
      
      for(UINT ip=0;ip< m_vOptPlayerPos.size();ip++)
      {
         if(m_vOptPlayerPos[ip].wW == m_dwCurrentGMWorld)
         {
            MapCoordToMouse(iOffX,iOffY,m_vOptPlayerPos[ip].wX,m_vOptPlayerPos[ip].wY,iXD,iYD);
            sprintf_s(strPPName,255,"- %s",m_vOptPlayerPos[ip].strName);
            fSmallNormal_12->DrawFont(hdc, iXD  ,iYD-7   , g_Global.GetBtnTCE(), 0, strPPName);
         }
      }
   }

   if(m_strMapCoord[0] != 0x00)
   {
      // Draw les texte...
      char strBtnTexte[255];
      // Draw Box Header
      sprintf_s(strBtnTexte,255  ,"%s",m_strMapCoord);
      fSmallNormal_14->DrawFont(hdc, iOffX+TxtOff(fSmallNormal_14,hdc,strBtnTexte ,212)+ 228+1, iOffY+438+1 , CL_BLACK    , 0, strBtnTexte);
      fSmallNormal_14->DrawFont(hdc, iOffX+TxtOff(fSmallNormal_14,hdc,strBtnTexte ,212)+ 228  , iOffY+438   , g_DefColorD, 0, strBtnTexte);
   }


   if(m_dwShowWhat == 1 || m_dwShowWhat == 2 || m_dwShowWhat == 3 || m_dwShowWhat == 4 || m_dwShowWhat == 5 || m_dwShowWhat == 6)
   {
      char strInfo[255];
      //dessine la liste des truc selon la position du scroll....
      unsigned int indexStart = flagListCtrl.GetScrollPos();
      int iYStart = 95;
      for(int i=0;i<17;i++)
      {
         if(indexStart < m_vListDraw.size())
         {
            if(m_dwShowWhat == 6)
            {
               if(m_vListDraw[indexStart].ushID < 10000)
                  sprintf_s(strInfo,255  ,"WDA");
               else
                  sprintf_s(strInfo,255  ,"DLL");
            }
            else
               sprintf_s(strInfo,255  ,"%d",m_vListDraw[indexStart].ushID);
            
            fSmallNormal_12->DrawFont(hdc, iOffX+ 38 , iOffY+iYStart+(i*18) , g_DefColorD    , 0, strInfo);      
            
            sprintf_s(strInfo,255  ,"%s",m_vListDraw[indexStart].strName);
            fSmallNormal_12->DrawFont(hdc, iOffX+ 191, iOffY+iYStart+(i*18) , g_DefColorD    , 0, strInfo);      
         }
         indexStart++;
      }
   }
   else if(m_dwShowWhat == 8) //GmMsg
   {
      //dessine la liste des truc selon la position du scroll....
      unsigned int indexStart = gmmsgList.GetScrollPos();
      int iYStart = 95;
      for(int i=0;i<7;i++)
      {
         if(indexStart < m_vListDrawGMMsg.size())
         {
            fSmallNormal_12->DrawFont(hdc, iOffX+ 38 , iOffY+iYStart+(i*44)    , g_DefColorD    , 0, m_vListDrawGMMsg[indexStart].strTimeStamp);      
            fSmallNormal_12->DrawFont(hdc, iOffX+ 38 , iOffY+iYStart+(i*44)+20 , g_DefColorD    , 0, m_vListDrawGMMsg[indexStart].strPlayerName);      

            FormatText MessageFmt;
            MessageFmt.SetText(m_vListDrawGMMsg[indexStart].strMessage);
            MessageFmt.Format(70);
            for(int l=0;l<MessageFmt.GetnbLine();l++)
            {
               fSmallNormal_12->DrawFont(hdc, iOffX+ 191, iOffY+iYStart+(i*44)+(l*13) , g_DefColorD    , 0, MessageFmt.GetText(l));      
            }

            
         }
         indexStart++;
      }
   }
   DXDReleaseDC(hdc, 86);
}

void V3_GMDlg::Hide()
{
   flagListCtrl.DeleteList();
   optionsList.DeleteList();
   eventsList.DeleteList();
   gmmsgList.DeleteList();
   m_vQuestFlagSkinList.clear();
   m_vQuestFlagSkinListL.clear();
   m_vOptGMMsg.clear();
   m_vOptGMMsgL.clear();
   m_vListDraw.clear();
   m_vListDrawGMMsg.clear();
   m_vOptList.clear();

   BoxUI::Hide();
}





void V3_GMDlg::StartDragDlg( MousePos mousePos )
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

void V3_GMDlg::StopDragDlg()
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
      
      if(m_iXPos+g_GMDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_GMDlgW;
      if(m_iYPos+g_GMDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_GMDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastGMXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_GMDlg::DragEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_GMDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}



void V3_GMDlg::PrevButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   
   if(me->m_dwShowWhat == 1 || me->m_dwShowWhat == 2 || me->m_dwShowWhat == 3 || me->m_dwShowWhat == 4 || me->m_dwShowWhat == 5 || me->m_dwShowWhat == 6 || me->m_dwShowWhat == 7 || me->m_dwShowWhat == 8 || me->m_dwShowWhat == 9 || me->m_dwShowWhat == 10)
   {
      me->m_dwShowWhat = 0;
      me->m_GMMapFlagSkin.Hide();
      me->flagListCtrl .Hide();
      me->allButton.Hide();
      me->kewWordButton.Hide();
      
      //NM_OPT
      me->m_GMMapOptSkin.Hide();
      me->optionsList.Hide();

      //NM EVENTS
      me->m_GMMapOptSkin.Hide();
      me->eventsList.Hide();

      me->m_GMMsgBack.Hide();
      me->gmmsgList.Hide();
      me->RefreshButton.Hide();
      me->CloseButton.Hide();

      //GmDraw
      me->m_GMDrawBack.Hide();
      me->GMDTileButton.Hide();
      me->DrawBtnM.Hide();
      me->DrawBtnI.Hide();
      me->m_BtnCB.Hide();
      me->m_btnMIName.Hide();
      me->m_stMIName.Hide();



      
            
      me->m_GMMapPageB.Show();
      if(me->m_dwCurrentGMWorld < 0 || me->m_dwCurrentGMWorld >= NBR_GM_MAP)
         me->m_dwCurrentGMWorld = 0;
      me->ReadImageFile(me->m_dwCurrentGMWorld,me->m_pGMMapData,me->m_pGMMapDataPal);
      me->m_bShowCurrentGMWorld = TRUE;
      me->TeleportBtn.Show();
      
   }
   else if(me->m_dwShowWhat == 0)
   {
      me->m_dwCurrentGMWorld--;
      if(me->m_dwCurrentGMWorld <0)
         me->m_dwCurrentGMWorld = 0;

      me->m_GMMapPageB.Show();
      if(me->m_dwCurrentGMWorld < 0 || me->m_dwCurrentGMWorld >= NBR_GM_MAP)
         me->m_dwCurrentGMWorld = 0;
      me->ReadImageFile(me->m_dwCurrentGMWorld,me->m_pGMMapData,me->m_pGMMapDataPal);
      me->m_bShowCurrentGMWorld = TRUE;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_GMDlg::NextButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   
   if(me->m_dwShowWhat == 1 || me->m_dwShowWhat == 2 || me->m_dwShowWhat == 3 || me->m_dwShowWhat == 4 || me->m_dwShowWhat == 5 || me->m_dwShowWhat == 6 || me->m_dwShowWhat == 7 || me->m_dwShowWhat == 8|| me->m_dwShowWhat == 9|| me->m_dwShowWhat == 10)
   {
      me->m_dwShowWhat = 0;
      me->m_GMMapFlagSkin.Hide();
      me->flagListCtrl .Hide();
      me->allButton.Hide();
      me->kewWordButton.Hide();
      
      //NM_OPT
      me->m_GMMapOptSkin.Hide();
      me->optionsList.Hide();

      //NM_EVENTS
      me->m_GMMapOptSkin.Hide();
      me->eventsList.Hide();

      me->m_GMMsgBack.Hide();
      me->gmmsgList.Hide();
      me->RefreshButton.Hide();
      me->CloseButton.Hide();

      //GmDraw
      me->m_GMDrawBack.Hide();
      me->GMDTileButton.Hide();
      me->DrawBtnM.Hide();
      me->DrawBtnI.Hide();
      me->m_BtnCB.Hide();
      me->m_btnMIName.Hide();
      me->m_stMIName.Hide();

      me->m_GMMapPageB.Show();
      if(me->m_dwCurrentGMWorld < 0 || me->m_dwCurrentGMWorld >= NBR_GM_MAP)
         me->m_dwCurrentGMWorld = 0;
      me->ReadImageFile(me->m_dwCurrentGMWorld,me->m_pGMMapData,me->m_pGMMapDataPal);
      me->m_bShowCurrentGMWorld = TRUE;
      me->TeleportBtn.Show();
      
   }
   else if(me->m_dwShowWhat == 0)
   {
      me->m_dwCurrentGMWorld++;
      if(me->m_dwCurrentGMWorld >= NBR_GM_MAP)
         me->m_dwCurrentGMWorld = NBR_GM_MAP-1;
      me->m_GMMapPageB.Show();
      if(me->m_dwCurrentGMWorld < 0 || me->m_dwCurrentGMWorld >= NBR_GM_MAP)
         me->m_dwCurrentGMWorld = 0;
      me->ReadImageFile(me->m_dwCurrentGMWorld,me->m_pGMMapData,me->m_pGMMapDataPal);
      me->m_bShowCurrentGMWorld = TRUE;
   }
} 

void V3_GMDlg::FlagButtonEvent::LeftClicked( void )
{
  
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_SvrQuestFlagList;
   SEND_PACKET( packet );
} 

void V3_GMDlg::SkinButtonEvent::LeftClicked( void )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_SvrMonsterSkin;
   SEND_PACKET( packet );
} 

void V3_GMDlg::ItemsButtonEvent::LeftClicked( void )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_SvrItemsList;
   SEND_PACKET( packet );
} 

void V3_GMDlg::MonstersButtonEvent::LeftClicked( void )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_SvrMonsterList;
   SEND_PACKET( packet );
} 

void V3_GMDlg::SpellsButtonEvent::LeftClicked( void )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_SvrSpellList;
   SEND_PACKET( packet );
}

void V3_GMDlg::NpcsButtonEvent::LeftClicked( void )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_SvrNPC;
   SEND_PACKET( packet );
} 

void V3_GMDlg::OptionsButtonEvent::LeftClicked( void )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_SvrOptions;
   SEND_PACKET( packet );
}  

void V3_GMDlg::EventsButtonEvent::LeftClicked( void )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_GetEventsList;
   SEND_PACKET( packet );
}  


void V3_GMDlg::PPButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );

   if(me->m_vOptPlayerPos.size() > 0)
   {
      me->ClearPosList();
      me->ppButton.SetBtnInfo("Show PPos",g_DefColorH);
   }
   else
   {
      TFCPacket packet;
      packet << (RQ_SIZE)RQ_GetAllPlayerPos;
      SEND_PACKET( packet );
      me->ppButton.SetBtnInfo("Hide PPos",g_DefColorH);
   }
}  

void V3_GMDlg::GMRButtonEvent::LeftClicked( void )
{
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_GMMSG_Get;
   SEND_PACKET( packet );
}  

void V3_GMDlg::GMDButtonEvent::LeftClicked( void )
{
  V3_GMDlg::GetInstance()->Display(NULL,true,9,0);

} 


void V3_GMDlg::TileEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );


   if(me->m_dwShowWhat == 9)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      if(me->MouseToTileCoord(mousePos.x,mousePos.y))
      {
         if(mousePos.x >=0 && mousePos.x < 36 && mousePos.y >=0 && mousePos.y < 46)
         {
            me->m_bTileMosaic[mousePos.y][mousePos.x] = 1-me->m_bTileMosaic[mousePos.y][mousePos.x];
         }
         
         //char strTmp[512];
         //sprintf_s(strTmp,512,"Tile %d,%d",mousePos.x,mousePos.y);
         //V3_ChatLogDlg::GetInstance()->AddListLogs("", strTmp, CL_YELLOW, true);
      
      }
   }
} 

void V3_GMDlg::DrawBtnMButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   for(int y=0;y<46;y++)
   {
      for(int x=0;x<36;x++)
      {
         if(me->m_bTileMosaic[y][x])
         {
            int iX = (Player.xPos - 17)+x;
            int iY = (Player.yPos - 25)+y; 
            int iW = Player.World;
            char strCommand[100];
            sprintf_s(strCommand,100,"summon monster %s at %d,%d,%d",me->strMIName,iX,iY,iW);
            ParseText(strCommand);
         }
      }
   }
} 


void V3_GMDlg::DrawBtnIButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   for(int y=0;y<46;y++)
   {
      for(int x=0;x<36;x++)
      {
         if(me->m_bTileMosaic[y][x])
         {
            int iX = (Player.xPos - 17)+x;
            int iY = (Player.yPos - 25)+y; 
            int iW = Player.World;
            char strCommand[100];
            sprintf_s(strCommand,100,"summon item %s at %d,%d,%d",me->strMIName,iX,iY,iW);
            ParseText(strCommand);
         }
      }
   }
} 

void V3_GMDlg::MINameButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   
   V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,g_GUILocalString[99] ,me->strMIName,20,475);
   V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
   V3_AskValueDlg::GetInstance()->SetParent(me);
   V3_AskValueDlg::GetInstance()->SetXYPos(((g_GMDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_GMDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
   V3_AskValueDlg::GetInstance()->Show(1);

   me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );
} 


//////////////////////////////////////////////////////////////////////////////////////////
void V3_GMDlg::TeleportEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   
   
   if(me->m_dwShowWhat == 0)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      if(me->MouseToMapCoord(mousePos.x,mousePos.y))
      {
         char strCommand[100];
         sprintf_s(strCommand,100,"teleport to %d,%d,%d",mousePos.x,mousePos.y,me->m_dwCurrentGMWorld);
         ParseText(strCommand);
      }
   }
} 



//////////////////////////////////////////////////////////////////////////////////////////
void V3_GMDlg::AllButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   
   
   if(me->m_dwShowWhat == 1 || me->m_dwShowWhat == 2 || me->m_dwShowWhat == 3 || me->m_dwShowWhat == 4 || me->m_dwShowWhat == 5 || me->m_dwShowWhat == 6)
   {
      me->ListToControlGM();
   }
} 

void V3_GMDlg::KewWordButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   
   
   if(me->m_dwShowWhat == 1 || me->m_dwShowWhat == 2 || me->m_dwShowWhat == 3 || me->m_dwShowWhat == 4 || me->m_dwShowWhat == 5 || me->m_dwShowWhat == 6)
   {

      V3_AskValueDlg::GetInstance()->SetDialogText1(false,0,"Search string :" ,"",20,475);
      V3_AskValueDlg::GetInstance()->SetButtonText(g_GUILocalString[10] ,g_GUILocalString[11],738,739);
      V3_AskValueDlg::GetInstance()->SetParent(me);
      V3_AskValueDlg::GetInstance()->SetXYPos(((g_GMDlgW-g_AskValueDlgW)/2)+me->m_iXPos , ((g_GMDlgH-g_AskValueDlgH)/2)+me->m_iYPos);
      V3_AskValueDlg::GetInstance()->Show(0);
      
      me->RequestForegroundControl( V3_AskValueDlg::GetInstance() );

      //me->fileNamePopup.Show();
      //me->fileNamePopup.SetFileName( "" );
      //me->RequestForegroundControl( &me->fileNamePopup );
   }
} 

void V3_GMDlg::RefreshButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );


   if(me->m_dwShowWhat == 8)
   {
      TFCPacket packet;
      packet << (RQ_SIZE)RQ_GMMSG_Get;
      SEND_PACKET( packet );
   }
} 

void V3_GMDlg::CloseButtonEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );


   if(me->m_dwShowWhat == 8)
   {
      int Row = me->gmmsgList.GetSelectedRow();

      if (Row >= 0)
      {
         int ID = me->m_vListDrawGMMsg[Row].dwMessageID;
         TFCPacket packet;
         packet << (RQ_SIZE)RQ_GMMSG_Close;
         packet << (long)ID;
         SEND_PACKET( packet );

         TFCPacket packet2;
         packet2 << (RQ_SIZE)RQ_GMMSG_Get;
         SEND_PACKET( packet2 );
      }
   }
} 


void V3_GMDlg::FlagListEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   
   
   
   int Row = me->flagListCtrl.GetSelectedRow();
   
   if (OpenClipboard(NULL))
   {
      EmptyClipboard();
      HGLOBAL hClipboardData;
      hClipboardData = GlobalAlloc(GMEM_DDESHARE, strlen(me->m_vListDraw[Row].strName)+1);
      char * pchData;
      pchData = (char*)GlobalLock(hClipboardData);
      strcpy_s(pchData,strlen(me->m_vListDraw[Row].strName)+1, LPCSTR(me->m_vListDraw[Row].strName));
      GlobalUnlock(hClipboardData);
      SetClipboardData(CF_TEXT,hClipboardData);
      CloseClipboard();

       me->m_stMIName.SetText( me->m_vListDraw[Row].strName,g_DefColorD );
      sprintf_s( me->strMIName,512,"%s",me->m_vListDraw[Row].strName);
      
   }   
} 

void V3_GMDlg::OptionsListEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   
   
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
      
      char strText[100];
      strText[0] = 0x00;
      
      
      switch(optUI->dwID)
      {
         case  0 : sprintf_s(strText,100,"SWITCH PVP DROPS %s",(optUI->bState==true?"off":"on")); break;
         case  1 : sprintf_s(strText,100,"SWITCH PVM DROPS %s",(optUI->bState==true?"off":"on")); break;
         case  2 : sprintf_s(strText,100,"UDPFILTERSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case  3 : sprintf_s(strText,100,"UDPLOGSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case  4 : sprintf_s(strText,100,"RELOADSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case  5 : sprintf_s(strText,100,"GUILDSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case  6 : sprintf_s(strText,100,"AHSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case  7 : sprintf_s(strText,100,"PROFESSION %s",(optUI->bState==true?"off":"on")); break;
         case  8 : sprintf_s(strText,100,"DAMAGEHEALSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case  9 : sprintf_s(strText,100,"RPSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case 10 : sprintf_s(strText,100,"PVPSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case 11 : sprintf_s(strText,100,"DUELSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case 12 : sprintf_s(strText,100,"GMMSGSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case 13 : sprintf_s(strText,100,"JAILSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case 14 : sprintf_s(strText,100,"PSEUDONAMESYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case 15 : sprintf_s(strText,100,"CCSHORTCUTSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case 16 : sprintf_s(strText,100,"XPSTATSYSTEM %s",(optUI->bState==true?"off":"on")); break;
         case 17 : sprintf_s(strText,100,"BANKMANAGEMENT %s",(optUI->bState==true?"off":"on")); break;
         case 18 : sprintf_s(strText,100,"SCROLLXPMANAGEMENT %s",(optUI->bState==true?"off":"on")); break;
         case 19 : sprintf_s(strText,100,"ANTIPLUGSYSTEM %s",(optUI->bState==true?"off":"on")); break;
      }

      
      if (strText[0])
      {
         TFCPacket packetCmd;
         packetCmd << (short)RQ_IndirectTalk;
         packetCmd << (long)Player.ID;
         packetCmd << (char)0;
         packetCmd << (long)Custom.gTextColor;
         packetCmd << (long)Player.lKey;
         packetCmd << (short)strlen(strText);
         packetCmd << (char *)strText;
         SEND_PACKET(packetCmd); 
         
         
         Sleep(250);
         TFCPacket packet;
         packet << (RQ_SIZE)RQ_SvrOptions;
         SEND_PACKET( packet );
      }
   }  
} 

void V3_GMDlg::EventsListEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );
   int col = me->eventsList.GetSelectedColumn();

   string channelId;

   // Click in names listing
   if( col == 0 )
   {
      OptDlgUI *optUI = static_cast< OptDlgUI * >( me->eventsList.GetSelection() );

      if( optUI == NULL )
         return;
   }
   // Clicked on the toggle button
   else if (col == 1)
   {
      OptDlgUI *optUI = static_cast< OptDlgUI * >( me->eventsList.GetSelection() );

      if( optUI == NULL )
         return;

      //optUI->dwID
      char strTmp[100];
      if(optUI->bState)
         sprintf_s(strTmp,100,"#setglobalflag %d to %d",me->m_vEventsList[optUI->dwID].lFlagID,me->m_vEventsList[optUI->dwID].lStopValue);
      else
         sprintf_s(strTmp,100,"#setglobalflag %d to %d",me->m_vEventsList[optUI->dwID].lFlagID,1);

      TFCPacket packetCmd;
      packetCmd << (short)RQ_IndirectTalk;
      packetCmd << (long)Player.ID;
      packetCmd << (char)0;
      packetCmd << (long)Custom.gTextColor;
      packetCmd << (long)Player.lKey;
      packetCmd << (short)strlen(strTmp);
      packetCmd << (char *)strTmp;
      SEND_PACKET(packetCmd); 

      _beginthread( UpdateDifered, 0, NULL );

   }  
} 

void UpdateDifered(LPVOID pParam)
{
   Sleep(5000);
   TFCPacket packet;
   packet << (RQ_SIZE)RQ_GetEventsList;
   SEND_PACKET( packet );
}

void V3_GMDlg::GMMsgListEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );


} 

bool V3_GMDlg::MouseToTileCoord(long &iX, long &iY)
{
   int dwOX = m_iXPos;
   int dwOY = m_iYPos;
   if(dwOY < 0)
      dwOY = 0;

   dwOX+=46;
   dwOY+=63;

   if(iX < dwOX || iX > dwOX+576 || 
      iY < dwOY || iY > dwOY+368 )
      return false; // out of map...

   //set to 0 base...
   int ipX = iX-dwOX;
   int ipY = iY-dwOY;

   iX = ipX/16;
   iY = ipY/8;

   return true;
}


bool V3_GMDlg::MouseToMapCoord(long &iX, long &iY)
{
   int dwOX = m_iXPos;
   int dwOY = m_iYPos;
   if(dwOY < 0)
      dwOY = 0;
   
   dwOX+=23;
   dwOY+=59;
   
   if(iX < dwOX || iX > dwOX+622 || 
      iY < dwOY || iY > dwOY+380 )
      return false; // out of map...
   
   //set to 0 base...
   int ipX = iX-dwOX;
   int ipY = iY-dwOY;
   
   iX = ipX*3072/622;
   iY = ipY*3072/380;
   
   return true;
}

bool V3_GMDlg::MapCoordToMouse(int iOffX,int iOffY,long iXs, long iYs, long &iX, long &iY)
{
   int dwOX = iOffX;
   int dwOY = iOffY;
   if(dwOY < 0)
      dwOY = 0;
   
   dwOX+=23;
   dwOY+=59;
   
   iX = (iXs*622/3072)+dwOX;
   iY = (iYs*380/3072)+dwOY;
   
   return true;
}


void V3_GMDlg::ClearPosList()
{
   m_bLockPlayerPos = true;
   Sleep(200); //make sure the draw is completed...
   
   m_vOptPlayerPos.clear();
}

void V3_GMDlg::AddPlayerPos(char *pstrName,WORD wX, WORD wY, WORD wW)
{
   sPlayerPos newPP;
   
   sprintf_s(newPP.strName,100,"%s",pstrName);
   newPP.wX = wX;
   newPP.wY = wY;
   newPP.wW = wW;
   
   m_vOptPlayerPos.push_back(newPP);
}


void V3_GMDlg::ReadyPosList()
{
   m_bLockPlayerPos = false;
}

void V3_GMDlg::ClearEventsList()
{
   m_vEventsList.clear();
}

void V3_GMDlg::UpdateEventsList(char *pstrName, long lFlagID, long lStopValue, long lStatus)
{
   sEventsList newE;
   newE.lFlagID    = lFlagID;
   newE.lStopValue = lStopValue;
   newE.lStatus    = lStatus;
   sprintf_s(newE.strName,256,"%s",pstrName);
   m_vEventsList.push_back(newE);
}

void V3_GMDlg::EventsListToControl()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   eventsList.DeleteList();
   int ii = 0;
   for(unsigned int i=0;i<m_vEventsList.size();i++)
   {
      AddEventsParameters(i,m_vEventsList[i].strName,m_vEventsList[i].lFlagID,m_vEventsList[i].lStopValue,m_vEventsList[i].lStatus);
   }
   eventsList.SetScrollPos( 0 );
}

void V3_GMDlg::AddEventsParameters(int iID,char *pstrName, long lFlagID, long lStopValue, long lStatus)
{
   bool bStatus = false;
   if(lStatus > 0 )
      bStatus = true;

   eventsList.AddNewItem();
   OptDlgUI *OptUI = new OptDlgUI;

   OptUI->SetText( pstrName, g_DefColorD, 640, 0, 0 );
   OptUI->SetCenter( false );
   OptUI->SetPos( FramePos( 0, 0, OptUI->GetWidth(), OptUI->GetHeight() ) );
   OptUI->optionID = pstrName;
   OptUI->bState = bStatus;   
   OptUI->dwID   = iID;
   eventsList.AddColumnItem( 0, OptUI );


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
   optDlgToggleUI->bState = bStatus;
   eventsList.AddColumnItem( 1, optDlgToggleUI );
}



void V3_GMDlg::ClearOptList()
{
   m_vOptList.clear();
}

void V3_GMDlg::UpdateOptList(char *pstrName, BYTE chOpt)
{
   sOptList newOpt;
   newOpt.chOpt = chOpt;
   sprintf_s(newOpt.strName,100,"%s",pstrName);
   m_vOptList.push_back(newOpt);
}

void V3_GMDlg::OptListToControl()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );
   optionsList.DeleteList();
   int ii = 0;
   for(unsigned int i=0;i<m_vOptList.size();i++)
   {
      AddOptParameters(i,m_vOptList[i].strName,m_vOptList[i].chOpt);
   }
   optionsList.SetScrollPos( 0 );
}

void V3_GMDlg::AddOptParameters(int iID,char *pstrName, int dwStatus)
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
   OptUI->bState = bStatus;   
   OptUI->dwID   = iID;
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
   optDlgToggleUI->bState = bStatus;
   optionsList.AddColumnItem( 1, optDlgToggleUI );
}



void V3_GMDlg::ClearFlagSkinList()
{
   m_vQuestFlagSkinList.clear();
}

void V3_GMDlg::ClearFlagGMMsg()
{
   m_vOptGMMsg.clear();
}


void V3_GMDlg::UpdateQuestFlagSkinList(int iIndex,unsigned short ushID,char *pstrName)
{
   vector <sQuestflagList>::iterator iIndexOK = m_vQuestFlagSkinList.begin();
   for(unsigned int i=0;i<m_vQuestFlagSkinList.size();i++)
   {
      if(m_vQuestFlagSkinList[i].ushID == ushID)
         return; // already inserted...
      else if(_stricmp(m_vQuestFlagSkinList[i].strName,pstrName) <0)
      //else if(m_vQuestFlagSkinList[i].ushID < ushID)
         iIndexOK++;
      else
         break; // peu inserer
   }

   sQuestflagList newFlag;
   newFlag.ushID = ushID;
   sprintf_s(newFlag.strName,100,"%s",pstrName);
   m_vQuestFlagSkinList.insert(iIndexOK,newFlag);
}

void V3_GMDlg::UpdateDisplayGM(bool bAll, char *pstrKeyWorld)
{
   m_bUpdateAll = bAll;
   if(pstrKeyWorld != NULL)
      sprintf_s(m_strKeyWorld,512,pstrKeyWorld);
   else
      m_strKeyWorld[0] = 0x00;

   m_bNeedUpdateList = true;
}

void V3_GMDlg::UpdateGMMsgList(DWORD dwID,char *pstrTime,char *pstrName,char *pstrMsg)
{
   sGMMsgList newMsg;
   newMsg.dwMessageID = dwID;
   sprintf_s(newMsg.strTimeStamp ,50 ,"%s",pstrTime);
   sprintf_s(newMsg.strPlayerName,50 ,"%s",pstrName);
   sprintf_s(newMsg.strMessage   ,255,"%s",pstrMsg);
   m_vOptGMMsg.push_back(newMsg);
}

void V3_GMDlg::UpdateDisplayGMMsg()
{
   m_bNeedUpdateListGMMsg = true;
}

void V3_GMDlg::ListToControlGM(bool bAll, char *pstrKeyWord)
{
   //CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   LockListL();
   flagListCtrl.DeleteList();
   m_vListDraw.clear();
   
   //char strTmp[100];
   int ii = 0;

   for(unsigned int i=0;i<m_vQuestFlagSkinListL.size();i++)
   {
      bool bAdd = true;
      if(!bAll && pstrKeyWord)
      {
         std::string strKeyW(pstrKeyWord);
         std::transform(strKeyW.begin(), strKeyW.end(), strKeyW.begin(), tolower);
         std::string strAll (m_vQuestFlagSkinListL[i].strName);
         std::transform(strAll.begin() , strAll.end() , strAll.begin() , tolower);
         
         if(strstr(strAll.c_str(),strKeyW.c_str()) == NULL)
            bAdd = false;
      }
      
      
      if(bAdd)
      {
         flagListCtrl.AddNewItem();
         

         StrListTmp *pTmp = new StrListTmp;
         pTmp->strTmp[0] = 0;
         flagListCtrl.AddColumnItem( 0, pTmp );

         sQuestflagList newI;
         newI.ushID = m_vQuestFlagSkinListL[i].ushID;
         sprintf_s(newI.strName,100,"%s",m_vQuestFlagSkinListL[i].strName);
         m_vListDraw.push_back(newI);
      }
   }
   
   flagListCtrl.SetScrollPos( 0 );
   UnlockListL();
}

void V3_GMDlg::ListToControlGMMsg()
{
   LockListL();
   gmmsgList.DeleteList();
   m_vListDrawGMMsg.clear();

   //char strTmp[100];
   int ii = 0;

   for(unsigned int i=0;i<m_vOptGMMsgL.size();i++)
   {
      gmmsgList.AddNewItem();

      StrListTmp *pTmp = new StrListTmp;
      pTmp->strTmp[0] = 0;
      gmmsgList.AddColumnItem( 0, pTmp );

      sGMMsgList newI;
      newI.dwMessageID = m_vOptGMMsgL[i].dwMessageID;
      sprintf_s(newI.strTimeStamp,50,"%s",m_vOptGMMsgL[i].strTimeStamp);
      sprintf_s(newI.strPlayerName,50,"%s",m_vOptGMMsgL[i].strPlayerName);
      sprintf_s(newI.strMessage,255,"%s",m_vOptGMMsgL[i].strMessage);
      m_vListDrawGMMsg.push_back(newI);
   }

   gmmsgList.SetScrollPos( 0 );
   UnlockListL();
}


void V3_GMDlg::UpdateDrawStatus()
{
   if(m_dwCurrentGMWorld==0)
   {
      if(prevButton.IsShown())
         prevButton.Hide();
      if(!nextButton.IsShown())
         nextButton.Show();
   }
   else if(m_dwCurrentGMWorld==NBR_GM_MAP-1)
   {
      if(nextButton.IsShown())
         nextButton.Hide();
      if(!prevButton.IsShown())
         prevButton.Show();
   }
   else
   {
      if(!nextButton.IsShown())
         nextButton.Show();
      if(!prevButton.IsShown())
         prevButton.Show();
   }
   
   if(m_dwShowWhat == 0)
   {
      
      sprintf_s(m_strHeaderText,100,"GMMap World %02d",m_dwCurrentGMWorld);
      
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      if(MouseToMapCoord(mousePos.x,mousePos.y))
      {
         sprintf_s(m_strMapCoord,100,"%d,%d",mousePos.x,mousePos.y);
      }
      else
      {
         m_strMapCoord[0] = 0x00;
      }
   }
   else if(m_dwShowWhat == 1)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"GM WDA Quest Flags");
   }
   else if(m_dwShowWhat == 2)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"GM WDA Monsters Skins");
   }
   else if(m_dwShowWhat == 3)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"GM WDA Items List");
   }
   else if(m_dwShowWhat == 4)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"GM WDA Monsters List");
   }
   else if(m_dwShowWhat == 5)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"GM WDA Spells List");
   }
   else if(m_dwShowWhat == 6)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"GM NPCs List");
   }
   else if(m_dwShowWhat == 7)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"Systems options");
   }
   else if(m_dwShowWhat == 8)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"GM Request");
   }
   else if(m_dwShowWhat == 9)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"GM Draw Alpha");
   }
   else if(m_dwShowWhat == 10)
   {
      m_strMapCoord[0] = 0x00;
      sprintf_s(m_strHeaderText,100,"Events");
   }
}

int V3_GMDlg::TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise)
{
   int dwSize = (int)pFont->GetFLen(hdc,pStr);
   return((dwBoxZise-dwSize)/2);
}


void V3_GMDlg::AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey)
{
   if(!bOK)
      return;
   
   switch(uiShowCodeP)
   {
      case 0:
      {
         V3_GMDlg::GetInstance()->ListToControlGM(false, pStrValue1);
      }
      break;
      case 1:
      {
        m_stMIName.SetText( pStrValue1,g_DefColorD );
        sprintf_s(strMIName,512,"%s",pStrValue1);
      }
      break;
   }
}


void V3_GMDlg::ReadGMMapBDImage()
{
   int dwNbrImage = 0;

   unsigned long dwHeader       = (1000*54);
   unsigned char *pHeader       = new unsigned char[dwHeader];
   memset(pHeader,0x00,dwHeader);


   unsigned long dwHeaderC      = 0;
   unsigned char *pHeaderC      = NULL;


   ClientConfig *pConfig = g_Global.GetClientConfig();
   char strPathFile[1024];
   char strPathFileC[1024];
   sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGE_GMMAPNAME_HEADER);
   sprintf_s(strPathFileC,1024,"%s\\%s%s",pConfig->strPath,Player.ServerName,V2IMAGE_GMMAPNAME_HEADER);


   FILE *pfH = NULL;
   fopen_s(&pfH,strPathFileC,"rb");
   if(pfH == NULL)
      fopen_s(&pfH,strPathFile,"rb");
   if(pfH)
   {
      fread(&dwNbrImage ,4,1,pfH);
      fread(&dwHeaderC  ,4,1,pfH);
      pHeaderC       = new unsigned char[dwHeaderC];
      fread(pHeaderC    ,dwHeaderC,1,pfH);
      fclose(pfH);
   }
   else
   {
      if(pHeader)
         delete []pHeader;
      if(pHeaderC)
         delete []pHeaderC;
      return;
   }


   if( uncompress(pHeader, &dwHeader, pHeaderC, dwHeaderC) != Z_OK )
   {
      if(pHeader)
         delete []pHeader;
      if(pHeaderC)
         delete []pHeaderC;
      return;
   }

   unsigned char *pHeaderOTmp = pHeader;
   unsigned char *pHeaderNTmp = pHeader+(1000*4);

   for(int i=0;i<dwNbrImage;i++)
   {
      int dwOffset;
      memcpy(&dwOffset,pHeaderOTmp,4);

      sImagesGMMap newImage;
      sprintf_s(newImage.strName,255,"%s",pHeaderNTmp);
      newImage.iOffset = dwOffset;

      m_vImageGMMap.push_back(newImage);

      pHeaderNTmp+=50;
      pHeaderOTmp+=4;
   }

   int dwSize = m_vImageGMMap.size();

   if(pHeader)
      delete []pHeader;
   if(pHeaderC)
      delete []pHeaderC;
}

void V3_GMDlg::ReadImageFile(int IndexImage, BYTE *pImage, BYTE *pPal)
{
   //on reload pas la meme image pour rien...
   if(IndexImage == m_iLastDiaplayedImageMap)
      return;

   m_iLastDiaplayedImageMap = IndexImage;

   bool bFound = false;
   int iIndex = 0;
   char strNom[50];
   sprintf_s(strNom,50,"GMMap%02d",IndexImage);

   for(unsigned long i=0;i<m_vImageGMMap.size();i++)
   {
      if (!_stricmp( strNom,m_vImageGMMap[i].strName ))
      {
         bFound = true;
         iIndex = i;
         i = m_vImageGMMap.size()+1;
      }
   }

   if(!bFound)
      return;

   int iOffsetReal = m_vImageGMMap[iIndex].iOffset;


   ClientConfig *pConfig = g_Global.GetClientConfig();
   char strPathFile[1024];
   char strPathFileC[1024];
   sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGE_GMMAPNAME_DATA);
   sprintf_s(strPathFileC,1024,"%s\\%s%s",pConfig->strPath,Player.ServerName,V2IMAGE_GMMAPNAME_DATA);

   FILE *pfD = NULL;
   fopen_s(&pfD,strPathFileC,"rb");
   if(pfD == NULL)
      fopen_s(&pfD,strPathFile,"rb");
   if(pfD)
   {
      fseek( pfD, iOffsetReal, SEEK_SET);

      unsigned long dwData       = (GMMAP_WIDTH*GMMAP_HEIGHT)+(256*3);
      unsigned char *pData       = new unsigned char[dwData];


      unsigned long dwDataC      = 0;
      fread(&dwDataC  ,4,1,pfD);
      unsigned char *pDataC      = new unsigned char[dwDataC];

      fread(pDataC    ,dwDataC,1,pfD);

      fclose(pfD);

      if( uncompress(pData, &dwData, pDataC, dwDataC) != Z_OK )
      {
         if(pData)
            delete []pData;
         if(pDataC)
            delete []pDataC;

         return;
      }

      BYTE *pchImageByte = new BYTE[GMMAP_WIDTH*GMMAP_HEIGHT];

      int dwPosDest = 0;

      for(int y=GMMAP_HEIGHT-1;y>=0;y--)
      {
         for(int x=0;x<GMMAP_WIDTH;x++)
         {
            pchImageByte[(y*GMMAP_WIDTH)+x] = (BYTE) pData[(dwPosDest*GMMAP_WIDTH)+x];
         }
         dwPosDest++;
      }

      memcpy(pImage,pchImageByte,GMMAP_WIDTH*GMMAP_HEIGHT);
      memcpy(pPal  ,pData+(GMMAP_WIDTH*GMMAP_HEIGHT),256*3);

      if(pData)
         delete []pData;
      if(pDataC)
         delete []pDataC;
      if(pchImageByte)
         delete []pchImageByte;
   }


   bNeedToUpdateMap = TRUE;
}


void V3_GMDlg::BtnCBEvent::LeftClicked( void )
{
   V3_GMDlg *me = static_cast< V3_GMDlg * >( GetUI() );

   V3_ComboDlg::GetInstance()->ClearCombo();

   int iScrollPos = 0;
   int iSelectedCCPos = 0;

   ClientConfig *pConfig = g_Global.GetClientConfig();
   char strPathFile[1024];
   sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGE_GMMDRAW_DATA);
   me->LoadFileData(strPathFile);
   //AddCC to Combo...
   for(UINT i=0;i<me->m_DrawList.size();i++)
   {
      V3_ComboDlg::GetInstance()->AddComboItem(me->m_DrawList[i].strName);
   }

   V3_ComboDlg::GetInstance()->SetParent(me);
   V3_ComboDlg::GetInstance()->SetXYPos(me->m_iXPos+46 ,me->m_iYPos+436-g_ComboDlgH);
   V3_ComboDlg::GetInstance()->Show(0,iScrollPos);

   me->RequestForegroundControl( V3_ComboDlg::GetInstance() );
}

void V3_GMDlg::LoadFileData(char *pstrName)
{
   m_DrawList.clear();

   FILE *pf=NULL;

   fopen_s(&pf,pstrName,"rb");
   if(pf)
   {
      int iNbrDraw = 0;
      fread(&iNbrDraw ,4,1,pf);
      for(int i=0;i<iNbrDraw;i++)
      {
         sDrawData newData;
         fread(newData.strName, sizeof(char), 100, pf);
         fread(&newData.dwDraw, sizeof(DWORD), 46*36, pf);

         m_DrawList.push_back(newData);
      }
      fclose(pf);
   }
}

void V3_GMDlg::ComboReturn(UINT uiShowCodeP,char *pStrValue)
{
   for(UINT i=0;i<m_DrawList.size();i++)
   {
      if(strcmp(m_DrawList[i].strName,pStrValue)==0)
      {
         //Updatedraw...
         for(int y=0;y<46;y++)
         {
            for(int x=0;x<36;x++)
            {
               m_bTileMosaic[y][x] = FALSE;
               if(m_DrawList[i].dwDraw[y][x])
                  m_bTileMosaic[y][x] = TRUE;
            }
         }
         return;
      }
   }
}

