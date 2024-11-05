#ifndef _IMG_IMPL_H_
#define _IMG_IMPL_H_

class CxImage;
class ICLImg;

class ImgImpl_xImage : public ICLImg
{
public:
	ImgImpl_xImage();
	~ImgImpl_xImage();
	virtual bool ReadFromFile(const TCHAR * szFilename);
	virtual bool WriteToFile(const TCHAR* szFilename, int iEncodeType);
	virtual bool ReadFromMemory(unsigned char* pBuf, long nSize, int iEncodeType);
	virtual bool ReadFromArray(unsigned char* pArray, int iWid, int iHei, int iBitsperpixel, int iBytesperline, bool bFlipImage, bool alpha);
	virtual bool Scale(int nDestWid = 96, int nDestHei = 96);
	virtual bool Encode(unsigned char*& pBuf, long& nSize, int iEncodeType);
	virtual void FreeMemory(void* pBuf);
	virtual CBitmap* GetAsBitmap(bool bIsBlendAlpha) const;

protected:
	void blendalpha();
	CxImage* m_pXImage;
	CxImage* m_pBaImage;	// Image with alpha layer blended
	CBitmap* m_pCacheBitmap;// Cached Bitmap
};


#endif
