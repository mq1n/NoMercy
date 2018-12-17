#include "NM_Index.h"
#include "NM_Main.h"
#include "Terminator.h"
#include "DynamicWinapi.h"
#include "ProcessFunctions.h"
#include "ThreadFunctions.h"
#include "WindowEnumerator.h"
#include "Defines.h"

inline bool TerminateProcessWithJobObject(HANDLE hProcess)
{
	auto hJobject = CreateJobObjectA(NULL, xorstr("NmcyJob").crypt_get());
	if (!IS_VALID_HANDLE(hJobject))
		return false;

	if (!AssignProcessToJobObject(hJobject, hProcess))
		return false;

	if (!TerminateJobObject(hJobject, EXIT_SUCCESS))
		return false;

	g_winapiApiTable->WaitForSingleObject(hProcess, 1000);
	return true;
}

inline bool TerminateProcessWithDebugObject(HANDLE hProcess)
{
	OBJECT_ATTRIBUTES oa;
	InitializeObjectAttributes(&oa, 0, 0, 0, 0);

	HANDLE hDebugObject;
	auto ntStat = g_winapiApiTable->NtCreateDebugObject(&hDebugObject, DEBUG_ALL_ACCESS, &oa, DEBUG_OBJECT_KILLONCLOSE);
	if (ntStat >= STATUS_SUCCESS)
	{
		ntStat = g_winapiApiTable->NtDebugActiveProcess(hProcess, hDebugObject);
		if (ntStat >= STATUS_SUCCESS)
		{
			g_winapiApiTable->CloseHandle(hDebugObject);
			return true;
		}
	}
	g_winapiApiTable->CloseHandle(hDebugObject);
	return false;
}

bool CTerminator::TerminateProcess(HANDLE hProcess)
{
//	auto dwProcessID = g_winapiApiTable->GetProcessId(hProcess);

//	// Check at the first
//	auto bProcessIsAlive = CProcessFunctions::ProcessIsItAlive(dwProcessID);
//	if (!bProcessIsAlive)
//		return true;

	// First method
	auto ntStat = g_winapiApiTable->NtTerminateProcess(hProcess, EXIT_SUCCESS);
	if (!NT_SUCCESS(ntStat))
		return false;

	return true;
#if 0
	auto dwProcessID = g_winapiApiTable->GetProcessId(hProcess);

	// Check 2.time
	auto bProcessIsAlive = CProcessFunctions::ProcessIsItAlive(dwProcessID);
	if (!bProcessIsAlive)
		return true;

	// Second method
	if (!TerminateProcessWithJobObject(hProcess))
		return false;

	// Check 3.time
	bProcessIsAlive = CProcessFunctions::ProcessIsItAlive(dwProcessID);
	if (!bProcessIsAlive)
		return true;

	// Third method
	g_winapiApiTable->WinStationTerminateProcess(NULL, dwProcessID, DBG_TERMINATE_PROCESS);

	// Check 4.time
	bProcessIsAlive = CProcessFunctions::ProcessIsItAlive(dwProcessID);
	if (!bProcessIsAlive)
		return true;

	// Last method
	if (!TerminateProcessWithDebugObject(hProcess))
		return false;

	// Check 5.time
	bProcessIsAlive = CProcessFunctions::ProcessIsItAlive(dwProcessID);
	if (!bProcessIsAlive)
		return true;

	DEBUG_LOG(LL_CRI, "Fatal error!!!!");
	g_winapiApiTable->LdrShutdownProcess();

#ifndef _M_X64
	__asm
	{
		mov eax, 0
		jmp eax
	}
#endif

	DEBUG_LOG(LL_CRI, "[0]");
	g_nmApp->FunctionsInstance()->InvokeBSOD();

	return bProcessIsAlive;
#endif
}

bool CTerminator::TerminateThread(HANDLE hThread)
{
	auto dwThreadId = g_winapiApiTable->GetThreadId(hThread);

	// Check at the first
	auto bThreadIsAlive = CThreadFunctions::ThreadIsItAlive(dwThreadId);
	if (!bThreadIsAlive)
		return true;

	// First method
	auto ntStat = g_winapiApiTable->TerminateThread(hThread, EXIT_SUCCESS);
	if (!NT_SUCCESS(ntStat))
		return false;

	// Check 2.time
	bThreadIsAlive = CThreadFunctions::ThreadIsItAlive(dwThreadId);
	if (!bThreadIsAlive)
		return true;

	if (!PostThreadMessageA(dwThreadId, WM_QUIT, 0, 0))
		return false;

	// Check 3.time
	bThreadIsAlive = CThreadFunctions::ThreadIsItAlive(dwThreadId);
	if (!bThreadIsAlive)
		return true;

	return bThreadIsAlive;
}

bool CTerminator::TerminateWindow(HWND hWnd)
{
	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator))
		return false;

	auto vWindows = windowEnumerator->EnumerateWindows();
	if (vWindows.empty())
		return false;

	// Check at the first
	if (!HasInVector(vWindows, hWnd))
		return false;

	// First method
	g_winapiApiTable->SendMessageA(hWnd, WM_CLOSE, 0, 0);
	g_winapiApiTable->SendMessageA(hWnd, WM_QUIT, 0, 0);
	g_winapiApiTable->SendMessageA(hWnd, WM_DESTROY, 0, 0);

	// Check 2.time
	vWindows.clear();
	vWindows = windowEnumerator->EnumerateWindows();
	if (!HasInVector(vWindows, hWnd))
		return true;

	// Second method
	PostMessageA(hWnd, WM_CLOSE, 0, 0);
	PostMessageA(hWnd, WM_QUIT, 0, 0);
	PostMessageA(hWnd, WM_DESTROY, 0, 0);

	// Check 3.time
	vWindows.clear();
	vWindows = windowEnumerator->EnumerateWindows();
	if (!HasInVector(vWindows, hWnd))
		return true;

	// Last method
	if (!DestroyWindow(hWnd))
		return false;

	// Last check
	vWindows.clear();
	vWindows = windowEnumerator->EnumerateWindows();
	if (!HasInVector(vWindows, hWnd))
		return true;

	return false;
}
