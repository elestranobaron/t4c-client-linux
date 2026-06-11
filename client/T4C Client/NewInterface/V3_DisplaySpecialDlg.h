// V3_DisplaySpecialDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"


const int iNBR_LETTER_PAGE  =  3; //2 map + carte laby
const int iNBR_MAP_PAGE     =  4;


class V3_DisplaySpecialDlg : public BoxUI  
{
public:	
	virtual ~V3_DisplaySpecialDlg();


    static V3_DisplaySpecialDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    int TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise);
    void Hide();

    void ShowLoadedBmp(BYTE *pData, BYTE* pPal,int dwW, int dwH, char* pStrName);
    void ShowLetter(int iletterIndex, char* pStrName);
    void ShowMaps();

        

protected:


private:
   // List containers.
    

protected:

   int  m_iXPos;
   int  m_iYPos;
   bool m_bDataLoaded;
   int  m_iDisplayLetter;
   int  m_iDisplayMap;

   char m_strTitle[1024];


   
   
public:
   ButtonUI  Drag_Button;


private:
   V3_DisplaySpecialDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   CV2Sprite m_sprBack;
   CV2Sprite m_sprBackM;
   GraphUI  m_TitleIcon;
   GraphUI  m_LetterPage[iNBR_LETTER_PAGE];
   GraphUI  m_MapPage[iNBR_MAP_PAGE];
   
   
   ButtonUI  X_Button;
   ButtonUI  N_Button;
   ButtonUI  P_Button;


   class PrevButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } prevButtonEvent;
   
   class NextButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } nextButtonEvent;

  
   friend PrevButtonEvent;
   friend NextButtonEvent;
};


