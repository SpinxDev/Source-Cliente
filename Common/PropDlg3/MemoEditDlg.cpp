// MemoEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PropDlg_rc.h"
#include "MemoEditDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMemoEditDlg dialog


CMemoEditDlg::CMemoEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMemoEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMemoEditDlg)
	m_Memo = _T("");
	//}}AFX_DATA_INIT
}


void CMemoEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMemoEditDlg)
	DDX_Text(pDX, IDC_EDITMEMO, m_Memo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMemoEditDlg, CDialog)
	//{{AFX_MSG_MAP(CMemoEditDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemoEditDlg message handlers
