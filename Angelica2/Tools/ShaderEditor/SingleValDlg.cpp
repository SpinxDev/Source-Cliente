// SingleValDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shadereditor.h"
#include "SingleValDlg.h"

///////////////////////////////////////////////////////////////////////////
// 
//	CSingleValDlg dialog
//
///////////////////////////////////////////////////////////////////////////

CSingleValDlg::CSingleValDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSingleValDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSingleValDlg)
	m_strPrompt = _T("");
	//}}AFX_DATA_INIT

	m_iType		= VAL_FLOAT;
	m_bHex		= false;
	m_iLimit	= 256;

	m_fValue	 = 0;
	m_dwValue	 = 0;
	m_szValue[0] = '\0';
}


void CSingleValDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSingleValDlg)
	DDX_Control(pDX, IDC_VALUE, m_ValueEdit);
	DDX_Text(pDX, IDC_PROMPT, m_strPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSingleValDlg, CDialog)
	//{{AFX_MSG_MAP(CSingleValDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
//
//	CSingleValDlg message handlers
//
///////////////////////////////////////////////////////////////////////////

bool CSingleValDlg::Init(const char* szTitle, float fValue)
{
	m_iType		= VAL_FLOAT;
	m_fValue	= fValue;

	strcpy(m_szTitle, szTitle);

	return true;
}

bool CSingleValDlg::Init(const char* szTitle, DWORD dwValue, bool bInt, bool bHex)
{
	if (bInt)
		m_iType = VAL_INT;
	else
		m_iType	= VAL_DWORD;

	m_dwValue = dwValue;
	m_bHex	  = bHex;
	
	strcpy(m_szTitle, szTitle);

	return true;
}

bool CSingleValDlg::Init(const char* szTitle, const char* szValue, int iLimit)
{
	m_iType	 = VAL_STRING;
	m_iLimit = iLimit;

	strcpy(m_szValue, szValue);

	strcpy(m_szTitle, szTitle);

	return true;
}

BOOL CSingleValDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_iType == VAL_FLOAT)
	{
		sprintf(m_szValue, "%f", m_fValue);
		m_strPrompt = "请输入浮点数：";
	}
	else if (m_iType == VAL_DWORD || m_iType == VAL_INT)
	{
		if (m_bHex)
		{
			sprintf(m_szValue, "%x", m_dwValue);
			m_strPrompt = "请输入 16 进制整数：";
		}
		else if (m_iType == VAL_DWORD)
		{
			sprintf(m_szValue, "%u", m_dwValue);
			m_strPrompt = "请输入 10 进制无符号整数：";
		}
		else
		{
			sprintf(m_szValue, "%d", (int)m_dwValue);
			m_strPrompt = "请输入 10 进制整数：";
		}
	}
	else
		m_strPrompt = "请输入字符串：";

	m_ValueEdit.SetLimitText(m_iLimit);
	m_ValueEdit.SetWindowText(m_szValue);

	//	Set dialog's title
	SetWindowText(m_szTitle);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSingleValDlg::OnOK() 
{
	UpdateData(TRUE);

	m_ValueEdit.GetWindowText(m_szValue, 255);

	if (m_szValue[0] == '\0')
	{
		MessageBox("无效的值。");
		return;
	}

	if (m_iType == VAL_FLOAT)
		m_fValue = (float)atof(m_szValue);
	else if (m_iType == VAL_DWORD || m_iType == VAL_INT)
	{
		if (m_bHex)
			sscanf(m_szValue, "%x", &m_dwValue);
		else if (m_iType == VAL_DWORD)
			sscanf(m_szValue, "%u", &m_dwValue);
		else
			sscanf(m_szValue, "%d", &m_dwValue);
	}

	CDialog::OnOK();
}



