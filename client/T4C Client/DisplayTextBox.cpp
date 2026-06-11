#include "DisplayTextBox.h"
#include "FormatText.h"
#include "Mouse.h"
#include "MouseAction.h"
#include "MacroHandler.h"

const int g_NbrMaxLine    = 14;
const int g_MaxWidth      = 260;
const int g_OffsetBackBox  = 6;
const int g_OffsetBackBoxT = 2*g_OffsetBackBox;
 
extern UserGeneralConfig Custom;

bool isemptyDB(const char *s) {
   int l = strlen(s);
   for (int i = 0; i < l; i++)
      if (s[i] != ' ')
         return false;
   return true;
}

CDisplayTextBox::CDisplayTextBox(void)
{
   m_pFont = FontManager::GetInstance()->GetDisplayTextFont();
   m_ImageBackTL.LoadSprite( "V3_DialogBoxBack");
   m_ImageBackBL.LoadSprite( "V3_DialogBoxBackBottom");
   m_ImageBackTR.LoadSprite( "V3_DialogBoxBackRight");
   m_ImageBackBR.LoadSprite( "V3_DialogBoxBackBottomRight");
   m_ImageBackTLM.LoadSprite( "V3_DialogBoxBackMask");
   m_ImageBackBLM.LoadSprite( "V3_DialogBoxBackBottomMask");
   m_ImageBackTRM.LoadSprite( "V3_DialogBoxBackRightMask");
   m_ImageBackBRM.LoadSprite( "V3_DialogBoxBackBottomRightMask");
   
   m_Text            = "";
   m_TextOri         = "";
   m_LastSelectedLink= "";
   m_dwColor         = 0;
   m_BlackBackground = false;
   m_dwFlag          =  TO_DEFAULT;
   m_iNBLines        = 0;
   m_iLastDrawX      = 0;
   m_iLastDrawY      = 0;
}

CDisplayTextBox::~CDisplayTextBox(void)
{
}

void CDisplayTextBox::SetText(char *pstrTxt,DWORD dwColor,BOOL bEnableHyperLink,DWORD dwTalkToID,char *pstrMod)
{
   
   m_bEnableHyperLink = bEnableHyperLink;
   m_dwTalkToID       = dwTalkToID;
   m_vHyperLink.clear();
   m_vHyperLinkKey.clear();
   std::vector<std::string> vRep;
   std::vector<std::string> vRepMod;
   if (pstrTxt && pstrTxt[0] && !isemptyDB(pstrTxt)) 
   {
      char strProcess[2048];
      sprintf_s(strProcess,2048,"%s",pstrTxt);
      m_TextOri = strProcess;

      //On process al nouvelle methode...
      //Hé ! Vous ! J'ai besoin d'aide ! @Je vais vous "aider"@ @Cours toujours, "adieu" !@
      //On extrait tout ce qui ets entre @@ et on y associe un mot clef qui est entre ""
      char strRepR[1000];
      char strRepK[1000];
      char strRepRep[1000];

      int iPHEnd = 0;
      int iSR = 0;
      int iSK = 0;
      int iSRep = 0;
      bool bStartR = false;
      bool bStartK = false;
      for(UINT i=0;i<strlen(strProcess);i++)
      {
         if(!bStartR && strProcess[i] == '@')
         {
            //Start la Reponse
            if(iPHEnd == 0)
               iPHEnd = i-1;
            iSR = 0;
            iSRep = 0;
            bStartR = true;
         }
         else if(bStartR && strProcess[i] == '@')
         {
            //stop the reponse
            strRepR[iSR] = 0x00;
            strRepRep[iSRep] = 0x00;
            vRep.push_back(strRepR);
            vRepMod.push_back(strRepRep);
            
            bStartR = false;
         }
         else if(bStartR)
         {
            strRepRep[iSRep++] = strProcess[i];
            if(!bStartK && strProcess[i] == '"')
            {
               //Start la key
               iSK = 0;
               bStartK = true;
            }
            else if(bStartK && strProcess[i] == '"')
            {
               //STOP la key
               strRepK[iSK] = 0x00;
               m_vHyperLinkKey.push_back(strRepK);
               bStartK = false;
            }
            else if(bStartK)
            {
               strRepR[iSR++] = strProcess[i];
               strRepK[iSK++] = strProcess[i];
            }
            else
            {
               strRepR[iSR++] = strProcess[i];
            }
         }
      }

      if(iPHEnd >0 && m_vHyperLinkKey.size() >0 && vRep.size()>0 && m_vHyperLinkKey.size() == vRep.size())
      {
         //coupe les reponse du texte
         strProcess[iPHEnd] = 0x00;
      }






      
      if(m_dwTalkToID > 0 && iPHEnd == 0)
      {

         char strTmp[2048];
         if(Custom.iLanguageID == 0) //english
            sprintf_s(strTmp,2048,"%s <> <>\"> Bye.\"",strProcess);
         else if(Custom.iLanguageID == 1) //Francais
            sprintf_s(strTmp,2048,"%s <> <>\"> Au revoir.\"",strProcess);
         else
            sprintf_s(strTmp,2048,"%s <> <>\"> Bye.\"",strProcess);

         m_Text    = strTmp;
      }
      else
      {
         if(iPHEnd > 0)
         {
            char strTmp[2048];
            char strTmpR[2048];
            char strTmp2[2048];
            sprintf_s(strTmp,"%s",strProcess);
            sprintf_s(strTmpR,"%s",strProcess);
            
            for( UINT k=0;k<vRep.size();k++)
            {
               if(k == 0)
                  sprintf_s(strTmp2,2048," <> <>\"> %s\" ",vRep[k].c_str());
               else
                  sprintf_s(strTmp2,2048," <>\"> %s\" ",vRep[k].c_str());
               strcat_s(strTmp,2048,strTmp2);

               sprintf_s(strTmp2,2048," <>> %s",vRepMod[k].c_str());
               strcat_s(strTmpR,2048,strTmp2);
              
            }
            m_Text    = strTmp;

            if(pstrMod)
               sprintf_s(pstrMod,2048,"%s",strTmpR);
         }
         else 
            m_Text    = strProcess;
      }

      m_dwColor = dwColor;
      //force color NMS
      if(m_dwTalkToID != 0) //NPC
         m_dwColor = CL_DISPLAY_BOX_WHITE;
      else
         m_dwColor = CL_DISPLAY_BOX_WHITE;

      CreateTextBox();
   }
   else
   {
      //reset text
      m_Text    = "";
      m_TextOri = "";
      m_LastSelectedLink= "";
   }
}

