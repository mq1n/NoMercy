#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Watchdog.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "Timer.h"
#include "WindowEnumerator.h"
#include "ThreadFunctions.h"

LRESULT CALLBACK HookWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static auto i				= 0;
	static auto mBackups		= g_nmApp->WatchdogInstance()->GetWindowBackups();
	static auto pWatchdogTimer	= CTimer<std::chrono::milliseconds>();

	if (pWatchdogTimer.diff() > 5000)
	{
		if (i >= INT_MAX - 1)
			i = 0;

		DEBUG_LOG(LL_SYS, "Watchdog event called [%d] - Window: 0x%X Proc: 0x%X", i++, hWnd, mBackups[hWnd]);

		if (g_nmApp->SelfThreadMgrInstance()->IsTickCheckerThreadIntegrityCorrupted() == false)
		{
			DEBUG_LOG(LL_ERR, "Tick checker thread integrity check fail!");

			g_nmApp->OnCloseRequest(EXIT_ERR_TICK_CHECKER_THREAD_CORRUPTED, g_winapiApiTable->GetLastError());
			return 0;
		}

		DEBUG_LOG(LL_SYS, "Watchdog event succesfully completed [%d] - Window: 0x%X Proc: 0x%X", i, hWnd, mBackups[hWnd]);

		// Reset timer
		pWatchdogTimer.reset();

		DEBUG_LOG(LL_SYS, "Watchdog timer cleaned.");
	}

	return g_winapiApiTable->CallWindowProcA(mBackups[hWnd], hWnd, uMsg, wParam, lParam);
}


CWatchdog::CWatchdog() :
	m_bInitialized(false), m_iWatchDogCheckCount(0)
{
	m_windowBackupMap.clear();
}

