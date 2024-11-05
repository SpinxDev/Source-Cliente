// PageSkeletonTree.cpp : implementation file
//

#include "Global.h"
#include "SkeletonExportor.h"
#include "PageSkeletonTree.h"
#include "SheetSkeleton.h"
#include "BoneCandidate.h"
#include "SkeletonCandidate.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CPageSkeletonTree property page

IMPLEMENT_DYNCREATE(CPageSkeletonTree, CPropertyPage)

CPageSkeletonTree::CPageSkeletonTree() : CPropertyPage(CPageSkeletonTree::IDD)
, m_bNoNub(FALSE)
{
	//{{AFX_DATA_INIT(CPageSkeletonTree)
	m_strStep = _T("");
	m_bSortBone = FALSE;
	//}}AFX_DATA_INIT

	m_iStepIndex = 0;
	m_iStepTotal = 0;
	m_bNoNub = TRUE;

	m_pSkeletonCandidate = NULL;
}

CPageSkeletonTree::~CPageSkeletonTree()
{
}

void CPageSkeletonTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSkeletonTree)
	DDX_Control(pDX, IDC_TREE_SKELETON, m_SkeletonTree);
	DDX_Text(pDX, IDC_STATIC_STEP, m_strStep);
	DDX_Check(pDX, IDC_CHECK_SORTBONE, m_bSortBone);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_NONUB, m_bNoNub);
}


BEGIN_MESSAGE_MAP(CPageSkeletonTree, CPropertyPage)
	//{{AFX_MSG_MAP(CPageSkeletonTree)
	ON_BN_CLICKED(IDC_BTN_SELECTALL, OnBtnSelectall)
	ON_BN_CLICKED(IDC_BTN_UNSELECTALL, OnBtnUnselectall)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_NONUB, &CPageSkeletonTree::OnBnClickedCheckNonub)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageSkeletonTree message handlers

void CPageSkeletonTree::SetStep(int iIndex, int iTotal)
{
	m_iStepIndex = iIndex;
	m_iStepTotal = iTotal;

	m_strStep.Format("Step %d of %d", iIndex, iTotal);
}

BOOL CPageSkeletonTree::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//	Initialize image list
	m_ImageList.Create(16, 16, ILC_COLOR | ILC_MASK, 5, 1);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_OTHER));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_BONE));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_DUMMY));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FAKEBONE));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_HOOK));
	m_SkeletonTree.SetImageList(&m_ImageList, TVSIL_NORMAL);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPageSkeletonTree::OnSetActive() 
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

bool CPageSkeletonTree::BeginPage()
{
	//	Initialize hierarchy tree control
	m_SkeletonTree.DeleteAllItems();

	if (m_pSkeletonCandidate)
	{
		//	Get root bone candidate list
		AList2<int, int>& RootBoneList = m_pSkeletonCandidate->GetRootBoneList();

		//	Loop through all root bone candidates
		ALISTPOSITION pos = RootBoneList.GetHeadPosition();
		while (pos)
		{
			//	Insert bone candidate into the tree control
			InsertBoneCandidate(true, RootBoneList.GetNext(pos), TVI_ROOT);
		}

		//	Loop through all root hook candidates
		for (int i=0; i < m_pSkeletonCandidate->GetHookNum(); i++)
		{
			CBoneCandidate* pBoneCandidate = m_pSkeletonCandidate->GetHook(i);
			if (pBoneCandidate->GetParentId() < 0)
				InsertBoneCandidate(false, i, TVI_ROOT);
		}
	}

	OnNoNubFlagChanged();

	return true;
}

void CPageSkeletonTree::InsertBoneCandidate(bool bBone, int iIndex, HTREEITEM hParentItem)
{
	CBoneCandidate* pBoneCandidate;
	
	if (bBone)
		pBoneCandidate = m_pSkeletonCandidate->GetBoneCandidate(iIndex);
	else
		pBoneCandidate = m_pSkeletonCandidate->GetHook(iIndex);

	//	Insert bone candidate into the tree control
	CMAXNode* pNode = pBoneCandidate->GetNode();

	HTREEITEM hItem = m_SkeletonTree.InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE,
		pNode->GetName(), pNode->GetType(), pNode->GetType(), 
	//	TVIS_EXPANDED | INDEXTOSTATEIMAGEMASK(pBoneCandidate->IsSelected() ? 2 : 1), TVIS_EXPANDED | TVIS_STATEIMAGEMASK,
		TVIS_EXPANDED, TVIS_EXPANDED,
		(DWORD)pBoneCandidate, hParentItem, TVI_LAST);

	m_SkeletonTree.SetCheck(hItem, pBoneCandidate->IsSelected());

	int i;

	//	Insert all children bone candidates into the tree control
	for (i=0; i < pBoneCandidate->GetChildNum(); i++)
		InsertBoneCandidate(true, pBoneCandidate->GetChildID(i), hItem);
	
	//	Insert all hooks into the tree control
	for (i=0; i < pBoneCandidate->GetHookNum(); i++)
		InsertBoneCandidate(false, pBoneCandidate->GetHookID(i), hItem);
}

