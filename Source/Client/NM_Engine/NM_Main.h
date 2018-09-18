#pragma once

#pragma warning(disable: 4091)

#include <SDKDDKVer.h>
#include <Windows.h>
#include <windowsx.h>

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <algorithm>
#include <memory>
#include <vector>
#include <map>
#include <cstdlib>
#include <mutex>
#include <Sstream>
#include <Locale>
#include <algorithm>
#include <list>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <stack>
#include <new>
#include <future>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <random>
#include <csignal>

#include <TlHelp32.h>
#include <Psapi.h>
#include <io.h>
#include <sys/stat.h>
#include <sddl.h>
#include <Iphlpapi.h>
#include <wincrypt.h>
#include <WinTrust.h>
#include <mscat.h>
#include <AccCtrl.h>
#include <Aclapi.h>
#include <assert.h>
#include <intrin.h>
#include <excpt.h>
#include <comdef.h>
#include <WinInet.h>
#include <wbemidl.h>
#include <ShellAPI.h>
#include <winevt.h>
#include <Softpub.h>

#include "NM_App.h"
#include "Windows_Structs.h"

namespace Gdiplus
{
	using std::min;
	using std::max;
};

#define Stringize( L )     #L 
#define MakeString( M, L ) M(L)
#define $Line MakeString( Stringize, __LINE__ )
#define TODO_MSG __FILE__ "(" $Line ") : "

#ifndef _DEBUG
#define CUSTOM_LOG_FILENAME			XOR("NoMercy.log")
#else
#define CUSTOM_LOG_FILENAME			XOR("NoMercy/NoMercy.log")
#endif
#define CUSTOM_HOOK_LOG_FILENAME	XOR("NoMercy/NoMercyHook.log")
#define CUSTOM_NET_LOG_FILENAME		XOR("NoMercy/NoMercyNetwork.log")

#define STATUS_NO_MORE_ENTRIES				((NTSTATUS)0x8000001AL)
#define STATUS_RESOURCE_NAME_NOT_FOUND		((NTSTATUS)0xC000008B)
#define STATUS_INFO_LENGTH_MISMATCH			((NTSTATUS)0xC0000004)
#define STATUS_NOT_ALL_ASSIGNED				((NTSTATUS)0x00000106L)
#define STATUS_INVALID_PAGE_PROTECTION		((NTSTATUS)0xC0000045)
#define STATUS_UNABLE_TO_FREE_VM			((NTSTATUS)0xC000001A)
#define STATUS_BAD_TOKEN_TYPE				((NTSTATUS)0xC00000A8)
#define STATUS_BUFFER_OVERFLOW				((NTSTATUS)0x80000005L)
#define STATUS_DEBUGGER_INACTIVE			((NTSTATUS)0xC0000354L)
#define STATUS_SUCCESS						((NTSTATUS)0x00000000L)
#define STATUS_UNSUCCESSFUL					((NTSTATUS)0xC0000001)
#define ACCESS_DENIED						((NTSTATUS)0xC0000022)

#define NT_SUCCESS(Status)					(((NTSTATUS)(Status)) >= 0)
#define NT_ERROR(Status)					((((ULONG)(Status)) >> 30) == 3)

#define NtCurrentProcess					((HANDLE)-1)
#define NtCurrentThread						((HANDLE)-2)

#define IS_VALID_HANDLE(handle)				(handle && handle != INVALID_HANDLE_VALUE)
#define IS_VALID_SMART_PTR(ptr)				(ptr && ptr.get())

#define MakePtr(a, b)    ( ((ULONG_PTR)a ) + ((ULONG_PTR)b ) )
#define LDRP_IMAGE_DLL                          0x00000004
#define LDRP_PROCESS_ATTACH_CALLED              0x00080000

#define DEBUG_OBJECT_KILLONCLOSE	0x1
#define DEBUG_READ_EVENT			0x0001
#define DEBUG_PROCESS_ASSIGN		0x0002
#define DEBUG_SET_INFORMATION		0x0004
#define DEBUG_QUERY_INFORMATION		0x0008
#define DEBUG_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE |  DEBUG_READ_EVENT | DEBUG_PROCESS_ASSIGN | DEBUG_SET_INFORMATION | DEBUG_QUERY_INFORMATION)

#define DIRECTORY_QUERY					0x0001
#define OBJ_CASE_INSENSITIVE			0x00000040L

#define DUPLICATE_SAME_ATTRIBUTES		0x00000004

#define STATUS_SERVICE_NOTIFICATION		0x40000018
#define HARDERROR_OVERRIDE_ERRORMODE	0x10000000

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

#define IsEqualStr(str1, str2)				(!strcmp(str1, str2))
#define HasInVector(targetvector, element)	(std::find(targetvector.begin(), targetvector.end(), element) != targetvector.end())

static inline TEB* GetCurrentTeb()
{
	return reinterpret_cast<TEB*>(
#ifdef _M_X64
		__readgsqword(offsetof(NT_TIB, Self))
#elif _M_IX86
		__readfsdword(offsetof(NT_TIB, Self))
#else
#error "architecture unsupported"
#endif
	);
}
static inline PEB* GetCurrentPEB()
{
	return reinterpret_cast<PEB*>(GetCurrentTeb()->ProcessEnvironmentBlock);
}

extern std::shared_ptr <CNM_App> g_nmApp;
