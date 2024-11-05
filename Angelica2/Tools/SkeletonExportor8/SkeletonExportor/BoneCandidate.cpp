/*
 * FILE: BoneCandidate.cpp
 *
 * DESCRIPTION: Bone candidate class
 *
 * CREATED BY: duyuxin, 2003/8/26
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "BoneCandidate.h"
#include "MAXInterface.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CBoneCandidate
//
///////////////////////////////////////////////////////////////////////////

CBoneCandidate::CBoneCandidate()
{
	m_pNode				= NULL;
	m_bInitTMChanged	= false;
	m_bFakeBone			= false;
	m_bHook				= false;
	m_iSelectedId		= -1;
	m_bFlipped			= false;

	m_matInitTM.IdentityMatrix();
}

CBoneCandidate::~CBoneCandidate()
{
	if (m_pNode)
	{
		delete m_pNode;
		m_pNode = NULL;
	}
}

bool CBoneCandidate::Create(int iID, int iParentId, CMAXNode* pNode, bool bFake)
{
	m_iID		= iID;
	m_iParent	= iParentId;
	m_pNode		= pNode;
	m_bFakeBone	= bFake;

	MAXNODETYPE Type = pNode->GetType();
	
	if (Type != NODE_DUMMY && Type != NODE_OTHER)
		m_bSelected = true;
	else
		m_bSelected = false;
	
	//	Calculate current node TM scale
	Matrix3 tmNode = pNode->GetINode()->GetNodeTM(0);
	m_vNodeTMScale = g_pMAXInterface->GetMatrixScale(tmNode);

	return true;
}

bool CBoneCandidate::AddChildId(int iChildId)
{
	m_aChildren.Add(iChildId);
	return true;
}

bool CBoneCandidate::AddHookId(int iHookId)
{
	m_aHooks.Add(iHookId);
	return true;
}

void CBoneCandidate::SetInitTM(const Matrix3& mat)
{ 
	m_matInitTM = mat;
	m_bInitTMChanged = true;
}


