#pragma once
#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIScroll.h"
#include "AUIComboBox.h"
#include "AUIEditBox.h"
#include "AUICheckBox.h"
#include "AUIProgress.h"
#include "AUITextArea.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUIRadioButton.h"
#include "auidialog.h"

#include "EC_GameSession.h"
#include "EC_Game.h"
#include "EC_GameRun.h"

class CDlgGuildPlayerSearch : public CDlgBase
{
	friend class CECGameUIMan;

	AUI_DECLARE_EVENT_MAP()

public:
	CDlgGuildPlayerSearch();
	ACString proclaim;
	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnCommand_add(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnCommand_minus(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnCommand_cancel(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

protected:
	bool bContinue;
	int nNum;
	PAUIRADIOBUTTON m_pRdo_Level1;
	PAUIRADIOBUTTON m_pRdo_Level2;
	PAUIRADIOBUTTON m_pRdo_PVE;
	PAUIRADIOBUTTON m_pRdo_PVP;
	PAUICOMBOBOX m_pCombo_Reborn;
	PAUIEDITBOX m_pEdi_Level;
	PAUIOBJECT m_pEdi_Words;

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual bool Render();
};
