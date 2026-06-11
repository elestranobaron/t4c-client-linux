// V3_BoutiqueDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"

typedef struct _sAchatListT
{
   char strName[128];
   char strDesc[512];
   int      iCost;
}sAchatListT;


class V3_BoutiqueDlg : public BoxUI  
{
public:	
	virtual ~V3_BoutiqueDlg();


    static V3_BoutiqueDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void CallMainNMSAchat();
    void SetNbrNMSGold(DWORD dwNbr);
    void ClearAchatList();
    void AddAchatOption(char *pstrName,char *pstrDesc, int iCost);
    
    void ListToControl();
    void HideConfirmWindows();
    
    void  SetAchatOpt(DWORD dwOpt){m_dwSelectedAchatOpt = dwOpt;}
    DWORD GetSelectedOpt (){return m_dwSelectedAchatOpt;}
    DWORD GetSelectedItem(){return m_dwSelectedItemList;}

    void ConfirmReturn(bool bOK,UINT uiShowCodeP);
    
    ListUI   achatList;
    char     m_strDescription[512];

        

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
   // List containers.
    
public:
   ButtonUI  Drag_Button;

protected:


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   DWORD    m_dwSelectedAchatOpt;
   DWORD    m_dwSelectedItemList;
   DWORD    m_dwNbrNMSGold;
   bool     m_bLockDrawList;


   std::vector <sAchatListT>        m_vAchatList;
   std::vector <sAchatListT>        m_vListDraw;


private:
   V3_BoutiqueDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;
   


   ButtonUI  AchatOpt1Button;
   ButtonUI  AchatOpt2Button;
   ButtonUI  AchatOpt3Button;
   ButtonUI  AchatOpt4Button;
   ButtonUI  AchatShowMinionSkin;
   ButtonUI  AcheterButton;
   ButtonUI  PanierButton;

   GraphUI  achatListingSelect;
   GraphUI  achatListScrollUp;
   GraphUI  achatListScrollDown;
   
   StaticTextUI achatDescription;

   StaticTextUI m_stHEcu;
   StaticTextUI m_stEcu;
   StaticTextUI m_sthCol1;
   StaticTextUI m_sthCol2;


  

   class AchatOpt1ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } achatOpt1ButtonEvent;
   
   class AchatOpt2ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } achatOpt2ButtonEvent;
   
   class AchatOpt3ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } achatOpt3ButtonEvent;
   
   class AchatOpt4ButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } achatOpt4ButtonEvent;

   class AchatShowMinionSkinEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } achatShowMinionSkinEvent;

   
   class AcheterButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } acheterButtonEvent;
   
   class PanierButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } panierButtonEvent;

   class AchatListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } achatListEvent;
    
   
   friend AchatListEvent;
   friend AchatOpt1ButtonEvent;
   friend AchatOpt2ButtonEvent;
   friend AchatOpt3ButtonEvent;
   friend AchatOpt4ButtonEvent;
   friend AcheterButtonEvent;
   friend PanierButtonEvent;

 

   class OptDlgUI : public StaticTextUI
   {
   public:
      std::string optionID;
      bool        bState;
      int         dwID;
      int         iCost;
   };
   
   class StrListTmp : public GameUI
   {
   public:
      char strTmp[2];
    } strListTmp;
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

};


