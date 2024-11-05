#if !defined(AFX_DLGTERRAINLOD_H__DD88721C_98B5_42C4_9680_5E05D0D114AE__INCLUDED_)
#define AFX_DLGTERRAINLOD_H__DD88721C_98B5_42C4_9680_5E05D0D114AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTerrainLOD.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTerrainLOD dialog

class CDlgTerrainLOD : public CDialog
{
// Construction
public:
	CDlgTerrainLOD(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTerrainLOD)
	enum { IDD = IDD_TERRAINLOD };
	float	m_fDist1;
	float	m_fDist2;
	int		m_iLODType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTerrainLOD)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTerrainLOD)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTERRAINLOD_H__DD88721C_98B5_42C4_9680_5E05D0D114AE__INCLUDED_)
