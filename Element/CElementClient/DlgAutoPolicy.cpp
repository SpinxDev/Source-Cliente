// Filename	: DlgAutoPolicy.cpp
// Creator	: Shizhenhua
// Date		: 2013/8/30

#include "DlgAutoPolicy.h"
//#include "DlgSkill.h"
#include "DlgSkillSubOther.h"
#include "DlgAutoHPMP.h"
#include "EC_AutoPolicy.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_ShortcutSet.h"
#include "EC_Configs.h"
#include "EC_HostPlayer.h"
#include "EC_Skill.h"
#include "EC_ComputerAid.h"
#include "EC_Shortcut.h"
#include "EC_UIConfigs.h"
#include "EC_PlayerWrapper.h"

#include <AFI.h>
#include <AUICTranslate.h>

/*Alien - foi adicionado - Auto Cultivo Aprimorado*/
#include "auidialog.h"
#include "CSplit.h"
#include "itemdataman.h"
#include "DlgBase.h"
#include "elementdataman.h"
#include "EC_ManNPC.h"
#include "EC_World.h"
#include "EC_NPC.h"
#include "EC_ManMatter.h"
#include "EC_Matter.h"
#include "EC_UIHelper.h"
#include "EC_HPWorkTrace.h"
#include "Alien/DlgAutoPolicyMatter.h"
#include "EC_UIManager.h"
/*Ate Aqui*/

#define new A_DEBUG_NEW

static const int MOEDA = 3044;
static const int MIN_RANGE = 20;
static const int LIST_TYPE = 2;
static const int LIST_GROUP = 1; /*Alien - foi adicionado*/

AUI_BEGIN_COMMAND_MAP(CDlgAutoPolicy, CDlgBase)
AUI_ON_COMMAND("Btn_AutoOpen", OnCommand_StartOrStop)
AUI_ON_COMMAND("Btn_Confirm", OnCommand_Confirm)
AUI_ON_COMMAND("Btn_Cancel", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_AutoHPMP", OnCommand_SetAutoHPMP)
AUI_ON_COMMAND("Btn_Add", OnCommand_Add)		/*Alien - foi adicionado*/
AUI_ON_COMMAND("Btn_Delete", OnCommand_Delete)	/*Alien - foi adicionado*/
AUI_ON_COMMAND("Rdo_TypeMonster", OnCommand_Switch)
AUI_ON_COMMAND("RadioButton_122710383", OnCommand_Switch)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoPolicy, CDlgBase)
AUI_ON_EVENT("Img_Skill*", WM_LBUTTONDOWN, OnEvent_LButtonDown)
AUI_END_EVENT_MAP()

CDlgAutoPolicy::CDlgAutoPolicy()
{
	cfgData = CECAutoPolicy::GetInstance().GetConfigData();	/*Alien - foi adicionado*/
	m_pListMonster = NULL;		/*Alien - foi adicionado*/
	m_pListArea = NULL;			/*Alien - foi adicionado*/
	m_pTxt_MonsterName = NULL;	/*Alien - foi adicionado*/
	list_monsters.RemoveAll();	/*Alien - foi adicionado*/
	list_matters.RemoveAll();	/*Alien - foi adicionado*/
	m_pImg_Skill1 = NULL;
	m_pImg_Skill2 = NULL;
	m_pImg_Skill3 = NULL;
	m_pImg_Skill4 = NULL;
	m_pImg_Skill5 = NULL;
	m_pTxt_Interval_Skill2 = NULL;
	m_pTxt_Interval_Skill3 = NULL;
	m_pTxt_Interval_Skill4 = NULL;
	m_pTxt_Interval_Skill5 = NULL;
	m_pTxt_Time = NULL;
	m_pTxt_Range = NULL;
	m_pCmb_AutoPick = NULL;
	m_pBtn_Switch = NULL;
	m_pLbl_RemainTime = NULL;
	bCollect = false;
}

