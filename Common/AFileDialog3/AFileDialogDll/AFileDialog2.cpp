/*
* FILE: AFileDialog2.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/18
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "AFileDialog2.h"
#include "AListCtrl2.h"
#include "AFileItem.h"
#include "AFileItemType.h"
#include "AFilePreviewWnd.h"
#include "Render.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

extern CAFileDialogDllApp theApp;

extern void _SetBitmapTransparent(CBitmap* pBitmap, COLORREF colorTrans, COLORREF colorDest);

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

struct UPDATE_IMAGE
{
	CString strUpdatePath;
	CString strFileName;
	ICLFile::ICLItem* pICLItem;
};


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

CCriticalSection global_cs;


FILETIME sysTime2FileTime(const SYSTEMTIME& sysTime)
{
	FILETIME ftTime;
	SystemTimeToFileTime(&sysTime, &ftTime);
	return ftTime;
}

FILETIME ctime2FileTile(const CTime& time)
{
	SYSTEMTIME sysTime;
	time.GetAsSystemTime(sysTime);
	return sysTime2FileTime(sysTime);
}

bool is_skip_file(CFileFind& finder)
{
	if (finder.IsDots()) return true;
	if (finder.IsHidden()) return true;
	if (finder.GetFileName().Find(_T("acidb.db")) >= 0) return true;
	return false;
}

void _SplitCurDir(const CString& strCurDir, const CString& strBaseDir, CStringArray& aDirs)
{
	aDirs.RemoveAll();
	int nLen = strCurDir.GetLength() - strBaseDir.GetLength();
	CString strCurDif = strCurDir.Right(nLen);
	int n = strCurDif.Find('\\');
	while (n > 0)
	{
		aDirs.Add(strCurDif.Left(n));
		strCurDif = strCurDif.Mid(n+1);
		n = strCurDif.Find('\\');
	}
}

COLORREF _RGB2BITMAPCOLOR(COLORREF colorIn)
{
	return GetRValue(colorIn) << 16 | GetGValue(colorIn) << 8 | GetBValue(colorIn);
}

void _SetBitmapTransparent(CBitmap* pBitmap, COLORREF colorTrans, COLORREF colorDest)
{
	if (!pBitmap)
		return;

	BITMAP bitmap;
	pBitmap->GetBitmap(&bitmap);

	int iBitMapBytes = bitmap.bmHeight * bitmap.bmWidth;
	if (bitmap.bmBitsPixel == 32)
		iBitMapBytes *= 4;
	else if (bitmap.bmBitsPixel == 24)
		iBitMapBytes *= 3;
	else
		return;

	unsigned char* pBuffer = new unsigned char[iBitMapBytes];
	if (pBitmap->GetBitmapBits(iBitMapBytes, pBuffer) != iBitMapBytes) {
		delete [] pBuffer;
		return;
	}

	if (bitmap.bmBitsPixel == 32) {
		DWORD* pData = (DWORD*)pBuffer;
		for (int i = 0; i < bitmap.bmHeight; ++i)
		{
			for (int j = 0; j < bitmap.bmWidth; ++j)
			{
				int iIdx = i * bitmap.bmWidth + j; 
				if (pData[iIdx] == _RGB2BITMAPCOLOR(colorTrans)) {
					pData[iIdx] = _RGB2BITMAPCOLOR(colorDest);
				}
			}
		}
	}
	else if (bitmap.bmBitsPixel == 24) {
		unsigned char* pData = (unsigned char*)pBuffer;
		for (int i = 0; i < bitmap.bmHeight; ++i)
		{
			for (int j = 0; j < bitmap.bmWidth; ++j)
			{
				int iIdx = (i * bitmap.bmWidth + j) * 3;

				if (pData[iIdx] == (colorTrans & 0xff)
					&& pData[iIdx + 1] == ((colorTrans & 0xff00) >> 8)
					&& pData[iIdx + 2] == ((colorTrans & 0xff0000) >> 16)) {
						pData[iIdx] = (unsigned char)((colorDest & 0xff0000) >> 16);
						pData[iIdx + 1] = (unsigned char)((colorDest & 0xff00) >> 8);
						pData[iIdx + 2] = (unsigned char)(colorDest & 0xff);
				}
			}
		}
	}

	pBitmap->SetBitmapBits(iBitMapBytes, pBuffer);
	delete [] pBuffer;
}
//////////////////////////////////////////////////////////////////////////
//
//	Implementation of AFileDialog2::AFConfigures
//
//////////////////////////////////////////////////////////////////////////

TCHAR const * AFileDialog2::AFConfigures::s_szEntryLastOpenRelativePath		= _T("LastOpenRelativePath");
TCHAR const * AFileDialog2::AFConfigures::s_szEntryLastSelectedFilter		= _T("LastSelectedFilter");
TCHAR const * AFileDialog2::AFConfigures::s_szEntryLastCheckedAlphaSwitch	= _T("LastCheckedAlphaSwitch");
TCHAR const * AFileDialog2::AFConfigures::s_szEntryLastWndWidth				= _T("LastWndWidth");
TCHAR const * AFileDialog2::AFConfigures::s_szEntryLastWndHeight			= _T("LastWndHeight");
TCHAR const * AFileDialog2::AFConfigures::s_szEntryLastListStyle			= _T("LastListStyle");
TCHAR const * AFileDialog2::AFConfigures::s_szEntryLastSortStyle			= _T("LastSortStyle");

const TCHAR* AFileDialog2::AFConfigures::GetRegSection() const
{
	return _T("AFSettings");
}

void AFileDialog2::AFConfigures::LoadSettingsFromReg()
{
	m_strLastOpenRelativePath	= AfxGetApp()->GetProfileString(GetRegSection(), s_szEntryLastOpenRelativePath, m_strLastOpenRelativePath);
	m_nLastSelectedFilter		= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastSelectedFilter, m_nLastSelectedFilter);
	m_bLastCheckedAlphaSwitch	= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastCheckedAlphaSwitch, m_bLastCheckedAlphaSwitch);
	m_nCX						= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastWndWidth, m_nCX);
	m_nCY						= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastWndHeight, m_nCY);
	m_nListStyle				= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastListStyle, m_nListStyle);
	m_nSortStyle				= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastSortStyle, m_nSortStyle);
}

void AFileDialog2::AFConfigures::SaveSettingsToReg()
{
	AfxGetApp()->WriteProfileString(GetRegSection(), s_szEntryLastOpenRelativePath, m_strLastOpenRelativePath);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastSelectedFilter, m_nLastSelectedFilter);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastCheckedAlphaSwitch, m_bLastCheckedAlphaSwitch);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastWndWidth, m_nCX);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastWndHeight, m_nCY);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastListStyle, m_nListStyle);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastSortStyle, m_nSortStyle);
}

void AFileDialog2::AFConfigures::OnCurRelativePathChanged(const TCHAR* szCurFull) 
{
	AString strCurFull = AC2AS(szCurFull);
	AString strCurRel;
	af_GetRelativePathNoBase(strCurFull, af_GetBaseDir(),  strCurRel);
	m_strLastOpenRelativePath = AS2AC(strCurRel);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AFileDialog2MsgReciever
//	
///////////////////////////////////////////////////////////////////////////

class AFileDialog2MsgReciever : public ICLNotifier
{
public:
	AFileDialog2MsgReciever(AFileDialog2* pFD)
		: m_pFD(pFD)
	{
		ASSERT(m_pFD);
	}
	~AFileDialog2MsgReciever() {}

protected:

	virtual void FolderProcComplete(const TCHAR * szFolder);
	void UpdateThumbImages(const TCHAR* szCompletedFolder);

private:
	AFileDialog2* m_pFD;
};

void AFileDialog2MsgReciever::UpdateThumbImages(const TCHAR* szCompletedFolder)
{
//	RAII_LockUpdateCurDir Raii(m_pFD);
	CString strCurDir = m_pFD->GetCurDir();

	if (strCurDir.CompareNoCase(szCompletedFolder) != 0)
		return;

	ICLFile tmpFile;
	if (!tmpFile.Open(strCurDir))
		return;

	for (int iIdx = 0; iIdx < (int)tmpFile.GetICLItemCount(); ++iIdx)
	{
		if (strCurDir != m_pFD->GetCurDir())
			return;

		ICLFile::ICLItem* pItem = tmpFile.GetICLItemByIdx(iIdx);

		UPDATE_IMAGE upImg;
		upImg.pICLItem = pItem;
		upImg.strUpdatePath = szCompletedFolder;
		upImg.strFileName = AS2AC(pItem->strFileName);

		if (WaitForSingleObject(m_pFD->GetDestroyEvent().m_hObject, 0) == WAIT_OBJECT_0)
			return;

		m_pFD->SendMessage(WM_UPDATE_ONE_IMAGE, (WPARAM)&upImg, 0);
	}
}

void AFileDialog2MsgReciever::FolderProcComplete(const TCHAR * szFolder)
{
	// Be careful, for this part of code will be run in a worker thread.
	UpdateThumbImages(szFolder);
//	m_pFD->SendMessage(WM_FOLDER_PROC_COMPLETED, NULL, (LPARAM)szFolder);
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement AListCtrlListener
//
///////////////////////////////////////////////////////////////////////////

class AListCtrl2Listener : public AListCtrl2::Listener
{
public:
	AListCtrl2Listener(AFileDialog2* pFD)
		: m_pFD(pFD)
	{

	}
	~AListCtrl2Listener()
	{

	}

protected:

	virtual void OnItemClicked(AListItem* pItem);
	virtual void OnItemDbClicked(AListItem* pItem);

private:
	AFileDialog2* m_pFD;
};


void AListCtrl2Listener::OnItemClicked(AListItem* pItem)
{
	m_pFD->OnItemClicked(pItem);
}

void AListCtrl2Listener::OnItemDbClicked(AListItem* pItem)
{
	m_pFD->OnItemDbClicked(pItem);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AFileDialog2
//	
///////////////////////////////////////////////////////////////////////////

#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21

AFileDialog2::AFileDialog2(A3DDevice* pDevice
			 , BOOL bIsOpenFile
			 , const TCHAR* szInitRelativeDir /*= _T("")*/
			 , const TCHAR* szCaption /*= _T("")*/
			 , const TCHAR* szCustomExts
			 , int iListStyle /*= AListCtrl2::LV_THUMBVIEW*/
			 , CWnd* pParent /*= NULL*/)
			 : BaseDialog(AFileDialog2::IDD, pParent)
			 , m_pDevice(pDevice)
			 , m_pLstCtrl(new AListCtrl2)
			 , m_bIsOpenFile(bIsOpenFile)
			 , m_pICLPool(new ICLThreadPool())
			 , m_nListStyle(iListStyle)
			 , m_nSortStyle(0)
			 , m_strCaption(szCaption)
			 , m_strInitRelativeDir(szInitRelativeDir)
			 , m_pSound(new AFSoundPlayItem)
{
	//if (m_nListStyle < 0)
	m_nListStyle = m_afConfigs.GetLastListStyle();
	m_nSortStyle = m_afConfigs.GetLastSortStyle();

	ConstructInit(szCustomExts);
}

