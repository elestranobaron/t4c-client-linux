//--FILE DEFINITION--------------------------------------------------------
//
/*! \file
   File name:  NMPacketManager.cpp
*/
//-------------------------------------------------------------------------


#if defined(LINUX_PORT) && !defined(_WIN32)
#include <cctype>
#include <cstdio>
#else
#include <process.h>
#endif
#include "NMPacketManager.h"

#if defined(LINUX_PORT) && !defined(_WIN32)
typedef ULONG_PTR T4C_IOCP_KEY;
#define T4C_IOCP_PTR(p) reinterpret_cast<ULONG_PTR>(p)
#else
typedef DWORD T4C_IOCP_KEY;
#define T4C_IOCP_PTR(p) reinterpret_cast<DWORD>(p)
#endif




#define  WAIT_THREAD( __thread ) if( WaitForSingleObject( __thread, 2000 ) == WAIT_TIMEOUT ) {\
                                     TerminateThread( __thread, 1 ); }

#define  MEM_COMPRESS_NEED(ActualLen) (ULONG)(ActualLen *1.1 + 12);

extern void EnterGameTrace(const char *msg); // DEBUG trace (definie dans Packet.cpp)


/*
bool operator < ( const sockaddr_in &sock1, const sockaddr_in &sock2 )
{    
   return sock1.sin_addr.S_un.S_addr < sock2.sin_addr.S_un.S_addr ? true : sock1.sin_port < sock2.sin_port;        
}
*/

unsigned short Reverse(unsigned short n)
{
#if defined(LINUX_PORT) && !defined(_WIN32)
   return static_cast<unsigned short>((n >> 8) | (n << 8));
#else
   __asm mov ax,n
      __asm rol ax,8
#endif
}

void AddToLogFile(BOOL bRecv,LPCTSTR pszFmt, ...)
{
  
}


//--METHOD IMPLEMENTATION-------------------------------------------------
//
/*! Default constructor
*/
//------------------------------------------------------------------------
NMPacketManager::NMPacketManager()
{
   
   m_pUDP        = NULL;
   m_bInit       = false;
   
   m_bUDPMaintenancePacket   = TRUE;
   m_hUDPMaintenancePacket   = NULL;

   m_bUDPProcessPacketThread = TRUE;
   m_hUDPProcessPacketThread = NULL;
   m_hUDPReceivePacketIO     = NULL;

   m_bUDPSendPacketThread = TRUE;
   m_hUDPSendPacketThread = NULL;
   m_hUDPSendPacketIO     = NULL;

   m_bLostConnThread = TRUE;
   m_hLostConnThread = NULL;
   m_hLostConnIO     = NULL;

   m_bServerStarted  = FALSE;

   m_pUDPClientConnections = NULL;


   m_bServerStarted  = TRUE; //client always started...
    
   InitializeCriticalSection(&m_crSendingLock);
   InitializeCriticalSection(&m_crConnectionLock);
   InitializeCriticalSection(&m_crMaintenanceLock);
   InitializeCriticalSection(&m_crInterLockCnt);
}

//--METHOD IMPLEMENTATION-------------------------------------------------
//
/*! Destructor
*/
//------------------------------------------------------------------------
NMPacketManager::~NMPacketManager()
{
   if(m_pUDP)
      m_pUDP->Disconnect();

   m_bUDPMaintenancePacket        = FALSE;
   m_bUDPProcessPacketThread      = FALSE;
   m_bUDPSendPacketThread         = FALSE;
   m_bLostConnThread              = FALSE;
      

   // Signal sending and interpretation queues to start running.
   if(m_hUDPReceivePacketIO)
      CancelIo( m_hUDPReceivePacketIO);
   if(m_hUDPSendPacketIO)
      CancelIo( m_hUDPSendPacketIO);
   if(m_hLostConnIO)
      CancelIo( m_hLostConnIO);
  

   //Detruit toutes les connections...
   EnterCriticalSection(&m_crConnectionLock);

   for(UINT i = 0; i < m_UDPConnectionsList.size(); i++ )
   {
      if(m_UDPConnectionsList[i].pConn)
         delete m_UDPConnectionsList[i].pConn;
      m_UDPConnectionsList[i].pConn = NULL;
   }
   m_UDPConnectionsList.clear();

   /*
   UDPConnectionMap::iterator i;
   for( i = m_UDPConnections.begin(); i != m_UDPConnections.end(); i++ )
   {
      CUDPConnection* lpConnection = (*i).second;
      delete lpConnection;
   }
   m_UDPConnections.erase( m_UDPConnections.begin(), m_UDPConnections.end() );
   */

   if(m_pUDPClientConnections)
      delete m_pUDPClientConnections;
   m_pUDPClientConnections = NULL;
   LeaveCriticalSection(&m_crConnectionLock);

   if(m_hUDPProcessPacketThread)
      WAIT_THREAD( m_hUDPProcessPacketThread );
   if(m_hUDPSendPacketThread)
      WAIT_THREAD( m_hUDPSendPacketThread );
   if(m_hUDPMaintenancePacket)
      WAIT_THREAD( m_hUDPMaintenancePacket );
   if(m_hLostConnThread)
      WAIT_THREAD( m_hLostConnThread );

  
   m_hUDPProcessPacketThread   = NULL;
   m_hUDPSendPacketThread      = NULL;
   m_hUDPMaintenancePacket     = NULL;
   m_hLostConnThread           = NULL;
   

	if(m_pUDP)
   {
      delete m_pUDP;
      m_pUDP = NULL;
   }
   

   DeleteCriticalSection(&m_crSendingLock);
   DeleteCriticalSection(&m_crConnectionLock);
   DeleteCriticalSection(&m_crMaintenanceLock);
   DeleteCriticalSection(&m_crInterLockCnt);
}

CUDPConnection* NMPacketManager::GetConnection(sockaddr_in sockAddr,bool bCreate)
{
   return m_pUDPClientConnections;
}



bool NMPacketManager::Init(pfProcessCallback pfProcessCallback,void *pThis,bool bStartAsServer,int iPort,char *pstrIP)
{
   if(m_bInit)
      return false;

   m_dwGlobalNbrRetry                = 0;
   m_dwGlobalNbrLost                 = 0;
   m_dwNbrPacketSend                 = 0;
   m_dwNbrPacketRecv                 = 0;
   m_dwNbrPacketRecvAlreadyRegistred = 0;

   m_pParent = pThis;

   m_pfProcessCallback = pfProcessCallback;

   m_pUDP = new UDPClient();
   m_pUDP->SetReceiveParameters(ReceiveUDPCallback,this,PACKET_MAX_SIZE);	// Receive buffer size

   int iErr;
   if(bStartAsServer)
   {
      iErr = m_pUDP->StartServer(iPort,false);
   }
   else
   {
      iErr = m_pUDP->Connect(pstrIP,iPort,true);
      if(!iErr)
      {
         if(m_pUDPClientConnections)
         {
            delete m_pUDPClientConnections;
            m_pUDPClientConnections = NULL;
         }
         
            sockaddr_in SocketInToSvr;
         m_pUDP->GetConnectSockIn(&SocketInToSvr);
         m_pUDPClientConnections = new CUDPConnection();
         m_pUDPClientConnections->InitializeComm(SocketInToSvr, this);
      }
#if defined(LINUX_PORT) && !defined(_WIN32)
      else
      {
         std::fprintf(stderr, "[COMM] UDPClient::Connect failed err=%d host=%s port=%d\n",
                      iErr, pstrIP ? pstrIP : "?", iPort);
      }
#endif
   }


   if(!iErr)
   {
      m_bInit               = true;
      m_uiNbrInterpQueueCnt = 0;
      
      m_hUDPMaintenancePacket   = (HANDLE)_beginthread( UDPMaintenancePacket  , 0, this );   
      
      m_hUDPReceivePacketIO = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 1 );
      m_hUDPSendPacketIO    = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 1 );
      m_hLostConnIO         = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 1 );
      
      m_hUDPProcessPacketThread = (HANDLE)_beginthread( UDPProcessPacketFct, 0, this );  
      m_hUDPSendPacketThread    = (HANDLE)_beginthread( UDPSendPacketFct, 0, this );  

   }

   return m_bInit;
}

