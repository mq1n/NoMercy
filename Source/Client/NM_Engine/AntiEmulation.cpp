#include "NM_Index.h"
#include "NM_Main.h"
#include "AntiEmulation.h"
#include "DynamicWinapi.h"
#include "Functions.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "Timer.h"

#pragma region GeneralEmulationCheck
__forceinline bool RandomApiCheck()
{
	auto uTime = (unsigned int)time(NULL);
	srand(uTime);

	auto szFakeFunctionName = g_nmApp->FunctionsInstance()->GetRandomString(8);
	if (g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernel32, szFakeFunctionName.c_str()))
	{
		return false;
	}
	return true;
}

__forceinline bool CheckErrorMode()
{
	auto dwRealCode = g_winapiApiTable->SetErrorMode(0);
	auto dwCode = 1024UL;

	g_winapiApiTable->SetErrorMode(1024);
	if (g_winapiApiTable->SetErrorMode(0) != 1024)
	{
		return false;
	}

	if ((dwCode + 313) != 1337)
	{
		return false;
	}

	g_winapiApiTable->SetErrorMode(dwRealCode);
	return true;
}

__forceinline bool LoadNtOsKrnl()
{
	if (IsWindows8OrGreater() == true)
		return true;

	CHAR __ntoskrnlexe[] = { 'n', 't', 'o', 's', 'k', 'r', 'n', 'l', '.', 'e', 'x', 'e', 0x0 }; // ntoskrnl.exe

	auto hProc = g_winapiApiTable->LoadLibraryA(__ntoskrnlexe);
	if (!hProc)
	{
		return false;
	}
	
	g_winapiApiTable->FreeLibrary(hProc);
	return true;
}


bool CheckRegistry_SandboxProductIDs(LPDWORD pdwErrorCode)
{
	char RegKey[_MAX_PATH] = { 0 };
	DWORD BufSize = _MAX_PATH;
	DWORD dataType = REG_SZ;

	HKEY hKey;
	CHAR __regpath[] = { 'S', 'o', 'f', 't', 'w', 'a', 'r', 'e', '\\', 'M', 'i', 'c', 'r', 'o', 's', 'o', 'f', 't', '\\', 'W', 'i', 'n', 'd', 'o', 'w', 's', ' ', 'N', 'T', '\\', 'C', 'u', 'r', 'r', 'e', 'n', 't', 'V', 'e', 'r', 's', 'i', 'o', 'n', '\\', 'P', 'r', 'o', 'd', 'u', 'c', 't', 'I', 'D', 0x0 }; // Software\Microsoft\Windows NT\CurrentVersion\ProductID
	long lError = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, __regpath, NULL, KEY_QUERY_VALUE, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		long lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("0").crypt_get() /* column */, NULL, &dataType, (LPBYTE)&RegKey, &BufSize);
		if (lVal == ERROR_SUCCESS)
		{
			std::string szRegKey = RegKey;
	
			CHAR __key1[] = { '7', '6', '4', '8', '7', '-', '6', '4', '0', '-', '1', '4', '5', '7', '2', '3', '6', '-', '2', '3', '8', '3', '7', 0x0 }; // 76487-640-1457236-23837
			CHAR __key2[] = { '7', '6', '4', '8', '7', '-', '3', '3', '7', '-', '8', '4', '2', '9', '9', '5', '5', '-', '2', '2', '6', '1', '4', 0x0 }; // 76487-337-8429955-22614
			CHAR __key3[] = { '7', '6', '4', '8', '7', '-', '6', '4', '4', '-', '3', '1', '7', '7', '0', '3', '7', '-', '2', '3', '5', '1', '0', 0x0 }; // 76487-644-3177037-23510
			CHAR __key4[] = { '7', '6', '4', '9', '7', '-', '6', '4', '0', '-', '6', '3', '0', '8', '8', '7', '3', '-', '2', '3', '8', '3', '5', 0x0 }; // 76497-640-6308873-23835
			CHAR __key5[] = { '5', '5', '2', '7', '4', '-', '6', '4', '0', '-', '2', '6', '7', '3', '0', '6', '4', '-', '2', '3', '9', '5', '0', 0x0 }; // 55274-640-2673064-23950
			CHAR __key6[] = { '7', '6', '4', '8', '7', '-', '6', '4', '0', '-', '8', '8', '3', '4', '0', '0', '5', '-', '2', '3', '1', '9', '5', 0x0 }; // 76487-640-8834005-23195
			CHAR __key7[] = { '7', '6', '4', '8', '7', '-', '6', '4', '0', '-', '0', '7', '1', '6', '6', '6', '2', '-', '2', '3', '5', '3', '5', 0x0 }; // 76487-640-0716662-23535
			CHAR __key8[] = { '7', '6', '4', '8', '7', '-', '6', '4', '4', '-', '8', '6', '4', '8', '4', '6', '6', '-', '2', '3', '1', '0', '6', 0x0 }; // 76487-644-8648466-23106
			CHAR __key9[] = { '0', '0', '4', '2', '6', '-', '2', '9', '3', '-', '8', '1', '7', '0', '0', '3', '2', '-', '8', '5', '1', '4', '6', 0x0 }; // 00426-293-8170032-85146
			CHAR __key10[] = { '7', '6', '4', '8', '7', '-', '3', '4', '1', '-', '5', '8', '8', '3', '8', '1', '2', '-', '2', '2', '4', '2', '0', 0x0 }; // 76487-341-5883812-22420
			CHAR __key11[] = { '7', '6', '4', '8', '7', '-', 'O', 'E', 'M', '-', '0', '0', '2', '7', '4', '5', '3', '-', '6', '3', '7', '9', '6', 0x0 }; // 76487-OEM-0027453-63796

			if (szRegKey == __key1)
			{
				if (pdwErrorCode) *pdwErrorCode = 100;
				return false;
			}
			
			if (szRegKey == __key2) 
			{
				if (pdwErrorCode) *pdwErrorCode = 101;
				return false;
			}

			if (szRegKey == __key3) 
			{
				if (pdwErrorCode) *pdwErrorCode = 102;
				return false;
			}

			if (szRegKey == __key4) 
			{
				if (pdwErrorCode) *pdwErrorCode = 103;
				return false;
			}

			if (szRegKey == __key5) 
			{
				if (pdwErrorCode) *pdwErrorCode = 104;
				return false;
			}

			if (szRegKey == __key6) 
			{
				if (pdwErrorCode) *pdwErrorCode = 105;
				return false;
			}

			if (szRegKey == __key7) 
			{
				if (pdwErrorCode) *pdwErrorCode = 106;
				return false;
			}

			if (szRegKey == __key8) 
			{
				if (pdwErrorCode) *pdwErrorCode = 107;
				return false;
			}

			if (szRegKey == __key9) 
			{
				if (pdwErrorCode) *pdwErrorCode = 108;
				return false;
			}

			if (szRegKey == __key10) 
			{
				if (pdwErrorCode) *pdwErrorCode = 109;
				return false;
			}

			if (szRegKey == __key11) 
			{
				if (pdwErrorCode) *pdwErrorCode = 110;
				return false;
			}
		}
		g_winapiApiTable->RegCloseKey(hKey);
	}
	return true;
}
#pragma endregion GeneralEmulationCheck