bool CDlgAutoPolicy::OnInitDialog()
{
	DDX_Control("Img_Skill01", m_pImg_Skill1);
	DDX_Control("Img_Skill02", m_pImg_Skill2);
	DDX_Control("Img_Skill03", m_pImg_Skill3);
	DDX_Control("Img_Skill04", m_pImg_Skill4);
	DDX_Control("Img_Skill05", m_pImg_Skill5);
	DDX_Control("Skill02_Time", m_pTxt_Interval_Skill2);
	DDX_Control("Skill03_Time", m_pTxt_Interval_Skill3);
	DDX_Control("Skill04_Time", m_pTxt_Interval_Skill4);
	DDX_Control("Skill05_Time", m_pTxt_Interval_Skill5);
	DDX_Control("Edt_Time2", m_pTxt_Time);
	DDX_Control("Edt_Range", m_pTxt_Range);
	DDX_Control("Combo_Filter", m_pCmb_AutoPick);
	DDX_Control("Btn_AutoOpen", m_pBtn_Switch);
	DDX_Control("Txt_Time", m_pLbl_RemainTime);
	DDX_Control("DEFAULT_Monster", m_pTxt_MonsterName);	/*Alien - foi adicionado*/
	DDX_Control("List_MonsterName", m_pListMonster);	/*Alien - foi adicionado*/
	DDX_Control("Lst_MonsterArea", m_pListArea);		/*Alien - foi adicionado*/

	for( int i=0;i<3;i++ )
	{
		m_pCmb_AutoPick->AddString(GetStringFromTable(10928 + i));
	}

	m_pTxt_MonsterName->SetFocus(false);

	return true;
}

