#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "WindowCallback.h"
#include "Defines.h"

static HWINEVENTHOOK gs_hWndHandlerHook = 0;

void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD dwEventCode, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	if (hwnd)
	{
		switch (dwEventCode)
		{
			case EVENT_OBJECT_REORDER:		// window create
			case EVENT_SYSTEM_MINIMIZEEND:	// window maximize
			{
				g_nmApp->ScannerInstance()->OnWatcherWindowScan(hwnd);
			} break;
		}
	}
}

DWORD WINAPI WindowCheckRoutine(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Window watcher thread event has been started");

	auto pWindowCheckTimer	= CTimer<std::chrono::milliseconds>();

	auto currentThread		= std::shared_ptr<CThread>();
	do
	{
		if (pWindowCheckTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_WINDOW_CHECK_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_WINDOWCHECK);

	} while (!IS_VALID_SMART_PTR(currentThread));


	gs_hWndHandlerHook = g_winapiApiTable->SetWinEventHook(EVENT_MIN, EVENT_MAX, NULL, HandleWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
	if (!gs_hWndHandlerHook)
	{
		DEBUG_LOG(LL_ERR, "SetWinEventHook fail! Error: %u", g_winapiApiTable->GetLastError());
		g_nmApp->OnCloseRequest(EXIT_ERR_WinEventHook_FAIL, g_winapiApiTable->GetLastError());
		return 0;
	}

	auto pCheckTimer = CTimer<std::chrono::milliseconds>();
	
	MSG message;
	while (g_winapiApiTable->GetMessageA(&message, NULL, 0, 0))
	{
		g_winapiApiTable->TranslateMessage(&message);
		g_winapiApiTable->DispatchMessageA(&message);

		if (pCheckTimer.diff() > 5000)
		{
			// increase tick count
			pCheckTimer.reset();
		}
	}

	DEBUG_LOG(LL_ERR, "Window watcher timeout!");
	g_nmApp->OnCloseRequest(EXIT_ERR_WinEventHook_TIMEOUT, g_winapiApiTable->GetLastError());

	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc12()
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

bool CWindowCallback::InitWindowCheck()
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)WindowCheckRoutine);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		DEBUG_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_WINDOWCHECK, WindowCheckRoutine, ulFuncSize, nullptr, 5000 + 1000, false);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}

bool CWindowCallback::DestroyWindowCheck()
{
	if (gs_hWndHandlerHook && !g_winapiApiTable->UnhookWinEvent(gs_hWndHandlerHook))
		return false;

	return true;
}

