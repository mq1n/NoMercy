#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Quarentine.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "WindowEnumerator.h"
#include "ProcessFunctions.h"
#include "SafeProcessHandle.h"

// https://securityxploded.com/hidden-process-detection.php

inline bool IsHiddenProcess(DWORD dwProcessId)
{
	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator))
	{
		DEBUG_LOG(LL_ERR, "windowEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto hProc = g_winapiApiTable->OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
	if (!IS_VALID_HANDLE(hProc))
	{
		auto vAltWindows = windowEnumerator->EnumerateWindows(dwProcessId);
		if (vAltWindows.empty() == false)
		{
//			DEBUG_LOG(LL_ERR, "Hidden process detected. PID: %u", dwProcessId);
			return true;
		}
	
	}
	return false;
}

bool IScanner::CheckHiddenProcessWindows()
{
	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator)) 
	{
		DEBUG_LOG(LL_ERR, "windowEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto vWindows = windowEnumerator->EnumerateWindows();
	if (vWindows.empty()) {
		DEBUG_LOG(LL_ERR, "Can not enumerated windows");
		return false;
	}

	for (const auto & hWnd : vWindows)
	{
		auto dwPID = 0UL;
		g_winapiApiTable->GetWindowThreadProcessId(hWnd, &dwPID);

		if (dwPID < 5)
			continue;

		if (IsWindow(hWnd) && IsHiddenProcess(dwPID))
		{
			DEBUG_LOG(LL_ERR, "Hidden process detected. PID: %u", dwPID);
			return true;
		}
	}
	return false;
}

bool IsProcessLinked(DWORD dwProcessId, PSYSTEM_PROCESS_INFORMATION pInfos)
{
	auto pCurrent = pInfos;

	while (true) 
	{
		if ((DWORD)pCurrent->UniqueProcessId == dwProcessId)
			return true;

		if (pCurrent->NextEntryOffset == 0)
			break;
		pCurrent = (PSYSTEM_PROCESS_INFORMATION)((DWORD_PTR)pCurrent + pCurrent->NextEntryOffset);
	}

	return false;
}

bool CheckHiddenProcessHandles()
{
	auto dwLength = 1UL;
	auto pProcessInfos = (PSYSTEM_PROCESS_INFORMATION)malloc(dwLength);
	if (!pProcessInfos)
		return false;

	auto ntStatus = g_winapiApiTable->NtQuerySystemInformation(SystemProcessInformation, pProcessInfos, dwLength, &dwLength);
	if (!NT_SUCCESS(ntStatus) || ntStatus != STATUS_INFO_LENGTH_MISMATCH)
	{
		free(pProcessInfos);
		return false;
	}

	if (ntStatus == STATUS_INFO_LENGTH_MISMATCH)
	{
		pProcessInfos = (PSYSTEM_PROCESS_INFORMATION)realloc(pProcessInfos, dwLength);
	}

	if (!pProcessInfos)
		return false;

	ntStatus = g_winapiApiTable->NtQuerySystemInformation(SystemProcessInformation, pProcessInfos, dwLength, &dwLength);
	if (!NT_SUCCESS(ntStatus))
	{
		free(pProcessInfos);
		return false;
	}

	// ---

	dwLength = 1;
	auto pHandleInfos = (PSYSTEM_HANDLE_INFORMATION)malloc(dwLength);
	if (!pHandleInfos)
		return false;

	ntStatus = g_winapiApiTable->NtQuerySystemInformation(SystemHandleInformation, pHandleInfos, dwLength, &dwLength);
	if (!NT_SUCCESS(ntStatus) || ntStatus != STATUS_INFO_LENGTH_MISMATCH)
	{
		free(pProcessInfos);
		return false;
	}

	if (ntStatus == STATUS_INFO_LENGTH_MISMATCH)
	{
		pHandleInfos = (PSYSTEM_HANDLE_INFORMATION)malloc(dwLength);
	}

	if (!pHandleInfos)
		return false;

	ntStatus = g_winapiApiTable->NtQuerySystemInformation(SystemHandleInformation, pHandleInfos, dwLength, &dwLength);
	if (!NT_SUCCESS(ntStatus))
	{
		free(pHandleInfos);
		return false;
	}

	for (ULONG i = 0; i < pHandleInfos->HandleCount; i++) 
	{
		auto hCurrHandle	= pHandleInfos->Handles[i];
		auto hDupHandle		= HANDLE(INVALID_HANDLE_VALUE);
		auto hOwnerHandle	= HANDLE(INVALID_HANDLE_VALUE);

		if (hCurrHandle.ProcessId == g_winapiApiTable->GetCurrentProcessId()) /* Itself */
			continue;

		if (hCurrHandle.ProcessId < 5) /* System */
			continue;

		if (hCurrHandle.ObjectTypeNumber != 0x5 && hCurrHandle.ObjectTypeNumber != 0x7) // Just process handles 
			continue;

		hOwnerHandle = g_winapiApiTable->OpenProcess(PROCESS_DUP_HANDLE, FALSE, hCurrHandle.ProcessId);
		if (!IS_VALID_HANDLE(hOwnerHandle))
			continue;

		ntStatus = g_winapiApiTable->NtDuplicateObject(hOwnerHandle, (HANDLE)hCurrHandle.Handle, GetCurrentProcess(), &hDupHandle, 0, 0, DUPLICATE_SAME_ACCESS | DUPLICATE_SAME_ATTRIBUTES);
		if (!NT_SUCCESS(ntStatus))
		{
			g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hDupHandle);
			g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hOwnerHandle);
			continue;
		}

		auto dwProcessId = CProcessFunctions::GetProcessIdNative(hDupHandle);
		if (false == IsProcessLinked(dwProcessId, pProcessInfos))
		{
			DEBUG_LOG(LL_ERR, "Hidden process detected. PID: %u", dwProcessId);
			return true;
		}

		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hDupHandle);
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hOwnerHandle);
	}

	free(pHandleInfos);
	free(pProcessInfos);

	return false;
}

