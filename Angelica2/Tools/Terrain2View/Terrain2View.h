// Terrain2View.h : main header file for the MFCTEMP application
//

#if !defined(AFX_TERRAIN2VIEW_H__914F10B0_0D7C_4021_97FF_9538E788B3F1__INCLUDED_)
#define AFX_TERRAIN2VIEW_H__914F10B0_0D7C_4021_97FF_9538E788B3F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Settings.h"

class CMainFrame;

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewApp:
// See Terrain2View.cpp for the implementation of this class
//

class CTerrain2ViewApp : public CWinApp
{
public:
	CTerrain2ViewApp();

	//	Get main frame handle
	CMainFrame* GetMainFrame() { return (CMainFrame*)m_pMainWnd; }
	//	Get settings
	CSettings& GetSettings() { return m_Settings; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrain2ViewApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTerrain2ViewApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnViewBackcol();
	afx_msg void OnViewWireframe();
	afx_msg void OnUpdateViewWireframe(CCmdUI* pCmdUI);
	afx_msg void OnViewOcclusionline();
	afx_msg void OnUpdateViewOcclusionline(CCmdUI* pCmdUI);
	afx_msg void OnViewRendertrn();
	afx_msg void OnUpdateViewRendertrn(CCmdUI* pCmdUI);
	afx_msg void OnViewCameraspeed();
	afx_msg void OnViewUsehoriline();
	afx_msg void OnUpdateViewUsehoriline(CCmdUI* pCmdUI);
	afx_msg void OnViewBqOcclusion();
	afx_msg void OnUpdateViewBqOcclusion(CCmdUI* pCmdUI);
	afx_msg void OnViewBqTest();
	afx_msg void OnUpdateViewBqTest(CCmdUI* pCmdUI);
	afx_msg void OnViewBqHighlight();
	afx_msg void OnUpdateViewBqHighlight(CCmdUI* pCmdUI);
	afx_msg void OnViewPrompt();
	afx_msg void OnUpdateViewPrompt(CCmdUI* pCmdUI);
	afx_msg void OnViewCamerapos();
	afx_msg void OnViewViewradius();
	afx_msg void OnViewTerrainlod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	CSettings	m_Settings;

protected:

	//	Do some initial work
	bool InitApp();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAIN2VIEW_H__914F10B0_0D7C_4021_97FF_9538E788B3F1__INCLUDED_)
