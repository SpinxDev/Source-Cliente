// DlgAddSkin.cpp : implementation file
//

#include "Global.h"
#include "Model.h"
#include "DlgAddSkin.h"
#include "A3D.h"
#include "AF.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgAddSkin dialog


CDlgAddSkin::CDlgAddSkin(CModel* pModel, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddSkin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddSkin)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pModel = pModel;
	m_bModifiedFlag = false;
}


void CDlgAddSkin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddSkin)
	DDX_Control(pDX, IDC_LIST_SKIN, m_SkinList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddSkin, CDialog)
	//{{AFX_MSG_MAP(CDlgAddSkin)
	ON_BN_CLICKED(IDC_BTN_ADDSKIN, OnBtnAddskin)
	ON_BN_CLICKED(IDC_BTN_REPLACESKIN, OnBtnReplaceskin)
	ON_BN_CLICKED(IDC_BTN_REMOVESKIN, OnBtnRemoveskin)
	ON_BN_CLICKED(IDC_BTN_DELETESKIN, OnBtnDeleteskin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddSkin message handlers

BOOL CDlgAddSkin::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT(m_pModel);

	//	Initialize joint list
	m_SkinList.InsertColumn(0, "索引", LVCFMT_LEFT, 50);
	m_SkinList.InsertColumn(1, "文件名", LVCFMT_LEFT, 240);

	m_SkinList.SetExtendedStyle(m_SkinList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	UpdateSkinList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddSkin::OnBtnAddskin() 
{
/*	CString strPath = g_szWorkDir;
	strPath += "Models";

	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= true;
	Params.szFileFilter	= "*.ski";
	Params.szPrompt		= "请选择一个或多个皮肤文件";
	Params.szRootDir	= strPath;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	CString strFile;
	char szNum[10];

	DWORD pos = FileDlg.GetStartPosition();
	while (pos)
	{
		strFile = FileDlg.GetNextFullPathName(&pos);

		//	Change skin
		if (!m_pModel->ChangeSkin(-1, strFile))
		{
			char szMsg[500];
			sprintf(szMsg, "添加模型皮肤 %s 失败!", strFile);
			MessageBox(szMsg);
			continue;
		}

		//	Update skin list
		int iIndex = m_SkinList.GetItemCount();
		A3DSkin* pA3DSkin = m_pModel->GetSkin(iIndex);
		ASSERT(pA3DSkin);

		itoa(iIndex, szNum, 10);
		iIndex = m_SkinList.InsertItem(iIndex, szNum);
		m_SkinList.SetItemText(iIndex, 1, pA3DSkin->GetFileName());
	}

	m_bModifiedFlag = true;
*/
	char szFilter[] = "Skin 文件(*.ski)|*.ski||";
	DWORD dwFlags = OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.ski", NULL, dwFlags, szFilter, NULL);

	FileDia.m_ofn.lpstrFile = (LPTSTR)malloc(8192);
	if (!FileDia.m_ofn.lpstrFile)
	{
		MessageBox("Not enough memory!");
		return;
	}
	
	FileDia.m_ofn.nMaxFile		= 8191;
	FileDia.m_ofn.lpstrFile[0]	= '\0';		//	This is necessary

	//	Use last path as default path
	CString strPath = g_szWorkDir;
	strPath += "Models";

	if (m_strLastDir.GetLength())
		FileDia.m_ofn.lpstrInitialDir = m_strLastDir;
	else
		FileDia.m_ofn.lpstrInitialDir = strPath;

	if (FileDia.DoModal() != IDOK)
	{
		free(FileDia.m_ofn.lpstrFile);
		return;
	}

	CString strFile;
	char szNum[10];

	POSITION Pos = FileDia.GetStartPosition();
	while (Pos)
	{
		strFile = FileDia.GetNextPathName(Pos);

		//	Change skin
		if (!m_pModel->ChangeSkin(-1, strFile))
		{
			char szMsg[500];
			sprintf(szMsg, "添加模型皮肤 %s 失败!", strFile);
			MessageBox(szMsg);
			continue;
		}

		//	Update skin list
		int iIndex = m_SkinList.GetItemCount();
		A3DSkin* pA3DSkin = m_pModel->GetSkin(iIndex);
		ASSERT(pA3DSkin);

		itoa(iIndex, szNum, 10);
		iIndex = m_SkinList.InsertItem(iIndex, szNum);
		m_SkinList.SetItemText(iIndex, 1, pA3DSkin->GetFileName());
	}

	if (strFile.GetLength())
	{
		char szPath[MAX_PATH];
		af_GetFilePath(strFile, szPath, MAX_PATH);
		m_strLastDir = szPath;
	}

	free(FileDia.m_ofn.lpstrFile);

	m_bModifiedFlag = true;
}

void CDlgAddSkin::OnBtnReplaceskin() 
{
/*	//	Get selected skin
	if (!m_SkinList.GetSelectedCount())
	{
		MessageBox("请先选择要替换的皮肤文件。");
		return;
	}

	POSITION pos = m_SkinList.GetFirstSelectedItemPosition();
	int iSel = m_SkinList.GetNextSelectedItem(pos);

	//	Let user select a skin file
	CString strPath = g_szWorkDir;
	strPath += "Models";

	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= false;
	Params.szFileFilter	= "*.ski";
	Params.szPrompt		= "请选择一个皮肤文件";
	Params.szRootDir	= strPath;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	//	Change skin
	if (!m_pModel->ChangeSkin(iSel, FileDlg.GetFullPathName()))
	{
		MessageBox("替换模型皮肤失败!");
	}

	//	Update skin list
	A3DSkin* pA3DSkin = m_pModel->GetSkin(iSel);
	if (pA3DSkin)
		m_SkinList.SetItemText(iSel, 1, pA3DSkin->GetFileName());
	else
		m_SkinList.SetItemText(iSel, 1, "null");

	m_bModifiedFlag = true;
*/
	POSITION pos = m_SkinList.GetFirstSelectedItemPosition();
	if (!pos)
	{
		MessageBox("请先选择要替换的皮肤!");
		return;
	}

	int iSel = m_SkinList.GetNextSelectedItem(pos);

	//	Let user select a portal (.prt) file
	char szFilter[] = "Skin 文件(*.ski)|*.ski||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.ski", NULL, dwFlags, szFilter, NULL);

	//	Use last path as default path
	CString strPath = g_szWorkDir;
	strPath += "Models";

	if (m_strLastDir.GetLength())
		FileDia.m_ofn.lpstrInitialDir = m_strLastDir;
	else
		FileDia.m_ofn.lpstrInitialDir = strPath;
	
	if (FileDia.DoModal() != IDOK)
		return;

	//	Change skin
	if (!m_pModel->ChangeSkin(iSel, FileDia.GetPathName()))
	{
		MessageBox("替换模型皮肤失败!");
	}

	char szPath[MAX_PATH];
	af_GetFilePath(FileDia.GetPathName(), szPath, MAX_PATH);
	m_strLastDir = szPath;

	//	Update skin list
	A3DSkin* pA3DSkin = m_pModel->GetSkin(iSel);
	if (pA3DSkin)
		m_SkinList.SetItemText(iSel, 1, pA3DSkin->GetFileName());
	else
		m_SkinList.SetItemText(iSel, 1, "null");

	m_bModifiedFlag = true;
}

void CDlgAddSkin::OnBtnRemoveskin()
{
	//	Get selected skin
	if (!m_SkinList.GetSelectedCount())
	{
		MessageBox("请先选择要卸载的皮肤文件。");
		return;
	}

	POSITION pos = m_SkinList.GetFirstSelectedItemPosition();
	int iSel = m_SkinList.GetNextSelectedItem(pos);

	//	Remove skin
	m_pModel->ChangeSkin(iSel, NULL);

	//	Update skin list
	m_SkinList.SetItemText(iSel, 1, "null");

	m_bModifiedFlag = true;
}

void CDlgAddSkin::OnBtnDeleteskin() 
{
	//	Get selected skin
	if (!m_SkinList.GetSelectedCount())
	{
		MessageBox("请先选择要卸载的皮肤文件。");
		return;
	}

	if (MessageBox("彻底删除该项内容吗？", "提示", MB_YESNO) == IDNO)
		return;

	POSITION pos = m_SkinList.GetFirstSelectedItemPosition();
	int iSel = m_SkinList.GetNextSelectedItem(pos);

	//	Remove skin
	m_pModel->GetA3DSkinModel()->RemoveSkinItem(iSel);

	//	Update whole skin list
	UpdateSkinList();

	m_bModifiedFlag = true;
}

//	Update whole skin list
void CDlgAddSkin::UpdateSkinList()
{
	m_SkinList.DeleteAllItems();

	A3DSkinModel* pA3DSkinModel = m_pModel->GetA3DSkinModel();
	for (int i=0; i < pA3DSkinModel->GetSkinNum(); i++)
	{
		A3DSkin* pSkin = pA3DSkinModel->GetA3DSkin(i);

		char szNum[10];
		itoa(i, szNum, 10);

		if (pSkin)
		{
			int iItem = m_SkinList.InsertItem(i, szNum);
			m_SkinList.SetItemText(iItem, 1, pSkin->GetFileName());
		}
		else
		{
			int iItem = m_SkinList.InsertItem(i, szNum);
			m_SkinList.SetItemText(iItem, 1, "null");
		}
	}
}
