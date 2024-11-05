#include "DlgLibProduceRelationshipCostSubList.h"
#include "DlgLibProduceRelationshipCostSub.h"
#include "EC_GameUIMan.h"
#include "EC_Algorithm.h"
#include "elementdataman.h"
#include "ExpTypes.h"
#include "AUI.h"

AUI_BEGIN_EVENT_MAP(CDlgLibProduceRelationshipCostSubList, CDlgBase)
AUI_ON_EVENT("*",				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_ON_EVENT(NULL,				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_END_EVENT_MAP()

CDlgLibProduceRelationshipCostSubList::CDlgLibProduceRelationshipCostSubList(void)
{
	m_pCostSub = NULL;
	m_nSubDialog = 0;
}

CDlgLibProduceRelationshipCostSubList::~CDlgLibProduceRelationshipCostSubList(void)
{
}

void CDlgLibProduceRelationshipCostSubList::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	GetParentDlgControl()->OnDlgItemMessage(WM_MOUSEWHEEL, wParam, lParam);
}

bool CDlgLibProduceRelationshipCostSubList::OnInitDialog()
{	
	PAUIOBJECTLISTELEMENT pElement = GetFirstControl();
	while(pElement)
	{
		if (pElement->pThis->GetType() == AUIOBJECT_TYPE_SUBDIALOG)
			m_pCostSub = (PAUISUBDIALOG)pElement->pThis;
		pElement = pElement->pNext;
	}

	m_pCostSub->Show(true);

	PAUIDIALOG pDlg = m_pCostSub->GetSubDialog();
	m_strDialogFile = pDlg->GetFilename();
	m_pCostSub->SetDialog(NULL);
	return true;
}


void CDlgLibProduceRelationshipCostSubList::SetFirstItem(int id, int point)
{
	PAUIDIALOG pDlg = NULL;
	AString strName;
	if (m_strDialogName.IsEmpty())
	{
		pDlg = m_pAUIManager->GetDialog("Win_ProduceRelationshipCost_Sub");
		m_strDialogName = pDlg->GetName();
		strName.Format("%s_%d", m_strDialogName, m_nSubDialog);
		pDlg->SetName(strName);
	}
	else
	{
		strName.Format("%s_%d", m_strDialogName, m_nSubDialog);
		pDlg = GetGameUIMan()->GetDialog(strName);
		if (!pDlg)
		{
			pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_strDialogFile));
			pDlg->SetName(strName);
		}
		else
		{
			ASSERT(pDlg->GetDestroyFlag() == true);
			
			if (pDlg->GetDestroyFlag())
			{
				pDlg->SetDestroyFlag(false);
				pDlg->Show(true);
			}
		}
	}
	pDlg->Resize(m_pAUIManager->GetRect(), m_pAUIManager->GetRect());
	
	m_pCostSub->SetDialog(pDlg);

	CDlgLibProduceRelationshipCostSub *pShopCartSub = dynamic_cast<CDlgLibProduceRelationshipCostSub *>(m_pCostSub->GetSubDialog());
	pShopCartSub->SetItem(id, point);
}

void CDlgLibProduceRelationshipCostSubList::UpdateInfo()
{
	/*Clear();
	LIB_PRODUCE_RECIPE::__materials material;
	int tg = 0;
	DATA_TYPE dt;
	NPC_LIB_PRODUCE_SERVICE *service = (NPC_LIB_PRODUCE_SERVICE*)GetElementDataMan()->get_data_ptr(GetGameUIMan()->m_pCurNPCEssence->id_lib_produce_service, ID_SPACE_ESSENCE, dt);
	if (service && dt == DT_NPC_LIB_PRODUCE_SERVICE)
	{
		for (int i = 0; i < sizeof(service->id_goods) / sizeof(service->id_goods[0]); i++)
		{
			if (service->id_goods[i])
			{
				LIB_PRODUCE_RECIPE *recipe = (LIB_PRODUCE_RECIPE*)GetElementDataMan()->get_data_ptr(service->id_goods[i], ID_SPACE_ESSENCE, dt);
				if (recipe && dt == DT_LIB_PRODUCE_RECIPE)
				{					
					for (int m = 0; m < sizeof(recipe->materials) / sizeof(recipe->materials[0]); m++)
					{					
						if (recipe->materials[m].id)
						{
							if (tg == 0)
								material = recipe->materials[m];
							Append(recipe->materials[m].id, recipe->materials[m].point);
							tg++;
						}
					}
				}
			}
		}
	}*/

	//SetFirstItem(material.id, material.point);
}


