// V3_MacroDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\SaveGame.h"
#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"


class MacroDataUI : public GraphUI{
public:
   MacroDataUI() : autoTargetSelf( false )
   {
      memset(chGridPos,0x00,g_iNbrMacro);
   };
   VKey vKey;
   BYTE chGridPos[g_iNbrMacro];
   bool autoTargetSelf;
};


class MacroEvent
{
public:
   virtual void ExecMacro( MacroDataUI *macroUI ) = 0;
   virtual void SaveMacro( const list< MacroDataUI * > &macroList ) = 0;
};

class MacroItemUI : public ControlUI
{
public:
   MacroItemUI( MacroEvent *imacroEvent, MacroDataUI *macroUI, string macroName );
   ~MacroItemUI();
   
   void Draw( V2SPRITEFX *fx )
   {
      if( macroUI != NULL )
      {
         macroUI->SetPos( GetPos() );
         macroUI->Draw( fx );
      }
   }
   
   DWORD          macroId;
   MacroDataUI   *macroUI;
   MacroEvent    *macroEvent;
   string         macroName;
};


class V3_MacroDlg : public BoxUI  
{
public:	
	virtual ~V3_MacroDlg();

    static V3_MacroDlg *GetInstance();
    static unsigned int DisplayM( void *null );
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
 
    void AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey);

    void MacroToSave();
    void ResetMacro( void );
    void ReloadReset( void );
    void AddMacro( MacroDataUI *macroUI, string macroName, MacroEvent *macroEvent, bool echo = true );

    

    bool CallMacro( VKey vKey );
    bool KeyExists( VKey vKey );
     

    MacroItemUI *GetMacroItemUI(int iX, int iY){return m_MacroButton[iY][iX].macroItem;}

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
   

   
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
      virtual bool RightMouseDown( MousePos mousePos );
      MacroItemUI *macroItem;
      int          iIndex;
   };
   
   friend MacroButtonUI;

   MacroButtonUI m_MacroButton[g_iNbrMacroY][g_iNbrMacroX];

   
   void RemoveFromGrid( MacroItemUI *macroItem );

    

public:

protected:

   DWORD m_dwLastMacroID;

   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;
   bool m_bDisableMacro;

   int  m_iCurrentMP;


   
   
public:
   ButtonUI  Drag_Button;

private:
   V3_MacroDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  m_btnDelMacro;

   ButtonUI  X_Button;


   ButtonUI MOnglet01Button; //Personnage
   ButtonUI MOnglet02Button; //Inventaire
   ButtonUI MOnglet03Button; //Sorts
   ButtonUI MOnglet04Button; //Professions
   ButtonUI MOnglet05Button; //Guilde
   ButtonUI MOnglet06Button; //Groupe
   ButtonUI MOnglet07Button; //Macro
   ButtonUI MOnglet08Button; //Options

   ButtonUI m_btnMPUp;
   ButtonUI m_btnMPDn;


   ListUI   macroList;
   GraphUI  macroListSelect;
   GraphUI  macroScrollUp;
   GraphUI  macroScrollDown;
   
   StaticTextUI m_strListNom;
   StaticTextUI m_strListCle;
   StaticTextUI m_strMP;



   class MacroListEvent : public EventVisitor{
   public:
      void LeftDblClicked();
   } macroListEvent;
   friend MacroListEvent;


   class DelMacroButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } delMacroButtonEvent;
   friend DelMacroButtonEvent;

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



   class TabStatsEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabStatsEvent;
   friend TabStatsEvent;
   
   class TabInvEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabInvEvent;
   friend TabInvEvent;
   
   class TabSpellEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabSpellEvent;
   friend TabSpellEvent;
   
   class TabProfessionEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabProfessionEvent;
   friend TabProfessionEvent;
   
   class TabGuildeEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabGuildeEvent;
   friend TabGuildeEvent;
   
   class TabGroupEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabGroupEvent;
   friend TabGroupEvent;
     
/*
//    class TabMacroEvent : public EventVisitor{
//    public:
//       void LeftClicked( void );
//    } tabMacroEvent;
//    friend TabMacroEvent;
*/

   class TabOptionEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabOptionEvent;
   friend TabOptionEvent;
};


