// ICLFile.cpp: implementation of the ICLFile class.
//
//////////////////////////////////////////////////////////////////////

#include "ICLStdAfx.h"
#include "ICLFile.h"
#include <AF.h>

#ifdef _ANGELICA31
	typedef ADiskFile MyFile; 
#else
	typedef AFile MyFile;
#endif

bool readOneItem(AFile * pFile, ICLFile::ICLItem * pItem)
{
	ASSERT(pFile && pItem);
	DWORD dwReadLen;

	if (!pFile->ReadString(pItem->strFileName))
		return false;

	if (!pFile->Read(&pItem->ftTime, sizeof(pItem->ftTime), &dwReadLen))
		return false;

	if (!pFile->Read(&pItem->dwFileSize, sizeof(pItem->dwFileSize), &dwReadLen))
		return false;

	if (!pFile->Read(&pItem->dwDataLen, sizeof(pItem->dwDataLen), &dwReadLen))
		return false;

	if (!pFile->Read(&pItem->dwHeight, sizeof(pItem->dwHeight), &dwReadLen))
		return false;

	if (!pFile->Read(&pItem->dwWidth, sizeof(pItem->dwWidth), &dwReadLen))
		return false;

	pItem->pBuf = new unsigned char[pItem->dwDataLen];
	if (!pFile->Read(pItem->pBuf, pItem->dwDataLen, &dwReadLen))
		return false;

	return true;
}

bool writeOneItem(AFile * pFile, ICLFile::ICLItem * pItem)
{
	ASSERT(pFile && pItem);
	DWORD dwWritten;
	if (!pFile->WriteString(pItem->strFileName))
		return false;

	if (!pFile->Write(&pItem->ftTime, sizeof(pItem->ftTime), &dwWritten))
		return false;

	if (!pFile->Write(&pItem->dwFileSize, sizeof(pItem->dwFileSize), &dwWritten))
		return false;

	if (!pFile->Write(&pItem->dwDataLen, sizeof(pItem->dwDataLen), &dwWritten))
		return false;

	if (!pFile->Write(&pItem->dwHeight, sizeof(pItem->dwHeight), &dwWritten))
		return false;

	if (!pFile->Write(&pItem->dwWidth, sizeof(pItem->dwWidth), &dwWritten))
		return false;

	if (!pFile->Write(pItem->pBuf, pItem->dwDataLen, &dwWritten))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ICLFile::ICLFile()
{

}

ICLFile::~ICLFile()
{
	releaseItems();
}

bool ICLFile::Open(const TCHAR* szFolder)
{
	releaseItems();

	MyFile File;
	ACString strFullPath(szFolder);
	if (strFullPath.Right(1) != _T("\\"))
		strFullPath += _T("\\");

	strFullPath += ICL_CACHE_FILE_NAME;
	m_strFullPath = strFullPath;
	if (!File.Open(AC2AS(strFullPath), AFILE_BINARY | AFILE_OPENEXIST))
		return false;

	loadItems(&File);
	File.Close();
	return true;
}

bool ICLFile::Save()
{
	DWORD dwWritten;
	MyFile File;
	if (!File.Open(AC2AS(m_strFullPath), AFILE_BINARY | AFILE_CREATENEW))
	{
		a_LogOutput(1, "Failed to open file %s to write", AC2AS(m_strFullPath));
		return false;
	}

	ICLHeader header(m_aICLItemArray.size());
	if (!File.Write(&header, sizeof(header), &dwWritten))
		return false;

	for (ICLItemIterator itr = m_aICLItemArray.begin()
		; itr != m_aICLItemArray.end()
		; ++itr)
	{
		if (!writeOneItem(&File, *itr))
			return false;
	}

	File.Close();
	return true;
}

void ICLFile::releaseItems()
{
	for (ICLItemIterator itr = m_aICLItemArray.begin()
		; itr != m_aICLItemArray.end()
		; ++itr)
	{
		delete *itr;
	}
	m_aICLItemArray.clear();
}

bool ICLFile::loadItems(AFile * pFile)
{
	DWORD dwReadLen;
	ICLHeader header;
	if (!pFile->Read(&header, sizeof(header), &dwReadLen))
		return false;

	if (header.dwHeadFlag != ICL_CACHE_FILE_FLAG)
		return false;

	if (header.dwVersion != ICL_CACHE_FILE_VERSION)
		return false;

	ASSERT(header.dwItemCount >= 0);

	size_t nIdx, nCount = header.dwItemCount;
	for (nIdx = 0; nIdx < nCount; ++nIdx)
	{
		ICLItem* pItem = new ICLItem;
		if (!readOneItem(pFile, pItem)) {
			delete pItem;
			return false;
		}
		else
			m_aICLItemArray.push_back(pItem);
	}
	return true;
}

ICLFile::ICLItem* ICLFile::GetICLItemByName(const TCHAR * szFile)
{
	ICLItemIterator itr = findICLItem(AC2AS(szFile));
	if (itr == m_aICLItemArray.end())
		return NULL;
	return *itr;
}

ICLFile::ICLItem* ICLFile::GetICLItemByIdx(size_t nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < m_aICLItemArray.size());
	return m_aICLItemArray[nIdx];
}

bool ICLFile::AddICLItem(const char * szFilename, FILETIME ftFiletime, DWORD dwDataLen, DWORD dwFileSize, DWORD dwHeight, DWORD dwWidth, unsigned char* pBuf)
{
	ASSERT(szFilename);
	ICLItem* pItem = new ICLItem;
	pItem->strFileName = szFilename;
	pItem->ftTime = ftFiletime;
	pItem->dwDataLen = dwDataLen;
	pItem->dwFileSize = dwFileSize;
	pItem->dwHeight = dwHeight;
	pItem->dwWidth = dwWidth;
	pItem->pBuf = new unsigned char[dwDataLen];
	memcpy(pItem->pBuf, pBuf, dwDataLen);
	m_aICLItemArray.push_back(pItem);
	return true;
}

bool ICLFile::RemoveICLItem(const char * szFile)
{
	ASSERT(szFile);
	ICLItemIterator itr = findICLItem(szFile);
	if (itr == m_aICLItemArray.end())
		return false;
	delete *itr;
	m_aICLItemArray.erase(itr);
	return true;
}

ICLFile::ICLItemIterator ICLFile::findICLItem(const char * szFile)
{
	for (ICLItemIterator itr = m_aICLItemArray.begin()
		; itr != m_aICLItemArray.end()
		; ++itr)
	{
		if ((*itr)->strFileName.CompareNoCase(szFile) == 0)
			return itr;
	}
	return m_aICLItemArray.end();
}