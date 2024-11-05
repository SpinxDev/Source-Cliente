// AFileImageWrapper.cpp: implementation of the AFileImageWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AFileDialogDll.h"
#include "AFileImageWrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class AFileImageImpl
{
	
public:

	AFileImageImpl();
	~AFileImageImpl();

public:

	// Read anything as you like, but do check the return value
	bool ReadFromFile(A3DDevice* pDeivce, const TCHAR * szFilename);
	// Write image content to file, with specified encode type
	bool WriteToFile(const TCHAR* szFilename, AFileImageWrapper::EncodeType iType);
	// Scale would keep the original width / height's value, and fill the blank with white color
	bool Scale(int nDestWid = 96, int nDestHei = 96);
	// Get a pointer to a bitmap, you just use this pointer and do not need to delete it or call its DeleteObject()
	CBitmap* GetAsBitmap(bool bIsBlendAlpha = true);
	// Read a image file in memory encoded by PNG
	bool ReadFromMemory(unsigned char* pBuf, long nSize, AFileImageWrapper::EncodeType iType);
	// Create from array
	bool CreateFromArray(unsigned char* pArray, int iWid, int iHei, int iBitsperpixel, int iBytesperline, bool bFlipImage, bool bAlpha);
	// Free memroy allocated from Encode operation
	void FreeMemory(void* pBuf);
	// Encode the image file to a buffer with specified encode
	bool Encode(unsigned char*& pBuf, long& nSize, AFileImageWrapper::EncodeType iEncode);

	static int AFILEENCODE2ICLIMGENCODE(int iEncode);

protected:

	enum {
		FIT_BMP,		// .bmp
		FIT_JPG,		// .jpg
		FIT_DDS,		// .dds
		FIT_PNG,		// .png
		FIT_TGA,		// .tga
		FIT_TIF,		// .tif
		FIT_UNKNOWN,	// Unknown type
	};

	// Get type by filename
	static int GetTypeByFileName(const char * szFilename);

	void clear();
	void clearBitmap();
	void clearImage();

private:

	ICLImg* m_pImg;
	CBitmap* m_pBitmap;
	bool m_bIsDDS;
	bool m_bIsCurBA;

};

int AFileImageImpl::AFILEENCODE2ICLIMGENCODE(int iEncode)
{
	switch (iEncode)
	{
	case AFileImageWrapper::FILETYPE_BMP:
		return ICLImg::ICLIMG_BMP;
	case AFileImageWrapper::FILETYPE_JPG:
		return ICLImg::ICLIMG_JPG;
	case AFileImageWrapper::FILETYPE_TGA:
		return ICLImg::ICLIMG_TGA;
	case AFileImageWrapper::FILETYPE_PNG:
		return ICLImg::ICLIMG_PNG;
	default:
		return ICLImg::ICLIMG_PNG;
	}
}

int AFileImageImpl::GetTypeByFileName(const char * szFilename)
{
	if (af_CheckFileExt(szFilename, ".dds"))
		return FIT_DDS;
	else if (af_CheckFileExt(szFilename, ".bmp"))
		return FIT_BMP;
	else if (af_CheckFileExt(szFilename, ".jpg"))
		return FIT_JPG;
	else if (af_CheckFileExt(szFilename, ".png"))
		return FIT_PNG;
	else if (af_CheckFileExt(szFilename, ".tif"))
		return FIT_TIF;
	else if (af_CheckFileExt(szFilename, ".tga"))
		return FIT_TGA;
	return FIT_UNKNOWN;
}

AFileImageImpl::AFileImageImpl()
: m_pImg(NULL)
, m_pBitmap(NULL)
, m_bIsDDS(false)
, m_bIsCurBA(false)
{

}

AFileImageImpl::~AFileImageImpl()
{
	clear();
}

void AFileImageImpl::clear()
{
	clearBitmap();
	clearImage();
}

void AFileImageImpl::clearBitmap()
{
	m_pBitmap = NULL;
}

void AFileImageImpl::clearImage()
{
	delete m_pImg;
	m_pImg = NULL;
}

bool AFileImageImpl::ReadFromFile(A3DDevice* pDeivce, const TCHAR * szFilename)
{
	int nType = GetTypeByFileName(AC2AS(szFilename));
	if (nType == FIT_UNKNOWN)
		return false;

	if (nType == FIT_DDS)
	{
		clearBitmap();
		if (m_pImg && !m_bIsDDS) 
			clearImage();

		if (!m_pImg)
			m_pImg = new ImgImpl_DDS(pDeivce);
		
		m_bIsDDS = true;
		return m_pImg->ReadFromFile(szFilename);
	}

	else
	{
		clearBitmap();
		if (m_pImg && m_bIsDDS) {
			clearImage();
		}

		if (!m_pImg)
			m_pImg = new ImgImpl_xImage();

		m_bIsDDS = false;
		return m_pImg->ReadFromFile(szFilename);
	}
}

// Create from array
bool AFileImageImpl::CreateFromArray(unsigned char* pArray, int iWid, int iHei, int iBitsperpixel, int iBytesperline, bool bFlipImage, bool bAlpha)
{
	if (!m_pImg)
		m_pImg = new ImgImpl_xImage;

	return m_pImg->ReadFromArray(pArray, iWid, iHei, iBitsperpixel, iBytesperline, bFlipImage, bAlpha);
}

