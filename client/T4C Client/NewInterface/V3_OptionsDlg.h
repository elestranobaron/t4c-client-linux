// V3_OptionsDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "SliderUI.h"
#include "V3_AskValueDlg.h"
#include "V3_ConfirmDlg.h"


class V3_OptionsDlg : public BoxUI  
{
public:	
	virtual ~V3_OptionsDlg();

    static V3_OptionsDlg *GetInstance();
    static unsigned int DisplayM( void *null );
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey);
    void ConfirmReturn(bool bOK,UINT uiShowCodeP);
    void UpdatePlayerList();
    void Reload( void );
     

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   
   void UpdateOptionsList(bool bResize);
   void AddOptionsParameters(char *pstrName,int dwID, int dwStatus);
   void AddPlayer(char *pstrName,UINT uiID,int dwStatus);

private:
   // List containers.
    

protected:


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;
   bool m_bNMSDisplay;


   
   
public:
   ButtonUI  Drag_Button;



private:
   V3_OptionsDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  X_Button;


   ButtonUI MOnglet01Button; //Personnage
   ButtonUI MOnglet02Button; //Inventaire
   ButtonUI MOnglet03Button; //Sorts
   ButtonUI MOnglet04Button; //Professions
   ButtonUI MOnglet05Button; //Guilde
   ButtonUI MOnglet06Button; //Groupe
   ButtonUI MOnglet07Button; //Macro
   ButtonUI MOnglet08Button; //Options

   
   ListUI   optionsList;
   GraphUI  optionsListSelect;
   GraphUI  optionsScrollUp;
   GraphUI  optionsScrollDown;

   ListUI   reloadList;
   GraphUI  reloadListSelect;
   GraphUI  reloadScrollUp;
   GraphUI  reloadScrollDown;

   SliderUI m_sldMusic;
   SliderUI m_sldEffect;
   SliderUI m_sldAmbiant;
   SliderUI m_sldBright;

   ButtonUI m_btnCDMusicE;
   ButtonUI m_btnCDMusicD;
   ButtonUI m_btnLogFileName;

   //ButtonUI m_btnQuitter;
   ButtonUI m_btnResetGUI;
   ButtonUI m_btnLowCPU;
   ButtonUI m_btnDefault;
   ButtonUI m_btnReload;
   ButtonUI m_btnNMSGold;
   ButtonUI m_btnPanier;



   StaticTextUI m_stHParam;
   StaticTextUI m_stHSon;
   StaticTextUI m_stHReload;
   StaticTextUI m_stLuminosite;
   StaticTextUI m_stLogFileName;
   StaticTextUI m_stMusic;
   StaticTextUI m_stEffect;
   StaticTextUI m_stAmbiant;
   StaticTextUI m_stCDMusic;
   StaticTextUI m_stLogFileNameD;

   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;


   class OptionsListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } optionListEvent;
   friend OptionsListEvent;

   class ReloadListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } reloadListEvent;
   friend ReloadListEvent;

   class OptDlgUI : public StaticTextUI{
   public:
      std::string optionID;
      DWORD       dwState;
      int         dwID;
   };
   class OptDlgToggleUI : public ButtonUI{
   public:
      OptDlgToggleUI() : ButtonUI( EmptyEvent::GetInstance() ){};
      
      std::string optionID;
      bool        bState;
    };

   class MusicVolumeEvent : public EventVisitor{
      public:
         void LeftClicked( void );
   } musicVolumeEvent;

   class AmbiantVolumeEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } ambiantVolumeEvent;

   

   class EffectEvent : public EventVisitor{
      public:
         void LeftClicked( void );
   } effectEvent;

   class BrightnessEvent : public EventVisitor{
      public:
         void LeftClicked( void );
   } brightnessEvent;

   class CdMusicEvent : public EventVisitor{
   public:
	   void LeftClicked( void );
    } cdMusicEvent;

   class FileNameEvent : public EventVisitor{
   public:
      void LeftClicked();
   } fileNameEvent;

   class LowCpuButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } lowCpuButtonEvent;

   class DefaultButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } defaultButtonEvent;
   
   class ReloadButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } reloadButtonEvent;
   
   class NmsGOLDButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } nmsGOLDButtonEvent;
   
   class NmsPanierButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
    } nmsPanierButtonEvent;


   class QuitEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } quitButtonEvent;


   class ResetGUIButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
    } resetGUIButtonEvent;
   
    

   friend MusicVolumeEvent;
   friend AmbiantVolumeEvent;
   friend EffectEvent;
   friend BrightnessEvent;
   friend CdMusicEvent;
   friend FileNameEvent;    

   friend LowCpuButtonEvent;
   friend ReloadButtonEvent;
   friend NmsGOLDButtonEvent;
   friend NmsPanierButtonEvent;
   friend QuitEvent;
   friend ResetGUIButtonEvent;



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
   
   class TabMacroEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabMacroEvent;
   friend TabMacroEvent;
   
/*
//    class TabOptionEvent : public EventVisitor{
//    public:
//       void LeftClicked( void );
//    } tabOptionEvent;
//    friend TabOptionEvent;
*/

};





