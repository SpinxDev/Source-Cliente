#include "DlgAstrolabe.h"
#include "DlgBase.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_UIHelper.h"
#include "EC_HostPlayer.h"
#include "EC_FixedMsg.h"
#include "elementdataman.h"
#include "EC_Resource.h"
#include "EC_GameSession.h"

AUI_BEGIN_COMMAND_MAP(CDlgAstrolabe, CDlgBase)
AUI_ON_COMMAND("Btn_Swallow",	OnCommandSwallow)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAstrolabe, CDlgBase)

AUI_ON_EVENT("Btn_Point*",	WM_LBUTTONDOWN,	OnEventLButtonDown_PointClick)
AUI_ON_EVENT("Txt_AttName*", WM_MOUSEMOVE, OnEventMouseHover_Addons)

AUI_END_EVENT_MAP()

CDlgAstrolabe::CDlgAstrolabe()
{
	m_pProgLevel = NULL;
	m_pProgVIPLevel = NULL;
	m_pTxtLevel = NULL;
	m_pTxtVIPLevel = NULL;
	m_pTxtPointsValue = NULL;
	m_bSwallowing = false;
	memset(m_pTxtPointValue, 0, sizeof(m_pTxtPointValue));
	memset(m_pTxtAttName, 0, sizeof(m_pTxtAttName));
	memset(m_pTxtAttValue, 0, sizeof(m_pTxtAttValue));
	memset(m_pBtnPoint, 0, sizeof(m_pBtnPoint));
}

CDlgAstrolabe::~CDlgAstrolabe()
{
}

bool CDlgAstrolabe::OnInitDialog()
{
	DDX_Control("Prog_Level", m_pProgLevel);
	DDX_Control("Prog_VIPLevel", m_pProgVIPLevel);
	DDX_Control("Txt_Level", m_pTxtLevel);
	DDX_Control("Txt_VIPLevel", m_pTxtVIPLevel);
	DDX_Control("Txt_PointsValue", m_pTxtPointsValue);
	for (int i = 0; i < 10; i++)
	{
		AString str;
		str.Format("Txt_PointValue%02d", i);
		DDX_Control(str, m_pTxtPointValue[i]);
		m_pTxtPointValue[i]->SetText(L"1");
		str.Format("Txt_AttName%02d", i + 1);
		DDX_Control(str, m_pTxtAttName[i]);
		m_pTxtAttName[i]->Show(false);
		str.Format("Txt_AttValue%02d", i + 1);
		DDX_Control(str, m_pTxtAttValue[i]);
		m_pTxtAttValue[i]->Show(false);
		str.Format("Btn_Point%02d", i);
		DDX_Control(str, m_pBtnPoint[i]);
		m_pBtnPoint[i]->Enable(false);
		m_pBtnPoint[i]->SetFlash(false);
	}

	return true;
}

AWString CDlgAstrolabe::GetAddonName(CECIvtrEquip::PROPERTY prop)
{
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(prop.iType);
	BYTE byPropType = Pair.second ? *Pair.first : 0xff;
	AWString str;
	int p0 = prop.aParams[0];
	switch (byPropType)
	{
		case 167:
			str.Format(L"%ws", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_GOLDDEFENCE));
			break;	
			
		case 168:
			str.Format(L"%ws", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_WOODDEFENCE));			
			break;
			
		case 169:
			str.Format(L"%ws", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_WATERDEFENCE));			
			break;
			
		case 170:			
			str.Format(L"%ws", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_FIREDEFENCE));
			break;
			
		case 171:
			str.Format(L"%ws", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_EARTHDEFENCE));	
			break;
	}
	return str;	
}

