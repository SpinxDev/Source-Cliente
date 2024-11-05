// ICLTaskProcFolder.cpp: implementation of the ICLTaskProcFolder class.
//
//////////////////////////////////////////////////////////////////////

#include "ICLStdAfx.h"

#ifdef _ANGELICA31
#include "A3DGfxEx.h"
#include "ec_model.h"
#endif

class A3DDevice;
class A3DGfxEngine;

bool isSupportImgFile(const TCHAR * szFilename)
{
	ACString strFileName(szFilename);
	strFileName.MakeLower();
	if (AFCheckFileExt(strFileName, _T(".jpg")))
		return true;

#ifdef _ANGELICA31
	if (AFCheckFileExt(strFileName, _T(".ecm3")))
		return true;
	if (AFCheckFileExt(strFileName, _T(".gfx3")))
		return true;
	if (AFCheckFileExt(strFileName, _T(".SMD")))
		return true;
	if (AFCheckFileExt(strFileName, _T(".umd")))
		return true;
	if (AFCheckFileExt(strFileName, _T(".umds")))
		return true;

#elif defined _ANGELICA22 || defined _ANGELICA21

	if (AFCheckFileExt(strFileName, _T(".gfx")))
		return true;

	if (AFCheckFileExt(strFileName, _T(".ecm")))
		return true;

	if (AFCheckFileExt(strFileName, _T(".SMD")))
		return true;

#elif defined _ANGELICA2
	if (AFCheckFileExt(strFileName, _T(".gfx")))
		return true;

	if (AFCheckFileExt(strFileName, _T(".ecm")))
		return true;
#endif

	if (AFCheckFileExt(strFileName, _T(".tga")))
		return true;
	if (AFCheckFileExt(strFileName, _T(".dds")))
		return true;
	if (AFCheckFileExt(strFileName, _T(".png")))
		return true;
	if (AFCheckFileExt(strFileName, _T(".bmp")))
		return true;
	return false;
}

//zjy#ifdef _ANGELICA2
//zjyextern ICLImg* CreateImg(const char * szFilename, A3DDevice* pDevice);
//zjy#elif _ANGELICA31
extern ICLImg* CreateImg(const char * szFilename, A3DDevice* pDevice, A3DGfxEngine* pDGfxEngine, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pDAdditionalView*/);
//zjy#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*zjy
#ifdef _ANGELICA2
ICLTaskProcFolder::ICLTaskProcFolder(A3DDevice* pDevice, const TCHAR * szFolder, ICLNotifier* pMsgReceiver)
: m_pDevice(pDevice) 
, m_strTaskFolder(szFolder)
, m_pMsgReceiver(pMsgReceiver)
{
	//	ASSERT(pDevice && pMsgReceiver);
	if (m_strTaskFolder.Right(1) != "\\")
		m_strTaskFolder += "\\";
}

#elif _ANGELICA31
 */
ICLTaskProcFolder::ICLTaskProcFolder(const TCHAR * szFolder, ICLNotifier* pMsgReceiver, A3DDevice* pDevice, A3DGfxEngine* pDGfxEngine, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pDAdditionalView*/)
: m_strTaskFolder(szFolder)
, m_pMsgReceiver(pMsgReceiver)
, m_pDevice(pDevice)
, m_pDGfxEngine(pDGfxEngine)
, m_pDCamera(pDCamera)
, m_pDViewport(pDViewport)
, m_pRenderTarget(pRenderTarget)
//, m_pDAdditionalView(pDAdditionalView)
{
//	ASSERT(pDevice && pMsgReceiver);
	if (m_strTaskFolder.Right(1) != "\\")
		m_strTaskFolder += "\\";
}
//zjy#endif

ICLTaskProcFolder::~ICLTaskProcFolder()
{

}


