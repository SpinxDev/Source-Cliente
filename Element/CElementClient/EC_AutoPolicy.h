/*
 * FILE: EC_AutoPolicy.h
 *
 * DESCRIPTION:
 *
 * CREATED BY: Shizhenhua, 2013/8/21
 *
 * HISTORY:
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */

#pragma once

#include <ABaseDef.h>
#include <vector.h>
#include <AAssist.h>
#include "EC_Counter.h"
#include "ScriptValue.h"
#include "aarray.h"

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

class CECPlayerWrapper;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//	Class CECAutoPolicy
//
///////////////////////////////////////////////////////////////////////////

class CECAutoPolicy
{
public:
	// 策略类型
	enum
	{
		POLICY_AUTOKILLMONSTER, // 自动杀怪
		POLICY_COLLECTMATTER,
	};

	// 系统事件
	enum
	{
		EVENT_BEHURT = 1,
		EVENT_SKILLINTERRUPT,
		EVENT_RETURNTOWNOK,
		EVENT_CONFIGCHANGED,
	};

	struct CONFIG
	{
		int attack_skill;
		bool attack_iscombo;
		int assist_skill1;
		int assist_skill2;
		int assist_skill3;
		int assist_skill4;
		bool assist_iscombo1;
		bool assist_iscombo2;
		bool assist_iscombo3;
		bool assist_iscombo4;
		int nAssistInterval1;
		int nAssistInterval2;
		int nAssistInterval3;
		int nAssistInterval4;
		int nTime;
		int iAutoPickMode;		   // 无、所有物品、仅拾取金币
		int nPetrolRadius;		   // 巡逻半径
		AString ignoreMonsterName; // Alien - adicionado
		AString ignoreMatterName;	// Alien - adicionado
		int nAttackOrIgnore;	   // Alien - adicionado
		int nMonsterOrMatter;	   // Alien - adicionado

		CONFIG() : attack_skill(0), attack_iscombo(false), assist_skill1(0), assist_skill2(0), assist_skill3(0), assist_skill4(0), assist_iscombo1(false), assist_iscombo2(false), assist_iscombo3(false), assist_iscombo4(false),
				   nAssistInterval1(60000), nAssistInterval2(60000), nAssistInterval3(60000), nAssistInterval4(60000), nTime(3600000), iAutoPickMode(0), nPetrolRadius(500), ignoreMonsterName("") /*Alien - adicionado*/, ignoreMatterName("") /*Alien - adicionado*/, nAttackOrIgnore(1) /*Alien - adicionado*/, nMonsterOrMatter(1)
		{
		}
	};

	AArray<int> listMatters;

public:
	virtual ~CECAutoPolicy();

	// 初始化玩家自动策略
	bool Init();
	void Release();

	// 更新策略
	void Tick(DWORD dwDeltaTime);
	void Render();

	// 开始/结束自动策略
	void StartPolicy(int policyType);
	void StopPolicy();

	// 进入游戏世界
	void OnEnterWorld();

	// 离开游戏世界
	void OnLeaveWorld();

	// 系统事件
	void SendEvent_BeHurt(int attacker);
	void SendEvent_SkillInterrupt(int skill_id);
	void SendEvent_ReturnTown();
	void SendEvent_ConfigChanged();

	// 获取玩家对象
	CECPlayerWrapper *GetPlayerWrapper() { return m_pPlayer; }

	// 检测当前是否开启策略
	bool IsAutoPolicyEnabled() const { return !m_strCurPolicy.IsEmpty(); }

	// 获取当前策略名字
	const char *GetCurPolicy() const { return m_strCurPolicy; }

	// 获取配置数据
	const CONFIG &GetConfigData() const { return m_Config; }

	// 设置配置数据
	void SetConfigData(const CONFIG &data);

	// 获取剩余时间
	DWORD GetRemainTime() const;

	// 获取实例
	static CECAutoPolicy &GetInstance();

protected:
	CONFIG m_Config;
	CECPlayerWrapper *m_pPlayer;
	AString m_strCurPolicy;
	CECCounter m_cntTick;
	DWORD m_dwKeepingTime;
	DWORD m_dwCurrentTime;

private:
	CECAutoPolicy();

	// 加载指定脚本文件
	bool LoadLuaFile(const char *filename);
	// 执行指定的脚本函数
	void CallLuaFunc(const char *szTable, const char *szName, const abase::vector<CScriptValue> &args, abase::vector<CScriptValue> *ret = NULL);
	// 设置当前策略
	void SetCurPolicy(const char *szPolicyName);

	// 加载配置
	bool LoadConfigData();
	// 保存配置
	void SaveConfigData();
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////
