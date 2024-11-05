// PageActionCombo.cpp : implementation file
//

#include "Global.h"
#include "MainFrm.h"
#include "Model.h"
#include "SkeletonEditorDoc.h"
#include "PageActionCombo.h"
#include "A3D.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CPageActionCombo dialog


CPageActionCombo::CPageActionCombo(CWnd* pParent /*=NULL*/)
	: CDialog(CPageActionCombo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageActionCombo)
	m_strWeight1 = _T("");
	m_strWeight2 = _T("");
	m_strWeight3 = _T("");
	//}}AFX_DATA_INIT

	m_pModel = NULL;
	m_bBlendPlaying	= false;
	m_bGroupPlaying	= false;
}


void CPageActionCombo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageActionCombo)
	DDX_Control(pDX, IDC_COMBO_ACTION_A, m_ActionComboA);
	DDX_Control(pDX, IDC_LIST_ACTIONCANDIDATE, m_GroupList);
	DDX_Control(pDX, IDC_SLIDER_WEIGHT3, m_WeightSlider3);
	DDX_Control(pDX, IDC_SLIDER_WEIGHT2, m_WeightSlider2);
	DDX_Control(pDX, IDC_SLIDER_WEIGHT1, m_WeightSlider1);
	DDX_Control(pDX, IDC_COMBO_ACTION3, m_ActionCombo3);
	DDX_Control(pDX, IDC_COMBO_ACTION2, m_ActionCombo2);
	DDX_Control(pDX, IDC_COMBO_ACTION1, m_ActionCombo1);
	DDX_Text(pDX, IDC_STATIC_WEIGHT1, m_strWeight1);
	DDX_Text(pDX, IDC_STATIC_WEIGHT2, m_strWeight2);
	DDX_Text(pDX, IDC_STATIC_WEIGHT3, m_strWeight3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPageActionCombo, CDialog)
	//{{AFX_MSG_MAP(CPageActionCombo)
	ON_BN_CLICKED(IDC_BTN_PLAY, OnBtnPlay)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_ADDACTION, OnBtnAddaction)
	ON_BN_CLICKED(IDC_BTN_REMOVEACTION, OnBtnRemoveaction)
	ON_BN_CLICKED(IDC_BTN_MOVEUP, OnBtnMoveup)
	ON_BN_CLICKED(IDC_BTN_MOVEDOWN, OnBtnMovedown)
	ON_BN_CLICKED(IDC_BTN_GROUPPLAY, OnBtnGroupplay)
	ON_BN_CLICKED(IDC_BTN_JUMPTONEXT, OnBtnJumptonext)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_ACTIVATEPAGE, OnActivatePage)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageActionCombo message handlers

BOOL CPageActionCombo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_WeightSlider1.SetRange(0, 100);
	m_WeightSlider2.SetRange(0, 100);
	m_WeightSlider3.SetRange(0, 100);
	m_WeightSlider1.SetPageSize(10);
	m_WeightSlider2.SetPageSize(10);
	m_WeightSlider3.SetPageSize(10);

	m_strWeight1 = "0.00";
	m_strWeight2 = "0.00";
	m_strWeight3 = "0.00";

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	On activate / disactivate this page
void CPageActionCombo::OnActivatePage(WPARAM wParam, LPARAM lParam)
{
	if (wParam)	//	Activate this page
	{
		UpdateActions(false);

		if (m_pModel)
		{
			A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();
			pA3DModel->StopAllActions();
		}
	}
	else	//	Disactivate this page
	{
		if (m_bBlendPlaying)
			OnBtnPlay();

		if (m_bGroupPlaying)
			OnBtnGroupplay();
	}
}

