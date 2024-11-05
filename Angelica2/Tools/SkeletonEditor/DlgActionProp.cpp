// DlgActionProp.cpp : implementation file
//

#include "Global.h"
#include "DlgActionProp.h"
#include "SkeletonEditor.h"
#include "Model.h"
#include "Render.h"
#include "CommonFileName.h"

#include "A3D.h"
#include "AFI.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgActionProp dialog


CDlgActionProp::CDlgActionProp(CModel* pModel, A3DSkinModelActionCore* pAction, 
			bool bNewAction, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgActionProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgActionProp)
	m_strName = _T("");
	m_strTrackFile = _T("");
	//}}AFX_DATA_INIT

	m_pModel		= pModel;
	m_pAction		= pAction;
	m_bNewAction	= bNewAction;
}

void CDlgActionProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgActionProp)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_TRACKFILE, m_strTrackFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgActionProp, CDialog)
	//{{AFX_MSG_MAP(CDlgActionProp)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgActionProp message handlers

BOOL CDlgActionProp::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_pModel || !m_pModel)
	{
		ASSERT(m_pModel && m_pAction);
		return FALSE;
	}

	m_strName = m_pAction->GetName();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgActionProp::OnOK() 
{
	UpdateData(TRUE);

	if (!m_strName.GetLength())
	{
		MessageBox("请输入动作名字");
		return;
	}

	//	Check action name collision
	A3DSkinModelActionCore* pAction = m_pModel->GetA3DSkinModel()->GetAction(m_strName);
	if (pAction && pAction != m_pAction)
	{
		MessageBox("同名动作已经存在");
		return;
	}

	if (!m_strTrackFile.GetLength())
	{
		MessageBox("请选择一个 track 文件 (.stck)");
		return;
	}

	//	Get track file info
	SKELETONINFO TckInfo;
	if (!AUX_ReadTrackInfo(m_strTrackFile, TckInfo))
	{
		MessageBox("读取 track 文件失败!");
		return;
	}

	//	Track file must match current skeleton
	A3DSkeleton* pSkeleton = m_pModel->GetA3DSkinModel()->GetSkeleton();
	if (pSkeleton->GetAnimFPS() != TckInfo.iAnimFPS ||
		pSkeleton->GetBoneNum() != TckInfo.iNumBone)
	{
		MessageBox("选区的 track 文件和当前的骨骼文件不匹配 !");
		return;
	}

	m_pAction->SetName(m_strName);
	m_pAction->SetTrackSetFileName(m_strTrackFile, true);

	if (m_bNewAction)
	{
		//	Set frame infomation to action
		m_pAction->SetStartAndEndFrame((float)TckInfo.iStartFrame, (float)TckInfo.iEndFrame);
	}
	else	//	Edit action
	{
		//	Clamp start and end frame
		int f1 = m_pAction->GetStartFrame();
		int f2 = m_pAction->GetEndFrame();
		a_Clamp(f1, TckInfo.iStartFrame, TckInfo.iEndFrame);
		a_Clamp(f2, TckInfo.iStartFrame, TckInfo.iEndFrame);
		m_pAction->SetStartAndEndFrame((float)f1, (float)f2);
	}

	CDialog::OnOK();
}

void CDlgActionProp::OnBtnBrowse() 
{
	//	Get current model's path as basic path
	char szPath[MAX_PATH], szFullPath[MAX_PATH];
	AUX_GetModelTrackDir(m_pModel->GetA3DSkinModel(), szPath, false);
	AUX_GetModelTrackDir(m_pModel->GetA3DSkinModel(), szFullPath, true);

	if (!AUX_DirectoryExist(szFullPath))
	{
		MessageBox("Track 文件目录不存在，请先使用 '编辑->动作加工厂' 对模型进行编辑。");
		return;
	}

	CCommonFileName FileDlg(g_Render.GetA3DEngine(), 0, szPath);
	FileDlg.SetFileExtension("stck");
	if (FileDlg.DoModal() != IDOK)
		return;

	//	Update track file
	m_strTrackFile = FileDlg.GetFullFileName();
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_TRACKFILE, m_strTrackFile);
}
