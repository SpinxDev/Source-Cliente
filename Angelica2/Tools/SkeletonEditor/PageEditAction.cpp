// PageEditAction.cpp : implementation file
//

#include "Global.h"
#include "Model.h"
#include "MainFrm.h"
#include "PageEditAction.h"
#include "SkeletonEditorDoc.h"
#include "InterfaceWrapper.h"
#include "Render.h"
#include "DlgActionProp.h"

#include <A3D.h>
#include <AF.h>

#define new A_DEBUG_NEW

#define FRAME_LINE		1
#define FRAME_PAGE		5

/////////////////////////////////////////////////////////////////////////////
// CPageEditAction dialog


CPageEditAction::CPageEditAction(CWnd* pParent /*=NULL*/)
	: CDialog(CPageEditAction::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageEditAction)
	m_strAnimInfo = _T("动画信息［无］");
	m_iLoopNum = 1;
	m_bUseChannel = FALSE;
	m_iCurFrame = 0;
	m_bUseAbsTrack = FALSE;
	//}}AFX_DATA_INIT

	m_pModel = NULL;
}


void CPageEditAction::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageEditAction)
	DDX_Control(pDX, IDC_SLIDER_PLAYSTEP, m_PlaySlider);
	DDX_Control(pDX, IDC_LIST_ACTION, m_ActionList);
	DDX_Control(pDX, IDC_LIST_JOINT, m_JointList);
	DDX_Text(pDX, IDC_STATIC_ANIMINFO, m_strAnimInfo);
	DDX_Text(pDX, IDC_EDIT_LOOPNUM, m_iLoopNum);
	DDX_Check(pDX, IDC_CHECK_ENABLECHANNEL, m_bUseChannel);
	DDX_Text(pDX, IDC_EDIT_CURFRAME, m_iCurFrame);
	DDX_Check(pDX, IDC_CHECK_USEABSTRACK, m_bUseAbsTrack);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageEditAction, CDialog)
	//{{AFX_MSG_MAP(CPageEditAction)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_JOINT, OnDblclkListJoint)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_JOINT, OnItemchangedListJoint)
	ON_BN_CLICKED(IDC_BTN_ADDACTION, OnBtnAddaction)
	ON_BN_CLICKED(IDC_BTN_DELACTION, OnBtnDelaction)
	ON_BN_CLICKED(IDC_BTN_STOPACTION, OnBtnStopaction)
	ON_BN_CLICKED(IDC_BTN_SELECTALL, OnBtnSelectall)
	ON_BN_CLICKED(IDC_BTN_REVERSESELECT, OnBtnReverseselect)
	ON_BN_CLICKED(IDC_BTN_UNSELECTALL, OnBtnUnselectall)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ACTION, OnItemchangedListAction)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ACTION, OnDblclkListAction)
	ON_BN_CLICKED(IDC_BTN_EXPORTACTS, OnBtnExportacts)
	ON_BN_CLICKED(IDC_BTN_IMPORTACTS, OnBtnImportacts)
	ON_BN_CLICKED(IDC_BTN_EXPORTCHANNEL, OnBtnExportchannel)
	ON_BN_CLICKED(IDC_BTN_PLAYACTION, OnBtnPlayaction)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_PLAYSTEP, OnBtnPlaystep)
	ON_BN_CLICKED(IDC_BTN_ABSTRACK, OnBtnAbstrack)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_ACTIVATEPAGE, OnActivatePage)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageEditAction message handlers

