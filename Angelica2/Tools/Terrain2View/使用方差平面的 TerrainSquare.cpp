/*
 * FILE: TerrainSquare.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/7/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include <float.h>
#include "TerrainSquare.h"
#include "Render.h"

#include "A3DTerrain2.h"
#include "A3DTerrain2Blk.h"
#include "A3DMacros.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CBlockSquare
//	
///////////////////////////////////////////////////////////////////////////

CBlockSquare::CBlockSquare(CTerrainSquare* pTrnSquare)
{
	m_pTrnSquare = pTrnSquare;
}

CBlockSquare::~CBlockSquare()
{
}

//	Build block square from terrain2 block data
bool CBlockSquare::Build(A3DTerrain2Block* pBlock)
{
	//	Calculate least square sum
	CalcLeastSquareSum(pBlock);

	//	Calculate least square plane
	CalcLeastSquarePlane(pBlock);

	//	Calculate quad
	CalcQuad(pBlock, PT_MINIMUM, m_MinQuad);
	CalcQuad(pBlock, PT_MAXIMUM, m_MaxQuad);

	return true;
}

//	Get offset between block's center to origin
A3DVECTOR3 CBlockSquare::GetBlockOffset(A3DTerrain2Block* pBlock)
{
	A3DTerrain2* pTerrain = pBlock->GetTerrain();
	float fBlockSize = pBlock->GetBlockSize();

	const ARectF& rcTerrain = pTerrain->GetTerrainArea();
	float fOffx = rcTerrain.left + pBlock->GetColInTerrain() * fBlockSize + fBlockSize * 0.5f;
	float fOffz = rcTerrain.top - pBlock->GetRowInTerrain() * fBlockSize - fBlockSize * 0.5f;

	return A3DVECTOR3(fOffx, 0.0f, fOffz);
}

//	Calculate least square sum
void CBlockSquare::CalcLeastSquareSum(A3DTerrain2Block* pBlock)
{
	m_Sum.Clear();

    const int n = pBlock->GetBlockGrid() + 1;
    m_Sum.samples = (float)(n * n);

	//	Note: vertices in block are not ordered from top to bottom, from 
	//		left to right, but we can ignore vertex order because we just
	//		add them together
	int i, j, cnt=0;

	A3DVECTOR3 vOff = GetBlockOffset(pBlock);

	for (i=0; i < n; i++)
	{
		for (j=0; j < n; j++)
		{
			A3DVECTOR3 v = pBlock->GetVertexPos(cnt++);
			v.x -= vOff.x;
			v.z -= vOff.z;

            m_Sum.x += v.x;
            m_Sum.y += v.y;
            m_Sum.z += v.z;
            m_Sum.xx += v.x * v.x;
            m_Sum.xy += v.x * v.y;
            m_Sum.xz += v.x * v.z;
            m_Sum.zz += v.z * v.z;
            m_Sum.yz += v.y * v.z;
		}
	}
}

//	Calculate lease square plane
void CBlockSquare::CalcLeastSquarePlane(A3DTerrain2Block* pBlock)
{
	//	Calculate least square plane normal
	A3DVECTOR3 vCenter;
	CalcLeastSquarePlaneNormal(vCenter);

	m_Plane.d = vCenter.x * m_Plane.fNormalX + vCenter.y + vCenter.z * m_Plane.fNormalZ;

    float fLowest = FLT_MAX;
    float fHighest = -FLT_MAX;

    const int n = pBlock->GetBlockGrid() + 1;
	int i, j, cnt=0;

	A3DVECTOR3 vOff = GetBlockOffset(pBlock);

	for (i=0; i < n; i++)
	{
		for (j=0; j < n; j++)
		{
			//	note: fNormalY = 1
			A3DVECTOR3 v = pBlock->GetVertexPos(cnt++);
			v.x -= vOff.x;
			v.z -= vOff.z;

			float e = (v.x * m_Plane.fNormalX + v.y + v.z * m_Plane.fNormalZ);

            if (e < fLowest)
				fLowest = e;

            if (e > fHighest)
				fHighest = e;
		}
	}

//	m_Plane.fError = a_Max(fHighest - m_Plane.d, -(fLowest - m_Plane.d));
	m_Plane.fError1 = -(fLowest - m_Plane.d);
	m_Plane.fError2 = fHighest - m_Plane.d;
}

//	Calculate least square plane normal
void CBlockSquare::CalcLeastSquarePlaneNormal(A3DVECTOR3& vCenter)
{
	float denominator = 
		-2 * m_Sum.x * m_Sum.xz * m_Sum.z + 
		m_Sum.xx * m_Sum.z * m_Sum.z + 
		m_Sum.x * m_Sum.x * m_Sum.zz + 
		m_Sum.samples * (m_Sum.xz * m_Sum.xz - m_Sum.xx * m_Sum.zz);

    float inverse_denominator = 1.0f / denominator;

    float a = 
		m_Sum.xy * (-m_Sum.z * m_Sum.z + m_Sum.samples * m_Sum.zz) - 
		m_Sum.samples * m_Sum.xz * m_Sum.yz + m_Sum.x * m_Sum.z * m_Sum.yz + 
		(m_Sum.xz * m_Sum.z - m_Sum.x * m_Sum.zz) * m_Sum.y;

    float b = 
		-m_Sum.x * m_Sum.x * m_Sum.yz + 
		m_Sum.samples * (-m_Sum.xz * m_Sum.xy + m_Sum.xx * m_Sum.yz) - 
		m_Sum.xx * m_Sum.y * m_Sum.z + 
		m_Sum.x * (m_Sum.xy * m_Sum.z + m_Sum.xz * m_Sum.y);

    m_Plane.fNormalX = a * inverse_denominator;
    m_Plane.fNormalZ = b * inverse_denominator;
	//	note: fNormalY = 1

    float fInvSamples = 1.0f / m_Sum.samples;
    vCenter.x = m_Sum.x * fInvSamples;
    vCenter.y = m_Sum.y * fInvSamples;
    vCenter.z = m_Sum.z * fInvSamples;
}

//	Calculate quad
void CBlockSquare::CalcQuad(A3DTerrain2Block* pBlock, int iPlaneType, QUAD& quad)
{
//	const A3DAABB& aabb = pBlock->GetBlockAABB();
	A3DAABB aabb = pBlock->GetBlockAABB();
	A3DVECTOR3 vOff = GetBlockOffset(pBlock);
	aabb.Mins.x -= vOff.x;
	aabb.Maxs.x -= vOff.x;
	aabb.Mins.z -= vOff.z;
	aabb.Maxs.z -= vOff.z;
	aabb.CompleteCenterExts();

	//	Calculate quad plane d
	float quad_d = m_Plane.d;
	if (iPlaneType == PT_MINIMUM)
		quad_d -= m_Plane.fError1;
	else if (iPlaneType == PT_MAXIMUM)
		quad_d += m_Plane.fError2;

	// v1 v2
	// v4 v3
	float x1_nx = aabb.Mins.x * m_Plane.fNormalX;
	float z1_nz = aabb.Maxs.z * m_Plane.fNormalZ;
	float x2_nx = aabb.Maxs.x * m_Plane.fNormalX;
	float z2_nz = aabb.Mins.z * m_Plane.fNormalZ;

	quad.v1.x = aabb.Mins.x;
	quad.v1.y = quad_d - x1_nx - z1_nz;
	quad.v1.z = aabb.Maxs.z;

	quad.v2.x = aabb.Maxs.x;
	quad.v2.y = quad_d - x2_nx - z1_nz;
	quad.v2.z = aabb.Maxs.z;

	quad.v3.x = aabb.Maxs.x;
	quad.v3.y = quad_d - x2_nx - z2_nz;
	quad.v3.z = aabb.Mins.z;

	quad.v4.x = aabb.Mins.x;
	quad.v4.y = quad_d - x1_nx - z2_nz;
	quad.v4.z = aabb.Mins.z;
}

//	Save square data to file
bool CBlockSquare::Save(FILE* fp)
{
	HORIFILEBLKSQUARE data;
	memset(&data, 0, sizeof (data));

	data.y1	= m_MinQuad.v1.y;
	data.y2	= m_MinQuad.v2.y;
	data.y3	= m_MinQuad.v3.y;
	data.y4	= m_MinQuad.v4.y;
//	data.fOffsetY = m_Plane.fError * 2.0f;
	data.fOffsetY = m_Plane.fError1 + m_Plane.fError2;

	fwrite(&data, sizeof (data), 1, fp);

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CTerrainSquare
//	
///////////////////////////////////////////////////////////////////////////

CTerrainSquare::CTerrainSquare()
{
	m_fStepSize	= 512.0f;
	m_pProgress	= NULL;
}

CTerrainSquare::~CTerrainSquare()
{
}

//	Release object
void CTerrainSquare::Release()
{
	int i;

	//	Release all squares
	for (i=0; i < m_aBlkSquares.GetSize(); i++)
	{
		if (m_aBlkSquares[i])
			delete m_aBlkSquares[i];
	}

	m_aBlkSquares.RemoveAll();

	A3DRELEASE(m_pTerrain);
}

//	Build terrain squares from terrain2 file
bool CTerrainSquare::Build(const char* szFile, CProgressCtrl* pProgress/* NULL */)
{
	if (!LoadTerrain(szFile))
		return false;

	ResetBlockSquares();
		
	m_pProgress = pProgress;
	if (m_pProgress)
	{
		m_pProgress->SetRange(0, m_pTerrain->GetBlockNum());
		m_pProgress->SetStep(1);
		m_pProgress->SetPos(0);
	}

	const ARectF& rcTerrain = m_pTerrain->GetTerrainArea();
	float z = rcTerrain.top - m_fStepSize;

	for ( ; ; z -= m_fStepSize)
	{
		float x = rcTerrain.left + m_fStepSize;

		for ( ; ; x += m_fStepSize)
		{
			A3DVECTOR3 vCenter(x, 0.0f, z);
			m_pTerrain->SetLoadCenter(vCenter);

			//	Build block squares in current active area
			BuildBlockSquares();

			if (x + m_fStepSize >= rcTerrain.right)
				break;
		}

		if (z - m_fStepSize <= rcTerrain.bottom)
			break;
	}

	//	Ensure all block squares have been built
	for (int i=0; i < m_aBlkSquares.GetSize(); i++)
	{
		if (!m_aBlkSquares[i])
		{
			ASSERT(m_aBlkSquares[i]);
			return false;
		}
	}

	return true;
}

