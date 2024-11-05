
#include "APhysXStdAfx.h"

//----------------------------------------------------------------------
// static functions


// whether vScale is the unique scale (1, 1, 1), change the epsilon here
inline static bool EqualUniform1Scale(const NxVec3& vScale)
{
	return vScale.equals(NxVec3(1), 0.001f);
}

inline static bool IsUniformScale(const NxVec3& vScale)
{
	//??? efficiency and divided by zero error?
	return EqualUniform1Scale(vScale/vScale.x);
}

/// Identifiers of meshes are combined on the mesh name and the scale:
///	
///		MeshName				<no scale mentioned because it's 1 1 1>
///		MeshName<4.2,1.0,2.0>	<scale mentioned because it's not 1 1 1>
///
static AString GetMeshID(APhysXString MeshName, NxVec3 vScale)
{

	if(EqualUniform1Scale(vScale))
	{
		// if 3D scale is <1, 1, 1>, we just return the mesh name
		return MeshName;
	}
	else
	{
		// in other cases, we add the scale to the ID string
		AString strID(MeshName), strScale;
		strScale.Format("<%.1f,%.1f,%.1f>", vScale.x, vScale.y, vScale.z);
		strID += strScale;

		return strID;

	}
}

//----------------------------------------------------------------------

bool APhysXMeshCooker::Init(APhysXEngine* pPhysXEngine)
{
	if (0 == pPhysXEngine)
		return false;

	if (pPhysXEngine->IsPhysXDisbaled())
		return false;

	m_pNxCookingLib = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	if(m_pNxCookingLib)
	{
		if(m_pNxCookingLib->NxInitCooking(pPhysXEngine->GetAllocator(), pPhysXEngine->GetOutputStream()))
		{
			m_pNxPhysicsSDK = pPhysXEngine->GetNxPhysicsSDK();
			m_pAPhysXEngine = pPhysXEngine;
			return true;
		}
		else
		{

			gPhysXLog->Log("Error: Unable to initialize the PhysX Cooking Lib.");
			return false;
		}
	}
	else
	{
		gPhysXLog->Log("Error: Unable to retrieve PhysX Cooking Lib Error!");
		return false;
	}

}

void APhysXMeshCooker::Reset()
{
	// firstly, release the mesh 
	for(ConvexMeshMap::iterator it = m_mapConvexMeshes.begin(); it != m_mapConvexMeshes.end(); ++it)
	{
		NxConvexMesh* pConvexMesh = it->second;
		
		if(pConvexMesh->getReferenceCount()>0)
		{
			gPhysXLog->Log("Warning: Can't release convex mesh since it still has reference!");
		}
		else
			m_pNxPhysicsSDK->releaseConvexMesh(*pConvexMesh);
	}

	for(ClothMeshMap::iterator it = m_mapClothMeshes.begin(); it != m_mapClothMeshes.end(); ++it)
	{
		NxClothMesh* pClothMesh = it->second;

		if(pClothMesh->getReferenceCount()>0)
		{
			gPhysXLog->Log("Warning: Can't release cloth mesh since it still has reference!");
		}
		else
			m_pNxPhysicsSDK->releaseClothMesh(*pClothMesh);
	}

	for(TriangleMeshMap::iterator it = m_mapTriangleMeshes.begin(); it != m_mapTriangleMeshes.end(); ++it)
	{
		NxTriangleMesh* pTriangleMesh = it->second;

		if(pTriangleMesh->getReferenceCount()>0)
		{
			gPhysXLog->Log("Warning: Can't release triangle mesh since it still has reference!");
		}
		else	
			m_pNxPhysicsSDK->releaseTriangleMesh(*pTriangleMesh);
	}

	for(SoftBodyMeshMap::iterator it = m_mapSoftBodyMeshes.begin(); it != m_mapSoftBodyMeshes.end(); ++it)
	{
		NxSoftBodyMesh* pSBMesh = it->second;

		if(pSBMesh->getReferenceCount()>0)
		{
			gPhysXLog->Log("Warning: Can't release softbody mesh since it still has reference!");
		}
		else
			m_pNxPhysicsSDK->releaseSoftBodyMesh(*pSBMesh);
	}


	// clear all mesh maps
	m_mapTriangleMeshes.clear();
	m_mapConvexMeshes.clear();
	m_mapClothMeshes.clear();
	m_mapSoftBodyMeshes.clear();

}

