/*
 * FILE: A3DLitModel.cpp
 *
 * DESCRIPTION: Lit static model in Angelica Engine, usually used for building
 *
 * CREATED BY: Hedi, 2004/11/5
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DTypes.h"
#include "A3DFuncs.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "A3DObject.h"
#include "A3DTexture.h"
#include "A3DTextureMan.h"
#include "A3DPixelShader.h"
#include "A3DShaderMan.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DStream.h"
#include "A3DFrame.h"
#include "A3DMesh.h"
#include "A3DCamera.h"
#include "A3DCollision.h"
#include "A3DPI.h"
#include "AF.h"
#include "A3DShadowMap.h"
#include "A3DLitModelRender.h"

#include "A3DLitModel.h"

//////////////////////////////////////////////////////////////////////////
// class A3DLitMesh
//	
//		A mesh that was pre-lit
//////////////////////////////////////////////////////////////////////////

A3DLitMesh::A3DLitMesh(A3DLitModel * pLitModel)
{
	m_pParentModel			= pLitModel;

	m_pA3DDevice			= NULL;
	m_pA3DTexture			= NULL;
	m_pA3DReflectTexture	= NULL;
	
	m_pVerts				= NULL;
	m_pIndices				= NULL;
	m_pNormals				= NULL;

	m_pDayColors			= NULL;
	m_pNightColors			= NULL;
	m_fDNFactor				= -1.0f;

	m_bHasExtraColors		= false;
	m_pDayColorsExtra		= NULL;
	m_pNightColorsExtra		= NULL;

	memset(m_szTextureMap, 0, sizeof(m_szTextureMap));
	
	m_nVertCount			= 0;
	m_nFaceCount			= 0;

	m_bVisible				= true;
	m_nAlphaValue			= 255;

	m_vDisToCam				= 999999.0f;

	m_bSupportLM			= false;
	m_pLMCoords				= NULL;

#ifdef LM_BUMP_ENABLE
	m_pNormalMap			= NULL;
	memset(m_szNormalMapPath, 0, sizeof(m_szNormalMapPath));
	memset(&m_paramDay, 0, sizeof(A3DLIGHTPARAM));
	memset(&m_paramNight, 0, sizeof(A3DLIGHTPARAM));
#endif
}

A3DLitMesh::~A3DLitMesh()
{
	Release();
}

// Create/Release a lit mesh
bool A3DLitMesh::Create(A3DDevice * pA3DDevice, int nVertCount, int nFaceCount, const char * szTextureMap, bool bSupportLM)
{
	m_pA3DDevice		= pA3DDevice;
	m_nVertCount		= nVertCount;
	m_nFaceCount		= nFaceCount;
	strncpy(m_szTextureMap, szTextureMap, 256);

	if( !CreateVertexBuffers() )
	{
		g_A3DErrLog.Log("A3DLitMesh::Create(), failed to call CreateVertexBuffers()");
		return false;
	}

	if( !LoadTexture() )
	{
		g_A3DErrLog.Log("A3DLitMesh::Create(), failed to load texture!");
		return false;
	}

	A3DMATERIALPARAM param;
	memset(&param, 0, sizeof(param));
	param.Diffuse = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	param.Ambient = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	param.Specular = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	param.Emissive = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	param.Power = 0.0f;
	m_Material.Init(pA3DDevice, param);

	//Light map
	m_bSupportLM = bSupportLM;
	if( bSupportLM)
	{
		if(!CreateLMCoords())
		{
			return false;
		}
	}

	return true;
}

bool A3DLitMesh::Release()
{
	ReleaseTexture();
	ReleaseVertexBuffers();
	ReleaseLMCoords();
	return true;
}

bool A3DLitMesh::MergeMesh(A3DLitMesh * pMeshToBeMerged)
{
	int nNewVertCount = m_nVertCount + pMeshToBeMerged->GetNumVerts();
	int nNewFaceCount = m_nFaceCount + pMeshToBeMerged->GetNumFaces();

	A3DLMVERTEX *		pVerts;
	WORD *				pIndices;
	A3DVECTOR3 *		pNormals;
	A3DCOLOR *			pDayColors;
	A3DCOLOR *			pNightColors;

	pVerts = new A3DLMVERTEX[nNewVertCount];
	pIndices = new WORD[nNewFaceCount * 3];
	pNormals = new A3DVECTOR3[nNewVertCount];
	pDayColors = new A3DCOLOR[nNewVertCount];
	pNightColors = new A3DCOLOR[nNewVertCount];

	memcpy(pVerts, m_pVerts, sizeof(A3DLMVERTEX) * m_nVertCount);
	memcpy(pVerts + m_nVertCount, pMeshToBeMerged->GetVerts(), sizeof(A3DLMVERTEX) * pMeshToBeMerged->GetNumVerts());

	memcpy(pNormals, m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount);
	memcpy(pNormals + m_nVertCount, pMeshToBeMerged->GetNormals(), sizeof(A3DVECTOR3) * pMeshToBeMerged->GetNumVerts());

	memcpy(pDayColors, m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount);
	memcpy(pDayColors + m_nVertCount, pMeshToBeMerged->GetDayColors(), sizeof(A3DCOLOR) * pMeshToBeMerged->GetNumVerts());

	memcpy(pNightColors, m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount);
	memcpy(pNightColors + m_nVertCount, pMeshToBeMerged->GetNightColors(), sizeof(A3DCOLOR) * pMeshToBeMerged->GetNumVerts());

	memcpy(pIndices, m_pIndices, sizeof(WORD) * m_nFaceCount * 3);
	WORD * pIndices2 = pMeshToBeMerged->GetIndices();
	int nNumIndices1 = m_nFaceCount * 3;
	int nNumIndices2 = pMeshToBeMerged->GetNumFaces() * 3;
	for(int i=0; i<nNumIndices2; i++)
		pIndices[i + nNumIndices1] = pIndices2[i] + m_nVertCount;
	
	ReleaseVertexBuffers();

	//merged light map coords
	if( m_bSupportLM && pMeshToBeMerged->IsSupportLightMap())
	{
		A3DLIGHTMAPCOORD* pLMCoords = new A3DLIGHTMAPCOORD[nNewVertCount];
		memcpy(pLMCoords, m_pLMCoords, sizeof(A3DLIGHTMAPCOORD) * m_nVertCount);
		memcpy(pLMCoords + m_nVertCount, pMeshToBeMerged->GetLightMapCoords(), sizeof(A3DLIGHTMAPCOORD)* pMeshToBeMerged->GetNumVerts());
		ReleaseLMCoords();
	}

	m_pVerts = pVerts;
	m_pIndices = pIndices;
	m_pNormals = pNormals;
	m_pDayColors = pDayColors;
	m_pNightColors = pNightColors;

	m_nVertCount = nNewVertCount;
	m_nFaceCount = nNewFaceCount;

	m_aabb.Merge(pMeshToBeMerged->GetAABB());

	

	return true;
}

bool A3DLitMesh::LoadTexture()
{
	// first release old texture
	ReleaseTexture();

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szTextureMap, &m_pA3DTexture) )
	{
		g_A3DErrLog.Log("A3DLitMesh::LoadTexture(), failed to load texture [%s]!", m_szTextureMap);
		return false;
	}

	// try to load fake reflect texture;
	if( strlen(m_szTextureMap) > 4 && strstr(m_szTextureMap, "nrf_") )
	{
		char szFileName[MAX_PATH];
		af_GetFileTitle(m_szTextureMap, szFileName, MAX_PATH);
		sprintf(m_szReflectTextureMap, "shaders\\textures\\nrf\\%s", szFileName + 1);  
		if( !af_IsFileExist(m_szReflectTextureMap) )
			strcpy(m_szReflectTextureMap, "shaders\\textures\\nrf\\rf_default.dds");
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szReflectTextureMap, &m_pA3DReflectTexture) )
		{
			g_A3DErrLog.Log("A3DLitMesh::LoadTexture(), failed to load texture [%s]!", m_szReflectTextureMap);
			return false;
		}
	}

#ifdef LM_BUMP_ENABLE
	if( strlen(m_szTextureMap) > 4 )
	{
		strncpy(m_szNormalMapPath, m_szTextureMap, 256);
		char szExt[5];
		strncpy(szExt, m_szNormalMapPath + strlen(m_szNormalMapPath) - 4, 5);
		m_szNormalMapPath[strlen(m_szNormalMapPath) - 4] = '\0';
		strcat(m_szNormalMapPath, "_normal");
		strcat(m_szNormalMapPath, szExt);
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szNormalMapPath/*"textures\\dev\\bumptest.tga"*/, &m_pNormalMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::LoadTexture(), failed to load texture [%s]!", m_szNormalMapPath);
			return false;
		}
	}
