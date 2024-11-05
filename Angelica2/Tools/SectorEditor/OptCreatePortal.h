/*
 * FILE: OptCreatePortal.h
 *
 * DESCRIPTION: Create portal operation class
 *
 * CREATED BY: Duyuxin, 2003/7/24
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _OPTCREATEPORTAL_H_
#define _OPTCREATEPORTAL_H_

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
class COrthogonalWnd;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COptCreatePortal
//
///////////////////////////////////////////////////////////////////////////

class COptCreatePortal : public COperation
{
public:		//	Types

public:		//	Constructor and Destructor

	COptCreatePortal(int iID);
	virtual ~COptCreatePortal() {}

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

	CPortal* GetPortal() { return m_pPortal; }

protected:	//	Attributes

	CPortal*	m_pPortal;
	bool		m_bDrag;		//	Drag flag
	POINT		m_ptStart;		//	Start position
	A3DVECTOR3	m_vStart;

	COrthogonalWnd*	m_pViewWnd;

	int		m_iAxisH;		//	Three axis's index
	int		m_iAxisV;
	int		m_iAxisZ;

protected:	//	Operations

	void UpdatePortal(int x, int y);
	void BuildPortalVerts(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, A3DVECTOR3* aVerts);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_OPTCREATEPORTAL_H_



