// NTime.cpp: implementation of the NTime class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <stdio.h>

#include "NTime.h"
#include "IDirectXDraw.h"
#include "TFCPlayer.h"
#include "Tileset.h"
#include "MacroHandler.h"
#include "App.h"
#include "Global.h"
#include "Lightmap.h"

#include "NewInterface/V3_LifeDlg.h"

typedef struct _s15Bits
{
   DWORD   dwB:5;
   DWORD   dwG:5;
   DWORD   dwR:5;
   DWORD   Reserved:1;
}s15Bits;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NTime g_TimeStructure;

NTime::NTime()
{
	ForceRed    = 3;
	ForceGreen  = 3;
	ForceBlue   = 3;
	Seconde = 0;
	Hour = 0;
	Day = 0;
	Month = 0;
	Year = 0;
   CurrentLight = 0;
   CurrentLow = 0;
}

NTime::~NTime()
{

}

const int NIGHT_R = 1,
          NIGHT_G = 4, 
          NIGHT_B = 5,

          AUBE_R  = 31,
          AUBE_G  = 21,
          AUBE_B  = 11,

          DAY_R   = 31,
          DAY_G   = 27,
          DAY_B   = 23,

          DUNGEON_R = 3,//4,
          DUNGEON_G = 3,//4,
          DUNGEON_B = 3,//4,

          CAVERN_R = 3,//3,
          CAVERN_G = 3,//3,
          CAVERN_B = 3;//3;

/*
const int NIGHT_R = 3,
		    NIGHT_G = 3,
		    NIGHT_B = 5,
		    
          AUBE_R  = 26,
		    AUBE_G  = 19,
		    AUBE_B  = 10,
		    
          DAY_R   = 31,
		    DAY_G   = 31,
		    DAY_B   = 31,

          DUNGEON_R = 4,
          DUNGEON_G = 4,
          DUNGEON_B = 4,

          CAVERN_R = 3,
          CAVERN_G = 3,
          CAVERN_B = 3;
  */    
void NTime::AddSeconde(void) {
	Seconde++;
	if (Seconde == 60) {
		Seconde = 0;
		Minute++;
		if (Minute == 60) {
			Minute = 0;
			Hour++;
			if (Hour == 24) {
				Hour = 0;
				Day++;
				if (Day == 7) {
					Day = 0;
					Week++;
					if (Week == 4) {
						Week = 0;
						Month++;
						if (Month == 12) {
							Month = 0;
							Year++;
						}
					}
				}
			}
		}
	}
}

