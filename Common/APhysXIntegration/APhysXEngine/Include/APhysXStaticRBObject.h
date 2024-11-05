/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	Description:

----------------------------------------------------------------------*/

#ifndef _APHYSXSTATICRBOBJECT_H_
#define _APHYSXSTATICRBOBJECT_H_


class APHYSX_DLL_EXPORT APhysXStaticRBObject : public APhysXRigidBodyObject
{
	// just use GetType() instead
	//DeclareObjType(APX_OBJTYPE_STATIC);

	// friend class
	friend class APhysXScene;

public:

	// Graphics <---> Physics
	virtual void SyncGraphicsData(){}			// do nothing
	virtual void SyncPhysicsData() {}			// do nothing


protected:

	APhysXStaticRBObject(){};
};


#endif