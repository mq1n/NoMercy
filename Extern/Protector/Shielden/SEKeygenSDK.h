/*
Safengine Keygen SDK
*/
#pragma once

typedef struct _SELicenseOptionsA
{
	char		UserID[256];
	char		Remarks[1024];
	BOOL		LockHardwareID;
	BOOL		LockCPU;
	BOOL		LockMAC;
	BOOL		LockBIOS;
	BOOL		LockHDD;
	char		HardwareID[256];

	BOOL		NumDaysEn;
	DWORD		NumDays;

	BOOL		NumExecEn;
	DWORD		NumExec;

	BOOL		ExpDateEn;
	SYSTEMTIME	ExpDate;

	BOOL		CountryIdEn;
	DWORD		CountryId;

	BOOL		ExecTimeEn;
	DWORD		ExecTime;

	BOOL		TotalExecTimeEn;
	DWORD		TotalExecTime;

}SELicenseOptionsA, *PSELicenseOptionsA;

typedef struct _SELicenseOptionsW
{
	wchar_t		UserID[256];
	wchar_t		Remarks[1024];
	BOOL		LockHardwareID;
	BOOL		LockCPU;
	BOOL		LockMAC;
	BOOL		LockBIOS;
	BOOL		LockHDD;
	wchar_t		HardwareID[256];

	BOOL		NumDaysEn;
	DWORD		NumDays;

	BOOL		NumExecEn;
	DWORD		NumExec;

	BOOL		ExpDateEn;
	SYSTEMTIME	ExpDate;

	BOOL		CountryIdEn;
	DWORD		CountryId;

	BOOL		ExecTimeEn;
	DWORD		ExecTime;

	BOOL		TotalExecTimeEn;
	DWORD		TotalExecTime;

}SELicenseOptionsW, *PSELicenseOptionsW;

#if defined(__cplusplus)
extern "C" {
#endif
	__declspec(dllimport) BOOL WINAPI SEGenerateKeyFileA(SELicenseOptionsA* pOptions, char* pOutputPath, DWORD MaxChars);
	__declspec(dllimport) BOOL WINAPI SEGenerateKeyFileW(SELicenseOptionsW* pOptions, wchar_t* pOutputPath, DWORD MaxWChars);
	__declspec(dllimport) BOOL WINAPI SEGenerateKeyFileExA(SELicenseOptionsA* pOptions, char* pLicenseDat, char* pOutputPath, DWORD MaxChars);
	__declspec(dllimport) BOOL WINAPI SEGenerateKeyFileExW(SELicenseOptionsW* pOptions, wchar_t* pLicenseDat, wchar_t* pOutputPath, DWORD MaxWChars);
	__declspec(dllimport) DWORD WINAPI SEGetMaxKeyBufferSize();
	__declspec(dllimport) DWORD WINAPI SEGenerateKeyBufferA(SELicenseOptionsA* pOptions, char* pKeyFileName, DWORD MaxChars, char* pKeyBuffer);
	__declspec(dllimport) DWORD WINAPI SEGenerateKeyBufferW(SELicenseOptionsW* pOptions, wchar_t* pKeyFileName, DWORD MaxChars, char* pKeyBuffer);
	__declspec(dllimport) DWORD WINAPI SEGenerateKeyBufferExA(SELicenseOptionsA* pOptions, char* pLicenseDat, char* pKeyFileName, DWORD MaxChars, char* pKeyBuffer);
	__declspec(dllimport) DWORD WINAPI SEGenerateKeyBufferExW(SELicenseOptionsW* pOptions, wchar_t* pLicenseDat, wchar_t* pKeyFileName, DWORD MaxWChars, char* pKeyBuffer);
	__declspec(dllimport) BOOL WINAPI SEGenerateKeyFileWithDatA(SELicenseOptionsA* pOptions, char* pDat, char* pOutputPath, DWORD MaxChars);
	__declspec(dllimport) BOOL WINAPI SEGenerateKeyFileWithDatW(SELicenseOptionsW* pOptions, char* pDat, wchar_t* pOutputPath, DWORD MaxWChars);
	__declspec(dllimport) DWORD WINAPI SEGenerateKeyBufferWithDatA(SELicenseOptionsA* pOptions, char* pDat, char* pKeyFileName, DWORD MaxChars, char* pKeyBuffer);
	__declspec(dllimport) DWORD WINAPI SEGenerateKeyBufferWithDatW(SELicenseOptionsW* pOptions, char* pDat, wchar_t* pKeyFileName, DWORD MaxWChars, char* pKeyBuffer);
#if defined(__cplusplus)
}
#ifdef UNICODE
#define SELicenseOptions SELicenseOptionsW
#define SEGenerateKeyFile SEGenerateKeyFileW
#define SEGenerateKeyFileEx SEGenerateKeyFileExW
#define SEGenerateKeyFileWithDat SEGenerateKeyFileWithDatW
#define SEGenerateKeyBufferWithDat SEGenerateKeyBufferWithDatW
#else
#define SELicenseOptions SELicenseOptionsA
#define SEGenerateKeyFile SEGenerateKeyFileA
#define SEGenerateKeyFileEx SEGenerateKeyFileExA
#define SEGenerateKeyFileWithDat SEGenerateKeyFileWithDatA
#define SEGenerateKeyBufferWithDat SEGenerateKeyBufferWithDatA
#endif
#endif