int NMPacketManager::GetNbrConnection()
{
   int iNbr = 0;
   EnterCriticalSection(&m_crConnectionLock);
   if(m_pUDPClientConnections)
      iNbr = 1;
   LeaveCriticalSection(&m_crConnectionLock);
   return iNbr;
}

void NMPacketManager::ClearAllReceivedPacketIDs()
{
   EnterCriticalSection(&m_crConnectionLock);
   if(m_pUDPClientConnections)
      m_pUDPClientConnections->ResetReceivedPacketIDs();
   LeaveCriticalSection(&m_crConnectionLock);
}
 
bool NMPacketManager::GetConnectedSockIn(sockaddr_in *pSocketIn)
{
  if(!m_bInit)
     return false;
  
  m_pUDP->GetConnectSockIn(pSocketIn);
  return true;
}


int NMPacketManager::ReceiveUDPCallback(sockaddr_in sockAddr,unsigned char* pData, size_t DataLenght, int iQueueNbrItems, int iNbrPacketLost, void* pParam)
{
   NMPacketManager* me = reinterpret_cast<NMPacketManager*>(pParam);

   if(!me->m_bServerStarted) //server not started, we not accept packet...
      return 0;
   
   //AddToLogFile(TRUE,"-> %d bytes...(Qsize = %d) (lost = %d)\n",DataLenght,iQueueNbrItems,iNbrPacketLost);

   if( DataLenght < sizeof(PacketHeader) || DataLenght > PACKET_MAX_SIZE)
   {
      // DEBUG diagnostic : datagramme hors borne (trop petit/trop gros) rejete AVANT tout traitement.
      if (DataLenght >= 40)
      {
         char tb[96];
         sprintf_s(tb, 96, "UDP rejete (taille) nbuf=%d (min=%d max=%d)",
                   (int)DataLenght, (int)sizeof(PacketHeader), (int)PACKET_MAX_SIZE);
         EnterGameTrace(tb);
      }
      return 0;
   }

   // DEBUG diagnostic : trace de tout datagramme >= 40 octets recu au plus bas niveau (avant CRC),
   // pour determiner si les gros paquets (ex. opcode 13 ~90 o, en megapack) arrivent physiquement.
   if (DataLenght >= 40)
   {
      char hex[3 * 12 + 1]; hex[0] = 0;
      int dn = DataLenght < 12 ? (int)DataLenght : 12;
      for (int _i = 0; _i < dn; _i++)
      {
         char hb[4]; sprintf_s(hb, 4, "%02X ", (unsigned char)pData[_i]); strcat_s(hex, sizeof(hex), hb);
      }
      char tb[128];
      sprintf_s(tb, 128, "UDP recu nbuf=%d bytes=[%s]", (int)DataLenght, hex);
      EnterGameTrace(tb);
   }
   
   UDPPacket *pPacket = me->PacketAlloc(DataLenght);

   try
   {
      if(pPacket)
      {
         pPacket->ulBufferLength = DataLenght;
         memcpy(pPacket->pBuffer,pData,DataLenght);      //copie toutes les donnees
         pPacket->sockAddr = sockAddr;                   //copie le socket de celui qui a transmit

         //ici on valid le checksum dur les donnee autres que le headers...
         //tous les packet invalides seront filtrer ici...
         bool bCheckSumOK = me->IsChksumOK(pPacket->pBuffer,pPacket->ulBufferLength,pPacket->pHeader->uchCRC8);
         if(!bCheckSumOK)
         {
            AddToLogFile(TRUE,"   -> Invalid Checksum\n");
            // DEBUG diagnostic : un datagramme arrive mais est jete au CRC -> reparable cote client.
            if (DataLenght >= 40)
            {
               char tb[96];
               sprintf_s(tb, 96, "UDP rejete (CRC) nbuf=%d", (int)DataLenght);
               EnterGameTrace(tb);
            }
            me->PacketFree(pPacket); //RECV Le Packet a un MAUVAIS, traiter et detruit.
            return 0;
         }
         
         //we receive any packet from this USER, we reset the timeout of connection...
         EnterCriticalSection(&me->m_crConnectionLock);
         CUDPConnection* lpConnection = me->GetConnection( pPacket->sockAddr ,true);
         if(lpConnection )
            lpConnection->ResetTimeout();

         //si le packet demande un ACK, on envoie aussi le ACK
         if(pPacket->pHeader->ushNeedAck)
            me->SendAckPacket(pPacket);

         
         if(pPacket->ulBufferLength == sizeof(PacketHeader) && pPacket->pHeader->ushNeedAck == 0)
         {
            // on a recu un acknoledge... on cherche la parity de cet acknoledge et on elimine le pending
            if(lpConnection)
               lpConnection->DestroyPending(pPacket->pHeader->ushID);
            me->PacketFree(pPacket); //RECV Le Packet est un ACKNOLEDGE, traiter et detruit.
            LeaveCriticalSection(&me->m_crConnectionLock);
            return 0;
         }
         
         //Si c'est un packet splitter, on gere les fragment...
         if(pPacket->pHeader->ushSplit)
         {
            pPacket->pHeaderSplit = (PacketHeaderSplit*)(pPacket->pBuffer+sizeof(PacketHeader));
            pPacket->puchData     = (pPacket->pBuffer+sizeof(PacketHeader)+sizeof(PacketHeaderSplit));
            pPacket->ulDataLength = pPacket->ulBufferLength - (sizeof(PacketHeader)+sizeof(PacketHeaderSplit));

            if(pPacket->pHeader->ushCompress)
            {
               pPacket->pHeaderComp   = (PacketHeaderComp*)(pPacket->pBuffer+sizeof(PacketHeader)+sizeof(PacketHeaderSplit));
               pPacket->puchData     += sizeof(PacketHeaderComp);
               pPacket->ulDataLength -= sizeof(PacketHeaderComp);
            }
            
            //AddToLogFile(TRUE,"   -> Ack Split %d %d-%d\n",pPacket->pHeader->ushID,pPacket->pHeaderSplit->uchPartNbr,pPacket->pHeaderSplit->uchMaxPart);

            if(lpConnection)
               lpConnection->AddPacketFragment(pPacket);
            me->PacketFree(pPacket); //CV13:missing packet delete leack found 01
            LeaveCriticalSection(&me->m_crConnectionLock);
            return 0;
         }
         else
         {
            LeaveCriticalSection(&me->m_crConnectionLock);
            if(pPacket->pHeader->ushCompress)
            {
               pPacket->pHeaderComp  = (PacketHeaderComp*)(pPacket->pBuffer+sizeof(PacketHeader));
               pPacket->puchData     = (pPacket->pBuffer+sizeof(PacketHeader)+sizeof(PacketHeaderComp));
               pPacket->ulDataLength = pPacket->ulBufferLength - (sizeof(PacketHeader)+sizeof(PacketHeaderComp));
            }
            else
            {
               pPacket->puchData     = (pPacket->pBuffer+sizeof(PacketHeader));
               pPacket->ulDataLength = pPacket->ulBufferLength - sizeof(PacketHeader);
            }
            //On a un packet complet compresser ou non, on envoie dans la liste des packet a traiter
            //AddToLogFile(TRUE,"   -> Recived packet\n",pPacket->pHeader->ushID);
  		 
			
			me->PostReceivePacket(pPacket);
         }
      }
   }
   catch (...)
   {
      me->PacketFree(pPacket);
   }

   return 0;
}


