// DlgBuildHoriCull.cpp : implementation file
//

#include "Global.h"
#include "DlgBuildHoriCull.h"
#include "Render.h"
#include "MainFrm.h"
#include "Terrain2ViewDoc.h"
#include "CommonFileName.h"
#include "TerrainSquare.h"

#include "afi.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildHoriCull dialog


CDlgBuildHoriCull::CDlgBuildHoriCull(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBuildHoriCull::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBuildHoriCull)
	m_strTrnFile = _T("");
	//}}AFX_DATA_INIT
}


void CDlgBuildHoriCull::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBuildHoriCull)
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strTrnFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBuildHoriCull, CDialog)
	//{{AFX_MSG_MAP(CDlgBuildHoriCull)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_BN_CLICKED(IDC_BUILD, OnBuild)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildHoriCull message handlers

BOOL CDlgBuildHoriCull::OnInitDialog()
{
	CDialog::OnInitDialog();
	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBuildHoriCull::OnBtnBrowse() 
{
	CCommonFileName FileDlg(g_Render.GetA3DEngine(), 0);
	FileDlg.SetFileExtension("trn2");
	if (FileDlg.DoModal() != IDOK)
		return;

	m_strTrnFile = FileDlg.GetFullFileName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_FILENAME, m_strTrnFile);
}

void CDlgBuildHoriCull::OnBuild() 
{
	if (!UpdateData(TRUE))
		return;

	if (!m_strTrnFile.GetLength())
	{
		MessageBox("请先选择一个地形文件");
		return;
	}

	//	Selected terrain couldn't be the one we are traveling
	char szFile[MAX_PATH];
	af_GetRelativePath(glb_GetMainFrame()->GetDocument()->GetTerrainFile(), szFile);
	if (!m_strTrnFile.CompareNoCase(szFile))
	{
		MessageBox("不能选择当前正在游览的地形");
		return;
	}

	CTerrainSquare TrnSquare;
	if (!TrnSquare.Build(m_strTrnFile, &m_Progress))
	{
		TrnSquare.Release();
		MessageBox("数据生成失败!");
		return;
	}

	//	Save data
	sprintf(szFile, "%s\\%s", af_GetBaseDir(), m_strTrnFile);
	af_ChangeFileExt(szFile, MAX_PATH, ".t2hc");

	if (!TrnSquare.Save(szFile))
	{
		TrnSquare.Release();
		MessageBox("数据保存失败!");
		return;
	}

	TrnSquare.Release();

	MessageBox("操作成功!");
}



