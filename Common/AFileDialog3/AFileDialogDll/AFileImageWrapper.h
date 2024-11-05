// AFileImageWrapper.h: interface for the AFileImageWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFILEIMAGEWRAPPER_H__28369F3B_AD26_4053_9B87_62015788CD83__INCLUDED_)
#define AFX_AFILEIMAGEWRAPPER_H__28369F3B_AD26_4053_9B87_62015788CD83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AFileExpDecl.h"

class ICLImg;
class A3DDevice;
class AFileImageImpl;

class _AFILE_EXP_ AFileImageWrapper
{

public:

	// encode type used by library user
	enum EncodeType
	{
		FILETYPE_BMP,
		FILETYPE_JPG,
		FILETYPE_PNG,
		FILETYPE_TGA,
	};

public:

	AFileImageWrapper();
	virtual ~AFileImageWrapper();

public:

	// Create from array
	virtual bool CreateFromArray(unsigned char* pArray, int iWid, int iHei, int iBitsperpixel, int iBytesperline, bool bFlipImage);
	virtual bool CreateFromArray(unsigned char* pArray, int iWid, int iHei, int iBitsperpixel, int iBytesperline, bool bFlipImage, bool bAlpha);
	// Read anything as you like, but do check the return value
	virtual bool ReadFromFile(A3DDevice* pDeivce, const TCHAR * szFilename);
	// Write image content to file, with specified encode type
	virtual bool WriteToFile(const TCHAR* szFilename, EncodeType iType);
	// Scale would keep the original width / height's value, and fill the blank with white color
	virtual bool Scale(int nDestWid = 96, int nDestHei = 96);
	// Get a pointer to a bitmap, you just use this pointer and do not need to delete it or call its DeleteObject()
	virtual CBitmap* GetAsBitmap(bool bIsBlendAlpha = true);
	// Read a image file in memory encoded by PNG
	virtual bool ReadFromMemory(unsigned char* pBuf, long nSize);
	// Read a image file in memory by specified encode
	virtual bool ReadFromMemoryEx(unsigned char* pBuf, long nSize, EncodeType iEncode);
	// Encode the image file to a buffer with PNG codec
	virtual bool Encode(unsigned char*& pBuf, long& nSize);
	// Encode the image file to a buffer with specified encode
	virtual bool EncodeEx(unsigned char*& pBuf, long& nSize, EncodeType iEncode);
	// Free memory allocated from Encode operation
	virtual void FreeMemory(void* pBuf);

private:

	AFileImageImpl* m_pImageImpl;

private:

	AFileImageWrapper(const AFileImageWrapper&);
	AFileImageWrapper& operator = (const AFileImageWrapper&);
};

#endif // !defined(AFX_AFILEIMAGEWRAPPER_H__28369F3B_AD26_4053_9B87_62015788CD83__INCLUDED_)
