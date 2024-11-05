#include "A3DTextureMan.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"
#include "A3DShaderMan.h"
#include "A3DShader.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DGDI.h"
#include "AFI.h"
#include "AMemory.h"
#include "ACSWrapper.h"

#define new A_DEBUG_NEW

A3DTextureMan::A3DTextureMan()
{
	m_pA3DDevice	= NULL;
	m_iTexSizeCnt	= 0;

	InitializeCriticalSection(&m_cs);
}

A3DTextureMan::~A3DTextureMan()
{
	DeleteCriticalSection(&m_cs);
}

bool A3DTextureMan::Init(A3DDevice * pDevice)
{
	m_pA3DDevice = pDevice;
	return true;
}

bool A3DTextureMan::Release()
{
	ReleaseAllTextures();
	return true;
}

//	Release all textures
void A3DTextureMan::ReleaseAllTextures()
{
	ACSWrapper csa(&m_cs);

	//	We should release shader texture first, or it will cause access violation
	ALISTPOSITION pos = m_ListTexture.GetHeadPosition();
	while (pos)
	{
		TEXTURERECORD* pTextureRecord = m_ListTexture.GetNext(pos);
		A3DTexture* pA3DTexture = pTextureRecord->pA3DTexture;

		if (pA3DTexture && pA3DTexture->IsShaderTexture())
		{
			pA3DTexture->Release();
			delete pA3DTexture;
			pTextureRecord->pA3DTexture = NULL;
		}
	}

	//	Release rest textures
	pos = m_ListTexture.GetHeadPosition();
	while (pos)
	{
		TEXTURERECORD* pTextureRecord = m_ListTexture.GetNext(pos);
		A3DTexture* pA3DTexture = pTextureRecord->pA3DTexture;

		if (pA3DTexture)
		{
			pA3DTexture->Release();
			delete pA3DTexture;
		}

		delete pTextureRecord;
	}

	m_ListTexture.RemoveAll();
}

A3DTextureMan::TEXTURERECORD* A3DTextureMan::FindTexture(const char* szFilename, A3DFORMAT FmtDesired)
{
	ACSWrapper csa(&m_cs);

	ALISTPOSITION pos = m_ListTexture.GetHeadPosition();
	while (pos)
	{
		TEXTURERECORD* pTextureRecord = m_ListTexture.GetNext(pos);

		if (pTextureRecord->fmtDesired == FmtDesired && 
			!stricmp(szFilename, pTextureRecord->strFilename))
			return pTextureRecord;
	}

	return NULL;
}

bool A3DTextureMan::LoadTextureFromFileInFolder(const char* pszFilename, const char* szFolder, A3DTexture ** ppA3DTexture, DWORD dwTextureFlags, int nMipLevel)
{
	char szRelativePath[MAX_PATH];
	af_GetFullPathNoBase(szRelativePath, szFolder, pszFilename);
	return LoadTextureFromFile(szRelativePath, ppA3DTexture, dwTextureFlags, nMipLevel);
}

bool A3DTextureMan::LoadTextureFromFile(const char* pszFilename, A3DTexture** ppA3DTexture, DWORD dwTextureFlags, int nMipLevel)
{
	//	We don't need to load an empty texture;
	if (!pszFilename || !pszFilename[0])
	{
		*ppA3DTexture = NULL;
		return true;
	}

	ACSWrapper csa(&m_cs);

	TEXTURERECORD* pTextureRecord = FindTexture(pszFilename, A3DFMT_UNKNOWN);
	if (pTextureRecord)
	{
		pTextureRecord->nRefCount++;
		*ppA3DTexture = pTextureRecord->pA3DTexture;
		return true;
	}

	*ppA3DTexture = NULL;
	A3DTexture* pNewA3DTexture;

	//	Transform to lower case
	char szNameLwr[MAX_PATH];
	strncpy(szNameLwr, pszFilename, MAX_PATH);
	strlwr(szNameLwr);
	
	char* pChar = strstr(szNameLwr, ".sdr.");
	if (pChar)
	{
		// It is a shader file;
		*(pChar+4) = '\0';
		pNewA3DTexture = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(szNameLwr);
		if (!pNewA3DTexture)
			return false;
	}
	else	//	Is a texture
	{
		if (!(pNewA3DTexture = new A3DTexture))
		{
			g_A3DErrLog.Log("A3DTextureMan::LoadTextureFromFile(), Not enough memory!");
			return false;
		}

		char szFullpath[MAX_PATH];
		if (pszFilename[0] == '\0')
			af_GetFullPath(szFullpath, "Textures\\MissingTex.bmp");
		else
			af_GetFullPath(szFullpath, pszFilename);
	
		if (dwTextureFlags & A3DTF_DETAILMAP)
			pNewA3DTexture->SetIsDetailTexture(true);

		if (dwTextureFlags & A3DTF_LIGHTMAP)
			pNewA3DTexture->SetIsLightMap(true);

		if (dwTextureFlags & A3DTF_MIPLEVEL)
			pNewA3DTexture->SetDesciredMipLevel(nMipLevel);

		if (dwTextureFlags & A3DTF_NODOWNSAMPLE)
			pNewA3DTexture->SetNoDownSample(true);

		if (!pNewA3DTexture->LoadFromFile(m_pA3DDevice, szFullpath, pszFilename))
		{
			g_A3DErrLog.Log("A3DTextureMan::LoadTextureFromFile(), Create new A3DTexture fail!");
			return false;
		}
	}

	if (!(pTextureRecord = new TEXTURERECORD))
	{
		g_A3DErrLog.Log("A3DTextureMan::LoadTextureFile Not enough Memory!");
		return false;
	}

	pTextureRecord->strFilename = pszFilename;
	pTextureRecord->pA3DTexture = pNewA3DTexture;
	pTextureRecord->fmtDesired	= A3DFMT_UNKNOWN;
	pTextureRecord->nRefCount	= 1;
	
	pNewA3DTexture->m_dwPosInMan = (DWORD)m_ListTexture.AddTail(pTextureRecord);
	csa.Detach(true);

	pNewA3DTexture->TickAnimation();
	*ppA3DTexture = pNewA3DTexture;

	return true;
}

