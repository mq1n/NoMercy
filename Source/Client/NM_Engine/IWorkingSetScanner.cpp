#include "NM_Index.h"
#include "NM_Main.h"
#include "Defines.h"
#include "ProcessFunctions.h"

#if 0
void ShowWSChanges(HANDLE hTargetProcess)
{
	auto dwWatchInfoSize = DWORD(sizeof(PSAPI_WS_WATCH_INFORMATION_EX) * 1000);
	auto WatchInfoEx = (PPSAPI_WS_WATCH_INFORMATION_EX)malloc(dwWatchInfoSize);
	if (!WatchInfoEx)
	{
		printf("WatchInfoEx allocation fail! Size: %u Error: %u\n", dwWatchInfoSize, GetLastError());
		return;
	}

	while (1)
	{
		memset(WatchInfoEx, 0, dwWatchInfoSize);

		if (!GetWsChangesEx(hTargetProcess, WatchInfoEx, &dwWatchInfoSize))
		{
			auto dwErrorCode = GetLastError();

			if (dwErrorCode == ERROR_NO_MORE_ITEMS)
			{
				Sleep(1);
				continue;
			}

			if (dwErrorCode != ERROR_INSUFFICIENT_BUFFER)
			{
				printf("GetWsChangesEx fail! Error: %u\n", dwErrorCode);
				free(WatchInfoEx);
				return;
			}

			dwWatchInfoSize *= 2;

			WatchInfoEx = (PPSAPI_WS_WATCH_INFORMATION_EX)realloc(WatchInfoEx, dwWatchInfoSize);
			if (!WatchInfoEx)
			{
				printf("WatchInfoEx reallocation fail! Size: %u Error: %u\n", dwWatchInfoSize, GetLastError());
				if (WatchInfoEx)
					free(WatchInfoEx);
				return;
			}

			continue;
		}

		for (std::size_t i = 0;; ++i)
		{
			auto info = &WatchInfoEx[i];
			if (!info->BasicInfo.FaultingPc)
				break;

			DEBUG_LOG(LL_SYS, "-----------------------------------\n");
			DEBUG_LOG(LL_SYS, "TID: %u Pc: %p Va: %p\n", (info->FaultingThreadId), info->BasicInfo.FaultingPc, info->BasicInfo.FaultingVa);

			auto hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, (DWORD)info->FaultingThreadId);
			DEBUG_LOG(LL_SYS, "hThread: %p", hThread);

			if (hThread && hThread != INVALID_HANDLE_VALUE)
			{
				THREAD_BASIC_INFORMATION ThreadInfo;
				auto ntStatus = g_winapiApiTable->NtQueryInformationThread(hThread, ThreadBasicInformation, &ThreadInfo, sizeof(ThreadInfo), NULL);
				DEBUG_LOG(LL_SYS, "ntStatus: %u", ntStatus);

				if (NT_SUCCESS(ntStatus))
				{
					auto dwPID = (DWORD)ThreadInfo.ClientId.UniqueProcess;
					auto dwCurrPID = GetCurrentProcessId();
					DEBUG_LOG(LL_SYS, "Access owner PID: %u Current PID: %u", dwPID, dwCurrPID);

					if (dwPID != GetCurrentProcessId())
					{
						auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
						DEBUG_LOG(LL_SYS, "hProcess: %p", hProcess);

						if (hProcess && hProcess != INVALID_HANDLE_VALUE)
						{
							LPVOID pCurrentThreadAddress = 0;
							ntStatus = g_winapiApiTable->NtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &pCurrentThreadAddress, sizeof(pCurrentThreadAddress), NULL);
							DEBUG_LOG(LL_SYS, "Ntstatus: %u Start adress: %p", ntStatus, pCurrentThreadAddress);

							auto szProcessName = CProcessFunctions::GetProcessName(hProcess);

							wchar_t wszImagePath[MAX_PATH];
							if (GetMappedFileNameW(hProcess, pCurrentThreadAddress, wszImagePath, MAX_PATH) != 0) {
								DEBUG_LOG(LL_SYS, "Process name: %s Thread owner: %ls", szProcessName.c_str(), wszImagePath);
							}

							CloseHandle(hProcess);
						}
					}
				}
				CloseHandle(hThread);
			}
		}
	}
}



int InitWSWatcher()
{
	auto hNtdll = LoadLibraryA("ntdll");
	if (!hNtdll) {
		printf("hNtdll fail! Last error: %u\n", GetLastError());
		return 0;
	}

#if defined(_M_IX86) // Not supported
	BOOL Wow64Process = FALSE;
	if (IsWow64Process(NtCurrentProcess, &Wow64Process) && Wow64Process)
	{
		printf("This process cannot be run under Wow64.\n");
		return 0;
	}
#endif

	if (!InitializeProcessForWsWatch(NtCurrentProcess))
	{
		printf("InitializeProcessForWsWatch fail. Error: %u\n", GetLastError());
		return 0;
	}

	ShowWSChanges(NtCurrentProcess);

	Sleep(INFINITE);
	return 0;
}
#endif

