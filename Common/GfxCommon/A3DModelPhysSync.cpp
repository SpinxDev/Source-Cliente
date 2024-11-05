/*
 * FILE: A3DModelPhysSync.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/5/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#ifdef A3D_PHYSX

#include "A3DModelPhysSync.h"
#include "A3DModelPhysSyncData.h"

#include "APhysX.h"
#include "EC_Model.h"

#include <A3DSkinModel.h>
#include <A3DSkeleton.h>
#include <memory>


//------------------------------------------------------------------------
// some test code for breakable skeleton objects...

class ASkinModelBreakReport : public APhysXUserBreakReport
{
	
public:

	virtual void OnBreakNotify(APhysXBreakable* pBreakable)
	{

		APhysXObject* pBreakObj = pBreakable->IsAPhysXObject();

		if(pBreakObj)
		{
		
			PhysBreakInfo physBreakInfo;
			physBreakInfo.mBreakPos = APhysXConverter::N2A_Vector3(pBreakable->GetBreakPos());
			physBreakInfo.mBreakMagnitude = pBreakable->GetBreakMagnitude();

			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnBreak(physBreakInfo);
		
			}

			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnBreak(physBreakInfo);
			}

		}

	}

	virtual void OnFirstBreakNotify(APhysXBreakable* pBreakable) 
	{
		APhysXObject* pBreakObj = pBreakable->IsAPhysXObject();
		
		if(pBreakObj)
		{
			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnFirstBreak();
				
			}
			
			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnFirstBreak();
				
			}
			
		}
	}
	
	virtual void OnAllBreakNotify(APhysXBreakable* pBreakable) 
	{
		APhysXObject* pBreakObj = pBreakable->IsAPhysXObject();
		
		if(pBreakObj)
		{
			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnAllBreak();
				
			}
			
			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnAllBreak();
				
			}
			
		}
	}

	ASkinModelBreakReport()
	{
		gPhysXBreakReport->Register(this);
	}


};

// try to register this break report after the APhysXScene is created...
static ASkinModelBreakReport gSkinModelBreakReport;



//------------------------------------------------------------------------


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
//	Implement A3DModelPhysSync::ACTOR_IMP
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSync::ACTOR_IMP::ACTOR_IMP()
{
	pActor	= NULL;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysSync::BONE_MAT
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSync::BONE_MAT::BONE_MAT()
: matBoneAbs(A3DMATRIX4::IDENTITY)
, matBoneInvAbs(A3DMATRIX4::IDENTITY)
{
	bMainBone = false;
}

A3DModelPhysSync::BONE_SCALE_CACHE_DATA::BONE_SCALE_CACHE_DATA(A3DBone* pBone)
: fLocalLengthScale(pBone->GetLocalLengthScale())
, fLocalThickScale(pBone->GetLocalThickScale())
, fAccuWholeScale(pBone->GetAccuWholeScale())
{

}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysSync
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSync::A3DModelPhysSync(A3DModelPhysics* pModelPhysics)
: m_pModelPhysics(pModelPhysics)
{
	ASSERT(m_pModelPhysics);

	m_pPhysScene	= NULL;
	m_pCoreData		= NULL;
	m_pPhysModel	= NULL;
	m_pPhysAttacher	= NULL;
	m_pSkinModel	= NULL;
	m_bPhysBindOk	= false;
	m_bPhysBroken	= false;
	m_iPhysState	= PHY_STATE_ANIMATION;
}

A3DModelPhysSync::~A3DModelPhysSync()
{
	//	Release physical attacher (including phys model)
	ReleasePhysAttacher();

	//	Release all actor implements
	ReleaseActorImps();

	//	Release all attacher link info
	ReleaseAttacherLinkInfo();

	m_aBoneMats.RemoveAll();
}

//	Release physical attacher
void A3DModelPhysSync::ReleasePhysAttacher()
{
	if (m_pPhysAttacher && m_pPhysScene)
	{
		m_pPhysScene->ReleasePhysXObject(m_pPhysAttacher);
		m_pPhysAttacher = NULL;
		m_pPhysModel = NULL;
	}
}

/*	Bind skin model with a physical object

	bReBindAll: if this flag is set to false, we assume that transform matrices between bone and actor have been 
			calculated and they won't be changed. otherwise (bReBindAll = true) all the transform matrices will
			be re-calculated.

	bRebuildDesc: if this flag is set to true, the old cached attacher-desc would be released and re-build a new one
*/
bool A3DModelPhysSync::Bind(A3DSkinModel* pSkinModel, A3DModelPhysSyncData* pCoreData, APhysXScene* pPhysScene, bool bRebuildDesc /*= false*/)
{
	if (!pSkinModel || !pCoreData || !pPhysScene)
		return false;

	if (!pCoreData->GetBindInitFlag())
		return false;

	m_bPhysBindOk	= false;
	m_bPhysBroken	= false;
	m_pPhysScene	= pPhysScene;
	m_pSkinModel	= pSkinModel;
	m_pCoreData		= pCoreData;

	//	Create new physical attacher including physical model
	if (!CreatePhysAttacherAndModel(pSkinModel, bRebuildDesc))
		return false;

	//	Build actor implements
	if (!BuildActorImps())
		return false;

	//	Allocalte space for transform data
	int iBoneNum = pSkinModel->GetSkeleton()->GetBoneNum();
	m_aBoneMats.SetSize(iBoneNum, 4);

	m_bPhysBindOk = true;

	//	After creation, set all attachers be dynamic
	m_pPhysAttacher->EnableAttacherKinematic(false);

	//	Apply current physical state
	ChangePhysState(m_iPhysState);

	return true;
}

