#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "AntiDebug.h"
#include "DynamicWinapi.h"
#include "Functions.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "ProcessFunctions.h"
#include "PEHelper.h"

#define NT_GLOBAL_FLAG_DEBUGGED (0x70)

#pragma optimize("", off)

#ifndef _M_X64

#pragma data_seg(".nmX1")
auto dwVehintoAddr = 0UL;
#pragma data_seg()
#pragma comment(linker, "/section:.nmX1,RWS")


LONG NTAPI VehIntoExceptionFilter( PEXCEPTION_POINTERS ExceptionInfo )
{
	ExceptionInfo->ContextRecord->Eip = dwVehintoAddr;
	return EXCEPTION_CONTINUE_EXECUTION;
}

inline void VehIntoBreak()
{
	auto pVEHHandle = ::g_winapiApiTable->AddVectoredExceptionHandler(1, VehIntoExceptionFilter);

	_asm
	{
		mov dwVehintoAddr, offset Intosafe;
		mov ecx, 1;
	}
here:
	_asm
	{
		rol ecx, 1;
		into;
		jmp here;
	}
Intosafe:

	if (pVEHHandle)
		g_winapiApiTable->RemoveVectoredExceptionHandler(pVEHHandle);
}

#endif

inline bool SetFakeImageBase()
{
	auto pCurrModule = reinterpret_cast<ANTI_MODULE_INFO*>(g_nmApp->DataInstance()->GetAntiModuleInformations());
	if (!pCurrModule)
		return false;

	auto dwCurrBase = reinterpret_cast<DWORD_PTR>(pCurrModule->BaseAddress);
	if (!dwCurrBase)
		return false;

	auto pLdrData = reinterpret_cast<LDR_MODULE*>(g_nmApp->DynamicWinapiInstance()->GetLdrModule(dwCurrBase));
	if (!pLdrData)
		return false;

	auto pNewBase = reinterpret_cast<DWORD_PTR>(pLdrData->BaseAddress) + 0x100000;
	DEBUG_LOG(LL_SYS, "Old image base: %p redirected to: %p", pLdrData->BaseAddress, pNewBase);

	pLdrData->BaseAddress = reinterpret_cast<LPVOID>(pNewBase);
	return true;
}

inline bool SetFakeImageSize()
{
	auto pCurrModule = reinterpret_cast<ANTI_MODULE_INFO*>(g_nmApp->DataInstance()->GetAntiModuleInformations());
	if (!pCurrModule)
		return false;

	auto dwCurrBase = reinterpret_cast<DWORD_PTR>(pCurrModule->BaseAddress);
	if (!dwCurrBase)
		return false;

	auto pLdrData = reinterpret_cast<LDR_MODULE*>(g_nmApp->DynamicWinapiInstance()->GetLdrModule(dwCurrBase));
	if (!pLdrData)
		return false;

	auto dwNewSize = 0x1000000;
	DEBUG_LOG(LL_SYS, "Old image size: %p converted to: %p", pLdrData->SizeOfImage, dwNewSize);

	pLdrData->SizeOfImage = dwNewSize;
	return true;
}

inline void CrashDebuggerViaFormatText()
{
	CHAR szFakeFormat[] = { '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', '%', 's', 0x0 }; //%s%s%s%s%s%s%s%s%s%s%s
	g_winapiApiTable->OutputDebugStringA(szFakeFormat);
}


inline bool BasicDebugTriggered()
{
	if (g_winapiApiTable->IsDebuggerPresent()) 
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: g_winapiApiTable->IsDebuggerPresent");
		return true;
	}

	auto pPEB = GetCurrentPEB();
	if (pPEB && pPEB->BeingDebugged)
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: pPEB->BeingDebugged");
		return true;
	}

	if (pPEB && pPEB->NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: pPEB->NtGlobalFlag");
		return true;
	}

	auto dwFlags = 0UL;
	auto dwReturnSize = 0UL;
	if (NT_SUCCESS(g_winapiApiTable->NtQueryInformationProcess(NtCurrentProcess, ProcessDebugFlags, &dwFlags, sizeof(dwFlags), &dwReturnSize)))
	{
		if (dwReturnSize != sizeof(dwFlags) || dwFlags == 0)
		{
			DEBUG_LOG(LL_ERR, "Debugger detected via: NtQueryInformationProcess.ProcessDebugFlags");
			return true;
		}
	}

	return false;
}

