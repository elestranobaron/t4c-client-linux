// DropZoneUI.h
//

#if !defined(DropZoneUI_h)
#define DropZoneUI_h

#include "ControlUI.h"

class DropZoneUI : public ControlUI
{
public:
    DropZoneUI( EventVisitor &eventVisitor );

    void Draw( V2SPRITEFX *vsfFX );
    void DrawHelp( V2SPRITEFX *vsfFX );

    bool LeftMouseDown( MousePos mousePos );
    bool LeftMouseUp( MousePos mousePos );
    bool RightMouseUp( MousePos mousePos );

    void SetItem( GameUI *newUI ){
        currentUI = newUI;
    }

    GameUI *GetItem( void ){
        return currentUI;
    }

    void SetBypassRClick( BOOL bVal ){
       bypassRightClick = bVal;
    }


private:
    void DragItem( MousePos mousePos );
    void DropItem( MousePos mousePos );

    GameUI  *currentUI;

    BOOL bypassRightClick;
    EventVisitor &dropzoneEvent;
};

#endif /* DropZoneUI_h */
