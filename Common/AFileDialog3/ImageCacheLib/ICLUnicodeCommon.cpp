#include "ICLStdAfx.h"

bool CheckFileExtW(const wchar_t* szFileName, const wchar_t* szExt, int iExtLen, int iFileNameLen)
{
	AString str = WC2AS(szFileName);
	AString ext = WC2AS(szExt);
	return af_CheckFileExt(str, ext, iExtLen, iFileNameLen);
}