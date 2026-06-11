// V3_ItemInfoDlg.cpp:
//
//////////////////////////////////////////////////////////////////////
#include "../pch.h"

#include "RootBoxUI.h"
#include "..\GUILocalString.h"  //g_GUILocalString[]
#include "..\SaveGame.h"
#include "..\App.h"
#include "V3_ItemInfoDlg.h"
#include "V3_ChatLogDlg.h"


extern Font *fSmallNormal_14;

extern DWORD TargetID;
extern CSaveGame g_SaveGame;


extern int TalkToX, TalkToY, TalkToW;
extern DWORD TalkToID;


#define STAT_SKILL_INVALID		0
#define STAT_INT			1
#define STAT_END			2
#define STAT_STR			3
#define STAT_WIS			4
#define STAT_LUCK			5
#define STAT_AGI			6
#define STAT_ATTACK			8
#define STAT_DODGE			9
#define STAT_DAMAGE			10
#define STAT_RADIANCE       		11
#define STAT_AIR_RESIST     		12
#define STAT_FIRE_RESIST    		13
#define STAT_WATER_RESIST   		14
#define STAT_EARTH_RESIST   		15
#define STAT_AIR_POWER      		16
#define STAT_FIRE_POWER     		17
#define STAT_WATER_POWER    		18
#define STAT_EARTH_POWER    		19
#define STAT_AC             		20
#define STAT_LIGHT_RESIST   		21
#define STAT_DARK_RESIST    		22
#define STAT_LIGHT_POWER    		23
#define STAT_DARK_POWER     		24
#define STAT_MAX_HP         		25
#define STAT_MAX_MANA       		26
#define SKILL_STUN_BLOW     		27
#define SKILL_POWERUL_BLOW		28
#define SKILL_FIRST_AID			29
#define SKILL_PARRY			30
#define SKILL_MEDITATE			31
#define SKILL_DODGE			32
#define SKILL_ATTACK			33
#define SKILL_HIDE			34
#define SKILL_ROB			35
#define SKILL_SNEAK			36
#define SKILL_SEARCH			37
#define SKILL_PICKLOCK			38
#define SKILL_ARMOR_PENETRATION		39
#define SKILL_PEEK			40
#define SKILL_RAPID_HEALING		41
#define SKILL_ARCHERY			42
#define SKILL_DUALWEAPONS			43
#define SKILL_PLUNDER		   	44
#define SKILL_RESURECT		   	45
#define SKILL_POWERCONJURING	  	46
#define SKILL_PRIMALSCREAM	   	47
#define SKILL_IMMOBILISATION  	48
#define SKILL_CRITICAL_STRIKE    49


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//EmptyEvent::GetInstance()

V3_ItemInfoDlg::V3_ItemInfoDlg():
X_Button( hideEvent ),
Drag_Button(dragEvent),
m_bDragDialog (false)

