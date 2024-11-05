#include "DlgGuildSearch.h"
#include "DlgGuildDiplomacyMan.h"
#include "DlgGuildMan.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Faction.h"
#include "AWStringWithWildcard.h"
#include "Network\\factionlistonline_re.hpp"

#include "EC_ManPlayer.h"
#include "EC_Player.h"
#include "EC_World.h"
#include "EC_Game.h"
#include "EC_ElsePlayer.h"
#include "EC_ManNPC.h"
#include "DlgGuildCreate.h"
#include "EC_GameRun.h"
#include "CSplit.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGuildSearch, CDlgBase)
AUI_ON_COMMAND("Btn_Create", OnCommand_Btn_Create)
AUI_ON_COMMAND("Btn_Invite", OnCommand_Btn_Invite)
AUI_ON_COMMAND("Btn_Search", OnCommand_Btn_Search)
AUI_ON_COMMAND("Edt_GuildName", OnCommand_Btn_Search)
AUI_ON_COMMAND("Btn_Num", OnCommand_Sort_By_NUM)
AUI_ON_COMMAND("Btn_Level", OnCommand_Sort_By_LEVEL)
AUI_ON_COMMAND("Btn_Name", OnCommand_Sort_By_NAME)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGuildSearch, CDlgBase)
AUI_ON_EVENT("List_Guild",	WM_LBUTTONUP,	OnEventLButtonUp)
AUI_END_EVENT_MAP()

CDlgGuildSearch::CDlgGuildSearch()
{
	m_pList_Guild = NULL;
	m_pBtn_Search = NULL;
	m_pEdt_GuildName = NULL;
	m_pTxt_Message = NULL;
	m_bFactionInfoReady = false;
	m_queryCounter.SetPeriod(1000);
	m_queryCounter.Reset(true);
}

bool CDlgGuildSearch::OnInitDialog()
{
	DDX_Control("List_Guild", m_pList_Guild);
	DDX_Control("Btn_Search", m_pBtn_Search);
	DDX_Control("Edt_GuildName", m_pEdt_GuildName);
	DDX_Control("Txt_Message", m_pTxt_Message);
	return true;
}

void CDlgGuildSearch::OnShowDialog()
{
	Refresh();
}

void CDlgGuildSearch::Refresh()
{
	//	 清除帮派数据
	m_factionArray.clear();
	m_factionMasterArray.clear();
	m_factionProclaimArray.clear();
	GetGameSession()->faction_get_onlinelist();
	m_bFactionInfoReady = false;
	m_queryCounter.Reset(true);

	//	更新界面
	UpdateUI();
}

void CDlgGuildSearch::OnCommand_Btn_Create(const char *szCommand)
{
	GetGameUIMan()->m_pDlgGuildCreate->Show(true);
	GetGameUIMan()->m_pDlgGuildCreate->isCustomCreate = true;
}

void CDlgGuildSearch::OnCommand_Btn_Invite(const char *szCommand)
{
	
}

