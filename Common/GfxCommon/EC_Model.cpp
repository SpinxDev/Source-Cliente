/*
 * FILE: EC_Model.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#include <A3DEngine.h>
#include <AFileImage.h>
#include <A3DBone.h>
#include <A3DCamera.h>
#include <A3DSkinModel.h>
#include <A3DSkinModelAux.h>
#include <AMSoundBufferMan.h>
#include <A3DSkinModelAct.h>
#include <A3DConfig.h>
#include <shlwapi.h>
#include <afi.h>
#include <A3DJoint.h>

#ifdef A3D_PHYSX
	#include "A3DModelPhysSync.h"
	#include "A3DModelPhysSyncData.h"
	#include "A3DModelPhysics.h"
#endif

#include "base64.h"
#include "EC_Model.h"
#include "EC_ModelMan.h"
#include "EC_ModelHook.h"
#include "LuaUtil.h"
#include "scriptvalue.h"
#include "LuaAPI.h"
#include "LuaState.h"
#ifndef GFX_EDITOR
	#include "ConvexHullData.h"
#endif

#include "EC_ModelLua.h"


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

// 为了兼容不同国家语言code page
char* ECMPathFindFileNameA(const char* szPath)
{
	char* ret = NULL;
	int i = 0;
	char ch;

	while ((ch = szPath[i]) != 0)
	{
		if (::IsDBCSLeadByteEx(936, ch))
		{
			if (szPath[i+1] == 0)
				break;

			i += 2;
			continue;
		}

		if (ch == '\\' || ch == '/' || ch == ':')
			ret = (char*)szPath + i;

		i++;
	}

	if (ret == NULL)
		return (char*)szPath;

	return ret + 1;
}

char* ECMPathFindExtensionA(const char* szPath)
{
	char* ret = NULL;
	int i = 0;
	char ch;

	while ((ch = szPath[i]) != 0)
	{
		if (::IsDBCSLeadByteEx(936, ch))
		{
			i++;

			if (szPath[i] == 0)
				break;

			i++;
			continue;
		}

		if (ch == '.')
			ret = (char*)szPath + i;

		i++;
	}

	if (ret)
		return ret;

	return (char*)szPath + i;
}

#define MODEL_FILE_TYPE	AFILE_TEXT

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


CECModel::GFX_NOTIFY_DELAY_START_FUNC CECModel::m_NotifyDelayStartFunc = NULL;
extern float _ecm_total_tick_time;

extern void g_GfxApplyPixelShaderConsts(A3DDevice* pA3DDevice, const GfxPSConstVec& ConstsVec, DWORD dwPSTickTime, int nIndexOffset);
extern void g_GfxLoadPixelShaderConsts(AFile* pFile, AString& strPixelShader, AString& strShaderTex, GfxPSConstVec& vecPSConsts);
extern void g_GfxSavePixelShaderConsts(AFile* pFile, const AString& strPixelShader, const AString& strShaderTex, const GfxPSConstVec& vecPSConsts);

#ifdef _ANGELICA21
	#include <A3DExternalFunc.h>
	CECModel::ECM_CUSTOM_RENDER_FUNC CECModel::m_CustomRenderFunc = a3d_CustomRenderModelWithReplacePS;
#else
	CECModel::ECM_CUSTOM_RENDER_FUNC CECModel::m_CustomRenderFunc = NULL;
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


static const char* _format_file_version		= "Version: %d";
static const char* _format_skinmodel_path	= "SkinModelPath: %s";
static const char* _format_addi_skin_count	= "AddiSkinCount: %d";
static const char* _format_addi_skin_path	= "AddiSkinPath: %s";
static const char* _format_comact_count		= "ComActCount: %d";
static const char* _format_child_count		= "ChildCount: %d";
static const char* _format_child_path		= "ChildPath: %s";
static const char* _format_child_name		= "ChildName: %s";
static const char* _format_hh_name			= "HHName: %s";
static const char* _format_cc_name			= "CCName: %s";
static const char* _format_cogfx_num		= "CoGfxNum: %d";
static const char* _format_new_scale		= "NewScale: %d";
static const char* _format_bone_num			= "BoneNum: %d";
static const char* _format_bone_index		= "BoneIndex: %d";
static const char* _format_bone_scl_type	= "BoneSclType: %d";
static const char* _format_bone_scale		= "BoneScale: %f, %f, %f";
static const char* _format_org_color		= "OrgColor: %x";
static const char* _format_emissive_col		= "EmissiveCol: %x";
static const char* _format_def_speed		= "DefSpeed: %f";
static const char* _format_srcbld			= "SrcBlend: %d";
static const char* _format_destbld			= "DestBlend: %d";
static const char* _format_float			= "Float: %f";
static const char* _format_outer_num		= "OuterNum: %d";
static const char* _format_script_count		= "ScriptCount: %d";
static const char* _format_id				= "id: %d";
static const char* _format_script_lines		= "ScriptLines: %d";
static const char* _format_channel_count	= "ChannelCount: %d";
static const char* _format_channel			= "Channel: %d";
static const char* _format_channel_mask		= "ChannelMask: %d";
static const char* _format_autoupdate		= "AutoUpdata: %d";
static const char* _format_phys_file		= "PhysFileName: %s";
static const char* _format_ecmhook_count	= "ECMHookCount: %d";
static const char* _format_can_castshadow	= "CanCastShadow: %d";
static const char* _format_render_model		= "RenderModel: %d";
static const char* _format_render_edge		= "RenderEdge: %d";
static const char* _format_shaderpath		= "ShaderFile: %s";
static const char* _format_shadertex		= "ShaderTex: %s";
static const char* _format_psconstcount		= "PSConstCount: %d";
static const char* _format_psindex			= "PSConstIndex: %d";
static const char* _format_psvalue			= "PSConstValue: %f, %f, %f, %f";
static const char* _format_pstargetcount	= "PSTargetCount: %d";
static const char* _format_psinterval		= "PSInterval: %d";
static const char* _format_psloop			= "PSLoopCount: %d";

#define GFX_FADE_OUT_TIME	1000

/*
 *	V2 04.11.27
 *  Remark: Add Bind Parent
 *	V3 04.12.11
 *	Remark: Add Com Act Loops
 *	V5 04.12.20
 *	Remark: Add Child Models Hierarchy
 *	V6 05.1.4
 *	Remark: Update com action loop
 *	V7 05.1.4
 *	Remark: add event info
 *	V8 05.1.18
 *	Remark: add additional skin
 *	V9 05.1.19
 *	Remark: add per basic act loop
 *	V10 05.1.24
 *	Remark: add gfx play speed control
 *	V11 05.1.29
 *	Remark: add material change org val
 *	V12 05.2.1
 *	Remark: update blade trail
 * 	V13 05.3.26
 *	Remark: update can fadeout
 *	V14 05.4.9
 *	Remark: add co gfx
 *	V15 05.4.28
 *	Remark: update sfx info, now can fade out
 *	V16	05.5.12
 *	Remark: add bone scale
 *	V17 05.5.13
 *	Remark: use model alpha
 *	V18	05.5.18
 *	Remark: event play once in loops
 *	V19	05.6.10
 *	Remark: add hook rot angle
 *	V20	05.10.11
 *	Remark: add event timespan
 *	V21	05.10.20
 *	Remark: add blend mode
 *	V22 06.4.20
 *	Remark: gfx use outer path
 *	V23	06.5.22
 *	Remark: attack point add path and divisions attr
 *	V24	06.5.29
 *	Remark: add gfx alpha
 *	V25	06.8.21
 *	Remark: add scripts
 *	V26	06.9.25
 *	Remark: script length unlimited
 *	V27	07.1.3
 *	Remark: add def play speed
 *	V28 07.6.8
 *	Remark: add new bone scale method
 *	V29 07.7.5
 *	Remark: add scale base bone
 *	V30 07.8.20
 *	Remark: add comact rank
 *	V31	07.8.20
 *	Remark: add model channel info
 *	V32 07.8.22
 *	Remark: add channel event mask, comact event channel
 *	V33	08.1.24
 *	Remark: save ecm auto update flag
 *	V34 08.6.5
 *	Remark: add physics data
 *	V35 08.10.30
 *	Remark: add gfx relative to ECModel original flag
 *	V36 08.12.19
 *	Remark: change ACTION_INFO's m_nLoops to m_nMinLoop and m_nMaxLoop, may generate a random loop number if minLoop != maxLoop
 *	V37 08.12.23
 *	Remark: change the save method of ECM's smd filepath, version before37, save the path as (NOTION: Relative path's head NEVER has a "\", it just starts by Models\\) Models\\...\\xxx.smd
 *			After version 37, will only save the smd's filename
 *	V38 09.1.12
 *	Remark: change the save method of ECM's smd filepath, version 37 would only save the smd's filename and try to find the file in the ecm's path
 *			After version 38, will save the relative pathname of the smd file
 *	V39 09.2.12			
 *	Remark: attack point add delaytime
 *	V40 09.3.6
 *	Remark: add playspeed to A3DCombinedAction
 *	V41 09.5.21
 *	Remark: add ECModelHook to CECModelStaticData
 *	V42 09.6.4
 *	Remark: add child act transtime
 *	V43	09.11.4
 *	Remark: add can cast shadow flag
 *	V44 09.11.5
 *	Remark: add a new event type of camera control point
 *	V45 09.11.9
 *	Remark: can render skin model
 *	V46 09.11.11
 *	Remark: add accelaration to camera control point's movement
 *	V47 09.11.18
 *	Remark: big changes to camera control point's parameters
 *			Add bezier control style keypoint, keep keypoint interp mode, add camera's target point
 *	V48 09.12.21
 *	Remark: add render edge flag
 *	V49	10.1.7
 *	Remark: add a new event type of model scale change
 *			add StopChildrenAct property to A3DCombinedAction
 *			add ResetMaterialScale property to A3DCombinedAction
 *	V50 10.2.26
 *	Remark: change ecm's gfx_info fadeout property from a boolean flag to a time period
 *	V51 10.3.11
 *	Remark: a new event type of MaterialScaleTrans
 *	V52	10.3.12
 *	Remark: add ExtraEmissive Color to model file, it takes effect when loading
 *	V53 10.5.13
 *	Remark: FX_BASE_INFO add random files support
 *	V54 10.5.14
 *	Remark: GFX_INFO add delay time in order to appear a delay effect while updated by hook or model
 *	V55 10.9.19
 *	Remark: fix MaterialScaleTrans apply to child property is not saved
 *	V56	10.9.27
 *	Remark: script event now can run in config state
 *	V57 11.2.28
 *	Remark: Now A3DShader supported, but only supported in dx9 for some flags only active in it
 *	V58	11.3.15
 *	Remark: FX_BASE_INFO add custom file path method, application can change the default path
 *	V59 11.3.18
 *	Remark: Add shader consts loop property
 *	V60	11.5.31
 *	Remark: Shader info can be loaded from file
 *	V61 11.7.27
 *	Remark: Add Audio type event
 *	V62 11.10.24
 *	Remark: ECM Event add custom data property
 *	V63 11.12.22
 *	Remark: GfxScriptEvent add member 'usage' to identify event sub type
 *	V64 12.1.19
 *	Remark: SGC event add orientation
 *	V65 12.2.7
 *	Remark: AudioEventInfo add audio volume property
 *  V66 12.3.14
 *	Remark: Gfx info cogfx rot with model
 *  V67 12.4.16
 *  Remark: Bone Scale Info use file scale when Stopped
 */

DWORD EC_MODEL_CUR_VERSION = 67;

#define ECMODEL_PROJ_MAGIC (unsigned int) (('e'<<24) | ('c' << 16) | ('b' << 8) | ('p'))
#define ECMODEL_PROJ_VERSION (unsigned int) (0x00000001)

struct ECModelBrushProjHead_t
{
	unsigned int	nMagic;
	unsigned int	nVersion;
	char			szECModelFile[MAX_PATH];
	int				nFrame;
	unsigned int	nNumHull;
};

#ifndef GFX_EDITOR

	extern const char* AfxGetECMHullPath();

#endif

// script

const char* _script_descript[enumECMScriptCount] =
{
	"每次播放动作时生效",
	"每次停止动作时生效",
	"每次加载时生效",
	"每次释放时生效",
	"每个模型加载后生效",
	"每次换装备时生效",
	"每次加载时生效",
	"每次物理破碎时生效",
};

const char* _script_func_name[enumECMScriptCount] =
{
	"StartAct",
	"EndAct",
	"Init",
	"Release",
	"ModelLoaded",
	"ChangeEquip",
	"ChangeEquipTableInit",
	"OnPhysBreak",
};

const ECM_SCRIPT_VAR ecm_script_var_tb[enumECMScriptCount] =
{
	{	4,	enumECMVarModel, enumECMVarActChannel, enumECMVarActName, enumECMVarFashionMode },
	{   3,  enumECMVarModel, enumECMVarActChannel, enumECMVarActName },
	{   0	},
	{   0	},
	{	1,	enumECMVarModel	},
	{	6,	enumECMVarModel, enumECMVarEquipId, enumECMVarPathId, enumECMVarEquipFlag, enumECMVarEquipIndex, enumECMVarFashionMode },
	{	0	},
	{	4,	enumECMVarModel, enumECMVarBreakOffsetX, enumECMVarBreakOffsetY, enumECMVarBreakOffsetZ	},
};

const char* _script_var_name[enumECMVarCount] =
{
	"model",
	"act_name",
	"act_channel",
	"equip_id",
	"equip_flag",
	"fashion_mode",
	"path_id",
	"id",
	"equip_index",
	"break_offset_x",
	"break_offset_y",
	"break_offset_z",
	"caster_id",
	"casttarget_id",
	"self",
};

int ecm_get_var_count(int func)
{
	return ecm_script_var_tb[func].var_count;
}

const char* ecm_get_var_name(int func, int var_index)
{
	return _script_var_name[ecm_script_var_tb[func].var_index[var_index]];
}

inline AString _format_ecm_table_name(void* p)
{
	AString strTable;
	strTable.Format("ECM%x", (int)p);
	return strTable;
}

inline DWORD get_action_tick_time(DWORD dwUpdateTime, bool bNoAnim)
{
	return bNoAnim ? 0 : dwUpdateTime;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static int _str_compare(const void* arg1, const void* arg2)
{
	A3DCombinedAction* p1;
	A3DCombinedAction* p2;
	p1 = *(A3DCombinedAction**)arg1;
	p2 = *(A3DCombinedAction**)arg2;
	return strcmp(p1->GetName(), p2->GetName());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECModel::ChannelActNode
//	
///////////////////////////////////////////////////////////////////////////

CECModel::ChannelActNode::ChannelActNode() :
m_Rank(0),
m_pActive(0),
m_pActFlag(0),
m_dwFlagMode(0)
{
}

CECModel::ChannelActNode::~ChannelActNode()
{
	delete m_pActive;
	RemoveQueuedActs();
}

void CECModel::ChannelActNode::RemoveQueuedActs()
{
	ALISTPOSITION pos = m_QueuedActs.GetHeadPosition();
	
	while (pos)
		delete m_QueuedActs.GetNext(pos);
	
	m_QueuedActs.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECModel::DelayModeInfo
//	
///////////////////////////////////////////////////////////////////////////

void CECModel::DelayModeInfo::OnStartDelay(int nDelayTime)
{
	m_nDelayMode = CECModel::DM_PLAYSPEED_DELAY;
	m_nDelayTime = nDelayTime;
	m_nTimeElapsed = 0;
}

int CECModel::DelayModeInfo::OnUpdate(DWORD dwTickTime)
{
	if (m_nDelayMode == CECModel::DM_PLAYSPEED_JUMP)
		m_nDelayMode = CECModel::DM_PLAYSPEED_NORMAL;

	if (m_nDelayMode == CECModel::DM_PLAYSPEED_NORMAL)
		return m_nDelayMode;

	m_nTimeElapsed += dwTickTime;
	if (m_nTimeElapsed >= m_nDelayTime)
		m_nDelayMode = CECModel::DM_PLAYSPEED_JUMP;
	
	return m_nDelayMode;
}

int CECModel::DelayModeInfo::GetElapsedTime()
{
	return m_nTimeElapsed;
}

void CECModel::DelayModeInfo::OnPlayComAction()
{
	m_nDelayMode = CECModel::DM_PLAYSPEED_NORMAL;
	m_nDelayTime = 0;
	m_nTimeElapsed = 0;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECModel
//	
///////////////////////////////////////////////////////////////////////////

CECModel::CECModel()
{
	m_nId					= 0;
	m_bInheritParentId		= true;
	m_pA3DDevice			= NULL;
	m_pMapModel				= NULL;
	m_pA3DSkinModel			= NULL;
	m_bAutoUpdate			= true;
	m_dwUpdTimeCnt			= 0;
	m_bLoadSkinModel 		= false;
	m_fPlaySpeed			= 1.0f;
	m_bShown				= true;
	m_bDisableCamShake		= false;
	m_bCastShadow			= false;
	m_bSlowestUpdate		= false;
	m_iAABBType				= AABB_AUTOSEL;
	m_bCanAffectedByParent	= true;
	m_fInnerAlpha			= 1.0f;
	m_fOuterAlpha			= 1.0f;
	m_fAlpha				= 1.0f;
	m_InnerColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_OuterColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_Color.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_bAbsTrack				= false;
	m_fRootBoneScaleFactor	= 1.0f;
	m_fGfxScaleFactor		= 1.0f;
	m_bGfxScaleChanged		= false;
	m_bUpdateLightInfo		= true;
	m_bFirstActPlayed		= false;
	m_bGfxUseLod			= true;
	m_bCoGfxSleep			= false;
	m_nSfxPriority			= 0;
	m_nMainChannel			= 0;
	m_pModelPhysics			= NULL;
	m_bPhysEnabled			= true;
#ifdef GFX_EDITOR
	m_bIsDelayActive		= true;
#else
	m_bIsDelayActive		= false;
#endif
	m_nErrCode				= ECMERR_UNDEFINED;
	m_bFashionMode			= false;
	m_bCreatedByGfx			= false;
	m_pPhysBreakHandler		= NULL;
	m_pBlurInfo				= NULL;
	m_bEnableScriptEvent	= true;
	m_bUseParentSpeed		= false;
	m_bEventUsePlaySpeed	= true;
	m_pPixelShader			= NULL;
	m_dwPSTickTime			= 0;
	m_pReplacePS			= NULL;
	m_ModelAABB.Clear();

	memset(m_EventMasks, EVENT_MASK_ALL, sizeof(m_EventMasks));
	m_dwChildRenderFlag	= 0;
}

CECModel::~CECModel()
{
	Release();
}

//	Initalize model
bool CECModel::Load(
	const char* szModelFile,
	bool bLoadSkinModel,
	int iSkinFlag,
	bool bLoadChildECModel,
	bool bLoadAdditionalSkin,
	bool bIsCreateModelPhys)
{
	using namespace LuaBind;

	if (m_pMapModel)
	{
		assert(false);
		return false;
	}

	m_pMapModel = AfxGetECModelMan()->LoadModelData(szModelFile, bLoadAdditionalSkin);

	if (!m_pMapModel)
		return false;

	//	Initialize the event mask from static data
	memcpy(m_EventMasks, m_pMapModel->m_EventMasks, sizeof(m_EventMasks));

	CoGfxMap& GfxMap = m_pMapModel->m_CoGfxMap;
	CoGfxMap::iterator it = GfxMap.begin();

	for (; it != GfxMap.end(); ++it)
	{
		PGFX_INFO pSrc = it->second;
		PGFX_INFO pInfo = static_cast<PGFX_INFO>(EVENT_INFO::CloneFrom(pSrc->GetComAct(), *pSrc));

		if (!pInfo)
			continue;

		pInfo->LoadGfx();

		if (!pInfo->GetGfx())
		{
			delete pInfo;
			continue;
		}

		TransferEcmProperties(pInfo->GetGfx());
		m_CoGfxMap[it->first] = pInfo;
	}

	// 可以为空模型
	if (!m_pMapModel->m_strSkinModelPath.IsEmpty())
	{
		if (af_IsFileExist(m_pMapModel->m_strSkinModelPath))
			m_pA3DSkinModel = AfxLoadA3DSkinModel(m_pMapModel->m_strSkinModelPath, iSkinFlag);
		else
			m_pA3DSkinModel = NULL;

		bool single = (m_pMapModel->m_strSkinModelPath.FindOneOf("/\\") == -1);


		// if load failed, suppose the m_strSkinModelPath is a single file name 
		// and try to find the file in the ecm's path
		if (!m_pA3DSkinModel && single) 
		{
			char szBuf[MAX_PATH];
			strcpy(szBuf, szModelFile);
			strcpy(ECMPathFindFileNameA(szBuf), m_pMapModel->m_strSkinModelPath);
			m_pA3DSkinModel = AfxLoadA3DSkinModel(szBuf, iSkinFlag);
		}

		if (!m_pA3DSkinModel)
		{
			m_nErrCode = ECMERR_SMDLOADFAILED;
			Release();
			return false;
		}

		//	We want to update child model's skin model in child CECModels
		//	so disable internal update child model by this flag
		//	but we need to update the whole AABB by ourself
		m_pA3DSkinModel->SetAutoUpdateChildFlag(false);
	}
	else
		m_pA3DSkinModel = NULL;

	m_pA3DDevice = AfxGetA3DDevice();
	ReloadPixelShader();
	m_bLoadSkinModel = (m_pA3DSkinModel != 0);
	m_bAutoUpdate = m_pMapModel->m_bAutoUpdate;

	UpdateBaseActTimeSpan();
	ShowCoGfx(true);

	if (g_pA3DConfig->GetFlagNewBoneScale())
		UpdateBoneScaleEx();
	else
		UpdateBoneScales();

#ifdef GFX_EDITOR

	if (bLoadAdditionalSkin && m_pA3DSkinModel)
	{
		AString strBase = "Models\\";

		for (size_t i = 0; i < m_pMapModel->m_AdditionalSkinLst.size(); i++)
		{
			AString strSkin = strBase + m_pMapModel->m_AdditionalSkinLst[i];

			if (m_pA3DSkinModel->AddSkinFile(strSkin, true) < 0)
				continue;
		}
	}

#endif

	ResetMaterialOrg();


	ResetMaterialScale();

	if (bLoadChildECModel)
		LoadChildModels();

	UpdateModelAABB();

	if (m_pMapModel->m_ScriptEnable[enumECMScriptModelLoaded])
	{
		abase::vector<CScriptValue> args, results;
		args.push_back((void *)this);
		m_pMapModel->m_ScriptMemTbl.Call(_script_func_name[enumECMScriptModelLoaded], args, results);
	}

	if (m_pA3DSkinModel)
	{
		if (!m_pMapModel->m_bChannelInfoInit)
		{
			m_pMapModel->m_bChannelInfoInit = true;
			A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();

			if (pSke)
			{
				for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
				{
					ActChannelInfo* pChannel = m_pMapModel->m_ChannelInfoArr[i];

					if (!pChannel)
						continue;
					
					for (size_t j = 0; j < pChannel->bone_names.size(); j++)
					{
						int index;
						A3DBone* pBone = pSke->GetBone(pChannel->bone_names[j], &index);
						
						if (!pBone)
							continue;
						
						A3DAnimJoint* pJoint = pBone->GetAnimJointPtr();
						pSke->GetJoint(pJoint->GetName(), &index);
						pChannel->joint_indices.push_back(index);						
					}
					
				}
			}
		}

		for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			ActChannelInfo* pChannel = m_pMapModel->m_ChannelInfoArr[i];

			if (pChannel && pChannel->joint_indices.size())
				m_pA3DSkinModel->BuildActionChannel(i, pChannel->joint_indices.size(), pChannel->joint_indices.begin());
		}

		//	Create model physics system
		if (bIsCreateModelPhys)
			CreateModelPhysics();
	}

	return true;
}

void CECModel::LoadChildModels()
{
	CECModelStaticData::ChildInfoArray& ChildInfoArray = m_pMapModel->m_ChildInfoArray;
	for (size_t i = 0; i < ChildInfoArray.size(); i++)
	{
		const CECModelStaticData::ChildInfo* pChild = ChildInfoArray[i];

		AddChildModel(
			pChild->m_strName,
			false,
			pChild->m_strHHName,
			pChild->m_strPath,
			pChild->m_strCCName);
	}
}

bool CECModel::LoadBoneScaleInfo(const char* szFile)
{
	if (!g_pA3DConfig->GetFlagNewBoneScale())
		return false;

	AFileImage file;

#ifdef _ANGELICA22
	if (!file.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
#else
	if (!file.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY))
#endif
		return false;

	BoneScaleExArray arr;
	char* name = 0;
	bool ok = false;

	try
	{
		DWORD len;
		int num;
		file.Read(&num, sizeof(num), &len);

		if (len != sizeof(num))
			throw "";

		for (int i = 0; i < num; i++)
		{
			BoneScaleEx* p = new BoneScaleEx;
			file.Read(p, sizeof(BoneScaleEx), &len);

			if (len != sizeof(BoneScaleEx))
			{
				delete p;
				throw "";
			}

			arr.push_back(p);
		}

		file.Read(&num, sizeof(num), &len);

		if (len != sizeof(num))
			throw "";

		if( num )
		{
			name = new char[num+1];
			name[num] = 0;
			file.Read(name, num, &len);

			if ((int)len != num)
				throw "";
		}

		UpdateBoneScaleEx(arr, name);
		ok = true;
	}
	catch(const char*)
	{
	}

	for (size_t i = 0; i < arr.size(); i++)
		delete arr[i];

	delete[] name;
	file.Close();
	return ok;
}

//	Release model
void CECModel::Release()
{
	m_aComboModels.RemoveAll();

	for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		m_ChannelActs[i].Release();

	// must release child models before release self
	for (ECModelMap::iterator child_it = m_ChildModels.begin(); child_it != m_ChildModels.end(); ++child_it)
	{
		child_it->second->Release();
		delete child_it->second;
	}

	m_ChildModels.clear();

#ifdef A3D_PHYSX

	// must release A3DModelPhysics before A3DSkinModel
	// because A3DModelPhysics contains a lot of A3DSkinModel information
	// otherwise, there would be a problem when we use the A3DSkinModel information in the A3DModelPhysics::Release()

	A3DRELEASE(m_pModelPhysics);

#endif

	if (m_bLoadSkinModel && m_pA3DSkinModel)
	{
		AfxReleaseA3DSkinModel(m_pA3DSkinModel);
		m_pA3DSkinModel = NULL;
		m_bLoadSkinModel = false;
	}

	for (CoGfxMap::iterator itGfx = m_CoGfxMap.begin(); itGfx != m_CoGfxMap.end(); ++itGfx)
		delete itGfx->second;

	m_CoGfxMap.clear();

	// must after act being released
	ALISTPOSITION pos = m_FadeOutSfxLst.GetHeadPosition();
	while (pos) AfxReleaseSoundNonLoop(m_FadeOutSfxLst.GetNext(pos));
	m_FadeOutSfxLst.RemoveAll();

	if (m_pMapModel)
	{
		AfxGetECModelMan()->Lock();
		AfxGetECModelMan()->ReleaseModel(m_pMapModel->m_strFilePath);
		AfxGetECModelMan()->Unlock();
		m_pMapModel = NULL;
	}

	m_bFashionMode = false;
	m_EquipSlotMap.clear();
	m_SlotEquipMap.clear();

	if (m_pBlurInfo)
	{
		delete m_pBlurInfo;
		m_pBlurInfo = NULL;
	}

	if (m_pPixelShader)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseShader(&m_pPixelShader);
		m_pPixelShader = NULL;
	}

	RemoveReplaceShader();
}

bool CECModel::SetSkinModel(A3DEngine* pEngine, const char* szFile)
{
	A3DSkinModel* pSkin = NULL;

	if (strlen(szFile))
	{
		pSkin = new A3DSkinModel;

		if (!pSkin->Init(pEngine) || !pSkin->Load(szFile))
		{
			delete pSkin;
			return false;
		}

		pSkin->SetAutoUpdateChildFlag(false);
	}

	if (!m_pMapModel)
	{
		m_pMapModel = new CECModelStaticData;
		m_pMapModel->m_strFilePath = "temp.ecm";
		AfxGetECModelMan()->Lock();
		AfxGetECModelMan()->AddModel(m_pMapModel->m_strFilePath, m_pMapModel);
		AfxGetECModelMan()->Unlock();
	}

	if (m_pA3DSkinModel)
	{
		m_pA3DSkinModel->Release();
		delete m_pA3DSkinModel;
	}

	m_pA3DSkinModel = pSkin;
	m_bLoadSkinModel = (m_pA3DSkinModel != 0);
	// Still get the relative path of the .smd file, but when saving the .ecm file, we would only save the filename of the .smd file
	m_pMapModel->m_strSkinModelPath = szFile;
	
	//	Create model physics system
	CreateModelPhysics();

	return true;
}

const A3DSHADER& CECModel::GetBlendMode() const { return m_pMapModel->m_BlendMode; }
void CECModel::SetBlendMode(const A3DSHADER& bld) { m_pMapModel->m_BlendMode = bld; }
A3DCOLOR CECModel::GetOrgColor() const { return m_pMapModel->m_OrgColor; }
void CECModel::SetOrgColor(A3DCOLOR cl) { m_pMapModel->m_OrgColor = cl; }
A3DCOLOR CECModel::GetExtraEmissive() const { return m_pMapModel->m_EmissiveColor; }
void CECModel::SetExtraEmissive(A3DCOLOR cl) { m_pMapModel->m_EmissiveColor = cl; }
float* CECModel::GetOuterData() { return m_pMapModel->m_OuterData; }
float CECModel::GetDefPlaySpeed() const { return m_pMapModel->m_fDefPlaySpeed; }
void CECModel::SetDefPlaySpeed(float fSpeed) { m_pMapModel->m_fDefPlaySpeed = fSpeed; }
const bool CECModel::CanCastShadow() const { return m_pMapModel->m_bCanCastShadow; }
void CECModel::SetCanCastShadow(bool b) { m_pMapModel->m_bCanCastShadow = b; }
const bool CECModel::CanRenderSkinModel() const { return m_pMapModel->m_bRenderSkinModel; }
void CECModel::SetRenderSkinModel(bool b) { m_pMapModel->m_bRenderSkinModel = b; }
bool CECModel::CanRenderEdge() const { return m_pMapModel->m_bRenderEdge; }
void CECModel::SetRenderEdge(bool b) { m_pMapModel->m_bRenderEdge = b; }

//	Create model physical sync
bool CECModel::CreateModelPhysics()
{
	if (!m_bPhysEnabled)
		return true;

#ifdef A3D_PHYSX

	A3DRELEASE(m_pModelPhysics);

	if (m_pA3DSkinModel && gGetAPhysXScene())
	{
		m_pModelPhysics = new A3DModelPhysics;

		if (m_pModelPhysics->Init(gGetAPhysXScene(), this))
		{
			if (m_pMapModel->m_pPhysSyncData)
			{
				m_pModelPhysics->CreateModelSync(m_pMapModel->m_pPhysSyncData);
			}
		}
	}

#endif

	return true;
}

void CECModel::ResetMaterialOrg()
{
	if (m_pA3DSkinModel)
	{
		A3DSkinModel::LIGHTINFO LightInfo;
		memset(&LightInfo, 0, sizeof (LightInfo));
		const A3DLIGHTPARAM& lp = AfxGetLightparam();

		LightInfo.colAmbient	= m_pA3DDevice->GetAmbientValue();
		LightInfo.vLightDir		= lp.Direction;
		LightInfo.colDirDiff	= lp.Diffuse;
		LightInfo.colDirSpec	= lp.Specular;
		LightInfo.bPtLight		= false;

		m_pA3DSkinModel->SetLightInfo(LightInfo);
		m_pA3DSkinModel->SetSrcBlend(m_pMapModel->m_BlendMode.SrcBlend);
		m_pA3DSkinModel->SetDstBlend(m_pMapModel->m_BlendMode.DestBlend);
		m_pA3DSkinModel->EnableSpecular(true);
		m_pA3DSkinModel->SetExtraEmissive(m_pMapModel->m_EmissiveColor);
		m_pA3DSkinModel->Update(5);
	}
}

void CECModel::ResetMaterialScale()
{
	A3DCOLORVALUE cl(1.0f);
	if (m_pMapModel)
		cl = m_pMapModel->m_OrgColor;

	SetInnerAlpha(cl.a);
	cl.a = 1.0f;
	SetInnerColor(cl);
	
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		if (it->second->m_bCanAffectedByParent)
			it->second->ResetMaterialScale();
		
	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel)
			continue;

		pComboModel->ResetMaterialScale();
	}
}

