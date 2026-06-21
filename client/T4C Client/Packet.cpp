#pragma warning( disable : 4786 )
#pragma warning( disable : 4291 )

#include "pch.h"
#include <math.h>
#include "TFCPlayer.h"
#include "NTime.h"
#include "VisualObjectList.h"
#include "TFCSocket.h"
#include "TileSet.h"
#include "NTime.h"
#include "ObjectListing.h"
#include "Packet.h"
#include "pf.h"
#include <process.h>
#include "MacroHandler.h"
#include <stdio.h>
#include "old.h"
#include <eh.h>
#include "App.h" 
#include "PacketHandling.h"
#include "Global.h"
#include "Puppet.h"
#include "GUILocalString.h"
#include "SystemAction.h"
 
#include "NewInterface/RootBoxUI.h"
#include "NewInterface/V3_DisplaySpecialDlg.h"
#include "NewInterface/V3_DisplayHelpDlg.h"
#include "NewInterface/V3_Mp3Dlg.h"
#include "NewInterface/V3_ArenePointsDlg.h"
#include "NewInterface/V3_LifeDlg.h"
#include "NewInterface/V3_StatDlg.h"
#include "NewInterface/V3_OptionsDlg.h"
#include "NewInterface/V3_MacroDlg.h"
#include "NewInterface/V3_ChatLogDlg.h"
#include "NewInterface/V3_MainBarDlg.h"
#include "NewInterface/V3_ChatDlg.h"
#include "NewInterface/V3_MacroDlg.h"
#include "NewInterface/V3_SpellDlg.h"
#include "NewInterface/V3_GroupeDlg.h"
#include "NewInterface/V3_EffectStatusDlg.h"
#include "NewInterface/V3_GroupeDlg.h"
#include "NewInterface/V3_GuildDlg.h"
#include "NewInterface/V3_ProfessionDlg.h"
#include "NewInterface/V3_BuyDlg.h"
#include "NewInterface/V3_TrainDlg.h"
#include "NewInterface/V3_TrainProfDlg.h"
#include "NewInterface/V3_InvDlg.h"
#include "NewInterface/V3_BoutiqueDlg.h"
#include "NewInterface/V3_PanierDlg.h"
#include "NewInterface/V3_ChestDlg.h"
#include "NewInterface/V3_ChestNewDlg.h"
#include "NewInterface/V3_GuildChestDlg.h"
#include "NewInterface/V3_GuildChestNewDlg.h"
#include "NewInterface/V3_GMDlg.h"
#include "NewInterface/V3_VolDlg.h"
#include "NewInterface/V3_RTMapDlg.h"
#include "NewInterface/V3_AHDlg.h"
#include "NewInterface/V3_AHVendreDlg.h"
#include "NewInterface/V3_ItemInfoDlg.h"
#include "NewInterface/V3_TradeDlg.h"
#include "NewInterface/V3_InteractionRP.h"
#include "NewInterface/V3_InteractionPVP.h"
#include "NewInterface/V3_QuestBookP1.h"
#include "NewInterface/V3_QuestBookP2.h"
#include "NewInterface/V3_QuestBookP3.h"
#include "NewInterface/V3_Arene1WaitDlg.h"
#include "NewInterface/V3_Arene1PlayDlg.h"
#include "NewInterface/V3_PacmanDlg.h"


#include "Weather.h"
#include "SaveGame.h"
extern CSaveGame g_SaveGame;

extern bool seraphDisplay;
extern bool goodSeraph;

extern BOOL g_bSmoothFloorLoading;
static BOOL    g_bEnterGamePart2Complete = FALSE;
static UINT CALLBACK  EntergamePart2(LPVOID pParam);


#include "Global.h"
extern Global g_Global;


void CloseAllUI(void) 
{
 
}

BOOL DoNotMove = FALSE;
BOOL INGAME = FALSE;

DWORD SpellID = -10000;

bool noException = true;

HANDLE hDrawThread = NULL;
UINT iDrawThreadID = NULL;

HANDLE hMouseActionThread = NULL;
UINT iMouseActionThreadID = 0;

extern bool g_UiInit;
extern bool g_pendingInventoryRefresh;
extern int SlowMeDown;
bool Code13 = false;
bool EnterGame = false;

extern bool Attack;
extern bool Get;
extern bool Use;

extern int xResize;
extern int yResize;
extern int ResizeCorrection;

BOOL FirstTime = TRUE;

extern BOOL ReAsk;

extern DWORD Try;

extern int DrawLastMoving;

/// TEMP
bool g_DisplayLetter = false;
/// END TEMP

extern bool Draw;

extern bool bCanMovePL;

extern bool NeedRedraw;

extern bool Retrace;


inline int Abs(int x) {
    return ((x < 0) ? (-x) : (x));
}

extern bool Follow;

extern bool ChangePal;


extern bool stShow;

extern UINT CALLBACK DrawThread(LPVOID pParam);
extern UINT CALLBACK MouseActionThread(LPVOID pParam);

extern bool Err;

extern TFCSocket Remote;

extern char LastState;

static int const TFC_MENU						= 5;

extern char TFC_State;

extern bool Register;

extern volatile unsigned long Round;

extern T3VSBSound *Victim;

extern BACK Backpack[24];

extern int TalkToX, TalkToY, TalkToW;
extern unsigned long TalkToID;
extern DWORD SelectedID;
extern short TalkToOffset;

extern DWORD LastAsk3;
extern DWORD LastAsk6;

extern T3VSBSound SoundFX[250];
extern T3VSBSound ConnectSoundFX;

extern bool LevelUp;

extern bool WantPreGame;

extern DWORD g_dwPutPlayerSentAt;
extern int   g_iPutPlayerRetry;
extern int   g_iPutPlayerBusyRetry;

extern bool  g_waitingEnterWorld46;
extern DWORD g_enterWorld46SentAt;
extern int   g_enterWorld46RetryCount;

extern void EnterGameTrace(const char *msg);

extern bool bDisplayList;
extern char DisplayList[25][100];
extern int  DisplayInt[25];
extern int  nbList;

extern TFCPlayer Player;
extern DWORD LAG2;

extern TFCSocket Remote;


HANDLE InterPacketEvent = NULL;

#define HEADER_SIZE     ( KEY_SIZE + CHECKSUM_SIZE )

//////////////////////////////////////////////////////////////////////////////////////////
inline void GetEquipItem
//////////////////////////////////////////////////////////////////////////////////////////
//  Gets an equipped item from an equip packet.
// 
(
 int equipPos,  // The equipment position.
 TFCPacket *msg // The packet.
 )
 // Return: inline void, 
 //////////////////////////////////////////////////////////////////////////////////////////
{
    DWORD  itemID;
    WORD   appear;
    LPBYTE name;
    WORD   qty;
    WORD   baseID;
    long   charges;
    
    GET_LONG( itemID );
    GET_WORD( appear );
    GET_WORD( baseID );
    GET_WORD( qty );
    GET_LONG( charges );
    GET_STRING( name );   

    // If the itemID is empty.
    if( itemID == 0 ){
        // If there was an equipped item at that position.
        if( Player.lpbEquipped[ equipPos ] != NULL ){
            // Delete the bag item.
            delete Player.lpbEquipped[ equipPos ];
            
            Player.lpbEquipped[ equipPos ] = NULL;
        }
        delete name;
    }else{
        // If the item is currently NULL.
        if( Player.lpbEquipped[ equipPos ] == NULL ){
            Player.lpbEquipped[ equipPos ] = new BAG_ITEM;
        }
        
        BAG_ITEM *lpBagItem = Player.lpbEquipped[ equipPos ];
        
        lpBagItem->nX = -1;	// assign random pos to item
        lpBagItem->nY = -1;
        lpBagItem->dwID = itemID;
        lpBagItem->wBaseID = baseID;
        strcpy_s( reinterpret_cast< char * >( lpBagItem->chIName ),512, reinterpret_cast< char * >( name ) );
        
        lpBagItem->Type = appear;
        lpBagItem->lpSprite = InvItemIcons( appear );
        lpBagItem->dwQty = qty;
        lpBagItem->iCharges = charges;
		lpBagItem->chTarget = 0;
		lpBagItem->chAttack = 0;
		lpBagItem->chPVP    = 0;
        
        delete name;
    }
}

class InterPacket : public TemplateList <unsigned char *> {
public:
    void Set(void) {
        SetEvent(InterPacketEvent);
    }
};

InterPacket ug_InterPacket;

QWORD qwPacketUseCounter[150];
QWORD qwPacketUseTime[150];
QWORD qwPacketUsePeek[150];

//////////////////////////////////////////////////////////////////////////////////////////
void ShootArrow
//////////////////////////////////////////////////////////////////////////////////////////
// Shoots an arrow
// 
(
 DWORD CASTER,  // The arrow's caster
 DWORD TARGET,  // The arrow's target. Can be 0.
 SHORT TPOSX,   // The target X pos.
 SHORT TPOSY,   // The target Y pos.
 bool  collide, // Destroy on site or continue forever.
 char  targetHp // The new target's relative hp, if a target was specified.
 )
 //////////////////////////////////////////////////////////////////////////////////////////
{
    SHORT CPOSX, CPOSY;
    signed char x2, y2;
    //World.Lock(54);//NM:ShootArrow
    Objects.Lock(1000);
    
    if( TARGET ){
        // Get the position of the target unit.
        if (Player.ID == TARGET) {
            TPOSX = Player.xPos;
            TPOSY = Player.yPos;
        }else{
            if( Objects.Found((signed char *)&x2, (signed char *)&y2, TARGET) ) {
                TPOSX = x2 + Player.xPos;
                TPOSY = y2 + Player.yPos;
                
                Objects.SetHP( TARGET, targetHp );
            }
        }
    }
    if (!CASTER) {
        //World.Unlock(54);//NM:ShootArrow
        Objects.Unlock(1000);
        
        return;
    }
    // If the player is the caster, determine the arrow's path according to its pos.
    if ( Player.ID == CASTER ){
        CPOSX = Player.xPos;
        CPOSY = Player.yPos;
         Objects.GetMainObject()->Direction = 1;
        int a = ((TPOSX-Player.xPos)+11)*3;
        int b = ((TPOSY-Player.yPos)+16)*2;
        // Determine the position according to its quadrant.
        if (b > 30) {
            if (a > 30) {
                a -= 30;
                b -= 30;
                if (a > b*2) {
                    Objects.GetMainObject()->Direction = 6;
                } else if (b > a*2) {
                    Objects.GetMainObject()->Direction = 2;
                } else {
                    Objects.GetMainObject()->Direction = 3;	
                }
            } else {
                b -= 30;
                a = 30 - a;
                if (a > b*2) {
                    Objects.GetMainObject()->Direction = 4;
                } else if (b > a*2) {
                    Objects.GetMainObject()->Direction = 2;
                } else {
                    Objects.GetMainObject()->Direction = 1;
                }
            }
        } else {
            if (a > 30) {
                a -= 30;
                b = 30 - b;
                if (a > b*2) {
                    Objects.GetMainObject()->Direction = 6;
                } else if (b > a*2) {
                    Objects.GetMainObject()->Direction = 8;
                } else {
                    Objects.GetMainObject()->Direction = 9;
                }
            } else {
                a = 30 - a;
                b = 30 - b;
                if (a > b*2) {
                    Objects.GetMainObject()->Direction = 4;
                } else if (b > a*2) {
                    Objects.GetMainObject()->Direction = 8;
                } else {
                    Objects.GetMainObject()->Direction = 7;
                }
            }			
        }
    }else{
        // Set the object's direction according to the target's pos.
        if (Objects.Found((signed char *)&x2, (signed char *)&y2, CASTER)) {
            CPOSX = x2 + Player.xPos;
            CPOSY = y2 + Player.yPos;
            if( TARGET != NULL ){
                Objects.SetDirection(CASTER, TARGET, true);
            }
        }
    }
    
    
    // If the arrow didn't collide with anything
    if( !collide ){
        if( TPOSY == CPOSY ){
            if( TPOSX < CPOSX ){
                TPOSX = Player.xPos - 14;
            }else{
                TPOSX = Player.xPos + 14;
            }
        }else if( TPOSX == CPOSX ){
            if( TPOSY < CPOSY ){
                TPOSY = Player.yPos - 18;
            }else{
                TPOSY = Player.yPos + 18;
            }
        }else{
            // Extrapolate it out of the screen.
            double slope = static_cast< double >( CPOSY - TPOSY ) / 
                static_cast< double >( CPOSX - TPOSX );
            
            if( slope > 1 || slope < -1 ){
                // Grow the Y part and extrapolate the X part.
                slope = 1 / slope;
                
                if( TPOSY < CPOSY ){
                    TPOSY = TPOSY - 20;
                }else{
                    TPOSY = TPOSY + 20;
                }
                
                TPOSX = (short)( (double)( TPOSY - CPOSY ) * slope ) + CPOSX;
            }else{
                // Grow the X part and extrapolate the Y part.
                if( TPOSX < CPOSX ){
                    TPOSX = TPOSX - 20;
                }else{
                    TPOSX = TPOSX + 20;
                }
                
                TPOSY = (short)( (double)( TPOSX - CPOSX ) * slope ) + CPOSY;
            }
            
        }
    }
    
    int xDiff = TPOSX - CPOSX;
    int yDiff = TPOSY - CPOSY;
    
    DWORD direction = 1;
    
    if (xDiff < 0 && yDiff == 0){
        direction = 4;
    }else if (xDiff > 0 && yDiff == 0){
        direction = 6;
    }else if (xDiff == 0 && yDiff > 0){
        direction = 2;
    }else if (xDiff == 0 && yDiff < 0){
        direction = 8;
    }else if (xDiff < 0 && yDiff > 0){
        direction = 1;
    }else if (xDiff < 0 && yDiff < 0){
        direction = 7;
    }else if (xDiff > 0 && yDiff > 0){
        direction = 3;
    }else if (xDiff > 0 && yDiff < 0){
        direction = 9;
    }
    
    Follow = NULL;
    Objects.Add(SpellID, __SPELL_ARROW, CPOSX, CPOSY, 0, 0, Follow, 100, -1,-1,(char)direction, true ,0,0);
    if (TPOSX && TPOSY) 
    {
        // Arrow ID.
        DWORD SummonID = 0;
        if( collide )
        {
            //SummonID = __SPELL_BROKEN_ARROW;
        }
        Objects.MoveObject(SpellID, TPOSX, TPOSY, TRUE, SummonID, 100,-1,-1, __SPELL_ARROW, 0, 100,false,false,0);
    }
    
    
    if ( CASTER == Player.ID ) {        
        Objects.PlAttack( TARGET, targetHp );
        
        Objects.GetMainObject()->Chose = 1;
        Objects.GetMainObject()->Speed = 0;
        Objects.GetMainObject()->AttSpeed = 0;        
        if( Objects.GetMainObject()->AttFrame > 3 ){
            Objects.GetMainObject()->AttFrame = 3;
        }
        Objects.GetMainObject()->Direction = (unsigned char)direction;
        
        int xDiff = TPOSX - Player.xPos;
        int yDiff = TPOSY - Player.yPos;
        
        if (xDiff < 0 && yDiff == 0){
            Objects.GetMainObject()->Direction = 4;
        }else if (xDiff > 0 && yDiff == 0){
            Objects.GetMainObject()->Direction = 6;
        }else if (xDiff == 0 && yDiff > 0){
            Objects.GetMainObject()->Direction = 2;
        }else if (xDiff == 0 && yDiff < 0){
            Objects.GetMainObject()->Direction = 8;
        }else if (xDiff < 0 && yDiff > 0){
            Objects.GetMainObject()->Direction = 1;
        }else if (xDiff < 0 && yDiff < 0){
            Objects.GetMainObject()->Direction = 7;
        }else if (xDiff > 0 && yDiff > 0){
            Objects.GetMainObject()->Direction = 3;
        }else if (xDiff > 0 && yDiff < 0)
        {
            Objects.GetMainObject()->Direction = 9;
        }
        else
        { 
           Objects.GetMainObject()->Direction = 1;
        }
    }else{
        Objects.SetAttack( CASTER, TARGET, 1, 1, targetHp, direction );
    }
    
    SpellID--;
    Objects.Unlock(1000);
    //World.Unlock(54);//NM:ShootArrow
}


//#pragma optimize( "", off ) 
// Trace diagnostique de l'entree en jeu (debug interblocage 46 / loading vs serveur Linux).
// Chemin ABSOLU pour etre trouvable quel que soit le repertoire de travail du client (process
// eleve = CWD parfois System32). A retirer une fois le flux valide.
void EnterGameTrace(const char *msg)
{
   FILE *f = NULL;
   fopen_s(&f, "C:\\t4c_trace.log", "a");
   if (!f)
      fopen_s(&f, "entergame_trace.log", "a"); // repli si C:\ non inscriptible
   if (f)
   {
      fprintf(f, "%lu %s\n", (unsigned long)GetTickCount(), msg);
      fclose(f);
   }
}

