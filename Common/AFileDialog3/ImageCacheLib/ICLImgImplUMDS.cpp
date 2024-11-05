#include "ICLStdAfx.h"
//////////////////////////////////////////////////////////////////////////
//
//	Impl for ImgImpl_UMDS
//
//////////////////////////////////////////////////////////////////////////
#if defined _ANGELICA31
#include <d3dx9.h>
#include "a3dgfxengine.h"
#include "a3dgfxexman.h"
#include "A3DGfxEx.h"
#include <A3DAdditionalView.h>
#include "a3dadditionalview.h"

ImgImpl_UMDS::ImgImpl_UMDS(A3DDevice* pDevice, A3DGfxEngine* pGfxEngine, A3DCamera* pCamera, A3DViewport* pViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pAdditionalView*/)
: m_pDevice(pDevice)
, m_pGfxEngine(pGfxEngine)
, m_pCamera(pCamera)
, m_pViewport(pViewport)
, m_pRenderTarget(pRenderTarget)
//, m_pAdditionalView(pAdditionalView)
{
	ASSERT(m_pDevice);
	m_Height = 0;
	m_Width = 0;
}

ImgImpl_UMDS::~ImgImpl_UMDS()
{
	A3DRELEASE(m_pUmds);
}

bool ImgImpl_UMDS::ReadFromFile(const TCHAR* szFilename)
{
	if (!m_pDevice || !m_pGfxEngine || !m_pCamera || !m_pViewport/* || !m_pAdditionalView*/)
		return false;
	
	m_pEngine = m_pDevice->GetA3DEngine();
	if (!m_pEngine)
		return false;

	bool bRet;
	AString strumds;
	strumds = AC2AS(szFilename);
	m_pUmds = new A3DUnlitModelSet;
	if (!m_pUmds->Load(m_pDevice, strumds))
		return false;

	int umdnum = m_pUmds->GetNumModels();
	if (!umdnum || umdnum < 1)
		return false;

	A3DAABB aabb;
	aabb = m_pUmds->GetModel(0)->GetModelAABB();
	A3DVECTOR3 Min(aabb.Mins.x, aabb.Mins.y, aabb.Mins.z);
	A3DVECTOR3 Max(aabb.Maxs.x, aabb.Maxs.y, aabb.Maxs.z);

	A3DVECTOR3 pos = m_pUmds->GetModel(0)->GetPos();

	for (int nIdx = 1; nIdx < umdnum; nIdx++)
	{
		aabb = m_pUmds->GetModel(nIdx)->GetModelAABB();
		if (aabb.Mins.x < Min.x)
			Min.x = aabb.Mins.x;
		if (aabb.Mins.y < Min.y)
			Min.y = aabb.Mins.y;
		if (aabb.Mins.z < Min.z)
			Min.z = aabb.Mins.z;

		if (aabb.Maxs.x > Max.x)
			Max.x = aabb.Maxs.x;
		if (aabb.Maxs.y > Max.y)
			Max.y = aabb.Maxs.y;
		if (aabb.Maxs.z > Max.z)
			Max.z = aabb.Maxs.z;		
	}

	//还需要修改下摄像机放置的位置，来调整下距离
	float fHeight = Max.y - Min.y;
	a_ClampFloor(fHeight, 1.f);

	float fdetx = Max.x - Min.x;
	float fdetz = Max.z - Min.z;

	float fRadius;
	A3DVECTOR3 vPos(0.f, 0.f, 0.f);
	if (fdetx > fdetz)
	{
		fRadius = Max.z + max(fHeight, fdetx);
		vPos.Set(0, fHeight/2, fRadius);
	}
	else
	{
		fRadius = Max.x + max(fHeight, fdetz);
		vPos.Set(fRadius, fHeight/2, 0);
	}
	
	A3DVECTOR3 vLookAt(0, fHeight/2, 0);
	vLookAt = vLookAt + pos;

	A3DVECTOR3 vDir(vLookAt - vPos);

	m_pCamera->SetPos(vPos);
	m_pCamera->SetDirAndUp(vDir, A3DVECTOR3::vAxisY);

	if (!m_pEngine->BeginRender())
		return false;
	/*
	if (!m_pAdditionalView->ApplyToDevice())
		return false;
	*/
	if (!m_pRenderTarget->ApplyToDevice())
		return false;

	m_pViewport->Active();
	m_pViewport->ClearDevice();

	m_pUmds->Render(m_pViewport);

	if (!m_pEngine->EndRender())
		return false;

	//m_pAdditionalView->WithdrawFromDevice();
	m_pRenderTarget->WithdrawFromDevice();

	RECT rc_lock;
	D3DLOCKED_RECT locked_rect;
	rc_lock.left = rc_lock.top = 0;
	/*
	rc_lock.right = m_pAdditionalView->GetHeight();
	rc_lock.bottom = m_pAdditionalView->GetWidth();
	IDirect3DSurface9* pBkBuffer = m_pAdditionalView->GetBackBuffer()->m_pD3DSurface;
	*/
	rc_lock.right = m_pRenderTarget->GetHeight();
	rc_lock.bottom = m_pRenderTarget->GetWidth();
	IDirect3DSurface9* pBkBuffer = m_pRenderTarget->GetTargetSurface()->m_pD3DSurface;

	if (!pBkBuffer)
		return false;
	D3DSURFACE_DESC desc;
	if (FAILED(pBkBuffer->GetDesc(&desc)))
		return false;
	IDirect3DSurface9* pTmpSurface = NULL;
	if (FAILED(m_pDevice->GetD3DDevice()->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &pTmpSurface, NULL)))
		return false;
	if (FAILED(m_pDevice->GetD3DDevice()->GetRenderTargetData(pBkBuffer, pTmpSurface)))
		return false;
	if (HRESULT result = (pTmpSurface->LockRect(&locked_rect, &rc_lock, D3DLOCK_READONLY)))
		return false;
	bRet = m_pXImage->CreateFromArray((unsigned char*)locked_rect.pBits, rc_lock.right, rc_lock.bottom, 32, locked_rect.Pitch, true);
	m_pXImage->AlphaSet(255);
	pTmpSurface->UnlockRect();
	pTmpSurface->Release();

	if (!bRet)
		return false;

	blendalpha();

	return true;

}

#endif