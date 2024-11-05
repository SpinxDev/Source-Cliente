/*
 * FILE: A3DVertexShader.cpp
 *
 * DESCRIPTION: Routines for vertex shader
 *
 * CREATED BY: duyuxin, 2003/10/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DVertexShader.h"
#include "A3DPI.h"
#include "A3DDevice.h"
#include "AF.h"

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
//	Implement A3DVertexShader
//
///////////////////////////////////////////////////////////////////////////

A3DVertexShader* A3DVertexShader::m_pCurShader = NULL;

A3DVertexShader::A3DVertexShader()
{
	m_dwClassID			= A3D_CID_VERTEXSHADER;
	m_pA3DDevice		= NULL;
	m_pD3DXShaderBuf	= NULL;
	m_hD3DShader		= NULL;
	m_dwShaderID		= 0;
	m_dwPosInMan		= 0;
}

A3DVertexShader::~A3DVertexShader()
{
}

/*	Initialize object

	Return true for success, otherwise return false;
*/
bool A3DVertexShader::Init(A3DDevice* pDevice)
{
	m_pA3DDevice = pDevice;
	return true;
}

//	Release object
void A3DVertexShader::Release()
{
	if (!m_pA3DDevice)
		return;

	IDirect3DDevice8* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	if (m_hD3DShader)
	{
		pD3DDevice->DeleteVertexShader(m_hD3DShader);
		m_hD3DShader = NULL;
	}

	if (m_pD3DXShaderBuf)
	{
		m_pD3DXShaderBuf->Release();
		m_pD3DXShaderBuf = NULL;
	}
}

/*	Load vertex shader from file

	Return true for success, otherwise return false;

	szFile: file name
	bBinary: true, load binary vertex file. false, load ascii vertex file
	pdwDecl: vertex shader declarator
*/
bool A3DVertexShader::Load(const char* szFile, bool bBinary, DWORD* pdwDecl)
{
	char szFullFileName[MAX_PATH];
	af_GetFullPath(szFullFileName, (char*)szFile);

	HRESULT hr;

	if (bBinary)
	{
		AFileImage File;

		if (!File.Open(szFullFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
		{
			File.Close();
			g_A3DErrLog.Log("A3DVertexShader::Load, Cannot open file %s!", szFullFileName);
			return false;
		}

		if (D3DXCreateBuffer(File.GetFileLength(), &m_pD3DXShaderBuf) != D3D_OK)
		{
			File.Close();
		//	g_A3DErrLog.Log("A3DVertexShader::Load, Failed to create D3DXBuffer!");
			return false;
		}

		//	Copy file data
		memcpy(m_pD3DXShaderBuf->GetBufferPointer(), File.GetFileBuffer(), File.GetFileLength());

		File.Close();
	}
	else	//	Load ascii file
	{
		AFileImage File;

		if (!File.Open(szFullFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
		{
			File.Close();
			g_A3DErrLog.Log("A3DVertexShader::Load, Cannot open file %s!", szFullFileName);
			return false;
		}

		if ((hr = D3DXAssembleShader(File.GetFileBuffer(), File.GetFileLength(), 0, NULL, &m_pD3DXShaderBuf, NULL)) != D3D_OK)
		{
		//	g_A3DErrLog.Log("A3DVertexShader::Load, Failed to load vertex shader.");
			return false;
		}

		File.Close();
	}

	IDirect3DDevice8* pD3DDevice = m_pA3DDevice->GetD3DDevice();
	DWORD dwUsage = 0;
	
	if( !m_pA3DDevice->IsHardwareTL() || !m_pA3DDevice->TestVertexShaderVersion(1, 1) )
		dwUsage = D3DUSAGE_SOFTWAREPROCESSING;

	if ((hr = pD3DDevice->CreateVertexShader(pdwDecl, (DWORD*)m_pD3DXShaderBuf->GetBufferPointer(),
											&m_hD3DShader, dwUsage)) != D3D_OK)
	{
		g_A3DErrLog.Log("A3DVertexShader::Load, Failed to create vertex shader.");
		return false;
	}

	//	Record file name
	af_GetRelativePath(szFullFileName, m_strFileName);
	m_dwShaderID = a_MakeIDFromLowString(m_strFileName);

	//	After DX 8.1, vertex shaders needn't to be recreated when device reset, 
	//	they will be remebered by DX automatically.	so we release vertex buffer here.
	m_pD3DXShaderBuf->Release();
	m_pD3DXShaderBuf = NULL;

	return true;
}

/*	Apply vertex shader

	Return true for success, otherwise return false.
*/
bool A3DVertexShader::Appear()
{
	if (!m_hD3DShader)
		return false;

	if (m_pCurShader == this)
		return true;

	if (!m_pA3DDevice->SetDeviceVertexShader(m_hD3DShader))
	{
		g_A3DErrLog.Log("A3DVertexShader::Appear, Failed to apply vertex shader.");
		return false;
	}

	m_pCurShader = this;

	return true;
}

//	Abolish vertex shader
bool A3DVertexShader::Disappear()
{
	if (m_pCurShader == this)
	{
		m_pA3DDevice->ClearVertexShader();
		m_pCurShader = NULL;
	}

	return true;
}

