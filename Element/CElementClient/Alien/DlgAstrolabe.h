#pragma once
#include "DlgBase.h"
#include "auilabel.h"
#include "auiprogress.h"
#include "auistillimagebutton.h"

#include "EC_IvtrEquip.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_Inventory.h"

class CDlgAstrolabe : public CDlgBase
{
	friend class CECGameUIMan;
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgAstrolabe();
	~CDlgAstrolabe();

	bool SwallowCard(unsigned char index, unsigned int count, bool bBatchSwallow);
	bool IsSwallowing() { return m_bSwallowing; }
	void AstrolabeOperation(S2C::astrolabe_operate_result res);
	void Refresh();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	void OnEventLButtonDown_PointClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMouseHover_Addons(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnCommandSwallow(const char* szCommand);
	AWString GetAddonName(CECIvtrEquip::PROPERTY prop);
	void SetSwallowing(bool swallow);
	

private:
	PAUIPROGRESS	m_pProgLevel; // nivel
	PAUIPROGRESS	m_pProgVIPLevel; // energia astral
	PAUILABEL		m_pTxtLevel;
	PAUILABEL		m_pTxtVIPLevel;
	PAUILABEL		m_pTxtPointsValue;
	PAUILABEL		m_pTxtPointValue[10];
	PAUILABEL		m_pTxtAttName[10];
	PAUILABEL		m_pTxtAttValue[10];
	PAUISTILLIMAGEBUTTON		m_pBtnPoint[10];
	bool m_bSwallowing;
};
