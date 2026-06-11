// *******************************************************************************************
// ***                                                                                     ***
//      File Name: SystemAction.cpp
//      Project:   TFC Client
//      Creation:  19/11/1997
//      Author:    Benoit Thomas (TH)
// ***                                                                                     ***
// *******************************************************************************************
// ***                                                                                     ***
//      Change History
//
//         Date            Ver.      Author         Purpose
//         ----            ----      ------         -------
//         19/11/1997      1.0        TH             Initial developpement
//
//      Description
//          Set the CallBack function pour les actions du jeux.
//          
// ***                                                                                     ***
//	*******************************************************************************************
// ***         Copyright (c) 1997-1998 Vircom. All rights reserved.                        ***

#include "pch.h"
#include "System.h"
#include "TfcPacket.h"
#include "TFCSocket.h"
#include "SystemAction.h"
#include "VisualObjectList.h"
#include "Global.h"
#include "NewInterface/V3_DisplaySpecialDlg.h"

DWORD g_dwLastMacroCallAtt = 0;
DWORD g_dwLastMacroCallSM  = 0;


//**************************************************************************************//
UINT AttackMode
//**************************************************************************************//
// Callback Function for using a Skill.
// 
(
 LPVOID pParam // Paramteter
)
//**************************************************************************************//
{
   if(timeGetTime()-g_dwLastMacroCallAtt > 100)
   {
      g_dwLastMacroCallAtt =timeGetTime();
      Objects.Lock(109);
      Objects.AttackMode();
      Objects.Unlock(109);
   }
	return 0;
}


UINT UpdateAttackMode(LPVOID)
{
   Objects.Lock(109);
   Objects.UpdateAttackMode();
   Objects.Unlock(109);
   return 0;
}

//**************************************************************************************//
UINT ShowActualMap
//**************************************************************************************//
// Callback Function for using a Skill.
// 
(
 LPVOID pParam // Paramteter
)
//**************************************************************************************//
{
   /*
	Objects.Lock(0);
	F1Help.ShowMap(false);
	Objects.Unlock(0);
   */

	return 0;
}

//**************************************************************************************//
// Callback Function for using a Skill.
// 
//**************************************************************************************//
UINT ShowWorldMap(LPVOID pParam)
{
   if(timeGetTime()-g_dwLastMacroCallSM > 500)
   {
      g_dwLastMacroCallSM =timeGetTime();
      V3_DisplaySpecialDlg::GetInstance()->ShowMaps();
   }
	return 0;
}




