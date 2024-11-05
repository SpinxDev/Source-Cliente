// Filename	: DlgGuildSearch.h
// Creator	: Xu Wenbin
// Date		: 2010/11/10

#pragma once

#include "EC_Counter.h"

#include "DlgBase.h"
#include <AUIListBox.h>
#include <vector.h>
#include <iostream>
#include <map>
#include <string>
#include "gnoctets.h"
#include "DlgAutoPolicyMatter.h"

namespace GNET
{
	class FactionListOnline_Re;
}

class CDlgGuildSearch : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgGuildSearch();

	void OnCommand_Btn_Create(const char *szCommand);
	void OnCommand_Btn_Invite(const char *szCommand);
	void OnCommand_Btn_Search(const char *szCommand);
	void OnCommand_Sort_By_NUM(const char *szCommand);
	void OnCommand_Sort_By_LEVEL(const char *szCommand);
	void OnCommand_Sort_By_NAME(const char *szCommand);
	
	void OnPrtcFactionListOnline_Re(GNET::FactionListOnline_Re *pProtocol);
	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void Refresh();
	void UpdateUI();
	int	 GetFactionSel();
	int GetFactionMaster();
	bool GetLineString(int idFaction, ACString &strText, ACString strMaster = _AL(""), ACString strProclaim = _AL(""));
	ACString GetProclaimDesc(ACString proclaim);
	
private:
	enum
	{
		LIST_QUERYHINT_INDEX = 0,		//	帮派信息是否已获取
		LIST_FACTION_INDEX = 1,			//	帮派ID在列表数据中的下标
		LIST_FACTION_MASTER = 2,
		LIST_PROCLAIM_INDEX = 3,
	};
	enum
	{
		QUERY_HINT_OK = 0,					//	帮派信息已获取
		QUERY_HINT_WAITING = 1,			//	待向服务器查询
		QUERY_HINT_SENT = 2,				//	已向服务器查询，待返回
	};
	PAUILISTBOX	m_pList_Guild;
	PAUIOBJECT	m_pBtn_Search;
	PAUIOBJECT	m_pEdt_GuildName;
	PAUIOBJECT	m_pTxt_Message;

	abase::vector<int> m_factionArray;		//	所有满足条件的帮派列表
	abase::vector<GNET::Octets> m_factionMasterArray;
	abase::vector<GNET::Octets> m_factionProclaimArray;
	bool					m_bFactionInfoReady;	//	所有帮派名称等信息是否都已查到
	CECCounter		m_queryCounter;
};