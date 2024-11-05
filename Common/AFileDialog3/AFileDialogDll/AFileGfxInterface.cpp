/*
* FILE: AFileGfxInterface.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/08/26
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/


#include "StdAfx.h"

#ifdef _ANGELICA31

#include "AFileGfxInterface.h"
#include "AFileSkillGfxEventMan.h"
#include "A3DSkillGfxComposer.h"
#include "AFilePreviewWnd.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AFileGfxInterface
//	
///////////////////////////////////////////////////////////////////////////

AFileGfxInterface::AFileGfxInterface(void)
{
	m_pA3DSkillGfxEventMan = NULL;
	m_pA3DSkillGfxComposerMan = NULL;
	m_pA3DGfxEngine = NULL;
	m_bIsInSkillAttackEditor = false;
}

AFileGfxInterface::~AFileGfxInterface(void)
{
}

bool AFileGfxInterface::Init(A3DGfxEngine* pA3DGfxEngine)
{
	if (!pA3DGfxEngine)
		return false;

	m_pA3DGfxEngine = pA3DGfxEngine;

	m_pA3DSkillGfxEventMan = new AFileSkillGfxEventMan();
	if (!m_pA3DSkillGfxEventMan) 
	{
		a_LogOutput(1, "AFileGfxInterface::Init(A3DGfxEngine* pA3DGfxEngine), not enough memory!");
		return false;
	}

	if (!m_pA3DSkillGfxEventMan->Init(m_pA3DGfxEngine->GetA3DGfxExMan()))
	{
		a_LogOutput(1, "AFileGfxInterface::Init(A3DGfxEngine* pA3DGfxEngine), failed to initialize A3DSkillGfxEventMan!");
		return false;
	}

	m_pA3DSkillGfxComposerMan = new A3DSkillGfxComposerMan();
	if (!m_pA3DSkillGfxComposerMan)
	{
		a_LogOutput(1, "AFileGfxInterface::Init(A3DGfxEngine* pA3DGfxEngine), not enough memory!");
		return false;
	}

	if (!m_pA3DSkillGfxComposerMan->Init(pA3DGfxEngine))
	{
		a_LogOutput(1, "AFileGfxInterface::Init(A3DGfxEngine* pA3DGfxEngine), failed to initialize A3DSkillGfxComposerMan!");
		return false;
	}

	return true;
}

void AFileGfxInterface::Release()
{
	A3DRELEASE(m_pA3DSkillGfxComposerMan);

	delete m_pA3DSkillGfxEventMan;
	m_pA3DSkillGfxEventMan = NULL;
}

//	For Skill Gfx Event
A3DSkillGfxEventMan* AFileGfxInterface::GetSkillGfxEventMan() const
{
	return m_pA3DSkillGfxEventMan;
}

A3DSkillGfxComposerMan* AFileGfxInterface::GetSkillGfxComposerMan() const
{
	return m_pA3DSkillGfxComposerMan;
}

typedef void* (*TypeNewOp)(size_t);
typedef void (*TypeDeleteOp)(void*);

//	For SkinModel
A3DSkinModel* AFileGfxInterface::LoadA3DSkinModel(const char* szFile, int iSkinFlag) const
{
	A3DSkinModel* pSkinModel = new A3DSkinModel;

	if (!pSkinModel->Init(m_pA3DGfxEngine->GetA3DEngine()) || !pSkinModel->Load(szFile, iSkinFlag))
	{
		A3DRELEASE(pSkinModel);
		return NULL;
	}

	return pSkinModel;
}

void AFileGfxInterface::ReleaseA3DSkinModel(A3DSkinModel* pModel) const
{
	pModel->Release();
	delete pModel;
}


bool AFileGfxInterface::GetModelUpdateFlag() const
{
	return true;
}

void AFileGfxInterface::SetA3DSkinModelSceneLightInfo(A3DSkinModel* pA3DSkinModel) const
{
	A3DSkinModelLight LightInfo = pA3DSkinModel->GetLightInfo();
	GfxLightParamList& ls = m_pA3DGfxEngine->GetA3DGfxExMan()->GetLightParamList();

	if (ls.size())
	{
		A3DLIGHTPARAM* param = &ls[0]->GetLightParam();
		LightInfo.pl_bEnable = true;
		LightInfo.pl_vPos = param->Position;
		LightInfo.pl_color = param->Diffuse;
		///LightInfo.pl_colSpecular =  param->Specular * param->Specular.a * 2.0f;
		LightInfo.pl_vAtten = A3DVECTOR3(param->Attenuation0, param->Attenuation1, param->Attenuation2);
	}
	else
		LightInfo.pl_bEnable = false;

	pA3DSkinModel->SetLightInfo(LightInfo);
}

void AFileGfxInterface::ECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel) const
{

}

float AFileGfxInterface::GetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm) const
{
	if (pNorm) *pNorm = A3DVECTOR3::vAxisY;
	return 0;
}

const A3DLIGHTPARAM& AFileGfxInterface::GetLightparam() const
{
	static const A3DCOLORVALUE diffuse(0.8f, 0.8f, 0.9f, 1.0f);
	static const A3DCOLORVALUE spec(1.0f, 1.0f, 1.0f, 1.0f);
	static const A3DCOLORVALUE ambient(0.0f, 0.0f, 0.0f, 1.0f);

	static A3DLIGHTPARAM p;
	memset(&p, 0, sizeof(p));
	p.Type		= A3DLIGHT_DIRECTIONAL;
	p.Diffuse	= diffuse;
	p.Specular	= spec;
	p.Ambient	= ambient;
	p.Position	= A3DVECTOR3(0.0f, 0.0f, 0.0f);
	p.Direction	= Normalize(A3DVECTOR3(0.0f, -0.7f, 1.0f));
	return p;
}

bool AFileGfxInterface::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel) const
{
	return false;
}

//	For Sound
AM3DSoundBuffer* AFileGfxInterface::LoadNonLoopSound(const char* szFile, int nSfxPriority) const
{
	AMSoundBufferMan* pMan = m_pA3DGfxEngine->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
	return pMan->Load3DSound(szFile, true);
}

AM3DSoundBuffer* AFileGfxInterface::LoadLoopSound(const char* szFile) const
{
	return LoadNonLoopSound(szFile, 0);
}

void AFileGfxInterface::ReleaseSoundNonLoop(AM3DSoundBuffer*& pSound) const
{
	m_pA3DGfxEngine->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&pSound);
}

void AFileGfxInterface::ReleaseSoundLoop(AM3DSoundBuffer*& pSound) const
{
	ReleaseSoundNonLoop(pSound);
}


AudioEngine::EventInstance* AFileGfxInterface::CreateAudioEventInstance(const char* szEventFullPath) const
{
	return NULL;
}

void AFileGfxInterface::ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const
{

}

A3DCamera* AFileGfxInterface::GetA3DCamera() const
{
	if (GetPreviewWnd())
		return GetPreviewWnd()->GetA3DCamera();
	else
		return NULL;
}

//	For others
float AFileGfxInterface::GetAverageFrameRate() const
{
	return 30.f;
}

bool AFileGfxInterface::GetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount) const
{
	pVerts[0] = A3DVECTOR3(vCenter.x - fRadus, vCenter.y, vCenter.z + fRadus);
	pVerts[1] = A3DVECTOR3(vCenter.x + fRadus, vCenter.y, vCenter.z + fRadus);
	pVerts[2] = A3DVECTOR3(vCenter.x - fRadus, vCenter.y, vCenter.z - fRadus);
	pVerts[3] = A3DVECTOR3(vCenter.x + fRadus, vCenter.y, vCenter.z - fRadus);
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 1;
	pIndices[4] = 3;
	pIndices[5] = 2;
	nVertCount = 4;
	nIndexCount = 6;
	return true;
}


bool AFileGfxInterface::PlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets) const
{
	if (!m_bIsInSkillAttackEditor)
		return true;

	A3DSkillGfxComposerMan* pMan = GetSkillGfxComposerMan();
	A3DSkillGfxEventMan* pEventMan = GetSkillGfxEventMan();

	pMan->Play(
		strAtkFile,
		SerialID,
		nCasterID,
		nCastTargetID,
		pFixedPoint);

	A3DSkillGfxEvent* pEvent = pEventMan->GetSkillGfxEvent(nCasterID, SerialID);

	if (!pEvent)
		return true;

	pEvent = pEvent->GetLast();
	pEvent->SetDivisions(nDivisions);

	TARGET_DATA td;
	td.idTarget = 2;
	td.nDamage = 100;
	td.dwModifier = 0;
	pEvent->AddOneTarget(td);
	return true;
}

void AFileGfxInterface::SkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier, const ECMODEL_SHAKE* shake) const
{
	if (!m_bIsInSkillAttackEditor)
		return;

	if (ISkillGfxEventCallback* pCallback = GetSkillGfxEventMan()->GetSkillGfxEventCallback())
		pCallback->ShowDamageValue(idCaster, idTarget, nDamage, 1, dwModifier, shake);
}

void AFileGfxInterface::SkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier) const
{

}

bool AFileGfxInterface::SkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage, const ECMODEL_SHAKE* shake) const
{
	if (!m_bIsInSkillAttackEditor)
		return true;

	A3DSkillGfxEventMan* pMan = GetSkillGfxEventMan();
	A3DSkillGfxEvent* pEvent = pMan->GetSkillGfxEvent(nCaster, SerialId);

	if (pEvent)
	{
		TARGET_DATA td;
		td.dwModifier = dwModifier;
		td.idTarget = nTarget;
		td.nDamage = nDamage;

		if (!pEvent->AddOneTarget(td))
		{
			if (ISkillGfxEventCallback* pCallback = pMan->GetSkillGfxEventCallback())
				pCallback->ShowDamageValue(nCaster, nTarget, nDamage, 1, td.dwModifier, shake);
		}

		return true;
	}

	return false;
}


#endif