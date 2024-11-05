/*
 * FILE: A3DCombinedAction.h
 *
 * DESCRIPTION: Combined Action, with gfx and sfx
 *
 * CREATED BY: ZhangYu, 2004/9/18
 *
 * HISTORY:
 *
 */

#ifndef _A3DCOMBINEDACTION_H_
#define _A3DCOMBINEDACTION_H_

#include "A3DObject.h"
#include "A3DTypes.h"
#include "A3DGFXEx.h"
#include "AM3DSoundBuffer.h"
#include "AMSoundEngine.h"
#include "AString.h"
#include "AList2.h"
#include "A3DSkeleton.h"
#include "A3DGFXExMan.h"
#include "GfxCommonTypes.h"

#define		EVENT_TYPE_NONE		-1
#define		EVENT_TYPE_BASE		100
#define		EVENT_TYPE_GFX		(EVENT_TYPE_BASE + 0)
#define		EVENT_TYPE_SFX		(EVENT_TYPE_BASE + 1)
#define		EVENT_TYPE_CHLDACT	(EVENT_TYPE_BASE + 2)
#define		EVENT_TYPE_MATCHG	(EVENT_TYPE_BASE + 3)
#define		EVENT_TYPE_ATT_PT	(EVENT_TYPE_BASE + 4)
#define		EVENT_TYPE_SCRIPT	(EVENT_TYPE_BASE + 5)
#define		EVENT_TYPE_CAM_PT	(EVENT_TYPE_BASE + 6)
#define		EVENT_TYPE_MODELSCLCHG (EVENT_TYPE_BASE + 7)
#define		EVENT_TYPE_MATTRANS	(EVENT_TYPE_BASE + 8)
#define		EVENT_TYPE_AUDIOEVENT	(EVENT_TYPE_BASE + 9)

// when add new type, change this definition
#define		EVENT_TYPE_END		EVENT_TYPE_AUDIOEVENT

//	event enabling mask, the same order as event type
//	as the mask set, this type of event is enabled, while other types will be disabled
#define		EVENT_MASK_NONE		0				//	No event would be played
#define		EVENT_MASK_ALL		0xFFFFFFFF		//	all events be played
#define		EVENT_MASK_GFX		0x01			//	Only GFX			(1 << 0)
#define		EVENT_MASK_SFX		0x02			//	Only SFX			(1 << 1)
#define		EVENT_MASK_CHLDACT	0x04			//	Only ChildAct		(1 << 2)
#define		EVENT_MASK_MATCHG	0x08			//	Only MatChg			(1 << 3)
#define		EVENT_MASK_ATT_PT	0x10			//	Only ATT			(1 << 4)
#define		EVENT_MASK_SCRIPT	0x20			//	Only Script			(1 << 5)
#define		EVENT_MASK_CAM_PT	0x40			//	Only CamPt			(1 << 6)
#define		EVENT_MASK_CHLDSCLCHG 0x80			//	Only Child Scale Change (1 << 7)
#define		EVENT_MASK_MATTRANS 0x100			//	Only Mat Trans		(1 << 8)
#define		EVENT_MASK_AUDIOEVENT 0x200			//	Only AudioEvent		(1 << 9)

#define		VECTOR_XYZ(n) n.x,n.y,n.z
#define		VECTORADDR_XYZ(n) &n.x,&n.y,&n.z
#define		QUAT_XYZW(n) n.x,n.y,n.z,n.w
#define		QUATADDR_XYZW(n) &n.x,&n.y,&n.z,&n.w

enum ModParamDataType
{
	MOD_PARAM_NONE,
	MOD_PARAM_POS,
	MOD_PARAM_HOOK
};

struct ModActParamData
{
	ModParamDataType	m_Type;
	AString				m_strHook;
	bool				m_bOutCmd;

	ModActParamData(ModParamDataType nType = MOD_PARAM_NONE) 
		: m_Type(nType)
		, m_bOutCmd(false) 
	{

	}
	void Init();
	bool Load(AFile* pFile, DWORD dwVersion);
	bool Save(AFile* pFile);
};

inline void ModActParamData::Init()
{
	m_strHook.Empty();
}

inline ModParamDataType MapGFXType(const GfxValueType& GfxType)
{
	if (GfxType == GFX_VALUE_MATRIX4)
		return MOD_PARAM_HOOK;
	else if (GfxType == GFX_VALUE_VECTOR3)
		return MOD_PARAM_POS;
	return MOD_PARAM_NONE;
}

struct ModActParam
{
	ModActParam() : m_nParamId(-1) {}
	ModActParam(const char * szEleName, int nParamId, ModParamDataType nType)
		: m_strEleName(szEleName)
		, m_nParamId(nParamId)
		, m_ParamData(nType)
	{
		m_ParamData.Init();
	}
	ModActParam(const ModActParam& src)
	{
		m_strEleName	= src.m_strEleName;
		m_nParamId		= src.m_nParamId;
		m_ParamData		= src.m_ParamData;
	}

	AString			m_strEleName;
	int				m_nParamId;
	ModActParamData	m_ParamData;

	bool Load(AFile* pFile, DWORD dwVersion);
	bool Save(AFile* pFile);
};

typedef abase::vector<ModActParam*> ModActParamList;

class CECModel;
class AMSoundBufferMan;
class RandStringContainer;

class ACTION_INFO
{
public:
	ACTION_INFO() :
	m_dwStartTime(0),
	m_dwEndTime(0),
	m_dwSpan(0),
	//m_nLoops(1)
	m_nMinLoops(1),
	m_nMaxLoops(1)
	{
	}

	ACTION_INFO(const ACTION_INFO& src)
	{
		m_strName		= src.m_strName;
		m_dwStartTime	= src.m_dwStartTime;
		m_dwEndTime		= src.m_dwEndTime;
		m_dwSpan		= src.m_dwSpan;
	//	m_nLoops		= src.m_nLoops;
		m_nMinLoops		= src.m_nMinLoops;
		m_nMaxLoops		= src.m_nMaxLoops;
	}

protected:
	friend class A3DCombinedAction;

	AString		m_strName;
	DWORD		m_dwStartTime;
	DWORD		m_dwEndTime;
	DWORD		m_dwSpan;
	int			m_nMinLoops;
	int			m_nMaxLoops;

public:
	const AString& GetName() const { return m_strName; }
	void SetName(const AString& strName) { m_strName = strName; }
	DWORD GetStartTime() const { return m_dwStartTime; }
	void SetStartTime(DWORD dwStartTime) { m_dwStartTime = dwStartTime; m_dwEndTime = m_dwStartTime + m_dwSpan; }
	DWORD GetTimeSpan() const { return m_dwSpan; }
	void SetTimeSpan(DWORD dwTimeSpan) { m_dwSpan = dwTimeSpan; m_dwEndTime = m_dwStartTime + m_dwSpan; }
	DWORD GetEndTime() const { return m_dwEndTime; }
	int GetMinLoops() const { return m_nMinLoops; }
	int GetMaxLoops() const { return m_nMaxLoops; }
	void SetLoops(int nMinLoops, int nMaxLoops) { m_nMinLoops = nMinLoops; m_nMaxLoops = nMaxLoops; }
	bool IsInfinite() const { return m_nMinLoops == -1 || m_nMaxLoops == -1; }
	int CalcLoopNum() { return IsInfinite() ? -1 : a_Random(m_nMinLoops, m_nMaxLoops); }

