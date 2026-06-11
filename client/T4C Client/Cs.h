#ifndef __CS_H
#define __CS_H


class CriticalSection {
	private:
		static double    OldPriority;
		static char      OldName[100];
		static char	     OldID[100];
		char             Name[100];
		double           Priority;
		double           SavePriority;
		char             SaveName[100];
		char			 SaveID[100];
		CRITICAL_SECTION csLock;
	
	public:
		CriticalSection(void) {
			strcpy_s(Name,100, "Not Identified");
			Priority = 0.0;
			InitializeCriticalSection(&csLock);
		}

		CriticalSection(char *N, double P) {
			strcpy_s(Name,100, N);
			Priority = P;
			InitializeCriticalSection(&csLock);
		}

		void SetCriticalSection(char *N, double P) {
			strcpy_s(Name,100, N);
			Priority = P;
		}

		~CriticalSection(void) {
			DeleteCriticalSection(&csLock);
		}

		void Lock(char *ID) 
      {
			EnterCriticalSection(&csLock);
		}

		void Unlock(char *ID) 
      {
			LeaveCriticalSection(&csLock);
		}
};

#endif