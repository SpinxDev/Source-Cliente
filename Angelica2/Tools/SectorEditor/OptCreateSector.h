/*
 * FILE: OptCreateSector.h
 *
 * DESCRIPTION: Create sector operation class
 *
 * CREATED BY: Duyuxin, 2003/7/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _OPTCREATESECTOR_H_
#define _OPTCREATESECTOR_H_

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

class CSector;
class COrthogonalWnd;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COptCreateSector
//
///////////////////////////////////////////////////////////////////////////

class COptCreateSector : public COperation
{
public:		//	Types

public:		//	Constructor and Destructor

	COptCreateSector(int iID);
	virtual ~COptCreateSector() {}

public:		//	Attributes

public:		//	Operations

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

	CSector* GetSector() { return m_pSector; }

protected:	//	Attributes

	CSector*	m_pSector;
	bool		m_bDrag;		//	Drag flag
	POINT		m_ptStart;		//	Start position
	A3DVECTOR3	m_vStart;

	COrthogonalWnd*	m_pViewWnd;

	int		m_iAxisH;		//	Three axis's index
	int		m_iAxisV;
	int		m_iAxisZ;

protected:	//	Operations

	void UpdateSector(int x, int y);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_OPTCREATESECTOR_H_



