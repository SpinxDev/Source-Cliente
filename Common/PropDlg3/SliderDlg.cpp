// SliderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "propdlg_rc.h"
#include "propertysliderbox.h"
#include "SliderDlg.h"

void Trim0(CString& str)
{
	int nDot = 0;
	if (nDot = str.Find('.') != -1)
	{
		while (str.Right(1) == _T("0"))
		{
			if (str.GetLength() - nDot == 3)
			{
				return;
			}
			str = str.Left(str.GetLength() - 1);
		}
	}
}

// CXSliderDlg dialog

IMPLEMENT_DYNAMIC(CXSliderDlg, CDialog)

CXSliderDlg::CXSliderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXSliderDlg::IDD, pParent)
{
	m_Value = 0;
	m_fMin = 0;
	m_fMax = 100.0f;
	m_fStep = 1.0f;
}

CXSliderDlg::~CXSliderDlg()
{
}

void CXSliderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_SLIDER_MIN, m_StaticMin);
	DDX_Control(pDX, IDC_STATIC_SLIDER_MAX, m_StaticMax);
	DDX_Control(pDX, IDC_STATIC_SLIDER_POS, m_StaticPos);
}


BEGIN_MESSAGE_MAP(CXSliderDlg, CDialog)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CXSliderDlg message handlers

void CXSliderDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default


	int pos = m_SliderCtrl.GetPos();

	if(m_Value.GetType()==AVariant::AVT_FLOAT)
	{
		m_Value = m_fMin + m_fStep * pos;

		CString str;
		str.Format(_T("%f"),(float)m_Value);
		Trim0(str);
		m_StaticPos.SetWindowText(str);
	}
	else if(m_Value.GetType()==AVariant::AVT_INT)
	{
		m_Value = (int)(m_fMin + m_fStep * pos);
		CString str;
		str.Format(_T("%d"),(int)m_Value);
		m_StaticPos.SetWindowText(str);
	}

	if(m_pSliderBox) m_pSliderBox->TestUpdateSliderData(m_Value);
	
	

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CXSliderDlg::OnOK()
{
	if(m_pSliderBox) m_pSliderBox->UpdateSliderData(m_Value);
	CDialog::OnOK();
}

void CXSliderDlg::OnCancel()
{
	if(m_pSliderBox) m_pSliderBox->UpdateSliderData(m_OldValue,false);
	CDialog::OnCancel();
}

void CXSliderDlg::Exit()
{
	OnCancel();
}



BOOL CXSliderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect rc;
	GetClientRect(&rc);
	int nWidth = rc.Width() - 220;
	rc.left = rc.Width()/2 - nWidth/2 - 40;
	rc.right = rc.Width()/2 + nWidth/2 - 40;

	int nHeight = rc.Height() - 60;
	rc.top = rc.Height()/2 - nHeight/2;
	rc.bottom = rc.Height()/2 + nHeight/2;
	m_SliderCtrl.Create(WS_VISIBLE,rc,this,IDC_SLIDER_XPROPERTY);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CXSliderDlg::SetValue(AVariant value)
{ 
	m_Value = value; m_OldValue = m_Value;
	
	CString str;

	if(m_Value.GetType()==AVariant::AVT_INT)
	{
		if((int)m_Value < (int)m_fMin)
			m_Value = (int)m_fMin;
		if((int)m_Value > (int)m_fMax)
			m_Value = (int)m_fMax;
	}
	else if(m_Value.GetType()==AVariant::AVT_FLOAT)
	{
		if((float)m_Value < m_fMin)
			m_Value = m_fMin;
		if((float)m_Value > m_fMax)
			m_Value = m_fMax;
	}
	
	if(m_Value.GetType()==AVariant::AVT_FLOAT)
	{
		str.Format(_T("%f"),float(m_Value));
		Trim0(str);
		m_StaticPos.SetWindowText(str);
	
		str.Format(_T("%f"),m_fMin);
		Trim0(str);
		m_StaticMin.SetWindowText(str);

		str.Format(_T("%f"),m_fMax);
		Trim0(str);
		m_StaticMax.SetWindowText(str);

		int pos = (int)((float(m_Value) - m_fMin)/m_fStep);
		m_SliderCtrl.SetPos(pos);
	}else if(m_Value.GetType()==AVariant::AVT_INT)
	{
		str.Format(_T("%d"),int(m_Value));
		m_StaticPos.SetWindowText(str);
	
		str.Format(_T("%d"),(int)m_fMin);
		m_StaticMin.SetWindowText(str);

		str.Format(_T("%d"),(int)m_fMax);
		m_StaticMax.SetWindowText(str);

		int pos = (int)((int(m_Value) - m_fMin)/m_fStep);
		m_SliderCtrl.SetPos(pos);
	}
}

void CXSliderDlg::Init(CPropertySliderBox *pBox, AVariant mMin, AVariant mMax, AVariant mStep)
{ 
	m_pSliderBox = pBox;
	
	if(mMin.GetType()==AVariant::AVT_INT)
	{
		m_fMin = float((int)mMin); m_fMax = float((int)mMax); m_fStep = float((int)mStep);
	}
	else if(mMin.GetType()==AVariant::AVT_FLOAT)
	{
		m_fMin = float(mMin); m_fMax = float(mMax); m_fStep = float(mStep);
	}
	
	int step_num = (int)((m_fMax - m_fMin)/m_fStep);
	m_SliderCtrl.SetRange(0,step_num);
};
