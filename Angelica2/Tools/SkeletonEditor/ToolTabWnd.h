#if !defined(AFX_TOOLTABWND_H__8254C98C_3C2C_45FC_AF99_26E7E5A13724__INCLUDED_)
#define AFX_TOOLTABWND_H__8254C98C_3C2C_45FC_AF99_26E7E5A13724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTabWnd.h : header file
//

#include "PageEditHooks.h"
#include "PageEditAction.h"
#include "PageActionCombo.h"
#include "PageSuppleTest.h"
#include "PageScaleBone.h"

class CModel;

/////////////////////////////////////////////////////////////////////////////
// CToolTabWnd window

class CToolTabWnd : public CWnd
{
// Construction
public:
	
	enum
	{
		PAGE_EDITACTION = 0,
	//	PAGE_ACTIONCOMBO,
		PAGE_EDITHOOK,
		PAGE_SUPPLETEST,
		PAGE_SCALEBONE,
	};

	CToolTabWnd();

public:		//	Attributes

public:		//	Operations

	//	Release resources
	void Release();

	//	Bind model
	bool BindModel(CModel* pModel);
	//	Update bone selection
	void UpdateBoneSelection();
	//	Update skin information
	void UpdateSkinInfo();
	//	Update physique information
	void UpdatePhysiqueInfo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTabWnd)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CToolTabWnd();

protected:	//	Attributes

	CTabCtrl			m_TabCtrl;	
	CPageEditHooks		m_EditHookPage;
	CPageEditAction		m_EditActionPage;
	CPageActionCombo	m_ActionComboPage;
	CPageSuppleTest		m_SuppleTestPage;
	CPageScaleBone		m_ScaleBonePage;

	CDialog*		m_pCurPage;

protected:	//	Operations

	//	Change page
	void ChangePage();
	//	Resize current page
	void ResizeCurPage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTabWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	afx_msg void OnInputWndEnd(WPARAM wParam, LPARAM lParam);
	afx_msg void OnComboWndEnd(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTABWND_H__8254C98C_3C2C_45FC_AF99_26E7E5A13724__INCLUDED_)
