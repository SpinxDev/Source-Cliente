/*
 * FILE: PhysiqueCandidate.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/5/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _PHYSIQUECANDIDATE_H_
#define _PHYSIQUECANDIDATE_H_

#include "max.h"
#include "AArray.h"

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
//	Class CPhysiqueCandidate
//	
///////////////////////////////////////////////////////////////////////////

class CPhysiqueCandidate
{
public:		//	Types

public:		//	Constructor and Destructor

	CPhysiqueCandidate();
	virtual ~CPhysiqueCandidate();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init();
	//	Release object
	void Release();

	//	Get hit box node number
	int GetHitBoxNodeNum() { return m_aHitBoxes.GetSize(); }
	//	Get hit box node
	CMAXNode* GetHitBoxNode(int n) { return m_aHitBoxes[n]; }

	//	Get physique shape number
	int GetPhysiqueShapeNum() { return m_aPhyShapes.GetSize(); }
	//	Get physique shape
	CMAXNode* GetPhysiqueShape(int n) { return m_aPhyShapes[n]; }

protected:	//	Attributes

	APtrArray<CMAXNode*>	m_aHitBoxes;	//	Hit boxes
	APtrArray<CMAXNode*>	m_aPhyShapes;	//	Physique shapes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_PHYSIQUECANDIDATE_H_
