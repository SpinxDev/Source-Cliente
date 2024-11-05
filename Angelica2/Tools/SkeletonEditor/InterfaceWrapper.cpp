/*
 * FILE: InterfaceWrapper.cpp
 *
 * DESCRIPTION:
 *
 * CREATED BY: duyuxin, 2006/7/11
 *
 * HISTORY:
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#include "stdafx.h"
#include "InterfaceWrapper.h"

#include <A3DSkinModelAct.h>

#define new A_DEBUG_NEW

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
//	Implement CSkinModel
//
///////////////////////////////////////////////////////////////////////////

CSkinModel::CSkinModel()
{
	m_bOneStep	= false;
	m_fCurFrame	= -1.0f;
}

//	Update routine
bool CSkinModel::FrameUpdate(int iDeltaTime)
{
/*	//	test code...
	Update(m_bOneStep ? 0 : iDeltaTime);
	for (int i=0; i < m_aChildModels.GetSize(); i++)
		m_aChildModels[i]->Update(iDeltaTime);

	return true;
*/
	return Update(m_bOneStep ? 0 : iDeltaTime);
}

//	Step frame
void CSkinModel::StepFrame(bool bInc, const char* szAction)
{
	if (!m_pA3DSkeleton || !m_bOneStep)
		return;
	
	A3DSkinModelActionCore* pAction = GetAction(szAction);
	if (!pAction)
		return;

	float fStartFrame = pAction->GetStartFrame();
	float fEndFrame = pAction->GetEndFrame();
	float fFrame;

	if (m_fCurFrame < 0.0f || fStartFrame == fEndFrame)
	{
		fFrame = fStartFrame;
	}
	else
	{
		if (bInc)
			fFrame = m_fCurFrame + 1.0f;
		else
			fFrame = m_fCurFrame - 1.0f;

		if (fFrame > fEndFrame)
			fFrame = fStartFrame;
		
		if (fFrame < fStartFrame)
			fFrame = fEndFrame;
	}
	
	PlayActionByNameDC(szAction, 1, 0);
	int iAbsTime = (int)(1000.0f / m_pA3DSkeleton->GetAnimFPS() * (fFrame - fStartFrame) + 0.5f);
	Update(iAbsTime);

	m_fCurFrame = fFrame;
}

//	Step frame to specified frame
void CSkinModel::StepFrameTo(float fFrame, const char* szAction)
{
	if (!m_pA3DSkeleton || !m_bOneStep)
		return;
	
	A3DSkinModelActionCore* pAction = GetAction(szAction);
	if (!pAction)
		return;

	float fStartFrame = pAction->GetStartFrame();
	float fEndFrame = pAction->GetEndFrame();
	a_Clamp(fFrame, fStartFrame, fEndFrame);

	PlayActionByNameDC(szAction, 1, 0);
	int iAbsTime = (int)(1000.0f / m_pA3DSkeleton->GetAnimFPS() * (fFrame - fStartFrame) + 0.5f);
	Update(iAbsTime);

	m_fCurFrame = fFrame;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CSkeletonWrapper
//
///////////////////////////////////////////////////////////////////////////

//	Initialize object
bool CSkeletonWrapper::Init()
{
	A3DSkeleton::Init();
	return true;
}

//	Release object
void CSkeletonWrapper::Release()
{
	A3DSkeleton::Release();
}

//	Load skeleton data form file
bool CSkeletonWrapper::Load(const char* szFile)
{
	return A3DSkeleton::Load(szFile);
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CSkinWrapper
//
///////////////////////////////////////////////////////////////////////////

	//	Initialize object
bool CSkinWrapper::Init(A3DEngine* pA3DEngine)
{
	A3DSkin::Init(pA3DEngine);
	return true;
}

//	Release object
void CSkinWrapper::Release()
{
	A3DSkin::Release();
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CTrackSetWrapper
//
///////////////////////////////////////////////////////////////////////////

//	Create a empty bone track
CTrackSetWrapper::BONETRACK* CTrackSetWrapper::CreateBoneTrack(int id)
{
	//	Ensure track id collision
	BONETRACK* pTrack = GetBoneTrack(id);
	if (pTrack)
	{
		ASSERT(!pTrack);
		return false;
	}

	if (!(pTrack = new BONETRACK))
		return false;

	pTrack->pPosTrack	= NULL;
	pTrack->pRotTrack	= NULL;

	//	Add track to table
	m_TrackTab.put(id, pTrack);
	
	return pTrack;
}
