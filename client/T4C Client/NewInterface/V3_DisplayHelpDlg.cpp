// V3_DisplayHelpDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "V3_DisplayHelpDlg.h"
#include "V3_ChatLogDlg.h"


#define V2IMAGEHELP_FILEPATH "\\Help\\V3_Help"

extern Font *fSmallNormal_14;

extern DWORD TargetID;
extern CSaveGame g_SaveGame;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_DisplayHelpDlg::V3_DisplayHelpDlg():
X_Button( hideEvent ),
P_Button(prevButtonEvent),
N_Button(nextButtonEvent),
I_Button(indexButtonEvent),
m_iDisplayHelp(-1),
Link1_1(link11ButtonEvent),
Link1_2(link12ButtonEvent),
Link1_3(link13ButtonEvent),
Link1_4(link14ButtonEvent),
Link1_5(link15ButtonEvent),
Link1_6(link16ButtonEvent),
Link1_7(link17ButtonEvent),
Link1_8(link18ButtonEvent),
Link1_9(link19ButtonEvent),
Link2_1(link21ButtonEvent),
Link2_2(link22ButtonEvent),
Link2_3(link23ButtonEvent),
Link2_4(link24ButtonEvent),
Link2_5(link25ButtonEvent),
Link2_6(link26ButtonEvent),
Link2_7(link27ButtonEvent),
Link2_8(link28ButtonEvent),
Link2_9(link29ButtonEvent)
{
   int i=0;

   m_iXPos = (g_Global.GetScreenW()-g_DSHDlgW)/2;;
   m_iYPos = (((g_Global.GetScreenH()-g_DSHDlgH)/2)-(g_MainBarDlgH/2));
   
   //SetHelpId( this, 297 );
  
   
   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_AHIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   P_Button.SetSpriteId   ( "V3_BtnP_N" );
   P_Button.SetDownUI     ( "V3_BtnP_N" );
   P_Button.SetHighlightUI( "V3_BtnP_H" );

   N_Button.SetSpriteId   ( "V3_BtnN_N" );
   N_Button.SetDownUI     ( "V3_BtnN_N" );
   N_Button.SetHighlightUI( "V3_BtnN_H" );

   I_Button.SetSpriteId   ( "BtnIndexN" );
   I_Button.SetDownUI     ( "BtnIndexN" );
   I_Button.SetHighlightUI( "BtnIndexH" );

 
   prevButtonEvent.SetUI( this );
   nextButtonEvent.SetUI( this );
   indexButtonEvent.SetUI( this );

   link11ButtonEvent.SetUI( this );
   link12ButtonEvent.SetUI( this );
   link13ButtonEvent.SetUI( this );
   link14ButtonEvent.SetUI( this );
   link15ButtonEvent.SetUI( this );
   link16ButtonEvent.SetUI( this );
   link17ButtonEvent.SetUI( this );
   link18ButtonEvent.SetUI( this );
   link19ButtonEvent.SetUI( this );
   link21ButtonEvent.SetUI( this );
   link22ButtonEvent.SetUI( this );
   link23ButtonEvent.SetUI( this );
   link24ButtonEvent.SetUI( this );
   link25ButtonEvent.SetUI( this );
   link26ButtonEvent.SetUI( this );
   link27ButtonEvent.SetUI( this );
   link28ButtonEvent.SetUI( this );
   link29ButtonEvent.SetUI( this );



   ClientConfig *pConfig = g_Global.GetClientConfig();
   for(i=0;i<iNBR_HELP_PAGE;i++)
      sprintf_s(m_strHelpPageName[i],1024,"%s%s%s%02d.bmp",pConfig->strPath,V2IMAGEHELP_FILEPATH,Player.szLanguage,i);


   m_bHelpPageLoaded = false;
   m_HelpPageHDM.LoadSprite( "ImageBDIHelpMask" );

   
   UpdateAllFramePos(m_iXPos,m_iYPos);

   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &P_Button);
   AddChild( &N_Button);
   AddChild( &I_Button);
   
   AddChild( &Link1_1);
   AddChild( &Link1_2);
   AddChild( &Link1_3);
   AddChild( &Link1_4);
   AddChild( &Link1_5);
   AddChild( &Link1_6);
   AddChild( &Link1_7);
   AddChild( &Link1_8);
   AddChild( &Link1_9);
   AddChild( &Link2_1);
   AddChild( &Link2_2);
   AddChild( &Link2_3);
   AddChild( &Link2_4);
   AddChild( &Link2_5);
   AddChild( &Link2_6);
   AddChild( &Link2_7);
   AddChild( &Link2_8);
   AddChild( &Link2_9);


   P_Button.Hide();
   N_Button.Hide();
   I_Button.Hide();

   Link1_1.Hide();
   Link1_2.Hide();
   Link1_3.Hide();
   Link1_4.Hide();
   Link1_5.Hide();
   Link1_6.Hide();
   Link1_7.Hide();
   Link1_8.Hide();
   Link1_9.Hide();
   Link2_1.Hide();
   Link2_2.Hide();
   Link2_3.Hide();
   Link2_4.Hide();
   Link2_5.Hide();
   Link2_6.Hide();
   Link2_7.Hide();
   Link2_8.Hide();
   Link2_9.Hide();

  
   RootBoxUI::GetInstance()->AddChild( this );
} 

