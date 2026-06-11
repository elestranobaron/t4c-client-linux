// V3_ProfessionDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"

#include "V3_ProfessionDlg.h"
#include "V3_StatDlg.h"
#include "V3_SpellDlg.h"
#include "V3_GuildDlg.h"
#include "V3_GroupeDlg.h"
#include "V3_MacroDlg.h"
#include "V3_OptionsDlg.h"
#include "V3_InvDlg.h"


#include "V3_AskValueDlg.h"
#include "V3_ChatLogDlg.h"
#include "V3_MainBarDlg.h"

extern Font *fSmallNormal_12;
extern CSaveGame g_SaveGame;

DWORD g_dwLastMacroCallProfession = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_ProfessionDlg::V3_ProfessionDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
MOnglet01Button(tabStatsEvent            ,g_GUILocalString[309],g_DefColorH ),
MOnglet02Button(tabInvEvent              ,g_GUILocalString[310],g_DefColorH ),
MOnglet03Button(tabSpellEvent            ,g_GUILocalString[311],g_DefColorH ),
MOnglet04Button(EmptyEvent::GetInstance(),g_GUILocalString[312],g_DefColorH ),
MOnglet05Button(tabGuildeEvent           ,g_GUILocalString[313],g_DefColorH ),
MOnglet06Button(tabGroupEvent            ,g_GUILocalString[314],g_DefColorH ),
MOnglet07Button(tabMacroEvent            ,g_GUILocalString[315],g_DefColorH ),
MOnglet08Button(tabOptionEvent           ,g_GUILocalString[316],g_DefColorH ),
m_bDragDialog (false),
m_ListProfession( professionListEvent ),
m_ListFormule   ( formuleListEvent),
m_btnFabriquer  ( fabriquerButtonEvent,g_LocalString[601],g_DefColorH),
m_ListSkin      (skinListEvent),
m_btnIng01      (buttonIngredientEvent),
m_btnIng02      (buttonIngredientEvent),
m_btnIng03      (buttonIngredientEvent),
m_btnIng04      (buttonIngredientEvent),
m_btnIng05      (buttonIngredientEvent),
m_btnIng06      (buttonIngredientEvent),
m_btnIng07      (buttonIngredientEvent),
m_btnIng08      (buttonIngredientEvent),
m_btnIng09      (buttonIngredientEvent),
m_btnIng10      (buttonIngredientEvent)
{
   int i=0;

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastXPos();
   m_iYPos = g_Global.GetDialogLastYPos();
   
   SetHelpId( this, 261 );
  
   m_BackgroundS.SetSpriteId("V3_ProfessionBack2",true,"V3_ProfessionBack2Mask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_ProfessionIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );

   m_btnIng01.SetSpriteId( "" );
   m_btnIng02.SetSpriteId( "" );
   m_btnIng03.SetSpriteId( "" );
   m_btnIng04.SetSpriteId( "" );
   m_btnIng05.SetSpriteId( "" );
   m_btnIng06.SetSpriteId( "" );
   m_btnIng07.SetSpriteId( "" );
   m_btnIng08.SetSpriteId( "" );
   m_btnIng09.SetSpriteId( "" );
   m_btnIng10.SetSpriteId( "" );
   m_btnIng01.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   m_btnIng02.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   m_btnIng03.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   m_btnIng04.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   m_btnIng05.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   m_btnIng06.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   m_btnIng07.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   m_btnIng08.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   m_btnIng09.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   m_btnIng10.SetHelpText(FontManager::GetInstance()->GetSmallFont( ));
   buttonIngredientEvent.SetUI    ( this );

   for(int i=0;i<10;i++)
      m_dwResetHelp[i] = 0;
   
   
  

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
   //tabProfessionEvent.SetUI(this);
   tabGuildeEvent    .SetUI(this);
   tabGroupEvent     .SetUI(this);
   tabMacroEvent     .SetUI(this);
   tabOptionEvent    .SetUI(this);

  


   m_btnFabriquer.SetSpriteId   ( "V3_Btn92x24_N" );
   m_btnFabriquer.SetDownUI     ( "V3_Btn92x24_D" );
   m_btnFabriquer.SetHighlightUI( "V3_Btn92x24_H" );
   m_btnFabriquer.Hide();
   fabriquerButtonEvent.SetUI( this );
   SetHelpId( m_btnFabriquer, 601 );


   //Liste des metiers...
   
   m_ListProfession.SetupList( 2, 18, FramePos( 0, 0, 4, 1 ) );
   m_ListProfession.SetColumnSize( 0, 89 );
   m_ListProfession.SetColumnSize( 1, 44 );
   m_ListProfession.CenterColumn( 1 );
   ProfessionListingSelectionGraphPatch.SetPos( FramePos( 0, 0, 137, 18 ) );
   ProfessionListingSelectionGraphPatch.SetSpriteId( "V3_ProfessionSelect" );
   ProfessionListingSelectionGraphPatch.Show();
   m_ListProfession.SetSelectionSprite( &ProfessionListingSelectionGraphPatch, -2);
   professionListEvent.SetUI( this );
   
    
   //Liste des formules...
   FormuleListingSelectionGraphPatch.SetPos( FramePos( 0, 0, 508, 20 ) );
   FormuleListingSelectionGraphPatch.SetSpriteId( "V3_ProfessionIngredientSelect2" );
   FormuleListingSelectionGraphPatch.Show();
   m_ListFormule.SetupList( 1, 20, FramePos( 0, 0, 0, 1 ) );
   m_ListFormule.SetColumnSize( 0, 508 );
   m_ListFormule.SetSelectionSprite( &FormuleListingSelectionGraphPatch, 0);
   m_ListFormule.SetScrollDownDownUI( &FormuleListScrollDown );
   m_ListFormule.SetScrollUpDownUI( &FormuleListScrollUp );
   formuleListEvent.SetUI( this );

   m_ListSkin.SetupList( 1, 138, FramePos( 0, 0, 0, 0 ) );
   m_ListSkin.CenterColumn( 0 );
   m_ListSkin.SetColumnSize( 0, 140 );

   m_stHProf.SetText(g_LocalString[603],g_DefColorH);
   m_stHIngr.SetText(g_LocalString[606],g_DefColorH);
   
   

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
   AddChild( &m_btnIng01 );
   AddChild( &m_btnIng02 );
   AddChild( &m_btnIng03 );
   AddChild( &m_btnIng04 );
   AddChild( &m_btnIng05 );
   AddChild( &m_btnIng06 );
   AddChild( &m_btnIng07 );
   AddChild( &m_btnIng08 );
   AddChild( &m_btnIng09 );
   AddChild( &m_btnIng10 );


   AddChild( &m_btnFabriquer );
   AddChild( &m_ListProfession );
   AddChild( &m_ListFormule );
   AddChild( &m_ListSkin );
   AddChild( &m_stHProf );
   AddChild( &m_stHIngr );
   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   
} 

V3_ProfessionDlg::~V3_ProfessionDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_ProfessionDlg *V3_ProfessionDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_ProfessionDlg instance;
    return &instance;
    
} 

