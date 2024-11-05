/*
* FILE: AFileSkillGfxEventMan.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/6/19
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifdef _ANGELICA31

#ifndef _AFileSkillGfxEventMan_H_
#define _AFileSkillGfxEventMan_H_

#include "A3DSkillGfxEventMan.h"

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

class A3DSkinModel;
class A3DSkillGfxEvent;

enum GfxMoveMode;
enum GfxHitPos;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AFileSkillGfxEventMan
//	
///////////////////////////////////////////////////////////////////////////

class AFileSkillGfxEventMan : public A3DSkillGfxEventMan
{

public:		//	Types

public:		//	Constructor and Destructor

	AFileSkillGfxEventMan(void);
	virtual ~AFileSkillGfxEventMan(void);

public:		//	Attributes

public:		//	Operations

	void SetHostTargetModel(A3DSkinModel* pHostModel
		, A3DSkinModel* pTargetModel);

	void SetHostTargetPos(const A3DVECTOR3& vHostPos
		, const A3DVECTOR3& vTargetPos);

protected:	//	Attributes

	A3DVECTOR3 m_host_pos;
	A3DVECTOR3 m_target_pos;

	A3DSkinModel* m_host_model;
	A3DSkinModel* m_target_model;

protected:	//	Operations

	bool GetPosition(A3DSkinModel* pModel
		, clientid_t nId
		, A3DVECTOR3& vPos
		, GfxHitPos HitPos
		, const char* szHook
		, bool bRelHook
		, const A3DVECTOR3* pOffset
		, const char* szHanger
		, bool bChildHook);

	//	Override interface from A3DSkillGfxEvent
	virtual A3DSkillGfxEvent* CreateOneEvent(GfxMoveMode mode);

	virtual bool GetPositionById(const GETPOSITIONBYID_PARAMS& param, A3DVECTOR3& vPos);
	virtual bool GetDirAndUpById(clientid_t nId
		, A3DVECTOR3& vDir
		, A3DVECTOR3& vUp);

	virtual float GetTargetScale(clientid_t nTargetId) { return 1.8f; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AFileSkillGfxEventMan_H_

#endif	//	_ANGELICA31
