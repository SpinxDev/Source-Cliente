
#include "APhysXStdAfx.h"

#define TRIM_TEXT_LEN 30

//------------------------------------------------------------------------
inline static DWORD GetTransparentColor(DWORD dwColor, DWORD Alpha = 0xff)
{

	return (dwColor & 0x00ffffff | (Alpha<<24));
}


// trim a string to a fix length string, the chars beyond the length will be cut
// and if the input string is shorter than the length, space char will be added to
// the end.
static ACString TrimString(const ACString& strIn, int iLen)
{
	if(strIn.GetLength() >= iLen)
		return strIn.Left(iLen);

	ACString strOut(strIn);
	for(int i= strIn.GetLength(); i<iLen; i++)
	{
		strOut += ACString(_AL(" "));
	}
	return strOut;
}

//------------------------------------------------------------------------
void APhysXDebugRenderer::Init(APhysXEngine* pPhysXEngine)
{
	m_pA3DEngine = pPhysXEngine->GetA3DEngine();

	m_pFont = m_pA3DEngine->GetSystemFont();
	m_iFontHeight = m_pFont->GetFontOutputHeight();
	
}

void APhysXDebugRenderer::RenderData(const NxDebugRenderable* pDRData)
{

	if(!pDRData) return;

	if(m_pA3DEngine)
	{
		if(m_DebugRenderMode == APX_DRMODE_WIREFRAME)
		{
			A3DWireCollector* pWC = m_pA3DEngine->GetA3DWireCollector();
			
			// points 
#if 1
			// currently, we render the points as sphere, expensive????
			NxU32 NbPoints = pDRData->getNbPoints();
			const NxDebugPoint* Points = pDRData->getPoints();
			while(NbPoints--)
			{
				pWC->AddSphere(APhysXConverter::N2A_Vector3(Points->p), 0.05f, GetTransparentColor(Points->color));
				Points++;
			}
			
#endif

			// lines
			NxU32 NbLines = pDRData->getNbLines();
			const NxDebugLine* Lines = pDRData->getLines();

			while(NbLines--)
			{
				pWC->Add3DLine(APhysXConverter::N2A_Vector3(Lines->p0), APhysXConverter::N2A_Vector3(Lines->p1), GetTransparentColor(Lines->color));
				Lines++;
			}


			// triangles
			NxU32 NbTris = pDRData->getNbTriangles();
			const NxDebugTriangle* Triangles = pDRData->getTriangles();

			while(NbTris--)
			{
				pWC->Add3DLine(APhysXConverter::N2A_Vector3(Triangles->p0), APhysXConverter::N2A_Vector3(Triangles->p1), GetTransparentColor(Triangles->color));
				pWC->Add3DLine(APhysXConverter::N2A_Vector3(Triangles->p1), APhysXConverter::N2A_Vector3(Triangles->p2), GetTransparentColor(Triangles->color));
				pWC->Add3DLine(APhysXConverter::N2A_Vector3(Triangles->p2), APhysXConverter::N2A_Vector3(Triangles->p0), GetTransparentColor(Triangles->color));
			
				Triangles++;
			}

			pWC->Flush();

		}
		else if (m_DebugRenderMode == APX_DRMODE_SOLID)
		{
			A3DWireCollector* pWC = m_pA3DEngine->GetA3DWireCollector();
			A3DFlatCollector* pFC = m_pA3DEngine->GetA3DFlatCollector();

			// points 
#if 1
			// currently, we render the points as sphere, expensive????
			NxU32 NbPoints = pDRData->getNbPoints();
			const NxDebugPoint* Points = pDRData->getPoints();
			while(NbPoints--)
			{
				pWC->AddSphere(APhysXConverter::N2A_Vector3(Points->p), 0.05f, Points->color);
				Points++;
			}

#endif

			// lines
			NxU32 NbLines = pDRData->getNbLines();
			const NxDebugLine* Lines = pDRData->getLines();

			while(NbLines--)
			{
				pWC->Add3DLine(APhysXConverter::N2A_Vector3(Lines->p0), APhysXConverter::N2A_Vector3(Lines->p1), GetTransparentColor(Lines->color));
				Lines++;
			}

			// triangles
			
			NxU32 NbTris = pDRData->getNbTriangles();
			const NxDebugTriangle* Triangles = pDRData->getTriangles();
			A3DVECTOR3 Pos[3];
			//???note here, need wind order conversion here: anticlockwise -> clockwise
			WORD TriID[] = { 0, 1, 2};		

			while(NbTris--)
			{
				Pos[0] = APhysXConverter::N2A_Vector3(Triangles->p0);
				Pos[1] = APhysXConverter::N2A_Vector3(Triangles->p1);
				Pos[2] = APhysXConverter::N2A_Vector3(Triangles->p2);
				pFC->AddRenderData_3D(Pos, 3, TriID, 3, GetTransparentColor(Triangles->color, 0x7f));

				Triangles++;

			}

			pWC->Flush();
			pFC->Flush();
		}


	}

}

