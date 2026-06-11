// V3_ProfessionDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"




class V3_ProfessionDlg : public BoxUI  
{
public:	
	virtual ~V3_ProfessionDlg();

    static V3_ProfessionDlg *GetInstance();
    static unsigned int DisplayM( void *null );
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void UpdateProfessionList();
    void UpdateProfessionListP();
    void UpdateFormuleList(int iID);

     

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

   DWORD m_dwResetHelp[10];


   
   
public:
   ButtonUI  Drag_Button;



private:
   V3_ProfessionDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  X_Button;


   ButtonUI MOnglet01Button; //Personnage
   ButtonUI MOnglet02Button; //Inventaire
   ButtonUI MOnglet03Button; //Sorts
   ButtonUI MOnglet04Button; //Professions
   ButtonUI MOnglet05Button; //Guilde
   ButtonUI MOnglet06Button; //Groupe
   ButtonUI MOnglet07Button; //Macro
   ButtonUI MOnglet08Button; //Options

   ButtonUI m_btnIng01;
   ButtonUI m_btnIng02;
   ButtonUI m_btnIng03;
   ButtonUI m_btnIng04;
   ButtonUI m_btnIng05;
   ButtonUI m_btnIng06;
   ButtonUI m_btnIng07;
   ButtonUI m_btnIng08;
   ButtonUI m_btnIng09;
   ButtonUI m_btnIng10;


   class ListDlgUI : public StaticTextUI{
   public:
      std::string optionID;
      int         dwState;
      int         dwID;
    };

   ButtonUI  m_btnFabriquer;
   
   ListUI   m_ListProfession;
   GraphUI  ProfessionListingSelectionGraphPatch;
   
   ListUI   m_ListFormule;
   GraphUI  FormuleListingSelectionGraphPatch;
   GraphUI  FormuleListScrollUp;
   GraphUI  FormuleListScrollDown;
   
   ListUI   m_ListSkin;
   
   StaticTextUI m_stHProf;
   StaticTextUI m_stHIngr;


   class FabriquerButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } fabriquerButtonEvent;
   friend FabriquerButtonEvent;
   
   class ProfessionListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } professionListEvent;
   friend ProfessionListEvent;
   
   class FormuleListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } formuleListEvent;
   friend FormuleListEvent;
   
   class SkinListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } skinListEvent;
    friend SkinListEvent;

	class ButtonIngredientEvent : public EventVisitor{
	public:
		void LeftClicked( void );
	} buttonIngredientEvent;
	friend ButtonIngredientEvent;


   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;






   class TabStatsEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabStatsEvent;
   friend TabStatsEvent;
   
   class TabInvEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabInvEvent;
   friend TabInvEvent;
   
   class TabSpellEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabSpellEvent;
   friend TabSpellEvent;
   
//    class TabProfessionEvent : public EventVisitor{
//    public:
//       void LeftClicked( void );
//    } tabProfessionEvent;
//    friend TabProfessionEvent;
   
   class TabGuildeEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabGuildeEvent;
   friend TabGuildeEvent;
   
   class TabGroupEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabGroupEvent;
   friend TabGroupEvent;
   
   class TabMacroEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabMacroEvent;
   friend TabMacroEvent;

   class TabOptionEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabOptionEvent;
   friend TabOptionEvent;
   
 


};


