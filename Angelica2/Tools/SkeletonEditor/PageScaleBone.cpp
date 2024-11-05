// PageScaleBone.cpp : implementation file
//

#include "Global.h"
#include "PageScaleBone.h"
#include "Model.h"
#include "A3D.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CPageScaleBone dialog


CPageScaleBone::CPageScaleBone(CWnd* pParent /*=NULL*/)
	: CDialog(CPageScaleBone::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageScaleBone)
	m_strWarning = _T("");
	//}}AFX_DATA_INIT

	m_pModel = NULL;
}


void CPageScaleBone::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageScaleBone)
	DDX_Control(pDX, IDC_LIST_BONES, m_BoneList);
	DDX_Text(pDX, IDC_STATIC_WARNING, m_strWarning);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageScaleBone, CDialog)
	//{{AFX_MSG_MAP(CPageScaleBone)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BONES, OnItemchangedListBones)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BONES, OnDblclkListBones)
	ON_BN_CLICKED(IDC_BTN_RESTORE, OnBtnRestore)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_ACTIVATEPAGE, OnActivatePage)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageScaleBone message handlers

BOOL CPageScaleBone::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_strWarning = "这里进行的骨骼缩放调整只用于观看其的效果，缩放结果并不会被保存到模型文件中。";

	//	Initialize joint list
	m_BoneList.InsertColumn(BSI_NAME, "骨头名称", LVCFMT_LEFT, 150);
	m_BoneList.InsertColumn(BSI_LOCALLENGTH, "局部长度", LVCFMT_LEFT, 60);
	m_BoneList.InsertColumn(BSI_LOCALTHICK, "局部粗细", LVCFMT_LEFT, 70);
	m_BoneList.InsertColumn(BSI_WHOLESCALE, "整体比例", LVCFMT_LEFT, 70);

	m_BoneList.SetExtendedStyle(m_BoneList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	//	Create input window
	if (!m_InputWnd.Create(NULL, NULL, WS_POPUP, CRect(0, 0, 50, 40), this, 0))
		return FALSE;

	//	Create combo window
	if (!m_ComboWnd.Create(NULL, NULL, WS_POPUP, CRect(0, 0, 50, 80), this, 0))
		return FALSE;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPageScaleBone::PreTranslateMessage(MSG* pMsg) 
{
	//	Disable 'return' and 'escape' because these keys will close dialog
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

//	Release resources
void CPageScaleBone::Release()
{
}

//	Bind model object with this dialog
bool CPageScaleBone::BindModel(CModel* pModel)
{
	//	Clear old information
	if (m_pModel)
		ClearAllInfo();

	if (!pModel)
		return true;

	m_pModel = pModel;

	//	Reset bone list
	UpdateBoneList();

	return true;
}

//	Clear all information in dialog
void CPageScaleBone::ClearAllInfo()
{
	m_BoneList.DeleteAllItems();

	UpdateData(FALSE);
}

//	On activate / disactivate this page
void CPageScaleBone::OnActivatePage(WPARAM wParam, LPARAM lParam)
{
	if (wParam)	//	Activate this page
	{
		UpdateBoneSelection();
	}
	else	//	Disactivate this page
	{
		if (m_pModel)
			m_pModel->ClearBoneSelection();
	}
}

//	Update bone selection according to joint list checking states
void CPageScaleBone::UpdateBoneSelection()
{
	if (!m_pModel)
		return;

	int i, iNumItem = m_BoneList.GetItemCount();

	for (i=0; i < iNumItem; i++)
	{
		if (m_BoneList.GetCheck(i))
			m_pModel->SelectBone(i, true);
		else
			m_pModel->SelectBone(i, false);
	}
}

//	Update bone list
void CPageScaleBone::UpdateBoneList()
{
	if (!m_pModel)
		return;

	A3DSkeleton* pSkeleton = m_pModel->GetA3DSkinModel()->GetSkeleton();
	if (!pSkeleton)
		return;

	int i, iNumBone = pSkeleton->GetBoneNum();
	char szTemp[100];

	for (i=0; i < iNumBone; i++)
	{
		A3DBone* pBone = pSkeleton->GetBone(i);

		int iItem = m_BoneList.InsertItem(i, pBone->GetName());

		//	Local length scale factor
		sprintf(szTemp, "%.4f", pBone->GetLocalLengthScale());
		m_BoneList.SetItemText(iItem, BSI_LOCALLENGTH, szTemp);

		//	Local thickness scale factor
		sprintf(szTemp, "%.4f", pBone->GetLocalThickScale());
		m_BoneList.SetItemText(iItem, BSI_LOCALTHICK, szTemp);

		//	Whole scale factor
		sprintf(szTemp, "%.4f", pBone->GetWholeScale());
		m_BoneList.SetItemText(iItem, BSI_WHOLESCALE, szTemp);

		//	Bind item data
		m_BoneList.SetItemData(iItem, (DWORD)pBone);
	}
}

//	Update bone list check states
void CPageScaleBone::UpdateBoneListCheckStates()
{
	if (!m_pModel)
		return;

	for (int i=0; i < m_BoneList.GetItemCount(); i++)
	{
		if (m_pModel->BoneIsSelected(i))
			m_BoneList.SetCheck(i, TRUE);
		else
			m_BoneList.SetCheck(i, FALSE);
	}
}

void CPageScaleBone::OnItemchangedListBones(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	if (!m_pModel)
		return;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem < 0 || pNMListView->iSubItem != 0)
		return;

	bool bCheck1, bCheck2;

	bCheck1 = (pNMListView->uOldState & INDEXTOSTATEIMAGEMASK(2)) ? true : false;
	bCheck2 = (pNMListView->uNewState & INDEXTOSTATEIMAGEMASK(2)) ? true : false;
	
	if (bCheck1 == bCheck2)
		return;

	m_pModel->SelectBone(pNMListView->iItem, bCheck2 ? true : false);
}

//	Edit a bone list item through input window
void CPageScaleBone::EditBoneListItem(int iItem, int iSubItem, bool bColumn)
{
	if (!m_BoneList.GetItemCount())
		return;

	if (iSubItem == BSI_NAME)
		return;

	//	Get subitem rect
	CRect rc;
	m_BoneList.GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rc);
	m_BoneList.ClientToScreen(&rc);

	CString strText = m_BoneList.GetItemText(iItem, iSubItem);

	int iDataType;
	float fData;
	void* pData;

	switch (iSubItem)
	{
	case BSI_LOCALLENGTH:	//	Scale factors
	case BSI_LOCALTHICK:
	case BSI_WHOLESCALE:

		iDataType = CWndInput::DATA_FLOAT;
		fData = (float)atof(strText);
		pData = (void*)(&fData);
		break;

	default:
		return;
	}

	m_EditStatus.bEditing	= true;
	m_EditStatus.iWndID		= IDC_LIST_BONES;
	m_EditStatus.iItem		= bColumn ? -1 : iItem;
	m_EditStatus.iSubItem	= iSubItem;

	//	Popup input window
	m_InputWnd.Popup(IDD_SCALEBONE, iDataType, pData, rc);
}

