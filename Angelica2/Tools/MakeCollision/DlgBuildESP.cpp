// DlgBuildESP.cpp : implementation file
//

#include "Global.h"
#include "DlgBuildESP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildESP dialog


CDlgBuildESP::CDlgBuildESP(s_CONFIGS* pConfigs, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBuildESP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBuildESP)
	m_iLeafSize = 0;
	//}}AFX_DATA_INIT

	m_pConfigs = pConfigs;
}

void CDlgBuildESP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBuildESP)
	DDX_Text(pDX, IDC_EDIT_LEAFSIZE, m_iLeafSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBuildESP, CDialog)
	//{{AFX_MSG_MAP(CDlgBuildESP)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildESP message handlers

BOOL CDlgBuildESP::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pConfigs);

	m_iLeafSize	= m_pConfigs->iESPLeafSize;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBuildESP::OnOK() 
{
	UpdateData(TRUE);

	if (m_iLeafSize <= 0)
	{
		MessageBox("ESP叶子大小必须大于0");
		return;
	}

	m_pConfigs->iESPLeafSize = m_iLeafSize;

	CDialog::OnOK();
}