#endif

	return true;
}

bool A3DLitMesh::ReleaseTexture()
{
	if( m_pA3DTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
		m_pA3DTexture = NULL;
	}

	if( m_pA3DReflectTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DReflectTexture);
		m_pA3DReflectTexture = NULL;
	}

#ifdef LM_BUMP_ENABLE
	if( m_pNormalMap )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pNormalMap);
		m_pNormalMap = NULL;
	}
#endif

	return true;
}

bool A3DLitMesh::CreateVertexBuffers()
{
	bool bHasExtraColors = m_bHasExtraColors;

	// try to release old ones.
	ReleaseVertexBuffers();

	m_pVerts = new A3DLMVERTEX[m_nVertCount];
	m_pIndices = new WORD[m_nFaceCount * 3];
	m_pNormals = new A3DVECTOR3[m_nVertCount];
	m_pDayColors = new A3DCOLOR[m_nVertCount];
	m_pNightColors = new A3DCOLOR[m_nVertCount];

	memset(m_pVerts, 0, sizeof(A3DLMVERTEX) * m_nVertCount);
	memset(m_pIndices, 0, sizeof(WORD) * m_nFaceCount * 3);
	memset(m_pNormals, 0, sizeof(A3DVECTOR3) * m_nVertCount);
	memset(m_pDayColors, 0, sizeof(A3DCOLOR) * m_nVertCount);
	memset(m_pNightColors, 0, sizeof(A3DCOLOR) * m_nVertCount);

	if (bHasExtraColors)
	{
		m_bHasExtraColors = bHasExtraColors;
		m_pDayColorsExtra = new A3DCOLOR[m_nVertCount];
		m_pNightColorsExtra = new A3DCOLOR[m_nVertCount];
		memset(m_pDayColorsExtra, 0, sizeof(A3DCOLOR) * m_nVertCount);
		memset(m_pNightColorsExtra, 0, sizeof(A3DCOLOR) * m_nVertCount);
	}

	return true;
}

bool A3DLitMesh::ReleaseVertexBuffers()
{
	if( m_pNightColors )
	{
		delete [] m_pNightColors;
		m_pNightColors = NULL;
	}

	if( m_pDayColors )
	{
		delete [] m_pDayColors;
		m_pDayColors = NULL;
	}

	if( m_pNormals )
	{
		delete [] m_pNormals;
		m_pNormals = NULL;
	}

	if( m_pIndices )
	{
		delete [] m_pIndices;
		m_pIndices = NULL;
	}

	if( m_pVerts )
	{
		delete [] m_pVerts;
		m_pVerts = NULL;
	}

	if (m_bHasExtraColors)
	{
		m_bHasExtraColors = false;

		if( m_pNightColorsExtra )
		{
			delete [] m_pNightColorsExtra;
			m_pNightColorsExtra = NULL;
		}
		
		if( m_pDayColorsExtra )
		{
			delete [] m_pDayColorsExtra;
			m_pDayColorsExtra = NULL;
		}
	}

	return true;
}

bool A3DLitMesh::CreateLMCoords()
{
	ReleaseLMCoords();
	m_pLMCoords = new A3DLIGHTMAPCOORD[m_nVertCount];
	memset(m_pVerts, 0, sizeof(A3DLMVERTEX) * m_nVertCount);
	return true;
}
bool A3DLitMesh::ReleaseLMCoords()
{
	if( m_pLMCoords != NULL)
	{
		delete[] m_pLMCoords;
		m_pLMCoords = NULL;
	}
	return true;
}





bool A3DLitMesh::UpdateColors()
{
	int d, n;
	n = (int)(m_fDNFactor * 255.0f);
	d = 255 - n;

	for(int i=0; i<m_nVertCount; i++)
	{
		A3DCOLOR c0, c1, c2, c3;
		c0 = m_pDayColors[i] & 0x00ff00ff;
		c1 = m_pDayColors[i] & 0x0000ff00;
		c2 = m_pNightColors[i] & 0x00ff00ff;
		c3 = m_pNightColors[i] & 0x0000ff00;

		c0 = ((c0 * d + c2 * n) >> 8) & 0x00ff00ff;
		c1 = ((c1 * d + c3 * n) >> 8) & 0x0000ff00;
		m_pVerts[i].diffuse = 0xff000000 | c0 | c1;
	}

	return true;
}

// Draw to buffer
bool A3DLitMesh::DrawToBuffer(A3DLMVERTEX * pBuffer, int nStartVert, WORD * pIndices)
{
	int		i;

	float fDNFactor = m_pA3DDevice->GetA3DEngine()->GetA3DLitModelRender()->GetDNFactor();
	float delta = (float)fabs(fDNFactor - m_fDNFactor);

	// see if we need update the diffuse colors in m_pVerts
	if( delta > 1.0f / 256.0f )
	{
		m_fDNFactor = fDNFactor;
		UpdateColors();
	}
	
	if( m_nAlphaValue < 255 )
	{
		if( m_pParentModel->m_bNoTransform )
		{
			for(i=0; i<m_nVertCount; i++)
			{
				pBuffer[i].pos = m_pVerts[i].pos;
				pBuffer[i].normal = m_pVerts[i].normal;
				pBuffer[i].diffuse = (m_pVerts[i].diffuse & 0x00ffffff) | (m_nAlphaValue << 24);
				pBuffer[i].u = m_pVerts[i].u;
				pBuffer[i].v = m_pVerts[i].v;
#ifdef LM_BUMP_ENABLE
				pBuffer[i].normal = m_pVerts[i].normal;
				pBuffer[i].tangents = m_pVerts[i].tangents;
				pBuffer[i].binormals = m_pVerts[i].binormals;
#endif
			}
		}
		else
		{
			A3DMATRIX4 mat = m_pParentModel->m_matAbsoluteTM;
			for(i=0; i<m_nVertCount; i++)
			{
				pBuffer[i].pos = m_pVerts[i].pos * mat;
				pBuffer[i].normal = a3d_VectorMatrix3x3(m_pVerts[i].normal, mat);
				pBuffer[i].diffuse = (m_pVerts[i].diffuse & 0x00ffffff) | (m_nAlphaValue << 24);
				pBuffer[i].u = m_pVerts[i].u;
				pBuffer[i].v = m_pVerts[i].v;
#ifdef LM_BUMP_ENABLE
				pBuffer[i].normal = a3d_VectorMatrix3x3(m_pVerts[i].normal, mat);
				pBuffer[i].tangents = a3d_VectorMatrix3x3(m_pVerts[i].tangents, mat);
				pBuffer[i].binormals = a3d_VectorMatrix3x3(m_pVerts[i].binormals, mat);
#endif
			}
		}
	}
	else
	{
		if( m_pParentModel->m_bNoTransform )
			memcpy(pBuffer, m_pVerts, sizeof(A3DLMVERTEX) * m_nVertCount);
		else
		{
			A3DMATRIX4 mat = m_pParentModel->m_matAbsoluteTM;
			for(i=0; i<m_nVertCount; i++)
			{
				pBuffer[i].pos = m_pVerts[i].pos * mat;
				pBuffer[i].normal = a3d_VectorMatrix3x3(m_pVerts[i].normal, mat);
				pBuffer[i].diffuse = m_pVerts[i].diffuse;
				pBuffer[i].u = m_pVerts[i].u;
				pBuffer[i].v = m_pVerts[i].v;
#ifdef LM_BUMP_ENABLE
				pBuffer[i].normal = a3d_VectorMatrix3x3(m_pVerts[i].normal, mat);
				pBuffer[i].tangents = a3d_VectorMatrix3x3(m_pVerts[i].tangents, mat);
				pBuffer[i].binormals = a3d_VectorMatrix3x3(m_pVerts[i].binormals, mat);
#endif
			}
		}
	}

	for(i=0; i<m_nFaceCount * 3; i++)
		pIndices[i] = m_pIndices[i] + nStartVert;

	return true;
}

