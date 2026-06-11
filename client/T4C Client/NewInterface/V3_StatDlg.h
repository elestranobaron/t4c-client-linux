// V3_StatsDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "V3_MacroDlg.h"




class V3_StatsDlg : public BoxUI  
{
public:	
	virtual ~V3_StatsDlg();

    static V3_StatsDlg *GetInstance();
    static unsigned int DisplayM( void *null);
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    //UpdateStatus de la fenetre
    void UpdateCharacterSheet( char *pFromWho );
    void UpdateCharacterSheetHP();
    void UpdateCharacterSheetMP();
    void UpdateCharacterSheetWeight();
    void UpdateCharacterSheetStat();
    void UpdateSkills( void );
    void InitializeMacro();
    void Reload();

    void CastLastSkill( void );
     

protected:
   DWORD GetColor(int iStat,int iStatB,int iUpg);
   char* GetDummyString(int iVal);

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();

private:
   // List containers.
   class SkillUI : public GraphUI{
   public:
      USER_SKILL *userSkill;
    };
   class SkillMacroUI : public MacroDataUI{
   public:
      string name;
      DWORD skillId;
   };

   void UpdateAllFramePos(int iPosX,int iPosY);
   void ManageSkillMacroButton( SkillUI *skillUI );
   void UseSkill( SkillUI *skillUI );
   static UINT UseSkillPosition( LPVOID lpData );// The use skill position callback
	static UINT UseSkillUnit( LPVOID lpData );// The use skill unit callback

   void StringToSpacesString(char *pSource,char *pDest);
   
    

protected:


   DWORD  m_dwLastSkill;

   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;


   StaticTextUI m_statL[6];
   StaticTextUI m_statR[6];
   ButtonUI     m_hlpL[6];
   ButtonUI     m_hlpR[6];
   StaticTextUI m_XP[2];
   StaticTextUI m_XPT[2];
   StaticTextUI m_NomLvl[2];
   StaticTextUI m_PtsStat[2];
   StaticTextUI m_PtsSKill[2];
   StaticTextUI m_Karma[2];
   StaticTextUI m_ptsInter;
   StaticTextUI m_ptsInterT;
   

   int m_iUpgSTR;
   int m_iUpgEND;
   int m_iUpgAGI;
   int m_iUpgINT;
   int m_iUpgWIS;
   int m_usedStatPts;

   int m_iSelectDisplay;
   
   

   
   
public:
   ButtonUI  Drag_Button;



private:
   V3_StatsDlg();
   
   GraphUI  m_BackgroundS;
   GraphUI  m_BackgroundPR;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   GraphUI  m_backIconsIP;
   GraphUI  m_backIconsIPT;
   
   ButtonUI  X_Button;


   ButtonUI upgSTR_Button;
   ButtonUI upgEND_Button;
   ButtonUI upgAGI_Button;
   ButtonUI upgWIS_Button;
   ButtonUI upgINT_Button;
   
   ButtonUI dngSTR_Button;
   ButtonUI dngEND_Button;
   ButtonUI dngAGI_Button;
   ButtonUI dngWIS_Button;
   ButtonUI dngINT_Button;

   ButtonUI statsCommit_Button;
   ButtonUI addMacro_Button;

   ButtonUI IteractionRPButton;
   ButtonUI IteractionPVPButton;

   ButtonUI OngletStat_Button;
   ButtonUI OngletPR_Button;

   ButtonUI MOnglet01Button; //Personnage
   ButtonUI MOnglet02Button; //Inventaire
   ButtonUI MOnglet03Button; //Sorts
   ButtonUI MOnglet04Button; //Professions
   ButtonUI MOnglet05Button; //Guilde
   ButtonUI MOnglet06Button; //Groupe
   ButtonUI MOnglet07Button; //Macro
   ButtonUI MOnglet08Button; //Options

   

   ListUI   skillList;
   GraphUI  skillListSelect;
   GraphUI  skillScrollUp;
   GraphUI  skillScrollDown;




   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   class StatUpgEvent : public EventVisitor{
   public:
      void LeftClicked( void );
      
      int stat;
   }  upgSTRevent,
      upgENDevent,
      upgAGIevent,
      upgWISevent,
      upgINTevent;
   
   class StatDngEvent : public EventVisitor{
   public:
      void LeftClicked( void );
      
      int stat;
   }  dngSTRevent,
      dngENDevent,
      dngAGIevent,
      dngWISevent,
      dngINTevent;

   class StatsCommitEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } statsCommitEvent;

   class IteractionRPEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } iteractionRPEvent;

   class IteractionPVPEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } iteractionPVPEvent;

   class AddMacroEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } addMacroEvent;

   class SkillEvent : public EventVisitor{
   public:
      void LeftDblClicked( void );
      void LeftClicked( void );
    } skillEvent;

   // Macro callbacks.
   class SkillMacroEvent : public MacroEvent{
      virtual void ExecMacro( MacroDataUI *graphUI );
      virtual void SaveMacro( const list< MacroDataUI * > &macroList );
   } skillMacroEvent;
   

   class OngletStatEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } ongletStatEvent;

   class OngletPREvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } ongletPREvent;
   


   friend DragEvent;
   friend StatUpgEvent;
   friend StatDngEvent;
   friend StatsCommitEvent;
   friend AddMacroEvent;
   friend SkillEvent;
   friend SkillMacroEvent;
   friend OngletStatEvent;
   friend OngletPREvent;
   friend IteractionRPEvent;


/*
//    class TabStatsEvent : public EventVisitor{
//    public:
//       void LeftClicked( void );
//    } tabStatsEvent;
//    friend TabStatsEvent;
*/
   
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
   
   class TabOptionEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } tabOptionEvent;
   friend TabOptionEvent;




};


