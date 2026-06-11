#pragma warning( disable : 4291 )
#if defined(LINUX_PORT) && !defined(_WIN32)
#include "Comm.h"
#include "network/T4CClientStubs.h"
#include <cstdio>
#else
#include "pch.h"
#include "COMM.h"
#include "Global.h"
#include "App.h"
#include <process.h>
#endif

   
PacketCenter COMM;
 
PacketCenter::PacketCenter(void) {
// Constructor.
   Ctr = NULL;
   State = 0;
	InitializeCriticalSection(&csInternal);
	AddrIP = NULL;
   m_hSendAliveThread   = NULL;
   m_hSendAliveDone     = NULL;
} 
  
PacketCenter::~PacketCenter(void) {
// Destructor.
	DeleteCriticalSection(&csInternal);
	if (AddrIP)
		delete AddrIP;
}
 
void PacketCenter::Close(void) {
   if (Ctr) {
      delete Ctr;
      Ctr = NULL;
   }
   WSACleanup();
}

void PacketCenter::SetIPAddr(char *a) {
   AddrIP = new char [strlen(a) + 1];
   strcpy_s(AddrIP,strlen(a) + 1, a);
}

char *PacketCenter::GetIPAddr()
{
   return AddrIP;
}

void PacketCenter::SetAddrPort(short p) {
   AddrPort = p;
}

void PacketCenter::Lock(void) {
	EnterCriticalSection(&csInternal);
}

void PacketCenter::Unlock(void) {
	LeaveCriticalSection(&csInternal);
}

int PacketCenter::Receive(TFCPacket * &P) {
   if (tqQueue.NbObjects()) {
      P = tqQueue.Retreive();
      return 1;
   } 
   return 0;
}

void PacketCenter::AddPacket(TFCPacket *P) {
   tqQueue.AddToQueue(P);
}

BOOL PacketCenter::Create( COMM_INTR_CALLBACK lpIntr ) {
	Ctr = new NMPacketManager();
   
   //WSADATA  wsaData;
	//if (WSAStartup(MAKEWORD( 1, 1 ) ,&wsaData) == SOCKET_ERROR) {
	//}
   
   if (!Ctr->Init(lpIntr, this, false, AddrPort, AddrIP)) {
#if defined(LINUX_PORT) && !defined(_WIN32)
      std::fprintf(stderr, "[COMM] NMPacketManager::Init failed (host=%s port=%d)\n",
                   AddrIP ? AddrIP : "?", static_cast<int>(AddrPort));
#endif
      delete Ctr;
      Ctr = NULL;
      return FALSE;
   }
   Ctr->GetConnectedSockIn(&sockAddr);
   
   return TRUE;
}