#pragma region TimeDurationCheck
inline DWORD GetTickCountTimer()
{
	auto tStart = g_winapiApiTable->GetTickCount();

	g_winapiApiTable->Sleep(1000);
	__try {
		int* p = 0; // access violation
		*p = 0;
	}
	__except (1) { }
	g_winapiApiTable->Sleep(1000);

	auto tEnd = g_winapiApiTable->GetTickCount();
	return (tEnd - tStart);
}

inline bool CheckGetTickCount()
{	
	auto dwDiff = GetTickCountTimer();
	DEBUG_LOG(LL_SYS, "dwDiff: %u", dwDiff);

	if (dwDiff > 3000)
	{
		return false;
	}
	return true;
}

inline auto StdChronoTimer()
{
	CTimer <std::chrono::milliseconds> checkTimer;

	g_winapiApiTable->Sleep(1000);
	__try {
		int* p = 0; // access violation
		*p = 0;
	}
	__except (1) { }
	g_winapiApiTable->Sleep(1000);

	return checkTimer.diff();
}

inline bool CheckStdChrono()
{
	auto llDiff = StdChronoTimer();
	DEBUG_LOG(LL_SYS, "Diff: %lld", llDiff);

	if (llDiff > 3000)
	{
		return false;
	}
	return true;
}
#pragma endregion TimeDurationCheck

bool CAntiEmulation::InitTimeChecks(LPDWORD pdwErrorStep)
{
	DEBUG_LOG(LL_SYS, "Anti emulation InitTimeChecks has been started");

#ifdef _DEBUG
	if (g_winapiApiTable->IsDebuggerPresent()) // pass it for debug build and if have a attached debugger
		return true;
#endif

	if (CheckGetTickCount() == false)
	{
		if (pdwErrorStep) *pdwErrorStep = 1;
		return false;
	}

	if (CheckStdChrono() == false)
	{
		if (pdwErrorStep) *pdwErrorStep = 2;
		return false;
	}

	DEBUG_LOG(LL_SYS, "Anti emulation InitTimeChecks completed");
	return true;
}

bool CAntiEmulation::InitAntiEmulation(LPDWORD pdwErrorStep)
{
	DEBUG_LOG(LL_SYS, "Anti emulation has been started");

	auto dwSandboxRet = 0UL;

	if (RandomApiCheck() == false)
	{
		if (pdwErrorStep) *pdwErrorStep = 1;
		return false;
	}

	if (CheckErrorMode() == false)
	{
		if (pdwErrorStep) *pdwErrorStep = 2;
		return false;
	}

	if (LoadNtOsKrnl() == false)
	{
		if (pdwErrorStep) *pdwErrorStep = 3;
		return false;
	}

	if (CheckRegistry_SandboxProductIDs(&dwSandboxRet) == false)
	{
		if (pdwErrorStep) *pdwErrorStep = dwSandboxRet;
		return false;
	}

	DEBUG_LOG(LL_SYS, "Anti emulation completed");
	return true;
}

