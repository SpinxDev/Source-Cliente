// DlgColorValue.cpp : implementation file
//

#include "stdafx.h"
#include "shadereditor.h"
#include "DlgColorValue.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgColorValue dialog


CDlgColorValue::CDlgColorValue(const A3DCOLORVALUE& col, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColorValue::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColorValue)
	m_b = 0.0f;
	m_a = 0.0f;
	m_g = 0.0f;
	m_r = 0.0f;
	//}}AFX_DATA_INIT

	m_colValue = col;
}


void CDlgColorValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColorValue)
	DDX_Text(pDX, IDC_EDIT_B, m_b);
	DDV_MinMaxFloat(pDX, m_b, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_A, m_a);
	DDV_MinMaxFloat(pDX, m_a, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_G, m_g);
	DDV_MinMaxFloat(pDX, m_g, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_R, m_r);
	DDV_MinMaxFloat(pDX, m_r, 0.f, 1.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColorValue, CDialog)
	//{{AFX_MSG_MAP(CDlgColorValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColorValue message handlers

BOOL CDlgColorValue::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_r = m_colValue.r;
	m_g = m_colValue.g;
	m_b = m_colValue.b;
	m_a = m_colValue.a;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgColorValue::OnOK() 
{
	if (!UpdateData(TRUE))
		return;

	m_colValue.r = m_r;
	m_colValue.g = m_g;
	m_colValue.b = m_b;
	m_colValue.a = m_a;

	CDialog::OnOK();
}

