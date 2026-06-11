// NMPlayVideo.cpp: implementation of the CNMPlayVideo class.
//
//////////////////////////////////////////////////////////////////////

#include <streams.h>
#include "NMPlayVideo.h"
#include <atlbase.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const char * hresultNameLookup(HRESULT hres);

#ifndef CHECK_HR
#define CHECK_HR(expr) if (FAILED(expr)) {\
   OutputDebugString( hresultNameLookup(expr));\
   OutputDebugString( "\r\n");\
   DbgLog((LOG_ERROR, 0, \
   TEXT("FAILED: %s\nat Line:%d of %s"), \
   TEXT(#expr), __LINE__, TEXT(__FILE__) ));__leave; } else
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNMPlayVideo::CNMPlayVideo(HWND hWnd,HWND hWndParent)
{
   m_hPlayWnd   = hWnd;
   m_hParentWnd = hWndParent;
   
   m_lVolumePC      = 100;
   m_CurrentStatus  = Stopped;
   m_rtTotalTime    = 0;
   
   m_pGraphFilter  = NULL;
   m_pBasicAudio   = NULL;
   m_pGraphBuilder = NULL;
   m_pMediaControl = NULL;
   m_pMediaSeeking = NULL;
   CoInitialize(NULL);
}

CNMPlayVideo::~CNMPlayVideo()
{
   CloseClip();
   CoUninitialize();
}



HRESULT CNMPlayVideo::PlayMovieInWindow(LPTSTR szFile,int dwSize)
{
   
   WCHAR wFile[MAX_PATH];
   HRESULT   hr   = S_OK;
   IUnknown *pUnk = NULL;
   DWORD caps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanGetDuration; 

   MultiByteToWideChar(CP_ACP, 0, szFile, -1, wFile, MAX_PATH);
   __try
   {
      CHECK_HR(hr = CoCreateInstance(CLSID_FilterGraph,NULL, CLSCTX_INPROC,IID_IUnknown, (LPVOID *)&pUnk));
      CHECK_HR(hr = pUnk->QueryInterface(IID_IFilterGraph, (LPVOID *)&m_pGraphFilter));
      CHECK_HR(hr = pUnk->QueryInterface(IID_IGraphBuilder, (LPVOID *)&m_pGraphBuilder));
      CHECK_HR(hr = m_pGraphBuilder->RenderFile(wFile, NULL));
      CHECK_HR(hr = pUnk->QueryInterface(IID_IMediaControl, (LPVOID *)&m_pMediaControl));

      //
      // Not being able to get the IMediaEvent interface does
      // necessarly mean that we can't play the graph.
      //
      CHECK_HR(pUnk->QueryInterface(IID_IBasicAudio, (LPVOID *)&m_pBasicAudio));
      CHECK_HR(pUnk->QueryInterface(IID_IMediaSeeking, (LPVOID *)&m_pMediaSeeking));

      if(m_pBasicAudio)
         SetVolumePourcent(m_lVolumePC,true);

      
      if (m_pMediaSeeking && S_OK == m_pMediaSeeking->CheckCapabilities(&caps))		
         m_pMediaSeeking->GetDuration(&m_rtTotalTime);

      if(m_pMediaControl)
      {
         CHECK_HR(m_pMediaControl->Run());
         m_CurrentStatus = Running;
      }
   }
   __finally
   {
      if(FAILED(hr))
      {
         SAFE_RELEASE(m_pMediaSeeking);
         SAFE_RELEASE(m_pBasicAudio);
         SAFE_RELEASE(m_pMediaControl);
         SAFE_RELEASE(m_pGraphBuilder);
         SAFE_RELEASE(m_pGraphFilter);
      }

      SAFE_RELEASE(pUnk);
   }
   return hr;
/*
   
    WCHAR wFile[MAX_PATH];
    HRESULT hr = S_OK;

    MultiByteToWideChar(CP_ACP, 0, szFile, -1, wFile, MAX_PATH);

    // Get the interface for DirectShow's GraphBuilder
    NM_VALID(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGraphBuilder));

    if(SUCCEEDED(hr))
    {
        // Have the graph construct its the appropriate graph automatically
        NM_VALID(m_pGraphBuilder->RenderFile(wFile, NULL));

        // QueryInterface for DirectShow interfaces
        NM_VALID(m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl));
        NM_VALID(m_pGraphBuilder->QueryInterface(IID_IMediaSeeking, (void **)&m_pMediaSeeking));

        // Query for audio interfaces, which may not be relevant for video-only files
        NM_VALID(m_pGraphBuilder->QueryInterface(IID_IBasicAudio, (void **)&m_pBasicAudio));

        if(m_pBasicAudio)
        {
           SetVolumePourcent(m_lVolumePC,true);
        }

        DWORD caps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanGetDuration; 
        if (m_pMediaSeeking && S_OK == m_pMediaSeeking->CheckCapabilities(&caps))		
        {
           m_pMediaSeeking->GetDuration(&m_rtTotalTime);
        }
        // Run the graph to play the media file
        m_pMediaControl->Run();
        m_CurrentStatus = Running;
    }
    return hr;
    */
}




bool CNMPlayVideo::GetDVTimecode(REFERENCE_TIME *dwRefTime,int *dwHour, int *dwMin, int *dwSec, int *dwTotSec)
{
   REFERENCE_TIME rtCurrentTime;
   m_pMediaSeeking->GetPositions(&rtCurrentTime,NULL);
   *dwRefTime   = rtCurrentTime;

   *dwTotSec = (int)((*dwRefTime) / 10000000);
   *dwHour   = *dwTotSec / 3600;
   *dwMin    = (*dwTotSec - (*dwHour * 3600))/60;
   *dwSec    = (*dwTotSec - (*dwHour * 3600) - (*dwMin * 60));

   return true;
}


bool CNMPlayVideo::GetDVTimecodeTotal(REFERENCE_TIME *dwRefTime,int *dwHour, int *dwMin, int *dwSec, int *dwTotSec)
{
   *dwRefTime   = m_rtTotalTime;

   *dwTotSec = (int)((*dwRefTime) / 10000000);
   *dwHour   = *dwTotSec / 3600;
   *dwMin    = (*dwTotSec - (*dwHour * 3600))/60;
   *dwSec    = (*dwTotSec - (*dwHour * 3600) - (*dwMin * 60));


   return true;
}

REFERENCE_TIME CNMPlayVideo::GetDVTimecodeS()
{
   REFERENCE_TIME rtCurrentTime;
   m_pMediaSeeking->GetPositions(&rtCurrentTime,NULL);
   return rtCurrentTime;
}


REFERENCE_TIME CNMPlayVideo::GetDVTimecodeTotalS()
{
   return m_rtTotalTime;
}

void CNMPlayVideo::SetToStartPos()
{
   SetAviRefTime(0);
}


void CNMPlayVideo::CloseClip()
{
   
  if(m_pMediaControl)
  {
     m_pMediaControl->Stop();
  }
   
   m_CurrentStatus  = Stopped;
   
   
   SAFE_RELEASE(m_pMediaSeeking);
   SAFE_RELEASE(m_pBasicAudio);
   SAFE_RELEASE(m_pMediaControl);
   SAFE_RELEASE(m_pGraphBuilder);
   SAFE_RELEASE(m_pGraphFilter);
}

void CNMPlayVideo::GetFilename(TCHAR *pszFull, TCHAR *pszFile, int iLen)
{
   int nLength;
   TCHAR szPath[MAX_PATH]={0};
   BOOL bSetFile=FALSE;
   
   _tcscpy_s(szPath,MAX_PATH, pszFull);
   nLength = _tcslen(szPath);
   
   for (int i=nLength-1; i>=0; i--)
   {
      if ((szPath[i] == '\\') || (szPath[i] == '/'))
      {
         szPath[i] = '\0';
         lstrcpy(pszFile, &szPath[i+1]);
         bSetFile = TRUE;
         break;
      }
   }
   
   // If there was no path given (just a file name), then
   // just copy the full path to the target path.
   if (!bSetFile)
      _tcscpy_s(pszFile,iLen, pszFull);
}

void CNMPlayVideo::SetAviRefTime(REFERENCE_TIME RefTime)
{
   if ((!m_pMediaControl) || (!m_pMediaSeeking))
      return;

   if(RefTime > m_rtTotalTime)
      RefTime = m_rtTotalTime;

   m_pMediaSeeking->SetPositions(&RefTime,AM_SEEKING_AbsolutePositioning,NULL, AM_SEEKING_NoPositioning);
}

void CNMPlayVideo::SetVolumePourcent(int dwVal,bool bApply)
{
   m_lVolumePC = dwVal;

   if(bApply)
   {
      long dwVolume = 0;

      if(dwVal == 0)
         dwVolume = VOLUME_SILENCE;
      else if(dwVal == 100)
         dwVolume = VOLUME_FULL;
      else
      {
         int dwVal2 = (dwVal/2)+50;
         long dwVolumeTmp = (dwVal2 * 10000) / 100;
         dwVolume    = (10000 - dwVolumeTmp)*-1;
      }
   
      if(m_pBasicAudio)
      {
         m_pBasicAudio->put_Volume(dwVolume);
      }
   }
}

void CNMPlayVideo::CmdPlay()
{
   if ((!m_pMediaControl) || (!m_pMediaSeeking))
      return;
   
   m_pMediaControl->Run();
   m_CurrentStatus = Running;
}

void CNMPlayVideo::CmdStop()
{
   HRESULT hr;
   
   if ((!m_pMediaControl) || (!m_pMediaSeeking))
      return;
   
   // Stop and reset postion to beginning
   if((m_CurrentStatus == Paused) || (m_CurrentStatus == Running))
   {
      LONGLONG pos = 0;
      hr = m_pMediaControl->Stop();
      m_CurrentStatus = Stopped;
      
      hr = m_pMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning);
      
      // Display the first frame to indicate the reset condition
      hr = m_pMediaControl->Pause();
   }
}

void CNMPlayVideo::CmdPause()
{
   HRESULT hr;
   if (!m_pMediaControl)
      return;
   // Toggle play/pause behavior
   if((m_CurrentStatus == Paused) || (m_CurrentStatus == Stopped))
   {
      hr = m_pMediaControl->Run();
      m_CurrentStatus = Running;
   }
   else
   {
      hr = m_pMediaControl->Pause();
      m_CurrentStatus = Paused;
   }
}

void CNMPlayVideo::CmdPausePause()
{
   HRESULT hr;
   if (!m_pMediaControl)
      return;
   // Toggle play/pause behavior
   hr = m_pMediaControl->Pause();
   m_CurrentStatus = Paused;
}


const char * hresultNameLookup(HRESULT hres)
{
   switch(hres)
   {
   case VFW_E_CANNOT_RENDER:
      return "VFW_E_CANNOT_RENDER";
      break;
   case VFW_E_INVALID_FILE_FORMAT:
      return "VFW_E_INVALID_FILE_FORMAT";
      break;
   case VFW_E_NOT_FOUND:
      return "VFW_E_NOT_FOUND";
      break;
   case VFW_E_NOT_IN_GRAPH:
      return "VFW_E_NOT_IN_GRAPH";
      break;
   case VFW_E_UNKNOWN_FILE_TYPE:
      return "VFW_E_UNKNOWN_FILE_TYPE";
      break;
   case VFW_E_UNSUPPORTED_STREAM:
      return "VFW_E_UNSUPPORTED_STREAM";
      break;
   case VFW_E_CANNOT_CONNECT:
      return "VFW_E_CANNOT_CONNECT";
      break;
   case VFW_E_CANNOT_LOAD_SOURCE_FILTER:
      return "VFW_E_CANNOT_LOAD_SOURCE_FILTER";
      break;
   case VFW_S_PARTIAL_RENDER:
      return "VFW_S_PARTIAL_RENDER";
      break;
   case VFW_S_VIDEO_NOT_RENDERED:
      return "VFW_S_VIDEO_NOT_RENDERED";
      break;
   case VFW_S_AUDIO_NOT_RENDERED:
      return "VFW_S_AUDIO_NOT_RENDERED";
      break;
   case VFW_S_DUPLICATE_NAME:
      return "VFW_S_DUPLICATE_NAME";
      break;
   case VFW_S_MEDIA_TYPE_IGNORED:
      return "VFW_S_MEDIA_TYPE_IGNORED";
      break;
   case E_INVALIDARG:
      return "E_INVALIDARG";
      break;
   case DDERR_INCOMPATIBLEPRIMARY:
      return "DDERR_INCOMPATIBLEPRIMARY";
      break;
   case DDERR_INVALIDCAPS:
      return "DDERR_INVALIDCAPS";
      break;
   case DDERR_INVALIDOBJECT :
      return "DDERR_INVALIDOBJECT";
      break;
   case DDERR_INVALIDPIXELFORMAT:
      return "DDERR_INVALIDPIXELFORMAT";
      break;
   case DDERR_NOALPHAHW :
      return "DDERR_NOALPHAHW";
      break;
   case DDERR_NOCOOPERATIVELEVELSET :
      return "DDERR_NOCOOPERATIVELEVELSET";
      break;
   case DDERR_NODIRECTDRAWHW :
      return "DDERR_NODIRECTDRAWHW";
      break;
   case DDERR_NOEMULATION :
      return "DDERR_NOEMULATION";
      break;
   case VFW_E_BUFFERS_OUTSTANDING:
      return "VFW_E_BUFFERS_OUTSTANDING";
      break;
   case DDERR_NOEXCLUSIVEMODE :
      return "DDERR_NOEXCLUSIVEMODE ";
      break;
   case DDERR_NOFLIPHW:
      return "DDERR_NOFLIPHW";
      break;
   case DDERR_NOMIPMAPHW:
      return "DDERR_NOMIPMAPHW";
      break;
   case DDERR_NOOVERLAYHW :
      return "DDERR_NOOVERLAYHW ";
      break;
   case E_OUTOFMEMORY:
      return "E_OUTOFMEMORY";
      break;
   case VFW_E_NO_DISPLAY_PALETTE:
      return "VFW_E_NO_DISPLAY_PALETTE";
      break;
   case VFW_E_NO_COLOR_KEY_FOUND:
      return "VFW_E_NO_COLOR_KEY_FOUND";
      break;
   case VFW_E_PALETTE_SET:
      return "VFW_E_PALETTE_SET";
      break;
   case DDERR_NOZBUFFERHW :
      return "DDERR_NOZBUFFERHW ";
      break;
   case DDERR_OUTOFVIDEOMEMORY :
      return "DDERR_OUTOFVIDEOMEMORY";
      break;
   case DDERR_PRIMARYSURFACEALREADYEXISTS:
      return "DDERR_PRIMARYSURFACEALREADYEXISTS ";
      break;
   case DDERR_UNSUPPORTEDMODE:
      return "DDERR_UNSUPPORTEDMODE";
      break;
   case VFW_E_NO_ADVISE_SET:
      return "VFW_E_NO_ADVISE_SET";
      break;
   case S_OK:
      return "S_OK";
      break;
   case S_FALSE:
      return "S_FALSE";
      break;
   case VFW_S_CONNECTIONS_DEFERRED:
      return "VFW_S_CONNECTIONS_DEFERRED";
      break;
   case 0x80040154:
      return "Class not registered";
      break;
   case E_FAIL:
      return "E_FAIL";
      break;
   case VFW_E_DVD_OPERATION_INHIBITED:
      return "VFW_E_DVD_OPERATION_INHIBITED";
      break;
   case VFW_E_DVD_INVALIDDOMAIN:
      return "VFW_E_DVD_INVALIDDOMAIN";
      break;
   case E_NOTIMPL:
      return "E_NOTIMPL";
      break;
   case VFW_E_WRONG_STATE:
      return "VFW_E_WRONG_STATE";
      break;
   case E_PROP_SET_UNSUPPORTED:
      return "E_PROP_SET_UNSUPPORTED";
      break;
   case VFW_E_NO_PALETTE_AVAILABLE:
      return "VFW_E_NO_PALETTE_AVAILABLE";
      break;
   case E_UNEXPECTED:
      return "E_UNEXPECTED";
      break;
   case VFW_E_DVD_NO_BUTTON:
      return "VFW_E_DVD_NO_BUTTON";
      break;
   case VFW_E_DVD_GRAPHNOTREADY:
      return "VFW_E_DVD_GRAPHNOTREADY";
      break;
   case VFW_E_NOT_OVERLAY_CONNECTION:
      return "VFW_E_NOT_OVERLAY_CONNECTION";
      break;
   case VFW_E_DVD_RENDERFAIL:
      return "VFW_E_DVD_RENDERFAIL";
      break;
   case VFW_E_NOT_CONNECTED:
      return "VFW_E_NOT_CONNECTED";
      break;
   case E_NOINTERFACE:
      return "E_NOINTERFACE";
      break;
   case VFW_E_NO_COLOR_KEY_SET :
      return "VFW_E_NO_COLOR_KEY_SET ";
      break;
   case VFW_E_NO_INTERFACE:
      return "VFW_E_NO_INTERFACE";
      break;
   case 0x8004020c:
      return "VFW_E_BUFFER_NOTSET";
      break;
   case 0x80040225:
      return "VFW_E_NOT_PAUSED";
   case 0x80070002:
      return "System cannot find the file specified";
      break;
   case 0x80070003:
      return "System cannot find the path specified";
      break;
   case VFW_E_DVD_DECNOTENOUGH:
      return "VFW_E_DVD_DECNOTENOUGH";
      break;
   case VFW_E_ADVISE_ALREADY_SET:
      return "VFW_E_ADVISE_ALREADY_SET";
      break;
   case VFW_E_DVD_CMD_CANCELLED:
      return "VFW_E_DVD_CMD_CANCELLED";
      break;
   case VFW_E_DVD_MENU_DOES_NOT_EXIST:
      return "VFW_E_DVD_MENU_DOES_NOT_EXIST";
      break;
   case VFW_E_DVD_WRONG_SPEED:
      return "VFW_E_DVD_WRONG_SPEED";
      break;
   case VFW_S_DVD_NON_ONE_SEQUENTIAL:
      return "VFW_S_DVD_NON_ONE_SEQUENTIAL";
      break;
   case E_POINTER:
      return "E_POINTER";
      break;
   case VFW_E_DVD_NOT_IN_KARAOKE_MODE:
      return "VFW_E_DVD_NOT_IN_KARAOKE_MODE";
      break;
   case VFW_E_DVD_INVALID_DISC:
      return "VFW_E_DVD_INVALID_DISC";
      break;
   case VFW_E_DVD_STREAM_DISABLED:
      return "VFW_E_DVD_STREAM_DISABLED";
      break;
   case VFW_E_NOT_STOPPED:
      return "VFW_E_NOT_STOPPED";
      break;
   default:        
      return "Unrecognized";
      break;
   }
}