V3_DisplayHelpDlg::~V3_DisplayHelpDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_DisplayHelpDlg *V3_DisplayHelpDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_DisplayHelpDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_DisplayHelpDlg::Display( void *null)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   // If inventory is already shown.
   if( GetInstance()->IsShown())
   {
      // Hide box
      GetInstance()->Hide();
   }
   else
   {
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      GetInstance()->Show(true);
   }
   return 0;
}

void V3_DisplayHelpDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_DSHDlgW, iPosY+g_DSHDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+614, iPosY+27 , iPosX+614+27, iPosY+27+24 ) );
   P_Button.SetPos( FramePos( iPosX+21 , iPosY+408, iPosX+21+24 , iPosY+408+26 ) );
   N_Button.SetPos( FramePos( iPosX+622, iPosY+408, iPosX+622+27, iPosY+408+26 ) );
   I_Button.SetPos( FramePos( iPosX+583, iPosY+27, iPosX+610, iPosY+52 ) );

   /*
   for(int i=0;i<iNBR_HELP_PAGE;i++)
      m_HelpPage[i]  .SetPos( FramePos(iPosX, iPosY, iPosX+g_DSHDlgW, iPosY+g_DSHDlgH) );
   */

   Link1_1.SetPos( FramePos( iPosX+ 98, iPosY+133, iPosX+98+220, iPosY+133+14 ) );
   Link1_2.SetPos( FramePos( iPosX+ 98, iPosY+148, iPosX+98+220, iPosY+148+14 ) );
   Link1_3.SetPos( FramePos( iPosX+ 98, iPosY+162, iPosX+98+220, iPosY+162+14 ) );
   Link1_4.SetPos( FramePos( iPosX+ 98, iPosY+176, iPosX+98+220, iPosY+176+14 ) );
   Link1_5.SetPos( FramePos( iPosX+ 98, iPosY+191, iPosX+98+220, iPosY+191+14 ) );
   Link1_6.SetPos( FramePos( iPosX+ 98, iPosY+205, iPosX+98+220, iPosY+205+14 ) );
   Link1_7.SetPos( FramePos( iPosX+ 98, iPosY+220, iPosX+98+220, iPosY+220+14 ) );
   Link1_8.SetPos( FramePos( iPosX+ 98, iPosY+234, iPosX+98+220, iPosY+234+14 ) );
   Link1_9.SetPos( FramePos( iPosX+ 98, iPosY+248, iPosX+98+220, iPosY+248+14 ) );
   Link2_1.SetPos( FramePos( iPosX+378, iPosY+133, iPosX+378+220, iPosY+133+14 ) );
   Link2_2.SetPos( FramePos( iPosX+378, iPosY+148, iPosX+378+220, iPosY+148+14 ) );
   Link2_3.SetPos( FramePos( iPosX+378, iPosY+162, iPosX+378+220, iPosY+162+14 ) );
   Link2_4.SetPos( FramePos( iPosX+378, iPosY+176, iPosX+378+220, iPosY+176+14 ) );
   Link2_5.SetPos( FramePos( iPosX+378, iPosY+191, iPosX+378+220, iPosY+191+14 ) );
   Link2_6.SetPos( FramePos( iPosX+378, iPosY+205, iPosX+378+220, iPosY+205+14 ) );
   Link2_7.SetPos( FramePos( iPosX+378, iPosY+220, iPosX+378+220, iPosY+220+14 ) );
   Link2_8.SetPos( FramePos( iPosX+378, iPosY+234, iPosX+378+220, iPosY+234+14 ) );
   Link2_9.SetPos( FramePos( iPosX+378, iPosY+248, iPosX+378+220, iPosY+248+14 ) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_DisplayHelpDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

   if(m_iDisplayHelp >=0 && m_iDisplayHelp <iNBR_HELP_PAGE)
   {
      if(!m_bHelpPageLoaded)
      {
         //load la page
         //doit ajouter le load from HD ici...
         BYTE *pData = NULL;
         BYTE *pPal   = NULL;
         int dwIW,dwIH,dwIB;

         g_Global.LoadImageDataToBmpFile2(m_strHelpPageName[m_iDisplayHelp],&pData,&pPal,&dwIW,&dwIH,&dwIB);
         if(dwIB == 8)
         {
            //vFlipImage...
            BYTE *pDataTmp = new BYTE[dwIW*dwIH];
            for(int y=0,yi=dwIH-1 ; y<dwIH ; y++,yi--)
            {
               BYTE *pS = pData   + (y*dwIW);
               BYTE *pD = pDataTmp+ (yi*dwIW); 
               memcpy(pD,pS,dwIW);
            }
            m_HelpPageHD.Release();
            m_HelpPageHD.LoadSpriteData(pDataTmp,pPal,dwIW,dwIH,12);

            if(pDataTmp)
               delete []pDataTmp;
            pDataTmp = NULL;

            m_bHelpPageLoaded = true;
         }
         if(pData)
            delete []pData;
         pData = NULL;

         if(pPal)
            delete []pPal;
         pPal = NULL;

         
      }

      if(m_bHelpPageLoaded)
         m_HelpPageHD.DrawSpriteN( iOffX, iOffY, vsfFX,&m_HelpPageHDM);
      
   }
        
   //if(m_iDisplayHelp >=0 && m_iDisplayHelp <iNBR_HELP_PAGE)
   //   m_HelpPage[m_iDisplayHelp].Draw(vsfFX);
      
   
   GameUI::Draw( vsfFX );

}

