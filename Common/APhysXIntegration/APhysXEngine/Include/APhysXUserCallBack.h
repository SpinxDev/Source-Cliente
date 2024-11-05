
/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	all PhysX user-defined classes are laid in this .h and .cpp...

----------------------------------------------------------------------*/


#ifndef _APHYSXUSERCALLBACK_H_
#define _APHYSXUSERCALLBACK_H_

template<class T>
class APhysXSimplePtrManager
{

public:

	void Register(T* pT)
	{
		if(pT) APhysXUtilLib::APhysXArray_PushOnce(m_arrElements, pT);
	}
	
	void UnRegister(T* pT)
	{
		if(pT) m_arrElements.deleteEntry(pT);
	}

	void RemoveAll()
	{
		m_arrElements.clear();
	}

protected:

	APhysXArray<T *> m_arrElements;

};


//-------------------------------------------------------------------------------
// contact report call back...

class APHYSX_DLL_EXPORT APhysXContactReport : public NxUserContactReport, public APhysXSimplePtrManager<NxUserContactReport>
{

//----------------------------------------------------------
// singleton object
	APHYSX_DECLARE_SINGLETON(APhysXContactReport)
//----------------------------------------------------------


public:

	virtual void  onContactNotify(NxContactPair& contactPair, NxU32 events);


protected:

	// currently, we handle the RB hit in the default contact report implementation...
	virtual void  HandleRBHit(NxContactPair& contactPair, NxU32 events);

};

#define gPhysXContactReport (APhysXContactReport::GetPtrInstance())


//-------------------------------------------------------------------------------
// trigger report call back...

class APHYSX_DLL_EXPORT APhysXTriggerReport : public NxUserTriggerReport, public APhysXSimplePtrManager<NxUserTriggerReport>
 
{

	//----------------------------------------------------------
	// singleton object
	APHYSX_DECLARE_SINGLETON(APhysXTriggerReport)

	//----------------------------------------------------------


public:

	virtual void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);

};

#define gPhysXTriggerReport (APhysXTriggerReport::GetPtrInstance())



//-------------------------------------------------------------------------------
// break report call back...

class APHYSX_DLL_EXPORT APhysXBreakReport : public APhysXUserBreakReport, public APhysXSimplePtrManager<APhysXUserBreakReport>

{

	//----------------------------------------------------------
	// singleton object
	APHYSX_DECLARE_SINGLETON(APhysXBreakReport)

	//----------------------------------------------------------


public:

	virtual void OnBreakNotify(APhysXBreakable* pBreakable);
	virtual void OnFirstBreakNotify(APhysXBreakable* pBreakable);
	virtual void OnAllBreakNotify(APhysXBreakable* pBreakable);

};

#define gPhysXBreakReport (APhysXBreakReport::GetPtrInstance())


//-------------------------------------------------------------------------------
// creation/deletion report call back...
// derive this class for user-defined creation/deletion callback...

class APhysXObject;
class APhysXObjectConnector;
class APhysXScene;


class APHYSX_DLL_EXPORT APhysXUserCreationDeletionReport
{

public:

	// Beware these:
	//		all creation user notifying function is called after the real creation,
	// however, all deletion user notifying function is called before the real creation

	// Note: 
	//		if pObject's GetParentObject() is NULL, pObject is just created and owned by the scene,
	// otherwise, pObject is one component object of a compound object.
	virtual void OnObjectCreationNotify(APhysXObject* pObject) {}
	virtual void OnObjectDeletionNotify(APhysXObject* pObject) {}

	virtual void OnConnectorCreationNotify(APhysXObjectConnector* pObject, bool bInnerConnector) {}
	virtual void OnConnectorDeletionNotify(APhysXObjectConnector* pObject, bool bInnerConnector) {}

	virtual void OnSceneCreationNotify(APhysXScene* pScene) {} 
	virtual void OnSceneDeletionNotify(APhysXScene* pScene) {} 


protected:

	virtual ~APhysXUserCreationDeletionReport() {}

};


class APHYSX_DLL_EXPORT APhysXCreationDeletionReport : public APhysXUserCreationDeletionReport, public APhysXSimplePtrManager<APhysXUserCreationDeletionReport>

{

	//----------------------------------------------------------
	// singleton object
	APHYSX_DECLARE_SINGLETON(APhysXCreationDeletionReport)
	//----------------------------------------------------------


public:

	virtual void OnObjectCreationNotify(APhysXObject* pObject);
	virtual void OnObjectDeletionNotify(APhysXObject* pObject);

	virtual void OnConnectorCreationNotify(APhysXObjectConnector* pConnector, bool bInnerConnector);
	virtual void OnConnectorDeletionNotify(APhysXObjectConnector* pConnector, bool bInnerConnector);

	virtual void OnSceneCreationNotify(APhysXScene* pScene);
	virtual void OnSceneDeletionNotify(APhysXScene* pScene); 


protected:

	void PreUserObjectCreationNotify(APhysXObject* pObject);
	void PostUserObjectCreationNotify(APhysXObject* pObject);

	void PreUserObjectDeletionNotify(APhysXObject* pObject);
	void PostUserObjectDeletionNotify(APhysXObject* pObject);

};

#define gPhysXCreationDeletionReport (APhysXCreationDeletionReport::GetPtrInstance())


//-------------------------------------------------------------------------------
// sub-simulation report call back...

class APHYSX_DLL_EXPORT APhysXUserSubSimulationReport
{

public:

	virtual void PreSubSimulation(float dt) {}
	virtual void PostSubSimulation(float dt) {}

	// handle the creation/deletion event of the specified APhysXObject
	virtual void OnObjectCreation(APhysXObject* pObject) {}
	virtual void OnObjectDeletion(APhysXObject* pObject) {}

};

class APHYSX_DLL_EXPORT APhysXSubSimulationReport : public APhysXUserSubSimulationReport, public APhysXSimplePtrManager<APhysXUserSubSimulationReport>

{

	//----------------------------------------------------------
	// singleton object
	APHYSX_DECLARE_SINGLETON(APhysXSubSimulationReport)

	//----------------------------------------------------------


public:

	virtual void PreSubSimulation(float dt);
	virtual void PostSubSimulation(float dt);

	virtual void OnObjectCreation(APhysXObject* pObject);
	virtual void OnObjectDeletion(APhysXObject* pObject);

};

#define gPhysXSubSimulationReport (APhysXSubSimulationReport::GetPtrInstance())

#endif