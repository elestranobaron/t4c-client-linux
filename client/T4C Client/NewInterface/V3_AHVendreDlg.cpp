// V3_AHVendreDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "V3_AHVendreDlg.h"
#include "V3_ChatLogDlg.h"

extern CSaveGame g_SaveGame; 

#define V2IMAGE_GMNMS_DATA       "\\Game Files\\gmnmb.bin"
 
namespace 
{
    //  Drops back the given item UI in its original ui.
    bool DropBack(GameUI *itemUI)
    {
       GameUI *src = GameUI::GetDragParentUI();
       if( src != NULL )
       {
          // If the source is a grid UI
          if( typeid( *src ) == typeid( GridUI ) )
          {
             // Try to see if the grid has space left.
             GridUI *srcGrid = static_cast< GridUI * >( src );
             
             // If the grid spot isn't taken
             if( srcGrid->CanDrop( GameUI::GetDragInitialPos() ) )
             {
                // Redrop the item
                src->LeftMouseDown( GameUI::GetDragInitialPos() );
             }
             else
             {
                // Otherwise add the item.
                srcGrid->AddItem( itemUI );
                GameUI::CancelDrag();
                CMouseCursor::GetInstance()->SetCustomCursor( NULL );
             }
             
          }
          else
          {
             // Drop the item where it was first fetched.
             src->LeftMouseDown( GameUI::GetDragInitialPos() );
          }
          return true;
       }
       return false;
    };
    
} 
 


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_AHVendreDlg::V3_AHVendreDlg():
X_Button( hideEvent ),
m_btnPutItemHere(putItemHereEvent),
inventoryGrid( &(V3_InvDlg::GetInstance()->inventoryGrid) ),
invGridScrollDownGraph( &(V3_InvDlg::GetInstance()->gridScrollDownGraph) ),
invGridScrollUpGraph( &(V3_InvDlg::GetInstance()->gridScrollUpGraph) ),
iconGold( &(V3_InvDlg::GetInstance()->iconGold) ),
statGold( &(V3_InvDlg::GetInstance()->statGold) ),
instantItemInfo( &(V3_InvDlg::GetInstance()->instantItemInfo) ),
okButton( okButtonEvent,g_GUILocalString[292],g_DefColorH ),
cancelButton( cancelButtonEvent,g_GUILocalString[293],g_DefColorH ),
QtySlider( sliderQtyEvent ),
DelSlider( sliderDelEvent )
{
   int i=0;

   m_bNMSOptions = FALSE;
   char strPathFile[1024];
   ClientConfig *pConfig = g_Global.GetClientConfig();
   sprintf_s(strPathFile,1024,"%s%s",pConfig->strPath,V2IMAGE_GMNMS_DATA);
   FILE *pfGMD = NULL;
   fopen_s(&pfGMD,strPathFile,"rb");
   if(pfGMD)
   {
      fclose(pfGMD);
      m_bNMSOptions = TRUE;
   }

   m_iXPos = (g_Global.GetScreenW()-g_AHVDlgW)/2;
   m_iYPos = ((g_Global.GetScreenH()-g_AHVDlgH)/2)-(g_MainBarDlgH/2);
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_AH2VendreBack",true,"V3_AH2VendreBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_AHIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

     
   okButton.SetSpriteId   ( "V3_Btn72x24_N" );
   okButton.SetDownUI     ( "V3_Btn72x24_D" );
   okButton.SetHighlightUI( "V3_Btn72x24_H" );
   okButtonEvent.SetUI( this );
   okButton.Hide();
   SetHelpId( okButton, 641 );
   
   cancelButton.SetSpriteId   ( "V3_Btn72x24_N" );
   cancelButton.SetDownUI     ( "V3_Btn72x24_D" );
   cancelButton.SetHighlightUI( "V3_Btn72x24_H" );
   cancelButtonEvent.SetUI( this );
   SetHelpId( cancelButton, 642 );

   putItemHereEvent.SetUI( this );
   

   sliderQtyEvent.SetUI( this );
   sliderDelEvent.SetUI( this );
   textAreaQty .SetText( g_GUILocalString[288], g_DefColorH, 247 );
   textAreaQty .SetCenter( true );
   textAreaBuy .SetText( g_GUILocalString[289], g_DefColorH, 247 );
   textAreaBuy .SetCenter( true );
   textAreaBid .SetText( g_GUILocalString[290], g_DefColorH, 247 );
   textAreaBid .SetCenter( true );
   textAreaDel .SetText( g_GUILocalString[291], g_DefColorH, 247 );
   textAreaDel .SetCenter( true );

   textAreaBuyOr .SetText( g_GUILocalString[24], g_DefColorH, 247 );
   textAreaBuyOr .SetCenter( true );
   textAreaBuyEcu.SetText( g_GUILocalString[387], g_DefColorH, 247 );
   textAreaBuyEcu.SetCenter( true );



   QtyEdit.SetFilter( &amountEditFilter );
   QtyEdit.SetMaxTextLength( 9 );
   QtyEdit.EnableCursor( true );
   BuyEdit.SetFilter( &amountEditFilter );
   BuyEdit.SetMaxTextLength( 9 );
   BuyEdit.EnableCursor( true );
   BuyNMSEdit.SetFilter( &amountEditFilter );
   BuyNMSEdit.SetMaxTextLength( 4 );
   BuyNMSEdit.EnableCursor( true );
   BidEdit.SetFilter( &amountEditFilter );
   BidEdit.SetMaxTextLength( 9 );
   BidEdit.EnableCursor( true );
   DelEdit.SetFilter( &amountEditFilter );
   DelEdit.SetMaxTextLength( 1 );
   DelEdit.EnableCursor( true );
  
  
   m_stPutItemHere.SetText("",g_DefColorD);
   m_stPutItemHere.SetCenter( true );

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_stPutItemHere);
   AddChild( &m_btnPutItemHere);

   AddChild( &okButton);
   AddChild( &cancelButton);
   AddChild( &QtySlider);
   AddChild( &DelSlider);
   AddChild( &textAreaQty);
   AddChild( &textAreaBuy);
   AddChild( &textAreaBuyOr);
   
   AddChild( &textAreaBid);
   AddChild( &textAreaDel);
   AddChild( &QtyEdit);
   AddChild( &BuyEdit);
   if(m_bNMSOptions)
   {
      AddChild( &BuyNMSEdit);
      AddChild( &textAreaBuyEcu);
   }
   AddChild( &BidEdit);
   AddChild( &DelEdit);
   
   AddChild( inventoryGrid );
   AddChild( iconGold );
	AddChild( statGold );
   

   lastClickedItem = &QtyEdit;

  
   RootBoxUI::GetInstance()->AddChild( this );


} 

