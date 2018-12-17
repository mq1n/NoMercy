#include "NM_Index.h"
#include "NM_Main.h"
#include "AntiDebug.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "ProcessFunctions.h"
#include "SafeProcessHandle.h"

inline bool GetExplorerPIDbyProgman(LPDWORD pdwExplorerPID)
{
	auto hWnd = g_winapiApiTable->FindWindowExA(NULL, NULL, xorstr("Progman").crypt_get(), NULL);
	if (!hWnd)
		return false;

	auto dwPID = 0UL;
	auto dwTID = g_winapiApiTable->GetWindowThreadProcessId(hWnd, &dwPID);
	if (!dwTID || !dwPID)
		return false;

	*pdwExplorerPID = dwPID;
	return true;
}

inline bool GetExplorerPIDbyShellWindow(LPDWORD pdwExplorerPID)
{
	auto hWnd = g_winapiApiTable->GetShellWindow();
	if (!hWnd)
		return false;

	auto dwPID = 0UL;
	auto dwTID = g_winapiApiTable->GetWindowThreadProcessId(hWnd, &dwPID);
	if (!dwTID || !dwPID)
		return false;

	*pdwExplorerPID = dwPID;
	return true;
}

inline bool ParentOfParentPIDIsLegit(HANDLE hOwnProcess, DWORD dwProcessId)
{
	// Process informations
	auto dwParentPid = CProcessFunctions::GetParentProcessIdNative(hOwnProcess);
	if (!dwParentPid) 
	{
		DEBUG_LOG(LL_ERR, "Parent of parent pid is null! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto processEnumerator	= std::make_unique<CSafeProcessHandle>();
	auto hProcess			= processEnumerator->FindProcessFromPID(dwParentPid);
	if (!IS_VALID_HANDLE(hProcess)) 
	{
		DEBUG_LOG(LL_ERR, "Parent of parent process not found on process list! PID: %u", dwParentPid);

		hProcess = g_winapiApiTable->OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwParentPid);
		if (!IS_VALID_HANDLE(hProcess)) 
		{
			auto bIsAlive = CProcessFunctions::ProcessIsItAlive(dwParentPid);
			DEBUG_LOG(LL_ERR, "Parent of parent process can not open! Last error: %u IsAlive: %d", g_winapiApiTable->GetLastError(), bIsAlive);
			return false;
		}
	}

	auto szParentOfParentName = CProcessFunctions::GetProcessName(hProcess);
	if (szParentOfParentName.empty()) 
	{
		DEBUG_LOG(LL_ERR, "Parent of parent process name not found!");
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
		return false;
	}
	auto szLowerParentOfParentName		= g_nmApp->FunctionsInstance()->szLower(szParentOfParentName);
	auto szParentOfParentProcessPath	= g_nmApp->DirFunctionsInstance()->GetPathFromProcessName(szLowerParentOfParentName);
	auto szParentOfParentProcessName	= g_nmApp->DirFunctionsInstance()->GetNameFromPath(szLowerParentOfParentName);
	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);

	DEBUG_LOG(LL_SYS, "Parent of parent process: %s(%u)", szLowerParentOfParentName.c_str(), dwParentPid);

	// Windows informations
	auto szWindowsPath		= g_nmApp->DirFunctionsInstance()->WinPath();
	auto szLowerWindowsPath = g_nmApp->FunctionsInstance()->szLower(szWindowsPath);

	if (szLowerWindowsPath.empty())
	{
		DEBUG_LOG(LL_ERR, "Windows path not found!");
		return false;
	}

	auto dwExplorerPID = 0UL;
	if (!GetExplorerPIDbyShellWindow(&dwExplorerPID)) 
	{
		DEBUG_LOG(LL_CRI, "Explorer.exe pid not found! Last Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Explorer pid: %u", dwExplorerPID);

	// TODO: Compare explorer pid with GetExplorerPIDbyProgman

	if (szParentOfParentProcessPath != szLowerWindowsPath ||
		strcmp(szParentOfParentProcessName.c_str(), xorstr("explorer.exe").crypt_get()) ||
		dwExplorerPID != dwParentPid)
	{
		DEBUG_LOG(LL_CRI, "Parent process is not from windows or is not explorer.exe or restarted explorer.exe!");
		return false;
	}

	return true;
}


bool CAntiDebug::ParentCheck(const char* c_szPatcherName)
{
	std::string szLowerPatcherName(c_szPatcherName, strnlen(c_szPatcherName, MAX_PATH));
	std::transform(szLowerPatcherName.begin(), szLowerPatcherName.end(), szLowerPatcherName.begin(), tolower);
	DEBUG_LOG(LL_SYS, "Patcher Name: %s", szLowerPatcherName.c_str());

	// Process informations
	STARTUPINFOA si	= { 0 };
	si.cb			= sizeof(si);;
	g_winapiApiTable->GetStartupInfoA(&si);

	auto dwCurrentPID		= g_winapiApiTable->GetCurrentProcessId();
	auto dwParentPID		= CProcessFunctions::GetProcessParentProcessId(dwCurrentPID);
	auto dwParentPIDFromPEB = CProcessFunctions::GetParentProcessIdNative(NtCurrentProcess);
	DEBUG_LOG(LL_SYS, "Current PID: %u Parent PID: %u/%u", dwCurrentPID, dwParentPID, dwParentPIDFromPEB);

	if (dwParentPID != dwParentPIDFromPEB) { /* Anti Parent pid manipulation */
		DEBUG_LOG(LL_ERR, "Parent PID manipulation detected! IsVista/+: %d Parent pid: %u Parent pid PEB: %u", IsWindowsVistaOrGreater(), dwParentPID, dwParentPIDFromPEB);
		return false;
	}

	auto processEnumerator = std::make_unique<CSafeProcessHandle>();
	if (!processEnumerator || !processEnumerator.get()) {
		DEBUG_LOG(LL_ERR, "processEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto hProcess = processEnumerator->FindProcessFromPID(dwParentPID);
	if (!IS_VALID_HANDLE(hProcess)) 
	{
		DEBUG_LOG(LL_ERR, "Parent process not found on process list! PID: %u", dwParentPID);
		return true; 
#if 0
		hProcess = g_winapiApiTable->OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwParentPID);
		if (!IS_VALID_HANDLE(hProcess)) {
			auto bIsAlive = CProcessFunctions::ProcessIsItAlive(dwParentPID);
			DEBUG_LOG(LL_ERR, "Parent process can not open! Last error: %u IsAlive: %d", g_winapiApiTable->GetLastError(), bIsAlive);
			return false;
		}
#endif
	}

	auto szParentName = CProcessFunctions::GetProcessName(hProcess);
	if (szParentName.empty()) {
		DEBUG_LOG(LL_ERR, "Parent process name not found!");
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
		return false;
	}
	auto szLowerParentName		= g_nmApp->FunctionsInstance()->szLower(szParentName);
	auto szParentProcessPath	= g_nmApp->DirFunctionsInstance()->GetPathFromProcessName(szLowerParentName);
	auto szParentProcessName	= g_nmApp->DirFunctionsInstance()->GetNameFromPath(szLowerParentName);
	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);

	DEBUG_LOG(LL_SYS, "Parent process: %s(%u)", szLowerParentName.c_str(), dwParentPID);

	// Windows informations
	auto szWindowsPath		= g_nmApp->DirFunctionsInstance()->WinPath();
	auto szLowerWindowsPath = g_nmApp->FunctionsInstance()->szLower(szWindowsPath);

	if (szLowerWindowsPath.empty()) {
		DEBUG_LOG(LL_ERR, "Windows path not found!");
		return false;
	}

	auto dwExplorerPID = 0UL;
	if (!GetExplorerPIDbyShellWindow(&dwExplorerPID)) {
		DEBUG_LOG(LL_CRI, "Explorer.exe pid not found! Last Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "Explorer pid: %u", dwExplorerPID);

	// TODO: Compare explorer pid with GetExplorerPIDbyProgman

	// Have not patcher, so executed from directly
	if (szLowerPatcherName.empty())
	{
#ifdef _DEBUG
		if (!strcmp(szParentProcessName.c_str(), xorstr("devenv.exe").crypt_get()))
			return true;
#endif

		if (szParentProcessPath != szLowerWindowsPath ||
			strcmp(szParentProcessName.c_str(), xorstr("explorer.exe").crypt_get()) ||
			dwExplorerPID != dwParentPID)
		{
			DEBUG_LOG(LL_CRI, "Parent process is not from windows or is not explorer.exe or restarted explorer.exe!");
			return false;
		}

#if 0
		auto wszParentName = std::wstring(szLowerParentName.begin(), szLowerParentName.end());
		if (!g_nmApp->ScannerInstance()->IsFileDigitalSigned(wszParentName.c_str()))
		{
			DEBUG_LOG(LL_CRI, "Parent process is not a digital signed file!");
			return false;
		}

		if (!g_nmApp->ScannerInstance()->isValidFileCertificate(wszParentName.c_str()))
		{
			DEBUG_LOG(LL_CRI, "Parent process digital signature is not validated!");
			return false;
		}
#endif

#if 0
		// check startup params
		STARTUPINFO StartupInfo = { 0 };

		GetStartupInfo(&StartupInfo);

		if (StartupInfo.dwX != 0
			|| StartupInfo.dwY != 0
			|| StartupInfo.dwXCountChars != 0
			|| StartupInfo.dwYCountChars != 0
			|| StartupInfo.dwFillAttribute != 0
			|| StartupInfo.dwXSize != 0
			|| StartupInfo.dwYSize != 0)
		{

		}

		SYSTEMTIME sysTimeStart;
		SYSTEMTIME sysTimeEnd;
		FILETIME timeStart, timeEnd;

		GetSystemTime(&sysTimeStart);
		
		Sleep(1000); // TODO: change with cronos

		GetSystemTime(&sysTimeEnd);
		SystemTimeToFileTime(&sysTimeStart, &timeStart);
		SystemTimeToFileTime(&sysTimeEnd, &timeEnd);
		double timeExecution = (timeEnd.dwLowDateTime - timeStart.dwLowDateTime) / 10000.0;
		if (timeExecution >= g_doSmthExecutionTime)
		{

		}
#endif

	}
	else // Executed from a patcher
	{
		auto szExePath		= g_nmApp->DirFunctionsInstance()->ExePath();
		auto szLowerExePath = g_nmApp->FunctionsInstance()->szLower(szExePath);;
		DEBUG_LOG(LL_SYS, "Exe path: %s Parent path: %s Patcher name: %s Parent name: %s",
			szLowerExePath.c_str(), szParentProcessPath.c_str(), szLowerPatcherName.c_str(), szParentProcessName.c_str());

		if (szLowerExePath		!= szParentProcessPath ||
			szLowerPatcherName  != szParentProcessName)
		{
			DEBUG_LOG(LL_CRI, "Parent process is not from same path with main process or launched from different process!");
			return false;
		}

#if 0
		// Kapalý uygulamanýn parenti alýnmýyor
		if (!ParentOfParentPIDIsLegit(hProcess, dwParentPID))
			return false;
#endif

		// TODO: check patcher md5 and verify

		/* If parent process(autopatcher or launcher) is still alive, kill it */
		if (CProcessFunctions::ProcessIsItAlive(dwParentPID))
		{
			auto hParentProcess = g_winapiApiTable->OpenProcess(PROCESS_TERMINATE, FALSE, dwParentPID);
			if (IS_VALID_HANDLE(hParentProcess))
				g_winapiApiTable->NtTerminateProcess(hParentProcess, EXIT_SUCCESS);
			else
				g_winapiApiTable->WinStationTerminateProcess(NULL, dwParentPID, DBG_TERMINATE_PROCESS);
		}
	}

	return true;
}

