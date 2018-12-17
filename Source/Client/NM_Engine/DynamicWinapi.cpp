#include "NM_Index.h"
#include "NM_Main.h"
#include "DynamicWinAPI.h"
#include "WinVerHelper.h"
#include "SpdLog_Wrapper.h"
#include "NktHookWrapper.h"
#include "Defines.h"
#include "LDasm.h"

#include <boost/algorithm/string/predicate.hpp>
#include <lazy_importer.hpp>

std::shared_ptr <WINAPI_MODULE_TABLE>	g_winapiModuleTable;
std::shared_ptr <WINAPI_API_TABLE>		g_winapiApiTable;

CDynamicWinapi::CDynamicWinapi() :
	m_bHasInitialized(false)
{
	m_ntHelper = std::make_shared<CNT>();
	if (!IS_VALID_SMART_PTR(m_ntHelper))
	{
		DEBUG_LOG(LL_ERR, "m_ntHelper allocation fail! Error code: %u", LI_FIND(GetLastError)());
		return;
	}
}

CDynamicWinapi::~CDynamicWinapi()
{
	m_bHasInitialized = false;
}

bool CDynamicWinapi::IsValidHandle(HANDLE hTarget)
{
	auto dwInfo = 0UL;
	if (LI_FIND(GetHandleInformation)(hTarget, &dwInfo) == false)
		return false;
	return true;
}

bool CDynamicWinapi::SafeCloseHandle(HANDLE hTarget)
{
	__try
	{
		if (!IsValidHandle(hTarget))
			return false;

		if (m_bHasInitialized)
			return g_winapiApiTable->CloseHandle(hTarget);
		else
			return LI_FIND(CloseHandle)(hTarget);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
	return true;
}

inline bool PatchMemory(LPVOID lpSource, LPCVOID c_pTarget, DWORD dwSize)
{
	auto dwProtection = 0UL;
	__try
	{
		if (!VirtualProtect(lpSource, dwSize, PAGE_READWRITE, &dwProtection))
			return false;

		memcpy(lpSource, c_pTarget, dwSize);

		if (!VirtualProtect(lpSource, dwSize, dwProtection, &dwProtection))
			return false;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) 
	{ 
		return false;
	};

	return true;
}

bool CDynamicWinapi::PatchWow64Redirection() // TODO
{
#if 0
	if (!g_nmApp->FunctionsInstance()->IsWow64Process(NtCurrentProcess))
	{
		DEBUG_LOG(LL_SYS, "Fixup skipped for non-wow64 process!");
		return true;
	}

	auto pMappedNtdllInfo = g_nmApp->ManualMapInstance()->GetMappedModuleInformations(g_winapiModuleTable->hNtdll);
	if (!IS_VALID_SMART_PTR(pMappedNtdllInfo))
	{
		DEBUG_LOG(LL_SYS, "Can not found mapped ntdll module!");
		return false;
	}

	auto dwMappedWow64Transition = g_winapiApiTable->GetProcAddress((HMODULE)pMappedNtdllInfo->dwMappedBase, xorstr("Wow64Transition"));
	if (!dwMappedWow64Transition)
	{
		DEBUG_LOG(LL_SYS, "Mapped Wow64Transition not found!");
		return false;
	}

	auto dwMemoryWow64Transition = g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("Wow64Transition"));
	if (!dwMemoryWow64Transition)
	{
		DEBUG_LOG(LL_SYS, "Memory Wow64Transition not found!");
		return false;
	}

	DEBUG_LOG(LL_SYS, "Wow64Transition data; Memory: %p Mapped: %p", dwMemoryWow64Transition, dwMappedWow64Transition);

	if (!PatchMemory(dwMappedWow64Transition, dwMemoryWow64Transition, 5))
	{
		DEBUG_LOG(LL_SYS, "Wow64Transition patch fail!");
		return false;
	}

	// ---

	auto hWow64Cpu = g_winapiApiTable->GetModuleHandleA_o(xorstr("wow64cpu.dll"));
	if (!hWow64Cpu)
	{
		// ...
	}

	DWORD dwWow64Address = 0x0;
	__asm {
		push dword ptr fs : [0xC0]
		pop  dword ptr[dwWow64Address]
	}

	if (dwWow64Address is not in dwWow64Address range)
	{
		// ...
	}

	// TODO: Map wow64cpu

	auto pMappedWow64Cpu = g_nmApp->ManualMapInstance()->GetMappedModuleInformations(hWow64Cpu);
	if (!IS_VALID_SMART_PTR(pMappedWow64Cpu))
	{
		// ...
	}

	auto dwMemoryWow64GatewayDif = dwWow64Address - hWow64Cpu.module_base
	if (!dwMemoryWow64GatewayDif)
	{
		// ...
	}

	auto dwMappedWow64Gateway = pMappedWow64Cpu->dwMappedBase + dwMemoryWow64GatewayDif;
	if (!dwMappedWow64Gateway)
	{
		// ...
	}

	if (!PatchMemory(dwMappedWow64Gateway, dwWow64Address, 0x1000))
	{
		// ...
	}
#endif
	return true;
}

