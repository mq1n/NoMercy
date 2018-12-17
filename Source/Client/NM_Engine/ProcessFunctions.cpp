#include "NM_Index.h"
#include "NM_Main.h"
#include "ProcessFunctions.h"
#include "Defines.h"
#include "ThreadEnumerator.h"
#include "SafeProcessHandle.h"

DWORD CProcessFunctions::GetProcessParentProcessId(DWORD dwMainProcessId)
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (g_winapiApiTable->Process32First(hSnap, &pe)) 
	{
		do {
			if (pe.th32ProcessID == dwMainProcessId) 
			{
				g_winapiApiTable->CloseHandle(hSnap);
				return pe.th32ParentProcessID;
			}
		} while (g_winapiApiTable->Process32Next(hSnap, &pe));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return 0;
}

DWORD CProcessFunctions::GetProcessIdFromProcessName(const char* c_szProcessName)
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (g_winapiApiTable->Process32First(hSnap, &pe)) 
	{
		do {
			std::string szProcessName = c_szProcessName;
			auto wszProcessName = std::wstring(szProcessName.begin(), szProcessName.end());

			std::wstring wszCurrProcessName = pe.szExeFile;
			std::transform(wszCurrProcessName.begin(), wszCurrProcessName.end(), wszCurrProcessName.begin(), towlower);

			if (!wcscmp(wszCurrProcessName.c_str(), wszProcessName.c_str()))
			{
				g_winapiApiTable->CloseHandle(hSnap);
				return pe.th32ProcessID;
			}
		} while (g_winapiApiTable->Process32Next(hSnap, &pe));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return 0;
}

DWORD CProcessFunctions::GetProcessCountFromProcessName(const char* c_szProcessName)
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	DWORD dwCount = 0;
	PROCESSENTRY32 pe = { 0 };;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (g_winapiApiTable->Process32First(hSnap, &pe)) 
	{
		do {
			std::string szProcessName = c_szProcessName;
			auto wszProcessName = std::wstring(szProcessName.begin(), szProcessName.end());

			std::wstring wszCurrProcessName = pe.szExeFile;
			std::transform(wszCurrProcessName.begin(), wszCurrProcessName.end(), wszCurrProcessName.begin(), towlower);

			if (!wcscmp(wszCurrProcessName.c_str(), wszProcessName.c_str()))
				dwCount++;

		} while (g_winapiApiTable->Process32Next(hSnap, &pe));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return dwCount;
}

std::string CProcessFunctions::GetProcessNameFromProcessId(DWORD dwProcessId)
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	PROCESSENTRY32 pe = { 0 };;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (g_winapiApiTable->Process32First(hSnap, &pe))
	{
		do {
			if (dwProcessId == pe.th32ProcessID)
			{
				std::wstring wszCurrProcessName = pe.szExeFile;
				std::string szCurrProcessName = std::string(wszCurrProcessName.begin(), wszCurrProcessName.end());
				std::transform(szCurrProcessName.begin(), szCurrProcessName.end(), szCurrProcessName.begin(), tolower);

				g_winapiApiTable->CloseHandle(hSnap);
				return szCurrProcessName;
			}
		} while (g_winapiApiTable->Process32Next(hSnap, &pe));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return std::string("");
}

bool CProcessFunctions::ProcessIsItAlive(DWORD dwProcessId)
{
	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return 0;
	}

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (g_winapiApiTable->Process32First(hSnap, &pe))
	{
		do {
			if (pe.th32ProcessID == dwProcessId)
			{
				g_winapiApiTable->CloseHandle(hSnap);
				return true;
			}
		} while (g_winapiApiTable->Process32Next(hSnap, &pe));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return false;
}

std::vector <DWORD> CProcessFunctions::GetProcessIdsFromProcessName(const char* c_szProcessName)
{
	auto vPIDs = std::vector<DWORD>();

	auto hSnap = g_winapiApiTable->CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!IS_VALID_HANDLE(hSnap))
	{
		DEBUG_LOG(LL_ERR, "CreateToolhelp32Snapshot fail! Error: %u", g_winapiApiTable->GetLastError());
		return vPIDs;
	}

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (g_winapiApiTable->Process32First(hSnap, &pe))
	{
		do {
			std::string szProcessName = c_szProcessName;
			auto wszProcessName = std::wstring(szProcessName.begin(), szProcessName.end());
			
			std::wstring wszCurrProcessName = pe.szExeFile;
			std::transform(wszCurrProcessName.begin(), wszCurrProcessName.end(), wszCurrProcessName.begin(), towlower);

			if (wcsstr(wszCurrProcessName.c_str(), wszProcessName.c_str()))
				vPIDs.emplace_back(pe.th32ProcessID);

		} while (g_winapiApiTable->Process32Next(hSnap, &pe));
	}

	g_winapiApiTable->CloseHandle(hSnap);
	return vPIDs;
}



