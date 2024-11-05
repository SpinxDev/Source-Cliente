// CWButton.h : header file
//

#if !defined(CWBUTTON_H_INCLUDED)
#define CWBUTTON_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCWButton class

class CCWButton : public CButton
{
	DECLARE_DYNAMIC(CCWButton)

// Construction
public:
	CCWButton();

// Attributes
protected:
	CSize       m_cSize;
	CRect		m_rcItem;
	CRect		m_rcIcon;
	UINT		m_nState;
	bool		m_bLBtnDown;
	bool		m_bFlatLook;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCWButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void SetIcon(HICON hIcon,CSize cSize);
	//}}AFX_VIRTUAL

// Implementation
public:
	void DisableFlatLook() { m_bFlatLook = false; }
	void SetIconRect();
	virtual ~CCWButton();

// Generated message map functions
protected:
	//{{AFX_MSG(CCWButton)
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CWBUTTON_H_INCLUDED)

