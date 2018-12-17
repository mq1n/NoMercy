#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Defines.h"
#include "WinVerHelper.h"

inline int FilterShellcode(LPVOID lpTargetAddress)
{
	HOOK_LOG(LL_SYS, "Shellcode filter has been started!");

#ifdef _DEBUG
	BYTE bBytes[16] = { 0 };
	memcpy(bBytes, lpTargetAddress, sizeof(bBytes));

	HOOK_LOG(LL_SYS, "Shellcode Info -> Address: %p Bytes: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
		lpTargetAddress, bBytes[0], bBytes[1], bBytes[2], bBytes[3], bBytes[4], bBytes[5], bBytes[6], bBytes[7], bBytes[8], bBytes[9], bBytes[10], bBytes[11]);

	char szFileName[2048] = { 0 };
	g_winapiApiTable->GetMappedFileNameA(NtCurrentProcess, lpTargetAddress, szFileName, sizeof(szFileName));
	HOOK_LOG(LL_SYS, "Shellcode mapped module name: %s", szFileName ? szFileName : "*UNKNOWN*");
#endif

	auto pMemory = (BYTE*)lpTargetAddress;
	BYTE shellLdrLoad[5]				= { 0x55, 0x8B, 0xEC, 0x8D, 0x5 }; 
	BYTE shellManualMp[6]				= { 0x55, 0x8B, 0xEC, 0x51, 0x53, 0x8B }; 
	BYTE shellReflective[8]				= { 0x55, 0x89, 0xE5, 0x53, 0x83, 0xEC, 0x54, 0x8B }; 
	BYTE shellMLoad[8]					= { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x20, 0x53, 0x56 }; 
	BYTE shellhijack[10]				= { 0x68, 0xCC, 0xCC, 0xCC, 0xCC, 0x60, 0x9C, 0xBB, 0xCC, 0xCC }; 
	BYTE shellhijack2[10]				= { 0x60, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x81, 0xEB, 0x06 }; 
	BYTE shellhijack3[10]				= { 0x56, 0x8B, 0x35, 0x00, 0xC0, 0x27, 0x6A, 0x57, 0x8B, 0x3D }; 
	BYTE shellcreateremotethreadex[10]	= { 0xE8, 0x1D, 0x00, 0x00, 0x00, 0x50, 0x68, 0x58, 0x58, 0xC3 }; 
	BYTE shellcodeinjectrosdevil[8]		= { 0x68, 0xAC, 0xCE, 0xEA, 0xAC, 0x9C, 0x60, 0x68 }; 
	BYTE shellcodeLalakerAuto[8]		= { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x5D, 0xFF, 0x25 }; 

	// LdrLoadDll, LdrpLoadDll, ManualMap1
	if (memcmp(lpTargetAddress, &shellLdrLoad, 5) == 0)
	 	return LdrLoadShellCode1;

	// ManualMap2
	if (memcmp(lpTargetAddress, &shellManualMp, 6) == 0)
		return ManualMapShellCode1;

	// ManualMap3 ++
	if (*pMemory == 0x68 && *(pMemory + 5) == 0x68)
	{
		if (*(pMemory + 10) == 0xB8)
			return ManualMapShellCode2;
		else if (*(pMemory + 10) == 0x68)
			return ManualMapShellCode3;
	}

	// Reflective
	if (memcmp(lpTargetAddress, &shellReflective, sizeof(shellReflective)) == 0)
		return ReflectiveShellCode1;

	// Manual Load
	if (memcmp(lpTargetAddress, &shellMLoad, sizeof(shellMLoad)) == 0)
		return ManualLoadShellCode1;

	// Thread hijack 1
	if (memcmp(lpTargetAddress, &shellhijack, sizeof(shellhijack)) == 0)
		return ThreadHijackShellCode1;

	// Thread hijack 2
	if (memcmp(lpTargetAddress, &shellhijack2, sizeof(shellhijack2)) == 0)
		return ThreadHijackShellCode2;

	// Thread hijack 3
	if (memcmp(lpTargetAddress, &shellhijack3, sizeof(shellhijack3)) == 0)
		return ThreadHijackShellCode3;
	
	// Createremotethreadex 1
	if (memcmp(lpTargetAddress, &shellcreateremotethreadex, sizeof(shellcreateremotethreadex)) == 0)
		return CreateRemoteThreadExShellCode1;

	// Code injection 1
	if (memcmp(lpTargetAddress, &shellcodeinjectrosdevil, sizeof(shellcodeinjectrosdevil)) == 0)
		return CodeInjectionShellCode1;
	
	// Lalaker auto injector
	if (memcmp(lpTargetAddress, &shellcodeLalakerAuto, sizeof(shellcodeLalakerAuto)) == 0)
		return AutoInjectorLalakerShellCode;

	// Lalaker v110 external hack
	if (pMemory[0] == 0x68 && pMemory[1] == 0x0 && pMemory[2] == 0x0 && pMemory[5] == 0xe8 && pMemory[7] == 0xcd && pMemory[10] == 0x68)
		return LalakerMetin2HackV110;
	
	HOOK_LOG(LL_SYS, "Shellcode filter completed!");
	return ShellCodeNull;
}

