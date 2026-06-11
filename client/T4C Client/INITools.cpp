//--FILE DEFINITION-----------------------------------------------------------
//
/*! \file
   File name:  INITools.cpp(CINITools)

   Author:     Yves Lessard

   Date:       2002/07/01(YYYY/MM/DD)

   Library:		WIN32

   Version:    1.09

   Usage:      Reading & Writing Ini File Like.
	
   Notes:	   Implementation.	
*/
//----------------------------------------------------------------------------

//--FILE INCLUDES-------------------------------------------------------------
#include "INITools.h"



//!--CONSTRUCTOR--------------------------------------------------------------
//
//  Method Name:  CINITools()
//
/*!
    Notes         Constructor.
*/
//----------------------------------------------------------------------------
CINITools::CINITools()
{
   m_szFileName="";
   m_szErrorMessage = "";
}


//!--DESTRUCTOR----------------------------------------------------------------
//
// Method Name:  ~CINITools()
//
/*!

   Notes         Destructor. Clean-up
*/			
//-----------------------------------------------------------------------------
CINITools::~CINITools()
{
   ResetData();
}


///////////////////////////////////////////////////////////////////////////
//                         Private Functions
///////////////////////////////////////////////////////////////////////////


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   IsFileExist
//
/*!Access:        Private

   Parameters

   IN             Description

   <LPCTSTR>      The filename to check.

   OUT            Description

   <bool>         TRUE if founded else FALSE.

   Notes
*/
//--------------------------------------------------------------------------
bool CINITools::IsFileExist(LPCTSTR _szFileName)
{
   bool bResult = false;
	HANDLE hFind;
	WIN32_FIND_DATA dataFind;

	// First the file
	hFind = FindFirstFile(_szFileName, &dataFind);
	if ( hFind != INVALID_HANDLE_VALUE )
   {
      bResult = true;
   }
	// Close file handle
	FindClose(hFind);
   if(!bResult)
   {
      //create an empty file..
      FILE *pfINI = NULL;
      fopen_s(&pfINI,_szFileName,"wt");
      if(pfINI)
      {
         bResult = true;
         fclose(pfINI);
      }
   }
	
	return bResult;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   ResetData
//
/*!Access:        Private

   Parameters

   IN             Description

   <None>

   OUT            Description

   <None>

   Notes:         Clean up list array.
*/
//--------------------------------------------------------------------------
void CINITools::ResetData()
{
   SECTION*  section;
   if( !m_LstINI.empty() )
   {
      std::vector<SECTION*>::iterator Iter;
      for (Iter = m_LstINI.begin(); Iter != m_LstINI.end(); Iter++)
      {
         section = *Iter;
         std::vector<KEYS*>::iterator Iter1;
         if( !section->LstKeys.empty() )
            for( Iter1 = section->LstKeys.begin(); Iter1 != section->LstKeys.end(); Iter1++)
                  delete *Iter1;
         (*Iter)->LstKeys.erase((*Iter)->LstKeys.begin(), (*Iter)->LstKeys.end());
         delete section;
      }
   }
   m_LstINI.erase(m_LstINI.begin(), m_LstINI.end());
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   GetIniInfos
//
/*!Access:        Public

   Parameters

   IN             Description

   <None>

   OUT            Description

   <None>         

   Notes:         Fill internal  List array.
*/
//--------------------------------------------------------------------------
void CINITools::GetIniInfos()
{
   SECTION*  section;
   KEYS*     key;
   DWORD    dwBytesReaded =0;
   DWORD    dwByteToRead = MAX_CHARACTERS;
   char*    pszTemp = new char[MAX_CHARACTERS];
   char*    pszTempKey = new char[MAX_CHARACTERS];
   char*    pNextSection = NULL;
   char*    pNextKey = NULL;
   std::string szTemp;

   // Clean previous list
   ResetData();
   memset(pszTemp, 0, sizeof(char)*MAX_CHARACTERS);

   dwBytesReaded = ::GetPrivateProfileSectionNames(pszTemp, dwByteToRead, m_szFileName.c_str());
   while( dwBytesReaded == dwByteToRead -2 )
   {
      dwByteToRead *= 2;
      if(pszTemp)
         delete[]pszTemp;
      pszTemp = new char[dwByteToRead];
      dwBytesReaded = ::GetPrivateProfileSectionNames(pszTemp, dwByteToRead, m_szFileName.c_str());
   }//end while
  
   pNextSection = pszTemp;
   while(*pNextSection != NULL)
   {
      section = new SECTION;
      section->szSection = pNextSection;
      m_LstINI.push_back(section);
      pNextSection += strlen(pNextSection) + 1;
   }//end while

   // For each section fill the key
   if( !m_LstINI.empty() )
   {
      std::vector<SECTION*>::iterator Iter;
      for (Iter = m_LstINI.begin(); Iter != m_LstINI.end(); Iter++)
      {
         section = *Iter;
         // Read keys
         dwByteToRead = MAX_CHARACTERS;
         dwBytesReaded = ::GetPrivateProfileSection( section->szSection.c_str(), pszTempKey, dwByteToRead, m_szFileName.c_str());
         while( dwBytesReaded == dwByteToRead -2 )
         {
            dwByteToRead *= 2;
            if(pszTempKey)
               delete []pszTempKey;
            pszTempKey = new char[dwByteToRead];
            dwBytesReaded = ::GetPrivateProfileSection( section->szSection.c_str(), pszTempKey, dwByteToRead, m_szFileName.c_str());
         }//end while

         pNextKey = pszTempKey;
         while(*pNextKey != NULL)
         {
            key = new KEYS;
            if( ExtractKeyName(pNextKey, szTemp) )
               key->szKeyName = szTemp;
            else
               key->szKeyName.erase();

            if( ExtractKeyValue(pNextKey, szTemp) )
               key->szValue = szTemp;
            else
               key->szValue.erase();

            section->LstKeys.push_back(key);
            pNextKey += strlen(pNextKey) + 1;
         }//end while
      }
   }//end if

   if( pszTemp )
      delete []pszTemp;
   if( pszTempKey )
      delete []pszTempKey;
}

//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   ExtractKeyName
//
/*!Access:        Private

   Parameters

   IN             Description

   <LPCTSTR>      The Key from INI file.

   <string&>      Destination string to store result.

   OUT            Description

   <bool>         true if succeeded.

   Notes:         When using GetPrivateProfileSection(), we have the KeyName
                  and the data. We only want the KeyName.
*/
//--------------------------------------------------------------------------
bool CINITools::ExtractKeyName(LPCTSTR _szKey, std::string& _szDestination)
{
   bool  bResult = false;
   _szDestination.erase();
   const char* p;
   // Check if we have the Charac '='
   p = ::strchr(_szKey, '=');
   if ( p != NULL )
   {
      int iCount=0;
		p = (char*)_szKey;
		do
		{
			if ( *p !=  '=' )
			{
				_szDestination += *p;
				iCount++;
			}
			else
            bResult = true;
			p++;
		}while( !bResult);
   }
   return bResult;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   ExtractKeyValue
//
/*!Access:        Protected

   Parameters

   IN             Description

   <LPCTSTR>      The key to analyse

   <string&>      Destination string to store result.

   OUT            Description

   <bool>         true if value founded.

   Notes:         Must has '=' in the input parameter.
*/
//--------------------------------------------------------------------------
bool CINITools::ExtractKeyValue(const char* _pszKey, std::string& _szDestination)
{
   _szDestination.erase();
   const char* p;
   p = ::strchr(_pszKey, '=');
	if ( p != NULL )
	{
		p++;
      _szDestination = p;
      return true;
   }
   return false;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   CheckSectionKey
//
/*!Access:        Private

   Parameters

   IN             Description

   <char*>        The Section name.

   <char*>        The key name.

   OUT            Description

   <bool>         true if founded.

   Notes:         Call GetErrorMessage() if failed.
*/
//--------------------------------------------------------------------------
bool CINITools::CheckSectionKey(const char* _szSection, const char* _szKey)
{
   bool bResult = false;
   std::string szTemp;

   if( _szSection == NULL || _szKey == NULL 
      || strlen(_szSection) == 0 || strlen(_szKey) == 0 )
   {
      m_szErrorMessage="Missing parameters";
      return false;
   }

   if( !CheckSection(_szSection) )
      return false;

   if( !m_LstINI.empty() )
   {
      for(int i=0; i<(int)m_LstINI.size(); i++)
      {
         SECTION* pSection = m_LstINI[i];
         if( _strnicmp(pSection->szSection.c_str(), _szSection, strlen(_szSection)) == 0 )
         {
            if( !pSection->LstKeys.empty() )
            {
               for(int j=0; j<(int)pSection->LstKeys.size(); j++)
               {
                  KEYS* pKey= pSection->LstKeys[j];
                  if( _strnicmp(pKey->szKeyName.c_str(), _szKey, strlen(_szSection)) == 0 )
                  {
                     bResult = true;;
                     break;
                  }//end if
               }//end for
            }//end if
         }//end if
      }//end ofr
   }//end if
   return bResult;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   CheckSection
//
/*!Access:        Private

   Parameters

   IN             Description

   <char*>        The section to check.

   OUT            Description

   <bool>         true if ok else false.

   Notes:         Call GetErrorMessage() if failed.
*/
//--------------------------------------------------------------------------
bool CINITools::CheckSection(const char* _szSection)
{
   bool bResult = false;

   if( strlen(_szSection) > 0 )
   {
      for(int i=0; i<(int)m_LstINI.size(); i++)
      {
         SECTION* pSection = m_LstINI[i];
         if( _strnicmp(pSection->szSection.c_str(), _szSection, strlen(_szSection)) == 0 )
         {
            bResult = true;
            break;
         }//end if
      }//end for
   }//end if
   return bResult;
}


///////////////////////////////////////////////////////////////////////////
//                         Public Functions
///////////////////////////////////////////////////////////////////////////



//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   SetIniFileName
//
/*!Access:        Public

   Parameters

   IN             Description

   <char*>        The INI filename.

   OUT            Description

   <bool>         true if no error, if false use GetErrorMessage().

   Notes          If no fullpath then current directory is assumed.
                  We must build the full path cause of old API style.
*/
//--------------------------------------------------------------------------
bool CINITools::SetIniFileName(const char* _szFileName)
{
   bool bResult = false;
   char* pszPathName = new char[MAX_PATH];
	char* pszTemp = new char[MAX_PATH];
   
   if( ::strlen(_szFileName) > 0)
   {
      m_szFileName = _szFileName;
      
      if( IsFileExist(m_szFileName.c_str()) )
      {
         // Fill Internal Structures
         GetIniInfos();
         bResult = true;
      }
      else
      {  
         m_szErrorMessage = "File not founded";
      }
   }  
   else
   {
      m_szErrorMessage = "FileName empty.";
   }
   
   if( pszPathName )
      delete []pszPathName;
   if( pszTemp )
      delete []pszTemp;

   return bResult;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   GetINIValue
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      Section Name.

   <LPCTSTR>      Key Name.

   <LPCTSTR>      Return value if not founded (Default = "").

   OUT            Description

   <LPCTSTR>      Return string value.

   Notes:         Use to read a string value.
                  Section and Keyname are not key sensitive.
*/
//--------------------------------------------------------------------------
LPCTSTR CINITools::GetINIValue(LPCTSTR _szSection, LPCTSTR _szKey, LPCTSTR _szDefault)
{
   m_szTemp = _szDefault;

   if( ::strlen(_szSection) > 0 && strlen(_szKey) > 0 )
   {
      for(int i=0; i<(int)m_LstINI.size(); i++)
      {
         SECTION* pSection = m_LstINI[i];
         if( _strnicmp(pSection->szSection.c_str(), _szSection, ::strlen(_szSection))  == 0 ) 
         {
            for(int j=0; j<(int)pSection->LstKeys.size(); j++)
            {
               KEYS* pKey= pSection->LstKeys[j];
               if( _strnicmp(pKey->szKeyName.c_str(), _szKey, strlen(_szKey))  == 0 )
               {
                  m_szTemp = pKey->szValue;
                  break;
               }//end if
            }//end for
         }//end if
      }//end for
   }

   return m_szTemp.c_str();
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   WriteValue
//
/*!Access:        Public

   Parameters

   IN             Description

   <char>         Section Name.

   <char>         Key Name.

   <char>         The string value to write.

   OUT            Description

   <bool>         true if no error.

   Notes:         Write a string value.

*/
//--------------------------------------------------------------------------
bool CINITools::WriteINIValue(LPCTSTR _szSection, LPCTSTR _szKey, LPCTSTR _szValue)
{
   if( CheckSection(_szSection) )
   {
      if(!::WritePrivateProfileString(_szSection, _szKey, _szValue, m_szFileName.c_str()) )
      {
         m_szErrorMessage = "Key value not written.";
      }
      else 
         return true;
   }
   return false;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   WriteINIValueInt
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      Section Name.

   <LPCTSTR>      Key Name

   <int>          Integer value.

   OUT            Description

   <bool>         true if ok.

   Notes:         Write an integer value.
*/
//--------------------------------------------------------------------------
bool CINITools::WriteINIValueInt(LPCTSTR _szSection, LPCTSTR _szKey, int _iValue)
{
   char  szValue[20];

   _itoa_s(_iValue, szValue, 20,10);
   return(WriteINIValue(_szSection, _szKey, szValue));
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   GetINIValueInt
//
/*!Access:        Public

   Parameters

   IN             Description

   <char>         Section Name.

   <char>         Key Name.

   <int>          Default value if not founded.

   OUT            Description

   <int>          The value readed.

   Notes:         Section and Keyname are not key sensitive.
*/
//--------------------------------------------------------------------------
int CINITools::GetINIValueInt(const char* _szSection, const char* _szKey, int _iDefaultVal)
{
   volatile int   iVal = _iDefaultVal;
   m_szTemp.erase();
   
   char strDef[16];
   sprintf_s(strDef,16,"%d",_iDefaultVal);
   GetINIValue(_szSection, _szKey,strDef);
   iVal = atoi(m_szTemp.c_str());    
   return iVal;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   WriteValueDbl
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      Section Name.

   <LPCTSTR>      Key Name

   <double>       Double value.

   OUT            Description

   <bool>         true if no error.

   Notes:         Write an integer value.
*/
//--------------------------------------------------------------------------
bool CINITools::WriteINIValueDbl(LPCTSTR _szSection, LPCTSTR _szKey, double _dValue)
{
   char  szValue[20];

   // 6 digits precision
   sprintf_s(szValue,20, "%.6f", _dValue);
   return(WriteINIValue(_szSection, _szKey, szValue));
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   GetINIValueDbl
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      Section Name.

   <LPCTSTR>      Key Name.

   <double>       Default(0) value if not founded.

   OUT            Description

   <double>       The value readed.

   Notes:         Section and Keyname are not key sensitive.
*/
//--------------------------------------------------------------------------
double CINITools::GetINIValueDbl(LPCTSTR _szSection, LPCTSTR _szKey, double _dDefaultVal)
{
   volatile double   dVal = _dDefaultVal;
   
   m_szTemp = GetINIValue(_szSection, _szKey);
   if( m_szTemp.length() > 0 )
   {
      dVal = ::atof(m_szTemp.c_str());    
   }
   return dVal;
}



//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   WriteValueFlt
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      Section Name.

   <LPCTSTR>      Key Name

   <float>        Float value.

   OUT            Description

   <bool>         true if no error.

   Notes:         Write an integer value.
*/
//--------------------------------------------------------------------------
bool CINITools::WriteINIValueFlt(LPCTSTR _szSection, LPCTSTR _szKey, float _fValue)
{
   char  szValue[20];

   //  3 digits precision
   sprintf_s(szValue,20, "%.3f", _fValue);
   return( WriteINIValue(_szSection, _szKey, szValue) );
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   GetINIValueFlt
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      Section Name.

   <LPCTSTR>      Key Name.

   <float>        Default(0) value if not founded.

   OUT            Description

   <float>        The value readed.

   Notes:         Section and Keyname are not key sensitive.
*/
//--------------------------------------------------------------------------
float  CINITools::GetINIValueFlt(LPCTSTR _szSection, LPCTSTR _szKey, float  _fDefaultVal)
{
   volatile float fVal = _fDefaultVal;
   
   m_szTemp = GetINIValue(_szSection, _szKey);
   if( m_szTemp.length() > 0 )
   {
      fVal = (float)::atof(m_szTemp.c_str());    
   }
   return fVal;
}



//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   AddKey
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      The section Name.

   <LPCTSTR>      The new key to add.

   <LPCTSTR>      Default value for the key.(Default '0')

   OUT            Description

   <bool>         true if no error.

   Notes:         If Value to specify then 0 is the default.
*/
//--------------------------------------------------------------------------
bool CINITools::AddKey(LPCTSTR _szSection, LPCTSTR _szKey, LPCTSTR _szValue)
{
   if( m_szFileName.empty() )
   {
      m_szErrorMessage ="File not founded";
      return false;
   }

   if( CheckSection(_szSection) )
   {
      //* Founded section.
      if(!::WritePrivateProfileString(_szSection, _szKey, _szValue, m_szFileName.c_str()) )
      {
         m_szErrorMessage = "Key value not written.";
         return false;
      }
      else
      {
         // reload INI structure.
         GetIniInfos(); 
         return true;
      }
   }
   return false;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   RemoveKey
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      The section Name.

   <LPCTSTR>      The new key to add.

   OUT            Description

   <bool>         true if no error.

   Notes:         If Value to specify then 0 is the default.
*/
//--------------------------------------------------------------------------
bool CINITools::RemoveKey(LPCTSTR _szSection, LPCTSTR _szKey)
{
   if( CheckSectionKey(_szSection, _szKey) )
   {
      // Founded section & Key
      if(!::WritePrivateProfileString(_szSection, _szKey, NULL, m_szFileName.c_str()) )
      {
         m_szErrorMessage = "Key value not written.";
         return false;
      }
      else
      {
         // reload INI structure.
         GetIniInfos(); 
         return true;
      }
   }
   return false;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   AddSection
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      The new Section to add.

   OUT            Description

   <bool>         true if no error.

   Notes:         We create a dummy key and value.
                  Add the new section.
                  Then remove the dummy key.
*/
//--------------------------------------------------------------------------
bool CINITools::AddSection(LPCTSTR _szSection)
{
   if( m_szFileName.empty() )
   {
      m_szErrorMessage ="File not founded";
      return false;
   }

   if( !CheckSection(_szSection) )
   {
      if(!::WritePrivateProfileString(_szSection, "3YIU9", "0", m_szFileName.c_str()) )
      {
         m_szErrorMessage = "Key value not written.";
         return false;
      }
      else
      {
         GetIniInfos();
         return (RemoveKey(_szSection, "3YIU9"));
      }
   }
   else
   {
      m_szErrorMessage = "Section founded in INI File.";
      return false;
   }
   return false;
}


//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   RemoveSection
//
/*!Access:        Public

   Parameters

   IN             Description

   <LPCTSTR>      The section Name.


   OUT            Description

   <bool>         true if no error.

   Notes:         All associate Key to the section are deleted.
*/
//--------------------------------------------------------------------------
bool CINITools::RemoveSection(LPCTSTR _szSection)
{
   if( m_szFileName.empty() )
   {
      m_szErrorMessage ="File not founded";
      return false;
   }

   if( CheckSection(_szSection) )
   {
      // Founded section & Key
      if(!::WritePrivateProfileString(_szSection, NULL, NULL, m_szFileName.c_str()) )
      {
         m_szErrorMessage = "Key value not written.";
         return false;
      }
      else
      {
         GetIniInfos();
         return true;
      }
   }
   else
   {
      m_szErrorMessage = "Section not founded.";
      return false;
   }
   return false;
}



//-METHOD IMPLEMENTATION----------------------------------------------------
//
// Method Name:   GetSectionByIndex
//
/*!Access:        Public

   Parameters

   IN             Description

   <int>          Item Index


   OUT            Item name

   <LPCTSTR>      Section Name

   Notes:         Return Item Name by Index
*/
//--------------------------------------------------------------------------
LPCTSTR CINITools::GetSectionByIndex(int _iIndex)
{
   m_szTemp.erase();

	if( !m_LstINI.empty() )
	{
      _iIndex = _iIndex < 0 ? 0: _iIndex;
      _iIndex = _iIndex > (int)m_LstINI.size()-1 ? (int)m_LstINI.size()-1: _iIndex;
      SECTION* pSection = m_LstINI[_iIndex];
      if( pSection )
         m_szTemp = ((char*)pSection->szSection.c_str());
	}
	return m_szTemp.c_str();
}