PVOID CDynamicWinapi::GetModuleAddressFromName(const wchar_t* c_wszName, bool bIsCompleteCheck)
{
	auto pPEB = GetCurrentPEB();
	auto CurrentEntry = pPEB->LoaderData->InLoadOrderModuleList.Flink;
	while (CurrentEntry != &pPEB->LoaderData->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		auto Current = CONTAINING_RECORD(CurrentEntry, _LDR_MODULE, InLoadOrderModuleList);

		if (bIsCompleteCheck && boost::iequals(Current->FullDllName.Buffer, c_wszName))
			return Current->BaseAddress;
		else if (!bIsCompleteCheck && boost::contains(Current->FullDllName.Buffer, c_wszName))
			return Current->BaseAddress;

		CurrentEntry = CurrentEntry->Flink;
	}
	return nullptr;
}
std::string CDynamicWinapi::GetModuleNameFromAddress(DWORD_PTR dwAddress, bool bFullName)
{
	auto pPEB = GetCurrentPEB();
	auto CurrentEntry = pPEB->LoaderData->InLoadOrderModuleList.Flink;
	while (CurrentEntry != &pPEB->LoaderData->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		auto Current = CONTAINING_RECORD(CurrentEntry, _LDR_MODULE, InLoadOrderModuleList);

		if (dwAddress >= reinterpret_cast<DWORD_PTR>(Current->BaseAddress) && dwAddress <= (reinterpret_cast<DWORD_PTR>(Current->BaseAddress) + Current->SizeOfImage))
		{
			auto szName = std::string("");
			if (bFullName)
			{
				auto wszName	= std::wstring(Current->FullDllName.Buffer);
				szName			= std::string(wszName.begin(), wszName.end());
			}
			else
			{
				auto wszName	= std::wstring(Current->BaseDllName.Buffer);
				szName			= std::string(wszName.begin(), wszName.end());
			}

			return szName;
		}
		CurrentEntry = CurrentEntry->Flink;
	}
	return std::string("");
}
bool CDynamicWinapi::IsLoadedAddress(DWORD_PTR dwAddress)
{
	auto pPEB = GetCurrentPEB();
	auto CurrentEntry = pPEB->LoaderData->InLoadOrderModuleList.Flink;
	while (CurrentEntry != &pPEB->LoaderData->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		auto Current = CONTAINING_RECORD(CurrentEntry, _LDR_MODULE, InLoadOrderModuleList);
		if (dwAddress == reinterpret_cast<DWORD_PTR>(Current->BaseAddress))
			return true;

		CurrentEntry = CurrentEntry->Flink;
	}
	return false;
}
bool CDynamicWinapi::DestroyEntrypoint(DWORD_PTR dwAddress)
{
	auto pPEB = GetCurrentPEB();
	auto CurrentEntry = pPEB->LoaderData->InLoadOrderModuleList.Flink;
	while (CurrentEntry != &pPEB->LoaderData->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		auto Current = CONTAINING_RECORD(CurrentEntry, _LDR_MODULE, InLoadOrderModuleList);
		if (dwAddress == reinterpret_cast<DWORD_PTR>(Current->BaseAddress))
		{
			Current->EntryPoint = nullptr;
			return true;
		}

		CurrentEntry = CurrentEntry->Flink;
	}
	return false;
}
LPVOID CDynamicWinapi::GetLdrModule(DWORD_PTR dwAddress)
{
	auto pPEB = GetCurrentPEB();
	auto CurrentEntry = pPEB->LoaderData->InLoadOrderModuleList.Flink;
	while (CurrentEntry != &pPEB->LoaderData->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		auto Current = CONTAINING_RECORD(CurrentEntry, _LDR_MODULE, InLoadOrderModuleList);
		if (dwAddress == reinterpret_cast<DWORD_PTR>(Current->BaseAddress))
		{
			return Current;
		}

		CurrentEntry = CurrentEntry->Flink;
	}
	return nullptr;
}
LPVOID CDynamicWinapi::FindModuleFromAddress(DWORD_PTR dwAddress)
{
	auto pPEB = GetCurrentPEB();
	auto CurrentEntry = pPEB->LoaderData->InLoadOrderModuleList.Flink;
	while (CurrentEntry != &pPEB->LoaderData->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		auto Current = CONTAINING_RECORD(CurrentEntry, _LDR_MODULE, InLoadOrderModuleList);
		if (dwAddress >= reinterpret_cast<DWORD_PTR>(Current->BaseAddress) && dwAddress <= (reinterpret_cast<DWORD_PTR>(Current->BaseAddress) + Current->SizeOfImage))
		{
			return Current;
		}

		CurrentEntry = CurrentEntry->Flink;
	}
	return nullptr;
}

