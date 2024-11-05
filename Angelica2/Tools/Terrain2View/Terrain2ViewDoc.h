// Terrain2ViewDoc.h : interface of the CTerrain2ViewDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRAIN2VIEWDOC_H__A2D8998C_D4E6_4B3E_90DC_A1A6B24C92DC__INCLUDED_)
#define AFX_TERRAIN2VIEWDOC_H__A2D8998C_D4E6_4B3E_90DC_A1A6B24C92DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class A3DTerrain2Wrapper;

class CTerrain2ViewDoc : public CDocument
{
protected: // create from serialization only
	CTerrain2ViewDoc();
	DECLARE_DYNCREATE(CTerrain2ViewDoc)

// Attributes
public:

	friend class CTerrain2ViewView;
	friend class CSettings;

	//	Get terrain object
	A3DTerrain2Wrapper* GetTerrain() { return m_pA3DTerrain; }
	//	Get terrain file
	const char* GetTerrainFile() { return m_strTrnFile; }

// Operations
public:

	//	Logic tick
	bool Tick(DWORD dwTime);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrain2ViewDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTerrain2ViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	A3DTerrain2Wrapper*	m_pA3DTerrain;

	CString		m_strTrnFile;	//	Current terrain file

protected:

	//	Load terrain
	bool LoadTerrain(const char* szFile);

// Generated message map functions
protected:
	//{{AFX_MSG(CTerrain2ViewDoc)
	afx_msg void OnViewTrninfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAIN2VIEWDOC_H__A2D8998C_D4E6_4B3E_90DC_A1A6B24C92DC__INCLUDED_)
