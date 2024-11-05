
#define NOMINMAX
#include <windows.h>
#include <stdio.h>

#include "PhysXBenchmark.h"
#include "NxCooking.h"

#include "Stream.h"

// whether we fetch the simulated data back to user for rendering or some other use...
// #define FETCH_SIMULATED_DATA

// whether we initialize/close the cooking interface internally...
// #define MANAGE_INTERNAL_COOKING

namespace APhysXBenchmark
{

// fluid buffers
static NxVec3* gFluidParticleWriteData = NULL;
static NxU32* gFluidParticleNumber = NULL;

// cloth buffers
static NxMeshData gClothBuffer;

static NxClothMesh* gClothMesh = NULL;

NxActor* CreateGroundPlane(NxScene* pScene, NxReal d = 0);
NxCloth* CreateCurtain(NxScene* pScene, NxVec3 pos, NxClothDesc clothDesc, NxReal w, NxReal h, NxReal d);
NxFluid* CreateFluid(NxScene* pScene, NxVec3 pos, FluidParams fluidParams, bool bRunOnHardware = true);
void SetupEmitter(NxScene* pScene, NxVec3 pos);


NxScene* CreateTestScene(NxPhysicsSDK* pPhysXSDK,
						bool bRunOnHardware,			/* added by wenfeng, for SW PhysX benchmark */
						ClothParams clothParams,
						FluidParams fluidParams,
						NxReal maxTimeStep = 1.0f/60.f, 
						NxU32 maxIter = 8, 
						NxTimeStepMethod method = NX_TIMESTEP_FIXED)
{
	// Create the HW test scene
	NxSceneDesc sceneDesc;
	sceneDesc.maxTimestep = maxTimeStep;
	sceneDesc.maxIter = maxIter;
	sceneDesc.timeStepMethod = method;
	sceneDesc.gravity = NxVec3(0, -9.8f, 0);
	
	if(bRunOnHardware)
		sceneDesc.simType = NX_SIMULATION_HW;
	else
		sceneDesc.simType = NX_SIMULATION_SW;


	NxScene * pScene = pPhysXSDK->createScene(sceneDesc);

	if(!pScene)
		return NULL;

	// create a test scenario containing fluids and cloths interacting with each other
	CreateGroundPlane(pScene);
	NxClothDesc clothDesc;
	clothDesc.flags |= NX_CLF_VISUALIZATION | NX_CLF_FLUID_COLLISION;
	if(clothParams.isSelfCollision)
		clothDesc.flags |= NX_CLF_SELFCOLLISION;

	if(bRunOnHardware)
		clothDesc.flags |= NX_CLF_HARDWARE;
	else
		clothDesc.flags &= ~NX_CLF_HARDWARE;

	clothDesc.toFluidResponseCoefficient = 0.3f;
	clothDesc.fromFluidResponseCoefficient = 0.3f;
	clothDesc.thickness = 0.1f;
	
	NxCloth* pCloth = CreateCurtain(pScene, NxVec3(0, 0, 0), clothDesc, clothParams.width, clothParams.height, clothParams.gridSize);

	NxFluid* pFluid = CreateFluid(pScene, NxVec3(1, 2, -1), fluidParams, bRunOnHardware);

	if(!pCloth || !pFluid)
	{// if either cloth of fluid creation failed, we need to quit
		pPhysXSDK->releaseScene(*pScene);
		pScene = NULL;
	}

	return pScene;
}

void ReleaseScene(NxScene* pScene)
{
	NxPhysicsSDK& physXSDK = pScene->getPhysicsSDK();
	physXSDK.releaseScene(*pScene);

	if(gClothMesh) physXSDK.releaseClothMesh(*gClothMesh);

	if(gFluidParticleWriteData)
		delete [] gFluidParticleWriteData;
	if(gFluidParticleNumber)
		delete gFluidParticleNumber;

	if(gClothBuffer.numVerticesPtr && gClothBuffer.numIndicesPtr)
	{
		delete gClothBuffer.numVerticesPtr;
		delete [] gClothBuffer.verticesPosBegin;
		delete [] gClothBuffer.verticesNormalBegin;
		delete [] gClothBuffer.indicesBegin;
		delete gClothBuffer.numIndicesPtr;
	}

#ifdef MANAGE_INTERNAL_COOKING	

	// do not forget to close internal cooking...
	NxGetCookingLib(NX_PHYSICS_SDK_VERSION)->NxCloseCooking();

#endif

	return;
}

NxReal TestPhysXCapability(NxU32 secondsToTest, NxPhysicsSDK* pPhysXSDK, bool bRunOnHardware, 
							 ClothParams clothParams, FluidParams fluidParams,
							 NxReal maxTimeStep,NxU32 maxIter, NxTimeStepMethod method)
{
	// create a HW PhysX scene for the test
	NxScene* pScene = CreateTestScene(pPhysXSDK, bRunOnHardware, clothParams, fluidParams, maxTimeStep, maxIter, method);
	if(!pScene)
		return 0;

	// now HW scene is ready, save beginning time and go
	LARGE_INTEGER liFreq;
	QueryPerformanceFrequency(&liFreq);
	NxU64 ticksToMicroSeconds = liFreq.QuadPart/(1000*1000);
	
	LARGE_INTEGER liBegin;
	BOOL queryCounter = QueryPerformanceCounter(&liBegin);
	if(FALSE == queryCounter)
	{// what kind of CPU can't support this nowadays? It should be very old...
		return 0;
	}
	
	NxI64 milliSecondsToTest = secondsToTest * 1000;
	NxI64 milliSecondsTested = 0;
	NxU32 frames = 0;
	NxReal numberParticles = 0; // total number of cloth and fluid particles

	// do the test
	while(milliSecondsTested < milliSecondsToTest)
	{
		pScene->simulate(maxTimeStep);
		pScene->fetchResults(NX_ALL_FINISHED, true);

		LARGE_INTEGER liNow;
		QueryPerformanceCounter(&liNow);
		milliSecondsTested = (liNow.QuadPart - liBegin.QuadPart) / (ticksToMicroSeconds * 1000);
		frames++;

		if(gFluidParticleNumber && gClothBuffer.numVerticesPtr)
			numberParticles += *gFluidParticleNumber + *gClothBuffer.numVerticesPtr;
	}

	// now done, release test scene and return the fps got
	ReleaseScene(pScene);
	NxReal avgNumberParticles = numberParticles / frames;
	return frames * 1000.0f / milliSecondsTested;
}


NxActor* CreateGroundPlane(NxScene* pScene, NxReal d)
{
    // Create a plane with default descriptor
    NxPlaneShapeDesc planeDesc;
	planeDesc.d = d;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&planeDesc);
    return pScene->createActor(actorDesc);
}


