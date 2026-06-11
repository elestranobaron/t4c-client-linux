// T4CConfig.h : main header file for the T4CCONFIG application
//

#if !defined(AFX_T4CCONFIG_H__12D32DF0_8166_4A6D_874E_7FE4D39123EF__INCLUDED_)
#define AFX_T4CCONFIG_H__12D32DF0_8166_4A6D_874E_7FE4D39123EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CT4CConfigApp:
// See T4CConfig.cpp for the implementation of this class
//

class CT4CConfigApp : public CWinApp
{
public:
	CT4CConfigApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CT4CConfigApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CT4CConfigApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_T4CCONFIG_H__12D32DF0_8166_4A6D_874E_7FE4D39123EF__INCLUDED_)
