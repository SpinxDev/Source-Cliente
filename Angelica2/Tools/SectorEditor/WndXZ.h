/*
 * FILE: WndXZ.h
 *
 * DESCRIPTION: XZ orthogonal Viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_WNDXZ_H_
#define _WNDXZ_H_

#include "WndOrthogonal.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CXZWnd
//
///////////////////////////////////////////////////////////////////////////

class CXZWnd : public COrthogonalWnd
{
public:		//	Types

public:		//	Constructor and Destructor

	CXZWnd();
	virtual ~CXZWnd() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize viewport
	virtual bool Init(CRender* pRender, int x, int y, int iWid, int iHei);
	//	Release viewport
	virtual void Release();

	//	Render viewport
	virtual bool Render();

	//	Mouse action handler
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDbClk(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDbClk(int x, int y, DWORD dwFlags);

	//	Get camera's position represented by horizontal and vertical measures
	virtual void GetCameraPos(float* pfH, float* pfV);
	//	Set camera's position represented by horizontal and vertical measures
	virtual void SetCameraPos(float fH, float fV);

protected:	//	Attributes

protected:	//	Operations

	//	Draw coordinates characters
	virtual void DrawCoordinatesChar(int x1, int y1, int x2, int y2);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_WNDXZ_H_
