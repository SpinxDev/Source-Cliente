#include <Windows.h>

#include "A3DMaterial.h"
#include "A3DMesh.h"
#include "A3DTexture.h"
#include "A3DOBBFuncs.h"
#include "A3DBox.h"

//	Mesh version
#define MESHVERSION		2

A3DMesh::A3DMesh()
{
	m_nFrame = 0;
	m_nVertCount = 0;
	m_nIndexCount = 0;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_ppVertsBuffer = NULL;
	m_pIndices = NULL;
	m_pOBBs = NULL;
	m_pBoxOBB = NULL;

	m_bHasLOD = false;
	m_szDetailTexture[0] = '\0';
}

A3DMesh::~A3DMesh()
{
}

bool A3DMesh::Init(IDirect3DDevice8 * pDevice, int nFrameCount, int nVertCount, int nIndexCount, bool bWire)
{
	int i;

	m_pDevice = pDevice;
	m_nFrameCount = nFrameCount;
	m_nVertCount = nVertCount;
	m_nIndexCount = nIndexCount;
	m_bWire = bWire;
	ZeroMemory(&m_Property, sizeof(m_Property));

	if( m_bWire )
	{
		if( m_nIndexCount % 2 != 0 )
			return false;
	}
	else
	{
		if( m_nIndexCount % 3 != 0 )
			return false;
	}

	//Allocate memory for internal storage;
	m_pIndices = (WORD *) malloc(sizeof(WORD) * m_nIndexCount);
	if( NULL == m_pIndices ) return false;

	if( m_bHasLOD )
	{
		m_pMapTable = (WORD *) malloc(sizeof(WORD) * m_nVertCount);
		if( NULL == m_pMapTable ) return false;
		m_pRDIndices = (WORD *) malloc(sizeof(WORD) * m_nIndexCount);
		if( NULL == m_pRDIndices ) return false;

		//At least we should keep one triangle;
		m_iLODLimit = 3;
	}

	m_ppVertsBuffer = (A3DVERTEX **) malloc(sizeof(A3DVERTEX *) * m_nFrameCount);
	if( NULL == m_ppVertsBuffer ) return false;

	for(i=0; i<m_nFrameCount; i++)
	{
		m_ppVertsBuffer[i] = (A3DVERTEX *) malloc(sizeof(A3DVERTEX) * m_nVertCount);
		if( NULL == m_ppVertsBuffer[i] ) return false;
	}

	if( !CreateVertexBuffer(nVertCount, &m_pVertexBuffer) ) return false;
	if( !CreateIndexBuffer(nIndexCount, &m_pIndexBuffer) )	return false;

	if( !m_Texture.Init(m_pDevice) )
		return false;

	if( !m_DetailTexture.Init(m_pDevice) )
		return false;

	if( !m_Material.Init(m_pDevice) )
		return false;

	m_nFrame = 0;
	return true;
}

bool A3DMesh::SetMaterial(A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShininess, bool b2Sided)
{
	return m_Material.SetValues(clAmbient, clDiffuse, clEmissive, clSpecular, fShininess, b2Sided);
}

bool A3DMesh::SetTexture(char * szTextureName)
{
	if( !m_Texture.SetMapFile(szTextureName) )
		return false;

	return true;
}

bool A3DMesh::SetDetailTexture(char * szDetailTextureName, A3DMATRIX4 matDetailTM)
{
	strncpy(m_szDetailTexture, szDetailTextureName, MAX_PATH);
	m_matDetailTM = matDetailTM;

	if( !m_DetailTexture.SetMapFile(szDetailTextureName) )
		return false;

	return true;
}

