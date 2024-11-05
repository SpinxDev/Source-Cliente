/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	Note:
		this is a utility class which provides picking and dragging
	dynamic	NxActors or	NxCloths function to your application...

	Feb. 6, 2009:
			
		task to do: multi-thread support: acquire APhysXScene's Writing 
	Lock before create and release actors and joints...
----------------------------------------------------------------------*/

#ifndef _APHYSXPICKER_H_
#define _APHYSXPICKER_H_

class APhysXScene;

enum APhysXRaycastFilterType
{
	APX_RFTYPE_DANYMIC_RB,		// only raycast dynamic rigid body actors
	APX_RFTYPE_STATIC_RB,		// only raycast static rigid body actors
	APX_RFTYPE_RB,				// raycast all rigid body actors

	APX_RFTYPE_CLOTH,			// raycast cloths

	APX_RFTYPE_ALL,				// raycast rigid body actors and cloths
};

struct APhysXRayHitInfo
{
	NxVec3 mHitPos;
	float mDist;

	// only for case of hitting NxActor 
	NxVec3 mHitNormal;
	NxShape* mShape;
	NxActor* mActor;

	// only for case of hitting NxCloth
	NxCloth* mCloth;
	APhysXU32 mClothHitVertexID;

	APhysXRayHitInfo()
	{
		mShape = NULL;
		mActor = NULL;
		mCloth = NULL;
	}
};

class APHYSX_DLL_EXPORT APhysXPicker
{

public:

	// some utility functions...
	static bool Raycast(APhysXScene* pAPhysXScene, const NxRay& worldRay, APhysXRayHitInfo& hitInfo, APhysXRaycastFilterType apxRFType = APX_RFTYPE_ALL, float fMaxDist = NX_MAX_F32, APhysXU32 uGroupFilter = 0xffffffff);

	static bool Raycast(APhysXScene* pAPhysXScene, A3DViewport* pA3DViewPort, int iScreenX, int iScreenY, APhysXRayHitInfo& hitInfo, APhysXRaycastFilterType apxRFType = APX_RFTYPE_ALL, APhysXU32 uGroupFilter = 0xffffffff);

	// transform the mouse position to a world ray, return the distance from the near clip plane to the far clip plane...
	static float GetPickingRay(A3DViewport* pA3DViewPort, int iScreenX, int iScreenY, NxRay& worldRay);

public:
	
	APhysXPicker()
	{
		m_pNxScene = NULL;
		m_pA3DViewPort = NULL;

		m_pPickedActor = NULL;
		m_pPickedCloth = NULL;
		m_uPickedClothVertexID = 0;

		m_pAnchorActor = NULL;
		m_pAnchorJoint = NULL;

		m_fPickedPosDepth = 0.0f;
	}

	APhysXPicker(APhysXScene* pAPhysXScene, A3DViewport* pA3DViewPort)
	{
		Init(pAPhysXScene, pA3DViewPort);
	}

	~APhysXPicker() 
	{
		Release();
	}

	void Init(APhysXScene* pAPhysXScene, A3DViewport* pA3DViewPort);
	void Release();
	
	// if I picked some dynamic actor or cloth, return true; else return false
	bool OnMouseButtonDown(int x, int y);
	
	void OnMouseButtonUp(int x, int y);
	void OnMouseMove(int x, int y);

	// check if we are picking up some actor or cloth which is invalidated...
	bool Invalidate(const APhysXObject* pObject);
	bool Invalidate(const NxActor* pActor);
	bool Invalidate(const NxCloth* pCloth);

protected:

	void ReleasePicking();

	/*
	//----------------------------------------------------------------
	// obsolete code...
	// override this function if you have your own implementation...
	virtual bool ScreenPosToWorldRay(int x, int y, NxRay& worldRay);
	NxVec3 PickActorAndCloth(const NxRay& worldRay);
	
	*/

protected:

	NxScene* m_pNxScene;
	A3DViewport* m_pA3DViewPort;

	NxActor* m_pPickedActor;
	NxCloth* m_pPickedCloth;
	APhysXU32 m_uPickedClothVertexID;

	NxActor* m_pAnchorActor;
	NxJoint* m_pAnchorJoint;

	float m_fPickedPosDepth;
};



#endif