void CECModel::SetTransparent(float fTransparent)
{
	m_fOuterAlpha = (fTransparent == -1.0f ? 1.0f : 1.0f - fTransparent);
	m_fAlpha = m_fOuterAlpha * m_fInnerAlpha;
	
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetTransparent(GetTransparent());
	
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetTransparent(fTransparent);
	
	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel)
			continue;

		pComboModel->SetTransparent(fTransparent);
	}
}

void CECModel::SetInnerAlpha(float fInner)
{
	m_fInnerAlpha = fInner;
	m_fAlpha = m_fOuterAlpha * m_fInnerAlpha;
	
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetTransparent(GetTransparent());

	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel || !pComboModel->GetA3DSkinModel())
			continue;

		pComboModel->GetA3DSkinModel()->SetTransparent(GetTransparent());
	}
}

void CECModel::SetSrcBlend(A3DBLEND srcBlend)
{
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetSrcBlend(srcBlend);

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetSrcBlend(srcBlend);
}

void CECModel::SetDstBlend(A3DBLEND dstBlend)
{
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetDstBlend(dstBlend);

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetDstBlend(dstBlend);
}

void CECModel::SetColor(const A3DCOLORVALUE& cl)
{
	m_OuterColor = cl;
	m_Color = m_OuterColor * m_InnerColor;
	m_Color.Clamp();
	ApplyColor();

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetColor(cl);

	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel)
			continue;

		pComboModel->SetColor(cl);
	}
}

void CECModel::SetInnerColor(const A3DCOLORVALUE& cl)
{
	m_InnerColor = cl;
	m_Color = m_OuterColor * m_InnerColor;
	m_Color.Clamp();
	ApplyColor();
}

void CECModel::ApplyColor()
{
	static const A3DCOLORVALUE _scale(1.f, 1.f, 1.f, 1.f);

	if (m_pA3DSkinModel)
	{
		if (m_Color == _scale)
		{
			m_pA3DSkinModel->SetMaterialMethod(A3DSkinModel::MTL_ORIGINAL);
			m_pA3DSkinModel->SetMaterialScale(_scale);
		}
		else
		{
			m_pA3DSkinModel->SetMaterialMethod(A3DSkinModel::MTL_SCALED);
			m_pA3DSkinModel->SetMaterialScale(m_Color);
		}
	}
}

void CECModel::Show(bool bShow, bool bAffectChild/* = true*/)
{
	if (m_bShown == bShow)
		return;
	
	m_bShown = bShow;
	
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->Hide(!bShow);
	
	ShowCoGfx(bShow);
	
	if (!bAffectChild)
		return;

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		if (it->second->m_bCanAffectedByParent)
			it->second->Show(bShow, true);

	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel)
			continue;

		pComboModel->Show(bShow, true);
	}	
}

//	Scale specified bone
bool CECModel::ScaleBone(const char* szBone, int iScaleType, const A3DVECTOR3& vScale)
{
	if (!m_pA3DSkinModel)
		return false;

	A3DBone* pBone = m_pA3DSkinModel->GetSkeleton()->GetBone(szBone, NULL);

	if (!pBone)
		return false;

	switch (iScaleType)
	{
	case SCALE_NONE:	iScaleType = A3DBone::SCALE_NONE;	break;
	case SCALE_WHOLE:	iScaleType = A3DBone::SCALE_WHOLE;	break;
	case SCALE_LOCAL:	iScaleType = A3DBone::SCALE_LOCAL;	break;
	default:
		return false;
	}

	pBone->SetScaleType(iScaleType);
	pBone->SetScaleFactor(vScale);

	return true;
}

bool CECModel::ScaleBoneEx(const char* szBone, const A3DVECTOR3& vScale)
{
	if (!m_pA3DSkinModel)
		return false;

	A3DBone* pBone = m_pA3DSkinModel->GetSkeleton()->GetBone(szBone, NULL);

	if (!pBone)
		return false;

	pBone->SetLocalScale(vScale.x, vScale.y);
	pBone->SetWholeScale(vScale.z);
	return true;
}

void CECModel::CalcFootOffset(const char* szBaseBone)
{
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->GetSkeleton()->CalcFootOffset(szBaseBone);
}

DWORD CECModel::GetComActTimeSpanByName(const AString& strName)
{
	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
	CombinedActMap::iterator it = ActionMap.find(strName);
	if (it == ActionMap.end()) return 0;
	return it->second->GetMinComActTimeSpan();
}

void CECModel::AddCombinedAction(A3DCombinedAction* pAct)
{
	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
	ActionMap[AString(pAct->GetName())] = pAct;
}

void CECModel::RemoveCombinedAction(const AString& strName)
{
	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
	CombinedActMap::iterator it = ActionMap.find(strName);
	if (it == ActionMap.end()) return;
	delete it->second;
	ActionMap.erase(strName);
}

bool CECModel::RenameCombinedAction(A3DCombinedAction* pAct, const AString& strNewName)
{
	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
	CombinedActMap::iterator it = ActionMap.find(strNewName);
	if (it != ActionMap.end()) return false;
	ActionMap.erase(pAct->GetName());
	ActionMap[strNewName] = pAct;
	pAct->SetName(strNewName);
	return true;
}

//	Get bone's scale information
bool CECModel::GetBoneScaleInfo(const char* szBone, int* piScaleType, A3DVECTOR3& vScale)
{
	if (!m_pA3DSkinModel)
		return false;

	A3DBone* pBone = m_pA3DSkinModel->GetSkeleton()->GetBone(szBone, NULL);

	if (!pBone)
		return false;

	int iScaleType;

	switch (pBone->GetScaleType())
	{
	case A3DBone::SCALE_NONE:	iScaleType = SCALE_NONE;	break;
	case A3DBone::SCALE_WHOLE:	iScaleType = SCALE_WHOLE;	break;
	case A3DBone::SCALE_LOCAL:	iScaleType = SCALE_LOCAL;	break;
	default:
		return false;
	}

	*piScaleType = iScaleType;
	vScale = pBone->GetScaleFactor();

	return true;
}

void CECModel::SetBoneTransFlag(const char* szBone, bool bFlag)
{
	if (!m_pA3DSkinModel)
		return;

	A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();

	if (!pSke)
		return;

	A3DBone* pBone = pSke->GetBone(szBone, NULL);

	if (pBone)
		pBone->SetTransMask(bFlag ? A3DBone::TRANS_ALL : 0);
}

//	Add a new skin
bool CECModel::AddSkinFile(const char* szSkinFile)
{
	if (!m_pA3DSkinModel)
		return false;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		return m_pModelPhysics->AddSkinFile(szSkinFile);
	}

#endif	//	A3D_PHYSX

	int iSkin = m_pA3DSkinModel->AddSkinFile(szSkinFile, true);
	if (iSkin < 0)
		return false;

	return true;
}

//	Add a new skin
bool CECModel::AddSkin(A3DSkin* pSkin, bool bAutoRem/* true */)
{
	if (!m_pA3DSkinModel)
		return false;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		return m_pModelPhysics->AddSkin(pSkin, bAutoRem);
	}

#endif	//	A3D_PHYSX

	int iSkin = m_pA3DSkinModel->AddSkin(pSkin, bAutoRem);
	if (iSkin < 0)
		return false;

	return true;
}

//	Replace a skin
bool CECModel::ReplaceSkinFile(int iIndex, const char* szSkinFile, bool bAutoRem/* true */)
{
	if (!m_pA3DSkinModel)
		return false;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		return m_pModelPhysics->ReplaceSkinFile(iIndex, szSkinFile, bAutoRem);
	}

#endif	//	A3D_PHYSX

	if (!m_pA3DSkinModel->ReplaceSkinFile(iIndex, szSkinFile, bAutoRem))
		return false;

	return true;
}

//	Replace a skin
bool CECModel::ReplaceSkin(int iIndex, A3DSkin* pSkin, bool bAutoRem/* true */)
{
	if (!m_pA3DSkinModel)
		return false;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		return m_pModelPhysics->ReplaceSkin(iIndex, pSkin, bAutoRem);
	}

#endif	//	A3D_PHYSX

	if (!m_pA3DSkinModel->ReplaceSkin(iIndex, pSkin, bAutoRem))
		return false;

	return true;
}

//	Remove a skin item, this operation release both skin anit skin item
void CECModel::RemoveSkinItem(int iItem)
{
	if (!m_pA3DSkinModel)
		return;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		m_pModelPhysics->RemoveSkinItem(iItem);
		return;
	}

#endif	//	A3D_PHYSX

	A3DSkin* pOldSkin = NULL;
	A3DSkinModel::SKIN* pSkinItem = m_pA3DSkinModel->GetSkinItem(iItem);

	if (pSkinItem->pA3DSkin)
		pOldSkin = pSkinItem->pA3DSkin;

	m_pA3DSkinModel->RemoveSkinItem(iItem);
}

//	Release all skins
void CECModel::ReleaseAllSkins()
{
	if (!m_pA3DSkinModel)
		return;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		m_pModelPhysics->ReleaseAllSkins();
		return;
	}

#endif	//	A3D_PHYSX

	m_pA3DSkinModel->ReleaseAllSkins();
}

//	Get skin
A3DSkin* CECModel::GetA3DSkin(int iIndex)
{
	return m_pA3DSkinModel ? m_pA3DSkinModel->GetA3DSkin(iIndex) : NULL;
}

//	Show / Hide skin
void CECModel::ShowSkin(int iIndex, bool bShow)
{
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->ShowSkin(iIndex, bShow);
}

//	GetSkinShowFlag
bool CECModel::IsSkinShown(int iIndex)
{
	if (!m_pA3DSkinModel)
		return false;

	A3DSkinModel::SKIN* pSkinItem = m_pA3DSkinModel->GetSkinItem(iIndex);
	ASSERT(pSkinItem);
	return pSkinItem->bRender;
}

//	Open one cloth skin
bool CECModel::OpenClothSkin(int iSkinSlot, int iLinkType/* 0 */)
{
#ifdef A3D_PHYSX

	if (m_pModelPhysics)
		return m_pModelPhysics->OpenClothSkin(iSkinSlot, iLinkType);

#endif	//	A3D_PHYSX

	return false;
}

//	Get skin number
int CECModel::GetSkinNum()
{
	return m_pA3DSkinModel ? m_pA3DSkinModel->GetSkinNum() : 0;
}

//	Open all clothes
bool CECModel::OpenAllClothes(int iLinkType/* 0 */)
{
#ifdef A3D_PHYSX

	if (!m_pA3DSkinModel || !m_pModelPhysics)
		return false;

	int i, iNumSlot = m_pA3DSkinModel->GetSkinNum();
	for (i=0; i < iNumSlot; i++)
		m_pModelPhysics->OpenClothSkin(i, iLinkType);

#endif	//	A3D_PHYSX

	return true;
}

//	Close one cloth skin
void CECModel::CloseClothSkin(int iSkinSlot, bool bCompletely/* false */)
{
#ifdef A3D_PHYSX

	if (m_pModelPhysics)
		m_pModelPhysics->CloseClothSkin(iSkinSlot, bCompletely);

#endif	//	A3D_PHYSX
}

//	Close all clothes
void CECModel::CloseAllClothes(bool bCompletely/* false */)
{
#ifdef A3D_PHYSX

	if (!m_pA3DSkinModel || !m_pModelPhysics)
		return;

	int i, iNumSlot = m_pA3DSkinModel->GetSkinNum();

	for (i=0; i < iNumSlot; i++)
		m_pModelPhysics->CloseClothSkin(i, bCompletely);

#endif	//	A3D_PHYSX
}

//	Open child model link
bool CECModel::OpenChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher)
{
#ifdef A3D_PHYSX

	if (!szParentHook || !szChildAttacher)
		return false;

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return false;

	A3DModelPhysics* pPhys = pModel->GetModelPhysics();
	if (!pPhys->OpenPhysLinkAsChildModel(szParentHook, szChildAttacher))
		return false;

	//if (pPhys->GetPhysState() != A3DModelPhysSync::PHY_STATE_SIMULATE)
	//	pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_SIMULATE);

	//	Why do the copy? because the next operation of clear() may make the passed in parameters invalid
	ChildModelLinkInfo linkInfo(szParentHook, szChildAttacher);
	pModel->m_PhysHookAttachLinks.clear();
	pModel->m_PhysHookAttachLinks.push_back(linkInfo);
	return true;
	
#endif

	return false;
}

//	Add child model link (Must first open one child model phys link, then use add, the child attacher must not be used)
bool CECModel::AddChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher)
{
#ifdef A3D_PHYSX

	if (!szParentHook || !szChildAttacher)
		return false;

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return false;

	A3DModelPhysics* pPhys = pModel->GetModelPhysics();
	if (!pPhys->AddPhysLinkAsChildModel(szParentHook, szChildAttacher))
		return false;

	//if (pPhys->GetPhysState() != A3DModelPhysSync::PHY_STATE_SIMULATE)
	//	pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_SIMULATE);

	ChildModelLinkInfo linkInfo(szParentHook, szChildAttacher);
	pModel->m_PhysHookAttachLinks.push_back(linkInfo);
	return true;

#endif

	return false;
}

//	Remove child model link (find the link between szParentHook and szChildAttacher, if there is one, remove it)
bool CECModel::RemoveChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher)
{
#ifdef A3D_PHYSX

	if (!szParentHook || !szChildAttacher)
		return false;

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return false;

	A3DModelPhysics* pPhys = pModel->GetModelPhysics();
	if (!pPhys->RemovePhysLinkAsChildModel(szParentHook, szChildAttacher))
		return false;

	//if (pPhys->GetPhysState() != A3DModelPhysSync::PHY_STATE_ANIMATION)
	//	pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);

	ChildModelLinkInfoArray& aChildModelLinks = pModel->m_PhysHookAttachLinks;
	for (ChildModelLinkInfoArray::iterator ent_itr = aChildModelLinks.begin()
		; ent_itr != aChildModelLinks.end()
		; ++ent_itr)
	{
		if (ent_itr->m_strChildAttacher.Compare(szChildAttacher) != 0
			|| ent_itr->m_strParentHook.Compare(szParentHook) != 0)
			continue;

		aChildModelLinks.erase(ent_itr);
		break;		
	}

	return true;

#endif

	return false;

}

//	Close child model link
void CECModel::CloseChildModelLink(const char* szHanger)
{
#ifdef A3D_PHYSX

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return;

	A3DModelPhysics* pPhys = pModel->GetModelPhysics();
	pPhys->ClosePhysLinkAsChildModel();

	//if (pPhys->GetPhysState() != A3DModelPhysSync::PHY_STATE_ANIMATION)
	//	pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);

	pModel->m_PhysHookAttachLinks.clear();

#endif
}

//	Child model change physical link
void CECModel::ChangeChildModelLink(const char* szHanger, const char* szNewHook)
{
#ifdef A3D_PHYSX

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return;

	ChildModelLinkInfoArray& aLinkInfos = pModel->m_PhysHookAttachLinks;
	if (aLinkInfos.empty())
		return;

	for (ChildModelLinkInfoArray::iterator itr = aLinkInfos.begin()
		; itr != aLinkInfos.end()
		; ++itr)
	{
		pModel->GetModelPhysics()->ChangePhysLinkAsChildModel(itr->m_strChildAttacher, szNewHook);
		itr->m_strParentHook = szNewHook;
	}

#endif // A3D_PHYSX
}

