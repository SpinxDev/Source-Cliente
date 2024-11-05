/*
 * FILE: A3DModelPhysics.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#ifdef A3D_PHYSX

#include "A3DModelPhysics.h"
#include "A3DSkinPhysSync.h"
#include "A3DSkinPhysSyncData.h"
#include "A3DModelPhysSync.h"
#include "A3DModelPhysSyncData.h"
#include "A3DPhysRBCreator.h"

#include "APhysX.h"

#include <ALog.h>
#include <A3DSkinModel.h>
#include <A3DSkin.h>
//	#include <A3DSkeleton.h>
//	#include <A3DBone.h>
//	#include <A3DClothMesh.h>

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

//	Hard-link connect info
struct HL_CONNECT
{
	NxActor*				pHHActor;
	APhysXRigidBodyObject*	pRBActor;
	A3DMATRIX4				mat;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysics
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysics::A3DModelPhysics()
{
	m_pECModel			= NULL;
	m_pPhysScene		= NULL;
	m_pSkinModel		= NULL;
	m_pModelSync		= NULL;
	m_pModelSyncData	= NULL;
	// m_bCollisionCh		= true;
	
	//-----------------------------------------------------------------
	// the default channel is set to APX_COLLISION_CHANNEL_COMMON...
	m_idCollisionCh			=	APX_COLLISION_CHANNEL_COMMON;		
	m_uiMajorRBCollisionCh	=	APX_COLLISION_CHANNEL_COMMON;		
}

A3DModelPhysics::~A3DModelPhysics()
{
}

//	Initialize object
bool A3DModelPhysics::Init(APhysXScene* pPhysScene, A3DSkinModel* pSkinModel)
{
	if (m_pSkinModel)
	{
		//	Bind twice !
		ASSERT(!m_pSkinModel);
		return false;
	}

	m_pPhysScene = pPhysScene;
	m_pSkinModel = pSkinModel;

	//	Initialize sync skin slots
	int iNumSkin = m_pSkinModel->GetSkinNum();
	for (int i=0; i < iNumSkin; i++)
	{
		SKIN_SLOT* pSlot = new SKIN_SLOT;
		m_aSkinSlots.Add(pSlot);
	}

	return true;
}

bool A3DModelPhysics::Init(APhysXScene* pPhysScene, CECModel* pECModel)
{
	if (m_pECModel)
	{
		//	Bind twice !
		ASSERT(!m_pECModel);
		return false;
	}

	m_pECModel = pECModel;
	return Init(pPhysScene, pECModel->GetA3DSkinModel());
}

//	Release object
void A3DModelPhysics::Release()
{
	//	Before release, change the simulate state back to animation
	ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);

	//	Release all skin slots
	ReleaseAllSkinPhys();

	if (m_pModelSync)
	{
		delete m_pModelSync;
		m_pModelSync = NULL;
	}

	m_pModelSyncData = NULL;
}

//	Release all skin slots
void A3DModelPhysics::ReleaseAllSkinPhys()
{
	for (int i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		ReleaseSkinPhys(i);
		SKIN_SLOT* pSlot = m_aSkinSlots[i];
		delete pSlot;
	}

	m_aSkinSlots.RemoveAll();
}

//	Create model sync
bool A3DModelPhysics::CreateModelSync(A3DModelPhysSyncData* pSyncData)
{
	if (!pSyncData || !m_pPhysScene || !m_pSkinModel)
		return false;

	if (m_pModelSync)
	{
		int i;

		//	Ensure all clothes have been closed
		for (i=0; i < m_aSkinSlots.GetSize(); i++)
		{
			A3DSkinPhysSync* pSkinPhys = m_aSkinSlots[i]->pPhysSync;
			if (pSkinPhys && pSkinPhys->IsSkinBound())
			{
				ASSERT(0);
				return false;
			}
		}
		
		delete m_pModelSync;
		m_pModelSync = NULL;
	}

	if (!pSyncData->InitBind(m_pSkinModel))
		return false;

	m_pModelSyncData = pSyncData;

	m_pModelSync = new A3DModelPhysSync(this);

	if (!m_pModelSync->Bind(m_pSkinModel, pSyncData, m_pPhysScene))
	{
		delete m_pModelSync;
		m_pModelSync = NULL;
		return false;
	}

	m_uiMajorRBCollisionCh = m_pModelSync->GetSkeletonRBObject()->GetCollisionChannel();


/*
	if (m_bCollisionCh)
	{
		SetCollisionChannel(m_pPhysScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel());
	}
*/


	return true;
}

