#include <Windows.h>
#include "A3DTexture.h"

#pragma comment(lib, "d3dx8.lib")

A3DTexture::A3DTexture()
{
	m_pDXTexture = NULL;
	m_bMissing = true;
	m_szMapFile[0] = '\0';
}
	
A3DTexture::~A3DTexture()
{
}

bool A3DTexture::Init(IDirect3DDevice8 * pDevice)
{
	m_pDevice = pDevice;
	m_bMissing = true;
	m_szMapFile[0] = '\0';
	return true;
}

bool A3DTexture::SetMapFile(char * szMapFile)
{
	HRESULT	hval;

	strncpy(m_szMapFile, szMapFile, MAX_PATH);
	if( strlen(szMapFile) == 0 )
		return true;

	strncpy(m_szMapFile, szMapFile, MAX_PATH);
	
	//If this object is created outside D3D;
	if( m_pDevice == NULL )
		return true;
	
	//hval = D3DXCreateTextureFromFile(m_pDevice, szMapFile, &m_pDXTexture);
	hval = D3DXCreateTextureFromFileEx(m_pDevice, szMapFile, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pDXTexture);
	if( D3D_OK != hval )
	{
		char szBuffer[1024];
		D3DXGetErrorString(hval, szBuffer, 1024);
		m_bMissing = true;
	}
	else
		m_bMissing = false;

	return true;
}

bool A3DTexture::LoadFromResource(HINSTANCE hRCModule, int nIDResource)
{
	HRESULT	hval;

	//If this object is created outside D3D;
	if( m_pDevice == NULL )
		return true;

	hval = D3DXCreateTextureFromResourceA(m_pDevice, hRCModule, MAKEINTRESOURCE(nIDResource), &m_pDXTexture);
	if( D3D_OK != hval )
		m_bMissing = true;
	else
		m_bMissing = false;


	return true;
}

bool A3DTexture::Release()
{
	if( m_pDXTexture )
	{
		m_pDXTexture->Release();
		m_pDXTexture = NULL;
	}
	return true;
}

bool A3DTexture::Appear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pDevice == NULL )
		return true;

	if( !m_bMissing )
		m_pDevice->SetTexture(nLayer, m_pDXTexture);

	return true;
}

bool A3DTexture::Disappear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pDevice == NULL )
		return true;

	if( !m_bMissing )
		m_pDevice->SetTexture(nLayer, NULL);

	return true;
}

bool A3DTexture::Save(A3DFile * pFileToSave)
{
	if( pFileToSave->IsBinary() )
	{
		DWORD			dwWriteLength;
		char			szLineBuffer[2048];

		sprintf(szLineBuffer, "TEXTURE: %s", m_szMapFile);
		pFileToSave->Write(szLineBuffer, strlen(szLineBuffer) + 1, &dwWriteLength);
	}
	else
	{
		char szLineBuffer[2048];

		sprintf(szLineBuffer, "TEXTURE: %s", m_szMapFile);
		pFileToSave->WriteLine(szLineBuffer);
	}
	return true;
}

bool A3DTexture::Load(IDirect3DDevice8 * pDevice, A3DFile * pFileToLoad)
{
	if( pFileToLoad->IsBinary() )
	{
		DWORD			dwReadLength;
		char			szLineBuffer[2048];
		char			szResult[2048];

		pFileToLoad->ReadString(szLineBuffer, 2048, &dwReadLength);
		pFileToLoad->GetStringAfter(szLineBuffer, "TEXTURE: ", szResult);
		if( strlen(szResult) >= MAX_PATH )
			return false;

		if( !SetMapFile(szResult) )
			return false;
	}
	else
	{
		char szLineBuffer[A3DFILE_LINEMAXLEN];
		char szResult[A3DFILE_LINEMAXLEN];
		DWORD dwReadLen;

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		pFileToLoad->GetStringAfter(szLineBuffer, "TEXTURE: ", szResult);

		if( !SetMapFile(szResult) )
			return false;
	}

	return true;
}