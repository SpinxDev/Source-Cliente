// TexOPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MainForm.h"
#include "CommonDef.h"
#include "TexOPDlg.h"
#include "PropertyForm.h"

///////////////////////////////////////////////////////////////////////////
//
//	CTexOPDlg dialog
//
///////////////////////////////////////////////////////////////////////////

CTexOPDlg::CTexOPDlg(CWnd* pParent/* NULL */)
	: CDialog(CTexOPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTexOPDlg)
	//}}AFX_DATA_INIT
	m_iType		= OP_COLOR;
	m_iStage	= 0;
	m_hTimer	= 0;

	m_dwInitOP		= 0;
	m_dwInitArg1	= 0;
	m_dwInitArg2	= 0;
	m_dwInitMod1	= 0;
	m_dwInitMod2	= 0;
}

void CTexOPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTexOPDlg)
	DDX_Control(pDX, IDC_OPCOMBO, m_OPCombo);
	DDX_Control(pDX, IDC_MODCOMBO2, m_ModCombo2);
	DDX_Control(pDX, IDC_MODCOMBO1, m_ModCombo1);
	DDX_Control(pDX, IDC_ARGCOMBO2, m_ArgCombo2);
	DDX_Control(pDX, IDC_ARGCOMBO1, m_ArgCombo1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTexOPDlg, CDialog)
	//{{AFX_MSG_MAP(CTexOPDlg)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_ARGCOMBO1, OnSelchangeArgcombo1)
	ON_CBN_SELCHANGE(IDC_ARGCOMBO2, OnSelchangeArgcombo2)
	ON_CBN_SELCHANGE(IDC_MODCOMBO1, OnSelchangeModcombo1)
	ON_CBN_SELCHANGE(IDC_MODCOMBO2, OnSelchangeModcombo2)
	ON_CBN_SELCHANGE(IDC_OPCOMBO, OnSelchangeOpcombo)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_RESTORE, OnBtnRestore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
//
//	CTexOPDlg message handlers
//
///////////////////////////////////////////////////////////////////////////

bool CTexOPDlg::SetParam(int iStage, int iType, const SHADERSTAGE& stage)
{
	m_iStage	= iStage;
	m_iType		= iType;
	m_Stage		= stage;

	return true;
}

