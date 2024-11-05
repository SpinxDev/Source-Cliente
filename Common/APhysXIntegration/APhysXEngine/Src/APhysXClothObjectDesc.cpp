
#include "APhysXStdAfx.h"


void APhysXClothParameters::LoadPresetMaterial(int iPresetMaterial)
{
	// do this before calling this function...
	// SetToDefault();

	switch(iPresetMaterial)
	{
	case APX_CLOTH_MATERIAL_SILK:
		
		mBendingStiffness = 0.5f;
		mStretchingStiffness = 0.6f;
		mFriction = 0.0f;
		
		mFlags |= NX_CLF_BENDING;

		break;

	case APX_CLOTH_MATERIAL_FLAX:

		mFriction = 1.0f;
		mStretchingStiffness = 0.8f;
		mDampingCoefficient = 0.5f;
		mFlags |= NX_CLF_BENDING | NX_CLF_DAMPING;
		
		break;

	case APX_CLOTH_MATERIAL_FELT:

		mDampingCoefficient = 0.8f;
		mFlags |= NX_CLF_DAMPING | NX_CLF_COMDAMPING;
	
	    break;

	default:
		// do nothing...
	    break;
	}

}


bool APhysXClothObjectDesc::InitClothMesh(const NxClothMeshDesc& clothMeshDesc, const NxVec3& vScale3D)
{
	NxClothMeshDesc myClothMeshDesc = clothMeshDesc;
	
//------------------------------------------------------------------
// some compulsive setting...

// force to weld vertices
	
	// Note by Wenfeng, Nov. 6, 2008
	// I just found a bug about cloth mesh's welding...
	// 1. VRD can' t work well when I turn on the welding of cloth mesh...
	// 2. Interaction between Forcefield and welding-mesh cloth will crush the simulation...
 	// myClothMeshDesc.flags |= NX_CLOTH_MESH_WELD_VERTICES;
 	// myClothMeshDesc.weldingDistance	= 0.001f;

	//-------------------------------
	// test code: force to turn off the NX_CLOTH_MESH_WELD_VERTICES flag...
	// myClothMeshDesc.flags &= ~NX_CLOTH_MESH_WELD_VERTICES;

	
	// make sure the tearable flags of Mesh and Cloth keep consistent.
	if(mClothParameters.mFlags & NX_CLF_TEARABLE)
		myClothMeshDesc.flags |= NX_CLOTH_MESH_TEARABLE;
	else if(myClothMeshDesc.flags & NX_CLOTH_MESH_TEARABLE)
		mClothParameters.mFlags |= NX_CLF_TEARABLE;

//------------------------------------------------------------------

	NxVec3* VertBuf = NULL;
	if(vScale3D != NxVec3(1.0f))
	{
		// try to scale clothMeshDesc's points...
		VertBuf = new NxVec3[clothMeshDesc.numVertices];
		APhysXU8* pSrc = (APhysXU8*) clothMeshDesc.points;
		NxVec3* pDest = VertBuf;
		for(int i=0; i<clothMeshDesc.numVertices; i++, pSrc+= clothMeshDesc.pointStrideBytes, pDest++)
		{
			NxVec3& vSrc = *(NxVec3 *)pSrc;
			NxVec3& vDest = *pDest;

			vDest.arrayMultiply(vSrc, vScale3D);
		}

		myClothMeshDesc.points = VertBuf;
		myClothMeshDesc.pointStrideBytes = sizeof(NxVec3);
	}

	bool bCookRes = false;
	APhysXMemoryWriteBuffer buf;
	if(gPhysXEngine->GetMeshCooker()->CookClothMesh(myClothMeshDesc, buf))
	{
		SetClothMeshBuf(buf);

		mVertexNum = myClothMeshDesc.numVertices;
		mTriangleNum = myClothMeshDesc.numTriangles;

		bCookRes = true;
	}

	mCookLibVersion = APHYSX_CURRENT_PHYSX_SDK_VERSION;
	mClothMeshDesc.SetNxClothMeshDesc(myClothMeshDesc);

	if(VertBuf)
		delete [] VertBuf;

	return bCookRes;
}


