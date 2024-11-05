#include "DlgLibProduce.h"
#include "AFI.h"
#include "DlgEquipUp.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Inventory.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrMaterial.h"
#include "EC_Skill.h"
#include "AUICTranslate.h"
#include "elementdataman.h"
#include "EC_ShortcutMgr.h"
#include "DlgPreviewProduce.h"
#include "DlgWikiRecipe.h"
#include "DlgAutoHelp.h"
#include "DlgInputNO.h"
#include "DlgLibProduceRelationship.h"
#include "itemdataman.h"

AUI_BEGIN_COMMAND_MAP(CDlgLibProduce, CDlgBase)

AUI_ON_COMMAND("begin",			OnCommand_begin)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgLibProduce, CDlgBase)

AUI_ON_EVENT("U_*",			WM_LBUTTONDBLCLK,		OnEventLButtonDown_U)
AUI_ON_EVENT("U_*",			WM_RBUTTONDOWN,			OnEventLButtonDown_U)
AUI_ON_EVENT("Item_*",		WM_LBUTTONDOWN,			OnEventLButtonDown_Item)
AUI_ON_EVENT("Btn_Relationship",	WM_LBUTTONDOWN,	OnEventLButtonDown_Relationship)

AUI_END_EVENT_MAP()

typedef CECShortcutMgr::SimpleClickShortcut<CDlgLibProduce> InstallClickShortcut;

CDlgLibProduce::CDlgLibProduce()
{
}

CDlgLibProduce::~CDlgLibProduce()
{

}


void CDlgLibProduce::OnShowDialog()
{
	int points = 0;
	int money = 0;
	NPC_LIB_PRODUCE_SERVICE *essence = (NPC_LIB_PRODUCE_SERVICE*)GetDataPtr("ptr_NPC_LIB_PRODUCE_SERVICE");
	if (essence)
	{
		for(int j = 0; j < IVTRSIZE_NPCPACK; j++)
		{
			if (m_pImgU[j])
			{
				m_pImgU[j]->SetDataPtr(NULL);
				m_pImgU[j]->SetData(0);
				m_pImgU[j]->ClearCover();
				m_pImgU[j]->SetHint(_AL(""));
				m_pImgU[j]->SetText(_AL(""));
			}
			m_nMaterialID[j] = -1;
			m_nMaterialIndex[j] = -1;
		}
		for(int j = 0; j < CDLGLIBPRODUCE_MAX_MATERIALS; j++)
		{
			if (m_pImgItem[j])
			{
				m_pImgItem[j]->SetDataPtr(NULL);
				m_pImgItem[j]->SetData(0);
				m_pImgItem[j]->ClearCover();
				m_pImgItem[j]->SetHint(_AL(""));
				m_pImgItem[j]->SetText(_AL(""));
			}
		}	

		for (int i = 0; i < sizeof(essence->id_goods) / sizeof(essence->id_goods[0]); i++)
		{
			if (essence->id_goods[i])
			{
				/*DATA_TYPE dt;
				LIB_PRODUCE_RECIPE* recipe = (LIB_PRODUCE_RECIPE*)GetElementDataMan()->get_data_ptr(essence->id_goods[i], ID_SPACE_ESSENCE, dt);	
				if (recipe && dt == DT_LIB_PRODUCE_RECIPE)
				{
					points += recipe->produce_consume_point;
					money += recipe->produce_consume_money;
					listRecipe.Add(recipe);
				}	*/				
			}
		}
		
		GetGameUIMan()->CreateDlg(g_pGame->GetA3DEngine(), g_pGame->GetA3DDevice(), "produce_relationship.xml");
		GetGameUIMan()->m_pDlgLibProduceRelationship = dynamic_cast<CDlgLibProduceRelationship*>(GetGameUIMan()->GetDialog("Win_ProduceRelationship"));
		AWString str;
		m_pTxt_Number->SetText(L"0");
		str.Format(L"%d", money);
		m_pTxt_Gold->SetText(str);
		str.Format(L"%d", points);
		m_pEdi_SingleCost->SetText(str);
		m_pEdi_AllCost->SetText(L"0");
	}
	UpdateItems();
}

