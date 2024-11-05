// MakeCollisionDlg.cpp : implementation file
//

#include "Global.h"
#include "MakeCollision.h"
#include "MakeCollisionDlg.h"

#include "DlgBuildESP.h"
#include "DlgBuildKDT.h"
#include "Side.h"
#include "ESP_Write.h"
#include "ESP_Leaf.h"
#include "KDT_Write.h"
#include "KDT_Main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMakeCollisionDlg dialog

CMakeCollisionDlg::CMakeCollisionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeCollisionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMakeCollisionDlg)
	m_strSceneFile = _T("");
	m_bCreateBrush = FALSE;
	m_bMergeSide = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iCurProgSeg	= 0;
	m_iProgTotal	= 100;
}

void CMakeCollisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMakeCollisionDlg)
	DDX_Control(pDX, IDC_EDIT_LASTMSG, m_LastMsgEdit);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_MsgEdit);
	DDX_Control(pDX, IDC_PROGRESS_MAKE, m_Progress);
	DDX_Text(pDX, IDC_EDIT_SCENEFILE, m_strSceneFile);
	DDX_Check(pDX, IDC_CHECK_CREATEBRUSH, m_bCreateBrush);
	DDX_Check(pDX, IDC_CHECK_MERGETRIANGLE, m_bMergeSide);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMakeCollisionDlg, CDialog)
	//{{AFX_MSG_MAP(CMakeCollisionDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_MAKEESP, OnBtnMakeesp)
	ON_BN_CLICKED(IDC_BTN_MAKEKDTREE, OnBtnMakekdtree)
	ON_BN_CLICKED(IDC_BTN_CLEARMSG, OnBtnClearmsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMakeCollisionDlg message handlers

BOOL CMakeCollisionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//	Initialize configs
	DefaultConfigs();

	m_bCreateBrush	= !g_Configs.bNoBrush;
	m_bMergeSide	= g_Configs.bMergeSide;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMakeCollisionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMakeCollisionDlg::OnPaint() 
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
HCURSOR CMakeCollisionDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//	Set configs to default value
void CMakeCollisionDlg::DefaultConfigs()
{
	g_Configs.bMergeSide		= true;
	g_Configs.bNoBrush			= true;
	g_Configs.iESPLeafSize		= 20;
	g_Configs.fBrushThick		= 0.1f;
	g_Configs.fKDTMinLeafSize	= 40.0f;
	g_Configs.iKDTMinSideNum	= 30;
}

void CMakeCollisionDlg::OnBtnBrowse() 
{
	//	Let user select a BSP file
	char szFilter[] = "mox 文件(*.mox)|*.mox|mod 文件(*.mod)|*.mod||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
	CFileDialog	FileDia(TRUE, "*.mox", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() != IDOK)
		return;

	m_strSceneFile = FileDia.GetPathName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_SCENEFILE, m_strSceneFile);
}

void CMakeCollisionDlg::OnBtnMakeesp() 
{
	if (!CheckParameters())
		return;

	CDlgBuildESP ESPDlg(&g_Configs);
	if (ESPDlg.DoModal() != IDOK)
		return;

	AddMessage("=================== Building ESP ===================\n");

	SetCursor(LoadCursor(NULL, IDC_WAIT));

	//	Reset progress bar
	PROGSEGMENT aSegs[NUM_ESPPROG] = {{0, 600}, {600, 950}, {950, 1000}};
	ResetProgress(1000, NUM_ESPPROG, aSegs);

	//	Load scene
	bool bRet;

	if (strstr(m_strSceneFile, ".mox") || strstr(m_strSceneFile, ".MOX"))
		bRet = LoadSidesFromMOX(m_strSceneFile);
	else
		bRet = LoadSidesFromMOD(m_strSceneFile);

	if (!bRet)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox("场景加载失败!");
		return;
	}

	if (!ESP_Build())
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox("ESP 处理失败!");
		return;
	}

	char szFile[MAX_PATH];
	strcpy(szFile, m_strSceneFile);
	StripFileTail(szFile);
	strcat(szFile, ".esp");

	if (!ESP_WriteFile(szFile))
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox("ESP 保存失败!");
		return;
	}

	//	Release resources
	ESP_Release();
	DestroySides();

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	MessageBox("ESP 处理完毕!");
}