void CDlgAutoPolicy::UpdateList()
{
	m_pListMonster->ResetContent();
	m_pListArea->ResetContent();
	bCollect = false;
	if (GetCheckedRadioButton(2) == 1)
	{			
		for (int i = 0; i < list_monsters.GetSize(); i++)
		{
			elementdataman *pData = GetGame()->GetElementDataMan();
			DATA_TYPE dt;			
			MONSTER_ESSENCE *pEssence = (MONSTER_ESSENCE *)pData->get_data_ptr(list_monsters[i], ID_SPACE_ESSENCE, dt);
			if (pEssence && dt == DT_MONSTER_ESSENCE)
			{				
				ACString name = pEssence->name;
				m_pListMonster->AddString(name);
			}
		}

		CECNPCMan *pNPCMan = GetWorld()->GetNPCMan();
		const APtrArray<CECNPC *> &NPCs = pNPCMan->GetNPCsInMiniMap();
		int m = 0;	
		std::set<ACString> addedNPCs;
		for (int i = 0; i < NPCs.GetSize(); i++)
		{		
			if (NPCs[i]->IsMonsterNPC())
			{
				ACString npcName = NPCs[i]->GetName();

				// Verifica se o NPC já foi adicionado
				if (addedNPCs.find(npcName) == addedNPCs.end())
				{
					// Adiciona o NPC na lista e no conjunto de rastreamento
					m_pListArea->AddString(npcName);        
					m_pListArea->SetItemDataPtr(m, NPCs[i]);
					addedNPCs.insert(npcName);
					m++;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < list_matters.GetSize(); i++)
		{
			elementdataman *pData = GetGame()->GetElementDataMan();
			int id = -1;
				DATA_TYPE datatype;
				const void * dataptr = pData->get_data_ptr(list_matters[i], ID_SPACE_ESSENCE, datatype);
				if(dataptr == NULL || datatype == DT_INVALID)	return;
				
				switch(datatype) {
				case DT_MINE_ESSENCE:
					m_pListMonster->AddString(((MINE_ESSENCE*)dataptr)->name);
					break;
				case DT_REVIVESCROLL_ESSENCE:
					m_pListMonster->AddString(((REVIVESCROLL_ESSENCE*)dataptr)->name);
					break;
				case DT_MATERIAL_ESSENCE:
					m_pListMonster->AddString(((MATERIAL_ESSENCE*)dataptr)->name);
					break;
				default:
					break;
				}
		}

		int m = 0;	
		std::set<ACString> addedNPCs;
		abase::vector<CECMatter*> matters;
		CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
		ACString range = m_pTxt_Range->GetText();
		pMatterMan->FindMattersInRange(range.ToFloat(), true, matters);
		for (int i = 0; i < matters.size(); i++)	
		{
			ACString name = matters[i]->GetName();
			if (addedNPCs.find(name) == addedNPCs.end())
			{
				// Adiciona o NPC na lista e no conjunto de rastreamento
				m_pListArea->AddString(name);        
				m_pListArea->SetItemDataPtr(m, matters[i]);
				addedNPCs.insert(name);
				m++;
			}
		}
	}	
}

void CDlgAutoPolicy::OnShowDialog()
{
	if( !CECUIConfig::Instance().GetGameUI().bEnableAutoPolicy )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10931), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		Show(false);
		return;
	}

	/*Alien - foi removido a funcao cfgData e adicionado abaixo*/
	list_monsters.RemoveAll();
	list_matters.RemoveAll();
	CheckRadioButton(LIST_TYPE, CECAutoPolicy::GetInstance().GetConfigData().nMonsterOrMatter);
	CheckRadioButton(LIST_GROUP, CECAutoPolicy::GetInstance().GetConfigData().nAttackOrIgnore);
	m_pListMonster->ResetContent();
	m_pListArea->ResetContent();
	int i = 0;
	AString str_monster = CECAutoPolicy::GetInstance().GetConfigData().ignoreMonsterName;
	AString str_matter = CECAutoPolicy::GetInstance().GetConfigData().ignoreMatterName;
	CSplit patternSplitterMonster(str_monster);
	CSplit::VectorAString vecMonster = patternSplitterMonster.Split(";");
	if (!vecMonster.empty())
	{
		for (i = 0; i < vecMonster.size(); ++i)
		{			
			int id = vecMonster[i].ToInt();
			list_monsters.Add(id);			
		}
	}
	CSplit patternSplitterMatter(str_matter);
	CSplit::VectorAString vecMatter = patternSplitterMatter.Split(";");
	if (!vecMatter.empty())
	{
		for (i = 0; i < vecMatter.size(); ++i)
		{				
			int id = vecMatter[i].ToInt();
			list_matters.Add(id);			
		}
	}

	UpdateList();

	/*Ate aqui*/

	SetSkillImage(m_pImg_Skill1, cfgData.attack_skill, cfgData.attack_iscombo);
	SetSkillImage(m_pImg_Skill2, cfgData.assist_skill1, cfgData.assist_iscombo1);
	SetSkillImage(m_pImg_Skill3, cfgData.assist_skill2, cfgData.assist_iscombo2);
	SetSkillImage(m_pImg_Skill4, cfgData.assist_skill3, cfgData.assist_iscombo3);
	SetSkillImage(m_pImg_Skill5, cfgData.assist_skill4, cfgData.assist_iscombo4);

	ACString strText;
	strText.Format(_AL("%d"), cfgData.nAssistInterval1 / 1000);
	m_pTxt_Interval_Skill2->SetText(strText);
	strText.Format(_AL("%d"), cfgData.nAssistInterval2 / 1000);
	m_pTxt_Interval_Skill3->SetText(strText);
	strText.Format(_AL("%d"), cfgData.nAssistInterval3 / 1000);
	m_pTxt_Interval_Skill4->SetText(strText);
	strText.Format(_AL("%d"), cfgData.nAssistInterval4 / 1000);
	m_pTxt_Interval_Skill5->SetText(strText);
	strText.Format(_AL("%d"), cfgData.nTime / 60000);
	m_pTxt_Time->SetText(strText);
	strText.Format(_AL("%d"), cfgData.nPetrolRadius);
	m_pTxt_Range->SetText(strText);
	m_pCmb_AutoPick->SetCurSel(cfgData.iAutoPickMode);

	if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
		m_pBtn_Switch->SetText(GetStringFromTable(10925));
	else
		m_pBtn_Switch->SetText(GetStringFromTable(10924));

	m_pTxt_MonsterName->SetFocus(false);
}

