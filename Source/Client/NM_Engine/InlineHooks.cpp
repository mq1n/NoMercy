#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Hooks.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "LDasm.h"
#include "StackTrace.h"
#include "ThreadFunctions.h"
#include "PEHelper.h"
#include "ExceptionHandlers.h"
#include "Locks.h"

#include <Deviare-InProc/Include/NktHookLib.h>

void NTAPI LdrInitializeThunkDetour(PCONTEXT NormalContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
//	HOOK_LOG(LL_CRI, "LdrInitializeThunk triggered! Ctx: %p Arg: %p", NormalContext, SystemArgument1);
	printf("LdrInitializeThunk triggered! Ctx: %p Arg: %p", NormalContext, SystemArgument1);

#if 0
	//test
	void * _ecx = NULL;
	void * _edx = NULL;

	_asm 
	{
		mov _ecx, ecx;
		mov _edx, edx;
	}

#ifdef _M_IX86
	if (((DWORD)_edx) >= 0x70000000)
	{
		HOOK_LOG(LL_CRI, "test1");
	}
#endif

	if (!IsBadWritePtr(_edx, 1))
	{
		HOOK_LOG(LL_CRI, "test2");
	}

	__asm 
	{
		mov  ecx, _ecx;
		mov  edx, _edx;
	}
#endif

#if 0
	auto pThreadHookData = std::make_shared<SThreadHookQueueData>();
	if (IS_VALID_SMART_PTR(pThreadHookData))
	{
		pThreadHookData->dwThreadID		= g_winapiApiTable->GetCurrentThreadId();
		pThreadHookData->lpStartAddress = NormalContext ? (LPVOID)NormalContext->Eax : nullptr;
		pThreadHookData->lpEIP			= NormalContext ? (LPVOID)NormalContext->Eip : nullptr;

		g_nmApp->SelfHooksInstance()->ThreadHookCallback(pThreadHookData);
	}
#endif

	return hLdrInitializeThunk.Call(NormalContext, SystemArgument1, SystemArgument2);
}

ULONG NTAPI RtlGetFullPathName_UDetour(PCWSTR FileName, ULONG Size, PWSTR Buffer, PWSTR* ShortName)
{
	HOOK_LOG(LL_CRI, "RtlGetFullPathName_U triggered!");

#if 0
	auto pModuleHookQueueData = std::make_shared<SModuleHookQueueData>();
	if (IS_VALID_SMART_PTR(pModuleHookQueueData))
	{
		pModuleHookQueueData->wszModuleName = FileName;
		pModuleHookQueueData->dwThreadID	= g_winapiApiTable->GetCurrentThreadId();
		pModuleHookQueueData->iCheckType	= CHECK_TYPE_RtlGetFullPathName_U;

		g_nmApp->SelfHooksInstance()->ModuleHookCallback(pModuleHookQueueData);
	}
#endif

	return hRtlGetFullPathName_U.Call(FileName, Size, Buffer, ShortName);
}

NTSTATUS NTAPI LdrGetDllHandleExDetour(IN ULONG Flags, IN PWSTR DllPath OPTIONAL, IN PULONG DllCharacteristics OPTIONAL, IN PUNICODE_STRING DllName, OUT PVOID *DllHandle OPTIONAL)
{
	HOOK_LOG(LL_CRI, "LdrGetDllHandleEx triggered!");

#if 0
	auto pModuleRequestHookQueueData = std::make_shared<SModuleReqHookQueueData>();
	if (IS_VALID_SMART_PTR(pModuleRequestHookQueueData))
	{
		pModuleRequestHookQueueData->wszModuleName = DllName->Buffer;

		g_nmApp->SelfHooksInstance()->ModuleRequestHookCallback(pModuleRequestHookQueueData);
	}
#endif

	return hLdrGetDllHandleEx.Call(Flags, DllPath, DllCharacteristics, DllName, DllHandle);
}