unsigned int V3_ProfessionDlg::DisplayM( void *null )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   if(GetInstance()->IsShown())
      GetInstance()->Hide();
   else
   {
      if(timeGetTime()-g_dwLastMacroCallProfession > 500)
      {
         g_dwLastMacroCallProfession =timeGetTime();
         GetInstance()->Display(null);
      }
   }
   return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_ProfessionDlg::Display( void *null )
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   g_Global.SetLastDialogOpen(3);
   
   GetInstance()->m_btnFabriquer.Hide();

   // Destroy the previous Profession
   GetInstance()->m_ListProfession.DeleteList();
   GetInstance()->m_ListFormule.DeleteList();

   TFCPacket sending;	
   // Get the stats.
   sending << (RQ_SIZE)RQ_NM_GetProfession;	
   SEND_PACKET( sending );

   GetInstance()->m_iXPos = g_Global.GetDialogLastXPos();
   GetInstance()->m_iYPos = g_Global.GetDialogLastYPos();
   GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);
   // Show box.
   GetInstance()->Show();
   return 0;
}

void V3_ProfessionDlg::UpdateAllFramePos(int iPosX,int iPosY)
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

   m_btnIng01.SetPos ( FramePos( iPosX+362 , iPosY+268-145  , iPosX+362+147, iPosY+268-145+11 ) );
   m_btnIng02.SetPos ( FramePos( iPosX+362 , iPosY+279-145  , iPosX+362+147, iPosY+279-145+11 ) );
   m_btnIng03.SetPos ( FramePos( iPosX+362 , iPosY+290-145  , iPosX+362+147, iPosY+290-145+11 ) );
   m_btnIng04.SetPos ( FramePos( iPosX+362 , iPosY+301-145  , iPosX+362+147, iPosY+301-145+11 ) );
   m_btnIng05.SetPos ( FramePos( iPosX+362 , iPosY+312-145  , iPosX+362+147, iPosY+312-145+11 ) );
   m_btnIng06.SetPos ( FramePos( iPosX+362 , iPosY+323-145  , iPosX+362+147, iPosY+323-145+11 ) );
   m_btnIng07.SetPos ( FramePos( iPosX+362 , iPosY+334-145  , iPosX+362+147, iPosY+334-145+11 ) );
   m_btnIng08.SetPos ( FramePos( iPosX+362 , iPosY+345-145  , iPosX+362+147, iPosY+345-145+11 ) );
   m_btnIng09.SetPos ( FramePos( iPosX+362 , iPosY+356-145  , iPosX+362+147, iPosY+356-145+11 ) );
   m_btnIng10.SetPos ( FramePos( iPosX+362 , iPosY+367-145  , iPosX+362+147, iPosY+367-145+11 ) );

   m_btnFabriquer  .SetPos( FramePos( iPosX+257 , iPosY+404  , iPosX+257+92, iPosY+404+24 ) );
   m_ListProfession.SetPos( FramePos( iPosX+38  , iPosY+120  , iPosX+174   , iPosY+232    ) );
   m_ListSkin      .SetPos( FramePos( iPosX+179 , iPosY+96   , iPosX+318   , iPosY+233    ) );

   m_ListFormule.SetListDef(iPosX,iPosY,20,37,245,566,390,&FormuleListScrollUp,&FormuleListScrollDown);

   m_stHProf.SetPos ( FramePos( iPosX+38  , iPosY+97  , iPosX+174, iPosY+114 ) );
   m_stHIngr.SetPos ( FramePos( iPosX+331 , iPosY+97  , iPosX+565, iPosY+114 ) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_ProfessionDlg::Draw(V2SPRITEFX *vsfFX)
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
   m_btnFabriquer .DrawTextButtonExt(hdc);
   
   // Draw les texte...

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,g_LocalString[603],strlen(g_LocalString[603]));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);


   //draw formule requestitem and image skin...
   m_ListSkin.DeleteList();

   char strHelp[10][256];
   for(int i=0;i<10;i++)
      strHelp[i][0] = 0x00;

   Player.Lock();

   ListDlgUI *ListUI = static_cast< ListDlgUI * >( m_ListFormule.GetSelection() );
   if( ListUI != NULL && ListUI->dwState >0 && ListUI->dwID >= 0)
   {
      char strTmp1[256];
      char strTmp2[256];
      Player.tlProfessionAll.ToHead();
      LPUSER_PROFESSION p = Player.tlProfessionAll.GetObject();
      p->tlFormule.ToHead();
      
      for(int i=0;i< p->tlFormule.GetNbObjects();i++)
      {
         p->tlFormule.QueryNext();
         LPFORMULE_INFO pFormule = p->tlFormule.GetObject();
         //trouve la bonne formules...
         if(pFormule->ushID == ListUI->dwID)
         {
           
            m_ListSkin.AddNewItem();
            GraphUI *newGraphI = new GraphUI;
            CV2Sprite *lpVSF;
            lpVSF = InvItemIcons( pFormule->ushSkinID);
            newGraphI->SetVSF( lpVSF );
            newGraphI->SetPos( FramePos( 0, 0, lpVSF->GetWidth(), lpVSF->GetHeight() ) );
            m_ListSkin.AddColumnItem( 0, newGraphI );

            int dwWriteYPos = 121;
            pFormule->tlFormuleReq.ToHead();


            bool bCanMake = true;
			   int iIposIng = 0;
            while (pFormule->tlFormuleReq.QueryNext())
            {
               LPFORMULE_REQ pFReq = pFormule->tlFormuleReq.GetObject();

               COLORREF cr;
               if(pFReq->lNbrHave >= pFReq->lNbrNeed)
               {
                  cr = CL_GREEN;
               }
               else
               {
                  bCanMake = false;
                  cr = g_DefColorD;
               }


               //copie du help text
               sprintf_s(strTmp2,256  ,"%d x  %s  (%d/%d)",pFReq->lNbrNeed,pFReq->pName,pFReq->lNbrHave,pFReq->lNbrNeed);
               if(m_dwResetHelp[iIposIng] == 0)
               {
                  m_dwResetHelp[iIposIng] = 1;
                  sprintf_s(strHelp[iIposIng],256,"%s",strTmp2);
               }
               
               sprintf_s(strTmp1,256  ,"%d",pFReq->lNbrNeed);
               fSmallNormal_12->DrawFont(hdc, iOffX+332, iOffY+dwWriteYPos , cr , 0, strTmp1  );
               sprintf_s(strTmp2,256  ,"%s",pFReq->pName);
               if(strlen(strTmp2) >25)
               {
                  strTmp2[22] = 0x00;
                  strcat_s(strTmp2,256,"...");
               }
               sprintf_s(strTmp1,256  ,"x  %s",strTmp2);
               fSmallNormal_12->DrawFont(hdc, iOffX+353, iOffY+dwWriteYPos , cr , 0, strTmp1  );
               sprintf_s(strTmp1,256  ,"(%d/%d)",pFReq->lNbrHave,pFReq->lNbrNeed);
               fSmallNormal_12->DrawFont(hdc, iOffX+510, iOffY+dwWriteYPos , cr , 0, strTmp1  );

            
               
  
               dwWriteYPos+=11;
			      iIposIng++;
            }

            if(bCanMake && !m_btnFabriquer.IsShown())
               m_btnFabriquer.Show();
            else if(!bCanMake && m_btnFabriquer.IsShown())
               m_btnFabriquer.Hide();

            i = p->tlFormule.GetNbObjects();
         }
      }
   }
   else
   {
      if(m_btnFabriquer.IsShown())
         m_btnFabriquer.Hide();
   }

   Player.Unlock();

   DXDReleaseDC(hdc, 86);

   for(int i=0;i<10;i++)
   {
      if(m_dwResetHelp[i] == 1) //need tio chnage context help
      {
         switch(i)
         {
            case 0: 
               m_btnIng01.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng01.ResyncHelpText();
            break;
            case 1: 
               m_btnIng02.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng02.ResyncHelpText();
            break;
            case 2: 
               m_btnIng03.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng03.ResyncHelpText();
            break;
            case 3: 
               m_btnIng04.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng04.ResyncHelpText();
            break;
            case 4: 
               m_btnIng05.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng05.ResyncHelpText();
            break;
            case 5: 
               m_btnIng06.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng06.ResyncHelpText();
            break;
            case 6: 
               m_btnIng07.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng07.ResyncHelpText();
            break;
            case 7: 
               m_btnIng08.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng08.ResyncHelpText();
            break;
            case 8: 
               m_btnIng09.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng09.ResyncHelpText();
            break;
            case 9: 
               m_btnIng10.GetHelpText()->SetText(strHelp[i]); 
               m_btnIng10.ResyncHelpText();
            break;
         }
         m_dwResetHelp[i] = 2;
      }
   }
}