{
   int i=0;

   m_bDragDialog = false;
   m_iXPos = g_Global.GetDialogLastIFXPos();
   m_iYPos = g_Global.GetDialogLastIFYPos();
   
   SetHelpId( this, 297 );
  
   m_BackgroundS.SetSpriteId("V3_ItemInfoDlgBack",true,"V3_ItemInfoDlgBackMask");

   //Title Icons   
   m_TitleIcon .SetSpriteId( "V3_AHIcon");

   //Buttons
   X_Button.SetSpriteId   ( "V3_BtnX_N" );
   X_Button.SetDownUI     ( "V3_BtnX_N" );
   X_Button.SetHighlightUI( "V3_BtnX_H" ); 

   Drag_Button.SetSpriteId   ( "" );
   Drag_Button.SetDownUI     ( "" );
   Drag_Button.SetHighlightUI( "" ); 
   dragEvent.SetUI    ( this );
   
   
   m_stNameItemInfo.SetCenter( true );
   //m_stNameItemInfo.AutoTruncate(285);

     

   
  

   UpdateAllFramePos(m_iXPos,m_iYPos);

 
  
   AddChild( &m_BackgroundS); 
   AddChild( &m_TitleIcon ); 
   AddChild( &X_Button);
   AddChild( &m_stNameItemInfo);


   AddChild( &Drag_Button);
  
   RootBoxUI::GetInstance()->AddChild( this );

   sprintf_s(m_strBoustName[STAT_SKILL_INVALID     ],100,"%s",g_GUILocalString[175]);
   sprintf_s(m_strBoustName[STAT_INT	      		],100,"%s",g_GUILocalString[176]);
   sprintf_s(m_strBoustName[STAT_END	        		],100,"%s",g_GUILocalString[177]);
   sprintf_s(m_strBoustName[STAT_STR			      ],100,"%s",g_GUILocalString[178]);
   sprintf_s(m_strBoustName[STAT_WIS	      	   ],100,"%s",g_GUILocalString[179]);
   sprintf_s(m_strBoustName[STAT_LUCK	      	   ],100,"%s",g_GUILocalString[346]);
   sprintf_s(m_strBoustName[STAT_AGI	      	   ],100,"%s",g_GUILocalString[180]);
   sprintf_s(m_strBoustName[STAT_ATTACK            ],100,"%s",g_GUILocalString[181]);
   sprintf_s(m_strBoustName[STAT_DODGE			      ],100,"%s",g_GUILocalString[182]);
   sprintf_s(m_strBoustName[STAT_DAMAGE			   ],100,"%s",g_GUILocalString[183]);
   sprintf_s(m_strBoustName[STAT_RADIANCE         	],100,"%s",g_GUILocalString[184]);
   sprintf_s(m_strBoustName[STAT_AIR_RESIST       	],100,"%s",g_GUILocalString[185]);
   sprintf_s(m_strBoustName[STAT_FIRE_RESIST      	],100,"%s",g_GUILocalString[186]);
   sprintf_s(m_strBoustName[STAT_WATER_RESIST     	],100,"%s",g_GUILocalString[187]);
   sprintf_s(m_strBoustName[STAT_EARTH_RESIST     	],100,"%s",g_GUILocalString[188]);
   sprintf_s(m_strBoustName[STAT_AIR_POWER        	],100,"%s",g_GUILocalString[189]);
   sprintf_s(m_strBoustName[STAT_FIRE_POWER       	],100,"%s",g_GUILocalString[190]);
   sprintf_s(m_strBoustName[STAT_WATER_POWER      	],100,"%s",g_GUILocalString[191]);
   sprintf_s(m_strBoustName[STAT_EARTH_POWER      	],100,"%s",g_GUILocalString[192]);
   sprintf_s(m_strBoustName[STAT_AC               	],100,"%s",g_GUILocalString[193]);
   sprintf_s(m_strBoustName[STAT_LIGHT_RESIST     	],100,"%s",g_GUILocalString[194]);
   sprintf_s(m_strBoustName[STAT_DARK_RESIST      	],100,"%s",g_GUILocalString[195]);
   sprintf_s(m_strBoustName[STAT_LIGHT_POWER      	],100,"%s",g_GUILocalString[196]);
   sprintf_s(m_strBoustName[STAT_DARK_POWER       	],100,"%s",g_GUILocalString[197]);
   sprintf_s(m_strBoustName[STAT_MAX_HP           	],100,"%s",g_GUILocalString[198]);
   sprintf_s(m_strBoustName[STAT_MAX_MANA         	],100,"%s",g_GUILocalString[199]);
   sprintf_s(m_strBoustName[SKILL_STUN_BLOW       	],100,"%s",g_GUILocalString[200]);
   sprintf_s(m_strBoustName[SKILL_POWERUL_BLOW	   ],100,"%s",g_GUILocalString[201]);
   sprintf_s(m_strBoustName[SKILL_FIRST_AID		   ],100,"%s",g_GUILocalString[202]);
   sprintf_s(m_strBoustName[SKILL_PARRY			   ],100,"%s",g_GUILocalString[203]);
   sprintf_s(m_strBoustName[SKILL_MEDITATE		   ],100,"%s",g_GUILocalString[204]);
   sprintf_s(m_strBoustName[SKILL_DODGE			   ],100,"%s",g_GUILocalString[205]);
   sprintf_s(m_strBoustName[SKILL_ATTACK			   ],100,"%s",g_GUILocalString[206]);
   sprintf_s(m_strBoustName[SKILL_HIDE			      ],100,"%s",g_GUILocalString[207]);
   sprintf_s(m_strBoustName[SKILL_ROB			      ],100,"%s",g_GUILocalString[208]);
   sprintf_s(m_strBoustName[SKILL_SNEAK			   ],100,"%s",g_GUILocalString[209]);
   sprintf_s(m_strBoustName[SKILL_SEARCH			   ],100,"%s",g_GUILocalString[210]);
   sprintf_s(m_strBoustName[SKILL_PICKLOCK		   ],100,"%s",g_GUILocalString[211]);
   sprintf_s(m_strBoustName[SKILL_ARMOR_PENETRATION],100,"%s",g_GUILocalString[212]);
   sprintf_s(m_strBoustName[SKILL_PEEK			      ],100,"%s",g_GUILocalString[213]);
   sprintf_s(m_strBoustName[SKILL_RAPID_HEALING	   ],100,"%s",g_GUILocalString[214]);
   sprintf_s(m_strBoustName[SKILL_ARCHERY			   ],100,"%s",g_GUILocalString[215]);
   sprintf_s(m_strBoustName[SKILL_DUALWEAPONS  	   ],100,"%s",g_GUILocalString[216]);
   sprintf_s(m_strBoustName[SKILL_PLUNDER      	   ],100,"%s",g_GUILocalString[218]);
   sprintf_s(m_strBoustName[SKILL_RESURECT      	],100,"%s",g_GUILocalString[219]);
   sprintf_s(m_strBoustName[SKILL_POWERCONJURING   ],100,"%s",g_GUILocalString[356]);
   sprintf_s(m_strBoustName[SKILL_PRIMALSCREAM     ],100,"%s",g_GUILocalString[358]);
   sprintf_s(m_strBoustName[SKILL_IMMOBILISATION   ],100,"%s",g_GUILocalString[357]);
   sprintf_s(m_strBoustName[SKILL_CRITICAL_STRIKE  ],100,"%s",g_GUILocalString[359]);
} 

