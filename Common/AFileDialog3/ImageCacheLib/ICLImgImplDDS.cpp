#include "ICLStdAfx.h"

//////////////////////////////////////////////////////////////////////////
//
//	Impl for ImgImpl_DDS
//
//////////////////////////////////////////////////////////////////////////

#ifdef _ANGELICA2

#include <d3dx8.h>

#elif _ANGELICA31

#include <d3dx9.h>

#endif

ImgImpl_DDS::ImgImpl_DDS(A3DDevice* pDevice)
: m_pDevice(pDevice)
, m_pD3DSurface(NULL)
{
	ASSERT(m_pDevice);
	m_Height = 0;
	m_Width = 0;
}

ImgImpl_DDS::~ImgImpl_DDS()
{
	if (m_pD3DSurface) m_pD3DSurface->Release();
}

bool ImgImpl_DDS::ReadFromFile(const TCHAR * szFilename)
{
	bool bRet;

#ifdef _ANGELICA2

	D3DXIMAGE_INFO d3dximage_info;
	if (FAILED(D3DXGetImageInfoFromFile(szFilename, &d3dximage_info)))
		return false;

	if (FAILED(m_pDevice->GetD3DDevice()->CreateImageSurface(d3dximage_info.Width, d3dximage_info.Height, D3DFMT_A8R8G8B8, &m_pD3DSurface)))
		return false;

	AFileImage file;
	if(!file.Open(AC2AS(szFilename), AFILE_OPENEXIST | AFILE_BINARY))
		return false;

	DWORD nfilesize = file.GetFileLength();
	if(FAILED(D3DXLoadSurfaceFromFileInMemory(m_pD3DSurface, NULL, NULL, file.GetFileBuffer(),
										nfilesize,
										NULL, D3DX_FILTER_TRIANGLE, 0, &d3dximage_info)))
		return false;
	
	file.Close();
	
	RECT rc_lock;
	D3DLOCKED_RECT locked_rect;
	rc_lock.left = rc_lock.top = 0;
	rc_lock.bottom = d3dximage_info.Height, rc_lock.right = d3dximage_info.Width;
	if (FAILED(m_pD3DSurface->LockRect(&locked_rect, &rc_lock, D3DLOCK_READONLY)))
		return false;

	bRet = m_pXImage->CreateFromArray((unsigned char*)locked_rect.pBits, d3dximage_info.Width, d3dximage_info.Height, 32, locked_rect.Pitch, true);

	m_pD3DSurface->UnlockRect();
	
#elif defined(_ANGELICA31) || defined(_ANGELICA22) || defined(_ANGELICA21)

	D3DXIMAGE_INFO d3dximage_info;
	if (FAILED(D3DXGetImageInfoFromFile(szFilename, &d3dximage_info)))
		return false;
#if defined(_ANGELICA21)
	m_pD3DSurface = m_pDevice->CreateSystemMemSurface(d3dximage_info.Width, d3dximage_info.Height, A3DFMT_A8R8G8B8);
    if (!m_pD3DSurface)
		return false;
#else
	m_pD3DSurface = m_pDevice->CreateSystemMemSurface(d3dximage_info.Width, d3dximage_info.Height, A3DFMT_A8R8G8B8);
    //if (FAILED(m_pDevice->GetD3DDevice()->CreateOffscreenPlainSurface(d3dximage_info.Width, d3dximage_info.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pD3DSurface, NULL)))
	if(m_pD3DSurface == NULL)
        return false;
#endif
	m_Height = d3dximage_info.Height;
	m_Width = d3dximage_info.Width;

	AFileImage file;
	if(!file.Open(AC2AS(szFilename), AFILE_OPENEXIST | AFILE_BINARY))
		return false;

	DWORD nfilesize = file.GetFileLength();
	if(FAILED(D3DXLoadSurfaceFromFileInMemory(m_pD3DSurface, NULL, NULL, file.GetFileBuffer(),
		nfilesize,
		NULL, D3DX_FILTER_TRIANGLE, 0, &d3dximage_info)))
		return false;

	file.Close();

	RECT rc_lock;
	D3DLOCKED_RECT locked_rect;
	rc_lock.left = rc_lock.top = 0;
	rc_lock.bottom = d3dximage_info.Height, rc_lock.right = d3dximage_info.Width;
	if (FAILED(m_pD3DSurface->LockRect(&locked_rect, &rc_lock, D3DLOCK_READONLY)))
		return false;

	bRet = m_pXImage->CreateFromArray((unsigned char*)locked_rect.pBits, d3dximage_info.Width, d3dximage_info.Height, 32, locked_rect.Pitch, true);

	m_pD3DSurface->UnlockRect();

#endif

	m_Height = d3dximage_info.Height;
	m_Width = d3dximage_info.Width;

	if (!bRet)
		return false;

	blendalpha();
	return true;
}