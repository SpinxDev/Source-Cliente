#include "DlgLibProduceRelationshipAwardSubList.h"
#include "DlgLibProduceRelationshipAwardSub.h"
#include "EC_GameUIMan.h"
#include "EC_Algorithm.h"
#include "elementdataman.h"
#include "ExpTypes.h"
#include "AUI.h"

AUI_BEGIN_EVENT_MAP(CDlgLibProduceRelationshipAwardSubList, CDlgBase)
AUI_ON_EVENT("*",				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_ON_EVENT(NULL,				WM_MOUSEWHEEL,	OnMouseWheel)
AUI_END_EVENT_MAP()

CDlgLibProduceRelationshipAwardSubList::CDlgLibProduceRelationshipAwardSubList(void)
{
	m_pAwardSub = NULL;
	m_nSubDialog = 0;
}

CDlgLibProduceRelationshipAwardSubList::~CDlgLibProduceRelationshipAwardSubList(void)
{
}

void CDlgLibProduceRelationshipAwardSubList::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	GetParentDlgControl()->OnDlgItemMessage(WM_MOUSEWHEEL, wParam, lParam);
}

bool CDlgLibProduceRelationshipAwardSubList::OnInitDialog()
{
	OutputDebugStringA("AwardSubList\n");
	DDX_Control("Sub_FoodChoose", m_pAwardSub);
	m_pAwardSub->Show(true);

	PAUIDIALOG pDlg = m_pAwardSub->GetSubDialog();
	m_strDialogFile = pDlg->GetFilename();
	m_pAwardSub->SetDialog(NULL);
	return true;
}


void CDlgLibProduceRelationshipAwardSubList::SetFirstItem(int id, float probability)
{
	PAUIDIALOG pDlg = NULL;
	AString strName;
	if (m_strDialogName.IsEmpty())
	{
		pDlg = m_pAUIManager->GetDialog("Win_ProduceRelationshipAward_Sub");
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
	
	m_pAwardSub->SetDialog(pDlg);

	CDlgLibProduceRelationshipAwardSub *pShopCartSub = dynamic_cast<CDlgLibProduceRelationshipAwardSub *>(m_pAwardSub->GetSubDialog());
	pShopCartSub->SetItem(id, probability);
}

void CDlgLibProduceRelationshipAwardSubList::UpdateInfo()
{
	/*Clear();
	LIB_PRODUCE_RECIPE::__targets target;
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
					for (int m = 0; m < sizeof(recipe->targets) / sizeof(recipe->targets[0]); m++)
					{					
						if (recipe->targets[m].id)
						{
							if (tg == 0)
								target = recipe->targets[m];
							Append(recipe->targets[m].id, recipe->targets[m].probability);
							tg++;
						}
					}
				}
			}
		}
	}

	SetFirstItem(target.id, target.probability);*/
}


void CDlgLibProduceRelationshipAwardSubList::OnCommand_CANCEL(const char *szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgLibProduceRelationshipAwardSubList::Clear()
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
			if (pElement->pThis != m_pAwardSub)
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

AString CDlgLibProduceRelationshipAwardSubList::MakeSubDialogName(int index)
{
	AString strName;
	strName.Format("%s_%d", m_pAwardSub->GetName(), index);
	return strName;
}

PAUISUBDIALOG CDlgLibProduceRelationshipAwardSubList::GetSubDialog(int index)
{
	AString strName = MakeSubDialogName(index);
	PAUIOBJECT pObj = GetDlgItem(strName);
	return (PAUISUBDIALOG)pObj;
}

void CDlgLibProduceRelationshipAwardSubList::Append(int id, float point)
{
	PAUISUBDIALOG pSubDialog = AppendSubDialog();
	CDlgLibProduceRelationshipAwardSub *pShopCartSub = dynamic_cast<CDlgLibProduceRelationshipAwardSub *>(pSubDialog->GetSubDialog());
	pShopCartSub->SetItem(id, point);
}

PAUISUBDIALOG CDlgLibProduceRelationshipAwardSubList::AppendSubDialog()
{
	PAUISUBDIALOG pSub = (PAUISUBDIALOG)CloneControl(m_pAwardSub);
	pSub->Show(true);

	AString strName;
	strName = MakeSubDialogName(m_nSubDialog);
	pSub->SetName((char *)(const char *)strName);

	SIZE s = m_pAwardSub->GetDefaultSize();
	pSub->SetDefaultPos(0, m_nSubDialog*s.cy);

	PAUIDIALOG pDlg = NULL;
	if (m_strDialogName.IsEmpty())
	{
		pDlg = m_pAUIManager->GetDialog("Win_ProduceRelationshipAward_Sub");
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

void CDlgLibProduceRelationshipAwardSubList::FitBuildingSize()
{
	SIZE s = GetDefaultSize();

	SIZE buildSize = m_pAwardSub->GetDefaultSize();
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