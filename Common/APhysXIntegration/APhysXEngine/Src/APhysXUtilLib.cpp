
#include "APhysXStdAfx.h"
#include "PhysXBenchmark\PhysXBenchmark.h"
#include "OverlapDetector.h"

static NxU32 GetDefaultColor(const NxShapeType& st, const NX_BOOL isTriggerShape = NX_FALSE);
static void DrawBox(A3DWireCollector& a3dwc, const NxBoxShape& box, const NxU32& color);
static void DrawSphere(A3DWireCollector& a3dwc, const NxSphereShape& sphere, const NxU32& color);
static void DrawCapsule(A3DWireCollector& a3dwc, const NxCapsuleShape& capsule, const NxU32& color);
static void DrawConvex(A3DWireCollector& a3dwc, const NxConvexShape& convex, const NxU32& color);
static void DrawTriangleMesh(A3DWireCollector& a3dwc, const NxTriangleMeshShape& triMesh, const NxU32& color);
static void DrawHeightField(A3DWireCollector& a3dwc, const NxHeightFieldShape& hf, const NxU32& color);

bool APhysXUtilLib::IsSphereInNxActor(const NxActor* pNxActor, const NxSphere& sphere)
{
	if(pNxActor)
	{
		for(int i=0; i<pNxActor->getNbShapes(); i++)
		{
			NxShape* pCurShape = pNxActor->getShapes()[i];

			if(pCurShape->checkOverlapSphere(sphere))
			{
				return true;
			}
		}
	}

	return false;

}

bool APhysXUtilLib::IsPointInNxActor(const NxActor* pNxActor, const NxVec3& vPos, float fThickness)
{
	APHYSX_ASSERT(fThickness > 0.0f);

	NxSphere sphere;
	sphere.center = vPos;
	sphere.radius = fThickness;


	return IsSphereInNxActor(pNxActor, sphere);
}

void APhysXUtilLib::GetActorAABB(const NxActor* pNxActor, NxBounds3& aabb)
{
	aabb.setEmpty();

	if(pNxActor)
	{
		for(int i=0; i<pNxActor->getNbShapes(); i++)
		{
			NxShape* pCurShape = pNxActor->getShapes()[i];
			
			if(pCurShape->isPlane() || pCurShape->isWheel())
				continue;		// just ignore these two type of shapes

			NxBounds3 saabb;
			pCurShape->getWorldBounds(saabb);
			aabb.combine(saabb);
		}
	}

}

bool APhysXUtilLib::GetFFShapeAABB(const NxForceFieldShape* pFFShape, NxBounds3& aabb)
{
	NxUtilLib* pUtilLib = NxGetUtilLib();
	if(!pUtilLib) return false;

	switch(pFFShape->getType())
	{
	case NX_SHAPE_BOX:
		{
			const NxBoxForceFieldShape* pBoxFFShape = pFFShape->isBox();
			NxBox box;
			box.center = pFFShape->getPose().t;
			box.rot = pFFShape->getPose().M;
			box.extents = pBoxFFShape->getDimensions();

			NxVec3 vPos[8];
			pUtilLib->NxComputeBoxPoints(box, vPos);

			pUtilLib->NxComputeBounds(aabb.min, aabb.max, 8, vPos);
				
			return true;
	
		}
		break;
	case NX_SHAPE_SPHERE :
		{
			const NxSphereForceFieldShape* pSphereFFShape = pFFShape->isSphere();
			NxVec3 vPos = pFFShape->getPose().t;
			float fR = pSphereFFShape->getRadius();
			NxVec3 vExt(fR);
			aabb.max = vPos + vExt;
			aabb.min = vPos - vExt;
			
			return true;

		}
		break;
	case NX_SHAPE_CAPSULE:
		// to do...
	    // break;
	case NX_SHAPE_CONVEX:

		// to do...
	    // break;
	default:
		
		APHYSX_ASSERT(0);

		return false;

	    break;
	}

}

