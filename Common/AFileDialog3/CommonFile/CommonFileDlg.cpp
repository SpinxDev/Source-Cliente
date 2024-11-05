// CommonFileDlg.cpp : implementation file
//

#include "stdafx.h"
#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
#include "CommonFileDlg.h"
#include "Render.h"
#include "Shlwapi.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//	Create a directory
static bool AUX_CreateDirectory(const TCHAR* szDir)
{
	CString strDir = szDir;
	int iLen = strDir.GetLength();

	if (iLen <= 3)
		return true;

	//	Clear the last '\\'
	if (strDir.GetAt(iLen-1) == '\\')
		strDir.SetAt(iLen-1, '\0');

	//	Save current directory
	TCHAR szCurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurDir);

	CString strTemp;

	int iPos = strDir.Find('\\', 0);
	while (iPos > 0)
	{
		strTemp = strDir.Mid(0, iPos);
		CreateDirectory(strTemp, NULL);
		iPos = strDir.Find('\\', iPos+1);
	}

	CreateDirectory(szDir, NULL);

	//	Restore current directory
	SetCurrentDirectory(szCurDir);

	return true;
}

char g_szAppPath[MAX_PATH];

/////////////////////////////////////////////////////////////////////////////
// CCommonFileDlg dialog

//////////////////////////////////////////////////////////////////////////

CString _strDirFile;

CCommonFileDlg::CCommonFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommonFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCommonFileDlg)
	m_fileName = _T("");
	//}}AFX_DATA_INIT

	m_curDir = "";
	m_curRoot = "";
	m_relRoot = "";
	m_ext = "";
	m_workRoot = "";
	difRel = "";

	m_pCurNode = NULL;
	m_pRootNode = NULL;
	
	m_pPackage = NULL;
	m_pSoundBufMan = NULL;
	m_pSoundBuf = NULL;

	bChanged = true;
	bsaved = false;
	set_null = false;
	flags = AFILE_ALLOWMULFILES;
}

CCommonFileDlg::CCommonFileDlg(A3DEngine* pA3DEngine, DWORD dwmfileFlag, CString sRelPath, CWnd* pParent /* = NULL */)
	: CDialog(CCommonFileDlg::IDD, pParent)
{
	m_pA3DEngine = pA3DEngine;

	m_curDir = "";
	m_curRoot = "";
	m_relRoot = "";
	m_ext = "";		

	GetAllWorkRoot(sRelPath);
	m_workRoot = m_arrWorkRoot.GetAt(0);
	difRel = m_arrWorkRoot.GetAt(0);

	m_pCurNode = NULL;
	m_pRootNode = NULL;
	
	m_pPackage = NULL;
	m_pSoundBufMan = NULL;
	m_pSoundBuf = NULL;

	bChanged = true;
	bsaved = false;
	set_null = false;
	flags = dwmfileFlag;
//	flags = AFILE_ALLOWMULFILES;

}

void CCommonFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommonFileDlg)
	DDX_Control(pDX, IDC_SORT, m_sortBut);
	DDX_Control(pDX, IDC_STATIC_PREV, m_sPrev);
	DDX_Control(pDX, IDC_UP, m_upBut);
	DDX_Control(pDX, IDC_STATIC_IMAGEINFO, m_strImageInfo);
	DDX_Control(pDX, IDC_STATIC_FILEINFO, m_strFileInfo);
	DDX_Control(pDX, IDC_FILELIST, m_lstFile);
	DDX_Control(pDX, IDC_DIRLIST, m_lstDir);
	DDX_Control(pDX, IDC_COMBOBOXEX_LOOKIN, m_cboLookIn);
	DDX_Control(pDX, IDC_COMBO_FILETYPE, m_cboFileType);
	DDX_Control(pDX, IDC_CHECK, m_preview);
	DDX_Control(pDX, IDC_BACKWARD, m_prevBut);
	DDX_Text(pDX, IDC_FILENAME, m_fileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommonFileDlg, CDialog)
	//{{AFX_MSG_MAP(CCommonFileDlg)
	ON_EN_CHANGE(IDC_FILENAME, OnChangeFilename)
	ON_NOTIFY(NM_CLICK, IDC_DIRLIST, OnClickDirlist)
	ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, OnDblclkFilelist)
	ON_BN_CLICKED(IDC_CHECK, OnPreview)
	ON_CBN_SELCHANGE(IDC_COMBO_FILETYPE, OnSelchangeComboFiletype)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_LOOKIN, OnSelchangeComboboxexLookin)
	ON_BN_CLICKED(IDC_UP, OnbtnUp)
	ON_BN_CLICKED(IDC_BACKWARD, OnbtnBack)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, OnOpen)
	ON_BN_CLICKED(IDC_SORT, OnSort)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILELIST, OnItemchangedFilelist)
	ON_BN_CLICKED(ID_SET_NULL, OnSetNull)
	//ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommonFileDlg message handlers

