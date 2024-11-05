// PropertyColorBox.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include "PropDlg_rc.h"
#include "PropertySliderBox.h"
#include "CustomColorDialog.h"
#include "ColorPickerDlg.h"
#include <AC.h>
#include "AProperty.h"
#include "sliderdlg.h"
#include "PropertyList.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertySliderBox

CPropertySliderBox::CPropertySliderBox()
{
	m_bool_modified = FALSE;
	m_value = 0;
	m_pSliderDialog = NULL;
}

CPropertySliderBox::~CPropertySliderBox()
{
}


BEGIN_MESSAGE_MAP(CPropertySliderBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertySliderBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(BN_CLICKED, 101, OnBnClicked)
	ON_CONTROL(EN_CHANGE, 102, OnEditChanged)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertySliderBox message handlers

void CPropertySliderBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	ASSERT(m_ptr_property);
	if(pDX->m_bSaveAndValidate)
	{
		if(m_value.GetType() == AVariant::AVT_INT)
		{
			int value = m_value;
			DDX_Text(pDX, 102, value);
		}
		else if(m_value.GetType() == AVariant::AVT_FLOAT)
		{
			float value = m_value;
			DDX_Text(pDX, 102, value);
		}
		
		m_wnd_edit.SetModify(FALSE);
		ARange *pRange = m_ptr_property->GetPropertyRange();
		if (pRange)
		{
			DDV_MinMaxFloat(pDX, m_value, pRange->GetMinValue(), pRange->GetMaxValue());
		}
		m_ptr_property->SetValue(m_ptr_data, m_value);
		m_bool_modified = FALSE;
	}
	else
	{
		if(m_value.GetType() == AVariant::AVT_INT)
		{
			int value = m_ptr_property->GetValue(m_ptr_data);
			DDX_Text(pDX, 102, value);
		}
		else if(m_value.GetType() == AVariant::AVT_FLOAT)
		{
			float value = m_ptr_property->GetValue(m_ptr_data);
			DDX_Text(pDX, 102, value);
		}		
	}

	CPropertyBox::DoDataExchange(pDX);
}

int CPropertySliderBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	ARange* pRange = m_ptr_property->GetPropertyRange();

	m_pSliderDialog = new CXSliderDlg();
	m_pSliderDialog->Create(IDD_DIALOG_XSLIDER,this);
	m_pSliderDialog->ShowWindow(SW_HIDE);
	
	if(pRange) m_pSliderDialog->Init(this,pRange->GetMinValue(),pRange->GetMaxValue(),pRange->GetStep());
	else m_pSliderDialog->Init(this,0.0f,100.0f,1.0f);

	ASSERT(bResult);
	m_wnd_button.SetFont(GetFont());
	m_wnd_button.ModifyStyle(WS_TABSTOP,0);
	m_value = m_ptr_property->GetValue(m_ptr_data);

	bResult = m_wnd_edit.Create(
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		CRect(0,0,0,0),
		this,
		102);

	ASSERT(bResult);
	m_wnd_edit.SetFont(GetFont());

	UpdateData(FALSE);
	return 0;
}

void CPropertySliderBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	// TODO: 重置各单元大小
	m_wnd_button.MoveWindow(
		cx - cy + 1, 1,
		cy - 2, cy - 1);

	m_wnd_edit.MoveWindow(4,4,cx - 30,cy - 4);
}

void CPropertySliderBox::TestUpdateSliderData(AVariant s)
{
	m_value = s;
	Invalidate(FALSE);
	CPropertyList *pList = (CPropertyList*)GetParent();
	GetParent()->GetParent()->PostMessage(WM_LIST_DATA_CHANGING,(WPARAM)pList,pList->m_int_selected);
}

void CPropertySliderBox::UpdateSliderData(AVariant s, bool bUpdate)
{
	m_value = s;
	if( bUpdate )
	{
		m_bool_modified = TRUE;
		UpdateData(TRUE);
	}
	UpdateData( !bUpdate);
	Invalidate(FALSE);
	CPropertyList *pList = (CPropertyList*)GetParent();
	GetParent()->GetParent()->PostMessage(WM_LIST_DATA_CHANGED,(WPARAM)pList,pList->m_int_selected);
}

afx_msg void CPropertySliderBox::OnBnClicked()
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
	m_pSliderDialog->SetValue(m_value);
	m_pSliderDialog->ShowWindow(SW_SHOW);

}

BOOL CPropertySliderBox::IsModified()
{
	return m_bool_modified || m_wnd_edit.GetModify();
}

