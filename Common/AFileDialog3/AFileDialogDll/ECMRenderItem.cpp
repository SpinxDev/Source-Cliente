#include "stdAfx.h"

#ifdef _ANGELICA31
#include <A3DAdditionalView.h>
#endif

#include "AFileDialog.h"
#include "AFilePreviewWnd.h"
#include "Render.h"
#include "ECMRenderItem.h"
#include "EC_Model.h"


ECMRenderItem::ECMRenderItem(const char* szECModel, A3DGfxEngine* pGfxEngine)
{
#ifdef _ANGELICA31
	ASSERT(szECModel && szECModel[0] && pGfxEngine);
#elif defined _ANGELICA2
	ASSERT(szECModel && szECModel[0]);
#endif
	m_strECModel = szECModel;
	m_pGfxEngine = pGfxEngine;
	m_pECModel = new CECModel;
#ifdef _ANGELICA31
	m_pECModel->Init(pGfxEngine, NULL);
#endif
}

ECMRenderItem::~ECMRenderItem()
{
	Release();
}

bool ECMRenderItem::Init()
{
#ifdef _ANGELICA31
	if (!m_pECModel->Load(m_strECModel, true, 0, true, true))
		return false;

	if (m_pECModel->GetA3DSkinModel())
	{
		// Set light info
		A3DSkinModelLight li;
		li.colAmbient = m_pGfxEngine->GetA3DDevice()->GetAmbientValue();
		li.dl_vDir = Normalize(A3DVECTOR3(0.0f, -0.7f, -1.0f));
		li.dl_color = A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f);
		//li.dl_colSpecular = A3DCOLORVALUE(.3f, .3f, .3f, 1.0f);
		m_pECModel->GetA3DSkinModel()->SetLightInfo(li);
	}
#elif defined _ANGELICA2
	bool bResult = m_pECModel->Load(m_strECModel);
	if (m_pECModel->GetA3DSkinModel())
	{
		A3DSkinModel::LIGHTINFO LightInfo;
		LightInfo.colAmbient	= CRender::GetInstance()->GetA3DDevice()->GetAmbientColor();
		LightInfo.vLightDir		= Normalize(A3DVECTOR3(0.0f, -0.7f, -1.0f));//Direction of directional light
		LightInfo.colDirDiff	= A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);//Directional light's diffuse color
		LightInfo.colDirSpec	= A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);//	Directional light's specular color
		LightInfo.bPtLight		= false;//	false, disable dynamic point light,
		m_pECModel->GetA3DSkinModel()->SetLightInfo(LightInfo);
	}
#endif

	return true;
}

void ECMRenderItem::Release()
{
	A3DRELEASE(m_pECModel);
}

void ECMRenderItem::Play()
{

}

void ECMRenderItem::Stop()
{
	m_pECModel->StopAllActions();
}

void ECMRenderItem::Tick(DWORD dwTick)
{
	m_pECModel->Tick(dwTick);
}

void ECMRenderItem::Render(A3DEngine* pEngine, A3DViewport* pA3DViewport)
{
	m_pECModel->Render(pA3DViewport);
}

A3DVECTOR3 ECMRenderItem::GetPos() const
{
	if (m_pECModel->GetA3DSkinModel())
		return m_pECModel->GetA3DSkinModel()->GetPos();

	return m_pECModel->GetPos();
}

void ECMRenderItem::GetAABB(A3DAABB* aabb) const
{
	if (!aabb)
		return;

	if (m_pECModel && m_pECModel->GetA3DSkinModel())
	{
		*aabb = m_pECModel->GetA3DSkinModel()->GetModelAABB();
	}
	else
	{
		aabb->Maxs = aabb->Mins = aabb->Extents = aabb->Center = 0.0f;
	}
}

