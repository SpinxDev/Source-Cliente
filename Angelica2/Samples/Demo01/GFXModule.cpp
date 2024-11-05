/*
 * FILE: GFXModule.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/5/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#include "GFXModule.h"
#include "A3DGFXExMan.h"
#include "EC_Model.h"
#include "EC_ModelMan.h"
#include "Render.h"
#include "Game.h"

#include <A3DMacros.h>
#include <A3DViewport.h>
#include <A3DMatrix.h>
#include <A3DFuncs.h>

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

APhysXScene* gGetAPhysXScene(void)
{
	return NULL;
}

/*
A3DDevice* AfxGetA3DDevice()
{
//	return g_pGame->GetA3DDevice();
	return NULL;
}

float AfxGetAverageFrameRate()
{
//	return g_pGame->GetAverageFrameRate();
	return 10.0f;
}

A3DGFXExMan* AfxGetGFXExMan()
{
//	return g_pGame->GetA3DGFXExMan();
	return NULL;
}

CECModelMan* AfxGetECModelMan()
{
//	return g_pGame->GetECModelMan();
	return NULL;
}

A3DSkinModel* AfxLoadA3DSkinModel(const char* szFile, int iSkinFlag)
{
//	return g_pGame->LoadA3DSkinModel(szFile, iSkinFlag);
	return NULL;
}

void AfxReleaseA3DSkinModel(A3DSkinModel* pModel)
{
//	g_pGame->ReleaseA3DSkinModel(pModel);
}

const A3DLIGHTPARAM& AfxGetLightparam()
{
//	return g_pGame->GetDirLight()->GetLightparam();
	A3DLIGHTPARAM lp;
	return lp;
}

float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm)
{
//	float fHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vPos, pNorm);
//	return fHeight;
	return 0.0f;
}

bool AfxRayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel)
{
//	CECCDS* pCDS = g_pGame->GetGameRun()->GetWorld()->GetCDS();
//	RAYTRACERT TraceRt;

//	return pCDS->RayTrace(vStart, vVel, 1.0f, &TraceRt, TRACEOBJ_LBTNCLICK, 0);
	return false;
}

//	A3DVECTOR3 _cam_shake_offset(0);
//	bool _shaking_cam = false;
//	static int _shaking_count = 0;

A3DCamera * AfxGetA3DCamera(void)
{
//	return g_pGame->GetViewport()->GetA3DCamera();
	return NULL;
}

bool AfxGetModelUpdateFlag(void)
{
//	return g_pGame->GetConfigs()->GetModelUpdateFlag();
	return true;
}

void AfxBeginShakeCam()
{
//	_shaking_count++;
//	_shaking_cam = true;
}

void AfxEndShakeCam()
{
//	if (--_shaking_count == 0)
//	{
//		_shaking_cam = false;
//		_cam_shake_offset.Clear();
//	}
}

void AfxSetCamOffset(const A3DVECTOR3& vOffset)
{
//	_cam_shake_offset += vOffset;
}

AM3DSoundBuffer* AfxLoadNonLoopSound(const char* szFile)
{
//	return g_pGame->GetSoundCache()->LoadSound(szFile, enumSoundLoadNonLoop);
	return NULL;
}

AM3DSoundBuffer* AfxLoadLoopSound(const char* szFile)
{
//	return g_pGame->GetSoundCache()->LoadSound(szFile, enumSoundLoadLoop);
	return NULL;
}

void AfxReleaseSoundNonLoop(AM3DSoundBuffer*& pSound)
{
//	g_pGame->GetSoundCache()->ReleaseSound(pSound, enumSoundLoadNonLoop);
}

void AfxReleaseSoundLoop(AM3DSoundBuffer*& pSound)
{
//	g_pGame->GetSoundCache()->ReleaseSound(pSound, enumSoundLoadLoop);
}

void AfxECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel)
{
//	if (g_pGame->GetShadowRender() && !g_pGame->GetConfigs()->GetSystemSettings().bSimpleShadow)
//		g_pGame->GetShadowRender()->AddShadower(aabb.Center, aabb, SHADOW_RECEIVER_TERRAIN, ECModelRenderForShadow, pA3DSkinModel, false);
}

void AfxSetA3DSkinModelSceneLightInfo(A3DSkinModel* pA3DSkinModel)
{
// 	CECSceneLights* pSceneLights = g_pGame->GetGameRun()->GetWorld()->GetSceneLights();
// 	A3DSkinModel::LIGHTINFO LightInfo = pA3DSkinModel->GetLightInfo();
// 	LightInfo.colAmbient	= g_pGame->GetA3DDevice()->GetAmbientValue();
// 	LightInfo.vLightDir		= g_pGame->GetDirLight()->GetLightparam().Direction;
// 	LightInfo.colDirDiff	= g_pGame->GetDirLight()->GetLightparam().Diffuse;
// 
// 	if( pSceneLights )
// 	{
// 		CECSceneLights::SCENELIGHT theLight;
// 		if( pSceneLights->GetSceneLight(pA3DSkinModel->GetAbsoluteTM().GetRow(3), theLight) )
// 		{
// 			LightInfo.bPtLight = true;
// 			LightInfo.colPtDiff = theLight.diffuse;
// 			LightInfo.colPtAmb =  theLight.ambient;
// 			LightInfo.fPtRange = theLight.vRange;
// 			LightInfo.vPtAtten = A3DVECTOR3(theLight.vAttenuation0, theLight.vAttenuation1, theLight.vAttenuation2);
// 			LightInfo.vPtLightPos = theLight.vecPos;
// 		}
// 		else
// 			LightInfo.bPtLight = false;
// 	}
// 
// 	pA3DSkinModel->SetLightInfo(LightInfo);
}

bool AfxPlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, int nCasterID, int nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets)
{
	return true;
}

void AfxSkillGfxShowDamage(int idCaster, int idTarget, int nDamage, int nDivisions, DWORD dwModifier)
{
//	A3DSkillGfxMan::GetDamageShowFunc()(idCaster, idTarget, nDamage, 1, dwModifier);
}

void AfxSkillGfxShowCaster(int idCaster, DWORD dwModifier)
{
//	A3DSkillGfxMan::GetCasterShowFunc()(idCaster, dwModifier);
}

bool AfxSkillGfxAddDamageData(int nCaster, int nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage)
{
// 	using namespace _SGC;
// 	A3DSkillGfxMan* pMan = AfxGetSkillGfxEventMan();
// 	A3DSkillGfxEvent* pEvent = pMan->GetSkillGfxEvent(nCaster, SerialId);
// 
// 	if (pEvent)
// 	{
// 		TARGET_DATA td;
// 		td.dwModifier = dwModifier;
// 		td.idTarget = nTarget;
// 		td.nDamage = nDamage;
// 
// 		if (!pEvent->AddOneTarget(td))
// 			A3DSkillGfxMan::GetDamageShowFunc()(nTarget, nDamage, 1, td.dwModifier,0);
// 
// 		return true;
// 	}
// 
// 	return false;
}

const char* AfxGetECMHullPath()
{
	return "Models\\ECModelHull";
}


bool gGfxGetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount)
{
	return true;
}

AM3DSoundBuffer* AfxLoadNonLoopSound(char const*, int)
{
	return NULL;
}
*/
///////////////////////////////////////////////////////////////////////////
//	
//	Implement CGFXModule
//	
///////////////////////////////////////////////////////////////////////////

