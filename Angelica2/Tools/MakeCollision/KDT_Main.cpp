/* 
 * FILE: KDT_Main.cpp
 *
 * DESCRIPTION: KD-Tree routines
 *
 * CREATED BY: Duyuxin, 2003/8/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "MathLib.h"
#include "Plane.h"
#include "Winding.h"
#include "Side.h"
#include "KDT_Main.h"
#include "AMemory.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

#define BSPLEAF_PLANE	-1

/////////////////////////////////////////////////////////////////////////
//
//	Extern variables and functions
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////

AArray<s_KDTNODE*, s_KDTNODE*>	g_aKDTNodes;	//	BSP nodes

int l_iNumReachMinLeafSize;
int l_iNumReachMinSideNum;

/////////////////////////////////////////////////////////////////////////
//
//	Declaration of local functions
//
/////////////////////////////////////////////////////////////////////////

static s_KDTNODE* _AllocBSPNode();
static void _FreeBSPNode(s_KDTNODE* pNode);
static s_KDTSIDE* _AllocBSPSide();
static void _FreeBSPSide(s_KDTSIDE* pSide);
static s_KDTSIDE* _BuildBSPSideList();
static void _ReleaseBSPSideList();
static void _BuildBSPTreeNodes(s_KDTNODE* pNode, s_KDTSIDE* pSideList);
static int _SelectSplitPlane(s_KDTNODE* pNode, s_KDTSIDE* pSideList);
static void _CheckLeaves();

/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

bool KDT_Build()
{
	SetCurProgressSeg(KDTPROG_BUILDTREE);

	//	Build BSP side list
	s_KDTSIDE* pSideList = _BuildBSPSideList();
	if (!pSideList)
	{
		AddMessageToList("Failed to create BSP side list");
		return false;
	}

	StepProgressBar(1, 2);

	//	Allocate root node
	s_KDTNODE* pRoot = _AllocBSPNode();

	//	Calculate bounding box of scene
	ClearBound(pRoot->vMins.m, pRoot->vMaxs.m);
	
	for (int i=0; i < g_aSides.GetSize(); i++)
	{
		s_SIDE* pSide = g_aSides[i];
		for (int j=0; j < pSide->pWinding->iNumPts; j++)
			AddPointToBound(pRoot->vMins.m, pRoot->vMaxs.m, pSide->pWinding->pVerts[j]);
	}

	AddMessageToList("+ Build KD-Tree and filter sides ...");

	l_iNumReachMinLeafSize	= 0;
	l_iNumReachMinSideNum	= 0;

	_BuildBSPTreeNodes(pRoot, pSideList);

	AddMessageToList("+ %d leaves created for hasing too small size", l_iNumReachMinLeafSize);
	AddMessageToList("+ %d leaves created for containing too few sides", l_iNumReachMinSideNum);

	StepProgressBar(2, 2);

	//	Check KDT leaves
	_CheckLeaves();

	return true;
}

void KDT_Release()
{
	for (int i=0; i < g_aKDTNodes.GetSize(); i++)
	{
		DestroySideRefList(g_aKDTNodes[i]->pSideRef);
		_FreeBSPNode(g_aKDTNodes[i]);
	}

	g_aKDTNodes.RemoveAll();
}

s_KDTNODE* _AllocBSPNode()
{
	s_KDTNODE* pNode = new s_KDTNODE;
	ASSERT(pNode);

	memset(pNode, 0, sizeof (s_KDTNODE));

	g_aKDTNodes.Add(pNode);

	return pNode;
}

void _FreeBSPNode(s_KDTNODE* pNode)
{
	if (pNode)
		delete pNode;
}

s_KDTSIDE* _AllocBSPSide()
{
	s_KDTSIDE* pSide = new s_KDTSIDE;
	ASSERT(pSide);

	memset(pSide, 0, sizeof (s_KDTSIDE));

	return pSide;
}

void _FreeBSPSide(s_KDTSIDE* pSide)
{
	if (!pSide)
		return;

	DestroyWinding(pSide->pWinding);
	delete pSide;
}

//	Build BSP side list
s_KDTSIDE* _BuildBSPSideList()
{
	AddMessageToList("+ Build KDP sides ...");

	s_KDTSIDE* pHead = NULL;

	for (int i=0; i < g_aSides.GetSize(); i++)
	{
		s_SIDE* pSide = g_aSides[i];
		s_KDTSIDE* pBSPSide = _AllocBSPSide();

		pBSPSide->iSide		= i;
		pBSPSide->pWinding	= CopyWinding(pSide->pWinding);

		pBSPSide->pNext = pHead;
		pHead = pBSPSide;
	}

	return pHead;
}

//	Release BSP side list
void _ReleaseBSPSideList(s_KDTSIDE* pSideList)
{
	s_KDTSIDE* pSide = pSideList;

	while (pSide)
	{
		s_KDTSIDE* pTemp = pSide->pNext;
		_FreeBSPSide(pSide);
		pSide = pTemp;
	}
}

//	Build BSP nodes
void _BuildBSPTreeNodes(s_KDTNODE* pNode, s_KDTSIDE* pSideList)
{
	int iSplit = _SelectSplitPlane(pNode, pSideList);
	if (iSplit == BSPLEAF_PLANE)
	{
		pNode->iPlane	= BSPLEAF_PLANE;
		pNode->pSideRef	= NULL;

		//	Add side list to this leaf
		for (s_KDTSIDE* pSide = pSideList; pSide; pSide = pSide->pNext)
		{
			//	This side has existed ?
			s_SIDEREF* pRef = pNode->pSideRef;
			while (pRef)
			{
				if (pRef->iSide == pSide->iSide)
					break;

				pRef = pRef->pNext;
			}

			if (pRef)
				continue;
			
			pRef = AllocSideRef();
			pRef->iSide = pSide->iSide;

			pRef->pNext = pNode->pSideRef;
			pNode->pSideRef = pRef;
		}

		//	Release side list
		_ReleaseBSPSideList(pSideList);

		return;
	}

	pNode->iPlane = iSplit;

	s_PLANE Plane = g_aPlanes[iSplit];
	s_KDTSIDE *pSide, *pNext, *pChildList[2] = {NULL, NULL};
	s_WINDING *pFWinding, *pBWinding;
	int iSide;

	//	Split all faces by the selected splitting plane
	for (pSide = pSideList; pSide; pSide = pNext)
	{
		pNext = pSide->pNext;

		//	Check face's relative postion to splitting plane
		iSide = WindingOnPlaneSide(pSide->pWinding, Plane.vNormal, Plane.fDist, EPSILON_CLIP);
		if (iSide == SIDE_FRONT)
		{
			pSide->pNext = pChildList[0];
			pChildList[0] = pSide;
		}
		else if (iSide == SIDE_BACK)
		{
			pSide->pNext = pChildList[1];
			pChildList[1] = pSide;
		}
		else	//	iSide == SIDE_CROSS or SIDE_ON
		{
			ClipWindingWithPlane(pSide->pWinding, Plane.vNormal, Plane.fDist,
								 EPSILON_CLIP * 2, &pFWinding, &pBWinding);
			if (pFWinding)
			{
				s_KDTSIDE* pNewSide	= _AllocBSPSide();
				pNewSide->iSide		= pSide->iSide;
				pNewSide->pWinding	= pFWinding;
				pNewSide->pNext		= pChildList[0];
				pChildList[0] = pNewSide;
			}
			
			if (pBWinding)
			{
				s_KDTSIDE* pNewSide	= _AllocBSPSide();
				pNewSide->iSide		= pSide->iSide;
				pNewSide->pWinding	= pBWinding;
				pNewSide->pNext		= pChildList[1];
				pChildList[1] = pNewSide;
			}

			//	Free old side
			_FreeBSPSide(pSide);
		}
	}

	s_KDTNODE* pChildNode;
	int i;

	//	Build child nodes
	for (i=0; i < 2; i++)
	{
		pChildNode = _AllocBSPNode();
		pChildNode->pParent	= pNode;
		pNode->aChildren[i]	= pChildNode;

		pChildNode->vMins = pNode->vMins;
		pChildNode->vMaxs = pNode->vMaxs;
	}

	//	Calculate child node's bounding box
	for (i=0; i < 3; i++)
	{
		if (Plane.vNormal.m[i] == 1.0f)
		{
			pNode->aChildren[0]->vMins.m[i] = Plane.fDist;
			pNode->aChildren[1]->vMaxs.m[i] = Plane.fDist;
			break;
		}
	}

	ASSERT(i < 3);

	//	Continue to build children node space
	_BuildBSPTreeNodes(pNode->aChildren[0], pChildList[0]);
	_BuildBSPTreeNodes(pNode->aChildren[1], pChildList[1]);
}

int _SelectSplitPlane(s_KDTNODE* pNode, s_KDTSIDE* pSideList)
{
	s_PLANE aPlanes[3];
	int i, iNumPlane = 0;

	//	Check on axises from l_iCurAxis
	for (i=0; i < 3; i++)
	{
		if (pNode->vMaxs.m[i] - pNode->vMins.m[i] <= g_Configs.fKDTMinLeafSize)
			continue;

		aPlanes[iNumPlane].vNormal.Clear();
		aPlanes[iNumPlane].vNormal.m[i] = 1.0f;
		aPlanes[iNumPlane].fDist = (pNode->vMaxs.m[i] + pNode->vMins.m[i]) * 0.5f;
		iNumPlane++;
	}

	//	Leaf is too small ?
	if (!iNumPlane)
	{
		l_iNumReachMinLeafSize++;
		return BSPLEAF_PLANE;
	}

	//	Count side number
	int iCount = 0;

	s_KDTSIDE* pSide = pSideList;
	while (pSide)
	{
		iCount++;
		pSide = pSide->pNext;
	}

	//	Too few sides ?
	if (iCount < g_Configs.iKDTMinSideNum)
	{
		l_iNumReachMinSideNum++;
		return BSPLEAF_PLANE;
	}
	
	int iFront, iBack, iCross, iBestVal, iSelPlane;

	iBestVal  = 999999;
	iSelPlane = BSPLEAF_PLANE;

	//	Select plane
	for (i=0; i < iNumPlane; i++)
	{
		iFront = iBack = iCross = 0;

		for (pSide = pSideList; pSide; pSide = pSide->pNext)
		{
			int iSide = WindingOnPlaneSide(pSide->pWinding, aPlanes[i].vNormal, aPlanes[i].fDist, EPSILON_CLIP);
			if (iSide == SIDE_FRONT)
				iFront++;
			else if (iSide == SIDE_BACK)
				iBack++;
			else //	iSide == SIDE_CROSS or SIDE_ON
				iCross++;
		}

		int iValue = iCross;
		if (iValue < iBestVal)
		{
			iBestVal	= iValue;
			iSelPlane	= i;
		}
	}

	//	Cannot found a split face
	if (iSelPlane == BSPLEAF_PLANE)
		return BSPLEAF_PLANE;

	return SearchPlane(aPlanes[iSelPlane].vNormal, aPlanes[iSelPlane].fDist);
}

//	Check leaves
static void _CheckLeaves()
{
	AddMessageToList("+ Check KD-Tree leaves ...");

	int i, j, iBadLeaf = 0;

	for (i=0; i < g_aKDTNodes.GetSize(); i++)
	{
		s_KDTNODE* pNode = g_aKDTNodes[i];

		//	Is a leaf ?
		if (pNode->aChildren[0] || pNode->aChildren[1])
			continue;

		bool bFlag = false;
			
		//	Check leaf size
		for (j=0; j < 3; j++)
		{
			if (pNode->vMaxs.m[j] - pNode->vMins.m[j] > g_Configs.fKDTMinLeafSize)
			{
				bFlag = true;
				break;
			}
		}

		if (bFlag)
		{
			//	Check referenced side number
			j = 0; 
			s_SIDEREF* pRef = pNode->pSideRef;

			while (pRef)
			{
				pRef = pRef->pNext;
				j++;
			}

			if (j >= g_Configs.iKDTMinSideNum)
				AddMessageToList("+ KDT Node %d is a bad leaf", i);
		}
	}
}