bool APhysXClothObjectDesc::GetNxClothDesc(NxClothDesc& clothDesc)
{
	if(!mClothMeshBuf) return false;

	APhysXMemoryReadBuffer apxMemReadBuf;
	GetMemoryReadBuf(apxMemReadBuf);

	if(!m_pNxClothMesh)
	{
		NxClothMesh * pClothMesh = gPhysXEngine->GetMeshCooker()->GetClothMesh(apxMemReadBuf);
		if(!pClothMesh) return false;

		m_pNxClothMesh = pClothMesh;
	}

	clothDesc.clothMesh = m_pNxClothMesh;
	
	clothDesc.thickness = mClothParameters.mThickness;
	// test code...
	// clothDesc.thickness = 0.06f;

	clothDesc.density = mClothParameters.mDensity;
	clothDesc.bendingStiffness = mClothParameters.mBendingStiffness;
	clothDesc.stretchingStiffness = mClothParameters.mStretchingStiffness;
	clothDesc.dampingCoefficient = mClothParameters.mDampingCoefficient;
	clothDesc.friction = mClothParameters.mFriction;
	clothDesc.pressure = mClothParameters.mPressure;
	clothDesc.tearFactor = mClothParameters.mTearFactor;
	clothDesc.attachmentTearFactor = mClothParameters.mAttachmentTearFactor;
	clothDesc.attachmentResponseCoefficient = mClothParameters.mAttachmentResponseCoefficient;
	clothDesc.collisionResponseCoefficient = mClothParameters.mCollisionResponseCoefficient;
	clothDesc.toFluidResponseCoefficient = mClothParameters.mToFluidResponseCoefficient;
	clothDesc.fromFluidResponseCoefficient = mClothParameters.mFromFluidResponseCoefficient;
	clothDesc.minAdhereVelocity = mClothParameters.mMinAdhereVelocity;
	clothDesc.flags = mClothParameters.mFlags;
	
	//-----------------------------------------------------------------------------------
	// change to improve the cloth simulation performance, while this will 
	// result in some appearance drawback as well...
	clothDesc.solverIterations = mClothParameters.mSolverIterations;
	// clothDesc.solverIterations = 1;
	//-----------------------------------------------------------------------------------

	clothDesc.wakeUpCounter = mClothParameters.mWakeUpCounter;
	clothDesc.sleepLinearVelocity = mClothParameters.mSleepLinearVelocity;
	clothDesc.forceFieldMaterial = mClothParameters.mForceFieldMaterial;
	clothDesc.externalAcceleration = mClothParameters.mExternalAcceleration;
	clothDesc.windAcceleration = mClothParameters.mWindAcceleration;
	clothDesc.validBounds = mClothParameters.mValidBounds;
	clothDesc.relativeGridSpacing = mClothParameters.mRelativeGridSpacing;

	clothDesc.flags |= NX_CLF_VISUALIZATION;	// enable debug rendering...
	
	// make the flag controlled by desc freely...
	// clothDesc.flags |= NX_CLF_ADHERE;			// force to set this flag which is suitable for character cloth...
	
	// test code...
	// clothDesc.flags |= NX_CLF_HARDWARE;			
	
	if(clothDesc.flags & NX_CLF_TEARABLE && mTearVertexNumFactor == 1)
		mTearVertexNumFactor = 2;		// force set the tearable vertex number factor so that tearing has effect

	clothDesc.collisionGroup = mCollisionGroup;

	return true;
}

