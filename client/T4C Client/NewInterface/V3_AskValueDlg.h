// V3_AskValueDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "..\App.h"
#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "EditUI.h"
#include "StaticTextUI.h"


class V3_AskValueDlg : public BoxUI  
{
public:	
   virtual ~V3_AskValueDlg();
   
   static V3_AskValueDlg *GetInstance();
   
   virtual void Draw( V2SPRITEFX *vsfFX );
   virtual bool TextInput( char ch );
   virtual bool VKeyInput( VKey vKey );
   void Hide();
   void HideNoCB();
   void Show(UINT uiShowCodeP);
   void SetParent(BoxUI *pParent){m_pParent = pParent;}
   void SetXYPos(int iX,int iY);
   void SetDialogText1(bool bUseVKey, VKey vkValue,std::string strTagName, std::string strFileName,int iMaxLenght,int iHelpID); 
   void SetDialogText2( std::string strTagName, std::string strFileName,int iMaxLenght,int iHelpID); 
   void SetButtonText( std::string strOK, std::string strCancel,int iOKHelpID,int iCancelHelpID,bool bShowEffacer = false); 
   
   class OkEvent : public EventVisitor
   {
      public:	
         void LeftClicked();
   } okEvent;
   friend OkEvent;

   class EffacerEvent : public EventVisitor
   {
   public:	
      void LeftClicked();
   } effacerEvent;
   friend EffacerEvent;

     

protected:


private:
   BoxUI *m_pParent;
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   UINT m_uiShowCodeP;

   VKey vKey;


private:
   V3_AskValueDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);

   bool m_bUseAsk2;
   bool m_bEffacerBtn;
   bool m_bUseVKey;
   
   GraphUI  m_BackgroundS;
   GraphUI  m_BackAsk1;
   GraphUI  m_BackAsk2;
   EditUI   m_editValue1;
   EditUI   m_editValue2;
   ButtonUI m_btnCancel;
   ButtonUI m_btnOK;
   ButtonUI m_btnEffacer;
   StaticTextUI  m_stTag1;
   StaticTextUI  m_stTag2;
};


