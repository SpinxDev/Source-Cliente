// AFileDialog.cpp : implementation file
//

#include "stdafx.h"
#if 0
#include <afxpriv.h>
#include <process.h>
#include "AFileDialog.h"
#include "AFilePreviewWnd.h"
#include "AFileGfxInterface.h"
#include "Render.h"
#include "SyncWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _USE_BCGP

#include <BCGPVisualManager2003.h>

#endif

//const int IDC_LST_FILES = 2009;

extern CAFileDialogDllApp theApp;







static CBitmap* getBitmap(AFileDialog* pFileDialog, const TCHAR* szFullName)
{
	if (AFCheckFileExt(szFullName, _T(".jpg"))
		|| AFCheckFileExt(szFullName, _T(".bmp"))
		|| AFCheckFileExt(szFullName, _T(".dds"))
		|| AFCheckFileExt(szFullName, _T(".png"))
		|| AFCheckFileExt(szFullName, _T(".tif"))
		|| AFCheckFileExt(szFullName, _T(".tga")))
		return &pFileDialog->m_bmPicture;
	if (AFCheckFileExt(szFullName, _T(".txt")))
		return &pFileDialog->m_bmText;
	if (AFCheckFileExt(szFullName, _T(".wav"))
		|| AFCheckFileExt(szFullName, _T(".mp3"))
		|| AFCheckFileExt(szFullName, _T(".ogg")))
		return &pFileDialog->m_bmSound;

#ifdef _ANGELICA31
	if (AFCheckFileExt(szFullName, _T(".ecm3"))
		||AFCheckFileExt(szFullName, _T(".gfx3"))
		||AFCheckFileExt(szFullName, _T(".SMD"))
		||AFCheckFileExt(szFullName, _T(".umd"))
		||AFCheckFileExt(szFullName, _T(".umds")))
		return &pFileDialog->m_bmPicture;
#endif
	return &pFileDialog->m_bmOther;
}


class AFileDialogMsgReciever : public ICLNotifier
{
public:
	AFileDialogMsgReciever(AFileDialog* pFD)
		: m_pFD(pFD)
	{
		ASSERT(m_pFD);
	}
	~AFileDialogMsgReciever() {}

protected:
	virtual void FolderProcComplete(const TCHAR * szFolder);

private:
	AFileDialog* m_pFD;
};

void AFileDialogMsgReciever::FolderProcComplete(const TCHAR * szFolder)
{
	// Be careful, for this part of code will be run in a worker thread.
	m_pFD->SendMessage(WM_FOLDER_PROC_COMPLETED, NULL, (LPARAM)szFolder);
	//m_pFD->onUpdateFileList(szFolder);
}

class AListCtrlListener : public AListCtrl::Listener
{
public:
	AListCtrlListener(AFileDialog* pFileDialog)
		: m_pFileDialog(pFileDialog)
	{

	}
	
	virtual void OnItemClicked(int nItem);
	virtual void OnItemDbClicked(int nItem);

private:
	AFileDialog* m_pFileDialog;
};

void AListCtrlListener::OnItemClicked(int nItem)
{
	m_pFileDialog->OnFileLstClick(nItem);
}

void AListCtrlListener::OnItemDbClicked(int nItem)
{
	m_pFileDialog->OnFileLstDblClick(nItem);
}

void AFileDialog::AFITEM::SetFileTime(const FILETIME& ft)
{
	ftFiletime = ft;
}

void AFileDialog::AFITEM::SetFileSize(ULONGLONG dwFs)
{
	dwFilesize = dwFs;
}

time_t AFileDialog::AFITEM::GetTime() const
{
	CTime t(ftFiletime);
	return t.GetTime();
}

void AFileDialog::AFITEM::SetFullName(const TCHAR * szFullname)
{
	strFullname = szFullname;
}

AFileDialog::AFITEM_Image::AFITEM_Image() 
{
	SetType(AFT_IMG);
}

AFileDialog::AFITEM_Image::~AFITEM_Image()
{
}

void AFileDialog::AFITEM_Image::LoadFromMemory(unsigned char* pBuf, long size)
{
	pThumbnail.ReadFromMemory(pBuf, size);
}

CBitmap* AFileDialog::AFITEM_Image::GetBitmap(bool bIsAlphaBlend)
{
	return pThumbnail.GetAsBitmap(bIsAlphaBlend);
}

CBitmap* AFileDialog::AFITEM_Custom::GetBitmap(bool bIsAlphaBlend)
{
	if (m_bIsImage)
		return base_class::GetBitmap(bIsAlphaBlend);
	else
		return getBitmap(m_pFileDialog, GetFullName());
}

CBitmap* AFileDialog::AFITEM_Other::GetBitmap(bool)
{
	return getBitmap(pFileDialog, GetFullName());
}

CBitmap* AFileDialog::AFITEM_Default::GetBitmap(bool)
{
	return getBitmap(pFileDialog, GetFullName());
}

//////////////////////////////////////////////////////////////////////////
//
//	Implementation of AFileDialog::AFConfigures
//
//////////////////////////////////////////////////////////////////////////

TCHAR const * AFileDialog::AFConfigures::s_szEntryLastOpenRelativePath		= _T("LastOpenRelativePath");
TCHAR const * AFileDialog::AFConfigures::s_szEntryLastSelectedFilter		= _T("LastSelectedFilter");
TCHAR const * AFileDialog::AFConfigures::s_szEntryLastCheckedAlphaSwitch	= _T("LastCheckedAlphaSwitch");
TCHAR const * AFileDialog::AFConfigures::s_szEntryLastWndWidth				= _T("LastWndWidth");
TCHAR const * AFileDialog::AFConfigures::s_szEntryLastWndHeight				= _T("LastWndHeight");
TCHAR const * AFileDialog::AFConfigures::s_szEntryLastListStyle				= _T("LastListStyle");
TCHAR const * AFileDialog::AFConfigures::s_szEntryLastSortStyle				= _T("LastSortStyle");

