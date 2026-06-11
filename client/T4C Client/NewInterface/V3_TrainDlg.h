// V3_TrainDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"




class V3_TrainDlg : public BoxUI  
{
public:	
	virtual ~V3_TrainDlg();


    static V3_TrainDlg *GetInstance();
    static unsigned int Display( void *null ,UINT uiMode);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Show();

    struct SellItemInfo
    {
       SellItemInfo() : skillID( 0 ), canHave( true ), price( 0 ), qty( 0 ), maxQty( 0 ), teach(false)
       {

       }
       
       DWORD  skillID;
       bool   canHave;
       string name;
       string reqs;
       DWORD  price;
       WORD   qty;
       WORD   initialQty;
       WORD   maxQty;
       bool   teach;
       DWORD  skillPnts;
       DWORD  icon;
    };

   
    void Train( WORD skillPnts, const std::list< SellItemInfo > &sellItemList );
    void Learn( WORD skillPnts, const std::list< SellItemInfo > &sellItemList );

    UINT m_uiDisplayMode;
     

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



   
   
public:
   ButtonUI  Drag_Button;


private:
   V3_TrainDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  X_Button;



   ButtonUI  m_btnT;
   ButtonUI  m_btnL;
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
   StaticTextUI m_stHSkill;
   StaticTextUI m_stReq;
   StaticTextUI m_stTot;
   StaticTextUI m_stLeft;
   StaticTextUI m_stSkill;


   WORD m_wSkillPnts;

   class SellItemUI : public GraphUI{
   public:
      SellItemInfo itemInfo;
    };

   class TButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tButtonEvent;
   
   class ItemListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
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


