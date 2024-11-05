#pragma once
#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "exptypes.h"
#include "EC_IvtrItem.h"
#include "EC_GameUIMan.h"

class CDlgLibProduceRelationshipCostSub :
	public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgLibProduceRelationshipCostSub(void);
	~CDlgLibProduceRelationshipCostSub(void);

	void SetItem(int id, int point);
	PAUIOBJECT m_pTxt_Score;
	PAUIOBJECT m_pTxt_Name;
	PAUIIMAGEPICTURE m_pImg_Goods;
	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:
	virtual bool OnInitDialog();
};
