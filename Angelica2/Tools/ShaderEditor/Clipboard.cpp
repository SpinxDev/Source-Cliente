/*
 * FILE: Clipboard.cpp
 *
 * DESCRIPTION: clipboard routines
 *
 * CREATED BY: duyuxin, 2002/2/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "CommonDef.h"
#include "Clipboard.h"
#include "A3D.h"

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

CClipboard::CClipboard()
{
	m_iDataType	= CBDT_NONE;
	m_idData	= 0;
}

CClipboard::~CClipboard()
{
}

//	Clear texture info
void CClipboard::ClearTextureInfo()
{
	for (int i=0; i < A3DShader::MAXNUM_STAGE; i++)
	{
		m_aTexInfos[i].strFile	= "";
		m_aTexInfos[i].iD3DSlot	= -1;
	}
}

/*	Set data.

	Return true for success, otherwise return false.

	iType: data type
	idData: specific data id
	pData: data's address
*/
bool CClipboard::SetData(int iType, int idData, void* pData)
{
	if (iType == CBDT_GENERAL)
	{
		SHADERGENERAL General = *((SHADERGENERAL*)pData);

		if (General.pPShader)
		{
			m_strPixelShader = General.pPShader->GetFileName();
			General.pPShader = NULL;
		}
		else
			m_strPixelShader = "";

		m_General = General;
	}
	else if (iType == CBDT_STAGE)
	{
		SHADERSTAGE Stage = *((SHADERSTAGE*)pData);
		int iNumTex = Stage.aTextures.GetSize();
		int iIndex = idData;

		//	Clear texture info of each stage
		ClearTextureInfo();

		if (iNumTex)
		{
			if (Stage.iTexType == A3DSDTEX_NORMAL)
			{
				A3DTexture* pTexture = (A3DTexture*)Stage.aTextures[0];
				m_aTexInfos[iIndex].strFile = pTexture->GetMapFile();
			}
			else if (Stage.iTexType == A3DSDTEX_ANIMATION)
			{
				A3DTexture* pTexture = (A3DTexture*)Stage.aTextures[0];
				char szFile[MAX_PATH];
				strcpy(szFile, pTexture->GetMapFile());
				AnimTextureFileName(szFile);
				m_aTexInfos[iIndex].strFile = szFile;
			}
			else if (Stage.iTexType == A3DSDTEX_D3DTEX)
			{
				A3DTextureProxy* pTexProxy = (A3DTextureProxy*)Stage.aTextures[0];
				m_aTexInfos[iIndex].iD3DSlot = glb_SearchD3DTexSlot(pTexProxy);
			}

			for (int i=0; i < iNumTex; i++)
				Stage.aTextures[i] = NULL;
		}

		m_aStages[iIndex] = Stage;
	}
	else if (iType == CBDT_SHADER)
	{
		A3DShader* pShader = (A3DShader*)pData;

		//	Copy general properties
		SHADERGENERAL General = pShader->GetGeneralProps();
		SetData(CBDT_GENERAL, 0, (void*) &General);

		//	Copy stages
		m_iNumStage = 0;
		for (int i=0; i < pShader->GetStageNum(); i++)
		{
			SHADERSTAGE Stage = pShader->GetStage(i);
			SetData(CBDT_STAGE, i, (void*) &Stage);

			m_iNumStage++;
		}

		//	Copy shader's name
		m_strName = pShader->GetName();
	}
	else	//	TODO: handle CBDT_GENPROP and CBDT_STGPROP type data
		return false;

	m_iDataType = iType;
	m_idData	= idData;

	return true;
}

/*	Get data.

	Return true for success, otherwise return false.

	iType: data type
	idData: specific data id
	pData: in current version, this should be an address of a A3DShader object.

	Note: only when iType and idData match data in clipboard, data can be got
*/
bool CClipboard::GetData(int iType, int idData, void* pData)
{
	if (m_iDataType != iType)
		return false;

	A3DShader* pShader = (A3DShader*)pData;

	if (iType == CBDT_GENERAL)
	{
		pShader->SetGeneralProps(m_General);

		//	then change pixel shader
		if (m_strPixelShader.GetLength())
		{
			if (!pShader->ChangePixelShader(m_strPixelShader))
				return false;
		}
	}
	else if (iType == CBDT_STAGE)
	{
		//	Remove and re-insert has been done in CPropertyForm::OnStgPaste(), do we
		//	use SetStage instead of InsertStage.
	//	pShader->RemoveStage(idData);
	//	pShader->InsertStage(idData, m_aStages[m_idData]);

		SHADERSTAGE* pStage = &m_aStages[m_idData];
		pShader->SetStage(idData, *pStage);

		const TEXINFO& info = m_aTexInfos[m_idData];

		if (pStage->iTexType == A3DSDTEX_NORMAL || pStage->iTexType == A3DSDTEX_ANIMATION)
		{
			if (!pShader->ChangeStageTexture(idData, pStage->iTexType, (DWORD)(const char*)info.strFile, 
						A3DFMT_UNKNOWN, pStage->aTextures.GetSize()))
				return false;
		}
		else if (pStage->iTexType == A3DSDTEX_D3DTEX)
		{
			A3DTextureProxy* pTexProxy = NULL;
			if (info.iD3DSlot >= 0 && info.iD3DSlot < g_aD3DTexs.GetSize())
				pTexProxy = g_aD3DTexs[info.iD3DSlot].pTexProxy;

			if (!pShader->ChangeStageTexture_D3DTex(idData, pTexProxy))
				return false;
		}
	}
	else if (iType == CBDT_SHADER)
	{
		//	Paste general properties at first
		m_iDataType = CBDT_GENERAL;
		if (!GetData(CBDT_GENERAL, 0, pData))
			return false;

		//	Then get stages
		m_iDataType = CBDT_STAGE;

		int i;
		for (i=0; i < m_iNumStage; i++)
		{
			m_idData = i;
			if (!GetData(CBDT_STAGE, i, pData))
				return false;
		}

		m_iDataType = CBDT_SHADER;

		//	Set shander name
		pShader->SetName(m_strName);
	}
	else	//	TODO: handle CBDT_GENPROP and CBDT_STGPROP type data
		return false;

	return true;
}