bool A3DLitMesh::DrawLMCoordToBuffer(A3DLMLMVERTEX * pBuffer, int nStartVert, WORD * pIndices)
{
	int		i;
	
	float fDNFactor = m_pA3DDevice->GetA3DEngine()->GetA3DLitModelRender()->GetDNFactor();
	float delta = (float)fabs(fDNFactor - m_fDNFactor);
	
	// see if we need update the diffuse colors in m_pVerts
	if( delta > 1.0f / 256.0f )
	{
		m_fDNFactor = fDNFactor;
		UpdateColors();
	}
	
	if( m_nAlphaValue < 255 )
	{
		if( m_pParentModel->m_bNoTransform )
		{
			for(i=0; i<m_nVertCount; i++)
			{
				pBuffer[i].pos = m_pVerts[i].pos;
				pBuffer[i].normal = m_pVerts[i].normal;
				pBuffer[i].diffuse = (m_pVerts[i].diffuse & 0x00ffffff) | (m_nAlphaValue << 24);
				pBuffer[i].u = m_pVerts[i].u;
				pBuffer[i].v = m_pVerts[i].v;
				pBuffer[i].lm_u = m_pLMCoords[i].u;
				pBuffer[i].lm_v = m_pLMCoords[i].v;

			}
		}
		else
		{
			A3DMATRIX4 mat = m_pParentModel->m_matAbsoluteTM;
			for(i=0; i<m_nVertCount; i++)
			{
				pBuffer[i].pos = m_pVerts[i].pos * mat;
				pBuffer[i].normal = a3d_VectorMatrix3x3(m_pVerts[i].normal, mat);
				pBuffer[i].diffuse = (m_pVerts[i].diffuse & 0x00ffffff) | (m_nAlphaValue << 24);
				pBuffer[i].u = m_pVerts[i].u;
				pBuffer[i].v = m_pVerts[i].v;
				pBuffer[i].lm_u = m_pLMCoords[i].u;
				pBuffer[i].lm_v = m_pLMCoords[i].v;

			}
		}
	}
	else
	{
		if( m_pParentModel->m_bNoTransform )
		{
			for(i=0; i<m_nVertCount; i++)
			{
				pBuffer[i].pos = m_pVerts[i].pos;
				pBuffer[i].normal = m_pVerts[i].normal;
				pBuffer[i].diffuse = m_pVerts[i].diffuse;
				pBuffer[i].u = m_pVerts[i].u;
				pBuffer[i].v = m_pVerts[i].v;
				pBuffer[i].lm_u = m_pLMCoords[i].u;
				pBuffer[i].lm_v = m_pLMCoords[i].v;
				
			}		
		}
		else
		{
			A3DMATRIX4 mat = m_pParentModel->m_matAbsoluteTM;
			for(i=0; i<m_nVertCount; i++)
			{
				pBuffer[i].pos = m_pVerts[i].pos * mat;
				pBuffer[i].normal = a3d_VectorMatrix3x3(m_pVerts[i].normal, mat);
				pBuffer[i].diffuse = m_pVerts[i].diffuse;
				pBuffer[i].u = m_pVerts[i].u;
				pBuffer[i].v = m_pVerts[i].v;
				pBuffer[i].lm_u = m_pLMCoords[i].u;
				pBuffer[i].lm_v = m_pLMCoords[i].v;

			}
		}
	}
	
	for(i=0; i<m_nFaceCount * 3; i++)
		pIndices[i] = m_pIndices[i] + nStartVert;
	
	return true;
}


bool A3DLitMesh::RegisterForRenderShadowMap()
{
	if( !m_bVisible)
		return true;
	m_pA3DDevice->GetA3DEngine()->GetA3DLitModelRender()->RegisterRenderShadowMapMesh(this);
	return true;
}

// Register for render
bool A3DLitMesh::RegisterForRender(A3DViewport * pViewport)
{
	if( !m_bVisible )
		return true;

	m_vDisToCam = Magnitude(pViewport->GetCamera()->GetPos() - m_aabb.Center);

	m_pA3DDevice->GetA3DEngine()->GetA3DLitModelRender()->RegisterRenderMesh(pViewport, this);

	if( m_bSupportLM)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DLitModelRender()->RegisterLightMapMesh(pViewport, this);
	}
	else
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DLitModelRender()->RegisterVertLitMesh(pViewport, this);
	}


	return true;
}

// below function is in the A3DMesh.cpp
extern bool RayIntersectTriangle(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3 * pTriVerts, float * pvFraction, A3DVECTOR3 * pVecPos);

bool A3DLitMesh::RayTraceMesh(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, int * pIdFace)
{
	float			vFractionThis;
	A3DVECTOR3		vecHitPosThis;
	float			vMinFraction = 99999.9f;
	A3DVECTOR3		v0, v1, v2;

	int				n0, n1, n2;
	A3DVECTOR3		v[3];
	for(int i=0; i<m_nFaceCount*3; i+=3)
	{
		n0 = m_pIndices[i];
		n1 = m_pIndices[i + 1];
		n2 = m_pIndices[i + 2];

		v[0] = m_pVerts[n0].pos;
		v[1] = m_pVerts[n1].pos;
		v[2] = m_pVerts[n2].pos;

		if( RayIntersectTriangle(vecStart, vecDelta, v, &vFractionThis, &vecHitPosThis) )
		{
			if( vFractionThis < vMinFraction )
			{
				*pvFraction = vMinFraction = vFractionThis;
				vecHitPos = vecHitPosThis;
				v0 = v[0]; v1 = v[1]; v2 = v[2];
				if( pIdFace )
					*pIdFace = i/3;
			}
		}
	}

	if( vMinFraction < 1.0f )
	{
		// Should calculate normal here;
		vecNormal = Normalize(CrossProduct(v1 - v0, v2 - v1));
		return true;
	}

	return false;
}