	bool Load(AFile* pFile, DWORD dwVersion);
	bool Save(AFile* pFile);
};

typedef ACTION_INFO* PACTION_INFO;

class GFX_INFO;
class SFX_INFO;
class ChildActInfo;
class MaterialScaleChange;

class A3DCombActDynData;

class EVENT_INFO
{
public:
	EVENT_INFO(A3DCombinedAction* pAct) :
	m_dwStartTime(0),
	m_dwTimeSpan(-1),
	m_bOnce(false),
	m_pAct(pAct) {}
	virtual ~EVENT_INFO() {}

protected:
	int		m_nType;
	DWORD	m_dwStartTime;
	DWORD	m_dwTimeSpan;
	bool	m_bOnce;
	A3DCombinedAction* m_pAct;

public:
	DWORD GetStartTime() const { return m_dwStartTime; }
	void SetStartTime(DWORD dwStartTime) { m_dwStartTime = dwStartTime; }
	DWORD GetTimeSpan() const { return m_dwTimeSpan; }
	void SetTimeSpan(DWORD dwTimeSpan) { m_dwTimeSpan = dwTimeSpan; }
	virtual bool Load(AFile* pFile, DWORD dwVersion) = 0;
	virtual bool Save(AFile* pFile) = 0;
	virtual void Init(A3DDevice* pDev) {}
	virtual bool Start(A3DCombActDynData* pDynData) = 0;
	virtual void Resume() {}
	int GetType() const { return m_nType; }
	A3DCombinedAction* GetComAct() { return m_pAct; }
	static EVENT_INFO* LoadFromFile(A3DCombinedAction* pAct, AFile* pFile, DWORD dwVersion);
	static EVENT_INFO* CreateEventByType(A3DCombinedAction* pAct, int nType);
	bool LoadEventBase(AFile* pFile, DWORD dwVersion);
	void SaveEventBase(AFile* pFile);
	bool IsOnce() const { return m_bOnce; }
	void SetOnce(bool bOnce) { m_bOnce = bOnce; }
	virtual EVENT_INFO& Clone(const EVENT_INFO& src)
	{
		m_nType			= src.m_nType;
		m_dwStartTime	= src.m_dwStartTime;
		m_dwTimeSpan	= src.m_dwTimeSpan;
		m_bOnce			= src.m_bOnce;
		return *this;
	}
	static EVENT_INFO* CloneFrom(A3DCombinedAction* pAct, const EVENT_INFO& src)
	{
		return &CreateEventByType(pAct, src.m_nType)->Clone(src);
	}
};

class FX_BASE_INFO : public EVENT_INFO
{
public:

	FX_BASE_INFO(A3DCombinedAction* pAct);
	virtual ~FX_BASE_INFO();

protected:
	
	RandStringContainer* m_pFiles;
	AString		m_strHookName;
	A3DVECTOR3	m_vOffset;
	float		m_fYaw;
	float		m_fPitch;
	float		m_fRot;
	bool		m_bBindParent;
	bool		m_bModelAlpha;
	bool		m_bCustomFilePath;
	bool		m_bUseECMHook;
	A3DMATRIX4	m_matTran;
	DWORD		m_dwFadeOutTime;
	int			m_nCustomData;
public:
	
#ifdef GFX_EDITOR
	RandStringContainer* GetRandStringContainer() const { return m_pFiles; }
#endif

	int GetType() const { return m_nType; }
	const char* GetFilePath() const;
	void SetFilePath(const char* szPath);
	const char* GetRandFilePath() const;
	int GetFilePathCount() const;
	const char* GetFilePathByIndex(int iIdx) const;
	const AString& GetHookName() const { return m_strHookName; }
	void SetHookName(const AString& strName) { m_strHookName = strName; }
	bool IsUseECMHook() const { return m_bUseECMHook; }
	void SetUseECMHook(bool bUseECMHook) { m_bUseECMHook = bUseECMHook; }
	void SetOffset(const A3DVECTOR3& vOffset) { m_vOffset = vOffset; }
	A3DVECTOR3 GetOffset() const { return m_vOffset; }
	void SetYaw(float fYaw) { m_fYaw = fYaw; }
	float GetYaw() const { return m_fYaw; }
	void SetPitch(float fPitch) { m_fPitch = fPitch; }
	float GetPitch() const { return m_fPitch; }
	void SetRot(float fRot) { m_fRot = fRot; }
	float GetRot() const { return m_fRot; }
	bool CanFadeOut() const { return m_dwFadeOutTime != 0; }
	void SetFadeOut(bool bCan);
	DWORD GetFadeOutTime() const { return m_dwFadeOutTime; }
	void SetFadeOutTime(DWORD dwFadeOutTime) { m_dwFadeOutTime = dwFadeOutTime; }
	bool UseModelAlpha() const { return m_bModelAlpha; }
	void SetModelAlpha(bool bUse) { m_bModelAlpha = bUse; }
	bool IsCustomFilePath() const { return m_bCustomFilePath; }
	void SetCustomFilePath(bool b) { m_bCustomFilePath = b; }

	int GetCustomData() { return m_nCustomData; }
	void SetCustomData( int nData ) { m_nCustomData = nData; }

	void SetBindParent(bool bBindParent) { m_bBindParent = bBindParent; }
	bool IsBindParent() const { return m_bBindParent; }
	void BuildTranMat()
	{
		A3DQUATERNION q(m_fPitch, m_fYaw, m_fRot);
		q.ConvertToMatrix(m_matTran);
		m_matTran.SetRow(3, m_vOffset);
	}
	const A3DMATRIX4& GetMatTran() const { return m_matTran; }

	virtual bool Load(AFile* pFile, DWORD dwVersion) = 0;
	virtual bool Save(AFile* pFile) = 0;
	bool LoadBase(AFile* pFile, DWORD dwVersion);
	bool SaveBase(AFile* pFile);
	static FX_BASE_INFO* LoadFromFile(A3DCombinedAction* pAct, AFile* pFile, DWORD dwVersion);
	static FX_BASE_INFO* CreateFxInfo(A3DCombinedAction* pAct, int nType);
	virtual EVENT_INFO& Clone(const EVENT_INFO& src);
};

typedef FX_BASE_INFO* PFX_BASE_INFO;

