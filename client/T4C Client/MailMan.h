// MailMan.h: interface for the CMailMan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAILMAN_H__99148C57_15FD_4D06_9D9E_5A66AD50E95A__INCLUDED_)
#define AFX_MAILMAN_H__99148C57_15FD_4D06_9D9E_5A66AD50E95A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <mapi.h>
enum status { OK , CANT_LOGON , CANT_LOAD};

enum readstatus { GOTMAIL,NOMAIL ,FAILMAIL};



class CMailMan  
{
public:
	CMailMan();
	virtual ~CMailMan();

	int   LogonMail() ;
	void  LogoffMail(); 
	bool  SendMail(char *name, char * address,char * Subject,char * Text) ;

protected:
	HINSTANCE GetHandle();
	HINSTANCE		hMAPILib;
public:
	
	LHANDLE				lhSession;
	LPMAPILOGON			lpfnMAPILogon;
	LPMAPIFINDNEXT		lpfnMAPIFindNext;
	LPMAPIREADMAIL		lpfnMAPIReadMail;
	LPMAPILOGOFF		lpfnMAPILogoff;
	LPMAPIDETAILS		lpfnMAPIDetails;
	LPMAPIFREEBUFFER	lpfnMAPIFreeBuffer;
	LPMAPISENDMAIL		lpfnMAPISendMail;

};

#endif // !defined(AFX_MAILMAN_H__99148C57_15FD_4D06_9D9E_5A66AD50E95A__INCLUDED_)