//	Check whether the skeleton is scaled
bool A3DModelPhysSync::CheckSkeletonScaled(A3DSkeleton* pSkeleton)
{
	ASSERT(pSkeleton);

	const float fScaleDelta = 0.001f;

	if (!pSkeleton)
		return false;

	for (int iBoneIdx = 0; iBoneIdx < pSkeleton->GetBoneNum(); ++iBoneIdx)
	{
		A3DBone* pBone = pSkeleton->GetBone(iBoneIdx);
		ASSERT(pBone);

		if (fabs(pBone->GetLocalLengthScale() - 1.f) > fScaleDelta
			|| fabs(pBone->GetLocalThickScale() - 1.f) > fScaleDelta
			|| fabs(pBone->GetWholeScale() - 1.f) > fScaleDelta)
			return true;
	}

	return false;
}

//	Create physical model
bool A3DModelPhysSync::CreatePhysAttacherAndModel(A3DSkinModel* pSkinModel, bool bRebuildDesc)
{
	if (!m_pPhysScene || !m_pCoreData || !m_pCoreData->GetModelPhysDesc())
		return false;

	APhysXRBAttacher* pAttacherObj = NULL;
	
	//	Set model's pose 
	NxMat34 mtPose;
	APhysXConverter::A2N_Matrix44(pSkinModel->GetAbsoluteTM(), mtPose);

	if (!bRebuildDesc && !CheckSkeletonScaled(pSkinModel->GetSkeleton()))
	{
		APhysXObjectInstanceDesc apxObjInstanceDesc;
		apxObjInstanceDesc.mPhysXObjectDesc = m_pCoreData->GetAttacherPhysDesc();
		apxObjInstanceDesc.mGlobalPose = mtPose;
		apxObjInstanceDesc.mScale3D = NxVec3(1.0f);
		pAttacherObj = (APhysXRBAttacher*)m_pPhysScene->CreatePhysXObject(apxObjInstanceDesc);
	}
	else
	{
		//	Use RRID (Resource release is destruction) to delete the pointer at the destructing time
		std::auto_ptr<APhysXSkeletonRBObjectDesc> pObjectDesc((APhysXSkeletonRBObjectDesc*)m_pCoreData->GetModelPhysDesc()->Clone());
		if (!pObjectDesc.get())
			return false;
		
		//	Conside model's scale factor
		A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();
		
		int i, iActorNum = m_pCoreData->GetActorNum();
		A3DMATRIX4 matLocalPose, matUpToRoot;
		A3DVECTOR3 vOrigin, vAxisX, vAxisY, vAxisZ;
		
		for (i=0; i < iActorNum; i++)
		{
			A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(i);
			A3DBone* pMainBone = pSkeleton->GetBone(pActorData->Link.iMainBone);
			if (!pMainBone)
				return false;
			
			//	Get the child bone that decides actor's length
			A3DBone* pLenBone = NULL;
			if (pActorData->Link.iLenBone >= 0)
				pLenBone = pSkeleton->GetBone(pActorData->Link.iLenBone);
			
			vOrigin = pActorData->Link.matActor2Bone.GetRow(3);
			vAxisY = vOrigin + pActorData->Link.matActor2Bone.GetRow(1);
			vAxisX = vOrigin + pActorData->Link.matActor2Bone.GetRow(0);
			
			matUpToRoot = pMainBone->GetScaleMatrix() * pMainBone->GetUpToRootTM();
			vOrigin = vOrigin * matUpToRoot;
			vAxisY = vAxisY * matUpToRoot - vOrigin;
			vAxisX = vAxisX * matUpToRoot - vOrigin;
			
			float fLenY = vAxisY.Normalize();
			float fLenFactor = pLenBone ? pLenBone->GetLocalLengthScale() * pMainBone->GetAccuWholeScale() : fLenY;
			float fRadFactor = vAxisX.Normalize();
			vAxisZ = CrossProduct(vAxisX, vAxisY);
			
			//	Conside foot offset
			vOrigin.y -= pSkeleton->GetFootOffset();
			
			matLocalPose.SetRow(0, vAxisX);
			matLocalPose.SetRow(1, vAxisY);
			matLocalPose.SetRow(2, vAxisZ);
			matLocalPose.SetRow(3, vOrigin);
			
			APhysXActorDesc* pActorDesc = pObjectDesc->GetActorDesc(i);
			APhysXConverter::A2N_Matrix44(matLocalPose, pActorDesc->mLocalPose);
			pActorDesc->mScale3D.set(fRadFactor, fLenFactor, fRadFactor);
		}
		
		//	Adjust joint's position
		int iJointNum = m_pCoreData->GetJointNum();
		for (i=0; i < iJointNum; i++)
		{
			A3DModelPhysSyncData::JOINT_DATA* pJointData = m_pCoreData->GetJointByIndex(i);
			A3DBone* pBone = pSkeleton->GetBone(pJointData->iBoneIdx);
			if (!pBone)
				return false;
			
			//	Adjust joint's position
			vOrigin = pBone->GetUpToRootTM().GetRow(3);
			vOrigin.y -= pSkeleton->GetFootOffset();
			vOrigin += pJointData->GetOffset() * pBone->GetAccuWholeScale();
			APhysXJointDesc* pJointDesc = (APhysXJointDesc*)pObjectDesc->GetInnerConnector(i);
			pJointDesc->mGlobalAnchor = APhysXConverter::A2N_Vector3(vOrigin);
		}
		
		//	Create attacher desc by skeleton desc we generated previously
		//	We need to delay the creation of attacher's desc to this moment because the skeleton desc may be changed on scaling
		//	This creation would generate all the attachers we created in the editor by adding attacher
		std::auto_ptr<APhysXAttacherDesc> pAttacherDesc(m_pCoreData->CreateAttacherPhysDescBySkeletonDesc(pObjectDesc.get(), m_pSkinModel->GetSkeleton()));
		if (!pAttacherDesc.get())
			return false;

		APhysXObjectInstanceDesc apxObjInstanceDesc;
		apxObjInstanceDesc.mPhysXObjectDesc = pAttacherDesc.get();
		apxObjInstanceDesc.mGlobalPose = mtPose;
		apxObjInstanceDesc.mScale3D = NxVec3(1.0f);
		pAttacherObj = (APhysXRBAttacher*)m_pPhysScene->CreatePhysXObject(apxObjInstanceDesc);
	}


	if (!pAttacherObj)
		return false;

	ReleasePhysAttacher();
	m_pPhysAttacher = pAttacherObj;

	// Get the physical model's pointer out instead of creating a separate physical model object 
	m_pPhysModel = (APhysXSkeletonRBObject*)pAttacherObj->GetRBObject();

	m_pPhysModel->mUserData = this;

	return true;
}

