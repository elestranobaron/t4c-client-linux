// ************************************************************************
// ***                                                                  ***
//      File Name: GUILocalString.cpp
//      Project:   The 4th Coming
//      Creation:  25 Mars, 1997
//      Author:    Benoit Thomas (TH)
// ***                                                                  ***
// ************************************************************************
// ***                                                                  ***

// Include
#include "pch.h"
#include "GUILocalString.h"
#include "Global.h"
#include "TFCPlayer.h"
#include <string>


// Global var.
GUILocalString g_GUILocalString;
GUILocalString g_LocalString;

//*********************************************************************************
GUILocalString::GUILocalString( void )
/**********************************************************************************
 * Constructor
 */
{
// Constructor
	dwStringNb   = 0;
	InitializeCriticalSection(&csLanguageLock);
	bCreated = false;
}
		
//*********************************************************************************
GUILocalString::~GUILocalString( void )
/**********************************************************************************
 * Destructor
 */
{
// Destructor
	DeleteCriticalSection(&csLanguageLock);
	Release(); 
}

//*********************************************************************************
void GUILocalString::CreateLang( int dwLanguage ,bool bGUI)
/**********************************************************************************
 * Open the Local String file (currently a .txt) and copy all the string into memory.
 */
{
// Open Local String file and put all string into memory.
    LanguageString.resize( 1000 );
    bHotKey.resize( 1000 );

    UINT i;
    for( i = 0; i < LanguageString.size(); i++ ){
        bHotKey[ i ] = 0;
        LanguageString[ i ] = 0;
    }

	LoadAllStrings(dwLanguage,bGUI);

	bCreated = true;
}

//*********************************************************************************
void GUILocalString::Release( void )
/**********************************************************************************
 * Close the Local String file and release all the memory.
 */
{
// Close the Local String file.
   //if (LanguageFile) {
   //	fclose(LanguageFile);
   //  LanguageFile = NULL;
   //}
   for (unsigned int i = 0; i < dwStringNb; i++) {
      delete LanguageString[i];
   }
   dwStringNb = NULL;
}

//*********************************************************************************
char *GUILocalString::operator []
/**********************************************************************************
 * Get a string from an ID
 */
(
 unsigned int l                 // ID of the string
)
//*********************************************************************************
{
// Return a String
   if (bCreated) {
      if (l <= dwStringNb) {
	     return LanguageString[l-1];
	  } else {
		 return LanguageString[dwStringNb-1];
	  }
   } else {
	   return "Localized String not Initialized.";
   }
}


#include "EncRandom.h"
namespace t4csvr{
    class RandomTable{
    public:
        RandomTable(){
            rnd.SetSeed( 11677 );
            int i;
            for( i = 0; i < RandomTableSize; i++ ){
                lpRandomTable[ i ] = rnd( 0, 255 );
            }
        }

        char GetChar( long nPos ){
            return lpRandomTable[ nPos % RandomTableSize ];
        }
    
    private:
        enum{ RandomTableSize = 7823 };
        char  lpRandomTable[ RandomTableSize ];
        t4csvr::Random rnd;
    };
}

