// SkeletonEditorDoc.cpp : implementation of the CSkeletonEditorDoc class
//

#include "Global.h"
#include "SkeletonEditor.h"
#include "SkeletonEditorDoc.h"
#include "SkeletonEditorView.h"
#include "MainFrm.h"
#include "Model.h"
#include "Render.h"

#include "DlgSelectFile.h"
#include "DlgOption.h"
#include "DlgModelProp.h"
#include "DlgModelInfo.h"
#include "DlgAddSkin.h"

#include "A3D.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorDoc

IMPLEMENT_DYNCREATE(CSkeletonEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CSkeletonEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CSkeletonEditorDoc)
	ON_COMMAND(ID_EDIT_OPTION, OnEditOption)
	ON_COMMAND(ID_EDIT_CHANGESKIN, OnEditChangeskin)
	ON_COMMAND(ID_VIEW_MODELINFO, OnViewModelinfo)
	ON_COMMAND(ID_EDIT_MODELPROP, OnEditModelprop)
	ON_COMMAND(ID_EDIT_ADDPHYSIQUE, OnEditAddphysique)
	ON_COMMAND(ID_EDIT_REMOVEPHYSIQUE, OnEditRemovephysique)
	ON_COMMAND(ID_FILE_CLOSEDOC, OnFileClosedoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorDoc construction/destruction

extern CSkeletonEditorApp theApp;

CSkeletonEditorDoc::CSkeletonEditorDoc()
{
	m_pModel	= NULL;
}

CSkeletonEditorDoc::~CSkeletonEditorDoc()
{
}

BOOL CSkeletonEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

/*	CreateModel();

	if (AUX_GetMainFrame())
		AUX_GetMainFrame()->BindModel(m_pModel);
*/
	return TRUE;
}

BOOL CSkeletonEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	//	Create new model
	if (!CreateModel())
	{
		AUX_MessageBox(MB_OK, "创建模型失败！");
		return FALSE;
	}

	//	Load model file ...
	if (!m_pModel->Load(lpszPathName))
	{
		ReleaseModel();
		AUX_MessageBox(MB_OK, "加载模型文件失败！");
		return FALSE;
	}

	//	Bind model
	if (!AUX_GetMainFrame()->BindModel(m_pModel))
	{
		ReleaseModel();
		AUX_MessageBox(MB_OK, "BindModel 失败！");
		return FALSE;
	}

	//	Adjust camera position
	AUX_GetMainFrame()->GetView()->SetCameraPos(m_pModel->GetModelCenter(), m_pModel->GetModelRadius());

	return TRUE;
}

BOOL CSkeletonEditorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if (!m_pModel)
		return TRUE;

	if (!m_pModel->Save(lpszPathName))
	{
		AUX_MessageBox(MB_OK, "保存模型文件失败！");
		return FALSE;
	}

	SetModifiedFlag(FALSE);

	return TRUE;
//	return CDocument::OnSaveDocument(lpszPathName);
}

void CSkeletonEditorDoc::DeleteContents() 
{
	ReleaseModel();

	CMainFrame* pMainFrame = AUX_GetMainFrame();
	if (pMainFrame)
		pMainFrame->BindModel(NULL);
	
	CDocument::DeleteContents();
}

void CSkeletonEditorDoc::OnFileClosedoc() 
{
	DeleteContents();
}

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorDoc serialization

void CSkeletonEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorDoc diagnostics

#ifdef _DEBUG
void CSkeletonEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSkeletonEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorDoc commands

//	Create model object
bool CSkeletonEditorDoc::CreateModel()
{
	//	Release old model
	ReleaseModel();

	if (!g_Render.EngineIsReady())
		return true;

	if (!(m_pModel = new CModel))
	{
		g_Log.Log("CSkeletonEditorDoc::CreateModel, not enough memory !");
		return false;
	}

	if (!m_pModel->Init(&g_Render))
	{
		g_Log.Log("CSkeletonEditorDoc::CreateModel, Failed to initialize model !");
		return false;
	}

	return true;
}

//	Release model object
void CSkeletonEditorDoc::ReleaseModel()
{
	if (m_pModel)
	{
		m_pModel->Release();
		delete m_pModel;
		m_pModel = NULL;
	}
}

//	Change model skin
void CSkeletonEditorDoc::OnEditChangeskin() 
{
	if (!m_pModel)
		return;

	//	Ensure there is one skeleton
	if (!m_pModel->GetSkeleton())
	{
		AUX_MessageBox(MB_OK, "请先添加模型骨骼!");
		return;
	}

	CDlgAddSkin AddDlg(m_pModel);
	AddDlg.DoModal();

	if (AddDlg.GetModifiedFlag())
	{
		AUX_GetMainFrame()->GetToolTabWnd()->UpdateSkinInfo();
		SetModifiedFlag(TRUE);
	}

	//	Discard this frame
	AUX_GetApp()->DiscardFrame();
}

void CSkeletonEditorDoc::OnEditAddphysique() 
{
	if (!m_pModel)
		return;

	CString strPath = g_szWorkDir;
	strPath += "Models";

	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= false;
	Params.szFileFilter	= "*.phy";
	Params.szPrompt		= "请选择一个体型文件";
	Params.szRootDir	= strPath;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	//	Change skin
	if (!m_pModel->GetA3DSkinModel()->BindPhysique(FileDlg.GetFullPathName()))
	{
		AUX_MessageBox(MB_OK, "加载模型体型失败!");
		return;
	}

	AUX_GetMainFrame()->GetToolTabWnd()->UpdatePhysiqueInfo();

	SetModifiedFlag(TRUE);

	//	Discard this frame
	AUX_GetApp()->DiscardFrame();
}

void CSkeletonEditorDoc::OnEditRemovephysique() 
{
	if (!m_pModel)
		return;

	m_pModel->GetA3DSkinModel()->RemovePhysique();

	AUX_GetMainFrame()->GetToolTabWnd()->UpdatePhysiqueInfo();

	SetModifiedFlag(TRUE);
}

//	Frame move
bool CSkeletonEditorDoc::FrameMove(DWORD dwTime)
{
	if (m_pModel)
		m_pModel->FrameMove(dwTime);

	return true;
}

//	Render 
bool CSkeletonEditorDoc::Render(A3DViewport* pViewport)
{
	if (m_pModel)
		m_pModel->Render(pViewport);

	return true;
}

//	Option
void CSkeletonEditorDoc::OnEditOption() 
{
	CDlgOption OptionDlg(&g_Configs);
	if (OptionDlg.DoModal() == IDOK)
	{
		if (m_pModel)
			m_pModel->SetBoxAndStaffSize(g_Configs.fBoneBoxSize, g_Configs.fStaffRadius);

		//	Save configs
		AUX_SaveConfigs();
	}
}

BOOL CSkeletonEditorDoc::SaveModified() 
{
	return CDocument::SaveModified();
}

void CSkeletonEditorDoc::OnViewModelinfo() 
{
	if (!m_pModel)
		return;

	CDlgModelInfo InfoDlg(m_pModel);
	InfoDlg.DoModal();
}

void CSkeletonEditorDoc::OnEditModelprop() 
{
	if (!m_pModel)
		return;

	CDlgModelProp PropDlg(m_pModel);
	PropDlg.DoModal();

	if (PropDlg.GetModifiedFlag())
		SetModifiedFlag(TRUE);
}

