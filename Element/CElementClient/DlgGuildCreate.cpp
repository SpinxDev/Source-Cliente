// Filename	: DlgGuildCreate.cpp
// Creator	: Tom Zhou
// Date		: October 25, 2005

#include "DlgGuildCreate.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_Faction.h"

#include "EC_Game.h"
#include "elementdataman.h"
#include "EC_HostPlayer.h"
#include "gnoctets.h"
#include "factiondata.hxx"
#include "ids.hxx"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGuildCreate, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommand_confirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

CDlgGuildCreate::CDlgGuildCreate()
{
}

CDlgGuildCreate::~CDlgGuildCreate()
{
}

bool CDlgGuildCreate::OnInitDialog()
{
	m_pDEFAULT_Txt_Input = GetDlgItem("DEFAULT_Txt_Input");
	isCustomCreate = false;

	return true;
}

inline size_t a_str_size(const ACHAR* sz)
{
	return a_strlen(sz) * sizeof(ACHAR);
}

void CDlgGuildCreate::OnCommand_confirm(const char * szCommand)
{
	ACString strName = m_pDEFAULT_Txt_Input->GetText();
	ACString strProclaim =  _AL("");
	if( strName.GetLength() <= 0 ) return;
	if (isCustomCreate)
	{
		if (strName)
		{
			Octets o = GNET::create_param_ct(Octets(strName, a_str_size(strName)), Octets(strProclaim, a_str_size(strProclaim))).marshal();
			int iAction = GNET::_O_FACTION_CREATE;
			void* pDataBuf = o.begin();
			int iDataSize = o.size();

			using namespace C2S;

		#pragma pack(1)
			
			struct CONTENT
			{
				int iAction;
				int codigo;
			};   

		#pragma pack()

			int iSize = sizeof (cmd_header) + sizeof (CONTENT) + iDataSize;
			BYTE* pBuf = (BYTE*)a_malloctemp(iSize);
			if (!pBuf)
				return;
			
			((cmd_header*)pBuf)->cmd = 50003;

			BYTE* pData = pBuf + sizeof (cmd_header);
			CONTENT* pContent = (CONTENT*)pData;
			pContent->iAction = iAction;
			pContent->codigo = 501350;
			pData += sizeof (CONTENT);

			if (pDataBuf)
				memcpy(pData, pDataBuf, iDataSize);

			g_pGame->GetGameSession()->SendGameData(pBuf, iSize);
			a_freetemp(pBuf);
		}
	} else 	GetGameSession()->faction_create(strName, _AL(""));
	GetGameUIMan()->EndNPCService();
	Show(false);
}

void CDlgGuildCreate::OnCommand_CANCEL(const char * szCommand)
{
	GetGameUIMan()->EndNPCService();
	isCustomCreate = false;
	Show(false);
}
