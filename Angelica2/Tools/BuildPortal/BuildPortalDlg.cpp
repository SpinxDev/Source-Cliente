// BuildPortalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BuildPortal.h"
#include "BuildPortalDlg.h"

#include "AC.h"
#include "AF.h"
#include "A3D.h"
#include "PT_Interface.h"

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
// CBuildPortalDlg dialog

CBuildPortalDlg::CBuildPortalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBuildPortalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBuildPortalDlg)
	m_strDescFile = _T("");
	m_strMoxFile = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBuildPortalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBuildPortalDlg)
	DDX_Text(pDX, IDC_EDIT_DESCFILE, m_strDescFile);
	DDX_Text(pDX, IDC_EDIT_MOXFILE, m_strMoxFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBuildPortalDlg, CDialog)
	//{{AFX_MSG_MAP(CBuildPortalDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CREATE, OnBtnCreate)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_BROWSE1, OnBtnBrowse1)
	ON_BN_CLICKED(IDC_BTN_BROWSE2, OnBtnBrowse2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuildPortalDlg message handlers

BOOL CBuildPortalDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	if (!PT_Init())
	{
		EndDialog(0);
		return FALSE;
	}
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBuildPortalDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	PT_Release();
}

void CBuildPortalDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBuildPortalDlg::OnPaint() 
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
HCURSOR CBuildPortalDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CBuildPortalDlg::OnBtnBrowse1() 
{
	//	Let user select a mox file
	char szFilter[] = "场景模型文件(*.ils)|*.ils||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
	CFileDialog	FileDia(TRUE, "*.ils", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;

	m_strMoxFile = FileDia.GetPathName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_MOXFILE, m_strMoxFile);
}

void CBuildPortalDlg::OnBtnBrowse2() 
{
	//	Let user select a description file
	char szFilter[] = "描述文件(*.txt)|*.txt||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
	CFileDialog	FileDia(TRUE, "*.txt", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;

	m_strDescFile = FileDia.GetPathName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_DESCFILE, m_strDescFile);
}

void CBuildPortalDlg::OnBtnCreate() 
{
	UpdateData(TRUE);

	if (m_strMoxFile == "" || m_strDescFile == "")
	{
		MessageBox("无效的文件名!");
		return;
	}

	PT_Reset();

	if (!LoadSceneFile((LPCTSTR)m_strMoxFile))
	{
		MessageBox("加载场景失败!");
		return;
	}

	if (!LoadDescFile((LPCTSTR)m_strDescFile))
	{
		MessageBox("加载描述文件失败!");
		return;
	}

	if (!PT_Build())
	{
		MessageBox("PT_Build 失败!");
		return;
	}

	//	Save portal file
	CString strFile = m_strMoxFile;
	strFile.Replace(".ils", ".prt");

	PT_Save((LPCTSTR)strFile);
}

//	Load scene file
bool CBuildPortalDlg::LoadSceneFile(const char* szFile)
{
	A3DIBLScene Scene;
	if (!Scene.Init(NULL))
		return false;

	if (!Scene.Import((char*)szFile))
		return false;

	int iNumSurf = Scene.GetFaceCount();
	int iSize = iNumSurf * sizeof (A3DVECTOR3) * 3;
	A3DVECTOR3* aVertPos;

	if (!(aVertPos = (A3DVECTOR3*)a_malloc(iSize)))
		return false;

	if (!Scene.GetAllFaceVerts(aVertPos, &iSize))
		return false;

	if (!PT_AddRenderSurfs(aVertPos, iNumSurf))
		return false;

	a_free(aVertPos);
	Scene.Release();
	
	return true;
}

bool _ParseSector(AScriptFile* pFile)
{
	float x1, x2, z1, z2, y1, y2;

	x1 = pFile->GetNextTokenAsFloat(false);
	x2 = pFile->GetNextTokenAsFloat(false);
	z1 = pFile->GetNextTokenAsFloat(false);
	z2 = pFile->GetNextTokenAsFloat(false);
	y1 = -1.0f;
	y2 = 4.0f;

	WORD aIndices[] = 
	{
		0, 1, 2, 0, 2, 3, 
		1, 5, 6, 1, 6, 2,
		2, 6, 7, 2, 7, 3,
		3, 7, 4, 3, 4, 0,
		4, 5, 1, 4, 1, 0,
		5, 7, 6, 5, 4, 7
	};

	A3DVECTOR3 aVerts[8];
	aVerts[0].Set(x1, y2, z2);
	aVerts[1].Set(x2, y2, z2);
	aVerts[2].Set(x2, y2, z1);
	aVerts[3].Set(x1, y2, z1);
	aVerts[4].Set(x1, y1, z2);
	aVerts[5].Set(x2, y1, z2);
	aVerts[6].Set(x2, y1, z1);
	aVerts[7].Set(x1, y1, z1);

	PT_AddSector(aVerts, aIndices, 12);

	return true;
}

bool _ParsePortal(AScriptFile* pFile)
{
	int iFront, iBack, iType;
	float x1, x2, z1, z2, y1, y2;

	x1 = pFile->GetNextTokenAsFloat(false);
	x2 = pFile->GetNextTokenAsFloat(false);
	z1 = pFile->GetNextTokenAsFloat(false);
	z2 = pFile->GetNextTokenAsFloat(false);
	y1 = pFile->GetNextTokenAsFloat(false);
	y2 = pFile->GetNextTokenAsFloat(false);

	iFront	= pFile->GetNextTokenAsInt(false);
	iBack	= pFile->GetNextTokenAsInt(false);
	iType	= pFile->GetNextTokenAsInt(false);

	A3DVECTOR3 aVerts[4];
	aVerts[0].Set(x1, y1, z1);
	aVerts[1].Set(x1, y2, z1);
	aVerts[2].Set(x2, y2, z2);
	aVerts[3].Set(x2, y1, z2);

	PT_AddPortal(aVerts, 4, iFront, iBack, iType);

	return true;
}

//	Load sector and portal description file
bool CBuildPortalDlg::LoadDescFile(const char* szFile)
{
	AFile File;
	AScriptFile Script;

	if (!File.Open((char*)szFile, AFILE_BINARY | AFILE_OPENEXIST))
		return false;

	if (!Script.Open(&File))
		return false;
	
	File.Close();
	
	int iNumSector=0, iNumPortal=0;

	while (Script.GetNextToken(true))
	{
		if (!stricmp(Script.m_szToken, "sector"))
		{
			_ParseSector(&Script);
			iNumSector++;
		}
		else if (!stricmp(Script.m_szToken, "portal"))
		{
			_ParsePortal(&Script);
			iNumPortal++;
		}

		Script.SkipLine();
	}

	Script.Close();

	return true;
}


