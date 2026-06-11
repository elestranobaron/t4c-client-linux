// ButtonUI.h
//

#if !defined(ButtonUI_h)
#define ButtonUI_h

#include "GraphUI.h"
#include <string>

class ButtonUI : public ControlUI
{
public:
    ButtonUI( EventVisitor &eventVisitor = EmptyEvent::GetInstance(), std::string buttonText = "" ,DWORD TextColor = 0x00);
    
    // Buttons respond to mouse up/down (clicks).
    bool LeftMouseUp( MousePos mousePos );
    bool LeftMouseDown( MousePos mousePos );
    bool DragCycle( MousePos mousePos );
    void SetEnableDragCycle();
    int  GetSpinCnt(){return iSpinCnt;}
    

    // Needed to draw the button's text.
    void Draw( V2SPRITEFX *vsfFX );
    int TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise);
    void DrawTextButton(int iDownOffset = 0);
    void DrawTextButtonExt(HDC hdc);

    void SetDownUI( std::string downSpriteId,bool bMask=false,std::string downSpriteIdMask="" ){downUI.SetSpriteId( downSpriteId,bMask,downSpriteIdMask );}
    void SetHighlightUI( std::string highlightSpriteId,bool bMask=false,std::string highlightSpriteIdMask="" ){selectionUI.SetSpriteId( highlightSpriteId,bMask,highlightSpriteIdMask );}

    void SetPos( FramePos framePos );
    void SetBtnInfo(std::string strText,DWORD TextColor = 0x00);
    void EnableSound( bool enable ){soundEnabled = enable;}

private:
    GraphUI downUI;
    GraphUI selectionUI;

    bool pressed;
    bool enableDragCycle;
    bool soundEnabled;

    std::string	  buttonText;
    EventVisitor &buttonEvent;
    int     iSpinCnt;
    DWORD   dwTextColor;

};

#endif /* ButtonUI_h */
