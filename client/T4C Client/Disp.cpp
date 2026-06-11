// *******************************************************************************************
// ***                                                                                     ***
//      File Name: Disp.cpp
//      Project:   TFC Client
//      Creation:  23/11/1997
//      Author:    Benoit Thomas (TH)
// ***                                                                                     ***
// *******************************************************************************************
// ***                                                                                     ***
//      Change History
//
//         Date            Ver.      Author         Purpose
//         ----            ----      ------         -------
//         23/11/1997      1.0        TH             Initial developpement
//
//      Description
//          Font Structure.
//          
// ***                                                                                     ***
//	*******************************************************************************************
// ***         Copyright (c) 1997-1998 Vircom. All rights reserved.                        ***

#pragma warning( disable : 4291 )
#pragma warning( disable : 4786 )

#include "pch.h"
#include <stdio.h>
#include "IDirectXDraw.h"
#include "Disp.h"
#include "Global.h"

FontHandler * UniqueFontHandler = NULL;

//**************************************************************************************//
int CALLBACK FontHandlerCallBack
//**************************************************************************************//
// Load all the Current System Font.
// 
(
 LPLOGFONT lplf,    // Logical Description of a Font.
 LPTEXTMETRIC lptm, // Physical Description of a Font.
 DWORD FontType,    // FontType.
 LPARAM lParam      // User-Defined Parameter.
)
// Return: int, 1 Continue enumeration, 0 Stop enumeration 
//**************************************************************************************//
{
	FontList *fl = new FontList;

   memcpy(fl->lplfLogical, lplf, sizeof(LOGFONT));
	fl->lpszFontName = new TCHAR [strlen(lplf->lfFaceName)+1];
	strcpy_s(fl->lpszFontName,strlen(lplf->lfFaceName)+1, lplf->lfFaceName);

	UniqueFontHandler->AddToTail(fl);
	return 1; 
} 

//**************************************************************************************//
Font::Font( void )
//**************************************************************************************//
// Default Constructor.
// 
//**************************************************************************************//
{
	lplfLogical = new LOGFONT;
   hf = NULL;

}

//*** Destructor ***********************************************************************//
Font::~Font( void )
//**************************************************************************************//
// Default Constructor.
// 
//**************************************************************************************//
{
	delete [] lplfLogical;
   if (hf)
      DeleteObject(hf);
   hf = NULL;
}

//**************************************************************************************//
void Font::SetLogical
//**************************************************************************************//
// Set the Logical Font format
// 
(
 LPLOGFONT lplf // Logical Font Format
)
//**************************************************************************************//
{
	memcpy(lplfLogical, lplf, sizeof(LOGFONT));
}

//**************************************************************************************//
void Font::CreateF( void )
//**************************************************************************************//
// Create the Font to use
// 
//**************************************************************************************//
{
   lplfLogical->lfQuality = ANTIALIASED_QUALITY;
   hf = CreateFontIndirect(lplfLogical); 
}

//**************************************************************************************//
void Font::Release( void )
//**************************************************************************************//
// Delete the Font.
// 
//**************************************************************************************//
{
	DeleteObject(hf);
   hf = NULL;
}

//**************************************************************************************//
int Font::DrawFont
//**************************************************************************************//
// Draw the Text on the Screen
// 
(
 HDC hdc,
 int x,             // X Position of the Text 
 int y,             // Y Position of the Text
 int DefaultColor,  // Default Color of the Text
 int CurrentColor,  // Curremt Color of the Text
 char *Text,       // The Text
 int Flag           // Flag
)
//**************************************************************************************//
{
	int DebugHelp = 0;
	char *ptr;
	int C = 0;
   unsigned int i;
	LPTSTR NText = new TCHAR [strlen(Text)+1];
	strcpy_s(NText,strlen(Text)+1, Text);
	LPTSTR NText2 = new TCHAR [strlen(Text)+1];
	strcpy_s(NText2,strlen(Text)+1, Text);
	COLORREF Color = CurrentColor ? CurrentColor : DefaultColor;
	if (Flag & NDT_CENTER) {
      for (i = 0; i < strlen(NText2); i++) {
         if (NText2[i] == '{' || NText2[i] == '}' || NText2[i] == '\"')
            C++;
         else
            NText2[i-C] = NText2[i];
      }
      NText2[i-C] = NULL;
      x -= (GetFLen(hdc, NText2)/2);
	}
	C = 0;
	SelectObject(hdc, hf);
	SetBkMode(hdc, TRANSPARENT);
	ptr = NText;
	unsigned int Len = strlen(NText);
	for (i=0; i < Len; i++) {
		if (NText[i] == '\"') {
			NText[i] = 0;
			DebugHelp = 1;
			SetTextColor(hdc, Color);
			DebugHelp = 5;
			if (Color != CL_WHITE && Color != 0)
				Color = CL_WHITE;
			else
				Color = DefaultColor;
			::TextOut(hdc, x+C, y, ptr, strlen(ptr));
			C += GetFLen(hdc, ptr);
			ptr = (char *)(NText+i+1);
		}
		if (NText[i] == '{') {
			NText[i] = 0;
			DebugHelp = 2;
			SetTextColor(hdc, Color);
			DebugHelp = 6;
			if (Color != CL_GRAY_LIGHT_MID && Color != 0)
				Color = CL_GRAY_LIGHT_MID;
			else
				Color = DefaultColor;
			::TextOut(hdc, x+C, y, ptr, strlen(ptr));
			C += GetFLen(hdc, ptr);
			ptr = (char *)(NText+i+1);
		}
		if (NText[i] == '}') {
			NText[i] = 0;
			DebugHelp = 3;
			SetTextColor(hdc, Color);
			DebugHelp = 7;
			if (Color != CL_GRAY_LIGHT_MID && Color != 0)
				Color = CL_GRAY_LIGHT_MID;
			else
				Color = DefaultColor;
			::TextOut(hdc, x+C, y, ptr, strlen(ptr));
			C += GetFLen(hdc, ptr);
			ptr = (char *)(NText+i+1);
		}
	}
	DebugHelp = 4;
	SetTextColor(hdc, DefaultColor);
	DebugHelp = 8;
	::TextOut(hdc, x+C, y, ptr, strlen(ptr));  
	delete NText;
	delete NText2; 
   return 0; 
}

