#include "DlgFortunePallete.h"

#include "EC_GameUIMan.h"
#include "a3dgfxex.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "AFI.h"
#include <cstdlib>
#include <ctime>
#include "EC_GameSession.h"
#include "exptypes.h"

AUI_BEGIN_EVENT_MAP(CDlgFortunePallete, CDlgBase)

AUI_ON_EVENT("Item_*",		WM_LBUTTONDOWN,		OnEventLButtonDown_Item)
AUI_ON_EVENT("Img_Card*",	WM_LBUTTONUP,		OnEventLButtonUp_Img_Card)
AUI_ON_EVENT("Btn_Opne",	WM_LBUTTONUP, OnEventLButtonUp_Opne)

AUI_END_EVENT_MAP()

void CDlgFortunePallete::OnShowDialog()
{
	char szItem[40];
	for (int i = 1; i <= 10; i++)
	{
		sprintf(szItem, "Progress_%d", i);
		m_Progress[i] = (PAUIPROGRESS)GetDlgItem(szItem);
	}

	for( i = 1; i <= 30; i++ )
	{
		sprintf(szItem, "Item_%d", i);
		m_pImgItem[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	for (int i = 0; i < 6; i++)
	{
		sprintf(szItem, "Img_Card%d", i);
		m_pImg_Card[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		m_pImg_Card[i]->Show(false);
		m_pImg_Card[i]->SetData(i);
	}

	for (int i = 0; i < 6; i++)
	{
		sprintf(szItem, "Img_Prize%d", i);
		m_pImg_Prize[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		m_pImg_Prize[i]->Show(false);
	}

	for (int i = 0; i < 6; i++)
	{
		sprintf(szItem, "Gfx_CardBroken%d", i);
		m_pGfx_CardBroken[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		m_pGfx_CardBroken[i]->Show(false);
	}

	for (int i = 0; i < 6; i++)
	{
		sprintf(szItem, "Gfx_CardAward%d", i);
		m_pGfx_CardAward[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		m_pGfx_CardAward[i]->Show(false);
	}

	m_pTxt_KeyNumber = (PAUILABEL)GetDlgItem("Txt_KeyNumber");
	m_pTxt_Point = (PAUILABEL)GetDlgItem("Txt_Point");
	m_pBtn_SoloDraw = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SoloDraw");
	m_pBtn_Opne = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Opne");
	m_pScl_Item = (PAUISCROLL)GetDlgItem("Scl_Item");
	m_pImg_AwardGet = (PAUIIMAGEPICTURE)GetDlgItem("Img_AwardGet");
	m_pImg_AwardGetBG = (PAUIIMAGEPICTURE)GetDlgItem("Img_AwardGetBG");
	m_pTxt_GetAward = (PAUILABEL)GetDlgItem("Txt_GetAward");
	m_pBtn_Batch = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Batch");
	m_pBtn_Cancel = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Cancel");
	m_pBtn_Option = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Option");
	
	
	m_pGfx_Bg = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_Bg");
	m_pGfx_GetAward = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_GetAward");
	m_pGfx_Deal = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_Deal");

	m_pGfx_Bg->Show(false);
	m_pGfx_GetAward->Show(false);
	m_pGfx_Deal->Show(false);
	
}

bool CDlgFortunePallete::OnInitDialog()
{	
	
	return true;
}

bool CDlgFortunePallete::Render()
{
	CECInventory* pPack = GetHostPlayer()->GetPack();
	for( int i=0;i<pPack->GetSize();i++ )
	{
		CECIvtrItem* pItem = pPack->GetItem(i);
		if( !pItem || (pItem->GetTemplateID() != 61729) )
			continue;
		ACString str;
		str.Format(_AL("%d"), pItem->GetCount());
		m_pTxt_KeyNumber->SetText(str);
	}
	return CDlgBase::Render();
}

void CDlgFortunePallete::OnTick()
{
	
}

int CDlgFortunePallete::RandomItem()
{
	int id[6] = {11212, 61729, 61739, 50012, 50013, 50018};
	srand(time(0));
	int indiceAleatorio = rand() % 6;
    int valorAleatorio = id[indiceAleatorio];
	return valorAleatorio;
}

void CDlgFortunePallete::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{	
	
}

void CDlgFortunePallete::OnEventLButtonUp_Opne(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{	
	for (int i = 1; i < 6; i++)
	{
		m_pImg_Card[i]->Show(true);
		m_pGfx_CardAward[i]->Show(true);
		m_pGfx_CardAward[i]->StartGfx();
	}
}

void CDlgFortunePallete::OnEventLButtonUp_Img_Card(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{	
	int idx = pObj->GetData();	
	m_pImg_Card[idx]->Show(false);
	m_pGfx_CardBroken[idx]->Show(true);
	m_pGfx_CardBroken[idx]->StartGfx();
	int rnd = RandomItem();
	AString s;
	s.Format("ID: %d\n", rnd);
	OutputDebugStringA(s);
	int count = 1;
	CECIvtrItem *pItem = CECIvtrItem::CreateItem(rnd, 0, count, ID_SPACE_ESSENCE);
	if(pItem)
	{
		pItem->GetDetailDataFromLocal();	
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_Prize[idx]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		m_pImg_Prize[idx]->Show(true);
		c2s_SendDebugCmd(50000, 3, pItem->GetTemplateID(), pItem->GetCount(), 501350);		
	}	
	delete pItem;
}