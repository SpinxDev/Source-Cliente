/*
 * FILE: A3DTerrain2Wrapper.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/7/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2WRAPPER_H_
#define _A3DTERRAIN2WRAPPER_H_

#include "AArray.h"
#include "A3DTerrain2.h"
#include "A3DVertex.h"

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

class A3DTerrain2CullHC;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2Wrapper
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2Wrapper : public A3DTerrain2
{
public:		//	Types

	struct BLKEDGE
	{
		float	y1;
		float	y2;
	};

public:		//	Constructor and Destructor

	A3DTerrain2Wrapper();
	virtual ~A3DTerrain2Wrapper();

public:		//	Attributes

public:		//	Operations

	//	Initialize terrain object
	bool Init(A3DEngine* pA3DEngine, float fViewRadius, float fActRadius);
	//	Release terrain object
	virtual void Release();

	//	Load terrain data from file
	bool Load(const char* szFileName, float cx, float cz);
	//	Render routine
	bool Render(A3DViewport* pViewport);

	//	Render block quads
	bool RenderBlockQuads(A3DViewport* pViewport, int r_hl, int c_hl);
	//	Render horizon culling line
	bool RenderHorizonCull(A3DViewport* pViewport);

protected:	//	Attributes

	A3DTerrain2CullHC*	m_pHCCuller;	//	Horizon culler object

	AArray<A3DTLVERTEX, A3DTLVERTEX&>	m_aTLVerts;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DTERRAIN2WRAPPER_H_