#elif defined(_ANGELICA31)

AFileDialog2::AFileDialog2(BOOL bIsOpenFile
			 , const TCHAR* szInitRelativeDir /*= _T("")*/
			 , const TCHAR* szCaption /*= _T("")*/
			 , const TCHAR* szCustomExts /*= _T("")*/
			 , int iListStyle /*= AListCtrl2::LV_THUMBVIEW*/
			 , CWnd* pParent /*= NULL*/)
			 : BaseDialog(AFileDialog2::IDD, pParent)
			 , m_pLstCtrl(new AListCtrl2)
			 , m_bIsOpenFile(bIsOpenFile)
			 , m_pICLPool(new ICLThreadPool())
			 , m_nListStyle(iListStyle)
			 , m_nSortStyle(0)
			 , m_strCaption(szCaption)
			 , m_strInitRelativeDir(szInitRelativeDir)
			 , m_pSound(new AFSoundPlayItem)
{
//	if (m_nListStyle < 0)
	m_nListStyle = m_afConfigs.GetLastListStyle();
	m_nSortStyle = m_afConfigs.GetLastSortStyle();

	ConstructInit(szCustomExts);
}

#endif

void AFileDialog2::ConstructInit(const TCHAR* szCustomExts)
{
	m_pMsgNorifier = new AFileDialog2MsgReciever(this);
	m_pLstCtrlListener = new AListCtrl2Listener(this);

	m_aSupportedTypes[AFileItemType::AFI_IMAGE]		= new AFIImageType();
	m_aSupportedTypes[AFileItemType::AFI_ENGINE]	= new AFIEngineType();
	m_aSupportedTypes[AFileItemType::AFI_SOUND]		= new AFISoundType();
	m_aSupportedTypes[AFileItemType::AFI_DIR]		= new AFIDirType();
	m_aSupportedTypes[AFileItemType::AFI_CUSTOM]	= new AFICustomType(szCustomExts);
	m_aSupportedTypes[AFileItemType::AFI_DEFAULT]	= new AFIDefaultType();
}

