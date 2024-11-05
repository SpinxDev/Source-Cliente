#include "ICLStdAfx.h"
#include "PlatformIndependFuncs.h"
#include "a3dgfxexman.h"

//////////////////////////////////////////////////////////////////////////
//
//	Impl for ImgImpl_ECM
//
//////////////////////////////////////////////////////////////////////////

#ifdef _ANGELICA31 

#include <d3dx9.h>
#include "a3dgfxengine.h"
#include <A3DAdditionalView.h>
#include "a3dadditionalview.h"

#elif defined _ANGELICA2
#include <d3dx8.h>

#endif

#include "ec_model.h"

ImgImpl_ECM::ImgImpl_ECM(A3DDevice* pDevice, A3DGfxEngine* pDGfxEngine, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget /*, A3DAdditionalView* pDAdditionalView*/)
: m_pDevice(pDevice)
, m_pGfxEngine(pDGfxEngine)
, m_pCamera(pDCamera)
, m_pViewport(pDViewport)
, m_pRenderTarget(pRenderTarget)
{
	ASSERT(m_pDevice);
	m_Height = 0;
	m_Width = 0;
}

ImgImpl_ECM::~ImgImpl_ECM()
{
	A3DRELEASE(m_pECModel);
}

bool ImgImpl_ECM::ReadFromFile(const TCHAR * szFilename)
{

	AString strECModel;

#ifdef _ANGELICA31
	if  (!m_pDevice || !m_pGfxEngine || !m_pCamera || !m_pViewport ) 
		return false;
#elif defined _ANGELICA2
	if  (!m_pDevice || !m_pCamera || !m_pViewport ) 
		return false;
#endif

	strECModel = AC2AS(szFilename);
	m_pECModel = LoadECModel(m_pDevice, m_pGfxEngine, strECModel);
	if (!m_pECModel)
		return false;

	m_pECModel->Tick(50);

	A3DAABB aabb;
	if (m_pECModel->GetA3DSkinModel())
		aabb = m_pECModel->GetA3DSkinModel()->GetModelAABB();
	else
		aabb.Maxs = aabb.Mins = aabb.Extents = aabb.Center = 0.0f;

	float fHeight = aabb.Maxs.y - aabb.Mins.y;
	a_ClampFloor(fHeight,1.f);

	A3DVECTOR3 vExt(aabb.Maxs.x - aabb.Mins.x, 0, aabb.Maxs.z - aabb.Mins.z);
	float fRadius = vExt.Magnitude() * 2;
	a_ClampFloor(fRadius, 2.f);

	float fRad = DEG2RAD(100/100);

	A3DVECTOR3 vLookAt(0,fHeight/2,0);
	if (m_pECModel->GetA3DSkinModel())
		vLookAt = vLookAt + m_pECModel->GetA3DSkinModel()->GetPos();
	else
		vLookAt = vLookAt + m_pECModel->GetPos();
	A3DVECTOR3 vPos(fRadius,fHeight/2,0);

	A3DVECTOR3 vDir (vLookAt - vPos);
	m_pCamera->SetPos(vPos);
	m_pCamera->SetDirAndUp(vDir, A3DVECTOR3(0.0f, 1.0f, 0.0f));

	m_pEngine = m_pDevice->GetA3DEngine();
	if (!m_pEngine)
		return false;

	if (!m_pEngine->BeginRender())
		return false;

	if (!ApplyRenderTarget(m_pDevice, m_pRenderTarget))
		return false;

	m_pViewport->Active();
	m_pViewport->ClearDevice();

	m_pECModel->Render(m_pViewport);

	m_pEngine->GetA3DLitModelRender()->Render(m_pViewport);
	m_pEngine->GetA3DDevice()->SetAlphaBlendEnable(false);

#ifdef _ANGELICA31
	m_pEngine->GetA3DSkinMan()->GetCurSkinRender()->Render(m_pViewport, A3DSkinRender::RD_ALL);
#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)
	m_pEngine->GetA3DSkinMan()->GetCurSkinRender()->Render(m_pViewport, false);
#endif
	m_pEngine->GetA3DDevice()->SetAlphaBlendEnable(true);

#ifdef _ANGELICA31
	m_pGfxEngine->GetA3DGfxExMan()->RenderAllGfx(m_pViewport);
#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)
	AfxGetGFXExMan()->RenderAllGfx(m_pViewport);
#endif

	if (!m_pEngine->EndRender())
		return false;

	RestoreRenderTarget(m_pDevice, m_pRenderTarget);

	if (!CreateXImageFromRT(m_pDevice, m_pRenderTarget, m_pXImage))
		return false;

	blendalpha();

	return true;
}

