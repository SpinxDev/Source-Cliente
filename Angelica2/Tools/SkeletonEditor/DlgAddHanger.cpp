// DlgAddHanger.cpp : implementation file
//

#include "Global.h"
#include "DlgAddHanger.h"
#include "DlgSelectFile.h"
#include "Model.h"
#include "Hook.h"
#include "Render.h"
#include "A3D.h"
#include "AF.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgAddHanger dialog


CDlgAddHanger::CDlgAddHanger(CModel* pModel, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddHanger::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddHanger)
	m_strFileName = _T("");
	m_strName = _T("");
	//}}AFX_DATA_INIT

	m_pModel = pModel;
	m_iHangerType = A3DHanger::TYPE_UNKNOWN;
	m_strCC = "";
}


void CDlgAddHanger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddHanger)
	DDX_Control(pDX, IDC_COMBO_CC, m_CCCombo);
	DDX_Control(pDX, IDC_COMBO_HANGPOINTS, m_HangPtCombo);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddHanger, CDialog)
	//{{AFX_MSG_MAP(CDlgAddHanger)
	ON_BN_CLICKED(IDC_BTN_GFX, OnBtnGfx)
	ON_BN_CLICKED(IDC_BTN_MODEL, OnBtnModel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddHanger message handlers

BOOL CDlgAddHanger::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pModel);
	
	int i, iItem;

	//	Add hooks
	for (i=0; i < m_pModel->GetHookNum(); i++)
	{
		A3DSkeletonHook* pA3DHook = m_pModel->GetHook(i)->GetA3DHook();
		if (pA3DHook->GetHookType() == A3DSkeletonHook::HOOK_HH)
		{
			iItem = m_HangPtCombo.InsertString(-1, pA3DHook->GetName());
			m_HangPtCombo.SetItemData(iItem, 0);
		}
	}

	//	Add bones
	A3DSkeleton* pSkeleton = m_pModel->GetSkeleton();

	for (i=0; i < pSkeleton->GetBoneNum(); i++)
	{
		A3DBone* pBone = pSkeleton->GetBone(i);
		iItem = m_HangPtCombo.InsertString(-1, pBone->GetName());
		m_HangPtCombo.SetItemData(iItem, 1);
	}

	m_HangPtCombo.SetCurSel(0);

	m_CCCombo.EnableWindow(FALSE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddHanger::OnOK() 
{
	UpdateData(TRUE);

	if (m_iHangerType == A3DHanger::TYPE_UNKNOWN || m_strFileName == "")
	{
		MessageBox("请选择一个悬挂物。");
		return;
	}

	if (m_iHangerType == A3DHanger::TYPE_SKINMODEL)
	{
		int iSel = m_CCCombo.GetCurSel();
		if (iSel == CB_ERR)
		{
			MessageBox("选择的模型没有悬挂中心。");
			return;
		}

		m_CCCombo.GetLBText(iSel, m_strCC);
	}

	int iHangPt = m_HangPtCombo.GetCurSel();
	m_bOnBone = m_HangPtCombo.GetItemData(iHangPt) ? true : false;

	CString strText;
	m_HangPtCombo.GetLBText(iHangPt, strText);
	m_strBindTo = strText;

	CDialog::OnOK();
}

void CDlgAddHanger::OnBtnGfx() 
{
	CString strPath = g_szWorkDir;
	strPath += "gfx";

	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= false;
	Params.szFileFilter	= "*.gfx";
	Params.szPrompt		= "请选择一个 gfx 文件";
	Params.szRootDir	= strPath;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	m_iHangerType = A3DHanger::TYPE_GFX;
	m_strFileName = FileDlg.GetRelativePathName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_FILENAME, m_strFileName);

	ResetCCList();
}

void CDlgAddHanger::OnBtnModel() 
{
	CString strPath = g_szWorkDir;
	strPath += "Models";

	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= false;
	Params.szFileFilter	= "*.smd";
	Params.szPrompt		= "请选择一个 skin model 文件";
	Params.szRootDir	= strPath;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	AString strFile = FileDlg.GetFullPathName();
	af_GetRelativePath(strFile, strFile);

	m_iHangerType = A3DHanger::TYPE_SKINMODEL;
	m_strFileName = strFile;

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_FILENAME, m_strFileName);

	ResetCCList();
}

//	Reset connection center list
void CDlgAddHanger::ResetCCList()
{
	m_CCCombo.ResetContent();
	m_CCCombo.EnableWindow(FALSE);

	if (m_iHangerType == A3DHanger::TYPE_GFX)
		return;
	
	//	Load CC names
	A3DSkinModel Model;

	Model.Init(g_Render.GetA3DEngine());

	if (!Model.Load(m_strFileName))
	{
		Model.Release();
		MessageBox("加载模型失败!");
		return;
	}

	A3DSkeleton* pSkeleton = Model.GetSkeleton();
	if (!pSkeleton || !pSkeleton->GetHookNum())
	{
		Model.Release();
		return;
	}

	m_CCCombo.EnableWindow(TRUE);

	for (int i=0; i < pSkeleton->GetHookNum(); i++)
	{
		A3DSkeletonHook* pHook = pSkeleton->GetHook(i);
		if (pHook->GetHookType() == A3DSkeletonHook::HOOK_CC)
			m_CCCombo.InsertString(-1, pHook->GetName());
	}

	m_CCCombo.SetCurSel(0);

	Model.Release();
}


