/*
 * FILE: Model.h
 *
 * DESCRIPTION: Model class
 *
 * CREATED BY: Duyuxin, 2003/9/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_MODEL_H_
#define _MODEL_H_

#include "AArray.h"
#include "AList2.h"
#include "A3DTypes.h"
#include "Box3D.h"
#include "InterfaceWrapper.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Type of stop callback 
enum
{
	SCB_UNKNOWN = 0,
	SCB_GROUPACTION,
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CRender;
class A3DSkinModel;
class A3DSkinModelAction;
class A3DSkeleton;
class A3DSkin;
class A3DViewport;
class CBox3D;
class CStaff;
class CHook;
class CSkinModel;

//	Stop callback user data structure
struct MODELSTOPCALLBACK
{
	int		iType;		//	type
	DWORD	dwData;		//	Data
};

//	Skeleton info
struct SKELETONINFO
{
	DWORD	dwVersion;		//	File's version, SKEFILE_VERSION
	int		iNumBone;		//	Number of bones
	int		iAnimFPS;		//	Animation FPS
	int		iStartFrame;	//	Start frame
	int		iEndFrame;		//	End frame
};

extern MODELSTOPCALLBACK	g_ModelSCBData;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

//	Model action stop callback
void ActionStopCallback(A3DSkinModel* pSkinModel, A3DSkinModelAction* pAction, DWORD dwUserData);

//	Get skinmodel file (.smd) version
DWORD AUX_GetSkinModelFileVer(const char* szFile);
//	Get skeleton file (.bon) version
DWORD AUX_GetSkeletonFileVer(const char* szFile);
//	Convert .bon file to new version
bool AUX_ConvertSkeletonFile(const char* szFile);
//	Read skeleton file info
bool AUX_ReadSkeletonInfo(const char* szFile, SKELETONINFO& info);
//	Read track file info
bool AUX_ReadTrackInfo(const char* szFile, SKELETONINFO& info);

///////////////////////////////////////////////////////////////////////////
//
//	Class CModel
//
///////////////////////////////////////////////////////////////////////////

class CModel
{
public:		//	Types

	struct BONENODE
	{
		APtrArray<CStaff*>	aLinkStaffs;	//	Staffs that link parent and childrend

		CStaff*		aCoorStaffs[3];	//	Coordinates staffs
		CBox3D*		pBox;			//	Box represents bone
		bool		bSelected;		//	Selected flag
	};

public:		//	Constructor and Destructor

	CModel();
	virtual ~CModel() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize model
	bool Init(CRender* pRender);
	//	Release model
	void Release();

	//	Frame move
	bool FrameMove(DWORD dwTime);
	//	Render model
	bool Render(A3DViewport* pViewport);

	//	Load model from file
	bool Load(const char* szFile);
	//	Save model to file
	bool Save(const char* szFile);

	//	Change model's skin
	bool ChangeSkin(int iIndex, const char* szFile);
	//	Get skeleton
	A3DSkeleton* GetSkeleton();
	//	Get skin
	A3DSkin* GetSkin(int i);

	//	Get hook number
	int GetHookNum() { return m_aHooks.GetSize(); }
	//	Get hook by index
	CHook* GetHook(int n) { return m_aHooks[n]; }
	//	Get hook by name
	CHook* GetHook(const char* szName);
	//	Get hook index
	int GetHookIndex(const char* szName);

	//	Get action number
	int GetActionNum();
	//	Get hanger number
	int GetHangerNum();

	//	Selected bone
	void SelectBone(int iBone, bool bSelect);
	void SelectBone(const char* szBone, bool bSelect);
	void SelectBone(const RECT& rcSel, A3DViewport* pViewport, bool bAdd);
	//	Bone is selected ?
	bool BoneIsSelected(int iBone);
	bool BoneIsSelected(const char* szBone);
	//	Clear all bones' selection state
	void ClearBoneSelection();

	//	Set box size and staff radius
	void SetBoxAndStaffSize(float fBoxSize, float fStaffRadius);

	//	Get model center
	const A3DVECTOR3& GetModelCenter() { return m_vModelCenter; }
	//	Get model radius
	float GetModelRadius() { return m_fModelRadius; }

	//	Add rotate degree arounding Y axis
	void AddRotateY(float fDeg);
	//	Add rotate degree arounding X axis
	void AddRotateX(float fDeg);

	//	Get A3D skin model object
	CSkinModel* GetA3DSkinModel() { return m_pA3DModel; }

protected:	//	Attributes

	CRender*		m_pRender;			//	Render object
	CSkinModel*		m_pA3DModel;		//	A3D model object

	APtrArray<BONENODE*>	m_aBoneNodes;	//	Bone node array
	APtrArray<CHook*>		m_aHooks;		//	Hook array

	A3DVECTOR3		m_vModelCenter;		//	Model center
	float			m_fModelRadius;		//	Model radius

	float			m_fRotateY;			//	Degree around Y axis
	float			m_fRotateX;			//	Degree around X axis
	A3DMATRIX4		m_matRotate;		//	Rotate matrix
	CBox3D			m_ModelBox;			//	Bounding box of model

protected:	//	Operations

	//	Build bone nodes
	bool BuildBoneNodes();
	//	Release bone nodes
	void ReleaseBoneNodes();
	//	Build hooks
	bool BuildHooks();
	//	Release all hooks of specified skeleton
	void ReleaseHooks();
	//	Release all hooks
	void ReleaseAllHooks();

	//	Render bone nodes
	bool RenderBoneNodes();
	//	Render model's bounding box
	bool RenderModelBox();
	//	Render physique shapes
	bool RenderPhysiqueShapes();
	//	Draw model's normal
	bool RenderNormals();

	//	Update bone nodes
	bool UpdateBoneNodes();
	//	Update hooks
	bool UpdateHooks();

	//	Calculate model center and radius
	void CalcModelSize();
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_MODEL_H_
