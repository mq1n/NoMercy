#pragma once
#include <string>

class CProcessFunctions
{
	public:
		static DWORD		GetProcessParentProcessId(DWORD dwMainProcessId);
		static DWORD		GetProcessIdFromProcessName(const char* c_szProcessName);
		static DWORD		GetProcessCountFromProcessName(const char* c_szProcessName);
		static std::string	GetProcessNameFromProcessId(DWORD dwProcessId);
		static bool			ProcessIsItAlive(DWORD dwProcessId);
		static std::string	GetProcessFullName(HANDLE hProcess);
		static std::vector <DWORD> GetProcessIdsFromProcessName(const char* c_szProcessName);
		static std::string  GetProcessName(HANDLE hProcess);
		static DWORD_PTR	GetProcessIdNative(HANDLE hProcess);
		static DWORD_PTR	GetParentProcessIdNative(HANDLE hProcess);
		static std::string  DosDevicePath2LogicalPath(LPCSTR lpszDosPath);
		static std::string	GetParentProcessName(DWORD dwCurrPID);
};

