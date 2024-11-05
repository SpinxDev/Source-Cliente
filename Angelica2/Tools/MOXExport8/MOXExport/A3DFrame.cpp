#include "A3DFrame.h"
#include "A3DFuncs.h"
#include "A3DTypes.h"

#define FRAMEVERSION	2

A3DFrame::A3DFrame()
{
	m_pDevice = NULL;
	m_pParent = NULL;
	m_pRelativeTM = m_pAbsoluteTM = NULL;
	m_pBoundingBox = NULL;
	m_ppA3DBox = NULL;
	m_nBoundingBoxNum = 0;
	m_nFrame = 0;
}

A3DFrame::~A3DFrame()
{
}

bool A3DFrame::Init(IDirect3DDevice8 * pDevice, int nFrameCount)
{
	if( !m_ChildList.Init() )
		return false;
	if( !m_MeshList.Init() )
		return false;
	m_pDevice = pDevice;

	m_nFrameCount = nFrameCount;
	m_pRelativeTM = (A3DMATRIX4 *) malloc(sizeof(A3DMATRIX4) * m_nFrameCount);
	if( NULL == m_pRelativeTM )
		return false;
	ZeroMemory(m_pRelativeTM, sizeof(A3DMATRIX4) * m_nFrameCount);
	m_pAbsoluteTM = (A3DMATRIX4 *) malloc(sizeof(A3DMATRIX4) * m_nFrameCount);
	if(  NULL == m_pAbsoluteTM )
		return false;
	ZeroMemory(m_pAbsoluteTM, sizeof(A3DMATRIX4) * m_nFrameCount);
	m_nFrame = 0;

	m_nBoundingBoxNum = 0;
	m_pBoundingBox = NULL;
	m_ppA3DBox = NULL;
	return true;
}

bool A3DFrame::Release()
{
	m_pParent = NULL;
	if( m_ppA3DBox )
	{
		for(int i=0; i<m_nBoundingBoxNum; i++)
		{
			m_ppA3DBox[i]->Release();
			delete m_ppA3DBox[i];
			m_ppA3DBox[i] = NULL;
		}
	}

	if( m_pBoundingBox )
	{
		free(m_pBoundingBox);
		m_pBoundingBox = NULL;
	}
	
	m_nBoundingBoxNum = 0;
	if( m_pRelativeTM )
	{
		free(m_pRelativeTM);
		m_pRelativeTM = NULL;
	}
	if( m_pAbsoluteTM )
	{
		free(m_pAbsoluteTM);
		m_pAbsoluteTM = NULL;
	}
	//First release my meshes;
	ALISTELEMENT * pThisMeshElement = m_MeshList.GetHead()->pNext;
	while( pThisMeshElement != 	m_MeshList.GetTail() )
	{
		A3DMesh * pMesh = (A3DMesh *) pThisMeshElement->pData;
		if( strcmp(pMesh->GetName(), "SYSTEM_RESERVED") != 0 )//For ground mesh, temp solution;
		{
			pMesh->Release();
			delete pMesh;
		}
		pThisMeshElement = pThisMeshElement->pNext;
	}

	//Then, render my childs;
	ALISTELEMENT * pThisChildElement = m_ChildList.GetHead()->pNext;
	while( pThisChildElement != m_ChildList.GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pThisChildElement->pData;
		pFrame->Release();
		delete pFrame;
		pThisChildElement = pThisChildElement->pNext;
	}

	//Now release the list;
	m_ChildList.Release();
	m_MeshList.Release();

	return true;
}

bool A3DFrame::AddChild(A3DFrame * pChild)
{
	pChild->SetParent(this);

	return m_ChildList.Append((LPVOID) pChild);
}

bool A3DFrame::AddMesh(A3DMesh * pMesh)
{
	return m_MeshList.Append((LPVOID) pMesh);
}

A3DFrame * A3DFrame::GetFirstChildFrame()
{
	if( m_ChildList.GetSize() == 0 )
		return NULL;

	m_pLastFrameElement = m_ChildList.GetHead()->pNext;
	return (A3DFrame *) m_pLastFrameElement->pData;
}