bool A3DMesh::Release()
{
	int		i;

	if( m_pBoxOBB )
	{
		m_pBoxOBB->Release();
		delete m_pBoxOBB;
		m_pBoxOBB = NULL;
	}
	if( m_pOBBs )
	{
		free(m_pOBBs);
		m_pOBBs = NULL;
	}
	if( m_pVertexBuffer )
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
	if( m_pIndexBuffer )
	{
		m_pIndexBuffer->Release();		
		m_pIndexBuffer = NULL;
	}
	if( m_pIndices )
	{
		free(m_pIndices);
		m_pIndices = NULL;
	}
	if( m_ppVertsBuffer )
	{
		for(i=0; i<m_nFrameCount; i++)
		{
			free(m_ppVertsBuffer[i]);
			m_ppVertsBuffer[i] = NULL;
		}
		free(m_ppVertsBuffer);
		m_ppVertsBuffer = NULL;
	}

	m_Texture.Release();
	m_DetailTexture.Release();
	m_Material.Release();

	m_nVertCount = m_nIndexCount = 0;
	return true;
}

bool A3DMesh::Render()
{
	//If this object is created outside D3D;
	if( m_pDevice == NULL )
		return true;

	HRESULT hval;

	m_pDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(A3DVERTEX));
	m_pDevice->SetVertexShader(D3DFVF_A3DVERTEX);
	m_pDevice->SetIndices(m_pIndexBuffer, 0);

	if( m_szDetailTexture[0] )
	{
		m_DetailTexture.Appear(1);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);
		m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
		m_pDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		m_pDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		m_pDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_POINT);
		m_pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		m_pDevice->SetTransform(D3DTS_TEXTURE1, (D3DMATRIX *) &m_matDetailTM);
	}

	m_Texture.Appear(0);

	m_Material.Appear();

	if( m_bWire )
	{
		hval = m_pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, m_nVertCount, 0, m_nIndexCount / 2);
		if( D3D_OK != hval ) 
			return false;		
	}
	else
	{
		hval = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_nVertCount, 0, m_nIndexCount / 3);
		if( D3D_OK != hval ) 
			return false;
	}
	
	m_Texture.Disappear(0);

	m_pDevice->SetStreamSource(0, NULL, 0);
	m_pDevice->SetIndices(NULL, 0);

	//if( m_pBoxOBB )
	//	m_pBoxOBB->Render();

	if( m_szDetailTexture[0] )
	{
		m_DetailTexture.Disappear(1);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	}
	return true;
}

bool A3DMesh::UpdateVertexBuffer()
{
	//See if is used outside the d3d;
	if( !m_pDevice )
		return true;

	HRESULT			hval;

	A3DVERTEX * pVBVerts;
	hval = m_pVertexBuffer->Lock(0, 0, (LPBYTE *)&pVBVerts, NULL);
	if( D3D_OK != hval ) return false;

	memcpy(pVBVerts, m_ppVertsBuffer[m_nFrame], m_nVertCount * sizeof(A3DVERTEX));

	hval = m_pVertexBuffer->Unlock();
	if( D3D_OK != hval ) return false;
	
	return true;
}

bool A3DMesh::CreateVertexBuffer(int nVerts, IDirect3DVertexBuffer8 ** ppVertexBuffer)
{
	//See if is used outside the d3d;
	if( !m_pDevice )
		return true;

	HRESULT			hval;

	hval = m_pDevice->CreateVertexBuffer(nVerts * sizeof(A3DVERTEX), NULL, D3DFVF_A3DVERTEX, D3DPOOL_DEFAULT, ppVertexBuffer);
	if( D3D_OK != hval )
		return false;

	return true;
}

bool A3DMesh::SetVerts(int nFrame, A3DVERTEX * pVerts, int nVertCount)
{
	if( nVertCount != m_nVertCount || nFrame >= m_nFrameCount )
		return false;

	for(int i=0; i<nVertCount; i++)
	{
		pVerts[i];
	}
	//Store it in our own data structure;
	memcpy(m_ppVertsBuffer[nFrame], pVerts, sizeof(A3DVERTEX) * m_nVertCount);

	if( nFrame == 0 ) //For first frame automated be the current render frame;
		return UpdateVertexBuffer();

	return true;
}

