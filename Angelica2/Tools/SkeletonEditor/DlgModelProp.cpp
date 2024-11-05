// DlgModelProp.cpp : implementation file
//

#include "Global.h"
#include "SkeletonEditor.h"
#include "DlgModelProp.h"
#include "Model.h"

#include "A3D.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgModelProp dialog

CDlgModelProp::CDlgModelProp(CModel* pModel, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModelProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgModelProp)
	m_strPropName = _T("");
	m_strPropVal = _T("");
	//}}AFX_DATA_INIT

	m_pModel	= pModel;
	m_bModified	= NULL;
}

void CDlgModelProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgModelProp)
	DDX_Control(pDX, IDC_LIST_PROP, m_PropList);
	DDX_Text(pDX, IDC_EDIT_PROPNAME, m_strPropName);
	DDX_Text(pDX, IDC_EDIT_PROPVALUE, m_strPropVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgModelProp, CDialog)
	//{{AFX_MSG_MAP(CDlgModelProp)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROP, OnItemchangedListProp)
	ON_BN_CLICKED(IDC_BTN_SUBMIT, OnBtnSubmit)
	ON_BN_CLICKED(IDC_BTN_ADDPROP, OnBtnAddprop)
	ON_BN_CLICKED(IDC_BTN_DELPROP, OnBtnDelprop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgModelProp message handlers

BOOL CDlgModelProp::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pModel);

	//	Initialize property list
	m_PropList.InsertColumn(0, "属性名", LVCFMT_LEFT, 140);
	m_PropList.InsertColumn(1, "属性值", LVCFMT_LEFT, 140);
	m_PropList.SetExtendedStyle(m_PropList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int iCount = 0;

	A3DSkinModel::PropTable& PropTable = m_pModel->GetA3DSkinModel()->GetPropertyTable();
	A3DSkinModel::PropTable::iterator it = PropTable.begin();
	for (; it != PropTable.end(); ++it)
	{
		const AString* pstr = it.key();
		int iItem = m_PropList.InsertItem(iCount++, *pstr);

		pstr = it.value();
		m_PropList.SetItemText(iItem, 1, *pstr);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgModelProp::OnItemchangedListProp(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem < 0)
		return;

	if (pNMListView->uNewState & LVIS_SELECTED)
	{
		//	Item is selected
		m_strPropName = m_PropList.GetItemText(pNMListView->iItem, 0);
		m_strPropVal = m_PropList.GetItemText(pNMListView->iItem, 1);
		UpdateData(FALSE);
	}
}

void CDlgModelProp::OnBtnSubmit() 
{
	if (!m_PropList.GetSelectedCount())
		return;

	UpdateData(TRUE);

	if (m_strPropName == "")
	{
		MessageBox("属性名称不能为空!");
		return;
	}

	POSITION pos = m_PropList.GetFirstSelectedItemPosition();
	int iItem = m_PropList.GetNextSelectedItem(pos);
	
	CString strOldName = m_PropList.GetItemText(iItem, 0);

	if (strOldName != m_strPropName)
	{
		if (!m_pModel->GetA3DSkinModel()->AddProperty((LPCTSTR)m_strPropName, (LPCTSTR)m_strPropVal))
		{
			MessageBox("修改失败，可能是属性名冲突!");
			return;
		}

		//	Delete old property
		m_pModel->GetA3DSkinModel()->DeleteProperty((LPCTSTR)strOldName);
	}
	else
		m_pModel->GetA3DSkinModel()->SetProperty((LPCTSTR)m_strPropName, (LPCTSTR)m_strPropVal);

	m_PropList.SetItemText(iItem, 0, m_strPropName);
	m_PropList.SetItemText(iItem, 1, m_strPropVal);

	m_bModified = true;
}

void CDlgModelProp::OnBtnAddprop() 
{
	UpdateData(TRUE);

	if (m_strPropName == "")
	{
		MessageBox("属性名称不能为空!");
		return;
	}
	
	if (!m_pModel->GetA3DSkinModel()->AddProperty((LPCTSTR)m_strPropName, (LPCTSTR)m_strPropVal))
	{
		MessageBox("添加失败，可能是属性名冲突!");
		return;
	}

	int iItem = m_PropList.InsertItem(m_PropList.GetItemCount(), m_strPropName);
	m_PropList.SetItemText(iItem, 1, m_strPropVal);

	m_bModified = true;
}

void CDlgModelProp::OnBtnDelprop() 
{
	if (!m_PropList.GetSelectedCount())
		return;

	POSITION pos = m_PropList.GetFirstSelectedItemPosition();
	int iItem = m_PropList.GetNextSelectedItem(pos);

	CString strName = m_PropList.GetItemText(iItem, 0);
	m_pModel->GetA3DSkinModel()->DeleteProperty((LPCTSTR)strName);

	m_PropList.DeleteItem(iItem);

	m_bModified = true;
}
