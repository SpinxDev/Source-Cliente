/*
 * FILE: PT_BSP.h
 *
 * DESCRIPTION: BSP for sectors routines
 *
 * CREATED BY: Duyuxin, 2003/8/12
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "PT_BSP.h"
#include "PT_Sector.h"
#include "PT_Winding.h"
#include "PT_Math.h"
#include "PT_Plane.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

#define LEAF_PLANE	-1

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

//	BSP spliting face
struct s_BSPFACE
{
	int				iPlane;		//	Plane index
	bool			bChecked;	//	Used when spliting node
	s_WINDING*		pWinding;	//	Winding vertices
	s_BSPFACE*		pNext;	
};

//	Sector information used in BSP
struct s_BSPSECTOR
{
	s_PTSECTOR*		pSector;
	s_BSPSECTOR*	pNext;
};

//	Brush
struct s_BSPBRUSH
{
	float			vMins[3];		//	Bound box
	float			vMaxs[3];
	s_BSPBRUSH*		pNext;			//	Point to next brush
	int				iNumSides;		//	Number of sides
	s_PTSECTORSIDE*	aSides;			//	Sides
	s_PTSECTOR*		pSector;
};

namespace PORTAL
{
	s_BSPFACE* _BuildBSPFaces();
	void _BuildBSPTreeNodes(s_PTBSPNODE* pNode, s_BSPFACE* pFaceList);
	int _SelectSplitPlane(s_BSPFACE* pFaceList);
	s_PTBSPNODE* _AllocBSPNode();
	void _FreeBSPNode(s_PTBSPNODE* pNode);
	s_BSPFACE* _AllocBSPFace();
	void _FreeBSPFace(s_BSPFACE* pFace);
	
	s_BSPBRUSH* _BuildBrushList();
	s_BSPBRUSH* _AllocBrush(int iNumSides, s_PTSECTOR* pSector);
	void _FreeBrush(s_BSPBRUSH* pBrush);
	void _DestroyBrushList(s_BSPBRUSH* pBrushList);
	s_BSPBRUSH* _CopyBrush(s_BSPBRUSH* pSrcBrush);
	void _FilterBrushesIntoTree(s_BSPBRUSH* pBrushList, s_PTBSPNODE* pRoot);
	int _FilterBrushIntoTree(s_BSPBRUSH* pBrush, s_PTBSPNODE* pNode);
	void _SplitBrushWithPlane(s_BSPBRUSH* pBrush, int iPlane, s_BSPBRUSH** ppFront, s_BSPBRUSH** ppBack);
	int _BrushMostlyOnPlaneSide(s_BSPBRUSH* pBrush, A3DVECTOR3 vNormal, float fDist);
	void _BoundBrush(s_BSPBRUSH* pBrush);
	float BrushVolume(s_BSPBRUSH* pBrush);
}

/////////////////////////////////////////////////////////////////////////
//
//	Declaration of local functions
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

namespace PORTAL
{

//	Build BSP tree for sectors
bool ProcessBSP()
{
	//	Build BSP faces
	s_BSPFACE* pFaceList = _BuildBSPFaces();
	if (!pFaceList)
		return true;

	//	Create BSP nodes
	s_PTBSPNODE* pRoot = _AllocBSPNode();
	_BuildBSPTreeNodes(pRoot, pFaceList);

	//	Create brush list
	s_BSPBRUSH* pBrushList = _BuildBrushList();
	if (pBrushList)
	{
		//	Filter brushes into BSP tree
		_FilterBrushesIntoTree(pBrushList, pRoot);

		//	Release brush list
		_DestroyBrushList(pBrushList);
	}

	return true;
}

//	Release BSP tree
void ReleaseBSPNodes(s_PTBSPNODE* pNode)
{
	if (!pNode)
		return;

	if (pNode->iPlane != LEAF_PLANE)
	{
		ReleaseBSPNodes(pNode->aChildren[0]);
		ReleaseBSPNodes(pNode->aChildren[1]);
	}

	_FreeBSPNode(pNode);
}

//	Build planes used to split space
s_BSPFACE* _BuildBSPFaces()
{
	int i, j;
	s_BSPFACE* pFaceList = NULL;

	for (i=0; i < g_aSectors.GetSize(); i++)
	{
		s_PTSECTOR* pSector = g_aSectors[i];

		for (j=0; j < pSector->aSides.GetSize(); j++)
		{
			s_PTSECTORSIDE* pSide = pSector->aSides[j];
			s_BSPFACE* pBSPFace = _AllocBSPFace();

			pBSPFace->pWinding	= CopyWinding(pSide->pWinding);
			pBSPFace->iPlane	= pSide->iPlane;
			pBSPFace->bChecked	= false;

			pBSPFace->pNext = pFaceList;
			pFaceList = pBSPFace;
		}
	}

	return pFaceList;
}

//	Build nodes of BSP tree
void _BuildBSPTreeNodes(s_PTBSPNODE* pNode, s_BSPFACE* pFaceList)
{
	int iSplit = _SelectSplitPlane(pFaceList);
	if (iSplit == LEAF_PLANE)
	{
		pNode->iPlane = LEAF_PLANE;
		return;
	}

	pNode->iPlane = iSplit;

	s_PTPLANE Plane = g_aPlanes[iSplit];
	s_BSPFACE *pFace, *pNext, *pChildList[2] = {NULL, NULL};
	s_WINDING *pFWinding, *pBWinding;
	int iSide;

	//	Split all faces by the selected splitting plane
	for (pFace = pFaceList; pFace; pFace = pNext)
	{
		pNext = pFace->pNext;
	
		if (iSplit == pFace->iPlane)
		{
			_FreeBSPFace(pFace);
			continue;
		}

		//	Check face's relative postion to splitting plane
		iSide = WindingOnPlaneSide(pFace->pWinding, Plane.vNormal, Plane.fDist, EPSILON_CLIP);
		if (iSide == SIDE_FRONT)
		{
			pFace->pNext = pChildList[0];
			pChildList[0] = pFace;
		}
		else if (iSide == SIDE_BACK)
		{
			pFace->pNext = pChildList[1];
			pChildList[1] = pFace;
		}
		else if (iSide == SIDE_CROSS)
		{
			ClipWindingWithPlane(pFace->pWinding, Plane.vNormal, Plane.fDist,
								 EPSILON_CLIP * 2, &pFWinding, &pBWinding);
			if (pFWinding)
			{
				s_BSPFACE* pNewFace	= _AllocBSPFace();
				pNewFace->iPlane	= pFace->iPlane;
				pNewFace->pWinding	= pFWinding;
				pNewFace->pNext		= pChildList[0];
				pChildList[0] = pNewFace;
			}
			
			if (pBWinding)
			{
				s_BSPFACE* pNewFace	= _AllocBSPFace();
				pNewFace->iPlane	= pFace->iPlane;
				pNewFace->pWinding	= pBWinding;
				pNewFace->pNext		= pChildList[1];
				pChildList[1] = pNewFace;
			}

			//	Free old face
			_FreeBSPFace(pFace);
		}
		else	//	iSide == SIDE_ON
		{
			_FreeBSPFace(pFace);
		}
	}

	s_PTBSPNODE* pChildNode;

	//	Build child nodes
	for (int i=0; i < 2; i++)
	{
		pChildNode = _AllocBSPNode();
		pChildNode->pParent	= pNode;
		pNode->aChildren[i]	= pChildNode;
	}

	//	Continue to build children node space
	_BuildBSPTreeNodes(pNode->aChildren[0], pChildList[0]);
	_BuildBSPTreeNodes(pNode->aChildren[1], pChildList[1]);
}

//	Select a split plane
int _SelectSplitPlane(s_BSPFACE* pFaceList)
{
	int iFront, iBack, iCross, iOn, iBestVal, iSelPlane;
	s_BSPFACE *pSplit, *pCheck;

	iBestVal  = 999999;
	iSelPlane = LEAF_PLANE;

	//	First clear faces' check flag
	for (pSplit = pFaceList; pSplit; pSplit = pSplit->pNext)
		pSplit->bChecked = false;

	//	Select face
	for (pSplit = pFaceList; pSplit; pSplit = pSplit->pNext)
	{
		if (pSplit->bChecked)
			continue;

		s_PTPLANE Plane = g_aPlanes[pSplit->iPlane];

		iFront = iBack = iCross = iOn = 0;

		for (pCheck = pFaceList; pCheck; pCheck = pCheck->pNext)
		{
			if (pCheck->iPlane == pSplit->iPlane)
			{
				iOn++;
				pCheck->bChecked = true;	//	Don't use it as split plane again
				continue;
			}

			int iSide = WindingOnPlaneSide(pCheck->pWinding, Plane.vNormal, Plane.fDist, EPSILON_CLIP);
			if (iSide == SIDE_FRONT)
				iFront++;
			else if (iSide == SIDE_BACK)
				iBack++;
			else if (iSide == SIDE_CROSS)
				iCross++;
			else	//	iSide == SIDE_ON
			{
				iOn++;
				pCheck->bChecked = true;	//	Don't use it as split plane again
			}
		}

		int iValue = (int)abs(iFront * 2 - iBack * 2) + iCross - iOn;
		if (iValue < iBestVal)
		{
			iBestVal	= iValue;
			iSelPlane	= pSplit->iPlane;
		}
	}

	return iSelPlane;
}

//	Allocate BSP node
s_PTBSPNODE* _AllocBSPNode()
{
	s_PTBSPNODE* pNode = new s_PTBSPNODE;
	ASSERT(pNode);

	memset(pNode, 0, sizeof (s_PTBSPNODE));

	g_aBSPNodes.Add(pNode);

	return pNode;
}

//	Free BSP node
void _FreeBSPNode(s_PTBSPNODE* pNode)
{
	if (pNode)
		delete pNode;
}

//	Allocate BSP face
s_BSPFACE* _AllocBSPFace()
{
	s_BSPFACE* pFace = new s_BSPFACE;
	ASSERT(pFace);

	memset(pFace, 0, sizeof (s_BSPFACE));

	return pFace;
}

//	Free BSP face
void _FreeBSPFace(s_BSPFACE* pFace)
{
	if (pFace)
		delete pFace;
}

/*	Create a new brush structure.

	Return new brush's address for success, otherwise return NULL.

	iNumSide: number of sides in new brush
	pSector: sector which the brush is associated with
*/
s_BSPBRUSH* _AllocBrush(int iNumSides, s_PTSECTOR* pSector)
{
	s_BSPBRUSH* pBrush = new s_BSPBRUSH;
	ASSERT(pBrush);

	memset(pBrush, 0, sizeof (s_BSPBRUSH));

	pBrush->iNumSides	= iNumSides;
	pBrush->pSector		= pSector;
	pBrush->aSides		= new s_PTSECTORSIDE[iNumSides];
	memset(pBrush->aSides, 0, iNumSides * sizeof (s_PTSECTORSIDE));

	ClearBound(pBrush->vMins, pBrush->vMaxs);

	return pBrush;
}