// Save and load
bool A3DLitMesh::Save(AFile * pFileToSave)
{
	DWORD dwWriteLen;
	int i;

	A3DLMVERTEX_WITHOUTNORMAL * pVerts = NULL;

	// first output the version
	DWORD dwVersion = A3DLITMESH_VERSION;
	if( m_bSupportLM)
	{
		dwVersion = A3DLITMESH_SUPPORT_LIGHTMAP_VERSION;
	}
	if( !pFileToSave->Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	// now, output the name of this mesh
	char szName[64];
	szName[63] = '\0';
	strncpy(szName, GetName(), 63);
	if(	!pFileToSave->Write(szName, 64, &dwWriteLen) )
		goto WRITEFAIL;

	if( !pFileToSave->Write(m_szTextureMap, 256, &dwWriteLen) )
		goto WRITEFAIL;

	// now output the vertex count and face count
	if( !pFileToSave->Write(&m_nVertCount, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_nFaceCount, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;

	if (!m_bSupportLM)
	{
		if( !pFileToSave->Write(&m_bHasExtraColors, sizeof(bool), &dwWriteLen) )
			goto WRITEFAIL;
	}

	// then output the vertex and indices
	pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
	for(i=0; i<m_nVertCount; i++)
	{
		pVerts[i].pos = m_pVerts[i].pos;
		pVerts[i].diffuse = m_pVerts[i].diffuse;
		pVerts[i].u = m_pVerts[i].u;
		pVerts[i].v = m_pVerts[i].v;
	}
	if( !pFileToSave->Write(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwWriteLen) )
		goto WRITEFAIL;
	delete [] pVerts;

	if( !pFileToSave->Write(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwWriteLen) )
		goto WRITEFAIL;

	if (!m_bSupportLM)
	{
		if (m_bHasExtraColors)
		{
			if( !pFileToSave->Write(m_pDayColorsExtra, sizeof(A3DCOLOR) * m_nVertCount, &dwWriteLen) )
				goto WRITEFAIL;
			if( !pFileToSave->Write(m_pNightColorsExtra, sizeof(A3DCOLOR) * m_nVertCount, &dwWriteLen) )
			goto WRITEFAIL;
		}
	}

	// output the aabb
	if( !pFileToSave->Write(&m_aabb, sizeof(A3DAABB), &dwWriteLen) )
		goto WRITEFAIL;

	// output the material of this mesh
	if( !m_Material.Save(pFileToSave) )
		goto WRITEFAIL;

	//Save light map coords
	if( m_bSupportLM)
	{
		if( !pFileToSave->Write(m_pLMCoords, sizeof( A3DLIGHTMAPCOORD) * m_nVertCount, &dwWriteLen))
			goto WRITEFAIL;
	}

	return true;

WRITEFAIL:
	g_A3DErrLog.Log("A3DLitMesh::Save(), write to the file failed!");
	return false;
}

bool A3DLitMesh::DummyLoad(AFile * pFileToLoad)
{
	DWORD dwReadLen;

	// first read the version
	DWORD dwVersion;
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion == 0x10000001 )
	{
		return false;
	}
	else if( dwVersion == 0x10000002 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices
		pFileToLoad->Seek(sizeof(A3DLVERTEX) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);
	}
	else if( dwVersion == 0x10000003 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLVERTEX) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);
	}
	else if( dwVersion == 0x10000004 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);
	}
	else if( dwVersion == 0x10000005 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);


		A3DMaterial material;
		// load the material
		if( !material.Load(NULL, pFileToLoad) )
			goto READFAIL;
	}
	else if( dwVersion == 0x10000006 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);
		
		int nVertCount, nFaceCount;
		bool bHasExtraColor;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !pFileToLoad->Read(&bHasExtraColor, sizeof(bool), &dwReadLen) )
			goto READFAIL;
		
		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);

		if (bHasExtraColor)
		{
			pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
			pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		}
		
		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);
		
		
		A3DMaterial material;
		// load the material
		if( !material.Load(NULL, pFileToLoad) )
			goto READFAIL;
	}
	else if( dwVersion == A3DLITMESH_SUPPORT_LIGHTMAP_VERSION )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);
		
		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		
		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		
		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);
		
		
		A3DMaterial material;
		// load the material
		if( !material.Load(NULL, pFileToLoad) )
			goto READFAIL;

		//skip light map coords
		pFileToLoad->Seek(sizeof(A3DLIGHTMAPCOORD)* nVertCount, AFILE_SEEK_CUR);

		//skip light map name
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);
	}
	else
	{
		g_A3DErrLog.Log("A3DLitMesh::DummyLoad(), Unknown version [%x]!", dwVersion);
		return false;
	}
	return true;

READFAIL:
	g_A3DErrLog.Log("A3DLitMesh::DummyLoad(), read from the file failed!");
	return false;
}

bool A3DLitMesh::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	DWORD dwReadLen;

	// first read the version
	DWORD dwVersion;
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion == 0x10000001 )
	{
		return false;
	}
	else if( dwVersion == 0x10000002 )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		A3DLVERTEX * pVerts = new A3DLVERTEX[m_nVertCount];
		// then read in the vertex and indices
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLVERTEX) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		// now set day colors to those colors in m_pVerts;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = A3DVECTOR3(pVerts[i].x, pVerts[i].y, pVerts[i].z);
			m_pVerts[i].normal = m_pNormals[i];
			m_pVerts[i].u = pVerts[i].tu;
			m_pVerts[i].v = pVerts[i].tv;
			
			m_pDayColors[i] = m_pVerts[i].diffuse;
			m_pNightColors[i] = m_pVerts[i].diffuse;
		}

		delete [] pVerts;

		m_fDNFactor = -1.0f;

		// last read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;

#ifdef LM_BUMP_ENABLE
		BuildTangents();
#endif
	}
	else if( dwVersion == 0x10000003 )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		A3DLVERTEX * pVerts = new A3DLVERTEX[m_nVertCount];
		// then read in the vertex and indices
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLVERTEX) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		// now set day colors to those colors in m_pVerts;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = A3DVECTOR3(pVerts[i].x, pVerts[i].y, pVerts[i].z);
			m_pVerts[i].normal = m_pNormals[i];
			m_pVerts[i].u = pVerts[i].tu;
			m_pVerts[i].v = pVerts[i].tv;
		}

		delete [] pVerts;
		m_fDNFactor = -1.0f;

		// last read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;

#ifdef LM_BUMP_ENABLE
		BuildTangents();
#endif
	}
	else if( dwVersion == 0x10000004 )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		// then read in the vertex and indices
		A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = pVerts[i].pos;
			m_pVerts[i].diffuse = pVerts[i].diffuse;
			m_pVerts[i].u = pVerts[i].u;
			m_pVerts[i].v = pVerts[i].v;
			m_pVerts[i].normal = m_pNormals[i];
		}
		delete [] pVerts;

		m_fDNFactor = -1.0f;

		// last read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;
	}
	else if( dwVersion == 0x10000005 )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		// then read in the vertex and indices
		A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = pVerts[i].pos;
			m_pVerts[i].diffuse = pVerts[i].diffuse;
			m_pVerts[i].u = pVerts[i].u;
			m_pVerts[i].v = pVerts[i].v;
			m_pVerts[i].normal = m_pNormals[i];
		}
		delete [] pVerts;

		m_fDNFactor = -1.0f;

		// read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;

		// load the material
		if( !m_Material.Load(pA3DDevice, pFileToLoad) )
			goto READFAIL;
	}
	else if( dwVersion == 0x10000006 )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);
		
		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;
		
		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !pFileToLoad->Read(&m_bHasExtraColors, sizeof(bool), &dwReadLen) )
			goto READFAIL;
		
		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}
		
		// then read in the vertex and indices
		A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		
		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		if (m_bHasExtraColors)
		{
			if( !pFileToLoad->Read(m_pDayColorsExtra, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
				goto READFAIL;
			if( !pFileToLoad->Read(m_pNightColorsExtra, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
				goto READFAIL;
		}

		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = pVerts[i].pos;
			m_pVerts[i].diffuse = pVerts[i].diffuse;
			m_pVerts[i].u = pVerts[i].u;
			m_pVerts[i].v = pVerts[i].v;
			m_pVerts[i].normal = m_pNormals[i];
		}
		delete [] pVerts;
		
		m_fDNFactor = -1.0f;
		
		// read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;
		
		// load the material
		if( !m_Material.Load(pA3DDevice, pFileToLoad) )
			goto READFAIL;
	}
	else if( dwVersion == A3DLITMESH_SUPPORT_LIGHTMAP_VERSION )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);
		
		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;
		
		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		
		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap, true) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}
		
		// then read in the vertex and indices
		A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		
		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = pVerts[i].pos;
			m_pVerts[i].diffuse = pVerts[i].diffuse;
			m_pVerts[i].u = pVerts[i].u;
			m_pVerts[i].v = pVerts[i].v;
			m_pVerts[i].normal = m_pNormals[i];
		}
		delete [] pVerts;
		
		m_fDNFactor = -1.0f;
		
		// read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;
		
		// load the material
		if( !m_Material.Load(pA3DDevice, pFileToLoad) )
			goto READFAIL;

		// load light map coords
		if( !pFileToLoad->Read( m_pLMCoords, sizeof(A3DLIGHTMAPCOORD) * m_nVertCount, &dwReadLen))
			goto READFAIL;
		
	}
	else
	{
		g_A3DErrLog.Log("A3DLitMesh::Load(), Unknown version [%x]!", dwVersion);
		return false;
	}

	return true;

