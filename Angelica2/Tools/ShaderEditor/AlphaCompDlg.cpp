// AlphaCompDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "AlphaCompDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CAlphaCompDlg dialog


CAlphaCompDlg::CAlphaCompDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlphaCompDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlphaCompDlg)
	m_iRefValue = 0;
	//}}AFX_DATA_INIT

	m_iFunc = 0;
}


void CAlphaCompDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlphaCompDlg)
	DDX_Control(pDX, IDC_COMPCOMBO, m_CompFunc);
	DDX_Text(pDX, IDC_REFVALUE, m_iRefValue);
	DDV_MinMaxInt(pDX, m_iRefValue, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlphaCompDlg, CDialog)
	//{{AFX_MSG_MAP(CAlphaCompDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
// 
//	CAlphaCompDlg message handlers
//
///////////////////////////////////////////////////////////////////////////

//	Initialize
void CAlphaCompDlg::Init(int iFunc, int iRefValue)
{
	m_iFunc		= iFunc;
	m_iRefValue	= iRefValue;
}

BOOL CAlphaCompDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	Initialize compare function list
	int i;
	const char** aStrs = &g_aCompStrs[1];

	for (i=0; i < NUM_COMPSTR - 1; i++)
		m_CompFunc.InsertString(-1, aStrs[i]);

	m_CompFunc.SetCurSel(m_iFunc);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAlphaCompDlg::OnOK() 
{
	UpdateData(TRUE);
	
	m_iFunc = m_CompFunc.GetCurSel();

	CDialog::OnOK();
}