const TCHAR* AFileDialog::AFConfigures::GetRegSection() const
{
	return _T("AFSettings");
}

void AFileDialog::AFConfigures::LoadSettingsFromReg()
{
	m_strLastOpenRelativePath	= AfxGetApp()->GetProfileString(GetRegSection(), s_szEntryLastOpenRelativePath, m_strLastOpenRelativePath);
	m_nLastSelectedFilter		= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastSelectedFilter, m_nLastSelectedFilter);
	m_bLastCheckedAlphaSwitch	= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastCheckedAlphaSwitch, m_bLastCheckedAlphaSwitch);
	m_nCX						= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastWndWidth, m_nCX);
	m_nCY						= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastWndHeight, m_nCY);
	m_nListStyle				= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastListStyle, m_nListStyle);
	m_nSortStyle				= AfxGetApp()->GetProfileInt(GetRegSection(), s_szEntryLastSortStyle, m_nSortStyle);
}

void AFileDialog::AFConfigures::SaveSettingsToReg()
{
	AfxGetApp()->WriteProfileString(GetRegSection(), s_szEntryLastOpenRelativePath, m_strLastOpenRelativePath);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastSelectedFilter, m_nLastSelectedFilter);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastCheckedAlphaSwitch, m_bLastCheckedAlphaSwitch);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastWndWidth, m_nCX);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastWndHeight, m_nCY);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastListStyle, m_nListStyle);
	AfxGetApp()->WriteProfileInt(GetRegSection(), s_szEntryLastSortStyle, m_nSortStyle);
}

void AFileDialog::AFConfigures::OnCurRelativePathChanged(const TCHAR* szCurFull) 
{
	AString strCurFull = AC2AS(szCurFull);
	AString strCurRel;
	af_GetRelativePathNoBase(strCurFull, af_GetBaseDir(),  strCurRel);
	m_strLastOpenRelativePath = AS2AC(strCurRel);
}

//////////////////////////////////////////////////////////////////////////
//
//	Implementation of AFileDialog
//
//////////////////////////////////////////////////////////////////////////

bool AFileDialog::formatInitDir(const TCHAR * szRelativeDir)
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

void AFileDialog::initFileFilter()
{
	m_cbFileFilter.ResetContent();
	int nIdx = -1;
#ifdef _ANGELICA2
	nIdx = m_cbFileFilter.AddString(_AL("Texture Files (*.bmp, *.tga, *.jpg, *.png, *.dds)"));
#elif _ANGELICA31
	nIdx = m_cbFileFilter.AddString(_AL("Texture Files (*.bmp, *.tga, *.jpg, *.png, *.dds, *.ecm3, *.gfx3, *.SMD, *.umd, *.umds)"));
#endif
	m_cbFileFilter.SetItemData(nIdx, AFF_TEXTUREFILE);

	nIdx = m_cbFileFilter.AddString(_AL("Sound Files (*.mp3, *.ogg, *.wav)"));
	m_cbFileFilter.SetItemData(nIdx, AFF_SOUNDFILE);
	
	nIdx = m_cbFileFilter.AddString(_AL("Text Files (*.txt)"));
	m_cbFileFilter.SetItemData(nIdx, AFF_TEXTFILE);

	//if (!m_strExt.IsEmpty())
	//{
	//	CString strCustomFiles;
	//	strCustomFiles.Format(_AL("Custom Files (*.%s)"), m_strExt);
	//	nIdx = m_cbFileFilter.AddString(strCustomFiles);
	//	m_cbFileFilter.SetItemData(nIdx, AFF_CUSTOMFILE);
	//}

	if (!m_aCustomExtTypes.empty())
	{
		CString strCustomExts;
		strCustomExts.Format(_AL("Custom Files ("));
		for (size_t nExtIdx = 0; nExtIdx < m_aCustomExtTypes.size(); ++nExtIdx)
		{
			CString strTmp;
			strTmp.Format(_AL("*.%s"), m_aCustomExtTypes[nExtIdx]);
			strCustomExts += strTmp;

			if (nExtIdx != m_aCustomExtTypes.size() - 1)
				strCustomExts += _AL(' ');
		}
		strCustomExts += _AL(")");

		nIdx = m_cbFileFilter.AddString(strCustomExts);
		m_cbFileFilter.SetItemData(nIdx, AFF_CUSTOMFILE);
	}

	nIdx = m_cbFileFilter.AddString(_AL("All Files (*.*)"));
	m_cbFileFilter.SetItemData(nIdx, AFF_ALL);
	m_cbFileFilter.SetCurSel(getSelIdxByFilterType(AFF_ALL));
}

