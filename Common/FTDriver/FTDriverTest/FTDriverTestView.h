// FTDriverTestView.h : interface of the CFTDriverTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTDRIVERTESTVIEW_H__A938400D_2D1C_4117_BEE0_97D7BD67330A__INCLUDED_)
#define AFX_FTDRIVERTESTVIEW_H__A938400D_2D1C_4117_BEE0_97D7BD67330A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IFTManager;
class IFTFont;

class CFTDriverTestView : public CView
{
	IFTManager * m_pManager;
	IFTFont * m_pFontFT;
	IFTFont * m_pFontWT;
	bool m_bEnterDowned;
	CString m_strText;
	BYTE * m_pBuffer;
	DWORD m_dwFontFileLength;
protected: // create from serialization only
	CFTDriverTestView();
	DECLARE_DYNCREATE(CFTDriverTestView)

// Attributes
public:
	CFTDriverTestDoc* GetDocument();
	bool GetFaceNameTTF(const char *pFile,wchar_t * face);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFTDriverTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	wchar_t * ANSIToUNICODE(const char *strANSI);
	virtual ~CFTDriverTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	static unsigned __stdcall MultiThreadTestFunc(void * pData);
// Generated message map functions
protected:
	//{{AFX_MSG(CFTDriverTestView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTestMultiThread();
	afx_msg void OnSetFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FTDriverTestView.cpp
inline CFTDriverTestDoc* CFTDriverTestView::GetDocument()
   { return (CFTDriverTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTDRIVERTESTVIEW_H__A938400D_2D1C_4117_BEE0_97D7BD67330A__INCLUDED_)