void NMPacketManager::UDPProcessPacketFct(LPVOID lpData)
{
   NMPacketManager *me = (NMPacketManager *)lpData;

   while( me->m_bUDPProcessPacketThread )
   {
      
      DWORD dwFoo = 0;
      T4C_IOCP_KEY dwPacketAddr = 0;
#if defined(LINUX_PORT) && !defined(_WIN32)
      void* lpOverlapped = NULL;
#else
      OVERLAPPED* lpOverlapped = NULL;
#endif
      
        
      UDPPacket* pPacket=NULL;
      
      BOOL bRet = GetQueuedCompletionStatus( me->m_hUDPReceivePacketIO, &dwFoo, &dwPacketAddr, &lpOverlapped, WAIT_QUEUE_MAX ) ;

      if(bRet)
      {
        
         pPacket = reinterpret_cast< UDPPacket* >( dwPacketAddr );

         EnterCriticalSection(&me->m_crInterLockCnt);
         me->m_uiNbrInterpQueueCnt--;
         if(me->m_uiNbrInterpQueueCnt <0)
            me->m_uiNbrInterpQueueCnt = 0;
         LeaveCriticalSection(&me->m_crInterLockCnt);

         EnterCriticalSection(&me->m_crConnectionLock);
         CUDPConnection* lpConnection = me->GetConnection( pPacket->sockAddr ,false);
         if(lpConnection && lpConnection->AlreadyReceivedPacket(pPacket->pHeader->ushID) == false )
         {
            // Register the packet on the list of received packets
            lpConnection->RegisterReceivedPacketID(pPacket->pHeader->ushID);
            me->m_dwNbrPacketRecv++;
            LeaveCriticalSection(&me->m_crConnectionLock);
            //AddToLogFile(TRUE,"-> Register PacketID %d\n",pPacket->pHeader->ushID);
            // DEBUG diagnostic : ID accepte (pour correler les collisions de dedup).
            if (pPacket->ulBufferLength >= 12)
            {
               char tb[96];
               sprintf_s(tb, 96, "DEDUP OK ushID=%d nbuf=%d", (int)pPacket->pHeader->ushID, (int)pPacket->ulBufferLength);
               EnterGameTrace(tb);
            }
            me->AnalyzePacket( pPacket );
         }
         else
         {
            me->m_dwNbrPacketRecvAlreadyRegistred++;
            LeaveCriticalSection(&me->m_crConnectionLock);
            // DEBUG diagnostic : paquet jete car son ushID est deja dans l'historique de dedup.
            char tb[96];
            sprintf_s(tb, 96, "DEDUP DROP ushID=%d nbuf=%d%s",
                      (int)pPacket->pHeader->ushID, (int)pPacket->ulBufferLength,
                      lpConnection ? "" : " (conn NULL)");
            EnterGameTrace(tb);
         }
         
         //Packet a ete recu, traiter et detruit.
         me->PacketFree(pPacket); 

         Sleep(0);
      }
   }
}

void NMPacketManager::UDPSendPacketFct(LPVOID lpData)
{
   NMPacketManager *me = (NMPacketManager *)lpData;

   
   UDPPacket *pPacket = NULL;
   while( me->m_bUDPSendPacketThread )
   {
      
      DWORD dwFoo               = 0;
      T4C_IOCP_KEY dwPacketAddr = 0;
#if defined(LINUX_PORT) && !defined(_WIN32)
      void* lpOverlapped = NULL;
#else
      OVERLAPPED* lpOverlapped = NULL;
#endif
      
     
      BOOL bRet = GetQueuedCompletionStatus( me->m_hUDPSendPacketIO, &dwFoo, &dwPacketAddr, &lpOverlapped, WAIT_QUEUE_MAX ) ;
      if(bRet)
      {
         pPacket = reinterpret_cast< UDPPacket* >( dwPacketAddr );

         try
         {

            if(!pPacket->boDelete)
            {
               bool bSend = true;
               EnterCriticalSection(&me->m_crConnectionLock);
               CUDPConnection* lpConnection = me->GetConnection( pPacket->sockAddr ,false);
               if(lpConnection )
               {
                  if(pPacket->ulNbrAck >0)
                  {
                     pPacket->uldwTimeout = timeGetTime() + pPacket->ulAckDelay;
                     if(pPacket->boAddPending)
                     {
                        //a besoin d'un ACK et a forcement un retry...
                        if(!lpConnection->AddPending(pPacket))
                        {
                           //impossible ajouter le pending, on set comme si cetais un pack sans RETRY...
                           pPacket->ulAckDelay           = 0;
                           pPacket->ulNbrAck             = 0;
                           pPacket->pHeader->ushNeedAck  = 0;
                        }
                     }
                  }
               }
               else {
#if defined(LINUX_PORT) && !defined(_WIN32)
                  std::fprintf(stderr, "[COMM] UDPSend drop packID=%u (no connection)\n",
                               pPacket->pHeader ? static_cast<unsigned>(pPacket->pHeader->ushID) : 0u);
#endif
                  bSend = false; // no connection exist
               }
               LeaveCriticalSection(&me->m_crConnectionLock);
               
               if(bSend)
               {
                  if(pPacket->ulBufferLength >= sizeof(PacketHeader))
                     pPacket->pHeader->uchCRC8    = me->CalcChecksumComp2(pPacket->pBuffer,pPacket->ulBufferLength);
                                
                  AddToLogFile(FALSE,"<- Send PacketID %d [%d]\n",pPacket->pHeader->ushID,pPacket->sockAddr.sin_port);
                  
                  me->m_pUDP->SendTo(pPacket->sockAddr,(char*)pPacket->pBuffer,pPacket->ulBufferLength);
               }
               //Si pas de acknoledge, on delete le packet apres l'envoie sinon
               //on le reste vivant le temps de recevoir l'acknoledge...
               if(pPacket->ulNbrAck == 0)
                  me->PacketFree(pPacket); //SEND Le packet na plus de ACK, on peu le detruire.
            }
            else
            {
               me->PacketFree(pPacket); //SEND Le Packet est marquer a DELETE on le detruit sans l envoyer.
            }
         }
         catch (...)
         {
            me->PacketFree(pPacket);
         }

         Sleep(0);
      }
   }

}

void NMPacketManager::UDPMaintenancePacket(LPVOID lpData)
{
   NMPacketManager *me = (NMPacketManager *)lpData;

  
   while( me->m_bUDPMaintenancePacket )
   {
      Sleep( 500 );

      
      EnterCriticalSection(&me->m_crMaintenanceLock);
      EnterCriticalSection(&me->m_crConnectionLock);
      

      

      if(me->m_pUDPClientConnections)
      {
         me->m_pUDPClientConnections->VerifyTimedoutPending();
         // Verify and destroy fragmented packets that timedout before getting completed
         me->m_pUDPClientConnections->VerifyTimedoutFragments();
         //Verify if Mega Pack Timeout and need to send...
         me->m_pUDPClientConnections->VerifyMegaPack();
      }
      LeaveCriticalSection(&me->m_crConnectionLock);
      LeaveCriticalSection(&me->m_crMaintenanceLock);
   }
}

void NMPacketManager::UDPLostConnectionThread(LPVOID lpData)
{
   NMPacketManager *me = (NMPacketManager *)lpData;

   while( me->m_bLostConnThread )
   {
      DWORD dwFoo = 0;
      T4C_IOCP_KEY dwPacketAddr = 0;
#if defined(LINUX_PORT) && !defined(_WIN32)
      void* lpOverlapped = NULL;
#else
      OVERLAPPED* lpOverlapped = NULL;
#endif

      CUDPConnection* pConnection = NULL;
      BOOL bRet = FALSE;
      bRet = GetQueuedCompletionStatus( me->m_hLostConnIO, &dwFoo, &dwPacketAddr, &lpOverlapped, WAIT_QUEUE_MAX );
      if(bRet)
      {
         pConnection = reinterpret_cast< CUDPConnection* >( dwPacketAddr );
         if(pConnection)
         {
            AddToLogFile(FALSE,"***DELETE Connect Lost port(TIMEOUT) %d\n",pConnection->m_sockAddr.sin_port);
            delete pConnection;
            pConnection = NULL;
         }
         Sleep(100);
      }
   }
}

void NMPacketManager::OneShotEnableCommThread(LPVOID lpData)
{
   NMPacketManager *me = (NMPacketManager *)lpData;
   Sleep(20000);
   me->m_bServerStarted  = TRUE;
}



