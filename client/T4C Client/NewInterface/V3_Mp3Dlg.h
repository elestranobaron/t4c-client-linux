// V3_Mp3Dlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "SliderUI.h"
#include "StaticTextUI.h"




class V3_Mp3Dlg : public BoxUI  
{
public:	
	virtual ~V3_Mp3Dlg();

    static V3_Mp3Dlg *GetInstance();
    static unsigned int Display( void *null );
   
    virtual void Draw( V2SPRITEFX *vsfFX);
    void Hide();
    
protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
      
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   int m_iVolume10;


private:
   
   
   
public:
   ButtonUI  Drag_ButtonL;
   ButtonUI  Drag_ButtonR;

private:
   V3_Mp3Dlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI      m_Background;
   ButtonUI     PlayButton;
   ButtonUI     PauseButton;
   ButtonUI     NextButton;
   ButtonUI     PrevButton;
   ButtonUI     RndButtonOn;
   ButtonUI     RndButtonOf;
   ButtonUI     VolPButton;
   ButtonUI     VolMButton;
   StaticTextUI m_VolVal;




   class PlayButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } playButtonEvent;
   
   class PauseButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } pauseButtonEvent;
   
   class NextButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } nextButtonEvent;
   
   class PrevButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } prevButtonEvent;

   class RndButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } rndButtonEvent;

   class VolMButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } volMButtonEvent;

   class VolPButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } volPButtonEvent;


   
   friend PlayButtonEvent;
   friend PauseButtonEvent;
   friend NextButtonEvent;
   friend PrevButtonEvent;
   friend RndButtonEvent;
   friend VolMButtonEvent;
   friend VolPButtonEvent;



   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;


};