//	Update action list
void CPageActionCombo::UpdateActions(bool bModelChange)
{
	if (!m_pModel)
		return;

	//	Record selected selection
	CString str1="", str2="", str3="";

	if (!bModelChange)
	{
		int iSel = m_ActionCombo1.GetCurSel();
		if (iSel != CB_ERR)
			m_ActionCombo1.GetLBText(iSel, str1);

		iSel = m_ActionCombo2.GetCurSel();
		if (iSel != CB_ERR)
			m_ActionCombo2.GetLBText(iSel, str2);

		iSel = m_ActionCombo3.GetCurSel();
		if (iSel != CB_ERR)
			m_ActionCombo3.GetLBText(iSel, str3);
	}

	m_ActionCombo1.ResetContent();
	m_ActionCombo2.ResetContent();
	m_ActionCombo3.ResetContent();
	m_ActionComboA.ResetContent();

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();

	A3DSkinModelActionCore* pAction = pA3DModel->GetFirstAction();
	while (pAction)
	{
		int iItem;

		iItem = m_ActionCombo1.AddString(pAction->GetName());
		m_ActionCombo1.SetItemData(iItem, (DWORD)pAction);

		iItem = m_ActionCombo2.AddString(pAction->GetName());
		m_ActionCombo2.SetItemData(iItem, (DWORD)pAction);

		iItem = m_ActionCombo3.AddString(pAction->GetName());
		m_ActionCombo3.SetItemData(iItem, (DWORD)pAction);

		iItem = m_ActionComboA.AddString(pAction->GetName());
		m_ActionComboA.SetItemData(iItem, (DWORD)pAction);

		pAction = pA3DModel->GetNextAction();
	}

	if (pA3DModel->GetActionNum())
	{
		m_ActionComboA.SetCurSel(0);

		if (bModelChange)
		{
			//	Select the first action
			m_ActionCombo1.SetCurSel(0);
			m_ActionCombo2.SetCurSel(0);
			m_ActionCombo3.SetCurSel(0);
		}
		else
		{
			//	Restore selection
			if (str1 != "")
			{
				int iSel = m_ActionCombo1.FindString(-1, str1);
				if (iSel == CB_ERR)
					iSel = 0;

				m_ActionCombo1.SetCurSel(iSel);
			}

			if (str2 != "")
			{
				int iSel = m_ActionCombo2.FindString(-1, str2);
				if (iSel == CB_ERR)
					iSel = 0;

				m_ActionCombo2.SetCurSel(iSel);
			}

			if (str3 != "")
			{
				int iSel = m_ActionCombo3.FindString(-1, str3);
				if (iSel == CB_ERR)
					iSel = 0;

				m_ActionCombo3.SetCurSel(iSel);
			}
		}
	}
}

//	Release resources
void CPageActionCombo::Release()
{
}

//	Bind model object with this dialog
bool CPageActionCombo::BindModel(CModel* pModel)
{
	//	Clear old information
	if (m_pModel)
		ClearAllInfo();

	if (!pModel)
		return true;

	m_pModel = pModel;

	//	Reset action lists
	UpdateActions(true);

	return true;
}

//	Clear all information in dialog
void CPageActionCombo::ClearAllInfo()
{
	m_ActionCombo1.ResetContent();
	m_ActionCombo2.ResetContent();
	m_ActionCombo3.ResetContent();
	m_ActionComboA.ResetContent();
	m_GroupList.ResetContent();

	m_WeightSlider1.SetPos(0);
	m_WeightSlider2.SetPos(0);
	m_WeightSlider3.SetPos(0);

	m_strWeight1 = "0.00";
	m_strWeight2 = "0.00";
	m_strWeight3 = "0.00";

	UpdateData(FALSE);
}

void CPageActionCombo::OnBtnPlay() 
{
	if (m_bBlendPlaying)
	{
		StopBlendActions();
		GetDlgItem(IDC_BTN_PLAY)->SetWindowText("²¥·Å");
	}
	else
	{
		//	Stop group actions
		if (m_bGroupPlaying)
			OnBtnGroupplay();

		PlayBlendActions();
		GetDlgItem(IDC_BTN_PLAY)->SetWindowText("Í£Ö¹");
	}

	m_bBlendPlaying = !m_bBlendPlaying;
}