inline bool RemoteDebugTriggered()
{
	auto bDebugged = BOOL(FALSE);
	auto bApiRet = g_winapiApiTable->CheckRemoteDebuggerPresent(NtCurrentProcess, &bDebugged);
	if (bApiRet && bDebugged)
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: CheckRemoteDebuggerPresent");
		return true;
	}

	return false;
}

inline bool PEBWow64Triggered()
{
#ifndef _M_X64
	if (g_nmApp->FunctionsInstance()->IsWow64Process(NtCurrentProcess))
	{
		auto bDebugged = BYTE(0x0);
		__asm
		{
			pushad
				mov eax, dword ptr fs : [0x18]
				sub eax, 0x2000
				mov eax, dword ptr[eax + 0x60];
				mov al, byte ptr[eax + 0x2]
				mov bDebugged, al
				popad
		}

		if (bDebugged)
		{
			DEBUG_LOG(LL_ERR, "Debugger detected via: PEBWow64Triggered");
			return true;
		}
	}
#endif
	return false;
}

bool DebugPortTriggered()
{
	auto hPort = HANDLE(INVALID_HANDLE_VALUE);
	if (NT_SUCCESS(g_winapiApiTable->NtQueryInformationProcess(NtCurrentProcess, ProcessDebugPort, &hPort, sizeof(hPort), 0)))
	{
		if (IS_VALID_HANDLE(hPort)) 
		{
			DEBUG_LOG(LL_ERR, "Debugger detected via: ProcessDebugPort");
			return true;
		}
	}

	return false;
}

inline bool CheckCloseHandle()
{
	__try 
	{
		g_winapiApiTable->CloseHandle((HANDLE)0x13371337);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return true;
	}
	return false;
}

bool CloseHandleViaInvalidHandleTriggered()
{
	if (CheckCloseHandle())
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: CloseHandleViaInvalidHandleTriggered");
		return true;
	}
	return false;
}

inline bool CheckCloseHandle2()
{
	auto hMutex = g_winapiApiTable->CreateMutexA(NULL, FALSE, xorstr("ntdil.dli").crypt_get());
	if (IS_VALID_HANDLE(hMutex))
	{
		if (g_winapiApiTable->SetHandleInformation(hMutex, HANDLE_FLAG_PROTECT_FROM_CLOSE, HANDLE_FLAG_PROTECT_FROM_CLOSE))
		{
			__try 
			{
				g_winapiApiTable->CloseHandle(hMutex);
			}
			__except (HANDLE_FLAG_PROTECT_FROM_CLOSE)
			{
				return true;
			}
		}
	}

	return false;
}

bool CloseHandleViaProtectedHandleTriggered()
{
	if (CheckCloseHandle2())
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: CloseHandleViaProtectedHandleTriggered");
		return true;
	}

	return false;
}

bool DetachFromDebuggerProcessTriggered()
{
	auto hDebugObject = HANDLE(INVALID_HANDLE_VALUE);
	auto dwFlags = 0UL;

	auto ntStatus = g_winapiApiTable->NtQueryInformationProcess(NtCurrentProcess, ProcessDebugObjectHandle, &hDebugObject, sizeof(HANDLE), NULL);
	if (!NT_SUCCESS(ntStatus))
		return false;

	ntStatus = g_winapiApiTable->NtSetInformationDebugObject(hDebugObject, DebugObjectFlags, &dwFlags, sizeof(dwFlags), NULL);
	if (!NT_SUCCESS(ntStatus))
		return false;

	ntStatus = g_winapiApiTable->NtRemoveProcessDebug(NtCurrentProcess, hDebugObject);
	if (!NT_SUCCESS(ntStatus)) 
		return false;

	ntStatus = g_winapiApiTable->NtClose(hDebugObject);
	if (!NT_SUCCESS(ntStatus)) 
		return false;

	DEBUG_LOG(LL_ERR, "Debugger detected via: NtQuerySystemInformation.DetachFromDebuggerProcessTriggered");
	return true;
}