int AFileDialog::getFilterByExt(const TCHAR * szExt)
{
	CString strExt(szExt);
	strExt.TrimLeft(_AL("."));
#ifdef _ANGELICA2
	if (strExt.CompareNoCase(_AL("jpg")) == 0
		|| strExt.CompareNoCase(_AL("tga")) == 0
		|| strExt.CompareNoCase(_AL("png")) == 0
		|| strExt.CompareNoCase(_AL("dds")) == 0
		|| strExt.CompareNoCase(_AL("bmp")) == 0)
		return AFF_TEXTUREFILE;
#elif _ANGELICA31
	if (strExt.CompareNoCase(_AL("jpg")) == 0
		|| strExt.CompareNoCase(_AL("tga")) == 0
		|| strExt.CompareNoCase(_AL("png")) == 0
		|| strExt.CompareNoCase(_AL("dds")) == 0
		|| strExt.CompareNoCase(_AL("bmp")) == 0
		|| strExt.CompareNoCase(_AL("ecm3")) == 0
		|| strExt.CompareNoCase(_AL("gfx3")) == 0
		|| strExt.CompareNoCase(_AL("SMD")) == 0
		|| strExt.CompareNoCase(_AL("umd")) == 0
		|| strExt.CompareNoCase(_AL("umds")) == 0)
		return AFF_TEXTUREFILE;
#endif
	else if (strExt.CompareNoCase(_AL("txt")) == 0)
		return AFF_TEXTFILE;
	else if (strExt.CompareNoCase(_AL("mp3")) == 0
		|| strExt.CompareNoCase(_AL("wav")) == 0
		|| strExt.CompareNoCase(_AL("ogg")) == 0)
		return AFF_SOUNDFILE;
	else 
		/*if (strExt.CompareNoCase(m_strExt) == 0)
		return AFF_CUSTOMFILE;*/
	{
		for (size_t nIdx = 0; nIdx < m_aCustomExtTypes.size(); ++nIdx)
		{
			if (strExt.CompareNoCase(m_aCustomExtTypes[nIdx]) == 0)
				return AFF_CUSTOMFILE;
		}
	}
	return AFF_ALL;
}

void AFileDialog::splitCurDir(const CString& strCurDir, const CString& strBaseDir, CStringArray& aDirs)
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

void AFileDialog::updatePathShortCut()
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

	splitCurDir(m_strCurDir, m_strFullBaseDir, m_aDirs);
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

void AFileDialog::updateOnCurDirChanged(const TCHAR * szCurDir, bool bLogHis)
{
	if (m_strCurDir == szCurDir)
		return;

	SyncWrapper<CCriticalSection> glock(global_cs);//zjy

	if (!m_strCurDir.IsEmpty() && bLogHis)
		m_aDirHistory.Add(m_strCurDir);
	
	m_strCurDir = szCurDir;

#ifdef _ANGELICA2
	m_pICLPool->PushTask(new ICLTaskProcFolder(m_pDevice, szCurDir, m_pMsgNorifier));
#elif _ANGELICA31
	m_pICLPool->PushTask(new ICLTaskProcFolder(szCurDir, m_pMsgNorifier,AFilePreviewWnd::GetInstance()->GetA3DDevice(),AFilePreviewWnd::GetInstance()->GetA3DGfxEngine(),AFilePreviewWnd::GetInstance()->GetA3DCamera(),AFilePreviewWnd::GetInstance()->GetA3DViewPort(),AFilePreviewWnd::GetInstance()->GetA3DAdditionalView()));
#endif

	updateFileListByDefaultIcon(szCurDir);
	m_afConfigs.OnCurRelativePathChanged(szCurDir);
}

void AFileDialog::updateFileListByDefaultIcon(const TCHAR * szDir)
{
	if (m_strCurDir != szDir)
		return;

	clearPointerArray(m_aItems);

	CFileFind finder;
	CString strBase;
	strBase.Format(_T("%s\\%s"), szDir, _T("*.*"));
	BOOL bWorking = finder.FindFile(strBase);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (is_skip_file(finder))
			continue;

		if (finder.IsDirectory())
		{
			AFITEM_Directory* pAFItem = new AFITEM_Directory(&m_bmDirectory);
			CTime time;
			finder.GetLastWriteTime(time);
			pAFItem->SetFileTime(ctime2FileTile(time));
			pAFItem->SetFileSize(0);
			pAFItem->SetFullName(finder.GetFilePath());
			m_aItems.push_back(pAFItem);
			continue;
		}

		AFITEM_Default* pAFItem = new AFITEM_Default(this);
		CTime time;
		finder.GetLastWriteTime(time);
		pAFItem->SetFileTime(ctime2FileTile(time));
		pAFItem->SetFileSize(finder.GetLength());
		pAFItem->SetFullName(finder.GetFilePath());
		m_aItems.push_back(pAFItem);
	}
	
	updateListItems(getFilterTypeBySel(m_cbFileFilter));
}

bool AFileDialog::isAFItemInCustom(const AFITEM& item)
{
	for (size_t nIdx = 0; nIdx < m_aCustomExtTypes.size(); ++nIdx)
	{
		CString strTmp(item.GetFullName());
		CString strExtCur(m_aCustomExtTypes[nIdx]);
		strTmp.MakeUpper();
		strExtCur.MakeUpper();
		int iStart = strTmp.GetLength() - strExtCur.GetLength();
		if (strTmp.Find(strExtCur, iStart) == iStart)
			return true;
	}
	return false;
}

bool AFileDialog::checkIsInExt(const TCHAR ch)
{
	return isalpha(ch) || isdigit(ch);
}

void AFileDialog::updateCustomExtFiles(const TCHAR* szCustomExts)
{
	m_aCustomExtTypes.clear();

	CString strExt(szCustomExts);
	strExt.TrimLeft();
	strExt.TrimRight();
	if (strExt.IsEmpty())
		return;

	int iCurIdx = 0;
	CString strCur;
	for (; iCurIdx != strExt.GetLength(); ++iCurIdx)
	{
		if (checkIsInExt(strExt[iCurIdx]))
			strCur += strExt[iCurIdx];
		else if (!strCur.IsEmpty())
		{
			m_aCustomExtTypes.push_back(strCur);
			strCur.Empty();
		}
	}

	if (!strCur.IsEmpty())
	{
		m_aCustomExtTypes.push_back(strCur);
		strCur.Empty();
	}
}

int AFileDialog::getFilterTypeBySel(const AComboBox& cbFilter)
{
	int iSel = cbFilter.GetCurSel();
	DWORD_PTR dwpData = cbFilter.GetItemData(iSel);
	return static_cast<int>(dwpData);
}

