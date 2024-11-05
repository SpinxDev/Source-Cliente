#ifdef _DEBUG

#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <Windows.h>
#include "AMemory.h"
#include "dbghelp.h"
#include "hashmap.h"
#include "tchar.h"
#include <SHLWAPI.H>

void a_GetSmallMemInfo(int iSlot, int* piBlkSize, int* piBlkCnt, int* piFreeCnt);
int GetMemSlotNum();

static HANDLE _LoadSymForCurrentProcess( const char* UserSearchPath )
{
    static HANDLE hProcess;
    static const  int bufsz = 256;
    static char   szLastPath[bufsz];
	
    if( !hProcess || (UserSearchPath && strncmp(szLastPath, UserSearchPath, bufsz-1)) )
    {
        if (UserSearchPath)
        {
            strncpy( szLastPath, UserSearchPath, bufsz-1 );
        }
        else
            szLastPath[0] = 0;
        szLastPath[bufsz-1] = 0;
		
        hProcess = GetCurrentProcess();
        DWORD flags = SymSetOptions(SYMOPT_LOAD_LINES);
        bool bSuccess = SymInitialize(hProcess, (char*)UserSearchPath, TRUE) ? true : false;
        //assert(bSuccess);
    }
	
    return hProcess;
}

struct SymbloInfo
{
    const void* pAddr;
    CHAR		Name[128];
    CHAR		FileName[128];
    DWORD       LineNumber;
};

typedef abase::hash_map<void*, SymbloInfo> SymbloInfoMap;
static SymbloInfoMap _addr_info_map;
#define MAX_SYM_NAME 2000
static SymbloInfo _AddressToSymbol(void* pAddr, HANDLE hProcess )
{
    SymbloInfo si;
    memset(&si, 0, sizeof(si));
    si.pAddr = pAddr;
	
    DWORD  error;
    DWORD  dwAddress = (DWORD)pAddr;
    ULONG64 buffer[(sizeof(SYMBOL_INFO) +
        MAX_SYM_NAME * sizeof(TCHAR) +
        sizeof(ULONG64) - 1) / 
        sizeof(ULONG64)];
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO) buffer;
	
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;
    DWORD64 dw64Offset;
    if (SymFromAddr(hProcess, dwAddress, &dw64Offset, pSymbol))
    {
        _tcsncpy(si.Name, pSymbol->Name, sizeof(si.Name));
    }
    else
    {
        error = GetLastError();
    }
	
    IMAGEHLP_LINE line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
    DWORD dwOffset = (DWORD)dw64Offset;
    if (SymGetLineFromAddr(hProcess, dwAddress, &dwOffset, &line))
    {
        strncpy(si.FileName, line.FileName, sizeof(si.FileName));
        si.LineNumber = line.LineNumber;
    }
    else
    {
        error = GetLastError();
    }
	
    return si;
}

static char g_UserSearchPath[1024];
HANDLE _a_InitializeSym()
{
	static HANDLE hProcess = NULL;
	if (hProcess == NULL)
	{
		char szFileName[MAX_PATH];
		GetModuleFileNameA(NULL, szFileName, sizeof(szFileName));
		if (LPSTR pStr = PathFindFileNameA(szFileName))
			*pStr = NULL;
		
		char szUserSearchPath[1024] = {0};
		if (g_UserSearchPath[0])
		{
			sprintf(szUserSearchPath, "%s;%s", szFileName, g_UserSearchPath);
		}
		else
			strcpy(szUserSearchPath, szFileName);
		
		hProcess = _LoadSymForCurrentProcess(szFileName);
	}
	return hProcess;
}

void _a_RegisterAddressSymbol(void* pAddress)
{
	if (pAddress == NULL)
		return;

	static HANDLE hProcess = _a_InitializeSym();
	if (_addr_info_map.find(pAddress) == _addr_info_map.end())
	{
		_addr_info_map[pAddress] = _AddressToSymbol(pAddress, hProcess);
	}
}

void ExportSymbolInfo(void * ptr)
{
	SymbloInfo si;
	si = _AddressToSymbol(ptr, _a_InitializeSym());

	char szMsg[1024];
	sprintf(szMsg, " %s (%d): %s -- 0x%p\n", si.FileName, si.LineNumber, si.Name, si.pAddr);
	OutputDebugStringA(szMsg);
}

void DumpAdditionalInfo(FILE* pAddiInfoFile)
{
	//	Additional information
	int iTotalFreeBytes = 0;
	for (int i = 0; i < GetMemSlotNum(); ++i)
	{
		int iBlkSize, iBlkCnt, iFreeCnt;
		a_GetSmallMemInfo(i, &iBlkSize, &iBlkCnt, &iFreeCnt);
		fprintf (pAddiInfoFile, "%d slot has %d free bytes\n", iBlkSize, iBlkSize * iFreeCnt);
		iTotalFreeBytes += iBlkSize * iFreeCnt;
	}
	fprintf(pAddiInfoFile, "Totally %d free bytes\n", iTotalFreeBytes);
	
	fprintf (pAddiInfoFile, "----- ----- ----- -----\n");
	
	for (SymbloInfoMap::iterator itr = _addr_info_map.begin()
		; itr != _addr_info_map.end()
		; ++itr)
	{
		fprintf(pAddiInfoFile, "0x%p %s\n(%d): %s\n", itr->first, itr->second.FileName, itr->second.LineNumber, itr->second.Name);
	}
}

void _a_SetPdbSearchPath(const char* szUserSearchPath)
{
	memset(g_UserSearchPath, 0, sizeof(g_UserSearchPath));
	strncpy(g_UserSearchPath, szUserSearchPath, sizeof(g_UserSearchPath));
}

static FILE * g_pLogFile;
void _a_EnableMemoryHistoryLog(FILE * pMemHisFile)
{
	g_pLogFile = pMemHisFile;
}

FILE * GetMemoryHistoryLog()
{
	return g_pLogFile;
}

#endif
