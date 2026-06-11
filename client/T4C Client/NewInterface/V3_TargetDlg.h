// V3_TargetDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"


const int iNBR_TARGET_IMAGE_MAX = 330;

class V3_TargetDlg : public BoxUI  
{
public:	
	virtual ~V3_TargetDlg();

    static V3_TargetDlg *GetInstance();
    static unsigned int Display( void *null );
   
    virtual void Draw( V2SPRITEFX *vsfFX ,TFCObject *pObject);
    void Hide();
    
    void DrawSingleStatusItem(GraphUI &fullBar,int percent);

    void UpdateInfo(char *pStrName, int iPCHP, WORD wLevel);
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

   char m_strName[1024];
   int  m_iPCHP;
   WORD m_wLevel;

private:
   
   
   
public:
   ButtonUI  Drag_Button;

private:
   V3_TargetDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI      m_Background;
   GraphUI      m_PVBar;

   GraphUI      m_TargetImg[iNBR_TARGET_IMAGE_MAX];


   StaticTextUI m_stPV;
   StaticTextUI m_stNom;


   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;


};