//	Add a new skin
bool A3DModelPhysics::AddSkinFile(const char* szSkinFile)
{
	if (!m_pSkinModel)
		return false;

	int iSlot = m_pSkinModel->AddSkinFile(szSkinFile, true);
	if (iSlot < 0)
		return false;

	SKIN_SLOT* pSlot = new SKIN_SLOT;
	int iSlot2 = m_aSkinSlots.Add(pSlot);

	if (iSlot != iSlot2)
	{
		//	This shouldn't happen
		ASSERT(iSlot == iSlot2);
		return false;
	}

	return true;
}

//	Add a skin
bool A3DModelPhysics::AddSkin(A3DSkin* pSkin, bool bAutoRem/* true */)
{
	if (!m_pSkinModel)
		return false;

	int iSlot = m_pSkinModel->AddSkin(pSkin, bAutoRem);
	if (iSlot < 0)
		return false;

	SKIN_SLOT* pSlot = new SKIN_SLOT;
	int iSlot2 = m_aSkinSlots.Add(pSlot);

	if (iSlot != iSlot2)
	{
		//	This shouldn't happen
		ASSERT(iSlot == iSlot2);
		return false;
	}

	return true;
}

//	Replace a skin
bool A3DModelPhysics::ReplaceSkinFile(int iSlot, const char* szSkinFile, bool bAutoRem/* true */)
{
	if (!m_pSkinModel)
		return false;

	if (!m_pSkinModel->ReplaceSkinFile(iSlot, szSkinFile, bAutoRem))
		return false;

	ReleaseSkinPhys(iSlot);

	return true;
}

//	Replace a skin
bool A3DModelPhysics::ReplaceSkin(int iSlot, A3DSkin* pSkin, bool bAutoRem/* true */)
{
	if (!m_pSkinModel)
		return false;

	if (!m_pSkinModel->ReplaceSkin(iSlot, pSkin, bAutoRem))
		return false;

	ReleaseSkinPhys(iSlot);

	return true;
}

//	Remove a skin item, this operation release both skin and skin item
void A3DModelPhysics::RemoveSkinItem(int iSlot)
{
	if (!m_pSkinModel)
		return;

	m_pSkinModel->RemoveSkinItem(iSlot);
	ReleaseSkinPhys(iSlot, true);
}

//	Release all skins
void A3DModelPhysics::ReleaseAllSkins()
{
	if (m_pSkinModel)
		m_pSkinModel->ReleaseAllSkins();

	ReleaseAllSkinPhys();
}

//	Bind physical sync for specified skin
bool A3DModelPhysics::BindSkinPhys(int iSlot)
{
	if (!m_pPhysScene || !m_pSkinModel)
		return false;

	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);
	if (!pSkin)
		return false;

	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	if (!pSlot->pPhysSync)
	{
		ASSERT(pSlot->pPhysSync);
		return false;
	}

	//	Calculate approximate position for cloth skin
	A3DVECTOR3 vRefPos;

	//-------------------------------------------------------------------
	// revised by wenfeng, Dec. 23, 2008
	// since we will ignore the vRefPos passed into Bind(...), here we 
	// just use the simple value...

	/*
	if (m_pModelSyncData && m_pModelSyncData->GetBindInitFlag())
	{
		A3DBone* pRootBone = m_pSkinModel->GetSkeleton()->GetBone(m_pModelSyncData->m_iRootBone);
		vRefPos = pRootBone->GetAbsoluteTM().GetRow(3);
	}
	else
	*/

		vRefPos = m_pSkinModel->GetPos();
	//-------------------------------------------------------------------

	//	Do bind
	if (!pSlot->pPhysSync->Bind(m_pPhysScene, m_pSkinModel, pSkin, vRefPos))
		return false;

	return true;
}

