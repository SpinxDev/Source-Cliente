#include "StdAfx.h"

using std::set;
#include <algorithm>

#define IDU_THUMBNAIL		40001
#define IDU_ICON					40002
#define IDU_LIST					40003
#define IDU_DETAILS			40004

#define IDU_NAME					40006
#define IDU_SIZE					40007
#define IDU_TYPE					40008
#define IDU_TIME					40009


int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	bool bAscending = (lParamSort >> 10 == 1);
	if (bAscending)
	{
		lParamSort -= (1<<10);
	}
	AListCtrl::_FILEITEM* pItem1 = (AListCtrl::_FILEITEM*)lParam1;
	AListCtrl::_FILEITEM* pItem2 = (AListCtrl::_FILEITEM*)lParam2;
	int nReturn = 0;
	switch(lParamSort)
	{
	case FLSM_NAME:
		nReturn = _tcscmp(pItem1->csFileName,pItem2->csFileName);
		break;
	case FLSM_SIZE:
		nReturn = (pItem1->nSize < pItem2->nSize)?-1:1;
		break;
	case FLSM_TYPE:		
		nReturn = _tcscmp(pItem1->csFileType,pItem2->csFileType);
		break;
	case FLSM_TIME:
		nReturn = (pItem1->uModifiedTime < pItem2->uModifiedTime)?-1:1;
		break;
	}
	//	Special process for directory type, always make them higher than other types
	if (lParamSort != FLSM_TYPE) {
		CString type1(pItem1->csFileType);
		CString type2(pItem2->csFileType);
		type1.MakeUpper(); type2.MakeUpper();
		if (type1 == "DIRECTORY" && type2 != "DIRECTORY")
			nReturn = 1;
		else if (type1 != "DIRECTORY" && type2 == "DIRECTORY")
			nReturn = -1;
	}
	if (!bAscending)
		nReturn *= -1;
	return nReturn;
}

AListCtrl::_FILEITEM::_FILEITEM()
{
	pBitmapIcon = NULL;
	m_pbmUsing = new CBitmap;
}

AListCtrl::_FILEITEM::~_FILEITEM()
{
	if (m_pbmUsing)
	{
		m_pbmUsing->DeleteObject();
		delete m_pbmUsing;
		m_pbmUsing = NULL;
	}
}

AListCtrl::_FILEITEM::_FILEITEM(const _FILEITEM& rhs)
: csFileName(rhs.csFileName)
, nSize(rhs.nSize)
, csFileType(rhs.csFileType)
, uModifiedTime(rhs.uModifiedTime)
, pBitmapIcon(rhs.pBitmapIcon)
{
	m_pbmUsing = new CBitmap;
}

AListCtrl::_FILEITEM& AListCtrl::_FILEITEM::operator = (const _FILEITEM& rhs)
{
	if (&rhs == this)
		return *this;

	csFileName = rhs.csFileName;
	nSize = rhs.nSize;
	csFileType = rhs.csFileType;
	uModifiedTime = rhs.uModifiedTime;
	pBitmapIcon = rhs.pBitmapIcon;

	return *this;
}

BEGIN_MESSAGE_MAP(AListCtrl, CListCtrl)
	ON_WM_SIZE()
	ON_WM_NCDESTROY()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, AListCtrl::OnLvnColumnclick)
	ON_NOTIFY_REFLECT(NM_CLICK, AListCtrl::OnNMItemClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, AListCtrl::OnNMItemDbClick)
	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(IDU_THUMBNAIL, IDU_DETAILS, ChangeListStyle)
	ON_COMMAND_RANGE(IDU_NAME, IDU_TIME, ChangeSortMethod)
END_MESSAGE_MAP()

AListCtrl::AListCtrl(void)
{
	m_nIconSize				= 128;
	m_nCol					= 0;
	m_nItemSize				= 0;
	m_nStyle				= FLS_THUMBNAIL;
	m_nSortStyle			= IDU_NAME;
	m_bChangedStyle		= false;
	m_bEnableMenu				= false;
}

AListCtrl::~AListCtrl(void)
{
}

void AListCtrl::setIconSpacing()
{
	DWORD dwListExStyle = GetExtendedStyle();
	CSize szSpace;
	if (m_nStyle == FLS_THUMBNAIL) {
		if (!(dwListExStyle & LVS_EX_CHECKBOXES)) {
			szSpace.cx = m_nIconSize + 8;
			szSpace.cy = m_nIconSize + 25;
		}
		else
		{
			szSpace.cx = szSpace.cy = m_nIconSize + 25;
		}
	}
	else if (m_nStyle == FLS_ICON)
	{
		szSpace.cx = szSpace.cy = m_nIconSize * 2;
	}

	SetIconSpacing(szSpace);
}

