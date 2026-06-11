// ListUI.cpp
//
#include "../pch.h"

#include "ListUI.h"
#include "EventVisitor.h"

namespace{
    T3VSBSound *selectSound = NULL;

    void InitSound(){
        if( !selectSound ){
            selectSound = new T3VSBSound;

            selectSound->Create( "Generic Drop Item", TS_MEMORY );
        }

    }
};

ListUI::ListUI( EventVisitor &newVisitor )
    :   listEvent( newVisitor ),
        variableHeight( false ),
        selectionSprite( NULL ),
        selection( NULL ),
        selectionRC( NULL ),
        iSpinCnt(0)
{
    InitSound();
}

ListUI::~ListUI(){
    DeleteList();
}


void ListUI::SetListDef(int iOX, int iOY,int iBtnW,int iRX1,int iRY1,int iRX2,int iRY2,GraphUI *pSUp,GraphUI *pSDown)
{
   int iSXS = iRX2-iBtnW;
   
   SetPos          ( FramePos( iOX+iRX1, iOY+iRY1      , iOX+iRX2, iOY+iRY2         )   );
   SetScrollRegions( FramePos( iOX+iSXS, iOY+iRY1      , iOX+iRX2, iOY+iRY1+iBtnW   ), 
                     FramePos( iOX+iSXS, iOY+iRY2-iBtnW, iOX+iRX2, iOY+iRY2         ), 
                     FramePos( iOX+iSXS, iOY+iRY1+iBtnW, iOX+iRX2, iOY+iRY2-iBtnW-11)   ); //11 == 1/2 du bmp qui scroll
   pSUp->SetPos  (   FramePos( iOX+iSXS, iOY+iRY1      , iOX+iRX2, iOY+iRY1+iBtnW   )   );
   pSDown->SetPos(   FramePos( iOX+iSXS, iOY+iRY2-iBtnW, iOX+iRX2, iOY+iRY2         )   );

}


//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::Draw( V2SPRITEFX *vsfFX )
//////////////////////////////////////////////////////////////////////////////////////////
//  Draws the list box.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
    // Draw the listUI's background.
    GameUI::Draw( vsfFX );


    // Setup the V2SPRITEFX for all sprites.
    V2SPRITEFX fx;

    memset( &fx, 0, sizeof( fx ) );

    // Setup the clipping rectangle
    RECT rect = { GetPos().x, GetPos().y, GetPos().maxx, GetPos().maxy };
    fx.lpClipRect = &rect;
    
    // Specify no correction.
    fx.dwFX |= FX_NOCORRECTION;
    

    // Draw the list's items
    // For all the lines.
    int newY = GetPos().y - ( linePos * ( rowSize + spacing.Height() ) );
    
    UINT j;    
    for( j = 0; j < itemGrid[ 0 ].size(); j++ )
    {
        // For all the columns               
        UINT i;
        for( i = 0; i < itemGrid.size(); i++ )
        {
            GameUI *targetUI = itemGrid[ i ][ j ];
            if( targetUI != NULL )
            {
                FramePos newPos = targetUI->GetPos();
                int iRealYpos = 0;
                int columnOffset = 0;
                UINT z;
                for( z = 0; z < i; z++ )
                {
                    columnOffset += columnAttr[ z ].columnSize + spacing.Width();
                }

                if( columnAttr[ i ].centerColumn )
                { 
                   
                   iRealYpos = (int)(variableHeight ? newY + ( static_cast< double >( rowSize ) / 2.0 - 
                                                         static_cast< double >( targetUI->GetPos().Height() ) ) / 2.0
                                              : 
                                               ( j - linePos ) * ( rowSize + spacing.Height() ) + 
                                               ( static_cast< double >( rowSize ) / 2.0 - 
                                               static_cast< double >( targetUI->GetPos().Height() ) / 2.0 ) +GetPos().y);

                    newPos.SetNewOffset(MousePos(                            
                                                   (int)(( static_cast< double >( columnAttr[ i ].columnSize ) / 2.0 - 
                                                     static_cast< double >( targetUI->GetPos().Width() ) / 2.0 ) +columnOffset + GetPos().x),
                                                     iRealYpos));
                }
                else
                {
                   iRealYpos = variableHeight ? 
                                                newY + rowSize / 2 - targetUI->GetPos().Height() / 2
                                              : ( j - linePos ) * ( rowSize + spacing.Height() ) + rowSize / 2 - targetUI->GetPos().Height() / 2 +GetPos().y;
                    
                   //CV:add +2 pour juste tasser un peu les texte des frame quand pas centrer en test
                   newPos.SetNewOffset( MousePos(columnOffset + GetPos().x+2, iRealYpos));
                }

                // If a selection sprite was provided and it corresponds to this UI.
                if( selection == targetUI && selectionSprite != NULL )
                {
                    FramePos selectPos = selectionSprite->GetPos();

                    iRealYpos = variableHeight ? newY:( j - linePos ) * ( rowSize + spacing.Height() ) + GetPos().y + selectionYoff;
                    selectPos.SetNewOffset(MousePos(columnOffset + GetPos().x + selectionXoff,iRealYpos));
                    
                    selectionSprite->SetPos( selectPos );
                    selectionSprite->Draw( &fx );
                }

                targetUI->SetPos( newPos );

                if(iRealYpos >= GetPos().y &&  iRealYpos <GetPos().maxy)
                   targetUI->Draw( &fx ); // Draw the item at its position.  
            }
        }
        
        newY += itemGrid[ 0 ][ j ]->GetPos().Height();
    }

    ScrollUI::DrawScroll( vsfFX );
}