static HMODULE WINAPI MyGetModuleHandleA(_In_ LPCSTR lpModuleName)
{
	// DEBUG_LOG(LL_SYS, "MyGetModuleHandleA called for: %s", lpModuleName);

	auto hTmpModule = LI_FIND(GetModuleHandleA)(lpModuleName);
	if (!hTmpModule) 
	{
		DEBUG_LOG(LL_ERR, "GetModuleHandleA fail! Module: %s Error code: %u", lpModuleName, LI_FIND(GetLastError)());
		hTmpModule = LI_FIND(LoadLibraryA)(lpModuleName);
	}

	if (!hTmpModule)
	{
		DEBUG_LOG(LL_ERR, "LoadLibraryA fail! Module: %s Error code: %u", lpModuleName, LI_FIND(GetLastError)());

		auto szLowerModuleName = g_nmApp->FunctionsInstance()->szLower(lpModuleName);

		if (IsWindowsVistaOrGreater() == false && (strstr(szLowerModuleName.c_str(), xorstr("kernelbase").crypt_get()) || strstr(szLowerModuleName.c_str(), xorstr("wevtapi").crypt_get())))
		{
			DEBUG_LOG(LL_SYS, "Module: %s passed for WinXP", lpModuleName);
			return nullptr;
		}

		if ( (g_nmApp->DataInstance()->GetAppType() == METIN2_GAME && !strstr(szLowerModuleName.c_str(), xorstr("python").crypt_get()) ) ||
			g_nmApp->DataInstance()->GetAppType() != METIN2_GAME)
		{
			DEBUG_LOG(LL_ERR, "Module: %s not found!", lpModuleName);
			g_nmApp->OnCloseRequest(EXIT_ERR_MODULE_NOT_FOUND, g_winapiApiTable->GetLastError(), (void*)lpModuleName);
			return nullptr;
		}
	}

	// Add current module to module list
	if (hTmpModule && !g_nmApp->IsModuleExistOnModuleList(hTmpModule))
		g_nmApp->AddModuleToModuleList(hTmpModule);

	// Check routine
	const size_t cSize = strlen(lpModuleName) + 1;
	wchar_t* c_wszModuleName = new wchar_t[cSize];
	mbstowcs(c_wszModuleName, lpModuleName, cSize);

	auto lpAddressFromMemory_nkt = NktHelper::GetModuleBaseAddress_W(c_wszModuleName);
	if (lpAddressFromMemory_nkt && lpAddressFromMemory_nkt != hTmpModule && 
		(g_nmApp->DataInstance()->GetAntiModule() && lpAddressFromMemory_nkt != g_nmApp->DataInstance()->GetAntiModule()))
	{
		DEBUG_LOG(LL_ERR, "Module manipulation detected: %s | %p-%p", lpModuleName, lpAddressFromMemory_nkt, hTmpModule);

		g_nmApp->OnCloseRequest(EXIT_ERR_MODULE_MANIPULATION_DETECTED, g_winapiApiTable->GetLastError(), (void*)lpModuleName);
		return nullptr;
	}

	delete[] c_wszModuleName;

	return hTmpModule;
}


inline PVOID GetRealAddress(PVOID pAddress)
{
#ifdef _M_IX86
	if (*(PBYTE)pAddress == 0xE9 || *(PBYTE)pAddress == 0xE8)
		return Relative2Absolute(pAddress, 1, 5);

	if (*(PBYTE)pAddress == 0x68 && *((PBYTE)pAddress + 5) == 0xC3)
		return GetAbsolutePtr(pAddress, 1);

	if (*(PBYTE)pAddress == 0xB8 && *(PWORD)((PBYTE)pAddress + 5) == 0xE0FF)
		return GetAbsolutePtr(pAddress, 1);

	if (*(PWORD)pAddress == 0xFF2E)
		return GetAbsolutePtr(pAddress, 2);

#elif _M_X64

	if (*(PBYTE)pAddress == 0xE9)
		return Relative2Absolute(pAddress, 1, 5);

	if (*(PWORD)pAddress == 0xB849 && *(PWORD)((PBYTE)pAddress + 10) == 0xE0FF)
		return GetAbsolutePtr(pAddress, 2);

	if (*(PWORD)pAddress == 0x25FF && *(PULONG)((PBYTE)pAddress + 2) == 0x00000000)
		return GetAbsolutePtr(pAddress, 6);

	if (*(PWORD)pAddress == 0x25FF && *(PULONG)((PBYTE)pAddress + 2) != 0x00000000)
		return Relative2Absolute(pAddress, 2, 6);

	if (*(PWORD)pAddress == 0xB848 && *(PWORD)((PBYTE)pAddress + 10) == 0xC350)
		return GetAbsolutePtr(pAddress, 2);
#endif

	return pAddress;
}

inline bool InModuleRange(HMODULE hModule, DWORD_PTR dwAddress)
{
	bool bRet = false;

	MODULEINFO mi = { 0 };
	if (LI_FIND(GetModuleInformation)(NtCurrentProcess, hModule, &mi, sizeof(mi)))
	{
		auto dwBase = reinterpret_cast<DWORD_PTR>(mi.lpBaseOfDll);
		auto dwHi	= reinterpret_cast<DWORD_PTR>(mi.lpBaseOfDll) + mi.SizeOfImage;

		bRet = (dwAddress >= dwBase && dwAddress <= dwHi);
	}
	return bRet;
}