void APhysXMeshCooker::ReleaseObsoleteMesh()
{
#ifdef USE_STD_TYPE
	

	// to do... how to delete some items inside the scope of for(...){...} as for the case of std::map...
	APHYSX_ASSERT(0);

#else

	//----------------------------------------------------------------------------------
	// Note: Dec. 16, 2008
	// The obsolete implement only release the meshes which have names, while, as for those without
	// names, we can't release them in this way. So, we try the new method which keep a cached release
	// mesh array. Here, we will check to release the items in it....
	// ------------------
	// And beware how to delete/erase some elements in a for-loop code...
	// You should better use the backward order to release the item...
	//----------------------------------------------------------------------------------

	for(int i= m_arrCachedReleaseConvexMeshes.size()-1; i>=0; i--)
	{
		NxConvexMesh* pMesh = m_arrCachedReleaseConvexMeshes[i];
		if(pMesh->getReferenceCount() == 0)
		{
			m_pNxPhysicsSDK->releaseConvexMesh(*pMesh);
			m_arrCachedReleaseConvexMeshes.deleteEntry(pMesh);
		}
	}

	for(int i= m_arrCachedReleaseClothMeshes.size()-1; i>=0; i--)
	{
		NxClothMesh* pMesh = m_arrCachedReleaseClothMeshes[i];
		if(pMesh->getReferenceCount() == 0)
		{
			m_pNxPhysicsSDK->releaseClothMesh(*pMesh);
			m_arrCachedReleaseClothMeshes.deleteEntry(pMesh);
		}
	}

	for(int i= m_arrCachedReleaseTriangleMeshes.size()-1; i>=0; i--)
	{
		NxTriangleMesh* pMesh = m_arrCachedReleaseTriangleMeshes[i];
		if(pMesh->getReferenceCount() == 0)
		{
			m_pNxPhysicsSDK->releaseTriangleMesh(*pMesh);
			m_arrCachedReleaseTriangleMeshes.deleteEntry(pMesh);
		}
	}

	for(int i= m_arrCachedReleaseSoftBodyMeshes.size()-1; i>=0; i--)
	{
		NxSoftBodyMesh* pMesh = m_arrCachedReleaseSoftBodyMeshes[i];
		if(pMesh->getReferenceCount() == 0)
		{
			m_pNxPhysicsSDK->releaseSoftBodyMesh(*pMesh);
			m_arrCachedReleaseSoftBodyMeshes.deleteEntry(pMesh);
		}
	}

/*

	for(ConvexMeshMap::iterator it = m_mapConvexMeshes.begin(); it != m_mapConvexMeshes.end(); )
	{
		NxConvexMesh* pConvexMesh = it->second;

		if(pConvexMesh->getReferenceCount() == 0)
		{
			m_pNxPhysicsSDK->releaseConvexMesh(*pConvexMesh);
			it = m_mapConvexMeshes.erase(it);
		}
		else
		{
			++it;
		}
			
	}

	for(ClothMeshMap::iterator it = m_mapClothMeshes.begin(); it != m_mapClothMeshes.end(); ++it)
	{
		NxClothMesh* pClothMesh = it->second;

		if(pClothMesh->getReferenceCount() == 0)
		{
			m_pNxPhysicsSDK->releaseClothMesh(*pClothMesh);
			it = m_mapClothMeshes.erase(it);
		}
		else
		{
			++it;
		}			
	}

	for(TriangleMeshMap::iterator it = m_mapTriangleMeshes.begin(); it != m_mapTriangleMeshes.end(); ++it)
	{
		NxTriangleMesh* pTriangleMesh = it->second;

		if(pTriangleMesh->getReferenceCount() == 0)
		{
			m_pNxPhysicsSDK->releaseTriangleMesh(*pTriangleMesh);
			it = m_mapTriangleMeshes.erase(it);
		}
		else	
		{
			++it;
		}			
			
	}

	for(SoftBodyMeshMap::iterator it = m_mapSoftBodyMeshes.begin(); it != m_mapSoftBodyMeshes.end(); ++it)
	{
		NxSoftBodyMesh* pSBMesh = it->second;

		if(pSBMesh->getReferenceCount()>0)
		{
			m_pNxPhysicsSDK->releaseSoftBodyMesh(*pSBMesh);
			it = m_mapSoftBodyMeshes.erase(it);
		}
		else
		{
			++it;
		}			
	}
*/


#endif	
}

