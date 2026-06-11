// V3_AHDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"


#define EP_BODY         0         //eBody = 0
#define EP_FEET         1         //eFeet = 1
#define EP_GLOVE        2         //eGloves = 2
#define EP_HELM         3         //eHelm = 3
#define EP_LEG          4         //eLegs = 4
#define EP_FINGER       5         //eFinger = 5
#define EP_WRIST        6         //eWrist = 6
#define EP_NECK         7         //eNeck = 7
#define EP_WEAPON_RIGHT 8         //eWeaponRight = 8
#define EP_WEAPON_LEFT  9         //eWeaponLeft = 9
#define EP_WEAPON_TWO   10        //eTwoHanded = 10
#define EP_BELT         14        //eBelt = 14
#define EP_CAPE         15        //eCape = 15
#define EP_ORBE         16        //eOrbe = 16
#define EP_SEARCH       0xAAAA    //quand on recherche
#define EP_NONE         0xFFFF    //eNone = 0xFFFF


class V3_AHDlg : public BoxUI  
{
public:	
	virtual ~V3_AHDlg();


    static V3_AHDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();


    class ItemSold
    {
    public: 
       DWORD       dwOwnerID;
       std::string strName;
       DWORD       dwIndex;
       DWORD       dwQty;
       DWORD       dwTime;
       DWORD       dwBuy;
       DWORD       dwBuyNMS;
       DWORD       dwBid;
       DWORD       dwCurBid;
       DWORD       dwCurBidID;
       DWORD       dwEquipPos;
       DWORD       dwTimeEnter;
    };
    
   void UpdateItemList(DWORD dwPID,const std::vector< ItemSold > &ItemSold);
   void AHLock()    {  m_bLockDraw = true;  }
   void AHUnlock()  {  m_bLockDraw = false;  }
   bool IsDrawProgress(){return m_bDrawInProgress;}


   void AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey);
   void AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal);

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

   void UpdateSelectedEquip(int iID,bool bSearch = false, std::string strSearch = "");
   void ResetOngletSelect();
   void SetOngletSelect(int iID);

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

   bool m_bLockDraw;
   bool m_bDrawInProgress;
   int m_iEquipIdx[16];
   DWORD m_dwPlayerID;

   int m_dwCurrentOngletSelect;
   DWORD m_dwIndexItemBuy;
   DWORD m_dwTSItemBuy;

   BOOL  m_bNMSOptions;



private:
   V3_AHDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;
   


  
   ButtonUI  vendreButton;
   ButtonUI  buyNowButton;
   ButtonUI  buyNowNMSButton;
   ButtonUI  bidButton   ;
   ButtonUI  cancelButton;

   ListUI    SoldList;
   GraphUI   SoldListSelGraph;
   GraphUI   SoldListScrollUp;
   GraphUI   SoldListScrollDown;


   ButtonUI  m_Onglet00;
   ButtonUI  m_Onglet01;
   ButtonUI  m_Onglet02;
   ButtonUI  m_Onglet03;
   ButtonUI  m_Onglet04;
   ButtonUI  m_Onglet05;
   ButtonUI  m_Onglet06;
   ButtonUI  m_Onglet07;
   ButtonUI  m_Onglet08;
   ButtonUI  m_Onglet09;
   ButtonUI  m_Onglet10;
   ButtonUI  m_Onglet11;
   ButtonUI  m_Onglet12;
   ButtonUI  m_Onglet13;
   ButtonUI  m_Onglet14;
   ButtonUI  m_Onglet15;

   StaticTextUI m_stPlayerGold;

   StaticTextUI m_stCol1;
   StaticTextUI m_stCol2;
   StaticTextUI m_stCol3;
   StaticTextUI m_stCol4;
   StaticTextUI m_stCol5;
   StaticTextUI m_stCol6;
   StaticTextUI m_stCol7;


 
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;
   friend DragEvent;

   class VendreButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } vendreButtonEvent;  
   friend VendreButtonEvent;
      
   class BuyNowButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } buyNowButtonEvent;  
   friend BuyNowButtonEvent;

   class BuyNowNMSButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } buyNowNMSButtonEvent;  
   friend BuyNowNMSButtonEvent;
   
   class BidButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } bidButtonEvent;  
   friend BidButtonEvent;
   
   class CancelButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } cancelButtonEvent;  
   friend CancelButtonEvent;
   

   class SoldListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
      void RightClicked( void );
   } soldListEvent;
   friend SoldListEvent;

   
   
   class TabEvent00 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent00;

   class TabEvent01 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent01;

   class TabEvent02 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent02;

   class TabEvent03 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent03;

   class TabEvent04 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent04;

   class TabEvent05 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent05;

   class TabEvent06 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent06;

   class TabEvent07 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent07;

   class TabEvent08 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent08;

   class TabEvent09 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent09;

   class TabEvent10 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent10;

   class TabEvent11 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent11;

   class TabEvent12 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent12;

   class TabEvent13 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent13;
   
   class TabEvent14 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent14;

   class TabEvent15 : public EventVisitor{
   public: void LeftClicked( void );
   } tabEvent15;
   
   
   
   
   friend TabEvent00;
   friend TabEvent01;
   friend TabEvent02;
   friend TabEvent03;
   friend TabEvent04;
   friend TabEvent05;
   friend TabEvent06;
   friend TabEvent07;
   friend TabEvent08;
   friend TabEvent09;
   friend TabEvent10;
   friend TabEvent11;
   friend TabEvent12;
   friend TabEvent13;
   friend TabEvent14;
   friend TabEvent15;


   class ListDlgUI : public StaticTextUI{
   public:
      std::string optionID;
      int         dwState;
      int         dwID;
    };

   vector< ItemSold > m_AllItemList;

};