AFileDialog2::~AFileDialog2(void)
{
	ClearFileItems();

	if (m_pSound)
	{
		m_pSound->StopSound();
#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
		m_pSound->Release(get_AM_sound_buffer_man(m_pDevice));
#else
		m_pSound->Release(get_AM_sound_buffer_man(CRender::GetInstance()->GetA3DDevice()));
#endif
		delete m_pSound;
		m_pSound = NULL;
	}

	delete m_pLstCtrlListener;
	m_pLstCtrlListener = NULL;

	delete m_pMsgNorifier;
	m_pMsgNorifier = NULL;

	delete m_pICLPool;
	m_pICLPool = NULL;

	delete m_pLstCtrl;
	m_pLstCtrl = NULL;

	for (int iSupportedTypeIdx = 0; iSupportedTypeIdx < AFileItemType::AFI_NUM; ++iSupportedTypeIdx)
	{
		delete m_aSupportedTypes[iSupportedTypeIdx];
		m_aSupportedTypes[iSupportedTypeIdx] = NULL;
	}
}

BEGIN_MESSAGE_MAP(AFileDialog2, BaseDialog)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_CBN_SELCHANGE(IDC_CB_FILETYPES, OnSelchangeCbFiletypes)
	ON_CBN_SELCHANGE(IDC_CB_PATH_SHORTCUT, OnSelchangeCbPathShortcut)
	ON_MESSAGE(WM_FOLDER_PROC_COMPLETED, OnFolderProcCompleted)
	ON_MESSAGE(WM_UPDATE_ONE_IMAGE, OnUpdateOneImage)
	ON_MESSAGE(WM_UPDATE_FIRST_FOLDER, OnUpdateFirstFolder)
	ON_MESSAGE(WM_ONDBCLK_SELFILE, OnDbkSelectFile)
	ON_BN_CLICKED(IDC_BT_BACK, OnBtBack)
	ON_BN_CLICKED(IDC_BT_UP, OnBtUp)
	ON_BN_CLICKED(IDC_OpenFolder, OnOpenFolder)

#if defined( _MSC_VER ) && _MSC_VER < 1310
	ON_BN_CLICKED(IDC_IS_ALPHA_BLEND, AFileDialog2::OnBnClickedIsAlphaBlend)
	ON_BN_CLICKED(IDC_IS_PWINDOW_SHOW, AFileDialog2::OnBnClickedIsWindowShow)
	ON_BN_CLICKED(IDC_DISPLAY_LOD, AFileDialog2::OnBnClickedDisplayLod)
#else
	ON_BN_CLICKED(IDC_IS_ALPHA_BLEND, &AFileDialog2::OnBnClickedIsAlphaBlend)
	ON_BN_CLICKED(IDC_IS_PWINDOW_SHOW, &AFileDialog2::OnBnClickedIsWindowShow)
	ON_BN_CLICKED(IDC_DISPLAY_LOD, &AFileDialog2::OnBnClickedDisplayLod)
#endif

	ON_WM_CREATE()
END_MESSAGE_MAP()

bool AFileDialog2::FormatInitDir(const TCHAR * szRelativeDir, const TCHAR * szInitSubDir)
{
	CString strBaseDir(af_GetBaseDir());
	CString strRelativeDir(szRelativeDir);
	CString strWholePath;

	strRelativeDir.TrimLeft(_AL("\\"));
	strRelativeDir.TrimLeft(_AL("/"));
	strRelativeDir.TrimRight(_AL("\\"));
	strRelativeDir.TrimRight(_AL("/"));
	strBaseDir.TrimRight(_AL("\\"));
	strBaseDir.TrimRight(_AL("/"));

	if (strRelativeDir.IsEmpty())
		strWholePath.Format(_T("%s\\"), strBaseDir);
	else
		strWholePath.Format(_T("%s\\%s\\"), (LPCTSTR)strBaseDir, (LPCTSTR)strRelativeDir);

	// If init_subdir is set, then open it
	if (szInitSubDir != NULL && _tcslen(szInitSubDir) != 0)
	{
		CString strDir;
		strDir.Format(_T("%s%s\\"), strWholePath, szInitSubDir);
		if (PathIsDirectory(strDir))
		{
			m_strCurDir = strDir;
			m_strFullBaseDir = strWholePath;
			return true;
		}
	}

	// If last we opened a directory under szRelativeDir, then we try to reopen this specific directory again
	if (StrStr(m_afConfigs.GetLastRelativePath(), strRelativeDir) != NULL || strRelativeDir.IsEmpty()) {
		CString strLastRelPath;
		strLastRelPath.Format(_T("%s\\%s"), (LPCTSTR)strBaseDir, (LPCTSTR)m_afConfigs.GetLastRelativePath());
		if (strLastRelPath.Right(1) != _T("\\") && strLastRelPath.Right(1) != _T("/"))
			strLastRelPath += _T("\\");

		if (PathIsDirectory(strLastRelPath)) {
			m_strCurDir = strLastRelPath;
			m_strFullBaseDir = strWholePath;
			return true;
		}
		// Path is not a directory (no longer exist or under some other Engine Base Path)
		// We try open the path as normal
	}

	if (!PathIsDirectory(strWholePath))
	{
		if (!CreateDirectory(strWholePath, NULL))
			return false;
	}


	m_strCurDir = strWholePath;
	m_strFullBaseDir = strWholePath;
	
	return true;
}

void AFileDialog2::LoadResources()
{
	m_bmArrowUp.LoadBitmap(IDB_ARROW_UP);
	m_bmArrowBack.LoadBitmap(IDB_ARROW_BACK);
}

void AFileDialog2::ClearFileItems()
{
	for (int iItemIdx = 0; iItemIdx < m_aFileItems.GetSize(); ++iItemIdx)
		delete m_aFileItems[iItemIdx];

	m_aFileItems.RemoveAll();
}