//	Release a brush structure.
void _FreeBrush(s_BSPBRUSH* pBrush)
{
	if (pBrush)
	{
		if (pBrush->aSides)
		{
			for (int i=0; i < pBrush->iNumSides; i++)
				DestroyWinding(pBrush->aSides[i].pWinding);

			delete [] pBrush->aSides;
		}

		delete pBrush;
	}
}

//	Destroy brush list
void _DestroyBrushList(s_BSPBRUSH* pBrushList)
{
	s_BSPBRUSH* pNext;

	while (pBrushList)
	{
		pNext = pBrushList->pNext;
		_FreeBrush(pBrushList);
		pBrushList = pNext;
	}
}

/*	Create a new brush and copy source brush's data to it.

	Return new brush's address for success, otherwise return NULL

	pSrcBrush: source brush.
*/
s_BSPBRUSH* _CopyBrush(s_BSPBRUSH* pSrcBrush)
{
	s_BSPBRUSH* pBrush = _AllocBrush(pSrcBrush->iNumSides, pSrcBrush->pSector);
	if (!pBrush)
		return NULL;

	for (int i=0; i < 3; i++)
	{
		pBrush->vMins[i] = pSrcBrush->vMins[i];
		pBrush->vMaxs[i] = pSrcBrush->vMaxs[i];
	}

	for (i=0; i < pSrcBrush->iNumSides; i++)
	{
		pBrush->aSides[i].iPlane	= pSrcBrush->aSides[i].iPlane;
		pBrush->aSides[i].pWinding	= CopyWinding(pSrcBrush->aSides[i].pWinding);
	}

	return pBrush;
}

