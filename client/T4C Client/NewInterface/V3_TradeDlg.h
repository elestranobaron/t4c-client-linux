// V3_TradeDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "GridUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "V3_InvDlg.h"


const int g_AskFromBPToTrade   = 0;
const int g_AskFromTradeToBP   = 1;

const int CS_EDITINGITEMS = 0;
const int CS_READY        = 1;
const int CS_CONFIRMED    = 2;

const int AC_MYSELF  = 0;
const int AC_OTHER   = 1;
const int AC_BOTH    = 2;


class V3_TradeDlg : public BoxUI  
{
   friend bool V3_InvDlg::InventoryGridEvent::Dropping();
public:

public:	
	virtual ~V3_TradeDlg();


    static V3_TradeDlg *GetInstance();
    static unsigned int DisplayM( void *null);
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();


    void EventPlayerInvited( char *pCharacterName, DWORD pInvitorID, short pX, short pY);
    void EventTradeStarted ( char *pCharacterName );
    void EventTradeCanceled ();
    void EventTradeFinished();
	 void EventTradeStatusChanged(int pMyStatus, int pOtherStatus);
    void EventDroppingItemUIFromTradeToBackpack();

    void UpdateTrade( TemplateList< BAG_ITEM > *newTrade, int pAffected );
	 void UpdateItemsName (DWORD dwItemID, LPBYTE lpbItemName, ePlacesToSearch pWhereToLook);
        

    static void RequestTradeInvite(DWORD dwID, short xPos, short yPos);
    static void RequestTradeCancel();
    static void RequestTradeSetStatus(int Status);
    static void RequestTradeClear();
    static void RequestMoveItemFromTradeToBackpack( DWORD dwItemID, DWORD dwQty );
	 static void RequestMoveItemFromBackpackToTrade( DWORD dwItemID, DWORD dwQty );


    static unsigned int InvitePlayer( void *data );

    void AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal);
    void ConfirmReturn(bool bOK,UINT uiShowCodeP);

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void UpdateInterfaceStatus();
	void ResetTrade(); // Reset all trade data

private:
   // List containers.
   typedef V3_InvDlg::ItemUI ItemUI;

   friend V3_InvDlg::InventoryGridEvent;
   friend V3_InvDlg::JunkButtonEvent;
   friend V3_InvDlg::DropButtonEvent;

    

protected:

   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   char m_strMyInfo[100];
   char m_strOtherInfo[100];

   TemplateList< BAG_ITEM > tlMyTrade;
   TemplateList< BAG_ITEM > tlOtherTrade;
   BOOL	trading;
   int mMyStatus;
	int mOtherStatus;

   ItemUI *m_pItemUIReq;

   DWORD m_dwInvitorID;
   DWORD m_dwInvitorPX;
   DWORD m_dwInvitorPY;

   
   
public:
   ButtonUI  Drag_Button;


private:
   V3_TradeDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;



   ButtonUI     *junkButton;
   ButtonUI     *dropButton;
   GridUI	    *inventoryGrid;
   GraphUI      *invGridScrollUpGraph;
   GraphUI      *invGridScrollDownGraph;
   ButtonUI     *iconGold;
   StaticTextUI *statGold;
   StaticTextUI *instantItemInfo;

   StaticTextUI m_stMyInfo;
   StaticTextUI m_stOtherInfo;


   GraphUI    gridSelectedTile;

   GridUI	  tradeMyGrid;
   GraphUI    tradeMyGridScrollUpGraph;
   GraphUI    tradeMyGridScrollDownGraph;

   GridUI     tradeOtherGrid;
   GraphUI    tradeOtherGridScrollUpGraph;
   GraphUI    tradeOtherGridScrollDownGraph;
   
   
   ButtonUI	inviteButton;
   ButtonUI	cancelButton;
   ButtonUI	mReadyButtonON  , mReadyButtonOFF;
   ButtonUI	mConfirmButtonON, mConfirmButtonOFF, disabledConfirmButton;
   ButtonUI	clearButton     ;
   
   ButtonUI	mOtherReadyButton  ;
   ButtonUI	mOtherConfirmButton;
   


  

 
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

   // Cancel button click
   class CancelButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } cancelButtonEvent;    
   friend CancelButtonEvent;
   
   // Invite button click
   class InviteButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } inviteButtonEvent;    
	friend InviteButtonEvent;

   // Ready button click
   class ReadyButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } readyButtonEvent;    
	friend ReadyButtonEvent;

   // Confirm button click
   class ConfirmButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } confirmButtonEvent;    
	friend ConfirmButtonEvent;

   // Clear button click
   class ClearButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } clearButtonEvent;    
	friend ClearButtonEvent;


   // My trade grid events
   class TradeGridEvent : public EventVisitor{
   public:
      void Dropped( EventVisitor *sourceEvent );
      bool Dropping( void );
      void Dragged( void );
   } tradeGridEvent;
   friend TradeGridEvent;

};


