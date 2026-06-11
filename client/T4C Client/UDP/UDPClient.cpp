#include "UDPClient.h"
#include <iostream>
#if !defined(LINUX_PORT) || defined(_WIN32)
#include <process.h>
#endif
#if defined(LINUX_PORT) && !defined(_WIN32)
#include <cctype>
#include <cstdio>
#include <cerrno>
#include <cstring>
#endif

unsigned char COMM_key[]={
      0xC4 ,0x9B ,0x1B ,0x30 ,0xC5 ,0x0D ,0x58 ,0xB7, 0x10 ,0xC6 ,0xFD ,0x78 ,0xBB ,0x11 ,0x89 ,0xF3,
      0xCE ,0x7F ,0x33 ,0x3D ,0xA9 ,0x3F ,0xBA ,0x99, 0x55 ,0x9B ,0xD4 ,0x30 ,0x9F ,0xD8 ,0x55 ,0x54,
      0x4A ,0xED ,0x94 ,0xA3 ,0x33 ,0x09 ,0x4A ,0xE1, 0x08 ,0x00 ,0x01 ,0xA8 ,0x50 ,0x1E ,0xF6 ,0x82,
      0x05 ,0xC5 ,0xD6 ,0x5E ,0xF6 ,0x56 ,0x47 ,0x57, 0xAF ,0x49 ,0x82 ,0x5A ,0x98 ,0xC7 ,0x34 ,0xF1,
      0xE0 ,0x4A ,0xB4 ,0x96 ,0x7A ,0xEF ,0x2D ,0x2C, 0x3D ,0x66 ,0x60 ,0x99 ,0x42 ,0x52 ,0x83 ,0x53,
      0x74 ,0xFF ,0xA8 ,0x17 ,0xCF ,0xC6 ,0x82 ,0x4D, 0x57 ,0x16 ,0xB3 ,0xEB ,0x42 ,0xFD ,0x26 ,0x8B,
      0x05 ,0x64 ,0x2A ,0xC6 ,0xE0 ,0x12 ,0x33 ,0xDC, 0x70 ,0x6D ,0x38 ,0x19 ,0x20 ,0x1D ,0xA9 ,0x1B,
      0x03 ,0x89 ,0xA8 ,0xBA ,0x30 ,0x0D ,0x93 ,0xC9, 0xC5 ,0xCF ,0xF1 ,0xE2 ,0x1A ,0xD7 ,0x1D ,0xB1,
      0x2D ,0x3C ,0xBA ,0x29 ,0x91 ,0x53 ,0x1D ,0x60, 0x54 ,0xD5 ,0x5D ,0x0D ,0xCC ,0x43 ,0xE2 ,0xD9,
      0xF0 ,0xF5 ,0xE4 ,0xBE ,0xB9 ,0xA5 ,0x9C ,0x7B, 0xE2 ,0x4F ,0x56 ,0x85 ,0x2C ,0x63 ,0x03 ,0xAA,
      0x3E ,0xB2 ,0x30 ,0xA4 ,0xE1 ,0x49 ,0x62 ,0x76, 0xB8 ,0x94 ,0x40 ,0xB5 ,0xB3 ,0xBA ,0x12 ,0xC4,
      0x8B ,0xF5 ,0x2A ,0x8F ,0xC6 ,0x07 ,0x44 ,0x32, 0xA5 ,0x7C ,0xFE ,0x91 ,0x5E ,0x53 ,0x24 ,0x45,
      0xD1 ,0xC2 ,0xE4 ,0xB1 ,0x2D ,0x9B ,0x16, 0xF2 ,0xF8 ,0x65 ,0xFB ,0x8B ,0xAE ,0xBB ,0xD2 ,0xD5,
      0x8E ,0xA2 ,0xF4 ,0xC6 ,0x8C ,0x45 ,0x0A, 0x69 ,0x88 ,0x2F ,0x22 ,0x9F ,0xA8 ,0x03 ,0x3A ,0x9C,
      0xC1 ,0xA4 ,0x72 ,0x0A ,0x79 ,0xA3 ,0x7B, 0x3A ,0xAD ,0x40 ,0xE6 ,0x49 ,0xD6 ,0xC0 ,0xFD ,0x47,
      0xF1 ,0x57 ,0xFB ,0x3D ,0xCD ,0x99 ,0x5A, 0x1C ,0x44 ,0x82 ,0x3C ,0x8A ,0x45 ,0x0C ,0x40 ,0x07,
      0x26 ,0xD1 ,0xB1 ,0xA7 ,0xE6 ,0x92 ,0x1D, 0x4D ,0xAF ,0x60 ,0x9D ,0xE7 ,0x86 ,0x82 ,0xAB ,0x93,
      0xEE ,0xAC ,0x39 ,0x0E ,0xA4 ,0x7A ,0xBD, 0x8E ,0xD3 ,0xCD ,0x05 ,0x6A ,0xAE ,0x46 ,0x6D ,0x22,
      0x5A ,0x04 ,0xBC ,0xC2 ,0x6D ,0xC2 ,0xB7, 0x25 ,0x1A ,0x3E ,0xF7 ,0xA0 ,0x56 ,0xFB ,0x37 ,0x72,
      0x00 ,0x7B ,0xE7 ,0x92 ,0x2B ,0x60 ,0x0C, 0xDA ,0x6F ,0xAB ,0x77 ,0x99 ,0x9B ,0xCC ,0x3D ,0xC4,
      0xF7 ,0x36 ,0xEC ,0xD5 ,0x49 ,0xCC ,0x42, 0xFB ,0x45 ,0x92 ,0x0F ,0xEC ,0x20 ,0x65 ,0x38 ,0xDD,
      0xDE ,0x7F ,0x1A ,0x92 ,0x05 ,0x61 ,0x7F, 0x59 ,0x90 ,0xF5 ,0xCB ,0xB1 ,0x08 ,0xF7 ,0x67 ,0x07,
      0xA0 ,0xDB ,0x48 ,0xD5 ,0x8B ,0xF6 ,0x53, 0x49 ,0xCA ,0x58 ,0x3E ,0x85 ,0xFD ,0x38 ,0x8A ,0x0F,
      0x2D ,0xBB ,0x5A ,0xEA ,0x63 ,0x14 ,0x89, 0xA5 ,0xF0 ,0xC4 ,0x7B ,0x8B ,0x2D ,0x60 ,0xE7 ,0x46,
      0xBB ,0x61 ,0x9B ,0x44 ,0x17 ,0x4F ,0xC2, 0xC9 ,0x84 ,0xC8 ,0x1D ,0x66 ,0x48 ,0x2E ,0x46 ,0x81,
      0x03 ,0x93 ,0xDD ,0xD8 ,0xB4 ,0xC0 ,0x22, 0x96 ,0x8A ,0x73 ,0x40 ,0x84 ,0xE2 ,0xF4 ,0x19 ,0x73,
      0x42 ,0x99 ,0x76 ,0x1E ,0xCB ,0x05 ,0x54, 0xE7 ,0x5B ,0x86 ,0xC6 ,0x99 ,0x41 ,0xC4 ,0xEC ,0x47,
      0x3A ,0x41 ,0x3D ,0x15 ,0x70 ,0x3E ,0x83, 0x28 ,0x9A ,0x14 ,0x2A ,0xC6 ,0x95 ,0x0A ,0x5B ,0x81,
      0x2E ,0xDC ,0x91 ,0x3C ,0x3E ,0x12 ,0x63, 0x3C ,0xCC ,0x93 ,0x21 ,0xCA ,0xAA ,0x9F ,0x4A ,0x12,
      0xE9 ,0x3F ,0x53 ,0x9A ,0x52 ,0xA8 ,0x26, 0x20 ,0x13 ,0x31 ,0xE3 ,0xEC ,0xD3 ,0xE1 ,0x23 ,0x6E,
      0xB8 ,0xC4 ,0xE8 ,0xB8 ,0x4C ,0xAF ,0x88, 0x55 ,0x14 ,0x9D ,0x2E ,0xF5 ,0xE4 ,0xB0 ,0xD4 ,0x8E,
      0x6C ,0x47 ,0x39 ,0xA2 ,0x52 ,0x57 ,0xC5, 0xE1 ,0xF8 ,0x0F ,0x44 ,0x32 ,0x36 ,0x72 ,0xCD ,0xF1
               };

