/*
 * FILE: Side.cpp
 *
 * DESCRIPTION: Operations for sides
 *
 * CREATED BY: Duyuxin, 2001/9/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "Side.h"
#include "Plane.h"
#include "Brush.h"
#include "Winding.h"
#include "A3DFrame.h"
#include "A3DMesh.h"
#include "AFile.h"
#include "A3DModel.h"
#include "A3DSceneHull.h"
#include "AMemory.h"
#include "AF.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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

//	Counters
int	l_iNumMergeTris	= 0;	//	Pair of triangles were merged
int	l_iNumParallel	= 0;	//	Pair of parallel edges in quadrangles
int l_iSideCount	= 0;
int l_iSideTotal	= 0;

AArray<A3DFrame*, A3DFrame*>	l_aFrames;

/////////////////////////////////////////////////////////////////////////
//
//	Declaration of local functions
//
/////////////////////////////////////////////////////////////////////////

s_SIDE* _AllocSide();
void _FreeSide(s_SIDE* pSide);
void _CreateSides();
void _ParseA3DModel(A3DModel* pModel);
bool _CreateSidesFromTriangles(int iNumVert, int iNumIndex, A3DVERTEX* aVerts, WORD* aIndices, const A3DMESHPROP& MeshProps);
bool _IsConvexSide(A3DVERTEX* aVerts, WORD* aIndices, int iNumIndex, A3DVECTOR3& vNormal);
int _IdentifyQuadSide(A3DVERTEX* aVerts, WORD* aIndices, A3DVECTOR3& vNormal);
int _CalcSideNumber();

/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

s_SIDE* _AllocSide()
{
	s_SIDE* pSide = new s_SIDE;
	ASSERT(pSide);

	memset(pSide, 0, sizeof (s_SIDE));

	return pSide;
}

void _FreeSide(s_SIDE* pSide)
{
	if (!pSide)
		return;

	DestroyWinding(pSide->pWinding);

	delete pSide;
}

//	Calculate total side number
int _CalcSideNumber()
{
	int i, iNumTotal = 0;

	for (i=0; i < l_aFrames.GetSize(); i++)
	{
		A3DMesh* pMesh = l_aFrames[i]->GetFirstMesh();
		while (pMesh)
		{
			const A3DMESHPROP& MeshProp = pMesh->GetProperty();

			if (pMesh->GetFrameCount() <= 1 && !MeshProp.GetPickThroughFlag())
				iNumTotal += pMesh->GetIndexCount() / 3;

			//	Get next mesh
			pMesh = l_aFrames[i]->GetNextMesh();
		}
	}

	return iNumTotal;
}

//	Create sides from mesh data
void _CreateSides()
{
	int i, iNumVert, iNumIndex;
	A3DVERTEX* aVerts;
	WORD* aIndices;
	char szTexFile[MAX_PATH];
	bool bAlpha;

	l_iNumMergeTris = 0;
	l_iNumParallel	= 0;
	l_iSideCount	= 0;
	l_iSideTotal	= _CalcSideNumber();

	for (i=0; i < l_aFrames.GetSize(); i++)
	{
		A3DMesh* pMesh = l_aFrames[i]->GetFirstMesh();
		while (pMesh)
		{
			A3DMESHPROP MeshProp = pMesh->GetProperty();

			if (pMesh->GetFrameCount() > 1)
			{
				AddMessageToList("! _CreateSides, Mesh was ignored because of multi-frames");
				goto NextMesh;
			}

			if (MeshProp.GetPickThroughFlag())
				goto NextMesh;

			iNumVert	= pMesh->GetVertCount();
			iNumIndex	= pMesh->GetIndexCount();
			aVerts		= pMesh->GetVerts(0);
			aIndices	= pMesh->GetIndices();

			strcpy(szTexFile, pMesh->GetTextureMap());
			if (af_CheckFileExt(szTexFile, ".tga", 4) || af_CheckFileExt(szTexFile, ".sdr", 4))
				bAlpha = true;
			else
				bAlpha = false;
			
			if (bAlpha)
				MeshProp.SetNoMarkFlag(true);

			if (!_CreateSidesFromTriangles(iNumVert, iNumIndex, aVerts, aIndices, MeshProp))
				break;
			
		NextMesh:
			
			//	Get next mesh
			pMesh = l_aFrames[i]->GetNextMesh();
		}
	}

	AddMessageToList("+ %d pairs of triangles were mereged", l_iNumMergeTris);
	AddMessageToList("+ Total %d sides, %d brushes", g_aSides.GetSize(), g_aBrushes.GetSize());
	AddMessageToList("+ Total %d planes", g_aPlanes.GetSize());
}

/*	Create sides from triangles list.

	Return TRUE for success, otherwise return FALSE

	iNumVert: number of vertex.
	iNumIndex: number of index.
	aVerts: vertex array
	aIndices: index array.
	MeshProps: mesh properties
*/
bool _CreateSidesFromTriangles(int iNumVert, int iNumIndex, A3DVERTEX* aVerts, 
							   WORD* aIndices,  const A3DMESHPROP& MeshProps)
{
	int i, j;
	s_SIDE* pSide;
	s_WINDING* pWinding;

	if (g_Configs.bMergeSide && iNumIndex >= 6)	//	Merge triangles when possible
	{
		s_PLANE Plane1, Plane2;
		A3DVECTOR3 Tri1[3], Tri2[3];

		for (i=0; i < iNumIndex; )
		{
			if (i > iNumIndex - 6)
			{
				_CreateSidesFromTriangles(iNumVert, 3, aVerts, &aIndices[i], MeshProps);
				break;
			}

			for (j=0; j < 3; j++)
			{
				Tri1[j].x	= aVerts[aIndices[i+j]].x;
				Tri1[j].y	= aVerts[aIndices[i+j]].y;
				Tri1[j].z	= aVerts[aIndices[i+j]].z;

				Tri2[j].x	= aVerts[aIndices[i+3+j]].x;
				Tri2[j].y	= aVerts[aIndices[i+3+j]].y;
				Tri2[j].z	= aVerts[aIndices[i+3+j]].z;
			}

			MakePlaneFromPoints(Tri1[0], Tri1[1], Tri1[2], &Plane1);
			MakePlaneFromPoints(Tri2[0], Tri2[1], Tri2[2], &Plane2);

			if (!ComparePlane(&Plane1, &Plane2))	//	Planes aren't equal
			{
				_CreateSidesFromTriangles(iNumVert, 3, aVerts, &aIndices[i], MeshProps);
				i += 3;
			}
			else	//	Planes are equal
			{
				WORD aQuad[4], k=0, n, m;
				for (j=0; j < 3; j++)
				{
					//	Search common edge of the two triangles
					if (aIndices[i+j] != aIndices[i+3] &&
						aIndices[i+j] != aIndices[i+4] &&
						aIndices[i+j] != aIndices[i+5])
						aQuad[k++] = aIndices[i+j];
					else
					{
						aQuad[k++] = aIndices[i+j];

						//	Next vertex is also in the second triangle ?
						n = (j+1) % 3;
						if (aIndices[i+n] == aIndices[i+3] ||
							aIndices[i+n] == aIndices[i+4] ||
							aIndices[i+n] == aIndices[i+5])
						{
							//	Search vertex in second triangle which isn't in the first triangle
							for (m=0; m < 3; m++)
							{
								if (aIndices[i+3+m] != aIndices[i] &&
									aIndices[i+3+m] != aIndices[i+1] &&
									aIndices[i+3+m] != aIndices[i+2])
									break;
							}

							if (m == 3)
								break;
							else
								aQuad[k++] = aIndices[i+3+m];
						}
					}
				}

				//	Two triangles don't share edge or merged side isn't convex
				if (k != 4 || !_IsConvexSide(aVerts, aQuad, 4, Plane1.vNormal))
				{
					_CreateSidesFromTriangles(iNumVert, 3, aVerts, &aIndices[i], MeshProps);
					i += 3;
				}
				else	//	Create a quadrangle side
				{
					l_iNumMergeTris++;
					
					pSide	 = _AllocSide();
					pWinding = AllocWinding(4);
					
					//	Copy vertex positions
					for (j=0; j < 4; j++)
					{
						pWinding->pVerts[j].x = aVerts[aQuad[j]].x;
						pWinding->pVerts[j].y = aVerts[aQuad[j]].y;
						pWinding->pVerts[j].z = aVerts[aQuad[j]].z;
					}
					
					pSide->iPlane		= SearchPlane(Plane1.vNormal, Plane1.fDist);
					pSide->pWinding		= pWinding;
					pSide->MeshProps	= MeshProps;

					if (!g_Configs.bNoBrush)
						pSide->iBrush = GenerateBrushForSide(pSide);
					else
						pSide->iBrush = -1;
					
					g_aSides.Add(pSide);

					l_iSideCount += 2;
					StepProgressBar(l_iSideCount, l_iSideTotal);
					UpdateLastMessage("+ Create Sides and Brushes: %d / %d", l_iSideCount, l_iSideTotal);

					//	Does parallel edges exists in this quadrangle ?
			/*		int iParallel = IdentifyQuadSide(aVerts, aQuad, Plane1.vNormal);
					if (iParallel == 1)
						pSide->dwFlags |= SIDEFLAG_13PARALLEL;
					else if (iParallel == 2)
						pSide->dwFlags |= SIDEFLAG_24PARALLEL;
			
					l_iNumParallel += iParallel;
			*/
					i += 6;
				}
			}
		}
	}
	else	//	One triangle one side
	{
		for (i=0; i < iNumIndex; i+=3)
		{
			pSide	 = _AllocSide();
			pWinding = AllocWinding(3);
			
			//	Copy vertex positions
			for (j=0; j < 3; j++)
			{
				pWinding->pVerts[j].x = aVerts[aIndices[i+j]].x;
				pWinding->pVerts[j].y = aVerts[aIndices[i+j]].y;
				pWinding->pVerts[j].z = aVerts[aIndices[i+j]].z;
			}
			
			pSide->iPlane		= MakePlaneFromPoints(pWinding->pVerts[0], pWinding->pVerts[1], pWinding->pVerts[2]);
			pSide->pWinding		= pWinding;
			pSide->MeshProps	= MeshProps;

			if (!g_Configs.bNoBrush)
				pSide->iBrush = GenerateBrushForSide(pSide);
			else
				pSide->iBrush = -1;

			g_aSides.Add(pSide);

			l_iSideCount++;
			StepProgressBar(l_iSideCount, l_iSideTotal);
			UpdateLastMessage("+ Create Sides and Brushes: %d / %d", l_iSideCount, l_iSideTotal);
		}
	}

	return true;
}	

