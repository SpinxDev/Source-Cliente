#include "ICLStdAfx.h"
#include <A3DDevice.h>
#include <A3DRenderTarget.h>
#include <a3dgfxexMan.h>
#include <EC_Model.h>

#ifdef _ANGELICA31
#include <A3DGFXEngine.h>
#endif

A3DRenderTarget* CreateRenderTarget(A3DDevice* pDev, int iWid, int iHei, bool bTarget, bool bDepth)
{
#if defined _ANGELICA31  || defined(_ANGELICA22) || defined(_ANGELICA21)
	A3DRenderTarget::RTFMT rtfmt;

	rtfmt.iWidth = iWid;
	rtfmt.iHeight = iHei;
	rtfmt.fmtTarget = A3DFMT_A8R8G8B8;
	rtfmt.fmtDepth = A3DFMT_D24X8;
#elif defined _ANGELICA2
	A3DDEVFMT rtfmt;
	rtfmt.nWidth = iWid;
	rtfmt.nHeight = iHei;
	rtfmt.fmtTarget = A3DFMT_A8R8G8B8;
	rtfmt.fmtDepth = A3DFMT_D24X8;
#endif

	std::auto_ptr<A3DRenderTarget> pA3DRenderTarget(new A3DRenderTarget());
	if (!pA3DRenderTarget->Init(pDev, rtfmt, true, false))
		return NULL;

	return pA3DRenderTarget.release();
}


bool ApplyRenderTarget(A3DDevice* pDev, A3DRenderTarget* pRt)
{
#if defined _ANGELICA31 || defined _ANGELICA22 || defined _ANGELICA21
	if (!pRt->ApplyToDevice())
		return false;
#elif defined _ANGELICA2
	pDev->SetRenderTarget(pRt);
#endif
	return true;
}

void RestoreRenderTarget(A3DDevice* pDev, A3DRenderTarget* pRt)
{
#if defined _ANGELICA31 || defined _ANGELICA22 || defined _ANGELICA21
	pRt->WithdrawFromDevice();
#elif defined _ANGELICA2
	pDev->RestoreRenderTarget();
#endif
}


bool CreateXImageFromRT(A3DDevice* pDev, A3DRenderTarget* pRt, CxImage* pTarget)
{
#if defined(_ANGELICA31) || defined(_ANGELICA22) || defined _ANGELICA21
	IDirect3DSurface9* pBkBuffer = pRt->GetTargetSurface()->m_pD3DSurface;
#elif defined _ANGELICA2
	IDirect3DSurface8* pBkBuffer = pRt->GetTargetSurface();
#endif

	if (!pBkBuffer)
		return false;

	D3DSURFACE_DESC desc;
	if (FAILED(pBkBuffer->GetDesc(&desc)))
		return false;

	RECT rc_lock;
	D3DLOCKED_RECT locked_rect;
	rc_lock.left = rc_lock.top = 0;
	rc_lock.bottom = desc.Height;
	rc_lock.right = desc.Width;

#if defined _ANGELICA31 || defined(_ANGELICA22) || defined(_ANGELICA21)
	IDirect3DSurface9* pTmpSurface = NULL;

#ifdef _ANGELICA21
	pTmpSurface = pDev->CreateSystemMemSurface(desc.Width, desc.Height, (A3DFORMAT)desc.Format);
    if (!pTmpSurface)
		return false;
    if (FAILED(pDev->GetRenderTargetData(pBkBuffer, pTmpSurface)))
        return false;
#else
	pTmpSurface = pDev->CreateSystemMemSurface(desc.Width, desc.Height, (A3DFORMAT)desc.Format);
    //if (FAILED(pDev->GetD3DDevice()->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &pTmpSurface, NULL)))
	if(pTmpSurface == NULL)
        return false;
    if (FAILED(pDev->GetRenderTargetData(pBkBuffer, pTmpSurface)))
        return false;
#endif

	if (HRESULT result = (pTmpSurface->LockRect(&locked_rect, &rc_lock, D3DLOCK_READONLY)) )
		return false;

#elif defined _ANGELICA2
	IDirect3DSurface8* pTmpSurface = NULL;
	if (FAILED(pDev->GetD3DDevice()->CreateImageSurface(desc.Width, desc.Height, desc.Format, &pTmpSurface)))
		return false;

	if (HRESULT hval = (pDev->GetD3DDevice()->CopyRects(pBkBuffer, NULL, 0, pTmpSurface, NULL)) )
		return false;

	if (HRESULT result = (pTmpSurface->LockRect(&locked_rect, &rc_lock, D3DLOCK_READONLY)) )
		return false;	
#endif

	bool bRet = pTarget->CreateFromArray((unsigned char*)locked_rect.pBits, desc.Width, desc.Height, 32, locked_rect.Pitch, true);
	pTarget->AlphaSet(255);

	pTmpSurface->UnlockRect();
	pTmpSurface->Release();
	return bRet;
}


A3DGFXEx* LoadGfx(A3DDevice* pDev, A3DGFXExMan* pGfxExMan, const char* szGfxFile)
{
#ifdef _ANGELICA31
	return pGfxExMan->LoadGfx(szGfxFile);
#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)
	return pGfxExMan->LoadGfx(pDev, szGfxFile);
#endif
}

CECModel* LoadECModel(A3DDevice* pDev, A3DGfxEngine* pGfxEngine, const char* szECMFile)
{
	std::auto_ptr<CECModel> pECModel(new CECModel);

#ifdef _ANGELICA31
	pECModel->Init(pGfxEngine, NULL);
	if (!pECModel->Load(szECMFile, true, 0, true, true))
		return NULL;
#elif defined(_ANGELICA2) || defined(_ANGELICA22) || defined(_ANGELICA21)
	if (!pECModel->Load(szECMFile))
		return NULL;
#endif

	if (pECModel->GetA3DSkinModel())
	{	//set light info
#ifdef _ANGELICA31
		A3DSkinModelLight li;
		li.colAmbient = pGfxEngine->GetA3DDevice()->GetAmbientValue();
		li.dl_vDir = Normalize(A3DVECTOR3(0.0f, -0.7f, -1.0f));
		li.dl_color = A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f);
#elif defined(_ANGELICA2) || defined(_ANGELICA22) || defined(_ANGELICA21)
		A3DSkinModel::LIGHTINFO li;
		li.colAmbient = pDev->GetAmbientColor();
		li.vLightDir	= Normalize(A3DVECTOR3(0.0f, -0.7f, -1.0f));
		li.colDirDiff	= A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
		li.colDirSpec	= A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
		li.bPtLight		= false;
#endif
		//li.dl_colSpecular = A3DCOLORVALUE(.3f, .3f, .3f, 1.0f);
		pECModel->GetA3DSkinModel()->SetLightInfo(li);
	}
	else
		return NULL;

	return pECModel.release();
}