bool A3DMesh::AddMoreVertex(A3DVERTEX * pVerts, int nVertCount,	WORD * pIndices, int nIndexCount)
{
	int i;

	if( m_nFrameCount > 1 )
	{
		MessageBox(NULL, "A3DMesh::AddMoreVertex Must be called for static model!", "FAIL", MB_ICONSTOP | MB_OK);
		return false;
	}
	m_ppVertsBuffer[0] = (A3DVERTEX *) realloc(m_ppVertsBuffer[0], sizeof(A3DVERTEX) * (m_nVertCount + nVertCount));
	if( NULL == m_ppVertsBuffer[0] ) return false;

	m_pIndices = (WORD *) realloc(m_pIndices, sizeof(WORD) * (m_nIndexCount + nIndexCount));
	if( NULL == m_pIndices ) return false;

	memcpy(&m_ppVertsBuffer[0][m_nVertCount], pVerts, sizeof(A3DVERTEX) * nVertCount);
	for(i=0; i<nIndexCount; i++)
	{
		m_pIndices[m_nIndexCount + i] = pIndices[i] + m_nVertCount;
	}
	
	m_nVertCount += nVertCount;
	m_nIndexCount += nIndexCount;

	if( m_pVertexBuffer )
	{
		m_pVertexBuffer->Release();
	}
	if( m_pIndexBuffer )
	{
		m_pIndexBuffer->Release();
	}
	CreateVertexBuffer(m_nVertCount, &m_pVertexBuffer);
	CreateIndexBuffer(m_nIndexCount, &m_pIndexBuffer);

	UpdateVertexBuffer();
	UpdateIndexBuffer();
	return true;
}


bool A3DMesh::SetIndices(WORD * pIndices, int nIndexCount)
{
	if( nIndexCount != m_nIndexCount )
		return false;

	//Store it in our own data structure;
	memcpy(m_pIndices, pIndices, sizeof(WORD) * m_nIndexCount);

	UpdateIndexBuffer();
	return true;
}

bool A3DMesh::UpdateIndexBuffer()
{
	HRESULT hval;

	//If outside d3d;
	if( !m_pDevice )
		return true;

	//Update the d3d index buffer;
	WORD * pVBIndices;
	hval = m_pIndexBuffer->Lock(0, 0, (LPBYTE *)&pVBIndices, NULL);
	if( D3D_OK != hval ) return false;

	memcpy(pVBIndices, m_pIndices, m_nIndexCount * sizeof(WORD));

	hval = m_pIndexBuffer->Unlock();
	if( D3D_OK != hval ) return false;

	return true;
}

bool A3DMesh::CreateIndexBuffer(int nIndices, IDirect3DIndexBuffer8 ** ppIndexBuffer)
{
	//See if is used outside the d3d;
	if( !m_pDevice )
		return true;

	HRESULT hval;

	hval = m_pDevice->CreateIndexBuffer(nIndices * sizeof(WORD), NULL, D3DFMT_INDEX16, D3DPOOL_DEFAULT, ppIndexBuffer);
	if( D3D_OK != hval )
		return false;

	return true;
}

bool A3DMesh::TickAnimation()
{
	//Here we should use some scripts to control the mesh's behavior;
	if( m_nFrameCount > 1 )
	{
		m_nFrame ++;
		m_nFrame %= m_nFrameCount;
		UpdateVertexBuffer();
	}
	return true;
}