A3DFrame * A3DFrame::GetNextChildFrame()
{
	if( m_pLastFrameElement )
	{
		m_pLastFrameElement = m_pLastFrameElement->pNext;
		if( m_ChildList.GetTail() == m_pLastFrameElement )
			return NULL;
		else
			return (A3DFrame *) m_pLastFrameElement->pData;
	}

	//Not in GetFirstChild call;
	return NULL;
}

bool A3DFrame::FindChildByName(char * szChildName, A3DFrame ** ppFrame)
{
	//Depth First Search;
	ALISTELEMENT * pThisChildElement = m_ChildList.GetHead()->pNext;
	while( pThisChildElement != m_ChildList.GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pThisChildElement->pData;
		if( 0 == stricmp(szChildName, pFrame->GetName()) )
		{
			*ppFrame = pFrame;
			return true;
		}
		else
		{
			if( pFrame->FindChildByName(szChildName, ppFrame) )
				return true;
		}

		pThisChildElement = pThisChildElement->pNext;
	}

	return false;
}
	
A3DMesh * A3DFrame::GetFirstMesh()
{
	if( m_MeshList.GetSize() == 0 )
		return NULL;

	m_pLastMeshElement = m_MeshList.GetHead()->pNext;
	return (A3DMesh *) m_pLastMeshElement->pData;
}

A3DMesh * A3DFrame::GetNextMesh()
{
	if( m_pLastMeshElement )
	{
		m_pLastMeshElement = m_pLastMeshElement->pNext;
		if( m_MeshList.GetTail() == m_pLastMeshElement )
			return NULL;
		else
			return (A3DMesh *) m_pLastMeshElement->pData;
	}

	//Not in any GetFirstMesh Call;
	return NULL;
}

A3DMATRIX4 A3DFrame::GetAbsoluteTM(int nFrame)
{
	if( nFrame >= m_nFrameCount )
		nFrame = 0;

	return m_pAbsoluteTM[nFrame];
}

A3DMATRIX4 A3DFrame::GetRelativeTM(int nFrame)
{
	if( nFrame >= m_nFrameCount )
		nFrame = 0;

	return m_pRelativeTM[nFrame];
}

A3DMATRIX4 A3DFrame::GetAbsoluteTM()
{
	return m_pAbsoluteTM[m_nFrame];
}

A3DMATRIX4 A3DFrame::GetRelativeTM()
{
	return m_pRelativeTM[m_nFrame];
}

bool A3DFrame::Render()
{
	//If this object is created outside D3D;
	if( m_pDevice == NULL )
		return true;

	//Now determine the absolute Transform matrix;
	if( m_pParent )
		m_pAbsoluteTM[m_nFrame] = GetRelativeTM() * m_pParent->GetAbsoluteTM();
	else
		m_pAbsoluteTM[m_nFrame] = GetRelativeTM();
	A3DMATRIX4 matOldWorld, matNewWorld;
	m_pDevice->GetTransform(D3DTS_WORLD, (D3DMATRIX *)&matOldWorld);
	matNewWorld = m_pAbsoluteTM[m_nFrame] * matOldWorld;
	m_pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matNewWorld);
	//m_pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&m_pAbsoluteTM[m_nFrame]);

	//First render my meshes;
	ALISTELEMENT * pThisMeshElement = m_MeshList.GetHead()->pNext;
	while( pThisMeshElement != 	m_MeshList.GetTail() )
	{
		A3DMesh * pMesh = (A3DMesh *) pThisMeshElement->pData;
		if( !pMesh->Render() )
			return false;
		pThisMeshElement = pThisMeshElement->pNext;
	}

	//Last, we render the bounding boxes;
	for(int i=0; i<m_nBoundingBoxNum; i++)
	{
		m_ppA3DBox[i]->Render();
	}
	m_pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matOldWorld);

	//Then, render my childs;
	ALISTELEMENT * pThisChildElement = m_ChildList.GetHead()->pNext;
	while( pThisChildElement != m_ChildList.GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pThisChildElement->pData;
		if( !pFrame->Render() )
			return false;
		pThisChildElement = pThisChildElement->pNext;
	}

	return true;
}

