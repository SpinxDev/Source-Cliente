#if !defined(AFX_PAGEACTIONCOMBO_H__C975C8D9_A4B2_44E3_9FCB_11BB6D559C59__INCLUDED_)
#define AFX_PAGEACTIONCOMBO_H__C975C8D9_A4B2_44E3_9FCB_11BB6D559C59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageActionCombo.h : header file
//

class A3DSkinModelAction;

/////////////////////////////////////////////////////////////////////////////
// CPageActionCombo dialog

class CPageActionCombo : public CDialog
{
// Construction
public:
	CPageActionCombo(CWnd* pParent = NULL);   // standard constructor

public:		//	Attributes

public:		//	Operations

	//	Release resources
	void Release();
	//	Bind model object with this dialog
	bool BindModel(CModel* pModel);

	//	Action stop callback
	void ActionStopCallback(A3DSkinModelActionCore* pAction);

// Dialog Data
	//{{AFX_DATA(CPageActionCombo)
	enum { IDD = IDD_ACTIONCOMBO };
	CComboBox	m_ActionComboA;
	CListBox	m_GroupList;
	CSliderCtrl	m_WeightSlider3;
	CSliderCtrl	m_WeightSlider2;
	CSliderCtrl	m_WeightSlider1;
	CComboBox	m_ActionCombo3;
	CComboBox	m_ActionCombo2;
	CComboBox	m_ActionCombo1;
	CString	m_strWeight1;
	CString	m_strWeight2;
	CString	m_strWeight3;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageActionCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	CModel*		m_pModel;
	bool		m_bBlendPlaying;
	bool		m_bGroupPlaying;

protected:	//	Operations

	//	Clear all information in dialog
	void ClearAllInfo();
	//	Update action list
	void UpdateActions(bool bModelChange);
	//	Get selected action of specified list
	A3DSkinModelActionCore* GetSelectedAction(CComboBox* pComboBox);

	//	Play blended actions
	void PlayBlendActions();
	//	Stop blended actions
	void StopBlendActions();
	//	Play group actions
	void PlayGroupActions();
	//	Stop group actions
	void StopGroupActions();

	// Generated message map functions
	//{{AFX_MSG(CPageActionCombo)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnPlay();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBtnAddaction();
	afx_msg void OnBtnRemoveaction();
	afx_msg void OnBtnMoveup();
	afx_msg void OnBtnMovedown();
	afx_msg void OnBtnGroupplay();
	afx_msg void OnBtnJumptonext();
	//}}AFX_MSG

	//	On activate / disactivate this page
	afx_msg void OnActivatePage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEACTIONCOMBO_H__C975C8D9_A4B2_44E3_9FCB_11BB6D559C59__INCLUDED_)
