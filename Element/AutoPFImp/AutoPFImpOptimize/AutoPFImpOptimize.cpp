// AutoPFImpOptimize.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "EC_NativeAPI.h"

#include "../include/moveagent.h"
#include "../include/movemap.h"

#include <vector>
#include <algorithm>

#include <AMemory.h>

#include <stdio.h>
#include <io.h>
#include <time.h>

#include <mmsystem.h>

using namespace AutoMove;
using namespace std;

CMoveAgent	g_AutoMoveAgent;
HMODULE		g_hNTDLL = NULL;

bool GetVirtualSize(SIZE_T &s);
int	 GetMemoryIncrease(SIZE_T nLast, SIZE_T nCurrent);
int	 GetTimeElapsed(DWORD dwLast, DWORD dwCurrent);
string GetMemoryAsString(int nMemory);
string GetTimeAsString(int nTime);
string GetTimeAsString(float fTime);

int main(int argc, char* argv[])
{
	if (argc != 5){
		printf("参数个数错误\n");
		return 0;
	}
	char szModuleFilePath[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, szModuleFilePath, sizeof(szModuleFilePath)/sizeof(szModuleFilePath[0]));
	
	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(szModuleFilePath, drive, dir, fname, ext);

	string strDir = drive;
	strDir += "\\";
	strDir += dir;

	string strFile = argv[1];
	if (_access(strFile.c_str(), 0) == -1){
		printf("文件不存在 %s\n", strFile.c_str());
		return 0;
	}
	_splitpath(strFile.c_str(), NULL, NULL, fname, NULL);

	string strResultFile = strDir;
	strResultFile += fname;
	strResultFile += ".csv";
	bool bCreated = (_access(strResultFile.c_str(), 0) == -1);
	FILE *fResult = fopen(strResultFile.c_str(), "a");
	if (!fResult){
		printf("打开输出结果文件失败 %s\n", strResultFile.c_str());
		return 0;
	}
	if (bCreated){
		fprintf(fResult, "nClusterSize, nEntranceWidth, 加载消耗内存(MB), 成功寻径样例数, 成功寻径平均时间(s), 失败寻径样例数, 失败寻径平均时间(s), 总平均寻径时间(s), 平均消耗内存(MB)\n");
	}

	int nClusterSize = atoi(argv[2]);
	int nEntranceWidth = atoi(argv[3]);
	int nMaxTestCase = atoi(argv[4]);
	
	printf("nClusterSize=%d, nEntraceWidth=%d\n", nClusterSize, nEntranceWidth);

	fprintf(fResult, "%d, %d", nClusterSize, nEntranceWidth);

	SIZE_T nInitMemory(0), nLastMemory(0), nCurMemory(0);
	unsigned __int64 nSumMemory(0);
	int nMemoryIncrease(0);

	while (true)
	{
		if (!GetVirtualSize(nLastMemory)){
			printf("获取内存消耗失败\n");
			break;
		}
		nInitMemory = nLastMemory;
		if (!g_AutoMoveAgent.Load(strFile.c_str())){
			printf("加载文件失败 %s\n", strFile.c_str());
			break;
		}
		if (!g_AutoMoveAgent.IsReady()){
			printf("NOT READY\n");
			break;
		}
		GetVirtualSize(nCurMemory);
		nMemoryIncrease = GetMemoryIncrease(nLastMemory, nCurMemory);
		fprintf(fResult, ", %s", GetMemoryAsString(nMemoryIncrease).c_str());
		
		vector<A3DPOINT2> starts;
		vector<A3DPOINT2> ends;
		string strTestCaseFile = strDir;
		strTestCaseFile += fname;
		strTestCaseFile += ".testcase";
		FILE *fTestCase = fopen(strTestCaseFile.c_str(), "r");
		if (!fTestCase){
			//	随机创建 nMaxTestCase 个测试用例
			fTestCase = fopen(strTestCaseFile.c_str(), "w");
			if (!fTestCase){
				printf("创建测试用例文件失败 %s\n", strTestCaseFile.c_str());
				break;
			}
			CMoveMap *pMoveMap = g_AutoMoveAgent.GetMoveMap();
			int nImageWidth = pMoveMap->GetMapWidth();
			int nImageLength = pMoveMap->GetMapLength();
			int nTestCase(0);
			A3DPOINT2 ptStart, ptEnd;
			srand(time(NULL));
			starts.reserve(nMaxTestCase);
			ends.reserve(nMaxTestCase);
			while (nTestCase < nMaxTestCase)
			{
				ptStart.x = rand() * (nImageWidth-1)/ RAND_MAX;
				ptStart.y = rand() * (nImageLength-1)/ RAND_MAX;
				ptEnd.x = rand() * (nImageWidth-1)/ RAND_MAX;
				ptEnd.y = rand() * (nImageLength-1)/ RAND_MAX;
				if (ptStart == ptEnd){
					continue;
				}
				if (pMoveMap->WhichLayer(ptStart, 0) < 0 ||
					pMoveMap->WhichLayer(ptEnd, 0) < 0){
					continue;
				}
				if (find(starts.begin(), starts.end(), ptStart) != starts.end() ||
					find(ends.begin(), ends.end(), ptEnd) != ends.end()){
					//	重复结点
					continue;
				}
				starts.push_back(ptStart);
				ends.push_back(ptEnd);
				++ nTestCase;
			}

			//	写入文件
			for (int i(0); i < nMaxTestCase; ++ i)
			{
				fprintf(fTestCase, "%d,%d,%d,%d\n", starts[i].x, starts[i].y, ends[i].x, ends[i].y);
			}
			fflush(fTestCase);
		}else{
			//	读取文件
			A3DPOINT2 ptStart, ptEnd;
			while (4 == fscanf(fTestCase, "%d,%d,%d,%d", &ptStart.x, &ptStart.y, &ptEnd.x, &ptEnd.y)){
				starts.push_back(ptStart);
				ends.push_back(ptEnd);
			}
			vector<A3DPOINT2>(starts).swap(starts);
			vector<A3DPOINT2>(ends).swap(ends);
		}
		fclose(fTestCase);

		//	开展测试
		float fFoundSumTime(0.0f), fFailedSumTime(0.0f);
		int nFound(0), nFailed(0);
		DWORD dwLastTime(0), dwCurrentTime(0);
		for (size_t u = 0; u < starts.size(); ++ u)
		{
			printf("testcase %d...\n", u);

			dwLastTime = timeGetTime();
			GetVirtualSize(nLastMemory);
			
			bool bFound(false);
			int startLayer = g_AutoMoveAgent.WhichLayer(starts[u], 0);
			int endLayer = g_AutoMoveAgent.WhichLayer(ends[u], 0);
			if (startLayer >= 0 && endLayer >= 0 &&
				g_AutoMoveAgent.SetStartEnd(starts[u], startLayer, ends[u], endLayer) &&
				g_AutoMoveAgent.Search()){
				bFound = true;
			}

			dwCurrentTime = timeGetTime();
			GetVirtualSize(nCurMemory);
			nSumMemory += nCurMemory;
			if (bFound){
				fFoundSumTime += GetTimeElapsed(dwLastTime, dwCurrentTime);
				++ nFound;
				g_AutoMoveAgent.ResetSearch();
			}else{
				fFailedSumTime += GetTimeElapsed(dwLastTime, dwCurrentTime);
				++ nFailed;
			}
		}
		fprintf(fResult, ", %d, %s", nFound, GetTimeAsString(nFound <= 0 ? 0.0f : fFoundSumTime/nFound).c_str());
		fprintf(fResult, ", %d, %s", nFailed, GetTimeAsString(nFailed <= 0 ? 0.0f : fFailedSumTime/nFailed).c_str());
		fprintf(fResult, ", %s", GetTimeAsString(starts.empty() ? 0.0f : (fFoundSumTime+fFailedSumTime)/starts.size()).c_str());
		if (starts.empty()){
			fprintf(fResult, "\n");
		}else{
			unsigned __int64 nAvgMemory = nSumMemory/starts.size();
			nAvgMemory -= nInitMemory;
			fprintf(fResult, ", %s\n", GetMemoryAsString((int)nAvgMemory).c_str());
		}

		break;
	}

	fflush(fResult);
	fclose(fResult);

	if (g_hNTDLL){
		FreeLibrary(g_hNTDLL);
		g_hNTDLL = NULL;
	}

	return 0;
}

