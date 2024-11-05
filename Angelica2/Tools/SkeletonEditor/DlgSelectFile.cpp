// DlgSelectFile.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgSelectFile.h"
#include <shlwapi.h>

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectFile dialog


CDlgSelectFile::CDlgSelectFile(const PARAMS& Params, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectFile)
	m_strPrompt = _T("");
	//}}AFX_DATA_INIT

	ASSERT(Params.szRootDir);

	int iLen = strlen(Params.szRootDir);
	if (Params.szRootDir[iLen-1] == '\\')
		m_strRootDir = CString(Params.szRootDir, iLen-1);
	else
		m_strRootDir = Params.szRootDir;

	m_Params		= Params;
	m_strFilter		= Params.szFileFilter ? Params.szFileFilter : "*.*";
	m_strPrompt		= Params.szPrompt;

	m_strFileList	= "";
	m_iNumSelFile	= 0;
}

void CDlgSelectFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectFile)
	DDX_Control(pDX, IDC_TREE_DIRECTORY, m_DirTree);
	DDX_Control(pDX, IDC_LIST_FILE, m_FileList);
	DDX_Text(pDX, IDC_STATIC_PROMPT, m_strPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectFile, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectFile)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIRECTORY, OnSelchangedTreeDirectory)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, OnDblclkListFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectFile message handlers

BOOL CDlgSelectFile::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	Backup current directory
	GetCurrentDirectory(MAX_PATH, m_szCurPath);

	//	Initialize image list
	InitImageList();

	m_FileList.InsertColumn(0, "File name", LVCFMT_LEFT, 500);
	if (!m_Params.bMultiSelect)
	{
		DWORD dwStyle = ::GetWindowLong(m_FileList.m_hWnd, GWL_STYLE);
		::SetWindowLong(m_FileList.m_hWnd, GWL_STYLE, dwStyle | LVS_SINGLESEL);
	}

	//	Set root directory
	SetRootPath();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectFile::OnOK() 
{
	UpdateData(TRUE);
	
	HTREEITEM hItem = m_DirTree.GetSelectedItem();
	m_iNumSelFile = m_FileList.GetSelectedCount();

	if (!hItem || !m_iNumSelFile)
		return;

	//	Get full path and relative path
	GetItemPath(m_strFullPath, hItem, true);
	GetItemPath(m_strRelativePath, hItem, false);

	m_strFileList = "";

	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	while (pos)
	{
		int iItem = m_FileList.GetNextSelectedItem(pos);
		m_strFileList += m_FileList.GetItemText(iItem, 0) + "|";
	}

	CDialog::OnOK();
}

void CDlgSelectFile::OnDestroy() 
{
	//	Restore current directory
	SetCurrentDirectory(m_szCurPath);
	CDialog::OnDestroy();
}

//	Initialize image list
void CDlgSelectFile::InitImageList()
{
	m_Images.Create(16, 16, ILC_COLOR24, 128, 128);
	m_Images.SetBkColor(GetSysColor(COLOR_WINDOW));

	char szPath[MAX_PATH];
	GetSystemDirectory(szPath, MAX_PATH);

	SHFILEINFO Info;
	memset(&Info, 0, sizeof (Info));

	//	Icon of closed folder
	SHGetFileInfo(szPath, 0, &Info, sizeof (Info), SHGFI_ICON | SHGFI_SMALLICON);
	m_Images.Add(Info.hIcon);
	DestroyIcon(Info.hIcon);

	//	Icon of opended folder
	SHGetFileInfo(szPath, 0, &Info, sizeof (Info), SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON);
	m_Images.Add(Info.hIcon);
	DestroyIcon(Info.hIcon);

	m_DirTree.SetImageList(&m_Images, TVSIL_NORMAL);
}

//	Set root path
void CDlgSelectFile::SetRootPath()
{
	if (!SetCurrentDirectory(m_strRootDir))
		return;
	
	//	Clear directory tree
	m_DirTree.DeleteAllItems();

	SHFILEINFO Info;
	memset(&Info, 0, sizeof (Info));

	//	Get normal icon
	SHGetFileInfo(m_strRootDir, 0, &Info, sizeof (Info), SHGFI_ICON | SHGFI_SMALLICON);
	int iImage = m_Images.Add(Info.hIcon);
	DestroyIcon(Info.hIcon);

	//	Get opened icon
	SHGetFileInfo(m_strRootDir, 0, &Info, sizeof (Info), SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON);
	int iSelImage = m_Images.Add(Info.hIcon);
	DestroyIcon(Info.hIcon);
	
	//	Create root node
	HTREEITEM hRoot = m_DirTree.InsertItem(m_strRootDir, iImage, iSelImage);
	m_DirTree.SetItemData(hRoot, PathIsRoot(m_strRootDir) ? PTIT_DRIVER : PTIT_FOLDER);
	
	//	Recursively travel all sub directoies
	RecursiveDirectories(m_strRootDir, hRoot);

	m_DirTree.Expand(hRoot, TVE_EXPAND);
	m_DirTree.SelectItem(hRoot);
}