//	Change child model physical state
void CECModel::ChangeChildModelPhysState(const char* szHanger, int iState)
{
#ifdef A3D_PHYSX

	if (iState != A3DModelPhysSync::PHY_STATE_ANIMATION
		&& iState != A3DModelPhysSync::PHY_STATE_PARTSIMULATE
		&& iState != A3DModelPhysSync::PHY_STATE_SIMULATE)
		return;
	
	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return;

	pModel->GetModelPhysics()->ChangePhysState(iState);

#endif
}

//	Change all child model physical state
void CECModel::ChangeChildModelPhysState(int iState)
{
#ifdef A3D_PHYSX

	for (ECModelMap::iterator itr = m_ChildModels.begin()
		; itr != m_ChildModels.end()
		; ++itr)
	{
		ChangeChildModelPhysState(itr->first, iState);
	}

#endif
}

//	Set ECModel's collision channel, call this after A3DModelPhysics::Bind() or after EnablePhysSystem()
bool CECModel::SetCollisionChannel(int iMethod, CECModel* pParent/* = NULL*/, bool bSetChildCollisionChannel /* = true */)
{
#ifdef A3D_PHYSX

	if (!m_pModelPhysics)
		return false;

	A3DModelPhysics* pParentPhysics = NULL;
	if (pParent)
		pParentPhysics = pParent->GetModelPhysics();

	bool bRes = m_pModelPhysics->SetModelCollisionChannel(iMethod, pParentPhysics);

	if(bSetChildCollisionChannel)
	{
		for(int i=0; i<GetChildCount(); i++)
		{
			GetChildModel(i)->SetCollisionChannel(COLLISION_CHANNEL_AS_PARENT, this);
		}

	}


	return bRes;

#else

	return false;

#endif

}

int CECModel::GetCollisionChannel() const
{
#ifndef A3D_PHYSX
	return -1;
#else
	if (!m_pModelPhysics)
		return -1;

	APhysXU32 nCNL = m_pModelPhysics->GetModelCollisionChannel();
	int nRtnCNL = nCNL;

	switch(nCNL)
	{
	case APX_COLLISION_CHANNEL_COMMON:
		nRtnCNL = COLLISION_CHANNEL_AS_COMMON;
		break;
	case APX_COLLISION_CHANNEL_CC_COMMON_HOST:
		nRtnCNL = COLLISION_CHANNEL_AS_CC_COMMON_HOST;
		break;
	case APX_COLLISION_CHANNEL_COLLIDE_ALL:
	case APX_COLLISION_CHANNEL_CC_SKIP_COLLISION:
	case APX_COLLISION_CHANNEL_DYNAMIC_CC:
		assert(!"not supported yet!");
		break;
	default:
		nRtnCNL = COLLISION_CHANNEL_INDEPENDENT;
	}
	return nRtnCNL;
#endif
}

//	Break a joint (Break a joint if exist)
void CECModel::BreakJoint(const char * szJointName)
{
#ifdef A3D_PHYSX

	if (!m_pModelPhysics)
		return;

	m_pModelPhysics->BreakJoint(szJointName);

#endif
}

//	Break joint by bone
void CECModel::BreakJointByBone(const char * szBoneName)
{
#ifdef A3D_PHYSX

	if (!m_pModelPhysics)
		return;

	m_pModelPhysics->BreakJointByBone(szBoneName);

#endif
}

//	Apply force
bool CECModel::AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist, int iPhysForceType, float fSweepRadius /*= 0.0f*/)
{
#ifdef A3D_PHYSX

	if (!m_pModelPhysics)
		return false;

	return m_pModelPhysics->AddForce(vStart, vDir, fForceMagnitude, fMaxDist, iPhysForceType, fSweepRadius);

#endif

	return false;
}

void CECModel::SetClothWindAcceleration(const A3DVECTOR3& vWindAcceleration)
{

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
		m_pModelPhysics->SetClothWindAcceleration(vWindAcceleration);

#endif

}

//	Creator of EC_Model could set a break handler here
//	which could be called back when PhysX makes a break to this model.
void CECModel::SetPhysBreakHandler(ON_PHYS_BREAK pPhysBreakHandler)
{
	m_pPhysBreakHandler = pPhysBreakHandler;
}

//	This method is called back by the lower level objects in (A3DModelPhysSync::OnBreak)
//	User should not call this method directly
void CECModel::OnPhysBreak(const PhysBreakInfo& physBreakInfo)
{
#ifdef A3D_PHYSX

	A3DVECTOR3 vBreakPosOffset(0.0f);
	if(m_pA3DSkinModel)
	{
		vBreakPosOffset = physBreakInfo.mBreakPos - m_pA3DSkinModel->GetPos();
	}

	//	回调脚本
	if (m_pMapModel)
		m_pMapModel->OnScriptPhysBreak(this, vBreakPosOffset.x, vBreakPosOffset.y, vBreakPosOffset.z);

	//	回调Handler
	if (m_pPhysBreakHandler)
		(m_pPhysBreakHandler)(this);

#endif
}

//	Enable physical system
bool CECModel::EnablePhysSystem(bool bHardLink, bool bRecur /*= false*/, bool bOpenCloth /* = true*/)
{
#ifdef A3D_PHYSX

	m_bPhysEnabled = true;

	//	Create model physics system
	if (!CreateModelPhysics())
		return false;

	if (bOpenCloth)
	{
		//	Open all clothes
		int iClothLinkType = (bHardLink)?2:1;		// how about the saving default type?
		OpenAllClothes(iClothLinkType);
	}

	if (!bRecur)
		return true;

	//	When recur this function on child models
	for (ECModelMap::iterator child_itr = m_ChildModels.begin()
		; child_itr != m_ChildModels.end()
		; ++child_itr)
	{
		CECModel* pModel = child_itr->second;
		ASSERT( pModel );

		if (!pModel->EnablePhysSystem(bHardLink, bRecur, bOpenCloth))
			continue;

		const ChildModelLinkInfoArray& aLinkInfos = pModel->m_PhysHookAttachLinks;
		if (!TryOpenChildModelLinks(child_itr->first, aLinkInfos))
			continue;
	}

	return true;

#else

	return false;

#endif	//	A3D_PHYSX
}

//	Try to open all child model links
bool CECModel::TryOpenChildModelLinks(const char* szHanger, const ChildModelLinkInfoArray& aChildModelLinks)
{

#ifdef A3D_PHYSX

	ChildModelLinkInfoArray::const_iterator itr = aChildModelLinks.begin();
	if (itr == aChildModelLinks.end())
		return false;

	if (!OpenChildModelLink(szHanger, itr->m_strParentHook, itr->m_strChildAttacher))
		return false;

	for ( ++itr
		; itr != aChildModelLinks.end()
		; ++itr)
	{
		if (!AddChildModelLink(szHanger, itr->m_strParentHook, itr->m_strChildAttacher))
			return false;
	}

	return true;

#endif

	return false;

}

//	Disable physical system
void CECModel::DisablePhysSystem()
{
#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		//	Close all clothes
		CloseAllClothes(true);
		A3DRELEASE(m_pModelPhysics);
	}

	m_bPhysEnabled = false;

#endif
}

A3DMATRIX4 CECModel::GetGfxBindHookTransMat(GFX_INFO* pInfo, bool& bIsHookMat)
{
	ASSERT(pInfo);
	bIsHookMat = false;
	if (!pInfo || !m_pA3DSkinModel)
		return GetAbsTM();
	else
	if (pInfo->GetHookName().IsEmpty())
	{
		A3DMATRIX4 mat;
		if (!pInfo->IsRotWithModel())
		{
			mat.Identity();
			mat.SetRow(3, GetAbsTM().GetRow(3));
		}
		else
		{
			mat = GetAbsTM();
		}
		return mat;
	}
	
	if (pInfo->IsUseECMHook()) 
	{
		CECModelHook* pECMHook = GetECMHook(pInfo->GetHookName());
		if (pECMHook)
			return pECMHook->GetRelativeMatrix() * GetAbsTM();
	}
	
	// else 笑傲需要2者都要找
	A3DSkeletonHook* pSkeletonHook = m_pA3DSkinModel->GetSkeletonHook(pInfo->GetHookName(), true);
	if (pSkeletonHook)
	{
		bIsHookMat = true;
		return pSkeletonHook->GetAbsoluteTM();
	}
	
	return GetAbsTM();
}

static bool _VectorCompare(const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	return fabs(v1.x - v2.x) >= 0.0001
		|| fabs(v1.y - v2.y) >= 0.0001
		|| fabs(v1.z - v2.z) >= 0.0001;
}

//-----------------------------------------------------------------------
static A3DQUATERNION Slerp (float fT, const A3DQUATERNION& rkP,
							  const A3DQUATERNION& rkQ, bool shortestPath)
{
	float fCos = DotProduct(rkP, rkQ);
	A3DQUATERNION rkT;
	
	// Do we need to invert rotation?
	if (fCos < 0.0f && shortestPath)
	{
		fCos = -fCos;
		rkT = rkQ * -1.f;
	}
	else
	{
		rkT = rkQ;
	}
	
	if (fabs(fCos) < 1 - 1e-03)
	{
		// Standard case (slerp)
		float fSin = (float)sqrt(1.f - (fCos * fCos));
		float fAngle = (float)atan2(fSin, fCos);
		float fInvSin = 1.0f / fSin;
		float fCoeff0 = (float)sin((1.0f - fT) * fAngle) * fInvSin;
		float fCoeff1 = (float)sin(fT * fAngle) * fInvSin;
		return fCoeff0 * rkP + fCoeff1 * rkT;
	}
	else
	{
		// There are two situations:
		// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
		//    interpolation safely.
		// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
		//    are an infinite number of possibilities interpolation. but we haven't
		//    have method to fix this case, so just use linear interpolation here.
		A3DQUATERNION t = (1.0f - fT) * rkP + fT * rkT;
		// taking the complement requires renormalisation
		t.Normalize();
		return t;
	}
}

void _DelayUpdateGfxParentTM(A3DGFXEx* pGfx, const A3DMATRIX4& dstTM, float fPortion)
{
	A3DVECTOR3 vDstScale, vDstPos;
	A3DQUATERNION vDstOrient;
	a3d_DecomposeMatrix(dstTM, vDstScale, vDstOrient, vDstPos);	
	if (_VectorCompare(pGfx->GetPos(), vDstPos))
	{
		A3DVECTOR3 vNewPos = Interp_Position(pGfx->GetPos(), vDstPos, fPortion);
		A3DQUATERNION qNewDir = Slerp(fPortion, pGfx->GetDir(), vDstOrient, true);

		A3DMATRIX4 matNew;
		qNewDir.ConvertToMatrix(matNew);
		A3DQUATERNION qTestConvertBackDir(matNew);
		matNew.SetRow(3, vNewPos);
		pGfx->SetScale(a_Max(vDstScale.x, vDstScale.y, vDstScale.z));
		pGfx->SetParentTM(matNew);
	}
}

//	Tick routine
bool CECModel::Tick(DWORD dwDeltaTime, bool bNoAnim)
{
	AGPA_RECORD_FUNCTION_TIME;
#ifdef FUNCTION_ANALYSER
	ATTACH_FUNCTION_ANALYSER(true, 5, 0, m_pMapModel->m_strFilePath);
#endif

#ifdef GFX_STAT

	__int64 _start = ACounter::GetMicroSecondNow();

#endif

	DWORD dwUpdateTime = dwDeltaTime;
	dwUpdateTime = (DWORD)(dwUpdateTime * m_fPlaySpeed);

	int nDelayMode = m_kECMDelayInfo.OnUpdate(dwDeltaTime);
	if (nDelayMode == DM_PLAYSPEED_DELAY)
		dwUpdateTime = 0;
	else if (nDelayMode == DM_PLAYSPEED_JUMP)
		dwUpdateTime += m_kECMDelayInfo.GetElapsedTime();

	//	Update all channel combined actions
	UpdateChannelActs(get_action_tick_time(dwUpdateTime, bNoAnim));

	//	Tick skin model or model physics when in physics state
	TickSkinModel(get_action_tick_time(dwUpdateTime, bNoAnim));

	//	Tick motion blur
	if (m_pBlurInfo)
		TickMotionBlur(dwUpdateTime);

	m_dwPSTickTime += dwDeltaTime;

	//	Update all channel combined action events
	UpdateChannelEvents(get_action_tick_time(dwUpdateTime, bNoAnim), m_bEventUsePlaySpeed ? dwUpdateTime : dwDeltaTime);

	//	Update fade out sfxs
	UpdateFadeOutSfxLst(dwDeltaTime);

	//	CoGfx's tick use the UpdateTime (which is with playspeed considered)
	TickCoGfx(m_bEventUsePlaySpeed ? dwUpdateTime : dwDeltaTime);

	//	Tick all shown child models
	TickShownChildModels(dwUpdateTime, dwDeltaTime, bNoAnim);

	//	After skin model and all child model updated
	//	Update current model and all child models' aabb
	UpdateModelAABB();

#ifdef GFX_STAT

	{
		float _tick_time = (ACounter::GetMicroSecondNow() - _start) * 0.001f;
		_ecm_total_tick_time += _tick_time;
	}

#endif

	return true;
}

//	Tick ECModel combined actions
void CECModel::UpdateChannelActs(DWORD dwUpdateTime)
{
	if (dwUpdateTime == 0)
		return;
	
	for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		ChannelAct& ca = m_ChannelActs[i];
		ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();
		
		while (pos)
		{
			ALISTPOSITION posCur = pos;
			ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);
			bool bActFinished = false;
			
			if (pNode->m_dwFlagMode == COMACT_FLAG_MODE_ONCE_IGNOREGFX || pNode->m_dwFlagMode == COMACT_FLAG_MODE_ONCE_MULTIIGNOREGFX)
				bActFinished = pNode->m_pActive->IsActionStopped();
			else
				bActFinished = pNode->m_pActive->IsAllFinished();
			
			if (bActFinished)
			{
				if (pNode->m_dwFlagMode != COMACT_FLAG_MODE_NONE && pNode->m_pActFlag)
				{
					*pNode->m_pActFlag = true;
					
					if (pNode->m_dwFlagMode != COMACT_FLAG_MODE_ONCE_MULTIIGNOREGFX)
						pNode->m_dwFlagMode = COMACT_FLAG_MODE_NONE;
					
					pNode->m_pActFlag = NULL;
				}
				
				if (pNode->m_QueuedActs.GetCount())
				{
					A3DCombActDynData* pNext = pNode->m_QueuedActs.RemoveHead();
					
					if (pNext->GetStopPrevAct())
					{
						pNode->m_pActive->Stop(true);
						StopChildrenAct();
					}
					else
						pNode->m_pActive->Stop(false);
					
					// script
					m_pMapModel->OnScriptPlayAction(this, i, pNext->GetComAct()->GetName());

					//	如果QueueAction的时候设置了ResetSpeed，则这里将速度设置重置，默认情况下该标志为false
					if (pNext->IsSetSpeedWhenActStart())
						SetPlaySpeedByChannel(i, pNext->GetComAct());
					
					delete pNode->m_pActive;
					pNode->m_pActive = pNext;
					pNext->Play(i, 1.f, pNode->m_pActive->GetTransTime(), m_EventMasks[i], true, m_bAbsTrack, pNext->GetNoFxFlag());
					pNext->UpdateAct(dwUpdateTime);
					m_bAbsTrack = false;
				}
				else
				{
					pNode->m_pActive->Stop(false);
					ca.m_RankNodes.RemoveAt(posCur);
					delete pNode;
				}
			}
			else
				pNode->m_pActive->UpdateAct(dwUpdateTime);
		}
	}
}

//	Tick Skin Model
void CECModel::TickSkinModel(DWORD dwUpdateTime)
{
	if (!m_pA3DSkinModel)
		return;
	
#ifdef A3D_PHYSX

	if (m_pModelPhysics)
		m_pModelPhysics->UpdateBeforePhysXSim(dwUpdateTime);
	else
		m_pA3DSkinModel->Update(dwUpdateTime);

#else

	m_pA3DSkinModel->Update(dwUpdateTime);

#endif
	
}

//	Tick ECModel combined action events
void CECModel::UpdateChannelEvents(DWORD dwActTime, DWORD dwEventTime)
{
	for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		ChannelAct& ca = m_ChannelActs[i];
		ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();

		while (pos)
		{
			ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);
			pNode->m_pActive->UpdateEvent(dwActTime, dwEventTime);
		}
	}
}

//	Tick CoGfx
void CECModel::TickCoGfx(DWORD dwUpdateTime)
{
	if (m_bCoGfxSleep)
		return;

	for (CoGfxMap::iterator it = m_CoGfxMap.begin(); it != m_CoGfxMap.end(); ++it)
	{
		GFX_INFO* pInfo = it->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();

		if (!pGfx || pGfx->GetState() == ST_STOP || !pGfx->IsVisible())
			continue;

		bool bIsHookMat = false;
		A3DMATRIX4 matHookOrAbs = GetGfxBindHookTransMat(pInfo, bIsHookMat);
		A3DMATRIX4 matGfxInfo = pInfo->GetMatTran();

		if (pInfo->GetDelayTime())
			_DelayUpdateGfxParentTM(pGfx, matGfxInfo * matHookOrAbs, (float)dwUpdateTime / (float)pInfo->GetDelayTime());
		else
			pGfx->SetParentTM(matGfxInfo * matHookOrAbs);

		if (m_bGfxScaleChanged)
			pGfx->SetScale(pInfo->GetScale() * m_fGfxScaleFactor);

		if (pInfo->UseModelAlpha() && m_pA3DSkinModel)
		{
			float fAlpha, fTans = m_pA3DSkinModel->GetTransparent();

			if (m_pA3DSkinModel->IsHidden())
				fAlpha = 0;
			else if (fTans < 0)
				fAlpha = 1.0f;
			else
				fAlpha = 1.0f - fTans;

			pGfx->SetAlpha(fAlpha * pInfo->GetAlpha());
		}

		if (m_pA3DSkinModel)
			InnerUpdateCoGfxParam(static_cast<PGFX_INFO>(pInfo), m_pA3DSkinModel->GetSkeleton());

		if (dwUpdateTime)
			pGfx->TickAnimation(dwUpdateTime);
	}

	//	we have updated all active gfx scales above
	m_bGfxScaleChanged = false;
}

//	Tick Child Models
void CECModel::TickShownChildModels(DWORD dwParentTickTime, DWORD dwRealTickTime, bool bNoAnim)
{
	for (ECModelMap::iterator it2 = m_ChildModels.begin(); it2 != m_ChildModels.end(); ++it2)
	{
		CECModel* pChild = it2->second;

		if (pChild->IsShown())
		{
			pChild->Tick(pChild->IsUsingParentSpeed() ? dwParentTickTime : dwRealTickTime, bNoAnim);

#ifdef A3D_PHYSX

			ChildPhyMap::iterator itPhy = m_ChildPhyDelayMap.find(pChild);
			if (itPhy != m_ChildPhyDelayMap.end())
			{
				ChildPhyDelayInfo& delayInfo = itPhy->second;

				if (delayInfo.iDelayTime > 0)
				{
					delayInfo.iDelayTime -= dwRealTickTime;
					if (delayInfo.iDelayTime <= 0)
					{
						A3DModelPhysics* pPhys = pChild->GetModelPhysics();
						if (pPhys && pPhys->GetPhysState() != delayInfo.nPhyState)
							pPhys->ChangePhysState(delayInfo.nPhyState);

						m_ChildPhyDelayMap.erase(itPhy);
					}
				}
				else
					m_ChildPhyDelayMap.erase(itPhy);
			}

#endif
		}
	}
}

//	Update model aabb by skin model and all child models
void CECModel::UpdateModelAABB()
{
	m_ModelAABB.Clear();

	if (m_pA3DSkinModel)
	{
		a3d_ExpandAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs, m_pA3DSkinModel->GetModelAABB());
	}

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		CECModel* pChild = it->second;
		pChild->UpdateModelAABB();

		if (pChild->IsShown())
			a3d_ExpandAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs, pChild->m_ModelAABB);
	}

	m_ModelAABB.CompleteCenterExts();
}

// Set scale to all root-bones and gfx
bool CECModel::ScaleAllRootBonesAndGfx(float fScale)
{
	if (!m_pMapModel || !m_pA3DSkinModel || !m_pA3DSkinModel->GetSkeleton())
		return false;

	A3DSkeleton* pSkeleton = m_pA3DSkinModel->GetSkeleton();
	for (int iRootIdx = 0; iRootIdx < pSkeleton->GetRootBoneNum(); ++iRootIdx)
	{
		int iBoneIdx = pSkeleton->GetRootBone(iRootIdx);
		A3DBone* pRootBone = pSkeleton->GetBone(iBoneIdx);

		pRootBone->SetLocalScale(1.f, 1.f);

		float fInitScl = 1.f;
		if (BoneScaleEx* pSclInfo = GetBoneScaleExByIndex(iBoneIdx))
		{
			fInitScl = pSclInfo->m_fWholeFactor;
		}

		pRootBone->SetWholeScale(fScale * fInitScl);
	}

	m_fRootBoneScaleFactor = fScale;

	SetGfxScale(fScale);
	return true;
}

//	Inner Update CoGfx's ModParam
bool CECModel::InnerUpdateCoGfxParam(GFX_INFO* pInfo, A3DSkeleton* pSkeleton)
{
	if (!pInfo || !pSkeleton)
		return false;

	A3DGFXEx* pGfx = pInfo->GetGfx();
	if (!pGfx)
		return false;

	ModActParamList& lst = pInfo->GetParamList();
	size_t nModParamIdx , nModParamCount = lst.size();
	for (nModParamIdx = 0; nModParamIdx < nModParamCount; ++nModParamIdx)
	{
		ModActParam* pParam = lst[nModParamIdx];
		switch(pParam->m_nParamId)
		{
		case ID_PARAM_LTN_POS1:
		case ID_PARAM_LTN_POS2:
			{
				int nIndex = -1;
				A3DSkeletonHook* pHook = pSkeleton->GetHook(pParam->m_ParamData.m_strHook, &nIndex);
				if (!pHook)
					continue;
				pGfx->UpdateEleParam(pParam->m_strEleName, pParam->m_nParamId, pHook->GetAbsoluteTM().GetRow(3));
			}
			break;
		}
	}

	return true;
}

//	Check and update action's play speed
void CECModel::SetPlaySpeedByChannel(int nChannel, A3DCombinedAction* pComAct)
{
	if (m_nMainChannel == nChannel || 0 == nChannel)
	{
		m_fPlaySpeed = m_pMapModel->m_fDefPlaySpeed * pComAct->GetPlaySpeed();
	}
}

void CECModel::ApplyPixelShader()
{
	if (m_pReplacePS)
	{
		m_pReplacePS->Appear();
		g_GfxApplyPixelShaderConsts(m_pA3DDevice, m_vecReplacePSConsts, m_dwPSTickTime, 0);
	}
	else
	{
		m_pPixelShader->Appear();
		g_GfxApplyPixelShaderConsts(m_pA3DDevice, m_pMapModel->m_vecPSConsts, m_dwPSTickTime, 0);
	}
}

void CECModel::RestorePixelShader()
{
	m_pA3DDevice->ClearPixelShader();
}

bool CECModel::SetReplaceShader(const char* szShader, const char* szShaderTex, const GfxPSConstVec& consts)
{
	static const AString _base_path = "Gfx\\Textures\\";

	RemoveReplaceShader();
	m_pReplacePS = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(szShader);

	if (!m_pReplacePS)
		return false;

#ifdef _ANGELICA21

	if (szShaderTex[0])
	{
		AString strPath = _base_path + szShaderTex;
		m_pReplacePS->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)(const char*)strPath);
	}

#else

	if (szShaderTex[0])
	{
		AString strPath = _base_path + szShaderTex;
		m_pReplacePS->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)(const char*)strPath);
	}