void CCommonFileDlg::OnChangeFilename() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_fileName = "";
	UpdatePrevRegion();
	m_strFileInfo.SetWindowText(_T(""));
	m_strImageInfo.SetWindowText(_T(""));
	UpdateData(FALSE);
}

void CCommonFileDlg::OnClickDirlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int hItem =((NMLISTVIEW*)pNMHDR)->iItem;
	if(hItem < 0)
		return;

	CString csRoot(af_GetBaseDir());
	DWORD data = (DWORD)m_lstDir.GetItemData(hItem);
	CString csWorkRoot(m_arrWorkRoot.GetAt((int)data));
	m_curDir = csRoot;
	if(csWorkRoot.GetLength() != 0)
		m_curDir += "\\" + csWorkRoot;
	m_workRoot = m_curDir;


	UpdateLookinCombo();
	
	if(OpenMode == 1)
	{
		UpdateDiskListCtrl(m_curDir);
		UpdatePrevRegion();
	}
	
	if(OpenMode == 2)
	{
		OpenFromPackage(m_curDir);
		if(m_pRootNode != NULL)
		{
			if(m_pRootNode != NULL)
			{
				UpdatePckListCtrl(m_pRootNode);
				UpdatePrevRegion();
			}
		}
	}

	m_backDir.Add(m_curDir);
	bChanged = true;

	*pResult = 0;
}

void CCommonFileDlg::OnDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int hItem = ((NMLISTVIEW*)pNMHDR)->iItem;
	if(hItem < 0)
		return;

	CString strName = m_lstFile.GetItemText(hItem, 0);

	if(OpenMode == 1)
	{
		if(hItem < m_arrDir.GetSize())
		{
			m_curDir += _T("\\");
			m_curDir += strName;

			UpdateLookinCombo();

			UpdateDiskListCtrl(m_curDir);
			UpdatePrevRegion();
		}
		else
			Open();
	}
	
	if(OpenMode == 2)
	{
		if(m_pCurNode != NULL)
		{
			CPckTreeNode* pChild = m_pCurNode->FindChild(AC2AS((LPCTSTR)strName));
			if(pChild != NULL)
			{
				if(pChild->m_bIsDirectory)
				{
					m_curDir += _T("\\");
					m_curDir += strName;

					UpdateLookinCombo();

					UpdatePckListCtrl(pChild);
					UpdatePrevRegion();
				}
				else
					Open();
			}
		}
	}
	
	m_backDir.Add(m_curDir);
	bChanged = true;

	*pResult = 0;
}