void APhysXMeshCooker::Release()
{
	
	ReleaseObsoleteMesh();

	Reset();

	if(m_pNxCookingLib)
	{
		m_pNxCookingLib->NxCloseCooking();
		m_pNxCookingLib = NULL;
	}
}


bool APhysXMeshCooker::CookConvexMesh(const NxConvexMeshDesc& convexMeshDesc, NxStream& myNxStream)
{
	if(m_pNxCookingLib)
		return m_pNxCookingLib->NxCookConvexMesh(convexMeshDesc, myNxStream);
	else
		return false;
}

bool APhysXMeshCooker::CookClothMesh(const NxClothMeshDesc& clothMeshDesc, NxStream& myNxStream)
{
	if(m_pNxCookingLib)
		return m_pNxCookingLib->NxCookClothMesh(clothMeshDesc, myNxStream);
	else
		return false;

}

inline NxConvexMesh* APhysXMeshCooker::GetScaledConvexMesh(APhysXMemoryReadBuffer& apxMemReadBuf, float fScale)
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
		return 0;

	// scale the convex mesh stream
#define TMP_EPSILON 0.05f

	if(fScale > 1 + TMP_EPSILON || fScale < 1 - TMP_EPSILON )
	{
		APhysXMemoryWriteBuffer apxMemWriteBuf;
		bool bRes = m_pNxCookingLib->NxScaleCookedConvexMesh(apxMemReadBuf, fScale, apxMemWriteBuf);
		APHYSX_ASSERT(bRes);
		return m_pNxPhysicsSDK->createConvexMesh(APhysXMemoryReadBuffer(apxMemWriteBuf.data));
	}
	else
	{
		// no scaling needed...
		return m_pNxPhysicsSDK->createConvexMesh(apxMemReadBuf);

	}

#undef TMP_EPSILON

}

NxConvexMesh* APhysXMeshCooker::GetConvexMesh(APhysXMemoryReadBuffer& apxMemReadBuf, const char* szName, float fScale)
{
	if(!m_pNxCookingLib) return NULL;

	NxConvexMesh* pNxConvexMesh = NULL;

	// firstly, we check the szName of apxConvexShapeDesc...
	if(szName == NULL)
	{
		// no name case, we just create a new convex mesh
		pNxConvexMesh = GetScaledConvexMesh(apxMemReadBuf, fScale);

	}
	else
	{
		// if having name, we will try to search it in the map firstly
		APhysXString strMeshID(szName);
		APhysXString strScale;
		strScale.Format("<%.1f>", fScale);
		strMeshID += strScale;
		
		m_mutexConvexMeshes.Lock();
		ConvexMeshMap::iterator it = m_mapConvexMeshes.find(strMeshID);
		if(it != m_mapConvexMeshes.end())
		{
			m_mutexConvexMeshes.Unlock();
			return it->second;
		}

		// can not find it, then, we add a new item to the map
		pNxConvexMesh = GetScaledConvexMesh(apxMemReadBuf, fScale);

		if(pNxConvexMesh)
		{
			m_mapConvexMeshes.insert(ConvexMeshPair(strMeshID, pNxConvexMesh));
		}

		m_mutexConvexMeshes.Unlock();
	}

	if(!pNxConvexMesh)
	{
		gPhysXLog->Log("Warning: Failed on creating NxConvexMesh: create a NULL NxConvexMesh from buffer stream.");	
	}

	return pNxConvexMesh;

}