void CDlgGuildSearch::OnCommand_Btn_Search(const char *szCommand)
{
	if (!m_bFactionInfoReady)	return;

	//	获取输入
	ACString strSearch = m_pEdt_GuildName->GetText();
	strSearch.TrimLeft();		//	除去左侧空格
	strSearch.TrimRight();	//	除去右侧空格

	//	逐个匹配
	m_pList_Guild->ResetContent();

	ACString strPattern = _AL("*") + strSearch + _AL("*");
	ACStringWithWildcard matcher;

	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strName, strLine;
	for (size_t i = 0; i < m_factionArray.size(); ++ i)
	{
		int idFaction = m_factionArray[i];
		const Faction_Info *pFInfo = pFMan->GetFaction(idFaction, false);
		if (!pFInfo)	continue;

		strName = pFInfo->GetName();
		if (strName.IsEmpty())	continue;
		if (!matcher.IsMatch(strName, strPattern))
			continue;
		
		ACString strMaster = ACString((const ACHAR*)m_factionMasterArray[i].begin(), m_factionMasterArray[i].size() / sizeof(ACHAR));
		ACString strProclaim = ACString((const ACHAR*)m_factionProclaimArray[i].begin(), m_factionProclaimArray[i].size() / sizeof(ACHAR));
		int idMaster = GetGameRun()->GetPlayerID(strMaster);
		if( idMaster == 0 )	// online and valid
			GetGameSession()->GetPlayerIDByName(strMaster, 0);
		if (GetLineString(idFaction, strLine, strMaster, strProclaim))
		{
			m_pList_Guild->AddString(strLine);
			m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, idFaction, LIST_FACTION_INDEX);
			m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, idMaster, LIST_FACTION_MASTER);
			ACString *newProc = new ACString(strProclaim);
			m_pList_Guild->SetItemDataPtr(m_pList_Guild->GetCount()-1, newProc, LIST_PROCLAIM_INDEX);
		}
	}

	//	显示当前显示的帮派个数及帮派总数
	/*ACString strNum;
	strNum.Format(_AL("%d/%d"), m_pList_Guild->GetCount(), m_factionArray.size());
	GetDlgItem("Btn_Num")->SetHint(strNum);*/
}


void CDlgGuildSearch::OnCommand_Sort_By_NUM(const char *szCommand)
{
	if (!m_bFactionInfoReady)	return;
	m_pList_Guild->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_INT, 1);
}

void CDlgGuildSearch::OnCommand_Sort_By_LEVEL(const char *szCommand)
{
	if (!m_bFactionInfoReady)	return;
	m_pList_Guild->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_INT, 2);
}

void CDlgGuildSearch::OnCommand_Sort_By_NAME(const char *szCommand)
{
	if (!m_bFactionInfoReady)	return;
	m_pList_Guild->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_STRING, 0);
}

int CDlgGuildSearch::GetFactionSel()
{
	//	获取当前选中的帮派 id
	int idFaction(0);

	int nSel = m_pList_Guild->GetCurSel();
	if (nSel >= 0 && nSel < m_pList_Guild->GetCount())
	{
		idFaction = m_pList_Guild->GetItemData(nSel, LIST_FACTION_INDEX);
	}

	return idFaction;
}

int CDlgGuildSearch::GetFactionMaster()
{
	int idMaster(0);
	int nSel = m_pList_Guild->GetCurSel();
	if (nSel >= 0 && nSel < m_pList_Guild->GetCount())
	{
		idMaster = m_pList_Guild->GetItemData(nSel, LIST_FACTION_MASTER);
	}
	return idMaster;
}

void CDlgGuildSearch::UpdateUI()
{
	//	更新当前页在页面中的内容

	//	清除原内容
	m_pList_Guild->ResetContent();

	CECPlayerMan* pMan = GetWorld()->GetPlayerMan();
	//	添加新内容
	if (!m_factionArray.empty())
	{
		ACString strText;
		for (size_t i = 0; i < m_factionArray.size(); ++ i)
		{
			int idFaction = m_factionArray[i];
			ACString strMaster = ACString((const ACHAR*)m_factionMasterArray[i].begin(), m_factionMasterArray[i].size() / sizeof(ACHAR));
			ACString strProclaim = ACString((const ACHAR*)m_factionProclaimArray[i].begin(), m_factionProclaimArray[i].size() / sizeof(ACHAR));
			int idMaster = GetGameRun()->GetPlayerID(strMaster);
			if( idMaster == 0 )	// online and valid
				GetGameSession()->GetPlayerIDByName(strMaster, 0);
			if (GetLineString(idFaction, strText, strMaster, strProclaim))
			{				
				m_pList_Guild->AddString(strText);				
			}
			else
			{
				//	记录暂时未查询到的帮派信息，在OnTick中查询更新
				m_pList_Guild->AddString(strText);
				m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, QUERY_HINT_WAITING, LIST_QUERYHINT_INDEX);		//	需向服务器查询
			}
			m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, idFaction, LIST_FACTION_INDEX);
			m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, idMaster, LIST_FACTION_MASTER);
			ACString *newProc = new ACString(strProclaim);
			m_pList_Guild->SetItemDataPtr(m_pList_Guild->GetCount()-1, newProc, LIST_PROCLAIM_INDEX);
		}
	}

	//	显示当前显示的帮派个数及帮派总数
	/*ACString strNum;
	strNum.Format(_AL("%d/%d"), m_pList_Guild->GetCount(), m_factionArray.size());
	GetDlgItem("Btn_Num")->SetHint(strNum);*/

	//	重置搜索界面
	m_pEdt_GuildName->SetText(_AL(""));
	m_pEdt_GuildName->Enable(false);
	m_pBtn_Search->Enable(false);
}