bool CWatchdog::LoadWatchdog()
{
	// Check environment
	if (g_nmApp->DataInstance()->GetAppType() == NM_TESTAPP)
	{
		DEBUG_LOG(LL_SYS, "Watchdog creation has been passed on test console!");
		return true;
	}
	DEBUG_LOG(LL_SYS, "Watchdog creation has been started!");

	// Start timers
	m_watchdogTimer.reset();
	m_tickCheckTimer.reset();

	// Create window enumerator
	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator))
	{
		DEBUG_LOG(LL_ERR, "windowEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto vWindows = windowEnumerator->EnumerateWindows(g_winapiApiTable->GetCurrentProcessId());
	if (vWindows.empty())
	{
		DEBUG_LOG(LL_ERR, "Can not found any window for: %u", g_winapiApiTable->GetCurrentProcessId());
		return false;
	}

	// Enum windows
	bool bWatchdogCreatedToAnyWindow = false;
	for (auto hWnd : vWindows)
	{
		auto dwPID = 0UL;
		auto dwThreadId = g_winapiApiTable->GetWindowThreadProcessId(hWnd, &dwPID);
		if (dwThreadId == g_winapiApiTable->GetCurrentThreadId())
			continue;

		if (g_winapiApiTable->IsWindowVisible(hWnd) == FALSE)
			continue;

		char szTitle[MAX_PATH] = { 0x0 };
		g_winapiApiTable->GetWindowTextA(hWnd, szTitle, MAX_PATH);
#ifdef _DEBUG
		if (strstr(szTitle, "debug console"))
			continue;
#endif

		auto lptrResult = g_winapiApiTable->GetWindowLongA(hWnd, GWL_WNDPROC);
		if (!lptrResult)
		{
			DEBUG_LOG(LL_ERR, "GetWindowLongA fail! Last error: %u Target hwnd: %p", g_winapiApiTable->GetLastError(), hWnd);
			continue;
		}

		if (m_windowBackupMap.find(hWnd) != m_windowBackupMap.end() /* backup map is not empty */ &&
			m_windowBackupMap[hWnd] == HookWndProc /* current loop window's backup equal to our hooked wndproc */)
		{
			DEBUG_LOG(LL_SYS, "Watchdog is already created! Skipped on this window! Hwnd: %p", hWnd);
			continue;
		}

		// Create watchdog
		if (lptrResult != reinterpret_cast<LONG_PTR>(HookWndProc))
		{
			DEBUG_LOG(LL_SYS, "Non-protected window found! Watchdog creation has been started! Hwnd: %p", hWnd);

			auto lpOldProc = reinterpret_cast<WNDPROC>(g_winapiApiTable->SetWindowLongA(hWnd, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(HookWndProc)));
			m_windowBackupMap.insert({ hWnd, lpOldProc });

			DEBUG_LOG(LL_SYS, "Watchdog successfully created to: %p(%s)", hWnd, szTitle);
		}

		// Check new window result
		lptrResult = g_winapiApiTable->GetWindowLongA(hWnd, GWL_WNDPROC);
		if (!lptrResult) 
		{
			DEBUG_LOG(LL_ERR, "GetWindowLongA(Second attempt) fail! Last error: %u Target hwnd: %p", g_winapiApiTable->GetLastError(), hWnd);
			continue;
		}

		if (lptrResult != reinterpret_cast<LONG_PTR>(HookWndProc))
		{
			DEBUG_LOG(LL_ERR, "Watchdog can NOT created to: %p(%s) Last Error: %u", hWnd, szTitle, g_winapiApiTable->GetLastError());
			continue;
		}
		bWatchdogCreatedToAnyWindow = true;
	}

	DEBUG_LOG(LL_SYS, "Watchdog creation completed!");
	return bWatchdogCreatedToAnyWindow;
}

bool CWatchdog::PreCheckLoadedWatchdogs()
{
	// Check environment
	if (g_nmApp->DataInstance()->GetAppType() == NM_TESTAPP)
	{
		DEBUG_LOG(LL_SYS, "Watchdog creation has been passed on test console!");
		return true;
	}
	DEBUG_LOG(LL_SYS, "Watchdog creation has been started!");

	// Create window enumerator
	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator))
	{
		DEBUG_LOG(LL_ERR, "windowEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto vWindows = windowEnumerator->EnumerateWindows(g_winapiApiTable->GetCurrentProcessId());
	if (vWindows.empty())
	{
		DEBUG_LOG(LL_ERR, "Can not found any window for: %u", g_winapiApiTable->GetCurrentProcessId());
		return false;
	}

	// Enum windows
	bool bWatchdogCreatedToAnyWindow = false;
	for (const auto & hWnd : vWindows)
	{
		auto dwPID = 0UL;
		auto dwThreadId = g_winapiApiTable->GetWindowThreadProcessId(hWnd, &dwPID);
		if (dwThreadId == g_winapiApiTable->GetCurrentThreadId())
			continue;

		if (g_winapiApiTable->IsWindowVisible(hWnd) == FALSE)
			continue;

#ifdef _DEBUG
		char szTitle[MAX_PATH] = { 0x0 };
		g_winapiApiTable->GetWindowTextA(hWnd, szTitle, MAX_PATH);
		if (strstr(szTitle, "debug console"))
			continue;
#endif

		auto lptrResult = g_winapiApiTable->GetWindowLongA(hWnd, GWL_WNDPROC);
		if (!lptrResult)
		{
			DEBUG_LOG(LL_ERR, "GetWindowLongA fail! Last error: %u Target hwnd: %p", g_winapiApiTable->GetLastError(), hWnd);
			continue;
		}

		auto hThread = g_winapiApiTable->OpenThread(THREAD_QUERY_INFORMATION, FALSE, dwThreadId);
		if (!IS_VALID_HANDLE(hThread))
		{
			DEBUG_LOG(LL_ERR, "OpenThread fail! Last error: %u Target thread: %u", g_winapiApiTable->GetLastError(), dwThreadId);
			continue;
		}

		auto dwThreadStartAddr = CThreadFunctions::GetThreadStartAddress(hThread);
		if (!dwThreadStartAddr)
		{
			DEBUG_LOG(LL_ERR, "GetThreadStartAddress fail! Last error: %u", g_winapiApiTable->GetLastError());
			continue;
		}

		auto szThreadOwner = g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress(dwThreadStartAddr);
		auto szWndPtrOwner = g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD)lptrResult);

		DEBUG_LOG(LL_SYS, "Current thread: %u Current window: %p(%s) Thread: %p(%s) Window ptr: %p(%s)",
			dwThreadId, hWnd, szTitle, dwThreadStartAddr, szThreadOwner.c_str(), lptrResult, szWndPtrOwner.c_str());

		// TODO: Check and compare owners

		g_winapiApiTable->CloseHandle(hThread);
	}

	DEBUG_LOG(LL_SYS, "Watchdog check completed!");
	return true;
}