BOOL AListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	m_rectClient = rect;
	m_pParentWnd = pParentWnd;

	DWORD dwListStyle = LVS_ICON | LVS_SHOWSELALWAYS;
	if (!CListCtrl::Create(dwStyle | dwListStyle,m_rectClient,m_pParentWnd,1))
		return FALSE;
	DWORD dwListExStyle = GetExtendedStyle();

	setIconSpacing();

	if(!m_imageList.Create(m_nIconSize,m_nIconSize,ILC_COLORDDB,0,RGB(0,0,0)))
		return FALSE;

	static const TCHAR* _szViewSubMenu[] = 
	{
		_T("缩略图(&H)"),
		_T("图标(&N)"),
		_T("列表(&L)"),
		_T("详细信息(&D)")
	};
	static const TCHAR* _szArraySubMenu[] = 
	{
		_T("名称(&N)"),
		_T("大小(&S)"),
		_T("类型(&T)"),
		_T("修改时间(&M)")
	};

	InsertColumn(0,_T("文件名"),LVCFMT_LEFT,200);
	InsertColumn(1,_T("大小"),LVCFMT_RIGHT,120);
	InsertColumn(2,_T("类型"),LVCFMT_LEFT,100);
	InsertColumn(3,_T("修改日期"),LVCFMT_LEFT,165);

	m_ViewMenu.CreatePopupMenu();
	m_ArrayMenu.CreatePopupMenu();
	m_Menu.CreatePopupMenu();
	m_Menu.InsertMenu(0,MF_POPUP | MF_STRING,(UINT_PTR)m_ViewMenu.m_hMenu,_T("查看(&V)"));
	m_Menu.InsertMenu(1,MF_POPUP | MF_STRING,(UINT_PTR)m_ArrayMenu.m_hMenu,_T("排列图标(&I)"));

	for (int i=0;i<4;++i)
	{
		m_ViewMenu.InsertMenu(i,MF_STRING,IDU_THUMBNAIL+i, _szViewSubMenu[i]);
		m_ArrayMenu.InsertMenu(i,MF_STRING,IDU_NAME+i, _szArraySubMenu[i]);
	}
	m_ViewMenu.CheckMenuRadioItem(0,3,0,MF_BYPOSITION);
	
	return TRUE;
}

bool AListCtrl::SetIconSize(int nSize)
{
	if (nSize < 1)
		return false;
	if(m_nIconSize == nSize && m_imageList.GetImageCount() == m_nItemSize && !m_bChangedStyle)
		return false;
	if (m_nStyle == FLS_THUMBNAIL)
		m_nIconSize = nSize;

	m_imageList.DeleteImageList();
	
	if(!m_imageList.Create(m_nIconSize,m_nIconSize,ILC_COLORDDB,0,RGB(0,0,0)))
		return false;

	for (int i=0;i<m_nItemSize;++i)
	{
		HBITMAP hBitmap = (HBITMAP)CopyImage(m_Items[i].pBitmapIcon->m_hObject, IMAGE_BITMAP, m_nIconSize, m_nIconSize, LR_CREATEDIBSECTION);
		m_Items[i].m_pbmUsing->Detach();
		m_Items[i].m_pbmUsing->Attach(hBitmap);
		m_imageList.Add(m_Items[i].m_pbmUsing, RGB(0,0,0));
		//DeleteObject(hBitmap);
	}

	int nImageListType;
	switch(m_nStyle)
	{
	case FLS_THUMBNAIL:
	case FLS_ICON:
		nImageListType = LVSIL_NORMAL;
		break;
	case FLS_LIST:
	case FLS_DETAILS:
		nImageListType = LVSIL_SMALL;
		break;
	}
	SetImageList(&m_imageList,nImageListType);
//	if (m_nStyle == FLS_THUMBNAIL)
	{
		setIconSpacing();
	}
	List();
	return true;
}

void AListCtrl::SetItems(const AFVector<_FILEITEM>& Items)
{
	m_Items = Items;
	m_nItemSize = (int)m_Items.size();
	if (!m_hWnd)
		return;
	if (!SetIconSize(m_nIconSize))
		List();
}

void AListCtrl::ClearItems()
{
	m_Items.clear();
	m_nItemSize = (int)m_Items.size();
	if (!m_hWnd)
		return;
	if (!SetIconSize(m_nIconSize))
		List();
}