//extern WORD LightBuffer[SCREEN_W_DIV2*SCREEN_H_DIV2];
//##NMNMNM --[Partie qui gere le Temps]
void NTime::SetLight(void) 
{
   if((V3_LifeDlg::GetInstance()->IsDeath()))
   {
      //When death see in RED...
      ForceRed    = 31;
      ForceGreen  = 1;
      ForceBlue   = 1;
      LightMapOBJ.MakeBaseLightMap(ForceRed, ForceGreen, ForceBlue);
      return;
   }
   
   if(Custom.gDebugLightTime)
   {
      ForceRed    = Custom.gTIME_R;
      ForceGreen  = Custom.gTIME_G;
      ForceBlue   = Custom.gTIME_B;

     
      
      LightMapOBJ.MakeBaseLightMap(ForceRed, ForceGreen, ForceBlue);
      return;
   }
   if(Custom.gDebugLightDay || Custom.gDebugLightNight)
   {
      if(Custom.gDebugLightDay)
      {
         ForceRed    = DAY_R;
         ForceGreen  = DAY_G;
         ForceBlue   = DAY_B;
      }
      else
      {
         ForceRed    = NIGHT_R;
         ForceGreen  = NIGHT_G;
         ForceBlue   = NIGHT_B;
      }
   

      LightMapOBJ.MakeBaseLightMap(ForceRed, ForceGreen, ForceBlue);
      return;
   }

   WORD wVal = World.GetPosViewL(Player.xPos,Player.yPos);
   s15Bits sVal;
   memcpy(&sVal,&wVal,2);
   ForceRed   = sVal.dwR;
   ForceGreen = sVal.dwG;
   ForceBlue  = sVal.dwB;
   if(ForceRed != 31 || ForceGreen != 31 || ForceBlue != 31)
   {
      //ajouter eventuelle transition de lumiere...
      LightMapOBJ.MakeBaseLightMap(ForceRed, ForceGreen, ForceBlue);
      return;
   }

   //set le jour ou nuit si tout est blanc...
   if (Hour >= 0 && Hour < 4) 
   {
      // NIGHT
      ForceRed = NIGHT_R;
      ForceGreen = NIGHT_G;
      ForceBlue = NIGHT_B;
   } 
   else if (Hour >= 4 && Hour < 5) 
   {
      // NIGH->AUBE Part 1
      ForceRed  = NIGHT_R+Minute*((AUBE_R-NIGHT_R)/2)/60;
      ForceGreen = NIGHT_G+Minute*((AUBE_G-NIGHT_G)/2)/60;
      ForceBlue  = NIGHT_B+Minute*((AUBE_B-NIGHT_B)/2)/60;
   } 
   else if (Hour >= 5 && Hour < 6) 
   {
      // NIGH->AUBE Part 2
      ForceRed = NIGHT_R+((AUBE_R-NIGHT_R)/2)+Minute*((AUBE_R-NIGHT_R)/2)/60;
      ForceGreen = NIGHT_G+((AUBE_G-NIGHT_G)/2)+Minute*((AUBE_G-NIGHT_G)/2)/60;
      ForceBlue = NIGHT_B+((AUBE_B-NIGHT_B)/2)+Minute*((AUBE_B-NIGHT_B)/2)/60;
   } 
   else if (Hour >= 6 && Hour < 8) 
   {
      // AUBE
      ForceRed = AUBE_R;
      ForceGreen = AUBE_G;
      ForceBlue = AUBE_B;
   } 
   else if (Hour >= 8 && Hour < 9) 
   {
      // AUBE->DAY Part 1
      ForceRed = AUBE_R+Minute*((DAY_R-AUBE_R)/2)/60;
      ForceGreen = AUBE_G+Minute*((DAY_G-AUBE_G)/2)/60;
      ForceBlue = AUBE_B+Minute*((DAY_B-AUBE_B)/2)/60;
   } 
   else if (Hour >= 9 && Hour < 10) 
   {
      // AUBE->DAY Part 2
      ForceRed = AUBE_R+((DAY_R-AUBE_R)/2)+Minute*((DAY_R-AUBE_R)/2)/60;
      ForceGreen = AUBE_G+((DAY_G-AUBE_G)/2)+Minute*((DAY_G-AUBE_G)/2)/60;
      ForceBlue = AUBE_B+((DAY_B-AUBE_B)/2)+Minute*((DAY_B-AUBE_B)/2)/60;
   } 
   else if (Hour >= 10 && Hour < 19) 
   {
      // DAY
      ForceRed = DAY_R;
      ForceGreen = DAY_G;
      ForceBlue = DAY_B;
   } 
   else if (Hour >= 19 && Hour < 20) 
   {
      // DAY->NIGHT Part 1
      ForceRed = DAY_R+Minute*((NIGHT_R-DAY_R)/2)/60;
      ForceGreen = DAY_G+Minute*((NIGHT_G-DAY_G)/2)/60;
      ForceBlue = DAY_B+Minute*((NIGHT_B-DAY_B)/2)/60;
   } 
   else if (Hour >= 20 && Hour < 21) 
   {
      // DAY->NIGHT Part 2
      ForceRed = DAY_R+((NIGHT_R-DAY_R)/2)+Minute*((NIGHT_R-DAY_R)/2)/60;
      ForceGreen = DAY_G+((NIGHT_G-DAY_G)/2)+Minute*((NIGHT_G-DAY_G)/2)/60;
      ForceBlue = DAY_B+((NIGHT_B-DAY_B)/2)+Minute*((NIGHT_B-DAY_B)/2)/60;
   } 
   else if (Hour >= 21 && Hour < 24) 
   {
      ForceRed = NIGHT_R;
      ForceGreen = NIGHT_G;
      ForceBlue = NIGHT_B;
   } 
   
 


   LightMapOBJ.MakeBaseLightMap(ForceRed, ForceGreen, ForceBlue);
}