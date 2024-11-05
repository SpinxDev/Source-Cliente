#include "ICLStdAfx.h"
#include "PlatformIndependFuncs.h"

//////////////////////////////////////////////////////////////////////////
//
//	Impl for ImgImpl_Gfx
//
//////////////////////////////////////////////////////////////////////////

#ifdef _ANGELICA31

#include <d3dx9.h>
#include <A3DAdditionalView.h>
#include "a3dadditionalview.h"
#include "a3dgfxengine.h"
#include "ec_model.h"
#include "a3dgfxexman.h"
#include "A3DGfxEx.h"

#elif defined _ANGELICA2

#include <d3dx8.h>
#include "EC_Model.h"
#include "A3DGFXExMan.h"

#elif defined(_ANGELICA22)

#include "A3DPlatform.h"
#include "EC_Model.h"
#include "A3DGFXExMan.h"

#elif defined(_ANGELICA21)
#include <d3dx9.h>
#include "EC_Model.h"
#include "A3DGFXExMan.h"

#endif







ImgImpl_Gfx::ImgImpl_Gfx(A3DDevice* pDevice, A3DGFXExMan* pGfxMan, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pDAdditionalView*/)
: m_pDevice(pDevice)
, m_pGfxMan(pGfxMan)
, m_pCamera(pDCamera)
, m_pViewport(pDViewport)
, m_pRenderTarget(pRenderTarget)
{
	ASSERT(m_pDevice);
	m_Height = 0;
	m_Width = 0;
}

ImgImpl_Gfx::~ImgImpl_Gfx()
{
	A3DRELEASE(m_pGfx);
}

bool ImgImpl_Gfx::ReadFromFile(const TCHAR * szFilename)
{
	AString m_strGfx;

	if (!m_pDevice || !m_pCamera || !m_pViewport ) 
		return false;

	AString strFilename = AC2AS(szFilename);
	AString strGfxBase = af_GetBaseDir();
	strGfxBase += "\\Gfx";
	af_GetRelativePathNoBase(strFilename, strGfxBase, m_strGfx);
	
	m_pGfx = LoadGfx(m_pDevice, m_pGfxMan, m_strGfx);

	if (!m_pGfx)
		return false;

	if (!m_pGfx->Start())
		return false;

	A3DAABB aabb;
	aabb.Clear();

	if (m_pGfx)
		aabb = m_pGfx->GetAABBOrg();
	else
		aabb.Maxs = aabb.Mins = aabb.Extents = aabb.Center = 0.0f;

	float fHeight = aabb.Maxs.y - aabb.Mins.y;

	float fRadius = aabb.Maxs.x - aabb.Mins.x;
	A3DVECTOR3 vPos;

	a_ClampFloor(fRadius, fHeight);

	if (fHeight > 0.2)
	{
		fRadius = fRadius / 2.f + aabb.Maxs.z;
		a_ClampFloor(fHeight,1.f);
		vPos = (0.0f, fHeight / 2.f, fRadius);
	}
	else
	{	
		fRadius = fRadius + aabb.Maxs.z;
		vPos = (0.0f, fHeight + fRadius * cos(0.3f), fRadius * sin(0.3f));
	}

	A3DVECTOR3 vLookAt(0,fHeight/2,0);
	vLookAt = vLookAt + m_pGfx->GetPos();

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

	for (int i = 0; i < 10; ++i)
	{
		m_pGfx->TickAnimation(50);
		m_pGfxMan->RegisterGfx(m_pGfx);
		m_pGfxMan->RenderAllGfx(m_pViewport);
	}

	if (!m_pEngine->EndRender())
		return false;

	RestoreRenderTarget(m_pDevice, m_pRenderTarget);

	m_pGfx->Stop();

	if (!CreateXImageFromRT(m_pDevice, m_pRenderTarget, m_pXImage))
		return false;

	blendalpha();

	return true;

}
