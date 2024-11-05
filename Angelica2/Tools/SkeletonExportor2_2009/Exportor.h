/*
 * FILE: Exportor.h
 *
 * DESCRIPTION: Exportor routines
 *
 * CREATED BY: duyuxin, 2003/11/19
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EXPORTOR_H_
#define _EXPORTOR_H_

#include "max.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinModel;
class A3DSkeleton;
class A3DSkin;
class CSkinCandidate;
class CSkeletonCandidate;
class CMeshCandidate;
class CPhysiqueCandidate;
class A3DSkinMesh;
class A3DRigidMesh;
class A3DMorphMesh;
class CTrackSetWrapper;
class CSkeletonWrapper;
class CSkinWrapper;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CExportor
//
///////////////////////////////////////////////////////////////////////////

class CExportor
{
public:		//	Types

public:		//	Constructions and Destructions

	CExportor();
	virtual ~CExportor();

public:		//	Attributes

public:		//	Operaitons

	//	Initialize object
	bool Init(ExpInterface* ei, Interface* i);

	//	Export skeleton file
	bool ExportSkeleton(const char* szFile, CSkeletonCandidate* pSkeletonCandidate);
	//	Export skin file
	bool ExportSkin(const char* szFile, CSkinCandidate* pSkinCandidate, CSkeletonCandidate* pSkeletonCandidate);
	//	Export skin model file
	bool ExportSkinModel(const char* szFile, const char* szSkeleton, const char* szSkin, const char* szPhysique);
	//	Export physique file
	bool ExportPhysique(const char* szFile, CPhysiqueCandidate* pPhyCandidate, CSkeletonCandidate* pSkeletonCandidate);

protected:	//	Attributes

	ExpInterface*	m_ei;		//	Export interface
	Interface*		m_i;		//	Interface

	CTrackSetWrapper*	m_pTrackSet;			//	Current track set object
	CSkeletonWrapper*	m_pSkeleton;			//	Current skeleton object
	CSkinWrapper*		m_pSkin;
	A3DSkinModel*		m_pSkinModel;
	CSkeletonCandidate*	m_pSkeletonCandidate;
	CSkinCandidate*		m_pSkinCandidate;
	CPhysiqueCandidate*	m_pPhyCandidate;

	int		m_iNumSelectedBone;		//	Number of selected bone

protected:	//	Operations

	//	Emit bones
	bool EmitBones();
	//	Emit animations
	bool EmitAnimations();
	//	Emit hooks
	bool EmitHooks();

	//	Emit textures and materials
	bool EmitTexturesAndMaterials(const char* szTexPath);
	//	Emit meshes
	bool EmitMeshes();
	//	Emit a skin mesh
	bool EmitSkinMesh(CMeshCandidate* pMeshCandidate);
	//	Emit a rigid mesh
	bool EmitRigidMesh(CMeshCandidate* pMeshCandidate);
	//	Emit a morph skin mesh
	bool EmitMorphSkinMesh(CMeshCandidate* pMeshCandidate);
	//	Emit a morph rigid mesh
	bool EmitMorphRigidMesh(CMeshCandidate* pMeshCandidate);
	//	Emit a supple rigid mesh
	bool EmitSuppleRigidMesh(CMeshCandidate* pMeshCandidate);
	//	Emit a supple skin mesh
	bool EmitSuppleSkinMesh(CMeshCandidate* pMeshCandidate);
	//	Emit a muscle rigid mesh
	bool EmitMuscleRigidMesh(CMeshCandidate* pMeshCandidate);
	//	Emit a muscle skin mesh
	bool EmitMuscleSkinMesh(CMeshCandidate* pMeshCandidate);
	//	Fill skin mesh data
	bool FillSkinMeshData(A3DSkinMesh* pMesh, CMeshCandidate* pMeshCandidate);
	//	Fill rigid mesh data
	bool FillRigidMeshData(A3DRigidMesh* pMesh, CMeshCandidate* pMeshCandidate);
	//	Fill morph mesh data
	bool FillMorphMeshData(A3DMorphMesh* pMesh, CMeshCandidate* pMeshCandidate);
	//	Build skin bone name list
	bool EmitSkinBoneNames();
	//	Emit bone bounding boxes
	bool EmitBoneBoxes();
	//	Emit hit boxes
	bool EmitHitBoxes();
	//	Emit physique shapes
	bool EmitPhysiqueShapes();

	//	Save track set data to file
	bool SaveTrackSetData(const char* szSkeletonFile);
	//	Update skeleton file
	bool UpdateSkeletonFile(const char* szFile, CSkeletonCandidate* pSkeletonCandidate);
	//	Release temporary resources
	void ReleaseTempRes(A3DSkin* pSkin);
	//	Search bone name in skin's bone name list
	int SearchBoneNameInSkin(A3DSkin* pSkin, const char* szName, bool bAddNew);
};

bool Util_CopyFile(const char* szSrcPath, const char* szDestPath, BOOL bFailIfExists);
///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_EXPORTOR_H_

