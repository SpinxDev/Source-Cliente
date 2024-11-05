/*
 * FILE: BoneCandidate.h
 *
 * DESCRIPTION: Bone candidate class
 *
 * CREATED BY: duyuxin, 2003/8/26
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _BONECANDIDATE_H_
#define _BONECANDIDATE_H_

#include "max.h"
#include "AString.h"
#include "AArray.h"
#include "MAXNode.h"

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

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CBoneCandidate
//
///////////////////////////////////////////////////////////////////////////

class CBoneCandidate
{
public:		//	Types

public:		//	Constructions and Destructions

	CBoneCandidate();
	virtual ~CBoneCandidate();

public:		//	Attributes

public:		//	Operaitons

	bool Create(int iID, int iParentId, CMAXNode* pNode, bool bFake);

	bool AddChildId(int iChildId);
	bool AddHookId(int iHookId);
	int GetChildNum() { return m_aChildren.GetSize(); }
	int GetHookNum() { return m_aHooks.GetSize(); }
	int GetChildID(int n) { return m_aChildren[n]; }
	void SetChildID(int n, int id) { m_aChildren[n] = id; }
	int GetHookID(int n) { return m_aHooks[n]; }

	CMAXNode* GetNode() { return m_pNode; }
	int GetId() { return m_iID; }
	void SetId(int id) { m_iID = id; }
	const char* GetName() { return m_pNode->GetName(); }
	int GetParentId() { return m_iParent; }
	void SetParentId(int id) { m_iParent = id; }
	int GetSelectedId() { return m_iSelectedId; }
	bool IsSelected() { return m_bSelected; }
	void SetSelected(bool bSelected) { m_bSelected = bSelected; }
	void SetSelectedId(int iSelectedId) { m_iSelectedId = iSelectedId; }
	void SetSkeletonId(int iID) { m_iSkeletonId = iID; }
	int GetSkeletonId() { return m_iSkeletonId; }

	void SetInitTM(const Matrix3& mat);
	const Matrix3& GetInitTM() { return m_matInitTM; }
	const Point3& GetNodeTMScale() { return m_vNodeTMScale; } 

	void SetFakeBone(bool bFake) { m_bFakeBone = bFake; }
	bool IsFakeBone() { return m_bFakeBone; }

	void SetHookFlag(bool bHook) { m_bHook = bHook; } 
	bool IsHook() { return m_bHook; }

	void SetFlipFlag(bool bFlip) { m_bFlipped = bFlip; }
	bool GetFlipFlag() { return m_bFlipped; }

	bool InitTMChanged() { return m_bInitTMChanged; }

protected:	//	Attributes

	int			m_iID;
	int			m_iParent;			//	Parent's ID
	bool		m_bSelected;
	CMAXNode*	m_pNode;
	int			m_iSelectedId;
	int			m_iSkeletonId;		//	ID in skeleton
	bool		m_bFakeBone;		//	Fake bone flag
	bool		m_bHook;			//	This bone is a hook
	bool		m_bFlipped;			//	Flag flag

	bool		m_bInitTMChanged;	//	Init TM changed flag
	Matrix3		m_matInitTM;		//	Bone's initial TM which convert vertex form world space to bone space
	Point3		m_vNodeTMScale;		//	Scale of node TM when exporting

	AArray<int, int>	m_aChildren;	//	Children ID array
	AArray<int, int>	m_aHooks;		//	Hook ID array

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_BONECANDIDATE_H_

