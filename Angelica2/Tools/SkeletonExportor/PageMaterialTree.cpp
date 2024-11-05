// PageMaterialTree.cpp : implementation file
//

#include "Global.h"
#include "SkinCandidate.h"
#include "PageMaterialTree.h"
#include "MAXMesh.h"

/////////////////////////////////////////////////////////////////////////////
// CPageMaterialTree property page

IMPLEMENT_DYNCREATE(CPageMaterialTree, CPropertyPage)

CPageMaterialTree::CPageMaterialTree() : CPropertyPage(CPageMaterialTree::IDD)
{
	//{{AFX_DATA_INIT(CPageMaterialTree)
	m_strStep = _T("");
	//}}AFX_DATA_INIT

	m_iStepIndex = 0;
	m_iStepTotal = 0;

	m_pSkinCandidate = NULL;
}

CPageMaterialTree::~CPageMaterialTree()
{
}

void CPageMaterialTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMaterialTree)
	DDX_Control(pDX, IDC_TREE_MATERIAL, m_MtlTree);
	DDX_Text(pDX, IDC_STATIC_STEP, m_strStep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageMaterialTree, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMaterialTree)
	ON_BN_CLICKED(IDC_BTN_SELECTALL, OnBtnSelectall)
	ON_BN_CLICKED(IDC_BTN_UNSELECTALL, OnBtnUnselectall)
	ON_NOTIFY(NM_CLICK, IDC_TREE_MATERIAL, OnClickTreeMaterial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMaterialTree message handlers

void CPageMaterialTree::SetStep(int iIndex, int iTotal)
{
	m_iStepIndex = iIndex;
	m_iStepTotal = iTotal;

	m_strStep.Format("Step %d of %d", iIndex, iTotal);
}

BOOL CPageMaterialTree::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPageMaterialTree::OnSetActive() 
{
	//	Get property sheet
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();

	//	Set wizard buttons and text
	DWORD dwButtons = PSWIZB_NEXT;

	if (m_iStepIndex == m_iStepTotal)
		dwButtons |= PSWIZB_FINISH;

	if (m_iStepIndex > 1)
		dwButtons |= PSWIZB_BACK;

	pSheet->SetWizardButtons(dwButtons);

	// initialize page data
	if (!BeginPage())
		return FALSE;
	
	return CPropertyPage::OnSetActive();
}

bool CPageMaterialTree::BeginPage()
{
	//	Initialize hierarchy material control
	m_MtlTree.DeleteAllItems();

	int iNumMesh = m_pSkinCandidate->GetMAXMeshNum();

	for (int i=0; i < iNumMesh; i++)
	{
		CMAXMesh* pMesh = m_pSkinCandidate->GetMAXMesh(i);
		InsertMesh(pMesh, TVI_ROOT);
	}

	return true;
}

//	Insert a mesh
bool CPageMaterialTree::InsertMesh(CMAXMesh* pMesh, HTREEITEM hParentItem)
{
	char szText[512];
	sprintf(szText, "Node: %s", pMesh->GetINode()->GetName());

	HTREEITEM hMeshItem = m_MtlTree.InsertItem(TVIF_TEXT | TVIF_PARAM | TVIF_STATE,
		szText, 0, 0, TVIS_EXPANDED | INDEXTOSTATEIMAGEMASK(2),
		TVIS_EXPANDED | TVIS_STATEIMAGEMASK, (DWORD)pMesh, hParentItem, TVI_LAST);

	if (pMesh->UseDefaultMaterial())
	{
		InsertMaterial(pMesh, NULL, hMeshItem);
	}
	else
	{
		//	Insert all materials of this mesh
		for (int i=0; i < pMesh->GetMaterialCount(); i++)
		{
			CMAXMesh::MATERIAL* pMtl = pMesh->GetMaterial(i);
			if (pMtl->iParent == -1)
				InsertMaterial(pMesh, pMtl, hMeshItem);
		}
	}

	return true;
}

//	Insert a material
//	iIndex: material index, -1 means this is default material
bool CPageMaterialTree::InsertMaterial(CMAXMesh* pMesh, void* pMtl, HTREEITEM hParentItem)
{
	if (!pMtl)
	{
		m_MtlTree.InsertItem(TVIF_TEXT | TVIF_PARAM | TVIF_STATE,
			"Default A3D material", 0, 0, 
			TVIS_EXPANDED | INDEXTOSTATEIMAGEMASK(pMesh->IsDefMaterialSelected() ? 2 : 1),
			TVIS_EXPANDED | TVIS_STATEIMAGEMASK, NULL, hParentItem, TVI_LAST);
	}
	else
	{
		CMAXMesh::MATERIAL* pMaterial = (CMAXMesh::MATERIAL*)pMtl;

		HTREEITEM hItem = m_MtlTree.InsertItem(TVIF_TEXT | TVIF_PARAM | TVIF_STATE,
			pMaterial->pStdMtl->GetFullName(), 0, 0, 
			TVIS_EXPANDED | INDEXTOSTATEIMAGEMASK(pMaterial->bSelected ? 2 : 1),
			TVIS_EXPANDED | TVIS_STATEIMAGEMASK, (DWORD)pMaterial, hParentItem, TVI_LAST);

		//	Handle all sub-materials
		for (int i=0; i < pMesh->GetMaterialCount(); i++)
		{
			int iParent = pMesh->GetMaterialParent(i);
			if (iParent < 0 || pMesh->GetMaterial(i) != pMaterial)
				continue;

			InsertMaterial(pMesh, pMesh->GetMaterial(i), hItem);
		}
	}

	return true;
}

bool CPageMaterialTree::EndPage()
{
	//	Handle all materials
	HTREEITEM hMeshItem = m_MtlTree.GetChildItem(TVI_ROOT);
	while (hMeshItem)
	{
		CMAXMesh* pMesh = (CMAXMesh*)m_MtlTree.GetItemData(hMeshItem);

		HTREEITEM hMtlItem = m_MtlTree.GetChildItem(hMeshItem);
		while (hMtlItem)
		{
			UpdateMaterial(pMesh, hMtlItem);
			hMtlItem = m_MtlTree.GetNextSiblingItem(hMtlItem);
		}
		
		hMeshItem = m_MtlTree.GetNextSiblingItem(hMeshItem);
	}

	return true;
}

//	Update selection states of material and all it's children
void CPageMaterialTree::UpdateMaterial(CMAXMesh* pMesh, HTREEITEM hMtlItem)
{
	CMAXMesh::MATERIAL* pMtl = (CMAXMesh::MATERIAL*)m_MtlTree.GetItemData(hMtlItem);
	if (!pMtl)
	{
		if (pMesh->UseDefaultMaterial())
			pMesh->SelectDefMaterial(m_MtlTree.GetCheck(hMtlItem) ? true : false);

		return;
	}
	else
	{
		pMtl->bSelected = m_MtlTree.GetCheck(hMtlItem) ? true : false;

		//	Handle all children
		HTREEITEM hChild = m_MtlTree.GetChildItem(hMtlItem);
		while (hChild)
		{
			UpdateMaterial(pMesh, hChild);
			hChild = m_MtlTree.GetNextSiblingItem(hChild);
		}
	}
}

void CPageMaterialTree::OnBtnSelectall() 
{
	//	Handle all materials
	HTREEITEM hChild = m_MtlTree.GetChildItem(TVI_ROOT);
	while (hChild)
	{
		SelectMaterial(hChild, SEL_SELECT, true);
		hChild = m_MtlTree.GetNextSiblingItem(hChild);
	}
}

void CPageMaterialTree::OnBtnUnselectall() 
{
	//	Handle all materials
	HTREEITEM hChild = m_MtlTree.GetChildItem(TVI_ROOT);
	while (hChild)
	{
		SelectMaterial(hChild, SEL_UNSELECT, true);
		hChild = m_MtlTree.GetNextSiblingItem(hChild);
	}
}

//	Set selection state of a item and all it's children
void CPageMaterialTree::SelectMaterial(HTREEITEM hMtlItem, int iSelFlag, bool bSelfSet)
{
	if (bSelfSet)
		SelectMaterial(hMtlItem, iSelFlag);

	//	Handle all child
	HTREEITEM hChild = m_MtlTree.GetChildItem(hMtlItem);
	while (hChild)
	{
		SelectMaterial(hChild, iSelFlag, true);
		hChild = m_MtlTree.GetNextSiblingItem(hChild);
	}
}

//	Set selection state of a material item
void CPageMaterialTree::SelectMaterial(HTREEITEM hMtlItem, int iSelFlag)
{
	switch (iSelFlag)
	{
	case SEL_SELECT:	m_MtlTree.SetCheck(hMtlItem, TRUE);		break;
	case SEL_UNSELECT:	m_MtlTree.SetCheck(hMtlItem, FALSE);	break;
	case SEL_INVERSE:	m_MtlTree.SetCheck(hMtlItem, !m_MtlTree.GetCheck(hMtlItem));	break;
	}
}

void CPageMaterialTree::OnClickTreeMaterial(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	POINT ptScreen, pt;
	HTREEITEM hHitItem;
	UINT uFlags;

	//	Check whether cursor hit on a item
	//	Get cursor's position in tree control
	GetCursorPos(&ptScreen);
	pt = ptScreen;
	m_MtlTree.ScreenToClient(&pt);

	if (!(hHitItem = m_MtlTree.HitTest(pt, &uFlags)))
		return;

	if (uFlags & TVHT_ONITEMSTATEICON)
	{
		//	Note: this click action itself will case this item's check state
		//		change, so we only need to change the check states of it's children
		int iSelFlag = m_MtlTree.GetCheck(hHitItem) ? SEL_UNSELECT : SEL_SELECT;
		SelectMaterial(hHitItem, iSelFlag, false);
	}
}

BOOL CPageMaterialTree::OnWizardFinish() 
{
	//	Cleanup page data
	if (!EndPage())
		return -1;
	
	return CPropertyPage::OnWizardFinish();
}

LRESULT CPageMaterialTree::OnWizardNext() 
{
	//	Cleanup page data
	if (!EndPage())
		return -1;
	
	return CPropertyPage::OnWizardNext();
}


