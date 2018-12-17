#pragma once
/******************************************************************
*                                                                 *
*  VersionHelpers.h -- This module defines helper functions to    *
*                      promote version check with proper          *
*                      comparisons.                               *
*                                                                 *
*  Copyright (c) Microsoft Corp.  All rights reserved.            *
*                                                                 *
******************************************************************/
#ifndef _versionhelpers_H_INCLUDED_
#define _versionhelpers_H_INCLUDED_

#define WINAPI_FAMILY_PARTITION(Partitions)     (Partitions)
#define WINAPI_PARTITION_DESKTOP   (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#define _WIN32_WINNT_WINXP                  0x0501
#define _WIN32_WINNT_VISTA                  0x0600
#define _WIN32_WINNT_WIN7                   0x0601
#define _WIN32_WINNT_WIN8                   0x0602
#define _WIN32_WINNT_WINBLUE                0x0603
//#define _WIN32_WINNT_WIN10					0x0604

#include <Windows.h>
#pragma once

#pragma region Application Family

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#include <specstrings.h> // for _In_, etc.
#if !defined(__midl) && !defined(SORTPP_PASS)
#if (NTDDI_VERSION >= NTDDI_WINXP)
#ifdef __cplusplus
#define VERSIONHELPERAPI __forceinline bool
#else // __cplusplus
#define VERSIONHELPERAPI FORCEINLINE BOOL
#endif // __cplusplus

#include "DynamicWinapi.h"
#include <lazy_importer.hpp>

VERSIONHELPERAPI IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	if (g_winapiApiTable->RtlGetVersion)
	{
		RTL_OSVERSIONINFOEXW verInfo = { 0 };
		verInfo.dwOSVersionInfoSize = sizeof(verInfo);

		if (g_winapiApiTable->RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
		{
			if (verInfo.dwMajorVersion > wMajorVersion)
				return true;
			else if (verInfo.dwMajorVersion < wMajorVersion)
				return false;

			if (verInfo.dwMinorVersion > wMinorVersion)
				return true;
			else if (verInfo.dwMinorVersion < wMinorVersion)
				return false;

			if (verInfo.wServicePackMajor >= wServicePackMajor)
				return true;
		}
		return false;
	}
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
	DWORDLONG const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			VerSetConditionMask(
				0, VER_MAJORVERSION, VER_GREATER_EQUAL),
			VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;
	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

inline DWORD GetWindowsBuildNumber()
{
	DWORD dwResult = 0;
	if (g_winapiApiTable->RtlGetVersion)
	{
		RTL_OSVERSIONINFOEXW verInfo = { 0 };
		verInfo.dwOSVersionInfoSize = sizeof(verInfo);

		if (g_winapiApiTable->RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
			dwResult = verInfo.dwBuildNumber;
	}
	return dwResult;
}

inline DWORD GetWindowsMajorVersion()
{
	DWORD dwResult = 0;
	if (g_winapiApiTable->RtlGetVersion)
	{
		RTL_OSVERSIONINFOEXW verInfo = { 0 };
		verInfo.dwOSVersionInfoSize = sizeof(verInfo);

		if (g_winapiApiTable->RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
			dwResult = verInfo.dwMajorVersion;
	}
	return dwResult;
}

inline DWORD GetWindowsMinorVersion()
{
	DWORD dwResult = 0;
	if (g_winapiApiTable->RtlGetVersion)
	{
		RTL_OSVERSIONINFOEXW verInfo = { 0 };
		verInfo.dwOSVersionInfoSize = sizeof(verInfo);

		if (g_winapiApiTable->RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
			dwResult = verInfo.dwMinorVersion;
	}
	return dwResult;
}

inline DWORD GetWindowsServicePackVersion()
{
	DWORD dwResult = 0;
	if (g_winapiApiTable->RtlGetVersion)
	{
		RTL_OSVERSIONINFOEXW verInfo = { 0 };
		verInfo.dwOSVersionInfoSize = sizeof(verInfo);

		if (g_winapiApiTable->RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
			dwResult = verInfo.wServicePackMajor;
	}
	return dwResult;
}

inline BOOL IsFakeConditionalVersion()
{
	OSVERSIONINFOEXA pOsVersionInfo = { 0 };
	pOsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
	if (!LI_FIND(GetVersionExA)((POSVERSIONINFOA)&pOsVersionInfo))
		return FALSE;

	if (pOsVersionInfo.dwMajorVersion > 0xf1)
		return TRUE;

	return FALSE;
}


VERSIONHELPERAPI IsWindowsXPOrGreater() 
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 0);
}
VERSIONHELPERAPI IsWindowsXPSP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 1);
}
VERSIONHELPERAPI IsWindowsXPSP2OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 2);
}
VERSIONHELPERAPI IsWindowsXPSP3OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 3);
}

VERSIONHELPERAPI IsWindowsVistaOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}
VERSIONHELPERAPI IsWindowsVistaSP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 1);
}
VERSIONHELPERAPI IsWindowsVistaSP2OrGreater() 
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 2);
}

VERSIONHELPERAPI IsWindows7OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
}
VERSIONHELPERAPI IsWindows7SP1OrGreater() 
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1);
}

VERSIONHELPERAPI IsWindows8OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}

VERSIONHELPERAPI IsWindows8Point1OrGreater() 
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0);
}

VERSIONHELPERAPI IsWindows10OrGreater() 
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0);
}


VERSIONHELPERAPI IsWindowsServer()
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0, 0, VER_NT_WORKSTATION };

	DWORDLONG dwlConditionMask = 0;
	if (g_winapiApiTable->VerSetConditionMask)
		dwlConditionMask = g_winapiApiTable->VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);
	else
		dwlConditionMask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);

	if (g_winapiApiTable->VerifyVersionInfoW)
		return !g_winapiApiTable->VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
	return !VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
}


#endif // NTDDI_VERSION

#endif // defined(__midl)

#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) */

#pragma endregion

#endif // _VERSIONHELPERS_H_INCLUDED_
