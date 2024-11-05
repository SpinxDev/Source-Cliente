/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

//////////////////////////////////////////////////////////	
	
	APhysXBreakableRBObjectDesc.h 

	Created by He Wenfeng, Aug.11, 2009

	All breakable RB Object Descriptors are defined in this file.


----------------------------------------------------------------------*/

#ifndef	_APHYSXBREAKABLERBOBJECTDESC_H_
#define _APHYSXBREAKABLERBOBJECTDESC_H_


//-----------------------------------------------------------------------------------------
// Simple breakable skeleton object:  it will break all parts at once.

class APHYSX_DLL_EXPORT APhysXSimpleBreakableSkeletonRBObjectDesc:public APhysXSkeletonRBObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE);

public:

	APhysXSimpleBreakableSkeletonRBObjectDesc()
	{

		mBreakLimit = NX_MAX_F32;				// the default value means never break...

		mDynamic = false;
	}

	// virtual interface to clone myself
	DeclareCloneAPhysXObjectDesc(APhysXSimpleBreakableSkeletonRBObjectDesc)


public:

	float mBreakLimit;

	// test code, need not to save/load it currently...
	bool mDynamic;
};


//-----------------------------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXBreakableSkeletonRBObjectDesc:public APhysXSkeletonRBObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_SKELETON_BREAKABLE);

public:

	APhysXBreakableSkeletonRBObjectDesc()
	{

		m_pActorNeighborGraph = NULL;
		m_pActorSupportGraph = NULL;

		//----------------------------------------------------
		// Note: about the unit of force and the break limit
		// One unit here represents not Newton but 1000 newton!
		mActorBreakLimit = 100.0f;		
		mBreakFallOffFactor = 0.3f;
		
	}

	virtual ~APhysXBreakableSkeletonRBObjectDesc() { Release(); }

	virtual void Release();

	const APhysXBreakableSkeletonRBObjectDesc& operator= (const APhysXBreakableSkeletonRBObjectDesc& other);
	
	// virtual interface to clone myself
	DeclareCloneAPhysXObjectDesc(APhysXBreakableSkeletonRBObjectDesc)
	


	void SetActorNeighborGraph(APhysXGraph* pActorNeighborGraph);
	void SetActorSupportGraph(APhysXGraph* pActorSupportGraph);

	APhysXGraph* GetActorNeighborGraph() { return m_pActorNeighborGraph;}
	APhysXGraph* GetActorSupportGraph() { return m_pActorSupportGraph; }


public:

	// the force magnitude limit which make the actor breaks
	float mActorBreakLimit;

	// the fall-off factor when the break force passes from one actor to its' neighbor actors
	float mBreakFallOffFactor;


protected:

	// these will be the shared data with all its APhysXBreakableSkeletonRBObject instances
	APhysXGraph* m_pActorNeighborGraph;
	APhysXGraph* m_pActorSupportGraph;

};



#endif
