/*
 * FILE: MorphChannel.cpp
 *
 * DESCRIPTION: Morph channel class
 *
 * CREATED BY: duyuxin, 2003/11/7
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "MorphChannel.h"
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
//	Implement CMorphChannel
//
///////////////////////////////////////////////////////////////////////////

CMorphChannel::CMorphChannel() : m_aPosDeltas(2000, 500), m_aWeights(100, 100)
{
//	m_pMorphR3	= NULL;
	m_iChannel	= 0;
}

CMorphChannel::~CMorphChannel()
{
}
/*
//	Initialize object
bool CMorphChannel::Init(MorphR3* pMorphR3, int iChannel, const char* szName)
{
	m_pMorphR3	= pMorphR3;
	m_iChannel	= iChannel;
	m_strName	= szName;
	return true;
}
*/
//	Release object
void CMorphChannel::Release()
{
	m_aWeights.RemoveAll();
	m_aPosDeltas.RemoveAll();
	m_aNormalDeltas.RemoveAll();

//	m_pMorphR3 = NULL;
}

//	Add a vertex
void CMorphChannel::CalcVertexPosDelta(int iVert, const Point3& vOrigin, const Matrix3& matTM)
{
/*	morphChannel* pChannel = &m_pMorphR3->chanBank[m_iChannel];
	Point3 v = (vOrigin + pChannel->mDeltas[iVert] * 100.0f) * matTM;
	v = (v - vOrigin * matTM) * 0.01f;
	m_aPosDeltas.Add(A3DVECTOR3(v.x, v.z, v.y));
*/
}

//	Build channel weights
bool CMorphChannel::BuildChannelWeights(int iStartFrame, int iNumFrame, int iFPS)
{
/*	morphChannel* pChannel = &m_pMorphR3->chanBank[m_iChannel];

	for (int i=0; i < iNumFrame; i++)
	{
		TimeValue time = SecToTicks((iStartFrame + i) / (float)iFPS);
		float fWeight;
		pChannel->cblock->GetValue(0, time, fWeight, FOREVER);
		m_aWeights.Add(fWeight);
	}
*/
	return true;
}


