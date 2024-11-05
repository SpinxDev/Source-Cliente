// Filename	: DlgAutoPolicy.h
// Creator	: Shizhenhua
// Date		: 2013/8/30

#pragma once

#include "DlgBase.h"
#include <AUIImagePicture.h>
#include <AUIEditBox.h>
#include <AUICheckBox.h>
#include <AUIStillImageButton.h>
#include <AUILabel.h>
#include <AUIComboBox.h>

/*Alien - foi adicionado - Auto Cultivo Aprimorado*/
#include <AUIListBox.h>
#include "EC_AutoPolicy.h"
#include "vector.h"
#include "EC_Matter.h"
/*Ate Aqui*/


class CDlgAutoPolicy : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoPolicy();

	CECMatter* tmpMatter;
	void OnCommand_Add(const char * szCommand);		/*Alien - foi adicionado*/
	void OnCommand_Delete(const char * szCommand);	/*Alien - foi adicionado*/
	void OnCommand_Confirm(const char * szCommand);
	void OnCommand_StartOrStop(const char * szCommand);
	void OnCommand_SetAutoHPMP(const char * szCommand);
	void OnCommand_AutoPickup(const char * szCommand);
	void OnCommand_Switch(const char * szCommand);

	void OnEvent_LButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventLButtonDown_Lst_Item(WPARAM wParam, LPARAM lParam, AUIObject* pObj);	/*Alien - foi adicionado*/
	void DragDropItem(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	void UpdateList();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUIIMAGEPICTURE m_pImg_Skill1;
	PAUIIMAGEPICTURE m_pImg_Skill2;
	PAUIIMAGEPICTURE m_pImg_Skill3;
	PAUIIMAGEPICTURE m_pImg_Skill4;
	PAUIIMAGEPICTURE m_pImg_Skill5;
	PAUIEDITBOX m_pTxt_Interval_Skill2;
	PAUIEDITBOX m_pTxt_Interval_Skill3;
	PAUIEDITBOX m_pTxt_Interval_Skill4;
	PAUIEDITBOX m_pTxt_Interval_Skill5;
	PAUIEDITBOX m_pTxt_Time;
	PAUIEDITBOX m_pTxt_Range;
	PAUICOMBOBOX m_pCmb_AutoPick;
	PAUISTILLIMAGEBUTTON m_pBtn_Switch;
	PAUILABEL m_pLbl_RemainTime;
	/*Alien - foi adicionado*/
	PAUIEDITBOX m_pTxt_MonsterName;
	PAUILISTBOX m_pListMonster;
	PAUILISTBOX m_pListArea;
	CECAutoPolicy::CONFIG cfgData;
	AArray<int> list_monsters;
	AArray<int> list_matters;
	bool bCollect;
	/*Ate Aqui*/

private:
	void SaveConfigData();
	void SetSkillImage(PAUIIMAGEPICTURE pSkillImg, int skill_id, bool bCombo);
	ACString GetRemainTimeTxt();
};