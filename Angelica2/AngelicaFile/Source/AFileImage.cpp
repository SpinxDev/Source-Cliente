 /*
 * FILE: AFileImage.cpp
 *
 * DESCRIPTION: The class which operating the file images in memory for Angelica Engine
 *
 * CREATED BY: Hedi, 2001/12/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include <sys/stat.h>

#include "AFPI.h"
#include "AFileImage.h"
#include "AFilePackage.h"
#include "AFilePackMan.h"
#include "AFI.h"

#define new A_DEBUG_NEW

LPFNEXTDECRYPT AFileImage::m_lpfnExtDecrypt = NULL;

//	Write data to package
bool AFileImage::WriteToPack(const char* szFile, const void* pBuf, DWORD dwBufLen, bool bReplaceOnly)
{
	char szRelFile[MAX_PATH];
	af_GetRelativePath(szFile, szRelFile);

	AFilePackage* pPackage = (AFilePackage*)g_AFilePackMan.GetFilePck(szRelFile);
	if (pPackage)
	{
		AFilePackage::FILEENTRY Entry;
		int	nIndex;
		if (!pPackage->GetFileEntry(szRelFile, &Entry, &nIndex))
		{
			if (bReplaceOnly)
				return false;

			return pPackage->AppendFile(szRelFile, (BYTE*)pBuf, dwBufLen, true);
		}
		else
			return pPackage->ReplaceFile(szRelFile, (BYTE*)pBuf, dwBufLen, true);
	}
	else	//	Write to disk
	{
		AFile af;
		if (!af.Open("", szFile, AFILE_CREATENEW | AFILE_NOHEAD | AFILE_BINARY))
			return false;

		DWORD dwWrite;
		if (!af.Write((void*)pBuf, dwBufLen, &dwWrite))
			return false;

		af.Close();
	}

	return true;
}

void AFileImage::SetExtDecryptCallback(LPFNEXTDECRYPT lpfn)
{
	m_lpfnExtDecrypt = lpfn;
}

LPFNEXTDECRYPT AFileImage::GetExtDecryptCallback()
{
	return m_lpfnExtDecrypt;
}

AFileImage::AFileImage() : AFile()
{
	m_pPackage		= NULL;
	m_pFileImage	= NULL;
	m_nCurPtr		= 0;
	m_nFileLength	= 0;
	m_dwHandle		= 0;
	m_bTempMem		= false;
}

AFileImage::~AFileImage()
{
	Close();
}

bool AFileImage::Init(const char* szFullName, bool bTempMem)
{
	m_bTempMem = bTempMem;

	strncpy(m_szFileName, szFullName, MAX_PATH);
	af_GetRelativePath(szFullName, m_szRelativeName);
	
	m_pPackage = g_AFilePackMan.GetFilePck(m_szRelativeName);
	if( m_pPackage )
	{
		// using package file first
		DWORD dwFileLen;

		//	Init from a package
		if (!(m_dwHandle = m_pPackage->OpenSharedFile(m_szRelativeName, &m_pFileImage, &dwFileLen, bTempMem)))
		{
			// can't find it in package, so see if can load from disk.
			m_pPackage = NULL;
			goto SEPFILE;
		}

		m_nFileLength = (int)dwFileLen;
		m_dwTimeStamp = 0;

		//	Note: NOT to do ext decrypt for package file. DoExtDecrypt() may re-allocate the buffer
		//		pointted by m_pFileImage, and this will may cause problem when call AFilePackage::CloseSharedFile(),
		//		because the buffer recoreded in m_dwHandle has been freed by DoExtDecrypt().
	/*	if (!DoExtDecrypt())
		{
			AFERRLOG(("AFileImage::Init, Failed to call DoExtDecrypt() for package file !"));
			a_free(m_pFileImage);
			return false;
		}
	*/
		return true;
	}
	
SEPFILE:
	// no package for this file, so try to load from disk.
	FILE* pFile;
	if (!(pFile = fopen(szFullName, "rb")))
	{
#ifndef _DEBUG
		if( !strstr(szFullName, "Textures") && !strstr(szFullName, "Tex_") )
#endif
			AFERRLOG(("AFileImage::Can not open file [%s] from disk!", szFullName));
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	if (!(m_nFileLength = ftell(pFile)))
	{
		AFERRLOG(("AFileImage::Init The file [%s] is zero length!", szFullName));
		return false;
	}

	fseek(pFile, 0, SEEK_SET);

	if (m_bTempMem)
	{
		if (!(m_pFileImage = (BYTE*)a_malloctemp(m_nFileLength)))
		{
			AFERRLOG(("AFileImage::Init Not enough memory!"));
			return false;
		}
	}
	else
	{
		if (!(m_pFileImage = (BYTE*)a_malloc(m_nFileLength)))
		{
			AFERRLOG(("AFileImage::Init Not enough memory!"));
			return false;
		}
	}

	fread(m_pFileImage, m_nFileLength, 1, pFile);

	//	Try to do external decryption
	if (!DoExtDecrypt())
	{
		AFERRLOG(("AFileImage::Init, Failed to call DoExtDecrypt() for disk file !"));
		if (m_bTempMem)
			a_freetemp(m_pFileImage);
		else
			a_free(m_pFileImage);

		return false;
	}

	int idFile = _fileno(pFile);
	struct _stat fileStat;
	_fstat(idFile, &fileStat);
	m_dwTimeStamp = fileStat.st_mtime;

	fclose(pFile);

	return true;
}

