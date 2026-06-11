// V3_DisplayHelpDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"


const int iNBR_HELP_PAGE    =  43;


class V3_DisplayHelpDlg : public BoxUI  
{
public:	
	virtual ~V3_DisplayHelpDlg();


    static V3_DisplayHelpDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    int TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise);
    void Hide();

    void ShowHelp(int iPageIndex);

        

protected:


private:
   // List containers.
    

protected:

   int  m_iXPos;
   int  m_iYPos;
   int  m_iDisplayHelp;

   
   
public:
   ButtonUI  Drag_Button;


private:
   V3_DisplayHelpDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_TitleIcon;
   //GraphUI  m_HelpPage[iNBR_HELP_PAGE];

   bool     m_bHelpPageLoaded;
   char     m_strHelpPageName[iNBR_HELP_PAGE][1024];
   CV2Sprite  m_HelpPageHD;
   CV2Sprite  m_HelpPageHDM;
   
   
   ButtonUI  X_Button;
   ButtonUI  N_Button;
   ButtonUI  P_Button;
   ButtonUI  I_Button;

   ButtonUI  Link1_1;
   ButtonUI  Link1_2;
   ButtonUI  Link1_3;
   ButtonUI  Link1_4;
   ButtonUI  Link1_5;
   ButtonUI  Link1_6;
   ButtonUI  Link1_7;
   ButtonUI  Link1_8;
   ButtonUI  Link1_9;

   ButtonUI  Link2_1;
   ButtonUI  Link2_2;
   ButtonUI  Link2_3;
   ButtonUI  Link2_4;
   ButtonUI  Link2_5;
   ButtonUI  Link2_6;
   ButtonUI  Link2_7;
   ButtonUI  Link2_8;
   ButtonUI  Link2_9;


   class PrevButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } prevButtonEvent;
   
   class NextButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } nextButtonEvent;

   class IndexButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } indexButtonEvent;
   
   friend PrevButtonEvent;
   friend NextButtonEvent;
   friend IndexButtonEvent;


   class Link11ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link11ButtonEvent;

   class Link12ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link12ButtonEvent;

   class Link13ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link13ButtonEvent;

   class Link14ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link14ButtonEvent;

   class Link15ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link15ButtonEvent;

   class Link16ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link16ButtonEvent;

   class Link17ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link17ButtonEvent;

   class Link18ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link18ButtonEvent;

   class Link19ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link19ButtonEvent;



   class Link21ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link21ButtonEvent;
   
   class Link22ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link22ButtonEvent;
   
   class Link23ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link23ButtonEvent;
   
   class Link24ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link24ButtonEvent;
   
   class Link25ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link25ButtonEvent;
   
   class Link26ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link26ButtonEvent;
   
   class Link27ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link27ButtonEvent;
   
   class Link28ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link28ButtonEvent;
   
   class Link29ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } link29ButtonEvent;



   friend Link11ButtonEvent;
   friend Link12ButtonEvent;
   friend Link13ButtonEvent;
   friend Link14ButtonEvent;
   friend Link15ButtonEvent;
   friend Link16ButtonEvent;
   friend Link17ButtonEvent;
   friend Link18ButtonEvent;
   friend Link19ButtonEvent;

   friend Link21ButtonEvent;
   friend Link22ButtonEvent;
   friend Link23ButtonEvent;
   friend Link24ButtonEvent;
   friend Link25ButtonEvent;
   friend Link26ButtonEvent;
   friend Link27ButtonEvent;
   friend Link28ButtonEvent;
   friend Link29ButtonEvent;
  
 

};