//--METHOD IMPLEMENTATION-------------------------------------------------
//
/*! Default constructor
*/
//------------------------------------------------------------------------
UDPClient::UDPClient()
:   m_pSocket(NULL)
	,m_hSocketThread(NULL)
	,m_hReceiveThread(NULL)
	,m_bReceiveThread(FALSE)
	,m_pFcnRecvData(NULL)
	,m_pFcnSocketClosed(NULL)
	,m_pLogStream(&std::cout)
	,m_bProcessReceiveThread(FALSE)
	,m_hProcessReceiveThread(NULL)
{
   InitializeCriticalSection(&crLost);

   m_pSendTmpBuffer = new char[PACKET_MAX_SIZE];
}

//--METHOD IMPLEMENTATION-------------------------------------------------
//
/*! Destructor
*/
//------------------------------------------------------------------------
UDPClient::~UDPClient()
{
    Disconnect();

    if(m_pSendTmpBuffer)
       delete []m_pSendTmpBuffer;
    m_pSendTmpBuffer = NULL;

  	 DeleteCriticalSection(&crLost);
}

//--METHOD IMPLEMENTATION-------------------------------------------------
//
/*! Sets the receive callback and parameters

\param pCallbackRecvData Callback
\param pCallbackParam Callback parameter (can be NULL)
\param ReceiveBufferSize Receive buffer size (see notes for recommended size)
when the packet size is (the size of size have to be uint32_t).

\return An error code.

notes: Recommended receive buffer size is 2 times the expected data size.

\exception
*/
//------------------------------------------------------------------------
int UDPClient::SetReceiveParameters(pfNMSocketRecvData pCallbackRecvData,
									void* pCallbackParam,
									size_t ReceiveBufferSize)
{
	m_pFcnRecvData  = pCallbackRecvData;
	m_pFcnRecvParam = pCallbackParam;
	m_ReceiveBufferSize = ReceiveBufferSize;

	return eNO_ERROR;
}