READFAIL:
	g_A3DErrLog.Log("A3DLitMesh::Load(), read from the file failed!");
	return false;
}

// Change texture map
bool A3DLitMesh::ChangeTextureMap(const char * szTextureMap)
{
	strncpy(m_szTextureMap, szTextureMap, 255);

	return LoadTexture();
}

const A3DMATRIX4& A3DLitMesh::GetAbsMatrix()
{ 
	return m_pParentModel->m_matAbsoluteTM;
}

A3DTexture*  A3DLitMesh::GetLightMap()
{ 
	return m_pParentModel->GetLightMap();
}
A3DTexture* A3DLitMesh::GetNightLightMap()
{
	return m_pParentModel->GetNightLightMap();
}

const char*  A3DLitMesh::GetLightMapName() 
{ 
	return m_pParentModel->GetLightMapName();
}

const char*  A3DLitMesh::GetNightLightMapName() 
{ 
	return m_pParentModel->GetNightLightMapName();
}

void A3DLitMesh::SetExtraColors(bool bApplay)
{
	if (bApplay)
	{
		if (m_bHasExtraColors)
			return;

		m_bHasExtraColors = true;
		m_pDayColorsExtra = new A3DCOLOR[m_nVertCount];
		m_pNightColorsExtra = new A3DCOLOR[m_nVertCount];
		memset(m_pDayColorsExtra, 0, sizeof(A3DCOLOR) * m_nVertCount);
		memset(m_pNightColorsExtra, 0, sizeof(A3DCOLOR) * m_nVertCount);
	}
	else
	{
		m_bHasExtraColors = false;
		delete[] m_pDayColorsExtra;
		m_pDayColorsExtra = NULL;
		delete[] m_pNightColorsExtra;
		m_pNightColorsExtra = NULL;
	}
}

#ifdef LM_BUMP_ENABLE
bool A3DLitMesh::BuildTangents()
{
	int i;
	// first of all clear all vertex's tangent coords.
	for(i=0; i<m_nVertCount; i++)
	{
		m_pVerts[i].tangents.Clear();
		m_pVerts[i].binormals.Clear();
	}

	// now calcualte tangent space coords.
	for(i=0; i<m_nFaceCount; i++)
	{
		int i0, i1, i2;
		i0 = m_pIndices[i * 3];
		i1 = m_pIndices[i * 3 + 1];
		i2 = m_pIndices[i * 3 + 2];

		A3DLMVERTEX& v0 = m_pVerts[i0];
		A3DLMVERTEX& v1 = m_pVerts[i1];
		A3DLMVERTEX& v2 = m_pVerts[i2];

		A3DVECTOR3 d10 = v1.pos - v0.pos;
		A3DVECTOR3 d20 = v2.pos - v0.pos;
		float t10 = v1.u - v0.u;
		float t20 = v2.u - v0.u;
		float b10 = v0.v - v1.v;
		float b20 = v0.v - v2.v;
		
		A3DVECTOR3 faceTangents = (d10 * b20 - d20 * b10) * (1.0f / (t10 * b20 - t20 * b10));
		A3DVECTOR3 faceBinormals = (d10 * t20 - d20 * t10) * (1.0f / (b10 * t20 - b20 * t10));

		v0.tangents = v0.tangents + faceTangents;
		v0.binormals = v0.binormals + faceBinormals;

		v1.tangents = v1.tangents + faceTangents;
		v1.binormals = v1.binormals + faceBinormals;

		v2.tangents = v2.tangents + faceTangents;
		v2.binormals = v2.binormals + faceBinormals;
	}

	for(i=0; i<m_nVertCount; i++)
	{
		m_pVerts[i].tangents.Normalize();
		m_pVerts[i].binormals.Normalize();
		m_pVerts[i].normal = m_pNormals[i];
		/*
		m_pVerts[i].normal = Normalize(CrossProduct(m_pVerts[i].binormals, m_pVerts[i].tangents));
		if( DotProduct(m_pVerts[i].normal, m_pNormals[i]) < 0.0f )
			m_pVerts[i].normal = -m_pVerts[i].normal;
		*/
	}

	return true;
}

void A3DLitMesh::SetLightInfo(A3DLIGHTPARAM paramDay, A3DLIGHTPARAM paramNight)
{
	m_paramDay = paramDay;
	m_paramNight = paramNight;
}
#endif

//////////////////////////////////////////////////////////////////////////
// class A3DLitModel
//		
//		A model that was pre-lit
//////////////////////////////////////////////////////////////////////////

A3DLitModel::A3DLitModel()
: m_vecScale(1.0f, 1.0f, 1.0f), m_vecPos(0.0f), m_vecDir(0.0f, 0.0f, 1.0f), m_vecUp(0.0f, 1.0f, 0.0f)
{
	m_pA3DDevice			= NULL;
	m_bVisible				= true;

	m_ModelAABB.Clear();
	m_matAbsoluteTM.Identity();
	m_matNormalizedTM.Identity();
	m_bNoTransform			= true;

	memset(m_szLightMap, 0, sizeof(m_szLightMap));
	m_pLMTexture			= NULL;

	memset(m_szNightLightMap, 0, sizeof(m_szNightLightMap));
	m_pNightLMTexture		= NULL;

}

A3DLitModel::~A3DLitModel()
{
	Release();
}

	
// Adjust scale factor
bool A3DLitModel::SetScale(float vScaleX, float vScaleY, float vScaleZ)
{
	m_vecScale.x = vScaleX;
	m_vecScale.y = vScaleY;
	m_vecScale.z = vScaleZ;

	return UpdateAbsoluteTM();
}

// Initialize and finalize
bool A3DLitModel::Init(A3DDevice * pA3DDevice)
{
	// try to release old ones.
	Release();

	m_pA3DDevice		= pA3DDevice;

	return true;
}

bool A3DLitModel::Release()
{
	int nNumMeshes = m_Meshes.GetSize();

	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];

		pMesh->Release();
		delete pMesh;
	}

	m_Meshes.RemoveAll();

	ReleaseLightMap();

	return true;
}

// Render methods
bool A3DLitModel::Render(A3DViewport * pViewport, bool bUpdateShadowMap)
{
	//register shadow map model
	using namespace A3D;	
	if( g_pShadowMap->IsInit() && bUpdateShadowMap)
	{
		if( !m_bVisible )
			return true;
		
		int nNumMeshes = m_Meshes.GetSize();
		for( int nMesh = 0; nMesh < nNumMeshes; nMesh++)
		{
			A3DLitMesh* pMesh = m_Meshes[nMesh];
			pMesh->RegisterForRenderShadowMap();
		}
		
		return true;
	}

	//register render model
	if( !pViewport->GetCamera()->AABBInViewFrustum(m_ModelAABB.Mins, m_ModelAABB.Maxs) ||
		!m_bVisible )
		return true;

	// now register this model's meshes in lit model renderer
	int nNumMeshes = m_Meshes.GetSize();
	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];
		A3DAABB aabb = pMesh->GetAABB();

		if( m_bNoTransform && !pViewport->GetCamera()->AABBInViewFrustum(aabb.Mins, aabb.Maxs) )
			continue;
		
		pMesh->RegisterForRender(pViewport);
	}

	return true;
}