//	End editing
void CPageScaleBone::EndEditItem(bool bCancel)
{
	if (!m_EditStatus.bEditing)
		return;

	m_EditStatus.bEditing = false;

	if (bCancel)
		return;

	if (m_EditStatus.iWndID == IDC_LIST_BONES)
	{
		CString strResult = m_InputWnd.GetResultString();
		float fData;

		switch (m_InputWnd.GetDataType())
		{
		case CWndInput::DATA_FLOAT:

			fData = (float)atof(strResult);
			strResult.Format("%.4f", fData);

			if (m_EditStatus.iSubItem == BSI_LOCALLENGTH ||
				m_EditStatus.iSubItem == BSI_LOCALTHICK ||
				m_EditStatus.iSubItem == BSI_WHOLESCALE)
			{
				if (fData <= 0.0f)
					return;
			}

			break;
		}
	
		if (m_EditStatus.iItem < 0)		//	Edit whole column
		{
			//	Update all checked item
			for (int i=0; i < m_BoneList.GetItemCount(); i++)
			{
				if (m_BoneList.GetCheck(i))
				{
					m_BoneList.SetItemText(i, m_EditStatus.iSubItem, strResult);

					//	Update bone data
					UpdateBoneScaleData(i, m_EditStatus.iSubItem);
				}
			}
		}
		else
		{
			m_BoneList.SetItemText(m_EditStatus.iItem, m_EditStatus.iSubItem, strResult);

			//	Update bone data
			UpdateBoneScaleData(m_EditStatus.iItem, m_EditStatus.iSubItem);
		}
	}
}

