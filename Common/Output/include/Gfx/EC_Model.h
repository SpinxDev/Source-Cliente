/*
 * FILE: EC_Model.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <A3DVector.h>
#include <A3DCoordinate.h>
#include <AArray.h>
#include <A3DGeometry.h>
#include <A3DSkinModel.h>
#include <AString.h>
#include <A3DBone.h>
#include <AList2.h>
#include <hashtab.h>
#include <hashmap.h>
#include <A3DTypes.h>

#include <float.h>
#include "LuaMemTbl.h"
#include "GfxCommonTypes.h"

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
class A3DSkinModelRenderModifier;
class A3DSkin;
class CECViewport;
class AM3DSoundBuffer;
class A3DCombinedAction;
class A3DCombActDynData;
class CECModelStaticData;
class CECModel;
class CECModelHook;
class A3DModelPhysSyncData;
class A3DModelPhysSync;
class A3DModelPhysics;
struct PhysBreakInfo;
class A3DGFXEx;
class A3DShader;
class GFX_INFO;
class ChildActInfo;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

// script
enum
{
	enumECMScriptStartAction = 0,
	enumECMScriptEndActioin,
	enumECMScriptInit,
	enumECMScriptRelease,
	enumECMScriptModelLoaded,
	enumECMScriptChangeEquip,
	enumECMScriptChangeEquipTableInit,
	enumECMScriptPhysBreak,
	enumECMScriptCount
};

enum
{
	enumECMVarModel = 0,
	enumECMVarActName,
	enumECMVarActChannel,
	enumECMVarEquipId,
	enumECMVarEquipFlag,
	enumECMVarFashionMode,
	enumECMVarPathId,
	enumECMVarId,
	enumECMVarEquipIndex,
	enumECMVarBreakOffsetX,
	enumECMVarBreakOffsetY,
	enumECMVarBreakOffsetZ,
	enumECMVarCasterId,
	enumECMVarCastTargetId,
	enumECMVarSelf,
	enumECMVarCount,
};

#define ECM_SCRIPT_MAX_VAR_COUNT 8

struct ECM_SCRIPT_VAR
{
	int var_count;
	int var_index[ECM_SCRIPT_MAX_VAR_COUNT];
};

class CLuaState;
int ecm_get_var_count(int func);
const char* ecm_get_var_name(int func, int var_index);
void InitECMApi(CLuaState * pState);

#define COMACT_FLAG_MODE_NONE					0
#define COMACT_FLAG_MODE_ONCE					1
#define COMACT_FLAG_MODE_ONCE_IGNOREGFX			2
#define COMACT_FLAG_MODE_ONCE_MULTIIGNOREGFX	3

#define OUTER_DATA_COUNT						8

struct BoneScale
{
	int			m_nIndex;
	int			m_nType;
	A3DVECTOR3	m_vScale;
};

struct BoneScaleEx
{
	int			m_nIndex;
	float		m_fLenFactor;
	float		m_fThickFactor;
	float		m_fWholeFactor;
};

typedef abase::vector<BoneScale*> BoneScaleArray;
typedef abase::vector<BoneScaleEx*> BoneScaleExArray;
typedef abase::hash_map<AString, A3DCombinedAction*> CombinedActMap;
typedef abase::hash_map<AString, GFX_INFO*> CoGfxMap;
typedef abase::hash_map<AString, CECModel*> ECModelMap;
typedef abase::hash_map<AString, CECModelHook*> ECModelHookMap;
typedef abase::vector<A3DMATRIX4> ECMBlendMatricesSet;
typedef APtrList<ECMBlendMatricesSet*> ECMBlendMatricesSetCollector;
typedef abase::vector<A3DCOLOR> ECMBlurColorArray;

// ecm level motion blur
struct ECMActionBlurInfo
{
	ECMBlendMatricesSetCollector	m_MatricesCollector;
	ECMBlurColorArray				m_Colors;
	abase::vector<int>				m_BoneIndices;
	DWORD							m_dwTotalTime;
	DWORD							m_dwInterval;
	bool							m_bApplyToChildren;
	bool							m_bHighlight;

	bool							m_bRoot;
	bool							m_bCanAddFrame;
	size_t							m_uCurMatricesCount;
	size_t							m_uStartIndex;
	DWORD							m_dwCurTime;
	DWORD							m_dwCurTick;
	bool							m_bStopWhenActChange;

	ECMActionBlurInfo()
	{
		m_bCanAddFrame = false;
		m_uCurMatricesCount = 0;
		m_uStartIndex = 0;
		m_dwCurTime = 0;
		m_dwCurTick = 0;
		m_bStopWhenActChange = true;
	}

	~ECMActionBlurInfo()
	{
		ALISTPOSITION pos = m_MatricesCollector.GetHeadPosition();

		while (pos)
			delete m_MatricesCollector.GetNext(pos);
	}

	bool CanRenderCurFrame() const { return m_uCurMatricesCount > 1; }
};

struct ActChannelInfo
{
	abase::vector<AString> bone_names;
	abase::vector<int> joint_indices;

	ActChannelInfo& operator = (const ActChannelInfo& src)
	{
		bone_names = src.bone_names;
		joint_indices = src.joint_indices;
		return *this;
	}
};

namespace CHBasedCD
{
	class CConvexHullData;
	typedef abase::vector<CConvexHullData*> ConvexHullDataArray;
}

class CECModelStaticData
{
public:

	CECModelStaticData();

	~CECModelStaticData()
	{
		Release();
	}

protected:

	AString			m_strFilePath;
	AString			m_strSkinModelPath;		//	Skin model file name
	bool			m_bAutoUpdate;
	AString			m_strHangerName;
	bool			m_bActMapped;
	CombinedActMap	m_ActionMap;
	DWORD			m_dwVersion;
	AString			m_strHook;
	AString			m_strCCName;
	BoneScaleArray	m_BoneScales;
	BoneScaleExArray m_BoneScaleExArr;
	AString			m_strScaleBaseBone;
	A3DCOLOR		m_OrgColor;
	A3DCOLOR		m_EmissiveColor;
	float			m_fDefPlaySpeed;
	A3DSHADER		m_BlendMode;
	float			m_OuterData[OUTER_DATA_COUNT];
	bool			m_bCanCastShadow;
	bool			m_bRenderSkinModel;
	bool			m_bRenderEdge;

	CoGfxMap		m_CoGfxMap;
	CHBasedCD::ConvexHullDataArray m_ConvexHullDataArr;
	A3DAABB			m_CHAABB;
	ActChannelInfo*	m_ChannelInfoArr[A3DSkinModel::ACTCHA_MAX];
	bool			m_bChannelInfoInit;
	DWORD			m_EventMasks[A3DSkinModel::ACTCHA_MAX];

	AString			m_Scripts[enumECMScriptCount];
	bool			m_ScriptEnable[enumECMScriptCount];
	CLuaMemTbl		m_ScriptMemTbl;
	bool			m_bInitGlobalScript;
	AString			m_strGlobalScriptName;

	struct ChildInfo
	{
		AString		m_strName;
		AString		m_strPath;
		AString		m_strHHName;
		AString		m_strCCName;
	};

	typedef abase::vector<ChildInfo*> ChildInfoArray;
	ChildInfoArray m_ChildInfoArray;
	abase::vector<AString> m_AdditionalSkinLst;

	AString m_strPhysFileName;
	A3DModelPhysSyncData* m_pPhysSyncData;

	ECModelHookMap m_ECModelHookMap;

	AString				m_strPixelShader;
	AString				m_strShaderTex;
	GfxPSConstVec		m_vecPSConsts;

	friend class CECModel;
	friend class GfxScriptEvent;

protected:

	void SaveAdditionalSkin(AFile* pFile);
	bool LoadAdditionalSkin(AFile* pFile, DWORD dwVersion, bool bLoadAdditionalSkin);
	void AddScriptMethod(int index, const char* szScript);
	void OnScriptPlayAction(CECModel* pModel, int nChannel, const char* szActName);
	void OnScriptChangeEquip(CECModel* pModel, int nEquipId, int nEquipFlag, bool bFashionMode, int nPathId, int nEquipIndex);
	void OnScriptPhysBreak(CECModel* pModel, float fBreakOffsetX, float fBreakOffsetY, float fBreakOffsetZ);
	void InitGlobalScript();
	
public:

	bool LoadData(const char* szModelFile, bool bLoadAdditionalSkin);
	bool Save(const char* szFile, CECModel* pModel);
	bool SaveBoneScaleInfo(const char* szFile);
	void Release();
	CHBasedCD::ConvexHullDataArray& GetConvexHullData() { return m_ConvexHullDataArr; }
	bool HasCHAABB() const { return m_ConvexHullDataArr.size() != 0; }
	const A3DAABB& GetCHAABB() const { return m_CHAABB; }
	bool UpdateScript(int index, const char* strScript, bool bCompileOnly);
	CoGfxMap& GetCoGfxMap() { return m_CoGfxMap; }
	CombinedActMap& GetCombinedActMap() { return m_ActionMap; }
	const AString& GetPhysFileName() const { return m_strPhysFileName; }
	void SetPhysFileName(const char* szFile) { m_strPhysFileName = szFile; }
	A3DModelPhysSyncData* GetPhysSyncData() { return m_pPhysSyncData; }
	void SetPhysSyncData(A3DModelPhysSyncData* p) { m_pPhysSyncData = p; }
	DWORD GetVersion() const { return m_dwVersion; }
	const AString& GetPixelShaderPath() const { return m_strPixelShader; }
	void SetPixelShaderPath(const char* szPath) { m_strPixelShader = szPath; }
	const AString& GetShaderTexture() const { return m_strShaderTex; }
	void SetShaderTexture(const char* szPath) { m_strShaderTex = szPath; }
	GfxPSConstVec& GetPSConstVec() { return m_vecPSConsts; }
	void SetActionEventMask(int nChannel, DWORD dwMask) { m_EventMasks[nChannel] = dwMask; }
	DWORD GetActionEventMask(int nChannel) const { return m_EventMasks[nChannel]; }
	const DWORD* GetActionEventMask() const { return m_EventMasks; }

	void OnScriptEndAction(CECModel* pModel, int nChannel, const char* szActName);

	// for editor use only
	void Reset();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECModel
//	
///////////////////////////////////////////////////////////////////////////

typedef void (*ON_PHYS_BREAK)(CECModel* pECModel);

class CECModel : public A3DCoordinate
{
public:		//	Types

	//	Bone scale type
	enum
	{
		SCALE_NONE = 0,		//	No scale
		SCALE_WHOLE,		//	Scale will effect all children
		SCALE_LOCAL,		//	Scale only effect this bone
	};

	//	AABB type
	enum
	{
		AABB_SKELETON,
		AABB_INITMESH,
		AABB_AUTOSEL,
	};

	//	Action channel
	enum
	{
		ACTCHA_WOUND = 1,
	};

	//	Delay Mode
	enum
	{
		DM_PLAYSPEED_NORMAL	= 0,		//	ECM tick as normal
		DM_PLAYSPEED_DELAY,				//	ECM hold for a while in which we tick 0
		DM_PLAYSPEED_JUMP,				//	ECM jump the elapsed time in tick
	};

	//	Collision channel method
	enum
	{
		COLLISION_CHANNEL_INDEPENDENT = 0,	//	As independent collision channel
		COLLISION_CHANNEL_AS_PARENT,	//	As same as parent (if exist parent)
		COLLISION_CHANNEL_AS_COMMON,	//	As common (collision with all objects)

		COLLISION_CHANNEL_AS_CC_COMMON_HOST,	// just for the ECModel which will be the Avatar model and moved by APhysXCharacterController...
		COLLISION_CHANNEL_AS_MAJORRB,			// just as the major RB's collision channel which may be set in the ModEditor...
		
	};

	//	Phys AddForce Type
	enum
	{
		PFT_FORCE,						// parameter has unit of mass * distance/ time^2, i.e. a force
		PFT_IMPULSE,					// parameter has unit of mass * distance /time
		PFT_VELOCITY_CHANGE,			// parameter has unit of distance / time, i.e. the effect is mass independent: a velocity change.
		PFT_SMOOTH_IMPULSE,				// same as NX_IMPULSE but the effect is applied over all substeps. Use this for motion controllers that repeatedly apply an impulse.
		PFT_SMOOTH_VELOCITY_CHANGE,		// same as NX_VELOCITY_CHANGE but the effect is applied over all substeps. Use this for motion controllers that repeatedly apply an impulse.
		PFT_ACCELERATION				// parameter has unit of distance/ time^2, i.e. an acceleration. It gets treated just like a force except the mass is not divided out before integration.
	};
	
	//	Error Types
	enum
	{
		ECMERR_SMDLOADFAILED	= 0,	//	SMD file load failed
		ECMERR_UNDEFINED		= -1,	//	Undefined error
	};

public:		//	Constructor and Destructor

	CECModel();
	virtual ~CECModel();

public:		//	Attributes

public:		//	Operations

	//	Initalize model
	//	Do not load additional skin for default (only editors need this feature)
	//	Do not create physics for default
	//	User is acquired to create physics after the model is set to correct position
	bool Load(const char* szModelFile, bool bLoadSkinModel=true, int iSkinFlag=0, bool bLoadChildECModel=true, bool bLoadAdditionalSkin=false, bool bIsCreateModelPhys = false);
	bool LoadBoneScaleInfo(const char* szFile);
	bool Save(const char* szFile) { return m_pMapModel->Save(szFile, this); }
	bool SaveBoneScaleInfo(const char* szFile) { return m_pMapModel->SaveBoneScaleInfo(szFile); }

	bool IsLoaded() { return m_pMapModel != NULL; }
	DWORD GetECMVersion() const { return m_pMapModel->m_dwVersion; }
	
	//	This function will set current model's id property to the id passed in
	//	And also all the child models
	void SetId(clientid_t id);
	clientid_t GetId() const { return m_nId; }
	bool InheritParentId() const { return m_bInheritParentId; }
	void SetInheritParentId(bool b) { m_bInheritParentId = b; }

	//	Release model
	void Release();

	//	Set absolute position
	virtual void SetPos(const A3DVECTOR3& vPos);
	//	Set absolute forward and up direction
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Set absolute transform matrix
	virtual void SetAbsoluteTM(const A3DMATRIX4& mat);

	const A3DMATRIX4& GetAbsTM()
	{
		if (m_pA3DSkinModel)
			return m_pA3DSkinModel->GetAbsoluteTM();

		return m_matAbsoluteTM;
	}

	//	Tick routine
	bool Tick(DWORD dwDeltaTime, bool bNoAnim = false);
	//	Render routine
	bool Render(A3DViewport* pViewport, bool bRenderAtOnce=false, bool bRenderSkinModel=true, 
		DWORD dwFlags=0, A3DSkinModelRenderModifier* pRenderModifier = NULL);
	//	Render action fx, no skin model
	void RenderActionFX(A3DViewport* pViewport);

	//	Add a new skin
	bool AddSkinFile(const char* szSkinFile);
	//	Add a new skin
	bool AddSkin(A3DSkin* pSkin, bool bAutoRem=true);
	//	Replace a skin
	bool ReplaceSkinFile(int iIndex, const char* szSkinFile, bool bAutoRem=true);
	//	Replace a skin
	bool ReplaceSkin(int iIndex, A3DSkin* pSkin, bool bAutoRem=true);
	//	Remove a skin item, this operation release both skin and skin item
	void RemoveSkinItem(int iItem);
	//	Get skin
	A3DSkin* GetA3DSkin(int iIndex);
	//	Release all skins
	void ReleaseAllSkins();
	//	Show / Hide skin
	void ShowSkin(int iIndex, bool bShow);
	//	GetSkinShowFlag
	bool IsSkinShown(int iIndex);
	//	Get skin number
	int GetSkinNum();

	//	Open one cloth skin
	//	iLinkType: 0 - default type; 1: soft link; 2: hard link
	bool OpenClothSkin(int iSkinSlot, int iLinkType=0);
	//	Open all clothes
	//	iLinkType: 0 - default type; 1: soft link; 2: hard link
	bool OpenAllClothes(int iLinkType=0);
	//	Close one cloth skin
	void CloseClothSkin(int iSkinSlot, bool bCompletely=false);
	//	Close all clothes
	void CloseAllClothes(bool bCompletely=false);
	//	Open child model link
	bool OpenChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher);
	//	Add child model link (Must first open one child model phys link, then use add, the child attacher must not be used)
	bool AddChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher);
	//	Remove child model link (find the link between szParentHook and szChildAttacher, if there is one, remove it)
	bool RemoveChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher);
	//	Close child model link
	void CloseChildModelLink(const char* szHanger);
	//	Child model change physical link
	void ChangeChildModelLink(const char* szHanger, const char* szNewHook);
	//	Change child model physical state
	//	iState: A3DModelPhysSync::PHY_STATE_ANIMATION / PHY_STATE_SIMULATE / (not prefered)PHY_STATE_PARTSIMULATE
	void ChangeChildModelPhysState(const char* szHanger, int iState);
	//	Change all child model physical state
	void ChangeChildModelPhysState(int iState);
	//	Set ECModel's collision channel, call this after A3DModelPhysics::Bind() or after EnablePhysSystem()
	//	iMethod: COLLISION_CHANNEL_INDEPENDENT - As indenpend collision channel
	//			 COLLISION_CHANNEL_AS_PARENT - As same as parent (if exist parent)
	//			 COLLISION_CHANNEL_AS_COMMON - As common (collision with all objects)
	bool SetCollisionChannel(int iMethod, CECModel* pParent = NULL, bool bSetChildCollisionChannel = true);
	//	Get ECModel's collision channel irrespective children ECM
	//	return -1 if the PhysX system of ECM is disabled.
	//  return the enum type that defined in ECM (except for COLLISION_CHANNEL_AS_PARENT and COLLISION_CHANNEL_AS_MAJORRB)
	int  GetCollisionChannel() const;
	
	//	Interface related for breakable
	//	Break a joint (Break a joint if exist)
	void BreakJoint(const char * szJointName);
	//	Break joint by bone
	void BreakJointByBone(const char * szBoneName);
	//	Apply force
	//	iPhysForceType can be one of the following values
	// 		PFT_FORCE,						// parameter has unit of mass * distance/ time^2, i.e. a force
	// 		PFT_IMPULSE,					// parameter has unit of mass * distance /time
	// 		PFT_VELOCITY_CHANGE,			// parameter has unit of distance / time, i.e. the effect is mass independent: a velocity change.
	// 		PFT_SMOOTH_IMPULSE,				// same as NX_IMPULSE but the effect is applied over all substeps. Use this for motion controllers that repeatedly apply an impulse.
	// 		PFT_SMOOTH_VELOCITY_CHANGE,		// same as NX_VELOCITY_CHANGE but the effect is applied over all substeps. Use this for motion controllers that repeatedly apply an impulse.
	// 		PFT_ACCELERATION				// parameter has unit of distance/ time^2, i.e. an acceleration. It gets treated just like a force except the mass is not divided out before integration.
	bool AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist = FLT_MAX, int iPhysForceType = PFT_FORCE, float fSweepRadius = 0.0f);
	
	void SetClothWindAcceleration(const A3DVECTOR3& vWindAcceleration);

	//	Creator of EC_Model could set a break handler here
	//	which could be called back when PhysX makes a break to this model.
	void SetPhysBreakHandler(ON_PHYS_BREAK pPhysBreakHandler);
	//	This method is called back by the lower level objects in (A3DModelPhysSync::OnBreak)
	//	User should not call this method directly
	void OnPhysBreak(const PhysBreakInfo& physBreakInfo);

	//	Enable physical system
	//	Parameters:
	//	bHardLink == true: means the Skin is linked with the model physics objects by a physics joint and updated in physics system
	//				false: means the skin is updated by the model's hook position each time the model is animated
	//	bRecur	  == true: means recur this function on each child model, and try open child models' physics link
	//			    false: do nothing above
	bool EnablePhysSystem(bool bHardLink, bool bRecur = false, bool bOpenCloth = true);
	//	Disable physical system
	void DisablePhysSystem();

	//	Scale specified bone
	bool ScaleBone(const char* szBone, int iScaleType, const A3DVECTOR3& vScale);
	//	Get bone's scale information
	bool GetBoneScaleInfo(const char* szBone, int* piScaleType, A3DVECTOR3& vScale);
	//	Set bone trans flag
	void SetBoneTransFlag(const char* szBone, bool bFlag);
	
	bool ScaleBoneEx(const char* szBone, const A3DVECTOR3& vScale);
	void CalcFootOffset(const char* szBaseBone);

	CECModelStaticData* GetStaticData() { return m_pMapModel; }
	//	Get A3D skin model object
	A3DSkinModel* GetA3DSkinModel() { return m_pA3DSkinModel; }
	//	Get auto-update flag
	bool GetAutoUpdateFlag() { return m_bAutoUpdate; }
	//	Set auto-update flag
	void SetAutoUpdateFlag(bool bAuto) { m_bAutoUpdate = bAuto; }
	//	Get slowest update flag
	bool GetSlowestUpdateFlag() { return m_bSlowestUpdate; } 
	//	Set slowest update flag
	void SetSlowestUpdateFlag(bool bTrue) { m_bSlowestUpdate = bTrue; } 

	bool GetDefAutoUpdateFlag() const { return m_pMapModel->m_bAutoUpdate; }
	void SetDefAutoUpdateFlag(bool bAuto) { m_pMapModel->m_bAutoUpdate = bAuto; }

	//	Combination action interfaces
	int GetComActCount() const { return (int)m_pMapModel->m_ActionMap.size(); }

	A3DCombinedAction* GetComActByIndex(int nIndex)
	{
		CombinedActMap::iterator it = m_pMapModel->m_ActionMap.begin();
		while (nIndex--) ++it;
		return it->second;
	}

	A3DCombinedAction* GetComActByName(const AString& strName)
	{
		CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
		CombinedActMap::iterator it = ActionMap.find(strName);
		if (it == ActionMap.end()) return NULL;
		return it->second;
	}

	DWORD GetComActTimeSpanByName(const AString& strName);
	void AddCombinedAction(A3DCombinedAction* pAct);
	void RemoveCombinedAction(const AString& strName);
	bool RenameCombinedAction(A3DCombinedAction* pAct, const AString& strNewName);

	int GetAdditionalSkinCount() const { return (int)m_pMapModel->m_AdditionalSkinLst.size(); }
	const AString& GetAdditionalSkin(int nIndex) const { return m_pMapModel->m_AdditionalSkinLst[nIndex]; }
	void AddAdditionalSkin(const char* szFile) { m_pMapModel->m_AdditionalSkinLst.push_back(szFile); }
	void DelAdditionalSkin(int nIndex) { m_pMapModel->m_AdditionalSkinLst.erase(&m_pMapModel->m_AdditionalSkinLst[nIndex]); }

	//	Play a combined action by name
	//	nChannel	: Action channel
	//	szActName	: Action name
	//	fWeight		: Weight used in action blending
	//	bRestart	: Whether to replay this action while already playing the exact same action
	//	nTransTime	: Length of interpolation time with last action
	//	bForceStop	: Stop the current playing action
	//	dwUserData	: User data you may set in and be used later (get it by GetCurActionUserData)
	//	bNoFx		: Do not play effects on this action
	bool PlayActionByName(const char* szActName, float fWeight, bool bRestart=true, int nTransTime=200, bool bForceStop=true, DWORD dwUserData=0, bool bNoFx=false, bool *pActFlag=NULL, DWORD dwFlagMode=COMACT_FLAG_MODE_NONE);
	bool PlayActionByName(int nChannel, const char* szActName, float fWeight, bool bRestart=true, int nTransTime=200, bool bForceStop=true, DWORD dwUserData=0, bool bNoFx=false, bool *pActFlag=NULL, DWORD dwFlagMode=COMACT_FLAG_MODE_NONE);

	//	Queue an action into the waiting list
	//	szActName	: Action name
	//	nTransTime	: Length of interpolation time with last action
	//	dwUserData	: User data you may set in and be used later (get it by GetCurActionUserData)
	//	bForceStopPRevAct	: Do not wait until the end of all previous action's GFXs
	//	bCheckTailDup		: Check the tail action of queue, if same, ignore this one
	//	bNoFx				: Do not play effects on this action
	bool QueueAction(const char* szActName, int nTransTime=200, DWORD dwUserData=0, bool bForceStopPrevAct=false, bool bCheckTailDup = false, bool bNoFx = false, bool bResetSpeed = false)
	{
		return QueueAction(0, szActName, nTransTime, dwUserData, bForceStopPrevAct, bCheckTailDup, bNoFx, bResetSpeed);
	}

	bool QueueAction(int nChannel, const char* szActName, int nTransTime=200, DWORD dwUserData=0, bool bForceStopPrevAct=false, bool bCheckTailDup = false, bool bNoFx = false, bool bResetSpeed = false);

	void SetComActFlag(bool* pFlag, DWORD dwFlagMode) {	SetComActFlag(0, pFlag, dwFlagMode); }
	void SetComActFlag(int nChannel, bool* pFlag, DWORD dwFlagMode)
	{
		ChannelAct& ca = m_ChannelActs[nChannel];
		ChannelActNode* pNode = ca.GetHighestRankNode();

		if (pNode)
		{
			pNode->m_pActFlag = pFlag;
			pNode->m_dwFlagMode = dwFlagMode;
		}
	}

	void ClearComActFlagAllRankNodes (bool bSignalCurrent) { ClearComActFlagAllRankNodes(0, bSignalCurrent); }
	void ClearComActFlagAllRankNodes(int nChannel, bool bSignalCurrent)
	{
		ChannelAct& ca = m_ChannelActs[nChannel];


		ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();

		while (pos)
		{
			ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);

			if (pNode)
			{
				if (pNode->m_pActFlag && bSignalCurrent)
					*pNode->m_pActFlag = true;

				//ca.m_dwFlagMode = COMACT_FLAG_MODE_NONE;
				pNode->m_pActFlag = NULL;
			}
		}
	}

	void ClearComActFlag(bool bSignalCurrent) { ClearComActFlag(0, bSignalCurrent); }
	void ClearComActFlag(int nChannel, bool bSignalCurrent)
	{
		ChannelAct& ca = m_ChannelActs[nChannel];
		ChannelActNode* pNode = ca.GetHighestRankNode();

		if (pNode)
		{
			if (pNode->m_pActFlag && bSignalCurrent)
				*pNode->m_pActFlag = true;

			//ca.m_dwFlagMode = COMACT_FLAG_MODE_NONE;
			pNode->m_pActFlag = NULL;
		}
	}

	void StopAllActions(bool bStopFx = false);
	void StopChildrenAct();
	void StopChannelAction(int nChannel, bool bStopAct, bool bStopFx = false);
	void StopChannelAction(int nChannel, BYTE rank, bool bStopAct, bool bStopFx = false);

	bool AddECMHook(const char* szName, int nID, float fScale, const A3DVECTOR3& vPos);
	bool RenameECMHook(const char* szOldName, const char* szNewName);
	void RemoveECMHook(const char* szName);
	int GetECMHookCount() const;
	CECModelHook* GetECMHook(int iIndex) const;
	CECModelHook* GetECMHook(const char* szName) const;

protected:

	bool IsECMHookValid(const ECModelHookMap& hookMap, const char* szName) const;
	A3DMATRIX4 GetGfxBindHookTransMat(GFX_INFO* pInfo, bool& bIsHookMat);

public:
	
	void PlayGfx(const char* szPath, const char* szHook, float fScale = 1.0f, bool bFadeOut = true, bool bUseECMHook = false, DWORD dwFadeOutTime = 0);
	//	Play Gfx with offset, pitch, yaw and rot : Called by script in ECM_PlayGfxEx
	void PlayGfx(const char* szPath, const char* szHook, float fScale, bool bFadeOut, const A3DVECTOR3& vOffset, float fPitch, float fYaw, float fRot, bool bUseECMHook = false, DWORD dwFadeOutTime = 0);
	
	void RemoveGfx(const char* szPath, const char* szHook);
	void RemoveGfx(const char* szKey);
	A3DGFXEx* GetGfx(const char* szPath, const char* szHook);

	A3DSMActionChannel* BuildActionChannel(int iChannel, int iNumJoint, int* aJoints)
	{
		if (m_pA3DSkinModel)
			return m_pA3DSkinModel->BuildActionChannel(iChannel, iNumJoint, aJoints);
		else
			return NULL;
	}

	A3DSMActionChannel* GetActionChannel(int iChannel)
	{
		if (m_pA3DSkinModel)
			return m_pA3DSkinModel->GetActionChannel(iChannel);
		else
			return NULL;
	}

	ActChannelInfo** GetActChannelInfo() { return m_pMapModel ? m_pMapModel->m_ChannelInfoArr : 0; }

	A3DCombActDynData* GetCurAction(int nChannel)
	{
		assert(nChannel >= 0 && nChannel < A3DSkinModel::ACTCHA_MAX);
		ChannelAct& ca = m_ChannelActs[nChannel];
		ChannelActNode* pNode = ca.GetHighestRankNode();

		if (pNode)
			return pNode->m_pActive;
		else
			return 0;
	}

	bool IsActionStopped(int nChannel);

	//	Get current action's user data
	DWORD GetCurActionUserData(int nChannel);
	DWORD GetCurActionUserData() { return GetCurActionUserData(0); }


	//	Set Action Event Mask will make some type of the event be enabled
	//	Use the method like this:
	//	SetActionEventMask(nChannel, EVENT_MASK_GFX) -> this will make all events not be played except the GFX
	//	the definition of EVENT_MASK_GFX and other related could be see in A3DCombinedAction.h
	//	Use EVENT_MASK_ALL to enable all events, which is the default value
	void SetActionEventMask(int nChannel, DWORD dwMask) { m_EventMasks[nChannel] = dwMask; }
	DWORD GetActionEventMask(int nChannel) const { return m_EventMasks[nChannel]; }

	void Show(bool bShow, bool bAffectChild = true);
	bool IsShown() const { return m_bShown; }
	void ShowChildModel(const char* szHangerName, bool bShow)
	{
		CECModel* pChild = GetChildModel(szHangerName);

		if (pChild)
			pChild->Show(bShow);
	}
	bool IsChildModelShown(const char* szHangerName) const
	{
		CECModel* pChild = GetChildModel(szHangerName);
		if (pChild)
			return pChild->IsShown();
		
		return false;
	}

	//	szHangerName : a name that specify this particular child model
	//	szHookName : this model's hook name on which the child model will be hanged on
	//	szCCName : child model's hook name by which the child model is hooked
	bool AddChildModel(const char* szHangerName, bool bOnBone, const char* szHookName, const char* szModelFile, const char* szCCName);
	bool AddChildModel(const char* szHangerName, bool bOnBone, const char* szHookName, CECModel* pChild, const char* szCCName);
	void RemoveChildModel(const char* szHangerName, bool bDel = true);
	int GetChildCount() const { return (int)m_ChildModels.size(); }
	bool ChildChangeHook(const char* szHanger, const char* szNewHook);

	CECModel* GetChildModel(const char* szChild) const
	{
		ECModelMap::const_iterator it = m_ChildModels.find(szChild);
		if (it == m_ChildModels.end()) return NULL;
		return it->second;
	}

	CECModel* GetChildModel(int nIndex) const
	{
		ECModelMap::const_iterator it = m_ChildModels.begin();
		while (nIndex--) ++it;
		return it->second;
	}

	const AString& GetHangerName(int nIndex) const
	{
		ECModelMap::const_iterator it = m_ChildModels.begin();
		while (nIndex--) ++it;
		return it->first;
	}

	const char* GetHangerName(CECModel* pModel) const
	{
		for (ECModelMap::const_iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		{
			if (it->second == pModel)
				return it->first;
		}

		return NULL;
	}

	void AddCoGfx(GFX_INFO* pInfo);
	void DelCoGfx(GFX_INFO* pInfo);
	AString GetCoGfxName(GFX_INFO* pInfo);
	CoGfxMap& GetCoGfxMap() { return m_CoGfxMap; }
	void UpdateCoGfx(AString strIndex, GFX_INFO* pSrc);

	void QueueFadeOutSfx(AM3DSoundBuffer* pSfx) { m_FadeOutSfxLst.AddTail(pSfx); }
	AString GetFilePath() const { return m_pMapModel->m_strFilePath; }
	bool ParentActivate(const AString& strActName, const ChildActInfo* pHostInfo, CECModel* pParentModel, DWORD dwDeltaTime);
	void ActivateChildAct(const AString& strActName, const ChildActInfo* pHostInfo, CECModel* pParentModel, DWORD dwDeltaTime);

	void ResetMaterialOrg();

	void ResetMaterialScale();

	void SetPlaySpeed(float fSpeed) { m_fPlaySpeed = fSpeed * m_pMapModel->m_fDefPlaySpeed; }
	float GetPlaySpeed() const { return m_fPlaySpeed; }

	BoneScaleArray& GetBoneScales() { return m_pMapModel->m_BoneScales; }
	BoneScale* GetBoneScaleByIndex(int nIndex)
	{
		size_t i;

		for (i = 0; i < m_pMapModel->m_BoneScales.size(); i++)
			if (m_pMapModel->m_BoneScales[i]->m_nIndex == nIndex)
				return m_pMapModel->m_BoneScales[i];

		return NULL;
	}

	void RemoveBoneScale(BoneScale* pScale);
	void UpdateBoneScales();

	BoneScaleExArray& GetBoneScaleExArr() { return m_pMapModel->m_BoneScaleExArr; }
	BoneScaleEx* GetBoneScaleExByIndex(int nIndex)
	{
		for (size_t i = 0; i < m_pMapModel->m_BoneScaleExArr.size(); i++)
			if (m_pMapModel->m_BoneScaleExArr[i]->m_nIndex == nIndex)
				return m_pMapModel->m_BoneScaleExArr[i];

		return NULL;
	}

	void RemoveBoneScaleEx(BoneScaleEx* pScale);
	void UpdateBoneScaleEx();
	void UpdateBoneScaleEx(BoneScaleExArray& arr, const char* szBaseBone);
	AString GetBaseBoneName() const { return m_pMapModel->m_strScaleBaseBone; }
	void SetBaseBoneName(const char* szName) { m_pMapModel->m_strScaleBaseBone = szName; }

	//	Set aabb type
	void SetAABBType(int iType);

	//	shadow cast manipulation
	inline bool GetCastShadow()	const		{ return m_bCastShadow; }
	inline void SetCastShadow(bool flag)	{ m_bCastShadow = flag; }

	const char * GetSkinModelPath()			{ return m_pMapModel->m_strSkinModelPath; }
	CHBasedCD::ConvexHullDataArray& GetConvexHullData() { return m_pMapModel->GetConvexHullData(); }
	bool HasCHAABB() const { return m_pMapModel->HasCHAABB(); }
	const A3DAABB& GetCHAABB() const { return m_pMapModel->GetCHAABB(); }

	//	Get Model AABB 
	//	If skinmodel exist, return m_ModelAABB 
	//	which is updated by Skin model and all visible child models
	//	Else return the CH AABB
	const A3DAABB& GetModelAABB() const;

	void SetAffectedByParent(bool b) { m_bCanAffectedByParent = b; }
	bool CanAffectedByParent() const { return m_bCanAffectedByParent; }

	float GetTransparent() const { return m_fAlpha == 1.0f ? -1.0f : 1.0f - m_fAlpha; }
	void SetTransparent(float fTransparent);
	void SetInnerAlpha(float fInner);

	void SetSrcBlend(A3DBLEND srcBlend);
	void SetDstBlend(A3DBLEND dstBlend);

	const A3DCOLORVALUE& GetColor() const { return m_Color; }
	void SetColor(const A3DCOLORVALUE& cl);
	void SetInnerColor(const A3DCOLORVALUE& cl);
	void ApplyColor();

	void Reset()
	{
		if (!m_pMapModel)
			return;

		m_pMapModel->Reset();
	}

	//	Build wound action channel
	bool BuildWoundActionChannel();

	bool IsPlayingComAct() const
	{
		const ChannelAct& ca = m_ChannelActs[0];
		return ca.GetHighestRankNode()!= 0;
	}

	A3DCombActDynData* GetCurComAct()
	{
		ChannelAct& ca = m_ChannelActs[0];
		ChannelActNode* pNode = ca.GetHighestRankNode();
		return pNode ? pNode->m_pActive : 0;
	}

	bool SetSkinModel(A3DEngine* pEngine, const char* szFile);

	//	Interfaces for modifying m_pMapModel data
	const A3DSHADER& GetBlendMode() const;
	void SetBlendMode(const A3DSHADER& bld);
	A3DCOLOR GetOrgColor() const;
	void SetOrgColor(A3DCOLOR cl);
	A3DCOLOR GetExtraEmissive() const;
	void SetExtraEmissive(A3DCOLOR cl);
	float* GetOuterData();
	float GetDefPlaySpeed() const;
	void SetDefPlaySpeed(float fSpeed);
	const bool CanCastShadow() const;
	void SetCanCastShadow(bool b);
	const bool CanRenderSkinModel() const;
	void SetRenderSkinModel(bool b);
	bool CanRenderEdge() const;
	void SetRenderEdge(bool b);

	void SetUseAbsTrack(bool b) { m_bAbsTrack = b; }

	//	Set scale to all root-bones and gfx
	//	bMulSclInfo will take the bone scale info into consideration
	bool ScaleAllRootBonesAndGfx(float fScale);
	float	GetAllRootBonesScale() const { return m_fRootBoneScaleFactor; }
	//	Set a scale factor for gfx on this ecmodel
	//	Set gfx size scale and (added 2009.10.23) gfx offset scale relative to the hook.
	float GetGfxScale() const { return m_fGfxScaleFactor; }
	void SetGfxScale(float fScale) { m_fGfxScaleFactor = fScale; m_bGfxScaleChanged = true; }

	bool IsGfxUseLOD() const { return m_bGfxUseLod; }
	//	Set Gfx Use Lod will affect on all gfxs currently loaded, default is with force for compatibility
	inline void SetGfxUseLOD(bool b) { SetGfxUseLOD(b, true); }
	//	bForce means do not check current state, and force set property to all related gfxs
	void SetGfxUseLOD(bool b, bool bForce);
	//	Is this ECModel and all related GFX disable shaking camera
	inline bool GetDisableCamShake() const	{ return m_bDisableCamShake; }
	//	Set disable cam shake will affect on all gfxs currently loaded
	void SetDisableCamShake(bool b);
	//	Get / Set this ECModel is created by GFX or not
	inline bool IsCreatedByGfx() const { return m_bCreatedByGfx; }
	void SetCreatedByGfx(bool b) { m_bCreatedByGfx = b; }


	int GetSfxPriority() const { return m_nSfxPriority; }
	void SetSfxPriority(int n) { m_nSfxPriority = n; }
	int GetMainChannel() const { return m_nMainChannel; }
	void SetMainChannel(int n) { m_nMainChannel = n; }
	bool GetFashionMode() const { return m_bFashionMode; }
	bool HasEquip(int id) const { return m_EquipSlotMap.find(id) != m_EquipSlotMap.end(); }

	int GetEquipId(int nIndex) const
	{
		abase::hash_map<int, int>::const_iterator it = m_SlotEquipMap.find(nIndex);

		if (it == m_SlotEquipMap.end())
			return 0;
		else
			return it->second;
	}

	bool GetUpdateLightInfoFlag() const { return m_bUpdateLightInfo; }
	void SetUpdateLightInfoFlag(bool b) { m_bUpdateLightInfo = b; }

	bool RegisterAbsTrackOfBone(const char* szBoneName)
	{
		if (!m_pA3DSkinModel)
			return false;

		return m_pA3DSkinModel->RegisterAbsTrackOfBone(szBoneName);
	}
	AString GetHookName() const { return m_strHookName; }
	AString GetCCName() const { return m_strCC; }

	void PlayAttackAction(const char* szActName, unsigned char SerialId, clientid_t nCasterId, clientid_t nCastTarget, const A3DVECTOR3* pFixedPoint, DWORD dwUserData = 0, bool bNoFx = false)
	{
		PlayAttackAction(0, szActName, 200, SerialId, nCasterId, nCastTarget, pFixedPoint, dwUserData, bNoFx);
	}

	void PlayAttackAction(int nChannel, const char* szActName, unsigned char SerialId, clientid_t nCasterId, clientid_t nCastTarget, const A3DVECTOR3* pFixedPoint, DWORD dwUserData = 0, bool bNoFx = false)
	{
		PlayAttackAction(nChannel, szActName, 200, SerialId, nCasterId, nCastTarget, pFixedPoint, dwUserData, bNoFx);
	}

	void PlayAttackAction(int nChannel, const char* szActName, int nTransTime, unsigned char SerialId, clientid_t nCasterId, clientid_t nCastTarget, const A3DVECTOR3* pFixedPoint, DWORD dwUserData, bool bNoFx);

	void AddOneAttackDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage)
	{
		AddOneAttackDamageData(0, nCaster, nTarget, SerialId, dwModifier, nDamage);
	}

	void AddOneAttackDamageData(int nChannel, clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage, bool bIsAttDelay = false, int nAttDelayTimeIdx = 0);

	void OnScriptChangeEquip(int nEquipId, int nEquipFlag, bool bFashionMode, int nPathId = 0, int nEquipIndex = 0);

	A3DModelPhysics* GetModelPhysics() { return m_pModelPhysics; }
	A3DModelPhysSync* GetPhysSync();
	
	//	Sync routine after physics simulation
	bool SyncModelPhys();
	//	Resize physical actors and clothes after model size changed
	bool ResizePhysObjects();
	
	//	Add a combo model
	void AddComboModel(CECModel* pModel) { m_aComboModels.Add(pModel); }
	//	Remove combo model
	void RemoveComboModel(CECModel* pModel);
	void RemoveComboModelByIndex(int n) { m_aComboModels.RemoveAt(n); }
	//	Find combo model
	int FindComboModel(CECModel* pModel) { return m_aComboModels.Find(pModel); }
	//	Get combo model number
	int GetComboModelNum() { return m_aComboModels.GetSize(); }
	//	Get a combo model by index
	CECModel* GetComboModel(int n) { return m_aComboModels[n]; }

	//	Used by editor
	int GetLastError() const { return m_nErrCode; }

	bool SetReplaceShader(const char* szShader, const char* szShaderTex, const GfxPSConstVec& consts);
	void RemoveReplaceShader();
	
	typedef void (WINAPI *GFX_NOTIFY_DELAY_START_FUNC)(clientid_t nTargetId, clientid_t nCasterId, int nDelayTime);
	typedef void (*ECM_CUSTOM_RENDER_FUNC)(A3DViewport* pView, A3DSkinModel* pSkinModel, A3DShader* pRefShader, const GfxPSConstVec& ConstsVec, DWORD dwPSPlayedTime, const AString& strPrefix, bool bForceAlpha);

	static GFX_NOTIFY_DELAY_START_FUNC m_NotifyDelayStartFunc;
	static GFX_NOTIFY_DELAY_START_FUNC GetNotifyDelayStartFunc() { return m_NotifyDelayStartFunc; }
	static void SetNotifyDelayStartFunc(GFX_NOTIFY_DELAY_START_FUNC _func) { m_NotifyDelayStartFunc = _func; }

	static ECM_CUSTOM_RENDER_FUNC m_CustomRenderFunc;
	static ECM_CUSTOM_RENDER_FUNC GetCustomRenderFunc() { return m_CustomRenderFunc; }
	static void SetCustomRenderFunc(ECM_CUSTOM_RENDER_FUNC _func) { m_CustomRenderFunc = _func; }
	static void FinalizeCustomRenderFunc();

protected:	//	Attributes

	bool				m_bCreatedByGfx;		//	flag indicates whether this ecmodel is created by a GFX (ECModel element type)
	bool				m_bDisableCamShake;		//	flag indicates whether this ecmodel and all its related gfx will shake camera, false for default
	bool				m_bCastShadow;			//	flag indicates whether it will cast shadow
	bool				m_bAutoUpdate;			//	true, automatically change update frequence
	bool				m_bSlowestUpdate;		//	true, force to use slowest auto update frequence
	DWORD				m_dwUpdTimeCnt;			//	Update time coutner
	int					m_nErrCode;				//	Last error code
	A3DAABB				m_ModelAABB;			//	ECModel's aabb (updated by skinmodel and all child models)

	A3DDevice*			m_pA3DDevice;
	A3DSkinModel*		m_pA3DSkinModel;	//	A3D model object
	CECModelStaticData*	m_pMapModel;

	APtrArray<CECModel*>	m_aComboModels;		//	Combo models. combo models aren't controlled by this model. It only gives a way for high
												//	level application a chance to reference other models through this model


	typedef APtrList<A3DCombActDynData*> ActQueue;

	struct ChannelActNode
	{
		BYTE				m_Rank;
		A3DCombActDynData*	m_pActive;
		bool*				m_pActFlag;
		DWORD				m_dwFlagMode;
		ActQueue			m_QueuedActs;

		ChannelActNode();
		~ChannelActNode();
		void RemoveQueuedActs();
	};

	struct ChannelAct
	{
		APtrList<ChannelActNode*> m_RankNodes;

		~ChannelAct()
		{
			Release();
		}

		void Release()
		{
			ALISTPOSITION pos = m_RankNodes.GetHeadPosition();

			while (pos)
				delete m_RankNodes.GetNext(pos);

			m_RankNodes.RemoveAll();
		}

		ChannelActNode* GetNodeByRank(BYTE rank) const
		{
			ALISTPOSITION pos = m_RankNodes.GetHeadPosition();

			while (pos)
			{
				ChannelActNode* pNode = m_RankNodes.GetNext(pos);

				if (pNode->m_Rank == rank)
					return pNode;
			}

			return 0;
		}

		ChannelActNode* GetHighestRankNode() const
		{
			ChannelActNode* pHighest = 0;
			ALISTPOSITION pos = m_RankNodes.GetHeadPosition();

			while (pos)
			{
				ChannelActNode* pNode = m_RankNodes.GetNext(pos);

				if (!pHighest || pNode->m_Rank > pHighest->m_Rank)
					pHighest = pNode;
			}

			return pHighest;
		}

		ChannelActNode* RemoveNodeByRank(BYTE rank)
		{
			ALISTPOSITION pos = m_RankNodes.GetHeadPosition();

			while (pos)
			{
				ALISTPOSITION posCur = pos;
				ChannelActNode* pNode = m_RankNodes.GetNext(pos);

				if (pNode->m_Rank == rank)
				{
					m_RankNodes.RemoveAt(posCur);
					return pNode;
				}
			}

			return 0;
		}
	};

	ON_PHYS_BREAK		m_pPhysBreakHandler;
	clientid_t			m_nId;				// id defined by user
	bool				m_bInheritParentId;
	ChannelAct			m_ChannelActs[A3DSkinModel::ACTCHA_MAX];
	AString				m_strHookName;		// parent hook name
	AString				m_strCC;			// child model CC hook name
	bool				m_bLoadSkinModel;
	float				m_fPlaySpeed;
	bool				m_bShown;
	int					m_iAABBType;
	bool				m_bCanAffectedByParent;
	float				m_fInnerAlpha;
	float				m_fOuterAlpha;
	float				m_fAlpha;
	A3DCOLORVALUE		m_InnerColor;
	A3DCOLORVALUE		m_OuterColor;
	A3DCOLORVALUE		m_Color;
	bool				m_bAbsTrack;
	float				m_fRootBoneScaleFactor;
	float				m_fGfxScaleFactor;
	bool				m_bGfxScaleChanged;
	bool				m_bUpdateLightInfo;
	bool				m_bFirstActPlayed;
	bool				m_bGfxUseLod;
	bool				m_bCoGfxSleep;				// All Co Gfxs do not take part in tick or render
	int					m_nSfxPriority;
	int					m_nMainChannel;
	bool				m_bFashionMode;
	bool				m_bUseParentSpeed;
	bool				m_bEventUsePlaySpeed;
	abase::hash_map<int, int> m_EquipSlotMap;
	abase::hash_map<int, int> m_SlotEquipMap;

	ECModelMap m_ChildModels;

	struct ChildPhyDelayInfo
	{
		ChildPhyDelayInfo() { Reset(); }
		ChildPhyDelayInfo(int _iDelayTime, int _nPhyState)
			: iDelayTime(_iDelayTime), nPhyState(_nPhyState) {}
		void Reset() { iDelayTime = 0; nPhyState = 1; }

		int iDelayTime;
		int nPhyState;
	};
	typedef abase::hash_map<CECModel*, ChildPhyDelayInfo> ChildPhyMap;
	ChildPhyMap m_ChildPhyDelayMap;

	typedef abase::vector<A3DCombinedAction*> CombinedActLst;

	typedef APtrList<AM3DSoundBuffer*> FadeOutSfxLst;
	FadeOutSfxLst m_FadeOutSfxLst;

	CoGfxMap m_CoGfxMap;

	A3DModelPhysics*	m_pModelPhysics;
	bool				m_bPhysEnabled;		//	Physical system enable flag

	struct ChildModelLinkInfo
	{
		ChildModelLinkInfo() {}
		ChildModelLinkInfo(const char* szParentHook, const char* szChildAttacher)
			: m_strParentHook(szParentHook)
			, m_strChildAttacher(szChildAttacher)
		{

		}

		void SetHook(const char* szHook) { m_strParentHook = szHook; }

		AString m_strParentHook;
		AString m_strChildAttacher;
	};

	typedef abase::vector<ChildModelLinkInfo> ChildModelLinkInfoArray;
	ChildModelLinkInfoArray m_PhysHookAttachLinks;

	//	Delay Mode
	struct DelayModeInfo {
		DelayModeInfo() 
			: m_nDelayMode(CECModel::DM_PLAYSPEED_NORMAL)
			, m_nDelayTime(0)
			, m_nTimeElapsed(0) {}

		void OnStartDelay(int nDelayTime);	// On start delay mode
		int OnUpdate(DWORD dwTickTime);							// Return the current delay mode state
		int GetElapsedTime();
		void OnPlayComAction();

		int m_nDelayMode;
		int m_nDelayTime;
		int m_nTimeElapsed;
	};

	DelayModeInfo		m_kECMDelayInfo;		//	ECModel delay mode information
	bool				m_bIsDelayActive;		//	Specify whether ECModel uses Delay

	// ecm level motion blur
	ECMActionBlurInfo*	m_pBlurInfo;

	// Enable script event
	bool				m_bEnableScriptEvent;

	// A3DShader
	A3DShader*			m_pPixelShader;
	DWORD				m_dwPSTickTime;

	A3DShader*			m_pReplacePS;
	GfxPSConstVec		m_vecReplacePSConsts;

	//	Event mask
	DWORD				m_EventMasks[A3DSkinModel::ACTCHA_MAX];

	DWORD				m_dwChildRenderFlag;

protected:	//	Operations

	//	Try to open all child model links
	bool TryOpenChildModelLinks(const char* szHanger, const ChildModelLinkInfoArray& aChildModelLinks);

	bool IsGfxScaleChanged() const { return m_bGfxScaleChanged; }

	//	Create model physical sync
	bool CreateModelPhysics();
	
	//	Inner Update CoGfx's ModParam
	bool InnerUpdateCoGfxParam(GFX_INFO* pInfo, A3DSkeleton* pSkeleton);
	
	//	Check and update action's play speed
	void SetPlaySpeedByChannel(int nChannel, A3DCombinedAction* pComAct);
	
	//	Tick ECModel combined actions
	void UpdateChannelActs(DWORD dwUpdateTime);
	//	Tick ECModel combined action events
	void UpdateChannelEvents(DWORD dwActTime, DWORD dwEventTime);
	//	Tick Skin Model
	void TickSkinModel(DWORD dwUpdateTime);
	//	Tick CoGfx
	void TickCoGfx(DWORD dwUpdateTime);
	//	Tick Child Models
	void TickShownChildModels(DWORD dwParentTickTime, DWORD dwRealTickTime, bool bNoAnim);

	//	Render CoGfx
	void RenderCoGfx();

	//	Apply blur matrix
	void ApplyBlurMatrices(int nIndex);

	//	Clear motion blur
	void ClearMotionBlur();

	void ApplyPixelShader();
	void RestorePixelShader();

	//	Update model aabb by skin model and all child models
	void UpdateModelAABB();

public:		//	Operations

	//	Inner help functions for gfx property transition
	void TransferEcmProperties(A3DGFXEx* pGfx);
	void SetDelayActive(bool b) { m_bIsDelayActive = b; }
	bool IsDelayActive() const { return m_bIsDelayActive; }
	void StartECModelDelayMode(int nDelayTime);
	void UpdateBaseActTimeSpan();
	void UpdateHookIndex() {}
	void UpdateCoGfxHook();
	void UpdateFadeOutSfxLst(DWORD dwDelta);

	//	This function only show or stop all current CoGfx, it does not contain any state, just a command like operation
	void ShowCoGfx(bool bShow);
	//	This function will make all CoGfx stop from Ticking or Rendering, like they are sleeping
	void SleepCoGfx(bool bSleep, bool bRecurOnChild);
	bool GetSleepCoGfx() const { return m_bCoGfxSleep; }

	void LoadChildModels();
	const AString& GetScript(int index) const { return m_pMapModel->m_Scripts[index]; }
	bool UpdateScript(int index, const char* strScript, bool bCompileOnly) { return m_pMapModel->UpdateScript(index, strScript, bCompileOnly); }
	bool IsGlobalScriptInit() const { return m_pMapModel->m_bInitGlobalScript; }
	void SetGlobalScriptInit(bool b) { m_pMapModel->m_bInitGlobalScript = b; }
	void InitGlobalScript() { m_pMapModel->InitGlobalScript(); }
	ECModelMap::iterator GetChildModelIterator() { return m_ChildModels.begin(); }

	void SetMotionBlurInfo(ECMActionBlurInfo* pInfo);
	ECMActionBlurInfo* GetMotionBlurInfo() { return m_pBlurInfo; }
	bool CalcMotionBlurParams();
	void TickMotionBlur(DWORD dwDeltaTime);
	void RenderMotionBlur(A3DViewport* pViewport);
	
	void RemoveMotionBlurInfo()
	{
		if (m_pBlurInfo)
		{
			if (m_pBlurInfo->m_bApplyToChildren)
			{
				for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
				{
					CECModel* pChild = it->second;
					pChild->RemoveMotionBlurInfo();
				}
			}

			delete m_pBlurInfo;
			m_pBlurInfo = NULL;
		}
	}

	bool IsScriptEventEnable() const { return m_bEnableScriptEvent; }
	void SetScriptEventEnable(bool b) { m_bEnableScriptEvent = b; }

	bool IsUsingParentSpeed() const { return m_bUseParentSpeed; }
	void SetUseParentSpeed(bool b) { m_bUseParentSpeed = b; }

	bool IsEventUsePlaySpeed() const { return m_bEventUsePlaySpeed; }
	void SetEventUsePlaySpeed(bool b) { m_bEventUsePlaySpeed = b; }

	void ReloadPixelShader();

	DWORD GetChildRenderFlag() const { return m_dwChildRenderFlag; }
	void SetChildRenderFlag(DWORD dwFlag);

	friend class A3DCombinedAction;
	friend class A3DCombActDynData;
	friend class CECModelStaticData;
	friend class GFX_INFO;
	friend class GFX_BINDING;
};