BOOL CCommonFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//InitEngine();
	//	Get current directory
	if (!AFD_IsInitialized())
		AFD_DllInitialize();

	GetModuleFileNameA(NULL,g_szAppPath, MAX_PATH); 
	AString temp(g_szAppPath);
	
	int position=temp.ReverseFind('\\'); 
    temp=temp.Left(position+1); 
	
	strcpy(g_szAppPath,temp);
	SetCurrentDirectoryA(g_szAppPath);

	m_sPrev.InitRenderWnd();
	m_pA3DEngine = CRender::GetInstance()->GetA3DEngine();

	//InitDialog
	TCHAR szModule[MAX_PATH];
	GetModuleFileName(NULL, szModule, MAX_PATH);
	LPTSTR szFileName = ::PathFindFileName(szModule);
	LPTSTR szExt = ::PathFindExtension(szFileName);
	szExt[0] = _T('\0');
	_strDirFile = szFileName;
	_strDirFile += _T(".dir");
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	bmpbutton[0].LoadBitmap(IDB_BACK);
	m_prevBut.SetBitmap(HBITMAP(bmpbutton[0]));
	bmpbutton[1].LoadBitmap(IDB_UP);
	m_upBut.SetBitmap(HBITMAP(bmpbutton[1]));
	bmpbutton[2].LoadBitmap(IDB_SORT);
	m_sortBut.SetBitmap(HBITMAP(bmpbutton[2]));

	m_imageList.Create(IDB_LIST, 16, 16, RGB(255, 0, 255));
	m_dirImageList.Create(IDB_DIR, 24, 24, RGB(255, 255, 255));

	m_lstFile.SetImageList(&m_imageList, LVSIL_SMALL);
	if(!flags)
	{
		LONG lStyle;
		lStyle = GetWindowLong(m_lstFile.m_hWnd, GWL_STYLE);
		lStyle |= LVS_SINGLESEL;
		SetWindowLong(m_lstFile.m_hWnd, GWL_STYLE, lStyle);
	}
	
	m_lstDir.SetImageList(&m_dirImageList, LVSIL_NORMAL);
	m_lstDir.SetBkColor(RGB(95, 95, 95));
	m_lstDir.SetTextBkColor(RGB(95, 95, 95));
	m_lstDir.SetTextColor(RGB(255, 255, 255));

	m_cboLookIn.SetImageList(&m_imageList);
	
	m_preview.SetCheck(TRUE);

	InitExt();

	if(!OpenSavedLog())
		if(!InitDir())
			return FALSE;

	AfxSetResourceHandle(m_hHostResourceHandle);

	SetTimer(1000,33,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCommonFileDlg::OnPreview() 
{
	// TODO: Add your control notification handler code here
	m_preview.SetCheck((m_preview.GetCheck()==TRUE) ? 1 : 0);

	if(m_preview.GetCheck())
		m_sPrev.ShowWindow(SW_SHOW);

	if(m_preview.GetCheck() && m_fileName != "")
	{
		GetRelCurRoot();
		CString sFileName = m_curDir + _T("\\") + m_fileName;

		int reti = m_fileName.ReverseFind('.');
		int len = m_fileName.GetLength();
		CString fileext = m_fileName.Right(len - reti);
		fileext.MakeLower();

		if(fileext.Find(_T(".bmp")) != -1 || 
			fileext.Find(_T(".tga")) != -1 ||
			fileext.Find(_T(".jpg")) != -1 ||
			fileext.Find(_T(".ecm")) != -1 ||
			fileext.Find(_T(".gfx")) != -1 ||
			fileext.Find(_T(".png")) != -1 ||
			fileext.Find(_T(".dds")) != -1)
		{
				LoadPrev(sFileName);
		}
		if(m_fileName.Find(_T("mp3")) != -1 || m_fileName.Find(_T("wav")) != -1)
		{
			m_pSoundBuf = m_pSoundBufMan->Load2DSound(AC2AS((LPCTSTR)(m_relRoot+_T("\\")+m_fileName)));
			if(m_pSoundBuf != NULL)
			{
				m_pSoundBuf->Stop();
				m_pSoundBuf->Play(0);
			}
		}
			
	}
	else
	{
		if(m_pSoundBuf != NULL)
			m_pSoundBuf->Stop();
		m_strFileInfo.SetWindowText(_AL(""));
		m_strImageInfo.SetWindowText(_AL(""));
		UpdatePrevRegion();
		
		if(!m_preview.GetCheck())
			m_sPrev.ShowWindow(SW_HIDE);
	}	
}

void CCommonFileDlg::OnSelchangeComboFiletype() 
{
	// TODO: Add your control notification handler code here

	if (m_strExt.IsEmpty())
	{
		int nIndex = m_cboFileType.GetCurSel();
		if(nIndex == 0)
			m_ext = "*.*";
		else
		{
			m_cboFileType.GetLBText(nIndex, m_ext);

			int nCount = m_ext.Find(')')-m_ext.Find('(')-1;
			m_ext = m_ext.Mid(m_ext.Find('(')+1, nCount);
		}
	}

	if(OpenMode == 1)
	{
		UpdateDiskListCtrl(m_curDir);
		UpdatePrevRegion();
	}

	if(OpenMode == 2)
	{
		UpdatePckListCtrl(m_pCurNode);	
		UpdatePrevRegion();
	}
}

void CCommonFileDlg::OnSelchangeComboboxexLookin() 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_cboLookIn.GetCurSel();

	if(OpenMode == 1)
	{
		for(int i=m_cboLookIn.GetCount()-1; i>nIndex; i--)
		{
			int nPos = m_curDir.ReverseFind('\\');
			m_curDir = m_curDir.Left(nPos);
		}
		
		UpdateDiskListCtrl(m_curDir);
		UpdatePrevRegion();
	}

	if(OpenMode == 2)
	{
		CPckTreeNode* pNode = m_pCurNode;
		for(int i=nIndex+1; i<m_cboLookIn.GetCount(); i++)
		{
			int nPos = m_curDir.ReverseFind('\\');
			m_curDir = m_curDir.Left(nPos);
			pNode = pNode->m_pParent;
		}
		UpdatePckListCtrl(pNode);
		UpdatePrevRegion();
	}
	
	m_backDir.Add(m_curDir);
	bChanged = true;

	for(int i=m_cboLookIn.GetCount()-1; i>nIndex; i--)
		m_cboLookIn.DeleteString(i);

	m_cboLookIn.SetCurSel(m_cboLookIn.GetCount()-1);	
}

void CCommonFileDlg::OnbtnUp() 
{
	// TODO: Add your control notification handler code here
	if(m_cboLookIn.GetCount()-1 != 0)
	{
		int nPos = m_curDir.ReverseFind('\\');
		m_curDir = m_curDir.Left(nPos);

		if(OpenMode == 1)
		{
			UpdateDiskListCtrl(m_curDir);
			UpdatePrevRegion();
		}
		
		if(OpenMode == 2)
		{
			CPckTreeNode* pNode = m_pCurNode->m_pParent;
			
			UpdatePckListCtrl(pNode);
			UpdatePrevRegion();
		}
		
		m_backDir.Add(m_curDir);
		bChanged = true;
		
		m_cboLookIn.DeleteString(m_cboLookIn.GetCount()-1);
		m_cboLookIn.SetCurSel(m_cboLookIn.GetCount()-1);

	}
	
}