V3_AHVendreDlg::~V3_AHVendreDlg()
{
   }

//////////////////////////////////////////////////////////////////////////////////////////
V3_AHVendreDlg *V3_AHVendreDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_AHVendreDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_AHVendreDlg::Display( void *null)
{
   // Lock the root box since this call came from some unknown thread.
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );
   

   // If inventory is already shown.
   if( GetInstance()->IsShown())
   {
      // Hide box
      GetInstance()->Hide();
   }
   else
   {
      GetInstance()->m_iXPos = (g_Global.GetScreenW()-g_AHVDlgW)/2;
      GetInstance()->m_iYPos = ((g_Global.GetScreenH()-g_AHVDlgH)/2)-(g_MainBarDlgH/2);
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      GetInstance()->m_stPutItemHere.SetText(g_GUILocalString[345],g_DefColorD);

      GetInstance()->Show();
   }
   return 0;
}

void V3_AHVendreDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_AHVDlgW, iPosY+g_AHVDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_AHVDlgW, iPosY+g_AHVDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+554, iPosY+27, iPosX+554+27, iPosY+27+24 ) );
   m_btnPutItemHere.SetPos ( FramePos( iPosX+40, iPosY+68 , iPosX+288, iPosY+95) );
   m_stPutItemHere .SetPos ( FramePos( iPosX+41, iPosY+69 , iPosX+287, iPosY+94) );

   okButton    .SetPos( FramePos( iPosX+88 , iPosY+378 , iPosX+88 +72 ,iPosY+378+24));
   cancelButton.SetPos( FramePos( iPosX+170, iPosY+378 , iPosX+170+72 ,iPosY+378+24));
   QtySlider   .SetSliderDef(0,10,iPosX,iPosY,20,40,139,173,158);
   DelSlider   .SetSliderDef(0,10,iPosX,iPosY,20,40,317,173,336);

   textAreaQty.SetPos ( FramePos( iPosX+41, iPosY+118 , iPosX+287, iPosY+135) );
   textAreaBuy.SetPos ( FramePos( iPosX+41, iPosY+177 , iPosX+287, iPosY+194) );
   textAreaBid.SetPos ( FramePos( iPosX+41, iPosY+247 , iPosX+287, iPosY+264) );
   textAreaDel.SetPos ( FramePos( iPosX+41, iPosY+296 , iPosX+287, iPosY+313) );

   textAreaBuyOr.SetPos ( FramePos( iPosX+41, iPosY+199 , iPosX+154, iPosY+216) );

   if(m_bNMSOptions)
      textAreaBuyEcu.SetPos ( FramePos( iPosX+41, iPosY+220 , iPosX+154, iPosY+237) );

   QtyEdit.SetPos ( FramePos( iPosX+180, iPosY+140 , iPosX+287, iPosY+157) );
   BuyEdit.SetPos ( FramePos( iPosX+180, iPosY+199 , iPosX+287, iPosY+216) );
   BuyNMSEdit.SetPos ( FramePos( iPosX+180, iPosY+220 , iPosX+287, iPosY+237) );
   BidEdit.SetPos ( FramePos( iPosX+180, iPosY+269 , iPosX+287, iPosY+286) );
   DelEdit.SetPos ( FramePos( iPosX+180, iPosY+318 , iPosX+287, iPosY+335) );


   statGold->SetPos ( FramePos( iPosX+448, iPosY+343 , iPosX+548, iPosY+363) );
   iconGold->SetPos ( FramePos( iPosX+543, iPosY+342 , iPosX+567, iPosY+364) );
   instantItemInfo->SetPos( FramePos( iPosX+306, iPosY+69, iPosX+540, iPosY+94 ) );
   instantItemInfo->SetCenter( false );
   inventoryGrid  ->SetGridDef(iPosX,iPosY,20,309 ,103,567,334,&(V3_InvDlg::GetInstance()->gridScrollUpGraph) ,&(V3_InvDlg::GetInstance()->gridScrollDownGraph) );
}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_AHVendreDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

   

   GameUI::Draw( vsfFX );

   HDC hdc;
   DXDGetDC(&hdc, 86);

   okButton.DrawTextButtonExt(hdc);
   cancelButton.DrawTextButtonExt(hdc);
   
   // Draw les texte...
   char strTitle[100];
   sprintf_s(strTitle,100  ,"%s - %s",g_GUILocalString[268],g_GUILocalString[284]);

   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc, g_DefColorH);
   HANDLE  hOldSurfaceFont;
   hOldSurfaceFont = SelectObject(hdc, FontManager::GetInstance()->GetGUIHFontHandle());

   //Draw All header
   TextOut(hdc,iOffX+68, iOffY+30,strTitle,strlen(strTitle));
   //reset Text
   SelectObject(hdc, hOldSurfaceFont);
   DXDReleaseDC(hdc, 86);

   if( GetLastClickedItem() == &QtyEdit )
   {
      QtyEdit.EnableCursor( true );
      BuyEdit.EnableCursor( false );
      BuyNMSEdit.EnableCursor( false );
      BidEdit.EnableCursor( false );
      DelEdit.EnableCursor( false );
   }
   else if( GetLastClickedItem() == &BuyEdit )
   {
      QtyEdit.EnableCursor( false );
      BuyEdit.EnableCursor( true );
      BuyNMSEdit.EnableCursor( false );
      BidEdit.EnableCursor( false );
      DelEdit.EnableCursor( false );
   }
   else if( GetLastClickedItem() == &BuyNMSEdit )
   {
      QtyEdit.EnableCursor( false );
      BuyEdit.EnableCursor( false );
      BuyNMSEdit.EnableCursor( true );
      BidEdit.EnableCursor( false );
      DelEdit.EnableCursor( false );
   }
   else if( GetLastClickedItem() == &BidEdit )
   {
      QtyEdit.EnableCursor( false );
      BuyEdit.EnableCursor( false );
      BuyNMSEdit.EnableCursor( false );
      BidEdit.EnableCursor( true );
      DelEdit.EnableCursor( false );
   }
   else if( GetLastClickedItem() == &DelEdit )
   {
      QtyEdit.EnableCursor( false );
      BuyEdit.EnableCursor( false );
      BuyNMSEdit.EnableCursor( false );
      BidEdit.EnableCursor( false );
      DelEdit.EnableCursor( true );
   }

   // Get the current mouse coordinates.
   MousePos mousePos( MouseDevice::xPos, MouseDevice::yPos );
   
   ItemUI * itemUI = NULL;
   if( inventoryGrid->GetPos().MousePosInFrame( mousePos ) )
      itemUI = static_cast< ItemUI * >( inventoryGrid->GetGridItemAt( mousePos ) );
   
   // Display the instant help text.
   if( itemUI != NULL )
   {
      //TextObject *to = itemUI->GetHelpText();
      LPBAG_ITEM bagItem = itemUI->bagItem;
      
      // If the item doesn't have a name.
      if(/* to == NULL && */bagItem->chIName[ 0 ] == 0 )
      {
         // Send a query item's name.
         TFCPacket sending;
         sending << (RQ_SIZE)RQ_QueryItemName;
         sending << (char)PL_SEARCHBACKPACK;
         sending << (long)bagItem->dwID;
         SEND_PACKET( sending );
         
         // Avoid the drawing thread to send continuous QueryItemName packets.
         bagItem->chIName[ 0 ] = ' ';
         bagItem->chIName[ 1 ] = 0;
      }
      else
      {
         //if( to != NULL )
         {
            //instantItemInfo->SetText(to->GetText(),CL_ORANGE,246);
            instantItemInfo->SetText(itemUI->strName2,CL_ORANGE,246);
            instantItemInfo->Draw( vsfFX );
         }
      }        
   }

}

