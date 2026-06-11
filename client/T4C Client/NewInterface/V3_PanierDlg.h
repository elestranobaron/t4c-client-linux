// V3_PanierDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"


typedef struct _sVosAchat
{
   char     strName[128];
   char     strInf [32];
   DWORD    dwID;
   char     chCanCrediter;
}sVosAchatT;


class V3_PanierDlg : public BoxUI  
{
public:	
	virtual ~V3_PanierDlg();


    static V3_PanierDlg *GetInstance();
    static unsigned int Display( void *null);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    int TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise);

    void RequestAchats();
    void SetNbrNMSGold(DWORD dwNbr);
    void SetSanction(USHORT ushNbrSanction, DWORD dwNbrSecRemain);
    void ClearAchatList();
    void AddAchatOption(char *pstrName,char *pstrInf, DWORD dwID, char chCanCrediter);
    void ListToControl();
    DWORD GetSelectedItem(){return m_dwSelectedItemList;}
        
    ListUI   achatList;
    char     m_strDescription[512];

    void ConfirmReturn(bool bOK,UINT uiShowCodeP);

        

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

   DWORD    m_dwSelectedAchatOpt;
   DWORD    m_dwSelectedItemList;
   DWORD    m_dwNbrNMSGold;
   bool     m_bLockDrawList;

   USHORT   m_ushNbrSanction;
   DWORD    m_dwNbrSecRemain;


   std::vector <sVosAchatT>        m_vAchatList;
   std::vector <sVosAchatT>        m_vListDraw;

   
   
public:
   ButtonUI  Drag_Button;

private:
   V3_PanierDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;



   ButtonUI  UtiliserButton;
   ButtonUI  CrediterButton;
   ButtonUI  BoutiqueButton;

   GraphUI  achatListingSelect;
   GraphUI  achatListScrollUp;
   GraphUI  achatListScrollDown;
   GraphUI  SanctionJailIcon;
   
   StaticTextUI achatDescription;

   StaticTextUI m_stHSanction;
   StaticTextUI m_stHEcu;
   StaticTextUI m_stEcu;
   StaticTextUI m_sthCol1;
   StaticTextUI m_sthCol2;

   GraphUI  backSanctionL1[4];
   GraphUI  backSanctionL2[4];
   GraphUI  backSanctionL3[4];


  

   class UtiliserButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } utiliserButtonEvent;
   
   class CrediterButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } crediterButtonEvent;
   class BoutiqueButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } boutiqueButtonEvent;
   
   
   friend UtiliserButtonEvent;
   friend CrediterButtonEvent;
   friend BoutiqueButtonEvent;


   class AchatListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } achatListEvent;
    
   friend AchatListEvent;


 

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