int AFileDialog::getSelIdxByFilterType(int iFilter)
{
	const AComboBox& cbFilter = m_cbFileFilter;
	int iSelIdx = 0;
	for (; iSelIdx < cbFilter.GetCount(); ++iSelIdx)
	{
		if (cbFilter.GetItemData(iSelIdx) == (DWORD_PTR)iFilter)
		{
			return iSelIdx;
		}
	}
	return iSelIdx - 1;
}

void AFileDialog::onUpdateFileList(const TCHAR * szDir)
{
	if (m_strCurDir == szDir)
	{
		m_pICLFile->Open(szDir);
		
		clearPointerArray(m_aItems);

		CFileFind finder;
		CString strBase;
		strBase.Format(_T("%s\\%s"), szDir, _T("*.*"));
		BOOL bWorking = finder.FindFile(strBase);
		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			if (is_skip_file(finder))
				continue;

			if (finder.IsDirectory())
			{
				AFITEM_Directory* pAFItem = new AFITEM_Directory(&m_bmDirectory);
				CTime time;
				finder.GetLastWriteTime(time);
				pAFItem->SetFileTime(ctime2FileTile(time));
				pAFItem->SetFileSize(0);
				pAFItem->SetFullName(finder.GetFilePath());
				m_aItems.push_back(pAFItem);
				continue;
			}
			
			CString strFileName = finder.GetFileName();
			int nExtIndex = strFileName.ReverseFind('.');
			if (nExtIndex != -1)
			{
				int nFilterType = getFilterByExt(strFileName.Right(strFileName.GetLength() - nExtIndex));
				if (nFilterType == AFF_TEXTUREFILE) 
				{
					ICLFile::ICLItem* pItem = m_pICLFile->GetICLItemByName(finder.GetFileName());
					if (!pItem)
					{
						AFITEM_Other* pAFItem = new AFITEM_Other(this);
						CTime time;
						finder.GetLastWriteTime(time);
						pAFItem->SetFileTime(ctime2FileTile(time));
						pAFItem->SetFileSize(finder.GetLength());
						pAFItem->SetFullName(finder.GetFilePath());
						m_aItems.push_back(pAFItem);
						continue;
					}
					AFITEM_Image* pAFItem = new AFITEM_Image;
					pAFItem->SetFileSize(pItem->dwFileSize);
					pAFItem->SetFileTime(pItem->ftTime);
					pAFItem->SetFullName(finder.GetFilePath());
					pAFItem->LoadFromMemory(pItem->pBuf, pItem->dwDataLen);
					m_aItems.push_back(pAFItem);
					continue;
				}
				else if (nFilterType == AFF_SOUNDFILE)
				{
					AFITEM_Sound* pAFItem = new AFITEM_Sound(&m_bmSound);
					CTime time;
					finder.GetLastWriteTime(time);
					pAFItem->SetFileTime(ctime2FileTile(time));
					pAFItem->SetFileSize(finder.GetLength());
					pAFItem->SetFullName(finder.GetFilePath());
					m_aItems.push_back(pAFItem);
					continue;
				}
				else if (nFilterType == AFF_CUSTOMFILE)
				{
					ICLFile::ICLItem* pItem = m_pICLFile->GetICLItemByName(finder.GetFileName());
					bool bIsExistImage = pItem != NULL;
					AFITEM_Custom* pAFItem = new AFITEM_Custom(this, bIsExistImage);
					CTime time;
					finder.GetLastWriteTime(time);
					pAFItem->SetFileTime(ctime2FileTile(time));
					pAFItem->SetFileSize(finder.GetLength());
					pAFItem->SetFullName(finder.GetFilePath());
					if (bIsExistImage)
						pAFItem->LoadFromMemory(pItem->pBuf, pItem->dwDataLen);
					m_aItems.push_back(pAFItem);
					continue;
				}
			}
			
			AFITEM_Other* pAFItem = new AFITEM_Other(this);
			CTime time;
			finder.GetLastWriteTime(time);
			pAFItem->SetFileTime(ctime2FileTile(time));
			pAFItem->SetFileSize(finder.GetLength());
			pAFItem->SetFullName(finder.GetFilePath());
			m_aItems.push_back(pAFItem);
		}

		updateListItems(getFilterTypeBySel(m_cbFileFilter));
	}
}

void AFileDialog::updateListItems(int nFilterType)
{
	m_pLstFiles->ClearItems();

	AFVector<AListCtrl::_FILEITEM> aFI;
	for (AFItemIterator itr = m_aItems.begin(); itr != m_aItems.end(); ++itr)
	{
		AFITEM& afItem = **itr;
		//if (afItem.GetType() != AFT_DIR
		//	&& afItem.GetType() != AFT_DEFAULT
		//	&& ((nFilterType != AFF_ALL
		//		&& (nFilterType == AFF_TEXTUREFILE && afItem.GetType() != AFT_IMG)
		//		&& (nFilterType == AFF_SOUNDFILE && afItem.GetType() != AFT_SOUND)
		//		&& (nFilterType == AFF_TEXTFILE && afItem.GetType() != AFT_TEXT)
		//		&& (nFilterType == AFF_CUSTOMFILE && !isAFItemInCustom(afItem))
		//		)
		//	|| (nFilterType == AFF_TEXTUREFILE && afItem.GetType() != AFT_IMG)
		//	|| (nFilterType == AFF_SOUNDFILE && afItem.GetType() != AFT_SOUND)
		//	|| (nFilterType == AFF_TEXTFILE && afItem.GetType() != AFT_TEXT)
		//	|| (nFilterType == AFF_CUSTOMFILE && !isAFItemInCustom(afItem)))
		//	)
		//	continue;

		if (afItem.GetType() == AFT_DIR)
			goto L_OK;

		if (afItem.GetType() == AFT_DEFAULT)
			goto L_OK;

		if (nFilterType == AFF_ALL)
			goto L_OK;

		if (nFilterType == AFF_TEXTUREFILE && afItem.GetType() == AFT_IMG)
			goto L_OK;

		if (nFilterType == AFF_SOUNDFILE && afItem.GetType() == AFT_SOUND)
			goto L_OK;

		if (nFilterType == AFF_TEXTFILE && afItem.GetType() == AFT_TEXT)
			goto L_OK;

		if (nFilterType == AFF_CUSTOMFILE && isAFItemInCustom(afItem))
			goto L_OK;

		continue;

L_OK:
		AListCtrl::_FILEITEM _ft;
		_ft.csFileName = PathFindFileName(afItem.GetFullName());
		_ft.nSize = afItem.GetFileSize();
		_ft.pBitmapIcon = afItem.GetBitmap(isAlphaBlendChecked());
		_ft.uModifiedTime = afItem.GetTime();
		_ft.csFileType = 
			afItem.GetType() == AFT_IMG ? _AL("Picture") : 
			afItem.GetType() == AFT_SOUND ? _AL("Sound") : 
			afItem.GetType() == AFT_DIR ? _AL("Directory") : _AL("Others");
		aFI.push_back(_ft);	
	}

	m_pLstFiles->SetItems(aFI);
	m_pLstFiles->SortBy(FLSM_TYPE);
}


