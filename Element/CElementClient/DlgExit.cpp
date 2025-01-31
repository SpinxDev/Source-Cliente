// Filename	: DlgExit.cpp
// Creator	: Tom Zhou
// Date		: October 27, 2005

#include "DlgExit.h"
#include "EC_Game.h" // Alien - adicionado
#include "EC_MsgDef.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "Network\\gnetdef.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgExit, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommand_confirm)

AUI_END_COMMAND_MAP()

CDlgExit::CDlgExit()
{
}

CDlgExit::~CDlgExit()
{
}

bool CDlgExit::OnInitDialog()
{
	m_pChk_Forcequit = (PAUICHECKBOX)GetDlgItem("Chk_Forcequit");
	m_pChk_StayOnline = (PAUICHECKBOX)GetDlgItem("Chk_stayon"); // Alien - adicionado
	return true;
}

void CDlgExit::OnCommand_confirm(const char * szCommand)
{
	if( m_pChk_Forcequit->IsChecked() || !GetGameSession()->IsConnected())
		m_pChk_StayOnline->IsChecked() /*Alien adicionado*/ ? GetGameRun()->PostMessage((DWORD)GetGame()->GetGameInit().hWnd, WM_QUIT, 0, 0) /*Alien adicionado*/ : GetGameRun()->PostMessage(MSG_EXITGAME, -1, 0);		
	else
		GetGameRun()->GetPendingLogOut().AppendForSaveConfig(new CECPendingLogoutFull());

	Show(false);
}
