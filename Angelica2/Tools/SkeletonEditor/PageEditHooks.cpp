// PageEditHooks.cpp : implementation file
//

#include "Global.h"
#include "PageEditHooks.h"
#include "Model.h"
#include "Hook.h"
#include "MainFrm.h"
#include "SkeletonEditorDoc.h"
#include "DlgAddHanger.h"
#include "A3D.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CPageEditHooks dialog


CPageEditHooks::CPageEditHooks(CWnd* pParent /*=NULL*/)
	: CDialog(CPageEditHooks::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageEditHooks)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pModel = NULL;
}


void CPageEditHooks::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageEditHooks)
	DDX_Control(pDX, IDC_LIST_HOOKS, m_HookList);
	DDX_Control(pDX, IDC_LIST_HANGERS, m_HangerList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageEditHooks, CDialog)
	//{{AFX_MSG_MAP(CPageEditHooks)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_HOOKS, OnItemchangedListHooks)
	ON_BN_CLICKED(IDC_BTN_ADDHANGER, OnBtnAddhanger)
	ON_BN_CLICKED(IDC_BTN_DELETEHANGER, OnBtnDeletehanger)
	ON_BN_CLICKED(IDC_BTN_STOPHANGER, OnBtnStophanger)
	ON_BN_CLICKED(IDC_BTN_TESTHANGER, OnBtnTesthanger)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_HANGERS, OnDblclkListHangers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageEditHooks message handlers

