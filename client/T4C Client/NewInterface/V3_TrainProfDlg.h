// V3_TrainProfDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"




class V3_TrainProfDlg : public BoxUI  
{
public:	
	virtual ~V3_TrainProfDlg();


    static V3_TrainProfDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Show();

    USHORT ushStatApo;
    USHORT ushStatBij;
    USHORT ushStatCou;
    USHORT ushStatArm;
    USHORT ushStatFor;
    USHORT ushStatEbe;
    DWORD  dwStatGold;
    
    struct ProfItemInfo
    {
       ProfItemInfo() : ushID( 0 ), canLearn( 0 ), dwCost( 0 ), teach(false), ushSkillLevel ( 0 )
       {
       }
       
       BYTE   canLearn;
       bool   teach;
       BYTE   chProfession;
       USHORT ushID;
       USHORT ushSkillLevel;
       DWORD  dwCost;
       string name;
   };

   void TrainProf(DWORD newGoldVal,const std::list< ProfItemInfo > &sellItemList );
     

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
   // List containers.
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;


   DWORD dwGoldValue;
   DWORD dwGoldBuyPrice;


   
   
public:
   ButtonUI  Drag_Button;

private:
   V3_TrainProfDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  X_Button;



   ButtonUI  m_btnT;
   GraphUI   m_Icons;
   ListUI    m_ListItem;
   GraphUI   selectionSprite;
   GraphUI   listArrowUp;
   GraphUI   listArrowDown;

   StaticTextUI m_stHCol1;
   StaticTextUI m_stHCol2;
   StaticTextUI m_stHCol3;

   StaticTextUI m_stHOr;
   StaticTextUI m_stOr;
   StaticTextUI m_stStatApo;
   StaticTextUI m_stStatBij;
   StaticTextUI m_stStatCou;
   StaticTextUI m_stStatArm;
   StaticTextUI m_stStatFor;
   StaticTextUI m_stStatEbe;
   

   class ProfItemUI : public GraphUI{
   public:
      ProfItemInfo itemInfo;
    };

   class TButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tButtonEvent;
   
   class ItemListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
      void LeftDblClicked();
   } itemListEvent;
   
   friend ItemListEvent;
   friend TButtonEvent;
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

};