bool A3DFrame::TickAnimation()
{
	//First update my meshes;
	ALISTELEMENT * pThisMeshElement = m_MeshList.GetHead()->pNext;
	while( pThisMeshElement != 	m_MeshList.GetTail() )
	{
		A3DMesh * pMesh = (A3DMesh *) pThisMeshElement->pData;
		if( !pMesh->TickAnimation() )
			return false;
		pThisMeshElement = pThisMeshElement->pNext;
	}

	//Then, update my childs;
	ALISTELEMENT * pThisChildElement = m_ChildList.GetHead()->pNext;
	while( pThisChildElement != m_ChildList.GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pThisChildElement->pData;
		if( !pFrame->TickAnimation() )
			return false;
		pThisChildElement = pThisChildElement->pNext;
	}

	m_nFrame = (m_nFrame + 1) % m_nFrameCount;
	return true;
}

bool A3DFrame::SetTM(A3DMATRIX4 TM, bool bRelative, int nFrame)
{
	if( nFrame >= m_nFrameCount )
		return false;

	if( bRelative )
		m_pRelativeTM[nFrame] = TM;
	else
		m_pAbsoluteTM[nFrame] = TM;

	return true;
}

bool A3DFrame::SetPos(A3DFrame * pRefFrame, A3DVECTOR3 vecPos, int nFrame)
{
	return true;
}

bool A3DFrame::SetOrientation(A3DFrame * pRefFrame, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, int nFrame)
{
	return true;
}

bool A3DFrame::AddRotation(A3DVECTOR3 vecAxis, FLOAT fDegree, int nFrame)
{
	return true;
}

bool A3DFrame::AddTranslation(A3DVECTOR3 vecOffset, int nFrame)
{
	return true;
}

bool A3DFrame::Scale(FLOAT fScale, int nFrame)
{
	return true;
}

A3DVECTOR3 A3DFrame::Transform(A3DVECTOR3 vec, int nFrame)
{
	if( nFrame >= m_nFrameCount )
		return A3DVECTOR3(0.0f);

	A3DVECTOR3 ret;
	
	ret = vec * m_pAbsoluteTM[nFrame];
	return ret;
}

A3DVECTOR3 A3DFrame::InverseTransform(A3DVECTOR3 vec, int nFrame)
{
	A3DVECTOR3 ret;

	if( nFrame >= m_nFrameCount )
		return A3DVECTOR3(0.0f);

	ret = vec * InverseTM(m_pAbsoluteTM[nFrame]);
	return ret;
}