void CCommonFileDlg::OnbtnBack() 
{
	// TODO: Add your control notification handler code here
	if(bChanged)
	{
		if(m_backDir.GetAt(m_backDir.GetSize()-1) != m_workRoot)
			m_backDir.RemoveAt(m_backDir.GetSize()-1);
		bChanged = false;
	}
	
	if(!bChanged)
	{
		if(m_backDir.GetSize() != 0)
		{
			m_curDir = m_backDir.GetAt(m_backDir.GetSize()-1);

			UpdateLookinCombo();

			if(OpenMode == 1)
			{
				UpdateDiskListCtrl(m_curDir);
				UpdatePrevRegion();
			}

			if(OpenMode == 2)
			{
				CPckTreeNode* pNode = m_pRootNode;
				for(int i=1; i<m_cboLookIn.GetCount(); i++)
				{
					CString sDir;
					m_cboLookIn.GetLBText(i, sDir);
					pNode = pNode->FindChild(AC2AS(sDir));
				}
				UpdatePckListCtrl(pNode);
				UpdatePrevRegion();
			}

			if(m_backDir.GetAt(m_backDir.GetSize()-1) != m_workRoot)
				m_backDir.RemoveAt(m_backDir.GetSize()-1);

		}
	}

}

bool CCommonFileDlg::OpenSavedLog()
{
	if (!m_pA3DEngine)
	{
		g_AFDllLog.Log("Error in %s, No engine initialized, make sure AFD_DllInitialize() is called", __FUNCTION__);
		return false;
	}

	m_pSoundBufMan = m_pA3DEngine->GetAMEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();

	m_curRoot = af_GetBaseDir();

#ifdef _UNICODE
	FILE* fpFile = _wfopen(_strDirFile, L"rb");
#else
	FILE* fpFile = fopen(_strDirFile, "rb");
#endif

	if(NULL == fpFile) return false;

	int nNum = 0;
	fread(&nNum, sizeof(int), 1, fpFile);
	char savelog[MAX_PATH];
	fread(savelog, sizeof(char), nNum, fpFile);
	savelog[nNum] = '\0';

	if (difRel.IsEmpty())
	{
		m_workRoot = m_curRoot;
		m_curDir = m_curRoot + savelog;
	}
	else if(strstr(savelog, AC2AS(difRel)) != NULL)
	{
		m_workRoot = m_curRoot + _T("\\") + m_workRoot;
		m_curDir = m_curRoot + savelog;
	}
	else
		return false;

	fclose(fpFile);

	if (!PathFileExists(m_workRoot))
	{
		AUX_CreateDirectory(m_workRoot);
	}

	if (!PathFileExists(m_curDir))
		m_curDir = m_workRoot;

	bsaved = true;

	AFilePackage* g_pAFilePackage = NULL;
	
	if(g_pAFilePackage == NULL)
	{
		// Read package files from disk
		OpenMode = 1;

		if(!OpenFromDisk(m_workRoot))
			return false;
	}
	else
	{
		m_pPackage = g_pAFilePackage;
		// Read package files from package
		OpenMode = 2;
		if(!OpenFromPackage(m_workRoot))
			return false;
	}
		
	m_backDir.Add(m_workRoot);

	return true;
}

bool CCommonFileDlg::InitDir()
{
	if (!m_pA3DEngine)
	{
		g_AFDllLog.Log("Error in %s, No engine initialized, make sure AFD_DllInitialize() is called", __FUNCTION__);
		return false;
	}

	m_pSoundBufMan = m_pA3DEngine->GetAMEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();

	m_curRoot = af_GetBaseDir();
	if(m_workRoot == "")
	{
		m_workRoot = m_curRoot;
	}
	else
	{
		m_workRoot = m_curRoot + _T("\\") + m_workRoot;
	}
	
	AFilePackage* g_pAFilePackage = NULL;

	if(g_pAFilePackage == NULL)
	{
		// Read package files from disk
		OpenMode = 1;

		if(!OpenFromDisk(m_workRoot))
			return false;
	}
	else
	{
		m_pPackage = g_pAFilePackage;
		// Read package files from package
		OpenMode = 2;
		if(!OpenFromPackage(m_workRoot))
			return false;
	}
		
	m_backDir.Add(m_workRoot);

	return true;
}

void CCommonFileDlg::InitExt()
{
	m_cboFileType.ResetContent();

	if (m_strExt.IsEmpty())
	{
		m_cboFileType.AddString(_AL("All Files (*.*)"));
		m_cboFileType.AddString(_AL("Texture Files (*.bmp, *.tga, *.jpg, *.dds)"));
		m_cboFileType.AddString(_AL("3D Objects (*.ecm, *.gfx)"));
		m_cboFileType.AddString(_AL("Sound Files (*.mp3, *.wav)"));
		m_cboFileType.AddString(_AL("Text Files(*.txt)"));

		m_cboFileType.SetCurSel(0);
		m_ext = "*.*";
	}
	else
	{
		m_cboFileType.AddString(m_strExt + "文件");
		m_cboFileType.SetCurSel(0);
		m_ext = m_strExt;
		m_ext.MakeLower();
	}
}