void AFileDialog2::RecreateImageList()
{
	if (m_FileItemImages.GetSafeHandle())
		m_FileItemImages.DeleteImageList();

	m_FileItemImages.Create(IMG_SIZE_THUMB, IMG_SIZE_THUMB, ILC_COLOR24, 5, 10);

	for (int iSupportTypeIdx = 0; iSupportTypeIdx < AFileItemType::AFI_NUM; ++iSupportTypeIdx)
		m_aSupportedTypes[iSupportTypeIdx]->OnPrepareDefaultThumbImage(&m_FileItemImages);
}

void AFileDialog2::CreateFileItemsForDir(const TCHAR * szDir)
{
	CString strDir(szDir);
	if (m_strCurDir != strDir)
		return;

	ClearFileItems();
	RecreateImageList();

	CFileFind finder;
	CString strBase;
	strBase.Format(_T("%s\\%s"), strDir, _T("*.*"));
	BOOL bWorking = finder.FindFile(strBase);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (is_skip_file(finder))
			continue;

		CTime time;
		finder.GetLastWriteTime(time);

		if (finder.IsDirectory())
		{
			m_aFileItems.Add(new DirectoryItem
				( m_aSupportedTypes[AFileItemType::AFI_DIR]
				, finder.GetFileName()
				, ctime2FileTile(time)
				, 0
				, finder.GetFilePath()));
			continue;
		}

		for (int iTypeIdx = 0; iTypeIdx < AFileItemType::AFI_NUM; ++iTypeIdx)
		{
			if (!m_aSupportedTypes[iTypeIdx]->IsCurrentType(finder.GetFileName()))
				continue;

			AFileItem* pItem = new AFileItem(m_aSupportedTypes[iTypeIdx]
			, finder.GetFileName()
				, ctime2FileTile(time)
				, finder.GetLength()
				, finder.GetFilePath());

			m_aFileItems.Add(pItem);
			break;
		}
	}

	UpdateListCtrlByFilter(GetCurFilterType());
}

void AFileDialog2::UpdateListCtrlByFilter(const AFileItemType* pCurType)
{
	m_pLstCtrl->ClearItems();

	int iItemIdx = 0;
	//	First Add Directories
	for (iItemIdx = 0; iItemIdx < m_aFileItems.GetSize(); ++iItemIdx)
	{
		if (m_aFileItems[iItemIdx]->GetItemType()->GetTypeId() == AFileItemType::AFI_DIR)
		{
			m_pLstCtrl->AddItem(m_aFileItems[iItemIdx]);
			continue;
		}
	}

	//	Then Add other type of items
	for (iItemIdx = 0; iItemIdx < m_aFileItems.GetSize(); ++iItemIdx)
	{
		AFileItem* pItem = m_aFileItems[iItemIdx];
		if (pItem->GetItemType()->GetTypeId() == AFileItemType::AFI_DIR)
			continue;

		if (IgnoreItem(pItem->GetItemTitle()))
			continue;

		if (pCurType == pItem->GetItemType())
		{
			m_pLstCtrl->AddItem(pItem);
			continue;
		}

		if (pCurType->IsCurrentType(pItem->GetItemTitle()))
		{
			m_pLstCtrl->AddItem(pItem);
			continue;
		}

		if (pCurType->GetTypeId() == AFileItemType::AFI_DEFAULT)
		{
			m_pLstCtrl->AddItem(pItem);
			continue;
		}
	}

	if (m_pLstCtrl->GetSortType() != AListCtrl2::SORT_NAME)
		m_pLstCtrl->SortBy(m_pLstCtrl->GetSortType(), FALSE);
}

bool AFileDialog2::IgnoreItem(const TCHAR* szTitle)
{
	if (!IsDisplayLod() && StrStrI(szTitle, TEXT("_lod")) != NULL)
		return true;

	return false;
}

const AFileItemType* AFileDialog2::GetCurFilterType() const
{	
	int iFileFilterSel = m_cbFileFilter.GetCurSel();
	int iCurrentSelType = (int)m_cbFileFilter.GetItemData(iFileFilterSel);
	ASSERT (iCurrentSelType >= 0 || iCurrentSelType < AFileItemType::AFI_NUM);
	return m_aSupportedTypes[iCurrentSelType];
}

int AFileDialog2::GetFilterCBIdxByType(int iType)
{
	for (int iIdx = 0; iIdx < m_cbFileFilter.GetCount(); ++iIdx)
	{
		if (iType == (int)m_cbFileFilter.GetItemData(iIdx))
			return iIdx;
	}

	return -1;
}

//	Create a file dialog
INT_PTR AFileDialog2::DoModal()
{
	m_hHostResourceHandle = AfxGetResourceHandle();
	AfxSetResourceHandle(theApp.m_hInstance);

	// generate the Full-base-path, if failed, we do not create the dialog
	if (!FormatInitDir(m_strInitRelativeDir, m_strInitSubPath))
	{
		m_nErrorCode = AFDE_PATHNOTEXIST;
		return m_nErrorCode;
	}

	return BaseDialog::DoModal();
}

//	Get relative file path with out basedir and initdir.
const TCHAR* AFileDialog2::GetRelativeFileName()
{
	int iFind = m_strSelFileFullName.Find(m_strFullBaseDir);
	if (iFind < 0)
		return _T("");
	m_strRelativePath = m_strSelFileFullName.Right(m_strSelFileFullName.GetLength() - m_strFullBaseDir.GetLength());
	return m_strRelativePath;
}

//	Get full file path name
const TCHAR* AFileDialog2::GetFullFileName() const
{
	return m_strSelFileFullName;
}

//	Get selected files
const AFVector<CString> AFileDialog2::GetSelFiles() const
{
	return m_aSelFiles;
}

void AFileDialog2::InitFileFilters()
{
	m_cbFileFilter.ResetContent();

	for (int iSupportTypeIdx = 0; iSupportTypeIdx < AFileItemType::AFI_NUM; ++iSupportTypeIdx)
	{
		AFileItemType* pItemType = m_aSupportedTypes[iSupportTypeIdx];
		if (!pItemType->IsValidFileType())
			continue;

		CString strFilter;
		strFilter.Format(_T("%s (%s)"), pItemType->GetTypeName(), pItemType->GetFilterStr());
		int nIdx = m_cbFileFilter.AddString(strFilter);
		m_cbFileFilter.SetItemData(nIdx, iSupportTypeIdx);
	}

	m_cbFileFilter.SetCurSel(m_cbFileFilter.GetCount() - 1);
}