bool APhysXUtilLib::GetFFShapeGroupAABB(NxForceFieldShapeGroup& ffShapeGroup, NxBounds3& aabb)
{
	ffShapeGroup.resetShapesIterator();
	NxForceFieldShape* pFFShape = NULL;
	NxBounds3 shapeAABB;
	while((pFFShape = ffShapeGroup.getNextShape()) != NULL)
	{
		if(GetFFShapeAABB(pFFShape, shapeAABB))
		{
			aabb.combine(shapeAABB);
		}
	}

	return true;

}

void APhysXUtilLib::SetActorGroupMask(NxActor* pNxActor, const NxGroupsMask& gm)
{
	if(pNxActor)
	{
		for(int i=0; i<pNxActor->getNbShapes(); i++)
		{
			NxShape* pCurShape = pNxActor->getShapes()[i];
			pCurShape->setGroupsMask(gm);
		}
	}
}


/*
template<class T> 
void APhysXUtilLib::PushOnce(APhysXArray<T>& arr, const T& ele)
{
	

}
*/

float APhysXUtilLib::PhysXBenchmark(bool bRunOnHardware, APhysXU32 uTestSeconds)
{
	if(gPhysXEngine->GetNxPhysicsSDK())
	{
		return APhysXBenchmark::TestPhysXCapability(uTestSeconds, gPhysXEngine->GetNxPhysicsSDK(), bRunOnHardware);
	}
	else
		return 0;

}

/*
void GetOverlapActors(NxActor* pActor, APhysXArray<NxActor* >& arrOverlapActors, const NxVec3& vSweepDir, float fDistanceThresh)
{

#define MAX_REPORT_SHAPE_NUM 32

	NxSweepQueryHit sweepQH[MAX_REPORT_SHAPE_NUM];
	int iOverlapNum = pActor->linearSweep(fDistanceThresh * vSweepDir, NX_SF_STATICS | NX_SF_DYNAMICS | NX_SF_ALL_HITS , NULL, MAX_REPORT_SHAPE_NUM, sweepQH, NULL);

	for(int i = 0; i < iOverlapNum; i++)
	{
		NxActor* pOverlappedActor = &(sweepQH[i].hitShape->getActor());
		arrOverlapActors.push_back(pOverlappedActor);
	}
	
}
*/

void APhysXUtilLib::EnableDebugRender(NxActor* pNxActor, bool bEnable /* = true */)
{
	if(pNxActor)
	{
		for(int i=0; i<pNxActor->getNbShapes(); i++)
		{
			NxShape* pCurShape = pNxActor->getShapes()[i];
			
			if(bEnable)
				pCurShape->setFlag(NX_SF_VISUALIZATION, true);
			else
				pCurShape->setFlag(NX_SF_VISUALIZATION, false);
		}
	}

}

void APhysXUtilLib::DebugDrawAPhysXObject(A3DWireCollector& a3dwc, const APhysXObject* pPhysXObject, const DWORD dwCol /* = 0 */ )
{
	if(pPhysXObject->IsRigidBody())
	{
		// currently, we only support RB object rendering...
		const APhysXRigidBodyObject* pRBObject = (APhysXRigidBodyObject*)pPhysXObject;
		APhysXU32 uActorNum = pRBObject->GetNxActorNum();
		while(uActorNum--)
		{
			DebugDrawActor(a3dwc, *(pRBObject->GetNxActor(uActorNum)), dwCol);
		}

	}
}

void APhysXUtilLib::DebugDrawActor(A3DWireCollector& a3dwc, const NxActor& actor, const DWORD dwCol /* = 0 */)
{
	NxShape*const* ppShapes = actor.getShapes();
	NxU32 nShapes = actor.getNbShapes();
	for (NxU32 i = 0; i < nShapes; ++i)
		DebugDrawShape(a3dwc, *ppShapes[i], dwCol);
}