void AFileDialog::loadResources()
{
	m_bmDirectory.LoadBitmap(IDB_DIRECTORY);
	m_bmPicture.LoadBitmap(IDB_PICTURE);
	m_bmText.LoadBitmap(IDB_TEXT);
	m_bmSound.LoadBitmap(IDB_SOUND);
	m_bmOther.LoadBitmap(IDB_OTHER);
	_SetBitmapTransparent(&m_bmDirectory, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
	_SetBitmapTransparent(&m_bmPicture, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
	_SetBitmapTransparent(&m_bmSound, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
	_SetBitmapTransparent(&m_bmText, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
	_SetBitmapTransparent(&m_bmOther, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));

	m_bmArrowUp.LoadBitmap(IDB_ARROW_UP);
	m_bmArrowBack.LoadBitmap(IDB_ARROW_BACK);
}

size_t AFileDialog::getAFItemByName(const TCHAR * szName) const
{
	size_t i, nCount = m_aItems.size();
	for (i = 0; i < nCount; ++i)
	{
		if (StrCmp(PathFindFileName(m_aItems[i]->GetFullName()), szName) == 0)
			return i;
	}
	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// AFileDialog dialog


AFileDialog::AFileDialog(A3DDevice* pDevice
		, BOOL bIsOpenFile
		, LPCTSTR szInitRelativeDir
		, LPCTSTR szCaption
		, LPCTSTR szExt
		, int iListStyle /*= FLS_THUMBNAIL*/
		, CWnd* pParent /*=NULL*/)
	: base_class(AFileDialog::IDD, pParent)
	, m_pDevice(pDevice)
	, m_pLstFiles(new AListCtrl)
	, m_strInitRelativeDir(szInitRelativeDir)
	, m_strCaption(szCaption)
	, m_strExt(szExt)
	, m_bIsOpenFile(bIsOpenFile)
	, m_nErrorCode(AFDE_OK)
	, m_nListStyle(iListStyle)
	, m_nSortStyle(0)
	, m_pICLFile(new ICLFile)
	, m_pICLPool(new ICLThreadPool(1))
	, m_pSoundBufferMan(NULL)
	, m_pSound(new AFSoundPlayItem)
#ifdef _ANGELICA31
	, m_pPreviewWnd(/*new AFilePreviewWnd*/AFilePreviewWnd::GetInstance())
#else
	, m_pPreviewWnd(NULL)
#endif
{
	//{{AFX_DATA_INIT(AFileDialog)
	//}}AFX_DATA_INIT
	ASSERT(m_pDevice);
	m_pMsgNorifier = new AFileDialogMsgReciever(this);
	m_pLstCtrlListener = new AListCtrlListener(this);
	if (m_nListStyle < 0)
		m_nListStyle = m_afConfigs.GetLastListStyle();
	if (m_nSortStyle < 0)
		m_nSortStyle = m_afConfigs.GetLastListStyle();

	m_pSoundBufferMan = get_AM_sound_buffer_man(m_pDevice);
	m_pSoundEngine = get_AM_sound_engine(m_pDevice);

	//if (!m_strExt.IsEmpty())
	//{
	//	if (m_strExt.GetAt(0) == '.')
	//	{
	//		m_strExt = m_strExt.Right(m_strExt.GetLength() - 1);
	//	}
	//}

	updateCustomExtFiles(m_strExt);
}

AFileDialog::~AFileDialog()
{

	if (m_pSound) m_pSound->Release(m_pSoundBufferMan);
	delete m_pSound;

	clearPointerArray(m_aItems);
	delete m_pICLPool;
	delete m_pICLFile;
	delete m_pLstFiles;
	delete m_pMsgNorifier;
	delete m_pLstCtrlListener;

#ifdef _ANGELICA31

//	A3DRELEASE(m_pPreviewWnd);

#endif

#ifdef _USE_BCGP

	CBCGPVisualManager::DestroyInstance();
	
#endif
}

void AFileDialog::DoDataExchange(CDataExchange* pDX)
{
	base_class::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AFileDialog)
	DDX_Control(pDX, IDC_BT_UP, m_btUp);
	DDX_Control(pDX, IDC_BT_BACK, m_btBack);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_OpenFolder, m_OpenFolder);
	DDX_Control(pDX, IDC_STATIC_LOOKIN, m_txtLookIn);
	DDX_Control(pDX, IDC_STATIC_FILETYPE, m_txtFileType);

	DDX_Control(pDX, IDC_STATIC_FILENAME, m_txtFileName);
	DDX_Control(pDX, IDC_STR_FILENAME, m_edFileName);
	DDX_Control(pDX, IDC_CB_PATH_SHORTCUT, m_cbPathShortCut);
	DDX_Control(pDX, IDC_CB_FILETYPES, m_cbFileFilter);
	DDX_Control(pDX, IDC_IS_ALPHA_BLEND, m_cxAlphaBlend);
	DDX_Control(pDX, IDC_IS_PWINDOW_SHOW, m_cxPWndShow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AFileDialog, base_class)
	//{{AFX_MSG_MAP(AFileDialog)
	ON_CBN_SELCHANGE(IDC_CB_FILETYPES, OnSelchangeCbFiletypes)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_PAINT()
	
	ON_CBN_SELCHANGE(IDC_CB_PATH_SHORTCUT, OnSelchangeCbPathShortcut)
	ON_BN_CLICKED(IDC_BT_BACK, OnBtBack)
	ON_BN_CLICKED(IDC_BT_UP, OnBtUp)
	ON_MESSAGE(WM_FOLDER_PROC_COMPLETED, OnFolderProcCompleted)
	//}}AFX_MSG_MAP
#if defined( _MSC_VER ) && _MSC_VER < 1310
	ON_BN_CLICKED(IDC_IS_ALPHA_BLEND, AFileDialog::OnBnClickedIsAlphaBlend)
	ON_BN_CLICKED(IDC_IS_PWINDOW_SHOW, AFileDialog::OnBnClickedIsWindowShow)
#else
	ON_BN_CLICKED(IDC_IS_ALPHA_BLEND, &AFileDialog::OnBnClickedIsAlphaBlend)
	ON_BN_CLICKED(IDC_IS_PWINDOW_SHOW, &AFileDialog::OnBnClickedIsWindowShow)
#endif
	ON_WM_ENTERIDLE()
	ON_WM_MOVE()

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// AFileDialog message handlers

void AFileDialog::OnPaint()
{
	base_class::OnPaint();

	
}

LRESULT AFileDialog::OnFolderProcCompleted(WPARAM wParam, LPARAM lParam)
{
	const TCHAR * szFolder = (const TCHAR*)lParam;
	if (szFolder && _tcslen(szFolder) > 0)
		onUpdateFileList(szFolder);
	return 0;
}

INT_PTR AFileDialog::DoModal() 
{
	m_hHostResourceHandle = AfxGetResourceHandle();
	AfxSetResourceHandle(theApp.m_hInstance);

	// generate the Full-base-path, if failed, we do not create the dialog
	if (!formatInitDir(m_strInitRelativeDir)) {
		m_nErrorCode = AFDE_PATHNOTEXIST;
		return m_nErrorCode;
	}

#ifdef _ANGELICA31

	AFD_DllInitialize();

	if (!CRender::GetInstance()->IsInitialized())
	{
		m_nErrorCode = AFDE_DLLNOTINITIALIZED;
		return m_nErrorCode;
	}

#endif

	return static_cast<int>(base_class::DoModal());
}

BOOL AFileDialog::OnInitDialog() 
{
	base_class::OnInitDialog();

#ifdef _USE_BCGP

	EnableVisualManagerStyle();

	//	This is because that the BCGP Static will make the static control's bk ground to be transparent
	//	But there might be some reason which lead to the bk ground unpainted
	//	Simply make the bkground be valid.
	m_txtLookIn.m_bOnGlass = TRUE;
	m_txtFileType.m_bOnGlass = TRUE;
	m_txtFileName.m_bOnGlass = TRUE;
	
	//m_txtLookIn.Invalidate();
	//m_txtFileType.Invalidate();
	//m_txtFileName.Invalidate();
	
#endif

	loadResources();
	initFileFilter();

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
	rc.bottom = static_cast<LONG>(dlgRc.Height() * 0.78f);

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER;
	dwStyle |= !m_bIsOpenFile ? LVS_SINGLESEL : 0;
	m_pLstFiles->Create(dwStyle, rc, this, IDC_LST_FILES);
	m_pLstFiles->SetListStyle(m_nListStyle);
	m_pLstFiles->SetIconSize(96);
	m_pLstFiles->EnableMenu();
	m_pLstFiles->UpdateWindow();
	m_pLstFiles->ShowWindow(SW_SHOW);
	m_pLstFiles->AddListener(m_pLstCtrlListener);

	CString strOriCur = m_strCurDir;
	m_strCurDir.Empty();
	updateOnCurDirChanged(strOriCur);
	updatePathShortCut();

	m_cxAlphaBlend.SetCheck(m_afConfigs.GetLastCheckedAlphaBlendSwitch() ? BST_CHECKED : 0);
	m_cxPWndShow.SetCheck(0);

	if (/*m_strExt.IsEmpty()*/m_aCustomExtTypes.empty())
		m_cbFileFilter.SetCurSel(getSelIdxByFilterType(m_afConfigs.GetLastSelectedFilter()));
	else
		m_cbFileFilter.SetCurSel(getSelIdxByFilterType(AFF_CUSTOMFILE));

	if (!m_bIsOpenFile)
		m_btOK.SetWindowText(_T("&Save"));

	if (!m_lstImgShortCut.GetSafeHandle())
	{
		HBITMAP hBitmap = (HBITMAP)CopyImage(m_bmDirectory.m_hObject, IMAGE_BITMAP, 16, 16, LR_CREATEDIBSECTION);
		m_bmDirSmall.Attach(hBitmap);
		m_lstImgShortCut.Create(16, 16, 0, 0, 16);
		m_lstImgShortCut.Add(&m_bmDirSmall, RGB(255, 0, 255));
	}
	
	m_cbPathShortCut.SetImageList(&m_lstImgShortCut);

	m_dynSizeArray.push_back(DynSizeCtrl(m_pLstFiles, this, false, true, false, true, true, true, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_cbPathShortCut, this, false, true, false, true, false, false, true, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_cbFileFilter, this, true, false, false, true, true, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_edFileName, this, true, false, false, true, true, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_txtLookIn, this, false, true, false, true, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_txtFileName, this, true, false, false, true, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_txtFileType, this, true, false, false, true, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_btOK, this, true, false, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_OpenFolder, this, true, false, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_btCancel, this, true, false, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_btBack, this, false, true, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_btUp, this, false, true, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_cxAlphaBlend, this, false, true, true, false, false, false, false, false));
	m_dynSizeArray.push_back(DynSizeCtrl(&m_cxPWndShow, this, false, true, true, false, false, false, false, false));


	m_btUp.SetBitmap(m_bmArrowUp);
	m_btBack.SetBitmap(m_bmArrowBack);

	AfxSetResourceHandle(m_hHostResourceHandle);

	initWndSize(m_afConfigs.GetLastCX(), m_afConfigs.GetLastCY());


#ifdef _ANGELICA31

//	m_pA3DGfxEngine->Init(m_pDevice->GetA3DEngine(), m_pGfxInterface);
//	m_pGfxInterface->Init(m_pA3DGfxEngine);

	createPreviewWnd(CRender::GetInstance()->GetA3DDevice());

#endif

	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AFileDialog::OnSelchangeCbFiletypes() 
{
	updateListItems(getFilterTypeBySel(m_cbFileFilter));
	m_afConfigs.OnCurSelectedFilterChanged(getFilterTypeBySel(m_cbFileFilter));
}

void AFileDialog::OnSize(UINT nType, int cx, int cy) 
{
	base_class::OnSize(nType, cx, cy);
	
	for (DynSizeCtrlIterator itr = m_dynSizeArray.begin(); itr != m_dynSizeArray.end(); ++itr)
		itr->ResizeCtrl(cx, cy, m_lastwsize, m_origwsize);
	
	m_lastwsize.x = cx;
	m_lastwsize.y = cy;
	adjustPreviewWndPos();
}

void AFileDialog::OnSizing(UINT nSide, LPRECT lpRect)
{
	base_class::OnSizing(nSide, lpRect);

	switch(nSide)
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

void AFileDialog::OnFileLstClick(int nItem)
{
	m_pSound->StopSound();

	if (nItem == -1)
	{
		m_edFileName.SetWindowText(_T(""));
	}
	else
	{
		AFVector<CString> lstSel = m_pLstFiles->GetSelectedFiles();
		if (lstSel.size() == 0) 
		{

			m_edFileName.SetWindowText(_T(""));

#ifdef _ANGELICA31

			if (m_pPreviewWnd && isPWndShow())
				m_pPreviewWnd->SetPreviewFile(NULL);

#endif

		}
		else
		{
			size_t nIdx = getAFItemByName(lstSel.front());
			if (m_aItems[nIdx]->GetType() == AFT_DIR)
				m_edFileName.SetWindowText(_T(""));
			else {
				m_edFileName.SetWindowText(lstSel.front());
			}

			if (m_pSoundBufferMan) {

				if (is_support_sound(lstSel.front()) != AF_SUPPORT_SOUND_UNKNOWN) {
					AFTaskPlaySound task(m_pDevice, m_pSound, m_aItems[nIdx]->GetFullName());
					task.Do();
				}
			}

#ifdef _ANGELICA31

			if (m_pPreviewWnd && isPWndShow())
			{
				m_pPreviewWnd->SetPreviewFile(m_aItems[nIdx]->GetFullName());
				
				adjustPreviewWndPos();

				m_pLstFiles->SetFocus();
			}

#endif
			
			
		}

	}
}

void AFileDialog::OnFileLstDblClick(int nItem)
{
	if (nItem == -1)
	{
		m_edFileName.SetWindowText(_T(""));
		return;
	}

	AFVector<CString> lstSel = m_pLstFiles->GetSelectedFiles();
	if (lstSel.size() != 1)
		return;

	size_t nIdx = getAFItemByName(lstSel.front());
	if (m_aItems[nIdx]->GetType() == AFT_DIR)
	{
		CString strNewCur = m_aItems[nIdx]->GetFullName();
		if (strNewCur.Right(1) != _T("\\") && strNewCur.Right(1) != _T("/"))
			strNewCur += _T("\\");
		updateOnCurDirChanged(strNewCur);
		updatePathShortCut();
	}
	else {
		OnOK();
	}
}

bool AFileDialog::isDir(const TCHAR * szName) const
{
	size_t nFind = getAFItemByName(szName);
	if (nFind < 0 || nFind > m_aItems.size())
		return false;

	if (m_aItems[nFind]->GetType() == AFT_DIR)
		return true;

	return false;
}

AFVector<CString>::const_iterator AFileDialog::isContainDir(const AFVector<CString>& vec) const
{
	for (size_t nIdx = 0; nIdx < vec.size(); ++nIdx)
	{
		if (isDir(vec[nIdx]))
			return vec.begin() + nIdx;
	}
	return vec.end();
}

void AFileDialog::filterDir(AFVector<CString>& vec)
{
	for (size_t nIdx = 0; nIdx < vec.size();)
	{
		if (isDir(vec[nIdx]))
			vec.erase(vec.begin() + nIdx);
		else
			++nIdx;
	}
}

void AFileDialog::OnOK() 
{
	m_aSelFiles = m_pLstFiles->GetSelectedFiles();
	
	if (m_bIsOpenFile)
	{
		if (!m_aSelFiles.size())
			return;

		AFVector<CString>::const_iterator itr = isContainDir(m_aSelFiles);
		if (itr != m_aSelFiles.end()) {
			size_t nIdx = getAFItemByName(*itr);
			CString strNewCur = m_aItems[nIdx]->GetFullName();
			ASSERT(m_aItems[nIdx]->GetType() == AFT_DIR);
			if (strNewCur.Right(1) != _T("\\") && strNewCur.Right(1) != _T("/"))
				strNewCur += _T("\\");
			updateOnCurDirChanged(strNewCur);
			updatePathShortCut();
			return;
		}

		for (size_t nIdx = 0; nIdx < m_aSelFiles.size(); ++nIdx)
		{
			size_t nFindIdx = getAFItemByName(m_aSelFiles[nIdx]);
			if (nFindIdx < 0 || nFindIdx >= m_aItems.size())
				return;
			m_aSelFiles[nIdx] = m_aItems[nFindIdx]->GetFullName();
			m_strSelFileFullName = m_aSelFiles[nIdx];
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

		if (getAFItemByName(strInputName) != -1
			&& AfxMessageBox(_T("Are you sure to replace the existing file?"), MB_YESNO) != IDYES)
			return;
		else
			m_strSelFileFullName.Format(_T("%s%s"), (LPCTSTR)m_strCurDir, (LPCTSTR)strInputName);
	}

	base_class::OnOK();
}

void AFileDialog::OnSelchangeCbPathShortcut() 
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
	
	updateOnCurDirChanged(strFormatCurPath);
	updatePathShortCut();
}

void AFileDialog::OnBtBack() 
{
	if (!m_aDirHistory.GetSize())
		return;

	INT_PTR nIdx = m_aDirHistory.GetSize() - 1;
	CString strLastDir = m_aDirHistory.GetAt(nIdx);
	m_aDirHistory.RemoveAt(nIdx);
	updateOnCurDirChanged(strLastDir, false);
	updatePathShortCut();
}

void AFileDialog::OnBtUp() 
{
	if (m_strCurDir == m_strFullBaseDir)
		return;

	CStringArray aDirs;
	splitCurDir(m_strCurDir, m_strFullBaseDir, aDirs);
	if (!aDirs.GetSize())
		return;

	CString strNewCur(m_strFullBaseDir);
	for (int nIdx = 0; nIdx < aDirs.GetSize() - 1; ++nIdx)
	{
		strNewCur += aDirs.GetAt(nIdx);
		strNewCur += _T("\\");
	}

	updateOnCurDirChanged(strNewCur);
	updatePathShortCut();
}

void AFileDialog::OnBnClickedIsAlphaBlend()
{
	updateListItems(getFilterTypeBySel(m_cbFileFilter));
	m_afConfigs.OnCurAlphaBlendSwitchChanged(isAlphaBlendChecked());
}

bool AFileDialog::isAlphaBlendChecked() const
{
	return m_cxAlphaBlend.GetCheck() == BST_CHECKED;
}

//	String acquire is initialization
const TCHAR* AFileDialog::GetRelativeFileName()
{
	int iFind = m_strSelFileFullName.Find(m_strFullBaseDir);
	if (iFind < 0)
		return _T("");
	m_strRelativePath = m_strSelFileFullName.Right(m_strSelFileFullName.GetLength() - m_strFullBaseDir.GetLength());
	return m_strRelativePath;
}

BOOL AFileDialog::DestroyWindow()
{
	CRect rc;
	GetWindowRect(&rc);
	m_afConfigs.OnSize(rc.Width(), rc.Height());
	m_afConfigs.OnChangeListStyle(m_pLstFiles->GetListStyle());
	m_afConfigs.OnChangeSortStyle(m_pLstFiles->GetSortStyle());

#ifdef _ANGELICA31

	destroyPreviewWnd();

#endif
	
	return base_class::DestroyWindow();
}

void AFileDialog::initWndSize(int cx, int cy)
{
	CRect rc;
	GetWindowRect(&rc);
	rc.right += (cx - rc.Width());
	rc.bottom += (cy - rc.Height());
	MoveWindow(&rc);
}

int AFileDialog::createPreviewWnd(A3DDevice* pA3DDevice)
{
#ifdef _ANGELICA31

	AFilePreviewWnd::GetInstance()->SetParent(NULL);
	adjustPreviewWndPos();
	
#endif

	return 0;
}

void AFileDialog::destroyPreviewWnd()
{
#ifdef _ANGELICA31

	AFilePreviewWnd::GetInstance()->SetPreviewFile(NULL);
	AFilePreviewWnd::GetInstance()->SetParent(NULL);
	AFilePreviewWnd::GetInstance()->ShowWindow(SW_HIDE);
	AFilePreviewWnd::GetInstance()->SetWShow(false);

#endif
}

void AFileDialog::adjustPreviewWndPos()
{
#ifdef _ANGELICA31

	CRect rcPreviewWnd;
	AFilePreviewWnd::GetInstance()->GetWindowRect(&rcPreviewWnd);

	CRect rcFileDlg;
	GetWindowRect(&rcFileDlg);
	rcPreviewWnd.MoveToXY(rcFileDlg.right, rcFileDlg.top);
	AFilePreviewWnd::GetInstance()->MoveWindow(&rcPreviewWnd, TRUE);
	AFilePreviewWnd::GetInstance()->SetWindowPos(CWnd::FromHandle(HWND_TOPMOST), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

#endif

}
void AFileDialog::OnMove(int x, int y)
{
	base_class::OnMove(x, y);

	adjustPreviewWndPos();
}

void AFileDialog::OnBnClickedIsWindowShow()
{
#ifdef _ANGELICA31

	// TODO: Add your control notification handler code here
	if(!isPWndShow())
	{
		AFilePreviewWnd::GetInstance()->ShowWindow(SW_HIDE);
		AFilePreviewWnd::GetInstance()->SetWShow(false);
	}

#endif

}

bool AFileDialog::isPWndShow() const
{
	return m_cxPWndShow.GetCheck() == BST_CHECKED;
}

#endif