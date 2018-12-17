#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "NtApiWrapper.h"
#include "Defines.h"

#include <rewolf-wow64ext\src\wow64ext.h>

#define DELAY_ONE_MICROSECOND 	(-10)
#define DELAY_ONE_MILLISECOND	(DELAY_ONE_MICROSECOND * 1000)

static int gs_iProcessorCount = 0;

inline bool IsX64()
{
	SYSTEM_INFO SysInfo;
	g_winapiApiTable->GetNativeSystemInfo(&SysInfo);

	return (SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64);
}

inline int GetProcessorCount()
{
	SYSTEM_BASIC_INFORMATION sbi;
	auto ntStatus = g_winapiApiTable->NtQuerySystemInformation(SystemBasicInformation, &sbi, sizeof(sbi), NULL);
	if (NT_SUCCESS(ntStatus))
		return sbi.NumberOfProcessors;
	return 1;
}


HANDLE CNT::OpenProcess(DWORD dwDesiredAccess, DWORD dwProcessId)
{
	OBJECT_ATTRIBUTES oa;
	InitializeObjectAttributes(&oa, 0, 0, 0, 0);

	HANDLE hProcess = NULL;
	CLIENT_ID cid = { reinterpret_cast<HANDLE>(dwProcessId), NULL };

	auto ntStatus = g_winapiApiTable->NtOpenProcess(&hProcess, dwDesiredAccess, &oa, &cid);
	g_winapiApiTable->SetLastError(ntStatus);

	return hProcess;
}

HANDLE CNT::OpenProcessToken(HANDLE hProcess, ACCESS_MASK desiredAccess)
{
	HANDLE hProcessToken = 0;

	auto ntStatus = g_winapiApiTable->NtOpenProcessToken(hProcess, desiredAccess, &hProcessToken);
	if (NT_SUCCESS(ntStatus))
		return hProcessToken;

	g_winapiApiTable->SetLastError(ntStatus);
	return nullptr;
}

HANDLE CNT::OpenThread(DWORD dwDesiredAccess, DWORD dwThreadId)
{
	OBJECT_ATTRIBUTES oa;
	InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);

	HANDLE hThread = NULL;
	CLIENT_ID cid = { NULL, reinterpret_cast<HANDLE>(dwThreadId) };

	auto ntStatus = g_winapiApiTable->NtOpenThread(&hThread, dwDesiredAccess, &oa, &cid);
	g_winapiApiTable->SetLastError(ntStatus);

	return hThread;
}

HANDLE CNT::CreateThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParam, PDWORD_PTR pdwThreadId)
{
	HANDLE hThread = 0;
	CLIENT_ID cid = { 0, 0 };

#ifdef _DEBUG
	DWORD dwFlag = 0; /* Allow debugger access for debug build */
#else
	DWORD dwFlag = 0x00000004; /* HideFromDebugger */
#endif

	auto ntStatus = g_winapiApiTable->RtlCreateUserThread(NtCurrentProcess, NULL, 0, 0, 0, 0, lpStartAddress, lpParam, &hThread, &cid);
	g_winapiApiTable->SetLastError(ntStatus);

	if (NT_SUCCESS(ntStatus))
	{
		if (pdwThreadId) *pdwThreadId = DWORD_PTR(cid.UniqueThread);
		return hThread;
	}

	return nullptr;
}

bool CNT::CreateThreadAndWait(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParam, DWORD dwDelay, LPDWORD pdwExitCode)
{
	auto hThread = CreateThread(lpStartAddress, lpParam, nullptr);
	if (!IS_VALID_HANDLE(hThread))
	{
		DEBUG_LOG(LL_ERR, "RtlCreateUserThread fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	if (WaitObject(hThread, dwDelay) != WAIT_OBJECT_0)
	{
		DEBUG_LOG(LL_ERR, "WaitForSingleObject fail! Error: %u", g_winapiApiTable->GetLastError());
		CloseHandle(hThread);
		return false;
	}

	THREAD_BASIC_INFORMATION tbi;
	auto ntStatus = g_winapiApiTable->NtQueryInformationThread(hThread, ThreadBasicInformation, &tbi, sizeof(tbi), NULL);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "NtQueryInformationThread fail! Status: %p", ntStatus);
		CloseHandle(hThread);
		return false;
	}

	CloseHandle(hThread);
	if (pdwExitCode) *pdwExitCode = tbi.ExitStatus;
	return true;
}