//	Load terrain
bool CTerrainSquare::LoadTerrain(const char* szFile)
{
	m_pTerrain = new A3DTerrain2;

	if (!m_pTerrain->Init(g_Render.GetA3DEngine(), m_fStepSize, m_fStepSize))
	{
		delete m_pTerrain;
		a_LogOutput(1, "CTerrainSquare::LoadTerrain", "Failed to initialize terrain");
		return false;
	}

	if (!m_pTerrain->Load(szFile, 0.0f, 0.0f))
	{
		m_pTerrain->Release();
		delete m_pTerrain;
		a_LogOutput(1, "CTerrainSquare::LoadTerrain, Failed to load A3DTerrain2.");
		return false;
	}

	return true;
}

//	Reset block squares
void CTerrainSquare::ResetBlockSquares()
{
	int i;

	//	Release all squares
	for (i=0; i < m_aBlkSquares.GetSize(); i++)
	{
		if (m_aBlkSquares[i])
			delete m_aBlkSquares[i];
	}

	m_aBlkSquares.RemoveAll();

	int iNumBlock = m_pTerrain->GetBlockNum();
	m_aBlkSquares.SetSize(iNumBlock, 32);

	for (i=0; i < iNumBlock; i++)
		m_aBlkSquares[i] = NULL;
}

