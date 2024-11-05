#if !defined(AFX_WNDCOMBO_H__728C24FA_8D87_4352_ADEF_984C5AA3F2E1__INCLUDED_)
#define AFX_WNDCOMBO_H__728C24FA_8D87_4352_ADEF_984C5AA3F2E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndCombo window

class CWndCombo : public CWnd
{
public:		//	Types

	enum
	{
		ID_COMBOWINDOW = 101,	//	Edit window ID
	};

// Construction
public:
	CWndCombo();

public:		//	Attributes

public:		//	Operations

	//	Add a new string
	int InsertString(int iIndex, const char* szStr) { return m_ComboWnd.InsertString(iIndex, szStr); }
	//	Delete string
	void DeleteString(const char* szStr);
	//	Get string number
	int GetStringNum() { m_ComboWnd.GetCount(); }
	//	Clear all combo strings
	void ClearAllStrings() { m_ComboWnd.ResetContent(); }

	//	Popup window, begin to input data
	bool Popup(int iOwnWndID, int iSelected, const RECT& rc);
	//	Close window, end inputing data
	void Close();

	//	Get owner window's ID
	int GetOwnerWindowID() { return m_iOwnWndID; }

	int GetSelectedIndex() { return m_iSelected; }
	CString& GetSelectedString() { return m_strSelected; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndCombo)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWndCombo();

protected:	//	Attributes

	CComboBox	m_ComboWnd;
	CFont		m_Font;
	int			m_iInitSel;		//	Initial selection
	int			m_iSelected;	//	Selected index
	CString		m_strSelected;	//	Selected string
	int			m_iOwnWndID;	//	ID of own window

	bool		m_bSelecting;	//	Selecting flag

protected:	//	Operations

	void EndSelecting(bool bCancel, bool bClickEnd, const POINT* pPoint);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndCombo)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDCOMBO_H__728C24FA_8D87_4352_ADEF_984C5AA3F2E1__INCLUDED_)
