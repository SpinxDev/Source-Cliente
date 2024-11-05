/*----------------------------------------------------------------------

Common header of APhysXEngine header files...

Created by He Wenfeng, Jan.2, 2008

Description:

----------------------------------------------------------------------*/

#ifndef _APHYSXDYNAMICRBOBJECT_H_
#define _APHYSXDYNAMICRBOBJECT_H_


class APHYSX_DLL_EXPORT APhysXDynamicRBObject : public APhysXRigidBodyObject
{
	// just use GetType() instead
	//DeclareObjType(APX_OBJTYPE_DYNAMIC);

	// friend class
	friend class APhysXScene;

public:

	// Graphics <---> Physics
	// just use the parent's method
	//virtual void SyncGraphicsData(){}			
	//virtual void SyncPhysicsData() {}			


protected:

	APhysXDynamicRBObject(){};
};


#endif