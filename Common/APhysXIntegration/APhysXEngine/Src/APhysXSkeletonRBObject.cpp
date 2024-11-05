
#include "APhysXStdAfx.h"


void APhysXSkeletonRBObject::OnCreateNxActorDesc(NxActorDesc* pNxActorDesc, int iActorID) 
{
	APhysXDynamicRBObject::OnCreateNxActorDesc(pNxActorDesc, iActorID);

	//------------------------------------------------------------------------------
	// we'd better offer the right of adjusting the following parameters to users...

#if 0

	// increase the actor's iteration count since there are joints connected to actors...
	NxBodyDesc* pBodyDesc = (NxBodyDesc*)pNxActorDesc->body;
	pBodyDesc->solverIterationCount = 60;

	// pNxActorDesc->density = 10.0f;

	pBodyDesc->linearDamping = 0.3f;
	pBodyDesc->angularDamping = 0.3f;

#endif

}


