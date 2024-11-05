/*
 * FILE: MorphChannel.h
 *
 * DESCRIPTION: Morph channel class
 *
 * CREATED BY: duyuxin, 2003/11/7
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _MORPHCHANNEL_H_
#define _MORPHCHANNEL_H_

#include "max.h"
#include "wm3.h"
#include "A3DTypes.h"
#include "AArray.h"
#include "AString.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CMorphChannel
//
///////////////////////////////////////////////////////////////////////////

class CMorphChannel
{
public:		//	Types

public:		//	Constructions and Destructions

	CMorphChannel();
	virtual ~CMorphChannel();

public:		//	Attributes

public:		//	Operaitons

	//	Initialize object
	bool Init(MorphR3* pMorphR3, int iChannel, const char* szName);
	//	Release object
	void Release();

	//	Build channel weights
	bool BuildChannelWeights(int iStartFrame, int iNumFrame, int iFPS);
	//	Calculate vertex's position delta
	void CalcVertexPosDelta(int iVert, const Point3& vOrigin, const Matrix3& matTM);

	//	Get weight of specified frame
	float GetWeight(int iFrame) { return m_aWeights[iFrame]; }
	//	Get position delta
	const A3DVECTOR3& GetPosDelta(int n) { return m_aPosDeltas[n]; }
	//	Get channel name
	const char* GetName() { return m_strName; } 
	//	Get normal delta
	const A3DVECTOR3& GetNormalDelta(int n) { return m_aNormalDeltas[n]; }

	//	Get normal delta array
	AArray<A3DVECTOR3, A3DVECTOR3&>& GetPosDeltaArray() { return m_aPosDeltas; }
	AArray<A3DVECTOR3, A3DVECTOR3&>& GetNormalDeltaArray() { return m_aNormalDeltas; }

protected:	//	Attributes

	AString		m_strName;		//	Channel name
	MorphR3*	m_pMorphR3;		//	Morph modifier
	int			m_iChannel;		//	Channel index

	AArray<float, float>			m_aWeights;			//	Weights of every frame
	AArray<A3DVECTOR3, A3DVECTOR3&>	m_aPosDeltas;		//	Vertex position deltas
	AArray<A3DVECTOR3, A3DVECTOR3&>	m_aNormalDeltas;	//	Vertex normal deltas

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_MORPHCHANNEL_H_