NTSTATUS NTAPI NtMapViewOfSectionDetour(HANDLE SectionHandle, HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits, SIZE_T CommitSize, PLARGE_INTEGER SectionOffset, PSIZE_T ViewSize, SECTION_INHERIT InheritDisposition, ULONG AllocationType, ULONG Protect)
{
//	HOOK_LOG(LL_CRI, "NtMapViewOfSection triggered!");
	printf("NtMapViewOfSection triggered!");

#if 0
	auto currTeb = reinterpret_cast<TEB*>(__readfsdword(offsetof(NT_TIB, Self)));
	if (currTeb)
	{
		auto pSectionHookQueueData = std::make_shared<SSectionHookQueueData>();
		if (IS_VALID_SMART_PTR(pSectionHookQueueData))
		{
			pSectionHookQueueData->dwBase					= (DWORD)&BaseAddress;
			pSectionHookQueueData->lpArbitraryUserPointer	= currTeb->NtTib.ArbitraryUserPointer;

			g_nmApp->SelfHooksInstance()->SectionHookCallback(pSectionHookQueueData);
		}
	}
#endif

	return hNtMapViewOfSection.Call(SectionHandle, ProcessHandle, BaseAddress, ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition, AllocationType, Protect);
}

LONG WINAPI SetWindowLongADetour(HWND hWnd, int nIndex, LONG dwNewLong)
{
	HOOK_LOG(LL_CRI, "SetWindowLongA triggered! Wnd: %p Idx: %d NewTarget: %p", hWnd, nIndex, dwNewLong);

#if 0
	auto pWndProcHookQueueData = std::make_shared<SWndProcHookQueueData>();
	if (IS_VALID_SMART_PTR(pWndProcHookQueueData))
	{
		pWndProcHookQueueData->iType		= 1;
		pWndProcHookQueueData->hWnd			= hWnd;
		pWndProcHookQueueData->nIndex		= nIndex;
		pWndProcHookQueueData->dwNewLong	= dwNewLong;

		g_nmApp->SelfHooksInstance()->WndProcHookCallback(pWndProcHookQueueData);
	}
#endif

	return hSetWindowLongA.Call(hWnd, nIndex, dwNewLong);
}

LONG WINAPI SetWindowLongWDetour(HWND hWnd, int nIndex, LONG dwNewLong)
{
	HOOK_LOG(LL_CRI, "SetWindowLongW triggered! Wnd: %p Idx: %d NewTarget: %p", hWnd, nIndex, dwNewLong);

#if 0
	auto pWndProcHookQueueData = std::make_shared<SWndProcHookQueueData>();
	if (IS_VALID_SMART_PTR(pWndProcHookQueueData))
	{
		pWndProcHookQueueData->iType		= 2;
		pWndProcHookQueueData->hWnd			= hWnd;
		pWndProcHookQueueData->nIndex		= nIndex;
		pWndProcHookQueueData->dwNewLong	= dwNewLong;

		g_nmApp->SelfHooksInstance()->WndProcHookCallback(pWndProcHookQueueData);
	}
#endif

	return hSetWindowLongW.Call(hWnd, nIndex, dwNewLong);
}

inline bool ConnectRoutine(const struct sockaddr* name)
{
	HOOK_LOG(LL_SYS, "Connect called!");

	auto sockInfo = *(sockaddr_in*)name;
	auto szTargetAddress = g_winapiApiTable->inet_ntoa(sockInfo.sin_addr);

	DWORD dwErrorType = 0;
	if (g_nmApp->AnalyserInstance()->OnConnected(szTargetAddress, &dwErrorType) == false)
	{
		HOOK_LOG(LL_ERR, "Unknown connection blocked! Error code: %u", dwErrorType);
		return false;
	}
	return true;
}

int WINAPI connectDetour(UINT_PTR s, const struct sockaddr* name, int namelen)
{
	HOOK_LOG(LL_CRI, "connect triggered! Socket: %p Name: %s Length: %d", s, name, namelen);

#if 0
	if (ConnectRoutine(name) == false)
		return 0;
#endif

	return hconnect.Call(s, name, namelen);
}


