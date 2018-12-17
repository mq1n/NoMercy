#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "NM_SelfThreadManager.h"
#include "ExceptionHandlers.h"

static auto gs_dwTimeoutDelay = 30000UL;

CSelfThreadManager::CSelfThreadManager()
{
	m_threadTimeMap.clear();
	m_threadTicksMap.clear();
}

DWORD CSelfThreadManager::GetLastCheckTime(DWORD dwThreadCode)
{
	if (m_threadTimeMap.find(dwThreadCode) == m_threadTimeMap.end())
	{
		DEBUG_LOG(LL_ERR, "Unknown thread ID: %u", dwThreadCode);
		return -1;
	}
	return m_threadTimeMap[dwThreadCode];
};
void CSelfThreadManager::SetLastCheckTime(DWORD dwThreadCode, DWORD dwTime)
{
	m_threadTimeMap[dwThreadCode] = dwTime;
};

void CSelfThreadManager::IncreaseThreadTick(DWORD dwThreadCode)
{
	if (m_threadTicksMap.find(dwThreadCode) == m_threadTicksMap.end())
	{
		m_threadTicksMap[dwThreadCode] = 1;
		return;
	}
	m_threadTicksMap[dwThreadCode] += 1;
}
void CSelfThreadManager::DecreaseThreadTick(DWORD dwThreadCode)
{
	if (m_threadTicksMap.find(dwThreadCode) == m_threadTicksMap.end())
	{
		DEBUG_LOG(LL_ERR, "Unknown thread ID: %u", dwThreadCode);
		return;
	}
	m_threadTicksMap[dwThreadCode] -= 1;
}
void CSelfThreadManager::ReleaseThreadTicks(DWORD dwThreadCode)
{
	if (m_threadTicksMap.find(dwThreadCode) == m_threadTicksMap.end())
	{
//		DEBUG_LOG(LL_ERR, "Unknown thread ID: %u", dwThreadCode);
		return;
	}
	m_threadTicksMap[dwThreadCode] = 0;
}
DWORD CSelfThreadManager::GetThreadTick(DWORD dwThreadCode)
{
	if (m_threadTicksMap.find(dwThreadCode) == m_threadTicksMap.end())
	{
		DEBUG_LOG(LL_ERR, "Unknown thread ID: %u", dwThreadCode);
		return -1;
	}
	return m_threadTicksMap[dwThreadCode];
}


DWORD WINAPI InitExThreadTickCheck(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Thread tick checker thread event has been started");

	auto pTickThreadTimer = CTimer<std::chrono::milliseconds>();

	auto currentThread = std::shared_ptr<CThread>();
	do
	{
		if (pTickThreadTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_TICK_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_THREADTICKCHECKER);

	} while (!IS_VALID_SMART_PTR(currentThread));


	while (1)
	{
		DEBUG_LOG(LL_SYS, "Thread tick check processing!");

		auto vThreadList = g_nmApp->ThreadManagerInstance()->GetThreadList();
		if (vThreadList.size() > 0)
		{
			for (const auto & pThread : vThreadList)
			{
				if (IS_VALID_SMART_PTR(pThread))
				{
					auto bTemporary = pThread->bIsTemporaryThread;
					if (bTemporary == false)
					{
						auto iThreadCode		= pThread->iThreadCode;
						auto dwMaxDelay			= pThread->dwMaxDelay + gs_dwTimeoutDelay;
						auto dwThreadTickCount	= g_nmApp->SelfThreadMgrInstance()->GetThreadTick(iThreadCode);
						auto dwLastCheckTime	= g_nmApp->SelfThreadMgrInstance()->GetLastCheckTime(iThreadCode);
						auto dwCurrentTime		= GetCurrentTime();

						DEBUG_LOG(LL_SYS, "Thread code: %d Temporary thread: %d Bind'd tick count is: %u Max Delay: %u Last check: %u Current: %u Dif: %u",
							iThreadCode, bTemporary, dwThreadTickCount, dwMaxDelay, dwLastCheckTime, dwCurrentTime, dwCurrentTime - dwLastCheckTime);

						if (!dwLastCheckTime)
						{
							g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(iThreadCode, GetCurrentTime());
							continue;
						}

						if ((dwCurrentTime - dwLastCheckTime) >= dwMaxDelay)
						{
							if (dwThreadTickCount == 0)
							{
								if (iThreadCode == SELF_THREAD_WATCHDOG && g_nmApp->WatchdogInstance()->GetInitilizationStatus() == false)
								{
									continue;
								}
								DEBUG_LOG(LL_ERR, "Null tick count on Thread: %d", iThreadCode);

								g_nmApp->OnCloseRequest(EXIT_ERR_THREAD_COMMUNICATION_FAIL, g_winapiApiTable->GetLastError());
								return 0;
							}
						}

						g_nmApp->SelfThreadMgrInstance()->ReleaseThreadTicks(iThreadCode);
					}
				}
			}
		}

		g_nmApp->SelfThreadMgrInstance()->IncreaseThreadTick(SELF_THREAD_THREADTICKCHECKER);
		g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(SELF_THREAD_THREADTICKCHECKER, GetCurrentTime());

		g_winapiApiTable->Sleep(10000);
	}

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc10()
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

bool CSelfThreadManager::InitThreadTickChecker()
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)InitExThreadTickCheck);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		DEBUG_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_THREADTICKCHECKER, InitExThreadTickCheck, ulFuncSize, nullptr, 60000, false);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}