//**************************************************************************************//
void Font::SetSize
//**************************************************************************************//
// Set both Width and Height;
// 
(
 long Width, // Font Width
 long Height, // Font Height
 long Weight
)
//**************************************************************************************//
{

	if (!Width) 
   {
		SetSize((float)(Height / lplfLogical->lfHeight));
	} 
   else if (!Height) 
   {
		SetSize((float)(Width / lplfLogical->lfWidth));
	} 
   else if (Width && Height) 
   {
		lplfLogical->lfHeight = Height;
		lplfLogical->lfWidth  = Width;
	}
   lplfLogical->lfWeight = Weight;
}

//**************************************************************************************//
void Font::SetSize
//**************************************************************************************//
// Set the Font Size in %
// 
(
 double Pc // % of the Original Font Size
)
//**************************************************************************************//
{
	lplfLogical->lfWidth  = (long)(lplfLogical->lfWidth  * Pc);
	lplfLogical->lfHeight = (long)(lplfLogical->lfHeight * Pc);
}

//**************************************************************************************//
int Font::GetFLen
//**************************************************************************************//
// Give the size of the String
// 
(
 HDC hdc,        // Handle du Device Context
 LPTSTR lpszText // the Text String
)
// Return: int, Size of the String in Pixel.
//**************************************************************************************//
{
	SIZE sz;
   sz.cx = 0;
   sz.cy = 0;
  
   HANDLE hOld = SelectObject(hdc, hf);
	BOOL bOK = GetTextExtentPoint32(hdc, lpszText, strlen(lpszText), &sz);  //ERR -->Caller partout ce truc a valider...
   SelectObject(hdc,hOld);
	
	return sz.cx;
}

//**************************************************************************************//
int Font::GetFHeight
//**************************************************************************************//
// Get the Font Height
// 
(
 HDC hdc // Handle du Device Context
)
// Return: int, Height of the Letter X in Pixel
//**************************************************************************************//
{
	SIZE sz;
   sz.cx = 0;
   sz.cy = 0;
   HANDLE hOld = SelectObject(hdc, hf);
	GetTextExtentPoint32(hdc, "X", 1, &sz); //OK -->Juste au loading pas en mode repetition...
   SelectObject(hdc,hOld);
	return sz.cy;
}

//**************************************************************************************//
FontList::FontList( void )
//**************************************************************************************//
// Default Constructor
// 
//**************************************************************************************//
{
	lplfLogical = new LOGFONT;
   lpszFontName = NULL;
}
 
//*** Destructor ***********************************************************************//
FontList::~FontList( void )
//**************************************************************************************//
// Default Destructor
// 
//**************************************************************************************//
{
	delete lplfLogical;
   if (lpszFontName)
      delete lpszFontName;
}

//**************************************************************************************//
FontHandler::FontHandler( void )
//**************************************************************************************//
// Default Constructor.
// 
//**************************************************************************************//
{

}

//*** Destructor ***********************************************************************//
FontHandler::~FontHandler( void )
//**************************************************************************************//
// Default Destructor.
// 
//**************************************************************************************//
{
	ToHead();
	while(QueryNext()) {
   	FontList *flObject;
		flObject = GetObject();
      RemoveObject();
      delete flObject;
	}
}

//**************************************************************************************//
void FontHandler::Initialize( void )
//**************************************************************************************//
// Load all the Current System Font.
// 
//**************************************************************************************//
{
   int iDebug = 0;
      HDC hdc;
      iDebug = 1;
	
   	DXDGetDCConnect(&hdc, 1);
      iDebug = 2;
   	EnumFonts(hdc, NULL, (FONTENUMPROC) FontHandlerCallBack, NULL);
      iDebug = 3;
   	DXDReleaseDC(hdc, 1);
      iDebug = 4;
}

//**************************************************************************************//
Font *FontHandler::GetFont
//**************************************************************************************//
// Get a System Font
// 
(
 LPTSTR lpszFontName // Name of a Font.
)
// Return: Font *, the Font Structure
//**************************************************************************************//
{
	Font     *foReturn = new Font;
	FontList *flObject;
	bool Found = false;

	ToHead();
	while(QueryNext()) {
		flObject = GetObject();
		if (!strcmp(flObject->lpszFontName, lpszFontName)) {
			foReturn->SetLogical(flObject->lplfLogical);
			Found = true;
			SetQueryState(STOP_QUERY);
		}
	}

	if (!Found) {
		ToHead();
		while(QueryNext()) {
			flObject = GetObject();
			if (!strcmp(flObject->lpszFontName, "System")) {
				foReturn->SetLogical(flObject->lplfLogical);
				Found = true;
				SetQueryState(STOP_QUERY);
			}
		}
	}

	return foReturn;
}