inline bool NtDelayExecutionRoutine(DWORD_PTR dwCaller)
{
	// TODO: Check sleep interval and curr tid

	if (g_winapiModuleTable->hKernelbase)
	{
		auto kernelbaseSleepEx = (DWORD_PTR)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernelbase, xorstr("SleepEx").crypt_get());
		auto dif1 = dwCaller - kernelbaseSleepEx;
		auto dif11 = kernelbaseSleepEx - dwCaller;
		if (dif1 < 0xFF || dif11 < 0xFF)
			return true;

		auto kernelbaseSleep = (DWORD_PTR)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernelbase, xorstr("Sleep").crypt_get());
		auto dif2 = dwCaller - kernelbaseSleep;
		auto dif21 = kernelbaseSleep - dwCaller;
		if (dif2 < 0xFF || dif21 < 0xFF)
			return true;
	}

	auto kernelSleep = (DWORD_PTR)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("Sleep").crypt_get());
	auto dif3 = dwCaller - kernelSleep;
	auto dif31 = kernelSleep - dwCaller;
	if (dif3 < 0xFF || dif31 < 0xFF)
		return true;

	auto kernelSleepEx = (DWORD_PTR)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("SleepEx").crypt_get());
	auto dif4 = dwCaller - kernelSleepEx;
	auto dif41 = kernelSleepEx - dwCaller;
	if (dif4 < 0xFF || dif41 < 0xFF)
		return true;


	char szFileName[2048] = { 0 };
	if (g_winapiApiTable->GetMappedFileNameA(NtCurrentProcess, (LPVOID)dwCaller, szFileName, 2048))
	{
		if (g_nmApp->DirFunctionsInstance()->IsFromWindowsPath(szFileName))
			if (strstr(szFileName, xorstr("kernel32.dll").crypt_get()) || strstr(szFileName, xorstr("kernelbase.dll").crypt_get()))
				return true;
	}

	HOOK_LOG(LL_ERR, xorstr("Wait request coming from unknown target! %s:%p Skipped!").crypt_get(), szFileName, dwCaller);
	return false;
}

NTSTATUS NTAPI NtDelayExecutionDetour(BOOL Alertable, PLARGE_INTEGER DelayInterval)
{
	HOOK_LOG(LL_CRI, "NtDelayExecution triggered! Alert: %d Delay: %p", Alertable, DelayInterval->QuadPart);

#if 0
	// TODO: If alert state is active, check current thread

	auto dwCaller = 0UL;
	__asm {
		push dword ptr[ebp + 4]
		pop  dword ptr[dwCaller]
	}

	if (NtDelayExecutionRoutine(dwCaller) == false)
		return STATUS_INVALID_PARAMETER;
#endif

	return hNtDelayExecution.Call(Alertable, DelayInterval);
}



BOOL WINAPI ClientThreadSetupDetour(VOID)
{
	HOOK_LOG(LL_CRI, "ClientThreadSetup triggered! TID: %u", g_winapiApiTable->GetCurrentThreadId());

	return hClientThreadSetup.Call();
}

NTSTATUS NTAPI NtCreateSectionDetour(PHANDLE SectionHandle, ULONG DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PLARGE_INTEGER MaximumSize, ULONG PageAttributess,
	ULONG SectionAttributes, HANDLE FileHandle)
{
	HOOK_LOG(LL_CRI, "NtCreateSection triggered!");

#if 0
	// TODO: Ýf it is not sec image, check is it contain MZ header

	HOOK_LOG(LL_CRI, "NtCreateSection informations: Handle: %p Access: %p ObjAttr: %p PageAttr: %p SectionAttr: %p",
		*SectionHandle, DesiredAccess, ObjectAttributes ? ObjectAttributes->Attributes : 0, PageAttributess, SectionAttributes);

	auto ulReturnLength = 0UL;
	OBJECT_NAME_INFORMATION pName[4096] = { 0 };
	auto ntStat = g_winapiApiTable->NtQueryObject(FileHandle, ObjectNameInformation, &pName, 4096, &ulReturnLength);
	if (NT_SUCCESS(ntStat))
	{
		if (pName && pName->Name.Buffer && pName->Name.Length)
		{
			auto wszName = std::wstring(pName->Name.Buffer);
			auto szName = std::string(wszName.begin(), wszName.end());

			HOOK_LOG(LL_CRI, "NtCreateSection informations: File: %s IsImage: %d", szName.c_str(), SectionAttributes == SEC_IMAGE);
		}
	}
#endif

	return hNtCreateSection.Call(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, PageAttributess, SectionAttributes, FileHandle);
}