//	Release all actor implements
void A3DModelPhysSync::ReleaseActorImps()
{
	for (int i=0; i < m_aActorImps.GetSize(); i++)
	{
		ACTOR_IMP* pImp = m_aActorImps[i];
		delete pImp;
	}

	m_aActorImps.RemoveAll();
}

//	Release all attacher link infos
void A3DModelPhysSync::ReleaseAttacherLinkInfo()
{
	for (int i = 0; i < m_aAttacherLinks.GetSize(); ++i)
	{
		ATTACHER_LINK* pLink = m_aAttacherLinks[i];
		delete pLink;
	}

	m_aAttacherLinks.RemoveAll();
}

//	Build actor implements
bool A3DModelPhysSync::BuildActorImps()
{
	//	Release old actor implements
	ReleaseActorImps();

	if (!m_pPhysModel || !m_pCoreData)
		return false;

	int i, iActorNum = m_pCoreData->GetActorNum();
	for (i=0; i < iActorNum; i++)
	{
		A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(i);
		ACTOR_IMP* pImp = new ACTOR_IMP;

		pImp->pActor = m_pPhysModel->GetNxActor(pActorData->strName);
		if (!pImp->pActor)
			return false;

		m_aActorImps.Add(pImp);
	}

	return true;
}

//	Build attacher link infos
bool A3DModelPhysSync::BuildAttacherLinkInfos(const char* szParentHook, const char* szChildAttacher)
{
	ASSERT(szParentHook && szChildAttacher);

	//	Release old attacher link infos
	ReleaseAttacherLinkInfo();

	if (!m_pPhysAttacher || !m_pPhysModel || !m_pCoreData || !m_pSkinModel)
		return false;

 	A3DSkinModel* pParentModel = m_pSkinModel->GetParent();
 	if (!pParentModel)
 		return false;

 	A3DSkeleton* pParentSkeleton = pParentModel->GetSkeleton();
 	if (!pParentSkeleton)
 		return false;

	//	Can not find parent hook
	int iHookIdx = -1;
	if (!pParentSkeleton->GetHook(szParentHook, &iHookIdx))
		return false;

	//	Can not find attacher
	int iAttacherIdx = -1;
	if (!m_pCoreData->GetAttacherByName(szChildAttacher, &iAttacherIdx))
		return false;

	ATTACHER_LINK* pAttacherLink = new ATTACHER_LINK;
	pAttacherLink->strParentHook = szParentHook;
	pAttacherLink->strAttacher = szChildAttacher;
	pAttacherLink->nParentHookIdx = iHookIdx;
	pAttacherLink->nAttacherIdx = iAttacherIdx;
	m_aAttacherLinks.Add(pAttacherLink);
	return true;
}

//	Update model's animation. Call this before physics simulation
bool A3DModelPhysSync::UpdateModelAnim(int iDeltaTime)
{
	if (!m_pSkinModel)
		return false;

	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
	{
		m_pSkinModel->Update(iDeltaTime);
		return true;
	}

	if (m_iPhysState == PHY_STATE_ANIMATION)
		m_pSkinModel->Update(iDeltaTime);
	else
		m_pSkinModel->UpdateSkeletonAndAnim(iDeltaTime, false);

	//	Update actor pose by animation
	UpdateActorsByAnim(false);

	//	Update attachers by parent & link info
	UpdateAttachersByParentAndLinkInfo();

	return true;
}

//	Update actor state by animation
void A3DModelPhysSync::UpdateActorsByAnim(bool bTeleport)
{
	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return;

	if (m_iPhysState == PHY_STATE_ANIMATION || m_iPhysState == PHY_STATE_PARTSIMULATE)
	{
		A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
		A3DMATRIX4 mat;
		NxMat34 matNx;

		//	Sync actors pose from bones
		for (int i=0; i < m_aActorImps.GetSize(); i++)
		{
			ACTOR_IMP* pActorImp = m_aActorImps[i];
			if (!pActorImp->pActor->readBodyFlag(NX_BF_KINEMATIC))
				continue;

			A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(i);
			A3DModelPhysSyncData::LINK_INFO& link = pActorData->Link;

			//	Get major bone
			A3DBone* pBone = pSkeleton->GetBone(link.iMainBone);
			mat = link.matActor2Bone * pBone->GetNoScaleAbsTM();
			APhysXConverter::A2N_Matrix44(mat, matNx);

			//---------------------------------------------------
			// try to acquire the writelock of the scene before 
			// make changes on actors...

			m_pPhysScene->LockWritingScene();
			
			if (bTeleport)
				pActorImp->pActor->setGlobalPose(matNx);
			else
			{
				assert(matNx.isFinite());
//				assert(matNx.M.isRotation());
				pActorImp->pActor->moveGlobalPose(matNx);
				//pActorImp->pActor->setGlobalPose(matNx);
			}
				
			
			m_pPhysScene->UnlockWritingScene();
			//---------------------------------------------------
		}
	}
}

