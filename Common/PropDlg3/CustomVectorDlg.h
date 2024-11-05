#if !defined(AFX_CUSTOMVECTORDLG_H__BFAB392E_A99B_4D46_9DA1_CF4D1072A960__INCLUDED_)
#define AFX_CUSTOMVECTORDLG_H__BFAB392E_A99B_4D46_9DA1_CF4D1072A960__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomVectorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomVectorDlg dialog

class A3DVECTOR3;
class CCustomVectorDlg : public CDialog
{
// Construction
public:
	//@param valueArr: size == dimension
	CCustomVectorDlg(size_t dimension, float valueArr[], CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomVectorDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	size_t m_dimension;
	float m_valueArr[4];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomVectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomVectorDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMVECTORDLG_H__BFAB392E_A99B_4D46_9DA1_CF4D1072A960__INCLUDED_)