void NMPacketManager::SendUDPPacket( sockaddr_in sockAddr, unsigned char* pBuffer, int nBufferSize, 
                                DWORD dwAckDelay, DWORD dwMaxAck, bool bCompress,bool bAutoCompress,bool bSendDirect)
{
 
   EnterCriticalSection(&m_crSendingLock);

   //on check si le packet doit etre compresser en premier...
   //si oui on le compresse car une fois compresser il est possible
   //quil entre dans un fragment
   
   bool  bCompressed = false;
   if(bAutoCompress)
   {
      if(nBufferSize > 2000) 
         bCompressed = true;
   }
   else
   {
      bCompressed = bCompress;
   }

   BYTE *pCompressData = NULL;

   BYTE *pFinalData;
   DWORD dwFinalLength;

   PacketHeaderComp CompHeader;

   if(bCompressed)
   {
      ULONG dwTaille  = nBufferSize;
#if defined(LINUX_PORT) && !defined(_WIN32)
      uLongf dwTailleC = MEM_COMPRESS_NEED(nBufferSize);
#else
      ULONG dwTailleC = MEM_COMPRESS_NEED(nBufferSize);
#endif

      pCompressData = new BYTE[dwTailleC];
      if( compress(pCompressData, &dwTailleC, pBuffer, dwTaille) != Z_OK )
      {
         //ADD Log :Impossible de compresser on sort de ici le pack ne dois pas etre bon...
         if(pCompressData)
            delete []pCompressData;
         pCompressData = NULL;
         LeaveCriticalSection(&m_crSendingLock);//manque un leave ici...
         return;
      }

      //on valide que le buffer compresser est bien plus petit que le buffer original, si
      //ce n'est pas le cas, on enverra le buffer NON compresser
      if(dwTailleC < dwTaille)
      {
         //on utilise les data compressed...
         CompHeader.ulPackData   = dwTailleC;
         CompHeader.ulUnpackData = dwTaille;

         pFinalData    = pCompressData;
         dwFinalLength = dwTailleC;

      }
      else
      {
         bCompressed = false;
         pFinalData    = pBuffer;
         dwFinalLength = nBufferSize;
      }
   }
   else
   {
      pFinalData    = pBuffer;
      dwFinalLength = nBufferSize;
   }


   if(dwFinalLength <= PACKET_DATA_MAX) //None splitted packet...
   {
      unsigned short ushPackID = 0;
      EnterCriticalSection(&m_crConnectionLock);
      CUDPConnection* lpConnection = GetConnection( sockAddr ,false);
      if(!lpConnection)
      {
#if defined(LINUX_PORT) && !defined(_WIN32)
         if (nBufferSize >= 2) {
            const short opcode = static_cast<short>(pBuffer[0] | (pBuffer[1] << 8));
            std::fprintf(stderr, "[COMM] SendUDPPacket drop opcode %d (no UDP connection)\n",
                         static_cast<int>(opcode));
         }
#endif
         if(pCompressData)
            delete []pCompressData;
         pCompressData = NULL;

         LeaveCriticalSection(&m_crConnectionLock);
         LeaveCriticalSection(&m_crSendingLock);
         return;
      }
      ushPackID = lpConnection->GetPacketID();


      int iPacketLength = dwFinalLength+sizeof(PacketHeader);
      if(bCompressed)
         iPacketLength += sizeof(PacketHeaderComp);


      UDPPacket* pPacket = PacketAlloc(iPacketLength);

      pPacket->boAddPending = TRUE; // if ya des ack on enverra le pending...

      pPacket->puchData  = pPacket->pBuffer + sizeof(PacketHeader);
      if(bCompressed)
      {
         pPacket->pHeaderComp = (PacketHeaderComp*)(pPacket->pBuffer+sizeof(PacketHeader));
         pPacket->puchData += sizeof(PacketHeaderComp);
      }
      pPacket->ulBufferLength = iPacketLength;
      pPacket->ulAckDelay     = dwAckDelay;
      pPacket->ulNbrAck       = dwMaxAck;

      //fill header struct
      pPacket->sockAddr               = sockAddr; 
      pPacket->pHeader->ushID         = ushPackID;
      pPacket->pHeader->ushNeedAck    = 0;
      pPacket->pHeader->ushCompress   = 0;
      pPacket->pHeader->ushSplit      = 0;
      pPacket->pHeader->uchPartNumber = 0;

      if(pPacket->ulNbrAck >0)
         pPacket->pHeader->ushNeedAck  = 1;

      if(bCompressed)
      {
         // fill compressed struct
         pPacket->pHeader->ushCompress = 1;
         pPacket->pHeaderComp->ulPackData   = CompHeader.ulPackData;
         pPacket->pHeaderComp->ulUnpackData = CompHeader.ulUnpackData;
      }
 
      pPacket->ulDataLength = dwFinalLength;
      memcpy( pPacket->puchData, pFinalData, dwFinalLength );
     
      //Send the packet to Sending queue
      m_dwNbrPacketSend++;
      PostSendPacket(pPacket); //Client mode always send directly

      LeaveCriticalSection(&m_crConnectionLock);
   }
   else
   {
      //Split packet...
      //On dois creer des parties de packet et les envoyer...

      int numberOfPieces = (dwFinalLength / PACKET_DATA_MAX) + 1;
      if (numberOfPieces > PACKET_DATA_MAX_SPLIT_PART) 
      {
         if(pCompressData)
            delete []pCompressData;
         pCompressData = NULL;

         LeaveCriticalSection(&m_crSendingLock);
         return;
      }

      unsigned short ushPackID[PACKET_DATA_MAX_SPLIT_PART];
      unsigned short packetSplitNO = 0;
      EnterCriticalSection(&m_crConnectionLock);
      CUDPConnection* lpConnection = GetConnection( sockAddr ,false);
      if(lpConnection == NULL)
      {
         if(pCompressData)
            delete []pCompressData;
         pCompressData = NULL;

         LeaveCriticalSection(&m_crConnectionLock);
         LeaveCriticalSection(&m_crSendingLock);
         return;
      }
      int i=0;
      for(i = 0; i < numberOfPieces; ++i )
         ushPackID[i] = lpConnection->GetPacketID();
      packetSplitNO = lpConnection->GetPacketSplitID();
      LeaveCriticalSection(&m_crConnectionLock);

      // For all pieces...
      for(i = 0; i < numberOfPieces; ++i )
      {
         int pieceSize = 0;
         if( i == numberOfPieces - 1 )  // If this is the last piece
         {
            pieceSize = dwFinalLength % PACKET_DATA_MAX;
         } 
         else
         {
            pieceSize = PACKET_DATA_MAX;
         }

         int iPacketLength = pieceSize + sizeof(PacketHeader) + sizeof(PacketHeaderSplit);
         if(bCompressed)
            iPacketLength += sizeof(PacketHeaderComp);

         UDPPacket* pPacket = PacketAlloc(iPacketLength);

         pPacket->boAddPending = TRUE; // if ya des ack on enverra le pending...
         
         pPacket->pHeaderSplit = (PacketHeaderSplit*)(pPacket->pBuffer+sizeof(PacketHeader));
         pPacket->puchData  = pPacket->pBuffer + sizeof(PacketHeader) + sizeof(PacketHeaderSplit);
         if(bCompressed)
         {
            pPacket->pHeaderComp = (PacketHeaderComp*)(pPacket->pBuffer+sizeof(PacketHeader)+sizeof(PacketHeaderSplit));
            pPacket->puchData += sizeof(PacketHeaderComp);
         }

         pPacket->ulBufferLength = iPacketLength;

         memcpy( pPacket->puchData, pFinalData + (i*PACKET_DATA_MAX), pieceSize);
         pPacket->ulDataLength = pieceSize;
         pPacket->ulAckDelay     = dwAckDelay;
         pPacket->ulNbrAck       = dwMaxAck;

         //fill header struct
         pPacket->sockAddr               = sockAddr; 
         pPacket->pHeader->ushID         = ushPackID[i];
         pPacket->pHeader->ushNeedAck    = 0;
         pPacket->pHeader->ushCompress   = 0;
         pPacket->pHeader->ushSplit      = 1;
         pPacket->pHeader->uchPartNumber = i;
         
         if(pPacket->ulNbrAck >0)
            pPacket->pHeader->ushNeedAck  = 1;

         //Fill the split structure
         pPacket->pHeaderSplit->uchSplitNO  = packetSplitNO;
         pPacket->pHeaderSplit->uchPartNbr  = i;
         pPacket->pHeaderSplit->uchMaxPart  = numberOfPieces;
         pPacket->pHeaderSplit->ushDataSize = (unsigned short)dwFinalLength;
         
         if(bCompressed)
         {
            // fill compressed struct
            pPacket->pHeader->ushCompress = 1;
            pPacket->pHeaderComp->ulPackData   = CompHeader.ulPackData;
            pPacket->pHeaderComp->ulUnpackData = CompHeader.ulUnpackData;
         }
         
         //Send the packet to Sending queue
         m_dwNbrPacketSend++;
         PostSendPacket(pPacket); //packet fragmenter on send direct...
      }
   }

   if(pCompressData)
      delete []pCompressData;
   pCompressData = NULL;

   LeaveCriticalSection(&m_crSendingLock);
}