bool CCommonFileDlg::OpenFromDisk(CString sPath)
{
	if(!bsaved)
		m_curDir = sPath;

	UpdateLookinCombo();
	
	if(!UpdateDiskListCtrl(m_curDir))
		return false;

	if(!UpdateDiskDirCtrl(m_workRoot))
		return false;

	return true;
}

bool CCommonFileDlg::OpenFromPackage(CString sPath)
{
	if(sPath == "")
	{
		AfxMessageBox(_AL("Bad path name parameter for open pckfile!"));
		return false;
	}

	m_tnRoot.m_strName = sPath;
	m_tnRoot.m_bIsDirectory = TRUE;
	m_tnRoot.m_dwNodeData = 0;
	m_tnRoot.m_pParent = NULL;

	int nCount = m_pPackage->GetFileNumber();
	for(int i=0; i<nCount; i++)
	{
#if defined _ANGELICA21
		const AFilePackage::FILEENTRY_READ* fe = m_pPackage->GetFileEntryByIndex(i);
#else
		const AFilePackage::FILEENTRY* fe = m_pPackage->GetFileEntryByIndex(i);
#endif
		if (!fe)
		{
			AfxMessageBox(_AL("Cannot get file entry!"));
			ClosePckListCtrl();
			return FALSE;
		}

		if (!m_tnRoot.AddFullName(fe->szFileName))
		{
			m_pPackage->RemoveFile(fe->szFileName);
			nCount--;
			i--;
		}
	}

	m_curDir = m_tnRoot.m_strName;
	
	UpdateLookinCombo();

	if(!UpdatePckListCtrl(&m_tnRoot))
		return false;

	if(!UpdatePckDirCtrl(&m_tnRoot))
		return false;

	m_pRootNode = &m_tnRoot;

	return true;
}

bool CCommonFileDlg::UpdateDiskListCtrl(CString sPath)
{
	m_lstFile.DeleteAllItems();
	m_arrDir.RemoveAll();
	m_arrFiles.RemoveAll();

	m_arrFilelist.RemoveAll();

	m_fileName.Empty();
	m_strFileInfo.SetWindowText(_T(""));
	m_strImageInfo.SetWindowText(_T(""));
	UpdateData(FALSE);

	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString FileName;

	BOOL fFinished = FALSE;

	hSearch = FindFirstFile(sPath+_T("\\")+_T("*"), &FileData);
	if(hSearch == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_AL("No files found!"));
		return false;
	}
	
	while(!fFinished)
	{
		FileName = FileData.cFileName;
		if(strcmp(AC2AS(FileName), ".") != 0 && strcmp(AC2AS(FileName), "..") != 0)
		{
			if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				m_lstFile.InsertItem(0, FileName, 2);

				if(m_arrDir.GetSize()==0)
					m_arrDir.Add(FileName);
				else
				{
					int dircount = (int)m_arrDir.GetSize();
					int dirnum;
					for(dirnum=0; dirnum<dircount; dirnum++)
					{
						if(FileName < m_arrDir.GetAt(dirnum))
						{
							m_arrDir.InsertAt(dirnum, FileName);
							break;
						}
					}
					if(dirnum == dircount)
						m_arrDir.Add(FileName);
				}
			}
			else
			{
				int nCount;
				CString temp;
				if(FileName.Find('.') > 0)
				{
					int len = FileName.GetLength();
					temp = FileName.Right(len-FileName.ReverseFind('.')-1);
					temp.MakeLower();
				}

				if(m_ext == _T("*.*"))
				{
					if(temp.Find(_T("bmp")) != -1 || 
							temp.Find(_T("dds")) != -1 || 
							temp.Find(_T("jpg")) != -1 || 
							temp.Find(_T("tga")) != -1)
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, FileName, 9);
						InsertFiles(FileName);
					}
					else if(temp.Find(_T("mp3")) != -1 || 
								temp.Find(_T("wav")) != -1)
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, FileName, 10);
						InsertFiles(FileName);
					}
					else
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, FileName, 8);
						InsertFiles(FileName);
					}
				}
				else
				{
					if(m_ext.Find(temp) != -1)
					{
						if(m_ext.Find(_T("bmp")) != -1)
						{
							nCount = m_lstFile.GetItemCount();
							m_lstFile.InsertItem(nCount, FileName, 9);
							InsertFiles(FileName);
						}
						else if(m_ext.Find(_T("mp3")) != -1)
						{
							nCount = m_lstFile.GetItemCount();
							m_lstFile.InsertItem(nCount, FileName, 10);
							InsertFiles(FileName);
						}
						else
						{
							nCount = m_lstFile.GetItemCount();
							m_lstFile.InsertItem(nCount, FileName, 8);
							InsertFiles(FileName);
						}
					}
				}
			}
		}
	
		if(!FindNextFile(hSearch, &FileData))
		{
			if(GetLastError() == ERROR_NO_MORE_FILES)
			{
				fFinished = TRUE;
			}
			else
			{
				AfxMessageBox(_T("Couldn't find next file."));
				return false;
			}
		}
	}

	if(!FindClose(hSearch))
	{
		AfxMessageBox(_T("Couldn't close search handle."));
		return false;
	}

	return true;
}


