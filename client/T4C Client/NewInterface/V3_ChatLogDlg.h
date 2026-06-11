// V3_ChatLogDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "TextPageUI.h"




class V3_ChatLogDlg : public BoxUI  
{
public:	
   virtual ~V3_ChatLogDlg();
   //virtual bool VKeyInput( VKey vKey );
   
   static V3_ChatLogDlg *GetInstance();
   static unsigned int Display( void *null );
   
   virtual void Draw( V2SPRITEFX *vsfFX );
   void Hide();
   void Show();
   void AddListLogs(string fromUser, string newText, DWORD  baseColor, bool screenEcho, bool allowNewLine = true );
   void ClearListLogs();

     

protected:

   bool StartDragDlg( MousePos mousePos );
   void StopDragDlg();

   bool StartSizeDlg( MousePos mousePos );
   void StopSizeDlg();

public:
   TextPageUI   m_ListText;
   GraphUI      m_Background;
   GraphUI      m_BackgroundF;
      
    

protected:

   int  m_iXPos;
   int  m_iYPos;
   int  m_iHVal;
   int  m_iWVal;
   int  m_iPinnedStatus;


   //Drag Dialog
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   //ResizeY Dialog
   int  m_iYPosSR;
   int  m_iXPosSR;
   bool m_bSizeDialogX;
   bool m_bSizeDialogY;


private:

   
   
public:
   ButtonUI  Drag_Button;
   ButtonUI  UPSize_Button;
   ButtonUI  RightSize_Button;

   

private:
   V3_ChatLogDlg();
   void UpdateAllFramePos(int iPosX,int iPosY, int iW,int iH);
   void DrawSingleStatusItem(GraphUI &fullBar,int percent);
   
   
   GraphUI      m_RightScroll;
   GraphUI      m_RightScrollUPBtn;
   GraphUI      m_RightScrollDNBtn;

   ButtonUI     m_ListEnd;
   ButtonUI     m_ListPinE;
   ButtonUI     m_ListPinD;
   GraphUI      m_ListScroll;
   GraphUI      m_ListScrollUp;
   GraphUI      m_ListScrollDown;

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;
   friend DragEvent;

   class SizeEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
      void StartDragLButtonDown( int mX, int mY );
   } sizeEvent;
   friend SizeEvent;


   


   class ListEndEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } listEndEvent;
   
   friend ListEndEvent;

   class PinEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } pinEvent;
   
   friend PinEvent;



};


