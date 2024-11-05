/*
 * FILE: Box3D.h
 *
 * DESCRIPTION: 3D box class
 *
 * CREATED BY: Duyuxin, 2003/7/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_BOX3D_H_
#define _BOX3D_H_

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

class CRender;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CBox3D
//
///////////////////////////////////////////////////////////////////////////

class CBox3D
{
public:		//	Types

public:		//	Constructor and Destructor

	CBox3D();
	virtual ~CBox3D() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize box
	bool Init(CRender* pRender, const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);
	//	Release box
	void Release();

	//	Render sector
	bool Render();

	A3DVECTOR3 GetMins() { return m_vMins; }
	A3DVECTOR3 GetMaxs() { return m_vMaxs; }

	//	Resize box
	void SetMinsAndMaxs(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);

	//	Set/Get color
	void SetColor(DWORD dwCol) { m_dwColor = dwCol; }
	DWORD GetColor() { return m_dwColor; }

protected:	//	Attributes

	CRender*	m_pRender;		//	Render object

	A3DVECTOR3	m_vMins;		//	Box volume
	A3DVECTOR3	m_vMaxs;
	DWORD		m_dwColor;		//	Box's color

	static WORD	m_aIndices[24];	//	Indices

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_BOX3D_H_
