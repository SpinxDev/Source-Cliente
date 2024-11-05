/*
 * FILE: Bound.h
 *
 * DESCRIPTION: Bound box class
 *
 * CREATED BY: Duyuxin, 2003/7/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_BOUND_H_
#define _BOUND_H_

#include "Box3D.h"

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

class CRender;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CBound
//
///////////////////////////////////////////////////////////////////////////

class CBound
{
public:		//	Types

public:		//	Constructor and Destructor

	CBound();
	virtual ~CBound() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize sector
	bool Init(CRender* pRender, const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);
	//	Release sector
	void Release();

	//	Render sector
	bool Render();

	A3DVECTOR3 GetMins() { return m_Box3D.GetMins(); }
	A3DVECTOR3 GetMaxs() { return m_Box3D.GetMaxs(); }

	//	Resize sector
	void SetMinsAndMaxs(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
	{
		m_Box3D.SetMinsAndMaxs(vMins, vMaxs);
	}

	void SetBound(int iAxis, bool bMin, float fVal);

	//	Set/Get color
	void SetColor(DWORD dwCol) { m_Box3D.SetColor(dwCol); }
	DWORD GetColor() { return m_Box3D.GetColor(); }

protected:	//	Attributes

	CBox3D		m_Box3D;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_BOUND_H_
