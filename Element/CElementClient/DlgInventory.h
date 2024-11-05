// Filename	: DlgInventory.h
// Creator	: Tom Zhou
// Date		: October 10, 2005

#pragma once

#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUIScroll.h"
#include "AUICheckBox.h"
#include "EC_ManNPC.h"
#include "EC_NPCServer.h"
#include "EC_GPDataType.h"

#define CECDLGSHOP_PACKMAX 32
#define CECDLGSHOP_PACKLINEMAX 8

class CECIvtrItem;
class CECLoginPlayer;
class CECFashionModel;
class CECComputeMoveParamFunction;
class CDlgInventory : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgInventory();
	virtual ~CDlgInventory();
	virtual bool Tick(void);

	void OnCommand_choosemoney(const char * szCommand);
	void OnCommand_normalitem(const char * szCommand);
	void OnCommand_questitem(const char * szCommand);
	void OnCommand_normalmode(const char * szCommand);
	void OnCommand_wearmode(const char * szCommand);
	void OnCommand_Fashion(const char * szCommand);
	void OnCommand_Try(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_FashionStore(const char * szCommand);
	void OnCommand_arrange(const char *szCommand);
	void OnCommand_meridian(const char *szCommand);
	void OnCommand_FashionShop(const char *szCommand);
	void OnCommand_GeneralCard(const char* szCommand);
	void OnCommand_Booth(const char* szCommand);
	void OnCommand_Storage(const char* szCommand);
	void OnCommand_Mailbox(const char* szCommand);
	void OnCommand_Consignacao(const char* szCommand);
	void OnCommand_Repair(const char* szCommand);
	void OnCommand_Refine(const char* szCommand);
	void OnCommand_MakeSlot(const char* szCommand);
	void OnCommand_InstallGem(const char* szCommand);
	void OnCommand_IncubarPet(const char* szCommand);
	void OnCommand_ItemCreator(const char* szCommand);
	void OnCommand_NoteToMoney(const char* szCommand);
	void OnCommand_MoneyToNote(const char* szCommand);	
	void OnCommand_astrolabe(const char* szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblclkItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblclkEquip(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnEventLButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseWheel_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	int GetShowItem();
	void SetShowItem(int nShowItem);
	DWORD GetCameraParameter()const;

	enum
	{
		INVENTORY_ITEM_NORMAL = 0,
		INVENTORY_ITEM_TASK
	};
	
	bool ValidateEquipmentPassword();
	bool ValidateEquipmentTakeOff(int equipmentPos);
	bool ValidateEquipmentExchange(int iSrc, CECIvtrItem *pItem, int exchangePos);
	bool ValidateEquipmentExchange(int iSrc, CECIvtrItem *pItem);
	bool ValidateEquipmentShortcut(CECIvtrItem *pItem);

	enum NormalItemShowMode
	{
		NORMAL_ITEM_SHOW_MODE_INTEGRATED,  // All normal item is shown in inventory
		NORMAL_ITEM_SHOW_MODE_SEPERATED    // Normal item in expanded space shown in bag
	};
	NormalItemShowMode GetNormalItemShowMode();

	void DropItem(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void DropEquip(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void ExchangeItem(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void ExchangeEquip(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void ExchangeTradeItem(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	void SetBagModeSplit();

protected:
	int m_nShowIvtrItem;
	int	m_nTotalItem;
	CECLoginPlayer	*	m_pPlayer;

	PAUIOBJECT m_pTxt_Gold;
	PAUISTILLIMAGEBUTTON m_pBtn_NormalItem;
	PAUISTILLIMAGEBUTTON m_pBtn_QuestItem;
	PAUISTILLIMAGEBUTTON m_pBtn_NormalMode;
	PAUISTILLIMAGEBUTTON m_pBtn_WearMode;
	PAUICHECKBOX m_pChkWearMode;
	PAUICHECKBOX m_pChkBag;
	PAUISCROLL m_pScl_Item;
	PAUIIMAGEPICTURE m_pImgEquip[SIZE_ALL_EQUIPIVTR];
	PAUIIMAGEPICTURE m_pImgItem[CECDLGSHOP_PACKMAX];
	PAUIOBJECT			m_pBtn_Meridians;
	PAUIOBJECT			m_pBtn_FashionShop;
	PAUILABEL			m_pLblLing;
	PAUILABEL			m_pLblMai;
	// Alien
	PAUIIMAGEPICTURE	m_pImg_Char;

	bool UpdateInventory();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release();
	virtual bool Render();
	virtual void OnTick();
	void ResetPlayer();
	void UpdatePlayer();
	void CreatePlayer();
	void LoadPlayer();

	void CapturePlayerParams(int mouseX, int mouseY);
	
	// for notify user the layout changing is over
	virtual bool OnChangeLayoutBegin();
	virtual void OnChangeLayoutEnd(bool bAllDone);

	CECIvtrItem* GetSelectItem(PAUIOBJECT pObj);
	CECFashionModel* m_pFashionModel;
	CECComputeMoveParamFunction * m_pMoveParamFunction;

private:
	bool ValidateEquipmentPositionChange(int equipmentPos);
	bool IsFashionSlot(int iSlot);
};
