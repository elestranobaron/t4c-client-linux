// V3_GuildChestNewDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "V3_InvDlg.h"


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


class V3_GuildChestNewDlg : public BoxUI  
{
public:	
	virtual ~V3_GuildChestNewDlg();


    static V3_GuildChestNewDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Reload();
    void ClearItem();


    class cItemChest
    {
       public: 
          static bool compareMyDataPredicate(cItemChest lhs, cItemChest rhs) 
          { 
             std::string s1,s2;
             s1 = lhs.strName.c_str();
             s2 = rhs.strName.c_str();
             std::replace( s1.begin(), s1.end(), 'ä','a' );
             std::replace( s1.begin(), s1.end(), 'ï','i' );
             std::replace( s1.begin(), s1.end(), 'ë','e' );
             std::replace( s1.begin(), s1.end(), 'ö','o' );
             std::replace( s1.begin(), s1.end(), 'ü','u' );
             std::replace( s1.begin(), s1.end(), 'é','e' );
             std::replace( s1.begin(), s1.end(), 'è','e' );
             std::replace( s1.begin(), s1.end(), 'à','a' );
             std::replace( s1.begin(), s1.end(), 'ù','u' );
             std::replace( s1.begin(), s1.end(), 'â','a' );
             std::replace( s1.begin(), s1.end(), 'î','i' );
             std::replace( s1.begin(), s1.end(), 'ê','e' );
             std::replace( s1.begin(), s1.end(), 'ô','o' );
             std::replace( s1.begin(), s1.end(), 'û','u' );

             std::replace( s2.begin(), s2.end(), 'ä','a' );
             std::replace( s2.begin(), s2.end(), 'ï','i' );
             std::replace( s2.begin(), s2.end(), 'ë','e' );
             std::replace( s2.begin(), s2.end(), 'ö','o' );
             std::replace( s2.begin(), s2.end(), 'ü','u' );
             std::replace( s2.begin(), s2.end(), 'é','e' );
             std::replace( s2.begin(), s2.end(), 'è','e' );
             std::replace( s2.begin(), s2.end(), 'à','a' );
             std::replace( s2.begin(), s2.end(), 'ù','u' );
             std::replace( s2.begin(), s2.end(), 'â','a' );
             std::replace( s2.begin(), s2.end(), 'î','i' );
             std::replace( s2.begin(), s2.end(), 'ê','e' );
             std::replace( s2.begin(), s2.end(), 'ô','o' );
             std::replace( s2.begin(), s2.end(), 'û','u' );
             return (s1 < s2); 
             //return (lhs.strName < rhs.strName); 
          }

          std::string strName;
          DWORD       dwAppearance;
          DWORD       dwID;
          DWORD       dwStaticRef;
          DWORD       dwQty;
          long        lCharge;
          long        lEquipPos;
    };
    
   void UpdateItemList(std::vector< cItemChest > &ItemChest);
   void ChestNewLock()    {  m_bLockDraw = true;  }
   void ChestNewUnlock()  {  m_bLockDraw = false;  }
   bool IsDrawProgress(){return m_bDrawInProgress;}


   void AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey);
   void AskValueReturnSld(bool bOK,UINT uiShowCodeP,char *pStrValue1,int iSldVal);

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

   void UpdateSelectedEquip(int iID,bool bSearch = false, std::string strSearch = "");
   void ResetOngletSelect();
   void SetOngletSelect(int iID);

   void RequestMoveItemFromChestToBackpack( DWORD dwItemID, DWORD dwQty );
   void RequestMoveItemFromBackpackToChest( DWORD dwItemID, DWORD dwQty );

   void UpdateEquipDropZone(int iListPos);

private:
   // List containers.
   typedef V3_InvDlg::ItemUI ItemUI;

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
   char m_strLastSearch[512];

   int m_dwCurrentOngletSelect;
   BOOL m_bNeedToUpdateList;


   ItemUI *m_pItemUIReq;

   BAG_ITEM  *m_pDisplayBagItem;



private:
   V3_GuildChestNewDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;
   



   ListUI    ChestList;
   GraphUI   ChestListSelGraph;
   GraphUI   ChestListScrollUp;
   GraphUI   ChestListScrollDown;


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

   DropZoneUI mainDropZoneRemChest;
   DropZoneUI mainDropZoneAddChest;


   friend V3_InvDlg::InventoryGridEvent;
   friend V3_InvDlg::JunkButtonEvent;
   friend V3_InvDlg::DropButtonEvent;

   StaticTextUI statQty;
   StaticTextUI statGoldChest;
   ButtonUI     statGoldChestBtn;


   ButtonUI  *junkButton;
   ButtonUI  *dropButton;

   GridUI	  *inventoryGrid;
   GraphUI    *invGridScrollUpGraph;
   GraphUI    *invGridScrollDownGraph;

   ButtonUI  *iconGold;
   StaticTextUI *statGold;
   StaticTextUI *instantItemInfo;

   class ChestGridEvent : public EventVisitor
   {
   public:
      void RightClicked( void );
      bool Dropping( void );
      void Dragged( void );
   } chestGridEvent;
   friend ChestGridEvent;

 
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;
   friend DragEvent;

     

   class ChestListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
      void RightClicked( void );
   } chestListEvent;
   friend ChestListEvent;

   class StatGoldChestBtnEvent : public EventVisitor{
   public: void LeftClicked( void );
   } statGoldChestBtnEvent;
   friend StatGoldChestBtnEvent;
   
   
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

   vector< cItemChest > m_AllItemList;

};


