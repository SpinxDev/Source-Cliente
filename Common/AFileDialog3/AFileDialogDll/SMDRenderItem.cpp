#include "stdAfx.h"

#ifdef _ANGELICA31

#include <A3DAdditionalView.h>
#include "AFileDialog.h"
#include "AFilePreviewWnd.h"
#include "Render.h"
#include "SMDRenderItem.h"

SMDRenderItem::SMDRenderItem(const char* szSMD, A3DDevice* pA3DDevice)
{
	ASSERT(szSMD && szSMD[0] && pA3DDevice);
	m_strSMD = szSMD;
	m_pSMD = new A3DSkinModel;
	m_pA3DDevice = pA3DDevice;
	m_pSMD->Init(m_pA3DDevice->GetA3DEngine());
}

SMDRenderItem::~SMDRenderItem()
{
	Release();
}

bool SMDRenderItem::Init()
{
	if (!m_pSMD->Load(m_strSMD))
		return false;
	// Set light info
	A3DSkinModelLight li;
	li.colAmbient = m_pA3DDevice->GetAmbientValue();
	li.dl_vDir = Normalize(A3DVECTOR3(0.0f, -0.7f, -1.0f));
	li.dl_color = A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f);
	//li.dl_colSpecular = A3DCOLORVALUE(.3f, .3f, .3f, 1.0f);
	m_pSMD->SetLightInfo(li);
	m_pSMD->Update(1);
	return true;
}

void SMDRenderItem::Release()
{
	A3DRELEASE(m_pSMD);
}

void SMDRenderItem::Play()
{

}

void SMDRenderItem::Stop()
{

}

void SMDRenderItem::Tick(DWORD dwTick)
{

}

void SMDRenderItem::Render(A3DEngine* pEngine, A3DViewport* pA3DViewport)
{
	m_pSMD->Render(pA3DViewport);
}

A3DVECTOR3 SMDRenderItem::GetPos() const
{
	return m_pSMD->GetPos();
}

void SMDRenderItem::GetAABB(A3DAABB* aabb) const
{
	if (!aabb)
		return;
	*aabb = m_pSMD->GetModelAABB();

}

#endif