bool AFileImage::Release()
{
	if (!m_pPackage)
	{
		if (m_pFileImage)
		{
			if (m_bTempMem)
				a_freetemp(m_pFileImage);
			else
				a_free(m_pFileImage);
			
			m_pFileImage = NULL;
		}
	}
	else
	{
		if (m_dwHandle && m_pFileImage)
		{
			m_pPackage->CloseSharedFile(m_dwHandle);
			m_pFileImage = NULL;
			m_dwHandle	 = 0;
		}
	}

	return true;
}

bool AFileImage::DoExtDecrypt()
{
	if (!m_lpfnExtDecrypt)
		return true;
	
	const void* pSrcData = m_pFileImage;
	DWORD dwSrcSize = m_nFileLength;

	//	Get data size after decryption
	DWORD dwRealSize = 0;
	DWORD dwRet = m_lpfnExtDecrypt(pSrcData, dwSrcSize, NULL, &dwRealSize);
	if (dwRet == 1)
	{
		//	Needn't to do decryption
		return true;
	}
	else if (dwRet == 0)
	{
		//	To do decryption
		void* pDstBuf = NULL;
		if (m_bTempMem)
			pDstBuf = a_malloctemp(dwRealSize);
		else
			pDstBuf = a_malloc(dwRealSize);

		if (!pDstBuf)
		{
			AFERRLOG(("AFileImage::Init Not enough memory!"));
			return false;
		}

		dwRet = m_lpfnExtDecrypt(pSrcData, dwSrcSize, pDstBuf, &dwRealSize);
		if (dwRet != 0)
		{
			//	Free old buffer and set new one
			if (m_bTempMem)
				a_freetemp(pDstBuf);
			else
				a_free(pDstBuf);
			
			AFERRLOG(("AFileImage::DoExtDecrypt, call m_lpfnExtDecrypt failed ! [ret=%d]", dwRet));
			return false;
		}

		//	Free old buffer and set new one
		if (m_bTempMem)
			a_freetemp(m_pFileImage);
		else
			a_free(m_pFileImage);

		m_pFileImage = (BYTE*)pDstBuf;
		//	Reset file length
		m_nFileLength = (int)dwRealSize;

		return true;
	}
	else
	{
		AFERRLOG(("AFileImage::DoExtDecrypt, call m_lpfnExtDecrypt failed ! [ret=%d]", dwRet));
		return false;
	}
}

bool AFileImage::Open(const char* szFolderName, const char * szFileName, DWORD dwFlags)
{
	char szFullPath[MAX_PATH];
	af_GetFullPath(szFullPath, szFolderName, szFileName);
	return Open(szFullPath, dwFlags);
}

bool AFileImage::Open(const char* szFullPath, DWORD dwFlags)
{
	// If we have opened it already, we must close it;
	if( m_bHasOpened )
		Close();
	
	bool bTempMem = (dwFlags & AFILE_TEMPMEMORY) ? true : false;

	if( !Init(szFullPath, bTempMem) )
	{
		return false;
	}

	if( dwFlags & AFILE_OPENEXIST )
	{
	}
	else
	{
		AFERRLOG(("AFileImage::Open Current we only support read flag to operate a file image"));
		return false;
	}

	DWORD dwFOURCC;
	int   nRead;

	m_dwFlags = dwFlags;
	m_dwFlags = dwFlags & (~(AFILE_BINARY | AFILE_TEXT));
	if( !fimg_read((LPBYTE)&dwFOURCC, 4, &nRead) )
		return false;

	if( dwFOURCC == 0x42584f4d )
		m_dwFlags |= AFILE_BINARY;
	else if( dwFOURCC == 0x54584f4d )
		m_dwFlags |= AFILE_TEXT;
	else
	{
		//Default we use text mode, for we can edit it by hand, and we will not add 
		//the shitting FOURCC at the beginning of the file
		m_dwFlags |= AFILE_TEXT;
		fimg_seek(0, AFILE_SEEK_SET);
	}
	
	m_bHasOpened = true;
	return true;
}

bool AFileImage::Close()
{
	Release();

	m_nCurPtr = 0;
	m_szFileName[0] = '\0';
	return true;
}

bool AFileImage::ResetPointer()
{
	fimg_seek(0, AFILE_SEEK_SET);
	return true;
}

bool AFileImage::fimg_read(LPBYTE pBuffer, int nSize, int * pReadSize)
{
	int nSizeToRead = nSize;

	if( m_nCurPtr + nSizeToRead > m_nFileLength )
		nSizeToRead = m_nFileLength - m_nCurPtr;
	
	if( nSizeToRead <= 0 )
	{
		*pReadSize = 0;
		return nSize == 0 ? true : false;
	}

	memcpy(pBuffer, m_pFileImage + m_nCurPtr, nSizeToRead);
	m_nCurPtr += nSizeToRead;
	*pReadSize = nSizeToRead;
	return true;
}

