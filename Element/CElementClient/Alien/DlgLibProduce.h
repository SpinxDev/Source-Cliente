#pragma once
#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "EC_TimeSafeChecker.h"
#include "ExpTypes.h"
#include "DlgLibProduceRelationship.h"

#define CDLGLIBPRODUCE_MAX_MATERIALS	24

class CDlgLibProduce :  public CDlgBase
{
	friend class CECGameUIMan;
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgLibProduce();
	virtual ~CDlgLibProduce();

	void OnCommand_begin(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void RandomItems();

	void OnEventLButtonDown_U(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Relationship(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	bool IsProducing(){return m_pPrgs_1 ? (m_pPrgs_1->GetData() > 0) : false;};
	void SetMaterial(int iSrc, int iDst, int count);
	void UpdateItems();

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	virtual void OnMessageBox(AUIDialog* pDlg, int iRetVal);

	PAUISUBDIALOG pSub;

protected:
	PAUIPROGRESS m_pPrgs_1;
	PAUIOBJECT m_pTxt_Number;
	PAUIOBJECT m_pTxt_Gold;
	PAUIOBJECT m_pEdi_SingleCost;
	PAUIOBJECT m_pEdi_AllCost;
	PAUISTILLIMAGEBUTTON m_pBtn_Produce;
	PAUIIMAGEPICTURE m_pImgU[IVTRSIZE_NPCPACK];
	PAUIIMAGEPICTURE m_pImgItem[CDLGLIBPRODUCE_MAX_MATERIALS];
	CECTimeSafeChecker m_produceTimer;
	int m_nMaterialIndex[IVTRSIZE_NPCPACK];
	int m_nMaterialID[IVTRSIZE_NPCPACK];
	AArray<LIB_PRODUCE_RECIPE*> listRecipe;

	virtual bool OnInitDialog();
	virtual bool Tick();
	virtual void OnShowDialog();

};