NxClothMesh* APhysXMeshCooker::GetClothMesh(APhysXMemoryReadBuffer& apxMemReadBuf, const char* szName, float fScale)
{
	// at present, we just ignore the parameter "fScale"

	if(!m_pNxCookingLib) return NULL;

	NxClothMesh* pNxClothMesh = NULL;

	// firstly, we check the szName of apxConvexShapeDesc...
	if(szName == NULL)
	{
		// no name case, we just create a new convex mesh
		pNxClothMesh = m_pNxPhysicsSDK->createClothMesh(apxMemReadBuf);
	}
	else
	{
		// if having name, we will try to search it in the map firstly
		APhysXString strMeshID(szName);

		m_mutexClothMeshes.Lock();
		ClothMeshMap::iterator it = m_mapClothMeshes.find(strMeshID);
		if(it != m_mapClothMeshes.end())
		{
			m_mutexClothMeshes.Unlock();
			return it->second;
		}

		// can not find it, then, we add a new item to the map

		// no scaling at present
		pNxClothMesh = m_pNxPhysicsSDK->createClothMesh(apxMemReadBuf);

		if(pNxClothMesh)
		{
			m_mapClothMeshes.insert(ClothMeshPair(strMeshID, pNxClothMesh));
		}
		m_mutexClothMeshes.Unlock();
	}

	if(!pNxClothMesh)
	{
		gPhysXLog->Log("Warning: Failed on creating NxClothMesh: create a NULL NxClothMesh from buffer stream.");	
	}

	return pNxClothMesh;
}


void APhysXMeshCooker::ForceReleaseConvexMesh(NxConvexMesh* pConvexMesh)
{
	//??? there seems to be some efficiency issue since the following operation 
	// first, we remove it from the map
	for(ConvexMeshMap::iterator it = m_mapConvexMeshes.begin(); it != m_mapConvexMeshes.end(); ++it)
	{
		if(pConvexMesh == it->second)
		{
			m_mapConvexMeshes.erase(it);
			break;
		}

	}		

	// then, we try to release the mesh...

	// Note: Dec.17, 2008. Wenfeng
	// In the new implement for multi-thread support, we always do the real release of 
	// the object in the APhysXScene's simulate function, so this result in that the 
	// following condition won't be satisfied all the time ...
	if(pConvexMesh->getReferenceCount() == 0)
	{
		m_pAPhysXEngine->GetNxPhysicsSDK()->releaseConvexMesh(*pConvexMesh);
	}
	else
	{
		APhysXUtilLib::APhysXArray_PushOnce(m_arrCachedReleaseConvexMeshes, pConvexMesh);

		// gPhysXLog->Log("Can't release convex mesh since it still has reference!");
	}
}

void APhysXMeshCooker::ForceReleaseClothMesh(NxClothMesh* pClothMesh)
{
	// remove it from the map
	for(ClothMeshMap::iterator it = m_mapClothMeshes.begin(); it != m_mapClothMeshes.end(); ++it)
	{
		if(pClothMesh == it->second)
		{
			m_mapClothMeshes.erase(it);
			break;
		}

	}		

	// then, we try to release the mesh...

	// Note: Dec.17, 2008. Wenfeng
	// In the new implement for multi-thread support, we always do the real release of 
	// the object in the APhysXScene's simulate function, so this result in that the 
	// following condition won't be satisfied all the time ...

	if(pClothMesh->getReferenceCount() == 0)
	{
		m_pAPhysXEngine->GetNxPhysicsSDK()->releaseClothMesh(*pClothMesh);

	}
	else
	{
		APhysXUtilLib::APhysXArray_PushOnce(m_arrCachedReleaseClothMeshes, pClothMesh);
		// gPhysXLog->Log("Can't release cloth mesh since it still has reference!");
	}

}

