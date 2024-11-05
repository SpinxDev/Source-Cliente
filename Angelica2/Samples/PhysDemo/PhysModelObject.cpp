/*
 * FILE: PhysModelObject.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/5/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "PhysModelObject.h"
#include "Render.h"
#include "Game.h"
#include "InterfaceWrapper.h"
#include "A3DModelPhysics.h"
#include "A3DModelPhysSync.h"
#include "A3DModelPhysSyncData.h"
#include "A3DSkinPhysSync.h"
#include "A3DSkinPhysSyncData.h"

#include "APhysX.h"

#include <A3DMacros.h>
#include <A3DViewport.h>
#include <A3DSkeleton.h>
#include <A3DBone.h>
#include <A3DDevice.h>
#include <A3DEngine.h>
#include <A3DSkinMan.h>

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

//	2-stick model parameters
const char* l_2s_szUpperStick = "upper_stick";
const char* l_2s_szLowerStick = "lower_stick";
const char* l_2s_szJoint = "2s_joint";
float l_2s_fStickLen = 1.0f;
float l_2s_fStickRad = 0.15f;

const char* l_szClothActor = "cloth_actor";
const char* l_szClothActor2 = "cloth_actor2";

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CPhysModelObject
//	
///////////////////////////////////////////////////////////////////////////

CPhysModelObject::CPhysModelObject()
{
	m_pSkinModel	= NULL;
	m_pPhysSyncData	= NULL;
	m_pModelPhys	= NULL;
	m_iType			= TYPE_UNKNOWN;
}

CPhysModelObject::~CPhysModelObject()
{
}

//	Load object data from file
bool CPhysModelObject::Init(int iType)
{
	m_iType = iType;

	if (m_iType == TYPE_2STICKS)
	{
		//	Load skin model
		m_pSkinModel = LoadSkinModel("Models\\2Sticks\\2Sticks.smd");
		if (!m_pSkinModel)
		{
			a_LogOutput(1, "CPhysModelObject::Init2Sticks, Failed to load model");
			return false;
		}
	}

	return true;
}

//	Release object
void CPhysModelObject::Release()
{
	A3DRELEASE(m_pModelPhys);

	if (m_pPhysSyncData)
	{
		delete m_pPhysSyncData;
		m_pPhysSyncData = NULL;
	}

	A3DRELEASE(m_pSkinModel);
}

//	Set position
void CPhysModelObject::SetPos(const A3DVECTOR3& vPos)
{
	if (m_pSkinModel)
		m_pSkinModel->SetPos(vPos);
}

//	Set orientation
void CPhysModelObject::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (m_pSkinModel)
		m_pSkinModel->SetDirAndUp(vDir, vUp);
}

//	Setup object
bool CPhysModelObject::Setup(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	m_pModelPhys = new A3DModelPhysics;

	if (!m_pModelPhys->Init(g_Game.GetPhysXScene(), m_pSkinModel))
		return false;

	if (m_iType == TYPE_2STICKS)
	{
		if (!Setup2Sticks(vPos + g_vAxisY * 2.0f))
			return false;
	}

	return true;
}

//	Load skin model from file
CSkinModel* CPhysModelObject::LoadSkinModel(const char* szFile)
{
	CSkinModel* pModel = new CSkinModel;
	if (!pModel)
		return NULL;

	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		a_LogOutput(1, "CPhysModelObject::LoadSkinModel, failed to initialize skin model");
		return NULL;
	}

	if (!pModel->Load(szFile, 0))
	{
		delete pModel;
		a_LogOutput(1, "CPhysModelObject::LoadSkinModel, Failed to load %s", szFile);
		return NULL;
	}

	//	Set light for model
	A3DSkinModel::LIGHTINFO LightInfo;
	memset(&LightInfo, 0, sizeof (LightInfo));
	const A3DLIGHTPARAM& lp = g_Render.GetDirectionalLight()->GetLightparam();

	LightInfo.colAmbient	= g_Render.GetA3DDevice()->GetAmbientValue();
	LightInfo.vLightDir		= lp.Direction;
	LightInfo.colDirDiff	= lp.Diffuse;
	LightInfo.colDirSpec	= lp.Specular;
	LightInfo.bPtLight		= false;

	pModel->SetLightInfo(LightInfo);

	return pModel;
}

//	Tick routine
bool CPhysModelObject::Tick(DWORD dwDeltaTime)
{
	if (m_pModelPhys)
		m_pModelPhys->UpdateBeforePhysXSim(dwDeltaTime);
	else
		m_pSkinModel->Update(dwDeltaTime);

	return true; 
}

//	Sync physical data to graphics data
bool CPhysModelObject::Sync()
{
	if (m_pModelPhys)
		m_pModelPhys->SyncAfterPhysXSim();

	return true;
}

//	Render routine
bool CPhysModelObject::Render(A3DViewport* pViewport)
{
	if (m_pSkinModel)
		m_pSkinModel->Render(pViewport);

	return true;
}

//	Render proxy
bool CPhysModelObject::RenderProxy(A3DViewport* pViewport)
{
	if (m_pSkinModel)
		m_pSkinModel->Render(pViewport);

	return true;
}

//	Change physical state
bool CPhysModelObject::ChangePhysicalState(int iState)
{
	if (!m_pSkinModel || !m_pModelPhys->GetModelSync())
		return false;

	m_iPhysState = iState;

	m_pModelPhys->ChangePhysState(iState);

	if (iState == PHY_STATE_ANIMATION)
	{
		if (m_iType == TYPE_2STICKS)
			m_pSkinModel->PlayActionByNameDC("2sticks_rotate", -1, 0);
	}
	else if (iState == PHY_STATE_PARTSIMULATE)
	{
		if (m_iType == TYPE_2STICKS)
			m_pSkinModel->PlayActionByNameDC("2sticks_rotate", -1, 0);
	}

	return true;
}

//	Initialize 2-stick object
bool CPhysModelObject::Setup2Sticks(const A3DVECTOR3& vPos)
{
	//	Setup skin model position
	m_pSkinModel->SetPos(A3DVECTOR3(0.0f));
	m_pSkinModel->Update(0);

	m_pPhysSyncData = new A3DModelPhysSyncData;

	//	Prepare physical object data ...
	//	Create upper stick
	A3DModelPhysSyncData::ACTOR_DATA* pUpperActorData = m_pPhysSyncData->CreateActor(l_2s_szUpperStick, A3DVECTOR3(0.0f, l_2s_fStickLen * 1.5f, 0.0f));
	APhysXActorDesc& apxActorDesc = *pUpperActorData->GetActorDesc();
	apxActorDesc.mDensity = 1.0f;
	apxActorDesc.mHasBody = true;

	APhysXMaterialDesc& apxMaterialDesc = *pUpperActorData->GetMaterialDesc();
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	//	Add actor shape
	APhysXCapsuleShapeDesc CapsuleDesc;
	CapsuleDesc.mRadius = l_2s_fStickRad;
	CapsuleDesc.mHeight = l_2s_fStickLen;
	CapsuleDesc.UseDefaultMaterial();
	
	pUpperActorData->AddShape(&CapsuleDesc);

	//	Create lower stick
	A3DModelPhysSyncData::ACTOR_DATA* pLowerActorData = m_pPhysSyncData->CreateActor(l_2s_szLowerStick, A3DVECTOR3(0.0f, l_2s_fStickLen * 0.5f, 0.0f));
	apxActorDesc.mDensity = 1.0f;
	apxActorDesc.mHasBody = true;

	apxMaterialDesc = *pLowerActorData->GetMaterialDesc();
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	pLowerActorData->AddShape(&CapsuleDesc);

	//	Link actors and bone
	m_pPhysSyncData->LinkBoneToActor(l_2s_szLowerStick, m_pSkinModel, "Bone01");
	m_pPhysSyncData->LinkBoneToActor(l_2s_szUpperStick, m_pSkinModel, "Bone02");
	m_pPhysSyncData->LinkBoneToActor(l_2s_szUpperStick, m_pSkinModel, "Bone03");
	m_pPhysSyncData->LinkActorToBone(l_2s_szLowerStick, m_pSkinModel, "Bone01");
	m_pPhysSyncData->LinkActorToBone(l_2s_szUpperStick, m_pSkinModel, "Bone02");
	m_pPhysSyncData->SetRootBone("Bone01");

	pLowerActorData->GetLinkInfo().GetBoneDataByName("Bone01")->SetAnimMajorFlag(true);
	pUpperActorData->GetLinkInfo().GetBoneDataByName("Bone02")->SetAnimMajorFlag(false);
	pUpperActorData->GetLinkInfo().GetBoneDataByName("Bone03")->SetAnimMajorFlag(false);

	//	Create joint
	APhysXSphericalJointDesc JointDesc;
	JointDesc.mGlobalAnchor = NxVec3(0.0f, l_2s_fStickLen, 0.0f);
	JointDesc.mGlobalAxis = NxVec3(0.0f, 1.0f, 0.0f);
	A3DModelPhysSyncData::JOINT_DATA* pJointData = m_pPhysSyncData->CreateJoint(l_2s_szJoint, &JointDesc);

	//	Link joint and actors
	m_pPhysSyncData->LinkActorAndJoint(l_2s_szUpperStick, l_2s_szJoint, 0);
	m_pPhysSyncData->LinkActorAndJoint(l_2s_szLowerStick, l_2s_szJoint, 1);

	//	Temp code
//	if (!m_pPhysSyncData->Load("Models\\2Sticks\\2Sticks.mphy"))
//		return false;

	//	Temp code
//	m_pPhysSyncData->Save("F:\\dyx\\VC++\\Angelica2\\Samples\\PhysDemo\\Bin\\Models\\2Sticks\\2Sticks.mphy");

	if (!m_pModelPhys->CreateModelSync(m_pPhysSyncData))
		return false;

	//	Set current physical state
	ChangePhysicalState(g_Game.GetPhysState());

	//	Restore skin model position
	m_pSkinModel->SetPos(vPos);
	m_pSkinModel->Update(0);
	m_pModelPhys->GetModelSync()->Teleport();

	//	Add cloth skins
	if (!AddClothSkin())
		return false;

	return true;
}

//	Add cloth skin
bool CPhysModelObject::AddClothSkin()
{
	//	Use cloth 1
	if (0)
	{
		//	Add cloth
		int iSkinSlot = m_pModelPhys->AddSkinFile("Models\\2Sticks\\cloth.ski");

		A3DSkinPhysSyncData* pSyncData = m_pModelPhys->BeginEditClothSkin(iSkinSlot);
		if (!pSyncData)
			return false;

		//	Create a sphere actor at origin
		A3DSkinPhysSyncData::ACTOR_DATA* pClothActor = pSyncData->GetActorByName(l_szClothActor);
		if (!pClothActor)
		{
			pClothActor = pSyncData->CreateActor(l_szClothActor, g_vOrigin);

			APhysXActorDesc* apxActorDesc = pClothActor->GetActorDesc();
			apxActorDesc->mDensity = 1.0f;
			apxActorDesc->mHasBody = true;

			APhysXMaterialDesc* apxMaterialDesc = pClothActor->GetMaterialDesc();
			apxMaterialDesc->mDynamicFriction = 0.1f;
			apxMaterialDesc->mRestitution = 1.0f;

			//	Add actor shape
			APhysXSphereShapeDesc SphereDesc;
			SphereDesc.mRadius = 0.1f;
			SphereDesc.UseDefaultMaterial();

			pClothActor->AddShape(&SphereDesc);
		}

		//	Bind skin physical data
		if (!pSyncData->InitBind(g_Game.GetPhysXScene()))
		{
			m_pModelPhys->EndEditClothSkin(pSyncData);
			return false;
		}

		//	Save data to file
		AString strFile = g_szWorkDir;
		strFile += "\\";
		strFile += pSyncData->GetFileName();
		if (!pSyncData->Save(strFile))
		{
			m_pModelPhys->EndEditClothSkin(pSyncData);
			return false;
		}

		m_pModelPhys->EndEditClothSkin(pSyncData);
		pSyncData = NULL;

		//	Link cloth
		if (!m_pModelPhys->OpenClothSkin_SL(iSkinSlot, l_szClothActor, "HH_cloth", false))
	//	if (!m_pModelPhys->OpenClothSkin_HL(iSkinSlot, l_szClothActor, "HH_cloth", false))
			return false;
	}

	//	Use cloth 2
	if (1)
	{
		//	Add skin
		int iSkinSlot = m_pModelPhys->AddSkinFile("Models\\2Sticks\\cloth2.ski");

		A3DSkinPhysSyncData* pSyncData = m_pModelPhys->BeginEditClothSkin(iSkinSlot);
		if (!pSyncData)
			return false;

		//	Create a sphere actor at origin
		A3DSkinPhysSyncData::ACTOR_DATA* pClothActor = pSyncData->GetActorByName(l_szClothActor);
		if (!pClothActor)
		{
			pClothActor = pSyncData->CreateActor(l_szClothActor, g_vOrigin);

			APhysXActorDesc* apxActorDesc = pClothActor->GetActorDesc();
			apxActorDesc->mDensity = 1.0f;
			apxActorDesc->mHasBody = true;

			APhysXMaterialDesc* apxMaterialDesc = pClothActor->GetMaterialDesc();
			apxMaterialDesc->mDynamicFriction = 0.1f;
			apxMaterialDesc->mRestitution = 1.0f;

			//	Add actor shape
			APhysXSphereShapeDesc SphereDesc;
			SphereDesc.mRadius = 0.1f;
			SphereDesc.UseDefaultMaterial();

			pClothActor->AddShape(&SphereDesc);

			//	Create second sphere actor
			A3DSkinPhysSyncData::ACTOR_DATA* pClothActor2 = pSyncData->CreateActor(l_szClothActor2, A3DVECTOR3(2.0f, 0.0f, 0.0f));
			apxActorDesc = pClothActor2->GetActorDesc();
			apxActorDesc->mDensity = 1.0f;
			apxActorDesc->mHasBody = true;

			apxMaterialDesc = pClothActor2->GetMaterialDesc();
			apxMaterialDesc->mDynamicFriction = 0.1f;
			apxMaterialDesc->mRestitution = 1.0f;

			pClothActor2->AddShape(&SphereDesc);
		}

		//	Bind skin physical data
		if (!pSyncData->InitBind(g_Game.GetPhysXScene()))
		{
			m_pModelPhys->EndEditClothSkin(pSyncData);
			return false;
		}

		//	Save data to file
		AString strFile = g_szWorkDir;
		strFile += "\\";
		strFile += pSyncData->GetFileName();
		if (!pSyncData->Save(strFile))
		{
			m_pModelPhys->EndEditClothSkin(pSyncData);
			return false;
		}

		m_pModelPhys->EndEditClothSkin(pSyncData);
		pSyncData = NULL;

		//	Link cloth
		A3DModelPhysics::CLOTH_LINK aLinks[2];

		aLinks[0].szRBActor	= l_szClothActor;
		aLinks[0].szHookHH	= "HH_cloth";
		aLinks[0].bHHIsBone	= false;

		aLinks[1].szRBActor	= l_szClothActor2;
		aLinks[1].szHookHH	= "HH_cloth2";
		aLinks[1].bHHIsBone	= false;

	//	if (!m_pModelPhys->OpenClothSkin_SL(iSkinSlot, 2, aLinks))
		if (!m_pModelPhys->OpenClothSkin_HL(iSkinSlot, 2, aLinks))
			return false;
	}

	return true;
}


