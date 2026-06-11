// MailMan.cpp: implementation of the CMailMan class.
//
//////////////////////////////////////////////////////////////////////
// This generic class uses MAPI to read and send emails via your application.
// This work is a clean class inspired by others that did a bit hera dn did a 
// bit there but never took it this far for its easy usage.No dll
// no nasty extra code.Just a simple class to use.
// As always I say:Keep It Simple Stupid
//////////////////////////////////////////////////////////////////////
#include "MailMan.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMailMan::CMailMan()
{

	hMAPILib = LoadLibrary("MAPI32.DLL");     
	lpfnMAPILogon=(LPMAPILOGON)GetProcAddress(hMAPILib, "MAPILogon");
	lpfnMAPIFindNext=(LPMAPIFINDNEXT)GetProcAddress(hMAPILib, "MAPIFindNext");
	lpfnMAPIReadMail=(LPMAPIREADMAIL)GetProcAddress(hMAPILib, "MAPIReadMail");
	lpfnMAPILogoff=(LPMAPILOGOFF)GetProcAddress(hMAPILib, "MAPILogoff");
	lpfnMAPIDetails=(LPMAPIDETAILS)GetProcAddress(hMAPILib, "MAPIDetails");
	lpfnMAPIFreeBuffer=(LPMAPIFREEBUFFER)GetProcAddress(hMAPILib, "MAPIFreeBuffer");
	lpfnMAPISendMail=(LPMAPISENDMAIL)GetProcAddress(hMAPILib, "MAPISendMail");

}

CMailMan::~CMailMan()
{
	FreeLibrary(hMAPILib);

}

int CMailMan::LogonMail() 
{
	if (!hMAPILib) 
		return CANT_LOAD; 
	else
	if(lpfnMAPILogon(0L,"","",MAPI_LOGON_UI ,0L, &lhSession) != SUCCESS_SUCCESS) 
		return CANT_LOGON;
	else
		return OK;
}
HINSTANCE CMailMan::GetHandle()
{
	return hMAPILib;
}


void CMailMan::LogoffMail() 
{
	lpfnMAPILogoff(lhSession,0L,0L,0L); //log off from MAPI
}
//=============================MAPI===============================================================
bool CMailMan::SendMail(char *name, char * address,char * Subject,char * Text) 
{
//address can be made of several addresses all delimited by a ;
	int i=0;


	HRESULT hr = S_OK; 
	MapiMessage Message; 
	MapiRecipDesc recip; 
	Message.ulReserved = 0; 

	Message.lpszSubject = (LPTSTR)((LPCTSTR)Subject); //NULL; //limit subject to less than 256 characters... 
	Message.lpszNoteText =(LPTSTR)((LPCTSTR) Text) ; 
	Message.lpszMessageType = NULL;		//for non-IPM messages, leave NULL 
	Message.lpszDateReceived = NULL;	//date format is YYYY/MM/DD HH:MM (24 hour clock) 
	Message.lpszConversationID = NULL;	//ignore this... 
	Message.flFlags = 0;				//could set MAPI_RECEIPT_REQUESTED for read report 
	Message.lpOriginator = NULL;		//sender info (MapiRecipDesc structure) 
	Message.nRecipCount = 1;			//number of recipients in array below 
	Message.lpRecips = &recip;			//array of MapiRecipDesc structures 
	Message.nFileCount = 0;				//number of attached files 
	Message.lpFiles = NULL;				//array of MapiFileDesc structures 
	Message.flFlags=0;

	memset(&recip, 0, sizeof(MapiRecipDesc)); //zero it all to defaults 
	recip.ulRecipClass = MAPI_TO; //regular recipient 
	recip.lpszName = (LPTSTR)((LPCTSTR)name); 
   char email[200];
   sprintf_s(email,200,"SMTP:%s",address);
	recip.lpszAddress = (LPTSTR)((LPCTSTR)email); 
	if (NULL == hMAPILib) 
	{ 
		return false; 
	} 
	lpfnMAPISendMail(0, 0, &Message, MAPI_LOGON_UI  , 0);
	return true; 
}