void APhysXDebugRenderer::RenderExtraDebugData(APhysXScene* pScene)
{

	
	if(m_pA3DEngine && pScene)
	{
		A3DWireCollector* pWC = m_pA3DEngine->GetA3DWireCollector();
/*
		// render the bounding box of all cloths...
		for(int i = 0; i < pNxScene->getNbCloths(); i++)
		{
			NxCloth* pCloth = pNxScene->getCloths()[i];
			NxBounds3 bound;
			pCloth->getWorldBounds(bound);
			A3DAABB aabb;
			aabb.Mins = APhysXConverter::N2A_Vector3(bound.min);
			aabb.Maxs = APhysXConverter::N2A_Vector3(bound.max);
			aabb.CompleteCenterExts();

			pWC->AddAABB(aabb, 0xffff0000);
		}
*/

		// render the aabb of all objects...
		for(int i=0; i<pScene->GetPhysXObjectNum(); i++)
		{
			NxBounds3 aabb;
			APhysXObject* pObject = pScene->GetPhysXObject(i);
			pObject->GetAABB(aabb);

			A3DAABB a_aabb;
			APhysXConverter::N2A_AABB(aabb, a_aabb);

			pWC->AddAABB(a_aabb, 0xffffff00);

		}

	}


}

void APhysXDebugRenderer::PreRender()
{
	m_iTextCurX = m_iTextStartX;
	m_iTextCurY = m_iTextStartY;
}

inline void APhysXDebugRenderer::RenderLineText(const ACHAR* szOutput, DWORD dwColor)
{

#if APHYSX_ANGELICA_VERSION < 300
	m_pFont->TextOut(m_iTextCurX, m_iTextCurY, szOutput, dwColor);
#else
	m_pFont->AddText(m_iTextCurX, m_iTextCurY, szOutput, a_strlen(szOutput), dwColor);
#endif

	m_iTextCurY+= m_iFontHeight + 1;
}

void APhysXDebugRenderer::RenderPerfItem(const APhysXPerfItem* pPerfItem, DWORD dwColor)
{
	if(!pPerfItem) return;

	ACString strOutput;
	strOutput.Format(_AL("  FrameTime: %7.3f Ms, AvgFrameTime: %7.3f Ms, MaxFrameTime: %7.3f Ms"), pPerfItem->GetCurFrameMs(), pPerfItem->GetAvgFrameMs(), pPerfItem->GetMaxFrameSecondsInLastWindow() * 1000);
	strOutput = TrimString(AS2AC(pPerfItem->GetName()), 20) + strOutput;

	RenderLineText(strOutput,dwColor);
}

void APhysXDebugRenderer::RenderPerfStats(const APhysXPerfStats* pPerfStats, DWORD dwColor)
{

	RenderLineText(_AL("========== Performance Stats =========="), dwColor);

	// render simulation related performance 
	const APhysXPerfItem* pPerfItem = NULL;
	for(int i=0; i<pPerfStats->GetPerfStatsNum(); i++)
	{
		pPerfItem = pPerfStats->GetPerfItem(i);
		RenderPerfItem(pPerfItem, dwColor);
	}
}


