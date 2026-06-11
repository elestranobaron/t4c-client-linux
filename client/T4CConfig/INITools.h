//--FILE DEFINITION-----------------------------------------------------------
//
/*! \file
   File name:  INITools.h(CINITools)

   Author:     Yves Lessard

   Date:       2002/07/01(YYYY/MM/DD)

   Library:		WIN32

   Version:    1.09

   Usage:      Reading & Writing Ini File Like.
	
   Notes:		


   Modifications:
   Name        Date        Notes
   Y.Lessard   2003/05/09  Added GetSectionByIndex() function
   Y.Lessard   2003/09/22  Simplify some Functions()
   Y.Lessard   2005/10/31  Remove Warnings for NET2003
*/
//----------------------------------------------------------------------------
#if !defined(AFX_INITOOLS_H__3578D681_51E6_4FC4_910F_13E766C73646__INCLUDED_)
#define AFX_INITOOLS_H__3578D681_51E6_4FC4_910F_13E766C73646__INCLUDED_



//--FILE INCLUDES-------------------------------------------------------------
#include <windows.h>
// Turn OFF warning from STL
#pragma warning (disable: 4786 4788)
#include <vector>
#include <string>
#pragma warning (default: 4786 4788)

//--TYPE DEFINITIONS----------------------------------------------------------
#define  MAX_CHARACTERS    1024


//--CLASS DEFINITION----------------------------------------------------------
//
//  Class name:   CINITools
//
/*! Notes:        
*/
//----------------------------------------------------------------------------
class CINITools  
{
public:
   //--CONSTRUCTOR-------------------------------------
	CINITools();
   //--DESTRUCTOR--------------------------------------
	virtual ~CINITools();

public:
   //--IN-CLASS DEFINITIONS--------------------------------------------------
 
public:
   //--PUBLIC METHODS----------------------------------
	bool     SetIniFileName(const char* _szFileName);
   LPCTSTR  GetINIValue(LPCTSTR _szSection, LPCTSTR _szKey, LPCTSTR _szDefault= "");
   bool     WriteINIValue(LPCTSTR _szSection, LPCTSTR _szKey, LPCTSTR _szValue);
   bool     WriteINIValueInt(LPCTSTR _szSection, LPCTSTR _szKey, int _iValue);
   int      GetINIValueInt(const char* _szSection, const char* _szKey,int _iDefaultVal=0);
   bool     WriteINIValueDbl(LPCTSTR _szSection, LPCTSTR _szKey, double _dValue);
   double   GetINIValueDbl(LPCTSTR _szSection, LPCTSTR _szKey, double  _dDefaultVal=0);
   bool     WriteINIValueFlt(LPCTSTR _szSection, LPCTSTR _szKey, float _fValue);
   float    GetINIValueFlt(LPCTSTR _szSection, LPCTSTR _szKey, float  _fDefaultVal=0);
	bool     AddKey(LPCTSTR _szSection, LPCTSTR _szKey,LPCTSTR _szValue="0");
   bool     RemoveKey(LPCTSTR _szSection, LPCTSTR _szKey);
   bool     AddSection(LPCTSTR _szSection);
   bool     RemoveSection(LPCTSTR _szSection);
   bool     CheckSectionKey(const char* _szSection, const char* _szKey);
   bool     CheckSection(const char* _szSection);
   int      GetSectionCount() { return (int)m_LstINI.size(); };
	LPCTSTR  GetSectionByIndex(int _iIndex);


   // Helpers
   inline const char*  GetErrorMessage() { return m_szErrorMessage.c_str(); };

private:
   //--PRIVATE ATTRIBUTES------------------------------
   std::string    m_szFileName;
   typedef struct _KEYS
   {
      std::string    szKeyName;
      std::string    szValue;
   }KEYS;
   typedef struct _SECTION
   {
      std::vector<KEYS*>LstKeys;
      std::string    szSection;
   }SECTION;

   std::vector<SECTION*>m_LstINI;
   std::string          m_szTemp;
   std::string          m_szErrorMessage;

   //--PRIVATE METHODS---------------------------------
   bool     IsFileExist(LPCTSTR  _szFileName);
   bool     ExtractKeyName(LPCTSTR _szKey, std::string& _szDestination);
   bool     ExtractKeyValue(LPCTSTR _pszKey, std::string& _szDestination);
	void     ResetData();
	void     GetIniInfos();
   

};
#endif // !defined(AFX_INITOOLS_H__3578D681_51E6_4FC4_910F_13E766C73646__INCLUDED_)
