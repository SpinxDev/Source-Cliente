#ifndef _IMG_IMPL_DDS_H_
#define _IMG_IMPL_DDS_H_

class A3DTexture;
class A3DDevice;
class ImgImpl_xImage;

#ifdef _ANGELICA2
#include <d3d8.h>
#define PtrD3DSurface LPDIRECT3DSURFACE8
#elif defined(_ANGELICA31) || defined(_ANGELICA22) || defined(_ANGELICA21)
	#ifdef _ANGELICA22
		#include "A3DPlatform.h"
	#else
		#include <d3d9.h>
	#endif
struct IDirect3DSurface9;
#define PtrD3DSurface LPDIRECT3DSURFACE9

#else
#error "You must define a Angelica version macro"

#endif

class ImgImpl_DDS : public ImgImpl_xImage
{
public:
	ImgImpl_DDS(A3DDevice* pDevice);
	~ImgImpl_DDS();

protected:
	virtual bool ReadFromFile(const TCHAR * szFilename);

private:
	A3DDevice* m_pDevice;
	A3DTexture* m_pTex;
	PtrD3DSurface m_pD3DSurface;
};

#endif