//	Release all sides
void DestroySides()
{
	for (int i=0; i < g_aSides.GetSize(); i++)
		_FreeSide(g_aSides[i]);

	g_aSides.RemoveAll();
}

/*	Check specified side is a convex polygon or not.

	Return TRUE is specified side is convex, otherwise return FALSE.

	aVerts: vertex array
	aIndices: index array
	iNumIdx: number of index
	vNormal: normal of plane side is on.
*/
bool _IsConvexSide(A3DVERTEX* aVerts, WORD* aIndices, int iNumIndex, A3DVECTOR3& vNormal)
{
	s_PLANE Plane;
	A3DVECTOR3 v1, v2, vEdge;
	int i, j, n;

	for (i=0; i < iNumIndex; i++)
	{
		j = (i+1) % iNumIndex;

		v1.x = aVerts[aIndices[i]].x;
		v1.y = aVerts[aIndices[i]].y;
		v1.z = aVerts[aIndices[i]].z;
		v2.x = aVerts[aIndices[j]].x;
		v2.y = aVerts[aIndices[j]].y;
		v2.z = aVerts[aIndices[j]].z;

		vEdge = v2 - v1;

		Plane.vNormal	= Normalize(CrossProduct(vEdge, vNormal));
		Plane.fDist		= DotProduct(Plane.vNormal, v1);

		//	All vertices should behind or on edge plane if side is convex
		for (n=0; n < iNumIndex; n++)
		{
			if (n == i || n == j)
				continue;

			v1.x = aVerts[aIndices[n]].x;
			v1.y = aVerts[aIndices[n]].y;
			v1.z = aVerts[aIndices[n]].z;

			if ((DotProduct(v1, Plane.vNormal) - Plane.fDist) >= 0.001f)
				return false;
		}
	}

	return true;
}

