// ************************************************************************
// ***                                                                  ***
//      Project:   TFC Client
//      Creation:  29 June, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***
//      Change History
//
//         Date            Ver.	  Author	     Purpose
//         ----            ----    ------      -------
//         06-29-1997      1.0     TH          Initial development
//
//      Description
//         Provide fonction to load a Sprite from a Image or an ID
// ***                                                                  ***
//	************************************************************************
// *** Copyright (c) 1997-1998 Vircom. All rights reserved.             ***
#include "pch.h"
#pragma warning( disable : 4291 )

#include "Icon3D.h" 
#include "Random.h" 
#include "Global.h"
#include "V2PalManager.h"

inline int Abs(int x) {
	return ((x < 0) ? (-x) : (x));
}

Sprite3D::Sprite3D()
// Constructeur
{
   bAlreadyLoad = FALSE;
	
	LastAccess = timeGetTime();
	MaxWidth = 0;
	MaxHeight = 0;
	MaxxCor = 0;
	MaxyCor = 0;

   View000 = NULL;
	View045 = NULL;
	View090 = NULL;
	View135 = NULL;
	View180 = NULL;

   Att000 = NULL;
	Att045 = NULL;
	Att090 = NULL;
	Att135 = NULL;
	Att180 = NULL;

   Rng000 = NULL;
	Rng045 = NULL;
	Rng090 = NULL;
	Rng135 = NULL;
	Rng180 = NULL;

   StMov000 = NULL;
   StMov045 = NULL;
   StMov090 = NULL;
   StMov135 = NULL;
   StMov180 = NULL;

   StAtt000 = NULL;
   StAtt045 = NULL;
   StAtt090 = NULL;
   StAtt135 = NULL;
   StAtt180 = NULL;



   Corpse = NULL;
   StAtt = -1;
   StMov = -1;
   bColorG2R[0] = 28;
   bColorG2R[1] = 41;
   bColorG2R[2] = 40;
   bColorG2R[3] = 223;
   bColorG2R[4] = 210;
   bColorG2R[5] = 208;
   bColorG2R[6] = 97;
   bColorG2R[7] = 2;
   bColorG2R[8] = 2;
   bColorG2R[9] = 2;
}

Sprite3D::~Sprite3D()
// Destructeur
{
   Release();
}

void Sprite3D::Release(void) {
   bAlreadyLoad = FALSE;
   
   if (View000) {
      delete [] View000;
	   View000 = NULL;
   }

   if (View045) {
   	delete [] View045;
	  	View045 = NULL;
   }

   if (View090) {
   	delete [] View090;
	   View090 = NULL;
   }

   if (View135) {
	   delete [] View135;
	   View135 = NULL;
   }

   if (View180) {
	   delete [] View180;
	   View180 = NULL;
   }

   if (Att000) {
      delete [] Att000;
		Att000 = NULL;
   }

   if (Att045) {
		delete [] Att045;
		Att045 = NULL;
   }

   if (Att090) {
		delete [] Att090;
		Att090 = NULL;
   }

   if (Att135) {
		delete [] Att135;
		Att135 = NULL;
   }

   if (Att180) {
		delete [] Att180;
		Att180 = NULL;
   }

   if (Rng000) {
      delete [] Rng000;
		Rng000 = NULL;
   }

   if (Rng045) {
		delete [] Rng045;
		Rng045 = NULL;
   }

   if (Rng090) {
		delete [] Rng090;
		Rng090 = NULL;
   }

   if (Rng135) {
		delete [] Rng135;
		Rng135 = NULL;
   }

   if (Rng180) {
		delete [] Rng180;
		Rng180 = NULL;
   }

   if (Corpse) {
      delete [] Corpse;
      Corpse = NULL;
   }

   if (StMov000) {
      delete [] StMov000;
      StMov000 = NULL;
   }

   if (StMov045) {
      delete [] StMov045;
      StMov045 = NULL;
   }

   if (StMov090) {
      delete [] StMov090;
      StMov090 = NULL;
   }

   if (StMov135) {
      delete [] StMov135;
      StMov135 = NULL;
   }

   if (StMov180) {
      delete [] StMov180;
      StMov180 = NULL;
   }

   if (StAtt000) {
      delete [] StAtt000;
      StAtt000 = NULL;
   }

   if (StAtt045) {
      delete [] StAtt045;
      StAtt045 = NULL;
   }

   if (StAtt090) {
      delete [] StAtt090;
      StAtt090 = NULL;
   }

   if (StAtt135) {
      delete [] StAtt135;
      StAtt135 = NULL;
   }

   if (StAtt180) {
      delete [] StAtt180;
      StAtt180 = NULL;
   }





}

void Sprite3D::LoadSprite3D(int faces, char *ViewID, int palID) {
   if (bAlreadyLoad)
		return;
	
   
   bAlreadyLoad = TRUE;

	LastAccess = timeGetTime();
   
   // Load a 3D Sprite
	char ID[80];
	
	Type = 2;
	faces /= FRAMING;
	Faces = faces;
	FacesA = 0;
   FacesC = 0;
   FacesR = 0;
   FacesStA = 0;
   FacesStM = 0;
   StMov = 0;
	StAtt = 0;
	
   View000 = new CV2Sprite [faces];
	View045 = new CV2Sprite [faces];
	View090 = new CV2Sprite [faces];
	View135 = new CV2Sprite [faces];
	View180 = new CV2Sprite [faces];

   MaxxCor = 1000;
	MaxyCor = 1000;
	int ChoseX = 0;
	int ChoseY = 0;
   int i;
   int iB000 = 0, iB045 = 0, iB090 = 0, iB135 = 0, iB180 = 0;

	//LPBYTE bestPal = CV2PalManager::GetInstance()->GetPal(ViewID, palID);

	for (i = 0; i < faces; i++) 
   {
      sprintf_s(ID,80,"%s000-%c",ViewID,(i*FRAMING) + 'a');
		//iB000 = View000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
      iB000 = View000[i].CreateSprite(ID, iB000, palID);
      
		if (MaxWidth < View000[i].GetWidth()) MaxWidth = View000[i].GetWidth();
		if (MaxHeight < View000[i].GetHeight()) MaxHeight = View000[i].GetHeight();
		if (MaxxCor > View000[i].GetCX()) MaxxCor = View000[i].GetCX();
		if (MaxyCor > View000[i].GetCY()) MaxyCor = View000[i].GetCY();
		if (ChoseX < View000[i].GetCX() + View000[i].GetWidth()) ChoseX = View000[i].GetCX() + View000[i].GetWidth();
		if (ChoseY < View000[i].GetCY() + View000[i].GetHeight()) ChoseY = View000[i].GetCY() + View000[i].GetHeight();

      sprintf_s(ID,80,"%s045-%c",ViewID,(i*FRAMING) + 'a');
		//iB045 = View045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
      iB045 = View045[i].CreateSprite(ID, iB045, palID);

		if (MaxWidth < View045[i].GetWidth()) MaxWidth = View045[i].GetWidth();
		if (MaxHeight < View045[i].GetHeight()) MaxHeight = View045[i].GetHeight();
		if (MaxxCor > View045[i].GetCX()) MaxxCor = View045[i].GetCX();
		if (MaxyCor > View045[i].GetCY()) MaxyCor = View045[i].GetCY();
		if (ChoseX < View045[i].GetCX() + View045[i].GetWidth()) ChoseX = View045[i].GetCX() + View045[i].GetWidth();
		if (ChoseY < View045[i].GetCY() + View045[i].GetHeight()) ChoseY = View045[i].GetCY() + View045[i].GetHeight();
		
      sprintf_s(ID,80,"%s090-%c",ViewID,(i*FRAMING) + 'a');
		//iB090 = View090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
      iB090 = View090[i].CreateSprite(ID, iB090, palID);
		if (MaxWidth < View090[i].GetWidth()) MaxWidth = View090[i].GetWidth();
		if (MaxHeight < View090[i].GetHeight()) MaxHeight = View090[i].GetHeight();
		if (MaxxCor > View090[i].GetCX()) MaxxCor = View090[i].GetCX();
		if (MaxyCor > View090[i].GetCY()) MaxyCor = View090[i].GetCY();
		if (ChoseX < View090[i].GetCX() + View090[i].GetWidth()) ChoseX = View090[i].GetCX() + View090[i].GetWidth();
		if (ChoseY < View090[i].GetCY() + View090[i].GetHeight()) ChoseY = View090[i].GetCY() + View090[i].GetHeight();
		
      sprintf_s(ID,80,"%s135-%c",ViewID,(i*FRAMING) + 'a');
		//iB135 = View135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
      iB135 = View135[i].CreateSprite(ID, iB135,palID);

		if (MaxWidth < View135[i].GetWidth()) MaxWidth = View135[i].GetWidth();
		if (MaxHeight < View135[i].GetHeight()) MaxHeight = View135[i].GetHeight();
		if (MaxxCor > View135[i].GetCX()) MaxxCor = View135[i].GetCX();
		if (MaxyCor > View135[i].GetCY()) MaxyCor = View135[i].GetCY();
		if (ChoseX < View135[i].GetCX() + View135[i].GetWidth()) ChoseX = View135[i].GetCX() + View135[i].GetWidth();
		if (ChoseY < View135[i].GetCY() + View135[i].GetHeight()) ChoseY = View135[i].GetCY() + View135[i].GetHeight();
		
      sprintf_s(ID,80,"%s180-%c",ViewID,(i*FRAMING) + 'a');
		//iB180 = View180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
      iB180 = View180[i].CreateSprite(ID, iB180, palID);
      
		if (MaxWidth < View180[i].GetWidth()) MaxWidth = View180[i].GetWidth();
		if (MaxHeight < View180[i].GetHeight()) MaxHeight = View180[i].GetHeight();
		if (MaxxCor > View180[i].GetCX()) MaxxCor = View180[i].GetCX();
		if (MaxyCor > View180[i].GetCY()) MaxyCor = View180[i].GetCY();
		if (ChoseX < View180[i].GetCX() + View180[i].GetWidth()) ChoseX = View180[i].GetCX() + View180[i].GetWidth();
		if (ChoseY < View180[i].GetCY() + View180[i].GetHeight()) ChoseY = View180[i].GetCY() + View180[i].GetHeight();
		
   }

   fTransparent = View000[0].GetfTransparent();
	TransColor = View000[0].GetTransColor();
   if ((MaxxCor + MaxyCor) % 2) {
      MaxxCor--;
      MaxWidth++;
   }
	MaxWidth += ChoseX - (MaxxCor + MaxWidth);
	MaxHeight += ChoseY - (MaxyCor + MaxHeight);

   if (Type == 3)
      Type = 0;
}

