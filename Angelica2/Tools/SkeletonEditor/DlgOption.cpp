// DlgOption.cpp : implementation file
//

#include "Global.h"
#include "SkeletonEditor.h"
#include "DlgOption.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgOption dialog


CDlgOption::CDlgOption(EDITCONFIG* pConfig, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOption)
	m_fBoxSize = 0.0f;
	m_fStaffRadius = 0.0f;
	m_iCamMoveSpeed = -1;
	m_iCamRotSpeed = -1;
	m_iMouseDragSpeed = -1;
	//}}AFX_DATA_INIT

	m_pConfig = pConfig;
}


void CDlgOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOption)
	DDX_Text(pDX, IDC_EDIT_BOXSIZE, m_fBoxSize);
	DDX_Text(pDX, IDC_EDIT_STAFFRADIUS, m_fStaffRadius);
	DDX_CBIndex(pDX, IDC_COMBO_CAMERAMOVESPEED, m_iCamMoveSpeed);
	DDX_CBIndex(pDX, IDC_COMBO_CAMERAROTSPEED, m_iCamRotSpeed);
	DDX_CBIndex(pDX, IDC_COMBO_MOUSEDRAGSPEED, m_iMouseDragSpeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialog)
	//{{AFX_MSG_MAP(CDlgOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOption message handlers

BOOL CDlgOption::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pConfig);

	m_fBoxSize			= m_pConfig->fBoneBoxSize * 2.0f;
	m_fStaffRadius		= m_pConfig->fStaffRadius;
	m_iCamMoveSpeed		= m_pConfig->iCameraMoveSpeed - 1;
	m_iCamRotSpeed		= m_pConfig->iCameraRotSpeed - 1;
	m_iMouseDragSpeed	= m_pConfig->iMouseDragSpeed - 1;
	
	CComboBox* pCombo1 = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERAMOVESPEED);
	CComboBox* pCombo2 = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERAROTSPEED);
	CComboBox* pCombo3 = (CComboBox*)GetDlgItem(IDC_COMBO_MOUSEDRAGSPEED);

	for (int i=1; i <= 9; i++)
	{
		char sz[10];
		itoa(i, sz, 10);

		pCombo1->InsertString(-1, sz);
		pCombo2->InsertString(-1, sz);
		pCombo3->InsertString(-1, sz);
	}

	pCombo1->SetCurSel(m_iCamMoveSpeed);
	pCombo2->SetCurSel(m_iCamRotSpeed);
	pCombo3->SetCurSel(m_iMouseDragSpeed);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOption::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	
	if (m_fBoxSize <= 0.0f || m_fStaffRadius <= 0.0f)
	{
		MessageBox("请输入大于 0 的尺寸");
		return;
	}

	m_pConfig->fBoneBoxSize	= m_fBoxSize * 0.5f;
	m_pConfig->fStaffRadius	= m_fStaffRadius;

	m_pConfig->iCameraMoveSpeed	= m_iCamMoveSpeed + 1;
	m_pConfig->iCameraRotSpeed	= m_iCamRotSpeed + 1;
	m_pConfig->iMouseDragSpeed	= m_iMouseDragSpeed + 1;

	CDialog::OnOK();
}

