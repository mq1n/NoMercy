#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Analyser.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "ThreadFunctions.h"
#include "ThreadHelper.h"

static auto s_bConnectChecked = false;

// TODO: debug logs

bool CAnalyser::OnThreadCreated(DWORD dwThreadID, LPVOID lpStartAddress, LPVOID lpEIP, LPDWORD pdwErrorType)
{
#if 0
	// Check module owner name with file scan wrapper
	// Start address + mbi.regionsize section scan wrapper
	// Check start address is in text section range
	// Check eip register is in text section range
	// check mbi.type, is it mem_image or what
	// check priority
	// enum modules check thread's module, if is not pointed to any module range, kill
	// check stack
	// teb -> tib -> ArbitraryUserPointer -> file analyse
#endif

	HOOK_LOG(LL_SYS, "Thread check started! TID: %u Start address: %p", dwThreadID, lpStartAddress);

	if (g_nmApp->ThreadManagerInstance()->GetThreadFromId(dwThreadID))
		return true;

	auto pThread = std::make_unique<CThread>(dwThreadID);
	if (!IS_VALID_SMART_PTR(pThread)|| !pThread->IsCorrect())
	{
		if (pdwErrorType) *pdwErrorType = (DWORD)-1;
		return false;
	}

	auto _pStartAddress = lpStartAddress;
	if (!_pStartAddress)
		_pStartAddress = (LPVOID)pThread->GetStartAddress();

	auto _pEIP = (DWORD)lpEIP;
	if (!_pEIP)
	{
		auto pContext = pThread->GetContext();
		if (pContext && pContext.get())
		{
#ifndef _M_X64
			_pEIP = pContext->Eip;
#else
			_pEIP = pContext->Rip;
#endif
		}
	}

	if (!_pStartAddress)
	{
		if (pdwErrorType) *pdwErrorType = 100;

		auto szTempName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(_pStartAddress);
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szTempName.c_str());

		return false;
	}

	auto dwReturnCode = 0UL;
	auto bIsCallerLegit = IsCallerAddressLegit(_pStartAddress, CHECK_TYPE_THREAD, "", &dwReturnCode);
	if (!bIsCallerLegit)
	{
		if (pdwErrorType) *pdwErrorType = dwReturnCode;

		auto szTempName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(_pStartAddress);
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szTempName.c_str());

		return false;
	}

	if (pThread->IsItAlive() == false)
		return true;

	auto dwStartAddress = pThread->GetStartAddress(); 
	if (dwStartAddress != (DWORD)_pStartAddress)  // spoofed address?
	{
		if (pdwErrorType) *pdwErrorType = 101;

		auto szTempName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(_pStartAddress);
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szTempName.c_str());

		return false;
	}

	if (pThread->IsRemoteThread()) 
	{
		if (pdwErrorType) *pdwErrorType = 102;

		auto szTempName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(_pStartAddress);
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szTempName.c_str());

		return false;
	}

	if (pThread->HasSuspend()) 
	{
		if (pdwErrorType) *pdwErrorType = 103;

		auto szTempName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(_pStartAddress);
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szTempName.c_str());

		return false;
	}

	if (g_nmApp->DynamicWinapiInstance()->IsLoadedAddress(dwStartAddress)) // unlinked or mmapped
	{ 
		if (pdwErrorType) *pdwErrorType = 104;

		auto szTempName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(_pStartAddress);
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szTempName.c_str());

		return false;
	}

	auto dwDbgUiRemoteBreakin = (DWORD)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("DbgUiRemoteBreakin").crypt_get());
	if (_pEIP && _pEIP == dwDbgUiRemoteBreakin && !dwStartAddress) 
	{
		if (pdwErrorType) *pdwErrorType = 105;

		auto szTempName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(_pStartAddress);
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szTempName.c_str());

		return false;
	}
		
	if (pThread->HasDebugRegisters())
		pThread->ClearDebugRegisters();

	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (g_winapiApiTable->VirtualQuery(_pStartAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.AllocationBase && mbi.AllocationBase == _pStartAddress) /* If thread started in page's allocated base aka. from page EP */
		{
			if (pdwErrorType) *pdwErrorType = 106;

			auto szTempName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(_pStartAddress);
			Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szTempName.c_str());

			return false;
		}