bool A3DMesh::Save(A3DFile * pFileToSave)
{
	//Before Save, we should calculate the OBBs;
	//BuildOBBs();

	// it is an empty mesh, just skip the saving
	if( m_nVertCount <= 0 || m_nIndexCount <= 0 )
		return true;

	if( pFileToSave->IsBinary() )
	{
		int	i;
		DWORD dwWriteLength;
		char szLineBuffer[2048];
		
		//	Write version
		sprintf(szLineBuffer, "VERSION: %d", MESHVERSION);
		pFileToSave->Write(szLineBuffer, strlen(szLineBuffer) + 1, &dwWriteLength);
		//	Write mesh name
		sprintf(szLineBuffer, "MESH: %s", GetName());
		pFileToSave->Write(szLineBuffer, strlen(szLineBuffer) + 1, &dwWriteLength);

		//Vertex Count;
		pFileToSave->Write(&m_nVertCount, sizeof(int), &dwWriteLength);
		//Index Count;
		pFileToSave->Write(&m_nIndexCount, sizeof(int), &dwWriteLength);
		//Frame Count:
		pFileToSave->Write(&m_nFrameCount, sizeof(int), &dwWriteLength);
		//m_bHasLOD:
		pFileToSave->Write(&m_bHasLOD, sizeof(bool), &dwWriteLength);
		//Triangle List or not;
		pFileToSave->Write(&m_bWire, sizeof(bool), &dwWriteLength);
		
		//	User defined properties;
		SaveMeshPropV2(pFileToSave, true);

		//Indices;
		pFileToSave->Write(m_pIndices, sizeof(WORD) * m_nIndexCount, &dwWriteLength);

		//Frame Vertex;
		for(i=0; i<m_nFrameCount; i++)
			pFileToSave->Write(m_ppVertsBuffer[i], sizeof(A3DVERTEX) * m_nVertCount, &dwWriteLength);

		if( strlen(m_szDetailTexture) )
		{
			sprintf(szLineBuffer, "DETAILTEXTURE: %s", m_szDetailTexture);
			pFileToSave->Write(szLineBuffer, strlen(szLineBuffer) + 1, &dwWriteLength);

			pFileToSave->Write(&m_matDetailTM, sizeof(A3DMATRIX4), &dwWriteLength);
		}

		//Texture;
		if( !m_Texture.Save(pFileToSave) )
			return false;
		
		//Material
		if( !m_Material.Save(pFileToSave) )
			return false;
	}
	else
	{
		char szLineBuffer[2048];

		//	Write version
		sprintf(szLineBuffer, "VERSION: %d", MESHVERSION);
		pFileToSave->WriteLine(szLineBuffer);
		//	Write mesh name
		sprintf(szLineBuffer, "MESH: %s\n{", GetName());
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "VERTCOUNT: %d", m_nVertCount);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "INDEXCOUNT: %d", m_nIndexCount);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "FRAMECOUNT: %d", m_nFrameCount);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "HASLOD: %d", m_bHasLOD);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "TRIANGLELIST: %d", !m_bWire);
		pFileToSave->WriteLine(szLineBuffer);
		
		//	User defined properties;
		SaveMeshPropV2(pFileToSave, false);

		pFileToSave->WriteLine("INDEX: \n{");
		if( m_bWire )
		{
			for(int n=0; n<m_nIndexCount / 2; n++)
			{
				sprintf(szLineBuffer, "%d, %d", m_pIndices[n * 2], m_pIndices[n * 2 + 1]) ;
				pFileToSave->WriteLine(szLineBuffer);
			}
		}
		else
		{
			for(int n=0; n<m_nIndexCount / 3; n++)
			{
				sprintf(szLineBuffer, "%d, %d, %d", m_pIndices[n * 3], m_pIndices[n * 3 + 1], m_pIndices[n * 3 + 2]) ;
				pFileToSave->WriteLine(szLineBuffer);
			}
		}
		//<== INDEX: {
		pFileToSave->WriteLine("}");

		pFileToSave->WriteLine("VERTEX: \n{");
		for(int i=0; i<m_nFrameCount; i++)
		{
			sprintf(szLineBuffer, "FRAME%d\n{", i);
			pFileToSave->WriteLine(szLineBuffer);

			for(int v=0; v<m_nVertCount; v++)
			{
				sprintf(szLineBuffer, "(%f, %f, %f, %f, %f, %f, %f, %f)",
					m_ppVertsBuffer[i][v].x, m_ppVertsBuffer[i][v].y, m_ppVertsBuffer[i][v].z,
					m_ppVertsBuffer[i][v].nx, m_ppVertsBuffer[i][v].ny, m_ppVertsBuffer[i][v].nz,
					m_ppVertsBuffer[i][v].tu, m_ppVertsBuffer[i][v].tv);
				pFileToSave->WriteLine(szLineBuffer);
			}
			//<==FRAME%d{
			pFileToSave->WriteLine("}");
		}
		//<==VERTEX: {
		pFileToSave->WriteLine("}");

		if( strlen(m_szDetailTexture) )
		{
			sprintf(szLineBuffer, "DETAILTEXTURE: %s", m_szDetailTexture);
			pFileToSave->WriteLine(szLineBuffer);

			sprintf(szLineBuffer, "(%f, %f, %f, %f)", m_matDetailTM._11, m_matDetailTM._12, m_matDetailTM._13, m_matDetailTM._14);
			pFileToSave->WriteLine(szLineBuffer);
			sprintf(szLineBuffer, "(%f, %f, %f, %f)", m_matDetailTM._21, m_matDetailTM._22, m_matDetailTM._23, m_matDetailTM._24);
			pFileToSave->WriteLine(szLineBuffer);
			sprintf(szLineBuffer, "(%f, %f, %f, %f)", m_matDetailTM._31, m_matDetailTM._32, m_matDetailTM._33, m_matDetailTM._34);
			pFileToSave->WriteLine(szLineBuffer);
			sprintf(szLineBuffer, "(%f, %f, %f, %f)", m_matDetailTM._41, m_matDetailTM._42, m_matDetailTM._43, m_matDetailTM._44);
			pFileToSave->WriteLine(szLineBuffer);
		}

		if( !m_Texture.Save(pFileToSave) )
			return false;

		if( !m_Material.Save(pFileToSave) )
			return false;

		//<== MESH: %s {
		pFileToSave->WriteLine("}");
	}
	return true;
}

