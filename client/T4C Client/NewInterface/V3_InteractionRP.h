// V3_InteractionRPDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"

typedef struct _sRPList
{
   int iMessageID;
   char strPLName[50];
   char strMessage[256];
}sRPList;

typedef struct _sPLList
{
   int iPlID;
   std::string strPlName;
}sPLList;


class V3_InteractionRPDlg : public BoxUI  
{
public:	
	virtual ~V3_InteractionRPDlg();


    static V3_InteractionRPDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey);
    void ConfirmReturn(bool bOK,UINT uiShowCodeP);

    void         LockList(){EnterCriticalSection(&crLockList);};
    void         UnlockList(){LeaveCriticalSection(&crLockList);};

    void ClearAllList();
    void UpdateInfo(int iPC, int iLevel, int iPoints);
    void UpdateRPList(int iID,char *pName,char *pstrMsg);
    void UpdatePLList(int iPlID,char *pName);
    void UpdateRPActive(char chRPType, char *pstrSubject);
    void UpdateDisplayList();
    void AcceptPlayer(char *pstrPLName);
    void InvitePlayer(char *pstrPLName);
    
    

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void ListToControlDisplay();

private:
   // List containers.
   char                       m_chHaveRP;
   std::string                m_strSubject;
   std::vector <sRPList>      m_vRPList;
   std::vector <sPLList>      m_vPLList;

   char                       m_chHaveRPL;
   std::string                m_strSubjectL;
   std::vector <sRPList>      m_vRPListL;
   std::vector <sPLList>      m_vPLListL;
    
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

   int m_iRPlevelPC;
   int m_iPoints;



private:
   V3_InteractionRPDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   static unsigned int InvitePlayer( void *data );
   void DrawSingleStatusItem(GraphUI &fullBar,int percent);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsL;
   
   ButtonUI  X_Button;
   

   ButtonUI  RefreshBtn  ;
   ButtonUI  CreerBtn    ;
   ButtonUI  RejoindreBtn;
   ButtonUI  InviterBtn  ;
   ButtonUI  QuitterBtn  ;
   ButtonUI  TerminerBtn ;
   ButtonUI  SignalerBtn ;
   ButtonUI  ExpulserBtn ;

   ListUI   PlayerList;
   GraphUI  PlayerSelect;
   GraphUI  PlayerListScrollUp;
   GraphUI  PlayerListScrollDown;

   ListUI   PhraseList;
   GraphUI  PhraseSelect;
   GraphUI  PhraseListScrollUp;
   GraphUI  PhraseListScrollDown;
   
   StaticTextUI stPhraseActive;
   StaticTextUI stPhraseComplete;

   StaticTextUI m_stHNom;
   StaticTextUI m_stHPhrase;
   StaticTextUI m_stLevel;

   GraphUI      m_RPBar;
  
   class RefreshButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } refreshButtonEvent;

   class CrerButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } crerButtonEvent;
   
   class RejoindreButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } rejoindreButtonEvent;
   
   class InviterButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } inviterButtonEvent;

   class ExpulserButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } expulserButtonEvent;
   
   class QuitterButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } quitterButtonEvent;

   class TerminerButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } terminerButtonEvent;

   class SignalerButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } signalerButtonEvent;



   class PlayerListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } playerListEvent;

   class PhraseListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } phraseListEvent;
    
   
   friend PlayerListEvent;
   friend PhraseListEvent;
   friend CrerButtonEvent;
   friend RejoindreButtonEvent;
   friend InviterButtonEvent;
   friend ExpulserButtonEvent;
   friend QuitterButtonEvent;
   friend TerminerButtonEvent;
   friend SignalerButtonEvent;

 
/*
   class OptDlgUI : public StaticTextUI
   {
   public:
      std::string optionID;
      bool        bState;
      int         dwID;
      int         iCost;
   };
   
   class StrListTmp : public GameUI
   {
   public:
      char strTmp[2];
    } strListTmp;
   */

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

};