void APhysXUtilLib::DebugDrawShape(A3DWireCollector& a3dwc, const NxShape& shape, const DWORD dwCol)
{
	switch(shape.getType())
	{
	case NX_SHAPE_PLANE:
		break;
	case NX_SHAPE_BOX:
		DrawBox(a3dwc, *shape.isBox(), dwCol);
		break;
	case NX_SHAPE_SPHERE:
		DrawSphere(a3dwc, *shape.isSphere(), dwCol);
		break;
	case NX_SHAPE_CAPSULE:
		DrawCapsule(a3dwc, *shape.isCapsule(), dwCol);
		break;
	case NX_SHAPE_CONVEX:
		DrawConvex(a3dwc, *shape.isConvexMesh(), dwCol);
		break;		
	case NX_SHAPE_MESH:
		DrawTriangleMesh(a3dwc, *shape.isTriangleMesh(), dwCol);
		break;
	case NX_SHAPE_HEIGHTFIELD:
		DrawHeightField(a3dwc, *shape.isHeightField(), dwCol);
		break;
	case NX_SHAPE_WHEEL:
		break;
	default:
		break;
	}
}

void APhysXUtilLib::DebugDrawArraw(A3DWireCollector& a3dwc, const NxVec3& vStartPos, const NxVec3& vEndPos, const DWORD dwCol /* = 0xffffff00 */ )
{

	NxVec3 t0 = vEndPos - vStartPos, t1, t2;
	NxNormalToTangents(t0, t1, t2);
	float fSize = t0.normalize();
	fSize = (fSize>1.0f)? 0.15f : fSize * 0.15f;

	NxVec3 lobe1  = vEndPos - t0 * fSize + t1 * fSize;
	NxVec3 lobe2  = vEndPos - t0 * fSize - t1 * fSize;
	NxVec3 lobe3  = vEndPos - t0 * fSize + t2 * fSize;
	NxVec3 lobe4  = vEndPos - t0 * fSize - t2 * fSize;

	a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(vStartPos), APhysXConverter::N2A_Vector3(vEndPos), dwCol);

	a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(lobe1), APhysXConverter::N2A_Vector3(vEndPos), dwCol);
	a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(lobe2), APhysXConverter::N2A_Vector3(vEndPos), dwCol);
	a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(lobe3), APhysXConverter::N2A_Vector3(vEndPos), dwCol);
	a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(lobe4), APhysXConverter::N2A_Vector3(vEndPos), dwCol);
}

NxU32 GetDefaultColor(const NxShapeType& st, const NX_BOOL isTriggerShape)
{
	NxU32 defColor = 0xFF00ffff;  //cyan
	if (isTriggerShape)
		return defColor;

	switch(st)
	{
	case NX_SHAPE_PLANE:
		break;
	case NX_SHAPE_BOX:  
		defColor = 0xFFff0000;    //red
		break;
	case NX_SHAPE_SPHERE:
		defColor = 0xFF00ff00;    //green
		break;
	case NX_SHAPE_CAPSULE:
		defColor = 0xFF0000ff;    //blue 
		break;
	case NX_SHAPE_CONVEX:
		defColor = 0xFFff8000;	  //orange 
		break;		
	case NX_SHAPE_MESH:
		defColor = 0xFFff80c0;	  //pink 
		break;
	case NX_SHAPE_HEIGHTFIELD:
		defColor = 0xFFffff00;    //yellow
		break;
	case NX_SHAPE_WHEEL:
		break;
	default:
		break;
	}
	return defColor;
}

void DrawBox(A3DWireCollector& a3dwc, const NxBoxShape& box, const NxU32& color)
{
	NxU32 c = color;
	if (0 == c)
		c = GetDefaultColor(NX_SHAPE_BOX, box.getFlag(NX_TRIGGER_ENABLE));

	NxBox obb;
	box.getWorldOBB(obb);

	APhysXDrawBasic::DrawBox(a3dwc, obb, c);
}

void DrawSphere(A3DWireCollector& a3dwc, const NxSphereShape& sphere, const NxU32& color)
{
	NxU32 c = color;
	if (0 == c)
		c = GetDefaultColor(NX_SHAPE_SPHERE, sphere.getFlag(NX_TRIGGER_ENABLE));

	APhysXDrawBasic::DrawSphere(a3dwc, sphere.getRadius(), c, 0xffffffff, &sphere.getGlobalPose());
}