void CDlgAutoPolicy::OnTick()
{
	if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
		m_pBtn_Switch->SetText(GetStringFromTable(10925));
	else
		m_pBtn_Switch->SetText(GetStringFromTable(10924));

	m_pLbl_RemainTime->SetText(GetRemainTimeTxt());
	GetDlgItem("Btn_AutoHPMP")->Enable(CEComputerAid::Instance().CanUse());

	if (bCollect)
	{
		
		CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
		CECMatter* tmpMatter = pMatterMan->FindMatterNearHostByList(cfgData.nPetrolRadius, true, list_matters, cfgData.nAttackOrIgnore == 1 ? false: true);
		if (tmpMatter)
		{
			if (g_pGame->GetGameRun()->GetHostPlayer()->PickupObject(tmpMatter->GetMatterID(), true))
			{
				
			}
		}
	}

	/*if (bCollect)
	{		
		int gather = 0;
		for(int i = 0; i < list_matters.GetSize(); i++)
		{
			CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
			ACString range = m_pTxt_Range->GetText();
			CECMatter* pMatter = pMatterMan->FindMatterNearHost(range.ToFloat(), true);
			if (GetCheckedRadioButton(1) == 1)
			{
				if (pMatter->GetTemplateID() == list_matters[i])
					continue;
			
				if (GetHostPlayer()->GetWorkMan()->IsPickingUp())
				{
					return;
				}

				if (gather == 1 && GetHostPlayer()->GetMoveControl().IsStop() && GetHostPlayer()->GetWorkMan()->IsPickingUp())
					gather = 0;

				if (gather == 0 && GetHostPlayer()->PickupObject(pMatter->GetMatterID(), pMatter->IsMine()) && !GetHostPlayer()->GetWorkMan()->IsPickingUp())
				{
					ACString str;
					str.Format(_AL("Movendo para %s"), pMatter->GetName());
					GetGameRun()->AddChatMessage(str, 0);
					bCollect = false;
				}
			} else if (GetCheckedRadioButton(1) == 2)
			{
				if (pMatter->GetTemplateID() != list_matters[i])
					continue;
			
				if (GetHostPlayer()->GetWorkMan()->IsPickingUp())
				{
					return;
				}

				if (gather == 1 && GetHostPlayer()->GetMoveControl().IsStop() && GetHostPlayer()->GetWorkMan()->IsPickingUp())
					gather = 0;

				if (gather == 0 && GetHostPlayer()->PickupObject(pMatter->GetMatterID(), pMatter->IsMine()) && !GetHostPlayer()->GetWorkMan()->IsPickingUp())
				{
					ACString str;
					str.Format(_AL("Movendo para %s"), pMatter->GetName());
					GetGameRun()->AddChatMessage(str, 0);
					gather = 1;
				}
			}
		}
	}*/
}

/*Alien - adicionado comandos de Add e Delete*/
void CDlgAutoPolicy::OnCommand_Add(const char *szCmd)
{
	if (m_pListMonster->GetCount() >= 10)
	{
		GetGameUIMan()->ShowErrorMsg(_AL("Erro!\r\nNúmero máximo permitido de monstros/materiais: 10"));
	}
	else
	{
		AString str = AC2AS(m_pTxt_MonsterName->GetText());
		if (str.GetLength() > 0)
		{
			elementdataman *pData = GetGame()->GetElementDataMan();
			DATA_TYPE dt;
			if (GetCheckedRadioButton(2) == 1)
			{
				MONSTER_ESSENCE *pEssence = (MONSTER_ESSENCE *)pData->get_data_ptr(atoi(str), ID_SPACE_ESSENCE, dt);
				if (pEssence && dt == DT_MONSTER_ESSENCE)
				{
					m_pListMonster->AddString(pEssence->name);
					int id = pEssence->id;
					list_monsters.Add(id);
					m_pTxt_MonsterName->SetText(_AL(""));
				}
			}
			else
			{
				int id = -1;
				DATA_TYPE datatype;
				const void * dataptr = pData->get_data_ptr(atoi(str), ID_SPACE_ESSENCE, datatype);
				if(dataptr == NULL || datatype == DT_INVALID)	return;
				
				switch(datatype) {
				case DT_MINE_ESSENCE:
					id = ((MINE_ESSENCE*)dataptr)->id;
					m_pListMonster->AddString(((MINE_ESSENCE*)dataptr)->name);
					list_matters.Add(id);
					m_pTxt_MonsterName->SetText(_AL(""));
					break;
				case DT_REVIVESCROLL_ESSENCE:
					id = ((REVIVESCROLL_ESSENCE*)dataptr)->id;
					m_pListMonster->AddString(((REVIVESCROLL_ESSENCE*)dataptr)->name);
					list_matters.Add(id);
					m_pTxt_MonsterName->SetText(_AL(""));
					break;
				case DT_MATERIAL_ESSENCE:
					id = ((MATERIAL_ESSENCE*)dataptr)->id;
					m_pListMonster->AddString(((MATERIAL_ESSENCE*)dataptr)->name);
					list_matters.Add(id);
					m_pTxt_MonsterName->SetText(_AL(""));
					break;
				default:
					break;
				}
			}
		} 
		else 
		{
			if (GetCheckedRadioButton(2) == 1)
			{
				CECNPC *pNpc = (CECNPC *)m_pListArea->GetItemDataPtr(m_pListArea->GetCurSel());
				m_pListMonster->AddString(pNpc->GetName());	
				int id = pNpc->GetTemplateID();
				list_monsters.Add(id);	
			}
			else
			{
				CECMatter *pNpc = (CECMatter*)m_pListArea->GetItemDataPtr(m_pListArea->GetCurSel());
				m_pListMonster->AddString(pNpc->GetName());	
				int id = pNpc->GetTemplateID();
				list_matters.Add(id);	
			}
		}
	}
}

