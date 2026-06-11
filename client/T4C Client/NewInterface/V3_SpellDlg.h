// V3_SpellDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include "V3_MacroDlg.h"



#define TARGET_UNIT_ANY								0
#define TARGET_UNIT_OBJECT							1
#define TARGET_UNIT_NPC								2
#define TARGET_UNIT_PC								3
#define TARGET_UNIT_LIVING							4
#define TARGET_SELF									5
#define TARGET_POSITION								6
#define TARGET_UNIT_PC_NONSEL						7
#define TARGET_UNIT_ANY_NONSELF					    8
#define TARGET_UNIT_LIVING_NONSELF		 		    9
#define TARGET_UNIT_LIVING_MONSTER				    10
#define TARGET_UNIT_LIVING_MONSTER_NONSELF          11
#define TARGET_UNIT_LIVING_PLAYER				    12
#define TARGET_UNIT_ANY_PLAYER					    13
#define TARGET_GROUP_UNIT                           14
#define TARGET_GROUP_SELF                           15
#define TARGET_GROUP_POSITION                       16
#define TARGET_NONGROUP_UNIT                        17
#define TARGET_NONGROUP_SELF                        18
#define TARGET_NONGROUP_POSITION                    19


class V3_SpellDlg : public BoxUI  
{
public:	
	virtual ~V3_SpellDlg();

    static V3_SpellDlg *GetInstance();
    static unsigned int DisplayM( void *null );
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void InitializeMacro( void );
    void Reload();

    enum TargetPriority{
       monsterPriority = 0,
          equalPriority   = 1,
          playerPriority  = 2
    }; 

    struct Spell
    {
       string name;
       string desc;
       WORD   Id;
       DWORD  duration;
       enum{ mental, physical } type;
       BYTE   targetType;
       BYTE   AttackSpell;
       WORD   manaCost;
       WORD   level;
       DWORD  icon;
       
       enum Element
       {
          fire = 1, water = 2, air = 3, earth = 4, light = 5, dark = 6, normal = 7
       } element;
       
       inline bool operator < ( const Spell &spell2 ) const;
    };
    
    void UpdateSpells( const list< Spell > &spellList );

    bool CastSpell( DWORD spellId, bool autoTargetSelf );
    void CastLastSpell();
    void RequestSpellList();
     

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();



private:
   void UpdateSpellPages( void );
   void UpdateIndexes( void );
   void ClearIndex( Spell::Element element );
   void CastSpell( const Spell &spell, bool autoTargetSelf, bool noCallback = false );
   static UINT CastSpellPosition( void *data );
   static UINT CastSpellUnit( void *data );
    


public:
   ButtonUI  Drag_Button;

protected:


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;


   int  m_iCurrentPage;
   list< Spell > spellList;
   DWORD  lastSpell;
   MouseDevice mouse;
   AnimV2Sprite V3CastCursor;



private:
   V3_SpellDlg();
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

   ButtonUI m_btnLArrow;
   ButtonUI m_btnRArrow;

   ButtonUI m_btnAir;
   ButtonUI m_btnFire;
   ButtonUI m_btnEarth;
   ButtonUI m_btnWater;
   ButtonUI m_btnLight;
   ButtonUI m_btnDark;
   ButtonUI m_btnNormal;

   ButtonUI m_btnM0;
   ButtonUI m_btnM1;
   ButtonUI m_btnM2;
   ButtonUI m_btnM3;
   ButtonUI m_btnM4;
   ButtonUI m_btnM5;
   ButtonUI m_btnM6;
   ButtonUI m_btnM7;
   ButtonUI m_btnM8;
   ButtonUI m_btnM9;
   

   GraphUI m_SignetAir;
   GraphUI m_SignetFire;
   GraphUI m_SignetEarth;
   GraphUI m_SignetWater;
   GraphUI m_SignetLight;
   GraphUI m_SignetDark;
   GraphUI m_SignetNormal;


   
   class SpellClickEvent : public EventVisitor{
   public:
      SpellClickEvent( int newSpellNum ) : spellNum( newSpellNum ){}
      
      void LeftDblClicked();
      void LeftClicked();
      
      int spellNum;
   }  spellEvent0,
      spellEvent1,
      spellEvent2,
      spellEvent3,
      spellEvent4,
      spellEvent5,
      spellEvent6,
      spellEvent7,
      spellEvent8,
      spellEvent9;
   friend SpellClickEvent;

   // Internal SpellPage controls.
   class SpellPageUI : public ControlUI
   {
      public:
         SpellPageUI( EventVisitor &event );
      
         Spell spell;
      
         virtual void Draw( V2SPRITEFX *vsfFX );
         virtual bool LeftMouseUp( MousePos mousePos );
         virtual void SetPos( FramePos framePos );
      
         void Select( bool selectState );
      
         inline void FillSpellPage( const Spell &spell );
         inline void ClearPage( void );
      
     
      private:
         EventVisitor &spellEvent;
      
         StaticTextUI m_stName;
         StaticTextUI m_stLevel[2];
         StaticTextUI m_stDuration[2];
         StaticTextUI m_stType[2];
         StaticTextUI m_strManaCost[2];
         GraphUI      icon;
      
         FramePos initialIconPos;

         BOOL m_bSelectState;
   }   PageSpell0,PageSpell1,PageSpell2,PageSpell3,PageSpell4,PageSpell5,PageSpell6,PageSpell7,PageSpell8,PageSpell9;  


   class ElementEvent : public EventVisitor{
   public:
      ElementEvent( Spell::Element ielement ) : element( ielement )
      {}
      
      void LeftClicked( void );
      
      Spell::Element element;
   } airEvent, 
     fireEvent, 
     earthEvent, 
     waterEvent, 
     lightEvent, 
     darkEvent, 
     normalEvent;
   
    friend ElementEvent;



   class LArrowEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } lArrowEvent;
   friend LArrowEvent;
   class RArrowEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } rArrowEvent;
   friend RArrowEvent;


   class M0ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m0ButtonEvent;
   friend M0ButtonEvent;

   class M1ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m1ButtonEvent;
   friend M1ButtonEvent;

   class M2ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m2ButtonEvent;
   friend M2ButtonEvent;

   class M3ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m3ButtonEvent;
   friend M3ButtonEvent;

   class M4ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m4ButtonEvent;
   friend M4ButtonEvent;

   class M5ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m5ButtonEvent;
   friend M5ButtonEvent;

   class M6ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m6ButtonEvent;
   friend M6ButtonEvent;

   class M7ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m7ButtonEvent;
   friend M7ButtonEvent;

   class M8ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m8ButtonEvent;
   friend M8ButtonEvent;

   class M9ButtonEvent : public EventVisitor{
   public:
      void LeftClicked();
   } m9ButtonEvent;
   friend M9ButtonEvent;

   // Macro callbacks.
   class SpellMacroEvent : public MacroEvent{
      virtual void ExecMacro( MacroDataUI *graphUI );
      virtual void SaveMacro( const list< MacroDataUI * > &macroList );
   } spellMacroEvent;
   
   friend SpellMacroEvent;

   class SpellMacroUI : public MacroDataUI{
   public:
      Spell spell;
      WORD  iconId;
    };


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

/*
//    class TabSpellEvent : public EventVisitor{
//    public:
//       void LeftClicked( void );
//    } tabSpellEvent;
//    friend TabSpellEvent;
*/

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


