// NMPlayVideo.h: interface for the CNMPlayVideo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NMPLAYVIDEO_H__36E8EC72_AE07_4C34_993E_F4A766986012__INCLUDED_)
#define AFX_NMPLAYVIDEO_H__36E8EC72_AE07_4C34_993E_F4A766986012__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dshow.h>
#include <tchar.h>

#define VOLUME_FULL     0L
#define VOLUME_SILENCE  -10000L

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
#define NM_VALID(x) if (FAILED(hr=(x))) \
{return hr;}

#define Stopped 0
#define Paused  1
#define Running 2

void Msg(char *szFormat, ...);

class CNMPlayVideo  
{
public:
	CNMPlayVideo(HWND hWnd,HWND hWndParent);
	virtual ~CNMPlayVideo();


//////////////////////////////
//  FONCTION
//////////////////////////////
public:
   void CmdPlay();
   void CmdStop();
   void CmdPause();
   void CmdPausePause();
   void SetAviRefTime(REFERENCE_TIME RefTime);
   void SetVolumePourcent(int dwVal,bool bApply);
   bool GetDVTimecode(REFERENCE_TIME *dwRefTime,int *dwHour, int *dwMin, int *dwSec,int *dwTotSec);
   bool GetDVTimecodeTotal(REFERENCE_TIME *dwRefTime,int *dwHour, int *dwMin, int *dwSec, int *dwTotSec);
   REFERENCE_TIME GetDVTimecodeS();
   REFERENCE_TIME GetDVTimecodeTotalS();
   void SetToStartPos();
   int  GetState(){return m_CurrentStatus;}
   void CloseClip(void);  

   HRESULT PlayMovieInWindow(LPTSTR szFile,int dwSize);

protected:
   void GetFilename(TCHAR *pszFull, TCHAR *pszFile, int iLen);
  
//////////////////////////////
//  VARIABLE
//////////////////////////////
public:
   long  m_lWidth;
   long  m_lHeight;

   REFERENCE_TIME m_rtTotalTime;

protected:
   int   m_CurrentStatus;
   long  m_lVolumePC;
   HWND  m_hPlayWnd;
   HWND  m_hParentWnd;

   IFilterGraph  *m_pGraphFilter;
   IGraphBuilder *m_pGraphBuilder;
   IBasicAudio   *m_pBasicAudio;
   IMediaControl *m_pMediaControl;
   IMediaSeeking *m_pMediaSeeking;

};

#endif // !defined(AFX_NMPLAYVIDEO_H__36E8EC72_AE07_4C34_993E_F4A766986012__INCLUDED_)
