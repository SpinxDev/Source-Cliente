#if !defined(AFX_DLGPORTALSECTOR_H__45103F67_394F_45F2_9F4D_91EE5170B696__INCLUDED_)
#define AFX_DLGPORTALSECTOR_H__45103F67_394F_45F2_9F4D_91EE5170B696__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPortalSector.h : header file
//

class CSectorEditorDoc;
class CSector;
class CEditObject;

/////////////////////////////////////////////////////////////////////////////
// CDlgPortalSector dialog

class CDlgPortalSector : public CDialog
{
// Construction
public:
	CDlgPortalSector(CSectorEditorDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPortalSector)
	enum { IDD = IDD_PORTALSECTOR };
	CListCtrl	m_SectorList;
	CListCtrl	m_PortalList;
	CButton	m_strSectors;
	CButton	m_strPortals;
	//}}AFX_DATA

	CEditObject* GetSelObject() { return m_pSelObject; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPortalSector)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CSectorEditorDoc*	m_pDoc;
	CEditObject*		m_pSelObject;

	//	Initialize portal list
	void InitPortalList();
	//	Initialize sector list
	void InitSectorList();
	//	Update portal list
	void UpdatePortalList(bool bInit);
	//	Update sector list
	void UpdateSectorList(bool bInit);
	//	Get associated portals of specified sector
	void GetLinkedPortals(CSector* pSector, char* szText);
	//	Get selected object
	CEditObject* GetSelectedObject(int* piID, int* piIndex);
	//	Remove selection in list
	void RemoveSelection(bool bPortalList);

	// Generated message map functions
	//{{AFX_MSG(CDlgPortalSector)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtnDelete();
	afx_msg void OnItemchangedListPortals(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSectors(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPORTALSECTOR_H__45103F67_394F_45F2_9F4D_91EE5170B696__INCLUDED_)
