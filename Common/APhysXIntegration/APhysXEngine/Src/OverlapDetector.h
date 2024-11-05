/*
* FILE: OverlapDetector.h
*
* DESCRIPTION: 
*	This class is used for the detection of actors overlap and sweep.
*	Support modes: 1. checks overlap or not between two actors
*                  2. performs a linear sweep through space with the actor 
*                  3. inflates the actor in 6 directions and then checks overlap
*
*   Caveats/Limitations:
*         1. Any calling moveGlobalPosition() before detection function in the same frame will
*			 have no effect on the position of actor. Due to getGlobalPosition() will not be 
*            updated until a simulation step is taken.
*
*		  2. Any trigger shapes will not contribute to an actor's detection
*
*         3. The OverlapDetector instance must be destroyed before release NxPhysicsSDK if 
*            you have invoked any interfaces of OverlapDetector. 
*
*   About the output parameter "COverlapDetailInfo":
*         1. This is an optional parameter. By default, all of detection function are not generate
*			 any detailed information.
*
*         2. Currently, only contact normal was captured. The normal information were accumulated
*            by "other" shape throughout all of contact events. (In each event, all of "PatchNormal"
*            were accumulated.) That means every hit shape has a corresponding normal. And the normal
*            are normalized well.
*         
*         3. For overlap function, the hit shape normal are accumulated again. Then, it's also 
*            normalized and output as the detail information of detected actor.
*
*         4. The direction of normal is from hit shape/actor to main actor. 
*
* CREATED BY: Yang Liu, 2009/06/19
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _OVERLAP_DETECTOR_H_
#define _OVERLAP_DETECTOR_H_

#include <NxVec3.h>
#include <NxArray.h>
#include "..\..\NxuStream2\Include\NXU_helper.h"
#include <NxUserContactReport.h> 

class NxScene;
class NxActor;
class NxShape;
namespace APhysXUtility
{

struct COverlapDetailInfo
{
	COverlapDetailInfo() { Zero(); }
	void Zero() 
	{
		avgNormal.zero();
	}

	COverlapDetailInfo& operator +=(const COverlapDetailInfo& v)
	{
		avgNormal += v.avgNormal;
		return *this;
	}

	// the direction is toward actor0(main actor)
	NxVec3 avgNormal;
};

class COverlapDetector
{
public:
	static const float gEpsilon; 

	enum ErrorCode
	{
		E_SUCCESS				 = 0,
		E_DIFFERENT_SCENE_ACTORS = 1,
		E_CREATE_SCENE_FAIL		 = 2,
		E_MAP_ACTORS_FAIL		 = 3,
		E_HAS_PLANE_SHAPE		 = 4,
		E_HAS_TRIANGLE_SHAPE	 = 5,
		E_HAS_HEIGHTFIELD_SHAPE	 = 6,
	};

	COverlapDetector();
	~COverlapDetector();

public:
	/*
	checks overlap or not between two actors.

	Parameters:
		[in]  actor0	The main actor  
		[in]  actor1	The target actor
		[out] state		Return the error code value
		[out] pOutInfo	Optional. Output more details if the actors are overlapping

	Return Value:
		If the function succeeds, and the actors are overlapping, the return value is true. Or the return value is false. 
		If the function fails, the return value is false. To get more error information, see the 'state' parameter.

		Return true, if two actors just touch each other.

	Remarks:
		You should input two actors that are in the same scene. 
		If the main actor contains Plane shape, Triangle shape or HeightField shape, the function fails.
	*/
	bool CheckOverlap(const NxActor& actor0, const NxActor& actor1, ErrorCode& state, COverlapDetailInfo* pOutInfo = 0);

	/*
	inflates the actor in 6 directions and then checks overlap or not with other actors.

	Parameters:
		[in]		actor/actor0	The main actor  
		[in]		actor1			The target actor
		[in][out]	growSize		Input the desired inflation size. Output the actual inflation size. (if growSize <= 0, growSize = 0)
		[out]		HitShapes		Store all reported shapes
		[out]		state			Return the error code value
		[in]		pRotation		Optional. The rotation matrix
		[out]		pOutInfo		Optional. Output more details if the actors are overlapping.
									For the array version, each hit shape has a corresponding detail info by index.

	Return Value:
		If the function succeeds, and the actors are overlapping any shape, the return value is true. Or the return value is false. 
		If the function fails, the return value is false. To get more error information, see the 'state' parameter.

	Remarks:
		Actually, the inflation action was simulated by sweeping in 6 directions(+/-X, +/-Y, +/-Z). The default coordinate system is 
		the world frame. However, you can input a matrix to rotate the frame.

		If the main actor contains Plane shape, Triangle shape or HeightField shape, the function fails.
	*/
	bool Inflate6DCheckOverlap(const NxActor& actor0, const NxActor& actor1, float& growSize, ErrorCode& state, const NxMat33* pRotation = 0, COverlapDetailInfo* pOutInfo = 0);
	bool Inflate6DCheckOverlap(const NxActor& actor, float& growSize, NxArray<NxShape*>& HitShapes, ErrorCode& state, const NxMat33* pRotation = 0, NxArray<COverlapDetailInfo>* pOutInfo = 0);

	/*
	performs a linear sweep through space with the actor.

	Parameters:
		[in]  actor		The main actor  
		[in]  motion	Length and direction of the sweep
		[in]  dir		Direction of the sweep
		[in]  distance	Length of the sweep
		[out] HitShapes	Store all reported shapes
		[out] state		Return the error code value
		[out] pOutInfo	Optional. Output more details if any shape is swept. Each hit shape has a corresponding detail info by index.

	Return Value:
		If the function succeeds, and any shape was swept, the return value is true. Or the return value is false. 
		If the function fails, the return value is false. To get more error information, see the 'state' parameter.

	Remarks:
		If the main actor contains Plane shape, Triangle shape or HeightField shape, the function fails.
	*/
	bool LinearSweep(const NxActor& actor, const NxVec3& motion, NxArray<NxShape*>& HitShapes, ErrorCode& state, NxArray<COverlapDetailInfo>* pOutInfo = 0);
	bool LinearSweep(const NxActor& actor, const NxVec3& dir, const float distance, NxArray<NxShape*>& HitShapes, ErrorCode& state, NxArray<COverlapDetailInfo>* pOutInfo = 0);

	/*
	mirrors all actors of the source scene .

	Parameters:
		[in]  scene		The source scene  
		[out] state		Return the error code value

	Return Value:
		If the function succeeds, the return value is true. 
		If the function fails, the return value is false. To get more error information, see the 'state' parameter.

	Remarks:
		This function don't change anything in the source scene.
	*/
	bool SnapshotScene(NxScene& scene, ErrorCode& state);