ULONG WINAPI RtlDispatchExceptionDetour(PEXCEPTION_RECORD ExceptionInfo, CONTEXT * pContext)
{
//	HOOK_LOG(LL_CRI, "RtlDispatchException triggered! Exception data: %p Context: %p", ExceptionInfo, *pContext);

	// TODO: check - if *(BYTE*)ExceptionInfo->ExceptionAddress == 0xCC

//	EXCEPTION_POINTERS ep;
//	ep.ExceptionRecord	= ExceptionInfo;
//	ep.ContextRecord	= pContext;
//	CExceptionHandlers::OnExceptionThrowed(&ep);

#if 0
	auto dwErrorType = 0UL;
	if (g_nmApp->AnalyserInstance()->OnExceptionThrowed(ExceptionInfo, &dwErrorType) == false)
	{
		auto szLog = g_nmApp->FunctionsInstance()->CreateString(xorstr("A unknown exception throwed! Error code: %u"), dwErrorType);
		FileLogf(CUSTOM_LOG_FILENAME, szLog.c_str());

		// dont call external func just terminate or return access violation
		return STATUS_ACCESS_VIOLATION;
	}
#endif

	return m_fnRtlDispatchException(ExceptionInfo, pContext);
}

bool InitKiUserExceptionDispatcherHook()
{
	auto pAddr = (BYTE *)NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hNtdll, xorstr("KiUserExceptionDispatcher").crypt_get());
	if (!pAddr)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_EXCEPTION_DISPATCHER_FOUND_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "KiUserExceptionDispatcher at: %p - %0x%X", pAddr, *pAddr);

	while (*pAddr != 0xE8)
		pAddr++;
	DEBUG_LOG(LL_SYS, "KiUserExceptionDispatcher call: %p - %0x%X", pAddr, *pAddr);

	m_fnRtlDispatchException = (pfnRtlDispatchException)((*(DWORD *)(pAddr + 1)) + 5 + (DWORD_PTR)pAddr);
	if (!m_fnRtlDispatchException || IsBadCodePtr((FARPROC)m_fnRtlDispatchException))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_EXCEPTION_DISPATCHER_BASE_FOUND_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "RtlDispatchException: %p", m_fnRtlDispatchException);

	auto dwNewAddr = (DWORD)RtlDispatchExceptionDetour - (DWORD)pAddr - 5;
	if (!dwNewAddr)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_EXCEPTION_DISPATCHER_PATCH_ADDR_FOUND_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "KiUserExceptionDispatcher patch addr: %p", dwNewAddr);

	DWORD dwOldProtection;
	if (!g_winapiApiTable->VirtualProtect((LPVOID)pAddr, 0x1000, PAGE_EXECUTE_READWRITE, &dwOldProtection))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_EXCEPTION_DISPATCHER_PRE_PROTECT_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	SIZE_T cbWrittenSize = 0;
	if (!g_winapiApiTable->WriteProcessMemory(NtCurrentProcess, (PVOID)((DWORD)pAddr + 1), &dwNewAddr, 4, &cbWrittenSize))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_EXCEPTION_DISPATCHER_WRITE_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	return true;
}