class FX_BINDING_BASE
{
public:
	FX_BINDING_BASE(A3DCombActDynData* pDynData) :
	m_pInfo(0),
	m_pDynData(pDynData) {}
	virtual ~FX_BINDING_BASE() {}

protected:
	friend class A3DCombinedAction;
	EVENT_INFO*			m_pInfo;
	A3DCombActDynData*	m_pDynData;

public:
	virtual bool IsStop() const = 0;
	virtual void Stop() const {}
	virtual void Render(A3DViewport* pView) {};
	virtual void UpdateParam(CECModel* pECModel, int nDeltaTime) {}
	void SetInfo(EVENT_INFO* pInfo) { m_pInfo = pInfo; }
	const EVENT_INFO* GetInfo() const { return m_pInfo; }
	A3DCombActDynData* GetDynData() { return m_pDynData; }
};

typedef FX_BINDING_BASE* PFX_BINDING_BASE;

class GFX_BINDING : public FX_BINDING_BASE
{
public:
	A3DGFXEx*	m_pGfx;
	bool		m_bNeedSetTM;
	bool		m_bInitTMUpdated;	// is the first time a delay update gfx to be set to place
	DWORD		m_dwTimeSpan;

	GFX_BINDING(A3DCombActDynData* pDynData);
	~GFX_BINDING();

	virtual void Stop() const { m_pGfx->Stop(); }
	virtual bool IsStop() const { return m_pGfx->GetState() == ST_STOP; }
	virtual void Render(A3DViewport* pView) { AfxGetGFXExMan()->RegisterGfx(m_pGfx); }
	virtual void UpdateParam(CECModel* pECModel, int nDeltaTime);
private:
	A3DMATRIX4 CalcGfxParentTM(A3DSkeletonHook* pHook, CECModel* pECModel);
	void UpdateGfxParentTM(A3DSkeletonHook* pHook, CECModel* pECModel, int iDeltaTime);
};

typedef GFX_BINDING* PGFX_BINDING;

class GFX_INFO : public FX_BASE_INFO
{
public:

	GFX_INFO(A3DCombinedAction* pAct);
	virtual ~GFX_INFO();

protected:
	A3DDevice*		m_pDev;
	A3DGFXEx*		m_pGfx;
	float			m_fScale;
	float			m_fAlpha;
	float			m_fPlaySpeed;
	bool			m_bUseOuterPath;
	bool			m_bRelativeToECModel;
	int				m_iDelayTime;			// delay time from current position to the dest position
	ModActParamList	m_ParamList;
	bool			m_bRotWithModel;		//if cogfx rotate with model

public:
	virtual void Resume() {}
	int GetDelayTime() const { return m_iDelayTime; }
	void SetDelayTime(int iDelayTime) { m_iDelayTime = iDelayTime; }
	float GetScale() const { return m_fScale; }
	void SetScale(float fScale) { m_fScale = fScale; }
	float GetAlpha() const { return m_fAlpha; }
	void SetAlpha(float fAlpha) { m_fAlpha = fAlpha; }
	float GetPlaySpeed() const { return m_fPlaySpeed; }
	void SetPlaySpeed(float fSpeed) { m_fPlaySpeed = fSpeed; }
	bool IsUseOuterPath() const { return m_bUseOuterPath; }
	void SetUseOuterPath(bool b) { m_bUseOuterPath = b; }
	bool RelativeToECModel() const { return m_bRelativeToECModel; }
	void SetRelativeToECModel(bool b) { m_bRelativeToECModel = b; }
	ModActParamList& GetParamList() { return m_ParamList; }
	const ModActParamList& GetParamList() const { return m_ParamList; }
	bool IsRotWithModel() const { return m_bRotWithModel; }
	void SetRotWithModel(bool bRot) { m_bRotWithModel = bRot; }
	ModActParam* GetParam(const AString& strEle, int nParamId)
	{
		for (size_t i = 0; i < m_ParamList.size(); i++)
		{
			if (strEle == m_ParamList[i]->m_strEleName
			 && nParamId == m_ParamList[i]->m_nParamId)
			 return m_ParamList[i];
		}
		return NULL;
	}
	void CheckBindParam(GFX_BINDING* pBind)
	{
		size_t i = 0;
		while (i < m_ParamList.size())
		{
			ModActParam* pParam = m_ParamList[i];
			if (!pBind->m_pGfx->IsParamEnable(pParam->m_strEleName, pParam->m_nParamId))
			{
				m_ParamList.erase(m_ParamList.begin()+i);
				delete pParam;
				continue;
			}
			i++;
		}
	}
	virtual bool Start(A3DCombActDynData* pDynData);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual void Init(A3DDevice* pDev) { m_pDev = pDev; }
	A3DGFXEx* GetGfx() { return m_pGfx; }
	void LoadGfx();
	virtual EVENT_INFO& Clone(const EVENT_INFO& src);
};

typedef GFX_INFO* PGFX_INFO;

class SFX_INFO : public FX_BASE_INFO
{
public:

	SFX_INFO(A3DCombinedAction* pAct);
	virtual ~SFX_INFO() {}

protected:

	AMSoundEngine*	m_pAMEngine;
	A3DDevice*		m_pDev;
	GfxSoundParamInfo m_SoundParamInfo;
	AString			m_strLastSoundFile;
	int				m_iLastSoundCount;

public:
	
	GfxSoundParamInfo& GetSoundParamInfo() { return m_SoundParamInfo; }
	const GfxSoundParamInfo& GetSoundParamInfo() const { return m_SoundParamInfo; }
	virtual bool Start(A3DCombActDynData* pDynData);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	void Init(A3DDevice* pDev);
	AMSoundEngine* GetAMEng() { return m_pAMEngine; }
	virtual EVENT_INFO& Clone(const EVENT_INFO& src);

private:
	void CheckRandamSoundCount(AString& strFile);
	void UpdateLastSound(const AString& strFile);
};

typedef SFX_INFO* PSFX_INFO;

class SFX_BINDING : public FX_BINDING_BASE
{
public:
	AM3DSoundBuffer*	m_pSfx;

	SFX_BINDING(A3DCombActDynData* pDynData) : FX_BINDING_BASE(pDynData), m_pSfx(0) {}
	~SFX_BINDING();

	virtual bool IsStop() const { return !m_pSfx->IsPlaying(); }
	virtual void UpdateParam(CECModel* pECModel, int nDeltaTime);
};

typedef SFX_BINDING* PSFX_BINDING;

///////////////////////////////////////////////////////////////////////////
//	
//	Class AUDIOEVENT_INFO
//	
///////////////////////////////////////////////////////////////////////////

class AUDIOEVENT_INFO 
	: public FX_BASE_INFO
{
public:
	AUDIOEVENT_INFO(A3DCombinedAction* pAct);
	virtual ~AUDIOEVENT_INFO() {}

protected:

	float m_fMinDist;
	float m_fMaxDist;
	float m_fVolume;
	bool m_bUseCustom;

public:

	virtual bool Start(A3DCombActDynData* pDynData);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual EVENT_INFO& Clone(const EVENT_INFO& src);
	virtual bool IsAudioInfo() const { return true; }

	void SetVolume(float fVolume);
	float GetVolume() const { return m_fVolume; }
	void SetMinMaxDist(float fMinDist, float fMaxDist);
	float GetMinDist() const { return m_fMinDist; }
	float GetMaxDist() const { return m_fMaxDist; }
	bool GetUseCustom() const { return m_bUseCustom; }
	void SetUseCustom(bool b) { m_bUseCustom = b; }

};

