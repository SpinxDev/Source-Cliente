
#include "APhysXStdAfx.h"

//--------------------------------------------------------------------------------
// precompiler option note:
//
//     if APX_MANAGE_INTER_OBJECT_CONNECTOR is set to 1, we will manage the creation and 
// release of inter-object connector internally in APhysXEngine lib. the major work lies
// in if we release an object we will find all connectors it is linked and delete them as well.
//
//     if APX_MANAGE_INTER_OBJECT_CONNECTOR is set to 0, we won't handle the above work...
//
#define APX_MANAGE_INTER_OBJECT_CONNECTOR 0
//--------------------------------------------------------------------------------


/*
bool APhysXObjectConnector::OnCreate(APhysXObject* pAPhysXObject, const APhysXObjectConnectorDesc& apxObjConnectorDesc)
{
	m_pAPhysXScene = pAPhysXObject->GetPhysXScene();

	m_apxConnectorType = (APhysXConnectorType)apxObjConnectorDesc.GetConnectorType();
	m_pConnectedAPhysXObject = m_pAPhysXScene->GetPhysXObject(apxObjConnectorDesc.mOtherPhysXObjectName);
	
	if(m_pConnectedAPhysXObject)

	return true;

}
*/

APhysXObjectConnector* APhysXObjectConnector::CreateConnectorPtr(APhysXConnectorType cntType)
{

	APhysXObjectConnector* pConnector = NULL;

	switch(cntType)
	{
	//------------------------------------------------
	// all these types are corresponding to APhysXJoint pointer...
	case APX_CNTTYPE_CYLINDRICAL_JOINT:
		// break;
	case APX_CNTTYPE_D6_JOINT:
		// break;
	case APX_CNTTYPE_DISTANCE_JOINT:
	    // break;
	case APX_CNTTYPE_FIXED_JOINT:
	    // break;
	case APX_CNTTYPE_POINTINPLANE_JOINT:
		// break;
	case APX_CNTTYPE_POINTONLINE_JOINT:
		// break;
	case APX_CNTTYPE_PRISMATIC_JOINT:
	    // break;
	case APX_CNTTYPE_PULLEY_JOINT:
	    // break;
	case APX_CNTTYPE_REVOLUTE_JOINT:
		//break;
	case APX_CNTTYPE_SPHERICAL_JOINT:

		pConnector = new APhysXJoint;
		break;
	//------------------------------------------------
	case APX_CNTTYPE_CLOTH_ATTACH_RB:

		pConnector = new APhysXClothAttachRBConnector;
		break;
	
	case APX_CNTTYPE_FLUIDEMITTER_ATTACH_RB:

		pConnector = new APhysXFluidEmitterAttachRBConnector;
		break;

	default:

		APHYSX_ASSERT(0);

	    break;
	}

	return pConnector;
}

bool APhysXObjectConnector::OnCreateByPhysXObject(APhysXObject* pAPhysXObject, const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc)
{
	APHYSX_ASSERT(pAPhysXObject);

	if(!APhysXUtilLib::CheckProperRigidTransform(apxObjConnectorInstanceDesc.mGlobalPose))
	{
		gPhysXLog->Log("Warning: try to create a connector in a Non Proper Rigid Transformation Pose!");
	}


	m_pAPhysXScene = pAPhysXObject->GetPhysXScene();

	if(apxObjConnectorInstanceDesc.mPhysXConnectorDesc->IsAPhysXObjectsSet())
	{
		// directly use the objects data member...
		m_pAPhysXObject1 = apxObjConnectorInstanceDesc.mPhysXConnectorDesc->mPhysXObject1;
		m_pAPhysXObject2 = apxObjConnectorInstanceDesc.mPhysXConnectorDesc->mPhysXObject2;
	}
	else
	{
		if(pAPhysXObject->IsCompound())
		{
			// compound object type, we should find the two child objects of the creator object
			APhysXCompoundObject* pCompoundObject = (APhysXCompoundObject*) pAPhysXObject;
			m_pAPhysXObject1 = pCompoundObject->GetPhysXObjectComponent(apxObjConnectorInstanceDesc.mPhysXConnectorDesc->mPhysXObject1Name);
			m_pAPhysXObject2 = pCompoundObject->GetPhysXObjectComponent(apxObjConnectorInstanceDesc.mPhysXConnectorDesc->mPhysXObject2Name);

			APHYSX_ASSERT(m_pAPhysXObject1 && m_pAPhysXObject2);
		}
		else
		{
			// other object type, both two objects of connector is just the creator object.
			m_pAPhysXObject1 = m_pAPhysXObject2 = pAPhysXObject;
		}

	}

	// set the name of the connector
	m_szName = apxObjConnectorInstanceDesc.mPhysXConnectorDesc->GetName();

	bool bRes = CreateConnector(apxObjConnectorInstanceDesc);

	// user's creation callback
	if(bRes) 
	{
		gPhysXCreationDeletionReport->OnConnectorCreationNotify(this, true);
	}

	return bRes;
}