void NMPacketManager::SendAckPacket(UDPPacket* pPacket) 
{
   UDPPacket* pPacketAck = PacketAlloc(sizeof(PacketHeader));

   pPacketAck->ulBufferLength  = sizeof(PacketHeader);
      
   // The ack's ID is the ID of the packet it acknowledges.
   // And no flags :)
   pPacketAck->pHeader->ushID         = pPacket->pHeader->ushID;
   pPacketAck->pHeader->ushNeedAck    = 0;
   pPacketAck->pHeader->ushCompress   = 0;
   pPacketAck->pHeader->ushSplit      = 0;
   pPacketAck->pHeader->uchCRC8       = 0x00;
   pPacketAck->pHeader->uchPartNumber = pPacket->pHeader->uchPartNumber;
   // Send packet back to its destination
   pPacketAck->sockAddr = pPacket->sockAddr;   //copie le socket de celui qui a transmit
   PostSendPacket(pPacketAck); //send direct les ACK...
}



void NMPacketManager::AnalyzePacket(UDPPacket *pPacket)
{
   //rendu ici on a un packet complet compresser ou non...
   //on peu donc appeler le callback avec les donnee de ce 
   //packet pour terminer par supprimer ce packet...

   if(pPacket->pHeader->ushCompress)
   {
      if(pPacket->pHeaderComp->ulUnpackData < 0x00800000) //8 megs
      {
         //on alloue un buffer temporaire pour decompresset le ZIP...
         BYTE *pDataUnpack = new BYTE[pPacket->pHeaderComp->ulUnpackData];
      

         uLongf ulfUnpack = pPacket->pHeaderComp->ulUnpackData;
         if(uncompress((BYTE*)pDataUnpack,&ulfUnpack,(BYTE*)pPacket->puchData,pPacket->pHeaderComp->ulPackData) == 0)
         {
            pPacket->pHeaderComp->ulUnpackData = (unsigned int)ulfUnpack;
            //AddToLogFile(TRUE,"-> Analyse COmpressed Data: PACK = %d, UNPACK = %d   ==GAIN = %d bytes\n",pPacket->pHeaderComp->ulPackData,pPacket->pHeaderComp->ulUnpackData,pPacket->pHeaderComp->ulUnpackData-pPacket->pHeaderComp->ulPackData);
            //AddToLogFile(TRUE,"-> Analyse Data: ID = %d\n",pPacket->pHeader->ushID);
            if(m_pfProcessCallback)
               m_pfProcessCallback( m_pParent,pPacket->sockAddr, pDataUnpack, pPacket->pHeaderComp->ulUnpackData,0);
         }
         else
         {
            AddToLogFile(TRUE,"-> Unable to undompress packet\n");
         }

         if(pDataUnpack)
            delete []pDataUnpack;
         pDataUnpack = NULL;
      }
      else
      {
         //ADD LOG:  taille compresser invalide...
      }

   }
   else
   {
      //AddToLogFile(TRUE,"-> Analyse Data: ID = %d\n",pPacket->pHeader->ushID);
      //make a validation if is a MEGAPack



      if(pPacket->ulDataLength >4)
      {
         unsigned short ushPacketID = Reverse(*(short *)&pPacket->puchData[0]);
         if(ushPacketID == 0xFFFF)
         {
            unsigned short ushNbr = Reverse(*(short *)&pPacket->puchData[2]);
            //AddToLogFile(TRUE,"-> Analyse MEGAPACK: ID = %d (nbr sub packet = %d)\n",pPacket->pHeader->ushID,ushNbr);
            //on a un mega pack, on dois traiter tous ses packet...
            unsigned char *pCurDataPos = pPacket->puchData+4;
            unsigned char *pDataEnd    = pPacket->puchData + pPacket->ulDataLength;
            unsigned long ulDataLen    = 0;
            for(int i=0;i<ushNbr;i++)
            {
               if (pCurDataPos + 2 > pDataEnd)
                  break;
               ulDataLen = Reverse(*(short *)&pCurDataPos[0]); pCurDataPos+=2;
               if (ulDataLen == 0 || pCurDataPos + ulDataLen > pDataEnd)
                  break;

               if(m_pfProcessCallback)
                  m_pfProcessCallback( m_pParent,pPacket->sockAddr, pCurDataPos, ulDataLen,0);

               pCurDataPos+= ulDataLen;
            }
         }
         else
         {
            //packet normal on traite...
            if(m_pfProcessCallback)
               m_pfProcessCallback( m_pParent,pPacket->sockAddr, pPacket->puchData, pPacket->ulDataLength,0);
         }
      }
      else
      {
         if(m_pfProcessCallback)
            m_pfProcessCallback( m_pParent,pPacket->sockAddr, pPacket->puchData, pPacket->ulDataLength,0);
      }
   }
}


UDPPacket *NMPacketManager::PacketAlloc(int size)
{
   int iErr = 1;
   UDPPacket *packet = new UDPPacket;
   if (packet) 
   {
      packet->uldwTimeoutMegaT= 0x00000000;
      packet->uldwTimeoutMegaE= 0x00000000;
      packet->uldwTimeout     = 0xFFFFFFFF;
      packet->pBuffer = new unsigned char[size];
      if (packet->pBuffer) 
      {

         packet->boDelete       = FALSE;
         packet->boAddPending   = FALSE;
         packet->ulBufferLength = 0;
         packet->pHeader        = (PacketHeader*)packet->pBuffer;
         packet->pHeaderSplit   = NULL;
         packet->pHeaderComp    = NULL;
         packet->puchData       = NULL;
         packet->ulDataLength   = 0;
         iErr = 0;
      }
   }

   if(iErr)
   {
      PacketFree(packet); //ALLOC  Ce packet c'est mal alouer donc detruit.
      packet = NULL;
   }
   return(packet);
}

void NMPacketManager::PacketFree(UDPPacket *packet)
{
   if ( packet ) 
   {
      if ( packet->pBuffer )
      {
         delete []packet->pBuffer;
         packet->pBuffer = NULL;
      }
      delete packet; 
      packet = NULL;
   }
}