//	Release physical sync for specified skin
void A3DModelPhysics::ReleaseSkinPhys(int iSlot, bool bRemSlot/* false */)
{
	if (!m_pPhysScene)
		return;

	if (iSlot < 0 || iSlot >= m_aSkinSlots.GetSize())
	{
		ASSERT(0);
		return;
	}

	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	if (pSlot->pPhysSync)
	{
		delete pSlot->pPhysSync;
		pSlot->pPhysSync = NULL;
	}

	//	Release all hard-link connectors
	for (int i=0; i < pSlot->m_aHLCons.GetSize(); i++)
	{
		if (pSlot->m_aHLCons[i])
			m_pPhysScene->ReleasePhysXObjectConnnector(pSlot->m_aHLCons[i]);
	}

	pSlot->m_aHLCons.RemoveAll(false);

	if (bRemSlot)
	{
		delete pSlot;
		m_aSkinSlots.RemoveAt(iSlot);
	}
}

//	Change specified skin to cloth.
//	iLinkType: 0 - default type; 1: soft link; 2: hard link
bool A3DModelPhysics::OpenClothSkin(int iSlot, int iLinkType/* 0 */)
{
	if (!OpenClothSkin_Inner(iSlot))
		return false;

	if (!iLinkType)
	{
		SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
		if (pSlot->pPhysSync->GetSyncData()->GetSoftLinkFlag())
			iLinkType = 1;
		else
			iLinkType = 2;
	}

	bool bSuccess = true;

	if (iLinkType == 1)		//	Soft link
	{
		bSuccess = OpenClothSkin_SL(iSlot);
	}
	else	//	Hard link
	{
		bSuccess = OpenClothSkin_HL(iSlot);
	}

	if (!bSuccess)
	{
		A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);
		pSkin->EnableClothes(false);
		ReleaseSkinPhys(iSlot);
	}

	return bSuccess;
}

//	Change specified skin to cloth. Soft-Link version
bool A3DModelPhysics::OpenClothSkin_Inner(int iSlot)
{
	if (!m_pPhysScene || !m_pSkinModel)
		return false;

	//	Change skin to cloth
	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);
	if (!pSkin)
		return false;

	if (iSlot < 0 || iSlot >= m_aSkinSlots.GetSize())
	{
		ASSERT(0);
		return false;
	}

	//	Check if cloth physical data exists
	AString strSyncFile = pSkin->GetFileName();
	af_ChangeFileExt(strSyncFile, ".sphy");
	if (!af_IsFileExist(strSyncFile))
		return false;

	//	Create skin physical sync if it doesn't exist
	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	bool bNewPhysSync = false;

	if (!pSlot->pPhysSync)
	{
		pSlot->pPhysSync = new A3DSkinPhysSync;
		bNewPhysSync = true;
	}

	//	Bind physical data
	if (!BindSkinPhys(iSlot))
	{
		if (bNewPhysSync)
		{
			delete pSlot->pPhysSync;
			pSlot->pPhysSync = NULL;
		}

		return false;
	}

	//	Set collision channel
	pSlot->pPhysSync->GetClothAttacher()->SetCollisionChannel(m_idCollisionCh);
	
	//	Get source mesh info
	A3DSkinPhysSyncData* pSyncData = pSlot->pPhysSync->GetSyncData();
	int i, aRigids[256], aSkins[256];
	int iNumRigidMesh = pSyncData->m_aRigidMeshInfo.GetSize();
	int iNumSkinMesh = pSyncData->m_aSkinMeshInfo.GetSize();
	ASSERT(iNumRigidMesh <= 256);
	ASSERT(iNumSkinMesh <= 256);
	
	for (i=0; i < iNumRigidMesh; i++)
		aRigids[i] = pSyncData->m_aRigidMeshInfo[i]->iIndex;

	for (i=0; i < iNumSkinMesh; i++)
		aSkins[i] = pSyncData->m_aSkinMeshInfo[i]->iIndex;

	//	Change skin to cloth
	if (!pSkin->GenerateClothes(iNumRigidMesh, aRigids, iNumSkinMesh, aSkins))
		return false;
		
	pSkin->EnableClothes(true);

	return true;
}

//	Open physical property for specified skin. Soft-Link version
bool A3DModelPhysics::OpenClothSkin_SL(int iSlot)
{
	//	Make soft-link flag
	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	pSlot->bSoftLink = true;
	pSlot->pPhysSync->GetClothAttacher()->EnableAttacherKinematic(true);

	return true;
}

