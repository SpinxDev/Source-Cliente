// DlgTerrainInfo.cpp : implementation file
//

#include "Global.h"
#include "DlgTerrainInfo.h"
#include "A3DTerrain2Wrapper.h"

#include <A3DTerrain2.h>
#include <A3DTerrain2Loader.h>

/////////////////////////////////////////////////////////////////////////////
// CDlgTerrainInfo dialog


CDlgTerrainInfo::CDlgTerrainInfo(A3DTerrain2Wrapper* pTerrain, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTerrainInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTerrainInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pTerrain = pTerrain;
}


void CDlgTerrainInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTerrainInfo)
	DDX_Control(pDX, IDC_LIST_TRNINFO, m_InfoList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTerrainInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgTerrainInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTerrainInfo message handlers

BOOL CDlgTerrainInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_InfoList.InsertColumn(0, "", LVCFMT_LEFT, 180);
	m_InfoList.InsertColumn(1, "", LVCFMT_LEFT, 220);
	m_InfoList.SetExtendedStyle(m_InfoList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	if (!m_pTerrain)
		return TRUE;

	int iItem, iCount=0;
	char szText[100];

	//	Loader type
	iItem = m_InfoList.InsertItem(iCount++, "Loader type");
	if (m_pTerrain->GetTerrainLoader()->GetLoaderID() == A3DTerrain2Loader::LOADER_A)
		m_InfoList.SetItemText(iItem, 1, "LOADER_A");
	else
		m_InfoList.SetItemText(iItem, 1, "LOADER_B");

	//	sub-terrain number
	iItem = m_InfoList.InsertItem(iCount++, "Sub-terrain number");
	if (m_pTerrain->GetTerrainLoader()->GetLoaderID() == A3DTerrain2Loader::LOADER_A)
		m_InfoList.SetItemText(iItem, 1, "1");
	else if (m_pTerrain->GetTerrainLoader()->GetLoaderID() == A3DTerrain2Loader::LOADER_B)
	{
		int r = m_pTerrain->GetBlockRowNum() * m_pTerrain->GetBlockGrid() / m_pTerrain->GetSubTerrainGrid();
		int c = m_pTerrain->GetBlockColNum() * m_pTerrain->GetBlockGrid() / m_pTerrain->GetSubTerrainGrid();
		sprintf(szText, "C * R = %d * %d = %d", c, r, r * c);
		m_InfoList.SetItemText(iItem, 1, szText);
	}

	//	terrain size
	iItem = m_InfoList.InsertItem(iCount++, "Terrain size");
	const ARectF& rc = m_pTerrain->GetTerrainArea();
	sprintf(szText, "W * L = %.2f * %.2f", rc.Width(), -rc.Height());
	m_InfoList.SetItemText(iItem, 1, szText);

	//	grid size
	iItem = m_InfoList.InsertItem(iCount++, "Grid size");
	sprintf(szText, "%.2f", m_pTerrain->GetGridSize());
	m_InfoList.SetItemText(iItem, 1, szText);

	//	block grid
	iItem = m_InfoList.InsertItem(iCount++, "Grid of each block");
	sprintf(szText, "%d", m_pTerrain->GetBlockGrid());
	m_InfoList.SetItemText(iItem, 1, szText);

	//	block size
	iItem = m_InfoList.InsertItem(iCount++, "Block size");
	sprintf(szText, "%.2f", m_pTerrain->GetBlockGrid() * m_pTerrain->GetGridSize());
	m_InfoList.SetItemText(iItem, 1, szText);

	//	mask grid
	iItem = m_InfoList.InsertItem(iCount++, "Grid of each mask area");
	sprintf(szText, "%d", m_pTerrain->GetMaskGrid());
	m_InfoList.SetItemText(iItem, 1, szText);

	//	mask size
	iItem = m_InfoList.InsertItem(iCount++, "Mask area size");
	sprintf(szText, "%.2f", m_pTerrain->GetMaskGrid() * m_pTerrain->GetGridSize());
	m_InfoList.SetItemText(iItem, 1, szText);

	//	sub-terrain grid
	iItem = m_InfoList.InsertItem(iCount++, "Grid of each sub-terrain");
	sprintf(szText, "%d", m_pTerrain->GetSubTerrainGrid());
	m_InfoList.SetItemText(iItem, 1, szText);

	//	sub_terrain size
	iItem = m_InfoList.InsertItem(iCount++, "Sub-terrain size");
	sprintf(szText, "%.2f", m_pTerrain->GetSubTerrainGrid() * m_pTerrain->GetGridSize());
	m_InfoList.SetItemText(iItem, 1, szText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