/*	Filter brushes into BPS tree and set leaves's opaque property accordingly.

	pBrushList: brushes list's header.
	pRoot: BSP tree's root node.
*/
void _FilterBrushesIntoTree(s_BSPBRUSH* pBrushList, s_PTBSPNODE* pRoot)
{
	s_BSPBRUSH* pBrush = pBrushList;
	while (pBrush)
	{
		_FilterBrushIntoTree(pBrush, pRoot);
		pBrush = pBrush->pNext;
	}
}

/*	Filter a brush into BSP tree and set leaves's opaque property accordingly.

	Return number of reference that BSP tree to the brush.

	pBrush: brush will be filter into BSP tree.
	pNode: BSP tree's root node.
*/
int _FilterBrushIntoTree(s_BSPBRUSH* pBrush, s_PTBSPNODE* pNode)
{
	if (!pBrush)
		return 0;

	//	Is a leaf ?
	if (pNode->iPlane == LEAF_PLANE)
	{
		//	This leaf has been occupied ?
		if (pNode->pSector)
		{
			PORTAL::g_Log.Log("_FilterBrushIntoTree, Leaf has been occupied !");
			return 0;
		}

		pNode->pSector = pBrush->pSector;

		//	Needn't any more
		_FreeBrush(pBrush);
		return 1;
	}

	//	Split brush with node's spliting plane
	s_BSPBRUSH *pFront, *pBack, *pNewBrush = _CopyBrush(pBrush);
	_SplitBrushWithPlane(pNewBrush, pNode->iPlane, &pFront, &pBack);
	_FreeBrush(pNewBrush);

	//	Continue
	int iNumRef = 0;
	iNumRef += _FilterBrushIntoTree(pFront, pNode->aChildren[0]);
	iNumRef += _FilterBrushIntoTree(pBack, pNode->aChildren[1]);

	return iNumRef;
}

