
#include "APhysXStdAfx.h"

APhysXShapeDesc* APhysXShapeDescManager::CreateShapeDesc(APhysXShapeType apxShapeType)
{
	APhysXShapeDesc* pShapeDesc = NULL;

	switch (apxShapeType)
	{
	case APX_SHAPETYPE_PLANE:

		pShapeDesc = new APhysXPlaneShapeDesc;
		break;

	case APX_SHAPETYPE_SPHERE:

		pShapeDesc = new APhysXSphereShapeDesc;
		break;

	case APX_SHAPETYPE_BOX:

		pShapeDesc = new APhysXBoxShapeDesc;
		break;

	case APX_SHAPETYPE_CAPSULE:

		pShapeDesc = new APhysXCapsuleShapeDesc;
		break;

	case APX_SHAPETYPE_CONVEX:

		pShapeDesc = new APhysXConvexShapeDesc;
		break;

	case APX_SHAPETYPE_HEIGHTFIELD:

		pShapeDesc = new APhysXHeightFieldShapeDesc;
		break;

	default:

		APHYSX_ASSERT(0);
		break;
	};

	return pShapeDesc;
}

/* 
// it seems no need any more...

APhysXShapeDesc* APhysXShapeDescManager::CreateAndPushShapeDesc(APhysXShapeType apxShapeType)
{
	APhysXShapeDesc* pShapeDesc = NULL;

	switch (apxShapeType)
	{
		case APX_SHAPETYPE_PLANE:
			
			pShapeDesc = new APhysXPlaneShapeDesc;
			break;

		case APX_SHAPETYPE_SPHERE:
			
			pShapeDesc = new APhysXSphereShapeDesc;
			break;

		case APX_SHAPETYPE_BOX:

			pShapeDesc = new APhysXBoxShapeDesc;
			break;

		case APX_SHAPETYPE_CAPSULE:

			pShapeDesc = new APhysXCapsuleShapeDesc;
			break;

		case APX_SHAPETYPE_CONVEX:

			pShapeDesc = new APhysXConvexShapeDesc;
			break;

		case APX_SHAPETYPE_HEIGHTFIELD:
			
			pShapeDesc = new APhysXHeightFieldShapeDesc;
			break;

		default:

			APHYSX_ASSERT(0);
			break;
	};

	if(pShapeDesc)
		m_arrShapeDescs.push_back(pShapeDesc);

	return pShapeDesc;
	
}

APhysXShapeDesc* APhysXShapeDescManager::CreateAndPushShapeDesc(APhysXShapeDesc* pShapeDesc)
{
	APhysXShapeDesc* pInternalShapeDesc = CreateShapeDesc(pShapeDesc->GetShapeType());

	if(pInternalShapeDesc)
	{
		switch (pInternalShapeDesc->GetShapeType())
		{
		
		case APX_SHAPETYPE_PLANE:

			{
				APhysXPlaneShapeDesc& DestShapeDesc = *(APhysXPlaneShapeDesc *)pInternalShapeDesc;
				APhysXPlaneShapeDesc& SrcShapeDesc = *(APhysXPlaneShapeDesc *)pShapeDesc;
				DestShapeDesc = SrcShapeDesc;
			}

			break;

		case APX_SHAPETYPE_SPHERE:

			{
				APhysXSphereShapeDesc& DestShapeDesc = *(APhysXSphereShapeDesc *)pInternalShapeDesc;
				APhysXSphereShapeDesc& SrcShapeDesc = *(APhysXSphereShapeDesc *)pShapeDesc;
				DestShapeDesc = SrcShapeDesc;
			}

			break;

		case APX_SHAPETYPE_BOX:

			{
				APhysXBoxShapeDesc& DestShapeDesc = *(APhysXBoxShapeDesc *)pInternalShapeDesc;
				APhysXBoxShapeDesc& SrcShapeDesc = *(APhysXBoxShapeDesc *)pShapeDesc;
				DestShapeDesc = SrcShapeDesc;
			}

			break;

		case APX_SHAPETYPE_CAPSULE:

			{
				APhysXCapsuleShapeDesc& DestShapeDesc = *(APhysXCapsuleShapeDesc *)pInternalShapeDesc;
				APhysXCapsuleShapeDesc& SrcShapeDesc = *(APhysXCapsuleShapeDesc *)pShapeDesc;
				DestShapeDesc = SrcShapeDesc;
			}

			break;

		case APX_SHAPETYPE_CONVEX:

			{
				APhysXConvexShapeDesc& DestShapeDesc = *(APhysXConvexShapeDesc *)pInternalShapeDesc;
				APhysXConvexShapeDesc& SrcShapeDesc = *(APhysXConvexShapeDesc *)pShapeDesc;
				DestShapeDesc = SrcShapeDesc;
			}

			break;

		case APX_SHAPETYPE_HEIGHTFIELD:

			{
				APhysXHeightFieldShapeDesc& DestShapeDesc = *(APhysXHeightFieldShapeDesc *)pInternalShapeDesc;
				APhysXHeightFieldShapeDesc& SrcShapeDesc = *(APhysXHeightFieldShapeDesc *)pShapeDesc;
				DestShapeDesc = SrcShapeDesc;

			}

			break;

		default:
			
			APHYSX_ASSERT(0);
			break;
		};
	}

	return pInternalShapeDesc;
}
*/