#endif

	const size_t consts_count = consts.size();

	if (consts_count)
	{
		m_vecReplacePSConsts.reserve(consts_count);

		for (size_t i = 0; i < consts_count; i++)
		{
			m_vecReplacePSConsts.push_back(consts[i]);
			m_vecReplacePSConsts[i].CalcTotalTime();
		}
	}

	m_dwPSTickTime = 0;
	return true;
}

void CECModel::RemoveReplaceShader()
{
	if (m_pReplacePS)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseShader(&m_pReplacePS);
		m_pReplacePS = NULL;
		m_vecReplacePSConsts.clear();
		m_dwPSTickTime = 0;
	}
}

//	Render routine
bool CECModel::Render(A3DViewport* pViewport, bool bRenderAtOnce/* false */, bool bRenderSkinModel/*=true*/, 
					  DWORD dwFlags/*=0*/, A3DSkinModelRenderModifier* pRenderModifier)
{
	AGPA_RECORD_FUNCTION_TIME;
	bool bVisible;
	bool bRenderSmdFlag;

#ifdef _ANGELICA21
	bVisible = false;
#endif

	// first of all update light info
	if (m_pA3DSkinModel && m_bLoadSkinModel && m_bUpdateLightInfo)
		AfxSetA3DSkinModelSceneLightInfo(m_pA3DSkinModel);

	RenderActionFX(pViewport);

#ifdef _ANGELICA21
	bRenderSmdFlag = m_pA3DSkinModel && (m_bLoadSkinModel || (m_dwChildRenderFlag & SMD_CHILD_RENDER_SKIP)) && m_pMapModel->m_bRenderSkinModel && bRenderSkinModel;
#else
	bRenderSmdFlag = m_pA3DSkinModel && m_bLoadSkinModel && m_pMapModel->m_bRenderSkinModel && bRenderSkinModel;
#endif

	if (bRenderSmdFlag)
	{
		A3DAABB aabb;

		//	dyx: 2009.4.1, For the models that only some parts of them have convex hulls,
		//		CH aabb doesn't works well. A3DSkinModel::UpdateHitBoxes use new way 
		//		to calculate model's aabb, so don't use HasCHAABB() anymore. 
		if (HasCHAABB())
		{
			aabb.Center = GetCHAABB().Center * GetAbsTM();
			aabb.Extents = GetCHAABB().Extents;
			aabb.Extents.x = aabb.Extents.y = aabb.Extents.z = max(max(aabb.Extents.x, aabb.Extents.y), aabb.Extents.z);
			aabb.CompleteMinsMaxs();

			aabb.Merge(m_ModelAABB);
		}
		else
			aabb = m_ModelAABB;

		bVisible = pViewport->GetCamera()->AABBInViewFrustum(aabb);

		if (bVisible)
		{
#ifdef _ANGELICA21
			A3DShader* pCustomShader;
			const GfxPSConstVec* pPSConstVec;

			if (m_pReplacePS)
			{
				pCustomShader = m_pReplacePS;
				pPSConstVec = &m_vecReplacePSConsts;
			}
			else
			{
				pCustomShader = m_pPixelShader;
				pPSConstVec = &m_pMapModel->m_vecPSConsts;
			}

			if (pCustomShader && (dwFlags & (A3DSkinModel::RAO_NOPIXELSHADER | A3DSkinModel::RAO_NOTEXTURE)) == 0)
			{
				if (m_CustomRenderFunc)
				{
					SHADERGENERAL& sh = pCustomShader->GetGeneralProps();
					(*m_CustomRenderFunc)(pViewport, m_pA3DSkinModel, pCustomShader, *pPSConstVec, m_dwPSTickTime, sh.strHLSLPrefix, false);
				}
				else
				{
					ApplyPixelShader();
					m_pA3DSkinModel->RenderAtOnce(pViewport, dwFlags | A3DSkinModel::RAO_NOPIXELSHADER, false);
					RestorePixelShader();
				}
			}
			else
			{
				if (!bRenderAtOnce)
					m_pA3DSkinModel->Render(pViewport, false);
				else
					m_pA3DSkinModel->RenderAtOnce(pViewport, dwFlags, false);
			}
#else
			if (!bRenderAtOnce)
			{
				#ifdef _ANGELICA22
					m_pA3DSkinModel->Render(pViewport, false, pRenderModifier);
				#else
					m_pA3DSkinModel->Render(pViewport, false);
				#endif
			}
			else
				m_pA3DSkinModel->RenderAtOnce(pViewport, dwFlags, false);
#endif
		}

		if (m_pMapModel->m_bCanCastShadow && m_bCastShadow && m_pA3DSkinModel)
			AfxECModelAddShadower(aabb.Center, aabb, m_pA3DSkinModel);
	}

	if (m_pBlurInfo && m_pBlurInfo->m_bRoot && m_pA3DSkinModel)
	{
		AfxGetGFXExMan()->RegisterECMForMotionBlur(this);
	}

	RenderCoGfx();
	
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		CECModel* pChild = it->second;

		if (pChild->m_bShown)
		{
#ifdef _ANGELICA21
			if (pChild->GetChildRenderFlag() & SMD_CHILD_RENDER_SKIP)
				continue;
#endif

			pChild->Render(pViewport, bRenderAtOnce, bRenderSkinModel, dwFlags, pRenderModifier);
		}
	}

#ifdef _ANGELICA21
	return bVisible;
#else
	return true;
#endif
}

void CECModel::ClearMotionBlur()
{
	if (!m_pBlurInfo)
		return;
		
	m_pBlurInfo->m_uCurMatricesCount = 0;
	
	if (!m_pBlurInfo->m_bApplyToChildren)
		return;

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		CECModel* pChild = it->second;
		pChild->ClearMotionBlur();
	}
}

bool CECModel::CalcMotionBlurParams()
{
	assert(m_pBlurInfo && m_pA3DSkinModel);

	if (!m_pA3DSkinModel->GetBlendMatUpdated())
	{
		ClearMotionBlur();
		return false;
	}

	if (!m_pBlurInfo->m_bCanAddFrame)
		return true;

	int nBoneNum = m_pA3DSkinModel->GetSkeleton()->GetBoneNum();
	const A3DMATRIX4* aTBlendMats = m_pA3DSkinModel->GetTVSBlendMatrices();

	if (m_pBlurInfo->m_uCurMatricesCount < m_pBlurInfo->m_Colors.size() + 1)
	{
		ECMBlendMatricesSet* pSet = m_pBlurInfo->m_MatricesCollector.GetByIndex(m_pBlurInfo->m_uCurMatricesCount);
		memcpy(pSet->begin(), aTBlendMats, sizeof(A3DMATRIX4) * nBoneNum);
		m_pBlurInfo->m_uCurMatricesCount++;
	}
	else
	{
		ECMBlendMatricesSet* pSet = m_pBlurInfo->m_MatricesCollector.RemoveHead();
		memcpy(pSet->begin(), aTBlendMats, sizeof(A3DMATRIX4) * nBoneNum);
		m_pBlurInfo->m_MatricesCollector.AddTail(pSet);
	}

	if (m_pBlurInfo->m_bApplyToChildren)
	{
		for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		{
			CECModel* pChild = it->second;

			if (pChild->m_bShown && pChild->m_pBlurInfo && pChild->m_pA3DSkinModel)
				pChild->CalcMotionBlurParams();
		}
	}

	return true;
}

void CECModel::TickMotionBlur(DWORD dwDeltaTime)
{
	m_pBlurInfo->m_dwCurTime += dwDeltaTime;

	if (m_pBlurInfo->m_dwCurTime > m_pBlurInfo->m_dwTotalTime)
	{
		m_pBlurInfo->m_uStartIndex++;

		if (m_pBlurInfo->m_uStartIndex + 1 >= m_pBlurInfo->m_uCurMatricesCount)
		{
			RemoveMotionBlurInfo();
			return;
		}
		else
		{
			m_pBlurInfo->m_bCanAddFrame = true;
		}
	}
	else
	{
		m_pBlurInfo->m_dwCurTick += dwDeltaTime;

		if (m_pBlurInfo->m_dwCurTick >= m_pBlurInfo->m_dwInterval)
		{
			m_pBlurInfo->m_dwCurTick -= m_pBlurInfo->m_dwInterval;
			m_pBlurInfo->m_bCanAddFrame = true;
		}
		else
			m_pBlurInfo->m_bCanAddFrame = false;
	}
}

void CECModel::RenderMotionBlur(A3DViewport* pViewport)
{
	assert(m_pA3DSkinModel && m_pBlurInfo && m_pBlurInfo->m_bRoot);

	if (!m_pA3DSkinModel)
		return;

	if (!CalcMotionBlurParams())
		return;

	if (m_pBlurInfo->m_uCurMatricesCount > 1)
	{
		if (m_pBlurInfo->m_bHighlight)
		{
			m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
			m_pA3DDevice->SetDestAlpha(A3DBLEND_ONE);
		}
		else
		{
			m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
			m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		}

		float fOldAlpha = m_fInnerAlpha;
		A3DCOLORVALUE OldColor = m_InnerColor;
		A3DCOLORVALUE c;
		size_t diff;
		size_t nIndex;

		for (size_t i = m_pBlurInfo->m_uStartIndex; i < m_pBlurInfo->m_uCurMatricesCount - 1; i++)
		{
			diff = m_pBlurInfo->m_Colors.size() + 1 - m_pBlurInfo->m_uCurMatricesCount;
			nIndex = i + diff;
			c = m_pBlurInfo->m_Colors[nIndex];
			SetInnerAlpha(c.a);
			c.a = 1.0f;
			SetInnerColor(c);
			ApplyBlurMatrices(i);
			m_pA3DSkinModel->RenderAtOnce(pViewport, 0, false);
		}

		SetInnerColor(OldColor);
		SetInnerAlpha(fOldAlpha);

		if (m_pBlurInfo->m_bHighlight)
			m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}
}

void CECModel::ApplyBlurMatrices(int nIndex)
{
	if (!m_pA3DSkinModel)
		return;

	if (nIndex < m_pBlurInfo->m_MatricesCollector.GetCount() && nIndex < (int)m_pBlurInfo->m_uCurMatricesCount - 1)
	{
		ECMBlendMatricesSet* pSet = m_pBlurInfo->m_MatricesCollector.GetByIndex(nIndex);
		int nBoneNum = m_pA3DSkinModel->GetSkeleton()->GetBoneNum();
		A3DMATRIX4* aTBlendMats = const_cast<A3DMATRIX4*>(m_pA3DSkinModel->GetTVSBlendMatrices());
		size_t uBonesSelected = m_pBlurInfo->m_BoneIndices.size();

		if (uBonesSelected)
		{
			for (size_t i = 0; i < uBonesSelected; i++)
			{
				int nIndex = m_pBlurInfo->m_BoneIndices[i];

				if (nIndex >= 0 && nIndex < nBoneNum)
					memcpy(aTBlendMats + nIndex, pSet->begin() + nIndex, sizeof(A3DMATRIX4));
			}
		}
		else
			memcpy(aTBlendMats, pSet->begin(), sizeof(A3DMATRIX4) * nBoneNum);
	}

	if (m_pBlurInfo->m_bApplyToChildren)
	{
		for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		{
			CECModel* pChild = it->second;

			if (pChild->m_bShown && pChild->m_pBlurInfo)
				pChild->ApplyBlurMatrices(nIndex);
		}
	}
}

//	Render CoGfx
void CECModel::RenderCoGfx()
{
	if (m_bCoGfxSleep)
		return;

	for (CoGfxMap::iterator itGfx = m_CoGfxMap.begin(); itGfx != m_CoGfxMap.end(); ++itGfx)
	{
		A3DGFXEx* pGfx = itGfx->second->GetGfx();
		if (!pGfx)
			continue;

		if (pGfx->GetState() != ST_STOP)
			AfxGetGFXExMan()->RegisterGfx(pGfx);
	}
}


void CECModel::RenderActionFX(A3DViewport* pViewport)
{
	for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		ChannelAct& ca = m_ChannelActs[i];
		ChannelActNode* pNode = ca.GetHighestRankNode();

		if (pNode)
			pNode->m_pActive->Render(pViewport);
	}
}

void CECModel::StartECModelDelayMode(int nDelayTime)
{
	m_kECMDelayInfo.OnStartDelay(nDelayTime);
}

void CECModel::UpdateBaseActTimeSpan()
{
#ifndef GFX_EDITOR

	if (m_pMapModel->m_bActMapped)
		return;

#endif

	if (!m_pA3DSkinModel)
		return;

	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;

	for (CombinedActMap::iterator it = ActionMap.begin(); it != ActionMap.end(); ++it)
	{
		for (int i = 0; i < it->second->GetBaseActCount(); i++)
		{
			PACTION_INFO pInfo = it->second->GetBaseAct(i);
			A3DSkinModelActionCore* pBaseAct = m_pA3DSkinModel->GetAction(pInfo->GetName());
			if (pBaseAct) pInfo->SetTimeSpan(pBaseAct->GetActionTime());
		}

		it->second->CalcMinComActTimeSpan();
	}

#ifndef GFX_EDITOR

	m_pMapModel->m_bActMapped = true;

#endif
}

void CECModel::ShowCoGfx(bool bShow)
{
	for (CoGfxMap::iterator it = m_CoGfxMap.begin(); it != m_CoGfxMap.end(); ++it)
	{
		PGFX_INFO pInfo = it->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();
		assert(pGfx);

		if (bShow)
		{
			pGfx->SetScale(pInfo->GetScale() * m_fGfxScaleFactor);
			pGfx->SetAlpha(pInfo->GetAlpha());
			pGfx->SetPlaySpeed(pInfo->GetPlaySpeed());
			pGfx->SetSfxPriority(m_nSfxPriority);
			TransferEcmProperties(pGfx);
			pGfx->Start(true);
		}
		else
			pGfx->Stop();
	}
}

//	This function will make all CoGfx stop from Ticking or Rendering, like they are sleeping
void CECModel::SleepCoGfx(bool bSleep, bool bRecurOnChild)
{
	m_bCoGfxSleep = bSleep;

	if (!bRecurOnChild)
		return;

	for (ECModelMap::iterator it = m_ChildModels.begin()
		; it != m_ChildModels.end()
		; ++it)
	{
		it->second->SleepCoGfx(bSleep, bRecurOnChild);
	}
}

void CECModel::RemoveBoneScale(BoneScale* pScale)
{
	for (size_t i = 0; i < m_pMapModel->m_BoneScales.size(); i++)
	{
		if (m_pMapModel->m_BoneScales[i] == pScale)
		{
			if (m_pA3DSkinModel)
			{
				A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();
				A3DBone* pBone = pSke->GetBone(pScale->m_nIndex);
				pBone->SetScaleFactor(1.0f);
				pBone->SetScaleType(0);
			}
			
			m_pMapModel->m_BoneScales.erase(&m_pMapModel->m_BoneScales[i]);
			return;
		}
	}
}

void CECModel::UpdateBoneScales()
{
	if (!m_pA3DSkinModel)
		return;

	A3DSkeleton * pSke = m_pA3DSkinModel->GetSkeleton();

	if (!pSke)
		return;

	BoneScaleArray& BoneScales = m_pMapModel->m_BoneScales;

	for (size_t i = 0; i < BoneScales.size(); i++)
	{
		BoneScale* pScale = BoneScales[i];

		if (pScale->m_nIndex >= pSke->GetBoneNum()) continue;
		A3DBone* pBone = pSke->GetBone(pScale->m_nIndex);
		if (!pBone) continue;
		pBone->SetScaleFactor(pScale->m_vScale);
		pBone->SetScaleType(pScale->m_nType);
	}
}


void CECModel::RemoveBoneScaleEx(BoneScaleEx* pScale)
{
	for (size_t i = 0; i < m_pMapModel->m_BoneScaleExArr.size(); i++)
	{
		if (m_pMapModel->m_BoneScaleExArr[i] == pScale)
		{
			if (m_pA3DSkinModel)
			{
				A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();
				A3DBone* pBone = pSke->GetBone(pScale->m_nIndex);
				
				if (pBone)
				{
					pBone->SetLocalScale(1.0f, 1.0f);
					pBone->SetWholeScale(1.0f);
				}
			}
			
			m_pMapModel->m_BoneScaleExArr.erase(m_pMapModel->m_BoneScaleExArr.begin()+i);
			return;
		}
	}
}

void CECModel::UpdateBoneScaleEx()
{
	UpdateBoneScaleEx(m_pMapModel->m_BoneScaleExArr, m_pMapModel->m_strScaleBaseBone);
}

void CECModel::UpdateBoneScaleEx(BoneScaleExArray& arr, const char* szBaseBone)
{
	if (!m_pA3DSkinModel)
		return;

	A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();

	if (!pSke)
		return;

	for (int n = 0; n < pSke->GetBoneNum(); n++)
	{
		A3DBone* pBone = pSke->GetBone(n);

		if (pBone)
		{
			pBone->SetLocalScale(1.0f, 1.0f);
			pBone->SetWholeScale(1.0f);
		}
	}

	for (size_t i = 0; i < arr.size(); i++)
	{
		BoneScaleEx* pScale = arr[i];

		if (pScale->m_nIndex >= pSke->GetBoneNum())
			continue;

		A3DBone* pBone = pSke->GetBone(pScale->m_nIndex);

		if (pBone)
		{
			pBone->SetLocalScale(pScale->m_fLenFactor, pScale->m_fThickFactor);
			pBone->SetWholeScale(pScale->m_fWholeFactor);
		}
	}

	pSke->CalcFootOffset(szBaseBone);
}

bool CECModel::PlayActionByName(const char* szActName, float fWeight, bool bRestart/* true */,
					int nTransTime/* 200 */, bool bForceStop/* true */, DWORD dwUserData/* 0 */, bool bNoFx, bool *pActFlag, DWORD dwFlagMode)
{
	return PlayActionByName(0, szActName, fWeight, bRestart, nTransTime, bForceStop, dwUserData, bNoFx, pActFlag, dwFlagMode);
}

bool CECModel::PlayActionByName(int nChannel, const char* szActName, float fWeight, bool bRestart/* true */,
					int nTransTime/* 200 */, bool bForceStop/* true */, DWORD dwUserData/* 0 */, bool bNoFx, bool *pActFlag, DWORD dwFlagMode)
{
	if (!szActName)
		return false;

	A3DCombinedAction* pComAct = GetComActByName(szActName);

	if (pComAct == NULL)
		return false;

	BYTE rank = pComAct->GetRank(nChannel);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.GetNodeByRank(rank);
	A3DCombActDynData* pActive;

	if (pNode)
	{
		pActive = pNode->m_pActive;
		pNode->RemoveQueuedActs();
	}
	else
		pActive = 0;

	if (!bRestart && pActive && stricmp(pActive->GetComAct()->GetName(), szActName) == 0)
		return true;

	if (m_pBlurInfo && m_pBlurInfo->m_bRoot && m_pBlurInfo->m_bStopWhenActChange)
		RemoveMotionBlurInfo();

	// script
	m_pMapModel->OnScriptPlayAction(this, nChannel, szActName);

	// delay mode state
	m_kECMDelayInfo.OnPlayComAction();

	if (pActive != NULL)
	{
		pActive->Stop(bForceStop);
		delete pActive;
	}
	else if (!m_bFirstActPlayed)
	{
		m_bFirstActPlayed = true;
		nTransTime = 0;
	}

	if (!pNode)
	{
		pNode = new ChannelActNode();
		pNode->m_Rank = rank;
		ca.m_RankNodes.AddTail(pNode);
	}

	SetPlaySpeedByChannel(nChannel, pComAct);
	
	pNode->m_pActive = new A3DCombActDynData(pComAct, this);
	pNode->m_pActive->SetUserData(dwUserData);
	pNode->m_pActive->Play(nChannel, fWeight, nTransTime, m_EventMasks[nChannel], bRestart, m_bAbsTrack, bNoFx);
	m_bAbsTrack = false;

	// clear notify flag each time we play a new action
	// SL - START
	// this is needed from parameters so that act can be flagged as done
	//pNode->m_pActFlag = NULL;
	//pNode->m_dwFlagMode = COMACT_FLAG_MODE_NONE;
	pNode->m_pActFlag = pActFlag;
	pNode->m_dwFlagMode = dwFlagMode;
	// SL - END
	return true;
}

void CECModel::PlayAttackAction(
	int nChannel,
	const char* szActName,
	int nTransTime,
	unsigned char SerialId,
	clientid_t nCasterId,
	clientid_t nCastTarget,
	const A3DVECTOR3* pFixedPoint,
	DWORD dwUserData,
	bool bNoFx)
{
	if (m_pA3DSkinModel == NULL)
		return;

	A3DCombinedAction* pComAct = GetComActByName(szActName);

	if (pComAct == NULL)
		return;

	BYTE rank = pComAct->GetRank(nChannel);
	StopChannelAction(nChannel, rank, true);

	// script
	m_pMapModel->OnScriptPlayAction(this, nChannel, szActName);

	// delay mode on play
	m_kECMDelayInfo.OnPlayComAction();

	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = new ChannelActNode();
	pNode->m_Rank = rank;
	ca.m_RankNodes.AddTail(pNode);
	pNode->m_pActive = new A3DCombActDynData(pComAct, this);
	pNode->m_pActive->SetAttackAct(true);
	pNode->m_pActive->SetCasterId(nCasterId);
	pNode->m_pActive->SetCastTargetId(nCastTarget);
	pNode->m_pActive->SetSerialId(SerialId);
	pNode->m_pActive->SetUserData(dwUserData);

	if (!nCastTarget && pFixedPoint)
		pNode->m_pActive->SetFixedPoint(*pFixedPoint);

	SetPlaySpeedByChannel(nChannel, pComAct);
	
	pNode->m_pActive->Play(nChannel, 1.0f, nTransTime, m_EventMasks[nChannel], true, m_bAbsTrack, bNoFx);
	m_bAbsTrack = false;
	pNode->m_pActFlag = NULL;
	pNode->m_dwFlagMode = COMACT_FLAG_MODE_ONCE_IGNOREGFX;
}

void CECModel::OnScriptChangeEquip(int nEquipId, int nEquipFlag, bool bFashionMode, int nPathId, int nEquipIndex)
{
	m_bFashionMode = bFashionMode;

	if (nEquipFlag)
	{
		m_SlotEquipMap[nEquipIndex] = nEquipId;
		m_EquipSlotMap[nEquipId] = nEquipIndex;
	}
	else
	{
		m_SlotEquipMap.erase(nEquipIndex);
		m_EquipSlotMap.erase(nEquipId);
	}

	m_pMapModel->OnScriptChangeEquip(this, nEquipId, nEquipFlag, bFashionMode, nPathId, nEquipIndex);
}

void CECModel::AddOneAttackDamageData(
	int nChannel,
	clientid_t nCaster,
	clientid_t nTarget,
	unsigned char SerialId,
	DWORD dwModifier,
	int nDamage,
	bool bIsAttDelay,
	int nAttDelayTimeIdx)
{
	if (AfxSkillGfxAddDamageData(nCaster, nTarget, SerialId, dwModifier, nDamage))
		return;

	ChannelAct& ca = m_ChannelActs[nChannel];
	ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();

	while (pos)
	{
		ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);

		if (pNode->m_pActive->IsAttackAct())
		{
			TARGET_DATA td;
			td.dwModifier = dwModifier;
			td.idTarget = nTarget;
			td.nDamage = nDamage;
			td.bIsAttDelay = bIsAttDelay;
			td.nAttDelayTimeIdx = nAttDelayTimeIdx;
			pNode->m_pActive->AddOneTarget(td);
			return;
		}
	}

	AfxSkillGfxShowDamage(nCaster, nTarget, nDamage, 1, dwModifier);
	AfxSkillGfxShowCaster(nCaster, dwModifier);
}