void CDisplayTextBox::CreateTextBox()
{
   FormatText ft;
   ft.SetText(m_Text.c_str());
   ft.Format(g_MaxWidth, m_pFont, false);

   HDC hdc;
   int iW,iH,iNbLine;

   DXDGetDC(&hdc, 6);
   iH = (int)m_pFont->GetFHeight(hdc);
   iW = (int)m_pFont->GetFWidth(hdc, ft.GetText(0));

   m_iFontHeight = iH;

   iNbLine = ft.GetnbLine();
   if(iNbLine > g_NbrMaxLine)
      iNbLine = g_NbrMaxLine;

   m_iNBLines = iNbLine;
   for (int i=0; i<iNbLine; i++)  
   {
      int iT = (int)m_pFont->GetFWidth(hdc, ft.GetText(i));
      if (iW < iT)
         iW = iT;
   }

   DXDReleaseDC(hdc, 6);

   int spriteHeight = (iH* iNbLine)+g_OffsetBackBoxT;
   iW +=g_OffsetBackBoxT;

   m_TextBack.Release();
   m_TextBack.Create(iW, spriteHeight, !m_BlackBackground, !m_BlackBackground ? ((lpDXDirectDraw->wGMask >> 1) & lpDXDirectDraw->wGMask) + ((lpDXDirectDraw->wRMask >> 1) & lpDXDirectDraw->wRMask) : 0);

   DWORD CurrentColor = m_dwColor;
   DWORD NewColor     = m_dwColor;

   // Draw the Text
   if(m_TextBack.GetSurface())
   {
      m_TextBack.GetSpriteDC(&hdc);

      for (int i=0; i<iNbLine; i++) 
      {
         TextColor(hdc, ft.GetText(i), g_OffsetBackBox, g_OffsetBackBox+(i*(iH)), CurrentColor, NewColor, false, false);
      }
   
      m_TextBack.ReleaseSpriteDC(hdc);
   }


}