bool A3DMesh::Load(IDirect3DDevice8 * pDevice, A3DFile * pFileToLoad)
{
	int iVersion = 1;

	if( pFileToLoad->IsBinary() )
	{
		int	i;
		DWORD dwReadLength;
		char szLineBuffer[2048], szResult[2048];
		
		//	Load version
		pFileToLoad->ReadString(szLineBuffer, 2048, &dwReadLength);
		if (pFileToLoad->GetStringAfter(szLineBuffer, "VERSION: ", szResult))
		{
			iVersion = atoi(szResult);
			pFileToLoad->ReadString(szLineBuffer, 2048, &dwReadLength);
		}

		pFileToLoad->GetStringAfter(szLineBuffer, "MESH: ", szResult);
		if( strlen(szResult) >= MAX_NAME )
			return false;
		SetName(szResult);

		//Vertex Count;
		pFileToLoad->Read(&m_nVertCount, sizeof(int), &dwReadLength);
		//Index Count;
		pFileToLoad->Read(&m_nIndexCount, sizeof(int), &dwReadLength);
		//Frame Count:
		pFileToLoad->Read(&m_nFrameCount, sizeof(int), &dwReadLength);
		//Has LOD:
		pFileToLoad->Read(&m_bHasLOD, sizeof(bool), &dwReadLength);
		//Triangle List or not;
		pFileToLoad->Read(&m_bWire, sizeof(bool), &dwReadLength);
		
		if( !Init(pDevice, m_nFrameCount, m_nVertCount, m_nIndexCount, m_bWire) )
			return false;

		//User defined property;
		LoadMeshPropV2(pFileToLoad, true);

		//Indices;
		pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nIndexCount, &dwReadLength);

		if( m_bHasLOD )
		{
			pFileToLoad->Read(&m_vLODMaxDis, sizeof(FLOAT), &dwReadLength);
			pFileToLoad->Read(&m_vLODMinDis, sizeof(FLOAT), &dwReadLength);
			pFileToLoad->Read(&m_iLODLimit, sizeof(int), &dwReadLength);
			pFileToLoad->Read(m_pMapTable, sizeof(WORD) * m_nVertCount, &dwReadLength);
		}

		//Frame Vertex;
		for(i=0; i<m_nFrameCount; i++)
			pFileToLoad->Read(m_ppVertsBuffer[i], sizeof(A3DVERTEX) * m_nVertCount, &dwReadLength);

		//You must call UpdateVertexBuffer to fill the IDirect3DVertexBuffer;
		UpdateVertexBuffer();
		UpdateIndexBuffer();

		//Texture;
		if( !m_Texture.Load(pDevice, pFileToLoad) )
			return false;
		
		//Material
		if( !m_Material.Load(pDevice, pFileToLoad) )
			return false;
	}
	else
	{
		char szLineBuffer[A3DFILE_LINEMAXLEN];
		char szResult[A3DFILE_LINEMAXLEN];
		int n0, n1, n2;
		DWORD dwReadLen;
		int  nval;
		A3DMESHPROP property;

		//	Load version
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if (pFileToLoad->GetStringAfter(szLineBuffer, "VERSION: ", szResult))
		{
			iVersion = atoi(szResult);
			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		}

		pFileToLoad->GetStringAfter(szLineBuffer, "MESH: ", szResult);
		SetName(szResult);

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
			return false;

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "VERTCOUNT: %d", &m_nVertCount);
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "INDEXCOUNT: %d", &m_nIndexCount);
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "FRAMECOUNT: %d", &m_nFrameCount);
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "HASLOD: %d", &m_bHasLOD);
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "TRIANGLELIST: %d", &nval);
		m_bWire = !(nval ? true : false);

		if( !Init(pDevice, m_nFrameCount, m_nVertCount, m_nIndexCount, m_bWire) )
			return false;

		//	User defined properties
		LoadMeshPropV2(pFileToLoad, false);

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "INDEX: ") )
			return false;

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
			return false;

		if( m_bWire )
		{
			for(int n=0; n<m_nIndexCount / 2; n++)
			{
				pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "%d, %d", &n0, &n1);
				m_pIndices[n * 2] = n0;
				m_pIndices[n * 2 + 1] = n1;
			}
		}
		else
		{
			for(int n=0; n<m_nIndexCount / 3; n++)
			{
				pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "%d, %d, %d", &n0, &n1, &n2);
				m_pIndices[n * 3] = n0;
				m_pIndices[n * 3 + 1] = n1;
				m_pIndices[n * 3 + 2] = n2;
			}
		}

		//<== INDEX: {
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;

		//Vertex Map Table;
		if( m_bHasLOD )
		{
			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "LODLIMIT: %d", &m_iLODLimit);

			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "VERTMAP: ") )
				return false;
			
			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") )
				return false;

			for(int n=0; n<m_nVertCount; n++)
			{
				pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "%d", &n0);
				m_pMapTable[n] = n0;
			}
			//<== VERTMAP: {
			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") )
				return false;
		}

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "VERTEX: ") )
			return false;

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
			return false;

		for(int i=0; i<m_nFrameCount; i++)
		{
			int n;

			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			pFileToLoad->GetStringAfter(szLineBuffer, "FRAME", szResult);
			n = atoi(szResult);
			if( n != i ) 
				return false;
			
			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") )
				return false;
			
			for(int v=0; v<m_nVertCount; v++)
			{
				pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);

				sscanf(szLineBuffer, "(%f, %f, %f, %f, %f, %f, %f, %f)",
					&m_ppVertsBuffer[i][v].x, &m_ppVertsBuffer[i][v].y, &m_ppVertsBuffer[i][v].z,
					&m_ppVertsBuffer[i][v].nx, &m_ppVertsBuffer[i][v].ny, &m_ppVertsBuffer[i][v].nz,
					&m_ppVertsBuffer[i][v].tu, &m_ppVertsBuffer[i][v].tv);
			}
			//<==FRAME%d{
			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") )
				return false;
		}
		//<==VERTEX: {
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;

		//You must call UpdateVertexBuffer to fill the IDirect3DVertexBuffer;
		UpdateVertexBuffer();
		UpdateIndexBuffer();

		if( !m_Texture.Load(pDevice, pFileToLoad) )
			return false;

		if( !m_Material.Load(pDevice, pFileToLoad) )
			return false;

		//<== MESH: %s {
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;
	}
	return true;
}