ACString CDlgGuildSearch::GetProclaimDesc(ACString proclaim)
{
	CSplit s(proclaim);
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\\g"));
	int pvp = -1;
	int level = -1;	
	ACString newProclaim;
	for(int i = 0; i < vec.size(); i++)
	{
		ACString str = vec[i];
		if (str.Left(1).ToInt() == 1)
		{
			str.CutLeft(1);
			if (str.ToInt() == 1)
				newProclaim += _AL("[PVP] ");
			else newProclaim += _AL("[PVE] ");
		}
		if (str.Left(1).ToInt() == 2)
		{
			str.CutLeft(1);
			ACString ns;
			if (str.ToInt() == 1)			
				newProclaim += _AL("Restrito ");
			else newProclaim += _AL("Aberto ");
		}
		if (str.Left(1).ToInt() == 3)
		{
			str.CutLeft(1);		
			newProclaim += ACString().Format(_AL("(Nv. %d+)"), str.ToInt());
		}
	}



	return newProclaim;
}

bool CDlgGuildSearch::GetLineString(int idFaction, ACString &strText, ACString strMaster, ACString strProclaim)
{
	bool bRet(false);
	CECFactionMan *pFMan = GetGame()->GetFactionMan();	
	const Faction_Info *pFInfo = pFMan->GetFaction(idFaction, false);
	if (pFInfo)
	{
		ACString strName = pFInfo->GetName();
		if (strName.IsEmpty())
			strName = _AL("-");
		if (strMaster.IsEmpty())
			strMaster = _AL("-");
		if (strProclaim.IsEmpty())
			strProclaim = _AL("-");
		strText.Format(_AL("%s\t%s\t%d\t%d\t%s"), strName, strMaster, pFInfo->GetMemNum(), pFInfo->GetLevel(), GetProclaimDesc(strProclaim));
		bRet = true;
	}
	else
	{
		strText = _AL("-\t-\t-\t-\t-");
	}
	return bRet;
}

