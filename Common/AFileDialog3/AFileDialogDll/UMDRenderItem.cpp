#include "stdAfx.h"
#ifdef _ANGELICA31

#include <A3DAdditionalView.h>
#include "AFileDialog.h"
#include "AFilePreviewWnd.h"
#include "Render.h"
#include "UMDRenderItem.h"

UMDRenderItem::UMDRenderItem(const char* szUMD, A3DDevice* pA3DDevice)
{
	ASSERT(szUMD && szUMD[0] && pA3DDevice);
	m_strUMD = szUMD;
	m_pUMD = new A3DUnlitModel;
	m_pA3DDevice = pA3DDevice;
	m_pUMD->Init(m_pA3DDevice);
}

UMDRenderItem::~UMDRenderItem()
{
	Release();
}

bool UMDRenderItem::Init()
{
	if (!m_pUMD->Load(m_pA3DDevice,m_strUMD))
		return false;

	return true;
}

void UMDRenderItem::Release()
{
	A3DRELEASE(m_pUMD);
}

void UMDRenderItem::Play()
{

}

void UMDRenderItem::Tick(DWORD dwTick)
{

}

void UMDRenderItem::Stop()
{

}

void UMDRenderItem::Render(A3DEngine* pEngine, A3DViewport* pA3DViewport)
{
	m_pUMD->Render(pA3DViewport);
}

A3DVECTOR3 UMDRenderItem::GetPos() const
{
	return m_pUMD->GetPos();
}

void UMDRenderItem::GetAABB(A3DAABB *aabb) const
{
	if (!aabb)
		return;
	*aabb = m_pUMD->GetModelAABB();
}


#endif