//	Play blended actions
void CPageActionCombo::PlayBlendActions()
{
	if (!m_pModel)
		return;

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();
	if (!pA3DModel->GetActionNum())
		return;

	//	Stop current playing
	pA3DModel->StopAllActions();

	CString strAction;

	int iPos = m_WeightSlider1.GetPos();
	if (iPos)
	{
		int iSel = m_ActionCombo1.GetCurSel();
		if (iSel != CB_ERR)
		{
			m_ActionCombo1.GetLBText(iSel, strAction);
			pA3DModel->PlayActionByName(strAction, iPos * 0.01f);
		}
	}

	iPos = m_WeightSlider2.GetPos();
	if (iPos)
	{
		int iSel = m_ActionCombo2.GetCurSel();
		if (iSel != CB_ERR)
		{
			m_ActionCombo2.GetLBText(iSel, strAction);
			pA3DModel->PlayActionByName(strAction, iPos * 0.01f);
		}
	}

	iPos = m_WeightSlider3.GetPos();
	if (iPos)
	{
		int iSel = m_ActionCombo3.GetCurSel();
		if (iSel != CB_ERR)
		{
			m_ActionCombo3.GetLBText(iSel, strAction);
			pA3DModel->PlayActionByName(strAction, iPos * 0.01f);
		}
	}

	//	Disable action list
	m_ActionCombo1.EnableWindow(FALSE);
	m_ActionCombo2.EnableWindow(FALSE);
	m_ActionCombo3.EnableWindow(FALSE);
}

//	Stop blended actions
void CPageActionCombo::StopBlendActions()
{
	if (!m_pModel)
		return;

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();
	pA3DModel->StopAllActions();

	//	Enable action list
	m_ActionCombo1.EnableWindow(TRUE);
	m_ActionCombo2.EnableWindow(TRUE);
	m_ActionCombo3.EnableWindow(TRUE);
}

void CPageActionCombo::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar == (void*)&m_WeightSlider1 ||
		pScrollBar == (void*)&m_WeightSlider2 ||
		pScrollBar == (void*)&m_WeightSlider3)
	{
		float fPos = 0.0f;
	//	A3DSkinModelActionCore* pAction;

		if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK)
			fPos = nPos * 0.01f;
		else
			fPos = ((CSliderCtrl*)pScrollBar)->GetPos() * 0.01f;

		if (pScrollBar == (void*)&m_WeightSlider1)
		{
			m_strWeight1.Format("%.2f", fPos);

			CDataExchange dx(this, FALSE);
			DDX_Text(&dx, IDC_STATIC_WEIGHT1, m_strWeight1);

		//	if ((pAction = GetSelectedAction(&m_ActionCombo1)))
		//		pAction->SetActionWeight(fPos);
		}
		else if (pScrollBar == (void*)&m_WeightSlider2)
		{
			m_strWeight2.Format("%.2f", fPos);

			CDataExchange dx(this, FALSE);
			DDX_Text(&dx, IDC_STATIC_WEIGHT2, m_strWeight2);

		//	if ((pAction = GetSelectedAction(&m_ActionCombo2)))
		//		pAction->SetActionWeight(fPos);
		}
		else if (pScrollBar == (void*)&m_WeightSlider3)
		{
			m_strWeight3.Format("%.2f", fPos);

			CDataExchange dx(this, FALSE);
			DDX_Text(&dx, IDC_STATIC_WEIGHT3, m_strWeight3);

		//	if ((pAction = GetSelectedAction(&m_ActionCombo3)))
		//		pAction->SetActionWeight(fPos);
		}
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//	Get selected action of specified list
A3DSkinModelActionCore* CPageActionCombo::GetSelectedAction(CComboBox* pComboBox)
{
	if (!m_pModel || !pComboBox)
		return NULL;

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();

	int iSel = pComboBox->GetCurSel();
	if (iSel == CB_ERR)
		return NULL;

	A3DSkinModelActionCore* pAction = (A3DSkinModelActionCore*)pComboBox->GetItemData(iSel);

	return pAction;
}

void CPageActionCombo::OnBtnAddaction() 
{
	A3DSkinModelActionCore* pAction = GetSelectedAction(&m_ActionComboA);
	if (!pAction)
		return;

	//	Add this action to group list
	int iItem = m_GroupList.InsertString(-1, pAction->GetName());
	m_GroupList.SetItemData(iItem, (DWORD)pAction);
}

void CPageActionCombo::OnBtnRemoveaction() 
{
	int iSel = m_GroupList.GetCurSel();
	if (iSel == LB_ERR)
		return;

	//	Remove selected action
	m_GroupList.DeleteString(iSel);

	//	If there is following item, select it
	if (iSel < m_GroupList.GetCount())
		m_GroupList.SetCurSel(iSel);
}

