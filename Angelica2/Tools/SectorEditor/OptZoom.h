/*
 * FILE: OptZoom.h
 *
 * DESCRIPTION: Zoom operation class
 *
 * CREATED BY: Duyuxin, 2003/7/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _OPTZOOM_H_
#define _OPTZOOM_H_

#include "OptBase.h"
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

class COrthogonalWnd;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COptZoom
//
///////////////////////////////////////////////////////////////////////////

class COptZoom : public COperation
{
public:		//	Types

public:		//	Constructor and Destructor

	COptZoom(int iID);
	virtual ~COptZoom() {}

public:		//	Attributes

public:		//	Operations

	//	Set magnify flag
	void SetMagnifyFlag(bool bMagnify) { m_bMagnify = bMagnify; }

	//	Begin operation
	virtual bool Begin();
	//	End operation
	virtual bool End(bool bCancel);
	
	//	Tick routinue
	virtual bool FrameMove(DWORD dwTime);
	//	Render routinue
	virtual bool Render(CViewportWnd* pView);

	//	Mouse operations
	virtual void OnMouseMove(int x, int y, DWORD dwFlags);
	virtual void OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual void OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual void OnRButtonDown(int x, int y, DWORD dwFlags);

	//	Get magnify flag
	bool GetMagnifyFlag() { return m_bMagnify; }

protected:	//	Attributes

	bool		m_bMagnify;		//	true, magnify; false, minify
	bool		m_bDrag;		//	Drag flag
	A3DVECTOR3	m_vStart;
	A3DVECTOR3	m_vEnd;

	COrthogonalWnd*	m_pViewWnd;

	int		m_iAxisH;		//	Three axis's index
	int		m_iAxisV;
	int		m_iAxisZ;

protected:	//	Operations

	void UpdateSelection(int x, int y);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_OPTZOOM_H_



