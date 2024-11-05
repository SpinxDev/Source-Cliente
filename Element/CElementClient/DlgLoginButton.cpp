// Filename	: DlgLoginButton.cpp
// Creator	: Xu Wenbin
// Date		: 2013/9/5

#include "DlgLoginButton.h"
#include "EC_LoginUIMan.h"

#ifdef ARC
#include "Arc/Asia/EC_ArcAsia.h"
#endif

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgLoginButton, CDlgTheme)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("link",			OnCommandLink)
AUI_ON_COMMAND("changeserver",	OnCommandChangeServer)
AUI_END_COMMAND_MAP()

CDlgLoginButton::CDlgLoginButton()
{
}

void CDlgLoginButton::OnCommandCancel(const char* szCommand)
{
	PostQuitMessage(0);
}

void CDlgLoginButton::OnCommandChangeServer(const char* szCommand)
{
	m_pAUIManager->GetDialog("Win_LoginServerList")->Show(true);
}

void CDlgLoginButton::OnCommandLink(const char* szCommand)
{
	AString strURL = GetBaseUIMan()->GetURL("URL", "Register");
	CECBaseUIMan *pBaseUIMan = GetBaseUIMan();
#ifdef ARC
	if (CC_SDK::ArcOverlay::Instance().IsLoaded()){
		pBaseUIMan->NavigateURL(strURL, NULL, true);
		return;
	}
#endif
	pBaseUIMan->NavigateURL(strURL);
}
