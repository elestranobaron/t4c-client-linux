#include "pch.h"
#include "FormatText.h"
#include "IDirectXDraw.h"
#include "Global.h"

FormatText::FormatText(void) {
   pString = NULL;
   ppText  = NULL;

   nbLine = 0;
	SystemFormat = TRUE;
}

FormatText::~FormatText(void) {
   if (pString)
      delete []pString;
   pString = NULL;

   if (ppText) 
   {
      for (int i = 0; i < 2500; i++)
      {
         if(ppText[i])
            delete []ppText[i];
      }
      delete []ppText;
      ppText = NULL;
   }
}

void FormatText::SetText(const char *s) 
{
   if (s) 
   {
      if (pString) 
      {
         //concatene la new string dans la vieille
         int iLen = strlen(pString) + strlen(s) + 2;
         char *NewString = new char [iLen];
         strcpy_s(NewString,iLen, pString);
         strcat_s(NewString,iLen, " ");
         strcat_s(NewString,iLen, s);

         delete []pString;
         pString = NewString;
      } 
      else 
      {
         char *NewString = new char [strlen(s) + 1];
         strcpy_s(NewString,strlen(s) + 1, s);
         pString = NewString;
      }
   }
}

char * FormatText::GetString(void) 
{
	return pString;
}

void FormatText::Format(int PixelPerLine, Font *fFont, bool indentNewLine) 
{
   if (ppText) 
   {
      for (int i = 0; i < 2500; i++)
      {
         if(ppText[i])
            delete []ppText[i];
      }
      delete []ppText;
      ppText = NULL;
   }
   
   ppText      = new char * [2500];
   for(int i=0;i<2500;i++)
      ppText[i] = NULL;

   int itempLen = strlen(pString) + 1;
   char *Temp  = new char [itempLen];
   char *tText = new char [2500];



   char *NewWord;
   int Len = 0;
   int OldnbLine = 0;
   int extraIndentLen = 0;
   
   bool Exit = false;
			
   strcpy_s(Temp,itempLen, pString);
   char *next_token1 = NULL;
   NewWord = strtok_s(Temp, " ",&next_token1);
   
   nbLine = 0;
   Len  = 0;
   
   HDC hdc;
   DXDGetDC(&hdc, 2);
  
   
  
   while (NewWord) 
   {
      ZeroMemory(tText, 2500);
      Exit = false;
      if (NewWord)
         Len = fFont->GetFLen(hdc, NewWord);
      
      if( nbLine != 0 && extraIndentLen == 0 )
      {
         extraIndentLen = fFont->GetFLen( hdc, "  " );
      }
      
      while (Len + fFont->GetFLen(hdc, tText) + extraIndentLen >= PixelPerLine) 
      {
         int len = strlen(NewWord);
         NewWord[len-1] = NULL;
         Len = fFont->GetFLen(hdc, NewWord);
      }
      
      while (fFont->GetFLen(hdc, tText) + Len + extraIndentLen < PixelPerLine && NewWord && !Exit) 
      {
         strcat_s(tText,2500, NewWord);
         
         if (fFont->GetFLen(hdc, tText) < PixelPerLine)
            strcat_s(tText,2500, " ");
         
         NewWord = strtok_s(NULL, " ",&next_token1);
         if (NewWord) 
         {
            Len = fFont->GetFLen(hdc, NewWord);
            if (!strncmp(NewWord, "<>", 2) && SystemFormat) 
            {
               Exit = true;
               NewWord+=2;
               //if(strlen(NewWord) >2)
               //   strcpy_s(NewWord,2500, &NewWord[2]);
               //else
			      //   NewWord[0] = 0x00;
               Len = fFont->GetFLen(hdc, NewWord);
            }
         }
      } 
      
      if( nbLine == 0 || !indentNewLine && indentNewLine )
      {
         ppText[nbLine] = new char [strlen(tText)+1];
         strcpy_s(ppText[nbLine],strlen(tText)+1, tText);
      } 
      else if (nbLine < 2500)
      { 
         ppText[nbLine] = new char [strlen(tText)+3];
         strcpy_s(ppText[nbLine],strlen(tText)+3, "  ");
         strcat_s(ppText[nbLine],strlen(tText)+3, tText);
      }
      else
      {
         int cacabug = 1;
      }
      nbLine++;
   }
   DXDReleaseDC(hdc, 2); 
   
   delete []Temp;
   delete []tText;
}