void V3_DisplayHelpDlg::Hide()
{
   m_iDisplayHelp   = -1;
   P_Button.Hide();
   N_Button.Hide();
   I_Button.Hide();
   Link1_1.Hide();
   Link1_2.Hide();
   Link1_3.Hide();
   Link1_4.Hide();
   Link1_5.Hide();
   Link1_6.Hide();
   Link1_7.Hide();
   Link1_8.Hide();
   Link1_9.Hide();
   Link2_1.Hide();
   Link2_2.Hide();
   Link2_3.Hide();
   Link2_4.Hide();
   Link2_5.Hide();
   Link2_6.Hide();
   Link2_7.Hide();
   Link2_8.Hide();
   Link2_9.Hide();

   m_bHelpPageLoaded = false;
   m_HelpPageHD.Release();
   BoxUI::Hide();
}

int V3_DisplayHelpDlg::TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise)
{
   int dwSize = (int)pFont->GetFLen(hdc,pStr);
   return((dwBoxZise-dwSize)/2);
}


void V3_DisplayHelpDlg::ShowHelp(int iPageIndex)
{
   m_bHelpPageLoaded = false;

   int iIndex = iPageIndex;
   if(iIndex < 0)
      iIndex = 0;
   else if(iIndex >= iNBR_HELP_PAGE)
      iIndex = iNBR_HELP_PAGE;

  
   m_iDisplayHelp = iIndex;
   if(iIndex == 0)
      N_Button.Show();
   else if(iIndex == iNBR_HELP_PAGE-1)
      P_Button.Show();
   else
   {
      N_Button.Show();
      P_Button.Show();
   }

   I_Button.Show();
   BoxUI::Show(true);

   if(m_iDisplayHelp == 1)
   {
      Link1_1.Show();
      Link1_2.Show();
      Link1_3.Show();
      Link1_4.Show();
      Link1_5.Show();
      Link1_6.Show();
      Link1_7.Show();
      Link1_8.Show();
      Link1_9.Show();
      Link2_1.Show();
      Link2_2.Show();
      Link2_3.Show();
      Link2_4.Show();
      Link2_5.Show();
      Link2_6.Show();
      Link2_7.Show();
      Link2_8.Show();
      Link2_9.Show();
   }
   else
   {
      Link1_1.Hide();
      Link1_2.Hide();
      Link1_3.Hide();
      Link1_4.Hide();
      Link1_5.Hide();
      Link1_6.Hide();
      Link1_7.Hide();
      Link1_8.Hide();
      Link1_9.Hide();
      Link2_1.Hide();
      Link2_2.Hide();
      Link2_3.Hide();
      Link2_4.Hide();
      Link2_5.Hide();
      Link2_6.Hide();
      Link2_7.Hide();
      Link2_8.Hide();
      Link2_9.Hide();
   }
   
}