static FARPROC WINAPI MyGetProcAddress(_In_ HMODULE hModule, _In_ LPCSTR lpProcName)
{
	auto fptmpAdr = reinterpret_cast<FARPROC>(NktHelper::GetProcAddress_A(hModule, lpProcName));
	if (!fptmpAdr)
	{
		fptmpAdr = CustomGetProcAddress(hModule, lpProcName);
	}

	if (!fptmpAdr)
	{
		fptmpAdr = LI_FIND(GetProcAddress)(hModule, lpProcName);
	}

	if (!fptmpAdr)
	{
		auto szModuleName = g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD)hModule);

		DEBUG_LOG(LL_ERR, "ERROR! %s!%s Windows API not initialized!", szModuleName.c_str(), lpProcName);

		g_nmApp->OnCloseRequest(EXIT_ERR_WINAPI_INIT_FAIL, g_winapiApiTable->GetLastError(), (void*)lpProcName);
		return 0;
	}

	if (g_nmApp->DynamicWinapiInstance()->HasInitialized() == false) 
	{
		auto szModuleName = g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress((DWORD)hModule);
		DEBUG_LOG(LL_SYS, "Getprocaddress called for: %s!%s Address: %p", szModuleName.c_str(), lpProcName, fptmpAdr);

#if 0
		// TESTME
		auto iStep = 0;
		while (InModuleRange(hModule, reinterpret_cast<DWORD_PTR>(fptmpAdr)) == false)
		{
			auto pRedirectPtr = (FARPROC)GetRealAddress(fptmpAdr);
			auto szRealOwner = g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress(reinterpret_cast<DWORD_PTR>(pRedirectPtr));
			DEBUG_LOG(LL_ERR, "Hook detected! API: %s Hooked address: %p Real Address: %p Owner: %s Step: %d", lpProcName, fptmpAdr, pRedirectPtr, szRealOwner.c_str(), iStep++);
			fptmpAdr = pRedirectPtr;
		}
#endif

		auto fpRealAddress = (FARPROC)GetRealAddress(fptmpAdr);
		if (fpRealAddress != fptmpAdr)
		{
			auto szRealOwner = g_nmApp->DynamicWinapiInstance()->GetModuleNameFromAddress(reinterpret_cast<DWORD_PTR>(fpRealAddress));
			DEBUG_LOG(LL_ERR, "Hook detected! Hooked address: %p Real Address: %p Owner: %s", fptmpAdr, fpRealAddress, szRealOwner.c_str());
			fptmpAdr = fpRealAddress;
		}
	}

	return fptmpAdr;
}

HMODULE CDynamicWinapi::GetPythonHandle()
{
	auto hPython = g_nmApp->DataInstance()->GetPythonHandle();
	if (hPython)
		return hPython;

	auto hTmp27 = MyGetModuleHandleA(xorstr("python27.dll").crypt_get());
	if (hTmp27)
	{
		g_nmApp->DataInstance()->SetPythonName(xorstr("python27.dll").crypt_get());
		g_nmApp->DataInstance()->SetPythonHandle(hTmp27);

		return hTmp27;
	}

	auto hTmp22 = MyGetModuleHandleA(xorstr("python22.dll").crypt_get());
	if (hTmp22)
	{
		g_nmApp->DataInstance()->SetPythonName(xorstr("python22.dll").crypt_get());
		g_nmApp->DataInstance()->SetPythonHandle(hTmp22);

		return hTmp22;
	}

	DEBUG_LOG(LL_CRI, "Unknown python module!");
	return nullptr;
}


bool CDynamicWinapi::BindBaseAPIs()
{
	g_winapiApiTable->GetProcAddress = MyGetProcAddress;
	if (!g_winapiApiTable->GetProcAddress) 
	{
		DEBUG_LOG(LL_ERR, xorstr("MyGetProcAddress not found!").crypt_get());
		return false;
	}

	g_winapiApiTable->GetProcAddress_o = LI_FIND(GetProcAddress);
	if (!g_winapiApiTable->GetProcAddress_o)
	{
		DEBUG_LOG(LL_ERR, xorstr("GetProcAddress_o not found!").crypt_get());
		return false;
	}

	g_winapiApiTable->GetModuleHandleA = MyGetModuleHandleA;
	if (!g_winapiApiTable->GetModuleHandleA)
	{
		DEBUG_LOG(LL_ERR, xorstr("MyGetModuleHandleA not found!").crypt_get());
		return false;
	}

	g_winapiApiTable->GetModuleHandleA_o = LI_FIND(GetModuleHandleA);
	if (!g_winapiApiTable->GetModuleHandleA_o) 
	{
		DEBUG_LOG(LL_ERR, xorstr("GetModuleHandleA_o not found!").crypt_get());
		return false;
	}

	g_winapiApiTable->LoadLibraryA = LI_FIND(LoadLibraryA);
	if (!g_winapiApiTable->LoadLibraryA) 
	{
		DEBUG_LOG(LL_ERR, xorstr("LoadLibraryA not found!").crypt_get());
		return false;
	}

	return true;
}