void CPageActionCombo::OnBtnMoveup() 
{
	int iSel = m_GroupList.GetCurSel();
	if (iSel == LB_ERR || !iSel)
		return;

	A3DSkinModelActionCore* pAction = (A3DSkinModelActionCore*)m_GroupList.GetItemData(iSel);
	m_GroupList.DeleteString(iSel);

	iSel = m_GroupList.InsertString(iSel-1, pAction->GetName());
	m_GroupList.SetItemData(iSel, (DWORD)pAction);
	m_GroupList.SetCurSel(iSel);
}

void CPageActionCombo::OnBtnMovedown() 
{
	int iSel = m_GroupList.GetCurSel();
	if (iSel == LB_ERR || iSel == m_GroupList.GetCount()-1)
		return;

	A3DSkinModelActionCore* pAction = (A3DSkinModelActionCore*)m_GroupList.GetItemData(iSel);
	m_GroupList.DeleteString(iSel);

	iSel = m_GroupList.InsertString(iSel+1, pAction->GetName());
	m_GroupList.SetItemData(iSel, (DWORD)pAction);
	m_GroupList.SetCurSel(iSel);
}

void CPageActionCombo::OnBtnGroupplay() 
{
	if (m_bGroupPlaying)
	{
		StopGroupActions();
		GetDlgItem(IDC_BTN_GROUPPLAY)->SetWindowText("²¥·Å");
	}
	else
	{
		//	Stop blend actions
		if (m_bBlendPlaying)
			OnBtnPlay();

		PlayGroupActions();
		GetDlgItem(IDC_BTN_GROUPPLAY)->SetWindowText("Í£Ö¹");
	}

	m_bGroupPlaying = !m_bGroupPlaying;
}

//	Action stop callback
void CPageActionCombo::ActionStopCallback(A3DSkinModelActionCore* pAction)
{
	OnBtnJumptonext();
}

//	Play group actions
void CPageActionCombo::PlayGroupActions()
{
/*	if (!m_pModel || !m_GroupList.GetCount())
		return;

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();

	//	Set stop callback function
	g_ModelSCBData.iType	= SCB_GROUPACTION;
	g_ModelSCBData.dwData	= (DWORD)this;
	pA3DModel->SetActionStopCallback(::ActionStopCallback, 0);

	//	Select the first action and play it
	m_GroupList.SetCurSel(0);
	A3DSkinModelActionCore* pAction = (A3DSkinModelActionCore*)m_GroupList.GetItemData(0);
	pA3DModel->PlayActionByName(pAction->GetName(), 1.0f);

	//	Disable some operations
	GetDlgItem(IDC_BTN_ADDACTION)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_REMOVEACTION)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_MOVEUP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_MOVEDOWN)->EnableWindow(FALSE);	*/
}

//	Stop group actions
void CPageActionCombo::StopGroupActions()
{
/*	if (!m_pModel || !m_GroupList.GetCount())
		return;

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();

	//	Clear stop callback function
	pA3DModel->StopAllActions();
	pA3DModel->SetActionStopCallback(NULL, 0);

	//	Remove selection
	m_GroupList.SetCurSel(-1);

	//	Enable some operations
	GetDlgItem(IDC_BTN_ADDACTION)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_REMOVEACTION)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_MOVEUP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_MOVEDOWN)->EnableWindow(TRUE);	*/
}

void CPageActionCombo::OnBtnJumptonext() 
{
	if (!m_pModel || !m_GroupList.GetCount())
		return;

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();

	int iSel = m_GroupList.GetCurSel();
	if (iSel == m_GroupList.GetCount() - 1)
	{
		//	All actions have been played
		OnBtnGroupplay();
		return;
	}

	m_GroupList.SetCurSel(iSel+1);
	A3DSkinModelActionCore* pAction = (A3DSkinModelActionCore*)m_GroupList.GetItemData(iSel+1);
	pA3DModel->PlayActionByName(pAction->GetName(), 1.0f);
}

BOOL CPageActionCombo::PreTranslateMessage(MSG* pMsg) 
{
	//	Disable 'return' and 'escape' because these keys will close dialog
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
