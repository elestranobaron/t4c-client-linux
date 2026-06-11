// V3_QuestBookP2.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"

/*
typedef struct _sQuestActiveList
{
   USHORT        ushUniqueID;            // le id unique de cette quest book
   USHORT        ushQuestLevel;          // Niveau de la quete
   BYTE          chStep;
   std::string   strName;                // Nom de la quest
   std::vector <std::string> aStepMsg;   // la liste des message pour chaque step
}sQuestActiveList;
*/


class V3_QuestBookP2 : public BoxUI  
{
public:	
	virtual ~V3_QuestBookP2();


    static V3_QuestBookP2 *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void         LockList(){EnterCriticalSection(&crLockList);};
    void         UnlockList(){LeaveCriticalSection(&crLockList);};

    void ClearAllList();
    void UpdateQuestList   (USHORT ushUniqueID,USHORT ushLevel,BYTE chStep,char *pstrName,std::vector<std::string> &aStepMsg);
    void UpdateDisplayList();
    int GetListSize(){return m_vQuestListL.size();}
    void ConfirmReturn(bool bOK,UINT uiShowCodeP);

    class sQuestActiveList
    {
    public:
       static bool compareMyDataPredicate(sQuestActiveList lhs, sQuestActiveList rhs) 
       { 
          std::string s1,s2;
          s1 = lhs.strName.c_str();
          s2 = rhs.strName.c_str();
          std::replace( s1.begin(), s1.end(), 'É','E' );
          std::replace( s1.begin(), s1.end(), 'È','E' );
          std::replace( s1.begin(), s1.end(), 'Ê','E' );
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

          std::replace( s2.begin(), s2.end(), 'É','E' );
          std::replace( s2.begin(), s2.end(), 'È','E' );
          std::replace( s2.begin(), s2.end(), 'Ê','E' );
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
          if(lhs.ushQuestLevel < rhs.ushQuestLevel)
             return true;
          if(lhs.ushQuestLevel == rhs.ushQuestLevel)
             return (s1 < s2); 
          return false;
       }

       USHORT        ushUniqueID;            // le id unique de cette quest book
       USHORT        ushQuestLevel;          // Niveau de la quete
       BYTE          chStep;
       std::string   strName;                // Nom de la quest
       std::vector <std::string> aStepMsg;   // la liste des message pour chaque step
    };
    

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void ListToControlDisplay();
   void ListToControlDisplayStep();
   COLORREF  QuestColoybyLevel(int iQL);

private:
   // List containers.
   std::vector <sQuestActiveList>  m_vQuestList;
   std::vector <sQuestActiveList>  m_vQuestListL;

   int m_iSelQuest;
   int m_iSelQuestL;

   unsigned short m_ushRemoveQuestID;

       
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
   bool     m_bNeedUpdateSelect;




private:
   V3_QuestBookP2();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsL;
   GraphUI  m_backIconsP;
   
   ButtonUI  X_Button;
   

   ButtonUI  btnPage1;
   ButtonUI  btnPage2;
   ButtonUI  btnPage3;
   ButtonUI  btnAbandonner;
   
   ListUI   QuestList;
   GraphUI  QuestSelect;
   GraphUI  QuestListScrollUp;
   GraphUI  QuestListScrollDown;

   ListUI   QuestStepList;
   GraphUI  QuestStepSelect;
   GraphUI  QuestStepListScrollUp;
   GraphUI  QuestStepListScrollDown;

   StaticTextUI m_stHList;
   StaticTextUI m_stFList;


   StaticTextUI m_stStep1;
   StaticTextUI m_stStep2;
   StaticTextUI m_stStep3;
   StaticTextUI m_stStep4;
   StaticTextUI m_stStep5;
   StaticTextUI m_stStep6;

   class AbandonnerButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } abandonnerButtonEvent;
   
   class Page1ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } page1ButtonEvent;

   class Page2ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } page2ButtonEvent;

   class Page3ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } page3ButtonEvent;


   class QuestListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } questListEvent;

   class QuestStepListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } questStepListEvent;
    
   
   friend QuestListEvent;
   friend QuestStepListEvent;
   friend Page1ButtonEvent;
   friend Page2ButtonEvent;
   friend Page3ButtonEvent;
   friend AbandonnerButtonEvent;

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

};


