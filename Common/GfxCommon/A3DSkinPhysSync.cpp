/*
 * FILE: A3DSkinPhysSync.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/17
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#ifdef A3D_PHYSX

#include "A3DSkinPhysSync.h"
#include "A3DSkinPhysSyncData.h"

#include "APhysX.h"

#include <ALog.h>
#include <A3DSkinModel.h>
#include <A3DSkin.h>
#include <A3DSkeleton.h>
#include <A3DBone.h>
#include <A3DClothMesh.h>

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
//	Implement A3DSkinPhysSync
//	
///////////////////////////////////////////////////////////////////////////

A3DSkinPhysSync::A3DSkinPhysSync()
{
	m_pCoreData			= NULL;
	m_pPhysScene		= NULL;
	m_pPhysSkin			= NULL;
	m_pScaleClothDesc	= NULL;
	m_pA3DSkin			= NULL;
}

A3DSkinPhysSync::~A3DSkinPhysSync()
{
	//	Release physical skin
	Unbind(true);
}

//	Bind physical data with specified skin
bool A3DSkinPhysSync::Bind(APhysXScene* pPhysScene, A3DSkinModel* pSkinModel, A3DSkin* pSkin, const A3DVECTOR3& vRefPos)
{
	if (!pPhysScene || !pSkinModel || !pSkin)
		return false;

	//	Release old physical cloth
	Unbind(false);
	//	Clear old link info
	m_aRBActorLinks.RemoveAll();

	//	Create core data if it doesn't exist
	if (!m_pCoreData)
	{
		//	Make sync data file name
		AString strSyncFile = pSkin->GetFileName();
		af_ChangeFileExt(strSyncFile, ".sphy");

		//	Create sync data object
		m_pCoreData = A3DSkinPhysSyncDataMan::GetGlobalMan().LoadSyncData(strSyncFile);
		if (!m_pCoreData || !m_pCoreData->GetBindInitFlag())
			return false;
	}

	m_pPhysScene = pPhysScene;

	//	Check if skin can be bound with this physical data
	if (!m_pCoreData->SkinBindCheck(pSkin))
	{
		a_LogOutput(1, "A3DSkinPhysSync::Bind, failed to call SkinBindCheck() for skin %s", pSkin->GetFileName());
		return false;
	}

	//	Create RB actor lnik info
	int i, iNumActor = m_pCoreData->GetActorNum();
	m_aRBActorLinks.SetSize(iNumActor, 4);

	for (i=0; i < iNumActor; i++)
	{
		A3DSkinPhysSyncData::ACTOR_DATA* pActor = m_pCoreData->GetActorByIndex(i);
		RBACTOR_LINK& link = m_aRBActorLinks[i];
		link.bHHIsBone = pActor->bHHIsBone;		

		if (pActor->bHHIsBone)
		{
			if (!pSkinModel->GetSkeleton()->GetBone(pActor->strHHHook, &link.iHookHHIdx))
			{
				a_LogOutput(1, "A3DSkinPhysSync::Bind, failed to find bone %s", pActor->strHHHook);
				return false;
			}
		}
		else
		{
			if (!pSkinModel->GetSkeleton()->GetHook(pActor->strHHHook, &link.iHookHHIdx))
			{
				a_LogOutput(1, "A3DSkinPhysSync::Bind, failed to find HH hook %s", pActor->strHHHook);
				return false;
			}
		}
	}

	bool bMeshScaled = CheckMeshScaled(pSkinModel);
	APhysXClothAttacherDesc* pAttacherDesc = NULL;

	if (bMeshScaled)
	{
		if (!m_pScaleClothDesc)
		{
			m_pScaleClothDesc = new APhysXClothAttacherDesc;
			m_pCoreData->CreateScaledSkinPhysDesc(pSkinModel, pSkin, m_pPhysScene, m_pScaleClothDesc);
		}
		
		pAttacherDesc = m_pScaleClothDesc;
	}
	else
		pAttacherDesc = m_pCoreData->GetSkinPhysDesc();

	//	Create physical object
	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = pAttacherDesc;
//	apxObjInstanceDesc.mGlobalPose.id();
	apxObjInstanceDesc.mScale3D = NxVec3(1.0f);


	//-------------------------------------------------------------------
	// revised by wenfeng, Dec. 23, 2008
	// we should consider the orientation as well as the position when 
	// creating the cloth object... and so, here we just ignore the 
	// vRefPos passed in...

	A3DMATRIX4 mat = pSkinModel->GetAbsoluteTM();
	// A3DMATRIX4 mat = a3d_Translate(vRefPos.x, vRefPos.y, vRefPos.z);
	//-------------------------------------------------------------------

	
	APhysXConverter::A2N_Matrix44(mat, apxObjInstanceDesc.mGlobalPose);

	m_pPhysSkin = (APhysXClothAttacher*)pPhysScene->CreatePhysXObject(apxObjInstanceDesc);
	if (!m_pPhysSkin)
	{
		if (bMeshScaled)
		{
			delete pAttacherDesc;
			pAttacherDesc = NULL;
		}

		a_LogOutput(1, "A3DSkinPhysSync::Bind, failed to create APhysXClothAttacher for skin %s", pSkin->GetFileName());
		return false;
	}

	//	Record bound skin
	m_pA3DSkin = pSkin;

	return true;
}

//	Un-bind physical data
void A3DSkinPhysSync::Unbind(bool bDelCoreData/* false */)
{
	//	Note: Physical system require use release m_pPhysSkin before m_pCoreData
	if (m_pPhysSkin && m_pPhysScene)
	{
		m_pPhysScene->ReleasePhysXObject(m_pPhysSkin);
		m_pPhysSkin = NULL;
	}

	if (bDelCoreData)
	{
		if (m_pScaleClothDesc)
		{
			delete m_pScaleClothDesc;
			m_pScaleClothDesc = NULL;
		}

		if (m_pCoreData)
		{
			A3DSkinPhysSyncDataMan::GetGlobalMan().ReleaseSyncData(m_pCoreData);
			m_pCoreData = NULL;
		}
	}
}