bool A3DFrame::Save(A3DFile * pFileToSave)
{
	if( pFileToSave->IsBinary() )
	{
		DWORD dwWriteLength;
		DWORD dwData;
		char  szLineBuffer[2048];

		//	Save version
		sprintf(szLineBuffer, "VERSION: %d", FRAMEVERSION);
		pFileToSave->Write(szLineBuffer, strlen(szLineBuffer) + 1, &dwWriteLength);
		//	Save name
		sprintf(szLineBuffer, "FRAME: %s", GetName());
		pFileToSave->Write(szLineBuffer, strlen(szLineBuffer) + 1, &dwWriteLength);

		//Save Frame Matrix;
		pFileToSave->Write(&m_nFrameCount, sizeof(int), &dwWriteLength);
		pFileToSave->Write(m_pRelativeTM, sizeof(A3DMATRIX4) * m_nFrameCount, &dwWriteLength);

		//	Save Bounding Box;
		SaveFrameOBBV2(pFileToSave, true);
		
		//Now save my meshes, but we should get rid of empty meshes;
		dwData = 0;
		ALISTELEMENT * pThisMeshElement = m_MeshList.GetHead()->pNext;
		while( pThisMeshElement != 	m_MeshList.GetTail() )
		{
			A3DMesh * pMesh = (A3DMesh *) pThisMeshElement->pData;
			if( !pMesh->CleanMesh() )
				return false;

			if( pMesh->GetVertexCount() > 0 && pMesh->GetIndexCount() > 0 )
				dwData ++;

			pThisMeshElement = pThisMeshElement->pNext;
		}
		pFileToSave->Write(&dwData, sizeof(DWORD), &dwWriteLength);
		
		pThisMeshElement = m_MeshList.GetHead()->pNext;
		while( pThisMeshElement != 	m_MeshList.GetTail() )
		{
			A3DMesh * pMesh = (A3DMesh *) pThisMeshElement->pData;
			if( !pMesh->Save(pFileToSave) )
				return false;
			pThisMeshElement = pThisMeshElement->pNext;
		}
		
		dwData = m_ChildList.GetSize();
		pFileToSave->Write(&dwData, sizeof(DWORD), &dwWriteLength);
		
		ALISTELEMENT * pThisChildElement = m_ChildList.GetHead()->pNext;
		while( pThisChildElement != m_ChildList.GetTail() )
		{
			A3DFrame * pFrame = (A3DFrame *) pThisChildElement->pData;
			if( !pFrame->Save(pFileToSave) )
				return false;
			pThisChildElement = pThisChildElement->pNext;
		}
	}
	else
	{
		char szLineBuffer[A3DFILE_LINEMAXLEN];

		//	Write version
		sprintf(szLineBuffer, "VERSION: %d", FRAMEVERSION);
		pFileToSave->WriteLine(szLineBuffer);
		//	Write name
		sprintf(szLineBuffer, "FRAME: %s\n{", GetName());
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "FRAMECOUNT: %d\n{", m_nFrameCount);
		pFileToSave->WriteLine(szLineBuffer);
		for(int i=0; i<m_nFrameCount; i++)
		{
			sprintf(szLineBuffer, "\t(%d):", i);
			pFileToSave->WriteLine(szLineBuffer);
			for(int j=0; j<4; j++)
			{
				sprintf(szLineBuffer, "\t\t[%f, %f, %f, %f]", m_pRelativeTM[i].m[j][0],
					m_pRelativeTM[i].m[j][1], m_pRelativeTM[i].m[j][2], m_pRelativeTM[i].m[j][3]);
				pFileToSave->WriteLine(szLineBuffer);
			}
		}
		//<== FRAMECOUNT: %d {
		pFileToSave->WriteLine("}");

		//	Save Bounding Boxes;
		SaveFrameOBBV2(pFileToSave, false);

		//Now save my meshes;
		int nRealMesh = 0;
		ALISTELEMENT * pThisMeshElement = m_MeshList.GetHead()->pNext;
		while( pThisMeshElement != 	m_MeshList.GetTail() )
		{
			A3DMesh * pMesh = (A3DMesh *) pThisMeshElement->pData;
			if( !pMesh->CleanMesh() )
				return false;

			if( pMesh->GetVertexCount() > 0 && pMesh->GetIndexCount() > 0 )
				nRealMesh ++;

			pThisMeshElement = pThisMeshElement->pNext;
		}
		sprintf(szLineBuffer, "MESHCOUNT: %d\n{", nRealMesh);
		pFileToSave->WriteLine(szLineBuffer);
		
		pThisMeshElement = m_MeshList.GetHead()->pNext;
		while( pThisMeshElement != 	m_MeshList.GetTail() )
		{
			A3DMesh * pMesh = (A3DMesh *) pThisMeshElement->pData;
			if( !pMesh->Save(pFileToSave) )
				return false;
			pThisMeshElement = pThisMeshElement->pNext;
		}
		//<== MESHCOUNT: %d	{
		pFileToSave->WriteLine("}");

		sprintf(szLineBuffer, "CHILDCOUNT: %d\n{", m_ChildList.GetSize());
		pFileToSave->WriteLine(szLineBuffer);
		
		ALISTELEMENT * pThisChildElement = m_ChildList.GetHead()->pNext;
		while( pThisChildElement != m_ChildList.GetTail() )
		{
			A3DFrame * pFrame = (A3DFrame *) pThisChildElement->pData;
			if( !pFrame->Save(pFileToSave) )
				return false;
			pThisChildElement = pThisChildElement->pNext;
		}
		// <== CHILDCOUNT: %d { 
		pFileToSave->WriteLine("}");

		// <== FRAME: %s { 
		pFileToSave->WriteLine("}");
	}
	return true;
}

