/*
 * FILE: A3DMathUtil.h
 *
 * DESCRIPTION: The math utility class for Angelica 3D Engine, which using table searching for
				many math calculations;
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMATHUTILITY_H_
#define _A3DMATHUTILITY_H_

#include "A3DTypes.h"

class A3DMathUtility
{
private:
	// The precision of sin and cos table, in degree defualt is 0.1;
	int				m_nTableLength;	  
	FLOAT			m_vPrecision;

	FLOAT			* m_pSinTable;
	FLOAT			* m_pCosTable;

protected:
public:
	A3DMathUtility();
	~A3DMathUtility();

	bool Init(int nTableLength=3600);
	bool Release();

	inline FLOAT SIN(FLOAT vDegree)
	{
		int nTableIndex = (int)(vDegree / m_vPrecision);
		nTableIndex = nTableIndex % m_nTableLength;
		if( nTableIndex < 0 ) nTableIndex += m_nTableLength;

		return m_pSinTable[nTableIndex];
	}
	inline FLOAT COS(FLOAT vDegree)
	{
		int nTableIndex = (int)(vDegree / m_vPrecision);
		nTableIndex = nTableIndex % m_nTableLength;
		if( nTableIndex < 0 ) nTableIndex += m_nTableLength;

		return m_pCosTable[nTableIndex];
	}
};

typedef A3DMathUtility * PA3DMathUtility;

extern A3DMathUtility * g_pA3DMathUtility;
#endif//_A3DMATHUTIL_H_


