/*
* FILE: AFileSkillGfxEventMan.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/6/19
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "AFileSkillGfxEventMan.h"

#ifdef _ANGELICA31

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
//	Implement AFileSkillGfxEventMan
//	
///////////////////////////////////////////////////////////////////////////

AFileSkillGfxEventMan::AFileSkillGfxEventMan(void)
: m_host_pos(0, 0, 0)
, m_target_pos(0, 0, 10.0f)
, m_host_model(NULL)
, m_target_model(NULL)
{	
}

AFileSkillGfxEventMan::~AFileSkillGfxEventMan(void)
{
}


void AFileSkillGfxEventMan::SetHostTargetModel(A3DSkinModel* pHostModel
												, A3DSkinModel* pTargetModel)
{
	m_host_model = pHostModel;
	m_target_model = pTargetModel;
}

void AFileSkillGfxEventMan::SetHostTargetPos(const A3DVECTOR3& vHostPos
											  , const A3DVECTOR3& vTargetPos)
{
	m_host_pos = vHostPos;
	m_target_pos = vTargetPos;
}

A3DSkillGfxEvent* AFileSkillGfxEventMan::CreateOneEvent(GfxMoveMode mode)
{
	return new A3DSkillGfxEvent(this, mode);
}

bool AFileSkillGfxEventMan::GetPosition(A3DSkinModel* pModel, clientid_t nId, A3DVECTOR3& vPos, GfxHitPos HitPos, const char* szHook, bool bRelHook, const A3DVECTOR3* pOffset, const char* szHanger, bool bChildHook)
{
	if (pModel == NULL)
		return false;

	if (szHook)
	{
		A3DSkeletonHook* pHook = NULL;

		if (bChildHook && szHanger)
		{
			A3DSkinModel* pChild = pModel->GetChildModelByName(szHanger);
			if (pChild)
				pHook = pChild->GetSkeletonHook(szHook, true);
		}
		else
			pHook = pModel->GetSkeletonHook(szHook, true);

		if (pHook)
		{
			if (bRelHook)
				vPos = pHook->GetAbsoluteTM() * (*pOffset);
			else
			{
				vPos = pModel->GetAbsoluteTM() * (*pOffset);
				vPos = vPos - pModel->GetAbsoluteTM().GetRow(3) + pHook->GetAbsoluteTM().GetRow(3);
			}

			return true;
		}
		else if (pOffset)
			vPos += *pOffset;			
	}

	return false;
}

bool AFileSkillGfxEventMan::GetPositionById(const GETPOSITIONBYID_PARAMS& param, A3DVECTOR3& vPos)
{
	if (param.m_nId == 1)
	{
		if (!GetPosition(m_host_model, param.m_nId, vPos, param.m_HitPos, param.m_szHook, param.m_bRelHook, param.m_pOffset, param.m_szHanger, param.m_bChildHook))
			vPos = m_host_pos;
		else
			return true;
	}
	else
	{
		if (!GetPosition(m_target_model, param.m_nId, vPos, param.m_HitPos, param.m_szHook, param.m_bRelHook, param.m_pOffset, param.m_szHanger, param.m_bChildHook))
			vPos = m_target_pos;
		else
			return true;
	}

	if (param.m_HitPos != enumHitBottom)
		vPos.y += .9f;

	if (param.m_pOffset)
		vPos += *param.m_pOffset;

	return true;
}

bool AFileSkillGfxEventMan::GetDirAndUpById(clientid_t nId, A3DVECTOR3& vDir, A3DVECTOR3& vUp)
{
	vDir.Set(0, 0, 1.0f);
	vUp.Set(0, 1.0f, 0);
	return true;
}
#endif