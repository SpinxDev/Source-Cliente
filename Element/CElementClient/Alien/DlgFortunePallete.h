#pragma once
#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIScroll.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CDlgFortunePallete : public CDlgBase
{
	friend class CECGameUIMan;

	AUI_DECLARE_EVENT_MAP()

public:
	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonUp_Opne(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonUp_Img_Card(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

protected:

	int RandomItem();

	PAUIPROGRESS m_Progress[10];
	PAUIIMAGEPICTURE m_pImgItem[30];
	PAUIIMAGEPICTURE m_pImg_Card[6];
	PAUIIMAGEPICTURE m_pImg_Prize[6];
	PAUIIMAGEPICTURE m_pGfx_CardBroken[6];
	PAUIIMAGEPICTURE m_pGfx_CardAward[6];
	PAUIIMAGEPICTURE m_pGfx_Bg;
	PAUILABEL m_pTxt_KeyNumber;
	PAUILABEL m_pTxt_Point;
	PAUISTILLIMAGEBUTTON m_pBtn_SoloDraw;
	PAUISTILLIMAGEBUTTON m_pBtn_Opne;
	PAUISCROLL m_pScl_Item;
	PAUIIMAGEPICTURE m_pImg_AwardGet;
	PAUIIMAGEPICTURE m_pImg_AwardGetBG;
	PAUIIMAGEPICTURE m_pGfx_GetAward;
	PAUILABEL m_pTxt_GetAward;
	PAUISTILLIMAGEBUTTON m_pBtn_Batch;
	PAUISTILLIMAGEBUTTON m_pBtn_Cancel;
	PAUISTILLIMAGEBUTTON m_pBtn_Option;
	PAUIIMAGEPICTURE m_pGfx_Deal;

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual bool Render();
};