// Visibility Adjustion
void A3DLitModel::SetVisibility(bool bVisible)
{
	m_bVisible = bVisible;
	return;
}

bool A3DLitModel::NormalizeTransforms()
{
	if( m_bNoTransform )
		return true;

	int nNumMeshes = m_Meshes.GetSize();
	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];
		A3DLMVERTEX * pVerts = pMesh->GetVerts();
		A3DVECTOR3 * pNormals = pMesh->GetNormals();

		A3DAABB		aabb;
		aabb.Clear();

		// first transfrom all verts and normals.
		for(int n=0; n<pMesh->GetNumVerts(); n++)
		{
			A3DVECTOR3 vecPos = pVerts[n].pos * m_matAbsoluteTM;
			pVerts[n].pos = vecPos;
			pNormals[n] = Normalize(a3d_VectorMatrix3x3(pNormals[n], m_matAbsoluteTM));
			pVerts[n].normal = pNormals[n];

#ifdef LM_BUMP_ENABLE
			pVerts[n].normal = Normalize(a3d_VectorMatrix3x3(pVerts[n].normal, m_matAbsoluteTM));
			pVerts[n].tangents = Normalize(a3d_VectorMatrix3x3(pVerts[n].tangents, m_matAbsoluteTM));
			pVerts[n].binormals = Normalize(a3d_VectorMatrix3x3(pVerts[n].binormals, m_matAbsoluteTM));
#endif

			aabb.AddVertex(vecPos);
		}

		aabb.CompleteCenterExts();
		pMesh->SetAABB(aabb);
	}

	m_matNormalizedTM = m_matNormalizedTM * m_matAbsoluteTM;
	m_matAbsoluteTM = IdentityMatrix();
	m_bNoTransform = true;
	return true;
}

bool A3DLitModel::UpdateAbsoluteTM()
{
	m_matAbsoluteTM = InverseTM(m_matNormalizedTM) * Scaling(m_vecScale.x, m_vecScale.y, m_vecScale.z) * TransformMatrix(m_vecDir, m_vecUp, m_vecPos);

	// now update the model's aabb information
	int nNumMeshes = m_Meshes.GetSize();
	m_ModelAABB.Clear();

	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];
		const A3DAABB& aabb = pMesh->GetAABB();

		A3DVECTOR3 verts[8];
		aabb.GetVertices(verts, NULL, false);

		for(int j=0; j<8; j++)
		{
			verts[j] = verts[j] * m_matAbsoluteTM;
			m_ModelAABB.AddVertex(verts[j]);
		}
	}

	m_ModelAABB.CompleteCenterExts();

	if( m_matAbsoluteTM == IdentityMatrix() )
		m_bNoTransform = true;
	else
		m_bNoTransform = false;
	return true;
}

// Direct Set the position and orientation methods
bool A3DLitModel::SetPos(const A3DVECTOR3& vecPos)
{
	m_vecPos = vecPos;

	return UpdateAbsoluteTM();
}

bool A3DLitModel::SetDirAndUp(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp)
{
	m_vecDir = vecDir;
	m_vecUp = vecUp;

	return UpdateAbsoluteTM();
}

// Relatively adjust orientation and position methods
bool A3DLitModel::Move(const A3DVECTOR3& vecDeltaPos)
{
	m_vecPos = m_vecPos + vecDeltaPos;

	return UpdateAbsoluteTM();
}

bool A3DLitModel::RotateX(float vDeltaRad)
{
	A3DMATRIX4 matOrient = TransformMatrix(m_vecDir, m_vecUp, A3DVECTOR3(0.0f));

	matOrient = a3d_RotateX(vDeltaRad) * matOrient;

	m_vecDir = matOrient.GetRow(2);
	m_vecUp = matOrient.GetRow(1);

	return UpdateAbsoluteTM();
}

bool A3DLitModel::RotateY(float vDeltaRad)
{
	A3DMATRIX4 matOrient = TransformMatrix(m_vecDir, m_vecUp, A3DVECTOR3(0.0f));

	matOrient = a3d_RotateY(vDeltaRad) * matOrient;

	m_vecDir = matOrient.GetRow(2);
	m_vecUp = matOrient.GetRow(1);

	return UpdateAbsoluteTM();
}

bool A3DLitModel::RotateZ(float vDeltaRad)
{
	A3DMATRIX4 matOrient = TransformMatrix(m_vecDir, m_vecUp, A3DVECTOR3(0.0f));

	matOrient = a3d_RotateZ(vDeltaRad) * matOrient;

	m_vecDir = matOrient.GetRow(2);
	m_vecUp = matOrient.GetRow(1);

	return UpdateAbsoluteTM();
}

bool A3DLitModel::RayTraceAABB(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	// first see if can hit the model's AABB
	if( !CLS_RayToAABB3(vecStart, vecDelta, m_ModelAABB.Mins, m_ModelAABB.Maxs, vecHitPos, pvFraction, vecNormal) )
		return false;

	A3DVECTOR3 vecLocalStart, vecLocalDelta;
	A3DMATRIX4 matIntoModel = m_matAbsoluteTM;
	matIntoModel.InverseTM();

	vecLocalStart = vecStart * matIntoModel;
	vecLocalDelta = a3d_VectorMatrix3x3(vecDelta, matIntoModel);

	float vMinFraction = 99999.9f;
	float vFractionThis = 0.0f;
	A3DVECTOR3 vecLocalHit;
	A3DVECTOR3 vecLocalNormal;

	int nNumMeshes = m_Meshes.GetSize();
	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];

		A3DAABB aabb = pMesh->GetAABB();
		if( CLS_RayToAABB3(vecLocalStart, vecLocalDelta, aabb.Mins, aabb.Maxs, vecLocalHit, &vFractionThis, vecLocalNormal) )
		{
			// see if nearer than the nearest one
			if( vFractionThis < vMinFraction )
			{
				vMinFraction = vFractionThis;
				vecHitPos = vecLocalHit;
				vecNormal = vecLocalNormal;
			}
		}
	}

	if( vMinFraction < 1.0f )
	{
		vecHitPos = vecHitPos * m_matAbsoluteTM;
		vecNormal = a3d_VectorMatrix3x3(vecNormal, m_matAbsoluteTM);
		*pvFraction = vMinFraction;
		return true;
	}

	return false;
}

bool A3DLitModel::RayTraceMesh(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, int * pIdMesh, int * pIdFace)
{
	// first see if can hit the model's AABB
	if( !CLS_RayToAABB3(vecStart, vecDelta, m_ModelAABB.Mins, m_ModelAABB.Maxs, vecHitPos, pvFraction, vecNormal) )
		return false;

	A3DVECTOR3 vecLocalStart, vecLocalDelta;
	A3DMATRIX4 matIntoModel = m_matAbsoluteTM;
	matIntoModel.InverseTM();

	vecLocalStart = vecStart * matIntoModel;
	vecLocalDelta = a3d_VectorMatrix3x3(vecDelta, matIntoModel);

	float vMinFraction = 99999.9f;
	float vFractionThis = 0.0f;
	A3DVECTOR3 vecLocalHit;
	A3DVECTOR3 vecLocalNormal;
	int idMesh = -1;
	int idFace = -1;

	int nNumMeshes = m_Meshes.GetSize();
	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];
		int idFaceThis = -1;

		A3DAABB aabb = pMesh->GetAABB();
		if( CLS_RayToAABB3(vecLocalStart, vecLocalDelta, aabb.Mins, aabb.Maxs, vecLocalHit, &vFractionThis, vecLocalNormal) )
		{
			// then take more accurate test to mesh
			if( pMesh->RayTraceMesh(vecLocalStart, vecLocalDelta, vecLocalHit, &vFractionThis, vecLocalNormal, &idFaceThis) )
			{
				// see if nearer than the nearest one
				if( vFractionThis < vMinFraction )
				{
					vMinFraction = vFractionThis;
					vecHitPos = vecLocalHit;
					vecNormal = vecLocalNormal;
					idMesh = i;
					idFace = idFaceThis;
				}
			}
		}
	}

	if( vMinFraction < 1.0f )
	{
		vecHitPos = vecHitPos * m_matAbsoluteTM;
		vecNormal = a3d_VectorMatrix3x3(vecNormal, m_matAbsoluteTM);
		*pvFraction = vMinFraction;
		if( pIdMesh )
			*pIdMesh = idMesh;
		if( pIdFace )
			*pIdFace = idFace;
		return true;
	}

	return false;
}