bool KernelDebugInformationCheckTriggered()
{
	SYSTEM_KERNEL_DEBUGGER_INFORMATION pSKDI = { 0 };
	if (NT_SUCCESS(g_winapiApiTable->NtQuerySystemInformation(SystemKernelDebuggerInformation, &pSKDI, sizeof(pSKDI), NULL)))
	{
		if (pSKDI.KernelDebuggerEnabled && !pSKDI.KernelDebuggerNotPresent)
		{
			DEBUG_LOG(LL_ERR, "Debugger detected via: SystemKernelDebuggerInformation");
			return true;
		}
	}
	return false;
}


bool SofticeSymbolTriggered()
{
	auto hSoftICE	= g_winapiApiTable->CreateFileA(xorstr("////.//SICE").crypt_get(),			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	auto hSoftICE2	= g_winapiApiTable->CreateFileA(xorstr("////.//SIWVID").crypt_get(),		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	auto hSoftICE3	= g_winapiApiTable->CreateFileA(xorstr("////.//SIWVIDSTART").crypt_get(),	GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	auto hNTICE		= g_winapiApiTable->CreateFileA(xorstr("////.//NTICE").crypt_get(),			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	auto hIceExt	= g_winapiApiTable->CreateFileA(xorstr("////.//ICEEXT").crypt_get(),		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	auto hTRW		= g_winapiApiTable->CreateFileA(xorstr("////.//TRW").crypt_get(),			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	auto hTRWDebug	= g_winapiApiTable->CreateFileA(xorstr("////.//TRWDEBUG").crypt_get(),		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (IS_VALID_HANDLE(hSoftICE)) 
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SofticeSymbolTriggered.__Softice");
		return true;
	}
	if (IS_VALID_HANDLE(hSoftICE2))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SofticeSymbolTriggered.__Softice2");
		return true;
	}
	if (IS_VALID_HANDLE(hSoftICE3)) 
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SofticeSymbolTriggered.__Softice3");
		return true;
	}
	if (IS_VALID_HANDLE(hNTICE))  
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SofticeSymbolTriggered.__NTIce");
		return true;
	}
	if (IS_VALID_HANDLE(hIceExt)) 
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SofticeSymbolTriggered.__IceExt");
		return true;
	}
	if (IS_VALID_HANDLE(hTRW))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SofticeSymbolTriggered.__TRW");
		return true;
	}
	if (IS_VALID_HANDLE(hTRWDebug)) 
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SofticeSymbolTriggered.__TRWDEBUG");
		return true;
	}

	return false;
}

bool SyserSymbolTriggered()
{
	auto hSyser	= g_winapiApiTable->CreateFileA(xorstr("\\\\.\\Syser").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSyser)) 
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SyserSymbolTriggered.hSyser");
		return true;
	}

	auto hSyserBoot	= g_winapiApiTable->CreateFileA(xorstr("\\\\.\\SyserBoot").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSyserBoot)) 
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SyserSymbolTriggered.hSyserBoot");
		return true;
	}

	auto hSyserDebugMsg = g_winapiApiTable->CreateFileA(xorstr("\\\\.\\SyserDbgMsg").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSyserDebugMsg))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SyserSymbolTriggered.hSyserDebugMsg");
		return true;
	}

	return false;
}

bool GlobalFlagsClearInProcessTriggered()
{
	auto pImageBase	= (PBYTE)g_winapiModuleTable->hBaseModule;
	if (!pImageBase)
		return false;

	auto pIDH = (PIMAGE_DOS_HEADER)pImageBase;
	if (!pIDH || pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pINH = (PIMAGE_NT_HEADERS)(pImageBase + pIDH->e_lfanew);
	if (!pINH || pINH->Signature != IMAGE_NT_SIGNATURE)
		return false;

	auto pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pImageBase + pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress);
	if (pImageLoadConfigDirectory->GlobalFlagsClear)
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: GlobalFlagsClearInProcessTriggered");
		return true;
	}

	return false;
}

