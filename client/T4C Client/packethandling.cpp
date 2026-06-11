// ************************************************************************
// ***                                                                  ***
//      File Name: PacketHandling.cpp
//      Project:   The 4th Coming
//      Creation:  5 May, 1999
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***

#pragma warning( disable : 4291 )
#pragma warning( disable : 4786 )

// Include
#include "pch.h"
#include "App.h"
#include "PacketHandling.h"
#include "VisualObjectList.h"
#include "ObjectListing.h"
#include "TFCPlayer.h"
#include "GUILocalString.h"
#include "Packet.h"
#include "pf.h"
#include "TileSet.h"
#include "COMM.h"
#include "NTime.h"
#include "Global.h"
#include "SaveGame.h"
extern CSaveGame g_SaveGame;

#include "Global.h"
extern Global g_Global;

#include "NewInterface/V3_ChatLogDlg.h"
#include "NewInterface/V3_ChatDlg.h"

// Global var.
extern BOOL DoNotMove;
extern int DrawLastMoving;
extern bool Draw;
extern BOOL INGAME;
extern HANDLE hDrawThread;
extern UINT iDrawThreadID;
extern bool stShow;
extern BOOL FirstTime;
extern UINT CALLBACK DrawThread(LPVOID pParam);
extern bool EnterGame;
extern bool seraphDisplay;
extern bool goodSeraph;
extern T3VSBSound SoundFX[250];
extern DWORD TargetID;

extern int TalkToX, TalkToY, TalkToW;
extern unsigned long TalkToID;
extern DWORD SelectedID;
extern short TalkToOffset;

inline int Abs2(int x) {
   return ((x < 0) ? (-x) : (x));
}

void CloseAllUI(void);


//using namespace vir;//DC a mettre en commentaire apres debeug

void PacketHandling::SetUnitStat(TFCPacket *Msg) 
{
   // Set unit statistic, permet de faire des changements de Light, Status et Hp.
   // Log packet type of RQ_UnitUpdate.
   
   short TYPE,MOBID;
   DWORD FRIENDLYID;
   DWORD ID;
   char LIGHT;
   char STATUS;
   char PHP;
   char CombatMode;
   char chHiddenInv2;
   
   Msg->Get((long  *)&FRIENDLYID);
   Msg->Get((short *)&MOBID);
   Msg->Get((short *)&TYPE);
   Msg->Get((long  *)&ID);
   Msg->Get((char  *)&LIGHT);
   Msg->Get((char  *)&STATUS);
   Msg->Get((char  *)&PHP);
   Msg->Get((char  *)&CombatMode);
   Msg->Get((char  *)&chHiddenInv2); //NMNMNM_INV2

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
   
      
   Objects.Lock(3);
   Objects.SetUnit(ID, TYPE, LIGHT, STATUS, PHP,CombatMode,chHiddenInv2,true,MOBID);
   Objects.Unlock(3);
}

void PacketHandling::SetUnitSmiles(TFCPacket *Msg) 
{
   short TYPESmile;
   DWORD ID;
   Msg->Get((short *)&TYPESmile);
   Msg->Get((long  *)&ID);

   Objects.Lock(3);
   Objects.SetSmile(ID, TYPESmile);
   Objects.Unlock(3);
}