//	Check if mesh needs to be scaled before it changes to cloth
bool A3DSkinPhysSync::CheckMeshScaled(A3DSkinModel* pSkinModel)
{
	A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();

	int i, j;
	int iNumMesh = m_pCoreData->GetMeshNum();
	
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinPhysSyncData::MESH_DATA* pMeshData = m_pCoreData->GetMeshByIndex(i);
		AArray<int>& aBlendBones = pMeshData->GetBlendBones();

		for (j=0; j < aBlendBones.GetSize(); j++)
		{
			A3DBone* pBone = pSkeleton->GetBone(aBlendBones[j]);
			if (pBone->GetAccuWholeScale() != 1.0f)// ||
			//	pBone->GetLocalLengthScale() != 1.0f)
				return true;

			//	Child's thick and length change will also effect parent bone's
			//	local scale matrix. 
			int m, iNumChild = pBone->GetChildNum();
			if (iNumChild)
			{
				for (m=0; m < iNumChild; m++)
				{
					A3DBone* pChildBone = pBone->GetChildPtr(m);
					if (pChildBone->GetLocalThickScale() != 1.0f ||
						pChildBone->GetLocalLengthScale() != 1.0f)
						return true;
				}
			}
			else if (pBone->GetLocalThickScale() != 1.0f)
			{
				//	If bone has child, it's thickness only effect it's parent bone, 
				//	but if bone hasn't child, we will have to consider it's thick factor.
				return true;		
			}
		}
	}

	return false;
}

//	Update pose of RB actors using soft-link. Call this before physics simulation
bool A3DSkinPhysSync::UpdateRBActorPose(A3DSkinModel* pSkinModel)
{
	if (!pSkinModel || !IsSkinBound())
		return false;

	int i;
	A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();
	A3DMATRIX4 matHH;
	NxMat34 matNx;

	for (i=0; i < m_aRBActorLinks.GetSize(); i++)
	{
		const RBACTOR_LINK& link = m_aRBActorLinks[i];

		//	Get absolute TM of hh-type hook
		if (link.bHHIsBone)
		{
			A3DBone* pBone = pSkeleton->GetBone(link.iHookHHIdx);
			matHH = pBone->GetNoScaleAbsTM();
		}
		else
		{
			A3DSkeletonHook* pHook = pSkeleton->GetHook(link.iHookHHIdx);
			matHH = pHook->GetNoScaleAbsTM();
		}

		//	Try to get RB actor
		A3DSkinPhysSyncData::ACTOR_DATA* pActor = m_pCoreData->GetActorByIndex(i);
		if (!pActor)
		{
			//	This shouldn't happen
			ASSERT(pActor);
			continue;
		}

		APhysXConverter::A2N_Matrix44(matHH, matNx);
		m_pPhysSkin->SetAttacherPose(pActor->strName, matNx);
	}

	return true;
}

