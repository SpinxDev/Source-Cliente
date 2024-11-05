#if !defined(AFX_RENDERWND_H__D751057C_7D99_470E_82C2_7446A8982B72__INCLUDED_)
#define AFX_RENDERWND_H__D751057C_7D99_470E_82C2_7446A8982B72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenderWnd.h : header file
//
#include "StdAfx.h"
#include "RenderableObject.h"
#include "Render.h"

class A3DRenderTarget;

/////////////////////////////////////////////////////////////////////////////
// CRenderWnd window

class CRenderWnd : public CStatic
{
// Construction
public:
	CRenderWnd();
	
	bool InitRenderWnd();

	bool SetRenderableObject(CString& filepath);
	void BlankWnd();

// Attributes
public:

	CRenderableObject * m_pRenderableObject;

	A3DEngine*		m_pA3DEngine;
	A3DDevice*		m_pA3DDevice;
	A3DCamera*		m_pA3DCamera;
	A3DOrthoCamera* m_pA3DOrthoCamera;
	A3DViewport*	m_pA3DViewport;
	A3DRenderTarget* m_pRenderTarget;
#ifdef _ANGELICA22
	IDirect3DSurface9* m_pMemSurface;
#endif
	DWORD* m_dwBuffer;

	bool m_bOrtho;
	
	int m_iWidth;
	int m_iHeight;
	
	CString m_sizeinfo;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRenderWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRenderWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	
	bool InitEngine();
	void ReleaseEngine();
	
	void Render();
	void Tick();

private:

	A3DVECTOR3		m_CameraPos;
	A3DVECTOR3		m_CameraDir;
	A3DVECTOR3		m_CameraUp;
	A3DAABB			m_ObjectAABB;
	A3DVECTOR3		m_LookAt;
	float			m_OrigDis;

	BOOL m_LbuttonDown;
	CPoint m_OldMousePos;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDERWND_H__D751057C_7D99_470E_82C2_7446A8982B72__INCLUDED_)
