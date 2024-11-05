/*
 * FILE: MAXNode.cpp
 *
 * DESCRIPTION: 3DMAX node class
 *
 * CREATED BY: duyuxin, 2003/8/25
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "MAXNode.h"
#include "MAXInterface.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


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
//	Implement CMAXNode
//
///////////////////////////////////////////////////////////////////////////

CMAXNode::CMAXNode()
{
	m_pINode = NULL;
}

bool CMAXNode::Create(INode *pINode)
{
	if (!pINode)
		return false;

	m_pINode = pINode;

	return true;
}

CMAXNode* CMAXNode::GetChild(int iChildId)
{
	//	Check if the internal node is valid
	if (!m_pINode)
		return NULL;

	//	Check if the given node id is valid
	if (iChildId < 0 || iChildId >= m_pINode->NumberOfChildren())
		return NULL;
	
	//	Allocate a new max node instance
	CMAXNode* pNode = new CMAXNode;
	if (!pNode)
		return NULL;

	//	Create the max node
	if (!pNode->Create(m_pINode->GetChildNode(iChildId)))
	{
		delete pNode;
		return NULL;
	}

	return pNode;
}

int CMAXNode::GetChildCount()
{
	//	Check if the internal node is valid
	if (!m_pINode)
		return 0;

	return m_pINode->NumberOfChildren();
}

const char* CMAXNode::GetName()
{
	//	Check if the internal node is valid
	if (!m_pINode)
		return "<void>";

	return m_pINode->GetName();
}

MAXNODETYPE CMAXNode::GetType()
{
	if (!g_pMAXInterface)
		return NODE_OTHER;

	//	Find the corresponding type of the node
	if (g_pMAXInterface->IsBone(this))
		return NODE_BONE;
	else if (g_pMAXInterface->IsDummy(this))
	{
		if (g_pMAXInterface->IsHook(this))
			return NODE_HOOK;
		else if (g_pMAXInterface->IsHitBox(this))
			return NODE_HITBOX;
		else
			return NODE_DUMMY;
	}
	else if (g_pMAXInterface->IsPhyShape(this))
	{
		//	Is a physique-shape node ?
		return NODE_PHYSHAPE;
	}
	else if (g_pMAXInterface->IsMesh(this))
		return NODE_MESH;

	return NODE_OTHER;
}

