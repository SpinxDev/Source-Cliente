/*
 * FILE: Exportor.cpp
 *
 * DESCRIPTION: Exportor routines
 *
 * CREATED BY: duyuxin, 2003/11/19
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Exportor.h"
#include "MAXInterface.h"
#include "SkeletonCandidate.h"
#include "SkinCandidate.h"
#include "BoneCandidate.h"
#include "MeshCandidate.h"
#include "VertexCandidate.h"
#include "PhysiqueCandidate.h"
#include "SheetSkinMesh.h"
#include "MorphChannel.h"
#include "InterfaceWrapper.h"
#include <Dummy.h>
#include <simpobj.h>

#include <A3DEngine.h>
#include <A3DSkinModel.h>
#include <A3DBone.h>
#include <A3DJoint.h>
#include <A3DTexture.h>
#include <A3DMorphRigidMesh.h>
#include <A3DMorphSkinMesh.h>
#include <A3DSuppleMesh.h>
#include <A3DMuscleMesh.h>
#include <A3DTrackMan.h>
#include <afi.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	See A3DSkinMesh.h for detail of SKIN_BUMP_ENABLE
#undef SKIN_BUMP_ENABLE

#define PB_CYL_RADIUS		0		//	Cylinder radius parameter index
#define PB_CYL_HEIGHT		1		//	Cylinder height parameter index
#define PB_SPHERE_RADIUS	0		//	Sphere radius parameter index

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

struct JOINTITEM
{
	A3DAnimJoint*		pJoint;
	A3DVector3Track*	pPosTrack;
	A3DQuaternionTrack*	pRotTrack;
};

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

//	Compare position
static inline bool _PosEqual(const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	if (fabs(v1.x - v2.x) < 0.0001f &&
		fabs(v1.y - v2.y) < 0.0001f &&
		fabs(v1.z - v2.z) < 0.0001f)
		return true;

	return false;
}

//	Compare orientation
static inline bool _OrientationEqual(const A3DQUATERNION& q1, const A3DQUATERNION& q2)
{
	if (fabs(q1.x - q2.x) < 0.000001f &&
		fabs(q1.y - q2.y) < 0.000001f &&
		fabs(q1.z - q2.z) < 0.000001f &&
		fabs(q1.w - q2.w) < 0.000001f)
		return true;

	return false;
}

//	Create position track
static A3DVector3Track* _CreatePosTrack(int iNumKey, int iFrameRate, int iNumSeg)
{
	A3DVector3Track* pPosTrack = new A3DVector3Track;
	if (!pPosTrack)
		return NULL;

	if (!pPosTrack->Create(iNumKey, iFrameRate, iNumSeg))
	{
		delete pPosTrack;
		return NULL;
	}

	return pPosTrack;
}

//	Create rotation track
static A3DQuaternionTrack* _CreateRotTrack(int iNumKey, int iFrameRate, int iNumSeg)
{
	A3DQuaternionTrack* pRotTrack = new A3DQuaternionTrack;
	if (!pRotTrack)
		return NULL;

	if (!pRotTrack->Create(iNumKey, iFrameRate, iNumSeg))
	{
		delete pRotTrack;
		return false;
	}

	pRotTrack->SetInterType(TRACKDATA_INTERTYPE_SLERP);

	return pRotTrack;
}

//	Frame to time
static int _FrameToTime(int iFrame, int iFPS)
{
	return (int)(iFrame * 1000.0f / iFPS);
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CExportor
//
///////////////////////////////////////////////////////////////////////////

CExportor::CExportor()
{
	m_ei = NULL;
	m_i	 = NULL;

	m_pTrackSet				= NULL;
	m_pSkeleton				= NULL;
	m_pSkin					= NULL;
	m_pSkinModel			= NULL;
	m_pSkeletonCandidate	= NULL;
	m_pSkinCandidate		= NULL;
	m_pPhyCandidate			= NULL;
}

CExportor::~CExportor()
{
}

//	Initialize object
bool CExportor::Init(ExpInterface* ei, Interface* i)
{
	m_ei = ei;
	m_i	 = i;

	return true;
}

//	Export skeleton file
bool CExportor::ExportSkeleton(const char* szFile, CSkeletonCandidate* pSkeletonCandidate)
{
	//	Build the selected ids of the bone candidates
	m_iNumSelectedBone = pSkeletonCandidate->GetSelectedBoneNum();
	if (!m_iNumSelectedBone)
	{
		AfxMessageBox("No bones selected to export.", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	//	Set bone init matrices
	pSkeletonCandidate->SetBoneInitMatrices();

	CSkeletonWrapper Skeleton;
	CTrackSetWrapper TrackSet;
	
	m_pSkeleton	= &Skeleton;
	m_pTrackSet = &TrackSet;
	m_pSkeletonCandidate = pSkeletonCandidate;

	//	Use file name as skeleton's name
//	char szName[256];
//	AFileMod_GetFileTitle((char*)name, szName, 256);
	
	if (!Skeleton.Init())
	{
		AfxMessageBox("Failed to create A3D skeleton object", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	int iOutStartFrame = 0;
	int iOutEndFrame = pSkeletonCandidate->GetEndFrame() - pSkeletonCandidate->GetStartFrame();
	Skeleton.SetAnimFPS(pSkeletonCandidate->GetAnimFPS());
	TrackSet.SetAnimFPS(pSkeletonCandidate->GetAnimFPS());
	TrackSet.SetAnimFrameRange(iOutStartFrame, iOutEndFrame);

	a_SendLogToDebugWnd(0, "Emit bones ...");

	//	Emit bones
	if (!EmitBones())
	{
		AfxMessageBox("Failed to export bones", MB_OK | MB_ICONEXCLAMATION);
		Skeleton.Release();
		return false;
	}

	//	Emit hooks
	if (!EmitHooks())
	{
		AfxMessageBox("Failed to export hooks", MB_OK | MB_ICONEXCLAMATION);
		Skeleton.Release();
		return false;
	}

	a_SendLogToDebugWnd(0, "Emit animations ...");
	a_SendLogToDebugWnd(0, "Frame range: (%d -- %d), FPS: %d", TrackSet.GetAnimStartFrame(), TrackSet.GetAnimEndFrame(), TrackSet.GetAnimFPS());

	//	Emit animations
	if (!EmitAnimations())
	{
		AfxMessageBox("Failed to export animations", MB_OK | MB_ICONEXCLAMATION);
		Skeleton.Release();
		return false;
	}

	a_SendLogToDebugWnd(0, "Save skeleton file ...");

	//	Write skeleton file
	if (!Skeleton.Save(szFile))
	{
		AfxMessageBox("Failed to save skeleton file", MB_OK | MB_ICONEXCLAMATION);
		Skeleton.Release();
		return false;
	}

	//	Release resources
	Skeleton.Release();

	//	Save track set data to .stck file
	SaveTrackSetData(szFile);

	return true;
}

//	Save track set data to file
bool CExportor::SaveTrackSetData(const char* szSkeletonFile)
{
/*	char szTckPath[MAX_PATH], szTitle[MAX_PATH];
	af_GetFilePath(szSkeletonFile, szTckPath, MAX_PATH);
	af_GetFileTitle(szSkeletonFile, szTitle, MAX_PATH);
	AUX_ChangeFileExt(szTitle, MAX_PATH, "");

	strcat(szTckPath, "\\tcks_");
	strcat(szTckPath, szTitle);
	AUX_CreateDirectory(szTckPath);

	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\%s.stck", szTckPath, szTitle);
*/
	char szFile[MAX_PATH];
	strcpy(szFile, szSkeletonFile);
	AUX_ChangeFileExt(szFile, MAX_PATH, ".stck");

	if (!m_pTrackSet->Save(szFile))
	{
		AfxMessageBox("Failed to save track set file", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

//	Export skin file
bool CExportor::ExportSkin(const char* szFile, CSkinCandidate* pSkinCandidate, 
						CSkeletonCandidate* pSkeletonCandidate)
{
	A3DEngine* pA3DEngine = new A3DEngine;

	CSkinWrapper Skin;

	if (!Skin.Init(pA3DEngine))
	{
		AfxMessageBox("Failed to initialize A3DSkin object", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	m_pSkin					= &Skin;
	m_pSkinCandidate		= pSkinCandidate;
	m_pSkeletonCandidate	= pSkeletonCandidate;

	Skin.SetMinWeight(pSkinCandidate->GetMinWeight());
	Skin.SetSkeletonBoneNum(pSkeletonCandidate->GetSelectedBoneNum());

	//	Build skin bone name list
	EmitSkinBoneNames();

	//	Build texture path
	char szTexPath[MAX_PATH], szTitle[MAX_PATH];
	af_GetFilePath(szFile, szTexPath, MAX_PATH);
	af_GetFileTitle(szFile, szTitle, MAX_PATH);
	AUX_ChangeFileExt(szTitle, MAX_PATH, "");

	if (pSkinCandidate->GetExtraTexDirFlag())
	{
		strcat(szTexPath, "\\Tex_");
		strcat(szTexPath, szTitle);
		strcat(szTexPath, "\\");
	}
	else
		strcat(szTexPath, "\\Textures\\");

	//	Emit textures and materials
	if (!EmitTexturesAndMaterials(szTexPath))
	{
		AfxMessageBox("Failed to emit textures and materials", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	//	Emit meshes
	if (!EmitMeshes())
	{
		AfxMessageBox("Failed to emit meshes", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	//	Emit bone bounding boxes
/*	if (!EmitBoneBoxes(&Skin, pSkinCandidate))
	{
		AfxMessageBox("Failed to emit bone bounding boxes", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
*/
	//	Save data to file
	if (!Skin.Save(szFile))
	{
		AfxMessageBox("Failed to save skin file", MB_OK | MB_ICONEXCLAMATION);
		ReleaseTempRes(&Skin);
		Skin.Release();
		return false;
	}

	//	Update skeleton file if necessary
	if (pSkeletonCandidate->GetUpdateFileFlag())
	{
		if (!UpdateSkeletonFile(pSkeletonCandidate->GetFileName(), pSkeletonCandidate))
			AfxMessageBox("Failed to update skeleton file", MB_OK | MB_ICONEXCLAMATION);
	}

	//	Release resources
	ReleaseTempRes(&Skin);
	Skin.Release();

	delete pA3DEngine;

	return true;
}

//	Export skin model file
bool CExportor::ExportSkinModel(const char* szFile, const char* szSkeleton, 
								const char* szSkin, const char* szPhysique)
{
	A3DSkinModel SkinModel;

	const char* aSkinFiles[1];
	aSkinFiles[0] = szSkin;

	if (!SkinModel.EmptySave(szFile, szSkeleton, aSkinFiles, 1, szPhysique))
	{
		AfxMessageBox("Failed to save skin model file", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

//	Export physique file
bool CExportor::ExportPhysique(const char* szFile, CPhysiqueCandidate* pPhyCandidate, CSkeletonCandidate* pSkeletonCandidate)
{
	A3DEngine* pA3DEngine = new A3DEngine;

	A3DSkinModel SkinModel;

	if (!SkinModel.Init(pA3DEngine))
		return false;

	m_pSkinModel			= &SkinModel;
	m_pSkeletonCandidate	= pSkeletonCandidate;
	m_pPhyCandidate			= pPhyCandidate;

	//	Emit hit boxes
	if (!EmitHitBoxes())
	{
		AfxMessageBox("Failed to emit hit boxes", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	//	Emit physique shapes
	if (!EmitPhysiqueShapes())
	{
		AfxMessageBox("Failed to emit hit physique shapes", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!SkinModel.SavePhysique(szFile))
	{
		AfxMessageBox("Failed to save physique data", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	SkinModel.Release();

	delete pA3DEngine;

	return true;
}

//	Emit bones
bool CExportor::EmitBones()
{
	//	Get bone candidate vector
	APtrArray<CBoneCandidate*>& aBones = m_pSkeletonCandidate->GetBoneArray();

	//	Start the progress info
	g_pMAXInterface->StartProgressInfo("Emit bones ...");

	int iBone, iSelectedId;
	APtrArray<A3DBone*> aA3DBones(aBones.GetSize(), 10);

	for (iBone=0, iSelectedId=0; iBone < aBones.GetSize(); iBone++)
	{
		//	Get the bone candidate
		CBoneCandidate* pBoneCandidate = aBones[iBone];

		//	Only export selected bone candidates
		if (!pBoneCandidate->IsSelected())
			continue;
		
		//	Update the progress info
		iSelectedId++;
		g_pMAXInterface->SetProgressInfo((100.0f * iSelectedId) / m_iNumSelectedBone);

		//	Allocate new core bone instance
		A3DBone* pA3DBone = new A3DBone();
		if (!pA3DBone)
			return false;

		pA3DBone->Init(m_pSkeleton);

		//	Create the core bone instance
		pA3DBone->SetName(pBoneCandidate->GetName());
		pA3DBone->SetFakeFlag(pBoneCandidate->IsFakeBone());

		//	Get the parent id of the bone candidate
		int iParent = m_pSkeletonCandidate->GetParentSelectedId(true, iBone);

		//	Set the parentId
		pA3DBone->SetParent(iParent < 0 ? -1 : iParent);

		//	Get the transform matrix relative to parent node
		A3DMATRIX4 mat;
		m_pSkeletonCandidate->GetTranslationAndRotation(true, iBone, -1.0f, mat);
		if (DotProduct(CrossProduct(mat.GetRow(0), mat.GetRow(1)), mat.GetRow(2)) < 0.0f)
		{
			pBoneCandidate->SetFlipFlag(true);
			pA3DBone->SetFlippedFlag(true);
		}
		else
		{
			pBoneCandidate->SetFlipFlag(false);
			pA3DBone->SetFlippedFlag(false);
		}

		pA3DBone->SetOriginalMatrix(mat);

		//	Get the transform matrix from model space to bone space
	//	pSkeletonCandidate->GetTranslationAndRotationBoneSpace(iBone, -1.0f, mat);
		g_pMAXInterface->MatrixFromMAXToA3D(pBoneCandidate->GetInitTM(), mat);
		pA3DBone->SetBoneInitTM(mat);

		//	Set the skeleton of the core bone instance
		pA3DBone->SetSkeleton(m_pSkeleton);

		//	Add the bone to array
		int iIndex = aA3DBones.Add(pA3DBone);
		pBoneCandidate->SetSkeletonId(iIndex);
	}

	//	Link parents and children
	for (iBone=0; iBone < aA3DBones.GetSize(); iBone++)
	{
		int iParent = aA3DBones[iBone]->GetParent();

		//	Adjust child list of parent bone
		if (iParent >= 0)
		{
			//	Get parent core bone
			A3DBone* pParentBone = aA3DBones[iParent];
			if (!pParentBone)
				return false;

			//	Add this core bone to the child list of the parent bone
			pParentBone->AddChild(iBone);
		}
	}

	//	Add all bones to skeleton
	for (iBone=0; iBone < aA3DBones.GetSize(); iBone++)
		m_pSkeleton->AddBone(aA3DBones[iBone]);

	//	Stop the progress info
	g_pMAXInterface->StopProgressInfo();
	
	return true;
}

//	Emit a hook
bool CExportor::EmitHooks()
{
	for (int i=0; i < m_pSkeletonCandidate->GetHookNum(); i++)
	{
		CBoneCandidate* pBoneCandidate = m_pSkeletonCandidate->GetHook(i);
		if (!pBoneCandidate->IsSelected())
			continue;

		const char* szName = pBoneCandidate->GetName();

		A3DSkeletonHook* pHook = new A3DSkeletonHook(m_pSkeleton);
		if (!pHook)
		{
			a_SendLogToDebugWnd(1, "CExportor::EmitHook, Not enough memory");
			return false;
		}

		pHook->SetName(pBoneCandidate->GetName());

	/*	if (szName[0] == 'H' && szName[1] == 'H' && szName[2] == '_')
		{
			int iParent = pSkeletonCandidate->GetParentSelectedId(false, i);
			if (iParent < 0)
			{
				delete pHook;
				a_SendLogToDebugWnd(1, "CExportor::EmitHooks, hook [%s] is unlinked", pBoneCandidate->GetName());
				continue;
			}

			pHook->SetHookType(A3DSkeletonHook::HOOK_HH);

			//	Set the attached bone index
			pHook->SetBone(iParent);

			//	Get the transform matrix relative to parent node
			A3DMATRIX4 mat;
			pSkeletonCandidate->GetTranslationAndRotation(false, i, -1.0f, mat);
			pHook->SetHookTM(mat);
		}
		else if (szName[0] == 'C' && szName[1] == 'C' && szName[2] == '_')
		{
			pHook->SetHookType(A3DSkeletonHook::HOOK_CC);
			pHook->SetBone(-1);

			A3DMATRIX4 matA3D;
			g_pMAXInterface->GetTranslationAndRotationBoneSpace(pBoneCandidate->GetNode(), -1.0f, matA3D);
			pHook->SetHookTM(matA3D);
		}
	*/
		if (szName[0] == 'H' && szName[1] == 'H' && szName[2] == '_')
			pHook->SetHookType(A3DSkeletonHook::HOOK_HH);
		else if (szName[0] == 'C' && szName[1] == 'C' && szName[2] == '_')
			pHook->SetHookType(A3DSkeletonHook::HOOK_CC);
		else
			continue;

		int iParent = m_pSkeletonCandidate->GetParentSelectedId(false, i);
	/*	if (iParent < 0)
		{
			delete pHook;
			a_SendLogToDebugWnd(1, "CExportor::EmitHooks, hook [%s] is unlinked", pBoneCandidate->GetName());
			continue;
		}
	*/
		//	Set the attached bone index
		pHook->SetBone(iParent);

		//	Get the transform matrix relative to parent node
		A3DMATRIX4 mat;
		m_pSkeletonCandidate->GetTranslationAndRotation(false, i, -1.0f, mat);
		pHook->SetHookTM(mat);

		m_pSkeleton->AddHook(pHook);
	}

	return true;
}

//	Emit animations
bool CExportor::EmitAnimations()
{
	//	Get bone candidate vector
	APtrArray<CBoneCandidate*>& aBones = m_pSkeletonCandidate->GetBoneArray();

	//	Get frame information
	int iInStartFrame = m_pSkeletonCandidate->GetStartFrame();
	int iInEndFrame = m_pSkeletonCandidate->GetEndFrame();
	int iNumFrame = iInEndFrame - iInStartFrame + 1;
	int iOutStartFrame = 0;
	int iOutEndFrame = iNumFrame - 1;
	int iFPS = m_pSkeletonCandidate->GetAnimFPS();

	int iBone, iSelectedId, iBaseIdx = m_pSkeleton->GetJointNum();
	AArray<JOINTITEM, JOINTITEM&> aJoints(aBones.GetSize(), 10);

	//	Create joint for selected bones
	for (iBone=0, iSelectedId=0; iBone < aBones.GetSize(); iBone++)
	{
		//	Get the bone candidate
		CBoneCandidate* pBoneCandidate = aBones[iBone];

		//	Cnly create joints for the selected bone candidates
		if (!pBoneCandidate->IsSelected() || pBoneCandidate->IsHook())
			continue;

		//	Allocate new core track instance
		A3DAnimJoint* pJoint = new A3DAnimJoint;
		if (!pJoint)
			return false;

		pJoint->Init(m_pSkeleton);

		//	Add a suffix to joint name
		AString strName = pBoneCandidate->GetName();
		pJoint->SetName(strName + "_AnimJoint");

		//	Create the tracks
		A3DVector3Track* pPosTrack = _CreatePosTrack(iNumFrame, iFPS, 1);
		A3DQuaternionTrack*	pRotTrack = _CreateRotTrack(iNumFrame, iFPS, 1);
		if (!pPosTrack || !pRotTrack)
		{
			AfxMessageBox("Failed to create joint tracks.", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		//	Link joint and bone
		A3DBone* pBone = m_pSkeleton->GetBone(pBoneCandidate->GetSkeletonId());
		pJoint->SetBones(pBone->GetParent(), pBoneCandidate->GetSkeletonId());
		pBone->AddJoint(iBaseIdx + iSelectedId);

		iSelectedId++;

		JOINTITEM JointItem;
		JointItem.pJoint	= pJoint;
		JointItem.pPosTrack	= pPosTrack;
		JointItem.pRotTrack	= pRotTrack;

		aJoints.Add(JointItem);
	}

	//	Start the progress info
	g_pMAXInterface->StartProgressInfo("Emit animations ...");

	//	Record bones' animation tracks
	for (iBone=0, iSelectedId=0; iBone < aBones.GetSize(); iBone++)
	{
		CBoneCandidate* pBoneCandidate = aBones[iBone];

		//	Cnly create joints for the selected bone candidates
		if (!pBoneCandidate->IsSelected() || pBoneCandidate->IsHook())
			continue;

		g_pMAXInterface->SetProgressInfo((100.0f * iSelectedId) / m_iNumSelectedBone);

		JOINTITEM& JointItem = aJoints[iSelectedId++];
		A3DVECTOR3* pPosTrack = JointItem.pPosTrack->GetKeyFrames();
		A3DQUATERNION* pRotTrack = JointItem.pRotTrack->GetKeyFrames();

		for (int i=0; i < iNumFrame; i++)
		{
			float fTime = (iInStartFrame + i) / (float)iFPS;

			A3DMATRIX4 mat;
			m_pSkeletonCandidate->GetTranslationAndRotation(true, iBone, fTime, mat);
			if (pBoneCandidate->GetFlipFlag())
			{
				mat._11 = -mat._11;
				mat._12 = -mat._12;
				mat._13 = -mat._13;
			}

			A3DQUATERNION Quat(mat);
			pPosTrack[i] = mat.GetRow(3);
			pRotTrack[i] = Quat;
		}
	}

	//	Stop the progress info
	g_pMAXInterface->StopProgressInfo();

	int i, j;

	//	Compress animation tracks and add them to skeleton
	for (i=0; i < aJoints.GetSize(); i++)
	{
		JOINTITEM& JointItem = aJoints[i];

		//	Clear JointItem.pJoint after it is added to skeleton, 
		//	so that it won't be deleted by JOINTITEM's destructor
		int iJointIdx = m_pSkeleton->AddJoint(JointItem.pJoint);
		JointItem.pJoint = NULL;

		//	Create a bone track
		CTrackSetWrapper::BONETRACK* pBoneTrack = m_pTrackSet->CreateBoneTrack(iJointIdx);
		if (!pBoneTrack)
		{
			AfxMessageBox("Failed to create bone track.", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		int t1, t2, f1, f2, iFrame;
	
		//	Handle position track ..............................
		A3DVECTOR3* pPosTrack = JointItem.pPosTrack->GetKeyFrames();

		bool bAllSame = false;

		//	Search for the first animation frame
		for (j=1; j < iNumFrame; j++)
		{
			if (!_PosEqual(pPosTrack[0], pPosTrack[j]))
				break;
		}

		if (j >= iNumFrame)
			bAllSame = true;
		else
		{
			f1 = j - 1;
			t1 = _FrameToTime(iOutStartFrame + f1, iFPS);

			//	Search for the last animation frame
			for (j=iNumFrame-2; j >= f1; j--)
			{
				if (!_PosEqual(pPosTrack[iNumFrame-1], pPosTrack[j]))
					break;
			}

			if (j < f1)
				bAllSame = true;
			else
			{
				f2 = j + 1;
				t2 = _FrameToTime(iOutStartFrame + f2, iFPS);
				iFrame = f2 - f1 + 1;
			}
		}

		if (bAllSame)
		{
			//	All frames are same
			t1 = _FrameToTime(iOutStartFrame, iFPS);
			t2 = _FrameToTime(iOutEndFrame, iFPS);
			f1 = 0;
			f2 = 0;
			iFrame = 1;
		}

		//	Create new track to stored compressed data
		A3DVector3Track* pNewPosTrack = _CreatePosTrack(iFrame, iFPS, 1);
		if (!pNewPosTrack)
		{
			AfxMessageBox("Failed to create destination joint position track.", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		//	Copy track data
		A3DVECTOR3* pPosTrack2 = pNewPosTrack->GetKeyFrames();
		for (j=0; j < iFrame; j++)
			pPosTrack2[j] = pPosTrack[f1+j];

		A3DVector3Track::SEGMENT* pPosSegment = pNewPosTrack->GetSegments();
		pPosSegment->iStartTime	= t1;
		pPosSegment->iEndTime	= t2;
		pPosSegment->iStartKey	= 0;
		pPosSegment->iEndKey	= iFrame - 1;

		//	Handle rotation track ..............................
		A3DQUATERNION* pRotTrack = JointItem.pRotTrack->GetKeyFrames();

		bAllSame = false;

		//	Search for the first animation frame
		for (j=1; j < iNumFrame; j++)
		{
			if (!_OrientationEqual(pRotTrack[0], pRotTrack[j]))
				break;
		}

		if (j >= iNumFrame)
			bAllSame = true;
		else
		{
			f1 = j - 1;
			t1 = _FrameToTime(iOutStartFrame + f1, iFPS);

			//	Search for the last animation frame
			for (j=iNumFrame-2; j >= f1; j--)
			{
				if (!_OrientationEqual(pRotTrack[iNumFrame-1], pRotTrack[j]))
					break;
			}

			if (j < f1)
				bAllSame = true;
			else
			{
				f2 = j + 1;
				t2 = _FrameToTime(iOutStartFrame + f2, iFPS);
				iFrame = f2 - f1 + 1;
			}
		}

		if (bAllSame)
		{
			//	All frames are same
			t1 = _FrameToTime(iOutStartFrame, iFPS);
			t2 = _FrameToTime(iOutEndFrame, iFPS);
			f1 = 0;
			f2 = 0;
			iFrame = 1;
		}

		//	Create new track to stored compressed data
		A3DQuaternionTrack* pNewRotTrack = _CreateRotTrack(iFrame, iFPS, 1);
		if (!pNewRotTrack)
		{
			AfxMessageBox("Failed to create destination joint rotation track.", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		//	Copy track data
		A3DQUATERNION* pRotTrack2 = pNewRotTrack->GetKeyFrames();
		for (j=0; j < iFrame; j++)
			pRotTrack2[j] = pRotTrack[f1+j];

		A3DQuaternionTrack::SEGMENT* pRotSegment = pNewRotTrack->GetSegments();
		pRotSegment->iStartTime	= t1;
		pRotSegment->iEndTime	= t2;
		pRotSegment->iStartKey	= 0;
		pRotSegment->iEndKey	= iFrame - 1;

		//	Record new tracks
		pBoneTrack->pPosTrack = pNewPosTrack;
		pBoneTrack->pRotTrack = pNewRotTrack;
	}

	//	Release temporary joints
	for (i=0; i < aJoints.GetSize(); i++)
	{
		JOINTITEM& JointItem = aJoints[i];
		if (JointItem.pJoint) delete JointItem.pJoint;
		if (JointItem.pPosTrack) delete JointItem.pPosTrack;
		if (JointItem.pRotTrack) delete JointItem.pRotTrack;
	}

	return true;
}

//	Emit textures and materials
bool CExportor::EmitTexturesAndMaterials(const char* szTexPath)
{
	int i;
	AString strTexFile, strPath;
	char szFileTitle[MAX_PATH];

	APtrArray<A3DTexture*>& aTextures = m_pSkin->GetTextureArray();
	APtrArray<A3DMaterial*>& aMaterials = m_pSkin->GetMaterialArray();

	strPath = szTexPath;

	//	Emit textures
	if (m_pSkinCandidate->GetTextureNum())
	{
		//	Create texture path
		AUX_CreateDirectory(szTexPath);

		for (i=0; i < m_pSkinCandidate->GetTextureNum(); i++)
		{
			A3DTexture* pTexture = new A3DTexture;
			if (!pTexture)
				return false;

			//	Get texture file title
			strTexFile = m_pSkinCandidate->GetTextureName(i);
			af_GetFileTitle(strTexFile, szFileTitle, MAX_PATH);

			pTexture->SetMapFile(szFileTitle);
			aTextures.Add(pTexture);

			//	Copy file
			strTexFile = strPath + szFileTitle;
			CopyFile(m_pSkinCandidate->GetTextureName(i), strTexFile, FALSE);
		}
	}

	//	Emit materials
	for (i=0; i < m_pSkinCandidate->GetMaterialNum(); i++)
	{
		A3DMaterial* pMtl = m_pSkinCandidate->GetMaterial(i);
		aMaterials.Add(pMtl);
	}

	return true;
}

//	Emit meshes
bool CExportor::EmitMeshes()
{
	//	Emit meshes
	for (int i=0; i < m_pSkinCandidate->GetMeshNum(); i++)
	{
		CMeshCandidate* pMeshCandidate = (CMeshCandidate*)m_pSkinCandidate->GetMesh(i);

		bool bRet = true;

		if (pMeshCandidate->IsMorphMesh())
		{
			if (pMeshCandidate->IsRigidMesh())
				bRet = EmitMorphRigidMesh(pMeshCandidate);
			else	//	Morph skin mesh
				bRet = EmitMorphSkinMesh(pMeshCandidate);
		}
		else if (pMeshCandidate->IsMuscleMesh())
		{
			if (pMeshCandidate->IsRigidMesh())
				bRet = EmitMuscleRigidMesh(pMeshCandidate);
			else	//	Morph skin mesh
				bRet = EmitMuscleSkinMesh(pMeshCandidate);
		}
		else if (pMeshCandidate->IsSuppleMesh())
		{
			if (pMeshCandidate->IsRigidMesh())
				bRet = EmitSuppleRigidMesh(pMeshCandidate);
			else	//	Supple skin mesh
				bRet = EmitSuppleSkinMesh(pMeshCandidate);
		}
		else
		{
			if (pMeshCandidate->IsRigidMesh())
				bRet = EmitRigidMesh(pMeshCandidate);
			else
				bRet = EmitSkinMesh(pMeshCandidate);
		}

		if (!bRet)
			return false;
	}

	return true;
}

//	Fill skin mesh data
bool CExportor::FillSkinMeshData(A3DSkinMesh* pMesh, CMeshCandidate* pMeshCandidate)
{
	pMesh->SetName(pMeshCandidate->GetName());
	pMesh->SetTextureIndex(pMeshCandidate->GetTextureIndex());
	pMesh->SetMaterialIndex(pMeshCandidate->GetMaterialIndex());

	int iNumVert = pMeshCandidate->GetVertexNum();
	int iNumIdx	 = pMeshCandidate->GetFaceNum() * 3;

	if (!pMesh->CreateOriginalBuffers(iNumVert, iNumIdx))
	{
		a_SendLogToDebugWnd(1, "CExportor::FillSkinMeshData, Failed to create buffers");
		return false;
	}

	int j;
	SKIN_VERTEX* aVerts = pMesh->GetOriginVertexBuf();

	for (j=0; j < iNumVert; j++)
	{
		SKIN_VERTEX* pVert = &aVerts[j];
		CVertexCandidate* pVertCandidate = pMeshCandidate->GetVertexCandidate(j);

		A3DVECTOR3 v = pVertCandidate->GetPos();
		pVert->vPos[0] = v.x;
		pVert->vPos[1] = v.y;
		pVert->vPos[2] = v.z;

		pVert->vNormal[0] = pVertCandidate->m_vSmoothNormal.x;
		pVert->vNormal[1] = pVertCandidate->m_vSmoothNormal.y;
		pVert->vNormal[2] = pVertCandidate->m_vSmoothNormal.z;

		pVertCandidate->GetNormalTextureCoord(&pVert->tu, &pVert->tv);
		
		int iNumWeight = pVertCandidate->GetInfluenceNum();
		if (iNumWeight > 4)
			iNumWeight = 4;

		pVert->aWeights[0] = pVert->aWeights[1] = pVert->aWeights[2] = 0.0f;

		if (!iNumWeight)	//	No bone effect this vertex
		{
			//	Bind vertex to model itself
			pVert->dwMatIndices = (DWORD)m_pSkin->GetSkinBoneNum();
			pVert->aWeights[0]	= 1.0f;
		}
		else
		{
			pVert->dwMatIndices = 0;

			for (int n=0; n < iNumWeight; n++)
			{
				CVertexCandidate::INFLUENCE Influence = pVertCandidate->GetInfluence(n);

				if (n < 3)
					pVert->aWeights[n] = Influence.fWeight;
				
				//	Note: here we remap bone's index !
				CBoneCandidate* pBone = m_pSkeletonCandidate->GetBoneCandidateBySelectedID(Influence.iBone);
				int iBone = SearchBoneNameInSkin(m_pSkin, pBone->GetName(), false);
				if (iBone >= 0)
					pVert->dwMatIndices |= iBone << (n * 8);
				else
					return false;	//	Shouldn't happen
			}
		}
	}

	WORD* aIndices = pMesh->GetOriginIndexBuf();
	int iCount = 0;
	
	for (j=0; j < pMeshCandidate->GetFaceNum(); j++)
	{
		const CMeshCandidate::FACE& Face = pMeshCandidate->GetFace(j);
		aIndices[iCount++] = Face.aVertIdx[0];
		aIndices[iCount++] = Face.aVertIdx[1];
		aIndices[iCount++] = Face.aVertIdx[2];
	}

	return true;
}

//	Fill rigid mesh data
bool CExportor::FillRigidMeshData(A3DRigidMesh* pMesh, CMeshCandidate* pMeshCandidate)
{
	pMesh->SetName(pMeshCandidate->GetName());
	pMesh->SetTextureIndex(pMeshCandidate->GetTextureIndex());
	pMesh->SetMaterialIndex(pMeshCandidate->GetMaterialIndex());
	pMesh->SetBoneIndex(pMeshCandidate->GetBoneIndex());

	int iNumVert = pMeshCandidate->GetVertexNum();
	int iNumIdx	 = pMeshCandidate->GetFaceNum() * 3;

	if (!pMesh->CreateOriginalBuffers(iNumVert, iNumIdx))
	{
		a_SendLogToDebugWnd(1, "CExportor::FillRigidMeshData, Failed to create buffers");
		return false;
	}

	int j;
	A3DVERTEX* aVerts = pMesh->GetOriginVertexBuf();

	for (j=0; j < iNumVert; j++)
	{
		A3DVERTEX* pVert = &aVerts[j];
		CVertexCandidate* pVertCandidate = pMeshCandidate->GetVertexCandidate(j);

		A3DVECTOR3 v = pVertCandidate->GetPos();
		pVert->x = v.x;
		pVert->y = v.y;
		pVert->z = v.z;

		pVert->nx = pVertCandidate->m_vSmoothNormal.x;
		pVert->ny = pVertCandidate->m_vSmoothNormal.y;
		pVert->nz = pVertCandidate->m_vSmoothNormal.z;

		pVertCandidate->GetNormalTextureCoord(&pVert->tu, &pVert->tv);
	}		

	WORD* aIndices = pMesh->GetOriginIndexBuf();
	int iCount = 0;
	
	for (j=0; j < pMeshCandidate->GetFaceNum(); j++)
	{
		const CMeshCandidate::FACE& Face = pMeshCandidate->GetFace(j);
		aIndices[iCount++] = Face.aVertIdx[0];
		aIndices[iCount++] = Face.aVertIdx[1];
		aIndices[iCount++] = Face.aVertIdx[2];
	}

	return true;
}

//	Fill morph mesh data
bool CExportor::FillMorphMeshData(A3DMorphMesh* pMesh, CMeshCandidate* pMeshCandidate)
{
	int iStartFrame	= pMeshCandidate->GetAnimStartFrame();
	int iNumFrame	= pMeshCandidate->GetAnimFrameNum();
	int iEndFrame	= iStartFrame + iNumFrame - 1;
	int iFPS		= pMeshCandidate->GetAnimFPS();

	pMesh->SetAnimFrameRange(iStartFrame, iEndFrame);
	pMesh->SetAnimFPS(iFPS);

	int i, j, iNumVert = pMeshCandidate->GetVertexNum();
	int iNumChannel = pMeshCandidate->GetMorphChannelNum();

	//	Handle channel data
	for (i=0; i < iNumChannel; i++)
	{
		A3DMorphMesh::CHANNEL* pDest = new A3DMorphMesh::CHANNEL;
		if (!pDest)
			return false;

		CMorphChannel* pSrc = pMeshCandidate->GetMorphChannel(i);

		//	Channel name
		pDest->strName		= pSrc->GetName();
		pDest->pStream		= NULL;
		pDest->aOriginVerts	= NULL;
		pDest->pWeightTrack	= NULL;

		//	Vertex data
		if (!(pDest->aOriginVerts = new A3DMCVERTEX[iNumVert]))
		{
			a_SendLogToDebugWnd(1, "CExportor::FillMorphMeshData, Not enough memory");
			return false;
		}

		for (j=0; j < iNumVert; j++)
		{
			const A3DVECTOR3& v = pSrc->GetPosDelta(j);
			pDest->aOriginVerts[j].vPos[0] = v.x;
			pDest->aOriginVerts[j].vPos[1] = v.y;
			pDest->aOriginVerts[j].vPos[2] = v.z;

			const A3DVECTOR3& n = pSrc->GetNormalDelta(j);
			pDest->aOriginVerts[j].vNormal[0] = n.x;
			pDest->aOriginVerts[j].vNormal[1] = n.y;
			pDest->aOriginVerts[j].vNormal[2] = n.z;
		}

		//	Weight track
		A3DFloatTrack* pTrack = pMesh->CreateWeightTrack(pSrc->GetName(), iNumFrame, iFPS, 1, m_pSkin);
		if (!pTrack)
		{
			a_SendLogToDebugWnd(1, "CExportor::FillMorphMeshData, Failed to craete weight track");
			return false;
		}
		
		//	Copy weight data
		float* aWeights = pTrack->GetKeyFrames();
		for (j=0; j < iNumFrame; j++)
			aWeights[j] = pSrc->GetWeight(j);

		A3DFloatTrack::SEGMENT* pWeightSegment = pTrack->GetSegments();
		pWeightSegment->iStartTime	= (int)(iStartFrame * 1000.0f / iFPS);
		pWeightSegment->iEndTime	= (int)(iEndFrame * 1000.0f / iFPS);
		pWeightSegment->iStartKey	= 0;
		pWeightSegment->iEndKey		= iNumFrame - 1;

		pDest->pWeightTrack = pTrack;

		//	Add channel to mesh
		pMesh->AddChannel(pDest);
	}

	BYTE* aSort = new BYTE[iNumFrame * iNumChannel];
	if (!aSort)
	{
		a_SendLogToDebugWnd(1, "CExportor::FillMorphMeshData, Not enough memory");
		return false;
	}

	int iCount = 0;

	//	Handle channel sort data
	for (i=0; i < iNumFrame; i++)
	{
		BYTE* pSrc = pMeshCandidate->GetMorphChannelSort(i);

		for (j=0; j < iNumChannel; j++)
			aSort[iCount++] = pSrc[j];
	}

	pMesh->SetChannelSorts(aSort);

	return true;
}

//	Emit a skin mesh
bool CExportor::EmitSkinMesh(CMeshCandidate* pMeshCandidate)
{
	A3DSkinMesh* pMesh = new A3DSkinMesh;
	if (!pMesh)
		return false;

	if (!pMesh->Init(m_pSkin->GetA3DEngine()))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitSkinMesh, Failed to initialize A3DSkinMesh");
		return false;
	}

	//	Fill skin mesh data
	if (!FillSkinMeshData(pMesh, pMeshCandidate))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitSkinMesh, Failed fill mesh data");
		return false;
	}

	A3DSkinMeshRef ref = {pMesh, true, -1.0f, NULL};
	m_pSkin->GetSkinMeshArray().Add(ref);

	return true;
}

//	Emit a rigid mesh
bool CExportor::EmitRigidMesh(CMeshCandidate* pMeshCandidate)
{
	//	The mesh didn't select bound bone
	if (pMeshCandidate->GetBoneIndex() < 0)
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitRigidMesh, rigid mesh miss bone");
		return true;
	}

	A3DRigidMesh* pMesh = new A3DRigidMesh;
	if (!pMesh)
		return false;

	if (!pMesh->Init(m_pSkin->GetA3DEngine()))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitRigidMesh, Failed to initialize A3DSkinMesh");
		return false;
	}

	//	Fill rigid mesh data
	if (!FillRigidMeshData(pMesh, pMeshCandidate))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitRigidMesh, Failed to fill mesh data");
		return false;
	}

	A3DSkinMeshRef ref = {pMesh, true, -1.0f, NULL};
	m_pSkin->GetRigidMeshArray().Add(ref);

	return true;
}

//	Emit a morph skin mesh
bool CExportor::EmitMorphSkinMesh(CMeshCandidate* pMeshCandidate)
{
	A3DMorphSkinMesh* pMesh = new A3DMorphSkinMesh;
	if (!pMesh)
		return false;

	if (!pMesh->Init(m_pSkin->GetA3DEngine()))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMorphSkinMesh, Failed to initialize A3DSkinMesh");
		return false;
	}

	//	Fill skin mesh data
	if (!FillSkinMeshData(pMesh, pMeshCandidate))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMorphSkinMesh, Failed fill mesh data");
		return false;
	}

	//	Fill morph data
	if (!FillMorphMeshData(&pMesh->m_MorphData, pMeshCandidate))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMorphSkinMesh, Failed fill morph data");
		return false;
	}

	A3DSkinMeshRef ref = {pMesh, true, -1.0f, NULL};
	m_pSkin->GetMorphSkinMeshArray().Add(ref);

	return true;
}

//	Emit a morph rigid mesh
bool CExportor::EmitMorphRigidMesh(CMeshCandidate* pMeshCandidate)
{
	//	The bone this mesh bound to isn't selected
	if (pMeshCandidate->GetBoneIndex() < 0)
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMorphRigidMesh, morph rigid mesh miss bone");
		return true;
	}

	A3DMorphRigidMesh* pMesh = new A3DMorphRigidMesh;
	if (!pMesh)
		return false;

	if (!pMesh->Init(m_pSkin->GetA3DEngine()))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMorphRigidMesh, Failed to initialize A3DSkinMesh");
		return false;
	}

	//	Fill rigid mesh data
	if (!FillRigidMeshData(pMesh, pMeshCandidate))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMorphRigidMesh, Failed to fill mesh data");
		return false;
	}

	//	Fill morph data
	if (!FillMorphMeshData(&pMesh->m_MorphData, pMeshCandidate))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMorphRigidMesh, Failed fill morph data");
		return false;
	}

	A3DSkinMeshRef ref = {pMesh, true, -1.0f, NULL};
	m_pSkin->GetMorphRigidMeshArray().Add(ref);

	return true;
}

//	Emit a supple rigid mesh
bool CExportor::EmitSuppleRigidMesh(CMeshCandidate* pMeshCandidate)
{
	A3DSuppleMesh* pMesh = new A3DSuppleMesh;
	if (!pMesh)
		return false;

	if (!pMesh->Init(m_pSkin->GetA3DEngine()))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitSuppleRigidMesh, Failed to initialize A3DSuppleMesh");
		return false;
	}

	pMesh->SetName(pMeshCandidate->GetName());
	pMesh->SetTextureIndex(pMeshCandidate->GetTextureIndex());
	pMesh->SetMaterialIndex(pMeshCandidate->GetMaterialIndex());
	pMesh->SetGravityFactor(pMeshCandidate->GetGravityFactor());
	pMesh->SetAirResistance(pMeshCandidate->GetAirResistance());
	pMesh->SetVertWeight(pMeshCandidate->GetVertWeight());

	int iNumRVert	= pMeshCandidate->GetVertexNum();
	int iNumSVert	= pMeshCandidate->GetSpringVertexNum();
	int iNumIdx		= pMeshCandidate->GetFaceNum() * 3;
	int iNumSpring	= pMeshCandidate->GetSpringNum();

	if (!pMesh->CreateOriginalBuffers(iNumSVert, iNumRVert, iNumIdx, iNumSpring))
	{
		a_SendLogToDebugWnd(1, "CExportor::FillRigidMeshData, Failed to create buffers");
		return false;
	}

	int j;
	
	//	Fill spring vertices
	A3DSuppleMesh::SVERTEX* aSVerts = pMesh->GetOriginSpringVertexBuf();
	for (j=0; j < iNumSVert; j++)
	{
		A3DSuppleMesh::SVERTEX* pVert = &aSVerts[j];
		CSpringVertex* pSpringVertex = pMeshCandidate->GetSpringVertex(j);

		pVert->vPos = pSpringVertex->GetPos();

		pVert->aWeights[0]	= 1.0f;
		pVert->aWeights[1]	= 0.0f;
		pVert->aWeights[2]	= 0.0f;
		pVert->dwMatIndices = (DWORD)pMeshCandidate->GetBoneIndex();
		pVert->fBoneFactor	= pSpringVertex->GetPhysicalProperty().fBoneFactor;
	}

	//	Fill render vertices
	A3DSuppleMesh::RVERTEX* aRVerts = pMesh->GetOriginRenderVertexBuf();
	for (j=0; j < iNumRVert; j++)
	{
		A3DSuppleMesh::RVERTEX* pVert = &aRVerts[j];
		CVertexCandidate* pVertCandidate = pMeshCandidate->GetVertexCandidate(j);

		pVert->vNormal	= pVertCandidate->m_vSmoothNormal;
		pVert->iSVertex	= pVertCandidate->GetVertexID();

		pVertCandidate->GetNormalTextureCoord(&pVert->tu, &pVert->tv);
	}		

	//	Fill indices
	WORD* aIndices = pMesh->GetOriginIndexBuf();
	int iCount = 0;
	
	for (j=0; j < pMeshCandidate->GetFaceNum(); j++)
	{
		const CMeshCandidate::FACE& Face = pMeshCandidate->GetFace(j);
		aIndices[iCount++] = Face.aVertIdx[0];
		aIndices[iCount++] = Face.aVertIdx[1];
		aIndices[iCount++] = Face.aVertIdx[2];
	}

	//	Fill springs
	A3DSuppleMesh::SPRING* aSprings = pMesh->GetSpringBuf();

	for (j=0; j < iNumSpring; j++)
	{
		A3DSuppleMesh::SPRING* pDest = &aSprings[j];
		const CMeshCandidate::SPRING& Src = pMeshCandidate->GetSpring(j);

		pDest->v1 = Src.aVertIdx[0];
		pDest->v2 = Src.aVertIdx[1];
		pDest->fCoefficient	= Src.fCoefficient;
		pDest->fIdleLen		= Src.iIdleLen;
	}

	A3DSkinMeshRef ref = {pMesh, true, -1.0f, NULL};
	m_pSkin->GetSuppleMeshArray().Add(ref);

	return true;
}

//	Emit a supple skin mesh
bool CExportor::EmitSuppleSkinMesh(CMeshCandidate* pMeshCandidate)
{
	A3DSuppleMesh* pMesh = new A3DSuppleMesh;
	if (!pMesh)
		return false;

	if (!pMesh->Init(m_pSkin->GetA3DEngine()))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitSuppleRigidMesh, Failed to initialize A3DSuppleMesh");
		return false;
	}

	pMesh->SetName(pMeshCandidate->GetName());
	pMesh->SetTextureIndex(pMeshCandidate->GetTextureIndex());
	pMesh->SetMaterialIndex(pMeshCandidate->GetMaterialIndex());
	pMesh->SetGravityFactor(pMeshCandidate->GetGravityFactor());
	pMesh->SetAirResistance(pMeshCandidate->GetAirResistance());
	pMesh->SetVertWeight(pMeshCandidate->GetVertWeight());

	int iNumRVert	= pMeshCandidate->GetVertexNum();
	int iNumSVert	= pMeshCandidate->GetSpringVertexNum();
	int iNumIdx		= pMeshCandidate->GetFaceNum() * 3;
	int iNumSpring	= pMeshCandidate->GetSpringNum();

	if (!pMesh->CreateOriginalBuffers(iNumSVert, iNumRVert, iNumIdx, iNumSpring))
	{
		a_SendLogToDebugWnd(1, "CExportor::FillRigidMeshData, Failed to create buffers");
		return false;
	}

	int j;

	//	Fill spring vertices
	A3DSuppleMesh::SVERTEX* aSVerts = pMesh->GetOriginSpringVertexBuf();
	for (j=0; j < iNumSVert; j++)
	{
		A3DSuppleMesh::SVERTEX* pVert = &aSVerts[j];
		CSpringVertex* pSpringVertex = pMeshCandidate->GetSpringVertex(j);

		pVert->vPos = pSpringVertex->GetPos();

		int iNumWeight = pSpringVertex->GetInfluenceNum();
		if (iNumWeight > 4)
			iNumWeight = 4;

		pVert->aWeights[0] = 0.0f;
		pVert->aWeights[1] = 0.0f;
		pVert->aWeights[2] = 0.0f;

		if (!iNumWeight)	//	No bone effect this vertex
		{
			//	Bind vertex to model itself
			pVert->dwMatIndices	= (DWORD)m_pSkin->GetSkeletonBoneNum();
			pVert->aWeights[0]	= 1.0f;
		}
		else
		{
			pVert->dwMatIndices = 0;

			for (int n=0; n < iNumWeight; n++)
			{
				CVertexCandidate::INFLUENCE Influence = pSpringVertex->GetInfluence(n);

				if (n < 3)
					pVert->aWeights[n] = Influence.fWeight;
			
				pVert->dwMatIndices |= Influence.iBone << (n * 8);
			}
		}

		pVert->fBoneFactor = pSpringVertex->GetPhysicalProperty().fBoneFactor;
	}

	//	Fill render vertices
	A3DSuppleMesh::RVERTEX* aVerts = pMesh->GetOriginRenderVertexBuf();
	for (j=0; j < iNumRVert; j++)
	{
		A3DSuppleMesh::RVERTEX* pVert = &aVerts[j];
		CVertexCandidate* pVertCandidate = pMeshCandidate->GetVertexCandidate(j);

		pVert->vNormal	= pVertCandidate->m_vSmoothNormal;
		pVert->iSVertex	= pVertCandidate->GetVertexID();

		pVertCandidate->GetNormalTextureCoord(&pVert->tu, &pVert->tv);
	}		

	//	Fill indices
	WORD* aIndices = pMesh->GetOriginIndexBuf();
	int iCount = 0;
	
	for (j=0; j < pMeshCandidate->GetFaceNum(); j++)
	{
		const CMeshCandidate::FACE& Face = pMeshCandidate->GetFace(j);
		aIndices[iCount++] = Face.aVertIdx[0];
		aIndices[iCount++] = Face.aVertIdx[1];
		aIndices[iCount++] = Face.aVertIdx[2];
	}

	//	Fill springs
	A3DSuppleMesh::SPRING* aSprings = pMesh->GetSpringBuf();

	for (j=0; j < iNumSpring; j++)
	{
		A3DSuppleMesh::SPRING* pDest = &aSprings[j];
		const CMeshCandidate::SPRING& Src = pMeshCandidate->GetSpring(j);

		pDest->v1 = Src.aVertIdx[0];
		pDest->v2 = Src.aVertIdx[1];
		pDest->fCoefficient	= Src.fCoefficient;
		pDest->fIdleLen		= Src.iIdleLen;
	}

	A3DSkinMeshRef ref = {pMesh, true, -1.0f, NULL};
	m_pSkin->GetSuppleMeshArray().Add(ref);

	return true;
}

//	Emit a muscle rigid mesh
bool CExportor::EmitMuscleRigidMesh(CMeshCandidate* pMeshCandidate)
{
	A3DMuscleMesh* pMesh = new A3DMuscleMesh;
	if (!pMesh)
		return false;

	if (!pMesh->Init(m_pSkin->GetA3DEngine()))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMuscleRigidMesh, Failed to initialize A3DMuscleMesh");
		return false;
	}

	pMesh->SetName(pMeshCandidate->GetName());
	pMesh->SetTextureIndex(pMeshCandidate->GetTextureIndex());
	pMesh->SetMaterialIndex(pMeshCandidate->GetMaterialIndex());

	int iNumVert	= pMeshCandidate->GetVertexNum();
	int iNumIdx		= pMeshCandidate->GetFaceNum() * 3;

	if (!pMesh->CreateOriginalBuffers(iNumVert, iNumIdx))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMuscleRigidMesh, Failed to create buffers");
		return false;
	}

	int j;
	
	//	Fill vertices
	A3DBVERTEX3* aVerts = pMesh->GetOriginVertexBuf();
	for (j=0; j < iNumVert; j++)
	{
		A3DBVERTEX3* pVert = &aVerts[j];
		CVertexCandidate* pVertCandidate = pMeshCandidate->GetVertexCandidate(j);

		A3DVECTOR3 v = pVertCandidate->GetPos();
		pVert->vPos[0] = v.x;
		pVert->vPos[1] = v.y;
		pVert->vPos[2] = v.z;

		pVert->vNormal[0]	= pVertCandidate->m_vSmoothNormal.x;
		pVert->vNormal[1]	= pVertCandidate->m_vSmoothNormal.y;
		pVert->vNormal[2]	= pVertCandidate->m_vSmoothNormal.z;

		pVert->dwMatIndices = (DWORD)pMeshCandidate->GetBoneIndex();
		pVert->aWeights[0]	= 1.0f;
		pVert->aWeights[1]	= 0.0f;
		pVert->aWeights[2]	= 0.0f;

		pVertCandidate->GetNormalTextureCoord(&pVert->tu, &pVert->tv);
	}

	//	Fill indices
	WORD* aIndices = pMesh->GetOriginIndexBuf();
	int iCount = 0;
	
	for (j=0; j < pMeshCandidate->GetFaceNum(); j++)
	{
		const CMeshCandidate::FACE& Face = pMeshCandidate->GetFace(j);
		aIndices[iCount++] = Face.aVertIdx[0];
		aIndices[iCount++] = Face.aVertIdx[1];
		aIndices[iCount++] = Face.aVertIdx[2];
	}

	A3DSkinMeshRef ref = {pMesh, true, -1.0f, NULL};
	m_pSkin->GetMuscleMeshArray().Add(ref);

	return true;
}

//	Emit a muscle skin mesh
bool CExportor::EmitMuscleSkinMesh(CMeshCandidate* pMeshCandidate)
{
	A3DMuscleMesh* pMesh = new A3DMuscleMesh;
	if (!pMesh)
		return false;

	if (!pMesh->Init(m_pSkin->GetA3DEngine()))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMuscleSkinMesh, Failed to initialize A3DMuscleMesh");
		return false;
	}

	pMesh->SetName(pMeshCandidate->GetName());
	pMesh->SetTextureIndex(pMeshCandidate->GetTextureIndex());
	pMesh->SetMaterialIndex(pMeshCandidate->GetMaterialIndex());

	int iNumVert	= pMeshCandidate->GetVertexNum();
	int iNumIdx		= pMeshCandidate->GetFaceNum() * 3;

	if (!pMesh->CreateOriginalBuffers(iNumVert, iNumIdx))
	{
		a_SendLogToDebugWnd(1, "CExportor::EmitMuscleSkinMesh, Failed to create buffers");
		return false;
	}

	int j;

	//	Fill vertices
	A3DBVERTEX3* aVerts = pMesh->GetOriginVertexBuf();
	for (j=0; j < iNumVert; j++)
	{
		A3DBVERTEX3* pVert = &aVerts[j];
		CVertexCandidate* pVertCandidate = pMeshCandidate->GetVertexCandidate(j);

		A3DVECTOR3 v = pVertCandidate->GetPos();
		pVert->vPos[0] = v.x;
		pVert->vPos[1] = v.y;
		pVert->vPos[2] = v.z;

		pVert->vNormal[0]	= pVertCandidate->m_vSmoothNormal.x;
		pVert->vNormal[1]	= pVertCandidate->m_vSmoothNormal.y;
		pVert->vNormal[2]	= pVertCandidate->m_vSmoothNormal.z;

		pVertCandidate->GetNormalTextureCoord(&pVert->tu, &pVert->tv);
		
		int iNumWeight = pVertCandidate->GetInfluenceNum();
		if (iNumWeight > 4)
			iNumWeight = 4;

		pVert->aWeights[0] = 0.0f;
		pVert->aWeights[1] = 0.0f;
		pVert->aWeights[2] = 0.0f;

		if (!iNumWeight)	//	No bone effect this vertex
		{
			//	Bind vertex to model itself
			pVert->dwMatIndices	= (DWORD)m_pSkin->GetSkeletonBoneNum();
			pVert->aWeights[0]	= 1.0f;
		}
		else
		{
			pVert->dwMatIndices = 0;

			for (int n=0; n < iNumWeight; n++)
			{
				CVertexCandidate::INFLUENCE Influence = pVertCandidate->GetInfluence(n);

				if (n < 3)
					pVert->aWeights[n] = Influence.fWeight;
				
				pVert->dwMatIndices |= Influence.iBone << (n * 8);
			}
		}
	}

	//	Fill indices
	WORD* aIndices = pMesh->GetOriginIndexBuf();
	int iCount = 0;
	
	for (j=0; j < pMeshCandidate->GetFaceNum(); j++)
	{
		const CMeshCandidate::FACE& Face = pMeshCandidate->GetFace(j);
		aIndices[iCount++] = Face.aVertIdx[0];
		aIndices[iCount++] = Face.aVertIdx[1];
		aIndices[iCount++] = Face.aVertIdx[2];
	}

	A3DSkinMeshRef ref = {pMesh, true, -1.0f, NULL};
	m_pSkin->GetMuscleMeshArray().Add(ref);

	return true;
}

//	Emit bone bounding boxes
bool CExportor::EmitBoneBoxes()
{
/*	for (int i=0; i < pSkinCandidate->GetBoneBoxNum(); i++)
	{
		const CSkinCandidate::BONEBOX& SrcBox = pSkinCandidate->GetBoneBox(i);
		if (!SrcBox.bValid)
			continue;

		A3DBoneHitBox* pDestBox = new A3DBoneHitBox;
		if (!pDestBox)
			return false;

		pDestBox->SetBone(SrcBox.iBone);
		pDestBox->SetAABB(SrcBox.aabb);

		pSkin->AddBoneHitBox(pDestBox);
	}
*/
	return true;
}

//	Emit hit boxes
bool CExportor::EmitHitBoxes()
{
	TimeValue time = m_i->GetTime();
	Point3 pos, x, y, z, o;
	Matrix3 matObject, matParent;
	A3DOBB obb;

	for (int i=0; i < m_pPhyCandidate->GetHitBoxNodeNum(); i++)
	{
		INode* pINode = m_pPhyCandidate->GetHitBoxNode(i)->GetINode();
		INode* pParent = pINode->GetParentNode();

		if (!pParent || pParent == m_i->GetRootNode())
		{
			a_SendLogToDebugWnd(1, "CExportor::EmitHitBoxes, Couldn't find hit box's parent");
			continue;
		}
	
		int iBoneIdx = m_pSkeletonCandidate->GetBoneSelectedId(pParent->GetName());
		if (iBoneIdx < 0)
		{
			a_SendLogToDebugWnd(1, "CExportor::EmitHitBoxes, Hit box's parent isn't selected");
			continue;
		}

		ObjectState os = pINode->EvalWorldState(time);
		Box3 box = ((DummyObject*)os.obj)->GetBox();

		matParent = g_pMAXInterface->NormalizeTM(pParent->GetNodeTM(time));
		matParent.Invert();

		matObject = pINode->GetObjTMAfterWSM(time) * matParent;
		
		//	Transform box into parent's frame;
		box.pmin = box.pmin * matObject;
		box.pmax = box.pmax * matObject;

		//	Transform axis;
		o = Point3.Origin * matObject;
		x = Point3.XAxis * matObject - o;
		y = Point3.ZAxis * matObject - o;
		z = Point3.YAxis * matObject - o;

		x = x.Normalize();
		y = y.Normalize();
		z = z.Normalize();

		if (fabs(DotProd(CrossProd(x, y), z)) < 1.0f - 1e-3)
		{
			a_SendLogToDebugWnd(1, "CExportor::EmitHitBoxes, Distorted hit box");
			continue;
		}

		//	Calculate OBB;
		pos = (box.pmin + box.pmax) * 0.5f;
		obb.Center.Set(pos.x, pos.z, pos.y);
		obb.XAxis.Set(x.x, x.z, x.y);
		obb.YAxis.Set(y.x, y.z, y.y);
		obb.ZAxis.Set(z.x, z.z, z.y);
		
		pos = (box.pmax - box.pmin) * 0.5f;
		obb.Extents.x = fabs(DotProd(pos, x)); 
		obb.Extents.y = fabs(DotProd(pos, y));
		obb.Extents.z = fabs(DotProd(pos, z));

		obb.ExtX = obb.XAxis * obb.Extents.x;
		obb.ExtY = obb.YAxis * obb.Extents.y;
		obb.ExtZ = obb.ZAxis * obb.Extents.z;

		//	Add hit bot to skin model
		m_pSkinModel->AddSkinHitBox(pINode->GetName(), iBoneIdx, obb);
	}

	return true;
}

//	Emit physique shapes
bool CExportor::EmitPhysiqueShapes()
{
	TimeValue time = m_i->GetTime();
	Point3 pos, x, y, z, o;
	Matrix3 matObject, matParent;
	A3DCCYLINDER cc;
	Interval Inter;

	enum
	{
		TYPE_SPHERE = 0,	//	Sphere
		TYPE_CYLINDER,		//	Cylinder
		TYPE_UNKNOWN,		//	Unknown
	};

	for (int i=0; i < m_pPhyCandidate->GetPhysiqueShapeNum(); i++)
	{
		INode* pINode = m_pPhyCandidate->GetPhysiqueShape(i)->GetINode();
		INode* pParent = pINode->GetParentNode();

		if (!pParent || pParent == m_i->GetRootNode())
		{
			a_SendLogToDebugWnd(1, "CExportor::EmitPhysiqueShapes, Couldn't find physique shape's parent");
			continue;
		}
	
		int iBoneIdx = m_pSkeletonCandidate->GetBoneSelectedId(pParent->GetName());
		if (iBoneIdx < 0)
		{
			a_SendLogToDebugWnd(1, "CExportor::EmitPhysiqueShapes, Physique shape's parent isn't selected");
			continue;
		}

		int iType = TYPE_UNKNOWN;

		ObjectState os = pINode->EvalWorldState(time);
		if (os.obj->CanConvertToType(Class_ID(CYLINDER_CLASS_ID, 0)))
			iType = TYPE_CYLINDER;
		else if (os.obj->CanConvertToType(Class_ID(SPHERE_CLASS_ID, 0)))
			iType = TYPE_SPHERE;
		else
		{
			a_SendLogToDebugWnd(1, "CExportor::EmitPhysiqueShapes, Physique shape's parent isn't selected");
			continue;
		}

		//	Get object's bounding box
		Box3 box;
		os.obj->GetDeformBBox(time, box);

		matParent = g_pMAXInterface->NormalizeTM(pParent->GetNodeTM(time));
		matParent.Invert();
		matObject = pINode->GetObjTMAfterWSM(time) * matParent;
		
		//	Transform box into parent's frame;
		box.pmin = box.pmin * matObject;
		box.pmax = box.pmax * matObject;

		//	Transform axis;
		o = Point3.Origin * matObject;
		x = Point3.XAxis * matObject - o;
		y = Point3.ZAxis * matObject - o;
		z = Point3.YAxis * matObject - o;

		x = x.Normalize();
		y = y.Normalize();
		z = z.Normalize();

		if (fabs(DotProd(CrossProd(x, y), z)) < 1.0f - 1e-3)
		{
			a_SendLogToDebugWnd(1, "CExportor::EmitPhysiqueShapes, Distorted hit box");
			continue;
		}

		//	Calculate capped-cylinder
		pos = (box.pmin + box.pmax) * 0.5f;
		cc.vCenter.Set(pos.x, pos.z, pos.y);
		cc.vAxisX.Set(x.x, x.z, x.y);
		cc.vAxisY.Set(y.x, y.z, y.y);
		cc.vAxisZ.Set(z.x, z.z, z.y);

		if (iType == TYPE_CYLINDER)
		{
			GenCylinder* pCylinder = (GenCylinder*)os.obj->ConvertToType(time, Class_ID(CYLINDER_CLASS_ID, 0));
			pCylinder->pblock->GetValue(PB_CYL_HEIGHT, time, cc.fHalfLen, Inter);
			pCylinder->pblock->GetValue(PB_CYL_RADIUS, time, cc.fRadius, Inter);

			cc.fHalfLen *= 0.5f;
		}
		else if (iType == TYPE_SPHERE)
		{
			GenSphere* pSphere = (GenSphere*)os.obj->ConvertToType(time, Class_ID(SPHERE_CLASS_ID, 0));
			pSphere->pblock->GetValue(PB_SPHERE_RADIUS, time, cc.fRadius, Inter);

			cc.fHalfLen	= 0.0f;
		}
		
		//	Add physique shape to skin model
		m_pSkinModel->AddSkinPhyShape(pINode->GetName(), iBoneIdx, cc);
	}

	return true;
}

//	Update skeleton file
bool CExportor::UpdateSkeletonFile(const char* szFile, CSkeletonCandidate* pSkeletonCandidate)
{
	//	Open skeleton file
	CSkeletonWrapper Skeleton;

	if (!Skeleton.Init())
	{
		a_SendLogToDebugWnd(1, "CExportor::UpdateSkeletonFile, Failed to initialize A3DSkeleton");
		return false;
	}

	if (!Skeleton.Load(szFile))
	{
		a_SendLogToDebugWnd(1, "CExportor::UpdateSkeletonFile, Failed to load skeleton file %s", szFile);
		return false;
	}

	//	Get bone candidate vector
	APtrArray<CBoneCandidate*>& aBones = pSkeletonCandidate->GetBoneArray();

	for (int iBone=0; iBone < aBones.GetSize(); iBone++)
	{
		//	Get the bone candidate
		CBoneCandidate* pBoneCandidate = aBones[iBone];
		if (!pBoneCandidate->InitTMChanged())
			continue;

		A3DBone* pBone = Skeleton.GetBone(pBoneCandidate->GetName(), NULL);
		if (pBone)
		{
			A3DMATRIX4 mat;
			g_pMAXInterface->MatrixFromMAXToA3D(pBoneCandidate->GetInitTM(), mat);
			pBone->SetBoneInitTM(mat);
		}
	}

	if (!Skeleton.Save(szFile))
	{
		a_SendLogToDebugWnd(1, "CExportor::UpdateSkeletonFile, Failed to save skeleton file %s", szFile);
		return false;
	}

	Skeleton.Release();

	return true;
}

//	Release temporary resources
void CExportor::ReleaseTempRes(A3DSkin* pSkin)
{
	APtrArray<A3DTexture*>& aTextures = pSkin->GetTextureArray();
	APtrArray<A3DMaterial*>& aMaterials = pSkin->GetMaterialArray();

	for (int i=0; i < aTextures.GetSize(); i++)
	{
		A3DTexture* pTexture = (A3DTexture*)aTextures[i];
		delete pTexture;
	}

	aTextures.RemoveAll();
	aMaterials.RemoveAll();
}

//	Build skin bone name list
bool CExportor::EmitSkinBoneNames()
{
	int i, j;

	for (i=0; i < m_pSkinCandidate->GetMeshNum(); i++)
	{
		CMeshCandidate* pMeshCandidate = (CMeshCandidate*)m_pSkinCandidate->GetMesh(i);

		if (pMeshCandidate->IsMorphMesh() || pMeshCandidate->IsMuscleMesh() ||
			pMeshCandidate->IsSuppleMesh() || pMeshCandidate->IsRigidMesh())
			continue;

		int iNumVert = pMeshCandidate->GetVertexNum();

		for (j=0; j < iNumVert; j++)
		{
			CVertexCandidate* pVertCandidate = pMeshCandidate->GetVertexCandidate(j);

			int iNumWeight = pVertCandidate->GetInfluenceNum();
			if (iNumWeight > 4)
				iNumWeight = 4;

			for (int n=0; n < iNumWeight; n++)
			{
				CVertexCandidate::INFLUENCE Influence = pVertCandidate->GetInfluence(n);
				
				//	Note: here we remap bone's index !
				CBoneCandidate* pBone = m_pSkeletonCandidate->GetBoneCandidateBySelectedID(Influence.iBone);
				SearchBoneNameInSkin(m_pSkin, pBone->GetName(), true);
			}
		}
	}

	return true;
}

//	Search bone name in skin's bone name list
int CExportor::SearchBoneNameInSkin(A3DSkin* pSkin, const char* szName, bool bAddNew)
{
	APtrArray<AString*>& aNames = pSkin->GetBoneNameArray();

	//	Check whether name has existed or not.
	for (int i=0; i < aNames.GetSize(); i++)
	{
		if (!aNames[i]->CompareNoCase(szName))
			return i;
	}

	if (bAddNew)
	{
		//	Name doesn't exists, add it
		AString* pstr = new AString(szName);
		return aNames.Add(pstr);
	}
	else
		return -1;
}