void AFileDialog2::OnSelchangeCbFiletypes() 
{
	const AFileItemType* pType = GetCurFilterType();
	UpdateListCtrlByFilter(pType);

	if (pType)
		m_afConfigs.OnCurSelectedFilterChanged(pType->GetTypeId());
}

void AFileDialog2::OnSelchangeCbPathShortcut()
{
	int nSel = m_cbPathShortCut.GetCurSel();
	if (nSel == -1)
		return;

	CString strFormatCurPath = m_strFullBaseDir;
	int nIdx = 0;
	while (nIdx != nSel)
	{
		strFormatCurPath += m_aDirs.GetAt(nIdx);
		strFormatCurPath += _T("\\");
		++nIdx;
	}

	if (m_strCurDir == strFormatCurPath)
		return;

	SetCurDir(strFormatCurPath);
}

LRESULT AFileDialog2::OnFolderProcCompleted(WPARAM wParam, LPARAM lParam)
{
	const TCHAR * szFolder = (const TCHAR*)lParam;
	if (!szFolder || _tcslen(szFolder) == 0)
		return 0;
	
	if (m_strCurDir.CompareNoCase(szFolder) != 0)
		return 0;
	


	return 0;
}

LRESULT AFileDialog2::OnUpdateOneImage(WPARAM wParam, LPARAM lParam)
{
	if (!m_FileItemImages.GetSafeHandle())
		return 0;

	UPDATE_IMAGE *upImg = (UPDATE_IMAGE*)wParam;
	if (!upImg || !upImg->pICLItem || upImg->strUpdatePath.GetLength() == 0)
		return 0;

	CString strCompletedDir = upImg->strUpdatePath;
	if (strCompletedDir.CompareNoCase(m_strCurDir) != 0)
		return 0;

	AFileItem* pFileItem = NULL;
	int iIdx;
	for (iIdx = 0; iIdx < m_aFileItems.GetSize(); ++iIdx)
	{
		if (upImg->strFileName.Compare(m_aFileItems[iIdx]->GetItemTitle()) != 0)
			continue;

		pFileItem = m_aFileItems[iIdx];
		pFileItem->ChangeSizeText(upImg->pICLItem->dwHeight, upImg->pICLItem->dwWidth);
		pFileItem->LoadFileImage(upImg->pICLItem->pBuf, upImg->pICLItem->dwDataLen);
		pFileItem->SetThumbImgIdx(0, m_FileItemImages.Add(pFileItem->GetBitmap(false), RGB(0, 0, 0)));
		pFileItem->SetThumbImgIdx(1, m_FileItemImages.Add(pFileItem->GetBitmap(true), RGB(0, 0, 0)));
		break;
	}

	if (!pFileItem)
		return 0;
	
	for (iIdx = 0; iIdx < m_pLstCtrl->GetItemCount(); ++iIdx)
	{
		if (pFileItem != m_pLstCtrl->GetItem(iIdx))
			continue;

		RECT rc;
		m_pLstCtrl->GetItemRect(iIdx, &rc, LVIR_BOUNDS);
		m_pLstCtrl->InvalidateRect(&rc);
		break;
	}

	return 0;
}

void AFileDialog2::SetCurDir(const TCHAR* szDir, bool bLogHis /*= true*/)
{
	if (m_strCurDir == szDir)
		return;

	if (!m_strCurDir.IsEmpty() && bLogHis)
		m_aDirHistory.Add(m_strCurDir);

	m_strCurDir = szDir;
	UpdateOnCurDirChanged(szDir);

	UpdatePathShortCut();
}

void AFileDialog2::UpdateOnCurDirChanged(const TCHAR * szCurDir)
{
	// 拷贝一份，szCurDir可能失效
	CString strCurDir(szCurDir);
	ASSERT(m_strCurDir == strCurDir);
	
	CreateFileItemsForDir(strCurDir);

//zjy#ifdef _ANGELICA2
//zjy	m_pICLPool->PushTask(new ICLTaskProcFolder(m_pDevice, szCurDir, m_pMsgNorifier));
//zjy#elif _ANGELICA31

	if (GetPreviewWnd())
	{
		m_pICLPool->PushTask(new ICLTaskProcFolder(strCurDir, m_pMsgNorifier
			, GetPreviewWnd()->GetA3DDevice()
			, GetPreviewWnd()->GetA3DGfxEngine()
			, GetPreviewWnd()->GetA3DCamera()
			, GetPreviewWnd()->GetA3DViewPort()
			, GetPreviewWnd()->GetA3DRenderTarget()
			/*, GetPreviewWnd()->GetA3DAdditionalView()*/));
	}

//zjy#endif

	m_afConfigs.OnCurRelativePathChanged(strCurDir);
}

void AFileDialog2::InitWndSize(int cx, int cy)
{
	CRect rc;
	GetWindowRect(&rc);
	rc.right += (cx - rc.Width());
	rc.bottom += (cy - rc.Height());
	MoveWindow(&rc);
}


