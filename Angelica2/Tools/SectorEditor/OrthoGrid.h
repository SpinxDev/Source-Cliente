/*
 * FILE: OrthoGrid.h
 *
 * DESCRIPTION: Grid used in orthogonal window
 *
 * CREATED BY: Duyuxin, 2003/7/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_ORTHOGRID_H_
#define _ORTHOGRID_H_

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

class CRender;
class A3DStream;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COrthoGrid
//
///////////////////////////////////////////////////////////////////////////

class COrthoGrid
{
public:		//	Types

	struct RECTF
	{
		float	left;
		float	top;
		float	right;
		float	bottom;

		void Set(float l, float t, float r, float b)
		{
			left	= l;
			right	= r;
			top		= t;
			bottom	= b;
		}
	};

public:		//	Constructor and Destructor

	COrthoGrid();
	virtual ~COrthoGrid() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize grid object
	bool Init(CRender* pRender, int iAxisH, int iAxisV, int iAxisZ);
	//	Release object
	void Release();

	//	Render grid
	bool Render();
	//	Resize grid
	bool Resize(float l, float r, float b, float t, float fGridSize);
	bool Resize(float fExtH, float fExtV, float fGridSize);
	//	Move grid
	bool MoveTo(float l, float b);
	bool Move(float fOffH, float fOffV);

	//	Get grid size
	float GetGridSize() { return m_fGridSize; }

protected:	//	Attributes

	CRender*	m_pRender;
	A3DStream*	m_pStream;

	int			m_iAxisH;		//	Three axis's index
	int			m_iAxisV;
	int			m_iAxisZ;
	float		m_fGridSize;	//	Grid size
	RECTF		m_AlignArea;	//	Align grid area
	RECTF		m_Area;			//	Non-align grid area
	
	float		m_fLeft;		//	Position of grid's left-bottom position
	float		m_fBottom;
	int			m_iNumLineH;	//	Number of horizontal line
	int			m_iNumLineV;	//	Number of vertical line

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_ORTHOGRID_H_
