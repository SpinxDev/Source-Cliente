#pragma once
#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "exptypes.h"
#include "EC_IvtrItem.h"
#include "EC_GameUIMan.h"

class CDlgLibProduceRelationshipAwardSub :
	public CDlgBase
{
public:
	CDlgLibProduceRelationshipAwardSub(void);
	~CDlgLibProduceRelationshipAwardSub(void);

	void SetItem(int id, float prob);
	PAUIOBJECT m_pTxt_Probability;
	PAUIOBJECT m_pTxt_Name;
	PAUIIMAGEPICTURE m_pImg_Goods;
};
