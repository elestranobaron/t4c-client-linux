// V3_QuestBookP3.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"

/*
typedef struct _sQuestCompleteList
{
   USHORT        ushUniqueID;            // le id unique de cette quest book
   USHORT        ushQuestLevel;          // Niveau de la quete
   std::string   strName;                // Nom de la quest
}sQuestCompleteList;
*/

class V3_QuestBookP3 : public BoxUI  
{
public:	
	virtual ~V3_QuestBookP3();


    static V3_QuestBookP3 *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void         LockList(){EnterCriticalSection(&crLockList);};
    void         UnlockList(){LeaveCriticalSection(&crLockList);};

    void ClearAllList();
    void UpdateQuestList   (USHORT ushUniqueID,USHORT ushLevel,char *pstrName);
    void UpdateDisplayList();
    int GetListSize(){return m_vQuestListL.size();}
    
    class sQuestCompleteList
    {
    public:
       static bool compareMyDataPredicate(sQuestCompleteList lhs, sQuestCompleteList rhs) 
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
       std::string   strName;     
    };

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void ListToControlDisplay();
   void ListToControlDisplayStep();

private:
   // List containers.
   std::vector <sQuestCompleteList>  m_vQuestList;
   std::vector <sQuestCompleteList>  m_vQuestListL;

       
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




private:
   V3_QuestBookP3();
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
   
   ListUI   QuestList;
   GraphUI  QuestSelect;
   GraphUI  QuestListScrollUp;
   GraphUI  QuestListScrollDown;

   StaticTextUI m_stHList;
   StaticTextUI m_stFList;

   
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

    
   
   friend QuestListEvent;
   friend Page1ButtonEvent;
   friend Page2ButtonEvent;
   friend Page3ButtonEvent;

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

};