int AFileDialog2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (BaseDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

LRESULT AFileDialog2::OnUpdateFirstFolder(WPARAM wParam, LPARAM lParam)
{
	CString strOriCur = m_strCurDir;
	m_strCurDir.Empty();
	SetCurDir(strOriCur);
	return 0;
}

LRESULT AFileDialog2::OnDbkSelectFile(WPARAM wParam, LPARAM lParam)
{
	OnOK();
	return 0;
}

BOOL AFileDialog2::OnInitDialog()
{
	BaseDialog::OnInitDialog();

	InitFileFilters();
	LoadResources();

	m_FileItemImages.Create(IMG_SIZE_THUMB, IMG_SIZE_THUMB, ILC_COLOR24, 5, 10);

	CRect dlgRc;
	GetWindowRect(&dlgRc);

	CRect clgRc;
	GetClientRect(&clgRc);
	m_origwsize.x = clgRc.Width();
	m_origwsize.y = clgRc.Height();
	m_lastwsize.x = clgRc.Width();
	m_lastwsize.y = clgRc.Height();

	RECT rc;
	rc.left = static_cast<LONG>(dlgRc.Width() * 0.02f);
	rc.right = static_cast<LONG>(dlgRc.Width() * ( 1.f - 0.02f));
	rc.top = static_cast<LONG>(dlgRc.Height() * 0.08f);
	rc.bottom = static_cast<LONG>(dlgRc.Height() * 0.72f);

	m_cxAlphaBlend.SetCheck(m_afConfigs.GetLastCheckedAlphaBlendSwitch() ? BST_CHECKED : 0);
	m_pLstCtrl->SetUseThumbImgType(IsAlphaBlendChecked() ? 1 : 0);

	m_pLstCtrl->SetThumbNailImgList(&m_FileItemImages);
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER;
	dwStyle |= !m_bIsOpenFile ? LVS_SINGLESEL : 0;
	m_pLstCtrl->Create(dwStyle, rc, this, IDC_LST_FILES);
	m_pLstCtrl->UpdateWindow();
	m_pLstCtrl->ShowWindow(SW_SHOW);
	m_pLstCtrl->SetLVType(m_nListStyle);//SetLVType(AListCtrl2::LV_THUMBVIEW);
	m_pLstCtrl->SetSortType(m_nSortStyle);

	m_pLstCtrl->SetListener(m_pLstCtrlListener);


	for (int iSupportTypeIdx = 0; iSupportTypeIdx < AFileItemType::AFI_NUM; ++iSupportTypeIdx)
		m_aSupportedTypes[iSupportTypeIdx]->OnUseDefaultImage(m_pLstCtrl);

	if (m_aCustomExtTypes.empty())
	{
		int iSelIdx = GetFilterCBIdxByType(m_afConfigs.GetLastSelectedFilter());
		m_cbFileFilter.SetCurSel(iSelIdx >= 0 ? iSelIdx : m_cbFileFilter.GetCount() - 1);
	}
	else
		m_cbFileFilter.SetCurSel(GetFilterCBIdxByType(AFileItemType::AFI_CUSTOM));

	if (!m_bIsOpenFile)
		m_btOK.SetWindowText(_T("&Save"));

	m_dynSizeArray.push_back(DynSizeCtrl(m_pLstCtrl, this, false, true, false, true, true, true, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_cbPathShortCut, this, false, true, false, true, false, false, true, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_cbFileFilter, this, true, false, false, true, true, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_edFileName, this, true, false, false, true, true, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_txtLookIn, this, false, true, false, true, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_txtFileInfo, this, true, false, false, true, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_txtFileName, this, true, false, false, true, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_txtFileType, this, true, false, false, true, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_btOK, this, true, false, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_btCancel, this, true, false, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_OpenFolder, this, true, false, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_btBack, this, false, true, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_btUp, this, false, true, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_cxAlphaBlend, this, false, true, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_cxPWndShow, this, false, true, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_cxDisplayLod, this, true, false, true, false, false, false, false, false));

	m_btUp.SetBitmap(m_bmArrowUp);
	m_btBack.SetBitmap(m_bmArrowBack);

	AfxSetResourceHandle(m_hHostResourceHandle);

	InitWndSize(m_afConfigs.GetLastCX(), m_afConfigs.GetLastCY());

	CreatePreviewWnd();

	PostMessage(WM_UPDATE_FIRST_FOLDER, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void AFileDialog2::DoDataExchange(CDataExchange* pDX)
{
	BaseDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(AFileDialog)
	DDX_Control(pDX, IDC_BT_UP, m_btUp);
	DDX_Control(pDX, IDC_BT_BACK, m_btBack);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_STATIC_LOOKIN, m_txtLookIn);
	DDX_Control(pDX, IDC_STATIC_FILETYPE, m_txtFileType);
	DDX_Control(pDX, IDC_STATIC_FILEINFO, m_txtFileInfo);
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_txtFileName);
	DDX_Control(pDX, IDC_STR_FILENAME, m_edFileName);
	DDX_Control(pDX, IDC_CB_PATH_SHORTCUT, m_cbPathShortCut);
	DDX_Control(pDX, IDC_CB_FILETYPES, m_cbFileFilter);
	DDX_Control(pDX, IDC_IS_ALPHA_BLEND, m_cxAlphaBlend);
	DDX_Control(pDX, IDC_IS_PWINDOW_SHOW, m_cxPWndShow);
	DDX_Control(pDX, IDC_DISPLAY_LOD, m_cxDisplayLod);
	DDX_Control(pDX, IDC_OpenFolder, m_OpenFolder);
	//}}AFX_DATA_MAP
}

void AFileDialog2::OnSize(UINT nType, int cx, int cy)
{
	BaseDialog::OnSize(nType, cx, cy);

	for (DynSizeCtrlIterator itr = m_dynSizeArray.begin(); itr != m_dynSizeArray.end(); ++itr)
		itr->ResizeCtrl(cx, cy, m_lastwsize, m_origwsize);

	m_lastwsize.x = cx;
	m_lastwsize.y = cy;
}

void AFileDialog2::OnSizing(UINT fwSide, LPRECT lpRect)
{
	BaseDialog::OnSizing(fwSide, lpRect);

	switch(fwSide)
	{
	case WMSZ_LEFT:

		if (lpRect->right - lpRect->left < AFILE_DLG_MIN_WIDTH) {
			lpRect->left = lpRect->right - AFILE_DLG_MIN_WIDTH;
		}

		break;

	case WMSZ_TOP:

		if (lpRect->bottom - lpRect->top < AFILE_DLG_MIN_HEIGHT) {
			lpRect->top = lpRect->bottom - AFILE_DLG_MIN_HEIGHT;
		}

		break;

	case WMSZ_RIGHT:

		if (lpRect->right - lpRect->left < AFILE_DLG_MIN_WIDTH) {
			lpRect->right = lpRect->left + AFILE_DLG_MIN_WIDTH;
		}

		break;

	case WMSZ_BOTTOM:

		if (lpRect->bottom - lpRect->top < AFILE_DLG_MIN_HEIGHT) {
			lpRect->bottom = lpRect->top + AFILE_DLG_MIN_HEIGHT;
		}

		break;

	case WMSZ_TOPLEFT:

		if (lpRect->bottom - lpRect->top < AFILE_DLG_MIN_HEIGHT) {
			lpRect->top = lpRect->bottom - AFILE_DLG_MIN_HEIGHT;
		}

		if (lpRect->right - lpRect->left < AFILE_DLG_MIN_WIDTH) {
			lpRect->left = lpRect->right - AFILE_DLG_MIN_WIDTH;
		}

		break;

	case WMSZ_BOTTOMLEFT:

		if (lpRect->bottom - lpRect->top < AFILE_DLG_MIN_HEIGHT) {
			lpRect->bottom = lpRect->top + AFILE_DLG_MIN_HEIGHT;
		}

		if (lpRect->right - lpRect->left < AFILE_DLG_MIN_WIDTH) {
			lpRect->left = lpRect->right - AFILE_DLG_MIN_WIDTH;
		}

		break;
	case WMSZ_BOTTOMRIGHT:

		if (lpRect->bottom - lpRect->top < AFILE_DLG_MIN_HEIGHT) {
			lpRect->bottom = lpRect->top + AFILE_DLG_MIN_HEIGHT;
		}

		if (lpRect->right - lpRect->left < AFILE_DLG_MIN_WIDTH) {
			lpRect->right = lpRect->left + AFILE_DLG_MIN_WIDTH;
		}

		break;
	case WMSZ_TOPRIGHT:

		if (lpRect->bottom - lpRect->top < AFILE_DLG_MIN_HEIGHT) {
			lpRect->top = lpRect->bottom - AFILE_DLG_MIN_HEIGHT;
		}

		if (lpRect->right - lpRect->left < AFILE_DLG_MIN_WIDTH) {
			lpRect->right = lpRect->left + AFILE_DLG_MIN_WIDTH;
		}

		break;
	}
}

bool AFileDialog2::IsPWndShow() const
{
	return m_cxPWndShow.GetCheck() == BST_CHECKED;
}

bool AFileDialog2::IsAlphaBlendChecked() const
{
	return m_cxAlphaBlend.GetCheck() == BST_CHECKED;
}

bool AFileDialog2::IsDisplayLod() const
{
	return m_cxDisplayLod.GetCheck() == BST_CHECKED;
}

int AFileDialog2::CreatePreviewWnd()
{
//zjy#ifdef _ANGELICA31

	if (GetPreviewWnd())
		GetPreviewWnd()->SetParent(NULL);

	AdjustPreviewWndPos();

//#endif

	return 0;
}

void AFileDialog2::DestroyPreviewWnd()
{
//zjy#ifdef _ANGELICA31

	if (GetPreviewWnd())
	{
		GetPreviewWnd()->SetPreviewFile(NULL);
		GetPreviewWnd()->SetParent(NULL);
		GetPreviewWnd()->ShowWindow(SW_HIDE);
		GetPreviewWnd()->SetWShow(false);
	}

//#endif
}


void AFileDialog2::AdjustPreviewWndPos()
{
//zjy#ifdef _ANGELICA31

	if (AFilePreviewWnd* pPreviewWnd = GetPreviewWnd())
	{
		CRect rcPreviewWnd;
		pPreviewWnd->GetWindowRect(&rcPreviewWnd);

		CRect rcFileDlg;
		GetWindowRect(&rcFileDlg);

#ifdef _ANGELICA31
		rcPreviewWnd.MoveToXY(rcFileDlg.right, rcFileDlg.top);
#elif defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
		int detx = rcFileDlg.right - rcPreviewWnd.TopLeft().x;
		int dety = rcFileDlg.top - rcPreviewWnd.TopLeft().y;
		rcPreviewWnd.OffsetRect(detx, dety);
		
#endif
		pPreviewWnd->MoveWindow(&rcPreviewWnd, TRUE);
		pPreviewWnd->SetWindowPos(CWnd::FromHandle(HWND_TOPMOST), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}

//#endif

}

//	Find cached file item by file name
AFileItem* AFileDialog2::FindFileItem(const TCHAR* szName, int* piIndex) const
{
	if (piIndex)
		*piIndex = -1;

	for (int iIdx = 0; iIdx < m_aFileItems.GetSize(); ++iIdx)
	{
		if (_tcsicmp(m_aFileItems[iIdx]->GetItemTitle(), szName) == 0)
		{
			if (piIndex)
				*piIndex = iIdx;

			return m_aFileItems[iIdx];
		}
	}

	return NULL;
}

void AFileDialog2::TryPlaySound(const TCHAR* szFile)
{
	if (is_support_sound(szFile) == AF_SUPPORT_SOUND_UNKNOWN)
		return;

#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
	AFTaskPlaySound task(m_pDevice, m_pSound, szFile);
#else
	AFTaskPlaySound task(CRender::GetInstance()->GetA3DDevice(), m_pSound, szFile);
#endif
	task.Do();
}

void AFileDialog2::StopSound()
{
	m_pSound->StopSound();
}

void AFileDialog2::OnItemClicked(AListItem* pItem)
{
	StopSound();

	if (!pItem || m_pLstCtrl->GetSelectedCount() == 0)
	{
		m_edFileName.SetWindowText(_T(""));

//zjy#ifdef _ANGELICA31

		if (GetPreviewWnd() && IsPWndShow())
			GetPreviewWnd()->SetPreviewFile(NULL);

//#endif

	}
	else
	{
		POSITION pos = m_pLstCtrl->GetFirstSelectedItemPosition();
		ASSERT( pos );
		int iItem = m_pLstCtrl->GetNextSelectedItem(pos);
		AListItem* pFirstSelItem = m_pLstCtrl->GetItem(iItem);
		AFileItem* pFileItem = (AFileItem*)(pFirstSelItem);

		CString fileinfo(_T("文件信息: "));
		
		if (pFileItem->GetItemType()->GetTypeId() == AFileItemType::AFI_IMAGE)
			fileinfo =  fileinfo + _T("ImageSize(w*h) ") + pFileItem->GetImageSize();

		m_txtFileInfo.SetWindowText(fileinfo);
		
		if (pFileItem->GetItemType()->GetTypeId() == AFileItemType::AFI_DIR)
			m_edFileName.SetWindowText(_T(""));
		else
			m_edFileName.SetWindowText(pFileItem->GetItemTitle());

//zjy#ifdef _ANGELICA31

		if (GetPreviewWnd() && IsPWndShow())
		{
			AFileItem* pFileItem = (AFileItem*)(pItem);
			GetPreviewWnd()->SetPreviewFile(pFileItem->GetFullPath());
			AdjustPreviewWndPos();
			m_pLstCtrl->SetFocus();
		}
//#endif

		TryPlaySound(pFileItem->GetFullPath());

	}
}

void AFileDialog2::OnItemDbClicked(AListItem* pItem)
{
	if (NULL == pItem)
	{
		m_edFileName.SetWindowText(_T(""));
		return;
	}

	if (m_pLstCtrl->GetSelectedCount() != 1)
		return;

	AFileItem* pFileItem = (AFileItem*)(pItem);
	if (!pFileItem)
		return;

	if (pFileItem->GetItemType()->GetTypeId() != AFileItemType::AFI_DIR)
	{
		OnOK();
	}
	else
	{
		CString strNewDir = pFileItem->GetFullPath();

		// after this function called 
		SetCurDir(strNewDir);
		// pFileItem is invalid...
		
	}
}

void AFileDialog2::UpdatePathShortCut()
{
	m_cbPathShortCut.ResetContent();

	COMBOBOXEXITEM _item;
	ZeroMemory(&_item, sizeof(_item));
	_item.mask = CBEIF_INDENT | CBEIF_IMAGE |CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
	_item.iIndent = 0;
	_item.iItem = 0;
	_item.iImage = 0;
	_item.pszText = (ACHAR*)(LPCTSTR)m_strFullBaseDir;
	m_cbPathShortCut.InsertItem(&_item);

	_SplitCurDir(m_strCurDir, m_strFullBaseDir, m_aDirs);
	if (m_strCurDir != m_strFullBaseDir && m_aDirs.GetSize())
	{
		int nItemIdx = 1;
		for (int i = 0; i < m_aDirs.GetSize(); ++i, ++nItemIdx)
		{
			_item.mask = CBEIF_INDENT | CBEIF_IMAGE |CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
			_item.iItem = nItemIdx;
			_item.iIndent = nItemIdx;
			_item.iImage = 0;
			_item.pszText = (ACHAR*)(LPCTSTR)m_aDirs.GetAt(i);
			m_cbPathShortCut.InsertItem(&_item);
		}
	}

	m_cbPathShortCut.SetCurSel(m_cbPathShortCut.GetCount() - 1);
}

void AFileDialog2::OnOK()
{
	if (m_bIsOpenFile)
	{
		if (0 == m_pLstCtrl->GetSelectedCount())
			return;

		POSITION pos = m_pLstCtrl->GetFirstSelectedItemPosition();
		while (pos)
		{
			int iItem = m_pLstCtrl->GetNextSelectedItem(pos);
			AListItem* pItem = m_pLstCtrl->GetItem(iItem);
			AFileItem* pFileItem = (AFileItem*)(pItem);
			if (!pFileItem)
				continue;

			if (pFileItem->GetItemType()->GetTypeId() != AFileItemType::AFI_DIR)
				continue;

			// directory item
			SetCurDir(pFileItem->GetFullPath());
			return;
		}

		m_aSelFiles.clear();

		// so no directory is selected here
		pos = m_pLstCtrl->GetFirstSelectedItemPosition();
		while (pos)
		{
			int iItem = m_pLstCtrl->GetNextSelectedItem(pos);
			AListItem* pItem = m_pLstCtrl->GetItem(iItem);
			AFileItem* pFileItem = (AFileItem*)(pItem);
			if (!pFileItem)
				continue;

			ASSERT( pFileItem->GetItemType()->GetTypeId() != AFileItemType::AFI_DIR );

			m_aSelFiles.push_back(pFileItem->GetFullPath());
			m_strSelFileFullName = pFileItem->GetFullPath();
		}
	}
	else
	{
		CString strInputName;
		m_edFileName.GetWindowText(strInputName);
		if (strInputName.IsEmpty()) 
		{
			m_edFileName.SetFocus();
			return;
		}

		int iIdx = -1;
		if (FindFileItem(strInputName, &iIdx) != NULL
			&& AfxMessageBox(_T("Are you sure to replace the existing file?"), MB_YESNO) != IDYES)
			return;
		else
			m_strSelFileFullName.Format(_T("%s%s"), (LPCTSTR)m_strCurDir, (LPCTSTR)strInputName);
	}

	BaseDialog::OnOK();
}


void AFileDialog2::OnBtBack() 
{
	if (!m_aDirHistory.GetSize())
		return;

	INT_PTR nIdx = m_aDirHistory.GetSize() - 1;
	CString strLastDir = m_aDirHistory.GetAt(nIdx);
	m_aDirHistory.RemoveAt(nIdx);
	SetCurDir(strLastDir, false);
	
}

void AFileDialog2::OnBtUp()
{
	if (m_strCurDir == m_strFullBaseDir)
		return;

	CString strNewCur = m_strCurDir;
	int iLastSlash = strNewCur.ReverseFind('\\');
	if (iLastSlash == strNewCur.GetLength() - 1)
	{
		strNewCur = strNewCur.Left(iLastSlash);
		iLastSlash = strNewCur.ReverseFind('\\');
		ASSERT( iLastSlash >= 0 );
		strNewCur = m_strCurDir.Left(iLastSlash + 1);
	}

	SetCurDir(strNewCur);
}

void AFileDialog2::OnOpenFolder()
{
	ShellExecute(NULL, TEXT("open"),m_strCurDir, NULL, NULL, SW_SHOWNORMAL);
}

void AFileDialog2::OnBnClickedIsAlphaBlend()
{
	m_afConfigs.OnCurAlphaBlendSwitchChanged(IsAlphaBlendChecked());
	m_pLstCtrl->SetUseThumbImgType(IsAlphaBlendChecked() ? 1 : 0);
	m_pLstCtrl->Invalidate();
}

void AFileDialog2::OnBnClickedIsWindowShow()
{
#ifdef _ANGELICA31

	// TODO: Add your control notification handler code here
	if (!IsPWndShow() && GetPreviewWnd())
	{
		GetPreviewWnd()->ShowWindow(SW_HIDE);
		GetPreviewWnd()->SetWShow(false);
	}

#endif
}

BOOL AFileDialog2::DestroyWindow()
{
	m_FileItemImages.DeleteImageList();

	CRect rc;
	GetWindowRect(&rc);
	m_afConfigs.OnSize(rc.Width(), rc.Height());
	m_afConfigs.OnChangeListStyle(m_pLstCtrl->GetLVType());
	m_afConfigs.OnChangeSortStyle(m_pLstCtrl->GetSortType());

	DestroyPreviewWnd();

	m_DestroyEvent.SetEvent();

	return BaseDialog::DestroyWindow();
}


void AFileDialog2::OnBnClickedDisplayLod()
{
	UpdateListCtrlByFilter(GetCurFilterType());
}

void AFileDialog2::SetInitSubDir(const TCHAR* szSubDir)
{
	m_strInitSubPath = szSubDir;
}

void AFileDialog2::OnCancel()
{
	BaseDialog::OnCancel();
}