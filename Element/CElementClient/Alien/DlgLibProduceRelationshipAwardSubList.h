#ifndef _ELEMENTCLIENT_DLGLIBPRODUCERELATIONSHIPAWARDSUBLIST_H_
#define _ELEMENTCLIENT_DLGLIBPRODUCERELATIONSHIPAWARDSUBLIST_H_
#include "dlgbase.h"
#include "auisubdialog.h"

class CDlgLibProduceRelationshipAwardSub;

class CDlgLibProduceRelationshipAwardSubList :	public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgLibProduceRelationshipAwardSubList(void);
	virtual ~CDlgLibProduceRelationshipAwardSubList(void);

	void OnCommand_CANCEL(const char *szCommand);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	void SetFirstItem(int id, float probability);
	void UpdateInfo();
	void Clear();
	void FitBuildingSize();
protected:
	virtual bool OnInitDialog();

	PAUISUBDIALOG m_pAwardSub;
	int		m_nSubDialog;	
	AString		m_strDialogName;
	AString		m_strDialogFile;

	AString MakeSubDialogName(int index);
	PAUISUBDIALOG GetSubDialog(int index);
	void Append(int id, float prob);
	PAUISUBDIALOG AppendSubDialog();

};
#endif