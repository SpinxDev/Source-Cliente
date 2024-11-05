/*
 * FILE: SkeletonCandidate.h
 *
 * DESCRIPTION: Skeleton candidate class
 *
 * CREATED BY: duyuxin, 2003/8/26
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _SKELETONCANDIDATE_H_
#define _SKELETONCANDIDATE_H_

#include "max.h"
#include "AArray.h"
#include "AList2.h"
#include "AString.h"
#include "A3DTypes.h"

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

class CMAXNode;
class A3DSkeleton;
class A3DBone;
class CBoneCandidate;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSkeletonCandidate
//
///////////////////////////////////////////////////////////////////////////

class CSkeletonCandidate
{
public:		//	Types

public:		//	Constructions and Destructions

	CSkeletonCandidate();
	virtual ~CSkeletonCandidate();

public:		//	Attributes

public:		//	Operaitons

	//	Initialize object
	bool Init();
	//	Initialize from file
	bool InitFromFile(const char* szSkeletonFile);
	//	Release object
	void Release();

	//	Set bone init matrices
	bool SetBoneInitMatrices();

	//	Get selected parent of specified bone
	int GetParentSelectedId(bool bBone, int iIndex);
	//	Build selected ID of bones
	int BuildSelectedId();
	//	Get specified node's transform matrix relative to parent node
	void GetTranslationAndRotation(bool bBone, int iIndex, float fTime, A3DMATRIX4& mat);
	//	Get specified node's transform matrix in bone space
	void GetTranslationAndRotationBoneSpace(bool bBone, int iIndex, float fTime, A3DMATRIX4& mat);
	//	Check bone name collision
	bool CheckBoneNameCollision();
	//	Sort all bones
	void SortBones();
	//	Get bone's ID through it's name
	int GetBoneId(const char* szName);
	//	Get bone's selelcted ID through it's name
	int GetBoneSelectedId(const char* szName);
	//	Get bone
	CBoneCandidate* GetBoneCandidate(int n) { return m_aBones[n]; }
	//	Get bone total number
	int GetBoneNum() { return m_aBones.GetSize(); }
	//	Get selected bone's number
	int GetSelectedBoneNum() { return m_iNumSelBone; } 
	//	Get bone by selected id
	CBoneCandidate* GetBoneCandidateBySelectedID(int idSelected);

	APtrArray<CBoneCandidate*>& GetBoneArray() { return m_aBones; }
	AList2<int, int>& GetRootBoneList() { return m_RootBoneList; }

	//	Get hook number
	int GetHookNum() { return m_aHooks.GetSize(); }
	//	Add a hook
	int AddHook(CBoneCandidate* pHook) { return m_aHooks.Add(pHook); }
	//	Get hook
	CBoneCandidate* GetHook(int n) { return m_aHooks[n]; }
 
	//	Set and get start frame
	void SetStartFrame(int iFrame) { m_iStartFrame = iFrame; }
	int GetStartFrame() { return m_iStartFrame; }

	//	Set and get end frame
	void SetEndFrame(int iFrame) { m_iEndFrame = iFrame; }
	int GetEndFrame() { return m_iEndFrame; }

	//	Set / Get animation FPS
	void SetAnimFPS(int iFPS) { m_iAnmiFPS = iFPS; }
	int GetAnimFPS() { return m_iAnmiFPS; }

	//	Get skeleton file name
	const char* GetFileName() { return m_strSkeletonFile; }
	
	//	Set / Get update file flag
	void SetUpdateFileFlag(bool bUpdate) { m_bUpdateFile = bUpdate; }
	bool GetUpdateFileFlag() { return m_bUpdateFile; }

protected:	//	Attributes

	APtrArray<CBoneCandidate*>	m_aBones;
	APtrArray<CBoneCandidate*>	m_aHooks;
	AList2<int, int>			m_RootBoneList;

	AString	m_strSkeletonFile;
	int		m_iStartFrame;
	int		m_iEndFrame;
	int		m_iAnmiFPS;
	bool	m_bUpdateFile;		//	Update skeleton file flag
	int		m_iNumSelBone;		//	Number of selected bone

protected:	//	Operations

	bool AddNode(CMAXNode* pNode, int iParent, bool bSelected);
	bool AddNode(A3DSkeleton* pA3DSkeleton, A3DBone* pBone, int iParent);

	//	Add mesh nodes
	bool AddMeshNode(CMAXNode* pMAXNode, APtrArray<CMAXNode*>& aMeshes);
	//	Check name collision of bone node
	bool CheckNameCollision(const char* szName);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SKELETONCANDIDATE_H_

