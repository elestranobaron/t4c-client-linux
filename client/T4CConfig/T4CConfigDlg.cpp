// T4CConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "T4CConfig.h"
#include "T4CConfigDlg.h"
#include "Zlib/zlib.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  MEM_COMPRESS_NEED(ActualLen) (ULONG)(ActualLen *1.1 + 12);

#define  SAVE_FILE_VERSION  5

 
/////////////////////////////////////////////////////////////////////////////
// CT4CConfigDlg dialog

CT4CConfigDlg::CT4CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CT4CConfigDlg::IDD, pParent)
   , m_bUseMMX(FALSE)
   , m_bGuild(FALSE)
   , m_chGuild(_T(""))
   , m_bProfession(FALSE)
   , m_chProfession(_T(""))
   , m_bUseVBox(FALSE)
   , m_bTakeItemID(FALSE)
   , m_chTakeItemID(_T(""))
{
	//{{AFX_DATA_INIT(CT4CConfigDlg)
	m_dwCBLanguage = -1;
	m_bWindowed = FALSE;
   m_bUseMMX   = FALSE;
	m_bRes640 = FALSE;
	m_bRes800 = FALSE;
	m_bRes1024 = FALSE;
	m_bRes1280 = FALSE;
	m_bCharacter = FALSE;
	m_bChatter = FALSE;
	m_bGroup = FALSE;
	m_bInventory = FALSE;
	m_bMacro = FALSE;
	m_bOption = FALSE;
	m_bScreenShot = FALSE;
	m_bSpell = FALSE;
	m_bTrade = FALSE;
	m_chCharacter = _T("");
	m_chChatter = _T("");
	m_chGroup = _T("");
	m_chInventory = _T("");
	m_chMacro = _T("");
	m_chOption = _T("");
	m_chScreenShot = _T("");
	m_chSpell = _T("");
	m_chTrade = _T("");
	m_strMp3PlayList = _T("");
	m_bDisableMusicConnect = FALSE;
	m_bRes1280x720 = FALSE;
	m_bRes1280x800 = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CT4CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CT4CConfigDlg)
   DDX_Control(pDX, IDC_BTN_COLOR, m_btnColor);
   DDX_Control(pDX, IDC_LIST_COLOR, m_ListColor);
   DDX_CBIndex(pDX, IDC_CB_LANGUAGE, m_dwCBLanguage);
   DDX_Check(pDX, IDC_CHK_WINDOWED, m_bWindowed);
   DDX_Check(pDX, IDC_CHK_RESOL_640, m_bRes640);
   DDX_Check(pDX, IDC_CHK_RESOL_800, m_bRes800);
   DDX_Check(pDX, IDC_CHK_RESOL_1024, m_bRes1024);
   DDX_Check(pDX, IDC_CHK_RESOL_1280, m_bRes1280);
   DDX_Check(pDX, IDC_CHK_CHARACTERUI, m_bCharacter);
   DDX_Check(pDX, IDC_CHK_CHATTERUI, m_bChatter);
   DDX_Check(pDX, IDC_CHK_GROUPUI, m_bGroup);
   DDX_Check(pDX, IDC_CHK_INVENTORY, m_bInventory);
   DDX_Check(pDX, IDC_CHK_MACROUI, m_bMacro);
   DDX_Check(pDX, IDC_CHK_OPTION, m_bOption);
   DDX_Check(pDX, IDC_CHK_SCREENSHOT, m_bScreenShot);
   DDX_Check(pDX, IDC_CHK_SPELLUI, m_bSpell);
   DDX_Check(pDX, IDC_CHK_TRADE, m_bTrade);
   DDX_Text(pDX, IDC_EDIT_CHARACTERUI, m_chCharacter);
   DDX_Text(pDX, IDC_EDIT_CHATTER, m_chChatter);
   DDX_Text(pDX, IDC_EDIT_GROUPUI, m_chGroup);
   DDX_Text(pDX, IDC_EDIT_INVENTORY, m_chInventory);
   DDX_Text(pDX, IDC_EDIT_MACROUI, m_chMacro);
   DDX_Text(pDX, IDC_EDIT_OPTION, m_chOption);
   DDX_Text(pDX, IDC_EDIT_SCREENSHOT, m_chScreenShot);
   DDX_Text(pDX, IDC_EDIT_SPELLUI, m_chSpell);
   DDX_Text(pDX, IDC_EDIT_TRADE, m_chTrade);
   DDX_Text(pDX, IDC_MP3_PLAYLIST_FILE, m_strMp3PlayList);
   DDX_Check(pDX, IDC_CHK_CONNECT_MUSIC, m_bDisableMusicConnect);
   DDX_Check(pDX, IDC_CHK_RESOL_1280X720, m_bRes1280x720);
   DDX_Check(pDX, IDC_CHK_RESOL_1280X800, m_bRes1280x800);
   //}}AFX_DATA_MAP
   DDX_Check(pDX, IDC_CHK_USEMMX, m_bUseMMX);
   DDX_Check(pDX, IDC_CHK_GUILDID, m_bGuild);
   DDX_Text(pDX, IDC_EDIT_GUILDID, m_chGuild);
   DDX_Check(pDX, IDC_CHK_PROFESSION, m_bProfession);
   DDX_Text(pDX, IDC_EDIT_PROFESSION, m_chProfession);
   DDX_Check(pDX, IDC_CHK_VBOX, m_bUseVBox);
   DDX_Check(pDX, IDC_CHK_TAKE_ITEM_ID, m_bTakeItemID);
   DDX_Text(pDX, IDC_EDIT_TAKE_ITEM_ID, m_chTakeItemID);
}