//	Save version 2 frame obb
bool A3DFrame::SaveFrameOBBV2(A3DFile* pFile, bool bBinary)
{
	DWORD dwWriteLength;

	if (bBinary)	//	Binary format
	{
		pFile->Write(&m_nBoundingBoxNum, sizeof (int), &dwWriteLength);
		pFile->Write(m_pBoundingBox, sizeof (A3DFRAMEOBB) * m_nBoundingBoxNum, &dwWriteLength);
	}
	else	//	Text format
	{
		char szLineBuffer[A3DFILE_LINEMAXLEN];

		sprintf(szLineBuffer, "BOUNDINGBOXNUM: %d\n{", m_nBoundingBoxNum);
		pFile->WriteLine(szLineBuffer);

		for (int i=0; i < m_nBoundingBoxNum; i++)
		{
			sprintf(szLineBuffer, "%s", m_pBoundingBox[i].szName);
			pFile->WriteLine(szLineBuffer);
			sprintf(szLineBuffer, "(%f, %f, %f)", m_pBoundingBox[i].a3dOBB.Center.x, m_pBoundingBox[i].a3dOBB.Center.y, m_pBoundingBox[i].a3dOBB.Center.z);
			pFile->WriteLine(szLineBuffer);
			sprintf(szLineBuffer, "(%f, %f, %f)", m_pBoundingBox[i].a3dOBB.XAxis.x, m_pBoundingBox[i].a3dOBB.XAxis.y, m_pBoundingBox[i].a3dOBB.XAxis.z);
			pFile->WriteLine(szLineBuffer);
			sprintf(szLineBuffer, "(%f, %f, %f)", m_pBoundingBox[i].a3dOBB.YAxis.x, m_pBoundingBox[i].a3dOBB.YAxis.y, m_pBoundingBox[i].a3dOBB.YAxis.z);
			pFile->WriteLine(szLineBuffer);
			sprintf(szLineBuffer, "(%f, %f, %f)", m_pBoundingBox[i].a3dOBB.ZAxis.x, m_pBoundingBox[i].a3dOBB.ZAxis.y, m_pBoundingBox[i].a3dOBB.ZAxis.z);
			pFile->WriteLine(szLineBuffer);
			sprintf(szLineBuffer, "(%f, %f, %f)", m_pBoundingBox[i].a3dOBB.Extents.x, m_pBoundingBox[i].a3dOBB.Extents.y, m_pBoundingBox[i].a3dOBB.Extents.z);
			pFile->WriteLine(szLineBuffer);

			//	TODO: write properties and material type
		}

		//	<== BOUNDINGBOXNUM: %d {
		pFile->WriteLine("}");
	}

	return true;
}

