// V3_ConfirmDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "..\App.h"
#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "StaticTextUI.h"


class V3_ConfirmDlg : public BoxUI  
{
public:	
   virtual ~V3_ConfirmDlg();
   
   static V3_ConfirmDlg *GetInstance();
   
   virtual void Draw( V2SPRITEFX *vsfFX );
   virtual bool VKeyInput( VKey vKey );
   void Hide();
   void HideNoCB();
   void Show(UINT uiShowCodeP);
   void SetParent(BoxUI *pParent){m_pParent = pParent;}
   void SetXYPos(int iX,int iY);
   void SetDialogText( std::string strTagName); 
   void SetButtonText( std::string strOK, std::string strCancel,int iOKHelpID,int iCancelHelpID); 
   
   class OkEvent : public EventVisitor
   {
      public:	
         void LeftClicked();
   } okEvent;
   friend OkEvent;

     

protected:


private:
   BoxUI *m_pParent;
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   UINT m_uiShowCodeP;


private:
   V3_ConfirmDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   ButtonUI m_btnCancel;
   ButtonUI m_btnOK;
   StaticTextUI  m_stTag;
};


