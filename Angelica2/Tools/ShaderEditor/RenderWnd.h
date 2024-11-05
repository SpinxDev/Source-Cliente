#if !defined(AFX_RENDERWND_H__205DAB6F_AEB3_4C6C_BB39_74FF333EEBD2__INCLUDED_)
#define AFX_RENDERWND_H__205DAB6F_AEB3_4C6C_BB39_74FF333EEBD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenderWnd.h : header file
//

#include "Decal.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define RENDERAREA_WIDTH	400
#define RENDERAREA_HEIGHT	400

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	CRenderWnd window
//
///////////////////////////////////////////////////////////////////////////

class CRenderWnd : public CWnd
{
// Construction
public:
	CRenderWnd();

// Attributes
public:

	bool		InitA3DEngine(HINSTANCE hInstance);		//	Initialize A3D engine
	void		ReleaseA3DEngine();		//	Release A3D engine

	void		ChangeShader(A3DShader* pShader);
	bool		Render();		//	Render window

	void		SetBackColor(DWORD dwCol)	{	m_dwBackCol = dwCol;	}
	DWORD		GetBackColor()				{	return m_dwBackCol;		}

	void		SetVertColor(DWORD dwCol)	{	m_dwVertCol = dwCol;	}
	DWORD		GetVertColor()				{	return m_dwVertCol;		}

	//	Change background texture
	bool ChangeBackgroundTexture(const char* szFile);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderWnd)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRenderWnd();

// Attributes
protected:

	CDecal		m_Decal;			//	Decal
	A3DShader*	m_pCurShader;		//	Current shader
	A3DTexture*	m_pBackTexture;		//	Background texture
	DWORD		m_dwBackCol;		//	Background color
	DWORD		m_dwVertCol;		//	Vertex color

// Operations
protected:

	bool		RenderBackground();	//	Render background

	// Generated message map functions
protected:
	//{{AFX_MSG(CRenderWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDERWND_H__205DAB6F_AEB3_4C6C_BB39_74FF333EEBD2__INCLUDED_)