bool APhysXMeshCooker::TryReleaseConvexMesh(NxConvexMesh* pConvexMesh)
{
	if(pConvexMesh->getReferenceCount() == 0)
	{
		ForceReleaseConvexMesh(pConvexMesh);
		return true;
	}
	else
		return false;
	
}


bool APhysXMeshCooker::TryReleaseClothMesh(NxClothMesh* pClothMesh)
{
	if(pClothMesh->getReferenceCount() == 0)
	{
		ForceReleaseClothMesh(pClothMesh);
		return true;
	}
	else
		return false;

}

//-----------------------------------------------------------------------------------------------
// obsolete interface...
/*

NxConvexMesh* APhysXMeshCooker::GetConvexMesh(APhysXConvexShapeDesc* pAPXConvexShapeDesc, float fScale)
{
	if(!m_pNxCookingLib) return NULL;

	NxConvexMesh* pNxConvexMesh = NULL;

	// firstly, we check the szName of apxConvexShapeDesc...
	if(pAPXConvexShapeDesc->GetName() == NULL)
	{
		// no name case, we just create a new convex mesh
		APhysXMemoryReadBuffer apxMemReadBuf;
		pAPXConvexShapeDesc->GetMemoryReadBuf(apxMemReadBuf);
		pNxConvexMesh = m_pNxPhysicsSDK->createConvexMesh(apxMemReadBuf);

	}
	else
	{
		// if having name, we will try to search it in the map firstly
		APhysXString strMeshID(pAPXConvexShapeDesc->GetName());
		APhysXString strScale;
		strScale.Format("<%.1f>", fScale);
		strMeshID += strScale;
		ConvexMeshMap::iterator it = m_mapConvexMeshes.find(strMeshID);
		if(it != m_mapConvexMeshes.end())
		{
			return it->second;
		}
		
		// can not find it, then, we add a new item to the map
		APhysXMemoryReadBuffer apxMemReadBuf;
		pAPXConvexShapeDesc->GetMemoryReadBuf(apxMemReadBuf);

		// scale the convex mesh stream
#define TMP_EPSILON 0.05f
		
		if(fScale > 1 + TMP_EPSILON || fScale < 1 - TMP_EPSILON )
		{
			APhysXMemoryWriteBuffer apxMemWriteBuf;
			bool bRes = m_pNxCookingLib->NxScaleCookedConvexMesh(apxMemReadBuf, fScale, apxMemWriteBuf);
			APHYSX_ASSERT(bRes);
			pNxConvexMesh = m_pNxPhysicsSDK->createConvexMesh(APhysXMemoryReadBuffer(apxMemWriteBuf.data));
		}
		else
		{
			// no scaling needed...
			pNxConvexMesh = m_pNxPhysicsSDK->createConvexMesh(apxMemReadBuf);

		}

#undef TMP_EPSILON
		

		if(pNxConvexMesh)
		{
			m_mapConvexMeshes.insert(ConvexMeshPair(strMeshID, pNxConvexMesh));
		}
	}

	if(!pNxConvexMesh)
	{
		gPhysXLog->Log("Cook Null Convex Mesh!");	
	}

	return pNxConvexMesh;

}

NxClothMesh* APhysXMeshCooker::GetClothMesh(APhysXClothObjectDesc* pAPXConvexShapeDesc, float fScale)
{
	// at present, we just ignore the parameter "fScale"


	return NULL;
}



NxConvexMesh* APhysXMeshCooker::CookConvexMesh(const NxConvexMeshDesc& ConvexMeshDesc,NxReal rUniformScale)
{
	if(!m_pNxCookingLib) return NULL;

	APhysXMemoryWriteBuffer buf;
	bool bRes = m_pNxCookingLib->NxCookConvexMesh(ConvexMeshDesc, buf);
	if(bRes)
	{
		NxConvexMesh* pConvexMesh;
		if(rUniformScale == 1)
		{
			pConvexMesh = m_pNxPhysicsSDK->createConvexMesh(APhysXMemoryReadBuffer(buf.data));
		}
		else
		{
			APhysXMemoryWriteBuffer bufScaled;
			m_pNxCookingLib->NxScaleCookedConvexMesh(buf, rUniformScale, bufScaled);
			pConvexMesh = m_pNxPhysicsSDK->createConvexMesh(APhysXMemoryReadBuffer(bufScaled.data));
		}

		return pConvexMesh;
	}
	else
	{
		gPhysXLog->Log("Cook Convex Mesh Error!");
		return NULL;
	}

}

NxConvexMesh* APhysXMeshCooker::GetConvexMesh(const NxConvexMeshDesc& ConvexMeshDesc, APhysXString MeshName, NxVec3 vScale)
{
	if(!m_pNxCookingLib) return NULL;

	//first, search the mesh in map
	AString MeshID = GetMeshID(MeshName, vScale);

	ConvexMeshMap::iterator it = m_mapConvexMeshes.find(MeshID);
	if(it != m_mapConvexMeshes.end())
	{
		return it->second;
	}
	else
	{
		// cook the mesh and insert it to mesh pool
		NxConvexMesh* pConvexMesh = NULL;

		if(EqualUniform1Scale(vScale))
		{
			pConvexMesh = CookConvexMesh(ConvexMeshDesc);
		}
		else if(IsUniformScale(vScale))
		{
			pConvexMesh = CookConvexMesh(ConvexMeshDesc, vScale.x);
		}
		else
		{
			// other cases, we have to scale convex mesh data
			NxConvexMeshDesc ScaledCovexMeshDesc(ConvexMeshDesc);
			ScaledCovexMeshDesc.pointStrideBytes = sizeof(NxVec3);

			NxVec3 * ScaledPoints = new NxVec3[ConvexMeshDesc.numVertices];
			const NxReal* Pos = static_cast<const NxReal* >(ConvexMeshDesc.points);
			const NxU8* uPos = static_cast<const NxU8* >(ConvexMeshDesc.points);
			for(int i=0; i<ConvexMeshDesc.numVertices; i++)
			{
				ScaledPoints[i].set(Pos[0] * vScale.x, Pos[1] * vScale.y, Pos[2] * vScale.z);
				uPos += ConvexMeshDesc.pointStrideBytes;
				Pos = (const NxReal* )(uPos);
			}
			ScaledCovexMeshDesc.points = ScaledPoints;
			
			pConvexMesh = CookConvexMesh(ScaledCovexMeshDesc);
		}

		if(pConvexMesh)
		{
			// insert to the mesh map
			m_mapConvexMeshes.insert(ConvexMeshPair(MeshID, pConvexMesh));
		}

#ifdef APHYSX_DEBUG
		else
		{
			gPhysXLog->Log("Cook Null Convex Mesh!");	
		}

#endif
		return pConvexMesh;

	}

}

NxTriangleMesh* APhysXMeshCooker::GetTriangleMesh(const NxTriangleMeshDesc& ConvexMeshDesc, APhysXString MeshName, NxVec3 vScale)
{
	// to do

	return NULL;
}

NxClothMesh* APhysXMeshCooker::GetClothMesh(const NxClothMeshDesc& ConvexMeshDesc, APhysXString MeshName, NxVec3 vScale)
{
	// to do

	return NULL;
}

NxSoftBodyMesh* GetSoftBodyMesh(const NxSoftBodyMeshDesc& ConvexMeshDesc, AString MeshName, NxVec3 vScale)
{
	// to do

}

*/