bool CDlgLibProduce::OnInitDialog()
{
	m_pPrgs_1 = (PAUIPROGRESS)GetDlgItem("Prgs_1");
	m_pTxt_Number = (PAUIOBJECT)GetDlgItem("Txt_Number");
	m_pTxt_Gold = (PAUIOBJECT)GetDlgItem("Txt_Gold");	
	m_pEdi_SingleCost = (PAUIOBJECT)GetDlgItem("Edi_SingleCost");
	m_pEdi_AllCost = (PAUIOBJECT)GetDlgItem("Edi_AllCost");
	m_pBtn_Produce = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Produce");	
	int i;
	char szItem[40];
	for( i = 0; i < IVTRSIZE_NPCPACK; i++ )
	{
		sprintf(szItem, "U_%02d", i + 1);
		m_pImgU[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);		
		m_nMaterialID[i] = -1;
		m_nMaterialIndex[i] = -1;
	}

	for( i = 0; i < CDLGLIBPRODUCE_MAX_MATERIALS; i++ )
	{
		sprintf(szItem, "Item_%d", i + 1);
		m_pImgItem[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}	
	
	return true;
}

void CDlgLibProduce::OnCommand_begin(const char * szCommand)
{
	int msecs = 0;
	int num = 0;
	for(int i = 0; i < listRecipe.GetSize(); i++)
	{
		if (listRecipe.GetAt(i)->id > 0)
		{
			msecs += listRecipe.GetAt(i)->duration;
			num++;
		}
	}
	int fmsec = (msecs / num) * 1000;
	m_pPrgs_1->SetData(fmsec > 0 ? fmsec : 5000);
	m_pPrgs_1->SetAutoProgress(fmsec > 0 ? fmsec : 5000, 0, AUIPROGRESS_MAX);
}

void CDlgLibProduce::RandomItems()
{
	abase::vector<CECIvtrItem*> listItems = abase::vector<CECIvtrItem*>();
	int num = a_atoi(m_pTxt_Number->GetText());
	if (num > 0)
	{
		for (int i = 0; i < num; i++)
		{			
			float fProb = abase::RandUniform();
			bool bGiven = false;
			for(int a = 0; a < listRecipe.GetSize(); a++)
			{
				if (listRecipe.GetAt(a)->id > 0)
				{
					for (int b = 0; b < sizeof(listRecipe.GetAt(a)->targets) / sizeof(listRecipe.GetAt(a)->targets[0]); b++)
					{
						const LIB_PRODUCE_RECIPE::__targets &mat = listRecipe.GetAt(a)->targets[b];
						if (mat.probability > 0.9999f)
						{
							CECIvtrItem *pItem = CECIvtrItem::CreateItem(mat.id, 0, mat.num);
							if (pItem)
							{
								pItem->GetDetailDataFromLocal();
								ACString str;
								str.Format(_AL("[%d] LIB PRODUCE RECIPE CHANCE OF 100%%: [%d] %s"),i, mat.id, pItem->GetName());
								g_pGame->GetGameRun()->AddChatMessage(str, 0);
								listItems.push_back(pItem);
							}
						}
						else if (!bGiven)
						{
							if (fProb <= mat.probability)
							{
								CECIvtrItem *pItem = CECIvtrItem::CreateItem(mat.id, 0, mat.num);if (pItem)
								{
									pItem->GetDetailDataFromLocal();
									ACString str;
									str.Format(_AL("[%d] LIB PRODUCE RECIPE NORMAL: [%d] %s"),i, mat.id, pItem->GetName());
									g_pGame->GetGameRun()->AddChatMessage(str, 0);
									listItems.push_back(pItem);
									bGiven = true;
								}
							}
							else
								fProb -= mat.probability;
						}
					}
				}
			}			
		}
	}
	int idx = 0;
	AUICTranslate trans;
	for (int i = 0; i < listItems.size(); i++)
	{
		CECIvtrItem *pItem = listItems.at(i);
		if (pItem)
		{
			m_pImgItem[idx]->SetData(pItem->GetCount());
			m_pImgItem[idx]->SetDataPtr(pItem, "ptr_CECIvtrItem");
			GetGameUIMan()->SetCover(m_pImgItem[idx], pItem->GetIconFile());
			m_pImgItem[idx]->SetHint(trans.Translate(pItem->GetDesc()));
			ACString s;
			s.Format(L"%d", pItem->GetCount());
			m_pImgItem[idx]->SetText(pItem->GetCount() > 1 ? s : L"");
			delete pItem;
			idx++;
		}
	}
}

void CDlgLibProduce::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetGameUIMan()->EndNPCService();
	GetHostPlayer()->GetPack()->UnfreezeAllItems();
}