void Sprite3D::LoadSprite3D(int faces, int faces2, int faces3, int faces4, int faces5, char *ViewID, int type, int stAtt, int stMov, int palID) {
   if (bAlreadyLoad)
		return;
	bAlreadyLoad = TRUE;


   LastAccess = timeGetTime();
   // Load a 3D Sprite
	char ID[80];
		
	Type = type;
	faces /= FRAMING;
	faces2 /= FRAMING;
	Faces = faces;
	FacesA = faces2;
   FacesR = 0;  //NMNMNM_Faces
	FacesC = faces5;
   StAtt = stAtt;
   StMov = stMov;
   FacesStA = faces3;
   FacesStM = faces4;
	
	View000 = new CV2Sprite [faces];
	View045 = new CV2Sprite [faces];
	View090 = new CV2Sprite [faces];
	View135 = new CV2Sprite [faces];
	View180 = new CV2Sprite [faces];

	Att000 = new CV2Sprite [faces2];
	Att045 = new CV2Sprite [faces2];
	Att090 = new CV2Sprite [faces2];
	Att135 = new CV2Sprite [faces2];
	Att180 = new CV2Sprite [faces2];

   /*
   Rng000 = new CV2Sprite [faces2];
   Rng045 = new CV2Sprite [faces2];
   Rng090 = new CV2Sprite [faces2];
   Rng135 = new CV2Sprite [faces2];
   Rng180 = new CV2Sprite [faces2];
   */


   if (faces5)
		Corpse = new CV2Sprite[faces5];



   if (StAtt == -1) {
      StAtt000 = new CV2Sprite [faces3];
      StAtt045 = new CV2Sprite [faces3];
      StAtt090 = new CV2Sprite [faces3];
      StAtt135 = new CV2Sprite [faces3];
      StAtt180 = new CV2Sprite [faces3];
   }
   
   if (StMov == -1) {
      StMov000 = new CV2Sprite [faces4];
      StMov045 = new CV2Sprite [faces4];
      StMov090 = new CV2Sprite [faces4];
      StMov135 = new CV2Sprite [faces4];
      StMov180 = new CV2Sprite [faces4];
   }

	int ChoseX = 0;
	int ChoseY = 0;
   int iBC = 0;

   // New pallet loading system
	char ViewIDA[80];
	char ViewIDStAtt[80];
	char ViewIDStMov[80];
	char ViewIDC[80];
   sprintf_s(ViewIDC,80,"%sC",ViewID);
	LPBYTE bestPal = CV2PalManager::GetInstance()->GetPal(ViewIDC, palID);

   int i = 0;
   for (i = 0; i < faces5; i++) 
   {
      sprintf_s(ID,80,"%sC-%c",ViewID,i+'a');
      iBC = Corpse[i].CreateSpriteByPal(ID, iBC, bestPal,palID);
   }
	

	bestPal = CV2PalManager::GetInstance()->GetPal(ViewID, palID);
	int iB000 = 0, iB045 = 0, iB090 = 0, iB135 = 0, iB180 = 0;
   for (i = 0; i < faces; i++) 
   {
      sprintf_s(ID,80,"%s000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = View000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
		if (MaxWidth < View000[i].GetWidth()) MaxWidth = View000[i].GetWidth();
		if (MaxHeight < View000[i].GetHeight()) MaxHeight = View000[i].GetHeight();
		if (MaxxCor > View000[i].GetCX()) MaxxCor = View000[i].GetCX();
		if (MaxyCor > View000[i].GetCY()) MaxyCor = View000[i].GetCY();
		if (ChoseX < View000[i].GetCX() + View000[i].GetWidth()) ChoseX = View000[i].GetCX() + View000[i].GetWidth();
		if (ChoseY < View000[i].GetCY() + View000[i].GetHeight()) ChoseY = View000[i].GetCY() + View000[i].GetHeight();

      sprintf_s(ID,80,"%s045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = View045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
		if (MaxWidth < View045[i].GetWidth()) MaxWidth = View045[i].GetWidth();
		if (MaxHeight < View045[i].GetHeight()) MaxHeight = View045[i].GetHeight();
		if (MaxxCor > View045[i].GetCX()) MaxxCor = View045[i].GetCX();
		if (MaxyCor > View045[i].GetCY()) MaxyCor = View045[i].GetCY();
		if (ChoseX < View045[i].GetCX() + View045[i].GetWidth()) ChoseX = View045[i].GetCX() + View045[i].GetWidth();
		if (ChoseY < View045[i].GetCY() + View045[i].GetHeight()) ChoseY = View045[i].GetCY() + View045[i].GetHeight();
		
      sprintf_s(ID,80,"%s090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = View090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
		if (MaxWidth < View090[i].GetWidth()) MaxWidth = View090[i].GetWidth();
		if (MaxHeight < View090[i].GetHeight()) MaxHeight = View090[i].GetHeight();
		if (MaxxCor > View090[i].GetCX()) MaxxCor = View090[i].GetCX();
		if (MaxyCor > View090[i].GetCY()) MaxyCor = View090[i].GetCY();
		if (ChoseX < View090[i].GetCX() + View090[i].GetWidth()) ChoseX = View090[i].GetCX() + View090[i].GetWidth();
		if (ChoseY < View090[i].GetCY() + View090[i].GetHeight()) ChoseY = View090[i].GetCY() + View090[i].GetHeight();
		
		sprintf_s(ID,80,"%s135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = View135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
		if (MaxWidth < View135[i].GetWidth()) MaxWidth = View135[i].GetWidth();
		if (MaxHeight < View135[i].GetHeight()) MaxHeight = View135[i].GetHeight();
		if (MaxxCor > View135[i].GetCX()) MaxxCor = View135[i].GetCX();
		if (MaxyCor > View135[i].GetCY()) MaxyCor = View135[i].GetCY();
		if (ChoseX < View135[i].GetCX() + View135[i].GetWidth()) ChoseX = View135[i].GetCX() + View135[i].GetWidth();
		if (ChoseY < View135[i].GetCY() + View135[i].GetHeight()) ChoseY = View135[i].GetCY() + View135[i].GetHeight();
		
		sprintf_s(ID,80,"%s180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = View180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
		if (MaxWidth < View180[i].GetWidth()) MaxWidth = View180[i].GetWidth();
		if (MaxHeight < View180[i].GetHeight()) MaxHeight = View180[i].GetHeight();
		if (MaxxCor > View180[i].GetCX()) MaxxCor = View180[i].GetCX();
		if (MaxyCor > View180[i].GetCY()) MaxyCor = View180[i].GetCY();
		if (ChoseX < View180[i].GetCX() + View180[i].GetWidth()) ChoseX = View180[i].GetCX() + View180[i].GetWidth();
		if (ChoseY < View180[i].GetCY() + View180[i].GetHeight()) ChoseY = View180[i].GetCY() + View180[i].GetHeight();
		
	}

   sprintf_s(ViewIDA,80,"%sA",ViewID);
	bestPal = CV2PalManager::GetInstance()->GetPal(ViewIDA, palID);
	iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces2; i++) 
   {
      if (Type != 3)
         sprintf_s(ID,80,"%sA000-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = Att000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
		if (MaxWidth < Att000[i].GetWidth()) MaxWidth = Att000[i].GetWidth();
		if (MaxHeight < Att000[i].GetHeight()) MaxHeight = Att000[i].GetHeight();
		if (MaxxCor > Att000[i].GetCX()) MaxxCor = Att000[i].GetCX();
		if (MaxyCor > Att000[i].GetCY()) MaxyCor = Att000[i].GetCY();
		if (ChoseX < Att000[i].GetCX() + Att000[i].GetWidth()) ChoseX = Att000[i].GetCX() + Att000[i].GetWidth();
		if (ChoseY < Att000[i].GetCY() + Att000[i].GetHeight()) ChoseY = Att000[i].GetCY() + Att000[i].GetHeight();
      //Force un Bow pour emulation PC
      /*
      iB000 = Rng000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
      if (MaxWidth < Rng000[i].GetWidth()) MaxWidth = Rng000[i].GetWidth();
      if (MaxHeight < Rng000[i].GetHeight()) MaxHeight = Rng000[i].GetHeight();
      if (MaxxCor > Rng000[i].GetCX()) MaxxCor = Rng000[i].GetCX();
      if (MaxyCor > Rng000[i].GetCY()) MaxyCor = Rng000[i].GetCY();
      if (ChoseX < Rng000[i].GetCX() + Rng000[i].GetWidth()) ChoseX = Rng000[i].GetCX() + Rng000[i].GetWidth();
		if (ChoseY < Rng000[i].GetCY() + Rng000[i].GetHeight()) ChoseY = Rng000[i].GetCY() + Rng000[i].GetHeight();
      */

      if (Type != 3)
         sprintf_s(ID,80,"%sA045-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = Att045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
		if (MaxWidth < Att045[i].GetWidth()) MaxWidth = Att045[i].GetWidth();
		if (MaxHeight < Att045[i].GetHeight()) MaxHeight = Att045[i].GetHeight();
		if (MaxxCor > Att045[i].GetCX()) MaxxCor = Att045[i].GetCX();
		if (MaxyCor > Att045[i].GetCY()) MaxyCor = Att045[i].GetCY();
		if (ChoseX < Att045[i].GetCX() + Att045[i].GetWidth()) ChoseX = Att045[i].GetCX() + Att045[i].GetWidth();
		if (ChoseY < Att045[i].GetCY() + Att045[i].GetHeight()) ChoseY = Att045[i].GetCY() + Att045[i].GetHeight();
      //Force un Bow pour emulation PC
      /*
      iB045 = Rng045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
      if (MaxWidth < Rng045[i].GetWidth()) MaxWidth = Rng045[i].GetWidth();
      if (MaxHeight < Rng045[i].GetHeight()) MaxHeight = Rng045[i].GetHeight();
      if (MaxxCor > Rng045[i].GetCX()) MaxxCor = Rng045[i].GetCX();
      if (MaxyCor > Rng045[i].GetCY()) MaxyCor = Rng045[i].GetCY();
      if (ChoseX < Rng045[i].GetCX() + Rng045[i].GetWidth()) ChoseX = Rng045[i].GetCX() + Rng045[i].GetWidth();
		if (ChoseY < Rng045[i].GetCY() + Rng045[i].GetHeight()) ChoseY = Rng045[i].GetCY() + Rng045[i].GetHeight();
      */
		
      if (Type != 3)
         sprintf_s(ID,80,"%sA090-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = Att090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
		if (MaxWidth < Att090[i].GetWidth()) MaxWidth = Att090[i].GetWidth();
		if (MaxHeight < Att090[i].GetHeight()) MaxHeight = Att090[i].GetHeight();
		if (MaxxCor > Att090[i].GetCX()) MaxxCor = Att090[i].GetCX();
		if (MaxyCor > Att090[i].GetCY()) MaxyCor = Att090[i].GetCY();
		if (ChoseX < Att090[i].GetCX() + Att090[i].GetWidth()) ChoseX = Att090[i].GetCX() + Att090[i].GetWidth();
		if (ChoseY < Att090[i].GetCY() + Att090[i].GetHeight()) ChoseY = Att090[i].GetCY() + Att090[i].GetHeight();
      //Force un Bow pour emulation PC
      /*
      iB090 = Rng090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
      if (MaxWidth < Rng090[i].GetWidth()) MaxWidth = Rng090[i].GetWidth();
      if (MaxHeight < Rng090[i].GetHeight()) MaxHeight = Rng090[i].GetHeight();
      if (MaxxCor > Rng090[i].GetCX()) MaxxCor = Rng090[i].GetCX();
      if (MaxyCor > Rng090[i].GetCY()) MaxyCor = Rng090[i].GetCY();
      if (ChoseX < Rng090[i].GetCX() + Rng090[i].GetWidth()) ChoseX = Rng090[i].GetCX() + Rng090[i].GetWidth();
		if (ChoseY < Rng090[i].GetCY() + Rng090[i].GetHeight()) ChoseY = Rng090[i].GetCY() + Rng090[i].GetHeight();
      */
		
      if (Type != 3)
         sprintf_s(ID,80,"%sA135-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = Att135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
		if (MaxWidth < Att135[i].GetWidth()) MaxWidth = Att135[i].GetWidth();
		if (MaxHeight < Att135[i].GetHeight()) MaxHeight = Att135[i].GetHeight();
		if (MaxxCor > Att135[i].GetCX()) MaxxCor = Att135[i].GetCX();
		if (MaxyCor > Att135[i].GetCY()) MaxyCor = Att135[i].GetCY();
		if (ChoseX < Att135[i].GetCX() + Att135[i].GetWidth()) ChoseX = Att135[i].GetCX() + Att135[i].GetWidth();
		if (ChoseY < Att135[i].GetCY() + Att135[i].GetHeight()) ChoseY = Att135[i].GetCY() + Att135[i].GetHeight();
      //Force un Bow pour emulation PC
      /*
      iB135 = Rng135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
      if (MaxWidth < Rng135[i].GetWidth()) MaxWidth = Rng135[i].GetWidth();
      if (MaxHeight < Rng135[i].GetHeight()) MaxHeight = Rng135[i].GetHeight();
      if (MaxxCor > Rng135[i].GetCX()) MaxxCor = Rng135[i].GetCX();
      if (MaxyCor > Rng135[i].GetCY()) MaxyCor = Rng135[i].GetCY();
      if (ChoseX < Rng135[i].GetCX() + Rng135[i].GetWidth()) ChoseX = Rng135[i].GetCX() + Rng135[i].GetWidth();
		if (ChoseY < Rng135[i].GetCY() + Rng135[i].GetHeight()) ChoseY = Rng135[i].GetCY() + Rng135[i].GetHeight();
		*/

      if (Type != 3)
         sprintf_s(ID,80,"%sA180-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = Att180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
		if (MaxWidth < Att180[i].GetWidth()) MaxWidth = Att180[i].GetWidth();
		if (MaxHeight < Att180[i].GetHeight()) MaxHeight = Att180[i].GetHeight();
		if (MaxxCor > Att180[i].GetCX()) MaxxCor = Att180[i].GetCX();
		if (MaxyCor > Att180[i].GetCY()) MaxyCor = Att180[i].GetCY();
		if (ChoseX < Att180[i].GetCX() + Att180[i].GetWidth()) ChoseX = Att180[i].GetCX() + Att180[i].GetWidth();
		if (ChoseY < Att180[i].GetCY() + Att180[i].GetHeight()) ChoseY = Att180[i].GetCY() + Att180[i].GetHeight();
      //Force un Bow pour emulation PC
      /*
      iB180 = Rng180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
      if (MaxWidth < Rng180[i].GetWidth()) MaxWidth = Rng180[i].GetWidth();
      if (MaxHeight < Rng180[i].GetHeight()) MaxHeight = Rng180[i].GetHeight();
      if (MaxxCor > Rng180[i].GetCX()) MaxxCor = Rng180[i].GetCX();
      if (MaxyCor > Rng180[i].GetCY()) MaxyCor = Rng180[i].GetCY();
      if (ChoseX < Rng180[i].GetCX() + Rng180[i].GetWidth()) ChoseX = Rng180[i].GetCX() + Rng180[i].GetWidth();
		if (ChoseY < Rng180[i].GetCY() + Rng180[i].GetHeight()) ChoseY = Rng180[i].GetCY() + Rng180[i].GetHeight();
      */
		
	}

   sprintf_s(ViewIDStAtt,80,"%sStAtt",ViewID);
	bestPal = CV2PalManager::GetInstance()->GetPal(ViewIDStAtt, palID);
   iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces3; i++) 
   {
      sprintf_s(ID,80,"%sStAtt000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = StAtt000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
		if (MaxWidth < StAtt000[i].GetWidth()) MaxWidth = StAtt000[i].GetWidth();
		if (MaxHeight < StAtt000[i].GetHeight()) MaxHeight = StAtt000[i].GetHeight();
		if (MaxxCor > StAtt000[i].GetCX()) MaxxCor = StAtt000[i].GetCX();
		if (MaxyCor > StAtt000[i].GetCY()) MaxyCor = StAtt000[i].GetCY();
		if (ChoseX < StAtt000[i].GetCX() + StAtt000[i].GetWidth()) ChoseX = StAtt000[i].GetCX() + StAtt000[i].GetWidth();
		if (ChoseY < StAtt000[i].GetCY() + StAtt000[i].GetHeight()) ChoseY = StAtt000[i].GetCY() + StAtt000[i].GetHeight();

		sprintf_s(ID,80,"%sStAtt045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = StAtt045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
		if (MaxWidth < StAtt045[i].GetWidth()) MaxWidth = StAtt045[i].GetWidth();
		if (MaxHeight < StAtt045[i].GetHeight()) MaxHeight = StAtt045[i].GetHeight();
		if (MaxxCor > StAtt045[i].GetCX()) MaxxCor = StAtt045[i].GetCX();
		if (MaxyCor > StAtt045[i].GetCY()) MaxyCor = StAtt045[i].GetCY();
		if (ChoseX < StAtt045[i].GetCX() + StAtt045[i].GetWidth()) ChoseX = StAtt045[i].GetCX() + StAtt045[i].GetWidth();
		if (ChoseY < StAtt045[i].GetCY() + StAtt045[i].GetHeight()) ChoseY = StAtt045[i].GetCY() + StAtt045[i].GetHeight();
		
		sprintf_s(ID,80,"%sStAtt090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = StAtt090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
		if (MaxWidth < StAtt090[i].GetWidth()) MaxWidth = StAtt090[i].GetWidth();
		if (MaxHeight < StAtt090[i].GetHeight()) MaxHeight = StAtt090[i].GetHeight();
		if (MaxxCor > StAtt090[i].GetCX()) MaxxCor = StAtt090[i].GetCX();
		if (MaxyCor > StAtt090[i].GetCY()) MaxyCor = StAtt090[i].GetCY();
		if (ChoseX < StAtt090[i].GetCX() + StAtt090[i].GetWidth()) ChoseX = StAtt090[i].GetCX() + StAtt090[i].GetWidth();
		if (ChoseY < StAtt090[i].GetCY() + StAtt090[i].GetHeight()) ChoseY = StAtt090[i].GetCY() + StAtt090[i].GetHeight();
		
		sprintf_s(ID,80,"%sStAtt135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = StAtt135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
		if (MaxWidth < StAtt135[i].GetWidth()) MaxWidth = StAtt135[i].GetWidth();
		if (MaxHeight < StAtt135[i].GetHeight()) MaxHeight = StAtt135[i].GetHeight();
		if (MaxxCor > StAtt135[i].GetCX()) MaxxCor = StAtt135[i].GetCX();
		if (MaxyCor > StAtt135[i].GetCY()) MaxyCor = StAtt135[i].GetCY();
		if (ChoseX < StAtt135[i].GetCX() + StAtt135[i].GetWidth()) ChoseX = StAtt135[i].GetCX() + StAtt135[i].GetWidth();
		if (ChoseY < StAtt135[i].GetCY() + StAtt135[i].GetHeight()) ChoseY = StAtt135[i].GetCY() + StAtt135[i].GetHeight();
		
		sprintf_s(ID,80,"%sStAtt180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = StAtt180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
		if (MaxWidth < StAtt180[i].GetWidth()) MaxWidth = StAtt180[i].GetWidth();
		if (MaxHeight < StAtt180[i].GetHeight()) MaxHeight = StAtt180[i].GetHeight();
		if (MaxxCor > StAtt180[i].GetCX()) MaxxCor = StAtt180[i].GetCX();
		if (MaxyCor > StAtt180[i].GetCY()) MaxyCor = StAtt180[i].GetCY();
		if (ChoseX < StAtt180[i].GetCX() + StAtt180[i].GetWidth()) ChoseX = StAtt180[i].GetCX() + StAtt180[i].GetWidth();
		if (ChoseY < StAtt180[i].GetCY() + StAtt180[i].GetHeight()) ChoseY = StAtt180[i].GetCY() + StAtt180[i].GetHeight();
		
	}

   sprintf_s(ViewIDStMov,80,"%sStMov",ViewID);
	bestPal = CV2PalManager::GetInstance()->GetPal(ViewIDStMov, palID);
	iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces4; i++) 
   {
      sprintf_s(ID,80,"%sStMov000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = StMov000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
		if (MaxWidth < StMov000[i].GetWidth()) MaxWidth = StMov000[i].GetWidth();
		if (MaxHeight < StMov000[i].GetHeight()) MaxHeight = StMov000[i].GetHeight();
		if (MaxxCor > StMov000[i].GetCX()) MaxxCor = StMov000[i].GetCX();
		if (MaxyCor > StMov000[i].GetCY()) MaxyCor = StMov000[i].GetCY();
		if (ChoseX < StMov000[i].GetCX() + StMov000[i].GetWidth()) ChoseX = StMov000[i].GetCX() + StMov000[i].GetWidth();
		if (ChoseY < StMov000[i].GetCY() + StMov000[i].GetHeight()) ChoseY = StMov000[i].GetCY() + StMov000[i].GetHeight();

		sprintf_s(ID,80,"%sStMov045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = StMov045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
		if (MaxWidth < StMov045[i].GetWidth()) MaxWidth = StMov045[i].GetWidth();
		if (MaxHeight < StMov045[i].GetHeight()) MaxHeight = StMov045[i].GetHeight();
		if (MaxxCor > StMov045[i].GetCX()) MaxxCor = StMov045[i].GetCX();
		if (MaxyCor > StMov045[i].GetCY()) MaxyCor = StMov045[i].GetCY();
		if (ChoseX < StMov045[i].GetCX() + StMov045[i].GetWidth()) ChoseX = StMov045[i].GetCX() + StMov045[i].GetWidth();
		if (ChoseY < StMov045[i].GetCY() + StMov045[i].GetHeight()) ChoseY = StMov045[i].GetCY() + StMov045[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = StMov090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
		if (MaxWidth < StMov090[i].GetWidth()) MaxWidth = StMov090[i].GetWidth();
		if (MaxHeight < StMov090[i].GetHeight()) MaxHeight = StMov090[i].GetHeight();
		if (MaxxCor > StMov090[i].GetCX()) MaxxCor = StMov090[i].GetCX();
		if (MaxyCor > StMov090[i].GetCY()) MaxyCor = StMov090[i].GetCY();
		if (ChoseX < StMov090[i].GetCX() + StMov090[i].GetWidth()) ChoseX = StMov090[i].GetCX() + StMov090[i].GetWidth();
		if (ChoseY < StMov090[i].GetCY() + StMov090[i].GetHeight()) ChoseY = StMov090[i].GetCY() + StMov090[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = StMov135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
		if (MaxWidth < StMov135[i].GetWidth()) MaxWidth = StMov135[i].GetWidth();
		if (MaxHeight < StMov135[i].GetHeight()) MaxHeight = StMov135[i].GetHeight();
		if (MaxxCor > StMov135[i].GetCX()) MaxxCor = StMov135[i].GetCX();
		if (MaxyCor > StMov135[i].GetCY()) MaxyCor = StMov135[i].GetCY();
		if (ChoseX < StMov135[i].GetCX() + StMov135[i].GetWidth()) ChoseX = StMov135[i].GetCX() + StMov135[i].GetWidth();
		if (ChoseY < StMov135[i].GetCY() + StMov135[i].GetHeight()) ChoseY = StMov135[i].GetCY() + StMov135[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = StMov180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
		if (MaxWidth < StMov180[i].GetWidth()) MaxWidth = StMov180[i].GetWidth();
		if (MaxHeight < StMov180[i].GetHeight()) MaxHeight = StMov180[i].GetHeight();
		if (MaxxCor > StMov180[i].GetCX()) MaxxCor = StMov180[i].GetCX();
		if (MaxyCor > StMov180[i].GetCY()) MaxyCor = StMov180[i].GetCY();
		if (ChoseX < StMov180[i].GetCX() + StMov180[i].GetWidth()) ChoseX = StMov180[i].GetCX() + StMov180[i].GetWidth();
		if (ChoseY < StMov180[i].GetCY() + StMov180[i].GetHeight()) ChoseY = StMov180[i].GetCY() + StMov180[i].GetHeight();
		
	}


	fTransparent = View000[0].GetfTransparent();
	TransColor = View000[0].GetTransColor();
   if ((MaxxCor % 2)) {
      MaxxCor--;
      MaxWidth++;
   }
   if (!(MaxyCor % 2)) {
      MaxyCor--;
      MaxHeight++;
   }

	MaxWidth += ChoseX - (MaxxCor + MaxWidth);
	MaxHeight += ChoseY - (MaxyCor + MaxHeight);

   if (Type == 3)
      Type = 0;
}


void Sprite3D::LoadSprite3D(int faces, int faces4, int faces5, char *ViewID, int type, int stMov, int palID) {
   if (bAlreadyLoad)
		return;
	bAlreadyLoad = TRUE;


   LastAccess = timeGetTime();
   
   // Load a 3D Sprite
	char ID[80];
	
	Type = type;
	faces /= FRAMING;
	Faces = faces;
	FacesA = 0;
   FacesR = 0;  //NMNMNM_Faces
   FacesC = faces5;
   FacesStA = 0;
   FacesStM = faces4;
   StMov = stMov;
	StAtt = 0;
	
   View000 = new CV2Sprite [faces];
	View045 = new CV2Sprite [faces];
	View090 = new CV2Sprite [faces];
	View135 = new CV2Sprite [faces];
	View180 = new CV2Sprite [faces];

   Corpse = new CV2Sprite[faces5];

   if (StMov == -1) {
      StMov000 = new CV2Sprite [faces4];
      StMov045 = new CV2Sprite [faces4];
      StMov090 = new CV2Sprite [faces4];
      StMov135 = new CV2Sprite [faces4];
      StMov180 = new CV2Sprite [faces4];
   }

   MaxxCor = 1000;
	MaxyCor = 1000;
	int ChoseX = 0;
	int ChoseY = 0;
   int iBC = 0;

   int i = 0; 
   for (i = 0; i < faces5; i++) 
   {
      sprintf_s(ID,80,"%sC-%c",ViewID,i+'a');
      iBC = Corpse[i].CreateSprite(ID, iBC, palID);
   }
	

	int iB000 = 0, iB045 = 0, iB090 = 0, iB135 = 0, iB180 = 0;
   
   for (i = 0; i < faces; i++) 
   {
      sprintf_s(ID,80,"%s000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = View000[i].CreateSprite(ID, iB000, palID);
		if (MaxWidth < View000[i].GetWidth()) MaxWidth = View000[i].GetWidth();
		if (MaxHeight < View000[i].GetHeight()) MaxHeight = View000[i].GetHeight();
		if (MaxxCor > View000[i].GetCX()) MaxxCor = View000[i].GetCX();
		if (MaxyCor > View000[i].GetCY()) MaxyCor = View000[i].GetCY();
		if (ChoseX < View000[i].GetCX() + View000[i].GetWidth()) ChoseX = View000[i].GetCX() + View000[i].GetWidth();
		if (ChoseY < View000[i].GetCY() + View000[i].GetHeight()) ChoseY = View000[i].GetCY() + View000[i].GetHeight();

		sprintf_s(ID,80,"%s045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = View045[i].CreateSprite(ID, iB045, palID);
		if (MaxWidth < View045[i].GetWidth()) MaxWidth = View045[i].GetWidth();
		if (MaxHeight < View045[i].GetHeight()) MaxHeight = View045[i].GetHeight();
		if (MaxxCor > View045[i].GetCX()) MaxxCor = View045[i].GetCX();
		if (MaxyCor > View045[i].GetCY()) MaxyCor = View045[i].GetCY();
		if (ChoseX < View045[i].GetCX() + View045[i].GetWidth()) ChoseX = View045[i].GetCX() + View045[i].GetWidth();
		if (ChoseY < View045[i].GetCY() + View045[i].GetHeight()) ChoseY = View045[i].GetCY() + View045[i].GetHeight();
		
		sprintf_s(ID,80,"%s090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = View090[i].CreateSprite(ID, iB090, palID);
		if (MaxWidth < View090[i].GetWidth()) MaxWidth = View090[i].GetWidth();
		if (MaxHeight < View090[i].GetHeight()) MaxHeight = View090[i].GetHeight();
		if (MaxxCor > View090[i].GetCX()) MaxxCor = View090[i].GetCX();
		if (MaxyCor > View090[i].GetCY()) MaxyCor = View090[i].GetCY();
		if (ChoseX < View090[i].GetCX() + View090[i].GetWidth()) ChoseX = View090[i].GetCX() + View090[i].GetWidth();
		if (ChoseY < View090[i].GetCY() + View090[i].GetHeight()) ChoseY = View090[i].GetCY() + View090[i].GetHeight();
		
		sprintf_s(ID,80,"%s135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = View135[i].CreateSprite(ID, iB135, palID);
		if (MaxWidth < View135[i].GetWidth()) MaxWidth = View135[i].GetWidth();
		if (MaxHeight < View135[i].GetHeight()) MaxHeight = View135[i].GetHeight();
		if (MaxxCor > View135[i].GetCX()) MaxxCor = View135[i].GetCX();
		if (MaxyCor > View135[i].GetCY()) MaxyCor = View135[i].GetCY();
		if (ChoseX < View135[i].GetCX() + View135[i].GetWidth()) ChoseX = View135[i].GetCX() + View135[i].GetWidth();
		if (ChoseY < View135[i].GetCY() + View135[i].GetHeight()) ChoseY = View135[i].GetCY() + View135[i].GetHeight();
		
		sprintf_s(ID,80,"%s180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = View180[i].CreateSprite(ID, iB180, palID);
		if (MaxWidth < View180[i].GetWidth()) MaxWidth = View180[i].GetWidth();
		if (MaxHeight < View180[i].GetHeight()) MaxHeight = View180[i].GetHeight();
		if (MaxxCor > View180[i].GetCX()) MaxxCor = View180[i].GetCX();
		if (MaxyCor > View180[i].GetCY()) MaxyCor = View180[i].GetCY();
		if (ChoseX < View180[i].GetCX() + View180[i].GetWidth()) ChoseX = View180[i].GetCX() + View180[i].GetWidth();
		if (ChoseY < View180[i].GetCY() + View180[i].GetHeight()) ChoseY = View180[i].GetCY() + View180[i].GetHeight();
		
   }

	iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces4; i++) 
   {
      sprintf_s(ID,80,"%sStMov000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = StMov000[i].CreateSprite(ID, iB000, palID);
		if (MaxWidth < StMov000[i].GetWidth()) MaxWidth = StMov000[i].GetWidth();
		if (MaxHeight < StMov000[i].GetHeight()) MaxHeight = StMov000[i].GetHeight();
		if (MaxxCor > StMov000[i].GetCX()) MaxxCor = StMov000[i].GetCX();
		if (MaxyCor > StMov000[i].GetCY()) MaxyCor = StMov000[i].GetCY();
		if (ChoseX < StMov000[i].GetCX() + StMov000[i].GetWidth()) ChoseX = StMov000[i].GetCX() + StMov000[i].GetWidth();
		if (ChoseY < StMov000[i].GetCY() + StMov000[i].GetHeight()) ChoseY = StMov000[i].GetCY() + StMov000[i].GetHeight();

		sprintf_s(ID,80,"%sStMov045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = StMov045[i].CreateSprite(ID, iB045, palID);
		if (MaxWidth < StMov045[i].GetWidth()) MaxWidth = StMov045[i].GetWidth();
		if (MaxHeight < StMov045[i].GetHeight()) MaxHeight = StMov045[i].GetHeight();
		if (MaxxCor > StMov045[i].GetCX()) MaxxCor = StMov045[i].GetCX();
		if (MaxyCor > StMov045[i].GetCY()) MaxyCor = StMov045[i].GetCY();
		if (ChoseX < StMov045[i].GetCX() + StMov045[i].GetWidth()) ChoseX = StMov045[i].GetCX() + StMov045[i].GetWidth();
		if (ChoseY < StMov045[i].GetCY() + StMov045[i].GetHeight()) ChoseY = StMov045[i].GetCY() + StMov045[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = StMov090[i].CreateSprite(ID, iB090, palID);
		if (MaxWidth < StMov090[i].GetWidth()) MaxWidth = StMov090[i].GetWidth();
		if (MaxHeight < StMov090[i].GetHeight()) MaxHeight = StMov090[i].GetHeight();
		if (MaxxCor > StMov090[i].GetCX()) MaxxCor = StMov090[i].GetCX();
		if (MaxyCor > StMov090[i].GetCY()) MaxyCor = StMov090[i].GetCY();
		if (ChoseX < StMov090[i].GetCX() + StMov090[i].GetWidth()) ChoseX = StMov090[i].GetCX() + StMov090[i].GetWidth();
		if (ChoseY < StMov090[i].GetCY() + StMov090[i].GetHeight()) ChoseY = StMov090[i].GetCY() + StMov090[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = StMov135[i].CreateSprite(ID, iB135, palID);
		if (MaxWidth < StMov135[i].GetWidth()) MaxWidth = StMov135[i].GetWidth();
		if (MaxHeight < StMov135[i].GetHeight()) MaxHeight = StMov135[i].GetHeight();
		if (MaxxCor > StMov135[i].GetCX()) MaxxCor = StMov135[i].GetCX();
		if (MaxyCor > StMov135[i].GetCY()) MaxyCor = StMov135[i].GetCY();
		if (ChoseX < StMov135[i].GetCX() + StMov135[i].GetWidth()) ChoseX = StMov135[i].GetCX() + StMov135[i].GetWidth();
		if (ChoseY < StMov135[i].GetCY() + StMov135[i].GetHeight()) ChoseY = StMov135[i].GetCY() + StMov135[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = StMov180[i].CreateSprite(ID, iB180, palID);
		if (MaxWidth < StMov180[i].GetWidth()) MaxWidth = StMov180[i].GetWidth();
		if (MaxHeight < StMov180[i].GetHeight()) MaxHeight = StMov180[i].GetHeight();
		if (MaxxCor > StMov180[i].GetCX()) MaxxCor = StMov180[i].GetCX();
		if (MaxyCor > StMov180[i].GetCY()) MaxyCor = StMov180[i].GetCY();
		if (ChoseX < StMov180[i].GetCX() + StMov180[i].GetWidth()) ChoseX = StMov180[i].GetCX() + StMov180[i].GetWidth();
		if (ChoseY < StMov180[i].GetCY() + StMov180[i].GetHeight()) ChoseY = StMov180[i].GetCY() + StMov180[i].GetHeight();
		
	}



   fTransparent = View000[0].GetfTransparent();
	TransColor = View000[0].GetTransColor();
   if ((MaxxCor + MaxyCor) % 2) {
      MaxxCor--;
      MaxWidth++;
   }
	MaxWidth += ChoseX - (MaxxCor + MaxWidth);
	MaxHeight += ChoseY - (MaxyCor + MaxHeight);

   if (Type == 3)
      Type = 0;
}

inline bool IsRangedObject( TFCObject *obj );

void Sprite3D::DrawSprite3D(int Direction, int Frame, int X, int Y, BYTE Stand, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *Object,Sprite3D *pMask) {
	LastAccess = timeGetTime();
   V2SPRITEFX FX;
   ZeroMemory(&FX, sizeof(FX));
   FX.lpClipRect = Clip;
   FX.lpDirectDrawSurface = Surface;
   if (bOutlineColor > 100 && bOutlineColor != 150)
      bOutlineColor = 100;
   if (Object)
      FX.dwFX |= Object->bInvisible ? FX_NODRAW : NULL;
   if (boOutline) {
      FX.dwFX |= FX_OUTLINE;
		if (bOutlineColor == 150) {
			FX.dwOutlineColor = SYSTEM_OUTLINE;
		} else {
         float dFraction = (float)bOutlineColor/100;
         if (dFraction >= 0.500) {
            FX.dwOutlineColor = ((int)((1.000-dFraction) * lpDXDirectDraw->wRMask * 2) & lpDXDirectDraw->wRMask) + (lpDXDirectDraw->wGMask);
         } else {
            FX.dwOutlineColor = (lpDXDirectDraw->wRMask) + ((int)((dFraction)*lpDXDirectDraw->wGMask * 2) & lpDXDirectDraw->wGMask);
         }
		}
   }
   if ((Stand == ST_ATTACK_STANDING || Stand == ST_ATTACK) && FacesA && View045) {
    if (StAtt > -1) {
         switch (Direction) {
            case 1: 
                if( IsRangedObject( Object ) && Rng045)
                {
                    Rng045[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng045[StAtt]:NULL);
                }else{
                    Att045[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att045[StAtt]:NULL);
                }
            break;
            case 2: 
                if( IsRangedObject( Object ) && Rng000){
                  Rng000[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng000[StAtt]:NULL);
                }else{
                   Att000[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att000[StAtt]:NULL); 
                } 
            break;
		      case 3: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
                if( IsRangedObject( Object ) && Rng045){
                    Rng045[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng045[StAtt]:NULL);
                }else{
                    Att045[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att045[StAtt]:NULL);
                }
            break;
            case 4:
                if( IsRangedObject( Object ) && Rng090){
                   Rng090[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng090[StAtt]:NULL);
                }else{
                   Att090[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att090[StAtt]:NULL); 
                }
            break;
		      case 6: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
                if( IsRangedObject( Object ) && Rng090){
                    Rng090[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng090[StAtt]:NULL);
                }else{
                    Att090[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att090[StAtt]:NULL);
                }
            break;
            case 7: 
                if( IsRangedObject( Object ) && Rng135){
                    Rng135[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng135[StAtt]:NULL);
                }else{
                    Att135[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att135[StAtt]:NULL);
                }
            break;
            case 8: 
                if( IsRangedObject( Object )&& Rng180 ){
                    Rng180[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng180[StAtt]:NULL);
                }else{
                    Att180[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att180[StAtt]:NULL); 
                }
            break;
		      case 9: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
                if( IsRangedObject( Object) && Rng135){
                     Rng135[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng135[StAtt]:NULL);
                }else{
                    Att135[StAtt].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att135[StAtt]:NULL);
                }
            break;
         }
      } else {
       if(Frame >= FacesA)
          Frame = 0;

         switch (Direction) {
            case 1: 
               StAtt045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StAtt045[Frame]:NULL); 
            break;
            case 2: 
               StAtt000[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StAtt000[Frame]:NULL); 
            break;
		      case 3: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               StAtt045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StAtt045[Frame]:NULL);
            break;
            case 4: 
               StAtt090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StAtt090[Frame]:NULL); 
            break;
		      case 6: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               StAtt090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StAtt090[Frame]:NULL);
            break;
            case 7: 
               StAtt135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StAtt135[Frame]:NULL); 
            break;
            case 8: 
               StAtt180[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StAtt180[Frame]:NULL); 
            break;
		      case 9: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               StAtt135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StAtt135[Frame]:NULL);
            break;
         }
      }
   } else if (Stand == ST_STANDING && View045) {
    if (StMov > -1) {
         switch (Direction) {
            case 1: 
               View045[StMov].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View045[StMov]:NULL); 
            break;
            case 2: 
               View000[StMov].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View000[StMov]:NULL); 
            break;
		      case 3: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               View045[StMov].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View045[StMov]:NULL);
            break;
            case 4: 
               View090[StMov].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View090[StMov]:NULL); 
            break;
		      case 6: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               View090[StMov].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View090[StMov]:NULL);
            break;
            case 7: 
               View135[StMov].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View135[StMov]:NULL); 
            break;
            case 8: 
               View180[StMov].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View180[StMov]:NULL); 
            break;
		      case 9: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               View135[StMov].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View135[StMov]:NULL);
            break;
         }
      } else {
       if(Frame >= FacesStM)
          Frame = 0;

         switch (Direction) {
            case 1: 
               StMov045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StMov045[Frame]:NULL); 
            break;
            case 2: 
               StMov000[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StMov000[Frame]:NULL); 
            break;
		      case 3: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               StMov045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StMov045[Frame]:NULL);
            break;
            case 4: 
               StMov090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StMov090[Frame]:NULL); 
            break;
		      case 6: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               StMov090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StMov090[Frame]:NULL);
            break;
            case 7: 
               StMov135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StMov135[Frame]:NULL); 
            break;
            case 8:
               StMov180[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StMov180[Frame]:NULL); 
            break;
		      case 9: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               StMov135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->StMov135[Frame]:NULL);
            break;
         }
      }
   } else {
      if (View045) {
         if(Frame >= Faces)
          Frame = 0;
         switch (Direction) {
            case 1: 
               View045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View045[Frame]:NULL); 
            break;
	         case 2: 
               View000[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View000[Frame]:NULL); 
            break;
		      case 3: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               View045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View045[Frame]:NULL); 
            break;
		      case 4: 
               View090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View090[Frame]:NULL); 
            break;
		      case 6: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               View090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View090[Frame]:NULL);
            break;
		      case 7: 
               View135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View135[Frame]:NULL); 
            break;
		      case 8: 
               View180[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View180[Frame]:NULL); 
            break;
		      case 9: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               View135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->View135[Frame]:NULL);
            break;
	      }
      } else 
      {
         View000[Frame].DrawSpriteN(X, Y, &FX);
      }
   }
}

