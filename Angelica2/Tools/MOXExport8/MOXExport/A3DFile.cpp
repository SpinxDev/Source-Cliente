#include "A3DFile.h"

A3DFile::A3DFile()
{
	m_szFileName[0] = '\0';
	m_pFile = NULL;
	m_dwFlags = 0;
}

A3DFile::~A3DFile()
{
}

bool A3DFile::Open(char * szFileName, DWORD dwFlags)
{
	strncpy(m_szFileName, szFileName, MAX_PATH);
	char szOpenFlag[32];

	szOpenFlag[0] = '\0';
	if( dwFlags & A3DFILE_OPENEXIST )
		strcat(szOpenFlag, "r");
	if( dwFlags & A3DFILE_CREATENEW )
		strcat(szOpenFlag, "w");
	if( dwFlags & A3DFILE_OPENAPPEND )
		strcat(szOpenFlag, "a");

	//If there is no binary or text flag, the default is binary;
	if( dwFlags & A3DFILE_TEXT )
		strcat(szOpenFlag, "t");
	else
		strcat(szOpenFlag, "b");
	m_pFile = fopen(m_szFileName, szOpenFlag);
	if( NULL == m_pFile )
		return false;

	DWORD dwFOURCC;

	if( dwFlags & A3DFILE_CREATENEW )
	{
		m_dwFlags = dwFlags;
		if( m_dwFlags & A3DFILE_TEXT )
		{
			dwFOURCC = 0x54584f4d;
			fwrite(&dwFOURCC, 4, 1, m_pFile);
		}
		else
		{
			dwFOURCC = 0x42584f4d;
			fwrite(&dwFOURCC, 4, 1, m_pFile);
		}
	}
	else
	{
		m_dwFlags = dwFlags & (~(A3DFILE_BINARY | A3DFILE_TEXT));

		fread(&dwFOURCC, 4, 1, m_pFile);
		if( dwFOURCC == 0x42584f4d )
			m_dwFlags |= A3DFILE_BINARY;
		else if( dwFOURCC == 0x54584f4d )
			m_dwFlags |= A3DFILE_TEXT;
		else
			return false;
	}
	
	return true;
}

bool A3DFile::Close()
{
	if( m_pFile )
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	return true;
}

bool A3DFile::Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength)
{
	*pReadLength = fread(pBuffer, 1, dwBufferLength, m_pFile);
	return true;
}

bool A3DFile::Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength)
{
	*pWriteLength = fwrite(pBuffer, 1, dwBufferLength, m_pFile);
	return true;
}

bool A3DFile::ReadLine(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength)
{
	if( !fgets(szLineBuffer, dwBufferLength, m_pFile) )
		return false;

	//chop the \n\r
	if( szLineBuffer[strlen(szLineBuffer) - 1] == '\n' || szLineBuffer[strlen(szLineBuffer) - 1] == '\r' )
		szLineBuffer[strlen(szLineBuffer) - 1] = '\0';

	if( szLineBuffer[strlen(szLineBuffer) - 1] == '\n' || szLineBuffer[strlen(szLineBuffer) - 1] == '\r' )
		szLineBuffer[strlen(szLineBuffer) - 1] = '\0';

	*pdwReadLength = strlen(szLineBuffer) + 1;
	return true;
}

bool A3DFile::ReadString(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength)
{
	char ch;
	DWORD nStrLen = 0;

	fread(&ch, 1, 1, m_pFile);
	while( ch )
	{
		szLineBuffer[nStrLen] = ch;
		nStrLen ++;

		if( nStrLen >= dwBufferLength )
			return false;

		fread(&ch, 1, 1, m_pFile);
	}

	szLineBuffer[nStrLen] = '\0';

	*pdwReadLength = nStrLen + 1;
	return true;
}

bool A3DFile::WriteLine(char * szLineBuffer)
{
	if( fprintf(m_pFile, "%s\n", szLineBuffer) < 0 )
		return false;
	return true;
}

DWORD A3DFile::GetPos()
{
	DWORD dwPos;

	dwPos = (DWORD) ftell(m_pFile);

	return dwPos;
}

bool A3DFile::Seek(DWORD dwBytes)
{
	if( 0 != fseek(m_pFile, dwBytes, SEEK_SET) )
		return false;

	return true;
}

bool A3DFile::GetStringAfter(char * szBuffer, char * szTag, char * szResult)
{
	char * pch;

	szResult[0] = '\0';
	pch = strstr(szBuffer, szTag);
	if( pch != szBuffer )
		return false;

	pch += strlen(szTag);
	strcpy(szResult, pch);
	return true;
}