void DrawCapsule(A3DWireCollector& a3dwc, const NxCapsuleShape& capsule, const NxU32& color)
{
	NxU32 c = color;
	if (0 == c)
		c = GetDefaultColor(NX_SHAPE_CAPSULE, capsule.getFlag(NX_TRIGGER_ENABLE));

	APhysXDrawBasic::DrawCapsule(a3dwc, capsule.getRadius(), capsule.getHeight(), c, 0xffffffff, &capsule.getGlobalPose());
}

void DrawConvex(A3DWireCollector& a3dwc, const NxConvexShape& convex, const NxU32& color)
{
	NxU32 c = color;
	if (0 == c)
		c = GetDefaultColor(NX_SHAPE_CONVEX, convex.getFlag(NX_TRIGGER_ENABLE));
	
	NxConvexMeshDesc meshDesc;
	convex.getConvexMesh().saveToDesc(meshDesc);
	APhysXDrawBasic::DrawConvex(a3dwc, meshDesc, c, &convex.getGlobalPose());
}

void DrawTriangleMesh(A3DWireCollector& a3dwc, const NxTriangleMeshShape& triMesh, const NxU32& color)
{
	NxU32 c = color;
	if (0 == c)
		c = GetDefaultColor(NX_SHAPE_MESH, triMesh.getFlag(NX_TRIGGER_ENABLE));

	NxTriangleMeshDesc meshDesc;
	triMesh.getTriangleMesh().saveToDesc(meshDesc);
	APhysXDrawBasic::DrawTriangle(a3dwc, meshDesc, c, &triMesh.getGlobalPose());
}

void DrawHeightField(A3DWireCollector& a3dwc, const NxHeightFieldShape& hfShape, const NxU32& color)
{
	NxU32 c = color;
	if (0 == c)
		c = GetDefaultColor(NX_SHAPE_HEIGHTFIELD, hfShape.getFlag(NX_TRIGGER_ENABLE));

	const NxHeightField& HF = hfShape.getHeightField();
	NxHeightFieldDesc hfDesc;
	HF.saveToDesc(hfDesc);

	const NxU32 size = hfDesc.nbRows * hfDesc.nbColumns;

	static APhysXArray<A3DVECTOR3> vtxArray;
	static APhysXArray<WORD> vtxIndices;
	vtxArray.clear();
	vtxIndices.clear();

	A3DVECTOR3 pos;
	NxTriangle tri;
	NxU32 idxTri = 0;

	for(NxU32 i = 0; i < hfDesc.nbRows - 1; ++i)
	{
		idxTri = 2 * i * hfDesc.nbColumns;
		for(NxU32 j = 0; j < hfDesc.nbColumns - 1; ++j)
		{
			if (hfShape.getTriangle(tri, 0, 0, idxTri, true, true))
			{
				NxU32 idx = vtxArray.size();
				pos.Set(tri.verts[0].x, tri.verts[0].y, tri.verts[0].z);
				vtxArray.push_back(pos);
				pos.Set(tri.verts[1].x, tri.verts[1].y, tri.verts[1].z);
				vtxArray.push_back(pos);
				pos.Set(tri.verts[2].x, tri.verts[2].y, tri.verts[2].z);
				vtxArray.push_back(pos);

				vtxIndices.push_back(idx);
				vtxIndices.push_back(idx + 1);
				vtxIndices.push_back(idx);
				vtxIndices.push_back(idx + 2);
				vtxIndices.push_back(idx + 1);
				vtxIndices.push_back(idx + 2);
			}

			++idxTri;
			if (hfShape.getTriangle(tri, 0, 0, idxTri, true, true))
			{
				NxU32 idx = vtxArray.size();
				pos.Set(tri.verts[0].x, tri.verts[0].y, tri.verts[0].z);
				vtxArray.push_back(pos);
				pos.Set(tri.verts[1].x, tri.verts[1].y, tri.verts[1].z);
				vtxArray.push_back(pos);
				pos.Set(tri.verts[2].x, tri.verts[2].y, tri.verts[2].z);
				vtxArray.push_back(pos);

				vtxIndices.push_back(idx);
				vtxIndices.push_back(idx + 1);
				vtxIndices.push_back(idx);
				vtxIndices.push_back(idx + 2);
				vtxIndices.push_back(idx + 1);
				vtxIndices.push_back(idx + 2);
			}
			++idxTri;
		}
	}
	a3dwc.AddRenderData_3D(&vtxArray[0], vtxArray.size(), &vtxIndices[0], vtxIndices.size(), c);
}



