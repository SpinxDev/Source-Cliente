#pragma once
#include "DlgBase.h"
#include "AUISubDialog.h"

class CDlgLibProduceRelationshipAwardSubList;
class CDlgLibProduceRelationshipCostSubList;

class CDlgLibProduceRelationship : public CDlgBase
{

	AUI_DECLARE_EVENT_MAP();

public:
	CDlgLibProduceRelationship();
	virtual ~CDlgLibProduceRelationship();

	PAUISUBDIALOG	m_pSubAward;
	PAUISUBDIALOG	m_pSubCost;
	void SetBarSize(int size);
	void UpdateInfo();

	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

private:
	CDlgLibProduceRelationshipAwardSubList *m_pSubAwardSubList;
	CDlgLibProduceRelationshipCostSubList *m_pSubCostSubList;

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};
