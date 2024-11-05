/*
* FILE: AFileGfxInterface.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/08/26
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifdef _ANGELICA31

#ifndef _AFileGfxInterface_H_
#define _AFileGfxInterface_H_


#include "A3DGfxInterface.h"

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
//	Class AFileGfxInterface
//	
///////////////////////////////////////////////////////////////////////////

class AFileGfxInterface : public A3DGfxInterface
{

public:		//	Types

public:		//	Constructor and Destructor

	AFileGfxInterface(void);
	virtual ~AFileGfxInterface(void);

public:		//	Attributes

public:		//	Operations

	bool Init(A3DGfxEngine* pA3DGfxEngine);
	void Release();

	//	If the editor is Skill attack editor, then set this flag to true.
	void SetSkillAttackEditorFlag(bool bIsInSkillAttackEditor) {  m_bIsInSkillAttackEditor = bIsInSkillAttackEditor; }

	//	For Skill Gfx Event
	virtual A3DSkillGfxEventMan* GetSkillGfxEventMan() const;
	virtual A3DSkillGfxComposerMan* GetSkillGfxComposerMan() const;

protected:	//	Attributes

	A3DGfxEngine*					m_pA3DGfxEngine;
	bool							m_bIsInSkillAttackEditor;
	A3DSkillGfxEventMan*			m_pA3DSkillGfxEventMan;
	A3DSkillGfxComposerMan*			m_pA3DSkillGfxComposerMan;

protected:	//	Operations

	//	For SkinModel
	virtual A3DSkinModel* LoadA3DSkinModel(const char* szFile, int iSkinFlag) const;
	virtual void ReleaseA3DSkinModel(A3DSkinModel* pModel) const;

	virtual const char * GetECMHullPath() const { return NULL; }

	virtual bool GetModelUpdateFlag() const;
	virtual void SetA3DSkinModelSceneLightInfo(A3DSkinModel* pA3DSkinModel) const;
	virtual void ECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel) const;
	virtual float GetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm) const;
	virtual const A3DLIGHTPARAM& GetLightparam() const;
	virtual bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel) const;

	//	For Sound
	virtual AM3DSoundBuffer* LoadNonLoopSound(const char* szFile, int nPriority) const;
	virtual AM3DSoundBuffer* LoadLoopSound(const char* szFile) const;
	virtual void ReleaseSoundNonLoop(AM3DSoundBuffer*& pSound) const;
	virtual void ReleaseSoundLoop(AM3DSoundBuffer*& pSound) const;

	virtual AudioEngine::EventInstance* CreateAudioEventInstance(const char* szEventFullPath) const;
	virtual void ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const;

	//	For others
	virtual A3DCamera* GetA3DCamera() const;
	virtual float GetAverageFrameRate() const;
	virtual bool GetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount) const;

	virtual bool PlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets) const;
	virtual void SkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier, const ECMODEL_SHAKE* shake) const;
	virtual void SkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier) const;
	virtual bool SkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage, const ECMODEL_SHAKE* shake) const;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AFileGfxInterface_H_


#endif	//	_ANGELICA31