bool GetVirtualSize(SIZE_T &s)
{	
	bool bRet(false);
	
	//	尝试初始化 NtQuerySystemInformation
	static bool s_bInit = false;
	static PFNNtQuerySystemInformation NtQuerySystemInformation = NULL;
	if (!s_bInit){
		s_bInit = true;
		g_hNTDLL = LoadLibraryW(L"ntdll.dll");
		if (!g_hNTDLL){
			printf("CECOptimize::GetVirtualSize,load ntdll.dll failed(GetLastError()=%u)\n", GetLastError());
		}else{
			NtQuerySystemInformation = (PFNNtQuerySystemInformation)GetProcAddress(g_hNTDLL, "NtQuerySystemInformation");
			if (!NtQuerySystemInformation){
				printf("CECOptimize::GetVirtualSize,Get NtQuerySystemInformation failed(GetLastError()=%u)\n", GetLastError());
			}
		}
	}
	
	//	使用 NtQuerySystemInformation 枚举并获取当前进程信息
	while (NtQuerySystemInformation)
	{
		static ULONG initialBufferSize = 0x4000;
		NTSTATUS status;
		PVOID buffer;
		ULONG bufferSize;
		
		bufferSize = initialBufferSize;
		buffer = (BYTE*)a_malloctemp(bufferSize);
		
		while (TRUE)
		{
			status = NtQuerySystemInformation(
				SystemProcessesAndThreadsInformation,
				buffer,
				bufferSize,
				&bufferSize
				);
			
			if (status == STATUS_BUFFER_TOO_SMALL || status == STATUS_INFO_LENGTH_MISMATCH){
				a_freetemp(buffer);
				buffer = a_malloctemp(bufferSize);
			}else{
				break;
			}
		}
		
		if (!NT_SUCCESS(status)){
			a_freetemp(buffer);
			break;
		}		
		if (bufferSize <= 0x20000){
			initialBufferSize = bufferSize;
		}
		
		DWORD currentProcess = GetCurrentProcessId();		
		PSYSTEM_PROCESSES pSysProcess = (PSYSTEM_PROCESSES)buffer ;
		while (pSysProcess)
		{
			if (pSysProcess->ProcessId == currentProcess){
				s = pSysProcess->VmCounters.VirtualSize;
				bRet = true;
				break;
			}
			if (pSysProcess->NextEntryDelta == 0){
				break;
			}
			pSysProcess = (PSYSTEM_PROCESSES)( (DWORD)pSysProcess + pSysProcess->NextEntryDelta ) ;
		}
		
		a_freetemp(buffer);
		break;
	}
	
	return bRet;
}

int GetMemoryIncrease(SIZE_T nLast, SIZE_T nCurrent)
{
	if (nCurrent >= nLast){
		return (int)(nCurrent - nLast);
	}
	return -(int)(nLast - nCurrent);
}

int	 GetTimeElapsed(DWORD dwLast, DWORD dwCurrent)
{
	if (dwCurrent >= dwLast){
		return (int)(dwCurrent - dwLast);
	}
	return -(int)(dwLast - dwCurrent);
}

string GetMemoryAsString(int nMemory)
{	
	string str;
	if (nMemory != 0){
		static char buffer[128] = {0};
		sprintf(buffer, "%.3f", nMemory/1024.0f/1024.0f);
		str = buffer;
	}else{
		str = "0";
	}
	return str;
}

string GetTimeAsString(int nTime)
{
	static char buffer[128] = {0};
	sprintf(buffer, "%.3f", nTime*0.001f);
	return buffer;
}

string GetTimeAsString(float fTime)
{
	static char buffer[128] = {0};
	sprintf(buffer, "%.3f", fTime*0.001f);
	return buffer;
}