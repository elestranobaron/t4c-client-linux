//--FILE DEFINITION--------------------------------------------------------
//
/*! \file
   File name:  NMSocket.cpp
*/
//-------------------------------------------------------------------------


#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <vector>
#if !defined(LINUX_PORT) || defined(_WIN32)
#include <conio.h>
#endif
#include "Socket.h"

//--METHOD IMPLEMENTATION-------------------------------------------------
//
/*! Default constructor
*/
//------------------------------------------------------------------------
NMSocket::NMSocket()
:	m_hSocket(NULL)
{

}

//--METHOD IMPLEMENTATION-------------------------------------------------
//
/*! Destructor
*/
//------------------------------------------------------------------------
NMSocket::~NMSocket()
{
	DeInit();
}

int NMSocket::Init(const SOCKET& rSocketHandle)
{
	WSAEVENT hEventSend = WSACreateEvent();
	WSAEVENT hEventRecv = WSACreateEvent();
	if (hEventSend == NULL ||
		hEventRecv == NULL)
	{
		WSACloseEvent(hEventSend);
		WSACloseEvent(hEventRecv);
		return eERROR_INITIALIZATION;
	}


 	memset(&m_OverlappedRecv, 0, sizeof(m_OverlappedRecv));
 	m_OverlappedRecv.hEvent = hEventRecv;
 	memset(&m_OverlappedSend, 0, sizeof(m_OverlappedSend));
 	m_OverlappedSend.hEvent = hEventSend;


	m_hSocket = rSocketHandle;

	return eNO_ERROR;
}

int NMSocket::DeInit()
{

 	WSACloseEvent(m_OverlappedRecv.hEvent);
 	WSACloseEvent(m_OverlappedSend.hEvent);


	return eNO_ERROR;
}
