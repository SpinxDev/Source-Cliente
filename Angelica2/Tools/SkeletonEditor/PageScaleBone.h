#if !defined(AFX_PAGESCALEBONE_H__75BA4E38_B4CB_4B56_907B_39227F60E1F2__INCLUDED_)
#define AFX_PAGESCALEBONE_H__75BA4E38_B4CB_4B56_907B_39227F60E1F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageScaleBone.h : header file
//

#include "WndInput.h"
#include "WndCombo.h"

class CModel;

/////////////////////////////////////////////////////////////////////////////
// CPageScaleBone dialog

class CPageScaleBone : public CDialog
{
public:		//	Types

	//	Sub item index of bone list
	enum
	{
		BSI_NAME = 0,
		BSI_LOCALLENGTH,
		BSI_LOCALTHICK,
		BSI_WHOLESCALE,
	};

	//	Editing status
	struct EDITSTATUS
	{
		bool	bEditing;	//	true, be editing
		int		iWndID;		//	ID of window whose content was edited
		int		iItem;		//	Item index
		int		iSubItem;	//	Sub item index
	};

public:		//	Construction
	CPageScaleBone(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPageScaleBone)
	enum { IDD = IDD_SCALEBONE };
	CListCtrl	m_BoneList;
	CString	m_strWarning;
	//}}AFX_DATA

public:		//	Attributes

public:		//	Operations

	//	Release resources
	void Release();
	//	Bind model object with this dialog
	bool BindModel(CModel* pModel);

	//	Update bone list check states
	void UpdateBoneListCheckStates();

	//	Edit a bone list item through input window
	void EditBoneListItem(int iItem, int iSubItem, bool bColumn);
	//	End editing item through input window
	void EndEditItem(bool bCancel);
	//	Edit a bone list item through combo box
	void ComboBoneListItem(int iItem, int iSubItem, bool bColumn);
	//	End editing item through combo box
	void EndComboItem(bool bCancel);
	//	Be editing ?
	bool BeEditing() { return m_EditStatus.bEditing; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageScaleBone)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	CModel*		m_pModel;
	CWndInput	m_InputWnd;
	CWndCombo	m_ComboWnd;

	EDITSTATUS	m_EditStatus;

protected:	//	Operations

	//	Clear all information in dialog
	void ClearAllInfo();
	//	Update bone list
	void UpdateBoneList();

	//	Update bone selection according to joint list checking states
	void UpdateBoneSelection();
	//	Update bone scale data
	void UpdateBoneScaleData(int iItem, int iSubItem);

	// Generated message map functions
	//{{AFX_MSG(CPageScaleBone)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedListBones(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListBones(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnRestore();
	//}}AFX_MSG

	//	On activate / disactivate this page
	afx_msg void OnActivatePage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESCALEBONE_H__75BA4E38_B4CB_4B56_907B_39227F60E1F2__INCLUDED_)
