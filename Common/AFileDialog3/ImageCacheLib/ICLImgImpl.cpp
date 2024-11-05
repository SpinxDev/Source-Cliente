#include "ICLStdAfx.h"
#include <a3dgfxexman.h>

#ifdef _ANGELICA31
#include <A3DGFXENgine.h>
#endif

inline bool isECModel(const char * szFilename)
{
#ifdef _ANGELICA31
	if (af_CheckFileExt(szFilename, ".ecm3"))
		return true;
#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)
	if (af_CheckFileExt(szFilename, ".ecm"))
		return true;
#endif

	return false;
}
inline bool isSMD(const char * szFilename)
{
	if (af_CheckFileExt(szFilename, ".SMD"))
		return true;
	return false;
}
inline bool isUMD(const char * szFilename)
{
	if (af_CheckFileExt(szFilename, ".umd"))
		return true;
	return false;
}
inline bool isUMDS(const char* szFilename)
{
	if (af_CheckFileExt(szFilename, ".umds"))
		return true;
	return false;
}
inline bool isGfx(const char * szFilename)
{
#ifdef _ANGELICA31
	if (af_CheckFileExt(szFilename, ".gfx3"))
		return true;
#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)
	if (af_CheckFileExt(szFilename, ".gfx"))
		return true;
#endif

	return false;
}

inline bool isDDS(const char * szFilename)
{
	if (af_CheckFileExt(szFilename, ".dds"))
		return true;
	return false;
}

inline bool isPicture(const char * szFilename)
{
	if (af_CheckFileExt(szFilename, ".bmp")
		|| af_CheckFileExt(szFilename, ".jpg")
		|| af_CheckFileExt(szFilename, ".tga")
		|| af_CheckFileExt(szFilename, ".png")
		|| af_CheckFileExt(szFilename, ".dds"))
		return true;
#ifdef _ANGELICA31
	if (af_CheckFileExt(szFilename, ".ecm3")
		|| af_CheckFileExt(szFilename, ".gfx3")
		|| af_CheckFileExt(szFilename, ".SMD")
		|| af_CheckFileExt(szFilename, ".umd")
		|| af_CheckFileExt(szFilename, ".umds"))
		return true;

#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)
	if (af_CheckFileExt(szFilename, ".ecm")
		|| af_CheckFileExt(szFilename, ".gfx")
		|| af_CheckFileExt(szFilename, ".smd")
		)
		return true;

#endif
	return false;
}

/*zjy
#ifdef _ANGELICA2
ICLImg* CreateImg(const char * szFilename, A3DDevice* pDevice)
{
	if (!isPicture(szFilename))
		return NULL;

	if (isDDS(szFilename)) 
	{
		if (!pDevice)
			return NULL;

		return new ImgImpl_DDS(pDevice);
	}
	return new ImgImpl_xImage;
}

#elif _ANGELICA31
*/

ICLImg* CreateImg(const char * szFilename, A3DDevice* pDevice, A3DGfxEngine* pDGfxEngine, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pDAdditionalView*/)
{
	if (!isPicture(szFilename))
		return NULL;

#ifdef _ANGELICA31
	if (!pDevice || !pDGfxEngine)
		return NULL;
#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)
	if (!pDevice)
		return NULL;
#endif

	if (isDDS(szFilename)) 
	{
		return new ImgImpl_DDS(pDevice);
	}

#if defined _ANGELICA2

	if (isGfx(szFilename))
	{
		return new ImgImpl_Gfx(pDevice, AfxGetGFXExMan(), pDCamera, pDViewport, pRenderTarget);
	}

	if (isECModel(szFilename))
	{
		return new ImgImpl_ECM(pDevice, pDGfxEngine, pDCamera, pDViewport, pRenderTarget);
	}

#elif defined(_ANGELICA22) || defined(_ANGELICA21)
	if (isGfx(szFilename))
	{
		return new ImgImpl_Gfx(pDevice, AfxGetGFXExMan(), pDCamera, pDViewport, pRenderTarget);
	}

	if (isECModel(szFilename))
	{
		return new ImgImpl_ECM(pDevice, pDGfxEngine, pDCamera, pDViewport, pRenderTarget);
	}

	if (isSMD(szFilename))
	{
		return new ImgImpl_SMD(pDevice, pDGfxEngine, pDCamera, pDViewport, pRenderTarget);
	}

#elif defined _ANGELICA31 

	if (isECModel(szFilename))
	{
		return new ImgImpl_ECM(pDevice, pDGfxEngine, pDCamera, pDViewport, pRenderTarget);//pDAdditionalView
	}
	if (isGfx(szFilename))
	{
		return new ImgImpl_Gfx(pDevice, pDGfxEngine->GetA3DGfxExMan(), pDCamera, pDViewport, pRenderTarget);
	}
	if (isSMD(szFilename))
	{
		return new ImgImpl_SMD(pDevice, pDGfxEngine, pDCamera, pDViewport, pRenderTarget);
	}
	if (isUMD(szFilename))
	{
		return new ImgImpl_UMD(pDevice, pDGfxEngine, pDCamera, pDViewport, pRenderTarget);
	}
	if (isUMDS(szFilename))
	{
		return new ImgImpl_UMDS(pDevice, pDGfxEngine, pDCamera, pDViewport, pRenderTarget);
	}
#endif

	return new ImgImpl_xImage;
}
//zjy#endif