/*	Search parallel edges in a quadrangle.

	Return 0 if no edges parallel in this quadrangle, 1 if 1, 3 edges parallel, 
	2 if 2, 4 edges parallel.

	aVerts: vertices of quadrangle.
	aIndices: index of quadrangle.
	vNormal: normal of plane on which quadrangle lies
*/
int _IdentifyQuadSide(A3DVERTEX* aVerts, WORD* aIndices, A3DVECTOR3& vNormal)
{
	int iRet = 0;

	A3DVECTOR3 v0, v1, v2, v3;
	A3DVECTOR3 vEdgeNormal1, vEdgeNormal2;
	A3DVECTOR3 vEdge1, vEdge2;
	A3DVERTEX* pVert;

	pVert	= &aVerts[aIndices[0]];
	v0		= A3DVECTOR3(pVert->x, pVert->y, pVert->z);
	pVert	= &aVerts[aIndices[1]];
	v1		= A3DVECTOR3(pVert->x, pVert->y, pVert->z);
	pVert	= &aVerts[aIndices[2]];
	v2		= A3DVECTOR3(pVert->x, pVert->y, pVert->z);
	pVert	= &aVerts[aIndices[3]];
	v3		= A3DVECTOR3(pVert->x, pVert->y, pVert->z);

	vEdge1 = v1 - v0;
	vEdge2 = v3 - v2;

	vEdgeNormal1 = Normalize(CrossProduct(vNormal, vEdge1));
	vEdgeNormal2 = Normalize(CrossProduct(vNormal, vEdge2));

	if (DotProduct(vEdgeNormal1, vEdgeNormal2) + 1.0f < 1e-6f)
		iRet++;

	vEdge1 = v2 - v1;
	vEdge2 = v0 - v3; 

	vEdgeNormal1 = Normalize(CrossProduct(vNormal, vEdge1));
	vEdgeNormal2 = Normalize(CrossProduct(vNormal, vEdge2));

	if (DotProduct(vEdgeNormal1, vEdgeNormal2) + 1.0f < 1e-6f)
		iRet++;

	return iRet;
}

