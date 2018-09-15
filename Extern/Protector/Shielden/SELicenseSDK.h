/*
Safengine Licensing SDK
*/
#pragma once

#ifndef _SELICENSE_TYPES

#define _SELICENSE_TYPES

typedef struct _SELicenseUserInfoA
{
	char		UserID[256];
	char		Remarks[1024];
	SYSTEMTIME	LicenseDate;

} SELicenseUserInfoA, *PSELicenseUserInfoA;

typedef struct _SELicenseUserInfoW
{
	wchar_t		UserID[256];
	wchar_t		Remarks[1024];
	SYSTEMTIME	LicenseDate;

} SELicenseUserInfoW, *PSELicenseUserInfoW;

typedef struct _SELicenseTrialInfo
{
	DWORD		NumDays;
	DWORD		NumExec;
	SYSTEMTIME	ExpDate;
	DWORD		CountryId;
	DWORD		ExecTime;
	DWORD		TotalExecTime;

} SELicenseTrialInfo, *PSELicenseTrialInfo;

typedef struct _SELicenseHashInfo
{
	char	Hash[16];

} SELicenseHashInfo, *PSELicenseHashInfo;

typedef enum _SEErrorType
{
	SE_ERR_SUCCESS,
	SE_ERR_INTERNAL_ERROR,
	SE_ERR_TOOL_DETECTION,
	SE_ERR_CHECKSUM_FAILED,
	SE_ERR_VIRTUALIZATION_FOUND,
	SE_ERR_LICENSE_NOT_FOUND,
	SE_ERR_LICENSE_CORRUPTED,
	SE_ERR_LICENSE_FILE_MISMATCH,
	SE_ERR_LICENSE_HARDWARE_ID_MISMATCH,

	SE_ERR_LICENSE_DAYS_EXPIRED,
	SE_ERR_LICENSE_EXEC_EXPIRED,
	SE_ERR_LICENSE_DATE_EXPIRED,
	SE_ERR_LICENSE_COUNTRY_ID_MISMATCH,
	SE_ERR_LICENSE_NO_MORE_EXEC_TIME,
	SE_ERR_LICENSE_NO_MORE_TOTALEXEC_TIME,
	SE_ERR_LICENSE_BANNED,
	SE_ERR_SERVER_ERROR

} SEErrorType;

#endif

#if defined(__cplusplus)
extern "C" {
#endif
	__declspec(dllimport) DWORD WINAPI SEGetNumExecUsed();
	__declspec(dllimport) DWORD WINAPI SEGetNumExecLeft();
	__declspec(dllimport) SEErrorType WINAPI SESetNumExecUsed(DWORD Num);
	__declspec(dllimport) DWORD WINAPI SEGetExecTimeUsed();
	__declspec(dllimport) DWORD WINAPI SEGetExecTimeLeft();
	__declspec(dllimport) SEErrorType WINAPI SESetExecTime(DWORD Num);
	__declspec(dllimport) DWORD WINAPI SEGetTotalExecTimeUsed();
	__declspec(dllimport) DWORD WINAPI SEGetTotalExecTimeLeft();
	__declspec(dllimport) SEErrorType WINAPI SESetTotalExecTime(DWORD Num);
	__declspec(dllimport) DWORD WINAPI SEGetNumDaysUsed();
	__declspec(dllimport) DWORD WINAPI SEGetNumDaysLeft();
	__declspec(dllimport) SEErrorType WINAPI SECheckHardwareID();
	__declspec(dllimport) SEErrorType WINAPI SECheckExpDate();
	__declspec(dllimport) SEErrorType WINAPI SECheckExecTime();
	__declspec(dllimport) SEErrorType WINAPI SECheckTotalExecTime();
	__declspec(dllimport) SEErrorType WINAPI SECheckCountryID();
	__declspec(dllimport) SEErrorType WINAPI SEGetLicenseUserInfoA(SELicenseUserInfoA* pInfo);
	__declspec(dllimport) SEErrorType WINAPI SEGetLicenseUserInfoW(SELicenseUserInfoW* pInfo);
	__declspec(dllimport) SEErrorType WINAPI SEGetLicenseTrialInfo(SELicenseTrialInfo* pInfo);
	__declspec(dllimport) SEErrorType WINAPI SEGetHardwareIDA(char* pBuf, int MaxChars);
	__declspec(dllimport) SEErrorType WINAPI SEGetHardwareIDW(wchar_t* pBuf, int MaxWChars);
	__declspec(dllimport) SEErrorType WINAPI SECheckLicenseFileA(char* LicenseFileName);
	__declspec(dllimport) SEErrorType WINAPI SECheckLicenseFileW(wchar_t* LicenseFileName);
	__declspec(dllimport) SEErrorType WINAPI SECheckLicenseFileEx(char* pLicenseFile, DWORD Size);
	__declspec(dllimport) SEErrorType WINAPI SEGetLicenseHash(SELicenseHashInfo* pInfo);
	__declspec(dllimport) void WINAPI SENotifyLicenseBanned(BOOL Exit);
	__declspec(dllimport) SEErrorType WINAPI SEResetTrial();
#if defined(__cplusplus)
}
#endif

#ifdef UNICODE
#define SELicenseUserInfo SELicenseUserInfoW
#define SEGetLicenseUserInfo SEGetLicenseUserInfoW
#define SECheckLicenseFile SECheckLicenseFileW
#define SEGetHardwareID SEGetHardwareIDW
#else
#define SELicenseUserInfo SELicenseUserInfoA
#define SEGetLicenseUserInfo SEGetLicenseUserInfoA
#define SECheckLicenseFile SECheckLicenseFileA
#define SEGetHardwareID SEGetHardwareIDA
#endif