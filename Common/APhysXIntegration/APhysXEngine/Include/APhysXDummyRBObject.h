/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

	Description:
		
		APhysXDummyRBObject is a class which defines the PhysX objects only
	having only physics instances but not graphics ones corresponding to Angelica
	Engine object.
----------------------------------------------------------------------*/

#ifndef _APHYSXDUMMYRBOBJECT_H_
#define _APHYSXDUMMYRBOBJECT_H_


class APHYSX_DLL_EXPORT APhysXDummyRBObject : public APhysXRigidBodyObject
{
	// just use GetType() instead
	//DeclareObjType(APX_OBJTYPE_DUMMY);

	// friend class
	friend class APhysXScene;

public:

	virtual void SerializePhysX() {	}
	virtual void DeserializePhysX() { }


	// Graphics <---> Physics
	virtual void SyncGraphicsData(){}			// do nothing
	virtual void SyncPhysicsData() {}			// do nothing


protected:
	
	APhysXDummyRBObject(){};


};


#endif