NxActor* CreateBox(NxScene* pScene, const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&boxDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return pScene->createActor(actorDesc);	
}

const int TEAR_MEMORY_FACTOR = 2;
void GenerateRegularMeshDesc(NxClothMeshDesc &desc, NxReal w, NxReal h, NxReal d)
{
	int numX = (int)(w / d) + 1;
	int numY = (int)(h / d) + 1;

	desc.numVertices				= (numX+1) * (numY+1);
	desc.numTriangles				= numX*numY*2;
	desc.pointStrideBytes			= sizeof(NxVec3);
	desc.triangleStrideBytes		= 3*sizeof(NxU32);
	desc.vertexMassStrideBytes		= sizeof(NxReal);
	desc.vertexFlagStrideBytes		= sizeof(NxU32);
	desc.points						= (NxVec3*)malloc(sizeof(NxVec3)*desc.numVertices);
	desc.triangles					= (NxU32*)malloc(sizeof(NxU32)*desc.numTriangles*3);
	desc.vertexMasses				= 0;
	desc.vertexFlags				= 0;
	desc.flags						= 0;

	int i,j;
	NxVec3 *p = (NxVec3*)desc.points;
	for (i = 0; i <= numY; i++) {
		for (j = 0; j <= numX; j++) {
			p->set(d*j, 0.0f, d*i); 
			p++;
		}
	}

	NxU32 *id = (NxU32*)desc.triangles;
	for (i = 0; i < numY; i++) {
		for (j = 0; j < numX; j++) {
			NxU32 i0 = i * (numX+1) + j;
			NxU32 i1 = i0 + 1;
			NxU32 i2 = i0 + (numX+1);
			NxU32 i3 = i2 + 1;
			if ((j+i)%2) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}
}

void ReleaseMeshDescBuffers(const NxClothMeshDesc& desc)
{
	NxVec3* p = (NxVec3*)desc.points;
	NxU32* t = (NxU32*)desc.triangles;
	
	free(p);
	free(t);
}

NxClothMesh* CookClothMesh(NxPhysicsSDK& pPhysXSDK, NxClothMeshDesc& desc)
{
	// we cook the mesh on the fly through a memory stream
	MemoryWriteBuffer wb;
	assert(desc.isValid());

#if 0

	if(!NxInitCooking())
		return NULL;

	if(!NxCookClothMesh(desc, wb))
		return NULL;

	if(!NxGetCookingLib(NX_PHYSICS_SDK_VERSION)->NxInitCooking())
		return NULL;

	if(!NxGetCookingLib(NX_PHYSICS_SDK_VERSION)->NxCookClothMesh(desc, wb))
		return NULL;

#endif

	//--------------------------------------------------------------------------
	//??? Note by wenfeng, May 7, 2009
	// why it does not work? we have to use the cooking interface's virtual function
	// other than NxCookClothMesh() directly... but why? 
	// in fact, this will make the application statically linked to NxCooking.dll but
	// not PhysXCooking.dll via PhysXLoader.dll's dynamic loading. Obviously, they are
	// two different dlls and maybe incur some inconsistency, even crash.

	// if(!NxCookClothMesh(desc, wb))
	// 	return NULL;

#ifdef MANAGE_INTERNAL_COOKING	
	
	if(!NxGetCookingLib(NX_PHYSICS_SDK_VERSION)->NxInitCooking())
		return NULL;

#endif

	if(!NxGetCookingLib(NX_PHYSICS_SDK_VERSION)->NxCookClothMesh(desc, wb))
		return NULL;

	//--------------------------------------------------------------------------


	MemoryReadBuffer rb(wb.data);
	NxClothMesh* pClothMesh = pPhysXSDK.createClothMesh(rb);
	return pClothMesh;
}

NxCloth* CreateCloth(NxScene* pScene, NxClothDesc clothDesc, NxReal w, NxReal h, NxReal d)
{
	NxClothMeshDesc meshDesc;
	GenerateRegularMeshDesc(meshDesc, w, h, d);
	
	NxPhysicsSDK& physXSDK = pScene->getPhysicsSDK();
	NxClothMesh* pMesh = CookClothMesh(physXSDK, meshDesc);

	ReleaseMeshDescBuffers(meshDesc);

	if(!pMesh)
		return NULL;

	gClothMesh = pMesh;

	clothDesc.clothMesh = pMesh;

#ifdef	FETCH_SIMULATED_DATA

	gClothBuffer.numVerticesPtr = new NxU32;
	gClothBuffer.verticesPosBegin = new NxVec3[meshDesc.numVertices];
	gClothBuffer.verticesPosByteStride = sizeof(NxVec3);
	gClothBuffer.verticesNormalBegin = new NxVec3[meshDesc.numVertices];
	gClothBuffer.verticesNormalByteStride = sizeof(NxVec3);
	gClothBuffer.indicesBegin = new NxU32[meshDesc.numTriangles * 3];
	gClothBuffer.numIndicesPtr = new NxU32;
	gClothBuffer.indicesByteStride = sizeof(NxU32);
	gClothBuffer.maxIndices = meshDesc.numTriangles * 3;
	gClothBuffer.maxVertices = meshDesc.numVertices;

#endif

	clothDesc.meshData = gClothBuffer;
	
	return pScene->createCloth(clothDesc);
}

NxCloth* CreateCurtain(NxScene* pScene, NxVec3 pos, NxClothDesc clothDesc, NxReal w, NxReal h, NxReal d)
{
	clothDesc.globalPose.t = pos + NxVec3(0, h, 0);
	NxCloth* pCloth = CreateCloth(pScene, clothDesc, w, h, d);
	if(!pCloth)
		return NULL;

	// Create the curtain stick
	NxActor* pStick = CreateBox(pScene, NxVec3(pos.x + 0.5f * w, pos.y + h, pos.z), NxVec3(0.5f * w, 0.1f, 0.1f), 0);

	// Create the curtain
	pCloth->attachToShape(pStick->getShapes()[0], 0);
	return pCloth;
}


NxFluid* CreateFluid(NxScene* pScene, NxVec3 pos, FluidParams fluidParams, bool bRunOnHardware)
{
	NxFluidDesc fluidDesc;

	fluidDesc.simulationMethod = NX_F_SPH;
	fluidDesc.kernelRadiusMultiplier = 2.3f;
	fluidDesc.restParticlesPerMeter = 10;
	fluidDesc.motionLimitMultiplier = 100.0f;
	fluidDesc.packetSizeMultiplier = 64;
	fluidDesc.restDensity = 1000;
	fluidDesc.viscosity = 1;
	fluidDesc.stiffness = 1000;
	fluidDesc.damping = 1;
	fluidDesc.collisionMethod = NX_F_DYNAMIC | NX_F_STATIC;

	fluidDesc.maxParticles = fluidParams.maxParticles; 

#ifdef	FETCH_SIMULATED_DATA

	gFluidParticleWriteData = new NxVec3[fluidDesc.maxParticles];
	fluidDesc.particlesWriteData.bufferPos = &gFluidParticleWriteData[0].x;
	fluidDesc.particlesWriteData.bufferPosByteStride = sizeof(NxVec3);
	gFluidParticleNumber = new NxU32;
	fluidDesc.particlesWriteData.numParticlesPtr = gFluidParticleNumber;

#endif

	fluidDesc.flags |= NX_FF_VISUALIZATION;

	fluidDesc.collisionDistanceMultiplier = 0.4f;
	
	if(bRunOnHardware)
		fluidDesc.flags |= NX_FF_HARDWARE;	// by default, fluids needs to run in HW
	else
		fluidDesc.flags &= ~NX_FF_HARDWARE;
	
	NxFluid* pFluid = pScene->createFluid(fluidDesc);
	if(!pFluid)
		return NULL;
	
	NxFluidEmitterDesc emitterDesc;
	emitterDesc.dimensionX = 0.3f;
	emitterDesc.dimensionY = 0.1f;
	emitterDesc.shape = NX_FE_RECTANGULAR;
	emitterDesc.relPose.t = pos;
	emitterDesc.type = NX_FE_CONSTANT_FLOW_RATE;
	emitterDesc.rate = fluidParams.emittingRate; 
	emitterDesc.maxParticles = fluidParams.maxParticles; 
	emitterDesc.particleLifetime = fluidParams.particleLiftTime;
	emitterDesc.fluidVelocityMagnitude = 4.0;
	NxFluidEmitter* pFluidEmitter = pFluid->createEmitter(emitterDesc);

	NxVec3 pos2(pos.x, pos.y, pos.z * -1);
	NxFluidEmitterDesc emitterDesc2;
	emitterDesc2.dimensionX = 0.3f;
	emitterDesc2.dimensionY = 0.1f;
	emitterDesc2.shape = NX_FE_RECTANGULAR;
	emitterDesc2.relPose.t = pos2;
	emitterDesc2.relPose.M.rotY(NxPi);
	emitterDesc2.type = NX_FE_CONSTANT_FLOW_RATE;
	emitterDesc2.rate = fluidParams.emittingRate; 
	emitterDesc2.maxParticles = fluidParams.maxParticles; 
	emitterDesc2.particleLifetime = fluidParams.particleLiftTime;
	emitterDesc2.fluidVelocityMagnitude = 4.0;
	NxFluidEmitter* pFluidEmitter2 = pFluid->createEmitter(emitterDesc2);

	return pFluid;
}

};