int UDPClient::SetShutdownParameters(pfNMSocketClosed pCallbackSocketClosed)
{
    m_pFcnSocketClosed = pCallbackSocketClosed;

    return eNO_ERROR;
}

//--METHOD IMPLEMENTATION-------------------------------------------------
//
/*! Connect the client to a server on the given address and given port.

\param Address Server address.
\param Port Server port.

\return An error code.
*/
//------------------------------------------------------------------------
int UDPClient::Connect(LPTSTR Address, int Port,bool bBroadcast)
{
   int Ret;
   WSADATA wsaData = { 0 };

   // Initialize winsock
   if ((Ret = WSAStartup(MAKEWORD(2,0), &wsaData)) != 0)
   {
      //*m_pLogStream << "Winsock error: WSAStartup() failed " << WSAGetLastError() << std::endl;
      return eERROR_INITIALIZATION;
   }

   // Create the socket
   SOCKET hSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
   //SOCKET hSocket = socket(AF_INET, SOCK_DGRAM,0);
   if (hSocket == INVALID_SOCKET)
   {
      //*m_pLogStream << "Winsock error: WSASocket() failed " << WSAGetLastError() << std::endl;
      WSACleanup();
      return eERROR_INITIALIZATION;
   }

   memset(&m_sockAddrLocal  , 0, sizeof(m_sockAddrLocal));
   memset(&m_sockAddrRecv   , 0, sizeof(m_sockAddrRecv));
   memset(&m_sockAddrSender , 0, sizeof(m_sockAddrSender));


   //Set the receive SocketIN
   m_sockAddrRecv.sin_family = AF_INET;
   hostent *HostInfo;    
   if (!isdigit(Address[0])) 
   {   
      // Server Name is Alphanumeric
      HostInfo = gethostbyname(Address);
      if (!HostInfo) 
      {
         //*m_pLogStream << "Winsock error: gethostbyname() failed " << WSAGetLastError() << std::endl;
         return eERROR_INITIALIZATION;
      }
      memcpy(&(m_sockAddrRecv.sin_addr), HostInfo->h_addr, HostInfo->h_length);
   } 
   else  
   {  
      m_sockAddrRecv.sin_addr.s_addr = inet_addr(Address); // Server Name is a nnn.nnn.nnn.nnn
   }
   m_sockAddrRecv.sin_port = htons(Port);

   m_sockAddrLocal.sin_family = AF_INET;
#if defined(LINUX_PORT) && !defined(_WIN32)
   m_sockAddrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
#else
   m_sockAddrLocal.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
#endif
   m_sockAddrLocal.sin_port = htons(0);


   int iRet = 0;
   if(bBroadcast)
      iRet = bind(hSocket, (struct sockaddr *)&m_sockAddrLocal, sizeof(struct sockaddr_in));
   else
      iRet = connect(hSocket, (struct sockaddr *)&m_sockAddrRecv, sizeof(struct sockaddr_in));

   if (iRet == 0)
   {
#if defined(LINUX_PORT) && !defined(_WIN32)
      socklen_t nLen = sizeof(sockaddr_in);
#else
      int nLen = sizeof( sockaddr_in );
#endif
      getsockname( hSocket, (sockaddr *)&m_sockAddrLocal, &nLen );


#if defined(LINUX_PORT) && !defined(_WIN32)
      std::fprintf(stderr, "[COMM] UDP connect local port %u -> %s:%d\n",
                   static_cast<unsigned>(ntohs(m_sockAddrLocal.sin_port)),
                   Address, Port);
#endif

      //Set socket options...
      int bufsize = (1024 * 1024)*4; //4 megs buffer
      int iErr;
      iErr = setsockopt(hSocket,SOL_SOCKET, SO_RCVBUF,(char *)&bufsize, sizeof bufsize);
      iErr = setsockopt(hSocket,SOL_SOCKET, SO_SNDBUF,(char *)&bufsize, sizeof bufsize);
      //BOOL bBroadcast = TRUE;
      //iErr = setsockopt(hSocket,SOL_SOCKET, SO_BROADCAST,(char *)&bBroadcast, sizeof bBroadcast);

      m_pSocket = new NMSocket;
      m_pSocket->Init(hSocket);
      m_hReceiveThread   = (HANDLE)_beginthread( fReceiveData  , 0, this );
   }
   else
   {
      //*m_pLogStream << "Winsock error: bind() failed " << WSAGetLastError() << std::endl;
      WSACleanup();
      return eERROR_INITIALIZATION;
   }

   return eNO_ERROR;
}