bool CheckDebugObjectsTriggered()
{
	if (IsWindows7OrGreater() == false)
		return false;

	ULONG dwSize = 0;
	POBJECT_TYPES_INFORMATION pTypesInfo = NULL;

	auto ntStatus = g_winapiApiTable->NtQueryObject(NULL, ObjectTypesInformation, NULL, 0, &dwSize);
	if (ntStatus == STATUS_INFO_LENGTH_MISMATCH)
	{
		pTypesInfo = (POBJECT_TYPES_INFORMATION)g_winapiApiTable->VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		if (!pTypesInfo)
			return false;

		ntStatus = g_winapiApiTable->NtQueryObject(NULL, ObjectTypesInformation, pTypesInfo, dwSize, &dwSize);
		if (!NT_SUCCESS(ntStatus))
			g_winapiApiTable->VirtualFree(pTypesInfo, 0, MEM_RELEASE);
	}

	if (ntStatus == STATUS_SUCCESS)
	{
		for (std::size_t i = 0; i < pTypesInfo->NumberOfTypes; i++)
		{
			if (pTypesInfo->TypeInformation[i].TypeName.Length && pTypesInfo->TypeInformation[i].TypeName.Buffer)
			{
				auto wszTypeName = std::wstring(pTypesInfo->TypeInformation[i].TypeName.Buffer);
				auto szTypeName = std::string(wszTypeName.begin(), wszTypeName.end());

				if (!strcmp(szTypeName.c_str(), xorstr("DebugObject").crypt_get()))
				{
					g_winapiApiTable->VirtualFree(pTypesInfo, 0, MEM_RELEASE);
					DEBUG_LOG(LL_ERR, "Debugger detected via: CheckDebugObjectsTriggered");
					return true;
				}
			}
		}

		g_winapiApiTable->VirtualFree(pTypesInfo, 0, MEM_RELEASE);
	}
	return false;
}

bool ThreadBreakOnTerminationTriggered()
{
	auto dwResult = 0UL;
	if (NT_SUCCESS(g_winapiApiTable->ZwSetInformationThread(NtCurrentThread, ThreadBreakOnTermination, &dwResult, sizeof(dwResult))))
	{
		if (dwResult)
		{
			DEBUG_LOG(LL_ERR, "Debugger detected via: ThreadBreakOnTerminationTriggered");
			return true;
		}
	}
	
	return false;
}

bool CAntiDebug::SeDebugPrivTriggered()
{
	auto dwCsrssPid = g_winapiApiTable->CsrGetProcessId();
	if (!dwCsrssPid)
		return false;

	auto hProcess = g_winapiApiTable->OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwCsrssPid);
	if (IS_VALID_HANDLE(hProcess)) 
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: SeDebugPrivTriggered");
		return true;
	}

	return false;
}


bool CheckDebugObjectSize()
{
	OBJECT_ATTRIBUTES oa;
	InitializeObjectAttributes(&oa, 0, 0, 0, 0);

	BYTE pMemory[0x1000] = { 0 };

	auto hDebugObject = HANDLE(INVALID_HANDLE_VALUE);
	if (NT_SUCCESS(g_winapiApiTable->NtCreateDebugObject(&hDebugObject, DEBUG_ALL_ACCESS, &oa, 0)))
	{
		auto pObjectType = reinterpret_cast<POBJECT_TYPE_INFORMATION>(pMemory);
		if (NT_SUCCESS(g_winapiApiTable->NtQueryObject(hDebugObject, ObjectTypeInformation, pObjectType, sizeof(pMemory), 0)))
		{
			if (pObjectType->TotalNumberOfObjects == 0)
			{
				DEBUG_LOG(LL_ERR, "Debugger detected via: CheckDebugObjectHandles");
				return true;
			}
		}
		g_winapiApiTable->NtClose(hDebugObject);
	}

	return false;
}

inline bool CheckSystemDebugControl()
{
	auto dwReturnLength		= 0UL;
	auto ntStatus			= g_winapiApiTable->NtSystemDebugControl(SysDbgBreakPoint, NULL, 0, NULL, 0, NULL);

	DEBUG_LOG(LL_SYS, "NtSystemDebugControl completed! Status: %p Return length: %u", ntStatus, dwReturnLength);
	return (ntStatus != STATUS_DEBUGGER_INACTIVE);
}

