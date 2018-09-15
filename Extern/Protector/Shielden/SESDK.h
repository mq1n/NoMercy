/*
Safengine SDK
*/

#pragma once

#ifdef __BORLANDC__
 
#define SE_UNPROTECT_START                  __emit__ (0xEB, 0x09, 0x53, 0x45, 0x55, 0x50, 0x42, 0x45, 0x47, 0x4E, 0x00);

#define SE_UNPROTECT_END                    __emit__ (0xEB, 0x09, 0x53, 0x45, 0x55, 0x50, 0x45, 0x4E, 0x44, 0x50, 0x00);

#define SE_PROTECT_START                    __emit__ (0xEB, 0x07, 0x53, 0x45, 0x42, 0x45, 0x47, 0x4E, 0x00);

#define SE_PROTECT_START_MUTATION           __emit__ (0xEB, 0x07, 0x53, 0x45, 0x42, 0x45, 0x47, 0x4E, 0x4D);

#define SE_PROTECT_START_VIRTUALIZATION     __emit__ (0xEB, 0x07, 0x53, 0x45, 0x42, 0x45, 0x47, 0x4E, 0x56);

#define SE_PROTECT_START_ULTRA              __emit__ (0xEB, 0x07, 0x53, 0x45, 0x42, 0x45, 0x47, 0x4E, 0x55);

#define SE_PROTECT_END                      __emit__ (0xEB, 0x07, 0x53, 0x45, 0x45, 0x4E, 0x44, 0x50, 0x00);

#else

#define SE_UNPROTECT_START __asm _emit 0xEB\
	__asm _emit 0x09\
	__asm _emit 0x53\
	__asm _emit 0x45\
	__asm _emit 0x55\
	__asm _emit 0x50\
	__asm _emit 0x42\
	__asm _emit 0x45\
	__asm _emit 0x47\
	__asm _emit 0x4E\
	__asm _emit 0x00;

#define SE_UNPROTECT_END __asm _emit 0xEB\
	__asm _emit 0x09\
	__asm _emit 0x53\
	__asm _emit 0x45\
	__asm _emit 0x55\
	__asm _emit 0x50\
	__asm _emit 0x45\
	__asm _emit 0x4E\
	__asm _emit 0x44\
	__asm _emit 0x50\
	__asm _emit 0x00;

#define SE_PROTECT_START __asm _emit 0xEB\
	__asm _emit 0x07\
	__asm _emit 0x53\
	__asm _emit 0x45\
	__asm _emit 0x42\
	__asm _emit 0x45\
	__asm _emit 0x47\
	__asm _emit 0x4E\
	__asm _emit 0x00;

#define SE_PROTECT_START_MUTATION __asm _emit 0xEB\
	__asm _emit 0x07\
	__asm _emit 0x53\
	__asm _emit 0x45\
	__asm _emit 0x42\
	__asm _emit 0x45\
	__asm _emit 0x47\
	__asm _emit 0x4E\
	__asm _emit 0x4D;

#define SE_PROTECT_START_VIRTUALIZATION __asm _emit 0xEB\
	__asm _emit 0x07\
	__asm _emit 0x53\
	__asm _emit 0x45\
	__asm _emit 0x42\
	__asm _emit 0x45\
	__asm _emit 0x47\
	__asm _emit 0x4E\
	__asm _emit 0x56;


#define SE_PROTECT_START_ULTRA __asm _emit 0xEB\
	__asm _emit 0x07\
	__asm _emit 0x53\
	__asm _emit 0x45\
	__asm _emit 0x42\
	__asm _emit 0x45\
	__asm _emit 0x47\
	__asm _emit 0x4E\
	__asm _emit 0x55;

#define SE_PROTECT_END __asm _emit 0xEB\
	__asm _emit 0x07\
	__asm _emit 0x53\
	__asm _emit 0x45\
	__asm _emit 0x45\
	__asm _emit 0x4E\
	__asm _emit 0x44\
	__asm _emit 0x50\
	__asm _emit 0x00;

#endif

#if defined(__cplusplus)
extern "C" {
#endif
	__declspec(dllimport) void WINAPI SEProtectStart();
	__declspec(dllimport) void WINAPI SEProtectStartMutation();
	__declspec(dllimport) void WINAPI SEProtectStartVirtualization();
	__declspec(dllimport) void WINAPI SEProtectStartUltra();
	__declspec(dllimport) void WINAPI SEProtectEnd();
	__declspec(dllimport) void WINAPI SEUnProtectStart();
	__declspec(dllimport) void WINAPI SEUnProtectEnd();
#if defined(__cplusplus)
}
#endif

#ifndef SESDK_MACROS_ONLY

#ifndef SESDK_TYPES

#define SESDK_TYPES

typedef enum _SEAppStatus
{
	SE_STATUS_INIT,
	SE_STATUS_NORMAL
} SEAppStatus;

#endif

#if defined(__cplusplus)
extern "C" {
#endif
	__declspec(dllimport) BOOL WINAPI SECheckProtection();
	__declspec(dllimport) char* WINAPI SEDecodeStringA(char* pStr);
	__declspec(dllimport) wchar_t* WINAPI SEDecodeStringW(wchar_t* pStr);
	__declspec(dllimport) void WINAPI SEFreeString(void* pStr);
	__declspec(dllimport) SEAppStatus WINAPI SEGetAppStatus();
	__declspec(dllimport) void WINAPI SESetAppStatus(SEAppStatus NewStatus);
	__declspec(dllimport) void WINAPI SEGetProtectionDate(SYSTEMTIME* pDate);
	__declspec(dllimport) BOOL WINAPI SEAddMemoryGuard(void* pData, DWORD dwSize);
	__declspec(dllimport) BOOL WINAPI SEDelMemoryGuard(void* pData, DWORD dwSize);
#if defined(__cplusplus)
}
#endif

#ifdef UNICODE
#define SEDecodeString SEDecodeStringW
#else
#define SEDecodeString SEDecodeStringA
#endif

#endif