BEGIN_MESSAGE_MAP(CT4CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CT4CConfigDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHK_RESOL_640, OnChkResol640)
	ON_BN_CLICKED(IDC_CHK_RESOL_800, OnChkResol800)
	ON_BN_CLICKED(IDC_CHK_RESOL_1024, OnChkResol1024)
	ON_BN_CLICKED(IDC_CHK_RESOL_1280, OnChkResol1280)
	ON_CBN_SELCHANGE(IDC_CB_LANGUAGE, OnSelchangeCbLanguage)
	ON_BN_CLICKED(IDC_CHK_WINDOWED, OnChkWindowed)
	ON_BN_CLICKED(IDC_CHK_INVENTORY, OnChkInventory)
	ON_BN_CLICKED(IDC_CHK_CHARACTERUI, OnChkCharacterui)
	ON_BN_CLICKED(IDC_CHK_CHATTERUI, OnChkChatterui)
	ON_BN_CLICKED(IDC_CHK_GROUPUI, OnChkGroupui)
	ON_BN_CLICKED(IDC_CHK_SPELLUI, OnChkSpellui)
	ON_BN_CLICKED(IDC_CHK_MACROUI, OnChkMacroui)
	ON_BN_CLICKED(IDC_CHK_TRADE, OnChkTrade)
	ON_BN_CLICKED(IDC_CHK_OPTION, OnChkOption)
	ON_BN_CLICKED(IDC_CHK_SCREENSHOT, OnChkScreenshot)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_MOD, OnBtnMod)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDC_BTN_RESTAURE_DEFAULT_COLOR, OnBtnRestaureDefaultColor)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_EN_CHANGE(IDC_EDIT_INVENTORY, OnChangeEditMacro)
	ON_BN_CLICKED(IDC_BTN_MP3_BROWSE, OnBtnMp3Browse)
	ON_BN_CLICKED(IDC_CHK_CONNECT_MUSIC, OnChkConnectMusic)
	ON_BN_CLICKED(IDC_CHK_RESOL_1280X720, OnChkResol1280x720)
	ON_EN_CHANGE(IDC_EDIT_CHARACTERUI, OnChangeEditMacro)
	ON_EN_CHANGE(IDC_EDIT_CHATTER, OnChangeEditMacro)
	ON_EN_CHANGE(IDC_EDIT_GROUPUI, OnChangeEditMacro)
	ON_EN_CHANGE(IDC_EDIT_SPELLUI, OnChangeEditMacro)
	ON_EN_CHANGE(IDC_EDIT_MACROUI, OnChangeEditMacro)
	ON_EN_CHANGE(IDC_EDIT_TRADE, OnChangeEditMacro)
	ON_EN_CHANGE(IDC_EDIT_OPTION, OnChangeEditMacro)
	ON_EN_CHANGE(IDC_EDIT_SCREENSHOT, OnChangeEditMacro)
	ON_BN_CLICKED(IDC_CHK_RESOL_1280X800, OnChkResol1280x800)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_BTN_COLOR, &CT4CConfigDlg::OnBnClickedBtnColor)
   ON_BN_CLICKED(IDC_CHK_USEMMX, &CT4CConfigDlg::OnBnClickedChkUsemmx)
   ON_BN_CLICKED(IDC_CHK_GUILDID, &CT4CConfigDlg::OnBnClickedChkGuildid)
   ON_BN_CLICKED(IDC_CHK_PROFESSION, &CT4CConfigDlg::OnBnClickedChkProfession)
   ON_BN_CLICKED(IDC_CHK_VBOX, &CT4CConfigDlg::OnBnClickedChkVbox)
   ON_BN_CLICKED(IDC_CHK_TAKE_ITEM_ID, &CT4CConfigDlg::OnBnClickedChkTakeItemId)
   ON_EN_CHANGE(IDC_EDIT_TAKE_ITEM_ID, &CT4CConfigDlg::OnEnChangeEditTakeItemId)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CT4CConfigDlg message handlers



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CT4CConfigDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CT4CConfigDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CT4CConfigDlg::OnOK() 
{
   return;	
} 

void CT4CConfigDlg::OnCancel() 
{
   UpdateData(TRUE);
   m_sConfig.bMacro[0] = m_bInventory ;
   m_sConfig.bMacro[1] = m_bCharacter ;
   m_sConfig.bMacro[2] = m_bChatter   ;
   m_sConfig.bMacro[3] = m_bGroup     ;
   m_sConfig.bMacro[4] = m_bSpell     ;
   m_sConfig.bMacro[5] = m_bMacro     ;
   m_sConfig.bMacro[6] = m_bTrade     ;
   m_sConfig.bMacro[7] = m_bOption    ;
   m_sConfig.bMacro[8] = m_bScreenShot;
   m_sConfig.bMacro[9] = m_bGuild;
   m_sConfig.bMacro[10]= m_bProfession;
   m_sConfig.bMacro[11]= m_bTakeItemID;

   m_sConfig.chMacroKey[0] = m_chInventory[0]; //Inventory
   m_sConfig.chMacroKey[1] = m_chCharacter[0]; //Charecter
   m_sConfig.chMacroKey[2] = m_chChatter[0];   //Chatter
   m_sConfig.chMacroKey[3] = m_chGroup[0];     //GroupPlay
   m_sConfig.chMacroKey[4] = m_chSpell[0];     //Spell
   m_sConfig.chMacroKey[5] = m_chMacro[0];     //Macro
   m_sConfig.chMacroKey[6] = m_chTrade[0];     //Trade
   m_sConfig.chMacroKey[7] = m_chOption[0];    //Option
   m_sConfig.chMacroKey[8] = m_chScreenShot[0];//Screenshot
   m_sConfig.chMacroKey[9] = m_chGuild[0];     //Guild
   m_sConfig.chMacroKey[10]= m_chProfession[0];//Profession
   m_sConfig.chMacroKey[11]= m_chTakeItemID[0];//Take Item ID

   if(m_bConfigChange)
   {
      if(MessageBox(m_Language[224],m_Language[223],MB_ICONWARNING+MB_YESNO) == IDYES)
      {
         WriteConfig();
      }
   }
	
	CDialog::OnCancel();
}




