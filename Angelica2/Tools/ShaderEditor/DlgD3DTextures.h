#if !defined(AFX_DLGD3DTEXTURES_H__6C2E260F_28F7_4A7F_9167_F669287F1C79__INCLUDED_)
#define AFX_DLGD3DTEXTURES_H__6C2E260F_28F7_4A7F_9167_F669287F1C79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgD3DTextures.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgD3DTextures dialog

class CDlgD3DTextures : public CDialog
{
// Construction
public:
	CDlgD3DTextures(CWnd* pParent = NULL);   // standard constructor

	//	Get currently selected texture slot
	int GetCurSelSlot() { return m_iCurSelSlot; }
	void SetCurSelSlot(int iSlot) { m_iCurSelSlot = iSlot; }

// Dialog Data
	//{{AFX_DATA(CDlgD3DTextures)
	enum { IDD = IDD_D3DTEXTURES };
	CListCtrl	m_TexList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgD3DTextures)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int		m_iCurSelSlot;

	// Generated message map functions
	//{{AFX_MSG(CDlgD3DTextures)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGD3DTEXTURES_H__6C2E260F_28F7_4A7F_9167_F669287F1C79__INCLUDED_)