std::string CProcessFunctions::GetProcessFullName(HANDLE hProcess)
{
	CHAR processPath[MAX_PATH] = { 0 };
	if (g_winapiApiTable->GetProcessImageFileNameA(hProcess, processPath, _countof(processPath)))
		return processPath;

	memset(&processPath, 0, MAX_PATH);
	if (g_winapiApiTable->GetModuleFileNameExA(hProcess, NULL, processPath, _countof(processPath)))
		return processPath;

	return std::string("");
}

std::string CProcessFunctions::GetProcessName(HANDLE hProcess)
{
	auto szDosName = GetProcessFullName(hProcess);
	if (szDosName.empty()) return std::string("");

	auto szProcessName = DosDevicePath2LogicalPath(szDosName.c_str());
	if (szProcessName.empty()) return std::string("");

	std::transform(szProcessName.begin(), szProcessName.end(), szProcessName.begin(), tolower);
	return szProcessName;
}

DWORD_PTR CProcessFunctions::GetProcessIdNative(HANDLE hProcess)
{
	PROCESS_BASIC_INFORMATION pPBI = { 0 };
	auto ntStat = g_winapiApiTable->NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pPBI, sizeof(pPBI), 0);
	if (!NT_SUCCESS(ntStat))
	{
		DEBUG_LOG(LL_ERR, "NtQueryInformationProcess fail! Target process: %p Status: %p", hProcess, ntStat);
		return 0UL;
	}

	return reinterpret_cast<DWORD_PTR>(pPBI.UniqueProcessId);
}

DWORD_PTR CProcessFunctions::GetParentProcessIdNative(HANDLE hProcess)
{
	PROCESS_BASIC_INFORMATION pPBI = { 0 };
	auto ntStat = g_winapiApiTable->NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pPBI, sizeof(pPBI), 0);
	if (!NT_SUCCESS(ntStat))
	{
		DEBUG_LOG(LL_ERR, "NtQueryInformationProcess fail! Target process: %p Status: %p", hProcess, ntStat);
		return 0UL;
	}

	return reinterpret_cast<DWORD_PTR>(pPBI.InheritedFromUniqueProcessId);
}

std::string CProcessFunctions::GetParentProcessName(DWORD dwCurrPID)
{
	std::string szOutput = "";

	auto dwParentPID = CProcessFunctions::GetProcessParentProcessId(dwCurrPID);
	if (!dwParentPID)
	{
		DEBUG_LOG(LL_ERR, "Parent PID not found! Last error: %u", g_winapiApiTable->GetLastError());
		return szOutput;
	}

	auto processEnumerator = std::make_unique<CSafeProcessHandle>();
	if (!processEnumerator || !processEnumerator.get())
	{
		DEBUG_LOG(LL_ERR, "processEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return szOutput;
	}

	auto hProcess = processEnumerator->FindProcessFromPID(dwParentPID);
	if (!IS_VALID_HANDLE(hProcess)) 
	{
		DEBUG_LOG(LL_ERR, "Parent process not found on process list! PID: %u", dwParentPID);

		hProcess = g_winapiApiTable->OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwParentPID);
		if (!IS_VALID_HANDLE(hProcess)) 
		{
			auto bIsAlive = CProcessFunctions::ProcessIsItAlive(dwParentPID);
			DEBUG_LOG(LL_ERR, "Parent process can not open! Last error: %u IsAlive: %d", g_winapiApiTable->GetLastError(), bIsAlive);
			return szOutput;
		}
	}

	auto szParentName = CProcessFunctions::GetProcessName(hProcess);
	if (szParentName.empty()) 
	{
		DEBUG_LOG(LL_ERR, "Parent process name not found!");
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
		return szOutput;
	}
	auto szLowerParentName = g_nmApp->FunctionsInstance()->szLower(szParentName);

	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
	szOutput = szLowerParentName;
	return szOutput;
}

std::string CProcessFunctions::DosDevicePath2LogicalPath(LPCSTR lpszDosPath)
{
	std::string strResult;
	char szTemp[MAX_PATH];
	szTemp[0] = '\0';

	if (lpszDosPath == NULL || strlen(lpszDosPath) == NULL || !GetLogicalDriveStringsA(_countof(szTemp) - 1, szTemp))
		return strResult;

	char szName[MAX_PATH];
	char szDrive[3] = " :";
	BOOL bFound = FALSE;
	char* p = szTemp;

	do {
		// Copy the drive letter to the template string
		*szDrive = *p;

		// Look up each device name
		if (QueryDosDeviceA(szDrive, szName, _countof(szName))) 
		{
			UINT uNameLen = (UINT)strlen(szName);

			if (uNameLen < MAX_PATH)
			{
				bFound = strncmp(lpszDosPath, szName, uNameLen) == 0;

				if (bFound) {
					// Reconstruct pszFilename using szTemp
					// Replace device path with DOS path
					char szTempFile[MAX_PATH];
					sprintf_s(szTempFile, xorstr("%s%s").crypt_get(), szDrive, lpszDosPath + uNameLen);
					strResult = szTempFile;
				}
			}
		}

		// Go to the next NULL character.
		while (*p++);
	} while (!bFound && *p); // end of string

	return strResult;
}

