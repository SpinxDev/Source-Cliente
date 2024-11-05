
#include "APhysXStdAfx.h"

bool APhysXConvexShapeDesc::Save(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Save(pNxStream))
		return false;
	
	// save my version...
	SaveVersion(pNxStream);

	pNxStream->storeDword(mConvexMeshFlags);
	pNxStream->storeDword(mConvexMeshBufSize);

	if(mConvexMeshBufSize && mConvexMeshBuf)
	{
		pNxStream->storeBuffer(mConvexMeshBuf, mConvexMeshBufSize);
	}
	
	pNxStream->storeDword(mCookLibVersion);
	mConvexMeshDesc.Save(pNxStream);

	return true;

}

bool APhysXConvexShapeDesc::Load(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Load(pNxStream))
		return false;

	Release();

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer >= 0xcc000001)
	{
		mConvexMeshFlags = pNxStream->readDword();
		mConvexMeshBufSize = pNxStream->readDword();

		if(mConvexMeshBufSize)
		{
			mConvexMeshBuf = new APhysXU8[mConvexMeshBufSize];
			pNxStream->readBuffer(mConvexMeshBuf, mConvexMeshBufSize);
		}

		//--------------------------------------------------------
		// Note: wenfeng, Sept. 11, 2011
		// here is a little trick since the PhysX 281 and 284 have the same convex cooking results
		// old cooking data are under the version of 2.8.1
		// mCookLibVersion = APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 1);
		mCookLibVersion = APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4);
	}

	if(m_StreamObjVer >= 0xcc000002)
	{
		mCookLibVersion = pNxStream->readDword();
		mConvexMeshDesc.Load(pNxStream);

		//--------------------------------------------------------
		// Note: wenfeng, Sept. 11, 2011
		// here is a little trick since the PhysX 281 and 284 have the same convex cooking results
		// old cooking data are under the version of 2.8.1
		mCookLibVersion = APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4);
	}

	// re-cooking...
	if(mCookLibVersion != APHYSX_CURRENT_PHYSX_SDK_VERSION)
	{
		// we have to reconstruct the cooked convex stream...

		gPhysXLog->Log("Note: Need to recook the convex mesh since cook lib version has changed!");

		NxConvexMeshDesc convexMeshDesc;
		mConvexMeshDesc.GetNxConvexMeshDesc(convexMeshDesc);
		// force to re-compute the convex?...
		// convexMeshDesc.flags |= NX_CF_COMPUTE_CONVEX;

		APhysXMemoryWriteBuffer buf;
		if(gPhysXEngine->GetMeshCooker()->CookConvexMesh(convexMeshDesc, buf))
		{
			SetConvexMeshBuf(buf);
		}
		else
		{
			gPhysXLog->Log("Warning: Failed on recooking convex mesh!");
			return false;
		}
	}

	return true;
}

void APhysXConvexShapeDesc::GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D)
{
	APHYSX_ASSERT(NxMath::equals(vSacle3D.x, vSacle3D.y, APHYSX_SCALE_EPSILON) && NxMath::equals(vSacle3D.x, vSacle3D.z, APHYSX_SCALE_EPSILON));
	APhysXShapeDesc::GetNxShapeDesc(InNxShapeDesc, vSacle3D);
	NxConvexShapeDesc& ConvexShapeDesc = (NxConvexShapeDesc&) InNxShapeDesc;
	ConvexShapeDesc.meshFlags = mConvexMeshFlags;

	//---------------------------------------------------------
	// PhysX Bug found by wenfeng, Aug 18, 2009
	// Scaling the convex mesh method do not change the volume of the convex shape,
	// as a result, the mass has not be scaled yet.
	// This is just a temporary walk around...
	ConvexShapeDesc.density = vSacle3D.x * vSacle3D.x * vSacle3D.x;

	// try to create ConvexMesh via the cooked mesh data
	APhysXMemoryReadBuffer apxMemReadBuf;
	GetMemoryReadBuf(apxMemReadBuf);
	ConvexShapeDesc.meshData = gPhysXEngine->GetMeshCooker()->GetConvexMesh(apxMemReadBuf, GetName(), vSacle3D.x);

	// keep record of those convex meshes I created...
	if(ConvexShapeDesc.meshData)
	{
		APhysXUtilLib::APhysXArray_PushOnce(mConvexMeshes, ConvexShapeDesc.meshData);
	}

}

bool APhysXConvexShapeDesc::InitConvexMesh(const NxConvexMeshDesc& convexMeshDesc)
{

	APhysXMemoryWriteBuffer buf;
	if(gPhysXEngine->GetMeshCooker()->CookConvexMesh(convexMeshDesc, buf))
	{
		SetConvexMeshBuf(buf);

		mCookLibVersion = APHYSX_CURRENT_PHYSX_SDK_VERSION;
		mConvexMeshDesc.SetNxConvexMeshDesc(convexMeshDesc);

		return true;
	}
	else 
		return false;
}


void APhysXConvexShapeDesc::SetConvexMeshBuf(APhysXMemoryWriteBuffer& apxMemWriteBuf)
{
	Release();
	mConvexMeshBufSize = apxMemWriteBuf.currentSize;
	mConvexMeshBuf = new APhysXU8[mConvexMeshBufSize];

	memcpy(mConvexMeshBuf, apxMemWriteBuf.data, mConvexMeshBufSize);

}

void APhysXConvexShapeDesc::GetMemoryReadBuf(APhysXMemoryReadBuffer& apxMemReadBuf)
{
	apxMemReadBuf.buffer = mConvexMeshBuf;
}


void APhysXConvexShapeDesc::Release() 
{
	if(mConvexMeshBuf)
	{
		delete [] mConvexMeshBuf;
		mConvexMeshBuf = NULL;
		mConvexMeshBufSize = 0;
	}

	// force to release all convex meshes, how about the temp object desc???
	for(int i=0; i < mConvexMeshes.size(); i++)
	{
		gPhysXEngine->GetMeshCooker()->TryReleaseConvexMesh(mConvexMeshes[i]);
	}
	mConvexMeshes.clear();
}

const APhysXConvexShapeDesc& APhysXConvexShapeDesc::operator= (const APhysXConvexShapeDesc& other)
{
	// copy the data of super class...
	APhysXShapeDesc& mySuper = *(APhysXShapeDesc* )this;
	APhysXShapeDesc& otherSuper = *(APhysXShapeDesc* )(&other);
	mySuper = otherSuper;

	// then copy the buffer...
	Release();

	mConvexMeshFlags = other.mConvexMeshFlags;
	mConvexMeshBufSize = other.mConvexMeshBufSize;
	mConvexMeshBuf = new APhysXU8[mConvexMeshBufSize];
	memcpy(mConvexMeshBuf, other.mConvexMeshBuf, mConvexMeshBufSize);

	mCookLibVersion = other.mCookLibVersion;
	mConvexMeshDesc = other.mConvexMeshDesc;

	return *this;
}