//	Change specified skin to cloth. Hard-Link version
bool A3DModelPhysics::OpenClothSkin_HL(int iSlot)
{
	if (!m_pModelSync)
		return false;

	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);

	//	Make hard-link flag
	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	pSlot->bSoftLink = false;
	A3DSkinPhysSync* pSkinPhys = pSlot->pPhysSync;
	pSkinPhys->GetClothAttacher()->EnableAttacherKinematic(false);

	int i, iNumLink = pSkinPhys->m_aRBActorLinks.GetSize();
	AArray<HL_CONNECT> aConnects;
	aConnects.SetSize(iNumLink, 4);

	//	Record connect info.
	for (i=0; i < iNumLink; i++)
	{
		const A3DSkinPhysSync::RBACTOR_LINK& link = pSkinPhys->m_aRBActorLinks[i];
		HL_CONNECT& connect = aConnects[i];

		//	Get actor that hh-type hook is bound with
		connect.pHHActor = m_pModelSync->GetActorByHook(link.iHookHHIdx, link.bHHIsBone, connect.mat);
		if (!connect.pHHActor)
			return false;

		//	Get actor that drags cloth
		connect.pRBActor = pSkinPhys->GetRBActor(i);
		if (!connect.pRBActor)
			return false;
	}

	NxMat34 matNx;

	//	Create connectors
	for (i=0; i < aConnects.GetSize(); i++)
	{
		const HL_CONNECT& connect = aConnects[i];

		APhysXConverter::A2N_Matrix44(connect.mat, matNx);
		connect.pRBActor->SetPose(matNx);

		APhysXFixedJointDesc apxFixedJointDesc;
		apxFixedJointDesc.mGlobalAnchor.set(matNx.t);
		apxFixedJointDesc.mPhysXObject1 = m_pModelSync->GetSkeletonRBObject();
		apxFixedJointDesc.mNxActor1 = connect.pHHActor;
		apxFixedJointDesc.mPhysXObject2 = connect.pRBActor;
		apxFixedJointDesc.mNxActor2 = connect.pRBActor->GetNxActor(0);

		APhysXObjectConnectorInstanceDesc apxConnectorInstanceDesc;
		apxConnectorInstanceDesc.mPhysXConnectorDesc = &apxFixedJointDesc;

		APhysXObjectConnector* pPhysXConnector = m_pPhysScene->CreatePhysXObjectConnector(apxConnectorInstanceDesc);
		pSlot->m_aHLCons.Add(pPhysXConnector);
	}

	return true;
}

/*	Close physical property for specified skin

	bCompletely: true, completely release physical cloth data; false, only unbind physical cloth
*/
void A3DModelPhysics::CloseClothSkin(int iSlot, bool bCompletely/* false */)
{
	if (!m_pPhysScene || !m_pSkinModel)
		return;

	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);
	if (pSkin)
		pSkin->EnableClothes(false);

	if (iSlot < 0 || iSlot >= m_aSkinSlots.GetSize())
	{
		ASSERT(0);
		return;
	}

	//	Unbind skin physical data
	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	if (pSlot->pPhysSync)
	{
		if (bCompletely)
		{
			delete pSlot->pPhysSync;
			pSlot->pPhysSync = NULL;
		}
		else
		{
			pSlot->pPhysSync->Unbind();
		}
	}

	//	Release all hard-link connectors
	for (int i=0; i < pSlot->m_aHLCons.GetSize(); i++)
	{
		if (pSlot->m_aHLCons[i])
			m_pPhysScene->ReleasePhysXObjectConnnector(pSlot->m_aHLCons[i]);
	}

	pSlot->m_aHLCons.RemoveAll(false);
}

//	Check if skin's physical property is ready
bool A3DModelPhysics::IsClothSkin(int iSlot) const
{
	if (!m_pPhysScene)
		return false;

	if (iSlot < 0 || iSlot >= m_aSkinSlots.GetSize())
	{
		ASSERT(0);
		return false;
	}

	A3DSkinPhysSync* pSkinPhys = m_aSkinSlots[iSlot]->pPhysSync;
	if (!pSkinPhys || !pSkinPhys->IsSkinBound())
		return false;

	return true;
}