void PacketHandling::IndirectTalk(TFCPacket *Msg) 
{
    // IndirectTalk (evey talking in the game)
    // Log packet type of 27.
    
    unsigned long ID;
    short Len;
    unsigned long Color;
    unsigned char Direction = 2;
    char *Temp;
    char isNpc = 0;
    
    Msg->Get((long *)&ID);           // Get talking ID.
    Msg->Get((char *)&Direction);	   // Get talking Direction.
    Msg->Get((long *)&Color);        // Get Text Color.
    Msg->Get((char *)&isNpc );
    
    
    // If Dragon then Direction = South!
    Objects.Lock(4);
    if (ID == Objects.FoundType(__MONSTER_DRAGON))
        Direction = 2;
    Objects.Unlock(4);
    
    // Set static color.
    Color=CL_CYAN;
    
    Msg->Get((short *)&Len);   // Get text Len
    
    // Allocate Temp var.
    Temp = new char [Len+1];   
    
    // Get Text.
    int j = 0;
    for (j = 0; j < Len; j++) {
        Msg->Get((char *)&Temp[j]);
    }
    Temp[Len] = 0;
    
    
    short unitNameLen = 0;
    Msg->Get((short*)&unitNameLen);
    
    char *unitName = new char [unitNameLen+1];
    
    for (j = 0; j < unitNameLen; j++) {
        Msg->Get((char *)&unitName[j]);
    }
    unitName[unitNameLen] = 0;

    DWORD color = CL_RED;
    Msg->Get((DWORD *)&color);
    
    if (Len && unitNameLen) 
    {
        // Have we receive any text ?
        if (ID == Player.ID) 
        {
            // Is the player talking ?
            
            Objects.Lock(5);
            
            

            Objects.GetMainObject()->SetTalkText( Temp, Custom.gTextColor ,FALSE,0,NULL);

           
            Objects.GetMainObject()->Temp = timeGetTime();
            if (Direction){
                Objects.GetMainObject()->Direction = Direction;
            }         
            
            Objects.Unlock(5);
            
            
            if( g_SaveGame.GetOptionsParam()->bLogUser )
            {
                // Log to backscroll
                string str( g_LocalString[ 529 ] );
                str += Temp;
                
                V3_ChatLogDlg::GetInstance()->AddListLogs("",str.c_str(),SYSTEM_COLOR,false,false);
            }
            
        } 
        else 
        {
           if(!V3_ChatDlg::GetInstance()->IsUserIgnored( unitName ))  //receive page from ignored
           {
               // Is another character talking
               // Put text on top of unit.
               Objects.Lock(6);
               //Objects.SetName(ID, unitName, FALSE, color);
               char strTextMod[2048];
               strTextMod[0] = 0x00;

               Objects.Text(ID, Temp, Color, Direction,strTextMod);
               Objects.Unlock(6);

               bool bLogNPCT = true;
               if(!g_SaveGame.GetOptionsParam()->blogNPC && ID != TargetID)
                  bLogNPCT = false; //log pas aucun NPC

               if(!g_SaveGame.GetOptionsParam()->blogNPCPerso && ID == TargetID)
                  bLogNPCT = false; //log pas les npc a soit
            

               if(strTextMod[0] != 0x00)
               {

               }
            
               if( bLogNPCT || !isNpc )
               {         
                  int iLen =0;
                  if(strTextMod[0] != 0x00)
                     iLen =strlen( unitName ) + strlen( strTextMod ) + 10 ;
                  else 
                     iLen =strlen( unitName ) + strlen( Temp ) + 10 ;
                  char *backscrollText = new char[iLen];
                  strcpy_s( backscrollText,iLen, "{" );
                  strcat_s( backscrollText,iLen, unitName );
                  strcat_s( backscrollText,iLen, "}" );
                  strcat_s( backscrollText,iLen, "\":\" " );
                  if(strTextMod[0] != 0x00)
                     strcat_s( backscrollText,iLen, strTextMod );
                  else
                     strcat_s( backscrollText,iLen, Temp );


                  V3_ChatLogDlg::GetInstance()->AddListLogs( unitName, backscrollText, Color, false, false );

                  delete backscrollText;
               }
            }
            
            
        }
    }
    delete Temp;
    delete unitName;
    
    
}

