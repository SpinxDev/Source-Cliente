#include "ICLStdAfx.h"
#include "PlatformIndependFuncs.h"

//////////////////////////////////////////////////////////////////////////
//
//	Impl for ImgImpl_SMD
//
//////////////////////////////////////////////////////////////////////////

#if defined _ANGELICA31

#include <d3dx9.h>
#include "a3dgfxengine.h"
#include "a3dgfxexman.h"
#include "A3DGfxEx.h"
#include "ec_model.h"
#include <A3DAdditionalView.h>
#include "a3dadditionalview.h"

#endif

ImgImpl_SMD::ImgImpl_SMD(A3DDevice* pDevice, A3DGfxEngine* pDGfxEngine, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pDAdditionalView*/)
: m_pDevice(pDevice)
, m_pGfxEngine(pDGfxEngine)
, m_pCamera(pDCamera)
, m_pViewport(pDViewport)
, m_pRenderTarget(pRenderTarget)
//, m_pAdditionalView(pDAdditionalView)

{
	ASSERT(m_pDevice);
	m_Height = 0;
	m_Width = 0;
}

ImgImpl_SMD::~ImgImpl_SMD()
{
	A3DRELEASE(m_pSmd);
}

bool ImgImpl_SMD::ReadFromFile(const TCHAR * szFilename)
{

	if (!m_pDevice || !m_pCamera || !m_pViewport/* || !m_pAdditionalView*/)
		return false;

	AString strSmd;
	strSmd = AC2AS(szFilename);
	m_pEngine = m_pDevice->GetA3DEngine();
	if (!m_pEngine)
		return false;

	m_pSmd = new A3DSkinModel;
	if (!m_pSmd->Init(m_pEngine))
		return false;

	if (!m_pSmd->Load(strSmd))
		return false;

	m_pSmd->Update(1);
#ifdef _ANGELICA31
	A3DSkinModelLight li;
	li.colAmbient = m_pDevice->GetAmbientValue();
	li.dl_vDir = Normalize(A3DVECTOR3(0.0f, -0.7f, -1.0f));
	li.dl_color = A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f);
	//li.dl_colSpecular = A3DCOLORVALUE(.3f, .3f, .3f, 1.0f);
	m_pSmd->SetLightInfo(li);
#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)
	A3DSkinModel::LIGHTINFO LightInfo;
	LightInfo.colAmbient	= m_pDevice->GetAmbientColor();
	LightInfo.vLightDir		= Normalize(A3DVECTOR3(0.0f, -0.7f, -1.0f));//Direction of directional light
	LightInfo.colDirDiff	= A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);//Directional light's diffuse color
	LightInfo.colDirSpec	= A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);//	Directional light's specular color
	LightInfo.bPtLight		= false;//	false, disable dynamic point light,
	m_pSmd->SetLightInfo(LightInfo);
#endif	
	
	A3DAABB aabb;
	aabb = m_pSmd->GetModelAABB();

	float fHeight = aabb.Maxs.y - aabb.Mins.y;
	a_ClampFloor(fHeight,1.f);
	A3DVECTOR3 vExt(aabb.Maxs.x - aabb.Mins.x, 0, aabb.Maxs.z - aabb.Maxs.z);
	float fRadius = vExt.Magnitude() * 2.f;
	a_ClampFloor(fRadius, 2.f);

	A3DVECTOR3 vLookAt(0, fHeight/2, 0);
	vLookAt = vLookAt + m_pSmd->GetPos();

	A3DVECTOR3 vPos(fRadius, fHeight/2, 0);

	A3DVECTOR3 vDir(vLookAt - vPos);
	m_pCamera->SetPos(vPos);
	m_pCamera->SetDirAndUp(vDir, A3DVECTOR3(0.0f, 1.0f, 0.0f));

	if (!m_pEngine->BeginRender())
		return false;
	/*
	if (!m_pAdditionalView->ApplyToDevice())
		return false;
		*/
	if (!ApplyRenderTarget(m_pDevice, m_pRenderTarget))
		return false;

	m_pViewport->Active();
	m_pViewport->ClearDevice();

	m_pSmd->Render(m_pViewport);
	m_pEngine->GetA3DLitModelRender()->Render(m_pViewport);
	m_pDevice->SetAlphaBlendEnable(false);
#ifdef _ANGELICA31
	m_pEngine->GetA3DSkinMan()->GetCurSkinRender()->Render(m_pViewport, A3DSkinRender::RD_ALL);
#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)
	m_pEngine->GetA3DSkinMan()->GetCurSkinRender()->Render(m_pViewport, false);
#endif
	m_pDevice->SetAlphaBlendEnable(true);

	if (!m_pEngine->EndRender())
		return false;

	RestoreRenderTarget(m_pDevice, m_pRenderTarget);

	if (!CreateXImageFromRT(m_pDevice, m_pRenderTarget, m_pXImage))
		return false;

	blendalpha();

	return true;
}