//*********************************************************************************
void GUILocalString::LoadAllStrings( int dwLanguage ,bool bGUI)
/**********************************************************************************
* Load all the string from the Local String file into memory.
*/
{
	dwStringNb = 0;

	char buf[ 256 ];
	char lngFile[ _MAX_PATH ];

	if(bGUI)
	{
		if(dwLanguage == 0)
			sprintf_s( lngFile,_MAX_PATH, "EnglishGUI.elng");
		else if(dwLanguage == 1)
			sprintf_s( lngFile,_MAX_PATH, "FrenchGUI.elng");
      else if(dwLanguage == 2)
         sprintf_s( lngFile,_MAX_PATH, "GermanGUI.elng");
		else
			sprintf_s( lngFile,_MAX_PATH, "EnglishGUI.elng");
	}
	else
	{
		if(dwLanguage == 0)
			sprintf_s( lngFile,_MAX_PATH, "English.elng");
		else if(dwLanguage == 1)
			sprintf_s( lngFile,_MAX_PATH, "French.elng");
      else if(dwLanguage == 2)
         sprintf_s( lngFile,_MAX_PATH, "german.elng");
		else
			sprintf_s( lngFile,_MAX_PATH, "English.elng");
	}
	

	FILE *f = NULL;
	fopen_s(&f, lngFile, "rb" );
	if (f == NULL) 
	{
		LanguageString[0] = new char [100];
		strcpy_s(LanguageString[0],100, "Cannot open language file: EnglishGUI.elng");
		dwStringNb = 1;
		return;
	}

	fseek(f, 0, SEEK_END);
	int sizeBuf = ftell(f);
	char *fulBuf = new char [sizeBuf];
	fseek(f, 0, SEEK_SET);
	fread(fulBuf, 1, sizeBuf, f);  
	fclose(f);
	int iCur = 0;

	// Decryption
	t4csvr::RandomTable cRandomTable;
	for (int i = 0; i < sizeBuf; i++) 
	{
		fulBuf[i] = cRandomTable.GetChar( i + 1 ) ^ (char)fulBuf[i];
	}

	DWORD lineCount = 0;
	int ch = fulBuf[iCur++];
	if( iCur >= sizeBuf )
	{
		//OutputDebugString( "\r\nEmpty file " );
		//OutputDebugString( lngFile );
		if(fulBuf)
			delete []fulBuf;
		return;
	}

	do
	{
		// If a string ID was found.
		if( ch == '[' )
		{
			DWORD id = 0;

			// Get the file Id
			while( 1 )
			{
				ch = fulBuf[iCur++];

				if( ch == ']' )
				{
					break;
				}
				else if( iCur >= sizeBuf )
				{
					//OutputDebugString( "\r\nEnd of file reached querying Id in file " );
					//OutputDebugString( lngFile );
					if(fulBuf)
						delete []fulBuf;
					return;
				}
				if( !isdigit( ch ) )
				{
					//OutputDebugString( "\r\nUndefined character in ID on line " );
					_itoa_s( lineCount, buf,256, 10 ) ;
					//OutputDebugString( buf);
					if(fulBuf)
						delete []fulBuf;
					return;
				}
				else
				{
					id *= 10;
					id += ch - '0';
				}
			}
			if( id == 0 )
			{
				//OutputDebugString( "\r\nInvalid ID 0 on line " );
				_itoa_s( lineCount, buf,256, 10 ) ;
				//OutputDebugString( buf );
				if(fulBuf)
					delete []fulBuf;
				return;
			}

			// While we didn't find another ID.
			std::string str;
			do
			{                
				// Search for an opening quote
				do
				{
					ch = fulBuf[iCur++];
					if( ch == '[' )
					{
						break;
					}
					if( iCur >= sizeBuf )
					{
						break;
					}
					if( ch != '\r' && ch != '\n' && ch != '\t' && ch != ' ' && ch != '\"' )
					{
						//OutputDebugString( "\r\nInvalid character found looking for string on line " );
						_itoa_s( lineCount, buf,256, 10 ) ;
						//OutputDebugString( buf );
						if(fulBuf)
							delete []fulBuf;
						return;
					}
					if( ch == '\r' )
					{
						lineCount++;
					}
				}while( iCur < sizeBuf && ch != '\"' );

				if( ch == '[' )
				{
					break;
				}

				if(iCur < sizeBuf)
					ch = fulBuf[iCur++];
				while( iCur < sizeBuf && ch != '\"' )
				{
					bool getnext = true;
					// Conversions
					switch( ch )
					{
						case '\\': str += '\"'; break;
						case '_':
							ch = fulBuf[iCur++];
							bHotKey[ id ] = ch;
							str += ch;
						break;
						case '<': 
							ch = fulBuf[iCur++];
							if( ch == '>' ){
								getnext = true;
								str += "\r\n";
							}
							else
							{
								str += '<';
								getnext = false;
							}
						break;
						case '\n':
						case '\r':
							//OutputDebugString( "\r\nEnd-of-line character found parsing string on line " );
							_itoa_s( lineCount, buf,256, 10 ) ;
							//OutputDebugString( buf );
							if(fulBuf)
								delete []fulBuf;
						return;
						default:
							str += ch;
						break;
					}

					if( getnext )
					{
						ch = fulBuf[iCur++];
					}
				};
			}while( ch != '[' && iCur < sizeBuf );

			LanguageString[ id - 1 ] = new char [ str.size() + 1 ];
			strcpy_s( LanguageString[ id - 1 ],str.size() + 1, str.c_str() );

			if( dwStringNb < id )
		 {
			 dwStringNb = id;
			}
		}
		else
		{
			ch = fulBuf[iCur++];
		}
	}while( iCur < sizeBuf  );
	if(fulBuf)
		delete []fulBuf;
}



//*********************************************************************************
char GUILocalString::GetHotKey
/**********************************************************************************
 * Get the hotkey of a string from a string ID.
 * 
 * @REMARK A hotkey is a letter in the string that begins with a _ like in
 * _Example, E is the hotkey.
 */
(
 int l                          // ID of the string
)
//*********************************************************************************
{
   if (bCreated) {
      if ((DWORD)l <= dwStringNb) {
        return bHotKey[l];
	  } else {
        return bHotKey[dwStringNb];
	  }
   } else {
	   return 'L';
   }
}

void GUILocalString::CannotParse(void) {
   Release();
   LanguageString[0] = new char [250];
   sprintf_s(LanguageString[0],250, "Error in parsing file: %s.elng", Player.szLanguage);
   dwStringNb = 1;
}