bool CCommonFileDlg::UpdateDiskDirCtrl(CString sPath)
{
	m_lstDir.DeleteAllItems();

	for (int i=0; i<m_arrWorkRoot.GetSize(); ++i)
	{
		m_lstDir.InsertItem(i, m_arrWorkRoot.GetAt(i), 1);
		m_lstDir.SetItemData(i, i);
	}

	return true;
}

void CCommonFileDlg::UpdateArrCurDir(CString sDir)
{
	m_arrCurDir.RemoveAll();
	if(m_curDir.GetLength() == 0)
		return;

	CString temp = sDir;
	int n = temp.Find('\\');
	while (n > 0)
	{
		m_arrCurDir.Add(temp.Left(n));
		temp = temp.Mid(n+1);
		n = temp.Find('\\');
	}
	m_arrCurDir.Add(temp);
}

void CCommonFileDlg::UpdateLookinCombo()
{
	COMBOBOXEXITEM item;
	ZeroMemory(&item, sizeof(item));
	
	m_cboLookIn.ResetContent();
	
	if(m_curDir == m_workRoot)
	{
		item.mask = CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
		item.iItem = 0;
		item.iImage = 1;
		item.pszText = (ACHAR*)(LPCTSTR)m_curDir;
		m_cboLookIn.InsertItem (&item);
		m_cboLookIn.SetCurSel(0);
	}
	else
	{
		item.mask = CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
		item.iItem = 0;
		item.iImage = 1;
		item.pszText = (ACHAR*)(LPCTSTR)m_workRoot;
		m_cboLookIn.InsertItem (&item);
		
		GetRelWorkRoot();
		UpdateArrCurDir(m_relworkRoot);

		for(int i=0; i<m_arrCurDir.GetSize(); i++)
		{
			item.iItem = i+1;
			item.iImage = 2;
			item.iSelectedImage = 2;
			item.iIndent = i+1;
			item.pszText = (ACHAR*)(LPCTSTR)m_arrCurDir.GetAt(i);
			m_cboLookIn.InsertItem(&item);
		}

		m_cboLookIn.SetCurSel(m_cboLookIn.GetCount()-1);
	}
}

bool CCommonFileDlg::UpdatePckListCtrl(CPckTreeNode* pNode)
{
	m_lstFile.DeleteAllItems();
	m_arrDir.RemoveAll();

	m_arrFilelist.RemoveAll();

	m_fileName.Empty();
	m_strFileInfo.SetWindowText(_T(""));
	m_strImageInfo.SetWindowText(_T(""));
	UpdateData(FALSE);

	m_pCurNode = pNode;
	if(pNode == NULL)
		return false;

	POSITION pos = pNode->m_lstChilds.GetHeadPosition();
	while(pos != NULL)
	{
		CPckTreeNode* pChild = pNode->m_lstChilds.GetNext(pos);
		if(pChild == NULL)
			continue;
		if(pChild->m_bIsDirectory)
		{
			m_lstFile.InsertItem(0, pChild->m_strName, 2);

			m_arrDir.Add(pChild->m_strName);
		}
		else
		{
			int nCount = m_lstFile.GetItemCount();
			CString temp;
			CString sFileName = pChild->m_strName;

			if(sFileName.Find('.') > 0)
			{
				int len = sFileName.GetLength();
				temp = sFileName.Right(len-sFileName.ReverseFind('.')-1);
				temp.MakeLower();
			}

			if(m_ext == _T("*.*"))
			{
				if(temp.Find(_T("bmp")) != -1 || 
							temp.Find(_T("dds")) != -1 || 
							temp.Find(_T("jpg")) != -1 || 
							temp.Find(_T("tga")) != -1)
				{
					nCount = m_lstFile.GetItemCount();
					m_lstFile.InsertItem(nCount, sFileName, 9);
				}
				else if(temp.Find(_T("mp3")) != -1 || 
							temp.Find(_T("wav")) != -1)
				{
					nCount = m_lstFile.GetItemCount();
					m_lstFile.InsertItem(nCount, sFileName, 10);
				}
				else
				{
					nCount = m_lstFile.GetItemCount();
					m_lstFile.InsertItem(nCount, sFileName, 8);
				}
			}
			else
			{
				if(m_ext.Find(temp) != -1)
				{
					if(m_ext.Find(_T("bmp")) != -1)
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, sFileName, 9);
					}
					else if(m_ext.Find(_T("mp3")) != -1)
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, sFileName, 10);
					}
					else
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, sFileName, 8);
					}
				}
			}
		}
	}

	return true;
}

bool CCommonFileDlg::UpdatePckDirCtrl(CPckTreeNode* pNode)
{
	m_lstDir.DeleteAllItems();
	
	m_pCurNode = pNode;
	if(pNode == NULL)
		return false;

	POSITION pos = pNode->m_lstChilds.GetHeadPosition();
	while(pos != NULL)
	{
		CPckTreeNode* pChild = pNode->m_lstChilds.GetNext(pos);
		if(pChild == NULL)
			continue;
		if(pChild->m_bIsDirectory)
		{
			m_lstDir.InsertItem(0, pChild->m_strName, 1);
		}
	}

	return true;
}

