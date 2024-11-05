// DlgTerrainLOD.cpp : implementation file
//

#include "Global.h"
#include "Terrain2View.h"
#include "DlgTerrainLOD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTerrainLOD dialog


CDlgTerrainLOD::CDlgTerrainLOD(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTerrainLOD::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTerrainLOD)
	m_fDist1 = 0.0f;
	m_fDist2 = 0.0f;
	m_iLODType = 1;
	//}}AFX_DATA_INIT
}


void CDlgTerrainLOD::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTerrainLOD)
	DDX_Text(pDX, IDC_EDIT_DIST1, m_fDist1);
	DDV_MinMaxFloat(pDX, m_fDist1, 0.0, 999999.0);
	DDX_Text(pDX, IDC_EDIT_DIST2, m_fDist2);
	DDV_MinMaxFloat(pDX, m_fDist2, 0.0, 999999.0);
	DDX_Radio(pDX, IDC_RADIO1, m_iLODType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTerrainLOD, CDialog)
	//{{AFX_MSG_MAP(CDlgTerrainLOD)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTerrainLOD message handlers

BOOL CDlgTerrainLOD::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTerrainLOD::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	
	CDialog::OnOK();
}


