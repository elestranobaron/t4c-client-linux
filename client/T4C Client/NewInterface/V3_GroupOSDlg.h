// V3_GroupOSDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"




class V3_GroupOSDlg : public BoxUI  
{
public:	
	virtual ~V3_GroupOSDlg();

    static V3_GroupOSDlg *GetInstance();
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Show();


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


private:
   
   
   
public:
   ButtonUI  Drag_Button;

private:
   V3_GroupOSDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   

   GraphUI      BackGround;
   GraphUI      fullBar;
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



