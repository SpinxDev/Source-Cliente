
#include "APhysXStdAfx.h"


bool APhysXHeightFieldShapeDesc::Save(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeDword(mRowNum);
	pNxStream->storeDword(mColumnNum);

	if(mRowNum && mColumnNum)
	{
		pNxStream->storeBuffer(mSampleData, mRowNum * mColumnNum * sizeof(APhysXU32));
	}

	pNxStream->storeFloat(mHeightScale);
	pNxStream->storeFloat(mRowScale);
	pNxStream->storeFloat(mColumnScale);

	return true;

}

bool APhysXHeightFieldShapeDesc::Load(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Load(pNxStream))
		return false;

	Release();

	// load my version...
	LoadVersion(pNxStream);
	
	mRowNum = pNxStream->readDword();
	mColumnNum = pNxStream->readDword();

	if(mRowNum && mColumnNum)
	{
		int iSampleDataSize = mRowNum * mColumnNum;
		mSampleData = new APhysXU32[iSampleDataSize];
		pNxStream->readBuffer(mSampleData, iSampleDataSize * sizeof(APhysXU32));
	}

	mHeightScale = pNxStream->readFloat();
	mRowScale = pNxStream->readFloat();
	mColumnScale = pNxStream->readFloat();

	return true;
}

NxShapeDesc* APhysXHeightFieldShapeDesc::CreateNxShapeDesc(const NxVec3& vSacle3D /* = NxVec3(1.0f) */)
{
	NxShapeDesc* pNxShapeDesc = new NxHeightFieldShapeDesc;
	GetNxShapeDesc(*pNxShapeDesc, vSacle3D);
	return pNxShapeDesc;
}

//----------------------------------------------------
// Note by wenfeng, Nov. 18, 2010
// some weird thing in the following snippets,  when we build 
// in normal release mode, it can incur some exception for some case,
// however, if we turn off the optimization of this function, it works OK!
//
// + Note by wenfeng, Nov. 19, 2010
// after some careful investigation, I still could not figure out the issue
// why it crash in some case. And after I split the GetNxShapeDesc(...) function
// into two parts. It can work without turning off the optimization.

inline NxHeightField* APhysXHeightFieldShapeDesc::CreateNxHeightField()
{
	// just ignore the vScale3D parameter!
	NxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.nbColumns		= mColumnNum;
	heightFieldDesc.nbRows			= mRowNum;
	heightFieldDesc.samples = mSampleData;

	//------------------------------------------------------------
	// this member has been deprecated...
	// heightFieldDesc.verticalExtent	= -1000.0f;
	heightFieldDesc.thickness = -1000.0f;	

	//heightFieldDesc.flags = NX_HF_NO_BOUNDARY_EDGES;
	heightFieldDesc.sampleStride	= sizeof(NxU32);

	NxHeightField* pHeightHield = gPhysXEngine->GetNxPhysicsSDK()->createHeightField(heightFieldDesc);
	APHYSX_ASSERT(pHeightHield);

	return pHeightHield;

}

// #pragma optimize("", off)

void APhysXHeightFieldShapeDesc::GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D)
{

	APhysXShapeDesc::GetNxShapeDesc(InNxShapeDesc, vSacle3D);

#define _DISABLE_HEIGHTFIELD_DEBUGRENDER_
#ifdef	_DISABLE_HEIGHTFIELD_DEBUGRENDER_

	InNxShapeDesc.shapeFlags &= ~NX_SF_VISUALIZATION;

#endif	

	NxHeightFieldShapeDesc& heightFieldShapeDesc = (NxHeightFieldShapeDesc&)InNxShapeDesc;
	heightFieldShapeDesc.heightScale = mHeightScale;
	
	//--------------------------------------------------------------
	// revised by wenfeng, Jan. 19, 2012
	// in some cases, designers will create the planar height field with all height equaling to the unique value,
	// while, in our PhysX Terrain import function, this will generate Zero heightScale. Although this shape desc 
	// is not valid, PhysX still create the HF actors correctly, but the dynamic actors colliding with this HF will
	// generate wrong pose and velocity...

	if(heightFieldShapeDesc.heightScale <= NX_EPS_REAL)
		heightFieldShapeDesc.heightScale = 1.0f;

	heightFieldShapeDesc.rowScale = mRowScale;
	heightFieldShapeDesc.columnScale = mColumnScale;
	heightFieldShapeDesc.materialIndexHighBits =0;
	heightFieldShapeDesc.holeMaterial = APHYSX_HF_HOLE_MATERIAL_ID;	
	// it seems that changing this flag has no effect on Fluid Particle rolling around terrain...	
	// heightFieldShapeDesc.meshFlags = NX_MESH_SMOOTH_SPHERE_COLLISIONS;

	heightFieldShapeDesc.heightField = CreateNxHeightField();

	APHYSX_ASSERT(heightFieldShapeDesc.isValid());
}

// #pragma optimize("", on)



void APhysXHeightFieldShapeDesc::TransposeRowCol()
{
	if(mRowNum == mColumnNum)
	{
		// change the cell tessellation way
		/*
									Sample Data Format, 32 bits
			-------------------------------------------------------------------------------------
			unused/reserved		materialIndex1		tessFlag		materialIndex0		height
			-------------------------------------------------------------------------------------
				1 bit				7 bits			 1 bit				7 bits			16 bits
			-------------------------------------------------------------------------------------
		*/

		for(int row = 0; row < mRowNum; row++)
		{
			for(int col = 0; col < mColumnNum; col++)
			{
				APhysXU32 uSample = GetSample(row, col);
				uSample = uSample ^ 0x800000;					
				SetSample(row, col, uSample);
			}
		}

		// swap the sample data of col and row
		APhysXU32 uSample;
		for(int row = 0; row < mRowNum; row++)
		{
			for(int col = 0; col < row; col++)
			{
				uSample = GetSample(row, col);
				SetSample(row, col, GetSample(col,row));
				SetSample(col, row, uSample);

			}
		}

		float fTmp = mRowScale;
		mRowScale = mColumnScale;
		mColumnScale = fTmp;

	}
	else
	{

		// to do...
		APHYSX_ASSERT(0);

		APhysXU32 uTmp = mRowNum;
		mRowNum = mColumnNum;
		mColumnNum = uTmp;


	}
	

}

void APhysXHeightFieldShapeDesc::Release()
{
	if(mSampleData)
	{
		delete [] mSampleData;
		mSampleData = NULL;
	}
}

void APhysXHeightFieldShapeDesc::AllocZeroSampleData()
{
	Release();
	int iSampleDataSize = mRowNum * mColumnNum;
	mSampleData = new APhysXU32[iSampleDataSize];
	memset(mSampleData, 0, iSampleDataSize * sizeof(APhysXU32));
}

const APhysXHeightFieldShapeDesc& APhysXHeightFieldShapeDesc::operator= (const APhysXHeightFieldShapeDesc& other)
{
	// copy the data of super class...
	APhysXShapeDesc& mySuper = *(APhysXShapeDesc* )this;
	APhysXShapeDesc& otherSuper = *(APhysXShapeDesc* )(&other);
	mySuper = otherSuper;

	// then copy the buffer...
	Release();

	mRowNum = other.mRowNum;
	mColumnNum = other.mColumnNum;
	int iSampleDataSize = mRowNum * mColumnNum;
	mSampleData = new APhysXU32[iSampleDataSize];
	memcpy(mSampleData, other.mSampleData, iSampleDataSize * sizeof(APhysXU32));

	mHeightScale = other.mHeightScale;
	mRowScale = other.mRowScale;
	mColumnScale = other.mColumnScale;

	return *this;
}