bool CSelfThreadManager::IsTickCheckerThreadIntegrityCorrupted()
{
	auto pThread = g_nmApp->ThreadManagerInstance()->GetThreadInfo(SELF_THREAD_THREADTICKCHECKER);
	if (!IS_VALID_SMART_PTR(pThread) || !IS_VALID_SMART_PTR(pThread->customThread) || pThread->customThread->IsCorrect() == false)
		return false;

	auto dwMaxDelay			= pThread->dwMaxDelay + gs_dwTimeoutDelay;

	auto dwThreadTickCount	= g_nmApp->SelfThreadMgrInstance()->GetThreadTick(SELF_THREAD_THREADTICKCHECKER);
	auto dwLastCheckTime	= g_nmApp->SelfThreadMgrInstance()->GetLastCheckTime(SELF_THREAD_THREADTICKCHECKER);

	auto dwCurrentTime		= g_winapiApiTable->GetTickCount();

	DEBUG_LOG(LL_SYS, "Tick checker thread integrity check started! Target thread: %u(%p) Max delay: %u Last check: %u Current: %u Dif: %u Tick: %u",
		pThread->customThread->GetId(), pThread->customThread->GetHandle(), dwMaxDelay, dwLastCheckTime, dwCurrentTime, dwCurrentTime - dwLastCheckTime, dwThreadTickCount);

	if ((dwCurrentTime - dwLastCheckTime) >= dwMaxDelay)
	{
		if (dwThreadTickCount == 0)
		{
			DEBUG_LOG(LL_CRI, "Null tick count on tick checker thread !!!");
			return true;
		}
	}

	ReleaseThreadTicks(SELF_THREAD_THREADTICKCHECKER);
	return false;
}

/// 
inline bool CheckThreadEx(HANDLE hThread, LPDWORD pdwErrorCode)
{
	if (!IS_VALID_HANDLE(hThread)) 
	{
		if (pdwErrorCode) *pdwErrorCode = 1;
		return false;
	}

	if (!g_nmApp->DynamicWinapiInstance()->IsValidHandle(hThread))
	{
		if (pdwErrorCode) *pdwErrorCode = 2;
		return false;
	}

	auto pThread = g_nmApp->ThreadManagerInstance()->GetThreadFromHandle(hThread);
	if (!IS_VALID_SMART_PTR(pThread) || !pThread->IsCorrect())
	{
		if (pdwErrorCode) *pdwErrorCode = 3;
		return false;
	}

	if (pThread->IsItAlive() == false)
	{
		if (pdwErrorCode) *pdwErrorCode = 4;
		return false;
	}

	if (pThread->HasSuspend()) 
	{
		if (pdwErrorCode) *pdwErrorCode = 5;
		return false;
	}

	if (pThread->IsGoodPriority() == false) 
	{
		if (pdwErrorCode) *pdwErrorCode = 6;
		return false;
	}

	return true;
}

bool CSelfThreadManager::CheckThread(HANDLE hThread, LPDWORD pdwErrorCode)
{
	__try
	{
		if (!CheckThreadEx(hThread, pdwErrorCode))
			return false;
	}
	__except (CExceptionHandlers::OnExceptionThrowed(GetExceptionInformation()))
	{
//		Logf(CUSTOM_LOG_FILENAME, xorstr("Check thread fail!\n").crypt_get());
	}

	return true;
}

void CSelfThreadManager::CheckSelfThreads()
{	
	DEBUG_LOG(LL_SYS, "Self thread check started!");

	auto vThreadList = g_nmApp->ThreadManagerInstance()->GetThreadList();
	if (vThreadList.size() > 0)
	{
		for (const auto & pThread : vThreadList)
		{
			if (IS_VALID_SMART_PTR(pThread) && !pThread->bIsTemporaryThread && IS_VALID_SMART_PTR(pThread->customThread) && pThread->customThread->IsCorrect())
			{
				auto dwErrorCode = 0UL;
				if (CheckThread(pThread->hThread, &dwErrorCode) == false)
				{
					DEBUG_LOG(LL_ERR, "Thread integrity failed! Thread: %d Error: %u", pThread->iThreadCode, dwErrorCode);
					g_nmApp->OnCloseRequest(EXIT_ERR_THREAD_INTEGRITY_FAIL, g_winapiApiTable->GetLastError());
					return;
				}
			}
		}
	}

	DEBUG_LOG(LL_SYS, "Self thread check completed!");
}

