/*
 * FILE: OptLinkSector.h
 *
 * DESCRIPTION: Link sector with portal operation class
 *
 * CREATED BY: Duyuxin, 2003/7/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _OPTLINKSECTOR_H_
#define _OPTLINKSECTOR_H_

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
//	Class COptLinkSector
//
///////////////////////////////////////////////////////////////////////////

class COptLinkSector : public COperation
{
public:		//	Types

public:		//	Constructor and Destructor

	COptLinkSector(int iID);
	virtual ~COptLinkSector() {}

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
	CSector*	m_aSectors[2];
	int			m_aLinkModes[2];

	COrthogonalWnd*	m_pViewWnd;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_OPTLINKSECTOR_H_



