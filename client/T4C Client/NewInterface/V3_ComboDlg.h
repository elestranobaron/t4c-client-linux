// V3_ComboDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "..\App.h"
#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "StaticTextUI.h"
#include "ListUI.h"


class V3_ComboDlg : public BoxUI  
{
public:	
   virtual ~V3_ComboDlg();
   
   static V3_ComboDlg *GetInstance();
   
   virtual void Draw( V2SPRITEFX *vsfFX );
   void Hide();
   void Show(UINT uiShowCodeP,int iScrollPos);
   void SetParent(BoxUI *pParent){m_pParent = pParent;}
   void SetXYPos(int iX,int iY);
   void ClearCombo();
   void AddComboItem(std::string strName); 
   

     

protected:


private:
   BoxUI *m_pParent;
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   UINT m_uiShowCodeP;


private:
   V3_ComboDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   ListUI   cbList;
   GraphUI  cbListSelect;
   GraphUI  cbScrollUp;
   GraphUI  cbScrollDown;
   ButtonUI cbXButton;


   class CbListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } cbListEvent;
   friend CbListEvent;

   class XEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } xEvent;
   
   friend XEvent;


   class ComboDlgUI : public StaticTextUI{
   public:
      std::string strName;
   };

};