BOOL CT4CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
  
   char strSavePath[1024];
   //set le path de save...
   if(SHGetSpecialFolderPath(NULL,strSavePath,CSIDL_PERSONAL   ,FALSE) != 1)
      GetTempPath(MAX_PATH,strSavePath);
   strcat_s(strSavePath,"\\Dialsoft");
   CreateDirectory(strSavePath,NULL);
   strcat_s(strSavePath,"\\T4CDEV");
   CreateDirectory(strSavePath,NULL);

   m_strPathSave.Format("%s",strSavePath);

   ReadConfig();

   
   m_bRes640       = FALSE;
   m_bRes800       = FALSE;
   m_bRes1024      = FALSE;
   m_bRes1280      = FALSE;
   m_bRes1280x720  = FALSE;
   m_bRes1280x800  = FALSE;
   
   m_bDisableMusicConnect = m_sConfig.bDisConMusic;
   m_bWindowed            = m_sConfig.bWindowed;
   m_bUseMMX              = m_sConfig.bUSEMMX;
   m_bUseVBox             = m_sConfig.bUSEVBOX;
   if(m_sConfig.dwResolution == 0)
      m_bRes640 = TRUE;
   else if(m_sConfig.dwResolution == 1)
      m_bRes800 = TRUE;
   else if(m_sConfig.dwResolution == 2)
      m_bRes1024 = TRUE;
   else if(m_sConfig.dwResolution == 3)
      m_bRes1280 = TRUE;
   else if(m_sConfig.dwResolution == 4)
      m_bRes1280x720 = TRUE;
   else if(m_sConfig.dwResolution == 5)
      m_bRes1280x800 = TRUE;
   else
      m_bRes1024 = TRUE;

   m_dwCBLanguage = m_sConfig.dwLanguage;

   //set les macro
   m_bInventory = m_sConfig.bMacro[0];
   m_bCharacter = m_sConfig.bMacro[1];
   m_bChatter   = m_sConfig.bMacro[2];
   m_bGroup     = m_sConfig.bMacro[3];
   m_bSpell     = m_sConfig.bMacro[4];
   m_bMacro     = m_sConfig.bMacro[5];
   m_bTrade     = m_sConfig.bMacro[6];
   m_bOption    = m_sConfig.bMacro[7];
   m_bScreenShot= m_sConfig.bMacro[8];
   m_bGuild     = m_sConfig.bMacro[9];
   m_bProfession= m_sConfig.bMacro[10];
   m_bTakeItemID= m_sConfig.bMacro[11];
   
   m_chInventory  .Format("%C",m_sConfig.chMacroKey[0]);//Inventory
   m_chCharacter  .Format("%C",m_sConfig.chMacroKey[1]);//Charecter
   m_chChatter    .Format("%C",m_sConfig.chMacroKey[2]);//Chatter
   m_chGroup      .Format("%C",m_sConfig.chMacroKey[3]);//GroupPlay
   m_chSpell      .Format("%C",m_sConfig.chMacroKey[4]);//Spell
   m_chMacro      .Format("%C",m_sConfig.chMacroKey[5]);//Macro
   m_chTrade      .Format("%C",m_sConfig.chMacroKey[6]);//Trade
   m_chOption     .Format("%C",m_sConfig.chMacroKey[7]);//Option
   m_chScreenShot .Format("%C",m_sConfig.chMacroKey[8]);//Screenshot
   m_chGuild      .Format("%C",m_sConfig.chMacroKey[9]);//Guild
   m_chProfession .Format("%C",m_sConfig.chMacroKey[10]);//Profession
   m_chTakeItemID .Format("%C",m_sConfig.chMacroKey[11]);//Take Item ID

   if(!m_bInventory)
      GetDlgItem(IDC_EDIT_INVENTORY)->EnableWindow(FALSE);
   if(!m_bCharacter)
      GetDlgItem(IDC_EDIT_CHARACTERUI)->EnableWindow(FALSE);
   if(!m_bChatter)
      GetDlgItem(IDC_EDIT_CHATTER)->EnableWindow(FALSE);
   if(!m_bGroup)
      GetDlgItem(IDC_EDIT_GROUPUI)->EnableWindow(FALSE);
   if(!m_bSpell)
      GetDlgItem(IDC_EDIT_SPELLUI)->EnableWindow(FALSE);
   if(!m_bMacro)
      GetDlgItem(IDC_EDIT_MACROUI)->EnableWindow(FALSE);
   if(!m_bTrade)
      GetDlgItem(IDC_EDIT_TRADE)->EnableWindow(FALSE);
   if(!m_bOption)
      GetDlgItem(IDC_EDIT_OPTION)->EnableWindow(FALSE);
   if(!m_bScreenShot)
      GetDlgItem(IDC_EDIT_SCREENSHOT)->EnableWindow(FALSE);
   if(!m_bGuild)
      GetDlgItem(IDC_EDIT_GUILDID)->EnableWindow(FALSE);
   if(!m_bProfession)
      GetDlgItem(IDC_EDIT_PROFESSION)->EnableWindow(FALSE);
   if(!m_bTakeItemID)
      GetDlgItem(IDC_EDIT_TAKE_ITEM_ID)->EnableWindow(FALSE);

   

   m_ListColor.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);
   m_ListColor.InsertColumn(0,"",LVCFMT_LEFT,209);
   
   DrawListColor();


   m_btnColor.EnableAutomaticButton(_T("Automatic"), RGB(255, 0, 255));
   m_btnColor.EnableOtherButton(_T("Other"));
   m_btnColor.SetColor((COLORREF)-1);
   m_btnColor.SetColumnsNumber(10);

   
   UpdateData(FALSE);

   UpdateAllControlTXT();

   m_bConfigChange = FALSE;
	
	return TRUE;  
}

void CT4CConfigDlg::OnDestroy() 
{
   m_Language.Release();
   CDialog::OnDestroy();
}


void CT4CConfigDlg::OnSelchangeCbLanguage() 
{
	UpdateData(TRUE);
   m_sConfig.dwLanguage = m_dwCBLanguage;
   UpdateAllControlTXT();
   m_bConfigChange = TRUE;
}