BOOL CPageEditHooks::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//	Initialize hook list
	m_HookList.InsertColumn(0, "挂点", LVCFMT_LEFT, 400);

	m_HookList.SetExtendedStyle(m_HookList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	//	Initialize hanger list
	m_HangerList.InsertColumn(HSI_BINDTO, "悬挂点", LVCFMT_LEFT, 120);
	m_HangerList.InsertColumn(HSI_NAME, "名称", LVCFMT_LEFT, 80);
	m_HangerList.InsertColumn(HSI_TYPE, "类型", LVCFMT_LEFT, 50);
	m_HangerList.InsertColumn(HSI_SCALE, "缩放", LVCFMT_LEFT, 60);
	m_HangerList.InsertColumn(HSI_OBJECT, "悬挂物", LVCFMT_LEFT, 400);

	m_HangerList.SetExtendedStyle(m_HangerList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	//	Create input window
	if (!m_InputWnd.Create(NULL, NULL, WS_POPUP, CRect(0, 0, 50, 40), this, 0))
		return FALSE;

	//	Create combo window
	if (!m_ComboWnd.Create(NULL, NULL, WS_POPUP, CRect(0, 0, 50, 80), this, 0))
		return FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Release resources
void CPageEditHooks::Release()
{
	ClearAllInfo();

	m_pModel = NULL;
}

//	Bind model object with this dialog
bool CPageEditHooks::BindModel(CModel* pModel)
{
	//	Clear old information
	if (m_pModel)
		ClearAllInfo();

	if (!pModel)
		return true;

	m_pModel = pModel;

	//	Reset hook list
	if (!UpdateHookList())
		return false;

	//	Update hanger list
	if (!UpdateHangerList())
		return false;

	return true;
}

//	Clear all information in dialog
void CPageEditHooks::ClearAllInfo()
{
	m_HookList.DeleteAllItems();
	m_HangerList.DeleteAllItems();
}

//	Reset hook list
bool CPageEditHooks::UpdateHookList()
{
	if (!m_pModel)
		return true;

	m_HookList.DeleteAllItems();

	for (int i=0; i < m_pModel->GetHookNum(); i++)
	{
		CHook* pHook = m_pModel->GetHook(i);
		A3DSkeletonHook* pA3DHook = pHook->GetA3DHook();
		
		//	Hook name
		int iItem = m_HookList.InsertItem(i, pA3DHook->GetName());

		//	Associate hook pointer
		m_HookList.SetItemData(iItem, (DWORD)pHook);

		m_HookList.SetCheck(iItem, pHook->IsVisible() ? TRUE : FALSE);
	}

	return true;
}

//	Check or uncheck a item in hook list
void CPageEditHooks::CheckHookItem(int iItem)
{
	CHook* pHook = (CHook*)m_HookList.GetItemData(iItem);

	if (m_HookList.GetCheck(iItem))
	{
		//	Check item
		pHook->SetVisibleFlag(true);
	}
	else
	{
		//	Uncheck item
		pHook->SetVisibleFlag(false);
	}
}

//	Update hanger list
bool CPageEditHooks::UpdateHangerList()
{
	if (!m_pModel)
		return true;

	AString str;

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();

	for (int i=0; i < pA3DModel->GetHangerNum(); i++)
	{
		A3DHanger* pHanger = pA3DModel->GetHanger(i);

		//	Bind to what
		str = pHanger->GetBindName();
		if (pHanger->IsBoundOnBone())
			str = "[B]" + str;
		else
			str = "[H]" + str;

		int iItem = m_HangerList.InsertItem(i, str);

		//	Associate hanger pointer
		m_HangerList.SetItemData(iItem, (DWORD)pHanger);

		//	Update hanger's information
		UpdateHangerItem(iItem);
	}

	return true;
}

//	Update hanger Item
void CPageEditHooks::UpdateHangerItem(int iItem)
{
	A3DHanger* pHanger = (A3DHanger*)m_HangerList.GetItemData(iItem);

	AString str;

	//	Hanger's name
	m_HangerList.SetItemText(iItem, HSI_NAME, pHanger->GetName());

	//	hanger type
	if (pHanger->GetType() == A3DHanger::TYPE_SKINMODEL)
	{
		A3DSkinModelHanger* pModelHanger = (A3DSkinModelHanger*)pHanger;
		m_HangerList.SetItemText(iItem, HSI_TYPE, "model");
		m_HangerList.SetItemText(iItem, HSI_SCALE, pModelHanger->GetInheritScaleFlag() ? "true" : "false");
		m_HangerList.SetItemText(iItem, HSI_OBJECT, pModelHanger->GetModelFile());
	}
	else if (pHanger->GetType() == A3DHanger::TYPE_GFX)
	{
		A3DGFXHanger* pGFXHanger = (A3DGFXHanger*)pHanger;
		m_HangerList.SetItemText(iItem, HSI_TYPE, "gfx");
		m_HangerList.SetItemText(iItem, HSI_SCALE, pGFXHanger->GetInheritScaleFlag() ? "true" : "false");
		m_HangerList.SetItemText(iItem, HSI_OBJECT, pGFXHanger->GetGFXFile());
	}
}

void CPageEditHooks::OnItemchangedListHooks(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem < 0 || pNMListView->iSubItem != 0)
		return;

	bool bCheck1, bCheck2;

	bCheck1 = (pNMListView->uOldState & INDEXTOSTATEIMAGEMASK(2)) ? true : false;
	bCheck2 = (pNMListView->uNewState & INDEXTOSTATEIMAGEMASK(2)) ? true : false;
	
	if (bCheck1 == bCheck2)
		return;

	CheckHookItem(pNMListView->iItem);
}

void CPageEditHooks::OnBtnAddhanger() 
{
	if (!m_pModel)
		return;

	CDlgAddHanger AddDlg(m_pModel);
	if (AddDlg.DoModal() != IDOK)
		return;

	//	Get hanger type
	int iType = AddDlg.GetHangerType();

	A3DHanger* pHanger = NULL;

	if (iType == A3DHanger::TYPE_GFX)
	{
		return;
	/*	A3DGFXHanger* pGFXHanger = new A3DGFXHanger(m_pModel->GetA3DSkinModel());
		if (!pGFXHanger)
			return;

		pGFXHanger->SetGFXFile(AddDlg.GetFileName());

		pHanger = pGFXHanger;

		pHanger->SetName(AddDlg.GetName());
		pHanger->SetBindInfo(AddDlg.IsOnBone(), AddDlg.GetBindTo());

		if (!pHanger->LoadHangerObject())
		{
			AUX_MessageBox(MB_OK, "加载悬挂物失败 %s。", AddDlg.GetFileName());
			return;
		}

		//	Add hanger to model
		if (!m_pModel->GetA3DSkinModel()->AddHanger(pHanger))
			return;
	*/
	}
	else if (iType == A3DHanger::TYPE_SKINMODEL)
	{
		A3DSkinModelHanger* pModelHanger = m_pModel->GetA3DSkinModel()->AddChildModel(AddDlg.GetName(),
				AddDlg.IsOnBone(), AddDlg.GetBindTo(), AddDlg.GetFileName(), AddDlg.GetCCName());

		if (!pModelHanger)
		{
			AUX_MessageBox(MB_OK, "加载悬挂物失败 %s。", AddDlg.GetFileName());
			return;
		}

		pModelHanger->EnableInheritScale(true);

		pHanger = pModelHanger;
	}

	//	Update hanger list
	int iNumHanger = m_HangerList.GetItemCount();

	//	Bind to what
	AString str = pHanger->GetBindName();
	if (pHanger->IsBoundOnBone())
		str = "[B]" + str;
	else
		str = "[H]" + str;

	int iItem = m_HangerList.InsertItem(iNumHanger, str);

	//	Associate hanger pointer
	m_HangerList.SetItemData(iItem, (DWORD)pHanger);

	//	Update hanger's information
	UpdateHangerItem(iItem);

	AUX_GetMainFrame()->GetDocument()->SetModifiedFlag(TRUE);
}

void CPageEditHooks::OnBtnDeletehanger() 
{
	if (!m_HangerList.GetSelectedCount())
		return;

	if (AUX_MessageBox(MB_YESNO, "确实要删除选中的悬挂物吗？") == IDNO)
		return;

	POSITION pos = m_HangerList.GetFirstSelectedItemPosition();
	int iItem = m_HangerList.GetNextSelectedItem(pos);

	//	Delete hanger
	A3DHanger* pHanger = (A3DHanger*)m_HangerList.GetItemData(iItem);
	if (pHanger->GetType() == A3DHanger::TYPE_SKINMODEL)
	{
		m_pModel->GetA3DSkinModel()->RemoveChildModel(pHanger->GetName());
		pHanger = NULL;
	}

	m_HangerList.DeleteItem(iItem);

	AUX_GetMainFrame()->GetDocument()->SetModifiedFlag(TRUE);
}

void CPageEditHooks::OnBtnStophanger() 
{
	for (int i=0; i < m_HangerList.GetItemCount(); i++)
	{
		A3DHanger* pHanger = (A3DHanger*)m_HangerList.GetItemData(i);

		if (pHanger->GetType() == A3DHanger::TYPE_GFX)
		{
			A3DGraphicsFX* pGFX = ((A3DGFXHanger*)pHanger)->GetGFXObject();
			pGFX->Stop(false);
		}
	}
}

void CPageEditHooks::OnBtnTesthanger() 
{
	if (!m_HangerList.GetSelectedCount())
		return;

	POSITION pos = m_HangerList.GetFirstSelectedItemPosition();
	int iItem = m_HangerList.GetNextSelectedItem(pos);

	A3DHanger* pHanger = (A3DHanger*)m_HangerList.GetItemData(iItem);

	if (pHanger->GetType() == A3DHanger::TYPE_GFX)
	{
		A3DGraphicsFX* pGFX = ((A3DGFXHanger*)pHanger)->GetGFXObject();
		pGFX->Start(false);
	}
}

void CPageEditHooks::OnDblclkListHangers(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NMITEMACTIVATE* pNMListView = (NMITEMACTIVATE*)pNMHDR;
	if (pNMListView->iItem < 0 || 
		(pNMListView->iSubItem != HSI_NAME &&
		pNMListView->iSubItem != HSI_SCALE))
		return;

	if (pNMListView->iSubItem == HSI_NAME)
		EditHangerListItem(pNMListView->iItem, pNMListView->iSubItem);
	else if (pNMListView->iSubItem == HSI_SCALE)
		ComboHangerListItem(pNMListView->iItem, pNMListView->iSubItem);
}

//	Edit a hanger list item
void CPageEditHooks::EditHangerListItem(int iItem, int iSubItem)
{
	if (iItem < 0 || iSubItem != HSI_NAME)
		return;

	//	Get subitem rect
	CRect rc;
	m_HangerList.GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rc);
	m_HangerList.ClientToScreen(&rc);

	CString strText = m_HangerList.GetItemText(iItem, iSubItem);

	m_EditStatus.bEditing	= true;
	m_EditStatus.iWndID		= IDC_LIST_HANGERS;
	m_EditStatus.iItem		= iItem;
	m_EditStatus.iSubItem	= iSubItem;

	//	Popup input window
	m_InputWnd.Popup(IDD_EDITHOOK, CWndInput::DATA_STRING, (void*)(LPCTSTR)strText, rc);
}