void APhysXShapeDescManager::ReleaseShapeDesc(APhysXShapeDesc* pShapeDesc)
{
	m_arrShapeDescs.deleteEntry(pShapeDesc);
	delete pShapeDesc;
	pShapeDesc = NULL;

}

bool APhysXShapeDescManager::Save(NxStream* pNxStream)
{
	// firstly, we save the number of the array
	pNxStream->storeDword(m_arrShapeDescs.size());

	// then we save the each element of array
	for(int i=0; i<m_arrShapeDescs.size(); i++)
	{
		APhysXShapeDesc* pShapeDesc = m_arrShapeDescs[i];
		
		// save the shape type
		pNxStream->storeDword(pShapeDesc->GetShapeType());

		// save the shape data
		pShapeDesc->Save(pNxStream);

	}
	
	return true;

}

bool APhysXShapeDescManager::Load(NxStream* pNxStream)
{
	APhysXU32 l = pNxStream->readDword();
	if(l)
	{
		// clear the ShapeDesc array...
		m_arrShapeDescs.clear();

		for(int i=0; i<l; i++)
		{
			APhysXU32 ShapeType = pNxStream->readDword();
			APhysXShapeDesc* pShapeDesc = CreateShapeDesc((APhysXShapeType)ShapeType);

			APHYSX_ASSERT(pShapeDesc);

			if(pShapeDesc->Load(pNxStream))
			{
				m_arrShapeDescs.push_back(pShapeDesc);
			}
			else
			{
				m_arrShapeDescs.push_back(NULL);		// we should still push a NULL pointer as the placeholder

				gPhysXLog->Log("Warning: Failed on loading APhysXShapeDesc.");
				delete pShapeDesc;
			}
		}

		// update here?
		//UpdateNxShapeDescs();
	}

	return true;

}

void APhysXShapeDescManager::Release()
{

	for(int i=0; i< m_arrShapeDescs.size(); i++)
	{
		delete m_arrShapeDescs[i];
	}

	m_arrShapeDescs.clear();

	/*
	for(int i= 0; i<m_arrNxShapeDescs.size(); i++)
	{
		delete m_arrNxShapeDescs[i];
	}
	m_arrNxShapeDescs.clear();
	*/

}

/*
void APhysXShapeDescManager::UpdateNxShapeDescs()
{
	
	m_arrNxShapeDescs.clear();

	for(int i=0; i< m_arrShapeDescs.size(); i++)
	{
		NxShapeDesc* pNxShapeDesc = m_arrShapeDescs[i]->CreateNxShapeDesc();
		m_arrNxShapeDescs.push_back(pNxShapeDesc);
	}

}
*/

APhysXShapeDesc* APhysXShapeDescManager::CreateAndPushShapeDesc(APhysXShapeType apxShapeType)
{
	APhysXShapeDesc* pShapeDesc = CreateShapeDesc(apxShapeType);

	if(pShapeDesc) 
		m_arrShapeDescs.push_back(pShapeDesc);

	return pShapeDesc;
}

APhysXShapeDesc* APhysXShapeDescManager::CreateAndPushShapeDesc(const APhysXShapeDesc* pShapeDesc)
{
	if(pShapeDesc)
	{
		APhysXShapeDesc* pClonedShapeDesc = pShapeDesc->Clone();
		m_arrShapeDescs.push_back(pClonedShapeDesc);

		return pClonedShapeDesc;
	}
	else
	{
		return NULL;
	}
}

const APhysXShapeDescManager& APhysXShapeDescManager::operator= (const APhysXShapeDescManager& other)
{
	Release();

	for(int i = 0; i< other.GetNum(); i++)
	{
		m_arrShapeDescs.push_back(other.GetPhysXShapeDesc(i)->Clone());	
	}

	return *this;
}
