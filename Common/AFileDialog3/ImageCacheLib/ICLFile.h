// ICLFile.h: interface for the ICLFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICLFILE_H__98D1E6A1_B871_48C4_8B7C_BB7F7E4B5D51__INCLUDED_)
#define AFX_ICLFILE_H__98D1E6A1_B871_48C4_8B7C_BB7F7E4B5D51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ICL_CACHE_FILE_NAME _T("acidb.db")

#define ICL_CACHE_FILE_FLAG 0xacacacac

/*
 * Version 2: Change cache file's save format from JPG to PNG, in order to save the alpha channel
 */

#define ICL_CACHE_FILE_VERSION 3

class ICLFile
{
public:
	struct ICLHeader
	{
		ICLHeader(DWORD itemCount = 0)
			: dwItemCount(itemCount)
			, dwHeadFlag(ICL_CACHE_FILE_FLAG)
			, dwVersion(ICL_CACHE_FILE_VERSION)
		{

		}
		DWORD dwHeadFlag;
		DWORD dwVersion;
		DWORD dwItemCount;
	};

	struct ICLItem
	{
		ICLItem()
			: dwDataLen(0)
			, pBuf(NULL)
		{
			ftTime.dwHighDateTime = 0;
			ftTime.dwLowDateTime = 0;
		}
		~ICLItem()
		{
			delete[] pBuf;
		}
		AString strFileName;
		FILETIME ftTime;
		DWORD dwDataLen;
		DWORD dwFileSize;
		DWORD dwHeight;
		DWORD dwWidth;
		unsigned char* pBuf;
	};

	typedef abase::vector<ICLItem*> ICLItemArray;
	typedef ICLItemArray::iterator ICLItemIterator;

public:
	ICLFile();
	virtual ~ICLFile();

public:

	bool Open(const TCHAR* szFolder);
	bool Save();
	size_t GetICLItemCount() const { return m_aICLItemArray.size(); }
	ICLItem* GetICLItemByName(const TCHAR * szFile);
	ICLItem* GetICLItemByIdx(size_t nIdx);
	// ICLFile would not hold the pBuf, but just copy from it
	bool AddICLItem(const char * szFilename, FILETIME ftFiletime, DWORD dwDataLen, DWORD dwFileSize, DWORD dwHeight, DWORD dwWidth, unsigned char* pBuf);
	bool RemoveICLItem(const char * szFile);

private:
	ACString m_strFullPath;
	ICLItemArray m_aICLItemArray;

private:

	void releaseItems();
	bool loadItems(AFile * pFile);
	ICLItemIterator findICLItem(const char * szFile);
};

#endif // !defined(AFX_ICLFILE_H__98D1E6A1_B871_48C4_8B7C_BB7F7E4B5D51__INCLUDED_)