void CT4CConfigDlg::ReadConfig()
{
   
   CString strConfig;
   strConfig.Format("%s\\T4CConfig.ini",m_strPathSave);

   m_iniTools.SetIniFileName(strConfig);

   if(m_iniTools.CheckSection("SETTINGS"))
   {
      m_sConfig.dwResolution  = m_iniTools.GetINIValueInt("SETTINGS","GAME_RESOLUTION",2);
      m_sConfig.bWindowed     = m_iniTools.GetINIValueInt("SETTINGS","GAME_WINDOWED",1);
      m_sConfig.bUSEMMX       = m_iniTools.GetINIValueInt("SETTINGS","GAME_OLD_MMX",0);
      m_sConfig.bUSEVBOX      = m_iniTools.GetINIValueInt("SETTINGS","GAME_VBOX",0);
      m_sConfig.dwLanguage    = m_iniTools.GetINIValueInt("SETTINGS","GAME_LANGUAGE",0);
      m_sConfig.bDisConMusic  = m_iniTools.GetINIValueInt("SETTINGS","GAME_CONNECT_MUSIC_OFF",0);
      m_strMp3PlayList.Format("%s",m_iniTools.GetINIValue("SETTINGS","GAME_MP3_PLAYLIST"  ,""));
   }
   else
   {
      m_iniTools.AddSection("SETTINGS");
      m_iniTools.AddKey("SETTINGS","GAME_RESOLUTION");
      m_iniTools.AddKey("SETTINGS","GAME_WINDOWED");
      m_iniTools.AddKey("SETTINGS","GAME_OLD_MMX");
      m_iniTools.AddKey("SETTINGS","GAME_VBOX");
      m_iniTools.AddKey("SETTINGS","GAME_LANGUAGE");
      m_iniTools.AddKey("SETTINGS","GAME_CONNECT_MUSIC_OFF");
      m_iniTools.AddKey("SETTINGS","GAME_MP3_PLAYLIST");

      m_sConfig.dwResolution  = 2;
      m_sConfig.bWindowed     = 1;
      m_sConfig.bUSEMMX       = 0;
      m_sConfig.bUSEVBOX      = 0;
      m_sConfig.dwLanguage    = 0;
      m_sConfig.bDisConMusic  = 0;
      m_strMp3PlayList        = "";
      
      m_iniTools.WriteINIValueInt("SETTINGS","GAME_RESOLUTION"        ,m_sConfig.dwResolution);
      m_iniTools.WriteINIValueInt("SETTINGS","GAME_WINDOWED"          ,m_sConfig.bWindowed   );
      m_iniTools.WriteINIValueInt("SETTINGS","GAME_OLD_MMX"           ,m_sConfig.bUSEMMX     );
      m_iniTools.WriteINIValueInt("SETTINGS","GAME_VBOX"              ,m_sConfig.bUSEVBOX     );
      m_iniTools.WriteINIValueInt("SETTINGS","GAME_LANGUAGE"          ,m_sConfig.dwLanguage  );
      m_iniTools.WriteINIValueInt("SETTINGS","GAME_CONNECT_MUSIC_OFF" ,m_sConfig.bDisConMusic);
      m_iniTools.WriteINIValue   ("SETTINGS","GAME_MP3_PLAYLIST"      ,"" );
      
   }

 
   if(m_iniTools.CheckSection("MACRO"))
   {
      m_sConfig.chMacroKey[0]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","INVENTORY_KEY",'I');
      m_sConfig.chMacroKey[1]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","CHARACTER_KEY",'S');
      m_sConfig.chMacroKey[2]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","CHATTER_KEY",'L');
      m_sConfig.chMacroKey[3]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","GROUPPLAY_KEY",'G');
      m_sConfig.chMacroKey[4]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","SPELL_KEY",'P');
      m_sConfig.chMacroKey[5]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","MACRO_KEY",'M');
      m_sConfig.chMacroKey[6]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","TRADE_KEY",'T');
      m_sConfig.chMacroKey[7]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","OPTION_KEY",'O');
      m_sConfig.chMacroKey[8]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","SCREENSHOT_KEY",'H');
      m_sConfig.chMacroKey[9]  = (BYTE)m_iniTools.GetINIValueInt("MACRO","GUILD_KEY",'D');
      m_sConfig.chMacroKey[10] = (BYTE)m_iniTools.GetINIValueInt("MACRO","PROFESSION_KEY",'J');
      m_sConfig.chMacroKey[11] = (BYTE)m_iniTools.GetINIValueInt("MACRO","TAKEITEMID_KEY",'K');

      m_sConfig.bMacro[0]      = m_iniTools.GetINIValueInt("MACRO","INVENTORY_ENABLE",1);
      m_sConfig.bMacro[1]      = m_iniTools.GetINIValueInt("MACRO","CHARACTER_ENABLE",1);
      m_sConfig.bMacro[2]      = m_iniTools.GetINIValueInt("MACRO","CHATTER_ENABLE",1);
      m_sConfig.bMacro[3]      = m_iniTools.GetINIValueInt("MACRO","GROUPPLAY_ENABLE",1);
      m_sConfig.bMacro[4]      = m_iniTools.GetINIValueInt("MACRO","SPELL_ENABLE",1);
      m_sConfig.bMacro[5]      = m_iniTools.GetINIValueInt("MACRO","MACRO_ENABLE",1);
      m_sConfig.bMacro[6]      = m_iniTools.GetINIValueInt("MACRO","TRADE_ENABLE",1);
      m_sConfig.bMacro[7]      = m_iniTools.GetINIValueInt("MACRO","OPTION_ENABLE",1);
      m_sConfig.bMacro[8]      = m_iniTools.GetINIValueInt("MACRO","SCREENSHOT_ENABLE",1);
      m_sConfig.bMacro[9]      = m_iniTools.GetINIValueInt("MACRO","GUILD_ENABLE",1);
      m_sConfig.bMacro[10]     = m_iniTools.GetINIValueInt("MACRO","PROFESSION_ENABLE",1);
      m_sConfig.bMacro[11]     = m_iniTools.GetINIValueInt("MACRO","TAKEITEMID_ENABLE",1);
      
   }
   else
   {
      m_iniTools.AddSection("MACRO");
      m_iniTools.AddKey("MACRO","INVENTORY_KEY");
      m_iniTools.AddKey("MACRO","CHARACTER_KEY");
      m_iniTools.AddKey("MACRO","CHATTER_KEY");
      m_iniTools.AddKey("MACRO","GROUPPLAY_KEY");
      m_iniTools.AddKey("MACRO","SPELL_KEY");
      m_iniTools.AddKey("MACRO","MACRO_KEY");
      m_iniTools.AddKey("MACRO","TRADE_KEY");
      m_iniTools.AddKey("MACRO","OPTION_KEY");
      m_iniTools.AddKey("MACRO","SCREENSHOT_KEY");
      m_iniTools.AddKey("MACRO","TAKEITEMID_KEY");
      m_iniTools.AddKey("MACRO","INVENTORY_ENABLE");
      m_iniTools.AddKey("MACRO","CHARACTER_ENABLE");
      m_iniTools.AddKey("MACRO","CHATTER_ENABLE");
      m_iniTools.AddKey("MACRO","GROUPPLAY_ENABLE");
      m_iniTools.AddKey("MACRO","SPELL_ENABLE");
      m_iniTools.AddKey("MACRO","MACRO_ENABLE");
      m_iniTools.AddKey("MACRO","TRADE_ENABLE");
      m_iniTools.AddKey("MACRO","OPTION_ENABLE");
      m_iniTools.AddKey("MACRO","SCREENSHOT_ENABLE");
      m_iniTools.AddKey("MACRO","TAKEITEMID_ENABLE");

      for(int i=0;i<11;i++)
         m_sConfig.bMacro[i] = TRUE;
      m_sConfig.chMacroKey[0] = 'I' ;//Inventory
      m_sConfig.chMacroKey[1] = 'S' ;//Charecter
      m_sConfig.chMacroKey[2] = 'L' ;//Chatter
      m_sConfig.chMacroKey[3] = 'G' ;//GroupPlay
      m_sConfig.chMacroKey[4] = 'P' ;//Spell
      m_sConfig.chMacroKey[5] = 'M' ;//Macro
      m_sConfig.chMacroKey[6] = 'T' ;//Trade
      m_sConfig.chMacroKey[7] = 'O' ;//Option
      m_sConfig.chMacroKey[8] = 'H' ;//Screenshot
      m_sConfig.chMacroKey[9] = 'D' ;//Guild
      m_sConfig.chMacroKey[10]= 'J' ;//Profession
      m_sConfig.chMacroKey[11]= 'K' ;//Take Item ID

      m_iniTools.WriteINIValueInt("MACRO","INVENTORY_KEY" ,m_sConfig.chMacroKey[0]);
      m_iniTools.WriteINIValueInt("MACRO","CHARACTER_KEY" ,m_sConfig.chMacroKey[1]);
      m_iniTools.WriteINIValueInt("MACRO","CHATTER_KEY"   ,m_sConfig.chMacroKey[2]);
      m_iniTools.WriteINIValueInt("MACRO","GROUPPLAY_KEY" ,m_sConfig.chMacroKey[3]);
      m_iniTools.WriteINIValueInt("MACRO","SPELL_KEY"     ,m_sConfig.chMacroKey[4]);
      m_iniTools.WriteINIValueInt("MACRO","MACRO_KEY"     ,m_sConfig.chMacroKey[5]);
      m_iniTools.WriteINIValueInt("MACRO","TRADE_KEY"     ,m_sConfig.chMacroKey[6]);
      m_iniTools.WriteINIValueInt("MACRO","OPTION_KEY"    ,m_sConfig.chMacroKey[7]);
      m_iniTools.WriteINIValueInt("MACRO","SCREENSHOT_KEY",m_sConfig.chMacroKey[8]);
      m_iniTools.WriteINIValueInt("MACRO","GUILD_KEY"     ,m_sConfig.chMacroKey[9]);
      m_iniTools.WriteINIValueInt("MACRO","PROFESSION_KEY",m_sConfig.chMacroKey[10]);
      m_iniTools.WriteINIValueInt("MACRO","TAKEITEMID_KEY",m_sConfig.chMacroKey[11]);

      m_iniTools.WriteINIValueInt("MACRO","INVENTORY_ENABLE" ,m_sConfig.bMacro[0]);
      m_iniTools.WriteINIValueInt("MACRO","CHARACTER_ENABLE" ,m_sConfig.bMacro[1]);
      m_iniTools.WriteINIValueInt("MACRO","CHATTER_ENABLE"   ,m_sConfig.bMacro[2]);
      m_iniTools.WriteINIValueInt("MACRO","GROUPPLAY_ENABLE" ,m_sConfig.bMacro[3]);
      m_iniTools.WriteINIValueInt("MACRO","SPELL_ENABLE"     ,m_sConfig.bMacro[4]);
      m_iniTools.WriteINIValueInt("MACRO","MACRO_ENABLE"     ,m_sConfig.bMacro[5]);
      m_iniTools.WriteINIValueInt("MACRO","TRADE_ENABLE"     ,m_sConfig.bMacro[6]);
      m_iniTools.WriteINIValueInt("MACRO","OPTION_ENABLE"    ,m_sConfig.bMacro[7]);
      m_iniTools.WriteINIValueInt("MACRO","SCREENSHOT_ENABLE",m_sConfig.bMacro[8]);
      m_iniTools.WriteINIValueInt("MACRO","GUILD_ENABLE"     ,m_sConfig.bMacro[9]);
      m_iniTools.WriteINIValueInt("MACRO","PROFESSION_ENABLE",m_sConfig.bMacro[10]);
      m_iniTools.WriteINIValueInt("MACRO","TAKEITEMID_ENABLE",m_sConfig.bMacro[11]);
   }

   if(m_iniTools.CheckSection("COLOR"))
   {
      m_sConfig.dwNbrColor = 0;
      CString strColorID;
      CString strColorEnable;
      for(int i=0;i<50;i++)
      {
         strColorID    .Format("COLORID%02d",i);
         strColorEnable.Format("COLORENABLE%02d",i);

         DWORD dwColor       = m_iniTools.GetINIValueInt("COLOR",strColorID    ,0);
         DWORD dwColorStatus = m_iniTools.GetINIValueInt("COLOR",strColorEnable,0);
         if(dwColorStatus)
         {
            m_sConfig.dwNbrColor++;
            m_sConfig.CCColor.Add(dwColor);
         }
      }
   }
   else
   {
      CString strColorID;
      CString strColorEnable;

      m_iniTools.AddSection("COLOR");
      for(int i=0;i<50;i++)
      {
         strColorID    .Format("COLORID%02d",i);
         strColorEnable.Format("COLORENABLE%02d",i);

         m_iniTools.AddKey("COLOR",strColorID);
         m_iniTools.AddKey("COLOR",strColorEnable);
      }

      m_sConfig.dwNbrColor = 8;
      m_sConfig.CCColor.Add(RGB(17,181,205));
      m_sConfig.CCColor.Add(RGB(255,33,75));
      m_sConfig.CCColor.Add(RGB(90,119,216));
      m_sConfig.CCColor.Add(RGB(220,202,6));
      m_sConfig.CCColor.Add(RGB(5,197,37));
      m_sConfig.CCColor.Add(RGB(199,10,204));
      m_sConfig.CCColor.Add(RGB(192,192,192));
      m_sConfig.CCColor.Add(RGB(224,100,10));


      for(int i=0;i<50;i++)
      {
         strColorID    .Format("COLORID%02d",i);
         strColorEnable.Format("COLORENABLE%02d",i);

         if(i < m_sConfig.dwNbrColor)
         {
            m_iniTools.WriteINIValueInt("COLOR",strColorID     ,m_sConfig.CCColor[i]);
            m_iniTools.WriteINIValueInt("COLOR",strColorEnable ,1);
         }
         else
         {
            m_iniTools.WriteINIValueInt("COLOR",strColorID     ,0);
            m_iniTools.WriteINIValueInt("COLOR",strColorEnable ,0);
         }
      }
   }
}