void Sprite3D::DrawSprite3DA(int Direction, int Frame, int X, int Y, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *Object,Sprite3D *pMask) {
   if (FacesA) {
	LastAccess = timeGetTime();
   V2SPRITEFX FX;                                                                                    
   ZeroMemory(&FX, sizeof(FX));
   FX.lpClipRect = Clip;
   FX.lpDirectDrawSurface = Surface;
   if (Object)
      FX.dwFX |= Object->bInvisible ? FX_NODRAW : NULL;
   if (boOutline) {
      FX.dwFX |= FX_OUTLINE;
		if (bOutlineColor == 150) {
			FX.dwOutlineColor = SYSTEM_OUTLINE;
		} else {
         float dFraction = (float)bOutlineColor/100;
         if (dFraction >= 0.500) {
            FX.dwOutlineColor = ((int)((1.000-dFraction) * lpDXDirectDraw->wRMask * 2) & lpDXDirectDraw->wRMask) + (lpDXDirectDraw->wGMask);
         } else {
            FX.dwOutlineColor = (lpDXDirectDraw->wRMask) + ((int)((dFraction)*lpDXDirectDraw->wGMask * 2) & lpDXDirectDraw->wGMask);
         }
		}
   }
   if (View045) { //NMNMNMNM   NMNMNMNM
      switch (Direction) {
      	case 1: 
            Att045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att045[Frame]:NULL); 
         break;
	      case 2: 
            Att000[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att000[Frame]:NULL); 
         break;
		   case 3: 
            FX.dwFX |= FX_LEFTRIGHTMIRROR;
            Att045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att045[Frame]:NULL);
         break;
	      case 4: 
            Att090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att090[Frame]:NULL); 
         break;
         case 6: 
            FX.dwFX |= FX_LEFTRIGHTMIRROR;
            Att090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att090[Frame]:NULL);
         break;
	      case 7: 
            Att135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att135[Frame]:NULL); 
         break;
	      case 8: 
            Att180[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att180[Frame]:NULL); 
         break;
	      case 9: 
            FX.dwFX |= FX_LEFTRIGHTMIRROR;
            Att135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att135[Frame]:NULL);
         break;
      }
   } 
   else 
   {
      Att000[Frame].DrawSpriteN(X, Y, &FX);
   }
	} 
   else {
	}
}