//	Loading function for version 2 mesh's property
bool A3DMesh::LoadMeshPropV2(A3DFile* pFile, bool bBinary)
{
	m_Property.SetPropFlags(0);

	DWORD dwRead;

	if (bBinary)	//	Binary format
	{
		pFile->Read(&m_Property, sizeof (m_Property), &dwRead);
	}
	else	//	Text format
	{
		int iVal;
		char szLineBuffer[A3DFILE_LINEMAXLEN];

		//	Material type
		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "MaterialType: %d", &iVal);
		m_Property.SetMaterialType((A3DMATERIALTYPE)iVal);

		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "2Sides: %d", &iVal);
		if (iVal) m_Property.Set2SidesFlag(true);

		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "CanShootThrough: %d", &iVal);
		if (iVal) m_Property.SetShootThroughFlag(true);

		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "NoMark: %d", &iVal);
		if (iVal) m_Property.SetNoMarkFlag(true);

		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "CanPickThrough: %d", &iVal);
		if (iVal) m_Property.SetPickThroughFlag(true);

		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "OnGround: %d", &iVal);
		if (iVal) m_Property.SetOnGroundFlag(true);

		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "BSPPVS: %d", &iVal);
		if (iVal) m_Property.SetBSPPVSFlag(true);

		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "NotRender: %d", &iVal);
		if (iVal) m_Property.SetNotRenderFlag(true);

		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "WalkIgnore: %d", &iVal);
		if (iVal) m_Property.SetWalkIgnoreFlag(true);

		pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwRead);
		sscanf(szLineBuffer, "OnlyForWalk: %d", &iVal);
		if (iVal) m_Property.SetOnlyForWalkFlag(true);
	}

	return true;
}

