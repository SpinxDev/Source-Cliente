// CustomVectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PropDlg_rc.h"
#include "CustomVectorDlg.h"
#include <A3DVector.h>

/////////////////////////////////////////////////////////////////////////////
// CCustomVectorDlg dialog

static int SelectDialogTemplateID(size_t dimension)
{
	if (dimension == 2)
		return IDD_CUSTOM_VECTOR2_DLG;
	else if (dimension == 3)
		return IDD_CUSTOM_VECTOR3_DLG;
	else
		return IDD_CUSTOM_VECTOR4_DLG;
}

CCustomVectorDlg::CCustomVectorDlg(size_t dimension, float valueArr[], CWnd* pParent /*=NULL*/)
	: CDialog(SelectDialogTemplateID(dimension), pParent)
{
	if (dimension > 4)
		dimension = 4;
	m_dimension = dimension;

	for (size_t i=0; i<m_dimension; ++i)
	{
		m_valueArr[i] = valueArr[i];
	}

	//{{AFX_DATA_INIT(CCustomVectorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCustomVectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomVectorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	if (m_dimension >= 1)
		DDX_Text(pDX, IDC_EDIT_VECOTR_X, m_valueArr[0]);
	if (m_dimension >= 2)
		DDX_Text(pDX, IDC_EDIT_VECOTR_Y, m_valueArr[1]);
	if (m_dimension >= 3)
		DDX_Text(pDX, IDC_EDIT_VECOTR_Z, m_valueArr[2]);
	if (m_dimension >= 4)
		DDX_Text(pDX, IDC_EDIT_VECOTR_W, m_valueArr[3]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomVectorDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomVectorDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomVectorDlg message handlers
