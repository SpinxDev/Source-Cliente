#include "DlgGuildPlayerSearch.h"
#include "gnet.h"
#include "gameclient.h"
#include "EC_GPDataType.h"
#include "EC_Player.h"
#include "factionoprequest.hpp"
#include "EC_HostPlayer.h"
#include "ids.hxx"
#include "factiondata.hxx"
#include "EC_Faction.h"
#include "CSplit.h"


AUI_BEGIN_EVENT_MAP(CDlgGuildPlayerSearch, CDlgBase)

AUI_ON_EVENT("Btn_Publish",		WM_LBUTTONDOWN,		OnEventLButtonDown_Item)
AUI_ON_EVENT("Btn_LevelUP",		WM_LBUTTONDOWN,		OnCommand_add)
AUI_ON_EVENT("Btn_LevelDown",	WM_LBUTTONDOWN,		OnCommand_minus)
AUI_ON_EVENT("Btn_LevelUP",		WM_LBUTTONDBLCLK,		OnCommand_add)
AUI_ON_EVENT("Btn_LevelDown",	WM_LBUTTONDBLCLK,		OnCommand_minus)
AUI_ON_EVENT("Btn_LevelUP",		WM_LBUTTONUP,		OnCommand_cancel)
AUI_ON_EVENT("Btn_LevelDown",	WM_LBUTTONUP,		OnCommand_cancel)

AUI_END_EVENT_MAP()

CDlgGuildPlayerSearch::CDlgGuildPlayerSearch()
{
	proclaim = _AL("");
	m_pRdo_Level1 = NULL;
	m_pRdo_Level2 = NULL;
	m_pRdo_PVE = NULL;
	m_pRdo_PVP = NULL;
	m_pCombo_Reborn = NULL;
	m_pEdi_Level = NULL;
	m_pEdi_Words = NULL;
	nNum = -1;
}

void CDlgGuildPlayerSearch::OnShowDialog()
{			
	CSplit s(proclaim);
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\\g"));
	
	for(int i = 0; i < vec.size(); i++)
	{
		ACString str = vec[i];
		if (str.Left(1)== _AL("5"))
		{
			str.CutLeft(1);
			m_pEdi_Words->SetText(str);
		}
		else if (str.Left(1) == _AL("4"))
		{
			str.CutLeft(1);
			proclaim = str;
		}
		else if (str.Left(1) == _AL("3"))
		{
			str.CutLeft(1);
			m_pEdi_Level->SetText(str);
		}
		else if (str.Left(1) == _AL("2"))
		{
			str.CutLeft(1);
			CheckRadioButton(0, str.ToInt());
		}
		else if (str.Left(1) == _AL("1"))
		{
			str.CutLeft(1);
			CheckRadioButton(1, str.ToInt());
		}
	}
	bContinue = false;
}

bool CDlgGuildPlayerSearch::OnInitDialog()
{		
	DDX_Control("Rdo_Level1",m_pRdo_Level1);
	DDX_Control("Rdo_Level2",m_pRdo_Level2);
	DDX_Control("Rdo_PVE",m_pRdo_PVE);
	DDX_Control("Rdo_PVP",m_pRdo_PVP);
	DDX_Control("Combo_Reborn",m_pCombo_Reborn);
	DDX_Control("Edi_Level",m_pEdi_Level);
	DDX_Control("Edi_Words", m_pEdi_Words);

	PAUILABEL label = (PAUILABEL)GetDlgItem("Txt_Attention1");
	label->Show(false);

	CheckRadioButton(0, 0);
	CheckRadioButton(1, 0);
	bContinue = false;

	m_pEdi_Level->SetText(_AL("1"));

	return true;
}

bool CDlgGuildPlayerSearch::Render()
{
	return CDlgBase::Render();
}

void CDlgGuildPlayerSearch::OnTick()
{
	if (bContinue)
	{
		ACHAR szText[40];
		int nMax = 150;
		int nVal = a_atoi(m_pEdi_Level->GetText());
		if (nNum == 0)
		{
			if( nNum < nMax)
				nVal++;
		}
		else if (nNum == 1)
		{
			if( nNum > 0) 
				nVal--;
		}
		a_sprintf(szText, _AL("%d"), nVal);
		m_pEdi_Level->SetText(szText);
		ChangeFocus(m_pEdi_Level);
	}
}

void CDlgGuildPlayerSearch::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{		
	ACString strText;
	ACString level = m_pEdi_Level->GetText();
	ACString level_type;
	level_type.Format(_AL("%d"), GetCheckedRadioButton(0));
	ACString pvp_type;
	pvp_type.Format(_AL("%d"), GetCheckedRadioButton(1));
	strText.Format(_AL("1%s\\g2%s\\g3%s\\g4%s\\g5%s"),pvp_type, level_type, level, proclaim, m_pEdi_Words->GetText());
	GetGameSession()->faction_change_proclaim(strText);
	m_pEdi_Words->SetText(_AL(""));
	Show(false);
}

void CDlgGuildPlayerSearch::OnCommand_add(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	bContinue = true;
	nNum = 0;
}

void CDlgGuildPlayerSearch::OnCommand_minus(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	bContinue = true;
	nNum = 1;
}

void CDlgGuildPlayerSearch::OnCommand_cancel(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	bContinue = false;
	nNum = -1;
}