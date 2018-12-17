#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "Defines.h"

#define CODEINTEGRITY_OPTION_ENABLED						0x1
#define CODEINTEGRITY_OPTION_TESTSIGN						0x2
#define CODEINTEGRITY_OPTION_UMCI_ENABLED					0x4
#define CODEINTEGRITY_OPTION_UMCI_AUDITMODE_ENABLED			0x8
#define CODEINTEGRITY_OPTION_UMCI_EXCLUSIONPATHS_ENABLED	0x10
#define CODEINTEGRITY_OPTION_TEST_BUILD						0x20
#define CODEINTEGRITY_OPTION_PREPRODUCTION_BUILD			0x40
#define CODEINTEGRITY_OPTION_DEBUGMODE_ENABLED				0x80
#define CODEINTEGRITY_OPTION_FLIGHT_BUILD					0x100
#define CODEINTEGRITY_OPTION_FLIGHTING_ENABLED				0x200
#define CODEINTEGRITY_OPTION_HVCI_KMCI_ENABLED				0x400
#define CODEINTEGRITY_OPTION_HVCI_KMCI_AUDITMODE_ENABLED	0x800
#define CODEINTEGRITY_OPTION_HVCI_KMCI_STRICTMODE_ENABLED	0x1000
#define CODEINTEGRITY_OPTION_HVCI_IUM_ENABLED				0x2000

// TODO:
// SystemCodeIntegrityUnlockInformation SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION -- win10+
// NtLoadDriver "flpydisk" 

inline bool CheckTestSign_Type1()
{
	SYSTEM_CODEINTEGRITY_INFORMATION sci = { 0 };
	sci.Length = sizeof(sci);

	auto dwcbSz = 0UL;
	auto ntStat = g_winapiApiTable->NtQuerySystemInformation(SystemCodeIntegrityInformation, &sci, sizeof(sci), &dwcbSz);
	if (!NT_SUCCESS(ntStat) || dwcbSz != sizeof(sci))
		return false;

	auto bTestsigningEnabled = !!(sci.CodeIntegrityOptions & CODEINTEGRITY_OPTION_TESTSIGN);
	return bTestsigningEnabled;
}

inline bool CheckTestSign_Type2()
{
	bool bRet = false;
	char RegKey[_MAX_PATH] = { 0 };
	DWORD BufSize = _MAX_PATH;
	DWORD dataType = REG_DWORD;

	HKEY hKey;
	long lError = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("SYSTEM\\CurrentControlSet\\Control\\CI").crypt_get(), NULL, KEY_QUERY_VALUE, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		long lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("DebugFlags").crypt_get(), NULL, &dataType, (LPBYTE)&RegKey, &BufSize);
		if (lVal == ERROR_SUCCESS)
		{
			if (!strcmp(RegKey, xorstr("1").crypt_get()))
				bRet = true;
		}
		g_winapiApiTable->RegCloseKey(hKey);
	}
	return bRet;
}

inline bool CheckTestSign_Type3()
{
	bool bRet = false;
	char RegKey[_MAX_PATH] = { 0 };
	DWORD BufSize = _MAX_PATH;
	DWORD dataType = REG_SZ;

	HKEY hKey;
	long lError = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("SYSTEM\\CurrentControlSet\\Control").crypt_get(), NULL, KEY_QUERY_VALUE, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		long lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("SystemStartOptions").crypt_get(), NULL, &dataType, (LPBYTE)&RegKey, &BufSize);
		if (lVal == ERROR_SUCCESS)
		{
			if (strstr(RegKey, xorstr("TESTSIGNING").crypt_get()))
				bRet = true;
		}
		g_winapiApiTable->RegCloseKey(hKey);
	}
	return bRet;
}

inline bool CheckTestSign_Type4()
{
	bool bRet = false;
	char RegKey[_MAX_PATH] = { 0 };
	DWORD BufSize = _MAX_PATH;
	DWORD dataType = REG_SZ;

	HKEY hKey;
	long lError = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("SYSTEM\\CurrentControlSet\\Control").crypt_get(), NULL, KEY_QUERY_VALUE, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		long lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("SystemStartOptions").crypt_get(), NULL, &dataType, (LPBYTE)&RegKey, &BufSize);
		if (lVal == ERROR_SUCCESS)
		{
			if (strstr(RegKey, xorstr("DISABLE_INTEGRITY_CHECKS").crypt_get()))
				bRet = true;
		}
		g_winapiApiTable->RegCloseKey(hKey);
	}
	return bRet;
}

