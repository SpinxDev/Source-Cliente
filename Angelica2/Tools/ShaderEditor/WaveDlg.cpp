// WaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "WaveDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CWaveDlg dialog


CWaveDlg::CWaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaveDlg)
	m_fAmp = 0.0f;
	m_fBase = 0.0f;
	m_fPhase = 0.0f;
	m_iPeriod = 0;
	//}}AFX_DATA_INIT

	m_szTitle[0] = '\0';
	m_iFunc		 = 0;
	m_bTurb		 = false;
}


void CWaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaveDlg)
	DDX_Control(pDX, IDC_WAVECOMBO, m_WaveCombo);
	DDX_Text(pDX, IDC_AMPLITUDE, m_fAmp);
	DDX_Text(pDX, IDC_BASELINE, m_fBase);
	DDX_Text(pDX, IDC_PHASE, m_fPhase);
	DDX_Text(pDX, IDC_PERIOD, m_iPeriod);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaveDlg, CDialog)
	//{{AFX_MSG_MAP(CWaveDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaveDlg message handlers

bool CWaveDlg::Init(char* szTitle, SHADERWAVE& Wave)
{
	strcpy(m_szTitle, szTitle);

	m_iFunc		= Wave.iFunc - 1;
	m_fBase		= Wave.fBase;
	m_fAmp		= Wave.fAmp;
	m_fPhase	= Wave.fPhase;
	m_iPeriod	= Wave.iPeriod;

	m_bTurb	= (Wave.iType == SWO_TCTURB) ? true : false;

	return true;
}

void CWaveDlg::GetData(SHADERWAVE& Wave)
{
	Wave.iFunc	 = m_iFunc + 1;
	Wave.fBase	 = m_fBase;
	Wave.fAmp	 = m_fAmp;
	Wave.fPhase	 = m_fPhase;
	Wave.iPeriod = m_iPeriod;
}

BOOL CWaveDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_bTurb)
	{
		//	Disable function list and baseline
		m_WaveCombo.EnableWindow(FALSE);
		GetDlgItem(IDC_BASELINE)->EnableWindow(FALSE);
	}
	else
	{
		int i;
		const char** aStrs = &g_aWaveStrs[1];
		
		//	Initialze function list
		for (i=0; i < NUM_WAVESTR-1; i++)
			m_WaveCombo.InsertString(-1, aStrs[i]);
		
		m_WaveCombo.SetCurSel(m_iFunc);
		
		//	Dialog's title
		SetWindowText(m_szTitle);
		
		UpdateData(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaveDlg::OnOK() 
{
	UpdateData(TRUE);

	if (m_iPeriod <= 0)
	{
		MessageBox("Period (周期) 必须是正整数。");
		return;
	}

	m_iFunc = m_WaveCombo.GetCurSel();
	
	CDialog::OnOK();
}