//	End editing
void CPageEditHooks::EndEditItem(bool bCancel)
{
	if (!m_EditStatus.bEditing)
		return;

	m_EditStatus.bEditing = false;

	if (bCancel)
		return;

	if (m_EditStatus.iWndID == IDC_LIST_HANGERS)
	{
		A3DHanger* pHanger = (A3DHanger*)m_HangerList.GetItemData(m_EditStatus.iItem);

		CString strResult = m_InputWnd.GetResultString();

		if (m_EditStatus.iSubItem == HSI_NAME)
			pHanger->SetName(strResult);

		m_HangerList.SetItemText(m_EditStatus.iItem, m_EditStatus.iSubItem, strResult);

		AUX_GetMainFrame()->GetDocument()->SetModifiedFlag(TRUE);
	}
}

//	Edit a hanger list item through combo box
void CPageEditHooks::ComboHangerListItem(int iItem, int iSubItem)
{
	if (iItem < 0 || iSubItem != HSI_SCALE)
		return;

	//	Get subitem rect
	CRect rc;
	m_HangerList.GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rc);
	m_HangerList.ClientToScreen(&rc);

	//	Initialize combo-box window
	m_ComboWnd.ClearAllStrings();
	m_ComboWnd.InsertString(0, "true");
	m_ComboWnd.InsertString(1, "false");

	//	Select item
	A3DHanger* pHanger = (A3DHanger*)m_HangerList.GetItemData(iItem);
	int iSel = pHanger->GetInheritScaleFlag() ? 0 : 1;

	m_EditStatus.bEditing	= true;
	m_EditStatus.iWndID		= IDC_LIST_HANGERS;
	m_EditStatus.iItem		= iItem;
	m_EditStatus.iSubItem	= iSubItem;

	//	Popup input window
	m_ComboWnd.Popup(IDD_EDITHOOK, iSel, rc);
}

//	End editing item through combo box
void CPageEditHooks::EndComboItem(bool bCancel)
{
	if (!m_EditStatus.bEditing)
		return;

	m_EditStatus.bEditing = false;

	if (bCancel)
		return;

	if (m_EditStatus.iWndID == IDC_LIST_HANGERS)
	{
		int iSel = m_ComboWnd.GetSelectedIndex();

		m_HangerList.SetItemText(m_EditStatus.iItem, HSI_SCALE, m_ComboWnd.GetSelectedString());
		A3DHanger* pHanger = (A3DHanger*)m_HangerList.GetItemData(m_EditStatus.iItem);
		pHanger->EnableInheritScale(iSel == 0);
	}
}

BOOL CPageEditHooks::PreTranslateMessage(MSG* pMsg) 
{
	//	Disable 'return' and 'escape' because these keys will close dialog
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