void NMPacketManager::PacketAllocAndCopy( UDPPacket *pNPacket,UDPPacket *packet)
{
   pNPacket->sockAddr = packet->sockAddr;
   memcpy(pNPacket->pBuffer,packet->pBuffer,packet->ulBufferLength);
   pNPacket->ulBufferLength    = packet->ulBufferLength;
   pNPacket->ulDataLength      = packet->ulDataLength;
   pNPacket->boDelete          = packet->boDelete;
   pNPacket->uldwTimeout       = packet->uldwTimeout;
   pNPacket->uldwTimeoutMegaE  = packet->uldwTimeout;
   pNPacket->uldwTimeoutMegaT  = packet->uldwTimeout;
   pNPacket->ulAckDelay        = packet->ulAckDelay;
   pNPacket->ulNbrAck          = packet->ulNbrAck;
   pNPacket->boAddPending      = packet->boAddPending;

   //Copie le header...
   pNPacket->pHeader->ushID         = packet->pHeader->ushID;
   pNPacket->pHeader->ushNeedAck    = packet->pHeader->ushNeedAck;
   pNPacket->pHeader->ushCompress   = packet->pHeader->ushCompress;
   pNPacket->pHeader->ushSplit      = packet->pHeader->ushSplit;
   pNPacket->pHeader->uchCRC8       = packet->pHeader->uchCRC8;
   pNPacket->pHeader->uchPartNumber = packet->pHeader->uchPartNumber;
   pNPacket->puchData     = (pNPacket->pBuffer+sizeof(PacketHeader));

   if(pNPacket->pHeader->ushSplit)
   {
      pNPacket->pHeaderSplit = (PacketHeaderSplit*)(pNPacket->pBuffer+sizeof(PacketHeader));
      pNPacket->pHeaderSplit->uchSplitNO  = packet->pHeaderSplit->uchSplitNO;
      pNPacket->pHeaderSplit->uchPartNbr  = packet->pHeaderSplit->uchPartNbr;
      pNPacket->pHeaderSplit->uchMaxPart  = packet->pHeaderSplit->uchMaxPart;
      pNPacket->pHeaderSplit->ushDataSize = packet->pHeaderSplit->ushDataSize;
      pNPacket->puchData     = (pNPacket->pBuffer+sizeof(PacketHeader)+sizeof(PacketHeaderSplit));
   }
   if(pNPacket->pHeader->ushCompress)
   {
      if(pNPacket->pHeader->ushSplit)
      {
         pNPacket->pHeaderComp = (PacketHeaderComp*)(pNPacket->pBuffer+sizeof(PacketHeader)+sizeof(PacketHeaderSplit));
         pNPacket->puchData     = (pNPacket->pBuffer+sizeof(PacketHeader)+sizeof(PacketHeaderSplit)+sizeof(PacketHeaderComp));
      }
      else
      {
         pNPacket->pHeaderComp = (PacketHeaderComp*)(pNPacket->pBuffer+sizeof(PacketHeader));
         pNPacket->puchData     = (pNPacket->pBuffer+sizeof(PacketHeader)+sizeof(PacketHeaderComp));
      }

      pNPacket->pHeaderComp->ulUnpackData = packet->pHeaderComp->ulUnpackData;
      pNPacket->pHeaderComp->ulPackData   = packet->pHeaderComp->ulPackData;
   }


}

unsigned char  NMPacketManager::CalcChecksumComp2(unsigned char	*pData, unsigned long dwNbrData)
{
   if(!pData || dwNbrData <=0)
      return 0x00; // pas de donn?es a calculer
   
   unsigned char chkSum;
   unsigned char sumByte = 0xAA;
   // on additionne tous les BYTES
   for(unsigned long i=0;i<dwNbrData;i++)
   {
      if(i != 2) //Skip le checksum bytes...
         sumByte = sumByte+ *(unsigned char *)&pData[i];
   }
   
   chkSum = 0x100 - (sumByte);
   
   return chkSum;
}

bool NMPacketManager::IsChksumOK(unsigned char *pData, unsigned long dwNbrData,unsigned char uchCheckSum)
{
   try
   {
      if(!pData || dwNbrData <=0 || dwNbrData > PACKET_MAX_SIZE)
         return false; // pas de donn?es a calculer

      unsigned char sumByte = 0xAA;
      // on additionne tous les BYTES
      for(unsigned long i=0;i<dwNbrData;i++)
      {
         if(i != 2) //Skip le checksum bytes...
            sumByte = sumByte+ *(unsigned char *)&pData[i];
      }
      unsigned char uchVal = sumByte+uchCheckSum;
      if(uchVal)
         return false;

      return true;
   }
   catch (...)
   {
      return false;
   }
}



// Put the packet on the queue for receiving
inline void NMPacketManager::PostReceivePacket (UDPPacket* pPacket)
{
   EnterCriticalSection(&m_crInterLockCnt);
   m_uiNbrInterpQueueCnt++;
   LeaveCriticalSection(&m_crInterLockCnt);

   PostQueuedCompletionStatus( m_hUDPReceivePacketIO, 0, T4C_IOCP_PTR(pPacket), NULL );
}

inline void NMPacketManager::PostSendPacket (UDPPacket* pPacket) 
{
   PostQueuedCompletionStatus( m_hUDPSendPacketIO, 0, T4C_IOCP_PTR(pPacket), NULL );
}

inline void NMPacketManager::PostSendPacketMega (UDPPacket* pPacket) 
{
   //Allocate new packet here and stay megapack clean...
   UDPPacket *pMegaPack = PacketAlloc(PACKET_MAX_SIZE);
   PacketAllocAndCopy(pMegaPack,pPacket);
   PostQueuedCompletionStatus( m_hUDPSendPacketIO, 0, T4C_IOCP_PTR(pMegaPack), NULL );
}

inline void NMPacketManager::PostLostConnection  (CUDPConnection* pConnection)
{
   PostQueuedCompletionStatus( m_hLostConnIO, 0, T4C_IOCP_PTR(pConnection), NULL );
}

int NMPacketManager::GetIntrQueueSize()
{
   return m_uiNbrInterpQueueCnt;
}

vector< sockaddr_in > *NMPacketManager::GetLostConnections( void )
{
   if(m_pUDP)
   {
	   return m_pUDP->GetLostConnections();
   }

   return NULL;
}
void NMPacketManager::FreeLostConnections(BOOL boFlushList)
{
   if(m_pUDP)
   {
	   m_pUDP->FreeLostConnections(boFlushList);
   }
}

//---------- START   OF CUDPConnection METHODS ------------------
CUDPConnection::CUDPConnection()
{
    m_pNMPacketManager = NULL;
    m_ushCurrentID      = 0;
    m_ushCurrentSplitID = 0;
    m_ushOffsetID       = 0;
    
    for(int i=0;i<NBR_RECEIVED_REGISTRED_ID;i++)
       m_lReceivedPacketID[i] = -1;
    
    m_dwConnectionTimeout = timeGetTime() + PACKET_BACKLOG_TIMEOUT;

    m_pMegaPack = NULL;
 
    InitializeCriticalSection(&m_crPendingLock);
    InitializeCriticalSection(&m_crSplitLock);
    InitializeCriticalSection(&m_crMegaPackLock);
}

CUDPConnection::~CUDPConnection( void )
{

    DestroyAllPending();

    if(m_pNMPacketManager)
      m_pNMPacketManager->PacketFree(m_pMegaPack);
 
    DeleteCriticalSection(&m_crPendingLock);
    DeleteCriticalSection(&m_crSplitLock);
    DeleteCriticalSection(&m_crMegaPackLock);

}

int CUDPConnection::InitializeComm(const sockaddr_in &sAddr, NMPacketManager *pNMPacketManager)
{
   m_sockAddr         = sAddr;
   m_pNMPacketManager = pNMPacketManager;
  
   return 0;
}

void CUDPConnection::ResetTimeout()
{
   m_dwConnectionTimeout = timeGetTime() + PACKET_BACKLOG_TIMEOUT;
}

inline WORD CUDPConnection::GetPacketID() 
{
   // Get Packet ID
   ++m_ushCurrentID;
   if ( m_ushCurrentID  >= PACKET_ID_MAX) 
   {
      m_ushCurrentID = 0;
   }
   return m_ushCurrentID;
}

inline WORD CUDPConnection::GetPacketSplitID() 
{
   // Get Packet ID
   ++m_ushCurrentSplitID;
   if ( m_ushCurrentSplitID  >= NBR_PACKET_SPLIT_MAX) 
   {
      m_ushCurrentSplitID = 0;
   }
   return m_ushCurrentSplitID;
}


