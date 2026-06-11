#pragma warning( disable : 4291 )

#include "pch.h"
#include "Spell.h"
#include "Global.h"

Spell::Spell(void) {
   bAlreadyLoad = FALSE;
   SpellAnim = 0;
   SpellAnimA = 0;
}

Spell::~Spell(void) {
   Release();
}

BOOL Spell::Release(DWORD Wait) 
{
   if ((timeGetTime() - LastAccess) > Wait) 
   { 
      LastAccess = -1; 
      return TRUE; 
   } 
   return FALSE;
}

void Spell::Release(void) 
{
   bAlreadyLoad = FALSE;
   if (SpellAnim) 
   {
      delete [] SpellAnim;
      SpellAnim = 0;
   }
   if (SpellAnimA) 
   {
      delete [] SpellAnimA;
      SpellAnimA = 0;
   }
}

void Spell::LoadSpell(int f, char *Name, int p) 
{
   if (bAlreadyLoad)
		return;
   
   bAlreadyLoad = TRUE;

   LastAccess = timeGetTime();
   char ID[26*6+1];
   
   int ChoseX = 0;
	int ChoseY = 0;

   Faces = f;
   FacesA = 0;
   FacesC = 0;
   FacesR = 0;  //NMNMNM_Faces
   FacesStA = 0;
   FacesStM = 0;
   Type = 3;
   
   MaxxCor = 0;
   MaxyCor = 0;


   SpellAnim = new CV2Sprite [Faces];

   for (int i = 0; i < Faces; i++) {
	   strcpy_s(ID,157, Name);
      ID[strlen(ID)+1] = 0;
      if (i < 26) {
         ID[strlen(ID)] = i+'a';
      } else if (i < 26*2) {
         int Len = strlen(ID);
         ID[Len] = '2';
         ID[Len+1] = (i-26)+'a';
         ID[Len+2] = 0;
      } else if (i < 26*3) {
         int Len = strlen(ID);
         ID[Len] = '3';
         ID[Len+1] = (i-26*2)+'a';
         ID[Len+2] = 0;
      } else if (i < 26*4) {
         int Len = strlen(ID);
         ID[Len] = '4';
         ID[Len+1] = (i-26*3)+'a';
         ID[Len+2] = 0;
      } else if (i < 26*5) {
         int Len = strlen(ID);
         ID[Len] = '5';
         ID[Len+1] = (i-26*4)+'a';
         ID[Len+2] = 0;
      } else if (i < 26*6) {
         int Len = strlen(ID);
         ID[Len] = '6';
         ID[Len+1] = (i-26*5)+'a';
         ID[Len+2] = 0;
      }

      SpellAnim[i].CreateSprite(ID, 0, p);
		if (MaxWidth < SpellAnim[i].GetWidth()) MaxWidth = SpellAnim[i].GetWidth();
		if (MaxHeight < SpellAnim[i].GetHeight()) MaxHeight = SpellAnim[i].GetHeight();
		if (MaxxCor > SpellAnim[i].GetCX()) MaxxCor = SpellAnim[i].GetCX();
		if (MaxyCor > SpellAnim[i].GetCY()) MaxyCor = SpellAnim[i].GetCY();
		if (ChoseX < SpellAnim[i].GetCX() + SpellAnim[i].GetWidth()) ChoseX = SpellAnim[i].GetCX() + SpellAnim[i].GetWidth();
		if (ChoseY < SpellAnim[i].GetCY() + SpellAnim[i].GetHeight()) ChoseY = SpellAnim[i].GetCY() + SpellAnim[i].GetHeight();
   }

	fTransparent = SpellAnim[0].GetfTransparent();
	TransColor = SpellAnim[0].GetTransColor();
	MaxWidth += ChoseX - (MaxxCor + MaxWidth);
	MaxHeight += ChoseY - (MaxyCor + MaxHeight);
   if (MaxWidth > 60000 || MaxHeight > 60000)
      throw;
}

