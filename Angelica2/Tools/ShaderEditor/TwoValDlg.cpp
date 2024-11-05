// TwoValDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shadereditor.h"
#include "TwoValDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTwoValDlg dialog


CTwoValDlg::CTwoValDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTwoValDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTwoValDlg)
	m_strValue1 = _T("");
	m_strValue2 = _T("");
	//}}AFX_DATA_INIT

	m_iType		= VAL_FLOAT;
	m_fVal1		= 0.0f;
	m_fVal2		= 0.0f;
	m_dwVal1	= 0;
	m_dwVal2	= 0;

	m_szTitle[0]	= '\0';	
	m_szPrompt1[0]	= '\0';
	m_szPrompt2[0]	= '\0';
}


void CTwoValDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTwoValDlg)
	DDX_Control(pDX, IDC_VALUEEDIT2, m_ValEdit2);
	DDX_Control(pDX, IDC_VALUEEDIT1, m_ValEdit1);
	DDX_Text(pDX, IDC_VALUE1, m_strValue1);
	DDX_Text(pDX, IDC_VALUE2, m_strValue2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTwoValDlg, CDialog)
	//{{AFX_MSG_MAP(CTwoValDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
//
//	CTwoValDlg message handlers
//
///////////////////////////////////////////////////////////////////////////

bool CTwoValDlg::Init(char* szTitle, char* szPrompt1, char* szPrompt2, float fVal1, float fVal2)
{
	strcpy(m_szTitle, szTitle);
	strcpy(m_szPrompt1, szPrompt1);
	strcpy(m_szPrompt2, szPrompt2);

	m_iType = VAL_FLOAT;
	m_fVal1 = fVal1;
	m_fVal2 = fVal2;

	return true;
}

bool CTwoValDlg::Init(char* szTitle, bool bInt, char* szPrompt1, char* szPrompt2, DWORD dwVal1, DWORD dwVal2)
{
	strcpy(m_szTitle, szTitle);
	strcpy(m_szPrompt1, szPrompt1);
	strcpy(m_szPrompt2, szPrompt2);

	m_iType	 = bInt ? VAL_INT : VAL_DWORD;
	m_dwVal1 = dwVal1;
	m_dwVal2 = dwVal2;

	return true;
}

BOOL CTwoValDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char szText[256];

	if (m_iType == VAL_FLOAT)
	{
		sprintf(szText, "%f", m_fVal1);
		m_ValEdit1.SetWindowText(szText);

		sprintf(szText, "%f", m_fVal2);
		m_ValEdit2.SetWindowText(szText);
	}
	else if (m_iType == VAL_INT)
	{
		sprintf(szText, "%d", (int)m_dwVal1);
		m_ValEdit1.SetWindowText(szText);

		sprintf(szText, "%d", (int)m_dwVal2);
		m_ValEdit2.SetWindowText(szText);
	}
	else	//	m_iType == VAL_DWORD
	{
		sprintf(szText, "%u", m_dwVal1);
		m_ValEdit1.SetWindowText(szText);

		sprintf(szText, "%u", m_dwVal2);
		m_ValEdit2.SetWindowText(szText);
	}

	m_strValue1 = m_szPrompt1;
	m_strValue2 = m_szPrompt2;

	//	Dialog's title
	SetWindowText(m_szTitle);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTwoValDlg::OnOK() 
{
	UpdateData(TRUE);

	char szText[256];

	if (m_iType == VAL_FLOAT)
	{
		m_ValEdit1.GetWindowText(szText, 255);
		m_fVal1 = (float)atof(szText);

		m_ValEdit2.GetWindowText(szText, 255);
		m_fVal2 = (float)atof(szText);
	}
	else if (m_iType == VAL_INT)
	{
		m_ValEdit1.GetWindowText(szText, 255);
		m_dwVal1 = atoi(szText);

		m_ValEdit2.GetWindowText(szText, 255);
		m_dwVal2 = atoi(szText);
	}
	else	//	m_iType == VAL_DWORD
	{
		m_ValEdit1.GetWindowText(szText, 255);
		m_dwVal1 = (DWORD)atoi(szText);

		m_ValEdit2.GetWindowText(szText, 255);
		m_dwVal2 = (DWORD)atoi(szText);
	}

	CDialog::OnOK();
}


