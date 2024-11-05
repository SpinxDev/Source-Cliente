/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXMESHCOOKER_H_
#define _APHYSXMESHCOOKER_H_

class APhysXEngine;

class APHYSX_DLL_EXPORT APhysXMeshCooker
{

public:
	APhysXMeshCooker()
	{
		m_pNxCookingLib = NULL;
		m_pNxPhysicsSDK = NULL;
		m_pAPhysXEngine = NULL;
	}

	~APhysXMeshCooker(){}

	// call at the start to init lib
	bool Init(APhysXEngine* pPhysXEngine);
	// call manually to close lib
	void Release();

	// call to release some obsolete mesh resources...
	void ReleaseObsoleteMesh();

	// just release all data in the map, this operation will clear all map items...
	void Reset();
	
	bool IsValid() { return (m_pNxCookingLib != NULL); }

	// simple interface
	bool CookConvexMesh(const NxConvexMeshDesc& convexMeshDesc, NxStream& myNxStream);
	bool CookClothMesh(const NxClothMeshDesc& clothMeshDesc, NxStream& myNxStream);

	NxConvexMesh* GetConvexMesh(APhysXMemoryReadBuffer& apxMemReadBuf, const char* szName = NULL, float fScale = 1.0f);
	NxClothMesh* GetClothMesh(APhysXMemoryReadBuffer& apxMemReadBuf, const char* szName = NULL, float fScale = 1.0f);

	// force to release the mesh resources...
	void ForceReleaseConvexMesh(NxConvexMesh* pConvexMesh);
	void ForceReleaseClothMesh(NxClothMesh* pClothMesh);

	// test and then release the mesh resources...
	bool TryReleaseConvexMesh(NxConvexMesh* pConvexMesh);
	bool TryReleaseClothMesh(NxClothMesh* pClothMesh);

//-----------------------------------------------------------------
// the interfaces which is based on NxConvexMeshDesc...	
// now they are obsolete...
/*
	NxConvexMesh* GetConvexMesh(APhysXConvexShapeDesc* pAPXConvexShapeDesc, float fScale = 1.0f);
	NxClothMesh* GetClothMesh(APhysXClothObjectDesc* pAPXConvexShapeDesc, float fScale = 1.0f);

	NxConvexMesh* GetConvexMesh(const NxConvexMeshDesc& ConvexMeshDesc, APhysXString MeshName, NxVec3 vScale = NxVec3(1));
	NxTriangleMesh* GetTriangleMesh(const NxTriangleMeshDesc& ConvexMeshDesc, APhysXString MeshName, NxVec3 vScale = NxVec3(1));
	NxClothMesh* GetClothMesh(const NxClothMeshDesc& ConvexMeshDesc, APhysXString MeshName, NxVec3 vScale = NxVec3(1));
	NxSoftBodyMesh* GetSoftBodyMesh(const NxSoftBodyMeshDesc& ConvexMeshDesc, APhysXString MeshName, NxVec3 vScale = NxVec3(1));

protected:
	NxConvexMesh* CookConvexMesh(const NxConvexMeshDesc& ConvexMeshDesc, NxReal rUniformScale = 1);

*/
//-----------------------------------------------------------------

protected:

	NxConvexMesh* GetScaledConvexMesh(APhysXMemoryReadBuffer& apxMemReadBuf, float fScale);

protected:

	NxCookingInterface* m_pNxCookingLib;
	NxPhysicsSDK* m_pNxPhysicsSDK;
	APhysXEngine* m_pAPhysXEngine;

	//-----------------------------------------------------------------
	// define new types...

	typedef APhysXMap<APhysXString, NxTriangleMesh*> TriangleMeshMap;
	typedef APhysXMap<APhysXString, NxConvexMesh*> ConvexMeshMap;
	typedef APhysXMap<APhysXString, NxClothMesh*> ClothMeshMap;
	typedef APhysXMap<APhysXString, NxSoftBodyMesh*> SoftBodyMeshMap;

	// prefer define as following ...
	typedef TriangleMeshMap::value_type TriangleMeshPair;
	typedef ConvexMeshMap::value_type ConvexMeshPair;
	typedef ClothMeshMap::value_type ClothMeshPair;
	typedef SoftBodyMeshMap::value_type SoftBodyMeshPair;

	//--------------------------------------------------
	// to do...  Dec.16,2008, Wenfeng
	// multi-thread support...

	TriangleMeshMap m_mapTriangleMeshes;
	ConvexMeshMap m_mapConvexMeshes;
	ClothMeshMap m_mapClothMeshes;
	SoftBodyMeshMap m_mapSoftBodyMeshes;

	APhysXMutex m_mutexTriangleMeshes;
	APhysXMutex m_mutexConvexMeshes;
	APhysXMutex m_mutexClothMeshes;
	APhysXMutex m_mutexSBMeshes;


	APhysXArray<NxConvexMesh*> m_arrCachedReleaseConvexMeshes;
	APhysXArray<NxClothMesh*> m_arrCachedReleaseClothMeshes;
	APhysXArray<NxTriangleMesh*> m_arrCachedReleaseTriangleMeshes;
	APhysXArray<NxSoftBodyMesh*> m_arrCachedReleaseSoftBodyMeshes;

};



#endif