void CMakeCollisionDlg::OnBtnMakekdtree() 
{
	if (!CheckParameters())
		return;

	CDlgBuildKDT KDTDlg(&g_Configs);
	if (KDTDlg.DoModal() != IDOK)
		return;

	AddMessage("=================== Building KDT ===================\n");

	SetCursor(LoadCursor(NULL, IDC_WAIT));

	//	Reset progress bar
	PROGSEGMENT aSegs[NUM_KDTPROG] = {{0, 800}, {800, 950}, {950, 1000}};
	ResetProgress(1000, NUM_KDTPROG, aSegs);

	//	Load scene
	bool bRet;

	if (strstr(m_strSceneFile, ".mox") || strstr(m_strSceneFile, ".MOX"))
		bRet = LoadSidesFromMOX(m_strSceneFile);
	else
		bRet = LoadSidesFromMOD(m_strSceneFile);

	if (!bRet)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox("场景加载失败!");
		return;
	}

	if (!KDT_Build())
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox("KDT 处理失败!");
		return;
	}

	char szFile[MAX_PATH];
	strcpy(szFile, m_strSceneFile);
	StripFileTail(szFile);
	strcat(szFile, ".kdt");

	if (!KDT_WriteFile(szFile))
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox("KDT 保存失败!");
		return;
	}

	//	Release resources
	KDT_Release();
	DestroySides();

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	MessageBox("KDT 处理完毕!");
}

//	Check parameters
bool CMakeCollisionDlg::CheckParameters()
{
	UpdateData(TRUE);

	if (m_strSceneFile == "")
	{
		MessageBox("请指定场景文件!");
		return false;
	}

	g_Configs.bNoBrush		= m_bCreateBrush ? false : true;
	g_Configs.bMergeSide	= m_bMergeSide ? true : false;

	return true;
}

void CMakeCollisionDlg::AddMessage(const char* szMsg)
{
	//	Select all text at first then to cancel selection. This is the way to
	//	ensure caret is at the end of text when new text is added !!
	m_MsgEdit.SetSel(0, -1);
	m_MsgEdit.SetSel(-1, -1);
	m_MsgEdit.ReplaceSel(szMsg);
}

//	Update last message
void CMakeCollisionDlg::UpdateLastMessage(const char* szMsg)
{
	m_LastMsgEdit.SetSel(0, -1);
	m_LastMsgEdit.ReplaceSel(szMsg);
}

void CMakeCollisionDlg::OnBtnClearmsg() 
{
	m_MsgEdit.SetSel(0, -1);
	m_MsgEdit.ReplaceSel("");

	m_LastMsgEdit.SetSel(0, -1);
	m_LastMsgEdit.ReplaceSel("");
}

//	Initialize progress segment
void CMakeCollisionDlg::ResetProgress(int iTotal, int iNumSeg, PROGSEGMENT* aSegs)
{
	for (int i=0; i < iNumSeg; i++)
		m_aProgSegs[i] = aSegs[i];

	m_iProgTotal	= iTotal;
	m_iCurProgSeg	= 0;

	m_Progress.SetRange(0, iTotal);
	m_Progress.SetPos(0);
}

//	Step progress bar
void CMakeCollisionDlg::StepProgressBar(int iPercent)
{
	int iLen = m_aProgSegs[m_iCurProgSeg].iEnd - m_aProgSegs[m_iCurProgSeg].iStart;
	int iPos = m_aProgSegs[m_iCurProgSeg].iStart + (int)((iLen * iPercent / 100.0f) + 0.5f);

	m_Progress.SetPos(iPos);
}

