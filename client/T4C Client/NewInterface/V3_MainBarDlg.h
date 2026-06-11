// V3_MainBarDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\SaveGame.h"

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "EditUI.h"
#include "StaticTextUI.h"
#include "V3_MacroDlg.h"

const int SendToChannel = 1;
const int SendToPage    = 2;
const int SendToGame    = 3;


class V3_MainBarDlg : public BoxUI  
{
public:	
	virtual ~V3_MainBarDlg();

    static V3_MainBarDlg *GetInstance();
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Show();
    void AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey);
    void ConfirmReturn(bool bOK,UINT uiShowCodeP);

    virtual bool TextInput( char ch );  
    virtual bool VKeyInput( VKey vKey );

    void ClearChannelList();
    void AddChannel(std::string strName, bool bListen);

    void ComboReturn(UINT uiShowCodeP,char *pStrValue);
    void SmileReturn(UINT uiSmileID);

    int GetTextInputState(){return m_TextInputState;}
    void SetInputtextUI(std::string str){m_EditPageInput.SetText(str);}
    void ForceSetSendCC();
    void ShowQuitWindows();
    void UpdateMacroBtn();
    bool ManageESCGUIAll();
    
    virtual void SendMessageToCurrentChannel( const string &msg );

    void AskCompagnonName();

protected:
   
   int m_TextInputState;

   typedef struct _sChannel
   {
      std::string strName;
      bool        bListen;
   }sChannel;

   vector< sChannel > m_ChannelList;
   
   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
   
    

protected:

   int  m_iXPos;
   int  m_iYPos;
   int  m_iCurrentMP;
   int  m_iXPosSD;
   bool m_bDragDialog;

   std::string m_strLastCCSelected;

private:
   list<string> rollbackTyped; // List to allow rolling back the text you have already typed.
   list<string>::iterator ii;

   
   
public:
   ButtonUI  Drag_ButtonL;
   ButtonUI  Drag_ButtonR;


private:
   V3_MainBarDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI      m_Background;
   ButtonUI     m_BtnHome;
   ButtonUI     m_BtnExit;
   GraphUI      m_XPBar;
   StaticTextUI m_stXP;
   StaticTextUI m_stCCname;
   GraphUI      m_InputPatch;

   ButtonUI     m_BtnCCDlg;
   ButtonUI     m_BtnCC;
   ButtonUI     m_BtnDD;
   ButtonUI     m_BtnWW;
   ButtonUI     m_BtnCB;
   

   EditUI       m_EditInput;
   EditUI       m_EditPageInput;

   StaticTextUI m_strMP;
   ButtonUI     m_btnMPUp;
   ButtonUI     m_btnMPDn;

   ButtonUI     m_BtnChatLog;
   ButtonUI     m_BtnQuestBook;
   ButtonUI     m_BtnSmile;
   ButtonUI     m_BtnPVP;




   class BtnHomeEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnHomeEvent;
   
   friend BtnHomeEvent;

   class BtnExitEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnExitEvent;
   
   friend BtnExitEvent;

   class BtnCCDlgEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnCCDlgEvent;
   class BtnCCEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnCCEvent;
   class BtnDDEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnDDEvent;
   class BtnWWEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnWWEvent;
   class BtnCBEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnCBEvent;
   
   class BtnChatBoxEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnChatBoxEvent;

   class BtnQuestBookEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnQuestBookEvent;


   class BtnSmileEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnSmileEvent;

   class BtnPVPEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnPVPEvent;
   
   friend BtnCCDlgEvent;
   friend BtnCCEvent;
   friend BtnDDEvent;
   friend BtnWWEvent;
   friend BtnCBEvent;
   friend BtnChatBoxEvent;
   friend BtnSmileEvent;
   friend BtnPVPEvent;
   


   class MPUpEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } mpUpEvent;
   friend MPUpEvent;
   
   class MPDnEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } mpDnEvent;
   friend MPDnEvent;

   class DragEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
      void StartDragLButtonDown( int mX, int mY );
   } dragEvent;
   
   friend DragEvent;



   class MacroButtonUI : public GraphUI{
   public:
      MacroButtonUI();
      
      void Draw( V2SPRITEFX *fx )
      {
         if( macroItem != NULL )
         {
            if( macroItem->macroUI != NULL )
            {
               FramePos itemPos = macroItem->macroUI->GetPos();
               FramePos thisPos = GetPos();
               itemPos.SetNewOffset( MousePos( thisPos.x - itemPos.Width() / 2 + thisPos.Width() / 2,thisPos.y - itemPos.Height() / 2+ thisPos.Height() / 2));
               macroItem->macroUI->SetPos( itemPos );
               macroItem->macroUI->Draw( fx );
            }
         }
      }
      virtual bool LeftMouseDown( MousePos mousePos );
      virtual bool LeftMouseUp( MousePos mousePos );
      MacroItemUI *macroItem;
      int          iIndex;
   };
   
   friend MacroButtonUI;
   
   MacroButtonUI m_MacroButton[g_iNbrMacroY][g_iNbrMacroX];
  

  

};