bool CheckHiddenProcessBruteforce()
{
	for (auto i = 4UL; i < MAXDWORD; i += 4UL)
	{
		auto hProcess = g_winapiApiTable->OpenProcess(SYNCHRONIZE, FALSE, i);
		if (IS_VALID_HANDLE(hProcess))
		{
			if (IsHiddenProcess(i))
			{
				DEBUG_LOG(LL_ERR, "Hidden process detected. PID: %u", i);
				return true;
			}
			g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
		}
	}
	return false;
}

bool CheckHiddenProcessNative()
{
	auto processEnumerator = std::make_unique<CSafeProcessHandle>(SYNCHRONIZE);
	if (!IS_VALID_SMART_PTR(processEnumerator))
		return false;

	auto vProcessList = processEnumerator->EnumerateProcesses(true);
	if (vProcessList.empty())
		return false;

	// --

	auto dwLength = 1UL;
	auto pProcessInfos = (PSYSTEM_PROCESS_INFORMATION)malloc(dwLength);
	if (!pProcessInfos)
		return false;

	auto ntStatus = g_winapiApiTable->NtQuerySystemInformation(SystemProcessInformation, pProcessInfos, dwLength, &dwLength);
	if (!NT_SUCCESS(ntStatus) || ntStatus != STATUS_INFO_LENGTH_MISMATCH)
	{
		free(pProcessInfos);
		return false;
	}

	if (ntStatus == STATUS_INFO_LENGTH_MISMATCH)
	{
		pProcessInfos = (PSYSTEM_PROCESS_INFORMATION)realloc(pProcessInfos, dwLength);
	}

	if (!pProcessInfos)
		return false;

	ntStatus = g_winapiApiTable->NtQuerySystemInformation(SystemProcessInformation, pProcessInfos, dwLength, &dwLength);
	if (!NT_SUCCESS(ntStatus))
	{
		free(pProcessInfos);
		return false;
	}

	// ---

	for (const auto & hCurrProc : vProcessList)
	{
		auto dwProcessId = CProcessFunctions::GetProcessIdNative(hCurrProc);
		if (!dwProcessId)
			continue;

		if (false == IsProcessLinked(dwProcessId, pProcessInfos))
		{
			DEBUG_LOG(LL_ERR, "Hidden process detected. PID: %u", dwProcessId);
			return true;
		}
	}

	free(pProcessInfos);

	return false;
}