void CT4CConfigDlg::WriteConfig()
{
   //Write Settings
   m_iniTools.WriteINIValueInt("SETTINGS","GAME_RESOLUTION"        ,m_sConfig.dwResolution);
   m_iniTools.WriteINIValueInt("SETTINGS","GAME_WINDOWED"          ,m_sConfig.bWindowed   );
   m_iniTools.WriteINIValueInt("SETTINGS","GAME_OLD_MMX"           ,m_sConfig.bUSEMMX     );
   m_iniTools.WriteINIValueInt("SETTINGS","GAME_VBOX"              ,m_sConfig.bUSEVBOX    );
   m_iniTools.WriteINIValueInt("SETTINGS","GAME_LANGUAGE"          ,m_sConfig.dwLanguage  );
   m_iniTools.WriteINIValueInt("SETTINGS","GAME_CONNECT_MUSIC_OFF" ,m_sConfig.bDisConMusic);
   m_iniTools.WriteINIValue   ("SETTINGS","GAME_MP3_PLAYLIST"      ,m_strMp3PlayList );

   //Write Macro
   m_iniTools.WriteINIValueInt("MACRO","INVENTORY_KEY" ,m_sConfig.chMacroKey[0]);
   m_iniTools.WriteINIValueInt("MACRO","CHARACTER_KEY" ,m_sConfig.chMacroKey[1]);
   m_iniTools.WriteINIValueInt("MACRO","CHATTER_KEY"   ,m_sConfig.chMacroKey[2]);
   m_iniTools.WriteINIValueInt("MACRO","GROUPPLAY_KEY" ,m_sConfig.chMacroKey[3]);
   m_iniTools.WriteINIValueInt("MACRO","SPELL_KEY"     ,m_sConfig.chMacroKey[4]);
   m_iniTools.WriteINIValueInt("MACRO","MACRO_KEY"     ,m_sConfig.chMacroKey[5]);
   m_iniTools.WriteINIValueInt("MACRO","TRADE_KEY"     ,m_sConfig.chMacroKey[6]);
   m_iniTools.WriteINIValueInt("MACRO","OPTION_KEY"    ,m_sConfig.chMacroKey[7]);
   m_iniTools.WriteINIValueInt("MACRO","SCREENSHOT_KEY",m_sConfig.chMacroKey[8]);
   m_iniTools.WriteINIValueInt("MACRO","GUILD_KEY"     ,m_sConfig.chMacroKey[9]);
   m_iniTools.WriteINIValueInt("MACRO","PROFESSION_KEY",m_sConfig.chMacroKey[10]);
   m_iniTools.WriteINIValueInt("MACRO","TAKEITEMID_KEY",m_sConfig.chMacroKey[11]);

   m_iniTools.WriteINIValueInt("MACRO","INVENTORY_ENABLE" ,m_sConfig.bMacro[0]);
   m_iniTools.WriteINIValueInt("MACRO","CHARACTER_ENABLE" ,m_sConfig.bMacro[1]);
   m_iniTools.WriteINIValueInt("MACRO","CHATTER_ENABLE"   ,m_sConfig.bMacro[2]);
   m_iniTools.WriteINIValueInt("MACRO","GROUPPLAY_ENABLE" ,m_sConfig.bMacro[3]);
   m_iniTools.WriteINIValueInt("MACRO","SPELL_ENABLE"     ,m_sConfig.bMacro[4]);
   m_iniTools.WriteINIValueInt("MACRO","MACRO_ENABLE"     ,m_sConfig.bMacro[5]);
   m_iniTools.WriteINIValueInt("MACRO","TRADE_ENABLE"     ,m_sConfig.bMacro[6]);
   m_iniTools.WriteINIValueInt("MACRO","OPTION_ENABLE"    ,m_sConfig.bMacro[7]);
   m_iniTools.WriteINIValueInt("MACRO","SCREENSHOT_ENABLE",m_sConfig.bMacro[8]);
   m_iniTools.WriteINIValueInt("MACRO","GUILD_ENABLE"     ,m_sConfig.bMacro[9]);
   m_iniTools.WriteINIValueInt("MACRO","PROFESSION_ENABLE",m_sConfig.bMacro[10]);
   m_iniTools.WriteINIValueInt("MACRO","TAKEITEMID_ENABLE",m_sConfig.bMacro[11]);

   //Write Color
   CString strColorID;
   CString strColorEnable;

   for(int i=0;i<50;i++)
   {
      strColorID    .Format("COLORID%02d",i);
      strColorEnable.Format("COLORENABLE%02d",i);
 
      if(i < m_sConfig.dwNbrColor)
      {
         m_iniTools.WriteINIValueInt("COLOR",strColorID     ,m_sConfig.CCColor[i]);
         m_iniTools.WriteINIValueInt("COLOR",strColorEnable ,1);
      }
      else
      {
         m_iniTools.WriteINIValueInt("COLOR",strColorID     ,0);
         m_iniTools.WriteINIValueInt("COLOR",strColorEnable ,0);
      }
   }
}

