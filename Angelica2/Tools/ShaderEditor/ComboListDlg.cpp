// ComboListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "ComboListDlg.h"

#include "PropertyForm.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
//	CComboListDlg dialog
//
///////////////////////////////////////////////////////////////////////////

CComboListDlg::CComboListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComboListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComboListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iType = 0;
}


void CComboListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComboListDlg)
	DDX_Control(pDX, IDC_VALUELIST, m_ValueList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComboListDlg, CDialog)
	//{{AFX_MSG_MAP(CComboListDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
// 
//	CComboListDlg message handlers
//
///////////////////////////////////////////////////////////////////////////

//	Set dialog's type
bool CComboListDlg::Init(int iType, int iInitValue)
{
	m_iType		 = iType;
	m_iInitValue = iInitValue;

	return true;
}

BOOL CComboListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	const char* aBoolStrs[2] = {"false", "true"};
	char szText[128];
	const char** aStrs = NULL;
	const char* szTitle = NULL;
	int iNumItem;

	switch (m_iType)
	{
	case GND_ZFUNC:		
		
		aStrs	 = &g_aCompStrs[1];
		szTitle	 = g_aCompStrs[0];
		iNumItem = NUM_COMPSTR - 1;
		break;

	case GND_CULLMODE:

		aStrs	 = &g_aCullStrs[1];
		szTitle	 = g_aCullStrs[0];
		iNumItem = NUM_CULLSTR - 1;
		break;

	case GND_ZENABLE:

		strcpy(szText, "Z compare enable");
		aStrs	 = &aBoolStrs[0];
		szTitle	 = szText;
		iNumItem = 2;
		break;

	case GND_ZWENABLE:

		strcpy(szText, "Z write enable");
		aStrs	 = &aBoolStrs[0];
		szTitle	 = szText;
		iNumItem = 2;
		break;

	case SND_FILTER:

		aStrs	 = &g_aFilterStrs[1];
		szTitle	 = g_aFilterStrs[0];
		iNumItem = NUM_FILTERSTR - 1;
		break;

	case SND_ADDRESS:

		aStrs	 = &g_aTexAddrStrs[1];
		szTitle	 = g_aTexAddrStrs[0];
		iNumItem = NUM_TEXADDRSTR - 1;
		break;

	case SND_RESULTARG:

		aStrs	 = &g_aResultArgStrs[1];
		szTitle	 = g_aResultArgStrs[0];
		iNumItem = NUM_RESULTARGSTR - 1;
		break;

	default:

		m_iType	 = -1;
		break;
	}

	if (aStrs && szTitle)
	{
		int i;
		for (i=0; i < iNumItem; i++)
			m_ValueList.InsertString(-1, aStrs[i]);

		SetWindowText(szTitle);
	}

	m_ValueList.SetCurSel(m_iInitValue);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CComboListDlg::OnOK() 
{
	UpdateData(TRUE);

	if (m_iType != -1)
		m_iSelItem = m_ValueList.GetCurSel();
	
	CDialog::OnOK();
}



