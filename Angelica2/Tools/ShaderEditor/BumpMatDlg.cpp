// BumpMatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shadereditor.h"
#include "BumpMatDlg.h"

///////////////////////////////////////////////////////////////////////////
// 
//	CBumpMatDlg dialog
//
///////////////////////////////////////////////////////////////////////////

CBumpMatDlg::CBumpMatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBumpMatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBumpMatDlg)
	m_fMat00 = 0.0f;
	m_fMat01 = 0.0f;
	m_fMat10 = 0.0f;
	m_fMat11 = 0.0f;
	//}}AFX_DATA_INIT
}


void CBumpMatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBumpMatDlg)
	DDX_Text(pDX, IDC_MAT00, m_fMat00);
	DDX_Text(pDX, IDC_MAT01, m_fMat01);
	DDX_Text(pDX, IDC_MAT10, m_fMat10);
	DDX_Text(pDX, IDC_MAT11, m_fMat11);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBumpMatDlg, CDialog)
	//{{AFX_MSG_MAP(CBumpMatDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
//
//	CBumpMatDlg message handlers
//
///////////////////////////////////////////////////////////////////////////

bool CBumpMatDlg::Init(float f00, float f01, float f10, float f11)
{
	m_fMat00 = f00;
	m_fMat01 = f01;
	m_fMat10 = f10;
	m_fMat11 = f11;

	return true;
}

BOOL CBumpMatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBumpMatDlg::OnOK() 
{
	UpdateData(TRUE);
	
	CDialog::OnOK();
}

