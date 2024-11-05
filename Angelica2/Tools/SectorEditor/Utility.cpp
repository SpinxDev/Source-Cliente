/*
 * FILE: Global.cpp
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2002/6/30
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "MainFrm.h"
#include "Utility.h"
#include "A3DCollision.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

A3DVECTOR3	g_vOrigin(0.0f);
A3DVECTOR3	g_vAxisX(1.0f, 0.0f, 0.0f);
A3DVECTOR3	g_vAxisY(0.0f, 1.0f, 0.0f);
A3DVECTOR3	g_vAxisZ(0.0f, 0.0f, 1.0f);

A3DVECTOR3	g_vLastSectorMins(-20.0f);	//	Last modified sector's volume
A3DVECTOR3	g_vLastSectorMaxs(20.0f);

int	g_iCurCursor = CURSOR_ARROW;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

CMainFrame*	AUX_GetMainFrame()
{	
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	ASSERT(pMainFrame);
	return pMainFrame;
}

//	Output message with window's message box
int AUX_MessageBox(int iType, const char* szMsg, ...)
{
	static char szText[1024];
	
	va_list vaList;
	va_start(vaList, szMsg);
	vsprintf(szText, szMsg, vaList);
	va_end(vaList);

	return AfxGetMainWnd()->MessageBox(szText, "ÌבÊ¾", iType);
}

//	Collision detection for moving AABB
void AABBMoveToAABB(A3DVECTOR3& vMins1, A3DVECTOR3& vMaxs1, const A3DVECTOR3& vMins2, 
					const A3DVECTOR3& vMaxs2, const A3DVECTOR3& vDelta, float* pfFrac)
{
	int i;

	*pfFrac = 0.0f;

	for (i=0; i < 3; i++)
	{
		if (vDelta.m[i] == 0.0f)
			continue;

		float fFrac;

		if (vDelta.m[i] > 0)
		{
			if (vMins2.m[i] < vMaxs1.m[i])
				continue;

			fFrac = (vMins2.m[i] - vMaxs1.m[i]) / vDelta.m[i];
		}
		else
		{
			if (vMaxs2.m[i] > vMins1.m[i])
				continue;

			fFrac = (vMaxs2.m[i] - vMins1.m[i]) / vDelta.m[i];
		}

		if (fFrac > *pfFrac)
			*pfFrac = fFrac;
	}
}

bool AABBToAABB(const A3DVECTOR3& vMins1, const A3DVECTOR3& vMaxs1, const A3DVECTOR3& vMins2, const A3DVECTOR3& vMaxs2)
{
	A3DVECTOR3 vCenter1 = (vMins1 + vMaxs1) * 0.5f;
	A3DVECTOR3 vExt1 = vMaxs1 - vCenter1;
	A3DVECTOR3 vCenter2 = (vMins2 + vMaxs2) * 0.5f;
	A3DVECTOR3 vExt2 = vMaxs2 - vCenter2;

	return CLS_AABBAABBOverlap(vCenter1, vExt1, vCenter2, vExt2);
}

bool AABBInAABB(const A3DVECTOR3& vMins1, const A3DVECTOR3& vMaxs1, const A3DVECTOR3& vMins2, const A3DVECTOR3& vMaxs2)
{
	for (int i=0; i < 3; i++)
	{
		if (vMins1.m[i] < vMins2.m[i] || vMaxs1.m[i] > vMaxs2.m[i])
			break;
	}

	return i >= 3 ? true : false;
}

//	Change cursor to operation cursor
void AUX_SetCursor(int iStatus)
{
	switch (iStatus)
	{
	case CURSOR_OPERATION:	SetCursor(LoadCursor(NULL, IDC_CROSS));		break;
	case CURSOR_SIZEWE:		SetCursor(LoadCursor(NULL, IDC_SIZENS));	break;
	case CURSOR_SIZENS:		SetCursor(LoadCursor(NULL, IDC_SIZEWE));	break;
	case CURSOR_SIZEALL:	SetCursor(LoadCursor(NULL, IDC_SIZEALL));	break;
	case CURSOR_WAIT:		SetCursor(LoadCursor(NULL, IDC_WAIT));		break;
	case CURSOR_ARROW:
	default:				SetCursor(LoadCursor(NULL, IDC_ARROW));		break;
	}

	g_iCurCursor = iStatus;
}