void APhysXObjectConnector::OnReleaseByPhysXObject()
{
	// user's deletion callback
	gPhysXCreationDeletionReport->OnConnectorDeletionNotify(this, true);
	
	ReleaseConnector();

	// to do...
	m_pAPhysXObject1 = NULL;
	m_pAPhysXObject2 = NULL;
	m_pAPhysXScene = NULL;
}

bool APhysXObjectConnector::OnCreateByPhysXScene(APhysXScene* pAPhysXScene, const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc)
{

	if(!APhysXUtilLib::CheckProperRigidTransform(apxObjConnectorInstanceDesc.mGlobalPose))
	{
		gPhysXLog->Log("Warning: try to create a connector in a Non Proper Rigid Transformation Pose!");
	}

	m_pAPhysXScene = pAPhysXScene;
	
	if(apxObjConnectorInstanceDesc.mPhysXConnectorDesc->IsAPhysXObjectsSet())
	{
		// directly use the objects data member...
		m_pAPhysXObject1 = apxObjConnectorInstanceDesc.mPhysXConnectorDesc->mPhysXObject1;
		m_pAPhysXObject2 = apxObjConnectorInstanceDesc.mPhysXConnectorDesc->mPhysXObject2;
	}
	else
	{
		m_pAPhysXObject1 = m_pAPhysXScene->GetPhysXObject(apxObjConnectorInstanceDesc.mPhysXConnectorDesc->mPhysXObject1Name);
		m_pAPhysXObject2 = m_pAPhysXScene->GetPhysXObject(apxObjConnectorInstanceDesc.mPhysXConnectorDesc->mPhysXObject2Name);
	}

	if(m_pAPhysXObject1 || m_pAPhysXObject2)
	{
		// we must have at least one non-NULL APhysXObject
		bool bRes =  CreateConnector(apxObjConnectorInstanceDesc);

		// set the name of the connector
		m_szName = apxObjConnectorInstanceDesc.mPhysXConnectorDesc->GetName();

#if APX_MANAGE_INTER_OBJECT_CONNECTOR

		if(bRes)
		{
			if(m_pAPhysXObject1) m_pAPhysXObject1->AddOuterConnector(this);
			if(m_pAPhysXObject2) m_pAPhysXObject2->AddOuterConnector(this);
		}

#endif

		// user's creation callback
		if(bRes) 
		{
			gPhysXCreationDeletionReport->OnConnectorCreationNotify(this, false);
		}

		return bRes;
	}
	else
	{
		return false;
	}
}


void APhysXObjectConnector::OnReleaseByPhysXScene(APhysXObject* pAPhysXObjectReleased)
{

	// user's deletion callback
	gPhysXCreationDeletionReport->OnConnectorDeletionNotify(this, false);

	ReleaseConnector();

	APHYSX_ASSERT(m_pAPhysXObject1 != m_pAPhysXObject2);

#if APX_MANAGE_INTER_OBJECT_CONNECTOR


	if(pAPhysXObjectReleased)
	{
		if(pAPhysXObjectReleased == m_pAPhysXObject1)
		{
			if(m_pAPhysXObject2)
				m_pAPhysXObject2->RemoveOuterConnector(this);
		}
		else if(pAPhysXObjectReleased == m_pAPhysXObject2)
		{
			if(m_pAPhysXObject1)
				m_pAPhysXObject1->RemoveOuterConnector(this);
		}

	}
	else
	{
		if(m_pAPhysXObject1) m_pAPhysXObject1->RemoveOuterConnector(this);
		if(m_pAPhysXObject2) m_pAPhysXObject2->RemoveOuterConnector(this);
	}


#endif

	m_pAPhysXObject1 = NULL;
	m_pAPhysXObject2 = NULL;
	m_pAPhysXScene = NULL;
}