bool A3DSkinPhysSync::UpdateSkinByAnimation(A3DSkinModel* pSkinModel)
{
#if APHYSX_CURRENT_PHYSX_SDK_VERSION >= APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4)

	// do the skin by animation...
	if(m_pCoreData)
	{
		// firstly, we build the bone's matrices for CPU skinning...
		
		A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();
		int iBoneNum = pSkeleton->GetBoneNum();
		A3DMATRIX4* aMats = new A3DMATRIX4[iBoneNum];
		for(int i=0; i<iBoneNum; i++)
		{
			A3DBone* pBone = pSkeleton->GetBone(i);
			aMats[i] = pBone->GetBoneInitTM() * pBone->GetAbsoluteTM();
		}


		// then, we do the skinning

		int iClothNum = m_pCoreData->m_aSkinMeshInfo.GetSize();
		for(int i=0; i<iClothNum; i++)
		{
			A3DSkinMesh* pSkinMesh = m_pA3DSkin->GetSkinMesh(m_pCoreData->m_aSkinMeshInfo[i]->iIndex);
			APhysXClothObject* pClothObject = m_pPhysSkin->GetClothObject(m_pCoreData->m_aSkinMeshInfo[i]->strName);
			if(pClothObject && pClothObject->IsConstraintsEnabled())
			{
				int iVertNum = pSkinMesh->GetVertexNum();
				A3DVECTOR3* Verts = new A3DVECTOR3[iVertNum];
				A3DVECTOR3* Normals = new A3DVECTOR3[iVertNum];

				pSkinMesh->GetBlendedVertices(m_pA3DSkin, aMats, Verts, Normals);
				pClothObject->SetConstrainPositions((NxVec3* )Verts);
				//--------------------------------------
				// Currently, do not use constraint normals because in Angelica2 cloth meshes are always two-sided.
				// pClothObject->SetConstrainNormals((NxVec3* )Normals);

				delete [] Verts;
				delete [] Normals;
			}
		}


		delete [] aMats;

		return true;
	}
	else
		return true;

#else
	// for case of 
	return false;

#endif

}

//	Sync skin mesh from physical data. Call this after physics simulation
bool A3DSkinPhysSync::SyncSkinPhys(A3DSkin* pSkin)
{
	if (!pSkin || !IsSkinBound())
		return false;

	//	Check if skin has been bound with physical data
	if (m_pA3DSkin != pSkin)
	{
		ASSERT(m_pA3DSkin == pSkin);
		return false;
	}

	if (!pSkin->GetClothMeshNum() || !pSkin->IsClothesEnable())
		return true;

	//	Check mesh number, ASSERT is enough because m_pA3DSkin == pSkin means 
	//	that this skin has been bound with m_pCoreData exactly
	ASSERT(pSkin->GetSkinMeshNum() >= m_pCoreData->m_aSkinMeshInfo.GetSize());
	ASSERT(pSkin->GetRigidMeshNum() >= m_pCoreData->m_aRigidMeshInfo.GetSize());
	ASSERT(pSkin->GetClothMeshNum() == m_pCoreData->GetMeshNum());

	int i, iNumMesh = pSkin->GetClothMeshNum();

	for (i=0; i < iNumMesh; i++)
	{
		A3DClothMeshImp* pClothMesh = pSkin->GetClothMeshImp(i);
		APhysXClothObject* pClothPhys = m_pPhysSkin->GetClothObject(pClothMesh->GetName());
		if (!pClothPhys)
			continue;
		
		pClothMesh->UpdateMeshData(
			pClothPhys->GetVertexNum(), 
			pClothPhys->GetIndexNum(),
			(const A3DVECTOR3*)pClothPhys->GetVertexPositions(), 
			(const A3DVECTOR3*)pClothPhys->GetVertexNormals(), 
			(const int*)pClothPhys->GetIndices(), 
			pClothPhys->GetParentIndexNum(), 
			(const int*)pClothPhys->GetParentIndices());
	}

	return true;
}

//	Get specified RB actor
//	matActor (out): store actor's transform matrix
APhysXRigidBodyObject* A3DSkinPhysSync::GetRBActor(int iActorIdx)
{
	if (!IsSkinBound())
		return NULL;

	A3DSkinPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(iActorIdx);
	if (pActorData)
		return m_pPhysSkin->GetAttacherObject(pActorData->strName);
	else
		return NULL;
}


void A3DSkinPhysSync::SetClothWindAcceleration(const A3DVECTOR3& vWindAcceleration)
{
	if(m_pA3DSkin && m_pPhysSkin )
	{
		int i, iNumMesh = m_pA3DSkin->GetClothMeshNum();

		for (i=0; i < iNumMesh; i++)
		{
			A3DClothMeshImp* pClothMesh = m_pA3DSkin->GetClothMeshImp(i);
			APhysXClothObject* pClothPhys = m_pPhysSkin->GetClothObject(pClothMesh->GetName());
			if(pClothPhys) pClothPhys->SetWindAcceleration(APhysXConverter::A2N_Vector3(vWindAcceleration));
		}

	}
}

#endif	//	A3D_PHYSX