V3_ItemInfoDlg::~V3_ItemInfoDlg()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
V3_ItemInfoDlg *V3_ItemInfoDlg::GetInstance( void )
//////////////////////////////////////////////////////////////////////////////////////////
//  Returns the side menu instance.
//////////////////////////////////////////////////////////////////////////////////////////
{

    static V3_ItemInfoDlg instance;
    return &instance;
    
} 

//////////////////////////////////////////////////////////////////////////////////////////
//  Callback displaying the inventory macro.
// 
// Return: unsigned int, 0
//////////////////////////////////////////////////////////////////////////////////////////
unsigned int V3_ItemInfoDlg::Display( void *null)
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
      GetInstance()->m_iXPos = g_Global.GetDialogLastIFXPos();
      GetInstance()->m_iYPos = g_Global.GetDialogLastIFYPos();
      GetInstance()->UpdateAllFramePos(GetInstance()->m_iXPos,GetInstance()->m_iYPos);

      GetInstance()->Show();
   }
   return 0;
}

void V3_ItemInfoDlg::UpdateAllFramePos(int iPosX,int iPosY)
{
   SetPos( FramePos( iPosX, iPosY, iPosX+g_IFDlgW, iPosY+g_IFDlgH ) );
   m_BackgroundS.SetPos( FramePos( iPosX, iPosY, iPosX+g_IFDlgW, iPosY+g_IFDlgH ) );
   m_TitleIcon.SetPos( FramePos( iPosX+27, iPosY+27 , iPosX+27+24 ,iPosY+27+24));
   X_Button.SetPos( FramePos( iPosX+354, iPosY+27, iPosX+354+27, iPosY+27+24 ) );
   Drag_Button.SetPos( FramePos( iPosX+60, iPosY+20, iPosX+320, iPosY+57 ) );

   m_stNameItemInfo.SetPos( FramePos( iPosX+57, iPosY+27, iPosX+346, iPosY+53 ) );


}
 