//	Saving function for version 2 mesh's property
bool A3DMesh::SaveMeshPropV2(A3DFile* pFile, bool bBinary)
{
	DWORD dwWrite;

	if (bBinary)	//	Binary format
	{
		pFile->Write(&m_Property, sizeof (m_Property), &dwWrite);
	}
	else	//	Text format
	{
		char szLineBuffer[256];

		sprintf(szLineBuffer, "MaterialType: %d", m_Property.GetMaterialType());
		pFile->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "2Sides: %d", m_Property.Get2SidesFlag());
		pFile->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "CanShootThrough: %d", m_Property.GetShootThroughFlag());
		pFile->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "NoMark: %d", m_Property.GetNoMarkFlag());
		pFile->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "CanPickThrough: %d", m_Property.GetPickThroughFlag());
		pFile->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "OnGround: %d", m_Property.GetOnGroundFlag());
		pFile->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "BSPPVS: %d", m_Property.GetBSPPVSFlag());
		pFile->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "NotRender: %d", m_Property.GetNotRenderFlag());
		pFile->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "WalkIgnore: %d", m_Property.GetWalkIgnoreFlag());
		pFile->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "OnlyForWalk: %d", m_Property.GetOnlyForWalkFlag());
		pFile->WriteLine(szLineBuffer);
	}
	
	return true;
}