typedef AUDIOEVENT_INFO* PAUDIOEVENT_INFO;

class ChildActInfo : public EVENT_INFO
{
public:
	ChildActInfo(A3DCombinedAction* pAct)
	: EVENT_INFO(pAct),
	m_dwTransTime(200),
	m_bIsTrail(false),
	m_dwTrailTmSpan(0),
	m_dwSegCount(0),
	m_avPos(NULL),
	m_aqDir(NULL) { m_nType = EVENT_TYPE_CHLDACT; }

	~ChildActInfo()
	{
		if (m_avPos) delete[] m_avPos;
		if (m_aqDir) delete[] m_aqDir;
	}

protected:
	AString		m_strActName;
	AString		m_strHHName;
	DWORD		m_dwTransTime;
	bool		m_bIsTrail;
	DWORD		m_dwTrailTmSpan;
	DWORD		m_dwSegCount;
	A3DVECTOR3*	m_avPos;
	A3DQUATERNION* m_aqDir;

public:
	const AString& GetActName() const { return m_strActName; }
	void SetActName(const char* szName) { m_strActName = szName; };
	const AString& GetHHName() const { return m_strHHName; }
	void SetHHName(const char* szName) { m_strHHName = szName; }
	DWORD GetTransTime() const { return m_dwTransTime; }
	void SetTransTime(DWORD t) { m_dwTransTime = t; }
	bool IsTrailAct() const { return m_bIsTrail; }
	void SetIsTrailAct(bool bIsTrail) { m_bIsTrail = bIsTrail; }
	DWORD GetTrailTmSpan() const { return m_dwTrailTmSpan; }
	void SetTrailTmSpan(DWORD dwSpan) { m_dwTrailTmSpan = dwSpan; }
	bool CalcTrailData(CECModel* pECModel);
	bool GetTraiMatTran(DWORD dwTimeSpan, A3DMATRIX4& matTran) const;
	bool Load(AFile* pFile, DWORD dwVersion);
	bool Save(AFile* pFile);
	virtual bool Start(A3DCombActDynData* pDynData);
	virtual EVENT_INFO& Clone(const EVENT_INFO& src)
	{
		EVENT_INFO::Clone(src);
		const ChildActInfo* pSrc = static_cast<const ChildActInfo*>(&src);

		m_strActName	= pSrc->m_strActName;
		m_strHHName		= pSrc->m_strHHName;
		m_dwTransTime	= pSrc->m_dwTransTime;
		m_bIsTrail		= pSrc->m_bIsTrail;
		m_dwTrailTmSpan	= pSrc->m_dwTrailTmSpan;
		m_dwSegCount	= pSrc->m_dwSegCount;

		if (m_dwSegCount)
		{
			m_avPos = new A3DVECTOR3[m_dwSegCount];
			m_aqDir	= new A3DQUATERNION[m_dwSegCount];

			for (DWORD i = 0; i < m_dwSegCount; i++)
			{
				m_avPos[i]	= pSrc->m_avPos[i];
				m_aqDir[i]	= pSrc->m_aqDir[i];
			}
		}

		return *this;
	}
};

class MaterialScaleChange : public EVENT_INFO
{
public:
	MaterialScaleChange(A3DCombinedAction* pAct) :
	EVENT_INFO(pAct),
	m_ScaleOrg(1.0f),
	m_ScaleDelta(0.0f),
	m_ScaleMin(0.0f),
	m_ScaleMax(4.f),
	m_bApplyToChild(false)
	{ m_nType = EVENT_TYPE_MATCHG; }

protected:
	A3DCOLORVALUE	m_ScaleOrg;
	A3DCOLORVALUE	m_ScaleDelta;
	A3DCOLORVALUE	m_ScaleMin;
	A3DCOLORVALUE	m_ScaleMax;
	bool			m_bApplyToChild;

public:
	void SetOrg(const A3DCOLORVALUE& src) { m_ScaleOrg = src; }
	const A3DCOLORVALUE& GetOrg() const { return m_ScaleOrg; }
	void SetDelta(const A3DCOLORVALUE& src) { m_ScaleDelta = src; }
	const A3DCOLORVALUE& GetDelta() const { return m_ScaleDelta; }
	void SetMin(const A3DCOLORVALUE& src) { m_ScaleMin = src; }
	const A3DCOLORVALUE& GetMin() const { return m_ScaleMin; }
	void SetMax(const A3DCOLORVALUE& src) { m_ScaleMax = src; }
	const A3DCOLORVALUE& GetMax() const { return m_ScaleMax; }
	void SetApplyToChild(bool bApply) { m_bApplyToChild = bApply; }
	bool GetApplyToChild() const { return m_bApplyToChild; }
	bool Load(AFile* pFile, DWORD dwVersion);
	bool Save(AFile* pFile);
	virtual bool Start(A3DCombActDynData* pDynData);
	virtual EVENT_INFO& Clone(const EVENT_INFO& src)
	{
		EVENT_INFO::Clone(src);
		const MaterialScaleChange* pSrc = static_cast<const MaterialScaleChange*>(&src);

		m_ScaleOrg		= pSrc->m_ScaleOrg;
		m_ScaleDelta	= pSrc->m_ScaleDelta;
		m_ScaleMin		= pSrc->m_ScaleMin;
		m_ScaleMax		= pSrc->m_ScaleMax;
		m_bApplyToChild	= pSrc->m_bApplyToChild;

		return *this;
	}
};

class MATCHG_BINDING : public FX_BINDING_BASE
{
public:
	MATCHG_BINDING(A3DCombActDynData* pDynData) : FX_BINDING_BASE(pDynData), m_bStopped(false) {}
	~MATCHG_BINDING() {}

protected:
	bool m_bStopped;
	A3DCOLORVALUE m_ScaleCur;

public:
	A3DCOLORVALUE& GetScaleCur() { return m_ScaleCur; }
	virtual bool IsStop() const { return m_bStopped; }
	virtual void UpdateParam(CECModel* pECModel, int nDeltaTime);
};

class SGCAttackPointMark : public EVENT_INFO
{
public:
	SGCAttackPointMark(A3DCombinedAction* pAct) 
		: EVENT_INFO(pAct)
		, m_nDivisions(1)
		, m_bIsUseAttDelay(false)
		, m_pAction(pAct)
		, m_nAttackOrientation(0)
	{
		m_nType = EVENT_TYPE_ATT_PT;
		memset(m_nDelay, 0, DELAY_TYPE_NUM * sizeof(int));
	}

	virtual ~SGCAttackPointMark() {}

protected:
	
	AString m_strAtkFile;
	int m_nDivisions;
	bool m_bIsUseAttDelay;
	enum { DELAY_TYPE_NUM = 2 };
	int m_nDelay[DELAY_TYPE_NUM];
	int m_nAttackOrientation;		// some logical definition used by client program
	