bool CDynamicWinapi::BindModules() 
{
	g_winapiModuleTable->hBaseModule = g_winapiApiTable->GetModuleHandleA_o(NULL);
	if (!g_winapiModuleTable->hBaseModule) 
	{
		DEBUG_LOG(LL_ERR, "hBaseModule bind fail!");
		return false;
	}

	g_winapiModuleTable->hKernel32 = g_winapiApiTable->GetModuleHandleA(xorstr("kernel32.dll").crypt_get());
	if (!g_winapiModuleTable->hKernel32) 
	{
		DEBUG_LOG(LL_ERR, "hKernel32 bind fail!");
		return false;
	}
	g_winapiModuleTable->hNtdll = g_winapiApiTable->GetModuleHandleA(xorstr("ntdll.dll").crypt_get());
	if (!g_winapiModuleTable->hNtdll) 
	{
		DEBUG_LOG(LL_ERR, "hNtdll bind fail!");
		return false;
	}
	g_winapiModuleTable->hUser32 = g_winapiApiTable->GetModuleHandleA(xorstr("user32.dll").crypt_get());
	if (!g_winapiModuleTable->hUser32) 
	{
		DEBUG_LOG(LL_ERR, "hUser32 bind fail!");
		return false;
	}
	g_winapiModuleTable->hPsapi = g_winapiApiTable->GetModuleHandleA(xorstr("psapi.dll").crypt_get());
	if (!g_winapiModuleTable->hPsapi)
	{
		DEBUG_LOG(LL_ERR, "hPsapi bind fail!");
		return false;
	}
	g_winapiModuleTable->hDbghelp = g_winapiApiTable->GetModuleHandleA(xorstr("dbghelp.dll").crypt_get());
	if (!g_winapiModuleTable->hDbghelp)
	{
		DEBUG_LOG(LL_ERR, "hDbghelp bind fail!");
		return false;
	}
	g_winapiModuleTable->hKernelbase = g_winapiApiTable->GetModuleHandleA(xorstr("kernelbase.dll").crypt_get());
	if (!g_winapiModuleTable->hKernelbase && IsWindows7OrGreater()) 
	{
		DEBUG_LOG(LL_ERR, "hKernelbase bind fail!");
		return false;
	}
	g_winapiModuleTable->hDbghelp = g_winapiApiTable->GetModuleHandleA(xorstr("dbghelp.dll").crypt_get());
	if (!g_winapiModuleTable->hDbghelp)
	{
		DEBUG_LOG(LL_ERR, "hDbghelp bind fail!");
		return false;
	}
	g_winapiModuleTable->hAdvapi32 = g_winapiApiTable->GetModuleHandleA(xorstr("advapi32.dll").crypt_get());
	if (!g_winapiModuleTable->hAdvapi32)
	{
		DEBUG_LOG(LL_ERR, "hAdvapi32 bind fail!");
		return false;
	}
	g_winapiModuleTable->hWininet = g_winapiApiTable->GetModuleHandleA(xorstr("wininet.dll").crypt_get());
	if (!g_winapiModuleTable->hWininet)
	{
		DEBUG_LOG(LL_ERR, "hWininet bind fail!");
		return false;
	}
	g_winapiModuleTable->hWinsta = g_winapiApiTable->GetModuleHandleA(xorstr("winsta.dll").crypt_get());
	if (!g_winapiModuleTable->hWinsta)
	{
		DEBUG_LOG(LL_ERR, "hWinsta bind fail!");
		return false;
	}
	g_winapiModuleTable->hShlwapi = g_winapiApiTable->GetModuleHandleA(xorstr("shlwapi.dll").crypt_get());
	if (!g_winapiModuleTable->hShlwapi)
	{
		DEBUG_LOG(LL_ERR, "hShlwapi bind fail!");
		return false;
	}
	g_winapiModuleTable->hShell32 = g_winapiApiTable->GetModuleHandleA(xorstr("shell32.dll").crypt_get());
	if (!g_winapiModuleTable->hShell32)
	{
		DEBUG_LOG(LL_ERR, "hShell32 bind fail!");
		return false;
	}
	g_winapiModuleTable->hCrypt32 = g_winapiApiTable->GetModuleHandleA(xorstr("crypt32.dll").crypt_get());
	if (!g_winapiModuleTable->hCrypt32) 
	{
		DEBUG_LOG(LL_ERR, "hCrypt32 bind fail!");
		return false;
	}
	g_winapiModuleTable->hWs2_32 = g_winapiApiTable->GetModuleHandleA(xorstr("ws2_32.dll").crypt_get());
	if (!g_winapiModuleTable->hWs2_32) 
	{
		DEBUG_LOG(LL_ERR, "hWs2_32 bind fail!");
		return false;
	}
	g_winapiModuleTable->hIphlpapi = g_winapiApiTable->GetModuleHandleA(xorstr("iphlpapi.dll").crypt_get());
	if (!g_winapiModuleTable->hIphlpapi)
	{
		DEBUG_LOG(LL_ERR, "hIphlpapi bind fail!");
		return false;
	}
	g_winapiModuleTable->hMpr = g_winapiApiTable->GetModuleHandleA(xorstr("mpr.dll").crypt_get());
	if (!g_winapiModuleTable->hMpr) 
	{
		DEBUG_LOG(LL_ERR, "hMpr bind fail!");
		return false;
	}
	g_winapiModuleTable->hWintrust = g_winapiApiTable->GetModuleHandleA(xorstr("wintrust.dll").crypt_get());
	if (!g_winapiModuleTable->hWintrust) 
	{
		DEBUG_LOG(LL_ERR, "hWintrust bind fail!");
		return false;
	}
	g_winapiModuleTable->hDnsapi = g_winapiApiTable->GetModuleHandleA(xorstr("DNSAPI.dll").crypt_get());
	if (!g_winapiModuleTable->hDnsapi) 
	{
		DEBUG_LOG(LL_ERR, "hDnsapi bind fail!");
		return false;
	}
	g_winapiModuleTable->hOle32 = g_winapiApiTable->GetModuleHandleA(xorstr("ole32.dll").crypt_get());
	if (!g_winapiModuleTable->hOle32) 
	{
		DEBUG_LOG(LL_ERR, "hOle32 bind fail!");
		return false;
	}
	g_winapiModuleTable->hGdiPlus = g_winapiApiTable->GetModuleHandleA(xorstr("gdiplus.dll").crypt_get());
	if (!g_winapiModuleTable->hGdiPlus)
	{
		DEBUG_LOG(LL_ERR, "hGdiPlus bind fail!");
		return false;
	}
	g_winapiModuleTable->hGdi32 = g_winapiApiTable->GetModuleHandleA(xorstr("gdi32.dll").crypt_get());
	if (!g_winapiModuleTable->hGdi32) 
	{
		DEBUG_LOG(LL_ERR, "hGdi32 bind fail!");
		return false;
	}
	g_winapiModuleTable->hEvtApi = g_winapiApiTable->GetModuleHandleA(xorstr("wevtapi.dll").crypt_get());
	if (!g_winapiModuleTable->hEvtApi && IsWindowsVistaOrGreater()) 
	{
		DEBUG_LOG(LL_ERR, "hEvtApi bind fail!");
		return false;
	}
	g_winapiModuleTable->hUserEnv = g_winapiApiTable->GetModuleHandleA(xorstr("Userenv.dll").crypt_get());
	if (!g_winapiModuleTable->hUserEnv)
	{
		DEBUG_LOG(LL_ERR, "hUserEnv bind fail!");
		return false;
	}

	HMODULE hPython = nullptr;
	if (g_nmApp->DataInstance()->GetAppType() == METIN2_GAME)
	{
		hPython = GetPythonHandle();
		if (!hPython)
		{
			DEBUG_LOG(LL_ERR, "hPython bind fail!");
			return false;
		}
	}
	g_winapiModuleTable->hPython = hPython;

	return true;
}