BOOL CPageEditAction::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//	Initialize joint list
	m_JointList.InsertColumn(JSI_JOINTNAME, "节点名称", LVCFMT_LEFT, 285);

	m_JointList.SetExtendedStyle(m_JointList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	//	Initialize action list
	m_ActionList.InsertColumn(ASI_NAME, "动作名", LVCFMT_LEFT, 135);
	m_ActionList.InsertColumn(ASI_STARTFRAME, "起始帧", LVCFMT_LEFT, 60);
	m_ActionList.InsertColumn(ASI_ENDFRAME, "结束帧", LVCFMT_LEFT, 60);
	m_ActionList.InsertColumn(ASI_TRACKFILE, "Track", LVCFMT_LEFT, 200);

	m_ActionList.SetExtendedStyle(m_ActionList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	//	Create input window
	if (!m_InputWnd.Create(NULL, NULL, WS_POPUP, CRect(0, 0, 50, 40), this, 0))
		return false;

	//	Create combo window
	if (!m_ComboWnd.Create(NULL, NULL, WS_POPUP, CRect(0, 0, 50, 80), this, 0))
		return false;

	m_PlaySlider.SetLineSize(FRAME_LINE);
	m_PlaySlider.SetPageSize(FRAME_PAGE);
	m_PlaySlider.SetRange(0, 100);

	EnablePlayControls(false);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Release resources
void CPageEditAction::Release()
{
	ClearAllInfo();

	m_pModel = NULL;
}

//	Clear all information in dialog
void CPageEditAction::ClearAllInfo()
{
	ReleaseJointList();
	m_ActionList.DeleteAllItems();
	UpdateAnimInfo(true);
}

//	Release joint list
void CPageEditAction::ReleaseJointList()
{
	for (int i=0; i < m_JointList.GetItemCount(); i++)
	{
		JOINTDATA* pJointData = (JOINTDATA*)m_JointList.GetItemData(i);
		delete pJointData;
	}

	m_JointList.DeleteAllItems();
}

//	Initialize joint list
bool CPageEditAction::ResetJointList()
{
	if (!m_pModel)
		return true;

	A3DSkeleton* pSkeleton = m_pModel->GetSkeleton();
	if (!pSkeleton)
		return false;
	
	int i, iNumBone = pSkeleton->GetBoneNum();
	
	for (i=0; i < iNumBone; i++)
	{
		A3DBone* pBone = pSkeleton->GetBone(i);
		A3DAnimJoint* pJoint = pBone->GetAnimJointPtr();
		ASSERT(pJoint);

		//	Joint name
		int iItem = m_JointList.InsertItem(i, pJoint->GetName());

		JOINTDATA* pJointData = new JOINTDATA;
		ASSERT(pJointData);

		//	Get joint index
		int iJoint;
		pSkeleton->GetJoint(pJoint->GetName(), &iJoint);

		pJointData->pJoint	= pJoint;
		pJointData->iJoint	= iJoint;
		pJointData->iBone	= i;

		//	Associate joint pointer
		m_JointList.SetItemData(iItem, (DWORD)pJointData);
	}

	return true;
}

//	Initialize action list
bool CPageEditAction::ResetActionList()
{
	if (!m_pModel)
		return true;

	if (!m_pModel->GetActionNum())
		return true;

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();

	int iCount = 0;
	CString str;
	AString strTitle;

	A3DSkinModelActionCore* pActCore = pA3DModel->GetFirstAction();
	while (pActCore)
	{
		//	Action name
		int iItem = m_ActionList.InsertItem(iCount, pActCore->GetName());
		//	Start frame
		str.Format("%.2f", pActCore->GetStartFrame());
		m_ActionList.SetItemText(iItem, ASI_STARTFRAME, str);
		//	End frame
		str.Format("%.2f", pActCore->GetEndFrame());
		m_ActionList.SetItemText(iItem, ASI_ENDFRAME, str);
		//	Track file
		af_GetFileTitle(pActCore->GetTrackSetFileName(), strTitle);
		m_ActionList.SetItemText(iItem, ASI_TRACKFILE, strTitle);
		//	Set item data
		m_ActionList.SetItemData(iItem, (DWORD)pActCore);

		pActCore = pA3DModel->GetNextAction();
		iCount++;
	}
	
	return true;
}

//	Update animation information
void CPageEditAction::UpdateAnimInfo(bool bClear)
{
	A3DSkeleton* pSkeleton = NULL;

	if (bClear || !m_pModel || !m_pModel->GetSkeleton())
		pSkeleton = NULL;
	else
		pSkeleton = m_pModel->GetSkeleton();

	if (!pSkeleton)
	{
		m_strAnimInfo.Format("动画信息［无］");
	}
	else
	{
		m_strAnimInfo.Format("动画信息［FPS: %d］", pSkeleton->GetAnimFPS());
	}

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_STATIC_ANIMINFO, m_strAnimInfo);
}

//	Bind model object with this dialog
bool CPageEditAction::BindModel(CModel* pModel)
{
	//	Clear old information
	if (m_pModel)
		ClearAllInfo();

	if (!pModel)
		return true;

	m_pModel = pModel;

	//	Reset joint list
	if (!ResetJointList())
		return false;

	//	Reset action list
	if (!ResetActionList())
		return false;

	//	Update animation information
	UpdateAnimInfo(false);

	return true;
}

//	Select joint actions
void CPageEditAction::SelectJointActions(int iSelectType)
{
	if (iSelectType == SELECT_ALL || iSelectType == SELECT_NONE)
	{
		BOOL bCheck = iSelectType == SELECT_ALL ? TRUE : FALSE;

		for (int i=0; i < m_JointList.GetItemCount(); i++)
		{
			if (m_JointList.GetCheck(i) != bCheck)
			{
				m_JointList.SetCheck(i, bCheck);
				CheckJointListItem(i);
			}
		}
	}
	else	//	SELECT_REVERSE
	{
		for (int i=0; i < m_JointList.GetItemCount(); i++)
		{
			m_JointList.SetCheck(i, !m_JointList.GetCheck(i));
			CheckJointListItem(i);
		}
	}
}

//	Check or uncheck a joint list item
void CPageEditAction::CheckJointListItem(int iItem)
{
	if (!m_pModel || iItem < 0)
		return;

	JOINTDATA* pJointData = (JOINTDATA*)m_JointList.GetItemData(iItem);

	if (m_JointList.GetCheck(iItem))
		m_pModel->SelectBone(pJointData->iBone, true);
	else
		m_pModel->SelectBone(pJointData->iBone, false);
}

//	Update joint list item check states
void CPageEditAction::UpdateJointListCheckStates()
{
	if (!m_pModel)
		return;

	for (int i=0; i < m_JointList.GetItemCount(); i++)
	{
		JOINTDATA* pJointData = (JOINTDATA*)m_JointList.GetItemData(i);

		bool bChecked = m_JointList.GetCheck(i) ? true : false;
		bool bSelected = m_pModel->BoneIsSelected(pJointData->iBone);

		if (bChecked != bSelected)
		{
			m_JointList.SetCheck(i, bSelected ? TRUE : FALSE);
			CheckJointListItem(i);
		}
	}
}

//	Update bone selection according to joint list checking states
void CPageEditAction::UpdateBoneSelection()
{
	if (!m_pModel)
		return;

	int i, iNumItem = m_JointList.GetItemCount();

	for (i=0; i < iNumItem; i++)
	{
		JOINTDATA* pJointData = (JOINTDATA*)m_JointList.GetItemData(i);

		if (m_JointList.GetCheck(i))
			m_pModel->SelectBone(pJointData->iBone, true);
		else
			m_pModel->SelectBone(pJointData->iBone, false);
	}
}

//	Change selected action
void CPageEditAction::ChangeSelectedAction(int iIndex, A3DSkinModelActionCore* pNewAction)
{
	if (!m_pModel)
		return;

	//	Stop all actions
	CSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();
	pSkinModel->SetCurFrame(-1.0f);
	pSkinModel->StopAllActions();

	//	Reset play slider
	A3DSkinModelActionCore* pActCore = GetSelectedAction();
	if (pActCore)
	{
		int iStart = (int)(pActCore->GetStartFrame() + 0.5f);
		int iEnd = (int)(pActCore->GetEndFrame() + 0.5f);
		m_PlaySlider.SetRange(iStart, iEnd);
		m_PlaySlider.SetPos(iStart);

		m_iCurFrame = iStart;
		CDataExchange dx(this, FALSE);
		DDX_Text(&dx, IDC_EDIT_CURFRAME, m_iCurFrame);
	}
}

//	Get selected action
A3DSkinModelActionCore* CPageEditAction::GetSelectedAction()
{
	if (!m_ActionList.GetSelectedCount())
		return NULL;

	POSITION pos = m_ActionList.GetFirstSelectedItemPosition();
	int iSel = m_ActionList.GetNextSelectedItem(pos);

	return (A3DSkinModelActionCore*)m_ActionList.GetItemData(iSel);
}

void CPageEditAction::OnDblclkListJoint(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NMITEMACTIVATE* pNMListView = (NMITEMACTIVATE*)pNMHDR;
	if (pNMListView->iItem < 0 || pNMListView->iSubItem == 0)
		return;

	bool bColumn = (GetKeyState(VK_SHIFT) & 0x8000) ? true : false;
	EditJointListItem(pNMListView->iItem, pNMListView->iSubItem, bColumn);
}

void CPageEditAction::OnItemchangedListJoint(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem < 0 || pNMListView->iSubItem != 0)
		return;

	bool bCheck1, bCheck2;

	bCheck1 = (pNMListView->uOldState & INDEXTOSTATEIMAGEMASK(2)) ? true : false;
	bCheck2 = (pNMListView->uNewState & INDEXTOSTATEIMAGEMASK(2)) ? true : false;
	
	if (bCheck1 == bCheck2)
		return;

	CheckJointListItem(pNMListView->iItem);
}

