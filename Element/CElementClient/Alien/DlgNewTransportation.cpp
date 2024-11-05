#include "DlgNewTransportation.h"

#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "DlgWorldMap.h"
#include "EC_ManPlayer.h"
#include "EC_Player.h"
#include "EC_World.h"
#include "EC_Game.h"
#include "EC_ElsePlayer.h"
#include "EC_GameRun.h"
#include "DlgWorldMap.h"
#include "../ccommon/exptypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_EVENT_MAP(CDlgNewTransportation, CDlgBase)

AUI_ON_EVENT("Btn_*",		WM_LBUTTONDOWN,		OnEventLButtonDown_Item)
AUI_ON_EVENT("btn_northLand",		WM_LBUTTONDOWN,		OnEventLButtonDown_Item)

AUI_END_EVENT_MAP()

void CDlgNewTransportation::OnShowDialog()
{
	DDX_Control("Btn_Close", m_pBtn_Close);
	for (int i = 1; i < 8; i++)
	{
		char szItem[40];
		sprintf(szItem, "Btn_%d", i);
		m_pBtn[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(szItem);
		m_pBtn[i]->SetDataPtr((void*)i, "ptr_MapId");
	}
}

bool CDlgNewTransportation::OnInitDialog()
{	
	return true;
}

void CDlgNewTransportation::OnTick()
{
	
}

int CDlgNewTransportation::GetMapID(int id)
{
	switch(id)
	{
	case 1:
		return 1;
	case 2:
		return 161;
	case 3:
		return 137;
	case 4:
		return 121;
	case 5:
		return 122;
	case 6:
		return 163;
	default:
		return 0;
	}
}

void CDlgNewTransportation::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{	
	if( !pObj->GetDataPtr("ptr_MapId") )
		return;

	int id =  (int)pObj->GetDataPtr("ptr_MapId");
	int id2 = GetMapID(id);
	CDlgWorldMap *pMap = (CDlgWorldMap*)GetGameUIMan()->GetDialog("Win_WorldMapTravel");
	if (pMap)
	{
		pMap->BuildTravelMap(DT_TRANSMITSCROLL_ESSENCE, (void*)0, id2);
		pMap->Show(true);
		OnCommand_CANCEL("IDCANCEL");
	}
}