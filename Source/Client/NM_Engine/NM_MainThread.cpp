#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "WinInetAPI.h"
#include "AntiEmulation.h"
#include "AntiDebug.h"
#include "StackTrace.h"
#include "SafeThreadHandle.h"
#include "ExceptionHandlers.h"
#include "SelfProtection.h"
#include "AntiBreakpoint.h"
#include "Watchdog.h"

void ClientMainRoutine()
{
	static auto s_bFirstCheck = true;
	static auto s_iIdx = 0;

	if (s_iIdx >= INT_MAX - 1)
		s_iIdx = 1;

	DEBUG_LOG(LL_SYS, "Main routine works! Idx: %d", s_iIdx++);

	// TODO: Ping to service
	// TODO: Ping to driver
	// TODO: Ping to own shadow
	// TODO: Ping to NoMercy game server
	// TODO: Implement OnCloseRequest funcs


	// Try enable debug priv
	if (g_nmApp->AccessHelperInstance()->EnableDebugPrivileges() == false)
	{
		DEBUG_LOG(LL_ERR, "Debug priv can NOT assigned!");
		//		return;
	}

	// Just run first time
	if (s_bFirstCheck)
	{
		// Run scanner's first time routines
		if (g_nmApp->ScannerInstance()->RunFirstTimeScans() == false)
		{
			DEBUG_LOG(LL_ERR, "Unallowed scan object found on first time scans!");
			//			return;
		}
	}

	// Runtime anti debugging
	auto dwDebugDetectType = 0UL;
	if (CAntiDebug::CheckRuntimeAntiDebug(&dwDebugDetectType))
	{
		DEBUG_LOG(LL_ERR, "Runtime debugging detected! Error step: %u", dwDebugDetectType);
		//		return;
	}

	// WoW64 specific checks
	if (g_nmApp->FunctionsInstance()->IsWow64Process(NtCurrentProcess))
	{
		// Check wow32reserved hook
		g_nmApp->ScannerInstance()->CheckWow32ReservedHook();

		// Check wow64 callgate redirection,
		auto pCurrTeb = GetCurrentTeb();
		if (pCurrTeb && pCurrTeb->WOW32Reserved)
		{
			// Disables file system redirection for the calling thread.
			PVOID OldValue = NULL;
			if (g_nmApp->DirFunctionsInstance()->ManageFsRedirection(true, nullptr, &OldValue))
			{
				char szSystemDirectory[MAX_PATH] = { 0x0 };
				GetSystemDirectoryA(szSystemDirectory, MAX_PATH);

				auto szWow64Cpu = g_nmApp->FunctionsInstance()->szLower(szSystemDirectory) + xorstr("\\wow64cpu.dll").crypt_get();
				if (g_nmApp->DirFunctionsInstance()->IsFileExist(szWow64Cpu))
				{
					auto szWow32Owner = g_nmApp->FunctionsInstance()->GetModuleOwnerName(pCurrTeb->WOW32Reserved);

					if (szWow64Cpu != szWow32Owner)
					{
						DEBUG_LOG(LL_ERR, "Wow32reserved hook detected! TID: %u Callback owner: %s", g_winapiApiTable->GetCurrentThreadId(), szWow32Owner.c_str());
//						return;
					}
				}

				// Restore file system redirection for the calling thread.
				g_nmApp->DirFunctionsInstance()->ManageFsRedirection(false, OldValue, nullptr);
			}
		}
	}

	// Check scanner's local DB
	if (g_nmApp->ScannerInstance()->CheckLocalDB() == false)
	{
		DEBUG_LOG(LL_ERR, "Unallowed scan object found on local DB!");
//		return;
	}

	// Runtime anti dump
	if (CSelfProtection::IsDumpTriggered())
	{
		DEBUG_LOG(LL_ERR, "Memory dump detected");
//		return;
	}

	// Runtime anti breakpoint
	if (CAntiBreakpoint::HasHardwareBreakpoint())
	{
		DEBUG_LOG(LL_ERR, "Hardware breakpoint detected");
//		return;
	}

	// If first check, disable it for next check
	if (s_bFirstCheck)
		s_bFirstCheck = false;

	// Update ticks
	g_nmApp->SelfThreadMgrInstance()->IncreaseThreadTick(SELF_THREAD_NMMAINROUTINE);
	g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(SELF_THREAD_NMMAINROUTINE, GetCurrentTime());
}

DWORD WINAPI NoMercyClientMainRoutine(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Main routine thread event has been started!");

	auto pMainRoutineTimer = CTimer<std::chrono::milliseconds>();

	auto currentThread = std::shared_ptr<CThread>();
	do
	{
		if (pMainRoutineTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_MAIN_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_NMMAINROUTINE);

	} while (!IS_VALID_SMART_PTR(currentThread));

	// Routine
	while (true)
	{
		ClientMainRoutine();

		auto dwRandNumber = g_nmApp->FunctionsInstance()->GetRandomInt(7000, 15000);
		g_winapiApiTable->Sleep(dwRandNumber);
	}

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc9()
{
	__asm
	{
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
	}
}
#endif

bool CNM_App::InitializeMainCheckThread()
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)NoMercyClientMainRoutine);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		DEBUG_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_NMMAINROUTINE, NoMercyClientMainRoutine, ulFuncSize, nullptr, 15000 + 1000, true);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}
