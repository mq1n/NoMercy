/*
Safengine SDK (x64)
*/

#pragma once

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
	__declspec(dllimport) SEAppStatus WINAPI SEGetAppStatus();
	__declspec(dllimport) void WINAPI SESetAppStatus(SEAppStatus NewStatus);
	__declspec(dllimport) void WINAPI SEGetProtectionDate(SYSTEMTIME* pDate);
	__declspec(dllimport) BOOL WINAPI SEAddMemoryGuard(void* pData, DWORD dwSize);
	__declspec(dllimport) BOOL WINAPI SEDelMemoryGuard(void* pData, DWORD dwSize);
#if defined(__cplusplus)
}
#endif

#endif