//	Update attachers state by parent's hooks position
void A3DModelPhysSync::UpdateAttachersByParentAndLinkInfo()
{
	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return;
		
	//	Phys attacher object is generated (bind succeed)
	if (!m_pPhysAttacher)
		return;

	//	Only if this model is a child model, we need to update its attacher by parent's hook
	A3DSkinModel* pParent = m_pSkinModel->GetParent();
	if (!pParent)
		return;

	A3DSkeleton* pParentSkeleton = pParent->GetSkeleton();
	if (!pParentSkeleton)
		return;
	
	int nIdx, nNum = m_aAttacherLinks.GetSize();
	for (nIdx = 0; nIdx < nNum; ++nIdx)
	{
		if (m_aAttacherLinks[nIdx]->nParentHookIdx == -1)
			pParentSkeleton->GetHook(m_aAttacherLinks[nIdx]->strParentHook, &m_aAttacherLinks[nIdx]->nParentHookIdx);

		if (m_aAttacherLinks[nIdx]->nAttacherIdx == -1)
			m_pCoreData->GetAttacherByName(m_aAttacherLinks[nIdx]->strAttacher, &m_aAttacherLinks[nIdx]->nAttacherIdx);

		//	Can not look for the specified hook & attacher, skip this
		if (m_aAttacherLinks[nIdx]->nParentHookIdx == -1
			|| m_aAttacherLinks[nIdx]->nAttacherIdx == -1) {
			ASSERT(FALSE);
			continue;
		}
		
		A3DSkeletonHook* pHook = pParentSkeleton->GetHook(m_aAttacherLinks[nIdx]->nParentHookIdx);
		if (!pHook)
			continue;
		
		NxMat34 nmPos;
		APhysXConverter::A2N_Matrix44(pHook->GetNoScaleAbsTM(), nmPos);
//		assert(nmPos.M.isRotation());
		assert(nmPos.isFinite());
		//if(stricmp(m_aAttacherLinks[nIdx]->strAttacher, "chest") == 0)
		//{
		//	int i = 0;	
		//}

		m_pPhysAttacher->SetAttacherPose(m_aAttacherLinks[nIdx]->strAttacher, nmPos);
		/*
#ifdef _DEBUG
		AString ss;
		NxVec3 r1, r2, r3;
		r1 = nmPos.M.getRow(0);
		r2 = nmPos.M.getRow(1);
		r3 = nmPos.M.getRow(2);
		ss.Format("nmPos: %f, %f, %f, %f, %f, %f, %f, %f, %f     %f, %f, %f\n", r1.x, r1.y, r1.z, r2.x, r2.y, r2.z, r3.x, r3.y, r3.z, nmPos.t.x, nmPos.t.y, nmPos.t.z);
		OutputDebugStringA(ss);
#endif
		*/
	}
}

