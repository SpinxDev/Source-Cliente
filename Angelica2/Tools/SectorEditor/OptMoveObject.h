/*
 * FILE: OptMoveObject.h
 *
 * DESCRIPTION: Move object operation class
 *
 * CREATED BY: Duyuxin, 2003/7/25
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _OPTMOVEOBJECT_H_
#define _OPTMOVEOBJECT_H_

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

class CEditObject;
class COrthogonalWnd;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COptMoveObject
//
///////////////////////////////////////////////////////////////////////////

class COptMoveObject : public COperation
{
public:		//	Types

public:		//	Constructor and Destructor

	COptMoveObject(int iID);
	virtual ~COptMoveObject() {}

public:		//	Attributes

public:		//	Operations

	void SetObject(CEditObject* pObject) { m_pObject = pObject; }

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

protected:	//	Attributes

	CEditObject*	m_pObject;

	bool		m_bDrag;		//	Drag flag
	POINT		m_ptLast;		//	Last position
	A3DVECTOR3	m_vLast;		

	A3DVECTOR3	m_vOldMins;		//	Sector's old position
	A3DVECTOR3	m_vOldMaxs;
	A3DVECTOR3	m_aOldVerts[4];	//	Portal's original vertices

	COrthogonalWnd*	m_pViewWnd;

	int		m_iAxisH;		//	Three axis's index
	int		m_iAxisV;
	int		m_iAxisZ;

protected:	//	Operations

	//	Update object's position
	void UpdateObject(int x, int y);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_OPTMOVEOBJECT_H_



