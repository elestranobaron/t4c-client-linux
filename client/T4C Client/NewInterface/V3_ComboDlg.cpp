// V3_ComboDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "V3_ComboDlg.h"
#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\App.h"

#include "V3_StatDlg.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

V3_ComboDlg::V3_ComboDlg():
cbList ( cbListEvent  ),
cbXButton (xEvent),
m_pParent (NULL)
{
   int i=0;

   m_iXPos       = 0;
   m_iYPos       = 0;
   m_uiShowCodeP = 0;
  

   m_BackgroundS.SetSpriteId("V3_ComboBack",true,"V3_ComboBackMask");

   cbList.SetupList    ( 1, 20, FramePos( 0, 0, 0, 0 ) );
   cbList.SetColumnSize( 0, 155  );
   cbList.CenterColumn( 0 );
   cbList.SetScrollUpDownUI( &cbScrollUp );
   cbList.SetScrollDownDownUI( &cbScrollDown );
   
   cbListSelect.SetPos( FramePos( 0, 0, 156, 20 ) );
   cbListSelect.SetSpriteId( "V3_ComboListSelect" );
   cbListSelect.Show();
   cbList.SetSelectionSprite( &cbListSelect);
   cbListEvent.SetUI( this );

   
   
   cbXButton.SetSpriteId   ( "V3_CheckX18_ON_N" );
   cbXButton.SetDownUI     ( "V3_CheckX18_ON_N" );
   cbXButton.SetHighlightUI( "V3_CheckX18_ON_H" ); 
   xEvent.SetUI    ( this );


   UpdateAllFramePos(m_iXPos,m_iYPos);
  
   AddChild( &m_BackgroundS ); 
   AddChild( &cbList);
   AddChild( &cbXButton);
  
   RootBoxUI::GetInstance()->AddChild( this );

} 

V3_ComboDlg::~V3_ComboDlg()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
V3_ComboDlg *V3_ComboDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_ComboDlg instance;
    return &instance;
    
} 

void V3_ComboDlg::SetXYPos(int iX,int iY)
{
   m_iXPos = iX;
   m_iYPos = iY;
}

void V3_ComboDlg::ClearCombo()
{
   cbList.DeleteList();
}

void V3_ComboDlg::AddComboItem(std::string strName)
{
   cbList.AddNewItem();
   ComboDlgUI *cbUI = new ComboDlgUI;
   
   cbUI->SetText( strName, g_DefColorD, 640, 0, 0 );
   cbUI->SetCenter( false );
   cbUI->SetPos( FramePos( 0, 0, cbUI->GetWidth(), cbUI->GetHeight() ) );
   cbUI->strName = strName;
   cbList.AddColumnItem( 0, cbUI );
}

void V3_ComboDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_ComboDlgW, iPosY+g_ComboDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX    , iPosY     , iPosX+g_ComboDlgW   , iPosY+g_ComboDlgH    ) );

   cbList   .SetListDef(iPosX,iPosY,20,0 ,0,g_ComboDlgW,g_ComboDlgH,&cbScrollUp ,&cbScrollDown); 
   //refake le scroll UP car il new pas en position normale...
   int iSXS = g_ComboDlgW-20;
   cbList.SetScrollRegions( FramePos( iPosX+iSXS, iPosY+20   , iPosX+g_ComboDlgW, iPosY+20+20   ), 
                            FramePos( iPosX+iSXS, iPosY+g_ComboDlgH-20, iPosX+g_ComboDlgW, iPosY+g_ComboDlgH         ), 
                            FramePos( iPosX+iSXS, iPosY+20+20, iPosX+g_ComboDlgW, iPosY+g_ComboDlgH-20-11)   ); //11 == 1/2 du bmp qui scroll
   cbScrollUp.SetPos  (   FramePos( iPosX+155,iPosY+20, iPosX+155+20,iPosY+20+20));



   cbXButton.SetPos( FramePos( iPosX+156, iPosY+1, iPosX+156+18   , iPosY+1+18) );
   
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_ComboDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

   //backGame.Draw(vsfFX);
   GameUI::Draw( vsfFX );
}


void V3_ComboDlg::Hide()
{
   BoxUI::Hide();
}

void V3_ComboDlg::Show(UINT uiShowCodeP,int iScrollPos)
{
   m_uiShowCodeP = uiShowCodeP;
   cbList.SetScrollPos(iScrollPos);
   UpdateAllFramePos(m_iXPos,m_iYPos);
   BoxUI::Show();
}



void V3_ComboDlg::CbListEvent::LeftClicked( void )
{
   
   V3_ComboDlg *me = static_cast< V3_ComboDlg * >( GetUI() );
   
   int col = me->cbList.GetSelectedColumn();
   
   string channelId;
   
   // Click in names listing
   if( col == 0 )
   {
      ComboDlgUI *optUI = static_cast< ComboDlgUI * >( me->cbList.GetSelection() );
      
      if( optUI == NULL )
         return;

     
      if(me->m_pParent)
         me->m_pParent->ComboReturn(me->m_uiShowCodeP,(char*)optUI->strName.c_str());
      
      me->Hide();
   }
}

void V3_ComboDlg::XEvent::LeftClicked( void )
{
   V3_ComboDlg *me = static_cast< V3_ComboDlg * >( GetUI() );
   
   me->Hide();
}