BOOL CTexOPDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	const char** aStrs = NULL;
	DWORD dwOP=3, dwArg1=2, dwArg2=1, dwMod1=0, dwMod2=0;
	int i;

	if (m_iType == OP_ALPHA)
	{
		if (m_Stage.dwMask & SSMF_ALPHAOP)
		{
			dwOP = m_Stage.dwAlphaOp - 1;
			dwArg1 = m_Stage.dwAlphaArg1 & A3DTA_SELECTMASK;
			dwMod1 = ModFlagToIndex(m_Stage.dwAlphaArg1 & ~A3DTA_SELECTMASK);
			dwArg2 = m_Stage.dwAlphaArg2 & A3DTA_SELECTMASK;
			dwMod2 = ModFlagToIndex(m_Stage.dwAlphaArg2 & ~A3DTA_SELECTMASK);
		}
	}
	else
	{
		if (m_Stage.dwMask & SSMF_COLOROP)
		{
			dwOP = m_Stage.dwColorOp - 1;
			dwArg1 = m_Stage.dwColorArg1 & A3DTA_SELECTMASK;
			dwMod1 = ModFlagToIndex(m_Stage.dwColorArg1 & ~A3DTA_SELECTMASK);
			dwArg2 = m_Stage.dwColorArg2 & A3DTA_SELECTMASK;
			dwMod2 = ModFlagToIndex(m_Stage.dwColorArg2 & ~A3DTA_SELECTMASK);
		}
	}

	m_dwInitOP		= dwOP;
	m_dwInitArg1	= dwArg1;
	m_dwInitArg2	= dwArg2;
	m_dwInitMod1	= dwMod1;
	m_dwInitMod2	= dwMod2;

	//	Initialize OP list
	aStrs = &g_aTexOpStrs[1];
	for (i=0; i < NUM_TEXOPSTR-1; i++)
		m_OPCombo.InsertString(-1, aStrs[i]);

	m_OPCombo.SetCurSel(dwOP);

	//	Initialize argment flags
	aStrs = &g_aTexArgStrs[1];
	for (i=0; i < NUM_TEXARGSTR-1; i++)
	{
		m_ArgCombo1.InsertString(-1, aStrs[i]);
		m_ArgCombo2.InsertString(-1, aStrs[i]);
	}

	m_ArgCombo1.SetCurSel(dwArg1);
	m_ArgCombo2.SetCurSel(dwArg2);

	//	Initialize modifier flags
	aStrs = &g_aTexModStrs[0];
	for (i=0; i < NUM_TEXMODSTR; i++)
	{
		m_ModCombo1.InsertString(-1, aStrs[i]);
		m_ModCombo2.InsertString(-1, aStrs[i]);
	}
	
	m_ModCombo1.SetCurSel(dwMod1);
	m_ModCombo2.SetCurSel(dwMod2);

	//	Set dialog's title
	if (m_iType == OP_COLOR)
		SetWindowText("Color operation");
	else
		SetWindowText("Alpha operation");

	UpdateData(FALSE);

	//	Create a timer to trigger WM_TIMER message, after that system will
	//	generate a WM_ENTERIDLE message and send it to main frame.
	m_hTimer = SetTimer(1, 25, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTexOPDlg::OnOK() 
{
	CDialog::OnOK();
}

void CTexOPDlg::OnTimer(UINT nIDEvent) 
{
	//	Needn't to do anything
	CDialog::OnTimer(nIDEvent);
}

void CTexOPDlg::OnSelchangeArgcombo1() 
{
	ApplyChanges();
}

void CTexOPDlg::OnSelchangeArgcombo2() 
{
	ApplyChanges();
}

void CTexOPDlg::OnSelchangeModcombo1() 
{
	ApplyChanges();
}

void CTexOPDlg::OnSelchangeModcombo2() 
{
	ApplyChanges();
}

void CTexOPDlg::OnSelchangeOpcombo() 
{
	ApplyChanges();
}

void CTexOPDlg::ApplyChanges()
{
	UpdateData(TRUE);

	int iOP	= m_OPCombo.GetCurSel();
	DWORD dwArg1 = m_ArgCombo1.GetCurSel();
	DWORD dwMod1 = m_ModCombo1.GetCurSel();
	DWORD dwArg2 = m_ArgCombo2.GetCurSel();
	DWORD dwMod2 = m_ModCombo2.GetCurSel();

	if (dwMod1)
		dwMod1 = 1 << (4 + dwMod1 - 1);

	if (dwMod2)
		dwMod2 = 1 << (4 + dwMod2 - 1);

	if (m_iType == OP_COLOR)
	{
		m_Stage.dwMask |= SSMF_COLOROP;

		m_Stage.dwColorOp	= iOP + 1;
		m_Stage.dwColorArg1 = dwArg1 | dwMod1;
		m_Stage.dwColorArg2 = dwArg2 | dwMod2;

		CMainForm* pMainForm = (CMainForm*)AfxGetMainWnd();
		pMainForm->GetPropertyForm()->UpdateTextureOps(false, m_iStage, m_Stage);
	}
	else
	{
		m_Stage.dwMask |= SSMF_ALPHAOP;

		m_Stage.dwAlphaOp	= iOP + 1;
		m_Stage.dwAlphaArg1 = dwArg1 | dwMod1;
		m_Stage.dwAlphaArg2 = dwArg2 | dwMod2;

		CMainForm* pMainForm = (CMainForm*)AfxGetMainWnd();
		pMainForm->GetPropertyForm()->UpdateTextureOps(true, m_iStage, m_Stage);
	}
}


void CTexOPDlg::OnDestroy() 
{
	if (m_hTimer)
	{
		KillTimer(m_hTimer);
		m_hTimer = NULL;
	}

	CDialog::OnDestroy();
}

void CTexOPDlg::OnBtnRestore() 
{
	m_OPCombo.SetCurSel(m_dwInitOP);
	m_ArgCombo1.SetCurSel(m_dwInitArg1);
	m_ArgCombo2.SetCurSel(m_dwInitArg2);
	m_ModCombo1.SetCurSel(m_dwInitMod1);
	m_ModCombo2.SetCurSel(m_dwInitMod2);
	
	ApplyChanges();
}