bool CDynamicWinapi::BindAPIs()
{
	BindAPIs_1();
	BindAPIs_2();
	BindAPIs_3();
	BindAPIs_4();

	if (IsWindowsXPSP1OrGreater())
	{
		g_winapiApiTable->GetProcessId = (lpGetProcessId)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetProcessId").crypt_get());
	}

	if (IsWindowsVistaOrGreater())
	{
		g_winapiApiTable->NtCreateThreadEx					= (lpNtCreateThreadEx)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtCreateThreadEx").crypt_get());
		g_winapiApiTable->QueryFullProcessImageNameA		= (lpQueryFullProcessImageNameA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("QueryFullProcessImageNameA").crypt_get());
		g_winapiApiTable->GetThreadId						= (lpGetThreadId)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetThreadId").crypt_get());
		g_winapiApiTable->CsrGetProcessId					= (lpCsrGetProcessId)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("CsrGetProcessId").crypt_get());
		g_winapiApiTable->NtGetNextThread					= (lpNtGetNextThread)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtGetNextThread").crypt_get());
		g_winapiApiTable->NtGetNextProcess					= (lpNtGetNextProcess)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtGetNextProcess").crypt_get());
		g_winapiApiTable->ProcessIdToSessionId				= (lpProcessIdToSessionId)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("ProcessIdToSessionId").crypt_get());
		g_winapiApiTable->QueryWorkingSetEx					= (lpQueryWorkingSetEx)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hPsapi, xorstr("QueryWorkingSetEx").crypt_get());
		g_winapiApiTable->EvtCreateRenderContext			= (lpEvtCreateRenderContext)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hEvtApi, xorstr("EvtCreateRenderContext").crypt_get());
		g_winapiApiTable->EvtQuery							= (lpEvtQuery)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hEvtApi, xorstr("EvtQuery").crypt_get());
		g_winapiApiTable->EvtNext							= (lpEvtNext)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hEvtApi, xorstr("EvtNext").crypt_get());
		g_winapiApiTable->EvtRender							= (lpEvtRender)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hEvtApi, xorstr("EvtRender").crypt_get());
		g_winapiApiTable->EvtOpenPublisherMetadata			= (lpEvtOpenPublisherMetadata)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hEvtApi, xorstr("EvtOpenPublisherMetadata").crypt_get());
		g_winapiApiTable->EvtClose							= (lpEvtClose)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hEvtApi, xorstr("EvtClose").crypt_get());
	}

	auto bWow64Ret = BOOL(FALSE);
	if (g_winapiApiTable->IsWow64Process(NtCurrentProcess, &bWow64Ret) && bWow64Ret)
	{
		g_winapiApiTable->Wow64DisableWow64FsRedirection	= (lpWow64DisableWow64FsRedirection)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("Wow64DisableWow64FsRedirection").crypt_get());
		g_winapiApiTable->Wow64RevertWow64FsRedirection		= (lpWow64RevertWow64FsRedirection)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("Wow64RevertWow64FsRedirection").crypt_get());
		g_winapiApiTable->NtWow64ReadVirtualMemory64		= (lpNtWow64ReadVirtualMemory64)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtWow64ReadVirtualMemory64").crypt_get());
		g_winapiApiTable->NtWow64QueryInformationProcess64	= (lpNtWow64QueryInformationProcess64)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtWow64QueryInformationProcess64").crypt_get());
		g_winapiApiTable->NtWow64WriteVirtualMemory64		= (lpNtWow64WriteVirtualMemory64)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtWow64WriteVirtualMemory64").crypt_get());
		g_winapiApiTable->NtWow64AllocateVirtualMemory64	= (lpNtWow64AllocateVirtualMemory64)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtWow64AllocateVirtualMemory64").crypt_get());
	}
	
	return true;
}

