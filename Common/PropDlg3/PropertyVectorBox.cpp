// PropertyVectorBox.cpp : implementation file
//

#include "stdafx.h"
#include "PropDlg_rc.h"
#include "PropertyVectorBox.h"
#include "AObject.h"
#include "CustomVectorDlg.h"
#include "PropertyList.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyVectorBox

CPropertyVectorBox::CPropertyVectorBox(size_t dimension)
{
	m_bool_modified = FALSE;
	if (dimension < 1)
		dimension = 1;
	else if (dimension > 4)
		dimension = 4;
	m_dimension = dimension;
}

CPropertyVectorBox::~CPropertyVectorBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyVectorBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyVectorBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(BN_CLICKED, 101, OnBnClicked)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyVectorBox message handlers

void CPropertyVectorBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	ASSERT(m_ptr_property);

	if(pDX->m_bSaveAndValidate)
	{
		m_ptr_property->SetValue(m_ptr_data, MakeAVariant());
		m_bool_modified = FALSE;
	}
	else
	{
		SetAVariant(m_ptr_property->GetValue(m_ptr_data));
	}

	CPropertyBox::DoDataExchange(pDX);
}

int CPropertyVectorBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 创建按钮
	BOOL bResult = m_wnd_button.Create(
		_T(".."),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(0,0,0,0),
		this,
		101);

	ASSERT(bResult);
	m_wnd_button.SetFont(GetFont());
	m_wnd_button.ModifyStyle(WS_TABSTOP,0);
	SetAVariant(m_ptr_property->GetValue(m_ptr_data));
	UpdateData(FALSE);
	return 0;
}

void CPropertyVectorBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 移动按钮
	m_wnd_button.MoveWindow(
		cx - cy + 1, 1,
		cy - 2, cy - 1);
}

BOOL CPropertyVectorBox::IsModified()
{
	return m_bool_modified;
}

void CPropertyVectorBox::DrawSelf(CDC * pDC)
{
	ASSERT(pDC);
	ASSERT(m_ptr_property);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CString text;
	if (m_dimension == 2)
		text.Format(_T("X:%-.2f Y:%-.2f"), m_valueArr[0], m_valueArr[1]);
	else if (m_dimension == 3)
		text.Format(_T("X:%-.2f Y:%-.2f Z:%-.2f"), m_valueArr[0], m_valueArr[1], m_valueArr[2]);
	else	// m_dimension == 4
		text.Format(_T("X:%-.2f Y:%-.2f Z:%-.2f W:%-.2f"), m_valueArr[0], m_valueArr[1], m_valueArr[2], m_valueArr[3]);

	CRect rc;
	GetClientRect(rc);

	rc.top += edge_cy;
	rc.left += edge_cx + 1;
	rc.bottom -= edge_cy;

	int iSave = pDC->SaveDC();
	pDC->SelectObject(GetFont());
	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

	pDC->DrawText(text, rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	pDC->RestoreDC(iSave);
}

AVariant CPropertyVectorBox::MakeAVariant()
{
	if (m_dimension == 2)
		return AVariant(APointF(m_valueArr[0], m_valueArr[1]));
	else if (m_dimension == 3)
		return AVariant(A3DVECTOR3(m_valueArr[0], m_valueArr[1], m_valueArr[2]));
	else	// m_dimension == 4
		return AVariant(A3DVECTOR4(m_valueArr[0], m_valueArr[1], m_valueArr[2], m_valueArr[3]));
}

void CPropertyVectorBox::SetAVariant(AVariant variant)
{
	if (m_dimension == 2)
	{
		APointF value = variant;
		m_valueArr[0] = value.x;
		m_valueArr[1] = value.y;
	}
	else if (m_dimension == 3)
	{
		A3DVECTOR3 value = variant;
		m_valueArr[0] = value.x;
		m_valueArr[1] = value.y;
		m_valueArr[2] = value.z;
	}
	else	// m_dimension == 4
	{
		A3DVECTOR4 value = variant;
		m_valueArr[0] = value.x;
		m_valueArr[1] = value.y;
		m_valueArr[2] = value.z;
		m_valueArr[3] = value.w;
	}
}

afx_msg void CPropertyVectorBox::OnBnClicked()
{
	CCustomVectorDlg dlg(m_dimension, m_valueArr, this);
	if(dlg.DoModal() == IDOK)
	{
		for (size_t i=0; i<m_dimension; ++i)
		{
			m_valueArr[i] = dlg.m_valueArr[i];
		}
		m_bool_modified = TRUE;
		Invalidate(FALSE);
		CPropertyList *pList = (CPropertyList*)GetParent();
		GetParent()->GetParent()->PostMessage(WM_LIST_DATA_CHANGED,(WPARAM)pList,pList->m_int_selected);
	}
}

void CPropertyVectorBox::SetReadOnly( BOOL bReadOnly )
{
	if (m_wnd_button.GetSafeHwnd())
	{
		m_wnd_button.EnableWindow(!bReadOnly);
	}

	CPropertyBox::SetReadOnly(bReadOnly);
}