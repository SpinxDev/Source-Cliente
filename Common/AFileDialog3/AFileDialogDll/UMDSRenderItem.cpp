#include "stdAfx.h"
#ifdef _ANGELICA31

#include <A3DAdditionalView.h>
#include "AFileDialog.h"
#include "AFilePreviewWnd.h"
#include "Render.h"
#include "UMDSRenderItem.h"

UMDSRenderItem::UMDSRenderItem(const char* szUMDS, A3DDevice* pA3DDevice)
{
	ASSERT(szUMDS && szUMDS[0] && pA3DDevice);
	m_strUMDS = szUMDS;
	m_pUMDS = new A3DUnlitModelSet;
	m_pA3DDevice = pA3DDevice;
}

UMDSRenderItem::~UMDSRenderItem()
{
	Release();
}

bool UMDSRenderItem::Init()
{
	if (!m_pUMDS->Load(m_pA3DDevice,m_strUMDS))
		return false;
	if (!m_pUMDS->GetNumModels())
		return false;
	return true;
}

void UMDSRenderItem::Release()
{
	A3DRELEASE(m_pUMDS);
}

void UMDSRenderItem::Play()
{

}

void UMDSRenderItem::Stop()
{

}

void UMDSRenderItem::Tick(DWORD dwTick)
{

}

void UMDSRenderItem::Render(A3DEngine* pEngine, A3DViewport* pA3DViewport)
{
	m_pUMDS->Render(pA3DViewport);
}

A3DVECTOR3 UMDSRenderItem::GetPos() const
{
	return m_pUMDS->GetModel(0)->GetPos();
}

void UMDSRenderItem::GetAABB(A3DAABB *aabbset) const
{
	if (!aabbset)
		return;
	int umdnum = m_pUMDS->GetNumModels();
	*aabbset = m_pUMDS->GetModel(0)->GetModelAABB();

	for (int nIdx = 1; nIdx < umdnum; nIdx++)
	{
		A3DAABB aabb = m_pUMDS->GetModel(nIdx)->GetModelAABB();
		if (aabb.Mins.x < aabbset->Mins.x)
			aabbset->Mins.x = aabb.Mins.x;
		if (aabb.Mins.y < aabbset->Mins.y)
			aabbset->Mins.y = aabb.Mins.y;
		if (aabb.Mins.z < aabbset->Mins.z)
			aabbset->Mins.z = aabb.Mins.z;

		if (aabb.Maxs.x > aabbset->Maxs.x)
			aabbset->Maxs.x = aabb.Maxs.x;
		if (aabb.Maxs.y > aabbset->Maxs.y)
			aabbset->Maxs.y = aabb.Maxs.y;
		if (aabb.Maxs.z > aabbset->Maxs.z)
			aabbset->Maxs.z = aabb.Maxs.z;		
	}
	
}


#endif