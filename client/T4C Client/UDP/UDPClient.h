#pragma once

#include "Socket.h"
#include <map>
#include <vector>

using namespace std;

#define PACKET_MAX_SIZE                    1024//2048
#define NBR_MAX_QUEUE_PACKET_BUFFER_IN    16000

typedef struct _sPacketData
{
   unsigned char *pData;
   DWORD          dwDataLength;
   void          *pParent;
}sPacketData;



class UDPClient
{
public:
   enum eError
   {
      eNO_ERROR = 0,
      eERROR_INITIALIZATION,
      eERROR_INVALID_PARAMETER,
      eERROR_COULD_NOT_SEND,
   };

   UDPClient();
   ~UDPClient();

   int SetReceiveParameters(pfNMSocketRecvData pCallbackRecvData,
      void* pCallbackParam,
      size_t ReceiveBufferSize);

   int SetShutdownParameters(pfNMSocketClosed pCallbackSocketClosed);

   int StartServer(int Port, bool bBuffered);
   int Connect(LPTSTR Address, int Port,bool bBroadcast = false);
   void Disconnect();

   int SendTo(sockaddr_in sockTo,char* pBuff, size_t BuffLenght);

#if defined(LINUX_PORT) && !defined(_WIN32)
   unsigned long GetLocalIP() { return htonl(T4C_IN_ADDR_S_ADDR(m_sockAddrLocal)); }
#else
   unsigned long GetLocalIP(){return htonl(m_sockAddrLocal.sin_addr.S_un.S_addr);}
#endif

   void GetConnectSockIn(sockaddr_in *pSocketIn);

   vector< sockaddr_in > *GetLostConnections( void );
   void FreeLostConnections(BOOL boFlushList);

private:
   UDPClient(const UDPClient& rOther);
   UDPClient& operator=(const UDPClient& rOther);

   static void fReceiveData(LPVOID pParameter);
   

protected:
   // Receive callback and parameters
   pfNMSocketRecvData m_pFcnRecvData;

   void* m_pFcnRecvParam;
   size_t m_ReceiveBufferSize;
   char* m_pSendTmpBuffer;

   // Socket closed callback
   inline static int SocketClosedCallback(void* pThis, SOCKET pSocket, void* pParam);
   pfNMSocketClosed m_pFcnSocketClosed;

   BOOL m_bReceiveThread;
   NMSocket* m_pSocket;
   sockaddr_in  m_sockAddrLocal;
   sockaddr_in  m_sockAddrRecv;
   sockaddr_in  m_sockAddrSender;

   HANDLE m_hSocketThread;
   HANDLE m_hReceiveThread;
   
   BOOL   m_bProcessReceiveThread;
   HANDLE m_hProcessReceiveThread;

   std::ostream* m_pLogStream;


   CRITICAL_SECTION crLost;
   vector< sockaddr_in > vConnectionLost;

   
};


