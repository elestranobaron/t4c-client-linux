// T4CConfigDlg.h : header file
//

#if !defined(AFX_T4CCONFIGDLG_H__44060B1E_76F4_423C_9B22_BF75DEEF84F3__INCLUDED_)
#define AFX_T4CCONFIGDLG_H__44060B1E_76F4_423C_9B22_BF75DEEF84F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxtempl.h>
#include "GUILocalString.h"
#include "INITools.h"
#include "Controls\ColoredListCtrl.h"



/////////////////////////////////////////////////////////////////////////////
// CT4CConfigDlg dialog

class CT4CConfigDlg : public CDialog
{
// Construction
public:
	CT4CConfigDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CT4CConfigDlg)
	enum { IDD = IDD_T4CCONFIG_DIALOG };
	CMFCColorButton 	m_btnColor;
	CColoredListCtrl	m_ListColor;
	int		m_dwCBLanguage;
	BOOL	m_bWindowed;
   BOOL  m_bUseMMX;
	BOOL	m_bRes640;
	BOOL	m_bRes800;
	BOOL	m_bRes1024;
	BOOL	m_bRes1280;
	BOOL	m_bCharacter;
	BOOL	m_bChatter;
	BOOL	m_bGroup;
	BOOL	m_bInventory;
	BOOL	m_bMacro;
	BOOL	m_bOption;
	BOOL	m_bScreenShot;
	BOOL	m_bSpell;
	BOOL	m_bTrade;
	CString	m_chCharacter;
	CString	m_chChatter;
	CString	m_chGroup;
	CString	m_chInventory;
	CString	m_chMacro;
	CString	m_chOption;
	CString	m_chScreenShot;
	CString	m_chSpell;
	CString	m_chTrade;
	CString	m_strMp3PlayList;
	BOOL	m_bDisableMusicConnect;
	BOOL	m_bRes1280x720;
	BOOL	m_bRes1280x800;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CT4CConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CT4CConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnChkResol640();
	afx_msg void OnChkResol800();
	afx_msg void OnChkResol1024();
	afx_msg void OnChkResol1280();
	afx_msg void OnSelchangeCbLanguage();
	virtual void OnOK();
	afx_msg void OnChkWindowed();
	virtual void OnCancel();
	afx_msg void OnChkInventory();
	afx_msg void OnChkCharacterui();
	afx_msg void OnChkChatterui();
	afx_msg void OnChkGroupui();
	afx_msg void OnChkSpellui();
	afx_msg void OnChkMacroui();
	afx_msg void OnChkTrade();
	afx_msg void OnChkOption();
	afx_msg void OnChkScreenshot();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnMod();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnRestaureDefaultColor();
	afx_msg void OnBtnSave();
	afx_msg void OnChangeEditMacro();
	afx_msg void OnBtnMp3Browse();
	afx_msg void OnChkConnectMusic();
	afx_msg void OnChkResol1280x720();
	afx_msg void OnChkResol1280x800();
	//}}AFX_MSG
   afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
	DECLARE_MESSAGE_MAP()


public:

protected:
   typedef struct _sConfigParam
   {
      int dwNbrColor;
      CArray <int,int> CCColor;
      BOOL  bWindowed;
      BOOL  bUSEMMX;
      BOOL  bUSEVBOX;
      BOOL  bDisConMusic;
      DWORD dwResolution;
      int  dwLanguage;
      BOOL bMacro[12];
      BYTE chMacroKey[12];
   }sConfigParam;
   sConfigParam m_sConfig;



   CString m_strPathSave;
   GUILocalString m_Language;
   COLORREF m_crSelected;

   BOOL m_bConfigChange;

   CINITools    m_iniTools;


public:


protected:
   void CT4CConfigDlg::ReadConfig();
   void CT4CConfigDlg::WriteConfig();
   void UpdateAllControlTXT();
   void DrawListColor();

public:
   afx_msg void OnBnClickedBtnColor();
   
   afx_msg void OnBnClickedChkUsemmx();
   BOOL m_bGuild;
   CString m_chGuild;
   afx_msg void OnBnClickedChkGuildid();
   afx_msg void OnBnClickedChkProfession();
   BOOL m_bProfession;
   CString m_chProfession;
   afx_msg void OnBnClickedChkVbox();
   BOOL m_bUseVBox;
   BOOL m_bTakeItemID;
   afx_msg void OnBnClickedChkTakeItemId();
   CString m_chTakeItemID;
   afx_msg void OnEnChangeEditTakeItemId();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_T4CCONFIGDLG_H__44060B1E_76F4_423C_9B22_BF75DEEF84F3__INCLUDED_)