bool CECModel::QueueAction(int nChannel, const char* szActName, int nTransTime, DWORD dwUserData/* 0 */, bool bForceStopPrevAct, bool bCheckTailDup, bool bNoFx, bool bResetSpeed)
{
	A3DCombinedAction* pComAct = GetComActByName(szActName);

	if (!pComAct)
		return false;

	BYTE rank = pComAct->GetRank(nChannel);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.GetNodeByRank(rank);

	if (!pNode)
		return false;

	if (bCheckTailDup
		&& pNode->m_QueuedActs.GetCount()
		&& pNode->m_QueuedActs.GetTail()->GetComAct() == pComAct)
		return false;

	A3DCombActDynData* pDynData = new A3DCombActDynData(pComAct, this);
	pDynData->SetUserData(dwUserData);
	pDynData->SetTransTime(nTransTime);
	pDynData->SetStopPrevAct(bForceStopPrevAct);
	pDynData->SetNoFxFlag(bNoFx);
	pDynData->SetSpeedWhenActStart(bResetSpeed);
	pNode->m_QueuedActs.AddTail(pDynData);
	return true;
}

void CECModel::StopChildrenAct()
{
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		if (it->second->m_bCanAffectedByParent)
			it->second->StopAllActions();
}

void CECModel::StopAllActions(bool bStopFx)
{
	ResetMaterialScale();
	StopChannelAction(0, true, bStopFx);
	StopChildrenAct();
}

void CECModel::StopChannelAction(int nChannel, BYTE rank, bool bStopAct, bool bStopFx)
{
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.RemoveNodeByRank(rank);

	if (pNode)
	{
		pNode->m_pActive->Stop(bStopAct, bStopFx);
		delete pNode;
	}
}

void CECModel::StopChannelAction(int nChannel, bool bStopAct, bool bStopFx)
{
	ChannelAct& ca = m_ChannelActs[nChannel];
	ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();

	while (pos)
	{
		ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);
		pNode->m_pActive->Stop(bStopAct, bStopFx);
		delete pNode;
	}

	ca.m_RankNodes.RemoveAll();
}

void CECModel::SetId(clientid_t id)
{
	m_nId = id;
	
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		CECModel* pChild = it->second;

		if (pChild->InheritParentId())
			pChild->SetId(id);
	}
}

bool CECModel::AddChildModel(
	const char* szHangerName,
	bool bOnBone,
	const char* szHookName,
	const char* szModelFile,
	const char* szCCName)
{
	CECModel* pECModel = new CECModel;

	pECModel->SetGfxUseLOD(m_bGfxUseLod, false);
	pECModel->SetDisableCamShake(m_bDisableCamShake);
	pECModel->SetCreatedByGfx(m_bCreatedByGfx);
	pECModel->SetId(m_nId);

	if (!pECModel->Load(szModelFile, true, 0, true, true, false))
	{
		delete pECModel;
		return false;
	}

	if (!AddChildModel(szHangerName, bOnBone, szHookName, pECModel, szCCName))
	{
		pECModel->Release();
		delete pECModel;
		return false;
	}

#ifndef _ANGELICA21
	//	tick 0 to make the child model be update to the new position
	//	for phys attacher and related joints' creation
	pECModel->Tick(0);
#endif

	return true;
}

bool CECModel::AddChildModel(
	const char* szHangerName,
	bool bOnBone,
	const char* szHookName,
	CECModel* pChild,
	const char* szCCName)
{
	if (m_ChildModels.find(szHangerName) != m_ChildModels.end())
		return false;

	if (m_pA3DSkinModel && pChild->m_pA3DSkinModel)
	{
		if (!m_pA3DSkinModel->AddChildModel(
			szHangerName,
			bOnBone,
			szHookName,
			pChild->m_pA3DSkinModel,
			szCCName))
			return false;

		pChild->m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);
		pChild->m_bLoadSkinModel = false;
	}

	pChild->m_strHookName = szHookName;
	pChild->m_strCC = szCCName;
	pChild->SetGfxUseLOD(m_bGfxUseLod, false);
	pChild->SetDisableCamShake(m_bDisableCamShake);
	pChild->SetCreatedByGfx(m_bCreatedByGfx);

	if (pChild->InheritParentId())
		pChild->SetId(m_nId);

	pChild->RemoveMotionBlurInfo();
	m_ChildModels[szHangerName] = pChild;

	return true;
}

void CECModel::RemoveChildModel(const char* szHangerName, bool bDel)
{
	if (!bDel) 
	{
		if (m_pA3DSkinModel)
			m_pA3DSkinModel->UnbindChildModel(szHangerName);
	}

	ECModelMap::iterator it = m_ChildModels.find(szHangerName);

	if (it == m_ChildModels.end())
		return;

	CECModel* pChild = it->second;

	if (bDel)
	{
		m_ChildPhyDelayMap.erase(pChild);

		pChild->Release();
		delete pChild;
	}
	else
	{
		if (pChild->m_pA3DSkinModel)
			pChild->m_bLoadSkinModel = true;

		pChild->m_strHookName.Empty();
		pChild->m_strCC.Empty();
		CloseChildModelLink(szHangerName);
	}

	m_ChildModels.erase(szHangerName);

	// call this A3DSkinModel::RemoveChildModel function after pChild->Release()
	// for pChild->Release() will use information in A3DSkeleton
	// which would be released in A3DSkinModel::RemoveChildModel
	// otherwise, it will crash in pChild->Release()
	if (bDel && m_pA3DSkinModel)
	{
		m_pA3DSkinModel->RemoveChildModel(szHangerName);
	}
}

bool CECModel::ChildChangeHook(const char* szHanger, const char* szNewHook)
{
	CECModel* pChild = GetChildModel(szHanger);

	if (!pChild)
		return false;

	if (stricmp(pChild->m_strHookName, szNewHook) == 0)
		return true;

	AString strCC = pChild->m_strCC;
	
	//	First copy a new link info array if the original one exists
	ChildModelLinkInfoArray aPhyslinks;
	for (ChildModelLinkInfoArray::iterator itr = pChild->m_PhysHookAttachLinks.begin()
		; itr != pChild->m_PhysHookAttachLinks.end()
		; ++itr)
		aPhyslinks.push_back(ChildModelLinkInfo(szNewHook, itr->m_strChildAttacher));

#ifdef A3D_PHYSX
	int nOldPhyState = A3DModelPhysSync::PHY_STATE_ANIMATION;
	A3DModelPhysics* pPhys = pChild->GetModelPhysics();
	if (pPhys)
	{
		nOldPhyState = pPhys->GetPhysState();
		if (nOldPhyState != A3DModelPhysSync::PHY_STATE_ANIMATION)
			pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);
	}
#endif

	RemoveChildModel(szHanger, false);

	if (!AddChildModel(szHanger, false, szNewHook, pChild, strCC))
	{
		pChild->Release();
		delete pChild;
		return false;
	}

	TryOpenChildModelLinks(szHanger, aPhyslinks);

#ifdef A3D_PHYSX
	if (pPhys && nOldPhyState != A3DModelPhysSync::PHY_STATE_ANIMATION)
	{
		//pPhys->ChangePhysState(nOldPhyState);
		m_ChildPhyDelayMap[pChild] = ChildPhyDelayInfo(200, nOldPhyState);
	}
#endif

	return true;
}

void CECModel::AddCoGfx(GFX_INFO* pInfo)
{
	AString strIndex;
	int i = m_pMapModel->m_CoGfxMap.size();
	
	while (true)
	{
		strIndex.Format("%d_co", i);
		
		if (m_pMapModel->m_CoGfxMap.find(strIndex) == m_pMapModel->m_CoGfxMap.end())
			break;
		
		i++;
	}
	
	m_CoGfxMap[strIndex] = pInfo;
	m_pMapModel->m_CoGfxMap[strIndex] = static_cast<PGFX_INFO>(EVENT_INFO::CloneFrom(pInfo->GetComAct(), *pInfo));
}

void CECModel::DelCoGfx(GFX_INFO* pInfo)
{
	CoGfxMap::iterator it = m_CoGfxMap.begin();
	
	for (; it != m_CoGfxMap.end(); ++it)
	{
		if (it->second == pInfo)
		{
			delete m_pMapModel->m_CoGfxMap[it->first];
			m_pMapModel->m_CoGfxMap.erase(it->first);
			delete pInfo;
			m_CoGfxMap.erase(it->first);
			return;
		}
	}
}

AString CECModel::GetCoGfxName(GFX_INFO* pInfo)
{
	CoGfxMap::iterator it = m_CoGfxMap.begin();
	
	for (; it != m_CoGfxMap.end(); ++it)
	{
		if (it->second == pInfo)
			return it->first;
	}
	
	return AString();
}

void CECModel::UpdateCoGfx(AString strIndex, GFX_INFO* pSrc)
{
	CoGfxMap::iterator it = m_pMapModel->m_CoGfxMap.find(strIndex);
	
	if (it == m_pMapModel->m_CoGfxMap.end())
		return;
	
	PGFX_INFO pInfo = new GFX_INFO(NULL);
	pInfo->Clone(*pSrc);
	delete it->second;
	it->second = pInfo;
}

bool CECModel::ParentActivate(
	const AString& strActName,
	const ChildActInfo* pHostInfo,
	CECModel* pParentModel,
	DWORD dwDeltaTime)
{
	if (pHostInfo->IsTrailAct() && stricmp(pHostInfo->GetHHName(), m_strHookName) != 0)
		return false;

	A3DCombinedAction* pComAct = GetComActByName(strActName);

	if (!pComAct)
		return false;

	BYTE rank = pComAct->GetRank(0);
	ChannelAct& ca = m_ChannelActs[0];
	ChannelActNode* pNode = ca.RemoveNodeByRank(rank);

	// 当前播放动作是要播放动作，且是无限的
	if (pNode
	&& pNode->m_pActive->GetComAct() == pComAct
	&& !pNode->m_pActive->IsActionStopped()
	&& pNode->m_pActive->GetComAct()->IsInfinite())
	{
		ca.m_RankNodes.AddTail(pNode);
		return true;
	}

	delete pNode;
	pNode = new ChannelActNode();
	pNode->m_Rank = rank;
	ca.m_RankNodes.AddTail(pNode);

	pNode->m_pActive = new A3DCombActDynData(pComAct, this);
	pNode->m_pActive->SetParentActInfo(pHostInfo, pParentModel, dwDeltaTime);
	pNode->m_pActive->Play(0, 1.f, pHostInfo->GetTransTime(), m_EventMasks[0], false);

	return true;
}

void CECModel::ActivateChildAct(
	const AString& strActName,
	const ChildActInfo* pHostInfo,
	CECModel* pParentModel,
	DWORD dwDeltaTime)
{
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		if (it->second->m_bCanAffectedByParent)
			it->second->ParentActivate(
				strActName,
				pHostInfo,
				pParentModel,
				dwDeltaTime);
}

void CECModel::UpdateFadeOutSfxLst(DWORD dwDelta)
{
	ALISTPOSITION pos = m_FadeOutSfxLst.GetHeadPosition();
	AM3DSoundBuffer* pSfx;

	while (pos)
	{
		ALISTPOSITION posCur = pos;
		pSfx = m_FadeOutSfxLst.GetNext(pos);
		pSfx->CheckEnd();

		if (pSfx->IsStopped())
		{
			AfxReleaseSoundNonLoop(pSfx);
			m_FadeOutSfxLst.RemoveAt(posCur);
		}
	}
}

bool CECModel::IsECMHookValid(const ECModelHookMap& hookMap, const char* szName) const
{
	// already exist one with this name
	if (hookMap.find(szName) != hookMap.end())
		return false;

	//	exist a A3DSkeletonHook with this name
	if (m_pA3DSkinModel && m_pA3DSkinModel->GetSkeleton() && m_pA3DSkinModel->GetSkeletonHook(szName, true))
		return false;

	return true;
}

bool CECModel::AddECMHook(const char* szName, int nID, float fScale, const A3DVECTOR3& vPos)
{
	if (!m_pMapModel)
		return false;

	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	
	// check the name is valid
	if (!IsECMHookValid(hookMap, szName))
		return false;

	CECModelHook* pNewHook = new CECModelHook;
	pNewHook->SetName(szName);
	pNewHook->SetRelativeMatrix(a3d_Translate(vPos.x, vPos.y, vPos.z));
	pNewHook->SetID(nID);
	pNewHook->SetScaleFactor(fScale);
	hookMap[szName] = pNewHook;
	return true;
}

bool CECModel::RenameECMHook(const char* szOldName, const char* szNewName)
{
	if (!m_pMapModel)
		return false;

	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	
	// already exist one with this name
	if (!IsECMHookValid(hookMap, szNewName))
		return false;

	CECModelHook* pHook = GetECMHook(szOldName);
	pHook->SetName(szNewName);
	return true;
}

void CECModel::RemoveECMHook(const char* szName)
{
	if (!m_pMapModel)
		return;

	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	ECModelHookMap::iterator itr = hookMap.find(szName);
	if (itr != hookMap.end()) {
		delete itr->second;
		hookMap.erase(itr);
	}
}

int CECModel::GetECMHookCount() const
{
	if (!m_pMapModel)
		return 0;

	return m_pMapModel->m_ECModelHookMap.size();
}

CECModelHook* CECModel::GetECMHook(int iIndex) const
{
	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	ECModelHookMap::iterator it = hookMap.begin();
	while (iIndex-- && it != hookMap.end()) ++it;
	if (it == hookMap.end())
		return NULL;
	return it->second;
}

CECModelHook* CECModel::GetECMHook(const char* szName) const
{
	if (!m_pMapModel)
		return NULL;

	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	ECModelHookMap::iterator citr = hookMap.find(szName);
	if (citr == hookMap.end())
		return NULL;

	return citr->second;
}

//	Inner help functions for gfx property transition
void CECModel::TransferEcmProperties(A3DGFXEx* pGfx)
{
	ASSERT(pGfx);

	pGfx->SetDisableCamShake(m_bDisableCamShake);
	pGfx->SetUseLOD(m_bGfxUseLod);
	pGfx->SetCreatedByGFXECM(m_bCreatedByGfx);
	pGfx->SetId(GetId());
}

void CECModel::PlayGfx(const char* szPath, const char* szHook, float fScale, bool bFadeOut, bool bUseECMHook, DWORD dwFadeOutTime)
{
	if (!bFadeOut)
		dwFadeOutTime = 0;

	PGFX_INFO pInfo;
	AString strKey = szPath;
	strKey += szHook;
	CoGfxMap::iterator it =	m_CoGfxMap.find(strKey);

	if (bUseECMHook) {
		if (CECModelHook* pHook = GetECMHook(szHook))
		{
			fScale *= pHook->GetScaleFactor();
		}
	}

	if (it != m_CoGfxMap.end())
	{
		pInfo = it->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();
		TransferEcmProperties(pGfx);
		pGfx->SetScale(fScale * m_fGfxScaleFactor);
		pInfo->SetFadeOutTime(dwFadeOutTime);
		pInfo->SetModelAlpha(true);
		pInfo->SetHookName(szHook);
		pInfo->SetUseECMHook(bUseECMHook);
		pInfo->SetScale(fScale);
		pGfx->SetSfxPriority(m_nSfxPriority);
		pGfx->Start(true);
		return;
	}

	pInfo = new GFX_INFO(NULL);
	pInfo->SetFilePath(szPath);
	pInfo->SetHookName(szHook);
	pInfo->SetUseECMHook(bUseECMHook);
	pInfo->SetScale(fScale);
	pInfo->Init(m_pA3DDevice);
	pInfo->LoadGfx();

	if (!pInfo->GetGfx())
	{
		delete pInfo;
		return;
	}

	pInfo->BuildTranMat();
	pInfo->SetFadeOutTime(dwFadeOutTime);
	pInfo->SetModelAlpha(true);
	pInfo->GetGfx()->SetScale(fScale * m_fGfxScaleFactor);
	pInfo->GetGfx()->SetSfxPriority(m_nSfxPriority);
	TransferEcmProperties(pInfo->GetGfx());
	pInfo->GetGfx()->Start(true);

	m_CoGfxMap[strKey] = pInfo;
}

void CECModel::PlayGfx(const char* szPath, const char* szHook, float fScale, bool bFadeOut, const A3DVECTOR3& vOffset, float fPitch, float fYaw, float fRot, bool bUseECMHook, DWORD dwFadeOutTime)
{
	if (!bFadeOut)
		dwFadeOutTime = 0;

	PGFX_INFO pInfo;
	AString strKey = szPath;
	strKey += szHook;
	CoGfxMap::iterator it =	m_CoGfxMap.find(strKey);
	if (bUseECMHook) {
		if (CECModelHook* pHook = GetECMHook(szHook))
		{
			fScale *= pHook->GetScaleFactor();
		}
	}
	
	if (it != m_CoGfxMap.end())
	{
		pInfo = it->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();
		TransferEcmProperties(pGfx);
		pGfx->SetScale(fScale * m_fGfxScaleFactor);
		pInfo->SetFadeOutTime(dwFadeOutTime);
		pInfo->SetModelAlpha(true);
		pInfo->SetHookName(szHook);
		pInfo->SetUseECMHook(bUseECMHook);
		pInfo->SetScale(fScale);
		pInfo->SetOffset(vOffset);
		pInfo->SetPitch(fPitch);
		pInfo->SetYaw(fYaw);
		pInfo->SetRot(fRot);
		pInfo->BuildTranMat();
		pGfx->SetSfxPriority(m_nSfxPriority);
		pGfx->Start(true);
		return;
	}

	pInfo = new GFX_INFO(NULL);
	pInfo->SetFilePath(szPath);
	pInfo->SetHookName(szHook);
	pInfo->SetUseECMHook(bUseECMHook);
	pInfo->SetScale(fScale);
	pInfo->Init(m_pA3DDevice);
	pInfo->SetOffset(vOffset);
	pInfo->SetPitch(fPitch);
	pInfo->SetYaw(fYaw);
	pInfo->SetRot(fRot);
	pInfo->LoadGfx();

	if (!pInfo->GetGfx())
	{
		delete pInfo;
		return;
	}

	pInfo->BuildTranMat();
	pInfo->SetFadeOutTime(dwFadeOutTime);
	pInfo->SetModelAlpha(true);
	TransferEcmProperties(pInfo->GetGfx());
	pInfo->GetGfx()->SetScale(fScale * m_fGfxScaleFactor);
	pInfo->GetGfx()->SetSfxPriority(m_nSfxPriority);
	pInfo->GetGfx()->Start(true);
	
	m_CoGfxMap[strKey] = pInfo;
}

void CECModel::RemoveGfx(const char* szPath, const char* szHook)
{
	AString strKey = szPath;
	strKey += szHook;
	RemoveGfx(strKey);
}

void CECModel::RemoveGfx(const char* szKey)
{
	CoGfxMap::iterator it = m_CoGfxMap.find(szKey);
	if (it == m_CoGfxMap.end()) return;

	delete it->second;
	m_CoGfxMap.erase(szKey);
}

A3DGFXEx* CECModel::GetGfx(const char* szPath, const char* szHook)
{
	AString strKey = szPath;
	strKey += szHook;

	CoGfxMap::iterator it = m_CoGfxMap.find(strKey);
	if (it == m_CoGfxMap.end()) return NULL;
	else return it->second->GetGfx();
}

bool CECModel::IsActionStopped(int nChannel)
{
	assert(nChannel >= 0 && nChannel < A3DSkinModel::ACTCHA_MAX);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();
	
	while (pos)
	{
		ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);
		
		if (!pNode->m_pActive->IsActionStopped())
			return false;
	}
	
	return true;
}

void CECModel::UpdateCoGfxHook()
{
	CoGfxMap::iterator it = m_CoGfxMap.begin();

	for (; it != m_CoGfxMap.end(); ++it)
	{
		PGFX_INFO pInfo = it->second;
		pInfo->Init(m_pA3DDevice);
		pInfo->LoadGfx();

		if (!pInfo->GetGfx())
			continue;

		A3DGFXEx* pGfx = pInfo->GetGfx();
		pGfx->SetScale(pInfo->GetScale() * m_fGfxScaleFactor);
		pGfx->SetAlpha(pInfo->GetAlpha());
		pGfx->SetPlaySpeed(pInfo->GetPlaySpeed());
		pGfx->SetSfxPriority(m_nSfxPriority);
		TransferEcmProperties(pGfx);
		pGfx->Start(true);
	}
}

void CECModel::SetGfxUseLOD(bool b, bool bForce)
{
	if (m_bGfxUseLod == b && !bForce)
		return;

	m_bGfxUseLod = b;
	CoGfxMap::iterator itGfx = m_CoGfxMap.begin();

	for (; itGfx != m_CoGfxMap.end(); ++itGfx)
	{
		PGFX_INFO pInfo = itGfx->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();

		if (pGfx)
			pGfx->SetUseLOD(b);
	}

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetGfxUseLOD(b, bForce);
}

void CECModel::SetDisableCamShake(bool b)
{ 
	if (m_bDisableCamShake == b)
		return;

	m_bDisableCamShake = b; 

	// DO NOT set disable camera shake property for GFX that is already playing
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetDisableCamShake(b);
}

//	Get current action's user data
DWORD CECModel::GetCurActionUserData(int nChannel)
{
	assert(nChannel >= 0 && nChannel < A3DSkinModel::ACTCHA_MAX);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.GetHighestRankNode();

	if (pNode)
		return pNode->m_pActive->GetUserData();

	return (DWORD)(-1);
}

inline bool _is_vec_nan(const A3DVECTOR3& v)
{
	return _isnan(v.x) || _isnan(v.y) || _isnan(v.z);
}

//	Set absolute position
void CECModel::SetPos(const A3DVECTOR3& vPos)
{
	if (_is_vec_nan(vPos))
	{
		assert(false);
		return;
	}

	A3DCoordinate::SetPos(vPos);

	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetPos(vPos);
}

//	Set absolute forward and up direction
void CECModel::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (_is_vec_nan(vDir) || _is_vec_nan(vUp))
	{
		assert(false);
		return;
	}

	A3DCoordinate::SetDirAndUp(vDir, vUp);

	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetDirAndUp(vDir, vUp);
}

//	Set absolute transform matrix
void CECModel::SetAbsoluteTM(const A3DMATRIX4& mat)
{
	A3DCoordinate::SetAbsoluteTM(mat);

	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetAbsoluteTM(mat);
}

//	Set aabb type
void CECModel::SetAABBType(int iType)
{
	m_iAABBType = iType;

	if (m_pA3DSkinModel)
	{
		if (iType == AABB_SKELETON)
			m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_SKELETON);
		else if (iType == AABB_INITMESH)
			m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);
		else
		{
			int iNumBone = m_pA3DSkinModel->GetSkeleton()->GetBoneNum();
			const A3DAABB& aabb = m_pA3DSkinModel->GetModelAABB();
			if (iNumBone >= 10)
				m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_SKELETON);
			else if (iNumBone < 3 || aabb.Extents.x < 0.6f || aabb.Extents.y < 0.6f || aabb.Extents.z < 0.6f)
				m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);
			else
				m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_SKELETON);
		}
	}
}

const A3DAABB& CECModel::GetModelAABB() const
{
	if (m_pA3DSkinModel)
		return m_ModelAABB;
	else
		return m_pMapModel->GetCHAABB();
}

//	Build wound action channel
bool CECModel::BuildWoundActionChannel()
{
	if (!m_pA3DSkinModel)
		return false;

	A3DSkeleton* pSkeleton = m_pA3DSkinModel->GetSkeleton();
	int i, iNumJoint = pSkeleton->GetJointNum();

	int* aJoints = new int [iNumJoint];
	if (!aJoints)
		return false;

	for (i=0; i < iNumJoint; i++)
		aJoints[i] = i;

	A3DSMActionChannel* pChannel = m_pA3DSkinModel->BuildActionChannel(ACTCHA_WOUND, iNumJoint, aJoints);

	if (pChannel)
		pChannel->SetPlayMode(A3DSMActionChannel::PLAY_COMBINE);

	delete [] aJoints;

	return pChannel ? true : false;
}

A3DModelPhysSync* CECModel::GetPhysSync()
{
#ifdef A3D_PHYSX
	return m_pModelPhysics ? m_pModelPhysics->GetModelSync() : NULL;
#else
	return NULL;
#endif
}