#if 0
		HMODULE hOwner = nullptr;
		g_winapiApiTable->GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)dwStartAddress, &hOwner);
		if (!hOwner && mbi.Type == MEM_IMAGE)
		{
			SSectionEnumerateInfos sectionEnumerateInfos;
			sectionEnumerateInfos.dwBase			= (DWORD)mbi.BaseAddress;
			sectionEnumerateInfos.dwAllocationBase	= (DWORD)mbi.AllocationBase;
			sectionEnumerateInfos.ulSize			= mbi.RegionSize;
			sectionEnumerateInfos.dwProtect			= mbi.Protect;
			sectionEnumerateInfos.dwState			= mbi.State;
			sectionEnumerateInfos.dwType			= mbi.Type;
			sectionEnumerateInfos.bCanAccess		= true;

			ScanSection(&sectionEnumerateInfos);
		}
#endif
	}
	else
	{
		// TODO: block it, non accessable or what? 
	}

	auto szModuleName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(reinterpret_cast<LPVOID>(dwStartAddress));
	if (szModuleName.empty())
	{
		if (pdwErrorType) *pdwErrorType = 107;
		return false;
	}
	else
	{
		// TODO: Check thread owner module path
		auto szLowerModuleName = g_nmApp->FunctionsInstance()->szLower(szModuleName);

		auto szAntiFilename = g_nmApp->DataInstance()->GetAntiFileName();
		auto szLowerAntiFilename = g_nmApp->FunctionsInstance()->szLower(szAntiFilename);

		auto hOwnerModule = g_winapiApiTable->GetModuleHandleA_o(szLowerModuleName.c_str());
		if (!hOwnerModule)
		{
			if (!strstr(szLowerModuleName.c_str(), szLowerAntiFilename.c_str()))
			{
				if (pdwErrorType) *pdwErrorType = 108;
				Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szLowerModuleName.c_str());
				return false;
			}
		}

		MODULEINFO currentModInfo = { 0 };
		if (g_winapiApiTable->GetModuleInformation(NtCurrentProcess, hOwnerModule, &currentModInfo, sizeof(currentModInfo)))
		{
			auto dwModuleLow	= (DWORD)currentModInfo.lpBaseOfDll;
			auto dwModuleHi		= (DWORD)currentModInfo.lpBaseOfDll + currentModInfo.SizeOfImage;

			if (dwStartAddress < dwModuleLow || dwStartAddress > dwModuleHi) // out of module area
			{
				if (!strstr(szLowerModuleName.c_str(), szLowerAntiFilename.c_str()))
				{
					if (pdwErrorType) *pdwErrorType = 109;
					Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n").crypt_get(), szLowerModuleName.c_str());
					return false;
				}
			}

#if 0
			// main threadde tetikleniyor, eipin modülünü loglayýp whitelist oluþtur
			auto szEIPModuleName = g_nmApp->FunctionsInstance()->GetModuleOwnerName(_pEIP);
			if (_pEIP && (_pEIP < dwModuleLow || _pEIP > dwModuleHi))
			{
				if (pdwErrorType) *pdwErrorType = 110;
				Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n"), szEIPModuleName.c_str());
				return false;
			}
#endif
		}

#if 0
		if (!strstr(szLowerModuleName.c_str(), szLowerAntiFilename.c_str()))
		{
			std::wstring wszModuleName(szModuleName.begin(), szModuleName.end());

			if (!g_nmApp->ScannerInstance()->IsFileDigitalSigned(wszModuleName.c_str()))
			{
				if (pdwErrorType) *pdwErrorType = 111;
				Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n"), szLowerModuleName.c_str());
				return false;
			}

			if (!g_nmApp->ScannerInstance()->isValidFileCertificate(wszModuleName.c_str()))
			{
				if (pdwErrorType) *pdwErrorType = 112;
				Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal thread owner: %s\n"), szLowerModuleName.c_str());
				return false;
			}
		}
#endif
	}

	if (pThread->HasDebugRegisters()) // already cleaned but still exist
	{
		if (pdwErrorType) *pdwErrorType = 113;
		return false;
	}

	// TODO: Context scan

	return true;
}

