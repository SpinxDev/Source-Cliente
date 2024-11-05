#if !defined(AFX_DLGPHYSXTRNGEN_H__86675EC6_FF1D_4149_B1A7_2E1C87484655__INCLUDED_)
#define AFX_DLGPHYSXTRNGEN_H__86675EC6_FF1D_4149_B1A7_2E1C87484655__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPhysXTrnGen.h : header file
//

class A3DTerrain2;
class A3DTerrain2Block;

/////////////////////////////////////////////////////////////////////////////
// CDlgPhysXTrnGen dialog

class CDlgPhysXTrnGen : public CDialog
{
// Construction
public:
	CDlgPhysXTrnGen(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPhysXTrnGen)
	enum { IDD = IDD_TRNBUILDER };
	CProgressCtrl	m_Progress;
	CEdit	m_TrnInfoEdit;
	CString	m_strTrnFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPhysXTrnGen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	A3DTerrain2*	m_pTerrain;		//	Terrain object
	float*			m_pHeightMap;	//	Height map buffer

protected:

	//	Load terrain
	bool LoadTerrain(const char* szFile);
	//	Print terrain info.
	void PrintTerrainInfo();
	//	Insert info text
	void InsertInfoText(const char* szText);

	//	Build terrain PhysX data
	bool BuildTerrainPhysXData();
	//	Build physX terrain data for specified sub-terrain
	bool BuildSubTerrainPhysXData(int r, int c, int iSubIndex);
	//	Fill block height to height map buffer
	void FillBlockHeight(A3DTerrain2Block* pBlock, int br, int bc);
	//	Save physX terrain configs
	bool SavePhysXDataConfigs();
	//	Save physX terrain data for specified sub-terrain
	bool SaveSubTerrainPhysXData(int iSubIndex, float fMinHei, float fMaxHei);

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPhysXTrnGen)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtnBrowse();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPHYSXTRNGEN_H__86675EC6_FF1D_4149_B1A7_2E1C87484655__INCLUDED_)