//////////////////////////////////////////////////////////////////////////////////////////
void V3_ItemInfoDlg::Draw(V2SPRITEFX *vsfFX)
{
   if(!IsShown())
      return;

   int iOffX = m_iXPos;
   int iOffY = m_iYPos;

   if(m_bDragDialog)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );

      int iNewPosX = m_iXPos + (mousePos.x-m_iXPosSD);
      int iNewPosY = m_iYPos + (mousePos.y-m_iYPosSD);
      
      if(iNewPosX < 0)
         iNewPosX = 0;

      if(iNewPosY < 0)
         iNewPosY = 0;
      
      if(iNewPosX+g_IFDlgW > g_Global.GetScreenW())
         iNewPosX = g_Global.GetScreenW()-g_IFDlgW;
      if(iNewPosY+g_IFDlgH > g_Global.GetScreenH())
         iNewPosY = g_Global.GetScreenH()-g_IFDlgH;


      UpdateAllFramePos(iNewPosX,iNewPosY);

      iOffX = iNewPosX;
      iOffY = iNewPosY;

   }

   

   g_Global.Lock();
   if(g_Global.IsItemInfoAvailable())
   {
      sItemInfo *pInfo = g_Global.GetItemInfo();
      char strNameTmp[512];
      char strtmp[255];
      
      UINT dwTypeItem = 1;
      COLORREF crName;
      if(pInfo->shAC     == 0 && pInfo->shDodge == 0 && pInfo->shEnd == 0 && pInfo->lMinDamage == 0 && pInfo->lMaxDamage == 0 && 
         pInfo->shAttack == 0 && pInfo->shStr   == 0 && pInfo->shAgi == 0 && pInfo->shWis      == 0 && pInfo->shInt      == 0 && 
         pInfo->vBoust.size() == 0)
      {
         dwTypeItem = 0;
         crName = g_DefColorH;
      }
      
      if(dwTypeItem != 0)
      {
         //regarde les boust pour definir la couleur des items...
         if(pInfo->vBoust.size() >= 9)
         {
            dwTypeItem = 4;
            crName = CL_ITEM_GREEN;
         }
         else if(pInfo->vBoust.size() >= 6)
         {
            dwTypeItem = 3;
            crName = CL_ITEM_YELLOW;
         }
         else if(pInfo->vBoust.size() >= 3)
         {
            dwTypeItem = 2;
            crName = CL_ITEM_PURPLE;
         }
         else
         {
            dwTypeItem = 1;
            crName = CL_ITEM_BLUE;
         }
         
         sprintf_s(strNameTmp,512  ,"%s",pInfo->Name);
         m_stNameItemInfo.SetText(strNameTmp,crName,290);
        
         
         GameUI::Draw( vsfFX );
         
         
         
         HDC hdc;
         DXDGetDC(&hdc, 101);
         
         
         
         int dwWriteYPos = 70;
         if(dwTypeItem != 0)
         {
            int iOffXR1 = 100;
            int iOffXR2 = 190;
            
            if(pInfo->shStr != 0 || pInfo->shEnd != 0 || pInfo->shAgi    != 0 ||
               pInfo->shWis != 0 || pInfo->shInt != 0 || pInfo->shAttack != 0 )
            {
               sprintf_s(strtmp,255  ,"%s",g_GUILocalString[87]);
               fSmallNormal_14->DrawFont(hdc, iOffX+TxtOff(fSmallNormal_14,hdc,strtmp ,381), iOffY+dwWriteYPos , g_DefColorH , 0, strtmp  );
               sprintf_s(strtmp,255  ,"%s","______________________________");
               fSmallNormal_14->DrawFont(hdc, iOffX+TxtOff(fSmallNormal_14,hdc,strtmp ,381), iOffY+dwWriteYPos+5 , g_DefColorH , 0, strtmp  );
               dwWriteYPos+=5;
               //On Commence par les requirement !!!
               dwWriteYPos+=21;
               if(pInfo->shStr != 0)
               {
                  sprintf_s(strtmp,255  ,"%s",g_GUILocalString[88]);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
                  sprintf_s(strtmp,255  ,":  %d",pInfo->shStr);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
                  dwWriteYPos+=15;
               }
               if(pInfo->shEnd != 0)
               {
                  sprintf_s(strtmp,255  ,"%s",g_GUILocalString[89]);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
                  sprintf_s(strtmp,255  ,":  %d",pInfo->shEnd);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
                  dwWriteYPos+=15;
               }
               if(pInfo->shAgi != 0)
               {
                  sprintf_s(strtmp,255  ,"%s",g_GUILocalString[90]);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
                  sprintf_s(strtmp,255  ,":  %d",pInfo->shAgi);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
                  dwWriteYPos+=15;
               }
               if(pInfo->shWis != 0)
               {
                  sprintf_s(strtmp,255  ,"%s",g_GUILocalString[91]);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
                  sprintf_s(strtmp,255  ,":  %d",pInfo->shWis);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
                  dwWriteYPos+=15;
               }
               if(pInfo->shInt != 0)
               {
                  sprintf_s(strtmp,255  ,"%s",g_GUILocalString[92]);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
                  sprintf_s(strtmp,255  ,":  %d",pInfo->shInt);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
                  dwWriteYPos+=15;
               }
               if(pInfo->shAttack != 0)
               {
                  sprintf_s(strtmp,255  ,"%s",g_GUILocalString[93]);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
                  sprintf_s(strtmp,255  ,":  %d",pInfo->shAttack);
                  fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
                  dwWriteYPos+=15;
               }
               dwWriteYPos+=5;
            } 


            
            if(pInfo->lMinDamage >0 || pInfo->lMaxDamage >0)
            {
               // les boust Direct....
               sprintf_s(strtmp,255  ,"%s",g_GUILocalString[94]);
               fSmallNormal_14->DrawFont(hdc, iOffX+TxtOff(fSmallNormal_14,hdc,strtmp ,381), iOffY+dwWriteYPos , g_DefColorH , 0, strtmp  );
               sprintf_s(strtmp,255  ,"%s","______________________________");
               fSmallNormal_14->DrawFont(hdc, iOffX+TxtOff(fSmallNormal_14,hdc,strtmp ,381), iOffY+dwWriteYPos+5 , g_DefColorH , 0, strtmp  );
               dwWriteYPos+=5;
               dwWriteYPos+=21;
               
               sprintf_s(strtmp,255  ,"%s",g_GUILocalString[94]);
               fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
               sprintf_s(strtmp,255  ,":  ( %d - %d )",pInfo->lMinDamage,pInfo->lMaxDamage);
               fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
               dwWriteYPos+=15;
               dwWriteYPos+=5;
            }

            iOffXR1 = 95;
            iOffXR2 = 140;
            
            if(pInfo->shDodge !=0 || pInfo->shAC != 0 || pInfo->vBoust.size() >0)
            {
               // les boust Direct....
               sprintf_s(strtmp,255  ,"%s",g_GUILocalString[95]);
               fSmallNormal_14->DrawFont(hdc, iOffX+TxtOff(fSmallNormal_14,hdc,strtmp ,381), iOffY+dwWriteYPos , g_DefColorH , 0, strtmp  );
               sprintf_s(strtmp,255  ,"%s","______________________________");
               fSmallNormal_14->DrawFont(hdc, iOffX+TxtOff(fSmallNormal_14,hdc,strtmp ,381), iOffY+dwWriteYPos+5 , g_DefColorH , 0, strtmp  );
               dwWriteYPos+=5;
               dwWriteYPos+=21;
               
               //commence par le boust de AC
               int dwBoustAC = 0;
               if(pInfo->shAC != 0 || pInfo->vBoust.size() >0)
               {
                  if(pInfo->shAC != 0)
                     dwBoustAC += pInfo->shAC;
                  
                  for(UINT i=0;i<pInfo->vBoust.size();i++)
                  {
                     if(pInfo->vBoust[i].chBoustStat == 20)
                     {
                        dwBoustAC += pInfo->vBoust[i].lMaxVal;
                     }
                  }
                  if(dwBoustAC !=0)
                  {
                     sprintf_s(strtmp,255  ,"%+d",dwBoustAC);
                     fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
                     sprintf_s(strtmp,255  ,"%s",m_strBoustName[20]);
                     fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
                     dwWriteYPos+=13;
                  }
               }
               //ensuite la dodge...
               int dwBoustDodge = 0;
               if(pInfo->shDodge != 0 || pInfo->vBoust.size() >0)
               {
                  if(pInfo->shDodge != 0)
                     dwBoustDodge += (-1*pInfo->shDodge);
                  
                  for(UINT i=0;i<pInfo->vBoust.size();i++)
                  {
                     if(pInfo->vBoust[i].chBoustStat == 9)
                     {
                        dwBoustDodge += pInfo->vBoust[i].lMaxVal;
                     }
                  }
                  if(dwBoustDodge !=0)
                  {
                     sprintf_s(strtmp ,255 ,"%+d",dwBoustDodge);
                     fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
                     sprintf_s(strtmp ,255 ,"%s",m_strBoustName[9]);
                     fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
                     dwWriteYPos+=13;
                  }
               }
               if(dwBoustAC != 0 || dwBoustDodge != 0)
                  dwWriteYPos+=6;
               //maintenant on scan la liste des autre boust...
               for(UINT i=0;i<pInfo->vBoust.size();i++)
               {
                  if(pInfo->vBoust[i].chBoustStat != 0 && pInfo->vBoust[i].chBoustStat != 9 && pInfo->vBoust[i].chBoustStat != 20)
                  {
                     sprintf_s(strtmp,255  ,"%+d",pInfo->vBoust[i].lMaxVal);
                     fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR1, iOffY+dwWriteYPos  , g_DefColorH , 0, strtmp  );
                     sprintf_s(strtmp,255  ,"%s",m_strBoustName[pInfo->vBoust[i].chBoustStat]);
                     fSmallNormal_14->DrawFont(hdc, iOffX+iOffXR2, iOffY+dwWriteYPos  , crName , 0, strtmp  );
                     dwWriteYPos+=13;
                  }
               }
               dwWriteYPos+=5;
            }
         }
         DXDReleaseDC(hdc, 101);
      }
      else
      {
         Hide();
         if(pInfo->bInventory)
         {
         }
         else
         {
            Objects.Lock(40);
            Objects.Identify(pInfo->ulID);
            Objects.Unlock(40);
         }
      }
   }
   
   g_Global.Unlock();
   
}

