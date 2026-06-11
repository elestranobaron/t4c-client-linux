// V3_GuildChestDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "DropZoneUI.h"
#include "GridUI.h"
#include "V3_InvDlg.h"



class ChestUI;
class GuildChestUI;


const int g_AskChestToBackpackGuild = 0;
const int g_AskBackpackToChestGuild = 1;


class V3_GuildChestDlg : public BoxUI  
{
public:	
   friend ChestUI;
   friend GuildChestUI;

	virtual ~V3_GuildChestDlg();

    static V3_GuildChestDlg *GetInstance();
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();




    void UpdateChest( TemplateList< BAG_ITEM > *newChest );
    void Reload();
    void ClearItem();
    BOOL UpdateItemsName (DWORD dwItemID, LPBYTE lpbItemName);
    bool VKeyInput( VKey vKey );

    void RequestMoveItemFromChestToBackpack( DWORD dwItemID, DWORD dwQty );
    void RequestMoveItemFromBackpackToChest( DWORD dwItemID, DWORD dwQty );
    
    void GuildChestToSave( void );
    void LoadChest( void );

     
    void AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal);

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
   // List containers.
   typedef V3_InvDlg::ItemUI ItemUI;



protected:

   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   ItemUI *m_pItemUIReq;


   
   
public:
   ButtonUI  Drag_Button;



private:
   V3_GuildChestDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);


   friend V3_InvDlg::InventoryGridEvent;
   friend V3_InvDlg::JunkButtonEvent;
   friend V3_InvDlg::DropButtonEvent;

   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  X_Button;


   class DragEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
      void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

   ButtonUI  *junkButton;
   ButtonUI  *dropButton;
   
   GridUI	  *inventoryGrid;
   GraphUI    *invGridScrollUpGraph;
   GraphUI    *invGridScrollDownGraph;

   GridUI	 chestGrid;
   GraphUI   chestGridScrollUpGraph;
   GraphUI   chestGridScrollDownGraph;
   
   
   GraphUI   backChestT;
   GraphUI   backChestB;
   GraphUI   gridSelectedTile;
   ButtonUI  *iconGold;
   StaticTextUI *statGold;
   StaticTextUI *instantItemInfo;
   
   TemplateList< BAG_ITEM > tlChest;
   

   

   class ChestGuildGridEvent : public EventVisitor
   {
   public:
      bool Dropping( void );
      void Dragged( void );
   } chestGuildGridEvent;
   friend ChestGuildGridEvent;
   
	
};


