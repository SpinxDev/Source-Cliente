#ifndef _IMG_IMPL_SMD_H_
#define _IMG_IMPL_SMD_H_

#if defined _ANGELICA31 || defined(_ANGELICA22) || defined(_ANGELICA21) || defined(_ANGELICA2)

class A3DDevice;
class A3DGfxEngine;
class A3DCamera;
class A3DViewport;
class A3DAdditionalView;
class A3DSkinModel;
class A3DRenderTarget;

class ImgImpl_SMD : public ImgImpl_xImage
{
public:
	ImgImpl_SMD(A3DDevice* pDevice, A3DGfxEngine* pDGfxEngine, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pDAdditionalView*/);
	~ImgImpl_SMD();
protected:
	virtual bool ReadFromFile(const TCHAR * szFilename);
private:
	A3DCamera* m_pCamera;
	A3DViewport* m_pViewport;
	//A3DAdditionalView* m_pAdditionalView;
	A3DRenderTarget* m_pRenderTarget;
	A3DDevice* m_pDevice;
	A3DEngine* m_pEngine;
	A3DGfxEngine* m_pGfxEngine;
	A3DSkinModel* m_pSmd;

};

#endif

#endif