	A3DCombinedAction*		m_pAction;

public:
	const char* GetAtkFile() const { return m_strAtkFile; }
	void SetAtkFile(const char* szPath) { m_strAtkFile = szPath; }
	int GetDivisions() const { return m_nDivisions; }
	void SetDivisions(int nDivisions) { m_nDivisions = nDivisions; }
	int GetDelayTypeCount() const { return DELAY_TYPE_NUM; }
	int GetDelay(int nIdx) const { ASSERT(nIdx >= 0 && nIdx < DELAY_TYPE_NUM); return m_nDelay[nIdx]; }
	void SetDelay(int nIdx, int nMiliSeconds) { m_nDelay[nIdx] = nMiliSeconds; }
	bool GetUseAttDelay() const { return m_bIsUseAttDelay; }
	void SetUseAttDelay(bool bIsUseAttDelay) { m_bIsUseAttDelay = bIsUseAttDelay; }
	int GetAttOrientation() const { return m_nAttackOrientation; }
	void SetAttOrientation(int attOrient) { m_nAttackOrientation = attOrient; }
	bool Load(AFile* pFile, DWORD dwVersion);
	bool Save(AFile* pFile);
	virtual bool Start(A3DCombActDynData* pDynData);
	virtual EVENT_INFO& Clone(const EVENT_INFO& src)
	{
		EVENT_INFO::Clone(src);
		const SGCAttackPointMark* pSrc = static_cast<const SGCAttackPointMark*>(&src);
		m_strAtkFile = pSrc->m_strAtkFile;
		m_bIsUseAttDelay = pSrc->m_bIsUseAttDelay;
		m_nAttackOrientation = pSrc->m_nAttackOrientation;
		memcpy(m_nDelay, pSrc->m_nDelay, sizeof(m_nDelay));

		return *this;
	}
};

class GfxScriptEvent : public EVENT_INFO
{
public:
	GfxScriptEvent(A3DCombinedAction* pAct, int subType=1) 
		: EVENT_INFO(pAct), m_bInit(false), m_bConfigState(false), m_subType(subType)
	{
		m_nType = EVENT_TYPE_SCRIPT;
	}

	virtual ~GfxScriptEvent() {}

protected:

	bool m_bInit;
	AString m_strScript;
	AString m_strFuncName;
	bool m_bConfigState;
	int m_subType;

public:

	bool Load(AFile* pFile, DWORD dwVersion);
	bool Save(AFile* pFile);
	virtual bool Start(A3DCombActDynData* pDynData);
	const AString& GetScript() const { return m_strScript; }
	void SetScript(const char * szScript) { m_strScript = szScript; }
	bool IsConfigState() const { return m_bConfigState; }
	void SetConfigState(bool b) { m_bConfigState = b; }
	int GetSubType() const { return m_subType; }
	void SetSubType(int subType) { m_subType = subType; }

	virtual EVENT_INFO& Clone(const EVENT_INFO& src)
	{
		EVENT_INFO::Clone(src);
		m_strScript = static_cast<const GfxScriptEvent*>(&src)->m_strScript;
		m_bConfigState = static_cast<const GfxScriptEvent*>(&src)->m_bConfigState;
		m_subType = static_cast<const GfxScriptEvent*>(&src)->m_subType;
		return *this;
	}

#ifdef GFX_EDITOR

	//	Update script function to lua (remove if exist, then add)
	bool UpdateScript();

#endif
	
protected:

	//	help function for this class
	void GenerateFakeScriptParams(abase::vector<AString>& vargs);
	

};

class CameraControlEvent : public EVENT_INFO
{
public:		//	Types

	//	Only used for editor and save / load
	struct CameraControlParam
	{
		CameraControlParam();
		~CameraControlParam();
		
		bool Load(AFile* pFile, DWORD dwVersion);
		bool Save(AFile* pFile);
		
		float m_fCameraAngleAcc;		//	Camera's angle acceleration (degree / second^2)
		float m_fCameraLinearAcc;		//	Camera's linear acceleration (meter / second^2)

		bool m_bIsInterp;				//	This key is taken into consider when interpolating

		A3DVECTOR3 m_vTarget;			//	Target position relative host
		A3DVECTOR3 m_vCamera;			//	Camera position relative host
		
		void RemoveAllVerts() { m_VertArray.clear(); }
		void RemoveLastVert() { m_VertArray.pop_back(); }
		void RemoveVertAt(int nIndex) { m_VertArray.erase(m_VertArray.begin() + nIndex); }
		void AddVert(const A3DVECTOR3& vec) { m_VertArray.push_back(vec); }
		void AddVertAt(const A3DVECTOR3& vec, int nIndex) { m_VertArray.insert(m_VertArray.begin() + nIndex, vec); }
		size_t GetVertCount() const { return m_VertArray.size(); }
		A3DVECTOR3& GetVert(size_t uIndex) { return m_VertArray[uIndex]; }
		const A3DVECTOR3& GetVert(size_t uIndex) const { return m_VertArray[uIndex]; }
	
	private:
		abase::vector<A3DVECTOR3> m_VertArray;	//	Vertex that is used to calculate bezier point
	};

public:		//	Constructors / Destructor

	CameraControlEvent(A3DCombinedAction* pAct);
	virtual ~CameraControlEvent();

public:		//	Public operations

	void SetCameraControlParam(const CameraControlParam* pParam);
	const CameraControlParam* GetCameraControlParam() const { return &m_Param; }
	CameraControlParam* GetCameraControlParam() { return &m_Param; }

protected:	//	Protected attributes

	CameraControlParam m_Param;

protected:	//	Protected operations

	//	Overload functions
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual EVENT_INFO& Clone(const EVENT_INFO& src);
	
	//	Do nothing here
	virtual bool Start(A3DCombActDynData* pDynData) { return true; }
};


class BoneScaleChange : public EVENT_INFO
{
public:

	enum ScaleDestValue
	{
		SCALE_DEST0 = 0,
		SCALE_DEST1,
		SCALE_DEST2,
		SCALE_DEST3,
		SCALE_DEST4,
		SCALE_DEST_NUM,
	};
	
	struct ScaleDestParam
	{
		float m_fDestScale;			//	Scale destination value
		float m_fScaleFactor;		//	Scale factor
		DWORD m_dwScaleTime;		//	Scale time

		bool Save(AFile* pFile);
		bool Load(AFile* pFile, DWORD dwVersion);
	};

	struct BoneScaleParam
	{
		AString m_strBone;
		int m_iScaleType;			//	Scale type (Whole / Local Length / Local Thick)
		float m_fStartScale;		//	Scale start value
		int m_iDestNum;				//	Scale destination value number (less than SCALE_DEST_NUM)
		ScaleDestParam m_aScaleArray[SCALE_DEST_NUM];	// Scale destination parameters

		bool Save(AFile* pFile);
		bool Load(AFile* pFile, DWORD dwVersion);
	};

public:

	BoneScaleChange(A3DCombinedAction* pAct);
	virtual ~BoneScaleChange();