//	Build block squares in current active area
bool CTerrainSquare::BuildBlockSquares()
{
	A3DTerrain2::ACTBLOCKS* pActArea = m_pTerrain->GetActiveBlocks();
	const ARectI& rcArea = pActArea->rcArea;
	int r, c;

	for (r=rcArea.top; r < rcArea.bottom; r++)
	{
		for (c=rcArea.left; c < rcArea.right; c++)
		{
			int iIndex = r * m_pTerrain->GetBlockColNum() + c;
			if (m_aBlkSquares[iIndex])
				continue;

			CBlockSquare* pBlockSquare = new CBlockSquare(this);
			if (!pBlockSquare->Build(pActArea->GetBlock(r, c, false)))
			{
				delete pBlockSquare;
				a_LogOutput(1, "CTerrainSquare::BuildBlockSquares, Failed to build block (%d, %d) square.", r, c);
				return false;
			}

			m_aBlkSquares[iIndex] = pBlockSquare;

			if (m_pProgress)
				m_pProgress->StepIt();
		}
	}

	return true;
}

//	Save square data to file
bool CTerrainSquare::Save(const char* szFile)
{
	FILE* fp = fopen(szFile, "wb+");
	if (!fp)
	{
		a_LogOutput(1, "CTerrainSquare::Save, Failed to create file %s.", szFile);
		return false;
	}

	//	Write file flag and version
	DWORD dwTemp = HORIFILE_IDENTIFY;
	fwrite(&dwTemp, sizeof (dwTemp), 1, fp);
	dwTemp = HORIFILE_VERSION;
	fwrite(&dwTemp, sizeof (dwTemp), 1, fp);

	//	Write file header
	HORIFILEHEADER fh;
	memset(&fh, 0, sizeof (fh));
	fh.iNumBlockRow	= m_pTerrain->GetBlockRowNum();
	fh.iNumBlockCol	= m_pTerrain->GetBlockColNum();
	fwrite(&fh, sizeof (fh), 1, fp);

	//	Write block squares
	for (int i=0; i < m_aBlkSquares.GetSize(); i++)
	{
		CBlockSquare* pBlkSquare = m_aBlkSquares[i];
		ASSERT(pBlkSquare);
		pBlkSquare->Save(fp);
	}

	fclose(fp);
	return true;
}