inline void CUDPConnection::RegisterReceivedPacketID(unsigned short ushPacketID) 
{
   // Register the packet id
   m_lReceivedPacketID[m_ushOffsetID] = ushPacketID;

   m_ushOffsetID++;
   if (m_ushOffsetID >= NBR_RECEIVED_REGISTRED_ID) 
       m_ushOffsetID = 0; // We reached the end. Restart.
}

inline bool CUDPConnection::AlreadyReceivedPacket(unsigned short ushPacketID) 
{
   int iEnd = m_ushOffsetID;
   int iCnt = m_ushOffsetID;
   do
   {
      iCnt--;
      if (iCnt < 0) 
         iCnt = NBR_RECEIVED_REGISTRED_ID - 1;

      if (m_lReceivedPacketID[iCnt] == ushPacketID) 
         return true;

   }while (iCnt != iEnd);
   return false;
}

void CUDPConnection::ResetReceivedPacketIDs()
{
   // Reinitialise le buffer circulaire de dedup : aucun ID n'est plus considere comme « deja recu ».
   for (int i = 0; i < NBR_RECEIVED_REGISTRED_ID; i++)
      m_lReceivedPacketID[i] = -1;
   m_ushOffsetID = 0;
}

inline bool CUDPConnection::AddPending(UDPPacket* pPending) 
{
   bool bAdd = true;
   EnterCriticalSection(&m_crPendingLock);
   if (m_pendingPackets.size() < NBR_PACKET_PENDING_MAX) 
   {
      bAdd = true;
      m_pendingPackets.push_back(pPending);
   } 
   else 
   {
      bAdd = false;
   }
   LeaveCriticalSection(&m_crPendingLock);

   return bAdd;
}

bool CUDPConnection::DestroyPending (WORD packetID) 
{
   if(!m_pNMPacketManager)
      return false;

   EnterCriticalSection(&m_crPendingLock);
   vector<UDPPacket*>::iterator i = m_pendingPackets.begin();
   while (i != m_pendingPackets.end()) 
   {
      if ((*i)->pHeader->ushID == packetID) 
      {
         (*i)->boDelete = TRUE;
         /*
         UDPPacket* pPending = (*i);
         i = m_pendingPackets.erase(i);
         */
         LeaveCriticalSection(&m_crPendingLock);
         return true;
      }
      ++i;
   }
   LeaveCriticalSection(&m_crPendingLock);
   return false;
}

void CUDPConnection::DestroyAllPending() 
{
   if(!m_pNMPacketManager)
      return ;

   vector<UDPPacket*>::iterator iP = m_pendingPackets.begin();
   while ( iP != m_pendingPackets.end() ) 
   {
      UDPPacket* pPacket = (*iP);
      m_pNMPacketManager->PacketFree(pPacket);
      ++iP;
   }
   m_pendingPackets.clear();

   //destroy all fragment
   for (UINT iFrag=0; iFrag < m_FragmentsPackets.size();iFrag++) 
      m_pNMPacketManager->PacketFree(m_FragmentsPackets[iFrag].pPak);
   m_FragmentsPackets.clear();
}

inline bool CUDPConnection::ConnectionHasTimedout() 
{
   if (m_dwConnectionTimeout <= timeGetTime()) 
   {
      if ( m_pendingPackets.empty()  && m_FragmentsPackets.empty()) 
      {
         return true;
      }
   }
   return false;
}

inline bool CUDPConnection::AddPacketFragment(UDPPacket* pFragment)
{
   if(!m_pNMPacketManager)
      return false;
   
   bool bAdd = true;
   EnterCriticalSection(&m_crSplitLock);
   if (m_FragmentsPackets.size() < NBR_PACKET_SPLIT_MAX) 
   {
      // on valide que le packet n'est pas deja la...
      vector<UDPPacketSplit>::iterator itF = m_FragmentsPackets.begin();
      for(UINT i=0;i<m_FragmentsPackets.size();i++,itF++)
      {
         if(m_FragmentsPackets[i].pPak->pHeaderSplit->uchSplitNO == pFragment->pHeaderSplit->uchSplitNO)
         {
            int iPos = pFragment->pHeaderSplit->uchPartNbr*PACKET_DATA_MAX;
            if(iPos+pFragment->ulDataLength <= m_FragmentsPackets[i].pPak->ulDataLength)
            {
               m_FragmentsPackets[i].pPak->pHeaderSplit->uchPartNbr++;
               memcpy(m_FragmentsPackets[i].pPak->puchData+iPos,pFragment->puchData,pFragment->ulDataLength);
               m_FragmentsPackets[i].lPartReceive.push_back(pFragment->pHeaderSplit->uchPartNbr);

               //on valide si on a recu tous les fragment d<un packet...
               if(m_FragmentsPackets[i].pPak->pHeaderSplit->uchPartNbr == m_FragmentsPackets[i].pPak->pHeaderSplit->uchMaxPart)
               {
                  //on a recu tous les fragment de ce packet...on peu deleter de la liste ce packet
                  //et envoyer le PACK au receive packet complet...
                  UDPPacket *pNewComplete = m_FragmentsPackets[i].pPak;
                  itF = m_FragmentsPackets.erase(itF);

                  m_pNMPacketManager->PostReceivePacket(pNewComplete);
               }
            }
            
            LeaveCriticalSection(&m_crSplitLock);
            return bAdd;
         }
      }

      //le packet existe pas, c<est le premier fragment que l'on recoit...
      //on cree un packet temporaire qui contiendra toutes les donnee deu packet...
      int iTotalSize = pFragment->pHeaderSplit->ushDataSize+sizeof(PacketHeader)+sizeof(PacketHeaderSplit);

      if(pFragment->pHeader->ushCompress)
         iTotalSize += sizeof(PacketHeaderComp);

      UDPPacketSplit NewSplit;
      NewSplit.pPak = m_pNMPacketManager->PacketAlloc(iTotalSize);
      NewSplit.pPak->ulBufferLength = iTotalSize;
      NewSplit.pPak->puchData       = NewSplit.pPak->pBuffer + sizeof(PacketHeader) + sizeof(PacketHeaderSplit);
      NewSplit.pPak->ulDataLength   = pFragment->pHeaderSplit->ushDataSize;  

      NewSplit.pPak->pHeaderSplit = (PacketHeaderSplit*)(NewSplit.pPak->pBuffer+sizeof(PacketHeader));

      NewSplit.pPak->pHeader->ushCompress = pFragment->pHeader->ushCompress;
      NewSplit.pPak->pHeader->ushID       = pFragment->pHeader->ushID;
      NewSplit.pPak->pHeader->ushNeedAck  = 0;

      if(pFragment->pHeader->ushCompress)
      {
         NewSplit.pPak->pHeaderComp = (PacketHeaderComp*)(NewSplit.pPak->pBuffer+sizeof(PacketHeader)+sizeof(PacketHeaderSplit));
         NewSplit.pPak->puchData += sizeof(PacketHeaderComp);

         NewSplit.pPak->pHeaderComp->ulPackData   = pFragment->pHeaderComp->ulPackData;
         NewSplit.pPak->pHeaderComp->ulUnpackData = pFragment->pHeaderComp->ulUnpackData;
      }

      NewSplit.pPak->pHeaderSplit->uchSplitNO  = pFragment->pHeaderSplit->uchSplitNO;
      NewSplit.pPak->pHeaderSplit->uchPartNbr  = 1;
      NewSplit.pPak->pHeaderSplit->uchMaxPart  = pFragment->pHeaderSplit->uchMaxPart;
      NewSplit.pPak->pHeaderSplit->ushDataSize = pFragment->pHeaderSplit->ushDataSize;

      //copie la partie de data dans le gros bufer now...
      int iPos = pFragment->pHeaderSplit->uchPartNbr*PACKET_DATA_MAX;
      memcpy(NewSplit.pPak->puchData+iPos,pFragment->puchData,pFragment->ulDataLength);

      NewSplit.pPak->sockAddr = pFragment->sockAddr;
      NewSplit.pPak->uldwTimeout = timeGetTime() + PACKET_SPLIT_TIMEOUT;
      NewSplit.lPartReceive.push_back(pFragment->pHeaderSplit->uchPartNbr);
      m_FragmentsPackets.push_back(NewSplit);
   } 
   else 
   {
      bAdd = false;
      //ADD Log : le pending ets plein... etrange, mais on en mets plus dans la liste...
   }
   LeaveCriticalSection(&m_crSplitLock);
   
   return bAdd;
}

