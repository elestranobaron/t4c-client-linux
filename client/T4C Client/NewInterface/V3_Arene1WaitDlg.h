// V3_Arene1WaitDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"

typedef struct _sAr1PL
{
   int iPlID;
   std::string strPlName;
}sAr1PL;


class V3_Arene1WaitDlg : public BoxUI  
{
public:	
	virtual ~V3_Arene1WaitDlg();


    static V3_Arene1WaitDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();


    void         LockList(){EnterCriticalSection(&crLockList);};
    void         UnlockList(){LeaveCriticalSection(&crLockList);};

    void ClearAllList();
    void UpdateInfoWait(long lArenaID,long lArenaType,short dwStayMin,short dwStaySec,int iLevelMin,int iLevelMax);
    void UpdateTimeStay(short shStayMin,short shStaySec);
    void UpdatePLList(int iPlID,char *pName);
    void UpdateDisplayList();
    

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void ListToControlDisplay();

private:
   // List containers.
   std::vector <sAr1PL>      m_vPLList;
   std::vector <sAr1PL>      m_vPLListL;
    
public:
   ButtonUI  Drag_Button;

protected:
   class StrListTmp : public GameUI
   {
      public:
         char strTmp[2];

   } strListTmp;
   CRITICAL_SECTION  crLockList;

   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   bool     m_bLockDrawList;
   bool     m_bNeedUpdateList;

   int  m_iLevelMin;
   int  m_iLevelMax;
   int   m_iSecTotal;
   short m_shTimeStayMin;
   short m_shTimeStaySec;
   long  m_lArenaID;
   long  m_lArenaType;


private:
   V3_Arene1WaitDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   void DrawSingleStatusItem(GraphUI &fullBar,int percent);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;
   

   ButtonUI  QuitterBtn  ;

   ListUI   PlayerList;
   GraphUI  PlayerSelect;
   GraphUI  PlayerListScrollUp;
   GraphUI  PlayerListScrollDown;

   StaticTextUI stStartTime;
   StaticTextUI stListTitle;
  

   class QuitterButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } quitterButtonEvent;


   class PlayerListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } playerListEvent;

   
   
   friend PlayerListEvent;
   friend QuitterButtonEvent;
  
 
   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;
};


