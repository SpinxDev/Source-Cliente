#ifndef _IMG_IMPL_UMDS_H_
#define _IMG_IMPL_UMDS_H_

#if defined _ANGELICA31

class A3DDevice;
class A3DEngine;
class A3DGfxEngine;
class A3DCamera;
class A3DViewport;
class A3DAdditionalView;
class A3DRenderTarget;

class ImgImpl_UMDS : public ImgImpl_xImage
{
public:
	ImgImpl_UMDS(A3DDevice* pDevice, A3DGfxEngine* pGfxEngine, A3DCamera* pCamera, A3DViewport* pViewPort, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pAdditionalView*/);
	~ImgImpl_UMDS();
protected:
	virtual bool ReadFromFile(const TCHAR* szFilename);
private:
	A3DCamera* m_pCamera;
	A3DViewport* m_pViewport;
	//A3DAdditionalView* m_pAdditionalView;
	A3DRenderTarget* m_pRenderTarget;
	A3DDevice* m_pDevice;
	A3DEngine* m_pEngine;
	A3DGfxEngine* m_pGfxEngine;
	A3DUnlitModelSet* m_pUmds;

};


#endif

#endif