bool CAnalyser::OnModuleLoaded(const std::string & szName, DWORD dwCurrentTID, int iCheckType, LPDWORD pdwErrorType)
{
	HOOK_LOG(LL_SYS, "Module check started! Name: %s TID: %u", szName.c_str(), dwCurrentTID);

	// todo: At the first, check with whitelist (file sha & cert name sha) if whitelisted directly return true

	// name
	// hash
	// pe header

	// Name
	auto szLowerName = g_nmApp->FunctionsInstance()->szLower(szName);
	if (iCheckType == CHECK_TYPE_LDRLOAD && szLowerName.empty()) 
	{
		if (pdwErrorType) *pdwErrorType = (DWORD)-1;
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal module owner: %s\n").crypt_get(), szName.c_str());
		return false;
	}

	// Current thread
	auto pThread = std::make_unique<CThread>(dwCurrentTID);
	if (!pThread || !pThread.get() || !pThread->IsCorrect()) 
	{
		if (pdwErrorType) *pdwErrorType = (DWORD)-2;
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal module owner: %s\n").crypt_get(), szName.c_str());
		return false;
	}

	// If in the source string have a main process name pass it (windows xp and vista generic problem).
	auto szExeNameWithPath = g_nmApp->DirFunctionsInstance()->ExeNameWithPath();
	auto szLowerExecutable = g_nmApp->FunctionsInstance()->szLower(szExeNameWithPath);
	if (strstr(szLowerName.c_str(), szLowerExecutable.c_str()))
		return true;

	auto dwReturnCode = 0UL;
	auto bIsCallerLegit = IsCallerAddressLegit((LPVOID)pThread->GetStartAddress(), iCheckType, szLowerName, &dwReturnCode);
	if (!bIsCallerLegit) 
	{
		if (pdwErrorType) *pdwErrorType = dwReturnCode;
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal module owner: %s\n").crypt_get(), szName.c_str());
		return false;
	}

	// Module
	if (iCheckType == CHECK_TYPE_LDRLOAD)
	{
		if (szLowerName.find("\\", 0) != std::string::npos) // If included path in module name
		{
			if (!g_nmApp->DirFunctionsInstance()->IsFromWindowsPath(szLowerName) && !g_nmApp->DirFunctionsInstance()->IsFromCurrentPath(szLowerName))
			{
				if (!strstr(szLowerName.c_str(), xorstr("microsoft visual studio").crypt_get()) && !strstr(szLowerName.c_str(), xorstr("nvscpapi.dll").crypt_get()))
				{
					HOOK_LOG(LL_ERR, "Module: '%s' Is not from windows or current path", szLowerName.c_str());

					if (pdwErrorType) *pdwErrorType = 201;
					Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal module owner: %s\n").crypt_get(), szName.c_str());
					return false;
				}
			}
		}
	}

	auto szPath = g_nmApp->DirFunctionsInstance()->ExePath();
	auto szLowerExePath = g_nmApp->FunctionsInstance()->szLower(szPath);

	if (g_nmApp->DirFunctionsInstance()->IsFileExist(szLowerName))
	{
		std::wstring wszName(szLowerName.begin(), szLowerName.end());

#if 0
		if (!g_nmApp->ScannerInstance()->IsFileDigitalSigned(wszName.c_str()))
		{
			// TODO: Create whitelist, set whitelistedflag=true
			if (pdwErrorType) *pdwErrorType = 202;
			Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal module owner: %s\n"), szName.c_str());
			return false;
		}

		if (!whitelistedlag && !g_nmApp->ScannerInstance()->isValidFileCertificate(wszName.c_str()))
		{
			if (pdwErrorType) *pdwErrorType = 203;
			Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal module owner: %s\n"), szName.c_str());
			return false;
		}
#endif

		// If in the source string have a anticheat module name pass it.
		if (strstr(szLowerName.c_str(), szLowerExePath.c_str()))
		{
			auto szAntiFilename = g_nmApp->DataInstance()->GetAntiFileName();
			auto szLowerAntiFilename = g_nmApp->FunctionsInstance()->szLower(szAntiFilename);

			if (!strstr(szLowerName.c_str(), szLowerAntiFilename.c_str()))
			{
				if (!strstr(szLowerName.c_str(), xorstr(".pyd").crypt_get()) && !strstr(szLowerName.c_str(), xorstr(".mix").crypt_get()) &&
					!strstr(szLowerName.c_str(), xorstr(".m3d").crypt_get()) && !strstr(szLowerName.c_str(), xorstr(".flt").crypt_get()) &&
					!strstr(szLowerName.c_str(), xorstr(".asi").crypt_get()))
				{
					if (strcmp(szLowerName.c_str(), szLowerExecutable.c_str())) // isn't main executable
					{
						if (!g_nmApp->ScannerInstance()->isValidFileCertificate(wszName.c_str()))
						{
							if (!strstr(szLowerName.c_str(), xorstr("speedtreert.dll").crypt_get()))
							{
								if (pdwErrorType) *pdwErrorType = 204;
								Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal module owner: %s\n").crypt_get(), szName.c_str());
								return false;
							}
						}
					}
				}
			}
		}
		// todo: wrap to file analyser
	}
	else
	{
		// not exist file?
	}
	return true;
}

