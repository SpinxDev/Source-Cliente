/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSX_H_
#define _APHYSX_H_

//-----------------------------------------------------------------------
// all APhysXEngine header files are included here
//
// we use APhysXStdAfx.h and APhysXStdAfx.cpp to create the precompiled 
// header file which will optimize the compiling speed dramatically.
//
// pls. be ware of the order of the following included header files.
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
// prerequisites
#include "APhysXPrerequisites.h"

//-----------------------------------------------------------------------
// interface...
#include "APhysXBreakable.h"


//-----------------------------------------------------------------------
// utility
#include "APhysXSingleton.h"
#include "APhysXMutex.h"
#include "APhysXRefCountable.h"
#include "APhysXAllocator.h"
#include "APhysXConverter.h"
#include "APhysXStringManager.h"
#include "APhysXOutputStream.h"
#include "APhysXStream.h"
#include "APhysXUserGraphicsSync.h"
#include "APhysXPicker.h"
#include "APhysXUtilLib.h"

#include "APhysXUserCallBack.h"

#include "APhysXSerializerBase.h"

#include "APhysXPerfStats.h"
#include "APhysXDebugRenderer.h"

#include "APhysXMeshCooker.h"

#include "APhysXCollisionChannelManager.h"

#include "APhysXGraph.h"

//-----------------------------------------------------------------------
// description

// helper desc
#include "APhysXMeshDesc.h"

// actor and material desc
#include "APhysXActorDesc.h"
#include "APhysXMaterialDesc.h"

// shape desc
#include "APhysXShapeDesc.h"
#include "APhysXShapeDescManager.h"
#include "APhysXBoxShapeDesc.h"
#include "APhysXCapsuleShapeDesc.h"
#include "APhysXConvexShapeDesc.h"
#include "APhysXPlaneShapeDesc.h"
#include "APhysXSphereShapeDesc.h"
#include "APhysXHeightFieldShapeDesc.h"

// connector and joint desc
#include "APhysXObjectConnectorDesc.h"
#include "APhysXJointDesc.h"
#include "APhysXDistanceJointDesc.h"
#include "APhysXD6JointDesc.h"
#include "APhysXPulleyJointDesc.h"
#include "APhysXRevoluteJointDesc.h"
#include "APhysXSphericalJointDesc.h"
#include "APhysXClothAttachRBConnectorDesc.h"
#include "APhysXFluidEmitterAttachRBConnectorDesc.h"

// object desc
#include "APhysXObjectDesc.h"
#include "APhysXObjectDescManager.h"
#include "APhysXRigidBodyObjectDesc.h"
#include "APhysXClothObjectDesc.h"
#include "APhysXCompoundObjectDesc.h"
#include "APhysXAttacherDesc.h"
#include "APhysXRBAttacherDesc.h"
#include "APhysXClothAttacherDesc.h"
#include "APhysXFluidObjectDesc.h"
#include "APhysXFluidEmitterAttacherDesc.h"
#include "APhysXForceFieldObjectDesc.h"

#include "APhysXBreakableRBObjectDesc.h"

//-----------------------------------------------------------------------
// object
#include "APhysXObject.h"
#include "APhysXRigidBodyObject.h"
#include "APhysXDummyRBObject.h"
#include "APhysXStaticRBObject.h"
#include "APhysXDynamicRBObject.h"
#include "APhysXTerrainPatchRBObject.h"
#include "APhysXSkeletonRBObject.h"
#include "APhysXClothObject.h"
#include "APhysXCompoundObject.h"
#include "APhysXAttacher.h"
#include "APhysXRBAttacher.h"
#include "APhysXClothAttacher.h"
#include "APhysXFluidObject.h"
#include "APhysXFluidEmitterAttacher.h"
#include "APhysXForceFieldObject.h"

#include "APhysXBreakableRBObject.h"

// terrain
#include "APhysXTerrainQuadManager.h"
#include "APhysXTerrain.h"
#include "APhysXTerrainBlock.h"
#include "APhysXTerrainPatchRBObject.h"

// connectors and joints
#include "APhysXObjectConnector.h"
#include "APhysXJoint.h"
#include "APhysXClothAttachRBConnector.h"
#include "APhysXFluidEmitterAttachRBConnector.h"

//-----------------------------------------------------------------------
// scene
#include "APhysXScene.h"

//-----------------------------------------------------------------------
// advanced application 

// character controller...
#include "APhysXCharCtrlerDesc.h"
#include "APhysXCharCtrlerManager.h"
#include "APhysXCharCtrlerBase.h"
#include "APhysXCharCtrler.h"
#include "APhysXCharCtrlerNx.h"
#include "APhysXCharCtrlerDynamic.h"
#include "APhysXCharCtrlerLightWeight.h"
#include "APhysXCharCtrlerCallBack.h"

//-----------------------------------------------------------------------
// engine
#include "APhysXEngine.h"


#endif