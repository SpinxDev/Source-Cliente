// DlgInputFlot.cpp : implementation file
//

#include "stdafx.h"
#include <float.h>
#include "Terrain2View.h"
#include "DlgInputFlot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputFlot dialog

CDlgInputFlot::CDlgInputFlot(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputFlot::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputFlot)
	m_fValue = 0.0f;
	//}}AFX_DATA_INIT
	
	m_fMin	= -FLT_MAX;
	m_fMax	= FLT_MAX;
}

void CDlgInputFlot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputFlot)
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_PromptBtn);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_fValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputFlot, CDialog)
	//{{AFX_MSG_MAP(CDlgInputFlot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputFlot message handlers

BOOL CDlgInputFlot::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_PromptBtn.SetWindowText(m_strPrompt);
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInputFlot::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	
	if (m_fValue < m_fMin || m_fValue > m_fMax)
	{
		char szMsg[256];
		sprintf(szMsg, "输入的值必须在 [%f, %f] 范围内", m_fMin, m_fMax);
		MessageBox(szMsg);
		return;
	}

	CDialog::OnOK();
}