void HandlePacket(TFCPacket *Msg)
{
   DWORD intrQueueLen = 0;
   short Type;
   

   try 
   {
      
      COMM.KeepAlive();
      Msg->Get((short *)&Type);

   
      /*
      if(Type!= 10)
      {
         char strtoto[100]; 
         sprintf_s(strtoto,100,"++++++++IN TYPE  %d\n",Type);
         OutputDebugString(strtoto);
      }
      */
      
   }  
   catch (TFCPacketException *e) 
   {
      delete e;
      try 
      {
         RQ_SIZE ID = Msg->GetPacketID();
      } 
      catch (TFCPacketException *e) 
      {
         delete e;
      }
      throw;
   }

   // DEBUG: trace de tout paquet recu via HandlePacket (sauf keep-alive 10, trop bruyant).
   if (Type != 10)
   {
      char tb[96];
      sprintf_s(tb, 96, "HP recu type=%d", (int)Type);
      EnterGameTrace(tb);
   }

   //#if 1
   {
      if( COMM.GetCommCenter() != NULL )
      {
         intrQueueLen = COMM.GetCommCenter()->GetIntrQueueSize(); 

      }
      
      if( intrQueueLen > 30/*50*/ )
      {
         // If the packet is of any of these type, JUNK IT
         switch( Type )
         {
               //#if 0
               /*
               case RQ_MoveNorth:
               case RQ_MoveNorthEast:
               case RQ_MoveEast:
               case RQ_MoveSouthEast:
               case RQ_MoveSouth:
               case RQ_MoveSouthWest:
               case RQ_MoveWest:
               case RQ_MoveNorthWest:
               {
                 
                  LPBYTE lpbBuffer2;
                  int nBufferSize2;
                  unsigned long ID;

                  int iOffset = 8;
               
                  Msg->GetBuffer(lpbBuffer2, nBufferSize2);

                  ID =  lpbBuffer2[ HEADER_SIZE + iOffset+0 ] << 24;
                  ID += lpbBuffer2[ HEADER_SIZE + iOffset+1 ] << 16;
                  ID += lpbBuffer2[ HEADER_SIZE + iOffset+2 ] << 8;
                  ID += lpbBuffer2[ HEADER_SIZE + iOffset+3 ];	
                  
                  if( intrQueueLen > 25 && ID != Player.ID)
                  {
                     return;
                  }
               }
               break;
               */
            //#endif

            case RQ_UnitUpdate:
            case RQ_UpdateSmile:
            case RQ_Attack:
            case RQ_CastSpell:
            case RQ_HPchanged:
            case RQ_GetStatus:
            case RQ_GetStatus2:
            case RQ_XPchanged:
            case RQ_GoldChange:
            case RQ_NameChange:
            case RQ_SpellEffect:
            case RQ_ManaChanged:
            //case RQ_UpdateBackpackAfterUse:
            case RQ_UpdateGroupMemberHp:
            case RQ_UpdateWeight:
            case RQ_ArrowMiss:
            case RQ_ArrowHit:
            case RQ_HealingUnit:
            case RQ_DamageUnit:
               return;
         }
      };
     
   }
    //#endif
   
   
   if (Type == 10) 
   {
      TFCPacket Send; 
      
      Send << (short)10;
      
      SEND_PACKET(Send);
   } 
   else 
   {
      
      int CounterType = Type;
      
      if (Type >= 10000) {
         // Some packer are too high too fit in the 150 array. They become 100+ Type instead of
         // 10000+ Type.
         CounterType -= 10000;
         CounterType += 100;
      }
      
      const int RQ_UpdateGroupMembers   = 76;
      const int RQ_NotifyGroupDisband   = 82;
      //   const RQ_UpdateGroupMemberHp  = 87;
      
      
      
      switch(Type) {
         // Type of Packet
         // Get Equipped list	
         //////////////////////////////////////////////////////////////////////////////////////////
         // Returns the stats of a player.
         
         //////////////////////////////////////////////////////////////////////////////////////////
         // Update the autosplit status of the group.
      case RQ_SeraphArrival: 
         {
            // 10004 part
            signed char LIGHT;
            unsigned short X, Y, TYPE,MOBID;
            unsigned long ID,FRIENDLYID;
            unsigned char STATUS;
            char PHP;
            char CombatMode;
            char chHiddenInv2;
            unsigned long junk;
            Msg->Get((short *)&junk); // 10004
            Msg->Get((short *)&X);
            Msg->Get((short *)&Y);
            Msg->Get((long  *)&FRIENDLYID);  //OPn sen fou un seraph nes pas un MOB donc rien a faire avec
            Msg->Get((short *)&MOBID);
            Msg->Get((short *)&TYPE);
            Msg->Get((long  *)&ID);
            Msg->Get((char  *)&LIGHT);
            Msg->Get((char  *)&STATUS);
            Msg->Get((char  *)&PHP);
            Msg->Get((char  *)&CombatMode);
            Msg->Get((char  *)&chHiddenInv2); //NMNMNM_Inv2


            //un seraph est toujours un joueur donc aucun effet sur les friendly
            /*
            if(Player.FactionID >0 && MOBID > 0)
            {
               //check si ami ou pas si pas AMI force curseur ATTACK
               DWORD dwFactionMask = g_Global.GetPLFactionMask(Player.FactionID);
               if( !(FRIENDLYID & dwFactionMask) && STATUS != 0x00)
               {
                  //ennemie
                  STATUS = VOL_CANNOTTALK;
               }
            }
            */
            
            // 68 Part
            
            WORD  BODY, FEET, GLOVES, HELM, LEGS, W_RIGHT, W_LEFT, CAPE, HAIR, TAG;
            
            Msg->Get((short *)&junk);
            Msg->Get((long *)&ID);
            Msg->Get((short *)&BODY);
            Msg->Get((short *)&FEET);
            Msg->Get((short *)&GLOVES);
            Msg->Get((short *)&HELM);
            Msg->Get((short *)&LEGS);
            Msg->Get((short *)&W_RIGHT);
            Msg->Get((short *)&W_LEFT);
            Msg->Get((short *)&CAPE);
            Msg->Get((short *)&HAIR);
            Msg->Get((short *)&TAG);
            //NMNMNM_TAG
            
            // Addition.
            if( g_SaveGame.GetOptionsParam()->bSeraphAnim)
            {
               if (ID != Player.ID) 
               {
                  World.Lock(75);//NM:Seraph Arival
                  Objects.Lock(1002);
                  if (TYPE == __PLAYER_HUMAN_PUPPET) 
                  {
                     if (CAPE == __OBJGROUP_SERAPH_WHITE_WINGS || CAPE == __OBJGROUP_ARCHWINGS_WHITE || CAPE == __OBJGROUP_NMS_X6WHITE_WINGS) 
                     {
                        Objects.Add(ID, __SPELL_SERAPH_MALE, X, Y, 100, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     }
                     else if (CAPE == __OBJGROUP_SERAPH_BLACK_WINGS || CAPE == __OBJGROUP_DARKWINGS || CAPE == __OBJGROUP_NMS_X6BLACK_WINGS) 
                     {
                        Objects.Add(ID, __SPELL_EVIL_SERAPH_MALE, X, Y, 0, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     }

                     else if (CAPE == __OBJGROUP_SERAPH_DECHU_WINGS || CAPE == __OBJGROUP_SERAPH_DECHU2_WINGS || CAPE == __OBJGROUP_SERAPH_NMS_nDECHU_WINGS01
                                                                                                              || CAPE == __OBJGROUP_SERAPH_NMS_nDECHU_WINGS02
                                                                                                              || CAPE == __OBJGROUP_SERAPH_NMS_nDECHU_WINGS03 
                                                                                                              || CAPE == __OBJGROUP_SERAPH_DARKWINGS_PINK    ) 
                     {
                        Objects.Add(ID, __SPELL_DECHU_MALE, X, Y, 0, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     }
                     else 
                     {
								Objects.Add(ID, TYPE, X, Y, LIGHT, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     }

                  } 
                  else if (TYPE == __PLAYER_HUMAN_FEMALE) 
                  {
                     if (CAPE == __OBJGROUP_SERAPH_WHITE_WINGS || CAPE == __OBJGROUP_ARCHWINGS_WHITE || CAPE == __OBJGROUP_NMS_X6WHITE_WINGS) 
                     {
                        Objects.Add(ID, __SPELL_SERAPH_FEMALE, X, Y, 100, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     } 
                     else if (CAPE == __OBJGROUP_SERAPH_BLACK_WINGS || CAPE == __OBJGROUP_DARKWINGS || CAPE == __OBJGROUP_NMS_X6BLACK_WINGS) 
                     {
                        Objects.Add(ID, __SPELL_EVIL_SERAPH_FEMALE, X, Y, 0, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     } 
                     else if (CAPE == __OBJGROUP_SERAPH_DECHU_WINGS || CAPE == __OBJGROUP_SERAPH_DECHU2_WINGS || CAPE == __OBJGROUP_SERAPH_NMS_nDECHU_WINGS01 
                                                                                                              || CAPE == __OBJGROUP_SERAPH_NMS_nDECHU_WINGS02 
                                                                                                              || CAPE == __OBJGROUP_SERAPH_NMS_nDECHU_WINGS03 
                                                                                                              || CAPE == __OBJGROUP_SERAPH_DARKWINGS_PINK     )
                     {
                        Objects.Add(ID, __SPELL_DECHU_FEMALE, X, Y, 0, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     }
                     else 
                     {
								Objects.Add(ID, TYPE, X, Y, LIGHT, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     }
                  }
                  Objects.Unlock(1002);
                  World.Unlock(75);//NM:Seraph Arival
               } 
               else if(!Player.AlreadyPlayHisAnim)
               {
                  Player.AlreadyPlayHisAnim = true;
                  Player.xPos = X;
                  Player.yPos = Y;
                  Player.Light = LIGHT;
                  
                  g_Global.ValidMapZonePosition(Player.xPos,Player.yPos);

                  Objects.Lock(0);
                  if (CAPE == __OBJGROUP_SERAPH_WHITE_WINGS || CAPE == __OBJGROUP_ARCHWINGS_WHITE || CAPE == __OBJGROUP_NMS_X6WHITE_WINGS) 
                  {
                     Objects.Add(SpellID, __SPELL_SERAPH, X, Y, 100, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     ConnectSoundFX.Create("Seraph", TS_MEMORY);
                     ConnectSoundFX.Play(FALSE);
                  } 
                  else if (CAPE == __OBJGROUP_SERAPH_BLACK_WINGS || CAPE == __OBJGROUP_DARKWINGS || CAPE == __OBJGROUP_NMS_X6BLACK_WINGS) 
                  {
                     Objects.Add(SpellID, __SPELL_EVIL_SERAPH, X, Y, 0, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     ConnectSoundFX.Create("Evil Seraph", TS_MEMORY);
                     ConnectSoundFX.Play(FALSE);
                  } 
                  else if (CAPE == __OBJGROUP_SERAPH_DECHU_WINGS || CAPE == __OBJGROUP_SERAPH_DECHU2_WINGS || CAPE == __OBJGROUP_SERAPH_NMS_nDECHU_WINGS01
                                                                                                           || CAPE == __OBJGROUP_SERAPH_NMS_nDECHU_WINGS02 
                                                                                                           || CAPE == __OBJGROUP_SERAPH_NMS_nDECHU_WINGS03 
                                                                                                           || CAPE == __OBJGROUP_SERAPH_DARKWINGS_PINK) 
                  {
                     Objects.Add(ID, __SPELL_DECHU, X, Y, 0, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                     ConnectSoundFX.Create("Evil Seraph", TS_MEMORY);
                     ConnectSoundFX.Play(FALSE);
                  }
                  Objects.Unlock(0);
                  SpellID--;
               }
            } 
            else 
            {
               
               if (ID != Player.ID) 
               { // .. and its not my own entrance..
                  Objects.Lock(1);
                  Objects.Add(ID, TYPE, X, Y, LIGHT, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                  Objects.Unlock(1);
               }
            }
            
            Objects.Lock(57);
            Objects.SetPuppet(ID, BODY, FEET, GLOVES, HELM, LEGS, W_RIGHT, W_LEFT, CAPE, HAIR,TAG);
            Objects.Unlock(57);
            } break;
            
        case RQ_GroupToggleAutoSplit:
        {
           TFCPacket *msg = Msg;
           BYTE bAutoShare = 0;
           GET_BYTE( bAutoShare );
           V3_GroupeDlg::GetInstance()->UpdateAutoShare( bAutoShare == 0 ? false : true );
           
        } break;
           //////////////////////////////////////////////////////////////////////////////////////////
           // Update the HP of a single group member.
        case RQ_UpdateGroupMemberHp:
        {
           TFCPacket *msg = Msg;
           
           DWORD id;
           WORD  hpPercent;
           
           GET_LONG( id );
           GET_WORD( hpPercent );
           
           // Update the member's hp.
           V3_GroupeDlg::GetInstance()->UpdateMemberHp( id, hpPercent );
           Objects.Lock(0);
           Objects.SetHP( id, (char)hpPercent );
           Objects.Unlock(0);
        } break;
           
           //////////////////////////////////////////////////////////////////////////////////////////   
           // Player left group.
        case RQ_GroupLeave:
        {
           V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 520 ], SYSTEM_COLOR, false );
           list< GroupMember > emptyMemberList;
           V3_GroupeDlg::GetInstance()->UpdateMembers( emptyMemberList, false );
        }break;
           
           //////////////////////////////////////////////////////////////////////////////////////////   
           // Group got disbanded.
        case RQ_NotifyGroupDisband:
        {
           list< GroupMember > emptyMemberList;
           V3_GroupeDlg::GetInstance()->UpdateMembers( emptyMemberList, false ,true);
        }break;
           //////////////////////////////////////////////////////////////////////////////////////////
           // Receive the group members.
        case RQ_UpdateGroupMembers:
        {
           TFCPacket *msg = Msg;
           
           WORD groupSize = 0;
           
           BYTE bAutoShare = 0;
           
           GET_BYTE( bAutoShare );
           
           GET_WORD( groupSize );
           
           list< GroupMember > memberList;
           
           WORD i;
           for( i = 0; i < groupSize; i++ )
           {
              GroupMember member;
              BYTE *userName;
              BYTE  leader;
              
              GET_LONG( member.id );
              GET_WORD( member.level );
              GET_WORD( member.hpPercent );
              GET_BYTE( leader );
              
              if( leader != 0 ){
                 member.leader = true;
              }else{
                 member.leader = false;
              }
              
              GET_STRING( userName );
              
              
              member.name = reinterpret_cast< const char * >( userName );
              
              memberList.push_back( member );
              
              delete userName;
           }
           
           V3_GroupeDlg::GetInstance()->UpdateMembers( memberList, bAutoShare == 0 ? false : true );
           
        }break;
           //////////////////////////////////////////////////////////////////////////////////////////
           // Received a group invitation.
        case RQ_GroupInvite:
        {
           
           TFCPacket *msg = Msg;
           
           DWORD sourceID = 0;
           BYTE *sourceName;
           
           GET_LONG( sourceID );
           GET_STRING( sourceName );
           
           V3_GroupeDlg::GetInstance()->Join( sourceID, reinterpret_cast< const char * >( sourceName ) );
           
           delete sourceName;       
        } break;
           
           //////////////////////////////////////////////////////////////////////////////////////////   
        case RQ_GetChatterChannelList: 
        {
           V3_MainBarDlg::GetInstance()->ClearChannelList();
                      
           short wChannelQuantity = 0;
           int i = 0, j = 0;
           BYTE *channel;
           BYTE bListen;
           
           TFCPacket *msg = Msg;
           
           // Get number of channels.
           GET_WORD( wChannelQuantity );
           
           vector< V3_ChatDlg::Channel > channelList( wChannelQuantity );
           
           for (j = 0; j < wChannelQuantity; j++) 
           {
              
              GET_STRING( channel );
              GET_BYTE  ( bListen );
              
              V3_MainBarDlg::GetInstance()->AddChannel((char*)channel,bListen == 0 ? false : true);
              
              V3_ChatDlg::Channel chan;
              chan.channelID   = reinterpret_cast< const char * >( channel );
              chan.listenState = bListen == 0 ? false : true;
              channelList[ j ] = chan;
              delete channel;
           }
           V3_ChatDlg::GetInstance()->UpdateChannels(channelList );
        } break;
           
        case RQ_GetChatterUserList2: 
           {
              // Get Chatter User List
              
              short wUserQuantity = 0;
              short Len = 0;
              int i = 0, j = 0;
              BYTE *szChannel;
              BYTE *szUser;
              BYTE *szTitle;
              BYTE *szGuild;
              BYTE bListen;
              BYTE bLevel;
			     BYTE bHidden = 0;
              
              // Get Channel
              TFCPacket *msg = Msg;		
              
              GET_STRING( szChannel );
              
              // Get User
              GET_WORD( wUserQuantity );
              if(wUserQuantity >=20000 )
                 wUserQuantity-=20000;
                 
              vector< V3_ChatDlg::User > users( wUserQuantity );
              
              for (j = 0; j < wUserQuantity; j++) 
              {
                 GET_STRING( szUser  );
                 GET_STRING( szGuild );
                 GET_STRING( szTitle );
                 GET_BYTE  ( bListen );  // Is user listening.
                 GET_BYTE  ( bLevel  );  // Is user listening.
			        GET_BYTE  ( bHidden  );  // Is user listening.
                
                 
                 V3_ChatDlg::User user;
                 user.userName    = reinterpret_cast< const char * >( szUser );
                 user.title       = reinterpret_cast< const char * >( szTitle );
                 user.Guild       = reinterpret_cast< const char * >( szGuild );
                 user.UserState   = bListen == 0 ? false : true;
                 user.chGodLevel  = bLevel;
			        user.chHidden    = bHidden;
                 
                 users[ j ] = user;
                 
                 delete szUser;
                 delete szTitle;
                 delete szGuild;
              }
              
              if( V3_ChatDlg::GetInstance()->GetSelectedChannel() == reinterpret_cast< const char * >( szChannel ) )
              {
                 V3_ChatDlg::GetInstance()->UpdateChannelUsers( users );
              }
              
              delete szChannel;
        } break;
           
        case RQ_AddRemoveChatterChannel: 
        {
           
        } break;
           
        case RQ_SendChatterChannelMessage: 
        {
           // Receive Chatter Message.
           
           char *szChannel = NULL;
           char *szSender = NULL;
           char *szMessage = NULL;
           short Len = 0;
           int i = 0;
           
           // Get Channel
           Msg->Get((short *)&Len);
           szChannel = new char [Len+1];
           for (i = 0; i < Len; i++)
              Msg->Get((char *)&szChannel[i]);
           szChannel[Len] = 0;
           
           // Get Sender
           Msg->Get((short *)&Len);
           szSender = new char [Len+1];
           for (i = 0; i < Len; i++)
              Msg->Get((char *)&szSender[i]);
           szSender[Len] = 0;
           
           // Get Message
           Msg->Get((short *)&Len);
           szMessage = new char [Len+1];
           for (i = 0; i < Len; i++)
              Msg->Get((char *)&szMessage[i]);
           szMessage[Len] = 0;
           
           // Form a readable string for backlog.
           string backMsg( "[\"CC " );
           backMsg += szChannel;
           backMsg += "\"] \"";
           backMsg += szSender;
           backMsg += "\": ";
           backMsg += szMessage;
           
           DWORD color = V3_ChatDlg::GetInstance()->GetChannelColor( szChannel );
           
           
           V3_ChatLogDlg::GetInstance()->AddListLogs( szSender, backMsg, color, true, false );
           
           if(szSender)
              delete []szSender;
           if(szChannel)
              delete []szChannel;
           if(szMessage)
              delete []szMessage;
           
        } break;
           
        case 73: 
        {
            
           WORD TYPE;
           BYTE StringNumber;
           
           Msg->Get((short *)&TYPE);
           
           switch (TYPE) 
           {
              case 240: StringNumber = 192; break;
              case 241: StringNumber = 191; break;
              case 244: StringNumber = 193; break;
              default:  StringNumber = 197; break;
           }
           
           V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[StringNumber],SYSTEM_COLOR,true);
           
        } break;
           
        case RQ_ExitGame: 
        {
           g_App.SetError(1, g_LocalString[500]);
           g_App.Close(g_LocalString[500],true);
           COMM.SetAlive(15000);
        } break;
        case RQ_ExitGameAntiPlug:
           PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
        break;
           
        case RQ_MissingUnit: 
        {
           // Missing Unit
           
           DWORD ID;
           WORD  TYPE;
           
           Msg->Get((long *)&ID);
           Msg->Get((short *)&TYPE);
           
           
           if (TYPE == 24) 
           {
              // Packet Type == Attack
              Objects.Lock(51);
              if (Objects.CheckDead(ID) && !Objects.CheckDelete(ID)) 
              {
                 
                 Objects.PlAttack(ID, 0);
                 Objects.GetMainObject()->Chose = 1;
                 Objects.GetMainObject()->Speed = 0;
                 Objects.GetMainObject()->AttSpeed = 0;
                 signed char X, Y;
                 Objects.Found(&X, &Y, ID);
                 if (X < 0 && Y == 0)
                    Objects.GetMainObject()->Direction = 4;
                 else if (X > 0 && Y == 0)
                    Objects.GetMainObject()->Direction = 6;
                 else if (X == 0 && Y > 0)
                    Objects.GetMainObject()->Direction = 2;
                 else if (X == 0 && Y < 0)
                    Objects.GetMainObject()->Direction = 8;
                 else if (X < 0 && Y > 0)
                    Objects.GetMainObject()->Direction = 1;
                 else if (X < 0 && Y < 0)
                    Objects.GetMainObject()->Direction = 7;
                 else if (X > 0 && Y > 0)
                    Objects.GetMainObject()->Direction = 3;
                 else if (X > 0 && Y < 0)
                    Objects.GetMainObject()->Direction = 9;
                 else 
                    Objects.GetMainObject()->Direction = 1;
                 
                 if (Objects.SetMissing(ID, TRUE)) 
                 {
                    Objects.SetMissing(ID, FALSE);
                 }
              }
              Objects.Unlock(51);
           } 
           else if (TYPE == 11) 
           {
              Objects.Lock(52);
              if (Objects.FoundID(ID)) 
              {
                 Objects.Delete(ID);
                 V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[196],CL_RED,true);
              }
              Objects.Unlock(52);
           } 
           else 
           {
              // Something Else.
              Objects.Lock(53);	
              if (Objects.CheckDead(ID) && !Objects.CheckDelete(ID)) 
              {
                 Objects.SetMissing(ID, TRUE);
              }
              Objects.Unlock(53);
           }
           
        } break;
           
        case RQ_UnitUpdate: PacketHandling::SetUnitStat(Msg); break;
        case RQ_UpdateSmile: PacketHandling::SetUnitSmiles(Msg); break;
           
        case RQ_SpellEffect: 
           {
           WORD SPELLID;
           DWORD CASTER, TARGET;
           SHORT TPOSX, TPOSY;
           SHORT CPOSX, CPOSY;
           signed char x2, y2;
           DWORD Follow = NULL;
           DWORD SummonID = 0;
           bool EvilSpell = false;
           DWORD spellEffectId;
           DWORD spellChildId;
           

           //char strtoto[100];
           //sprintf_s(strtoto,100,"Receive Sp3ellID %d\n",SPELLID);
           //OutputDebugString(strtoto);


           
           Msg->Get((short *)&SPELLID);
           Msg->Get((long *)&CASTER);
           Msg->Get((long *)&TARGET);
           Msg->Get((short *)&TPOSX);
           Msg->Get((short *)&TPOSY);
           Msg->Get((short *)&CPOSX);
           Msg->Get((short *)&CPOSY);
           Msg->Get((long *)&spellEffectId);  // The spell effect's unique ID.
           Msg->Get((long *)&spellChildId);  // The spell effect's child effect ID.
           
           
           
           if ( Player.ID == CASTER && TARGET != Player.ID) {
              TalkToID = NULL;
           }
           
           //World.Lock(54); //NM:LockcastSpell
           Objects.Lock(1000);
           
           switch (SPELLID) 
           {
           case __SPELL_SMALLEXPLOSION:
           case __SPELL_GREATEXPLOSION:
           case __SPELL_FLAK2:
           case __SPELL_FLAK:
           case __SPELL_PENTACLE:
           case __SPELL_SHOCKWAVE:
           case __SPELL_GREATBOLT:
           // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_GREATBOLT_AZURE:
           case __SPELL_GREATBOLT_CRIMSON:
           case __SPELL_GREATBOLT_LIME:
           case __SPELL_GREATBOLT_EMERALD:
           case __SPELL_GREATBOLT_GREEN:
           case __SPELL_GREATBOLT_CANDY:
           case __SPELL_GREATBOLT_RED:
           case __SPELL_GREATBOLT_BLACK:
           // END DIALSOFT NEW SPELLS
           case __SPELL_CURSE:
           case __SPELL_ELECTRICSHIELD:
           case __SPELL_SEMIBIGEXPLOSION:
           case __SPELL_FREEZE:
           case __SPELL_ROCKFLY:
           case __SPELL_ICECLOUD:
           case __SPELL_POISONCLOUD:
           case __SPELL_ENTANGLE:
           case __SPELL_GLACIER:
           case __SPELL_GATEWAY_1:
           case __SPELL_METEORS:
           // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_METEOR_BLACK:
           case __SPELL_METEOR_WHITE:
           case __SPELL_METEOR_PURPLE:
           case __SPELL_METEOR_COBOLT:
           case __SPELL_METEOR_GREEN:
           case __SPELL_METEOR_CANDY:
           case __SPELL_METEOR_GOLD:
           case __SPELL_METEOR_BROWN:
           case __SPELL_METEOR_DARK:
           case __SPELL_METEOR_BLUE:
           case __SPELL_METEOR_VIOLET:
           // END DIALSOFT NEW SPELLS

           case __SPELL_BOULDERS: 
           case __SPELL_NMS_BOULDERS: 
           case __SPELL_FIRECIRCLE: 
           case __SPELL_FIREBALL_CIRCLE:
              EvilSpell = true;
           case __SPELL_BLESS:
           case __SPELL_HEALING:
           case __SPELL_SERAPH_MALE:
           case __SPELL_SERAPH_FEMALE:
           case __SPELL_SERAPH:
           case __SPELL_HEALSERIOUS:
           case __SPELL_GREENWIPE:
           case __SPELL_BLUEWIPE:
           case __SPELL_FIREWIPE:
           case __SPELL_REDWIPE:
              //Dialsoft new spells
           case __SPELL_MEDIUM_HEALING:
           case __SPELL_ICE_TREE:
           case __SPELL_SUPRAHEAL:
           case __SPELL_SKILLS_EFFECT:
           case __SPELL_LEVELUP:
           case __SPELL_NEW_LIGHTNING1:
           case __SPELL_NEW_FIRE1:
           case __SPELL_NEW_POISON1:
           case __SPELL_NEW_NMS_SUPRAHEAL1:
           case __SPELL_NEW_NMS_SUPRAHEAL2:
           case __SPELL_NEW_NMS_SUPRAHEAL3:
           case __SPELL_NEW_NMS_SUPRAHEAL4:
           case __SPELL_ENTER_COMBAT_MODE1:
           case __SPELL_ENTER_COMBAT_MODE2:
           case __SPELL_ENTER_COMBAT_MODE3:
           case __SPELL_NEW_NMS_LOVELOVELOVE :
           case __SPELL_NEW_NMS_LOVE002:
           case __SPELL_NEW_SHOUT01:
           case __SPELL_NEW_SHOUT02:
           case __SPELL_NEW_SHOUT02_VERT  :
           case __SPELL_NEW_SHOUT02_BLEU  :
           case __SPELL_NEW_SHOUT02_ORANGE:
           case __SPELL_NEW_SHOUT02_JAUNE :
           case __SPELL_NEW_SHOUT02_VIOLET:
           case __SPELL_NEW_IMMOBILISATION:
           case __SPELL_NEW_IMMOBILISATION_ROUGE  :
           case __SPELL_NEW_IMMOBILISATION_VERT   :
           case __SPELL_NEW_IMMOBILISATION_BLEU   :
           case __SPELL_NEW_IMMOBILISATION_ORANGE :
           case __SPELL_NEW_IMMOBILISATION_JAUNE  :
           case __SPELL_NEW_IMMOBILISATION_VIOLET :
           case __SPELL_NEW_VERGLA:
           case __SPELL_NEW_BLOOD:

           case __SPELL_NEW_NMS_BALL_PARTICLE_BLEU:
           case __SPELL_NEW_NMS_BALL_PARTICLE_RED:
           case __SPELL_NEW_NMS_BALL_PARTICLE_FIRE:
           case __SPELL_NEW_NMS_BALL_PARTICLE_GREEN:
           case __SPELL_NEW_NMS_BALL_PARTICLE_YELLOW:
           case __SPELL_NEW_NMS_BALL_PARTICLE_PURPLE:
           case __SPELL_NEW_NMS_BALL_PARTICLE_PINK :
           case __SPELL_NEW_NMS_BALL_PARTICLE_WHITE:
           case __SPELL_NEW_NMS_ROCKE01ALPHA:
           case __SPELL_NEW_NMS_ROCKS01ALPHA:
           case __SPELL_NEW_NMS_ROCKF01ALPHA:
           case __SPELL_NEW_NMS_LIGHT01ALPHA:
           case __SPELL_NEW_NMS_BALL_BLEU    :
           case __SPELL_NEW_NMS_BALL_RED     :
           case __SPELL_NEW_NMS_BALL_FIRE    :
           case __SPELL_NEW_NMS_BALL_GREEN   :
           case __SPELL_NEW_NMS_BALL_YELLOW  :
           case __SPELL_NEW_NMS_BALL_PURPLE  :
           case __SPELL_NEW_NMS_BALL_PINK    :
           case __SPELL_NEW_NMS_BALL_WHITE   :
           

              //Dialsoft end
              if (!TARGET && !TPOSX && !TPOSY)
                 TARGET = CASTER;
              
              if (TARGET) 
              {
                 if (Player.ID == TARGET) 
                 {
                    CPOSX = Player.xPos;
                    CPOSY = Player.yPos;
                 } 
                 else 
                 {
                    if (Objects.Found(&x2, &y2, TARGET)) 
                    {
                       CPOSX = x2 + Player.xPos;
                       CPOSY = y2 + Player.yPos;
                    } 
                    else 
                    {
                       CPOSX = TPOSX;
                       CPOSY = TPOSY;
                    }
                 } 
                 TPOSX = 0;
                 TPOSY = 0;
                 Follow = TARGET;
              } else {
                 CPOSX = TPOSX;
                 CPOSY = TPOSY;
                 TPOSX = 0;
                 TPOSY = 0;
              } 
              break;
              
           case __SPELL_FIREBALL:
           case __SPELL_FIREBOLT:
           // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_FIREBOLT_BLACK:
           case __SPELL_FIREBOLT_WHITE:
           case __SPELL_FIREBOLT_YELLOW:
           case __SPELL_FIREBOLT_GREEN:
           case __SPELL_FIREBOLT_BLUE:
           case __SPELL_FIREBOLT_PURPLE:
           // END DIALSOFT NEW SPELLS

           case __SPELL_LIGHTNINGBOLT:
           case __SPELL_ICESHARD:
           case __SPELL_STONESHARD:
           case __SPELL_SWORD01:
           case __SPELL_SNOWBALL:
           case __SPELL_SNOWGIFT:
           case __SPELL_POISONARROW:
           case __SPELL_CURSE_WITH_RED_BALL:
           case __SPELL_FIREWIPE_WITH_RED_BALL:
           case __SPELL_FLAK_WITH_RED_BALL:
           case __SPELL_FLAK2_WITH_RED_BALL:
           case __SPELL_GREATEXPLOSION_WITH_RED_BALL:
           case __SPELL_METEOR_WITH_RED_BALL:
           case __SPELL_PENTACLE_WITH_RED_BALL:
           case __SPELL_SEMIBIGEXPLOSION_WITH_RED_BALL:
           case __SPELL_SMALLEXPLOSION_WITH_RED_BALL:
           case __SPELL_FIRECIRCLE_WITH_RED_BALL:
              // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_METEOR_BLACK_WITH_RED_BALL:
           case __SPELL_METEOR_WHITE_WITH_RED_BALL:
           case __SPELL_METEOR_PURPLE_WITH_RED_BALL:
           case __SPELL_METEOR_COBOLT_WITH_RED_BALL:
           case __SPELL_METEOR_GREEN_WITH_RED_BALL:
           case __SPELL_METEOR_CANDY_WITH_RED_BALL:
           case __SPELL_METEOR_GOLD_WITH_RED_BALL:
           case __SPELL_METEOR_BROWN_WITH_RED_BALL:
           case __SPELL_METEOR_DARK_WITH_RED_BALL:
           case __SPELL_METEOR_BLUE_WITH_RED_BALL:
           case __SPELL_METEOR_VIOLET_WITH_RED_BALL:
           case __SPELL_GREATBOLT_AZURE_WITH_RED_BALL:
           case __SPELL_GREATBOLT_CRIMSON_WITH_RED_BALL:
           case __SPELL_GREATBOLT_LIME_WITH_RED_BALL:
           case __SPELL_GREATBOLT_EMERALD_WITH_RED_BALL:
           case __SPELL_GREATBOLT_GREEN_WITH_RED_BALL:
           case __SPELL_GREATBOLT_CANDY_WITH_RED_BALL:
           case __SPELL_GREATBOLT_RED_WITH_RED_BALL:
           case __SPELL_GREATBOLT_BLACK_WITH_RED_BALL:
              // END DIALSOFT NEW SPELLS

           case __SPELL_FREEZE_WITH_BLUE_BALL:
           case __SPELL_GLACIER_WITH_BLUE_BALL:
           case __SPELL_GREATBOLT_WITH_BLUE_BALL:
           case __SPELL_ICECLOUD_WITH_BLUE_BALL:
           case __SPELL_POISONCLOUD_WITH_BLUE_BALL:
              // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_METEOR_WITH_BLUE_BALL:
           case __SPELL_METEOR_BLACK_WITH_BLUE_BALL:
           case __SPELL_METEOR_WHITE_WITH_BLUE_BALL:
           case __SPELL_METEOR_PURPLE_WITH_BLUE_BALL:
           case __SPELL_METEOR_COBOLT_WITH_BLUE_BALL:
           case __SPELL_METEOR_GREEN_WITH_BLUE_BALL:
           case __SPELL_METEOR_CANDY_WITH_BLUE_BALL:
           case __SPELL_METEOR_GOLD_WITH_BLUE_BALL:
           case __SPELL_METEOR_BROWN_WITH_BLUE_BALL:
           case __SPELL_METEOR_DARK_WITH_BLUE_BALL:
           case __SPELL_METEOR_BLUE_WITH_BLUE_BALL:
           case __SPELL_METEOR_VIOLET_WITH_BLUE_BALL:
           case __SPELL_GREATBOLT_AZURE_WITH_BLUE_BALL:
           case __SPELL_GREATBOLT_CRIMSON_WITH_BLUE_BALL:
           case __SPELL_GREATBOLT_LIME_WITH_BLUE_BALL:
           case __SPELL_GREATBOLT_EMERALD_WITH_BLUE_BALL:
           case __SPELL_GREATBOLT_GREEN_WITH_BLUE_BALL:
           case __SPELL_GREATBOLT_CANDY_WITH_BLUE_BALL:
           case __SPELL_GREATBOLT_RED_WITH_BLUE_BALL:
           case __SPELL_GREATBOLT_BLACK_WITH_BLUE_BALL:
              // END DIALSOFT NEW SPELLS

           case __SPELL_FREEZE_WITH_YELLOW_BALL:
           case __SPELL_GREATBOLT_WITH_YELLOW_BALL:
           case __SPELL_ICECLOUD_WITH_YELLOW_BALL:
              // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_METEOR_WITH_YELLOW_BALL:
           case __SPELL_METEOR_BLACK_WITH_YELLOW_BALL:
           case __SPELL_METEOR_WHITE_WITH_YELLOW_BALL:
           case __SPELL_METEOR_PURPLE_WITH_YELLOW_BALL:
           case __SPELL_METEOR_COBOLT_WITH_YELLOW_BALL:
           case __SPELL_METEOR_GREEN_WITH_YELLOW_BALL:
           case __SPELL_METEOR_CANDY_WITH_YELLOW_BALL:
           case __SPELL_METEOR_GOLD_WITH_YELLOW_BALL:
           case __SPELL_METEOR_BROWN_WITH_YELLOW_BALL:
           case __SPELL_METEOR_DARK_WITH_YELLOW_BALL:
           case __SPELL_METEOR_BLUE_WITH_YELLOW_BALL:
           case __SPELL_METEOR_VIOLET_WITH_YELLOW_BALL:
           case __SPELL_GREATBOLT_AZURE_WITH_YELLOW_BALL:
           case __SPELL_GREATBOLT_CRIMSON_WITH_YELLOW_BALL:
           case __SPELL_GREATBOLT_LIME_WITH_YELLOW_BALL:
           case __SPELL_GREATBOLT_EMERALD_WITH_YELLOW_BALL:
           case __SPELL_GREATBOLT_GREEN_WITH_YELLOW_BALL:
           case __SPELL_GREATBOLT_CANDY_WITH_YELLOW_BALL:
           case __SPELL_GREATBOLT_RED_WITH_YELLOW_BALL:
           case __SPELL_GREATBOLT_BLACK_WITH_YELLOW_BALL:
              // END DIALSOFT NEW SPELLS

           case __SPELL_BOULDERS_WITH_GREEN_BALL:
           case __SPELL_NMS_BOULDERS_WITH_GREEN_BALL:
           case __SPELL_ENTANGLE_WITH_GREEN_BALL:
           case __SPELL_FLAK_WITH_GREEN_BALL:
           case __SPELL_FLAK2_WITH_GREEN_BALL:
           case __SPELL_PENTACLE_WITH_GREEN_BALL:
           case __SPELL_POISONCLOUD_WITH_GREEN_BALL:
              // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_METEOR_WITH_GREEN_BALL:
           case __SPELL_METEOR_BLACK_WITH_GREEN_BALL:
           case __SPELL_METEOR_WHITE_WITH_GREEN_BALL:
           case __SPELL_METEOR_PURPLE_WITH_GREEN_BALL:
           case __SPELL_METEOR_COBOLT_WITH_GREEN_BALL:
           case __SPELL_METEOR_GREEN_WITH_GREEN_BALL:
           case __SPELL_METEOR_CANDY_WITH_GREEN_BALL:
           case __SPELL_METEOR_GOLD_WITH_GREEN_BALL:
           case __SPELL_METEOR_BROWN_WITH_GREEN_BALL:
           case __SPELL_METEOR_DARK_WITH_GREEN_BALL:
           case __SPELL_METEOR_BLUE_WITH_GREEN_BALL:
           case __SPELL_METEOR_VIOLET_WITH_GREEN_BALL:
           case __SPELL_GREATBOLT_AZURE_WITH_GREEN_BALL:
           case __SPELL_GREATBOLT_CRIMSON_WITH_GREEN_BALL:
           case __SPELL_GREATBOLT_LIME_WITH_GREEN_BALL:
           case __SPELL_GREATBOLT_EMERALD_WITH_GREEN_BALL:
           case __SPELL_GREATBOLT_GREEN_WITH_GREEN_BALL:
           case __SPELL_GREATBOLT_CANDY_WITH_GREEN_BALL:
           case __SPELL_GREATBOLT_RED_WITH_GREEN_BALL:
           case __SPELL_GREATBOLT_BLACK_WITH_GREEN_BALL:
              // END DIALSOFT NEW SPELLS

           case __SPELL_FLAK_WITH_WHITE_BALL:
           case __SPELL_FLAK2_WITH_WHITE_BALL:
              // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_METEOR_WITH_WHITE_BALL:
           case __SPELL_METEOR_BLACK_WITH_WHITE_BALL:
           case __SPELL_METEOR_WHITE_WITH_WHITE_BALL:
           case __SPELL_METEOR_PURPLE_WITH_WHITE_BALL:
           case __SPELL_METEOR_COBOLT_WITH_WHITE_BALL:
           case __SPELL_METEOR_GREEN_WITH_WHITE_BALL:
           case __SPELL_METEOR_CANDY_WITH_WHITE_BALL:
           case __SPELL_METEOR_GOLD_WITH_WHITE_BALL:
           case __SPELL_METEOR_BROWN_WITH_WHITE_BALL:
           case __SPELL_METEOR_DARK_WITH_WHITE_BALL:
           case __SPELL_METEOR_BLUE_WITH_WHITE_BALL:
           case __SPELL_METEOR_VIOLET_WITH_WHITE_BALL:
           case __SPELL_GREATBOLT_AZURE_WITH_WHITE_BALL:
           case __SPELL_GREATBOLT_CRIMSON_WITH_WHITE_BALL:
           case __SPELL_GREATBOLT_LIME_WITH_WHITE_BALL:
           case __SPELL_GREATBOLT_EMERALD_WITH_WHITE_BALL:
           case __SPELL_GREATBOLT_GREEN_WITH_WHITE_BALL:
           case __SPELL_GREATBOLT_CANDY_WITH_WHITE_BALL:
           case __SPELL_GREATBOLT_RED_WITH_WHITE_BALL:
           case __SPELL_GREATBOLT_BLACK_WITH_WHITE_BALL:
              // END DIALSOFT NEW SPELLS

           case __SPELL_CURSE_WITH_BLACK_BALL:
           case __SPELL_ENTANGLE_WITH_BLACK_BALL:
           case __SPELL_GREATEXPLOSION_WITH_BLACK_BALL:
           case __SPELL_METEOR_WITH_BLACK_BALL:
           case __SPELL_SEMIBIGEXPLOSION_WITH_BLACK_BALL:
           case __SPELL_SMALLEXPLOSION_WITH_BLACK_BALL:
           case __SPELL_FIRECIRCLE_WITH_BLACK_BALL:
              // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_METEOR_BLACK_WITH_BLACK_BALL:
           case __SPELL_METEOR_WHITE_WITH_BLACK_BALL:
           case __SPELL_METEOR_PURPLE_WITH_BLACK_BALL:
           case __SPELL_METEOR_COBOLT_WITH_BLACK_BALL:
           case __SPELL_METEOR_GREEN_WITH_BLACK_BALL:
           case __SPELL_METEOR_CANDY_WITH_BLACK_BALL:
           case __SPELL_METEOR_GOLD_WITH_BLACK_BALL:
           case __SPELL_METEOR_BROWN_WITH_BLACK_BALL:
           case __SPELL_METEOR_DARK_WITH_BLACK_BALL:
           case __SPELL_METEOR_BLUE_WITH_BLACK_BALL:
           case __SPELL_METEOR_VIOLET_WITH_BLACK_BALL:
           case __SPELL_GREATBOLT_AZURE_WITH_BLACK_BALL:
           case __SPELL_GREATBOLT_CRIMSON_WITH_BLACK_BALL:
           case __SPELL_GREATBOLT_LIME_WITH_BLACK_BALL:
           case __SPELL_GREATBOLT_EMERALD_WITH_BLACK_BALL:
           case __SPELL_GREATBOLT_GREEN_WITH_BLACK_BALL:
           case __SPELL_GREATBOLT_CANDY_WITH_BLACK_BALL:
           case __SPELL_GREATBOLT_RED_WITH_BLACK_BALL:
           case __SPELL_GREATBOLT_BLACK_WITH_BLACK_BALL:
              // END DIALSOFT NEW SPELLS
              
           case __SPELL_FLAK_WITH_PURPLE_BALL:
           case __SPELL_FLAK2_WITH_PURPLE_BALL:
           case __SPELL_GREATEXPLOSION_WITH_PURPLE_BALL:
           case __SPELL_METEOR_WITH_PURPLE_BALL:
           case __SPELL_PENTACLE_WITH_PURPLE_BALL:
           case __SPELL_POISONCLOUD_WITH_PURPLE_BALL:
           case __SPELL_SEMIBIGEXPLOSION_WITH_PURPLE_BALL:
           case __SPELL_SMALLEXPLOSION_WITH_PURPLE_BALL:
           case __SPELL_FIRECIRCLE_WITH_PURPLE_BALL:
              // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_METEOR_BLACK_WITH_PURPLE_BALL:
           case __SPELL_METEOR_WHITE_WITH_PURPLE_BALL:
           case __SPELL_METEOR_PURPLE_WITH_PURPLE_BALL:
           case __SPELL_METEOR_COBOLT_WITH_PURPLE_BALL:
           case __SPELL_METEOR_GREEN_WITH_PURPLE_BALL:
           case __SPELL_METEOR_CANDY_WITH_PURPLE_BALL:
           case __SPELL_METEOR_GOLD_WITH_PURPLE_BALL:
           case __SPELL_METEOR_BROWN_WITH_PURPLE_BALL:
           case __SPELL_METEOR_DARK_WITH_PURPLE_BALL:
           case __SPELL_METEOR_BLUE_WITH_PURPLE_BALL:
           case __SPELL_METEOR_VIOLET_WITH_PURPLE_BALL:
           case __SPELL_GREATBOLT_AZURE_WITH_PURPLE_BALL:
           case __SPELL_GREATBOLT_CRIMSON_WITH_PURPLE_BALL:
           case __SPELL_GREATBOLT_LIME_WITH_PURPLE_BALL:
           case __SPELL_GREATBOLT_EMERALD_WITH_PURPLE_BALL:
           case __SPELL_GREATBOLT_GREEN_WITH_PURPLE_BALL:
           case __SPELL_GREATBOLT_CANDY_WITH_PURPLE_BALL:
           case __SPELL_GREATBOLT_RED_WITH_PURPLE_BALL:
           case __SPELL_GREATBOLT_BLACK_WITH_PURPLE_BALL:
              // END DIALSOFT NEW SPELLS

           case __SPELL_NEWFIREBALL:
           case __SPELL_NEWFIREBALL2:
           case __SPELL_NEW_NMS_JETPIERRE:
              // BEGIN DIALSOFT NEW SPELLS
           case __SPELL_NEWFIREBALL_BLACK:
           case __SPELL_NEWFIREBALL_WHITE:
           case __SPELL_NEWFIREBALL_AZURE:
           case __SPELL_NEWFIREBALL_MAGENTA:
           case __SPELL_NEWFIREBALL_YELLOW:
           case __SPELL_NEWFIREBALL_PURPLE:
              // END DIALSOFT NEW SPELLS

           case __SPELL_ELECTRICSHIELD_WITH_PURPLE_BALL:
              EvilSpell = true;
           case __SPELL_CURSE_WITH_PURPLE_BALL:
           case __SPELL_RED_ENERGY_BALL:
           case __SPELL_BLUE_ENERGY_BALL:
           case __SPELL_YELLOW_ENERGY_BALL:
           case __SPELL_GREEN_ENERGY_BALL:
           case __SPELL_WHITE_ENERGY_BALL:
           case __SPELL_BLACK_ENERGY_BALL:
           case __SPELL_PURPLE_ENERGY_BALL:
           case __SPELL_GATEWAY_1_WITH_RED_BALL:
           case __SPELL_REDWIPE_WITH_RED_BALL:
           case __SPELL_BLUEWIPE_WITH_BLUE_BALL:
           case __SPELL_CURSE_WITH_BLUE_BALL:
           case __SPELL_ELECTRICSHIELD_WITH_BLUE_BALL:
           case __SPELL_GREENWIPE_WITH_BLUE_BALL:
           case __SPELL_HEALING_WITH_BLUE_BALL:
           case __SPELL_HEALSERIOUS_WITH_BLUE_BALL:
           case __SPELL_BLESS_WITH_YELLOW_BALL:
           case __SPELL_BLUEWIPE_WITH_YELLOW_BALL:
           case __SPELL_CURSE_WITH_YELLOW_BALL:
           case __SPELL_ELECTRICSHIELD_WITH_YELLOW_BALL:
           case __SPELL_GATEWAY_1_WITH_YELLOW_BALL:
           case __SPELL_CURSE_WITH_GREEN_BALL:
           case __SPELL_GREENWIPE_WITH_GREEN_BALL:
           case __SPELL_ROCKFLY_WITH_GREEN_BALL:
           case __SPELL_BLESS_WITH_WHITE_BALL:
           case __SPELL_BLUEWIPE_WITH_WHITE_BALL:
           case __SPELL_CURSE_WITH_WHITE_BALL:
           case __SPELL_ELECTRICSHIELD_WITH_WHITE_BALL:
           case __SPELL_HEALING_WITH_WHITE_BALL:
           case __SPELL_HEALSERIOUS_WITH_WHITE_BALL:
           case __SPELL_ICECLOUD_WITH_WHITE_BALL:
           case __SPELL_FIREWIPE_WITH_BLACK_BALL:
           case __SPELL_GREENWIPE_WITH_BLACK_BALL:
           case __SPELL_REDWIPE_WITH_BLACK_BALL:
           case __SPELL_BLUEWIPE_WITH_PURPLE_BALL:
           case __SPELL_GATEWAY_1_WITH_PURPLE_BALL:
           case __SPELL_GREENWIPE_WITH_PURPLE_BALL:

           case __SPELL_NEW_NMS_BALL_PARTICLE_BLEU_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_PARTICLE_RED_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_PARTICLE_FIRE_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_PARTICLE_GREEN_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_PARTICLE_YELLOW_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_PARTICLE_PURPLE_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_PARTICLE_PINK_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_PARTICLE_WHITE_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_BLEU_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_RED_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_FIRE_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_GREEN_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_YELLOW_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_PURPLE_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_PINK_WITH_BALL:
           case __SPELL_NEW_NMS_BALL_WHITE_WITH_BALL:
           case __SPELL_NEW_FIRE1_WITH_BALL:
           case __SPELL_NEW_IMMOBILISATION_WITH_BALL:
           case __SPELL_NEW_IMMOBILISATION_ROUGE_WITH_BALL:
           case __SPELL_NEW_IMMOBILISATION_VERT_WITH_BALL:
           case __SPELL_NEW_IMMOBILISATION_BLEU_WITH_BALL:
           case __SPELL_NEW_IMMOBILISATION_ORANGE_WITH_BALL:
           case __SPELL_NEW_IMMOBILISATION_JAUNE_WITH_BALL:
           case __SPELL_NEW_IMMOBILISATION_VIOLET_WITH_BALL:
           case __SPELL_NEW_NMS_JETPIERRE_WITH_BALL:
           case __SPELL_NEW_NMS_LIGHT01ALPHA_WITH_BALL:
           case __SPELL_NEW_LIGHTNING1_WITH_BALL:
           case __SPELL_NEW_POISON1_WITH_BALL:
           case __SPELL_NEW_NMS_ROCKE01ALPHA_WITH_BALL:
           case __SPELL_NEW_NMS_ROCKS01ALPHA_WITH_BALL:
           case __SPELL_NEW_NMS_ROCKF01ALPHA_WITH_BALL:
           case __SPELL_NEW_SHOUT01_WITH_BALL:
           case __SPELL_NEW_SHOUT02_WITH_BALL:
           case __SPELL_NEW_SHOUT02_VERT_WITH_BALL:
           case __SPELL_NEW_SHOUT02_BLEU_WITH_BALL:
           case __SPELL_NEW_SHOUT02_ORANGE_WITH_BALL:
           case __SPELL_NEW_SHOUT02_JAUNE_WITH_BALL:
           case  __SPELL_NEW_SHOUT02_VIOLET_WITH_BALL:
           case  __SPELL_NEW_NMS_SUPRAHEAL1_WITH_BALL:
           case  __SPELL_NEW_NMS_SUPRAHEAL2_WITH_BALL:
           case  __SPELL_NEW_NMS_SUPRAHEAL3_WITH_BALL:
           case  __SPELL_NEW_NMS_SUPRAHEAL4_WITH_BALL:
           case  __SPELL_NEW_VERGLA_WITH_BALL:

              // Drake5
              if (TARGET) 
              {
                 Follow = TARGET;
                 if (Player.ID == TARGET) 
                 {
                    TPOSX = Player.xPos;
                    TPOSY = Player.yPos;
                 } 
                 else 
                 {
                    if (Objects.Found((signed char *)&x2, (signed char *)&y2, TARGET)) 
                    {
                       TPOSX = x2 + Player.xPos;
                       TPOSY = y2 + Player.yPos;
                    }
                 }
              }
              
              if (CASTER) 
              {
                 if (Player.ID == CASTER && !(TPOSX == Player.xPos && TPOSY == Player.yPos)) {
                    CPOSX = Player.xPos;
                    CPOSY = Player.yPos;
                    Objects.GetMainObject()->Direction = 1;
                    int a = ((TPOSX-Player.xPos)+11)*3;
                    int b = ((TPOSY-Player.yPos)+16)*2;
                    if (b > 30) 
                    {
                       if (a > 30) 
                       {
                          a -= 30;
                          b -= 30;
                          if (a > b*2) 
                          {
                             Objects.GetMainObject()->Direction = 6;
                          } 
                          else if (b > a*2) 
                          {
                             Objects.GetMainObject()->Direction = 2;
                          } 
                          else 
                          {
                             Objects.GetMainObject()->Direction = 3;	
                          }
                       } 
                       else 
                       {
                          b -= 30;
                          a = 30 - a;
                          if (a > b*2) 
                          {
                             Objects.GetMainObject()->Direction = 4;
                          } 
                          else if (b > a*2) 
                          {
                             Objects.GetMainObject()->Direction = 2;
                          } 
                          else 
                          {
                             Objects.GetMainObject()->Direction = 1;
                          }
                       }
                    } 
                    else 
                    {
                       if (a > 30) 
                       {
                          a -= 30;
                          b = 30 - b;
                          if (a > b*2) 
                          {
                             Objects.GetMainObject()->Direction = 6;
                          } 
                          else if (b > a*2) 
                          {
                             Objects.GetMainObject()->Direction = 8;
                          } 
                          else 
                          {
                             Objects.GetMainObject()->Direction = 9;
                          }
                       } 
                       else 
                       {
                          a = 30 - a;
                          b = 30 - b;
                          if (a > b*2) 
                          {
                             Objects.GetMainObject()->Direction = 4;
                          } 
                          else if (b > a*2) 
                          {
                             Objects.GetMainObject()->Direction = 8;
                          } 
                          else 
                          {
                             Objects.GetMainObject()->Direction = 7;
                          }
                       }			
                    }
                 } 
                 else 
                 {
                    if (Objects.Found((signed char *)&x2, (signed char *)&y2, CASTER)) {
                       CPOSX = x2 + Player.xPos;
                       CPOSY = y2 + Player.yPos;
                       Objects.SetDirection(CASTER, TARGET, true);
                    }
                 }
              }
              break;
      }
      
      switch (SPELLID) {
      case __SPELL_FIREBALL_CIRCLE:
         {
            float radius = 7;
            int nbProjectile = 20;
            
            double pi = 3.141592654;
            static int iAngle = 0;
            double angle = (float)iAngle;
            iAngle++;            
            for (int ite = 0; ite < nbProjectile; ite++) 
            {
               Objects.Add(SpellID, __SPELL_FIREBALL, CPOSX, CPOSY, 0, 0, Follow, 100, -1,-1,2, false, spellEffectId, spellChildId);
               Objects.MoveObject(SpellID, (long)(CPOSX+( cos( angle ) * radius )), (long)(CPOSY+( sin( angle ) * radius )), TRUE, SummonID, 100,-1,-1, SPELLID, 0, 100,false,false,0);
               SpellID--;
               angle += (pi*2)/(float)nbProjectile;
            }
         } break;
      default:
         
         
         Objects.Add(SpellID, SPELLID, CPOSX, CPOSY, 0, 0, Follow, 100, -1,-1,2, false, spellEffectId, spellChildId);
         if (TPOSX && TPOSY) {
            switch (SPELLID) {
            case __SPELL_FIREBALL:      SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_FIREBOLT:      SummonID = __SPELL_SMALLEXPLOSION;  break;
               // BEGIN DIALSOFT NEW SPELLS
            case __SPELL_FIREBOLT_BLACK:		SummonID = __SPELL_SMALLEXPLOSION;  break;
            case __SPELL_FIREBOLT_WHITE:			SummonID = __SPELL_SMALLEXPLOSION;  break;
            case __SPELL_FIREBOLT_YELLOW:		SummonID = __SPELL_SMALLEXPLOSION;  break;
            case __SPELL_FIREBOLT_GREEN:		SummonID = __SPELL_SMALLEXPLOSION;  break;
            case __SPELL_FIREBOLT_BLUE:			SummonID = __SPELL_SMALLEXPLOSION;  break;
            case __SPELL_FIREBOLT_PURPLE:		SummonID = __SPELL_SMALLEXPLOSION;  break;
               // END DIALSOFT NEW SPELLS
            case __SPELL_NEWFIREBALL:   SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_NEWFIREBALL2:  SummonID = __SPELL_GREATEXPLOSION;  break;
               // BEGIN DIALSOFT NEW SPELLS
            case __SPELL_NEWFIREBALL_BLACK:   SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_NEWFIREBALL_WHITE:   SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_NEWFIREBALL_AZURE:   SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_NEWFIREBALL_MAGENTA:   SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_NEWFIREBALL_YELLOW:   SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_NEWFIREBALL_PURPLE:   SummonID = __SPELL_GREATEXPLOSION;  break;
               // END DIALSOFT NEW SPELLS

            case __SPELL_LIGHTNINGBOLT: SummonID = __SPELL_ELECTRICSHIELD;  break;
            case __SPELL_ICESHARD:      SummonID = __SPELL_ICECLOUD;        break;
            case __SPELL_STONESHARD:    SummonID = __SPELL_ROCKFLY;         break;
            case __SPELL_POISONARROW:   SummonID = __SPELL_POISONCLOUD;     break;
            case __SPELL_BLESS_WITH_YELLOW_BALL:				SummonID = __SPELL_BLESS;  break;
            case __SPELL_BLESS_WITH_WHITE_BALL:					SummonID = __SPELL_BLESS;  break;
            case __SPELL_BLUEWIPE_WITH_BLUE_BALL:				SummonID = __SPELL_BLUEWIPE;  break;
            case __SPELL_BLUEWIPE_WITH_YELLOW_BALL:			SummonID = __SPELL_BLUEWIPE;  break;
            case __SPELL_BLUEWIPE_WITH_WHITE_BALL:				SummonID = __SPELL_BLUEWIPE;  break;
            case __SPELL_BLUEWIPE_WITH_PURPLE_BALL:			SummonID = __SPELL_BLUEWIPE;  break;
            case __SPELL_BOULDERS_WITH_GREEN_BALL:				SummonID = __SPELL_BOULDERS;  break;
            case __SPELL_NMS_BOULDERS_WITH_GREEN_BALL:      SummonID = __SPELL_NMS_BOULDERS;  break;
            case __SPELL_CURSE_WITH_RED_BALL:					SummonID = __SPELL_CURSE;  break;
            case __SPELL_CURSE_WITH_BLUE_BALL:					SummonID = __SPELL_CURSE;  break;
            case __SPELL_CURSE_WITH_YELLOW_BALL:				SummonID = __SPELL_CURSE;  break;
            case __SPELL_CURSE_WITH_GREEN_BALL:					SummonID = __SPELL_CURSE;  break;
            case __SPELL_CURSE_WITH_WHITE_BALL:					SummonID = __SPELL_CURSE;  break;
            case __SPELL_CURSE_WITH_BLACK_BALL:					SummonID = __SPELL_CURSE;  break;
            case __SPELL_CURSE_WITH_PURPLE_BALL:				SummonID = __SPELL_CURSE;  break;
            case __SPELL_ELECTRICSHIELD_WITH_BLUE_BALL:		SummonID = __SPELL_ELECTRICSHIELD;  break;
            case __SPELL_ELECTRICSHIELD_WITH_YELLOW_BALL:	SummonID = __SPELL_ELECTRICSHIELD;  break;
            case __SPELL_ELECTRICSHIELD_WITH_WHITE_BALL:		SummonID = __SPELL_ELECTRICSHIELD;  break;
            case __SPELL_ELECTRICSHIELD_WITH_PURPLE_BALL:	SummonID = __SPELL_ELECTRICSHIELD;  break;
            case __SPELL_ENTANGLE_WITH_GREEN_BALL:				SummonID = __SPELL_ENTANGLE;  break;
            case __SPELL_ENTANGLE_WITH_BLACK_BALL:				SummonID = __SPELL_ENTANGLE;  break;
            case __SPELL_FIREWIPE_WITH_RED_BALL:				SummonID = __SPELL_FIREWIPE;  break;
            case __SPELL_FIREWIPE_WITH_BLACK_BALL:				SummonID = __SPELL_FIREWIPE;  break;
            case __SPELL_FLAK_WITH_RED_BALL:						SummonID = __SPELL_FLAK;  break;
            case __SPELL_FLAK_WITH_GREEN_BALL:					SummonID = __SPELL_FLAK;  break;
            case __SPELL_FLAK_WITH_WHITE_BALL:					SummonID = __SPELL_FLAK;  break;
            case __SPELL_FLAK_WITH_PURPLE_BALL:					SummonID = __SPELL_FLAK;  break;
            case __SPELL_FLAK2_WITH_RED_BALL:					SummonID = __SPELL_FLAK2;  break;
            case __SPELL_FLAK2_WITH_GREEN_BALL:					SummonID = __SPELL_FLAK2;  break;
            case __SPELL_FLAK2_WITH_WHITE_BALL:					SummonID = __SPELL_FLAK2;  break;
            case __SPELL_FLAK2_WITH_PURPLE_BALL:				SummonID = __SPELL_FLAK2;  break;
            case __SPELL_FREEZE_WITH_BLUE_BALL:					SummonID = __SPELL_FREEZE;  break;
            case __SPELL_FREEZE_WITH_YELLOW_BALL:				SummonID = __SPELL_FREEZE;  break;
            case __SPELL_GATEWAY_1_WITH_RED_BALL:				SummonID = __SPELL_GATEWAY_1;  break;
            case __SPELL_GATEWAY_1_WITH_YELLOW_BALL:			SummonID = __SPELL_GATEWAY_1;  break;
            case __SPELL_GATEWAY_1_WITH_PURPLE_BALL:			SummonID = __SPELL_GATEWAY_1;  break;
            case __SPELL_GLACIER_WITH_BLUE_BALL:				SummonID = __SPELL_GLACIER;  break;
            case __SPELL_GREATBOLT_WITH_BLUE_BALL:				SummonID = __SPELL_GREATBOLT;  break;
            case __SPELL_GREATBOLT_WITH_YELLOW_BALL:			SummonID = __SPELL_GREATBOLT;  break;
               // BEGIN DIALSOFT NEW SPELLS
            case __SPELL_GREATBOLT_AZURE_WITH_RED_BALL:			SummonID = __SPELL_GREATBOLT_AZURE;  break;
            case __SPELL_GREATBOLT_CRIMSON_WITH_RED_BALL:		SummonID = __SPELL_GREATBOLT_CRIMSON;  break;
            case __SPELL_GREATBOLT_LIME_WITH_RED_BALL:			SummonID = __SPELL_GREATBOLT_LIME;  break;
            case __SPELL_GREATBOLT_EMERALD_WITH_RED_BALL:		SummonID = __SPELL_GREATBOLT_EMERALD;  break;
            case __SPELL_GREATBOLT_GREEN_WITH_RED_BALL:			SummonID = __SPELL_GREATBOLT_GREEN;  break;
            case __SPELL_GREATBOLT_CANDY_WITH_RED_BALL:			SummonID = __SPELL_GREATBOLT_CANDY;  break;
            case __SPELL_GREATBOLT_RED_WITH_RED_BALL:			SummonID = __SPELL_GREATBOLT_RED;  break;
            case __SPELL_GREATBOLT_BLACK_WITH_RED_BALL:			SummonID = __SPELL_GREATBOLT_BLACK;  break;
            case __SPELL_GREATBOLT_AZURE_WITH_BLUE_BALL:			SummonID = __SPELL_GREATBOLT_AZURE;  break;
            case __SPELL_GREATBOLT_CRIMSON_WITH_BLUE_BALL:		SummonID = __SPELL_GREATBOLT_CRIMSON;  break;
            case __SPELL_GREATBOLT_LIME_WITH_BLUE_BALL:			SummonID = __SPELL_GREATBOLT_LIME;  break;
            case __SPELL_GREATBOLT_EMERALD_WITH_BLUE_BALL:		SummonID = __SPELL_GREATBOLT_EMERALD;  break;
            case __SPELL_GREATBOLT_GREEN_WITH_BLUE_BALL:		SummonID = __SPELL_GREATBOLT_GREEN;  break;
            case __SPELL_GREATBOLT_CANDY_WITH_BLUE_BALL:		SummonID = __SPELL_GREATBOLT_CANDY;  break;
            case __SPELL_GREATBOLT_RED_WITH_BLUE_BALL:			SummonID = __SPELL_GREATBOLT_RED;  break;
            case __SPELL_GREATBOLT_BLACK_WITH_BLUE_BALL:		SummonID = __SPELL_GREATBOLT_BLACK;  break;
            case __SPELL_GREATBOLT_AZURE_WITH_YELLOW_BALL:		SummonID = __SPELL_GREATBOLT_AZURE;  break;
            case __SPELL_GREATBOLT_CRIMSON_WITH_YELLOW_BALL:	SummonID = __SPELL_GREATBOLT_CRIMSON;  break;
            case __SPELL_GREATBOLT_LIME_WITH_YELLOW_BALL:		SummonID = __SPELL_GREATBOLT_LIME;  break;
            case __SPELL_GREATBOLT_EMERALD_WITH_YELLOW_BALL:	SummonID = __SPELL_GREATBOLT_EMERALD;  break;
            case __SPELL_GREATBOLT_GREEN_WITH_YELLOW_BALL:		SummonID = __SPELL_GREATBOLT_GREEN;  break;
            case __SPELL_GREATBOLT_CANDY_WITH_YELLOW_BALL:		SummonID = __SPELL_GREATBOLT_CANDY;  break;
            case __SPELL_GREATBOLT_RED_WITH_YELLOW_BALL:		SummonID = __SPELL_GREATBOLT_RED;  break;
            case __SPELL_GREATBOLT_BLACK_WITH_YELLOW_BALL:		SummonID = __SPELL_GREATBOLT_BLACK;  break;
            case __SPELL_GREATBOLT_AZURE_WITH_GREEN_BALL:		SummonID = __SPELL_GREATBOLT_AZURE;  break;
            case __SPELL_GREATBOLT_CRIMSON_WITH_GREEN_BALL:		SummonID = __SPELL_GREATBOLT_CRIMSON;  break;
            case __SPELL_GREATBOLT_LIME_WITH_GREEN_BALL:		SummonID = __SPELL_GREATBOLT_LIME;  break;
            case __SPELL_GREATBOLT_EMERALD_WITH_GREEN_BALL:		SummonID = __SPELL_GREATBOLT_EMERALD;  break;
            case __SPELL_GREATBOLT_GREEN_WITH_GREEN_BALL:		SummonID = __SPELL_GREATBOLT_GREEN;  break;
            case __SPELL_GREATBOLT_CANDY_WITH_GREEN_BALL:		SummonID = __SPELL_GREATBOLT_CANDY;  break;
            case __SPELL_GREATBOLT_RED_WITH_GREEN_BALL:			SummonID = __SPELL_GREATBOLT_RED;  break;
            case __SPELL_GREATBOLT_BLACK_WITH_GREEN_BALL:		SummonID = __SPELL_GREATBOLT_BLACK;  break;
            case __SPELL_GREATBOLT_AZURE_WITH_WHITE_BALL:		SummonID = __SPELL_GREATBOLT_AZURE;  break;
            case __SPELL_GREATBOLT_CRIMSON_WITH_WHITE_BALL:		SummonID = __SPELL_GREATBOLT_CRIMSON;  break;
            case __SPELL_GREATBOLT_LIME_WITH_WHITE_BALL:		SummonID = __SPELL_GREATBOLT_LIME;  break;
            case __SPELL_GREATBOLT_EMERALD_WITH_WHITE_BALL:		SummonID = __SPELL_GREATBOLT_EMERALD;  break;
            case __SPELL_GREATBOLT_GREEN_WITH_WHITE_BALL:		SummonID = __SPELL_GREATBOLT_GREEN;  break;
            case __SPELL_GREATBOLT_CANDY_WITH_WHITE_BALL:		SummonID = __SPELL_GREATBOLT_CANDY;  break;
            case __SPELL_GREATBOLT_RED_WITH_WHITE_BALL:			SummonID = __SPELL_GREATBOLT_RED;  break;
            case __SPELL_GREATBOLT_BLACK_WITH_WHITE_BALL:		SummonID = __SPELL_GREATBOLT_BLACK;  break;
            case __SPELL_GREATBOLT_AZURE_WITH_BLACK_BALL:		SummonID = __SPELL_GREATBOLT_AZURE;  break;
            case __SPELL_GREATBOLT_CRIMSON_WITH_BLACK_BALL:		SummonID = __SPELL_GREATBOLT_CRIMSON;  break;
            case __SPELL_GREATBOLT_LIME_WITH_BLACK_BALL:		SummonID = __SPELL_GREATBOLT_LIME;  break;
            case __SPELL_GREATBOLT_EMERALD_WITH_BLACK_BALL:		SummonID = __SPELL_GREATBOLT_EMERALD;  break;
            case __SPELL_GREATBOLT_GREEN_WITH_BLACK_BALL:		SummonID = __SPELL_GREATBOLT_GREEN;  break;
            case __SPELL_GREATBOLT_CANDY_WITH_BLACK_BALL:		SummonID = __SPELL_GREATBOLT_CANDY;  break;
            case __SPELL_GREATBOLT_RED_WITH_BLACK_BALL:			SummonID = __SPELL_GREATBOLT_RED;  break;
            case __SPELL_GREATBOLT_BLACK_WITH_BLACK_BALL:		SummonID = __SPELL_GREATBOLT_BLACK;  break;
            case __SPELL_GREATBOLT_AZURE_WITH_PURPLE_BALL:		SummonID = __SPELL_GREATBOLT_AZURE;  break;
            case __SPELL_GREATBOLT_CRIMSON_WITH_PURPLE_BALL:	SummonID = __SPELL_GREATBOLT_CRIMSON;  break;
            case __SPELL_GREATBOLT_LIME_WITH_PURPLE_BALL:		SummonID = __SPELL_GREATBOLT_LIME;  break;
            case __SPELL_GREATBOLT_EMERALD_WITH_PURPLE_BALL:	SummonID = __SPELL_GREATBOLT_EMERALD;  break;
            case __SPELL_GREATBOLT_GREEN_WITH_PURPLE_BALL:		SummonID = __SPELL_GREATBOLT_GREEN;  break;
            case __SPELL_GREATBOLT_CANDY_WITH_PURPLE_BALL:		SummonID = __SPELL_GREATBOLT_CANDY;  break;
            case __SPELL_GREATBOLT_RED_WITH_PURPLE_BALL:		SummonID = __SPELL_GREATBOLT_RED;  break;
            case __SPELL_GREATBOLT_BLACK_WITH_PURPLE_BALL:		SummonID = __SPELL_GREATBOLT_BLACK;  break;
               // END DIALSOFT NEW SPELLS
               
            case __SPELL_GREATEXPLOSION_WITH_RED_BALL:		SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_GREATEXPLOSION_WITH_BLACK_BALL:		SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_GREATEXPLOSION_WITH_PURPLE_BALL:	SummonID = __SPELL_GREATEXPLOSION;  break;
            case __SPELL_GREENWIPE_WITH_BLUE_BALL:				SummonID = __SPELL_GREENWIPE;  break;
            case __SPELL_GREENWIPE_WITH_GREEN_BALL:			SummonID = __SPELL_GREENWIPE;  break;
            case __SPELL_GREENWIPE_WITH_BLACK_BALL:			SummonID = __SPELL_GREENWIPE;  break;
            case __SPELL_GREENWIPE_WITH_PURPLE_BALL:			SummonID = __SPELL_GREENWIPE;  break;
            case __SPELL_HEALING_WITH_BLUE_BALL:				SummonID = __SPELL_HEALING;  break;
            case __SPELL_HEALING_WITH_WHITE_BALL:				SummonID = __SPELL_HEALING;  break;
            case __SPELL_HEALSERIOUS_WITH_BLUE_BALL:			SummonID = __SPELL_HEALSERIOUS;  break;
            case __SPELL_HEALSERIOUS_WITH_WHITE_BALL:			SummonID = __SPELL_HEALSERIOUS;  break;
            case __SPELL_ICECLOUD_WITH_BLUE_BALL:				SummonID = __SPELL_ICECLOUD;  break;
            case __SPELL_ICECLOUD_WITH_YELLOW_BALL:			SummonID = __SPELL_ICECLOUD;  break;
            case __SPELL_ICECLOUD_WITH_WHITE_BALL:				SummonID = __SPELL_ICECLOUD;  break;
            case __SPELL_METEOR_WITH_RED_BALL:					SummonID = __SPELL_METEORS;  break;
            case __SPELL_METEOR_WITH_BLACK_BALL:				SummonID = __SPELL_METEORS;  break;
            case __SPELL_METEOR_WITH_PURPLE_BALL:				SummonID = __SPELL_METEORS;  break;
               // BEGIN DIALSOFT NEW SPELLS
            case __SPELL_METEOR_BLACK_WITH_RED_BALL:			SummonID = __SPELL_METEOR_BLACK;  break;
            case __SPELL_METEOR_WHITE_WITH_RED_BALL:			SummonID = __SPELL_METEOR_WHITE;  break;
            case __SPELL_METEOR_PURPLE_WITH_RED_BALL:			SummonID = __SPELL_METEOR_PURPLE;  break;
            case __SPELL_METEOR_COBOLT_WITH_RED_BALL:			SummonID = __SPELL_METEOR_COBOLT;  break;
            case __SPELL_METEOR_GREEN_WITH_RED_BALL:			SummonID = __SPELL_METEOR_GREEN;  break;
            case __SPELL_METEOR_CANDY_WITH_RED_BALL:			SummonID = __SPELL_METEOR_CANDY;  break;
            case __SPELL_METEOR_GOLD_WITH_RED_BALL:				SummonID = __SPELL_METEOR_GOLD;  break;
            case __SPELL_METEOR_BROWN_WITH_RED_BALL:			SummonID = __SPELL_METEOR_BROWN;  break;
            case __SPELL_METEOR_DARK_WITH_RED_BALL:				SummonID = __SPELL_METEOR_DARK;  break;
            case __SPELL_METEOR_BLUE_WITH_RED_BALL:				SummonID = __SPELL_METEOR_BLUE;  break;
            case __SPELL_METEOR_VIOLET_WITH_RED_BALL:			SummonID = __SPELL_METEOR_VIOLET;  break;
            case __SPELL_METEOR_WITH_BLUE_BALL:					SummonID = __SPELL_METEORS;  break;
            case __SPELL_METEOR_BLACK_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_BLACK;  break;
            case __SPELL_METEOR_WHITE_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_WHITE;  break;
            case __SPELL_METEOR_PURPLE_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_PURPLE;  break;
            case __SPELL_METEOR_COBOLT_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_COBOLT;  break;
            case __SPELL_METEOR_GREEN_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_GREEN;  break;
            case __SPELL_METEOR_CANDY_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_CANDY;  break;
            case __SPELL_METEOR_GOLD_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_GOLD;  break;
            case __SPELL_METEOR_BROWN_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_BROWN;  break;
            case __SPELL_METEOR_DARK_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_DARK;  break;
            case __SPELL_METEOR_BLUE_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_BLUE;  break;
            case __SPELL_METEOR_VIOLET_WITH_BLUE_BALL:			SummonID = __SPELL_METEOR_VIOLET;  break;
            case __SPELL_METEOR_WITH_YELLOW_BALL:				SummonID = __SPELL_METEORS;  break;
            case __SPELL_METEOR_BLACK_WITH_YELLOW_BALL:			SummonID = __SPELL_METEOR_BLACK;  break;
            case __SPELL_METEOR_WHITE_WITH_YELLOW_BALL:			SummonID = __SPELL_METEOR_WHITE;  break;
            case __SPELL_METEOR_PURPLE_WITH_YELLOW_BALL:		SummonID = __SPELL_METEOR_PURPLE;  break;
            case __SPELL_METEOR_COBOLT_WITH_YELLOW_BALL:		SummonID = __SPELL_METEOR_COBOLT;  break;
            case __SPELL_METEOR_GREEN_WITH_YELLOW_BALL:			SummonID = __SPELL_METEOR_GREEN;  break;
            case __SPELL_METEOR_CANDY_WITH_YELLOW_BALL:			SummonID = __SPELL_METEOR_CANDY;  break;
            case __SPELL_METEOR_GOLD_WITH_YELLOW_BALL:			SummonID = __SPELL_METEOR_GOLD;  break;
            case __SPELL_METEOR_BROWN_WITH_YELLOW_BALL:			SummonID = __SPELL_METEOR_BROWN;  break;
            case __SPELL_METEOR_DARK_WITH_YELLOW_BALL:			SummonID = __SPELL_METEOR_DARK;  break;
            case __SPELL_METEOR_BLUE_WITH_YELLOW_BALL:			SummonID = __SPELL_METEOR_BLUE;  break;
            case __SPELL_METEOR_VIOLET_WITH_YELLOW_BALL:		SummonID = __SPELL_METEOR_VIOLET;  break;
            case __SPELL_METEOR_WITH_GREEN_BALL:				SummonID = __SPELL_METEORS;  break;
            case __SPELL_METEOR_BLACK_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_BLACK;  break;
            case __SPELL_METEOR_WHITE_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_WHITE;  break;
            case __SPELL_METEOR_PURPLE_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_PURPLE;  break;
            case __SPELL_METEOR_COBOLT_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_COBOLT;  break;
            case __SPELL_METEOR_GREEN_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_GREEN;  break;
            case __SPELL_METEOR_CANDY_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_CANDY;  break;
            case __SPELL_METEOR_GOLD_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_GOLD;  break;
            case __SPELL_METEOR_BROWN_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_BROWN;  break;
            case __SPELL_METEOR_DARK_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_DARK;  break;
            case __SPELL_METEOR_BLUE_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_BLUE;  break;
            case __SPELL_METEOR_VIOLET_WITH_GREEN_BALL:			SummonID = __SPELL_METEOR_VIOLET;  break;
            case __SPELL_METEOR_WITH_WHITE_BALL:				SummonID = __SPELL_METEORS;  break;
            case __SPELL_METEOR_BLACK_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_BLACK;  break;
            case __SPELL_METEOR_WHITE_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_WHITE;  break;
            case __SPELL_METEOR_PURPLE_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_PURPLE;  break;
            case __SPELL_METEOR_COBOLT_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_COBOLT;  break;
            case __SPELL_METEOR_GREEN_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_GREEN;  break;
            case __SPELL_METEOR_CANDY_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_CANDY;  break;
            case __SPELL_METEOR_GOLD_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_GOLD;  break;
            case __SPELL_METEOR_BROWN_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_BROWN;  break;
            case __SPELL_METEOR_DARK_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_DARK;  break;
            case __SPELL_METEOR_BLUE_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_BLUE;  break;
            case __SPELL_METEOR_VIOLET_WITH_WHITE_BALL:			SummonID = __SPELL_METEOR_VIOLET;  break;
            case __SPELL_METEOR_BLACK_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_BLACK;  break;
            case __SPELL_METEOR_WHITE_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_WHITE;  break;
            case __SPELL_METEOR_PURPLE_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_PURPLE;  break;
            case __SPELL_METEOR_COBOLT_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_COBOLT;  break;
            case __SPELL_METEOR_GREEN_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_GREEN;  break;
            case __SPELL_METEOR_CANDY_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_CANDY;  break;
            case __SPELL_METEOR_GOLD_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_GOLD;  break;
            case __SPELL_METEOR_BROWN_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_BROWN;  break;
            case __SPELL_METEOR_DARK_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_DARK;  break;
            case __SPELL_METEOR_BLUE_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_BLUE;  break;
            case __SPELL_METEOR_VIOLET_WITH_BLACK_BALL:			SummonID = __SPELL_METEOR_VIOLET;  break;
            case __SPELL_METEOR_BLACK_WITH_PURPLE_BALL:			SummonID = __SPELL_METEOR_BLACK;  break;
            case __SPELL_METEOR_WHITE_WITH_PURPLE_BALL:			SummonID = __SPELL_METEOR_WHITE;  break;
            case __SPELL_METEOR_PURPLE_WITH_PURPLE_BALL:		SummonID = __SPELL_METEOR_PURPLE;  break;
            case __SPELL_METEOR_COBOLT_WITH_PURPLE_BALL:		SummonID = __SPELL_METEOR_COBOLT;  break;
            case __SPELL_METEOR_GREEN_WITH_PURPLE_BALL:			SummonID = __SPELL_METEOR_GREEN;  break;
            case __SPELL_METEOR_CANDY_WITH_PURPLE_BALL:			SummonID = __SPELL_METEOR_CANDY;  break;
            case __SPELL_METEOR_GOLD_WITH_PURPLE_BALL:			SummonID = __SPELL_METEOR_GOLD;  break;
            case __SPELL_METEOR_BROWN_WITH_PURPLE_BALL:			SummonID = __SPELL_METEOR_BROWN;  break;
            case __SPELL_METEOR_DARK_WITH_PURPLE_BALL:			SummonID = __SPELL_METEOR_DARK;  break;
            case __SPELL_METEOR_BLUE_WITH_PURPLE_BALL:			SummonID = __SPELL_METEOR_BLUE;  break;
            case __SPELL_METEOR_VIOLET_WITH_PURPLE_BALL:		SummonID = __SPELL_METEOR_VIOLET;  break;
               // END DIALSOFT NEW SPELLS

            case __SPELL_PENTACLE_WITH_RED_BALL:				SummonID = __SPELL_PENTACLE;  break;
            case __SPELL_PENTACLE_WITH_GREEN_BALL:				SummonID = __SPELL_PENTACLE;  break;
            case __SPELL_PENTACLE_WITH_PURPLE_BALL:			SummonID = __SPELL_PENTACLE;  break;
            case __SPELL_POISONCLOUD_WITH_BLUE_BALL:			SummonID = __SPELL_POISONCLOUD;  break;
            case __SPELL_POISONCLOUD_WITH_GREEN_BALL:			SummonID = __SPELL_POISONCLOUD;  break;
            case __SPELL_POISONCLOUD_WITH_PURPLE_BALL:		SummonID = __SPELL_POISONCLOUD;  break;
            case __SPELL_REDWIPE_WITH_RED_BALL:					SummonID = __SPELL_REDWIPE;  break;
            case __SPELL_REDWIPE_WITH_BLACK_BALL:				SummonID = __SPELL_REDWIPE;  break;
            case __SPELL_ROCKFLY_WITH_GREEN_BALL:				SummonID = __SPELL_ROCKFLY;  break;
            case __SPELL_SEMIBIGEXPLOSION_WITH_RED_BALL:		SummonID = __SPELL_SEMIBIGEXPLOSION;  break;
            case __SPELL_SEMIBIGEXPLOSION_WITH_BLACK_BALL:	SummonID = __SPELL_SEMIBIGEXPLOSION;  break;
            case __SPELL_SEMIBIGEXPLOSION_WITH_PURPLE_BALL: SummonID = __SPELL_SEMIBIGEXPLOSION;  break;
            case __SPELL_SMALLEXPLOSION_WITH_RED_BALL:		SummonID = __SPELL_SMALLEXPLOSION;  break;
            case __SPELL_SMALLEXPLOSION_WITH_BLACK_BALL:		SummonID = __SPELL_SMALLEXPLOSION;  break;
            case __SPELL_SMALLEXPLOSION_WITH_PURPLE_BALL:	SummonID = __SPELL_SMALLEXPLOSION;  break;
            case __SPELL_FIRECIRCLE_WITH_RED_BALL:	         SummonID = __SPELL_FIRECIRCLE;  break;
            case __SPELL_FIRECIRCLE_WITH_BLACK_BALL:        SummonID = __SPELL_FIRECIRCLE;  break;
            case __SPELL_FIRECIRCLE_WITH_PURPLE_BALL:       SummonID = __SPELL_FIRECIRCLE;  break;


            case __SPELL_NEW_NMS_BALL_PARTICLE_BLEU_WITH_BALL  :  SummonID = __SPELL_NEW_NMS_BALL_PARTICLE_BLEU  ;  break;
            case __SPELL_NEW_NMS_BALL_PARTICLE_RED_WITH_BALL   :  SummonID = __SPELL_NEW_NMS_BALL_PARTICLE_RED   ;  break;
            case __SPELL_NEW_NMS_BALL_PARTICLE_FIRE_WITH_BALL  :  SummonID = __SPELL_NEW_NMS_BALL_PARTICLE_FIRE  ;  break;
            case __SPELL_NEW_NMS_BALL_PARTICLE_GREEN_WITH_BALL :  SummonID = __SPELL_NEW_NMS_BALL_PARTICLE_GREEN ;  break;
            case __SPELL_NEW_NMS_BALL_PARTICLE_YELLOW_WITH_BALL:  SummonID = __SPELL_NEW_NMS_BALL_PARTICLE_YELLOW;  break;
            case __SPELL_NEW_NMS_BALL_PARTICLE_PURPLE_WITH_BALL:  SummonID = __SPELL_NEW_NMS_BALL_PARTICLE_PURPLE;  break;
            case __SPELL_NEW_NMS_BALL_PARTICLE_PINK_WITH_BALL  :  SummonID = __SPELL_NEW_NMS_BALL_PARTICLE_PINK  ;  break;
            case __SPELL_NEW_NMS_BALL_PARTICLE_WHITE_WITH_BALL :  SummonID = __SPELL_NEW_NMS_BALL_PARTICLE_WHITE ;  break;
            case __SPELL_NEW_NMS_BALL_BLEU_WITH_BALL           :  SummonID = __SPELL_NEW_NMS_BALL_BLEU           ;  break;
            case __SPELL_NEW_NMS_BALL_RED_WITH_BALL            :  SummonID = __SPELL_NEW_NMS_BALL_RED            ;  break;
            case __SPELL_NEW_NMS_BALL_FIRE_WITH_BALL           :  SummonID = __SPELL_NEW_NMS_BALL_FIRE           ;  break;
            case __SPELL_NEW_NMS_BALL_GREEN_WITH_BALL          :  SummonID = __SPELL_NEW_NMS_BALL_GREEN          ;  break;
            case __SPELL_NEW_NMS_BALL_YELLOW_WITH_BALL         :  SummonID = __SPELL_NEW_NMS_BALL_YELLOW         ;  break;
            case __SPELL_NEW_NMS_BALL_PURPLE_WITH_BALL         :  SummonID = __SPELL_NEW_NMS_BALL_PURPLE         ;  break;
            case __SPELL_NEW_NMS_BALL_PINK_WITH_BALL           :  SummonID = __SPELL_NEW_NMS_BALL_PINK           ;  break;
            case __SPELL_NEW_NMS_BALL_WHITE_WITH_BALL          :  SummonID = __SPELL_NEW_NMS_BALL_WHITE          ;  break;
            case __SPELL_NEW_FIRE1_WITH_BALL                   :  SummonID = __SPELL_NEW_FIRE1                   ;  break;
            case __SPELL_NEW_IMMOBILISATION_WITH_BALL          :  SummonID = __SPELL_NEW_IMMOBILISATION          ;  break;
            case __SPELL_NEW_IMMOBILISATION_ROUGE_WITH_BALL    :  SummonID = __SPELL_NEW_IMMOBILISATION_ROUGE    ;  break;
            case __SPELL_NEW_IMMOBILISATION_VERT_WITH_BALL     :  SummonID = __SPELL_NEW_IMMOBILISATION_VERT     ;  break;
            case __SPELL_NEW_IMMOBILISATION_BLEU_WITH_BALL     :  SummonID = __SPELL_NEW_IMMOBILISATION_BLEU     ;  break;
            case __SPELL_NEW_IMMOBILISATION_ORANGE_WITH_BALL   :  SummonID = __SPELL_NEW_IMMOBILISATION_ORANGE   ;  break;
            case __SPELL_NEW_IMMOBILISATION_JAUNE_WITH_BALL    :  SummonID = __SPELL_NEW_IMMOBILISATION_JAUNE    ;  break;
            case __SPELL_NEW_IMMOBILISATION_VIOLET_WITH_BALL   :  SummonID = __SPELL_NEW_IMMOBILISATION_VIOLET   ;  break;
            case __SPELL_NEW_NMS_JETPIERRE_WITH_BALL           :  SummonID = __SPELL_NEW_NMS_JETPIERRE           ;  break;
            case __SPELL_NEW_NMS_LIGHT01ALPHA_WITH_BALL        :  SummonID = __SPELL_NEW_NMS_LIGHT01ALPHA        ;  break;
            case __SPELL_NEW_LIGHTNING1_WITH_BALL              :  SummonID = __SPELL_NEW_LIGHTNING1              ;  break;
            case __SPELL_NEW_POISON1_WITH_BALL                 :  SummonID = __SPELL_NEW_POISON1                 ;  break;
            case __SPELL_NEW_NMS_ROCKE01ALPHA_WITH_BALL        :  SummonID = __SPELL_NEW_NMS_ROCKE01ALPHA        ;  break;
            case __SPELL_NEW_NMS_ROCKS01ALPHA_WITH_BALL        :  SummonID = __SPELL_NEW_NMS_ROCKS01ALPHA        ;  break;
            case __SPELL_NEW_NMS_ROCKF01ALPHA_WITH_BALL        :  SummonID = __SPELL_NEW_NMS_ROCKF01ALPHA        ;  break;
            case __SPELL_NEW_SHOUT01_WITH_BALL                 :  SummonID = __SPELL_NEW_SHOUT01                 ;  break;
            case __SPELL_NEW_SHOUT02_WITH_BALL                 :  SummonID = __SPELL_NEW_SHOUT02                 ;  break;
            case __SPELL_NEW_SHOUT02_VERT_WITH_BALL            :  SummonID = __SPELL_NEW_SHOUT02_VERT            ;  break;
            case __SPELL_NEW_SHOUT02_BLEU_WITH_BALL            :  SummonID = __SPELL_NEW_SHOUT02_BLEU            ;  break;
            case __SPELL_NEW_SHOUT02_ORANGE_WITH_BALL          :  SummonID = __SPELL_NEW_SHOUT02_ORANGE          ;  break;
            case __SPELL_NEW_SHOUT02_JAUNE_WITH_BALL           :  SummonID = __SPELL_NEW_SHOUT02_JAUNE           ;  break;
            case __SPELL_NEW_SHOUT02_VIOLET_WITH_BALL          :  SummonID = __SPELL_NEW_SHOUT02_VIOLET          ;  break;
            case __SPELL_NEW_NMS_SUPRAHEAL1_WITH_BALL          :  SummonID = __SPELL_NEW_NMS_SUPRAHEAL1          ;  break;
            case __SPELL_NEW_NMS_SUPRAHEAL2_WITH_BALL          :  SummonID = __SPELL_NEW_NMS_SUPRAHEAL2          ;  break;
            case __SPELL_NEW_NMS_SUPRAHEAL3_WITH_BALL          :  SummonID = __SPELL_NEW_NMS_SUPRAHEAL3          ;  break;
            case __SPELL_NEW_NMS_SUPRAHEAL4_WITH_BALL          :  SummonID = __SPELL_NEW_NMS_SUPRAHEAL4          ;  break;
            case __SPELL_NEW_VERGLA_WITH_BALL                  :  SummonID = __SPELL_NEW_VERGLA                  ;  break;


















            };
            Objects.MoveObject(SpellID, TPOSX, TPOSY, TRUE, SummonID, 100,-1,-1, SPELLID, 0, 100,false,false,0);
         }
         SpellID--;
      }
      Objects.Unlock(1000);
      //World.Unlock(54);
      
      if (EvilSpell && CASTER != Player.ID && TARGET == Player.ID) 
      {
         Objects.Lock(0);
         Objects.SetEvil(CASTER);
         Objects.Unlock(0);
      }
      
   } break;
   //////////////////////////////////////////////////////////////////////////////////////////
   // Arrow hit
    case RQ_ArrowHit:{
       TFCPacket *msg = Msg;
       
       DWORD CASTER = 0;
       DWORD TARGET = 0;
       char  newHp = 0;
       
       GET_LONG( CASTER );
       GET_LONG( TARGET );
       GET_BYTE( newHp );
       
       ShootArrow( CASTER, TARGET, 0, 0, true, newHp );
                     } break;
       //////////////////////////////////////////////////////////////////////////////////////////   
       // Arrow misses.
    case RQ_ArrowMiss:{
       TFCPacket *msg = Msg;
       
       DWORD CASTER;
       SHORT TPOSX, TPOSY;
       BYTE COLLIDE;
       bool collide;
       
       GET_LONG( CASTER );
       GET_WORD( TPOSX );
       GET_WORD( TPOSY );
       GET_BYTE( COLLIDE );
       collide = COLLIDE != 0 ? true : false;
       
       ShootArrow( CASTER, 0, TPOSX, TPOSY, collide, 0 );        
       
                      }; break;
       
    case RQ_TeleportPlayer: PacketHandling::TeleportPlayer(Msg); break;
       
    case RQ_GetNearItems:   
       {
          if (Code13) {
             Objects.Lock(17);
             Objects.SetSet(TRUE);
             Objects.AddSDObject();
             Objects.AddSDAnimObject();
             Objects.Unlock(17); 
             
             Objects.Lock(19);
             Objects.Direction2 = 1;
             Objects.GetMainObject()->SpriteNumber = 1;
             Objects.Unlock(19);
             
             g_GameMusic.LoadNewSound("RQ_GetNearItems");
             
             g_Var.inGame = false;
             
             Objects.Lock(13);
             Objects.Direction2 = 1;
             Objects.GetMainObject()->SpriteNumber = 1;
             Objects.Unlock(13);
             
             INGAME = TRUE;
             if (!hDrawThread) {
                DXDClear();
                DXDFlip();//avant le draw thread clear 1-2
                DXDClear();
                DXDFlip();//avant le draw thread clear 2-2
                hDrawThread = (HANDLE)_beginthreadex(NULL, 0, DrawThread, NULL, CREATE_SUSPENDED, &iDrawThreadID);
             }
             if( !hMouseActionThread ){
                hMouseActionThread = (HANDLE)_beginthreadex(NULL, 0, MouseActionThread, NULL, 0, &iMouseActionThreadID);
             }
             
             
             stShow = true; 

            
             ResumeThread(hDrawThread);     
             
            
             
             World.Lock(20); //NM:getNearItem SWITCH
             World.SetFading = true;
             World.FadeLevel = 0;
             World.Switch();
             World.Unlock(20);
             
             
             Code13 = false;
             noException = false;

          }
          
          if (EnterGame) {
             TFCPacket Send;
             
             Send << (short)46;
             
             SEND_PACKET(Send);
          }
       } break;
       
    case RQ_Remort: 
       {
          V3_MacroDlg::GetInstance()->ResetMacro();
       } 
       break;
       
    case RQ_GetStatus: { 
       
       
       DWORD High;
       DWORD Low;
       
       // Get the status information.
       Msg->Get((long *)&Player.Hp);
       Msg->Get((long *)&Player.MaxHp);
       Msg->Get((short *)&Player.Mana);
       Msg->Get((short *)&Player.MaxMana);
       Msg->Get((long *)&High);
       Msg->Get((long *)&Low);
       Player.Exp = ((__int64)(High) << 32) + (__int64)(Low);
       Msg->Get((short *)&Player.bAC );
       Msg->Get((short *)&Player.AC );
       Msg->Get((short *)&Player.bStr);
       Msg->Get((short *)&Player.bEnd);
       Msg->Get((short *)&Player.bAgi);
       Msg->Get((short *)&Player.bWil);
       Msg->Get((short *)&Player.bWis);
       Msg->Get((short *)&Player.bInt);
       Msg->Get((short *)&Player.bLck);
       Msg->Get((short *)&Player.StatsPts);
       Msg->Get((short *)&Player.Str);
       Msg->Get((short *)&Player.End);
       Msg->Get((short *)&Player.Agi);
       Msg->Get((short *)&Player.Wil);
       Msg->Get((short *)&Player.Wis);
       Msg->Get((short *)&Player.Int);
       Msg->Get((short *)&Player.Lck);
       Msg->Get((short *)&Player.Level);
       Msg->Get((short *)&Player.SkillPts );
       Msg->Get((short *)&Player.Weight );
       Msg->Get((short *)&Player.MaxWeight );
       Msg->Get((short *)&Player.Karma );
       Msg->Get((short *)&Player.TrueMaxHp );
       Msg->Get((short *)&Player.TrueMaxMana );
       Msg->Get((short *)&Player.Crime );
       Msg->Get((short *)&Player.Honor );
       Msg->Get((long *)&Player.InteractionPts);
       Msg->Get((long *)&Player.InteractionPtsT);
       
       
       /*StatsBox *lpStatsBox = static_cast< StatsBox *>( GraphBoxHandler::GetBox( GWIDBOX_STATS ) );
       // If no stat box is currently displayed
       if( lpStatsBox == NULL ){
       // Create and register a new one
       lpStatsBox = new StatsBox( 0 );						
       GraphBoxHandler::RegisterBox( lpStatsBox );
       lpStatsBox->Lock();
       }
       
         lpStatsBox->AddPacketStats( Msg );
       lpStatsBox->Unlock();*/
       
       // Update the character sheet.
       if(V3_StatsDlg::GetInstance()->IsShown())
         V3_StatsDlg::GetInstance()->UpdateCharacterSheet("OK-RQ_GetStatus");
       if(V3_InvDlg::GetInstance()->IsShown())
         V3_InvDlg::GetInstance()->UpdateCharacterSheet();
       
    } break;

    case RQ_GetStatus2: 
    { 
       Msg->Get((short *)&Player.iPAir );
       Msg->Get((short *)&Player.iPFeu);
       Msg->Get((short *)&Player.iPEau);
       Msg->Get((short *)&Player.iPTerre);
       Msg->Get((short *)&Player.iPNoir);
       Msg->Get((short *)&Player.iPLumiere);
       Msg->Get((short *)&Player.iPBAir);
       Msg->Get((short *)&Player.iPBFeu);
       Msg->Get((short *)&Player.iPBEau);
       Msg->Get((short *)&Player.iPBTerre);
       Msg->Get((short *)&Player.iPBNoir);
       Msg->Get((short *)&Player.iPBLumiere);

       Msg->Get((short *)&Player.iRAir );
       Msg->Get((short *)&Player.iRFeu);
       Msg->Get((short *)&Player.iREau);
       Msg->Get((short *)&Player.iRTerre);
       Msg->Get((short *)&Player.iRNoir);
       Msg->Get((short *)&Player.iRLumiere);
       Msg->Get((short *)&Player.iRBAir);
       Msg->Get((short *)&Player.iRBFeu);
       Msg->Get((short *)&Player.iRBEau);
       Msg->Get((short *)&Player.iRBTerre);
       Msg->Get((short *)&Player.iRBNoir);
       Msg->Get((short *)&Player.iRBLumiere);
       Msg->Get((short *)&Player.Sexx);
      
           
       // Update the character sheet.
       //V3_StatsDlg::GetInstance()->UpdateCharacterSheet_dis("RQ_GetStatus2");
    } break;
       
       
    case RQ_GoldChange: 
    {
       long Gold;
       Msg->Get((long *)&Gold);
       Player.Gold = Gold;
       V3_InvDlg::GetInstance()->UpdateGold();
    } break;

    case RQ_AskCompagnonName: 
    {
       //demande le nom du compagnon
      V3_MainBarDlg::GetInstance()->AskCompagnonName();
    } break;

   case RQ_NameChange: 
   {
       int iDx = 0;
       int i=0;
       for(i=0;i<Custom.m_shNumberOfPlayer;i++)
       {
         if(strcmp(Custom.m_strName[i],Player.Name)==0)
            iDx = i;
       }
       
       int Len = 0;
       Msg->Get((short *)&Len);
       for (i = 0; i < Len; i++)
          Msg->Get((char *)&Player.Name[i]);
       
       Player.Name[Len] = 0;

       g_SaveGame.bSetFileName(Player.Account,Player.Name); //NMNMNM Change le nom du save quand le nom change...

       sprintf_s(Custom.m_strName[iDx],100,"%s",Player.Name);

    } break;
       
       
    case RQ_Page: 
    {
       BOOL IgnoreUser = FALSE;
       int iPageType;
       char szText[1024];
       char szName[1024];
       DWORD dwColor; 
       
       iPageType = 1;
       
       bool confirm = false;
       short Len;
       memset(szText, 0, 1024);
       
       Msg->Get((short *)&Len);
       for (int i = 0; i < Len; i++)
          Msg->Get((char *)&szName[i]);
       
       szName[Len] = 0;
       
       if (Len == 0) {
          iPageType = 2;
       }
       
       Msg->Get((short *)&Len);           
       for (int j = 0; j < Len; j++) 
          Msg->Get((char *)&szText[j]);
       
       szText[Len] = 0;
       
       if (!szName[0] && Len) 
       {
		  //if( strstr( szText, g_LocalString[ 727 ] ) != 0 )
		  {
			  // Mean that is a page confirmation
			  sprintf_s(szName,1024, g_LocalString[89], szText);
			  int iColor = CL_YELLOW_LIGHT;
			  if(!V3_ChatDlg::GetInstance()->IsUserIgnored( szText ))  //receive page from ignored
			  {
   			  SoundFX[44].Play(FALSE);
	  		     V3_ChatLogDlg::GetInstance()->AddListLogs( szText, szName, iColor, true, false );
			  }
		  } 
       } 
       else if(!V3_ChatDlg::GetInstance()->IsUserIgnored( szName )) //receive page from ignored
       {
          SoundFX[44].Play(FALSE);
          
          dwColor = CL_MAGENTA;
          
          if (Len == 0) 
             iPageType = 3;
          
          char Temp[1024];
          int iColor;
          bool show = true;
          if (iPageType == 1) { // Someone pages you
             sprintf_s(Temp,1024, g_LocalString[88], szName, szText);
             iColor = CL_GRAY_LIGHT;
             ClientConfig *pConfig = g_Global.GetClientConfig();
             if (pConfig->dwAfkStatus)
             {
                // So we won't flood if we are ignored or pages self,
                if( (strcmp( szText, g_LocalString[ 502 ] ) != 0 ) && (_stricmp( szText, pConfig->strAfkMessage ))) 
                {
                   TFCPacket sending;
                   sending << (RQ_SIZE)RQ_Page;
                   sending << (short)strlen( szName );
                   sending << (char *)szName;
                   sending << (short)strlen( pConfig->strAfkMessage );
                   sending << (char *)pConfig->strAfkMessage;
                   
                   SEND_PACKET( sending );
                }
             }
          }
          if (iPageType == 2) {
             sprintf_s(Temp,1024, g_LocalString[89], szText);
             strcpy_s( szName,1024, szText );
             iColor = CL_YELLOW_LIGHT;
             //show = false;
          }
          if (iPageType == 3) { // User not found
             sprintf_s(Temp,1024, g_LocalString[90]);
             iColor = CL_CYAN_LIGHT;
          }
          
          if( show )
          {
             V3_ChatLogDlg::GetInstance()->AddListLogs( szName, Temp, iColor, true, false );
          }
       }
       else
       {
          // If the received text is the ignore text, do not resend
          // another ignoring page (avoid loops).
          if( strcmp( szText, g_LocalString[ 502 ] ) != 0 )
          {
             TFCPacket sending;
             sending << (RQ_SIZE)RQ_Page;
             sending << (short)strlen( szName );
             sending << (char *)szName;
             sending << (short)strlen( g_LocalString[ 502 ] );
             sending << (char *)g_LocalString[ 502 ];
             
             SEND_PACKET( sending ); 
          }
       }
       
                  } break;
                  
    case RQ_Shout: break;
       
    case RQ_FromPreInGameToInGame: 
       PacketHandling::FromPreInGameToInGame(Msg); 
    break;
       
    case 47: {
    
             } break;
       
    case RQ_UpdateFactionID: 
    {
        Msg->Get((short *)&Player.FactionID);
        TFCPacket sending;	
        sending << (RQ_SIZE)RQ_UpdateFactionID;	
        SEND_PACKET( sending );
    }
    break;
    

    case RQ_LevelUp: {
       unsigned long High, Low;
       unsigned long oldLevel = Player.Level;
       
       Msg->Get((short *)&Player.Level);
       Msg->Get((long *)&High);
       Msg->Get((long *)&Low);
       Msg->Get((long *)&Player.Hp);
       Msg->Get((long *)&Player.MaxHp);
       Msg->Get((short *)&Player.Mana);
       Msg->Get((short *)&Player.MaxMana);
       // hp, max hp, mana, max mana
       
       
       if (oldLevel < Player.Level) 
       {
          Player.ExpLastLevel = Player.Exp2Go;
          Player.Exp2Go = ((__int64)(High) << 32) + (__int64)(Low);

          LevelUp = true;
          LastAsk3 = timeGetTime();
          //++NMNMNM --[Add Spell levelUP]
          Objects.Lock(0);
          Objects.Add(SpellID, __SPELL_LEVELUP, Player.xPos, Player.yPos, 100, 0, NULL, 100,-1,-1,2,false,0,0);
          SpellID--;
          Objects.Unlock(0);
       }
       
             } break;
       
    case 44: {
       unsigned long High, Low;
       
       Msg->Get((long *)&High);
       Msg->Get((long *)&Low);
       Player.Exp = ((__int64)(High) << 32) + (__int64)Low;

             } break;
       //////////////////////////////////////////////////////////////////////////////////////////
       // Server messages
    case RQ_ServerMessage:{
       
       // Retreive the width and height of the message box.
       WORD wX = 0, wY = 0;
       WORD wI;
       WORD wLen;
       char *lpbText;
       DWORD color = SYSTEM_COLOR;
       
       
       Msg->Get( (short *)&wX );
       Msg->Get( (short *)&wY );
       Msg->Get( (short *)&wLen );
       
       if( wLen > 0 ){
          lpbText = new char[ wLen + 1 ];
          
          for( wI = 0; wI < wLen; wI++ ){
             Msg->Get( (char *)&lpbText[ wI ] );
          }
          lpbText[ wI ] = 0;
          
          
          Msg->Get((DWORD *)&color);

          V3_ChatLogDlg::GetInstance()->AddListLogs( "", lpbText, color, true );
          delete lpbText;
       }      
       
                          } break;
       
       ////////////////////////
       // NightMare -- Add 2005-03-17 (Update Ophiste code by NightMare)
       // Add Weather packet info and change state of weather...
       // Now Support only Rain
    case RQ_WeatherMsg:
       {
          
          enum { OFF, ON };
          
          short value;
          bool bValue;
		  DWORD dwEffect;
          
		  Msg->Get( (long *)&dwEffect ); 
          Msg->Get( (short *)&value );
          
          switch(value)
          {
          case OFF:
             bValue = false;
             break;
          case ON:
             bValue = true;
             break;
          default:
             bValue = false;
             break;
          }

		  if(dwEffect == 1)
			 Custom.gbEnableGameRain = bValue;
		  else if(dwEffect == 2)
		     Custom.gbEnableGameSnow = bValue;
		  else if(dwEffect == 3)
		     Custom.gbEnableGameFog = bValue;



		  /*
		  m_wRainStatus
          
          if(CWeather::GetInstance()->GetRainState() != (int)bValue) {
             CWeather::GetInstance()->SetRainState((int)bValue);
		  
          }
		  */
       }
       break;
       // END NM
       /////////////////////////
       
       //////////////////////////////////////////////////////////////////////////////////////////
       // Info messages
    case RQ_OpenURL:{
       
       
       char *lpbText;
       WORD wLen;
       Msg->Get( (short *)&wLen );
       
       if( wLen > 0 )
	   {
          lpbText = new char[ wLen + 1 + 7 ];
          strcpy_s(lpbText, wLen + 1 + 7, "http://");
          
          int wI = 0;
          for( wI = 0; wI < wLen; wI++ ){
             Msg->Get( (char *)&lpbText[ wI + 7 ] );
          }
          lpbText[ wI + 7 ] = 0;
          
          ShellExecute(NULL, "open", lpbText, NULL, NULL, SW_MAXIMIZE);
          delete lpbText;
       }
       
                    } break;
    case RQ_InfoMessage:
       {
          
          // Retreive the width and height of the message box.
          WORD wI;
          WORD wLen;
          char *lpbText;
          DWORD type;
          DWORD color;
          
          
          Msg->Get( (long *)&type ); // TODO: implement a way to allow user to disable displaying of some message types
          Msg->Get( (long *)&color );
          Msg->Get( (short *)&wLen );
          
          if( wLen > 0 )
          {
             lpbText = new char[ wLen + 1 ];
             
             for( wI = 0; wI < wLen; wI++ )
             {
                Msg->Get( (char *)&lpbText[ wI ] );
             }
             lpbText[ wI ] = 0;
             
             
             V3_ChatLogDlg::GetInstance()->AddListLogs( "", lpbText, color, true );
             delete lpbText;
          }      
       } 
       break;

       
       //////////////////////////////////////////////////////////////////////////////////////////				
       // Update equipment list.
    case RQ_ViewEquiped: {
       
       char rangedAttack = 0;
       Msg->Get( (char *)&rangedAttack );
       
       if( rangedAttack != 0 )
       {
          Player.rangedAttack = true;
       }
       else
       { 
          Player.rangedAttack = false;
       }
       
       // Get the packet information.
       GetEquipItem( body, Msg );
       GetEquipItem( gloves, Msg );
       GetEquipItem( helm, Msg );
       GetEquipItem( legs, Msg );
       GetEquipItem( bracelets, Msg );
       GetEquipItem( necklace, Msg );
       GetEquipItem( weapon_right, Msg );
       GetEquipItem( weapon_left, Msg );
       GetEquipItem( ring1, Msg );
       GetEquipItem( ring2, Msg );
       GetEquipItem( belt, Msg );
       GetEquipItem( sleeves, Msg );
       GetEquipItem( feet, Msg );
       GetEquipItem( Orbe1, Msg );
       
       // Synchronize the inventory UI.
       V3_InvDlg::GetInstance()->SyncEquipment();
       
       // Lock the root box since this call came from some unknown thread.
       //CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
       
       // Update the old stats box.
       //StatsBox::PacketEquip( Msg );
       
                         } break;
       
       //////////////////////////////////////////////////////////////////////////////////////////						
       // Train skill list
    case RQ_SendTrainSkillList: 
    {
       
       TFCPacket *msg = Msg;
       
       DWORD currentGold = 0;
       WORD nbItems = 0;
       std::list< V3_TrainDlg::SellItemInfo > itemList;
       
       WORD skillPnts;
       
       GET_WORD( skillPnts );
       GET_WORD( nbItems );
       
       // Get the item list from the packet.
       WORD i;
       for( i = 0; i < nbItems; i++ )
       {
          LPBYTE name;                
          V3_TrainDlg::SellItemInfo trainInfo;
          
          BYTE canHave = 0;
          
          GET_BYTE( canHave );	        // TRUE if skill can be trained.
          trainInfo.canHave = canHave != 0 ? true : false;
          GET_WORD( trainInfo.skillID );	// ID of the skill.
          GET_WORD( trainInfo.qty );		// Current skill strength.
          
          GET_WORD( trainInfo.maxQty );	// Maximum points this NPC can train.
          GET_LONG( trainInfo.price );	// Cost of skill training per skill points.
          
          GET_STRING( name );		// Length of skill name
          
          trainInfo.initialQty = trainInfo.qty;
          
          trainInfo.name = reinterpret_cast< const char *>( name );
          itemList.push_back( trainInfo );
          
          delete name;
       }
       V3_TrainDlg::GetInstance()->Train( skillPnts, itemList );
    } break;
       
       //////////////////////////////////////////////////////////////////////////////////////////						
       // Teach skill list
    case RQ_SendTeachSkillList:
    {
       
       TFCPacket *msg = Msg;
       DWORD currentGold = 0;
       WORD nbItems = 0;
       std::list< V3_TrainDlg::SellItemInfo > itemList;
       
       WORD skillPnts;
       
       GET_WORD( skillPnts );
       GET_WORD( nbItems );
       
       // Get the item list from the packet.
       WORD i;
       for( i = 0; i < nbItems; i++ )
       {
          LPBYTE name;
          LPBYTE req;
          V3_TrainDlg::SellItemInfo trainInfo;
          
          BYTE canHave = 0;
          
          GET_BYTE( canHave );	        // TRUE if skill can be trained.
          trainInfo.canHave = canHave != 0 ? true : false;
          GET_WORD( trainInfo.skillID );	// ID of the skill.
          GET_LONG( trainInfo.price );	// Cost of skill training per skill points.
          GET_STRING( name );		        // Length of skill name
          GET_STRING( req );
          GET_LONG( trainInfo.skillPnts );
          GET_LONG( trainInfo.icon );
          
          
          
          trainInfo.name = reinterpret_cast< const char *>( name );
          trainInfo.reqs = reinterpret_cast< const char *>( req );
          itemList.push_back( trainInfo );
          
          delete req;
          delete name;
       }
       V3_TrainDlg::GetInstance()->Learn( skillPnts, itemList );
       
    } break;
    //////////////////////////////////////////////////////////////////////////////////////////						
    // Teach Profession list
    case RQ_NM_SendTeachFormuleList:
    {
       
       TFCPacket *msg = Msg;
       WORD nbItems = 0;

       std::list< V3_TrainProfDlg::ProfItemInfo > itemList;
              
       GET_WORD ( V3_TrainProfDlg::GetInstance()->ushStatApo );
       GET_WORD ( V3_TrainProfDlg::GetInstance()->ushStatBij );
       GET_WORD ( V3_TrainProfDlg::GetInstance()->ushStatCou );
       GET_WORD ( V3_TrainProfDlg::GetInstance()->ushStatArm );
       GET_WORD ( V3_TrainProfDlg::GetInstance()->ushStatFor );
       GET_WORD ( V3_TrainProfDlg::GetInstance()->ushStatEbe );
       GET_LONG ( V3_TrainProfDlg::GetInstance()->dwStatGold );
       GET_WORD ( nbItems );
       
       
       WORD i;
       for( i = 0; i < nbItems; i++ )
       {
          LPBYTE name;
          V3_TrainProfDlg::ProfItemInfo trainInfo;
          
         
          GET_BYTE( trainInfo.canLearn );	        
          GET_BYTE( trainInfo.chProfession );	        
          GET_WORD( trainInfo.ushID );
          GET_WORD( trainInfo.ushSkillLevel );
          GET_LONG( trainInfo.dwCost );
          GET_STRING( name );	
          
          trainInfo.name = reinterpret_cast< const char *>( name );
          itemList.push_back( trainInfo );
          
          delete name;
       }
       V3_TrainProfDlg::GetInstance()->TrainProf( V3_TrainProfDlg::GetInstance()->dwStatGold, itemList );
    } break;
       
       //////////////////////////////////////////////////////////////////////////////////////////
       // Sent by server to update the client's time
    case RQ_GetTime: {
       Msg->Get((char *)&g_TimeStructure.Seconde);
       Msg->Get((char *)&g_TimeStructure.Minute);
       Msg->Get((char *)&g_TimeStructure.Hour);
       Msg->Get((char *)&g_TimeStructure.Day);
       Msg->Get((char *)&g_TimeStructure.Week);
       Msg->Get((char *)&g_TimeStructure.Month);
       Msg->Get((short *)&g_TimeStructure.Year);
       
                     } break;	
    case RQ_1stApril:
    {
       TFCPacket *msg = Msg;
       BYTE *AprilMessage;
       GET_STRING( AprilMessage );

       if(g_Var.dw1stAprilCnt == 0)
       {
          g_Var.wBubbleAnimCnt01 = 1;
          g_Var.wBubbleAnimCnt01R= 0;
          g_Var.wBubbleAnimCnt02 = 5;
          g_Var.wBubbleAnimCnt02R= 0;
          g_Var.wBubbleAnimCnt03 = 10;
          g_Var.wBubbleAnimCnt03R= 0;
          g_Var.dw1stAprilCnt = 1;
          g_Var.iFishXPos     = -g_Var.vsfFish.GetWidth();
          g_Var.iFishYPos     = g_Global.GetScreenH()-(rand()%75)-(2*g_Var.vsfFish.GetHeight());
          g_Var.bDisplayedMessage = false;
          sprintf_s(g_Var.strFishMessage,1024,"%s",AprilMessage);
       }
       if(AprilMessage)
          delete AprilMessage;

    }break;
       //////////////////////////////////////////////////////////////////////////////////////////
       // Receive skill list for skill use.
    case RQ_GetSkillList:{
       // First update the Player.tlUserSkill list.
       WORD skillQuantity = 0;
       
       // For the GET_ macros to work.
       TFCPacket *msg = Msg;
       
       GET_WORD( skillQuantity );
       
       Player.Lock();
       
       // Destroy the previous user skills.
       Player.tlUserSkill.ToHead();
       while( Player.tlUserSkill.QueryNext() )
       {
          Player.tlUserSkill.DeleteObject();
       }
       
       
       // Get all the skills in the packet.
       WORD i;
       for( i = 0; i < skillQuantity; i++ ){
          WORD skillID;
          BYTE skillUse;
          WORD skillPnts;
          WORD truePnts;
          BYTE *skillName;
          BYTE *skillDesc;
          
          GET_WORD( skillID );
          GET_BYTE( skillUse );
          GET_WORD( skillPnts );
          GET_WORD( truePnts );
          GET_STRING( skillName );
          GET_STRING( skillDesc );
          
          // Add new user skill for checking
          LPUSER_SKILL lpUserSkill = new USER_SKILL;
          
          lpUserSkill->SkillName  = reinterpret_cast< char * >( skillName );
          lpUserSkill->desc       = reinterpret_cast< char * >( skillDesc );
          lpUserSkill->wID        = skillID;
          lpUserSkill->dwStrength = skillPnts;
          lpUserSkill->bUse       = skillUse;
          lpUserSkill->dwTrueStrength = truePnts;
          
          if(skillDesc)
            delete skillDesc;
          if(skillName)
             delete skillName;
          
          Player.tlUserSkill.AddToTail( lpUserSkill );
       }
       
       Player.Unlock();
       
       V3_StatsDlg::GetInstance()->UpdateSkills();
    } break;
       //////////////////////////////////////////////////////////////////////////////////////////
       // Received a shop buying request
    case RQ_SendBuyItemList:
    {
       TFCPacket *msg = Msg;
       
       DWORD currentGold = 0;
       WORD nbItems = 0;
       std::list< V3_BuyDlg::SellItemInfo > itemList;
       
       GET_LONG( currentGold );
       GET_WORD( nbItems     );
       // Get the item list from the packet.
       WORD i;
       for( i = 0; i < nbItems; i++ )
       {
          LPBYTE name;
          LPBYTE req;
          V3_BuyDlg::SellItemInfo sellInfo;                
          
          sellInfo.dwID = 0;
          GET_WORD  ( sellInfo.dwID );
          GET_WORD  ( sellInfo.appearance );
          GET_LONG  ( sellInfo.price );
          BYTE bEquip = 0;
          GET_BYTE  ( bEquip );
          GET_STRING( name );                
          GET_STRING( req );
          
          if( bEquip != 0 )
             sellInfo.canEquip = true;
          else
             sellInfo.canEquip = false;
          
          sellInfo.name = reinterpret_cast< const char *>( name );
          sellInfo.reqs = reinterpret_cast< const char *>( req );
          itemList.push_back( sellInfo );
          
          delete req;
          delete name;
       }
       V3_BuyDlg::GetInstance()->Buy( itemList );
    } break;


    case RQ_SendPointsItemList:
       {
          TFCPacket *msg = Msg;

          DWORD currentPoints = 0;
          WORD nbItems = 0;
          std::list< V3_BuyDlg::SellItemInfo > itemList;

          GET_LONG( currentPoints );
          GET_WORD( nbItems     );
          // Get the item list from the packet.
          WORD i;
          for( i = 0; i < nbItems; i++ )
          {
             LPBYTE name;
             LPBYTE req;
             V3_BuyDlg::SellItemInfo sellInfo;                

             sellInfo.dwID = 0;
             GET_WORD  ( sellInfo.dwID );
             GET_WORD  ( sellInfo.appearance );
             GET_LONG  ( sellInfo.price );
             BYTE bEquip = 0;
             GET_BYTE  ( bEquip );
             GET_STRING( name );                
             GET_STRING( req );

             if( bEquip != 0 )
                sellInfo.canEquip = true;
             else
                sellInfo.canEquip = false;

             sellInfo.name = reinterpret_cast< const char *>( name );
             sellInfo.reqs = reinterpret_cast< const char *>( req );
             itemList.push_back( sellInfo );

             delete req;
             delete name;
          }
          V3_BuyDlg::GetInstance()->Echanger( itemList ,currentPoints);
       } break;
       //////////////////////////////////////////////////////////////////////////////////////////
       // Received a shop selling request
    case RQ_SendSellItemList:{
       
       TFCPacket *msg = Msg;
       
       DWORD currentGold = 0;
       WORD nbItems = 0;
       std::list< V3_BuyDlg::SellItemInfo > itemList;
       
       GET_LONG( currentGold );
       GET_WORD( nbItems     );
       // Get the item list from the packet.
       WORD i;
       for( i = 0; i < nbItems; i++ ){
          LPBYTE name;                
          V3_BuyDlg::SellItemInfo sellInfo;
          
          GET_LONG  ( sellInfo.dwID );
          GET_WORD  ( sellInfo.appearance );
          GET_LONG  ( sellInfo.price );
          GET_LONG  ( sellInfo.maxQty );
          GET_STRING( name );
          
          sellInfo.name = reinterpret_cast< const char *>( name );
          itemList.push_back( sellInfo );
          
          delete name;
       }
       V3_BuyDlg::GetInstance()->Sell( itemList );
    } break;
       //////////////////////////////////////////////////////////////////////////////////////////				
       // Receive requested spell list.
    case RQ_SendSpellList:{
       
       TFCPacket *msg = Msg;
       
       WORD spellCount;
       WORD i;
       
       BYTE bUpdate = 0;
       
       GET_BYTE( bUpdate );
       
       GET_WORD( Player.Mana );
       GET_WORD( Player.MaxMana );
       GET_WORD( spellCount );
       
       list< V3_SpellDlg::Spell > spellList;
       
       for( i = 0; i < spellCount; i++ )
       {
          V3_SpellDlg::Spell spell;
          
          // Get each spells in the packet.
          GET_WORD( spell.Id );
          
          if( spell.Id != 0 )
          {
             LPBYTE name;
             LPBYTE desc;
             WORD element, type;
             
             GET_BYTE( spell.targetType );
             GET_BYTE( spell.AttackSpell );
             GET_WORD( spell.manaCost );
             
             GET_LONG( spell.duration );
             GET_WORD( spell.level );
             GET_WORD( element );
             GET_WORD( type );
             GET_LONG( spell.icon );
             
             GET_STRING( desc );            
             GET_STRING( name );
             
             spell.desc = reinterpret_cast< const char * >( desc );
             spell.name = reinterpret_cast< const char * >( name );
             
             switch( element )
             {
                case 0: spell.element = V3_SpellDlg::Spell::normal; break;
                case 1: spell.element = V3_SpellDlg::Spell::fire; break;
                case 2: spell.element = V3_SpellDlg::Spell::earth; break;
                case 3: spell.element = V3_SpellDlg::Spell::air; break;
                case 4: spell.element = V3_SpellDlg::Spell::water; break;
                case 5: spell.element = V3_SpellDlg::Spell::light; break;
                case 6: spell.element = V3_SpellDlg::Spell::dark; break;
             };
             switch( type )
             {
               case 1: spell.type = V3_SpellDlg::Spell::physical; break;
               case 2: spell.type = V3_SpellDlg::Spell::mental; break;
             };
             
             
             delete name;
             delete desc;
             
             spellList.push_back( spell );
          }
       }
       
       V3_SpellDlg::GetInstance()->UpdateSpells( spellList );
      } break;
       
    case RQ_GodFlagUpdate:{
       TFCPacket *msg = Msg;
       
       
       BYTE flag;
       BYTE status;
       GET_BYTE( flag );
       GET_BYTE( status );
       
       switch( flag ){
       case 1:
          if( status != 0 ){
             Player.CanRunScripts = true;
          }else{
             Player.CanRunScripts = false;
          }
          break;
       case 2:
          if( status != 0 ){
             Player.CanSlayUsers = true;
          }else{
             Player.CanSlayUsers = false;
          }
          break;
       }
                          } break;
       
    case 10003: {
       short SkillID, Return;
       Msg->Get((short *)&SkillID);
       Msg->Get((short *)&Return);
       switch (SkillID) {
       case 5: // WarCry
          //									SoundFX[10].Play(false);
          break;
       }
                } break;
       
    case RQ_HPchanged: 
    {
       Msg->Get((long *)&Player.Hp);            

       // Auto-adaptation serveur Linux : NE PAS toucher V3_StatsDlg tant que l'UI in-game n'est pas
       // initialisee (g_UiInit, pose par le handler opcode 13). Le serveur Linux pousse un RQ_HPchanged
       // (33) AVANT la fin de l'entree : appeler GetInstance() ici declencherait la TOUTE PREMIERE
       // construction du dialogue sur le THREAD RESEAU, qui interbloque avec le thread de rendu (verrou
       // RootBoxUI/draw) -> thread reseau fige -> l'opcode 13 n'est jamais traite -> blocage au loading.
       // La valeur Player.Hp est deja stockee ci-dessus ; l'opcode 13 reconstruira la feuille ensuite.
       if(g_UiInit && V3_StatsDlg::GetInstance()->IsShown())
         V3_StatsDlg::GetInstance()->UpdateCharacterSheetHP();

    } break;

    case RQ_ManaChanged: 
    {
       Msg->Get((WORD *)&Player.Mana);            
       
       if(g_UiInit && V3_StatsDlg::GetInstance()->IsShown())
         V3_StatsDlg::GetInstance()->UpdateCharacterSheetMP();
    } break; 
       
    case RQ_PuppetInformation: {
       
       DWORD ID;
       WORD  BODY, FEET, GLOVES, HELM, LEGS, W_RIGHT, W_LEFT, CAPE, HAIR,TAG;
       
       Msg->Get((long *)&ID);
       Msg->Get((short *)&BODY);
       Msg->Get((short *)&FEET);
       Msg->Get((short *)&GLOVES);
       Msg->Get((short *)&HELM);
       Msg->Get((short *)&LEGS);
       Msg->Get((short *)&W_RIGHT);
       Msg->Get((short *)&W_LEFT);
       Msg->Get((short *)&CAPE);
       Msg->Get((short *)&HAIR);
       Msg->Get((short *)&TAG);
     
       
       Objects.Lock(57);
       Objects.SetPuppet(ID, BODY, FEET, GLOVES, HELM, LEGS, W_RIGHT, W_LEFT, CAPE, HAIR,TAG);
       Objects.Unlock(57);

       //OutputDebugString("receicve Puppet Info 1\n");
       
             } break;
       
    case RQ_UseObject:
       break;
       
    case RQ_UseSkill:
       break;
        
    case RQ_EquipObject: {
                         } break;
       
    case RQ_GetUnitName: {
       unsigned long ID = 0;
       short Len;
       char *Name      = new char [100];
       char *GuildName = new char [100];
       memset(Name     , 0, 100);
       memset(GuildName, 0, 100);
       DWORD color;
       DWORD GuildColor;

       Msg->Get((long *)&ID);
      
       //Get the Name...
       Msg->Get((short *)&Len);
       int j = 0;
       for (j = 0; j < Len && j < 100; j++) 
          Msg->Get((char *)&Name[j]);
       Name[Len < 100 ? Len : 100] = 0;

       //Get the GuildName...
       Msg->Get((short *)&Len);
       for (j = 0; j < Len && j < 100; j++) 
          Msg->Get((char *)&GuildName[j]);
       GuildName[Len < 100 ? Len : 100] = 0;

       Msg->Get((unsigned long *)&color);
       Msg->Get((unsigned long *)&GuildColor);
       
       Objects.Lock(58);
       if( strlen( GuildName ) > 0 )
       { 
         char strGuildName[102];
         sprintf_s(strGuildName,102,"[%s]",GuildName);
         Objects.SetGuildName(ID,strGuildName, GuildColor );//BLBLBL
       }

       Objects.SetName(ID, Name, TRUE, color,0);
       Objects.Identify(ID);
       Objects.Unlock(58);
       delete Name;
	    Name = NULL;
	    delete GuildName;
	    GuildName = NULL;
    } break;

    case RQ_GetUnitName2: {
       unsigned long ID = 0;
       short Len;
       char *Name      = new char [100];
       char *GuildName = new char [100];
       memset(Name     , 0, 100);
       memset(GuildName, 0, 100);
       DWORD color;
       DWORD GuildColor;
       short shShow;
       
       Msg->Get((long *)&ID);
       //Msg->Get((short *)&shLevel);
       
       
       //Get the Name...
       Msg->Get((short *)&Len);
       int j = 0;
       for (j = 0; j < Len && j < 100; j++) 
          Msg->Get((char *)&Name[j]);
       Name[Len < 100 ? Len : 100] = 0;
       
       //Get the GuildName...
       Msg->Get((short *)&Len);
       for (j = 0; j < Len && j < 100; j++) 
          Msg->Get((char *)&GuildName[j]);
       GuildName[Len < 100 ? Len : 100] = 0;
       
       Msg->Get((unsigned long *)&color);
       Msg->Get((unsigned long *)&GuildColor);
       Msg->Get((short *)&shShow);
       
       Objects.Lock(58);
       if( strlen( GuildName ) > 0 )
       { 
          char strGuildName[102];
          sprintf_s(strGuildName,102,"[%s]",GuildName);
          Objects.SetGuildName(ID,strGuildName, GuildColor );//BLBLBL
       }
       
       if(shShow >0)
       {
         Objects.SetName(ID, Name, TRUE, color,0);
         Objects.Identify(ID);
       }
       else
       {
         Objects.SetName(ID, Name, FALSE, color,0);
       }
       Objects.Unlock(58);
       delete Name;
       Name = NULL;
       delete GuildName;
       GuildName = NULL;
    } break;
       
    case 36: {
       TalkToX = 0;
       TalkToY = 0;
       TalkToW = 0;
       TalkToID = NULL;
       SelectedID = NULL;
       TalkToOffset = 0;
             } break;
       
    case 34: {
       DWORD Err = 0;
       // Deplacer le texte de NPC.
       unsigned long ID = 0;
       short Offset = 0;
       
       Err = 1;
       Msg->Get((long *)&ID);
       Err = 2;
       Msg->Get((short *)&Offset);
       
       
       Objects.Lock(60);
       Objects.SetOffset(ID, Offset);
       Objects.Unlock(60);
       
             } break;
       //////////////////////////////////////////////////////////////////////////////////////////
       // Sent by the server for the client to update its backpack
	case RQ_ViewBackpack2: {

		// View Backpack

		char *lpbName = NULL;

		unsigned short nbObjects = 0;
		//short Len;
		DWORD dwContainerID;	// ID of the container object.
		WORD  wNbObjects;		// Quantity of objects.
		WORD  wAppearance;		// Appearance of the object.
		DWORD dwID;				// ID of the object.
		BOOL  boFound;
		WORD  wBaseID;
		DWORD dwQty;
		int i;
		char cDisplay;			// 1 if box should be displayed
		long lCharges;
		BYTE chtarget;
		BYTE chAttack;
		BYTE chPVP;

		Msg->Get( (char *)&cDisplay );
		Msg->Get( (long *)&dwContainerID );
		Msg->Get( (short *)&wNbObjects );			

		TemplateList <BAG_ITEM> tlTempBackpack;

		LPBAG_ITEM lpBagItem;
		for ( i = 0; i < wNbObjects; i++ ) 
		{
			Msg->Get( (short *)&wAppearance );
			Msg->Get( (long  *)&dwID );
			Msg->Get( (short *)&wBaseID );
			Msg->Get( (long  *)&dwQty );
			Msg->Get( (long  *)&lCharges );
			Msg->Get( (char  *)&chtarget );
         Msg->Get( (char  *)&chAttack );
         Msg->Get( (char  *)&chPVP );

			// Create a new bag item.
			lpBagItem = new BAG_ITEM;
			lpBagItem->nX = -1;	// assign random pos to item
			lpBagItem->nY = -1;
			lpBagItem->dwID = dwID;
			lpBagItem->chIName[0] = 0;
			lpBagItem->Type = wAppearance;
			lpBagItem->lpSprite = InvItemIcons( wAppearance );		// TODO
			lpBagItem->wBaseID = wBaseID;
			lpBagItem->dwQty     = dwQty;
			lpBagItem->iCharges  = lCharges;
			lpBagItem->chTarget  = (BYTE)chtarget;
			lpBagItem->chAttack  = (BYTE)chAttack;
			lpBagItem->chPVP     = (BYTE)chPVP;
			lpBagItem->associatedUI = NULL;
			// and add it to the backpack

			tlTempBackpack.AddToTail( lpBagItem );
		}

		// First check if there are items which were removed
		Player.tlBackpack.Lock("HandlePacket 2");


		Player.tlBackpack.ToHead();
		while( Player.tlBackpack.QueryNext() )
		{
			lpBagItem = Player.tlBackpack.GetObject();

			boFound = FALSE;			
			tlTempBackpack.ToHead();
			while( tlTempBackpack.QueryNext() && !boFound )
			{
				LPBAG_ITEM newBagItem = tlTempBackpack.GetObject();

				// If the item doesn't have an ID yet and an item of the same
				// type is found.
				if( lpBagItem->dwID == 0 && 
					lpBagItem->associatedUI != NULL && 
					lpBagItem->wBaseID == newBagItem->wBaseID )
				{

					strcpy_s((char*)lpBagItem->chIName,512 ,(char*)newBagItem->chIName );

					// Use this new item's ID as the item's ID.
					lpBagItem->dwID = newBagItem->dwID;
				}

				if( newBagItem->dwID == lpBagItem->dwID )
				{
					// Update the bag item's item count
					lpBagItem->dwQty = newBagItem->dwQty;
					lpBagItem->iCharges = newBagItem->iCharges;
					lpBagItem->chTarget = newBagItem->chTarget;
					lpBagItem->chAttack = newBagItem->chAttack;
					lpBagItem->chPVP    = newBagItem->chPVP;

					// Update the associatedUI's name.
					if( lpBagItem->associatedUI != NULL )
					{
						static_cast< V3_InvDlg::ItemUI * >(lpBagItem->associatedUI)->UpdateName2();
					}

					boFound = TRUE;
				}
			}

			// If the object wasn't found in the new backpack, remove it
			if( !boFound )
			{
				Player.tlBackpack.DeleteObject();
			}
		}


		tlTempBackpack.ToHead();
		while( tlTempBackpack.QueryNext() )
		{
			lpBagItem = tlTempBackpack.GetObject();

			boFound = FALSE;
			Player.tlBackpack.ToHead();
			while( Player.tlBackpack.QueryNext() && !boFound )
			{
				if( Player.tlBackpack.GetObject()->dwID == lpBagItem->dwID )
				{
					boFound = TRUE;
				}
			}

			// If the object wasn't found in the old backpack, add it
			if( !boFound ){
				Player.tlBackpack.AddToTail( lpBagItem );
				tlTempBackpack.RemoveObject();
			}
		}

		tlTempBackpack.ToHead();
		while( tlTempBackpack.QueryNext() )
		{
			tlTempBackpack.DeleteObject();
		}


		Player.tlBackpack.ToHead();
		while (Player.tlBackpack.QueryNext()) 
		{
			BAG_ITEM *obj = Player.tlBackpack.GetObject();
			if (!obj->chIName[0]) 
			{
				TFCPacket sending;
				sending << (RQ_SIZE)RQ_QueryItemName;
				sending << (char)PL_SEARCHBACKPACK;
				sending << (long)obj->dwID;
				SEND_PACKET( sending );
			}
		}

		Player.tlBackpack.Unlock("HandlePacket 2");

		// Reporter la mise a jour UI sur le thread de rendu (evite deadlock threadLock a la mort).
		if (g_UiInit)
		   g_pendingInventoryRefresh = true;

						  } break;

    case RQ_UpdateBackpackAfterUse :
    {
       // View Backpack
       bool bDrawLog = false;
       char buf[512];
       
       char *lpbName = NULL;
       
       unsigned short nbObjects = 0;
       //short Len;
       DWORD dwContainerID;	// ID of the container object.
       WORD  wAppearance;		// Appearance of the object.
       DWORD dwID;				// ID of the object.
       BOOL  boFound;
       WORD  wBaseID;
       DWORD dwQty;
       char cDisplay;			// 1 if box should be displayed
       long lCharges;
       
       Msg->Get( (char *)&cDisplay );
       Msg->Get( (long *)&dwContainerID );
       
       Msg->Get( (short *)&wAppearance );
       Msg->Get( (long  *)&dwID );
       Msg->Get( (short *)&wBaseID );
       Msg->Get( (long  *)&dwQty );
       Msg->Get( (long  *)&lCharges );
       
       // First check if there are items which were removed
       Player.tlBackpack.Lock("HandlePacket 2");
       
       LPBAG_ITEM lpBagItem;
       boFound = FALSE;			

       Player.tlBackpack.ToHead();
       while( Player.tlBackpack.QueryNext() && !boFound)
       {
          lpBagItem = Player.tlBackpack.GetObject();
          
          // If the item doesn't have an ID yet and an item of the same // type is found.
          if( lpBagItem->dwID == 0 && 
             lpBagItem->associatedUI != NULL && 
             lpBagItem->wBaseID == wBaseID )
          {
             
             lpBagItem->chIName[0] = 0x00;
             lpBagItem->dwID = dwID;
          }
          
          if( dwID == lpBagItem->dwID )
          {
             // Update the bag item's item count
             lpBagItem->dwQty   = dwQty;
             lpBagItem->iCharges = lCharges;

             //affiche message de quantite
             sprintf_s( buf,512, g_LocalString[ 731 ], lpBagItem->chIName, lpBagItem->dwQty );
             bDrawLog = true;
             
             // Update the associatedUI's name.
             if( lpBagItem->associatedUI != NULL )
             {
               
                static_cast< V3_InvDlg::ItemUI * >(lpBagItem->associatedUI)->UpdateName2();
             }
             
             boFound = TRUE;
          }
       }


       
       Player.tlBackpack.ToHead();
       while (Player.tlBackpack.QueryNext()) 
       {
          BAG_ITEM *obj = Player.tlBackpack.GetObject();
          if (!obj->chIName[0]) 
          {
             TFCPacket sending;
             sending << (RQ_SIZE)RQ_QueryItemName;
             sending << (char)PL_SEARCHBACKPACK;
             sending << (long)obj->dwID;
             SEND_PACKET( sending );
          }
       }
       
       Player.tlBackpack.Unlock("HandlePacket 2");

       if(bDrawLog)
       {
          if(g_SaveGame.GetOptionsParam()->bDisplayRestant)
             V3_ChatLogDlg::GetInstance()->AddListLogs( "", buf, SYSTEM_COLOR,true);
          else
             V3_ChatLogDlg::GetInstance()->AddListLogs( "", buf, SYSTEM_COLOR,false);
       }

      
    }
    break;
    case RQ_ShowChestNew: {  V3_ChestNewDlg::GetInstance()->Display(NULL); }; break;
    case RQ_HideChestNew: {  V3_ChestNewDlg::GetInstance()->Hide();        }; break;
    case RQ_ChestNewContents:
    {
       DWORD dwQuantity = 0;
       BYTE  *szName;
       DWORD       dwAppearance;
       DWORD       dwID;
       DWORD       dwStaticRef;
       DWORD       dwQty;
       long        lCharge;
       long        lEquipPos;

       // Get Channel
       TFCPacket *msg = Msg;		
       GET_WORD( dwQuantity );
       vector< V3_ChestNewDlg::cItemChest > chestItemList( dwQuantity );

       char strTmp[100];
       sprintf_s(strTmp,100,"\n************ CHEST have %d Items ************\n",dwQuantity);
       OutputDebugString(strTmp);
 

       for (UINT i=0; i<dwQuantity; i++) 
       {
          GET_STRING( szName  );
          GET_WORD  ( dwAppearance );
          GET_LONG  ( dwID );
          GET_WORD  ( dwStaticRef );
          GET_LONG  ( dwQty );
          GET_LONG  ( lCharge );
          GET_LONG  ( lEquipPos );

          //if(((char*)szName)[0] == '?')
          //   ((char*)szName)[0] = 'E';

         

          V3_ChestNewDlg::cItemChest ItemS;
          ItemS.strName      = reinterpret_cast< const char * >( szName );
          //ItemS.strName[0]   = 0x00;
          ItemS.dwAppearance = dwAppearance;
          ItemS.dwID         = dwID;
          ItemS.dwStaticRef  = dwStaticRef;
          ItemS.dwQty        = dwQty;
          ItemS.lCharge      = lCharge;
          ItemS.lEquipPos    = lEquipPos;
          chestItemList[ i ] = ItemS;
          delete szName;
       } 

       V3_ChestNewDlg::GetInstance()->UpdateItemList(chestItemList);
    }
    break;
       
    case RQ_ShowChest: {V3_ChestDlg::GetInstance()->Display(NULL);}; break;
    case RQ_HideChest: {V3_ChestDlg::GetInstance()->Hide();       }; break;
       //////////////////////////////////////////////////////////////////////////////////////////
       // Sent by the server for the client to update the chest contents
    case RQ_ChestContents: {
       // View Backpack

       WORD  wNbObjects;		// Quantity of objects.
       WORD  wAppearance;		// Appearance of the object.
       DWORD dwID;				// ID of the object.
       WORD  wBaseID;
       DWORD dwQty;
       //BOOL  boFound;
       long lCharges;
       int i;

       Msg->Get( (short *)&wNbObjects );			

       TemplateList <BAG_ITEM> tlTempBackpack;

       LPBAG_ITEM lpBagItem;
       for ( i = 0; i < wNbObjects; i++ ) {
          Msg->Get( (short *)&wAppearance );
          Msg->Get( (long  *)&dwID );
          Msg->Get( (short *)&wBaseID );
          Msg->Get( (long  *)&dwQty );
          Msg->Get( (long  *)&lCharges );

          // Create a new bag item.
          lpBagItem = new BAG_ITEM;
          lpBagItem->nX         = -1;	// assign random pos to item
          lpBagItem->nY         = -1;
          lpBagItem->dwID       = dwID;
          lpBagItem->chIName[0] = 0;
          lpBagItem->Type       = wAppearance;
          lpBagItem->lpSprite   = InvItemIcons( wAppearance );		// TODO
          lpBagItem->wBaseID    = wBaseID;
          lpBagItem->dwQty      = dwQty;
          lpBagItem->iCharges   = lCharges;
          lpBagItem->chTarget   = 0;
          lpBagItem->chAttack = 0;
          lpBagItem->chPVP    = 0;
          // and add it to the backpack

          tlTempBackpack.AddToTail( lpBagItem );
       }

       // Update the new interface inventory.

       V3_ChestDlg::GetInstance()->UpdateChest( &tlTempBackpack );
    } break;
        //////////////////////////////////////////////////////////////////////////////////////////
    // Sent by the server to notify someone is inviting the player to trade
    case RQ_TradeInvite: {

		TFCPacket *msg = Msg;
		BYTE *invitorName;
		DWORD invitorID;
		int x, y;
		GET_STRING( invitorName );
		GET_LONG( invitorID );
		GET_WORD(x);
		GET_WORD(y);
		V3_TradeDlg::GetInstance()->EventPlayerInvited((char*)invitorName, invitorID, x, y);
	}; break;
    //////////////////////////////////////////////////////////////////////////////////////////
    // Sent by the server to notify that the trade has started
	case RQ_TradeStarted: {

		TFCPacket *msg = Msg;
		BYTE *invitorName;
		GET_STRING( invitorName );
		V3_TradeDlg::GetInstance()->EventTradeStarted((char*)invitorName);
	}; break;
    //////////////////////////////////////////////////////////////////////////////////////////
    // Sent by the server to notify that the trade has started
	case RQ_TradeCancel: {

		V3_TradeDlg::GetInstance()->EventTradeCanceled();
	}; break;
    //////////////////////////////////////////////////////////////////////////////////////////
    // Sent by the server to notify that the trade has finished
	case RQ_TradeFinish: {
		V3_TradeDlg::GetInstance()->EventTradeFinished();
	}; break;
        //////////////////////////////////////////////////////////////////////////////////////////
        // Sent by the server for the client to update the trade contents
    case RQ_TradeContents: {
        // View Backpack
        
		int affectedChar;
		WORD tempAffectedChar;
		Msg->Get( (short*)&tempAffectedChar );
		affectedChar = tempAffectedChar;

		char *lpbName = NULL;
        
        WORD  wNbObjects;		// Quantity of objects.
        WORD  wAppearance;		// Appearance of the object.
        DWORD dwID;				// ID of the object.
        WORD  wBaseID;
        DWORD dwQty;
        //BOOL  boFound;
        long lCharges;
        int i;

        Msg->Get( (short *)&wNbObjects );			
        
        TemplateList <BAG_ITEM> tlTempBackpack;
        
        LPBAG_ITEM lpBagItem;
        for ( i = 0; i < wNbObjects; i++ ) {

            Msg->Get( (short *)&wAppearance );
            Msg->Get( (long  *)&dwID );
            Msg->Get( (short *)&wBaseID );
            Msg->Get( (long  *)&dwQty );
            Msg->Get( (long  *)&lCharges );
            
            // Create a new bag item.
            lpBagItem = new BAG_ITEM;
            lpBagItem->nX = -1;	// assign random pos to item
            lpBagItem->nY = -1;
            lpBagItem->dwID = dwID;
            lpBagItem->chIName[0] = 0;
            lpBagItem->Type = wAppearance;
            lpBagItem->lpSprite = InvItemIcons( wAppearance );		// TODO
            lpBagItem->wBaseID = wBaseID;
            lpBagItem->dwQty     = dwQty;
            lpBagItem->iCharges  = lCharges;
			lpBagItem->chTarget  = 0;
			lpBagItem->chAttack = 0;
			lpBagItem->chPVP    = 0;
            // and add it to the backpack
            
            tlTempBackpack.AddToTail( lpBagItem );
        }

        // Update the new interface inventory.

        if (affectedChar == AC_MYSELF) 
        {
		   	V3_TradeDlg::GetInstance()->UpdateTrade( &tlTempBackpack, AC_MYSELF );
        } 
        else if (affectedChar == AC_OTHER) 
        {
			   // Update other's trade contents
		   	V3_TradeDlg::GetInstance()->UpdateTrade( &tlTempBackpack, AC_OTHER );
        }
        
       } break;

	case RQ_TradeSetStatus:
		{

			TFCPacket *msg = Msg;
			int myStatus, otherStatus;
			WORD temp;
			GET_WORD(temp);
			myStatus = temp;
			GET_WORD(temp);
			otherStatus = temp;
			V3_TradeDlg::GetInstance()->EventTradeStatusChanged(myStatus, otherStatus);

		} break;

       
       //////////////////////////////////////////////////////////////////////////////////////////
       // Rob a user!
    case RQ_Rob:
    {
       TFCPacket *msg = Msg;
       
       list< V3_VolDlg::Item > itemList;
       
       char canRob = 0;
       DWORD targetId = 0;
       BYTE *targetName;
       GET_BYTE( canRob );
       GET_LONG( targetId );
       GET_STRING( targetName );
       
       WORD nbObjects;
       GET_WORD( nbObjects );
       
       WORD i;
       for ( i = 0; i < nbObjects; i++ ) 
       {
          V3_VolDlg::Item item;
          
          BYTE *name;
          
          GET_WORD( item.appearance );
          GET_LONG( item.id );
          GET_WORD( item.baseId );
          GET_LONG( item.qty );
          GET_STRING( name );
          item.itemName = reinterpret_cast< char * >( name );
          
          char buf[ 256 ];
		  _itoa_s( item.qty, buf,256, 10 );
          item.itemName += " x";
          item.itemName += buf;
          
          itemList.push_back( item );
          
          delete name;
       }
       
       V3_VolDlg::GetInstance()->Rob(canRob != 0 ? true : false, reinterpret_cast< char * >( targetName ), itemList );
       
       delete targetName;
   } break;
       //////////////////////////////////////////////////////////////////////////////////////////    /
       // Force a premature dispell of the robbing window
    case RQ_DispellRob:
    {
       V3_VolDlg::GetInstance()->DispellRob();
       
    } break;
       //////////////////////////////////////////////////////////////////////////////////////////
       // This request is sent as a response to a QueryItemName packet
    case RQ_QueryItemName:
       {
          //OutputDebugString("Entre RQ_QueryItemName\n");
          LPBAG_ITEM lpBagItem = NULL;
          LPBYTE lpbName = NULL;
          DWORD dwID;
          BYTE  whereToSearchFrom;
          BOOL boFound = FALSE;								
          TFCPacket *msg = Msg;
          
          GET_BYTE( whereToSearchFrom );
          GET_LONG( dwID );
          GET_STRING( lpbName );

          if(strlen((char*)lpbName) >255)
             lpbName[255] = NULL;

         
        
          if (whereToSearchFrom == PL_SEARCHBACKPACK) 
          {
             
             Player.tlBackpack.Lock("HandlePacket 3");
             Player.tlBackpack.ToHead();
             while( Player.tlBackpack.QueryNext() )
             {
                lpBagItem = Player.tlBackpack.GetObject();
                
                // Update each item with the same base ID.
                if( lpBagItem->dwID == dwID)
                {
                   strcpy_s((char *)lpBagItem->chIName,512, (char *)lpbName);

                   
                   // If there is an associated UI.
                   if( lpBagItem->associatedUI != NULL )
                   {
                      // Set its help text according to the UI's text.
                      static_cast< V3_InvDlg::ItemUI * >( lpBagItem->associatedUI )->UpdateName2();
                   }
                   break;
                }
             }
             
             Player.tlBackpack.Unlock("HandlePacket 3");
          }
          else if (whereToSearchFrom == PL_SEARCHCHEST) 
          {
             V3_ChestDlg::GetInstance()->UpdateItemsName(dwID, lpbName);
          }
          else if (whereToSearchFrom == PL_SEARCHGUILDCHEST) 
          {
             V3_GuildChestDlg::GetInstance()->UpdateItemsName(dwID, lpbName);
          }
          else if (whereToSearchFrom == PL_SEARCHMYTRADE || whereToSearchFrom == PL_SEARCHOTHERTRADE) 
          {
             V3_TradeDlg::GetInstance()->UpdateItemsName(dwID, lpbName, (ePlacesToSearch)whereToSearchFrom);
          }
          if(lpbName)
            delete []lpbName;
          lpbName = NULL;
          
       }
       break;
    case RQ_QueryItemInfo:
       {
          //Annex : stat values ( from Unit.h ) 

        
          TFCPacket *msg = Msg;
          
          short shNbrBoust;
          BYTE   chStatus;  // 0 = OK, data follows. 1 = no such unit. 2 = not an object 
          g_Global.Lock();
          g_Global.ResetItemInfo();
          sItemInfo *pNewInfo = g_Global.GetItemInfo();
         
          
          GET_BYTE  ( chStatus );
          if(chStatus == 0)
          {
             GET_STRING( pNewInfo->Name );
             GET_WORD  ( pNewInfo->shAppearance);
             GET_BYTE  ( pNewInfo->chRadiance );
             GET_WORD  ( pNewInfo->shAC);
             GET_WORD  ( pNewInfo->shDodge);
             GET_WORD  ( pNewInfo->shEnd);
             GET_LONG  ( pNewInfo->lMinDamage);
             GET_LONG  ( pNewInfo->lMaxDamage);
             GET_WORD  ( pNewInfo->shAttack);
             GET_WORD  ( pNewInfo->shStr);
             GET_WORD  ( pNewInfo->shAgi);
             GET_WORD  ( pNewInfo->shWis);
             GET_WORD  ( pNewInfo->shInt);

               
             GET_WORD  ( shNbrBoust);

             for(int n=0;n<shNbrBoust;n++)
             {
                sItemBoust sBoustNew;
                GET_BYTE  ( sBoustNew.chBoustStat);
                GET_LONG  ( sBoustNew.lMinVal);
                GET_LONG  ( sBoustNew.lMaxVal);
                
                //scan to sort...
                bool bInsert = false;
                vector <sItemBoust>::iterator it = pNewInfo->vBoust.begin();
                for(UINT b=0;b<pNewInfo->vBoust.size();b++,it++)
                {
                   if(sBoustNew.chBoustStat >= pNewInfo->vBoust[b].chBoustStat)
                   {
                      //on insert here
                      bInsert = true;
                      pNewInfo->vBoust.insert(it,sBoustNew);
                      break;
                   }
                }
                if(!bInsert)
                   pNewInfo->vBoust.push_back(sBoustNew);

             }
             pNewInfo->bSet = true;
          }
          else
             pNewInfo->bSet = false;
          
          g_Global.Unlock();

       }
       break;

    case RQ_DamageUnit: 
    {

       if(g_SaveGame.GetOptionsParam()->bDisplayDamage)
       {
          unsigned long ID = 0;
          DWORD color;
          short Len;
          char *strDamage = new char [100];
          memset(strDamage, 0, 100);
       
          Msg->Get((long *)&ID);
          Msg->Get((short *)&Len);
          for (int j = 0; j < Len && j < 100; j++) 
             Msg->Get((char *)&strDamage[j]);
          strDamage[Len < 100 ? Len : 100] = 0;
          Msg->Get((unsigned long *)&color);
       
          Objects.Lock(58);
          Objects.SetDamageTxt(ID,strDamage,color);
          Objects.Unlock(58);
          delete strDamage;
       }
    } break;

    case RQ_HealingUnit: 
       {
          
          if(g_SaveGame.GetOptionsParam()->bDisplayHealing)
          {
             unsigned long ID = 0;
             DWORD color;
             short Len;
             char *strDamage = new char [100];
             memset(strDamage, 0, 100);
             
             Msg->Get((long *)&ID);
             Msg->Get((short *)&Len);
             for (int j = 0; j < Len && j < 100; j++) 
                Msg->Get((char *)&strDamage[j]);
             strDamage[Len < 100 ? Len : 100] = 0;
             Msg->Get((unsigned long *)&color);
             
             Objects.Lock(58);
             Objects.SetDamageTxt(ID,strDamage,color);
             Objects.Unlock(58);
             delete strDamage;
          }
    } break;

    case RQ_UpdateWeight:{
       DWORD w, maxW;
       
       TFCPacket *msg = Msg;
       GET_LONG( w );
       GET_LONG( maxW );
       
       Player.Weight    = (USHORT)w;
       Player.MaxWeight = (USHORT)maxW;
       
       if( V3_StatsDlg::GetInstance()->IsShown())
         V3_StatsDlg::GetInstance()->UpdateCharacterSheetWeight();
       
    }break;


    case RQ_NM_GetGuildList:
    {
          
          short Len = 0;
          int i = 0, j = 0;

          short wUserQuantity = 0;
          DWORD dwPermissionU = 0;
          BYTE  *szNote;
          BYTE  *szUser;
          BYTE  *szGuildName;
          BYTE  *szGuildR1;
          BYTE  *szGuildR2;
          BYTE  *szGuildR3;
          BYTE  *szGuildR4;
          BYTE  *szGuildR5;
          DWORD dwPermission;
          DWORD dwTitre;
          DWORD dwID;

          BYTE  bOnline;
          BYTE  bShow;
          
          // Get Channel
          TFCPacket *msg = Msg;		

          GET_STRING( szGuildR1 );
          GET_STRING( szGuildR2 );
          GET_STRING( szGuildR3 );
          GET_STRING( szGuildR4 );
          GET_STRING( szGuildR5 );
          
          GET_BYTE  ( bShow );
          GET_STRING( szGuildName );
          GET_LONG( dwPermissionU );
          GET_STRING( szNote );
          GET_WORD( wUserQuantity );
          
          vector< V3_GuildDlg::User > users( wUserQuantity );
          
          for (j = 0; j < wUserQuantity; j++) 
          {
             GET_STRING( szUser  );
             GET_LONG  ( dwTitre );
             GET_LONG  ( dwID );
             GET_LONG  ( dwPermission );
             GET_BYTE  ( bOnline );
             
             
             V3_GuildDlg::User user;
             user.strUserName    = reinterpret_cast< const char * >( szUser );
             user.dwTitle        = dwTitre;
             user.dwID           = dwID;
             user.dwPermission   = dwPermission;
             user.bOnline        = bOnline == 0 ? false : true;
             
             users[ j ] = user;
             
             delete szUser;
          }
          V3_GuildDlg::GetInstance()->UpdateGuildList(bShow,users,(char*)szGuildName,(char*)szNote,dwPermissionU,
                                                      (char*)szGuildR1,(char*)szGuildR2,(char*)szGuildR3,(char*)szGuildR4,(char*)szGuildR5);

          delete szNote;
          delete szGuildName;
          delete szGuildR1;
          delete szGuildR2;
          delete szGuildR3;
          delete szGuildR4;
          delete szGuildR5;
          
    }break;
    case RQ_NM_GuildInvite:
    {
       
       TFCPacket *msg = Msg;
       
       DWORD sourceID = 0;
       BYTE *sourceName;
       BYTE *GuildName;
       
       GET_LONG( sourceID );
       GET_STRING( sourceName );
       GET_STRING( GuildName );
       
       V3_GuildDlg::GetInstance()->Join( sourceID, reinterpret_cast< const char * >( sourceName ),reinterpret_cast< const char * >( GuildName ) );
       
       delete sourceName;       
       delete GuildName;  
    } break;



    case RQ_NM_GuildLeave:
    {
       V3_ChatLogDlg::GetInstance()->AddListLogs( "", g_LocalString[ 520 ], SYSTEM_COLOR, false );
    }break;

    case RQ_NM_GuildGetLogs:
    {
       TFCPacket *msg = Msg;
       
       BYTE chStatus;
       GET_BYTE  ( chStatus );
       if(chStatus == 0)
       {
          V3_GuildDlg::GetInstance()->ClearLogList();
       }
       else if(chStatus == 2)
       {
          V3_GuildDlg::GetInstance()->DisplayLog();
       }
       else
       {
          BYTE *psDate;
          BYTE *psLogs;

          GET_STRING( psDate );
          GET_STRING( psLogs );

          V3_GuildDlg::GetInstance()->AddLogs((char*)psDate, (char*)psLogs);

          delete psDate;       
          delete psLogs; 
       }
    }
    break;

    case RQ_GuildChestNewShow: {V3_GuildChestNewDlg::GetInstance()->Display(NULL);}; break;
    case RQ_GuildChestNewHide: {V3_GuildChestNewDlg::GetInstance()->Hide();       }; break;
    case RQ_GuildChestNewContents:
       {
          DWORD dwQuantity = 0;
          BYTE  *szName;
          DWORD       dwAppearance;
          DWORD       dwID;
          DWORD       dwStaticRef;
          DWORD       dwQty;
          long        lCharge;
          long        lEquipPos;

          // Get Channel
          TFCPacket *msg = Msg;		
          GET_WORD( dwQuantity );
          vector< V3_GuildChestNewDlg::cItemChest > chestItemList( dwQuantity );

          char strTmp[100];
          sprintf_s(strTmp,100,"\n************ CHEST have %d Items ************\n",dwQuantity);
          OutputDebugString(strTmp);


          for (UINT i=0; i<dwQuantity; i++) 
          {
             GET_STRING( szName  );
             GET_WORD  ( dwAppearance );
             GET_LONG  ( dwID );
             GET_WORD  ( dwStaticRef );
             GET_LONG  ( dwQty );
             GET_LONG  ( lCharge );
             GET_LONG  ( lEquipPos );

             //if(((char*)szName)[0] == '?')
             //   ((char*)szName)[0] = 'E';



             V3_GuildChestNewDlg::cItemChest ItemS;
             ItemS.strName      = reinterpret_cast< const char * >( szName );
             //ItemS.strName[0]   = 0x00;
             ItemS.dwAppearance = dwAppearance;
             ItemS.dwID         = dwID;
             ItemS.dwStaticRef  = dwStaticRef;
             ItemS.dwQty        = dwQty;
             ItemS.lCharge      = lCharge;
             ItemS.lEquipPos    = lEquipPos;
             chestItemList[ i ] = ItemS;
             delete szName;
          } 

          V3_GuildChestNewDlg::GetInstance()->UpdateItemList(chestItemList);
       }
       break;

    case RQ_NM_GuildChestShow: {V3_GuildChestDlg::GetInstance()->Display(NULL);}; break;
    case RQ_NM_GuildChestHide: {V3_GuildChestDlg::GetInstance()->Hide();       }; break;

    case RQ_NM_GuildChestContents: 
    {
       // View Backpack
       WORD  wNbObjects;		// Quantity of objects.
       WORD  wAppearance;		// Appearance of the object.
       DWORD dwID;				// ID of the object.
       WORD  wBaseID;
       DWORD dwQty;
       //BOOL  boFound;
       long lCharges;
       int i;
       
       Msg->Get( (short *)&wNbObjects );			
       
       TemplateList <BAG_ITEM> tlTempBackpack;
       
       LPBAG_ITEM lpBagItem;
       for ( i = 0; i < wNbObjects; i++ ) 
       {
          Msg->Get( (short *)&wAppearance );
          Msg->Get( (long  *)&dwID );
          Msg->Get( (short *)&wBaseID );
          Msg->Get( (long  *)&dwQty );
          Msg->Get( (long  *)&lCharges );

          // Create a new bag item.
          lpBagItem = new BAG_ITEM;
          lpBagItem->nX = -1;	// assign random pos to item
          lpBagItem->nY = -1;
          lpBagItem->dwID = dwID;
          lpBagItem->chIName[0] = 0;
          lpBagItem->Type = wAppearance;
          lpBagItem->lpSprite = InvItemIcons( wAppearance );		// TODO
          lpBagItem->wBaseID = wBaseID;
          lpBagItem->dwQty     = dwQty;
          lpBagItem->iCharges   = lCharges;
          lpBagItem->chTarget   = 0;
          lpBagItem->chAttack = 0;
          lpBagItem->chPVP    = 0;
          // and add it to the backpack
          
          tlTempBackpack.AddToTail( lpBagItem );
       }
       
       // Update the new interface inventory.
       V3_GuildChestDlg::GetInstance()->UpdateChest( &tlTempBackpack );
    } break;
       



   case RQ_NM_GetAHList:
    {

          //V3_AHDlg::GetInstance()->AHLock();
          //while(V3_AHDlg::GetInstance()->IsDrawProgress())
          //   Sleep(0);
          
          short Len = 0;
          DWORD i = 0, j = 0;

          DWORD dwQuantity = 0;

          DWORD dwPlayerID;
          DWORD dwIsShow;
          BYTE  *szName;
          DWORD dwOwnerID;
          DWORD dwIndex;
          DWORD dwQty;
          DWORD dwTime;
          DWORD dwBuy;
          DWORD dwBuyNMS;
          DWORD dwBid;
          DWORD dwCurBid;
          DWORD dwBidOwnerID;
          DWORD dwEquipPos;
          DWORD dwTS;

          // Get Channel
          TFCPacket *msg = Msg;		

          GET_LONG( dwPlayerID );
          GET_LONG( dwIsShow );
          GET_LONG( dwQuantity );

          
          vector< V3_AHDlg::ItemSold > ItemList( dwQuantity );
          
          for (j = 0; j < dwQuantity; j++) 
          {
             GET_LONG  ( dwIndex );
             GET_LONG  ( dwOwnerID );
             GET_STRING( szName  );
             GET_LONG  ( dwQty );
             GET_LONG  ( dwTime );
             GET_LONG  ( dwBuy );
             GET_LONG  ( dwBuyNMS );
             GET_LONG  ( dwBid );
             GET_LONG  ( dwCurBid );
             GET_LONG  ( dwBidOwnerID );
             GET_LONG  ( dwEquipPos );
             GET_LONG  ( dwTS );

             

             V3_AHDlg::ItemSold ItemS;
             ItemS.strName    = reinterpret_cast< const char * >( szName );
             ItemS.dwIndex    = dwIndex;
             ItemS.dwOwnerID  = dwOwnerID;
             ItemS.dwQty      = dwQty;
             ItemS.dwTime     = dwTime;
             ItemS.dwBuy      = dwBuy;
             ItemS.dwBuyNMS   = dwBuyNMS;
             ItemS.dwBid      = dwBid;
             ItemS.dwCurBid   = dwCurBid;
             ItemS.dwCurBidID = dwBidOwnerID;
             ItemS.dwEquipPos = dwEquipPos;
             ItemS.dwTimeEnter= dwTS;
             
             
             ItemList[ j ] = ItemS;
             delete szName;
          } 
          
          V3_AHDlg::GetInstance()->UpdateItemList(dwPlayerID,ItemList);

          if(dwIsShow)
            V3_AHDlg::GetInstance()->Display(NULL);
          
       //V3_AHDlg::GetInstance()->AHUnlock();
    }break;

   case RQ_NM_NMSGOLD_AchatOpt1:
   {
      TFCPacket *msg = Msg;
      
      DWORD  dwNbrNMSGold;
      USHORT ushNbrAchat;
      GET_LONG( dwNbrNMSGold );
      GET_WORD( ushNbrAchat);
      
      
      
      V3_BoutiqueDlg::GetInstance()->ClearAchatList();
      
      for(DWORD i=0;i<ushNbrAchat;i++)
      {
         DWORD          dwCost;
         unsigned char *pName = NULL; 
         unsigned char *pDesc = NULL; 
         
         GET_STRING( pName );
         GET_STRING( pDesc );
         GET_LONG  ( dwCost);
         
         if(strlen((char*)pName) > 127)
            pName[127] = 0;
         if(strlen((char*)pDesc) > 511)
            pDesc[511] = 0;
         
         
         
         V3_BoutiqueDlg::GetInstance()->AddAchatOption((char*)pName,(char*)pDesc,dwCost);
         
         if(pName)
            delete []pName;
         pName = NULL;
         if(pDesc)
            delete []pDesc;
         pDesc = NULL;
      }
      V3_BoutiqueDlg::GetInstance()->SetAchatOpt(1);
      V3_BoutiqueDlg::GetInstance()->SetNbrNMSGold(dwNbrNMSGold);
      V3_BoutiqueDlg::GetInstance()->ListToControl();
   } 
   break;

   case RQ_NM_NMSGOLD_AchatOpt2:
   {
      TFCPacket *msg = Msg;
      
      DWORD  dwNbrNMSGold;
      USHORT ushNbrAchat;
      GET_LONG( dwNbrNMSGold );
      GET_WORD( ushNbrAchat);
      
      
      
      V3_BoutiqueDlg::GetInstance()->ClearAchatList();
      
      for(DWORD i=0;i<ushNbrAchat;i++)
      {
         DWORD          dwCost;
         unsigned char *pName = NULL; 
         unsigned char *pDesc = NULL;
         
         GET_STRING( pName );
         GET_LONG  ( dwCost);
         GET_STRING( pDesc );
         
         if(strlen((char*)pName) > 127)
            pName[127] = 0;
         if(strlen((char*)pDesc) > 511)
            pDesc[511] = 0;
        
         
         
         V3_BoutiqueDlg::GetInstance()->AddAchatOption((char*)pName,(char*)pDesc,dwCost);
         
         if(pName)
            delete []pName;
         pName = NULL;
         if(pDesc)
            delete []pDesc;
         pDesc = NULL;
      }
      V3_BoutiqueDlg::GetInstance()->SetAchatOpt(2);
      V3_BoutiqueDlg::GetInstance()->SetNbrNMSGold(dwNbrNMSGold);
      V3_BoutiqueDlg::GetInstance()->ListToControl();
   } 
   break;

   case RQ_NM_NMSGOLD_AchatOpt3:
   {
      TFCPacket *msg = Msg;
      
      DWORD  dwNbrNMSGold;
      USHORT ushNbrAchat;
      GET_LONG( dwNbrNMSGold );
      GET_WORD( ushNbrAchat);
     
      
      
      
      V3_BoutiqueDlg::GetInstance()->ClearAchatList();
      
      for(DWORD i=0;i<ushNbrAchat;i++)
      {
         DWORD          dwCost;
         unsigned char *pName = NULL; 
         unsigned char *pDesc = NULL;
         
         GET_STRING( pName );
         GET_LONG  ( dwCost);
         GET_STRING( pDesc );
         
         if(strlen((char*)pName) > 127)
            pName[127] = 0;
         if(strlen((char*)pDesc) > 511)
            pDesc[511] = 0;
         
         
         V3_BoutiqueDlg::GetInstance()->AddAchatOption((char*)pName,(char*)pDesc,dwCost);
         
         if(pName)
            delete []pName;
         pName = NULL;
         if(pDesc)
            delete []pDesc;
         pDesc = NULL;
      }
      V3_BoutiqueDlg::GetInstance()->SetAchatOpt(3);
      V3_BoutiqueDlg::GetInstance()->SetNbrNMSGold(dwNbrNMSGold);
      V3_BoutiqueDlg::GetInstance()->ListToControl();
   } 
   break;


   case RQ_NM_NMSGOLD_ListPanier:
      {
         TFCPacket *msg = Msg;
         
         DWORD  dwNbrNMSGold;
         USHORT ushNbrAchat;
         GET_LONG( dwNbrNMSGold );
         GET_WORD( ushNbrAchat);
         
         
         
         V3_PanierDlg::GetInstance()->ClearAchatList();
         
         for(DWORD i=0;i<ushNbrAchat;i++)
         {
            char           chCanCredite;
            DWORD          dwID;
            unsigned char *pName = NULL; 
            unsigned char *pInf = NULL; 
            
            GET_STRING( pName );
            GET_STRING( pInf );
            GET_LONG  ( dwID);
            GET_BYTE  ( chCanCredite);
            
                      
            
            V3_PanierDlg::GetInstance()->AddAchatOption((char*)pName,(char*)pInf,dwID,chCanCredite);
            
            if(pName)
               delete []pName;
            pName = NULL;
            if(pInf)
               delete []pInf;
            pInf = NULL;
         }
         V3_PanierDlg::GetInstance()->SetNbrNMSGold(dwNbrNMSGold);
         V3_PanierDlg::GetInstance()->ListToControl();
      } 
   break; 
   case RQ_NM_NMSGOLD_Sanction:
   {
       TFCPacket *msg = Msg;

       USHORT ushNbrSanction;
       DWORD  dwNbrHrsJail;
       GET_WORD( ushNbrSanction );
       GET_LONG( dwNbrHrsJail );

       V3_PanierDlg::GetInstance()->SetSanction(ushNbrSanction,dwNbrHrsJail);

   }
   break;



   case RQ_GetAllPlayerPos:
   {
 
   
      TFCPacket *msg = Msg;

      unsigned short ushNbrPl;
      GET_WORD( ushNbrPl );

      V3_GMDlg::GetInstance()->ClearPosList();

      for(DWORD i=0;i<ushNbrPl;i++)
      {
         unsigned short ushX,ushY,ushW;
         unsigned char *pName = NULL; 
         GET_STRING( pName );
         GET_WORD( ushX );
         GET_WORD( ushY );
         GET_WORD( ushW );


         V3_GMDlg::GetInstance()->AddPlayerPos((char*)pName,ushX,ushY,ushW);




         if(pName)
            delete []pName;
          pName = NULL;
      }

      V3_GMDlg::GetInstance()->ReadyPosList();


   }
   break;

   case RQ_GetAllArealinkForWorld:
   {
      TFCPacket *msg = Msg;
      
      long ulNbrArealink;
      long ulWorld;
      GET_LONG( ulNbrArealink );
      GET_LONG( ulWorld );
      if(ulWorld <0 || ulWorld >= NBR_ZONE_MAP)
         ulWorld = 0;


      g_Global.m_vArealinkVector[ulWorld].clear();

      for(int i=0;i<ulNbrArealink;i++)
      {
         sLinkAreaWDA nA;
         
         GET_WORD( nA.dwSrcX );
         GET_WORD( nA.dwSrcY );
         GET_WORD( nA.dwSrcW );
         GET_WORD( nA.dwDesX );
         GET_WORD( nA.dwDesY );
         GET_WORD( nA.dwDesW );
         
         g_Global.m_vArealinkVector[ulWorld].push_back(nA);
      }
   }
   break;


    case RQ_SvrOptions:
    {
       TFCPacket *msg = Msg;

       DWORD dwNbrOption;

       GET_LONG( dwNbrOption );


       V3_GMDlg::GetInstance()->ClearOptList();

       for(DWORD i=0;i<dwNbrOption;i++)
       {
          BYTE chOpt;
          unsigned char *pName = NULL; 
          GET_STRING( pName );
          GET_BYTE( chOpt );

          V3_GMDlg::GetInstance()->UpdateOptList((char*)pName,chOpt);


          if(pName)
             delete []pName;
          pName = NULL;
       }


       V3_GMDlg::GetInstance()->OptListToControl();
       V3_GMDlg::GetInstance()->Display(NULL,true,7,0);
    }break;

    case RQ_GetEventsList:
    {
       TFCPacket *msg = Msg;

       V3_GMDlg::GetInstance()->ClearEventsList();

       DWORD dwNbrE;
       GET_LONG( dwNbrE );
       for(DWORD i=0;i<dwNbrE;i++)
       {
          long lFlagID,lValueStop,lStatus;
          unsigned char *pName = NULL; 
          GET_STRING( pName );
          GET_LONG( lFlagID );
          GET_LONG( lValueStop );
          GET_LONG( lStatus );

          V3_GMDlg::GetInstance()->UpdateEventsList((char*)pName,lFlagID,lValueStop,lStatus);


          if(pName)
             delete []pName;
          pName = NULL;
       }

       V3_GMDlg::GetInstance()->EventsListToControl();
       V3_GMDlg::GetInstance()->Display(NULL,true,10,0);
    }
    break;


    
    case RQ_GMMSG_Get:
    {
       TFCPacket *msg = Msg;

       DWORD dwNBR;
       GET_LONG( dwNBR );

       bool bProcess = true;
       V3_GMDlg::GetInstance()->LockList();

       V3_GMDlg::GetInstance()->ClearFlagGMMsg();
       for(DWORD i=0;i<dwNBR;i++)
       {
          DWORD dwID;
          unsigned char *pTime = NULL; 
          unsigned char *pName = NULL; 
          unsigned char *pMsg  = NULL; 
          GET_LONG( dwID );
          GET_STRING( pTime );
          GET_STRING( pName );
          GET_STRING( pMsg );

          V3_GMDlg::GetInstance()->UpdateGMMsgList(dwID,(char*)pTime,(char*)pName,(char*)pMsg);

          if(pTime)
             delete []pTime;
          pTime = NULL;

          if(pName)
             delete []pName;
          pName = NULL;

          if(pMsg)
             delete []pMsg;
          pMsg = NULL;
       }

       V3_GMDlg::GetInstance()->UpdateDisplayGMMsg();
       V3_GMDlg::GetInstance()->Display(NULL,true,8,0);
       V3_GMDlg::GetInstance()->UnlockList();
    }break;


    case RQ_RPStatus: 
    {
       TFCPacket *msg = Msg;
       BYTE  chRPStatus;
       GET_BYTE(chRPStatus);
       g_Var.wRPStatus = chRPStatus;
    }; 
    break;


    case RQ_RP_BroadCastPVP:
    {
       TFCPacket *msg = Msg;

       USHORT ushNBR;
       GET_WORD( ushNBR );

       V3_InteractionPVPDlg::GetInstance()->LockList();
       V3_InteractionPVPDlg::GetInstance()->ClearAllList();
       if(ushNBR > 0)
       {
          for(USHORT i=0;i<ushNBR;i++)
          {
             unsigned char uchID,uchType,uchEnable;
             unsigned char *pName = NULL; 
             unsigned char *pDesc  = NULL; 
             GET_BYTE( uchID );
             GET_BYTE( uchType );
             GET_STRING( pName );
             GET_STRING( pDesc );
             GET_BYTE( uchEnable );

             V3_InteractionPVPDlg::GetInstance()->UpdateAreneList(uchID,uchType,uchEnable,(char*)pName,(char*)pDesc);

             if(pName)
                delete []pName;
             pName = NULL;

             if(pDesc)
                delete []pDesc;
             pDesc = NULL;
          }
       }
       V3_InteractionPVPDlg::GetInstance()->UnlockList();
       V3_InteractionPVPDlg::GetInstance()->UpdateDisplayList();
    }break;

    case RQ_RP_BroadCastPVPStat:
    {
       TFCPacket *msg = Msg;

       USHORT ushNBR;
       GET_WORD( ushNBR );

       V3_InteractionPVPDlg::GetInstance()->LockList();
       if(ushNBR > 0)
       {
          for(USHORT i=0;i<ushNBR;i++)
          {
             unsigned char uchEnable,uchStatus,uchNbrMin,uchNbrSec;
             unsigned short ushLevelMin,ushLevelMax;
             GET_BYTE( uchEnable );
             GET_BYTE( uchStatus );
             GET_WORD( ushLevelMin );
             GET_WORD( ushLevelMax );
             GET_BYTE( uchNbrSec );
             GET_BYTE( uchNbrMin );
             V3_InteractionPVPDlg::GetInstance()->UpdateAreneListStat(i,uchEnable,uchStatus,ushLevelMin,ushLevelMax,uchNbrSec,uchNbrMin);
          }
       }
       V3_InteractionPVPDlg::GetInstance()->UnlockList();
       V3_InteractionPVPDlg::GetInstance()->UpdateDisplayListStat();
    }break;

    case RQ_RP_BroadCastRP:
    {
       char chHaveRP;
       TFCPacket *msg = Msg;

       USHORT ushNBR;
       long lPC,lLevel,iPoint;
       GET_LONG( lPC );
       GET_LONG( lLevel );
       GET_LONG( iPoint );
       GET_WORD( ushNBR );

       V3_InteractionRPDlg::GetInstance()->LockList();
       V3_InteractionRPDlg::GetInstance()->ClearAllList();

       V3_InteractionRPDlg::GetInstance()->UpdateInfo(lPC,lLevel,iPoint);


       if(ushNBR > 0)
       {
          for(USHORT i=0;i<ushNBR;i++)
          {
             int iID;
             unsigned char *pName = NULL; 
             unsigned char *pMsg  = NULL; 
             GET_LONG( iID );
             GET_STRING( pName );
             GET_STRING( pMsg );

             V3_InteractionRPDlg::GetInstance()->UpdateRPList(iID,(char*)pName,(char*)pMsg);

             if(pName)
                delete []pName;
             pName = NULL;

             if(pMsg)
                delete []pMsg;
             pMsg = NULL;
          }

          GET_BYTE( chHaveRP );

          if(chHaveRP > 0)
          {
             unsigned char *pSubject = NULL; 
             GET_STRING( pSubject );

             V3_InteractionRPDlg::GetInstance()->UpdateRPActive(chHaveRP,(char*)pSubject);

             int iPlID;
             USHORT ushNBRPl;
             GET_WORD( ushNBRPl );
             for(USHORT i=0;i<ushNBRPl;i++)
             {
                unsigned char *pName = NULL; 
                GET_LONG( iPlID );
                GET_STRING( pName );

                V3_InteractionRPDlg::GetInstance()->UpdatePLList(iPlID,(char*)pName);

                if(pName)
                   delete []pName;
                pName = NULL;
             }

             if(pSubject)
                delete []pSubject;
             pSubject = NULL;
          }
          else
          {
             V3_InteractionRPDlg::GetInstance()->UpdateRPActive(chHaveRP,NULL);
          }
       }
       
       V3_InteractionRPDlg::GetInstance()->UnlockList();
       V3_InteractionRPDlg::GetInstance()->UpdateDisplayList();
    }break;
 

    case RQ_RP_RejoindreRP:
    {
       TFCPacket *msg = Msg;
       DWORD sourceID = 0;
       BYTE *sourceName;
       GET_STRING( sourceName );
       V3_InteractionRPDlg::GetInstance()->AcceptPlayer((char*)sourceName);
       delete sourceName;       
    } break;

    case RQ_RP_InviteRP:
    {
       TFCPacket *msg = Msg;
       DWORD sourceID = 0;
       BYTE *sourceName;
       GET_STRING( sourceName );
       V3_InteractionRPDlg::GetInstance()->InvitePlayer((char*)sourceName);
       delete sourceName;
    } break;


    case RQ_QB_GetQuestList:
    {
       TFCPacket *msg = Msg;

       long lNBR;
       GET_LONG( lNBR );

       V3_QuestBookP1::GetInstance()->LockList();
       V3_QuestBookP1::GetInstance()->ClearAllList();
       if(lNBR > 0)
       {
          for(USHORT i=0;i<lNBR;i++)
          {
             USHORT ushQuestID,ushQuestLevel;
             BYTE   chStatus;
             unsigned char *pName = NULL; 

             GET_WORD( ushQuestID);
             GET_WORD( ushQuestLevel);
             GET_BYTE( chStatus);
             GET_STRING( pName );

             V3_QuestBookP1::GetInstance()->UpdateQuestList(ushQuestID,ushQuestLevel,chStatus,(char*)pName);

             if(pName)
                delete []pName;
             pName = NULL;
          }
       }
       V3_QuestBookP1::GetInstance()->UnlockList();
       V3_QuestBookP1::GetInstance()->UpdateDisplayList();
       if(!V3_QuestBookP1::GetInstance()->IsShown())
          V3_QuestBookP1::GetInstance()->Display(NULL);
    }break;

    case RQ_QB_GetQuestMsg:
    {
       TFCPacket *msg = Msg;

       unsigned char *pMsg = NULL; 
       USHORT ushUniqueID;
       GET_WORD( ushUniqueID );
       GET_STRING( pMsg );

       V3_QuestBookP1::GetInstance()->UpdateQuestMessage(ushUniqueID,(char*)pMsg);

       if(pMsg)
          delete []pMsg;
       pMsg = NULL;

    }break;

    case RQ_QB_GetActiveQuest:
    {
       TFCPacket *msg = Msg;

       long lNBR;
       GET_LONG( lNBR );

       V3_QuestBookP2::GetInstance()->LockList();
       V3_QuestBookP2::GetInstance()->ClearAllList();
       if(lNBR > 0)
       {
          for(USHORT i=0;i<lNBR;i++)
          {
             USHORT ushQuestID,ushQuestLevel;
             BYTE   chStep,chNbrStep;
             unsigned char *pName = NULL; 
             unsigned char *pMsg  = NULL; 

             GET_WORD( ushQuestID);
             GET_WORD( ushQuestLevel);
             GET_BYTE( chStep);
             GET_STRING( pName );
             GET_BYTE( chNbrStep);

             std::vector<std::string> aStepMsg;
             if(chNbrStep > 0)
             {
                for(BYTE s=0;s<chNbrStep;s++)
                {
                   GET_STRING( pMsg );
                   std::string strTmp = (char*)pMsg;
                   aStepMsg.push_back(strTmp);
                   if(pMsg){ delete []pMsg;  pMsg = NULL;}
                }
             }
             if(ushQuestID >0)
               V3_QuestBookP2::GetInstance()->UpdateQuestList(ushQuestID,ushQuestLevel,chStep,(char*)pName,aStepMsg);

             if(pName)
                delete []pName;
             pName = NULL;
          }
       }
       V3_QuestBookP2::GetInstance()->UnlockList();
       V3_QuestBookP2::GetInstance()->UpdateDisplayList();
       if(!V3_QuestBookP2::GetInstance()->IsShown())
         V3_QuestBookP2::GetInstance()->Display(NULL);  
    }break;

    case RQ_QB_GetQuestListComplete:
    {
       TFCPacket *msg = Msg;

       long lNBR;
       GET_LONG( lNBR );

       V3_QuestBookP3::GetInstance()->LockList();
       V3_QuestBookP3::GetInstance()->ClearAllList();
       if(lNBR > 0)
       {
          for(USHORT i=0;i<lNBR;i++)
          {
             USHORT ushQuestID,ushQuestLevel;
             unsigned char *pName = NULL; 

             GET_WORD( ushQuestID);
             GET_WORD( ushQuestLevel);
             GET_STRING( pName );
             V3_QuestBookP3::GetInstance()->UpdateQuestList(ushQuestID,ushQuestLevel,(char*)pName);

             if(pName)
                delete []pName;
             pName = NULL;
          }
       }
       V3_QuestBookP3::GetInstance()->UnlockList();
       V3_QuestBookP3::GetInstance()->UpdateDisplayList();
       if(!V3_QuestBookP3::GetInstance()->IsShown())
          V3_QuestBookP3::GetInstance()->Display(NULL); 
    }break;

    case RQ_QB_StopQuest:
    {
       TFCPacket *msg = Msg;

       unsigned char *pMsg = NULL; 
       char chStatus = 0;
       GET_BYTE( chStatus );

       if(chStatus == 1)
       {
          TFCPacket Send;
          Send << (RQ_SIZE)RQ_QB_GetActiveQuest;
          SEND_PACKET(Send);
       }
       
       if(pMsg)
          delete []pMsg;
       pMsg = NULL;
    }break;

    
    case RQ_ARENA_SendTakeStatus :
    {
       TFCPacket *msg = Msg;
       char chPC;
       char chStatus;

       GET_BYTE( chPC );
       GET_BYTE( chStatus );

       V3_ArenePointsDlg::GetInstance()->UpdateFlagProgress(chStatus,chPC);
    }
    break;


    case RQ_ARENA1_GetWaitPlayerList:
    {
       TFCPacket *msg = Msg;

       long  lArenaID;
       long  lArenaType;
       short shStayMin;
       short shStaySec;
       short shLevelMin,shLevelMax,nbrPl;

       GET_LONG( lArenaID );
       GET_LONG( lArenaType );
       GET_WORD( shStayMin );
       GET_WORD( shStaySec );
       GET_WORD( shLevelMin );
       GET_WORD( shLevelMax );
       GET_WORD( nbrPl );

       V3_Arene1WaitDlg::GetInstance()->LockList();
       V3_Arene1WaitDlg::GetInstance()->ClearAllList();

       V3_Arene1WaitDlg::GetInstance()->UpdateInfoWait(lArenaID,lArenaType,shStayMin,shStaySec,shLevelMin,shLevelMax);


       if(nbrPl > 0)
       {
          for(USHORT i=0;i<nbrPl;i++)
          {
             int iPlID ;
             unsigned char *pName = NULL; 
             GET_LONG( iPlID );
             GET_STRING( pName );

             V3_Arene1WaitDlg::GetInstance()->UpdatePLList(iPlID,(char*)pName);

             if(pName)
                delete []pName;
             pName = NULL;
          }
       }

       V3_Arene1WaitDlg::GetInstance()->UnlockList();
       V3_Arene1WaitDlg::GetInstance()->UpdateDisplayList();
       if(nbrPl > 0)
          V3_Arene1WaitDlg::GetInstance()->Display(NULL);
       else
          V3_Arene1WaitDlg::GetInstance()->Hide();

    }break;
 
    case RQ_ARENA1_UpdateTimeBS :
    {
       TFCPacket *msg = Msg;

       long lAreneID;
       char chWitch;
       long  lArenaType;
       short shStayMin;
       short shStaySec;

       GET_LONG( lAreneID );
       GET_BYTE( chWitch );
       GET_LONG( lArenaType );
       GET_WORD( shStayMin );
       GET_WORD( shStaySec );

       if(chWitch == 1)
       {
          V3_Arene1WaitDlg::GetInstance()->UpdateTimeStay(shStayMin,shStaySec);
          V3_ArenePointsDlg::GetInstance()->UpdateInfoPlay(lAreneID,lArenaType,0,0,shStayMin,shStaySec);
       }
       else if(chWitch == 2)
       {
          if(V3_Arene1WaitDlg::GetInstance()->IsShown())
             V3_Arene1WaitDlg::GetInstance()->Hide();

          char chScoreBleue;
          char chScoreRed;
          GET_BYTE( chScoreBleue );
          GET_BYTE( chScoreRed );

          V3_ArenePointsDlg::GetInstance()->UpdateInfoPlay(lAreneID,lArenaType,chScoreBleue,chScoreRed,shStayMin,shStaySec);
       }
    }
    break;

    case RQ_ARENA1_UpdatePlayStat:
    {
       bool bSetTimer = true;
       if(V3_Arene1PlayDlg::GetInstance()->IsShown())
          bSetTimer = false;

       TFCPacket *msg = Msg;

       int iArenaID,iArenaType;
       short shStayMin,shStaySec;
       bool bOnList;
       short shLevelMin,shLevelMax,nbrPl,shPointB,shPointR;

       GET_LONG( iArenaID );
       GET_LONG( iArenaType );
       GET_WORD( shStayMin );
       GET_WORD( shStaySec );
       GET_BYTE( bOnList );
       GET_WORD( shLevelMin );
       GET_WORD( shLevelMax );
       GET_WORD( shPointB );
       GET_WORD( shPointR );
       GET_WORD( nbrPl );

       V3_Arene1PlayDlg::GetInstance()->LockList();
       V3_Arene1PlayDlg::GetInstance()->ClearAllList();
       V3_Arene1PlayDlg::GetInstance()->UpdateInfo(iArenaID,iArenaType,shStayMin,shStaySec,bOnList,shLevelMin,shLevelMax,shPointB,shPointR,bSetTimer);


       if(nbrPl > 0)
       {
          for(USHORT i=0;i<nbrPl;i++)
          {
             int iPlID ;
             unsigned char *pName = NULL; 
             short shTeamID,shFlag,shKill,shDead,PlayerPoints;
             GET_LONG( iPlID );
             GET_STRING( pName );
             GET_WORD( shTeamID );
             GET_WORD( shFlag );
             GET_WORD( shKill );
             GET_WORD( shDead );
             GET_WORD( PlayerPoints );
             

             V3_Arene1PlayDlg::GetInstance()->UpdatePLList(iPlID,(char*)pName,shFlag,shKill,shDead,shTeamID,PlayerPoints);

             if(pName)
                delete []pName;
             pName = NULL;
          }
       }
       V3_Arene1PlayDlg::GetInstance()->UnlockList();
       V3_Arene1PlayDlg::GetInstance()->UpdateDisplayList();
       if(!V3_Arene1PlayDlg::GetInstance()->IsShown())
         V3_Arene1PlayDlg::GetInstance()->Display(NULL);
    }break;



    

    case RQ_SvrNPC:
       {
          DWORD dwPartCur;
          DWORD dwPartMax;
          DWORD dwPartNbr;
          DWORD dwPartSplit;
          
          TFCPacket *msg = Msg;
          
          GET_LONG( dwPartCur );
          GET_LONG( dwPartMax );
          GET_LONG( dwPartNbr );
          GET_LONG( dwPartSplit );
          
          
          bool bProcess = true;
          V3_GMDlg::GetInstance()->LockList();
          if(dwPartCur == 0)
          {
             V3_GMDlg::GetInstance()->SetWaitWhat(6);
             V3_GMDlg::GetInstance()->ClearFlagSkinList();
          }
          else
          {
             //si on recoit des packet de ce qui nes pas demasnder un les flush...
             if(V3_GMDlg::GetInstance()->GetWaitWhat()!= 6)
               bProcess = false;
          }
          
          if(bProcess)
          {
             for(DWORD i=0;i<dwPartNbr;i++)
             {
                unsigned short ushID;
                unsigned char *pName = NULL; 
                GET_WORD( ushID );
                GET_STRING( pName );

                //sprintf_s(strtoto,1024,"%d %s\n",ushID,pName);
                //OutputDebugString(strtoto);
                
                V3_GMDlg::GetInstance()->UpdateQuestFlagSkinList(dwPartCur*dwPartSplit+i,ushID,(char*)pName);
                
                if(pName)
                   delete []pName;
                pName = NULL;
             }
             
             if(dwPartCur == dwPartMax-1)   
             {
                //la derniere part, on display la page des liste...
                V3_GMDlg::GetInstance()->UpdateDisplayGM();
                V3_GMDlg::GetInstance()->Display(NULL,true,6,0);
                V3_GMDlg::GetInstance()->SetWaitWhat(-1);
             }
          }
          V3_GMDlg::GetInstance()->UnlockList();
    }break;
    
    case RQ_SvrSpellList:
    {
       DWORD dwPartCur;
       DWORD dwPartMax;
       DWORD dwPartNbr;
       DWORD dwPartSplit;
       
       TFCPacket *msg = Msg;
       
       GET_LONG( dwPartCur );
       GET_LONG( dwPartMax );
       GET_LONG( dwPartNbr );
       GET_LONG( dwPartSplit );
       

       bool bProcess = true;
       V3_GMDlg::GetInstance()->LockList();
       
       if(dwPartCur == 0)
       {
          V3_GMDlg::GetInstance()->SetWaitWhat(5);
          V3_GMDlg::GetInstance()->ClearFlagSkinList();
       }
       else
       {
          //si on recoit des packet de ce qui nes pas demasnder un les flush...
          if(V3_GMDlg::GetInstance()->GetWaitWhat()!= 5)
             bProcess = false;
       }
        
       if(bProcess)
       {
          for(DWORD i=0;i<dwPartNbr;i++)
          {
             unsigned short ushID;
             unsigned char *pName = NULL; 
             GET_WORD( ushID );
             GET_STRING( pName );
             
             V3_GMDlg::GetInstance()->UpdateQuestFlagSkinList(dwPartCur*dwPartSplit+i,ushID,(char*)pName);
             
             if(pName)
                delete []pName;
             pName = NULL;
          }
          
          if(dwPartCur == dwPartMax-1)   
          {
             //la derniere part, on display la page des liste...
             V3_GMDlg::GetInstance()->UpdateDisplayGM();
             V3_GMDlg::GetInstance()->Display(NULL,true,5,0);
             V3_GMDlg::GetInstance()->SetWaitWhat(-1);
          }
       }
       V3_GMDlg::GetInstance()->UnlockList();

    }break;

    case RQ_SvrMonsterList:
       {
          DWORD dwPartCur;
          DWORD dwPartMax;
          DWORD dwPartNbr;
          DWORD dwPartSplit;
          
          TFCPacket *msg = Msg;
          
          GET_LONG( dwPartCur );
          GET_LONG( dwPartMax );
          GET_LONG( dwPartNbr );
          GET_LONG( dwPartSplit );
          
          bool bProcess = true;
          V3_GMDlg::GetInstance()->LockList();
          if(dwPartCur == 0)
          {
             V3_GMDlg::GetInstance()->SetWaitWhat(4);
             V3_GMDlg::GetInstance()->ClearFlagSkinList();
          }
          else
          {
             //si on recoit des packet de ce qui nes pas demasnder un les flush...
             if(V3_GMDlg::GetInstance()->GetWaitWhat()!= 4)
                bProcess = false;
          }
          
          if(bProcess)
          {
             for(DWORD i=0;i<dwPartNbr;i++)
             {
                unsigned short ushID;
                unsigned char *pName = NULL; 
                GET_WORD( ushID );
                GET_STRING( pName );
                

                V3_GMDlg::GetInstance()->UpdateQuestFlagSkinList(dwPartCur*dwPartSplit+i,ushID,(char*)pName);
                
                if(pName)
                   delete []pName;
                pName = NULL;
             }
             
             if(dwPartCur == dwPartMax-1)   
             {
                //la derniere part, on display la page des liste...
                V3_GMDlg::GetInstance()->UpdateDisplayGM();
                V3_GMDlg::GetInstance()->Display(NULL,true,4,0);
                V3_GMDlg::GetInstance()->SetWaitWhat(-1);
             }
          }
          V3_GMDlg::GetInstance()->UnlockList();
    }break;

    case RQ_SvrItemsList:
    {
       DWORD dwPartCur;
       DWORD dwPartMax;
       DWORD dwPartNbr;
       DWORD dwPartSplit;
       
       TFCPacket *msg = Msg;
       
       GET_LONG( dwPartCur );
       GET_LONG( dwPartMax );
       GET_LONG( dwPartNbr );
       GET_LONG( dwPartSplit );

       
       
       bool bProcess = true;
       V3_GMDlg::GetInstance()->LockList();
       if(dwPartCur == 0)
       {
          V3_GMDlg::GetInstance()->SetWaitWhat(3);
          V3_GMDlg::GetInstance()->ClearFlagSkinList();
       }
       else
       {
          //si on recoit des packet de ce qui nes pas demasnder un les flush...
          if(V3_GMDlg::GetInstance()->GetWaitWhat()!= 3)
          {
             bProcess = false;
          }
       }
       
       if(bProcess)
       {
          for(DWORD i=0;i<dwPartNbr;i++)
          {
             unsigned short ushID;
             unsigned char *pName = NULL; 
             GET_WORD( ushID );
             GET_STRING( pName );
             
             V3_GMDlg::GetInstance()->UpdateQuestFlagSkinList(dwPartCur*dwPartSplit+i,ushID,(char*)pName);
             
             if(pName)
                delete []pName;
             pName = NULL;
          }
          
          if(dwPartCur == dwPartMax-1)   
          {
             //la derniere part, on display la page des liste...
             V3_GMDlg::GetInstance()->UpdateDisplayGM();
             V3_GMDlg::GetInstance()->Display(NULL,true,3,0);
             V3_GMDlg::GetInstance()->SetWaitWhat(-1);
          }
       }
       V3_GMDlg::GetInstance()->UnlockList();
    }break;

    case RQ_SvrMonsterSkin:
    {
       DWORD dwPartCur;
       DWORD dwPartMax;
       DWORD dwPartNbr;
       DWORD dwPartSplit;
       
       TFCPacket *msg = Msg;
       
       GET_LONG( dwPartCur );
       GET_LONG( dwPartMax );
       GET_LONG( dwPartNbr );
       GET_LONG( dwPartSplit );

       
       
       bool bProcess = true;
       V3_GMDlg::GetInstance()->LockList();
       if(dwPartCur == 0)
       {
          V3_GMDlg::GetInstance()->SetWaitWhat(2);
          V3_GMDlg::GetInstance()->ClearFlagSkinList();
       }
       else
       {
          //si on recoit des packet de ce qui nes pas demasnder un les flush...
          if(V3_GMDlg::GetInstance()->GetWaitWhat()!= 2)
             bProcess = false;
       }

       if(bProcess)
       {
          for(DWORD i=0;i<dwPartNbr;i++)
          {
             unsigned short ushID;
             unsigned char *pName = NULL; 
             GET_WORD( ushID );
             GET_STRING( pName );
             
             V3_GMDlg::GetInstance()->UpdateQuestFlagSkinList(dwPartCur*dwPartSplit+i,ushID,(char*)pName);
             
             if(pName)
                delete []pName;
             pName = NULL;
          }
          
          if(dwPartCur == dwPartMax-1)   
          {
             //la derniere part, on display la page des liste...
             V3_GMDlg::GetInstance()->UpdateDisplayGM();
             V3_GMDlg::GetInstance()->Display(NULL,true,2,0);
             V3_GMDlg::GetInstance()->SetWaitWhat(-1);
          }
       }
       V3_GMDlg::GetInstance()->UnlockList();
    }break;
       
   case RQ_SvrQuestFlagList:
   {
      DWORD dwPartCur;
      DWORD dwPartMax;
      DWORD dwPartNbr;
      DWORD dwPartSplit;
      
      TFCPacket *msg = Msg;

      GET_LONG( dwPartCur );
      GET_LONG( dwPartMax );
      GET_LONG( dwPartNbr );
      GET_LONG( dwPartSplit );

      
      bool bProcess = true;
      V3_GMDlg::GetInstance()->LockList();
      if(dwPartCur == 0)
      {
         V3_GMDlg::GetInstance()->SetWaitWhat(1);
         V3_GMDlg::GetInstance()->ClearFlagSkinList();
      }
      else
      {
         //si on recoit des packet de ce qui nes pas demasnder un les flush...
         if(V3_GMDlg::GetInstance()->GetWaitWhat()!= 1)
            bProcess = false;
      }

      if(bProcess)
      {
         for(DWORD i=0;i<dwPartNbr;i++)
         {
            unsigned short ushID;
            unsigned char *pName = NULL; 
            GET_WORD( ushID );
            GET_STRING( pName );

            V3_GMDlg::GetInstance()->UpdateQuestFlagSkinList(dwPartCur*dwPartSplit+i,ushID,(char*)pName);

            if(pName)
               delete []pName;
            pName = NULL;
         }

         if(dwPartCur == dwPartMax-1)   
         {
            //la derniere part, on display la page des liste...
            V3_GMDlg::GetInstance()->UpdateDisplayGM();
            V3_GMDlg::GetInstance()->Display(NULL,true,1,0);
            V3_GMDlg::GetInstance()->SetWaitWhat(-1);
         }
      }
      V3_GMDlg::GetInstance()->UnlockList();
      
    }break;

    case RQ_AttackMode:
    {
       DWORD dwAsk, dwSet;
       
       TFCPacket *msg = Msg;
       GET_LONG( dwAsk );
       GET_LONG( dwSet );

       if(dwSet >0)
       {
          Player.AttackMode = dwAsk;
          UpdateAttackMode(false);
       }
       
       
    }break;


      

    case RQ_NM_DeathStatus:
      {

		   TFCPacket *msg = Msg;

         BYTE  chStatus;

         GET_BYTE(chStatus);

		   V3_LifeDlg::QueueDeadStatus(chStatus);
      }
    break;
    case RQ_NM_DeathProgress: 
      {

		   TFCPacket *msg = Msg;

         WORD  wTimeCurrent;
         WORD  wTimeTotal;
         BYTE  chCanResurect;

		   GET_WORD(wTimeCurrent);
		   GET_WORD(wTimeTotal);
         GET_BYTE(chCanResurect);

		   V3_LifeDlg::QueueDeadInfo(wTimeCurrent,wTimeTotal,chCanResurect);
	   }; 
    break;
    case RQ_NM_XPScrollProgress:
      {
         
         TFCPacket *msg = Msg;
         
         BYTE  chStatus;
         WORD  wMinTime;
         GET_BYTE(chStatus);
         GET_WORD(wMinTime);

         if(chStatus == 0x00)
            g_Var.wDisplayXPScroll = 1;
         else
            g_Var.wDisplayXPScroll = 0xFFFF;

         g_Var.wDisplayXPScrollT = wMinTime;

         //Need to set the variable to display icons....
      }
    break;
    case RQ_NM_ORScrollProgress:
       {
          
          TFCPacket *msg = Msg;
          
          BYTE  chStatus;
          WORD  wMinTime;
          GET_BYTE(chStatus);
          GET_WORD(wMinTime);
          
          if(chStatus == 0x00)
             g_Var.wDisplayORScroll = 1;
          else
             g_Var.wDisplayORScroll = 0xFFFF;

          g_Var.wDisplayORScrollT = wMinTime;
          
          //Need to set the variable to display icons....
       }
    break;
    

    case RQ_NM_GetProfession:
    {
       
       TFCPacket *msg = Msg;

       Player.Lock();
       Player.DeleteProfession();
       
       LPUSER_PROFESSION lpUserProf = new USER_PROFESSION;
       
       GET_LONG( lpUserProf->iNbrProfession );
       GET_LONG( lpUserProf->iNbrFormule );

       //recup tous les profesion name
       int i=0;
       for(i=0;i<lpUserProf->iNbrProfession;i++)
       {
          LPPROFESSION_NAME pProfName = new PROFESSION_NAME;
          
          GET_WORD  ( pProfName->ushProfSkill);

          pProfName->chProfessionID = i;
          if(i == 0)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[229]);
          else if(i == 1)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[230]);
          else if(i == 2)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[231]);
          else if(i == 3)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[232]);
          else if(i == 4)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[233]);
          else if(i == 5)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[234]);

          lpUserProf->tlProfession.AddToTail( pProfName );
       }

       //recup toutes les formules
       for(i=0;i<lpUserProf->iNbrFormule;i++)
       {
          LPFORMULE_INFO pFormule = new FORMULE_INFO;
 
          GET_BYTE  ( pFormule->chProfessionID);
          GET_WORD  ( pFormule->ushID);
          GET_WORD  ( pFormule->ushSkillLevel);
          GET_WORD  ( pFormule->ushSkinID);
          GET_STRING( pFormule->pName );
          GET_LONG  ( pFormule->iNbrReq );

          for(int j=0;j<pFormule->iNbrReq;j++)
          {
             LPFORMULE_REQ pFormuleReq = new FORMULE_REQ;
             GET_STRING( pFormuleReq->pName );
             GET_LONG  ( pFormuleReq->lNbrNeed );
             GET_LONG  ( pFormuleReq->lNbrHave );
             pFormule->tlFormuleReq.AddToTail( pFormuleReq );
          }

          if(pFormule->chProfessionID == 0 && pFormule->ushID == 0 && pFormule->ushSkillLevel == 0 &&
             pFormule->ushSkinID == 0 && pFormule->iNbrReq == 0)
          {
            //cette formule nexiste plus, on l;<ajoute pas...
            delete pFormule;
            pFormule = NULL;
          }
          else
             lpUserProf->tlFormule.AddToTail( pFormule );
       }
       Player.tlProfessionAll.AddToTail( lpUserProf );

       
       Player.Unlock();
       V3_ProfessionDlg::GetInstance()->UpdateProfessionList();
       
    }
    break;

    case RQ_NM_SendMakeFormule:
    {
       
       TFCPacket *msg = Msg;

       Player.Lock();
       Player.DeleteProfession();
       
       LPUSER_PROFESSION lpUserProf = new USER_PROFESSION;
       
       GET_LONG( lpUserProf->iNbrProfession );
       GET_LONG( lpUserProf->iNbrFormule );

       //recup tous les profesion name
       int i=0;
       for(i=0;i<lpUserProf->iNbrProfession;i++)
       {
          LPPROFESSION_NAME pProfName = new PROFESSION_NAME;
          
          GET_WORD  ( pProfName->ushProfSkill);

          pProfName->chProfessionID = i;
          if(i == 0)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[229]);
          else if(i == 1)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[230]);
          else if(i == 2)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[231]);
          else if(i == 3)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[232]);
          else if(i == 4)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[233]);
          else if(i == 5)
             sprintf_s(pProfName->strName,50,"%s",g_GUILocalString[234]);

          lpUserProf->tlProfession.AddToTail( pProfName );
       }

       //recup toutes les formules
       for(i=0;i<lpUserProf->iNbrFormule;i++)
       {
          LPFORMULE_INFO pFormule = new FORMULE_INFO;

          GET_BYTE  ( pFormule->chProfessionID);
          GET_WORD  ( pFormule->ushID);
          GET_WORD  ( pFormule->ushSkillLevel);
          GET_WORD  ( pFormule->ushSkinID);
          GET_STRING( pFormule->pName );
          GET_LONG  ( pFormule->iNbrReq );

          for(int j=0;j<pFormule->iNbrReq;j++)
          {
             LPFORMULE_REQ pFormuleReq = new FORMULE_REQ;
             GET_STRING( pFormuleReq->pName );
             GET_LONG  ( pFormuleReq->lNbrNeed );
             GET_LONG  ( pFormuleReq->lNbrHave );
             pFormule->tlFormuleReq.AddToTail( pFormuleReq );
          }
          lpUserProf->tlFormule.AddToTail( pFormule );
       }
       Player.tlProfessionAll.AddToTail( lpUserProf );

       Player.Unlock();
       V3_ProfessionDlg::GetInstance()->UpdateProfessionListP();
       
    }

    break;

    
       
    case 10001: {
       // Attaque Reussit
       unsigned long IDAttack;
       unsigned long IDDefend;
       char TypeAttack;
       char TypeDefend;
       char pHp;
       char CombatMode;
       short AXPos;
       short AYPos;
       short DXPos;
       short DYPos;
       
       Msg->Get((long *)&IDAttack);
       Msg->Get((long *)&IDDefend);
       Msg->Get((char *)&TypeAttack);
       Msg->Get((char *)&TypeDefend);
       Msg->Get((char *)&pHp);
       Msg->Get((char *)&CombatMode);
       Msg->Get((short *)&AXPos);
       Msg->Get((short *)&AYPos);
       Msg->Get((short *)&DXPos);
       Msg->Get((short *)&DYPos);
       
       
       if (IDAttack == Player.ID) {
          TalkToID = NULL;
          
          
       }
       
       Objects.Lock(61);
       if (IDAttack != Player.ID && !Objects.FoundID(IDAttack)) {
          TFCPacket Send;
          
          Send << (RQ_SIZE)71;
          Send << (long)IDAttack;
          Send << (short)AXPos;
          Send << (short)AYPos;
          
          SEND_PACKET(Send);
       }
       
       if (IDDefend != Player.ID && !Objects. FoundID(IDDefend)) {
          TFCPacket Send;
          
          Send << (RQ_SIZE)71;
          Send << (long)IDDefend;
          Send << (short)DXPos;
          Send << (short)DYPos;
          
          SEND_PACKET(Send);
       }
       Objects.Unlock(61);
       
       Objects.Lock(63);
       Objects.MoveObject(IDAttack, AXPos, AYPos, FALSE, 0, -1,-1,-1, 0, 0, 0,false,false,0);
       Objects.MoveObject(IDDefend, DXPos, DYPos, FALSE, 0, -1,-1,-1, 0, 0, 0,false,false,0);
       Objects.Unlock(63);
       
       if (IDAttack == Player.ID) {
          Objects.Lock(64);
          Objects.PlAttack(IDDefend, pHp);
          Objects.GetMainObject()->Chose = 1;
          Objects.GetMainObject()->Speed = 0;
          Objects.GetMainObject()->AttSpeed = 0;
          Objects.Unlock(64);
          signed char X, Y;
          Objects.Lock(65);
          Objects.Found(&X, &Y, IDDefend);
          if (X < 0 && Y == 0)
             Objects.GetMainObject()->Direction = 4;
          else if (X > 0 && Y == 0)
             Objects.GetMainObject()->Direction = 6;
          else if (X == 0 && Y > 0)
             Objects.GetMainObject()->Direction = 2;
          else if (X == 0 && Y < 0)
             Objects.GetMainObject()->Direction = 8;
          else if (X < 0 && Y > 0)
             Objects.GetMainObject()->Direction = 1;
          else if (X < 0 && Y < 0)
             Objects.GetMainObject()->Direction = 7;
          else if (X > 0 && Y > 0)
             Objects.GetMainObject()->Direction = 3;
          else if (X > 0 && Y < 0)
             Objects.GetMainObject()->Direction = 9;
          else
             Objects.GetMainObject()->Direction = 1;
          Objects.Unlock(65);
          
          Err = true;
       } else {
          Objects.Lock(66);
          Objects.SetAttack(IDAttack, IDDefend, 1, 1, pHp);
          //Objects.Anim(IDAttack, 1);
          //Objects.SetDirection(IDAttack, IDDefend);
          Objects.Unlock(66);
       }
       
        } break;
        
        case 10002: {
           // Attaque Misser
           unsigned long IDAttack;
           unsigned long IDDefend;
           short AXPos;
           short AYPos;
           short DXPos;
           short DYPos;
           
           Msg->Get((long *)&IDAttack);
           Msg->Get((long *)&IDDefend);
           Msg->Get((short *)&AXPos);
           Msg->Get((short *)&AYPos);
           Msg->Get((short *)&DXPos);
           Msg->Get((short *)&DYPos);
           
           if (IDAttack == Player.ID) {
              TalkToID = NULL;
              
           }
           
           
           Objects.Lock(67);
           if (IDAttack != Player.ID && !Objects.FoundID(IDAttack) ) {
              TFCPacket Send;
              
              Send << (RQ_SIZE)71;
              Send << (long)IDAttack;
              Send << (short)AXPos;
              Send << (short)AYPos;
              
              SEND_PACKET(Send);
           }
           
           if (IDDefend) {
              
              if (IDDefend && IDDefend != Player.ID && !Objects. FoundID(IDDefend)) {
                 TFCPacket Send;
                 
                 Send << (RQ_SIZE)71;
                 Send << (long)IDDefend;
                 Send << (short)DXPos;
                 Send << (short)DYPos;
                 
                 SEND_PACKET(Send);
              }
              Objects.Unlock(67);
              
              Objects.Lock(68);
              Objects.MoveObject(IDAttack, AXPos, AYPos, FALSE, 0, -1,-1,-1, 0, 0, 0,false,false,0);
              Objects.MoveObject(IDDefend, DXPos, DYPos, FALSE, 0, -1,-1,-1, 0 ,0 ,0,false,false,0);
              Objects.Unlock(68);
              
              if (IDAttack == Player.ID) {
                 Objects.Lock(69);
                 Objects.PlAttack(NULL, 0);
                 Objects.GetMainObject()->Chose = 1;
                 Objects.GetMainObject()->Speed = 0;
                 Objects.Unlock(69);
                 signed char X, Y;
                 if (IDDefend) {
                    Objects.Lock(70);
                    Objects.Found(&X, &Y, IDDefend);
                    if (X < 0 && Y == 0)
                       Objects.GetMainObject()->Direction = 4;
                    else if (X > 0 && Y == 0)
                       Objects.GetMainObject()->Direction = 6;
                    else if (X == 0 && Y > 0)
                       Objects.GetMainObject()->Direction = 2;
                    else if (X == 0 && Y < 0)
                       Objects.GetMainObject()->Direction = 8;
                    else if (X < 0 && Y > 0)
                       Objects.GetMainObject()->Direction = 1;
                    else if (X < 0 && Y < 0)
                       Objects.GetMainObject()->Direction = 7;
                    else if (X > 0 && Y > 0)
                       Objects.GetMainObject()->Direction = 3;
                    else if (X > 0 && Y < 0)
                       Objects.GetMainObject()->Direction = 9;
                    else
                       Objects.GetMainObject()->Direction = 1;

                    Objects.Unlock(70);
                 }
                 Err = true;
              } else {
                 Objects.Lock(71);
                 Objects.SetAttack(IDAttack, IDDefend, 1, 0, -1);
                 Objects.Unlock(71);
              }
           } else {
              if (IDAttack == Player.ID) {
                 Objects.PlAttack(NULL, 0);
                 Objects.GetMainObject()->Chose = 1;
                 Objects.GetMainObject()->Speed = 0;
              } else {
                 Objects.SetAttack(IDAttack, IDDefend, 1, 0, -1, 0);
              }
              Objects.Unlock(67);
           }
           
        } break;
        
        case RQ_IndirectTalk: PacketHandling::IndirectTalk(Msg); break;
           
        case RQ_DirectedTalk: break;
        case RQ_DirectedTalkNoFeed: break;
           
        case RQ_ReturnToMenu: //RQ_ReturnToMenu
        {
           unsigned char Validation;
           Msg->Get((char *)&Validation);
           switch (Validation) 
           {
              case 0: // Account registred

                 Register = true;
                 //on dois reenvoyer le load du player au serveur...

                 Code13 = false;
                 EnterGame = false;

                 g_Var.inGame = false;
                 INGAME       = FALSE;


                 while(!g_bDrawThreadPaused)
                    Sleep(100);///NMNMNM_NEW_TEST
                 g_uiReloadForceTeleport = 1;
                 g_uiReloadForce = 1;
                 g_SaveGame.SuspendSaveThread(TRUE);
                 g_SaveGame.Save_SaveGame(); //On Reload, on Save avant de quitter...
                 


                 Player.tlBackpack.Lock("reload");
                 Player.tlBackpack.ToHead();
                 while( Player.tlBackpack.QueryNext() )
                 {
                    BAG_ITEM *obj = Player.tlBackpack.GetObject();
                    obj->chIName[0] = 0;
                    Player.tlBackpack.DeleteObject();
                 }
                 Player.tlBackpack.Unlock("reload");

                 for(int ii=0;ii<17;ii++)
                 {
                    if( Player.lpbEquipped[ ii ] != NULL )
                    {
                       delete Player.lpbEquipped[ ii ];
                       Player.lpbEquipped[ ii ] = NULL;
                    }
                 }

                 TFCPacket Send;
                 Send << (short)RQ_PutPlayerInGame;
                 Send << (char)strlen(Custom.m_strName[Custom.m_shSelectedReloadPlayer]);
                 Send << (char *)Custom.m_strName[Custom.m_shSelectedReloadPlayer];
                 Send << (long)Player.lKey;
                 strcpy_s(Player.Name,256, Custom.m_strName[Custom.m_shSelectedReloadPlayer]);
                 Player.Level = Custom.m_shLevel[Custom.m_shSelectedReloadPlayer];
                 Objects.GetMainObject()->Type = Custom.m_shRace[Custom.m_shSelectedReloadPlayer];


                 g_Var.wDisplayXPScroll  = 0;
                 g_Var.wDisplayXPScrollT = 0;
                 g_Var.wDisplayORScroll  = 0;
                 g_Var.wDisplayORScrollT = 0;
                 

                 V3_ChatLogDlg::GetInstance()->ClearListLogs();
                 V3_MacroDlg::GetInstance()->ReloadReset();
                 V3_InvDlg::GetInstance()->ClearItem();
                 V3_ChestDlg::GetInstance()->ClearItem();
                 V3_ChestNewDlg::GetInstance()->ClearItem();
                 V3_GuildChestDlg::GetInstance()->ClearItem();
                 V3_GuildChestNewDlg::GetInstance()->ClearItem();
                 V3_EffectStatusDlg::GetInstance()->DispellAllEffect();
                 V3_ChatDlg::GetInstance()->ClearChannelList();
                 Objects.Lock(0);
                 Objects.DeleteAll();
                 Objects.Unlock(0);
                 //++NMNMNM --[ Load Savegame Settings]
                 g_SaveGame.Load_SaveGame(Player.Account,Player.Name);
             
                 SEND_PACKET(Send);
                 
              break;
           }
        } break;
           
        case RQ_PutPlayerInGame: {

           {
              LPBYTE _trBuf = NULL; int _trN = 0;
              try { Msg->GetBuffer(_trBuf, _trN); } catch (...) {}
              char tb[96]; sprintf_s(tb, 96, "13: case entry, pktbytes=%d", _trN);
              EnterGameTrace(tb);
           }
           
           unsigned char ERR;
           Msg->Get((char *)&ERR);
           if (ERR != 0)
           {
              if (ERR == 7 && g_iPutPlayerBusyRetry < 4)
              {
                 g_iPutPlayerBusyRetry++;
                 g_dwPutPlayerSentAt = timeGetTime() - 2000;
                 char tb[96];
                 sprintf_s(tb, 96, "13: ERR=7 busy — retry %d/4", g_iPutPlayerBusyRetry);
                 EnterGameTrace(tb);
                 break;
              }
              WantPreGame = false;
              char tb[64];
              sprintf_s(tb, 64, "13: ERR=%u fatal", (unsigned)ERR);
              EnterGameTrace(tb);
              break;
           }

           WantPreGame = false;
           Sleep(200);
           unsigned long High = 0;
           unsigned long Low = 0;
           
           EnterGameTrace("13: ERR=0 OK");
           Msg->Get((long  *)&Player.FactionID);
           Msg->Get((long  *)&Player.ID);
           Msg->Get((short *)&Player.xPos);
           Msg->Get((short *)&Player.yPos);
           Msg->Get((short *)&Player.World);
           EnterGameTrace("13: faction/id/pos lus");

           Msg->Get((long *)&Player.Hp);
           Msg->Get((long *)&Player.MaxHp);
           Msg->Get((short *)&Player.Mana);
           Msg->Get((short *)&Player.MaxMana);
           EnterGameTrace("13: hp/mana lus");
           Msg->Get((long *)&High);
           Msg->Get((long *)&Low);
           Player.Exp = ((__int64)(High) << 32) + (__int64)(Low);
           Msg->Get((long *)&High);
           Msg->Get((long *)&Low);
           Player.Exp2Go = ((__int64)(High) << 32) + (__int64)(Low);
           Msg->Get((short *)&Player.Str);
           Msg->Get((short *)&Player.End);
           Msg->Get((short *)&Player.Agi);
           Msg->Get((short *)&Player.Wil);
           Msg->Get((short *)&Player.Wis);
           Msg->Get((short *)&Player.Int);
           Msg->Get((short *)&Player.Lck);
           Msg->Get((char *)&g_TimeStructure.Seconde);
           Msg->Get((char *)&g_TimeStructure.Minute);
           Msg->Get((char *)&g_TimeStructure.Hour);
           Msg->Get((char *)&g_TimeStructure.Day);
           Msg->Get((char *)&g_TimeStructure.Week);
           Msg->Get((char *)&g_TimeStructure.Month);
           Msg->Get((short *)&g_TimeStructure.Year);
           Msg->Get((long *)&Player.Gold);
           Msg->Get((short *)&Player.Level);
           Msg->Get((long *)&High);
           Msg->Get((long *)&Low);
           Msg->Get((char *)&Custom.gWinterrize);
           Player.ExpLastLevel = ((__int64)(High) << 32) + (__int64)(Low);

           EnterGameTrace("13: handler enter, stats parsed");

           // Auto-adaptation serveur Linux (entree en jeu) ? ENVOI PROACTIF DU 46.
           //
           // Le client Windows n'emet normalement le 46 (RQ_FromPreInGameToInGame) que dans le
           // handler TFCAddObject, c.-a-d. UNIQUEMENT a la reception du push ? inview ? du serveur.
           // Or ce push n'arrive qu'apres que le serveur a bascule in_game dans
           // StartAsyncFromPregameToGame()... lui-meme declenche par la reception du 46. Le serveur
           // Windows brise ce cycle en poussant l'inview tot (branche #ifdef _WIN32 de
           // AsyncRQFUNC_PutPlayerInGame) ; le serveur Linux ne le fait pas -> interblocage : le
           // perso reste bloque en preInGame=1 / in_game=0.
           //
           // On envoie donc le 46 ICI, des reception de l'opcode 13 (comme le client Linux), AVANT
           // l'ecran de loading local : ainsi le 46 part meme si le loading prend du temps. Cote
           // serveur, StartAsyncFromPregameToGame() bascule in_game PUIS pousse sac + statut +
           // inview (TFCAddObject). Inoffensif face a un serveur Windows : un 46 recu alors qu'on
           // est deja in_game retombe sur la branche no-op du handler (TFCMessagesHandler.cpp L1028).
           {
              TFCPacket SendEnter;
              SendEnter << (RQ_SIZE)RQ_FromPreInGameToInGame;
              SEND_PACKET(SendEnter);
              g_waitingEnterWorld46 = true;
              g_enterWorld46SentAt = timeGetTime();
              g_enterWorld46RetryCount = 0;
           }
           EnterGameTrace("13: 46 envoye (proactif)");

           // Force load these UI's
           g_UiInit = true;
           
           

           // Update the character sheet.
           EnterGameTrace("13: dlg DisplayHelp");    V3_DisplayHelpDlg::GetInstance();
           EnterGameTrace("13: dlg DisplaySpecial"); V3_DisplaySpecialDlg::GetInstance();
           EnterGameTrace("13: dlg Mp3");            V3_Mp3Dlg  ::GetInstance();
           EnterGameTrace("13: dlg ArenePoints");    V3_ArenePointsDlg::GetInstance();
           EnterGameTrace("13: dlg ItemInfo");       V3_ItemInfoDlg::GetInstance();
           EnterGameTrace("13: dlg Chest");          V3_ChestDlg::GetInstance();
           EnterGameTrace("13: dlg ChestNew");       V3_ChestNewDlg::GetInstance();
           EnterGameTrace("13: dlg GuildChest");     V3_GuildChestDlg::GetInstance();
           EnterGameTrace("13: dlg GuildChestNew");  V3_GuildChestNewDlg::GetInstance();
           EnterGameTrace("13: dlg AHVendre");       V3_AHVendreDlg::GetInstance();
           EnterGameTrace("13: dlg RTMap");          V3_RTMapDlg::GetInstance();
           EnterGameTrace("13: dlg GM");             V3_GMDlg::GetInstance();
           EnterGameTrace("13: dlg Options");        V3_OptionsDlg::GetInstance();
           EnterGameTrace("13: dlg Macro");          V3_MacroDlg::GetInstance();
           EnterGameTrace("13: dlg Spell");          V3_SpellDlg::GetInstance();
           EnterGameTrace("13: dlg Groupe");         V3_GroupeDlg::GetInstance();
           EnterGameTrace("13: dlg Profession");     V3_ProfessionDlg::GetInstance();
           EnterGameTrace("13: dlg Inv");            V3_InvDlg::GetInstance();
           EnterGameTrace("13: dlg Life");           V3_LifeDlg::GetInstance();
           EnterGameTrace("13: SpellList");          V3_SpellDlg::GetInstance()->RequestSpellList();
           EnterGameTrace("13: StatsUpdate");        V3_StatsDlg::GetInstance()->UpdateCharacterSheet("OK-TFCInGame");
           EnterGameTrace("13: dialogs+stats OK");

           if(g_uiReloadForce)
           {
              
              V3_ChestDlg        ::GetInstance()->Reload();
              V3_ChestNewDlg     ::GetInstance()->Reload();
              V3_GuildChestDlg   ::GetInstance()->Reload();
              V3_GuildChestNewDlg::GetInstance()->Reload();
              V3_OptionsDlg      ::GetInstance()->Reload();
              V3_InvDlg          ::GetInstance()->Reload();
              V3_StatsDlg        ::GetInstance()->Reload();
              V3_SpellDlg        ::GetInstance()->Reload();
              
              g_uiReloadForce = 0;
              TFCPacket Send2;
              Send2 << (RQ_SIZE)RQ_GetNearItems;
              SEND_PACKET(Send2);
           }

           EnterGameTrace("13: avant ClientInitialize");
           RootBoxUI::GetInstance()->ClientInitialize();
           EnterGameTrace("13: ClientInitialize OK");

           V3_InvDlg::GetInstance()->UpdateCharacterSheet();
           EnterGameTrace("13: InvUpdate OK");
           RootBoxUI::GetInstance()->SetSideMenuState(true);
           EnterGameTrace("13: SideMenu OK");
           
           // Get the skill list.
           TFCPacket sending;
           sending << (RQ_SIZE)RQ_GetSkillList;	
           SEND_PACKET( sending );
           //Sleep(50);

           IDirectXImage diLoading;
           try 
           {
              // Load.PCX for 800 and 1024
              char strLOADName[128];
              sprintf_s(strLOADName,128,"Images\\load%d%d.PCX",g_Global.GetDisplayW(),g_Global.GetDisplayH());
              if (diLoading.LoadImage(strLOADName,g_Global.GetDisplayW(),g_Global.GetDisplayH())) 
              {
              }
           } 
           catch (...) 
           {
           };

           int dwOffset = g_Var.vsfProgressB[0].GetHeight()+10;
           
           // CV2Sprite for Progressbar
           int dwX = (g_Global.GetScreenW()-g_Var.vsfProgressB[0].GetWidth())/2;
           V2SPRITEFX VsfFX;
           ZeroMemory(&VsfFX, sizeof(V2SPRITEFX));
           VsfFX.lpClipRect = new RECT; 
           VsfFX.lpClipRect->left   = dwX;
           VsfFX.lpClipRect->top    = g_Global.GetScreenH()-dwOffset;
           VsfFX.lpClipRect->right  = VsfFX.lpClipRect->left + g_Var.vsfProgressB[0].GetWidth();
           VsfFX.lpClipRect->bottom = VsfFX.lpClipRect->top  + g_Var.vsfProgressB[0].GetHeight();


           g_bEnterGamePart2Complete = FALSE;
           //::MessageBox(NULL,"Start Loadingpart2","",MB_OK);
           UINT ID;
           HANDLE hThread = NULL;
           while(!hThread)
           {
               hThread = (HANDLE)_beginthreadex( NULL, 0,  EntergamePart2, NULL, 0, &ID);
               Sleep(100);
           }
           
           {
              char tb[160];
              sprintf_s(tb,160,"13: avant loadloop smooth=%d part2=%d",
                        g_bSmoothFloorLoading?1:0, g_bEnterGamePart2Complete?1:0);
              EnterGameTrace(tb);
           }
           while(!g_bEnterGamePart2Complete || !g_bSmoothFloorLoading)
           {
              
              Sleep(50);
              if(diLoading)
                 DXDBlt(diLoading);
              
              g_Var.vsfProgressB[0].DrawSpriteN(dwX,g_Global.GetScreenH()-dwOffset, &VsfFX,&g_Var.vsfProgressB[1]);


              int dwXT =  (dwX+2)+(g_Var.wProgressCnt*7);
              g_Var.vsfProgressT[0].DrawSpriteN(dwXT,g_Global.GetScreenH()-dwOffset+2, &VsfFX,&g_Var.vsfProgressT[1]);
              DXDFlip();//attente avant entrer en jeux
              g_Var.wProgressCnt += g_Var.wProgressInc;
              if(g_Var.wProgressCnt >=37 || g_Var.wProgressCnt <1)
              {
                 g_Var.wProgressInc*=-1;
              }
           }
           if(diLoading)
              diLoading.Release();
          
           delete VsfFX.lpClipRect;
           DXDClear();

           g_SaveGame.SuspendSaveThread(FALSE);

          
           //::MessageBox(NULL,"END Loadingpart2","",MB_OK);

           
           switch (ERR) {
           case TFCCreateNewPlayer_CreatedPlayer: 
              // TODO
              break;
              
           case TFCCreateNewPlayer_AccountInGame:
              // TODO
              break;
              
           case TFCCreateNewPlayer_NoCredits:
              // TODO
              throw;
              break;
              
           case TFCCreateNewPlayer_TooManyAccounts:
              // TODO
              throw;
              break;
               
           case TFCCreateNewPlayer_PlayerAlreadyExists:
              // TODO
              throw;
              break;
              
           case TFCCreateNewPlayer_PlayerDontExists:
              // TODO
              throw;
              break;
              
           case TFCCreateNewPlayer_PlayerAlreadyGame:
              // TODO
              throw;
              break;
              
           case TFCCreateNewPlayer_InvalidNameSpec:
              // TODO
              throw;
              break;
              
           case 12:
              throw;
              break;
           }
           
           if ((!Player.Exp && FirstTime) || Custom.gfirstTimeAddOn) 
           {
              FirstTime = FALSE;
              if (Player.Exp == 0 || Custom.gfirstTimeAddOn)
                 Custom.gfirstTimeAddOn = false;
              V3_DisplayHelpDlg::GetInstance()->ShowHelp(0);
           }
           

           EnterGameTrace("13: loadloop terminee");

           Code13 = true;
           EnterGame = true;

           // Le 46 (RQ_FromPreInGameToInGame) a deja ete envoye en debut de handler, juste apres le
           // parsing des stats ? exactement comme le client Linux (T4CLoginSession.cpp:
           // ParsePutPlayerInGame -> SendPostEnterWorldPackets). C'est lui qui bascule in_game cote
           // serveur Linux et declenche le push sac/statut/inview.
           //
           // Apres l'entree, on tire RQ_GetNearItems (60), comme le client Linux apres la reponse 46.
           // (GetSkillList (39) a deja ete envoye plus haut.) On NE rejoue PAS GetStatus/ViewInv ici :
           // le client Linux ne le fait pas, et le serveur pousse deja ces donnees.
           TFCPacket Send;
           Send << (short)60;
           SEND_PACKET(Send);

           EnterGameTrace("13: 60 envoye, fin handler");
           
           
        } break; 
        
        case 10004: {
           signed char LIGHT;
           unsigned short X, Y, TYPE,MOBID;
           unsigned long ID,FRIENDLYID;
           unsigned char STATUS;
           char PHP;
           char CombatMode;
           char chHiddenInv2;
           Msg->Get((short *)&X);
           Msg->Get((short *)&Y);
           Msg->Get((long *)&FRIENDLYID);
           Msg->Get((short *)&MOBID);
           Msg->Get((short *)&TYPE);
           Msg->Get((long  *)&ID);
           Msg->Get((char *)&LIGHT);
           Msg->Get((char *)&STATUS);
           Msg->Get((char *)&PHP);
           Msg->Get((char *)&CombatMode);
           Msg->Get((char *)&chHiddenInv2); //NMNMNM_Inv2

           if(/*Player.FactionID >0 && */MOBID > 0)
           {
              //check si ami ou pas si pas AMI force curseur ATTACK
              DWORD dwFactionMask = g_Global.GetPLFactionMask(Player.FactionID);
              if( !(FRIENDLYID & dwFactionMask) && STATUS != VOL_CANNOTTALK)
              {
                 //ennemie
                 STATUS = VOL_CANNOTTALK;
              }
           }

           
           if (TYPE == 10002)
              TYPE = 20039;
           if (TYPE == 15002)
              TYPE = 25039;
           if (TYPE == 10004)
              TYPE = 20042;
           if (TYPE == 15004)
              TYPE = 25042;
           if (TYPE == 10001)
              TYPE = 20043;
           if (TYPE == 15001)
              TYPE = 25043;
           if (TYPE == 10003)
              TYPE = 20044;
           if (TYPE == 15003)
              TYPE = 25044;
           if (TYPE == 0)
              TYPE = 274;
           
          
           // Addition.
           if (ID != Player.ID) 
           {
              //World.Lock(75); //NM:AddNewObject
              Objects.Lock(1002);
              Objects.Add(ID, TYPE, X, Y, LIGHT, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
              Objects.Unlock(1002);
              //World.Unlock(75);
           } 
           else 
           {
              if (abs(Player.xPos-X) > 5 || abs(Player.yPos-Y) > 5) 
              {
                 Player.xPos = X;
                 Player.yPos = Y;

                 g_Global.ValidMapZonePosition(Player.xPos,Player.yPos);
              }
              Player.Light = LIGHT;
           }
           
                    } break;
           
        case RQ_GetPlayerPos: 
           { 
              signed short WORLD, X, Y;
              while (NeedRedraw) 
                 Sleep(10);
              //							Msg->Get((short *)&GeX);
              //							Msg->Get((short *)&GeY);
              Msg->Get((short *)&X);
              Msg->Get((short *)&Y);
              Msg->Get((short *)&WORLD);


              if (X < 0 || X > 3072 || Y < 0 || Y > 3072 || WORLD < 0 || WORLD > (World.NumberofWorld-1)) 
              {
                 V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[98],CL_RED,true);
                 Sleep(2500);
                 PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
                 return;
              }

              if (X == Player.xPos && Y == Player.yPos && WORLD == Player.World) 
              {
                 if (ReAsk) 
                 {
                    ReAsk = FALSE;
                    if (!pfIsAtPosition() && !bCanMovePL) 
                    {
                       short Menrage = (short)pfGetNextMovement();
                       if (Menrage) 
                       {
                          TFCPacket Send;
                          Send << Menrage;
                          SEND_PACKET(Send);
                       }
                       Try = timeGetTime();
                    } 
                    else 
                    {
                       bCanMovePL = true;
                    }
                 } 
                 else 
                 {
                    Attack = false;
                    Use = false;
                    Get = false;
                    Follow = false;
                    bCanMovePL = true;
                    pfStopMovement("Move 2");
                 }
              } 
              else 
              {
                 //									if (Abs(Player.xPos - X) > 1 || Abs(Player.yPos - Y) > 1) {
                 //										CString Text;
                 //										Text.Format("Erreur dans l'ordre des position Last (%d, %d) New (%d %d)", Player.xPos, Player.yPos, X, Y);
                 //									}
                 Objects.Direction2 = 1;


                 //AfxBeginThread(ChoseFunction, NULL);
                 signed char lMovX = X - Player.xPos;
                 signed char lMovY = Y - Player.yPos;

                 if ((Abs(lMovX) > 10) || (abs(lMovY) > 10) || WORLD != Player.World) 
                 {
                    CloseAllUI();
                    DoNotMove = FALSE;
                    DrawLastMoving = 0;
                    World.Done = 0;
                    Draw = true;
                    Player.xPos = X;
                    Player.yPos = Y;
                    Player.World = WORLD;
                    // on commence par loader la map du new world...
                    g_Global.LoadZoneMapWorld(Player.World,Player.xPos,Player.yPos);
                    g_Global.ForceDisplayZone(Player.xPos,Player.yPos);
                    //g_Global.ValidMapZonePosition(Player.xPos,Player.yPos);


                    World.Lock(78);//NM Move World
                    Objects.Lock(77);
                    Objects.AddSDObject();
                    Objects.AddSDAnimObject();
                    World.SetPosition(Player.xPos, Player.yPos, Player.World, FALSE);
                    Objects.MovePlayer(lMovX, lMovY);
                    Objects.SetSet(TRUE);
                    Objects.Direction2 = 1;
                    Objects.GetMainObject()->SpriteNumber = 1;
                    Objects.Unlock(77); 
                    World.Unlock(78);


                 } 
                 else 
                 {
                    if (Abs(lMovX) > 1 || Abs(lMovY) > 1) 
                    {
                       if (X-Player.xPos > 1)
                          X = Player.xPos + 1;
                       else if (X-Player.xPos < 0)
                          X = Player.xPos - 1;
                       if (Y-Player.yPos > 1)
                          Y = Player.yPos + 1;
                       else if (Y-Player.yPos < 0)
                          Y = Player.yPos - 1;

                       Retrace = true;
                    }
                     
                    World.Lock(83); //NM Move World
                    Objects.Lock(1004);
                    Objects.Direction2 = World.MoveToPosition(X, Y);
                    Objects.Unlock(1004);
                    World.Unlock(83);
                    NeedRedraw = true;
                 }
              }
           } break;
      
           case TFCMoveID: 
           {
              signed char LIGHT;
              signed short X, Y;
              unsigned long ID;
              unsigned short TYPE,MOBID;
              unsigned long FRIENDLYID;
              unsigned char STATUS;
              char PHP;
              char CombatMode;
              char chHiddenInv2;
              //							Msg->Get((short *)&GeX);
              //							Msg->Get((short *)&GeY);
              Msg->Get((short*)&X);
              Msg->Get((short*)&Y);
              Msg->Get((long *)&FRIENDLYID);
              Msg->Get((short*)&MOBID);
              Msg->Get((short*)&TYPE);
              Msg->Get((long *)&ID);
              Msg->Get((char *)&LIGHT);
              Msg->Get((char *)&STATUS);
              Msg->Get((char *)&PHP);
              Msg->Get((char *)&CombatMode);
              Msg->Get((char *)&chHiddenInv2);

              if(/*Player.FactionID >0 && */MOBID > 0)
              {
                 //check si ami ou pas si pas AMI force curseur ATTACK
                 DWORD dwFactionMask = g_Global.GetPLFactionMask(Player.FactionID);
                 if( !(FRIENDLYID & dwFactionMask) && STATUS != 0x00)
                 {
                    //ennemie
                    STATUS = VOL_CANNOTTALK;
                 }
              }

              if( intrQueueLen > 50 && ID != Player.ID)
              {
                 return;
              }

              if (!DoNotMove || (DoNotMove && ID != Player.ID) ) 
              {
                 if (TYPE == 10002)
                    TYPE = 20039;
                 if (TYPE == 15002)
                    TYPE = 25039;
                 if (TYPE == 10004)
                    TYPE = 20042;
                 if (TYPE == 15004)
                    TYPE = 25042;
                 if (TYPE == 10001)
                    TYPE = 20043;
                 if (TYPE == 15001)
                    TYPE = 25043;
                 if (TYPE == 10003)
                    TYPE = 20044;
                 if (TYPE == 15003)
                    TYPE = 25044;


                 if (X < 0 || X > 3072 || Y < 0 || Y > 3072) {
                    V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[98],CL_RED,true);
                    Sleep(2500);
                    PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
                    return;
                 }


                 if (ID == Player.ID) 
                 {

                    g_GameMusic.LoadNewSound("TFCMoveID");

                    if (TalkToX || TalkToY) 
                    {
                       if (Abs(TalkToX - Player.xPos) >= 15 || Abs(TalkToY - Player.yPos) >= 15) 
                       {
                          TFCPacket Send;

                          Send << (short)RQ_BreakConversation;
                          Send << (long)TalkToID;
                          Send << (short)TalkToX;
                          Send << (short)TalkToY;

                          TalkToX = 0;
                          TalkToY = 0;
                          TalkToW = 0;
                          TalkToID = 0;
                          TalkToOffset = 0;
                          SelectedID = 0;

                          SEND_PACKET(Send);
                       }
                    }

                    if (X == Player.xPos && Y == Player.yPos) 
                    {
                       pfStopMovement("Move 3");
                       Try = timeGetTime();
                       bCanMovePL = true;
                    } 
                    else 
                    {
                       signed char lMovX = X - Player.xPos;
                       signed char lMovY = Y - Player.yPos;

                       if (Abs(lMovX) > 10 || Abs(lMovY) > 10) 
                       {
                          // hide all UI.                                                              
                          CloseAllUI();
                          DoNotMove = FALSE;
                          DrawLastMoving = 0;
                          World.Done = 0;
                          Draw = true;
                          Player.xPos = X;
                          Player.yPos = Y;

                          g_Global.ValidMapZonePosition(Player.xPos,Player.yPos);

                          World.Lock(78);//NM:Move ID >10 donc teleport
                          Objects.Lock(77);
                          Objects.AddSDObject();
                          Objects.AddSDAnimObject();
                          World.SetPosition(Player.xPos, Player.yPos, Player.World, FALSE);
                          Objects.MovePlayer(lMovX, lMovY);
                          Objects.SetSet(TRUE);
                          Objects.Direction2 = 1;
                          Objects.GetMainObject()->SpriteNumber = 1;
                          Objects.Unlock(82);
                          World.Unlock(78);


                          TFCPacket Send;
                          Send << (short)60;
                          SEND_PACKET(Send);
                       } 
                       else 
                       {
                          if (Abs(lMovX) > 1 || Abs(lMovY) > 1) 
                          {
                             if (X-Player.xPos > 1)
                                X = Player.xPos + 1;
                             else if (X-Player.xPos < 0)
                                X = Player.xPos - 1;
                             if (Y-Player.yPos > 1)
                                Y = Player.yPos + 1;
                             else if (Y-Player.yPos < 0)
                                Y = Player.yPos - 1;

                             Retrace = true;
                          }

                          Player.Light = LIGHT;
                          World.Lock(88);//NM:Move World
                          Objects.Lock(1006);
                          Objects.Direction2 = World.MoveToPosition(X, Y);
                          Objects.Unlock(1006);
                          World.Unlock(88);

                          NeedRedraw = true;
                       }
                    }
                 }  
                 else 
                 {
                    //World.Lock(90);//NM:Move Object Only
                    Objects.Lock(1007);
                    Objects.MoveObject(ID, X, Y, FALSE, 0, PHP,CombatMode,chHiddenInv2, TYPE, LIGHT, STATUS, true,true,MOBID);
                    Objects.Unlock(1007);
                    //World.Unlock(90);
                 }
              }
           } break;
      
        case TFCStillConnected: {
           TFCPacket Send; 
           
           Send << (short)10;
           
           SEND_PACKET(Send);
                                } break;
           
        case TFCDeleteObject: 
        {
           unsigned long ID;
           char Temp;
           
           Msg->Get((char *)&Temp);
           Msg->Get((long  *)&ID);
           
           // Delete object.
           if (ID == Player.ID) 
           {
           } 
           else 
           {
              Objects.Lock(91);
              Objects.Delete(ID);
              Objects.Unlock(91);
           }
        } break;
           
        case RQ_DepositObject: {
           unsigned short TYPE;
           unsigned long ID;
           unsigned long ULDEAD;
           Msg->Get((short *)&TYPE);
           Msg->Get((long  *)&ID);
           Msg->Get((long  *)&ULDEAD);
           

           if (TYPE == 10002)
              TYPE = 20039;
           if (TYPE == 15002)
              TYPE = 25039;
           if (TYPE == 10004)
              TYPE = 20042;
           if (TYPE == 15004)
              TYPE = 25042;
           if (TYPE == 10001)
              TYPE = 20043;
           if (TYPE == 15001)
              TYPE = 25043;
           if (TYPE == 10003)
              TYPE = 20044;
           if (TYPE == 15003)
              TYPE = 25044;
           
           if(TYPE == 11000)
              OutputDebugString("********** RECU DEAD....\n\n");
           
           
           // Changement de type.
           int x;
           if (Player.ID != ID) 
           {
              //World.Lock(11234);//NM:Onject Only
              Objects.Lock(92);
              x = Objects.ChangeType(ID, TYPE,ULDEAD);
              Objects.Unlock(92);
              //World.Unlock(11234);
           } 
           else 
           {
              //World.Lock(11234); //NM:Onject Only
              Objects.Lock(93);
              x = Objects.ChangePlayerType(TYPE);
              Objects.Unlock(93);
              //World.Unlock(11234);
           }
                              } break;
           
           
        // Adds an effect to the V3_EffectStatusDlg
        case RQ_CreateEffectStatus:
        {
           
           
           TFCPacket *msg = Msg;
           
           DWORD effectId, time, iconId, totalTime;
           LPBYTE desc = NULL;
           
           GET_LONG( effectId );
           GET_LONG( time );
           GET_LONG( totalTime );
           GET_LONG( iconId );
           GET_STRING( desc );
           
           V3_EffectStatusDlg::GetInstance()->AddEffect(effectId,time,totalTime,iconId,reinterpret_cast< char * >( desc ));
           delete desc;
        }break;
        // Removes an effect to the V3_EffectStatusDlg
        case RQ_DispellEffectStatus:
        {
           TFCPacket *msg = Msg;
           DWORD effectId;
           GET_LONG( effectId );

		   char strTmp[100];
		   sprintf_s(strTmp,100,"Dispell SpellID %d\n",effectId);
		   OutputDebugString(strTmp);


           V3_EffectStatusDlg::GetInstance()->DispellEffect(effectId);
        } break;
           
           
        case TFCAddObject: {
           
           signed char LIGHT;
           unsigned short X, Y, TYPE, nbObject,MOBID;
           unsigned long ID,FRIENDLYID;
           unsigned char STATUS;
           char PHP;
           char CombatMode;
           char chHiddenInv2;
           Msg->Get((short *)&nbObject);
           
           for (unsigned int i = 0; i < nbObject; i++) {
              Msg->Get((short *)&X);
              Msg->Get((short *)&Y);
              Msg->Get((long  *)&FRIENDLYID);
              Msg->Get((short *)&MOBID);
              Msg->Get((short *)&TYPE);
              Msg->Get((long  *)&ID);
              Msg->Get((char  *)&LIGHT);
              Msg->Get((char  *)&STATUS);
              Msg->Get((char  *)&PHP);
              Msg->Get((char  *)&CombatMode);
              Msg->Get((char  *)&chHiddenInv2);

              if(/*Player.FactionID >0 && */MOBID > 0)
              {
                 //check si ami ou pas si pas AMI force curseur ATTACK
                 DWORD dwFactionMask = g_Global.GetPLFactionMask(Player.FactionID);
                 if( !(FRIENDLYID & dwFactionMask) && STATUS != 0x00)
                 {
                    //ennemie
                    STATUS = VOL_CANNOTTALK;
                 }
              }

              
              if (TYPE == 10002)
                 TYPE = 20039;
              if (TYPE == 15002)
                 TYPE = 25039;
              if (TYPE == 10004)
                 TYPE = 20042;
              if (TYPE == 15004)
                 TYPE = 25042;
              if (TYPE == 10001)
                 TYPE = 20043;
              if (TYPE == 15001)
                 TYPE = 25043;
              if (TYPE == 10003)
                 TYPE = 20044;
              if (TYPE == 15003)
                 TYPE = 25044;
              
              
              // Addition.
              if (ID != Player.ID) {
                 //World.Lock(94);//NM:Onject Only
                 Objects.Lock(1008);
                 Objects.Add(ID, TYPE, X, Y, LIGHT, STATUS, NULL, PHP,CombatMode,chHiddenInv2,2,false,0,0);
                 Objects.Unlock(1008);
                 //World.Unlock(94);
              }
              
           }
           
           if (Code13) {
              Objects.Lock(17);
              Objects.SetSet(TRUE);
              Objects.AddSDObject();
              Objects.AddSDAnimObject();
              Objects.Unlock(17); 
              
              Objects.Lock(19);
              Objects.Direction2 = 1;
              Objects.GetMainObject()->SpriteNumber = 1;
              Objects.Unlock(19);
              
              g_GameMusic.LoadNewSound("TFCAddObject");
              
              g_Var.inGame = false;
              
              Objects.Lock(13);
              Objects.Direction2 = 1;
              Objects.GetMainObject()->SpriteNumber = 1;
              Objects.Unlock(13);
              
              INGAME = TRUE;
              if (!hDrawThread) {
                 DXDClear(); //reload clear 1-2
                 DXDFlip();
                 DXDClear();//reload clear 2-2
                 DXDFlip();
                 hDrawThread = (HANDLE)_beginthreadex(NULL, 0, DrawThread, NULL, CREATE_SUSPENDED, &iDrawThreadID);
              }
              if( !hMouseActionThread ){
                 hMouseActionThread = (HANDLE)_beginthreadex(NULL, 0, MouseActionThread, NULL, 0, &iMouseActionThreadID);
              }
              
              stShow = true; 
              
              ResumeThread(hDrawThread);      
              
             
              
              World.Lock(20);//NM:SWITCH
              World.SetFading = true;
              World.FadeLevel = 0;
              World.Switch();
              World.Unlock(20);
              
              
              Code13 = false;
              noException = false;
           }
           
           if (EnterGame) 
           {
              TFCPacket Send;
              Send << (short)RQ_FromPreInGameToInGame;
              SEND_PACKET(Send);
           }
           
        } break;
    } // End of Switch(Type)
   }
}	
//#pragma optimize( "", on ) 

static UINT CALLBACK EntergamePart2(LPVOID pParam)
{
   Player.Lock();
   Objects.Lock(72);
   Objects.SetSet(TRUE);
   World.Lock(73); //NM:EnterGame
   World.SetPosition(Player.xPos, Player.yPos, Player.World, FALSE); 
   Objects.Direction2 = 1;
   Objects.GetMainObject()->SpriteNumber = 1;
   World.Unlock(73);
   Objects.Unlock(72);
   Player.Unlock();
   g_bEnterGamePart2Complete = TRUE;

   return 0;
}  


