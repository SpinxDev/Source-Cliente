#include "DlgAutoPolicyMatter.h"
#include "auidialog.h"
#include "CSplit.h"
#include "itemdataman.h"
#include "DlgBase.h"
#include "elementdataman.h"
#include "EC_AutoPolicy.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_ManNPC.h"
#include "EC_World.h"
#include "EC_NPC.h"
#include "EC_ManMatter.h"
#include "EC_Matter.h"
#include "EC_UIHelper.h"
#include "EC_HPWorkTrace.h"
#include "EC_HostPlayer.h"


CDlgAutoPolicyMatter::CDlgAutoPolicyMatter()
{
	m_pTxt_Hint = NULL;
	range = 20.0f;
}

bool CDlgAutoPolicyMatter::OnInitDialog()
{
	DDX_Control("Txt_Hint", m_pTxt_Hint);
	m_pTxt_Hint->SetText(_AL("Autocoleta..."));
	return true;
}

void CDlgAutoPolicyMatter::OnShowDialog()
{
	
}

void CDlgAutoPolicyMatter::OnTick()
{
	int gather = 0;
	for(int i = 0; i < list_matters.GetSize(); i++)
	{
		CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
		CECMatter* pMatter = pMatterMan->FindMatterNearHost(range, true);
		if (pMatter->GetTemplateID() == list_matters[i])
			continue;

		if (GetHostPlayer()->GetWorkMan()->IsPickingUp())
			return;
		if (GetHostPlayer()->IsGathering())
		{
			return;
		}
		if (GetHostPlayer()->PickupObject(pMatter->GetMatterID(), pMatter->IsMine()))
		{
		}
	}
}