void CDlgAutoPolicy::OnCommand_Delete(const char *szCmd)
{
	int nSel = m_pListMonster->GetCurSel();
	if (nSel >= 0 && nSel < m_pListMonster->GetCount())
	{
		m_pListMonster->DeleteString(m_pListMonster->GetCurSel());
		if (GetCheckedRadioButton(LIST_TYPE) == 1)
			list_monsters.RemoveAt(nSel);
		else list_matters.RemoveAt(nSel);
	}
}
/*Ate aqui*/

void CDlgAutoPolicy::OnCommand_Confirm(const char * szCommand)
{
	SaveConfigData();
	Show(false);

	CECAutoPolicy& autoPolicy = CECAutoPolicy::GetInstance();
	if( autoPolicy.IsAutoPolicyEnabled() )
		autoPolicy.SendEvent_ConfigChanged();
}

void CDlgAutoPolicy::OnCommand_StartOrStop(const char * szCommand)
{
	if( !CECUIConfig::Instance().GetGameUI().bEnableAutoPolicy )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10931), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		Show(false);
		return;
	}

	if( !szCommand )
		OnShowDialog();

	if( !m_pImg_Skill1->GetData() && cfgData.nMonsterOrMatter == 0)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10923), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	if( CECAutoPolicy::GetInstance().GetPlayerWrapper()->GetWeaponEndurance() == 0 && cfgData.nMonsterOrMatter == 0)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10932), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	int iTime = a_atoi(m_pTxt_Time->GetText());
	if( iTime <= 0 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10926), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	if( IsShow() )
		SaveConfigData();

	if (GetCheckedRadioButton(LIST_TYPE) == 1)
	{
		bCollect = false;
		CECAutoPolicy& policy = CECAutoPolicy::GetInstance();
		if( policy.IsAutoPolicyEnabled() )
			policy.StopPolicy();
		else
			policy.StartPolicy(CECAutoPolicy::POLICY_AUTOKILLMONSTER);
	}
	else 
	{
		bCollect = true;
		CECAutoPolicy& policy = CECAutoPolicy::GetInstance();
		if( policy.IsAutoPolicyEnabled() )
			policy.StopPolicy();
		else
			policy.StartPolicy(CECAutoPolicy::POLICY_COLLECTMATTER);
	}
}

void CDlgAutoPolicy::OnCommand_SetAutoHPMP(const char * szCommand)
{
	GetGameUIMan()->m_pDlgAutoHPMP->SwitchShow();
}

void CDlgAutoPolicy::OnCommand_Switch(const char * szCommand)
{
	UpdateList();
}