//////////////////////////////////////////////////////////////////////////////////////////
void V3_DisplayHelpDlg::PrevButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->m_bHelpPageLoaded = false;
   if(me->m_iDisplayHelp >=0)
   {
      if(me->m_iDisplayHelp >0)
      {
         me->m_iDisplayHelp--;
         if(me->m_iDisplayHelp == 0)
            me->P_Button.Hide();
         me->N_Button.Show();
         
         if(me->m_iDisplayHelp == 1)
         {
            me->Link1_1.Show();
            me->Link1_2.Show();
            me->Link1_3.Show();
            me->Link1_4.Show();
            me->Link1_5.Show();
            me->Link1_6.Show();
            me->Link1_7.Show();
            me->Link1_8.Show();
            me->Link1_9.Show();
            me->Link2_1.Show();
            me->Link2_2.Show();
            me->Link2_3.Show();
            me->Link2_4.Show();
            me->Link2_5.Show();
            me->Link2_6.Show();
            me->Link2_7.Show();
            me->Link2_8.Show();
            me->Link2_9.Show();
         }
         else
         {
            me->Link1_1.Hide();
            me->Link1_2.Hide();
            me->Link1_3.Hide();
            me->Link1_4.Hide();
            me->Link1_5.Hide();
            me->Link1_6.Hide();
            me->Link1_7.Hide();
            me->Link1_8.Hide();
            me->Link1_9.Hide();
            me->Link2_1.Hide();
            me->Link2_2.Hide();
            me->Link2_3.Hide();
            me->Link2_4.Hide();
            me->Link2_5.Hide();
            me->Link2_6.Hide();
            me->Link2_7.Hide();
            me->Link2_8.Hide();
            me->Link2_9.Hide();
         }

      }
   }
  
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_DisplayHelpDlg::NextButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   me->m_bHelpPageLoaded = false;
   if(me->m_iDisplayHelp >=0)
   {
      if(me->m_iDisplayHelp < iNBR_HELP_PAGE-1)
      {
         me->m_iDisplayHelp++;
         if(me->m_iDisplayHelp == iNBR_HELP_PAGE-1)
            me->N_Button.Hide();
         me->P_Button.Show();

         if(me->m_iDisplayHelp == 1)
         {
            me->Link1_1.Show();
            me->Link1_2.Show();
            me->Link1_3.Show();
            me->Link1_4.Show();
            me->Link1_5.Show();
            me->Link1_6.Show();
            me->Link1_7.Show();
            me->Link1_8.Show();
            me->Link1_9.Show();
            me->Link2_1.Show();
            me->Link2_2.Show();
            me->Link2_3.Show();
            me->Link2_4.Show();
            me->Link2_5.Show();
            me->Link2_6.Show();
            me->Link2_7.Show();
            me->Link2_8.Show();
            me->Link2_9.Show();
         }
         else
         {
            me->Link1_1.Hide();
            me->Link1_2.Hide();
            me->Link1_3.Hide();
            me->Link1_4.Hide();
            me->Link1_5.Hide();
            me->Link1_6.Hide();
            me->Link1_7.Hide();
            me->Link1_8.Hide();
            me->Link1_9.Hide();
            me->Link2_1.Hide();
            me->Link2_2.Hide();
            me->Link2_3.Hide();
            me->Link2_4.Hide();
            me->Link2_5.Hide();
            me->Link2_6.Hide();
            me->Link2_7.Hide();
            me->Link2_8.Hide();
            me->Link2_9.Hide();
         }
      }
   }
}



//////////////////////////////////////////////////////////////////////////////////////////
void V3_DisplayHelpDlg::IndexButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(1);
}


void V3_DisplayHelpDlg::Link11ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(2);
}

void V3_DisplayHelpDlg::Link12ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(9);
}

void V3_DisplayHelpDlg::Link13ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(14);
}

void V3_DisplayHelpDlg::Link14ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(15);
}

void V3_DisplayHelpDlg::Link15ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(17);
}

void V3_DisplayHelpDlg::Link16ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(26);
}

void V3_DisplayHelpDlg::Link17ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(27);
}

void V3_DisplayHelpDlg::Link18ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(28);
}

void V3_DisplayHelpDlg::Link19ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(30);
}


void V3_DisplayHelpDlg::Link21ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(31);
}

void V3_DisplayHelpDlg::Link22ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(32);
}

void V3_DisplayHelpDlg::Link23ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(34);
}

void V3_DisplayHelpDlg::Link24ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(35);
}

void V3_DisplayHelpDlg::Link25ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(37);
}

void V3_DisplayHelpDlg::Link26ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(38);
}

void V3_DisplayHelpDlg::Link27ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(39);
}

void V3_DisplayHelpDlg::Link28ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(41);
}

void V3_DisplayHelpDlg::Link29ButtonEvent::LeftClicked( void )
{
   V3_DisplayHelpDlg *me = static_cast< V3_DisplayHelpDlg * >( GetUI() );
   
   me->ShowHelp(42);
}