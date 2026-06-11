// TextPageUI.cpp: implementation of the TextPageUI class.
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"


#include "TextPageUI.h"
#include "..\FormatText.h"

const int g_iMaxLoggedLines       = 1000;
const int g_iMaxLoggedLinesBackup = 500;
const int g_iRemovedLines         = 500;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextPageUI::TextPageUI() : 
m_iFontSize( 10),
m_iMaxLines(0)
{
   m_TempText = FontManager::GetInstance()->GetSmallFont();
   m_TempText->SetText("zZ");

   for(int i=0;i<100;i++)
      m_ListDrawST[i].SetCenter(false);

   ::InitializeCriticalSection(&m_crLock);

}  

TextPageUI::~TextPageUI()
{
    delete m_TempText; 
    DeleteCriticalSection(&m_crLock);
}

void TextPageUI::SetPageDef(int iOX, int iOY,int iBtnW,int iRX1,int iRY1,int iRX2,int iRY2,GraphUI *pSUp,GraphUI *pSDown,int iSOYT,int iSOYB,bool bChangeScroll,bool bLeftScroll)
{
   int iScrollPos = GetScrollPos();
   
   int iNbrVisibleOld =  GetPos().Height()/m_iFontSize;

   if(!bLeftScroll)
   {
      int iSXS = iRX2-iBtnW;
      
      
      SetPos     ( FramePos( iOX+iRX1  , iOY+iRY1 , iOX+iRX2          , iOY+iRY2  ));
      SetTextArea( FramePos( iOX+iRX1+5, iOY+iRY1 , iOX+iRX2-(iBtnW+5), iOY+iRY2  ),bChangeScroll);
      SetScrollRegions( FramePos( iOX+iSXS, iOY+iRY1+iSOYT      , iOX+iRX2, iOY+iRY1+iBtnW+iSOYT ), 
                        FramePos( iOX+iSXS, iOY+iRY2-iBtnW-iSOYB     , iOX+iRX2, iOY+iRY2-iSOYB            ), 
                        FramePos( iOX+iSXS, iOY+iRY1+iBtnW+iSOYT, iOX+iRX2, iOY+iRY2-iBtnW-11-iSOYB)  ); //11 == 1/2 du bmp qui scroll
      
      pSUp->SetPos  (   FramePos( iOX+iSXS, iOY+iRY1+iSOYT      , iOX+iRX2, iOY+iRY1+iBtnW+iSOYT )   );
      pSDown->SetPos(   FramePos( iOX+iSXS, iOY+iRY2-iBtnW-iSOYB, iOX+iRX2, iOY+iRY2-iSOYB         )   );
   }
   else
   {
      SetPos     ( FramePos( iOX+iRX1  , iOY+iRY1 , iOX+iRX2          , iOY+iRY2  ));
      SetTextArea( FramePos( iOX+iRX1+iBtnW+5, iOY+iRY1 , iOX+iRX2-5, iOY+iRY2  ),bChangeScroll);
      SetScrollRegions( FramePos( iOX, iOY+iRY1+iSOYT      , iOX+iBtnW, iOY+iRY1+iBtnW+iSOYT ), 
                        FramePos( iOX, iOY+iRY2-iBtnW-iSOYB, iOX+iBtnW, iOY+iRY2-iSOYB       ), 
                        FramePos( iOX, iOY+iRY1+iBtnW+iSOYT, iOX+iBtnW, iOY+iRY2-iBtnW-11-iSOYB)  ); //11 == 1/2 du bmp qui scroll
      
      pSUp->SetPos  (   FramePos( iOX, iOY+iRY1+iSOYT , iOX+iBtnW, iOY+iRY1+iBtnW+iSOYT )   );
      pSDown->SetPos(   FramePos( iOX, iOY+iRY2-iBtnW-iSOYB, iOX+iBtnW, iOY+iRY2-iSOYB         )   );
   }

   int iNbrVisibleNew =  GetPos().Height()/m_iFontSize;
   if(iNbrVisibleNew < iNbrVisibleOld)
   {
      int iDiff = iNbrVisibleOld-iNbrVisibleNew ;
      SetScrollPos(iScrollPos+iDiff);
   }

   
   
}