bool CheckHeapSetInformation()
{
	ULONG uHeapInfo = 2; /* HEAP_LFH */
	if (!g_winapiApiTable->HeapSetInformation(g_winapiApiTable->GetProcessHeap(), HeapCompatibilityInformation, &uHeapInfo, sizeof(uHeapInfo)))
		return true;

	return false;
}

bool CheckHeapTail()
{
	DWORD flag[] = { 0xabababab, 0xabababab };
	auto buff = reinterpret_cast<DWORD_PTR>(g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(32));

	auto temp = buff + 32;

	auto dwRet = memcmp((LPVOID)temp, (LPVOID)flag, 8);

	g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(reinterpret_cast<PVOID>(buff));
	return dwRet == 0;
}

bool CheckSharedUserData()
{
	auto pUserSharedData = (KUSER_SHARED_DATA *)0x7FFE0000; // The fixed user mode address of KUSER_SHARED_DATA

	BOOLEAN KdDebuggerEnabled		= (pUserSharedData->KdDebuggerEnabled & 0x1) == 0x1;
	BOOLEAN KdDebuggerNotPresent	= (pUserSharedData->KdDebuggerEnabled & 0x2) == 0x0;

	if (KdDebuggerEnabled || !KdDebuggerNotPresent)
		return true;

	return false;
}

bool CheckDebugFilterState()
{
	auto ntStatus = g_winapiApiTable->NtSetDebugFilterState(0, 0, TRUE);
	if (!NT_SUCCESS(ntStatus))
		return true;

	return false;
}

bool CheckEmulationWithTime()
{
	auto tStart = std::chrono::high_resolution_clock::now();

	g_winapiApiTable->GetCurrentProcessId();
	g_winapiApiTable->GetCurrentProcessId();
	g_winapiApiTable->GetCurrentProcessId();
	g_winapiApiTable->GetCurrentProcessId();

	auto tDiff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tStart).count();

	if (tDiff > 100)
	{
		return true;
	}
	return false;
}

bool CheckHeapFlags()
{
	auto bRet			= false;
	auto pDebugBuffer	= g_winapiApiTable->RtlCreateQueryDebugBuffer(0, FALSE);
	if (pDebugBuffer)
	{
		auto ntStatus = g_winapiApiTable->RtlQueryProcessDebugInformation(g_winapiApiTable->GetCurrentProcessId(), PDI_HEAPS | PDI_HEAP_BLOCKS, pDebugBuffer);
		if (NT_SUCCESS(ntStatus))
		{
			auto pHeapInfo = PDEBUG_HEAP_INFORMATION(PULONG(pDebugBuffer->Heaps) + 1);

			if (pHeapInfo->Flags == 0x50000062)
				bRet = true;

			g_winapiApiTable->RtlDestroyQueryDebugBuffer(pDebugBuffer);
		}
	}
	return bRet;
}

bool CheckCloseWindow()
{
	auto dwRet = g_winapiApiTable->CloseWindow((HWND)0x13371337);
	if (dwRet != 0 || g_winapiApiTable->GetLastError() != ERROR_INVALID_WINDOW_HANDLE)
		return true;
	return false;
}

bool CheckSystemTime()
{
	auto bRet = false;
	BOOLEAN bAdjustPrivRet;
	auto ntStatus = g_winapiApiTable->RtlAdjustPrivilege(SE_SYSTEMTIME_PRIVILEGE, TRUE, FALSE, &bAdjustPrivRet);
	if (NT_SUCCESS(ntStatus))
	{
		auto hEvent = g_winapiApiTable->CreateEventA(NULL, FALSE, FALSE, NULL);
		if (IS_VALID_HANDLE(hEvent))
		{
			if (NT_SUCCESS(g_winapiApiTable->NtSetSystemInformation(SystemTimeSlipNotification, &hEvent, sizeof(hEvent))))
			{
				if (g_winapiApiTable->WaitForSingleObject(hEvent, 1) == WAIT_OBJECT_0)
					bRet = true;
			}
			g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hEvent);
		}
	}
	return bRet;
}

