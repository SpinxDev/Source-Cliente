#include "DlgLibProduceRelationshipCostSub.h"
#include "DlgLibProduceRelationshipCostSubList.h"
#include "AUI.h"

AUI_BEGIN_EVENT_MAP(CDlgLibProduceRelationshipCostSub, CDlgBase)
AUI_ON_EVENT("*",				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_ON_EVENT(NULL,				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_END_EVENT_MAP()

CDlgLibProduceRelationshipCostSub::CDlgLibProduceRelationshipCostSub(void)
{
	m_pTxt_Score = NULL;
	m_pImg_Goods = NULL;
	m_pTxt_Name = NULL;
}

CDlgLibProduceRelationshipCostSub::~CDlgLibProduceRelationshipCostSub(void)
{
}

bool CDlgLibProduceRelationshipCostSub::OnInitDialog()
{
	DDX_Control("Txt_Score", m_pTxt_Score);
	DDX_Control("Img_Goods", m_pImg_Goods);
	DDX_Control("Txt_Name", m_pTxt_Name);	
	if (m_pTxt_Score && m_pImg_Goods && m_pTxt_Name)
		OutputDebugStringA("FOI\n");
	else 
		OutputDebugStringA("NAO FOI\n");
	return true;
}

void CDlgLibProduceRelationshipCostSub::SetItem(int id, int point)
{	
	CECIvtrItem *pItem = CECIvtrItem::CreateItem(id, 0, 1, ID_SPACE_ESSENCE);
	if (pItem)
	{
		pItem->GetDetailDataFromLocal();
		AWString str;
		str.Format(L"%d", point);
		m_pTxt_Score->SetText(str);
		m_pTxt_Name->SetText(pItem->GetName());
		GetGameUIMan()->SetCover(m_pImg_Goods, pItem->GetIconFile());
		GetGameUIMan()->SetHint(m_pImg_Goods, pItem);
		delete pItem;
	}
}

void CDlgLibProduceRelationshipCostSub::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	CDlgLibProduceRelationshipCostSubList* p = (CDlgLibProduceRelationshipCostSubList*)(GetParentDlgControl()->GetParent());
	p->OnMouseWheel(wParam, lParam, pObj);
}