void CPageEditAction::OnBtnAddaction() 
{
	if (!m_pModel)
		return;

	CString strName;
	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();

	if (!pA3DModel || !pA3DModel->GetSkeleton())
		return;

	//	Only new version skin model can be edited
	if (pA3DModel->GetVersion() < 7)
	{
		MessageBox("当前打开的是老版本的模型文件，请先使用 '编辑->动作加工厂' 将其转换成新版本后再进行动作编辑。");
		return;
	}

	//	Create a unique name
	while (1)
	{
		strName.Format("新动作%d", rand());
		if (!pA3DModel->GetAction(strName))
			break;
	}

	A3DSkeleton* pSkeleton = pA3DModel->GetSkeleton();
	int iFrameRate = pSkeleton->GetAnimFPS();
	A3DSkinModelActionCore* pActionCore = new A3DSkinModelActionCore(iFrameRate);
	pActionCore->SetName(strName);

	//	Let user edit action's property
	CDlgActionProp PropDlg(m_pModel, pActionCore, true);
	if (PropDlg.DoModal() != IDOK)
	{
		delete pActionCore;
		return;
	}

	//	Add action to skin model
	A3DSkinModelMan* pSkinModelMan = g_Render.GetA3DEngine()->GetA3DSkinModelMan();
	if (!pSkinModelMan->AddAction(pA3DModel, pActionCore))
	{
		delete pActionCore;
		g_Log.Log("CPageEditAction::OnBtnAddaction, Failed to add action %s", strName);
		return;
	}

	CString str;
	char szTitle[MAX_PATH];

	//	Action name
	int iItem = m_ActionList.InsertItem(m_ActionList.GetItemCount(), pActionCore->GetName());
	//	Start frame
	str.Format("%.2f", pActionCore->GetStartFrame());
	m_ActionList.SetItemText(iItem, ASI_STARTFRAME, str);
	//	End frame
	str.Format("%.2f", pActionCore->GetEndFrame());
	m_ActionList.SetItemText(iItem, ASI_ENDFRAME, str);
	//	Track file
	af_GetFileTitle(pActionCore->GetTrackSetFileName(), szTitle, MAX_PATH);
	m_ActionList.SetItemText(iItem, ASI_TRACKFILE, szTitle);

	//	Item data
	m_ActionList.SetItemData(iItem, (DWORD)pActionCore);

	//	Select new action
	m_ActionList.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
	ChangeSelectedAction(iItem, pActionCore);

	AUX_GetMainFrame()->GetDocument()->SetModifiedFlag(TRUE);
}

