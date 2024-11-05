/*
 * FILE: Utility.h
 *
 * DESCRIPTION: Utility routines
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "Global.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Cursor status
enum
{
	CURSOR_ARROW = 0,		//	Normal cursor
	CURSOR_OPERATION,		//	Operation
	CURSOR_SIZEWE,			//	Horizontal size
	CURSOR_SIZENS,			//	Vertical size
	CURSOR_SIZEALL,			//	Size all
	CURSOR_WAIT,			//	Wait
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

extern A3DVECTOR3	g_vOrigin;
extern A3DVECTOR3	g_vAxisX;
extern A3DVECTOR3	g_vAxisY;
extern A3DVECTOR3	g_vAxisZ;

extern int			g_iCurCursor;

extern A3DVECTOR3	g_vLastSectorMins;	//	Last modified sector's volume
extern A3DVECTOR3	g_vLastSectorMaxs;

class CMainFrame;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

CMainFrame*	AUX_GetMainFrame();
int AUX_MessageBox(int iType, const char* szMsg, ...);
void AUX_SetCursor(int iStatus);

//	Collision detection for moving AABB
void AABBMoveToAABB(A3DVECTOR3& vMins1, A3DVECTOR3& vMaxs1, const A3DVECTOR3& vMins2, 
					const A3DVECTOR3& vMaxs2, const A3DVECTOR3& vDelta, float* pfFrac);
bool AABBToAABB(const A3DVECTOR3& vMins1, const A3DVECTOR3& vMaxs1, const A3DVECTOR3& vMins2, const A3DVECTOR3& vMaxs2);
bool AABBInAABB(const A3DVECTOR3& vMins1, const A3DVECTOR3& vMaxs1, const A3DVECTOR3& vMins2, const A3DVECTOR3& vMaxs2);

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

inline float SnapToGrid(float v, float fGrid)
{
	if (v >= 0.0f)
		return (float)(floor(v / fGrid + 0.5f) * fGrid);
	else
		return (float)(-floor(-v / fGrid + 0.5f) * fGrid);
}

inline void SnapVector(A3DVECTOR3* pvIn, A3DVECTOR3* pvOut, float fGrid)
{
	pvOut->x = SnapToGrid(pvIn->x, fGrid);
	pvOut->y = SnapToGrid(pvIn->y, fGrid);
	pvOut->z = SnapToGrid(pvIn->z, fGrid);
}

#endif	//	_UTILITY_H_