void AListCtrl::List()
{	
	DeleteAllItems();
	for (int i=0;i<m_nItemSize;++i)
	{
		LV_ITEM lvItem = {0};
		lvItem.lParam = (LPARAM)&m_Items[i];
		lvItem.iImage = i;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvItem.pszText = m_Items[i].csFileName.GetBuffer(0);
		lvItem.iItem = InsertItem(&lvItem);

		CString csSizeKB;
		ULONGLONG nSizeKB = m_Items[i].nSize/1024 + ((m_Items[i].nSize%1024 == 0)?0:1);
		csSizeKB.Format(_T("%d"), nSizeKB);
		for (int j=csSizeKB.GetLength()%3; j<csSizeKB.GetLength(); j+=3)
		{
			if (j == 0 || j == (csSizeKB.GetLength() - 1))
				continue;
			csSizeKB.Insert(j,_T(','));
			++j;
		}
		csSizeKB += _T(" KB");
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 1;
		lvItem.pszText = csSizeKB.GetBuffer(0);
		SetItem(&lvItem);
		
		lvItem.iSubItem = 2;
		lvItem.pszText = m_Items[i].csFileType.GetBuffer(0);
		SetItem(&lvItem);

		CTime tModifiedTime(m_Items[i].uModifiedTime);
		CString csModifiedTime;
		csModifiedTime.Format(_T("%4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d"),
			tModifiedTime.GetYear(),
			tModifiedTime.GetMonth(),
			tModifiedTime.GetDay(),
			tModifiedTime.GetHour(),
			tModifiedTime.GetMinute(),
			tModifiedTime.GetSecond());
		lvItem.iSubItem = 3;
		lvItem.pszText = csModifiedTime.GetBuffer(0);
		SetItem(&lvItem);
	}	
}

void AListCtrl::SetListStyle(int nStyle)
{
	if (m_nStyle == nStyle)
		return;
	m_nStyle = nStyle;
	m_bChangedStyle = true;
	DWORD dwListStyle = WS_VISIBLE | WS_CHILD | LVS_SHOWSELALWAYS;
	switch(nStyle)
	{
	case FLS_THUMBNAIL:
		dwListStyle |= LVS_ICON;
		m_nIconSize = 96;
		break;
	case FLS_ICON:
		dwListStyle |= LVS_ICON;
		m_nIconSize = 32;
		break;
	case FLS_LIST:
		dwListStyle |= LVS_LIST;
		m_nIconSize = 16;
		break;
	case FLS_DETAILS:
		dwListStyle |= LVS_REPORT;
		m_nIconSize = 16;
		break;
	}

	ModifyStyle(LVS_ICON|LVS_LIST|LVS_REPORT, dwListStyle);
	SetIconSize(m_nIconSize);
	ChangeListStyle(m_nStyle + IDU_THUMBNAIL);

	//	Dam it! this is just only a hacking way to avoid some strange appearance
	//	when we change style from List to Icon/Thumbnail
	//if (m_nStyle == FLS_THUMBNAIL || m_nStyle == FLS_ICON) 
	{
		Arrange(LVA_ALIGNTOP);
		SortItems(CompareFunc, FLSM_NAME);
	}
}


void AListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	if (!m_hWnd)
		return;
	if (m_nStyle == FLS_DETAILS)
		return;
	if (GetItemCount() == 0)
		return;

	m_rectClient = CRect(0,0,cx,cy);
	CRect rectItem;
	GetItemRect(0,&rectItem,LVIR_BOUNDS);
	int nCol = (int)((float)m_rectClient.Width()/rectItem.Width() + 0.5f);
	if (m_nCol == 0)
	{
		m_nCol = nCol;
		return;
	}
	if (m_nCol == nCol)
		return;
	m_nCol = nCol;
	Arrange(LVA_DEFAULT);
}

void AListCtrl::AddItem(const _FILEITEM& fileItem)
{
	m_Items.push_back(fileItem);
	m_nItemSize = (int)m_Items.size();
	SetIconSize(m_nIconSize);
	List();
}

AFVector<CString> AListCtrl::GetSelectedFiles()
{
	AFVector<CString> vecSelectedFiles;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = GetNextSelectedItem(pos);
		_FILEITEM* pFileItem = (_FILEITEM*)GetItemData(nItem);
		vecSelectedFiles.push_back(pFileItem->csFileName);
	}
	return vecSelectedFiles;
}