void CPageEditAction::OnBtnDelaction() 
{
	if (!m_pModel || !m_ActionList.GetSelectedCount())
		return;

	//	Only new version skin model can be edited
	if (m_pModel->GetA3DSkinModel()->GetVersion() < 7)
	{
		MessageBox("当前打开的是老版本的模型文件，请先使用 '编辑->动作加工厂' 将其转换成新版本后再进行动作编辑。");
		return;
	}

	POSITION pos = m_ActionList.GetFirstSelectedItemPosition();
	int iSel = m_ActionList.GetNextSelectedItem(pos);

	if (AUX_MessageBox(MB_YESNO, "确实要删除选中的动作吗") == IDNO)
		return;

	A3DSkinModelActionCore* pActCore = (A3DSkinModelActionCore*)m_ActionList.GetItemData(iSel);
	A3DSkinModelMan* pSkinModelMan = g_Render.GetA3DEngine()->GetA3DSkinModelMan();
	pSkinModelMan->RemoveActionByName(m_pModel->GetA3DSkinModel(), pActCore->GetName());

	//	Remove from list box
	m_ActionList.DeleteItem(iSel);

	AUX_GetMainFrame()->GetDocument()->SetModifiedFlag(TRUE);
}

void CPageEditAction::OnBtnPlayaction() 
{
	if (!m_pModel || !UpdateData(TRUE))
		return;

	A3DSkinModelActionCore* pActCore = GetSelectedAction();
	if (!pActCore)
	{
		AUX_MessageBox(MB_OK, "请先选中要播放的动作");
		return;
	}

	int iChannel = A3DSkinModel::ACTCHA_DEFAULT;
	if (m_bUseChannel)
	{
		iChannel = ACTCHA_NORMAL;
		BuildActionChannel();
	}

	CSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();

	pSkinModel->SetOneStep(false);
	pSkinModel->PlayActionByName(
					pActCore->GetName(),	//	Action name
					iChannel,	//	Channel index
					m_iLoopNum,	//	Loop number
					200,	//	Translate time
					true,	//	Restart flag
					false,	//	Children play action flag
					m_bUseAbsTrack ? true : false);		//	Use absolute track flag

	EnablePlayControls(false);
}

