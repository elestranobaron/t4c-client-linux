#pragma once

#include "App.h"
#include "NewFont.h"
#include "V2Sprite.h"

typedef struct _sHyperLink
{
   std::string strLinkMsg;
   std::string strLinkKey;
   int iX1;
   int iY1;
   int iX2;
   int iY2;
   DWORD dwTalkID;
}sHyperLink;

class CDisplayTextBox
{
public:
   friend FontManager;

   CDisplayTextBox(void);
   ~CDisplayTextBox(void);

   void SetText(char *pstrTxt,DWORD dwColor,BOOL bEnableHyperLink,DWORD dwTalkToID,char *pstrMod);
   int  GetNblines(){return m_iNBLines;}
   bool IsEmpty(){return m_Text.empty();}
   LPCTSTR GetText( void ){return m_Text.c_str();}
   int  GetHeight(void);
   int  GetWidth(void);
   int  GetWidthTO(LPCTSTR s = NULL);
   void Draw(int x, int y, V2SPRITEFX * = NULL);
   bool ProcessHyperLink(DWORD dwTalkPNJID,int imX, int imY, char *pstrLink, DWORD &dwID);
   
protected:
   void CreateTextBox();
   void TextColor(HDC, const char *text, int xPos, int yPos, DWORD &currentColor, 
                  DWORD &newColor, bool skipColorCoding, bool transparentBackground);

protected:

   CV2Sprite   m_ImageBackTL;
   CV2Sprite   m_ImageBackTR;
   CV2Sprite   m_ImageBackBL;
   CV2Sprite   m_ImageBackBR;
   CV2Sprite   m_ImageBackTLM;
   CV2Sprite   m_ImageBackTRM;
   CV2Sprite   m_ImageBackBLM;
   CV2Sprite   m_ImageBackBRM;
   Sprite      m_TextBack;
   FontObject *m_pFont;

   std::string m_Text;
   std::string m_TextOri;
   DWORD       m_dwColor;
   bool        m_BlackBackground;
   DWORD       m_dwFlag;
   int         m_iNBLines;
   int         m_iFontHeight;
   BOOL        m_bEnableHyperLink;
   std::string m_LastSelectedLink;
   int         m_iLastDrawX;
   int         m_iLastDrawY;
   DWORD       m_dwTalkToID;

   std::vector<sHyperLink>  m_vHyperLink;
   std::vector<std::string> m_vHyperLinkKey;


};
