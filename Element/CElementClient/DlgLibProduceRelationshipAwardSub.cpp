#include "DlgLibProduceRelationshipAwardSub.h"

CDlgLibProduceRelationshipAwardSub::CDlgLibProduceRelationshipAwardSub(void)
{
	DDX_Control("Txt_Probability", m_pTxt_Probability);
	DDX_Control("Img_Goods", m_pImg_Goods);
	DDX_Control("Txt_Name", m_pTxt_Name);
}

CDlgLibProduceRelationshipAwardSub::~CDlgLibProduceRelationshipAwardSub(void)
{
}

void CDlgLibProduceRelationshipAwardSub::SetItem(int id, float prob)
{	
	CECIvtrItem *pItem = CECIvtrItem::CreateItem(id, 0, 1, ID_SPACE_ESSENCE);
	if (pItem)
	{
		pItem->GetDetailDataFromLocal();
		AWString str;
		str.Format(L"%f", prob);
		m_pTxt_Probability->SetText(str);
		m_pTxt_Name->SetText(pItem->GetName());
		GetGameUIMan()->SetCover(m_pImg_Goods, pItem->GetIconFile());
		GetGameUIMan()->SetHint(m_pImg_Goods, pItem);
		delete pItem;
	}
}