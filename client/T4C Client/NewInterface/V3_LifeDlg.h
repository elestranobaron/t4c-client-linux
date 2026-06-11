// V3_LifeDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"

const int iNBR_SKIN_IMAGE_MAX = 300;


class V3_LifeDlg : public BoxUI  
{
public:	
	virtual ~V3_LifeDlg();

    static V3_LifeDlg *GetInstance();
    static unsigned int Display( void *null );
   
    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    bool RightMouseUp( MousePos mousePos );

    void UpdateDeadStatus(BYTE chStatus,bool bProcessHide);
    void UpdateDeadInfo(WORD wTimeCurrent,WORD wTimeTotal,BYTE chCanResurect);
    int  IsDeath(){return m_dwDeadStatus;}

    
    void DrawSingleStatusItem(GraphUI &fullBar,int percent);

     

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

   int          m_dwDeadStatus;
   int          m_dwDeadCur;
   int          m_dwDeadMax;
   int          m_dwCanRestore;


private:
   
public:
   ButtonUI     Drag_Button;

private:
   V3_LifeDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI      m_Background;
   GraphUI      m_BackgroundD;
   GraphUI      m_PVBar;
   GraphUI      m_PMBar;
   GraphUI      m_DTBar;
   GraphUI      m_DeathImage;
   GraphUI      m_stAttack;
   GraphUI      m_stXPScroll;
   GraphUI      m_stORScroll;
   GraphUI      m_stRPDisable;

   ButtonUI     m_DeathBtnRestore;
   StaticTextUI m_stPV;
   StaticTextUI m_stPM;
   StaticTextUI m_stLv;
   StaticTextUI m_stNom;

   ButtonUI     m_invBtnPV;
   ButtonUI     m_invBtnPM;

   GraphUI      m_TargetImg[iNBR_SKIN_IMAGE_MAX];


   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

   class DeathRestoreEvent : public EventVisitor{
   public:
      void LeftClicked( void );
    } m_DeathBtnRestoreEvent;
   friend DeathRestoreEvent;

 

};