void CPageEditAction::OnBtnPlaystep() 
{
	EnablePlayControls(true);
}

void CPageEditAction::OnBtnStopaction() 
{
	if (!m_pModel)
		return;

	A3DSkinModelActionCore* pActCore = GetSelectedAction();
	if (!pActCore)
		return;

	CSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();
	pSkinModel->SetCurFrame(-1.0f);
	pSkinModel->StopAllActions();
}

void CPageEditAction::OnBtnSelectall() 
{
	SelectJointActions(SELECT_ALL);
}

void CPageEditAction::OnBtnReverseselect() 
{
	SelectJointActions(SELECT_REVERSE);
}

void CPageEditAction::OnBtnUnselectall() 
{
	SelectJointActions(SELECT_NONE);
}

//	Edit joint list item
void CPageEditAction::EditJointListItem(int iItem, int iSubItem, bool bColumn)
{
/*	if (!m_JointList.GetItemCount())
		return;

	if (iSubItem == JSI_JOINTNAME)
		return;

	//	Get subitem rect
	CRect rc;
	m_JointList.GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rc);
	m_JointList.ClientToScreen(&rc);

	CString strText = m_JointList.GetItemText(iItem, iSubItem);

	int iDataType, iData;
	float fData;
	void* pData;

	switch (iSubItem)
	{
	case JSI_LOOPTIME:		//	Loop time

		iDataType = CWndInput::DATA_INTEGER;
		iData = atoi(strText);
		pData = (void*)(&iData);
		break;

	default:
		return;
	}

	m_EditStatus.bEditing	= true;
	m_EditStatus.iWndID		= IDC_LIST_JOINT;
	m_EditStatus.iItem		= bColumn ? -1 : iItem;
	m_EditStatus.iSubItem	= iSubItem;

	//	Popup input window
	m_InputWnd.Popup(IDD_EDITANIMATION, iDataType, pData, rc);
*/
}

//	Edit a action list item
void CPageEditAction::EditActionListItem(int iItem, int iSubItem)
{
	if (!m_ActionList.GetItemCount())
		return;

	//	Get subitem rect
	CRect rc;
	m_ActionList.GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rc);
	m_ActionList.ClientToScreen(&rc);

	CString strText = m_ActionList.GetItemText(iItem, iSubItem);

	int iDataType;
	float fData;
	void* pData;

	switch (iSubItem)
	{
	case ASI_NAME:		//	Action name

		iDataType = CWndInput::DATA_STRING;
		pData = (void*)(LPCTSTR)strText;
		break;

	case ASI_STARTFRAME:	//	Start frame
	case ASI_ENDFRAME:		//	End frame

		iDataType = CWndInput::DATA_FLOAT;
		fData = (float)atof(strText);
		pData = (void*)(&fData);
		break;

	case ASI_TRACKFILE:		//	Track couldn't be changed
	default:
		return;
	}

	m_EditStatus.bEditing	= true;
	m_EditStatus.iWndID		= IDC_LIST_ACTION;
	m_EditStatus.iItem		= iItem;
	m_EditStatus.iSubItem	= iSubItem;

	//	Popup input window
	m_InputWnd.Popup(IDD_EDITANIMATION, iDataType, pData, rc);
}

