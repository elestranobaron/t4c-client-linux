// ButtonUI.cpp
//

#include "../pch.h"

#include "ButtonUI.h"

extern Font *V3_BtnFontB;

namespace{

    ButtonUI *globalCurrentPressedUI = NULL;

    T3VSBSound *buttonDownSound = NULL;
    T3VSBSound *buttonUpSound = NULL;

    void InitSound(){
        if( !buttonDownSound ){
            buttonDownSound = new T3VSBSound;
            buttonUpSound = new T3VSBSound;

            buttonDownSound->Create( "Generic Drop Item", TS_MEMORY );
            buttonUpSound->Create( "Button Release Sound", TS_MEMORY );
        }

    }
};
 
ButtonUI::ButtonUI( EventVisitor &eventVisitor, std::string theText ,DWORD TextColor)
    :   buttonEvent( eventVisitor ),
        buttonText  ( theText ),
        pressed( true ),
        enableDragCycle( false ),
        soundEnabled( true ),
        iSpinCnt(0),
        dwTextColor(TextColor)
{
    InitSound();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ButtonUI::Draw( V2SPRITEFX *vsfFX )
//////////////////////////////////////////////////////////////////////////////////////////
//  Draws the button text on top of the normal sprite.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
    // If the button was pressed.
    if( pressed && GameUI::IsLeftMouseDown() && downUI.IsSpriteDefined() )
    {
        // Draw the down button version.
        downUI.Draw( vsfFX );
        //DrawTextButton(1);
        return;
    }
    else
    {
        pressed = false;
    }

    // If the mouse is hovering over the button.
    MousePos mousePos( MouseDevice::xPos, MouseDevice::yPos );

    if( GetPos().MousePosInFrame( mousePos ) && selectionUI.IsSpriteDefined() )
    {
        // Draw the selection UI
        selectionUI.Draw( vsfFX );
        //DrawTextButton();
        return;
    }

    // Draw the button normally.
    GameUI::Draw( vsfFX );

    //DrawTextButton();

    
    
}
 
int ButtonUI::TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise)
{
   int dwSize = (int)pFont->GetFLen(hdc,pStr);
   return((dwBoxZise-dwSize)/2);
}

void ButtonUI::DrawTextButton(int iDownOffset)
{
   if(buttonText != "")
   {
      HDC hdc;
      DXDGetDC(&hdc, 35);

      FramePos btnFrame = GetPos();
      int iFontH = V3_BtnFontB->GetFHeight(hdc);
      int yBaseOffset = (btnFrame.Height()-iFontH)/2;
      
      
      V3_BtnFontB->DrawFont(hdc, btnFrame.x+TxtOff(V3_BtnFontB,hdc,(char*)buttonText.c_str() ,btnFrame.Width()), btnFrame.y+yBaseOffset+iDownOffset , dwTextColor    , 0, (char*)buttonText.c_str());
      DXDReleaseDC(hdc, 35);
    }
}

void ButtonUI::DrawTextButtonExt(HDC hdc)
{
   if(buttonText != "" && IsShown())
   {
      int iOffset = 0;
      if( pressed && GameUI::IsLeftMouseDown() && downUI.IsSpriteDefined() )
         iOffset = 1;

      FramePos btnFrame = GetPos();
      int iFontH = V3_BtnFontB->GetFHeight(hdc);
      int yBaseOffset = (btnFrame.Height()-iFontH)/2;


      V3_BtnFontB->DrawFont(hdc, btnFrame.x+TxtOff(V3_BtnFontB,hdc,(char*)buttonText.c_str() ,btnFrame.Width()), btnFrame.y+yBaseOffset+iOffset , dwTextColor    , 0, (char*)buttonText.c_str());
   }
}


//////////////////////////////////////////////////////////////////////////////////////////
bool ButtonUI::LeftMouseUp( MousePos mousePos )
//////////////////////////////////////////////////////////////////////////////////////////
// Mouse button was released on top of this control.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
    InitSound();

    if( IsDblClick() )
    {
        if( soundEnabled )
        {
            buttonUpSound->Play( TRUE );
        }
        buttonEvent.LeftDblClicked();
    }
    else
    {
        if( soundEnabled )
        {
            buttonUpSound->Play( TRUE );
        }
        // Notify the button's event.
        buttonEvent.LeftClicked();
    }

    iSpinCnt = 0;
    return true;
}

bool ButtonUI::LeftMouseDown( MousePos mousePos )
{
    InitSound();

    if( soundEnabled )
    {
        buttonDownSound->Play( TRUE );
    }
    buttonEvent.StartDragLButtonDown(mousePos.x,mousePos.y);
    pressed = true;
    iSpinCnt = 0;
    return true;
}

bool ButtonUI::DragCycle( MousePos mousePos ){
    
    if( enableDragCycle )
    {
        buttonEvent.LeftClicked();
        iSpinCnt++;
    }

    return true;
}

void ButtonUI::SetEnableDragCycle()
{
   enableDragCycle = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ButtonUI::SetPos
//////////////////////////////////////////////////////////////////////////////////////////
//  Sets the button's position
// 
(
 FramePos framePos // The position
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    // Assign the button's position to the down and highlight UIs.
    downUI.SetPos( framePos );
    selectionUI.SetPos( framePos );

    GameUI::SetPos( framePos );
}

void ButtonUI::SetBtnInfo(std::string strText,DWORD TextColor)
{
   buttonText = strText;
   dwTextColor = TextColor;
}