void V3_ItemInfoDlg::Hide()
{
   BoxUI::Hide();
   g_Global.ResetItemInfo();
}





void V3_ItemInfoDlg::StartDragDlg( MousePos mousePos )
{
   if(!g_SaveGame.GetOptionsParam()->bGUIDynamic)
      return;
   
   if(!m_bDragDialog)
   {
      //si premier start drag, on prend la position de depart du drag
      //pour en calculer le deltat X et Y du deplacement de la fenetre...
      {
         
         m_bDragDialog = true;
         m_iXPosSD = mousePos.x;
         m_iYPosSD = mousePos.y;

         RootBoxUI::GetInstance()->SetDragUI((GameUI*)&Drag_Button);
         g_Global.SetDraggingDialog(true);
      }
   }
}

void V3_ItemInfoDlg::StopDragDlg()
{
   if(m_bDragDialog)
   {
      POINT mousePos;
      MouseDevice md;
      md.GetPosition( (int *)&mousePos.x, (int *)&mousePos.y );
      
      m_iXPos = m_iXPos + (mousePos.x-m_iXPosSD);
      m_iYPos = m_iYPos + (mousePos.y-m_iYPosSD);
      
      if(m_iXPos < 0)
         m_iXPos = 0;
      if(m_iYPos < 0)
         m_iYPos = 0;
      
      if(m_iXPos+g_IFDlgW > g_Global.GetScreenW())
         m_iXPos = g_Global.GetScreenW()-g_IFDlgW;
      if(m_iYPos+g_IFDlgH > g_Global.GetScreenH())
         m_iYPos = g_Global.GetScreenH()-g_IFDlgH;
      
      UpdateAllFramePos(m_iXPos,m_iYPos);
      g_Global.SetDialogLastIFXYPos(m_iXPos,m_iYPos);
      
      m_bDragDialog = false;

      
      g_Global.SetDraggingDialog(false);
      RootBoxUI::GetInstance()->SetDragUI(NULL);
      
   }
}

///////////////////////////////////////////////////////
// HIDDEN GIALOG DRAG BUTTON
///////////////////////////////////////////////////////
void V3_ItemInfoDlg::DragEvent::LeftClicked( void )
{
   V3_ItemInfoDlg *me = static_cast< V3_ItemInfoDlg * >( GetUI() );
   
   me->StopDragDlg();
}

void V3_ItemInfoDlg::DragEvent::StartDragLButtonDown( int mX, int mY )
{
   V3_ItemInfoDlg *me = static_cast< V3_ItemInfoDlg * >( GetUI() );

   MousePos mp;
   mp.x = mX;
   mp.y = mY;
   me->StartDragDlg(mp);
}


int V3_ItemInfoDlg::TxtOff(Font *pFont, HDC hdc, char *pStr,int dwBoxZise)
{
   int dwSize = (int)pFont->GetFLen(hdc,pStr);
   return((dwBoxZise-dwSize)/2);
}