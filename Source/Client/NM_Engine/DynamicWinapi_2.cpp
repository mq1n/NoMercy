#include "NM_Index.h"
#include "NM_Main.h"
#include "DynamicWinapi.h"

void CDynamicWinapi::BindAPIs_2()
{
	g_winapiApiTable->OpenThread = (lpOpenThread)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("OpenThread").crypt_get());
	g_winapiApiTable->NtDuplicateObject = (lpNtDuplicateObject)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtDuplicateObject").crypt_get());
	g_winapiApiTable->NtQueryObject = (lpNtQueryObject)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtQueryObject").crypt_get());
	g_winapiApiTable->GetFileSize = (lpGetFileSize)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetFileSize").crypt_get());
	g_winapiApiTable->FindFirstFileA = (lpFindFirstFileA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("FindFirstFileA").crypt_get());
	g_winapiApiTable->FindNextFileA = (lpFindNextFileA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("FindNextFileA").crypt_get());
	g_winapiApiTable->SetFileAttributesA = (lpSetFileAttributesA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("SetFileAttributesA").crypt_get());
	g_winapiApiTable->RemoveDirectoryA = (lpRemoveDirectoryA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("RemoveDirectoryA").crypt_get());
	g_winapiApiTable->DeleteFileA = (lpDeleteFileA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("DeleteFileA").crypt_get());
	g_winapiApiTable->GetFileAttributesA = (lpGetFileAttributesA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetFileAttributesA").crypt_get());
	g_winapiApiTable->FindClose = (lpFindClose)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("FindClose").crypt_get());
	g_winapiApiTable->LookupPrivilegeValueA = (lpLookupPrivilegeValueA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("LookupPrivilegeValueA").crypt_get());
	g_winapiApiTable->LookupPrivilegeValueW = (lpLookupPrivilegeValueW)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("LookupPrivilegeValueW").crypt_get());
	g_winapiApiTable->AdjustTokenPrivileges = (lpAdjustTokenPrivileges)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("AdjustTokenPrivileges").crypt_get());
	g_winapiApiTable->OpenProcessToken = (lpOpenProcessToken)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("OpenProcessToken").crypt_get());
	g_winapiApiTable->NtSetDebugFilterState = (lpNtSetDebugFilterState)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtSetDebugFilterState").crypt_get());
	g_winapiApiTable->WNetGetProviderNameA = (lpWNetGetProviderNameA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hMpr, xorstr("WNetGetProviderNameA").crypt_get());
	g_winapiApiTable->NtTerminateProcess = (lpNtTerminateProcess)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtTerminateProcess").crypt_get());
	g_winapiApiTable->GetSystemInfo = (lpGetSystemInfo)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetSystemInfo").crypt_get());
	g_winapiApiTable->CreateFileMappingA = (lpCreateFileMappingA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("CreateFileMappingA").crypt_get());
	g_winapiApiTable->MapViewOfFile = (lpMapViewOfFile)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("MapViewOfFile").crypt_get());
	g_winapiApiTable->UnmapViewOfFile = (lpUnmapViewOfFile)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("UnmapViewOfFile").crypt_get());
	g_winapiApiTable->ReadProcessMemory = (lpReadProcessMemory)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("ReadProcessMemory").crypt_get());
	g_winapiApiTable->AllocateAndInitializeSid = (lpAllocateAndInitializeSid)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("AllocateAndInitializeSid").crypt_get());
	g_winapiApiTable->GetTokenInformation = (lpGetTokenInformation)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("GetTokenInformation").crypt_get());
	g_winapiApiTable->GlobalAlloc = (lpGlobalAlloc)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GlobalAlloc").crypt_get());
	g_winapiApiTable->InitializeAcl = (lpInitializeAcl)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("InitializeAcl").crypt_get());
	g_winapiApiTable->AddAccessDeniedAce = (lpAddAccessDeniedAce)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("AddAccessDeniedAce").crypt_get());
	g_winapiApiTable->AddAccessAllowedAce = (lpAddAccessAllowedAce)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("AddAccessAllowedAce").crypt_get());
	g_winapiApiTable->SetSecurityInfo = (lpSetSecurityInfo)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("SetSecurityInfo").crypt_get());
	g_winapiApiTable->FreeSid = (lpFreeSid)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("FreeSid").crypt_get());
	g_winapiApiTable->GetForegroundWindow = (lpGetForegroundWindow)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("GetForegroundWindow").crypt_get());
	g_winapiApiTable->TerminateThread = (lpTerminateThread)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("TerminateThread").crypt_get());
	g_winapiApiTable->SendMessageA = (lpSendMessageA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("SendMessageA").crypt_get());
	g_winapiApiTable->SetThreadContext = (lpSetThreadContext)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("SetThreadContext").crypt_get());
	g_winapiApiTable->SuspendThread = (lpSuspendThread)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("SuspendThread").crypt_get());
	g_winapiApiTable->BlockInput = (lpBlockInput)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("BlockInput").crypt_get());
	g_winapiApiTable->GetWindowModuleFileNameA = (lpGetWindowModuleFileNameA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("GetWindowModuleFileNameA").crypt_get());
	g_winapiApiTable->RemoveVectoredExceptionHandler = (lpRemoveVectoredExceptionHandler)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("RemoveVectoredExceptionHandler").crypt_get());
	g_winapiApiTable->NtQueryVirtualMemory = (lpNtQueryVirtualMemory)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtQueryVirtualMemory").crypt_get());
	g_winapiApiTable->PeekMessageA = (lpPeekMessageA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("PeekMessageA").crypt_get());
	g_winapiApiTable->GetMessageA = (lpGetMessageA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("GetMessageA").crypt_get());
	g_winapiApiTable->TranslateMessage = (lpTranslateMessage)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("TranslateMessage").crypt_get());
	g_winapiApiTable->DispatchMessageA = (lpDispatchMessageA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("DispatchMessageA").crypt_get());
	g_winapiApiTable->UnhookWindowsHookEx = (lpUnhookWindowsHookEx)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("UnhookWindowsHookEx").crypt_get());
	g_winapiApiTable->SHGetSpecialFolderPathA = (lpSHGetSpecialFolderPathA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hShell32, xorstr("SHGetSpecialFolderPathA").crypt_get());
	g_winapiApiTable->FreeLibraryAndExitThread = (lpFreeLibraryAndExitThread)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("FreeLibraryAndExitThread").crypt_get());
	g_winapiApiTable->NtUnmapViewOfSection = (lpNtUnmapViewOfSection)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtUnmapViewOfSection").crypt_get());
	g_winapiApiTable->EndTask = (lpEndTask)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("EndTask").crypt_get());
	g_winapiApiTable->VirtualAlloc = (lpVirtualAlloc)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("VirtualAlloc").crypt_get());
	g_winapiApiTable->DebugBreak = (lpDebugBreak)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("DebugBreak").crypt_get());
	g_winapiApiTable->GetModuleHandleExA = (lpGetModuleHandleExA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetModuleHandleExA").crypt_get());
	g_winapiApiTable->ReadFile = (lpReadFile)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("ReadFile").crypt_get());
	g_winapiApiTable->NtSetInformationProcess = (lpNtSetInformationProcess)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtSetInformationProcess").crypt_get());
	g_winapiApiTable->NtAllocateVirtualMemory = (lpNtAllocateVirtualMemory)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtAllocateVirtualMemory").crypt_get());
	g_winapiApiTable->GetShellWindow = (lpGetShellWindow)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("GetShellWindow").crypt_get());
	g_winapiApiTable->CreateFileW = (lpCreateFileW)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("CreateFileW").crypt_get());
	g_winapiApiTable->CryptCATAdminCalcHashFromFileHandle = (lpCryptCATAdminCalcHashFromFileHandle)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hWintrust, xorstr("CryptCATAdminCalcHashFromFileHandle").crypt_get());
	g_winapiApiTable->CryptCATAdminAcquireContext = (lpCryptCATAdminAcquireContext)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hWintrust, xorstr("CryptCATAdminAcquireContext").crypt_get());
	g_winapiApiTable->CryptCATAdminEnumCatalogFromHash = (lpCryptCATAdminEnumCatalogFromHash)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hWintrust, xorstr("CryptCATAdminEnumCatalogFromHash").crypt_get());
	g_winapiApiTable->CryptCATAdminReleaseContext = (lpCryptCATAdminReleaseContext)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hWintrust, xorstr("CryptCATAdminReleaseContext").crypt_get());
	g_winapiApiTable->CryptCATCatalogInfoFromContext = (lpCryptCATCatalogInfoFromContext)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hWintrust, xorstr("CryptCATCatalogInfoFromContext").crypt_get());
	g_winapiApiTable->EnumProcessModules = (lpEnumProcessModules)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hPsapi, xorstr("EnumProcessModules").crypt_get());
	g_winapiApiTable->GetModuleFileNameExA = (lpGetModuleFileNameExA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hPsapi, xorstr("GetModuleFileNameExA").crypt_get());
	g_winapiApiTable->RtlGetVersion = (lpRtlGetVersion)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlGetVersion").crypt_get());
	g_winapiApiTable->VerifyVersionInfoW = (lpVerifyVersionInfoW)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("VerifyVersionInfoW").crypt_get());
	g_winapiApiTable->VerSetConditionMask = (lpVerSetConditionMask)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("VerSetConditionMask").crypt_get());
	g_winapiApiTable->GetWindowLongA = (lpGetWindowLongA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("GetWindowLongA").crypt_get());
	g_winapiApiTable->InitializeCriticalSection = (lpInitializeCriticalSection)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("InitializeCriticalSection").crypt_get());
	g_winapiApiTable->EnterCriticalSection = (lpEnterCriticalSection)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("EnterCriticalSection").crypt_get());
	g_winapiApiTable->LeaveCriticalSection = (lpLeaveCriticalSection)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("LeaveCriticalSection").crypt_get());
	g_winapiApiTable->DeleteCriticalSection = (lpDeleteCriticalSection)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("DeleteCriticalSection").crypt_get());
	g_winapiApiTable->GetProcessImageFileNameA = (lpGetProcessImageFileNameA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hPsapi, xorstr("GetProcessImageFileNameA").crypt_get());
	g_winapiApiTable->GetLogicalDriveStringsA = (lpGetLogicalDriveStringsA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetLogicalDriveStringsA").crypt_get());
	g_winapiApiTable->GetSystemMetrics = (lpGetSystemMetrics)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("GetSystemMetrics").crypt_get());
	g_winapiApiTable->RtlAdjustPrivilege = (lpRtlAdjustPrivilege)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlAdjustPrivilege").crypt_get());

	g_winapiApiTable->EnumProcesses = (lpEnumProcesses)g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hKernel32, xorstr("EnumProcesses").crypt_get());
	if (!g_winapiApiTable->EnumProcesses)
		g_winapiApiTable->EnumProcesses = (lpEnumProcesses)g_winapiApiTable->GetProcAddress_o(g_winapiModuleTable->hPsapi, xorstr("EnumProcesses").crypt_get());

}