void V3_ProfessionDlg::Hide()
{
   BoxUI::Hide();
}





void V3_ProfessionDlg::StartDragDlg( MousePos mousePos )
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

void V3_ProfessionDlg::StopDragDlg()
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
void V3_ProfessionDlg::DragEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_ProfessionDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}






void V3_ProfessionDlg::TabStatsEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
   
   V3_StatsDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_ProfessionDlg::TabInvEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
   
   V3_InvDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_ProfessionDlg::TabSpellEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
   
   V3_SpellDlg::GetInstance()->Display(NULL);
   me->Hide();
}

/*
// void V3_ProfessionDlg::TabProfessionEvent::LeftClicked( void )
// {
//    V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
//    
// }
*/

void V3_ProfessionDlg::TabGuildeEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );

   V3_GuildDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_ProfessionDlg::TabGroupEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
   
   V3_GroupeDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_ProfessionDlg::TabMacroEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );

   V3_MacroDlg::GetInstance()->Display(NULL);
   me->Hide();
}

void V3_ProfessionDlg::TabOptionEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
   
   V3_OptionsDlg::GetInstance()->Display(NULL);
   me->Hide();
}



void V3_ProfessionDlg::FabriquerButtonEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
   
   Player.Lock();
   ListDlgUI *ListUI = static_cast< ListDlgUI * >( me->m_ListFormule.GetSelection() );
   if( ListUI != NULL && ListUI->dwState >0 && ListUI->dwID >= 0)
   {
      Player.tlProfessionAll.ToHead();
      LPUSER_PROFESSION p = Player.tlProfessionAll.GetObject();
      p->tlFormule.ToHead();
      
      for(int i=0;i< p->tlFormule.GetNbObjects();i++)
      {
         p->tlFormule.QueryNext();
         LPFORMULE_INFO pFormule = p->tlFormule.GetObject();
         //trouve la bonne formules...
         if(pFormule->ushID == ListUI->dwID)
         {
            
            TFCPacket sending;
            sending << (short)RQ_NM_SendMakeFormule;
            sending << (short)pFormule->ushID;	// ID of Formule to make
            SEND_PACKET( sending );         
            
            i = p->tlFormule.GetNbObjects();
         }
      }
   }
   Player.Unlock();
}

