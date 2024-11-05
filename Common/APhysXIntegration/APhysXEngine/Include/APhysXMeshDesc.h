/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, May.3, 2011


	APhysXMeshDesc: 
		
		define the mesh data which are independent of PhysX Cooking lib

----------------------------------------------------------------------*/

#ifndef	_APHYSXMESHDESC_H_
#define _APHYSXMESHDESC_H_


class APhysXSimpleMeshDesc : public APhysXSerializerBase
{
	DeclareVersion(0xcc000001);

public:

	APhysXSimpleMeshDesc()
	{	
		mVertexNum = 0;
		mTriangleNum = 0;

		mVertexBuf = NULL;
		mTriIndexBuf = NULL;

		mFlags = 0;
	}

	virtual ~APhysXSimpleMeshDesc() { Release(); } 
	virtual void Release();

	const APhysXSimpleMeshDesc& operator= (const APhysXSimpleMeshDesc& other);

	void SetNxConvexMeshDesc(const NxConvexMeshDesc& convexMeshDesc);
	void GetNxConvexMeshDesc(NxConvexMeshDesc& convexMeshDesc) const;

	void SetNxClothMeshDesc(const NxClothMeshDesc& clothMeshDesc);
	void GetNxClothMeshDesc(NxClothMeshDesc& clothMeshDesc) const;

protected:

	void InitBuf(APhysXU32 uVertNum, APhysXU32 uTriNum);

protected:

	APhysXU32 mVertexNum;
	APhysXU32 mTriangleNum;

	APhysXU8* mVertexBuf;
	APhysXU8* mTriIndexBuf;

	APhysXU32 mFlags;

};





#endif