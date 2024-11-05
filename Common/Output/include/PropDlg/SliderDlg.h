#pragma once


// CSliderDlg dialog
#include <afxcmn.h>
#include "afxwin.h"
#include "AVariant.h"

class CPropertySliderBox;

class CXSliderDlg  : public CDialog
{
	DECLARE_DYNAMIC(CXSliderDlg)

public:
	CXSliderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CXSliderDlg();

	void SetValue(AVariant value);
	void Init(CPropertySliderBox *pBox, AVariant mMin, AVariant mMax, AVariant mStep);
	void Exit();
// Dialog Data
	enum { IDD = IDD_DIALOG_XSLIDER };
	CSliderCtrl	m_SliderCtrl;


protected:
	CPropertySliderBox* m_pSliderBox;
	AVariant m_Value,m_OldValue;
	float m_fMin;
	float m_fMax;
	float m_fStep;
	

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	CStatic m_StaticMin;
	CStatic m_StaticMax;
	CStatic m_StaticPos;
};