ImgImpl_xImage::ImgImpl_xImage()
: m_pBaImage(NULL)
{
	m_pXImage = new CxImage;
	m_pCacheBitmap = new CBitmap;
}

ImgImpl_xImage::~ImgImpl_xImage()
{
	delete m_pXImage;
	delete m_pBaImage;

	if (m_pCacheBitmap)
	{
		m_pCacheBitmap->DeleteObject();
		delete m_pCacheBitmap;
		m_pCacheBitmap = NULL;
	}
}

bool ImgImpl_xImage::ReadFromFile(const TCHAR * szFilename)
{
	DWORD dwReadAsType = CXIMAGE_FORMAT_JPG;
	if (AFCheckFileExt(szFilename, _T(".bmp")))
		dwReadAsType = CXIMAGE_FORMAT_BMP;
	else if (AFCheckFileExt(szFilename, _T(".tga")))
		dwReadAsType = CXIMAGE_FORMAT_TGA;
	else if (AFCheckFileExt(szFilename, _T(".png")))
		dwReadAsType = CXIMAGE_FORMAT_PNG;

	if (!m_pXImage->Load(szFilename, dwReadAsType))
		return false;

	m_Height = m_pXImage->GetHeight();
	m_Width = m_pXImage->GetWidth();

	blendalpha();
	return true;
}

bool ImgImpl_xImage::WriteToFile(const TCHAR* szFilename, int iEncodeType)
{
	return m_pXImage->Save(szFilename, ICLIMGTYPE2CXIMGTYPE(iEncodeType));
}

bool ImgImpl_xImage::ReadFromMemory(unsigned char* pBuf, long nSize, int iEncodeType)
{
	if (!m_pXImage->Decode(pBuf, nSize, ICLIMGTYPE2CXIMGTYPE(iEncodeType)))
		return false;

	blendalpha();
	return true;
}

bool ImgImpl_xImage::ReadFromArray(unsigned char* pArray, int iWid, int iHei, int iBitsperpixel, int iBytesperline, bool bFlipImage, bool alpha)
{
	if (!m_pXImage->CreateFromArray(pArray, iWid, iHei, iBitsperpixel, iBytesperline, bFlipImage))
		return false;

	if (!alpha)
	{
		m_pXImage->AlphaSet(255);
	}	
	blendalpha();
	return true;
}

bool ImgImpl_xImage::Scale(int nDestWid, int nDestHei)
{
	RGBQUAD rgb;
	rgb.rgbRed = 255;
	rgb.rgbGreen = 255;
	rgb.rgbBlue = 255;
	rgb.rgbReserved = 255;
		
	if (nDestWid < 0 || nDestHei < 0)
		return false;

	if (!m_pXImage || !m_pBaImage)
		return false;

	m_pXImage->Thumbnail(nDestWid, nDestHei, rgb);
	m_pBaImage->Thumbnail(nDestWid, nDestHei, rgb);
	return true;
}

bool ImgImpl_xImage::Encode(unsigned char*& pBuf, long& nSize, int iEncodeType)
{
	return m_pXImage->Encode(pBuf, nSize, ICLIMGTYPE2CXIMGTYPE(iEncodeType));
}

void ImgImpl_xImage::FreeMemory(void* pBuf)
{
	m_pXImage->FreeMemory(pBuf);
}

CBitmap* ImgImpl_xImage::GetAsBitmap(bool bIsBlendAlpha) const
{
	if (!m_pXImage || !m_pBaImage || !m_pCacheBitmap)
		return NULL;

	if (bIsBlendAlpha) {
		ASSERT(m_pBaImage);
		if (m_pCacheBitmap->GetSafeHandle())
			m_pCacheBitmap->DeleteObject();

		m_pCacheBitmap->Attach(m_pBaImage->MakeBitmap());
	}
	else
	{
		if (m_pCacheBitmap->GetSafeHandle())
			m_pCacheBitmap->DeleteObject();

		m_pCacheBitmap->Attach(m_pXImage->MakeBitmap());
	}

	return m_pCacheBitmap;
}

void ImgImpl_xImage::blendalpha()
{
	if (m_pBaImage)
		delete m_pBaImage;
	
	m_pBaImage = new CxImage(*m_pXImage);
	
	if (m_pBaImage && m_pBaImage->AlphaIsValid())
		m_pBaImage->AlphaStrip();
}




