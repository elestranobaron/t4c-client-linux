// V3_EffectStatusDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"




class V3_EffectStatusDlg : public BoxUI
{
public:	
	virtual ~V3_EffectStatusDlg();

    static V3_EffectStatusDlg *GetInstance();
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Show();

    void AddEffect( DWORD id, DWORD time, DWORD totalTime, DWORD iconId, std::string desc );
    void DispellEffect( DWORD id );    
    void DispellAllEffect();   
    
    bool LeftMouseDown( MousePos mousePos );
    bool LeftMouseUp( MousePos mousePos );
    bool RightMouseUp(MousePos mousePos);

     

protected:

   inline void SetListSize( unsigned long newSize )
   {
      m_iMaxListSize = newSize;
      // If the line is further than the end.
      if( m_iListPos >= m_iMaxListSize )
      {
         // Set line pos to last position.
         if( m_iMaxListSize == 0 )
            m_iListPos = 0;
         else
            m_iListPos = m_iMaxListSize - 1;
      }
    }

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void StartSizeDlg( MousePos mousePos );
   void StopSizeDlg();

private:
   void UpdateAllFramePos(int iPosX,int iPosY);
      
  
public:
   ButtonUI     Drag_Button;
   ButtonUI     UPSize_Button;

protected:

   int   m_iMaxListSize;
   int   m_iListPos;
   int   m_iPinnedStatus;
   int   m_iNbrVisibleEffect;


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   int  m_iYPosSR;
   int  m_iNbrVisibleEffectSR;

   bool m_bDragDialog;
   bool m_bSizeDialogY;

   bool m_bScrollEnabled;
   bool enabled;


   ButtonUI m_BtnScrollUp;
   ButtonUI m_BtnScrollDn;
   ButtonUI     m_ListPinE;
   ButtonUI     m_ListPinD;

private:
   class Effect : public GraphUI{
   public:
      bool operator < ( const Effect &eff ) const{
         return expireTime < eff.expireTime;
      }
      
      DWORD id;
      DWORD expireTime;
      DWORD initialTime;
      std::string desc;
      bool infinite;
    };

   class EffectDisplay : public GraphUI
   {
   public:
      EffectDisplay() : effect( NULL ),bTextInitFirstTime(false){};
      
      // Delegate drawing operations to the underlying effect object.
      virtual void Draw( V2SPRITEFX *vsfFX );
      
      virtual bool LeftMouseUp( MousePos mousePos );
      
      Effect *effect;
      bool bTextInitFirstTime;
      
      GraphUI emptyGauge;
   };
   

   std::vector< EffectDisplay > displayedEffects;
   std::list< Effect > m_ListEffects;

   V3_EffectStatusDlg();

   void CalcVisibleEffects( void );
   void CalcEffectInfo( void );
  
   


   class ScrollUpEvent : public EventVisitor{
   public:
      void LeftClicked();
   } scrollUpEvent;
   
   class ScrollDownEvent : public EventVisitor{
   public:
      void LeftClicked();
   } scrollDownEvent;
   
   friend ScrollDownEvent;
   friend ScrollUpEvent;

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

   class PinEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } pinEvent;
   
   friend PinEvent;
   

};