bool CECModel::SyncModelPhys()
{
#ifdef A3D_PHYSX
	
	// Sync all child models' physics
	for (ECModelMap::iterator it2 = m_ChildModels.begin(); it2 != m_ChildModels.end(); ++it2)
	{
		CECModel* pChild = it2->second;

		if (pChild->m_bShown)
			pChild->SyncModelPhys();
	}

	if (m_pModelPhysics)
		return m_pModelPhysics->SyncAfterPhysXSim();
	else
		return true;

#endif

	return true;
}

//	Resize physical actors and clothes after model size changed
bool CECModel::ResizePhysObjects()
{
#ifdef A3D_PHYSX

	if (!m_bPhysEnabled)
		return true;

	if (!m_pModelPhysics)
		return false;

	struct SKIN_STATE
	{
		bool	bCloth;		//	Changed to cloth
		int		iLinkType;	//	Link type
	};

	//	Record what skins have been changed to cloth and their link type
	SKIN_STATE aSkinStates[256];
	memset(aSkinStates, 0, sizeof aSkinStates);

	int i, iNumSkin = m_pModelPhysics->GetSkinSlotNum();
	ASSERT(iNumSkin <= 256);

	for (i=0; i < iNumSkin; i++)
	{
		if (m_pModelPhysics->IsClothSkin(i))
			aSkinStates[i].bCloth = true;

		aSkinStates[i].iLinkType = m_pModelPhysics->GetClothLinkType(i);
	}

	//	Cloth all clothes
	CloseAllClothes(true);

	//	Create model physics
	if (!m_pModelPhysics->CreateModelSync(m_pMapModel->m_pPhysSyncData))
		return false;

	//	Open all clothes
	for (i=0; i < iNumSkin; i++)
	{
		const SKIN_STATE& st = aSkinStates[i];
		if (st.bCloth)
			m_pModelPhysics->OpenClothSkin(i, st.iLinkType);
	}

#endif	//	A3D_PHYSX

	return true;
}

//	Remove combo model
void CECModel::RemoveComboModel(CECModel* pModel)
{
	int n = FindComboModel(pModel);
	if (n >= 0)
		RemoveComboModelByIndex(n);
}

void CECModel::SetMotionBlurInfo(ECMActionBlurInfo* pInfo)
{
	delete m_pBlurInfo;
	m_pBlurInfo = pInfo;

	if (m_pA3DSkinModel)
	{
		A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();

		if (pSke)
		{
			for (size_t i = 0; i < pInfo->m_Colors.size() + 1; i++)
			{
				ECMBlendMatricesSet* pSet = new ECMBlendMatricesSet;
				int nBoneNum = pSke->GetBoneNum();
				pSet->reserve(nBoneNum);

				for (int n = 0; n < nBoneNum; n++)
					pSet->push_back(A3DMATRIX4());

				pInfo->m_MatricesCollector.AddTail(pSet);
			}
		}
	}

	if (pInfo->m_bApplyToChildren)
	{
		for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		{
			CECModel* pChild = it->second;
			ECMActionBlurInfo* pChildInfo = new ECMActionBlurInfo;

			for (size_t i = 0; i < pInfo->m_Colors.size(); i++)
				pChildInfo->m_Colors.push_back(pInfo->m_Colors[i]);

			pChildInfo->m_dwTotalTime = pInfo->m_dwTotalTime;
			pChildInfo->m_dwInterval = pInfo->m_dwInterval;
			pChildInfo->m_bApplyToChildren = true;
			pChildInfo->m_bHighlight = pInfo->m_bHighlight;
			pChildInfo->m_bRoot = false;
			pChild->SetMotionBlurInfo(pChildInfo);
		}
	}
}

void CECModel::ReloadPixelShader()
{
	if (m_pPixelShader)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseShader(&m_pPixelShader);
		m_pPixelShader = NULL;
	}

	if (!m_pMapModel->m_strPixelShader.IsEmpty())
	{
		m_pPixelShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(m_pMapModel->m_strPixelShader);

		if (m_pPixelShader)
		{
#ifdef _ANGELICA21
			if (!m_pMapModel->m_strShaderTex.IsEmpty())
			{
				AString strPath = "Gfx\\Textures\\" + m_pMapModel->m_strShaderTex;
				m_pPixelShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)(const char*)strPath);
			}
#else
			if (!m_pMapModel->m_strShaderTex.IsEmpty())
			{
				AString strPath = "Gfx\\Textures\\" + m_pMapModel->m_strShaderTex;
				m_pPixelShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)(const char*)strPath);
			}
#endif
			m_dwPSTickTime = 0;
		}
	}
}

void CECModel::SetChildRenderFlag(DWORD dwFlag)
{
#ifdef _ANGELICA21

	m_dwChildRenderFlag = dwFlag;

	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetChildRenderFlag(dwFlag);

#endif
}

void CECModel::FinalizeCustomRenderFunc()
{
#ifdef _ANGELICA21
	a3d_FinalizeCustomRender();
#endif
}

CECModelStaticData::CECModelStaticData() :
m_bActMapped(false),
m_bAutoUpdate(true),
m_dwVersion(0),
m_OrgColor(A3DCOLORRGBA(255, 255, 255, 255)),
m_EmissiveColor(A3DCOLORRGBA(0, 0, 0, 0)),
m_fDefPlaySpeed(1.0f),
m_bCanCastShadow(true),
m_bRenderSkinModel(true),
m_bRenderEdge(true),
m_bChannelInfoInit(false),
m_bInitGlobalScript(false)
{
	for (int i = 0; i < OUTER_DATA_COUNT; i++)
		m_OuterData[i] = 1.0f;

	m_BlendMode.SrcBlend = A3DBLEND_SRCALPHA;
	m_BlendMode.DestBlend = A3DBLEND_INVSRCALPHA;
	m_CHAABB.Clear();
	memset(m_ScriptEnable, 0, sizeof(m_ScriptEnable));
	memset(m_ChannelInfoArr, 0, sizeof(m_ChannelInfoArr));
	memset(m_EventMasks, -1, sizeof(m_EventMasks));

#ifdef A3D_PHYSX

	m_pPhysSyncData = new A3DModelPhysSyncData;
	
#else

	m_pPhysSyncData = NULL;

#endif
}

bool CECModelStaticData::LoadData(const char* szModelFile, bool bLoadAdditionalSkin)
{
	if (szModelFile[0] == '\0')
		return false;

	AFileImage file;

#ifdef _ANGELICA22
	if (!file.Open(szModelFile, AFILE_OPENEXIST | MODEL_FILE_TYPE | AFILE_TEMPMEMORY))
#else
	if (!file.Open(szModelFile, AFILE_OPENEXIST | MODEL_FILE_TYPE ))
#endif
	{
		a_LogOutput(1, "%s, Failed to open ecm file %s", "CECModelStaticData::LoadData", szModelFile);
		return false;
	}

#ifdef GFX_EDITOR

	Release();

#endif

	A3DDevice* pA3DDevice = AfxGetA3DDevice();
	m_strFilePath = szModelFile;
	AString strECMTable = _format_ecm_table_name(this);
	m_ScriptMemTbl.Init(strECMTable);

	DWORD dwReadLen;
	int i;
	int nComActCount	= 0;
	int nCoGfxNum		= 0;
	int nBoneScaleNum	= 0;
	int nOuterDataNum	= 0;

	if (file.IsBinary())
	{
		file.Read(&m_dwVersion, sizeof(m_dwVersion), &dwReadLen);
		file.ReadString(m_strSkinModelPath);
		file.Read(&m_OrgColor, sizeof(m_OrgColor), &dwReadLen);
		file.Read(&m_BlendMode, sizeof(m_BlendMode), &dwReadLen);

		file.Read(&nOuterDataNum, sizeof(nOuterDataNum), &dwReadLen);
		for (i = 0; i < nOuterDataNum; i++)
			file.Read(&m_OuterData[i], sizeof(m_OuterData[i]), &dwReadLen);

		file.Read(&nBoneScaleNum, sizeof(nBoneScaleNum), &dwReadLen);
		for (i = 0; i < nBoneScaleNum; i++)
		{
			BoneScale* pBoneScale = new BoneScale;
			file.Read(&pBoneScale->m_nIndex, sizeof(pBoneScale->m_nIndex), &dwReadLen);
			file.Read(&pBoneScale->m_nType, sizeof(pBoneScale->m_nType), &dwReadLen);
			file.Read(&pBoneScale->m_vScale.m, sizeof(pBoneScale->m_vScale.m), &dwReadLen);
		}

		file.Read(&nCoGfxNum, sizeof(nCoGfxNum), &dwReadLen);
		file.Read(&nComActCount, sizeof(nComActCount), &dwReadLen);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];

		file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_file_version, &m_dwVersion);

		// Version check is necessary in Client
		if (m_dwVersion > EC_MODEL_CUR_VERSION)
		{
			a_LogOutput(1, "%s, Wrong file version %u (%u or lower required).", "CECModelStaticData::LoadData", m_dwVersion, EC_MODEL_CUR_VERSION);
			m_dwVersion = (DWORD)-1;
			file.Close();
			return false;
		}

		// Notion: some files of version 37 would get a single file name here
		// This is because we tried to save only file name in this version which is proved inconsistent with the Project of ReWuPaiDui.
		// As we changed back to save the relative pathname and process as if all files have saved with the pathname.
		// Some file of version 37 would be opened failed and need a fix operation in the ECM editor.
		szBuf[0] = '\0';
		file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_skinmodel_path, szBuf);
		m_strSkinModelPath = szBuf;

		if (m_dwVersion >= 33)
		{
			int nBool;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_autoupdate, &nBool);
			m_bAutoUpdate = (nBool != 0);
		}

		if (m_dwVersion >= 16)
		{
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_org_color, &m_OrgColor);
			
			if (m_dwVersion >= 52)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_emissive_col, &m_EmissiveColor);
			}

			if (m_dwVersion >= 21)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_srcbld, &m_BlendMode.SrcBlend);

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_destbld, &m_BlendMode.DestBlend);
			}

			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_outer_num, &nOuterDataNum);

			for (i = 0; i < nOuterDataNum; i++)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_float, &m_OuterData[i]);
			}

			bool bNewBoneScaleMethod = false;

			if (m_dwVersion >= 28)
			{
				int nRead;
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_new_scale, &nRead);
				bNewBoneScaleMethod = (nRead != 0);
			}

			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_bone_num, &nBoneScaleNum);

			if (bNewBoneScaleMethod)
			{
				for (i = 0; i < nBoneScaleNum; i++)
				{
					BoneScaleEx* pBoneScale = new BoneScaleEx;

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_index, &pBoneScale->m_nIndex);

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_scale, &pBoneScale->m_fLenFactor, &pBoneScale->m_fThickFactor, &pBoneScale->m_fWholeFactor);

					m_BoneScaleExArr.push_back(pBoneScale);
				}
			}
			else
			{
				for (i = 0; i < nBoneScaleNum; i++)
				{
					BoneScale* pBoneScale = new BoneScale;

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_index, &pBoneScale->m_nIndex);

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_scl_type, &pBoneScale->m_nType);

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_scale, VECTORADDR_XYZ(pBoneScale->m_vScale));

					m_BoneScales.push_back(pBoneScale);
				}
			}

			if (m_dwVersion >= 29)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				m_strScaleBaseBone = szLine;
			}
		}

		if (m_dwVersion >= 27)
		{
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_def_speed, &m_fDefPlaySpeed);
		}

		if (m_dwVersion >= 43)
		{
			int nBool = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_can_castshadow, &nBool);
			m_bCanCastShadow = (nBool != 0);
		}

		if (m_dwVersion >= 45)
		{
			int nBool = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_render_model, &nBool);
			m_bRenderSkinModel = (nBool != 0);
		}

		if (m_dwVersion >= 48)
		{
			int nBool = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_render_edge, &nBool);
			m_bRenderEdge = (nBool != 0);
		}

		if (m_dwVersion >= 57)
		{
			if (m_dwVersion >= 60)
				g_GfxLoadPixelShaderConsts(&file, m_strPixelShader, m_strShaderTex, m_vecPSConsts);
			else
			{
				char szBuf[MAX_PATH];
				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_shaderpath, szBuf);
				m_strPixelShader = szBuf;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_shadertex, szBuf);
				m_strShaderTex = szBuf;

				int i, j;
				int nPSConstCount = 0;
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_psconstcount, &nPSConstCount);
				m_vecPSConsts.reserve(nPSConstCount);

				int nTargetCount;

				for (i = 0; i < nPSConstCount; i++)
				{
					GfxPixelShaderConst psconst;
					A3DCOLORVALUE& value = psconst.init_val;

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_psindex, &psconst.index);

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_psvalue, &value.r, &value.g, &value.b, &value.a);

					if (m_dwVersion >= 59)
					{
						file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLine, _format_psloop, &psconst.loop_count);
					}
					else
						psconst.loop_count = 1;

					nTargetCount = 0;
					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_pstargetcount, &nTargetCount);
					psconst.target_vals.reserve(nTargetCount);

					for (j = 0; j < nTargetCount; j++)
					{
						GfxPixelShaderTargetValue tv;
						A3DCOLORVALUE& value = tv.value;

						file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLine, _format_psinterval, &tv.interval);

						if (int(tv.interval) < 0 && m_dwVersion < 59)
							tv.interval = 0;

						file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLine, _format_psvalue, &value.r, &value.g, &value.b, &value.a);

						psconst.target_vals.push_back(tv);
					}

					psconst.CalcTotalTime();
					m_vecPSConsts.push_back(psconst);
				}
			}
		}

		if (m_dwVersion >= 31)
		{
			int channel_count = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_channel_count, &channel_count);

			for (i = 0; i < channel_count; i++)
			{
				ActChannelInfo* pChannel = new ActChannelInfo;
				int channel = 0;
				int bone_num = 0;
				int j;

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_channel, &channel);

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_bone_num, &bone_num);

				for (j = 0; j < bone_num; j++)
				{
					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					pChannel->bone_names.push_back(AString(szLine));
				}

				if (channel >= 0 && channel < A3DSkinModel::ACTCHA_MAX)
				{
					delete m_ChannelInfoArr[channel];
					m_ChannelInfoArr[channel] = pChannel;
				}
				else
				{
					delete pChannel;
					assert(false);
				}
			}
		}

		if (m_dwVersion >= 32)
		{
			int count = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_channel_count, &count);

			for (i = 0; i < count; i++)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_channel_mask, &m_EventMasks[i]);
			}
		}

		if (m_dwVersion >= 14)
		{
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_cogfx_num, &nCoGfxNum);
		}

		file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_comact_count, &nComActCount);
	}

	for (i = 0; i < nCoGfxNum; i++)
	{
		PGFX_INFO pEvent = static_cast<PGFX_INFO>(EVENT_INFO::LoadFromFile(
				NULL,
				&file,
				m_dwVersion));

		if (!pEvent) continue;

		char buf[20];
		sprintf(buf, "%d_co", i);

		pEvent->Init(pA3DDevice);
		m_CoGfxMap[AString(buf)] = pEvent;
	}

	for (i = 0; i < nComActCount; i++)
	{
		A3DCombinedAction* pAct = new A3DCombinedAction();

		if (!pAct->Load(pA3DDevice, &file, m_dwVersion))
		{
			delete pAct;
			continue;
		}

		AString strName(pAct->GetName());
		CombinedActMap::iterator it = m_ActionMap.find(strName);

		if (it != m_ActionMap.end())
			delete it->second;

		m_ActionMap[strName] = pAct;
	}

	if (file.IsBinary())
	{
	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];

		if (m_dwVersion >= 25)
		{
			int nScriptCount = 0;

			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_script_count, &nScriptCount);

			for (i = 0; i < nScriptCount; i++)
			{
				int id = -1;
				int len = 0;
				int lines = 1;
				AString str;

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_id, &id);

				if (m_dwVersion >= 26)
				{
					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_script_lines, &lines);
				}

				while (lines)
				{
					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					str += szLine;
					lines--;
				}

				int buf_len = str.GetLength() + 32;
				char* pBuf = new char[buf_len];
				len = base64_decode((char*)(const char*)str, str.GetLength(), (unsigned char*)pBuf);
				pBuf[len] = 0;

				if (id >= 0 && id < enumECMScriptCount)
				{
					m_Scripts[id] = pBuf;
					AddScriptMethod(id, pBuf);
				}

				delete[] pBuf;
			}
		}
	}

	//	Load A3D skin model from file
	if (m_dwVersion >= 8)
		LoadAdditionalSkin(&file, m_dwVersion, bLoadAdditionalSkin);

	if (m_dwVersion >= 5)
	{
		int nChildCount = 0;

		if (file.IsBinary())
		{
			file.Read(&nChildCount, sizeof(nChildCount), &dwReadLen);

			for (int i = 0; i < nChildCount; i++)
			{
				ChildInfo* pChild = new ChildInfo;

				file.ReadString(pChild->m_strName);
				file.ReadString(pChild->m_strPath);
				file.ReadString(pChild->m_strHHName);
				file.ReadString(pChild->m_strCCName);

				m_ChildInfoArray.push_back(pChild);
			}
		}
		else
		{
			char	szLine[AFILE_LINEMAXLEN];
			char	szBuf[AFILE_LINEMAXLEN];

			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_child_count, &nChildCount);

			for (int i = 0; i < nChildCount; i++)
			{
				ChildInfo* pChild = new ChildInfo;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_child_name, szBuf);
				pChild->m_strName = szBuf;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_child_path, szBuf);
				pChild->m_strPath = szBuf;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_hh_name, szBuf);
				pChild->m_strHHName = szBuf;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_cc_name, szBuf);
				pChild->m_strCCName = szBuf;

				m_ChildInfoArray.push_back(pChild);
			}

			if (m_dwVersion >= 34)
			{
				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_phys_file, szBuf);
				m_strPhysFileName = szBuf;
			}

			if (m_dwVersion >= 41)
			{

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				int iCount = 0;
				sscanf(szLine, _format_ecmhook_count, &iCount);
		
				for (int i = 0; i < iCount; ++i)
				{
					CECModelHook* pHook = new CECModelHook;
					if (!pHook->Load(&file, m_dwVersion))
					{
						delete pHook;
						continue;
					}

					ECModelHookMap::iterator itr = m_ECModelHookMap.find(pHook->GetName());
					if (itr != m_ECModelHookMap.end())
					{
						delete itr->second;
					}

					m_ECModelHookMap[pHook->GetName()] = pHook;
				}
			}
		}
	}

	file.Close();

	if (m_ScriptEnable[enumECMScriptInit])
	{
		abase::vector<CScriptValue> args, results;
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptInit], args, results);
	}

	if (m_ScriptEnable[enumECMScriptChangeEquipTableInit])
	{
		abase::vector<CScriptValue> args, results;
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptChangeEquipTableInit], args, results);
	}

#ifdef A3D_PHYSX

	if (!m_strPhysFileName.IsEmpty())
	{
		char sz[MAX_PATH];
		strcpy(sz, szModelFile);
		strcpy(ECMPathFindFileNameA(sz), m_strPhysFileName);

		if (af_IsFileExist(sz))
		{
			if (!m_pPhysSyncData)
				m_pPhysSyncData = new A3DModelPhysSyncData;
			
			if (!m_pPhysSyncData->Load(sz))
			{
				delete m_pPhysSyncData;
				m_pPhysSyncData = new A3DModelPhysSyncData;
			}
		}
	}

#endif

#ifndef GFX_EDITOR

	const char* szPath = strstr(szModelFile, "\\");
	if (!szPath) return true;

	char szProjFile[MAX_PATH];
	strcpy(szProjFile, AfxGetECMHullPath());
	strcat(szProjFile, szPath);

	char* ext = ECMPathFindExtensionA(szProjFile);
	if (*ext==0) return true;
	strcpy(ext, ".ecp");

	if (!af_IsFileExist(szProjFile) || !file.Open(szProjFile, AFILE_OPENEXIST | AFILE_BINARY))
		return true;

	ECModelBrushProjHead_t ProjHead;
	DWORD dwLen;

	file.Read(&ProjHead, sizeof(ProjHead), &dwLen);
	m_CHAABB.Clear();

	if (ProjHead.nVersion == ECMODEL_PROJ_VERSION)
	{
		for (unsigned i = 0; i < ProjHead.nNumHull; i++)
		{
			CHBasedCD::CConvexHullData* pHull = new CHBasedCD::CConvexHullData;
			pHull->LoadBinaryData(&file);
			// currently, the ECModelHullEditor has turned the model 180 degree which is a mistake,
			// so we have to turn it back before we use it.
			pHull->Transform(TransformMatrix(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0), A3DVECTOR3(0)));
			m_ConvexHullDataArr.push_back(pHull);
			m_CHAABB.Merge(pHull->GetAABB());
		}
	}

	file.Close();

#endif

	return true;
}

