/*
 * FILE: PhysObject.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/3/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _PHYSOBJECT_H_
#define _PHYSOBJECT_H_

#include <A3DVector.h>

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

class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CPhysObject
//	
///////////////////////////////////////////////////////////////////////////

class CPhysObject
{
public:		//	Types

	//	Physical state
	enum
	{
		PHY_STATE_PURESIMULATE = 0,		//	Pure physical simulation
		PHY_STATE_ANIMATION,			//	Pure model animation
		PHY_STATE_PARTSIMULATE,			//	Partial physical simulation
		NUM_PHY_STATE,
	};

public:		//	Constructor and Destructor

	CPhysObject();
	virtual ~CPhysObject();

public:		//	Attributes

public:		//	Operations

	//	Release object
	virtual void Release() {}
	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime) { return true; }
	//	Sync physical data to graphics data
	virtual bool Sync() { return true; }
	//	Render routine
	virtual bool Render(A3DViewport* pViewport) { return true; }
	//	Render proxy
	virtual bool RenderProxy(A3DViewport* pViewport) { return true; }

	//	Set position
	virtual void SetPos(const A3DVECTOR3& vPos) {}
	//	Set orientation
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) {}
	//	Setup object
	virtual bool Setup(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) { return true; }

	//	Change physical state
	virtual bool ChangePhysicalState(int iState) { return true; }

protected:	//	Attributes

	int		m_iPhysState;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_PHYSOBJECT_H_
