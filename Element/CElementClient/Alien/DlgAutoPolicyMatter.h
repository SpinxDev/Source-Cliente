#pragma once
#include "DlgBase.h"
#include "AUILabel.h"

class CDlgAutoPolicyMatter :	public CDlgBase
{
public:
	CDlgAutoPolicyMatter();

	PAUILABEL m_pTxt_Hint;
	AArray<int> list_matters;
	float range;

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
};
