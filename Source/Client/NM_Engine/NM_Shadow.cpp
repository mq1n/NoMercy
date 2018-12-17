#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Functions.h"
#include "Defines.h"
#include "ProcessFunctions.h"
#include "SafeThreadHandle.h"
#include "SafeProcessHandle.h"
#include "ThreadEnumerator.h"
#include "Terminator.h"
#include "ThreadExitCallback.h"
#include "StackTrace.h"
#include "ExceptionHandlers.h"
#include <Shlobj.h>

static auto		s_pExitCallbackHelper	= std::make_shared<CThreadExitWatcher>();
static auto		s_dwTargetPID			= 0UL;
static auto		s_dwShadowPID			= 0UL;
static auto		s_bExitTriggered		= false;
static auto		s_hProcess				= HANDLE(INVALID_HANDLE_VALUE);
static auto		s_hShadowThread			= HANDLE(INVALID_HANDLE_VALUE);

void NTAPI GameExitCallback(PVOID pParam, BOOLEAN)
{
	s_bExitTriggered = true;
	DEBUG_LOG(LL_CRI, "Protected process terminated! Alive ret: %d", CProcessFunctions::ProcessIsItAlive(s_dwTargetPID));
}

void NTAPI ShadowExitCallback(PVOID pParam, BOOLEAN)
{
	DEBUG_LOG(LL_CRI, "Watchdog process terminated! Alive ret: %d", CProcessFunctions::ProcessIsItAlive(s_dwShadowPID));
	g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_PROCESS_TERMINATED, g_winapiApiTable->GetLastError());
}

