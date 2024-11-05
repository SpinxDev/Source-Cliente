// PropertyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PropDlg_rc.h"
#include "PropertyDialog.h"
#include "PropertyList.h"
#include "PropertyBox.h"
#include "AObject.h"
#include <assert.h>

/////////////////////////////////////////////////////////////////////////////
// CPropertyDialog dialog


CPropertyDialog::CPropertyDialog(A3DEngine* pEngine, APropertyObject * pData,CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PROPERTY_DIALOG, pParent)
{
	CPropertyBox::g_pA3DEngine = pEngine;
	m_pData = pData;
	//{{AFX_DATA_INIT(CPropertyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pList = NULL;
}


void CPropertyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_OPTION, m_btnOption);
	if(m_pList == NULL) return;
	if(pDX->m_bSaveAndValidate)
	{
		if (!m_pList->UpdateData(TRUE))
		{
			pDX->Fail();
			return;
		}
	}
	else
	{
		m_pList->UpdateData(FALSE);
	}
	//{{AFX_DATA_MAP(CPropertyDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyDialog, CDialog)
	//{{AFX_MSG_MAP(CPropertyDialog)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_OPTION, OnBnClickedOption)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyDialog message handlers

BOOL CPropertyDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rc;
	CWnd * pRect = GetDlgItem(IDC_RECT_PROPERIES);
	if(pRect)
	{
		pRect->GetWindowRect(rc);
		ScreenToClient(rc);
	}
	else
	{
		MessageBox(_T("不能创建属性列表"));
		assert(false && "不能创建属性列表");
		return false;
	}
	SetWindowText(_T("Attributes"));
	
	m_pList = new CPropertyList();
	BOOL bResult = m_pList->Create("Property sheet", WS_CHILD | WS_VISIBLE, rc, this, 101);
	ASSERT(bResult);
	m_pList->AttachDataObject(m_pData);

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyDialog::OnDestroy() 
{
	CDialog::OnDestroy();

	if(m_pList)
	{
		m_pList->DestroyWindow();
		delete m_pList;
		m_pList = NULL;
	}
}

void CPropertyDialog::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	CDialog::OnOK();
}

void CPropertyDialog::OnBnClickedOption()
{
	// TODO: Add your control notification handler code here
}