inline bool CheckTestSign_Type5()
{
	HKEY hTestKey;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("BCD00000000\\Objects").crypt_get(), 0, KEY_READ, &hTestKey) != ERROR_SUCCESS)
		return false;

	char    achKey[255];
	DWORD    cbName;
	char    achClass[MAX_PATH] = "";
	DWORD    cchClassName = MAX_PATH;
	DWORD    cSubKeys = 0;
	DWORD    cbMaxSubKey;
	DWORD    cchMaxClass;
	DWORD    cValues;
	DWORD    cchMaxValue;
	DWORD    cbMaxValueData;
	DWORD    cbSecurityDescriptor;
	FILETIME ftLastWriteTime;

	bool bRet = false;

	DWORD dwReturn[1000];
	DWORD dwBufSize = sizeof(dwReturn);

	auto dwApiRetCode = RegQueryInfoKeyA(hTestKey, achClass, &cchClassName, NULL, &cSubKeys, &cbMaxSubKey, &cchMaxClass, &cValues, &cchMaxValue, &cbMaxValueData,
		&cbSecurityDescriptor, &ftLastWriteTime);

	if (cSubKeys)
	{
		for (DWORD i = 0; i < cSubKeys; i++)
		{
			cbName = 255;
			dwApiRetCode = RegEnumKeyExA(hTestKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);
			if (dwApiRetCode == ERROR_SUCCESS)
			{
				char szNewWay[4096];
				sprintf(szNewWay, xorstr("BCD00000000\\Objects\\%s\\Elements\\16000049").crypt_get(), achKey);

				HKEY hnewKey;
				long lError = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, szNewWay, NULL, KEY_QUERY_VALUE, &hnewKey);
				if (lError == ERROR_SUCCESS)
				{
					long lVal = g_winapiApiTable->RegQueryValueExA(hnewKey, xorstr("Element").crypt_get(), NULL, 0, (LPBYTE)dwReturn, &dwBufSize);
					if (lVal == ERROR_SUCCESS)
					{
						if (dwReturn[0] == 1UL)
							bRet = true;
					}
					g_winapiApiTable->RegCloseKey(hnewKey);
				}

			}
		}
	}

	g_winapiApiTable->RegCloseKey(hTestKey);
	return bRet;
}

bool CheckTestSign_Type6()
{
	bool bRet = false;
	BYTE Result;
	DWORD BufSize = sizeof(Result);
	DWORD dataType = REG_BINARY;

	HKEY hKey;
	long lError = g_winapiApiTable->RegOpenKeyExA(HKEY_LOCAL_MACHINE, xorstr("SOFTWARE\\Microsoft\\Driver Signing").crypt_get(), NULL, KEY_QUERY_VALUE, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		long lVal = g_winapiApiTable->RegQueryValueExA(hKey, xorstr("Policy").crypt_get(), NULL, &dataType, &Result, &BufSize);
		if (lVal == ERROR_SUCCESS)
		{
			if (Result == 0x02)
				bRet = true;
		}
		g_winapiApiTable->RegCloseKey(hKey);
	}
	return bRet;
}

bool IScanner::IsTestSignEnabled(LPDWORD pdwReturnCode)
{
#ifdef DRIVER_TEST
	return false;
#endif

	auto dwTestSignRet = 0UL;
	do
	{
		if (CheckTestSign_Type1())
		{
			dwTestSignRet = 1;
			break;
		}

		if (CheckTestSign_Type2())
		{
			dwTestSignRet = 2;
			break;
		}

		if (CheckTestSign_Type3())
		{
			dwTestSignRet = 3;
			break;
		}

		if (CheckTestSign_Type4())
		{
			dwTestSignRet = 4;
			break;
		}

		if (CheckTestSign_Type5())
		{
			dwTestSignRet = 5;
			break;
		}

		if (CheckTestSign_Type6())
		{
			dwTestSignRet = 6;
			break;
		}

	} while (false);

	if (pdwReturnCode) *pdwReturnCode = dwTestSignRet;
	return dwTestSignRet != 0;
}


bool IScanner::IsSecureBootEnabled()
{
	SYSTEM_SECUREBOOT_INFORMATION ssbi = { 0 };

	auto dwcbSz = 0UL;
	auto ntStat = g_winapiApiTable->NtQuerySystemInformation(SystemSecureBootInformation, &ssbi, sizeof(ssbi), &dwcbSz);
	if (!NT_SUCCESS(ntStat) || dwcbSz != sizeof(ssbi))
	{
		DEBUG_LOG(LL_ERR, "SystemSecureBootInformation query fail! Ntstatus: %p Return size: %u Ctx size: %u", ntStat, dwcbSz, sizeof(ssbi));
		return true; 
	}

	if (ssbi.SecureBootCapable && !ssbi.SecureBootEnabled)
		return false;
	return true;
}