inline bool CreateTargetProcessHandle()
{
	auto processEnumerator = std::make_unique<CSafeProcessHandle>();
	if (!IS_VALID_SMART_PTR(processEnumerator)) 
	{
		DEBUG_LOG(LL_ERR, "processEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	s_hProcess = g_winapiApiTable->OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, s_dwTargetPID);
	if (!IS_VALID_HANDLE(s_hProcess))
	{
		auto bIsAlive = CProcessFunctions::ProcessIsItAlive(s_dwTargetPID);
		DEBUG_LOG(LL_ERR, "Process can not open! Last error: %u IsAlive: %d", g_winapiApiTable->GetLastError(), bIsAlive);
		return false;
	}
	DEBUG_LOG(LL_ERR, "Handle created for protected process: %p", s_hProcess);

	auto bProtectRet = g_winapiApiTable->SetHandleInformation(s_hProcess, HANDLE_FLAG_PROTECT_FROM_CLOSE, HANDLE_FLAG_PROTECT_FROM_CLOSE);
	if (!bProtectRet)
	{
		DEBUG_LOG(LL_ERR, "Process handle can not protected! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	if (!IS_VALID_SMART_PTR(s_pExitCallbackHelper)) 
	{
		DEBUG_LOG(LL_ERR, "Process watcher class can not allocated! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	if (!s_pExitCallbackHelper->InitializeExitCallback(s_hProcess, GameExitCallback)) 
	{
		DEBUG_LOG(LL_ERR, "Process watcher can not initialized! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	return true;
}

inline bool HasSuspendedThread()
{
	auto threadEnumerator = std::make_shared<CThreadEnumerator>(s_dwTargetPID);
	if (!IS_VALID_SMART_PTR(threadEnumerator))
	{
		DEBUG_LOG(LL_ERR, "threadEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return true;
	}

	auto systemThreadOwnerProcInfo = (SYSTEM_PROCESS_INFORMATION*)threadEnumerator->GetProcInfo();
	if (systemThreadOwnerProcInfo == nullptr) 
	{
		DEBUG_LOG(LL_ERR, "systemThreadOwnerProcInfo is null! Last error: %u", g_winapiApiTable->GetLastError());
		return true;
	}

	auto dwThreadCount = threadEnumerator->GetThreadCount(systemThreadOwnerProcInfo);
	if (!dwThreadCount)
	{
		DEBUG_LOG(LL_ERR, "dwThreadCount is null! Last error: %u", g_winapiApiTable->GetLastError());
		return true;
	}

	auto pk_Thread = (SYSTEM_THREAD_INFORMATION*)threadEnumerator->GetThreadList(systemThreadOwnerProcInfo);
	if (pk_Thread == nullptr) 
	{
		DEBUG_LOG(LL_ERR, "pk_Thread is null! Last error: %u", g_winapiApiTable->GetLastError());
		return true;
	}

	for (std::size_t i = 0; i < dwThreadCount; i++)
	{
		auto dwThreadId = reinterpret_cast<DWORD_PTR>(pk_Thread->ClientId.UniqueThread);
		DEBUG_LOG(LL_SYS, "Checking, Loop idx: %u Thread: %u State: %u Wait Reason: %u", i, dwThreadId, pk_Thread->ThreadState, pk_Thread->WaitReason);

		if (pk_Thread->ThreadState == Waiting && pk_Thread->WaitReason == Suspended) 
		{
			DEBUG_LOG(LL_ERR, "Suspended thread found! Thread ID: %u", dwThreadId);
			return true;
		}

		pk_Thread++;
	}

	return false;
}

inline void InitAccessRules()
{
	return; //TODO

	g_nmApp->AccessHelperInstance()->BlockAccess(NtCurrentProcess);
	g_nmApp->AccessHelperInstance()->DenyProcessAccess();
	g_nmApp->AccessHelperInstance()->SetMitigationPolicys();
}

void CFunctions::InitShadow(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	s_dwTargetPID		= std::atol(lpszCmdLine);
	auto ullCheckCount	= 0ULL;

	BOOLEAN boAdjustPrivRet;
	auto ntStatus = g_winapiApiTable->RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, TRUE, FALSE, &boAdjustPrivRet);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_SYS, "Shadow process access rules adjustion fail! Error: %p", ntStatus);
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_ADJUST_PRIV_FAIL, ntStatus);
		return;
	}
	DEBUG_LOG(LL_SYS, "RtlAdjustPrivilege completed! Status: %p Ret: %d", ntStatus, boAdjustPrivRet);

	// TODO: Create pipe to service
	// TODO: Create handle to driver

	if (CreateTargetProcessHandle() == false) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_HANDLE_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return;
	}

	InitAccessRules();

	while (1)
	{
		if (s_bExitTriggered)
			return;

		if (CProcessFunctions::ProcessIsItAlive(s_dwTargetPID) == false) 
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_PROTECTED_PROCESS_NOT_ALIVE, g_winapiApiTable->GetLastError());
			return;
		}

		// Check service is it alive
		// Check game is it suspened
		// Check service is it suspened
		// Check service status
		// Try contact with service with another ipc method

		DEBUG_LOG(LL_SYS, "Check: %lld completed!", ullCheckCount++);
		g_winapiApiTable->Sleep(5000);
	}
}

bool CFunctions::RunShadow(DWORD dwProcessId)
{
	// Get anticheat module informations
	auto pselfInfo = g_nmApp->DataInstance()->GetAntiModuleInformations();
	if (!pselfInfo) 
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_NULL_MODULE_INFO, g_winapiApiTable->GetLastError());
		return false;
	}
	auto selfInfo = (ANTI_MODULE_INFO*)pselfInfo;

	// Build watchdog process argument
	char szText[256];
	sprintf(szText, xorstr("/c rundll32.exe \"%ls\",InitializeShadow %u").crypt_get(), selfInfo->FullDllName.Buffer, dwProcessId);
	DEBUG_LOG(LL_SYS, "Argument: %s", szText);

	// Build cmd path
	char szCmdPath[MAX_PATH * 2];
	if (!SHGetSpecialFolderPathA(NULL, szCmdPath, CSIDL_SYSTEM, FALSE))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_SYS_PATH_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	strcat(szCmdPath, xorstr("\\cmd.exe").crypt_get());
	DEBUG_LOG(LL_SYS, szCmdPath);

	// Create watchdog process
	PROCESS_INFORMATION pi	= { 0 };
	STARTUPINFOA si			= { 0 };
	si.cb					= sizeof(si);
	si.wShowWindow			= SW_HIDE;
	si.dwFlags				= STARTF_USESHOWWINDOW;

	auto szPath = g_nmApp->DirFunctionsInstance()->ExePath();
	if (!g_winapiApiTable->CreateProcessA(szCmdPath, szText, NULL, NULL, FALSE, NULL, NULL, szPath.c_str(), &si, &pi))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "New process created! PID: %u Handle: %p", pi.dwProcessId, pi.hProcess);

	g_nmApp->DataInstance()->SetShadowProcessHandle(pi.hProcess);
	s_hShadowThread = pi.hThread;

	return true;
}

