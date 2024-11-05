// ToolTabWnd.cpp : implementation file
//

#include "Global.h"
#include "ToolTabWnd.h"
#include "Model.h"

#define new A_DEBUG_NEW

enum
{
	WNDID_TABCONTROL = 100,
};

//	Action stop callback function
void ActionStopCallback(A3DSkinModel* pSkinModel, A3DSkinModelAction* pAction, DWORD dwUserData)
{
/*	if (g_ModelSCBData.iType == SCB_GROUPACTION)
	{
		CPageActionCombo* pPage = (CPageActionCombo*)g_ModelSCBData.dwData;
		pPage->ActionStopCallback(pAction);
	}
*/
}

/////////////////////////////////////////////////////////////////////////////
// CToolTabWnd

CToolTabWnd::CToolTabWnd()
{
	m_pCurPage	= NULL;
}

CToolTabWnd::~CToolTabWnd()
{
}


BEGIN_MESSAGE_MAP(CToolTabWnd, CWnd)
	//{{AFX_MSG_MAP(CToolTabWnd)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_INPUTWNDEND, OnInputWndEnd)
	ON_MESSAGE(WM_COMBOWNDEND, OnComboWndEnd)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CToolTabWnd message handlers

BOOL CToolTabWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CWnd::CreateEx(0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
						LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)), (HBRUSH)COLOR_BACKGROUND),
						lpszWindowName, 
						dwStyle,
						rect, 
						pParentWnd, 
						nID, 
						pContext))
		return FALSE;

	RECT rcWnd = {0, 0, 100, 100};

	//	Create tab control
	m_TabCtrl.Create(WS_CHILD | WS_VISIBLE, rcWnd, this, WNDID_TABCONTROL);

	//	Change tab control font
//	m_TabCtrl.SetFont(GetFont());

	//	Add pages
	m_TabCtrl.InsertItem(PAGE_EDITACTION, "动作编辑");
//	m_TabCtrl.InsertItem(PAGE_ACTIONCOMBO, "动作组合");
	m_TabCtrl.InsertItem(PAGE_EDITHOOK, "悬挂物");
	m_TabCtrl.InsertItem(PAGE_SUPPLETEST, "柔体测试");
	m_TabCtrl.InsertItem(PAGE_SCALEBONE, "骨骼缩放");

	//	Create pages
	m_EditActionPage.Create(IDD_EDITANIMATION, &m_TabCtrl);
//	m_ActionComboPage.Create(IDD_ACTIONCOMBO, &m_TabCtrl);
	m_EditHookPage.Create(IDD_EDITHOOK, &m_TabCtrl);
	m_SuppleTestPage.Create(IDD_SUPPLETEST, &m_TabCtrl);
	m_ScaleBonePage.Create(IDD_SCALEBONE, &m_TabCtrl);

	//	Set default page
	m_TabCtrl.SetCurSel(IDD_EDITANIMATION);
	ChangePage();

	return TRUE;
}

void CToolTabWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if (!m_TabCtrl.m_hWnd)
		return;

	RECT rcWnd;

	//	Get size of dialog window.
	GetClientRect(&rcWnd);

	//	Move the tab control to the new position and size.
	m_TabCtrl.MoveWindow(&rcWnd, TRUE);

	//	Resize current page
	ResizeCurPage();
}

//	Release resources
void CToolTabWnd::Release()
{
	m_EditActionPage.Release();
//	m_ActionComboPage.Release();
	m_EditHookPage.Release();
	m_SuppleTestPage.Release();
	m_ScaleBonePage.Release();
}

BOOL CToolTabWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pnmhdr = (NMHDR*)lParam;

	if (wParam == WNDID_TABCONTROL)	
	{
		//	Is tab control
		if (pnmhdr->code == TCN_SELCHANGE)
			ChangePage();
	}
	
	return CWnd::OnNotify(wParam, lParam, pResult);
}

//	Change page
void CToolTabWnd::ChangePage()
{
	int iSel = m_TabCtrl.GetCurSel();
	CDialog* pDialog = NULL;

	switch (iSel)
	{
	case PAGE_EDITACTION:	pDialog = &m_EditActionPage;	break;
//	case PAGE_ACTIONCOMBO:	pDialog = &m_ActionComboPage;	break;
	case PAGE_EDITHOOK:		pDialog = &m_EditHookPage;		break;
	case PAGE_SUPPLETEST:	pDialog = &m_SuppleTestPage;	break;
	case PAGE_SCALEBONE:	pDialog = &m_ScaleBonePage;		break;
	default:
		return;
	}

	if (m_pCurPage)
	{
		m_pCurPage->SendMessage(WM_ACTIVATEPAGE, 0, 0);
		m_pCurPage->ShowWindow(SW_HIDE);
	}

	pDialog->SendMessage(WM_ACTIVATEPAGE, 1, 0);

	m_pCurPage = pDialog;

	ResizeCurPage();
}

//	Resize current page
void CToolTabWnd::ResizeCurPage()
{
	if (!m_pCurPage)
		return;

	//	Adjust the rectangle to fit the tab control into the dialog's client rectangle.
	RECT rcClient;
	m_TabCtrl.GetClientRect(&rcClient);
	m_TabCtrl.AdjustRect(FALSE, &rcClient);

	m_pCurPage->MoveWindow(&rcClient);
	m_pCurPage->ShowWindow(SW_SHOW);
}

//	Bind model
bool CToolTabWnd::BindModel(CModel* pModel)
{
	m_EditActionPage.BindModel(pModel);
//	m_ActionComboPage.BindModel(pModel);
	m_EditHookPage.BindModel(pModel);
	m_SuppleTestPage.BindModel(pModel);
	m_ScaleBonePage.BindModel(pModel);
	return true;
}

//	Update bone selection
void CToolTabWnd::UpdateBoneSelection()
{
	if (m_pCurPage == &m_EditActionPage)
		m_EditActionPage.UpdateJointListCheckStates();
	else if (m_pCurPage == &m_ScaleBonePage)
		m_ScaleBonePage.UpdateBoneListCheckStates();
}

void CToolTabWnd::OnInputWndEnd(WPARAM wParam, LPARAM lParam)
{
	bool bCancel = wParam ? true : false;

	switch (lParam)
	{
	case IDD_EDITHOOK:

		m_EditHookPage.EndEditItem(bCancel);
		break;

	case IDD_EDITANIMATION:
		
		m_EditActionPage.EndEditItem(bCancel);
		break;

	case IDD_SCALEBONE:
		
		m_ScaleBonePage.EndEditItem(bCancel);
		break;
	}
}

void CToolTabWnd::OnComboWndEnd(WPARAM wParam, LPARAM lParam)
{
	bool bCancel = wParam ? true : false;

	switch (lParam)
	{
	case IDD_SCALEBONE:
		
		m_ScaleBonePage.EndComboItem(bCancel);
		break;
		
	case IDD_EDITHOOK:

		m_EditHookPage.EndComboItem(bCancel);
		break;
	}
}

//	Update skin information
void CToolTabWnd::UpdateSkinInfo()
{
	m_SuppleTestPage.UpdateSkinInfo();
}

//	Update physique information
void CToolTabWnd::UpdatePhysiqueInfo()
{
	m_SuppleTestPage.UpdatePhysiqueInfo();
}

