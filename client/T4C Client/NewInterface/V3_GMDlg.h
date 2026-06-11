// V3_GMDlg.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BoxUI.h"
#include "ButtonUI.h"
#include "GraphUI.h"
#include "ListUI.h"
#include "StaticTextUI.h"



#define NBR_GM_MAP     8


typedef struct _sGMMsgList
{
   DWORD dwMessageID;
   char strTimeStamp[50];
   char strPlayerName[50];
   char strMessage[255];
}sGMMsgList;

typedef struct _sQuestflagList
{
   unsigned short ushID;
   char strName[100];
}sQuestflagList;

typedef struct _sOptList
{
   char strName[100];
   BYTE chOpt;
}sOptList;

typedef struct _sEventsList
{
   char strName[256];
   long lFlagID;
   long lStopValue;
   long lStatus;
}sEventsList;


typedef struct _sPlayerPos
{
   char strName[100];
   WORD wX;
   WORD wY;
   WORD wW;
}sPlayerPos;


class V3_GMDlg : public BoxUI  
{
public:	
	virtual ~V3_GMDlg();


    static V3_GMDlg *GetInstance();
    static unsigned int Display( void *null,bool putForeground = true,int dwShowWhat = 0,int dwValDef = 0);

    virtual void Draw( V2SPRITEFX *vsfFX );
    void Hide();
    int          TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise);

    void         LockList(){EnterCriticalSection(&crLockList);};
    void         UnlockList(){LeaveCriticalSection(&crLockList);};
    void         LockListL(){EnterCriticalSection(&crLockListL);};
    void         UnlockListL(){LeaveCriticalSection(&crLockListL);};

    void         ClearFlagGMMsg();
    void         ClearFlagSkinList();
    void         UpdateQuestFlagSkinList(int iIndex,unsigned short ushID,char *pstrName);
    void         ListToControlGM(bool bAll = true, char *pstrKeyWord = NULL);
    void         ListToControlGMMsg();
    
    void         UpdateGMMsgList(DWORD dwID,char *pstrTime,char *pstrName,char *pstrMsg);

    int          GetWaitWhat(){return m_iCurrentWaitWhat;}
    void         SetWaitWhat(int iW){m_iCurrentWaitWhat = iW;}
    void         UpdateDisplayGM(bool bAll = true, char *pstrKeyWord = NULL);
    void         UpdateDisplayGMMsg();
    
    void         ClearEventsList();
    void         UpdateEventsList(char *pstrName, long lFlagID, long lStopValue, long lStatus);
    void         EventsListToControl();
    void         AddEventsParameters(int iID,char *pstrName, long lFlagID, long lStopValue, long lStatus);
    
    void         ClearOptList();
    void         UpdateOptList(char *pstrName, BYTE chOpt);
    void         OptListToControl();
    void         AddOptParameters(int iID,char *pstrName, int dwStatus);
    
   
    void         ClearPosList();
    void         AddPlayerPos(char *pstrName,WORD wX, WORD wY, WORD wW);
    void         ReadyPosList();
        

    void AskValueReturn(bool bOK,UINT uiShowCodeP,char *pStrValue1,char *pStrValue2,VKey vKey);

    std::vector <sQuestflagList>  m_vListDraw;
    std::vector <sGMMsgList>      m_vListDrawGMMsg;

    ListUI   flagListCtrl;

protected:

   void StartDragDlg( MousePos mousePos );
   void StopDragDlg();
   void UpdateDrawStatus();
   void ReadGMMapBDImage();
   void ReadImageFile(int IndexImage, BYTE *pImage, BYTE *pPal);

private:
   // List containers.

   std::vector <sQuestflagList>  m_vQuestFlagSkinList;
   std::vector <sQuestflagList>  m_vQuestFlagSkinListL;
   std::vector <sOptList>        m_vOptList;
   std::vector <sPlayerPos>      m_vOptPlayerPos;
   std::vector <sGMMsgList>      m_vOptGMMsg;
   std::vector <sGMMsgList>      m_vOptGMMsgL;
   std::vector <sEventsList>     m_vEventsList;
   

protected:
   CRITICAL_SECTION  crLockList;
   CRITICAL_SECTION  crLockListL;
   int               m_iCurrentWaitWhat;

   int  m_iXPos;
   int  m_iYPos;
   int  m_iXPosSD;
   int  m_iYPosSD;
   bool m_bDragDialog;

   bool m_bUpdateAll;
   bool m_bNeedUpdateList;
   bool m_bNeedUpdateListGMMsg;
   char m_strKeyWorld[512];

   

   
   
public:
   ButtonUI  Drag_Button;

   typedef struct _sImages
   {
      char strName[255];
      int  iOffset;
   }sImagesGMMap;

public:
   vector< sImagesGMMap > m_vImageGMMap;


