//////////////////////////////////////////////////
// PhysXBenchmark.h: declaration of the function to test a PC's hardware capability to do PhysX simulation
//		
//		Originally created by Nvidia PhysX Group.
//
//		Revised by He Wenfeng, May 6, 2009
//		Revision Keypoints:	
//			1. SW Benchmark support
//			2. use NxCookingInterface instead of NxInitCooking() directly
//			3. fix the bug of original version: omit calling NxCloseCooking() at end
// 
/////////////////////////////////////////////////

#ifndef __PHYSXBENCHMARK_H__
#define __PHYSXBENCHMARK_H__

#include "NxPhysics.h"


//-----------------------------------------------------------------------
// Note by wenfeng, May 12, 2009
//
// there is a bug of MSVC6.0 which fails to compile the function with
// two default arguments of user-defined class type with  the 
// default constructor, so we have to work around the issue...
//
// Refer to the following line:
//		1. ClothParams::ClothParams(bool bInit = true){...}
//
//		2. FluidParams::FluidParams(bool bInit = true){...}
//
//		3. NxReal TestPhysXCapability(	NxU32 secondsToTest, 
//										NxPhysicsSDK* pPhysXSDK,
//										bool bRunOnHardware = true,
//										ClothParams clothParams = ClothParams(true),
//										FluidParams fluidParams = FluidParams(true),
//										NxReal maxTimeStep = 1.0f/60.f, 
//										NxU32 maxIter = 8, 
//										NxTimeStepMethod method = NX_TIMESTEP_FIXED
//									 );
//-----------------------------------------------------------------------


namespace APhysXBenchmark
{


/**
Parameters for the cloths in test scenario.
NxReal	width:				width of cloth
NxReal	height:				height of cloth
NxReal	gridSize:			for simplifying the test, cloths will be created with regular mesh; gridSize specifies the distance between two adjacent cloth vertexes
bool	isSelfCollision:	if cloths are self-collision enabled
*/
class ClothParams
{
public:
	NxReal	width;
	NxReal	height;
	NxReal	gridSize;

	bool	isSelfCollision;

	// added by wenfeng...
	// bool    bRunOnHardware;

	ClothParams(bool bInit = true)
	{
		if(bInit)
		{
			width = 3;
			height = 4;
			gridSize = 0.05f;
			isSelfCollision = true;
		}
		// bRunOnHardware = true;
	}
};


/**
Parameters for the fluids in test scenario.
NxU32	maxParticles:		maximum number of particles of the fluid; must not exceed 65535.
NxU32	emittingRate:		how many particles a fluid emitter can emit per second
NxReal	particleLiftTime:	how long a particle can live in the test; 0 means forever
*/
class FluidParams
{
public:
	NxU32	maxParticles;
	NxReal	emittingRate;
	NxReal	particleLiftTime;

	FluidParams(bool bInit = true)
	{
		if(bInit)
		{

			maxParticles = 10000;
			// emittingRate = 250;
			emittingRate = 1000;
			particleLiftTime = 0;

		}
	}
};

/**
Parameters:
NxU32 secondsToTest: defines the time length, in seconds, to do the test
NxPhysicsSDK* pPhysXSDK: a PhysX SDK object prepared for this test
ClothParams clothParams: cloth parameters for the cloth created in testing
FluidParams fluidParams: fuild parameters for the fluid and its emitters created in testing
NxReal maxTimeStep: max time step of PhysX simulatgion, by default is 1.0f/60.f, i.e. 60 fps
NxU32 maxIter: max iterations of a simulation, by default is 8
NxTimeStepMethod method: time step method of a simulation step, by default is fixed time step

For more about the last three parameters please refer to PhysX documentation.

Return value:
Return 0 if it only can do CPU simulation, no hardware PhysX capability;
otherwise, return the average fps of PhysX simulation during the given secondsToTest.
*/
NxReal TestPhysXCapability(NxU32 secondsToTest, 
							 NxPhysicsSDK* pPhysXSDK,
							 bool bRunOnHardware = true,
							 ClothParams clothParams = ClothParams(true),
							 FluidParams fluidParams = FluidParams(true),
							 NxReal maxTimeStep = 1.0f/60.f, 
							 NxU32 maxIter = 8, 
							 NxTimeStepMethod method = NX_TIMESTEP_FIXED
							 );



};

#endif