void Sprite3D::DrawSprite3DR(int Direction, int Frame, int X, int Y, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *Object,Sprite3D *pMask) {
   if (FacesR) 
   {
      LastAccess = timeGetTime();
      V2SPRITEFX FX;                                                                                    
      ZeroMemory(&FX, sizeof(FX));
      FX.lpClipRect = Clip;
      FX.lpDirectDrawSurface = Surface;
      if (Object)
         FX.dwFX |= Object->bInvisible ? FX_NODRAW : NULL;
      if (boOutline) 
      {
         FX.dwFX |= FX_OUTLINE;
         if (bOutlineColor == 150) 
         {
            FX.dwOutlineColor = SYSTEM_OUTLINE;
         } 
         else 
         {
            float dFraction = (float)bOutlineColor/100;
            if (dFraction >= 0.500) 
            {
               FX.dwOutlineColor = ((int)((1.000-dFraction) * lpDXDirectDraw->wRMask * 2) & lpDXDirectDraw->wRMask) + (lpDXDirectDraw->wGMask);
            } 
            else 
            {
               FX.dwOutlineColor = (lpDXDirectDraw->wRMask) + ((int)((dFraction)*lpDXDirectDraw->wGMask * 2) & lpDXDirectDraw->wGMask);
            }
         }
      }
      if (View045) 
      {
         switch (Direction) 
         {
            case 1: 
               //            Rng045Shd[Frame].DrawSpriteN(X, Y, &FX); 
               if(Rng045)
                  Rng045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng045[Frame]:NULL);
               else
                  Att045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att045[Frame]:NULL);

            break;
            case 2: 
               //            Rng045Shd[Frame].DrawSpriteN(X, Y, &FX); 
               if(Rng000)
                  Rng000[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng000[Frame]:NULL); 
               else
                  Att000[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att000[Frame]:NULL);
            break;
            case 3: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               //            Rng270Shd[Frame].DrawSpriteN(X, Y, &FX); 
               if(Rng045)
                  Rng045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng045[Frame]:NULL);
               else
                  Att045[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att045[Frame]:NULL);
            break;
            case 4: 
               //            Rng090Shd[Frame].DrawSpriteN(X, Y, &FX); 
               if(Rng090)
                  Rng090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng090[Frame]:NULL);
               else
                  Att090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att090[Frame]:NULL);
            break;
            case 6: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               //            Rng270Shd[Frame].DrawSpriteN(X, Y, &FX); 
               if(Rng090)
                  Rng090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng090[Frame]:NULL);
               else
                  Att090[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att090[Frame]:NULL);
            break;
            case 7: 
               //            Rng135Shd[Frame].DrawSpriteN(X, Y, &FX);
               if(Rng135)
                  Rng135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng135[Frame]:NULL); 
               else
                  Att135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att135[Frame]:NULL);
            break;
            case 8: 
               //            Rng180Shd[Frame].DrawSpriteN(X, Y, &FX); 
               if(Rng180)
                  Rng180[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng180[Frame]:NULL); 
               else
                  Att180[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att180[Frame]:NULL);
            break;
            case 9: 
               FX.dwFX |= FX_LEFTRIGHTMIRROR;
               //            Rng225Shd[Frame].DrawSpriteN(X, Y, &FX); 
               if(Rng135)
                  Rng135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Rng135[Frame]:NULL);
               else
                  Att135[Frame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Att135[Frame]:NULL);
            break;
         }
      } 
      else 
      {
         //      Rng000Shd[Frame].DrawSpriteN(X, Y, &FX);
         if(Rng000)
            Rng000[Frame].DrawSpriteN(X, Y, &FX);
      }
   } 
   else 
   {
   }
}


Random r(0, 1);

void Sprite3D::DrawCorpse(int CorpseFrame, int X, int Y, RECT *Clip, RECT *OutBound, RECT *Wrap, LPDIRECTDRAWSURFACE7 Surface, signed char Format, int AbsX, int AbsY, TFCObject *,Sprite3D *pMask) 
{
   LastAccess = timeGetTime();
   V2SPRITEFX FX;
   ZeroMemory(&FX, sizeof(FX));
   FX.lpClipRect = Clip;
   FX.lpDirectDrawSurface = Surface;
   if (r(AbsX*3072+AbsY)) 
      FX.dwFX = FX_LEFTRIGHTMIRROR;
	Corpse[CorpseFrame].DrawSpriteN(X, Y, &FX,(pMask)?&pMask->Corpse[CorpseFrame]:NULL);
}

