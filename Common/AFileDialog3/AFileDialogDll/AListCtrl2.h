/*
* FILE: AListCtrl2.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/18
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AListCtrl2_H_
#define _AListCtrl2_H_


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define IMG_SIZE_THUMB	96
#define IMG_SIZE_ICON	32
#define IMG_SIZE_SMALL	16

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class AListCtrl2;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AListItem
//	
///////////////////////////////////////////////////////////////////////////

class AListItem
{
public:		//	Types

public:		//	Constructor and Destructor

	AListItem(void);
	virtual ~AListItem(void);

public:		//	Attributes

public:		//	Operations

	bool IsVisible() const { return m_bVisible; }
	void SetVisible(bool b) { m_bVisible = b; }
	int GetItemIdx() const { return m_iItem; }
	int GetItemTextCount() const { return m_aItemText.GetSize(); }
	void SetItemText(int iIdx, const TCHAR* szSubItem);
	const TCHAR* GetItemText(int iIdx) const { return m_aItemText[iIdx]; }
	void SetCommonImgIdx(int iCommonImgIdx) { m_iCommonImgIdx = iCommonImgIdx; }
	int GetCommonImgIdx() const { return m_iCommonImgIdx; }

	void SetThumbImgIdx(int iIdx, int iThumbImgIdx) { m_iThumbImgIdx[iIdx] = iThumbImgIdx; }
	int GetThumbImgIdx(int iIdx) const { return m_iThumbImgIdx[iIdx]; }

	virtual bool Less(const AListItem* pRhs, int iPropIdx) const = 0;

protected:	//	Attributes

	AListCtrl2* m_pListCtrl;
	AArray<CString> m_aItemText;
	int m_iItem;
	int m_iCommonImgIdx;
	int m_iThumbImgIdx[2];		// 0: normal thumb image, 1: alpha blended thumb image
	bool m_bVisible;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class AListCtrl2
//	
///////////////////////////////////////////////////////////////////////////

class AListCtrl2 : public CListCtrl
{

public:		//	Types

	enum
	{
		LV_THUMBVIEW,	// 缩略图
		LV_ICON,		// 图标
		LV_LIST,		// 列表
		LV_DETAIL,		// 详细信息
	};

	enum
	{
		SORT_NAME = 0,
		SORT_SIZE,
		SORT_TYPE,
		SORT_TIME,
	};

	class Listener
	{
	public:
		virtual ~Listener() = 0 {}
		virtual void OnItemClicked(AListItem* pItem) = 0;
		virtual void OnItemDbClicked(AListItem* pItem) = 0;
	};

	struct Node
	{
		int ItemID;
		ACHAR thechar;
		abase::hash_map<TCHAR, Node*> Array;
	};

public:		//	Constructor and Destructor

	AListCtrl2(void);
	virtual ~AListCtrl2(void);

public:		//	Attributes

public:		//	Operations

	void SetLVType(int iLVType);
	void SetSortType(int iSortType);
	int GetLVType() const { return m_iLVType; }
	int GetSortType() const { return m_iSortType; }
	void SetListener(Listener* pListener) { m_pListener = pListener;  }
	int AddCommonImage(CBitmap* pImage);
	int AddThumbImage(CBitmap* pImage);
	void ReplaceThumbImage(int iIdx, CBitmap* pImage);
	int AddItem(AListItem* pItem);
	AListItem* GetItem(int iItem) const 
	{ 
		if (iItem >= static_cast<int>(m_IdxItem.size()))
			return NULL;

		if (m_IdxItem[iItem] >= m_aItems.GetSize())
			return NULL;

		return m_aItems[m_IdxItem[iItem]];
	}
	void ClearItems();
	void SortBy(int iSortType, bool bAscending);
	void SetUseThumbImgType(int iThumbImgIdx) { m_iThumbImgIdx = iThumbImgIdx; }
	void SetThumbNailImgList(CImageList* pImgLst) { m_pThumbImages = pImgLst; }

protected:	//	Attributes

	int m_iLVType;
	int m_iSortType;
	int m_iThumbImgIdx;				// 0 : no alpha mode / 1 : alpha mode
	Listener* m_pListener;
	CBitmap m_ThumbMaskMap;
	const COLORREF m_clKey;
	CImageList *m_pThumbImages;		// Use outside image list
	CImageList m_IconImages;
	CImageList m_SmallImages;
	APtrArray<AListItem*> m_aItems;
	APtrArray<CBitmap*> m_aCacheBitmaps;
	CMenu m_ViewMenu;
	CMenu m_SortMenu;
	CMenu m_ContextMenu;
	DWORD m_dwLastTick;
	Node *m_root;
	Node *m_psearch;
	abase::vector<int> m_IdxItem;
	abase::vector<int> m_ItemIdx;


protected:	//	Operations

	void ArchiveSelection(AArray<int>& aArchiveSelIdx, bool bIsArchive);
	CBitmap* CreateCacheBitmap(HBITMAP hBitmap);

	DECLARE_MESSAGE_MAP()

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void BuildTree(const ACHAR *filename, int ItemId);
	void DeleteTree(Node* root);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOdFindItem(NMHDR* pNMHDR, LRESULT *pResult);
	afx_msg void OnChangeListStyle(UINT uID);
	afx_msg void OnSortList(UINT uID);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AListCtrl2_H_


