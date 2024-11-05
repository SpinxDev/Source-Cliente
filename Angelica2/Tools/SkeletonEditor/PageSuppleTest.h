#if !defined(AFX_PAGESUPPLETEST_H__649FC4D1_269E_468C_99A6_2DA7F166F37E__INCLUDED_)
#define AFX_PAGESUPPLETEST_H__649FC4D1_269E_468C_99A6_2DA7F166F37E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageSuppleTest.h : header file
//

class CModel;

/////////////////////////////////////////////////////////////////////////////
// CPageSuppleTest dialog

class CPageSuppleTest : public CDialog
{
// Construction
public:
	CPageSuppleTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPageSuppleTest)
	enum { IDD = IDD_SUPPLETEST };
	CListBox	m_PhyShapeList;
	CSpinButtonCtrl	m_ZSpin;
	CSpinButtonCtrl	m_YSpin;
	CSpinButtonCtrl	m_XSpin;
	CSpinButtonCtrl	m_RadiusSpin;
	CListBox	m_SuppleMeshList;
	CSpinButtonCtrl	m_LengthSpin;
	CString	m_strWarning;
	CString	m_strCoefficient;
	CString	m_strAirResist;
	CString	m_strGravity;
	CString	m_strVertWeight;
	CString	m_strLength;
	CString	m_strRadius;
	CString	m_strX;
	CString	m_strY;
	CString	m_strZ;
	//}}AFX_DATA

public:		//	Attributes

public:		//	Operations

	//	Release resources
	void Release();
	//	Bind model object with this dialog
	bool BindModel(CModel* pModel);
	//	Update skin information
	void UpdateSkinInfo();
	//	Update physique information
	void UpdatePhysiqueInfo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageSuppleTest)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	CModel*		m_pModel;

protected:	//	Operations

	//	Clear all information in dialog
	void ClearAllInfo();
	//	Update supple mesh list
	void UpdateSuppleMeshList();
	//	Update physique shape list
	void UpdatePhysiqueShapeList();

	//	Update supple data
	void UpdateSuppleData(int iItem, bool bSave);
	//	Update physique shape data
	void UpdatePhysiqueShapeData(int iItem, bool bSave);
	//	Handle key RETURN down
	void OnReturnDown(HWND hWnd);
	//	Accept float value
	float AcceptFloatValue(int iControlID, CString& strValue, float fFloor, float fRoof);
	//	Spin float value
	float SpinFloatValue(NM_UPDOWN* pNMUpDown, int iControlID, CString& strValue, float fStep, float fFloor, float fRoof);

	// Generated message map functions
	//{{AFX_MSG(CPageSuppleTest)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListPhyshape();
	afx_msg void OnSelchangeListSupplemesh();
	afx_msg void OnDeltaposSpinX(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinY(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinZ(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLength(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinRadius(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	//	On activate / disactivate this page
	afx_msg void OnActivatePage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESUPPLETEST_H__649FC4D1_269E_468C_99A6_2DA7F166F37E__INCLUDED_)