bool CFunctions::KillShadow()
{
	auto hShadowProc = g_nmApp->DataInstance()->GetShadowProcessHandle();
	if (!IS_VALID_HANDLE(hShadowProc) || !g_nmApp->DynamicWinapiInstance()->IsValidHandle(hShadowProc))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_HANDLE_NOT_VALID, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Shadow handle: %p", hShadowProc);

	auto dwShadowPID = g_winapiApiTable->GetProcessId(hShadowProc);
	if (!dwShadowPID)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_PID_NOT_EXIST, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Shadow pid: %u", dwShadowPID);

	if (!CTerminator::TerminateProcess(hShadowProc))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_TERMINATE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Shadow kill completed!");

	auto bIsRunning = CProcessFunctions::ProcessIsItAlive(dwShadowPID);
	if (bIsRunning)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_STILL_WORKS, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Shadow run check: %d", bIsRunning);

	return true;
}

bool CFunctions::CreateShadow(DWORD dwProtectedPID)
{
	if (g_nmApp->DataInstance()->GetAppType() != NM_TESTAPP)
	{
		if (!g_nmApp->FunctionsInstance()->RunShadow(dwProtectedPID))
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_RUN_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}
		DEBUG_LOG(LL_SYS, "Shadow run completed!");
	}

	auto hShadowProc = g_nmApp->DataInstance()->GetShadowProcessHandle();
	if (!IS_VALID_HANDLE(hShadowProc) || !g_nmApp->DynamicWinapiInstance()->IsValidHandle(hShadowProc))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_SHADOW_NOT_FOUND, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Shadow handle: %p", hShadowProc);

	s_dwShadowPID = g_winapiApiTable->GetProcessId(hShadowProc);
	if (!s_dwShadowPID)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_PID_NOT_EXIST_2, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Shadow pid: %u", s_dwShadowPID);

	auto bIsRunning = CProcessFunctions::ProcessIsItAlive(s_dwShadowPID);
	if (!bIsRunning)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_NOT_WORKS, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Shadow run check: %d", bIsRunning);

	if (!IS_VALID_SMART_PTR(s_pExitCallbackHelper))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_EXIT_CALLBACK_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (!s_pExitCallbackHelper->InitializeExitCallback(s_hShadowThread, ShadowExitCallback))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_SHADOW_EXIT_CALLBACK_INIT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	return true;
}

DWORD CFunctions::FindOwnShadowProcessId()
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (g_winapiApiTable->Process32FirstW(hSnap, &pe))
	{
		do 
		{			
			if (pe.th32ParentProcessID == g_winapiApiTable->GetCurrentProcessId())
			{
				auto szLowerProcName	= g_nmApp->FunctionsInstance()->wszLower(pe.szExeFile);
				std::wstring szRundll	= xorstr(L"rundll").crypt_get();

				if (wcsstr(szLowerProcName.c_str(), szRundll.c_str()))
					return pe.th32ProcessID;
			}

		} while (g_winapiApiTable->Process32NextW(hSnap, &pe));
	}

	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hSnap);
	return 0;
}