//	End editing
void CPageEditAction::EndEditItem(bool bCancel)
{
	if (!m_pModel || !m_EditStatus.bEditing)
		return;

	m_EditStatus.bEditing = false;

	if (bCancel)
		return;

	if (m_EditStatus.iWndID == IDC_LIST_JOINT)
	{
	/*	CString strResult = m_InputWnd.GetResultString();

		int iData;
		float fData;

		switch (m_InputWnd.GetDataType())
		{
		case CWndInput::DATA_INTEGER:

			iData = atoi(strResult);
			strResult.Format("%d", iData);
			break;

		case CWndInput::DATA_FLOAT:

			fData = (float)atof(strResult);
			strResult.Format("%.2f", fData);
			break;
		}
	
		m_JointList.SetItemText(m_EditStatus.iItem, m_EditStatus.iSubItem, strResult);
	*/
	}
	else if (m_EditStatus.iWndID == IDC_LIST_ACTION)
	{
		CString strResult = m_InputWnd.GetResultString();

		int iData;
		float fData;

		switch (m_InputWnd.GetDataType())
		{
		case CWndInput::DATA_INTEGER:

			iData = atoi(strResult);
			strResult.Format("%d", iData);
			break;

		case CWndInput::DATA_FLOAT:

			fData = (float)atof(strResult);
			strResult.Format("%.2f", fData);
			break;

		case CWndInput::DATA_STRING:

			break;
		}

		if (!strResult.GetLength())
			return;

		A3DSkinModelActionCore* pActCore = (A3DSkinModelActionCore*)m_ActionList.GetItemData(m_EditStatus.iItem);
	
		if (m_EditStatus.iSubItem == ASI_NAME)
		{
			A3DSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();
			A3DSkinModelActionCore* pCmpAction = pSkinModel->GetAction(strResult);

			if (pCmpAction && pCmpAction != pActCore)
			{
				AUX_MessageBox(MB_OK, "同名动作已经存在");
				return;
			}

			pActCore->SetName(strResult);
		}
		else if (m_EditStatus.iSubItem == ASI_STARTFRAME)
		{
			pActCore->SetStartAndEndFrame(fData, pActCore->GetEndFrame());
		}
		else if (m_EditStatus.iSubItem == ASI_ENDFRAME)
		{
			pActCore->SetStartAndEndFrame(pActCore->GetStartFrame(), fData);
		}

		m_ActionList.SetItemText(m_EditStatus.iItem, m_EditStatus.iSubItem, strResult);

		AUX_GetMainFrame()->GetDocument()->SetModifiedFlag(TRUE);
	}
}

void CPageEditAction::OnItemchangedListAction(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem < 0 || pNMListView->iSubItem != 0)
		return;

	bool bSel1 = (pNMListView->uOldState & LVIS_SELECTED) ? true : false;
	bool bSel2 = (pNMListView->uNewState & LVIS_SELECTED) ? true : false;

	if (bSel1 != bSel2 && bSel2)
		ChangeSelectedAction(pNMListView->iItem, NULL);
}

void CPageEditAction::OnDblclkListAction(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NMITEMACTIVATE* pNMListView = (NMITEMACTIVATE*)pNMHDR;
	if (pNMListView->iItem < 0)
		return;

	EditActionListItem(pNMListView->iItem, pNMListView->iSubItem);
}

