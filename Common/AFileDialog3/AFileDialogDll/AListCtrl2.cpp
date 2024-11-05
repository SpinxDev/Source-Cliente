/*
* FILE: AListCtrl2.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/18
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "AListCtrl2.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define IDU_THUMBNAIL		40001
#define IDU_ICON			40002
#define IDU_LIST			40003
#define IDU_DETAILS			40004

#define IDU_NAME			40006
#define IDU_SIZE			40007
#define IDU_TYPE			40008
#define IDU_TIME			40009

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AListItem
//	
///////////////////////////////////////////////////////////////////////////


AListItem::AListItem(void)
: m_pListCtrl(NULL)
, m_iItem(-1)
, m_iCommonImgIdx(-1)
, m_bVisible(true)
{
	m_iThumbImgIdx[0] = m_iThumbImgIdx[1] = -1;
}

AListItem::~AListItem(void)
{

}

void AListItem::SetItemText(int iIdx, const TCHAR* szSubItem) 
{
	while (m_aItemText.GetSize() <= iIdx)
		m_aItemText.Add(CString());

	m_aItemText[iIdx] = szSubItem;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AListCtrl2
//	
///////////////////////////////////////////////////////////////////////////

AListCtrl2::AListCtrl2(void)
: m_pListener(NULL)
, m_clKey(RGB(0, 0, 0))
, m_iThumbImgIdx(0)
, m_root(new Node)
{
	m_dwLastTick = GetTickCount();
	
}

AListCtrl2::~AListCtrl2(void)
{
	
	ClearItems();
	delete m_root;
}

void AListCtrl2::ClearItems()
{
	DeleteTree(m_root);

	m_IdxItem.clear();
	m_ItemIdx.clear();
	m_root = new Node;

	m_aItems.RemoveAll();
	
	for (int iCacheBitmapIdx = 0; iCacheBitmapIdx < m_aCacheBitmaps.GetSize(); ++iCacheBitmapIdx)
	{
		//m_aCacheBitmaps[iCacheBitmapIdx]->DeleteObject();
		delete m_aCacheBitmaps[iCacheBitmapIdx];
	}
	
	m_aCacheBitmaps.RemoveAll();

	if (GetSafeHwnd())
		CListCtrl::DeleteAllItems();
}

void AListCtrl2::DeleteTree(Node* root)
{
	if (root->Array.size()!=0)
	{
		abase::hash_map<TCHAR, Node*>::iterator map_id;
		map_id = root->Array.begin();
		
		while (map_id != root->Array.end())
		{
			Node *retp = root->Array[map_id->first];
			DeleteTree(retp);
			++map_id;
		}
	}
	delete root;

}

BOOL AListCtrl2::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= LVS_OWNERDATA;
	cs.style |= LVS_ICON;
	cs.dwExStyle |= LVS_EX_DOUBLEBUFFER;

	m_iLVType = LV_ICON;
	m_iSortType = SORT_NAME;

	return CListCtrl::PreCreateWindow(cs);
}

CBitmap* AListCtrl2::CreateCacheBitmap(HBITMAP hBitmap)
{
	CBitmap* pBitmap = CBitmap::FromHandle(hBitmap);
	m_aCacheBitmaps.Add(pBitmap);
	return pBitmap;
}

int AListCtrl2::AddCommonImage(CBitmap* pImage)
{
	ASSERT( m_SmallImages.GetImageCount() == m_IconImages.GetImageCount() );

	int iIconImgIdx = -1;
	{
		HBITMAP hIconBitmap = (HBITMAP)CopyImage(pImage->GetSafeHandle(), IMAGE_BITMAP, IMG_SIZE_ICON, IMG_SIZE_ICON, LR_CREATEDIBSECTION);
		iIconImgIdx = m_IconImages.Add(CBitmap::FromHandle(hIconBitmap), m_clKey);
	}

	int iSmallImgIdx = -1;
	{
		HBITMAP hSmallBitmap = (HBITMAP)CopyImage(pImage->GetSafeHandle(), IMAGE_BITMAP, IMG_SIZE_SMALL, IMG_SIZE_SMALL, LR_CREATEDIBSECTION);
		iSmallImgIdx = m_SmallImages.Add(CBitmap::FromHandle(hSmallBitmap), m_clKey);
	}

	ASSERT( iSmallImgIdx == iIconImgIdx );
	return iSmallImgIdx;
}

int AListCtrl2::AddThumbImage(CBitmap* pImage)
{
	a_LogOutput(1, "thumb image number %d", m_pThumbImages->GetImageCount());
	return m_pThumbImages->Add(pImage, m_clKey);
	//HBITMAP hThumbBitmap = (HBITMAP)CopyImage(pImage->GetSafeHandle(), IMAGE_BITMAP, IMG_SIZE_THUMB, IMG_SIZE_THUMB, LR_CREATEDIBSECTION);
	//return m_ThumbImages.Add(CreateCacheBitmap(hThumbBitmap), m_clKey);
}

void AListCtrl2::ReplaceThumbImage(int iIdx, CBitmap* pImage)
{
	m_pThumbImages->Replace(iIdx, pImage, &m_ThumbMaskMap);
	//HBITMAP hThumbBItmap = (HBITMAP)CopyImage(pImage->GetSafeHandle(), IMAGE_BITMAP, IMG_SIZE_THUMB, IMG_SIZE_THUMB, LR_CREATEDIBSECTION);
	//m_ThumbImages.Replace(iIdx, CreateCacheBitmap(hThumbBItmap), &m_ThumbMaskMap);
}

BEGIN_MESSAGE_MAP(AListCtrl2, CListCtrl)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(IDU_THUMBNAIL, IDU_DETAILS, OnChangeListStyle)
	ON_COMMAND_RANGE(IDU_NAME, IDU_TIME, OnSortList)
	//ON_WM_CHAR()
#if defined( _MSC_VER ) && _MSC_VER < 1310
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, AListCtrl2::OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, AListCtrl2::OnLvnColumnclick)
	ON_NOTIFY_REFLECT(LVN_ODFINDITEM, AListCtrl2::OnOdFindItem)
	ON_NOTIFY_REFLECT(NM_CLICK, AListCtrl2::OnNMClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, AListCtrl2::OnNMDblclk)
#else
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &AListCtrl2::OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &AListCtrl2::OnLvnColumnclick)
	ON_NOTIFY_REFLECT(LVN_ODFINDITEM, &AListCtrl2::OnOdFindItem)
	ON_NOTIFY_REFLECT(NM_CLICK, &AListCtrl2::OnNMClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &AListCtrl2::OnNMDblclk)
#endif

END_MESSAGE_MAP()

int AListCtrl2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	m_IconImages.Create(IMG_SIZE_ICON, IMG_SIZE_ICON, ILC_COLOR24, 5, 10);
	m_SmallImages.Create(IMG_SIZE_SMALL, IMG_SIZE_SMALL, ILC_COLOR24, 5, 10);

	SetImageList(&m_SmallImages, LVSIL_SMALL);
	SetImageList(&m_IconImages, LVSIL_NORMAL);

	static const TCHAR* szViewSubMenu[] = 
	{
		_T("缩略图(&H)"),
		_T("图标(&N)"),
		_T("列表(&L)"),
		_T("详细信息(&D)")
	};

	m_ViewMenu.CreatePopupMenu();
	int i;
	for (i = 0; i < sizeof(szViewSubMenu) / sizeof(szViewSubMenu[0]); ++i)
		m_ViewMenu.InsertMenu(i, MF_STRING, IDU_THUMBNAIL + i, szViewSubMenu[i]);
	
	static const TCHAR* szArraySubMenu[] = 
	{
		_T("名称(&N)"),
		_T("大小(&S)"),
		_T("类型(&T)"),
		_T("修改时间(&M)")
	};

	m_SortMenu.CreatePopupMenu();
	for (i = 0; i < sizeof(szArraySubMenu) / sizeof(szArraySubMenu[0]); ++i)
		m_SortMenu.InsertMenu(i, MF_STRING, IDU_NAME + i, szArraySubMenu[i]);

	m_ContextMenu.CreatePopupMenu();
	m_ContextMenu.AppendMenu( MF_POPUP | MF_STRING, (UINT_PTR)m_ViewMenu.m_hMenu, _T("查看(&V)"));
	m_ContextMenu.AppendMenu( MF_POPUP | MF_STRING, (UINT_PTR)m_SortMenu.m_hMenu, _T("排列图标(&I)"));


	InsertColumn(0, _T("文件名"), LVCFMT_LEFT, 200);
	InsertColumn(1, _T("大小"), LVCFMT_RIGHT, 120);
	InsertColumn(2, _T("类型"), LVCFMT_LEFT, 100);
	InsertColumn(3, _T("图片大小"), LVCFMT_RIGHT, 100);
	InsertColumn(4, _T("修改日期"), LVCFMT_LEFT, 165);

	return 0;
}

void AListCtrl2::SetLVType(int iLVType)
{
	DWORD dwStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	DWORD dwLstType = dwStyle & LVS_TYPEMASK;
	m_iLVType = iLVType;
	switch (iLVType)
	{
	case LV_THUMBVIEW:
		dwLstType = LVS_ICON;
		SetImageList(m_pThumbImages, LVSIL_NORMAL);
		break;
	case LV_ICON:
		dwLstType = LVS_ICON;
		SetImageList(&m_IconImages, LVSIL_NORMAL);
		break;
	case LV_LIST:
		dwLstType = LVS_LIST;
		SetImageList(&m_SmallImages, LVSIL_SMALL);

		break;
	case LV_DETAIL:
		dwLstType = LVS_REPORT;
		SetImageList(&m_SmallImages, LVSIL_SMALL);
		break;
	default:
		return;
	}

	dwStyle = dwStyle & 0xFFFFFFFC;
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | dwLstType);
	Invalidate();
}

void AListCtrl2::SetSortType(int iSortType)
{
	if ( iSortType >= SORT_NAME || iSortType <= SORT_TIME )
		m_iSortType = iSortType;
}

int AListCtrl2::AddItem(AListItem* pItem)
{
	ASSERT(GetItemCount() == m_aItems.GetSize());
	int iItem = m_aItems.Add(pItem);
	BuildTree(pItem->GetItemText(0), m_aItems.GetSize());
	ASSERT(m_IdxItem.size() == m_aItems.GetSize());
	ASSERT(m_ItemIdx.size() == m_aItems.GetSize());
	InsertItem(iItem, _TEXT(""));
	ASSERT(GetItemCount() == m_aItems.GetSize());
	return iItem;
}

void AListCtrl2::BuildTree(const ACHAR *filename, int ItemId)
{
	if (filename == NULL)
		return;
	
	m_IdxItem.push_back(ItemId-1);
	m_ItemIdx.push_back(ItemId-1);

	Node *point = m_root;
	Node *retNode = NULL;
	for (int i = 0; ; ++i)
	{
		ACHAR a = filename[i];
		if (point->Array[a] == NULL)
		{
			retNode = new Node;
			point->Array[a] = retNode;
			point = retNode;
			point->ItemID = -1;
			point->thechar = a;
		}
		else
		{
			point = point->Array[a];
		}
		
		if (filename[i+1]==NULL)
		{
			point->ItemID = ItemId-1;
			break;
		}


	}
}

void AListCtrl2::OnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;

	int iItemIndx= m_IdxItem[pItem->iItem];

	if (!m_aItems[iItemIndx]->IsVisible())
		return;

	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		lstrcpy(pItem->pszText, m_aItems[iItemIndx]->GetItemText(pItem->iSubItem));
	}

	if (pItem->mask & LVIF_IMAGE) //valid image?
	{
		switch (m_iLVType)
		{
		case LV_THUMBVIEW:
			pItem->iImage = m_aItems[iItemIndx]->GetThumbImgIdx(m_iThumbImgIdx);
			break;
		case LV_ICON:
		case LV_LIST:
		case LV_DETAIL:
			pItem->iImage= m_aItems[iItemIndx]->GetCommonImgIdx();
			break;
		default:
			pItem->iImage= m_aItems[iItemIndx]->GetCommonImgIdx();
			break;
		}
		
	}
}

void AListCtrl2::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here
	m_ContextMenu.TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this);
}

void AListCtrl2::OnChangeListStyle(UINT uID)
{
	switch(uID)
	{
	case IDU_THUMBNAIL:
		SetLVType(LV_THUMBVIEW);
		break;
	case IDU_ICON:
		SetLVType(LV_ICON);
		break;
	case IDU_LIST:
		SetLVType(LV_LIST);
		break;
	case IDU_DETAILS:
		SetLVType(LV_DETAIL);
		break;
	}
}

static void SwapItemArray(APtrArray<AListItem*>& aItems, int iItemL, int iItemR)
{
	AListItem* pItemTmp = aItems[iItemL];
	aItems[iItemL] = aItems[iItemR];
	aItems[iItemR] = pItemTmp;
}

struct CompareFunctor
{
	CompareFunctor(APtrArray<AListItem*>& aItems, int iPropIdx)
		: m_aItems(aItems)
		, m_iPropIdx(iPropIdx)
	{

	}

	bool operator() (int iLeft, int iRight) const;

private:
	APtrArray<AListItem*>& m_aItems;
	int m_iPropIdx;
};

bool CompareFunctor::operator () (int iLeft, int iRight) const
{
	AListItem* pItemLeft = m_aItems[iLeft];
	AListItem* pItemRight = m_aItems[iRight];
	bool bIsLess = pItemLeft->Less(pItemRight, m_iPropIdx);
//	ASSERT(!pItemRight->Less(pItemLeft, m_iPropIdx) == bIsLess);
	return bIsLess;
}

void AListCtrl2::ArchiveSelection(AArray<int>& aArchiveSelIdx, bool bIsArchive)
{
	if (bIsArchive)
	{
		aArchiveSelIdx.RemoveAll();

		POSITION pos = GetFirstSelectedItemPosition();
		while (pos)
		{
			int iItem = GetNextSelectedItem(pos);
			aArchiveSelIdx.Add(m_IdxItem[iItem]);
		}
	}
	else
	{
		for (int iIdx = 0; iIdx < GetItemCount(); ++iIdx)
		{
			SetItemState(iIdx, 0, LVIS_SELECTED);
		}

		for (int iSelectIdx = 0; iSelectIdx < aArchiveSelIdx.GetSize(); ++iSelectIdx)
		{
			SetItemState(m_ItemIdx[aArchiveSelIdx[iSelectIdx]], LVIS_SELECTED, LVIS_SELECTED);
		}
	}
}

void AListCtrl2::SortBy(int iSortType, bool bAscending = TRUE)
{
	static bool bDescend[4] = { false, false, false, false };
	ASSERT( iSortType >= SORT_NAME && iSortType <= SORT_TIME);
	m_iSortType = iSortType;

	AArray<int> aArchiveSelIdx(GetSelectedCount(), 4);
	ArchiveSelection(aArchiveSelIdx, true);

	int iPropIdx = iSortType;
	
	if (bAscending == TRUE)
		bDescend[iPropIdx] = !bDescend[iPropIdx];

	std::sort(m_IdxItem.begin(), m_IdxItem.end(), CompareFunctor(m_aItems, iPropIdx));
	
	if (bDescend[iPropIdx])
		std::reverse(m_IdxItem.begin(), m_IdxItem.end());

	ASSERT(m_IdxItem.size() == m_ItemIdx.size());
	for (size_t iIdx = 0; iIdx < m_IdxItem.size(); ++iIdx)
	{
		m_ItemIdx[m_IdxItem[iIdx]] = iIdx;
	}

	ArchiveSelection(aArchiveSelIdx, false);
	Invalidate();
}

void AListCtrl2::OnSortList(UINT uID)
{
	int iSortType = 0;
	switch (uID)
	{
	case IDU_NAME:
		iSortType = SORT_NAME;
		break;
	case IDU_SIZE:
		iSortType = SORT_SIZE;
		break;
	case IDU_TYPE:
		iSortType = SORT_TYPE;
		break;
	case IDU_TIME:
		iSortType = SORT_TIME;
		break;
	default:
		return;
	}
	
	SortBy(iSortType);
}

void AListCtrl2::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iSortType = 0;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	switch (pNMLV->iSubItem)
	{
	case 0:
		iSortType = SORT_NAME;
		break;
	case 1:
		iSortType = SORT_SIZE;
		break;
	case 2:
		iSortType = SORT_TYPE;
		break;
	case 4:
		iSortType = SORT_TIME;
		break;
	default:
		return;
	}

	SortBy(iSortType);
	*pResult = 0;
}

void AListCtrl2::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int iItem =((NMLISTVIEW*)pNMHDR)->iItem;
	
	if (!m_pListener)
		return;
	
	if (iItem < 0)
		m_pListener->OnItemClicked(NULL);
	else
		m_pListener->OnItemClicked(m_aItems[m_IdxItem[iItem]]);

	*pResult = 0;
}

void AListCtrl2::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int iItem =((NMLISTVIEW*)pNMHDR)->iItem;
	if (!m_pListener)
		return;

	if (iItem < 0)
		m_pListener->OnItemDbClicked(NULL);
	else
		m_pListener->OnItemDbClicked(m_aItems[m_IdxItem[iItem]]);


	*pResult = 0;
}

void AListCtrl2::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	DWORD dwDelta = GetTickCount() - m_dwLastTick;

	if (dwDelta>1000)
	{
		m_psearch = m_root;
	}

	if (m_psearch->Array.find((ACHAR)nChar) != m_psearch->Array.end())
	{
		m_psearch = m_psearch->Array[(ACHAR)nChar];
		if (m_psearch->ItemID >= 0)
		{
			int nId = m_ItemIdx[m_psearch->ItemID];
			EnsureVisible(nId, TRUE);
			SetItemState(nId, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		}
		else
		{
			abase::hash_map<TCHAR, Node*>::iterator retid = m_psearch->Array.begin();
			Node *retp = retid->second;
			while (retp->ItemID < 0)
			{
				retid = retp->Array.begin();
				retp = retid->second;
			}
			int nId = m_ItemIdx[retp->ItemID];
			EnsureVisible(nId, TRUE);
			SetItemState(nId, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		}

	}
	else
	{
		m_psearch = m_root;
	}
	m_dwLastTick = GetTickCount();	
}

void AListCtrl2::OnOdFindItem(NMHDR* pNMHDR, LRESULT *pResult)
{
	NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;

	*pResult = -1;

	if ((pFindInfo->lvfi.flags & LVFI_STRING) == 0)
		return;

	CString searchstr = pFindInfo->lvfi.psz;
	int startPos = pFindInfo->iStart;

	if (startPos >= (int)m_IdxItem.size())
		startPos = 0;

	int currentPos = startPos;

	do 
	{
		if (_tcsnicmp(m_aItems[m_IdxItem[currentPos]]->GetItemText(0), searchstr, searchstr.GetLength()) == 0)
		{
			*pResult = currentPos;
			break;
		}

		currentPos++;

		if (currentPos >= (int)m_IdxItem.size())
			currentPos = 0;

	} while (currentPos != startPos);

}

BOOL AListCtrl2::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//if (message == WM_NOTIFY)
	//{
	//	NMHDR* phdr = (NMHDR*)lParam;

	//	switch(phdr->code)
	//	{
	//	case LVN_ODFINDITEM: {
	//	
	//		NMLVFINDITEM* pFindItem = (NMLVFINDITEM*)lParam;
	//		switch (pFindItem->lvfi.flags)
	//		{
	//		case LVFI_PARAM:
	//			break;
	//		case LVFI_PARTIAL: {

	//			const TCHAR* pStr = pFindItem->lvfi.psz;
	//			Node * psearch = m_root;
	//			while (TCHAR ch = *pStr++)
	//			{
	//				abase::hash_map<TCHAR, Node*>::iterator itr = psearch->Array.find(ch);
	//				if (itr == psearch->Array.end())
	//				{
	//					*pResult = -1;
	//					return TRUE;
	//				}
	//				psearch = itr->second;
	//			}

	//			if (psearch != NULL)
	//			{
	//				if (psearch->ItemID >= 0)
	//					*pResult = m_ItemIdx[psearch->ItemID];
	//			}
	//			break; }
	//		case LVFI_STRING:
	//			for (int iIdx = 0; iIdx < m_aItems.GetSize(); ++iIdx)
	//			{
	//				if (strcmp(m_aItems[iIdx]->GetItemText(0), pFindItem->lvfi.psz) == 0)
	//				{
	//					*pResult = iIdx;
	//					return TRUE;
	//				}
	//			}
	//			break;
	//		case LVFI_WRAP:
	//			break;
	//		case LVFI_NEARESTXY:
	//			break;
	//		}
	//		
	//		*pResult = -1;
	//		return TRUE;

	//		break;}
	//	default:
	//		return CListCtrl::OnChildNotify(message, wParam, lParam, pResult);
	//	}
	//	return FALSE;
	//}

	return CListCtrl::OnChildNotify(message, wParam, lParam, pResult);
}