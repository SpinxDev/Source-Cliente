/*
 * FILE: OptSnapPortal.h
 *
 * DESCRIPTION: Snap portal to sector operation class
 *
 * CREATED BY: Duyuxin, 2003/7/30
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _OPTSNAPPORTAL_H_
#define _OPTSNAPPORTAL_H_

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

class CPortal;
class CSector;
class COrthogonalWnd;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COptSnapPortal
//
///////////////////////////////////////////////////////////////////////////

class COptSnapPortal : public COperation
{
public:		//	Types

public:		//	Constructor and Destructor

	COptSnapPortal(int iID);
	virtual ~COptSnapPortal() {}

public:		//	Attributes

public:		//	Operations

	void SetPortal(CPortal* pPortal) { m_pPortal = pPortal; }

	//	Begin operation
	virtual bool Begin();
	//	End operation
	virtual bool End(bool bCancel);
	
	//	Tick routinue
	virtual bool FrameMove(DWORD dwTime);
	//	Render routinue
	virtual bool Render(CViewportWnd* pView);

	//	Mouse operations
	virtual void OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual void OnRButtonDown(int x, int y, DWORD dwFlags);

protected:	//	Attributes

	CPortal*	m_pPortal;
	A3DVECTOR3	m_aOldVerts[4];		//	Portal's original vertices

	COrthogonalWnd*	m_pViewWnd;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_OPTSNAPPORTAL_H_