bool A3DFrame::Load(IDirect3DDevice8 * pDevice, A3DFile * pFileToLoad)
{
	int iVersion = 1;

	if( pFileToLoad->IsBinary() )
	{
		DWORD dwData;
		DWORD dwReadLength;
		char  szLineBuffer[2048];
		char  szResult[2048];
		int  nBoxNum;
		DWORD n;

		//	Load version
		pFileToLoad->ReadString(szLineBuffer, 2048, &dwReadLength);
		if (pFileToLoad->GetStringAfter(szLineBuffer, "VERSION: ", szResult))
		{
			iVersion = atoi(szResult);
			pFileToLoad->ReadString(szLineBuffer, 2048, &dwReadLength);
		}

		pFileToLoad->GetStringAfter(szLineBuffer, "FRAME: ", szResult);
		if( strlen(szResult) >= MAX_NAME )//Format error;
			return false;
		SetName(szResult);

		pFileToLoad->Read(&m_nFrameCount, sizeof(int), &dwReadLength);

		if( m_nFrameCount > MAX_FRAME )
			return false;

		if( !Init(pDevice, m_nFrameCount) )
			return false;

		pFileToLoad->Read(m_pRelativeTM, sizeof(A3DMATRIX4) * m_nFrameCount, &dwReadLength);
		
		//Load Bounding Box;
		pFileToLoad->Read(&nBoxNum, sizeof(int), &dwReadLength);
		LoadFrameOBBV2(nBoxNum, pFileToLoad, true);

		//Now load my meshes;
		pFileToLoad->Read(&dwData, sizeof(DWORD), &dwReadLength);
		for(n=0; n<dwData; n++)
		{
			A3DMesh * pMesh = new A3DMesh();
			
			m_MeshList.Append((LPVOID) pMesh);

			if( !pMesh->Load(pDevice, pFileToLoad) )
				return false;
		}
		
		//now load my childs;
		pFileToLoad->Read(&dwData, sizeof(DWORD), &dwReadLength);
		for(n=0; n<dwData; n++)
		{
			A3DFrame * pFrame = new A3DFrame();

			m_ChildList.Append(pFrame);

			if( !pFrame->Load(pDevice, pFileToLoad) )
				return false;

			pFrame->SetParent(this);
		}
	}
	else
	{
		char	szLineBuffer[A3DFILE_LINEMAXLEN];
		char	szResult[A3DFILE_LINEMAXLEN];
		DWORD	dwReadLen;
		int		nMeshCount, nChildCount, nBoxNum;
		int		i;

		//	Load version
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if (pFileToLoad->GetStringAfter(szLineBuffer, "VERSION: ", szResult))
		{
			iVersion = atoi(szResult);
			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		}

		pFileToLoad->GetStringAfter(szLineBuffer, "FRAME: ", szResult);
		SetName(szResult);

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
			return false;

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		pFileToLoad->GetStringAfter(szLineBuffer, "FRAMECOUNT: ", szResult);
		m_nFrameCount = atoi(szResult);
		if( m_nFrameCount <= 0 )
			return false;

		if( !Init(pDevice, m_nFrameCount) )
			return false;

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") != 0 )
			return false;

		for(i=0; i<m_nFrameCount; i++)
		{
			int n;
			pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "\t(%d):", &n);
			if( n != i )
				return false;
			for(int j=0; j<4; j++)
			{
				pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "\t\t[%f, %f, %f, %f]", &m_pRelativeTM[i].m[j][0],
					&m_pRelativeTM[i].m[j][1], &m_pRelativeTM[i].m[j][2], &m_pRelativeTM[i].m[j][3]);
			}
		}

		//<== FRAMECOUNT: %d {
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;

		//Load Bounding Boxes;
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		pFileToLoad->GetStringAfter(szLineBuffer, "BOUNDINGBOXNUM: ", szResult);
		nBoxNum = atoi(szResult);
		if( nBoxNum < 0 )
			return false;
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") != 0 )
			return false;

		LoadFrameOBBV2(nBoxNum, pFileToLoad, false);

		//<== BOUNDINGBOXNUM: %d {
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") != 0 )
			return false;

		//Now load my meshes;
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		pFileToLoad->GetStringAfter(szLineBuffer, "MESHCOUNT: ", szResult);
		nMeshCount = atoi(szResult);

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
			return false;

		for(i=0; i<nMeshCount; i++)
		{
			A3DMesh * pMesh = new A3DMesh();
			
			m_MeshList.Append((LPVOID) pMesh);

			if( !pMesh->Load(pDevice, pFileToLoad) )
				return false;
		}
		
		//<== MESHCOUNT: %d	{
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;

		//now load my childs;
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		pFileToLoad->GetStringAfter(szLineBuffer, "CHILDCOUNT: ", szResult);
		nChildCount = atoi(szResult);

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
			return false;

		for(i=0; i<nChildCount; i++)
		{
			A3DFrame * pFrame = new A3DFrame();

			m_ChildList.Append(pFrame);

			if( !pFrame->Load(pDevice, pFileToLoad) )
				return false;

			pFrame->SetParent(this);
		}
		
		// <== CHILDCOUNT: {
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;
		
		// <== FRAME: %s { 
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;

	}
	return true;
}

