// V3_AHVendreDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "EditUI.h"
#include "SliderUI.h"
#include "StaticTextUI.h"
#include "V3_InvDlg.h"




class V3_AHVendreDlg : public BoxUI  
{
public:	
	virtual ~V3_AHVendreDlg();


    static V3_AHVendreDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    virtual bool TextInput( char ch );
    virtual bool VKeyInput( VKey vKey );
    void Hide();

        

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
   // List containers.
    

protected:
   typedef V3_InvDlg::ItemUI ItemUI;


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;

   int  m_iLastItemID;

   BOOL m_bNMSOptions;

     



private:
   
   friend V3_InvDlg::InventoryGridEvent;


   V3_AHVendreDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;


   GridUI	     *inventoryGrid;
   GraphUI       *invGridScrollUpGraph;
   GraphUI       *invGridScrollDownGraph;
   ButtonUI      *iconGold;
   StaticTextUI  *statGold;
   StaticTextUI  *instantItemInfo;


   ButtonUI     m_btnPutItemHere;
   StaticTextUI m_stPutItemHere;



   ButtonUI okButton;
   ButtonUI cancelButton;
   SliderUI QtySlider;
   SliderUI DelSlider;
   EditUI   QtyEdit;
   EditUI   BuyEdit;
   EditUI   BuyNMSEdit;
   EditUI   BidEdit;
   EditUI   DelEdit;
   StaticTextUI textAreaQty;
   StaticTextUI textAreaBuy;
   StaticTextUI textAreaBid;
   StaticTextUI textAreaDel;
   StaticTextUI textAreaBuyOr;
   StaticTextUI textAreaBuyEcu;



   class PutItemHereEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } putItemHereEvent;  
   friend PutItemHereEvent;


   class OkButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
	} okButtonEvent;

	class CancelButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );        
   } cancelButtonEvent;
   
   
   class SliderQtyEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } sliderQtyEvent;
   
   class SliderDelEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } sliderDelEvent;
   
   
   friend OkButtonEvent;
   friend CancelButtonEvent;
   friend SliderQtyEvent;
   friend SliderDelEvent;


   // Text filter, only accepts numbers.
	class AmountEditFilter : public EditUI::Filter{
   public:
      bool IsOK( char nextCh, const std::string curText );
   } amountEditFilter;

};


