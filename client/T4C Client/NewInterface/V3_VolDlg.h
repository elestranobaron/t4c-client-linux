// V3_VolDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "GridUI.h"
#include "StaticTextUI.h"




class V3_VolDlg : public BoxUI  
{
public:	
	virtual ~V3_VolDlg();


    static V3_VolDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    class Item{
    public:
       std::string itemName;
       WORD appearance;
       WORD baseId;
       DWORD id;
       DWORD qty;
    };

    void Rob( bool canRob, std::string targetName, std::list< Item > &itemList );
    void DispellRob();
        

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
   V3_VolDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   void Rob();

   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;



   // A GameUI class to hold backpack items.
   class ItemUI : public GraphUI{
   public:
      Item itemData;
   };
   
   
   
   bool canRob;
   
   GraphUI   gridSelectedTile;
   GraphUI   gridScrollUpGraph;
   GraphUI   gridScrollDownGraph;
   
   GridUI   itemGrid;
   ButtonUI robButton;
   ButtonUI disabledRobButton;
   StaticTextUI targetName;
   StaticTextUI instantItemInfo;


  

 
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;
   friend DragEvent;

   class ItemGridEvent : public EventVisitor{
   public:
      void LeftDblClicked();
      virtual bool Dropping();
   } itemGridEvent;
   friend ItemGridEvent;
   
   class RobButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } robButtonEvent;   
   friend RobButtonEvent;

};


