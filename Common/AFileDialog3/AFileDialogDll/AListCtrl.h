#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include "TypeDefs.h"

/***FILE_LIST_STYLE***/
enum
{
	FLS_THUMBNAIL,
	FLS_ICON,
	FLS_LIST,
	FLS_DETAILS
};

/***FILE_LIST_SORT_MOTHOD***/
enum
{
	FLSM_NAME,
	FLSM_SIZE,
	FLSM_TYPE,
	FLSM_TIME
};

class AListCtrl : public CListCtrl
{
public:	
	struct _FILEITEM 
	{
		_FILEITEM();
		~_FILEITEM();
		_FILEITEM(const _FILEITEM& rhs);
		_FILEITEM& operator = (const _FILEITEM& rhs);		

	public:
		CBitmap*			pBitmapIcon;
		CString				csFileName;
		ULONGLONG			nSize;
		CString				csFileType;
		ULONGLONG			uModifiedTime;
		CBitmap*			m_pbmUsing;

	};

	class Listener
	{
	public:
		virtual void OnItemClicked(int nItem) = 0;
		virtual void OnItemDbClicked(int nItem) = 0;
	};

	typedef AFVector<Listener*> ListenerArray;
	typedef ListenerArray::iterator ListenerIterator;

public:
	AListCtrl(void);
	~AListCtrl(void);
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void AddListener(Listener* pListener);
	void RemoveListener(Listener* pListener);
	bool SetIconSize(int nSize);
	void SetItems(const AFVector<_FILEITEM>& Items);
	void ClearItems();
	AFVector<CString> GetSelectedFiles();
	AFVector<CString> GetCheckedFiles();
	void CheckFiles(const AFVector<CString>& vecFileNames);
	void SelectFiles(const AFVector<CString>& vecFileNames);
	void List();
	void SetListStyle(int nStyle);
	void AddItem(const _FILEITEM& fileItem);
	void SortBy(int nSortMethod, bool bAscending = true);
	int GetListStyle() {return m_nStyle;}
	int GetSortStyle() {return m_nSortStyle;}
	void EnableMenu(bool bEnable = true) {m_bEnableMenu = bEnable;}

protected:	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void ChangeListStyle(UINT nID);
	afx_msg void ChangeSortMethod(UINT nID);
	afx_msg void OnNMItemClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMItemDbClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNcDestroy();

protected:
	void setIconSpacing();

protected:
	CImageList						m_imageList;
	int								m_nIconSize;
	int								m_nCol;
	AFVector<_FILEITEM>				m_Items;
	int								m_nItemSize;
	int								m_nStyle;
	int								m_nSortStyle;
	CRect							m_rectClient;
	CWnd*							m_pParentWnd;
	bool							m_bChangedStyle;
	CMenu							m_Menu;
	CMenu							m_ViewMenu;
	CMenu							m_ArrayMenu;
	bool							m_bEnableMenu;
	ListenerArray					m_aListeners;

	DECLARE_MESSAGE_MAP()
};
