// hook.cpp : Defines the entry point for the DLL application.
//

#include "windows.h"
#include "mhook.h"
#define _T(X) L ## X



// Shared Data

#define DetourFunctionWithTrampoline Mhook_SetHook
#define DetourRemove Mhook_Unhook

void InstallHook(BOOL bActive);
void TrampolineUnHook();




/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    hInstance = (HINSTANCE) hModule;
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			char buf[512]={0,};
			GetModuleFileName(NULL,buf,512);
			_strlwr(buf);
			if(strstr(buf,"gfxeditor.exe")!=0)
			{
				InstallHook(TRUE);
			}
		}
		break;
	case DLL_PROCESS_DETACH:
		InstallHook(FALSE);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:

		break;
	}
    return TRUE;
}
*/
static const char *pathbuf =NULL;
//char *oldpath ="D:\\GfxEditor_en";

static const WCHAR *wpathbuf =NULL;
//WCHAR *woldpath =_T("D:\\GfxEditor_en");
static BOOL g_flag = FALSE;
BOOL GetInstallFlag()
{
	return g_flag;
}
void SetPathA(const char *buf)
{
	pathbuf = buf;
}
void SetPathW(const WCHAR *buf)
{
	wpathbuf = buf;
}
void InstallHook(BOOL bActive)
{
	if(bActive)
	{
		if(InitHookProc())
		{
			//SetPathA(oldpath);
			TrampolineHook();
		}
	}
	else
	{
		TrampolineUnHook();
	}
}


static LONG g_handlecount =0;
#define HOOKREG 0X56567878

static TP_RegQueryValueExA Real_RegQueryValueExA;
static TP_RegOpenKeyExA    Real_RegOpenKeyExA;
static TP_RegCloseKey      Real_RegCloseKey;


static LONG __stdcall GXXX_RegOpenKeyExA
(
 HKEY hKey,         // handle to open key
 LPCSTR lpSubKey,  // subkey name
 DWORD ulOptions,   // reserved
 REGSAM samDesired, // security access mask
 PHKEY phkResult    // handle to open key	  
 )
{
	LONG rv = 0;
	rv = Real_RegOpenKeyExA(hKey,lpSubKey,ulOptions,samDesired,phkResult);
	if((rv ==ERROR_SUCCESS)&&((DWORD)hKey ==0x80000002)&&(strcmp(lpSubKey,"Software\\Ageia Technologies")==0))
	{
		g_flag = TRUE;
	}
	if((rv !=ERROR_SUCCESS)&&((DWORD)hKey ==0x80000002)&&(strcmp(lpSubKey,"Software\\Ageia Technologies")==0))
	{
		rv = ERROR_SUCCESS;
		*phkResult = (HKEY)HOOKREG;
	}

	return rv;
}

static LONG __stdcall GXXX_RegQueryValueExA
				  (
				  HKEY a0,     // handle to key
				  LPCSTR a1,  // value name
				  LPDWORD a2,  // reserved
				  LPDWORD a3,  // type buffer
				  LPBYTE a4,   // data buffer
				  LPDWORD a5   // size of data buffer
				  )
{
	LONG rv =0;
	if(a1!=NULL)
	{
		if(strcmp(a1,"PhysXCore Path")==0)
		{
			if(pathbuf!=NULL)
			{
				
				if((*a5>strlen(pathbuf)+1))
				{
					rv = ERROR_SUCCESS;
					strcpy((char *)a4,pathbuf);
					*a3 = REG_SZ;
					*a5 = strlen(pathbuf)+1;
				}
				else
				{
					rv = ERROR_MORE_DATA;
					*a3 = REG_SZ;
					*a5 = strlen(pathbuf)+1;
				}
			}
			else
			{
				rv = Real_RegQueryValueExA(a0,a1,a2,a3,a4,a5);
			}
		}
		else
		{
			if(a0 != (HKEY)HOOKREG)
			{
				rv = Real_RegQueryValueExA(a0,a1,a2,a3,a4,a5);
			}
			else
			{
				rv = ERROR_SUCCESS;
			}
		}
	}
    return rv;
}

static LONG __stdcall GXXX_RegCloseKey(HKEY hKey)
{
	LONG rv =0;
	if(hKey ==(HKEY) HOOKREG)
	{
		rv = ERROR_SUCCESS;
		g_handlecount--;
	}
	else
	{
		rv = Real_RegCloseKey(hKey);
	}
	return rv;
}


static BOOL InitHookProc()
{
	static WORD first;
	if(first >0)
		return TRUE;
	HMODULE h_advapi32 = GetModuleHandle(TEXT("ADVAPI32.dll"));
	if(!h_advapi32)
		h_advapi32 = LoadLibrary(TEXT("ADVAPI32.dll"));
	if(h_advapi32)
	{
		Real_RegQueryValueExA =  (TP_RegQueryValueExA) GetProcAddress(h_advapi32,"RegQueryValueExA");	
		Real_RegOpenKeyExA =  (TP_RegOpenKeyExA) GetProcAddress(h_advapi32,"RegOpenKeyExA");
		Real_RegCloseKey =  (TP_RegCloseKey) GetProcAddress(h_advapi32,"RegCloseKey");
		first++;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static BOOL g_bhooked = FALSE;
static VOID TrampolineHook(VOID)
{
	InitHookProc();
	if(!g_bhooked)
	{
		//DetourFunctionWithTrampoline((PVOID*)&Real_EndPaint,(PVOID *)GXXX_EndPaint);
		DetourFunctionWithTrampoline((PVOID*)&Real_RegQueryValueExA,(PVOID *)GXXX_RegQueryValueExA);
		DetourFunctionWithTrampoline((PVOID*)&Real_RegOpenKeyExA,(PVOID *)GXXX_RegOpenKeyExA);
		DetourFunctionWithTrampoline((PVOID*)&Real_RegCloseKey,(PVOID *)GXXX_RegCloseKey);
		g_bhooked = TRUE;
	}
}
static VOID TrampolineUnHook(VOID)
{
	if(g_bhooked)
	{
		//DetourRemove((PVOID*)&Real_BeginPaint);
		DetourRemove((PVOID*)&Real_RegQueryValueExA);
		DetourRemove((PVOID*)&Real_RegOpenKeyExA);
		DetourRemove((PVOID*)&Real_RegCloseKey);
		g_bhooked = FALSE;
	}
}


//Copyright (c) 2007, Marton Anka
//
//Permission is hereby granted, free of charge, to any person obtaining a 
//copy of this software and associated documentation files (the "Software"), 
//to deal in the Software without restriction, including without limitation 
//the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the 
//Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included 
//in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
//OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
//THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
//FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
//IN THE SOFTWARE.

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include "mhook.h"


//=========================================================================
#ifndef cntof
#define cntof(a) (sizeof(a)/sizeof(a[0]))
#endif

//=========================================================================
#ifndef GOOD_HANDLE
#define GOOD_HANDLE(a) ((a!=INVALID_HANDLE_VALUE)&&(a!=NULL))
#endif

//=========================================================================
#ifndef gle
#define gle GetLastError
#endif

//=========================================================================
#ifndef ODPRINTF

#ifdef _DEBUG
#define ODPRINTF(a) //odprintf a
#else
#define ODPRINTF(a)
#endif


#endif //#ifndef ODPRINTF
/*
//=========================================================================
// The trampoline structure - stores every bit of info about a hook
typedef struct _tagMHOOKS_TRAMPOLINE {
	PBYTE	pSystemFunction;			// the original system function
	DWORD	cbOverwrittenCode;			// number of bytes overwritten by the jump
	PBYTE	pHookFunction;				// the hook function that we provide
	BYTE	codeJumpToHookFunction[32];	// placeholder for code that jumps to the hook function
	BYTE	codeTrampoline[32];			// placeholder for code that holds the first few
										//   bytes from the system function and a jump to the remainder
										//   in the original location
} MHOOKS_TRAMPOLINE;
*/
//=========================================================================
// Global vars
static BOOL g_bVarsInitialized = FALSE;
static CRITICAL_SECTION g_cs;
static MHOOKS_TRAMPOLINE* g_pHooks[MHOOKS_MAX_SUPPORTED_HOOKS];
static DWORD g_nHooksInUse = 0;
static HANDLE* g_hThreadHandles = NULL;
static DWORD g_nThreadHandles = 0;
#define MHOOK_JMPSIZE 5

//=========================================================================
// Toolhelp defintions so the functions can be dynamically bound to
typedef HANDLE (WINAPI * _CreateToolhelp32Snapshot)(
	DWORD dwFlags,       
	DWORD th32ProcessID  
	);

typedef BOOL (WINAPI * _Thread32First)(
									   HANDLE hSnapshot,     
									   LPTHREADENTRY32 lpte
									   );

typedef BOOL (WINAPI * _Thread32Next)(
									  HANDLE hSnapshot,     
									  LPTHREADENTRY32 lpte
									  );

//=========================================================================
// Bring in the toolhelp functions from kernel32
static _CreateToolhelp32Snapshot fnCreateToolhelp32Snapshot = (_CreateToolhelp32Snapshot) GetProcAddress(GetModuleHandleW(L"kernel32"), "CreateToolhelp32Snapshot");
static _Thread32First fnThread32First = (_Thread32First) GetProcAddress(GetModuleHandleW(L"kernel32"), "Thread32First");
static _Thread32Next fnThread32Next = (_Thread32Next) GetProcAddress(GetModuleHandleW(L"kernel32"), "Thread32Next");

//=========================================================================
static VOID EnterCritSec() {
	if (!g_bVarsInitialized) {
		InitializeCriticalSection(&g_cs);
		ZeroMemory(g_pHooks, sizeof(g_pHooks));
		g_bVarsInitialized = TRUE;
	}
	EnterCriticalSection(&g_cs);
}

//=========================================================================
static VOID LeaveCritSec() {
	LeaveCriticalSection(&g_cs);
}

//=========================================================================
// static INTERNAL function:
// 
// Skip over jumps that lead to the real function. Gets around import
// jump tables, etc.
//=========================================================================
static PBYTE SkipJumps(PBYTE pbCode) {
#ifdef _M_IX86_X64
	if (pbCode[0] == 0xff && pbCode[1] == 0x25) {
#ifdef _M_IX86
		// on x86 we have an absolute pointer...
		PBYTE pbTarget = *(PBYTE *)&pbCode[2];
		// ... that shows us an absolute pointer.
		return SkipJumps(*(PBYTE *)pbTarget);
#elif defined _M_X64
		// on x64 we have a 32-bit offset...
		INT32 lOffset = *(INT32 *)&pbCode[2];
		// ... that shows us an absolute pointer
		return SkipJumps(*(PBYTE*)(pbCode + 6 + lOffset));
#endif
	} else if (pbCode[0] == 0xe9) {
		// here the behavior is identical, we have...
		// ...a 32-bit offset to the destination.
		return SkipJumps(pbCode + 5 + *(INT32 *)&pbCode[1]);
	} else if (pbCode[0] == 0xeb) {
		// and finally an 8-bit offset to the destination
		return SkipJumps(pbCode + 2 + *(CHAR *)&pbCode[1]);
	}
#else
#error unsupported platform
#endif
	return pbCode;
}

//=========================================================================
// static INTERNAL function:
//
// Writes code at pbCode that jumps to pbJumpTo. Will attempt to do this
// in as few bytes as possible. Important on x64 where the long jump
// (0xff 0x25 ....) can take up 14 bytes.
//=========================================================================
static PBYTE EmitJump(PBYTE pbCode, PBYTE pbJumpTo) {
#ifdef _M_IX86_X64
	PBYTE pbJumpFrom = pbCode + 5;
	SIZE_T cbDiff = pbJumpFrom > pbJumpTo ? pbJumpFrom - pbJumpTo : pbJumpTo - pbJumpFrom;
	ODPRINTF((L"mhooks: EmitJump: Jumping from %p to %p, diff is %p", pbJumpFrom, pbJumpTo, cbDiff));
	if (cbDiff <= 0x7fff0000) {
		pbCode[0] = 0xe9;
		pbCode += 1;
		*((PDWORD)pbCode) = (DWORD)(DWORD_PTR)(pbJumpTo - pbJumpFrom);
		pbCode += sizeof(DWORD);
	} else {
		pbCode[0] = 0xff;
		pbCode[1] = 0x25;
		pbCode += 2;
#ifdef _M_IX86
		// on x86 we write an absolute address (just behind the instruction)
		*((PDWORD)pbCode) = (DWORD)(DWORD_PTR)(pbCode + sizeof(DWORD));
#elif defined _M_X64
		// on x64 we write the relative address of the same location
		*((PDWORD)pbCode) = (DWORD)0;
#endif
		pbCode += sizeof(DWORD);
		*((PDWORD_PTR)pbCode) = (DWORD_PTR)(pbJumpTo);
		pbCode += sizeof(DWORD_PTR);
	}
#else 
#error unsupported platform
#endif
	return pbCode;
}

//=========================================================================
// static INTERNAL function:
//
// Will try to allocate the trampoline structure within 2 gigabytes of
// the target function. 
//=========================================================================
static MHOOKS_TRAMPOLINE* TrampolineAlloc(PBYTE pSystemFunction) {

	MHOOKS_TRAMPOLINE* pTrampoline = NULL;

	// do we have room to store this guy?
	if (g_nHooksInUse < MHOOKS_MAX_SUPPORTED_HOOKS) {

		// determine lower and upper bounds for the allocation locations
		PBYTE pLower = pSystemFunction < (PBYTE)(DWORD_PTR)0x0000000080000000 ? 
							(PBYTE)(0x1) : (PBYTE)(pSystemFunction - (PBYTE)0x7fff0000);
		PBYTE pUpper = pSystemFunction < (PBYTE)(DWORD_PTR)0xffffffff80000000 ? 
			(PBYTE)(pSystemFunction + (DWORD_PTR)0x7ff80000) : (PBYTE)(DWORD_PTR)0xfffffffffff80000;
		ODPRINTF((L"mhooks: TrampolineAlloc: Allocating for %p between %p and %p", pSystemFunction, pLower, pUpper));

		SYSTEM_INFO sSysInfo =  {0};
		::GetSystemInfo(&sSysInfo);

		// go through the available memory blocks and try to allocate a chunk for us
		for (PBYTE pbAlloc = pLower; pbAlloc < pUpper;) {
			// determine current state
			MEMORY_BASIC_INFORMATION mbi;
			ODPRINTF((L"mhooks: TrampolineAlloc: Looking at address %p", pbAlloc));
			if (!VirtualQuery(pbAlloc, &mbi, sizeof(mbi)))
				break;
			// free & large enough?
			if (mbi.State == MEM_FREE && mbi.RegionSize >= sizeof(MHOOKS_TRAMPOLINE) && mbi.RegionSize >= sSysInfo.dwAllocationGranularity) {
				// yes, align the pointer to the 64K boundary first
				pbAlloc = (PBYTE)(ULONG_PTR((ULONG_PTR(pbAlloc) + (sSysInfo.dwAllocationGranularity-1)) / sSysInfo.dwAllocationGranularity) * sSysInfo.dwAllocationGranularity);
				// and then try to allocate it
				pTrampoline = (MHOOKS_TRAMPOLINE*)VirtualAlloc(pbAlloc, sizeof(MHOOKS_TRAMPOLINE), MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READ);
				if (pTrampoline) {
					ODPRINTF((L"mhooks: TrampolineAlloc: Allocated block at %p as the trampoline", pTrampoline));
					break;
				}
			}
			// continue the search
			pbAlloc = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
		}

		// found one
		if (pTrampoline) {
			// put it into our list so we know we'll have to free it
			for (DWORD i=0; i<MHOOKS_MAX_SUPPORTED_HOOKS; i++) {
				if (g_pHooks[i] == NULL) {
					g_pHooks[i] = pTrampoline;
					g_nHooksInUse++;
					break;
				}
			}
		}
	}

	return pTrampoline;
}

//=========================================================================
// static INTERNAL function:
//
// Return the static INTERNAL trampoline structure that belongs to a hooked function.
//=========================================================================
static MHOOKS_TRAMPOLINE* TrampolineGet(PBYTE pHookedFunction) {
	for (DWORD i=0; i<MHOOKS_MAX_SUPPORTED_HOOKS; i++) {
		if (g_pHooks[i]) {
			if (g_pHooks[i]->codeTrampoline == pHookedFunction)
				return g_pHooks[i];
		}
	}
	return NULL;
}

//=========================================================================
// static INTERNAL function:
//
// Free a trampoline structure.
//=========================================================================
static VOID TrampolineFree(MHOOKS_TRAMPOLINE* pTrampoline, BOOL bNeverUsed) {
	for (DWORD i=0; i<MHOOKS_MAX_SUPPORTED_HOOKS; i++) {
		if (g_pHooks[i] == pTrampoline) {
			g_pHooks[i] = NULL;
			// It might be OK to call VirtualFree, but most likely it isn't: 
			// If a thread has some of our trampoline code on its stack
			// and we yank the region from underneath it then it will
			// surely crash upon returning. So instead of freeing the 
			// memory we just let it leak. Ugly...
			if (bNeverUsed)
				VirtualFree(pTrampoline, 0, MEM_RELEASE);
			g_nHooksInUse--;
			break;
		}
	}
}

//=========================================================================
// static INTERNAL function:
//
// Suspend a given thread and try to make sure that its instruction
// pointer is not in the given range.
//=========================================================================
static HANDLE SuspendOneThread(DWORD dwThreadId, PBYTE pbCode, DWORD cbBytes) {
	// open the thread
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);
	if (GOOD_HANDLE(hThread)) {
		// attempt suspension
		DWORD dwSuspendCount = SuspendThread(hThread);
		if (dwSuspendCount != -1) {
			// see where the IP is
			CONTEXT ctx;
			ctx.ContextFlags = CONTEXT_CONTROL;
			int nTries = 0;
			while (GetThreadContext(hThread, &ctx)) {
#ifdef _M_IX86
				PBYTE pIp = (PBYTE)(DWORD_PTR)ctx.Eip;
#elif defined _M_X64
				PBYTE pIp = (PBYTE)(DWORD_PTR)ctx.Rip;
#endif
				if (pIp >= pbCode && pIp < (pbCode + cbBytes)) {
					if (nTries < 3) {
						// oops - we should try to get the instruction pointer out of here. 
						ODPRINTF((L"mhooks: SuspendOneThread: suspended thread %d - IP is at %p - IS COLLIDING WITH CODE", dwThreadId, pIp));
						ResumeThread(hThread);
						Sleep(100);
						SuspendThread(hThread);
						nTries++;
					} else {
						// we gave it all we could.
						ODPRINTF((L"mhooks: SuspendOneThread: suspended thread %d - IP is at %p - IS COLLIDING WITH CODE - CAN'T FIX", dwThreadId, pIp));
						ResumeThread(hThread);
						CloseHandle(hThread);
						hThread = NULL;
						break;
					}
				} else {
					// success, the IP is not conflicting
					ODPRINTF((L"mhooks: SuspendOneThread: Successfully suspended thread %d - IP is at %p", dwThreadId, pIp));
					break;
				}
			}
		} else {
			// couldn't suspend
			CloseHandle(hThread);
			hThread = NULL;
		}
	}
	return hThread;
}

//=========================================================================
// static INTERNAL function:
//
// Resumes all previously suspended threads in the current process.
//=========================================================================
static VOID ResumeOtherThreads() {
	// make sure things go as fast as possible
	INT nOriginalPriority = GetThreadPriority(GetCurrentThread());
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	// go through our list
	for (DWORD i=0; i<g_nThreadHandles; i++) {
		// and resume & close thread handles
		ResumeThread(g_hThreadHandles[i]);
		CloseHandle(g_hThreadHandles[i]);
	}
	// clean up
	free(g_hThreadHandles);
	g_hThreadHandles = NULL;
	g_nThreadHandles = 0;
	SetThreadPriority(GetCurrentThread(), nOriginalPriority);
}

//=========================================================================
// static INTERNAL function:
//
// Suspend all threads in this process while trying to make sure that their 
// instruction pointer is not in the given range.
//=========================================================================
static BOOL SuspendOtherThreads(PBYTE pbCode, DWORD cbBytes) {
	BOOL bRet = FALSE;
	// make sure we're the most important thread in the process
	INT nOriginalPriority = GetThreadPriority(GetCurrentThread());
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	// get a view of the threads in the system
	HANDLE hSnap = fnCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
	if (GOOD_HANDLE(hSnap)) {
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		// count threads in this process (except for ourselves)
		DWORD nThreadsInProcess = 0;
		if (fnThread32First(hSnap, &te)) {
			do {
				if (te.th32OwnerProcessID == GetCurrentProcessId()) {
					if (te.th32ThreadID != GetCurrentThreadId()) {
						nThreadsInProcess++;
					}
				}
				te.dwSize = sizeof(te);
			} while(fnThread32Next(hSnap, &te));
		}
		ODPRINTF((L"mhooks: SuspendOtherThreads: counted %d other threads", nThreadsInProcess));
		if (nThreadsInProcess) {
			// alloc buffer for the handles we really suspended
			g_hThreadHandles = (HANDLE*)malloc(nThreadsInProcess*sizeof(HANDLE));
			if (g_hThreadHandles) {
				ZeroMemory(g_hThreadHandles, nThreadsInProcess*sizeof(HANDLE));
				DWORD nCurrentThread = 0;
				BOOL bFailed = FALSE;
				te.dwSize = sizeof(te);
				// go through every thread
				if (fnThread32First(hSnap, &te)) {
					do {
						if (te.th32OwnerProcessID == GetCurrentProcessId()) {
							if (te.th32ThreadID != GetCurrentThreadId()) {
								// attempt to suspend it
								g_hThreadHandles[nCurrentThread] = SuspendOneThread(te.th32ThreadID, pbCode, cbBytes);
								if (GOOD_HANDLE(g_hThreadHandles[nCurrentThread])) {
									ODPRINTF((L"mhooks: SuspendOtherThreads: successfully suspended %d", te.th32ThreadID));
									nCurrentThread++;
								} else {
									ODPRINTF((L"mhooks: SuspendOtherThreads: error while suspending thread %d: %d", te.th32ThreadID, gle()));
									// TODO: this might not be the wisest choice
									// but we can choose to ignore failures on
									// thread suspension:
									// bFailed = TRUE;
								}
							}
						}
						te.dwSize = sizeof(te);
					} while(fnThread32Next(hSnap, &te) && !bFailed);
				}
				g_nThreadHandles = nCurrentThread;
				bRet = !bFailed;
			}
		}
		CloseHandle(hSnap);
		//TODO: we might want to have another go to make sure all threads
		// in the current process (including those that might have been
		// created since we took the original snapshot) have been 
		// suspended.
	} else {
		ODPRINTF((L"mhooks: SuspendOtherThreads: can't CreateToolhelp32Snapshot: %d", gle()));
	}
	SetThreadPriority(GetCurrentThread(), nOriginalPriority);
	if (!bRet) {
		ODPRINTF((L"mhooks: SuspendOtherThreads: Had a problem (or not running multithreaded), resuming all threads."));
		ResumeOtherThreads();
	}
	return bRet;
}

//=========================================================================
static DWORD DisassembleAndSkip(PVOID pFunction, DWORD dwMinLen) {
	DWORD dwRet = 0;
#ifdef _M_IX86
	ARCHITECTURE_TYPE arch = ARCH_X86;
#elif defined _M_X64
	ARCHITECTURE_TYPE arch = ARCH_X64;
#else
	#error unsupported platform
#endif
	DISASSEMBLER dis;
	if (InitDisassembler(&dis, arch)) {
		INSTRUCTION* pins = NULL;
		U8* pLoc = (U8*)pFunction;
		DWORD dwFlags = DISASM_DECODE | DISASM_DISASSEMBLE | DISASM_ALIGNOUTPUT;

		ODPRINTF((L"mhooks: DisassembleAndSkip: Disassembling %p", pLoc));
		while ( (dwRet < dwMinLen) && (pins = GetInstruction(&dis, (ULONG_PTR)pLoc, pLoc, dwFlags)) ) {
			ODPRINTF(("mhooks: DisassembleAndSkip: %p: %s", pLoc, pins->String));
			if (pins->Type == ITYPE_RET		) break;
			if (pins->Type == ITYPE_BRANCH	) break;
			if (pins->Type == ITYPE_BRANCHCC) break;
			if (pins->Type == ITYPE_CALL	) break;
			if (pins->Type == ITYPE_CALLCC	) break;
			if (pins->AnomalyOccurred		) break;
			dwRet += pins->Length;
			pLoc  += pins->Length;
		}

		CloseDisassembler(&dis);
	}

	return dwRet;
}

//=========================================================================
static BOOL Mhook_SetHook(PVOID *ppSystemFunction, PVOID pHookFunction) {
	MHOOKS_TRAMPOLINE* pTrampoline = NULL;
	PVOID pSystemFunction = *ppSystemFunction;
	// ensure thread-safety
	EnterCritSec();
	ODPRINTF((L"mhooks: Mhook_SetHook: Started on the job: %p / %p", pSystemFunction, pHookFunction));
	// find the real functions (jump over jump tables, if any)
	pSystemFunction = SkipJumps((PBYTE)pSystemFunction);
	pHookFunction   = SkipJumps((PBYTE)pHookFunction);
	ODPRINTF((L"mhooks: Mhook_SetHook: Started on the job: %p / %p", pSystemFunction, pHookFunction));
	// figure out the length of the overwrite zone
	DWORD dwInstructionLength = DisassembleAndSkip(pSystemFunction, MHOOK_JMPSIZE);
	if (dwInstructionLength >= MHOOK_JMPSIZE) {
		ODPRINTF((L"mhooks: Mhook_SetHook: disassembly signals %d bytes", dwInstructionLength));
		// suspend every other thread in this process, and make sure their IP 
		// is not in the code we're about to overwrite.
		SuspendOtherThreads((PBYTE)pSystemFunction, dwInstructionLength);
		// allocate a trampoline structure (note: it is pretty wasteful to get
		// VirtualAlloc to grab chunks of memory smaller than 100 bytes)
		pTrampoline = TrampolineAlloc((PBYTE)pSystemFunction);
		if (pTrampoline) {
			ODPRINTF((L"mhooks: Mhook_SetHook: allocated structure at %p", pTrampoline));
			// open ourselves so we can VirtualProtectEx
			HANDLE hProc = GetCurrentProcess();
			DWORD dwOldProtectSystemFunction = 0;
			DWORD dwOldProtectTrampolineFunction = 0;
			// set the system function to PAGE_EXECUTE_READWRITE
			if (VirtualProtectEx(hProc, pSystemFunction, dwInstructionLength, PAGE_EXECUTE_READWRITE, &dwOldProtectSystemFunction)) {
				ODPRINTF((L"mhooks: Mhook_SetHook: readwrite set on system function"));
				// mark our trampoline buffer to PAGE_EXECUTE_READWRITE
				if (VirtualProtectEx(hProc, pTrampoline, sizeof(MHOOKS_TRAMPOLINE), PAGE_EXECUTE_READWRITE, &dwOldProtectTrampolineFunction)) {
					ODPRINTF((L"mhooks: Mhook_SetHook: readwrite set on trampoline structure"));

					// create our trampoline function
					PBYTE pbCode = pTrampoline->codeTrampoline;
					// whatever was there in the beginning...
					for (DWORD i = 0; i<dwInstructionLength; i++) {
						pbCode[i] = ((PBYTE)pSystemFunction)[i];
					}
					pbCode += dwInstructionLength;
					// plus a jump to the continuation in the original location
					pbCode = EmitJump(pbCode, ((PBYTE)pSystemFunction) + dwInstructionLength);
					ODPRINTF((L"mhooks: Mhook_SetHook: updated the trampoline"));

					// create the function that jumps to the hook (we need this because 
					// jumping from the API to the hook directly might be a long jump, 
					// which is 14 bytes on x64, and we want to avoid that - the API
					// may or may not have room for such stuff. we do though.)
					pbCode = pTrampoline->codeJumpToHookFunction;
					pbCode = EmitJump(pbCode, (PBYTE)pHookFunction);
					ODPRINTF((L"mhooks: Mhook_SetHook: created reverse trampoline"));

					// update the API itself
					pbCode = (PBYTE)pSystemFunction;
					pbCode = EmitJump(pbCode, pTrampoline->codeJumpToHookFunction);
					ODPRINTF((L"mhooks: Mhook_SetHook: Hooked the function!"));

					// update data members
					pTrampoline->cbOverwrittenCode = dwInstructionLength;
					pTrampoline->pSystemFunction = (PBYTE)pSystemFunction;
					pTrampoline->pHookFunction = (PBYTE)pHookFunction;

					// flush instruction cache and restore original protection
					FlushInstructionCache(hProc, pTrampoline->codeTrampoline, dwInstructionLength);
					VirtualProtectEx(hProc, pTrampoline, sizeof(MHOOKS_TRAMPOLINE), dwOldProtectTrampolineFunction, &dwOldProtectTrampolineFunction);
				} else {
					ODPRINTF((L"mhooks: Mhook_SetHook: failed VirtualProtectEx 2: %d", gle()));
				}
				// flush instruction cache and restore original protection
				FlushInstructionCache(hProc, pSystemFunction, dwInstructionLength);
				VirtualProtectEx(hProc, pSystemFunction, dwInstructionLength, dwOldProtectSystemFunction, &dwOldProtectSystemFunction);
			} else {
				ODPRINTF((L"mhooks: Mhook_SetHook: failed VirtualProtectEx 1: %d", gle()));
			}
			if (pTrampoline->pSystemFunction) {
				*ppSystemFunction = pTrampoline->codeTrampoline;
			} else {
				// if we failed discard the trampoline (forcing VirtualFree)
				TrampolineFree(pTrampoline, TRUE);
				pTrampoline = NULL;
			}
		}
		// resume everybody else
		ResumeOtherThreads();
	} else {
		ODPRINTF((L"mhooks: disassembly signals %d bytes (unacceptable)", dwInstructionLength));
	}
	LeaveCritSec();
	return (pTrampoline != NULL);
}

//=========================================================================
static BOOL Mhook_Unhook(PVOID *ppHookedFunction) {
	ODPRINTF((L"mhooks: Mhook_Unhook: %p", *ppHookedFunction));
	BOOL bRet = FALSE;
	EnterCritSec();
	// get the trampoline structure that corresponds to our function
	MHOOKS_TRAMPOLINE* pTrampoline = TrampolineGet((PBYTE)*ppHookedFunction);
	if (pTrampoline) {
		// make sure nobody's executing code where we're about to overwrite a few bytes
		SuspendOtherThreads(pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode);
		ODPRINTF((L"mhooks: Mhook_Unhook: found struct at %p", pTrampoline));
		// open ourselves so we can VirtualProtectEx
		HANDLE hProc = GetCurrentProcess();
		DWORD dwOldProtectSystemFunction = 0;
		// make memory writable
		if (VirtualProtectEx(hProc, pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode, PAGE_EXECUTE_READWRITE, &dwOldProtectSystemFunction)) {
			ODPRINTF((L"mhooks: Mhook_Unhook: readwrite set on system function"));
			PBYTE pbCode = (PBYTE)pTrampoline->pSystemFunction;
			for (DWORD i = 0; i<pTrampoline->cbOverwrittenCode; i++) {
				pbCode[i] = pTrampoline->codeTrampoline[i];
			}
			// flush instruction cache and make memory unwritable
			FlushInstructionCache(hProc, pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode);
			VirtualProtectEx(hProc, pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode, dwOldProtectSystemFunction, &dwOldProtectSystemFunction);
			// return the original function pointer
			*ppHookedFunction = pTrampoline->pSystemFunction;
			bRet = TRUE;
			ODPRINTF((L"mhooks: Mhook_Unhook: sysfunc: %p", *ppHookedFunction));
			// free the trampoline while not really discarding it from memory
			TrampolineFree(pTrampoline, FALSE);
			ODPRINTF((L"mhooks: Mhook_Unhook: unhook successful"));
		} else {
			ODPRINTF((L"mhooks: Mhook_Unhook: failed VirtualProtectEx 1: %d", gle()));
		}
		// make the other guys runnable
		ResumeOtherThreads();
	}
	LeaveCritSec();
	return bRet;
}

//=========================================================================
// Copyright (C) 2003, Matt Conover (mconover@gmail.com)
#include <assert.h>
#include "mhook.h"

// NOTE: this assumes default scenarios (i.e., we assume CS/DS/ES/SS and flat
// and all have a base of 0 and limit of 0xffffffff, we don't try to verify
// that in the GDT)
//
// TODO: use inline assembly to get selector for segment
// Segment = x86 segment register (SEG_ES = 0, SEG_CS = 1, ...)
static BYTE *GetAbsoluteAddressFromSegment(BYTE Segment, DWORD Offset)
{
	switch (Segment)
	{
		// Windows uses a flat address space (except FS for x86 and GS for x64)
		case 0: // SEG_ES
		case 1: // SEG_CS
		case 2: // SEG_SS
		case 3: // SEG_DS
			return (BYTE *)(DWORD_PTR)Offset;
		case 4: // SEG_FS
		case 5: // SEG_GS
			return (BYTE *)(DWORD_PTR)Offset;
			// Note: we're really supposed to do this, but get_teb is not implemented
			// in this bastardized version of the disassembler.
			// return (BYTE *)get_teb() + Offset;
		default:
			assert(0);
			return (BYTE *)(DWORD_PTR)Offset;
	}
}




// Copyright (C) 2004, Matt Conover (mconover@gmail.com)
#undef NDEBUG
#include <assert.h>
#include <windows.h>

#ifdef NO_SANITY_CHECKS
#define NDEBUG
#undef assert
#define assert(x)
#endif

//////////////////////////////////////////////////////////////////////
// Global variables
//////////////////////////////////////////////////////////////////////

static ARCHITECTURE_FORMAT SupportedArchitectures[] =
{
	{ ARCH_X86,	&X86 },
	{ ARCH_X86_16, &X86 },
	{ ARCH_X64,	&X86 },
	{ ARCH_UNKNOWN, NULL }
};
/*
typedef struct _DISASM_ARG_INFO
{
	INSTRUCTION *MatchedInstruction;
	BOOL MatchPrefix;
	U8 *Opcode;
	U32 OpcodeLength;
	INSTRUCTION_TYPE InstructionType;
	U32 Count;
} DISASM_ARG_INFO;
*/
//////////////////////////////////////////////////////////////////////
// Function prototypes
//////////////////////////////////////////////////////////////////////

BOOL InitInstruction(INSTRUCTION *Instruction, DISASSEMBLER *Disassembler);
struct _ARCHITECTURE_FORMAT *GetArchitectureFormat(ARCHITECTURE_TYPE Type);

//////////////////////////////////////////////////////////////////////
// Disassembler setup
//////////////////////////////////////////////////////////////////////

static BOOL InitDisassembler(DISASSEMBLER *Disassembler, ARCHITECTURE_TYPE Architecture)
{
	ARCHITECTURE_FORMAT *ArchFormat;

	memset(Disassembler, 0, sizeof(DISASSEMBLER));
	Disassembler->Initialized = DISASSEMBLER_INITIALIZED;
	
	ArchFormat = GetArchitectureFormat(Architecture);
	if (!ArchFormat) { assert(0); return FALSE; }
	Disassembler->ArchType = ArchFormat->Type;
	Disassembler->Functions = ArchFormat->Functions;
	return TRUE;
}

static void CloseDisassembler(DISASSEMBLER *Disassembler)
{
	memset(Disassembler, 0, sizeof(DISASSEMBLER));
}

//////////////////////////////////////////////////////////////////////
// Instruction setup
//////////////////////////////////////////////////////////////////////

static BOOL InitInstruction(INSTRUCTION *Instruction, DISASSEMBLER *Disassembler)
{
	memset(Instruction, 0, sizeof(INSTRUCTION));
	Instruction->Initialized = INSTRUCTION_INITIALIZED;
	Instruction->Disassembler = Disassembler;
	memset(Instruction->String, ' ', MAX_OPCODE_DESCRIPTION-1);
	Instruction->String[MAX_OPCODE_DESCRIPTION-1] = '\0';
	return TRUE;
}

// If Decode = FALSE, only the following fields are valid:
// Instruction->Length, Instruction->Address, Instruction->Prefixes, Instruction->PrefixCount,
// Instruction->OpcodeBytes, Instruction->Instruction->OpcodeLength, Instruction->Groups,
// Instruction->Type, Instruction->OperandCount
//
// If Disassemble = TRUE, then Instruction->String is valid (also requires Decode = TRUE)
//
// WARNING: This will overwrite the previously obtained instruction
static INSTRUCTION *GetInstruction(DISASSEMBLER *Disassembler, U64 VirtualAddress, U8 *Address, U32 Flags)
{
	if (Disassembler->Initialized != DISASSEMBLER_INITIALIZED) { assert(0); return NULL; }
	assert(Address);
	InitInstruction(&Disassembler->Instruction, Disassembler);
	Disassembler->Instruction.Address = Address;	
	Disassembler->Instruction.VirtualAddressDelta = VirtualAddress - (U64)Address;
	if (!Disassembler->Functions->GetInstruction(&Disassembler->Instruction, Address, Flags))
	{
		assert(Disassembler->Instruction.Address == Address);
		assert(Disassembler->Instruction.Length < MAX_INSTRUCTION_LENGTH);

		// Save the address that failed, in case the lower-level disassembler didn't
		Disassembler->Instruction.Address = Address;
		Disassembler->Instruction.ErrorOccurred = TRUE;
		return NULL;
	}
	return &Disassembler->Instruction;
}

///////////////////////////////////////////////////////////////////////////
// Miscellaneous
///////////////////////////////////////////////////////////////////////////

static ARCHITECTURE_FORMAT *GetArchitectureFormat(ARCHITECTURE_TYPE Type)
{
	ARCHITECTURE_FORMAT *Format;
	for (Format = SupportedArchitectures; Format->Type != ARCH_UNKNOWN; Format++)
	{
		if (Format->Type == Type) return Format;
	}
	assert(0);
	return NULL;
}

// Copyright (C) 2004, Matt Conover (mconover@gmail.com)
#undef NDEBUG
#include <assert.h>


// Since addresses are static INTERNALly represented as 64-bit, we need to specially handle
// cases where IP + Displacement wraps around for 16-bit/32-bit operand size
// Otherwise, ignorethe possibility of wraparounds
#define SUPPORT_WRAPAROUND

#ifdef NO_SANITY_CHECKS
#undef NDEBUG
#undef DEBUG_DISASM
#undef assert
#define assert(x)
#endif

#ifdef DEBUG_DISASM
#define DISASM_OUTPUT(x) printf x
#else
#define DISASM_OUTPUT(x)
#endif

// Copyright (C) 2004, Matt Conover (mconover@gmail.com)
//
// The opcode tables in this file are based off the Intel Instruction Set Reference
// and an assortment of disassemblers, primarily libdisasm (by mammon)

#ifndef DISASM_X86_TABLES
#define DISASM_X86_TABLES

#define X86_GET_REG(val) ((val) & 7)
#define X86_GET_REG64(val) ((GET_REX_B(X86Instruction->rex_b) << 3) | ((val) & 7))

#define GET_MODRM_MOD(a) (((a) >> 6) & 3) // bits 6, 7
#define GET_MODRM_REG(a) (((a) >> 3) & 7) // bits 3, 4, 5
#define GET_MODRM_EXT(a) (((a) >> 3) & 7) // bits 3, 4, 5
#define GET_MODRM_RM(a)	 ((a) & 7) // bits 0, 1, 2

#define GET_SIB_SCALE(a) (((a) >> 6) & 3) // bits 6, 7
#define GET_SIB_INDEX(a) (((a) >> 3) & 7) // bits 3, 4, 5
#define GET_SIB_BASE(a)  ((a) & 7) // bits 0, 1, 2

#define REX_PREFIX_START 0x40
#define REX_PREFIX_END 0x4F
#define GET_REX_W(r) (((r) & 8) >> 3) // bit 3
#define GET_REX_R(r) (((r) & 4) >> 2) // bit 2
#define GET_REX_X(r) (((r) & 2) >> 1) // bit 1
#define GET_REX_B(r) ((r) & 1)        // bit 0
#define REX_MASK(n) ((n >> 16) & 0x0F) // bits 0-3

// Groupings to make the opcode table more readible
#define NOARGS { 0, 0, 0 }
#define NOCOND 0
#define NOGROUP NULL
#define NOACTION 0
#define NOCHANGE 0
#define IGNORED 0
#define NOINSTR NOGROUP, CPU_UNKNOWN, 0, "", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED
#define GROUP CPU_UNKNOWN, 0, "", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED
#define PREFIX NOGROUP, CPU_UNKNOWN, ITYPE_EXT_PREFIX, "", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED
#define EXT_FPU CPU_UNKNOWN, ITYPE_EXT_FPU, "", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED 
#define EXT_64 CPU_UNKNOWN, ITYPE_EXT_64, "", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED 
#define EXT_SUFFIX(a, b, c) CPU_UNKNOWN, ITYPE_EXT_SUFFIX, "", { a, b, c }, NOCOND, NOCHANGE, NOACTION, IGNORED
#define EXT_MODRM CPU_UNKNOWN, ITYPE_EXT_MODRM, "", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED

#define SET_MODRM(modrm, src) \
{ \
	(modrm).mod = GET_MODRM_MOD(src); \
	(modrm).reg = GET_MODRM_REG(src); \
	(modrm).rm = GET_MODRM_RM(src); \
}

#define SET_REX_MODRM(rex_modrm, rex, modrm) \
{ \
	(rex_modrm).rm = ((rex).b << 3) | (modrm).rm; \
	(rex_modrm).reg = ((rex).r << 3) | (modrm).reg; \
}

#define SET_SIB(sib, src) \
{ \
	(sib).scale = GET_SIB_SCALE(src); \
	(sib).index = GET_SIB_INDEX(src); \
	(sib).base =  GET_SIB_BASE(src); \
}

#define SET_REX_SIB(rex_sib, rex, sib) \
{ \
	(rex_sib).index = ((rex).x << 3) | (sib).index; \
	(rex_sib).base = ((rex).b << 3) | (sib).base; \
}

#define SET_REX(rex, src) \
{ \
	(rex).w = GET_REX_W(src);	\
	(rex).r = GET_REX_R(src);	\
	(rex).x = GET_REX_X(src);	\
	(rex).b = GET_REX_B(src); \
}

// Addressing modes
#define AMODE_A    0x00010000   
#define AMODE_C    0x00020000
#define AMODE_D    0x00030000
#define AMODE_E    0x00040000
#define AMODE_G    0x00050000
#define AMODE_I    0x00060000
#define AMODE_J    0x00070000
#define AMODE_M    0x00080000
#define AMODE_O    0x00090000
#define AMODE_P    0x000A0000
#define AMODE_Q    0x000B0000
#define AMODE_R    0x000C0000
#define AMODE_S    0x000D0000
#define AMODE_T    0x000E0000
#define AMODE_V    0x000F0000
#define AMODE_W    0x00100000
#define AMODE_X    0x00110000
#define AMODE_Y    0x00120000
#define AMODE_PR   0x00130000
#define AMODE_VR   0x00140000
#define AMODE_xlat 0x00150000

// Operand types
#define OPTYPE_a    0x01000000
#define OPTYPE_b    0x02000000
#define OPTYPE_d    0x03000000
#define OPTYPE_p    0x04000000
#define OPTYPE_q    0x05000000
#define OPTYPE_dt   0x06000000
#define OPTYPE_v    0x07000000
#define OPTYPE_w    0x08000000
#define OPTYPE_ps   0x09000000 // packed 128-bit single real
#define OPTYPE_pd   0x0A000000 // packed 128-bit double real
#define OPTYPE_pb   0x0B000000 // packed BCD (10 bytes, 18-bit precision)
#define OPTYPE_ss   0x0C000000 // scalar single real
#define OPTYPE_sd   0x0D000000 // scalar double real
#define OPTYPE_se   0x0E000000 // scalar extended real
#define OPTYPE_fev  0x0F000000 // FPU environment (28 bytes if 32-bit modes, 14 bytes in 16-bit mode)
#define OPTYPE_fst1 0x10000000 // FPU state (108 bytes in 32-bit modes, 94 bytes in 16-bit real mode)
#define OPTYPE_fst2 0x11000000 // FPU/MMX/XMM/MXCSR state (512 bytes)
#define OPTYPE_z    0x12000000
#define OPTYPE_o    0x13000000
#define OPTYPE_dq   0x14000000 // OPTYPE_d or OPTYPE_o
#define OPTYPE_mw   0x15000000 // word if memory, register size otherwise
#define OPTYPE_sso  0x16000000 // OPTYPE_ss or OPTYPE_o
#define OPTYPE_sdo  0x17000000 // OPTYPE_ss or OPTYPE_o
#define OPTYPE_cpu  0x18000000 // pointer to CPU state structure
#define OPTYPE_lea  0x19000000 // size set by other operand
// NOTE: if you change this, you must also update OptypeHandlers[] in disasm_x86.c
// Be sure to preserve the ordering

//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// Registers
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

static char *Addressing16[8] = {"bx+si","bx+di","bp+si","bp+di","si","di","bp","bx"};
static char *MMX_Registers[8] = {"mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"};
static char *SSE_Registers[8] = {"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"};
static char *DR_Registers[8] = {"dr0", "dr1", "dr2", "dr3", "dr4", "dr5", "dr6", "dr7"};
static char *CR_Registers[8] = {"cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7"};
static char *TR_Registers[8] = {"tr0", "tr1", "tr2", "tr3", "tr4", "tr5", "tr6", "tr7"};
static char *FPU_Registers[8] = {"st(0)", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)"};
static char *Segments[8] = {"es", "cs", "ss", "ds", "fs", "gs", "ERROR", "ERROR"};
static char *Registers8[8] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
static char *Registers16[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
static char *Registers32[8] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" };
static char *REX_Registers8[16] = {"al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b" };
static char *REX_Registers16[16] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w" };
static char *REX_Registers32[16] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d" };
static char *REX_Registers64[16] = {"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" };
static char *DataSizes[8+1] = {"byte ptr", "word ptr", "dword ptr", "6_byte ptr", "qword ptr", "10_byte ptr", "INVALID PTR", "INVALID PTR", "oword ptr"};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// FPU constants
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

static BYTE float_0[10]   = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static BYTE float_1[10] =   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x3F };
static BYTE float_l2t[10] = { 0xFE, 0x8A, 0x1B, 0xCD, 0x4B, 0x78, 0x9A, 0xD4, 0x00, 0x40 };
static BYTE float_l2e[10] = { 0xBC, 0xF0, 0x17, 0x5C, 0x29, 0x3B, 0xAA, 0xB8, 0xFF, 0x3F };
static BYTE float_pi[10]  = { 0x35, 0xC2, 0x68, 0x21, 0xA2, 0xDA, 0x0F, 0xC9, 0x00, 0x40 };
static BYTE float_lg2[10] = { 0x99, 0xF7, 0xCF, 0xFB, 0x84, 0x9A, 0x20, 0x9A, 0xFD, 0x3F };
static BYTE float_ln2[10] = { 0xAC, 0x79, 0xCF, 0xD1, 0xF7, 0x17, 0x72, 0xB1, 0xFE, 0x3F };

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// Tables
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

extern X86_OPCODE X86_Opcodes_2[0x100];
extern X86_OPCODE X86_Group_1_80[8], X86_Group_1_81[8], X86_Group_1_82[8], X86_Group_1_83[8], X86_Group_2_C0[8], X86_Group_2_C1[8], X86_Group_2_D0[8], X86_Group_2_D1[8], X86_Group_2_D2[8], X86_Group_2_D3[8], X86_Group_3_F6[8], X86_Group_3_F7[8], X86_Group_4[8], X86_Group_5[8], X86_Group_6[8], X86_Group_7[8], X86_Group_8[8], X86_Group_9[8], X86_Group_10[8], X86_Group_11[8], X86_Group_12_C6[8], X86_Group_12_C7[8], X86_Group_13[8], X86_Group_14[8], X86_Group_15[8], X86_Group_16[8], X86_Group_17[8], X86_Group_P[8];
extern X86_OPCODE X86_SSE[0x300], X86_SSE2_Group_13[24], X86_SSE2_Group_14[24], X86_SSE2_Group_15[24];
extern X86_OPCODE X86_ESC_0[0x48], X86_ESC_1[0x48], X86_ESC_2[0x48], X86_ESC_3[0x48], X86_ESC_3[0x48], X86_ESC_4[0x48], X86_ESC_5[0x48], X86_ESC_6[0x48], X86_ESC_7[0x48];
extern X86_OPCODE X86_3DNOW_0F[0x100];
extern X86_OPCODE X86_0F01_ModRM[0x100];
extern X86_OPCODE X86_Opcode_63[2], X86_Opcode_0F05[2];

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// Opcode tables
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

static X86_OPCODE X86_Opcodes_1[0x100] = // 1 byte opcodes
{
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { AMODE_E | OPTYPE_b | OP_DST, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { AMODE_G | OPTYPE_b | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { OPTYPE_REG_AL | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OPTYPE_ES | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OPTYPE_ES | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x07 */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { AMODE_E | OPTYPE_b | OP_DST, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x08 */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x09 */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { AMODE_G | OPTYPE_b | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x0A */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x0B */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { OPTYPE_REG_AL | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x0C */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x0D */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OPTYPE_CS | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x0E */
	{ X86_Opcodes_2, GROUP }, /* 0x0F */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { AMODE_E | OPTYPE_b | OP_DST, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x10 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x11 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { AMODE_G | OPTYPE_b | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x12 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x13 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { OPTYPE_REG_AL | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x14 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x15 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OPTYPE_SS | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x16 */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OPTYPE_SS | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x17 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { AMODE_E | OPTYPE_b | OP_DST, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x18 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x19 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { AMODE_G | OPTYPE_b | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x1A */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x1B */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { OPTYPE_REG_AL | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x1C */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x1D */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OPTYPE_DS | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x1E */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OPTYPE_DS | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x1F */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { AMODE_E | OPTYPE_b | OP_DST, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x20 */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x21 */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { AMODE_G | OPTYPE_b | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x22 */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x23 */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { OPTYPE_REG_AL | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x24 */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x25 */
	{ PREFIX }, /* 0x26 */
	{ NOGROUP, CPU_I386, ITYPE_BCDCONV, "daa", { OPTYPE_REG_AL | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_AF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x27 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { AMODE_E | OPTYPE_b | OP_DST, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x28 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x29 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { AMODE_G | OPTYPE_b | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x2A */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x2B */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { OPTYPE_REG_AL | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x2C */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x2D */
	{ PREFIX }, /* 0x2E */
	{ NOGROUP, CPU_I386, ITYPE_BCDCONV, "das", { OPTYPE_REG_AL | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_AF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x2F */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { AMODE_E | OPTYPE_b | OP_DST, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x30 */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x31 */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { AMODE_G | OPTYPE_b | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x32 */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x33 */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { OPTYPE_REG_AL | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x34 */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x35 */
	{ PREFIX }, /* 0x36 */
	{ NOGROUP, CPU_I386, ITYPE_BCDCONV, "aaa", { OPTYPE_REG_AL | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_AF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x37 */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { AMODE_E | OPTYPE_b | OP_SRC, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x38 */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { AMODE_E | OPTYPE_v | OP_SRC, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x39 */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { AMODE_G | OPTYPE_b | OP_SRC, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x3A */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { AMODE_G | OPTYPE_v | OP_SRC, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x3B */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { OPTYPE_REG_AL | OP_SIGNED | OP_SRC, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x3C */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { OPTYPE_REG_xAX_BIG | OP_SIGNED | OP_SRC, AMODE_I | OPTYPE_z | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x3D */
	{ PREFIX }, /* 0x3E */
	{ NOGROUP, CPU_I386, ITYPE_BCDCONV, "aas", { OPTYPE_REG_AL | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_AF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x3F */
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x40 */
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x41 */
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x42 */
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x43 */
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x44 */
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x45 */
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x46 */
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x47 */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x48 */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x49 */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x4A */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x4B */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x4C */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x4D */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x4E */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x4F */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OP_REG | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x50 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OP_REG | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x51 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OP_REG | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x52 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OP_REG | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x53 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OP_REG | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x54 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OP_REG | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x55 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OP_REG | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x56 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OP_REG | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x57 */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OP_REG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x58 */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OP_REG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x59 */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OP_REG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5A */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OP_REG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5B */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OP_REG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5C */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OP_REG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5D */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OP_REG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5E */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OP_REG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5F */
	{ NOGROUP, CPU_I386, ITYPE_PUSHA, "pushad", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x60 */
	{ NOGROUP, CPU_I386, ITYPE_POPA, "popad", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x61 */
	{ NOGROUP, CPU_I386, ITYPE_BOUNDS, "bound", { AMODE_G | OPTYPE_v | OP_SRC, AMODE_M | OPTYPE_a | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x62 */
	{ X86_Opcode_63, EXT_64 }, /* 0x63 */
	{ PREFIX }, /* 0x64 */
	{ PREFIX }, /* 0x65 */ 
	{ PREFIX }, /* 0x66 */
	{ PREFIX }, /* 0x67 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { AMODE_I | OPTYPE_z | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x68 */
	{ NOGROUP, CPU_I386, ITYPE_MUL, "imul", { AMODE_G | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_v | OP_SIGNED | OP_SRC, AMODE_I | OPTYPE_z | OP_SIGNED | OP_SRC }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x69 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x6A */
	{ NOGROUP, CPU_I386, ITYPE_MUL, "imul", { AMODE_G | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_v | OP_SIGNED | OP_SRC, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x6B */
	{ NOGROUP, CPU_I386, ITYPE_IN, "insb", { AMODE_Y | OPTYPE_b | OP_DST, OPTYPE_REG_DX | OP_SYS | OP_SRC, 0 }, COND_D, NOCHANGE, xDI_DEC | xCX_REP_DEC, xDI_INC | xCX_REP_DEC }, /* 0x6C */
	{ NOGROUP, CPU_I386, ITYPE_IN, "insd", { AMODE_Y | OPTYPE_z | OP_DST, OPTYPE_REG_DX | OP_SYS | OP_SRC, 0 }, COND_D, NOCHANGE, xDI_DECx | xCX_REP_DEC, xDI_INCx | xCX_REP_DEC }, /* 0x6D */
	{ NOGROUP, CPU_I386, ITYPE_OUT, "outsb", { OPTYPE_REG_DX | OP_SYS | OP_DST, AMODE_X | OPTYPE_b | OP_SRC, 0 }, COND_D, NOCHANGE, xSI_DEC | xCX_REP_DEC, xSI_INC  | xCX_REP_DEC }, /* 0x6E */
	{ NOGROUP, CPU_I386, ITYPE_OUT, "outsd", { OPTYPE_REG_DX | OP_SYS | OP_DST, AMODE_X | OPTYPE_z | OP_SRC, 0 }, COND_D, NOCHANGE, xSI_DECx | xCX_REP_DEC, xSI_INCx  | xCX_REP_DEC}, /* 0x6F */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jo", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_O, NOCHANGE, OP1_DST, NOACTION }, /* 0x70 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jno", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_NO, NOCHANGE, OP1_DST, NOACTION }, /* 0x71 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jb", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_C, NOCHANGE, OP1_DST, NOACTION }, /* 0x72 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jnb", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_NC, NOCHANGE, OP1_DST, NOACTION }, /* 0x73 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jz", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_Z, NOCHANGE, OP1_DST, NOACTION }, /* 0x74 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jnz", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_NZ, NOCHANGE, OP1_DST, NOACTION }, /* 0x75 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jbe", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_BE, NOCHANGE, OP1_DST, NOACTION }, /* 0x76 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "ja", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_A, NOCHANGE, OP1_DST, NOACTION }, /* 0x77 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "js", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_S, NOCHANGE, OP1_DST, NOACTION }, /* 0x78 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jns", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_NS, NOCHANGE, OP1_DST, NOACTION }, /* 0x79 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jpe", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_PE, NOCHANGE, OP1_DST, NOACTION }, /* 0x7A */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jpo", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_PO, NOCHANGE, OP1_DST, NOACTION }, /* 0x7B */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jl", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_L, NOCHANGE, OP1_DST, NOACTION }, /* 0x7C */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jge", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_GE, NOCHANGE, OP1_DST, NOACTION }, /* 0x7D */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jle", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_LE, NOCHANGE, OP1_DST, NOACTION }, /* 0x7E */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jg", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_G, NOCHANGE, OP1_DST, NOACTION }, /* 0x7F */
	{ X86_Group_1_80, GROUP }, /* 0x80 Eb Ib */
	{ X86_Group_1_81, GROUP }, /* 0x81 Ev Iz */
	{ X86_Group_1_82, GROUP }, /* 0x82 Eb Ib */
	{ X86_Group_1_83, GROUP }, /* 0x83 Ev Ib */
	{ NOGROUP, CPU_I386, ITYPE_TEST, "test", { AMODE_E | OPTYPE_b | OP_SRC, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x84 */
	{ NOGROUP, CPU_I386, ITYPE_TEST, "test", { AMODE_E | OPTYPE_v | OP_SRC, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x85 */
	{ NOGROUP, CPU_I386, ITYPE_XCHG, "xchg", { AMODE_E | OPTYPE_b | OP_SRC | OP_DST, AMODE_G | OPTYPE_b | OP_SRC | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x86 */
	{ NOGROUP, CPU_I386, ITYPE_XCHG, "xchg", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, AMODE_G | OPTYPE_v | OP_SRC | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x87 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_E | OPTYPE_b | OP_DST, AMODE_G | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x88 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x89 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_G | OPTYPE_b | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x8A */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x8B */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_E | OPTYPE_mw | OP_DST, AMODE_S | OPTYPE_w | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x8C */
	{ NOGROUP, CPU_I386, ITYPE_LEA, "lea", { AMODE_G | OPTYPE_v | OP_DST, AMODE_M | OPTYPE_lea | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x8D */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_S | OPTYPE_w | OP_DST, AMODE_E | OPTYPE_w | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x8E */
	{ X86_Group_10, GROUP }, /* 0x8F */
	{ NOGROUP, CPU_I386, ITYPE_NOP, "nop", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x90 */
	{ NOGROUP, CPU_I386, ITYPE_XCHG, "xchg", { OPTYPE_REG_xAX_BIG | OP_SRC | OP_DST, OP_REG | OP_SRC | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x91 */
	{ NOGROUP, CPU_I386, ITYPE_XCHG, "xchg", { OPTYPE_REG_xAX_BIG | OP_SRC | OP_DST, OP_REG | OP_SRC | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x92 */
	{ NOGROUP, CPU_I386, ITYPE_XCHG, "xchg", { OPTYPE_REG_xAX_BIG | OP_SRC | OP_DST, OP_REG | OP_SRC | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x93 */
	{ NOGROUP, CPU_I386, ITYPE_XCHG, "xchg", { OPTYPE_REG_xAX_BIG | OP_SRC | OP_DST, OP_REG | OP_SRC | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x94 */
	{ NOGROUP, CPU_I386, ITYPE_XCHG, "xchg", { OPTYPE_REG_xAX_BIG | OP_SRC | OP_DST, OP_REG | OP_SRC | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x95 */
	{ NOGROUP, CPU_I386, ITYPE_XCHG, "xchg", { OPTYPE_REG_xAX_BIG | OP_SRC | OP_DST, OP_REG | OP_SRC | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x96 */
	{ NOGROUP, CPU_I386, ITYPE_XCHG, "xchg", { OPTYPE_REG_xAX_BIG | OP_SRC | OP_DST, OP_REG | OP_SRC | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x97 */
	{ NOGROUP, CPU_I386, ITYPE_SZCONV, "cwde", { OPTYPE_REG_xAX_BIG | OP_SIGNED | OP_DST, OPTYPE_REG_xAX_SMALL | OP_SIGNED | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x98 */
	{ NOGROUP, CPU_I386, ITYPE_SZCONV, "cdq", { OPTYPE_xDX_HI_xAX_LO | OP_SIGNED | OP_DST, OPTYPE_REG_xAX_BIG | OP_SIGNED | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x99 */
	{ NOGROUP, CPU_I386, ITYPE_CALL, "call", { AMODE_A | OPTYPE_p | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x9A */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "wait", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x9B */
	{ NOGROUP, CPU_I386, ITYPE_PUSHF, "pushf", { OPTYPE_xFLAGS | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x9C */
	{ NOGROUP, CPU_I386, ITYPE_POPF, "popf", { OPTYPE_xFLAGS | OP_DST, 0, 0 }, NOCOND, FLAG_ALL_MOD, NOACTION, IGNORED }, /* 0x9D */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "sahf", { OPTYPE_FLAGS | OP_DST, OPTYPE_REG_AH | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_AF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x9E */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "lahf", { OPTYPE_REG_AH | OP_DST, OPTYPE_FLAGS | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x9F */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG_AL | OP_DST, AMODE_O | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xA0 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_O | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xA1 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_O | OPTYPE_b | OP_DST, OPTYPE_REG_AL | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xA2 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_O | OPTYPE_v | OP_DST, OPTYPE_REG_xAX_BIG | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xA3 */
	{ NOGROUP, CPU_I386, ITYPE_STRMOV, "movsb", { AMODE_Y | OPTYPE_b | OP_DST, AMODE_X | OPTYPE_b | OP_SRC, 0 }, COND_D, NOCHANGE, xDI_DEC | xSI_DEC | xCX_REP_DEC, xDI_INC | xSI_INC | xCX_REP_DEC}, /* 0xA4 */
	{ NOGROUP, CPU_I386, ITYPE_STRMOV, "movsd", { AMODE_Y | OPTYPE_z | OP_DST, AMODE_X | OPTYPE_z | OP_SRC, 0 }, COND_D, NOCHANGE, xDI_DECx | xSI_DECx| xCX_REP_DEC, xDI_INCx | xSI_INCx | xCX_REP_DEC }, /* 0xA5 */
	{ NOGROUP, CPU_I386, ITYPE_STRCMP, "cmpsb", { AMODE_X | OPTYPE_b | OP_SRC, AMODE_Y | OPTYPE_b | OP_SRC, 0 }, COND_D, FLAG_COMMON_MOD, xDI_DEC | xSI_DEC | xCX_REP_DEC, xDI_INC | xSI_INC | xCX_REP_DEC }, /* 0xA6 */
	{ NOGROUP, CPU_I386, ITYPE_STRCMP, "cmpsd", { AMODE_X | OPTYPE_z | OP_SRC, AMODE_Y | OPTYPE_z | OP_SRC, 0 }, COND_D, FLAG_COMMON_MOD, xDI_DECx | xSI_DECx | xCX_REP_DEC, xDI_INCx | xSI_INCx | xCX_REP_DEC}, /* 0xA7 */
	{ NOGROUP, CPU_I386, ITYPE_TEST, "test", { OPTYPE_REG_AL | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0xA8 */
	{ NOGROUP, CPU_I386, ITYPE_TEST, "test", { OPTYPE_REG_xAX_BIG | OP_SRC, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0xA9 */
	{ NOGROUP, CPU_I386, ITYPE_STRSTOR, "stosb", { AMODE_Y | OPTYPE_b | OP_DST, OPTYPE_REG_AL | OP_SRC, 0 }, COND_D, NOCHANGE, xDI_DEC | xSI_DEC | xCX_REP_DEC, xDI_INC | xSI_INC | xCX_REP_DEC }, /* 0xAA */
	{ NOGROUP, CPU_I386, ITYPE_STRSTOR, "stosd", { AMODE_Y | OPTYPE_z | OP_DST, OPTYPE_REG_xAX_BIG | OP_SRC, 0 }, COND_D, NOCHANGE, xDI_DECx | xSI_DECx | xCX_REP_DEC, xDI_INCx | xSI_INCx | xCX_REP_DEC }, /* 0xAB */
	{ NOGROUP, CPU_I386, ITYPE_STRLOAD, "lodsb", { OPTYPE_REG_AL | OP_DST, AMODE_X | OPTYPE_b | OP_SRC, 0 }, COND_D, NOCHANGE, xSI_DEC | xCX_REP_DEC, xSI_INC | xCX_REP_DEC }, /* 0xAC */
	{ NOGROUP, CPU_I386, ITYPE_STRLOAD, "lodsd", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_X | OPTYPE_z | OP_SRC, 0 }, COND_D, NOCHANGE, xSI_DECx | xCX_REP_DEC, xSI_INCx | xCX_REP_DEC }, /* 0xAD */
	{ NOGROUP, CPU_I386, ITYPE_STRCMP, "scasb", { OPTYPE_REG_AL | OP_SRC, AMODE_Y | OPTYPE_b | OP_SRC, 0 }, COND_D, FLAG_COMMON_MOD, xDI_DEC | xCX_REP_DEC, xDI_INC | xCX_REP_DEC }, /* 0xAE */
	{ NOGROUP, CPU_I386, ITYPE_STRCMP, "scasd", { OPTYPE_REG_xAX_BIG | OP_SRC, AMODE_Y | OPTYPE_z | OP_SRC, 0 }, COND_D, FLAG_COMMON_MOD, xDI_DECx, xDI_INCx }, /* 0xAF */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG8 | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB0 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG8 | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB1 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG8 | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB2 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG8 | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB3 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG8 | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB4 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG8 | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB5 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG8 | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB6 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OPTYPE_REG8 | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB7 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OP_REG | OP_DST, AMODE_I | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB8 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OP_REG | OP_DST, AMODE_I | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB9 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OP_REG | OP_DST, AMODE_I | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xBA */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OP_REG | OP_DST, AMODE_I | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xBB */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OP_REG | OP_DST, AMODE_I | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xBC */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OP_REG | OP_DST, AMODE_I | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xBD */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OP_REG | OP_DST, AMODE_I | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xBE */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { OP_REG | OP_DST, AMODE_I | OPTYPE_v | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xBF */
	{ X86_Group_2_C0, GROUP }, /* 0xC0 Eb Ib */
	{ X86_Group_2_C1, GROUP }, /* 0xC1 Ev Ib */
	{ NOGROUP, CPU_I386, ITYPE_RET, "ret", { AMODE_I | OPTYPE_w | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC2 */
	{ NOGROUP, CPU_I386, ITYPE_RET, "ret", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC3 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "les", { AMODE_G | OPTYPE_z | OP_DST, AMODE_M | OPTYPE_p | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC4 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "lds", { AMODE_G | OPTYPE_z | OP_DST, AMODE_M | OPTYPE_p | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC5 */
	{ X86_Group_12_C6, GROUP }, /* 0xC6 Eb Ib */
	{ X86_Group_12_C7, GROUP }, /* 0xC7 Ev Iz */
	{ NOGROUP, CPU_I386, ITYPE_ENTER, "enter", { OPTYPE_REG_xBP | OP_SRC | OP_DST, AMODE_I | OPTYPE_w | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC8 */
	{ NOGROUP, CPU_I386, ITYPE_LEAVE, "leave", { OPTYPE_REG_xBP | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC9 */
	{ NOGROUP, CPU_I386, ITYPE_RET, "retf", { AMODE_I | OPTYPE_w | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCA */
	{ NOGROUP, CPU_I386, ITYPE_RET, "retf", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCB */
	{ NOGROUP, CPU_I386, ITYPE_DEBUG, "int3", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCC */
	{ NOGROUP, CPU_I386, ITYPE_TRAP, "int", { AMODE_I | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCD */
	{ NOGROUP, CPU_I386, ITYPE_OFLOW, "into", NOARGS, NOCOND, NOCHANGE, NOACTION, NOACTION }, /* 0xCE */
	{ NOGROUP, CPU_I386, ITYPE_TRAPRET, "iret", NOARGS, NOCOND, FLAG_ALL_MOD, NOACTION, IGNORED }, /* 0xCF */
	{ X86_Group_2_D0, GROUP }, /* 0xD0 Eb, 1 */
	{ X86_Group_2_D1, GROUP }, /* 0xD1 Ev, 1 */
	{ X86_Group_2_D2, GROUP }, /* 0xD2 Eb, CL */
	{ X86_Group_2_D3, GROUP }, /* 0xD3 Ev, CL */
	{ NOGROUP, CPU_I386, ITYPE_BCDCONV, "aam", { OPTYPE_REG_AX | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD, NOACTION, IGNORED }, /* 0xD4 */
	{ NOGROUP, CPU_I386, ITYPE_BCDCONV, "aad", { OPTYPE_REG_AX | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD, NOACTION, IGNORED }, /* 0xD5 */
	{ NOGROUP, CPU_I386, ITYPE_ARITH, "salc", { OPTYPE_REG_AL | OP_DST, OPTYPE_FF | OP_SRC, OPTYPE_0 | OP_SRC }, COND_C, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0xD6 */
	{ NOGROUP, CPU_I386, ITYPE_XLAT, "xlat", { OPTYPE_REG_AL | OP_DST, AMODE_xlat | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xD7 */
	{ X86_ESC_0, EXT_FPU }, /* 0xD8 */
	{ X86_ESC_1, EXT_FPU }, /* 0xD9 */
	{ X86_ESC_2, EXT_FPU }, /* 0xDA */
	{ X86_ESC_3, EXT_FPU }, /* 0xDB */
	{ X86_ESC_4, EXT_FPU }, /* 0xDC */
	{ X86_ESC_5, EXT_FPU }, /* 0xDD */
	{ X86_ESC_6, EXT_FPU }, /* 0xDE */
	{ X86_ESC_7, EXT_FPU }, /* 0xDF */
	{ NOGROUP, CPU_I386, ITYPE_LOOPCC, "loopnz", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_REG_xCX_BIG_NZ | COND_NZ, NOCHANGE, OP1_DST | xCX_DEC, NOACTION }, /* 0xE0 */
	{ NOGROUP, CPU_I386, ITYPE_LOOPCC, "loopz", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_REG_xCX_BIG_NZ | COND_Z, NOCHANGE, OP1_DST | xCX_DEC, NOACTION }, /* 0xE1 */
	{ NOGROUP, CPU_I386, ITYPE_LOOPCC, "loop", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_REG_xCX_BIG_NZ, NOCHANGE, OP1_DST | xCX_DEC, NOACTION }, /* 0xE2 */
	{ NOGROUP, CPU_I386, ITYPE_LOOPCC, "jecxz", { AMODE_J | OPTYPE_b | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_REG_xCX_BIG_Z, NOCHANGE, OP1_DST, NOACTION }, /* 0xE3 */
	{ NOGROUP, CPU_I386, ITYPE_IN, "in", { OPTYPE_REG_AL | OP_DST, AMODE_I | OPTYPE_b | OP_SYS | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, NOACTION }, /* 0xE4 */
	{ NOGROUP, CPU_I386, ITYPE_IN, "in", { OPTYPE_REG_xAX_BIG | OP_DST, AMODE_I | OPTYPE_b | OP_SYS | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, NOACTION }, /* 0xE5 */
	{ NOGROUP, CPU_I386, ITYPE_OUT, "out", { AMODE_I | OPTYPE_b | OP_SYS | OP_DST, OPTYPE_REG_AL | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, NOACTION }, /* 0xE6 */
	{ NOGROUP, CPU_I386, ITYPE_OUT, "out", { AMODE_I | OPTYPE_b | OP_SYS | OP_DST, OPTYPE_REG_xAX_BIG | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, NOACTION }, /* 0xE7 */
	{ NOGROUP, CPU_I386, ITYPE_CALL, "call", { AMODE_J | OPTYPE_z | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE8 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCH, "jmp", { AMODE_J | OPTYPE_z | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE9 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCH, "jmp", { AMODE_A | OPTYPE_p | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEA */
	{ NOGROUP, CPU_I386, ITYPE_BRANCH, "jmp", { AMODE_J | OPTYPE_b | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEB */
	{ NOGROUP, CPU_I386, ITYPE_IN, "in", { OPTYPE_REG_AL | OP_DST, OPTYPE_REG_DX | OP_SYS | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEC */
	{ NOGROUP, CPU_I386, ITYPE_IN, "in", { OPTYPE_REG_xAX_BIG | OP_DST, OPTYPE_REG_DX | OP_SYS | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xED */
	{ NOGROUP, CPU_I386, ITYPE_OUT, "out", { OPTYPE_REG_DX | OP_SYS | OP_DST, OPTYPE_REG_AL | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEE */
	{ NOGROUP, CPU_I386, ITYPE_OUT, "out", { OPTYPE_REG_DX | OP_SYS | OP_DST, OPTYPE_REG_xAX_BIG | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEF */
	{ PREFIX }, /* 0xF0 */
	{ NOGROUP, CPU_I386, ITYPE_DEBUG, "int1", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, // aka icebp /* 0xF1 */
	{ PREFIX }, /* 0xF2 */
	{ PREFIX }, /* 0xF3 */
	{ NOGROUP, CPU_I386, ITYPE_HALT, "hlt", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF4 */
	{ NOGROUP, CPU_I386, ITYPE_TOGCF, "cmc", { OPTYPE_FLAGS | OP_DST, 0, 0 }, NOCOND, FLAG_CF_TOG, IGNORED }, /* 0xF5 */
	{ X86_Group_3_F6, GROUP }, /* 0xF6 Eb */
	{ X86_Group_3_F7, GROUP }, /* 0xF7 Ev */
	{ NOGROUP, CPU_I386, ITYPE_CLEARCF, "clc", { OPTYPE_FLAGS | OP_DST, 0, 0 }, NOCOND, FLAG_CF_CLR, NOACTION, IGNORED }, /* 0xF8 */
	{ NOGROUP, CPU_I386, ITYPE_SETCF, "stc", { OPTYPE_FLAGS | OP_DST, 0, 0 }, NOCOND, FLAG_CF_SET, NOACTION, IGNORED }, /* 0xF9 */
	{ NOGROUP, CPU_I386, ITYPE_CLEARIF, "cli", { OPTYPE_FLAGS | OP_DST, 0, 0 }, NOCOND, FLAG_IF_CLR, NOACTION, IGNORED }, /* 0xFA */
	{ NOGROUP, CPU_I386, ITYPE_SETIF, "sti", { OPTYPE_FLAGS | OP_DST, 0, 0 }, NOCOND, FLAG_IF_SET, NOACTION, IGNORED }, /* 0xFB */
	{ NOGROUP, CPU_I386, ITYPE_CLEARDF, "cld", { OPTYPE_FLAGS | OP_DST, 0, 0 }, NOCOND, FLAG_DF_CLR, NOACTION, IGNORED }, /* 0xFC */
	{ NOGROUP, CPU_I386, ITYPE_SETDF, "std", { OPTYPE_FLAGS | OP_DST, 0, 0 }, NOCOND, FLAG_DF_SET, NOACTION, IGNORED }, /* 0xFD */
	{ X86_Group_4, GROUP }, /* 0xFE */
	{ X86_Group_5, GROUP }, /* 0xFF */
};

X86_OPCODE X86_Opcodes_2[0x100] = // 2 byte opcodes
{
	{ X86_Group_6, GROUP }, /* 0x00 */
	{ X86_0F01_ModRM, EXT_MODRM }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "lar", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_w | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "lsl", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_w | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOINSTR }, /* 0x04 */
	{ X86_Opcode_0F05, EXT_64 }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "clts", { OPTYPE_CR0 | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_SYSCALLRET, "sysret", { OPTYPE_STAR_MSR | OP_MSR | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x07 */
	// 0F 07 could also be this undocumented instruction on older CPUs:
	// { NOGROUP, CPU_I386, ITYPE_SYSTEM, "loadall", { AMODE_Y | OPTYPE_cpu | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x07 */
	{ NOGROUP, CPU_I486, ITYPE_SYSTEM, "invd", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x08 */
	{ NOGROUP, CPU_I486, ITYPE_SYSTEM, "wbinvd", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x09 */
	{ NOINSTR }, /* 0x0A */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_INVALID, "undef" /* aka UD2 */, NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x0B */
	{ NOINSTR }, /* 0x0C */
	{ X86_Group_P, GROUP }, /* 0x0D */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "femms", { OPTYPE_FPU_TAG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x0E */
	{ X86_3DNOW_0F, EXT_SUFFIX(AMODE_P | OPTYPE_q, AMODE_Q | OPTYPE_q, 0) }, /* 0x0F */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movups", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x10 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movups", { AMODE_W | OPTYPE_ps | OP_DST, AMODE_V | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x11 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movlps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x12 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movlps", { AMODE_M | OPTYPE_q | OP_DST, AMODE_V | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x13 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "unpcklps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x14 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "unpckhps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x15 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movhps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x16 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movhps", { AMODE_M | OPTYPE_q | OP_DST, AMODE_V | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x17 */
	{ X86_Group_17, GROUP }, /* 0x18 */
	{ NOINSTR }, /* 0x19 */
	{ NOINSTR }, /* 0x1A */
	{ NOINSTR }, /* 0x1B */
	{ NOINSTR }, /* 0x1C */
	{ NOINSTR }, /* 0x1D */
	{ NOINSTR }, /* 0x1E */
	{ NOINSTR }, /* 0x1F */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_R | OPTYPE_dq | OP_DST, AMODE_C | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x20 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_R | OPTYPE_dq | OP_DST, AMODE_D | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x21 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_C | OPTYPE_dq | OP_DST, AMODE_R | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x22 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_D | OPTYPE_dq | OP_DST, AMODE_R | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x23 */
	{ NOINSTR }, /* 0x24 */ 
	//{ NOGROUP, CPU_I486, ITYPE_MOV, "mov", { AMODE_R | OPTYPE_d | OP_DST, AMODE_T | OPTYPE_d | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x24 */
	{ NOINSTR }, /* 0x25 */
	{ NOINSTR }, /* 0x26 */
	//{ NOGROUP, CPU_I486, ITYPE_MOV, "mov", { AMODE_T | OPTYPE_d | OP_DST, AMODE_R | OPTYPE_d | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x26 */
	{ NOINSTR }, /* 0x27 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movaps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x28 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movaps", { AMODE_W | OPTYPE_ps | OP_DST, AMODE_V | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x29 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "cvtpi2ps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x2A */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movntps", { AMODE_M | OPTYPE_o | OP_DST, AMODE_V | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x2B */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "cvttps2pi", { AMODE_P | OPTYPE_q | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x2C */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "cvtps2pi", { AMODE_P | OPTYPE_q | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x2D */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_CMP, "ucomiss", { AMODE_V | OPTYPE_ss | OP_SRC, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_CF_MOD | FLAG_PF_MOD | FLAG_OF_CLR | FLAG_SF_CLR | FLAG_AF_CLR, NOACTION, IGNORED }, /* 0x2E */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_CMP, "comiss", { AMODE_V | OPTYPE_ps | OP_SRC, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_CF_MOD | FLAG_PF_MOD | FLAG_OF_CLR | FLAG_SF_CLR | FLAG_AF_CLR, NOACTION, IGNORED }, /* 0x2F */
	{ NOGROUP, CPU_PENTIUM, ITYPE_SYSTEM, "wrmsr", { OPTYPE_REG_ECX | OP_MSR | OP_DST, OPTYPE_EDX_HI_EAX_LO | OP_SRC, 0 } , NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x30 */
	{ NOGROUP, CPU_PENTIUM, ITYPE_SYSTEM, "rdtsc", { OPTYPE_EDX_HI_EAX_LO | OP_DST, OPTYPE_TSC | OP_MSR | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x31 */
	{ NOGROUP, CPU_PENTIUM, ITYPE_SYSTEM, "rdmsr", { OPTYPE_EDX_HI_EAX_LO | OP_DST, OPTYPE_REG_ECX | OP_MSR | OP_SRC, 0 } , NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x32 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_OTHER, "rdpmc", { OPTYPE_EDX_HI_EAX_LO | OP_DST, OPTYPE_REG_ECX | OP_SYS | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x33 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSCALL, "sysenter", { OPTYPE_CS_MSR | OP_MSR | OP_SRC, OPTYPE_EIP_MSR | OP_MSR | OP_SRC, OPTYPE_ESP_MSR | OP_MSR | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x34 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSCALLRET, "sysexit", { OPTYPE_CS_MSR | OP_MSR | OP_SRC, OPTYPE_EIP_MSR | OP_MSR | OP_SRC, OPTYPE_ESP_MSR | OP_MSR | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x35 */
	{ NOINSTR }, /* 0x36 */
	{ NOINSTR }, /* 0x37 */
	{ NOINSTR }, /* 0x38 */
	{ NOINSTR }, /* 0x39 */
	{ NOINSTR }, /* 0x3A */
	{ NOINSTR }, /* 0x3B */
	{ NOINSTR }, /* 0x3C */
	{ NOINSTR }, /* 0x3D */
	{ NOINSTR }, /* 0x3E */
	{ NOINSTR }, /* 0x3F */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovo", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_O, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x40 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovno", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_NO, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x41 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovc", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_C, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x42 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovnc", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_NC, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x43 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovz", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_Z, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x44 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovnz", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_NZ, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x45 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovbe", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_BE, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x46 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmova", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_A, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x47 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovs", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_S, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x48 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovns", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_NS, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x49 */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovpe", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_PE, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x4A */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovpo", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_PO, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x4B */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovl", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_L, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x4C */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovge", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_GE, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x4D */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovle", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_LE, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x4E */
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_MOVCC, "cmovg", { AMODE_G | OPTYPE_v | OP_COND_DST, AMODE_E | OPTYPE_v | OP_COND_SRC, 0 }, COND_G, NOCHANGE, OP1_DST | OP2_SRC, NOACTION }, /* 0x4F */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "movmskps", { AMODE_G | OPTYPE_d | OP_DST, AMODE_VR | OPTYPE_ps| OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x50 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "sqrtps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x51 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "rsqrtps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x52 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "rcpps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x53 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_AND, "andps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x54 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_AND, "andnps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x55 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_OR, "orps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x56 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_XOR, "xorps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x57 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_ADD, "addps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x58 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MUL, "mulps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x59 */
	{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtps2pd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5A */
	{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtdq2ps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5B */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_SUB, "subps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5C */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "minps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5D */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_DIV, "divps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5E */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "maxps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x5F */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "punpcklbw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_d | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x60 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "punpcklwd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_d | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x61 */	
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "punpckldq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_d | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x62 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "packsswb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x63 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_CMP, "pcmpgtb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x64 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_CMP, "pcmpgtw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x65 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_CMP, "pcmpgtd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x66 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "packuswb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x67 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "punpckhbw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_d | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x68 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "punpckhwd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_d | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x69 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "punpckhdq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_d | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x6A */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "packssdw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x6B */
	{ NOINSTR }, /* 0x6C */
	{ NOINSTR }, /* 0x6D */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_MOV, "movd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_E | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x6E */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_MOV, "movq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x6F */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "pshufw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x70 */
	{ X86_Group_13, GROUP }, /* 0x71 */
	{ X86_Group_14, GROUP }, /* 0x72 */
	{ X86_Group_15, GROUP }, /* 0x73 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_CMP, "pcmpeqb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x74 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_CMP, "pcmpeqw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x75 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_CMP, "pcmpeqd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x76 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "emms", { OPTYPE_FPU_TAG | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x77 */
	{ NOINSTR }, /* 0x78 */
	{ NOINSTR }, /* 0x79 */
	{ NOINSTR }, /* 0x7A */
	{ NOINSTR }, /* 0x7B */
	{ NOINSTR }, /* 0x7C */
	{ NOINSTR }, /* 0x7D */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_MOV, "movd", { AMODE_E | OPTYPE_dq | OP_DST, AMODE_P | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x7E */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_MOV, "movq", { AMODE_Q | OPTYPE_q | OP_DST, AMODE_P | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x7F */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jo", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_O, NOCHANGE, OP1_DST, NOACTION }, /* 0x80 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jno", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_NO, NOCHANGE, OP1_DST, NOACTION }, /* 0x81 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jb", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_C, NOCHANGE, OP1_DST, NOACTION }, /* 0x82 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jnb", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_NC, NOCHANGE, OP1_DST, NOACTION }, /* 0x83 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jz", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_Z, NOCHANGE, OP1_DST, NOACTION }, /* 0x84 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jnz", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_NZ, NOCHANGE, OP1_DST, NOACTION }, /* 0x85 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jbe", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_BE, NOCHANGE, OP1_DST, NOACTION }, /* 0x86 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "ja", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_A, NOCHANGE, OP1_DST, NOACTION }, /* 0x87 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "js", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_S, NOCHANGE, OP1_DST, NOACTION }, /* 0x88 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jns", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_NS, NOCHANGE, OP1_DST, NOACTION }, /* 0x89 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jpe", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_PE, NOCHANGE, OP1_DST, NOACTION }, /* 0x8A */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jpo", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_PO, NOCHANGE, OP1_DST, NOACTION }, /* 0x8B */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jl", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_L, NOCHANGE, OP1_DST, NOACTION }, /* 0x8C */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jge", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_GE, NOCHANGE, OP1_DST, NOACTION }, /* 0x8D */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jle", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_LE, NOCHANGE, OP1_DST, NOACTION }, /* 0x8E */
	{ NOGROUP, CPU_I386, ITYPE_BRANCHCC, "jg", { AMODE_J | OPTYPE_z | OP_SRC | OP_COND_EXEC, 0, 0 }, COND_G, NOCHANGE, OP1_DST, NOACTION }, /* 0x8F */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "seto", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_O, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x90 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setno", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_NO, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x91 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setb", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_C, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x92 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setnb", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_NC, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x93 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "sete", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_Z, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x94 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setne", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_NZ, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x95 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setbe", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_BE, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x96 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "seta", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_A, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x97 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "sets", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_S, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x98 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setns", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_NS, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x99 */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setpe", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_PE, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x9A */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setpo", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_PO, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x9B */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setl", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_L, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x9C */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setge", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_GE, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x9D */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setle", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_LE, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x9E */
	{ NOGROUP, CPU_I386, ITYPE_MOVCC, "setg", { AMODE_E | OPTYPE_b | OP_COND_DST, OPTYPE_1 | OP_COND_SRC, OPTYPE_0 | OP_COND_SRC }, COND_G, NOCHANGE, OP1_DST | OP2_SRC, OP1_DST | OP3_SRC }, /* 0x9F */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OPTYPE_FS | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xA0 */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OPTYPE_FS | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xA1 */
	{ NOGROUP, CPU_PENTIUM, ITYPE_CPUID, "cpuid", { OPTYPE_EDX_ECX_EBX_EAX | OP_DST, OPTYPE_REG_xAX_BIG | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xA2 */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "bt", { AMODE_E | OPTYPE_v | OP_SRC, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0xA3 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "shld", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0xA4 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "shld", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, OPTYPE_REG_CL | OP_SRC }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0xA5 */
	{ NOINSTR }, /* 0xA6 */
	{ NOINSTR }, /* 0xA7 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { OPTYPE_GS | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xA8 */
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { OPTYPE_GS | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xA9 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "rsm", NOARGS, NOCOND, FLAG_ALL_MOD, NOACTION, IGNORED }, /* 0xAA */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "bts", { AMODE_E | OPTYPE_v | OP_SRC, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0xAB */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "shrd", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0xAC */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "shrd", { AMODE_E | OPTYPE_v | OP_DST, AMODE_G | OPTYPE_v | OP_SRC, OPTYPE_REG_CL | OP_SRC }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0xAD */
	{ X86_Group_16, GROUP }, /* 0xAE */
	{ NOGROUP, CPU_I386, ITYPE_MUL, "imul", { AMODE_G | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_v | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0xAF */
	{ NOGROUP, CPU_I486, ITYPE_XCHGCC, "cmpxchg", { AMODE_E | OPTYPE_b | OP_SRC | OP_COND_DST, OPTYPE_REG_AL | OP_SRC | OP_COND_DST, AMODE_G | OPTYPE_b | OP_COND_SRC }, COND_OP1_EQ_OP2, FLAG_COMMON_MOD, OP1_DST | OP3_SRC, OP2_DST | OP1_SRC }, /* 0xB0 */
	{ NOGROUP, CPU_I486, ITYPE_XCHGCC, "cmpxchg", { AMODE_E | OPTYPE_v | OP_SRC | OP_COND_DST, OPTYPE_REG_xAX_BIG | OP_SRC | OP_COND_DST, AMODE_G | OPTYPE_v | OP_COND_SRC }, COND_OP1_EQ_OP2, FLAG_COMMON_MOD, OP1_DST | OP3_SRC, OP2_DST | OP1_SRC }, /* 0xB1 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "lss", { AMODE_G | OPTYPE_z | OP_DST, AMODE_M | OPTYPE_p | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB2 */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "btr", { AMODE_E | OPTYPE_v | OP_SRC, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0xB3 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "lfs", { AMODE_G | OPTYPE_z | OP_DST, AMODE_M | OPTYPE_p | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB4 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "lgs", { AMODE_G | OPTYPE_z | OP_DST, AMODE_M | OPTYPE_p | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB5 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "movzx", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB6 */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "movzx", { AMODE_G | OPTYPE_v | OP_DST, AMODE_E | OPTYPE_w | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xB7 */
	{ NOINSTR }, /* 0xB8 */
	{ X86_Group_11, GROUP }, /* 0xB9 */
	{ X86_Group_8, GROUP }, /* 0xBA */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "btc", { AMODE_E | OPTYPE_v | OP_SRC, AMODE_G | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0xBB */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "bsf", { AMODE_G | OPTYPE_v | OP_SRC, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD, NOACTION, IGNORED }, /* 0xBC */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "bsr", { AMODE_G | OPTYPE_v | OP_SRC, AMODE_E | OPTYPE_v | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD, NOACTION, IGNORED }, /* 0xBD */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "movsx", { AMODE_G | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xBE */
	{ NOGROUP, CPU_I386, ITYPE_MOV, "movsx", { AMODE_G | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_w | OP_SIGNED | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xBF */
	{ NOGROUP, CPU_I486, ITYPE_XCHGADD, "xadd", { AMODE_E | OPTYPE_b | OP_SRC | OP_DST, AMODE_G | OPTYPE_b | OP_SRC | OP_DST, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0xC0 */
	{ NOGROUP, CPU_I486, ITYPE_XCHGADD, "xadd", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, AMODE_G | OPTYPE_v | OP_SRC | OP_DST, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0xC1 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_CMP, "cmpps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0xC2 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movnti", { AMODE_M | OPTYPE_dq | OP_DST, AMODE_G | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC3 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "pinsrw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_E | OPTYPE_w | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC4 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "pextrw", { AMODE_G | OPTYPE_d | OP_DST, AMODE_PR | OPTYPE_q | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC5 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "shufps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC6 */
	{ X86_Group_9, GROUP }, /* 0xC7 */
	{ NOGROUP, CPU_I486, ITYPE_XCHG, "bswap", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC8 */
	{ NOGROUP, CPU_I486, ITYPE_XCHG, "bswap", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC9 */
	{ NOGROUP, CPU_I486, ITYPE_XCHG, "bswap", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCA */
	{ NOGROUP, CPU_I486, ITYPE_XCHG, "bswap", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCB */
	{ NOGROUP, CPU_I486, ITYPE_XCHG, "bswap", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCC */
	{ NOGROUP, CPU_I486, ITYPE_XCHG, "bswap", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCD */
	{ NOGROUP, CPU_I486, ITYPE_XCHG, "bswap", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCE */
	{ NOGROUP, CPU_I486, ITYPE_XCHG, "bswap", { OP_REG | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xCF */
	{ NOINSTR }, /* 0xD0 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psrlw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xD1 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psrld", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xD2 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psrlq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xD3 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_ADD, "paddq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xD4 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_MUL, "pmullw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xD5 */
	{ NOINSTR }, /* 0xD6 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "pmovmskb", { AMODE_G | OPTYPE_d | OP_DST, AMODE_PR | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xD7 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_SUB, "psubusb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xD8 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_SUB, "psubusw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xD9 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "pminub", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xDA */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_AND, "pand", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xDB */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_ADD, "paddusb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xDC */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_ADD, "paddusw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xDD */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "pmaxub", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xDE */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_AND, "pandn", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xDF */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "pavgb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE0 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psraw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE1 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psrad", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE2 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "pavgw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE3 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MUL, "pmulhuw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE4 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_MUL, "pmulhw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE5 */
	{ NOINSTR }, /* 0xE6 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MOV, "movntq", { AMODE_M | OPTYPE_q | OP_DST, AMODE_P | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE7 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_SUB, "psubsb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE8 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_SUB, "psubsw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xE9 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "pminsw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEA */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_OR, "por", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEB */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_ADD, "paddsb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEC */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_ADD, "paddsw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xED */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "pmaxsw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEE */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_XOR, "pxor", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xEF */
	{ NOINSTR }, /* 0xF0 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psllw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF1 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "pslld", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF2 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psllq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF3 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_MUL, "pmuludq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF4 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_ADD, "pmaddwd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF5 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "psadbw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF6 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "maskmovq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_PR | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF7 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_SUB, "psubb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF8 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_SUB, "psubw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xF9 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_SUB, "psubd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xFA */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE_SUB, "psubq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xFB */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_ADD, "paddb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xFC */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_ADD, "paddw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xFD */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX_ADD, "paddd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xFE */
	{ NOINSTR } /* 0xFF */, 
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// Groups
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

X86_OPCODE X86_Group_1_80[8] = // 80
{
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { AMODE_E | OPTYPE_b | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_1_81[8] = // 81
{
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { AMODE_E | OPTYPE_v | OP_SRC, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_1_82[8] = // 82
{
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_SRC, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_1_83[8] = // 83
{
	{ NOGROUP, CPU_I386, ITYPE_ADD, "add", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_OR, "or", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ADD, "adc", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sbb", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_AND, "and", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SUB, "sub", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_XOR, "xor", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_CMP, "cmp", { AMODE_E | OPTYPE_v | OP_SIGNED | OP_SRC, AMODE_I | OPTYPE_b | OP_SIGNED | OP_SRC, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_2_C0[8] = // C0
{
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rol", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "ror", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rcl", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "rcr", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "shl", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "shr", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "sal", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "sar", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_2_C1[8] = // C1
{
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rol", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "ror", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rcl", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "rcr", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "shl", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "shr", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "sal", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "sar", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_2_D0[8] = // D0
{
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rol", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "ror", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rcl", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "rcr", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "shl", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "shr", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "sal", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "sar", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_2_D1[8] = // D1
{
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rol", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "ror", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rcl", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "rcr", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "shl", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "shr", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "sal", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "sar", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_1 | OP_SRC, 0 }, NOCOND, FLAG_OF_MOD | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_2_D2[8] = // D2
{
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rol", { AMODE_E | OPTYPE_b | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "ror", { AMODE_E | OPTYPE_b | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rcl", { AMODE_E | OPTYPE_b | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "rcr", { AMODE_E | OPTYPE_b | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "shl", { AMODE_E | OPTYPE_b | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "shr", { AMODE_E | OPTYPE_b | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "sal", { AMODE_E | OPTYPE_b | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "sar", { AMODE_E | OPTYPE_b | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_2_D3[8] = // D3
{
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rol", { AMODE_E | OPTYPE_v | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "ror", { AMODE_E | OPTYPE_v | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_ROL, "rcl", { AMODE_E | OPTYPE_v | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_ROR, "rcr", { AMODE_E | OPTYPE_v | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "shl", { AMODE_E | OPTYPE_v | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "shr", { AMODE_E | OPTYPE_v | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_SHL, "sal", { AMODE_E | OPTYPE_v | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_SHR, "sar", { AMODE_E | OPTYPE_v | OP_DST, OPTYPE_REG_CL | OP_SRC, 0 }, NOCOND, FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_3_F6[8] = // F6
{
	{ NOGROUP, CPU_I386, ITYPE_TEST, "test", { AMODE_E | OPTYPE_b | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_TEST, "test", { AMODE_E | OPTYPE_b | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_NOT, "not", { AMODE_E | OPTYPE_b | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_NEG, "neg", { AMODE_E | OPTYPE_b | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_MUL, "mul", { OPTYPE_REG_AX | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, OPTYPE_REG_AL | OP_SRC }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_MUL, "imul", { OPTYPE_REG_AX | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_b | OP_SIGNED | OP_SRC, OPTYPE_REG_AL | OP_SIGNED | OP_SRC }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_DIV, "div", { OPTYPE_REG_AX | OP_DST, AMODE_E | OPTYPE_b | OP_SRC, OPTYPE_REG_AX | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_DIV, "idiv", { OPTYPE_REG_AX | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_b | OP_SIGNED | OP_SRC, OPTYPE_REG_AX | OP_SIGNED | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED } /* 0x07 */,
};

X86_OPCODE X86_Group_3_F7[8] = // F7
{
	{ NOGROUP, CPU_I386, ITYPE_TEST, "test", { AMODE_E | OPTYPE_v | OP_SRC, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_TEST, "test", { AMODE_E | OPTYPE_v | OP_SRC, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, FLAG_OF_CLR | FLAG_SF_MOD | FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_CLR, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_NOT, "not", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_NEG, "neg", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_MUL, "mul", { OPTYPE_xDX_HI_xAX_LO | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, OPTYPE_REG_xAX_BIG | OP_SRC }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_MUL, "imul", { OPTYPE_xDX_HI_xAX_LO | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_v | OP_SIGNED | OP_SRC, OPTYPE_REG_xAX_BIG | OP_SRC }, NOCOND, FLAG_OF_MOD | FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_DIV, "div", { OPTYPE_xDX_HI_xAX_LO | OP_DST, AMODE_E | OPTYPE_v | OP_SRC, OPTYPE_REG_xAX_BIG | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_DIV, "idiv", { OPTYPE_xDX_HI_xAX_LO | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_v | OP_SIGNED | OP_SRC, OPTYPE_REG_xAX_BIG | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_4[8] = // FE
{
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { AMODE_E | OPTYPE_b | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { AMODE_E | OPTYPE_b | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x01 */
	{ NOINSTR }, /* 0x03 */
	{ NOINSTR }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOINSTR }, /* 0x06 */
	{ NOINSTR } /* 0x07 */
};

X86_OPCODE X86_Group_5[8] = // FF
{
	{ NOGROUP, CPU_I386, ITYPE_INC, "inc", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_DEC, "dec", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, 0, 0 }, NOCOND, FLAG_OF_MOD|FLAG_SF_MOD|FLAG_ZF_MOD|FLAG_AF_MOD|FLAG_PF_MOD, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_CALL, "call", { AMODE_E | OPTYPE_v | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_CALL, "call", { AMODE_E | OPTYPE_p | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCH, "jmp", { AMODE_E | OPTYPE_v | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_BRANCH, "jmp", { AMODE_E | OPTYPE_p | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_PUSH, "push", { AMODE_E | OPTYPE_v | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOINSTR }, /* 0x07 */
};

X86_OPCODE X86_Group_6[8] = // 0F 00
{
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "sldt", { AMODE_E | OPTYPE_mw | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "str", { AMODE_E | OPTYPE_mw | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "lldt", { AMODE_E | OPTYPE_w | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "ltr", { AMODE_E | OPTYPE_w | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "verr", { AMODE_E | OPTYPE_w | OP_SRC, 0, 0 }, NOCOND, FLAG_ZF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "verw", { AMODE_E | OPTYPE_w | OP_SRC, 0, 0 }, NOCOND, FLAG_ZF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_IA64, ITYPE_BRANCH, "jmpe", { AMODE_E | OPTYPE_v | OP_SRC | OP_EXEC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOINSTR } /* 0x07 */
};

X86_OPCODE X86_Group_7[8] = // 0F 01
{
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "sgdt", { AMODE_M | OPTYPE_dt | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "sidt", { AMODE_M | OPTYPE_dt | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "lgdt", { AMODE_M | OPTYPE_dt | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "lidt", { AMODE_M | OPTYPE_dt | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "smsw", { AMODE_E | OPTYPE_mw | OP_DST, OPTYPE_CR0 | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "lmsw", { OPTYPE_CR0 | OP_DST, AMODE_E | OPTYPE_w | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I486, ITYPE_SYSTEM, "invlpg", { AMODE_M | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_8[8] = // 0F BA
{
	{ NOINSTR }, /* 0x00 */
	{ NOINSTR }, /* 0x01 */
	{ NOINSTR }, /* 0x02 */
	{ NOINSTR }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "bt", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "bts", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "btr", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_BITTEST, "btc", { AMODE_E | OPTYPE_v | OP_SRC | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, FLAG_CF_MOD, NOACTION, IGNORED }, /* 0x07 */ 
};

X86_OPCODE X86_Group_9[8] = // 0F C7
{
	{ NOINSTR }, /* 0x00 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_XCHGCC, "cmpxchg8b", { AMODE_M | OPTYPE_q | OP_SRC | OP_COND_DST, OPTYPE_xDX_HI_xAX_LO | OP_SRC | OP_COND_DST, OPTYPE_xCX_HI_xBX_LO | OP_COND_SRC }, COND_OP1_EQ_OP2, FLAG_ZF_MOD, OP1_DST | OP3_SRC, OP2_DST | OP1_SRC }, /* 0x02 */
	{ NOINSTR }, /* 0x03 */
	{ NOINSTR }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOINSTR }, /* 0x06 */
	{ NOINSTR }, /* 0x07 */
};

X86_OPCODE X86_Group_10[8] = // 8F (NOTE: AMD64 labels this Group 1A)
{
	{ NOGROUP, CPU_I386, ITYPE_POP, "pop", { AMODE_E | OPTYPE_v | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x00 */
	{ NOINSTR }, /* 0x01 */
	{ NOINSTR }, /* 0x02 */
	{ NOINSTR }, /* 0x03 */
	{ NOINSTR }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOINSTR }, /* 0x06 */
	{ NOINSTR }, /* 0x07 */
};

X86_OPCODE X86_Group_11[8] = // 0F B9 (NOTE: AMD64 labels this Group 10)
{
	{ NOGROUP, CPU_I386, ITYPE_INVALID, "undef", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_I386, ITYPE_INVALID, "undef", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_I386, ITYPE_INVALID, "undef", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_I386, ITYPE_INVALID, "undef", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_I386, ITYPE_INVALID, "undef", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_I386, ITYPE_INVALID, "undef", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_I386, ITYPE_INVALID, "undef", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_I386, ITYPE_INVALID, "undef", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED } /* 0x07 */
};

X86_OPCODE X86_Group_12_C6[8] = // C6 (NOTE: AMD64 labels this Group 11)
{
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_E | OPTYPE_b | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0xC6 */
	{ NOINSTR }, /* 0x01 */
	{ NOINSTR }, /* 0x02 */
	{ NOINSTR }, /* 0x03 */
	{ NOINSTR }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOINSTR }, /* 0x06 */
	{ NOINSTR }, /* 0x07 */
};

X86_OPCODE X86_Group_12_C7[8] = // C7 (NOTE: AMD64 labels this Group 11)
{
	{ NOGROUP, CPU_I386, ITYPE_MOV, "mov", { AMODE_E | OPTYPE_v | OP_DST, AMODE_I | OPTYPE_z | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x00 */
	{ NOINSTR }, /* 0x01 */
	{ NOINSTR }, /* 0x02 */
	{ NOINSTR }, /* 0x03 */
	{ NOINSTR }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOINSTR }, /* 0x06 */
	{ NOINSTR }, /* 0x07 */
};

// NOTE: the X86_SSE2_* is only followed if it is a 3-byte opcode (e.g., prefix is 66, F2, or F3)
X86_OPCODE X86_Group_13[8] = // 0F 71 (NOTE: AMD64 labels this Group 12)
{
	{ NOINSTR }, /* 0x00 */
	{ NOINSTR }, /* 0x01 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psrlw", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOINSTR }, /* 0x03 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psraw", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psllw", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOINSTR }, /* 0x07 */
};

X86_OPCODE X86_Group_14[8] = // 0F 72 (NOTE: AMD64 labels this Group 13)
{
	{ NOINSTR }, /* 0x00 */
	{ NOINSTR }, /* 0x01 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psrld", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOINSTR }, /* 0x03 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psrad", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "pslld", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOINSTR }, /* 0x07 */
};

X86_OPCODE X86_Group_15[8] = // 0F 73 (NOTE: AMD64 labels this Group 14)
{
	{ NOINSTR }, /* 0x00 */
	{ NOINSTR }, /* 0x01 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psrlq", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psrldq", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x03 */
	{ NOINSTR }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "psllq", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */ 
	{ NOGROUP, CPU_PENTIUM2, ITYPE_MMX, "pslldq", { AMODE_PR | OPTYPE_q | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED } /* 0x07 */ 
};

X86_OPCODE X86_Group_16[8] = // 0F AE (NOTE: AMD64 labels this Group 15)
{
	{ NOGROUP, CPU_PENTIUM2, ITYPE_FPU, "fxsave", { AMODE_M | OPTYPE_fst2 | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_FPU, "fxrstor", { AMODE_M | OPTYPE_fst2 | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "ldmxcsr", { AMODE_M | OPTYPE_d | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "stmxcsr", { AMODE_M | OPTYPE_d | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x03 */
	{ NOINSTR }, /* 0x04 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "lfence", NOARGS, NOCOND, NOCHANGE, SERIALIZE_READ, IGNORED }, /* 0x05 */ 
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "mfence", NOARGS, NOCOND, NOCHANGE, SERIALIZE_ALL, IGNORED }, /* 0x06 */ 
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "sfence", NOARGS, NOCOND, NOCHANGE, SERIALIZE_WRITE, IGNORED } /* 0x07 */
};

X86_OPCODE X86_Group_17[8] = // 0F 18 (NOTE: AMD64 labels this Group 16)
{
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "prefetchnta", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x00 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "prefetcht0", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "prefetcht1", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "prefetcht2", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x03 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "hintnop", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x04 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "hintnop", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x05 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "hintnop", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
	{ NOGROUP, CPU_PENTIUM2, ITYPE_SYSTEM, "hintnop", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x07 */
};

X86_OPCODE X86_Group_P[8] = // 0F 0D
{
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "prefetch", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x01 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "prefetchw", { AMODE_E | OPTYPE_b | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
	{ NOINSTR }, /* 0x03 */
	{ NOINSTR }, /* 0x04 */
	{ NOINSTR }, /* 0x05 */
	{ NOINSTR }, /* 0x06 */
	{ NOINSTR }, /* 0x07 */
};


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// FPU (ESC) opcodes
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


X86_OPCODE X86_ESC_0[0x48] = // D8 
{
	//
	// ModRM < C0
	// Index 0x00-0x07 = opcode extension
	//

	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_ss | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x00
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_ss | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x01
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { AMODE_M | OPTYPE_ss | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // 0x02
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { AMODE_M | OPTYPE_ss | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // 0x03
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_ss | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x04
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_ss | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x05
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_ss | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x06
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_ss | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x07

	//
	// ModRM >= C0: ST(0) <- ST(0) + ST(i)
	// Index 0x08-0x47 = ModRM 0xC0-0xFF
	//

	// C0-C7
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x7
	// C8-CF
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	// D0-D7
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x7
	// D8-DF
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xF
	// E0-E7
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x7
	// E8-EF
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xF
	// F0-F7
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x7
	// F8-FF
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }  // xF
};


X86_OPCODE X86_ESC_1[0x48] = // D9
{
	//
	// ModRM < C0
	// Index 0x00-0x07 = opcode extension
	//

	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { AMODE_M | OPTYPE_ss | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // 0x00
	{ NOINSTR }, // 0x01
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { AMODE_M | OPTYPE_ss | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x02
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { AMODE_M | OPTYPE_ss | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // 0x03
	{ NOGROUP, CPU_I287, ITYPE_FLOADENV, "fldenv", { AMODE_M | OPTYPE_fev | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // 0x04
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fldcw", { OPTYPE_FPU_CONTROL | OP_DST, AMODE_M | OPTYPE_w | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // 0x05
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstenv", { AMODE_M | OPTYPE_fev | OP_DST, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // 0x06
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fnstcw", { AMODE_M | OPTYPE_w | OP_DST, OPTYPE_FPU_CONTROL | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // 0x07

	//
	// ModRM >= C0
	// Index 0x08-0x47 = ModRM 0xC0-0xFF
	//

	// C0-C7
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { OPTYPE_STx | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { OPTYPE_STx | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { OPTYPE_STx | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { OPTYPE_STx | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { OPTYPE_STx | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { OPTYPE_STx | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { OPTYPE_STx | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { OPTYPE_STx | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x7
	// C8-CF
	{ NOGROUP, CPU_I287, ITYPE_FEXCH, "fxch", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC | OP_DST, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FEXCH, "fxch", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC | OP_DST, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FEXCH, "fxch", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC | OP_DST, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FEXCH, "fxch", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC | OP_DST, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FEXCH, "fxch", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC | OP_DST, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FEXCH, "fxch", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC | OP_DST, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FEXCH, "fxch", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC | OP_DST, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FEXCH, "fxch", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_STx | OP_SRC | OP_DST, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xF
	// D0-D7
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fnop", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	{ NOINSTR }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// D8-DF
	{ NOINSTR }, // x8
	{ NOINSTR }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }, // xF
	// E0-E7
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fchs", { OPTYPE_ST0 | OP_SRC | OP_DST, 0, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fabs", { OPTYPE_ST0 | OP_SRC | OP_DST, 0, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3 
	{ NOGROUP, CPU_I287, ITYPE_FPU, "ftst", { OPTYPE_ST0 | OP_SRC, OPTYPE_FLDZ | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fxam", { OPTYPE_ST0 | OP_SRC, 0, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// E8-EF
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld1", { OPTYPE_FLD1 | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fldl2t", { OPTYPE_FLDL2T | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fldl2e", { OPTYPE_FLDL2E | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fldpi", { OPTYPE_FLDPI | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fldlg2", { OPTYPE_FLDLG2| OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fldln2", { OPTYPE_FLDLN2 | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fldz", { OPTYPE_FLDZ | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // xE
	{ NOINSTR }, // xF
	// F0-F7
	{ NOGROUP, CPU_I287, ITYPE_FPU, "f2xm1", { OPTYPE_ST0 | OP_SRC | OP_DST, 0, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fyl2x", { OPTYPE_ST1 | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fptan", { OPTYPE_ST0 | OP_SRC | OP_DST, 0, 0 }, NOCOND, FPU_C1_MOD|FPU_C2_MOD, FPU_STACK_PUSH, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fpatan", { OPTYPE_ST1 | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC , 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fxtract", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_ST1 | OP_DST, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x4
	{ NOGROUP, CPU_I387, ITYPE_FPU, "fprem1", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_ST1 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fdecstp", NOARGS, NOCOND, FPU_C1_MOD, FPU_STACK_DEC, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fincstp", NOARGS, NOCOND, FPU_C1_MOD, FPU_STACK_INC, IGNORED }, // x7
	// F8-FF
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fprem", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_ST1 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fyl2xp1", { OPTYPE_ST1 | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fsqrt", { OPTYPE_ST0 | OP_SRC | OP_DST, 0, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fsincos", { OPTYPE_ST0 | OP_SRC | OP_DST, 0, 0 }, NOCOND, FPU_C1_MOD|FPU_C2_MOD, FPU_STACK_PUSH, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FPU, "frndint", { OPTYPE_ST0 | OP_SRC | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fscale", { OPTYPE_ST0 | OP_SRC | OP_DST, OPTYPE_ST1 | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fsin", { OPTYPE_ST0 | OP_SRC | OP_DST, 0, 0 }, NOCOND, FPU_C1_MOD|FPU_C2_MOD, NOACTION, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fcos", { OPTYPE_ST0 | OP_SRC | OP_DST, 0, 0 }, NOCOND, FPU_C1_MOD|FPU_C2_MOD, NOACTION, IGNORED } // xF
};


X86_OPCODE X86_ESC_2[0x48] = // DA
{
	//
	// ModRM < C0
	// Index 0x00-0x07 = opcode extension
	//

	{ NOGROUP, CPU_I287, ITYPE_FADD, "fiadd", { OPTYPE_ST0 | OP_SIGNED | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fimul", { OPTYPE_ST0 | OP_SIGNED | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "ficom", { AMODE_M | OPTYPE_sd | OP_SIGNED | OP_SRC, OPTYPE_ST0 | OP_SRC | OP_DST, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "ficomp", { AMODE_M | OPTYPE_sd | OP_SIGNED | OP_SRC, OPTYPE_ST0 | OP_SRC | OP_DST, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fisub", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fisubr", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fidiv", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fidivr", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x7

	//
	// ModRM >= C0
	// Index 0x08-0x47 = ModRM 0xC0-0xFF
	//

	// C0-C7
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_B, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x0
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_B, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x1
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_B, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x2
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_B, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x3
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_B, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x4
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_B, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x5
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_B, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x6
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_B, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x7
	// C8-CF
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmove", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_E, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x8
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmove", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_E, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x9
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmove", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_E, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xA
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmove", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_E, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xB
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmove", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_E, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xC
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmove", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_E, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xD
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmove", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_E, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xE
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmove", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_E, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xF
	// D0-D7
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_BE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x0
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_BE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x1
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_BE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x2
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_BE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x3
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_BE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x4
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_BE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x5
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_BE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x6
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_BE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x7
	// D8-DF
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_U, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x8
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_U, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x9
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_U, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xA
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_U, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xB
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_U, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xC
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_U, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xD
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_U, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xE
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FMOVCC, "fcmovu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_COND_SRC, 0 }, COND_U, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xF
	// E0-E7
	{ NOINSTR }, // x0
	{ NOINSTR }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// E8-EF
	{ NOINSTR }, // x8
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucompp", { OPTYPE_ST1 | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP2, IGNORED }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }, // xF
	// F0-F7
	{ NOINSTR }, // x0
	{ NOINSTR }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// F8-FF
	{ NOINSTR }, // x8
	{ NOINSTR }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }  // xF
};



X86_OPCODE X86_ESC_3[0x48] = // DB
{
	//
	// ModRM < C0
	// Index 0x00-0x07 = opcode extension
	//

	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fild", { AMODE_M | OPTYPE_d | OP_SIGNED | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // 0x00
	{ NOGROUP, CPU_PENTIUM4, ITYPE_FSTORE, "fisttp", { AMODE_M | OPTYPE_d | OP_SIGNED | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // 0x01
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fist", { AMODE_M | OPTYPE_d | OP_SIGNED | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x02
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fistp", { AMODE_M | OPTYPE_d | OP_SIGNED | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // 0x03
	{ NOINSTR }, // 0x04
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { AMODE_M | OPTYPE_se | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // 0x05
	{ NOINSTR }, // 0x06
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { AMODE_M | OPTYPE_se | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // 0x07
	
	//
	// ModRM >= C0
	// Index 0x08-0x47 = ModRM 0xC0-0xFF
	//

	// C0-C7
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NB, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x0
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NB, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x1
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NB, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x2
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NB, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x3
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NB, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x4
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NB, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x5
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NB, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x6
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnb", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NB, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x7
	// C8-CF
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovne", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x8
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovne", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x9
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovne", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xA
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovne", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xB
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovne", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xC
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovne", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xD
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovne", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xE
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovne", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xF
	// D0-D7
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NBE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x0
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NBE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x1
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NBE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x2
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NBE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x3
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NBE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x4
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NBE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x5
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NBE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x6
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnbe", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NBE, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x7
	// D8-DF
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NU, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x8
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NU, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // x9
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NU, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xA
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NU, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xB
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NU, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xC
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NU, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xD
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NU, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xE
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FPU, "fcmovnu", { OPTYPE_ST0 | OP_COND_DST, OPTYPE_STx | OP_SRC, 0 }, COND_NU, FPU_C1_MOD, OP1_DST | OP2_SRC, NOACTION }, // xF
	// E0-E7
	{ NOINSTR }, // x0
	{ NOINSTR }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FPU, "fnclex", { OPTYPE_FPU_STATUS | OP_DST, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FPU, "finit", { OPTYPE_FPU_STATUS | OP_DST, 0, 0 }, NOCOND, FPU_ALL_CLR, NOACTION, IGNORED }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// E8-EF
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x8
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x9
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // xA
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // xB
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // xC
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // xD
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // xE
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // xF
	// F0-F7
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomi", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, NOACTION, IGNORED }, // x7
	// F8-FF
	{ NOINSTR }, // x8
	{ NOINSTR }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }  // xF
};

X86_OPCODE X86_ESC_4[0x48] = // DC
{
	//
	// ModRM < C0
	// Index 0x00-0x07 = opcode extension
	//

	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x00
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x01
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcom", { AMODE_M | OPTYPE_sd | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // 0x02
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcomp", { AMODE_M | OPTYPE_sd | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // 0x03
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x04
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x05
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x06
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_sd | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x07

	//
	// ModRM >= C0
	// Index 0x08-0x47 = ModRM 0xC0-0xFF
	//

	// C0-C7
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FADD, "fadd", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x7
	// C8-CF
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmul", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xF
	// D0-D7
	{ NOINSTR }, // x0
	{ NOINSTR }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// D8-DF
	{ NOINSTR }, // x8
	{ NOINSTR }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }, // xF
	// E0-E7
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x7
	// E8-EF
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsub", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xF
	// F0-F7
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivr", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x7
	// F8-FF
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdiv", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED } // xF
};


X86_OPCODE X86_ESC_5[0x48] = // DD
{
	//
	// ModRM < C0
	// Index 0x00-0x07 = opcode extension
	// 
	//

	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fld", { AMODE_M | OPTYPE_sd | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // x0
	{ NOGROUP, CPU_PENTIUM4, ITYPE_FSTORE, "fisttp", { AMODE_M | OPTYPE_d | OP_SIGNED | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { AMODE_M | OPTYPE_sd | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { AMODE_M | OPTYPE_sd | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FRESTORE, "frstor", { AMODE_M | OPTYPE_fst1 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x4
	{ NOINSTR }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FSAVE, "fsave", { AMODE_M | OPTYPE_fst1 | OP_DST, 0 }, NOCOND, FPU_ALL_CLR, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fnstsw", { AMODE_M | OPTYPE_w | OP_DST, OPTYPE_FPU_STATUS | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x7

	//
	// ModRM >= C0
	// Index 0x08-0x47 = ModRM 0xC0-0xFF
	//

	// C0-C7
	{ NOGROUP, CPU_I287, ITYPE_FPU, "ffree", { OPTYPE_FPU_TAG | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FPU, "ffree", { OPTYPE_FPU_TAG | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FPU, "ffree", { OPTYPE_FPU_TAG | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FPU, "ffree", { OPTYPE_FPU_TAG | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FPU, "ffree", { OPTYPE_FPU_TAG | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FPU, "ffree", { OPTYPE_FPU_TAG | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FPU, "ffree", { OPTYPE_FPU_TAG | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FPU, "ffree", { OPTYPE_FPU_TAG | OP_DST, OPTYPE_STx | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x7
	// C8-CF
	{ NOINSTR }, // x8
	{ NOINSTR }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }, // xF
	// D0-D7
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fst", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // x7
	// D8-DF
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fstp", { OPTYPE_STx | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xF
	// E0-E7
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x0
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x1
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x2
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x3
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x4
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x5
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x6
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucom", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // x7
	// E8-EF
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x8
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x9
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fucomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xB
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xC
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xD
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xE
	{ NOGROUP, CPU_I387, ITYPE_FCOMP, "fucomp", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xF
	// F0-F7
	{ NOINSTR }, // x0
	{ NOINSTR }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// F8-FF
	{ NOINSTR }, // x8
	{ NOINSTR }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }  // xF
};


X86_OPCODE X86_ESC_6[0x48] = // DE
{
	//
	// ModRM < C0
	// Index 0x00-0x07 = opcode extension
	//

	{ NOGROUP, CPU_I287, ITYPE_FADD, "fiadd", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_w | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x00
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fimul", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_w | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x01
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "ficom", { AMODE_M | OPTYPE_w | OP_SIGNED | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, NOACTION, IGNORED }, // 0x02
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "ficomp", { AMODE_M | OPTYPE_w | OP_SIGNED | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // 0x03
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fisub", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_w | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x04
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fisubr", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_w | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x05
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fidiv", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_w | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x06
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fidivr", { OPTYPE_ST0 | OP_SRC | OP_DST, AMODE_M | OPTYPE_w | OP_SIGNED | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x07
	//
	// ModRM >= C0
	// Index 0x08-0x47 = ModRM 0xC0-0xFF
	//

	// C0-C7
	{ NOGROUP, CPU_I287, ITYPE_FADD, "faddp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FADD, "faddp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FADD, "faddp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FADD, "faddp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FADD, "faddp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FADD, "faddp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FADD, "faddp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FADD, "faddp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x7
	// C8-CF
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmulp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmulp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmulp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmulp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmulp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmulp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmulp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FMUL, "fmulp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xF
	// D0-D7
	{ NOINSTR }, // x0
	{ NOINSTR }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// D8-DF
	{ NOINSTR }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FCOMP, "fcompp", { OPTYPE_ST1 | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_ALL_MOD, FPU_STACK_POP2, IGNORED }, // x9
	{ NOINSTR }, // 0xA
	{ NOINSTR }, // 0xB
	{ NOINSTR }, // 0xC
	{ NOINSTR }, // 0xD
	{ NOINSTR }, // 0xE
	{ NOINSTR }, // 0xF
	// E0-E7
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x7
	// E8-EF
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FSUB, "fsubp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xF
	// F0-F7
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x0
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x1
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x2
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x3
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x4
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x5
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x6
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivrp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x7
	// F8-FF
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x8
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // x9
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xA
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xB
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xC
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xD
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // xE
	{ NOGROUP, CPU_I287, ITYPE_FDIV, "fdivp", { OPTYPE_STx | OP_SRC | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }  // xF
};


X86_OPCODE X86_ESC_7[0x48] = // DF
{
	//
	// ModRM < C0
	// Index 0x00-0x07 = opcode extension
	//

	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fild", { AMODE_M | OPTYPE_w | OP_SIGNED | OP_SRC, 0, 0}, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // 0x00
	{ NOGROUP, CPU_PENTIUM4, ITYPE_FSTORE, "fisttp", { AMODE_M | OPTYPE_w | OP_SIGNED | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // 0x01
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fist", { AMODE_M | OPTYPE_w | OP_SIGNED | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, NOACTION, IGNORED }, // 0x02
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fistp", { AMODE_M | OPTYPE_w | OP_SIGNED | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // 0x03
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fbld", { AMODE_M | OPTYPE_pb | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // 0x04
	{ NOGROUP, CPU_I287, ITYPE_FLOAD, "fild", { AMODE_M | OPTYPE_q | OP_SIGNED | OP_SRC, 0, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_PUSH, IGNORED }, // 0x05
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fbstp", { AMODE_M | OPTYPE_pb | OP_DST, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // 0x06
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fistp", { AMODE_M | OPTYPE_q | OP_SIGNED | OP_DST, OPTYPE_ST0 | OP_SRC, 0}, NOCOND, FPU_C1_MOD, FPU_STACK_POP, IGNORED }, // 0x07

	//
	// ModRM >= C0
	// Index 0x08-0x47 = ModRM 0xC0-0xFF
	//
	// C0-C7
	{ NOINSTR }, // x0
	{ NOINSTR }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// C8-CF
	{ NOINSTR }, // x8
	{ NOINSTR }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }, // xF
	// D0-D7
	{ NOINSTR }, // x0
	{ NOINSTR }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// D8-DF
	{ NOINSTR }, // x8
	{ NOINSTR }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }, // xF
	// E0-E7
	{ NOGROUP, CPU_I287, ITYPE_FSTORE, "fnstsw", { OPTYPE_REG_AX | OP_DST, OPTYPE_FPU_STATUS | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	{ NOINSTR }, // x1
	{ NOINSTR }, // x2
	{ NOINSTR }, // x3
	{ NOINSTR }, // x4
	{ NOINSTR }, // x5
	{ NOINSTR }, // x6
	{ NOINSTR }, // x7
	// E8-EF
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x8
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x9
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xA
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xB
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xC
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xD
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xE
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fucomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // xF
	// F0-F7
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x0
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x1
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x2
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x3
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x4
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x5
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x6
	{ NOGROUP, CPU_PENTIUM_PRO, ITYPE_FCOMP, "fcomip", { OPTYPE_STx | OP_SRC, OPTYPE_ST0 | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_PF_MOD | FLAG_CF_MOD | FPU_ALL_MOD, FPU_STACK_POP, IGNORED }, // x7
	// F8-FF
	{ NOINSTR }, // x8
	{ NOINSTR }, // x9
	{ NOINSTR }, // xA
	{ NOINSTR }, // xB
	{ NOINSTR }, // xC
	{ NOINSTR }, // xD
	{ NOINSTR }, // xE
	{ NOINSTR }  // xF
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// SSE opcodes
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

X86_OPCODE X86_SSE[0x300] =
{
	// prefix 0x66 (operand size)
		/* 0x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 1x */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movupd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movupd", { AMODE_W | OPTYPE_pd | OP_DST, AMODE_V | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movlpd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_M | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movlpd", { AMODE_M | OPTYPE_q | OP_DST, AMODE_V | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x3
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "unpcklpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x4
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "unpckhpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movhpd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_M | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "mpvhpd", { AMODE_M | OPTYPE_q | OP_DST, AMODE_V | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 2x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movapd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movapd", { AMODE_W | OPTYPE_o | OP_DST, AMODE_V | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x9
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtpi2pd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movntpd", { AMODE_M | OPTYPE_o | OP_DST, AMODE_V | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvttpd2pi", { AMODE_P | OPTYPE_q | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtpd2pi", { AMODE_P | OPTYPE_q | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_CMP, "ucomisd", { AMODE_V | OPTYPE_sd | OP_SRC, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_CF_MOD | FLAG_PF_MOD | FLAG_OF_CLR | FLAG_SF_CLR | FLAG_AF_CLR, NOACTION, IGNORED }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_CMP, "comisd", { AMODE_V | OPTYPE_pd | OP_SRC, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD | FLAG_CF_MOD | FLAG_PF_MOD | FLAG_OF_CLR | FLAG_SF_CLR | FLAG_AF_CLR, NOACTION, IGNORED }, // xF
	
		/* 3x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 4x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 5x */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "movmskpd", { AMODE_G | OPTYPE_d | OP_DST, AMODE_VR | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "sqrtpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_AND, "andpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x4
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_AND, "andnpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_OR, "orpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_XOR, "xorpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x7
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "addpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MUL, "mulpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x9
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtpd2ps", { AMODE_V | OPTYPE_ps | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtps2dq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "subpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "minpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_DIV, "divpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "maxpd", { AMODE_V | OPTYPE_pd | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* 6x */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "punpcklbw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "punpcklwd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "punpckldq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "packsswb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x3
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pcmpgtb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x4
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pcmpgtw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pcmpgtd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "packuswb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x7
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "punpckhbw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "punpckhwd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x9
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "punpckhdq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "packssdw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "punpcklqdq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "punpckhqdq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_E | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movdqa", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* 7x */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pshufd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { X86_SSE2_Group_13, GROUP }, // x1
	  { X86_SSE2_Group_14, GROUP }, // x2
	  { X86_SSE2_Group_15, GROUP }, // x3
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_CMP, "pcmpeqb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x4
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_CMP, "pcmpeqw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_CMP, "pcmpeqd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE3_ADD, "haddpd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE3_SUB, "hsubpd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movd", { AMODE_E | OPTYPE_dq | OP_DST, AMODE_V | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movdqa", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* 8x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 9x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Ax */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Bx */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Cx */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_CMP, "cmppd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, // x2
	  { NOINSTR }, // x3
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pinsrw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_E | OPTYPE_w | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x4
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pextrw", { AMODE_G | OPTYPE_d | OP_DST, AMODE_VR| OPTYPE_o | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "shufpd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Dx */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE3, "addsubpd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psrlw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psrld", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psrlq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x3
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "paddq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x4
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MUL, "pmullw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movq", { AMODE_W | OPTYPE_q | OP_DST, AMODE_V | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pmovmskb", { AMODE_G | OPTYPE_d | OP_DST, AMODE_VR| OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x7
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "psubusb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "psubusw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x9
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pminub", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_AND, "pand", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "paddusb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "paddusw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pmaxub", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_AND, "pandn", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* Ex */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pavgb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psraw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psrad", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pavgw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x3
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MUL, "pmulhuw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x4
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MUL, "pmulhw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvttpd2dq", { AMODE_V | OPTYPE_q | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movntdq", { AMODE_M | OPTYPE_o | OP_DST, AMODE_V | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x7
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "psubsb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "psubsw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x9
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pminsw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_OR, "por", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "paddsb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "paddsw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pmaxuw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_XOR, "pxor", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* Fx */
	  { NOINSTR }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psllw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pslld", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psllq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x3
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MUL, "pmuludq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x4
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "pmaddwd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psadbw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "maskmovdqu", { AMODE_V | OPTYPE_o | OP_DST, AMODE_VR| OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x7
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "psubb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "psubw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x9
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "psubd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "psubq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "paddb", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "paddw", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "paddd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	  { NOINSTR }, // xF

	// prefix 0xf2 (repne)
		/* 0x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 1x */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movsd", { AMODE_V | OPTYPE_sdo | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movsd", { AMODE_W | OPTYPE_sd | OP_DST, AMODE_V | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE3_MOV, "movddup", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 2x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtsi2sd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_E | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOINSTR }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvttsd2si", { AMODE_G | OPTYPE_dq | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtsd2si", { AMODE_G | OPTYPE_dq | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 3x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 4x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 5x */
	  { NOINSTR }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "sqrtsd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "addsd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MUL, "mulsd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x9
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtsd2ss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOINSTR }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "subsd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "minsd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_DIV, "divsd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "maxsd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* 6x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movdqa", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* 7x */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pshuflw", { AMODE_V | OPTYPE_q | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE3_ADD, "haddps", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE3_SUB, "hsubps", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 8x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 9x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Ax */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Bx */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Cx */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_CMP, "cmpsd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_W | OPTYPE_sd | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Dx */
	  { NOGROUP, CPU_PENTIUM3, ITYPE_SSE3, "addsubps", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movdq2q", { AMODE_P | OPTYPE_q | OP_DST, AMODE_VR | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Ex */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtpd2dq", { AMODE_V | OPTYPE_q | OP_DST, AMODE_W | OPTYPE_pd | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Fx */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE3, "lddqu", { AMODE_V | OPTYPE_o | OP_DST, AMODE_M | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF

	// prefix 0xf3 (rep)
		/* 0x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 1x */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movss", { AMODE_V | OPTYPE_sso | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movss", { AMODE_W | OPTYPE_ss | OP_DST, AMODE_V | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE3_MOV, "movsldup", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE3_MOV, "movshdup", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 2x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "cvtsi2ss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_E | OPTYPE_dq | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOINSTR }, // xB
	  { NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "cvttss2si", { AMODE_G | OPTYPE_dq | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM3, ITYPE_SSE, "cvtss2si", { AMODE_G | OPTYPE_dq | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 3x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 4x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 5x */
	  { NOINSTR }, // x0
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "sqrtss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x1
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "rsqrtss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x2
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "rcpss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_ADD, "addss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MUL, "mulss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x9
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtss2sd", { AMODE_V | OPTYPE_sd | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xA
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvttps2dq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_ps | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xB
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_SUB, "subss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xC
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "minss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_DIV, "divss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "maxss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* 6x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movdqu", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* 7x */
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pshufhw", { AMODE_V | OPTYPE_q | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movq", { AMODE_V | OPTYPE_q | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xE
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movdqu", { AMODE_W | OPTYPE_o | OP_DST, AMODE_V | OPTYPE_o | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // xF
	
		/* 8x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* 9x */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Ax */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Bx */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Cx */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOGROUP, CPU_PENTIUM3, ITYPE_SSE_CMP, "cmpss", { AMODE_V | OPTYPE_ss | OP_DST, AMODE_W | OPTYPE_ss | OP_SRC, AMODE_I | OPTYPE_b | OP_SRC }, NOCOND, FLAG_COMMON_MOD, NOACTION, IGNORED }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Dx */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2_MOV, "movq2dq", { AMODE_V | OPTYPE_o | OP_DST, AMODE_PR | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Ex */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "cvtdq2pd", { AMODE_V | OPTYPE_o | OP_DST, AMODE_W | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR }, // xF
	
		/* Fx */
	  { NOINSTR }, // x0
	  { NOINSTR }, // x1
	  { NOINSTR }, // x2
	  { NOINSTR }, // x3
	  { NOINSTR }, // x4
	  { NOINSTR }, // x5
	  { NOINSTR }, // x6
	  { NOINSTR }, // x7
	  { NOINSTR }, // x8
	  { NOINSTR }, // x9
	  { NOINSTR }, // xA
	  { NOINSTR }, // xB
	  { NOINSTR }, // xC
	  { NOINSTR }, // xD
	  { NOINSTR }, // xE
	  { NOINSTR } // xF
};

X86_OPCODE X86_SSE2_Group_13[24] = // 66/F2/F3 0F 71
{
	// prefix 0x66 (operand size)
		{ NOINSTR }, /* 0x00 */
		{ NOINSTR }, /* 0x01 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psrlw", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
		{ NOINSTR }, /* 0x03 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psraw", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x04 */
		{ NOINSTR }, /* 0x05 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psllw", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
		{ NOINSTR }, /* 0x07 */

	// prefix 0xf2 (repne)
		{ NOINSTR }, /* 0x00 */
		{ NOINSTR }, /* 0x01 */
		{ NOINSTR }, /* 0x02 */
		{ NOINSTR }, /* 0x03 */
		{ NOINSTR }, /* 0x04 */
		{ NOINSTR }, /* 0x05 */
		{ NOINSTR }, /* 0x06 */
		{ NOINSTR }, /* 0x07 */

	// prefix 0xf3 (rep)
		{ NOINSTR }, /* 0x00 */
		{ NOINSTR }, /* 0x01 */
		{ NOINSTR }, /* 0x02 */
		{ NOINSTR }, /* 0x03 */
		{ NOINSTR }, /* 0x04 */
		{ NOINSTR }, /* 0x05 */
		{ NOINSTR }, /* 0x06 */
		{ NOINSTR }, /* 0x07 */
};

X86_OPCODE X86_SSE2_Group_14[24] = // 66/F2/F3 0F 72
{
	// prefix 0x66 (operand size)
		{ NOINSTR }, /* 0x00 */
		{ NOINSTR }, /* 0x01 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psrld", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
		{ NOINSTR }, /* 0x03 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psrad", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x04 */
		{ NOINSTR }, /* 0x05 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pslld", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
		{ NOINSTR }, /* 0x07 */

	// prefix 0xf2 (repne)
		{ NOINSTR }, /* 0x00 */
		{ NOINSTR }, /* 0x01 */
		{ NOINSTR }, /* 0x02 */
		{ NOINSTR }, /* 0x03 */
		{ NOINSTR }, /* 0x04 */
		{ NOINSTR }, /* 0x05 */
		{ NOINSTR }, /* 0x06 */
		{ NOINSTR }, /* 0x07 */

	// prefix 0xf3 (rep)
		{ NOINSTR }, /* 0x00 */
		{ NOINSTR }, /* 0x01 */
		{ NOINSTR }, /* 0x02 */
		{ NOINSTR }, /* 0x03 */
		{ NOINSTR }, /* 0x04 */
		{ NOINSTR }, /* 0x05 */
		{ NOINSTR }, /* 0x06 */
		{ NOINSTR }, /* 0x07 */
};

X86_OPCODE X86_SSE2_Group_15[24] =
{
	// prefix 0x66 (operand size)
		{ NOINSTR }, /* 0x00 */
		{ NOINSTR }, /* 0x01 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psrlq", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x02 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psrldq", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x03 */
		{ NOINSTR }, /* 0x04 */
		{ NOINSTR }, /* 0x05 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "psllq", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x06 */
		{ NOGROUP, CPU_PENTIUM4, ITYPE_SSE2, "pslldq", { AMODE_VR | OPTYPE_o | OP_DST, AMODE_I | OPTYPE_b | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0x07 */

	// prefix 0xf2 (repne)
		{ NOINSTR }, /* 0x00 */
		{ NOINSTR }, /* 0x01 */
		{ NOINSTR }, /* 0x02 */
		{ NOINSTR }, /* 0x03 */
		{ NOINSTR }, /* 0x04 */
		{ NOINSTR }, /* 0x05 */
		{ NOINSTR }, /* 0x06 */
		{ NOINSTR }, /* 0x07 */

	// prefix 0xf3 (rep)
		{ NOINSTR }, /* 0x00 */
		{ NOINSTR }, /* 0x01 */
		{ NOINSTR }, /* 0x02 */
		{ NOINSTR }, /* 0x03 */
		{ NOINSTR }, /* 0x04 */
		{ NOINSTR }, /* 0x05 */
		{ NOINSTR }, /* 0x06 */
		{ NOINSTR }, /* 0x07 */
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// 3DNow opcodes
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

X86_OPCODE X86_3DNOW_0F[0x100] =
{
	{ NOINSTR }, /* 00 */
	{ NOINSTR }, /* 01 */
	{ NOINSTR }, /* 02 */
	{ NOINSTR }, /* 03 */
	{ NOINSTR }, /* 04 */
	{ NOINSTR }, /* 05 */
	{ NOINSTR }, /* 06 */
	{ NOINSTR }, /* 07 */
	{ NOINSTR }, /* 08 */
	{ NOINSTR }, /* 09 */
	{ NOINSTR }, /* 0A */
	{ NOINSTR }, /* 0B */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pi2fw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0C */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pi2fd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 0D */
	{ NOINSTR }, /* 0E */
	{ NOINSTR }, /* 0F */
	{ NOINSTR }, /* 10 */
	{ NOINSTR }, /* 11 */
	{ NOINSTR }, /* 12 */
	{ NOINSTR }, /* 13 */
	{ NOINSTR }, /* 14 */
	{ NOINSTR }, /* 15 */
	{ NOINSTR }, /* 16 */
	{ NOINSTR }, /* 17 */
	{ NOINSTR }, /* 18 */
	{ NOINSTR }, /* 19 */
	{ NOINSTR }, /* 1A */
	{ NOINSTR }, /* 1B */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pf2iw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 1C */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pf2id", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 1D */
	{ NOINSTR }, /* 1E */
	{ NOINSTR }, /* 1F */
	{ NOINSTR }, /* 20 */
	{ NOINSTR }, /* 21 */
	{ NOINSTR }, /* 22 */
	{ NOINSTR }, /* 23 */
	{ NOINSTR }, /* 24 */
	{ NOINSTR }, /* 25 */
	{ NOINSTR }, /* 26 */
	{ NOINSTR }, /* 27 */
	{ NOINSTR }, /* 28 */
	{ NOINSTR }, /* 29 */
	{ NOINSTR }, /* 2A */
	{ NOINSTR }, /* 2B */
	{ NOINSTR }, /* 2C */
	{ NOINSTR }, /* 2D */
	{ NOINSTR }, /* 2E */
	{ NOINSTR }, /* 2F */
	{ NOINSTR }, /* 30 */
	{ NOINSTR }, /* 31 */
	{ NOINSTR }, /* 32 */
	{ NOINSTR }, /* 33 */
	{ NOINSTR }, /* 34 */
	{ NOINSTR }, /* 35 */
	{ NOINSTR }, /* 36 */
	{ NOINSTR }, /* 37 */
	{ NOINSTR }, /* 38 */
	{ NOINSTR }, /* 39 */
	{ NOINSTR }, /* 3A */
	{ NOINSTR }, /* 3B */
	{ NOINSTR }, /* 3C */
	{ NOINSTR }, /* 3D */
	{ NOINSTR }, /* 3E */
	{ NOINSTR }, /* 3F */
	{ NOINSTR }, /* 40 */
	{ NOINSTR }, /* 41 */
	{ NOINSTR }, /* 42 */
	{ NOINSTR }, /* 43 */
	{ NOINSTR }, /* 44 */
	{ NOINSTR }, /* 45 */
	{ NOINSTR }, /* 46 */
	{ NOINSTR }, /* 47 */
	{ NOINSTR }, /* 48 */
	{ NOINSTR }, /* 49 */
	{ NOINSTR }, /* 4A */
	{ NOINSTR }, /* 4B */
	{ NOINSTR }, /* 4C */
	{ NOINSTR }, /* 4D */
	{ NOINSTR }, /* 4E */
	{ NOINSTR }, /* 4F */
	{ NOINSTR }, /* 50 */
	{ NOINSTR }, /* 51 */
	{ NOINSTR }, /* 52 */
	{ NOINSTR }, /* 53 */
	{ NOINSTR }, /* 54 */
	{ NOINSTR }, /* 55 */
	{ NOINSTR }, /* 56 */
	{ NOINSTR }, /* 57 */
	{ NOINSTR }, /* 58 */
	{ NOINSTR }, /* 59 */
	{ NOINSTR }, /* 5A */
	{ NOINSTR }, /* 5B */
	{ NOINSTR }, /* 5C */
	{ NOINSTR }, /* 5D */
	{ NOINSTR }, /* 5E */
	{ NOINSTR }, /* 5F */
	{ NOINSTR }, /* 60 */
	{ NOINSTR }, /* 61 */
	{ NOINSTR }, /* 62 */
	{ NOINSTR }, /* 63 */
	{ NOINSTR }, /* 64 */
	{ NOINSTR }, /* 65 */
	{ NOINSTR }, /* 66 */
	{ NOINSTR }, /* 67 */
	{ NOINSTR }, /* 68 */
	{ NOINSTR }, /* 69 */
	{ NOINSTR }, /* 6A */
	{ NOINSTR }, /* 6B */
	{ NOINSTR }, /* 6C */
	{ NOINSTR }, /* 6D */
	{ NOINSTR }, /* 6E */
	{ NOINSTR }, /* 6F */
	{ NOINSTR }, /* 70 */
	{ NOINSTR }, /* 71 */
	{ NOINSTR }, /* 72 */
	{ NOINSTR }, /* 73 */
	{ NOINSTR }, /* 74 */
	{ NOINSTR }, /* 75 */
	{ NOINSTR }, /* 76 */
	{ NOINSTR }, /* 77 */
	{ NOINSTR }, /* 78 */
	{ NOINSTR }, /* 79 */
	{ NOINSTR }, /* 7A */
	{ NOINSTR }, /* 7B */
	{ NOINSTR }, /* 7C */
	{ NOINSTR }, /* 7D */
	{ NOINSTR }, /* 7E */
	{ NOINSTR }, /* 7F */
	{ NOINSTR }, /* 80 */
	{ NOINSTR }, /* 81 */
	{ NOINSTR }, /* 82 */
	{ NOINSTR }, /* 83 */
	{ NOINSTR }, /* 84 */
	{ NOINSTR }, /* 85 */
	{ NOINSTR }, /* 86 */
	{ NOINSTR }, /* 87 */
	{ NOINSTR }, /* 88 */
	{ NOINSTR }, /* 89 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfnacc", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 8A */
	{ NOINSTR }, /* 8B */
	{ NOINSTR }, /* 8C */
	{ NOINSTR }, /* 8D */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfpnacc", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 8E */
	{ NOINSTR }, /* 8F */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW_CMP, "pfcmpge", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 90 */
	{ NOINSTR }, /* 91 */
	{ NOINSTR }, /* 92 */
	{ NOINSTR }, /* 93 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfmin", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 94 */
	{ NOINSTR }, /* 95 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfrcp", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 96 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfrsqrt", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 97 */
	{ NOINSTR }, /* 98 */
	{ NOINSTR }, /* 99 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW_SUB, "pfsub", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 9A */
	{ NOINSTR }, /* 9B */
	{ NOINSTR }, /* 9C */
	{ NOINSTR }, /* 9D */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW_ADD, "pfadd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* 9E */
	{ NOINSTR }, /* 9F */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW_CMP, "pfcmpgt", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* A0 */
	{ NOINSTR }, /* A1 */
	{ NOINSTR }, /* A2 */
	{ NOINSTR }, /* A3 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfmax", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* A4 */
	{ NOINSTR }, /* A5 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfrcpit1", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* A6 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfrsqit1", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* A7 */
	{ NOINSTR }, /* A8 */
	{ NOINSTR }, /* A9 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW_SUB, "pfsubr", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* AA */
	{ NOINSTR }, /* AB */
	{ NOINSTR }, /* AC */
	{ NOINSTR }, /* AD */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfacc", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* AE */
	{ NOINSTR }, /* AF */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW_CMP, "pfcmpeq", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* B0 */
	{ NOINSTR }, /* B1 */
	{ NOINSTR }, /* B2 */
	{ NOINSTR }, /* B3 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW_MUL, "pfmul", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* B4 */
	{ NOINSTR }, /* B5 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pfrcpit2", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* B6 */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW_MUL, "pmulhrw", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* B7 */
	{ NOINSTR },		 /* B8 */
	{ NOINSTR }, /* B9 */
	{ NOINSTR }, /* BA */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW_XCHG, "pswapd", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* BB */
	{ NOINSTR }, /* BC */
	{ NOINSTR }, /* BD */
	{ NOINSTR }, /* BE */
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_3DNOW, "pavgb", { AMODE_P | OPTYPE_q | OP_DST, AMODE_Q | OPTYPE_q | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, /* BF */
	{ NOINSTR }, /* C0 */
	{ NOINSTR }, /* C1 */
	{ NOINSTR }, /* C2 */
	{ NOINSTR }, /* C3 */
	{ NOINSTR }, /* C4 */
	{ NOINSTR }, /* C5 */
	{ NOINSTR }, /* C6 */
	{ NOINSTR }, /* C7 */
	{ NOINSTR }, /* C8 */
	{ NOINSTR }, /* C9 */
	{ NOINSTR }, /* CA */
	{ NOINSTR }, /* CB */
	{ NOINSTR }, /* CC */
	{ NOINSTR }, /* CD */
	{ NOINSTR }, /* CE */
	{ NOINSTR }, /* CF */
	{ NOINSTR }, /* D0 */
	{ NOINSTR }, /* D1 */
	{ NOINSTR }, /* D2 */
	{ NOINSTR }, /* D3 */
	{ NOINSTR }, /* D4 */
	{ NOINSTR }, /* D5 */
	{ NOINSTR }, /* D6 */
	{ NOINSTR }, /* D7 */
	{ NOINSTR }, /* D8 */
	{ NOINSTR }, /* D9 */
	{ NOINSTR }, /* DA */
	{ NOINSTR }, /* DB */
	{ NOINSTR }, /* DC */
	{ NOINSTR }, /* DD */
	{ NOINSTR }, /* DE */
	{ NOINSTR }, /* DF */
	{ NOINSTR }, /* E0 */
	{ NOINSTR }, /* E1 */
	{ NOINSTR }, /* E2 */
	{ NOINSTR }, /* E3 */
	{ NOINSTR }, /* E4 */
	{ NOINSTR }, /* E5 */
	{ NOINSTR }, /* E6 */
	{ NOINSTR }, /* E7 */
	{ NOINSTR }, /* E8 */
	{ NOINSTR }, /* E9 */
	{ NOINSTR }, /* EA */
	{ NOINSTR }, /* EB */
	{ NOINSTR }, /* EC */
	{ NOINSTR }, /* ED */
	{ NOINSTR }, /* EE */
	{ NOINSTR }, /* EF */
	{ NOINSTR }, /* F0 */
	{ NOINSTR }, /* F1 */
	{ NOINSTR }, /* F2 */
	{ NOINSTR }, /* F3 */
	{ NOINSTR }, /* F4 */
	{ NOINSTR }, /* F5 */
	{ NOINSTR }, /* F6 */
	{ NOINSTR }, /* F7 */
	{ NOINSTR }, /* F8 */
	{ NOINSTR }, /* F9 */
	{ NOINSTR }, /* FA */
	{ NOINSTR }, /* FB */
	{ NOINSTR }, /* FC */
	{ NOINSTR }, /* FD */
	{ NOINSTR }, /* FE */
	{ NOINSTR }	/* FF */
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// 64-bit replacement opcodes
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

X86_OPCODE X86_Opcode_63[2] =
{
	{ NOGROUP, CPU_I386, ITYPE_SYSTEM, "arpl", { AMODE_E | OPTYPE_w | OP_SRC, AMODE_G | OPTYPE_w | OP_SRC, 0 }, NOCOND, FLAG_ZF_MOD, NOACTION, IGNORED }, // !ARCH_AMD64
	{ NOGROUP, CPU_AMD64, ITYPE_MOV, "movsxd", { AMODE_G | OPTYPE_v | OP_SIGNED | OP_DST, AMODE_E | OPTYPE_d | OP_SIGNED | OP_SRC, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED } // ARCH_AMD64
};

X86_OPCODE X86_Opcode_0F05[2] =
{
	{ NOGROUP, CPU_AMD_K6_2, ITYPE_SYSCALL, "syscall", { OPTYPE_STAR_MSR | OP_MSR | OP_SRC, OPTYPE_CSTAR_MSR | OP_MSR | OP_SRC, OPTYPE_FMASK_MSR | OP_MSR | OP_SRC }, NOCOND, FLAG_ZF_MOD, NOACTION, IGNORED }, // !ARCH_AMD64
	{ NOGROUP, CPU_AMD64, ITYPE_SYSCALL, "syscall", { OPTYPE_STAR_MSR | OP_MSR | OP_SRC, OPTYPE_LSTAR_MSR | OP_MSR | OP_SRC, OPTYPE_FMASK_MSR | OP_MSR | OP_SRC }, NOCOND, NOCHANGE, NOACTION, IGNORED } // ARCH_AMD64
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// Other 3 byte opcodes
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// Three byte opcodes where the third opcode byte is ModRM
X86_OPCODE X86_0F01_ModRM[0x100] = 
{
	/* 0x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* 1x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* 2x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* 3x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* 4x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* 5x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* 6x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* 7x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* 8x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* 9x */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* Ax */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* Bx */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* Cx */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { NOGROUP, CPU_PRESCOTT, ITYPE_SYSTEM, "monitor", NOARGS, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
  { NOGROUP, CPU_PRESCOTT, ITYPE_SYSTEM, "mwait", NOARGS, NOCOND, NOCHANGE, SERIALIZE_ALL, IGNORED }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* Dx */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* Ex */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { X86_Group_7, GROUP }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }, // xF

	/* Fx */
  { X86_Group_7, GROUP }, // x0
  { X86_Group_7, GROUP }, // x1
  { X86_Group_7, GROUP }, // x2
  { X86_Group_7, GROUP }, // x3
  { X86_Group_7, GROUP }, // x4
  { X86_Group_7, GROUP }, // x5
  { X86_Group_7, GROUP }, // x6
  { X86_Group_7, GROUP }, // x7
  { NOGROUP, CPU_AMD64, ITYPE_SYSTEM, "swapgs", { OPTYPE_KERNELBASE_MSR | OP_MSR | OP_SRC, 0, 0 }, NOCOND, NOCHANGE, NOACTION, IGNORED }, // x8
  { X86_Group_7, GROUP }, // x9
  { X86_Group_7, GROUP }, // xA
  { X86_Group_7, GROUP }, // xB
  { X86_Group_7, GROUP }, // xC
  { X86_Group_7, GROUP }, // xD
  { X86_Group_7, GROUP }, // xE
  { X86_Group_7, GROUP }  // xF
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// Sanity checking tables
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

#define S2 1 // SSE2
#define S3 2 // SSE3
static BYTE X86_ModRM_1[0x100] =
{
  //      x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
  /* 0x */ 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 
  /* 1x */ 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 
  /* 2x */ 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 
  /* 3x */ 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 
  /* 4x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* 5x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* 6x */ 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 
  /* 7x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* 8x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
  /* 9x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* Ax */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* Bx */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* Cx */ 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* Dx */ 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 
  /* Ex */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* Fx */ 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1
};
static BYTE X86_ModRM_2[0x100] =
{
  //      x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
  /* 0x */ 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 
  /* 1x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
  /* 2x */ 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 
  /* 3x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* 4x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
  /* 5x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
  /* 6x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
  /* 7x */ 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 
  /* 8x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* 9x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
  /* Ax */ 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 
  /* Bx */ 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 
  /* Cx */ 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
  /* Dx */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
  /* Ex */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
  /* Fx */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
};

static BYTE X86_SSE_2[0x100] = 
{
  /*      x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF        */
  /* 0x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* 1x */ 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* 2x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0,
  /* 3x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* 4x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* 5x */ 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
  /* 6x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
  /* 7x */ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  /* 8x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* 9x */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Ax */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Bx */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Cx */ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Dx */ 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Ex */ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Fx */ 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0
};

// Indicates if a LOCK prefix is allowed
// The following are allowed:
// add, adc, and, btc, btr, bts, cmpxchg, cmpxchg8, dec, inc, 
// neg, not, or, sbb, sub, xor, xadd, xchg
#define GR 2
static BYTE X86_LockPrefix_1[0x100] =
{
  //       x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF
  /* 0x */  1,  1,  1,  1,  0,  0,  0,  0,  1,  1,  1,  1,  0,  0,  0,  0, 
  /* 1x */  1,  1,  1,  1,  0,  0,  0,  0,  1,  1,  1,  1,  0,  0,  0,  0, 
  /* 2x */  1,  1,  1,  1,  0,  0,  0,  0,  1,  1,  1,  1,  0,  0,  0,  0, 
  /* 3x */  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 4x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 5x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 6x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 7x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 8x */ GR, GR, GR, GR,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 9x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Ax */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Bx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Cx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Dx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Ex */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Fx */  0,  0,  0,  0,  0,  0, GR, GR,  0,  0,  0,  0,  0,  0, GR, GR
};

static BYTE X86_LockPrefix_2[0x100] =
{
  //       x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF
  /* 0x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 1x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 2x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 3x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 4x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 5x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 6x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 7x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 8x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* 9x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Ax */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0, 
  /* Bx */  1,  1,  0,  1,  0,  0,  0,  0,  0,  0, GR,  1,  0,  0,  0,  0, 
  /* Cx */  1,  1,  0,  0,  0,  0,  0, GR,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Dx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Ex */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  /* Fx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static BYTE X86_LockPrefix_Groups[17][8] =
{
//  x0  x1  x2  x3  x4  x5  x6  x7
	{  1,  1,  1,  1,  1,  1,  1,  0 }, // group 1
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 2
	{  0,  0,  1,  1,  0,  0,  0,  0 }, // group 3
	{  1,  1,  0,  0,  0,  0,  0,  0 }, // group 4
	{  1,  1,  0,  0,  0,  0,  0,  0 }, // group 5
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 6
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 7
	{  0,  0,  0,  0,  1,  1,  1,  1 }, // group 8
	{  0,  1,  0,  0,  0,  0,  0,  0 }, // group 9
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 10
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 11
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 12
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 13
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 14
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 15
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 16
	{  0,  0,  0,  0,  0,  0,  0,  0 }, // group 17
};

#define X86_MAX_GROUP 19
static BYTE X86_Groups_1[0x100] = // one-byte opcodes
{
	/*       x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */
	/* 0x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 0x */
	/* 1x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 1x */
	/* 2x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 2x */
	/* 3x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 3x */
	/* 4x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 4x */
	/* 5x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 5x */
	/* 6x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 6x */
	/* 7x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 7x */
	/* 8x */  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10, /* 8x */
	/* 9x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 9x */
	/* Ax */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ax */
	/* Bx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Bx */
	/* Cx */  2,  2,  0,  0,  0,  0, 12, 12,  0,  0,  0,  0,  0,  0,  0,  0, /* Cx */
	/* Dx */  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Dx */
	/* Ex */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ex */
	/* Fx */  0,  0,  0,  0,  0,  0,  3,  3,  0,  0,  0,  0,  0,  0,  4,  5  /* Fx */
};

// 19 = Group P
// 20 = 3DNow
static BYTE X86_Groups_2[0x100] = // two-byte opcodes
{
	/*       x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */
	/* 0x */  6,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 18,  0, 19, /* 0x */
	/* 1x */  0,  0,  0,  0,  0,  0,  0,  0, 17,  0,  0,  0,  0,  0,  0,  0, /* 1x */
	/* 2x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 2x */
	/* 3x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 3x */
	/* 4x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 4x */
	/* 5x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 5x */
	/* 6x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 6x */
	/* 7x */  0, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 7x */
	/* 8x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 8x */
	/* 9x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 9x */
	/* Ax */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 16,  0, /* Ax */
	/* Bx */  0,  0,  0,  0,  0,  0,  0,  0,  0, 11,  8,  0,  0,  0,  0,  0, /* Bx */
	/* Cx */  0,  0,  0,  0,  0,  0,  0,  9,  0,  0,  0,  0,  0,  0,  0,  0, /* Cx */
	/* Dx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Dx */
	/* Ex */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ex */
	/* Fx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  /* Fx */
};

// Indicate which 1-byte opcodes are invalid with a 16-bit operand size
static BYTE X86_Invalid_Op16_1[0x100] = 
{
	/*       x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */
	/* 0x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 0x */
	/* 1x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 1x */
	/* 2x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 2x */
	/* 3x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 3x */
	/* 4x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 4x */
	/* 5x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 5x */
	/* 6x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 6x */
	/* 7x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 7x */
	/* 8x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 8x */
	/* 9x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 9x */
	/* Ax */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ax */
	/* Bx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Bx */
	/* Cx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Cx */
	/* Dx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Dx */
	/* Ex */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ex */
	/* Fx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  /* Fx */
};

// Indicate which 2-byte opcodes are invalid with a 16-bit operand size
static BYTE X86_Invalid_Op16_2[0x100] =
{
	/*       x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */
	/* 0x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 0x */
	/* 1x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 1x */
	/* 2x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 2x */
	/* 3x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 3x */
	/* 4x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 4x */
	/* 5x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 5x */
	/* 6x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 6x */
	/* 7x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 7x */
	/* 8x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 8x */
	/* 9x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 9x */
	/* Ax */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ax */
	/* Bx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Bx */
	/* Cx */  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1, /* Cx */
	/* Dx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Dx */
	/* Ex */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ex */
	/* Fx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  /* Fx */
};

// Indicate which 1-byte opcodes are invalid with a 64-bit address size
static BYTE X86_Invalid_Addr64_1[0x100] =
{
	/*       x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */
	/* 0x */  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  1,  0, /* 0x */
	/* 1x */  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0, /* 1x */
	/* 2x */  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1, /* 2x */
	/* 3x */  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1, /* 3x */
	/* 4x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 4x */
	/* 5x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 5x */
	/* 6x */  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 6x */
	/* 7x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 7x */
	/* 8x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 8x */
	/* 9x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0, /* 9x */
	/* Ax */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ax */
	/* Bx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Bx */
	/* Cx */  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0, /* Cx */
	/* Dx */  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Dx */
	/* Ex */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0, /* Ex */
	/* Fx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  /* Fx */
};

// Indicate which 2-byte opcodes are invalid with a 64-bit address size
static BYTE X86_Invalid_Addr64_2[0x100] =
{
	/*       x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */
	/* 0x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 0x */
	/* 1x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 1x */
	/* 2x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 2x */
	/* 3x */  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 3x */
	/* 4x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 4x */
	/* 5x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 5x */
	/* 6x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 6x */
	/* 7x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 7x */
	/* 8x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 8x */
	/* 9x */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 9x */
	/* Ax */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ax */
	/* Bx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Bx */
	/* Cx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Cx */
	/* Dx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Dx */
	/* Ex */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* Ex */
	/* Fx */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  /* Fx */
};

#endif // DISASM_X86_TABLES

#ifdef _WIN64
#pragma warning(disable:4311 4312)
#endif

////////////////////////////////////////////////////////////////////////
// static INTERNAL macros
////////////////////////////////////////////////////////////////////////

#define VIRTUAL_ADDRESS ((U64)Instruction->Address + Instruction->VirtualAddressDelta)

#define AMD64_DIFF (AMD64_8BIT_OFFSET-X86_8BIT_OFFSET)
#define IS_AMD64() (INS_ARCH_TYPE(Instruction) == ARCH_X64)
#define IS_X86_32() (INS_ARCH_TYPE(Instruction) == ARCH_X86)
#define IS_X86_16() (INS_ARCH_TYPE(Instruction) == ARCH_X86_16)

#define X86_BOUND 0x62
#define X86_PUSH_REG 0x50
#define X86_PUSH_CS 0x0e
#define X86_PUSH_DS 0x1e
#define X86_PUSH_SS 0x16
#define X86_PUSH_ES 0x06
#define X86_PUSH_FS 0xa0
#define X86_PUSH_GS 0xa8
#define X86_PUSH_U8 0x6a
#define X86_PUSH_U32 0x68
#define X86_POP_DS 0x1f
#define X86_POP_ES 0x07
#define X86_POP_SS 0x17
#define X86_POP_FS 0xa1
#define X86_POP_GS 0xa9
#define X86_POP_REG 0x58

#define OPCSTR Instruction->String+Instruction->StringIndex
#define APPEND Instruction->StringIndex += (U8)_snprintf
#define APPENDPAD(x) \
{ \
	if (Instruction->StringAligned) \
	{  \
			if (Instruction->StringIndex > x) assert(0); \
			while (x != Instruction->StringIndex) APPENDB(' ');  \
	}  \
	else if (Instruction->StringIndex) \
	{  \
		APPENDB(' '); \
	} \
}

#define APPENDB(a) Instruction->String[Instruction->StringIndex++] = a
#define APPENDS(a) APPEND(OPCSTR, SIZE_LEFT, a);

#define SIZE_LEFT (MAX_OPCODE_DESCRIPTION-1 > Instruction->StringIndex ? MAX_OPCODE_DESCRIPTION-Instruction->StringIndex : 0)

// If an address size prefix is used for an instruction that doesn't make sense, restore it
// to the default

#define SANITY_CHECK_OPERAND_SIZE() \
{ \
	if (!Instruction->AnomalyOccurred && X86Instruction->HasOperandSizePrefix) \
	{ \
		if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Unexpected operand size prefix\n", VIRTUAL_ADDRESS); \
		Instruction->AnomalyOccurred = TRUE; \
		X86Instruction->HasOperandSizePrefix = FALSE; \
		switch (X86Instruction->OperandSize) \
		{ \
			case 4: X86Instruction->OperandSize = 2; break; \
			case 2: X86Instruction->OperandSize = 4; break; \
			default: assert(0); \
		} \
	} \
}

#define SANITY_CHECK_ADDRESS_SIZE() \
{ \
	if (!Instruction->AnomalyOccurred && X86Instruction->HasAddressSizePrefix) \
	{ \
		if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Unexpected address size prefix\n", VIRTUAL_ADDRESS); \
		Instruction->AnomalyOccurred = TRUE; \
	} \
	X86Instruction->HasAddressSizePrefix = FALSE; \
	switch (INS_ARCH_TYPE(Instruction)) \
	{ \
		case ARCH_X64: X86Instruction->AddressSize = 8; break; \
		case ARCH_X86: X86Instruction->AddressSize = 4; break; \
		case ARCH_X86_16: X86Instruction->AddressSize = 2; break; \
	} \
}

#define SANITY_CHECK_SEGMENT_OVERRIDE() \
	if (!Instruction->AnomalyOccurred && X86Instruction->HasSegmentOverridePrefix) \
	{ \
		if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Unexpected segment override\n", VIRTUAL_ADDRESS); \
		Instruction->AnomalyOccurred = TRUE; \
	}

#define INSTR_INC(size) \
{ \
	Instruction->Length += size; \
	Address += size; \
}

#define X86_SET_TARGET() \
{ \
	if (X86Instruction->HasSelector) \
	{ \
		if (!Instruction->AnomalyOccurred) \
		{ \
			if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: unexpected segment 0x%02X\n", VIRTUAL_ADDRESS, X86Instruction->Selector); \
			Instruction->AnomalyOccurred = TRUE; \
		} \
	} \
	else \
	{ \
		switch (X86Instruction->Segment) \
		{ \
			case SEG_CS: \
			case SEG_DS: \
			case SEG_SS: \
			case SEG_ES: \
				assert(!X86Instruction->HasSelector); \
				Operand->TargetAddress = (U64)X86Instruction->Displacement; \
				/* assert(!GetAbsoluteAddressFromSegment((BYTE)X86Instruction->Segment, (DWORD)X86Instruction->Displacement) || GetAbsoluteAddressFromSegment(X86Instruction->Segment, (DWORD)X86Instruction->Displacement) == Operand->TargetAddress); */ \
				break; \
			case SEG_FS: \
			case SEG_GS: \
				assert(!X86Instruction->HasSelector); \
				Operand->TargetAddress = (U64)GetAbsoluteAddressFromSegment((BYTE)X86Instruction->Segment, (DWORD)X86Instruction->Displacement); \
				break; \
			default: \
				assert(0); /* shouldn't be possible */ \
				break; \
		} \
	} \
}

#define X86_SET_SEG(reg) \
{ \
	if (!X86Instruction->HasSegmentOverridePrefix && (reg == REG_EBP || reg == REG_ESP)) \
	{ \
		assert(!X86Instruction->HasSelector); \
		X86Instruction->Segment = SEG_SS; \
	} \
}

#define X86_SET_ADDR() \
{ \
	if (Operand->Flags & OP_DST) \
	{ \
		assert(!X86Instruction->HasDstAddressing); \
		X86Instruction->HasDstAddressing = TRUE; \
		X86Instruction->DstOpIndex[X86Instruction->DstOpCount] = (U8)OperandIndex; \
		X86Instruction->DstOpCount++; \
		X86Instruction->DstAddressIndex = (U8)OperandIndex; \
	} \
	if (Operand->Flags & OP_SRC) \
	{ \
		if (Instruction->Type != ITYPE_STRCMP) assert(!X86Instruction->HasSrcAddressing); \
		X86Instruction->HasSrcAddressing = TRUE; \
		X86Instruction->SrcOpIndex[X86Instruction->SrcOpCount] = (U8)OperandIndex; \
		X86Instruction->SrcOpCount++; \
		X86Instruction->SrcAddressIndex = (U8)OperandIndex; \
	} \
}

#define X86_SET_REG(reg) \
{ \
	if (Operand->Flags & OP_DST) \
	{ \
		X86Instruction->DstOpIndex[X86Instruction->DstOpCount] = (U8)OperandIndex; \
		X86Instruction->DstOpCount++; \
		assert(OperandIndex < 2); \
		if (Operand->Length > 1 && reg == REG_ESP) Instruction->Groups |= ITYPE_STACK; \
	} \
	if (Operand->Flags & OP_SRC) \
	{ \
		X86Instruction->SrcOpIndex[X86Instruction->SrcOpCount] = (U8)OperandIndex; \
		X86Instruction->SrcOpCount++; \
	} \
} 

#define CHECK_AMD64_REG() { if (IS_AMD64()) Operand->Register += AMD64_DIFF; }

////////////////////////////////////////////////////////////////////////
// static INTERNAL structures/variables
////////////////////////////////////////////////////////////////////////

static ARCHITECTURE_FORMAT_FUNCTIONS X86 = 
{
	X86_InitInstruction, 
	NULL,
	X86_GetInstruction,
	X86_FindFunctionByPrologue 
};

static char *X86_Registers[0xE0] = 
{
	// Segments
	"es", // 0x00
	"cs", // 0x01
	"ss", // 0x02
	"ds", // 0x03
	"fs", // 0x04
	"gs", // 0x05
	"flags", // 0x06
	"eflags", // 0x07
	"rflags", // 0x08
	"ip+ilen", // 0x09
	"eip+ilen", // 0x0A
	"rip+ilen", // 0x0B
	NULL, // 0x0C
	NULL, // 0x0D
	NULL, // 0x0E
	NULL, // 0x0F

	// Test
	"tr0", // 0x10
	"tr1", // 0x11
	"tr2", // 0x12
	"tr3", // 0x13
	"tr4", // 0x14
	"tr5", // 0x15
	"tr6", // 0x16
	"tr7", // 0x17
	"tr8", // 0x18
	"tr9", // 0x19
	"tr10", // 0x1A
	"tr11", // 0x1B
	"tr12", // 0x1C
	"tr13", // 0x1D
	"tr14", // 0x1E
	"tr15", // 0x1F

	// Control
	"cr0", // 0x20
	"cr1", // 0x21
	"cr2", // 0x22
	"cr3", // 0x23
	"cr4", // 0x24
	"cr5", // 0x25
	"cr6", // 0x26
	"cr7", // 0x27
	"cr8", // 0x18
	"cr9", // 0x19
	"cr10", // 0x1A
	"cr11", // 0x1B
	"cr12", // 0x1C
	"cr13", // 0x1D
	"cr14", // 0x1E
	"cr15", // 0x1F

	// Debug
	"dr0", // 0x30
	"dr1", // 0x31
	"dr2", // 0x32
	"dr3", // 0x33
	"dr4", // 0x34
	"dr5", // 0x35
	"dr6", // 0x36
	"dr7", // 0x37
	"dr8", // 0x38
	"dr9", // 0x39
	"dr10", // 0x3A
	"dr11", // 0x3B
	"dr12", // 0x3C
	"dr13", // 0x3D
	"dr14", // 0x3E
	"dr15", // 0x3F

	// FPU
	"st(0)", // 0x40
	"st(1)", // 0x41
	"st(2)", // 0x42
	"st(3)", // 0x43
	"st(4)", // 0x44
	"st(5)", // 0x45
	"st(6)", // 0x46
	"st(7)", // 0x47
	NULL, // 0x48
	NULL, // 0x49
	NULL, // 0x4A
	NULL, // 0x4B
	NULL, // 0x4C
	NULL, // 0x4D
	NULL, // 0x4E
	NULL, // 0x4F

	// MMX
	"mm0", // 0x50
	"mm1", // 
	"mm2",
	"mm3",
	"mm4",
	"mm5",
	"mm6",
	"mm7",
	NULL, // 0x58
	NULL, // 0x59
	NULL, // 0x5A
	NULL, // 0x5B
	NULL, // 0x5C
	NULL, // 0x5D
	NULL, // 0x5E
	NULL, // 0x5F

	// XMM
	"xmm0", // 0x60
	"xmm1", // 0x61
	"xmm2", // 0x62
	"xmm3", // 0x63
	"xmm4", // 0x64
	"xmm5", // 0x65
	"xmm6", // 0x66
	"xmm7", // 0x67
	"xmm8", // 0x68
	"xmm9", // 0x69
	"xmm10", // 0x6a
	"xmm11", // 0x6b
	"xmm12", // 0x6c
	"xmm13", // 0x6d
	"xmm14", // 0x6e
	"xmm15", // 0x6f

	// 8-bit
	"al", // 0x70
	"cl", // 0x71
	"dl", // 0x72
	"bl", // 0x73
	"ah", // 0x74
	"ch", // 0x75
	"dh", // 0x76
	"bh", // 0x77
 	NULL, // 0x78
	NULL, // 0x79
	NULL, // 0x7A
	NULL, // 0x7B
	NULL, // 0x7C
	NULL, // 0x7D
	NULL, // 0x7E
	NULL, // 0x7F

	// 16-bit
	"ax", // 0x80
	"cx", // 0x81
	"dx", // 0x82
	"bx", // 0x83
	"sp", // 0x84
	"bp", // 0x85
	"si", // 0x86
	"di", // 0x87
	NULL, // 0x88
	NULL, // 0x89
	NULL, // 0x8A
	NULL, // 0x8B
	NULL, // 0x8C
	NULL, // 0x8D
	NULL, // 0x8E
	NULL, // 0x8F

	// 32-bit
	"eax", // 0x90
	"ecx", // 0x91
	"edx", // 0x92
	"ebx", // 0x93
	"esp", // 0x94
	"ebp", // 0x95
	"esi", // 0x96
	"edi", // 0x97
	NULL, // 0x98
	NULL, // 0x99
	NULL, // 0x9A
	NULL, // 0x9B
	NULL, // 0x9C
	NULL, // 0x9D
	NULL, // 0x9E
	NULL, // 0x9F

	// X86-64 8-bit register
	"al", // 0xA0
	"cl", // 0xA1
	"dl", // 0xA2
	"bl", // 0xA3
	"spl", // 0xA4
	"bpl", // 0xA5
	"sil", // 0xA6
	"dil", // 0xA7
	"r8b", // 0xA8
	"r9b", // 0xA9
	"r10b", // 0xAA
	"r11b", // 0xAB
	"r12b", // 0xAC
	"r13b", // 0xAD
	"r14b", // 0xAE
	"r15b", // 0xAF

	// X86-64 16-bit register	
	"ax", // 0xB0
	"cx", // 0xB1
	"dx", // 0xB2
	"bx", // 0xB3
	"sp", // 0xB4
	"bp", // 0xB5
	"si", // 0xB6
	"di", // 0xB7
	"r8w", // 0xB8
	"r9w", // 0xB9
	"r10w", // 0xBA
	"r11w", // 0xBB
	"r12w", // 0xBC
	"r13w", // 0xBD
	"r14w", // 0xBE
	"r15w", // 0xBF

	// X86-64 32-bit register
	"eax", // 0xC0
	"ecx", // 0xC1
	"edx", // 0xC2
	"ebx", // 0xC3
	"esp", // 0xC4
	"ebp", // 0xC5
	"esi", // 0xC6
	"edi", // 0xC7
	"r8d", // 0xC8
	"r9d", // 0xC9
	"r10d", // 0xCA
	"r11d", // 0xCB
	"r12d", // 0xCC
	"r13d", // 0xCD
	"r14d", // 0xCE
	"r15d", // 0xCF

	// X86-64 64-bit register	
	"rax", // 0xD0
	"rcx", // 0xD1
	"rdx", // 0xD2
	"rbx", // 0xD3
	"rsp", // 0xD4
	"rbp", // 0xD5
	"rsi", // 0xD6
	"rdi", // 0xD7
	"r8", // 0xD8
	"r9", // 0xD9
	"r10", // 0xDA
	"r11", // 0xDB
	"r12", // 0xDC
	"r13", // 0xDD
	"r14", // 0xDE
	"r15" // 0xDF
};

void OutputBounds(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputGeneral(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputDescriptor(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputSegOffset(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputPackedReal(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputPackedBCD(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputScalarReal(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputScalarGeneral(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputFPUEnvironment(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputFPUState(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
void OutputCPUState(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);

typedef void (*OUTPUT_OPTYPE)(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex);
#define OPTYPE_SHIFT 24
#define MAX_OPTYPE_INDEX 26
static OUTPUT_OPTYPE OptypeHandlers[] =
{
	NULL,
	OutputBounds,         // 01 OPTYPE_a
	OutputGeneral,        // 02 OPTYPE_b
	OutputGeneral,        // 03 OPTYPE_d
	OutputSegOffset,      // 04 OPTYPE_p
	OutputGeneral,        // 05 OPTYPE_q
	OutputDescriptor,     // 06 OPTYPE_dt
	OutputGeneral,        // 07 OPTYPE_v
	OutputGeneral,        // 08 OPTYPE_w
	OutputPackedReal,     // 09 OPTYPE_ps
	OutputPackedReal,     // 0A OPTYPE_pd
	OutputPackedBCD,      // 0B OPTYPE_pb
	OutputScalarReal,     // 0C OPTYPE_ss
	OutputScalarReal,     // 0D OPTYPE_sd
	OutputScalarReal,     // 0E OPTYPE_se
	OutputFPUEnvironment, // 0F OPTYPE_fev
	OutputFPUState,       // 10 OPTYPE_fst1
	OutputFPUState,       // 11 OPTYPE_fst2
	OutputGeneral,        // 12 OPTYPE_z
	OutputGeneral,        // 13 OPTYPE_o
	OutputGeneral,        // 14 OPTYPE_dq
	OutputGeneral,        // 15 OPTYPE_mw
	OutputScalarGeneral,  // 16 OPTYPE_sso
	OutputScalarGeneral,  // 17 OPTYPE_sdo
	OutputCPUState,       // 18 OPTYPE_cpu
	OutputGeneral,        // 19 OPTYPE_lea
};

#define OPTYPE_a    0x01000000
#define OPTYPE_b    0x02000000
#define OPTYPE_d    0x03000000
#define OPTYPE_p    0x04000000
#define OPTYPE_q    0x05000000
#define OPTYPE_dt   0x06000000
#define OPTYPE_v    0x07000000
#define OPTYPE_w    0x08000000
#define OPTYPE_ps   0x09000000 // packed 128-bit single real
#define OPTYPE_pd   0x0A000000 // packed 128-bit double real
#define OPTYPE_pb	  0x0B000000 // packed BCD (10 bytes, 18-bit precision)
#define OPTYPE_ss   0x0C000000 // scalar single real
#define OPTYPE_sd   0x0D000000 // scalar double real
#define OPTYPE_se   0x0E000000 // scalar extended real
#define OPTYPE_fev  0x0F000000 // FPU environment (28 bytes if 32-bit modes, 14 bytes in 16-bit mode)
#define OPTYPE_fst1 0x10000000 // FPU state (108 bytes in 32-bit modes, 94 bytes in 16-bit real mode)
#define OPTYPE_fst2 0x11000000 // FPU/MMX/XMM/MXCSR state (512 bytes)
#define OPTYPE_z	  0x12000000
#define OPTYPE_o	  0x13000000
#define OPTYPE_dq   0x14000000 // OPTYPE_d or OPTYPE_o
#define OPTYPE_mw   0x15000000 // word if memory, register size otherwise
#define OPTYPE_sso  0x16000000 // OPTYPE_ss or OPTYPE_o
#define OPTYPE_sdo  0x17000000 // OPTYPE_ss or OPTYPE_o
#define OPTYPE_cpu  0x18000000 // pointer to CPU state structure
#define OPTYPE_lea  0x19000000 // size set by other operand

////////////////////////////////////////////////////////////////////////
// static INTERNAL functions
////////////////////////////////////////////////////////////////////////

#ifdef TEST_DISASM // TODO: remove
U32 X86_GetLength(INSTRUCTION *Instruction, U8 *Address);
#endif

static  BOOL IsValidLockPrefix(X86_INSTRUCTION *Instruction, U8 Opcode, U32 OpcodeLength, U8 Group, U8 OpcodeExtension);
static U8 *SetOperands(INSTRUCTION *Instruction, U8 *Address, U32 Flags);
static U8 *SetModRM32(INSTRUCTION *Instruction, U8 *Address, INSTRUCTION_OPERAND *Operand, U32 OperandIndex, BOOL SuppressErrors);
static U8 *SetModRM16(INSTRUCTION *Instruction, U8 *Address, INSTRUCTION_OPERAND *Operand, U32 OperandIndex, BOOL SuppressErrors);
static U8 *SetSIB(INSTRUCTION *Instruction, U8 *Address, INSTRUCTION_OPERAND *Operand, U32 OperandIndex, BOOL SuppressErrors);
static  U64 ApplyDisplacement(U64 Address, INSTRUCTION *Instruction);

//////////////////////////////////////////////////////////
// Instruction setup
//////////////////////////////////////////////////////////

#define APPLY_OFFSET(addr) \
{ \
	switch (X86Instruction->OperandSize) \
	{ \
		case 8: addr = ((U64)(addr + Instruction->VirtualAddressDelta)); break; \
		case 4: addr = (U64)((U32)(addr + Instruction->VirtualAddressDelta)); break; \
		case 2: addr = (U64)((U8)(addr + Instruction->VirtualAddressDelta)); break; \
		default: assert(0); break; \
	} \
}

static BOOL X86_InitInstruction(INSTRUCTION *Instruction)
{
	X86_INSTRUCTION *X86Instruction;
#ifdef NO_SANITY_CHECKS
	assert(0); // be sure assertions are disabled
#endif
	X86Instruction = &Instruction->X86;
	memset(X86Instruction, 0, sizeof(X86_INSTRUCTION));
	
	switch (INS_ARCH_TYPE(Instruction))
	{
		case ARCH_X64:
			X86Instruction->AddressSize = 8;
			X86Instruction->OperandSize = 4;
			break;
		case ARCH_X86:
			X86Instruction->AddressSize = 4;
			X86Instruction->OperandSize = 4;
			break;
		case ARCH_X86_16:
			X86Instruction->AddressSize = 2;
			X86Instruction->OperandSize = 2;
			break;
		default:
			assert(0);
			return FALSE;
	}
	X86Instruction->Instruction = Instruction;
	X86Instruction->Segment = SEG_DS;
	return TRUE;
}

////////////////////////////////////////////////////////////
// Formatting
// You can change these to whatever you prefer
////////////////////////////////////////////////////////////

#define X86_WRITE_OPFLAGS() \
	if (Flags & DISASM_SHOWFLAGS) \
	{ \
		APPENDB('{'); \
		assert(Operand->Flags & (OP_EXEC|OP_SRC|OP_DST)); \
		if (Operand->Flags & OP_IPREL) APPENDB('r'); \
		if (Operand->Flags & OP_FAR) APPENDB('f'); \
		if (Operand->Flags & OP_CONDR) APPENDB('c'); \
		if (Operand->Flags & OP_EXEC) APPENDB('X'); \
		else if (Operand->Flags & OP_SRC) APPENDB('R'); \
		if (Operand->Flags & OP_CONDW) APPENDB('c'); \
		if (Operand->Flags & OP_DST) APPENDB('W'); \
		if (Operand->Flags & OP_SYS) APPENDB('S'); \
		if (Operand->Flags & OP_ADDRESS) APPENDB('A'); \
		if (Operand->Flags & OP_PARAM) APPENDB('P'); \
		if (Operand->Flags & OP_LOCAL) APPENDB('L'); \
		if (Operand->Flags & OP_GLOBAL) APPENDB('G'); \
		APPENDB('}'); \
	}

#define X86_WRITE_IMMEDIATE() \
{ \
	switch (Operand->Length) \
	{ \
		case 8: \
			APPEND(OPCSTR, SIZE_LEFT, "0x%02I64X=", Operand->Value_U64); \
			if (Operand->Value_S64 >= 0 || !(Operand->Flags & OP_SIGNED)) APPEND(OPCSTR, SIZE_LEFT, "%I64u", Operand->Value_U64); \
			/*else APPEND(OPCSTR, SIZE_LEFT, "-0x%02I64X=%I64d", -Operand->Value_S64, Operand->Value_S64);*/ \
			else APPEND(OPCSTR, SIZE_LEFT, "%I64d", Operand->Value_S64); \
			break; \
		case 4: \
			APPEND(OPCSTR, SIZE_LEFT, "0x%02lX=", (U32)Operand->Value_U64); \
			if (Operand->Value_S64 >= 0 || !(Operand->Flags & OP_SIGNED)) APPEND(OPCSTR, SIZE_LEFT, "%lu", (U32)Operand->Value_U64); \
			/*else APPEND(OPCSTR, SIZE_LEFT, "-0x%02lX=%ld", (U32)-Operand->Value_S64, (S32)Operand->Value_S64);*/ \
			else APPEND(OPCSTR, SIZE_LEFT, "%ld", (S32)Operand->Value_S64); \
			break; \
		case 2: \
			APPEND(OPCSTR, SIZE_LEFT, "0x%02X=", (U16)Operand->Value_U64); \
			if (Operand->Value_S64 >= 0 || !(Operand->Flags & OP_SIGNED)) APPEND(OPCSTR, SIZE_LEFT, "%u", (U16)Operand->Value_U64); \
			/*else APPEND(OPCSTR, SIZE_LEFT, "-0x%02X=%d", (U16)-Operand->Value_S64, (S16)Operand->Value_S64);*/ \
			else APPEND(OPCSTR, SIZE_LEFT, "%d", (S16)Operand->Value_S64); \
			break; \
		case 1: \
			APPEND(OPCSTR, SIZE_LEFT, "0x%02X=", (U8)Operand->Value_U64); \
			if (Operand->Value_S64 >= 0 || !(Operand->Flags & OP_SIGNED)) APPEND(OPCSTR, SIZE_LEFT, "%u", (U8)Operand->Value_U64); \
			/*else APPEND(OPCSTR, SIZE_LEFT, "-0x%02X=%d", (U8)-Operand->Value_S64, (S8)Operand->Value_S64);*/ \
			else APPEND(OPCSTR, SIZE_LEFT, "%d", (S8)Operand->Value_S64); \
			break; \
		default: assert(0); break; \
	} \
}

#define X86_WRITE_ABSOLUTE_DISPLACEMENT() \
{  \
	switch (X86Instruction->AddressSize) \
	{ \
		case 8: \
			APPEND(OPCSTR, SIZE_LEFT, "0x%04I64X", X86Instruction->Displacement); \
			break; \
		case 4: \
			APPEND(OPCSTR, SIZE_LEFT, "0x%04lX", (U32)X86Instruction->Displacement); \
			break; \
		case 2: \
			APPEND(OPCSTR, SIZE_LEFT, "0x%04X", (U16)X86Instruction->Displacement); \
			break; \
		default: assert(0); break; \
	} \
}

#define X86_WRITE_RELATIVE_DISPLACEMENT64() \
	if (X86Instruction->Displacement >= 0) APPEND(OPCSTR, SIZE_LEFT, "+0x%02I64X", X86Instruction->Displacement); \
	else APPEND(OPCSTR, SIZE_LEFT, "-0x%02I64X", -X86Instruction->Displacement);

#define X86_WRITE_RELATIVE_DISPLACEMENT32() \
	if (X86Instruction->Displacement >= 0) APPEND(OPCSTR, SIZE_LEFT, "+0x%02lX", (U32)X86Instruction->Displacement); \
	else APPEND(OPCSTR, SIZE_LEFT, "-0x%02lX", (U32)-X86Instruction->Displacement);

#define X86_WRITE_RELATIVE_DISPLACEMENT16() \
	if (X86Instruction->Displacement >= 0) APPEND(OPCSTR, SIZE_LEFT, "+0x%02X", (U16)X86Instruction->Displacement); \
	else APPEND(OPCSTR, SIZE_LEFT, "-0x%02X", (U16)-X86Instruction->Displacement);

#define X86_WRITE_RELATIVE_DISPLACEMENT() \
{  \
	switch (X86Instruction->AddressSize) \
	{ \
		case 8: \
			X86_WRITE_RELATIVE_DISPLACEMENT64() \
			break; \
		case 4: \
			X86_WRITE_RELATIVE_DISPLACEMENT32() \
			break; \
		case 2: \
			X86_WRITE_RELATIVE_DISPLACEMENT16() \
			break; \
		default: assert(0); break; \
	} \
}

#define X86_WRITE_IP_OFFSET(op) \
{ \
	switch (X86Instruction->OperandSize) \
	{ \
		case 8: \
			APPENDS("[rip+ilen"); \
			assert((op)->TargetAddress); \
			X86_WRITE_RELATIVE_DISPLACEMENT64() \
			APPEND(OPCSTR, SIZE_LEFT, "]=0x%04I64X", (op)->TargetAddress+Instruction->VirtualAddressDelta); \
			break; \
		case 4: \
			APPENDS("[eip+ilen"); \
			assert((op)->TargetAddress); \
			X86_WRITE_RELATIVE_DISPLACEMENT32() \
			APPEND(OPCSTR, SIZE_LEFT, "]=0x%04lX", (U32)((op)->TargetAddress+Instruction->VirtualAddressDelta)); \
			break; \
		case 2: \
			APPENDS("[ip+ilen"); \
			X86_WRITE_RELATIVE_DISPLACEMENT16() \
			APPEND(OPCSTR, SIZE_LEFT, "]=0x%04X", (U16)((op)->TargetAddress+Instruction->VirtualAddressDelta)); \
			break; \
		default: assert(0); break; \
	} \
}

#define X86_WRITE_OFFSET(op) \
{ \
	assert((op)->Length <= 8); \
	if (X86Instruction->HasSelector) \
	{ \
		assert((op)->Flags & OP_FAR); \
		APPEND(OPCSTR, SIZE_LEFT, "%s 0x%02X:[", DataSizes[((op)->Length >> 1)], X86Instruction->Selector); \
	} \
	else \
	{ \
		assert(!((op)->Flags & OP_FAR)); \
		assert(X86Instruction->Segment < SEG_MAX) ; \
		APPEND(OPCSTR, SIZE_LEFT, "%s %s:[", DataSizes[((op)->Length >> 1)], Segments[X86Instruction->Segment]); \
	} \
	X86_WRITE_ABSOLUTE_DISPLACEMENT() \
	APPENDB(']'); \
}

static void OutputAddress(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	BOOL ShowDisplacement = FALSE;
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;

	assert(!X86Instruction->HasSelector);
	assert(X86Instruction->SrcAddressIndex == OperandIndex || X86Instruction->DstAddressIndex == OperandIndex);
	if (Operand->Length > 16 || (Operand->Length > 1 && (Operand->Length & 1))) APPEND(OPCSTR, SIZE_LEFT, "%d_byte ptr ", Operand->Length);
	else APPEND(OPCSTR, SIZE_LEFT, "%s ", DataSizes[Operand->Length >> 1]);

	//
	// This attempts to display the address intelligently
	// If it has a positive 32-bit displacement, it is shown as seg:Displacement[base+index*scale]
	// If it is a negative displacement or 8-bit, it is shown as seg:[base+index*scale+displacement]
	//
	APPEND(OPCSTR, SIZE_LEFT, "%s:", Segments[X86Instruction->Segment]);
	if (X86Instruction->HasBaseRegister)
	{
		if (X86Instruction->Displacement)
		{
			if (X86Instruction->HasFullDisplacement) X86_WRITE_ABSOLUTE_DISPLACEMENT()
			else ShowDisplacement = TRUE;
		}
		APPEND(OPCSTR, SIZE_LEFT, "[%s", X86_Registers[X86Instruction->BaseRegister]);
		if (X86Instruction->HasIndexRegister)
		{
			APPEND(OPCSTR, SIZE_LEFT, "+%s", X86_Registers[X86Instruction->IndexRegister]);
			if (X86Instruction->Scale > 1) APPEND(OPCSTR, SIZE_LEFT, "*%d", X86Instruction->Scale);
		}
		if (ShowDisplacement) X86_WRITE_RELATIVE_DISPLACEMENT()
		APPENDB(']');
		if (X86Instruction->Relative)
		{
			U64 Address = Operand->TargetAddress;
			assert(Address);
			APPLY_OFFSET(Address)
			APPEND(OPCSTR, SIZE_LEFT, "=[0x%04I64X]", Address);
		}
	}
	else if (X86Instruction->HasIndexRegister)
	{
		if (X86Instruction->Displacement)
		{
			if (X86Instruction->HasFullDisplacement) X86_WRITE_ABSOLUTE_DISPLACEMENT()
			else ShowDisplacement = TRUE;
		}
		APPEND(OPCSTR, SIZE_LEFT, "[%s", X86_Registers[X86Instruction->IndexRegister]);
		if (X86Instruction->Scale > 1) APPEND(OPCSTR, SIZE_LEFT, "*%d", X86Instruction->Scale);
		if (ShowDisplacement) X86_WRITE_RELATIVE_DISPLACEMENT()
		APPENDB(']');
	}
	else // just a displacement
	{
		APPENDB('[');
		X86_WRITE_ABSOLUTE_DISPLACEMENT()
		APPENDB(']');
	}
}

static void OutputBounds(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	assert(X86Instruction->HasSrcAddressing);
	assert(!(Operand->Length & 1));
	Operand->Length >>= 1;
	APPENDB('(');
	OutputAddress(Instruction, Operand, OperandIndex);
	APPENDS(", ");
	X86Instruction->Displacement += Operand->Length;
	OutputAddress(Instruction, Operand, OperandIndex);
	X86Instruction->Displacement -= Operand->Length;
	APPENDB(')');
	Operand->Length <<= 1;
}

static void OutputGeneral(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	if ((X86Instruction->HasDstAddressing && X86Instruction->DstAddressIndex == OperandIndex) ||
		(X86Instruction->HasSrcAddressing && X86Instruction->SrcAddressIndex == OperandIndex))
	{
		OutputAddress(Instruction, Operand, OperandIndex);
	}
	else
	{
		APPENDS(X86_Registers[Operand->Register]);
	}
}

static void OutputDescriptor(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	assert(X86Instruction->HasSrcAddressing || X86Instruction->HasDstAddressing);
	OutputAddress(Instruction, Operand, OperandIndex);
}

static void OutputPackedReal(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	if ((X86Instruction->HasDstAddressing && X86Instruction->DstAddressIndex == OperandIndex) ||
		(X86Instruction->HasSrcAddressing && X86Instruction->SrcAddressIndex == OperandIndex))
	{
		OutputAddress(Instruction, Operand, OperandIndex);
	}
	else
	{
		APPENDS(X86_Registers[Operand->Register]);
	}
}

static void OutputPackedBCD(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	if ((X86Instruction->HasDstAddressing && X86Instruction->DstAddressIndex == OperandIndex) ||
		(X86Instruction->HasSrcAddressing && X86Instruction->SrcAddressIndex == OperandIndex))
	{
		OutputAddress(Instruction, Operand, OperandIndex);
	}
	else
	{
		APPENDS(X86_Registers[Operand->Register]);
	}
}

static void OutputScalarReal(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	if ((X86Instruction->HasDstAddressing && X86Instruction->DstAddressIndex == OperandIndex) ||
		(X86Instruction->HasSrcAddressing && X86Instruction->SrcAddressIndex == OperandIndex))
	{
		OutputAddress(Instruction, Operand, OperandIndex);
	}
	else
	{
		APPENDS(X86_Registers[Operand->Register]);
	}
}

static void OutputScalarGeneral(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	if (Operand->Type == OPTYPE_FLOAT)
	{
		OutputScalarReal(Instruction, Operand, OperandIndex);
	}
	else
	{
		OutputGeneral(Instruction, Operand, OperandIndex);
	}
}

static void OutputFPUEnvironment(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	assert(X86Instruction->HasSrcAddressing || X86Instruction->HasDstAddressing);
	OutputAddress(Instruction, Operand, OperandIndex);
}

static void OutputFPUState(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	assert(X86Instruction->HasSrcAddressing || X86Instruction->HasDstAddressing);
	OutputAddress(Instruction, Operand, OperandIndex);
}

static void OutputCPUState(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	assert(X86Instruction->HasSrcAddressing);
	OutputAddress(Instruction, Operand, OperandIndex);
}

static void OutputSegOffset(INSTRUCTION *Instruction, INSTRUCTION_OPERAND *Operand, U32 OperandIndex)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	assert(X86Instruction->HasSrcAddressing);
	OutputAddress(Instruction, Operand, OperandIndex);	
}

////////////////////////////////////////////////////////////
// Prologue support
////////////////////////////////////////////////////////////
/*
typedef struct _PROLOGUE
{
	char *Data;
	U32 Length;
} PROLOGUE;
*/
static PROLOGUE StandardPrologues[] =
{
	{ "\x55\x8b\xec", 3 },
	{ "\x55\x89\xe5", 3 },
	{ "\x83\xec", 2 },
	{ "\x81\xec", 2 },
	// TODO: add AMD64 prologues
	// TODO: add VS2003/VS2003 prologues
	// TODO: add any unique prologues from other compilers
	{ NULL, 0 }
};

// Find the first function between StartAddress and EndAddress
// 
// This will match a standard prologue and then analyze the following instructions to verify
// it is a valid function
static U8 *X86_FindFunctionByPrologue(INSTRUCTION *Instruction, U8 *StartAddress, U8 *EndAddress, U32 Flags)
{
	assert(0); // TODO
	return NULL;
}

//////////////////////////////////////////////////////////
// Instruction decoder
//////////////////////////////////////////////////////////

static BOOL X86_GetInstruction(INSTRUCTION *Instruction, U8 *Address, U32 Flags)
{
	BOOL SpecialExtension = FALSE;
	U8 Opcode = 0, OpcodeExtension = 0, Group = 0, SSE_Prefix = 0, Suffix;
	U32 i = 0, Result = 0, tmpScale;
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	X86_OPCODE *X86Opcode;
#ifdef TEST_DISASM
	U32 InstructionLength = 0;
#endif
	INSTRUCTION_OPERAND *Operand, *Operand1 = NULL;
	DISASSEMBLER *Disassembler = Instruction->Disassembler;
	BOOL Decode = Flags & DISASM_DECODE;
	BOOL Disassemble = Flags & DISASM_DISASSEMBLE;
	BOOL SuppressErrors = Flags & DISASM_SUPPRESSERRORS;

	if (Disassemble && !Decode)
	{
		assert(0);
		Decode = TRUE;
	}

	if (!Address || !X86_InitInstruction(Instruction))
	{
		assert(0);
		goto abort;
	}

	assert(Instruction->Address == Address);
	assert(!Instruction->StringIndex && !Instruction->Length);

	Disassembler->Stage1Count++;
	if (Flags & DISASM_ALIGNOUTPUT) Instruction->StringAligned = TRUE;

	//
	// Get prefixes or three byte opcode
	//
	while (TRUE)
	{
		Opcode = *Address;
		INSTR_INC(1); // increment Instruction->Length and address
		X86Opcode = &X86_Opcodes_1[Opcode];

		// Handle a misplaced REX prefix -- AMD64 manual says it is just ignored
		if (IS_AMD64() && (Opcode >= REX_PREFIX_START && Opcode <= REX_PREFIX_END) && X86_PREFIX((&X86_Opcodes_1[*Address])))
		{
			if (!Instruction->AnomalyOccurred)
			{
				if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: REX prefix before legacy prefix 0x%02X\n", VIRTUAL_ADDRESS, Opcode);
				Instruction->AnomalyOccurred = TRUE;
			}
			continue;
		}
		
		if (X86_PREFIX(X86Opcode))
		{
			if (!Instruction->AnomalyOccurred)
			{
				for (i = 0; i < Instruction->PrefixCount; i++)
				{
					if (Instruction->Prefixes[i] == Opcode)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Duplicate prefix 0x%02X\n", VIRTUAL_ADDRESS, Opcode);
						Instruction->AnomalyOccurred = TRUE;
						break;
					}
				}
			}

			switch (Opcode)
			{
				case PREFIX_REPNE: // may be three byte opcode
					SSE_Prefix = Opcode;
					if (!Instruction->AnomalyOccurred && X86Instruction->HasRepeatWhileEqualPrefix)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Conflicting prefix\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					Instruction->Repeat = TRUE;
					X86Instruction->HasRepeatWhileEqualPrefix = FALSE;
					X86Instruction->HasRepeatWhileNotEqualPrefix = TRUE;
					break;
				case PREFIX_REP: // may be three byte opcode
					SSE_Prefix = Opcode;
					if (!Instruction->AnomalyOccurred && X86Instruction->HasRepeatWhileNotEqualPrefix)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Conflicting prefix\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}

					Instruction->Repeat = TRUE;
					X86Instruction->HasRepeatWhileNotEqualPrefix = FALSE;
					X86Instruction->HasRepeatWhileEqualPrefix = TRUE;
					break;

				case PREFIX_OPERAND_SIZE: // may be three byte opcode
					SSE_Prefix = Opcode;
					if (!Instruction->AnomalyOccurred && X86Instruction->HasOperandSizePrefix)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Conflicting prefix\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					
					if (X86Instruction->HasOperandSizePrefix) break;
					else X86Instruction->HasOperandSizePrefix = TRUE;
					switch (X86Instruction->OperandSize)
					{
						case 4: X86Instruction->OperandSize = 2; break;
						case 2: X86Instruction->OperandSize = 4; break;
						default: assert(0); goto abort;
					}
					break;

				case PREFIX_ADDRESS_SIZE:
					if (!Instruction->AnomalyOccurred && X86Instruction->HasAddressSizePrefix)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Conflicting prefix\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}

					if (X86Instruction->HasAddressSizePrefix) break;
					else X86Instruction->HasAddressSizePrefix = TRUE;
					switch (X86Instruction->AddressSize)
					{
						case 8:
							X86Instruction->AddressSize = 4;
							break;
						case 4:
							assert(!IS_AMD64()); // this should not be possible
							X86Instruction->AddressSize = 2;
							break;
						case 2:
							X86Instruction->AddressSize = 4;
							break;
						default: 
							assert(0); goto abort;
					}
					break;

				case PREFIX_SEGMENT_OVERRIDE_ES:
					SANITY_CHECK_SEGMENT_OVERRIDE();
					if (!IS_AMD64())
					{
						X86Instruction->HasSegmentOverridePrefix = TRUE;
						X86Instruction->Segment = SEG_ES;
					}
					else if (!Instruction->AnomalyOccurred)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Meaningless segment override\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					break;
				case PREFIX_SEGMENT_OVERRIDE_CS:
					SANITY_CHECK_SEGMENT_OVERRIDE();
					if (!IS_AMD64())
					{
						X86Instruction->HasSegmentOverridePrefix = TRUE;
						X86Instruction->Segment = SEG_CS;  
					}
					else if (!Instruction->AnomalyOccurred)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Meaningless segment override\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					break;
				case PREFIX_SEGMENT_OVERRIDE_SS:
					SANITY_CHECK_SEGMENT_OVERRIDE();
					if (!IS_AMD64())
					{
						X86Instruction->HasSegmentOverridePrefix = TRUE;
						X86Instruction->Segment = SEG_SS;
					}
					else if (!Instruction->AnomalyOccurred)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Meaningless segment override\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					break;
				case PREFIX_SEGMENT_OVERRIDE_DS:
					SANITY_CHECK_SEGMENT_OVERRIDE();
					if (!IS_AMD64())
					{
						X86Instruction->HasSegmentOverridePrefix = TRUE;
						X86Instruction->Segment = SEG_DS;
					}
					else if (!Instruction->AnomalyOccurred)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Meaningless segment override\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					break;
				case PREFIX_SEGMENT_OVERRIDE_FS:
					SANITY_CHECK_SEGMENT_OVERRIDE();
					X86Instruction->HasSegmentOverridePrefix = TRUE;
					X86Instruction->Segment = SEG_FS;
					break;
				case PREFIX_SEGMENT_OVERRIDE_GS:
					SANITY_CHECK_SEGMENT_OVERRIDE();
					X86Instruction->HasSegmentOverridePrefix = TRUE;
					X86Instruction->Segment = SEG_GS;
					break;

				case PREFIX_LOCK:
					if (!Instruction->AnomalyOccurred && X86Instruction->HasLockPrefix)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Conflicting prefix\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					X86Instruction->HasLockPrefix = TRUE;
					break;

				default:
					assert(0);
					goto abort;
			}

			if (Instruction->PrefixCount >= X86_MAX_INSTRUCTION_LEN)
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Reached maximum prefix count %d\n", VIRTUAL_ADDRESS, X86_MAX_PREFIX_LENGTH);
				goto abort;
			}
			else if (Instruction->PrefixCount == X86_MAX_PREFIX_LENGTH)
			{
				if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Reached maximum prefix count %d\n", VIRTUAL_ADDRESS, X86_MAX_PREFIX_LENGTH);
				Instruction->AnomalyOccurred = TRUE;
			}

			assert(Instruction->AnomalyOccurred || Instruction->PrefixCount < X86_MAX_PREFIX_LENGTH);
			Instruction->Prefixes[Instruction->PrefixCount] = Opcode;
			Instruction->PrefixCount++;
			//DISASM_OUTPUT(("[0x%08I64X] Prefix 0x%02X (prefix count %d)\n", VIRTUAL_ADDRESS, Opcode, Instruction->PrefixCount));
		}
		else
		{
			break;
		}
	}

	// Check for REX opcode
	// This is checked here instead of the prefix loop above because it must be the
	// last prefix
	if (IS_AMD64() && (Opcode >= REX_PREFIX_START && Opcode <= REX_PREFIX_END))
	{
		if (Instruction->PrefixCount >= X86_MAX_INSTRUCTION_LEN)
		{
			if (!SuppressErrors) printf("[0x%08I64X] ERROR: Reached maximum prefix count %d\n", VIRTUAL_ADDRESS, X86_MAX_PREFIX_LENGTH);
			goto abort;
		}
		else if (!Instruction->AnomalyOccurred && Instruction->PrefixCount == AMD64_MAX_PREFIX_LENGTH)
		{
			if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Reached maximum prefix count %d\n", VIRTUAL_ADDRESS, X86_MAX_PREFIX_LENGTH);
			Instruction->AnomalyOccurred = TRUE;
		}

		assert(Instruction->AnomalyOccurred || Instruction->PrefixCount < AMD64_MAX_PREFIX_LENGTH);

		Instruction->Prefixes[Instruction->PrefixCount] = Opcode;
		Instruction->PrefixCount++;
		X86Instruction->rex_b = Opcode;
		SET_REX(X86Instruction->rex, X86Instruction->rex_b);
		DISASM_OUTPUT(("[0x%08I64X] REX prefix 0x%02X (prefix count %d, w=%d, r=%d, x=%d, b=%d)\n", VIRTUAL_ADDRESS, Opcode, Instruction->PrefixCount, X86Instruction->rex.w, X86Instruction->rex.r, X86Instruction->rex.x, X86Instruction->rex.b));

		assert(X86Instruction->AddressSize >= 4);
		if (X86Instruction->rex.w)
		{
			X86Instruction->OperandSize = 8;
			X86Instruction->HasOperandSizePrefix = FALSE;
		}
		else if (X86Instruction->HasOperandSizePrefix)
		{
			assert(X86Instruction->OperandSize == 2);
		}
		else if (X86Instruction->rex_b == REX_PREFIX_START)
		{
			if (!Instruction->AnomalyOccurred)
			{
				if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: meaningless REX prefix used\n", VIRTUAL_ADDRESS);
				Instruction->AnomalyOccurred = TRUE;
			}
			X86Instruction->rex_b = 0;
		}

		Opcode = *Address;
		INSTR_INC(1); // increment Instruction->Length and address

		X86Opcode = &X86_Opcodes_1[Opcode];
		assert(!X86_PREFIX(X86Opcode));
	}
	//DISASM_OUTPUT(("[0x%08I64X] OperandSize = %d, AddressSize = %d\n", VIRTUAL_ADDRESS, X86Instruction->OperandSize, X86Instruction->AddressSize));
	Instruction->LastOpcode = Opcode;
	Instruction->OpcodeAddress = Address-1;

	if (X86_INVALID(X86Opcode))
	{
		if (!SuppressErrors) printf("[0x%08I64X] ERROR: Invalid opcode 0x%02X\n", VIRTUAL_ADDRESS, Opcode);
		goto abort;
	}

	if (Opcode == X86_TWO_BYTE_OPCODE)
	{
		//
		// Handle case that it is a group (with opcode extension), floating point, or two byte opcode
		//
		assert(!Instruction->OpcodeLength);
		Instruction->LastOpcode = Opcode = *Address;
		INSTR_INC(1); // increment Instruction->Length and address
		assert(X86Opcode->Table == X86_Opcodes_2);
		X86Opcode = &X86_Opcodes_2[Opcode];

		//
		// Check for errors
		//
		if (X86_INVALID(X86Opcode))
		{
			if (!SuppressErrors) printf("[0x%08I64X] ERROR: Invalid two byte opcode 0x%02X 0x%02X\n", VIRTUAL_ADDRESS, X86_TWO_BYTE_OPCODE, Opcode);
			goto abort;
		}
		
		if (X86Instruction->AddressSize == 8)
		{
			if (X86_Invalid_Addr64_2[Opcode])
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Opcode 0x%02X 0x%02X (\"%s\") illegal in 64-bit mode\n", VIRTUAL_ADDRESS, X86_TWO_BYTE_OPCODE, Opcode, X86Opcode->Mnemonic);
				goto abort;
			}
#if 0
			if (X86Instruction->rex_b &&
					(GET_REX_B(X86Instruction->rex_b) && !GET_REX_B(X86_REX_2[Opcode]) ||
					 GET_REX_X(X86Instruction->rex_b) && !GET_REX_X(X86_REX_2[Opcode]) ||
					 GET_REX_R(X86Instruction->rex_b) && !GET_REX_R(X86_REX_2[Opcode]) ||
					 GET_REX_W(X86Instruction->rex_b) && !GET_REX_W(X86_REX_2[Opcode])))
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Illegal REX prefix 0x%02X for opcode 0x%02X 0x%02X\n", VIRTUAL_ADDRESS, X86Instruction->rex_b, X86_TWO_BYTE_OPCODE, Opcode);
				assert(0);
				goto abort;
			}
#endif
		}

		if (X86Instruction->OperandSize == 2 && X86_Invalid_Op16_2[Opcode])
		{
			if (!SuppressErrors) printf("[0x%08I64X] ERROR: Opcode 0x%02X 0x%02X (\"%s\") illegal with 16-bit operand size\n", VIRTUAL_ADDRESS, X86_TWO_BYTE_OPCODE, Opcode, X86Opcode->Mnemonic);
			goto abort;
		}

		X86Instruction->HasModRM = X86_ModRM_2[Opcode];
		if (X86Instruction->HasModRM) X86Instruction->modrm_b = *Address;
		Instruction->OpcodeBytes[0] = X86_TWO_BYTE_OPCODE;
		Instruction->OpcodeBytes[1] = Opcode;
		Instruction->OpcodeLength = 2;

		if (X86_SPECIAL_EXTENSION(X86Opcode))
		{
			DISASM_OUTPUT(("[0x%08I64X] Special opcode extension 0x%02X 0x%02X\n", VIRTUAL_ADDRESS, X86_TWO_BYTE_OPCODE, Opcode));
			SpecialExtension = TRUE;
			goto HasSpecialExtension;
		}
		else if (SSE_Prefix && !X86_INVALID(&X86_SSE[Opcode])) // SSEx instruction
		{
			Instruction->OpcodeLength = 3;
			Instruction->OpcodeBytes[2] = SSE_Prefix;
			assert(Instruction->OpcodeBytes[1] == Opcode);

			// Since the prefix was really an opcode extension, remove it from
			// the prefix list
			for (i = 0; i < Instruction->PrefixCount; i++)
			{
				if (Instruction->Prefixes[i]) break;
			}
			assert(i != Instruction->PrefixCount);
			Instruction->PrefixCount--;
			Instruction->Prefixes[i] = 0;

			// Slide any prefixes following the removed prefix down by 1
			memmove(&Instruction->Prefixes[i], &Instruction->Prefixes[i+1], Instruction->PrefixCount-i);
			Instruction->Prefixes[Instruction->PrefixCount] = 0;
			Instruction->Repeat = FALSE;
			X86Instruction->HasRepeatWhileEqualPrefix = FALSE;
			X86Instruction->HasRepeatWhileNotEqualPrefix = FALSE;
			X86Instruction->HasOperandSizePrefix = FALSE;
			if (SSE_Prefix == PREFIX_OPERAND_SIZE)
			{
				if (IS_AMD64() && X86Instruction->rex.w) X86Instruction->OperandSize = 8;
				else X86Instruction->OperandSize = 4;
			}

			if (IS_X86_16())
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: SSE invalid in 16-bit mode\n", VIRTUAL_ADDRESS);
				goto abort;
			}
		
			assert(X86Instruction->HasModRM);
			switch (SSE_Prefix)
			{
				case PREFIX_OPERAND_SIZE: X86Opcode = &X86_SSE[0x000+Opcode]; break;
				case PREFIX_REPNE: X86Opcode = &X86_SSE[0x100+Opcode]; break;
				case PREFIX_REP: X86Opcode = &X86_SSE[0x200+Opcode]; break;
			}

			if (X86_INVALID(X86Opcode))
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Illegal SSE instruction opcode 0x%02X 0x%02X + prefix 0x%02X\n", VIRTUAL_ADDRESS, Instruction->OpcodeBytes[0], Instruction->OpcodeBytes[1], Instruction->OpcodeBytes[2]);
				goto abort;
			}
			else if (X86_EXTENDED_OPCODE(X86Opcode))
			{
				// SSE in group (13, 14, or 15)
				OpcodeExtension = GET_MODRM_EXT(X86Instruction->modrm_b);
				Group = X86_Groups_2[Opcode];
				X86Instruction->Group = (U8)Group;
				assert(Group >= 13 && Group <= 15 && X86Opcode->Table);
				switch (SSE_Prefix)
				{
					case PREFIX_OPERAND_SIZE: X86Opcode = &X86Opcode->Table[0x00+OpcodeExtension]; break;
					case PREFIX_REPNE: X86Opcode = &X86Opcode->Table[0x08+OpcodeExtension]; break;
					case PREFIX_REP: X86Opcode = &X86Opcode->Table[0x10+OpcodeExtension]; break;
				}

				if (X86_INVALID(X86Opcode))
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: Illegal SSE instruction opcode 0x%02X 0x%02X + prefix 0x%02X + extension %d\n", VIRTUAL_ADDRESS, Instruction->OpcodeBytes[0], Instruction->OpcodeBytes[1], Instruction->OpcodeBytes[2], OpcodeExtension);
					goto abort;
				}
			}

			Instruction->Repeat = FALSE;
			X86Instruction->HasRepeatWhileEqualPrefix = FALSE;
			X86Instruction->HasRepeatWhileNotEqualPrefix = FALSE;
			X86Instruction->HasOperandSizePrefix = FALSE;
			switch (X86_GET_CATEGORY(X86Opcode))
			{
				case ITYPE_SSE: case ITYPE_SSE2: case ITYPE_SSE3: break;
				default: assert(0); goto abort;
			}
		}
		else if (X86_EXTENDED_OPCODE(X86Opcode)) // 2 byte group
		{
			assert(!X86Opcode->MnemonicFlags);
			OpcodeExtension = GET_MODRM_EXT(X86Instruction->modrm_b);

			assert(X86Opcode->Table);
			X86Opcode = &X86Opcode->Table[OpcodeExtension];
			if (X86_INVALID(X86Opcode))
			{
				Instruction->Length++;
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Invalid group opcode 0x%02X 0x%02X extension 0x%02X\n", VIRTUAL_ADDRESS, X86_TWO_BYTE_OPCODE, Opcode, OpcodeExtension);
				goto abort;
			}

			assert(!X86_SPECIAL_EXTENSION(X86Opcode));
			Group = X86_Groups_2[Opcode];
			X86Instruction->Group = (U8)Group;
			assert(Group > 0 && Group <= 19);
			assert(X86Opcode->Mnemonic);
			DISASM_OUTPUT(("[0x%08I64X] Group %d (bytes 0x%02X 0x%02X) extension 0x%02X (\"%s\")\n", VIRTUAL_ADDRESS, Group, X86_TWO_BYTE_OPCODE, Opcode, OpcodeExtension, X86Opcode->Mnemonic));
		}
		else
		{
			assert(X86Opcode->Mnemonic);
			DISASM_OUTPUT(("[0x%08I64X] Two byte opcode 0x%02X 0x%02X (\"%s\")\n", VIRTUAL_ADDRESS, X86_TWO_BYTE_OPCODE, Opcode, X86Opcode->Mnemonic));
			X86Instruction->HasModRM = X86_ModRM_2[Opcode];
			if (X86Instruction->HasModRM) X86Instruction->modrm_b = *Address;
		}
	}
	else // 1-byte opcode
	{
		if (X86Instruction->AddressSize == 8)
		{
			if (X86_Invalid_Addr64_1[Opcode])
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Opcode 0x%02X (\"%s\") illegal in 64-bit mode\n", VIRTUAL_ADDRESS, Opcode, X86Opcode->Mnemonic);
				goto abort;
			}

#if 0
			if (X86Instruction->rex_b &&
				(GET_REX_B(X86Instruction->rex_b) && !GET_REX_B(X86_REX_1[Opcode]) ||
				 GET_REX_X(X86Instruction->rex_b) && !GET_REX_X(X86_REX_1[Opcode]) ||
				 GET_REX_R(X86Instruction->rex_b) && !GET_REX_R(X86_REX_1[Opcode]) ||
				 GET_REX_W(X86Instruction->rex_b) && !GET_REX_W(X86_REX_1[Opcode])))
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Illegal REX prefix 0x%02X for opcode 0x%02X\n", VIRTUAL_ADDRESS, X86Instruction->rex_b, Opcode);
				assert(0);
				goto abort;
			}
#endif
		}

		if (X86Instruction->OperandSize == 2 && X86_Invalid_Op16_1[Opcode])
		{
			if (!SuppressErrors) printf("[0x%08I64X] ERROR: Opcode 0x%02X (\"%s\") illegal with 16-bit operand size\n", VIRTUAL_ADDRESS, Opcode, X86Opcode->Mnemonic);
			goto abort;
		}

		Instruction->OpcodeBytes[0] = Opcode;
		Instruction->OpcodeLength = 1;
		X86Instruction->HasModRM = X86_ModRM_1[Opcode];
		if (X86Instruction->HasModRM) X86Instruction->modrm_b = *Address;

		if (X86_EXTENDED_OPCODE(X86Opcode)) // a group
		{
			assert(X86Instruction->HasModRM);
			OpcodeExtension = GET_MODRM_EXT(*Address); // leave Address pointing at ModRM byte

			if (X86_SPECIAL_EXTENSION(X86Opcode))
			{
				DISASM_OUTPUT(("[0x%08I64X] Special opcode extension 0x%02X 0x%02X\n", VIRTUAL_ADDRESS, X86_TWO_BYTE_OPCODE, Opcode));
				SpecialExtension = TRUE;
				goto HasSpecialExtension;
			}

			assert(X86Opcode->Table);
			X86Opcode = &X86Opcode->Table[OpcodeExtension];
			if (X86_INVALID(X86Opcode))
			{
				Instruction->Length++;
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Invalid group opcode 0x%02X extension 0x%02X\n", VIRTUAL_ADDRESS, Opcode, OpcodeExtension);
				goto abort;
			}

			Group = X86_Groups_1[Opcode];
			X86Instruction->Group = (U8)Group;
			DISASM_OUTPUT(("[0x%08I64X] Group %d (byte 0x%02X) extension 0x%02X (\"%s\")\n", VIRTUAL_ADDRESS, Group, Opcode, OpcodeExtension, X86Opcode->Mnemonic));
			assert(Group > 0 && Group <= 17);
			assert(X86Opcode->Mnemonic);
		}
		else
		{
			if (X86_SPECIAL_EXTENSION(X86Opcode))
			{
				DISASM_OUTPUT(("[0x%08I64X] Special opcode extension 0x%02X\n", VIRTUAL_ADDRESS, Opcode));
				SpecialExtension = TRUE;
				goto HasSpecialExtension;
			}

			DISASM_OUTPUT(("[0x%08I64X] One byte opcode 0x%02X (\"%s\")\n", VIRTUAL_ADDRESS, Opcode, X86Opcode->Mnemonic));
		}
	}

HasSpecialExtension:
	if (SpecialExtension)
	{
		if (X86Opcode->MnemonicFlags & ITYPE_EXT_MODRM)
		{
			assert(X86Opcode->Table);
			assert(Instruction->OpcodeLength == 2);
			assert(X86Instruction->HasModRM);
			X86Opcode = &X86Opcode->Table[*Address];
			if (X86_INVALID(X86Opcode))
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Illegal opcode 0x%02X 0x%02X + modrm 0x%02X\n", VIRTUAL_ADDRESS, Instruction->OpcodeBytes[0], Instruction->OpcodeBytes[1], *Address);
				goto abort;
			}
			else if (X86_EXTENDED_OPCODE(X86Opcode))
			{
				assert(!X86Opcode->MnemonicFlags);
				OpcodeExtension = GET_MODRM_EXT(X86Instruction->modrm_b);

				assert(X86Opcode->Table);
				X86Opcode = &X86Opcode->Table[OpcodeExtension];
				if (X86_INVALID(X86Opcode))
				{
					Instruction->Length++;
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: Invalid group opcode 0x%02X 0x%02X extension 0x%02X\n", VIRTUAL_ADDRESS, X86_TWO_BYTE_OPCODE, Opcode, OpcodeExtension);
					goto abort;
				}

				assert(!X86_SPECIAL_EXTENSION(X86Opcode));
				Group = X86_Groups_2[Opcode];
				X86Instruction->Group = (U8)Group;
				assert(Group > 0 && Group <= 19);
				assert(X86Opcode->Mnemonic);
				DISASM_OUTPUT(("[0x%08I64X] Group %d (bytes 0x%02X 0x%02X) extension 0x%02X (\"%s\")\n", VIRTUAL_ADDRESS, Group, X86_TWO_BYTE_OPCODE, Opcode, OpcodeExtension, X86Opcode->Mnemonic));
			}
			else if (!X86_OPERAND_COUNT(X86Opcode))
			{
				INSTR_INC(1); // increment Instruction->Length and address
			}
		}
		else if (X86Opcode->MnemonicFlags & ITYPE_EXT_FPU)
		{
			assert(X86Opcode->Table);		
			if (X86Instruction->modrm_b < 0xC0)
			{
				// It is an opcode extension, use the X86Opcode->Table
				OpcodeExtension = GET_MODRM_EXT(X86Instruction->modrm_b);
				X86Opcode = &X86Opcode->Table[OpcodeExtension];
			}
			else
			{
				// The whole ModRM byte is used, these start at index 0x08 in X86Opcode->Table
				OpcodeExtension = (X86Instruction->modrm_b & 0x3F);
				X86Opcode = &X86Opcode->Table[0x08 + OpcodeExtension];
			}

			if (X86_INVALID(X86Opcode))
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Invalid FPU opcode 0x%02X + modrm extension 0x%02X (index 0x%02X)\n", VIRTUAL_ADDRESS, Opcode, X86Instruction->modrm_b, 0x08 + OpcodeExtension);
				goto abort;
			}

			DISASM_OUTPUT(("[0x%08I64X] FPU instruction is (\"%s\"): 0x%02X + modrm 0x%02X (index 0x%02X)\n", VIRTUAL_ADDRESS, X86Opcode->Mnemonic, Opcode, X86Instruction->modrm_b, 0x08 + OpcodeExtension));
			if (!X86_OPERAND_COUNT(X86Opcode)) INSTR_INC(1); // increment Instruction->Length and address
		}
		else if (X86Opcode->MnemonicFlags & ITYPE_EXT_SUFFIX)
		{
			if (X86Instruction->HasOperandSizePrefix)
			{
				if (!Instruction->AnomalyOccurred && X86Opcode->Table == X86_3DNOW_0F)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: operand size prefix used with 3DNOW instruction\n", VIRTUAL_ADDRESS);
					Instruction->AnomalyOccurred = TRUE;
				}
				X86Instruction->HasOperandSizePrefix = FALSE;
				X86Instruction->OperandSize = 4;
			}
			Instruction->OperandCount = X86_OPERAND_COUNT(X86Opcode);
			assert(Instruction->OpcodeLength == 2 && X86Instruction->HasModRM && Instruction->OperandCount == 2);
			memcpy(&X86Instruction->Opcode, X86Opcode, sizeof(X86_OPCODE));
			Instruction->Operands[0].Flags = X86Opcode->OperandFlags[0] & X86_OPFLAGS_MASK;
			Instruction->Operands[1].Flags = X86Opcode->OperandFlags[1] & X86_OPFLAGS_MASK;
			Instruction->Operands[2].Flags = X86Opcode->OperandFlags[2] & X86_OPFLAGS_MASK;
			assert(Address == Instruction->Address + Instruction->Length);
			if (!SetOperands(Instruction, Address, Flags & DISASM_SUPPRESSERRORS)) goto abort;
			Suffix = Instruction->Address[Instruction->Length++];
			Instruction->OpcodeBytes[2] = Suffix;
			Instruction->OpcodeLength = 3;
			X86Opcode = &X86Opcode->Table[Suffix];
			
			if (X86_INVALID(X86Opcode))
			{
				if (!SuppressErrors) printf("[0x%08I64X] ERROR: Illegal opcode 0x%02X 0x%02X + suffix 0x%02X\n", VIRTUAL_ADDRESS, Instruction->OpcodeBytes[0], Instruction->OpcodeBytes[1], Suffix);
				goto abort;
			}
			assert(Instruction->Length >= 4 + Instruction->PrefixCount);
		}
		else if (X86Opcode->MnemonicFlags & ITYPE_EXT_64)
		{
			assert(X86Opcode->Table);
			if (IS_AMD64()) X86Opcode = &X86Opcode->Table[1];
			else X86Opcode = &X86Opcode->Table[0];
			assert(!X86_INVALID(X86Opcode));
		}
	}

	// Detect incompatibilities	
	if (IS_X86_16() && X86Opcode->CPU > CPU_I386)
	{
		if (!SuppressErrors) printf("[0x%08I64X] ERROR: Instruction \"%s\" (opcode 0x%02X) can't be used in 16-bit X86\n", VIRTUAL_ADDRESS, X86Opcode->Mnemonic, Instruction->LastOpcode);
		goto abort;
	}
	if (!IS_AMD64() && X86Opcode->CPU >= CPU_AMD64)
	{
		if (!SuppressErrors) printf("[0x%08I64X] ERROR: Instruction \"%s\" (opcode 0x%02X) can only be used in X86-64\n", VIRTUAL_ADDRESS, X86Opcode->Mnemonic, Instruction->LastOpcode);
		goto abort;
	}

	// Copy the opcode into the local structure and set the fields 
	assert(Instruction->OpcodeLength && !X86_INVALID(X86Opcode));
	memcpy(&X86Instruction->Opcode, X86Opcode, sizeof(X86_OPCODE));
	Instruction->Groups |= X86_GET_CATEGORY(X86Opcode);
	assert(Instruction->Groups);
	Instruction->Type |= X86_GET_TYPE(X86Opcode);
	assert((U32)Instruction->Type >= Instruction->Groups);
	Instruction->OperandCount = X86_OPERAND_COUNT(X86Opcode);

	//
	// Sanity check prefixes now that opcode is known and prefixes are resolved
	//

	// Instructions that implicitly reference the CS/DS can't have segment override prefixes
	switch (Instruction->Type)
	{
		case ITYPE_PUSHF: case ITYPE_POPF:
		case ITYPE_ENTER: case ITYPE_LEAVE:
			SANITY_CHECK_SEGMENT_OVERRIDE();
			X86Instruction->HasSegmentOverridePrefix = FALSE;
			X86Instruction->Segment = SEG_SS;
			break;
		case ITYPE_RET: case ITYPE_DEBUG: 
		case ITYPE_OFLOW: case ITYPE_TRAP: 
		case ITYPE_TRAPRET:
			SANITY_CHECK_SEGMENT_OVERRIDE();
			X86Instruction->HasSegmentOverridePrefix = FALSE;
			X86Instruction->Segment = SEG_CS;
			break;
	}

	// Check illegal prefixes used with FPU/MMX/SSEx
	if (Instruction->Groups & (ITYPE_FPU|ITYPE_MMX|ITYPE_SSE|ITYPE_SSE2|ITYPE_SSE3))
	{
		// Check for prefixes that produce unpredictable results
		for (i = 0; i < Instruction->PrefixCount; i++)
		{
			switch (Instruction->Prefixes[i])
			{
				case PREFIX_OPERAND_SIZE:
					switch (Instruction->Type)
					{
						case ITYPE_FSTOREENV: case ITYPE_FLOADENV: case ITYPE_FSAVE: case ITYPE_FRESTORE: continue;
						default: break;
					}

					if (!Instruction->AnomalyOccurred)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: operand size prefix used with FPU/MMX/SSEx\n", VIRTUAL_ADDRESS);
						goto abort;
					}
					X86Instruction->HasOperandSizePrefix = FALSE;
					if (X86Instruction->OperandSize == 2) X86Instruction->OperandSize = 2;
					break;

				case PREFIX_REPNE:
				case PREFIX_REP:
					if (Instruction->Groups & ITYPE_FPU) { assert(Instruction->Repeat); continue; }
					// The Intel manual says this results in unpredictable behavior -- it's not even
					// clear which SSE prefix is used as the third opcode byte in this case
					// (e.g., is it the first or last SSE prefix?)
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: rep/repne used with MMX/SSEx\n", VIRTUAL_ADDRESS);
					goto abort;

				default:
					break;
			}
		}
	}

	// Check for conflicts involving operand size
	if (IS_AMD64())
	{
		// Check for use of rex.w=1 with an operand size prefix
		if (X86Instruction->rex.w)
		{
			assert(X86Instruction->OperandSize == 8);
			for (i = 0; i < Instruction->PrefixCount; i++)
			{
				if (Instruction->Prefixes[i] == PREFIX_OPERAND_SIZE)
				{
					X86Instruction->HasOperandSizePrefix = FALSE;
					if (!Instruction->AnomalyOccurred)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: use of operand size prefix meaningless when REX.w=1\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}				
				}
			}
		}

		// Set default operand size to 64 instead of 32 for some instructions
		switch (Instruction->Type)
		{
			case ITYPE_PUSH: case ITYPE_POP: 
			case ITYPE_PUSHF: case ITYPE_POPF:
			case ITYPE_ENTER: case ITYPE_LEAVE:
			case ITYPE_CALL: case ITYPE_BRANCH:
			case ITYPE_LOOPCC: case ITYPE_RET:
				X86Instruction->HasDefault64Operand = TRUE;
				break;

			case ITYPE_SYSTEM:
				if (Instruction->OpcodeLength != 2) break;

				// lgdt/lidt/lldt/ltr
				if ((Instruction->LastOpcode == 0x00 || Instruction->LastOpcode == 0x01) && 
					(OpcodeExtension == 0x02 || OpcodeExtension == 0x03))
				{
					X86Instruction->HasDefault64Operand = TRUE;
				}
				break;

			default:
				break;
		}

		if (X86Instruction->HasDefault64Operand)
		{
			if (X86Instruction->rex.w)
			{
				if (!Instruction->AnomalyOccurred)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: use of REX.w is meaningless (default operand size is 64)\n", VIRTUAL_ADDRESS);
					Instruction->AnomalyOccurred = TRUE;
				}
				X86Instruction->rex_b &= ~8;
				X86Instruction->rex.w = 0;
			}

			if (X86Instruction->HasOperandSizePrefix)
			{
				assert(X86Instruction->OperandSize == 2);
				X86Instruction->HasDefault64Operand = FALSE;
			}
			else
			{
				assert(X86Instruction->OperandSize >= 4);
				X86Instruction->OperandSize = 8;
			}
		}
	}

	// Make sure rep/repe/repne is set correctly based on instruction
	if (Instruction->Repeat)
	{
		switch (Instruction->Type)
		{
			case ITYPE_IN:
			case ITYPE_OUT:
			case ITYPE_STRMOV:
			case ITYPE_STRSTOR:
			case ITYPE_STRLOAD:
				if (X86Instruction->HasRepeatWhileNotEqualPrefix)
				{
					if (!Instruction->AnomalyOccurred)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: REPNE should only be used with cmps/scas\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					// Treat it as just a "rep"
					X86Instruction->HasRepeatWhileNotEqualPrefix = FALSE;
					X86Instruction->HasRepeatWhileEqualPrefix = TRUE;
				}
				break;
			case ITYPE_STRCMP:
				break;
			default:
				if (!Instruction->AnomalyOccurred)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Repeat prefix used with non-string instruction\n", VIRTUAL_ADDRESS);
					Instruction->AnomalyOccurred = TRUE;
				}
				Instruction->Repeat = FALSE;
				X86Instruction->HasRepeatWhileEqualPrefix = FALSE;
				X86Instruction->HasRepeatWhileNotEqualPrefix = FALSE;
				break;
		}
	}

	if (Disassemble)
	{
		assert(!Instruction->StringIndex);
		if (X86Instruction->HasRepeatWhileEqualPrefix)
		{
			if (Instruction->Type == ITYPE_STRCMP) { APPENDS("repe "); }
			else { APPENDS("rep "); }
		}
		if (X86Instruction->HasRepeatWhileNotEqualPrefix) APPENDS("repne "); 
		if (X86Instruction->HasLockPrefix) APPENDS("lock "); 
		if (X86Instruction->HasBranchTakenPrefix) APPENDS("hinttake ");
		if (X86Instruction->HasBranchNotTakenPrefix) APPENDS("hintskip ");
		APPENDPAD(12);
		APPEND(OPCSTR, SIZE_LEFT, "%s", X86Opcode->Mnemonic);
		APPENDPAD(24);
	}

	if (Instruction->OperandCount)
	{
		Instruction->Operands[0].Flags = X86Opcode->OperandFlags[0] & X86_OPFLAGS_MASK;
		Instruction->Operands[1].Flags = X86Opcode->OperandFlags[1] & X86_OPFLAGS_MASK;
		Instruction->Operands[2].Flags = X86Opcode->OperandFlags[2] & X86_OPFLAGS_MASK;
		Address = SetOperands(Instruction, Address, Flags);
		if (!Address) goto abort;
		assert(!(Instruction->Operands[0].Flags & 0x7F));
		assert(!(Instruction->Operands[1].Flags & 0x7F));
		assert(!(Instruction->Operands[2].Flags & 0x7F));
	}

	Disassembler->Stage2Count++;

#ifdef TEST_DISASM
	//////////////////////////////////////////////////////////////////////
	// Test against other disassemblers
	//////////////////////////////////////////////////////////////////////

	if (IS_X86_32())
	{
		InstructionLength = X86_GetLength(Instruction, Instruction->Address);
		if (InstructionLength && Instruction->Length != InstructionLength)
		{
			printf("[0x%08I64X] WARNING: instruction lengths differ (%d vs %d)\n", VIRTUAL_ADDRESS, Instruction->Length, InstructionLength);
			DumpInstruction(Instruction, TRUE, TRUE);
			assert(0);
		}
	}
	else if (IS_AMD64())
	{
		// TODO: need other amd64 (x86-64) disassembler to test against
	}
	else if (IS_X86_16())
	{
		// TODO: need other x86 16-bit disassembler to test against
	}
#endif

	//////////////////////////////////////////////////////////////////////
	// Post-operand sanity checks
	//////////////////////////////////////////////////////////////////////

	if (!X86Instruction->HasDstAddressing && !X86Instruction->HasSrcAddressing)
	{
		if (X86Instruction->HasAddressSizePrefix)
		{
			if (!Instruction->AnomalyOccurred)
			{
				if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: address size prefix used with no addressing\n", VIRTUAL_ADDRESS);
				Instruction->AnomalyOccurred = TRUE;
			}
			X86Instruction->HasAddressSizePrefix = FALSE;
		}

		if (X86Instruction->HasSegmentOverridePrefix)
		{
			if (!Instruction->AnomalyOccurred)
			{
				if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: segment override used with no addressing\n", VIRTUAL_ADDRESS);
				Instruction->AnomalyOccurred = TRUE;
			}
			X86Instruction->HasSegmentOverridePrefix = FALSE;
		}	
	}

	// Detect use of unusual segments
	if (!Instruction->AnomalyOccurred && !IS_X86_16())
	{
		switch (X86Instruction->Segment)
		{
			case SEG_CS: case SEG_DS: case SEG_SS:
				break;
			case SEG_ES:
				switch (Instruction->Type)
				{
 					case ITYPE_IN: case ITYPE_STRMOV: case ITYPE_STRCMP: case ITYPE_STRSTOR:
						break;
					default:
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: use of unexpected segment ES\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
						break;
				}
				break;
			case SEG_FS:
				if (IS_X86_32() && !(Instruction->Groups & ITYPE_EXEC)) break;
				if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: use of unexpected segment FS\n", VIRTUAL_ADDRESS);
				Instruction->AnomalyOccurred = TRUE;
				break;
			case SEG_GS:
				if (IS_AMD64() && !(Instruction->Groups & ITYPE_EXEC)) break;
				if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: use of unexpected segment GS\n", VIRTUAL_ADDRESS);
				Instruction->AnomalyOccurred = TRUE;
				break;
			default:
				if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: unexpected segment 0x%02X\n", VIRTUAL_ADDRESS, X86Instruction->Selector);
				Instruction->AnomalyOccurred = TRUE;
				break;
		}
	}

	if ((X86Opcode->OperandFlags[0] & OP_COND_EXEC) == OP_COND_EXEC)
	{
 		assert(Instruction->Type == ITYPE_BRANCHCC || Instruction->Type == ITYPE_LOOPCC);
		for (i = 0; i < Instruction->PrefixCount; i++)
		{
			switch (Instruction->Prefixes[i])
			{
				case PREFIX_BRANCH_NOT_TAKEN:
					if (!Instruction->AnomalyOccurred && X86Instruction->Segment != SEG_CS)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Segment override used with conditional branch\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					X86Instruction->HasSegmentOverridePrefix = FALSE;
					X86Instruction->Segment = SEG_CS;
					X86Instruction->HasBranchNotTakenPrefix = TRUE;
					break;
				case PREFIX_BRANCH_TAKEN:
					if (!Instruction->AnomalyOccurred && X86Instruction->Segment != SEG_DS)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Segment override used with conditional branch\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					X86Instruction->HasSegmentOverridePrefix = FALSE;
					X86Instruction->Segment = SEG_CS;
					X86Instruction->HasBranchTakenPrefix = TRUE;
					break;
			}
		}
	}

	//
	// If lock prefix is enabled, verify it is valid
	//
	if (X86Instruction->HasLockPrefix && 
		!IsValidLockPrefix(X86Instruction, Opcode, Instruction->OpcodeLength, Group, OpcodeExtension))
	{
		if (!SuppressErrors) printf("[0x%08I64X] ERROR: Illegal use of lock prefix for instruction \"%s\"\n", VIRTUAL_ADDRESS, X86Opcode->Mnemonic);
		goto abort;
	}

	//////////////////////////////////////////////////////////////////////
	// Generate disassembly output
	//////////////////////////////////////////////////////////////////////

	if (Disassemble)
	{
		if ((Flags & DISASM_SHOWFLAGS) && 
			(X86Instruction->Opcode.Preconditions || X86Instruction->Opcode.FlagsChanged || X86Instruction->Opcode.ResultsIfTrue))
		{
			APPENDPAD(124);
			if (X86Instruction->Opcode.Preconditions)
			{
				Result = X86Instruction->Opcode.Preconditions;
				APPENDS("COND:{ ");
				if (Result & COND_L) APPENDS("L ");
				if (Result & COND_NL) APPENDS("NL ");
				if (Result & COND_LE) APPENDS("LE ");
				if (Result & COND_NLE) APPENDS("NLE ");
				if (Result & COND_G) APPENDS("G ");
				if (Result & COND_NG) APPENDS("NG ");
				if (Result & COND_GE) APPENDS("GE ");
				if (Result & COND_NGE) APPENDS("NGE ");
				if (Result & COND_A) APPENDS("A ");
				if (Result & COND_NA) APPENDS("NA ");
				if (Result & COND_AE) APPENDS("AE ");
				if (Result & COND_NAE) APPENDS("NAE ");
				if (Result & COND_B) APPENDS("B ");
				if (Result & COND_NB) APPENDS("NB ");
				if (Result & COND_BE) APPENDS("BE ");
				if (Result & COND_NBE) APPENDS("NBE ");
				if (Result & COND_E) APPENDS("E ");
				if (Result & COND_NE) APPENDS("NE ");
				if (Result & COND_C) APPENDS("C ");
				if (Result & COND_NC) APPENDS("NC ");
				if (Result & COND_Z) APPENDS("Z ");
				if (Result & COND_NZ) APPENDS("NZ ");
				if (Result & COND_P) APPENDS("P ");
				if (Result & COND_NP) APPENDS("NP ");
				if (Result & COND_PE) APPENDS("PE ");
				if (Result & COND_PO) APPENDS("PO ");
				if (Result & COND_O) APPENDS("O ");
				if (Result & COND_NO) APPENDS("NO ");
				if (Result & COND_U) APPENDS("U ");
				if (Result & COND_NU) APPENDS("NU ");
				if (Result & COND_S) APPENDS("S ");
				if (Result & COND_NS) APPENDS("NS ");
				if (Result & COND_D) APPENDS("D ");
				APPENDB('}');
			}

			if (X86Instruction->Opcode.FlagsChanged)
			{
				Result = X86Instruction->Opcode.FlagsChanged;

				if (Result & FLAG_SET_MASK)
				{
					APPENDS("SET:{ ");
					if (Result & FLAG_CF_SET) APPENDS("C ");
					if (Result & FLAG_DF_SET) APPENDS("D ");
					if (Result & FLAG_IF_SET) APPENDS("I ");
					APPENDB('}');
				}

				if (Result & FLAG_CLR_MASK)
				{
					APPENDS("CLR:{ ");
					if (Result & FLAG_SF_CLR) APPENDS("S ");
					if (Result & FLAG_ZF_CLR) APPENDS("Z ");
					if (Result & FLAG_AF_CLR) APPENDS("A ");
					if (Result & FLAG_CF_CLR) APPENDS("C ");
					if (Result & FLAG_DF_CLR) APPENDS("D ");
					if (Result & FLAG_IF_CLR) APPENDS("I ");
					if (Result & FLAG_OF_CLR) APPENDS("O ");
					if ((Result & FPU_ALL_CLR) == FPU_ALL_CLR)
					{
						APPENDS("FPU_ALL ");
					}
					else
					{
						if (Result & FPU_C0_CLR) APPENDS("FPU_C0 ");
						if (Result & FPU_C1_CLR) APPENDS("FPU_C1 ");
						if (Result & FPU_C2_CLR) APPENDS("FPU_C2 ");
						if (Result & FPU_C3_CLR) APPENDS("FPU_C3 ");
					}
					APPENDB('}');
				}

				if ((Result & FLAG_MOD_MASK) == FLAG_MOD_MASK)
				{
					APPENDS("MOD:{ ");
					if ((Result & FLAG_ALL_MOD) == FLAG_ALL_MOD)
					{
						APPENDS("FLAGS_ALL ");
					}
					else if ((Result & FLAG_COMMON_MOD) == FLAG_COMMON_MOD)
					{
						APPENDS("FLAGS_COMMON ");
					}
					else
					{
						if (Result & FLAG_OF_MOD) APPENDS("O ");
						if (Result & FLAG_SF_MOD) APPENDS("S ");
						if (Result & FLAG_ZF_MOD) APPENDS("Z ");
						if (Result & FLAG_AF_MOD) APPENDS("A ");
						if (Result & FLAG_PF_MOD) APPENDS("P ");
						if (Result & FLAG_CF_MOD) APPENDS("C ");
						if (Result & FLAG_DF_MOD) APPENDS("D ");
						if (Result & FLAG_IF_MOD) APPENDS("I ");
					}
					if ((Result & FPU_ALL_MOD) == FPU_ALL_MOD)
					{
						APPENDS("FPU_ALL ");
					}
					else
					{
						if (Result & FPU_C0_MOD) APPENDS("FPU_C0 ");
						if (Result & FPU_C1_MOD) APPENDS("FPU_C1 ");
						if (Result & FPU_C2_MOD) APPENDS("FPU_C2 ");
						if (Result & FPU_C3_MOD) APPENDS("FPU_C3 ");
					}
					APPENDB('}');
				}

				if (Result & FLAG_TOG_MASK)
				{
					APPENDS("TOG:{ ");
					if (Result & FLAG_CF_TOG) APPENDS("C ");
					APPENDB('}');
				}
			}
		}

		APPENDS("\n");
	}
	else
	{
		Instruction->String[0] = '\0';
	}

	if (!Instruction->Length || Instruction->Length > X86_MAX_INSTRUCTION_LEN)
	{
		if (!SuppressErrors) printf("[0x%08I64X] ERROR: maximum instruction length reached (\"%s\")\n", VIRTUAL_ADDRESS, X86Instruction->Opcode.Mnemonic);
		goto abort;
	}

	if (!Decode)
	{
		Disassembler->Stage3CountNoDecode++;
		return TRUE; // all work is done
	}

	//////////////////////////////////////////////////////////////////////
	// Detect particularly interesting intructions
	//////////////////////////////////////////////////////////////////////

	Operand1 = &Instruction->Operands[0];
	if (Instruction->Groups & ITYPE_EXEC)
	{
		// If it is a negative offset with a 1-byte or 2-byte offset, assume it is a loop
		if (Operand1->Type == OPTYPE_OFFSET && 
			Operand1->Length <= 2 && X86Instruction->Displacement < 0)
		{
			Instruction->CodeBranch.IsLoop = TRUE;
			Instruction->CodeBranch.Operand = Operand1;
		}

		if (!Instruction->AnomalyOccurred &&
			Operand1->TargetAddress >= (U64)Instruction->Address &&
			Operand1->TargetAddress < (U64)Instruction->Address + Instruction->Length)
		{
			if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: branch into the middle of an instruction\n", VIRTUAL_ADDRESS);
			Instruction->AnomalyOccurred = TRUE;
		}

		switch (Instruction->Type)
		{
			case ITYPE_BRANCH:
				Operand1->Flags |= OP_ADDRESS;
				assert(Instruction->OperandCount == 1);
				if (!(Operand1->Flags & (OP_GLOBAL|OP_FAR)))
				{
					assert(!X86Instruction->HasSelector);
					X86Instruction->Segment = SEG_CS;
				}
				
				if (Operand1->TargetAddress)
				{
					assert(!Instruction->CodeBranch.AddressOffset);
					Instruction->CodeBranch.Count = 1;
					Instruction->CodeBranch.Addresses[0] = Operand1->TargetAddress;
					Instruction->CodeBranch.Operand = Operand1;
				}
				// If there is both a base and index register, the Result will probably be too wrong
				// to even guess
				else if (X86Instruction->HasFullDisplacement && 
					 ((X86Instruction->HasBaseRegister && !X86Instruction->HasIndexRegister) ||
					 (!X86Instruction->HasBaseRegister && X86Instruction->HasIndexRegister)))
				{
					assert(Operand1->Length <= 0xFF);
					if (!X86Instruction->Scale)
					{
						if (Operand1->Length) X86Instruction->Scale = (U8)Operand1->Length;
						else X86Instruction->Scale = X86Instruction->OperandSize;
					}
					assert(Operand1->Length <= 0xFF);
					tmpScale = MAX(X86Instruction->Scale, Operand1->Length);

					assert(tmpScale <= 16);
					Instruction->CodeBranch.AddressOffset = (U8)tmpScale;
					for (i = 0; i < MAX_CODE_REFERENCE_COUNT; i++) Instruction->CodeBranch.Addresses[i] = (U64)X86Instruction->Displacement + (i * tmpScale);
					Instruction->CodeBranch.Count = i;
					Instruction->CodeBranch.IsIndirect = TRUE;
					Instruction->CodeBranch.Operand = Operand1;
				}
				break;

			case ITYPE_CALL:
				Instruction->Groups |= ITYPE_STACK;
				Instruction->CodeBranch.IsCall = TRUE;
				Operand1->Flags |= OP_ADDRESS;
				assert(Instruction->OperandCount == 1);
				if (!(Operand1->Flags & (OP_GLOBAL|OP_FAR)))
				{
					assert(!X86Instruction->HasSelector);
					X86Instruction->Segment = SEG_CS;
				}
								
				if (Operand1->TargetAddress)
				{
					assert(!Instruction->CodeBranch.AddressOffset);
					Instruction->CodeBranch.Count = 1;
					Instruction->CodeBranch.Addresses[0] = Operand1->TargetAddress;
					Instruction->CodeBranch.Operand = Operand1;
				}
				// If there is both a base and index register, the Result will probably be too wrong
				// to even guess
				else if (X86Instruction->HasFullDisplacement &&
					((X86Instruction->HasBaseRegister && !X86Instruction->HasIndexRegister) ||
					 (!X86Instruction->HasBaseRegister && X86Instruction->HasIndexRegister)))
				{
					//DISASM_OUTPUT(("[0x%08I64X] Scale %d, displacement 0x%08I64x\n", VIRTUAL_ADDRESS, X86Instruction->Scale, X86Instruction->Displacement));
					if (!X86Instruction->Scale)
					{
						assert(Operand1->Length <= 0xFF);
						if (Operand1->Length) X86Instruction->Scale = (U8)Operand1->Length;
						else X86Instruction->Scale = X86Instruction->OperandSize;
					}
					tmpScale = MAX(X86Instruction->Scale, Operand1->Length);

					assert(tmpScale <= 16);
					Instruction->CodeBranch.AddressOffset = (U8)tmpScale;
					assert(X86Instruction->Scale > 1);
					for (i = 0; i < MAX_CODE_REFERENCE_COUNT; i++) Instruction->CodeBranch.Addresses[i] = (U64)X86Instruction->Displacement + (i * tmpScale);
					Instruction->CodeBranch.Count = i;
					Instruction->CodeBranch.IsIndirect = TRUE;
					Instruction->CodeBranch.Operand = Operand1;
				}
				break;

			case ITYPE_BRANCHCC:
				assert(Instruction->OperandCount == 1);
				assert(Operand1->Flags & OP_ADDRESS);
				assert(Operand1->Type == OPTYPE_OFFSET);
				if (!(Operand1->Flags & (OP_GLOBAL|OP_FAR)))
				{
					assert(!X86Instruction->HasSelector);
					X86Instruction->Segment = SEG_CS;
				}

				if (Operand1->TargetAddress)
				{
					assert(!Instruction->CodeBranch.AddressOffset);
					Instruction->CodeBranch.Count = 2;
					Instruction->CodeBranch.Addresses[0] = Operand1->TargetAddress;
					Instruction->CodeBranch.Addresses[1] = (U64)Instruction->Address + Instruction->Length;
					Instruction->CodeBranch.Operand = Operand1;
				}
				break;

			case ITYPE_LOOPCC:
				Instruction->CodeBranch.IsLoop = TRUE;
				assert(Instruction->OperandCount == 1);
				assert(Operand1->Flags & OP_ADDRESS);
				assert(Operand1->Type == OPTYPE_OFFSET);
				assert(!(Operand1->Flags & (OP_GLOBAL|OP_FAR)));
				if (Operand1->TargetAddress)
				{
					assert(!Instruction->CodeBranch.AddressOffset);
					Instruction->CodeBranch.Count = 2;
					Instruction->CodeBranch.Addresses[0] = Operand1->TargetAddress;
					Instruction->CodeBranch.Addresses[1] = (U64)Instruction->Address + Instruction->Length;
					Instruction->CodeBranch.Operand = Operand1;
				}
				break;

			case ITYPE_RET:
				Instruction->Groups |= ITYPE_STACK;
				break;

			default:
				break; // do nothing
		}
	}
	else // possible data instruction
	{
		for (i = 0, Operand = Instruction->Operands; i < Instruction->OperandCount; i++, Operand++)
		{
			if (Operand->TargetAddress)
			{
				if (Operand->Flags & OP_DST)
				{
					assert(!Instruction->DataDst.Count);
					Instruction->DataDst.Count = 1;
					Instruction->DataDst.Addresses[0] = Operand->TargetAddress;
					Instruction->DataDst.DataSize = Operand->Length;
					Instruction->DataDst.Operand = Operand;
					DISASM_OUTPUT(("[0x%08I64X] Write of size %d to 0x%04I64X\n", VIRTUAL_ADDRESS, Operand->Length, Operand->TargetAddress));
				}
				if (Operand->Flags & OP_SRC)
				{
					assert(!Instruction->DataSrc.Count);
					Instruction->DataSrc.Count = 1;
					Instruction->DataSrc.Addresses[0] = Operand->TargetAddress;
					Instruction->DataSrc.DataSize = Operand->Length;
					Instruction->DataSrc.Operand = Operand;
					DISASM_OUTPUT(("[0x%08I64X] Read of size %d to 0x%04I64X\n", VIRTUAL_ADDRESS, Operand->Length, Operand->TargetAddress));
				}
			}

			// If there is both a base and index register, the Result will probably be too wrong
			// to even guess
			else if (Operand->Flags & OP_GLOBAL && 
				((X86Instruction->HasBaseRegister && !X86Instruction->HasIndexRegister) ||
				 (!X86Instruction->HasBaseRegister && X86Instruction->HasIndexRegister)))
			{
				DISASM_OUTPUT(("[0x%08I64X] Data reference (scale %d, size %d, displacement 0x%08I64x)\n", VIRTUAL_ADDRESS, X86Instruction->Scale, Operand->Length, X86Instruction->Displacement));
				if (!X86Instruction->Scale)
				{
					assert(Operand->Length <= 0xFF);
					if (Operand->Length) X86Instruction->Scale = (U8)Operand->Length;
					else X86Instruction->Scale = X86Instruction->OperandSize;
				}
				tmpScale = MAX(X86Instruction->Scale, Operand->Length);

				assert(X86Instruction->HasFullDisplacement);
				if (Operand->Flags & OP_DST)
				{
					assert(!Instruction->DataDst.Count);
					assert(tmpScale <= 16);
					Instruction->CodeBranch.AddressOffset = (U8)tmpScale;
					for (i = 0; i < MAX_DATA_REFERENCE_COUNT; i++) Instruction->DataDst.Addresses[i] = (U64)X86Instruction->Displacement + (i * tmpScale);
					Instruction->DataDst.Count = i;
					Instruction->DataDst.DataSize = Operand->Length;
					Instruction->DataDst.Operand = Operand;
				}					
				if (Operand->Flags & OP_SRC)
				{
					assert(!Instruction->DataSrc.Count);
					assert(tmpScale <= 16);
					Instruction->CodeBranch.AddressOffset = (U8)tmpScale;
					for (i = 0; i < MAX_DATA_REFERENCE_COUNT; i++) Instruction->DataSrc.Addresses[i] = (U64)X86Instruction->Displacement + (i * tmpScale);
					Instruction->DataSrc.Count = i;
					Instruction->DataSrc.DataSize = Operand->Length;
					Instruction->DataSrc.Operand = Operand;
				}
			}
		}
	}

	if (Instruction->Groups & ITYPE_STACK)
	{
		switch (Instruction->Type)
		{
			case ITYPE_PUSH:
				assert(Instruction->OperandCount == 1 && Operand1->Length);
				Instruction->StackChange = -Operand1->Length;
				SANITY_CHECK_ADDRESS_SIZE();
				break;

			case ITYPE_POP:
				assert(Instruction->OperandCount == 1 && Operand1->Length);
				Instruction->StackChange = Operand1->Length;
				SANITY_CHECK_ADDRESS_SIZE();
				break;

			case ITYPE_PUSHA:
				Instruction->StackChange = -(X86Instruction->OperandSize * 8); // xAX, xCX, xDX, xBX, xBP, xSP, xSI, xDI
				SANITY_CHECK_ADDRESS_SIZE();
				break;

			case ITYPE_POPA:
				Instruction->StackChange = X86Instruction->OperandSize * 8; // xDI, xSI, xSP, xBP, xBX, xDX, xCX, xAX
				SANITY_CHECK_ADDRESS_SIZE();
				break;

			case ITYPE_PUSHF:			
				Instruction->StackChange = -Operand1->Length;
				Instruction->NeedsEmulation = TRUE;
				SANITY_CHECK_ADDRESS_SIZE();
				break;

			case ITYPE_POPF:
				Instruction->StackChange = Operand1->Length;
				Instruction->NeedsEmulation = TRUE;
				SANITY_CHECK_ADDRESS_SIZE();
				break;

			case ITYPE_ENTER:
				if (!Instruction->AnomalyOccurred)
				{
					if (Instruction->Operands[1].Value_U64 & 3)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: ENTER has invalid operand 2\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					if (Instruction->Operands[2].Value_U64 & ~0x1F)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: ENTER has invalid operand 3\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
				}
				SANITY_CHECK_ADDRESS_SIZE();
				Instruction->Operands[2].Value_U64 &= 0x1F;

				// frame pointer + stack space
				i = Operand1->Length + (U32)Instruction->Operands[1].Value_U64;
				Instruction->StackChange = -((LONG)i);
				i = (U32)Instruction->Operands[2].Value_U64 * Operand1->Length;
				Instruction->StackChange -= i;
				break;

			case ITYPE_LEAVE:
				// This will do "mov esp, ebp; pop ebp" so the StackChange size is dynamic
				SANITY_CHECK_ADDRESS_SIZE();
				break;

			case ITYPE_CALL:
				Instruction->StackChange = -X86Instruction->OperandSize;
				SANITY_CHECK_ADDRESS_SIZE();
				break;

			case ITYPE_RET:
				Instruction->StackChange = X86Instruction->OperandSize;

				switch (Opcode)
				{
					case 0xC3: // ret with no args
						break;

					case 0xC2: // ret with 1 arg
						if (!Instruction->AnomalyOccurred && (Operand1->Value_U64 & 3))
						{
							if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: ret has invalid operand 1\n", VIRTUAL_ADDRESS);
							Instruction->AnomalyOccurred = TRUE;
						}
						Instruction->StackChange += (LONG)Operand1->Value_U64;
						break;

					case 0xCB: // far ret with no args
						Instruction->StackChange *= 2; // account for segment
						Instruction->StackChange += (LONG)Operand1->Value_U64;
						break;

					case 0xCA: // far ret with 1 arg
						if (!Instruction->AnomalyOccurred && (Operand1->Value_U64 & 3))
						{
							if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: retf has invalid operand 1\n", VIRTUAL_ADDRESS);
							Instruction->AnomalyOccurred = TRUE;
						}
						Instruction->StackChange *= 2; // account for segment
						Instruction->StackChange += (LONG)Operand1->Value_U64;
						break;
				}
				SANITY_CHECK_ADDRESS_SIZE();
				break;

			case ITYPE_ADD:
			case ITYPE_XCHGADD:
				if (Instruction->Operands[1].Value_S64) Instruction->StackChange = (LONG)(Instruction->Operands[1].Value_S64);
				break;
			case ITYPE_SUB:
				if (Instruction->Operands[1].Value_S64) Instruction->StackChange = (LONG)(-Instruction->Operands[1].Value_S64);
				break;
			case ITYPE_MOV:
			case ITYPE_AND:
				break;

			default:
				if (!Instruction->AnomalyOccurred)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Instruction \"%s\" is modifying the stack\n", VIRTUAL_ADDRESS, X86Opcode->Mnemonic);
					Instruction->AnomalyOccurred = TRUE;
				}
				break;
		}

		if (!Instruction->AnomalyOccurred &&
			((X86Instruction->OperandSize != 2 && (Instruction->StackChange & 3)) || (Instruction->StackChange & 1)))
		{
			if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: \"%s\" has invalid stack change 0x%02X\n", VIRTUAL_ADDRESS, X86Opcode->Mnemonic, Instruction->StackChange);
			Instruction->AnomalyOccurred = TRUE;
		}
	}

	if (Instruction->Groups & ITYPE_TRAPS)
	{
		switch (Instruction->Type)
		{
			case ITYPE_TRAP:
			case ITYPE_TRAPCC:
			case ITYPE_TRAPRET:
			case ITYPE_BOUNDS:
			case ITYPE_DEBUG:
			case ITYPE_TRACE:
			case ITYPE_INVALID:
			case ITYPE_OFLOW:
				Instruction->NeedsEmulation = TRUE;
				break;
			default:
				assert(0);
				break;
		}
	}

	if (Instruction->Groups & ITYPE_SYSTEM)
	{
		switch (Instruction->Type)
		{
			case ITYPE_CPUID:
			case ITYPE_SYSCALL:
			case ITYPE_SYSCALLRET:
				// This doesn't require privileges
				break;

			case ITYPE_HALT:
			case ITYPE_IN:
			case ITYPE_OUT:
			default:
				Instruction->NeedsEmulation = TRUE;
				break;
		}
	}

	Disassembler->Stage3CountWithDecode++;
	return TRUE;

abort:
	if (!SuppressErrors)
	{
#ifdef TEST_DISASM
		printf("Dump of 0x%04I64X:\n", VIRTUAL_ADDRESS);
		__try { DumpAsBytes(stdout, Instruction->Address, (ULONG_PTR)VIRTUAL_ADDRESS, 16, TRUE); }
		__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {}
#endif
		fflush(stdout);
	}
	return FALSE;
}

// Address = address to first byte after the opcode (e.g., first byte of ModR/M byte or
// immediate value
//
// Returns the address immediately following the operand (e.g., the next operand or the
// start of the next instruction
static  U8 *SetOperands(INSTRUCTION *Instruction, U8 *Address, U32 Flags)
{
	INSTRUCTION_OPERAND *Operand;
	U32 Index, OperandIndex;
	S64 Displacement = 0;
	U8 Register;
	U32 OperandFlags, OperandType, AddressMode, Segment;
	U8 Opcode;
	MODRM modrm;
	REX rex;
	REX_MODRM rex_modrm;
	X86_OPCODE *X86Opcode;
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;
	BOOL Decode = Flags & DISASM_DECODE;
	BOOL Disassemble = Flags & DISASM_DISASSEMBLE;
	BOOL SuppressErrors = Flags & DISASM_SUPPRESSERRORS;

	Opcode = Instruction->LastOpcode;
	X86Opcode = &X86Instruction->Opcode;

	// Setup Mod R/M byte
	if (X86Instruction->HasModRM)
	{
		SET_MODRM(X86Instruction->modrm, X86Instruction->modrm_b);
		modrm = X86Instruction->modrm;
		rex = X86Instruction->rex;
		SET_REX_MODRM(X86Instruction->rex_modrm, rex, modrm);
		rex_modrm = X86Instruction->rex_modrm;
		//DISASM_OUTPUT(("[0x%08I64X] ModRM = 0x%02X (mod=%d, reg=%d, rm=%d)\n", VIRTUAL_ADDRESS, X86Instruction->modrm_b, modrm.mod, rex_modrm.reg, rex_modrm.rm));
		INSTR_INC(1); // increment Instruction->Length and address
	}
	else
	{
		// initialize them to 0
		modrm = X86Instruction->modrm;
		rex = X86Instruction->rex;
		rex_modrm = X86Instruction->rex_modrm;
	}

	for (OperandIndex = 0; OperandIndex < Instruction->OperandCount; OperandIndex++)
	{
		Operand = &Instruction->Operands[OperandIndex];
		assert(!(Operand->Flags & 0x7F));
		
		OperandFlags = X86Opcode->OperandFlags[OperandIndex] & X86_OPFLAGS_MASK;
		OperandType = X86Opcode->OperandFlags[OperandIndex] & X86_OPTYPE_MASK;
		AddressMode = X86Opcode->OperandFlags[OperandIndex] & X86_AMODE_MASK;
		if (Decode && OperandIndex != 0) APPENDS(", ");

		switch (OperandType)
		{
			////////////////////////////////////////////////////////////
			// Special operand types with no associated addressing mode
			////////////////////////////////////////////////////////////

			case OPTYPE_0:
				if (!Decode) continue;
				Operand->Value_U64 = 0;
				Operand->Type = OPTYPE_IMM;
				//DISASM_OUTPUT(("[SetOperand] const 0\n"));
				if (Disassemble)
				{
					APPENDS("<0>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_1:
				if (!Decode) continue;
				Operand->Value_U64 = 1;
				Operand->Type = OPTYPE_IMM;
				//DISASM_OUTPUT(("[SetOperand] const 1\n"));
				if (Disassemble)
				{
					APPENDS("<1>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_FF:
				if (!Decode) continue;
				Operand->Value_U64 = 0xFF;
				Operand->Type = OPTYPE_IMM;
				//DISASM_OUTPUT(("[SetOperand] const 0xff\n"));
				if (Disassemble)
				{
					APPENDS("<0xFF>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_TSC:
				if (!Decode) continue;
				Operand->Length = 8;
				Operand->Type = OPTYPE_SPECIAL;
				//DISASM_OUTPUT(("[SetOperand] TSC\n"));
				if (Disassemble)
				{
					APPENDS("<TSC_MSR>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_CS_MSR:
				if (!Decode) continue;
				Operand->Length = 8;
				Operand->Type = OPTYPE_SPECIAL;
				//DISASM_OUTPUT(("[SetOperand] CS MSR\n"));
				if (Disassemble)
				{
					APPENDS("<CS_MSR>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_EIP_MSR:
				if (!Decode) continue;
				Operand->Length = 8;
				Operand->Type = OPTYPE_SPECIAL;
				//DISASM_OUTPUT(("[SetOperand] EIP MSR\n"));
				if (Disassemble)
				{
					APPENDS("<EIP_MSR>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_ESP_MSR:
				if (!Decode) continue;
				Operand->Length = 8;
				Operand->Type = OPTYPE_SPECIAL;
				//DISASM_OUTPUT(("[SetOperand] ESP MSR\n"));
				if (Disassemble)
				{
					APPENDS("<ESP_MSR>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_KERNELBASE_MSR:
				if (!Decode) continue;
				Operand->Length = 8;
				Operand->Type = OPTYPE_SPECIAL;
				//DISASM_OUTPUT(("[SetOperand] KernelBase MSR\n"));
				if (Disassemble)
				{
					APPENDS("<KRNLBASE_MSR>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_STAR_MSR:
				if (!Decode) continue;
				Operand->Length = 8;
				Operand->Type = OPTYPE_SPECIAL;
				//DISASM_OUTPUT(("[SetOperand] KernelBase MSR\n"));
				if (Disassemble)
				{
					APPENDS("<STAR_MSR>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_CSTAR_MSR:
				assert(!IS_AMD64());
				if (!Decode) continue;
				Operand->Length = 8;
				Operand->Type = OPTYPE_SPECIAL;
				//DISASM_OUTPUT(("[SetOperand] CSTAR MSR\n"));
				if (Disassemble)
				{
					APPENDS("<CSTAR_MSR>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_LSTAR_MSR:
				assert(IS_AMD64());
				if (!Decode) continue;
				Operand->Length = 8;
				Operand->Type = OPTYPE_SPECIAL;
				//DISASM_OUTPUT(("[SetOperand] LSTAR MSR\n"));
				if (Disassemble)
				{
					APPENDS("<LSTAR_MSR>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_FMASK_MSR:
				if (!Decode) continue;
				Operand->Length = 8;
				Operand->Type = OPTYPE_SPECIAL;
				//DISASM_OUTPUT(("[SetOperand] FMASK MSR\n"));
				if (Disassemble)
				{
					APPENDS("<FMASK_MSR>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OP_REG:
				if (!Decode) continue;
				// The reg field is included in the opcode
				Operand->Length = X86Instruction->OperandSize;
				Register = X86_GET_REG64(Opcode);
				switch (X86Instruction->OperandSize)
				{
					case 8:
						Operand->Register = AMD64_64BIT_OFFSET + Register;
						break;
					case 4:
						Operand->Register = X86_32BIT_OFFSET + Register;
						CHECK_AMD64_REG();
						break;
					case 2:
						Operand->Register = X86_16BIT_OFFSET + Register;
						CHECK_AMD64_REG();
						break;
					case 1:
						Operand->Register = X86_8BIT_OFFSET + Register;
						if (X86Instruction->rex_b) CHECK_AMD64_REG();
						break;
					default:
						assert(0);
						return NULL;
				}
				X86_SET_REG(Register);

				//DISASM_OUTPUT(("[SetOperand] OP_REG %s\n", X86_Registers[Operand->Register]));
				if (Disassemble)
				{
					APPENDB('<'); APPENDS(X86_Registers[Operand->Register]); APPENDB('>');
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_REG8:
				if (!Decode) continue;
				// The reg field is included in the opcode
				Operand->Length = 1;
				Register = X86_GET_REG64(Opcode);
				Operand->Register = X86_8BIT_OFFSET + Register;
				CHECK_AMD64_REG();
				X86_SET_REG(Register);

				//DISASM_OUTPUT(("[SetOperand] OP_REG %s\n", X86_Registers[Operand->Register]));
				if (Disassemble)
				{
					APPENDB('<'); APPENDS(X86_Registers[Operand->Register]); APPENDB('>');
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_REG_AL:
				if (!Decode) continue;
				Operand->Length = 1;
				Operand->Register = X86_REG_AL;
				X86_SET_REG(0);
				//DISASM_OUTPUT(("[SetOperand] reg AL\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_REG_CL:
				if (!Decode) continue;
				Operand->Length = 1;
				Operand->Register = X86_REG_CL;
				X86_SET_REG(0);
				//DISASM_OUTPUT(("[SetOperand] reg CL\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_REG_AH:
				if (!Decode) continue;
				Operand->Length = 1;
				Operand->Register = X86_REG_AH;
				X86_SET_REG(0);
				//DISASM_OUTPUT(("[SetOperand] reg AH\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_REG_AX:
				if (!Decode) continue;
				Operand->Length = 2;
				Operand->Register = X86_REG_AX;
				X86_SET_REG(0);
				//DISASM_OUTPUT(("[SetOperand] reg AX\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_REG_DX:
				if (!Decode) continue;
				Operand->Length = 2;
				Operand->Register = X86_REG_DX;
				X86_SET_REG(0);
				//DISASM_OUTPUT(("[SetOperand] reg DX\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_REG_ECX:
				if (!Decode) continue;
				Operand->Length = 4;
				Operand->Register = X86_REG_ECX;
				X86_SET_REG(0);
				//DISASM_OUTPUT(("[SetOperand] reg ECX\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_REG_xBP:
				if (!Decode) continue;
				Operand->Length = X86Instruction->OperandSize; 
				switch (X86Instruction->OperandSize)
				{
					case 8: Operand->Register = AMD64_REG_RBP; break;
					case 4: Operand->Register = X86_REG_EBP; break;
					case 2: Operand->Register = X86_REG_BP; break;
					default: assert(0); return NULL;
				}
				X86_SET_REG(0);
				//DISASM_OUTPUT(("[SetOperand] xAX_BIG (size = %d)\n", Operand->Length));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_REG_xAX_BIG:
				if (!Decode) continue;
				Operand->Length = X86Instruction->OperandSize; 
				switch (X86Instruction->OperandSize)
				{
					case 8: Operand->Register = AMD64_REG_RAX; break;
					case 4: Operand->Register = X86_REG_EAX; break;
					case 2: Operand->Register = X86_REG_AX; break;
					default: assert(0); return NULL;
				}
				X86_SET_REG(0);
				//DISASM_OUTPUT(("[SetOperand] xAX_BIG (size = %d)\n", Operand->Length));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_REG_xAX_SMALL:
				if (!Decode) continue;
				Operand->Length = X86Instruction->OperandSize >> 1; 
				switch (X86Instruction->OperandSize)
				{
					case 8: Operand->Register = X86_REG_EAX; break;
					case 4: Operand->Register = X86_REG_AX; break;
					case 2: Operand->Register = X86_REG_AL; break;
					default: assert(0); return NULL;
				}
				X86_SET_REG(0);
				//DISASM_OUTPUT(("[SetOperand] xAX_SMALL (size = %d)\n", Operand->Length));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_xCX_HI_xBX_LO:
				if (!Decode) continue;
				Operand->Length = X86Instruction->OperandSize << 1; 
				if (Disassemble)
				{
					switch (X86Instruction->OperandSize)
					{
						case 8: APPENDS("<RCX:RBX>"); break;
						case 4: APPENDS("<ECX:EBX>"); break;
						case 2: APPENDS("<CX:BX>"); break;
						default: assert(0); return NULL;
					}
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] xCX_BIG:xBX_BIG (size = %d)\n", Operand->Length));
				continue;
			case OPTYPE_xDX_HI_xAX_LO:
				if (!Decode) continue;
				Operand->Length = X86Instruction->OperandSize << 1; 
				if (Disassemble)
				{
					switch (X86Instruction->OperandSize)
					{
						case 8: APPENDS("<RDX:RAX>"); break;
						case 4: APPENDS("<EDX:EAX>"); break;
						case 2: APPENDS("<DX:AX>"); break;
						default: assert(0); return NULL;
					}
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] xDX_BIG:xAX_BIG (size = %d)\n", Operand->Length));
				continue;

			case OPTYPE_EDX_HI_EAX_LO:
				if (!Decode) continue;
				Operand->Length = 8;
				//DISASM_OUTPUT(("[SetOperand] EDX:EAX\n"));
				if (Disassemble)
				{
					APPENDS("<EDX:EAX>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_EDX_ECX_EBX_EAX:
				Operand->Length = 32;
				//DISASM_OUTPUT(("[SetOperand] EDX:ECX:EBX:EAX\n"));
				if (Disassemble)
				{
					APPENDS("<EDX:ECX:EBX:EAX>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_FLAGS:
				if (!Decode) continue;
				Operand->Length = 2;
				Operand->Flags |= OP_REG;
				Operand->Register = X86_REG_FLAGS;
				//DISASM_OUTPUT(("[SetOperand] reg FLAGS\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_xFLAGS:
				if (!Decode) continue;
				Operand->Length = X86Instruction->OperandSize; 
				Operand->Flags |= OP_REG;
				switch (X86Instruction->OperandSize)
				{
					case 8: Operand->Register = AMD64_REG_RFLAGS; break;
					case 4: Operand->Register = X86_REG_EFLAGS; break;
					case 2: Operand->Register = X86_REG_FLAGS; break;
					default: assert(0); return NULL;
				}
				//DISASM_OUTPUT(("[SetOperand] reg xFLAGS (size = %d)\n", Operand->Length));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_CS:
				if (!Decode) continue;
				if (Instruction->Type != ITYPE_PUSH && Instruction->Type != ITYPE_POP) Operand->Length = 2;
				else Operand->Length = X86Instruction->OperandSize;
				Operand->Register = X86_SEG_CS;
				Operand->Flags |= OP_REG;
				//DISASM_OUTPUT(("[SetOperand] seg CS\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_DS:
				if (!Decode) continue;
				if (Instruction->Type != ITYPE_PUSH && Instruction->Type != ITYPE_POP) Operand->Length = 2;
				else Operand->Length = X86Instruction->OperandSize;
				Operand->Register = X86_SEG_DS;
				Operand->Flags |= OP_REG;
				//DISASM_OUTPUT(("[SetOperand] seg DS\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_ES:
				if (!Decode) continue;
				if (Instruction->Type != ITYPE_PUSH && Instruction->Type != ITYPE_POP) Operand->Length = 2;
				else Operand->Length = X86Instruction->OperandSize;
				Operand->Register = X86_SEG_ES;
				Operand->Flags |= OP_REG;
				//DISASM_OUTPUT(("[SetOperand] seg ES\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_FS:
				if (!Decode) continue;
				if (Instruction->Type != ITYPE_PUSH && Instruction->Type != ITYPE_POP) Operand->Length = 2;
				else Operand->Length = X86Instruction->OperandSize;
				Operand->Register = X86_SEG_FS;
				Operand->Flags |= OP_REG;
				//DISASM_OUTPUT(("[SetOperand] seg FS\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_GS:
				if (!Decode) continue;
				if (Instruction->Type != ITYPE_PUSH && Instruction->Type != ITYPE_POP) Operand->Length = 2;
				else Operand->Length = X86Instruction->OperandSize;
				Operand->Register = X86_SEG_GS;
				Operand->Flags |= OP_REG;
				//DISASM_OUTPUT(("[SetOperand] seg GS\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_SS:
				if (!Decode) continue;
				if (Instruction->Type != ITYPE_PUSH && Instruction->Type != ITYPE_POP) Operand->Length = 2;
				else Operand->Length = X86Instruction->OperandSize;
				Operand->Register = X86_SEG_SS;
				Operand->Flags |= OP_REG;
				//DISASM_OUTPUT(("[SetOperand] seg SS\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_CR0:
				if (!Decode) continue;
				Operand->Length = X86Instruction->OperandSize;
				Operand->Register = X86_REG_CR0;
				Operand->Flags |= OP_REG;
				//DISASM_OUTPUT(("[SetOperand] reg CR0\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_STx:
				if (!Decode) continue;
				Operand->Length = 10;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Flags |= OP_REG;
				Register = X86_GET_REG(X86Instruction->modrm_b);
				Operand->Register = X86_FPU_OFFSET + Register;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_STx: reg st(%d)\n", Register));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_ST0:
				if (!Decode) continue;
				Operand->Length = 10;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Flags |= OP_REG;
				Operand->Register = X86_REG_ST0;
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_ST1:
				if (!Decode) continue;
				Operand->Length = 10;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Flags |= OP_REG;
				Operand->Register = X86_REG_ST1;
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "<%s>", X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_FPU_STATUS:
				if (!Decode) continue;
				Operand->Length = 2;
				if (Disassemble)
				{
					APPENDS("<FPUSTAT>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_FPU_CONTROL:
				if (!Decode) continue;
				Operand->Length = 2;
				if (Disassemble)
				{
					APPENDS("<FPUCTRL>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_FPU_TAG:
				if (!Decode) continue;
				Operand->Length = 2;
				if (Disassemble)
				{
					APPENDS("<FPUTAG>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			case OPTYPE_FLDZ:
				if (!Decode) continue;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Length = 10;
				if (Disassemble)
				{
					APPENDS("<0.0>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_FLD1:
				if (!Decode) continue;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Length = 10;
				if (Disassemble)
				{
					APPENDS("<1.0>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_FLDPI:
				if (!Decode) continue;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Length = 10;
				if (Disassemble)
				{
					APPENDS("<pi>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_FLDL2T:
				if (!Decode) continue;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Length = 10;
				if (Disassemble)
				{
					APPENDS("<log_2 10>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_FLDL2E:
				if (!Decode) continue;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Length = 10;
				if (Disassemble)
				{
					APPENDS("<log_2 e>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_FLDLG2:
				if (!Decode) continue;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Length = 10;
				if (Disassemble)
				{
					APPENDS("<log_10 2>");
					X86_WRITE_OPFLAGS();
				}
				continue;
			case OPTYPE_FLDLN2:
				if (!Decode) continue;
				Operand->Type = OPTYPE_FLOAT;
				Operand->Length = 10;
				if (Disassemble)
				{
					APPENDS("<ln 2>");
					X86_WRITE_OPFLAGS();
				}
				continue;

			////////////////////////////////////////////////////////////
			// Fixed sizes regardless of operand size
			////////////////////////////////////////////////////////////

			case OPTYPE_b: // byte regardless of operand size
				Operand->Length = 1;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_b (size 1, signed %d)\n", ((OperandFlags & OP_SIGNED) != 0)));
				break;

			case OPTYPE_w: // word regardless of operand size
				Operand->Length = 2;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_w (size 2)\n"));
				break;

			case OPTYPE_d: // dword regardless of operand size
				Operand->Length = 4;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_d (size 4)\n"));
				break;

			case OPTYPE_q: // qword regardless of operand size
				Operand->Length = 8;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_q (size 8)\n"));
				break;

			case OPTYPE_o: // oword regardless of operand size
				Operand->Length = 16;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_o (size 16)\n"));
				break;

			case OPTYPE_dt: // 6-byte or 10-byte pseudo descriptor (sgdt, lgdt, sidt, lidt)
				if (IS_AMD64()) Operand->Length = 10;
				else Operand->Length = 6;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_dt (%d bytes)\n", Operand->Length));
				break;

			case OPTYPE_cpu:
				if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Undocumented loadall instruction?\n", VIRTUAL_ADDRESS);
				Instruction->AnomalyOccurred = TRUE;
				Operand->Length = 204;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_cpu (size 204)\n"));
				break;

			////////////////////////////////////////////////////////////
			// Sizes depending on the operand size
			////////////////////////////////////////////////////////////

			case OPTYPE_z: // word if operand size is 16 bits and dword otherwise
				switch (X86Instruction->OperandSize)
				{
					case 8: case 4: Operand->Length = 4; break;
					case 2: Operand->Length = 2; break;
					default: assert(0); return NULL;
				}
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_z (length %d)\n", Operand->Length));
				break;

			case OPTYPE_v: // word, dword, or qword
				Operand->Length = X86Instruction->OperandSize;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_v (size %d, signed = %d)\n", Operand->Length, ((OperandFlags & OP_SIGNED) != 0)));
				break;

			case OPTYPE_a: // two word or dword operands in memory (used only by bound)
				assert(Instruction->OpcodeBytes[0] == X86_BOUND);
				switch (X86Instruction->OperandSize)
				{
					case 8: case 4: Operand->Length = 8; break;
					case 2: Operand->Length = 4; break;
					default: assert(0); return NULL;
				}
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_a (size %d)\n", Operand->Length));
				break;

			case OPTYPE_p: // 32-bit or 48-bit pointer depending on operand size
				if (!Instruction->AnomalyOccurred && X86Instruction->HasSegmentOverridePrefix)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Segment override used when segment is explicit\n", VIRTUAL_ADDRESS);
					Instruction->AnomalyOccurred = TRUE;
				}
				switch (X86Instruction->OperandSize)
				{
					case 8: case 4: Operand->Length = 6; break;
					case 2: Operand->Length = 4; break;
					default: assert(0); return NULL;
				}
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_p (length %d)\n", Operand->Length));
				break;

			case OPTYPE_dq: // dword or qword
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_dq (size 4 or 8)\n"));
				switch (X86Instruction->OperandSize)
				{
					case 8: Operand->Length = 8; break;
					case 4: case 2: Operand->Length = 4; break;
					default: assert(0); return NULL;
				}
				break;

			case OPTYPE_mw: // a word if the destination operand is memory
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_mw (size 0)\n"));
				assert(X86Instruction->HasModRM);		
				if (modrm.mod == 3) Operand->Length = X86Instruction->OperandSize; // using register
				else Operand->Length = 2; // using memory
				break;

			case OPTYPE_lea:
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_lea (size 0)\n"));
				assert(OperandIndex == 1);
				Operand->Length = Instruction->Operands[0].Length;
				break;

			////////////////////////////////////////////////////////////
			// FPU types
			////////////////////////////////////////////////////////////

			case OPTYPE_ps: // packed single real
				Operand->Length = 4;
				Operand->Type = OPTYPE_FLOAT;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_ps (packed single real)\n"));
				break;
			case OPTYPE_pd: // packed double real
				Operand->Length = 8;
				Operand->Type = OPTYPE_FLOAT;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_pd (packed double real)\n"));
				break;
			case OPTYPE_pb: // packed BCD
				Operand->Length = 10;
				Operand->Type = OPTYPE_BCD;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_pb (packed BCD)\n"));
				break;
			case OPTYPE_ss: // scalar single real
				Operand->Length = 4;
				Operand->Type = OPTYPE_FLOAT;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_ss (single real)\n"));
				break;
			case OPTYPE_sd: // scalar double real
				Operand->Length = 8;
				Operand->Type = OPTYPE_FLOAT;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_sd (double real)\n"));
				break;
			case OPTYPE_se: // extended real
				Operand->Length = 10;
				Operand->Type = OPTYPE_FLOAT;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_se (extended real)\n"));
				break;

			case OPTYPE_fev: // FPU environment (28 bytes in 32-bit modes, 14 bytes in 16-bit real mode)
				switch (X86Instruction->OperandSize)
				{
					case 8: case 4: Operand->Length = 28; break;
					case 2: Operand->Length = 14; break;
					default: assert(0); return NULL;
				}
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_fev (FPU environment, length %d)\n", Operand->Length));
				break;

			case OPTYPE_fst1: // FPU state (108 bytes in 32-bit modes, 94 bytes in 16-bit real mode)
				switch (X86Instruction->OperandSize)
				{
					case 8: case 4: Operand->Length = 108; break;
					case 2: Operand->Length = 94; break;
					default: assert(0); return NULL;
				}
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_fst1 (FPU state, length %d)\n", Operand->Length));
				break;

			case OPTYPE_fst2: // 512 bytes for FPU state (FPU + MMX + XXM + MXCSR)
				Operand->Length = 512;
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_fst2 (FPU + MMX + XXM + MXCSR state, length 512)\n"));
				break;

			case OPTYPE_sso:
				if (modrm.mod == 3) // from register
				{
					Operand->Length = 16;
				}
				else // from memory
				{
					Operand->Length = 4;
					Operand->Type = OPTYPE_FLOAT;
				}
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_sso (single real or oword)\n"));
				break;

			case OPTYPE_sdo:
				if (modrm.mod == 3) // from register
				{
					Operand->Length = 16;
				}
				else // from memory
				{
					Operand->Length = 8;
					Operand->Type = OPTYPE_FLOAT;
				}
				//DISASM_OUTPUT(("[SetOperand] OPTYPE_sso (double real or oword)\n"));
				break;

			default:
				assert(0);
				return NULL;
		}

		switch (AddressMode)
		{
			////////////////////////////////////////////////////////////
			// Special types
			////////////////////////////////////////////////////////////

			case AMODE_xlat: // DS:[EBX+AL]
				if (!Decode) continue;
				assert(Operand->Length == 1);
				Operand->Flags |= OP_ADDRESS | OP_REG;
				Operand->Type = OPTYPE_STRING;
				
				switch (X86Instruction->AddressSize)
				{
					case 8: Operand->Register = AMD64_REG_RBX; break;
					case 4: Operand->Register = X86_REG_EBX; break;
					case 2: Operand->Register = X86_REG_BX; break;
					default: assert(0); return NULL;
				}
				X86_SET_ADDR();
				X86Instruction->Scale = 1;
				X86Instruction->BaseRegister =(X86_REGISTER) Operand->Register;
				X86Instruction->HasBaseRegister = TRUE;
				X86Instruction->IndexRegister = X86_REG_AL;
				X86Instruction->HasIndexRegister = TRUE;

				//DISASM_OUTPUT(("[SetOperand] AMODE_xlat (DS:[EBX+AL])\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "%s:[%s]", 
						Segments[X86Instruction->Segment], X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			////////////////////////////////////////////////////////////
			// Without mod R/M byte
			////////////////////////////////////////////////////////////

			case AMODE_I: // immediate value
				if (Decode)
				{
					Operand->Type = OPTYPE_IMM;
					switch (Operand->Length)
					{
						case 8:
							if (OperandFlags & OP_SIGNED) Operand->Value_S64 = (S64)*((S64 *)Address);
							else Operand->Value_U64 = (U64)*((U64 *)Address);
							break;
						case 4:
							if (!(OperandFlags & OP_SIGNED) && OperandIndex == 1 && 
								(Instruction->Operands[0].Flags & (OP_REG|OP_ADDRESS)) &&
								Instruction->Operands[0].Length == 8)
							{
								// For some opcodes the second operand is a sign-extended imm32 value
								assert(X86Instruction->OperandSize == 8);
								switch (Instruction->Type)
								{
									case ITYPE_AND:
									case ITYPE_ADD:
									case ITYPE_XCHGADD:
									case ITYPE_CMP:
									case ITYPE_MOV:
									case ITYPE_SUB:
									case ITYPE_TEST:
									case ITYPE_OR:
									case ITYPE_XOR:
										assert(OperandIndex == 1);
										Operand->Value_S64 = (S64)*((S32 *)Address);
										break;
									default:
										assert(0);
										if (OperandFlags & OP_SIGNED) Operand->Value_S64 = (S64)*((S32 *)Address);
										else Operand->Value_U64 = (U64)*((U32 *)Address);
										break;
								}
							}
							else
							{
								if (OperandFlags & OP_SIGNED) Operand->Value_S64 = (S64)*((S32 *)Address);
								else Operand->Value_U64 = (U64)*((U32 *)Address);
							}
							break;
						case 2:
							if (OperandFlags & OP_SIGNED) Operand->Value_S64 = (S64)*((S16 *)Address);
							else Operand->Value_U64 = (U64)*((U16 *)Address);
							break;
						case 1:
							if (OperandFlags & OP_SIGNED) Operand->Value_S64 = (S64)*((S8 *)Address);
							else Operand->Value_U64 = (U64)*((U8 *)Address);
							break;
						default:
							assert(0);
							return NULL;
					}
				}
				INSTR_INC(Operand->Length); // increment Instruction->Length and address
				assert(X86Instruction->OperandSize >= Operand->Length);
				if (Instruction->Type == ITYPE_PUSH) Operand->Length = X86Instruction->OperandSize;

				//DISASM_OUTPUT(("[SetOperand] AMODE_I (immediate data)\n"));
				if (Disassemble)
				{
					X86_WRITE_IMMEDIATE();
					X86_WRITE_OPFLAGS();
				}
				continue;

			case AMODE_J: // IP-relative jump offset
				SANITY_CHECK_ADDRESS_SIZE();
				if (Decode)
				{
					Operand->Flags |= OP_IPREL | OP_SIGNED | OP_REG | OP_ADDRESS;
					Operand->Type = OPTYPE_OFFSET;
					switch (X86Instruction->OperandSize)
					{
						case 8: Operand->Register = AMD64_REG_RIP; break;
						case 4: Operand->Register = X86_REG_EIP; break;
						case 2: Operand->Register = X86_REG_IP; break;
						default: assert(0); return NULL;
					}
					switch (Operand->Length)
					{
						case 8: X86Instruction->Displacement = *((S64 *)Address); break;
						case 4: X86Instruction->Displacement = (S64)*((S32 *)Address); break;
						case 2: X86Instruction->Displacement = (S64)*((S16 *)Address); break;
						case 1: X86Instruction->Displacement = (S64)*((S8 *)Address); break;
						default: assert(0); return NULL;
					}					

					Operand->Value_S64 = X86Instruction->Displacement;
					X86Instruction->Relative = TRUE;

					if ((Operand->Flags & OP_COND) && !X86Instruction->Displacement)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: Both conditions of branch go to same address\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
				}

				INSTR_INC(Operand->Length); // increment Instruction->Length and address
				if (!Decode) continue;

				assert((Operand->Flags & OP_EXEC) && (Instruction->Groups & ITYPE_EXEC));
				Operand->TargetAddress = ApplyDisplacement((U64)Address, Instruction);
				X86Instruction->Relative = TRUE; 
				X86_SET_ADDR();
				SANITY_CHECK_SEGMENT_OVERRIDE();
				X86Instruction->HasSegmentOverridePrefix = FALSE;
				X86Instruction->Segment = SEG_CS;
				X86Instruction->BaseRegister = (X86_REGISTER)Operand->Register;
				X86Instruction->HasBaseRegister = TRUE;
				assert(Instruction->OperandCount == 1);
				//DISASM_OUTPUT(("[SetOperand] AMODE_J (branch with relative offset)\n"));
				if (Disassemble)
				{
					X86_WRITE_IP_OFFSET(Operand);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case AMODE_O: // word/dword offset
				Operand->Type = OPTYPE_OFFSET;
				Operand->Flags |= OP_ADDRESS;
				SANITY_CHECK_OPERAND_SIZE();
				switch (X86Instruction->AddressSize)
				{
					case 8:
						if (Operand->Flags & OP_SIGNED) X86Instruction->Displacement = *((S64 *)Address);
						else X86Instruction->Displacement = (S64)*((U64 *)Address);
						break;
					case 4:
						if (Operand->Flags & OP_SIGNED) X86Instruction->Displacement = (S64)*((S32 *)Address);
						else X86Instruction->Displacement = (S64)*((U32 *)Address);
						break;
					case 2:
						if (Operand->Flags & OP_SIGNED) X86Instruction->Displacement = (S64)*((S16 *)Address);
						else X86Instruction->Displacement = (S64)*((U16 *)Address);
						break;
					default:
						assert(0);
						return FALSE;
				}

				INSTR_INC(X86Instruction->AddressSize); // increment Instruction->Length and address
				if (!Decode) continue;

				X86Instruction->HasFullDisplacement = TRUE;
				X86_SET_ADDR();
				X86_SET_TARGET();
				assert(X86Instruction->Segment == SEG_DS || X86Instruction->HasSegmentOverridePrefix);
				//DISASM_OUTPUT(("[SetOperand] AMODE_O (offset)\n"));
				if (Disassemble)
				{
					X86_WRITE_OFFSET(Operand);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case AMODE_A: // absolute address
				Operand->Flags |= OP_ADDRESS | OP_FAR;
				SANITY_CHECK_ADDRESS_SIZE();
				SANITY_CHECK_SEGMENT_OVERRIDE();
				X86Instruction->HasSelector = TRUE;
				X86Instruction->HasSegmentOverridePrefix = FALSE;
				switch (Operand->Length)
				{
					case 6:
						X86Instruction->Segment = (X86_SEGMENT) *((U16 *)Address); INSTR_INC(2);
						X86Instruction->Displacement = (S64)*((S32 *)Address); INSTR_INC(4);
						break;
					case 4:
						X86Instruction->Segment =(X86_SEGMENT) *((U16 *)Address); INSTR_INC(2);
						X86Instruction->Displacement = (S64)*((S16 *)Address); INSTR_INC(2);
						break;
					default:
						assert(0);
						return FALSE;
				}
				if (!Decode) continue;
				X86Instruction->HasFullDisplacement = TRUE;
				X86_SET_ADDR();
				X86_SET_TARGET();
				//DISASM_OUTPUT(("[SetOperand] AMODE_A (absolute address)\n"));
				if (Disassemble)
				{
					X86_WRITE_OFFSET(Operand);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case AMODE_X: // DS:[ESI]
				if (!Decode) continue;
				Operand->Flags |= OP_ADDRESS | OP_REG;
				Operand->Type = OPTYPE_STRING;
				switch (X86Instruction->AddressSize)
				{
					case 8: Operand->Register = AMD64_REG_RSI; break;
					case 4: Operand->Register = X86_REG_ESI; break;
					case 2: Operand->Register = X86_REG_SI; break;
					default: assert(0); return NULL;
				}

				X86Instruction->BaseRegister =(X86_REGISTER) Operand->Register;
				X86Instruction->HasBaseRegister = TRUE;
				X86_SET_ADDR();
				if (!X86Instruction->HasSegmentOverridePrefix) X86Instruction->Segment = SEG_DS;

				//DISASM_OUTPUT(("[SetOperand] AMODE_X (addressing via DS:[ESI])\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "%s:[%s]", 
						Segments[X86Instruction->Segment], X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;

			case AMODE_Y: // ES:[EDI]
				if (!Decode) continue;
				Operand->Flags |= OP_ADDRESS | OP_REG;
				Operand->Type = OPTYPE_STRING;
				switch (X86Instruction->AddressSize)
				{
					case 8: Operand->Register = AMD64_REG_RDI; break;
					case 4: Operand->Register = X86_REG_EDI; break;
					case 2: Operand->Register = X86_REG_DI; break;
					default: assert(0); return NULL;
				}

				X86Instruction->BaseRegister = (X86_REGISTER)Operand->Register;
				X86Instruction->HasBaseRegister = TRUE;
				X86_SET_ADDR();
				if (X86Instruction->HasSegmentOverridePrefix)
				{
					if (!Instruction->AnomalyOccurred)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ANOMALY: segment override used with AMODE_Y\n", VIRTUAL_ADDRESS);
						Instruction->AnomalyOccurred = TRUE;
					}
					Segment = X86Instruction->DstSegment = SEG_ES;
					X86Instruction->HasDstSegment = TRUE;
				}
				else
				{
					Segment = X86Instruction->Segment = SEG_ES;
				}

				//DISASM_OUTPUT(("[SetOperand] AMODE_Y (addressing via ES:[EDI])\n"));
				if (Disassemble)
				{
					APPEND(OPCSTR, SIZE_LEFT, "%s:[%s]", 
						Segments[Segment], X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				continue;
			
			////////////////////////////////////////////////////////////
			// Mod R/M byte with only registers
			// Handle that case here since it is straightforward
			////////////////////////////////////////////////////////////

			case AMODE_PR: // modrm.rm = mmx register and modrm.mod = 11
				assert(X86Instruction->HasModRM);
				if (modrm.mod != 3)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: mod != 3 for AMODE_PR (\"%s\")\n", VIRTUAL_ADDRESS, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				else if (rex_modrm.rm > 7)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: invalid mmx register %d for AMODE_PR (\"%s\")\n", VIRTUAL_ADDRESS, rex_modrm.rm, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				else if (X86Instruction->OperandSize == 2)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: AMODE_PR illegal in 16-bit mode.[ rex_modrm.rm = %d ] (\"%s\")\n", VIRTUAL_ADDRESS, rex_modrm.rm, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				if (!Decode) continue;

				Operand->Flags |= OP_REG;
				Operand->Register = X86_MMX_OFFSET + rex_modrm.rm;
				X86_SET_REG(0);

				if (Disassemble)
				{
					assert(X86_Registers[Operand->Register]);
					APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_PR (MMX register)\n"));
				continue;

			case AMODE_VR: // modrm.rm = xmm register and modrm.mod = 11
				assert(X86Instruction->HasModRM);
				if (modrm.mod != 3)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: mod != 3 for AMODE_VR (\"%s\")\n", VIRTUAL_ADDRESS, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				else if (X86Instruction->OperandSize == 2)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: AMODE_VR illegal in 16-bit mode.[ rex_modrm.rm = %d ] (\"%s\")\n", VIRTUAL_ADDRESS, rex_modrm.rm, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				if (!Decode) continue;

				Operand->Flags |= OP_REG;
				Operand->Register = X86_XMM_OFFSET + rex_modrm.rm;
				X86_SET_REG(0);

				if (Disassemble)
				{
					APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_VR (XMM register)\n"));
				continue;

			case AMODE_P: // modrm.reg = mmx register
				assert(X86Instruction->HasModRM);
				if (rex_modrm.reg > 7)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: invalid mmx register %d for AMODE_P (\"%s\")\n", VIRTUAL_ADDRESS, rex_modrm.reg, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				else if (X86Instruction->OperandSize == 2)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: AMODE_P illegal in 16-bit mode (\"%s\")\n", VIRTUAL_ADDRESS, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				if (!Decode) continue;

				Operand->Flags |= OP_REG;
				Operand->Register = X86_MMX_OFFSET + rex_modrm.reg;
				X86_SET_REG(0);

				if (Disassemble)
				{
					APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_P (MMX register)\n"));
				continue;

			case AMODE_V: // modrm.reg = xmm register
				assert(X86Instruction->HasModRM);
				if (X86Instruction->OperandSize == 2)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: AMODE_P illegal in 16-bit mode (\"%s\")\n", VIRTUAL_ADDRESS, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				if (!Decode) continue;

				Operand->Flags |= OP_REG;
				Operand->Register = X86_XMM_OFFSET + rex_modrm.reg; break;
				X86_SET_REG(0);

				if (Disassemble)
				{
					APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_V (XMM register)\n"));
				continue;

			case AMODE_R: // modrm.rm is general register and modrm.mod = 11
				assert(X86Instruction->HasModRM);
				if (modrm.mod != 3)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: mod != 3 for AMODE_R (\"%s\")\n", VIRTUAL_ADDRESS, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				if (!Decode) continue;
				Operand->Flags |= OP_REG;
				switch (Operand->Length)
				{
					case 8: Operand->Register = AMD64_64BIT_OFFSET + rex_modrm.rm; break;
					case 4: Operand->Register = X86_32BIT_OFFSET, rex_modrm.rm; CHECK_AMD64_REG(); break;
					case 2: Operand->Register = X86_16BIT_OFFSET, rex_modrm.rm; CHECK_AMD64_REG(); break;
					case 1: Operand->Register = X86_8BIT_OFFSET, rex_modrm.rm; if (X86Instruction->rex_b) CHECK_AMD64_REG(); break;
					default: assert(0); return NULL;
				}
				X86_SET_REG(rex_modrm.rm);
				if (Disassemble)
				{
					APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_R (general register)\n"));
				continue;

			case AMODE_G: // modrm.reg = general register
				assert(X86Instruction->HasModRM);
				if (!Decode) continue;
				Operand->Flags |= OP_REG;
				switch (Operand->Length)
				{
					case 8: Operand->Register = AMD64_64BIT_OFFSET + rex_modrm.reg; break;
					case 4: Operand->Register = X86_32BIT_OFFSET + rex_modrm.reg; CHECK_AMD64_REG(); break;
					case 2: Operand->Register = X86_16BIT_OFFSET + rex_modrm.reg; CHECK_AMD64_REG(); break;
					case 1: Operand->Register = X86_8BIT_OFFSET + rex_modrm.reg; if (X86Instruction->rex_b) CHECK_AMD64_REG(); break;
					default: assert(0); return NULL;
				}
				X86_SET_REG(rex_modrm.reg);
				if (Disassemble)
				{
					APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_G (general register)\n"));
				continue;
			
			case AMODE_S: // modrm.reg = segment register
				assert(X86Instruction->HasModRM);
				if (!Decode) continue;
				Operand->Flags |= OP_REG;
				switch (X86Instruction->OperandSize)
				{
					case 8:
					case 4:
					case 2:
						if (rex_modrm.reg <= 5) Operand->Register = X86_SEGMENT_OFFSET + rex_modrm.reg;
						break;
					default:
						assert(0);
						return NULL;
				}

				X86_SET_REG(0);
				if (Disassemble)
				{
					if (rex_modrm.reg > 5) APPEND(OPCSTR, SIZE_LEFT, "seg_%02X", rex_modrm.reg);
					else APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_S (segment register)\n"));
				continue;

			case AMODE_T: // modrm.reg = test register
				assert(X86Instruction->HasModRM);
				if (!Decode) continue;
				Instruction->Groups |= ITYPE_SYSTEM;
				Instruction->NeedsEmulation = TRUE;
				Operand->Flags |= OP_REG;
				switch (X86Instruction->OperandSize)
				{
					case 8:
					case 4:
					case 2:
						Operand->Register = X86_TEST_OFFSET + rex_modrm.reg;
						break;
					default:
						assert(0);
						return NULL;
				}

				X86_SET_REG(0);
				if (Disassemble)
				{
					APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_T (test register)\n"));
				continue;

			case AMODE_C: // modrm.reg = control register
				assert(X86Instruction->HasModRM);
				assert(Instruction->Type == ITYPE_MOV);
				if (!Decode) continue;
				Instruction->Groups |= ITYPE_SYSTEM;
				Instruction->NeedsEmulation = TRUE;
				Operand->Flags |= OP_REG;
				if (IS_AMD64()) X86Instruction->OperandSize = 8;
				switch (X86Instruction->OperandSize)
				{
					case 8:
					case 4:
					case 2:
						Operand->Register = X86_CONTROL_OFFSET + rex_modrm.reg;
						break;
					default:
						assert(0);
						return NULL;
				}

				X86_SET_REG(0);
				if (Disassemble)
				{
					APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_C (control register)\n"));
				continue;

			case AMODE_D: // modrm.reg = debug register
				assert(X86Instruction->HasModRM);
				assert(Instruction->Type == ITYPE_MOV);
				if (!Decode) continue;
				Instruction->NeedsEmulation = TRUE;
				Operand->Flags |= OP_REG;
				Instruction->Groups |= ITYPE_SYSTEM;
				Instruction->NeedsEmulation = TRUE;
				if (IS_AMD64()) X86Instruction->OperandSize = 8;
				switch (X86Instruction->OperandSize)
				{
					case 8:
					case 4:
					case 2:
						Operand->Register = X86_DEBUG_OFFSET + rex_modrm.reg;
						break;
					default:
						assert(0);
						return NULL;
				}

				X86_SET_REG(0);
				if (Disassemble)
				{
					APPENDS(X86_Registers[Operand->Register]);
					X86_WRITE_OPFLAGS();
				}
				//DISASM_OUTPUT(("[SetOperand] AMODE_D (debug register)\n"));
				continue;

			////////////////////////////////////////////////////////////
			// Mod R/M byte with memory or register
			////////////////////////////////////////////////////////////

			case AMODE_M: // memory only
				assert(X86Instruction->HasModRM);
				if (modrm.mod == 3)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: mod = 3 for AMODE_M (\"%s\")\n", VIRTUAL_ADDRESS, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}
				assert(X86Instruction->Segment == SEG_DS || X86Instruction->HasSegmentOverridePrefix);
				//DISASM_OUTPUT(("[SetOperand] AMODE_M (memory only)\n"));
				Address = SetModRM32(Instruction, Address, Operand, OperandIndex, SuppressErrors);
				if (!Address) return NULL;
				break;

			case AMODE_E: // general register or memory
				assert(X86Instruction->HasModRM);
				if (OperandType == OPTYPE_p && modrm.mod == 3)
				{
					if (!SuppressErrors) printf("[0x%08I64X] ERROR: mod = 3 for AMODE_E with OPTYPE_p (\"%s\")\n", VIRTUAL_ADDRESS, X86Instruction->Opcode.Mnemonic);
					goto abort;
				}

				//DISASM_OUTPUT(("[SetOperand] AMODE_E (general register or memory)\n"));
				Address = SetModRM32(Instruction, Address, Operand, OperandIndex, SuppressErrors);
				if (!Address) return NULL;
				if (Decode && (Instruction->Type == ITYPE_PUSH || Instruction->Type == ITYPE_POP))
				{
					assert(X86Instruction->OperandSize >= Operand->Length);
					Operand->Length = X86Instruction->OperandSize;
				}
				break;

			case AMODE_Q: // mmx register or memory address
				assert(X86Instruction->HasModRM);
				//DISASM_OUTPUT(("[SetOperand] AMODE_Q (MMX register or memory address)\n"));
				if (modrm.mod == 3) // it is a register
				{
					if (rex_modrm.rm > 7)
					{
						if (!SuppressErrors) printf("[0x%08I64X] ERROR: invalid mmx register %d for AMODE_P (\"%s\")\n", VIRTUAL_ADDRESS, rex_modrm.rm, X86Instruction->Opcode.Mnemonic);
						goto abort;
					}
					Operand->Register = X86_MMX_OFFSET + rex_modrm.rm;
					Operand->Flags |= OP_REG;
					X86_SET_REG(0);
				}
				else
				{
					Address = SetModRM32(Instruction, Address, Operand, OperandIndex, SuppressErrors);
					if (!Address) return NULL;
				}
				break;

			case AMODE_W: // xmm register or memory address
				assert(X86Instruction->HasModRM);
				//DISASM_OUTPUT(("[SetOperand] AMODE_W (XMM register or memory address)\n"));
				if (modrm.mod == 3) // it is a register
				{
					Operand->Register = X86_XMM_OFFSET + rex_modrm.rm;
					Operand->Flags |= OP_REG;
					X86_SET_REG(0);
				}
				else
				{
					Address = SetModRM32(Instruction, Address, Operand, OperandIndex, SuppressErrors);
					if (!Address) return NULL;
				}
				break;

			default:
				assert(0);
				return NULL;
		}

		if (!Decode) continue;

		// If this is reached then SetModRM32 was called
		if ((Operand->Flags & OP_ADDRESS))
		{
			assert(Operand->Length);
			switch (Operand->Register)
			{
				case X86_REG_BP:
				case X86_REG_EBP:
				case AMD64_REG_RBP:
					if (X86Instruction->Displacement > 0) Operand->Flags |= OP_PARAM;
					else Operand->Flags |= OP_LOCAL;
					break;
				default:
					break;
			}
		}

		if (Disassemble)
		{
			Index = OperandType >> OPTYPE_SHIFT;
			assert(Index > 0 && Index < MAX_OPTYPE_INDEX && OptypeHandlers[Index]);
			OptypeHandlers[Index](Instruction, Operand, OperandIndex);
			X86_WRITE_OPFLAGS();
		}
	}

	return Address;

abort:
	if (!SuppressErrors)
	{
#ifdef TEST_DISASM
		printf("Dump of 0x%04I64X:\n", VIRTUAL_ADDRESS);
		__try { DumpAsBytes(stdout, Instruction->Address, (ULONG_PTR)VIRTUAL_ADDRESS, 16, TRUE); }
		__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {}
#endif
		fflush(stdout);
	}
	return NULL;
}

// NOTE: Address points one byte after ModRM
static  U8 *SetModRM16(INSTRUCTION *Instruction, U8 *Address, INSTRUCTION_OPERAND *Operand, U32 OperandIndex, BOOL SuppressErrors)
{
	MODRM modrm;
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;

	DISASM_OUTPUT(("[SetModRM16] Current instruction length = %d\n", Instruction->Length));
	modrm = X86Instruction->modrm;
	assert(!X86Instruction->rex_b);

	//
	// Both operands are registers
	// Condition: mod = 3
	//
	if (modrm.mod == 3)
	{
		//DISASM_OUTPUT(("[SetModRM16] Both regs (rm_reg %d)\n", modrm.rm));
		switch (Operand->Length)
		{
			case 4: Operand->Register = X86_32BIT_OFFSET + modrm.rm; break;
			case 2: Operand->Register = X86_16BIT_OFFSET + modrm.rm; break;
			case 1: Operand->Register = X86_8BIT_OFFSET + modrm.rm; break;
			default: assert(0); return NULL;
		}
		Operand->Flags |= OP_REG;
	}

	// 
	// Address is an absolute address (technically a 16-bit offset from DS:0)
	// Condition: mod = 0 and rm = 6
	//
	else if (modrm.mod == 0 && modrm.rm == 6)
	{
		//DISASM_OUTPUT(("[SetModRM16] Absolute addressing (displacement 0x%04X)\n", *(S16 *)Address));
		X86Instruction->Displacement = (S64)(*((S16 *)Address));
		if (IS_VALID_ADDRESS(X86Instruction->Displacement))
		{
			X86Instruction->HasFullDisplacement = TRUE;
			X86_SET_TARGET();
			Operand->Flags |= OP_GLOBAL;
		}
		X86_SET_ADDR();
		Operand->Flags |= OP_ADDRESS;
		INSTR_INC(2);
	}

	// Conditions:
	// (1) mod = 0 and rm != 6
	// (2) mod = 1-2 and rm = 0-7
	else
	{
		switch (modrm.mod)
		{
			case 0: // no displacement
				//DISASM_OUTPUT(("[SetModRM16] Indirect addressing (no displacement)\n"));
				break; 
			case 1: // 8-bit signed displacement
				//DISASM_OUTPUT(("[SetModRM16] Indirect addressing (displacement = 0x%02X, reg_rm = %d)\n", *(S8 *)Address, modrm.rm));
				X86Instruction->Displacement = (S64)(*((S8 *)Address));
				INSTR_INC(1); // increment Instruction->Length and address
				break;
			case 2: // 16-bit displacement
				//DISASM_OUTPUT(("[SetModRM16] Indirect addressing (displacement = 0x%04X, reg_rm = %d)\n", *(S16 *)Address, modrm.rm));
				X86Instruction->Displacement = (S64)(*((S16 *)Address));
				if (IS_VALID_ADDRESS(X86Instruction->Displacement))
				{
					Operand->Flags |= OP_GLOBAL;
					X86Instruction->HasFullDisplacement = TRUE;
				}
				INSTR_INC(2);
				break;
		}

		switch (modrm.rm)
		{
			case 0:
				//DISASM_OUTPUT(("[SetModRM16] Addressing mode [BX+SI]\n"));
				X86Instruction->BaseRegister = X86_REG_BX;
				X86Instruction->IndexRegister = X86_REG_SI;
				X86Instruction->HasIndexRegister = TRUE;
				break;
			case 1:
				//DISASM_OUTPUT(("[SetModRM16] Addressing mode [BX+DI]\n"));
				X86Instruction->BaseRegister = X86_REG_BX;
				X86Instruction->IndexRegister = X86_REG_DI;
				X86Instruction->HasIndexRegister = TRUE;
				break;
			case 2:
				//DISASM_OUTPUT(("[SetModRM16] Addressing mode [BP+SI]\n"));
				X86Instruction->BaseRegister = X86_REG_BP;
				X86Instruction->IndexRegister = X86_REG_SI;
				X86Instruction->HasIndexRegister = TRUE;
				X86_SET_SEG(REG_BP);
				break;
			case 3:
				//DISASM_OUTPUT(("[SetModRM16] Addressing mode [BP+DI]\n"));
				X86Instruction->BaseRegister = X86_REG_BP;
				X86Instruction->IndexRegister = X86_REG_DI;
				X86Instruction->HasIndexRegister = TRUE;
				X86_SET_SEG(REG_BP);
				break;
			case 4:
				//DISASM_OUTPUT(("[SetModRM16] Addressing mode [SI]\n"));
				X86Instruction->BaseRegister = X86_REG_SI;
				break;
			case 5:
				//DISASM_OUTPUT(("[SetModRM16] Addressing mode [DI]\n"));
				X86Instruction->BaseRegister = X86_REG_DI;
				break;
			case 6:
				//DISASM_OUTPUT(("[SetModRM16] Addressing mode [BP]\n"));
				X86Instruction->BaseRegister = X86_REG_BP;
				break;
			case 7:
				//DISASM_OUTPUT(("[SetModRM16] Addressing mode [BX]\n"));
				X86Instruction->BaseRegister = X86_REG_BX;
				break;
		}

		X86Instruction->HasBaseRegister = TRUE;
		Operand->Flags |= OP_ADDRESS | OP_REG;
		X86_SET_ADDR();
	}

	return Address;
}

// NOTE: Address points one byte after ModRM
static  U8 *SetModRM32(INSTRUCTION *Instruction, U8 *Address, INSTRUCTION_OPERAND *Operand, U32 OperandIndex, BOOL SuppressErrors)
{
	MODRM modrm;
	REX_MODRM rex_modrm;
	U32 i, ImmediateSize = 0;
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;

	if (X86Instruction->AddressSize == 2)
	{
		return SetModRM16(Instruction, Address, Operand, OperandIndex, SuppressErrors);
	}

	//DISASM_OUTPUT(("[SetModRM32] Length %d, modrm = 0x%02X\n", Instruction->Length, X86Instruction->modrm_b));
	modrm = X86Instruction->modrm;
	rex_modrm = X86Instruction->rex_modrm;

	//
	// Both operands are registers
	// Condition: mod = 3
	//
	if (modrm.mod == 3)
	{
		switch (Operand->Length)
		{
			case 8: Operand->Register = AMD64_64BIT_OFFSET + rex_modrm.rm; break;
			case 4: Operand->Register = X86_32BIT_OFFSET + rex_modrm.rm; CHECK_AMD64_REG(); break;
			case 2: Operand->Register = X86_16BIT_OFFSET + rex_modrm.rm; CHECK_AMD64_REG(); break;
			case 1: Operand->Register = X86_8BIT_OFFSET + rex_modrm.rm; if (X86Instruction->rex_b) CHECK_AMD64_REG(); break;
			default: assert(0); return NULL;
		}
		X86_SET_REG(rex_modrm.rm);
		Operand->Flags |= OP_REG;
	}

	// 
	// Address is an absolute address (technically a 32-bit offset from DS:0)
	// mod = 0 and rm = 5
	//
	else if (modrm.mod == 0 && modrm.rm == 5)
	{
		//DISASM_OUTPUT(("[SetModRM32] Absolute addressing (displacement 0x%08lX)\n", *(S32 *)Address));
		Operand->Flags |= OP_ADDRESS;
		X86Instruction->Displacement = (S64)*((S32 *)Address);
		INSTR_INC(4); // increment Instruction->Length and address

		if (IS_AMD64())
		{
			// RIP-relative addressing always replaced Disp32, even when using a 32-bit address space
			// (via address size override prefix)
			switch (X86Instruction->OperandSize)
			{
				case 8: Operand->Register = AMD64_REG_RIP; break;
				case 4: Operand->Register = X86_REG_EIP; break;
				case 2: Operand->Register = X86_REG_IP; break;
				default: assert(0); return NULL;
			}
			X86Instruction->BaseRegister = (X86_REGISTER)Operand->Register;
			X86Instruction->HasBaseRegister = TRUE;
			X86Instruction->Relative = TRUE;
			Operand->Flags |= OP_IPREL | OP_SIGNED | OP_REG;
			SANITY_CHECK_SEGMENT_OVERRIDE();
			if (!X86Instruction->HasSegmentOverridePrefix) X86Instruction->Segment = SEG_CS;
			X86Instruction->HasFullDisplacement = TRUE;

			// Since there may be an immediate value to follow, it is necessary
			// to determine the length in order get the proper offset
			//
			// Maybe there is a better way to do this, since this is wasteful
			// (the size of the immediate value will have to be decoded again later
			// in SetOperands)

			for (ImmediateSize = 0, i = OperandIndex+1; i < Instruction->OperandCount; i++)
			{
				if ((X86Instruction->Opcode.OperandFlags[i] & X86_AMODE_MASK) != AMODE_I) continue;
				else assert(!ImmediateSize);
				switch (X86Instruction->Opcode.OperandFlags[i] & X86_OPTYPE_MASK)
				{
					case OPTYPE_v:
						ImmediateSize = X86Instruction->OperandSize;
						break;
					case OPTYPE_z:
						switch (X86Instruction->OperandSize)
						{
							case 8: case 4: ImmediateSize = 4; break;
							case 2: ImmediateSize = 2; break;
							default: assert(0); return NULL;
						}
						break;
					case OPTYPE_b:
						ImmediateSize = 1;
						break;
					case OPTYPE_w:
						ImmediateSize = 2;
						break;
					case OPTYPE_1:
						break;
					default:
						assert(0);
						break;
				}
			}

			Operand->TargetAddress = ApplyDisplacement((U64)Address + ImmediateSize, Instruction);
		}
		else if (IS_VALID_ADDRESS(X86Instruction->Displacement))
		{
			X86_SET_TARGET();
			Operand->Flags |= OP_GLOBAL;
			X86Instruction->HasFullDisplacement = TRUE;
		}

		X86_SET_ADDR();
	}

	//
	// Addressing mode indicated by SIB byte
	// Condition: mod = 0-2 and rm = 4
	//
	else if (modrm.rm == 4)
	{
		// The X86_SET_*() is called from within SetSIB()
		Address = SetSIB(Instruction, Address, Operand, OperandIndex, SuppressErrors);
		if (!Address)
		{
			assert(0);
			return NULL;
		}

		if (X86Instruction->sib.base != 5) // if base == 5, the displacement is handled in SetSIB
		{
			switch (modrm.mod)
			{
				case 1: // 8-bit displacement
					//DISASM_OUTPUT(("[SetModRM32] After SIB: displacement 0x%02X\n", *((S8 *)Address)));
					X86Instruction->Displacement = (S64)(*((S8 *)Address));
					INSTR_INC(1); // increment Instruction->Length and address
					break;
				case 2: // 32-bit displacement
					//DISASM_OUTPUT(("[SetModRM32] After SIB: displacement 0x%08lX\n", *((S32 *)Address)));
					X86Instruction->Displacement = (S64)*((S32 *)Address);
					if (IS_VALID_ADDRESS(X86Instruction->Displacement))
					{
						Operand->Flags |= OP_GLOBAL;
						X86Instruction->HasFullDisplacement = TRUE;
					}
					INSTR_INC(4); // increment Instruction->Length and address
					break;
			}	
		}
	}

	// Indirect addressing
	// Conditions:
	// (1) mod = 0 and (rm = 0-3 or 6-7)
	// (2) mod = 1-2 and rm != 4
	else
	{
		switch (X86Instruction->AddressSize)
		{
			case 8: Operand->Register = AMD64_64BIT_OFFSET + rex_modrm.rm; break;
			case 4: Operand->Register = X86_32BIT_OFFSET + rex_modrm.rm; CHECK_AMD64_REG(); break;
			default: assert(0); return NULL;
		}
		X86Instruction->BaseRegister =(X86_REGISTER) Operand->Register;
		X86Instruction->HasBaseRegister = TRUE;
		Operand->Flags |= OP_ADDRESS | OP_REG;
		X86_SET_SEG(rex_modrm.rm);
		X86_SET_ADDR();

		switch (modrm.mod)
		{
			case 0: // no displacement
				//DISASM_OUTPUT(("[SetModRM32] Indirect addressing (no displacement, reg_rm = %d)\n", rex_modrm.rm));
				break; 
			case 1: // 8-bit signed displacement
				//DISASM_OUTPUT(("[SetModRM32] Indirect addressing (displacement = 0x%02X, reg_rm = %d)\n", *(S8 *)Address, rex_modrm.rm));
				X86Instruction->Displacement = (S64)(*((S8 *)Address));
				INSTR_INC(1); // increment Instruction->Length and address
				break;
			case 2: // 32-bit displacement
				//DISASM_OUTPUT(("[SetModRM32] Indirect addressing (displacement = 0x%08lX, reg_rm = %d)\n", *(S32 *)Address, rex_modrm.rm));
				X86Instruction->Displacement = (S64)*((S32 *)Address);
				if (IS_VALID_ADDRESS(X86Instruction->Displacement))
				{
					Operand->Flags |= OP_GLOBAL;
					X86Instruction->HasFullDisplacement = TRUE;
				}
				INSTR_INC(4); // increment Instruction->Length and address
				break;
		}
	}

	return Address;
}

// NOTE: Address points at SIB
static  U8 *SetSIB(INSTRUCTION *Instruction, U8 *Address, INSTRUCTION_OPERAND *Operand, U32 OperandIndex, BOOL SuppressErrors)
{
	REX rex;
	SIB sib;
	REX_SIB rex_sib;
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;

	X86Instruction->sib_b = *Address;
	SET_SIB(X86Instruction->sib, *Address);
	sib = X86Instruction->sib;
	rex = X86Instruction->rex;
	SET_REX_SIB(X86Instruction->rex_sib, rex, sib);
	rex_sib = X86Instruction->rex_sib;

	//if (!X86Instruction->rex_b) DISASM_OUTPUT(("[0x%08I64X] SIB = 0x%02X (scale=%d, index=%d, base=%d)\n", VIRTUAL_ADDRESS, *Address, sib.scale, sib.index, sib.base)); \
	//else DISASM_OUTPUT(("[0x%08I64X] SIB = 0x%02X (scale=%d, index=%d, base=%d)\n", VIRTUAL_ADDRESS, *Address, sib.scale, rex_sib.index, rex_sib.base)); \
	//DISASM_OUTPUT(("[SetSIB] Current instruction length = %d\n", Instruction->Length));

	Operand->Flags |= OP_ADDRESS;
	X86_SET_ADDR();
	INSTR_INC(1); // increment Instruction->Length and address

	if (sib.base == 5)
	{
		switch (X86Instruction->modrm.mod)
		{
			case 0:
				X86Instruction->Displacement = (S64)*((S32 *)Address);
				if (IS_VALID_ADDRESS(X86Instruction->Displacement))
				{
					X86Instruction->HasFullDisplacement = TRUE;
					X86_SET_TARGET();
					Operand->Flags |= OP_GLOBAL;
				}
				INSTR_INC(4);
				break;
			case 1:
				X86Instruction->Displacement = (S64)(*((S8 *)Address));
				if (rex_sib.base == 5)
				{
					switch (X86Instruction->AddressSize)
					{
						case 8: Operand->Register = AMD64_REG_RBP; break;
						case 4: Operand->Register = X86_REG_EBP; break;
						default: assert(0); return NULL;
					}
					X86_SET_SEG(REG_EBP);
				}
				else
				{
					Operand->Register = AMD64_REG_R13;
				}

				X86Instruction->BaseRegister =(X86_REGISTER) Operand->Register;
				X86Instruction->HasBaseRegister = TRUE;
				Operand->Flags |= OP_REG;
				INSTR_INC(1);
				break;
			case 2:
				X86Instruction->Displacement = (S64)*((S32 *)Address);
				if (rex_sib.base == 5)
				{
					switch (X86Instruction->AddressSize)
					{
						case 8: Operand->Register = AMD64_REG_RBP; break;
						case 4: Operand->Register = X86_REG_EBP; break;
						default: assert(0); return NULL;
					}
					X86_SET_SEG(REG_EBP);
				}
				else
				{
					Operand->Register = AMD64_REG_R13;
				}

				if (IS_VALID_ADDRESS(X86Instruction->Displacement))
				{
					Operand->Flags |= OP_GLOBAL;
					X86Instruction->HasFullDisplacement = TRUE;
				}
				X86Instruction->BaseRegister =(X86_REGISTER) Operand->Register;
				X86Instruction->HasBaseRegister = TRUE;
				Operand->Flags |= OP_REG;
				INSTR_INC(4);
				break;
		}
	}
	else
	{
		switch (X86Instruction->AddressSize)
		{
			case 8: Operand->Register = AMD64_64BIT_OFFSET + rex_sib.base; break;
			case 4: Operand->Register = X86_32BIT_OFFSET + rex_sib.base; CHECK_AMD64_REG(); break;
			default: assert(0); return NULL;
		}
		X86Instruction->BaseRegister =(X86_REGISTER) Operand->Register;
		X86Instruction->HasBaseRegister = TRUE;
		X86_SET_SEG(rex_sib.base);
		Operand->Flags |= OP_REG;
	}

	if (rex_sib.index != 4)
	{
		switch (X86Instruction->AddressSize)
		{
			case 8:
				X86Instruction->IndexRegister =(X86_REGISTER)( AMD64_64BIT_OFFSET + rex_sib.index);
				break;
			case 4:
				X86Instruction->IndexRegister =(X86_REGISTER)( X86_32BIT_OFFSET + rex_sib.index);
				break;
			default:
				fflush(stdout);
				assert(0);
				return NULL;
		}

		Operand->TargetAddress = 0;
		X86Instruction->HasIndexRegister = TRUE;
		//DISASM_OUTPUT(("[SetSIB] Index register = %s\n", X86_Registers[X86_32BIT_OFFSET + rex_sib.index]));

		switch (sib.scale)
		{
			case 0: X86Instruction->Scale = 1; break;
			case 1: X86Instruction->Scale = 2; break;
			case 2: X86Instruction->Scale = 4; break;
			case 3: X86Instruction->Scale = 8; break;
		}
		//DISASM_OUTPUT(("[SetSIB] Scale = %d\n", X86Instruction->Scale));
	}

	return Address;
}

static  U64 ApplyDisplacement(U64 Address, INSTRUCTION *Instruction)
{
	X86_INSTRUCTION *X86Instruction = &Instruction->X86;

#ifdef SUPPORT_WRAPAROUND
	U64 VirtualAddress = Address + Instruction->VirtualAddressDelta;
	switch (X86Instruction->OperandSize)
	{
		case 8:
		{
			U64 PreAddr = VirtualAddress;
			U64 PostAddr = PreAddr + X86Instruction->Displacement;
			return Address + (PostAddr - PreAddr);
		}
		case 4:
		{
			// We have to do this carefully...
			// If EIP = FFFFF000 and Displacement=2000 then the final IP should be 1000
			// due to wraparound
			U32 PreAddr = (U32)VirtualAddress;
			U32 PostAddr = PreAddr + (S32)X86Instruction->Displacement;
			return Address + (PostAddr - PreAddr);
		}
		case 2:
		{
			// We have to do this carefully...
			// If IP = F000 and Displacement=2000 then the final IP should be 1000
			// due to wraparound
			U16 PreAddr = (U16)VirtualAddress;
			U16 PostAddr = PreAddr + (S16)X86Instruction->Displacement;
			return Address + (PostAddr - PreAddr);
		}
		default:
			assert(0);
			return 0;
	}
#else
	return (Address + X86Instruction->Displacement);
#endif
}



static  BOOL IsValidLockPrefix(X86_INSTRUCTION *X86Instruction, U8 Opcode, U32 OpcodeLength, U8 Group, U8 OpcodeExtension)
{
	switch (OpcodeLength)
	{
		case 1:
			switch (X86_LockPrefix_1[Opcode])
			{
				case 0: // instruction can't be locked
					return FALSE;
				case 1: // instruction can be locked
					break;
				case GR:
					assert(Group);
					if (!X86_LockPrefix_Groups[Group-1][OpcodeExtension]) return FALSE;
					break;
				default:
					assert(0);
					return FALSE;
			}
			break;

		case 2:
		case 3:
			switch (X86_LockPrefix_2[Opcode])
			{
				case 0: // lock prefix is not acceptable
					return FALSE;
				case 1: // lock prefix allowed
					break;
				case GR:
					assert(Group);
					if (!X86_LockPrefix_Groups[Group-1][OpcodeExtension]) return FALSE;
					break;
				default:
					assert(0);
					return FALSE;
			}
			break;

		default:
			assert(0);
			return FALSE;
	}

	if (!X86Instruction->HasModRM || X86Instruction->modrm.mod == 3 || !X86Instruction->HasDstAddressing)
	{
		DISASM_OUTPUT(("[0x%08I64X] ERROR: Instruction \"%s\" with LOCK prefix has invalid ModRM addressing\n", VIRTUAL_ADDRESS, X86Instruction->Opcode.Mnemonic, X86Instruction->Instruction->Address));
		return FALSE;
	}

	return TRUE;
}