/*	Split brush with plane.

	pBrush: brush will be splited.
	iPlane: split plane's index.
	ppFront (out): splitted front brush
	ppBack (out): splitted back brush
*/
void _SplitBrushWithPlane(s_BSPBRUSH* pBrush, int iPlane, s_BSPBRUSH** ppFront, s_BSPBRUSH** ppBack)
{
	s_BSPBRUSH* aBrushes[2];
	s_WINDING *pWinding, *pMidWinding;
	int i, j;
	float fMaxFront=0, fMaxBack=0, d;

	A3DVECTOR3 vNormal = g_aPlanes[iPlane].vNormal;
	float fDist = g_aPlanes[iPlane].fDist;

	//	Check all vertices in source brush and use fMaxFront, fMaxBack to
	//	record the maximum distance from vertices to plane in both sides
	for (i=0; i < pBrush->iNumSides; i++)
	{
		if (!(pWinding = pBrush->aSides[i].pWinding))
			continue;

		for (j=0; j < pWinding->iNumPts; j++)
		{
			d = DotProduct(vNormal, pWinding->pVerts[j]) - fDist;
			if (d > 0 && d > fMaxFront)
				fMaxFront = d;
			else if (d < 0 && d < fMaxBack)
				fMaxBack = d;
		}
	}

	//	If all brush is in front of the plane
	if (fMaxBack > -EPSILON_CLIP)
	{
		*ppFront = _CopyBrush(pBrush);
		*ppBack	 = NULL;
		return;
	}
	
	//	If all brush is behide the plane
	if (fMaxFront < EPSILON_CLIP)
	{
		*ppFront = NULL;
		*ppBack	 = _CopyBrush(pBrush);
		return;
	}

	//	Create a new winding on splitting plane. Do this by clip splitting plane
	//	with all brush sides' back
	pMidWinding = BaseWindingForPlane(vNormal, fDist);
	for (i=0; i < pBrush->iNumSides && pMidWinding; i++)
	{
		s_PTPLANE Plane = g_aPlanes[pBrush->aSides[i].iPlane];

		pWinding	= pMidWinding;
		pMidWinding = CutFrontWindingWithPlane(pWinding, Plane.vNormal, Plane.fDist, 0);
		DestroyWinding(pWinding);
	}

	//	If middle winding is too small, then don't split brush
	if (!pMidWinding || WindingIsTiny(pMidWinding))
	{
		int iSide = _BrushMostlyOnPlaneSide(pBrush, vNormal, fDist);
		if (iSide == SIDE_FRONT)
		{
			*ppFront = _CopyBrush(pBrush);
			*ppBack	 = NULL;
		}
		else if (iSide == SIDE_BACK)
		{
			*ppFront = NULL;
			*ppBack  = _CopyBrush(pBrush);
		}
	
		return;
	}

	//	Then create two new brushes (front and back)
	for (i=0; i < 2; i++)
	{
		aBrushes[i] = _AllocBrush(pBrush->iNumSides + 1, pBrush->pSector);
		aBrushes[i]->iNumSides = 0;
	}

	//	Split all sides of current brush by splitting plane
	s_WINDING* w[2];	//	Windings
	s_PTSECTORSIDE* pSide;

	for (i=0; i < pBrush->iNumSides; i++)
	{
		ClipWindingWithPlane(pBrush->aSides[i].pWinding, vNormal, fDist, 0.0f, &w[0], &w[1]);

		//	Add splitted windings to front and back brush respectively
		for (j=0; j < 2; j++)
		{
			if (!w[j])
				continue;

			pSide  = &aBrushes[j]->aSides[aBrushes[j]->iNumSides];
			*pSide = pBrush->aSides[i];
			pSide->pWinding	= w[j];

			aBrushes[j]->iNumSides++;
		}
	}

	//	Check whether the two new brushes are valid or not
	for (i=0; i < 2; i++)
	{
		_BoundBrush(aBrushes[i]);

		for (j=0; j < 3; j++)
		{
			if (aBrushes[i]->vMins[j] < -MAXSIZE_WORLD ||
				aBrushes[i]->vMaxs[j] > MAXSIZE_WORLD)
				break;
		}

		if (j < 3 || aBrushes[i]->iNumSides < 3)
		{
			PORTAL::g_Log.Log("_SplitBrushWithPlane, Invalid brush after split");
			_FreeBrush(aBrushes[i]);
			aBrushes[i] = NULL;
		}
	}

	//	One brush was clipped
	if (!aBrushes[0] || !aBrushes[1])
	{
		if (!aBrushes[0] && !aBrushes[1])
			PORTAL::g_Log.Log("SplitBrushWithPlane, No brush left");
		else if (aBrushes[0])
		{
			_FreeBrush(aBrushes[0]);
			*ppFront = _CopyBrush(pBrush);
			*ppBack  = NULL;
		}
		else	//	aBrushes[1] != NULL
		{
			_FreeBrush(aBrushes[1]);
			*ppFront = NULL;
			*ppBack  = _CopyBrush(pBrush);
		}

		return;
	}

	//	Add middle winding to both new brushes
	pSide = &aBrushes[0]->aSides[aBrushes[0]->iNumSides++];
	pSide->pWinding = ReverseWinding(pMidWinding);
	pSide->iPlane	= MakePlaneFromPoints(pSide->pWinding->pVerts[0], pSide->pWinding->pVerts[1], pSide->pWinding->pVerts[2]);
	
	pSide = &aBrushes[1]->aSides[aBrushes[1]->iNumSides++];
	pSide->pWinding = pMidWinding;
	pSide->iPlane	= iPlane;

	//	If brush's volume is too small, we will discard it
	for (i=0; i < 2; i++)
	{
		if (BrushVolume(aBrushes[i]) < 0.001f)
		{
			_FreeBrush(aBrushes[i]);
			aBrushes[i] = NULL;
		}
	}

	*ppFront = aBrushes[0];
	*ppBack  = aBrushes[1];
}