void PacketCenter::SendPacket(TFCPacket *P) {

#if defined(LINUX_PORT) && !defined(_WIN32)
   if (g_boQuitApp && P->GetPacketID() != 20) {
      std::fprintf(stderr, "[COMM] SendPacket opcode %d ignored (g_boQuitApp)\n", static_cast<int>(P->GetPacketID()));
      return;
   }
#else
   if ( g_boQuitApp && P->GetPacketID() != 20)
      return;
#endif

   DWORD dwMaxAck = 0;
   DWORD dwMaxDelay = 0;

   LPBYTE lpbBuffer;
   INT    nbBuffer; 
   
   RQ_SIZE PacketID = P->GetPacketID();

   /*if(PacketID!= 10)
   {
      char strtoto[100]; 
      sprintf_s(strtoto,100,"++++++++OUT TYPE  %d\n",PacketID);
      OutputDebugString(strtoto);
   }*/
   

   
   switch (PacketID) 
   { 
      case RQ_QueryServerVersion: 
      case RQ_MessageOfDay: 
			dwMaxAck = 3;         
			dwMaxDelay = 4000;
		break;
      case RQ_RegisterAccount: 
#if defined(LINUX_PORT) && !defined(_WIN32)
         /* Reconnect: eviter pending ACK NM qui bloque les fragments suivants. */
         dwMaxAck = 0;
         dwMaxDelay = 0;
#else
         dwMaxAck = 3;         
         dwMaxDelay = 8000;
#endif
      break;

     
      case RQ_TradeInvite:
      case RQ_NM_GuildInvite:
      case RQ_PutPlayerInGame: 
      case RQ_DeletePlayer: 
      case RQ_CreatePlayer: 
      case RQ_GetPersonnalPClist: 
      case RQ_Reroll: 
      case RQ_FromPreInGameToInGame: 
      case RQ_TogglePage: 
      case RQ_QueryItemInfo: 
      case RQ_GetPersonnalPClistEquitSkin:
      case RQ_GetObject: 
      case RQ_UseObject2:
      case RQ_DepositObject:	
      case RQ_UseObject: 
      case RQ_Attack: 
      case RQ_CastSpell:	
      case RQ_UseSkill: 
      case RQ_TeleportPlayer:
      case RQ_ViewEquiped: 
      case RQ_EquipObject: 
      case RQ_UnequipObject: 
      case RQ_IndirectTalk: 
      case RQ_Shout: 
      case RQ_Page: 
      case RQ_DirectedTalk: 
      case RQ_DirectedTalkNoFeed:
      case RQ_BroadcastTextChange: 
      case RQ_GetUnitName:
		case RQ_BreakConversation: 
      case RQ_ReturnToMenu: 
      case RQ_GetSkillList : 
      case RQ_SendTrainSkillList: 
      case RQ_SendBuyItemList: 
      case RQ_SendPointsItemList:
      case RQ_GetStatus: 
      case RQ_GetTime:
      case RQ_AddRemoveChatterChannel: 
      case RQ_SendChatterChannelMessage: 
		case RQ_GetChatterUserList2: 
      case RQ_GetSkillStatPoints: 
      case RQ_SendStatTrain :
      case RQ_QueryItemName:
      case RQ_GetNearItems: 
      case RQ_PlayerFastMode:
      case RQ_SendSpellList:
      case RQ_ServerMessage: 
      case RQ_SpellEffect:
      case RQ_ManaChanged:
      case RQ_PuppetInformation: 
      case RQ_UnitUpdate: 
      case RQ_UpdateSmile:
      case RQ_ViewInv:
      case RQ_RP_InviteRP:
			dwMaxAck = 1;           //3
			dwMaxDelay = 2000;
		break;

#if defined(LINUX_PORT) && !defined(_WIN32)
      case RQ_ExitGame:
         /* Logout : pas d'ACK NM pending — sinon ExitGame part apres le Register suivant. */
         dwMaxAck = 0;
         dwMaxDelay = 0;
         break;
#else
      case RQ_ExitGame:
         dwMaxAck = 1;
         dwMaxDelay = 2000;
         break;
#endif

      default: 
         dwMaxAck = 0;
         dwMaxDelay = 0;
      break;
   }; 

      
   //P->EncryptPacket(); Its no longer used. Replaced by new crypt.
   P->GetBuffer(lpbBuffer, nbBuffer);
   if (!Ctr) {
      // Pas (ou plus) connecte : rien a envoyer, on evite un deref null.
      return;
   }
   Ctr->SendUDPPacket(sockAddr, lpbBuffer, nbBuffer, dwMaxDelay, dwMaxAck,false,true);

   dwLastSend = timeGetTime();
}

void PacketCenter::StartSendAlive()
{
   m_hSendAliveDone     = CreateEvent(NULL,TRUE,FALSE,"");
   m_hSendAliveThread   = (HANDLE)_beginthread( SendAliveDataThread  , 0, this );
}

void PacketCenter::StopSendAlive()
{
   SetEvent(m_hSendAliveDone);
   if( WaitForSingleObject( m_hSendAliveThread, 500) == WAIT_TIMEOUT ) 
      TerminateThread( m_hSendAliveThread, 1 );
   CloseHandle(m_hSendAliveDone);
}

void PacketCenter::SendAliveDataThread(LPVOID lpData)
{
   bool bRun = true; 
   
   PacketCenter * me = (PacketCenter*)lpData;

   while(bRun)
   {
      switch( ::WaitForSingleObject(me->m_hSendAliveDone,250))
      { 
         case WAIT_OBJECT_0 +1:
            bRun = false;
         break;
         case WAIT_TIMEOUT :
            {
               if (timeGetTime() - me->dwLastSend > 3000) 
               {
                  TFCPacket Send; 
                  Send << (short)10;
                  SEND_PACKET(Send);
                  //OutputDebugString("Envoie ACK BATARD....\n");
               }
            }
         break;
      }
   }
}


void PacketCenter::KeepAlive(void) 
{
	dwTime = timeGetTime();

}

BOOL PacketCenter::isAlive(void) {
	DWORD dwTest = timeGetTime();			
	if ((dwTime < dwTest) && ((dwTest - dwTime) > 120000)) {
		return FALSE;
	}
	
	return TRUE;
}

void PacketCenter::SetAlive(DWORD dwT) {
   dwTime = (timeGetTime() - (120000-dwT));
}


BOOL PacketCenter::isHalf(void)  
{
	DWORD dwTest = timeGetTime();			
	if ((dwTime < dwTest) && ((dwTest - dwTime) > 6000/*2500*/)) 
   {
		return FALSE;
	}
	
	return TRUE;
}

void PacketCenter::LongLive(void) {
	dwTime = -1;
}

NMPacketManager *PacketCenter::GetCommCenter(){
    return Ctr;
}