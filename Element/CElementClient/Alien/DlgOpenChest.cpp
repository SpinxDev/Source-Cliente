#include "DlgOpenChest.h"
#include "AFI.h"
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
#include "DlgInputNO.h"
#include "AUIDef.h"

#include "DlgWikiRecipe.h"

#define new A_DEBUG_NEW

AUI_BEGIN_EVENT_MAP(CDlgOpenChest, CDlgBase)

AUI_ON_EVENT("Item_*",		WM_LBUTTONDOWN,		OnEventLButtonDown_Item)
AUI_ON_EVENT("U_*",		WM_LBUTTONDOWN,		OnEventLButtonDown_Item)

AUI_END_EVENT_MAP()

typedef CECShortcutMgr::SimpleClickShortcut<CDlgOpenChest> InstallClickShortcut;

CDlgOpenChest::CDlgOpenChest()
{
	m_nCurShopSet = 1;
	m_iAutoProduceCount = 0;
	m_bProduceNext = false;

	m_produceTimer.Reset();
}

CDlgOpenChest::~CDlgOpenChest()
{
}

bool CDlgOpenChest::OnInitDialog()
{
	int i;
	char szItem[40];
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new InstallClickShortcut(this));
	m_pPrgs_1 = (PAUIPROGRESS)GetDlgItem("Prgs_1");
	m_pTxt_Number = (PAUIOBJECT)GetDlgItem("Txt_Number");
	m_pTxt_Gold = (PAUIOBJECT)GetDlgItem("Txt_Gold");
	m_pBtn_Produce = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Produce");

	for( i = 0; i < IVTRSIZE_NPCPACK; i++ )
	{
		sprintf(szItem, "U_%d", i + 1);
		m_pImgU[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	for( i = 0; i < IVTRSIZE_NPCPACK; i++ )
	{
		sprintf(szItem, "Item_%d", i + 1);
		m_pImgItem[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	return true;
}

void CDlgOpenChest::SetMaterial(int iSrc, int iDst)
{
	char szName[20];
	sprintf(szName, "U_%02d", iDst);
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
	OnEventLButtonDown_Item(0, 0, pImage);

	CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(iSrc);
	if (pItem){
		pImage->SetDataPtr(pItem, "ptr_CECIvtrItem");
		pItem->Freeze(true);
		pImage->SetColor(A3DCOLORRGB(255, 255, 255));
		// Icon
		AUICTranslate trans;
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		pImage->SetHint(trans.Translate(pItem->GetDesc()));
	}
}

bool CDlgOpenChest::Tick()
{
	if (m_produceTimer.TimeLength() > 0 && m_produceTimer.IsTimeArrived())	{
		m_pBtn_Produce->Enable(true);
		m_produceTimer.Reset(0);
	}
	
	return CDlgBase::Tick();
}

void CDlgOpenChest::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{	
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") )
		return;
	
	CECIvtrItem *pItem = (CECIvtrItem*)pObj->GetDataPtr("ptr_CECIvtrItem");
	if( pItem )
	{
		pItem->Freeze(false);
		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
		pImage->SetDataPtr(NULL);
		pImage->SetHint(_AL(""));
		pImage->SetCover(NULL, -1);
	}
}

void CDlgOpenChest::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// only in produce dialog
	if( 0 == stricmp(this->GetName(), "Win_ProduceLibrary") )
	{		
		// find material item in recipe
		if (!pObjOver)
		{
			int i=1;
			while (i<=32)
			{
				AString str;
				str.Format("U_%02d",i);
				pObjOver = GetDlgItem(str);
				 i++;
			}
			
			if(i>32) return;
		}

		// for drag-drop scene
		if( !pObjOver || !strstr(pObjOver->GetName(), "U_") )
		{
			return;
		}

		if( (!pObjOver || AUI_PRESS(VK_MENU)) && pIvtrSrc->GetPileLimit() > 1 )
		{
			GetGameUIMan()->InvokeNumberDialog(pObjSrc, NULL, CDlgInputNO::INPUTNO_SELL_ADD, pIvtrSrc->GetCount());
			
		}
		int iDst = atoi(pObjOver->GetName() + strlen("U_"));
		ACString str;
		str.Format(_AL("%d - %d"), iSrc, iDst);
		GetGameRun()->AddChatMessage(str, 0);
		SetMaterial(iSrc, iDst);
		GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	}
}