void CDisplayTextBox::TextColor(HDC hdc,const char *s,int xPos,int yPos,DWORD &CurrentColor,
                                DWORD &NewColor,bool skipColorCoding,bool transparentBackground)
{
   char Text[2048];
   char *ptr;
   UINT j;
   bool bDraw = false;   

   SelectObject(hdc, m_pFont->GetHFONT());
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, CurrentColor);

   ptr = Text;
   strcpy_s(Text,2048, s);
   DWORD colorSwitch = 0;

   for (j = 0; j < strlen(s); j++) 
   {
      switch (Text[j]) 
      {
        case '{':
           if (!(m_dwFlag & TO_DRAW_QUOTES)) 
           {
              Text[j] = 0;
              if( !skipColorCoding )
                 NewColor = CL_DISPLAY_BOX_GREY;
              bDraw = true;
           }
        break;
        case '}':
           if (!(m_dwFlag & TO_DRAW_QUOTES)) 
           {
              Text[j] = 0;
              if( !skipColorCoding )
                 NewColor = m_dwColor;
              bDraw = true;
           }
        break;
        case '\"':
           if (!(m_dwFlag & TO_DRAW_QUOTES)) 
           {
              colorSwitch++;
              Text[j] = 0;
              if( !skipColorCoding )
              {
                 if (colorSwitch & 1)
                    NewColor = CL_DISPLAY_BOX_LINK;
                 else
                    NewColor = m_dwColor;
              }
              bDraw = true;
           }
        break;
      }  
      if (bDraw || j == (strlen(s)-1)) 
      {
         int iOldX = xPos;
         DWORD dwOldColor = CurrentColor;

         bDraw = false;
         char szTemp[2048];
         sprintf_s(szTemp,2048, "|%s|", ptr);
         std::string strWorld = ptr;

         if(ptr == m_LastSelectedLink)
            CurrentColor = CL_DISPLAY_BOX_LINKO;

         SetTextColor(hdc, CurrentColor);
         ::TextOut(hdc, xPos, yPos, ptr, strlen(ptr));

         if( !skipColorCoding )
            CurrentColor = NewColor;

         
         xPos = xPos+ (int)m_pFont->GetFWidth(hdc, ptr);

         if((dwOldColor == CL_DISPLAY_BOX_LINK || dwOldColor == CL_DISPLAY_BOX_LINKO) && m_bEnableHyperLink)
         {
            sHyperLink newLink;
            newLink.strLinkMsg = strWorld;
            newLink.iX1        = iOldX;
            newLink.iY1        = yPos;
            newLink.iX2        = xPos;
            newLink.iY2        = yPos+m_iFontHeight;
            newLink.dwTalkID   = m_dwTalkToID;  
            if(m_vHyperLinkKey.size() >  m_vHyperLink.size())
            {
               int iOffsetK = m_vHyperLink.size();
               if(iOffsetK < 0)
                  iOffsetK = 0;
               if(iOffsetK >=0 && iOffsetK < m_vHyperLinkKey.size())
                  newLink.strLinkKey = m_vHyperLinkKey[iOffsetK];
               else
                  newLink.strLinkKey = strWorld;

               m_vHyperLink.push_back(newLink);
            }
            else
            {
              
               newLink.strLinkKey = strWorld;
               m_vHyperLink.push_back(newLink);
            }
            //char strTmp[1024];
            //sprintf_s(strTmp,1024,"%s   (%d,%d)  (%d,%d)\n",szTemp2,iOldX,yPos,xPos,yPos+m_iFontHeight);
            //OutputDebugString(strTmp);
         }
         

         ptr = &Text[j+1];
      }
   }
}

int CDisplayTextBox::GetHeight(void) 
{
   return m_TextBack.GetHeight();
}

int CDisplayTextBox::GetWidth(void) 
{
   return m_TextBack.GetWidth();
}

int CDisplayTextBox::GetWidthTO(LPCTSTR s) 
{
   if( s != NULL )
   {
      HDC hdc;
      DXDGetDC(&hdc, 9);
      int R = (int)m_pFont->GetFWidth( hdc, const_cast< char * >( s ) );
      DXDReleaseDC(hdc, 9);
      return R;
   }
   return m_TextBack.GetWidth();
}
                                         
