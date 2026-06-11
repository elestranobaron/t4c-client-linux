// TextPageUI.h: interface for the TextPageUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTPAGEUI_H__E665B792_96BD_11D3_85B7_00E02922FA40__INCLUDED_)
#define AFX_TEXTPAGEUI_H__E665B792_96BD_11D3_85B7_00E02922FA40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlUI.h"
#include "StaticTextUI.h"
#include "../ScrollUI.h"
#include "../NewFont.h"
#include <list>

class TextPageUI : public ControlUI, public ScrollUI
{
public:
	TextPageUI();
	virtual ~TextPageUI();

    virtual bool WheelUp( MousePos mousePos );
    virtual bool WheelDown( MousePos mousePos );
    virtual void Draw( V2SPRITEFX *vsfFX );
    virtual bool RightClick( MousePos mousePos );
    virtual bool LeftMouseDown( MousePos mousePos );
    virtual bool LeftMouseUp( MousePos mousePos );
    virtual bool DragCycle( MousePos mousePos );
    virtual void SetPos( FramePos framePos );

    void AddText( std::string str, DWORD textColor,bool bNewEntry = true);
    void ClearList();
    void ReconstructList();
    void SetPageDef(int iOX, int iOY,int iBtnW,int iRX1,int iRY1,int iRX2,int iRY2,GraphUI *pSUp,GraphUI *pSDown,int iSOYT,int iSOYB,bool bChangeScroll,bool bLeftScroll);

    void SetTextArea( FramePos itextFrame ,bool bChangeScroll = true);
    void UpdateViewSize( void );

private:    
    virtual void ScrollChanged( void );

    
    
    
    TextObject *m_TempText;

    typedef struct _ChatLineInfo
    {
        std::string text;
        DWORD textColor;
    }ChatLineInfo;

    

    FramePos m_textFrame;


    int m_iFontSize;
    int m_iMaxLines;
 

    
    
    int initialOffset;


    CRITICAL_SECTION m_crLock;

    
 
    StaticTextUI m_ListDrawST[100];
    typedef struct _sListBackup
    {
       std::string strInput;
       DWORD dwColor;
    }sListBackup;

    vector< ChatLineInfo > m_ListText;
    list< sListBackup  > m_vlAllInputString;
};

#endif // !defined(AFX_TEXTPAGEUI_H__E665B792_96BD_11D3_85B7_00E02922FA40__INCLUDED_)
