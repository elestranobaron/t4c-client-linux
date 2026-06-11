// V3_InvDlg.h: 
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
#include "V3_MacroDlg.h"



class V3_ChestDlg;
class V3_ChestNewDlg;
class V3_GuildChestDlg;
class V3_GuildChestNewDlg;
class V3_TradeDlg;
class V3_AHVendreDlg;


class V3_InvDlg : public BoxUI  
{
public:	

	enum TargetPriority{
		monsterPriority = 0,
		equalPriority   = 1,
		playerPriority  = 2
	};
   friend V3_ChestDlg;
   friend V3_ChestNewDlg;
   friend V3_GuildChestDlg;
   friend V3_GuildChestNewDlg;
   friend V3_AHVendreDlg;
 	friend V3_TradeDlg;

	virtual ~V3_InvDlg();

    static V3_InvDlg *GetInstance();
    static unsigned int DisplayM( void *null );
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void SetDefaultMacro( void );
    void UpdateCharacterSheet( void );
    void UpdateGold( void );
    void SyncEquipment( void );
    void UpdateInventory( TemplateList< BAG_ITEM > *newInventory );
    void BackpackToSave( void );
    void ClearItem();
    void Reload();
    void LoadBackpack( void );
    void InitializeMacro( void );
    void ReadImageFile(int dwOffset, BYTE *pImage, BYTE *pPal);
 
     
    void AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal);
    void ConfirmReturn(bool bOK,UINT uiShowCodeP);
	

    // A GameUI class to hold backpack items.
    class ItemUI : public GraphUI
    {
       public:
          ItemUI(){strName2[0] = 0x00;};
          // Called for help on item.
         bool RightMouseUp( MousePos mousePos );
         void UpdateName( void );
         void UpdateName2( void );
       
         void Setup();
         char strName2[512];
         BAG_ITEM *bagItem;
    };

	void UseItemDirect(ItemUI *item,LPBAG_ITEM bagItemPtr);

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void UpdateEquipDropZone( int bodyPos, DropZoneUI *dropZone );
   void ReadDisplayBDImage();

private:
	static UINT UseItemOnTarget( void *data );
   // List containers.
    

public:
   GridUI    inventoryGrid;
   

protected:

	MouseDevice mouse;
	AnimV2Sprite V3CastCursor;

   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   int     m_iJunkQty;
   ItemUI *m_pItemUIReq;



   
   
public:
   ButtonUI  Drag_Button;


private:
   V3_InvDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  X_Button;


   ButtonUI MOnglet01Button; //Personnage
   ButtonUI MOnglet02Button; //Inventaire
   ButtonUI MOnglet03Button; //Sorts
   ButtonUI MOnglet04Button; //Professions
   ButtonUI MOnglet05Button; //Guilde
   ButtonUI MOnglet06Button; //Groupe
   ButtonUI MOnglet07Button; //Macro
   ButtonUI MOnglet08Button; //Options



   ButtonUI     iconSTR;
   ButtonUI     iconEND;
   ButtonUI     iconAGI;
   ButtonUI     iconWIS;
   ButtonUI     iconINT;
   ButtonUI     iconAC;
   ButtonUI     iconGold;
   
   StaticTextUI statSTR;
   StaticTextUI statEND;
   StaticTextUI statAGI;
   StaticTextUI statWIS;
   StaticTextUI statINT;
   StaticTextUI statAC;
   StaticTextUI statGold;
   

   // Equipment drop zones.
   DropZoneUI mainDropZone;
   DropZoneUI headDropZone;
   DropZoneUI bodyDropZone;
   DropZoneUI leftDropZone;
   DropZoneUI rightDropZone;
   DropZoneUI legsDropZone;
   DropZoneUI feetDropZone;
   DropZoneUI capeDropZone;
   DropZoneUI glovesDropZone;
   DropZoneUI neckDropZone;
   DropZoneUI wristDropZone;
   DropZoneUI beltDropZone;
   DropZoneUI ring1DropZone;
   DropZoneUI ring2DropZone;
   DropZoneUI Orbe1DropZone;

   StaticTextUI instantItemInfo;

   GraphUI   gridSelectedTile;
   GraphUI   gridScrollUpGraph;
   GraphUI   gridScrollDownGraph;

   ButtonUI  macroButton;
   ButtonUI  TradeButton;
   ButtonUI  junkButton;
   ButtonUI  dropButton;
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;






   class TabStatsEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabStatsEvent;
   friend TabStatsEvent;
   
//    class TabInvEvent : public EventVisitor{
//    public:
//       void LeftClicked( void );
//    } tabInvEvent;
//    friend TabInvEvent;
   
   class TabSpellEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabSpellEvent;
   friend TabSpellEvent;
   
   class TabProfessionEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabProfessionEvent;
   friend TabProfessionEvent;
   
   class TabGuildeEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabGuildeEvent;
   friend TabGuildeEvent;
   
   class TabGroupEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabGroupEvent;
   friend TabGroupEvent;
   
   class TabMacroEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabMacroEvent;
   friend TabMacroEvent;

   class TabOptionEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabOptionEvent;
   friend TabOptionEvent;
   
 

   class EquipEvent : public EventVisitor{
   public:
      void Dropped( EventVisitor *sourceEvent );
      bool Dropping( void );
      void Dragged( void );
      bool CanDrag( GameUI *dragUI );
   } equipEvent;
   
   friend EquipEvent;

   class InventoryGridEvent : public EventVisitor{
   public:
      // Uses an inventory item.
      void LeftDblClicked();
      // Acknowledges dropped items.
      void Dropped( EventVisitor *sourceEvent );
      bool Dropping( void );
      void Dragged( void );
    } inventoryGridEvent;  
   friend InventoryGridEvent;

   class ItemMacroUI : public MacroDataUI{    
   public:
      string name;
      DWORD baseId;
      WORD  iconId;
    };

   // Junk button click
   class JunkButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
    } junkButtonEvent;  
   friend JunkButtonEvent;

   class DropButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
    } dropButtonEvent;
   friend DropButtonEvent;

   // Macro callbacks.
   class MacroButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
    } macroButtonEvent;
   friend MacroButtonEvent;

   // Trade button click
   class TradeButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
    } tradeButtonEvent;  
   friend TradeButtonEvent;

   class ItemMacroEvent : public MacroEvent{
      virtual void ExecMacro( MacroDataUI *graphUI );
      virtual void SaveMacro( const list< MacroDataUI * > &macroList );
   } itemMacroEvent;
   
    friend ItemMacroEvent;



   typedef struct _sImages
   {
      char strName[255];
      int  iOffset;
   }sImagesDisplay;
   
   public:
      vector< sImagesDisplay > m_vImageDisplay;

};


