// V3_DisplaySpecialDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "V3_DisplaySpecialDlg.h"
#include "V3_ChatLogDlg.h"


extern Font *fSmallNormal_14;

extern DWORD TargetID;
extern CSaveGame g_SaveGame;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_DisplaySpecialDlg::V3_DisplaySpecialDlg():
X_Button( hideEvent ),
P_Button(prevButtonEvent),
N_Button(nextButtonEvent),
m_iDisplayLetter (-1),
m_iDisplayMap(-1),
m_bDataLoaded(false)
{
   int i=0;

   m_iXPos = (g_Global.GetScreenW()-g_DSDlgW)/2;;
   m_iYPos = (((g_Global.GetScreenH()-g_DSDlgH)/2)-(g_MainBarDlgH/2));
   
   SetHelpId( this, 297 );
  
   
   m_sprBackM.LoadSprite( "ImageBDIMask" );

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

   prevButtonEvent.SetUI( this );
   nextButtonEvent.SetUI( this );

   

   char strTmp[128];
   for(i=0;i<iNBR_LETTER_PAGE-1;i++)
   {
      sprintf_s(strTmp,128,"%sLetter%d",Player.szLanguage,i); 
      m_LetterPage[i]  .SetSpriteId(strTmp,true,"ImageBDIMask");
   }
   sprintf_s(strTmp,128,"LabyMap"); 
   m_LetterPage[iNBR_LETTER_PAGE-1]  .SetSpriteId(strTmp,true,"ImageBDIMask");


   for(i=0;i<iNBR_MAP_PAGE;i++)
   {
      sprintf_s(strTmp,128,"V3_Map_World%d",i); 
      m_MapPage[i]  .SetSpriteId(strTmp,true,"ImageBDIMask");
   }

   UpdateAllFramePos(m_iXPos,m_iYPos);
 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &P_Button);
   AddChild( &N_Button);

   P_Button.Hide();
   N_Button.Hide();
  
   RootBoxUI::GetInstance()->AddChild( this );
} 

V3_DisplaySpecialDlg::~V3_DisplaySpecialDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_DisplaySpecialDlg *V3_DisplaySpecialDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_DisplaySpecialDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_DisplaySpecialDlg::Display( void *null)
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

void V3_DisplaySpecialDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_DSDlgW, iPosY+g_DSDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27 , iPosX+554+27, iPosY+27+24 ) );
   P_Button.SetPos( FramePos( iPosX+21 , iPosY+408, iPosX+21+24 , iPosY+408+26 ) );
   N_Button.SetPos( FramePos( iPosX+562, iPosY+408, iPosX+562+27, iPosY+408+26 ) );

   int i=0;
   for(i=0;i<iNBR_LETTER_PAGE;i++)
      m_LetterPage[i]  .SetPos( FramePos(iPosX, iPosY, iPosX+g_DSDlgW, iPosY+g_DSDlgH) );
   for(i=0;i<iNBR_MAP_PAGE;i++)
      m_MapPage[i]  .SetPos( FramePos(iPosX, iPosY, iPosX+g_DSDlgW, iPosY+g_DSDlgH) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_DisplaySpecialDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

        
   if(m_bDataLoaded)
      m_sprBack.DrawSpriteN( iOffX, iOffY, vsfFX,&m_sprBackM);
   else if(m_iDisplayLetter >=0 && m_iDisplayLetter <iNBR_LETTER_PAGE)
      m_LetterPage[m_iDisplayLetter].Draw(vsfFX);
   else if(m_iDisplayMap >=0 && m_iDisplayMap <iNBR_MAP_PAGE)
      m_MapPage[m_iDisplayMap].Draw(vsfFX);
     
   
   GameUI::Draw( vsfFX );

   HDC hdc;
   DXDGetDC(&hdc, 86);
   
   // Draw les texte...
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());
   
   //Draw All headercalc
   TextOut(hdc,iOffX+68, iOffY+30,m_strTitle,strlen(m_strTitle));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);
}

void V3_DisplaySpecialDlg::Hide()
{
   m_iDisplayLetter = -1;
   m_iDisplayMap    = -1;
   m_bDataLoaded    = false;
   m_sprBack.Release();
   P_Button.Hide();
   N_Button.Hide();
   BoxUI::Hide();
}

int V3_DisplaySpecialDlg::TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise)
{
   int dwSize = (int)pFont->GetFLen(hdc,pStr);
   return((dwBoxZise-dwSize)/2);
}

void V3_DisplaySpecialDlg::ShowLoadedBmp(BYTE *pData, BYTE* pPal,int dwW, int dwH, char* pStrName)
{
   m_sprBack.Release();
   m_sprBack.LoadSpriteData(pData,pPal,dwW,dwH,12);
   sprintf_s(m_strTitle,1024,"%s",pStrName);
   m_bDataLoaded = true;
   //Display(NULL);
   BoxUI::Show(true);
}

void V3_DisplaySpecialDlg::ShowLetter(int iletterIndex, char* pStrName)
{
   sprintf_s(m_strTitle,1024,"%s",pStrName);

   int iiDx = iletterIndex;
   if(iletterIndex <0 || iletterIndex >= iNBR_LETTER_PAGE)
      iletterIndex = 0;

   m_iDisplayLetter = iletterIndex;

   BoxUI::Show(true);

}

void V3_DisplaySpecialDlg::ShowMaps()
{
   sprintf_s(m_strTitle,1024,"%s",g_GUILocalString[101]);

   m_iDisplayMap = 0;
   N_Button.Show();
   BoxUI::Show(true);
   
}


//////////////////////////////////////////////////////////////////////////////////////////
void V3_DisplaySpecialDlg::PrevButtonEvent::LeftClicked( void )
{
   V3_DisplaySpecialDlg *me = static_cast< V3_DisplaySpecialDlg * >( GetUI() );
   
   if(me->m_iDisplayMap >=0)
   {
      if(me->m_iDisplayMap >0)
      {
         me->m_iDisplayMap--;
         if(me->m_iDisplayMap == 0)
            me->P_Button.Hide();
         me->N_Button.Show();
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_DisplaySpecialDlg::NextButtonEvent::LeftClicked( void )
{
   V3_DisplaySpecialDlg *me = static_cast< V3_DisplaySpecialDlg * >( GetUI() );
   
   if(me->m_iDisplayMap >=0)
   {
      if(me->m_iDisplayMap < iNBR_MAP_PAGE-1)
      {
         me->m_iDisplayMap++;
         if(me->m_iDisplayMap == iNBR_MAP_PAGE-1)
            me->N_Button.Hide();
         me->P_Button.Show();
      }
   }
}