void CT4CConfigDlg::UpdateAllControlTXT()
{
   m_Language.Release();
   m_Language.Create(m_sConfig.dwLanguage);
   SetWindowText(m_Language[220]);
   GetDlgItem(IDC_CHK_WINDOWED)     ->SetWindowText(m_Language[144]);
   GetDlgItem(IDC_CHK_USEMMX)       ->SetWindowText(m_Language[365]);
   GetDlgItem(IDC_CHK_VBOX)         ->SetWindowText(m_Language[373]);
   GetDlgItem(IDC_STATIC_GEN_CONFIG)->SetWindowText(m_Language[221]);
   GetDlgItem(IDC_SCREEN_RESOLUTION)->SetWindowText(m_Language[222]);
   GetDlgItem(IDC_BTN_SAVE)         ->SetWindowText(m_Language[366]);
   



   GetDlgItem(IDC_CHK_INVENTORY    )->SetWindowText(m_Language[23]);
   GetDlgItem(IDC_CHK_CHARACTERUI  )->SetWindowText(m_Language[15]);
   GetDlgItem(IDC_CHK_CHATTERUI    )->SetWindowText(m_Language[1]);
   GetDlgItem(IDC_CHK_GROUPUI      )->SetWindowText(m_Language[155]);
   GetDlgItem(IDC_CHK_GUILDID      )->SetWindowText(m_Language[369]);
   GetDlgItem(IDC_CHK_SPELLUI      )->SetWindowText(m_Language[28]);
   GetDlgItem(IDC_CHK_MACROUI      )->SetWindowText(m_Language[16]);
   GetDlgItem(IDC_CHK_TRADE        )->SetWindowText(m_Language[25]);
   GetDlgItem(IDC_CHK_OPTION       )->SetWindowText(m_Language[41]);
   GetDlgItem(IDC_CHK_SCREENSHOT   )->SetWindowText(m_Language[156]);
   GetDlgItem(IDC_STATIC_MAP       )->SetWindowText(m_Language[101]);
   GetDlgItem(IDC_STATIC_COMBAT    )->SetWindowText(m_Language[157]);
   GetDlgItem(IDC_CHK_PROFESSION   )->SetWindowText(m_Language[370]);
   GetDlgItem(IDC_CHK_TAKE_ITEM_ID )->SetWindowText(m_Language[393]);

   GetDlgItem(IDC_STR_TITLE                  )->SetWindowText(m_Language[162]);
   GetDlgItem(IDC_BTN_ADD                    )->SetWindowText(m_Language[163]);
   GetDlgItem(IDC_BTN_MOD                    )->SetWindowText(m_Language[164]);
   GetDlgItem(IDC_BTN_DEL                    )->SetWindowText(m_Language[165]);
   GetDlgItem(IDC_BTN_RESTAURE_DEFAULT_COLOR )->SetWindowText(m_Language[166]);
}


