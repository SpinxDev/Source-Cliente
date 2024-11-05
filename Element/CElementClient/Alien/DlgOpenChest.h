#pragma once


#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "EC_TimeSafeChecker.h"

class CDlgOpenChest : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_EVENT_MAP()

public:
	CDlgOpenChest();
	virtual ~CDlgOpenChest();

	void OnCommand_set(const char * szCommand);
	void OnCommand_add(const char * szCommand);
	void OnCommand_minus(const char * szCommand);
	void OnCommand_max(const char * szCommand);
	void OnCommand_begin(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	bool IsProducing(){return m_pPrgs_1 ? (m_pPrgs_1->GetData() > 0) : false;};
	void OnProduceSucc();
	void OnProduceEnd();
	void SetMaterial(int iSrc, int iDst);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	int m_nCurShopSet;
	int m_nCurThing;

	PAUIPROGRESS m_pPrgs_1;
	PAUIOBJECT m_pTxt_Number;
	PAUIOBJECT m_pTxt_Gold;
	PAUIOBJECT m_pTxt_out;
	PAUIIMAGEPICTURE m_pItem_out;
	PAUISTILLIMAGEBUTTON m_pBtn_Produce;
	PAUILABEL m_pTxt_CurrentSkillName;
	PAUILABEL m_pTxt_CurrentSkillLevel;
	PAUIPROGRESS m_pPrgs_Level;
	PAUILABEL m_pTxt_SkillLevel;
	PAUILABEL m_pTxt_SkillName;
	PAUILABEL m_pTxt_Bind;
	PAUISTILLIMAGEBUTTON m_pBtnSet[NUM_NPCIVTR];
	PAUIIMAGEPICTURE m_pImgU[IVTRSIZE_NPCPACK];
	PAUIIMAGEPICTURE m_pImgItem[32];
	PAUIOBJECT m_pTxt_no[32];
	int m_nSoltMaterail[32];
	int m_nRecipeIndexMaterail[32];
	ACString m_Confirm;

	int m_iAutoProduceCount;	// 剩余的合成次数
	bool m_bProduceNext;		// 是否合成下一个，合成失败的话会停止剩余的操作

	CECTimeSafeChecker m_produceTimer;

	virtual bool OnInitDialog();
	virtual bool Tick();

};
