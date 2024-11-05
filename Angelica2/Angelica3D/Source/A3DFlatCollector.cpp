/*
 * FILE: A3DFlatCollector.cpp
 *
 * DESCRIPTION: Flat object collector class
 *
 * CREATED BY: Duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DFlatCollector.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DFuncs.h"
#include "A3DMacros.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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
//	Implement A3DFlatCollector
//
///////////////////////////////////////////////////////////////////////////

A3DFlatCollector::A3DFlatCollector()
{
	m_pA3DEngine	= NULL;
	m_pStream3D		= NULL;
	m_aVertBuf3D	= NULL;
	m_aIdxBuf3D		= NULL;
	m_iVertBufSize	= 0;
	m_iIdxBufSize	= 0;
	m_iVertCnt3D	= 0;
	m_iIdxCnt3D		= 0;
	m_pStream2D		= NULL;
	m_aVertBuf2D	= NULL;
	m_aIdxBuf2D		= NULL;
	m_iVertCnt2D	= 0;
	m_iIdxCnt2D		= 0;
}

//	Initialize wire collector
bool A3DFlatCollector::Init(A3DEngine* pA3DEngine, int iVertBufSize/* 2048 */, int iIdxBufSize/* 8192 */)
{
	m_pA3DEngine	= pA3DEngine;
	m_iVertBufSize	= iVertBufSize;
	m_iIdxBufSize	= iIdxBufSize;

	//	Create stream
	if (!(m_pStream3D = new A3DStream))
	{
		g_A3DErrLog.Log("A3DFlatCollector::Init, Failed to create stream object.");
		return false;
	}

	if (!(m_pStream3D->Init(pA3DEngine->GetA3DDevice(), A3DVT_LVERTEX, m_iVertBufSize, 
		m_iIdxBufSize, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR)))
	{
		g_A3DErrLog.Log("A3DFlatCollector::Init, Failed to create stream object.");
		return false;
	}
	
	if (!(m_pStream2D = new A3DStream))
	{
		g_A3DErrLog.Log("A3DFlatCollector::Init, Failed to create stream object.");
		return false;
	}

	if (!(m_pStream2D->Init(pA3DEngine->GetA3DDevice(), A3DVT_TLVERTEX, m_iVertBufSize, 
		m_iIdxBufSize, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR)))
	{
		g_A3DErrLog.Log("A3DFlatCollector::Init, Failed to create stream object.");
		return false;
	}

	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	if (pA3DDevice)
		pA3DDevice->AddUnmanagedDevObject(this);

	return true;
}

//	Release wire collector
void A3DFlatCollector::Release()
{
	if (m_pA3DEngine)
	{
		A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
		if (pA3DDevice)
			pA3DDevice->RemoveUnmanagedDevObject(this);
	}

	if (m_pStream3D)
	{
		if (m_aVertBuf3D)		//	Stream still be locked ! this shouldn't happen
			LockStream_3D(false);
	
		A3DRELEASE(m_pStream3D);
	}

	if (m_pStream2D)
	{
		if (m_aVertBuf2D)		//	Stream still be locked ! this shouldn't happen
			LockStream_2D(false);
	
		A3DRELEASE(m_pStream2D);
	}
	
	m_pA3DEngine	= NULL;
	m_aVertBuf3D	= NULL;
	m_aIdxBuf3D		= NULL;
	m_aVertBuf2D	= NULL;
	m_aIdxBuf2D		= NULL;
}

//	Add vertex and index to render buffer
bool A3DFlatCollector::AddRenderData_3D(const A3DVECTOR3* aVerts, int iNumVert, 
							const WORD* aIndices, int iNumIdx, DWORD dwCol)
{
	ASSERT(m_pA3DEngine && m_pStream3D);

	if (iNumVert > m_iVertBufSize || iNumIdx > m_iIdxBufSize)
	{
		ASSERT(iNumVert <= m_iVertBufSize && iNumIdx <= m_iIdxBufSize);
		return false;
	}

	//	Vertex buffer has been full ?
	if (m_aVertBuf3D)
	{
		if (m_iVertCnt3D+iNumVert > m_iVertBufSize || m_iIdxCnt3D+iNumIdx > m_iIdxBufSize)
			Flush_3D();
	}

	//	Stream hasn't been locked, lock it.
	if (!m_aVertBuf3D)
	{
		if (!LockStream_3D(true))
		{
			g_A3DErrLog.Log("A3DFlatCollector::AddRenderData_3D, Failed to lock stream!");
			return false;
		}
	}

	A3DLVERTEX* pVert;
	WORD* pIndex;
	int i;
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	pVert	= &m_aVertBuf3D[m_iVertCnt3D];
	pIndex	= &m_aIdxBuf3D[m_iIdxCnt3D];

	//	Fill vertex buffer
	for (i=0; i < iNumVert; i++, pVert++)
	{
		pVert->x	= aVerts[i].x;
		pVert->y	= aVerts[i].y;
		pVert->z	= aVerts[i].z;
		pVert->tu	= 0.0f;
		pVert->tv	= 0.0f;

		pVert->specular	= Specular;
		pVert->diffuse	= dwCol;
	}

	//	Fill index buffer
	for (i=0; i < iNumIdx; i++, pIndex++)
		*pIndex = m_iVertCnt3D + aIndices[i];

	m_iVertCnt3D	+= iNumVert;
	m_iIdxCnt3D		+= iNumIdx;

	return true;
}

