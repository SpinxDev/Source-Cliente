/*
 * FILE: SectorEditorDocFile.cpp
 *
 * DESCRIPTION: File operations for document
 *
 * CREATED BY: Duyuxin, 2003/7/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "SectorEditorDoc.h"

#include "AF.h"
#include "A3D.h"
#include "SceneTexture.h"
#include "SceneFrame.h"
#include "Render.h"
#include "Sector.h"
#include "Portal.h"
#include "Utility.h"

#include "A3DPortalFile.h"
#include "PT_Interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
//	Implement CSectorEditorDoc
//
///////////////////////////////////////////////////////////////////////////

//	Load document
bool CSectorEditorDoc::LoadDocument(const char* szFile)
{
	FILE* fp = fopen(szFile, "rb");
	if (!fp)
	{
		g_Log.Log("CSectorEditorDoc::LoadDocument, Failed to open file %s", szFile);
		return FALSE;
	}

	//	Read file header
	FILEHEADER Header;
	if (fread(&Header, 1, sizeof (Header), fp) != sizeof (Header))
	{
		fclose(fp);
		g_Log.Log("CSectorEditorDoc::LoadDocument, Failed to read file header");
		return FALSE;
	}

	if (Header.dwFlags != SEDFILE_FLAGS || Header.dwVersion > SEDFILE_VERSION)
	{
		fclose(fp);
		g_Log.Log("CSectorEditorDoc::LoadDocument, Invalid file format or version");
		return FALSE;
	}

	m_dwCurFileVer = Header.dwVersion;

	//	Read scene file's name
	m_szSceneFile[0] = '\0';

	int iLen=0;
	fread(&iLen, 1, sizeof (int), fp);

	if (iLen)
	{
		char szSceneFile[MAX_PATH];
		fread(szSceneFile, 1, iLen, fp);
		szSceneFile[iLen] = '\0';

		if (!ChangeSceneFile(szSceneFile))
		{
			g_Log.Log("CSectorEditorDoc::LoadDocument, Failed to change scene file %s", m_szSceneFile);
			AUX_MessageBox(MB_OK, "场景文件 %s 加载失败", szSceneFile);
		}
	}

	//	Read edit data
	if (!LoadEditData(fp))
	{
		fclose(fp);
		g_Log.Log("CSectorEditorDoc::LoadDocument, Failed to call LoadEditData()");
		return FALSE;
	}

	fclose(fp);

	return true;
}

//	Load scene
bool CSectorEditorDoc::LoadScene(const char* szFile)
{
	AFile File;
	if (!File.Open((char*)szFile, AFILE_BINARY | AFILE_OPENEXIST))
	{
		g_Log.Log("CSectorEditorDoc::LoadScene, failed to open file %s", szFile);
		return false;
	}

	//	Create texture scene
	if (!(m_pTextureScene = new CTextureScene))
	{
		g_Log.Log("CSectorEditorDoc::LoadScene, failed to new CTextureScene");
		return false;
	}

	if (!m_pTextureScene->Load(g_Render.GetA3DEngine(), &File))
	{
		m_pTextureScene->Release();
		delete m_pTextureScene;
		m_pTextureScene = NULL;

		g_Log.Log("CSectorEditorDoc::LoadScene, failed to load scene %s", szFile);
		return false;
	}

	File.Close();

	//	Create frame scene
	if (!(m_pFrameScene = new CFrameScene))
	{
		g_Log.Log("CSectorEditorDoc::LoadScene, failed to new CFrameScene");
		return false;
	}

	if (!(m_pFrameScene->Init(&g_Render)))
	{
		g_Log.Log("CSectorEditorDoc::LoadScene, failed to initialize CFrameScene");
		return false;
	}

	if (!(m_pFrameScene->Load(m_pTextureScene)))
	{
		g_Log.Log("CSectorEditorDoc::LoadScene, failed to load CFrameScene");
		return false;
	}

	//	Create visible surface buffers
	m_iNumFace = m_pTextureScene->GetFaceCount();

	if (!(m_aVisSurfs = new int [m_iNumFace]))
	{
		g_Log.Log("CTextureScene::Import, not enough memory");
		return false;
	}

	if (!(m_aVisSurfFlags = new BYTE [m_iNumFace]))
	{
		g_Log.Log("CTextureScene::Import, not enough memory");
		return false;
	}

	memset(m_aVisSurfFlags, 0, m_iNumFace);

	//	Build face AABB
	if (!BuildFaceAABB())
	{
		g_Log.Log("CTextureScene::Import, Failed to call BuildFaceAABB");
		return false;
	}

	m_iNumVisSurf = 0;

	return true;
}

//	Build face AABB
bool CSectorEditorDoc::BuildFaceAABB()
{
	ASSERT(m_pTextureScene);

	m_iNumFace = m_pTextureScene->GetFaceCount();

	A3DVECTOR3* aVertPos = new A3DVECTOR3[m_iNumFace * 3];
	if (!aVertPos)
	{
		g_Log.Log("CSectorEditorDoc::BuildFaceAABB, not enough memory");
		return false;
	}

	int iSize = m_iNumFace * 3 * sizeof (A3DVECTOR3);
	if (!m_pTextureScene->GetAllFaceVerts(aVertPos, &iSize))
	{
		g_Log.Log("CSectorEditorDoc::BuildFaceAABB, Failed to call GetAllFaceVerts");
		return false;
	}

	if (!(m_aFaceAABBs = new FACEAABB[m_iNumFace]))
	{
		g_Log.Log("CSectorEditorDoc::BuildFaceAABB, not enough memory");
		return false;
	}

	A3DVECTOR3* pVert = aVertPos;

	for (int i=0; i < m_iNumFace; i++, pVert+=3)
	{
		FACEAABB* pAABB = &m_aFaceAABBs[i];
		ClearAABB(pAABB->vMins, pAABB->vMaxs);

		AddVertexToAABB(pAABB->vMins, pAABB->vMaxs, pVert[0]);
		AddVertexToAABB(pAABB->vMins, pAABB->vMaxs, pVert[1]);
		AddVertexToAABB(pAABB->vMins, pAABB->vMaxs, pVert[2]);
	}

	delete [] aVertPos;

	return true;
}

//	Load edit data
bool CSectorEditorDoc::LoadEditData(FILE* fp)
{
	int i, iNum;

	//	Load sectors before portals !!
	fread(&iNum, 1, sizeof (int), fp);

	for (i=0; i < iNum; i++)
	{
		CSector* pSector = new CSector;

		if (!pSector->Init(&g_Render, g_vOrigin, g_vOrigin))
		{
			g_Log.Log("CSectorEditorDoc::LoadEditData, Failed to initialize sector");
			return false;
		}

		pSector->Load(fp);

		m_SectorList.AddTail(pSector);
	}

	//	Load portals
	fread(&iNum, 1, sizeof (int), fp);

	A3DVECTOR3 aVerts[4];
	aVerts[0] = aVerts[1] = aVerts[2] = aVerts[3] = g_vOrigin;

	for (i=0; i < iNum; i++)
	{
		CPortal* pPortal = new CPortal;
	
		if (!pPortal->Init(&g_Render, aVerts, 0))
		{
			g_Log.Log("CSectorEditorDoc::LoadEditData, Failed to initialize sector");
			return false;
		}

		pPortal->Load(fp);

		m_PortalList.AddTail(pPortal);
	}

	return true;
}

//	Save document
bool CSectorEditorDoc::SaveDocument(const char* szFile)
{
	FILE* fp = fopen(szFile, "wb+");
	if (!fp)
	{
		g_Log.Log("CSectorEditorDoc::SaveDocument, Failed to create file %s", szFile);
		return FALSE;
	}

	//	Save file header
	FILEHEADER Header;
	Header.dwFlags		= SEDFILE_FLAGS;
	Header.dwVersion	= SEDFILE_VERSION;
	Header.iNumPortal	= 0;
	Header.iNumSector	= 0;

	if (fwrite(&Header, 1, sizeof (Header), fp) != sizeof (Header))
	{
		fclose(fp);
		g_Log.Log("CSectorEditorDoc::SaveDocument, Failed to write file header");
		return FALSE;
	}

	//	Save scene file's name
	int iLen = strlen(m_szSceneFile);
	fwrite(&iLen, 1, sizeof (int), fp);

	if (iLen)
		fwrite(m_szSceneFile, 1, iLen, fp);

	//	Save edit data
	if (!SaveEditData(fp))
	{
		fclose(fp);
		g_Log.Log("CSectorEditorDoc::SaveDocument, Failed to call SaveEditData()");
		return FALSE;
	}

	fclose(fp);

	return true;
}

//	Save edit data
bool CSectorEditorDoc::SaveEditData(FILE* fp)
{
	ALISTPOSITION pos;

	//	Save sectors before portals
	int iNum = m_SectorList.GetCount();
	fwrite(&iNum, 1, sizeof (int), fp);

	pos = m_SectorList.GetHeadPosition();
	while (pos)
	{
		CSector* pSector = (CSector*)m_SectorList.GetNext(pos);
		pSector->Save(fp);
	}

	//	Save portals
	iNum = m_PortalList.GetCount();
	fwrite(&iNum, 1, sizeof (int), fp);

	pos = m_PortalList.GetHeadPosition();
	while (pos)
	{
		CPortal* pPortal = (CPortal*)m_PortalList.GetNext(pos);
		pPortal->Save(fp);
	}

	return true;
}

//	Export portal file
bool CSectorEditorDoc::ExportPortalFile(const char* szFile, PTBUILDPARAMS* pParams)
{
	if (!m_pTextureScene)
	{
		g_Log.Log("CSectorEditorDoc::ExportPortalFile, empty scene !");
		return false;
	}

	PT_Reset();

	//	Emit scene
	if (!EmitScene())
	{
		g_Log.Log("CSectorEditorDoc::ExportPortalFile, Failed to call EmitScene() !");
		return false;
	}

	//	Emit sectors
	if (!EmitSectors())
	{
		g_Log.Log("CSectorEditorDoc::ExportPortalFile, Failed to call EmitSectors() !");
		return false;
	}

	//	Emit portals
	if (!EmitPortals())
	{
		g_Log.Log("CSectorEditorDoc::ExportPortalFile, Failed to call EmitPortals() !");
		return false;
	}

	if (!PT_Build(*pParams))
	{
		g_Log.Log("CSectorEditorDoc::ExportPortalFile, Failed to call PT_Build() !");
		return false;
	}

	//	Save portal file
	PT_Save(szFile);

	return true;
}

//	Emit scene
bool CSectorEditorDoc::EmitScene()
{
	int iNumSurf = m_pTextureScene->GetFaceCount();
	int iSize = iNumSurf * sizeof (A3DVECTOR3) * 3;
	A3DVECTOR3* aVertPos;

	if (!(aVertPos = (A3DVECTOR3*)a_malloc(iSize)))
		return false;

	if (!m_pTextureScene->GetAllFaceVerts(aVertPos, &iSize))
		return false;

	if (!PT_AddRenderSurfs(aVertPos, iNumSurf))
		return false;

	a_free(aVertPos);

	return true;
}

//	Emit sectors
bool CSectorEditorDoc::EmitSectors()
{
	static WORD aIndices[] = 
	{
		0, 1, 2, 0, 2, 3, 
		1, 5, 6, 1, 6, 2,
		2, 6, 7, 2, 7, 3,
		3, 7, 4, 3, 4, 0,
		4, 5, 1, 4, 1, 0,
		5, 7, 6, 5, 4, 7
	};

	ALISTPOSITION pos = m_SectorList.GetHeadPosition();
	while (pos)
	{
		CSector* pSector = (CSector*)m_SectorList.GetNext(pos);

		A3DVECTOR3 vMins = pSector->GetMins();
		A3DVECTOR3 vMaxs = pSector->GetMaxs();

		A3DVECTOR3 aVerts[8];
		aVerts[0].Set(vMins.x, vMaxs.y, vMaxs.z);
		aVerts[1].Set(vMaxs.x, vMaxs.y, vMaxs.z);
		aVerts[2].Set(vMaxs.x, vMaxs.y, vMins.z);
		aVerts[3].Set(vMins.x, vMaxs.y, vMins.z);
		aVerts[4].Set(vMins.x, vMins.y, vMaxs.z);
		aVerts[5].Set(vMaxs.x, vMins.y, vMaxs.z);
		aVerts[6].Set(vMaxs.x, vMins.y, vMins.z);
		aVerts[7].Set(vMins.x, vMins.y, vMins.z);

		PT_AddSector(aVerts, aIndices, 12);
	}

	return true;
}

//	Emit portals
bool CSectorEditorDoc::EmitPortals()
{
	ALISTPOSITION pos = m_PortalList.GetHeadPosition();
	while (pos)
	{
		CPortal* pPortal = (CPortal*)m_PortalList.GetNext(pos);

		A3DVECTOR3 aVerts[4];
		pPortal->GetVertices(aVerts);

		int iFront, iBack, iType, s1, s2, m1, m2;
		CSector *ps1;
		A3DPLANE Plane;

		ps1 = pPortal->GetLinkedSector(0, &m1);
		s1 = GetSectorIndex(ps1);
		s2 = GetSectorIndex(pPortal->GetLinkedSector(1, &m2));

		if (s1 == -1 || s2 == -1)
			continue;

		Plane.CreatePlane(aVerts[0], aVerts[1], aVerts[2]);

		if (DotProduct(Plane.vNormal, ps1->GetCenter()) - Plane.fDist > 0)
		{
			iFront	= s1;
			iBack	= s2;

			if (m1 == CPortal::LINK_FROM)
				iType = PTTYPE_FRONTTOBACK;
			else if (m1 == CPortal::LINK_TO)
				iType = PTTYPE_BACKTOFRONT;
			else
				iType = PTTYPE_BIDIRECTIONAL;
		}
		else
		{
			iFront	= s2;
			iBack	= s1;

			if (m1 == CPortal::LINK_TO)
				iType = PTTYPE_FRONTTOBACK;
			else if (m1 == CPortal::LINK_FROM)
				iType = PTTYPE_BACKTOFRONT;
			else
				iType = PTTYPE_BIDIRECTIONAL;
		}

		PTADDPORTALDATA Data;

		Data.iBack		= iBack;
		Data.iFront		= iFront;
		Data.iNumVert	= 4;
		Data.iType		= iType;

		strcpy(Data.szName, pPortal->GetName());
		memcpy(Data.aVerts, aVerts, 4 * sizeof (A3DVECTOR3));

		PT_AddPortal(Data);
	}

	return true;
}

