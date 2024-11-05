/*
 * FILE: Global.h
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2004/4/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stdafx.h"
#include "Resource.h"

#include "AC.h"
#include "AF.h"
#include "AM.h"
#include "A3D.h"

#include "include/MoveAgent.h"
#include "include/MoveMap.h"
#include "include/LayerMap.h"
#include "include/bitimage.h"
#include "include/JointList.h"
#include "include/ClusterAbstraction.h"
#include "include/PfConstant.h"
#include "include/CluGraph.h"
#include "include/Island.h"
#include "include/MultiCluGraph.h"
using namespace AutoMove;


///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////
enum
{
	COL_START = 0,
	COL_END,
	COL_PASSABLE,
	COL_BLOCK,
	COL_RAW_PATH0,
	COL_RAW_PATH1,
	COL_OPT_PATH0,
	COL_OPT_PATH1,
	COL_MOVE_POS,
	COL_COUNT
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

extern ALog			g_Log;
extern char			g_szWorkDir[];

extern A3DVECTOR3	g_vOrigin;
extern A3DVECTOR3	g_vAxisX;
extern A3DVECTOR3	g_vAxisY;
extern A3DVECTOR3	g_vAxisZ;

extern  int         g_iClusterSize;
extern  int			g_iEntranceWidth;

extern  A3DPOINT2   g_ptStart;
extern  int         g_iLayerStart;
extern  A3DPOINT2   g_ptGoal;
extern  int         g_iLayerGoal;

extern vector<PathNode>  g_InitPath;

extern  BYTE        g_Color[];
extern  AString     g_szCfgPath;

extern CMoveAgent g_AutoMoveAgent;

class CMainFrame;
class CFormMap;
///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

CMainFrame*	AUX_GetMainFrame();
CFormMap  * AUX_GetFormMap();
void        AUX_SetWorkDir(const char * szDir);
void        AUX_SetStatusbar(const char * szTxt);

void TransMap2Scr(const A3DPOINT2& ptMap, A3DPOINT2& ptScr);
void TransMap2Scr(const APointF& ptMap, A3DPOINT2& ptScr);
void TransScr2Map(const A3DPOINT2& ptScr, A3DPOINT2& ptMap);


///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

template<class InputIterator, class T> 
InputIterator vec_find(InputIterator first, InputIterator last,  T&  target)
{
	InputIterator it;
	for (it = first; it != last; ++it)
	{
		if (*it == target)
		{
			break;
		}
	}
	return it;
}


#endif	//	_GLOBAL_H_