bool CheckDebugObjectHandle()
{
	auto hDebugObject = HANDLE(INVALID_HANDLE_VALUE);
	auto ntStatus = g_winapiApiTable->NtQueryInformationProcess(NtCurrentProcess, ProcessDebugObjectHandle, &hDebugObject, sizeof(HANDLE), NULL);
	if (!NT_SUCCESS(ntStatus))
		return false;

	if (ntStatus != STATUS_PORT_NOT_SET) 
		return true;

	if (ntStatus == STATUS_PORT_NOT_SET && hDebugObject)
		return true;

	return false;
}

bool CheckBugCheck()
{
	auto dwBugCheck = 0UL;
	auto ntStatus = g_winapiApiTable->NtQueryInformationProcess(NtCurrentProcess, ProcessDebugObjectHandle, (PVOID)&dwBugCheck, sizeof(dwBugCheck), &dwBugCheck);
	if (ntStatus == STATUS_PORT_NOT_SET && dwBugCheck != 4)
	{
		return true;
	}
	return false;
}

bool CheckDebugBoot()
{
	auto hKey = HKEY(nullptr);
	auto dwRegOpenRet = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("System\\CurrentControlSet\\Control").crypt_get(), 0, KEY_READ, &hKey);
	if (dwRegOpenRet != ERROR_SUCCESS)
	{
		return false;
	}

	char szBootOptions[1024]	= { 0 };
	auto dwLen					= DWORD(sizeof(szBootOptions) - sizeof(CHAR));
	auto dwRegGetRet			= g_winapiApiTable->RegGetValueA(hKey, NULL, xorstr("SystemStartOptions").crypt_get(), RRF_RT_REG_SZ, NULL, szBootOptions, &dwLen);
	if (dwRegGetRet != ERROR_SUCCESS)
	{
		g_winapiApiTable->RegCloseKey(hKey);
		return false;
	}

	auto szLowerOptions = g_nmApp->FunctionsInstance()->szLower(szBootOptions);
	if (strstr(szLowerOptions.c_str(), xorstr("debug").crypt_get()))
	{
		g_winapiApiTable->RegCloseKey(hKey);
		return true;
	}

	g_winapiApiTable->RegCloseKey(hKey);
	return false;
}

bool CheckWow64Context()
{
	WOW64_CONTEXT context = { 0 };
	context.ContextFlags = WOW64_CONTEXT_ALL;

	if (NT_SUCCESS(g_winapiApiTable->NtQueryInformationThread(NtCurrentThread, ThreadWow64Context, &context, sizeof(context), NULL)))
	{
		if (context.Dr0 || context.Dr1 || context.Dr2 || context.Dr3 || context.Dr6 || context.Dr7)
		{
			DEBUG_LOG(LL_ERR, "Debugger detected via: ThreadWow64Context");
			return true;
		}
	}
	return false;
}

bool CheckKernelDebugInformationEx()
{
	if (!IsWindows8Point1OrGreater())
		return false;

	SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX pSKDIex = { 0 };
	if (NT_SUCCESS(g_winapiApiTable->NtQuerySystemInformation(SystemKernelDebuggerInformationEx, &pSKDIex, sizeof(pSKDIex), NULL)))
	{
		if (pSKDIex.DebuggerAllowed || pSKDIex.DebuggerEnabled || pSKDIex.DebuggerPresent)
		{
			DEBUG_LOG(LL_ERR, "Debugger detected via: SystemKernelDebuggerInformationEx");
			return true;
		}
	}
	return false;
}

bool CheckKernelDebugFlags()
{
	if (!IsWindows10OrGreater())
		return false;

	SYSTEM_KERNEL_DEBUGGER_FLAGS pSKDF = { 0 };
	auto ntStatus = g_winapiApiTable->NtQuerySystemInformation(SystemKernelDebuggerFlags, &pSKDF, sizeof(pSKDF), NULL);
	if (NT_SUCCESS(ntStatus))
		return true;

	return false;
}