void CDlgGuildSearch::OnTick()
{
	//	检查是否有帮派信息需要更新显示
	//
	if (m_pList_Guild->GetCount() == 0) return;	//	列表为空时返回，服务器还未发送列表、或列表本身即为空
	if (m_bFactionInfoReady) return;					//	信息完整时返回，无需做多余计算

	bool bCanQueryNow = m_queryCounter.IncCounter(GetGame()->GetRealTickTime());

	const int MAX_QUERY_ONCE = 50;				//	每次最多向服务器查询的帮派名称个数
	AArray<int, int> factionIDs;
	bool bReady(true);

	ACString strText;
	for (int i = 0; i < m_pList_Guild->GetCount(); ++ i)
	{
		DWORD dwData = m_pList_Guild->GetItemData(i, LIST_QUERYHINT_INDEX);
		if (dwData == QUERY_HINT_OK)
			continue;

		//	待向服务器查询、已向服务器查询两种状态下都可尝试获取帮派信息
		int idFaction = m_pList_Guild->GetItemData(i, LIST_FACTION_INDEX);
		ACString strMaster = ACString((const ACHAR*)m_factionMasterArray[i].begin(), m_factionMasterArray[i].size() / sizeof(ACHAR));
		ACString strProclaim = ACString((const ACHAR*)m_factionProclaimArray[i].begin(), m_factionProclaimArray[i].size() / sizeof(ACHAR));
		int idMaster = GetGameRun()->GetPlayerID(strMaster);
		if( idMaster == 0 )	// online and valid
			GetGameSession()->GetPlayerIDByName(strMaster, 0);
		if (GetLineString(idFaction, strText, strMaster, strProclaim))
		{
			//	更新显示并清除相应标记
			m_pList_Guild->SetText(i, strText);
			m_pList_Guild->SetItemData(i, QUERY_HINT_OK, LIST_QUERYHINT_INDEX);
			m_pList_Guild->SetItemData(i, idMaster, LIST_FACTION_MASTER);
			ACString *newProc = new ACString(strProclaim);
			m_pList_Guild->SetItemDataPtr(i, newProc, LIST_PROCLAIM_INDEX);
			continue;
		}

		bReady = false;	//	至少有一个帮派信息还未获取

		if (dwData == QUERY_HINT_SENT)
			continue;		//	已向服务器查询

		if (!bCanQueryNow)
			continue;		//	查询冷却中

		//	添加查询
		if (factionIDs.GetSize() < MAX_QUERY_ONCE)
		{
			m_pList_Guild->SetItemData(i, QUERY_HINT_SENT, LIST_QUERYHINT_INDEX);
			m_pList_Guild->SetItemData(i, idMaster, LIST_FACTION_MASTER);
			ACString *newProc = new ACString(strProclaim);
			m_pList_Guild->SetItemDataPtr(i, newProc, LIST_PROCLAIM_INDEX);
			factionIDs.Add(idFaction);
		}
	}

	m_bFactionInfoReady = bReady;

	if (m_bFactionInfoReady)
	{
		//	搜索功能可以开放了
		m_pEdt_GuildName->Enable(true);
		m_pBtn_Search->Enable(true);
	}

	if (factionIDs.GetSize() > 0)
	{
		//GetGameSession()->GetFactionInfo(factionIDs.GetSize(), factionIDs.GetData());
		GetGameSession()->GetFactionInfoImmediately(factionIDs.GetSize(), factionIDs.GetData());
		m_queryCounter.Reset();
	}
}

void CDlgGuildSearch::OnPrtcFactionListOnline_Re(GNET::FactionListOnline_Re *pProtocol)
{
	// Alien - Recebe protocolo de Faction List Online
	if (pProtocol)
	{
		//	清除并拷贝新帮派列表
		m_factionArray.clear();
		m_factionMasterArray.clear();
		m_factionProclaimArray.clear();

		m_factionMasterArray.reserve(pProtocol->fid_list.size());
		m_factionArray.reserve(pProtocol->fid_list.size());
		m_factionProclaimArray.reserve(pProtocol->proclaim.size());

		for (size_t i = 0; i < pProtocol->fid_list.size(); ++ i){
			m_factionArray.push_back(pProtocol->fid_list[i]);
			m_factionMasterArray.push_back(pProtocol->fid_master[i]);
			m_factionProclaimArray.push_back(pProtocol->proclaim[i]);
		}		

		//	避免收到当前未预期的此协议
		m_bFactionInfoReady = false;
		m_queryCounter.Reset(true);

		//	更新界面
		UpdateUI();
	}
}

void CDlgGuildSearch::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{	
	int nSel = m_pList_Guild->GetCurSel();
	if (nSel >= 0 && nSel < m_pList_Guild->GetCount())
	{
		ACString* str = (ACString*)m_pList_Guild->GetItemDataPtr(nSel, LIST_PROCLAIM_INDEX);
		CSplit s(*str);
		CSPLIT_STRING_VECTOR vec = s.Split(_AL("\\g"));
		for (int i = 0; i < vec.size(); i++)
		{
			if (vec[i].ToInt() == 5)
			{
				vec[i].CutLeft(1);				
				m_pTxt_Message->SetText(vec[i]);
			}
		}
	}
}