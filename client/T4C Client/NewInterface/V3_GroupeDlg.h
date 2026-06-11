// V3_GroupeDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"


class GroupMember
{
public:
   DWORD       id;
   WORD        level;
   WORD        hpPercent;
   bool        leader;
   std::string name;
};

class HpMemberUI : public ControlUI, public GroupMember
{
public:
   HpMemberUI( void );
   void SetMember( const GroupMember &member )
   {
      id = member.id;
      level = member.level;
      hpPercent = member.hpPercent;
      leader = member.leader;
      name = member.name;
   }
   
   // Overload the drawing of the HP.
   void Draw( V2SPRITEFX *vsfFX );
private:
   GraphUI emptyBar;
   GraphUI fullBar;
};

class TextMemberUI : public StaticTextUI, public GroupMember{
public:
   void SetMember( const GroupMember &member )
   {
      id = member.id;
      level = member.level;
      hpPercent = member.hpPercent;
      leader = member.leader;
      name = member.name;
   }
   
   void Draw( V2SPRITEFX *vsfFX );
    };

class V3_GroupeDlg : public BoxUI  
{
public:
   

	 virtual ~V3_GroupeDlg();

    static V3_GroupeDlg *GetInstance();
    static unsigned int DisplayM( void *null );
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();

    void Join( DWORD sourceID, std::string sourceName );
    void UpdateMembers( const std::list< GroupMember > &members, bool autoShare , bool bDisband = false);
    void UpdateMemberHp( DWORD id, WORD hpPercent );
    void UpdateAutoShare( bool autoShare );

    void ConfirmReturn(bool bOK,UINT uiShowCodeP);

    
     

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void ResolveGraphPatches( void );

private:
   // List containers.

public:
   ListUI   m_ListMember;
    

protected:

   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   int  m_iMemberSize;
   bool m_bAutoShare;
   bool m_bWeAreTheLeader;
   BOOL m_bShowDisableSplit;



   
   
public:
   ButtonUI  Drag_Button;


private:
   V3_GroupeDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   static unsigned int InvitePlayer( void *data );
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  X_Button;


   
   GraphUI  memberSelectionGraphPatch;
   GraphUI  m_BtnSplitDisabled;

   ButtonUI  m_btnInvite;
   ButtonUI  m_btnQuitter;
   ButtonUI  m_btnJeter;
   ButtonUI  m_btnSeparer;
   ButtonUI  m_btnInviteD;
   ButtonUI  m_btnQuitterD;
   ButtonUI  m_btnJeterD;
   ButtonUI  m_btnSeparerD;

   ButtonUI MOnglet01Button; //Personnage
   ButtonUI MOnglet02Button; //Inventaire
   ButtonUI MOnglet03Button; //Sorts
   ButtonUI MOnglet04Button; //Professions
   ButtonUI MOnglet05Button; //Guilde
   ButtonUI MOnglet06Button; //Groupe
   ButtonUI MOnglet07Button; //Macro
   ButtonUI MOnglet08Button; //Options

   StaticTextUI m_strTitre[3];

   



   class InviteButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } inviteButtonEvent;
   
   class LeaveButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } leaveButtonEvent;
   
   class ExpelButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );        
   } expelButtonEvent;
   
   class AutoShareButtonEvent : public EventVisitor{
   public:
      void LeftClicked( void );
    } autoShareButtonEvent;

   friend LeaveButtonEvent;
   friend InviteButtonEvent;
   friend ExpelButtonEvent;
   friend AutoShareButtonEvent;



   


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
   
/*
//    class TabGroupEvent : public EventVisitor{
//    public:
//       void LeftClicked( void );
//    } tabGroupEvent;
//    friend TabGroupEvent;
*/
   
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