//	Sync model state from physical data. Call this after physics simulation
bool A3DModelPhysSync::SyncModelPhys()
{
	if (m_iPhysState == PHY_STATE_ANIMATION || !m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return true;
	bool bWrongSimulate = false;
	if (m_iPhysState == PHY_STATE_SIMULATE)
	{
		//	Set model's position base on simulation result
		int iRootBoneActor = m_pCoreData->m_iRootActor;
		if (iRootBoneActor < 0 || !m_pCoreData->m_pRootBoneData)
		{
			ASSERT(iRootBoneActor >= 0 && m_pCoreData->m_pRootBoneData);
			return false;
		}

		NxActor* pActor = m_aActorImps[iRootBoneActor]->pActor;
		if (!pActor)
			return false;

		//	Root bone's position will be used as model's position. At the same time
		//	we should reset model's direction to binding-time (when physical actors
		//	were bound with model) value.
		A3DMATRIX4 matActor;
		APhysXConverter::N2A_Matrix44(pActor->getGlobalPose(), matActor);
		A3DMATRIX4 mat = m_pCoreData->m_pRootBoneData->matBone2Actor * matActor;
		// assert(pActor->getGlobalPose().isFinite());
		if(!pActor->getGlobalPose().isFinite())
		{
			ChangePhysState(PHY_STATE_ANIMATION);
			bWrongSimulate = true;
		}
		else
		{
			m_pSkinModel->SetPos(mat.GetRow(3));
			m_pSkinModel->SetDirAndUp(A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
		}

	}
	
	if(!bWrongSimulate)
	{
		//	Update physically driven bones' state
		UpdateBonesByPhys();

		//	Sync physical state
		m_pSkinModel->Update(0, true);

	}

	return true;
}

//	Update physically driven bones' state
void A3DModelPhysSync::UpdateBonesByPhys()
{
	ASSERT(m_pSkinModel && m_pPhysModel);

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	int i, j;
	A3DMATRIX4 mat, matActor;

	//	Update all bones absolute matrices and their inverse matrices
	for (i=0; i < m_aActorImps.GetSize(); i++)
	{
		ACTOR_IMP* pActorImp = m_aActorImps[i];
		A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(i);
		A3DModelPhysSyncData::LINK_INFO& link = pActorData->Link;

		//	Get actor's absolute matrix
		APhysXConverter::N2A_Matrix44(pActorImp->pActor->getGlobalPose(), matActor);

		for (j=0; j < link.aBones.GetSize(); j++)
		{
			A3DModelPhysSyncData::BONE_DATA* pBoneData = link.aBones[j];
			int iBoneIdx = pBoneData->iBoneIdx;
			A3DBone* pBone = pSkeleton->GetBone(iBoneIdx);
			if (!pBone)
			{
				ASSERT(pBone);
				return;
			}

			m_aBoneMats[iBoneIdx].bMainBone = (iBoneIdx == link.iMainBone);

			//	Get bone's absolute matrix and it's inverse matrix
			if (pBone->IsAnimDriven())
			{
				m_aBoneMats[iBoneIdx].matBoneAbs = pBone->GetNoScaleAbsTM();
				a3d_InverseTM(m_aBoneMats[iBoneIdx].matBoneAbs, &m_aBoneMats[iBoneIdx].matBoneInvAbs);
			}
			else if (pBone->GetParent() < 0 || iBoneIdx == link.iMainBone)
			{
				mat = pBoneData->matBone2Actor * matActor;
				m_aBoneMats[iBoneIdx].matBoneAbs = mat;
				a3d_InverseTM(mat, &m_aBoneMats[iBoneIdx].matBoneInvAbs);
			}
		}
	}

	//	Get model's inverse absolute matrix
	A3DVECTOR3 vModelPos = m_pSkinModel->GetPos();
	A3DMATRIX4 matInvModel;
	a3d_InverseTM(a3d_Translate(vModelPos.x, vModelPos.y, vModelPos.z), &matInvModel);

	//	Calculate all bones relative matrices
	int iNumRoot = pSkeleton->GetRootBoneNum();
	for (i=0; i < iNumRoot; i++)
	{
		int iRootBone = pSkeleton->GetRootBone(i);
		UpdateBonesByPhys_r(matInvModel, iRootBone);
	}
}

void A3DModelPhysSync::UpdateBonesByPhys_r(const A3DMATRIX4& matInvModel, int iBoneIdx)
{
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	
	A3DBone* pBone = pSkeleton->GetBone(iBoneIdx);

	if (!pBone->IsAnimDriven())
	{
		A3DMATRIX4 mat;

		int iParentIdx = pBone->GetParent();
		if (iParentIdx >= 0)
		{
			if (m_aBoneMats[iBoneIdx].bMainBone)
				mat = m_aBoneMats[iBoneIdx].matBoneAbs * m_aBoneMats[iParentIdx].matBoneInvAbs;
			else
			{	
				mat = pBone->GetOriginalMatrix();

				//	Conside scale factor on bone's relative offset
				A3DBone* pParent = pBone->GetParentPtr();
				float fLocalLenSF = pBone->GetLocalLengthScale() * pParent->GetAccuWholeScale();

				if (fLocalLenSF != 1.0f)
				{
					mat._41 *= fLocalLenSF;
					mat._42 *= fLocalLenSF;
					mat._43 *= fLocalLenSF;
				}

				A3DMATRIX4 matBoneAbs = mat * m_aBoneMats[iParentIdx].matBoneAbs;
				m_aBoneMats[iBoneIdx].matBoneAbs = matBoneAbs;
				a3d_InverseTM(matBoneAbs, &m_aBoneMats[iBoneIdx].matBoneInvAbs);
			}
		}
		else
			mat = m_aBoneMats[iBoneIdx].matBoneAbs * matInvModel;

		//	Use SetRelativeTM() directly can accelerate bone's update
	//	A3DQUATERNION quat(mat);
	//	pBone->AddBlendState(quat, mat.GetRow(3), 1.0f, A3DBone::BM_NORMAL);
		pBone->SetRelativeTM(mat);
	}

	//	Handle children
	int i, iNumChild = pBone->GetChildNum();
	for (i=0; i < iNumChild; i++)
		UpdateBonesByPhys_r(matInvModel, pBone->GetChild(i));
}


void A3DModelPhysSync::SetBoneAnimDriven(bool bAnimDriven)
{

	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();

	int i, iNumBone = pSkeleton->GetBoneNum();
	for (i=0; i < iNumBone; i++)
	{
		A3DBone* pBone = pSkeleton->GetBone(i);
		pBone->SetAnimDriven(bAnimDriven);
	}

}

//	每次Break
void A3DModelPhysSync::OnBreak(const PhysBreakInfo& physBreakInfo)
{
	// 在A3DModelPhysSync的生命期中， m_pModelPhysics总应当是有效的
	ASSERT(m_pModelPhysics);
	
	// 有些场合可能不需要ECModel的指针 (A3DModelPhysics在初始化Init时，使用的不是CECModel*而是A3DSkinModel*)
	// 这种情况下也就不需要调用ECModel的OnPhysBreak
	//ASSERT(m_pModelPhysics->GetECModel());

	if (!m_pModelPhysics || !m_pModelPhysics->GetECModel())
		return;

	// 破碎时的逻辑变更，交由ECModel处理，包括调用脚本(更换皮肤，播放GFX，播放SFX)，以及回调外界传入的函数指针等
	m_pModelPhysics->GetECModel()->OnPhysBreak(physBreakInfo);

}

//	First Break
void A3DModelPhysSync::OnFirstBreak()
{

	// callback when the the object firstly breaks from sanity status

	// 在A3DModelPhysSync的生命期中， m_pModelPhysics总应当是有效的
	ASSERT(m_pModelPhysics);
	
	if (!m_pModelPhysics || !m_pModelPhysics->GetECModel())
		return;
	
	// 初次破碎时的逻辑变更，交由ECModel处理，包括调用脚本(更换皮肤，播放GFX，播放SFX)，以及回调外界传入的函数指针等
	// m_pModelPhysics->GetECModel()->OnPhysBreak();

	// set all bone's driven mode to physics
	SetBoneAnimDriven(false);
	
	m_iPhysState = PHY_STATE_SIMULATE;

	m_bPhysBroken = true;
}

//	All Break
void A3DModelPhysSync::OnAllBreak()
{
	// callback when all breaking part have been broken...
	
	// to do...

}


//	Change physical state
bool A3DModelPhysSync::ChangePhysState(int iState)
{
	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return false;

	m_iPhysState = iState;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();

	if (iState == PHY_STATE_SIMULATE)
	{
		m_pSkinModel->StopAllActions();

		int i, iNumBone = pSkeleton->GetBoneNum();
		for (i=0; i < iNumBone; i++)
		{
			A3DBone* pBone = pSkeleton->GetBone(i);
			pBone->SetAnimDriven(false);
		}

		/*
		for (i=0; i < m_aActorImps.GetSize(); i++)
		{
			ACTOR_IMP* pActorImp = m_aActorImps[i];
			
			A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(i);
			if(!(pActorData->GetActorDesc()->mBodyFlags & NX_BF_KINEMATIC))
			{
				//---------------------------------------------------
				// try to acquire the writelock of the scene before 
				// make changes on actors...
				m_pPhysScene->LockWritingScene();
				
				pActorImp->pActor->clearBodyFlag(NX_BF_KINEMATIC);
				// pActorImp->pActor->wakeUp();
				m_pPhysScene->UnlockWritingScene();
				//---------------------------------------------------

			}


		}
		*/

		// m_pPhysModel->EnableKinematic(false);
		m_pPhysModel->SetDrivenMode(APX_RBDRIVENMODE_PHYSICS);


		//	when this model is not a child model
		//	when we make the model in part-physical state / physical state
		//	we need to let the Attacher object be in the physical state too (simulate with other actors)
		//	while when the model is a child model
		//	we skip this and set the attachers' position by the parent's hooks' position
		if (!m_pSkinModel->GetParent())
			m_pPhysAttacher->EnableAttacherKinematic(false);
	}
	else if (iState == PHY_STATE_ANIMATION)
	{
		int i, iNumBone = pSkeleton->GetBoneNum();
		for (i=0; i < iNumBone; i++)
		{
			A3DBone* pBone = pSkeleton->GetBone(i);
			pBone->SetAnimDriven(true);
		}

		/*
		for (i=0; i < m_aActorImps.GetSize(); i++)
		{
			ACTOR_IMP* pActorImp = m_aActorImps[i];

			//---------------------------------------------------
			// try to acquire the writelock of the scene before 
			// make changes on actors...
			m_pPhysScene->LockWritingScene();

			pActorImp->pActor->raiseBodyFlag(NX_BF_KINEMATIC);

			m_pPhysScene->UnlockWritingScene();
			//---------------------------------------------------
		}
		*/
		m_pPhysModel->SetDrivenMode(APX_RBDRIVENMODE_ANIMATION);

		if (m_bPhysBroken)
		{
			// Once the Physical skeleton object is broken
			// and we want it to be set to kinematic
			// we need to re-bind it (in order to re-create the skeleton object)
			// Bind will change the m_bPhysBroken to false.
			//	Bind(m_pSkinModel, m_pCoreData, m_pPhysScene);

			// APhysXBreakableSkeletonRBObject* pBreakableSkeleton = (APhysXBreakableSkeletonRBObject*)m_pPhysModel;
			// pBreakableSkeleton->Reset();

			// try to use the latest function to retrieve the Breakable interface
			APhysXBreakable* pBreakable = m_pPhysModel->IsBreakable();
			if(pBreakable)
				pBreakable->Reset();

		}

	}
	else if (iState == PHY_STATE_PARTSIMULATE)
	{
		int i, j;
		APhysXArray<NxActor* > arrDynamicActors;

		for (i=0; i < m_aActorImps.GetSize(); i++)
		{
			ACTOR_IMP* pActorImp = m_aActorImps[i];
			A3DModelPhysSyncData::LINK_INFO& link = m_pCoreData->GetActorByIndex(i)->Link;

			for (j=0; j < link.aBones.GetSize(); j++)
			{
				A3DModelPhysSyncData::BONE_DATA* pBoneData = link.aBones[j];
				A3DBone* pBone = pSkeleton->GetBone(pBoneData->iBoneIdx);
				if (!pBone)
					continue;


				if (pBoneData->bAnimMajor)
				{
					pBone->SetAnimDriven(true);
				}
				else
				{
					pBone->SetAnimDriven(false);
					arrDynamicActors.push_back(pActorImp->pActor);
				}

			}

		}

		m_pPhysModel->SetDrivenMode(APX_RBDRIVENMODE_MIX, &arrDynamicActors);

		//	when this model is not a child model
		//	when we make the model in part-physical state / physical state
		//	we need to let the Attacher object be in the physical state too (simulate with other actors)
		//	while when the model is a child model
		//	we skip this and set the attachers' position by the parent's hooks' position
		if (!m_pSkinModel->GetParent())
			m_pPhysAttacher->EnableAttacherKinematic(false);

	}
	else
	{
		ASSERT(0);
		return false;
	}

	return true;
}

//	Get RB object mass
float A3DModelPhysSync::GetModelMass() const
{
	if (!m_pPhysModel)
		return 0.0f;

	return m_pPhysModel->GetMass();
}

//	After model is teleport to a new position, this function should be could to sync physical data
bool A3DModelPhysSync::Teleport()
{
	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return true;

	ASSERT(m_pSkinModel);

	//	Change physical state so that we can reset all actors' pose
	int iPhysState = -1;
	if (m_iPhysState != PHY_STATE_ANIMATION)
	{
		iPhysState = m_iPhysState;
		if (!ChangePhysState(PHY_STATE_ANIMATION))
			return false;
	}

	//	Update actor pose
	UpdateActorsByAnim(true);

	//	Restore physical state
	if (iPhysState != -1)
	{
		ChangePhysState(iPhysState);

		//	After we change actor's pose through setGlobalPose() method, actor may enter a static state
		//	even if it's still in dynamic state. So we force to wake up all actors if they are driven
		//	by physical engine.
		if (iPhysState == PHY_STATE_SIMULATE || iPhysState == PHY_STATE_PARTSIMULATE)
		{
			for (int i=0; i < m_aActorImps.GetSize(); i++)
			{
				ACTOR_IMP* pActorImp = m_aActorImps[i];
				pActorImp->pActor->wakeUp();
			}
		}
	}

	return true;
}

//	Find actor through specified bone or hh-type hook
//	matHook (out): store hook or bone's absolute TM
NxActor* A3DModelPhysSync::GetActorByHook(int iHookIndex, bool bHHIsBone, A3DMATRIX4& matHook)
{
	if (!m_bPhysBindOk)
		return NULL;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	int iBoneIdx = -1;

	//	Get absolute TM
	if (bHHIsBone)
	{
		A3DBone* pBone = pSkeleton->GetBone(iHookIndex);
		if (!pBone)
			return NULL;

		iBoneIdx = iHookIndex;
		matHook = pBone->GetNoScaleAbsTM();
	}
	else
	{
		A3DSkeletonHook* pHook = pSkeleton->GetHook(iHookIndex);
		if (!pHook)
			return NULL;

		iBoneIdx = pHook->GetBone();
		matHook	= pHook->GetNoScaleAbsTM();
	}
	
	if (iBoneIdx < 0)
		iBoneIdx = m_pCoreData->m_iRootBone;

	//	Get the actor on which hook / bone is bound
	int i, j;
	int iNumActor = m_pCoreData->GetActorNum();

	for (i=0; i < iNumActor; i++)
	{
		A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(i);
		A3DModelPhysSyncData::LINK_INFO& link = pActorData->Link;

		for (j=0; j < link.aBones.GetSize(); j++)
		{
			A3DModelPhysSyncData::BONE_DATA* pBoneData = link.aBones[j];
			if (iBoneIdx == pBoneData->iBoneIdx)
				return m_aActorImps[i]->pActor;
		}
	}

	//	Couldn't find the actor that specified bone is bound with ! 
	//	This shouldn't happen when m_bPhysBindOk == true
	ASSERT(0);

	return NULL;
}

bool A3DModelPhysSync::OpenPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher)
{
	ASSERT(szParentHook && szChildAttacher);

	if (!BuildAttacherLinkInfos(szParentHook, szChildAttacher))
		return false;

	if (!m_pPhysAttacher)
		return false;

	m_pPhysAttacher->EnableAttacherKinematic(false);
	m_pPhysAttacher->EnableAttacherKinematic(szChildAttacher, true);
	return true;
}

void A3DModelPhysSync::ClosePhysLinkAsChildModel()
{
	ReleaseAttacherLinkInfo();

	if (!m_pPhysAttacher)
		return;

	m_pPhysAttacher->EnableAttacherKinematic(false);
}

//	Check whether the physical link is already exist
bool A3DModelPhysSync::CheckPhysLinkExist(const char* szParentHook, const char* szChildAttacher)
{
	for (int iPhysLinkIdx = 0; iPhysLinkIdx < m_aAttacherLinks.GetSize(); ++iPhysLinkIdx)
	{
		ATTACHER_LINK* pLink = m_aAttacherLinks[iPhysLinkIdx];
		if (!pLink)
			continue;

		//	允许一个Hook对应多个Attacher，但是不能允许一个Attacher对应多个Hook
		if (pLink->strAttacher == szChildAttacher)
			return true;
	}

	return false;
}

//	Add physical link
bool A3DModelPhysSync::AddPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher)
{
	ASSERT(szParentHook && szChildAttacher);

	if (CheckPhysLinkExist(szParentHook, szChildAttacher))
		return false;

	if (!m_pPhysAttacher || !m_pPhysModel || !m_pCoreData || !m_pSkinModel)
		return false;

 	A3DSkinModel* pParentModel = m_pSkinModel->GetParent();
 	if (!pParentModel)
 		return false;

 	A3DSkeleton* pParentSkeleton = pParentModel->GetSkeleton();
 	if (!pParentSkeleton)
 		return false;

	//	Can not find parent hook
	int iHookIdx = -1;
	if (!pParentSkeleton->GetHook(szParentHook, &iHookIdx))
		return false;

	//	Can not find attacher
	int iAttacherIdx = -1;
	if (!m_pCoreData->GetAttacherByName(szChildAttacher, &iAttacherIdx))
		return false;

	ATTACHER_LINK* pAttacherLink = new ATTACHER_LINK;
	pAttacherLink->strParentHook = szParentHook;
	pAttacherLink->strAttacher = szChildAttacher;
	pAttacherLink->nParentHookIdx = iHookIdx;
	pAttacherLink->nAttacherIdx = iAttacherIdx;
	m_aAttacherLinks.Add(pAttacherLink);

	m_pPhysAttacher->EnableAttacherKinematic(szChildAttacher, true);

	return true;
}

