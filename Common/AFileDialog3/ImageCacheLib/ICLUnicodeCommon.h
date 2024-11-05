#ifndef _ICL_UNICODE_COMMON_H_
#define _ICL_UNICODE_COMMON_H_

#if defined(UNICODE) || defined(_UNICODE)

#define AFCheckFileExt CheckFileExtW

#else

#define AFCheckFileExt af_CheckFileExt

#endif

// the wide char version for af_CheckFileExt
bool CheckFileExtW(const wchar_t* szFileName, const wchar_t* szExt, int iExtLen=-1, int iFileNameLen=-1);


#endif