//	Add a box to render buffer
bool A3DFlatCollector::AddBox_3D(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, 
					const A3DVECTOR3& vRight, const A3DVECTOR3& vExts, DWORD dwCol, const A3DMATRIX4* pMat/* NULL */)
{
	//	Indices for flat box
	static const WORD aFlatIndices[36] =
	{
		0, 1, 2, 0, 2, 3, 
		1, 5, 6, 1, 6, 2,
		2, 6, 7, 2, 7, 3,
		3, 7, 4, 3, 4, 0,
		4, 5, 1, 4, 1, 0,
		5, 7, 6, 5, 4, 7,
	};

	float fx2 = vExts.x * 2.0f;
	float fy2 = vExts.y * 2.0f;
	float fz2 = vExts.z * 2.0f;

	A3DVECTOR3 aVerts[8];

	aVerts[0] = vPos + vDir * vExts.z + vUp * vExts.y - vRight * vExts.x;
	aVerts[1] = aVerts[0] + vRight * fx2;
	aVerts[2] = aVerts[1] - vDir * fz2;
	aVerts[3] = aVerts[0] - vDir * fz2;
	aVerts[4] = aVerts[0] - vUp * fy2;
	aVerts[5] = aVerts[4] + vRight * fx2;
	aVerts[6] = aVerts[5] - vDir * fz2;
	aVerts[7] = aVerts[4] - vDir * fz2;

	if (pMat)
	{
		for (int i=0; i < 8; i++)
			aVerts[i] = aVerts[i] * (*pMat);
	}

	//	Push to render buffer
	return AddRenderData_3D(aVerts, 8, aFlatIndices, 36, dwCol);
}

//	Add a AABB to render buffer
bool A3DFlatCollector::AddAABB_3D(const A3DAABB& aabb, DWORD dwCol, const A3DMATRIX4* pMat/* NULL */)
{
	return AddBox_3D(aabb.Center, A3D::g_vAxisZ, A3D::g_vAxisY, A3D::g_vAxisX, aabb.Extents, dwCol, pMat);
}

//	Add a OBB to render buffer
bool A3DFlatCollector::AddOBB_3D(const A3DOBB& obb, DWORD dwCol, const A3DMATRIX4* pMat/* NULL */)
{
	return AddBox_3D(obb.Center, obb.ZAxis, obb.YAxis, obb.XAxis, obb.Extents, dwCol, pMat);
}

//	Add vertex and index to render buffer
bool A3DFlatCollector::AddRenderData_2D(const A3DVECTOR3* aVerts, int iNumVert, const WORD* aIndices, int iNumIdx, DWORD dwCol)
{
	ASSERT(m_pA3DEngine && m_pStream2D);

	if (iNumVert > m_iVertBufSize || iNumIdx > m_iIdxBufSize)
	{
		ASSERT(iNumVert <= m_iVertBufSize && iNumIdx <= m_iIdxBufSize);
		return false;
	}

	//	Vertex buffer has been full ?
	if (m_aVertBuf2D)
	{
		if (m_iVertCnt2D+iNumVert > m_iVertBufSize || m_iIdxCnt2D+iNumIdx > m_iIdxBufSize)
			Flush_2D();
	}

	//	Stream hasn't been locked, lock it.
	if (!m_aVertBuf2D)
	{
		if (!LockStream_2D(true))
		{
			g_A3DErrLog.Log("A3DFlatCollector::AddRenderData_2D, Failed to lock stream!");
			return false;
		}
	}

	A3DTLVERTEX* pVert;
	WORD* pIndex;
	int i;
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	pVert	= &m_aVertBuf2D[m_iVertCnt2D];
	pIndex	= &m_aIdxBuf2D[m_iIdxCnt2D];

	//	Fill vertex buffer
	for (i=0; i < iNumVert; i++, pVert++)
	{
		pVert->x	= aVerts[i].x;
		pVert->y	= aVerts[i].y;
		pVert->z	= aVerts[i].z;
		pVert->rhw	= 1.0f;
		pVert->tu	= 0.0f;
		pVert->tv	= 0.0f;

		pVert->specular	= Specular;
		pVert->diffuse	= dwCol;
	}

	//	Fill index buffer
	for (i=0; i < iNumIdx; i++, pIndex++)
		*pIndex = m_iVertCnt2D + aIndices[i];

	m_iVertCnt2D	+= iNumVert;
	m_iIdxCnt2D		+= iNumIdx;

	return true;
}