//-------------------------------------------------------------------------------------
// some overlap utilities (for detailed overlap API docs, refer to OverlapDetector.h)
using namespace APhysXUtility;

static COverlapDetector s_OverlapDetector;

APhysXOverlapTestResult APhysXUtilLib::CheckOverlap(const NxActor& actor0, const NxActor& actor1, APhysXDetailOverlapInfo* pDetailInfo /* = NULL */ )
{
	COverlapDetector::ErrorCode err;
	COverlapDetailInfo odInfo;
	COverlapDetailInfo* pODInfo = (pDetailInfo)? (&odInfo) : NULL;


	bool bRes = s_OverlapDetector.CheckOverlap(actor0, actor1, err, pODInfo);
	
	APhysXOverlapTestResult otRes;
	if(err == COverlapDetector::E_SUCCESS)
	{
		otRes = bRes? APX_OTRES_TRUE : APX_OTRES_FALSE;

		if(bRes && pDetailInfo)
		{
			pDetailInfo->mOverlapNormal = pODInfo->avgNormal;
		}


	}
	else
		otRes = APX_OTRES_FAIL;

	return otRes;
}

APhysXOverlapTestResult APhysXUtilLib::CheckOverlap_Inflate(const NxActor& actor0, const NxActor& actor1, float growSize, APhysXDetailOverlapInfo* pDetailInfo /* = NULL */)
{

	COverlapDetector::ErrorCode err;
	COverlapDetailInfo odInfo;
	COverlapDetailInfo* pODInfo = (pDetailInfo)? (&odInfo) : NULL;

	bool bRes = s_OverlapDetector.Inflate6DCheckOverlap(actor0, actor1, growSize, err, NULL, pODInfo);

	APhysXOverlapTestResult otRes;
	if(err == COverlapDetector::E_SUCCESS)
	{
		otRes = bRes? APX_OTRES_TRUE : APX_OTRES_FALSE;

		if(bRes && pDetailInfo)
		{
			pDetailInfo->mOverlapNormal = pODInfo->avgNormal;
		}

	}
	else
		otRes = APX_OTRES_FAIL;

	return otRes;

}

APhysXOverlapTestResult APhysXUtilLib::CheckOverlap_Inflate(const NxActor& actor, float growSize, NxArray<NxShape*>& HitShapes, NxArray<APhysXDetailOverlapInfo>* pDetailInfo /* = NULL */)
{
	COverlapDetector::ErrorCode err;
	NxArray<COverlapDetailInfo> odInfo;
	NxArray<COverlapDetailInfo>* pODInfo = (pDetailInfo)? (&odInfo) : NULL;


	bool bRes = s_OverlapDetector.Inflate6DCheckOverlap(actor,growSize, HitShapes, err, NULL, pODInfo);

	APhysXOverlapTestResult otRes;
	if(err == COverlapDetector::E_SUCCESS)
	{
		otRes = bRes? APX_OTRES_TRUE : APX_OTRES_FALSE;

		if(bRes && pDetailInfo)
		{
			for(int i=0; i<pODInfo->size(); i++)
			{
				APhysXDetailOverlapInfo apxDOInfo;
				apxDOInfo.mOverlapNormal = (*pODInfo)[i].avgNormal;

				pDetailInfo->push_back(apxDOInfo);
			}

		}
	}
	else
		otRes = APX_OTRES_FAIL;

	return otRes;

}