bool CDynamicWinapi::CheckModulesIntegrity(std::string * pszModuleName, LPDWORD dpwErrorStep) // TODO: Implement signature based check, WinSxS and Apisetschema verifcaton
{
	auto szSystemPath		= g_nmApp->DirFunctionsInstance()->SystemPath();
	auto szLowerSystemPath	= g_nmApp->FunctionsInstance()->szLower(szSystemPath);

	auto szWindowsPath		= g_nmApp->DirFunctionsInstance()->WinPath();
	auto szLowerWindowsPath = g_nmApp->FunctionsInstance()->szLower(szWindowsPath);
	auto szLowerWinSxsPath	= szLowerWindowsPath + xorstr("\\winsxs").crypt_get();

	for (const auto & hModule : g_nmApp->GetSelfModuleList())
	{
		if (hModule == g_winapiModuleTable->hBaseModule) // pass process base check
			continue;

		char pszModuleFilePath[MAX_PATH] = { 'U', 'n', 'k', 'n', 'o', 'w', 'n', ' ', 'M', 'o', 'd', 'u', 'l', 'e', 0x0 };
		if (!g_winapiApiTable->GetModuleFileNameA(hModule, pszModuleFilePath, MAX_PATH) || !strlen(pszModuleFilePath) || !strcmp(pszModuleFilePath, xorstr("Unknown Module").crypt_get()))
		{
			auto szFromPEB = GetModuleNameFromAddress(reinterpret_cast<DWORD_PTR>(hModule));
			DEBUG_LOG(LL_ERR, "GetModuleFileNameA fail! Target Module: %p Error: %u Forced: %s", hModule, LI_FIND(GetLastError)(), szFromPEB.c_str());
			continue;
		}

		DEBUG_LOG(LL_SYS, "Checking dll integrity! Target Module: %s Handle: %p", pszModuleFilePath, hModule);

		auto szLowerModuleFilePath = g_nmApp->FunctionsInstance()->szLower(pszModuleFilePath);
		if (szLowerModuleFilePath.find(szLowerSystemPath) == std::string::npos && /* win path is not contain on module path */
			szLowerModuleFilePath.find(szLowerWinSxsPath) == std::string::npos /* winsxs path ^ */)
		{
			DEBUG_LOG(LL_ERR, "Target module path is NOT contain system path! Sys: %s Sxs: %s Module: %s",
				szLowerSystemPath.c_str(), szLowerWinSxsPath.c_str(), szLowerModuleFilePath.c_str());

			if (pszModuleName)  *pszModuleName = szLowerModuleFilePath;
			if (dpwErrorStep)	*dpwErrorStep = 1;
			return false;
		}
	}

#if 0
	// aþaðýsý için win10 ve üzeri kontrolü
	auto szWindowsPath		= g_nmApp->DirFunctionsInstance()->WinPath();
	auto szLowerWindowsPath = g_nmApp->FunctionsInstance()->szLower(szWindowsPath);

	auto szExePath			= g_nmApp->DirFunctionsInstance()->ExePath();
	auto szLowerExePath		= g_nmApp->FunctionsInstance()->szLower(szExePath);	

	// TODO: Check integritys of non signed dlls
	// TODO: Check cert integrity of signed dlls(Microsoft only)
	// TODO: Check file hash integrity

	auto vWhiteList = std::vector<std::string>(); // non digital signed windows modules
	vWhiteList.push_back(xorstr("dbghelp.dll").crypt_get());
	vWhiteList.push_back(xorstr("wininet.dll").crypt_get());
	vWhiteList.push_back(xorstr("gdiplus.dll").crypt_get());

	for (const auto & hModule : g_nmApp->GetSelfModuleList())
	{
		if (hModule == g_winapiModuleTable->hBaseModule) // pass process base check
			continue;

		char szModuleFilePath[MAX_PATH] = { 'U', 'n', 'k', 'n', 'o', 'w', 'n', ' ', 'M', 'o', 'd', 'u', 'l', 'e', 0x0 };
		if (!g_winapiApiTable->GetModuleFileNameA(hModule, szModuleFilePath, MAX_PATH) || !strlen(szModuleFilePath) || !strcmp(szModuleFilePath, xorstr("Unknown Module").crypt_get()))
		{
			auto szFromPEB = GetModuleNameFromAddress(reinterpret_cast<DWORD_PTR>(hModule));
			DEBUG_LOG(LL_ERR, "GetModuleFileNameA fail! Target Module: %p Error: %u Forced: %s", hModule, LI_FIND(GetLastError)(), szFromPEB.c_str());
			continue;
		}

		// TODO: GetModuleFileNameA & GetMappedFileName compare

		DEBUG_LOG(LL_SYS, "Checking dll integirty! Target Module: %s Handle: %p", szModuleFilePath, hModule);

		// TODO: Check python dll integrity

		// TODO: Aþaðýsýný tek koþulda düzenle
		auto szLowerModuleFilePath = g_nmApp->FunctionsInstance()->szLower(szModuleFilePath);
		auto wszLowerModuleFilePath = std::wstring(szLowerModuleFilePath.begin(), szLowerModuleFilePath.end());
		auto szLowerModuleName = g_nmApp->DirFunctionsInstance()->GetNameFromPath(szLowerModuleFilePath);

		auto bIsFromGamePath = bool(strstr(szLowerModuleFilePath.c_str(), szLowerExePath.c_str()));
		auto bIsFromWindows = bool(strstr(szLowerModuleFilePath.c_str(), szLowerWindowsPath.c_str()));

		if (g_nmApp->DataInstance()->GetAppType() == METIN2_GAME)
		{
			auto bIsPython						= (hModule == GetPythonHandle());
			auto bIsPythonAndNotFromGame		= (bIsPython && !bIsFromGamePath);
			auto bIsPythonAndNotFromWindows		= (bIsPython && !bIsFromWindows);
			auto bIsNotPythonAndNotFromWindows	= (!bIsPython && !bIsFromWindows);

			if ((bIsPythonAndNotFromGame && bIsPythonAndNotFromWindows) || bIsNotPythonAndNotFromWindows)
			{
				DEBUG_LOG(LL_ERR, "Corrupted module found! Module: %s", szLowerModuleFilePath.c_str());

				if (pszModuleName)  *pszModuleName	= szLowerModuleFilePath;
				if (dpwErrorStep)	*dpwErrorStep	= 1;
				return false;
			}

			auto bSignRet = g_nmApp->ScannerInstance()->IsFileDigitalSigned(wszLowerModuleFilePath.c_str());
			if (bSignRet == false && bIsPython == false && HasInVector(vWhiteList, szLowerModuleName) == false)
			{
				DEBUG_LOG(LL_ERR, "Corrupted module found! Module: %s", szLowerModuleFilePath.c_str());

				if (pszModuleName)  *pszModuleName	= szLowerModuleFilePath;
				if (dpwErrorStep)	*dpwErrorStep	= 2;
				return false;
			}
		}
		else // -> if (g_nmApp->DataInstance()->GetAppType() != METIN2_GAME)
		{
			if (!bIsFromWindows) /* module not from windows */
			{
				DEBUG_LOG(LL_ERR, "Corrupted module found! Module: %s", szLowerModuleFilePath.c_str());

				if (pszModuleName)  *pszModuleName	= szLowerModuleFilePath;
				if (dpwErrorStep)	*dpwErrorStep	= 3;
				return false;
			}

			auto bSignRet = g_nmApp->ScannerInstance()->IsFileDigitalSigned(wszLowerModuleFilePath.c_str());
			if (bSignRet == false && HasInVector(vWhiteList, szLowerModuleName) == false)
			{
				DEBUG_LOG(LL_ERR, "Corrupted module found! Module: %s", szLowerModuleFilePath.c_str());
				
//				if (pszModuleName)  *pszModuleName	= szLowerModuleFilePath;
//				if (dpwErrorStep)	*dpwErrorStep	= 4;
//				return false;
			}
		}
	}
#endif
	return true;
}