//	Add a rectangle to render buffer
bool A3DFlatCollector::AddRect_2D(int l, int t, int r, int b, DWORD dwCol, float fz/* 0.0f */)
{
	//	Indices for flat box
	static const WORD aFlatIndices[6] =
	{
		0, 1, 2, 0, 2, 3, 
	};

	A3DVECTOR3 aVerts[4];
	aVerts[0].x	= (float)l;
	aVerts[0].y	= (float)t;
	aVerts[0].z	= fz;

	aVerts[1].x	= (float)r;
	aVerts[1].y	= (float)t;
	aVerts[1].z	= fz;

	aVerts[2].x	= (float)r;
	aVerts[2].y	= (float)b;
	aVerts[2].z	= fz;

	aVerts[3].x	= (float)l;
	aVerts[3].y	= (float)b;
	aVerts[3].z	= fz;

	return AddRenderData_2D(aVerts, 4, aFlatIndices, 6, dwCol);
}

//	Lock/Unlock stream
bool A3DFlatCollector::LockStream_3D(bool bLock)
{
	if (bLock)
	{
		if (!m_pStream3D->LockVertexBuffer(0, 0, (BYTE**)&m_aVertBuf3D, 0))
			return false;

		if (!m_pStream3D->LockIndexBuffer(0, 0, (BYTE**)&m_aIdxBuf3D, 0))
			return false;

		m_iVertCnt3D	= 0;
		m_iIdxCnt3D		= 0;
	}
	else
	{
		if (!m_pStream3D->UnlockVertexBuffer())
			return false;

		if (!m_pStream3D->UnlockIndexBuffer())
			return false;

		m_aVertBuf3D	= NULL;
		m_aIdxBuf3D		= NULL;
	}

	return true;
}

//	Lock / Unlock stream
bool A3DFlatCollector::LockStream_2D(bool bLock)
{
	if (bLock)
	{
		if (!m_pStream2D->LockVertexBuffer(0, 0, (BYTE**)&m_aVertBuf2D, 0))
			return false;

		if (!m_pStream2D->LockIndexBuffer(0, 0, (BYTE**)&m_aIdxBuf2D, 0))
			return false;

		m_iVertCnt2D	= 0;
		m_iIdxCnt2D		= 0;
	}
	else
	{
		if (!m_pStream2D->UnlockVertexBuffer())
			return false;

		if (!m_pStream2D->UnlockIndexBuffer())
			return false;

		m_aVertBuf2D	= NULL;
		m_aIdxBuf2D		= NULL;
	}

	return true;
}

//	Flush sector
bool A3DFlatCollector::Flush_3D()
{
	if (m_aVertBuf3D)
		LockStream_3D(false);

	if (!m_iVertCnt3D || !m_iIdxCnt3D)
		return true;

	//	Set render states
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetDeviceVertexShader(A3DFVF_A3DLVERTEX);
	pA3DDevice->ClearTexture(0);

	pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
	m_pStream3D->Appear();

	//	Render
	if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iVertCnt3D, 0, m_iIdxCnt3D / 3))
		return false;

	//	Restore render state
	pA3DDevice->SetLightingEnable(true);

	m_iVertCnt3D	= 0;
	m_iIdxCnt3D		= 0;

	return true;
}

//	Flush sector
bool A3DFlatCollector::Flush_2D()
{
	if (m_aVertBuf2D)
		LockStream_2D(false);

	if (!m_iVertCnt2D || !m_iIdxCnt2D)
		return true;

	//	Set render states
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetDeviceVertexShader(A3DFVF_A3DTLVERTEX);
	pA3DDevice->ClearTexture(0);

	m_pStream2D->Appear();

	//	Render
	if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iVertCnt2D, 0, m_iIdxCnt2D / 3))
		return false;

	//	Restore render state
	pA3DDevice->SetLightingEnable(true);

	m_iVertCnt2D	= 0;
	m_iIdxCnt2D		= 0;

	return true;
}

//	Before device reset
bool A3DFlatCollector::BeforeDeviceReset()
{
	m_aVertBuf3D	= NULL;
	m_aIdxBuf3D		= NULL;
	m_iVertCnt3D	= 0;
	m_iIdxCnt3D		= 0;
	m_aVertBuf2D	= NULL;
	m_aIdxBuf2D		= NULL;
	m_iVertCnt2D	= 0;
	m_iIdxCnt2D		= 0;
	return true;
}

