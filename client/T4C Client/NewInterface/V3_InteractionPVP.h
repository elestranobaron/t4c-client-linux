// V3_InteractionPVPDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "..\NMTimerUtils.h"

typedef struct _sAreneList
{
   int  iIDList;
   int  iIDType;
   int  iEnable;
   int  iStatus;
   int  iLevelMin;
   int  iLevelMax;
   int  iNbrSec;
   int  iNbrMin;
   char strARName[50];
   char strARDesc[2048];
}sAreneList;

class V3_InteractionPVPDlg : public BoxUI  
{
public:	
	virtual ~V3_InteractionPVPDlg();


    static V3_InteractionPVPDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void         LockList(){EnterCriticalSection(&crLockList);};
    void         UnlockList(){LeaveCriticalSection(&crLockList);};

    void ClearAllList();
    void UpdateDisplayList();
    void UpdateDisplayListStat();
    std::string GetSelectedAreneName(){return m_strSelectedAreneName;}

    void UpdateAreneList(int iID,int iType,int iEnable,char *pName,char *pstrDesc);
    void UpdateAreneListStat(int iIndex,int iEnable,int iStatus,int  iLevelMin,int  iLevelMax,int  iNbrSec,int  iNbrMin);
    

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void ListToControlDisplay();

private:
   // List containers.
   std::string                m_strDesc;
   std::vector <sAreneList>   m_vArList;
   std::vector <sAreneList>   m_vArListL;

   CNMTimerUtils m_TimerTools;
    
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
   bool     m_bNeedUpdateListStat;
   std::string m_strSelectedAreneName;

   GraphUI  m_IconArene01;
   GraphUI  m_IconArene02;



private:
   V3_InteractionPVPDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;
   ButtonUI  RejoindreBtn  ;
   ButtonUI  AfficherBtn  ;
  
   ListUI   AreneList;
   GraphUI  AreneSelect;
   GraphUI  AreneListScrollUp;
   GraphUI  AreneListScrollDown;
   
   StaticTextUI m_stHArene1;
   StaticTextUI m_stHArene2;
  
   class RejoindreButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } rejoindreButtonEvent;

   class AfficherButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } afficherButtonEvent;

   class AreneListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } areneListEvent;
    
   
   friend AreneListEvent;
   friend RejoindreButtonEvent;
   friend AfficherButtonEvent;

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