void FormatText::Format(int PixelPerLine, FontObject *fFont, bool indentNewLine ) {
   if (ppText) 
   {
      for (int i = 0; i < 2500; i++)
      {
         if(ppText[i])
            delete []ppText[i];
      }
      delete []ppText;
      ppText = NULL;
   }
   
   ppText      = new char * [2500]; 
   for(int i=0;i<2500;i++)
      ppText[i] = NULL;
   
   int iTempLen = strlen(pString) + 1;
   char *Temp  = new char [iTempLen];
   char *tText = new char [2500];
   
   char *NewWord;
   int Len;
   int OldnbLine = 0;
   bool Exit = false;
   strcpy_s(Temp,iTempLen, pString);
   
   char *next_token1 = NULL;
   NewWord = strtok_s(Temp, " ",&next_token1);
   
   nbLine = 0;
   Len  = 0;
   
   HDC hdc;
   DXDGetDC(&hdc, 3);
   
   int extraIndentLen = 0;

   bool bEndLineTagBL        = false;
   bool bNextLineTagBL       = false;
   int  iEndLineTagBLOffset  = 0;
   int  iNextLineTagBLOffset = 0;

   bool bEndLineTagGR        = false;
   bool bNextLineTagGR       = false;
   int  iEndLineTagGROffset  = 0;
   int  iNextLineTagGROffset = 0;
   
   while (NewWord) 
   {
      ZeroMemory(tText, 2500);
      Exit = false;
      if (NewWord) 
      {
         Len = (int)fFont->GetFWidth(hdc, NewWord);
      }
      
      if( nbLine != 0 && extraIndentLen == 0 && indentNewLine )
      {
         extraIndentLen = (int)fFont->GetFWidth( hdc, "  " );
      }
       
      while (Len + fFont->GetFWidth(hdc, tText) + extraIndentLen >= PixelPerLine) 
      {
         int len = strlen(NewWord);
         NewWord[len-1] = NULL;
         Len = (int)fFont->GetFWidth(hdc, NewWord);
      }
      
      while (fFont->GetFWidth(hdc, tText) + Len + extraIndentLen < PixelPerLine && NewWord && !Exit) 
      {
         strcat_s(tText,2500, NewWord);
         
         if (fFont->GetFWidth(hdc, tText) < PixelPerLine)
            strcat_s(tText,2500, " ");
         
         NewWord = strtok_s(NULL, " ",&next_token1);
         if (NewWord) 
         {
            Len = (int)fFont->GetFWidth(hdc, NewWord);
            if (!strncmp(NewWord, "<>", 2) && SystemFormat) 
            {
               Exit = true;
               NewWord+=2;
               //if(strlen(NewWord) >2)
               //   strcpy_s(NewWord,2500, &NewWord[2]);
               //else
               //   NewWord[0] = 0x00;
               Len = (int)fFont->GetFWidth(hdc, NewWord);
            }
         }
      }
      
      if( nbLine == 0 || !indentNewLine )
      {
         //found si un code de couleur est en cours,... si oui
         //et que pas fini on ajoute a la fin de la ligne la fermeture du code
         //gere le blanc


         //validation du blanc...
         int iNbrBL  = 0;
         int iNbrGRO = 0;
         int iNbrGRC = 0;
         //si on dois ajouter un tag de debut de ligne on le compte
         if(bNextLineTagBL)
            iNbrBL = 1;
         if(bNextLineTagGR)
            iNbrGRO = 1;

         //scan trouver tous els blanc
         for(UINT i=0;i<strlen(tText);i++)
         {
            if(tText[i] == '"')
               iNbrBL++;
            if(tText[i] == '{')
               iNbrGRO++;
            if(tText[i] == '}')
               iNbrGRC++;
         }

         if(iNbrBL%2)
         {
            bEndLineTagBL       = true;
            iEndLineTagBLOffset = 2;
         }
         if(iNbrGRO != iNbrGRC)
         {
            bEndLineTagGR       = true;
            iEndLineTagGROffset = 1;
         }

         int iLen = strlen(tText)+1+iEndLineTagBLOffset+iNextLineTagBLOffset
                                   +iEndLineTagGROffset+iNextLineTagGROffset;
         ppText[nbLine] = new char [iLen];

         //on doit commencer avec un BL
         ppText[nbLine][0] = 0x00; //reset la ligne..
         if(bNextLineTagBL)
         {
            strcat_s(ppText[nbLine],iLen, "\"");
            bNextLineTagBL       = false;
            iNextLineTagBLOffset = 0;
         }
         if(bNextLineTagGR)
         {
            strcat_s(ppText[nbLine],iLen, "{");
            bNextLineTagGR       = false;
            iNextLineTagGROffset = 0;
         }
         strcat_s(ppText[nbLine],iLen, tText);


         if(bEndLineTagBL)
         {
            strcat_s(ppText[nbLine],iLen, "\"");
            bNextLineTagBL       = true;
            iNextLineTagBLOffset = 2;
            bEndLineTagBL        = false;
            iEndLineTagBLOffset  = 0;
         }

         if(bEndLineTagGR)
         {
            strcat_s(ppText[nbLine],iLen, "}");
            bNextLineTagGR       = true;
            iNextLineTagGROffset = 1;
            bEndLineTagGR        = false;
            iEndLineTagGROffset  = 0;
         }
         
      }
      else
      {
         ppText[nbLine] = new char [strlen(tText)+3];
         strcpy_s(ppText[nbLine],strlen(tText)+3, "  ");
         strcat_s(ppText[nbLine],strlen(tText)+3, tText);
      }
      nbLine++;
   }
   
   DXDReleaseDC(hdc, 3);
   
   delete []Temp;
   delete []tText;
}

