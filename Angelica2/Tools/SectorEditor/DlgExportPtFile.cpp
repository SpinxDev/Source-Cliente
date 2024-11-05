// DlgExportPtFile.cpp : implementation file
//

#include "Global.h"
#include "DlgExportPtFile.h"
#include "Utility.h"
#include "MainFrm.h"
#include "SectorEditorDoc.h"

#include "AF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportPtFile dialog


CDlgExportPtFile::CDlgExportPtFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportPtFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportPtFile)
	m_iDivX = 0;
	m_iDivY = 0;
	m_iDivZ = 0;
	m_fMaxLimit = 0.0f;
	m_fMinLimit = 0.0f;
	m_strPortalFile = _T("");
	//}}AFX_DATA_INIT
}


void CDlgExportPtFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportPtFile)
	DDX_Text(pDX, IDC_EDIT_DIVX, m_iDivX);
	DDV_MinMaxInt(pDX, m_iDivX, 0, 100);
	DDX_Text(pDX, IDC_EDIT_DIVY, m_iDivY);
	DDV_MinMaxInt(pDX, m_iDivY, 0, 100);
	DDX_Text(pDX, IDC_EDIT_DIVZ, m_iDivZ);
	DDV_MinMaxInt(pDX, m_iDivZ, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MAXLIMIT, m_fMaxLimit);
	DDX_Text(pDX, IDC_EDIT_MINLIMIT, m_fMinLimit);
	DDX_Text(pDX, IDC_EDIT_PORTALFILE, m_strPortalFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportPtFile, CDialog)
	//{{AFX_MSG_MAP(CDlgExportPtFile)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportPtFile message handlers

BOOL CDlgExportPtFile::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExportPtFile::OnOK() 
{
	UpdateData(TRUE);

	if (m_fMinLimit < 0.0f || m_fMaxLimit < 0.0f)
	{
		MessageBox("区域的最大和最小值不能为负.");
		return;
	}

	if (m_fMaxLimit < m_fMinLimit)
	{
		MessageBox("区域的最大值不能小于和最小值.");
		return;
	}

	if (m_strPortalFile == "")
	{
		MessageBox("请输入要导出的 Portal 文件的文件名.");
		return;
	}

	CDialog::OnOK();
}

void CDlgExportPtFile::OnBtnBrowse() 
{
	char szFilter[] = "Portal 文件(*.prt)|*.prt|所有文件(*.*)|*.*||";
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	CFileDialog	FileDia(FALSE, "*.prt", NULL, dwFlags, szFilter, NULL);

	//	Use scene file's path as default path
	char szDir[MAX_PATH];
	af_GetFilePath(AUX_GetMainFrame()->GetDocument()->GetSceneFile(), szDir, MAX_PATH);
	FileDia.m_ofn.lpstrInitialDir = szDir;

	if (FileDia.DoModal() != IDOK)
		return;

	m_strPortalFile = FileDia.GetPathName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_PORTALFILE, m_strPortalFile);
}