void Spell::LoadSpellA(int f, char *Name, char *NameA, int p) 
{
   if (bAlreadyLoad)
		return;
   
   bAlreadyLoad = TRUE;

   LastAccess = timeGetTime();
   char ID[26*6+1];
   char IDA[26*6+1];
   
   int ChoseX = 0;
	int ChoseY = 0;

   Faces = f;
   FacesA = 0;
   FacesR = 0;  //NMNMNM_Faces
   FacesC = 0;
   FacesStA = 0;
   FacesStM = 0;
   Type = 3;
   
   MaxxCor = 0;
   MaxyCor = 0;


   SpellAnim  = new CV2Sprite [Faces];
   SpellAnimA = new CV2Sprite [Faces];

   for (int i = 0; i < Faces; i++) 
   {
	   strcpy_s(ID ,157, Name);
      strcpy_s(IDA,157, NameA);
      ID[strlen(ID)+1] = 0;
      IDA[strlen(IDA)+1] = 0;
      if (i < 26) 
      {
         ID[strlen(ID)] = i+'a';
         IDA[strlen(IDA)] = i+'a';
      } 
      else if (i < 26*2) 
      {
         int Len = strlen(ID);
         ID[Len] = '2';
         ID[Len+1] = (i-26)+'a';
         ID[Len+2] = 0;

         int LenA = strlen(IDA);
         IDA[LenA] = '2';
         IDA[LenA+1] = (i-26)+'a';
         IDA[LenA+2] = 0;
      } 
      else if (i < 26*3) 
      {
         int Len = strlen(ID);
         ID[Len] = '3';
         ID[Len+1] = (i-26*2)+'a';
         ID[Len+2] = 0;

         int LenA = strlen(IDA);
         IDA[LenA] = '3';
         IDA[LenA+1] = (i-26*2)+'a';
         IDA[LenA+2] = 0;
      } 
      else if (i < 26*4) 
      {
         int Len = strlen(ID);
         ID[Len] = '4';
         ID[Len+1] = (i-26*3)+'a';
         ID[Len+2] = 0;

         int LenA = strlen(IDA);
         IDA[LenA] = '4';
         IDA[LenA+1] = (i-26*3)+'a';
         IDA[LenA+2] = 0;
      } 
      else if (i < 26*5) 
      {
         int Len = strlen(ID);
         ID[Len] = '5';
         ID[Len+1] = (i-26*4)+'a';
         ID[Len+2] = 0;

         int LenA = strlen(IDA);
         IDA[LenA] = '5';
         IDA[LenA+1] = (i-26*4)+'a';
         IDA[LenA+2] = 0;
      } 
      else if (i < 26*6) 
      {
         int Len = strlen(ID);
         ID[Len] = '6';
         ID[Len+1] = (i-26*5)+'a';
         ID[Len+2] = 0;

         int LenA = strlen(IDA);
         IDA[LenA] = '6';
         IDA[LenA+1] = (i-26*5)+'a';
         IDA[LenA+2] = 0;
      }

      SpellAnim[i] .CreateSprite(ID, 0, p);
      SpellAnimA[i].CreateSprite(IDA, 0, p);
		if (MaxWidth < SpellAnim[i].GetWidth()) 
         MaxWidth = SpellAnim[i].GetWidth();
		if (MaxHeight < SpellAnim[i].GetHeight()) 
         MaxHeight = SpellAnim[i].GetHeight();
		if (MaxxCor > SpellAnim[i].GetCX()) 
         MaxxCor = SpellAnim[i].GetCX();
		if (MaxyCor > SpellAnim[i].GetCY()) 
         MaxyCor = SpellAnim[i].GetCY();
		if (ChoseX < SpellAnim[i].GetCX() + SpellAnim[i].GetWidth()) 
         ChoseX = SpellAnim[i].GetCX() + SpellAnim[i].GetWidth();
		if (ChoseY < SpellAnim[i].GetCY() + SpellAnim[i].GetHeight()) 
         ChoseY = SpellAnim[i].GetCY() + SpellAnim[i].GetHeight();
   }

	fTransparent = SpellAnim[0].GetfTransparent();
	TransColor = SpellAnim[0].GetTransColor();
	MaxWidth += ChoseX - (MaxxCor + MaxWidth);
	MaxHeight += ChoseY - (MaxyCor + MaxHeight);
   if (MaxWidth > 60000 || MaxHeight > 60000)
      throw;
}


void Spell::DrawSprite3D(int Direction, int Frame, int X, int Y, BYTE Stand, RECT *Clip, BOOL boOutline, BYTE bOutlineColor, LPDIRECTDRAWSURFACE7 Surface, signed char Format, TFCObject *Object,Sprite3D *pMask) {
   LastAccess = timeGetTime();
   V2SPRITEFX FX;
   ZeroMemory(&FX, sizeof(FX));
   FX.lpClipRect = Clip;
   FX.lpDirectDrawSurface = Surface;
   if(SpellAnimA)
      SpellAnim[Frame].DrawSpriteN(X, Y, &FX,&SpellAnimA[Frame]);
   else
      SpellAnim[Frame].DrawSpriteN(X, Y, &FX);

}