//	Edit a bone list item through combo box
void CPageScaleBone::ComboBoneListItem(int iItem, int iSubItem, bool bColumn)
{
/*	if (iItem < 0 || iSubItem != BSI_LOCALSCALETYPE)
		return;

	//	Get subitem rect
	CRect rc;
	m_BoneList.GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rc);
	m_BoneList.ClientToScreen(&rc);

	//	Initialize combo-box window
	m_ComboWnd.ClearAllStrings();

	for (int i=0; i < 3; i++)
		m_ComboWnd.InsertString(i, l_aScaleTypeStrings[i]);

	//	Select item
	int iSel = 0;
	A3DBone* pBone = (A3DBone*)m_BoneList.GetItemData(iItem);
	switch (pBone->GetLocalScaleType())
	{
	case A3DBone::LST_NONE:			iSel = 0;	break;
	case A3DBone::LST_LENGTH:		iSel = 1;	break;
	case A3DBone::LST_THICKNESS:	iSel = 2;	break;
	}

	m_EditStatus.bEditing	= true;
	m_EditStatus.iWndID		= IDC_LIST_BONES;
	m_EditStatus.iItem		= bColumn ? -1 : iItem;
	m_EditStatus.iSubItem	= iSubItem;

	//	Popup input window
	m_ComboWnd.Popup(IDD_SCALEBONE, iSel, rc);
*/
}

//	End editing item through combo box
void CPageScaleBone::EndComboItem(bool bCancel)
{
/*	if (!m_EditStatus.bEditing)
		return;

	m_EditStatus.bEditing = false;

	if (bCancel)
		return;

	if (m_EditStatus.iWndID == IDC_LIST_BONES)
	{
		int iSel = m_ComboWnd.GetSelectedIndex();

		if (m_EditStatus.iItem < 0)		//	Edit whole column
		{
			//	Update all checked item
			for (int i=0; i < m_BoneList.GetItemCount(); i++)
			{
				if (m_BoneList.GetCheck(i))
				{
					m_BoneList.SetItemText(i, BSI_LOCALSCALETYPE, m_ComboWnd.GetSelectedString());

					//	Update bone data
					UpdateBoneScaleData(i, m_EditStatus.iSubItem);
				}
			}
		}
		else
		{
			m_BoneList.SetItemText(m_EditStatus.iItem, BSI_LOCALSCALETYPE, m_ComboWnd.GetSelectedString());

			//	Update bone data
			UpdateBoneScaleData(m_EditStatus.iItem, m_EditStatus.iSubItem);
		}
	}
*/
}

//	Double click bone list
void CPageScaleBone::OnDblclkListBones(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NMITEMACTIVATE* pNMListView = (NMITEMACTIVATE*)pNMHDR;
	if (pNMListView->iItem < 0 || pNMListView->iSubItem == 0)
		return;

	bool bColumn = (GetKeyState(VK_SHIFT) & 0x8000) ? true : false;

//	if (pNMListView->iSubItem == BSI_LOCALSCALETYPE)
//		ComboBoneListItem(pNMListView->iItem, pNMListView->iSubItem, bColumn);
//	else
		EditBoneListItem(pNMListView->iItem, pNMListView->iSubItem, bColumn);
}

//	Update bone scale data
void CPageScaleBone::UpdateBoneScaleData(int iItem, int iSubItem)
{
	A3DBone* pBone = (A3DBone*)m_BoneList.GetItemData(iItem);

	if (iSubItem == BSI_WHOLESCALE)
	{
		//	Get whole scale factor
		CString strText = m_BoneList.GetItemText(iItem, BSI_WHOLESCALE);
		float fScaleFactor = (float)atof(strText);
		pBone->SetWholeScale(fScaleFactor);
	}
	else if (iSubItem == BSI_LOCALLENGTH)
	{
		CString strText = m_BoneList.GetItemText(iItem, BSI_LOCALLENGTH);
		float fScale = (float)atof(strText);
		pBone->SetLocalScale(fScale, 0.0f);
	}
	else if (iSubItem == BSI_LOCALTHICK)
	{
		CString strText = m_BoneList.GetItemText(iItem, BSI_LOCALTHICK);
		float fScale = (float)atof(strText);
		pBone->SetLocalScale(0.0f, fScale);
	}
	
	m_pModel->GetSkeleton()->CalcFootOffset();
}

//	Restore all bones' states
void CPageScaleBone::OnBtnRestore() 
{
	if (!m_pModel || !m_pModel->GetA3DSkinModel()->GetSkeleton())
		return;

	if (MessageBox("你确定要恢复所有骨头的比例吗？", "提示", MB_YESNO) == IDNO)
		return;

	A3DSkeleton* pSkeleton = m_pModel->GetA3DSkinModel()->GetSkeleton();
	int i, iNumBone = pSkeleton->GetBoneNum();

	for (i=0; i < iNumBone; i++)
	{
		A3DBone* pBone = pSkeleton->GetBone(i);
		pBone->SetLocalScale(1.0f, 1.0f);
	}

	m_pModel->GetSkeleton()->CalcFootOffset();

	//	Reset bone list
	m_BoneList.DeleteAllItems();
	UpdateBoneList();
	UpdateBoneListCheckStates();
}


