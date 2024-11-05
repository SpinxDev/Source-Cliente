#ifndef _ICL_IMG_H_
#define _ICL_IMG_H_



class ICLImg
{
public:

	enum 
	{
		ICLIMG_PNG = 0,
		ICLIMG_JPG,
		ICLIMG_BMP,
		ICLIMG_TGA,
	};

public:

	virtual ~ICLImg() = 0 {}
	// load from file, automatically adapt the file type
	virtual bool ReadFromFile(const TCHAR * szFilename) = 0;
	// save support, default is not supported
	virtual bool WriteToFile(const TCHAR* szFilename, int iEncodeType) = 0;
	// Decode from memory, take the buffer as a PNG encoded image
	virtual bool ReadFromMemory(unsigned char* pBuf, long nSize, int iEncodeType = ICLIMG_PNG) = 0;
	// read from memory, take the buffer as a BMP image buffer array
	virtual bool ReadFromArray(unsigned char* pArray, int iWid, int iHei, int iBitsperpixel, int iBytesperline, bool bFlipImage, bool alpha) = 0;
	// scale the image
	virtual bool Scale(int nDestWid = 96, int nDestHei = 96) = 0;
	// encode the image to PNG buffer
	virtual bool Encode(unsigned char*& pBuf, long& nSize, int iEncodeType = ICLIMG_PNG) = 0;
	// free memory allocated by encode
	virtual void FreeMemory(void* pBuf) = 0;
	// Notion: user own the responsibilty to call the CBitmap::DeleteObject when
	//			no longer need the bitmap
	virtual CBitmap* GetAsBitmap(bool bIsBlendAlpha) const = 0;
	DWORD GetImageHeight() const {return m_Height;}
	DWORD GetImageWidth() const {return m_Width;}

	//	Convert ICLImg type to CxImage type, default (unknown type convert to PNG)
	static int ICLIMGTYPE2CXIMGTYPE(int iICLImgType);

protected:
	DWORD m_Height;
	DWORD m_Width;
};

#endif