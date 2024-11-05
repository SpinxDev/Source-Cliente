// cstm2dlg.cpp : implementation file
//

#include "stdafx.h"
#include "AngelicaAppWiz.h"
#include "cstm2dlg.h"
#include "AngelicaAppWizaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustom2Dlg dialog


CCustom2Dlg::CCustom2Dlg()
	: CAppWizStepDlg(CCustom2Dlg::IDD)
{
	//{{AFX_DATA_INIT(CCustom2Dlg)
	m_iWindowed = 0;
	m_iRenderSize = 0;
	//}}AFX_DATA_INIT
}


void CCustom2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustom2Dlg)
	DDX_Radio(pDX, IDC_RADIO1, m_iWindowed);
	DDX_Radio(pDX, IDC_RADIO5, m_iRenderSize);
	//}}AFX_DATA_MAP
}

// This is called whenever the user presses Next, Back, or Finish with this step
//  present.  Do all validation & data exchange from the dialog in this function.
BOOL CCustom2Dlg::OnDismiss()
{
	if (!UpdateData(TRUE))
		return FALSE;

	if ( m_iWindowed == 0)
		AngelicaAppWizaw.SetStringVar(_T("FULLSCREEN"), _T("false"));
	else
		AngelicaAppWizaw.SetStringVar(_T("FULLSCREEN"), _T("true"));

	if (m_iRenderSize == 0)
	{
		AngelicaAppWizaw.SetStringVar(_T("RENDER_WIDTH"), _T("640"));
		AngelicaAppWizaw.SetStringVar(_T("RENDER_HEIGHT"), _T("480"));
	}
	else if (m_iRenderSize == 1)
	{
		AngelicaAppWizaw.SetStringVar(_T("RENDER_WIDTH"), _T("800"));
		AngelicaAppWizaw.SetStringVar(_T("RENDER_HEIGHT"), _T("600"));
	}
	else	//	m_iRenderSize == 2
	{
		AngelicaAppWizaw.SetStringVar(_T("RENDER_WIDTH"), _T("1024"));
		AngelicaAppWizaw.SetStringVar(_T("RENDER_HEIGHT"), _T("768"));
	}

	return TRUE;	// return FALSE if the dialog shouldn't be dismissed
}


BEGIN_MESSAGE_MAP(CCustom2Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(CCustom2Dlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustom2Dlg message handlers

BOOL CCustom2Dlg::OnInitDialog() 
{
	CAppWizStepDlg::OnInitDialog();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