APhysXOverlapTestResult APhysXUtilLib::LinearSweep(const NxActor& actor, const NxVec3& motion, NxArray<NxShape*>& HitShapes, NxArray<APhysXDetailOverlapInfo>* pDetailInfo /* = NULL */)
{
	COverlapDetector::ErrorCode err;
	NxArray<COverlapDetailInfo> odInfo;
	NxArray<COverlapDetailInfo>* pODInfo = (pDetailInfo)? (&odInfo) : NULL;

	bool bRes = s_OverlapDetector.LinearSweep(actor, motion, HitShapes, err, pODInfo);

	APhysXOverlapTestResult otRes;
	if(err == COverlapDetector::E_SUCCESS)
	{
		otRes = bRes? APX_OTRES_TRUE : APX_OTRES_FALSE;

		if(bRes && pDetailInfo)
		{
			for(int i=0; i<pODInfo->size(); i++)
			{
				APhysXDetailOverlapInfo apxDOInfo;
				apxDOInfo.mOverlapNormal = (*pODInfo)[i].avgNormal;

				pDetailInfo->push_back(apxDOInfo);
			}
		}
	}
	else
		otRes = APX_OTRES_FAIL;

	return otRes;


}

APhysXOverlapTestResult APhysXUtilLib::LinearSweep(const NxActor& actor, const NxVec3& dir, const float distance, NxArray<NxShape*>& HitShapes, NxArray<APhysXDetailOverlapInfo>* pDetailInfo /* = NULL */)
{
	COverlapDetector::ErrorCode err;
	NxArray<COverlapDetailInfo> odInfo;
	NxArray<COverlapDetailInfo>* pODInfo = (pDetailInfo)? (&odInfo) : NULL;


	bool bRes = s_OverlapDetector.LinearSweep(actor,dir, distance, HitShapes, err, pODInfo);

	APhysXOverlapTestResult otRes;
	if(err == COverlapDetector::E_SUCCESS)
	{
		otRes = bRes? APX_OTRES_TRUE : APX_OTRES_FALSE;

		if(bRes && pDetailInfo)
		{
			for(int i=0; i<pODInfo->size(); i++)
			{
				APhysXDetailOverlapInfo apxDOInfo;
				apxDOInfo.mOverlapNormal = (*pODInfo)[i].avgNormal;

				pDetailInfo->push_back(apxDOInfo);
			}
		}
	}
	else
		otRes = APX_OTRES_FAIL;

	return otRes;

}

APhysXOverlapTestResult APhysXUtilLib::SnapshotScene(NxScene& scene)
{
	COverlapDetector::ErrorCode err;
	bool bRes = s_OverlapDetector.SnapshotScene(scene, err);

	APhysXOverlapTestResult otRes = bRes? APX_OTRES_SUCCESS : APX_OTRES_FAIL;

	return otRes;
}

APhysXOverlapTestResult APhysXUtilLib::SnapshotScene(APhysXScene* pApxScene)
{
	if(pApxScene && pApxScene->GetNxScene())
	{
		return SnapshotScene(*pApxScene->GetNxScene());		
	}
	else
		return APX_OTRES_FAIL;

}

struct APhysXActorOverlapInfo
{
	NxVec3 mOverlapNormal;
	
	// to add... some more info...

};

typedef APhysXMap<NxActor*, APhysXActorOverlapInfo> ActorOverlapInfoMap;
typedef ActorOverlapInfoMap::iterator ActorOverlapInfoIter;
typedef ActorOverlapInfoMap::value_type ActorOverlapInfoPair;