//	Recursively travel all sub directoies
void CDlgSelectFile::RecursiveDirectories(const char* szPath, HTREEITEM hParent)
{
	if (!::SetCurrentDirectory(szPath))
		return;

	CFileFind Find;
	BOOL bResult = Find.FindFile("*.*");

	while (bResult)
	{
		bResult = Find.FindNextFile();
		if (Find.IsDots() || !Find.IsDirectory())
			continue;

		HTREEITEM hItem = InsertPathItem(hParent, Find.GetFileName());

		//	Continue sub directories
		RecursiveDirectories(Find.GetFilePath(), hItem);
	}
}

//	Insert a path to directory tree
HTREEITEM CDlgSelectFile::InsertPathItem(HTREEITEM hParent, const char* szPath)
{
	int iImage=IMG_FOLDER, iSelImage=IMG_OPENEDFOLDER;

	HTREEITEM hItem = m_DirTree.InsertItem(szPath, iImage, iSelImage, hParent ? hParent : TVI_ROOT, TVI_LAST);
	m_DirTree.SetItemData(hItem, PTIT_FOLDER);
	
	return hItem;
}

//	When selected directory change
void CDlgSelectFile::OnSelchangedTreeDirectory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//	Get full path of this folder
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString strPath;
	GetItemPath(strPath, hItem, true);

	if (!SetCurrentDirectory(strPath))
		return;

	//	Display files of this folder
	m_FileList.DeleteAllItems();

	CFileFind Find;
	BOOL bResult = Find.FindFile(m_strFilter);

	while (bResult)
	{
		bResult = Find.FindNextFile();
		if (Find.IsDots() || Find.IsSystem() || Find.IsDirectory())
			continue;

		m_FileList.InsertItem(0, Find.GetFileName());
	}

	*pResult = 0;
}

//	Get item path
void CDlgSelectFile::GetItemPath(CString& strPath, HTREEITEM hItem, bool bFull)
{
	ASSERT(hItem);

	if (hItem == m_DirTree.GetRootItem())
	{
		if (bFull)	//	Get full path
			strPath = m_DirTree.GetItemText(hItem);
		else
			strPath = "";

		return;
	}

	strPath = m_DirTree.GetItemText(hItem);

	HTREEITEM hParent = m_DirTree.GetParentItem(hItem);
	while (hParent && hParent != m_DirTree.GetRootItem())
	{
		strPath = m_DirTree.GetItemText(hParent) + "\\" + strPath;
		hParent = m_DirTree.GetParentItem(hParent);
	}

	if (bFull)	//	Get full path
		strPath = m_DirTree.GetItemText(m_DirTree.GetRootItem()) + "\\" + strPath;
}

//	Get selected file's name
CString CDlgSelectFile::GetFileName()
{
	if (!m_iNumSelFile)
		return CString("");

	int n = m_strFileList.Find('|');

	return m_strFileList.Left(n);
}

//	Get selected file's relative path name
CString CDlgSelectFile::GetRelativePathName()
{
	if (!m_iNumSelFile)
		return CString("");

	if (m_strRelativePath == "")
		return GetFileName();
	else
		return m_strRelativePath + "\\" + GetFileName();
}

//	Get selected file's full path name
CString CDlgSelectFile::GetFullPathName()
{
	if (!m_iNumSelFile)
		return CString("");

	return m_strFullPath + "\\" + GetFileName();
}

//	For multi-selected style...
//	Get start position of file list
DWORD CDlgSelectFile::GetStartPosition()
{
	return (DWORD)(-1);
}

//	Get next file name in file list
CString CDlgSelectFile::GetNextFileName(DWORD* pdwPos)
{
	if (!m_iNumSelFile)
	{
		*pdwPos = 0;
		return CString("");
	}

	int iFrom = (*pdwPos == (DWORD)(-1)) ? 0 : (int)(*pdwPos) + 1;

	int iTo = m_strFileList.Find('|', iFrom);
	if (iTo < 0)	//	This should not happen
	{
		*pdwPos = 0;
		return CString("");
	}

	if (iTo == m_strFileList.GetLength()-1)
		*pdwPos = 0;
	else
		*pdwPos = (DWORD)iTo;

	return m_strFileList.Mid(iFrom, iTo-iFrom);
}

//	Get next relative path and name in file list
CString CDlgSelectFile::GetNextRelativePathName(DWORD* pdwPos)
{
	if (!m_iNumSelFile)
		return CString("");

	if (m_strRelativePath == "")
		return GetNextFileName(pdwPos);
	else
		return m_strRelativePath + "\\" + GetNextFileName(pdwPos);
}

//	Get next full path name in file list
CString CDlgSelectFile::GetNextFullPathName(DWORD* pdwPos)
{
	if (!m_iNumSelFile)
		return CString("");

	return m_strFullPath + "\\" + GetNextFileName(pdwPos);
}

//	Double click file
void CDlgSelectFile::OnDblclkListFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!m_Params.bMultiSelect && m_FileList.GetSelectedCount())
		OnOK();
	
	*pResult = 0;
}