CGFXModule::CGFXModule()
{
	m_pA3DGFXExMan	= NULL;
}

CGFXModule::~CGFXModule()
{
}

//	Initialize game
bool CGFXModule::Init()
{
	//	now create gfx manager
//	m_pA3DGFXExMan = new A3DGFXExMan();
	m_pA3DGFXExMan = AfxGetGFXExMan();

	if (!m_pA3DGFXExMan->Init(g_Render.GetA3DDevice()))
	{
		a_LogOutput(1, "CGFXModule::Init, Failed to initialize gfx manager!");
		return false;
	}

	return true;
}

//	Release game
void CGFXModule::Release()
{
	AfxGetGFXExMan()->Release();
}

//	Tick routine
bool CGFXModule::Tick(DWORD dwDeltaTime)
{
	//	Tick GFX Manager
	m_pA3DGFXExMan->Tick(dwDeltaTime);

	//	Tick all Gfx
	ALISTPOSITION pos = m_GFXList.GetHeadPosition();
	while (pos)
	{
		ALISTPOSITION posCur = pos;
		A3DGFXEx* pGfx = m_GFXList.GetNext(pos);

		//	Check whether GFX is out of date
		if (pGfx->GetState() == ST_STOP)
		{
			//	Release this auto GFX item
			pGfx->Release();
			delete pGfx;
			m_GFXList.RemoveAt(posCur);
		}
		else
			pGfx->TickAnimation(dwDeltaTime);
	}

	return true;
}

//	Render routine
bool CGFXModule::Render(A3DViewport* pViewport)
{
	//	Render all auto GFXs
	ALISTPOSITION pos = m_GFXList.GetHeadPosition();
	while (pos)
	{
		A3DGFXEx* pGfx = m_GFXList.GetNext(pos);
		m_pA3DGFXExMan->RegisterGfx(pGfx);
	}

	m_pA3DGFXExMan->RenderAllGfx(pViewport);

	return true;
}


/*	Play a GFX automatically

	szFile: GFX file name
	fScale: 0.0f means using the gfx own factor.
*/
bool CGFXModule::PlayGFX(const char* szFile, const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, 
					const A3DVECTOR3& vUp, float fScale/* 0.0f */)
{
	ASSERT(szFile && szFile[0]);
	ASSERT(m_pA3DGFXExMan);

	A3DGFXEx* pGfx = m_pA3DGFXExMan->LoadGfx(g_Render.GetA3DDevice(), szFile);
	if (!pGfx)
	{
		a_LogOutput(1, "CGFXModule::PlayGFXEx, Failed to load GFX %s", szFile);
		return false;
	}

	if (fScale > 0.0f)
		pGfx->SetScale(fScale);

	A3DMATRIX4 mat = a3d_TransformMatrix(vDir, vUp, vPos);
	pGfx->SetParentTM(mat);
	pGfx->Start(true);

	m_GFXList.AddTail(pGfx);

	return true;
}


