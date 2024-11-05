/*
 * FILE: WndPerspective.h
 *
 * DESCRIPTION: Perspective Viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_WNDPERSPECTIVE_H_
#define _WNDPERSPECTIVE_H_

#include "WndViewport.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DViewport;
class A3DCamera;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CPerspectiveWnd
//
///////////////////////////////////////////////////////////////////////////

class CPerspectiveWnd : public CViewportWnd
{
public:		//	Types

public:		//	Constructor and Destructor

	CPerspectiveWnd();
	virtual ~CPerspectiveWnd() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize viewport
	virtual bool Init(CRender* pRender, int x, int y, int iWid, int iHei);
	//	Release viewport
	virtual void Release();

	//	Reset window.
	virtual bool Reset();
	//	Frame move
	virtual bool FrameMove(DWORD dwTime);
	//	Render viewport
	virtual bool Render();
	//	Move and resize viewport
	virtual bool Move(int x, int y, int iWid, int iHei);

	//	Mouse action handler
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDbClk(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDbClk(int x, int y, DWORD dwFlags);

	//	Get camera
	A3DCamera* GetCamera() { return m_pCamera; }
	//	Get viewport
	A3DViewport* GetViewport() { return m_pViewport; }

protected:	//	Attributes

	A3DCamera*		m_pCamera;
	A3DViewport*	m_pViewport;

protected:	//	Operations

	//	Move camera
	void MoveCamera(DWORD dwTime);
	//	Rotate camera
	void RotateCamera(DWORD dwTime);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_WNDPERSPECTIVE_H_