//////////////////////////////////////////////////////////////////////////////////////////
bool ListUI::WheelUp
//////////////////////////////////////////////////////////////////////////////////////////
// Mouse wheel went up! Move list up (scroll up).
// 
(
 MousePos mousePos
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    ScrollUp( 1 );

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ListUI::WheelDown
//////////////////////////////////////////////////////////////////////////////////////////
// Mouse wheel went down! Move list down (scroll down).
// 
(
 MousePos mousePos
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    ScrollDown( 1 );

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ListUI::RightMouseUp
//////////////////////////////////////////////////////////////////////////////////////////
//  Mouse went up
// 
(
 MousePos mousePos // 
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    if( IsClick() )
    {
        GameUI *item = GetItemFromMousePos( mousePos );
        if( item != NULL )
        {
            item->DisplayHelp( true, MouseDevice::xPos - 2, MouseDevice::yPos - 2 );
        }
        else
        {
            GameUI::RightMouseUp( mousePos );
        }

        selection = item;
        selectedColumn = GetColumnFromMousePos( mousePos );
        selectedRow    = GetRowFromMousePos( mousePos );

        selectSound->Play( TRUE );

        if( !IsMouseInScrollBar( mousePos ))
           listEvent.RightClicked();
        
    }
    return true;
}

bool ListUI::LeftMouseDown( MousePos mousePos )
{
    // If this didn't hit the scroll bar.
    if( !ScrollClick( mousePos ) ){        
        if( IsDblClick() ){
            // Use item.
            DblClickItem( mousePos );
        }
    }
    
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ListUI::LeftMouseUp
//////////////////////////////////////////////////////////////////////////////////////////
// The mouse was released on top of this control.
// 
(
 MousePos mousePos
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    if( !IsMouseInScrollBar( mousePos ) && IsClick() )
    {
        ClickItem( mousePos );
    }

    iSpinCnt = 0;
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
const ListUI::GridItem ListUI::GetSelection( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the currently selected ListItem
//////////////////////////////////////////////////////////////////////////////////////////
{
    // Return the grid item at the selected position.
    return selection;
}

//////////////////////////////////////////////////////////////////////////////////////////
const ListUI::GridItem ListUI::GetSelectionRC( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the currently selected ListItem
//////////////////////////////////////////////////////////////////////////////////////////
{
   // Return the grid item at the selected position.
   return selectionRC;
}



//////////////////////////////////////////////////////////////////////////////////////////
int ListUI::GetSelectedColumn( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the selected column, -1 if none.
//////////////////////////////////////////////////////////////////////////////////////////
{
    return selectedColumn;
}

int ListUI::GetSelectedRow( void )
{
   return selectedRow;
}


//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::SetupList
//////////////////////////////////////////////////////////////////////////////////////////
// Set the list's size.
// 
(
 int theColumns,       // Number of columns in the list.
 int theRowSize,
 FramePos theSpacing   // Spacing between colums and rows.
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    rowSize = theRowSize;
    spacing = theSpacing;
    columns = theColumns;

    // Resize the vectors to fit the new column size.
    itemGrid.resize( theColumns );
    columnAttr.resize( theColumns );

}
//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::SetColumnSize
//////////////////////////////////////////////////////////////////////////////////////////
//  Sets the x size of a specific column
// 
(
 int columnNumber, // The column
 int xSize         // The x size.
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    columnAttr[ columnNumber ].columnSize = xSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::DeleteList( void )
//////////////////////////////////////////////////////////////////////////////////////////
// Deletes everything in the list UI.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{
    // If the list is already empty.
    if( itemGrid[ 0 ].size() == 0 ){
        return;
    }
     
    UINT i, j;
    for( i = 0; i < itemGrid.size(); i++ )
    {
        for( j = 0; j < itemGrid[ i ].size(); j++ )
        {
            
            if( itemGrid[ i ][ j ] != NULL ){
                // Delete the gameUI.
                delete itemGrid[ i ][ j ];
                itemGrid[ i ][ j ] = NULL;
            }
        }
        // Resive this column to 0.
        itemGrid[ i ].resize( 0 );
    }

    selection = NULL;
    selectionRC = NULL;
    selectedColumn = -1;
    selectedRow    = -1;

    

    linePos = 0;

    SetListSize( 0 );
}
//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::AddNewItem( bool autoScroll )
//////////////////////////////////////////////////////////////////////////////////////////
//  Adds a new row to the list.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{

    int listSize = itemGrid[ 0 ].size();

    // Remove the visible quantity of items.
    listSize -= GetPos().Height() / ( rowSize + spacing.Height()+1 );

    bool scroll = false;

    if( linePos == listSize - 1 ){
        scroll = true;
    }

    UINT i;
    // Grow all columns to fit the new row.
    for( i = 0; i < itemGrid.size(); i++ ){
        itemGrid[ i ].resize( itemGrid[ i ].size() + 1 );
        itemGrid[ i ][ itemGrid[ i ].size() - 1 ] = NULL;
    }

    listSize = itemGrid[ 0 ].size();

    // Remove the visible quantity of items.
    listSize -= GetPos().Height() / ( rowSize + spacing.Height()+1 );

    if( listSize < 0 ){
        listSize = 0;
    }

    SetListSize( listSize );

    if( scroll && autoScroll ){
        ScrollUp( 32767 );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::AddColumnItem
//////////////////////////////////////////////////////////////////////////////////////////
//  Modifies a column of the current row.
// 
(
 int columnNumber, // The column.
 GameUI *listItem  // The item to put there.
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    itemGrid[ columnNumber ][ itemGrid[ 0 ].size() - 1 ] = listItem;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::CenterColumn
//////////////////////////////////////////////////////////////////////////////////////////
//  Sets the specified column as being centered.
// 
(
 int columnNumber // The columns.
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    columnAttr[ columnNumber ].centerColumn = true;
}

//  Returns the GameUI pointed by the mouse cursor.
GameUI *ListUI::GetItemFromMousePos(MousePos mousePos)
{
           
    int line = ( mousePos.y - GetPos().y ) / ( rowSize + spacing.Height() );

    line += linePos;

    if( line < (int)itemGrid[ 0 ].size() ){

        return itemGrid[ 0 ][ line ];
    }
    return NULL;
}

GameUI *ListUI::GetItemFromMousePosRC( MousePos mousePos ,int Col)
{
   
   int line = ( mousePos.y - GetPos().y ) / ( rowSize + spacing.Height() );
   
   line += linePos;

   if(Col <0)
      return NULL;
   
   if( line < (int)itemGrid[ Col ].size() )
   {
      
      return itemGrid[ Col ][ line ];
   }
   return NULL;
}


//////////////////////////////////////////////////////////////////////////////////////////
int ListUI::GetColumnFromMousePos
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the selected column
// 
(
 MousePos mousePos
)
// Return: int, the hit column.
//////////////////////////////////////////////////////////////////////////////////////////
{    
    int len = GetPos().x;
    
    UINT i;
    for( i = 0; i < columnAttr.size(); i++ ){    
        len += columnAttr[ i ].columnSize;

        if( mousePos.x < len ){
            return i;
        }

        len += spacing.Width();
    }

    return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
int ListUI::GetRowFromMousePos
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the row selected by the current mouse pos.
// 
(
 MousePos mousePos
)
// Return: int, 
//////////////////////////////////////////////////////////////////////////////////////////
{
    int line = ( mousePos.y - GetPos().y ) / ( rowSize + spacing.Height() );

    line += linePos;

    if( line < (int)itemGrid[ 0 ].size() )
    {
        return line;
    }
    return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::DblClickItem
//////////////////////////////////////////////////////////////////////////////////////////
// An item got dbl clicked.
// 
(
 MousePos mousePos // the mouse pos.
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    GameUI *item = GetItemFromMousePos( mousePos );

    selection = item;
    selectedColumn = GetColumnFromMousePos( mousePos );
    selectedRow    = GetRowFromMousePos( mousePos );

    if( item != NULL ){        

        listEvent.LeftDblClicked();
    }
}

//  Clicked on an item.
void ListUI::ClickItem(MousePos mousePos)
{
    GameUI *item   = GetItemFromMousePos( mousePos );
    GameUI *itemRC = GetItemFromMousePosRC( mousePos, GetColumnFromMousePos( mousePos ));


    selection   = item;
    selectionRC = itemRC;
    selectedColumn = GetColumnFromMousePos( mousePos );
    selectedRow    = GetRowFromMousePos( mousePos );

    if( item != NULL )
    {
        selectSound->Play( TRUE );
        listEvent.LeftClicked();
        iSpinCnt++;
    }
}



//////////////////////////////////////////////////////////////////////////////////////////
bool ListUI::DragCycle
//////////////////////////////////////////////////////////////////////////////////////////
//  Received a drag notification.
// 
(
 MousePos mousePos // The mouse pos.
)

//////////////////////////////////////////////////////////////////////////////////////////
{
    if( IsMouseInScrollBar( mousePos ) ){
        return true;
    }
    
    GameUI *item = GetItemFromMousePos( mousePos );

    selection = item;
    selectedColumn = GetColumnFromMousePos( mousePos );
    selectedRow    = GetRowFromMousePos( mousePos );

    if( item != NULL )
    {        
        listEvent.LeftClicked();
        iSpinCnt++;
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
const ListUI::GridItem ListUI::GetSelectedRowItem
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the item in the specified column of the currently selected row.
// 
(
 int column // The column
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    if( selectedRow == -1 ){
        return NULL;
    }

    return itemGrid[ column ][ selectedRow ];
}

//////////////////////////////////////////////////////////////////////////////////////////
const ListUI::GridItem ListUI::GetRowItem
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the item in the specified column of the currently selected row.
// 
(
 int column, // The column
 int row    // The Row
 )
 //////////////////////////////////////////////////////////////////////////////////////////
{
   if( row < 0 || row >= (int)itemGrid[ column ].size())
   {
      return NULL;
   }

   if( column < 0 || column >= (int)itemGrid.size())
   {
      return NULL;
   }
   
   return itemGrid[ column ][ row ];
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::DeleteSelection( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Delete the current list selection.
// 
//////////////////////////////////////////////////////////////////////////////////////////
{    
    // Find the selected iterator
    // Grow all columns to fit the new row.
    UINT z;
    for( z = 0; z < itemGrid.size(); z++ ){
        int theRow = 0;
        vector< GridItem >::iterator i;
        for( i = itemGrid[ z ].begin(); i != itemGrid[ z ].end(); i++ ){        
            if( theRow == selectedRow ){
                delete (*i);
                itemGrid[ z ].erase( i );
                break;
            }
            theRow++;
        }
    }
    selection = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListUI::DrawHelp
//////////////////////////////////////////////////////////////////////////////////////////
//  Draws the help of all list items
// 
(
 V2SPRITEFX *vsfFX
)
//////////////////////////////////////////////////////////////////////////////////////////
{
    UINT j;    
    for( j = 0; j < itemGrid[ 0 ].size(); j++ ){
        GameUI *targetUI = itemGrid[ 0 ][ j ];
        if( targetUI != NULL ){
            targetUI->DrawHelp( vsfFX );
        }
    }
}