#include "NM_Index.h"
#include "NM_Main.h"
#include "ThreadFunctions.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "SafeThreadHandle.h"

DWORD CThreadFunctions::GetThreadOwnerProcessId(DWORD dwThreadID)
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	THREADENTRY32 ti = { 0 };
	ti.dwSize = sizeof(ti);

	if (g_winapiApiTable->Thread32First(hSnap, &ti))
	{
		do 
		{
			if (dwThreadID == ti.th32ThreadID) 
			{
				g_winapiApiTable->CloseHandle(hSnap);
				return ti.th32OwnerProcessID;
			}
		} while (g_winapiApiTable->Thread32Next(hSnap, &ti));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return 0;
}

bool CThreadFunctions::ThreadIsItAlive(DWORD dwThreadID)
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	THREADENTRY32 ti = { 0 };
	ti.dwSize = sizeof(ti);

	if (g_winapiApiTable->Thread32First(hSnap, &ti))
	{
		do 
		{
			if (dwThreadID == ti.th32ThreadID)
			{
				g_winapiApiTable->CloseHandle(hSnap);
				return true;
			}
		} while (g_winapiApiTable->Thread32Next(hSnap, &ti));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return false;
}

DWORD CThreadFunctions::GetThreadStartAddress(HANDLE hThread)
{
	DWORD dwCurrentThreadAddress = 0;
	auto ntStatus = g_winapiApiTable->NtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &dwCurrentThreadAddress, sizeof(dwCurrentThreadAddress), NULL);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "GetThreadStartAddress fail! Thread: %p Status: %p", hThread, ntStatus);
		return dwCurrentThreadAddress;
	}
	return dwCurrentThreadAddress;
}

DWORD CThreadFunctions::GetMainThreadId()
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	THREADENTRY32 ti = { 0 };
	ti.dwSize = sizeof(ti);

	auto pIDH = (IMAGE_DOS_HEADER*)g_winapiModuleTable->hBaseModule;
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pINH = (IMAGE_NT_HEADERS32*)(pIDH->e_lfanew + (DWORD)pIDH);
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return false;

	auto pEntryPoint = pINH->OptionalHeader.AddressOfEntryPoint + pINH->OptionalHeader.ImageBase;
	if (!pEntryPoint)
		return 0;

	if (g_winapiApiTable->Thread32First(hSnap, &ti))
	{
		do {
			if (ti.th32OwnerProcessID == g_winapiApiTable->GetCurrentProcessId())
			{
				auto hThread = g_winapiApiTable->OpenThread(THREAD_QUERY_INFORMATION, false, ti.th32ThreadID);
				if (IS_VALID_HANDLE(hThread))
				{
					auto dwStartAddress = GetThreadStartAddress(hThread);
					if (dwStartAddress == pEntryPoint)
					{
						g_winapiApiTable->CloseHandle(hThread);
						g_winapiApiTable->CloseHandle(hSnap);

						return ti.th32ThreadID;
					}
					g_winapiApiTable->CloseHandle(hThread);
				}
			}
		} while (g_winapiApiTable->Thread32Next(hSnap, &ti));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return 0;
}


DWORD CThreadFunctions::GetThreadIdFromAddress(DWORD dwAddress)
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	THREADENTRY32 ti = { 0 };
	ti.dwSize = sizeof(ti);

	if (g_winapiApiTable->Thread32First(hSnap, &ti))
	{
		do {
			if (ti.th32OwnerProcessID == g_winapiApiTable->GetCurrentProcessId())
			{
				auto hThread = g_winapiApiTable->OpenThread(THREAD_QUERY_INFORMATION, false, ti.th32ThreadID);
				if (IS_VALID_HANDLE(hThread))
				{
					auto dwStartAddress = GetThreadStartAddress(hThread);
					if (dwStartAddress == dwAddress)
					{
						g_winapiApiTable->CloseHandle(hThread);
						g_winapiApiTable->CloseHandle(hSnap);

						return ti.th32ThreadID;
					}
					g_winapiApiTable->CloseHandle(hThread);
				}
			}
		} while (g_winapiApiTable->Thread32Next(hSnap, &ti));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return 0;
}

HANDLE CThreadFunctions::CreateThread(int iCustomThreadCode, LPTHREAD_START_ROUTINE pFunc, LPVOID lpParam, LPDWORD pdwThreadId)
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started! Thread code: %d Vista+ %d", iCustomThreadCode, IsWindowsVistaOrGreater());

	auto dwThreadId = DWORD_PTR(0);
	auto hThread = g_nmApp->DynamicWinapiInstance()->NTHelper()->CreateThread(pFunc, lpParam, &dwThreadId);
	DEBUG_LOG(LL_SYS, "Thread creation completed(%d) Result: %d - Thread: %p", iCustomThreadCode, g_nmApp->DynamicWinapiInstance()->IsValidHandle(hThread), hThread);

	if (pdwThreadId) *pdwThreadId = dwThreadId;
	return hThread;
}

DWORD CThreadFunctions::GetThreadID(HANDLE hThread)
{
	if (!IS_VALID_HANDLE(hThread))
	{
		DEBUG_LOG(LL_ERR, "Thread handle: %p is NOT valid!", hThread);
		return 0;
	}

	THREAD_BASIC_INFORMATION ThreadInfo;
	auto ntStat = g_winapiApiTable->NtQueryInformationThread(hThread, 0, &ThreadInfo, sizeof(ThreadInfo), NULL);
	if (!NT_SUCCESS(ntStat)) 
	{
		DEBUG_LOG(LL_ERR, "NtQueryInformationThread fail! Status: %p", ntStat);
		return 0;
	}

	return (DWORD)ThreadInfo.ClientId.UniqueThread;
}

bool CThreadFunctions::ChangeThreadsStatus(bool bSuspend)
{
	auto threadEnumerator = std::make_unique<CSafeThreadHandle>();
	if (!IS_VALID_SMART_PTR(threadEnumerator))
	{
		DEBUG_LOG(LL_ERR, "threadEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto vThreads = threadEnumerator->EnumerateThreads(NtCurrentProcess);
	if (vThreads.empty()) 
	{
		DEBUG_LOG(LL_ERR, "Thread list is null!");
		return false;
	}

	// Suspend/Resume routine
	for (const auto & hThread : vThreads)
	{
		if (g_winapiApiTable->GetThreadId(hThread) == g_winapiApiTable->GetCurrentThreadId())
			continue;

		if (bSuspend)
			g_winapiApiTable->SuspendThread(hThread);
		else
			g_winapiApiTable->ResumeThread(hThread);
	}

	return true;
}


