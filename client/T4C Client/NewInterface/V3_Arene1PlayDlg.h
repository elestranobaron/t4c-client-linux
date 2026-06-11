// V3_Arene1PlayDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "..\NMTimerUtils.h"


typedef struct _sAr1PLay
{
   int iPlID;
   std::string strPlName;
   int iFlag;
   int iKill;
   int iDead;
   int iPlPoints;
}sAr1PLay;


class V3_Arene1PlayDlg : public BoxUI  
{
public:	
	virtual ~V3_Arene1PlayDlg();


    static V3_Arene1PlayDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();


    void         LockList(){EnterCriticalSection(&crLockList);};
    void         UnlockList(){LeaveCriticalSection(&crLockList);};

    void ClearAllList();
    void UpdateInfo(int iArenaID, int iArenaType,short dwStayMin,short dwStaySec,bool bOnList,int iLevelMin,int iLevelMax,int iPB, int iPR,bool bSetTimer);
    void UpdatePLList(int iPlID,char *pName,int iFlag, int iKill, int iDead,int iTeamID,int iPlPoints);
    void UpdateDisplayList();
    

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void ListToControlDisplay();

private:
   // List containers.
   std::vector <sAr1PLay>      m_vPLListB;
   std::vector <sAr1PLay>      m_vPLListLB;
   std::vector <sAr1PLay>      m_vPLListR;
   std::vector <sAr1PLay>      m_vPLListLR;
    
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

   int m_iArenaID;
   int m_iArenaType;

   bool     m_bLockDrawList;
   bool     m_bNeedUpdateList;

   int  m_iLevelMin;
   int  m_iLevelMax;
   int  m_iPointB;
   int  m_iPointR;
   bool m_bOnArene;
   int   m_iSecTotal;
   short m_shTimeStayMin;
   short m_shTimeStaySec;

   CNMTimerUtils m_TimerTools;



private:
   V3_Arene1PlayDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   void DrawSingleStatusItem(GraphUI &fullBar,int percent);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;
   

   ButtonUI  RejoindreBtn  ;
   ButtonUI  QuitterBtn  ;

   ListUI   PlayerListB;
   GraphUI  PlayerSelectB;
   GraphUI  PlayerListScrollUpB;
   GraphUI  PlayerListScrollDownB;

   ListUI   PlayerListR;
   GraphUI  PlayerSelectR;
   GraphUI  PlayerListScrollUpR;
   GraphUI  PlayerListScrollDownR;

   StaticTextUI stStayTime;
   StaticTextUI stBPoint;
   StaticTextUI stBName;
   StaticTextUI stBFlag;
   StaticTextUI stBKill;
   StaticTextUI stBDead;
   StaticTextUI stBPts;
   StaticTextUI stRPoint;
   StaticTextUI stRName;
   StaticTextUI stRFlag;
   StaticTextUI stRKill;
   StaticTextUI stRDead;
   StaticTextUI stRPts;

   GraphUI  m_FlagB;
   GraphUI  m_FlagR;
   
  
   class RejoindreButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } rejoindreButtonEvent;

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
   friend RejoindreButtonEvent;
   
 
   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

   void OnTimer(unsigned int nIDEvent);
   static int    TimerCallback(void* pThis,unsigned int nIDEvent);

};


