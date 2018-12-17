#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Quarentine.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "SafeThreadHandle.h"
#include "ThreadFunctions.h"
#include "ThreadHelper.h"
#include "ProcessFunctions.h"

#include <rewolf-wow64ext/src/wow64ext.h>

#if 0
* Thread context
* Thread frame
* mismatch check with netgetnextthread
#endif

bool IScanner::IsScannedThread(DWORD_PTR dwThreadId)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	return HasInVector(m_vScannedThreadIDs, dwThreadId);
}

void IScanner::OnScanThread(HANDLE hProcess, SYSTEM_EXTENDED_THREAD_INFORMATION * pCurrThread)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	// Process params
	auto dwThreadId		= pCurrThread ? reinterpret_cast<DWORD_PTR>(pCurrThread->ThreadInfo.ClientId.UniqueThread) : DWORD_PTR(0);
	auto dwProcessId	= g_winapiApiTable->GetProcessId(hProcess);
	auto pThread		= std::unique_ptr<CThread>();
	auto ntStatus		= NTSTATUS(0x0);
	auto ulReadBytes	= 0UL;

	SCANNER_LOG(LL_SYS, "Thread scanner has been started! Target thread: %lld Target proc: %p(%u)", dwThreadId, hProcess, dwProcessId);

	// Check params
	if (!IS_VALID_HANDLE(hProcess) || !g_nmApp->DynamicWinapiInstance()->IsValidHandle(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Thread owner process is NOT alive!");
		goto _Complete;
	}

	if (!pCurrThread)
	{
		SCANNER_LOG(LL_ERR, "Thread param is NULL!");
		goto _Complete;
	}

	// check already processed
	if (IsScannedThread(dwThreadId))
	{
		SCANNER_LOG(LL_SYS, "Thread already scanned!");
		goto _Complete;
	}

	// Add to checked list
	m_vScannedThreadIDs.push_back(dwThreadId);

	// Scan routine


_Complete:
	return;
}

bool IScanner::EnumerateThreads(HANDLE hProcess)
{
	auto ntStat				= NTSTATUS(0x0);
	auto dwProcessInfoSize	= 2000UL;
	auto dwProcessId		= g_winapiApiTable->GetProcessId(hProcess);

	auto pProcessInfo = (PSYSTEM_EXTENDED_PROCESS_INFORMATION)malloc(dwProcessInfoSize);
	ZeroMemory(pProcessInfo, dwProcessInfoSize);

	while ((ntStat = g_winapiApiTable->NtQuerySystemInformation(SystemExtendedProcessInformation, pProcessInfo, dwProcessInfoSize, NULL)) == STATUS_INFO_LENGTH_MISMATCH)
	{
		dwProcessInfoSize *= 2;
		pProcessInfo = (PSYSTEM_EXTENDED_PROCESS_INFORMATION)realloc(pProcessInfo, dwProcessInfoSize);
	}

	if (!NT_SUCCESS(ntStat))
	{
		SCANNER_LOG(LL_ERR, "NtQuerySystemInformation failed! Error code: %u Ntstatus: %u", g_winapiApiTable->GetLastError(), ntStat);

		free(pProcessInfo);
		return false;
	}

	auto pIterator = pProcessInfo;
	while (pIterator->NextEntryOffset)
	{
		if (pIterator->ProcessId == dwProcessId)
		{
			auto pThread = pIterator->Threads;
			if (!pThread)
			{
				return false;
			}

			for (auto i = 0UL; i < pIterator->NumberOfThreads; i++)
			{
				OnScanThread(hProcess, pThread);

				pThread++;
			}
		}

		if (pIterator->NextEntryOffset == 0)
			break;

		pIterator = (PSYSTEM_EXTENDED_PROCESS_INFORMATION)((ULONG_PTR)pIterator + pIterator->NextEntryOffset);
	}

	free(pProcessInfo);

	return true;
}

bool IScanner::ScanProcessThreads(HANDLE hProcess)
{
	SCANNER_LOG(LL_SYS, "Thread scanner has been started! Target process: %u(%p)", g_winapiApiTable->GetProcessId(hProcess), hProcess);

	if (!IS_VALID_HANDLE(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Target handle is NOT valid!");
		return true;
	}

	if (!g_nmApp->DynamicWinapiInstance()->IsValidHandle(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Target process is NOT active!");
		return true;
	}

	auto szProcessName = CProcessFunctions::GetProcessName(hProcess);
	if (szProcessName.empty())
	{
		SCANNER_LOG(LL_ERR, "Process name read fail! Target process: %p Error: %u", hProcess, g_winapiApiTable->GetLastError());
		return false;
	}
	SCANNER_LOG(LL_SYS, "Process image name: %s", szProcessName.c_str());

	return EnumerateThreads(hProcess);
}