	bool AddBoneScaleParam(const BoneScaleParam& boneSclParam);
	void RemoveBoneScaleParam(const char* szName);
	const APtrArray<BoneScaleParam*>& GetBoneScaleParams() const { return m_aBoneScaleChanges; }
	BoneScaleParam* GetBoneScaleParam(const char* szName, int* piIndex = NULL) const;
	bool IsUseFileScale() const { return m_bUseFileScale; }
	void SetUseFileScale(bool useFileScale) { m_bUseFileScale = useFileScale; }

protected:

	APtrArray<BoneScaleParam*>	m_aBoneScaleChanges;
	bool	m_bUseFileScale;

protected:	//	Protected operations

	//	Overload functions
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual EVENT_INFO& Clone(const EVENT_INFO& src);
	virtual bool Start(A3DCombActDynData* pDynData);

	void RemoveAllScaleParams();

private:

	BoneScaleChange(const BoneScaleChange&);
	BoneScaleChange& operator = (const BoneScaleChange&);
};

class MaterialScaleTrans : public EVENT_INFO
{
public:
	enum { COLOR_TBL_MAX_NUM = 10 };
	enum { COLOR_LOOKUP_TBL_LEN = 128 };

	friend class MATTRANS_BINDING;

public:

	MaterialScaleTrans(A3DCombinedAction* pAct);
	~MaterialScaleTrans();

public:

	A3DCOLOR GetOrgColor() const { return m_clOrg; }
	void SetOrgColor(A3DCOLOR dwCol) { m_clOrg = dwCol; }
	int GetDestNum() const { return m_nDestNum; }
	void SetDestNum(int iDestNum);
	A3DCOLOR GetDestColor(int iIdx) const { return m_clDest[iIdx]; }
	void SetDestColor(int iIdx, A3DCOLOR dwColor);
	float GetTransTime(int iIdx) const { return m_fTransTime[iIdx]; }
	void SetTransTime(int iIdx, float fTransTime);
	bool GetApplyToChild() const { return m_bApplyToChild; }
	void SetApplyToChild(bool bApplyToChild) { m_bApplyToChild = bApplyToChild; }

	void CalcLookupTbl();
	A3DCOLOR GetCurrentColor(int nSeg, float fPortion) const;
protected:

	A3DCOLOR	m_clOrg;
	int			m_nDestNum;
	A3DCOLOR	m_clDest[COLOR_TBL_MAX_NUM];
	float		m_fTransTime[COLOR_TBL_MAX_NUM];
	float		m_fTotalSpan;
	bool		m_bApplyToChild;

protected:

	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual bool Start(A3DCombActDynData* pDynData);
	virtual EVENT_INFO& Clone(const EVENT_INFO& src);

};

typedef abase::vector<PFX_BINDING_BASE> FxBindBaseList;
typedef APtrList<EVENT_INFO*> EventInfoList;
typedef abase::hash_map<AString, EVENT_INFO*> EventInfoMap;
typedef APtrList<PACTION_INFO> ActInfoList;
typedef APtrList<SGCAttackPointMark*> SGCAttackPointList;

class A3DCombinedAction : public A3DObject
{
public:

	A3DCombinedAction();
	virtual ~A3DCombinedAction() { Release(); }

protected:

	int					m_nLoops;
	ActInfoList			m_ActLst;
	EventInfoList		m_EventInfoLst;
	//SGCAttackPointList	m_AttackPtLst;
	// Original, this property describes the comact's span time, now the span time may change randomly, so this property only define the min span time
	// And rename it from m_dwComActSpan -> m_dwComActMinSpan
	DWORD				m_dwComActMinSpan;
	bool				m_bInfinite;
	BYTE				m_Ranks[A3DSkinModel::ACTCHA_MAX];
	int					m_aEventCounter[EVENT_TYPE_END - EVENT_TYPE_BASE + 1];
	int					m_nEventChannel;
	float				m_fPlaySpeed;
	bool				m_bResetMaterialScale;		// 动作停止时是否重置颜色改变状态 (默认为true)
	bool				m_bStopChildrenAct;			// 动作停止时停止子模型动作 (默认为false)

	friend class A3DCombActDynData;

public:

	// Base Act Part
	int GetBaseActCount() const { return m_ActLst.GetCount(); }
	PACTION_INFO GetBaseAct(int nIndex) { return m_ActLst.GetByIndex(nIndex); }

	PACTION_INFO AddOneAct(const AString& strName, DWORD dwStartTime, DWORD dwTimeSpan)
	{
		PACTION_INFO pInfo = new ACTION_INFO;
		pInfo->SetName(strName);
		pInfo->SetStartTime(dwStartTime);
		pInfo->SetTimeSpan(dwTimeSpan);

		ALISTPOSITION pos = m_ActLst.GetHeadPosition();
		while (pos)
		{
			ALISTPOSITION posTemp = pos;
			PACTION_INFO pNext = m_ActLst.GetNext(pos);
			if (dwStartTime <= pNext->GetStartTime())
			{
				m_ActLst.InsertBefore(posTemp, pInfo);
				return pInfo;
			}
		}
		m_ActLst.AddTail(pInfo);
		return pInfo;
	}

	void RemoveBaseAct(PACTION_INFO pInfo)
	{
		ALISTPOSITION pos = m_ActLst.GetHeadPosition();
		while (pos)
		{
			if (m_ActLst.GetAt(pos) == pInfo)
			{
				m_ActLst.RemoveAt(pos);
				break;
			}
			m_ActLst.GetNext(pos);
		}
		delete pInfo;
	}

	int GetEventCount() const { return m_EventInfoLst.GetCount(); }

	void AddOneEvent(EVENT_INFO* pInfo)
	{
		ALISTPOSITION pos = m_EventInfoLst.GetHeadPosition();

		while (pos)
		{
			ALISTPOSITION posTemp = pos;
			EVENT_INFO* pNext = m_EventInfoLst.GetNext(pos);

			if (pInfo->GetStartTime() < pNext->GetStartTime())
			{
				m_EventInfoLst.InsertBefore(posTemp, pInfo);
				++m_aEventCounter[pInfo->GetType() - EVENT_TYPE_BASE];
				return;
			}
		}

		m_EventInfoLst.AddTail(pInfo);
		++m_aEventCounter[pInfo->GetType() - EVENT_TYPE_BASE];
	}

	EVENT_INFO* GetEventInfo(int nIndex) { return m_EventInfoLst.GetByIndex(nIndex); }

	void RemoveEvent(EVENT_INFO* pInfo)
	{
		ALISTPOSITION pos = m_EventInfoLst.GetHeadPosition();

		while (pos)
		{
			if (m_EventInfoLst.GetAt(pos) == pInfo)
			{
				m_EventInfoLst.RemoveAt(pos);
				break;
			}

			m_EventInfoLst.GetNext(pos);
		}

		--m_aEventCounter[pInfo->GetType() - EVENT_TYPE_BASE];
		delete pInfo;	
	}