//	Get attacher
A3DModelPhysSync::ATTACHER_LINK* A3DModelPhysSync::GetPhysLinkAsChildModel(const char* szChildAttacher, int* pIndex) const
{
	for (int iPhysLinkIdx = 0; iPhysLinkIdx < m_aAttacherLinks.GetSize(); ++iPhysLinkIdx)
	{
		ATTACHER_LINK* pAttacherLink = m_aAttacherLinks[iPhysLinkIdx];
		if (pAttacherLink->strAttacher == szChildAttacher)
		{
			if (pIndex)
				*pIndex = iPhysLinkIdx;

			return pAttacherLink;
		}
	}

	return NULL;
}

//	Remove physical link
bool A3DModelPhysSync::RemovePhysLinkAsChildModel(const char* szChildAttacher)
{
	ASSERT(szChildAttacher);
	
	int iIndex = -1;
	ATTACHER_LINK* pAttacherLink = GetPhysLinkAsChildModel(szChildAttacher, &iIndex);
	if (!pAttacherLink)
		return false;

	m_pPhysAttacher->EnableAttacherKinematic(szChildAttacher, false);
			
	delete pAttacherLink;
	m_aAttacherLinks.RemoveAt(iIndex);
	return false;
}

//	Change physical link
bool A3DModelPhysSync::ChangePhysLinkAsChildModel(const char* szChildAttacher, const char* szNewHook)
{
	int iIndex = -1;

	ATTACHER_LINK* pAttacherLink = GetPhysLinkAsChildModel(szChildAttacher, &iIndex);
	if (!pAttacherLink)
		return false;

	A3DSkinModel* pParentModel = m_pSkinModel->GetParent();
 	if (!pParentModel)
 		return false;

 	A3DSkeleton* pParentSkeleton = pParentModel->GetSkeleton();
 	if (!pParentSkeleton)
 		return false;

	//	Can not find parent hook
	int iHookIdx = -1;
	if (!pParentSkeleton->GetHook(szNewHook, &iHookIdx))
		return false;

	pAttacherLink->strParentHook = szNewHook;
	pAttacherLink->nParentHookIdx = iHookIdx;
	return true;
}

