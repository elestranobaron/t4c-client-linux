// V3_AskValueSldDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "..\App.h"
#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "EditUI.h"
#include "SliderUI.h"
#include "StaticTextUI.h"


class V3_AskValueSldDlg : public BoxUI  
{
public:	
   virtual ~V3_AskValueSldDlg();
   
   static V3_AskValueSldDlg *GetInstance();
   
   virtual void Draw( V2SPRITEFX *vsfFX );
   virtual bool VKeyInput( VKey vKey );
   void Hide();
   void HideNoCB();
   void Show(UINT uiShowCodeP,int iDefaultVal);
   void SetParent(BoxUI *pParent);
   void SetXYPos(int iX,int iY);
   void SetDialogText(std::string strTagName, int iMaxLenght,int iHelpID,int iDefaultVal = 1); 
   void SetButtonText( std::string strOK, std::string strCancel,int iOKHelpID,int iCancelHelpID); 
   void SetDelay(DWORD dwDelay_ms);
   
   class OkEvent : public EventVisitor
   {
      public:	
         void LeftClicked();
   } okEvent;
   friend OkEvent;

   class CancelEvent : public EventVisitor
   {
   public:	
      void LeftClicked();
   } cancelEvent;
   friend CancelEvent;


   class SliderUpdateQtyEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } sliderUpdateQtyEvent;
   friend SliderUpdateQtyEvent;

   class AmountEditFilter : public EditUI::Filter
   {
   public:
      bool IsOK( char nextCh, const std::string curText );
   } amountEditFilter;

     

protected:


private:
   BoxUI *m_pParent;
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   UINT m_uiShowCodeP;
   int  m_iMaxQty;
   DWORD dwDelayUntill;


private:
   V3_AskValueSldDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);

   bool m_bUseVKey;
   
   GraphUI       m_BackgroundS;
   GraphUI       m_BackAsk;
   EditUI        m_editValue;
   SliderUI      m_SldValue;
   ButtonUI      m_btnCancel;
   ButtonUI      m_btnOK;
   StaticTextUI  m_stTag;
};