	void RemoveAllEventInfo()
	{
		ALISTPOSITION pos = m_EventInfoLst.GetHeadPosition();

		while (pos)
		{
			delete m_EventInfoLst.GetAt(pos);
			m_EventInfoLst.GetNext(pos);
		}

		m_EventInfoLst.RemoveAll();
		memset(m_aEventCounter, 0, sizeof(m_aEventCounter));
	}

	void RemoveEventInfoByType(int nType)
	{
		ALISTPOSITION pos = m_EventInfoLst.GetHeadPosition();

		while (pos)
		{
			EVENT_INFO* pInfo = m_EventInfoLst.GetAt(pos);

			if (pInfo->GetType() == nType)
			{
				ALISTPOSITION posCur = pos;
				m_EventInfoLst.GetNext(pos);
				m_EventInfoLst.RemoveAt(posCur);
				--m_aEventCounter[pInfo->GetType() - EVENT_TYPE_BASE];
				delete pInfo;
			}
			else
				m_EventInfoLst.GetNext(pos);
		}
	}

	int GetEventCountByType(int iEventType) const
	{
		ASSERT(iEventType >= EVENT_TYPE_BASE && iEventType <= EVENT_TYPE_END);
		return m_aEventCounter[iEventType- EVENT_TYPE_BASE];
	}

	void SortActList()
	{
		ALISTPOSITION pos1 = m_ActLst.GetHeadPosition();
		while (pos1 != m_ActLst.GetTailPosition())
		{
			ALISTPOSITION pos2 = m_ActLst.GetTailPosition();
			while (pos2 != pos1)
			{
				ALISTPOSITION posTemp = pos2;
				PACTION_INFO pInfo2 = m_ActLst.GetPrev(pos2);
				PACTION_INFO pInfo1 = m_ActLst.GetAt(pos2);
				if (pInfo2->GetStartTime() < pInfo1->GetStartTime())
				{
					m_ActLst.SetAt(posTemp, pInfo1);
					m_ActLst.SetAt(pos2, pInfo2);
				}
			}
			m_ActLst.GetNext(pos1);
		}
	}

	void SortEventInfoList()
	{
		ALISTPOSITION pos1 = m_EventInfoLst.GetHeadPosition();
		while (pos1 != m_EventInfoLst.GetTailPosition())
		{
			ALISTPOSITION pos2 = m_EventInfoLst.GetTailPosition();
			while (pos2 != pos1)
			{
				ALISTPOSITION posTemp = pos2;
				EVENT_INFO* pInfo2 = m_EventInfoLst.GetPrev(pos2);
				EVENT_INFO* pInfo1 = m_EventInfoLst.GetAt(pos2);
				if (pInfo2->GetStartTime() < pInfo1->GetStartTime())
				{
					m_EventInfoLst.SetAt(posTemp, pInfo1);
					m_EventInfoLst.SetAt(pos2, pInfo2);
				}
			}
			m_EventInfoLst.GetNext(pos1);
		}
	}

	int GetEventIndexOfType(EVENT_INFO* pEvent, int nType) const
	{
		ALISTPOSITION pos = m_EventInfoLst.GetHeadPosition();
		int idx = 0;
		while (pos)
		{
			EVENT_INFO* pEventInfo = m_EventInfoLst.GetNext(pos);
			if (pEventInfo->GetType() == nType)
			{
				if (pEventInfo == pEvent)
				{
					return idx;
				}
				++idx;
			}
		}

		return -1;
	}

	void Release()
	{
		ALISTPOSITION pos = m_ActLst.GetHeadPosition();
		while (pos)	delete m_ActLst.GetNext(pos);
		m_ActLst.RemoveAll();
		pos = m_EventInfoLst.GetHeadPosition();
		while (pos) delete m_EventInfoLst.GetNext(pos);
		m_EventInfoLst.RemoveAll();
	}

	bool Load(A3DDevice* pDev, AFile* pFile, DWORD dwVersion);
	bool Save(AFile* pFile);
	A3DCombinedAction& operator = (const A3DCombinedAction& src);
	
	int GetLoops() const { return m_nLoops; }
	void SetLoops(int nLoops) { m_nLoops = nLoops; }

	DWORD GetMinComActTimeSpan() const { return m_dwComActMinSpan; }
	void CalcMinComActTimeSpan()
	{
		m_dwComActMinSpan = 0;
		ALISTPOSITION pos = m_ActLst.GetHeadPosition();

		while (pos)
		{
			PACTION_INFO pInfo = m_ActLst.GetNext(pos);

			if (!pInfo->IsInfinite())
				m_dwComActMinSpan += pInfo->GetTimeSpan() * pInfo->GetMinLoops();
			else
				m_dwComActMinSpan += pInfo->GetTimeSpan();
		}
	}

	bool IsInfinite() const { return m_bInfinite; }
	void SetRank(int channel, BYTE rank) { m_Ranks[channel] = rank; }
	BYTE GetRank(int channel) const { return m_Ranks[channel]; }
	void SetEventChannel(int channel) { m_nEventChannel = channel; }
	int GetEventChannel() const { return m_nEventChannel; }
	void SetPlaySpeed(float fPlaySpeed) { m_fPlaySpeed = fPlaySpeed; }
	float GetPlaySpeed() const { return m_fPlaySpeed; }
	void SetResetMaterialScale(bool b) { m_bResetMaterialScale = b; }
	bool GetResetMaterialScale() const { return m_bResetMaterialScale; }
	void SetStopChildrenAct(bool b) { m_bStopChildrenAct = b; }
	bool GetStopChildrenAct() const { return m_bStopChildrenAct; }
};

class ACTIONDYN_DATA
{
public:
	ACTIONDYN_DATA(int nLoopNum = 0, PACTION_INFO pInfo = NULL) : 
	  m_nLoopNum(nLoopNum),
	  m_pInfo(pInfo) 
	  {}
	int GetLoopNum() const { return m_nLoopNum; }
	PACTION_INFO GetActInfo() const { return m_pInfo; }
	int GetTimeSpan() const { return m_pInfo->GetTimeSpan(); }
	int GetTotalTime() const { return GetLoopNum() * GetTimeSpan(); }
private:
	int m_nLoopNum;
	PACTION_INFO m_pInfo;
};

typedef AArray<ACTIONDYN_DATA> ActDynArray;

class A3DCombActDynData
{
public:
	A3DCombActDynData(A3DCombinedAction* pAct, CECModel* pECModel) :
	m_pAct(pAct),
	m_pECModel(pECModel),
	m_nCurLoop(0),
	m_nCurActLoop(0),
	m_dwTimeSpan(0),
	m_fWeight(1.0f),
	m_nTransTime(200),
	m_dwUserData(0),
	m_bAbsTrack(false),
	m_bNoFx(false),
	m_CurEventPos(0),
	m_pParentInfo(0),
	m_pParentModel(0),
	m_bStopPrevAct(false),
	m_dwDeltaTime(0),
	m_nChannel(0),
	m_dwEventMask(-1),
	m_idCaster(0),
	m_idCastTarget(0),
	m_ptFixed(0),
	m_SerialId(0),
	m_IsAttackAct(false),
	m_nCurActIndex(-1),
	m_dwDynComActSpan(0),
	m_bIsInfiniteComAct(false),
	m_bResetPSWhenActionStop(false),
	m_bSetSpeedWhenActStart(false),
	m_fModelScale(1.0f)
	{
		ActInfoList& actInfoList = pAct->m_ActLst;
		ALISTPOSITION pos = actInfoList.GetHeadPosition();
		while (pos)
		{
			PACTION_INFO pAct = actInfoList.GetNext(pos);
			int nDynLoopNum = pAct->CalcLoopNum();
			m_arrActLoopNum.Add(ACTIONDYN_DATA(nDynLoopNum, pAct));
			if (!m_bIsInfiniteComAct && nDynLoopNum < 0) m_bIsInfiniteComAct = true;
		}
		CalcDynComActSpan();
	}

