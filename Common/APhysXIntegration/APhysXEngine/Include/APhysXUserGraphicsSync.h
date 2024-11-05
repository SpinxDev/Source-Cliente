/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXUSERGRAPHICSSYNC_H_
#define _APHYSXUSERGRAPHICSSYNC_H_

class APhysXObject;

class APhysXUserGraphicsSync
{

public:

	virtual ~APhysXUserGraphicsSync() {}


	// update the graphics data from physics data
	virtual void SyncGraphicsData(const APhysXObject* pPhysXObject) = 0;
	
	// update the physics data from graphics data
	virtual void SyncPhysicsData(APhysXObject* pPhysXObject) = 0;

	// release the graphics data or other user data corresponding to the APhysXObject...
	virtual void ReleaseGraphicsData() = 0;

};

#endif