#include "stdAfx.h"

#include "AFileDialog.h"
#include "AFilePreviewWnd.h"
#include "A3DGfxEx.h"
#include "Render.h"
#include "GfxRenderItem.h"


#ifdef _ANGELICA31
#include <A3DAdditionalView.h>
#elif defined _ANGELICA2
#include "a3dgfxexman.h"
#endif

GfxRenderItem::GfxRenderItem(const char* szGfx, A3DGfxEngine* pGfxEngine)
{
#ifdef _ANGELICA31
	ASSERT(szGfx && szGfx[0] && pGfxEngine);
#elif defined _ANGELICA2
	ASSERT(szGfx && szGfx[0]);
#endif

	m_pGfxEngine = pGfxEngine;

	AString strGfxBase = af_GetBaseDir();
	strGfxBase += "\\Gfx";
	af_GetRelativePathNoBase(szGfx, strGfxBase, m_strGfxPath);

	m_pGfx = NULL;
}

GfxRenderItem::~GfxRenderItem()
{
	Release();
}

bool GfxRenderItem::Init()
{
#ifdef _ANGELICA31
	m_pGfx = m_pGfxEngine->GetA3DGfxExMan()->LoadGfx(m_strGfxPath);
#elif defined _ANGELICA2
	m_pGfx = AfxGetGFXExMan()->LoadGfx(CRender::GetInstance()->GetA3DDevice(),m_strGfxPath);
#endif

	if (!m_pGfx)
		return false;

	return true;
}

void GfxRenderItem::Release()
{
	A3DRELEASE(m_pGfx);
}

void GfxRenderItem::Play()
{
	if (m_pGfx)
		m_pGfx->Start();
}

void GfxRenderItem::Stop()
{
	if (m_pGfx)
		m_pGfx->Stop();
}

void GfxRenderItem::Tick(DWORD dwTick)
{
	if (m_pGfx)
		m_pGfx->TickAnimation(dwTick);
}

void GfxRenderItem::Render(A3DEngine* pEngine, A3DViewport* pA3DViewport)
{
#ifdef _ANGELICA31
	if (m_pGfx)
		m_pGfxEngine->GetA3DGfxExMan()->RegisterGfx(m_pGfx);
#elif defined _ANGELICA2
	if (m_pGfx)
		AfxGetGFXExMan()->RegisterGfx(m_pGfx);
#endif

 }

A3DVECTOR3 GfxRenderItem::GetPos() const
{
	if (m_pGfx)
		return m_pGfx->GetPos();
	
	return A3DVECTOR3(0.f);
}

void GfxRenderItem::GetAABB(A3DAABB* aabb) const
{
	if (!aabb)
		return;

	aabb->Clear();

	if (m_pGfx)
		*aabb = m_pGfx->GetAABBOrg();
	else
		aabb->Maxs = aabb->Mins = aabb->Extents = aabb->Center = 0.0f;
}

