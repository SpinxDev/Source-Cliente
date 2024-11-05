#include <Windows.h>
#include <StdIO.h>
#include <StdLib.h>

#ifndef _A3DFILE_H_
#define _A3DFILE_H_

#define A3DFILE_OPENEXIST				0x00000001
#define A3DFILE_CREATENEW				0x00000002
#define A3DFILE_OPENAPPEND				0x00000004
#define A3DFILE_TEXT					0x00000008
#define A3DFILE_BINARY					0x00000010

#define A3DFILE_LINEMAXLEN				2048

class A3DFile
{
private:
	char	m_szFileName[MAX_PATH];
	FILE *	m_pFile;
	DWORD	m_dwFlags;

public:
	A3DFile();
	~A3DFile();
	bool Open(char * szFileName, DWORD dwFlags);
	bool Close();
	bool Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength);
	bool Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength);
	bool ReadLine(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength);
	bool ReadString(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength);
	bool WriteLine(char * szLineBuffer);
	bool GetStringAfter(char * szBuffer, char * szTag, char * szResult);
	DWORD GetPos();
	bool Seek(DWORD dwBytes);

	inline DWORD GetFlags() { return m_dwFlags; }
	//Binary first, so if there is no binary or text, it is a binary file;
	inline bool IsBinary() { return !IsText(); }
	inline bool IsText() { return (m_dwFlags & A3DFILE_TEXT) ? true : false; }
};

typedef A3DFile * PA3DFile;
#endif