// Encode the image file to a buffer with specified encode
bool AFileImageImpl::Encode(unsigned char*& pBuf, long& nSize, AFileImageWrapper::EncodeType iEncode)
{
	if (!m_pImg)
		return false;

	return m_pImg->Encode(pBuf, nSize, AFILEENCODE2ICLIMGENCODE(iEncode));
}

bool AFileImageImpl::WriteToFile(const TCHAR* szFilename, AFileImageWrapper::EncodeType iType)
{
	if (!m_pImg)
		return false;

	return m_pImg->WriteToFile(szFilename, AFILEENCODE2ICLIMGENCODE(iType));
}

bool AFileImageImpl::ReadFromMemory(unsigned char* pBuf, long nSize, AFileImageWrapper::EncodeType iType)
{
	if (!m_pImg)
		m_pImg = new ImgImpl_xImage;

	return m_pImg->ReadFromMemory(pBuf, nSize, AFILEENCODE2ICLIMGENCODE(iType));
}

bool AFileImageImpl::Scale(int nDestWid, int nDestHei)
{
	if (!m_pImg)
		return false;

	return m_pImg->Scale(nDestWid, nDestHei);
}

void AFileImageImpl::FreeMemory(void* pBuf)
{
	if (!m_pImg)
		return;

	m_pImg->FreeMemory(pBuf);
}

CBitmap* AFileImageImpl::GetAsBitmap(bool bIsBlendAlpha)
{
	if (m_bIsCurBA == bIsBlendAlpha && m_pBitmap)
		return m_pBitmap;
	
	if (!m_pBitmap) {
		m_pBitmap = m_pImg->GetAsBitmap(bIsBlendAlpha);
		m_bIsCurBA = bIsBlendAlpha;
		return m_pBitmap;
	}

	ASSERT(m_pBitmap);
	if (m_bIsCurBA != bIsBlendAlpha)
	{
		m_pBitmap = m_pImg->GetAsBitmap(bIsBlendAlpha);
		m_bIsCurBA = bIsBlendAlpha;
	}

	return m_pBitmap;
}


//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////


AFileImageWrapper::AFileImageWrapper()
: m_pImageImpl(new AFileImageImpl)
{

}

AFileImageWrapper::~AFileImageWrapper()
{
	delete m_pImageImpl;
}

bool AFileImageWrapper::CreateFromArray(unsigned char* pArray, int iWid, int iHei, int iBitsperpixel, int iBytesperline, bool bFlipImage)
{
	return m_pImageImpl->CreateFromArray(pArray, iWid, iHei, iBitsperpixel, iBytesperline, bFlipImage, false);
}

// Create from array
bool AFileImageWrapper::CreateFromArray(unsigned char* pArray, int iWid, int iHei, int iBitsperpixel, int iBytesperline, bool bFlipImage, bool bAlpha)
{
	return m_pImageImpl->CreateFromArray(pArray, iWid, iHei, iBitsperpixel, iBytesperline, bFlipImage, bAlpha);
}

// Read anything as you like, but do check the return value
bool AFileImageWrapper::ReadFromFile(A3DDevice* pDeivce, const TCHAR * szFilename)
{
	return m_pImageImpl->ReadFromFile(pDeivce, szFilename);
}

// Write image to file
bool AFileImageWrapper::WriteToFile(const TCHAR* szFilename, EncodeType iType)
{
	return m_pImageImpl->WriteToFile(szFilename, iType);
}

// Scale would keep the original width / height's value, and fill the blank with white color
bool AFileImageWrapper::Scale(int nDestWid /*= 96*/, int nDestHei /*= 96*/)
{
	return m_pImageImpl->Scale(nDestWid, nDestHei);
}

// Get a pointer to a bitmap, you just use this pointer and do not need to delete it or call its DeleteObject()
CBitmap* AFileImageWrapper::GetAsBitmap(bool bIsBlendAlpha /*= true*/)
{
	return m_pImageImpl->GetAsBitmap(bIsBlendAlpha);
}

// Read a image file in memory encoded by PNG
bool AFileImageWrapper::ReadFromMemory(unsigned char* pBuf, long nSize)
{
	return m_pImageImpl->ReadFromMemory(pBuf, nSize, FILETYPE_PNG);
}

// Read a image file in memory by specified encode
bool AFileImageWrapper::ReadFromMemoryEx(unsigned char* pBuf, long nSize, EncodeType iEncode)
{
	return m_pImageImpl->ReadFromMemory(pBuf, nSize, iEncode);
}

// Encode the image file to a buffer with PNG codec
bool AFileImageWrapper::Encode(unsigned char*& pBuf, long& nSize)
{
	return m_pImageImpl->Encode(pBuf, nSize, FILETYPE_PNG);
}

// Encode the image file to a buffer with specified encode
bool AFileImageWrapper::EncodeEx(unsigned char*& pBuf, long& nSize, EncodeType iEncode)
{
	return m_pImageImpl->Encode(pBuf, nSize, iEncode);
}

void AFileImageWrapper::FreeMemory(void* pBuf)
{
	m_pImageImpl->FreeMemory(pBuf);
}