bool CDynamicWinapi::Initialize(LPDWORD pdwErrorStep)
{
	g_winapiModuleTable = std::make_shared<WINAPI_MODULE_TABLE>();
	if (!IS_VALID_SMART_PTR(g_winapiModuleTable))
	{
		DEBUG_LOG(LL_CRI, "g_winapiModuleTable allocation fail! Last error: %u", LI_FIND(GetLastError)());
		if (pdwErrorStep) *pdwErrorStep = 1;
		return false;
	}

	g_winapiApiTable = std::make_shared<WINAPI_API_TABLE>();
	if (!IS_VALID_SMART_PTR(g_winapiApiTable))
	{
		DEBUG_LOG(LL_CRI, "g_winapiApiTable allocation fail! Last error: %u", LI_FIND(GetLastError)());
		if (pdwErrorStep) *pdwErrorStep = 2;
		return false;
	}

	if (!BindBaseAPIs())
	{
		DEBUG_LOG(LL_CRI, "BindBaseAPIs fail! Last error: %u", LI_FIND(GetLastError)());
		if (pdwErrorStep) *pdwErrorStep = 3;
		return false;
	}

	if (!BindModules())
	{
		DEBUG_LOG(LL_CRI, "BindModules fail! Last error: %u", LI_FIND(GetLastError)());
		if (pdwErrorStep) *pdwErrorStep = 4;
		return false;
	}

	if (!BindAPIs())
	{
		DEBUG_LOG(LL_CRI, "BindAPIs fail! Last error: %u", LI_FIND(GetLastError)());
		if (pdwErrorStep) *pdwErrorStep = 5;
		return false;
	}

	DEBUG_LOG(LL_SYS, "Dynamic winapi initilization completed!");
	m_bHasInitialized = true;
	return true;
}

void CDynamicWinapi::Finalize()
{
	if (IS_VALID_SMART_PTR(g_winapiModuleTable))
	{
		g_winapiModuleTable.reset();
		g_winapiModuleTable = nullptr;
	}
	if (IS_VALID_SMART_PTR(g_winapiApiTable))
	{
		g_winapiApiTable.reset();
		g_winapiApiTable = nullptr;
	}
	m_bHasInitialized = false;
}

bool CDynamicWinapi::HasInitialized()
{
	return m_bHasInitialized;
}
