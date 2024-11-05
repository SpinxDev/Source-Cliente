/*
 * FILE: AMiniDump.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "AMiniDump.h"
#include "AChar.h"
#include "ABaseDef.h"
#include <tchar.h>

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AMiniDump
//	
///////////////////////////////////////////////////////////////////////////

//	Only public method
bool AMiniDump::Create(HMODULE hModule, PEXCEPTION_POINTERS pExceptionInfo, const TCHAR* szDumpFile,
					   LPCRITICAL_SECTION pCS)
{
	CSmartHandle hImpersonationToken = NULL;
	if (!GetImpersonationToken(&hImpersonationToken.m_hHandle))
		return false;

	//	Load DBGHELP.dll
	CSmartModule hDbgDll = LoadDbgHelpDLL(hModule);
	if (!hDbgDll.m_hModule)
		return false;

	//	Get a pointer to MiniDumpWriteDump
	MINIDUMPWRITEDUMP pDumpFunction = (MINIDUMPWRITEDUMP)::GetProcAddress(hDbgDll.m_hModule, "MiniDumpWriteDump");
	if (!pDumpFunction)
		return false;

	// Create the dump file
	CSmartHandle hDumpFile = ::CreateFile(szDumpFile, GENERIC_READ | GENERIC_WRITE, 
									FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if (hDumpFile.m_hHandle == INVALID_HANDLE_VALUE)
		return false;

	//	Write the dump
	MINIDUMP_EXCEPTION_INFORMATION stInfo = {0};

	stInfo.ThreadId = GetCurrentThreadId();
	stInfo.ExceptionPointers = pExceptionInfo;
	stInfo.ClientPointers = TRUE;

	//	We need the SeDebugPrivilege to be able to run MiniDumpWriteDump
	TOKEN_PRIVILEGES tp;
	bool bPrivilegeEnabled = EnablePriv(SE_DEBUG_NAME, hImpersonationToken.m_hHandle, &tp);

	//	deghelp.DLL is not thread safe
	if (pCS)
		EnterCriticalSection(pCS);

	BOOL bRet = pDumpFunction(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile.m_hHandle, MiniDumpWithDataSegs, &stInfo, NULL, NULL);

	if (pCS)
		LeaveCriticalSection(pCS);

	if (bPrivilegeEnabled)
	{
		// Restore the privilege
		RestorePriv(hImpersonationToken.m_hHandle, &tp);
	}

	return bRet ? true : false;
}

bool AMiniDump::GetImpersonationToken(HANDLE* phToken)
{
	*phToken = NULL;

	if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, TRUE, phToken))
	{
		if (GetLastError() == ERROR_NO_TOKEN)
		{
			//	No impersonation token for the curren thread available - go for the process token
			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, phToken))
				return false;
		}
		else
			return false;
	}

	return true;
}

bool AMiniDump::EnablePriv(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld)
{
	BOOL bOk = FALSE;

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	bOk = LookupPrivilegeValue( 0, pszPriv, &tp.Privileges[0].Luid);
	if(bOk)
	{
		DWORD cbOld = sizeof(*ptpOld);
		bOk = AdjustTokenPrivileges(hToken, FALSE, &tp, cbOld, ptpOld, &cbOld);
	}

	return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}

bool AMiniDump::RestorePriv(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld)
{
	BOOL bOk = AdjustTokenPrivileges(hToken, FALSE, ptpOld, 0, 0, 0);	
	return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}

HMODULE AMiniDump::LoadDbgHelpDLL(HMODULE hModule)
{
	HMODULE hDll = NULL;

	//	Attempt to load local module first
	TCHAR szModulePath[MAX_PATH];
	if (GetModuleFileName(hModule, szModulePath, MAX_PATH))
	{
		TCHAR* pSlash = a_strrchr(szModulePath, _AL('\\'));
		if (pSlash)
		{
			a_strcpy(pSlash+1, _AL("dbghelp.dll"));
			hDll = ::LoadLibrary(szModulePath);
		}
	}

	if (!hDll)
	{
		//	If not found, load any available copy
		hDll = ::LoadLibrary(_AL("dbghelp.dll"));
	}

	return hDll;
}