void AListCtrl::SelectFiles(const AFVector<CString>& vecFileNames)
{
	std::set<CString> setFileNames;
	for (AFVector<CString>::const_iterator itr = vecFileNames.begin()
		; itr != vecFileNames.end()
		; ++itr)
	{
		CString str = (*itr);
		str.MakeLower();
		setFileNames.insert(str);
	}

	for (int i=0;i<GetItemCount();++i)
	{
		_FILEITEM* pFileItem = (_FILEITEM*)GetItemData(i);
		CString csName(pFileItem->csFileName);
		csName.MakeLower();
		if (setFileNames.find(csName) != setFileNames.end())
		{
			SetItemState(i,LVIS_SELECTED, LVIS_SELECTED);
		}
	}
}

AFVector<CString> AListCtrl::GetCheckedFiles()
{	
	AFVector<CString> vecCheckedFiles;

	DWORD dwExtendedStyle = GetExtendedStyle();
	if (!(dwExtendedStyle & LVS_EX_CHECKBOXES))
		return vecCheckedFiles;

	for (int i=0;i<GetItemCount();++i)
	{
		if(GetCheck(i))
		{
			_FILEITEM* pFileItem = (_FILEITEM*)GetItemData(i);
			CString csName(pFileItem->csFileName);
			csName.MakeLower();
			vecCheckedFiles.push_back(csName);
		}		
	}
	return vecCheckedFiles;
}

void AListCtrl::CheckFiles(const AFVector<CString>& vecFileNames)
{
	using namespace std;
	DWORD dwExtendedStyle = GetExtendedStyle();
	if (!(dwExtendedStyle & LVS_EX_CHECKBOXES))
		return;

	std::set<CString> setFileNames;
	for (AFVector<CString>::const_iterator itr = vecFileNames.begin()
		; itr != vecFileNames.end()
		; ++itr)
	{
		CString str = (*itr);
		str.MakeLower();
		setFileNames.insert(str);
	}

	for (int i=0;i<GetItemCount();++i)
	{
		_FILEITEM* pFileItem = (_FILEITEM*)GetItemData(i);
		CString csName(pFileItem->csFileName);
		csName.MakeLower();
		if (setFileNames.find(csName) != setFileNames.end())
		{
			SetCheck(i);
		}
	}
}

void AListCtrl::SortBy(int nSortMethod, bool bAscending)
{
	if (bAscending)
	{
		nSortMethod |= (1<<10);
	}
	SortItems(CompareFunc,nSortMethod);
}

void AListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	ChangeSortMethod(pNMLV->iSubItem + IDU_NAME);
	*pResult = 0;
}

void AListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if(m_bEnableMenu)
		m_Menu.TrackPopupMenu(TPM_RIGHTBUTTON,point.x,point.y,this);
}

void AListCtrl::ChangeListStyle(UINT nID)
{
	m_ViewMenu.CheckMenuRadioItem(0, 3, nID - IDU_THUMBNAIL,MF_BYPOSITION);
	SetListStyle(nID - IDU_THUMBNAIL);
}

void AListCtrl::ChangeSortMethod(UINT nID)
{
	static bool _bMethodAsc[] = 
	{
		false,
		false,
		false,
		false
	};
	int nIndex = nID - IDU_NAME;
	m_ArrayMenu.CheckMenuRadioItem(0, 3, nID - IDU_NAME,MF_BYPOSITION);	
	_bMethodAsc[nIndex] = !_bMethodAsc[nIndex];	
	SortBy(nIndex,_bMethodAsc[nIndex]);
}

void AListCtrl::AddListener(Listener* pListener)
{
	if (pListener)
		m_aListeners.push_back(pListener);
}

void AListCtrl::RemoveListener(Listener* pListener)
{
	if (!pListener)
		return;

	ListenerIterator itr = std::find(m_aListeners.begin(), m_aListeners.end(), pListener);
	if (itr == m_aListeners.end())
		return;

	m_aListeners.erase(itr);
}

void AListCtrl::OnNMItemClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	int hItem =((NMLISTVIEW*)pNMHDR)->iItem;
	for (ListenerIterator itr = m_aListeners.begin()
		; itr != m_aListeners.end()
		; ++itr)
	{
		(*itr)->OnItemClicked(hItem);
	}	
}

void AListCtrl::OnNMItemDbClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	int hItem =((NMLISTVIEW*)pNMHDR)->iItem;
	for (ListenerIterator itr = m_aListeners.begin()
		; itr != m_aListeners.end()
		; ++itr)
	{
		(*itr)->OnItemDbClicked(hItem);
	}
}

void AListCtrl::OnNcDestroy()
{
	//m_imageList.DeleteImageList();
	CListCtrl::OnNcDestroy();
}