void CDlgLibProduceRelationshipCostSubList::OnCommand_CANCEL(const char *szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgLibProduceRelationshipCostSubList::Clear()
{
	bool bUpdated(false);

	PAUISUBDIALOG pParent = GetParentDlgControl();
	if (pParent)
	{
		pParent->SetHBarPos(0);
		pParent->SetVBarPos(0);
	}

	PAUISUBDIALOG pSub = NULL;
	PAUIDIALOG pDlg = NULL;
	PAUIOBJECTLISTELEMENT pElement = GetFirstControl();
	while (pElement)
	{
		PAUIOBJECTLISTELEMENT pNext = GetNextControl(pElement);
		if (pElement->pThis->GetType() == AUIOBJECT_TYPE_SUBDIALOG)
		{
			if (pElement->pThis != m_pCostSub)
			{
				pSub = (PAUISUBDIALOG)pElement->pThis;
				pDlg = pSub->GetSubDialog();
				DeleteControl(pElement->pThis);
				if (pDlg)
				{
					pDlg->SetDestroyFlag(true);
				}

				m_nSubDialog --;

				bUpdated = true;
			}
		}
		pElement = pNext;
	}

	if (bUpdated)
	{
		FitBuildingSize();
	}
}

AString CDlgLibProduceRelationshipCostSubList::MakeSubDialogName(int index)
{
	AString strName;
	strName.Format("%s_%d", m_pCostSub->GetName(), index);
	return strName;
}

PAUISUBDIALOG CDlgLibProduceRelationshipCostSubList::GetSubDialog(int index)
{
	AString strName = MakeSubDialogName(index);
	PAUIOBJECT pObj = GetDlgItem(strName);
	return (PAUISUBDIALOG)pObj;
}

void CDlgLibProduceRelationshipCostSubList::Append(int id, int point)
{
	PAUISUBDIALOG pSubDialog = AppendSubDialog();
	CDlgLibProduceRelationshipCostSub *pShopCartSub = dynamic_cast<CDlgLibProduceRelationshipCostSub *>(pSubDialog->GetSubDialog());
	pShopCartSub->SetItem(id, point);
}

PAUISUBDIALOG CDlgLibProduceRelationshipCostSubList::AppendSubDialog()
{
	PAUISUBDIALOG pSub = (PAUISUBDIALOG)CloneControl(m_pCostSub);
	pSub->Show(true);

	AString strName;
	strName = MakeSubDialogName(m_nSubDialog);
	pSub->SetName((char *)(const char *)strName);

	SIZE s = m_pCostSub->GetDefaultSize();
	pSub->SetDefaultPos(0, m_nSubDialog*s.cy);

	PAUIDIALOG pDlg = NULL;
	if (m_strDialogName.IsEmpty())
	{
		pDlg = m_pAUIManager->GetDialog("Win_ProduceRelationshipCost_Sub");
		m_strDialogName = pDlg->GetName();
		strName.Format("%s_%d", m_strDialogName, m_nSubDialog);
		pDlg->SetName(strName);
	}
	else
	{
		strName.Format("%s_%d", m_strDialogName, m_nSubDialog);
		pDlg = GetGameUIMan()->GetDialog(strName);
		if (!pDlg)
		{
			pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_strDialogFile));
			pDlg->SetName(strName);
		}
		else
		{
			ASSERT(pDlg->GetDestroyFlag() == true);
			
			if (pDlg->GetDestroyFlag())
			{
				pDlg->SetDestroyFlag(false);
				pDlg->Show(true);
			}
		}
	}
	pDlg->Resize(m_pAUIManager->GetRect(), m_pAUIManager->GetRect());
	pSub->SetDialog(pDlg);
	m_nSubDialog ++;
	FitBuildingSize();

	return pSub;
}

void CDlgLibProduceRelationshipCostSubList::FitBuildingSize()
{
	SIZE s = GetDefaultSize();

	SIZE buildSize = m_pCostSub->GetDefaultSize();
	s.cy = max(1, m_nSubDialog) * buildSize.cy;

	SetDefaultSize(s.cx, s.cy);

	PAUISUBDIALOG pParent = GetParentDlgControl();
	if (pParent)
	{
		float fWindowScale = m_pAUIManager->GetWindowScale();
		int cx = (int)(s.cx * fWindowScale + 0.5f);
		int cy = (int)(s.cy * fWindowScale + 0.5f);
		pParent->SetSubDialogOrgSize(cx, cy);
	}
}