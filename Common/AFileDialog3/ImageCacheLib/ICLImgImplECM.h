#ifndef _IMG_IMPL_ECM_H_
#define _IMG_IMPL_ECM_H_

//zjy#ifdef _ANGELICA31

class A3DDevice;
class A3DGfxEngine;
class A3DCamera;
class A3DViewport;
//class A3DAdditionalView;
class A3DEngine;
class CECModel;
class A3DRenderTarget;

class ImgImpl_ECM : public ImgImpl_xImage
{
public:
	ImgImpl_ECM(A3DDevice* pDevice, A3DGfxEngine* pDGfxEngine, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pDAdditionalView*/);
	~ImgImpl_ECM();

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
	CECModel* m_pECModel;
};

//zjy#endif
#endif