A3DTexture * A3DTextureMan::DuplicateTexture(A3DTexture * pA3DTexture)
{
	if (!pA3DTexture || !pA3DTexture->m_dwPosInMan)
		return NULL;

	ACSWrapper csa(&m_cs);

	ALISTPOSITION pos = (ALISTPOSITION) pA3DTexture->m_dwPosInMan;
	TEXTURERECORD* pTextureRecord = m_ListTexture.GetAt(pos);
	ASSERT(pTextureRecord && pTextureRecord->pA3DTexture == pA3DTexture);
	pTextureRecord->nRefCount++;

	return pA3DTexture;
}

bool A3DTextureMan::ReleaseTexture(A3DTexture*& pA3DTexture)
{
	if (!pA3DTexture )
		return true;

	if( pA3DTexture->m_dwPosInMan)
	{
		ACSWrapper csa(&m_cs);

		// remove it from the texture manager if refcount == 0
		ALISTPOSITION pos = (ALISTPOSITION)pA3DTexture->m_dwPosInMan;

		TEXTURERECORD* pTextureRecord = m_ListTexture.GetAt(pos);
		ASSERT(pTextureRecord && pTextureRecord->pA3DTexture == pA3DTexture);

		pTextureRecord->nRefCount--;
		if (!pTextureRecord->nRefCount)
		{
			if( pA3DTexture->IsShaderTexture() )
			{
				m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseShader((A3DShader **)&pA3DTexture);
			}
			else
			{
				pA3DTexture->Release();
				delete pA3DTexture;
			}

			delete pTextureRecord;
			m_ListTexture.RemoveAt(pos);
		}
	}
	else
	{
		if( pA3DTexture->IsShaderTexture() )
			m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseShader((A3DShader **)&pA3DTexture);
		else
		{
			pA3DTexture->Release();
			delete pA3DTexture;
		}
	}

	pA3DTexture = NULL;

	return true;
}

bool A3DTextureMan::Reset()
{
	ReleaseAllTextures();
	return true;
}

bool A3DTextureMan::TickAnimation()
{
	ACSWrapper csa(&m_cs);

	ALISTPOSITION pos = m_ListTexture.GetHeadPosition();
	while (pos)
	{
		TEXTURERECORD* pTextureRecord = m_ListTexture.GetNext(pos);
		pTextureRecord->pA3DTexture->TickAnimation();
	}

	return true;
}