void V3_AHVendreDlg::Hide()
{
   BoxUI::Hide();
}



void V3_AHVendreDlg::PutItemHereEvent::LeftClicked( void )
{
   CAutoLock theLock( RootBoxUI::GetInstance()->GetLock() );

   V3_AHVendreDlg *me = static_cast< V3_AHVendreDlg * >( GetUI() );
   
   ItemUI *itemUI = static_cast< ItemUI * >( GetDragItem() );
   if( itemUI == NULL )
   {
      return;
   }
   
   if( itemUI->bagItem->dwQty == 0 )
   {
      return;
   }
   
   

   me->m_stPutItemHere.SetText((char*)itemUI->bagItem->chIName,g_DefColorD);

   //Setup la partie de gauche pour enabler la vente...
   me->okButton.Show();

   me->QtySlider   .SetSliderDef(1,itemUI->bagItem->dwQty,me->m_iXPos,me->m_iYPos,20,40,139,173,158);
   me->DelSlider   .SetSliderDef(1,5,me->m_iXPos,me->m_iYPos,20,40,317,173,336);

   me->QtySlider.SetSliderPos( 1 );
   me->DelSlider.SetSliderPos( 1 );

   me->m_iLastItemID  = itemUI->bagItem->dwID;

   DropBack( GetDragItem() );
   
   
   char strQty[10];
   char strBuy[10];
   char strBuyNMS[10];
   char strBid[10];
   char strDel[10];

   sprintf_s(strQty,10,"%d",1);
   sprintf_s(strBuy,10,"%d",0);
   sprintf_s(strBuyNMS,10,"%d",0);
   sprintf_s(strBid,10,"%d",0);
   sprintf_s(strDel,10,"%d",1);

   me->QtyEdit.SetText( strQty );
   me->BuyEdit.SetText( strBuy );
   me->BuyNMSEdit.SetText( strBuyNMS );
   me->BidEdit.SetText( strBid );
   me->DelEdit.SetText( strDel );

   me->lastClickedItem = &me->QtyEdit;
 }


