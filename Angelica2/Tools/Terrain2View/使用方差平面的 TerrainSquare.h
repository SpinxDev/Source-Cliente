/*
 * FILE: TerrainSquare.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/7/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#ifndef _TERRAINSQUARE_H_
#define _TERRAINSQUARE_H_

#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Identity and version of horizon cull file
#define	HORIFILE_IDENTIFY	(('H'<<24) | ('O'<<16) | ('R'<<8) | 'I')
#define HORIFILE_VERSION	1

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

//	Horizon cull file header
struct HORIFILEHEADER
{
	int iNumBlockRow;	//	Number of block row
	int iNumBlockCol;	//	Number of block column
};

//	Block square data
struct HORIFILEBLKSQUARE
{
	float	y1;
	float	y2;
	float	y3;
	float	y4;
	float	fOffsetY;
};

class A3DTerrain2;
class A3DTerrain2Block;
class CTerrainSquare;
class CProgressCtrl;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CBlockSquare
//	
///////////////////////////////////////////////////////////////////////////

class CBlockSquare
{
public:		//	Types

	//	Plane type
	enum
	{
		PT_CENTER = 0,	//	the center plane determined by least squares
		PT_MINIMUM,		//	the minimum plane below all node samples
		PT_MAXIMUM		//	the maximum plane above all node samples
	};

	//	Least square sum
	struct SQUARESUM
	{
		float x, y, z;
		float xx, xy, xz;
		float zz, yz;
		float samples;

		void Clear()
		{
			x = 0;
			y = 0;
			z = 0;
			xx = 0;
			xy = 0;
			xz = 0;
			zz = 0;
			yz = 0;
			samples = 0;
		}

		SQUARESUM& operator += (SQUARESUM &other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			xx += other.xx;
			xy += other.xy;
			xz += other.xz;
			zz += other.zz;
			yz += other.yz;
			samples += other.samples;
			return *this;
		}		
	};

	//	Least square plane
	struct SQUAREPLANE
	{
		float	fNormalX;	//	plane normal x
		float	fNormalZ;	//	plane normal z (normal y=1)
		float	fError1;	//	minimum plane error along normal - about plane d
		float	fError2;	//	maximum plane error along normal + about plane d
		float	d;			//	plane d relative to non-unit length normal
	};

	//	Block quads
	struct QUAD
	{
		A3DVECTOR3 v1, v2, v3, v4;
	};

public:		//	Constructor and Destructor

	CBlockSquare(CTerrainSquare* pTrnSquare);
	virtual ~CBlockSquare();

public:		//	Attributes

public:		//	Operations

	//	Build block square from terrain2 block data
	bool Build(A3DTerrain2Block* pBlock);
	//	Save square data to file
	bool Save(FILE* fp);

	//	Get least square plane
	const SQUAREPLANE& GetPlane() { return m_Plane; }
	//	Get quads
	const QUAD& GetMinQuad() { return m_MinQuad; }
	const QUAD& GetMaxQuad() { return m_MaxQuad; }

protected:	//	Attributes

	CTerrainSquare*	m_pTrnSquare;	//	Terrain square object

	SQUARESUM		m_Sum;		//	Least square sum
	SQUAREPLANE		m_Plane;	//	Least square plane
	QUAD			m_MinQuad;
	QUAD			m_MaxQuad;

protected:	//	Operations

	//	Calculate least square sum
	void CalcLeastSquareSum(A3DTerrain2Block* pBlock);
	//	Calculate lease square plane
	void CalcLeastSquarePlane(A3DTerrain2Block* pBlock);
	//	Calculate least square plane normal
	void CalcLeastSquarePlaneNormal(A3DVECTOR3& vCenter);
	//	Calculate quad
	void CalcQuad(A3DTerrain2Block* pBlock, int iPlaneType, QUAD& quad);
	//	Get offset between block's center to origin
	A3DVECTOR3 GetBlockOffset(A3DTerrain2Block* pBlock);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CTerrainSquare
//	
///////////////////////////////////////////////////////////////////////////

class CTerrainSquare
{
public:		//	Types

public:		//	Constructor and Destructor

	CTerrainSquare();
	virtual ~CTerrainSquare();

public:		//	Attributes

public:		//	Operations

	//	Build terrain squares from terrain2 file
	bool Build(const char* szFile, CProgressCtrl* pProgress=NULL);
	//	Save square data to file
	bool Save(const char* szFile);
	//	Release object
	void Release();

protected:	//	Attributes

	A3DTerrain2*	m_pTerrain;		//	Terrain object
	CProgressCtrl*	m_pProgress;	//	Progress bar
	float			m_fStepSize;

	APtrArray<CBlockSquare*>	m_aBlkSquares;

protected:	//	Operations

	//	Load terrain
	bool LoadTerrain(const char* szFile);
	//	Reset block squares
	void ResetBlockSquares();
	//	Build block squares in current active area
	bool BuildBlockSquares();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_TERRAINSQUARE_H_
