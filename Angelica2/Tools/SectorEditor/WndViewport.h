/*
 * FILE: WndViewport.h
 *
 * DESCRIPTION: Viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_WNDVIEWPORT_H_
#define _WNDVIEWPORT_H_

#include "A3DTypes.h"

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

class CRender;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CViewportWnd
//
///////////////////////////////////////////////////////////////////////////

class CViewportWnd
{
public:		//	Types

	//	Mouse drag infomation
	struct MOUSEDRAG
	{
		bool	bDrag;		//	Drag flag
		int		iLastX;		//	Last x
		int		iLastY;		//	Last y
	};

public:		//	Constructor and Destructor

	CViewportWnd();
	virtual ~CViewportWnd() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize viewport
	virtual bool Init(CRender* pRender, int x, int y, int iWid, int iHei);
	//	Release viewport
	virtual void Release();

	//	Reset window.
	virtual bool Reset() { return true; }
	//	Frame move
	virtual bool FrameMove(DWORD dwTime) { return true; } 
	//	Render viewport
	virtual bool Render() { return true; }
	//	Move and resize viewport
	virtual bool Move(int x, int y, int iWid, int iHei);

	//	Mouse action handler, return true means action has been handled.
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDbClk(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDbClk(int x, int y, DWORD dwFlags);

	virtual void OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) {}
	virtual void OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) {}

	//	Draw active frame
	virtual void DrawActiveFrame();
	//	Transform
	virtual void Transform(A3DVECTOR3& vIn, A3DVECTOR3& vOut) {}
	//	Inverse transform
	virtual void InvTransform(A3DVECTOR3& vIn, A3DVECTOR3& vOut) {}

	//	Text out basing on viewport coordinates
	void TextOut(int x, int y, char* szText, DWORD color);

	//	Get position and size
	int GetX() { return m_x; }
	int GetY() { return m_y; }
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; } 

	//	Render flag
	void EnableRender(bool bRender) { m_bRender = bRender; }
	bool GetRenderFlag() { return m_bRender; }

	//	Active viewport
	void Activate(bool bActivate);
	bool IsActive() { return m_bActive; }

	//	Point in viewport ?
	bool PtInViewport(int x, int y)
	{
		if (x >= 0 && x < m_iWidth && y >= 0 && y < m_iHeight)
			return true;

		return false;
	}

protected:	//	Attributes

	CRender*	m_pRender;		//	Render object
	int			m_x;			//	Position of viewport in render window
	int			m_y;
	int			m_iWidth;		//	Viewport size
	int			m_iHeight;
	bool		m_bRender;		//	Render flag
	bool		m_bActive;		//	Active flag

	MOUSEDRAG	m_LeftDrag;		//	Mouse left button drag information
	MOUSEDRAG	m_RightDrag;	//	Mouse right button drag information

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_WNDVIEWPORT_H_