//////////////////////////////////////////////////////////////////////////////////////////
bool V3_AHVendreDlg::VKeyInput(VKey ivkey) 
{
   int dwOffX = (g_Global.GetScreenW()-272)/2;
   int dwOffY = (g_Global.GetScreenH()-400-150)/2;
   
   if (ivkey == VK_RETURN) 
   {
      okButtonEvent.LeftClicked();
   } 
   else if (ivkey == VK_ESCAPE) 
   {
      cancelButtonEvent.LeftClicked();
   } 
   
   else if (ivkey == VK_LEFT) 
   {
      if(GetLastClickedItem() == &QtyEdit)
      {
         MousePos X(dwOffX+35,dwOffY+67);
         QtySlider.LeftMouseDown(X);
         QtySlider.LeftMouseUp(X);
      }
      else if(GetLastClickedItem() == &DelEdit)
      {
         MousePos X(dwOffX+35,dwOffY+286);
         DelSlider.LeftMouseDown(X);
         DelSlider.LeftMouseUp(X);
      }
   } 
   else if (ivkey == VK_RIGHT) 
   {
      if(GetLastClickedItem() == &QtyEdit)
      {
         MousePos X(dwOffX+123,dwOffY+67);
         QtySlider.LeftMouseDown(X);
         QtySlider.LeftMouseUp(X);
      }
      else if(GetLastClickedItem() == &DelEdit)
      {
         MousePos X(dwOffX+123,dwOffY+286);
         DelSlider.LeftMouseDown(X);
         DelSlider.LeftMouseUp(X);
      }
   }
   
   return true;
}

