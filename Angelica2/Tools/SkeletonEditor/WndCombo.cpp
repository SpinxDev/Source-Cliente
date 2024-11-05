// WndCombo.cpp : implementation file
//

#include "Global.h"
#include "SkeletonEditor.h"
#include "WndCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndCombo

CWndCombo::CWndCombo()
{
	m_iOwnWndID = 0;
}

CWndCombo::~CWndCombo()
{
}


BEGIN_MESSAGE_MAP(CWndCombo, CWnd)
	//{{AFX_MSG_MAP(CWndCombo)
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndCombo message handlers

BOOL CWndCombo::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (dwStyle & WS_CHILD)
		dwStyle |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	else
		dwStyle |= WS_CLIPCHILDREN;

	if (!CWnd::CreateEx(0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW))), 
						lpszWindowName,	dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;

	//	Get parent window's font
	if (pParentWnd)
	{
		CFont* pFont = pParentWnd->GetFont();

		LOGFONT Info;
		pFont->GetLogFont(&Info);

		m_Font.CreateFontIndirect(&Info);
	}

	//	Create combo control
	CRect rcCombo = rect;
	rcCombo.DeflateRect(0, 0);
	if (!m_ComboWnd.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, rcCombo, this, ID_COMBOWINDOW))
		return FALSE;

	m_ComboWnd.SetFont(&m_Font);

	return TRUE;
}

void CWndCombo::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if (IsWindow(m_ComboWnd.GetSafeHwnd()))
		m_ComboWnd.MoveWindow(0, 0, cx, cy);
}

//	Delete string
void CWndCombo::DeleteString(const char* szStr)
{
	int iIndex = m_ComboWnd.FindString(-1, szStr);
	if (iIndex != CB_ERR)
		m_ComboWnd.DeleteString(iIndex);
}

//	Popup window, begin to select
bool CWndCombo::Popup(int iOwnWndID, int iSelected, const RECT& rc)
{
	//	Adjust window's position and size
	SetWindowPos(GetParent(), rc.left, rc.top, rc.right-rc.left+1, rc.bottom-rc.top+1, 0);

	m_ComboWnd.SetFocus();
	m_iInitSel = m_ComboWnd.SetCurSel(iSelected);
	if (m_iInitSel == CB_ERR)
	{
		m_iInitSel = 0;
		m_ComboWnd.SetCurSel(0);
	}

	ShowWindow(SW_SHOW);
	UpdateWindow();

	//	Capture mouse
	m_iOwnWndID	 = iOwnWndID;
	m_bSelecting = true;

	return true;
}

//	Close window, end inputing data
void CWndCombo::Close()
{
	//	Get selecting result
	m_iSelected = m_ComboWnd.GetCurSel();
	if (m_iSelected == CB_ERR)
		m_iSelected	= m_iInitSel;

	m_ComboWnd.GetLBText(m_iSelected, m_strSelected);

	ShowWindow(SW_HIDE);
}

void CWndCombo::EndSelecting(bool bCancel, bool bClickEnd, const POINT* pPoint)
{
	if (!m_bSelecting)
		return;

	if (bClickEnd && pPoint)
	{
		RECT rcWnd;
		GetWindowRect(&rcWnd);

		POINT pt = *pPoint;
		ClientToScreen(&pt);

		if (PtInRect(&rcWnd, pt))
			return;
	}

	m_bSelecting = false;

	Close();

	//	Send message to parent
	CWnd* pWnd = GetOwner();
	if (pWnd)
		pWnd->SendMessage(WM_COMBOWNDEND, bCancel ? 1 : 0, m_iOwnWndID);
}

BOOL CWndCombo::PreTranslateMessage(MSG* pMsg) 
{
	if (!m_bSelecting)
		return CWnd::PreTranslateMessage(pMsg);

	if (pMsg->hwnd == m_ComboWnd.GetSafeHwnd())
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if (pMsg->wParam == VK_ESCAPE)
				EndSelecting(true, false, NULL);
		}
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}

void CWndCombo::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);

	if (m_bSelecting && nState == WA_INACTIVE)
		EndSelecting(false, false, NULL);
}


