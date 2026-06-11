#pragma once
#include <windows.h>

#define NBR_MAX_TIMERUTILS   20

typedef int (*NM_TIMERUTILS_CallbackFct)(void* pThis,unsigned int dwCode);

class CNMTimerUtils
{
public:
   CNMTimerUtils(void);
   ~CNMTimerUtils(void);

   void SetNMTimer(int iID,int iTime,NM_TIMERUTILS_CallbackFct pCallback,void *pParent);
   void KillNMTimer(int iID);

protected:
 
   HANDLE  m_hTimeTimerExit;
   HANDLE  m_hTimeTimerThread;

   typedef struct _nmTimer
   {
      bool bEnable;
      int iCnt;
      int iCntMax;
      NM_TIMERUTILS_CallbackFct   pCallbackFct;
      void *pParent;
   }nmTimer;

   nmTimer m_TimerInfo[NBR_MAX_TIMERUTILS];

   CRITICAL_SECTION  m_LockTimer;

private:
   static void   TimerThreadFct( LPVOID lpParam );
   static int    TimerCallback(void* pThis,unsigned int nIDEvent);
};