bool CSelfApiHooks::InitializePatchs()
{
	HOOK_LOG(LL_SYS, "API Patch Initilization has been started!");

	auto bRet = true;

#ifndef _DEBUG

#if (BLOCK_CONSOLE_WINDOW == TRUE)
	if (g_winapiModuleTable->hKernelbase)
	{
		if (g_nmApp->SelfHooksInstance()->BlockAPI(xorstr("kernelbase.dll").crypt_get(), xorstr("AllocConsole").crypt_get(), NOP_HOOK))
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_1_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}
		if (g_nmApp->SelfHooksInstance()->BlockAPI(xorstr("kernelbase.dll").crypt_get(), xorstr("AttachConsole").crypt_get(), NOP_HOOK))
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_2_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}
	}

	if (g_nmApp->SelfHooksInstance()->BlockAPI(xorstr("kernel32.dll").crypt_get(), xorstr("AllocConsole").crypt_get(), NOP_HOOK))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_3_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	if (g_nmApp->SelfHooksInstance()->BlockAPI(xorstr("kernel32.dll").crypt_get(), xorstr("GetConsoleWindow").crypt_get(), NOP_HOOK))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_4_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	if (g_nmApp->SelfHooksInstance()->BlockAPI(xorstr("kernel32.dll").crypt_get(), xorstr("AttachConsole").crypt_get(), NOP_HOOK))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_5_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

	if (g_nmApp->SelfHooksInstance()->BlockAPI(xorstr("ntdll.dll").crypt_get(), xorstr("DbgUiIssueRemoteBreakin").crypt_get(), RET_HOOK))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_6_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	if (g_nmApp->SelfHooksInstance()->BlockAPI(xorstr("ntdll.dll").crypt_get(), xorstr("DbgUiRemoteBreakin").crypt_get(), RET_HOOK))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_7_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	if (g_nmApp->SelfHooksInstance()->BlockAPI(xorstr("ntdll.dll").crypt_get(), xorstr("DbgBreakPoint").crypt_get(), NOP_HOOK))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_8_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
	if (g_nmApp->SelfHooksInstance()->BlockAPI(xorstr("ntdll.dll").crypt_get(), xorstr("DbgUserBreakPoint").crypt_get(), NOP_HOOK))
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_PATCH_9_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

	//if (hPython)
	//{
		//lpApiHooks.BlockAPI(xorstr("python"), xorstr("PyRun_SimpleString"), RET_HOOK);
		//lpApiHooks.BlockAPI(xorstr("python"), xorstr("PyRun_SimpleFile"), RET_HOOK);
		//lpApiHooks.BlockAPI(xorstr("python"), xorstr("PyRun_SimpleFileEx"), RET_HOOK);

		//lpApiHooks.BlockAPI(xorstr("python"), xorstr("PyRun_FileExFlags"), RET_HOOK);
		//lpApiHooks.BlockAPI(xorstr("python"), xorstr("PyFile_FromFile"), RET_HOOK);
		//lpApiHooks.BlockAPI(xorstr("python"), xorstr("PyParser_ASTFromFile"), RET_HOOK);
		//lpApiHooks.BlockAPI(xorstr("python"), xorstr("PyParser_ASTFromString"), NOP_HOOK);

		//lpApiHooks.BlockAPI(xorstr("python"), xorstr("PyFile_FromString"), RET_HOOK);
	//}

	HOOK_LOG(LL_SYS, "API Patch Initilization completed!");
	return bRet;
}


bool CSelfApiHooks::InitializeDetours()
{
	HOOK_LOG(LL_SYS, "Detours Initilization has been started!");

	m_pNktHook = new CNktHookLib();
	if (!m_pNktHook)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_CLASS_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

#ifdef _DEBUG
	m_pNktHook->SetEnableDebugOutput(true);
#else
	m_pNktHook->SetEnableDebugOutput(false);
#endif

	auto dwFailAttempt	= 0UL;
	auto dwErrCode		= 0UL;
	auto fnAddress		= LPVOID(nullptr);

	// Hook routine

	/// 1
	fnAddress = NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hWs2_32, xorstr("connect").crypt_get());
	if (!fnAddress)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_1_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	dwErrCode += (m_pNktHook->Hook(&(hconnect.Id), (LPVOID*)&(hconnect.Call), fnAddress, connectDetour, 0));
	if (dwErrCode)
	{
		DEBUG_LOG(LL_ERR, "Detour 1 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_1_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	if (IsWindowsVistaOrGreater())
	{
		///2 
		fnAddress = NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hNtdll, xorstr("LdrInitializeThunk").crypt_get());
		if (!fnAddress)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_2_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}

		dwErrCode += (m_pNktHook->Hook(&(hLdrInitializeThunk.Id), (LPVOID*)&(hLdrInitializeThunk.Call), fnAddress, LdrInitializeThunkDetour, 0));
		if (dwErrCode)
		{
			DEBUG_LOG(LL_ERR, "Detour 2 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

			g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_2_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}

		/// 3
		fnAddress = NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hNtdll, xorstr("RtlGetFullPathName_U").crypt_get());
		if (!fnAddress)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_3_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}

		dwErrCode += (m_pNktHook->Hook(&(hRtlGetFullPathName_U.Id), (LPVOID*)&(hRtlGetFullPathName_U.Call), fnAddress, RtlGetFullPathName_UDetour, 0));
		if (dwErrCode)
		{
			DEBUG_LOG(LL_ERR, "Detour 3 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

			g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_3_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}
	}

	if (IsWindows7OrGreater())
	{
		/// 4
		fnAddress = NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hNtdll, xorstr("LdrGetDllHandleEx").crypt_get());
		if (!fnAddress)
		{
			g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_4_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}

		dwErrCode += (m_pNktHook->Hook(&(hLdrGetDllHandleEx.Id), (LPVOID*)&(hLdrGetDllHandleEx.Call), fnAddress, LdrGetDllHandleExDetour, 0));
		if (dwErrCode)
		{
			DEBUG_LOG(LL_ERR, "Detour 4 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

			g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_4_FAIL, g_winapiApiTable->GetLastError());
			return false;
		}
	}
	
	/// 5
	fnAddress = NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hNtdll, xorstr("NtMapViewOfSection").crypt_get());
	if (!fnAddress)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_5_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

