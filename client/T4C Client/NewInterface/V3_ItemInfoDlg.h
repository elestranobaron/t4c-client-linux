// V3_ItemInfoDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"




class V3_ItemInfoDlg : public BoxUI  
{
public:	
	virtual ~V3_ItemInfoDlg();


    static V3_ItemInfoDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    int TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise);
    void Hide();

        

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

   char m_strBoustName[100][100];

   
   
public:
   ButtonUI  Drag_Button;


private:
   V3_ItemInfoDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;



  
   StaticTextUI m_stNameItemInfo;
 
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

};