bool CNT::SuspendThread(HANDLE hThread)
{
	auto ulCount = 0UL;
	auto ntStatus = g_winapiApiTable->NtSuspendThread(hThread, &ulCount);
	return NT_SUCCESS(ntStatus);
}

bool CNT::ResumeThread(HANDLE hThread, bool bLoop)
{
	auto ulCount = 0UL;

resumeRoutine:
	auto ntStatus = g_winapiApiTable->NtResumeThread(hThread, &ulCount);
	if (NT_SUCCESS(ntStatus))
	{
		if (bLoop && ulCount > 0)
			goto resumeRoutine;
		return true;
	}

	return false;
}

bool CNT::SuspendProcess(HANDLE hThread)
{
	auto ntStatus = g_winapiApiTable->NtSuspendProcess(hThread);
	return NT_SUCCESS(ntStatus);
}

bool CNT::ResumeProcess(HANDLE hProcess)
{
	auto ntStatus = g_winapiApiTable->NtResumeProcess(hProcess);
	return NT_SUCCESS(ntStatus);
}

bool CNT::CloseHandle(HANDLE hTarget)
{
	auto ntStatus = g_winapiApiTable->NtClose(hTarget);
	return NT_SUCCESS(ntStatus);
}

bool CNT::WaitObject(HANDLE hTarget, DWORD dwDelay)
{
	LARGE_INTEGER liDelay = { DELAY_ONE_MILLISECOND * dwDelay, -1 };

	auto ntStatus = g_winapiApiTable->NtWaitForSingleObject(hTarget, FALSE, &liDelay);
	return NT_SUCCESS(ntStatus);
}

bool CNT::Sleep(DWORD dwDelay)
{
	LARGE_INTEGER liDelay = { DELAY_ONE_MILLISECOND * dwDelay, -1 };

	auto ntStatus = g_winapiApiTable->NtDelayExecution(FALSE, &liDelay);
	return NT_SUCCESS(ntStatus);
}

void CNT::YieldCPU()
{
	if (!gs_iProcessorCount)
		gs_iProcessorCount = GetProcessorCount();

	if (gs_iProcessorCount > 1)
		this->Sleep(1);
	else
		g_winapiApiTable->NtYieldExecution();
}

bool CNT::TerminateThread(HANDLE hThread, NTSTATUS ulExitStatus)
{
	auto ntStatus = g_winapiApiTable->NtTerminateThread(hThread, ulExitStatus);
	return NT_SUCCESS(ntStatus);
}

bool CNT::TerminateProcess(HANDLE hProcess, NTSTATUS ulExitStatus)
{
	auto ntStatus = g_winapiApiTable->NtTerminateProcess(hProcess, ulExitStatus);
	return NT_SUCCESS(ntStatus);
}

NTSTATUS CNT::CreateFile(PHANDLE hFile, LPWSTR FilePath, ACCESS_MASK AccessMask, ULONG FileAttributes, ULONG ShareAccess, ULONG DispositionFlags, ULONG CreateOptions)
{
	OBJECT_ATTRIBUTES ObjectAttributes	= { 0 };
	IO_STATUS_BLOCK   IoStatusBlock		= { 0 };
	LARGE_INTEGER     AllocationSize	= { 0 };

	auto ntStatus = g_winapiApiTable->NtCreateFile(hFile, AccessMask, &ObjectAttributes, &IoStatusBlock, &AllocationSize, FileAttributes, ShareAccess, DispositionFlags, CreateOptions, NULL, 0);
	return ntStatus;
}

