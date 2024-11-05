
#include "APhysXStdAfx.h"

void APhysXSimpleMeshDesc::Release()
{
	mVertexNum = 0;
	mTriangleNum = 0;

	if(mVertexBuf)
	{
		delete [] mVertexBuf;
		mVertexBuf = NULL;
	}

	if(mTriIndexBuf)
	{
		delete [] mTriIndexBuf;
		mTriIndexBuf = NULL;
	}

	mFlags = 0;
}


void APhysXSimpleMeshDesc::InitBuf(APhysXU32 uVertNum, APhysXU32 uTriNum)
{
	Release();
	
	mVertexNum = uVertNum;
	mTriangleNum = uTriNum;

	if( mVertexNum > 0 )
	{
		APhysXU32 uSize = mVertexNum * sizeof(NxVec3);
		mVertexBuf = new APhysXU8[uSize];
	}

	if( mTriangleNum > 0)
	{
		APhysXU32 uSize = mTriangleNum * 3 * sizeof(APhysXU32);
		mTriIndexBuf = new APhysXU8[uSize];
	}

}


Begin_Save(APhysXSimpleMeshDesc, APhysXSerializerBase)

	pNxStream->storeDword(mVertexNum);
	pNxStream->storeDword(mTriangleNum);

	if( mVertexNum > 0 ) 
	{
		pNxStream->storeBuffer(mVertexBuf, mVertexNum * sizeof(NxVec3));
	}

	if( mTriangleNum > 0)
	{
		pNxStream->storeBuffer(mTriIndexBuf, mTriangleNum * 3 * sizeof(APhysXU32));
	}
	

	pNxStream->storeDword(mFlags);

End_Save()


Begin_Load(APhysXSimpleMeshDesc, APhysXSerializerBase)

	if(m_StreamObjVer >= 0xcc000001)
	{
		Release();

		mVertexNum = pNxStream->readDword();
		mTriangleNum = pNxStream->readDword();

		if( mVertexNum > 0 )
			
		{
			APhysXU32 uSize = mVertexNum * sizeof(NxVec3);
			mVertexBuf = new APhysXU8[uSize];
			pNxStream->readBuffer(mVertexBuf, uSize);

		}

		if( mTriangleNum > 0)
		{
			APhysXU32 uSize = mTriangleNum * 3 * sizeof(APhysXU32);
			mTriIndexBuf = new APhysXU8[uSize];
			pNxStream->readBuffer(mTriIndexBuf, uSize);

		}

		mFlags = pNxStream->readDword();

	}

End_Load()


const APhysXSimpleMeshDesc& APhysXSimpleMeshDesc::operator= (const APhysXSimpleMeshDesc& other)
{
	if(this == &other) return *this;

	Release();

	// clone parents' data
	(APhysXSerializerBase& ) *this = (APhysXSerializerBase& )other;


	// clone my data...
	mVertexNum = other.mVertexNum;
	mTriangleNum = other.mTriangleNum;
	mFlags = other.mFlags;

	if( mVertexNum > 0 ) 
	{
		APhysXU32 uSize = mVertexNum * sizeof(NxVec3);
		mVertexBuf = new APhysXU8[uSize];
		memcpy(mVertexBuf, other.mVertexBuf, uSize);
	}

	if(other.mTriangleNum > 0)
	{
		APhysXU32 uSize = mTriangleNum * 3 * sizeof(APhysXU32);
		mTriIndexBuf = new APhysXU8[uSize];
		memcpy(mTriIndexBuf, other.mTriIndexBuf, uSize);
	}

	return *this;

}


void APhysXSimpleMeshDesc::SetNxConvexMeshDesc(const NxConvexMeshDesc& convexMeshDesc)
{
	InitBuf(convexMeshDesc.numVertices, convexMeshDesc.numTriangles);
	mFlags = convexMeshDesc.flags;

	NxVec3* pDestVertex = (NxVec3*)mVertexBuf;
	const APhysXU8* pSrcVertex = (const APhysXU8*)convexMeshDesc.points;
	for(int i=0; i<convexMeshDesc.numVertices; i++)
	{
		*pDestVertex++ = * (NxVec3 *)pSrcVertex;
		pSrcVertex += convexMeshDesc.pointStrideBytes;
	}

	APhysXU32* pDestID = (APhysXU32 *)mTriIndexBuf;
	const APhysXU8* pSrcID = (const APhysXU8*)convexMeshDesc.triangles;

	for(int i=0; i<convexMeshDesc.numTriangles; i++)
	{
		APhysXU32* pSrcIDInU32 = (APhysXU32 *)pSrcID;
		
		*pDestID++ = * pSrcIDInU32;

		*pDestID++ = * (pSrcIDInU32 + 1);

		*pDestID++ = * (pSrcIDInU32 + 2);

		pSrcID += convexMeshDesc.triangleStrideBytes;
	}

}

void APhysXSimpleMeshDesc::GetNxConvexMeshDesc(NxConvexMeshDesc& convexMeshDesc) const
{
	convexMeshDesc.numVertices = mVertexNum;
	convexMeshDesc.numTriangles = mTriangleNum;

	convexMeshDesc.points = (const void *)mVertexBuf;
	convexMeshDesc.triangles = (const void *)mTriIndexBuf;

	convexMeshDesc.pointStrideBytes = sizeof(NxVec3);
	convexMeshDesc.triangleStrideBytes = 3 * sizeof(APhysXU32);

	convexMeshDesc.flags = mFlags;

}

void APhysXSimpleMeshDesc::SetNxClothMeshDesc(const NxClothMeshDesc& clothMeshDesc)
{

	InitBuf(clothMeshDesc.numVertices, clothMeshDesc.numTriangles);
	mFlags = clothMeshDesc.flags;

	NxVec3* pDestVertex = (NxVec3*)mVertexBuf;
	const APhysXU8* pSrcVertex = (const APhysXU8*)clothMeshDesc.points;
	for(int i=0; i<clothMeshDesc.numVertices; i++)
	{
		*pDestVertex++ = * (NxVec3 *)pSrcVertex;
		pSrcVertex += clothMeshDesc.pointStrideBytes;
	}

	APhysXU32* pDestID = (APhysXU32 *)mTriIndexBuf;
	const APhysXU8* pSrcID = (const APhysXU8*)clothMeshDesc.triangles;

	for(int i=0; i<clothMeshDesc.numTriangles; i++)
	{
		APhysXU32* pSrcIDInU32 = (APhysXU32 *)pSrcID;

		*pDestID++ = * pSrcIDInU32;

		*pDestID++ = * (pSrcIDInU32 + 1);

		*pDestID++ = * (pSrcIDInU32 + 2);

		pSrcID += clothMeshDesc.triangleStrideBytes;
	}

}

void APhysXSimpleMeshDesc::GetNxClothMeshDesc(NxClothMeshDesc& clothMeshDesc) const
{
	clothMeshDesc.numVertices = mVertexNum;
	clothMeshDesc.numTriangles = mTriangleNum;

	clothMeshDesc.points = (const void *)mVertexBuf;
	clothMeshDesc.triangles = (const void *)mTriIndexBuf;

	clothMeshDesc.pointStrideBytes = sizeof(NxVec3);
	clothMeshDesc.triangleStrideBytes = 3 * sizeof(APhysXU32);

	clothMeshDesc.flags = mFlags;

}