void PacketHandling::TeleportPlayer(TFCPacket *Msg) {
    // Teleport
    // Log Packet Type 57

    // Stop the game music (and reset it).
    if (g_Var.inGame) 
    {
       if (TalkToX || TalkToY) 
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

       OutputDebugString("teleport Player In Game...\n");
        DoNotMove = TRUE;
        CloseAllUI();
        Sleep(150);
        World.Lock(12); //NM:teleport player
        
        signed short WORLD, X, Y;
        Msg->Get((short *)&X);
        Msg->Get((short *)&Y);
        Msg->Get((short *)&WORLD);
        
        if (X < 0 || X > 3072 || Y < 0 || Y > 3072 || WORLD < 0 || WORLD > (World.NumberofWorld-1)) {
            V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[98],CL_RED,true);
            Sleep(2500);
            PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
            return;
        }
        
        pfStopMovement("Move 17");
        Player.xPos = X;
        Player.yPos = Y;
        Player.World = WORLD;
        
        Objects.Lock(7);
        Objects.DeleteAll();
        //Objects.ReleaseAll();
        Objects.Unlock(7);
        World.ChangeStatic();
        signed char lMovX = X - Player.xPos;
        signed char lMovY = Y - Player.yPos;
        
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
        
        Objects.Lock(1015);
        Objects.AddSDObject();
        Objects.AddSDAnimObject();
        World.SetPosition(Player.xPos, Player.yPos, Player.World);
        Objects.MovePlayer(lMovX, lMovY);
        Objects.SetSet(TRUE);
        Objects.Unlock(1015);
        World.SetFading = true;
        World.FadeLevel = 0;
        World.Switch();
        
        g_GameMusic.LoadNewSound("TeleportPlayer");
        World.Unlock(12);
        
        TFCPacket Send;
        Send << (short)60;
        EnterGame = true;
        SEND_PACKET(Send);
        
    }
    
    
}
void PacketHandling::FromPreInGameToInGame(TFCPacket *Msg) {
    // Send to Server when loading is finsih... receive from ser er for a confirmation.
    try {

        // If the player is a seraph, play the seraph sound
        DoNotMove = FALSE;
        g_Var.inGame = true;
        EnterGame = false;

        if(g_Var.InvalideSaveGameI)
        {
           g_Var.InvalideSaveGameI = false;
           V3_ChatLogDlg::GetInstance()->AddListLogs("", "Invalid (inventory / chest guild chest) Savegame...", CL_RED, true);
           V3_ChatLogDlg::GetInstance()->AddListLogs("", "Default will be used.", CL_RED, true);
        }
        if(g_Var.InvalideSaveGameM)
        {
           g_Var.InvalideSaveGameM = false;
           V3_ChatLogDlg::GetInstance()->AddListLogs("", "Invalid (Macro) Savegame...", CL_RED, true);
           V3_ChatLogDlg::GetInstance()->AddListLogs("", "Default will be used.", CL_RED, true);
        }
        if(g_Var.InvalideSaveGameC)
        {
           g_Var.InvalideSaveGameC = false;
           V3_ChatLogDlg::GetInstance()->AddListLogs("", "Invalid (Chatter chanal / ignore list user) Savegame...", CL_RED, true);
           V3_ChatLogDlg::GetInstance()->AddListLogs("", "Default will be used.", CL_RED, true);
        }


        Objects.CreateTorch();
        
        Objects.Lock(30);
        Objects.UpdateLight(TRUE);
        Objects.Unlock(30);

        if(g_uiReloadForceTeleport == 0)
        {
           //OutputDebugString("\n*********************************************\n");
           //OutputDebugString("***** FromPreInGameToInGame\n");
           //OutputDebugString("*********************************************\n\n");

           TFCPacket Send;
           Send << (RQ_SIZE)RQ_GetChatterChannelList;
           SEND_PACKET(Send);
        }
         


        if(g_uiReloadForceTeleport == 1)
        {

           TFCPacket Send2;
           Send2 << (short)39;
           SEND_PACKET(Send2);
           
           TFCPacket Send3;
           Send3 << (RQ_SIZE)RQ_SendSpellList;
           Send3 << (char)1;
           SEND_PACKET(Send3);
           
           
           TFCPacket Send3b;
           Send3b << (RQ_SIZE)RQ_ViewBackpack2;
           SEND_PACKET(Send3b);

             
           EnterGame = true;
           TFCPacket Send4;
           Send4 << (short)60;
           SEND_PACKET(Send4);


           PacketHandling::ForceTeleportPlayer(Player.World,Player.xPos,Player.yPos);
           g_uiReloadForceTeleport = 2;
        }
        else if(g_uiReloadForceTeleport == 2)
        {
            PacketHandling::ForceTeleportPlayer(Player.World,Player.xPos,Player.yPos);
            g_uiReloadForceTeleport = 0;
        }

        /*
        
        signed char LIGHT;
        unsigned short X, Y, TYPE, nbObject;
        unsigned long ID;
        unsigned char STATUS;
        char PHP;
        Msg->Get((char *)&X); // Dummy Junk Discard
        Msg->Get((short *)&X); // Dummy Junk Discard
        Msg->Get((short *)&nbObject);
        
        for (unsigned int i = 0; i < nbObject; i++) {
            Msg->Get((short *)&X);
            Msg->Get((short *)&Y);
            Msg->Get((short *)&TYPE);
            Msg->Get((long  *)&ID);
            Msg->Get((char *)&LIGHT);
            Msg->Get((char *)&STATUS);
            Msg->Get((char *)&PHP);
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
                World.Lock(21); //NM:Already commented
                Objects.Lock(1016);
                Objects.Add(ID, TYPE, X, Y, LIGHT, STATUS, NULL, PHP);
                Objects.Unlock(1016);
                World.Unlock(21);
            }
            
        }*/
        
        
    } catch (TFCPacketException *e) {
        // Imcomplete or corrupted packet, we don't care, continue....
        e; // disable a warning.
    }
}

