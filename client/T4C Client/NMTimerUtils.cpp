#include "NMTimerUtils.h"
#include <process.h>

CNMTimerUtils::CNMTimerUtils(void)
{
   for(int i=0;i<NBR_MAX_TIMERUTILS;i++)
   {
      m_TimerInfo[i].bEnable      = false;
      m_TimerInfo[i].iCnt         = 0;
      m_TimerInfo[i].iCntMax      = 0;
      m_TimerInfo[i].pCallbackFct = NULL;
      m_TimerInfo[i].pParent      = NULL;
   }


   ::InitializeCriticalSection(&m_LockTimer);

   m_hTimeTimerExit   = ::CreateEvent(NULL,TRUE , FALSE, NULL);
   m_hTimeTimerThread = (HANDLE)_beginthread( TimerThreadFct, 0, this);
}

CNMTimerUtils::~CNMTimerUtils(void)
{
   if(m_hTimeTimerExit && m_hTimeTimerThread)
   {
      ::SetEvent(m_hTimeTimerExit);
      if( ::WaitForSingleObject( m_hTimeTimerThread, 10000 ) == WAIT_TIMEOUT ) 
         ::TerminateThread( m_hTimeTimerThread, 0x69 );
      ::CloseHandle(m_hTimeTimerExit);
      m_hTimeTimerExit   = NULL;
      m_hTimeTimerThread = NULL;
   }

   DeleteCriticalSection(&m_LockTimer);
}

void CNMTimerUtils::TimerThreadFct(LPVOID lpParam)
{

   CNMTimerUtils* me = (CNMTimerUtils*)lpParam;
   bool bRun = true;
   while(bRun)
   {
      DWORD dwRet = WaitForSingleObject(me->m_hTimeTimerExit,10);
      switch(dwRet)
      {
         case WAIT_OBJECT_0+0 :
            bRun = false;
         break;
         default:
            //manage Timers...
            EnterCriticalSection(&me->m_LockTimer);

            for(int i=0;i<NBR_MAX_TIMERUTILS;i++)
            {
               if(me->m_TimerInfo[i].bEnable)
               {
                  me->m_TimerInfo[i].iCnt++;
                  if(me->m_TimerInfo[i].iCnt >= me->m_TimerInfo[i].iCntMax)
                  {
                     //dois caller la fonction...
                     if(me->m_TimerInfo[i].pCallbackFct)
                        me->m_TimerInfo[i].pCallbackFct(me->m_TimerInfo[i].pParent,i+1);
                     me->m_TimerInfo[i].iCnt = 0;
                  }
               }
            }

            LeaveCriticalSection(&me->m_LockTimer);
         break;
      }
   }
   return ; 
}

int CNMTimerUtils::TimerCallback(void* pThis,unsigned int dwCode)
{
   CNMTimerUtils *me = (CNMTimerUtils*)pThis;

   //do nothing supposed to be overwriten by used class

   return 0;
}

void CNMTimerUtils::SetNMTimer(int iID,int iTime,NM_TIMERUTILS_CallbackFct pCallback,void *pParent)
{
   if(iID <1 || iID >NBR_MAX_TIMERUTILS)
      return;

   EnterCriticalSection(&m_LockTimer);
   int irID = iID-1;
   m_TimerInfo[irID].iCnt           = 0;
   m_TimerInfo[irID].iCntMax        = iTime/10;
   m_TimerInfo[irID].bEnable        = true;
   m_TimerInfo[irID].pCallbackFct   = pCallback;
   m_TimerInfo[irID].pParent        = pParent;
   LeaveCriticalSection(&m_LockTimer);
}

void CNMTimerUtils::KillNMTimer(int iID)
{
   if(iID == -1) //Kill ALL
   {
      for(int i=0;i<NBR_MAX_TIMERUTILS;i++)
      {
         m_TimerInfo[i].bEnable      = false;
         m_TimerInfo[i].iCnt         = 0;
         m_TimerInfo[i].iCntMax      = 0;
         m_TimerInfo[i].pCallbackFct = NULL;
         m_TimerInfo[i].pParent      = NULL;
      }
      return;
   }


   if(iID <1 || iID >NBR_MAX_TIMERUTILS)
      return;

   EnterCriticalSection(&m_LockTimer);
   int irID = iID-1;
   m_TimerInfo[irID].bEnable        = false;
   m_TimerInfo[irID].iCnt           = 0;
   m_TimerInfo[irID].iCntMax        = 0;
   m_TimerInfo[irID].pCallbackFct   = NULL;
   m_TimerInfo[irID].pParent        = NULL;
   LeaveCriticalSection(&m_LockTimer);
}