bool ICLTaskProcFolder::Do()
{
	// check cache file
	ICLFile iclFile;
	iclFile.Open(m_strTaskFolder);

	// check files under folder
	typedef std::map<CString, FILEINFO> ImgFileMap;
	ImgFileMap mapImgFiles;
	CFileFind finder;
	CString strBase(m_strTaskFolder + "*.*");
	BOOL bWorking = finder.FindFile(strBase);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
			continue;

		CString strFind(finder.GetFileName());
		if (!isSupportImgFile(strFind))
			continue;

		CTime ct;
		finder.GetLastWriteTime(ct);
		mapImgFiles[strFind].fTime = ct;
		mapImgFiles[strFind].dwSize = finder.GetLength();
	}

	// get modified list
	for (size_t nIdx = 0; nIdx < iclFile.GetICLItemCount();)
	{
		ICLFile::ICLItem* pItem = iclFile.GetICLItemByIdx(nIdx);
		CTime itemTime(pItem->ftTime);

		ImgFileMap::iterator itr = mapImgFiles.find((LPCSTR)pItem->strFileName);
		//CTime& newTime = itr->second.fTime;
		//a_LogOutput(1, "Time in db: %d %d %d %d %d %d", itemTime.GetYear(), itemTime.GetMonth(), itemTime.GetDay(), itemTime.GetHour(), itemTime.GetMinute(), itemTime.GetSecond());
		//a_LogOutput(1, "Time of file: %d %d %d %d %d %d", newTime.GetYear(), newTime.GetMonth(), newTime.GetDay(), newTime.GetHour(), newTime.GetMinute(), newTime.GetSecond());
		if (itr == mapImgFiles.end()) {			// cached file no longer exists
			iclFile.RemoveICLItem(pItem->strFileName);
			continue;
		}
		else if (itr->second.fTime == itemTime)		// cached file is newer than or equal to the file
			mapImgFiles.erase(itr);

		++nIdx;
	}
	
	// now each item in the mapImgFiles is need to be updated one
	// process each modified file
	std::map<CString, FILEINFO>::iterator itr;
	for (itr = mapImgFiles.begin()
		; itr != mapImgFiles.end()
		; ++itr)
	{
		procOneFile(&iclFile, itr->first, itr->second);
	}

	if (mapImgFiles.size())
		iclFile.Save();
	m_pMsgReceiver->FolderProcComplete(m_strTaskFolder);
	return true;
}

void ICLTaskProcFolder::procOneFile(ICLFile* pICLFile, const TCHAR * szFilename, const FILEINFO& fi)
{
	ICLFile::ICLItem* pItem = pICLFile->GetICLItemByName(szFilename);
	CString strFullFilename;
	strFullFilename.Format(_T("%s%s"), (LPCTSTR)m_strTaskFolder, szFilename);

//zjy#ifdef _ANGELICA2
//zjy	std::auto_ptr<ICLImg> pImg(CreateImg(AC2AS(szFilename), m_pDevice));
//zjy#elif _ANGELICA31
	std::auto_ptr<ICLImg> pImg(CreateImg(AC2AS(szFilename), m_pDevice, m_pDGfxEngine, m_pDCamera, m_pDViewport, m_pRenderTarget/*m_pDAdditionalView*/));
//zjy#endif

	if (!pImg.get())
		return;

	if (pItem != NULL) {
		// Update Cache Image

		if (!pImg->ReadFromFile(strFullFilename))
			return;

		if (!pImg->Scale())
			return;

		unsigned char * pBuf = NULL;
		long nSize = 0;
		if (!pImg->Encode(pBuf, nSize))
			return;

		delete [] pItem->pBuf;
		pItem->pBuf = new unsigned char[nSize];
		memcpy(pItem->pBuf, pBuf, sizeof(unsigned char) * nSize);
		pImg->FreeMemory(pBuf);

		pItem->dwDataLen = nSize;
		pItem->dwFileSize = fi.dwSize;
		pItem->dwHeight = pImg->GetImageHeight();
		pItem->dwWidth = pImg->GetImageWidth();
		SYSTEMTIME sysTime;
		FILETIME ftLocalTime, ftTime;
		fi.fTime.GetAsSystemTime(sysTime);
		SystemTimeToFileTime(&sysTime, &ftLocalTime);
		LocalFileTimeToFileTime(&ftLocalTime, &ftTime);
		pItem->ftTime = ftTime;
	}
	else
	{
		// New Image

		if (!pImg->ReadFromFile(strFullFilename))
			return;

		if (!pImg->Scale())
			return;
		
		unsigned char * pBuf = NULL;
		long nSize = 0;
		if (!pImg->Encode(pBuf, nSize))
			return;

		SYSTEMTIME sysTime;
		FILETIME ftLocalTime, ftTime;
		DWORD dwHeight = pImg->GetImageHeight();
		DWORD dwWidth = pImg->GetImageWidth();
		fi.fTime.GetAsSystemTime(sysTime);
		SystemTimeToFileTime(&sysTime, &ftLocalTime);
		LocalFileTimeToFileTime(&ftLocalTime, &ftTime);
		pICLFile->AddICLItem(AC2AS(szFilename), ftTime, nSize, fi.dwSize, dwHeight, dwWidth, pBuf);
		pImg->FreeMemory(pBuf);
	}
}