//	Get skin physical sync object
A3DSkinPhysSync* A3DModelPhysics::GetSkinPhysSync(int iSlot) const
{
	if (iSlot >= 0 && iSlot < m_aSkinSlots.GetSize())
		return m_aSkinSlots[iSlot]->pPhysSync;
	else
		return NULL;
}

//	Get link type of specified cloth skin. 0-not link; 1-soft link; 2-hard link
int A3DModelPhysics::GetClothLinkType(int iSlot) const
{
	if (iSlot >= 0 && iSlot < m_aSkinSlots.GetSize())
	{
		SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
		if (pSlot->pPhysSync)
			return pSlot->bSoftLink ? 1 : 2;
	}

	return 0;
}

void A3DModelPhysics::SetClothWindAcceleration(const A3DVECTOR3& vWindAcceleration)
{
	int i;

	for (i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		A3DSkinPhysSync* pPhysSkin = m_aSkinSlots[i]->pPhysSync;
		if (pPhysSkin && pPhysSkin->IsSkinBound())
			pPhysSkin->SetClothWindAcceleration(vWindAcceleration);
	}	
}

//	Create phys link between parent model and child model
//	link info contains parent's hook name and child's attacher name
//	if child model(current model) is in phys simulate state, we update the attachers by parent's hooks
//	else if the child model is in animation state, we update the child model by parent's hooks using HH and CC hook as usual
bool A3DModelPhysics::OpenPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher) const
{
	if (!m_pModelSync)
		return false;

	return m_pModelSync->OpenPhysLinkAsChildModel(szParentHook, szChildAttacher);
}

void A3DModelPhysics::ClosePhysLinkAsChildModel() const
{
	if (!m_pModelSync)
		return;

	m_pModelSync->ClosePhysLinkAsChildModel();
}

bool A3DModelPhysics::AddPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher) const
{
	if (!m_pModelSync)
		return false;

	return m_pModelSync->AddPhysLinkAsChildModel(szParentHook, szChildAttacher);
}

bool A3DModelPhysics::RemovePhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher) const
{
	if (!m_pModelSync)
		return false;

	return m_pModelSync->RemovePhysLinkAsChildModel(szChildAttacher);
}

bool A3DModelPhysics::ChangePhysLinkAsChildModel(const char* szChildAttacher, const char* szNewHook) const
{
	if (!m_pModelSync)
		return false;

	return m_pModelSync->ChangePhysLinkAsChildModel(szChildAttacher, szNewHook);
}

//	Update routine before physics simulation
bool A3DModelPhysics::UpdateBeforePhysXSim(DWORD dwDeltaTime)
{
	if (!m_pSkinModel)
		return false;

	if (!m_pPhysScene)
	{
		//	Only tick skin model
		m_pSkinModel->Update((int)dwDeltaTime);
		return true;
	}

	//	Update model animations
	if (m_pModelSync && m_pModelSync->GetPhysBindFlag())
		m_pModelSync->UpdateModelAnim((int)dwDeltaTime);
	else
		m_pSkinModel->Update((int)dwDeltaTime);

	//	Update cloth attacher pose
	for (int i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		SKIN_SLOT* pSlot = m_aSkinSlots[i];
		A3DSkinPhysSync* pPhysSkin = pSlot->pPhysSync;

		if (pPhysSkin && pPhysSkin->IsSkinBound() && pSlot->bSoftLink)
		{
			pPhysSkin->UpdateRBActorPose(m_pSkinModel);
			pPhysSkin->UpdateSkinByAnimation(m_pSkinModel);
		}
	}

	return true;
}

//	Sync routine after physics simulation
bool A3DModelPhysics::SyncAfterPhysXSim()
{
	if (!m_pPhysScene || !m_pSkinModel)
		return false;

	//	Sync model pose from physics simulation result
	if (m_pModelSync && m_pModelSync->GetPhysBindFlag())
		m_pModelSync->SyncModelPhys();

	//	Sync skin mesh from physics simulation result
	for (int i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		A3DSkinPhysSync* pPhysSkin = m_aSkinSlots[i]->pPhysSync;
		A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(i);

		if (pPhysSkin && pPhysSkin->IsSkinBound() && pSkin)
			pPhysSkin->SyncSkinPhys(pSkin);
	}

	return true;
}

