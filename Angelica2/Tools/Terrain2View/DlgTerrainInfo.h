#if !defined(AFX_DLGTERRAININFO_H__545F0ECB_150F_4767_8DC4_688D8BCEFDCF__INCLUDED_)
#define AFX_DLGTERRAININFO_H__545F0ECB_150F_4767_8DC4_688D8BCEFDCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTerrainInfo.h : header file
//

class A3DTerrain2Wrapper;

/////////////////////////////////////////////////////////////////////////////
// CDlgTerrainInfo dialog

class CDlgTerrainInfo : public CDialog
{
// Construction
public:
	CDlgTerrainInfo(A3DTerrain2Wrapper* pTerrain, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTerrainInfo)
	enum { IDD = IDD_TERRAININFO };
	CListCtrl	m_InfoList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTerrainInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	A3DTerrain2Wrapper*		m_pTerrain;

	// Generated message map functions
	//{{AFX_MSG(CDlgTerrainInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTERRAININFO_H__545F0ECB_150F_4767_8DC4_688D8BCEFDCF__INCLUDED_)