void V3_ProfessionDlg::ProfessionListEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
   
   ListDlgUI *ListUI = static_cast< ListDlgUI * >( me->m_ListProfession.GetSelection() );
   
   
   if( ListUI != NULL && ListUI->dwState >0)
   {
      me->UpdateFormuleList(ListUI->dwID);
   }
   else
   {
      me->UpdateFormuleList(-1);
   }
   
}

void V3_ProfessionDlg::ButtonIngredientEvent::LeftClicked( void )
{
	V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );

}

void V3_ProfessionDlg::FormuleListEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );

   for(int i=0;i<10;i++)
      me->m_dwResetHelp[i] = 0;

   me->m_btnIng01.GetHelpText()->ResetText("");
   me->m_btnIng02.GetHelpText()->ResetText("");
   me->m_btnIng03.GetHelpText()->ResetText("");
   me->m_btnIng04.GetHelpText()->ResetText("");
   me->m_btnIng05.GetHelpText()->ResetText("");
   me->m_btnIng06.GetHelpText()->ResetText("");
   me->m_btnIng07.GetHelpText()->ResetText("");
   me->m_btnIng08.GetHelpText()->ResetText("");
   me->m_btnIng09.GetHelpText()->ResetText("");
   me->m_btnIng10.GetHelpText()->ResetText("");
}