	~A3DCombActDynData() { Release(); }

protected:
	A3DCombinedAction*	m_pAct;
	CECModel*			m_pECModel;
	int					m_nCurLoop;
	int					m_nCurActLoop;
	DWORD				m_dwTimeSpan;
	float				m_fWeight;
	int					m_nTransTime;
	DWORD				m_dwUserData;
	bool				m_bAbsTrack;
	bool				m_bNoFx;
	ALISTPOSITION		m_CurEventPos;
	const ChildActInfo*	m_pParentInfo;
	CECModel*			m_pParentModel;
	DWORD				m_dwDeltaTime;
	bool				m_bStopPrevAct;
	FxBindBaseList		m_ActFxArray;
	int					m_nChannel;
	DWORD				m_dwEventMask;
	ActDynArray			m_arrActLoopNum;
	int					m_nCurActIndex;
	DWORD				m_dwDynComActSpan;
	bool				m_bIsInfiniteComAct;
	EventInfoMap		m_mapDynAddedInfo;

	clientid_t			m_idCaster;
	clientid_t			m_idCastTarget;
	A3DVECTOR3			m_ptFixed;
	unsigned char		m_SerialId;
	TargetDataVec		m_TargetsInfo;
	bool				m_IsAttackAct;

	bool				m_bResetPSWhenActionStop;
	bool				m_bSetSpeedWhenActStart;

	float				m_fModelScale;

	friend class GFX_INFO;
	friend class SFX_INFO;
	friend class ChildActInfo;
	friend class MaterialScaleChange;
	friend class SGCAttackPointMark;
	friend class GfxScriptEvent;
	friend class BoneScaleChange;
	friend class MaterialScaleTrans;
	friend class AUDIOEVENT_INFO;

protected:

	void ExpireActFx();
	void UpdateFxParam(DWORD dwDeltaTime);
	void LoopCurAct(PACTION_INFO pCur, DWORD dwDeltaTime);
	void LoopNext(DWORD dwDeltaTime);

	bool IsCurActFinished(const PACTION_INFO pInfo) const 
	{ return !pInfo->IsInfinite() && m_nCurActLoop >= m_arrActLoopNum[m_nCurActIndex].GetLoopNum(); }
	void CalcDynComActSpan();
	void ClearParentInfo();

public:

	// script writer specify the event name, and make sure the name is uniquely
	bool AddDynEventInfo(const char* szName, const EVENT_INFO* pInfo, bool bIsStart = true);
	void ClearDynEventInfo();

	void RemoveAllActiveFx(bool bForceStopFx = false);

	void Resume();
	void Release();

	int GetActChannel() const { return m_nChannel; }
	DWORD GetCurTimeSpan() const { return m_dwTimeSpan; }
	DWORD GetComActTimeSpan() const { return m_dwDynComActSpan; }
	bool IsComActInfinite() const { return m_bIsInfiniteComAct; }

	void SetParentActInfo(const ChildActInfo* pParent, CECModel* pModel, DWORD dwDeltaTime);

	void Play(int nChannel, float fWeight, int nTransTime, DWORD dwEventMask, bool bRestart = true, bool bAbsTrack = false, bool bNoFx = false);
	void UpdateAct(DWORD dwDeltaTime);
	void UpdateEvent(DWORD dwActTime, DWORD dwEventTime);
	void Stop(bool bStopAct = true, bool bForceStopFx = false);
	void Render(A3DViewport* pView);
	int GetTransTime() const { return m_nTransTime; }
	void SetTransTime(int nTransTime) { m_nTransTime = nTransTime; }
	bool GetAbsTrack() const { return m_bAbsTrack; }
	void SetAbsTrack(bool b) { m_bAbsTrack = b; }
	bool GetNoFxFlag() const { return m_bNoFx; }
	void SetNoFxFlag(bool b) { m_bNoFx = b; }
	bool IsAllEventFinished() const;
	bool IsActionStopped() const { return m_pAct->m_ActLst.GetCount() == 0 || (m_pAct->m_nLoops != -1 && m_nCurLoop >= m_pAct->m_nLoops); }
	bool IsAllFinished() const { return IsActionStopped() && IsAllEventFinished(); }
	bool IsActionFinished() const { return m_dwTimeSpan >= m_dwDynComActSpan; }
	CECModel* GetHostModel() { return m_pECModel; }
	const ChildActInfo* GetParentActInfo() const { return m_pParentInfo; }
	CECModel* GetParentModel() { return m_pParentModel; }
	DWORD GetParentDeltaTime() { return m_dwDeltaTime; }
	A3DCombinedAction* GetComAct() { return m_pAct; }
	void SetUserData(DWORD dwUserData) { m_dwUserData = dwUserData; }
	DWORD GetUserData() { return m_dwUserData; }
	void SetStopPrevAct(bool b) { m_bStopPrevAct = b; }
	bool GetStopPrevAct() const { return m_bStopPrevAct; }
	bool IsResetPSWhenActStop() const { return m_bResetPSWhenActionStop; }
	void SetResetPSWhenActStop(bool b) { m_bResetPSWhenActionStop = b; }
	bool IsSetSpeedWhenActStart() const { return m_bSetSpeedWhenActStart; }
	void SetSpeedWhenActStart(bool b) { m_bSetSpeedWhenActStart = b; }

	// attack property
	void SetCasterId(clientid_t nCaster) { m_idCaster = nCaster; }
	clientid_t GetCasterId() const { return m_idCaster; }
	void SetCastTargetId(clientid_t idTarget) { m_idCastTarget = idTarget; }
	clientid_t GetCastTargetId() const { return m_idCastTarget; }
	void SetFixedPoint(const A3DVECTOR3& pt) { m_ptFixed = pt; }
	const A3DVECTOR3& GetFixedPoint() const { return m_ptFixed; }
	void SetSerialId(unsigned char id) { m_SerialId = id; }
	unsigned char GetSerialId() const { return m_SerialId; }
	void AddOneTarget(const TARGET_DATA& td) { m_TargetsInfo.push_back(td); }
	bool IsAttackAct() const { return m_IsAttackAct; }
	void SetAttackAct(bool b) { m_IsAttackAct = b; }
	void FlushDamageInfo();
	float	GetModelScale() const { return m_fModelScale; }
};

#endif

