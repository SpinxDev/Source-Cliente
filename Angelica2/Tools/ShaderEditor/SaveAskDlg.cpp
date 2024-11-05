// SaveAskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shadereditor.h"
#include "SaveAskDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CSaveAskDlg dialog


CSaveAskDlg::CSaveAskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveAskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveAskDlg)
	m_strPrompt = _T("");
	//}}AFX_DATA_INIT

	m_szFileName[0] = '\0';
}


void CSaveAskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveAskDlg)
	DDX_Text(pDX, IDC_PROMPT, m_strPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveAskDlg, CDialog)
	//{{AFX_MSG_MAP(CSaveAskDlg)
	ON_BN_CLICKED(IDC_NOTTOALL, OnNotoall)
	ON_BN_CLICKED(IDC_YES, OnYes)
	ON_BN_CLICKED(IDC_YESTOALL, OnYestoall)
	ON_BN_CLICKED(IDC_NOT, OnNot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveAskDlg message handlers

BOOL CSaveAskDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_strPrompt = "";

	if (m_szFileName[0])
	{
		m_strPrompt  = m_szFileName;
		m_strPrompt += " Ã»ÓÐ±£´æ¡£ ";
	}

	CString strText;
	strText.LoadString(IDS_SAVEPROMPT);

	m_strPrompt += strText;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Initialize data
bool CSaveAskDlg::Init(const char* szFile)
{
	if (szFile)
		strcpy(m_szFileName, szFile);
	else
		m_szFileName[0] = '\0';

	return true;
}

void CSaveAskDlg::OnNot() 
{
	EndDialog(IDC_NOT);
}

void CSaveAskDlg::OnNotoall() 
{
	EndDialog(IDC_NOTTOALL);
}

void CSaveAskDlg::OnYes() 
{
	EndDialog(IDC_YES);
}

void CSaveAskDlg::OnYestoall() 
{
	EndDialog(IDC_YESTOALL);
}