void V3_ProfessionDlg::SkinListEvent::LeftClicked( void )
{
   V3_ProfessionDlg *me = static_cast< V3_ProfessionDlg * >( GetUI() );
}



void V3_ProfessionDlg::UpdateProfessionList()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   Player.Lock();
   m_ListProfession.DeleteList();
   char strTmp[255];

   Player.tlProfessionAll.ToHead();
   LPUSER_PROFESSION p = Player.tlProfessionAll.GetObject();
   p->tlProfession.ToHead();

   while (p->tlProfession.QueryNext())
   {
      LPPROFESSION_NAME pProfName = p->tlProfession.GetObject();

      sprintf_s(strTmp,255,"%s",pProfName->strName);

      char buf[ 256 ];
      m_ListProfession.AddNewItem();
      
      ListDlgUI *ListUI = new ListDlgUI;
      ListUI->SetText( strTmp, g_DefColorD, 640, 0, 0 );
      ListUI->SetHelpText(FontManager::GetInstance()->GetSmallFont(g_GUILocalString[238+pProfName->chProfessionID]) );
      ListUI->SetCenter( false );
      ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
      ListUI->optionID = strTmp;
      ListUI->dwState  = 1; 
      ListUI->dwID     = pProfName->chProfessionID;
      m_ListProfession.AddColumnItem( 0, ListUI );

      ListUI = new ListDlgUI;
	  _itoa_s( pProfName->ushProfSkill, buf,256, 10 );
      ListUI->SetText(buf , g_DefColorD, 640, 0, 0 );
      ListUI->SetCenter( true );
      ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
	  _itoa_s( pProfName->ushProfSkill, buf,256, 10 );
      ListUI->optionID = buf;
      ListUI->dwState  = 1; 
      ListUI->dwID     = pProfName->chProfessionID;
      m_ListProfession.AddColumnItem( 1, ListUI );
   }
   m_ListProfession.SetScrollPos(0 );

   Player.Unlock();
}

