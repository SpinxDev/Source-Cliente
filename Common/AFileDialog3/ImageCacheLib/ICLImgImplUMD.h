#ifndef _IMG_IMPL_UMD_H_
#define _IMG_IMPL_UMD_H_

#if defined _ANGELICA31

class A3DDevice;
class A3DGfxEngine;
class A3DCamera;
class A3DViewport;
class A3DAdditionalView;
class A3DUnlitModel;
class A3DRenderTarget;

class ImgImpl_UMD : public ImgImpl_xImage
{
public:
	ImgImpl_UMD(A3DDevice* pDevice, A3DGfxEngine* pGfxEngine, A3DCamera* pCamera, A3DViewport* pViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pAdditionalView*/);
	~ImgImpl_UMD();
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
	A3DUnlitModel* m_pUmd;

};

#endif

#endif