int UDPClient::StartServer(int Port, bool bBuffered)
{
    int Ret;
    WSADATA wsaData = { 0 };

    // Initialize winsock
    if ((Ret = WSAStartup(MAKEWORD(2,0), &wsaData)) != 0)
    {
        //*m_pLogStream << "Winsock error: StartServer() failed " << WSAGetLastError() << std::endl;
        return eERROR_INITIALIZATION;
    }

    // Create the socket
    SOCKET hSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
    //SOCKET hSocket = socket(AF_INET, SOCK_DGRAM,0);
    if (hSocket == INVALID_SOCKET)
    {
        //*m_pLogStream << "Winsock error: StartServer() failed " << WSAGetLastError() << std::endl;
        WSACleanup();
        return eERROR_INITIALIZATION;
    }

    memset(&m_sockAddrLocal  , 0, sizeof(m_sockAddrLocal));
    memset(&m_sockAddrRecv   , 0, sizeof(m_sockAddrRecv));
    memset(&m_sockAddrSender , 0, sizeof(m_sockAddrSender));
    

    m_sockAddrLocal.sin_family = AF_INET;
    m_sockAddrLocal.sin_port = htons(Port);
 #if defined(LINUX_PORT) && !defined(_WIN32)
   m_sockAddrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
#else
   m_sockAddrLocal.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
#endif


    if (bind(hSocket, (SOCKADDR *) & m_sockAddrLocal, sizeof (m_sockAddrLocal)) == 0)
    {
       //Set socket options...
       int bufsize = (1024 * 1024)*4; //4 megs buffer
       int iErr;
       iErr = setsockopt(hSocket,SOL_SOCKET, SO_RCVBUF,(char *)&bufsize, sizeof bufsize);
       iErr = setsockopt(hSocket,SOL_SOCKET, SO_SNDBUF,(char *)&bufsize, sizeof bufsize);
       //BOOL bBroadcast = TRUE;
       //iErr = setsockopt(hSocket,SOL_SOCKET, SO_BROADCAST,(char *)&bBroadcast, sizeof bBroadcast);

        m_pSocket = new NMSocket;
        m_pSocket->Init(hSocket);

        m_hReceiveThread   = (HANDLE)_beginthread( fReceiveData, 0, this );
    }
    else
    {
        closesocket(hSocket);
        //*m_pLogStream << "Winsock error: StartServer() failed " << WSAGetLastError() << std::endl;
        WSACleanup();
        return eERROR_INITIALIZATION;
    }
    
    return eNO_ERROR;
}