void CDlgAstrolabe::Refresh()
{
	DATA_TYPE dt;
	ASTROLABE_LEVELEXP_CONFIG *config = (ASTROLABE_LEVELEXP_CONFIG*)GetElementDataMan()->get_data_ptr(2014, ID_SPACE_CONFIG, dt);	
	if (!config && dt != DT_ASTROLABE_LEVELEXP_CONFIG)
		return;
	ASTROLABE_ADDON_RANDOM_CONFIG *config2 = (ASTROLABE_ADDON_RANDOM_CONFIG*)GetElementDataMan()->get_data_ptr(2015, ID_SPACE_CONFIG, dt);	
	if (!config2 && dt != DT_ASTROLABE_ADDON_RANDOM_CONFIG)
		return;
	CECIvtrAstrolabe* pItem = (CECIvtrAstrolabe*)GetHostPlayer()->GetEquipment()->GetItem(EQUIPIVTR_ASTROLABE);
	CECPlayer::AstrolabeInfo info = GetHostPlayer()->GetAstrolabeInfo();
	ACString str;
	int exp_atual = pItem->GetEssence().exp;
	int exp_next = config->exp[exp_atual];
	m_pProgLevel->SetRangeMax(exp_next);
	m_pProgLevel->SetProgress(exp_atual);
	int level = pItem->GetEssence().level;
	int level_next = 0;
	for (int i = 0; i < sizeof(config2->levelup_exp) / sizeof(config2->levelup_exp[0]); i++)
	{
		if (level < config2->levelup_exp[i])
		{
			level_next = config2->levelup_exp[i];
			break;
		}
	}
	float aptit_max = 20;
	str.Format(L"%s %d/%d\r%s %d\r%s", g_pGame->GetItemDesc()->GetWideString(404), exp_atual, exp_next, g_pGame->GetItemDesc()->GetWideString(405), 50, GetStringFromTable(11518));
	m_pProgLevel->SetHint(str);
	str.Format(L"%s %d/%d\r%s %d\r%s", g_pGame->GetItemDesc()->GetWideString(406), level, level_next, g_pGame->GetItemDesc()->GetWideString(407), 10, GetStringFromTable(11519));
	ACString str2;
	str2.Format(GetStringFromTable(16705), level);
	str += str2;
	m_pProgVIPLevel->SetHint(str);
	str.Format(L"%s %.2f\r%s", g_pGame->GetItemDesc()->GetWideString(395), aptit_max, GetStringFromTable(11520));
	m_pTxtPointsValue->SetHint(str);
	astrolabe_essence ess = pItem->GetEssence();
	int sum_aptit = ess.SumAptit();
	str.Format(L"%.2f", sum_aptit);
	m_pTxtPointsValue->SetText(str);
	str.Format(L"%d", info.level);
	m_pTxtLevel->SetText(str);
	str.Format(L"%d", info.exp);
	m_pTxtVIPLevel->SetText(str);
	int menor[5] = { 1, 3, 5, 7, 9 };
	if (pItem)
	{
		for (int i = 0; i < pItem->GetPropertyNum(); i++)
		{			
			m_pTxtAttName[i]->Show(true);
			str.Format(GetStringFromTable(i < 5 ? 11516 : 11517), GetAddonName(pItem->GetProperty(i)));
			m_pTxtAttName[i]->SetText(str);
			m_pTxtAttValue[i]->Show(true);
			str.Format(L" %+d", pItem->GetProperty(i).aParams[0]);
			m_pTxtAttValue[i]->SetText(str);
			m_pBtnPoint[i]->Enable(true);
			m_pBtnPoint[i]->SetFlash(true);
		}
	}
}

void CDlgAstrolabe::OnShowDialog()
{
	Refresh();
}

void CDlgAstrolabe::OnEventLButtonDown_PointClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	
}

void CDlgAstrolabe::OnEventMouseHover_Addons(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int valor5estrelas = 2000;
	int valor10estrelas = 4100;
	ACString str;
	str.Format(L"%s +%d\r%s +%d", GetStringFromTable(11521), valor5estrelas, GetStringFromTable(11522), valor10estrelas);
	pObj->SetHint(str);
}

void CDlgAstrolabe::SetSwallowing(bool swallow)
{
	m_bSwallowing = swallow;
	PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_Swallow"));
	if (pBtn) pBtn->SetPushed(swallow);
}

void CDlgAstrolabe::OnCommandSwallow(const char* szCommand)
{
	PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(szCommand));
	if (pBtn) {
		GetGame()->ChangeCursor(RES_CUR_SWALLOW);
		SetSwallowing(true);
	}
}

bool CDlgAstrolabe::SwallowCard(unsigned char index, unsigned int count, bool bBatchSwallow)
{	
	CECInventory* pInv = GetHostPlayer()->GetPack();
	CECIvtrItem* pItem = (CECIvtrItem*)pInv->GetItem(index);
	if (!pItem) return false;
	OutputDebugStringA("\npItem\n");
	CECIvtrAstrolabe* pSwallow = dynamic_cast<CECIvtrAstrolabe*>(GetHostPlayer()->GetEquipment()->GetItem(EQUIPIVTR_ASTROLABE));
	if (!pSwallow) return false;
	OutputDebugStringA("\npSwallow\n");
	//const astrolabe_essence& card_data = pSwallow->GetEssence();
	
	OutputDebugStringA("\nc2s_SendCmdAstrolabeSwallow\n");
	
	if (m_bSwallowing) {
		OutputDebugStringA("\nc2s_SendCmdAstrolabeSwallow\n");
		c2s_SendCmdAstrolabeSwallow(index, pItem->GetTemplateID(), 0);
		return true;
	}
	return false;
}

void CDlgAstrolabe::AstrolabeOperation(S2C::astrolabe_operate_result res)
{
	Refresh();
}