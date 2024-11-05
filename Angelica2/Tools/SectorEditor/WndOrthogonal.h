/*
 * FILE: WndOrthogonal.h
 *
 * DESCRIPTION: Orthogonal viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_WNDORTHOGONAL_H_
#define _WNDORTHOGONAL_H_

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

class COrthoCamera;
class COrthoViewport;
class COrthoGrid;
class CSector;
class CPortal;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COrthogonalWnd
//
///////////////////////////////////////////////////////////////////////////

class COrthogonalWnd : public CViewportWnd
{
public:		//	Types

public:		//	Constructor and Destructor

	COrthogonalWnd();
	virtual ~COrthogonalWnd() {}

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

	//	Mouse action handler, return true means action has been handled.
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags);

	//	Transform
	virtual void Transform(A3DVECTOR3& vIn, A3DVECTOR3& vOut);
	//	Inverse transform
	virtual void InvTransform(A3DVECTOR3& vIn, A3DVECTOR3& vOut);

	//	Get camera's position represented by horizontal and vertical measures
	virtual void GetCameraPos(float* pfH, float* pfV) { *pfH = *pfV = 0.0f; }
	//	Set camera's position represented by horizontal and vertical measures
	virtual void SetCameraPos(float fH, float fV) {}

	//	Zoom viewport
	void Zoom(float fMultiply);
	void Zoom(bool bMagnify, float l, float r, float b, float t);

	//	Pick sector
	CSector* PickSector(int x, int y, int* piSide);
	//	Click sector
	int ClickSector(int x, int y, CSector* pSector);
	//	Pick portal
	CPortal* PickPortal(int x, int y, int* piSide);
	//	Click portal
	int ClickPortal(int x, int y, CPortal* pPortal);

	//	Get camera
	COrthoCamera* GetCamera() { return m_pCamera; }
	//	Get viewport
	COrthoViewport* GetViewport() { return m_pViewport; }
	//	Get grid
	COrthoGrid* GetGrid() { return m_pGrid; }

protected:	//	Attributes

	COrthoCamera*		m_pCamera;
	COrthoViewport*		m_pViewport;
	COrthoGrid*			m_pGrid;

protected:	//	Operations

	//	Adjust grid
	void AdjustGrid();
	//	Move grid
	void MoveGrid(float fOffH, float fOffV);
	//	Move camera
	void MoveCamera(DWORD dwTime);
	//	Zoom
	void Zoom(DWORD dwTime);

	//	Draw coordinates
	void DrawCoordinates();
	//	Draw grid size
	void DrawGridSize(float fSize);
	//	Draw Eye's position and direction
	void DrawEyePosAndDir();

	//	Draw coordinates characters
	virtual void DrawCoordinatesChar(int x1, int y1, int x2, int y2) {}
	//	On render
	virtual void OnRender() {}
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_WNDORTHOGONAL_H_
