#include "DlgLibProduceRelationshipAwardSub.h"
#include "DlgLibProduceRelationshipAwardSubList.h"
#include "AUI.h"
#include "DlgLibProduceRelationship.h"

AUI_BEGIN_EVENT_MAP(CDlgLibProduceRelationshipAwardSub, CDlgBase)
AUI_ON_EVENT("*",				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_ON_EVENT(NULL,				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_END_EVENT_MAP()

CDlgLibProduceRelationshipAwardSub::CDlgLibProduceRelationshipAwardSub(void)
{
	m_pTxt_Probability = NULL;
	m_pImg_Goods = NULL;
	m_pTxt_Name = NULL;
}

CDlgLibProduceRelationshipAwardSub::~CDlgLibProduceRelationshipAwardSub(void)
{
}

bool CDlgLibProduceRelationshipAwardSub::OnInitDialog()
{
	DDX_Control("Txt_Probability", m_pTxt_Probability);
	DDX_Control("Img_Goods", m_pImg_Goods);
	DDX_Control("Txt_Name", m_pTxt_Name);
	return true;
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

void CDlgLibProduceRelationshipAwardSub::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	CDlgLibProduceRelationshipAwardSubList* p = (CDlgLibProduceRelationshipAwardSubList*)(GetParentDlgControl()->GetParent());
	p->OnMouseWheel(wParam, lParam, pObj);
}