/*
 * FILE: PhysiqueCandidate.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/5/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "PhysiqueCandidate.h"
#include "MAXInterface.h"
#include "MAXNode.h"

#include "AF.h"
#include "A3D.h"

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
//	Implement CPhysiqueCandidate
//	
///////////////////////////////////////////////////////////////////////////

CPhysiqueCandidate::CPhysiqueCandidate()
{
}

CPhysiqueCandidate::~CPhysiqueCandidate()
{
}

//	Initialize object
bool CPhysiqueCandidate::Init()
{
	int iNode;
	
	//	Loop through all the selected nodes
	for (iNode=0; iNode < g_pMAXInterface->GetSelectedNodeCount(); iNode++)
	{
		//	Get the selected node
		CMAXNode* pNode = g_pMAXInterface->GetSelectedNode(iNode);

		int iType = pNode->GetType();

		if (iType == NODE_HITBOX)
			m_aHitBoxes.Add(pNode);
		else if (iType == NODE_PHYSHAPE)
			m_aPhyShapes.Add(pNode);
	}

	return true;
}

//	Release object
void CPhysiqueCandidate::Release()
{
	int i;

	//	Release all hit boxes
	for (i=0; i < m_aHitBoxes.GetSize(); i++)
	{
		CMAXNode* pNode = m_aHitBoxes[i];
		delete pNode;
	}

	//	Release all physique shapes
	for (i=0; i < m_aPhyShapes.GetSize(); i++)
	{
		CMAXNode* pNode = m_aPhyShapes[i];
		delete pNode;
	}
}