void CDlgAutoPolicy::OnEvent_LButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	if( pObj == m_pImg_Skill1 )
	{
		m_pImg_Skill1->ClearCover();
		m_pImg_Skill1->SetData(0);
		m_pImg_Skill1->SetData64(0);
		m_pImg_Skill1->SetHint(_AL(""));
	}
	else if( pObj == m_pImg_Skill2 )
	{
		m_pImg_Skill2->ClearCover();
		m_pImg_Skill2->SetData(0);
		m_pImg_Skill2->SetData64(0);
		m_pImg_Skill2->SetHint(_AL(""));
	}
	else if( pObj == m_pImg_Skill3 )
	{
		m_pImg_Skill3->ClearCover();
		m_pImg_Skill3->SetData(0);
		m_pImg_Skill3->SetData64(0);
		m_pImg_Skill3->SetHint(_AL(""));
	}
	else if( pObj == m_pImg_Skill4 )
	{
		m_pImg_Skill4->ClearCover();
		m_pImg_Skill4->SetData(0);
		m_pImg_Skill4->SetData64(0);
		m_pImg_Skill4->SetHint(_AL(""));
	}
	else if( pObj == m_pImg_Skill5 )
	{
		m_pImg_Skill5->ClearCover();
		m_pImg_Skill5->SetData(0);
		m_pImg_Skill5->SetData64(0);
		m_pImg_Skill5->SetHint(_AL(""));
	}
}

void CDlgAutoPolicy::DragDropItem(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if(strstr(pDlgSrc->GetName(), "Win_SkillSubListSkillItem")		// 20140708 新的技能学习对话框
		|| !stricmp(pDlgSrc->GetName(), "Win_SkillSubOther")		// 20140708 新的连锁技能对话框
		|| !stricmp(pDlgSrc->GetName(), "Win_SkillSubPool"))	
	{
		if( strstr(pObjSrc->GetName(), "Item") 
			|| strstr(pObjSrc->GetName(), "Img_ConSkill") )		// 20140708 新的连锁技能对话框
		{
			int nCombo = pObjSrc->GetData();
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkillImg ) SetSkillImage(pSkillImg, nCombo - 1, true);
		}
		else
		{
			CECSkill* pSkill = (CECSkill*)pObjSrc->GetDataPtr("ptr_CECSkill");
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkill && pSkillImg ) SetSkillImage(pSkillImg, pSkill->GetSkillID(), false);
		}
	}
	else if( strstr(pDlgSrc->GetName(), "Win_Quickbar") )
	{
		CECShortcut* pSC = (CECShortcut*)pObjSrc->GetDataPtr("ptr_CECShortcut");
		if( !pSC ) return;

		if( pSC->GetType() == CECShortcut::SCT_SKILL )
		{
			CECSCSkill* pSCSkill = dynamic_cast<CECSCSkill*>(pSC);
			CECSkill* pSkill = (CECSkill*)pSCSkill->GetSkill();
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkill && pSkillImg ) SetSkillImage(pSkillImg, pSkill->GetSkillID(), false);
		}
		else if( pSC->GetType() == CECShortcut::SCT_SKILLGRP )
		{
			CECSCSkillGrp* pSCSkillGrp = dynamic_cast<CECSCSkillGrp*>(pSC);
			int nCombo = pSCSkillGrp->GetGroupIndex();
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkillImg ) SetSkillImage(pSkillImg, nCombo, true);
		}
		else if( pSC->GetType() == CECShortcut::SCT_COMMAND )
		{
			CECSCCommand* pSCCommand = dynamic_cast<CECSCCommand*>(pSC);
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSCCommand->GetCommandID() == CECSCCommand::CMD_NORMALATTACK && pSkillImg )
				SetSkillImage(pSkillImg, -1, false);
		}
	}
	else if(!stricmp(pDlgSrc->GetName(), "Win_SkillSubAction") )		// 20140708 新的动作对话框
	{
		if( !stricmp(pObjSrc->GetName(), "BscCmd_03") )
		{
			PAUIIMAGEPICTURE pSkillImg = dynamic_cast<PAUIIMAGEPICTURE>(pObjOver);
			if( pSkillImg ) SetSkillImage(pSkillImg, -1, false);
		}
	}
}

