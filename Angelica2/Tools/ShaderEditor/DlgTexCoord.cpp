// DlgTexCoord.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "DlgTexCoord.h"
#include "A3DTypes.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTexCoord dialog


CDlgTexCoord::CDlgTexCoord(DWORD dwTexCoord, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTexCoord::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTexCoord)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_dwTexCoord = dwTexCoord;
}


void CDlgTexCoord::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTexCoord)
	DDX_Control(pDX, IDC_COMBO_FLAGS, m_FlagsCombo);
	DDX_Control(pDX, IDC_COMBO_INDEX, m_IndexCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTexCoord, CDialog)
	//{{AFX_MSG_MAP(CDlgTexCoord)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTexCoord message handlers

BOOL CDlgTexCoord::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int iIndex = m_dwTexCoord & 0x0000000F;
	int iFlag = m_dwTexCoord & 0x000F0000;

	int i;

	//	Initialize index combo-box
	for (i=0; i < 8; i++)
	{
		char szNum[10];
		itoa(i, szNum, 10);
		m_IndexCombo.InsertString(-1, szNum);
	}

	m_IndexCombo.SetCurSel(iIndex);

	//	Initialize flags combo-box
	for (i=1; i < NUM_TEXCOORDSTR; i++)
		m_FlagsCombo.InsertString(-1, g_aTexCoordFlags[i]);

	switch (iFlag)
	{
	case A3DTCI_CAMERASPACENORMAL:				m_FlagsCombo.SetCurSel(1);	break;
	case A3DTCI_CAMERASPACEPOSITION:			m_FlagsCombo.SetCurSel(2);	break;
	case A3DTCI_CAMERASPACEREFLECTIONVECTOR:	m_FlagsCombo.SetCurSel(3);	break;
	default:
		m_FlagsCombo.SetCurSel(0);
		break;
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTexCoord::OnOK() 
{
	if (!UpdateData(TRUE))
		return;

	int iIndex = m_IndexCombo.GetCurSel();

	int iFlag = 0;
	switch (m_FlagsCombo.GetCurSel())
	{
	case 0:	iFlag = A3DTCI_PASSTHRU;					break;
	case 1: iFlag = A3DTCI_CAMERASPACENORMAL;			break;
	case 2:	iFlag = A3DTCI_CAMERASPACEPOSITION;			break;
	case 3: iFlag = A3DTCI_CAMERASPACEREFLECTIONVECTOR;	break;
	}

	m_dwTexCoord = iFlag | iIndex;

	CDialog::OnOK();
}

