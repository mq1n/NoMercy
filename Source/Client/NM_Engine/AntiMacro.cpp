#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "AntiMacro.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Defines.h"

// TODO: when bad action happens get owner process id and report master srv 

#define LLKHF_LOWER_IL_INJECTED 0x00000002
#define LLMHF_LOWER_IL_INJECTED 0x00000002

#pragma optimize("", off)
static HHOOK s_hkMouseHook		= 0;
static HHOOK s_hkKeyboardHook	= 0;

// Low level Mouse filter proc
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_RBUTTONDOWN || wParam == WM_LBUTTONDOWN)
		{
			auto pHookData = (MSLLHOOKSTRUCT*)lParam;

			// TODO: Activate screen protection
			if ((pHookData->flags & LLMHF_INJECTED) == LLMHF_INJECTED)
				return TRUE;
			
			if ((pHookData->flags & LLMHF_LOWER_IL_INJECTED) == LLMHF_LOWER_IL_INJECTED)
				return TRUE;	
		}
	}
	return g_winapiApiTable->CallNextHookEx(s_hkMouseHook, nCode, wParam, lParam);
}

// Low level Keyboard filter proc
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			auto pHookData = (KBDLLHOOKSTRUCT*)lParam;

			// TODO: Activate screen protection
			if ((pHookData->flags & LLKHF_INJECTED) == LLKHF_INJECTED)
				return TRUE;
			
			if ((pHookData->flags & LLKHF_LOWER_IL_INJECTED) == LLKHF_LOWER_IL_INJECTED)
				return TRUE;
		}
	}

	return g_winapiApiTable->CallNextHookEx(s_hkKeyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI AntiMacroEx(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Anti macro thread event has been started");

	auto pMacroThreadTimer = CTimer<std::chrono::milliseconds>();

	auto currentThread = std::make_shared<CThread>();
	do
	{
		if (pMacroThreadTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_ANTI_MACRO_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_ANTIMACRO);

	} while (!IS_VALID_SMART_PTR(currentThread));

	auto hInstance		= g_winapiModuleTable->hBaseModule;
	s_hkMouseHook		= g_winapiApiTable->SetWindowsHookExA(WH_MOUSE_LL,		MouseHookProc,		hInstance, NULL);
	s_hkKeyboardHook	= g_winapiApiTable->SetWindowsHookExA(WH_KEYBOARD_LL,	KeyboardHookProc,	hInstance, NULL);
	
	if (IsWindowsVistaOrGreater())
	{
		if (!s_hkMouseHook)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_MOUSE_MACRO_HOOK_INIT_FAIL, g_winapiApiTable->GetLastError());
			return 0;
		}

		if (!s_hkKeyboardHook)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_KEYBOARD_MACRO_HOOK_INIT_FAIL, g_winapiApiTable->GetLastError());
			return 0;
		}
	}

	if (s_hkMouseHook && s_hkKeyboardHook)
	{
		CTimer <std::chrono::milliseconds> pCheckTimer;

		MSG message;
		while (g_winapiApiTable->GetMessageA(&message, NULL, 0, 0))
		{
			g_winapiApiTable->TranslateMessage(&message);
			g_winapiApiTable->DispatchMessageA(&message);

			if (pCheckTimer.diff() > 5000)
			{
				pCheckTimer.reset();

				// Reload hooks
#ifndef _DEBUG
				UnhookWindowsHookEx(s_hkMouseHook);
				s_hkMouseHook		= SetWindowsHookExA(WH_MOUSE_LL,		MouseHookProc,		hInstance, NULL);

				UnhookWindowsHookEx(s_hkKeyboardHook);
				s_hkKeyboardHook	= SetWindowsHookExA(WH_KEYBOARD_LL,		KeyboardHookProc,	hInstance, NULL);
#endif
			}

//			// Update ticks
//			g_nmApp->SelfThreadMgrInstance()->IncreaseThreadTick(SELF_THREAD_ANTIMACRO);
//			g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(SELF_THREAD_ANTIMACRO, GetCurrentTime());
		}

		g_nmApp->OnCloseRequest(EXIT_ERR_ANTI_MACRO_MSG_FAIL, g_winapiApiTable->GetLastError());
		return 0;
	}

	g_winapiApiTable->UnhookWindowsHookEx(s_hkMouseHook);
	g_winapiApiTable->UnhookWindowsHookEx(s_hkKeyboardHook);

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc2()
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

bool CAntiMacro::InitAntiMacro()
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)AntiMacroEx);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		DEBUG_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_ANTIMACRO, AntiMacroEx, ulFuncSize, nullptr, 5000 + 1000, true /* false */);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}

bool CAntiMacro::DestroyAntiMacro()
{
	if (s_hkMouseHook) 
	{
		if (!g_winapiApiTable->UnhookWindowsHookEx(s_hkMouseHook))
			return false;

		s_hkMouseHook = nullptr;
	}
	if (s_hkKeyboardHook) 
	{
		if (!g_winapiApiTable->UnhookWindowsHookEx(s_hkKeyboardHook))
			return false;

		s_hkKeyboardHook = nullptr;
	}
	return true;
}
#pragma optimize("", on)

