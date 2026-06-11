// V3_GuildDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"

typedef struct _sGuildPermission
{
   DWORD   Leader      :1;
   DWORD   CanInvite   :1;
   DWORD   CanKick     :1;
   DWORD   ViewChest   :1;
   DWORD   CanDeposit  :1;
   DWORD   CanTake     :1;
   DWORD   CanWriteNote:1;
   DWORD   CanChPerm   :1;
   DWORD   CanSeeLog   :1;
   DWORD   Reserved04:23;
} sGuildPermission;

typedef union _uGuildPermission
{
   DWORD  dwVal;
   sGuildPermission Perm;
}uGuildPermission;

const int g_CONFIRM_ACCEPT_INVITE = 0;
const int g_CONFIRM_LEAVE_GUILD   = 1;
const int g_CONFIRM_KICK_GUILD    = 2;

#define GUILD_FONDATEUR_RIGHT  0x000001FF

class V3_GuildDlg : public BoxUI  
{
public:	
	virtual ~V3_GuildDlg();

    static V3_GuildDlg *GetInstance();
    static unsigned int DisplayM( void *null );
    static unsigned int Display( void *null );

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Show();

    void UpdateRangDisplay(int iRandSelect,char*pR1,char*pR2,char*pR3,char*pR4,bool bReset);
    void UpdateRang(int iRandSelect,bool bReset);
    
    void UpdatePermDisplay(DWORD dwPermission,bool bReset);

    void ClearLogList();
    void AddLogs(char *pstrDatem ,char *pstrLogs);
    void DisplayLog();


    class User{
    public: 
       std::string strUserName;
       DWORD       dwTitle;
       DWORD       dwID;
       DWORD       dwPermission;
       bool        bOnline;
    };

    void ConfirmReturn(bool bOK,UINT uiShowCodeP);
    void AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey);

    void Join( DWORD sourceID, std::string sourceName , std::string GuildName );
    void UpdateGuildList(BYTE chShow,const std::vector< User > &GuildUsers, char *pstrGuildName, char *pstrNotes, DWORD dwPermission,
                         char *pstrR1,char *pstrR2,char *pstrR3,char *pstrR4,char *pstrR5);


    
    class GuildMemberUI : public StaticTextUI, public User{
    public:
       void SetMember( const User &member )
       {
          strUserName  = member.strUserName;
          dwTitle      = member.dwTitle;
          dwID         = member.dwID;
          dwPermission = member.dwPermission;
          bOnline      = member.bOnline;
       }
    };

    class UserUI : public StaticTextUI
    {
    public:
       std::string userID;
    };

    class RangUI : public StaticTextUI
    {
    public:
       std::string optionID;
       bool        bState;
       int         dwID;
    };
    class SettingRangToggle : public ButtonUI
    {
    public:
       SettingRangToggle() : ButtonUI( EmptyEvent::GetInstance() ){};
       
       std::string optionID;
       bool        bState;
       };

     

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void AddRangParam(char *pstrName,int dwID, int dwStatus);
   void AddPermParam(char *pstrName,int dwID, int dwStatus);

private:
   // List containers.
   static unsigned int InvitePlayer( void *data ); 

protected:


   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;


   std::string selectedUser;
   char m_strLastNote[255];
   char m_strGuildName[50];
   char m_strRangName[5][100];
   int  m_iKickoutID;

   char m_strRangN[4][100];
   int  m_iSelect[4];
   int  m_dwUserID;
   int  m_iCurrentTitle;

   uGuildPermission m_dwUserPermission;
   uGuildPermission m_dwCurrentPermission;

   CRITICAL_SECTION  crLock;

   
   
public:
   ButtonUI  Drag_Button;



private:
   V3_GuildDlg();
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



   ListUI   m_GuildList;
   GraphUI  m_GuildListingSelect;
   GraphUI  m_GuildListScrollUp;
   GraphUI  m_GuildListScrollDown;

   ListUI   m_ListPerm;
   ListUI   m_ListTitre;

   StaticTextUI m_strNotes;

   StaticTextUI m_strNom;
   StaticTextUI m_strRang;
   StaticTextUI m_strSetting;
   StaticTextUI m_strTitre;

   ButtonUI m_btnInviter;
   ButtonUI m_btnQuitter;
   ButtonUI m_btnKick;
   ButtonUI m_btnChangeNote;
   ButtonUI m_btnChangeSettings;
   ButtonUI m_btnViewLog;

   class PermListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } permListEvent;
   friend PermListEvent;
   class TitreListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } titreListEvent;
   friend TitreListEvent;
    

   class GuildListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
   } guildListEvent;
   friend GuildListEvent;

   class InviteButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } inviteButtonEvent;
   friend InviteButtonEvent;
   
   class LeaveButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
    } leaveButtonEvent;
   friend LeaveButtonEvent;

   class KickButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } kickButtonEvent;
   friend KickButtonEvent;

   class ChangeNoteButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } changeNoteButtonEvent;
   friend ChangeNoteButtonEvent;

   class ChangeSettingsButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } changeSettingsButtonEvent;
   friend ChangeSettingsButtonEvent;

   class ViewLogButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } viewLogButtonEvent;
   friend ViewLogButtonEvent;

   

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
   
/*
//    class TabGuildeEvent : public EventVisitor{
//    public:
//       void LeftClicked( void );
//    } tabGuildeEvent;
//    friend TabGuildeEvent;
*/
   
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

   class LogsPopup : public BoxUI
   {
   public:
      LogsPopup();
      void Draw( V2SPRITEFX *vsfFX );
      
      void ClearLogList();
      void AddLogs(char *pstrDate ,char *pstrLogs);
      void DisplayLog();
      
      
   private:
      class LogsUI : public StaticTextUI
      {
      public:
         std::string strTxt;
         int         dwID;
      };
      
      class LogListEvent : public EventVisitor
      {
      public:
         void LeftClicked( void );
      } logListEvent;
      
      
      ButtonUI X_Button;
      GraphUI  LogsBack;
      ListUI   ListLogs;
      GraphUI  ListScrollUp;
      GraphUI  ListScrollDown;
      StaticTextUI m_strT;
      StaticTextUI m_strL;
      
   } logsPopup;
   
   friend LogsPopup;
   
 


};


