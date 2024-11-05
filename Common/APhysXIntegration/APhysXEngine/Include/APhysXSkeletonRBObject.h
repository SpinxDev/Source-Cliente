/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	Description:

----------------------------------------------------------------------*/

#ifndef _APHYSXSKELETONRBOBJECT_H_
#define _APHYSXSKELETONRBOBJECT_H_


class APHYSX_DLL_EXPORT  APhysXSkeletonRBObject: public APhysXDynamicRBObject
{

	// friend class
	friend class APhysXScene;

public:

	// Graphics <---> Physics
	// just use the parent's method
	//virtual void SyncGraphicsData(){}			
	//virtual void SyncPhysicsData() {}			


protected:

	APhysXSkeletonRBObject(){};

	virtual void OnCreateNxActorDesc(NxActorDesc* pNxActorDesc, int iActorID);

};




#endif