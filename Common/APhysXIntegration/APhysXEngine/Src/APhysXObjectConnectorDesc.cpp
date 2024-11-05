#include "APhysXStdAfx.h"

bool APhysXObjectConnectorDesc::Save(NxStream* pNxStream)
{
	APhysXSerializerBase::Save(pNxStream);

	// save my version...
	SaveVersion(pNxStream);
	
	APhysXSerializerBase::StoreString(pNxStream, mPhysXObject1Name);
	APhysXSerializerBase::StoreString(pNxStream, mPhysXObject2Name);

	return true;
}

bool APhysXObjectConnectorDesc::Load(NxStream* pNxStream)
{ 
	APhysXSerializerBase::Load(pNxStream);

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

		mPhysXObject1Name = APhysXSerializerBase::ReadString(pNxStream);
		mPhysXObject2Name = APhysXSerializerBase::ReadString(pNxStream);

		return true;

	}
	else
		return false;
}

//----------------------------------------------------------------------------------

APhysXObjectConnectorDesc* APhysXObjectConnectorDescManager::CreateConnectorDesc(APhysXConnectorType apxConnectorType)
{
	APhysXObjectConnectorDesc* pConnectorDesc = NULL;

	switch(apxConnectorType)
	{
	case APX_CNTTYPE_CYLINDRICAL_JOINT:

		pConnectorDesc = new APhysXCylindricalJointDesc;

		break;

	case APX_CNTTYPE_D6_JOINT:
		
		pConnectorDesc = new APhysXD6JointDesc;
		
		break;
	
	case APX_CNTTYPE_DISTANCE_JOINT:

		pConnectorDesc = new APhysXDistanceJointDesc;

	    break;
	
	case APX_CNTTYPE_FIXED_JOINT:

		pConnectorDesc = new APhysXFixedJointDesc;

	    break;
	
	case APX_CNTTYPE_POINTINPLANE_JOINT:
		
		pConnectorDesc = new APhysXPointInPlaneJointDesc;

		break;
	
	case APX_CNTTYPE_POINTONLINE_JOINT:

		pConnectorDesc = new APhysXPointOnLineJointDesc;

		break;
	
	case APX_CNTTYPE_PRISMATIC_JOINT:

		pConnectorDesc = new APhysXPrismaticJointDesc;

	    break;

	case APX_CNTTYPE_PULLEY_JOINT:

		pConnectorDesc = new APhysXPulleyJointDesc;

	    break;
	
	case APX_CNTTYPE_REVOLUTE_JOINT:

		pConnectorDesc = new APhysXRevoluteJointDesc;

		break;

	case APX_CNTTYPE_SPHERICAL_JOINT:

		pConnectorDesc = new APhysXSphericalJointDesc;

		break;

	case APX_CNTTYPE_CLOTH_ATTACH_RB:

		pConnectorDesc = new APhysXClothAttachRBConnectorDesc;

		break;

	case APX_CNTTYPE_FLUIDEMITTER_ATTACH_RB:

		pConnectorDesc = new APhysXFluidEmitterAttachRBConnectorDesc;
		break;

	default:

		APHYSX_ASSERT(0);

	    break;
	}

	return pConnectorDesc;

}

bool APhysXObjectConnectorDescManager::Save(NxStream* pNxStream)
{
	// firstly, we save the number of the array
	pNxStream->storeDword(m_arrPhysXConnectorDescs.size());

	// then we save the each element of array
	for(int i=0; i<m_arrPhysXConnectorDescs.size(); i++)
	{
		APhysXObjectConnectorDesc* pConnectoreDesc = m_arrPhysXConnectorDescs[i];

		// save the shape type
		pNxStream->storeDword(pConnectoreDesc->GetConnectorType());

		// save the shape data
		pConnectoreDesc->Save(pNxStream);

	}

	return true;

}

bool APhysXObjectConnectorDescManager::Load(NxStream* pNxStream)
{
	APhysXU32 l = pNxStream->readDword();
	if(l)
	{
		// clear the ShapeDesc array...
		m_arrPhysXConnectorDescs.clear();

		for(int i=0; i<l; i++)
		{
			APhysXU32 ConnectorType = pNxStream->readDword();
			APhysXObjectConnectorDesc* pConnectoreDesc = CreateConnectorDesc((APhysXConnectorType)ConnectorType);

			APHYSX_ASSERT(pConnectoreDesc);

			if(!pConnectoreDesc->Load(pNxStream))
			{
				delete pConnectoreDesc;
			}

			m_arrPhysXConnectorDescs.push_back(pConnectoreDesc);
		}
	}

	return true;

}

void APhysXObjectConnectorDescManager::Release()
{
	for(int i=0; i< m_arrPhysXConnectorDescs.size(); i++)
	{
		delete m_arrPhysXConnectorDescs[i];
	}

	m_arrPhysXConnectorDescs.clear();

}