/*	Judge the furthest vertex in brush to plane is on which side of specified plane.

	Return brush's relative position to plane.

	pBrush: valid brush.
	vNormal: plane's normal
	fDist: d parameter of plane.
*/
int _BrushMostlyOnPlaneSide(s_BSPBRUSH* pBrush, A3DVECTOR3 vNormal, float fDist)
{
	int iSide=SIDE_FRONT, i, j;
	s_WINDING* pWinding;
	float d, fMax=0.0f;

	for (i=0; i < pBrush->iNumSides; i++)
	{
		pWinding = pBrush->aSides[i].pWinding;
		for (j=0; j < pWinding->iNumPts; j++)
		{
			d = DotProduct(vNormal, pWinding->pVerts[j]) - fDist;
			if (d > fMax)
			{
				iSide = SIDE_FRONT;
				fMax  = d;
			}
			else if (-d > fMax)
			{
				iSide = SIDE_BACK;
				fMax  = -d;
			}
		}
	}

	return iSide;
}

/*	Calculate brush's bound box

	pBrush: specified brush.
*/
void _BoundBrush(s_BSPBRUSH* pBrush)
{
	int i, j;
	s_WINDING *pWinding;

	ClearBound(pBrush->vMins, pBrush->vMaxs);

	for (i=0; i < pBrush->iNumSides; i++)
	{
		if (!(pWinding = pBrush->aSides[i].pWinding))
			continue;

		for (j=0; j < pWinding->iNumPts; j++)
			AddPointToBound(pBrush->vMins, pBrush->vMaxs, pWinding->pVerts[j]);
	}
}