//	Change physical state
bool A3DModelPhysics::ChangePhysState(int iState)
{
	if (!m_pModelSync)
		return false;

	if (!m_pModelSync->ChangePhysState(iState))
		return false;

	//	Wake up all clothes
	for (int i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		SKIN_SLOT* pSlot = m_aSkinSlots[i];
		if (pSlot->pPhysSync && pSlot->pPhysSync->GetClothAttacher())
			pSlot->pPhysSync->GetClothAttacher()->WakeUp();
	}

	return true;
}

//	Get current physical state
int A3DModelPhysics::GetPhysState() const
{
	if (m_pModelSync)
		return m_pModelSync->GetPhysState();
	else
		return A3DModelPhysSync::PHY_STATE_ANIMATION;
}

//	Get model mass
float A3DModelPhysics::GetModelMass() const
{
	if (!m_pModelSync)
		return 0.0f;

	return m_pModelSync->GetModelMass();
}

//	After model is teleport to a new position, this function should be could to sync physical data
bool A3DModelPhysics::Teleport()
{
	if (m_pModelSync)
		m_pModelSync->Teleport();

	return true;
}

//	Begin editing specified cloth skin. Should be called by editor only.
A3DSkinPhysSyncData* A3DModelPhysics::BeginEditClothSkin(int iSlot, const AArray<int>& aRigidMeshIdx, const AArray<int>& aSkinMeshIdx)
{
	if (!m_pPhysScene || !m_pSkinModel)
		return NULL;

	//	Change skin to cloth
	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);
	if (!pSkin)
		return NULL;

	A3DSkinPhysSyncData* pSyncData = new A3DSkinPhysSyncData;
	if (!pSyncData)
		return NULL;

	//	Make sync data file name
	AString strFile = pSkin->GetFileName();
	af_ChangeFileExt(strFile, ".sphy");

	//	Try to load data from file
	if (!pSyncData->Load(strFile))
	{
		//	Re-create a sync data object
		delete pSyncData;
		if (!(pSyncData = new A3DSkinPhysSyncData))
			return NULL;

		pSyncData->m_strFileName = strFile;
	}

	//	Rebuild meshes from skin
	if (!pSyncData->BuildMeshFromSkin(m_pSkinModel, pSkin, aRigidMeshIdx, aSkinMeshIdx))
	{
		delete pSyncData;
		return NULL;
	}

	return pSyncData;
}

//	End editing specified cloth skin. Should be called by editor only.
bool A3DModelPhysics::EndEditClothSkin(A3DSkinPhysSyncData* pSkinSyncData)
{
	if (!pSkinSyncData)
	{
		ASSERT(pSkinSyncData);
		return false;
	}

	delete pSkinSyncData;
	pSkinSyncData = NULL;

	return true;
}

//	Create an empty model sync. 
bool A3DModelPhysics::CreateEmptyModelSync()
{
	m_pModelSync = new A3DModelPhysSync(this);
	return m_pModelSync ? true : false;
}

//	Release model sync. 
void A3DModelPhysics::ReleaseModelSync()
{
	if (m_pModelSync)
	{
		delete m_pModelSync;
		m_pModelSync = NULL;
	}
}

bool A3DModelPhysics::SetModelCollisionChannel(int iMethod, A3DModelPhysics* pParentPhysics)
{
	if (iMethod == CECModel::COLLISION_CHANNEL_AS_PARENT && !pParentPhysics)
		return false;

	APhysXCollisionChannel uChannel = APX_COLLISION_CHANNEL_INVALID;
	switch(iMethod)
	{
	case CECModel::COLLISION_CHANNEL_INDEPENDENT:
		uChannel = m_pPhysScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel();
		if (uChannel == APX_COLLISION_CHANNEL_INVALID)
			uChannel = APX_COLLISION_CHANNEL_COMMON;
		break;
	case CECModel::COLLISION_CHANNEL_AS_PARENT:
		uChannel = pParentPhysics->GetCollisionChannel();
		break;
	case CECModel::COLLISION_CHANNEL_AS_COMMON:
		uChannel = APX_COLLISION_CHANNEL_COMMON;
		break;

	case CECModel::COLLISION_CHANNEL_AS_CC_COMMON_HOST:
		uChannel = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
		break;

	case CECModel::COLLISION_CHANNEL_AS_MAJORRB:
		uChannel = m_uiMajorRBCollisionCh;
		break;

	}

	SetCollisionChannel(uChannel);
	return true;
}

