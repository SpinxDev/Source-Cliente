// DlgPortalSector.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgPortalSector.h"
#include "SectorEditorDoc.h"
#include "Sector.h"
#include "Portal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPortalSector dialog


CDlgPortalSector::CDlgPortalSector(CSectorEditorDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPortalSector::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPortalSector)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDoc = pDoc;
	m_pSelObject = NULL;
}


void CDlgPortalSector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPortalSector)
	DDX_Control(pDX, IDC_LIST_SECTORS, m_SectorList);
	DDX_Control(pDX, IDC_LIST_PORTALS, m_PortalList);
	DDX_Control(pDX, IDC_STATIC_SECTORS, m_strSectors);
	DDX_Control(pDX, IDC_STATIC_PORTALS, m_strPortals);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPortalSector, CDialog)
	//{{AFX_MSG_MAP(CDlgPortalSector)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBtnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PORTALS, OnItemchangedListPortals)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SECTORS, OnItemchangedListSectors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPortalSector message handlers

BOOL CDlgPortalSector::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT(m_pDoc);

	//	Initialize sector list
	InitSectorList();

	//	Initialize portal list
	InitPortalList();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Initialize portal list
void CDlgPortalSector::InitPortalList()
{
	LVCOLUMN Column;
	Column.mask		= LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	Column.fmt		= LVCFMT_LEFT;
	Column.cx		= 40;
	Column.pszText	= "索引";
	m_PortalList.InsertColumn(0, &Column);

	Column.cx		= 80;
	Column.pszText	= "位置";
	m_PortalList.InsertColumn(1, &Column);

	Column.cx		= 80;
	Column.pszText	= "s1";
	m_PortalList.InsertColumn(2, &Column);

	Column.cx		= 80;
	Column.pszText	= "s2";
	m_PortalList.InsertColumn(3, &Column);

	//	Full row select enable
	m_PortalList.SetExtendedStyle(m_PortalList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	UpdatePortalList(true);
}

//	Initialize sector list
void CDlgPortalSector::InitSectorList()
{
	LVCOLUMN Column;
	Column.mask		= LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	Column.fmt		= LVCFMT_LEFT;
	Column.cx		= 40;
	Column.pszText	= "索引";
	m_SectorList.InsertColumn(0, &Column);

	Column.cx		= 180;
	Column.pszText	= "mins (x, y, z)";
	m_SectorList.InsertColumn(1, &Column);

	Column.cx		= 180;
	Column.pszText	= "maxs (x, y, z)";
	m_SectorList.InsertColumn(2, &Column);

	Column.cx		= 500;
	Column.pszText	= "Portals";
	m_SectorList.InsertColumn(3, &Column);

	//	Full row select enable
	m_SectorList.SetExtendedStyle(m_SectorList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	UpdateSectorList(true);
}

//	Get associated portals of specified sector
void CDlgPortalSector::GetLinkedPortals(CSector* pSector, char* szText)
{
	int iPortalCnt=0;
	bool bFirst=true, bLinked=false;
	CPortal* pPortal = m_pDoc->GetFirstPortal();

	while (pPortal)
	{
		int m1, m2;
		CSector *s1, *s2;
		
		s1 = pPortal->GetLinkedSector(0, &m1);
		s2 = pPortal->GetLinkedSector(1, &m2);

		if ((s1 == pSector && (m1 == CPortal::LINK_FROM || m1 == CPortal::LINK_BIDIR)) ||
			(s2 == pSector && (m2 == CPortal::LINK_FROM || m2 == CPortal::LINK_BIDIR)))
		{
			if (bFirst)
			{
				bFirst = false;
				sprintf(szText, "%d", m_pDoc->GetPortalIndex(pPortal));
			}
			else
			{
				char szStr[100];
				sprintf(szStr, ", %d", m_pDoc->GetPortalIndex(pPortal));
				strcat(szText, szStr);
			}

			bLinked = true;
		}

		iPortalCnt++;
		pPortal = m_pDoc->GetNextPortal();
	}

	if (!bLinked)
		strcpy(szText, "null");
}

//	Update portal list
void CDlgPortalSector::UpdatePortalList(bool bInit)
{
	char szText[128];
	char* szAxis[3] = {"X", "Y", "Z"};
	int iItem, iCount = 0;

	m_PortalList.DeleteAllItems();

	CEditObject* pSelObject = NULL;
	if (bInit)
		pSelObject = m_pDoc->GetSelectedEditObject();

	CPortal* pPortal = m_pDoc->GetFirstPortal();
	while (pPortal)
	{
		iItem = m_PortalList.InsertItem(iCount, "New item");

		//	Index
		itoa(iCount, szText, 10);
		m_PortalList.SetItemText(iItem, 0, szText);

		//	Position
		A3DVECTOR3 aVerts[4];
		pPortal->GetVertices(aVerts);
		sprintf(szText, "%s %.2f", szAxis[pPortal->GetMajorAxis()], aVerts[0].m[pPortal->GetMajorAxis()]);
		m_PortalList.SetItemText(iItem, 1, szText);

		//	s1
		CSector* s = pPortal->GetLinkedSector(0, NULL);
		if (s)
			sprintf(szText, "%d", m_pDoc->GetSectorIndex(s));
		else
			strcpy(szText, "null");

		m_PortalList.SetItemText(iItem, 2, szText);

		//	s2
		s = pPortal->GetLinkedSector(1, NULL);
		if (s)
			sprintf(szText, "%d", m_pDoc->GetSectorIndex(s));
		else
			strcpy(szText, "null");

		m_PortalList.SetItemText(iItem, 3, szText);

		//	Highlight selected portal
		if (pPortal == pSelObject)
			m_PortalList.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);

		iCount++;
		pPortal = m_pDoc->GetNextPortal();
	}

	sprintf(szText, "Portals (%d)", iCount);
	m_strPortals.SetWindowText(szText);
}

//	Update sector list
void CDlgPortalSector::UpdateSectorList(bool bInit)
{
	char szText[128];
	int iItem, iCount = 0;

	m_SectorList.DeleteAllItems();

	CEditObject* pSelObject = NULL;
	if (bInit)
		pSelObject = m_pDoc->GetSelectedEditObject();

	CSector* pSector = m_pDoc->GetFirstSector();
	while (pSector)
	{
		iItem = m_SectorList.InsertItem(iCount, "New item");

		//	Index
		itoa(iCount, szText, 10);
		m_SectorList.SetItemText(iItem, 0, szText);

		//	mins
		A3DVECTOR3 v = pSector->GetMins();
		sprintf(szText, "%.2f, %.2f, %.2f", v.x, v.y, v.z);
		m_SectorList.SetItemText(iItem, 1, szText);

		//	maxs
		v = pSector->GetMaxs();
		sprintf(szText, "%.2f, %.2f, %.2f", v.x, v.y, v.z);
		m_SectorList.SetItemText(iItem, 2, szText);

		//	Linked portals
		GetLinkedPortals(pSector, szText);
		m_SectorList.SetItemText(iItem, 3, szText);

		//	Highlight selected sector
		if (pSector == pSelObject)
			m_SectorList.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);

		iCount++;
		pSector = m_pDoc->GetNextSector();
	}

	sprintf(szText, "Sectors (%d)", iCount);
	m_strSectors.SetWindowText(szText);
}