NTSTATUS CNT::QueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength)
{
	auto bWow64Ret = BOOL(FALSE);
	if (g_winapiApiTable->IsWow64Process(NtCurrentProcess, &bWow64Ret) && bWow64Ret)
	{
		return g_winapiApiTable->NtWow64QueryInformationProcess64(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
	}
	return g_winapiApiTable->NtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}


PVOID CNT::Alloc(SIZE_T ulSize, const std::string & szName)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto pProcessHeap = g_winapiApiTable->GetProcessHeap();
	auto pMem = g_winapiApiTable->RtlAllocateHeap(pProcessHeap, HEAP_ZERO_MEMORY, ulSize);
//	DEBUG_LOG(LL_SYS, "Memory allocated: %p(%u) on: %p Name: %s", pMem, ulSize, pProcessHeap, szName.c_str());

	auto pBlockContext = std::make_shared<SHeapBlockContext>();
	if (IS_VALID_SMART_PTR(pBlockContext))
	{
		pBlockContext->pBase = pMem;
		strcpy(pBlockContext->szName, szName.c_str());

		m_vHeapBlockPool.push_back(pBlockContext);
	}

	return pMem;
}
bool CNT::Free(PVOID pMemBase)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	for (const auto & pCurrBlock : m_vHeapBlockPool)
	{
		if (IS_VALID_SMART_PTR(pCurrBlock))
		{
			if (pCurrBlock->pBase == pMemBase)
			{
				m_vHeapBlockPool.erase(std::remove(m_vHeapBlockPool.begin(), m_vHeapBlockPool.end(), pCurrBlock), m_vHeapBlockPool.end());
			}
		}
	}

	auto bRet			= BOOL(FALSE);
	auto pProcessHeap	= g_winapiApiTable->GetProcessHeap();
	if (pProcessHeap)
	{
		bRet = g_winapiApiTable->RtlFreeHeap(pProcessHeap, 0, pMemBase);
		if (bRet)
		{
//			ZeroMemory(pMemBase, sizeof(*pMemBase));
//			DEBUG_LOG(LL_SYS, "Memory free'd: %p on: %p", pMemBase, pProcessHeap);
		}
		else
		{
			DEBUG_LOG(LL_ERR, "Memory can NOT free'd: %p on: %p", pMemBase, pProcessHeap);
		}
	}

	return bRet == TRUE ? true : false;
}
PVOID CNT::ReAlloc(PVOID pMemBase, SIZE_T ulSize, const std::string & szName)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	for (const auto & pCurrBlock : m_vHeapBlockPool)
	{
		if (IS_VALID_SMART_PTR(pCurrBlock))
		{
			if (pCurrBlock->pBase == pMemBase)
			{
				m_vHeapBlockPool.erase(std::remove(m_vHeapBlockPool.begin(), m_vHeapBlockPool.end(), pCurrBlock), m_vHeapBlockPool.end());
			}
		}
	}

	auto pProcessHeap = g_winapiApiTable->GetProcessHeap();
	auto pNewMem = HeapReAlloc(pProcessHeap, HEAP_ZERO_MEMORY, pMemBase, ulSize);

	//	DEBUG_LOG(LL_SYS, "Memory re-allocated: %p(%u) on: %p Name: %s", pNewMem, ulSize, pProcessHeap, szName.c_str());

	auto pBlockContext = std::make_shared<SHeapBlockContext>();
	if (IS_VALID_SMART_PTR(pBlockContext))
	{
		pBlockContext->pBase = pNewMem;
		strcpy(pBlockContext->szName, szName.c_str());

		m_vHeapBlockPool.push_back(pBlockContext);
	}

	return pNewMem;
}
PVOID CNT::SmartAlloc(SIZE_T ulSize, const std::string & szName)
{
	return nullptr;
}
auto CNT::GetHeapBlocks()
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	return m_vHeapBlockPool;
}