void FormatText::SetSystem(BOOL f) 
{
	SystemFormat = f;
}

void FormatText::Format(UINT Len, bool indentNewLine ) 
{
   if (ppText) 
   {
      for (int i = 0; i < 2500; i++)
      {
         if(ppText[i])
            delete []ppText[i];
      }
      delete []ppText;
      ppText = NULL;
   }
   
   ppText      = new char * [2500];
   for(int i=0;i<2500;i++)
      ppText[i] = NULL;
   
   int iTempLen = strlen(pString) + 1;
   char *Temp  = new char [iTempLen];
   char *tText = new char [2500];
   char *NewWord;
   UINT Len2 = 0;
   int OldnbLine = 0;
   
   bool Exit = false;
   
   strcpy_s(Temp,iTempLen, pString);
   char *next_token1 = NULL;
   NewWord = strtok_s(Temp, " ",&next_token1);
   
   
   
   int extraIndentLen = 0;
   
   while (NewWord) 
   {
      ZeroMemory(tText, 2500);
      
      if (Len2 >= Len) 
      {
         NewWord[Len-1] = 0;
         Len2 = Len-1;
      }
      
      if( nbLine != 0 && extraIndentLen == 0 && indentNewLine )
      {
         extraIndentLen = 2;
      }
      
      Exit = false;
      while (strlen(tText) + Len2 + extraIndentLen < Len && NewWord && !Exit) 
      {
         strcat_s(tText,2500, NewWord);
         
         if (strlen(tText) < Len)
            strcat_s(tText,2500, " ");
         
         NewWord = strtok_s(NULL, " ",&next_token1);
         if (NewWord) {
            Len2 = strlen(NewWord);
            if (!strncmp(NewWord, "<>", 2) && SystemFormat) 
            {
               Exit = true;
               NewWord+=2;
               //if(strlen(NewWord) >2)
               //   strcpy_s(NewWord,2500, &NewWord[2]);
               //else
               //   NewWord[0] = 0x00;
               Len2 = strlen(NewWord);
            }
         } 
      }
      
      if( nbLine == 0 || !indentNewLine )
      {
         ppText[nbLine] = new char [strlen(tText)+1];
         strcpy_s(ppText[nbLine],strlen(tText)+1, tText);
      }
      else
      {
         ppText[nbLine] = new char [strlen(tText)+3];
         strcpy_s(ppText[nbLine],strlen(tText)+3, "  ");
         strcat_s(ppText[nbLine],strlen(tText)+3, tText);
      }
      nbLine++;
   }
   
   delete []Temp;
   delete []tText; 
}

int FormatText::GetnbLine(void) {
   return nbLine;
}

char *FormatText::GetText(int nb) {
   return ppText[nb];
}