//	Get breakable information
bool A3DModelPhysics::GetRBCreatorInfo(PhysRBCreatorInfo* pInfo)
{
	if (!pInfo || !m_pModelSyncData)
		return false;

	A3DPhysRBCreator* pRBCreator = m_pModelSyncData->GetPhysRBDescCreator();
	if (!pRBCreator)
		return false;

	if (!pRBCreator->GetCreatorInfo(pInfo))
		return false;

	return true;
}

//	Break Joint
void A3DModelPhysics::BreakJoint(const char* szJointName)
{
	if (!m_pModelSync)
		return;

	m_pModelSync->BreakJoint(szJointName);
}

//	Break Joint By Bone
void A3DModelPhysics::BreakJointByBone(const char* szBoneName)
{
	if (!m_pModelSync)
		return;

	m_pModelSync->BreakJointByBone(szBoneName);
}

//	Apply force
bool A3DModelPhysics::AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist, int iPhysForceType, float fSweepRadius /*= 0.0f*/)
{
	if (!m_pModelSync)
		return false;

	return m_pModelSync->AddForce(vStart, vDir, fForceMagnitude, fMaxDist, iPhysForceType, fSweepRadius);
}

#if 0
// remove the function...
//	Enable / Disable collision channel
void A3DModelPhysics::EnableCollisionChannel(bool bEnable)
{
/*
	
	// obsolete code...

	if (bEnable)
	{
		if (m_bCollisionCh && m_idCollisionCh != APX_COLLISION_CHANNEL_INVALID)
			return;		//	Collision channel has been enabled

		m_bCollisionCh	= true;
		m_idCollisionCh = m_pPhysScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel();

		if (m_idCollisionCh != APX_COLLISION_CHANNEL_INVALID)
		{
			if (m_pModelSync && m_pModelSync->GetSkeletonRBObject())
				m_pModelSync->GetSkeletonRBObject()->SetCollisionChannel(m_idCollisionCh);

			//	Set collision channel for cloth attachers
			SetCollisionChForClothes(m_idCollisionCh);
		}
	}
	else
	{
		if (m_idCollisionCh == APX_COLLISION_CHANNEL_INVALID)
			return;		//	Collision channel has been disabled

		m_bCollisionCh	= false;
		m_idCollisionCh	= APX_COLLISION_CHANNEL_INVALID;

		if (m_pModelSync && m_pModelSync->GetSkeletonRBObject())
			m_pModelSync->GetSkeletonRBObject()->SetCollisionChannel(APX_COLLISION_CHANNEL_COMMON);

		//	Set collision channel for cloth attachers
		SetCollisionChForClothes(APX_COLLISION_CHANNEL_COMMON);
	}

*/

	if(m_bCollisionCh == bEnable) return;

	m_bCollisionCh = bEnable;

	if(m_bCollisionCh)
	{
		SetCollisionChannel(m_pPhysScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel());
	}
	else
	{
		SetCollisionChannel(APX_COLLISION_CHANNEL_COMMON);
	}
}

#endif

//	Set collision channel for all physical skins
void A3DModelPhysics::SetCollisionChForClothes(unsigned int idChannel)
{
	int i;

	for (i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		A3DSkinPhysSync* pPhysSkin = m_aSkinSlots[i]->pPhysSync;
		if (pPhysSkin && pPhysSkin->IsSkinBound())
			pPhysSkin->GetClothAttacher()->SetCollisionChannel(idChannel);
	}
}

void A3DModelPhysics::SetCollisionChannel(unsigned int uCollisionChannel)
{
	if(m_idCollisionCh == uCollisionChannel || uCollisionChannel == APX_COLLISION_CHANNEL_INVALID)
		return;

	m_idCollisionCh = uCollisionChannel;

	if (m_pModelSync && m_pModelSync->GetAPhysxRBAttacher())
		m_pModelSync->GetAPhysxRBAttacher()->SetCollisionChannel(m_idCollisionCh);
	
	//	Set collision channel for cloth attachers
	SetCollisionChForClothes(m_idCollisionCh);

}

#endif	//	A3D_PHYSX