//	Load version 2 frame obb
bool A3DFrame::LoadFrameOBBV2(int iBoxNum, A3DFile* pFile, bool bBinary)
{
	DWORD dwReadLen;
	char szLineBuffer[A3DFILE_LINEMAXLEN];

	for (int i=0; i < iBoxNum; i++)
	{
		A3DFRAMEOBB obb;

		if (bBinary)	//	Binary format
		{
			pFile->Read(&obb, sizeof (obb), &dwReadLen);
			CompleteOBB(&obb.a3dOBB);
		}
		else	//	Text format
		{
			pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			strncpy(obb.szName, szLineBuffer, 32);
			pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "(%f, %f, %f)", &obb.a3dOBB.Center.x, &obb.a3dOBB.Center.y, &obb.a3dOBB.Center.z);
			pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "(%f, %f, %f)", &obb.a3dOBB.XAxis.x, &obb.a3dOBB.XAxis.y, &obb.a3dOBB.XAxis.z);
			pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "(%f, %f, %f)", &obb.a3dOBB.YAxis.x, &obb.a3dOBB.YAxis.y, &obb.a3dOBB.YAxis.z);
			pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "(%f, %f, %f)", &obb.a3dOBB.ZAxis.x, &obb.a3dOBB.ZAxis.y, &obb.a3dOBB.ZAxis.z);	
			pFile->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "(%f, %f, %f)", &obb.a3dOBB.Extents.x, &obb.a3dOBB.Extents.y, &obb.a3dOBB.Extents.z);

			CompleteOBB(&obb.a3dOBB);

			//	TODO: load Properties and material type
			obb.dwProps	= 0;
			obb.MtlType	= A3DMTL_CONCRETE;
		}

		if (!AddBoundingBox(obb))
			return false;
	}

	return true;
}
	
bool A3DFrame::GetPolygonInfo(int * pMeshCount, int * pVertCount, int * pIndexCount, int * pFrameCount)
{
	int nMeshCount = 0, nVertCount = 0, nIndexCount = 0, nFrameCount = 0;
	
	nMeshCount = m_MeshList.GetSize();
	nFrameCount = m_ChildList.GetSize();

	//First update my meshes;
	ALISTELEMENT * pThisMeshElement = m_MeshList.GetHead()->pNext;
	while( pThisMeshElement != 	m_MeshList.GetTail() )
	{
		A3DMesh * pMesh = (A3DMesh *) pThisMeshElement->pData;

		nVertCount += pMesh->GetVertexCount();
		nIndexCount += pMesh->GetIndexCount();

		pThisMeshElement = pThisMeshElement->pNext;
	}

	*pMeshCount = nMeshCount;
	*pVertCount = nVertCount;
	*pIndexCount = nIndexCount;
	*pFrameCount = nFrameCount;

	//Then, update my childs;
	ALISTELEMENT * pThisChildElement = m_ChildList.GetHead()->pNext;
	while( pThisChildElement != m_ChildList.GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pThisChildElement->pData;

		pFrame->GetPolygonInfo(&nMeshCount, &nVertCount, &nIndexCount, &nFrameCount);

		*pMeshCount += nMeshCount;
		*pVertCount += nVertCount;
		*pIndexCount += nIndexCount;
		*pFrameCount += nFrameCount;

		pThisChildElement = pThisChildElement->pNext;
	}

	return true;
}

bool A3DFrame::AddBoundingBox(const A3DFRAMEOBB& FrameOBB)
{
	A3DBox * pBox;

	m_pBoundingBox = (A3DFRAMEOBB*)realloc(m_pBoundingBox, sizeof(A3DFRAMEOBB) * (m_nBoundingBoxNum + 1));
	if (!m_pBoundingBox)
		return false;
	m_ppA3DBox = (A3DBox**)realloc(m_ppA3DBox, sizeof(A3DBox *) * (m_nBoundingBoxNum + 1));
	if (!m_ppA3DBox)
		return false;

	strncpy(m_pBoundingBox[m_nBoundingBoxNum].szName, FrameOBB.szName, 32);
	m_pBoundingBox[m_nBoundingBoxNum].a3dOBB = FrameOBB.a3dOBB;
	m_pBoundingBox[m_nBoundingBoxNum].dwProps = FrameOBB.dwProps;
	m_pBoundingBox[m_nBoundingBoxNum].MtlType = FrameOBB.MtlType;

	if (!(pBox = new A3DBox))
		return false;
	if (!pBox->Init(m_pDevice, FrameOBB.a3dOBB))
		return false;
	
	m_ppA3DBox[m_nBoundingBoxNum] = pBox;
	
	m_nBoundingBoxNum ++;
	return true;
}