APhysXOverlapTestResult APhysXUtilLib::CheckOverlap_Inflate(const NxActor& actor, float growSize, NxArray<NxActor*>& HitActors, NxArray<APhysXDetailOverlapInfo>* pDetailInfo /* = NULL */)
{
	NxArray<NxShape* > HitShapes;
	NxArray<APhysXDetailOverlapInfo> odInfo;
	NxArray<APhysXDetailOverlapInfo>* pODInfo = (pDetailInfo)? (&odInfo) : NULL;

	APhysXOverlapTestResult otRes = CheckOverlap_Inflate(actor, growSize, HitShapes, pODInfo);

	if(otRes == APX_OTRES_TRUE)
	{
		if(pDetailInfo)
		{
			ActorOverlapInfoMap aoMap;

			for(unsigned int i=0; i<HitShapes.size(); i++)
			{
				NxActor* pHitActor = &(HitShapes[i]->getActor());
				ActorOverlapInfoIter it = aoMap.find(pHitActor);
				if(it != aoMap.end())
				{
					it->second.mOverlapNormal += (*pODInfo)[i].mOverlapNormal;
				}
				else
				{
					APhysXActorOverlapInfo apxAOI;
					apxAOI.mOverlapNormal = (*pODInfo)[i].mOverlapNormal;
					aoMap.insert(ActorOverlapInfoPair(pHitActor, apxAOI));

				}

			}

			for(ActorOverlapInfoIter it = aoMap.begin(); it != aoMap.end(); ++it)
			{
				HitActors.push_back(it->first);
				it->second.mOverlapNormal.normalize();

				APhysXDetailOverlapInfo apxDOInfo;
				apxDOInfo.mOverlapNormal = it->second.mOverlapNormal;
				pDetailInfo->push_back(apxDOInfo);

			}

			aoMap.clear();

		}
		else
		{
			for(unsigned int i=0; i<HitShapes.size(); i++)
				APhysXUtilLib::APhysXArray_PushOnce(HitActors, &(HitShapes[i]->getActor()));

		}

	}

	return otRes;
}

APhysXOverlapTestResult APhysXUtilLib::LinearSweep(const NxActor& actor, const NxVec3& motion, NxArray<NxActor*>& HitActors, NxArray<APhysXDetailOverlapInfo>* pDetailInfo /* = NULL */)
{
	
	NxArray<NxShape* > HitShapes;
	NxArray<APhysXDetailOverlapInfo> odInfo;
	NxArray<APhysXDetailOverlapInfo>* pODInfo = (pDetailInfo)? (&odInfo) : NULL;


	APhysXOverlapTestResult otRes = LinearSweep(actor, motion, HitShapes, pODInfo);

	if(otRes == APX_OTRES_TRUE)
	{
		if(pDetailInfo)
		{
			ActorOverlapInfoMap aoMap;

			for(unsigned int i=0; i<HitShapes.size(); i++)
			{
				NxActor* pHitActor = &(HitShapes[i]->getActor());
				ActorOverlapInfoIter it = aoMap.find(pHitActor);
				if(it != aoMap.end())
				{
					it->second.mOverlapNormal += (*pODInfo)[i].mOverlapNormal;
				}
				else
				{
					APhysXActorOverlapInfo apxAOI;
					apxAOI.mOverlapNormal = (*pODInfo)[i].mOverlapNormal;
					aoMap.insert(ActorOverlapInfoPair(pHitActor, apxAOI));

				}

			}

			for(ActorOverlapInfoIter it = aoMap.begin(); it != aoMap.end(); ++it)
			{
				HitActors.push_back(it->first);
				it->second.mOverlapNormal.normalize();

				APhysXDetailOverlapInfo apxDOInfo;
				apxDOInfo.mOverlapNormal = it->second.mOverlapNormal;
				pDetailInfo->push_back(apxDOInfo);

			}

			aoMap.clear();

		}
		else
		{

			for(unsigned int i=0; i<HitShapes.size(); i++)
				APhysXUtilLib::APhysXArray_PushOnce(HitActors, &(HitShapes[i]->getActor()));
		}

	}

	return otRes;


}

