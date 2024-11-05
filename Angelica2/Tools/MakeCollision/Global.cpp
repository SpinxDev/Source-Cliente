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
#include "MakeCollisionDlg.h"

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

s_CONFIGS	g_Configs;

AArray<s_PLANE, s_PLANE&>	g_aPlanes(10000, 10000);	//	Planes
AArray<s_SIDE*, s_SIDE*>	g_aSides(10000, 10000);		//	Sides
AArray<s_BRUSH*, s_BRUSH*>	g_aBrushes(10000, 10000);	//	Brushes

s_ESPWORLD	g_ESPWorld = {0};

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


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

//	Add message to list box
void AddMessageToList(const char* szMsg, ...)
{
	CMakeCollisionDlg* pDlg = (CMakeCollisionDlg*)AfxGetMainWnd();
	if (!pDlg)
		return;

	static char szStr[1024];

	va_list vaList;
	va_start(vaList, szMsg);

	vsprintf(szStr, szMsg, vaList);
	strcat(szStr, "\n");

	va_end(vaList);

	pDlg->AddMessage(szStr);
}

void UpdateLastMessage(const char* szMsg, ...)
{
	CMakeCollisionDlg* pDlg = (CMakeCollisionDlg*)AfxGetMainWnd();
	if (!pDlg)
		return;

	static char szStr[1024];

	va_list vaList;
	va_start(vaList, szMsg);

	vsprintf(szStr, szMsg, vaList);

	va_end(vaList);

	pDlg->UpdateLastMessage(szStr);
}

//	Set current progress segment
void SetCurProgressSeg(int iSeg)
{
	CMakeCollisionDlg* pDlg = (CMakeCollisionDlg*)AfxGetMainWnd();
	if (!pDlg)
		return;

	pDlg->SetCurProgressSeg(iSeg);
}

//	Step progress bar
void StepProgressBar(int iCur, int iTotal)
{
	CMakeCollisionDlg* pDlg = (CMakeCollisionDlg*)AfxGetMainWnd();
	if (!pDlg)
		return;

	int iPercent = (int)(iCur * 100.0f / iTotal + 0.5f);

	pDlg->StepProgressBar(iPercent);
}

/*	Strip file's extension.

	szName: file name.
*/
void StripFileTail(char* szName)
{
	char* lpDot	  = strrchr(szName, '.');
	char* lpSlash = strrchr(szName, '\\');

	if (lpSlash)	//	Name include path
	{
		//	Ensure '.' follow '\\'
		if (lpDot && lpDot > lpSlash)
			*lpDot = '\0';
	}
	else	//	Only file name
	{
		if (lpDot)
			*lpDot = '\0';
	}
}
