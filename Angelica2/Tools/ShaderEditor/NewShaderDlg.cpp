// NewShaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shadereditor.h"
#include "NewShaderDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNewShaderDlg dialog


CNewShaderDlg::CNewShaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewShaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewShaderDlg)
	m_strFile = _T("");
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CNewShaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewShaderDlg)
	DDX_Text(pDX, IDC_FILE, m_strFile);
	DDV_MaxChars(pDX, m_strFile, 255);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 63);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewShaderDlg, CDialog)
	//{{AFX_MSG_MAP(CNewShaderDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewShaderDlg message handlers

BOOL CNewShaderDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewShaderDlg::OnOK() 
{
	UpdateData(TRUE);
	
	if (m_strName == "")
	{
		MessageBox("请输入 Shader 名字");
		return;
	}

	if (m_strFile == "")
	{
		MessageBox("请选择保存 Shader 的文件");
		return;
	}

	CDialog::OnOK();
}

//	Select shader file
void CNewShaderDlg::OnBrowse() 
{
	char szFilter[] = "Shader 文件(*.sdr)|*.sdr|所有文件(*.*)|*.*||";
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	CFileDialog	FileDia(FALSE, "*.sdr", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDOK)
	{
		m_strFile = FileDia.GetPathName();

		CDataExchange dx(this, FALSE);
		DDX_Text(&dx, IDC_FILE, m_strFile);
	}
}