//--METHOD IMPLEMENTATION-------------------------------------------------
//
/*! Disconnect the client from the server.

\param Address Server address.
\param Port Server port.

\return An error code.
*/
//------------------------------------------------------------------------
void UDPClient::Disconnect()
{
   //Close receive thread
   if(m_hProcessReceiveThread)
   {
      m_bProcessReceiveThread  = FALSE;
      if(::WaitForSingleObject(m_hProcessReceiveThread, 2000) == WAIT_TIMEOUT ) 
         TerminateThread( m_hProcessReceiveThread, 0x666 );
      m_hProcessReceiveThread = NULL;
   }

   if (m_pSocket)
   {
      // Close the socket
      closesocket(m_pSocket->m_hSocket);
   }

   //Close receive thread
   if(m_hReceiveThread)
   {
      m_bReceiveThread  = FALSE;
      if(::WaitForSingleObject(m_hReceiveThread, 2000) == WAIT_TIMEOUT ) 
         TerminateThread( m_hReceiveThread, 0x666 );
      m_hReceiveThread = NULL;
   }

   if (m_pSocket)
   {
      ::WaitForSingleObject(m_hSocketThread, INFINITE);
      m_hSocketThread = NULL;
      delete m_pSocket;
      m_pSocket = NULL;
   }

   
   
}


int UDPClient::SendTo(sockaddr_in sockTo,char* pBuff, size_t BuffLenght)
{
   DWORD BytesSent = 0;
   DWORD Flags = 0;
   WSABUF DataBuf;
   DataBuf.len = static_cast<u_long>(BuffLenght);
   
/*
//    for (UINT c=0; c<BuffLenght; c++)
//        m_pSendTmpBuffer[c] = pBuff[c]^COMM_key[c%512];
// 
//    if( sendto(m_pSocket->m_hSocket, m_pSendTmpBuffer, BuffLenght, 0, (sockaddr *)&sockTo, sizeof( sockaddr_in ) ) == SOCKET_ERROR)
//    {
//       // Handle connection lost errors.
//       switch( WSAGetLastError() )
//       {
//          case WSAECONNABORTED:
//          case WSAEHOSTUNREACH:
//          case WSAECONNRESET:
//          case WSAEADDRNOTAVAIL:
//          case WSAEAFNOSUPPORT:
//          case WSAETIMEDOUT:
//          case WSAENETUNREACH:
//             // Add the connection lost to the list of lost connections
//              EnterCriticalSection(&crLost);
//              vConnectionLost.push_back( sockTo );
//          	 LeaveCriticalSection(&crLost);
//          break;
//       };
//    }
*/



   DataBuf.buf = pBuff;
   int Ret = WSASendTo( m_pSocket->m_hSocket, &DataBuf, 1, &BytesSent, 0,(sockaddr *)&sockTo, sizeof( sockaddr_in ),&m_pSocket->m_OverlappedSend, NULL);

   if (Ret == SOCKET_ERROR) 
   {
      Ret = WSAGetLastError();
      if (Ret != WSA_IO_PENDING) 
      {
         EnterCriticalSection(&crLost);
         vConnectionLost.push_back( sockTo );
         LeaveCriticalSection(&crLost);
         return eERROR_COULD_NOT_SEND;
      }
   }


   return eNO_ERROR;
}


void UDPClient::GetConnectSockIn(sockaddr_in *pSocketIn)
{
   memcpy(pSocketIn , &m_sockAddrRecv, sizeof(sockaddr_in));
}



int UDPClient::SocketClosedCallback(void* pThis, SOCKET pSocket, void* pParam)
{
    UDPClient* pUDPClient = reinterpret_cast<UDPClient*>(pThis);

    if (pUDPClient->m_pFcnSocketClosed)
    {
        (pUDPClient->m_pFcnSocketClosed)(pParam);
    }

    return eNO_ERROR;
}