void V3_ProfessionDlg::UpdateProfessionListP()
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );


   Player.Lock();
   Player.tlProfessionAll.ToHead();
   LPUSER_PROFESSION p = Player.tlProfessionAll.GetObject();
   p->tlProfession.ToHead();

   int iCnt = 0;
   while (p->tlProfession.QueryNext())
   {
      LPPROFESSION_NAME pProfName = p->tlProfession.GetObject();
      char buf[ 256 ];

      ListDlgUI *pItems = (ListDlgUI *)m_ListProfession.GetRowItem(1,iCnt++);
      if (pItems)
      {
		   _itoa_s( pProfName->ushProfSkill, buf,256, 10 );
         pItems->SetText( buf, g_DefColorD, 640, 0, 0 );
         pItems->optionID = buf;
      }

   }
   Player.Unlock();
}


void V3_ProfessionDlg::UpdateFormuleList(int iID)
{
   CAutoLock lock( RootBoxUI::GetInstance()->GetLock() );

   Player.Lock();
   m_ListFormule.DeleteList();

   if(iID == -1)
   {
      Player.Unlock();
      return;
   }


  
   char strTmp[255];
   Player.tlProfessionAll.ToHead();
   LPUSER_PROFESSION p = Player.tlProfessionAll.GetObject();
   p->tlFormule.ToHead();

   int iCnt = 0;
   while (p->tlFormule.QueryNext())
   {
      LPFORMULE_INFO pFormule = p->tlFormule.GetObject();

      int icnt = 0;
      if(pFormule->chProfessionID == iID)
      {
         sprintf_s(strTmp,255,"%s",pFormule->pName);

         m_ListFormule.AddNewItem();
      
         ListDlgUI *ListUI = new ListDlgUI;
   
         ListUI->SetText( strTmp, g_DefColorD, 640, 0, 0 );
         ListUI->SetCenter( false );
         ListUI->SetPos( FramePos( 0, 0, ListUI->GetWidth(), ListUI->GetHeight() ) );
         ListUI->optionID = strTmp;
         ListUI->dwState  = 1; 
         ListUI->dwID     = pFormule->ushID;
         m_ListFormule.AddColumnItem( icnt++, ListUI );
      }
   }

   m_ListFormule.SetScrollPos(0 );
   Player.Unlock();
}


