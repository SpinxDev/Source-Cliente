/*
 * FILE: Global.cpp
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2004/4/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Render.h"
#include "MainFrm.h"
#include "FormMap.h"
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

ALog		g_Log;
char		g_szWorkDir[MAX_PATH];

A3DVECTOR3	g_vOrigin(0.0f);
A3DVECTOR3	g_vAxisX(1.0f, 0.0f, 0.0f);
A3DVECTOR3	g_vAxisY(0.0f, 1.0f, 0.0f);
A3DVECTOR3	g_vAxisZ(0.0f, 0.0f, 1.0f);

int         g_iClusterSize = 100;
int         g_iEntranceWidth = 30;

A3DPOINT2   g_ptStart(-1, -1);
A3DPOINT2   g_ptGoal(-1, -1);
int         g_iLayerStart = -1;
int         g_iLayerGoal = -1;

vector<PathNode>  g_InitPath;

AString           g_szCfgPath;

BYTE g_Color[COL_COUNT*3] = 
{ //r    g    b
	0,   0,   255,      //start
	255, 255, 0,		//end
	0,	 255, 0,        //passable
	255, 0,   0,        //block
	0,   0,   0,        //raw path0
	128, 128, 128,      //raw path1
	255, 255, 255,      //opt path0
	128, 255, 255,      //opt path1
	0,   0,   255,      //move pos
};

CMoveAgent g_AutoMoveAgent;

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


CMainFrame*	AUX_GetMainFrame()
{	
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	ASSERT(pMainFrame);
	return pMainFrame;
}

CFormMap * AUX_GetFormMap()
{
	return (CFormMap *)AUX_GetMainFrame()->GetTabCtrlBar()->GetView(RUNTIME_CLASS(CFormMap));
}

void AUX_SetWorkDir(const char * szDir)
{
	strcpy(g_szWorkDir, szDir);
	int len = strlen(g_szWorkDir);

	if ((g_szWorkDir[len-1] != '\\') && (g_szWorkDir[len-1] != '/'))
	{
		strcat(g_szWorkDir, "\\");
	}

	af_SetBaseDir(g_szWorkDir);
}

void TransMap2Scr(const A3DPOINT2& ptMap, A3DPOINT2& ptScr)
{
	TransMap2Scr(APointF(ptMap.x, ptMap.y), ptScr);
}

void TransMap2Scr(const APointF& ptMap, A3DPOINT2& ptScr)
{
	A3DVECTOR3 vMap, vScr;

	APointF ptWld = g_AutoMoveAgent.GetMoveMap()->TransMap2Wld(ptMap);
	vMap.x = ptWld.x;
	vMap.y = 0.0f;
	vMap.z = ptWld.y;
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vMap, vScr);
	ptScr.x = (int) vScr.x;
	ptScr.y = (int) vScr.y;
}


void TransScr2Map(const A3DPOINT2& ptScr, A3DPOINT2& ptMap)
{
 	A3DVECTOR3 vPos((float)ptScr.x, (float)ptScr.y, 0.0f);
 	g_Render.GetA3DViewport()->InvTransform(vPos, vPos);
	
	APointF  ptWld(vPos.x, vPos.z);
 	ptMap = g_AutoMoveAgent.GetMoveMap()->TransWld2Map(ptWld);
}


void AUX_SetStatusbar(const char * szTxt)
{
	AUX_GetMainFrame()->GetStatusBar()->SetPaneText(0, AS2AC(szTxt));
}