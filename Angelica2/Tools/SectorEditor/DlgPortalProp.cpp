// DlgPortalProp.cpp : implementation file
//

#include "stdafx.h"
#include "SectorEditor.h"
#include "DlgPortalProp.h"
#include "Portal.h"
#include "SectorEditorDoc.h"
#include "Utility.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPortalProp dialog


CDlgPortalProp::CDlgPortalProp(CPortal* pPortal, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPortalProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPortalProp)
	m_strName = _T("");
	//}}AFX_DATA_INIT

	m_pPortal = pPortal;
}


void CDlgPortalProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPortalProp)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 31);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPortalProp, CDialog)
	//{{AFX_MSG_MAP(CDlgPortalProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPortalProp message handlers

BOOL CDlgPortalProp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT(m_pPortal);

	m_strName = m_pPortal->GetName();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPortalProp::OnOK() 
{
	UpdateData(TRUE);

	CPortal* pPortal = AUX_GetMainFrame()->GetDocument()->GetPortalByName(m_strName);
	if (pPortal && pPortal != m_pPortal)
	{
		MessageBox("同名的 portal 已经存在!");
		return;
	}

	m_pPortal->SetName(m_strName);

	CDialog::OnOK();
}