private:
   V3_GMDlg();
   void UpdateAllFramePos(int iPosX,int iPosY);
   
   GraphUI  m_TitleIcon;
   
   ButtonUI  X_Button;




   

   int       m_dwShowWhat;
   GraphUI   m_GMMapPageB;
   GraphUI   m_GMMapFlagSkin;
   GraphUI   m_GMMapOptSkin;
   GraphUI   m_GMMsgBack;
   GraphUI   m_GMDrawBack;
   GraphUI   m_DrawTile[46][36];
   
   ButtonUI prevButton;
   ButtonUI nextButton;
   ButtonUI flagButton;
   ButtonUI skinButton;
   ButtonUI itemsButton;
   ButtonUI monstersButton;
   ButtonUI spellsButton;
   ButtonUI npcsButton;
   ButtonUI optionsButton;
   ButtonUI eventsButton;
   ButtonUI ppButton;
   ButtonUI GMRButton;
   ButtonUI GMDButton;
   ButtonUI GMDTileButton;
   ButtonUI TeleportBtn;
   ButtonUI DrawBtnM;
   ButtonUI DrawBtnI;
   ButtonUI m_BtnCB;
   ButtonUI m_btnMIName;
   StaticTextUI m_stMIName;
   char strMIName[512];
   
   ButtonUI allButton;
   ButtonUI kewWordButton;

   ButtonUI RefreshButton;
   ButtonUI CloseButton;
   
   
   GraphUI  flagListScrollUp;
   GraphUI  flagListScrollDown;
   GraphUI  flagListSelectionGraphPatch;
   
   ListUI   optionsList;
   GraphUI  optionsListingSelectionGraphPatch;
   GraphUI  optionsListScrollUp;
   GraphUI  optionsListScrollDown;

   ListUI   eventsList;
   GraphUI  eventsListingSelectionGraphPatch;
   GraphUI  eventsListScrollUp;
   GraphUI  eventsListScrollDown;

   //Gmmsg
   ListUI   gmmsgList;
   GraphUI  gmmsgListingSelectionGraphPatch;
   GraphUI  gmmsgListScrollUp;
   GraphUI  gmmsgListScrollDown;
   
   CV2Sprite m_sprGMMap;
   BOOL m_bShowCurrentGMWorld;
   BOOL bNeedToUpdateMap;
   int  m_dwCurrentGMWorld;
   int  m_iLastDiaplayedImageMap;
   
   BYTE *m_pGMMapData;
   BYTE *m_pGMMapDataPal;
   
   char  m_strHeaderText[100];
   char  m_strMapCoord[100];

   typedef struct _sDrawData
   {
      char strName[128];
      DWORD dwDraw[46][36];
   }sDrawData;

   std::vector <sDrawData>  m_DrawList;
   
   void LoadFileData(char *pstrName);
   bool MouseToTileCoord(long &iX, long &iY);
   bool MouseToMapCoord(long &iX, long &iY);
   bool MapCoordToMouse(int iOffX,int iOffY,long iXs, long iYs, long &iX, long &iY);

   void ComboReturn(UINT uiShowCodeP,char *pStrValue);
   
  
   bool m_bLockPlayerPos;

   BOOL m_bTileMosaic[46][36];
   

 
   

   class DragEvent : public EventVisitor
   {
      public:
         void LeftClicked( void );        
         void StartDragLButtonDown( int mX, int mY );
   } dragEvent;

   friend DragEvent;

   class DrawBtnMButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } drawBtnMButtonEvent;

   class DrawBtnIButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   } drawBtnIButtonEvent;


   class MINameButtonEvent : public EventVisitor
   {
   public:
      void LeftClicked( void );
   }miNameButtonEvent;
 
   class PrevButtonEvent : public EventVisitor
    {
      public:
           void LeftClicked( void );
    } prevButtonEvent;

    class NextButtonEvent : public EventVisitor
    {
      public:
           void LeftClicked( void );
    } nextButtonEvent;

    class FlagButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } flagButtonEvent;

    class SkinButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } skinButtonEvent;

    class ItemsButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } itemsButtonEvent;

    class MonstersButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } monstersButtonEvent;

    class SpellsButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } spellsButtonEvent;

    class NpcsButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } npcsButtonEvent;


    class OptionsButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } optionsButtonEvent;


    class EventsButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } eventsButtonEvent;

    class PPButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } ppButtonEvent;

    class GMRButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } gmrButtonEvent;

    class GMDButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } gmdButtonEvent;


    class TeleportEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } teleportEvent;

    class TileEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } tileEvent;

    class AllButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } allButtonEvent;

    class KewWordButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } kewWordButtonEvent;

    class RefreshButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } refreshButtonEvent;

    class CloseButtonEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );
    } closeButtonEvent;

    class BtnCBEvent : public EventVisitor
    {
    public:
       void LeftClicked( void );        
    } btnCBEvent;

    class FlagListEvent : public EventVisitor{
    public:
       void LeftClicked( void );
    } flagListEvent;

    class OptionsListEvent : public EventVisitor{
    public:
       void LeftClicked( void );
    } optionsListEvent;
    friend OptionsListEvent;

    class EventsListEvent : public EventVisitor{
    public:
       void LeftClicked( void );
    } eventsListEvent;
    friend EventsListEvent;

    class GMMsgListEvent : public EventVisitor{
    public:
       void LeftClicked( void );
    } gmmsgListEvent;
    friend GMMsgListEvent;

    class OptDlgUI : public StaticTextUI{
    public:
       std::string optionID;
       bool        bState;
       int         dwID;
    };

    class OptDlgToggleUI : public ButtonUI{
    public:
       OptDlgToggleUI() : ButtonUI( EmptyEvent::GetInstance() ){};
       
       std::string optionID;
       bool        bState;
    };

    class StrListTmp : public GameUI
    {
    public:
       char strTmp[2];
    } strListTmp;
    
    
    friend DrawBtnMButtonEvent;
    friend DrawBtnIButtonEvent;
    friend MINameButtonEvent;
    friend PrevButtonEvent;
    friend NextButtonEvent;
    friend FlagButtonEvent;
    friend SkinButtonEvent;
    friend ItemsButtonEvent;
    friend MonstersButtonEvent;
    friend SpellsButtonEvent;
    friend NpcsButtonEvent;
    friend OptionsButtonEvent;
    friend EventsButtonEvent;
    friend PPButtonEvent;
    friend GMRButtonEvent;
    friend GMDButtonEvent;
    friend TeleportEvent;
    friend TileEvent;
    friend AllButtonEvent;
    friend KewWordButtonEvent;
    friend RefreshButtonEvent;
    friend CloseButtonEvent;
    friend StrListTmp;
    friend BtnCBEvent;

};