// Load and Save
bool A3DLitModel::Save(const char * szFileName)
{
	AFile fileToSave;
	if( !fileToSave.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY) )
	{
		g_A3DErrLog.Log("A3DLitModel::Save(), failed to open file [%s]!", szFileName);
		return false;
	}

	Save(&fileToSave);

	fileToSave.Close();
	return true;
}

bool A3DLitModel::Load(A3DDevice * pA3DDevice, const char * szFileName)
{
	AFile fileToLoad;
	if( !fileToLoad.Open(szFileName, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		g_A3DErrLog.Log("A3DLitModel::Load(), failed to open file [%s]!", szFileName);
		return false;
	}

	Load(pA3DDevice, &fileToLoad);

	fileToLoad.Close();
	return true;
}

bool A3DLitModel::Save(AFile * pFileToSave)
{
	DWORD dwWriteLen;

	// first of all, output the file version
	DWORD dwVersion = A3DLITMODEL_VERSION;
	bool bSupportLM = false;
	int nNumMeshes;
	nNumMeshes = m_Meshes.GetSize();
	for( int nMesh = 0; nMesh < nNumMeshes; nMesh++)
	{
		A3DLitMesh * pMesh = m_Meshes[nMesh];
		if(pMesh->IsSupportLightMap())
		{
			bSupportLM = true;
			break;
		}
	}
	if( bSupportLM)
		dwVersion = A3DLITMODEL_LIGHTMAP_VERSION;
	
	if( !pFileToSave->Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	// then output some basic information of this model
	if( !pFileToSave->Write(&m_vecScale, sizeof(A3DVECTOR3), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vecDir, sizeof(A3DVECTOR3), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vecUp, sizeof(A3DVECTOR3), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vecPos, sizeof(A3DVECTOR3), &dwWriteLen) )
		goto WRITEFAIL;

	// now output the number of meshes in this model

	if( !pFileToSave->Write(&nNumMeshes, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;

	int i;
	for(i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];

		pMesh->Save(pFileToSave);
	}

	//Save light map
	if( bSupportLM)
	{
// 		if( m_szLightMap == NULL || m_szLightMap[0] == '\0')
// 		{
// 			g_A3DErrLog.Log("A3DLitModel::Save(), light map filename is null!");
// 			goto WRITEFAIL;
// 		}
		if( !pFileToSave->Write(m_szLightMap, MAX_PATH, &dwWriteLen) )
			goto WRITEFAIL;
		if( !pFileToSave->Write(m_szNightLightMap, MAX_PATH, &dwWriteLen) )
			goto WRITEFAIL;

	}

	return true;

WRITEFAIL:
	g_A3DErrLog.Log("A3DLitModel::Save(), failed to write into the file!");
	return false;
}

bool A3DLitModel::DummyLoad(AFile * pFileToLoad)
{
	DWORD dwReadLen;

	// first of all, read the file version
	DWORD dwVersion;
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion == 0x10000001 )
	{
		// then skip some basic information of this model
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * 4, AFILE_SEEK_CUR);

		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int i;
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->DummyLoad(pFileToLoad);

			pMesh->Release();
			delete pMesh;
		}
	}
	else if( dwVersion == 0x10000002 )
	{
		// then skip some basic information of this model
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * 4, AFILE_SEEK_CUR);

		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int i;
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->DummyLoad(pFileToLoad);

			pMesh->Release();
			delete pMesh;
		}
	}
	else
	{
		g_A3DErrLog.Log("A3DLitModel::DummyLoad(), Wrong version!");
		return false;
	}

	return true;

READFAIL:
	g_A3DErrLog.Log("A3DLitModel::DummyLoad(), failed to read from the file!");
	return false;
}

bool A3DLitModel::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	if( !Init(pA3DDevice) )
		return false;

	DWORD dwReadLen;

	// first of all, read the file version
	DWORD dwVersion;
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion == 0x10000001 )
	{
		// then output some basic information of this model
		if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;

		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int i;
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->Load(pA3DDevice, pFileToLoad);

			// now find if there is a same textured mesh, if so just merge them here
			int n=0;
			for(n=0; n<m_Meshes.GetSize(); n++)
			{
				if( pMesh->GetTexturePtr() == m_Meshes[n]->GetTexturePtr() && pMesh->GetMaterial().Match(&m_Meshes[n]->GetMaterial()) )
					break;
			}

			if( n == m_Meshes.GetSize() )
				m_Meshes.Add(pMesh);
			else
			{
				m_Meshes[n]->MergeMesh(pMesh);
				pMesh->Release();
				delete pMesh;
				pMesh = NULL;
			}
		}
	}
	else if( dwVersion == 0x10000002 )
	{
		// then output some basic information of this model
		if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;

		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int i;
	
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->Load(pA3DDevice, pFileToLoad);

			// now find if there is a same textured mesh, if so just merge them here
			int n=0;
			for(n=0; n<m_Meshes.GetSize(); n++)
			{
				if( pMesh->GetTexturePtr() == m_Meshes[n]->GetTexturePtr() && pMesh->GetMaterial().Match(&m_Meshes[n]->GetMaterial()) )
					break;
			}

			if( n == m_Meshes.GetSize() )
				m_Meshes.Add(pMesh);
			else
			{
				m_Meshes[n]->MergeMesh(pMesh);
				pMesh->Release();
				delete pMesh;
				pMesh = NULL;
			}
			
		}
	}
	else if( dwVersion == 0x10000100 )
	{
		// then output some basic information of this model
		if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		
		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;
		
		int i;
		
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->Load(pA3DDevice, pFileToLoad);	
			m_Meshes.Add(pMesh);		
		}

		//load light map name
		if( !pFileToLoad->Read( m_szLightMap, 256, &dwReadLen))
			goto READFAIL;

		if(!LoadLightMap(true, false))
		{
			return false;
		}
	}
	else if( dwVersion == A3DLITMODEL_LIGHTMAP_VERSION )
	{
		// then output some basic information of this model
		if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		
		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;
		
		int i;
		
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->Load(pA3DDevice, pFileToLoad);	
			m_Meshes.Add(pMesh);		
		}
		
		//load light map name
		if( !pFileToLoad->Read( m_szLightMap, MAX_PATH, &dwReadLen))
			goto READFAIL;
		if( !pFileToLoad->Read(m_szNightLightMap, MAX_PATH, &dwReadLen))
			goto READFAIL;
		
		if(!LoadLightMap(true, true))
		{
			return false;
		}
	}
	else
	{
		g_A3DErrLog.Log("A3DLitModel::Load(), Wrong version!");
		return false;
	}

	UpdateAbsoluteTM();

	return true;

READFAIL:
	g_A3DErrLog.Log("A3DLitModel::Load(), failed to read from the file!");
	return false;
}