//  Adds a paragraph to the page. The block is line feeded
void TextPageUI::AddText(std::string str,DWORD textColor,bool bNewEntry)
{
   int z = 0;
   EnterCriticalSection(&m_crLock);

   //Si une nouvelle entree on ajoute au logs...
   if(bNewEntry)
   {
      //backup en texte la liste des entrees pour reconstruire en cas de resize...
      sListBackup newEntry;
      newEntry.strInput = str;
      newEntry.dwColor  = textColor;
      m_vlAllInputString.push_back(newEntry);
      if(m_vlAllInputString.size() > g_iMaxLoggedLinesBackup)
         m_vlAllInputString.erase(m_vlAllInputString.begin());
   }
   



   int iNbrOldLine = m_ListText.size();

   //Format la nouvel entrree en images texte...
   //une ligne de texte peu prendre X ligne deependant le nombre de caractere et la largeur a dessiner
   FormatText fmtNewText;
   fmtNewText.SetText(str.c_str());
   fmtNewText.Format(m_textFrame.Width(),m_TempText->GetFont(),false);
   for(int i=0;i<fmtNewText.GetnbLine();i++)
   {
      ChatLineInfo p;
      p.text      = fmtNewText.GetText(i);
      p.textColor = textColor;
      m_ListText.push_back(p);
   }

    
    // If the backlog is getting too big
    if( m_ListText.size() > g_iMaxLoggedLines )
    {
        int iScrollNewPos = m_ListText.size()-g_iRemovedLines;
        iNbrOldLine -=g_iRemovedLines;
        m_ListText.erase(m_ListText.begin(),m_ListText.begin()+g_iRemovedLines);
        ScrollDown(g_iRemovedLines); // Scroll down to keep the current view insync.
    } 
    
    // Setup the new list size.
    int prevListSize = iNbrOldLine - m_iMaxLines;
    if( prevListSize < 0 )
        prevListSize = 0;
   
    int listSize = m_ListText.size() - m_iMaxLines + 1;
    if( listSize < 0 ) 
        listSize = 0;
    
    SetListSize( listSize );

    // If the scroll position pointed to the end of the previous text area.
    if( linePos == (unsigned long)prevListSize )
        ScrollUp( m_ListText.size() );  // Scroll down to the end of the text area.

    LeaveCriticalSection(&m_crLock);
}

void TextPageUI::ClearList()
{
   EnterCriticalSection(&m_crLock);
   m_ListText.clear();
   m_vlAllInputString.clear();
   ScrollDown( 0 );
   LeaveCriticalSection(&m_crLock);
}

void TextPageUI::ReconstructList()
{
   EnterCriticalSection(&m_crLock);
   m_ListText.clear();
   ScrollDown( 0 );

   list< sListBackup >::iterator itB = m_vlAllInputString.begin();
   list< sListBackup >::iterator itE = m_vlAllInputString.end();
   for(itB ; itB!=itE ; itB++)
      AddText(itB->strInput.c_str(),itB->dwColor,false);
   LeaveCriticalSection(&m_crLock);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TextPageUI::SetTextArea(FramePos itextFrame,bool bChangeScroll)
{
    m_textFrame = itextFrame;

    UpdateViewSize();
    
    //Update les control pour dessiner les texte...
    for(int i=0;i<m_iMaxLines;i++)
    {
       m_ListDrawST[i].SetPos ( FramePos( m_textFrame.x , m_textFrame.y +(i*m_iFontSize) , m_textFrame.maxx, m_textFrame.y +(i*m_iFontSize)+ m_iFontSize ) );
    }

    int listSize = m_ListText.size() - m_iMaxLines + 1;
    if( listSize < 0 )
        listSize = 0;

    SetListSize( listSize );

    if(bChangeScroll)
       ScrollUp( m_ListText.size() );
};
 
 
//  Updates the viewing size. Does not keep data from the previous screenText buffer.
void TextPageUI::UpdateViewSize( void )
{
    m_iFontSize = m_TempText->GetFontSize();
    if(m_iFontSize == 0)
       m_iFontSize = 1;

   m_iMaxLines = m_textFrame.Height() / m_iFontSize;

}



bool TextPageUI::WheelUp(MousePos mousePos)
{
   ScrollUp( 1 );
   return true;
}

bool TextPageUI::WheelDown(MousePos mousePos)
{
   ScrollDown( 1 );
   return true;
}

bool TextPageUI::RightClick( MousePos mousePos )
{
   return true;
}

bool TextPageUI::LeftMouseDown( MousePos mousePos )
{
   ScrollClick( mousePos );
   return true;
}

bool TextPageUI::LeftMouseUp(MousePos mousePos)
{
   return true;
}

void TextPageUI::SetPos(FramePos framePos)
{
   GameUI::SetPos( framePos );
}

bool TextPageUI::DragCycle(MousePos mousePos)
{
   LeftMouseUp( mousePos );
   return true;
}

void TextPageUI::ScrollChanged( void )
{
}




void TextPageUI::Draw(V2SPRITEFX *vsfFX)
{

   EnterCriticalSection(&m_crLock);
   DrawScroll( vsfFX );
   
   // Setup the clipping rectangle
   
   RECT rect = { m_textFrame.x, m_textFrame.y, m_textFrame.maxx + 1, m_textFrame.maxy };
   V2SPRITEFX fx;
   memset( &fx, 0, sizeof( fx ) );
   fx.lpClipRect = &rect;
   fx.dwFX |= FX_NOCORRECTION;


   
   for(UINT i=linePos,im=0;i<linePos+m_iMaxLines;i++,im++)
   {
      if(i < m_ListText.size() && im <100)
      {
         m_ListDrawST[im].SetText(m_ListText[i].text.c_str(),m_ListText[i].textColor);
         m_ListDrawST[im].Draw(&fx);
      }
   }
   LeaveCriticalSection(&m_crLock);
}