DWORD CheckOtherSymbols()
{
	auto hSymbol = g_winapiApiTable->CreateFileA(xorstr("\\\\.\\pipe\\cuckoo").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSymbol))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: pipe\\cuckoo");
		return 1;
	}

	hSymbol = g_winapiApiTable->CreateFileA(xorstr("\\\\.\\OsiData").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSymbol))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: OsiData");
		return 2;
	}

	hSymbol = g_winapiApiTable->CreateFileA(xorstr("\\\\.\\DbgMsg").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSymbol))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: DbgMsg");
		return 3;
	}

	hSymbol = g_winapiApiTable->CreateFileA(xorstr("\\\\.\\LiveKd").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSymbol))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: LiveKd");
		return 4;
	}

	hSymbol = g_winapiApiTable->CreateFileA(xorstr("\\\\.\\W32Dasm").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSymbol))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: W32Dasm");
		return 5;
	}

	hSymbol = g_winapiApiTable->CreateFileA(xorstr("\\\\.\\ICEDUMP").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSymbol))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: ICEDUMP");
		return 6;
	}

	hSymbol = g_winapiApiTable->CreateFileA(xorstr("\\\\.\\EXTREM").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSymbol))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: EXTREM");
		return 7;
	}

	hSymbol = g_winapiApiTable->CreateFileA(xorstr("\\\\.\\RING0").crypt_get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (IS_VALID_HANDLE(hSymbol))
	{
		DEBUG_LOG(LL_ERR, "Debugger detected via: RING0");
		return 8;
	}

	return 0;
}



bool CAntiDebug::InitAntiDebug(LPDWORD pdwErrorStep)
{
	DEBUG_LOG(LL_SYS, "Anti debug initialization has been started!");

	auto dwSymbolRet	= 0UL;
	auto dwDebugRet		= 0UL;
	do
	{
		CrashDebuggerViaFormatText();
		DEBUG_LOG(LL_SYS, "Anti debug step 1 completed!");

		if (BasicDebugTriggered())
		{
			dwDebugRet = 1;
			break;
		}

		if (RemoteDebugTriggered())
		{
			dwDebugRet = 2;
			break;
		}

		if (DetachFromDebuggerProcessTriggered())
		{
			dwDebugRet = 3;
			break;
		}

		if (PEBWow64Triggered())
		{
			dwDebugRet = 4;
			break;
		}

		if (DebugPortTriggered())
		{
			dwDebugRet = 5;
			break;
		}

		DEBUG_LOG(LL_SYS, "Anti debug step 2 completed!");

		if (CloseHandleViaInvalidHandleTriggered())
		{
			dwDebugRet = 6;
			break;
		}

		if (CloseHandleViaProtectedHandleTriggered())
		{
			dwDebugRet = 7;
			break;
		}

		if (KernelDebugInformationCheckTriggered())
		{
			dwDebugRet = 8;
			break;
		}

		if (SofticeSymbolTriggered())
		{
			dwDebugRet = 9;
			break;
		}

		if (SyserSymbolTriggered())
		{
			dwDebugRet = 10;
			break;
		}

		DEBUG_LOG(LL_SYS, "Anti debug step 3 completed!");

		if (CheckSystemDebugControl())
		{
			dwDebugRet = 11;
			break;
		}

		if (CheckDebugObjectsTriggered())
		{
			dwDebugRet = 12;
			break;
		}

		if (ThreadBreakOnTerminationTriggered())
		{
			dwDebugRet = 13;
			break;
		}

		if (g_nmApp->DataInstance()->IsPackedProcess() == false && GlobalFlagsClearInProcessTriggered())
		{
			dwDebugRet = 14;
			break;
		}

		if (!NT_SUCCESS(g_winapiApiTable->ZwSetInformationThread(NtCurrentThread, ThreadHideFromDebugger, 0, 0)))
		{
			dwDebugRet = 15;
			break;
		}

		DEBUG_LOG(LL_SYS, "Anti debug step 3 completed!");

		BOOLEAN bIsHidden = FALSE;
		if (!NT_SUCCESS(g_winapiApiTable->NtQueryInformationThread(NtCurrentThread, ThreadHideFromDebugger, &bIsHidden, sizeof(bIsHidden), NULL)))
		{
			dwDebugRet = 16;
			break;
		}

		if (!bIsHidden)
		{
			dwDebugRet = 17;
			break;
		}

		if (CheckSharedUserData())
		{
			dwDebugRet = 18;
			break;
		}

		if (CheckHeapSetInformation())
		{
			dwDebugRet = 19;
			break;
		}

		if (CheckHeapTail())
		{
			dwDebugRet = 20;
			break;
		}

		DEBUG_LOG(LL_SYS, "Anti debug step 4 completed!");

		if (CheckDebugFilterState())
		{
			dwDebugRet = 21;
			break;
		}

		if (CheckEmulationWithTime())
		{
			dwDebugRet = 22;
			break;
		}

		if (CheckHeapFlags())
		{
			dwDebugRet = 23;
			break;
		}

		if (CheckCloseWindow())
		{
			dwDebugRet = 24;
			break;
		}

		if (CheckSystemTime())
		{
			dwDebugRet = 25;
			break;
		}

		DEBUG_LOG(LL_SYS, "Anti debug step 5 completed!");

		if (CheckDebugObjectHandle())
		{
			dwDebugRet = 26;
			break;
		}

		if (CheckBugCheck())
		{
			dwDebugRet = 27;
			break;
		}

		if (CheckDebugBoot())
		{
			dwDebugRet = 28;
			break;
		}

		if (CheckWow64Context())
		{
			dwDebugRet = 29;
			break;
		}

		if (CheckKernelDebugInformationEx())
		{
			dwDebugRet = 30;
			break;
		}

		if (CheckKernelDebugFlags())
		{
			dwDebugRet = 31;
			break;
		}

		DEBUG_LOG(LL_SYS, "Anti debug step 6 completed!");

#ifndef _M_X64
		VehIntoBreak();
#endif

		DEBUG_LOG(LL_SYS, "Anti debug step 7 completed!");

		if (IsWindowsVistaOrGreater())
		{
			SetFakeImageSize();
			SetFakeImageBase();
		}

		DEBUG_LOG(LL_SYS, "Anti debug step 8 completed!");

		dwSymbolRet = CheckOtherSymbols();
		if (dwSymbolRet)
		{
			dwDebugRet = 100 + dwSymbolRet;
			break;
		}

	} while (false);

	DEBUG_LOG(dwDebugRet == 0 ? LL_SYS : LL_CRI, "Debug check routine completed! Result: %u", dwDebugRet);

	if (pdwErrorStep) *pdwErrorStep = dwDebugRet;
	return dwDebugRet == 0;
}