void CDlgLibProduce::OnEventLButtonDown_U(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{	
	if( !pObj ) return;
	int j = atoi(pObj->GetName() + strlen("U_")) - 1;
	if (m_nMaterialIndex[j] >= 0){
		if (CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(m_nMaterialIndex[j])){
			if (pItem->GetTemplateID() == m_nMaterialID[j]){
				pItem->Freeze(false);
			}
		}
	}
	pObj->SetDataPtr(NULL);
	pObj->SetData(0);
	((PAUIIMAGEPICTURE)pObj)->ClearCover();
	pObj->SetHint(_AL(""));
	pObj->SetText(_AL(""));
	m_nMaterialID[j] = -1;
	m_nMaterialIndex[j] = -1;
	UpdateItems();
}

void CDlgLibProduce::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	
}

void CDlgLibProduce::OnEventLButtonDown_Relationship(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	NPC_LIB_PRODUCE_SERVICE *essence = (NPC_LIB_PRODUCE_SERVICE*)GetDataPtr("ptr_NPC_LIB_PRODUCE_SERVICE");
	if (GetGameUIMan()->m_pDlgLibProduceRelationship)
	{		
		GetGameUIMan()->m_pDlgLibProduceRelationship->SetDataPtr(essence, "ptr_NPC_LIB_PRODUCE_SERVICE");
		GetGameUIMan()->m_pDlgLibProduceRelationship->Show(true, false);
	}	
}