void CPropertySliderBox::OnEditChanged()
{
	CString csText;
	m_wnd_edit.GetWindowText(csText);
	if(csText.GetLength() == 0)
		return;
	CString csInfo;	
	ARange* pRange = m_ptr_property->GetPropertyRange();
	if(m_value.GetType() == AVariant::AVT_INT)
	{		
		int iMin = (int)pRange->GetMinValue();
		int iMax = (int)pRange->GetMaxValue();
		csInfo.Format(_T("请输入%d到%d之间的数。"), iMin, iMax);
#ifdef UNICODE
		int value = _wtoi(csText);
#else
		int value = atoi(csText);
#endif
		
		m_value = value;		
		if(value > iMax)
		{
			MessageBox(csInfo);
			value = iMax;
			m_value = value;
			m_ptr_property->SetValue(m_ptr_data, m_value);
			UpdateData(FALSE);
		}
		if(value < iMax)
		{
			MessageBox(csInfo);
			value = iMax;
			m_value = value;
			m_ptr_property->SetValue(m_ptr_data, m_value);
			UpdateData(FALSE);
		}
	}
	else if(m_value.GetType() == AVariant::AVT_FLOAT)
	{
		float fMin = (float)pRange->GetMinValue();
		float fMax = (float)pRange->GetMaxValue();
		csInfo.Format(_T("请输入%.2f到%.2f之间的数。"), fMin, fMax);
#ifdef UNICODE
		float value = (float)_wtof(csText);
#else
		float value = (float)atof(csText);
#endif
		m_value = value;
		if(value > fMax)
		{
			MessageBox(csInfo);
			value = fMax;
			m_value = value;
			m_ptr_property->SetValue(m_ptr_data, m_value);
			UpdateData(FALSE);
		}
		if(value < fMin)
		{
			MessageBox(csInfo);
			value = fMin;
			m_value = value;
			m_ptr_property->SetValue(m_ptr_data, m_value);
			UpdateData(FALSE);
		}
	}
	
	m_pSliderDialog->SetValue(m_value);	
	m_wnd_edit.SetModify(TRUE);
	CPropertyList *pList = (CPropertyList*)GetParent();	
	GetParent()->GetParent()->PostMessage(WM_LIST_DATA_CHANGED,(WPARAM)pList,pList->m_int_selected);
}

void CPropertySliderBox::DrawSelf(CDC * pDC)
{
	ASSERT(pDC);
	ASSERT(m_ptr_property);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CString text;
	if(m_value.GetType()==AVariant::AVT_FLOAT)
		text.Format(_T("%.2f"), float(m_value));
	else if(m_value.GetType()==AVariant::AVT_INT) text.Format(_T("%d"), int(m_value));

	CRect rc;
	GetClientRect(rc);

	rc.top += edge_cy;
	rc.left += edge_cx + 1;
	rc.bottom -= edge_cy;

	int iSave = pDC->SaveDC();
	pDC->SelectObject(GetFont());
	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

	CBrush color_brush(GetSysColor(COLOR_WINDOW));
	pDC->FillRect(rc,&color_brush );
	pDC->DrawText(text, rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	pDC->RestoreDC(iSave);
}


void CPropertySliderBox::OnDestroy()
{
	CPropertyBox::OnDestroy();

	// TODO: Add your message handler code here
	if(m_pSliderDialog) 
	{
		m_pSliderDialog->DestroyWindow();
		delete m_pSliderDialog;
		m_pSliderDialog = NULL;
	}
}

void CPropertySliderBox::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyBox::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if(!bShow)
	{
		if(m_pSliderDialog && m_pSliderDialog->m_hWnd && m_pSliderDialog->IsWindowVisible())
			m_pSliderDialog->Exit();		
		if(IsModified())
		{
			UpdateData(TRUE);
		}
	}
	else
	{
		UpdateData(FALSE);
		ASet * pSet = m_ptr_property->GetPropertySet();
		if(!pSet)
		{
			m_wnd_edit.SetFocus();
			m_wnd_edit.SetSel(0,-1);
		}else
		{
			CWnd * pChild = GetWindow(GW_CHILD);
			if(pChild)
			{
				pChild->SetFocus();
			}
		}
	}

}

void CPropertySliderBox::SetReadOnly( BOOL bReadOnly )
{
	if (m_wnd_edit.GetSafeHwnd())
	{
		m_wnd_edit.SetReadOnly(bReadOnly);
	}

	if (m_wnd_button.GetSafeHwnd())
	{
		m_wnd_button.EnableWindow(!bReadOnly);
	}

	CPropertyBox::SetReadOnly(bReadOnly);
}