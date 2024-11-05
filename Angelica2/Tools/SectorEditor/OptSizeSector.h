/*
 * FILE: OptSizeSector.h
 *
 * DESCRIPTION: Size sector operation class
 *
 * CREATED BY: Duyuxin, 2003/7/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _OPTSIZESECTOR_H_
#define _OPTSIZESECTOR_H_

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
//	Class COptSizeSector
//
///////////////////////////////////////////////////////////////////////////

class COptSizeSector : public COperation
{
public:		//	Types

public:		//	Constructor and Destructor

	COptSizeSector(int iID);
	virtual ~COptSizeSector() {}

public:		//	Attributes

public:		//	Operations

	void SetSector(CSector* pSector) { m_pSector = pSector; }

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

	CSector* GetSector() { return m_pSector; }

protected:	//	Attributes

	CSector*	m_pSector;
	bool		m_bDrag;		//	Drag flag
	POINT		m_ptStart;		//	Start position
	int			m_iDragSide;	//	Drag side
	A3DVECTOR3	m_vStart;

	A3DVECTOR3	m_vOldMins;		//	Sector's old position
	A3DVECTOR3	m_vOldMaxs;

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


#endif	//	_OPTSIZESECTOR_H_



