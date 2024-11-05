/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXOBJECTCONNECTOR_H_
#define _APHYSXOBJECTCONNECTOR_H_


class APHYSX_DLL_EXPORT APhysXObjectConnector
{

	friend class APhysXScene;
	friend class APhysXObject;

public:
	
	APhysXObjectConnector() 
	{
		m_pAPhysXScene = NULL;
		
		m_pAPhysXObject1 = NULL;
		m_pAPhysXObject2 = NULL;
	}

	virtual ~APhysXObjectConnector() {}

	virtual APhysXJoint* GetJoint() { return NULL; }

	APhysXConnectorType GetType() const { return m_apxConnectorType; }
	
	/*
	const char* GetConnectedObjectName() 
	{ 
		return m_pConnectedAPhysXObject->GetName();
	}
	*/

	const char* GetName()
	{
		return m_szName;
	}


	static APhysXObjectConnector* CreateConnectorPtr(APhysXConnectorType cntType);

protected:

	//------------------------------------------------------------------------------------
	// overriden by the children classes to create and release real connector stuff...
	virtual bool CreateConnector(const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc) { return false; }
	virtual void ReleaseConnector() {}


	//------------------------------------------------------------------------------------
	// called for the case of inner joints, created by APhysXObject
	virtual bool OnCreateByPhysXObject(APhysXObject* pAPhysXObject, const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc);
	// called when the connector is released by APhysXObject
	virtual void OnReleaseByPhysXObject();
	
	// called for the case of inter-APhysXObject joints, created by APhysXScene
	virtual bool OnCreateByPhysXScene(APhysXScene* pAPhysXScene, const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc);
	// called when the connector is released by APhysXScene, pAPhysXObject is 
	// the released PhysXObject which triggers the release of joint, if it is NULL
	virtual void OnReleaseByPhysXScene(APhysXObject* pAPhysXObjectReleased = NULL);


protected:

	APhysXScene* m_pAPhysXScene;

	APhysXConnectorType m_apxConnectorType;

	//APhysXObject* m_pConnectedAPhysXObject;

	APhysXObject* m_pAPhysXObject1;
	APhysXObject* m_pAPhysXObject2;

	//-------------------------------------------------------------------------
	// name of connector: currently used as the id name in the scope 
	// of APhysXObject's inner connectors. This means that there may be
	// different connectors in the Scene with the same name. However,  
	// as for one APhysXObject's inner connectors, their name should be
	// unique...
	const char* m_szName;					
};

#endif