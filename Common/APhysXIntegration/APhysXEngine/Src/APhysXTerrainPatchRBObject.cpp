
#include "APhysXStdAfx.h"


void APhysXTerrainPatchRBObject::OnCreateNxActorDesc(NxActorDesc* pNxActorDesc, int iActorID)
{
	if(iActorID == 0)
	{

		NxHeightFieldShapeDesc* pHFShapeDesc = (NxHeightFieldShapeDesc*)pNxActorDesc->shapes[0];
		m_pNxHeightField = pHFShapeDesc->heightField;

		// turn on/off the visualization of terrin...
		if(m_pAPhysXScene->IsTerrainDebugRenderEnabled())
			pHFShapeDesc->shapeFlags |= NX_SF_VISUALIZATION;

		// modify the group to APX_COLLISION_GROUP_TERRAIN_RB
		pHFShapeDesc->group = APX_COLLISION_GROUP_TERRAIN_RB;
	}

}


void APhysXTerrainPatchRBObject::OnRelease()
{
	APhysXStaticRBObject::OnRelease();

	// not only release the actors but also try to release the HeightField created by NxPhysicsSDK
	if(m_pNxHeightField)
	{
		gPhysXEngine->GetNxPhysicsSDK()->releaseHeightField(*m_pNxHeightField);
		m_pNxHeightField = NULL;
	}

}