bool A3DLitModel::LoadFromMox(A3DDevice * pA3DDevice, const char * szMoxFile)
{
	if( !Init(pA3DDevice) )
		return false;
	
	AFileImage file;

	if( !file.Open(szMoxFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
	{
		g_A3DErrLog.Log("A3DLitModel::LoadFromMox(), failed to open file [%s]!", szMoxFile);
		return false;
	}

	A3DFrame * pFrame = new A3DFrame();

	// load from a mox file
	if( !pFrame->Load(m_pA3DDevice, &file) )
	{
		g_A3DErrLog.Log("A3DLitModel::LoadFromMox(), failed to load mox file!");
		return false;
	}
	file.Close();

	AddMeshFromFrame(pFrame);

	// no longer use frame, so delete it now.
	pFrame->Release();
	delete pFrame;

	file.Close();

	UpdateAbsoluteTM();
	return true;
}

bool A3DLitModel::AddMeshFromFrame(A3DFrame * pFrame)
{
	A3DMesh * pA3DMesh = pFrame->GetFirstMesh();
	
	A3DMATRIX4 matAbsoluteTM = pFrame->GetAbsoluteTM();

	int id = 0;
	while( pA3DMesh )
	{
		// vertex and face count
		int nVertCount = pA3DMesh->GetVertCount();
		int nFaceCount = pA3DMesh->GetIndexCount() / 3;

		bool bSupportLightMap = pA3DMesh->IsSupportLightMap();	

		A3DLitMesh * pLitMesh = AddNewMesh(nVertCount, nFaceCount, pA3DMesh->GetTextureMap(), bSupportLightMap);
		A3DLMVERTEX * pLitVerts = pLitMesh->GetVerts();
		A3DCOLOR *	 pDayColors = pLitMesh->GetDayColors();
		A3DCOLOR *   pNightColors = pLitMesh->GetNightColors();
		WORD *		 pLitIndices = pLitMesh->GetIndices();
		A3DVECTOR3 * pLitNormals = pLitMesh->GetNormals();

		
		A3DLIGHTMAPCOORD* pLitMeshLMCoords = pLitMesh->GetLightMapCoords();
		//copy light map coords
		if( bSupportLightMap)	
			memcpy( pLitMeshLMCoords, pA3DMesh->GetLightMapCoords(0), sizeof(A3DLIGHTMAPCOORD)* nVertCount);

		// first set the name of this LitMesh
		char szName[64];
		if( id == 0 )
			strncpy(szName, pFrame->GetName(), 64);
		else
			sprintf(szName, "%s-%d", pFrame->GetName(), id);
		pLitMesh->SetName(szName);

		A3DAABB		aabb;
		aabb.Clear();

		// retrieve vertex data
		int i;
		A3DVERTEX * pVerts = pA3DMesh->GetVerts(0);
		for(i=0; i<nVertCount; i++)
		{
			A3DVECTOR3 vecPos = A3DVECTOR3(pVerts[i].x, pVerts[i].y, pVerts[i].z) * matAbsoluteTM;
			A3DVECTOR3 vecNormal = a3d_VectorMatrix3x3(A3DVECTOR3(pVerts[i].nx, pVerts[i].ny, pVerts[i].nz), matAbsoluteTM);

			aabb.AddVertex(vecPos);

			pLitVerts[i].pos = vecPos;
			pLitVerts[i].normal = vecNormal;
			pLitVerts[i].diffuse = 0xff808080;
			pLitVerts[i].u = pVerts[i].tu;
			pLitVerts[i].v = pVerts[i].tv;

			pDayColors[i] = 0xff808080;
			pNightColors[i] = 0xff808080;

			pLitNormals[i] = vecNormal;
		}

		


		aabb.CompleteCenterExts();
		pLitMesh->SetAABB(aabb);

		// now set those material properties that we are interested in.
		pLitMesh->GetMaterial().SetEmissive(pA3DMesh->GetMaterial()->GetEmissive());
		if( pA3DMesh->GetMaterial()->Is2Sided() )
		{
			// for lit model, 2sided material just means leaves of the trees.
			pLitMesh->GetMaterial().Set2Sided(true);
		}
	
		// copy indices;
		memcpy(pLitIndices, pA3DMesh->GetIndices(), sizeof(WORD) * nFaceCount * 3);

		pA3DMesh = pFrame->GetNextMesh();
		id ++;
	}

	A3DFrame * pChildFrame = pFrame->GetFirstChildFrame();
	while( pChildFrame )
	{
		AddMeshFromFrame(pChildFrame);

		pChildFrame = pFrame->GetNextChildFrame();
	}

	return true;
}

// mesh manipulation
A3DLitMesh * A3DLitModel::AddNewMesh(int nVertCount, int nFaceCount, const char * szTextureMap, bool bSupportLightMap)
{
	A3DLitMesh * pNewMesh = new A3DLitMesh(this);
	if( !pNewMesh->Create(m_pA3DDevice, nVertCount, nFaceCount, szTextureMap, bSupportLightMap) )
	{
		delete pNewMesh;
		return NULL;
	}

	m_Meshes.Add(pNewMesh);
	return pNewMesh;
}

bool A3DLitModel::DeleteMesh(A3DLitMesh * pMesh)
{
	int nNumMeshes = m_Meshes.GetSize();

	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMeshThis = m_Meshes[i];

		if( pMeshThis == pMesh )
		{
			pMesh->Release();
			delete pMesh;

			m_Meshes.RemoveAt(i);
			return true;
		}
	}

	return true;
}

bool A3DLitModel::ChangeLitmeshTexture(const char* szMeshName, const char* szTexFile)
{

	int nNumMeshes = m_Meshes.GetSize();
	for(int i = 0; i < nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];
		
		const char* szCurMeshName = pMesh->GetName();
		if( stricmp(szCurMeshName, szMeshName) == 0) 
		{
			return pMesh->ChangeTextureMap(szTexFile);
		}
	}

	return false;
}

bool A3DLitModel::SetLightMap(const char* szLightMap)
{	
	if( szLightMap == NULL || szLightMap[0]=='\0')
		return false;

	ReleaseLightMap(true, false);
	strncpy(m_szLightMap, szLightMap, sizeof(m_szLightMap));
	if(!LoadLightMap(true, false))
		return false;	
	return true;
	
}

bool A3DLitModel::SetNightLightMap(const char* szNightLightMap)
{
	if( szNightLightMap == NULL || szNightLightMap[0]=='\0')
		return false;
	
	ReleaseLightMap(false, true);
	strncpy(m_szNightLightMap, szNightLightMap, sizeof(m_szNightLightMap));
	if(!LoadLightMap(false, true))
		return false;
	
	return true;
}

bool A3DLitModel::LoadLightMap(bool bDay, bool bNight)
{	
	if( bDay)
	{
		if( strlen(m_szLightMap) <= 0 || m_szLightMap[0] == '\0' )
			return true;

		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szLightMap, &m_pLMTexture) )
		{
			g_A3DErrLog.Log("A3DLitModel::LoadLightMap(), failed to load texture [%s]!", m_szLightMap);
			return false;
		}
	}

	if( bNight)
	{
		if( strlen(m_szNightLightMap) <= 0 || m_szNightLightMap[0] == '\0')
			return true;

		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szNightLightMap, &m_pNightLMTexture) )
		{
			g_A3DErrLog.Log("A3DLitModel::LoadLightMap(), failed to load texture [%s]!", m_szNightLightMap);
			return false;
		}
	}
	
	return true;
}

bool A3DLitModel::ReleaseLightMap(bool bDay , bool bNight )
{
	if(bDay)
	{
		if( m_pLMTexture )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pLMTexture);
			m_pLMTexture = NULL;
		}
	}

	if(bNight)
	{
		if( m_pNightLMTexture )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pNightLMTexture);
			m_pNightLMTexture = NULL;
		}
	}

	return true;
	
}

#ifdef LM_BUMP_ENABLE
void A3DLitModel::SetLightInfo(A3DLIGHTPARAM paramDay, A3DLIGHTPARAM paramNight)
{
	int nNumMeshes = m_Meshes.GetSize();

	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMeshThis = m_Meshes[i];

		pMeshThis->SetLightInfo(paramDay, paramNight);
	}
}
#endif