bool CECModelStaticData::Save(const char* szFile, CECModel* pModel)
{
	AFile file;

	if (!file.Open(szFile, AFILE_CREATENEW | MODEL_FILE_TYPE))
		return false;

	m_dwVersion = EC_MODEL_CUR_VERSION;

	if (file.IsBinary())
	{
		DWORD dwWrite;
		size_t size, i;

		file.Write(&m_dwVersion, sizeof(m_dwVersion), &dwWrite);
		file.WriteString(m_strSkinModelPath);
		file.Write(&m_OrgColor, sizeof(m_OrgColor), &dwWrite);
		file.Write(&m_BlendMode, sizeof(m_BlendMode), &dwWrite);

		i = OUTER_DATA_COUNT;
		file.Write(&i, sizeof(i), &dwWrite);
		for (i = 0; i < OUTER_DATA_COUNT; i++)
			file.Write(&m_OuterData[i], sizeof(float), &dwWrite);

		size = m_BoneScales.size();
		file.Write(&size, sizeof(size), &dwWrite);

		for (i = 0; i < size; i++)
		{
			BoneScale* pScale = m_BoneScales[i];

			file.Write(&pScale->m_nIndex, sizeof(pScale->m_nIndex), &dwWrite);
			file.Write(&pScale->m_nType, sizeof(pScale->m_nType), &dwWrite);
			file.Write(&pScale->m_vScale.m, sizeof(pScale->m_vScale.m), &dwWrite);
		}

		size = m_ActionMap.size();
		file.Write(&size, sizeof(size), &dwWrite);

		/*
		 *	Modified
		 */
		for (CombinedActMap::iterator it_act = m_ActionMap.begin(); it_act != m_ActionMap.end(); ++it_act)
			it_act->second->Save(&file);

		SaveAdditionalSkin(&file);
		
		size = m_CoGfxMap.size();
		file.Write(&size, sizeof(size), &dwWrite);

		size = pModel->m_ChildModels.size();
		file.Write(&size, sizeof(size), &dwWrite);

		for (CoGfxMap::iterator it = m_CoGfxMap.begin(); it != m_CoGfxMap.end(); ++it)
			it->second->Save(&file);

		for (ECModelMap::iterator it_child = pModel->m_ChildModels.begin(); it_child != pModel->m_ChildModels.end(); ++it_child)
		{
			file.WriteString(it_child->first);
			file.WriteString(it_child->second->m_pMapModel->m_strFilePath);
			file.WriteString(it_child->second->m_strHookName);
			file.WriteString(it_child->second->m_strCC);
		}
	}
	else
	{
		size_t	i;
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_file_version, m_dwVersion);
		file.WriteLine(szLine);

		sprintf(szLine, _format_skinmodel_path, m_strSkinModelPath);
		file.WriteLine(szLine);

		sprintf(szLine, _format_autoupdate, m_bAutoUpdate);
		file.WriteLine(szLine);

		sprintf(szLine, _format_org_color, m_OrgColor);
		file.WriteLine(szLine);

		sprintf(szLine, _format_emissive_col, m_EmissiveColor);
		file.WriteLine(szLine);

		sprintf(szLine, _format_srcbld, m_BlendMode.SrcBlend);
		file.WriteLine(szLine);

		sprintf(szLine, _format_destbld, m_BlendMode.DestBlend);
		file.WriteLine(szLine);				

		sprintf(szLine, _format_outer_num, OUTER_DATA_COUNT);
		file.WriteLine(szLine);

		for (i = 0; i < OUTER_DATA_COUNT; i++)
		{
			sprintf(szLine, _format_float, m_OuterData[i]);
			file.WriteLine(szLine);
		}

		bool bNewBoneScaleMethod = g_pA3DConfig->GetFlagNewBoneScale();
		sprintf(szLine, _format_new_scale, bNewBoneScaleMethod);
		file.WriteLine(szLine);

		if (bNewBoneScaleMethod)
		{
			sprintf(szLine, _format_bone_num, m_BoneScaleExArr.size());
			file.WriteLine(szLine);

			for (i = 0; i < m_BoneScaleExArr.size(); i++)
			{
				BoneScaleEx* pScale = m_BoneScaleExArr[i];

				sprintf(szLine, _format_bone_index, pScale->m_nIndex);
				file.WriteLine(szLine);

				sprintf(szLine, _format_bone_scale, pScale->m_fLenFactor, pScale->m_fThickFactor, pScale->m_fWholeFactor);
				file.WriteLine(szLine);
			}
		}
		else
		{
			sprintf(szLine, _format_bone_num, m_BoneScales.size());
			file.WriteLine(szLine);

			for (i = 0; i < m_BoneScales.size(); i++)
			{
				BoneScale* pScale = m_BoneScales[i];

				sprintf(szLine, _format_bone_index, pScale->m_nIndex);
				file.WriteLine(szLine);

				sprintf(szLine, _format_bone_scl_type, pScale->m_nType);
				file.WriteLine(szLine);

				sprintf(szLine, _format_bone_scale, VECTOR_XYZ(pScale->m_vScale));
				file.WriteLine(szLine);
			}
		}

		file.WriteLine(m_strScaleBaseBone);

		sprintf(szLine, _format_def_speed, m_fDefPlaySpeed);
		file.WriteLine(szLine);

		sprintf(szLine, _format_can_castshadow, (int)m_bCanCastShadow);
		file.WriteLine(szLine);

		sprintf(szLine, _format_render_model, (int)m_bRenderSkinModel);
		file.WriteLine(szLine);

		sprintf(szLine, _format_render_edge, (int)m_bRenderEdge);
		file.WriteLine(szLine);

		g_GfxSavePixelShaderConsts(&file, m_strPixelShader, m_strShaderTex, m_vecPSConsts);
		int channel_count = 0;

		for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			ActChannelInfo* pChannel = m_ChannelInfoArr[i];

			if (pChannel && pChannel->bone_names.size())
				channel_count++;
		}

		sprintf(szLine, _format_channel_count, channel_count);
		file.WriteLine(szLine);

		for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			ActChannelInfo* pChannel = m_ChannelInfoArr[i];

			if (pChannel && pChannel->bone_names.size())
			{
				sprintf(szLine, _format_channel, i);
				file.WriteLine(szLine);

				sprintf(szLine, _format_bone_num, (int)pChannel->bone_names.size());
				file.WriteLine(szLine);

				for (size_t j = 0; j < pChannel->bone_names.size(); j++)
				{
					file.WriteLine(pChannel->bone_names[j]);
				}
			}
		}

		sprintf(szLine, _format_channel_count, A3DSkinModel::ACTCHA_MAX);
		file.WriteLine(szLine);

		for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			sprintf(szLine, _format_channel_mask, m_EventMasks[i]);
			file.WriteLine(szLine);
		}

		sprintf(szLine, _format_cogfx_num, m_CoGfxMap.size());
		file.WriteLine(szLine);

		sprintf(szLine, _format_comact_count, m_ActionMap.size());
		file.WriteLine(szLine);

		for (CoGfxMap::iterator it = m_CoGfxMap.begin(); it != m_CoGfxMap.end(); ++it)
			it->second->Save(&file);

		abase::vector<A3DCombinedAction*> act_sort;

		for (CombinedActMap::iterator it_act = m_ActionMap.begin(); it_act != m_ActionMap.end(); ++it_act)
			act_sort.push_back(it_act->second);

		qsort(act_sort.begin(), act_sort.size(), sizeof(int), _str_compare);

		for (i = 0; i < act_sort.size(); i++)
			act_sort[i]->Save(&file);

		int nScriptCount = 0;

		for (i = 0; i < enumECMScriptCount; i++)
			if (!m_Scripts[i].IsEmpty())
				nScriptCount++;

		sprintf(szLine, _format_script_count, nScriptCount);
		file.WriteLine(szLine);

		for (i = 0; i < enumECMScriptCount; i++)
		{
			if (!m_Scripts[i].IsEmpty())
			{
				sprintf(szLine, _format_id, i);
				file.WriteLine(szLine);

				AString& str = m_Scripts[i];
				int nBufLen = (str.GetLength() + 3) / 3 * 4 + 32;
				char* pBuf = new char[nBufLen];
				int len = base64_encode((unsigned char*)(const char*)str, str.GetLength()+1, pBuf);
				int nLines = len / 1500;

				if (len > nLines * 1500)
					nLines++;

				sprintf(szLine, _format_script_lines, nLines);
				file.WriteLine(szLine);

				const char* pWrite = pBuf;

				while (len)
				{
					int nWrite = len > 1500 ? 1500 : len;
					len -= nWrite;

					AString s(pWrite, nWrite);
					file.WriteLine(s);
					pWrite += nWrite;
				}

				delete[] pBuf;
			}
		}

		SaveAdditionalSkin(&file);

		if (pModel)
		{
			sprintf(szLine, _format_child_count, pModel->m_ChildModels.size());
			file.WriteLine(szLine);

			for (ECModelMap::iterator it_child = pModel->m_ChildModels.begin(); it_child != pModel->m_ChildModels.end(); ++it_child)
			{
				sprintf(szLine, _format_child_name, it_child->first);
				file.WriteLine(szLine);

				sprintf(szLine, _format_child_path, it_child->second->m_pMapModel->m_strFilePath);
				file.WriteLine(szLine);

				sprintf(szLine, _format_hh_name, it_child->second->m_strHookName);
				file.WriteLine(szLine);

				sprintf(szLine, _format_cc_name, it_child->second->m_strCC);
				file.WriteLine(szLine);
			}
		}
		else
		{
			sprintf(szLine, _format_child_count, m_ChildInfoArray.size());
			file.WriteLine(szLine);

			for (i = 0; i < m_ChildInfoArray.size(); i++)
			{
				const ChildInfo* pChildInfo = m_ChildInfoArray[i];
				sprintf(szLine, _format_child_name, (const char*)pChildInfo->m_strName);
				file.WriteLine(szLine);

				sprintf(szLine, _format_child_path, (const char*)pChildInfo->m_strPath);
				file.WriteLine(szLine);

				sprintf(szLine, _format_hh_name, (const char*)pChildInfo->m_strHHName);
				file.WriteLine(szLine);

				sprintf(szLine, _format_cc_name, (const char*)pChildInfo->m_strCCName);
				file.WriteLine(szLine);
			}
		}

		sprintf(szLine, _format_phys_file, m_pPhysSyncData ? m_strPhysFileName : "");
		file.WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_ecmhook_count, m_ECModelHookMap.size());
		file.WriteLine(szLine);

		for (ECModelHookMap::iterator it_ecmhook = m_ECModelHookMap.begin(); it_ecmhook != m_ECModelHookMap.end(); ++it_ecmhook)
		{
			CECModelHook* pHook = it_ecmhook->second;
			if (!pHook)
				continue;

			pHook->Save(&file);
		}

	}

	file.Close();

#ifdef A3D_PHYSX

	if (m_pPhysSyncData)
	{
		char sz [MAX_PATH];
		strcpy(sz, szFile);
		strcpy(ECMPathFindFileNameA(sz), m_strPhysFileName);

		if (!m_pPhysSyncData->Save(sz))
			return false;
	}

#endif

	return true;
}

bool CECModelStaticData::SaveBoneScaleInfo(const char* szFile)
{
	AFile file;

	if (!file.Open(szFile, AFILE_CREATENEW | AFILE_BINARY))
		return false;

	DWORD len;
	int nNum = m_BoneScaleExArr.size();
	file.Write(&nNum, sizeof(nNum), &len);

	for (int i = 0; i < nNum; i++)
	{
		BoneScaleEx* p = m_BoneScaleExArr[i];
		file.Write(p, sizeof(BoneScaleEx), &len);
	}

	nNum = m_strScaleBaseBone.GetLength();
	file.Write(&nNum,sizeof(nNum), &len);
	file.Write((void*)(const char*)m_strScaleBaseBone, nNum, &len);

	file.Close();
	return true;
}

void CECModelStaticData::SaveAdditionalSkin(AFile* pFile)
{
	DWORD dwWrite;

	if (pFile->IsBinary())
	{
		size_t iCount = m_AdditionalSkinLst.size();
		pFile->Write(&iCount, sizeof(iCount), &dwWrite);

		for (size_t i = 0; i < iCount; i++)
			pFile->WriteString(m_AdditionalSkinLst[i]);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_addi_skin_count, m_AdditionalSkinLst.size());
		pFile->WriteLine(szLine);

		for (size_t i = 0; i < m_AdditionalSkinLst.size(); i++)
		{
			sprintf(szLine, _format_addi_skin_path, m_AdditionalSkinLst[i]);
			pFile->WriteLine(szLine);
		}
	}
}

void CECModelStaticData::Reset()
{
	int i;

	m_OrgColor = A3DCOLORRGBA(255, 255, 255, 255);
	m_EmissiveColor = A3DCOLORRGBA(0, 0, 0, 0);
	m_fDefPlaySpeed = 1.0f;
	m_BlendMode.SrcBlend = A3DBLEND_SRCALPHA;
	m_BlendMode.DestBlend = A3DBLEND_INVSRCALPHA;


	for (i = 0; i < OUTER_DATA_COUNT; i++)
		m_OuterData[i] = 1.0f;

	for (i = 0; i < (int)m_BoneScales.size(); i++)
		delete m_BoneScales[i];

	m_BoneScales.clear();

	for (i = 0; i < (int)m_BoneScaleExArr.size(); i++)
		delete m_BoneScaleExArr[i];

	m_BoneScaleExArr.clear();
	m_strScaleBaseBone.Empty();

	for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		delete m_ChannelInfoArr[i];
		m_ChannelInfoArr[i] = 0;
	}

	m_strPhysFileName.Empty();
}

void CECModelStaticData::AddScriptMethod(int index, const char* szScript)
{
	abase::vector<AString> vargs;
	int var_count = ecm_get_var_count(index);

	for (int i = 0; i < var_count; i++)
		vargs.push_back(ecm_get_var_name(index, i));

	m_ScriptEnable[index] = m_ScriptMemTbl.AddMethod(_script_func_name[index], vargs, szScript);
}

void CECModelStaticData::OnScriptPlayAction(CECModel* pModel, int nChannel, const char* szActName)
{
	using namespace LuaBind;

	if (m_ScriptEnable[enumECMScriptStartAction])
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath + szActName);
#endif

		abase::vector<CScriptValue> args, results;
		args.push_back(CScriptValue((void *)pModel));
		args.push_back(CScriptValue((double)nChannel));
		CScriptString str;
		str.SetUtf8(szActName, strlen(szActName));
		args.push_back(CScriptValue(str));
		args.push_back(CScriptValue(pModel->m_bFashionMode));
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptStartAction], args, results);
	}
}

void CECModelStaticData::OnScriptEndAction(CECModel* pModel, int nChannel, const char* szActName)
{
	using namespace LuaBind;

	if (m_ScriptEnable[enumECMScriptEndActioin])
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath + szActName);
#endif

		abase::vector<CScriptValue> args, results;
		args.push_back(CScriptValue((void*)pModel));
		args.push_back(CScriptValue((double)nChannel));
		CScriptString str;
		str.SetUtf8(szActName, strlen(szActName));
		args.push_back(CScriptValue(str));
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptEndActioin], args, results);
	}
}

void CECModelStaticData::OnScriptChangeEquip(CECModel* pModel, int nEquipId, int nEquipFlag, bool bFashionMode, int nPathId, int nEquipIndex)
{
	using namespace LuaBind;

	if (m_ScriptEnable[enumECMScriptChangeEquip])
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath);
#endif

		abase::vector<CScriptValue> args, results;
		args.push_back(CScriptValue((void *)pModel));
		args.push_back(CScriptValue((double)nEquipId));
		args.push_back(CScriptValue((double)nPathId));
		args.push_back(CScriptValue((double)nEquipFlag));
		args.push_back(CScriptValue((double)nEquipIndex));
		args.push_back(CScriptValue(bFashionMode));
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptChangeEquip], args, results);
	}
}

void CECModelStaticData::OnScriptPhysBreak(CECModel* pModel, float fBreakOffsetX, float fBreakOffsetY, float fBreakOffsetZ)
{
	using namespace LuaBind;

	if (m_ScriptEnable[enumECMScriptPhysBreak])
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath);
#endif

		abase::vector<CScriptValue> args, results;
		args.push_back(CScriptValue((void*)pModel));
		args.push_back(CScriptValue((double)fBreakOffsetX));
		args.push_back(CScriptValue((double)fBreakOffsetY));
		args.push_back(CScriptValue((double)fBreakOffsetZ));

		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptPhysBreak], args, results);
	}
}

void CECModelStaticData::InitGlobalScript()
{
	static int _index = 1;

	if (!m_bInitGlobalScript)
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath);
#endif

		m_strGlobalScriptName.Format("GlobalScriptFunc_%d", _index++);
		abase::vector<AString> vargs;
		int var_count = ecm_get_var_count(enumECMScriptInit);

		for (int i = 0; i < var_count; i++)
			vargs.push_back(ecm_get_var_name(enumECMScriptInit, i));

		m_bInitGlobalScript = AfxGetECModelMan()->GetActScriptTbl().AddMethod(m_strGlobalScriptName, vargs, m_Scripts[enumECMScriptInit]);

		if (m_bInitGlobalScript)
		{
			abase::vector<CScriptValue> args, results;
			AfxGetECModelMan()->GetActScriptTbl().Call(m_strGlobalScriptName, args, results);
		}

#ifdef GFX_EDITOR
		m_bInitGlobalScript = false;
#endif
	}
}

bool CECModelStaticData::LoadAdditionalSkin(AFile* pFile, DWORD dwVersion, bool bLoadAdditionalSkin)
{
	DWORD dwReadLen;
	int nSkinCount = 0;

	if (!pFile->IsBinary())
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_addi_skin_count, &nSkinCount);

		for (int i = 0; i < nSkinCount; i++)
		{
			szBuf[0] = '\0';
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);

#ifdef GFX_EDITOR

			sscanf(szLine, _format_addi_skin_path, szBuf);

			if (strlen(szBuf) && bLoadAdditionalSkin)
				m_AdditionalSkinLst.push_back(szBuf);

#endif

		}
	}

	return true;
}

void CECModelStaticData::Release()
{
	size_t i;

	m_strPixelShader.Empty();
	m_strShaderTex.Empty();
	m_vecPSConsts.clear();

	for (i = 0; i < m_BoneScales.size(); i++)
		delete m_BoneScales[i];

	m_BoneScales.clear();

	for (i = 0; i < m_BoneScaleExArr.size(); i++)
		delete m_BoneScaleExArr[i];

	m_BoneScaleExArr.clear();

	for (CoGfxMap::iterator itGfx = m_CoGfxMap.begin(); itGfx != m_CoGfxMap.end(); ++itGfx)
		delete itGfx->second;

	m_CoGfxMap.clear();

	for (CombinedActMap::iterator it = m_ActionMap.begin(); it != m_ActionMap.end(); ++it)
		delete it->second;

	m_AdditionalSkinLst.clear();
	m_ActionMap.clear();

	for (i = 0; i < m_ChildInfoArray.size(); i++)
		delete m_ChildInfoArray[i];

	m_ChildInfoArray.clear();

#ifndef GFX_EDITOR

	for (i = 0; i < m_ConvexHullDataArr.size(); i++)
		delete m_ConvexHullDataArr[i];

	m_ConvexHullDataArr.clear();

#endif

	for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		delete m_ChannelInfoArr[i];
		m_ChannelInfoArr[i] = 0;
	}

	m_bChannelInfoInit = false;

	if (m_ScriptEnable[enumECMScriptRelease])
	{
		abase::vector<CScriptValue> args, results;
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptRelease], args, results);
	}

	for (i = 0; i < enumECMScriptCount; i++)
		m_Scripts[i].Empty();

	memset(m_ScriptEnable, 0, sizeof(m_ScriptEnable));
	m_ScriptMemTbl.Release();

	if (m_bInitGlobalScript)
	{
		AfxGetECModelMan()->GetActScriptTbl().RemoveMethod(m_strGlobalScriptName);
		m_bInitGlobalScript = false;
	}

#ifdef A3D_PHYSX

	if (m_pPhysSyncData)
	{
		delete m_pPhysSyncData;
		m_pPhysSyncData = 0;
	}

#endif

	{
		for (ECModelHookMap::iterator itECMHook = m_ECModelHookMap.begin(); itECMHook != m_ECModelHookMap.end(); ++itECMHook)
			delete itECMHook->second;
		
		m_ECModelHookMap.clear();
	}
}

bool CECModelStaticData::UpdateScript(int index, const char* strScript, bool bCompileOnly)
{
	if (!bCompileOnly)
		m_Scripts[index] = strScript;

	m_ScriptMemTbl.RemoveMethod(_script_func_name[index]);
	AddScriptMethod(index, strScript);
	return m_ScriptEnable[index];
}

/*
	global function for lua
*/
using namespace LuaBind;

void ECM_ShowChildModel(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING 
		|| args[2].m_Type != CScriptValue::SVT_BOOL )
	{
		LUA_DEBUG_INFO("ECM_ShowChildModel, wrong args\n");
		return;
	}
	CECModel* pModel;
	AString szHangerName;
	bool bShow;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(szHangerName);
	bShow = args[2].GetBool();
	pModel->ShowChildModel(szHangerName, bShow);
}
IMPLEMENT_SCRIPT_API(ECM_ShowChildModel);

void ECM_SetBoneTransFlag(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING 
		|| args[2].m_Type != CScriptValue::SVT_BOOL )
	{
		LUA_DEBUG_INFO("ECM_SetBoneTransFlag, wrong args\n");
		return;
	}

	CECModel* pModel;
	AString strBone;
	bool bFlag;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strBone);
	bFlag = args[2].GetBool();

	pModel->SetBoneTransFlag(strBone, bFlag);
}
IMPLEMENT_SCRIPT_API(ECM_SetBoneTransFlag);



void ECM_ChildChangeHook(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING 
		|| args[2].m_Type != CScriptValue::SVT_STRING )
	{
		LUA_DEBUG_INFO("ECM_ChildChangeHook, wrong args\n");
		return;
	}
	CECModel* pModel;
	AString strHanger;
	AString strNewHook;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHanger);
	args[2].RetrieveUtf8(strNewHook);
	pModel->ChildChangeHook(strHanger, strNewHook);
}
IMPLEMENT_SCRIPT_API(ECM_ChildChangeHook);

void ECM_SetUseAbsTrack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_BOOL)
	{
		LUA_DEBUG_INFO("ECM_SetUseAbsTrack, wrong args\n");
		return;
	}
	CECModel* pModel;
	bool bUse;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	bUse = args[1].GetBool();

	pModel->SetUseAbsTrack(bUse);
}
IMPLEMENT_SCRIPT_API(ECM_SetUseAbsTrack);

void ECM_RegisterAbsTrackOfBone(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_RegisterAbsTrackOfBone, wrong args\n");
		return;
	}
	CECModel* pModel;
	AString strBone;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strBone);
	pModel->RegisterAbsTrackOfBone(strBone);
}
IMPLEMENT_SCRIPT_API(ECM_RegisterAbsTrackOfBone);

void ECM_PlayGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() < 3 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_PlayGfx, wrong args\n");
		return;
	}

	CECModel* pModel;
	AString strHook;
	AString strGfx;
	float fScale;
	bool bUseECMHook;
	DWORD dwFadeOutTime;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);

	if (args.size() > 3 && args[3].m_Type == CScriptValue::SVT_NUMBER)
		fScale = (float)args[3].GetDouble();
	else
		fScale = 1.0f;

	if (args.size() > 4 && args[4].m_Type == CScriptValue::SVT_BOOL)
		bUseECMHook = args[4].GetBool();
	else
		bUseECMHook = false;

	//	说明：以前的时候虽然bFadeOut传true，但是由于GFX_INFO中(析构函数)不支持淡出
	//  所以实际上并不存在淡出功能，从当前版本开始(2011.2.24)，GFX_INFO支持淡出了，同时新增了脚本参数用于控制淡出
	//	为了保留以前的行为，我们让该参数在没有设置的情况下还是默认为0，从而保持没有淡出的行为
	if (args.size() > 5 && args[5].m_Type == CScriptValue::SVT_NUMBER)
		dwFadeOutTime = args[5].GetInt();
	else
		dwFadeOutTime = 0;

	pModel->PlayGfx(strGfx, strHook, fScale, true, bUseECMHook, dwFadeOutTime);
}
IMPLEMENT_SCRIPT_API(ECM_PlayGfx);

void ECM_PlayGfxEx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() < 9 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING
		|| args[3].m_Type != CScriptValue::SVT_NUMBER
		|| args[4].m_Type != CScriptValue::SVT_NUMBER
		|| args[5].m_Type != CScriptValue::SVT_NUMBER
		|| args[6].m_Type != CScriptValue::SVT_NUMBER
		|| args[7].m_Type != CScriptValue::SVT_NUMBER
		|| args[8].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_PlayGfxEx, wrong args\n");
		return;
	}
	CECModel* pModel;
	AString strHook;
	AString strGfx;
	double fOffsetx;
	double fOffsety;
	double fOffsetz;
	double fPitch;
	double fYaw;
	double fRot;
	float fScale;
	bool bUseECMHook;
	DWORD dwFadeOutTime;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);
	fOffsetx = args[3].GetDouble();
	fOffsety = args[4].GetDouble();
	fOffsetz = args[5].GetDouble();
	fPitch	 = args[6].GetDouble();
	fYaw	 = args[7].GetDouble();
	fRot	 = args[8].GetDouble();

	if (args.size() > 9 && args[9].m_Type == CScriptValue::SVT_NUMBER)
		fScale = (float)args[9].GetDouble();
	else
		fScale = 1.0f;

	if (args.size() > 10 && args[10].m_Type == CScriptValue::SVT_BOOL)
		bUseECMHook = args[10].GetBool();
	else
		bUseECMHook = false;

	//	参考ECM_PlayGfx处本参数的注释
	if (args.size() > 11 && args[11].m_Type == CScriptValue::SVT_NUMBER)
		dwFadeOutTime = args[11].GetInt();
	else
		dwFadeOutTime = 0;

	pModel->PlayGfx(strGfx
		, strHook
		, fScale
		, true
		, A3DVECTOR3(static_cast<float>(fOffsetx), static_cast<float>(fOffsety), static_cast<float>(fOffsetz))
		, static_cast<float>(fPitch)
		, static_cast<float>(fYaw)
		, static_cast<float>(fRot)
		, bUseECMHook
		, dwFadeOutTime);
}
IMPLEMENT_SCRIPT_API(ECM_PlayGfxEx);

int ECM_RemoveGfx(lua_State * L)
{
	CECModel* pModel;
	AString strHook;
	AString strGfx;

	if (!CheckValue(TypeWrapper<LuaUserData>(), L, 1, (void*&)pModel))
		return 0;

	if (!CheckValue(TypeWrapper<const char*>(), L, 2, strHook))
		return 0;

	if (!CheckValue(TypeWrapper<const char*>(), L, 3, strGfx))
		return 0;

	pModel->RemoveGfx(strGfx, strHook);
	return 0;
}

void ECM_RemoveGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_RemoveGfx, wrong args\n");
		return;
	}
	CECModel* pModel;
	AString strHook;
	AString strGfx;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);

	pModel->RemoveGfx(strGfx, strHook);
}
IMPLEMENT_SCRIPT_API(ECM_RemoveGfx);