bool CAntiDebug::CheckRuntimeAntiDebug(LPDWORD pdwDetectType)
{
#ifdef _DEBUG
	if (g_winapiApiTable->IsDebuggerPresent()) // pass it for debug build and if have a attached debugger
		return false;
#endif

	if (DetachFromDebuggerProcessTriggered())
	{
		if (pdwDetectType) *pdwDetectType = 1;
		return true;
	}

	if (KernelDebugInformationCheckTriggered())
	{
		if (pdwDetectType) *pdwDetectType = 2;
		return true;
	}

	if (CloseHandleViaInvalidHandleTriggered())
	{
		if (pdwDetectType) *pdwDetectType = 3;
		return true;
	}

	return false;
}

bool CAntiDebug::IsImageSumCorrupted(DWORD dwBaseImage, DWORD dwCorrectSum)
{
	auto pIDH = reinterpret_cast<PIMAGE_DOS_HEADER>(dwBaseImage);
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pINH = reinterpret_cast<PIMAGE_NT_HEADERS>((reinterpret_cast<DWORD_PTR>(pIDH) + pIDH->e_lfanew));
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return false;

	auto pISH = reinterpret_cast<PIMAGE_SECTION_HEADER>((reinterpret_cast<DWORD_PTR>(pINH) + sizeof(pINH->Signature) + sizeof(IMAGE_FILE_HEADER) + pINH->FileHeader.SizeOfOptionalHeader));
	if (!pISH)
		return false;

	auto dwCurrentSum = CPEFunctions::CalculateChecksum(dwBaseImage + pISH->VirtualAddress, pISH->SizeOfRawData);
	return (dwCurrentSum != dwCorrectSum);
}

#pragma optimize("", on)
