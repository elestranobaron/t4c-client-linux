// SliderUI.h: interface for the SliderUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLIDERUI_H__E0278FC1_A35E_11D3_85C8_00E02922FA40__INCLUDED_)
#define AFX_SLIDERUI_H__E0278FC1_A35E_11D3_85C8_00E02922FA40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlUI.h"
#include "GraphUI.h"

class SliderUI : public ControlUI
{
public:
	SliderUI( EventVisitor &eventVisitor );
	virtual ~SliderUI();

    void SetSliderDef(int iMin, int iMax, int iOX, int iOY,int iBtnW,int iRX1,int iRY1,int iRX2,int iRY2);
    void SetRange( int minRange, int maxRange, FramePos sliderArea );
    void SetStep( int step );
    void SetSliderPos( int pos );
    void SetSliderGraph( GraphUI &graphUI );

    int GetSliderPos( void );

    virtual void Draw( V2SPRITEFX *vsfFX );

    virtual bool LeftMouseDown( MousePos mousePos );
    virtual bool LeftMouseUp( MousePos mousePos );

    virtual void SetScrollRegions( FramePos scrollLeft, FramePos scrollRight, std::string leftSpriteId, std::string rightSpriteId );


    void EnableSound( bool enable ){
        soundEnabled = enable;
    }

private:
    GraphUI sliderGraph;

    int step;
    int minRange;
    int maxRange;
    int RangeDelta;

    int curPos;
    int prevPos;

    bool dragging;

    GraphUI scrollLeftSprite;
    GraphUI scrollRightSprite;

    bool rightArrowDown;
    bool leftArrowDown;

    FramePos sliderArea;
    FramePos panRight, panLeft;

    EventVisitor &eventVisitor;

    bool soundEnabled;
};

#endif // !defined(AFX_SLIDERUI_H__E0278FC1_A35E_11D3_85C8_00E02922FA40__INCLUDED_)