void PacketHandling::ForceTeleportPlayer(USHORT WORLD, USHORT X, USHORT Y) 
{

    // Stop the game music (and reset it).
    if (g_Var.inGame) 
    {
        DoNotMove = TRUE;
        CloseAllUI();
        Sleep(150);
        World.Lock(12); //NM:Force teleport player
        if (X < 0 || X > 3072 || Y < 0 || Y > 3072 || WORLD < 0 || WORLD > (World.NumberofWorld-1)) 
        {
            V3_ChatLogDlg::GetInstance()->AddListLogs("",g_LocalString[98],CL_RED,true);
            Sleep(2500);
            PostMessage(siGethWnd(), WM_CLOSE, NULL, 0x6969);
            return;
        }
        pfStopMovement("Move 17");
        Player.xPos = X;
        Player.yPos = Y;
        Player.World = WORLD;
        
        Objects.Lock(7);
        Objects.DeleteAll();
        //Objects.ReleaseAll();
        Objects.Unlock(7);
        World.ChangeStatic();
        signed char lMovX = X - Player.xPos;
        signed char lMovY = Y - Player.yPos;
        
        DrawLastMoving = 0;
        World.Done = 0;
        Draw = true;
        
        //BOOL bForceUpdateName = TRUE;
        //if(Player.xPos == X && Player.yPos == Y && Player.World == WORLD)
        //   bForceUpdateName = FALSE;

        Player.xPos = X;
        Player.yPos = Y;
        Player.World = WORLD;
   
        // on commence par loader la map du new world...
        g_Global.LoadZoneMapWorld(Player.World,Player.xPos,Player.yPos);
        //if(bForceUpdateName)
           g_Global.ForceDisplayZone(Player.xPos,Player.yPos);
        //g_Global.ValidMapZonePosition(Player.xPos,Player.yPos);
        
        Objects.Lock(1015);
        Objects.AddSDObject();
        Objects.AddSDAnimObject();
        World.SetPosition(Player.xPos, Player.yPos, Player.World);
        Objects.MovePlayer(lMovX, lMovY);
        Objects.SetSet(TRUE);
        Objects.Unlock(1015);
        World.SetFading = true;
        World.FadeLevel = 0;
        World.Switch();
        g_GameMusic.LoadNewSound("ForceTeleportPlayer");
        EnterGame = true;
        World.Unlock(12);
    }
}
