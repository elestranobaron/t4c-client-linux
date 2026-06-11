// V3_SmileDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "..\App.h"
#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "StaticTextUI.h"
#include "ListUI.h"


class V3_SmileDlg : public BoxUI  
{
public:	
   virtual ~V3_SmileDlg();
   
   static V3_SmileDlg *GetInstance();
   
   virtual void Draw( V2SPRITEFX *vsfFX );
   void Hide();
   void Show();
   void SetParent(BoxUI *pParent){m_pParent = pParent;}
   void SetXYPos(int iX,int iY);

protected:

private:
   BoxUI *m_pParent;
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   UINT m_SelectedSmile;


private:
   V3_SmileDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   ButtonUI cbXButton;

   ButtonUI btnSmilex01y01;
   ButtonUI btnSmilex02y01;
   ButtonUI btnSmilex03y01;
   ButtonUI btnSmilex04y01;
   ButtonUI btnSmilex05y01;
   ButtonUI btnSmilex06y01;
   ButtonUI btnSmilex07y01;
   ButtonUI btnSmilex08y01;
   ButtonUI btnSmilex09y01;
   ButtonUI btnSmilex01y02;
   ButtonUI btnSmilex02y02;
   ButtonUI btnSmilex03y02;
   ButtonUI btnSmilex04y02;
   ButtonUI btnSmilex05y02;
   ButtonUI btnSmilex06y02;
   ButtonUI btnSmilex07y02;
   ButtonUI btnSmilex08y02;
   ButtonUI btnSmilex09y02;
   ButtonUI btnSmilex01y03;
   ButtonUI btnSmilex02y03;
   ButtonUI btnSmilex03y03;
   ButtonUI btnSmilex04y03;
   ButtonUI btnSmilex05y03;
   ButtonUI btnSmilex06y03;
   ButtonUI btnSmilex07y03;
   ButtonUI btnSmilex08y03;
   ButtonUI btnSmilex09y03;

   StaticTextUI statNom;



   class Smilex01y01Event : public EventVisitor{ public: void LeftClicked( void );} smilex01y01Event;
   class Smilex02y01Event : public EventVisitor{ public: void LeftClicked( void );} smilex02y01Event;
   class Smilex03y01Event : public EventVisitor{ public: void LeftClicked( void );} smilex03y01Event;
   class Smilex04y01Event : public EventVisitor{ public: void LeftClicked( void );} smilex04y01Event;
   class Smilex05y01Event : public EventVisitor{ public: void LeftClicked( void );} smilex05y01Event;
   class Smilex06y01Event : public EventVisitor{ public: void LeftClicked( void );} smilex06y01Event;
   class Smilex07y01Event : public EventVisitor{ public: void LeftClicked( void );} smilex07y01Event;
   class Smilex08y01Event : public EventVisitor{ public: void LeftClicked( void );} smilex08y01Event;
   class Smilex09y01Event : public EventVisitor{ public: void LeftClicked( void );} smilex09y01Event;
   class Smilex01y02Event : public EventVisitor{ public: void LeftClicked( void );} smilex01y02Event;
   class Smilex02y02Event : public EventVisitor{ public: void LeftClicked( void );} smilex02y02Event;
   class Smilex03y02Event : public EventVisitor{ public: void LeftClicked( void );} smilex03y02Event;
   class Smilex04y02Event : public EventVisitor{ public: void LeftClicked( void );} smilex04y02Event;
   class Smilex05y02Event : public EventVisitor{ public: void LeftClicked( void );} smilex05y02Event;
   class Smilex06y02Event : public EventVisitor{ public: void LeftClicked( void );} smilex06y02Event;
   class Smilex07y02Event : public EventVisitor{ public: void LeftClicked( void );} smilex07y02Event;
   class Smilex08y02Event : public EventVisitor{ public: void LeftClicked( void );} smilex08y02Event;
   class Smilex09y02Event : public EventVisitor{ public: void LeftClicked( void );} smilex09y02Event;
   class Smilex01y03Event : public EventVisitor{ public: void LeftClicked( void );} smilex01y03Event;
   class Smilex02y03Event : public EventVisitor{ public: void LeftClicked( void );} smilex02y03Event;
   class Smilex03y03Event : public EventVisitor{ public: void LeftClicked( void );} smilex03y03Event;
   class Smilex04y03Event : public EventVisitor{ public: void LeftClicked( void );} smilex04y03Event;
   class Smilex05y03Event : public EventVisitor{ public: void LeftClicked( void );} smilex05y03Event;
   class Smilex06y03Event : public EventVisitor{ public: void LeftClicked( void );} smilex06y03Event;
   class Smilex07y03Event : public EventVisitor{ public: void LeftClicked( void );} smilex07y03Event;
   class Smilex08y03Event : public EventVisitor{ public: void LeftClicked( void );} smilex08y03Event;
   class Smilex09y03Event : public EventVisitor{ public: void LeftClicked( void );} smilex09y03Event;
   
   
   friend Smilex01y01Event;
   friend Smilex02y01Event;
   friend Smilex03y01Event;
   friend Smilex04y01Event;
   friend Smilex05y01Event;
   friend Smilex06y01Event;
   friend Smilex07y01Event;
   friend Smilex08y01Event;
   friend Smilex09y01Event;
   friend Smilex01y02Event;
   friend Smilex02y02Event;
   friend Smilex03y02Event;
   friend Smilex04y02Event;
   friend Smilex05y02Event;
   friend Smilex06y02Event;
   friend Smilex07y02Event;
   friend Smilex08y02Event;
   friend Smilex09y02Event;
   friend Smilex01y03Event;
   friend Smilex02y03Event;
   friend Smilex03y03Event;
   friend Smilex04y03Event;
   friend Smilex05y03Event;
   friend Smilex06y03Event;
   friend Smilex07y03Event;
   friend Smilex08y03Event;
   friend Smilex09y03Event;

   class XEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } xEvent;
   friend XEvent;
};