void CT4CConfigDlg::OnChkWindowed() 
{
   UpdateData(TRUE);	
   m_sConfig.bWindowed = m_bWindowed;
   m_bConfigChange = TRUE;
}


void CT4CConfigDlg::OnBnClickedChkUsemmx()
{
   UpdateData(TRUE);	
   m_sConfig.bUSEMMX = m_bUseMMX;
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnBnClickedChkVbox()
{
   UpdateData(TRUE);	
   m_sConfig.bUSEVBOX = m_bUseVBox;
   m_bConfigChange = TRUE;
}


void CT4CConfigDlg::OnChkConnectMusic() 
{
   UpdateData(TRUE);	
   m_sConfig.bDisConMusic = m_bDisableMusicConnect;
   m_bConfigChange = TRUE;
   
}

 
void CT4CConfigDlg::OnChkResol640() 
{
	UpdateData(TRUE);
   m_bRes640       = TRUE;
   m_bRes800       = FALSE;
   m_bRes1024      = FALSE;
   m_bRes1280      = FALSE;
   m_bRes1280x720  = FALSE;
   m_bRes1280x800  = FALSE;
   m_sConfig.dwResolution = 0;
   UpdateData(FALSE);
   m_bConfigChange = TRUE;
}



void CT4CConfigDlg::OnChkResol800() 
{
	UpdateData(TRUE);
   m_bRes640       = FALSE;
   m_bRes800       = TRUE;
   m_bRes1024      = FALSE;
   m_bRes1280      = FALSE;
   m_bRes1280x720  = FALSE;
   m_bRes1280x800  = FALSE;
   m_sConfig.dwResolution = 1;
   UpdateData(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkResol1024() 
{
	UpdateData(TRUE);
   m_bRes640       = FALSE;
   m_bRes800       = FALSE;
   m_bRes1024      = TRUE;
   m_bRes1280      = FALSE;
   m_bRes1280x720  = FALSE;
   m_bRes1280x800  = FALSE;
   m_sConfig.dwResolution = 2;
   UpdateData(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkResol1280() 
{
   UpdateData(TRUE);
   m_bRes640       = FALSE;
   m_bRes800       = FALSE;
   m_bRes1024      = FALSE;
   m_bRes1280      = TRUE;
   m_bRes1280x720  = FALSE;
   m_bRes1280x800  = FALSE;
   m_sConfig.dwResolution = 3;
   UpdateData(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkResol1280x720() 
{
   UpdateData(TRUE);
   m_bRes640       = FALSE;
   m_bRes800       = FALSE;
   m_bRes1024      = FALSE;
   m_bRes1280      = FALSE;
   m_bRes1280x720  = TRUE;
   m_bRes1280x800  = FALSE;
   m_sConfig.dwResolution = 4;
   UpdateData(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkResol1280x800() 
{
   UpdateData(TRUE);
   m_bRes640       = FALSE;
   m_bRes800       = FALSE;
   m_bRes1024      = FALSE;
   m_bRes1280      = FALSE;
   m_bRes1280x720  = FALSE;
   m_bRes1280x800  = TRUE;
   m_sConfig.dwResolution = 5;
   UpdateData(FALSE);
   m_bConfigChange = TRUE;
   
}






void CT4CConfigDlg::OnChkInventory() 
{
	UpdateData(TRUE);
   if(m_bInventory)
      GetDlgItem(IDC_EDIT_INVENTORY)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_INVENTORY)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkCharacterui() 
{
	UpdateData(TRUE);
   if(m_bCharacter)
      GetDlgItem(IDC_EDIT_CHARACTERUI)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_CHARACTERUI)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkChatterui() 
{
	UpdateData(TRUE);
   if(m_bChatter)
      GetDlgItem(IDC_EDIT_CHATTER)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_CHATTER)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkGroupui() 
{
	UpdateData(TRUE);
   if(m_bGroup)
      GetDlgItem(IDC_EDIT_GROUPUI)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_GROUPUI)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnBnClickedChkGuildid()
{
   UpdateData(TRUE);
   if(m_bGroup)
      GetDlgItem(IDC_EDIT_GUILDID)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_GUILDID)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkSpellui() 
{
	UpdateData(TRUE);
   if(m_bSpell)
      GetDlgItem(IDC_EDIT_SPELLUI)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_SPELLUI)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkMacroui() 
{
	UpdateData(TRUE);
   if(m_bMacro)
      GetDlgItem(IDC_EDIT_MACROUI)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_MACROUI)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkTrade() 
{
	UpdateData(TRUE);
   if(m_bTrade)
      GetDlgItem(IDC_EDIT_TRADE)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_TRADE)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnChkOption() 
{
	UpdateData(TRUE);
   if(m_bOption)
      GetDlgItem(IDC_EDIT_OPTION)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_OPTION)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}


void CT4CConfigDlg::OnBnClickedChkProfession()
{
   UpdateData(TRUE);
   if(m_bProfession)
      GetDlgItem(IDC_EDIT_PROFESSION)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_PROFESSION)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}


void CT4CConfigDlg::OnChkScreenshot() 
{
	UpdateData(TRUE);
   if(m_bScreenShot)
      GetDlgItem(IDC_EDIT_SCREENSHOT)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_SCREENSHOT)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnBnClickedChkTakeItemId()
{
   UpdateData(TRUE);
   if(m_bTakeItemID)
      GetDlgItem(IDC_EDIT_TAKE_ITEM_ID)->EnableWindow(TRUE);
   else
      GetDlgItem(IDC_EDIT_TAKE_ITEM_ID)->EnableWindow(FALSE);
   m_bConfigChange = TRUE;
}


void  CT4CConfigDlg::DrawListColor()
{
   m_ListColor.DeleteAllItems();
   for(int i=0;i<m_sConfig.dwNbrColor;i++)
   {
      CString strTmp;
      strTmp.Format("0x%06X",m_sConfig.CCColor[i]);
      m_ListColor.InsertItem(i,strTmp,0);
   }
}

void CT4CConfigDlg::OnBtnAdd() 
{
	int dwColor = m_crSelected;
   m_sConfig.CCColor.Add(dwColor);
   m_sConfig.dwNbrColor++;
   DrawListColor();
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnBtnMod() 
{
	POSITION pos = m_ListColor.GetFirstSelectedItemPosition();
   if (pos)
   {
      int nItem = m_ListColor.GetNextSelectedItem(pos);
      if(nItem == 0)
      {
         MessageBox("You cant change or delete COLOR 0...","Error !",MB_OK+MB_ICONSTOP);
         return;
      }
      int dwColor = m_crSelected;
      m_sConfig.CCColor[nItem] = dwColor;
   }
	DrawListColor();
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnBtnDel() 
{
	POSITION pos = m_ListColor.GetFirstSelectedItemPosition();
   if (pos)
   {
      int nItem = m_ListColor.GetNextSelectedItem(pos);
      if(nItem == 0)
      {
         MessageBox("You cant change or delete COLOR 0...","Error !",MB_OK+MB_ICONSTOP);
         return;
      }
      m_sConfig.CCColor.RemoveAt(nItem);
      m_sConfig.dwNbrColor--;
   }
   DrawListColor();
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnBtnRestaureDefaultColor() 
{
	m_sConfig.CCColor.RemoveAll();
   m_sConfig.CCColor.Add(RGB(17,181,205));
   m_sConfig.CCColor.Add(RGB(255,33,75));	
   m_sConfig.CCColor.Add(RGB(90,119,216));
   m_sConfig.CCColor.Add(RGB(220,202,6));
   m_sConfig.CCColor.Add(RGB(5,197,37));
   m_sConfig.CCColor.Add(RGB(199,10,204));
   m_sConfig.CCColor.Add(RGB(192,192,192));
   m_sConfig.CCColor.Add(RGB(224,100,10));
   m_sConfig.dwNbrColor = 8;
   DrawListColor();
   m_bConfigChange = TRUE;
}

void CT4CConfigDlg::OnBnClickedBtnColor()
{
   m_crSelected = m_btnColor.GetColor();
}

void CT4CConfigDlg::OnBtnSave() 
{
	UpdateData(TRUE);
   m_sConfig.bMacro[0] = m_bInventory ;
   m_sConfig.bMacro[1] = m_bCharacter ;
   m_sConfig.bMacro[2] = m_bChatter   ;
   m_sConfig.bMacro[3] = m_bGroup     ;
   m_sConfig.bMacro[4] = m_bSpell     ;
   m_sConfig.bMacro[5] = m_bMacro     ;
   m_sConfig.bMacro[6] = m_bTrade     ;
   m_sConfig.bMacro[7] = m_bOption    ;
   m_sConfig.bMacro[8] = m_bScreenShot;
   m_sConfig.bMacro[9] = m_bGuild;
   m_sConfig.bMacro[10]= m_bProfession;

   m_sConfig.chMacroKey[0] = m_chInventory[0]; //Inventory
   m_sConfig.chMacroKey[1] = m_chCharacter[0]; //Charecter
   m_sConfig.chMacroKey[2] = m_chChatter[0];   //Chatter
   m_sConfig.chMacroKey[3] = m_chGroup[0];     //GroupPlay
   m_sConfig.chMacroKey[4] = m_chSpell[0];     //Spell
   m_sConfig.chMacroKey[5] = m_chMacro[0];     //Macro
   m_sConfig.chMacroKey[6] = m_chTrade[0];     //Trade
   m_sConfig.chMacroKey[7] = m_chOption[0];    //Option
   m_sConfig.chMacroKey[8] = m_chScreenShot[0];//Screenshot
   m_sConfig.chMacroKey[9] = m_chGuild[0];     //Guild
   m_sConfig.chMacroKey[10]= m_chProfession[0];//Profession
   m_sConfig.chMacroKey[11]= m_chTakeItemID[0];//Take item ID

   WriteConfig();
   m_bConfigChange = FALSE;
   CDialog::OnCancel();
}

void CT4CConfigDlg::OnChangeEditMacro() 
{
   m_bConfigChange = TRUE;
}
void CT4CConfigDlg::OnEnChangeEditTakeItemId()
{
   m_bConfigChange = TRUE;
}


void CT4CConfigDlg::OnBtnMp3Browse() 
{
   UpdateData(TRUE);
   CFileDialog wplSelectDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,"Window Media PLAYList Files (*.wpl)|*.wpl||");
   if(wplSelectDlg.DoModal() == IDOK)
   {
      m_strMp3PlayList = wplSelectDlg.GetPathName();
   }

   UpdateData(FALSE);
}