void CDlgAutoPolicy::SaveConfigData()
{
	int nNum = 0;
	unsigned __int64 nTemp;
	CECAutoPolicy::CONFIG cfgData;

	cfgData.attack_iscombo = (m_pImg_Skill1->GetData() == 1);
	if( m_pImg_Skill1->GetData() == 3 )
		cfgData.attack_skill = -1;	// 普攻
	else
	{
		nTemp = m_pImg_Skill1->GetData64();
		cfgData.attack_skill = (int)nTemp;
	}

	cfgData.assist_iscombo1 = (m_pImg_Skill2->GetData() == 1);	
	nTemp = m_pImg_Skill2->GetData64();
	cfgData.assist_skill1 = (int)nTemp;
	cfgData.assist_iscombo2 = (m_pImg_Skill3->GetData() == 1);	
	nTemp = m_pImg_Skill3->GetData64();
	cfgData.assist_skill2 = (int)nTemp;
	cfgData.assist_iscombo3 = (m_pImg_Skill4->GetData() == 1);	
	nTemp = m_pImg_Skill4->GetData64();
	cfgData.assist_skill3 = (int)nTemp;
	cfgData.assist_iscombo4 = (m_pImg_Skill5->GetData() == 1);	
	nTemp = m_pImg_Skill5->GetData64();
	cfgData.assist_skill4 = (int)nTemp;

	nNum = a_atoi(m_pTxt_Interval_Skill2->GetText());
	a_Clamp(nNum, 1, 999999);
	cfgData.nAssistInterval1 = nNum * 1000;
	nNum = a_atoi(m_pTxt_Interval_Skill3->GetText());
	a_Clamp(nNum, 1, 999999);
	cfgData.nAssistInterval2 = nNum * 1000;
	nNum = a_atoi(m_pTxt_Interval_Skill4->GetText());
	a_Clamp(nNum, 1, 999999);
	cfgData.nAssistInterval3 = nNum * 1000;
	nNum = a_atoi(m_pTxt_Interval_Skill5->GetText());
	a_Clamp(nNum, 1, 999999);
	cfgData.nAssistInterval4 = nNum * 1000;
	nNum = a_atoi(m_pTxt_Time->GetText());
	a_Clamp(nNum, 1, 9999);
	cfgData.nTime = nNum * 60000;
	nNum = a_atoi(m_pTxt_Range->GetText());
	a_Clamp(nNum, MIN_RANGE, 9999);
	cfgData.nPetrolRadius = nNum;
	cfgData.iAutoPickMode = m_pCmb_AutoPick->GetCurSel();

	/*Alien - adicionado abaixo*/
	int cnt = list_monsters.GetSize();
	ACString str;
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			if (i == 0)
				str.Format(_AL("%d"), list_monsters[i]);
			else
				str.Format(_AL("%s;%d"), str, list_monsters[i]);
		}
		cfgData.ignoreMonsterName = AC2AS(str);
	}
	int cnt2 = list_matters.GetSize();
	if (cnt2 > 0)
	{
		for (int i = 0; i < cnt2; i++)
		{
			if (i == 0)
				str.Format(_AL("%d"), list_matters[i]);
			else
				str.Format(_AL("%s;%d"), str, list_matters[i]);
		}
		cfgData.ignoreMatterName = AC2AS(str);
	}
	cfgData.nAttackOrIgnore = GetCheckedRadioButton(1);
	cfgData.nMonsterOrMatter = GetCheckedRadioButton(2);
	this->cfgData = cfgData;
	/*Ate aqui*/

	CECAutoPolicy::GetInstance().SetConfigData(cfgData);
}

