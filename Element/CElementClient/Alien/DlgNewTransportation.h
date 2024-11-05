#pragma once
#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIScroll.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CDlgNewTransportation : public CDlgBase
{
	friend class CECGameUIMan;
	friend class CECHostPlayer;
	AUI_DECLARE_EVENT_MAP()
public:
	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	int GetMapID(int id);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();


	PAUISTILLIMAGEBUTTON m_pBtn_Close;
	PAUISTILLIMAGEBUTTON m_pBtn[7];
	PAUISTILLIMAGEBUTTON m_pbtn_northLand;
	PAUILABEL m_ptxtLbl_Attention;

};