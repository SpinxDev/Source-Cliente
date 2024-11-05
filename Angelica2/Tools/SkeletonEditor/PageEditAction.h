#if !defined(AFX_PAGEEDITACTION_H__B40DF0A4_5677_4E80_9A34_A1E5E73611A2__INCLUDED_)
#define AFX_PAGEEDITACTION_H__B40DF0A4_5677_4E80_9A34_A1E5E73611A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageEditAction.h : header file
//

#include "WndInput.h"
#include "WndCombo.h"
#include "AString.h"
#include "AArray.h"

class CModel;
class A3DAnimJoint;
class A3DSkeleton;
class A3DSkinModelActionCore;

/////////////////////////////////////////////////////////////////////////////
// CPageEditAction dialog

class CPageEditAction : public CDialog
{
public:		//	Types

	//	Action channel index
	enum
	{
		ACTCHA_NORMAL = 1,
	};

	//	Joint data associated with joint list item
	struct JOINTDATA
	{
		int				iBone;		//	Corresponding bone index
		A3DAnimJoint*	pJoint;		//	Joint object
		int				iJoint;		//	Joint index in skeleton
	};

	//	Editing status
	struct EDITSTATUS
	{
		bool	bEditing;	//	true, be editing
		int		iWndID;		//	ID of window whose content was edited
		int		iItem;		//	Item index
		int		iSubItem;	//	Sub item index
	};

	//	Select type
	enum
	{
		SELECT_ALL = 0,		//	Select all
		SELECT_NONE,		//	Unselect all
		SELECT_REVERSE,		//	Reverse select
	};

	//	Sub item of joint list
	enum
	{
		JSI_JOINTNAME = 0,	//	Joint name
	};

	//	Sub item of action list
	enum
	{
		ASI_NAME = 0,		//	Action name
		ASI_STARTFRAME,
		ASI_ENDFRAME,
		ASI_TRACKFILE,
	};

// Construction
public:

	CPageEditAction(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPageEditAction)
	enum { IDD = IDD_EDITANIMATION };
	CSliderCtrl	m_PlaySlider;
	CListCtrl	m_ActionList;
	CListCtrl	m_JointList;
	CString	m_strAnimInfo;
	int		m_iLoopNum;
	BOOL	m_bUseChannel;
	int		m_iCurFrame;
	BOOL	m_bUseAbsTrack;
	//}}AFX_DATA

public:		//	Attributes

public:		//	Operations

	//	Release resources
	void Release();
	//	Bind model object with this dialog
	bool BindModel(CModel* pModel);

	//	Select joint actions
	void SelectJointActions(int iSelectType);
	//	Check or uncheck a joint list item
	void CheckJointListItem(int iItem);
	//	Update joint list item check states
	void UpdateJointListCheckStates();

	//	Edit a joint list item
	void EditJointListItem(int iItem, int iSubItem, bool bColumn);
	//	Edit a action list item
	void EditActionListItem(int iItem, int iSubItem);
	//	End editing
	void EndEditItem(bool bCancel);
	//	Be editing ?
	bool BeEditing() { return m_EditStatus.bEditing; }
	//	Enable playing controls
	void EnablePlayControls(bool bStepPlay);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageEditAction)
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
	//	Reset joint list
	bool ResetJointList();
	//	Reset action list
	bool ResetActionList();
	//	Update animation information
	void UpdateAnimInfo(bool bClear);
	//	Release joint list
	void ReleaseJointList();

	//	Change selected action
	void ChangeSelectedAction(int iIndex, A3DSkinModelActionCore* pNewAction);
	//	Get selected action
	A3DSkinModelActionCore* GetSelectedAction();
	//	Search action item by action's name
	int GetActionItem(const char* szName);
	//	Release all actions
	void ReleaseAllActions();

	//	Update bone selection according to joint list checking states
	void UpdateBoneSelection();
	//	Build action channel
	void BuildActionChannel();

	// Generated message map functions
	//{{AFX_MSG(CPageEditAction)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListJoint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListJoint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnAddaction();
	afx_msg void OnBtnDelaction();
	afx_msg void OnBtnStopaction();
	afx_msg void OnBtnSelectall();
	afx_msg void OnBtnReverseselect();
	afx_msg void OnBtnUnselectall();
	afx_msg void OnItemchangedListAction(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListAction(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnExportacts();
	afx_msg void OnBtnImportacts();
	afx_msg void OnBtnExportchannel();
	afx_msg void OnBtnPlayaction();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBtnPlaystep();
	afx_msg void OnBtnAbstrack();
	//}}AFX_MSG

	//	On activate / disactivate this page
	afx_msg void OnActivatePage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEEDITACTION_H__B40DF0A4_5677_4E80_9A34_A1E5E73611A2__INCLUDED_)
