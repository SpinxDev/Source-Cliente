#ifndef _ELEMENTCLIENT_DLGLIBPRODUCERELATIONSHIPCOSTSUBLIST_H_
#define _ELEMENTCLIENT_DLGLIBPRODUCERELATIONSHIPCOSTSUBLIST_H_
#include "dlgbase.h"
#include "auisubdialog.h"

class CDlgLibProduceRelationshipCostSub;

class CDlgLibProduceRelationshipCostSubList :	public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgLibProduceRelationshipCostSubList(void);
	virtual ~CDlgLibProduceRelationshipCostSubList(void);

	void OnCommand_CANCEL(const char *szCommand);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	void SetFirstItem(int id, int point);
	void UpdateInfo();
	void Clear();
	void FitBuildingSize();
protected:
	virtual bool OnInitDialog();

	PAUISUBDIALOG m_pCostSub;
	int		m_nSubDialog;	
	AString		m_strDialogName;
	AString		m_strDialogFile;

	AString MakeSubDialogName(int index);
	PAUISUBDIALOG GetSubDialog(int index);
	void Append(int id, int point);
	PAUISUBDIALOG AppendSubDialog();

};
#endif