//	Break Joint
void A3DModelPhysSync::BreakJoint(const char* szJoint)
{
	if (!m_pPhysModel)
		return;

	m_pPhysModel->RemoveJoint(szJoint);
}

//	Break Joint by bone name
void A3DModelPhysSync::BreakJointByBone(const char* szBoneName)
{
	if (!m_pPhysModel || !m_pCoreData || !szBoneName || !szBoneName[0])
		return;

	for (int iJointIdx = 0; iJointIdx < m_pCoreData->GetJointNum(); ++iJointIdx)
	{
		A3DModelPhysSyncData::JOINT_DATA* pJoint = m_pCoreData->GetJointByIndex(iJointIdx);
		if (strcmp(pJoint->GetLinkedBoneName(), szBoneName) == 0)
		{
			m_pPhysModel->RemoveJoint(pJoint->GetName());
			return;
		}
	}
}

//	Apply force
bool A3DModelPhysSync::AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist, int iPhysForceType, float fSweepRadius /*= 0.0f*/)
{
	if (!m_pPhysModel)
		return false;

	float fRealForceMagnitude = fForceMagnitude > NX_MAX_F32 ? NX_MAX_F32 : fForceMagnitude;
	
	NxVec3 vDirNor = APhysXConverter::A2N_Vector3(vDir);
	vDirNor.normalize();
	NxRay forceRay(APhysXConverter::A2N_Vector3(vStart), vDirNor);
	NxForceMode nxForceMode = NX_FORCE;
	switch (iPhysForceType)
	{
	case CECModel::PFT_FORCE:
		nxForceMode = NX_FORCE;
		break;
	case CECModel::PFT_IMPULSE:
		nxForceMode = NX_IMPULSE;
		break;
	case CECModel::PFT_VELOCITY_CHANGE:
		nxForceMode = NX_VELOCITY_CHANGE;
		break;
	case CECModel::PFT_SMOOTH_IMPULSE:
		nxForceMode = NX_SMOOTH_IMPULSE;
		break;
	case CECModel::PFT_SMOOTH_VELOCITY_CHANGE:
		nxForceMode = NX_SMOOTH_VELOCITY_CHANGE;
		break;
	case CECModel::PFT_ACCELERATION:
		nxForceMode = NX_ACCELERATION;
		break;
	default:
		ASSERT(FALSE && "Invalid Force Type.");
		break;
	}

	if(fSweepRadius == 0.0f)
		return m_pPhysModel->AddForce(forceRay, fRealForceMagnitude, fMaxDist, nxForceMode);
	else
		return m_pPhysModel->AddSweepForce(forceRay, fRealForceMagnitude, fSweepRadius, fMaxDist, nxForceMode);
}

#endif	//	A3D_PHYSX

