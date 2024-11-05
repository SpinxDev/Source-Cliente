// DlgComboVal.cpp : implementation file
//

#include "CommonDef.h"
#include "shadereditor.h"
#include "DlgComboVal.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgComboVal dialog


CDlgComboVal::CDlgComboVal(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgComboVal::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgComboVal)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_iCurSel = -1;
}


void CDlgComboVal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgComboVal)
	DDX_Control(pDX, IDC_STATIC_DESC, m_DescBtn);
	DDX_Control(pDX, IDC_COMBO_VALUE, m_ValCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgComboVal, CDialog)
	//{{AFX_MSG_MAP(CDlgComboVal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgComboVal message handlers

BOOL CDlgComboVal::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for (int i=0; i < m_aCandidates.GetSize(); i++)
		m_ValCombo.InsertString(-1, *m_aCandidates[i]);

	if (m_iCurSel >= 0 && m_iCurSel < m_aCandidates.GetSize())
		m_ValCombo.SetCurSel(m_iCurSel);
	else
		m_ValCombo.SetCurSel(0);

	m_DescBtn.SetWindowText(m_strDesc);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgComboVal::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	
	m_iCurSel = m_ValCombo.GetCurSel();

	CDialog::OnOK();
}

int CDlgComboVal::AddCandidate(const char* szVal)
{
	CString* pstr = new CString(szVal);
	return m_aCandidates.Add(pstr);
}

