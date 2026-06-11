#pragma warning( disable : 4291 )
#pragma warning(disable:4786)

#include "pch.h"
#include "Mouse.h"
#include "TFCPlayer.h"
#include "Bitmap.h"
#include "TileSet.h"
#include "VisualObjectList.h"
#include "TFCPacket.h"
#include "TFCSocket.h"
#include "MacroHandler.h"
#include "TileSet.h"
#include "Franck.h"
#include "pf.h"
#include <stdio.h>
#include "Global.h"
#include "Packet.h"
#include "UIMouseEvent.h"
#include "SaveGame.h"
extern CSaveGame g_SaveGame;

#include "NewInterface/RootBoxUI.h"
#include "NewInterface/V3_PacmanDlg.h"
#include "NewInterface/V3_LifeDlg.h"        
#include "NewInterface/V3_SpellDlg.h"       
#include "NewInterface/V3_AHDlg.h"          
#include "NewInterface/V3_BoutiqueDlg.h"    
#include "NewInterface/V3_InteractionRP.h"    
#include "NewInterface/V3_InteractionPvP.h"   
#include "NewInterface/V3_QuestBookP1.h"
#include "NewInterface/V3_QuestBookP2.h"
#include "NewInterface/V3_QuestBookP3.h"
#include "NewInterface/V3_BuyDlg.h"         
#include "NewInterface/V3_ChatDlg.h"        
#include "NewInterface/V3_ChatLogDlg.h"     
#include "NewInterface/V3_ChestDlg.h"       
#include "NewInterface/V3_ChestNewDlg.h"       
#include "NewInterface/V3_GMDlg.h"          
#include "NewInterface/V3_GroupOSDlg.h"     
#include "NewInterface/V3_GroupeDlg.h"      
#include "NewInterface/V3_GuildChestDlg.h"  
#include "NewInterface/V3_GuildChestNewDlg.h"  
#include "NewInterface/V3_GuildDlg.h"       
#include "NewInterface/V3_InvDlg.h"         
#include "NewInterface/V3_ItemInfoDlg.h"    
#include "NewInterface/V3_MacroDlg.h"       
#include "NewInterface/V3_MainBarDlg.h"     
#include "NewInterface/V3_OptionsDlg.h"     
#include "NewInterface/V3_PanierDlg.h"      
#include "NewInterface/V3_ProfessionDlg.h"  
#include "NewInterface/V3_StatDlg.h"       
#include "NewInterface/V3_TMIDlg.h"         
#include "NewInterface/V3_MP3Dlg.h"     
#include "NewInterface/V3_ArenePointsDlg.h"     
#include "NewInterface/V3_TargetDlg.h"      
#include "NewInterface/V3_TradeDlg.h"       
#include "NewInterface/V3_TrainDlg.h"       
#include "NewInterface/V3_TrainProfDlg.h"   
#include "NewInterface/V3_VolDlg.h"         
#include "NewInterface/V3_EffectStatusDlg.h"
#include "NewInterface/V3_Arene1WaitDlg.h"
#include "NewInterface/V3_Arene1PlayDlg.h"










#define DM_NONE  0
#define DM_DOWN  1
#define DM_CLICK  2
#define DM_DOUBLE_CLICK  3
#define DM_UP  4
#define DM_DRAG  5
#define DM_DROP  6

extern DWORD Try;

extern bool NeedRedraw;

time_t Old = 0;
time_t Old2 = 0;

extern bool bCanMovePL;
extern bool Move2;
const int MAXMOV = 20;
extern char Movement[MAXMOV];
extern bool Attack;
extern bool Get;
extern bool Use;
extern DWORD TargetID;

extern Bitmap Grid;
extern Bitmap GridID;
extern bool ForceSpell;
extern bool ForceSkill;
extern int XAttack;
extern int YAttack;
extern int XGet;
extern int YGet;
extern int XUse;
extern int YUse;
extern int TalkToX;
extern int TalkToY;
extern int TalkToW;
extern unsigned long TalkToID;
static unsigned int TalkTime = 0;
extern short TalkToOffset;
extern unsigned char Direction;
extern bool Follow;
extern int FollowID;
extern bool SHIFT_State;

extern volatile unsigned long Round;
extern BACK Backpack[24];
extern TFCSocket Remote;

extern CV2Sprite V3MouseCursor1;
extern CV2Sprite V3MouseCursor1A;
extern CV2Sprite V3MouseCursor19;
extern CV2Sprite V3MouseCursor19A;
extern CV2Sprite V3MouseCursor20;
extern CV2Sprite V3MouseCursor20A;
extern CV2Sprite V3MouseCursor21;
extern CV2Sprite V3MouseCursor21A;
extern CV2Sprite V3MouseCursor22;
extern CV2Sprite V3MouseCursor22A;


int x2, y2;

