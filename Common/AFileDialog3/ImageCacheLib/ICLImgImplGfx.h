#ifndef _IMG_IMPL_GFX_H_
#define _IMG_IMPL_GFX_H_



class A3DDevice;
class A3DCamera;
class A3DViewport;
class A3DRenderTarget;
//class A3DAdditionalView;
class A3DGfxEngine;
class A3DGFXEx;
class A3DGFXExMan;


class ImgImpl_Gfx : public ImgImpl_xImage
{
public:
	ImgImpl_Gfx(A3DDevice* pDevice, A3DGFXExMan* pGfxMan, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pDAdditionalView*/);
	~ImgImpl_Gfx();
protected:
	virtual bool ReadFromFile(const TCHAR * szFilename);
private:
	A3DCamera* m_pCamera;
	A3DViewport* m_pViewport;
	A3DRenderTarget* m_pRenderTarget;
	A3DDevice* m_pDevice;
	A3DEngine* m_pEngine;
	A3DGFXExMan* m_pGfxMan;
	A3DGFXEx* m_pGfx;

};

#endif