bool APhysXClothObjectDesc::Save(NxStream* pNxStream)
{
	if(!APhysXObjectDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeBuffer(&mClothParameters, sizeof(APhysXClothParameters));
	
	pNxStream->storeDword(mTearVertexNumFactor);
	pNxStream->storeDword(mVertexNum);
	pNxStream->storeDword(mTriangleNum);

	pNxStream->storeDword(mClothMeshBufSize);
	pNxStream->storeBuffer(mClothMeshBuf, mClothMeshBufSize);

	pNxStream->storeFloat(mConstrainMaxDistance);
	pNxStream->storeFloat(mConstrainMaxDistanceBias);
	pNxStream->storeFloat(mConstrainCollisionSphereRadius);
	pNxStream->storeFloat(mConstrainCollisionSphereDistance);

	pNxStream->storeDword(mCookLibVersion);
	mClothMeshDesc.Save(pNxStream);

	if(mClothVertexConstraints)
	{
		pNxStream->storeByte((NxU8)1);
		pNxStream->storeBuffer(mClothVertexConstraints, sizeof(APhysXClothVertexConstraintParameters) * mVertexNum);
	}
	else
		pNxStream->storeByte((NxU8)0);
	

	return true;
}


bool APhysXClothObjectDesc::Load(NxStream* pNxStream)
{
	if(!APhysXObjectDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	Release();

	if(m_StreamObjVer >= 0xcc000001)
	{
		pNxStream->readBuffer(&mClothParameters, sizeof(APhysXClothParameters));

		mTearVertexNumFactor = pNxStream->readDword();
		mVertexNum = pNxStream->readDword();
		mTriangleNum = pNxStream->readDword();

		mClothMeshBufSize = pNxStream->readDword();
		mClothMeshBuf = new APhysXU8[mClothMeshBufSize];
		pNxStream->readBuffer(mClothMeshBuf, mClothMeshBufSize);

		// old cooking data are under the version of 2.8.1
		mCookLibVersion = APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 1);
		
	}
	
	if(m_StreamObjVer >= 0xcc000002)
	{
		mConstrainMaxDistance = pNxStream->readFloat();
		mConstrainMaxDistanceBias = pNxStream->readFloat();
		mConstrainCollisionSphereRadius = pNxStream->readFloat();
		mConstrainCollisionSphereDistance = pNxStream->readFloat();

		mCookLibVersion = pNxStream->readDword();
		mClothMeshDesc.Load(pNxStream);
	}

	if(m_StreamObjVer >= 0xcc000003)
	{
		if(pNxStream->readByte())
		{
			mClothVertexConstraints = new APhysXClothVertexConstraintParameters[mVertexNum];
			pNxStream->readBuffer(mClothVertexConstraints, sizeof(APhysXClothVertexConstraintParameters) * mVertexNum);
		}
	}

	// re-cooking...
	if(mCookLibVersion != APHYSX_CURRENT_PHYSX_SDK_VERSION)
	{
		// we have to reconstruct the cooked cloth stream...

		gPhysXLog->Log("Note: Need to recook the cloth mesh since cook lib version has changed!");

		NxClothMeshDesc clothMeshDesc;
		mClothMeshDesc.GetNxClothMeshDesc(clothMeshDesc);
		// cloth welding handle...
		if(clothMeshDesc.flags | NX_CLOTH_MESH_WELD_VERTICES)
		{
			clothMeshDesc.weldingDistance = 0.001f;
		}

		APhysXMemoryWriteBuffer buf;
		if(gPhysXEngine->GetMeshCooker()->CookClothMesh(clothMeshDesc, buf))
		{
			SetClothMeshBuf(buf);
		}
		else
		{
			gPhysXLog->Log("Warning: Failed on recooking cloth mesh!");
			return false;
		}

	}


	return true;
}

APhysXObjectDesc* APhysXClothObjectDesc::Clone() const
{
	APhysXObjectDesc* pClonedObjectDesc = new APhysXClothObjectDesc;

	// clone parents' data
	*pClonedObjectDesc = *(APhysXObjectDesc* )this;

	APhysXClothObjectDesc* pClonedClothObjectDesc = (APhysXClothObjectDesc*) pClonedObjectDesc;
	pClonedClothObjectDesc->mClothParameters = mClothParameters;
	pClonedClothObjectDesc->mTearVertexNumFactor = mTearVertexNumFactor;

	pClonedClothObjectDesc->mVertexNum = mVertexNum;
	pClonedClothObjectDesc->mTriangleNum = mTriangleNum;

	pClonedClothObjectDesc->mClothMeshBufSize = mClothMeshBufSize;
	if(mClothMeshBufSize)
	{
		pClonedClothObjectDesc->mClothMeshBuf = new APhysXU8[mClothMeshBufSize];
		memcpy(pClonedClothObjectDesc->mClothMeshBuf,mClothMeshBuf, mClothMeshBufSize);
	}

	pClonedClothObjectDesc->mConstrainMaxDistance = mConstrainMaxDistance;
	pClonedClothObjectDesc->mConstrainMaxDistanceBias = mConstrainMaxDistanceBias;
	pClonedClothObjectDesc->mConstrainCollisionSphereRadius = mConstrainCollisionSphereRadius;
	pClonedClothObjectDesc->mConstrainCollisionSphereDistance = mConstrainCollisionSphereDistance;

	pClonedClothObjectDesc->mCookLibVersion = mCookLibVersion;
	pClonedClothObjectDesc->mClothMeshDesc = mClothMeshDesc;

	pClonedClothObjectDesc->SetClothVertexConstraints(mClothVertexConstraints);

	//??? fix me:  Should m_pNxClothMesh be cloned as well ???...
	// I think we can't copy the pointer here... Nov.25, 2008
	// pClonedClothObjectDesc->m_pNxClothMesh = m_pNxClothMesh;

	return pClonedObjectDesc;

}

void APhysXClothObjectDesc::SetClothMeshBuf(APhysXMemoryWriteBuffer& apxMemWriteBuf)
{
	Release();
	mClothMeshBufSize = apxMemWriteBuf.currentSize;
	mClothMeshBuf = new APhysXU8[mClothMeshBufSize];

	memcpy(mClothMeshBuf, apxMemWriteBuf.data, mClothMeshBufSize);

}

void APhysXClothObjectDesc::GetMemoryReadBuf(APhysXMemoryReadBuffer& apxMemReadBuf)
{
	apxMemReadBuf.buffer = mClothMeshBuf;
}

void APhysXClothObjectDesc::Release()
{
	if(mClothMeshBuf)
	{
		delete [] mClothMeshBuf;

		mClothMeshBuf = NULL;
		mClothMeshBufSize = 0;
	}

	if(mClothVertexConstraints)
	{
		delete[] mClothVertexConstraints;
		mClothVertexConstraints = NULL;
	}

	if(m_pNxClothMesh)
	{
		//--------------------------------------------------------------------
		// todo: use TryReleaseConvexMesh(...) instead? 
		// there seems to be no issues currently, so we just pend the revision...
		// gPhysXEngine->GetMeshCooker()->TryReleaseConvexMesh(m_pNxClothMesh);
		gPhysXEngine->GetMeshCooker()->ForceReleaseClothMesh(m_pNxClothMesh);
		m_pNxClothMesh = NULL;
	}
}

bool APhysXClothObjectDesc::SetClothVertexConstraints(const APhysXClothVertexConstraintParameters* apxCVCParas)
{
	if(mVertexNum  )
	{	
		if(mClothVertexConstraints)
		{
			delete [] mClothVertexConstraints;
			mClothVertexConstraints = NULL;
		}

		if(apxCVCParas)
		{
			mClothVertexConstraints = new APhysXClothVertexConstraintParameters[mVertexNum];
			memcpy(mClothVertexConstraints, apxCVCParas, sizeof(APhysXClothVertexConstraintParameters) * mVertexNum );
		}

		return true;
	}
	else
		return false;
}