bool CWatchdog::CheckLoadedWatchdogs()
{
	// Check environment
	if (g_nmApp->DataInstance()->GetAppType() == NM_TESTAPP)
	{
		DEBUG_LOG(LL_SYS, "Watchdog creation has been passed on test console!");
		return true;
	}
	DEBUG_LOG(LL_SYS, "Watchdog creation has been started!");

	// Create window enumerator
	auto windowEnumerator = std::make_unique<CWindowEnumerator>();
	if (!IS_VALID_SMART_PTR(windowEnumerator))
	{
		DEBUG_LOG(LL_ERR, "windowEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto vWindows = windowEnumerator->EnumerateWindows(g_winapiApiTable->GetCurrentProcessId());
	if (vWindows.empty()) 
	{
		DEBUG_LOG(LL_ERR, "Can not found any window for: %u", g_winapiApiTable->GetCurrentProcessId());
		return false;
	}

	// Enum windows
	bool bWatchdogCreatedToAnyWindow = false;
	for (const auto & hWnd : vWindows)
	{
		auto dwPID		= 0UL;
		auto dwThreadId = g_winapiApiTable->GetWindowThreadProcessId(hWnd, &dwPID);
		if (dwThreadId == g_winapiApiTable->GetCurrentThreadId())
			continue;

		if (g_winapiApiTable->IsWindowVisible(hWnd) == FALSE)
			continue;

#ifdef _DEBUG
		char szTitle[MAX_PATH] = { 0x0 };
		g_winapiApiTable->GetWindowTextA(hWnd, szTitle, MAX_PATH);
		if (strstr(szTitle, "debug console"))
			continue;
#endif

		auto lptrResult = g_winapiApiTable->GetWindowLongA(hWnd, GWL_WNDPROC);
		if (!lptrResult) 
		{
			DEBUG_LOG(LL_ERR, "GetWindowLongA fail! Last error: %u Target hwnd: %p", g_winapiApiTable->GetLastError(), hWnd);
			continue;
		}

		if (m_windowBackupMap.find(hWnd) == m_windowBackupMap.end())
		{
			DEBUG_LOG(LL_ERR, "Current window backup not found on backup list. hwnd: %p", hWnd);

			g_nmApp->OnCloseRequest(EXIT_ERR_WATCHDOG_BACKUP_CHECK_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}

		if (m_windowBackupMap[hWnd] != HookWndProc /* current loop window's backup is NOT equal to our hooked wndproc */)
		{
			// Erase old result from map
			auto it = m_windowBackupMap.find(hWnd);
			m_windowBackupMap.erase(it);

			// Create watchdog
			auto lpOldProc = reinterpret_cast<WNDPROC>(g_winapiApiTable->SetWindowLongA(hWnd, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(HookWndProc)));
			if (!lpOldProc)
			{
				continue;
			}
			m_windowBackupMap.insert({ hWnd, lpOldProc });

			// Check new window result
			lptrResult = g_winapiApiTable->GetWindowLongA(hWnd, GWL_WNDPROC);
			if (!lptrResult || lptrResult != reinterpret_cast<LONG_PTR>(HookWndProc))
			{
				DEBUG_LOG(LL_ERR, "Watchdog mismatch detected! Hwnd: %p Hooked proc: %p Backup: %p", hWnd, HookWndProc, m_windowBackupMap[hWnd]);
				return false;
			}
		}
	}

	DEBUG_LOG(LL_SYS, "Watchdog check completed!");
	return true;
}

size_t CWatchdog::GetWatchdogCount()
{
	return m_windowBackupMap.size();
}

bool CWatchdog::IsWatchdogWindow(HWND hWnd)
{
	return m_windowBackupMap.find(hWnd) != m_windowBackupMap.end();
}

std::map <HWND, WNDPROC> CWatchdog::GetWindowBackups()
{
	return m_windowBackupMap;
}

DWORD WINAPI InitializeWatchdogEx(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Watchdog thread event has been started");

	auto pWatchdogThreadTimer = CTimer<std::chrono::milliseconds>();

	auto currentThread = std::shared_ptr<CThread>();
	do
	{
		if (pWatchdogThreadTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_WATCHDOG_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_WATCHDOG);

	} while (!IS_VALID_SMART_PTR(currentThread));

	int iAttemptCount = 0;

	while (1)
	{
		auto ulCount = g_nmApp->WatchdogInstance()->GetWatchdogCount();
		if (ulCount == 0)
		{
			if (g_nmApp->WatchdogInstance()->PreCheckLoadedWatchdogs() == false)
			{
				// ...
			}

			auto bRet = g_nmApp->WatchdogInstance()->LoadWatchdog();
			if (bRet == false)
			{
				if (iAttemptCount++ <= 3)
				{
					g_winapiApiTable->Sleep(2000);
					continue;
				}

//				g_nmApp->OnCloseRequest(EXIT_ERR_WATCHDOG_LOAD_FAIL, g_winapiApiTable->GetLastError());
				g_nmApp->WatchdogInstance()->SetInitilizationStatus(false);
				DEBUG_LOG(LL_ERR, "Watchdog load fail!");
				return 0;
			}
			else
			{
				g_nmApp->WatchdogInstance()->SetInitilizationStatus(true);
				DEBUG_LOG(LL_SYS, "Watchdog succesfully loaded to: %d window(s)", g_nmApp->WatchdogInstance()->GetWatchdogCount());
			}
		}
		else
		{
			auto bRet = g_nmApp->WatchdogInstance()->CheckLoadedWatchdogs();
			if (bRet == false)
			{
				g_nmApp->OnCloseRequest(EXIT_ERR_WATCHDOG_CHECK_FAIL, g_winapiApiTable->GetLastError());
				return 0;
			}

			// Update ticks
			g_nmApp->SelfThreadMgrInstance()->IncreaseThreadTick(SELF_THREAD_WATCHDOG);
			g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(SELF_THREAD_WATCHDOG, GetCurrentTime());

			g_winapiApiTable->Sleep(30000);
		}
	}

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc11()
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

bool CWatchdog::InitializeWatchdog()
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)InitializeWatchdogEx);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		DEBUG_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_WATCHDOG, InitializeWatchdogEx, ulFuncSize, nullptr, 30000 + 1000, false);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}
