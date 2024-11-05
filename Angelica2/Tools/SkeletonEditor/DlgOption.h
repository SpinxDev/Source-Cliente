#if !defined(AFX_DLGOPTION_H__2F05934B_3DF6_4F05_9757_B8ECB031B40B__INCLUDED_)
#define AFX_DLGOPTION_H__2F05934B_3DF6_4F05_9757_B8ECB031B40B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOption.h : header file
//

struct EDITCONFIG;

/////////////////////////////////////////////////////////////////////////////
// CDlgOption dialog

class CDlgOption : public CDialog
{
// Construction
public:
	CDlgOption(EDITCONFIG* pConfig, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgOption)
	enum { IDD = IDD_OPTION };
	float	m_fBoxSize;
	float	m_fStaffRadius;
	int		m_iCamMoveSpeed;
	int		m_iCamRotSpeed;
	int		m_iMouseDragSpeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	EDITCONFIG*		m_pConfig;

	// Generated message map functions
	//{{AFX_MSG(CDlgOption)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPTION_H__2F05934B_3DF6_4F05_9757_B8ECB031B40B__INCLUDED_)