bool CAnalyser::IsCallerAddressLegit(LPVOID lpCaller, int iType, const std::string & szName, LPDWORD pdwReturnCode)
{
	HOOK_LOG(LL_SYS, "Caller Address checker has been started! Caller: %p Type: %d Name: %s", lpCaller, iType, szName.c_str());

	auto pCurrentSecHdr			= (IMAGE_SECTION_HEADER*)nullptr;
	auto dwDetectedType			= 0UL;
	auto pbCaller				= (LPBYTE)lpCaller;

	auto dwLoadLibraryA			=	g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernel32,	xorstr("LoadLibraryA").crypt_get());
	auto dwLoadLibraryW			=	g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernel32,	xorstr("LoadLibraryW").crypt_get());
	auto dwLoadLibraryExA		=	g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernel32,	xorstr("LoadLibraryExA").crypt_get());
	auto dwLoadLibraryExW		=	g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernel32,	xorstr("LoadLibraryExW").crypt_get());
	auto dwFreeLibrary			=	g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernel32,	xorstr("FreeLibrary").crypt_get());

	auto dwLdrLoadDll			=	g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hNtdll,		xorstr("LdrLoadDll").crypt_get());
	auto dwLdrUnloadDll			=	g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hNtdll,		xorstr("LdrUnloadDll").crypt_get());

	//

	MODULEINFO user32ModInfo = { 0 };
	if (!g_winapiApiTable->GetModuleInformation(NtCurrentProcess, g_winapiModuleTable->hUser32, &user32ModInfo, sizeof(user32ModInfo)))
	{
		HOOK_LOG(LL_ERR, "GetModuleInformation fail! Last error: %u", g_winapiApiTable->GetLastError());
		dwDetectedType = HOOK_CHECK_GETMODULEINFO_FAIL;
		return false;
	}

	auto dwUser32Low	= (DWORD)user32ModInfo.lpBaseOfDll;
	auto dwUser32Hi		= (DWORD)user32ModInfo.lpBaseOfDll + user32ModInfo.SizeOfImage;

	auto dwPythonLow	= 0UL;
	auto dwPythonHi		= 0UL;
	if (g_nmApp->DataInstance()->GetAppType() == METIN2_GAME)
	{
		MODULEINFO pythonModInfo = { 0 };
		if (!g_winapiApiTable->GetModuleInformation(NtCurrentProcess, g_winapiModuleTable->hPython, &pythonModInfo, sizeof(pythonModInfo)))
		{
			HOOK_LOG(LL_ERR, "GetModuleInformation(python) fail! Last error: %u", g_winapiApiTable->GetLastError());
			dwDetectedType = 100002;
			return false;
		}

		dwPythonLow = (DWORD)pythonModInfo.lpBaseOfDll;
		dwPythonHi	= (DWORD)pythonModInfo.lpBaseOfDll + pythonModInfo.SizeOfImage;
	}

	MEMORY_BASIC_INFORMATION mbiCaller = { 0 };
	if (!g_winapiApiTable->VirtualQuery(lpCaller, &mbiCaller, sizeof(mbiCaller)))
	{
		HOOK_LOG(LL_ERR, "VirtualQuery fail! Last error: %u", g_winapiApiTable->GetLastError());
		dwDetectedType = HOOK_CHECK_VIRTUALQUERY_FAIL;
		return false;
	}

	char szFileName[2048] = { 0 };
	if (!g_winapiApiTable->GetMappedFileNameA(NtCurrentProcess, lpCaller, szFileName, sizeof(szFileName)))
	{
		HOOK_LOG(LL_ERR, "GetMappedFileNameA fail! Last error: %u", g_winapiApiTable->GetLastError());
//		dwDetectedType = HOOK_CHECK_GETMAPPEDFILENAME_FAIL;
//		return false;
	}

	HOOK_LOG(LL_SYS, "Caller Address checker completed! Address: %p Name: %s", lpCaller, szFileName ? szFileName : xorstr("***Unknown***").crypt_get());

	int iShellInjectionRet = FilterShellcode(lpCaller);
	if (iShellInjectionRet)
	{
		dwDetectedType = iShellInjectionRet;
		goto _complete;
	}

	if (lpCaller == dwLoadLibraryA)
	{
		dwDetectedType = EaxLoadLibraryA;
		goto _complete;
	}
	else if (lpCaller == dwLoadLibraryW)
	{
		dwDetectedType = EaxLoadLibraryW;
		goto _complete;
	}
	else if (lpCaller == dwLoadLibraryExA)
	{
		dwDetectedType = EaxLoadLibraryExA;
		goto _complete;
	}
	else if (lpCaller == dwLoadLibraryExW)
	{
		dwDetectedType = EaxLoadLibraryExW;
		goto _complete;
	}
	else if (lpCaller == dwFreeLibrary)
	{
		dwDetectedType = EaxFreeLibrary;
		goto _complete;
	}
	else if (lpCaller == dwLdrLoadDll)
	{
		dwDetectedType = EaxLdrLoadDll;
		goto _complete;
	}
	else if (lpCaller == dwLdrUnloadDll)
	{
		dwDetectedType = EaxLdrUnloadDll;
		goto _complete;
	}
	else if ((DWORD)lpCaller >= dwUser32Low && (DWORD)lpCaller <= dwUser32Hi)
	{
		dwDetectedType = EaxSetWindowHookEx;
		goto _complete;
	}
	else if (g_nmApp->DataInstance()->GetAppType() == METIN2_GAME && ((DWORD)lpCaller >= dwPythonLow && (DWORD)lpCaller <= dwPythonHi))
	{
		dwDetectedType = EaxPython; // Python module range
		goto _complete;
	}

	if (IsWindowsVistaOrGreater())
	{
		auto dwRtlUserThreadStart = g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hNtdll, xorstr("RtlUserThreadStart").crypt_get());
		if (lpCaller == dwRtlUserThreadStart)
		{
			dwDetectedType = EaxRtlUserThreadStart;
			goto _complete;
		}

		auto dwNtCreateThread = g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hNtdll, xorstr("NtCreateThread").crypt_get());
		if (lpCaller == dwNtCreateThread)
		{
			dwDetectedType = EaxNtCreateThread;
			goto _complete;
		}

		auto dwNtCreateThreadEx	= g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hNtdll, xorstr("NtCreateThreadEx").crypt_get());
		if (lpCaller == dwNtCreateThreadEx)
		{
			dwDetectedType = EaxNtCreateThreadEx;
			goto _complete;
		}

		auto dwRtlCreateUserThread = g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hNtdll, xorstr("RtlCreateUserThread").crypt_get());
		if (lpCaller == dwRtlCreateUserThread)
		{
			dwDetectedType = EaxRtlCreateUserThread;
			goto _complete;
		}
	}

	if (g_winapiModuleTable->hKernelbase)
	{
		auto dwLoadLibraryA_KBase = g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernelbase, xorstr("LoadLibraryA").crypt_get());
		if (dwLoadLibraryA_KBase && lpCaller == dwLoadLibraryA_KBase)
		{
			dwDetectedType = EaxLoadLibraryA_KBase;
			goto _complete;
		}

		auto dwLoadLibraryW_KBase = g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernelbase, xorstr("LoadLibraryW").crypt_get());
		if (dwLoadLibraryW_KBase && lpCaller == dwLoadLibraryW_KBase)
		{
			dwDetectedType = EaxLoadLibraryW_KBase;
			goto _complete;
		}

		auto dwLoadLibraryExA_KBase = g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernelbase, xorstr("LoadLibraryExA").crypt_get());
		if (dwLoadLibraryExA_KBase && lpCaller == dwLoadLibraryExA_KBase)
		{
			dwDetectedType = EaxLoadLibraryExA_KBase;
			goto _complete;
		}

		auto dwLoadLibraryExW_KBase = g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernelbase, xorstr("LoadLibraryExW").crypt_get());
		if (dwLoadLibraryExW_KBase && lpCaller == dwLoadLibraryExW_KBase)
		{
			dwDetectedType = EaxLoadLibraryExW_KBase;
			goto _complete;
		}
	}

	if (iType == CHECK_TYPE_LDRLOAD)
	{
		if (!szName.empty() && g_nmApp->DirFunctionsInstance()->IsFromWindowsPath(szName) == false)
		{
			std::string szWhiteList[] = {
				xorstr("wintrust.dll").crypt_get(), xorstr("d3d9").crypt_get(), xorstr("kernel32").crypt_get(), xorstr("windowscodecs.dll").crypt_get()
			};

			bool bIsWhitelisted = false;
			for (const auto & szCurrentItem : szWhiteList)
			{
				if (strstr(szName.c_str(), szCurrentItem.c_str())) {
					HOOK_LOG(LL_SYS, "Whitelisted module name: %s", szName.c_str());
					bIsWhitelisted = true;
				}
			}

			if (!bIsWhitelisted)
			{
				if (*(pbCaller - 7) == 0x1C)
				{
					dwDetectedType = EaxSetWindowHookEx2;
					goto _complete;
				}
			}
		}
	}
	else if (iType == CHECK_TYPE_THREAD)
	{
		if (mbiCaller.Type != MEM_IMAGE)
		{
			dwDetectedType = EaxCodeInjectionType;
			goto _complete;
		}

		else if (mbiCaller.State != MEM_COMMIT)
		{
			dwDetectedType = EaxUnknownState;
			goto _complete;
		}
	}

	if ( (mbiCaller.AllocationProtect	!= PAGE_EXECUTE	&& mbiCaller.AllocationProtect	!= PAGE_EXECUTE_READ && mbiCaller.AllocationProtect	!= PAGE_EXECUTE_WRITECOPY) &&
		 (mbiCaller.Protect				!= PAGE_EXECUTE	&& mbiCaller.Protect			!= PAGE_EXECUTE_READ && mbiCaller.Protect			!= PAGE_EXECUTE_WRITECOPY))
	{
		dwDetectedType = EaxBadAllocatedProtectType;
		goto _complete;
	} 

	if (mbiCaller.Protect & PAGE_EXECUTE_READWRITE || mbiCaller.Protect & PAGE_EXECUTE_WRITECOPY ||
		mbiCaller.AllocationProtect & PAGE_EXECUTE_READWRITE || mbiCaller.AllocationProtect & PAGE_READWRITE || mbiCaller.AllocationProtect & PAGE_WRITECOPY)
	{
		dwDetectedType = EaxBadProtectType;
		goto _complete;
	}

	if (IsWindowsVistaOrGreater())
	{
		PSAPI_WORKING_SET_EX_INFORMATION pworkingSetExInformation = { 0 };
		pworkingSetExInformation.VirtualAddress = lpCaller;

		if (FALSE == g_winapiApiTable->QueryWorkingSetEx(NtCurrentProcess, &pworkingSetExInformation, sizeof(pworkingSetExInformation)))
		{
			dwDetectedType = QueryWorkingSetExFail;
			goto _complete;
		}

		if (!pworkingSetExInformation.VirtualAttributes.Valid)
		{
			dwDetectedType = QueryWorkingSetExNotValid;
			goto _complete;
		}
	}

	if (szFileName && g_nmApp->DataInstance()->GetAppType() == METIN2_GAME && iType == CHECK_TYPE_THREAD)
	{
		auto szLowerFileName	= g_nmApp->FunctionsInstance()->szLower(szFileName);

		auto szExecutablePath	= g_nmApp->DirFunctionsInstance()->ExeNameWithPath();
		auto szLowerExecutable	= g_nmApp->FunctionsInstance()->szLower(szFileName);

		if (szLowerFileName == szLowerExecutable)
		{
			dwDetectedType = EaxMainProcess;
			goto _complete;
		}

		if (szLowerFileName.empty() && mbiCaller.Type == MEM_PRIVATE && mbiCaller.RegionSize == 0x1000)
		{
			dwDetectedType = EaxMappedCode;
			goto _complete;
		}

		if (szLowerFileName.empty() && mbiCaller.State == 0x1000)
		{
			dwDetectedType = EaxMappedCode2;
			goto _complete;
		}
	}

	pCurrentSecHdr = (IMAGE_SECTION_HEADER*)lpCaller;
	if (pCurrentSecHdr && !pCurrentSecHdr->Characteristics)
	{
		dwDetectedType = NullCharacteristics;
		goto _complete;
	}

_complete:
	if (dwDetectedType)
	{
		HOOK_LOG(LL_ERR, "Detection triggered with: %u code.", dwDetectedType);
		if (pdwReturnCode) *pdwReturnCode = dwDetectedType;
		return false;
	}

	HOOK_LOG(LL_SYS, "Caller Address checker passed! Address: %p", lpCaller);
	return true;
}