void UDPClient::fReceiveData(LPVOID pParameter)
{
    UDPClient* me = reinterpret_cast<UDPClient*>(pParameter);
    NMSocket* pSocket = me->m_pSocket;

    me->m_bReceiveThread = TRUE;

    DWORD Flags;
    WSABUF WsaBuffer;

    DWORD RecvBytes;
    int nLen = sizeof( sockaddr_in );
    BYTE* pData = new BYTE[PACKET_MAX_SIZE];

    while (me->m_bReceiveThread)
    {
/*
//        RecvBytes = recvfrom(pSocket->m_hSocket, (char *)pData, PACKET_MAX_SIZE, 0, (sockaddr *)&me->m_sockAddrSender, &nLen);
//        int Ret = WSAGetLastError();
//        // If a socket error occured.
//        if( RecvBytes >0 &&  me->m_bReceiveThread && Ret == 0)
//        {
//           if (me->m_pFcnRecvData)
//           {
//              //for (UINT c=0; c<RecvBytes; c++)
//              //   pData[c] ^= COMM_key[c%512];
//              (*me->m_pFcnRecvData)(me->m_sockAddrSender,reinterpret_cast<char*>(&pData[0]), RecvBytes,0,0,me->m_pFcnRecvParam);
//           }
//        }
*/


        RecvBytes = 0;	
        Flags = 0;
        size_t ReservedSize = PACKET_MAX_SIZE;
        WsaBuffer.buf = reinterpret_cast<char*>(pData);
        WsaBuffer.len = static_cast<u_long>(ReservedSize);

        int Ret = WSARecvFrom(pSocket->m_hSocket, &WsaBuffer, 1, &RecvBytes, &Flags,(sockaddr *)&me->m_sockAddrSender, &nLen,&pSocket->m_OverlappedRecv, NULL);
        if (Ret == SOCKET_ERROR)
        {
            Ret = WSAGetLastError();
            if (Ret == WSA_IO_PENDING)
            {
                // ...then wait for the IO to complete.
                Ret = WSAWaitForMultipleEvents(1, &pSocket->m_OverlappedRecv.hEvent, TRUE, WSA_INFINITE, TRUE);
                WSAResetEvent(pSocket->m_OverlappedRecv.hEvent);
                if (Ret != WSA_WAIT_EVENT_0)
                {
                    //me->HandleError(WSAGetLastError());
                }

                // Get the result now that the event was signaled.
                Ret = WSAGetOverlappedResult(pSocket->m_hSocket, &pSocket->m_OverlappedRecv, &RecvBytes, FALSE, &Flags);
                if (Ret == FALSE)
                {
                    //me->HandleError(WSAGetLastError());
                }
                
                if (me->m_pFcnRecvData && RecvBytes >0 && Ret)
                {
/*
//                    for (UINT c=0; c<RecvBytes; c++)
//                       pData[c] ^= COMM_key[c%512];
*/

                   (*me->m_pFcnRecvData)(me->m_sockAddrSender,reinterpret_cast<unsigned char*>(&pData[0]), RecvBytes,0,0,me->m_pFcnRecvParam);
                }
            }
            else
            {
                //me->HandleError(Ret);
            }
        }
        else
        {
            if (me->m_pFcnRecvData  && RecvBytes >0)
            {
/*
//                for (UINT c=0; c<RecvBytes; c++)
//                   pData[c] ^= COMM_key[c%512];
*/
               (*me->m_pFcnRecvData)(me->m_sockAddrSender,reinterpret_cast<unsigned char*>(&pData[0]), RecvBytes,0,0,me->m_pFcnRecvParam);
            }
        }

        Sleep(0);
    }

    // Disconnect callback
    if (me->m_pFcnSocketClosed)
    {
        (*me->m_pFcnSocketClosed)(me);
    }

    if(pData)
        delete []pData;
    pData = NULL;
}


vector< sockaddr_in > *UDPClient::GetLostConnections( void )
/******************************************************************************/
{
	EnterCriticalSection(&crLost);
	return &vConnectionLost;
}
/******************************************************************************/
// Frees the lost connection vector fetched with GetLostConnection.
void UDPClient::FreeLostConnections(BOOL boFlushList)
/******************************************************************************/
{
	if( boFlushList )
	{
		// Flush the vector.
		vConnectionLost.erase( vConnectionLost.begin(), vConnectionLost.end() );
	}
	LeaveCriticalSection(&crLost);
}