APhysXOverlapTestResult APhysXUtilLib::LinearSweep(const NxActor& actor, const NxVec3& dir, const float distance, NxArray<NxActor*>& HitActors, NxArray<APhysXDetailOverlapInfo>* pDetailInfo /* = NULL */)
{

	NxArray<NxShape* > HitShapes;
	NxArray<APhysXDetailOverlapInfo> odInfo;
	NxArray<APhysXDetailOverlapInfo>* pODInfo = (pDetailInfo)? (&odInfo) : NULL;

	APhysXOverlapTestResult otRes = LinearSweep(actor, dir, distance, HitShapes, pODInfo);

	if(otRes == APX_OTRES_TRUE)
	{

		if(pDetailInfo)
		{
			ActorOverlapInfoMap aoMap;

			for(unsigned int i=0; i<HitShapes.size(); i++)
			{
				NxActor* pHitActor = &(HitShapes[i]->getActor());
				ActorOverlapInfoIter it = aoMap.find(pHitActor);
				if(it != aoMap.end())
				{
					it->second.mOverlapNormal += (*pODInfo)[i].mOverlapNormal;
				}
				else
				{
					APhysXActorOverlapInfo apxAOI;
					apxAOI.mOverlapNormal = (*pODInfo)[i].mOverlapNormal;
					aoMap.insert(ActorOverlapInfoPair(pHitActor, apxAOI));

				}

			}

			for(ActorOverlapInfoIter it = aoMap.begin(); it != aoMap.end(); ++it)
			{
				HitActors.push_back(it->first);
				it->second.mOverlapNormal.normalize();

				APhysXDetailOverlapInfo apxDOInfo;
				apxDOInfo.mOverlapNormal = it->second.mOverlapNormal;
				pDetailInfo->push_back(apxDOInfo);

			}

			aoMap.clear();

		}
		else
		{

			for(unsigned int i=0; i<HitShapes.size(); i++)
				APhysXUtilLib::APhysXArray_PushOnce(HitActors, &(HitShapes[i]->getActor()));
		}

	}

	return otRes;

}

NxVec3 APhysXUtilLib::Lerp(const NxVec3& v0, const NxVec3& v1, float t)
{
	NxVec3 v = t * v1 + ( 1 - t ) * v0;
	return v;
}

NxVec3 APhysXUtilLib::Slerp(const NxVec3& v0, const NxVec3& v1, float t, const NxVec3& vRefAxis /* = NxVec3(0.0f, 1.0f, 0.0f) */)
{
	float cosTheta = v0.dot(v1);
	float Theta = NxMath::acos(cosTheta);

	float sinTheta = NxMath::sin(Theta);
	
	if(NxMath::equals(sinTheta, 0.0f, 1e-4f)) 
	{
		if(cosTheta > 0.0f)
			return v0;			// v0 and v1 are almost in same direction
		else
		{
			// v0 and v1 are almost in the opposite direction
			NxVec3 vPerpendicular = v0.cross(vRefAxis);

			NxVec3 v = NxMath::cos( t * Theta ) * v0 + NxMath::sin( t * Theta ) * vPerpendicular;
			return v;			
		}
	}

	NxVec3 v = NxMath::sin( (1 - t) * Theta ) * v0 + NxMath::sin(t * Theta) * v1;
	return v/sinTheta;
}

NxVec3 APhysXUtilLib::GenRndDir()
{
	NxVec3 vDir;

	float fLat = NxMath::rand(-0.5f * APHYSX_PI, 0.5f * APHYSX_PI);
	float fXOZ;
	NxMath::sinCos(fLat, vDir.y, fXOZ);
	fXOZ = NxMath::abs(fXOZ);

	float fLong = NxMath::rand(-APHYSX_PI,  APHYSX_PI);
	NxMath::sinCos(fLong, vDir.z, vDir.x);
	vDir.x *= fXOZ;
	vDir.z *= fXOZ;

	return vDir;
}

NxVec3 APhysXUtilLib::GenRndAngVel(float fMaxVelMag)
{
	float fMag = NxMath::rand(0.0f, fMaxVelMag);

	return fMag * GenRndDir();
}