/////////////////////////////////////////////////////////////////////////
//
//	Routines used to load world data from .mox, .mod files
//
/////////////////////////////////////////////////////////////////////////

/*	Load frame data from MOX file and class them into clusters, then create sides
	in every cluster.

	Return TRUE for success, otherwise return FALSE.
*/
bool LoadSidesFromMOX(const char* szFileName)
{
	AddMessageToList("+ Load world data from MOX file...");
	SetCurProgressSeg(PROG_BUILDSIDES);

	if (!szFileName || !szFileName[0])
	{
		AddMessageToList("! LoadSidesFromMOX, Invalid parameter");
		return false;
	}

	AFile File;
	A3DFrame Scene;

	if (!File.Open((char*)szFileName, AFILE_OPENEXIST))
	{
		AddMessageToList("! LoadSidesFromMOX, Cannot find specified file");
		return false;
	}

	if (!Scene.Load(NULL, &File))
	{
		AddMessageToList("! LoadSidesFromMOX, Failed to load scene");
		return FALSE;
	}

	File.Close();

	AddMessageToList("+ Create Sides and Brushes ...");

	l_aFrames.Add(&Scene);

	//	Create sides
	_CreateSides();
	
 	Scene.Release();

	//	Release frame array
	l_aFrames.RemoveAll();

	return true;
}

/*	Load frame data from MOD file and class them into clusters, then create sides
	in every cluster.

	Return TRUE for success, otherwise return FALSE.
*/
bool LoadSidesFromMOD(const char* szFileName)
{
	AddMessageToList("+ Load world data from MOD file...");
	SetCurProgressSeg(PROG_BUILDSIDES);

	if (!szFileName || !szFileName[0])
	{
		AddMessageToList("! LoadSidesFromMOD, Invalid parameter");
		return false;
	}

	AFile File;
	A3DModel Model;

	if (!File.Open((char*)szFileName, AFILE_OPENEXIST))
	{
		AddMessageToList("! LoadSidesFromMOD, Cannot find specified file");
		return false;
	}
	
	if (!Model.Load(NULL, &File))
	{
		AddMessageToList("! LoadSidesFromMOD, Failed to load model");
		return false;
	}

	File.Close();

	AddMessageToList("+ Create sides and brushes...\n");

	_ParseA3DModel(&Model);

	//	Create sides
	_CreateSides();
	
 	Model.Release();

	//	Release frame array
	l_aFrames.RemoveAll();

	return true;
}

//	Parse A3D model and read data from it.
void _ParseA3DModel(A3DModel* pModel)
{
	if (!pModel)
		return;

	A3DModel* pChildModel;
	ALISTELEMENT* pElem = pModel->GetChildFrameList()->GetFirst();

	while (pElem != pModel->GetChildFrameList()->GetTail())
		l_aFrames.Add((A3DFrame*)pElem->pData);
	
	//	Handle child models
	pElem = pModel->GetChildModelList()->GetFirst();

	while (pElem != pModel->GetChildModelList()->GetTail())
	{
		pChildModel = (A3DModel*)pElem->pData;
		_ParseA3DModel(pChildModel);
		pElem = pElem->pNext;
	}
}

//	Allocate a new SIDEREF structure and return it's address
s_SIDEREF* AllocSideRef()
{
	s_SIDEREF* pRef = new s_SIDEREF;
	ASSERT(pRef);

	pRef->iSide = -1;
	pRef->pNext = NULL;

	return pRef;
}

//	Destory a s_SIDEREF structure link list.
void DestroySideRefList(s_SIDEREF* pList)
{
	s_SIDEREF* pRef;

	while (pList)
	{
		pRef  = pList;
		pList = pList->pNext;
		delete pRef;
	}
}

int GetSideIndex(s_SIDE* pSide)
{
	for (int i=0; i < g_aSides.GetSize(); i++)
	{
		if (pSide == g_aSides[i])
			return i;
	}

	return -1;
}

