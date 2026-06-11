// V3_ArenePointsDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "SliderUI.h"
#include "StaticTextUI.h"




class V3_ArenePointsDlg : public BoxUI  
{
public:	
	virtual ~V3_ArenePointsDlg();

    static V3_ArenePointsDlg *GetInstance();
    static unsigned int Display( void *null );
   
    virtual void Draw( V2SPRITEFX *vsfFX);
    void Hide();

    void UpdateFlagProgress(char code ,char chPC);
    void UpdateInfoPlay(long lAreneID,long lAreneType,int iPtsB, int iPtsR, int iNbrMin, int iNbrSec);
    long GetArenaID(){return m_lAreneID;}
    long GetArenaType(){return m_lAreneType;}
    
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

   int m_iPointBleu;
   int m_iPointRed;
   int m_iNbrMin;
   int m_iNbrSec;

   char m_chlastCode;
   char m_chlastCodeCnt;
   char m_chProgressPC;

   long m_lAreneID;
   long m_lAreneType;
  
private:
   
   
   
public:
   ButtonUI  Drag_Button;

private:
   V3_ArenePointsDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   void DrawSingleStatusItem(GraphUI &fullBar,int percent);
   
   GraphUI      m_Background;
   StaticTextUI m_PointB;
   StaticTextUI m_PointR;
   StaticTextUI m_Time;

   GraphUI      m_FlagBar;

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;


};