void CDlgAutoPolicy::SetSkillImage(PAUIIMAGEPICTURE pSkillImg, int skill_id, bool bCombo)
{
	const EC_VIDEO_SETTING& vs = GetGame()->GetConfigs()->GetVideoSettings();

	if( bCombo )
	{
		if( skill_id >= 0 && skill_id < EC_COMBOSKILL_NUM )
		{
			int iIcon = vs.comboSkill[skill_id].nIcon;
			if( iIcon )
			{
				pSkillImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], iIcon + 1);
				pSkillImg->SetData(1);
				pSkillImg->SetData64(skill_id);

				ACString strText;
				strText.Format(GetStringFromTable(804), skill_id);
				pSkillImg->SetHint(strText);
			}
			else
			{
				pSkillImg->ClearCover();
				pSkillImg->SetData(0);
				pSkillImg->SetData64(0);
				pSkillImg->SetHint(_AL(""));
			}
		}
		else
		{
			pSkillImg->ClearCover();
			pSkillImg->SetData(0);
			pSkillImg->SetData64(0);
			pSkillImg->SetHint(_AL(""));
		}
	}
	else
	{
		if( skill_id == -1 && pSkillImg != m_pImg_Skill2 )
		{
			CECShortcutSet* pBscSCS = GetGameRun()->GetGenCmdShortcuts();
			CECShortcut* pSC = pBscSCS->GetShortcut(2);
			if( pSC )
			{
				AString strFile;
				af_GetFileTitle(pSC->GetIconFile(), strFile);
				strFile.MakeLower();
				pSkillImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
				pSkillImg->SetData(3);
				pSkillImg->SetData64(0);

				const wchar_t* pszDesc = pSC->GetDesc();
				if( pszDesc )
				{
					AUICTranslate trans;
					AWString strHint = trans.Translate(pszDesc);
					pSkillImg->SetHint(strHint);
				}
				else
					pSkillImg->SetHint(_AL(""));
			}
		}
		else if( skill_id == -1 && pSkillImg != m_pImg_Skill3 )
		{
			CECShortcutSet* pBscSCS = GetGameRun()->GetGenCmdShortcuts();
			CECShortcut* pSC = pBscSCS->GetShortcut(2);
			if( pSC )
			{
				AString strFile;
				af_GetFileTitle(pSC->GetIconFile(), strFile);
				strFile.MakeLower();
				pSkillImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
				pSkillImg->SetData(3);
				pSkillImg->SetData64(0);

				const wchar_t* pszDesc = pSC->GetDesc();
				if( pszDesc )
				{
					AUICTranslate trans;
					AWString strHint = trans.Translate(pszDesc);
					pSkillImg->SetHint(strHint);
				}
				else
					pSkillImg->SetHint(_AL(""));
			}
		}
		else if( skill_id == -1 && pSkillImg != m_pImg_Skill4 )
		{
			CECShortcutSet* pBscSCS = GetGameRun()->GetGenCmdShortcuts();
			CECShortcut* pSC = pBscSCS->GetShortcut(2);
			if( pSC )
			{
				AString strFile;
				af_GetFileTitle(pSC->GetIconFile(), strFile);
				strFile.MakeLower();
				pSkillImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
				pSkillImg->SetData(3);
				pSkillImg->SetData64(0);

				const wchar_t* pszDesc = pSC->GetDesc();
				if( pszDesc )
				{
					AUICTranslate trans;
					AWString strHint = trans.Translate(pszDesc);
					pSkillImg->SetHint(strHint);
				}
				else
					pSkillImg->SetHint(_AL(""));
			}
		}
		else if( skill_id == -1 && pSkillImg != m_pImg_Skill5 )
		{
			CECShortcutSet* pBscSCS = GetGameRun()->GetGenCmdShortcuts();
			CECShortcut* pSC = pBscSCS->GetShortcut(2);
			if( pSC )
			{
				AString strFile;
				af_GetFileTitle(pSC->GetIconFile(), strFile);
				strFile.MakeLower();
				pSkillImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ACTION],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ACTION][strFile]);
				pSkillImg->SetData(3);
				pSkillImg->SetData64(0);

				const wchar_t* pszDesc = pSC->GetDesc();
				if( pszDesc )
				{
					AUICTranslate trans;
					AWString strHint = trans.Translate(pszDesc);
					pSkillImg->SetHint(strHint);
				}
				else
					pSkillImg->SetHint(_AL(""));
			}
		}
		else
		{
			CECSkill* pSkill = GetHostPlayer()->GetNormalSkill(skill_id);
			if( !pSkill ) pSkill = GetHostPlayer()->GetEquipSkillByID(skill_id);
			if( pSkill )
			{
				GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pSkillImg, pSkill);
				pSkillImg->SetData(2);
				pSkillImg->SetData64(skill_id);
			}
			else
			{
				GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pSkillImg, NULL);
				pSkillImg->SetData(0);
				pSkillImg->SetData64(0);
			}
		}
	}
}

ACString CDlgAutoPolicy::GetRemainTimeTxt()
{
	ACString strText;
	int nSec = CECAutoPolicy::GetInstance().GetRemainTime() / 1000;

	int nHour, nMin;
	nHour = nSec / 3600;
	nSec -= nHour * 3600;
	nMin = nSec / 60;
	nSec -= nMin * 60;
	strText.Format(GetStringFromTable(10927), nHour, nMin, nSec);
	return strText;
}