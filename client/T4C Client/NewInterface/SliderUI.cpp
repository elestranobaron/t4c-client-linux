// SliderUI.cpp: implementation of the SliderUI class.
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"
#include "SliderUI.h"

namespace
{

    T3VSBSound *buttonDownSound = NULL;
    T3VSBSound *buttonUpSound = NULL;

    void InitSound()
	{
        if( !buttonDownSound )
		{
            buttonDownSound = new T3VSBSound;
            buttonUpSound = new T3VSBSound;

            buttonDownSound->Create( "Generic Drop Item", TS_MEMORY );
            buttonUpSound->Create( "Button Release Sound", TS_MEMORY );
        }
    }
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SliderUI::SliderUI( EventVisitor &iEvent )  
:
    eventVisitor( iEvent ),
    step( 1 ),
    minRange( 0 ),
    maxRange( 100 ),
    RangeDelta(101),
    curPos( 0 ),
    dragging( false ),
    rightArrowDown( false ),
    leftArrowDown( false ),
    soundEnabled( true )
{
    InitSound();

    prevPos = curPos;

    sliderGraph.SetPos( FramePos( 0, 0, 20, 16 ) );
    //sliderGraph.SetSpriteId( "V3_HSliderTick" );
    sliderGraph.SetSpriteId( "V3_HSliderTickA",true,"V3_HSliderTickAMask" );
}

SliderUI::~SliderUI()
{

}

void SliderUI::SetSliderDef(int iMin, int iMax,int iOX, int iOY,int iBtnW,int iRX1,int iRY1,int iRX2,int iRY2)
{
  
   //Brightness
   SetPos  (             FramePos( iOX+iRX1      , iOY+iRY1, iOX+iRX2      , iOY+iRY2 ) );
   SetRange( iMin, iMax, FramePos( iOX+iRX1+iBtnW, iOY+iRY1, iOX+iRX2-iBtnW, iOY+iRY2 ) );

   SetScrollRegions( FramePos( iOX+iRX1      , iOY+iRY1, iOX+iRX1+iBtnW, iOY+iRY2 ),
                     FramePos( iOX+iRX2-iBtnW, iOY+iRY1, iOX+iRX2      , iOY+iRY2 ),"","" );
}

//  Sets the new slider range
void SliderUI::SetRange(int iminRange,int imaxRange,FramePos framePos)
{
    minRange = iminRange;
    maxRange = imaxRange;
    sliderArea = framePos;

    RangeDelta = maxRange-minRange+1;
}

//  Sets the step.
void SliderUI::SetStep(int istep)
{
    step = istep;
}

//  Sets the scroll regions
void SliderUI::SetScrollRegions(FramePos scrollLeft,FramePos scrollRight,std::string leftSpriteId,std::string rightSpriteId)
{
    panLeft = scrollLeft;
    panRight = scrollRight;
	
    if(leftSpriteId.c_str()[0] != NULL)
    {
		scrollLeftSprite.SetSpriteId( leftSpriteId );
		scrollRightSprite.SetSpriteId( rightSpriteId );
		
		scrollLeftSprite.SetPos( scrollLeft );
		scrollRightSprite.SetPos( scrollRight );
		
		scrollLeftSprite.Show();
		scrollRightSprite.Show();
    }
}

//  Sets the slider's position 
void SliderUI::SetSliderPos(int ipos)
{
    curPos = ipos;
    if( curPos > maxRange )
       curPos = maxRange;
    if( curPos < minRange )
      curPos = minRange;

    prevPos = curPos;
}

// Return: int, The position
int SliderUI::GetSliderPos( void )
{
    return curPos;
}

//  Sets the graphic to display as slider button
void SliderUI::SetSliderGraph(GraphUI &graphUI)
{
    sliderGraph.SetSpriteId( graphUI.GetSpriteId() );
    sliderGraph.SetPos( graphUI.GetPos() );
}


//  Draws the sliderUI.
void SliderUI::Draw(V2SPRITEFX *vsfFX)
{
    // If in drag mode and left mouse is down.
    if( dragging && IsLeftMouseDown() )
	{
        MousePos mousePos( MouseDevice::xPos, MouseDevice::yPos );
        
        // Need to be within the slider's width
        if( mousePos.x >= sliderArea.x && mousePos.x <= sliderArea.maxx )
		{
            // Do as a click.
            LeftMouseDown( mousePos );
        }
    }
	else
	{
        dragging = false;
    }
    
    if( rightArrowDown && IsLeftMouseDown() )
        scrollRightSprite.Draw( vsfFX );
    else
        rightArrowDown = false;
    
    if( leftArrowDown && IsLeftMouseDown() )
        scrollLeftSprite.Draw( vsfFX );
    else
        leftArrowDown = false;

    
    // If a graph was set (not a quicksilver style slider).
    if( sliderGraph.IsSpriteDefined() )
    {
        int x, y;

        FramePos sliderPos = sliderGraph.GetPos();

        // Set the slider's y position.
        y = sliderArea.Height() / 2 + sliderArea.y - sliderPos.Height() / 2;


        double relPos = static_cast< double >( curPos - minRange ) / 
                        static_cast< double >( RangeDelta);

 
		int iSlidDelta = sliderArea.Width()-sliderPos.Width();


      double dX = 0.00;
      if(RangeDelta == 1)
         dX = ((static_cast< double >(curPos-minRange)*iSlidDelta)/static_cast< double >(1))+sliderArea.x;
      else
         dX = ((static_cast< double >(curPos-minRange)*iSlidDelta)/static_cast< double >(RangeDelta-1))+sliderArea.x;

      /*
      if(RangeDelta == 1)
		   x = (((curPos-minRange)*iSlidDelta)/(1))+sliderArea.x;
      else
         x = (((curPos-minRange)*iSlidDelta)/(RangeDelta-1))+sliderArea.x;
      */

      x = (int)dX;

        //x = (int)(relPos * sliderArea.Width() - relPos * ( sliderPos.Width() / 2 ) + sliderArea.x);

        sliderPos.SetNewOffset( MousePos( x, y ) );

        sliderGraph.SetPos( sliderPos );
        sliderGraph.Draw( vsfFX );
    }
}

// Mouse up
bool SliderUI::LeftMouseUp(MousePos mousePos)
{
	if( panLeft.MousePosInFrame( mousePos ) )
	{
		InitSound();
		
		if( soundEnabled )
			buttonUpSound->Play( FALSE );
		
		curPos -= step;
		if( curPos < minRange )
			curPos = minRange;
		
		// If the positions changed.
		if( prevPos != curPos )
			eventVisitor.LeftClicked();// Call the event visitor.
		
		prevPos = curPos;
		
		return true;
	}
	else if( panRight.MousePosInFrame( mousePos ) )
	{
		InitSound();
		
		if( soundEnabled )
			buttonUpSound->Play( FALSE );
		
		curPos += step;
		if( curPos > maxRange )
			curPos = maxRange;
		
		// If the positions changed.
		if( prevPos != curPos )
			eventVisitor.LeftClicked(); // Call the event visitor.
		prevPos = curPos;
		
		return true;
	}
	return false;
}

//  Mouse down on the slider UI.
bool SliderUI::LeftMouseDown(MousePos mousePos)
{
	if( !sliderArea.MousePosInFrame( mousePos ) )
	{
		if( panLeft.MousePosInFrame( mousePos ) )
		{
			InitSound();
			
			if( soundEnabled )
				buttonDownSound->Play( FALSE );
			
			leftArrowDown = true;
			return true;
		}
		else if( panRight.MousePosInFrame( mousePos ) )
		{
			InitSound();
			
			if( soundEnabled )
				buttonDownSound->Play( FALSE );
			
			rightArrowDown = true;
			return true;
		}        
		return false;
	}
	
	int halfStep = sliderArea.Width() / ( RangeDelta ) * step / 2;
	
	double relPos = static_cast< double >( mousePos.x - sliderArea.x + halfStep ) / static_cast< double >( sliderArea.Width() );
	
	double absRange = static_cast< double >(RangeDelta ) / static_cast< double >( step );
	
	int newAbsPos = static_cast< int >( relPos * absRange );
	
	curPos = ( newAbsPos * step ) + minRange;
	if( curPos > maxRange )
		curPos = maxRange;
	if( curPos < minRange )
		curPos = minRange;
	
	
	
	dragging = true;
	
	// If the positions changed.
	if( prevPos != curPos )
	{
		InitSound();
		
		if( soundEnabled )
			buttonUpSound->Play( FALSE );
		
		
		eventVisitor.LeftClicked(); // Call the event visitor.
	}
	prevPos = curPos;
	
	return true;
}

