#include "DlgLibProduceRelationship.h"
#include "DlgLibProduceRelationshipAwardSubList.h"
#include "DlgLibProduceRelationshipCostSubList.h"
#include "EC_GameUIMan.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIClockIcon.h"
#include "AUISubDialog.h"
#include "AUIDef.h"
#include "AFI.h"

AUI_BEGIN_EVENT_MAP(CDlgLibProduceRelationship, CDlgBase)
AUI_ON_EVENT("*",				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_ON_EVENT(NULL,				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_END_EVENT_MAP()

CDlgLibProduceRelationship::CDlgLibProduceRelationship()
{		
	m_pSubAwardSubList = NULL;
	m_pSubCostSubList = NULL;
}

CDlgLibProduceRelationship::~CDlgLibProduceRelationship()
{		
}

void CDlgLibProduceRelationship::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
}

bool CDlgLibProduceRelationship::OnInitDialog()
{	
	DDX_Control("Sub_Award", m_pSubAward);
	DDX_Control("Sub_Cost", m_pSubCost);
	if (m_pSubAward && m_pSubCost)
		UpdateInfo();
	return true;
}

void CDlgLibProduceRelationship::SetBarSize(int size)
{
	
}

void CDlgLibProduceRelationship::OnShowDialog()
{	
	UpdateInfo();
}

void CDlgLibProduceRelationship::UpdateInfo()
{
	if (!m_pSubAwardSubList)
		m_pSubAwardSubList = (CDlgLibProduceRelationshipAwardSubList*)GetGameUIMan()->GetDialog("Win_ProduceRelationshipAward_Sublist");
	if (m_pSubAwardSubList)
	{
		m_pSubAwardSubList->UpdateInfo();
	}
	if (!m_pSubCostSubList)
		m_pSubCostSubList = (CDlgLibProduceRelationshipCostSubList*)GetGameUIMan()->GetDialog("Win_ProduceRelationshipCost_Sublist");
	if (m_pSubCostSubList)
	{
		m_pSubCostSubList->UpdateInfo();
	}
}