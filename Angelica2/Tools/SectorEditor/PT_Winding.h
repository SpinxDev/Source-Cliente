/*
 * FILE: PT_Winding.h
 *
 * DESCRIPTION: Declarations for winding functions
 *
 * CREATED BY: Duyuxin, 2003/6/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_PT_WINDING_H_
#define _PT_WINDING_H_

#include "PT_Internal.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

//	Maximum number of vertices in a winding
#define MAXNUM_WINDINGVERT		128

/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////

namespace PORTAL
{
	s_WINDING* AllocWinding(int iNumPts);
	void DestroyWinding(s_WINDING* pWinding);
	s_WINDING* CopyWinding(s_WINDING* pWinding);
	s_WINDING* ReverseWinding(s_WINDING* pWinding);
	int WindingOnPlaneSide(s_WINDING* pWinding, A3DVECTOR3& vNormal, FLOAT fDist, FLOAT fEpsilon);
	void ClipWindingWithPlane(s_WINDING* pWinding, A3DVECTOR3& vNormal, FLOAT fDist, float fEpsilon, s_WINDING** ppFront, s_WINDING** ppBack);
	s_WINDING* CutBackWindingWithPlane(s_WINDING* pWinding, A3DVECTOR3& vNormal, FLOAT fDist, float fEpsilon);
	s_WINDING* CutFrontWindingWithPlane(s_WINDING* pWinding, A3DVECTOR3& vNormal, FLOAT fDist, float fEpsilon);
	s_WINDING* BaseWindingForPlane(A3DVECTOR3& vNormal, FLOAT fDist);
	bool WindingIsTiny(s_WINDING* pWinding);
	float WindingArea(s_WINDING* pWinding);
	void WindingPlane(s_WINDING* pWinding, A3DVECTOR3& vNormal, FLOAT* pfDist);
	void AddWindingToConvexHull(s_WINDING* pWinding, s_WINDING** ppHull, A3DVECTOR3 vNormal);
	void WindingSphere(s_WINDING* pWinding, A3DVECTOR3& vOrigin, float* pfRadius);
}

#endif	//	_PT_WINDING_H_