void Sprite3D::LoadSpriteC(int faces, int faces2, int faces5, char *ViewID, int palID) {
   if (bAlreadyLoad)
		return;
	bAlreadyLoad = TRUE;

   LastAccess = timeGetTime();
   
   // Load a 3D Sprite
	char ID[80];
	char Temp2[2];
	Temp2[1] = 0;
	
	Type = 2;
	faces /= FRAMING;
	Faces = faces;
	FacesA = faces2;
   FacesR = 0; //NMNMNM_Faces
   FacesC = faces5;
   FacesStA = 0;
   FacesStM = 0;
   StMov = 0;
	StAtt = 0;
	
   View000 = new CV2Sprite [faces];
   Att000 = new CV2Sprite [faces2];
   View045 = 0;

   Corpse = new CV2Sprite[faces5];

   MaxxCor = 1000;
	MaxyCor = 1000;
	int ChoseX = 0;
	int ChoseY = 0;
   int iBC = 0;

   int i = 0; 
   for (i = 0; i < faces5; i++)
   {
      sprintf_s(ID,80,"%sC-%c",ViewID,i+'a');
      iBC = Corpse[i].CreateSprite(ID, iBC, palID);
   }
	
	
	int iB000 = 0;
   int leFrame = 0;
   for (i = 0; i < faces; i++) 
   {
		strcpy_s(ID,80, ViewID);
		if ((i*FRAMING)+1 < 100)
         strcat_s(ID,80, "0");
		if ((i*FRAMING)+1 < 10)
         strcat_s(ID,80, "0");
		if ((i*FRAMING)+1 < 1)
         strcat_s(ID,80, "0");
      leFrame = (i*FRAMING)+1;
      if (leFrame >= 100) {
         Temp2[0] = (leFrame/100)+'0';
         leFrame -= (leFrame/100)*100;
   		strcat_s(ID,80, Temp2);
      }
      if (leFrame >= 10) {
         Temp2[0] = (leFrame/10)+'0';
         leFrame -= (leFrame/10)*10;
   		strcat_s(ID,80, Temp2);
      }
      Temp2[0] = leFrame+'0';
		strcat_s(ID,80, Temp2);
		iB000 = View000[i].CreateSprite(ID, iB000, palID);
		if (MaxWidth < View000[i].GetWidth()) MaxWidth = View000[i].GetWidth();
		if (MaxHeight < View000[i].GetHeight()) MaxHeight = View000[i].GetHeight();
		if (MaxxCor > View000[i].GetCX()) MaxxCor = View000[i].GetCX();
		if (MaxyCor > View000[i].GetCY()) MaxyCor = View000[i].GetCY();
		if (ChoseX < View000[i].GetCX() + View000[i].GetWidth()) ChoseX = View000[i].GetCX() + View000[i].GetWidth();
		if (ChoseY < View000[i].GetCY() + View000[i].GetHeight()) ChoseY = View000[i].GetCY() + View000[i].GetHeight();
      
   }

	iB000 = 0;
   for (i = 0; i < faces2; i++) 
   {
		strcpy_s(ID,80, ViewID);
		strcat_s(ID,80, "a");
      if ((i*FRAMING)+1 < 10)
         strcat_s(ID,80, "0");
		if ((i*FRAMING)+1 < 1)
         strcat_s(ID,80, "0");
      leFrame = (i*FRAMING)+1;
      if (leFrame >= 100) {
         Temp2[0] = (leFrame/100)+'0';
         leFrame -= (leFrame/100)*100;
   		strcat_s(ID,80, Temp2);
      }
      if (leFrame >= 10) {
         Temp2[0] = (leFrame/10)+'0';
         leFrame -= (leFrame/10)*10;
   		strcat_s(ID,80, Temp2);
      }
      Temp2[0] = leFrame+'0';
		strcat_s(ID,80, Temp2);
		iB000 = Att000[i].CreateSprite(ID, iB000, palID);
		if (MaxWidth < Att000[i].GetWidth()) MaxWidth = Att000[i].GetWidth();
		if (MaxHeight < Att000[i].GetHeight()) MaxHeight = Att000[i].GetHeight();
		if (MaxxCor > Att000[i].GetCX()) MaxxCor = Att000[i].GetCX();
		if (MaxyCor > Att000[i].GetCY()) MaxyCor = Att000[i].GetCY();
		if (ChoseX < Att000[i].GetCX() + Att000[i].GetWidth()) ChoseX = Att000[i].GetCX() + Att000[i].GetWidth();
		if (ChoseY < Att000[i].GetCY() + Att000[i].GetHeight()) ChoseY = Att000[i].GetCY() + Att000[i].GetHeight();

   }

	fTransparent = View000[0].GetfTransparent();
	TransColor = View000[0].GetTransColor();
   if ((MaxxCor + MaxyCor) % 2) {
      MaxxCor--;
      MaxWidth++;
   }
	MaxWidth += ChoseX - (MaxxCor + MaxWidth);
	MaxHeight += ChoseY - (MaxyCor + MaxHeight);
}

INFO *Sprite3D::GetCurrentVSF(int Direction, int Frame, BYTE Stand) {
   LastAccess = timeGetTime();
   INFO *Return = new INFO;
   if ((Stand == ST_ATTACK_STANDING || Stand == ST_ATTACK) && FacesA && View045) {
    if (StAtt > -1) {
         switch (Direction) {
            case 1: 
               Return->Width = Att045[StAtt].GetWidth();
               Return->Height = Att045[StAtt].GetHeight();
               Return->CX = Att045[StAtt].GetCX();
               Return->CY = Att045[StAtt].GetCY();
               return Return; 
            break;
            case 2: 
               Return->Width = Att000[StAtt].GetWidth();
               Return->Height = Att000[StAtt].GetHeight();
               Return->CX = Att000[StAtt].GetCX();
               Return->CY = Att000[StAtt].GetCY();
               return Return; 
            break;
            case 3:  
               Return->Width = Att045[StAtt].GetWidth();
               Return->Height = Att045[StAtt].GetHeight();
               Return->CX = Att045[StAtt].GetFCX();
               Return->CY = Att045[StAtt].GetFCY();
               return Return; 
            break;
            case 4: 
               Return->Width = Att090[StAtt].GetWidth();
               Return->Height = Att090[StAtt].GetHeight();
               Return->CX = Att090[StAtt].GetCX();
               Return->CY = Att090[StAtt].GetCY();
               return Return; 
            break;
            case 6: 
               Return->Width = Att090[StAtt].GetWidth();
               Return->Height = Att090[StAtt].GetHeight();
               Return->CX = Att090[StAtt].GetFCX();
               Return->CY = Att090[StAtt].GetFCY();
               return Return; 
            break;
            case 7: 
               Return->Width = Att135[StAtt].GetWidth();
               Return->Height = Att135[StAtt].GetHeight();
               Return->CX = Att135[StAtt].GetCX();
               Return->CY = Att135[StAtt].GetCY();
               return Return; 
            break;
            case 8: 
               Return->Width = Att180[StAtt].GetWidth();
               Return->Height = Att180[StAtt].GetHeight();
               Return->CX = Att180[StAtt].GetCX();
               Return->CY = Att180[StAtt].GetCY();
               return Return; 
            break;
            case 9: 
               Return->Width = Att135[StAtt].GetWidth();
               Return->Height = Att135[StAtt].GetHeight();
               Return->CX = Att135[StAtt].GetFCX();
               Return->CY = Att135[StAtt].GetFCY();
               return Return; 
            break;
         }
      } else {
       if(Frame >= FacesA)
          Frame = 0;

         switch (Direction) {
            case 1: 
               Return->Width = StAtt045[Frame].GetWidth();
               Return->Height = StAtt045[Frame].GetHeight();
               Return->CX = StAtt045[Frame].GetCX();
               Return->CY = StAtt045[Frame].GetCY();
               return Return; 
            break;
            case 2: 
               Return->Width = StAtt000[Frame].GetWidth();
               Return->Height = StAtt000[Frame].GetHeight();
               Return->CX = StAtt000[Frame].GetCX();
               Return->CY = StAtt000[Frame].GetCY();
               return Return; 
            break;
            case 3:  
               Return->Width = StAtt045[Frame].GetWidth();
               Return->Height = StAtt045[Frame].GetHeight();
               Return->CX = StAtt045[Frame].GetFCX();
               Return->CY = StAtt045[Frame].GetFCY();
               return Return; 
            break;
            case 4: 
               Return->Width = StAtt090[Frame].GetWidth();
               Return->Height = StAtt090[Frame].GetHeight();
               Return->CX = StAtt090[Frame].GetCX();
               Return->CY = StAtt090[Frame].GetCY();
               return Return; 
            break;
            case 6: 
               Return->Width = StAtt090[Frame].GetWidth();
               Return->Height = StAtt090[Frame].GetHeight();
               Return->CX = StAtt090[Frame].GetFCX();
               Return->CY = StAtt090[Frame].GetFCY();
               return Return; 
            break;
            case 7: 
               Return->Width = StAtt135[Frame].GetWidth();
               Return->Height = StAtt135[Frame].GetHeight();
               Return->CX = StAtt135[Frame].GetCX();
               Return->CY = StAtt135[Frame].GetCY();
               return Return; 
            break;
            case 8: 
               Return->Width = StAtt180[Frame].GetWidth();
               Return->Height = StAtt180[Frame].GetHeight();
               Return->CX = StAtt180[Frame].GetCX();
               Return->CY = StAtt180[Frame].GetCY();
               return Return; 
            break;
            case 9: 
               Return->Width = StAtt135[Frame].GetWidth();
               Return->Height = StAtt135[Frame].GetHeight();
               Return->CX = StAtt135[Frame].GetFCX();
               Return->CY = StAtt135[Frame].GetFCY();
               return Return; 
            break;
         }
      }
   } else if (Stand == ST_STANDING && View045) {
    if (StMov > -1) {
         switch (Direction) {
            case 1: 
               Return->Width = View045[StMov].GetWidth();
               Return->Height = View045[StMov].GetHeight();
               Return->CX = View045[StMov].GetCX();
               Return->CY = View045[StMov].GetCY();
               return Return; 
            break;
            case 2: 
               Return->Width = View000[StMov].GetWidth();
               Return->Height = View000[StMov].GetHeight();
               Return->CX = View000[StMov].GetCX();
               Return->CY = View000[StMov].GetCY();
               return Return; 
            break;
            case 3:  
               Return->Width = View045[StMov].GetWidth();
               Return->Height = View045[StMov].GetHeight();
               Return->CX = View045[StMov].GetFCX();
               Return->CY = View045[StMov].GetFCY();
               return Return; 
            break;
            case 4: 
               Return->Width = View090[StMov].GetWidth();
               Return->Height = View090[StMov].GetHeight();
               Return->CX = View090[StMov].GetCX();
               Return->CY = View090[StMov].GetCY();
               return Return; 
            break;
            case 6: 
               Return->Width = View090[StMov].GetWidth();
               Return->Height = View090[StMov].GetHeight();
               Return->CX = View090[StMov].GetFCX();
               Return->CY = View090[StMov].GetFCY();
               return Return; 
            break;
            case 7: 
               Return->Width = View135[StMov].GetWidth();
               Return->Height = View135[StMov].GetHeight();
               Return->CX = View135[StMov].GetCX();
               Return->CY = View135[StMov].GetCY();
               return Return; 
            break;
            case 8: 
               Return->Width = View180[StMov].GetWidth();
               Return->Height = View180[StMov].GetHeight();
               Return->CX = View180[StMov].GetCX();
               Return->CY = View180[StMov].GetCY();
               return Return; 
            break;
            case 9: 
               Return->Width = View135[StMov].GetWidth();
               Return->Height = View135[StMov].GetHeight();
               Return->CX = View135[StMov].GetFCX();
               Return->CY = View135[StMov].GetFCY();
               return Return; 
            break;
         }
      } else {
       if(Frame >= FacesStM)
          Frame = 0;
         switch (Direction) {
            case 1: 
               Return->Width = StMov045[Frame].GetWidth();
               Return->Height = StMov045[Frame].GetHeight();
               Return->CX = StMov045[Frame].GetCX();
               Return->CY = StMov045[Frame].GetCY();
               return Return; 
            break;
            case 2: 
               Return->Width = StMov000[Frame].GetWidth();
               Return->Height = StMov000[Frame].GetHeight();
               Return->CX = StMov000[Frame].GetCX();
               Return->CY = StMov000[Frame].GetCY();
               return Return; 
            break;
            case 3:  
               Return->Width = StMov045[Frame].GetWidth();
               Return->Height = StMov045[Frame].GetHeight();
               Return->CX = StMov045[Frame].GetFCX();
               Return->CY = StMov045[Frame].GetFCY();
               return Return; 
            break;
            case 4: 
               Return->Width = StMov090[Frame].GetWidth();
               Return->Height = StMov090[Frame].GetHeight();
               Return->CX = StMov090[Frame].GetCX();
               Return->CY = StMov090[Frame].GetCY();
               return Return; 
            break;
            case 6: 
               Return->Width = StMov090[Frame].GetWidth();
               Return->Height = StMov090[Frame].GetHeight();
               Return->CX = StMov090[Frame].GetFCX();
               Return->CY = StMov090[Frame].GetFCY();
               return Return; 
            break;
            case 7: 
               Return->Width = StMov135[Frame].GetWidth();
               Return->Height = StMov135[Frame].GetHeight();
               Return->CX = StMov135[Frame].GetCX();
               Return->CY = StMov135[Frame].GetCY();
               return Return; 
            break;
            case 8: 
               Return->Width = StMov180[Frame].GetWidth();
               Return->Height = StMov180[Frame].GetHeight();
               Return->CX = StMov180[Frame].GetCX();
               Return->CY = StMov180[Frame].GetCY();
               return Return; 
            break;
            case 9: 
               Return->Width = StMov135[Frame].GetWidth();
               Return->Height = StMov135[Frame].GetHeight();
               Return->CX = StMov135[Frame].GetFCX();
               Return->CY = StMov135[Frame].GetFCY();
               return Return; 
            break;
         }
      }
   } else {
      if (View045) {
         if(Frame >= Faces)
          Frame = 0;
         switch (Direction) {
            case 1: 
               Return->Width = View045[Frame].GetWidth();
               Return->Height = View045[Frame].GetHeight();
               Return->CX = View045[Frame].GetCX();
               Return->CY = View045[Frame].GetCY();
               return Return; 
            break;
            case 2: 
               Return->Width = View000[Frame].GetWidth();
               Return->Height = View000[Frame].GetHeight();
               Return->CX = View000[Frame].GetCX();
               Return->CY = View000[Frame].GetCY();
               return Return; 
            break;
            case 3:  
               Return->Width = View045[Frame].GetWidth();
               Return->Height = View045[Frame].GetHeight();
               Return->CX = View045[Frame].GetFCX();
               Return->CY = View045[Frame].GetFCY();
               return Return; 
            break;
            case 4: 
               Return->Width = View090[Frame].GetWidth();
               Return->Height = View090[Frame].GetHeight();
               Return->CX = View090[Frame].GetCX();
               Return->CY = View090[Frame].GetCY();
               return Return; 
            break;
            case 6: 
               Return->Width = View090[Frame].GetWidth();
               Return->Height = View090[Frame].GetHeight();
               Return->CX = View090[Frame].GetFCX();
               Return->CY = View090[Frame].GetFCY();
               return Return; 
            break;
            case 7: 
               Return->Width = View135[Frame].GetWidth();
               Return->Height = View135[Frame].GetHeight();
               Return->CX = View135[Frame].GetCX();
               Return->CY = View135[Frame].GetCY();
               return Return; 
            break;
            case 8: 
               Return->Width = View180[Frame].GetWidth();
               Return->Height = View180[Frame].GetHeight();
               Return->CX = View180[Frame].GetCX();
               Return->CY = View180[Frame].GetCY();
               return Return; 
            break;
            case 9: 
               Return->Width = View135[Frame].GetWidth();
               Return->Height = View135[Frame].GetHeight();
               Return->CX = View135[Frame].GetFCX();
               Return->CY = View135[Frame].GetFCY();
               return Return; 
            break;
	      }
      } else {
          Return->Width = View000[Frame].GetWidth();
          Return->Height = View000[Frame].GetHeight();
          Return->CX = View000[Frame].GetFCX();
          Return->CY = View000[Frame].GetFCY();
          return Return; 
      }
   }

   // Default.
   Return->Width = View000[Frame].GetWidth();
   Return->Height = View000[Frame].GetHeight();
   Return->CX = View000[Frame].GetFCX();
   Return->CY = View000[Frame].GetFCY();
   return Return; 
}
 