void Talking(int b[4], LPPOINT);
void UseOb(int b[4], LPPOINT);
void UseObOnSite(int b[4], LPPOINT);
void GetOb(int b[4], LPPOINT);
void Combat(int b[4], LPPOINT);
void See(void);
void MovePl(int b[4], LPPOINT);
void Nothing(void);
	
DWORD FreezeID   = 0;
DWORD SelectedID = 0;

extern char Force;
extern int nbMovement;
extern time_t Old;

BOOL inAttack = FALSE;

BOOL bResetMouseQuery = TRUE;

inline int Abs(int x) {
	return ((x < 0) ? (-x) : (x));
}


void MouseActionReset(void)
{
   bResetMouseQuery = TRUE;
}

void MouseAction(void) 
{
   MouseDevice Mouse2;
   int xp, yp;
   int button[4];
   int TrueObject = -1;
   if (!Player.Dead) 
   {
      static BOOL boGetDrag = FALSE;
      
      Mouse2.GetStatus(&xp, &yp, button);
      g_Global.SetlastScrollStatus(0);
      
      POINT pt;
      pt.x = xp;
      pt.y = yp;
      
      if(bResetMouseQuery)
      {
         UIMouseEvent::GetInstance()->ResetQueuedEvent();
         bResetMouseQuery = FALSE;
      }
      
      if( UIMouseEvent::GetInstance()->ResolveQueuedEvent() )
         return;
      

      if( RootBoxUI::GetInstance()->IsMouseOwned( MousePos( pt ) ) )
      {
         if(g_SaveGame.GetOptionsParam()->bGUIDynamic)
         {

            //On Valide si on est sur un DRAG possible dans un GUI pour changer le curseur...
            if((V3_LifeDlg         ::GetInstance()->IsShown() && !V3_LifeDlg           ::GetInstance()->IsForegroundControlled() && V3_LifeDlg          ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_SpellDlg        ::GetInstance()->IsShown() && !V3_SpellDlg          ::GetInstance()->IsForegroundControlled() && V3_SpellDlg         ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_AHDlg           ::GetInstance()->IsShown() && !V3_AHDlg             ::GetInstance()->IsForegroundControlled() && V3_AHDlg            ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_BoutiqueDlg     ::GetInstance()->IsShown() && !V3_BoutiqueDlg       ::GetInstance()->IsForegroundControlled() && V3_BoutiqueDlg      ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_InteractionRPDlg::GetInstance()->IsShown() && !V3_InteractionRPDlg  ::GetInstance()->IsForegroundControlled() && V3_InteractionRPDlg ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_QuestBookP1     ::GetInstance()->IsShown() && !V3_QuestBookP1       ::GetInstance()->IsForegroundControlled() && V3_QuestBookP1      ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_QuestBookP2     ::GetInstance()->IsShown() && !V3_QuestBookP2       ::GetInstance()->IsForegroundControlled() && V3_QuestBookP2      ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_QuestBookP3     ::GetInstance()->IsShown() && !V3_QuestBookP3       ::GetInstance()->IsForegroundControlled() && V3_QuestBookP3      ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_InteractionPVPDlg::GetInstance()->IsShown()&& !V3_InteractionPVPDlg ::GetInstance()->IsForegroundControlled() && V3_InteractionPVPDlg::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_Arene1WaitDlg   ::GetInstance()->IsShown() && !V3_Arene1WaitDlg     ::GetInstance()->IsForegroundControlled() && V3_Arene1WaitDlg    ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_Arene1PlayDlg   ::GetInstance()->IsShown() && !V3_Arene1PlayDlg     ::GetInstance()->IsForegroundControlled() && V3_Arene1PlayDlg    ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_BuyDlg          ::GetInstance()->IsShown() && !V3_BuyDlg            ::GetInstance()->IsForegroundControlled() && V3_BuyDlg           ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_ChatDlg         ::GetInstance()->IsShown() && !V3_ChatDlg           ::GetInstance()->IsForegroundControlled() && V3_ChatDlg          ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_ChatLogDlg      ::GetInstance()->IsShown() && !V3_ChatLogDlg        ::GetInstance()->IsForegroundControlled() && V3_ChatLogDlg       ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok si pinner pas de drag car box fait 0,0,0,0
               (V3_ChestDlg        ::GetInstance()->IsShown() && !V3_ChestDlg          ::GetInstance()->IsForegroundControlled() && V3_ChestDlg         ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_ChestNewDlg     ::GetInstance()->IsShown() && !V3_ChestNewDlg       ::GetInstance()->IsForegroundControlled() && V3_ChestNewDlg      ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_GMDlg           ::GetInstance()->IsShown() && !V3_GMDlg             ::GetInstance()->IsForegroundControlled() && V3_GMDlg            ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_GroupOSDlg      ::GetInstance()->IsShown() && !V3_GroupOSDlg        ::GetInstance()->IsForegroundControlled() && V3_GroupOSDlg       ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok tous le GUI
               (V3_GroupeDlg       ::GetInstance()->IsShown() && !V3_GroupeDlg         ::GetInstance()->IsForegroundControlled() && V3_GroupeDlg        ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_GuildChestDlg   ::GetInstance()->IsShown() && !V3_GuildChestDlg     ::GetInstance()->IsForegroundControlled() && V3_GuildChestDlg    ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_GuildChestNewDlg::GetInstance()->IsShown() && !V3_GuildChestNewDlg  ::GetInstance()->IsForegroundControlled() && V3_GuildChestNewDlg ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_GuildDlg        ::GetInstance()->IsShown() && !V3_GuildDlg          ::GetInstance()->IsForegroundControlled() && V3_GuildDlg         ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_InvDlg          ::GetInstance()->IsShown() && !V3_InvDlg            ::GetInstance()->IsForegroundControlled() && V3_InvDlg           ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_ItemInfoDlg     ::GetInstance()->IsShown() && !V3_ItemInfoDlg       ::GetInstance()->IsForegroundControlled() && V3_ItemInfoDlg      ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_MacroDlg        ::GetInstance()->IsShown() && !V3_MacroDlg          ::GetInstance()->IsForegroundControlled() && V3_MacroDlg         ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_OptionsDlg      ::GetInstance()->IsShown() && !V3_OptionsDlg        ::GetInstance()->IsForegroundControlled() && V3_OptionsDlg       ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_PanierDlg       ::GetInstance()->IsShown() && !V3_PanierDlg         ::GetInstance()->IsForegroundControlled() && V3_PanierDlg        ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_ProfessionDlg   ::GetInstance()->IsShown() && !V3_ProfessionDlg     ::GetInstance()->IsForegroundControlled() && V3_ProfessionDlg    ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_StatsDlg        ::GetInstance()->IsShown() && !V3_StatsDlg          ::GetInstance()->IsForegroundControlled() && V3_StatsDlg         ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_TMIDlg          ::GetInstance()->IsShown() && !V3_TMIDlg            ::GetInstance()->IsForegroundControlled() && V3_TMIDlg           ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok tous le GUI
               (V3_TargetDlg       ::GetInstance()->IsShown() && !V3_TargetDlg         ::GetInstance()->IsForegroundControlled() && V3_TargetDlg        ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_TradeDlg        ::GetInstance()->IsShown() && !V3_TradeDlg          ::GetInstance()->IsForegroundControlled() && V3_TradeDlg         ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_TrainDlg        ::GetInstance()->IsShown() && !V3_TrainDlg          ::GetInstance()->IsForegroundControlled() && V3_TrainDlg         ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_TrainProfDlg    ::GetInstance()->IsShown() && !V3_TrainProfDlg      ::GetInstance()->IsForegroundControlled() && V3_TrainProfDlg     ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_VolDlg          ::GetInstance()->IsShown() && !V3_VolDlg            ::GetInstance()->IsForegroundControlled() && V3_VolDlg           ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_ArenePointsDlg  ::GetInstance()->IsShown() && !V3_ArenePointsDlg    ::GetInstance()->IsForegroundControlled() && V3_ArenePointsDlg   ::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))    || //ok
               (V3_MainBarDlg      ::GetInstance()->IsShown() && !V3_MainBarDlg        ::GetInstance()->IsForegroundControlled() && V3_MainBarDlg       ::GetInstance()->Drag_ButtonR.GetPos().MousePosInFrame(MousePos( pt )))   || //ok
               (V3_MainBarDlg      ::GetInstance()->IsShown() && !V3_MainBarDlg        ::GetInstance()->IsForegroundControlled() && V3_MainBarDlg       ::GetInstance()->Drag_ButtonL.GetPos().MousePosInFrame(MousePos( pt )))   || //ok
               (V3_Mp3Dlg          ::GetInstance()->IsShown() && !V3_Mp3Dlg            ::GetInstance()->IsForegroundControlled() && V3_Mp3Dlg           ::GetInstance()->Drag_ButtonL.GetPos().MousePosInFrame(MousePos( pt )))   || //ok
               (V3_Mp3Dlg          ::GetInstance()->IsShown() && !V3_Mp3Dlg            ::GetInstance()->IsForegroundControlled() && V3_Mp3Dlg           ::GetInstance()->Drag_ButtonR.GetPos().MousePosInFrame(MousePos( pt )))   || //ok
               (V3_EffectStatusDlg ::GetInstance()->IsShown() && V3_EffectStatusDlg    ::GetInstance()->Drag_Button.IsShown()    && !V3_EffectStatusDlg ::GetInstance()->IsForegroundControlled() && V3_EffectStatusDlg::GetInstance()->Drag_Button.GetPos().MousePosInFrame(MousePos( pt )))      //ok
              )
            {
               CMouseCursor::GetInstance()->SetCustomCursor( &V3MouseCursor19, 0, 0, true ,true,&V3MouseCursor19A);
            }
            
            else if((V3_ChatLogDlg     ::GetInstance()->IsShown()&& !V3_ChatLogDlg     ::GetInstance()->IsForegroundControlled()  && V3_ChatLogDlg     ::GetInstance()->UPSize_Button.GetPos().MousePosInFrame(MousePos( pt )))||
                    (V3_EffectStatusDlg::GetInstance()->IsShown()&& !V3_EffectStatusDlg::GetInstance()->IsForegroundControlled()  && V3_EffectStatusDlg::GetInstance()->UPSize_Button.GetPos().MousePosInFrame(MousePos( pt ))))
            {
               CMouseCursor::GetInstance()->SetCustomCursor( &V3MouseCursor20, 0, 0, true ,true,&V3MouseCursor20A);
            }
            else if((V3_ChatLogDlg     ::GetInstance()->IsShown()&& !V3_ChatLogDlg     ::GetInstance()->IsForegroundControlled()  && V3_ChatLogDlg     ::GetInstance()->RightSize_Button.GetPos().MousePosInFrame(MousePos( pt ))))
            {
               CMouseCursor::GetInstance()->SetCustomCursor( &V3MouseCursor21, 0, 0, true ,true,&V3MouseCursor21A);
            }
            
            else if(CMouseCursor::GetInstance()->GetGUIForced())
            {
               CMouseCursor::GetInstance()->SetCustomCursor(NULL);
            }
         }
         return;
      }
     
      if(!g_SaveGame.GetOptionsParam()->bAncienDisplayOH)
      {
         //check des keyworld
         BOOL bOnHyperLink = FALSE;
         TFCObject *Object = NULL;
         char  strLink[1024];
         DWORD dwNPCID;
         Objects.Lock(331);
         Objects.ToHead();
         while(Objects.QueryNext()) 
         {
            Object = Objects.GetObject();
            if( Object->IsTalkTextDrawn() && Object->Friendly == 2)
            {

               if(Object->ProcessHyperLink(TalkToID,xp,yp,strLink,dwNPCID))
                  bOnHyperLink = TRUE;
            }
         }
         Objects.Unlock(331);
         if(bOnHyperLink)
         {
            if(button[0] == DM_CLICK)
            {
               char  strLinkSend[1024];
               if(strstr(strLink,"Bye")!= NULL || strstr(strLink,"Au revoir")!= NULL)
               {
                  sprintf_s(strLinkSend,1024,"Bye Adieu");
               }
               else
               {
                  sprintf_s(strLinkSend,1024,"%s",strLink);
               }
               
               //on doit lancer le mot...
               signed char xDif = 0;
               signed char yDif = 0;
               Objects.Lock(35);
               Objects.Found(&xDif, &yDif, dwNPCID);
               Objects.NoIdentify(dwNPCID);
               Objects.Unlock(35);
               Direction = 1;
               int a = (xDif+11)*3;
               int b = (yDif+16)*2;
               if (b > 30) {
                  if (a > 30) {
                     a -= 30;
                     b -= 30;
                     if (a > b*2) {
                        Direction = 6;
                     } else if (b > a*2) {
                        Direction = 2;
                     } else {
                        Direction = 3;
                     }
                  } else {
                     b -= 30;
                     a = 30 - a;
                     if (a > b*2) {
                        Direction = 4;
                     } else if (b > a*2) {
                        Direction = 2;
                     } else {
                        Direction = 1;
                     } 
                  }
               } else {
                  if (a > 30) {
                     a -= 30;
                     b = 30 - b;
                     if (a > b*2) {
                        Direction = 6;
                     } else if (b > a*2) {
                        Direction = 8;
                     } else {
                        Direction = 9;
                     }
                  } else {
                     a = 30 - a;
                     b = 30 - b;
                     if (a > b*2) {
                        Direction = 4;
                     } else if (b > a*2) {
                        Direction = 8;
                     } else {
                        Direction = 7;
                     }
                  }			
               }

               TFCPacket Send; 

               TalkToX = (Player.xPos+xDif);
               TalkToY = (Player.yPos+yDif);
               TalkToW = Player.World;

               Send << (short)RQ_DirectedTalkNoFeed;
               Send << (short)(Player.xPos+xDif);
               Send << (short)(Player.yPos+yDif);
               TalkToOffset = 0;
               Send << (long)dwNPCID;
               Send << (char)Direction;
               Send << (long)Custom.gTextColor;
               Send << (long)Player.lKey;
               Send << (short)strlen(strLinkSend);
               Send << (char *)strLinkSend;
               SEND_PACKET(Send);
            }
            CMouseCursor::GetInstance()->SetCustomCursor( &V3MouseCursor22, 0, 0, true ,true,&V3MouseCursor22A);
            return;
         }
      }

      if(CMouseCursor::GetInstance()->GetGUIForced())
         CMouseCursor::GetInstance()->SetCustomCursor(NULL);
    
      
      BOOL GetMsgBox = FALSE;

      if (button[0] == DM_DRAG) 
      {
         if(!boGetDrag) 
         {
            Mouse2.GetDrag(&xp, &yp, 0);
            boGetDrag = TRUE;
            pt.x = xp;
            pt.y = yp;
         }
      } 
      else 
      {
         boGetDrag = FALSE;
      }

      
      if (GetMsgBox) 
         return;
      
      if (button[0] == DM_NONE) 
      {
         Old = 0;
         if (Force) 
         {
            Force = 0;
            pfStopMovement("Move 1");
            Attack = false;
            Use = false;
            Get = false;
         }
         inAttack = FALSE;
      }


      x2 = (xp+48-16)/32;
      y2 = (yp-8)/16;

      
      
      Objects.Lock(33);
      TargetID = GridID(x2, y2);

      //char strTmp[100];
      //sprintf_s(strTmp,100,"ChangeTarget %d\n",TargetID);
      //OutputDebugString(strTmp);


      POINT wp;
      Player.ScreenPosToWL(&pt, &wp);

      g_Global.SetMouseLastWorld(wp.x,wp.y);

      Objects.Unlock(33);
      
      
      BOOL boIsAnyInterfaceUIBlockingMove = FALSE;
		{
         if ( (V3_PacmanDlg::GetInstance()->IsShown()) ||
              (V3_TradeDlg::GetInstance()->IsShown())  /*||*/
            )
         {
            boIsAnyInterfaceUIBlockingMove = TRUE;
         }
		}

   


      if (boIsAnyInterfaceUIBlockingMove == FALSE && ( button[0] == DM_DRAG || button[0] == DM_DROP || button[0] == DM_DOWN || button[0] == DM_CLICK || button[0] == DM_DOUBLE_CLICK || button[1] == DM_CLICK || button[1] == DM_DOUBLE_CLICK || button[1] == DM_DRAG || button[1] == DM_DROP ) )  
      {
         if (Grid(x2, y2) == 3 && !Force && !ForceSpell && !ForceSkill && !SHIFT_State) 
         {
            // TALKING
            Talking(button, &pt);
         }
         else if (Grid(x2, y2) == 6 && !Force && !ForceSpell && !ForceSkill && !SHIFT_State) 
         {
            // USE_ONSITE
            UseObOnSite(button, &pt);            
         }
         else if (Grid(x2, y2) == 4 && !Force && !ForceSpell && !ForceSkill && !SHIFT_State) 
         {
            // USE
            UseOb(button, &pt);
         }
         else if (Grid(x2, y2) == 5 && !Force && !ForceSpell && !ForceSkill && !SHIFT_State) 
         {
            // GET
            GetOb(button, &pt);
         }
         else if (Grid(x2, y2) == 2 && !Force || ForceSpell || ForceSkill || SHIFT_State) 
         {
            // COMBAT
            Combat(button, &pt);
         }
         else if (Grid(x2, y2) == 1 && !Force && !ForceSpell && !ForceSkill && !SHIFT_State) 
         {
            // SEE
            See();																							
         }
         else if ((Grid(x2, y2) == 0 ||	Force) && !ForceSpell && !ForceSkill && !SHIFT_State) 
         {
            // MOVE
            MovePl(button, &pt);
         } 
         else 
         {
            // NOTHING
            Nothing();
         }
      }
   }
}	


void Talking(int button[4], LPPOINT pt) 
{
	// TALKING
	if (button[1] == DM_CLICK || button[1] == DM_DOUBLE_CLICK || button[1] == DM_DROP) 
   {
		signed char xDif = 0;
		signed char yDif = 0;
		unsigned long ID = 0;
		Objects.Lock(34);
		ID = GridID(x2, y2);
      Objects.Identify(ID); 
		Objects.Unlock(34);
	} 
   else if (button[0] == DM_CLICK || button[0] == DM_DOUBLE_CLICK || button[0] == DM_DROP) 
   {
		signed char xDif = 0;
		signed char yDif = 0;
      if (TalkToID != GridID(x2,y2) || timeGetTime() - TalkTime > 5000) 
      {
         TalkToID = GridID(x2, y2);
         SelectedID = TalkToID;

         TalkTime = timeGetTime();
   		Objects.Lock(35);
		   Objects.Found(&xDif, &yDif, TalkToID);
		   Objects.NoIdentify(TalkToID);
		   Objects.Unlock(35);
		   Direction = 1;
		   int a = (xDif+11)*3;
		   int b = (yDif+16)*2;
		   if (b > 30) {
   			if (a > 30) {
	   			a -= 30;
		   		b -= 30;
			   	if (a > b*2) {
				   	Direction = 6;
   				} else if (b > a*2) {
	   				Direction = 2;
		   		} else {
			   		Direction = 3;
				   }
   			} else {
	   			b -= 30;
		   		a = 30 - a;
			   	if (a > b*2) {
				   	Direction = 4;
   				} else if (b > a*2) {
	   				Direction = 2;
		   		} else {
			   		Direction = 1;
				   } 
   			}
	   	} else {
		   	if (a > 30) {
			   	a -= 30;
				   b = 30 - b;
   					if (a > b*2) {
   					Direction = 6;
	   			} else if (b > a*2) {
		   			Direction = 8;
			   	} else {
				   	Direction = 9;
   				}
	   		} else {
		   		a = 30 - a;
			   	b = 30 - b;
				   if (a > b*2) {
					   Direction = 4;
   				} else if (b > a*2) {
	   				Direction = 8;
		   		} else {
			   		Direction = 7;
				   }
   			}			
	   	}
	       
   		TFCPacket Send; 
      
		   TalkToX = (Player.xPos+xDif);
   		TalkToY = (Player.yPos+yDif);
         TalkToW = Player.World;

         Send << (short)RQ_DirectedTalk;
	   	Send << (short)(Player.xPos+xDif);
		   Send << (short)(Player.yPos+yDif);
	   	TalkToOffset = 0;
	   	Send << (long)TalkToID;
		   Send << (char)Direction;
   		Send << (long)Custom.gTextColor;
         Send << (long)Player.lKey;
	   	Send << (short)0;
		   Objects.Lock(37);
         Objects.GetMainObject()->Direction = Direction;
         if (Objects.FoundID(TalkToID)) 
         {
		      SEND_PACKET(Send);
         }
         Objects.Unlock(37);
	   }
   }       
}

void UseOb(int button[4], LPPOINT pt) {
	if (button[1] == DM_CLICK || button[1] == DM_DOUBLE_CLICK || button[1] == DM_DROP) {

		signed char xDif = 0;
		signed char yDif = 0;
		unsigned long ID = 0;
		Objects.Lock(38);
		ID = GridID(x2, y2);

		Objects.Identify(ID); 
		Objects.Unlock(38);
	} else if (button[0] == DM_CLICK || button[0] == DM_DOUBLE_CLICK || button[0] == DM_DROP) {
		Follow = true;
		Use = true;

		signed char xDif = 0;
		signed char yDif = 0;
		Objects.Lock(39);
		FollowID = GridID(x2, y2);
		Objects.Found(&xDif, &yDif, FollowID);
		Objects.NoIdentify(FollowID);
		Objects.Unlock(39);
	
		XUse = xDif + Player.xPos;
		YUse = yDif + Player.yPos;

		pfSetPosition(XUse, YUse);
      pfNearPosition();
      pfNearPosition();

      TalkToX = (Player.xPos+xDif);
   	TalkToY = (Player.yPos+yDif);
      TalkToW = Player.World;
        //TalkToID = FollowID;

		if (bCanMovePL && Move2) 
      {
			bCanMovePL = false;
			TFCPacket Send;
			
			if (pfIsAtPosition()) 
         {
				Follow = false;
				bCanMovePL = true;
				TFCPacket Send; 
				TFCPacket Info;

				Send << (short)23;
				Send << (short)XUse;
				Send << (short)YUse;
				Send << (long)FollowID;
			
				SEND_PACKET(Send);
				Use = false;
			} else {
				short Menrage = (short)pfGetNextMovement();
            if (Menrage) {
               Send << Menrage;
               Try = timeGetTime();

               SEND_PACKET(Send);
            }
			}
		}
	}
}

void UseObOnSite(int button[4], LPPOINT pt) {
	if (button[1] == DM_CLICK || button[1] == DM_DOUBLE_CLICK || button[1] == DM_DROP) {

		signed char xDif = 0;
		signed char yDif = 0;
		unsigned long ID = 0;
		Objects.Lock(38);
		ID = GridID(x2, y2);
      Objects.Identify(ID); 

      Objects.Unlock(38);
	} else if (button[0] == DM_CLICK || button[0] == DM_DOUBLE_CLICK || button[0] == DM_DROP) {
		//Use = true;

        DWORD foundID;

		signed char xDif = 0;
		signed char yDif = 0;
		Objects.Lock(39);
		foundID = GridID(x2, y2);
		Objects.Found(&xDif, &yDif, foundID);
		Objects.NoIdentify(foundID);
		Objects.Unlock(39);
	
		XUse = xDif + Player.xPos;                                                                               
		YUse = yDif + Player.yPos;

        /*TalkToX = (Player.xPos+xDif);
   		TalkToY = (Player.yPos+yDif);
        TalkToID = foundID;*/

		pfSetPosition(XUse, YUse);
        pfNearPosition();
        pfNearPosition();

		TFCPacket Send; 
		TFCPacket Info;

		Send << (short)23;
		Send << (short)XUse;
		Send << (short)YUse;
		Send << (long)foundID;
	
		SEND_PACKET(Send);
	}
}

void GetOb(int button[4], LPPOINT pt) {
// GET
	if (button[1] == DM_CLICK || button[1] == DM_DOUBLE_CLICK || button[1] == DM_DROP) {

		signed char xDif = 0;
		signed char yDif = 0;
		unsigned long ID = 0;
		Objects.Lock(40);
		ID = GridID(x2, y2);
	
		//++NMNMNM --[Add item Specification on right click item ON FLOOR]
      if(g_SaveGame.GetOptionsParam()->bShowItemSpec)
      {
         g_Global.ResetItemInfo();
         g_Global.SetItemInfoID(ID);
         g_Global.SetItemInfoInventory(false);
         TFCPacket sending2;
         sending2 << (RQ_SIZE)RQ_QueryItemInfo;
         sending2 << (long)ID;
         SEND_PACKET( sending2 );
      }
      else
		   Objects.Identify(ID); 
      

		Objects.Unlock(40);
	} else if (button[0] == DM_CLICK || button[0] == DM_DOUBLE_CLICK || button[0] == DM_DROP) { 
      Follow = true;
		Get = true;

		signed char xDif = 0;
		signed char yDif = 0;
		Objects.Lock(41);
		FollowID = GridID(x2, y2);
		Objects.Found(&xDif, &yDif, FollowID);
		Objects.NoIdentify(FollowID);
		Objects.Unlock(41);
	
		XGet = xDif + Player.xPos;
		YGet = yDif + Player.yPos;

		pfSetPosition(XGet, YGet);
      pfNearPosition();

		if (bCanMovePL && Move2) 
      {
			bCanMovePL = false;
			TFCPacket Send;
			
			if (pfIsAtPosition()) 
         {
				Follow = false;
				bCanMovePL = true;
				TFCPacket Send; 
				TFCPacket Info;

				Send << (short)11;
				Send << (short)XGet;
				Send << (short)YGet;
				Send << (long)FollowID;
			
				SEND_PACKET(Send);
				Get = false;
			} else {
				short Menrage = (short)pfGetNextMovement();
            if (Menrage) {
               Send << Menrage;
               Try = timeGetTime();
			
               SEND_PACKET(Send);
            }
			}
		}
	}
}

void Combat(int button[4], LPPOINT pt) 
{
	// COMBAT
   static DWORD dwLastTime = 0;
   if (button[0] == DM_DOUBLE_CLICK && g_SaveGame.GetOptionsParam()->bLockTarget) 
   {
		Objects.Lock(42);
		FreezeID = GridID(x2, y2);
		Objects.Unlock(42);
	}

   if (button[0] == DM_DOUBLE_CLICK )
   {
      Objects.Lock(42);
      SelectedID = GridID(x2, y2);
		Objects.Unlock(42);
   }
   
	
	if (button[1] == DM_CLICK || button[1] == DM_DOUBLE_CLICK || button[1] == DM_DROP) 
   {
		signed char xDif = 0;
		signed char yDif = 0;
		unsigned long ID = 0;
		Objects.Lock(43);
		ID = GridID(x2, y2);
	
		Objects.Identify(ID); 
		Objects.Unlock(43);
   } 
   else if ((button[0] == DM_DRAG || button[0] == DM_CLICK || button[0] == DM_DOUBLE_CLICK) && (!inAttack || FollowID != (int)GridID(x2, y2))) 
   {
      if (SHIFT_State && (timeGetTime() - dwLastTime) > 500) 
      {
         dwLastTime = timeGetTime();
         TFCPacket Send;
         TFCPacket Info;
         
         Send << (short)RQ_Attack;
         Send << (short)Player.xPos;
         Send << (short)Player.yPos;
         Send << (long)NULL;
         
         SEND_PACKET(Send);
      } 
      else if (!SHIFT_State) 
      {
         
         if (button[0] == DM_DRAG)
            inAttack = TRUE;

         signed char xDif = 0;
         signed char yDif = 0;
         
         if( Player.rangedAttack )
         {
            if (ForceSpell) 
            {
               bCanMovePL = true;
               Attack = false;

               V3_SpellDlg::GetInstance()->CastLastSpell();
               return;
            }

            if (ForceSkill) 
            {
               bCanMovePL = true;
               Attack = false;
               
               V3_StatsDlg::GetInstance()->CastLastSkill();
               return;
            }


             DWORD attackId;
             Objects.Lock(44);
             
             if (FreezeID && Objects.FoundID(FreezeID))
             {
                attackId = FreezeID;
             }
             else
             {
                attackId = GridID(x2, y2);
             }
             Objects.Found(&xDif, &yDif, attackId);
             Objects.Unlock(44);
             
             if( attackId != Player.ID )
             {
                short xPos = xDif + Player.xPos;
                short yPos = yDif + Player.yPos;

                TFCPacket Send;
                Send << (short)RQ_Attack;
                Send << (short)xPos;
                Send << (short)yPos;
                Send << (long)attackId;
          
                SEND_PACKET(Send);

                SelectedID = attackId;
             }

             return;
         }

         Follow = true;
         Attack = true;
         
         Objects.Lock(44);
         if (FreezeID && Objects.FoundID(FreezeID)) 
            FollowID = FreezeID;
         else
            FollowID = GridID(x2, y2);
         Objects.Found(&xDif, &yDif, FollowID);
         Objects.Unlock(44);
         
         XAttack = xDif + Player.xPos;
         YAttack = yDif + Player.yPos;
         
         if (!ForceSpell && !ForceSkill) 
         {
            pfSetPosition(XAttack, YAttack);
            pfNearPosition();
         }

         if(FollowID)
         {
            SelectedID = FollowID;
         }
         
         if (bCanMovePL && Move2) 
         {
            bCanMovePL = false;
            
            TFCPacket Send;
            TFCPacket Info;
            
            if (pfIsAtPosition() && !ForceSpell && !ForceSkill) 
            {
               Follow = false;
               bCanMovePL = true;
               if (FollowID) 
               {
                  TFCPacket Send; 
                  TFCPacket Info;
                  
                  Send << (short)RQ_Attack;
                  Send << (short)XAttack;
                  Send << (short)YAttack;
                  Send << (long)FollowID;
                  
                  SEND_PACKET(Send);
               }
               Attack = false;
            } 
            else if (!ForceSpell && !ForceSkill) 
            {
               short Menrage = (short)pfGetNextMovement();
               if (Menrage) 
               {
                  Send << Menrage;
                  
                  SEND_PACKET(Send);
               }
            }
            if (ForceSpell) 
            {
               bCanMovePL = true;
               Attack = false;

               V3_SpellDlg::GetInstance()->CastLastSpell();
            }
            
            if (ForceSkill) 
            {
               bCanMovePL = true;
               Attack = false;
               
               V3_StatsDlg::GetInstance()->CastLastSkill();
            }
         }
      }
   }
}

void See(void) {
// SEE
	int xDif = 0;
	int yDif = 0;
	xDif = x2 - 11;
	yDif = y2 - 16;
	signed char XTemp, YTemp;
	Objects.Lock(47);
	if (!Objects.RealPos(xDif, yDif, &XTemp, &YTemp))  {
		xDif++;
		if (!Objects.RealPos(xDif, yDif, &XTemp, &YTemp)) { 
			xDif-=2;
			Objects.RealPos(xDif, yDif, &XTemp, &YTemp);
		}
	}
	xDif = XTemp; yDif = YTemp;
	Objects.SetExtended(xDif, yDif);
	Objects.Unlock(47);
}

void MovePl(int button[4], LPPOINT pt) 
{
   if (FreezeID) 
   {
      Objects.Lock(49);
      if (!Objects.FoundID(FreezeID)) 
      {
         FreezeID = NULL;
      }
      Objects.Unlock(49);
   }

   if (SelectedID) 
   {
      Objects.Lock(49);
      if (!Objects.FoundID(SelectedID)) 
      {
         SelectedID = NULL;
      }
      Objects.Unlock(49);
   }
   
   if (button[0] == DM_DOUBLE_CLICK) 
   {
      FreezeID   = NULL;
      SelectedID = NULL;
   }
   
   if (button[1] == DM_CLICK || button[1] == DM_DOUBLE_CLICK || button[1] == DM_DROP) 
   {
      signed char xDif = 0;
      signed char yDif = 0;
      unsigned long ID = 0;
      Objects.Lock(50);
      ID = GridID(x2, y2);
      
      if (ID)
         Objects.Identify(ID); 
      
      Objects.Unlock(50);
   }
   if (button[1] == DM_NONE && FreezeID && GridID(x2, y2) && !Force) 
   {
      Combat(button, pt);
   } 
   else if (button[1] == DM_NONE && !inAttack && (button[0] == DM_CLICK || button[0] == DM_DOUBLE_CLICK || button[0] == DM_DRAG)) 
   {
      Follow = false;
      int xDif = 0;
      int yDif = 0;
      xDif = x2 - 11;
      yDif = y2 - 16;
      
      POINT wp;
      
      Player.ScreenPosToWL(pt, &wp);
      
      pfSetPosition(wp.x, wp.y);
      
      if (button[0] == DM_DRAG)
         Force = 1;
      
      if (bCanMovePL && !NeedRedraw && Move2) 
      {
         bCanMovePL = false;
         TFCPacket Send;
         TFCPacket Info;
         
         short Menrage = (short)pfGetNextMovement();
         if (Menrage) 
         {
            Send << Menrage;
            
            SEND_PACKET(Send);
            Try = timeGetTime();
         }
      }
   }
}

void Nothing(void) {
   FollowID = NULL;
}