BOOL CPageEditAction::PreTranslateMessage(MSG* pMsg) 
{
	//	Disable 'return' and 'escape' because these keys will close dialog
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

//	On activate / disactivate this page
void CPageEditAction::OnActivatePage(WPARAM wParam, LPARAM lParam)
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

void CPageEditAction::OnBtnExportacts() 
{
	if (!m_pModel)
		return;

	A3DSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();
	char szTitle[MAX_PATH];

	//	Only new version skin model can be edited
	if (m_pModel->GetA3DSkinModel()->GetVersion() < 7)
	{
		MessageBox("当前打开的是老版本的模型文件，请先使用 '编辑->动作加工厂' 将其转换成新版本后再进行动作编辑。");
		return;
	}

	//	Let user select a text file
	char szFilter[] = "Text Files (*.txt)|*.txt||";
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(FALSE, "*.txt", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;

	FILE* fp = fopen(FileDia.GetPathName(), "wt");
	if (!fp)
		return;

	fprintf(fp, "//  A3DSkinModel action list file\n\n");

	//	Write version
	int iVersion = ACTFILE_VERSION;
	fprintf(fp, "version %d\n\n", iVersion);

	//	Get first action
	A3DSkinModelActionCore* pActCore = pSkinModel->GetFirstAction();
	while (pActCore)
	{
		fprintf(fp, "\"%s\"\n{\n", pActCore->GetName());
		fprintf(fp, "    %f  %f\n", pActCore->GetStartFrame(), pActCore->GetEndFrame());

		af_GetFileTitle(pActCore->GetTrackSetFileName(), szTitle, MAX_PATH);
		fprintf(fp, "    \"%s\"\n", szTitle);

		fprintf(fp, "}\n\n");

		//	Get next action
		pActCore = pSkinModel->GetNextAction();
	}

	fclose(fp);
}

void CPageEditAction::OnBtnImportacts()
{
	if (!m_pModel)
		return;

	A3DSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();
	if (!pSkinModel || !pSkinModel->GetSkeleton())
		return;

	//	Only new version skin model can be edited
	if (m_pModel->GetA3DSkinModel()->GetVersion() < 7)
	{
		MessageBox("当前打开的是老版本的模型文件，请先使用 '编辑->动作加工厂' 将其转换成新版本后再进行动作编辑。");
		return;
	}

	int iNumBone = pSkinModel->GetSkeleton()->GetBoneNum();

	//	Let user select a text file
	char szFilter[] = "Text Files (*.txt)|*.txt||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.txt", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;

	AScriptFile File;
	if (!File.Open(FileDia.GetPathName()))
	{
		MessageBox("读取文件失败!");
		return;
	}

	AString strName;
	A3DSkinModelMan* pSkinModelMan = g_Render.GetA3DEngine()->GetA3DSkinModelMan();
	int iVersion = 1;

	if (!File.PeekNextToken(true))
		return;

	//	Read version
	if (!stricmp(File.m_szToken, "version"))
	{
		File.GetNextToken(true);	//	Skip 'version'
		iVersion = File.GetNextTokenAsInt(false);
	}

	if (iVersion < 3)
	{
		File.Close();
		MessageBox("该动作文件版本太老，请使用 '编辑->动作加工厂' 生成新的动作列表。!");
		return;
	}

	//	Release all current actions
	ReleaseAllActions();

	//	Get current model's track file path
	char szTckPath[MAX_PATH], szTckFile[MAX_PATH], szTitle[MAX_PATH];
	AUX_GetModelTrackDir(pSkinModel, szTckPath, false);

	while (File.GetNextToken(true))
	{
		strName = File.m_szToken;

		if (!File.MatchToken("{", true))
			break;

		A3DSkinModelActionCore* pActionCore = pSkinModelMan->NewActionCore(pSkinModel->GetSkeleton()->GetAnimFPS(), strName);

		//	Frame info
		float f1 = File.GetNextTokenAsFloat(true);
		float f2 = File.GetNextTokenAsFloat(false);
		pActionCore->SetStartAndEndFrame(f1, f2);

		if (iVersion >= 3)
		{
			//	Track file
			File.GetNextToken(true);
			strcpy(szTitle, File.m_szToken);
			
			if (szTckPath[0])
				sprintf(szTckFile, "%s\\%s", szTckPath, szTitle);

			pActionCore->SetTrackSetFileName(szTckFile, true);
		}

		if (!File.MatchToken("}", true))
		{
			delete pActionCore;
			break;
		}

		//	Check action name collision
		pSkinModelMan->AddAction(pSkinModel, pActionCore);

		//	Update action list
		int iItem = m_ActionList.InsertItem(m_ActionList.GetItemCount(), strName);
		if (iItem >= 0)
		{
			CString str;

			//	Action name
			m_ActionList.SetItemText(iItem, ASI_NAME, pActionCore->GetName());
			//	Start frame
			str.Format("%.2f", pActionCore->GetStartFrame());
			m_ActionList.SetItemText(iItem, ASI_STARTFRAME, str);
			//	End frame
			str.Format("%.2f", pActionCore->GetEndFrame());
			m_ActionList.SetItemText(iItem, ASI_ENDFRAME, str);
			//	Track file
			m_ActionList.SetItemText(iItem, ASI_TRACKFILE, szTitle);
			//	Item data
			m_ActionList.SetItemData(iItem, (DWORD)pActionCore);
		}
	}

	File.Close();

	AUX_GetMainFrame()->GetDocument()->SetModifiedFlag(TRUE);
}

//	Search action item by action's name
int CPageEditAction::GetActionItem(const char* szName)
{
	CString strActName;

	for (int i=0; i < m_ActionList.GetItemCount(); i++)
	{
		strActName = m_ActionList.GetItemText(i, ASI_NAME);
		if (!strActName.CompareNoCase(szName))
			return i;
	}

	return -1;
}

//	Release all actions
void CPageEditAction::ReleaseAllActions()
{
	if (!m_pModel)
		return;

	CString strActName;
	A3DSkinModelMan* pSkinModelMan = g_Render.GetA3DEngine()->GetA3DSkinModelMan();
	A3DSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();

	pSkinModel->StopAllActions();

	for (int i=0; i < m_ActionList.GetItemCount(); i++)
	{
		strActName = m_ActionList.GetItemText(i, ASI_NAME);
		pSkinModelMan->RemoveActionByName(pSkinModel, strActName);
	}

	m_ActionList.DeleteAllItems();
}

//	Build action channel
void CPageEditAction::BuildActionChannel()
{
	if (!m_pModel)
		return;

	int iNumJoint = m_JointList.GetItemCount();
	AArray<int, int> aJoints(0, iNumJoint);

	for (int i=0; i < m_JointList.GetItemCount(); i++)
	{
		if (m_JointList.GetCheck(i))
		{
			JOINTDATA* pData = (JOINTDATA*)m_JointList.GetItemData(i);
			aJoints.Add(pData->iJoint);
		}
	}

	m_pModel->GetA3DSkinModel()->BuildActionChannel(ACTCHA_NORMAL, aJoints.GetSize(), aJoints.GetData());
}

void CPageEditAction::OnBtnExportchannel() 
{
	//	Let user select a text file
	char szFilter[] = "Text Files (*.txt)|*.txt||";
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(FALSE, "*.txt", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;

	FILE* fp = fopen(FileDia.GetPathName(), "wt");
	if (!fp)
		return;

	for (int i=0; i < m_JointList.GetItemCount(); i++)
	{
		if (m_JointList.GetCheck(i))
		{
			JOINTDATA* pData = (JOINTDATA*)m_JointList.GetItemData(i);
			fprintf(fp, "%d, ", pData->iJoint);
		}
	}

	fclose(fp);
}

void CPageEditAction::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ((DWORD)pScrollBar == (DWORD)&m_PlaySlider)
	{
		A3DSkinModelActionCore* pActCore = GetSelectedAction();
		int curpos = m_PlaySlider.GetPos();

		if (pActCore && nSBCode != TB_ENDTRACK)
		{
			CSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();
			pSkinModel->SetOneStep(true);
			pSkinModel->StepFrameTo((float)curpos, pActCore->GetName());
		}

		m_iCurFrame = curpos;
		CDataExchange dx(this, FALSE);
		DDX_Text(&dx, IDC_EDIT_CURFRAME, m_iCurFrame);
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//	Enable playing controls
void CPageEditAction::EnablePlayControls(bool bStepPlay)
{
	if (bStepPlay)
	{
		GetDlgItem(IDC_EDIT_LOOPNUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_PLAYSTEP)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_LOOPNUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_PLAYSTEP)->EnableWindow(FALSE);
	}
}

void CPageEditAction::OnBtnAbstrack()
{
	if (!m_pModel)
		return;

	AArray<int, int> aJoints;

	for (int i=0; i < m_JointList.GetItemCount(); i++)
	{
		if (m_JointList.GetCheck(i))
		{
			JOINTDATA* pData = (JOINTDATA*)m_JointList.GetItemData(i);
			aJoints.Add(pData->iJoint);
		}
	}

	A3DSkinModel* pA3DModel = m_pModel->GetA3DSkinModel();
	pA3DModel->ResetAbsTrack();
	pA3DModel->RegisterAbsTrack(aJoints.GetSize(), aJoints.GetData());
}
