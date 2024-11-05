// cstm1dlg.cpp : implementation file
//

#include "stdafx.h"
#include "AngelicaAppWiz.h"
#include "cstm1dlg.h"
#include "AngelicaAppWizaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustom1Dlg dialog


CCustom1Dlg::CCustom1Dlg()
	: CAppWizStepDlg(CCustom1Dlg::IDD)
{
	//{{AFX_DATA_INIT(CCustom1Dlg)
	m_iAppType = 0;
	//}}AFX_DATA_INIT
}


void CCustom1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustom1Dlg)
	DDX_Radio(pDX, IDC_RADIO1, m_iAppType);
	//}}AFX_DATA_MAP
}

// This is called whenever the user presses Next, Back, or Finish with this step
//  present.  Do all validation & data exchange from the dialog in this function.
BOOL CCustom1Dlg::OnDismiss()
{
	if (!UpdateData(TRUE))
		return FALSE;

	if (m_iAppType == 0)
	{
		AngelicaAppWizaw.SetBoolVar(_T("APP_TYPE_WIN32"), true);
		AngelicaAppWizaw.SetBoolVar(_T("APP_TYPE_MFC"), false);

		//	Set default values. Symbol replace error may occurs without default
		//	values, because some steps may be skipped if user press "Finish" button
		//	directly.
		AngelicaAppWizaw.SetStringVar(_T("FULLSCREEN"), _T("false"));
		AngelicaAppWizaw.SetStringVar(_T("RENDER_WIDTH"), _T("800"));
		AngelicaAppWizaw.SetStringVar(_T("RENDER_HEIGHT"), _T("600"));
	}
	else	//	m_iAppType == 1
	{
		AngelicaAppWizaw.SetBoolVar(_T("APP_TYPE_WIN32"), false);
		AngelicaAppWizaw.SetBoolVar(_T("APP_TYPE_MFC"), true);

		//	Set default values. Symbol replace error may occurs without default
		//	values, because some steps may be skipped if user press "Finish" button
		//	directly.
		AngelicaAppWizaw.SetBoolVar(_T("MFC_LIB_STATIC"), false);
		AngelicaAppWizaw.SetBoolVar(_T("MFC_LIB_DLL"), true);
	}

	return TRUE;	// return FALSE if the dialog shouldn't be dismissed
}


BEGIN_MESSAGE_MAP(CCustom1Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(CCustom1Dlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustom1Dlg message handlers

BOOL CCustom1Dlg::OnInitDialog() 
{
	CAppWizStepDlg::OnInitDialog();
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