void CDlgLibProduce::OnItemDragDrop(CECIvtrItem* pItem, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	NPC_LIB_PRODUCE_SERVICE *essence = (NPC_LIB_PRODUCE_SERVICE*)GetDataPtr("ptr_NPC_LIB_PRODUCE_SERVICE");
	bool hasMaterial(false);
	if (essence)
	{
		for (int i = 0; i < sizeof(essence->id_goods) / sizeof(essence->id_goods[0]); i++)
		{
			if (essence->id_goods[i] > 0)
			{
				DATA_TYPE dt;
				LIB_PRODUCE_RECIPE *recipe = (LIB_PRODUCE_RECIPE*)GetElementDataMan()->get_data_ptr(essence->id_goods[i], ID_SPACE_ESSENCE, dt);	
				/*if (dt == DT_LIB_PRODUCE_RECIPE)
					for (int a = 0; a < sizeof(recipe->materials) / sizeof(recipe->materials[0]); a++)
					{
						if (recipe->materials[a].id != pItem->GetTemplateID())
							continue;
						hasMaterial = true;
					}*/
			}
		}
	}
	if (!hasMaterial)
	{
		GetGameUIMan()->MessageBox("",L"Este item nao esta na lista de materiais",MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
	}
	if (pItem && hasMaterial)
	{
		bool bInputMsgBox(false);

		if( pItem->GetPileLimit() > 1 && pItem->GetCount() > 1 )
		{
			if (strstr(pObjSrc->GetName(), "Item_"))
			{
				bInputMsgBox = true;
			}
		}
		ACHAR szText[40];
		if (bInputMsgBox)
		{
			int nInputNOMode = 0;
			CDlgInputNO *pShow = GetGameUIMan()->m_pDlgInputNO;
			PAUIOBJECT pEdit = pShow->GetDlgItem("DEFAULT_Txt_No.");
			int iDst = atoi(pObjOver->GetName() + strlen("U_")) - 1;
			if( strstr(pObjOver->GetName(), "U_") )
			{
				pEdit->SetData(pItem->GetCount());
				a_sprintf(szText, _AL("%d"), 1);
				pEdit->SetText(szText);
				pShow->SetData(iDst);
				pShow->SetDataPtr(pItem,"ptr_CECIvtrItem");
				pShow->SetDataPtr(pObjSrc,"ptr_AUIObject");
				pShow->Show(true, true);
				pShow->SetType(CDlgInputNO::INPUTNO_LIBPRODUCE_ITEM_NUM);
			}
		}
	}
}

void CDlgLibProduce::SetMaterial(int iSrc, int iDst, int count)
{
	char szName[20];
	AUICTranslate trans;
	CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(iSrc);
	if (pItem)
	{
		AString strFile;
		m_nMaterialIndex[iDst] = iSrc;
		m_nMaterialID[iDst] = pItem->GetTemplateID();
		pItem->Freeze(true);
		
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImgU[iDst]->SetData(count);
		m_pImgU[iDst]->SetDataPtr(pItem, "ptr_CECIvtrItem");
		m_pImgU[iDst]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY], GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		m_pImgU[iDst]->SetHint(trans.Translate(pItem->GetDesc()));
		if (count > 0)
		{
			AWString str;
			str.Format(_AL("%d"), count);
			m_pImgU[iDst]->SetText(str);
		}
	}
	UpdateItems();
}

void CDlgLibProduce::UpdateItems()
{
	AWString str;
	int points = 0;
	int money = 0;
	int all = 0;
	int require = a_atoi(m_pEdi_SingleCost->GetText());
	char szName[20];
	for(int i = 0; i < listRecipe.GetSize(); i++)
	{
		if (listRecipe.GetAt(i)->id > 0)
		{
			for (int a = 0; a < sizeof(listRecipe.GetAt(i)->materials) / sizeof(listRecipe.GetAt(i)->materials[0]); a++)
			{
				if (listRecipe.GetAt(i)->materials[a].id > 0)
				{
					for (int b = 0; b < sizeof(m_nMaterialID) / sizeof(m_nMaterialID[0]) ; b++)
					{
						if (m_nMaterialID[b] > 0)
						{
							if (m_nMaterialID[b] == listRecipe.GetAt(i)->materials[a].id)
							{
								points += listRecipe.GetAt(i)->materials[a].point * (int)m_pImgU[b]->GetData();
							}
						}
					}
				}
			}			
		}
	}
	
	int div = points / require;
	int mult = require * div;
	all = points - mult;

	str.Format(L"%d", div);
	m_pTxt_Number->SetText(str);
	str.Format(L"%d", money);
	m_pTxt_Gold->SetText(str);
	
	str.Format(L"%d", all);
	m_pEdi_AllCost->SetText(str);

	if (div > 0)	
		m_pBtn_Produce->Enable(true);
	else 
		m_pBtn_Produce->Enable(false);
}

void CDlgLibProduce::OnMessageBox(AUIDialog* pDlg, int iRetVal)
{

}


bool CDlgLibProduce::Tick()
{
	if (m_produceTimer.TimeLength() > 0 && m_produceTimer.IsTimeArrived())	{
		m_pBtn_Produce->Enable(true);
		m_produceTimer.Reset(0);
	}
	
	if (m_pPrgs_1->GetProgress() == AUIPROGRESS_MAX)
	{		
		RandomItems();
		m_pPrgs_1->SetData(0);
		m_pPrgs_1->SetProgress(0);
		m_pPrgs_1->SetAutoProgress(0, 0, 0);
	}
	
	return CDlgBase::Tick();
}