void CCommonFileDlg::ClosePckListCtrl()
{
	m_tnRoot.FreeAllChilds();

	if(m_pPackage != NULL)
	{
		delete m_pPackage;
		m_pPackage = NULL;
	}
}

bool CCommonFileDlg::LoadPrev(CString& sFile)
{
	if(sFile.IsEmpty() || sFile == m_strLastPath)
		return true;
	
	m_sPrev.SetRenderableObject(sFile);
	
	m_strImageInfo.SetWindowText(m_sPrev.m_sizeinfo);
	m_strImageInfo.UpdateData(FALSE);

	return true;
}

void CCommonFileDlg::UpdatePrevRegion()
{
	m_strLastPath.Empty();

	m_sPrev.BlankWnd();

	CRect rect;
	m_sPrev.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);
}

bool CCommonFileDlg::GetMulFiles()
{
	m_arrFilelist.RemoveAll();
	int nItem = -1;
	
	for (unsigned int i = 0; i < m_lstFile.GetSelectedCount(); i++)
	{
		nItem = m_lstFile.GetNextItem(nItem, LVNI_SELECTED);
		m_arrFilelist.Add(m_lstFile.GetItemText(nItem, 0));
	}

	return true;
}

void CCommonFileDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
}


void CCommonFileDlg::OnOpen() 
{
	// TODO: Add your control notification handler code here
	if (m_arrFilelist.GetSize() < 1)
		return;
	
	if (m_arrFilelist.GetSize() > 1)
	{
		for ( int i = 0; i < m_arrFilelist.GetSize(); i++)
		{
			//CString strName = m_arrFilelist.GetAt(i);
			CString fulfilename = m_curDir + _T("\\") + m_arrFilelist.GetAt(i);

			DWORD dwAttr = GetFileAttributes(fulfilename);

			if ( dwAttr != 0xFFFFFFFF && (dwAttr & FILE_ATTRIBUTE_DIRECTORY) )
			{
				MessageBox(_T("包含文件夹！请重新选择"));
				m_arrFilelist.RemoveAll();
				return;
			}
		}
	}

	if (m_arrFilelist.GetSize() == 1)
	{
		CString strName = m_arrFilelist.GetAt(0);
		CString fulfilename = m_curDir + _T("\\") + m_arrFilelist.GetAt(0);

		DWORD dwAttr = GetFileAttributes(fulfilename);
		
		if ( dwAttr != 0xFFFFFFFF && (dwAttr & FILE_ATTRIBUTE_DIRECTORY) )
		{
			if(OpenMode == 1)
			{
				m_curDir += _T("\\");
				m_curDir += strName;
				
				UpdateLookinCombo();

				UpdateDiskListCtrl(fulfilename);
				UpdatePrevRegion();
			}
		
			if(OpenMode == 2)
			{
				if(m_pCurNode != NULL)
				{
					CPckTreeNode* pChild = m_pCurNode->FindChild(AC2AS((LPCTSTR)strName));
					if(pChild != NULL)
					{
						m_curDir += _T("\\");
						m_curDir += strName;
						
						UpdateLookinCombo();
					
						UpdatePckListCtrl(pChild);
						UpdatePrevRegion();
					}
				}
			}
		
			m_backDir.Add(m_curDir);
			bChanged = true;
			m_arrFilelist.RemoveAll();

			return;
		}
		
	}
	Open();
}


void CCommonFileDlg::Open() 
{
	ClosePckListCtrl();

	m_imageList.DeleteImageList();
	m_dirImageList.DeleteImageList();

	bmp[0].DeleteObject();
	bmp[1].DeleteObject();
	m_bmpPrev.DeleteObject();
	
	if(m_pSoundBuf != NULL)
		m_pSoundBufMan->Release2DSound(&m_pSoundBuf);

	if (m_curDir.Find(m_curRoot) != -1)
	{
#ifdef _UNICODE
		FILE* fpFile = _wfopen(_strDirFile, L"wb");
#else
		FILE* fpFile = fopen(_strDirFile, "wb");
#endif

		if(NULL == fpFile) return;

		char savelog[MAX_PATH];
		int nNum;

		AString retdir = AC2AS(GetRootDir());
		int len = retdir.GetLength();
		strcpy(savelog, AC2AS(GetCurDir().Mid(len)));
		nNum = (int)strlen(savelog);
		fwrite(&nNum, sizeof(int), 1, fpFile);
		fwrite(savelog, sizeof(char), nNum, fpFile);

		fclose(fpFile);
	}

	CDialog::OnOK();
}

void CCommonFileDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	ClosePckListCtrl();

	m_imageList.DeleteImageList();
	m_dirImageList.DeleteImageList();

	bmp[0].DeleteObject();
	bmp[1].DeleteObject();
	m_bmpPrev.DeleteObject();
	
	if(m_pSoundBuf != NULL)
		m_pSoundBufMan->Release2DSound(&m_pSoundBuf);
	
	CDialog::OnCancel();
}