void Sprite3D::LoadSprite3D(int faces, int faces2, int faces3, int faces4, int faces5, int faces6, char *ViewID, int type, int stAtt, int stMov, int palID) {
	if (bAlreadyLoad)
		return;
	bAlreadyLoad = TRUE;
	
	
	LastAccess = timeGetTime();
	// Load a 3D Sprite
	char ID[80];
	
	Type = type;
	faces /= FRAMING;
	faces2 /= FRAMING;
	faces6 /= FRAMING;
	Faces = faces;
	FacesA = faces2;
	FacesC = faces5;
	FacesR = faces6;
	StAtt = stAtt;
	StMov = stMov;
	FacesStA = faces3;
	FacesStM = faces4;
	
	if (faces) {
		View000 = new CV2Sprite [faces];
		View045 = new CV2Sprite [faces];
		View090 = new CV2Sprite [faces];
		View135 = new CV2Sprite [faces];
		View180 = new CV2Sprite [faces];
	}
	
	if (faces2) {
		Att000 = new CV2Sprite [faces2];
		Att045 = new CV2Sprite [faces2];
		Att090 = new CV2Sprite [faces2];
		Att135 = new CV2Sprite [faces2];
		Att180 = new CV2Sprite [faces2];
	}
	
	if (faces6) {
		Rng000 = new CV2Sprite [faces6];
		Rng045 = new CV2Sprite [faces6];
		Rng090 = new CV2Sprite [faces6];
		Rng135 = new CV2Sprite [faces6];
		Rng180 = new CV2Sprite [faces6];
	}
	
	if (faces5)
		Corpse = new CV2Sprite[faces5];
	
	
	
	if (StAtt == -1) {
		StAtt000 = new CV2Sprite [faces3];
		StAtt045 = new CV2Sprite [faces3];
		StAtt090 = new CV2Sprite [faces3];
		StAtt135 = new CV2Sprite [faces3];
		StAtt180 = new CV2Sprite [faces3];
	}
	
	if (StMov == -1) {
		StMov000 = new CV2Sprite [faces4];
		StMov045 = new CV2Sprite [faces4];
		StMov090 = new CV2Sprite [faces4];
		StMov135 = new CV2Sprite [faces4];
		StMov180 = new CV2Sprite [faces4];
	}

	int ChoseX = 0;
	int ChoseY = 0;
	int iBC = 0;

	// New pallet loading system
	char ViewIDA[80];
	char ViewIDB[80];
	char ViewIDStAtt[80];
	char ViewIDStMov[80];
	char ViewIDC[80];

   sprintf_s(ViewIDC,80,"%sC",ViewID);
	LPBYTE bestPal = CV2PalManager::GetInstance()->GetPal(ViewIDC, palID);

   int i = 0;
	for (i = 0; i < faces5; i++) 
   {
      sprintf_s(ID,80,"%sC-%c",ViewID,i+'a');
		iBC = Corpse[i].CreateSpriteByPal(ID, iBC, bestPal,palID);
	}

   
	

	bestPal = CV2PalManager::GetInstance()->GetPal(ViewID, palID);
	int iB000 = 0, iB045 = 0, iB090 = 0, iB135 = 0, iB180 = 0;
   for (i = 0; i < faces; i++) 
   {
      sprintf_s(ID,80,"%s000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = View000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
		if (MaxWidth < View000[i].GetWidth()) MaxWidth = View000[i].GetWidth();
		if (MaxHeight < View000[i].GetHeight()) MaxHeight = View000[i].GetHeight();
		if (MaxxCor > View000[i].GetCX()) MaxxCor = View000[i].GetCX();
		if (MaxyCor > View000[i].GetCY()) MaxyCor = View000[i].GetCY();
		if (ChoseX < View000[i].GetCX() + View000[i].GetWidth()) ChoseX = View000[i].GetCX() + View000[i].GetWidth();
		if (ChoseY < View000[i].GetCY() + View000[i].GetHeight()) ChoseY = View000[i].GetCY() + View000[i].GetHeight();

		sprintf_s(ID,80,"%s045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = View045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
		if (MaxWidth < View045[i].GetWidth()) MaxWidth = View045[i].GetWidth();
		if (MaxHeight < View045[i].GetHeight()) MaxHeight = View045[i].GetHeight();
		if (MaxxCor > View045[i].GetCX()) MaxxCor = View045[i].GetCX();
		if (MaxyCor > View045[i].GetCY()) MaxyCor = View045[i].GetCY();
		if (ChoseX < View045[i].GetCX() + View045[i].GetWidth()) ChoseX = View045[i].GetCX() + View045[i].GetWidth();
		if (ChoseY < View045[i].GetCY() + View045[i].GetHeight()) ChoseY = View045[i].GetCY() + View045[i].GetHeight();
		
		sprintf_s(ID,80,"%s090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = View090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
		if (MaxWidth < View090[i].GetWidth()) MaxWidth = View090[i].GetWidth();
		if (MaxHeight < View090[i].GetHeight()) MaxHeight = View090[i].GetHeight();
		if (MaxxCor > View090[i].GetCX()) MaxxCor = View090[i].GetCX();
		if (MaxyCor > View090[i].GetCY()) MaxyCor = View090[i].GetCY();
		if (ChoseX < View090[i].GetCX() + View090[i].GetWidth()) ChoseX = View090[i].GetCX() + View090[i].GetWidth();
		if (ChoseY < View090[i].GetCY() + View090[i].GetHeight()) ChoseY = View090[i].GetCY() + View090[i].GetHeight();
		
		sprintf_s(ID,80,"%s135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = View135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
		if (MaxWidth < View135[i].GetWidth()) MaxWidth = View135[i].GetWidth();
		if (MaxHeight < View135[i].GetHeight()) MaxHeight = View135[i].GetHeight();
		if (MaxxCor > View135[i].GetCX()) MaxxCor = View135[i].GetCX();
		if (MaxyCor > View135[i].GetCY()) MaxyCor = View135[i].GetCY();
		if (ChoseX < View135[i].GetCX() + View135[i].GetWidth()) ChoseX = View135[i].GetCX() + View135[i].GetWidth();
		if (ChoseY < View135[i].GetCY() + View135[i].GetHeight()) ChoseY = View135[i].GetCY() + View135[i].GetHeight();
		
		sprintf_s(ID,80,"%s180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = View180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
		if (MaxWidth < View180[i].GetWidth()) MaxWidth = View180[i].GetWidth();
		if (MaxHeight < View180[i].GetHeight()) MaxHeight = View180[i].GetHeight();
		if (MaxxCor > View180[i].GetCX()) MaxxCor = View180[i].GetCX();
		if (MaxyCor > View180[i].GetCY()) MaxyCor = View180[i].GetCY();
		if (ChoseX < View180[i].GetCX() + View180[i].GetWidth()) ChoseX = View180[i].GetCX() + View180[i].GetWidth();
		if (ChoseY < View180[i].GetCY() + View180[i].GetHeight()) ChoseY = View180[i].GetCY() + View180[i].GetHeight();
		
	}

   sprintf_s(ViewIDA,80,"%sA",ViewID);
	bestPal = CV2PalManager::GetInstance()->GetPal(ViewIDA, palID);
	iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces2; i++) 
   {
      if(Type != 3)
         sprintf_s(ID,80,"%sA000-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = Att000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
		if (MaxWidth < Att000[i].GetWidth()) MaxWidth = Att000[i].GetWidth();
		if (MaxHeight < Att000[i].GetHeight()) MaxHeight = Att000[i].GetHeight();
		if (MaxxCor > Att000[i].GetCX()) MaxxCor = Att000[i].GetCX();
		if (MaxyCor > Att000[i].GetCY()) MaxyCor = Att000[i].GetCY();
		if (ChoseX < Att000[i].GetCX() + Att000[i].GetWidth()) ChoseX = Att000[i].GetCX() + Att000[i].GetWidth();
		if (ChoseY < Att000[i].GetCY() + Att000[i].GetHeight()) ChoseY = Att000[i].GetCY() + Att000[i].GetHeight();

      if(Type != 3)
         sprintf_s(ID,80,"%sA045-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = Att045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
		if (MaxWidth < Att045[i].GetWidth()) MaxWidth = Att045[i].GetWidth();
		if (MaxHeight < Att045[i].GetHeight()) MaxHeight = Att045[i].GetHeight();
		if (MaxxCor > Att045[i].GetCX()) MaxxCor = Att045[i].GetCX();
		if (MaxyCor > Att045[i].GetCY()) MaxyCor = Att045[i].GetCY();
		if (ChoseX < Att045[i].GetCX() + Att045[i].GetWidth()) ChoseX = Att045[i].GetCX() + Att045[i].GetWidth();
		if (ChoseY < Att045[i].GetCY() + Att045[i].GetHeight()) ChoseY = Att045[i].GetCY() + Att045[i].GetHeight();
		
      if(Type != 3)
         sprintf_s(ID,80,"%sA090-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = Att090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
		if (MaxWidth < Att090[i].GetWidth()) MaxWidth = Att090[i].GetWidth();
		if (MaxHeight < Att090[i].GetHeight()) MaxHeight = Att090[i].GetHeight();
		if (MaxxCor > Att090[i].GetCX()) MaxxCor = Att090[i].GetCX();
		if (MaxyCor > Att090[i].GetCY()) MaxyCor = Att090[i].GetCY();
		if (ChoseX < Att090[i].GetCX() + Att090[i].GetWidth()) ChoseX = Att090[i].GetCX() + Att090[i].GetWidth();
		if (ChoseY < Att090[i].GetCY() + Att090[i].GetHeight()) ChoseY = Att090[i].GetCY() + Att090[i].GetHeight();
		
      if(Type != 3)
         sprintf_s(ID,80,"%sA135-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = Att135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
		if (MaxWidth < Att135[i].GetWidth()) MaxWidth = Att135[i].GetWidth();
		if (MaxHeight < Att135[i].GetHeight()) MaxHeight = Att135[i].GetHeight();
		if (MaxxCor > Att135[i].GetCX()) MaxxCor = Att135[i].GetCX();
		if (MaxyCor > Att135[i].GetCY()) MaxyCor = Att135[i].GetCY();
		if (ChoseX < Att135[i].GetCX() + Att135[i].GetWidth()) ChoseX = Att135[i].GetCX() + Att135[i].GetWidth();
		if (ChoseY < Att135[i].GetCY() + Att135[i].GetHeight()) ChoseY = Att135[i].GetCY() + Att135[i].GetHeight();
		
      if(Type != 3)
         sprintf_s(ID,80,"%sA180-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = Att180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
		if (MaxWidth < Att180[i].GetWidth()) MaxWidth = Att180[i].GetWidth();
		if (MaxHeight < Att180[i].GetHeight()) MaxHeight = Att180[i].GetHeight();
		if (MaxxCor > Att180[i].GetCX()) MaxxCor = Att180[i].GetCX();
		if (MaxyCor > Att180[i].GetCY()) MaxyCor = Att180[i].GetCY();
		if (ChoseX < Att180[i].GetCX() + Att180[i].GetWidth()) ChoseX = Att180[i].GetCX() + Att180[i].GetWidth();
		if (ChoseY < Att180[i].GetCY() + Att180[i].GetHeight()) ChoseY = Att180[i].GetCY() + Att180[i].GetHeight();
		
	}

   sprintf_s(ViewIDB,80,"%sB",ViewID);
	bestPal = CV2PalManager::GetInstance()->GetPal(ViewIDB, palID);
	iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces6; i++) 
   {
      if(Type != 3)
         sprintf_s(ID,80,"%sB000-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = Rng000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
		if (MaxWidth < Rng000[i].GetWidth()) MaxWidth = Rng000[i].GetWidth();
		if (MaxHeight < Rng000[i].GetHeight()) MaxHeight = Rng000[i].GetHeight();
		if (MaxxCor > Rng000[i].GetCX()) MaxxCor = Rng000[i].GetCX();
		if (MaxyCor > Rng000[i].GetCY()) MaxyCor = Rng000[i].GetCY();
		if (ChoseX < Rng000[i].GetCX() + Rng000[i].GetWidth()) ChoseX = Rng000[i].GetCX() + Rng000[i].GetWidth();
		if (ChoseY < Rng000[i].GetCY() + Rng000[i].GetHeight()) ChoseY = Rng000[i].GetCY() + Rng000[i].GetHeight();

      if(Type != 3)
         sprintf_s(ID,80,"%sB045-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = Rng045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
		if (MaxWidth < Rng045[i].GetWidth()) MaxWidth = Rng045[i].GetWidth();
		if (MaxHeight < Rng045[i].GetHeight()) MaxHeight = Rng045[i].GetHeight();
		if (MaxxCor > Rng045[i].GetCX()) MaxxCor = Rng045[i].GetCX();
		if (MaxyCor > Rng045[i].GetCY()) MaxyCor = Rng045[i].GetCY();
		if (ChoseX < Rng045[i].GetCX() + Rng045[i].GetWidth()) ChoseX = Rng045[i].GetCX() + Rng045[i].GetWidth();
		if (ChoseY < Rng045[i].GetCY() + Rng045[i].GetHeight()) ChoseY = Rng045[i].GetCY() + Rng045[i].GetHeight();
		
      if(Type != 3)
         sprintf_s(ID,80,"%sB090-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = Rng090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
		if (MaxWidth < Rng090[i].GetWidth()) MaxWidth = Rng090[i].GetWidth();
		if (MaxHeight < Rng090[i].GetHeight()) MaxHeight = Rng090[i].GetHeight();
		if (MaxxCor > Rng090[i].GetCX()) MaxxCor = Rng090[i].GetCX();
		if (MaxyCor > Rng090[i].GetCY()) MaxyCor = Rng090[i].GetCY();
		if (ChoseX < Rng090[i].GetCX() + Rng090[i].GetWidth()) ChoseX = Rng090[i].GetCX() + Rng090[i].GetWidth();
		if (ChoseY < Rng090[i].GetCY() + Rng090[i].GetHeight()) ChoseY = Rng090[i].GetCY() + Rng090[i].GetHeight();
		
      if(Type != 3)
         sprintf_s(ID,80,"%sB135-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = Rng135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
		if (MaxWidth < Rng135[i].GetWidth()) MaxWidth = Rng135[i].GetWidth();
		if (MaxHeight < Rng135[i].GetHeight()) MaxHeight = Rng135[i].GetHeight();
		if (MaxxCor > Rng135[i].GetCX()) MaxxCor = Rng135[i].GetCX();
		if (MaxyCor > Rng135[i].GetCY()) MaxyCor = Rng135[i].GetCY();
		if (ChoseX < Rng135[i].GetCX() + Rng135[i].GetWidth()) ChoseX = Rng135[i].GetCX() + Rng135[i].GetWidth();
		if (ChoseY < Rng135[i].GetCY() + Rng135[i].GetHeight()) ChoseY = Rng135[i].GetCY() + Rng135[i].GetHeight();
		
      if(Type != 3)
         sprintf_s(ID,80,"%sB180-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = Rng180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
		if (MaxWidth < Rng180[i].GetWidth()) MaxWidth = Rng180[i].GetWidth();
		if (MaxHeight < Rng180[i].GetHeight()) MaxHeight = Rng180[i].GetHeight();
		if (MaxxCor > Rng180[i].GetCX()) MaxxCor = Rng180[i].GetCX();
		if (MaxyCor > Rng180[i].GetCY()) MaxyCor = Rng180[i].GetCY();
		if (ChoseX < Rng180[i].GetCX() + Rng180[i].GetWidth()) ChoseX = Rng180[i].GetCX() + Rng180[i].GetWidth();
		if (ChoseY < Rng180[i].GetCY() + Rng180[i].GetHeight()) ChoseY = Rng180[i].GetCY() + Rng180[i].GetHeight();
		
	}

	sprintf_s(ViewIDStAtt,80,"%sStAtt", ViewID);
	bestPal = CV2PalManager::GetInstance()->GetPal(ViewIDStAtt, palID);
   iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces3; i++) 
   {
      sprintf_s(ID,80,"%sStAtt000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = StAtt000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
		if (MaxWidth < StAtt000[i].GetWidth()) MaxWidth = StAtt000[i].GetWidth();
		if (MaxHeight < StAtt000[i].GetHeight()) MaxHeight = StAtt000[i].GetHeight();
		if (MaxxCor > StAtt000[i].GetCX()) MaxxCor = StAtt000[i].GetCX();
		if (MaxyCor > StAtt000[i].GetCY()) MaxyCor = StAtt000[i].GetCY();
		if (ChoseX < StAtt000[i].GetCX() + StAtt000[i].GetWidth()) ChoseX = StAtt000[i].GetCX() + StAtt000[i].GetWidth();
		if (ChoseY < StAtt000[i].GetCY() + StAtt000[i].GetHeight()) ChoseY = StAtt000[i].GetCY() + StAtt000[i].GetHeight();

		sprintf_s(ID,80,"%sStAtt045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = StAtt045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
		if (MaxWidth < StAtt045[i].GetWidth()) MaxWidth = StAtt045[i].GetWidth();
		if (MaxHeight < StAtt045[i].GetHeight()) MaxHeight = StAtt045[i].GetHeight();
		if (MaxxCor > StAtt045[i].GetCX()) MaxxCor = StAtt045[i].GetCX();
		if (MaxyCor > StAtt045[i].GetCY()) MaxyCor = StAtt045[i].GetCY();
		if (ChoseX < StAtt045[i].GetCX() + StAtt045[i].GetWidth()) ChoseX = StAtt045[i].GetCX() + StAtt045[i].GetWidth();
		if (ChoseY < StAtt045[i].GetCY() + StAtt045[i].GetHeight()) ChoseY = StAtt045[i].GetCY() + StAtt045[i].GetHeight();
		
		sprintf_s(ID,80,"%sStAtt090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = StAtt090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
		if (MaxWidth < StAtt090[i].GetWidth()) MaxWidth = StAtt090[i].GetWidth();
		if (MaxHeight < StAtt090[i].GetHeight()) MaxHeight = StAtt090[i].GetHeight();
		if (MaxxCor > StAtt090[i].GetCX()) MaxxCor = StAtt090[i].GetCX();
		if (MaxyCor > StAtt090[i].GetCY()) MaxyCor = StAtt090[i].GetCY();
		if (ChoseX < StAtt090[i].GetCX() + StAtt090[i].GetWidth()) ChoseX = StAtt090[i].GetCX() + StAtt090[i].GetWidth();
		if (ChoseY < StAtt090[i].GetCY() + StAtt090[i].GetHeight()) ChoseY = StAtt090[i].GetCY() + StAtt090[i].GetHeight();
		
		sprintf_s(ID,80,"%sStAtt135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = StAtt135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
		if (MaxWidth < StAtt135[i].GetWidth()) MaxWidth = StAtt135[i].GetWidth();
		if (MaxHeight < StAtt135[i].GetHeight()) MaxHeight = StAtt135[i].GetHeight();
		if (MaxxCor > StAtt135[i].GetCX()) MaxxCor = StAtt135[i].GetCX();
		if (MaxyCor > StAtt135[i].GetCY()) MaxyCor = StAtt135[i].GetCY();
		if (ChoseX < StAtt135[i].GetCX() + StAtt135[i].GetWidth()) ChoseX = StAtt135[i].GetCX() + StAtt135[i].GetWidth();
		if (ChoseY < StAtt135[i].GetCY() + StAtt135[i].GetHeight()) ChoseY = StAtt135[i].GetCY() + StAtt135[i].GetHeight();
		
		sprintf_s(ID,80,"%sStAtt180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = StAtt180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
		if (MaxWidth < StAtt180[i].GetWidth()) MaxWidth = StAtt180[i].GetWidth();
		if (MaxHeight < StAtt180[i].GetHeight()) MaxHeight = StAtt180[i].GetHeight();
		if (MaxxCor > StAtt180[i].GetCX()) MaxxCor = StAtt180[i].GetCX();
		if (MaxyCor > StAtt180[i].GetCY()) MaxyCor = StAtt180[i].GetCY();
		if (ChoseX < StAtt180[i].GetCX() + StAtt180[i].GetWidth()) ChoseX = StAtt180[i].GetCX() + StAtt180[i].GetWidth();
		if (ChoseY < StAtt180[i].GetCY() + StAtt180[i].GetHeight()) ChoseY = StAtt180[i].GetCY() + StAtt180[i].GetHeight();
		
	}

   sprintf_s(ViewIDStMov,80,"%sStMov", ViewID);
	bestPal = CV2PalManager::GetInstance()->GetPal(ViewIDStMov, palID);
	iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces4; i++) 
   {
      sprintf_s(ID,80,"%sStMov000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = StMov000[i].CreateSpriteByPal(ID, iB000, bestPal,palID);
		if (MaxWidth < StMov000[i].GetWidth()) MaxWidth = StMov000[i].GetWidth();
		if (MaxHeight < StMov000[i].GetHeight()) MaxHeight = StMov000[i].GetHeight();
		if (MaxxCor > StMov000[i].GetCX()) MaxxCor = StMov000[i].GetCX();
		if (MaxyCor > StMov000[i].GetCY()) MaxyCor = StMov000[i].GetCY();
		if (ChoseX < StMov000[i].GetCX() + StMov000[i].GetWidth()) ChoseX = StMov000[i].GetCX() + StMov000[i].GetWidth();
		if (ChoseY < StMov000[i].GetCY() + StMov000[i].GetHeight()) ChoseY = StMov000[i].GetCY() + StMov000[i].GetHeight();

		sprintf_s(ID,80,"%sStMov045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = StMov045[i].CreateSpriteByPal(ID, iB045, bestPal,palID);
		if (MaxWidth < StMov045[i].GetWidth()) MaxWidth = StMov045[i].GetWidth();
		if (MaxHeight < StMov045[i].GetHeight()) MaxHeight = StMov045[i].GetHeight();
		if (MaxxCor > StMov045[i].GetCX()) MaxxCor = StMov045[i].GetCX();
		if (MaxyCor > StMov045[i].GetCY()) MaxyCor = StMov045[i].GetCY();
		if (ChoseX < StMov045[i].GetCX() + StMov045[i].GetWidth()) ChoseX = StMov045[i].GetCX() + StMov045[i].GetWidth();
		if (ChoseY < StMov045[i].GetCY() + StMov045[i].GetHeight()) ChoseY = StMov045[i].GetCY() + StMov045[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = StMov090[i].CreateSpriteByPal(ID, iB090, bestPal,palID);
		if (MaxWidth < StMov090[i].GetWidth()) MaxWidth = StMov090[i].GetWidth();
		if (MaxHeight < StMov090[i].GetHeight()) MaxHeight = StMov090[i].GetHeight();
		if (MaxxCor > StMov090[i].GetCX()) MaxxCor = StMov090[i].GetCX();
		if (MaxyCor > StMov090[i].GetCY()) MaxyCor = StMov090[i].GetCY();
		if (ChoseX < StMov090[i].GetCX() + StMov090[i].GetWidth()) ChoseX = StMov090[i].GetCX() + StMov090[i].GetWidth();
		if (ChoseY < StMov090[i].GetCY() + StMov090[i].GetHeight()) ChoseY = StMov090[i].GetCY() + StMov090[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = StMov135[i].CreateSpriteByPal(ID, iB135, bestPal,palID);
		if (MaxWidth < StMov135[i].GetWidth()) MaxWidth = StMov135[i].GetWidth();
		if (MaxHeight < StMov135[i].GetHeight()) MaxHeight = StMov135[i].GetHeight();
		if (MaxxCor > StMov135[i].GetCX()) MaxxCor = StMov135[i].GetCX();
		if (MaxyCor > StMov135[i].GetCY()) MaxyCor = StMov135[i].GetCY();
		if (ChoseX < StMov135[i].GetCX() + StMov135[i].GetWidth()) ChoseX = StMov135[i].GetCX() + StMov135[i].GetWidth();
		if (ChoseY < StMov135[i].GetCY() + StMov135[i].GetHeight()) ChoseY = StMov135[i].GetCY() + StMov135[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = StMov180[i].CreateSpriteByPal(ID, iB180, bestPal,palID);
		if (MaxWidth < StMov180[i].GetWidth()) MaxWidth = StMov180[i].GetWidth();
		if (MaxHeight < StMov180[i].GetHeight()) MaxHeight = StMov180[i].GetHeight();
		if (MaxxCor > StMov180[i].GetCX()) MaxxCor = StMov180[i].GetCX();
		if (MaxyCor > StMov180[i].GetCY()) MaxyCor = StMov180[i].GetCY();
		if (ChoseX < StMov180[i].GetCX() + StMov180[i].GetWidth()) ChoseX = StMov180[i].GetCX() + StMov180[i].GetWidth();
		if (ChoseY < StMov180[i].GetCY() + StMov180[i].GetHeight()) ChoseY = StMov180[i].GetCY() + StMov180[i].GetHeight();
		
	}


   
	fTransparent = View000[0].GetfTransparent();
	TransColor = View000[0].GetTransColor();
   if ((MaxxCor % 2)) {
      MaxxCor--;
      MaxWidth++;
   }
   if (!(MaxyCor % 2)) {
      MaxyCor--;
      MaxHeight++;
   }

	MaxWidth += ChoseX - (MaxxCor + MaxWidth);
	MaxHeight += ChoseY - (MaxyCor + MaxHeight);

   if (Type == 3)
      Type = 0;
}


void Sprite3D::LoadSprite3D2(int faces, int faces2, int faces3, int faces4, int faces5, char *ViewID, int type, int stAtt, int stMov, int palID) {
   if (bAlreadyLoad)
		return;
	bAlreadyLoad = TRUE;


   LastAccess = timeGetTime();
   // Load a 3D Sprite
	char ID[80];
	
	Type = type;
	faces /= FRAMING;
	faces2 /= FRAMING;
	Faces = faces;
	FacesA = faces2;
   FacesR = 0;  //NMNMNM_Faces
	FacesC = faces5;
   StAtt = stAtt;
   StMov = stMov;
   FacesStA = faces3;
   FacesStM = faces4;
	
	View000 = new CV2Sprite [faces];
	View045 = new CV2Sprite [faces];
	View090 = new CV2Sprite [faces];
	View135 = new CV2Sprite [faces];
	View180 = new CV2Sprite [faces];

	Att000 = new CV2Sprite [faces2];
	Att045 = new CV2Sprite [faces2];
	Att090 = new CV2Sprite [faces2];
	Att135 = new CV2Sprite [faces2];
	Att180 = new CV2Sprite [faces2];

   if (faces5)
		Corpse = new CV2Sprite[faces5];

	



   if (StAtt == -1) {
      StAtt000 = new CV2Sprite [faces3];
      StAtt045 = new CV2Sprite [faces3];
      StAtt090 = new CV2Sprite [faces3];
      StAtt135 = new CV2Sprite [faces3];
      StAtt180 = new CV2Sprite [faces3];

   }
   
   if (StMov == -1) {
      StMov000 = new CV2Sprite [faces4];
      StMov045 = new CV2Sprite [faces4];
      StMov090 = new CV2Sprite [faces4];
      StMov135 = new CV2Sprite [faces4];
      StMov180 = new CV2Sprite [faces4];
   }

	int ChoseX = 0;
	int ChoseY = 0;
   int iBC = 0;

   int i = 0;
   for (i = 0; i < faces5; i++) 
   {
      sprintf_s(ID,80,"%sC-%c",ViewID,i+'a');
      iBC = Corpse[i].CreateSprite(ID, iBC, palID);
   }
	
	
	int iB000 = 0, iB045 = 0, iB090 = 0, iB135 = 0, iB180 = 0;
   for (i = 0; i < faces; i++) 
   {
      sprintf_s(ID,80,"%s000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = View000[i].CreateSprite(ID, iB000, palID);
		if (MaxWidth < View000[i].GetWidth()) MaxWidth = View000[i].GetWidth();
		if (MaxHeight < View000[i].GetHeight()) MaxHeight = View000[i].GetHeight();
		if (MaxxCor > View000[i].GetCX()) MaxxCor = View000[i].GetCX();
		if (MaxyCor > View000[i].GetCY()) MaxyCor = View000[i].GetCY();
		if (ChoseX < View000[i].GetCX() + View000[i].GetWidth()) ChoseX = View000[i].GetCX() + View000[i].GetWidth();
		if (ChoseY < View000[i].GetCY() + View000[i].GetHeight()) ChoseY = View000[i].GetCY() + View000[i].GetHeight();

		sprintf_s(ID,80,"%s045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = View045[i].CreateSprite(ID, iB045, palID);
		if (MaxWidth < View045[i].GetWidth()) MaxWidth = View045[i].GetWidth();
		if (MaxHeight < View045[i].GetHeight()) MaxHeight = View045[i].GetHeight();
		if (MaxxCor > View045[i].GetCX()) MaxxCor = View045[i].GetCX();
		if (MaxyCor > View045[i].GetCY()) MaxyCor = View045[i].GetCY();
		if (ChoseX < View045[i].GetCX() + View045[i].GetWidth()) ChoseX = View045[i].GetCX() + View045[i].GetWidth();
		if (ChoseY < View045[i].GetCY() + View045[i].GetHeight()) ChoseY = View045[i].GetCY() + View045[i].GetHeight();
		
		sprintf_s(ID,80,"%s090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = View090[i].CreateSprite(ID, iB090, palID);
		if (MaxWidth < View090[i].GetWidth()) MaxWidth = View090[i].GetWidth();
		if (MaxHeight < View090[i].GetHeight()) MaxHeight = View090[i].GetHeight();
		if (MaxxCor > View090[i].GetCX()) MaxxCor = View090[i].GetCX();
		if (MaxyCor > View090[i].GetCY()) MaxyCor = View090[i].GetCY();
		if (ChoseX < View090[i].GetCX() + View090[i].GetWidth()) ChoseX = View090[i].GetCX() + View090[i].GetWidth();
		if (ChoseY < View090[i].GetCY() + View090[i].GetHeight()) ChoseY = View090[i].GetCY() + View090[i].GetHeight();
		
		sprintf_s(ID,80,"%s135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = View135[i].CreateSprite(ID, iB135, palID);
		if (MaxWidth < View135[i].GetWidth()) MaxWidth = View135[i].GetWidth();
		if (MaxHeight < View135[i].GetHeight()) MaxHeight = View135[i].GetHeight();
		if (MaxxCor > View135[i].GetCX()) MaxxCor = View135[i].GetCX();
		if (MaxyCor > View135[i].GetCY()) MaxyCor = View135[i].GetCY();
		if (ChoseX < View135[i].GetCX() + View135[i].GetWidth()) ChoseX = View135[i].GetCX() + View135[i].GetWidth();
		if (ChoseY < View135[i].GetCY() + View135[i].GetHeight()) ChoseY = View135[i].GetCY() + View135[i].GetHeight();
		
		sprintf_s(ID,80,"%s180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = View180[i].CreateSprite(ID, iB180, palID);
		if (MaxWidth < View180[i].GetWidth()) MaxWidth = View180[i].GetWidth();
		if (MaxHeight < View180[i].GetHeight()) MaxHeight = View180[i].GetHeight();
		if (MaxxCor > View180[i].GetCX()) MaxxCor = View180[i].GetCX();
		if (MaxyCor > View180[i].GetCY()) MaxyCor = View180[i].GetCY();
		if (ChoseX < View180[i].GetCX() + View180[i].GetWidth()) ChoseX = View180[i].GetCX() + View180[i].GetWidth();
		if (ChoseY < View180[i].GetCY() + View180[i].GetHeight()) ChoseY = View180[i].GetCY() + View180[i].GetHeight();
		
	}

	iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces2; i++) 
   {
      if (Type != 3)
         sprintf_s(ID,80,"%sA000-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = Att000[i].CreateSprite(ID, iB000, palID);
		if (MaxWidth < Att000[i].GetWidth()) MaxWidth = Att000[i].GetWidth();
		if (MaxHeight < Att000[i].GetHeight()) MaxHeight = Att000[i].GetHeight();
		if (MaxxCor > Att000[i].GetCX()) MaxxCor = Att000[i].GetCX();
		if (MaxyCor > Att000[i].GetCY()) MaxyCor = Att000[i].GetCY();
		if (ChoseX < Att000[i].GetCX() + Att000[i].GetWidth()) ChoseX = Att000[i].GetCX() + Att000[i].GetWidth();
		if (ChoseY < Att000[i].GetCY() + Att000[i].GetHeight()) ChoseY = Att000[i].GetCY() + Att000[i].GetHeight();

      if (Type != 3)
         sprintf_s(ID,80,"%sA045-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = Att045[i].CreateSprite(ID, iB045, palID);
		if (MaxWidth < Att045[i].GetWidth()) MaxWidth = Att045[i].GetWidth();
		if (MaxHeight < Att045[i].GetHeight()) MaxHeight = Att045[i].GetHeight();
		if (MaxxCor > Att045[i].GetCX()) MaxxCor = Att045[i].GetCX();
		if (MaxyCor > Att045[i].GetCY()) MaxyCor = Att045[i].GetCY();
		if (ChoseX < Att045[i].GetCX() + Att045[i].GetWidth()) ChoseX = Att045[i].GetCX() + Att045[i].GetWidth();
		if (ChoseY < Att045[i].GetCY() + Att045[i].GetHeight()) ChoseY = Att045[i].GetCY() + Att045[i].GetHeight();
		
      if (Type != 3)
         sprintf_s(ID,80,"%sA090-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = Att090[i].CreateSprite(ID, iB090, palID);
		if (MaxWidth < Att090[i].GetWidth()) MaxWidth = Att090[i].GetWidth();
		if (MaxHeight < Att090[i].GetHeight()) MaxHeight = Att090[i].GetHeight();
		if (MaxxCor > Att090[i].GetCX()) MaxxCor = Att090[i].GetCX();
		if (MaxyCor > Att090[i].GetCY()) MaxyCor = Att090[i].GetCY();
		if (ChoseX < Att090[i].GetCX() + Att090[i].GetWidth()) ChoseX = Att090[i].GetCX() + Att090[i].GetWidth();
		if (ChoseY < Att090[i].GetCY() + Att090[i].GetHeight()) ChoseY = Att090[i].GetCY() + Att090[i].GetHeight();
		
      if (Type != 3)
         sprintf_s(ID,80,"%sA135-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = Att135[i].CreateSprite(ID, iB135, palID);
		if (MaxWidth < Att135[i].GetWidth()) MaxWidth = Att135[i].GetWidth();
		if (MaxHeight < Att135[i].GetHeight()) MaxHeight = Att135[i].GetHeight();
		if (MaxxCor > Att135[i].GetCX()) MaxxCor = Att135[i].GetCX();
		if (MaxyCor > Att135[i].GetCY()) MaxyCor = Att135[i].GetCY();
		if (ChoseX < Att135[i].GetCX() + Att135[i].GetWidth()) ChoseX = Att135[i].GetCX() + Att135[i].GetWidth();
		if (ChoseY < Att135[i].GetCY() + Att135[i].GetHeight()) ChoseY = Att135[i].GetCY() + Att135[i].GetHeight();
		
      if (Type != 3)
         sprintf_s(ID,80,"%sA180-%c",ViewID,(i*FRAMING) + 'a');
      else
         sprintf_s(ID,80,"%s180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = Att180[i].CreateSprite(ID, iB180, palID);
		if (MaxWidth < Att180[i].GetWidth()) MaxWidth = Att180[i].GetWidth();
		if (MaxHeight < Att180[i].GetHeight()) MaxHeight = Att180[i].GetHeight();
		if (MaxxCor > Att180[i].GetCX()) MaxxCor = Att180[i].GetCX();
		if (MaxyCor > Att180[i].GetCY()) MaxyCor = Att180[i].GetCY();
		if (ChoseX < Att180[i].GetCX() + Att180[i].GetWidth()) ChoseX = Att180[i].GetCX() + Att180[i].GetWidth();
		if (ChoseY < Att180[i].GetCY() + Att180[i].GetHeight()) ChoseY = Att180[i].GetCY() + Att180[i].GetHeight();
		
	}

   iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces3; i++) 
   {
      sprintf_s(ID,80,"%sStAtt000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = StAtt000[i].CreateSprite(ID, iB000, palID);
		if (MaxWidth < StAtt000[i].GetWidth()) MaxWidth = StAtt000[i].GetWidth();
		if (MaxHeight < StAtt000[i].GetHeight()) MaxHeight = StAtt000[i].GetHeight();
		if (MaxxCor > StAtt000[i].GetCX()) MaxxCor = StAtt000[i].GetCX();
		if (MaxyCor > StAtt000[i].GetCY()) MaxyCor = StAtt000[i].GetCY();
		if (ChoseX < StAtt000[i].GetCX() + StAtt000[i].GetWidth()) ChoseX = StAtt000[i].GetCX() + StAtt000[i].GetWidth();
		if (ChoseY < StAtt000[i].GetCY() + StAtt000[i].GetHeight()) ChoseY = StAtt000[i].GetCY() + StAtt000[i].GetHeight();

		sprintf_s(ID,80,"%sStAtt045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = StAtt045[i].CreateSprite(ID, iB045, palID);
		if (MaxWidth < StAtt045[i].GetWidth()) MaxWidth = StAtt045[i].GetWidth();
		if (MaxHeight < StAtt045[i].GetHeight()) MaxHeight = StAtt045[i].GetHeight();
		if (MaxxCor > StAtt045[i].GetCX()) MaxxCor = StAtt045[i].GetCX();
		if (MaxyCor > StAtt045[i].GetCY()) MaxyCor = StAtt045[i].GetCY();
		if (ChoseX < StAtt045[i].GetCX() + StAtt045[i].GetWidth()) ChoseX = StAtt045[i].GetCX() + StAtt045[i].GetWidth();
		if (ChoseY < StAtt045[i].GetCY() + StAtt045[i].GetHeight()) ChoseY = StAtt045[i].GetCY() + StAtt045[i].GetHeight();
		
		sprintf_s(ID,80,"%sStAtt090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = StAtt090[i].CreateSprite(ID, iB090, palID);
		if (MaxWidth < StAtt090[i].GetWidth()) MaxWidth = StAtt090[i].GetWidth();
		if (MaxHeight < StAtt090[i].GetHeight()) MaxHeight = StAtt090[i].GetHeight();
		if (MaxxCor > StAtt090[i].GetCX()) MaxxCor = StAtt090[i].GetCX();
		if (MaxyCor > StAtt090[i].GetCY()) MaxyCor = StAtt090[i].GetCY();
		if (ChoseX < StAtt090[i].GetCX() + StAtt090[i].GetWidth()) ChoseX = StAtt090[i].GetCX() + StAtt090[i].GetWidth();
		if (ChoseY < StAtt090[i].GetCY() + StAtt090[i].GetHeight()) ChoseY = StAtt090[i].GetCY() + StAtt090[i].GetHeight();
		
		sprintf_s(ID,80,"%sStAtt135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = StAtt135[i].CreateSprite(ID, iB135, palID);
		if (MaxWidth < StAtt135[i].GetWidth()) MaxWidth = StAtt135[i].GetWidth();
		if (MaxHeight < StAtt135[i].GetHeight()) MaxHeight = StAtt135[i].GetHeight();
		if (MaxxCor > StAtt135[i].GetCX()) MaxxCor = StAtt135[i].GetCX();
		if (MaxyCor > StAtt135[i].GetCY()) MaxyCor = StAtt135[i].GetCY();
		if (ChoseX < StAtt135[i].GetCX() + StAtt135[i].GetWidth()) ChoseX = StAtt135[i].GetCX() + StAtt135[i].GetWidth();
		if (ChoseY < StAtt135[i].GetCY() + StAtt135[i].GetHeight()) ChoseY = StAtt135[i].GetCY() + StAtt135[i].GetHeight();
		
		sprintf_s(ID,80,"%sStAtt180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = StAtt180[i].CreateSprite(ID, iB180, palID);
		if (MaxWidth < StAtt180[i].GetWidth()) MaxWidth = StAtt180[i].GetWidth();
		if (MaxHeight < StAtt180[i].GetHeight()) MaxHeight = StAtt180[i].GetHeight();
		if (MaxxCor > StAtt180[i].GetCX()) MaxxCor = StAtt180[i].GetCX();
		if (MaxyCor > StAtt180[i].GetCY()) MaxyCor = StAtt180[i].GetCY();
		if (ChoseX < StAtt180[i].GetCX() + StAtt180[i].GetWidth()) ChoseX = StAtt180[i].GetCX() + StAtt180[i].GetWidth();
		if (ChoseY < StAtt180[i].GetCY() + StAtt180[i].GetHeight()) ChoseY = StAtt180[i].GetCY() + StAtt180[i].GetHeight();
		
	}

	iB000 = 0; iB045 = 0; iB090 = 0; iB135 = 0; iB180 = 0;
   for (i = 0; i < faces4; i++) 
   {
      sprintf_s(ID,80,"%sStMov000-%c",ViewID,(i*FRAMING) + 'a');
		iB000 = StMov000[i].CreateSprite(ID, iB000, palID);
		if (MaxWidth < StMov000[i].GetWidth()) MaxWidth = StMov000[i].GetWidth();
		if (MaxHeight < StMov000[i].GetHeight()) MaxHeight = StMov000[i].GetHeight();
		if (MaxxCor > StMov000[i].GetCX()) MaxxCor = StMov000[i].GetCX();
		if (MaxyCor > StMov000[i].GetCY()) MaxyCor = StMov000[i].GetCY();
		if (ChoseX < StMov000[i].GetCX() + StMov000[i].GetWidth()) ChoseX = StMov000[i].GetCX() + StMov000[i].GetWidth();
		if (ChoseY < StMov000[i].GetCY() + StMov000[i].GetHeight()) ChoseY = StMov000[i].GetCY() + StMov000[i].GetHeight();

		sprintf_s(ID,80,"%sStMov045-%c",ViewID,(i*FRAMING) + 'a');
		iB045 = StMov045[i].CreateSprite(ID, iB045, palID);
		if (MaxWidth < StMov045[i].GetWidth()) MaxWidth = StMov045[i].GetWidth();
		if (MaxHeight < StMov045[i].GetHeight()) MaxHeight = StMov045[i].GetHeight();
		if (MaxxCor > StMov045[i].GetCX()) MaxxCor = StMov045[i].GetCX();
		if (MaxyCor > StMov045[i].GetCY()) MaxyCor = StMov045[i].GetCY();
		if (ChoseX < StMov045[i].GetCX() + StMov045[i].GetWidth()) ChoseX = StMov045[i].GetCX() + StMov045[i].GetWidth();
		if (ChoseY < StMov045[i].GetCY() + StMov045[i].GetHeight()) ChoseY = StMov045[i].GetCY() + StMov045[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov090-%c",ViewID,(i*FRAMING) + 'a');
		iB090 = StMov090[i].CreateSprite(ID, iB090, palID);
		if (MaxWidth < StMov090[i].GetWidth()) MaxWidth = StMov090[i].GetWidth();
		if (MaxHeight < StMov090[i].GetHeight()) MaxHeight = StMov090[i].GetHeight();
		if (MaxxCor > StMov090[i].GetCX()) MaxxCor = StMov090[i].GetCX();
		if (MaxyCor > StMov090[i].GetCY()) MaxyCor = StMov090[i].GetCY();
		if (ChoseX < StMov090[i].GetCX() + StMov090[i].GetWidth()) ChoseX = StMov090[i].GetCX() + StMov090[i].GetWidth();
		if (ChoseY < StMov090[i].GetCY() + StMov090[i].GetHeight()) ChoseY = StMov090[i].GetCY() + StMov090[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov135-%c",ViewID,(i*FRAMING) + 'a');
		iB135 = StMov135[i].CreateSprite(ID, iB135, palID);
		if (MaxWidth < StMov135[i].GetWidth()) MaxWidth = StMov135[i].GetWidth();
		if (MaxHeight < StMov135[i].GetHeight()) MaxHeight = StMov135[i].GetHeight();
		if (MaxxCor > StMov135[i].GetCX()) MaxxCor = StMov135[i].GetCX();
		if (MaxyCor > StMov135[i].GetCY()) MaxyCor = StMov135[i].GetCY();
		if (ChoseX < StMov135[i].GetCX() + StMov135[i].GetWidth()) ChoseX = StMov135[i].GetCX() + StMov135[i].GetWidth();
		if (ChoseY < StMov135[i].GetCY() + StMov135[i].GetHeight()) ChoseY = StMov135[i].GetCY() + StMov135[i].GetHeight();
		
		sprintf_s(ID,80,"%sStMov180-%c",ViewID,(i*FRAMING) + 'a');
		iB180 = StMov180[i].CreateSprite(ID, iB180, palID);
		if (MaxWidth < StMov180[i].GetWidth()) MaxWidth = StMov180[i].GetWidth();
		if (MaxHeight < StMov180[i].GetHeight()) MaxHeight = StMov180[i].GetHeight();
		if (MaxxCor > StMov180[i].GetCX()) MaxxCor = StMov180[i].GetCX();
		if (MaxyCor > StMov180[i].GetCY()) MaxyCor = StMov180[i].GetCY();
		if (ChoseX < StMov180[i].GetCX() + StMov180[i].GetWidth()) ChoseX = StMov180[i].GetCX() + StMov180[i].GetWidth();
		if (ChoseY < StMov180[i].GetCY() + StMov180[i].GetHeight()) ChoseY = StMov180[i].GetCY() + StMov180[i].GetHeight();
		
	}


	fTransparent = View000[0].GetfTransparent();
	TransColor = View000[0].GetTransColor();
   if ((MaxxCor % 2)) {
      MaxxCor--;
      MaxWidth++;
   }
   if (!(MaxyCor % 2)) {
      MaxyCor--;
      MaxHeight++;
   }

	MaxWidth += ChoseX - (MaxxCor + MaxWidth);
	MaxHeight += ChoseY - (MaxyCor + MaxHeight);

   if (Type == 3)
      Type = 0;
}


int Sprite3D::GetTextCorrection(void) {
   LastAccess = timeGetTime();
   return View000[0].GetHeight();
}

BOOL Sprite3D::Release(DWORD Wait) {
   if ((timeGetTime() - LastAccess) > Wait) { 
      LastAccess = -1; 
      return TRUE; 
   } 
   return FALSE;
}