bool CPageSkeletonTree::EndPage()
{
	UpdateData(TRUE);

	//	Get bone candidate vector
	APtrArray<CBoneCandidate*>& aBones = m_pSkeletonCandidate->GetBoneArray();

	//	Write back the select state of all bone candidates
	HTREEITEM hItem = m_SkeletonTree.GetRootItem();
	if (hItem)
	{
		//	Create a stack
		AArray<HTREEITEM, HTREEITEM&> aItems(aBones.GetSize() * 2, 10);

		//	Insert all root items into the stack
		while (hItem)
		{
			aItems.Add(hItem);
			hItem = m_SkeletonTree.GetNextSiblingItem(hItem);
		}

		//	Loop until we handled all items in the stack
		while (aItems.GetSize())
		{
			//	Get next item
			hItem = aItems[aItems.GetSize() - 1];
			aItems.RemoveTail();

			//	Get bone candidate id
			CBoneCandidate* pBone = (CBoneCandidate*)m_SkeletonTree.GetItemData(hItem);

			//	Set new select state of the bone candidate
		//	pBone->SetSelected((m_SkeletonTree.GetItemState(hItem, TVIS_STATEIMAGEMASK) & INDEXTOSTATEIMAGEMASK(2)) != 0);
			pBone->SetSelected(m_SkeletonTree.GetCheck(hItem) ? true : false);

			//	Fill all children of the current item into the stack
			if (m_SkeletonTree.ItemHasChildren(hItem))
			{
				//	Get the first child of the current item
				hItem = m_SkeletonTree.GetChildItem(hItem);

				// insert all child items into the stack
				while (hItem)
				{
					aItems.Add(hItem);
					hItem = m_SkeletonTree.GetNextSiblingItem(hItem);
				}
			}
		}
	}

	//	Now, name collision has been checked in CSkeletonCandidate::AddNode,
	//	so below checking code can be removed. It still exists just because checking
	//	another time do no-damage to the code
	//	Check bone name collision
	if (m_pSkeletonCandidate->CheckBoneNameCollision())
	{
		AUX_OutputMessage("选中的骨头中存在名字冲突!");
		return false;
	}

	if (m_bSortBone)
		m_pSkeletonCandidate->SortBones();

	//	Build selected bones' id
	m_pSkeletonCandidate->BuildSelectedId();

	return true;
}

LRESULT CPageSkeletonTree::OnWizardNext() 
{
	//	Cleanup page data
	if (!EndPage())
		return -1;

	return CPropertyPage::OnWizardNext();
}

BOOL CPageSkeletonTree::OnWizardFinish() 
{
	//	Cleanup page data
	if (!EndPage())
		return FALSE;

	return CPropertyPage::OnWizardFinish();
}

void CPageSkeletonTree::OnBtnSelectall() 
{
	//	Handle all bones
	HTREEITEM hChild = m_SkeletonTree.GetChildItem(TVI_ROOT);
	while (hChild)
	{
		SelectBones(hChild, SEL_SELECT);
		hChild = m_SkeletonTree.GetNextSiblingItem(hChild);
	}

	//	If No-nub is selected, unselect it because user want to
	//	select all bones
	if (m_bNoNub)
	{
		m_bNoNub = false;
		CDataExchange dx(this, FALSE);
		DDX_Check(&dx, IDC_CHECK_NONUB, m_bNoNub);
	}
}

void CPageSkeletonTree::OnBtnUnselectall() 
{
	//	Handle all bones
	HTREEITEM hChild = m_SkeletonTree.GetChildItem(TVI_ROOT);
	while (hChild)
	{
		SelectBones(hChild, SEL_UNSELECT);
		hChild = m_SkeletonTree.GetNextSiblingItem(hChild);
	}
}

void CPageSkeletonTree::SelectBones(HTREEITEM hBoneItem, int iSelFlag)
{
	SelectBone(hBoneItem, iSelFlag);

	//	Handle all child
	HTREEITEM hChild = m_SkeletonTree.GetChildItem(hBoneItem);
	while (hChild)
	{
		SelectBones(hChild, iSelFlag);
		hChild = m_SkeletonTree.GetNextSiblingItem(hChild);
	}
}

void CPageSkeletonTree::SelectBone(HTREEITEM hBoneItem, int iSelFlag)
{
	switch (iSelFlag)
	{
	case SEL_SELECT:	m_SkeletonTree.SetCheck(hBoneItem, TRUE);	break;
	case SEL_UNSELECT:	m_SkeletonTree.SetCheck(hBoneItem, FALSE);	break;
	}
}

void CPageSkeletonTree::OnBnClickedCheckNonub()
{
	m_bNoNub = !m_bNoNub;
	OnNoNubFlagChanged();
}

void CPageSkeletonTree::OnNoNubFlagChanged()
{
	AString strName;
	int iNumBone = m_pSkeletonCandidate->GetBoneArray().GetSize();

	//	Write back the select state of all bone candidates
	HTREEITEM hItem = m_SkeletonTree.GetRootItem();
	if (hItem)
	{
		//	Create a stack
		AArray<HTREEITEM, HTREEITEM&> aItems(iNumBone * 2, 10);

		//	Insert all root items into the stack
		while (hItem)
		{
			aItems.Add(hItem);
			hItem = m_SkeletonTree.GetNextSiblingItem(hItem);
		}

		//	Loop until we handled all items in the stack
		while (aItems.GetSize())
		{
			//	Get next item
			hItem = aItems[aItems.GetSize() - 1];
			aItems.RemoveTail();

			if (!m_SkeletonTree.ItemHasChildren(hItem))
			{
				//	Get bone name
				CBoneCandidate* pBone = (CBoneCandidate*)m_SkeletonTree.GetItemData(hItem);
				strName = pBone->GetName();

				int iLen = strName.GetLength();
				if (iLen >= 3 && 
					strName[iLen-3] == 'N' &&
					strName[iLen-2] == 'u' &&
					strName[iLen-1] == 'b')
				{
					m_SkeletonTree.SetCheck(hItem, !m_bNoNub);
				}
			}
			else	//	Fill all children of the current item into the stack
			{
				//	Get the first child of the current item
				hItem = m_SkeletonTree.GetChildItem(hItem);

				// insert all child items into the stack
				while (hItem)
				{
					aItems.Add(hItem);
					hItem = m_SkeletonTree.GetNextSiblingItem(hItem);
				}
			}
		}
	}
}

