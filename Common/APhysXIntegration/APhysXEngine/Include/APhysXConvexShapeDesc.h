/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCONVEXSHAPEDESC_H_
#define _APHYSXCONVEXSHAPEDESC_H_

class APhysXMemoryWriteBuffer;
class APhysXMemoryReadBuffer;

class APHYSX_DLL_EXPORT APhysXConvexShapeDesc:public APhysXShapeDesc
{
	//-------------------------------------------------------
	// version ++ : 
	//		to support different cook lib version
	DeclareVersion(0xcc000002);
	// DeclareVersion(0xcc000001);

	DeclareShapeType(APX_SHAPETYPE_CONVEX);

/*
	friend class APhysXShapeDescManager;

protected:
*/
public:

	APhysXConvexShapeDesc()
	{
		SetToDefault();
	}

	~APhysXConvexShapeDesc() { Release();}

	void Release();


	const APhysXConvexShapeDesc& operator= (const APhysXConvexShapeDesc& other);

	virtual void SetToDefault()
	{
		APhysXShapeDesc::SetToDefault();

		mConvexMeshFlags = 0;
		mConvexMeshBufSize = 0;
		mConvexMeshBuf = NULL;

		mCookLibVersion = 0;
	}

	bool InitConvexMesh(const NxConvexMeshDesc& convexMeshDesc); 


	// to be aware that this class just new the pointer
	// to NxShapeDesc but not save it, so save and release
	// of the pointer is responsibility of APhysXShapeDescManager
	virtual NxShapeDesc* CreateNxShapeDesc(const NxVec3& vSacle3D = NxVec3(1.0f))
	{
		NxShapeDesc* pNxShapeDesc = new NxConvexShapeDesc;
		GetNxShapeDesc(*pNxShapeDesc, vSacle3D);
		return pNxShapeDesc;
	}


	virtual void GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D = NxVec3(1.0f));

	// virtual interface
	virtual APhysXShapeDesc* Clone() const
	{
		APhysXShapeDesc* pPhysXShapeDesc = new APhysXConvexShapeDesc;
		*(APhysXConvexShapeDesc *)pPhysXShapeDesc = *this;
		return pPhysXShapeDesc;
	}

protected:

	void SetConvexMeshBuf(APhysXMemoryWriteBuffer& apxMemWriteBuf);

	void GetMemoryReadBuf(APhysXMemoryReadBuffer& apxMemReadBuf);

	// properties are public for direct access
public:
	APhysXU32 mConvexMeshFlags;

protected:

	// to save the cooked convex mesh
	APhysXU32 mConvexMeshBufSize;
	APhysXU8* mConvexMeshBuf;

	//----------------------------------------------------------------------
	// for compatible of different cooking formats...
	APhysXU32 mCookLibVersion;
	APhysXSimpleMeshDesc mConvexMeshDesc;

	// keep record of convex mesh resources...
	APhysXArray<NxConvexMesh* > mConvexMeshes;
};




#endif