#ifdef _DEBUG
	NxScene* GetGhostScene() const { return m_pScene; };
#endif

private:
	static const NxVec3 gvecZero;
	static const NxReal gFrequency;

	class Report : public NxUserContactReport
	{
	public:
		Report() { Initiate(); }

		void SetModeOverlap(const NxActor& main, const NxActor& Target, const bool bGetDetails);
		void SetModeSweep(const NxActor& main, NxArray<NxShape*>& HitShapes, NxArray<COverlapDetailInfo>* pOutInfo = 0);

		virtual void onContactNotify(NxContactPair& pair, NxU32 events);

		bool GetResult() const { Translation(); NormalizeDetails(); return m_bResult; }
		bool GetDetails(COverlapDetailInfo& OutInfo) const;

	private:
		void ClearResult();
		void Initiate();
		bool IsExpected(const NxActor* pActor0, const NxActor* pActor1) const;
		int  AddShape(NxShape& shape);
		void AddDetailInfo(const int index, const COverlapDetailInfo& diVal);
		void Translation() const;
		void NormalizeDetails() const;

	private:
		enum Mode
		{
			MODE_DETECT_OVERLAP     = 0,
			MODE_DETECT_LINEARSWEEP = 1,
		};

	private:
		bool m_bResult;
		Mode m_Mode;

		const NxActor*	m_pMain;
		const NxActor*	m_pTarget;

		NxU32					m_idxStart;
		NxArray<NxShape*>*		m_pHitShape;
		NxArray<COverlapDetailInfo>*	m_pCollector;

		NxArray<NxShape*>		m_OwnHitShape;
		NxArray<COverlapDetailInfo>	m_OwnCollector;
	};

	class StreamNotify : public NXU_userNotify, public NXU_errorReport
	{
	public:
		StreamNotify(NxPhysicsSDK& SDK) : m_PhysXSDK(SDK) { Initialization(); }
		~StreamNotify();

		void Initialization() {	m_HasError = false; }
		bool HasError() const { return m_HasError; }

		virtual void NXU_errorMessage(bool isError, const char* str);
		virtual bool NXU_preNotifyActor(NxActorDesc& actor, const char* userProperties);

	private:
		virtual void NXU_notifyConvexMesh(NxConvexMesh* c, const char* userProperties);
		virtual void NXU_notifyHeightField(NxHeightField* t, const char* userProperties); 
		virtual void NXU_notifyTriangleMesh(NxTriangleMesh* t, const char* userProperties); 

		void ChangeToSourceMeshData(NxShapeDesc& descShape, const NxActor& srcActor, const unsigned int index);
		void ReleaseAllConvexMesh();
		void ReleaseAllHeightField();
		void ReleaseAllTriangleMesh();

	private:
		bool m_HasError;

		NxPhysicsSDK& m_PhysXSDK;
		NxArray<NxConvexMesh*> m_ConvexMesh;
		NxArray<NxHeightField*> m_HeightField;
		NxArray<NxTriangleMesh*> m_TriangleMesh;
	};

	struct ActorInfo
	{
		ActorInfo() { pMain = 0; pTarget = 0; }

		NxActor* pMain;
		NxActor* pTarget;
		NxVec3 posMain;
	};

private:
	bool PreDetection(ErrorCode& state, ActorInfo& outInfo, const NxActor& actor0, const NxActor* pActor1 = 0);
	bool PostDetection(const ActorInfo& restoreInfo) const;

	void ReleaseScene();
	bool HasInvalditeShape(const NxActor& actor, ErrorCode& state) const;
	bool CloneAllActors(NxScene& srcScene, StreamNotify& notify);

	void MainActorMakeup(NxActor& actor, ActorInfo& outInfo) const;
	void MainActorRemover(const ActorInfo& restore) const;

	NxActor* FindActor(const NxActor& srcActor) const;
	NxActor* CopyShapeDescAndCreateActor(NxActorDesc& ad, const NxActor& srcActor) const;
	
	void DoSweepKernel(NxActor& mainActor, const NxVec3& motion) const;
	void DoInflateDetectKernel(NxActor& mainActor, const float growSize, const NxMat33* pRotation = 0) const;

	bool	IsOppositeDirection(const NxVec3& dir0, const NxVec3& dir1) const;
	NxVec3	ChangeVelocity(NxActor& actor, const NxVec3& motion) const;
	void	SceneSimulate(NxScene& scene) const;
	
private:
	NxScene* m_pScene;
	Report	 m_Receiver;
};

};

#endif