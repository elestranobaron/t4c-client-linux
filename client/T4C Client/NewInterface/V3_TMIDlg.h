// V3_TMIDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"




class V3_TMIDlg : public BoxUI  
{
public:	
	virtual ~V3_TMIDlg();

    static V3_TMIDlg *GetInstance();
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Show();

     

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
   void    SetupViewObject();
   void    LoadTmiWorld(int iWorld,int dwX,int dwY);
   void    DrawTMI( void );
   BOOL    CreateTMIMap(WORD* pMap,USHORT xPos,USHORT yPos,USHORT World);
   
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;


private:
   BOOL  m_bShowThisObjType[__OBJGROUP_LASTGROUP];
   int   m_dwCurrentLoadedWorld;

   char  m_TMIFile[512];
   char  m_TMIFileC[512];
   WORD *m_pTMIData;
   BYTE *m_pTMIMap;
   BYTE *m_pTMIPal;

   
   
public:
   ButtonUI  Drag_Button;

   
private:
   V3_TMIDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI      m_Background;
   GraphUI      m_BackMap;
   GraphUI      m_PlayerPos;

   StaticTextUI m_stPosition;


   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;


};