bool A3DTextureMan::PrecacheAllTexture()
{
	if( !m_pA3DDevice->GetD3DDevice() )
		return true;

	// We use a L vertex rectange to make all MIP-MAP level rendered;
	A3DLVERTEX	verts[6];
	verts[0] = A3DLVERTEX(A3DVECTOR3(-100.0f, -10.0f, 0.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);
	verts[1] = A3DLVERTEX(A3DVECTOR3(-100.0f, -10.0f, 2000.0f), 0xffffffff, 0xff000000, 1.0f, 0.0f);
	verts[2] = A3DLVERTEX(A3DVECTOR3( 100.0f, -10.0f, 2000.0f), 0xffffffff, 0xff000000, 0.0f, 1.0f);

	verts[3] = verts[2];
	verts[4] = A3DLVERTEX(A3DVECTOR3( 100.0f, -10.0f, 0.0f), 0xffffffff, 0xff000000, 1.0f, 1.0f);
	verts[5] = verts[0];
	
	D3DXMATRIX matProj;
	A3DMATRIX4 matProject;

	D3DXMatrixPerspectiveFovLH(&matProj, DEG2RAD(60.0f), 1.333f, 1.0f, 2000.0f);
	matProject = *(A3DMATRIX4 *)&matProj;

	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetProjectionMatrix(matProject);
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
	m_pA3DDevice->SetViewMatrix(a3d_IdentityMatrix());

	m_pA3DDevice->SetDeviceVertexShader(A3DFVF_A3DLVERTEX);

	ACSWrapper csa(&m_cs);

	ALISTPOSITION pos = m_ListTexture.GetHeadPosition();
	while (pos)
	{
		TEXTURERECORD* pTextureRecord = m_ListTexture.GetNext(pos);
		A3DTexture* pTexture = pTextureRecord->pA3DTexture;

		pTexture->Appear(0);
		
		m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, 2, verts, sizeof(A3DLVERTEX));

		pTexture->Disappear(0);
	}

	csa.Detach(true);

	m_pA3DDevice->SetLightingEnable(true);
	return true;
}

//	Load texture file with specified size and format
bool A3DTextureMan::LoadTextureFromFile(const char* szFile, int iWidth, int iHeight, A3DFORMAT Fmt,
						int iMipLevel, A3DTexture** ppA3DTexture, PALETTEENTRY* aPalEntries/* NULL */)
{
	//	We don't need to load an empty texture;
	if (!szFile || !szFile[0])
	{
		*ppA3DTexture = NULL;
		return true;
	}

	ACSWrapper csa(&m_cs);

	TEXTURERECORD* pTextureRecord = FindTexture(szFile, Fmt);
	if (pTextureRecord)
	{
		pTextureRecord->nRefCount++;
		*ppA3DTexture = pTextureRecord->pA3DTexture;
		return true;
	}

	*ppA3DTexture = NULL;
	
	A3DTexture* pNewA3DTexture = new A3DTexture;
	if (!pNewA3DTexture)
	{
		g_A3DErrLog.Log("A3DTextureMan::LoadTextureFromFile(), Not enough memory!");
		return false;
	}

	//	Transform to lower case
	if (!pNewA3DTexture->LoadFromFile(m_pA3DDevice, szFile, iWidth, iHeight, Fmt, iMipLevel, aPalEntries))
	{
		g_A3DErrLog.Log("A3DTextureMan::LoadTextureFromFile(), Failed to call A3DTexture::LoadFromFile!");
		return false;
	}

	if (!(pTextureRecord = new TEXTURERECORD))
	{
		g_A3DErrLog.Log("A3DTextureMan::LoadTextureFile Not enough Memory!");
		return false;
	}

	pTextureRecord->strFilename = szFile;
	pTextureRecord->pA3DTexture = pNewA3DTexture;
	pTextureRecord->fmtDesired	= Fmt;
	pTextureRecord->nRefCount	= 1;
	
	pNewA3DTexture->m_dwPosInMan = (DWORD)m_ListTexture.AddTail(pTextureRecord);
	csa.Detach(true);

	pNewA3DTexture->TickAnimation();
	*ppA3DTexture = pNewA3DTexture;

	return true;
}

//	Check whether textures of specified format can be created or not
bool A3DTextureMan::IsFormatSupported(A3DFORMAT Fmt)
{
	A3DTexture Texture;

	if (!Texture.Create(m_pA3DDevice, 16, 16, Fmt))
	{
		g_A3DErrLog.Log("A3DTextureMan::IsFormatSupported(), Falied to create texture of format [%d]!", Fmt);
		return false;
	}

	A3DFORMAT Fmt2 = Texture.GetFormat(0);
	bool bRet = (Fmt == Fmt2);

	Texture.Release();

	return bRet;
}

bool A3DTextureMan::ReloadAllTextures(bool bForceReload)
{
	ACSWrapper csa(&m_cs);

	//	We should release shader texture first, or it will cause access violation
	ALISTPOSITION pos = m_ListTexture.GetHeadPosition();
	while (pos)
	{
		TEXTURERECORD* pTextureRecord = m_ListTexture.GetNext(pos);
		A3DTexture* pA3DTexture = pTextureRecord->pA3DTexture;

		if (pA3DTexture && !pA3DTexture->IsShaderTexture())
			pA3DTexture->Reload(bForceReload);
	}

	return true;
}
