/*
 * FILE: MAXInterface.h
 *
 * DESCRIPTION: 3DMAX interface class
 *
 * CREATED BY: duyuxin, 2003/8/25
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _MAXINTERFACE_H_
#define _MAXINTERFACE_H_

#include "max.h"
#include "stdmat.h"
#include "AString.h"
#include "A3DTypes.h"
#include "A3DQuaternion.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	This is the class for all biped controllers except the root and the footsteps
//	#define BIPSLAVE_CONTROL_CLASS_ID Class_ID(0x9154, 0)
//	This is the class for the center of mass, biped root controller ("Bip01")
//	#define BIPBODY_CONTROL_CLASS_ID  Class_ID(0x9156, 0) 

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CMAXNode;
class CMAXMesh;
class A3DMaterial;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CMAXInterface
//
///////////////////////////////////////////////////////////////////////////

class CMAXInterface
{
public:		//	Types

public:		//	Constructions and Destructions

	CMAXInterface();
	virtual ~CMAXInterface() {}

public:		//	Attributes

public:		//	Operaitons

	bool Init(ExpInterface* pExpInterface, Interface* pInterface);

	int GetCurrentFrame();
	float GetCurrentTime();
	int GetStartFrame();
	int GetEndFrame();
	int GetStartTime();
	int GetEndTime();
	int GetFps();
	CWnd* GetMainWnd();
//	CBaseMaterial* GetMaterial(int materialId);
//	int GetMaterialCount();
	CMAXMesh* GetMesh(CMAXNode *pNode);
	CMAXNode* GetNode(const char* szName);
	int GetSelectedNodeCount();
	CMAXNode* GetSelectedNode(int iNodeId);
	CMAXNode* GetRootNode();
	void GetTranslationAndRotation(CMAXNode* pNode, CMAXNode* pParentNode, float fTime, A3DMATRIX4& mat);
	void GetTranslationAndRotationBoneSpace(CMAXNode* pNode, float fTime, A3DMATRIX4& mat);

	bool IsBone(CMAXNode* pNode);
	bool IsDummy(CMAXNode* pNode);
	bool IsMesh(CMAXNode* pNode);
	bool IsHook(CMAXNode* pNode);
	bool IsHitBox(CMAXNode* pNode);
	bool IsPhyShape(CMAXNode* pNode);

	bool IsBoneINode(INode* pINode);
	bool IsDummyINode(INode* pINode);
	bool IsMeshINode(INode* pINode);
	bool IsHookINode(INode* pINode);
	bool IsHitBoxINode(INode* pINode);
	bool IsPhyShapeINode(INode* pINode);

	//	Check a node contain physique or skin modifier
	int HasPhysiqueOrSkinModifier(INode* pINode);

	void SetProgressInfo(int percentage);
	void StartProgressInfo(const char* szText);
	void StopProgressInfo();

	//	Convert routines between 3DMAX matrix and A3D matrix
	void MatrixFromMAXToA3D(const Matrix3& matMAX, A3DMATRIX4& matA3D);
	//	Convert MAX material to A3D material
	void MaterialFromMAXToA3D(StdMat* pMAXMtl, A3DMaterial* pA3DMtl);

	//	Normalize matrix
	Matrix3 NormalizeTM(const Matrix3& mat);
	//	Get matrix scale
	Point3 GetMatrixScale(const Matrix3& mat);

	ExpInterface* GetExpInterface() { return m_ei; }
	Interface* GetInterface() { return m_i; }

protected:	//	Attributes

	ExpInterface*	m_ei;		//	Export interface
	Interface*		m_i;		//	Interface

protected:	//	Operations

	Matrix3 GetNodeTM(CMAXNode* pNode, float fTime);
	bool IsBipedBone(INode* pNode);

	static DWORD WINAPI ProgressFunction(LPVOID arg);
};

extern CMAXInterface*	g_pMAXInterface;

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_MAXINTERFACE_H_