bool CAnalyser::OnModuleRequested(const std::string & szName, LPDWORD pdwErrorType)
{
	auto szAntiFilename			= g_nmApp->DataInstance()->GetAntiFileName();
	auto szLowerAntiFilename	= g_nmApp->FunctionsInstance()->szLower(szAntiFilename);

	auto bRet = true;

	if (strstr(szName.c_str(), xorstr("python2").crypt_get()))
	{
		if (pdwErrorType) *pdwErrorType = 1;
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal module request owner: %s\n").crypt_get(), szName.c_str());
		bRet = false;
	}
	else if (strstr(szName.c_str(), szLowerAntiFilename.c_str())) 
	{
		if (pdwErrorType) *pdwErrorType = 2;
		Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal module request owner: %s\n").crypt_get(), szName.c_str());
		bRet = false;
	}

	if (bRet == false) {
		HOOK_LOG(LL_ERR, "LdrGetDllHandleEx Module handle request blocked! Module name: %s", szName.c_str());
	}
	return bRet;
}

bool CAnalyser::OnSectionMapped(DWORD dwBase, LPVOID lpArbitraryUserPointer, LPDWORD pdwErrorType)
{
#if 0
	MEMORY_BASIC_INFORMATION mbi;
	if (g_winapiApiTable->VirtualQuery((LPCVOID)dwBase, &mbi, sizeof(mbi)))
	{
		SSectionEnumerateInfos sectionScanInfos;

		sectionScanInfos.dwBase				= dwBase;
		sectionScanInfos.dwAllocationBase	= (DWORD)mbi.AllocationBase;
		sectionScanInfos.ulSize				= mbi.RegionSize;
		sectionScanInfos.dwState			= mbi.State;
		sectionScanInfos.dwProtect			= mbi.Protect;
		sectionScanInfos.dwType				= mbi.Type;
		sectionScanInfos.bCanAccess			= (mbi.State & MEM_COMMIT) && !(mbi.Protect & PAGE_NOACCESS) && !(mbi.Protect & PAGE_GUARD);

		g_nmApp->ScannerInstance()->ScanSection(&sectionScanInfos);
	}
#endif

	if (!lpArbitraryUserPointer)
		return true;

	std::wstring wszPath {
		static_cast<PCWSTR>(lpArbitraryUserPointer)
	};
	auto szPath		 = std::string(wszPath.begin(), wszPath.end());
	auto szLowerPath = g_nmApp->FunctionsInstance()->szLower(szPath);

	HOOK_LOG(LL_SYS, "NtMapViewOfSection arbitrary_user_pointer detected! Path: %s", szPath.c_str());

	if (szLowerPath.empty() == false)
	{
		if (szLowerPath.find("\\", 0) != std::string::npos) // If included path in module name
		{
			if (!g_nmApp->DirFunctionsInstance()->IsFromWindowsPath(szLowerPath) && !g_nmApp->DirFunctionsInstance()->IsFromCurrentPath(szLowerPath))
			{
				if (!strstr(szLowerPath.c_str(), xorstr(".pyd").crypt_get()) && !strstr(szLowerPath.c_str(), xorstr(".mix").crypt_get()) &&
					!strstr(szLowerPath.c_str(), xorstr(".m3d").crypt_get()) && !strstr(szLowerPath.c_str(), xorstr(".flt").crypt_get()) &&
					!strstr(szLowerPath.c_str(), xorstr(".asi").crypt_get()))
				{
					// todo: name whitelist microsoft visual studio, windows live, nahimic, idmkb, tv_w32, filter, pancafe, nvidia, tiptsf, btmmhook, fraps, bandicam

					if (!g_nmApp->DirFunctionsInstance()->IsFileExist(szLowerPath))
					{
						if (pdwErrorType) *pdwErrorType = 301;
						Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal memory block owner: %s\n").crypt_get(), szLowerPath.c_str());
						return false;
					}

					if (!g_nmApp->ScannerInstance()->IsFileDigitalSigned(wszPath.c_str()))
					{
						if (pdwErrorType) *pdwErrorType = 302;
						Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal memory block owner: %s\n").crypt_get(), szLowerPath.c_str());
						return false;
					}
					else if (!g_nmApp->ScannerInstance()->isValidFileCertificate(wszPath.c_str()))
					{
						if (pdwErrorType) *pdwErrorType = 303;
						Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal memory block owner: %s\n").crypt_get(), szLowerPath.c_str());
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool CAnalyser::OnExceptionThrowed(PEXCEPTION_RECORD ExceptionInfo, LPDWORD pdwErrorType)
{
	// HOOK_LOG(LL_SYS, "Exception detected!");

	if (ExceptionInfo && ExceptionInfo->ExceptionRecord)
	{
		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
		{
			if (pdwErrorType) *pdwErrorType = 401;
			return false;
		}

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_GUARD_PAGE)
		{
			if (pdwErrorType) *pdwErrorType = 402;
			return false;
		}

		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
		{
			if (pdwErrorType) *pdwErrorType = 403;
			return false;
		}
	}

	return true;
}

bool CAnalyser::OnConnected(const std::string & szTargetAddress, LPDWORD pdwErrorType)
{
	HOOK_LOG(LL_SYS, "Connection detected! Target: %s", szTargetAddress.c_str());

#ifdef BLOCK_CONNECTIONS
	return false;
#endif

#if 0
	CHAR __localhost[] = { '1', '2', '7', '.', '0', '.', '0', '.', '1', 0x0 }; // crashreport pls
	if (false == LPInternetAPI->IsLicensedIp(c_szConnectedTo) && strcmp(c_szConnectedTo, __localhost))
	{
		LPLog->DetourLog(0, "Unknown Connection: %s blocked!", c_szConnectedTo);
		return 0;
	}

#endif
	return true;
}

bool CAnalyser::OnWndProcHooked(HWND hWnd, int nIndex, LONG dwNewLong, LPDWORD pdwErrorType)
{
	if (dwNewLong)
	{
		HOOK_LOG(LL_SYS, "SetWindowLong called for: %p Index: %d NewValue: %p", hWnd, nIndex, dwNewLong);

		if (nIndex == GWL_WNDPROC && g_nmApp->WatchdogInstance()->IsWatchdogWindow(hWnd)) 
		{
			char szFileName[2048] = { 0 };
			if (g_winapiApiTable->GetMappedFileNameA(NtCurrentProcess, (LPVOID)dwNewLong, szFileName, sizeof(szFileName) && strlen(szFileName)))
			{
				HOOK_LOG(LL_SYS, "NewValue Owner: %s", szFileName);

				auto szLowerFilename = g_nmApp->FunctionsInstance()->szLower(szFileName);

				auto szAntiFilename			= g_nmApp->DataInstance()->GetAntiFileName();
				auto szLowerAntiFilename	= g_nmApp->FunctionsInstance()->szLower(szAntiFilename);

				if (!strstr(szLowerFilename.c_str(), szLowerAntiFilename.c_str()))
				{
					if (pdwErrorType) *pdwErrorType = 1;
					Logf(CUSTOM_LOG_FILENAME, xorstr("Illegal window owner: %s\n").crypt_get(), szFileName);
					return false;
				}
			}
		}
	}

	return true;
}