void logScriptValue(const CScriptValue& sv)
{
	switch(sv.m_Type)
	{
	case CScriptValue::SVT_BOOL:
		a_LogOutput(1, "Bool: %s", sv.GetBool() ? "True" : "False");
		break;
	case CScriptValue::SVT_NUMBER:
		a_LogOutput(1, "Double: %f", sv.GetDouble());
		break;
	case CScriptValue::SVT_STRING: 
		a_LogOutput(1, "String: %s", (LPCSTR)sv.m_String.GetUtf8());
		break;
	case CScriptValue::SVT_USERDATA:
		a_LogOutput(1, "UserData: %x", sv.GetUserData());
		break;
	default:
		a_LogOutput(1, "Unknown type of scriptvalue");
		break;
	}
}

// this functions is used as an example
void ECM_ArgPassIn(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA 
		|| args[1].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ArgPassIn, wrong args\nArg 1st: Pointer to ECModel\nArg 2nd: A table");
		return;
	}
	CECModel* pModel;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	const abase::vector<CScriptValue>& keys = args[1].m_ArrKey;
	const abase::vector<CScriptValue>& array = args[1].m_ArrVal;

	bool bPrintKeys = false;
	if (keys.size() == array.size())
		bPrintKeys = true;

	for (size_t nIdx = 0; nIdx < array.size(); ++nIdx)
	{
		if (bPrintKeys) {
			a_LogOutput(1, "Key:");
			logScriptValue(keys[nIdx]);
		}
		a_LogOutput(1, "Value:");
		logScriptValue(array[nIdx]);
	}
}
IMPLEMENT_SCRIPT_API(ECM_ArgPassIn);

// this functions is used as an example
void ECM_ArgPassOut(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 1 || args[0].m_Type != CScriptValue::SVT_USERDATA)
	{
		LUA_DEBUG_INFO("ECM_ArgPassOut, wrong args\nArg 1st: Pointer to ECModel");
		return;
	}

	CECModel* pModel;
	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);

	luaTableWrapper luatab;
	luatab.append("ModFilePath", pModel->GetFilePath());
	luatab.append("BoneScaleCount", pModel->GetBoneScaleExArr().size());

	results.clear();
	results.push_back(luatab);
}
IMPLEMENT_SCRIPT_API(ECM_ArgPassOut);

void ECM_ActPlayGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_STRING
		|| args[1].m_Type != CScriptValue::SVT_USERDATA
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ActPlayGfx, wrong args\nArg 1st: user defined unique name\nArg 2nd: act channel\nArg 3rd: pointer to ECModel\nArg 4th: table of params");
		return;
	}

	const char* szName = args[0].m_String.GetUtf8();
	if (!szName || !strlen(szName))
		return;

	CECModel* pModel;
	pModel = (CECModel *)args[1].GetUserData();
	assert(pModel);

	int nChannel = args[2].GetInt();
	
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	if (!pAct)
		return;

	luaTableWrapper luatab(args[3]);
	GFX_PLAY_INFO gpi(luatab, pAct->GetComAct(), AfxGetA3DDevice());
	gpi.Start(szName, pAct);
}
IMPLEMENT_SCRIPT_API(ECM_ActPlayGfx);

void ECM_ActPlaySfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_STRING
		|| args[1].m_Type != CScriptValue::SVT_USERDATA
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ActPlaySfx, wrong args\nArg 1st: user defined unique name\nArg 2nd: act channel\nArg 3rd: pointer to ECModel\nArg 4th: table of params");
		return;
	}

	const char* szName = args[0].m_String.GetUtf8();
	if (!szName || !strlen(szName))
		return;

	CECModel* pModel;
	pModel = (CECModel *)args[1].GetUserData();
	assert(pModel);

	int nChannel = args[2].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	if (!pAct)
		return;

	luaTableWrapper luatab(args[3]);
	SFX_PLAY_INFO spi(luatab, pAct->GetComAct(), AfxGetA3DDevice());
	spi.Start(szName, pAct);
}
IMPLEMENT_SCRIPT_API(ECM_ActPlaySfx);

void ECM_ActPlayChildAct(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_STRING
		|| args[1].m_Type != CScriptValue::SVT_USERDATA
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ActPlayChildAct, wrong args\nArg 1st: user defined unique name\nArg 2nd: act channel\nArg 3rd: pointer to ECModel\nArg 4th: table of params");
		return;
	}

	const char* szName = args[0].m_String.GetUtf8();
	if (!szName || !strlen(szName))
		return;

	CECModel* pModel;
	pModel = (CECModel *)args[1].GetUserData();
	assert(pModel);

	int nChannel = args[2].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	if (!pAct)
		return;

	luaTableWrapper luatab(args[3]);
	CHILDACT_PLAY_INFO capi(luatab, pAct->GetComAct(), AfxGetA3DDevice());
	capi.Start(szName, pAct);
}
IMPLEMENT_SCRIPT_API(ECM_ActPlayChildAct);

void ECM_SetAlpha(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA  || args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_SetAlpha, wrong args\n");
		return;
	}

	CECModel* pModel;
	float fAlpha;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	fAlpha = (float)args[1].GetDouble();
	pModel->SetTransparent(1.0f - fAlpha);
}
IMPLEMENT_SCRIPT_API(ECM_SetAlpha);

void ECM_SetColor(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_USERDATA || args[1].m_Type != CScriptValue::SVT_NUMBER
	 || args[2].m_Type != CScriptValue::SVT_NUMBER || args[3].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_SetColor, wrong args\n");
		return;
	}

	CECModel* pModel;
	A3DCOLORVALUE cl;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	cl.r = (float)args[1].GetDouble();
	cl.g = (float)args[2].GetDouble();
	cl.b = (float)args[3].GetDouble();
	cl.a = 1.0f;
	pModel->SetColor(cl);
}
IMPLEMENT_SCRIPT_API(ECM_SetColor);

void ECM_HasEquip(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA  || args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_HasEquip, wrong args\n");
		return;
	}

	CECModel* pModel;
	int id;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	id = args[1].GetInt();
	results.push_back(CScriptValue(pModel->HasEquip(id)));
}
IMPLEMENT_SCRIPT_API(ECM_HasEquip);

void ECM_GetEquipId(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA  || args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_GetEquipId, wrong args\n");
		return;
	}

	CECModel* pModel;
	int index;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	index = args[1].GetInt();
	results.push_back(CScriptValue((double)pModel->GetEquipId(index)));
}
IMPLEMENT_SCRIPT_API(ECM_GetEquipId);

void ECM_HasGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() < 3 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_HasGfx, wrong args\n");
		return;
	}

	CECModel* pModel;
	AString strHook;
	AString strGfx;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);
	results.push_back(CScriptValue(pModel->GetGfx(strGfx, strHook) != NULL));
}
IMPLEMENT_SCRIPT_API(ECM_HasGfx);

void ECM_ShowGfx(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() < 4 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING
		|| args[2].m_Type != CScriptValue::SVT_STRING
		|| args[3].m_Type != CScriptValue::SVT_BOOL)
	{
		LUA_DEBUG_INFO("ECM_ShowGfx, wrong args\n");
		return;
	}

	CECModel* pModel;
	AString strHook;
	AString strGfx;
	bool bShow;

	pModel = (CECModel *)args[0].GetUserData();
	assert(pModel);
	args[1].RetrieveUtf8(strHook);
	args[2].RetrieveUtf8(strGfx);
	bShow = args[3].GetBool();

	A3DGFXEx* pGfx = pModel->GetGfx(strGfx, strHook);

	if (pGfx)
		pGfx->SetVisible(bShow);
}
IMPLEMENT_SCRIPT_API(ECM_ShowGfx);

void ECM_GetChannelAct(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA  || args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_GetChannelAct, wrong args\n");
		return;
	}

	CECModel* pModel;
	int nChannel;

	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	nChannel = args[1].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	CScriptString str;

	if (pAct)
		str.SetUtf8(pAct->GetComAct()->GetName(), strlen(pAct->GetComAct()->GetName()));

	results.push_back(str);
}
IMPLEMENT_SCRIPT_API(ECM_GetChannelAct);

void ECM_GetFashionMode(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 1 || args[0].m_Type != CScriptValue::SVT_USERDATA)
	{
		LUA_DEBUG_INFO("ECM_GetFashionMode, wrong args\n");
		return;
	}

	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	results.push_back(CScriptValue(pModel->GetFashionMode()));
}
IMPLEMENT_SCRIPT_API(ECM_GetFashionMode);

void ECM_ReplaceSkinFile(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_ReplaceSkinFile, wrong args\n");
		return;
	}

	AString strSkinFile;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	int iIndex = args[1].GetInt();
	if (iIndex < 0 || iIndex >= pModel->GetSkinNum())
	{
		AString strMsg;
		strMsg.Format("ECM_GetSkinFile, passed in index(%d) error, there are only %d skin exist.", iIndex, pModel->GetSkinNum());
		LUA_DEBUG_INFO(strMsg);
		return;
	}

	args[2].RetrieveUtf8(strSkinFile);

	bool bRet = pModel->ReplaceSkinFile(iIndex, strSkinFile);

	results.push_back(CScriptValue(bRet));
}
IMPLEMENT_SCRIPT_API(ECM_ReplaceSkinFile)

void ECM_GetSkinFile(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 
		|| args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_GetSkinFile, wrong args\n");
		return;
	}

	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	int iIndex = args[1].GetInt();
	if (iIndex < 0 || iIndex >= pModel->GetSkinNum())
	{
		AString strMsg;
		strMsg.Format("ECM_GetSkinFile, passed in index(%d) error, there are only %d skin exist.", iIndex, pModel->GetSkinNum());
		LUA_DEBUG_INFO(strMsg);
		return;
	}

	CScriptString str;
	A3DSkin* pSkin = pModel->GetA3DSkin(iIndex);
	if (pSkin != NULL)
		str.SetAString(pSkin->GetFileName());

	results.push_back(CScriptValue(str));
}
IMPLEMENT_SCRIPT_API(ECM_GetSkinFile)

void ECM_AddChildModelLink(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING		// hangerName
		|| args[2].m_Type != CScriptValue::SVT_STRING		// parentHookName
		|| args[3].m_Type != CScriptValue::SVT_STRING		// childAttacherName
		)
	{
		LUA_DEBUG_INFO("ECM_AddChildModelLink, wrong args\n");
		return;
	}

	AString strHanger, strParentHook, strChildAttacher;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	
	args[1].RetrieveUtf8(strHanger);
	args[2].RetrieveUtf8(strParentHook);
	args[3].RetrieveUtf8(strChildAttacher);

	bool bRet = pModel->AddChildModelLink(strHanger, strParentHook, strChildAttacher);
	results.push_back(CScriptValue(bRet));
}
IMPLEMENT_SCRIPT_API(ECM_AddChildModelLink)

void ECM_RemoveChildModelLink(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING		// hangerName
		|| args[2].m_Type != CScriptValue::SVT_STRING		// parentHookName
		|| args[3].m_Type != CScriptValue::SVT_STRING		// childAttacherName
		)
	{
		LUA_DEBUG_INFO("ECM_RemoveChildModelLink, wrong args\n");
		return;
	}

	AString strHanger, strParentHook, strChildAttacher;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	assert(pModel);
	
	args[1].RetrieveUtf8(strHanger);
	args[2].RetrieveUtf8(strParentHook);
	args[3].RetrieveUtf8(strChildAttacher);

	bool bRet = pModel->RemoveChildModelLink(strHanger, strParentHook, strChildAttacher);
	results.push_back(CScriptValue(bRet));
}
IMPLEMENT_SCRIPT_API(ECM_RemoveChildModelLink)

void ECM_ResetChildModelInitPos(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING		// hangerName
		)
	{
		LUA_DEBUG_INFO("ECM_ResetChildModelInitPos, wrong args\n");
		return;
	}

	AString strHanger;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	if (!pModel)
	{
		assert(pModel);
		return;
	}
	
	args[1].RetrieveUtf8(strHanger);
	CECModel* pChildModel = pModel->GetChildModel(strHanger);
	if (!pChildModel)
		return;

	pChildModel->GetA3DSkinModel()->ResetToInitPose();
}
IMPLEMENT_SCRIPT_API(ECM_ResetChildModelInitPos)

void ECM_SetChildModelPhysState(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
#ifdef A3D_PHYSX

	if (args.size() != 3 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING		// hangerName
		|| args[2].m_Type != CScriptValue::SVT_BOOL			// true for phys state, false for animation state
		)
	{
		LUA_DEBUG_INFO("ECM_SetChildModelPhysState, wrong args\n");
		return;
	}

	AString strHanger;
	CECModel* pModel;
	pModel = (CECModel*)args[0].GetUserData();
	if (!pModel)
	{
		assert(pModel);
		return;
	}
	
	args[1].RetrieveUtf8(strHanger);
	bool bIsPhys = args[2].GetBool();

	pModel->ChangeChildModelPhysState(strHanger, bIsPhys ? A3DModelPhysSync::PHY_STATE_SIMULATE :  A3DModelPhysSync::PHY_STATE_ANIMATION);

#endif
}
IMPLEMENT_SCRIPT_API(ECM_SetChildModelPhysState)

void ECM_AddForce(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
#ifdef A3D_PHYSX

	if (args.size() < 6 || args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_NUMBER
		|| args[4].m_Type != CScriptValue::SVT_NUMBER
		|| args[5].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_AddForce, should take at least 6 args (model, x, y, z, force, type_name)");
		return;
	}

	A3DVECTOR3 vTarget;

	if (args.size() == 9)
	{
		if (args[6].m_Type != CScriptValue::SVT_NUMBER
			|| args[7].m_Type != CScriptValue::SVT_NUMBER
			|| args[8].m_Type != CScriptValue::SVT_NUMBER)
		{
			LUA_DEBUG_INFO("ECM_AddForce, target param wrong");
			return;
		}

		vTarget.Set((float)args[6].GetDouble(), (float)args[7].GetDouble(), (float)args[8].GetDouble());
	}
	else
		vTarget.Clear();

	CECModel* pModel = (CECModel*)args[0].GetUserData();
	A3DVECTOR3 vDir((float)args[1].GetDouble(), (float)args[2].GetDouble(), (float)args[3].GetDouble());
	float fForceMagnitude = (float)args[4].GetDouble();
	a_ClampFloor(fForceMagnitude, 0.0f);

	AString strForceType;
	args[5].RetrieveUtf8(strForceType);
	
	int iPhysForceType = CECModel::PFT_FORCE;
	if (strForceType.CompareNoCase("force") == 0)
		iPhysForceType = CECModel::PFT_FORCE;
	else if (strForceType.CompareNoCase("impulse") == 0)
		iPhysForceType = CECModel::PFT_IMPULSE;
	else if (strForceType.CompareNoCase("velocity") == 0)
		iPhysForceType = CECModel::PFT_VELOCITY_CHANGE;

	A3DVECTOR3 vStart = pModel->GetPos() + vTarget - vDir;
	bool bRet = pModel->AddForce(vStart, vDir, fForceMagnitude, FLT_MAX, iPhysForceType);

	results.push_back(CScriptValue(bRet));

#endif
}
IMPLEMENT_SCRIPT_API(ECM_AddForce)

void ECM_ActChangeModelScale(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 4
		|| args[0].m_Type != CScriptValue::SVT_STRING
		|| args[1].m_Type != CScriptValue::SVT_USERDATA
		|| args[2].m_Type != CScriptValue::SVT_NUMBER
		|| args[3].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_ActChangeChildModelScale, wrong args.\n");
		return;
	}

	const char* szName = args[0].m_String.GetUtf8();
	if (!szName || !szName[0])
		return;

	CECModel* pModel;
	pModel = (CECModel *)args[1].GetUserData();
	assert(pModel);

	int nChannel = args[2].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);
	if (!pAct)
		return;

	BONE_SCALE_CHANGE_PLAY_INFO bone_scl_play(args[3], pAct->GetComAct(), AfxGetA3DDevice());
	bone_scl_play.Start(szName, pAct);
}
IMPLEMENT_SCRIPT_API(ECM_ActChangeModelScale)

void ECM_MotionBlur(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2 || args[0].m_Type != CScriptValue::SVT_USERDATA || args[1].m_Type != CScriptValue::SVT_ARRAY)
	{
		LUA_DEBUG_INFO("ECM_MotionBlur, wrong args\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	luaTableWrapper wrapper(args[1]);
	const CScriptValue* pColors				= wrapper.raw_getitem("Colors");
	const CScriptValue* pBones				= wrapper.raw_getitem("Bones");
	const CScriptValue* pTotalTime			= wrapper.raw_getitem("TotalTime");
	const CScriptValue* pInterval			= wrapper.raw_getitem("Interval");
	const CScriptValue* pApplyToChild		= wrapper.raw_getitem("ApplyToChild");
	const CScriptValue* pHighlight			= wrapper.raw_getitem("Highlight");

	std::auto_ptr<ECMActionBlurInfo> pBlurInfo(new ECMActionBlurInfo);

	if (pColors && pColors->m_ArrVal.size())
	{
		for (size_t i = 0; i < pColors->m_ArrVal.size(); i++)
		{
			const CScriptValue& c = pColors->m_ArrVal[i];

			if (c.m_ArrVal.size() == 4)
			{
				int val = (c.m_ArrVal[0].GetInt() << 24) | (c.m_ArrVal[1].GetInt() << 16) | (c.m_ArrVal[2].GetInt() << 8) |(c.m_ArrVal[3].GetInt());
				pBlurInfo->m_Colors.push_back(val);
			}
		}

		if (pBlurInfo->m_Colors.size() == 0)
			return;
	}
	else
		return;

	if (pBones)
	{
		for (size_t i = 0; i < pBones->m_ArrVal.size(); i++)
			pBlurInfo->m_BoneIndices.push_back(pBones->m_ArrVal[i].GetInt());
	}

	if (pTotalTime)
		pBlurInfo->m_dwTotalTime = pTotalTime->GetInt();
	else
		pBlurInfo->m_dwTotalTime = 0;

	if (pInterval)
		pBlurInfo->m_dwInterval = pInterval->GetInt();
	else
		pBlurInfo->m_dwInterval = 33;

	if (pApplyToChild)
		pBlurInfo->m_bApplyToChildren = pApplyToChild->GetBool();
	else
		pBlurInfo->m_bApplyToChildren = true;

	if (pHighlight)
		pBlurInfo->m_bHighlight = pHighlight->GetBool();
	else
		pBlurInfo->m_bHighlight = false;

	pBlurInfo->m_bRoot = true;
	pModel->SetMotionBlurInfo(pBlurInfo.release());
}

IMPLEMENT_SCRIPT_API(ECM_MotionBlur)

void ECM_ApplyPixelShader(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 3
		|| args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER
		|| args[2].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_ApplyPixelShader, wrong args.\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	int nChannel = args[1].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);

	if (!pAct)
		return;

	const char* szPSPath = args[2].m_String.GetUtf8();
	AFileImage file;

#ifdef _ANGELICA22
	if (!file.Open(szPSPath, AFILE_OPENEXIST | AFILE_TEXT | AFILE_TEMPMEMORY))
#else
	if (!file.Open(szPSPath, AFILE_OPENEXIST | AFILE_TEXT))
#endif
		return;

	AString strPS, strTex;
	GfxPSConstVec vec;
	g_GfxLoadPixelShaderConsts(&file, strPS, strTex, vec);
	file.Close();
	pModel->SetReplaceShader(strPS, strTex, vec);
	pAct->SetResetPSWhenActStop(true);
}

IMPLEMENT_SCRIPT_API(ECM_ApplyPixelShader)

void ECM_RemovePixelShader(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (args.size() != 2
		|| args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_NUMBER)
	{
		LUA_DEBUG_INFO("ECM_RemovePixelShader, wrong args.\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	int nChannel = args[1].GetInt();
	A3DCombActDynData* pAct = pModel->GetCurAction(nChannel);

	if (!pAct)
		return;

	pModel->RemoveReplaceShader();
	pAct->SetResetPSWhenActStop(false);
}

IMPLEMENT_SCRIPT_API(ECM_RemovePixelShader)

void ECM_SetActionNearestInterp(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
#ifdef _ANGELICA21

	if (args.size() != 2
		|| args[0].m_Type != CScriptValue::SVT_USERDATA
		|| args[1].m_Type != CScriptValue::SVT_STRING)
	{
		LUA_DEBUG_INFO("ECM_SetActionNearestInterp, wrong args.\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	A3DSkinModel* pSkinModel = pModel->GetA3DSkinModel();

	if (!pSkinModel)
		return;

	const char* szAction = args[1].m_String.GetUtf8();
	A3DSkinModelActionCore* pAction = pSkinModel->GetAction(szAction);

	if (pAction)
		pAction->SetNearestInterp(true);

#endif
}

IMPLEMENT_SCRIPT_API(ECM_SetActionNearestInterp)

void ECM_SetAllActionNearestInterp(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
#ifdef _ANGELICA21

	if (args.size() != 1
		|| args[0].m_Type != CScriptValue::SVT_USERDATA)
	{
		LUA_DEBUG_INFO("ECM_SetActionNearestInterp, wrong args.\n");
		return;
	}

	CECModel* pModel = (CECModel*)args[0].GetUserData();

	if (!pModel)
	{
		assert(false);
		return;
	}

	A3DSkinModel* pSkinModel = pModel->GetA3DSkinModel();

	if (!pSkinModel)
		return;

	pSkinModel->SetAllActionNearestInterp();

#endif
}

IMPLEMENT_SCRIPT_API(ECM_SetAllActionNearestInterp)

void InitECMApi(CLuaState * pState)
{
	REGISTER_SCRIPT_API(ECMApi, ECM_ShowChildModel);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetBoneTransFlag);
	REGISTER_SCRIPT_API(ECMApi, ECM_ChildChangeHook);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetUseAbsTrack);
	REGISTER_SCRIPT_API(ECMApi, ECM_RegisterAbsTrackOfBone);
	REGISTER_SCRIPT_API(ECMApi, ECM_PlayGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_RemoveGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_PlayGfxEx);
	REGISTER_SCRIPT_API(ECMApi, ECM_ActPlayGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_ActPlaySfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_ActPlayChildAct);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetAlpha);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetColor);
	REGISTER_SCRIPT_API(ECMApi, ECM_HasEquip);
	REGISTER_SCRIPT_API(ECMApi, ECM_GetEquipId);
	REGISTER_SCRIPT_API(ECMApi, ECM_HasGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_ShowGfx);
	REGISTER_SCRIPT_API(ECMApi, ECM_GetChannelAct);
	REGISTER_SCRIPT_API(ECMApi, ECM_GetFashionMode);
	REGISTER_SCRIPT_API(ECMApi, ECM_ReplaceSkinFile);
	REGISTER_SCRIPT_API(ECMApi, ECM_GetSkinFile);
	REGISTER_SCRIPT_API(ECMApi, ECM_AddChildModelLink);
	REGISTER_SCRIPT_API(ECMApi, ECM_RemoveChildModelLink);
	REGISTER_SCRIPT_API(ECMApi, ECM_ResetChildModelInitPos);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetChildModelPhysState);
	REGISTER_SCRIPT_API(ECMApi, ECM_ActChangeModelScale);
	REGISTER_SCRIPT_API(ECMApi, ECM_MotionBlur);
	REGISTER_SCRIPT_API(ECMApi, ECM_AddForce);
	REGISTER_SCRIPT_API(ECMApi, ECM_ApplyPixelShader);
	REGISTER_SCRIPT_API(ECMApi, ECM_RemovePixelShader);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetActionNearestInterp);
	REGISTER_SCRIPT_API(ECMApi, ECM_SetAllActionNearestInterp);

	pState->RegisterLibApi("ECMApi");
}

#ifdef GFX_EDITOR

CECModelMan _ecmodel_man;

CECModelMan* AfxGetECModelMan()
{
	return &_ecmodel_man;
}

#endif
