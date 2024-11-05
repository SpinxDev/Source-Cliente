// DlgPosition.cpp : implementation file
//

#include "stdafx.h"
#include "Terrain2View.h"
#include "DlgPosition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPosition dialog


CDlgPosition::CDlgPosition(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPosition::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPosition)
	m_fx = 0.0f;
	m_fy = 0.0f;
	m_fz = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgPosition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPosition)
	DDX_Text(pDX, IDC_EDIT_X, m_fx);
	DDX_Text(pDX, IDC_EDIT_Y, m_fy);
	DDX_Text(pDX, IDC_EDIT_Z, m_fz);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPosition, CDialog)
	//{{AFX_MSG_MAP(CDlgPosition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPosition message handlers

BOOL CDlgPosition::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPosition::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	
	CDialog::OnOK();
}