#if 0
	// deadlocka giriyor
	dwErrCode += (m_pNktHook->Hook(&(hNtMapViewOfSection.Id), (LPVOID*)&(hNtMapViewOfSection.Call), fnAddress, NtMapViewOfSectionDetour, 0));
	if (dwErrCode)
	{
		DEBUG_LOG(LL_ERR, "Detour 5 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_5_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}
#endif

	/// Exception dispatcher
	if (InitKiUserExceptionDispatcherHook() == false)
	{
		DEBUG_LOG(LL_ERR, "Exception patcher can not initialized! Error: %u", g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_EXCEPTION_PATCH_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	/// 6
	fnAddress = NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hNtdll, xorstr("NtDelayExecution").crypt_get());
	if (!fnAddress)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_6_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	dwErrCode += (m_pNktHook->Hook(&(hNtDelayExecution.Id), (LPVOID*)&(hNtDelayExecution.Call), fnAddress, NtDelayExecutionDetour, 0));
	if (dwErrCode) 
	{
		DEBUG_LOG(LL_ERR, "Detour 6 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_6_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	/// 7
	fnAddress = NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hUser32, xorstr("SetWindowLongA").crypt_get());
	if (!fnAddress)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_7_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	dwErrCode += (m_pNktHook->Hook(&(hSetWindowLongA.Id), (LPVOID*)&(hSetWindowLongA.Call), fnAddress, SetWindowLongADetour, 0));
	if (dwErrCode)
	{
		DEBUG_LOG(LL_ERR, "Detour 7 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_7_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	/// 8
	fnAddress = NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hUser32, xorstr("SetWindowLongW").crypt_get());
	if (!fnAddress)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_8_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	dwErrCode += (m_pNktHook->Hook(&(hSetWindowLongW.Id), (LPVOID*)&(hSetWindowLongW.Call), fnAddress, SetWindowLongWDetour, 0));
	if (dwErrCode)
	{
		DEBUG_LOG(LL_ERR, "Detour 8 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_8_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	/// 9
	fnAddress = NktHookLibHelpers::GetProcedureAddress((HMODULE)g_winapiModuleTable->hUser32, xorstr("ClientThreadSetup").crypt_get());
	if (!fnAddress)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_9_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	dwErrCode += (m_pNktHook->Hook(&(hClientThreadSetup.Id), (LPVOID*)&(hClientThreadSetup.Call), fnAddress, ClientThreadSetupDetour, 0));
	if (dwErrCode)
	{
		DEBUG_LOG(LL_ERR, "Detour 9 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_9_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	/// 10
	// ...

	/// 11
	fnAddress = NktHookLibHelpers::GetProcedureAddress(g_winapiModuleTable->hNtdll, xorstr("NtCreateSection").crypt_get());
	if (!fnAddress)
	{
		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_API_FOUND_11_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	dwErrCode += (m_pNktHook->Hook(&(hNtCreateSection.Id), (LPVOID*)&(hNtCreateSection.Call), fnAddress, NtCreateSectionDetour, 0));
	if (dwErrCode)
	{
		DEBUG_LOG(LL_ERR, "Detour 11 can not initialized! Error code: %u Last Error: %u", dwErrCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_HOOK_DETOUR_INIT_11_FAIL, g_winapiApiTable->GetLastError());
		return false;
	}

	HOOK_LOG(LL_SYS, "Detours Initilization completed!");
	return true;
}