/*	Calculate brush's volume and return it.

	pBrush: specified brush.
*/
float BrushVolume(s_BSPBRUSH* pBrush)
{
	A3DVECTOR3 vCorner;
	s_WINDING* pWinding;
	float fVolume, fArea, d;

	//	Select a valid point from brush
	for (int i=0; i < pBrush->iNumSides; i++)
	{
		if (pBrush->aSides[i].pWinding)
			break;
	}

	if (i == pBrush->iNumSides)
		return 0.0f;

	vCorner = pBrush->aSides[i].pWinding->pVerts[0];
	fVolume = 0.0f;

	for (i=0; i < pBrush->iNumSides; i++)
	{
		if (!(pWinding = pBrush->aSides[i].pWinding))
			continue;

		s_PTPLANE Plane = g_aPlanes[pBrush->aSides[i].iPlane];
		fArea  = WindingArea(pWinding);
		d = -(DotProduct(vCorner, Plane.vNormal) - Plane.fDist);

		fVolume += d * fArea;
	}

	return fVolume / 3;
}

//	Build brush list
s_BSPBRUSH* _BuildBrushList()
{
	s_BSPBRUSH *pBrushList=NULL, *pBrush;

	int i, j, iNumSide;

	for (i=0; i < g_aSectors.GetSize(); i++)
	{
		s_PTSECTOR* pSector = g_aSectors[i];
		
		iNumSide = pSector->aSides.GetSize();
		pBrush = _AllocBrush(iNumSide, pSector);

		for (j=0; j < 3; j++)
		{
			pBrush->vMins[j] = pSector->vMins.m[j];
			pBrush->vMaxs[j] = pSector->vMaxs.m[j];
		}

		for (j=0; j < pSector->aSides.GetSize(); j++)
		{
			s_PTSECTORSIDE* pSide = pSector->aSides[j];

			pBrush->aSides[j].pWinding	= CopyWinding(pSide->pWinding);
			pBrush->aSides[j].iPlane	= pSide->iPlane;
		}

		pBrush->pNext = pBrushList;
		pBrushList = pBrush;
	}

	return pBrushList;
}

}