bool AFileImage::fimg_read_line(char * szLineBuffer, int nMaxLength, int * pReadSize)
{
	int nSizeRead = 0;

	ZeroMemory(szLineBuffer, nMaxLength);
	while( m_nCurPtr < m_nFileLength )
	{
		BYTE byteThis = m_pFileImage[m_nCurPtr];

		if( byteThis != 0x0d && byteThis != 0x0a )
		{
			// Not \n or \r, so copy it into the buffer;
			szLineBuffer[nSizeRead ++] = m_pFileImage[m_nCurPtr ++];
		}
		else
		{
			// We also need to copy \n into the buffer;
			szLineBuffer[nSizeRead ++] = m_pFileImage[m_nCurPtr ++];
			szLineBuffer[nSizeRead] = '\0';
			if( byteThis == 0x0d )
			{
				// We need to check if next byte is \r, if so, just remove it;
				if( m_nCurPtr < m_nFileLength )
				{
					if( m_pFileImage[m_nCurPtr] == 0x0a )
					{
						m_nCurPtr ++;
						nSizeRead ++;
					}
				}
			}
			
			break;
		}
	}

	*pReadSize = nSizeRead;

	if( nSizeRead <= 0 )
		return false;
	return true;
}

bool AFileImage::fimg_seek(int nOffset, int startPos)
{
	switch(startPos)
	{
	case AFILE_SEEK_SET:
		m_nCurPtr = nOffset;
		break;
	case AFILE_SEEK_CUR:
		m_nCurPtr += nOffset;
		break;
	case AFILE_SEEK_END:
		m_nCurPtr = m_nFileLength + nOffset;
		break;
	default:
		return false;
	}
	if( m_nCurPtr < 0 )
		m_nCurPtr = 0;
	else if( m_nCurPtr > m_nFileLength ) // To be compatible with fseek, we have to let the file pointer beyond the last character;
		m_nCurPtr = m_nFileLength;
	return true;
}

bool AFileImage::ReadLine(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength)
{
	int nReadSize;

	if( !fimg_read_line(szLineBuffer, dwBufferLength, &nReadSize) )
		return false;

	//chop the \n\r
	if( szLineBuffer[0] && (szLineBuffer[strlen(szLineBuffer) - 1] == '\n' || szLineBuffer[strlen(szLineBuffer) - 1] == '\r') )
		szLineBuffer[strlen(szLineBuffer) - 1] = '\0';

	if( szLineBuffer[0] && (szLineBuffer[strlen(szLineBuffer) - 1] == '\n' || szLineBuffer[strlen(szLineBuffer) - 1] == '\r') )
		szLineBuffer[strlen(szLineBuffer) - 1] = '\0';

	*pdwReadLength = strlen(szLineBuffer) + 1;
	return true;
}

bool AFileImage::Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength)
{
	int nReadSize;
	if( !fimg_read((LPBYTE)pBuffer, dwBufferLength, &nReadSize) )
		return false;

	*pReadLength = nReadSize;
	return true;
}

bool AFileImage::Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength)
{
	return false;
}

DWORD AFileImage::GetPos()
{
	return (DWORD) m_nCurPtr;
}

bool AFileImage::Seek(int iOffset, AFILE_SEEK origin)
{
	return fimg_seek(iOffset, (int)origin);
}

bool AFileImage::WriteLine(const char* szLineBuffer)
{
	return false;
}

bool AFileImage::ReadString(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength)
{
	char ch;
	DWORD dwReadLength;
	DWORD nStrLen = 0;

	Read(&ch, 1, &dwReadLength);
	while( ch )
	{
		szLineBuffer[nStrLen] = ch;
		nStrLen ++;

		if( nStrLen >= dwBufferLength )
			return false;

		Read(&ch, 1, &dwReadLength);
	}

	szLineBuffer[nStrLen] = '\0';

	*pdwReadLength = nStrLen + 1;
	return true;
}

bool AFileImage::WriteString(const AString& str)
{
	return false;
}

bool AFileImage::ReadString(AString& str)
{
	//	Only binary file is supported
/*	if (m_dwFlags & AFILE_TEXT)
	{
		ASSERT(!(m_dwFlags & AFILE_TEXT));
		return false;
	}
*/
	int iRead;

	//	Read length of string
	int iLen;
	if( !fimg_read((BYTE*) &iLen, sizeof (int), &iRead) )
		return false;

	//	Read string data
	if (iLen)
	{
		char* szBuf = (char*)a_malloctemp(iLen+1);
		if (!szBuf)
			return false;

		if( !fimg_read((BYTE*) szBuf, iLen, &iRead) )
		{
			a_freetemp(szBuf);
			return false;
		}
		szBuf[iLen] = '\0';
		str = szBuf;

		a_freetemp(szBuf);
	}
	else
		str = "";

	return true;
}