inline void CUDPConnection::VerifyTimedoutPending()
{
   if(!m_pNMPacketManager)
      return;

   UDPPacket* pResentPending = NULL;
   DWORD dwCurrentTime = timeGetTime();
   
      
   EnterCriticalSection(&m_crPendingLock);


   vector<UDPPacket*>::iterator i = m_pendingPackets.begin();
   while ( i != m_pendingPackets.end() ) 
   {
      if ( (*i)->uldwTimeout <= dwCurrentTime ) 
      {
         pResentPending = *i;
         if(pResentPending)
         {
            // It timed out, lets get it ready for retransmission!
            pResentPending->uldwTimeout = 0xFFFFFFFF;
            pResentPending->ulNbrAck--;
            pResentPending->boAddPending = FALSE; //les retry on ajoute jamnais au pending...
            if(pResentPending->ulNbrAck == 0 || pResentPending->boDelete)
            { 
               if(!pResentPending->boDelete)
                  m_pNMPacketManager->m_dwGlobalNbrLost++;

               pResentPending->ulAckDelay  = 0;
               pResentPending->boDelete = TRUE;
               pResentPending->pHeader->ushNeedAck = 0; // plus d'ack...

               //on delete ce pack du pending et on set le delete a TRUS donc sera detruit et pas envoyer
               vector<UDPPacket*>::iterator iDelete(i);
               i = m_pendingPackets.erase(iDelete);

               //on delete de la liste de pending, mais on ne delete pas le packet le SEND s<en chargera,...
               //car on set le packet avec PAS de RETRY...
            } 
            else
            {
               m_pNMPacketManager->m_dwGlobalNbrRetry++;
               pResentPending->ulAckDelay += (pResentPending->ulAckDelay/4); //add 25% of delay to timeout if connection are slow...
               ++i;
            }
            
            
            //resent this packet ID... and keep it in pending list
            m_pNMPacketManager->PostSendPacket(pResentPending);//resend direct le spacket retry...
         }
         else
         {
            ++i;
         }
      } 
      else 
      {
         ++i;
      }
   }
   LeaveCriticalSection(&m_crPendingLock);
}

inline void CUDPConnection::VerifyTimedoutFragments()
{
   if(!m_pNMPacketManager)
      return;
   
   DWORD dwCurrentTime = timeGetTime();
   
   EnterCriticalSection(&m_crSplitLock);
   
   
   vector<UDPPacketSplit>::iterator itF = m_FragmentsPackets.begin();
   while ( itF != m_FragmentsPackets.end() ) 
   {
      if ( (*itF).pPak->uldwTimeout <= dwCurrentTime ) 
      {
         UDPPacket *pNewComplete = (*itF).pPak;
         m_pNMPacketManager->PacketFree(pNewComplete);
         itF = m_FragmentsPackets.erase(itF);
      } 
      else 
      {
         ++itF;
      }
   }
   LeaveCriticalSection(&m_crSplitLock);
}



inline bool CUDPConnection::AddPacketToMegaPack(UDPPacket* pPack)
{
   if(!m_pNMPacketManager)
      return false;

   EnterCriticalSection(&m_crMegaPackLock);


   //on regarde si on peu ajouter ou pas...
   //si le pack est full on send le pack courant et on vas en creer un nouveau...
   if(m_pMegaPack && m_pMegaPack->ulBufferLength + pPack->ulDataLength+2 >= PACKET_DATA_MAX)
   {
      //le packet courant est plein...
      //AddToLogFile(FALSE,"Send megaPack SIZE");

      //CV14: Add to megapack send and clear the megapack here...
      m_pNMPacketManager->PostSendPacket(m_pMegaPack); 
      //m_pNMPacketManager->PostSendPacketMega(m_pMegaPack); 
      //m_pNMPacketManager->PacketFree(m_pMegaPack);

      m_pMegaPack = NULL;//on delete pas, le pointeur dois rester en vie on fait que resetter le ptr local...
   }

   if(m_pMegaPack == NULL)
   {
      //On dois creer le packet
      m_pMegaPack = m_pNMPacketManager->PacketAlloc(PACKET_MAX_SIZE);
      m_pMegaPack->puchData  = m_pMegaPack->pBuffer + sizeof(PacketHeader);

      m_pMegaPack->ulBufferLength = sizeof(PacketHeader)+4; //header + megapack headerID +nbrPart
      m_pMegaPack->ulDataLength   = 4;
      m_pMegaPack->ulAckDelay     = 1500;
      m_pMegaPack->ulNbrAck       = 3;
      m_pMegaPack->boAddPending   = TRUE;

      //fill header struct
      m_pMegaPack->sockAddr               = pPack->sockAddr; 
      m_pMegaPack->pHeader->ushID         = pPack->pHeader->ushID;
      m_pMegaPack->pHeader->ushNeedAck    = 1;
      m_pMegaPack->pHeader->ushCompress   = 0;
      m_pMegaPack->pHeader->ushSplit      = 0;
      m_pMegaPack->pHeader->uchPartNumber = 0;

      unsigned short ushNbrPack = 0;
      unsigned short ushMegaID  = 0xFFFF;
      *(unsigned short  *)&m_pMegaPack->puchData[ 0]  = Reverse(ushMegaID);
      *(unsigned short  *)&m_pMegaPack->puchData[ 2]  = Reverse(ushNbrPack);

      m_pMegaPack->uldwTimeoutMegaT = timeGetTime()+250; //1 second maximum for mega pack construction...
   }

   //ici on est sure de pouvoir ajouter
   //recupere le nbr de partie
   unsigned short  ushNbrPack = Reverse(*(short *)&m_pMegaPack->puchData[2]);
   ushNbrPack++;
   //reecris le bon nombre de part...
   *(unsigned short  *)&m_pMegaPack->puchData[ 2]  = Reverse(ushNbrPack);

   //maintenant ecrit la part dans le mega pack...
   unsigned char *pPosData = m_pMegaPack->puchData+m_pMegaPack->ulDataLength;
   *(unsigned short  *)&pPosData[0]  = Reverse((unsigned short)pPack->ulDataLength); //write the datalen...
   pPosData+=2;

   memcpy(pPosData,pPack->puchData,pPack->ulDataLength);
   m_pMegaPack->ulBufferLength = m_pMegaPack->ulBufferLength + pPack->ulDataLength+2;
   m_pMegaPack->ulDataLength   = m_pMegaPack->ulDataLength   + pPack->ulDataLength+2;

   //reset the timeout...
   m_pMegaPack->uldwTimeoutMegaE = timeGetTime()+50; //1 second maximum for mega pack construction...

   LeaveCriticalSection(&m_crMegaPackLock);
   return true;
}

inline void CUDPConnection::VerifyMegaPack()
{
   if(!m_pNMPacketManager)
      return;

   EnterCriticalSection(&m_crMegaPackLock);
   if(m_pMegaPack)
   {
      if(m_pMegaPack->uldwTimeoutMegaT < timeGetTime() || m_pMegaPack->uldwTimeoutMegaE < timeGetTime())
      {
         //exoirere on send le packet
         //AddToLogFile(FALSE,"Send megaPack Timeout");

         //CV14: Add to megapack send and clear the megapack here...
         m_pNMPacketManager->PostSendPacket(m_pMegaPack); 
         //m_pNMPacketManager->PostSendPacketMega(m_pMegaPack); 
         //m_pNMPacketManager->PacketFree(m_pMegaPack);
         m_pMegaPack = NULL;
      }
   }
   LeaveCriticalSection(&m_crMegaPackLock);
}


