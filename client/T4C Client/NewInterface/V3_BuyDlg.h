// V3_BuyDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"




class V3_BuyDlg : public BoxUI  
{
public:	
	virtual ~V3_BuyDlg();


    static V3_BuyDlg *GetInstance();
    static unsigned int Display( void *null ,UINT uiWhat);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Show();

    struct SellItemInfo
    {
       SellItemInfo() : dwID( 0 ), appearance( 0 ), price( 0 ), qty( 0 )
       {
       }
       
       DWORD  dwID;
       WORD   appearance;
       DWORD  price;
       string name;
       string reqs;
       DWORD  qty;
       bool   canEquip;
       DWORD  maxQty;
    };

    void Buy( const std::list< SellItemInfo > &sellItemList );
    void Sell( const std::list< SellItemInfo > &sellItemList );
    void Echanger( const std::list< SellItemInfo > &sellItemList ,unsigned long ulPoints);

    UINT m_uiBuyMode;
     

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
   // List containers.
   
public:
   ButtonUI  Drag_Button;

protected:


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   unsigned long m_ulCredits;





private:
   V3_BuyDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  X_Button;
   

 
   ButtonUI  m_btnTrade;
   ButtonUI  m_btnBuy;
   ButtonUI  m_btnSell;
   ListUI    m_ListItem;
   GraphUI   selectionSprite;
   GraphUI   listArrowUp;
   GraphUI   listArrowDown;

   StaticTextUI m_stHCol1;
   StaticTextUI m_stHCol2;
   StaticTextUI m_stHCol3;

   StaticTextUI m_stHOr;
   StaticTextUI m_stHReq;
   StaticTextUI m_stHTot;
   StaticTextUI m_stHLeft;
   StaticTextUI m_stReq;
   StaticTextUI m_stTot;
   StaticTextUI m_stLeft;

   class SellItemUI : public GraphUI{
   public:
      SellItemInfo itemInfo;
    };

   class BuyButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } buyButtonEvent;
   
   class ItemListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
      void LeftDblClicked();
   } itemListEvent;
   
   friend ItemListEvent;
   friend BuyButtonEvent;
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

};