// Keyboard hit in the join popup UI.
bool V3_AHVendreDlg::TextInput(char ch)
{
   // Enter pressed.        
   if( ch == '\r' )
   {
      okButtonEvent.LeftClicked();
   }
   // Tab pressed
   else if( ch == '\t' ) 
   {
      if( GetLastClickedItem() == &QtyEdit )
      {
         lastClickedItem = &BuyEdit;
      }
      else if( GetLastClickedItem() == &BuyEdit )
      {
         if(m_bNMSOptions)
            lastClickedItem = &BuyNMSEdit;
         else
            lastClickedItem = &BidEdit;
      }
      else if( GetLastClickedItem() == &BuyNMSEdit )
      {
         lastClickedItem = &BidEdit;
      }
      else if( GetLastClickedItem() == &BidEdit )
      {
         lastClickedItem = &DelEdit;
      }
      else if( GetLastClickedItem() == &DelEdit )
      {
         lastClickedItem = &QtyEdit;
      }
      
   }
   else if( ch != 0 )
   {
      // Give the kbhit event to the last clicked control.
      if( GetLastClickedItem() != NULL )
      {
         GetLastClickedItem()->TextInput( ch );
      }
   }
   
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_AHVendreDlg::OkButtonEvent::LeftClicked( void )
{
   V3_AHVendreDlg *me = static_cast< V3_AHVendreDlg * >( GetUI() );
   
   TFCPacket sending;
   DWORD qty   = atoi( me->QtyEdit.GetText().c_str() );
   DWORD buy   = atoi( me->BuyEdit.GetText().c_str() );
   DWORD buyNMS= atoi( me->BuyNMSEdit.GetText().c_str() );
   DWORD bid   = atoi( me->BidEdit.GetText().c_str() );
   DWORD delay = atoi( me->DelEdit.GetText().c_str() );
   
   sending << (RQ_SIZE)RQ_NM_AddAHItems;
   sending << (long)me->m_iLastItemID;
   sending << (long)qty;
   sending << (long)buy;
   sending << (long)buyNMS;
   sending << (long)bid;
   sending << (long)delay;
   // Send the packet.
   SEND_PACKET( sending );
   me->Hide();
}

//////////////////////////////////////////////////////////////////////////////////////////
void V3_AHVendreDlg::CancelButtonEvent::LeftClicked( void )
{
   V3_AHVendreDlg *me = static_cast< V3_AHVendreDlg * >( GetUI() );
   me->Hide();    
}

//////////////////////////////////////////////////////////////////////////////////////////

void V3_AHVendreDlg::SliderQtyEvent::LeftClicked( void )
{
   V3_AHVendreDlg *me = static_cast< V3_AHVendreDlg * >( GetUI() );
   
   char buf[ 256 ];
   _itoa_s( me->QtySlider.GetSliderPos(), buf,256, 10 );
   me->QtyEdit.SetText(buf);
}

void V3_AHVendreDlg::SliderDelEvent::LeftClicked( void )
{
   V3_AHVendreDlg *me = static_cast< V3_AHVendreDlg * >( GetUI() );
   
   char buf[ 256 ];
   _itoa_s( me->DelSlider.GetSliderPos(), buf,256, 10 );
   me->DelEdit.SetText(buf);
}


//////////////////////////////////////////////////////////////////////////////////////////
bool V3_AHVendreDlg::AmountEditFilter::IsOK(char nextCh,const std::string curText)
{
   return isdigit( nextCh ) != 0 ? true : false;
}
