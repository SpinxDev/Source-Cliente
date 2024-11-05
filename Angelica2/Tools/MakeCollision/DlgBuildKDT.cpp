// DlgBuildKDT.cpp : implementation file
//

#include "Global.h"
#include "DlgBuildKDT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildKDT dialog


CDlgBuildKDT::CDlgBuildKDT(s_CONFIGS* pConfigs, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBuildKDT::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBuildKDT)
	m_fMinLeafSize = 0.0f;
	m_iMinSideNum = 0;
	//}}AFX_DATA_INIT

	m_pConfigs = pConfigs;
}


void CDlgBuildKDT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBuildKDT)
	DDX_Text(pDX, IDC_EDIT_MINLEAFSIZE, m_fMinLeafSize);
	DDX_Text(pDX, IDC_EDIT_MINSIDENUM, m_iMinSideNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBuildKDT, CDialog)
	//{{AFX_MSG_MAP(CDlgBuildKDT)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildKDT message handlers

BOOL CDlgBuildKDT::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT(m_pConfigs);

	m_fMinLeafSize	= m_pConfigs->fKDTMinLeafSize;
	m_iMinSideNum	= m_pConfigs->iKDTMinSideNum;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBuildKDT::OnOK() 
{
	UpdateData(TRUE);
	
	if (m_fMinLeafSize <= 0.0f)
	{
		MessageBox("切分节点的最小尺寸 必须大于 0.0");
		return;
	}

	if (m_iMinSideNum < 0)
		m_iMinSideNum = 0;

	m_pConfigs->fKDTMinLeafSize	= m_fMinLeafSize;
	m_pConfigs->iKDTMinSideNum	= m_iMinSideNum;

	CDialog::OnOK();
}