bool A3DMesh::BuildOBBs()
{
	if( m_pOBBs )
	{
		free(m_pOBBs);
		m_pOBBs = NULL;
	}
	
	m_pOBBs = (A3DOBB *) malloc(sizeof(A3DOBB) * m_nFrameCount);
	if( NULL == m_pOBBs )
		return false;

	for(int i=0; i<m_nFrameCount; i++)
	{
		m_pOBBs[i] = GetOBB(m_ppVertsBuffer[i], m_nVertCount, m_pIndices, m_nIndexCount / 3);
	}

	if( m_pBoxOBB )
	{
		m_pBoxOBB->Release();
		delete m_pBoxOBB;
		m_pBoxOBB = NULL;
	}

	m_pBoxOBB = new A3DBox();
	if( NULL == m_pBoxOBB )
		return false;

	m_pBoxOBB->Init(m_pDevice, m_pOBBs[0]);
	return true;
}

bool A3DMesh::CleanMesh()
{
	int		i, n;
	int nNewVertCount = 0;
	int nNewIndexCount = 0;

	// Reference counter;
	int * pUseTab = (int *) malloc(sizeof(int) * m_nVertCount);
	if( NULL == pUseTab ) return false;

	// Remap table;
	int * pMapTab = (int *) malloc(sizeof(int) * m_nVertCount);
	if( NULL == pMapTab ) return false;

	// Invmap table;
	int * pInvMapTab = (int *) malloc(sizeof(int) * m_nVertCount);
	if( NULL == pInvMapTab ) return false;

	ZeroMemory(pUseTab, sizeof(int) * m_nVertCount);
	ZeroMemory(pMapTab, sizeof(int) * m_nVertCount);
	ZeroMemory(pInvMapTab, sizeof(int) * m_nVertCount);

	// We only check overlapping vertex for mesh without morphing;
	if( m_nFrameCount == 1 )
	{
		nNewIndexCount = 0;
		for(i=0; i<m_nIndexCount; i += 3)
		{
			A3DVECTOR3 vec0, vec1, vec2;
			vec0.x = m_ppVertsBuffer[0][m_pIndices[i]].x;
			vec0.y = m_ppVertsBuffer[0][m_pIndices[i]].y;
			vec0.z = m_ppVertsBuffer[0][m_pIndices[i]].z;

			vec1.x = m_ppVertsBuffer[0][m_pIndices[i + 1]].x;
			vec1.y = m_ppVertsBuffer[0][m_pIndices[i + 1]].y;
			vec1.z = m_ppVertsBuffer[0][m_pIndices[i + 1]].z;

			vec2.x = m_ppVertsBuffer[0][m_pIndices[i + 2]].x;
			vec2.y = m_ppVertsBuffer[0][m_pIndices[i + 2]].y;
			vec2.z = m_ppVertsBuffer[0][m_pIndices[i + 2]].z;

			if( Magnitude(vec0 - vec1) > 1e-6f && Magnitude(vec0 - vec2) > 1e-6f && Magnitude(vec1 - vec2) > 1e-6f )
			{
				// Delete this face;
				m_pIndices[nNewIndexCount] = m_pIndices[i];
				m_pIndices[nNewIndexCount + 1] = m_pIndices[i + 1];
				m_pIndices[nNewIndexCount + 2] = m_pIndices[i + 2];
				nNewIndexCount += 3;
			}
		}
		m_nIndexCount = nNewIndexCount;
	}

	for(i=0; i<m_nIndexCount; i++)
		pUseTab[m_pIndices[i]] ++;		
	
	for(i=0; i<m_nVertCount; i++)
	{
		if( pUseTab[i] )
		{
			pMapTab[i] = nNewVertCount;
			pInvMapTab[nNewVertCount] = i;
			nNewVertCount ++;
		}
	}

	for(i=0; i<nNewVertCount; i++)
	{
		for(n=0; n<m_nFrameCount; n++)
		{
			m_ppVertsBuffer[n][i] = m_ppVertsBuffer[n][pInvMapTab[i]];
		}
	}			  

	for(i=0; i<m_nIndexCount; i++)
	{
		m_pIndices[i] = pMapTab[m_pIndices[i]];
	}
	
	free(pUseTab);
	free(pMapTab);
	free(pInvMapTab);

	m_nVertCount = nNewVertCount;
	return true;
}