// DlgTexTransform.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "DlgTexTransform.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTexTransform dialog


CDlgTexTransform::CDlgTexTransform(const A3DMATRIX4& mat, DWORD dwFlag, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTexTransform::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTexTransform)
	m_11 = mat._11;
	m_12 = mat._12;
	m_13 = mat._13;
	m_14 = mat._14;
	m_21 = mat._21;
	m_22 = mat._22;
	m_23 = mat._23;
	m_24 = mat._24;
	m_31 = mat._31;
	m_32 = mat._32;
	m_33 = mat._33;
	m_34 = mat._34;
	m_41 = mat._41;
	m_42 = mat._42;
	m_43 = mat._43;
	m_44 = mat._44;
	m_bProjected = FALSE;
	//}}AFX_DATA_INIT

	m_dwFlag = dwFlag;
}


void CDlgTexTransform::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTexTransform)
	DDX_Control(pDX, IDC_COMBO_FLAGS, m_FlagCombo);
	DDX_Text(pDX, IDC_EDIT_M11, m_11);
	DDX_Text(pDX, IDC_EDIT_M12, m_12);
	DDX_Text(pDX, IDC_EDIT_M13, m_13);
	DDX_Text(pDX, IDC_EDIT_M14, m_14);
	DDX_Text(pDX, IDC_EDIT_M21, m_21);
	DDX_Text(pDX, IDC_EDIT_M22, m_22);
	DDX_Text(pDX, IDC_EDIT_M23, m_23);
	DDX_Text(pDX, IDC_EDIT_M24, m_24);
	DDX_Text(pDX, IDC_EDIT_M31, m_31);
	DDX_Text(pDX, IDC_EDIT_M32, m_32);
	DDX_Text(pDX, IDC_EDIT_M33, m_33);
	DDX_Text(pDX, IDC_EDIT_M34, m_34);
	DDX_Text(pDX, IDC_EDIT_M41, m_41);
	DDX_Text(pDX, IDC_EDIT_M42, m_42);
	DDX_Text(pDX, IDC_EDIT_M43, m_43);
	DDX_Text(pDX, IDC_EDIT_M44, m_44);
	DDX_Check(pDX, IDC_CHECK_PROJECTED, m_bProjected);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTexTransform, CDialog)
	//{{AFX_MSG_MAP(CDlgTexTransform)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTexTransform message handlers

BOOL CDlgTexTransform::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	Initialize flag list
	for (int i=1; i < NUM_TEXTRANSSTR; i++)
		m_FlagCombo.InsertString(-1, g_aTexTransFlags[i]);

	int iIndex = m_dwFlag & 0x0000000f;
	m_FlagCombo.SetCurSel(iIndex);

	m_bProjected = (m_dwFlag & A3DTTFF_PROJECTED) ? true : false;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTexTransform::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	
	m_matTM._11	= m_11;
	m_matTM._12	= m_12;
	m_matTM._13	= m_13;
	m_matTM._14	= m_14;
	m_matTM._21	= m_21;
	m_matTM._22	= m_22;
	m_matTM._23	= m_23;
	m_matTM._24	= m_24;
	m_matTM._31	= m_31;
	m_matTM._32	= m_32;
	m_matTM._33	= m_33;
	m_matTM._34	= m_34;
	m_matTM._41	= m_41;
	m_matTM._42	= m_42;
	m_matTM._43	= m_43;
	m_matTM._44	= m_44;

	m_dwFlag = (DWORD)m_FlagCombo.GetCurSel();
	if (m_bProjected)
		m_dwFlag |= A3DTTFF_PROJECTED;

	CDialog::OnOK();
}
