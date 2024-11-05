// PropertyColorBox.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include "PropDlg_rc.h"
#include "PropertyColorBox.h"
#include "CustomColorDialog.h"
#include "ColorPickerDlg.h"
#include <AC.h>
#include "AProperty.h"
#include "PropertyList.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertyColorBox

CPropertyColorBox::CPropertyColorBox()
{
	m_bool_modified = FALSE;
	m_color_value = RGB(255,255,255);
	m_pColorDialog = NULL;
}

CPropertyColorBox::~CPropertyColorBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyColorBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyColorBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(BN_CLICKED, 101, OnBnClicked)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyColorBox message handlers

void CPropertyColorBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	ASSERT(m_ptr_property);
	if(pDX->m_bSaveAndValidate)
	{
		m_ptr_property->SetValue(m_ptr_data, m_color_value);
		m_bool_modified = FALSE;
	}
	else
	{
		m_color_value = m_ptr_property->GetValue(m_ptr_data);
	}

	CPropertyBox::DoDataExchange(pDX);
}

int CPropertyColorBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 创建子控件
	BOOL bResult = m_wnd_button.Create(
		_T(".."),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(0,0,0,0),
		this,
		101);

	m_pColorDialog = new CColorPickerDlg(0);
	m_pColorDialog->Create(IDD_DIALOG_COLORS,this);
	m_pColorDialog->ShowWindow(SW_HIDE);
	m_pColorDialog->SetColorBox(this);
	

	ASSERT(bResult);
	m_wnd_button.SetFont(GetFont());
	m_wnd_button.ModifyStyle(WS_TABSTOP,0);
	m_color_value = m_ptr_property->GetValue(m_ptr_data);
	UpdateData(FALSE);
	return 0;
}

void CPropertyColorBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	// TODO: 重置各单元大小
	m_wnd_button.MoveWindow(
		cx - cy + 1, 1,
		cy - 2, cy - 1);
}

void CPropertyColorBox::TestUpdateColorData(DWORD cl)
{
	m_color_value = cl;
	Invalidate(FALSE);
	CPropertyList *pList = (CPropertyList*)GetParent();
	GetParent()->GetParent()->PostMessage(WM_LIST_DATA_CHANGING,(WPARAM)pList,pList->m_int_selected);
}

void CPropertyColorBox::UpdateColorData(DWORD cl, bool bUpdate)
{
	m_color_value = cl;
	if( bUpdate ) m_bool_modified = TRUE;
	Invalidate(FALSE);
	CPropertyList *pList = (CPropertyList*)GetParent();
	GetParent()->GetParent()->PostMessage(WM_LIST_DATA_CHANGED,(WPARAM)pList,pList->m_int_selected);
}

afx_msg void CPropertyColorBox::OnBnClicked()
{
	/*
	CCustomColorDialog dlg(m_color_value, CC_ENABLETEMPLATE | CC_ANYCOLOR | CC_FULLOPEN);
	dlg.m_cc.lpTemplateName = MAKEINTRESOURCE(IDC_CHOOSECOLOR_DIALOG);
	if(dlg.DoModal() == IDOK)
	{
		//m_color_value = dlg.GetColor();
		//m_bool_modified = TRUE;
		//Invalidate(FALSE);
		GetParent()->GetParent()->PostMessage(WM_LIST_DATA_CHANGED,0,0);
	}*/
	m_pColorDialog->SetColor(m_color_value);
	m_pColorDialog->ShowWindow(SW_SHOW);

}

BOOL CPropertyColorBox::IsModified()
{
	return m_bool_modified;
}

void CPropertyColorBox::DrawSelf(CDC * pDC)
{
	ASSERT(pDC);
	ASSERT(m_ptr_property);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CString text;
	text.Format(_T("0x%lX"), m_color_value);

	int iSave = pDC->SaveDC();

	COLORREF text_color = RGB(
			A3DCOLOR_GETRED(m_color_value),
			A3DCOLOR_GETGREEN(m_color_value),
			A3DCOLOR_GETBLUE(m_color_value));

	// 绘制颜色方框
	CRect color_rc, button_rc;
	GetClientRect(color_rc);

	color_rc.left += edge_cx;
	color_rc.top += edge_cy;
	color_rc.bottom -= edge_cy;
	color_rc.right = color_rc.left + color_rc.Height();

	CBrush color_brush(text_color);
	pDC->FillRect(color_rc, &color_brush);
	SelectObject(pDC->m_hDC, HPEN(GetStockObject(BLACK_PEN)));
	pDC->DrawEdge(color_rc, BDR_SUNKENINNER | BDR_RAISEDOUTER, BF_RECT | BF_MONO);

	CRect rc;
	GetClientRect(rc);

	rc.top += edge_cy;
	rc.left = color_rc.right + edge_cx;
	rc.bottom -= edge_cy;

	pDC->SelectObject(GetFont());
	//pDC->SetTextColor(text_color);
	pDC->SetTextColor(0);
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
	pDC->DrawText(text, rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	pDC->RestoreDC(iSave);
}


void CPropertyColorBox::OnDestroy()
{
	CPropertyBox::OnDestroy();

	// TODO: Add your message handler code here
	if(m_pColorDialog) 
	{
		m_pColorDialog->DestroyWindow();
		delete m_pColorDialog;
		m_pColorDialog = NULL;
	}
}

void CPropertyColorBox::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyBox::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if(!bShow) if(m_pColorDialog && m_pColorDialog->m_hWnd && m_pColorDialog->IsWindowVisible()) m_pColorDialog->Exit();

}

void CPropertyColorBox::SetReadOnly( BOOL bReadOnly )
{
	if (m_wnd_button.GetSafeHwnd())
	{
		m_wnd_button.EnableWindow(!bReadOnly);
	}

	CPropertyBox::SetReadOnly(bReadOnly);
}