void CDisplayTextBox::Draw(int x, int y, V2SPRITEFX *fx) 
{
   if( m_Text.empty() ){
      return;
   }

   m_iLastDrawX = x;
   m_iLastDrawY = y;

   RECT r = {0, 0, g_Global.GetScreenW(), g_Global.GetScreenH()};
   if (fx) 
   {
      r.left   = fx->lpClipRect->left;
      r.top    = fx->lpClipRect->top;
      r.right  = fx->lpClipRect->right;
      r.bottom = fx->lpClipRect->bottom;
   }
   // Out of the Clipping Border.
   if (x > r.right)         return;
   if (y > r.bottom)        return;
   if (x + m_TextBack.GetWidth() < r.left) return;
   if (y + m_TextBack.GetWidth() < r.top) return;

   RECT rectTL  = { x, y, x+GetWidth()-4 , y+GetHeight()-4 };
   RECT rectBL  = { x, y+GetHeight()-4, x+GetWidth()-4 , y+GetHeight() };
   RECT rectTR  = { x+GetWidth()-4, y, x+GetWidth() , y+GetHeight()-4 };
   RECT rectBR  = { x+GetWidth()-4, y+GetHeight()-4, x+GetWidth() , y+GetHeight() };
   V2SPRITEFX fxB;
   memset( &fxB, 0, sizeof( fxB ) );
   fxB.dwFX |= FX_NOCORRECTION;

   if(rectTL.left < 0)
      rectTL.left = 0;
   if(rectTL.right > g_Global.GetScreenW())
      rectTL.right = g_Global.GetScreenW();
   if(rectTL.top < 0)
      rectTL.top = 0;
   if(rectTL.bottom > g_Global.GetScreenH())
      rectTL.bottom = g_Global.GetScreenH();

   if(rectBL.left < 0)
      rectBL.left = 0;
   if(rectBL.right > g_Global.GetScreenW())
      rectBL.right = g_Global.GetScreenW();
   if(rectBL.top < 0)
      rectBL.top = 0;
   if(rectBL.bottom > g_Global.GetScreenH())
      rectBL.bottom = g_Global.GetScreenH();

   if(rectTR.left < 0)
      rectTR.left = 0;
   if(rectTR.right > g_Global.GetScreenW())
      rectTR.right = g_Global.GetScreenW();
   if(rectTR.top < 0)
      rectTR.top = 0;
   if(rectTR.bottom > g_Global.GetScreenH())
      rectTR.bottom = g_Global.GetScreenH();

   if(rectBR.left < 0)
      rectBR.left = 0;
   if(rectBR.right > g_Global.GetScreenW())
      rectBR.right = g_Global.GetScreenW();
   if(rectBR.top < 0)
      rectBR.top = 0;
   if(rectBR.bottom > g_Global.GetScreenH())
      rectBR.bottom = g_Global.GetScreenH();


   fxB.lpClipRect = &rectTL;
   m_ImageBackTL.DrawSpriteN( x, y, &fxB,&m_ImageBackTLM);
   fxB.lpClipRect = &rectBL;
   m_ImageBackBL.DrawSpriteN( x, y+GetHeight()-4, &fxB,&m_ImageBackBLM);
   fxB.lpClipRect = &rectTR;
   m_ImageBackTR.DrawSpriteN( x+GetWidth()-4, y, &fxB,&m_ImageBackTRM);
   fxB.lpClipRect = &rectBR;
   m_ImageBackBR.DrawSpriteN( x+GetWidth()-4, y+GetHeight()-4, &fxB,&m_ImageBackBRM);



   m_TextBack.DrawSprite(x, y, &r);    
}

bool CDisplayTextBox::ProcessHyperLink(DWORD dwTalkPNJID,int imX, int imY, char *pstrLink, DWORD &dwID)
{
   if(dwTalkPNJID == 0 && m_dwTalkToID > 0)
   {
      //reset le text sans le adieu...
      m_dwTalkToID = 0;
      SetText((char*)m_TextOri.c_str(),m_dwColor,FALSE,m_dwTalkToID,NULL);
      return false;

   }
   int xp  = imX- m_iLastDrawX;
   int yp  = imY- m_iLastDrawY;

   bool bLink = FALSE;
   for(UINT i=0;i<m_vHyperLink.size();i++)
   {
      if(xp >= m_vHyperLink[i].iX1 && xp <= m_vHyperLink[i].iX2 &&
         yp >= m_vHyperLink[i].iY1 && yp <= m_vHyperLink[i].iY2    )
      {
         bLink = TRUE;
         //char strTmp[100];
         //sprintf_s(strTmp,100,"On Link %s\n",m_vHyperLink[i].strLink.c_str());
         //OutputDebugString(strTmp);

         if(m_LastSelectedLink != m_vHyperLink[i].strLinkMsg)
         {
            m_LastSelectedLink = m_vHyperLink[i].strLinkMsg;
            SetText((char*)m_TextOri.c_str(),m_dwColor,TRUE,m_dwTalkToID,NULL);
         }
         dwID = m_vHyperLink[i].dwTalkID;
         sprintf_s(pstrLink,1024,"%s",m_vHyperLink[i].strLinkKey.c_str());
         return true;
      }
   }
   if(!bLink && m_LastSelectedLink != "")
   {
      //plus sur un link
      m_LastSelectedLink = "";
      SetText((char*)m_TextOri.c_str(),m_dwColor,TRUE,m_dwTalkToID,NULL);
      return false;
   }
   return false;
}