void APhysXDebugRenderer::RenderSceneStats(NxScene* pNxScene, DWORD dwColor)
{
	const int iTrimLen = TRIM_TEXT_LEN;
	NxSceneStats sceneStats;
	pNxScene->getStats(sceneStats);

	RenderLineText(_AL("============= Scene Stats ============="), dwColor);
	ACString strOutput;
	
	// num of actors
	strOutput.Format(_AL("    %d"), sceneStats.numActors);
	strOutput = TrimString(_AL("numActors:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of dynamic actors
	strOutput.Format(_AL("    %d"), sceneStats.numDynamicActors);
	strOutput = TrimString(_AL("numDynamicActors:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);
	
	// num of awake dynamic actors
	strOutput.Format(_AL("    %d"), sceneStats.numDynamicActorsInAwakeGroups);
	strOutput = TrimString(_AL("numAwakeDynamicActors:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of pairs
	strOutput.Format(_AL("    %d"), sceneStats.numPairs);
	strOutput = TrimString(_AL("numPairs:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of contacts
	strOutput.Format(_AL("    %d"), sceneStats.numContacts);
	strOutput = TrimString(_AL("numContacts:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of joints
	strOutput.Format(_AL("    %d"), sceneStats.numJoints);
	strOutput = TrimString(_AL("numJoints:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of static shapes
	strOutput.Format(_AL("    %d"), sceneStats.numStaticShapes);
	strOutput = TrimString(_AL("numStaticShapes:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of dynamic shapes
	strOutput.Format(_AL("    %d"), sceneStats.numDynamicShapes);
	strOutput = TrimString(_AL("numDynamicShapes:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of materials
	strOutput.Format(_AL("    %d"), pNxScene->getNbMaterials());
	strOutput = TrimString(_AL("numMaterials:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of forcefields
	strOutput.Format(_AL("    %d"), pNxScene->getNbForceFields());
	strOutput = TrimString(_AL("numForceFields:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of cloths and cloth vertices...
	int iClothNum = pNxScene->getNbCloths();
	int iClothVertexNum = 0;
	for(int i=0; i<iClothNum; i++)
	{
		iClothVertexNum += pNxScene->getCloths()[i]->getNumberOfParticles();
	}
	strOutput.Format(_AL("    %d"), iClothNum);
	strOutput = TrimString(_AL("numCloths:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);
	strOutput.Format(_AL("    %d"), iClothVertexNum);
	strOutput = TrimString(_AL("numTotalClothVertices:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of fluids and fluid particles...
	int iFluidNum = pNxScene->getNbFluids();
	int iFluidParticleNum = 0;
	for(int i=0; i<iFluidNum; i++)
	{
		NxParticleData particleData = pNxScene->getFluids()[i]->getParticlesWriteData();
		iFluidParticleNum += *particleData.numParticlesPtr;
	}
	strOutput.Format(_AL("    %d"), iFluidNum);
	strOutput = TrimString(_AL("numFluids:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);
	strOutput.Format(_AL("    %d"), iFluidParticleNum);
	strOutput = TrimString(_AL("numTotalFluidParticles:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

#if APHYSX_SUPPORT_COMPARTMENT

	// num of compartments...
	strOutput.Format(_AL("    %d"), pNxScene->getNbCompartments());
	strOutput = TrimString(_AL("numCompartments:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

#endif
	
	APhysXScene* pAPhysXScene = (APhysXScene*)(pNxScene->userData);
	if(pAPhysXScene)
	{
		static int s_iAccumCounter = 0;
		static float s_fAccumSimTime = 0.0f;
		static float s_fMaxSimTime = 0.0f;
		static float s_fAccumSubTimeStep = 0.0f;
		static float s_fMaxSubTimeStep = 0.0f;
		static float s_fAccumIterNum = 0.0f;
		static float s_fMaxIterNum = 0.0f;
		
		static float s_fSumSimTime = 0.0f;
		static float s_fSumSubTimeStep = 0.0f;
		static float s_fSumIterNum = 0.0f;

		static float s_fLastMaxSimTime = 0.0f;
		static float s_fLastMaxSubTimeStep = 0.0f;
		static float s_fLastMaxIterNum = 0.0f;

		float fTimeStep = pAPhysXScene->GetSimulateTimeStep();
		float fSimTime = pAPhysXScene->GetCurFrameDt();
		float fSteps = fSimTime/fTimeStep;

		s_iAccumCounter++;

		s_fAccumSimTime += fSimTime;
		s_fAccumSubTimeStep += fTimeStep;
		s_fAccumIterNum += fSteps;
		
		if(fSimTime > s_fMaxSimTime) s_fMaxSimTime = fSimTime;
		if(fTimeStep> s_fMaxSubTimeStep) s_fMaxSubTimeStep = fTimeStep;
		if(fSteps > s_fMaxIterNum) s_fMaxIterNum = fSteps;

#define  FRAME_WINDOW_NUM 30
		if(s_iAccumCounter >= FRAME_WINDOW_NUM)
		{
			s_fSumSimTime = s_fAccumSimTime;
			s_fSumSubTimeStep = s_fAccumSubTimeStep;
			s_fSumIterNum = s_fAccumIterNum;

			s_fLastMaxSimTime = s_fMaxSimTime;
			s_fLastMaxSubTimeStep = s_fMaxSubTimeStep;
			s_fLastMaxIterNum = s_fMaxIterNum;

			s_iAccumCounter = 0;

			s_fAccumSimTime = 0;
			s_fAccumSubTimeStep = 0;
			s_fAccumIterNum = 0;

			s_fMaxSimTime = 0;
			s_fMaxSubTimeStep = 0;
			s_fMaxIterNum = 0;
		}

		strOutput.Format(_AL("    %4.1f Ms, Avg:    %4.1f Ms, Max:    %4.1f"), fSimTime * 1000, s_fSumSimTime * 1000 / FRAME_WINDOW_NUM, s_fLastMaxSimTime * 1000);
		strOutput = TrimString(_AL("Simulation Time:"), iTrimLen) + strOutput;
		RenderLineText(strOutput, dwColor);

		strOutput.Format(_AL("    %4.1f Ms, Avg:    %4.1f Ms, Max:    %4.1f"), fTimeStep * 1000, s_fSumSubTimeStep * 1000 / FRAME_WINDOW_NUM, s_fLastMaxSubTimeStep * 1000 );
		strOutput = TrimString(_AL("Sub TimeStep:"), iTrimLen) + strOutput;
		RenderLineText(strOutput, dwColor);

		strOutput.Format(_AL("    %4.1f   , Avg:    %4.1f   , Max:    %4.1f"), fSteps, s_fSumIterNum / FRAME_WINDOW_NUM, s_fLastMaxIterNum);
		strOutput = TrimString(_AL("Iteration Num:"), iTrimLen) + strOutput;
		RenderLineText(strOutput, dwColor);

	}

}

void APhysXDebugRenderer::RenderPhysXSDKStats(NxPhysicsSDK* pSDK, DWORD dwColor)
{
	const int iTrimLen = TRIM_TEXT_LEN;
	RenderLineText(_AL("========== NxPhysicsSDK Stats =========="), dwColor);
	ACString strOutput;	

	// PhysX SDK Version
	strOutput.Format(_AL("    %d.%d.%d"), NX_SDK_VERSION_MAJOR, NX_SDK_VERSION_MINOR, NX_SDK_VERSION_BUGFIX);
	strOutput = TrimString(_AL("PhysX SDK Version:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// is hardware PhysX available?...
	if(gPhysXEngine->HasPhysXHardware())
		strOutput.Format(_AL("    Yes"));
	else
		strOutput.Format(_AL("    No"));
	strOutput = TrimString(_AL("PhysX Hardware Available:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// is hardware PhysX enabled?
	if(gPhysXEngine->IsHardwarePhysXEnabled())
		strOutput.Format(_AL("    Yes"));
	else
		strOutput.Format(_AL("    No"));
	strOutput = TrimString(_AL("Hardware PhysX Enabled:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// is PerfStats function enabled?
	if(gPhysXEngine->GetFlag(APX_ENGINEFLAG_ENABLEPROFILE))
		strOutput.Format(_AL("    Yes"));
	else
		strOutput.Format(_AL("    No"));
	strOutput = TrimString(_AL("Performance Profile Enabled:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// is PerfStats log enabled?
	if(gPhysXEngine->GetFlag(APX_ENGINEFLAG_LOGPROFILE))
		strOutput.Format(_AL("    Yes"));
	else
		strOutput.Format(_AL("    No"));
	strOutput = TrimString(_AL("Performance Log Enabled:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of NxScenes
	strOutput.Format(_AL("    %d"), pSDK->getNbScenes());
	strOutput = TrimString(_AL("numNxScenes:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of CCD Skeletons
	strOutput.Format(_AL("    %d"), pSDK->getNbCCDSkeletons());
	strOutput = TrimString(_AL("numCCDSkeletons:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of Height Fields
	strOutput.Format(_AL("    %d"), pSDK->getNbHeightFields());
	strOutput = TrimString(_AL("numHeightFields:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of Convex Meshes
	strOutput.Format(_AL("    %d"), pSDK->getNbConvexMeshes());
	strOutput = TrimString(_AL("numConvexMeshes:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of Triangle Meshes
	strOutput.Format(_AL("    %d"), pSDK->getNbTriangleMeshes());
	strOutput = TrimString(_AL("numTriangleMeshes:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of Cloth Meshes
	strOutput.Format(_AL("    %d"), pSDK->getNbClothMeshes());
	strOutput = TrimString(_AL("numClothMeshes:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// num of Softbody Meshes
	strOutput.Format(_AL("    %d"), pSDK->getNbSoftBodyMeshes());
	strOutput = TrimString(_AL("numSoftbodyMeshes:"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);

	// memory allocated by PhysX internally...
#if APHYSX_TRACE_MEMORY_ALLOC

	APhysXAllocator* pAllocater = gPhysXEngine->GetAllocator();

	strOutput.Format(_AL("    %d"), pAllocater->GetAllocMemSize());
	strOutput = TrimString(_AL("AllocMemSize(Bytes):"), iTrimLen) + strOutput;
	RenderLineText(strOutput, dwColor);
	
#endif

}

void APhysXDebugRenderer::RenderForceField(NxForceField& ff, DWORD dwColor)
{
	assert(0 != m_pA3DEngine);
	if(!m_pA3DEngine)
		return;

	NxMat34 ffPose = ff.getPose();
	NxActor* pActor = ff.getActor();

	NxForceFieldShapeGroup& FFSG = ff.getIncludeShapeGroup();
	FFSG.resetShapesIterator();
	NxU32 nCount = FFSG.getNbShapes();
	for (NxU32 i = 0; i < nCount; ++i)
	{
		NxForceFieldShape* pFFS = FFSG.getNextShape();
		NxMat34 mat = pFFS->getPose();
		mat.multiply(ffPose, mat);
		if (pActor)
			mat.multiply(pActor->getGlobalPose(), mat);

		RenderForceFieldShape(*pFFS, dwColor, &mat);
	}

	ff.resetShapeGroupsIterator();
	NxU32 nSGCount = ff.getNbShapeGroups();
	for (NxU32 j = 0; j < nSGCount; ++j)
	{
		NxForceFieldShapeGroup* pFFSG = ff.getNextShapeGroup();
		pFFSG->resetShapesIterator();
		if (NX_FFSG_EXCLUDE_GROUP == pFFSG->getFlags())
			dwColor = 0xff000000 | ~dwColor;

		NxU32 nffsCount = pFFSG->getNbShapes();
		for (NxU32 k = 0; k < nffsCount; ++k)
		{
			NxForceFieldShape* pFFS = FFSG.getNextShape();
			RenderForceFieldShape(*pFFS, dwColor, 0);
		}
	}
}

void APhysXDebugRenderer::RenderForceFieldShape(NxForceFieldShape& ffs, DWORD dwColor, NxMat34* pPose)
{
	A3DWireCollector* pWC = m_pA3DEngine->GetA3DWireCollector();
	if (0 == pWC) return;

	NxMat34 mat = ffs.getPose();
	NxMat34* pMat = &mat;
	if (0 != pPose)
		pMat = pPose;

	NxBoxForceFieldShape* pBox = ffs.isBox();
	if (0 != pBox)
	{
		APhysXDrawBasic::DrawBox(*pWC, NxBox(pMat->t, pBox->getDimensions(), pMat->M), GetTransparentColor(dwColor));
		return;
	}

	NxSphereForceFieldShape* pSphere = ffs.isSphere();
	if (0 != pSphere)
	{
		APhysXDrawBasic::DrawSphere(*pWC, pSphere->getRadius(), GetTransparentColor(dwColor), 0xffffffff, pMat);
		return;
	}
	NxCapsuleForceFieldShape* pCapsule = ffs.isCapsule();
	if (0 != pCapsule)
	{
		APhysXDrawBasic::DrawCapsule(*pWC, pCapsule->getRadius(), pCapsule->getHeight(), GetTransparentColor(dwColor), 0xffffffff, pMat);
		return;
	}
	NxConvexForceFieldShape* pConvex = ffs.isConvex();
	if (0 != pConvex)
	{
		NxConvexForceFieldShapeDesc desc;
		pConvex->saveToDesc(desc);
		NxConvexMeshDesc meshDesc;
		desc.meshData->saveToDesc(meshDesc);
		APhysXDrawBasic::DrawConvex(*pWC, meshDesc, GetTransparentColor(dwColor), pMat);
		return;
	}
}

void APhysXDebugRenderer::RenderCC(const APhysXCCBase& cc, DWORD dwColor)
{
	A3DWireCollector* pWC = m_pA3DEngine->GetA3DWireCollector();
	if (0 == pWC) return;

	NxMat34 mat;
	mat.M = cc.GetGlobalOrientation();
	mat.t = cc.GetCenterPosition();

	APhysXCCBase::APhysXVolumeType vt;
	const NxVec3 vol = cc.GetVolume(vt);
	switch(vt)
	{
	case APhysXCCBase::APX_VT_BOX:
		APhysXDrawBasic::DrawBox(*pWC, NxBox(mat.t, vol, mat.M), GetTransparentColor(dwColor));
		break;
	case APhysXCCBase::APX_VT_CAPSULE:
		APhysXDrawBasic::DrawCapsule(*pWC, vol.x, (vol.y - vol.x * 2), GetTransparentColor(dwColor), APhysXDrawBasic::DF_NODRAW_DIM_XOZ, &mat);
		break;
	default:
		break;
	}
}

float APhysXDrawBasic::cosVal[nAllSegment] = { 0 };
float APhysXDrawBasic::sinVal[nAllSegment] = { 0 };
void APhysXDrawBasic::InitSinCos()
{
	static bool bInited = false;
	if (bInited)
		return;
	
	float Delta = APHYSX_PI / nSegmentInSemiCircle;
	
	float Theta = Delta;
	for(int i = 0; i < nAllSegment; ++i)
	{
		cosVal[i] = NxMath::cos(Theta);
		sinVal[i] = NxMath::sin(Theta);
		Theta += Delta;
	}
	bInited = true;
}

void APhysXDrawBasic::DrawBox(A3DWireCollector& a3dwc, const NxBox& box, NxU32 color)
{
	NxVec3 pp[8];
	NxComputeBoxPoints(box, pp);
	
	const NxU32* Indices = NxGetBoxEdges();
	for(NxU32 i=0;i<12;i++)
	{
		NxU32 VRef0 = *Indices++;
		NxU32 VRef1 = *Indices++;
		a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(pp[VRef0]), APhysXConverter::N2A_Vector3(pp[VRef1]), color);
	}
}

void APhysXDrawBasic::DrawSphere(A3DWireCollector& a3dwc, float radius, NxU32 color, int flags, const NxMat34* pPose)
{
	if (!(DF_DRAW_DIM_XOY & flags) && !(DF_DRAW_DIM_XOZ & flags) && !(DF_DRAW_DIM_ZOY & flags))
		return;

	float ySign = 1.0f;
	if (DF_DRAW_BOTTOM_SEMISPHERE & flags)
		ySign = -1.0f;

	int nAllCount = nSegmentInSemiCircle;
	if ((DF_DRAW_WHOLE_SPHERE & flags) || 
		((DF_DRAW_TOP_SEMISPHERE & flags) && (DF_DRAW_BOTTOM_SEMISPHERE & flags)))
	{
		nAllCount = nAllSegment;
		ySign = 1.0f;
	}
	
	A3DMATRIX4 mat;
	mat.Identity();
	if (0 != pPose)
		APhysXConverter::N2A_Matrix44(*pPose, mat);
	
	int i;
	A3DVECTOR3 p0, p1;
	InitSinCos();
	
	// in xoy plane
	if (DF_DRAW_DIM_XOY & flags)
	{
		p0.Set(-radius, 0, 0);
		p0 = p0 * mat;
		for(i = 0; i < nAllCount; ++i)
		{
			p1.Set(0, 0, 0);
			p1.x -= radius * cosVal[i];
			p1.y += radius * sinVal[i] * ySign;
			p1 = p1 * mat;
			a3dwc.Add3DLine(p0, p1, color);
			p0 = p1;
		}
	}

	
	// in zoy plane
	if (DF_DRAW_DIM_ZOY & flags)
	{
		p0.Set(0, 0, -radius);
		p0 = p0 * mat;
		for(i = 0; i < nAllCount; ++i)
		{
			p1.Set(0, 0, 0);
			p1.z -= radius * cosVal[i];
			p1.y += radius * sinVal[i] * ySign;
			p1 = p1 * mat;
			a3dwc.Add3DLine(p0, p1, color);
			p0 = p1;
		}
	}
	
	// in xoz plane
	if (DF_DRAW_DIM_XOZ & flags)
	{
		p0.Set(0, 0, -radius);
		p0 = p0 * mat;
		for(i = 0; i < nAllSegment; ++i)
		{
			p1.Set(0, 0, 0);
			p1.z -= radius * cosVal[i];
			p1.x += radius * sinVal[i];
			p1 = p1 * mat;
			a3dwc.Add3DLine(p0, p1, color);
			p0 = p1;
		}
	}
}

void APhysXDrawBasic::DrawCapsule(A3DWireCollector& a3dwc, float radius, float height, NxU32 color, int flags, const NxMat34* pPose)
{
	if (!(DF_DRAW_DIM_XOY & flags) && !(DF_DRAW_DIM_XOZ & flags) && !(DF_DRAW_DIM_ZOY & flags))
		return;

	bool IsAllSphere = APhysXDrawBasic::DF_DRAW_WHOLE_SPHERE & flags;
	if (0xffffffff == flags)
	{
		flags = DF_DRAW_DIM_ALL;
		IsAllSphere = false;
	}

	float halfH = height * 0.5f;
	NxMat34 mat;
	mat.id();
	if (0 != pPose)
		mat = *pPose;

	NxVec3 xn0(-radius, halfH, 0);
	NxVec3 xn1(-radius, -halfH, 0);
	mat.multiply(xn0, xn0);
	mat.multiply(xn1, xn1);
	a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(xn0), APhysXConverter::N2A_Vector3(xn1), color);

	NxVec3 xp0(radius, halfH, 0);
	NxVec3 xp1(radius, -halfH, 0);
	mat.multiply(xp0, xp0);
	mat.multiply(xp1, xp1);
	a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(xp0), APhysXConverter::N2A_Vector3(xp1), color);

	NxVec3 zn0(0, halfH, -radius);
	NxVec3 zn1(0, -halfH, -radius);
	mat.multiply(zn0, zn0);
	mat.multiply(zn1, zn1);
	a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(zn0), APhysXConverter::N2A_Vector3(zn1), color);

	NxVec3 zp0(0, halfH, radius);
	NxVec3 zp1(0, -halfH, radius);
	mat.multiply(zp0, zp0);
	mat.multiply(zp1, zp1);
	a3dwc.Add3DLine(APhysXConverter::N2A_Vector3(zp0), APhysXConverter::N2A_Vector3(zp1), color);

	NxVec3 p0(0, halfH, 0);
	NxVec3 p1(0, -halfH, 0);
	mat.multiply(p0, p0);
	mat.multiply(p1, p1);
	mat.t = p0;
	if (!IsAllSphere) 
	{
		flags |= APhysXDrawBasic::DF_DRAW_TOP_SEMISPHERE;
		flags &= ~APhysXDrawBasic::DF_DRAW_BOTTOM_SEMISPHERE;
	}
	APhysXDrawBasic::DrawSphere(a3dwc, radius, color, flags, &mat);
	mat.t = p1;
	if (!IsAllSphere) 
	{
		flags &= ~APhysXDrawBasic::DF_DRAW_TOP_SEMISPHERE;
		flags |= APhysXDrawBasic::DF_DRAW_BOTTOM_SEMISPHERE;
	}
	APhysXDrawBasic::DrawSphere(a3dwc, radius, color, flags, &mat);
}

void APhysXDrawBasic::DrawConvex(A3DWireCollector& a3dwc, const NxConvexMeshDesc& meshDesc, NxU32 color, const NxMat34* pPose)
{
	static APhysXArray<A3DVECTOR3> vtxArray;
	vtxArray.clear();
	GenerateVertices(meshDesc.points, meshDesc.pointStrideBytes, meshDesc.numVertices, vtxArray, pPose);
	
	static APhysXArray<WORD> vtxIndices;
	vtxIndices.clear();
	bool bIs16Bit = false;
	if (meshDesc.flags & NX_CF_16_BIT_INDICES)	bIs16Bit = true;
	GenerateIndices(meshDesc.triangles, meshDesc.triangleStrideBytes, meshDesc.numTriangles, vtxIndices, bIs16Bit);
	
	a3dwc.AddRenderData_3D(&vtxArray[0], vtxArray.size(), &vtxIndices[0], vtxIndices.size(), color);
}

void APhysXDrawBasic::DrawTriangle(A3DWireCollector& a3dwc, const NxTriangleMeshDesc& meshDesc, NxU32 color, const NxMat34* pPose)
{
	static APhysXArray<A3DVECTOR3> vtxArray;
	vtxArray.clear();
	GenerateVertices(meshDesc.points, meshDesc.pointStrideBytes, meshDesc.numVertices, vtxArray, pPose);
	
	static APhysXArray<WORD> vtxIndices;
	vtxIndices.clear();
	bool bIs16Bit = false;
	if (meshDesc.flags & NX_CF_16_BIT_INDICES)	bIs16Bit = true;
	GenerateIndices(meshDesc.triangles, meshDesc.triangleStrideBytes, meshDesc.numTriangles, vtxIndices, bIs16Bit);
	
	a3dwc.AddRenderData_3D(&vtxArray[0], vtxArray.size(), &vtxIndices[0], vtxIndices.size(), color);
}

void APhysXDrawBasic::GenerateVertices(const void* points, const NxU32 pointStrideBytes, const NxU32 numVertices, APhysXArray<A3DVECTOR3>& outvtxArray, const NxMat34* pMat)
{
	NxU32 nSize = outvtxArray.size() + numVertices;
	outvtxArray.reserve(nSize);
	
	NxVec3 pos;
	
	const char* pBytes = static_cast<const char*>(points);
	for (NxU32 i = 0; i < numVertices; ++i)
	{
		const NxVec3* pVec3 = reinterpret_cast<const NxVec3*>(pBytes);
		pos = *pVec3;
		if (0 != pMat) pMat->multiply(*pVec3, pos);
		outvtxArray.push_back(APhysXConverter::N2A_Vector3(pos));
		pBytes += pointStrideBytes; 
	}
}

void APhysXDrawBasic::GenerateIndices(const void* triangles, const NxU32 triangleStrideBytes, const NxU32 numTriangles, APhysXArray<WORD>& outvtxArray, const bool is16BitIndices)
{
	NxU32 nSize = outvtxArray.size() + numTriangles * 3;
	outvtxArray.reserve(nSize);
	
	const char* pBytes = static_cast<const char*>(triangles);
	if (is16BitIndices)
	{
		for (NxU32 i = 0; i < numTriangles; ++i)
		{
			const NxU16* pU16 = reinterpret_cast<const NxU16*>(pBytes);
			outvtxArray.push_back(pU16[0]);
			outvtxArray.push_back(pU16[1]);
			outvtxArray.push_back(pU16[0]);
			outvtxArray.push_back(pU16[2]);
			outvtxArray.push_back(pU16[1]);
			outvtxArray.push_back(pU16[2]);
			pBytes += triangleStrideBytes; 
		}
	}
	else
	{
		for (NxU32 i = 0; i < numTriangles; ++i)
		{	
			const NxU32* pU32 = reinterpret_cast<const NxU32*>(pBytes);
			outvtxArray.push_back(pU32[0]);
			outvtxArray.push_back(pU32[1]);
			outvtxArray.push_back(pU32[0]);
			outvtxArray.push_back(pU32[2]);
			outvtxArray.push_back(pU32[1]);
			outvtxArray.push_back(pU32[2]);
			pBytes += triangleStrideBytes; 
		}
	}
}