void CCommonFileDlg::OnSort() 
{
	SortItemsDescending();
}

void CCommonFileDlg::SortItemsDescending()
{
	m_lstFile.DeleteAllItems();
	int nCount = (int)m_arrDir.GetSize();
	CString filename;

	int i;
	for(i=0; i<nCount; i++)
	{
		m_lstFile.InsertItem(i, m_arrDir.GetAt(i), 2);
	}
	for(i=0; i<m_arrFiles.GetSize(); i++)
	{
		filename = m_arrFiles.GetAt(i);
		if(filename.Find(_T("bmp")) != -1 || filename.Find(_T("dds")) != -1 || 
			filename.Find(_T("jpg")) != -1 || filename.Find(_T("tga")) != -1)
			m_lstFile.InsertItem(i+nCount, m_arrFiles.GetAt(i), 9);
		else if(filename.Find(_T("mp3")) != -1 || 
				filename.Find(_T("wav")) != -1)
			m_lstFile.InsertItem(i+nCount, m_arrFiles.GetAt(i), 10);
		else
			m_lstFile.InsertItem(i+nCount, m_arrFiles.GetAt(i), 8);
	}
}

void CCommonFileDlg::InsertFiles(CString file)
{
	if(m_arrFiles.GetSize()==0)
		m_arrFiles.Add(file);
	else
	{
		int filecount = (int)m_arrFiles.GetSize();
		int filenum;
		for(filenum=0; filenum<filecount; filenum++)
		{
			if(file < m_arrFiles.GetAt(filenum))
			{
				m_arrFiles.InsertAt(filenum, file);
					break;
			}
		}
		if(filenum == filecount)
			m_arrFiles.Add(file);
	}
}

int CCommonFileDlg::DoModal() 
{
	m_hHostResourceHandle = AfxGetResourceHandle();
	AfxSetResourceHandle(theApp.m_hInstance);

	return (int)CDialog::DoModal();
}

void CCommonFileDlg::OnItemchangedFilelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if ( !(((NMLISTVIEW*)pNMHDR)->uChanged == LVIF_STATE) || !((NMLISTVIEW*)pNMHDR)->uNewState )
		return;

	int hItem = ((NMLISTVIEW*)pNMHDR)->iItem;
	*pResult = 0;

	if(hItem < 0)
	{
		m_fileName = "";
		if(m_pSoundBuf != NULL)
			m_pSoundBuf->Stop();
		UpdatePrevRegion();
		m_strFileInfo.SetWindowText(_T(""));
		m_strImageInfo.SetWindowText(_T(""));
		UpdateData(FALSE);

		return;
	}

	GetRelCurRoot();
	m_fileName = m_lstFile.GetItemText(hItem, 0);

	if(hItem < m_arrDir.GetSize())
	{
		m_fileName = "";
		if(m_pSoundBuf != NULL)
			m_pSoundBuf->Stop();
		UpdatePrevRegion();
		m_strFileInfo.SetWindowText(_T(""));
		m_strImageInfo.SetWindowText(_T(""));
		UpdateData(FALSE);
	}
	else
	{
		if(m_preview.GetCheck() == TRUE)
		{
			int reti = m_fileName.ReverseFind('.');
			int len = m_fileName.GetLength();

			CString fileext = m_fileName.Right(len - reti);
			fileext.MakeLower();

			if(fileext.Find(_T(".bmp")) != -1 || 
					fileext.Find(_T(".tga")) != -1 ||
					fileext.Find(_T(".jpg")) != -1 ||
					fileext.Find(_T(".ecm")) != -1 ||
					fileext.Find(_T(".gfx")) != -1 ||
					fileext.Find(_T(".png")) != -1 ||
					fileext.Find(_T(".dds")) != -1)
			{
				LoadPrev(m_curDir+"\\"+m_fileName);
			}
			else if(m_fileName.Find(_T("mp3")) != -1 || m_fileName.Find(_T("wav")) != -1)
			{
				if(m_pSoundBuf != NULL)
					m_pSoundBufMan->Release2DSound(&m_pSoundBuf);
				m_pSoundBuf = m_pSoundBufMan->Load2DSound(AC2AS((LPCTSTR)(m_relRoot+"\\"+m_fileName)));
				if(m_pSoundBuf != NULL)
				{
					m_pSoundBuf->Stop();
					m_pSoundBuf->Play(0);
				}
			}
			else
				UpdatePrevRegion();

			m_strLastPath = m_curDir+_T("\\")+m_fileName;
		}
		else
		{
			UpdatePrevRegion();
			m_strFileInfo.SetWindowText(_T(""));
			m_strImageInfo.SetWindowText(_T(""));

			//m_strLastPath.Empty();
		}
		UpdateData(FALSE);
	}

	if(flags == AFILE_ALLOWMULFILES)
		GetMulFiles();
}

void CCommonFileDlg::OnSetNull() 
{
	set_null = true;
	EndDialog(IDOK);
}

#endif