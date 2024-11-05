// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__C975837B_FE54_4661_AA90_F4C57AF5513C__INCLUDED_)
#define AFX_CHILDVIEW_H__C975837B_FE54_4661_AA90_F4C57AF5513C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChildView window

namespace AutoMove
{
	class CLayerMap;
}

class CGridRender;

class CChildView : public CWnd
{
public:
	struct Stat 
	{
		DWORD t_setup;
		DWORD t_search;
		DWORD t_optimize;
	};
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();
	
	bool Render();
	void Tick(DWORD dwTickTime);
	bool LoadMapCfg(const char * szPath, bool bConnectAndSave = false);
	bool LoadMapBin(const char * szPath);

	bool ViewLayer(int iLayer);
	AutoMove::CLayerMap * GetCurLayer() 
	{
		return m_pCurLayer;
	}

	void Save();

	void CreateClu();
	CGridRender * GetGridRender() const
	{
		return m_pGrid;
	}
	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdatePane (CCmdUI *pCmdUI);
	afx_msg void OnSetStart();
	afx_msg void OnSetGoal();
	afx_msg void OnSearch();
	afx_msg void OnMove();
	afx_msg void OnAlgCluG();
	afx_msg void OnUpdateAlgCluG(CCmdUI* pCmdUI);
	afx_msg void OnAlgCluMa();
	afx_msg void OnUpdateAlgCluMa(CCmdUI* pCmdUI);
	afx_msg void OnAlgCluMh();
	afx_msg void OnUpdateAlgCluMh(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void _ReleaseGame();
	void _DrawPath();
	bool _SaveTxt();
	bool _SaveBin();

private:
	AutoMove::CLayerMap *     m_pCurLayer;
	int						  m_iCurLayer;
	CGridRender *			  m_pGrid;
	Stat					  m_Stat;
	int						  m_CurMoveIndex;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__C975837B_FE54_4661_AA90_F4C57AF5513C__INCLUDED_)
