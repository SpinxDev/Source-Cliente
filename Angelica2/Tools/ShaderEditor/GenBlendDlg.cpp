// GenBlendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "GenBlendDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CGenBlendDlg dialog


CGenBlendDlg::CGenBlendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenBlendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGenBlendDlg)
	//}}AFX_DATA_INIT

	m_iSrcBlend	= 0;
	m_iDstBlend = 0;
}


void CGenBlendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenBlendDlg)
	DDX_Control(pDX, IDC_SRCBLENDCOMBO, m_SrcBlend);
	DDX_Control(pDX, IDC_DESTBLENDCOMBO, m_DestBlend);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenBlendDlg, CDialog)
	//{{AFX_MSG_MAP(CGenBlendDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
//
//	CGenBlendDlg message handlers
//
///////////////////////////////////////////////////////////////////////////

//	Initialize
void CGenBlendDlg::Init(int iSrcBlend, int iDstBlend)
{
	m_iSrcBlend = iSrcBlend;
	m_iDstBlend = iDstBlend;
}

BOOL CGenBlendDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int i;
	const char** aStrs = &g_aBlendStrs[1];;

	for (i=0; i < NUM_BLENDSTR - 1; i++)
	{
		m_SrcBlend.InsertString(-1, aStrs[i]);
		m_DestBlend.InsertString(-1, aStrs[i]);
	}

	m_SrcBlend.SetCurSel(m_iSrcBlend);
	m_DestBlend.SetCurSel(m_iDstBlend);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGenBlendDlg::OnOK() 
{
	UpdateData(TRUE);
	
	m_iSrcBlend = m_SrcBlend.GetCurSel();
	m_iDstBlend = m_DestBlend.GetCurSel();

	CDialog::OnOK();
}