void CDlgPortalSector::OnOK() 
{
	UpdateData(TRUE);
	
	m_pSelObject = GetSelectedObject(NULL, NULL);

	CDialog::OnOK();
}

void CDlgPortalSector::OnBtnDelete() 
{
	int iID, iIndex;
	CEditObject* pObject = GetSelectedObject(&iID, &iIndex);

	if (!pObject)
		return;

	if (MessageBox("确定要删除该物体吗?", "提示", MB_YESNO) == IDNO)
		return;

	if (iID == EOID_SECTOR)
	{
		CSector* pSector = (CSector*)pObject;
		m_pDoc->RemoveSector(pSector, true);
		pSector->Release();
		delete pSector;

		m_SectorList.DeleteItem(iIndex);
	}
	else if (iID == EOID_PORTAL)
	{
		CPortal* pPortal = (CPortal*)pObject;
		m_pDoc->RemovePortal(pPortal, true);
		pPortal->Release();
		delete pPortal;

		m_PortalList.DeleteItem(iIndex);
	}

	UpdatePortalList(false);
	UpdateSectorList(false);
}

//	Get selected object
CEditObject* CDlgPortalSector::GetSelectedObject(int* piID, int* piIndex)
{
	CEditObject* pObject = NULL;

	if (m_PortalList.GetSelectedCount())
	{
		POSITION pos = m_PortalList.GetFirstSelectedItemPosition();
		int iItem = m_PortalList.GetNextSelectedItem(pos);
		pObject = m_pDoc->GetPortalByIndex(iItem);

		if (piID)
			*piID = EOID_PORTAL;

		if (piIndex)
			*piIndex = iItem;
	}
	else if (m_SectorList.GetSelectedCount())
	{
		POSITION pos = m_SectorList.GetFirstSelectedItemPosition();
		int iItem = m_SectorList.GetNextSelectedItem(pos);
		pObject = m_pDoc->GetSectorByIndex(iItem);

		if (piID)
			*piID = EOID_SECTOR;

		if (piIndex)
			*piIndex = iItem;
	}
	else if (piID)
		*piID = EOID_UNKNOWN;

	return pObject;
}

void CDlgPortalSector::OnItemchangedListPortals(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem < 0)
		return;

	if (!(pNMListView->uOldState & LVIS_SELECTED) && 
		(pNMListView->uNewState & LVIS_SELECTED))
	{
		//	One portal is selected
		RemoveSelection(false);
	}
}

void CDlgPortalSector::OnItemchangedListSectors(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem < 0)
		return;

	if (!(pNMListView->uOldState & LVIS_SELECTED) && 
		(pNMListView->uNewState & LVIS_SELECTED))
	{
		//	One sector is selected
		RemoveSelection(true);
	}
}

//	Remove selection in list
void CDlgPortalSector::RemoveSelection(bool bPortalList)
{
	if (bPortalList)
	{
		POSITION pos = m_PortalList.GetFirstSelectedItemPosition();
		int iItem = m_PortalList.GetNextSelectedItem(pos);
		m_PortalList.SetItemState(iItem, ~LVIS_SELECTED, LVIS_SELECTED);
	}
	else
	{
		POSITION pos = m_SectorList.GetFirstSelectedItemPosition();
		int iItem = m_SectorList.GetNextSelectedItem(pos);
		m_SectorList.SetItemState(iItem, ~LVIS_SELECTED, LVIS_SELECTED);
	}
}



