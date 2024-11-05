#if !defined(AFX_PAGEEDITHOOKS_H__7532BB28_050F_4C73_8D4D_B7AEDA537FD5__INCLUDED_)
#define AFX_PAGEEDITHOOKS_H__7532BB28_050F_4C73_8D4D_B7AEDA537FD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageEditHooks.h : header file
//

#include "WndInput.h"
#include "WndCombo.h"

class CModel;
class A3DSkeleton;
class CHook;
class A3DHanger;

/////////////////////////////////////////////////////////////////////////////
// CPageEditHooks dialog

class CPageEditHooks : public CDialog
{
public:		//	Types

	//	Editing status
	struct EDITSTATUS
	{
		bool	bEditing;	//	true, be editing
		int		iWndID;		//	ID of window whose content was edited
		int		iItem;		//	Item index
		int		iSubItem;	//	Sub item index
	};

	//	Sub item of hanger list
	enum
	{
		HSI_BINDTO = 0,		//	Bind to what
		HSI_NAME,			//	Hanger name
		HSI_TYPE,			//	Hanger type
		HSI_SCALE,			//	Inherit scale enable
		HSI_OBJECT,			//	Hanger object
	};

// Construction
public:

	CPageEditHooks(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPageEditHooks)
	enum { IDD = IDD_EDITHOOK };
	CListCtrl	m_HookList;
	CListCtrl	m_HangerList;
	//}}AFX_DATA

public:		//	Attributes

public:		//	Operations

	//	Release resources
	void Release();
	//	Bind model object with this dialog
	bool BindModel(CModel* pModel);

	//	Update hook list
	bool UpdateHookList();
	//	Check or uncheck a item in hook list
	void CheckHookItem(int iItem);

	//	Edit a hanger list item
	void EditHangerListItem(int iItem, int iSubItem);
	//	End editing
	void EndEditItem(bool bCancel);
	//	Edit a hanger list item through combo box
	void ComboHangerListItem(int iItem, int iSubItem);
	//	End editing hanger list item through combo box
	void EndComboItem(bool bCancel);
	//	Be editing ?
	bool BeEditing() { return m_EditStatus.bEditing; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageEditHooks)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	CModel*		m_pModel;
	EDITSTATUS	m_EditStatus;
	CWndInput	m_InputWnd;
	CWndCombo	m_ComboWnd;

protected:	//	Operations

	//	Clear all information in dialog
	void ClearAllInfo();
	//	Update hanger Item
	void UpdateHangerItem(int iItem);
	//	Update hanger list
	bool UpdateHangerList();

	// Generated message map functions
	//{{AFX_MSG(CPageEditHooks)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedListHooks(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnAddhanger();
	afx_msg void OnBtnDeletehanger();
	afx_msg void OnBtnStophanger();
	afx_msg void OnBtnTesthanger();
	afx_msg void OnDblclkListHangers(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEEDITHOOKS_H__7532BB28_050F_4C73_8D4D_B7AEDA537FD5__INCLUDED_)
