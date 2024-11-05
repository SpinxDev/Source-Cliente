/*
 * FILE: Render.h
 *
 * DESCRIPTION: Render class
 *
 * CREATED BY: Duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_RENDER_H_
#define _RENDER_H_

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Pre-created text strings
enum
{
	PSTR_X = 0,		//	'X'
	PSTR_Y,			//	'Y'
	PSTR_Z,			//	'Z'
	PSTR_GRID,		//	'Grid'
	PSTR_100,		//	100.0
	PSTR_10,		//	10.0
	PSTR_1,			//	1.0
	PSTR_01,		//	0.1
	PSTR_005,		//	0.05
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DDevice;	
class A3DLight;	
class A3DFont;
class A3DStringSet;
class CFlatCollector;
class CWireCollector;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CRender
//
///////////////////////////////////////////////////////////////////////////

class CRender
{
public:		//	Types

public:		//	Constructor and Destructor

	CRender();
	virtual ~CRender() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize game
	bool Init(HINSTANCE hInst, HWND hWnd);
	//	Release game
	void Release();

	//	Resize device
	bool ResizeDevice(int iWid, int iHei);
	//	Begin render
	bool BeginRender();
	//	End render
	bool EndRender();

	//	Output text
	void TextOut(int x, int y, char* szText, DWORD color);
	//	Output pre-created text
	void TextOut(int iText, int x, int y, DWORD color);
	//	Flush pre-created text
	void FlushPreText();

	//	Draw 2D rectangle
	void Draw2DRect(const RECT& rc, DWORD dwCol, bool bFrame);

	//	Get interface
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() {	return m_pA3DDevice; }
	A3DLight* GetDirectionalLight() { return m_pDirLight; }
	
	//	Engine is ready ?
	bool EngineIsReady() { return m_bEngineOK; }

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;		//	A3D engine
	A3DDevice*		m_pA3DDevice;		//	A3D device
	A3DLight*		m_pDirLight;		//	Directional light
	A3DFont*		m_pFont;			//	Font object
	A3DStringSet*	m_pStrings;			//	Pre-created strings

	HINSTANCE		m_hInstance;		//	Handle of instance
	HWND			m_hRenderWnd;		//	Render window's handle
	int				m_iRenderWid;		//	Render size
	int				m_iRenderHei;

	bool			m_bEngineOK;		//	Engine is ready

protected:	//	Operations

	//	Initialize A3D engine
	bool InitA3DEngine();
	//	Release A3D eigine
	void ReleaseA3DEngine();
	//	Create pre-created text string
	bool CreatePreText(DWORD hFont);
};

extern CRender	g_Render;

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

#endif	//	_RENDER_H_
