// V3_ChatDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"
#include <string>
#include <list>
#include <set>
#include <map>



class V3_ChatDlg : public BoxUI  
{
public:	
	virtual ~V3_ChatDlg();

    static V3_ChatDlg *GetInstance();
    static unsigned int DisplayM( void *null );
    static unsigned int Display( void *null );

    typedef std::set< std::string, StringCompareI > IgnoreSet;

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    void Show();
    void ClientInitialize();
    void SetupServerAllParam();
    void AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey);

    void EnterChatterChannel(string channelID, string pwd,DWORD  color,bool   bEnable,bool   bUpdateList,bool   bClearCCList);
    bool IsUserIgnored( std::string userName );
   
    class User{
    public:
       std::string userName;
       std::string title;
       std::string Guild;
       bool        UserState;
       char        chGodLevel;
	   char        chHidden;
    };
    class Channel{
    public:
       std::string channelID;
       bool        listenState;
    };


    void ClearChannelList();
    void UpdateChannels    ( const std::vector< Channel > &suscribedChannels );
    void UpdateChannelUsers(const std::vector< User > &channelUsers);
    DWORD GetChannelColor( std::string channelId );
    std::string GetSelectedChannel(){return selectedChannel;}
    IgnoreSet *GetIgnoreList(){return &ignoreList;}
    void CCToSave(); 

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


   class RegChannel
   {
      public:
         std::string channelID;
         std::string pwd;
         DWORD color;
         bool  bActive;
      
         bool operator < ( const RegChannel &r ) const
         {
            return( _stricmp( channelID.c_str(), r.channelID.c_str() ) < 0 );
         }
   };
   
   std::set< RegChannel > regChannels;
   //std::map< std::string, DWORD > savedColor;
   //std::map< std::string, bool  > savedStatus;
   
   class ChannelUI : public StaticTextUI
   {
      public:
         enum color
         {
            cyan   = CC_COL_0
         };
      
         std::string channelID;
         bool        listenState;
         color       channelColor;
   };

   std::string selectedChannel;
   std::string selectedUser;
   IgnoreSet ignoreList;

   
   

   class ChannelToggleUI : public ButtonUI
   {
   public:
      ChannelToggleUI() : ButtonUI( EmptyEvent::GetInstance() ){};
      
      std::string channelID;
      bool        listenState;
   };
   
   class UserUI : public StaticTextUI{
   public:
      std::string userID;
   };
   


public:
   ButtonUI  Drag_Button;


private:
   V3_ChatDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_BackgroundS;
   GraphUI  m_backIconsS;
   GraphUI  m_backIconsPR;
   GraphUI  m_TitleIcon;
   GraphUI  m_backIconsSH;
   GraphUI  m_backIconsSF;
   
   ButtonUI  X_Button;


   ButtonUI m_btnAddCC;
   ButtonUI m_btnDelCC;
   ButtonUI m_btnTogglePage;
   ButtonUI m_btnRequete;


   StaticTextUI m_stHCC;
   StaticTextUI m_stHUName;
   StaticTextUI m_stHUTitle;
   StaticTextUI m_stHUGuild;
   StaticTextUI m_stHInfo;

   ListUI   m_ListCC;
   GraphUI  m_ListCCSelectionGraphPatch;
   GraphUI  m_ListCCScrollUp;
   GraphUI  m_ListCCScrollDown;

   ListUI   m_ListUser;
   GraphUI  m_ListUserSelectionGraphPatch;
   GraphUI  m_ListUserScrollUp;
   GraphUI  m_ListUserScrollDown;



   static UINT  WINAPI ChannelRefreshThread( void *nil );
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

   class ChannelListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
      void LeftDblClicked( void );
    } channelListEvent;
   friend ChannelListEvent;

   class UserListEvent : public EventVisitor{
   public:
      void LeftClicked( void );
      void LeftDblClicked( void );
    } userListEvent;
   friend UserListEvent;

   class BtnAddCCEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnAddCCEvent;
   friend BtnAddCCEvent;

   class BtnDelCCEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnDelCCEvent;
   friend BtnDelCCEvent;

   class BtnTogglePageEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnTogglePageEvent;
   friend BtnTogglePageEvent;

   class BtnRequeteEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );        
   } btnRequeteEvent;
   friend BtnRequeteEvent;

};


