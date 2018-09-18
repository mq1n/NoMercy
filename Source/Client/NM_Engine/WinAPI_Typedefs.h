#pragma once
#include "Windows_Structs.h"

typedef FARPROC(WINAPI* lpGetProcAddress)(_In_ HMODULE hModule, _In_ LPCSTR  lpProcName);
typedef HMODULE(WINAPI* lpGetModuleHandleA)(_In_opt_ LPCTSTR lpModuleName);
typedef HMODULE(WINAPI* lpLoadLibraryA)(_In_opt_ LPCSTR lpFileName);
typedef BOOL(WINAPI* lpIsDebuggerPresent)(void);
typedef BOOL(WINAPI* lpAllocConsole)(void);
typedef int(WINAPI* lplstrcmpA)(_In_ LPCTSTR lpString1, _In_ LPCTSTR lpString2);
typedef UINT(WINAPI* lpGetWindowsDirectoryA)(_Out_ LPTSTR lpBuffer, _In_  UINT uSize);
typedef VOID(WINAPI* lpSleep)(_In_ DWORD dwMilliseconds);
typedef NTSTATUS(NTAPI* lpNtWriteVirtualMemory)(HANDLE, PVOID, CONST VOID *, SIZE_T, PSIZE_T);
typedef int (WINAPI* lpWinStationTerminateProcess)(HANDLE hServer, DWORD dwProcessId, UINT uExitCode);
typedef void(WINAPI* lpOutputDebugString)(_In_ LPCSTR str);
typedef HANDLE(WINAPI* lpCreateToolhelp32Snapshot)(_In_ DWORD dwFlags, _In_ DWORD th32ProcessID);
typedef BOOL(WINAPI* lpProcess32First)(_In_ HANDLE hSnapshot, _Inout_ LPPROCESSENTRY32 lppe);
typedef BOOL(WINAPI* lpProcess32Next)(_In_ HANDLE hSnapshot, _Out_ LPPROCESSENTRY32 lppe);
typedef BOOL(WINAPI* lpModule32First)(_In_ HANDLE hSnapshot, _Inout_ LPMODULEENTRY32 lpme);
typedef BOOL(WINAPI* lpModule32Next)(_In_ HANDLE hSnapshot, _Out_ LPMODULEENTRY32 lpme);
typedef DWORD(WINAPI* lpCharUpperBuffA)(_Inout_ LPTSTR lpsz, _In_ DWORD cchLength);
typedef int(WINAPI* lpMessageBoxA)(HWND wind, LPCSTR msg, LPCSTR title, UINT type);
typedef NTSTATUS(WINAPI* lpNtQueryInformationProcess)(HANDLE a, PROCESSINFOCLASS b, PVOID c, ULONG d, PULONG e);
typedef NTSTATUS(WINAPI* lpNtQueryInformationThread)(HANDLE, LONG, PVOID, ULONG, PULONG);
typedef NTSTATUS(NTAPI* lpNtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
typedef LONG(WINAPI* lpZwSetInformationThread)(HANDLE, DWORD, PVOID, ULONG);
typedef BOOL(WINAPI* lpCheckRemoteDebuggerPresent)(HANDLE a, PBOOL b);
typedef VOID(WINAPI* lpExitThread)(DWORD dwExitCode);
typedef DWORD(WINAPI* lpGetCurrentProcessId)(void);
typedef HANDLE(WINAPI* lpGetCurrenThread)(void);
typedef BOOL(WINAPI* lpVirtualFree)(_In_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD  dwFreeType);
typedef BOOL(WINAPI* lpVirtualProtect)(_In_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flNewProtect, _Out_ PDWORD lpflOldProtect);
typedef LPVOID(WINAPI* lpVirtualAlloc)(_In_opt_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flAllocationType, _In_ DWORD flProtect);
typedef HANDLE(WINAPI* lpCreateFile)(__in LPCSTR lpFileName, __in DWORD dwDesiredAccess, __in DWORD dwShareMode, __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	__in DWORD dwCreationDisposition, __in DWORD dwFlagsAndAttributes, __in_opt HANDLE hTemplateFile);
typedef LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI* lpSetUnhandledExceptionFilter)(_In_ LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
typedef LPVOID(WINAPI* lpCreateThread)(LPSECURITY_ATTRIBUTES  lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
typedef BOOL(WINAPI* lpGetVersionEx)(_Inout_ LPOSVERSIONINFO lpVersionInfo);
typedef BOOL(WINAPI* lpWriteProcessMemory)(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten);
typedef DWORD(WINAPI* lpGetAdaptersInfo)(_Out_ PIP_ADAPTER_INFO pAdapterInfo, _Inout_ PULONG pOutBufLen);
typedef BOOL(WINAPI* lpGetComputerName)(_Out_ LPTSTR lpBuffer, _Inout_ LPDWORD lpnSize);
typedef BOOL(WINAPI* lpGetVolumeInformation)(LPCTSTR lpRootPathName, LPTSTR  lpVolumeNameBuffer, DWORD   nVolumeNameSize, LPDWORD lpVolumeSerialNumber,
	LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPTSTR  lpFileSystemNameBuffer, DWORD   nFileSystemNameSize);
typedef UINT(WINAPI* lpGetDriveType)(_In_opt_ LPCTSTR lpRootPathName);
typedef HLOCAL(WINAPI* lpLocalAlloc)(_In_ UINT uFlags, _In_ SIZE_T uBytes);
typedef DWORD(WINAPI* lpGetModuleFileName)(_In_opt_ HMODULE hModule, _Out_ LPTSTR lpFilename, _In_ DWORD nSize);
typedef BOOL(WINAPI* lpGetThreadContext)(_In_ HANDLE hThread, _Inout_ LPCONTEXT lpContext);
typedef DWORD(WINAPI* lpWaitForSingleObject)(_In_ HANDLE hHandle, _In_ DWORD  dwMilliseconds);
typedef DWORD(WINAPI* lpResumeThread)(_In_ HANDLE hThread);
typedef BOOL(WINAPI* lpCloseHandle)(_In_ HANDLE hObject);
typedef DWORD(WINAPI* lpGetDeviceDriverBaseName)(_In_  LPVOID ImageBase, _Out_ LPTSTR lpBaseName, _In_  DWORD  nSize);
typedef BOOL(WINAPI* lpEnumDeviceDrivers)(_Out_ LPVOID  *lpImageBase, _In_  DWORD   cb, _Out_ LPDWORD lpcbNeeded);
typedef DWORD(WINAPI* lpGetTickCount)(void);
typedef BOOL(WINAPI* lpEnumWindows)(_In_ WNDENUMPROC lpEnumFunc, _In_ LPARAM      lParam);
typedef DWORD(WINAPI* lpGetLastError)(void);
typedef BOOL(WINAPI* lpGetUserName)(_Out_   LPTSTR  lpBuffer, _Inout_ LPDWORD lpnSize);
typedef HINSTANCE(WINAPI* lpShellExecute)(_In_opt_ HWND    hwnd, _In_opt_ LPCTSTR lpOperation, _In_     LPCTSTR lpFile,
	_In_opt_ LPCTSTR lpParameters, _In_opt_ LPCTSTR lpDirectory, _In_     INT     nShowCmd);
typedef DWORD(WINAPI* lpGetModuleBaseNameA)(_In_     HANDLE  hProcess, _In_opt_ HMODULE hModule, _Out_    LPTSTR  lpBaseName, _In_     DWORD   nSize);
typedef void(WINAPI* lpSetLastError)(_In_ DWORD dwErrCode);
typedef LRESULT(WINAPI* lpCallNextHookEx)(_In_opt_ HHOOK  hhk, _In_     int    nCode, _In_     WPARAM wParam, _In_     LPARAM lParam);
typedef HHOOK(WINAPI* lpSetWindowsHookEx)(_In_ int       idHook, _In_ HOOKPROC  lpfn, __in_opt HINSTANCE hMod, _In_ DWORD     dwThreadId);
typedef NTSTATUS(WINAPI* lpNtResumeProcess)(HANDLE ProcessHandle);
typedef PVOID(WINAPI* lpAddVectoredExceptionHandler)(_In_ ULONG FirstHandler, _In_ PVECTORED_EXCEPTION_HANDLER VectoredHandler);
typedef SIZE_T(WINAPI* lpVirtualQuery)(_In_opt_ LPCVOID lpAddress, _Out_ PMEMORY_BASIC_INFORMATION lpBuffer, _In_ SIZE_T dwLength);
typedef int(WINAPI* lpMessageBoxTimeout)(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
typedef NTSTATUS(NTAPI* lpNtClose)(HANDLE);
typedef NTSTATUS(NTAPI* lpNtSetInformationDebugObject)(HANDLE DebugHandle, DEBUGOBJECTINFOCLASS DebugObjectInformationClass, PVOID DebugInformation, ULONG DebugInformationLength, PULONG ReturnLength);
typedef NTSTATUS(NTAPI* lpNtRemoveProcessDebug)(HANDLE ProcessHandle, HANDLE DebugHandle);
typedef DWORD(WINAPI* lpFormatMessage)(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPSTR lpBuffer, DWORD nSize, va_list *Arguments);
typedef int(WINAPI* lpMultiByteToWideChar)(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
typedef int(WINAPI* lpWideCharToMultiByte)(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
typedef BOOL(WINAPI* lpCreateProcessA)(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
typedef VOID(WINAPI* lpGetStartupInfoA)(LPSTARTUPINFOA lpStartupInfo);
typedef BOOL(WINAPI* lpGetExitCodeProcess)(_In_  HANDLE  hProcess, _Out_ LPDWORD lpExitCode);
typedef VOID(WINAPI* lpPostQuitMessage)(_In_ int nExitCode);
typedef HANDLE(WINAPI* lpOpenProcess)(_In_ DWORD dwDesiredAccess, _In_ BOOL  bInheritHandle, _In_ DWORD dwProcessId);
typedef DWORD(WINAPI* lpGetCurrentThreadId)(VOID);
typedef BOOL(WINAPI* lpGetModuleInformation)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb);
typedef DWORD(WINAPI* lpGetMappedfilename)(HANDLE hProcess, LPVOID lpv, LPSTR lpFilename, DWORD nSize);
typedef BOOL(WINAPI* lpGetModuleHandleExW)(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule);
typedef BOOL(WINAPI* lpFreeLibrary)(HMODULE hLibModule);
typedef HMODULE(WINAPI* lpGetModuleHandleW)(LPCWSTR lpModuleName);
typedef NTSTATUS(NTAPI *lpNtCreateThreadEx)(PHANDLE hThread, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, ULONG CreateFlags, ULONG_PTR StackZeroBits, SIZE_T SizeOfStackCommit, SIZE_T SizeOfStackReserve, LPVOID AttributeList);
typedef LRESULT(WINAPI* lpCallWindowProcA)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
typedef HWND(WINAPI* lpFindWindowExA)(HWND hWndParent, HWND hWndChildAfter, LPCSTR lpszClass, LPCSTR lpszWindow);
typedef DWORD(WINAPI* lpGetWindowThreadProcessId)(HWND hWnd, LPDWORD lpdwProcessId);
typedef BOOL(WINAPI* lpIsWindowVisible)(__in HWND hWnd);
typedef int(WINAPI* lpGetClassNameA)(HWND hWnd, LPSTR lpClassName, int nMaxCount);
typedef int(WINAPI* lpGetWindowTextA)(HWND hWnd, LPSTR lpString, int nMaxCount);
typedef LONG(WINAPI* lpSetWindowLongA)(HWND hWnd, int nIndex, LONG dwNewLong);
typedef BOOL(WINAPI* lpThread32First)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
typedef BOOL(WINAPI* lpThread32Next)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
typedef HANDLE(WINAPI* lpOpenThread)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);
typedef NTSTATUS(NTAPI* lpNtDuplicateObject)(HANDLE SourceProcessHandle, HANDLE SourceHandle, HANDLE TargetProcessHandle, PHANDLE TargetHandle, ACCESS_MASK DesiredAccess,
	ULONG Attributes, ULONG Options);
typedef NTSTATUS(NTAPI* lpNtQueryObject)(HANDLE ObjectHandle, ULONG ObjectInformationClass, PVOID ObjectInformation, ULONG ObjectInformationLength, PULONG ReturnLength);
typedef DWORD(WINAPI* lpGetFileSize)(HANDLE hFile, LPDWORD lpFileSizeHigh);
typedef HANDLE(WINAPI* lpFindFirstFileA)(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
typedef BOOL(WINAPI* lpFindNextFileA)(HANDLE hFindFile, PWIN32_FIND_DATAA lpFindFileData);
typedef BOOL(WINAPI* lpSetFileAttributesA)(LPCSTR lpFileName, DWORD dwFileAttributes);
typedef BOOL(WINAPI* lpRemoveDirectoryA)(LPCSTR lpPathName);
typedef BOOL(WINAPI* lpDeleteFileA)(LPCSTR lpFileName);
typedef DWORD(WINAPI* lpGetFileAttributesA)(LPCSTR lpFileName);
typedef BOOL(WINAPI* lpFindClose)(HANDLE hFindFile);
typedef BOOL(WINAPI* lpLookupPrivilegeValueA)(LPCSTR lpSystemName, LPCSTR lpName, PLUID lpLuid);
typedef BOOL(WINAPI* lpAdjustTokenPrivileges)(HANDLE TokenHandle, BOOL DisableAllPrivileges, PTOKEN_PRIVILEGES NewState, DWORD BufferLength,
	PTOKEN_PRIVILEGES PreviousState, PDWORD ReturnLength);
typedef BOOL(WINAPI* lpOpenProcessToken)(HANDLE ProcessHandle, DWORD DesiredAccess, PHANDLE TokenHandle);
typedef NTSTATUS(NTAPI* lpNtSetDebugFilterState)(ULONG ComponentId, unsigned int Level, char State);
typedef NTSTATUS(NTAPI* lpZwQuerySystemInformation)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength OPTIONAL);
typedef DWORD(WINAPI* lpWNetGetProviderNameA)(DWORD dwNetType, LPSTR lpProviderName, LPDWORD lpBufferSize);
typedef NTSTATUS(NTAPI* lpNtTerminateProcess)(HANDLE ProcessHandle, NTSTATUS ExitStatus);
typedef VOID(WINAPI* lpGetSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
typedef HANDLE(WINAPI* lpCreateFileMappingA)(HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh,
	DWORD dwMaximumSizeLow, LPCSTR lpName);
typedef LPVOID(WINAPI* lpMapViewOfFile)(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap);
typedef BOOL(WINAPI* lpUnmapViewOfFile)(LPCVOID lpBaseAddress);
typedef BOOL(WINAPI* lpReadProcessMemory)(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T * lpNumberOfBytesRead);
typedef BOOL(WINAPI* lpAllocateAndInitializeSid)(PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority, BYTE nSubAuthorityCount, DWORD nSubAuthority0,
	DWORD nSubAuthority1, DWORD nSubAuthority2, DWORD nSubAuthority3, DWORD nSubAuthority4, DWORD nSubAuthority5, DWORD nSubAuthority6,
	DWORD nSubAuthority7, PSID *pSid);
typedef BOOL(WINAPI* lpGetTokenInformation)(HANDLE TokenHandle, TOKEN_INFORMATION_CLASS TokenInformationClass, LPVOID TokenInformation, DWORD TokenInformationLength,
	PDWORD ReturnLength);
typedef HGLOBAL(WINAPI* lpGlobalAlloc)(UINT uFlags, SIZE_T dwBytes);
typedef BOOL(WINAPI* lpInitializeAcl)(PACL pAcl, DWORD nAclLength, DWORD dwAclRevision);
typedef BOOL(WINAPI* lpAddAccessDeniedAce)(PACL pAcl, DWORD dwAceRevision, DWORD AccessMask, PSID pSid);
typedef BOOL(WINAPI* lpAddAccessAllowedAce)(PACL pAcl, DWORD dwAceRevision, DWORD AccessMask, PSID pSid);
typedef DWORD(WINAPI* lpSetSecurityInfo)(HANDLE handle, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION  SecurityInfo, PSID psidOwner, PSID psidGroup,
	PACL pDacl, PACL pSacl);
typedef PVOID(WINAPI* lpFreeSid)(PSID pSid);
typedef BOOL(WINAPI* lpQueryFullProcessImageNameA)(HANDLE hProcess, DWORD dwFlags, LPSTR lpExeName, PDWORD lpdwSize);
typedef HWND(WINAPI* lpGetForegroundWindow)(VOID);
typedef BOOL(WINAPI* lpTerminateThread)(HANDLE hThread, DWORD dwExitCode);
typedef LRESULT(WINAPI* lpSendMessageA)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
typedef BOOL(WINAPI* lpSetThreadContext)(HANDLE hThread, CONST CONTEXT *lpContext);
typedef DWORD(WINAPI* lpSuspendThread)(HANDLE hThread);
typedef DWORD(WINAPI* lpGetThreadId)(HANDLE hThread);
typedef BOOL(WINAPI* lpBlockInput)(BOOL fBlockIt);
typedef UINT(WINAPI* lpGetWindowModuleFileNameA)(HWND hwnd, LPTSTR lpszFileName, UINT cchFileNameMax);
typedef ULONG(WINAPI* lpRemoveVectoredExceptionHandler)(PVOID Handle);
typedef NTSTATUS(NTAPI* lpNtQueryVirtualMemory)(HANDLE, PVOID, MEMORY_INFORMATION_CLASS, PVOID, ULONG, PULONG);
typedef BOOL(WINAPI* lpPeekMessageA)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
typedef BOOL(WINAPI* lpGetMessageA)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
typedef BOOL(WINAPI* lpTranslateMessage)(CONST MSG *lpMsg);
typedef LRESULT(WINAPI* lpDispatchMessageA)(CONST MSG *lpMsg);
typedef BOOL(WINAPI* lpUnhookWindowsHookEx)(HHOOK hhk);
typedef BOOL(WINAPI* lpSHGetSpecialFolderPathA)(HWND hwndOwner, LPTSTR lpszPath, int csidl, BOOL fCreate);
typedef VOID(WINAPI* lpFreeLibraryAndExitThread)(HMODULE hLibModule, DWORD dwExitCode);
typedef LONG(WINAPI* lpNtUnmapViewOfSection)(HANDLE ProcessHandle, PVOID BaseAddress);
typedef BOOL(WINAPI* lpEndTask)(HWND, BOOL, BOOL);
typedef void(WINAPI* lpDebugBreak)(void);
typedef BOOL(WINAPI* lpGetModuleHandleExA)(DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule);
typedef BOOL(WINAPI* lpReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
typedef NTSTATUS(WINAPI* lpLdrGetDllFullName)(int handle, PUNICODE_STRING dest);
typedef NTSTATUS(NTAPI* lpNtSetInformationProcess)(HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength);
typedef NTSTATUS(NTAPI* lpNtAllocateVirtualMemory)(HANDLE ProcessHandle, PVOID *BaseAddress, ULONG_PTR ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect);
typedef HWND(WINAPI* lpGetShellWindow)(VOID);
typedef HANDLE(WINAPI* lpCreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef BOOL(WINAPI* lpCryptCATAdminCalcHashFromFileHandle)(HANDLE hFile, DWORD *pcbHash, BYTE *pbHash, DWORD dwFlags);
typedef BOOL(WINAPI* lpCryptCATAdminAcquireContext)(HCATADMIN *phCatAdmin, const GUID *pgSubsystem, DWORD dwFlags);
typedef HCATINFO(WINAPI* lpCryptCATAdminEnumCatalogFromHash)(HCATADMIN hCatAdmin, BYTE *pbHash, DWORD cbHash, DWORD dwFlags, HCATINFO *phPrevCatInfo);
typedef BOOL(WINAPI* lpCryptCATAdminReleaseContext)(HCATADMIN hCatAdmin, DWORD dwFlags);
typedef BOOL(WINAPI* lpCryptCATCatalogInfoFromContext)(HCATINFO hCatInfo, CATALOG_INFO *psCatInfo, DWORD dwFlags);
typedef BOOL(WINAPI* lpEnumProcessModules)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD(WINAPI* lpGetModuleFileNameExA)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize);
typedef NTSTATUS(NTAPI* lpRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
typedef BOOL(WINAPI* lpVerifyVersionInfoW)(LPOSVERSIONINFOEXW lpVersionInformation, DWORD dwTypeMask, DWORDLONG dwlConditionMask);
typedef ULONGLONG(NTAPI* lpVerSetConditionMask)(ULONGLONG ConditionMask, DWORD TypeMask, BYTE Condition);
typedef DWORD(NTAPI* lpCsrGetProcessId)();
typedef LONG(WINAPI* lpGetWindowLongA)(HWND hWnd, int nIndex);
typedef VOID(WINAPI* lpInitializeCriticalSection)(LPCRITICAL_SECTION lpCriticalSection);
typedef VOID(WINAPI* lpEnterCriticalSection)(LPCRITICAL_SECTION lpCriticalSection);
typedef VOID(WINAPI* lpLeaveCriticalSection)(LPCRITICAL_SECTION lpCriticalSection);
typedef VOID(WINAPI* lpDeleteCriticalSection)(LPCRITICAL_SECTION lpCriticalSection);
typedef DWORD(WINAPI* lpGetProcessId)(HANDLE Process);
typedef DWORD(WINAPI* lpGetProcessImageFileNameA)(HANDLE hProcess, LPTSTR lpImageFileName, DWORD nSize);
typedef DWORD(WINAPI* lpGetLogicalDriveStringsA)(DWORD nBufferLength, LPTSTR lpBuffer);
typedef int(WINAPI* lpGetSystemMetrics)(int nIndex);
typedef NTSTATUS(NTAPI* lpRtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
typedef BOOL(WINAPI* lpEnumProcesses)(DWORD *pProcessIds, DWORD cb, DWORD *pBytesReturned);
typedef BOOL(WINAPI* lpOpenThreadToken)(HANDLE ThreadHandle, DWORD DesiredAccess, BOOL OpenAsSelf, PHANDLE TokenHandle);
typedef DWORD(WINAPI* lpGetModuleFileNameW)(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
typedef DWORD(WINAPI* lpGetDeviceDriverFileNameA)(LPVOID ImageBase, LPSTR lpFilename, DWORD nSize);
typedef SC_HANDLE(WINAPI* lpOpenSCManagerA)(LPCSTR lpMachineName, LPCSTR lpDatabaseName, DWORD dwDesiredAccess);
typedef BOOL(WINAPI* lpEnumServicesStatusA)(SC_HANDLE hSCManager, DWORD dwServiceType, DWORD dwServiceState, LPENUM_SERVICE_STATUSA lpServices,
	DWORD cbBufSize, LPDWORD pcbBytesNeeded, LPDWORD lpServicesReturned, LPDWORD lpResumeHandle);
typedef BOOL(WINAPI* lpCloseServiceHandle)(SC_HANDLE hSCObject);
typedef BOOL(WINAPI* lpToolhelp32ReadProcessMemory)(DWORD th32ProcessID, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T cbRead, SIZE_T* lpNumberOfBytesRead);
typedef HANDLE(WINAPI* lpOpenEventA)(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName);
typedef HLOCAL(WINAPI* lpLocalFree)(HLOCAL hMem);
typedef DWORD(WINAPI* lpSetEntriesInAclA)(ULONG cCountOfExplicitEntries, PEXPLICIT_ACCESS pListOfExplicitEntries, PACL OldAcl, PACL *NewAcl);
typedef BOOL(WINAPI* lpSetPriorityClass)(HANDLE hProcess, DWORD dwPriorityClass);
typedef VOID(WINAPI* lpBuildExplicitAccessWithNameA)(PEXPLICIT_ACCESS pExplicitAccess, LPTSTR pTrusteeName, DWORD AccessPermissions, ACCESS_MODE AccessMode, DWORD Inheritance);
typedef BOOL(WINAPI* lpConvertStringSecurityDescriptorToSecurityDescriptorA)(LPCTSTR StringSecurityDescriptor, DWORD  StringSDRevision, PSECURITY_DESCRIPTOR *SecurityDescriptor, PULONG SecurityDescriptorSize);
typedef BOOL(WINAPI* lpSetKernelObjectSecurity)(HANDLE Handle, SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor);
typedef HRESULT(WINAPI* lpCoInitialize)(LPVOID pvReserved);
typedef void(WINAPI* lpCoUninitialize)(void);
typedef HINTERNET(WINAPI* lpInternetOpenA)(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET(WINAPI* lpInternetOpenUrlA)(HINTERNET hInternet, LPCSTR lpszUrl, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwFlags, DWORD_PTR dwContext);
typedef BOOL(WINAPI* lpHttpQueryInfoA)(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
typedef BOOL(WINAPI* lpInternetCloseHandle)(HINTERNET hInternet);
typedef BOOL(WINAPI* lpInternetReadFile)(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
typedef int(WINAPI* lpWSAStartup)(WORD wVersionRequested, LPWSADATA2 lpWSAData);
typedef struct hostent*(WINAPI* lpgethostbyname)(const char *name);
typedef int(WINAPI* lpWSACleanup)(void);
typedef int(WINAPI* lpWSAGetLastError)(void);
typedef char*(WINAPI* lpinet_ntoa)(struct in_addr in);
typedef BOOL(WINAPI* lpGetFileTime)(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);
typedef BOOL(WINAPI* lpGetWindowRect)(HWND hWnd, LPRECT lpRect);
typedef HWND(WINAPI* lpGetDesktopWindow)(void);
typedef HDC(WINAPI* lpGetDC)(HWND hWnd);
typedef HDC(WINAPI* lpCreateCompatibleDC)(HDC hdc);
typedef BOOL(WINAPI* lpBitBlt)(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
typedef HBITMAP(WINAPI* lpCreateCompatibleBitmap)(HDC hdc, int nWidth, int nHeight);
typedef HGDIOBJ(WINAPI* lpSelectObject)(HDC hdc, HGDIOBJ hgdiobj);
typedef BOOL(WINAPI* lpCheckTokenMembership)(HANDLE TokenHandle, PSID SidToCheck, PBOOL IsMember);
typedef DWORD(WINAPI* lpGetTempPathA)(DWORD nBufferLength, LPSTR lpBuffer);
typedef UINT(WINAPI* lpGetTempFileNameA)(LPCSTR lpPathName, LPCSTR lpPrefixString, UINT uUnique, LPSTR lpTempFileName);
typedef BOOL(WINAPI* lpQueryWorkingSet)(HANDLE hProcess, PVOID pv, DWORD cb);
typedef IMAGE_NT_HEADERS*(NTAPI* lpRtlImageNtHeader)(unsigned long ImageBase);
typedef NTSTATUS(NTAPI* lpZwOpenDirectoryObject)(PHANDLE DirectoryHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes);
typedef NTSTATUS(NTAPI* lpZwClose)(HANDLE Handle);
typedef NTSTATUS(NTAPI* lpZwQueryDirectoryObject)(HANDLE DirectoryHandle, PVOID Buffer, ULONG BufferLength, BOOLEAN ReturnSingleEntry, BOOLEAN RestartScan, PULONG Context, PULONG ReturnLength);
typedef VOID(NTAPI* lpRtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
typedef BOOL(WINAPI* lpDeviceIoControl)(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer,
	DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
typedef BOOL(WINAPI* lpSetProcessMitigationPolicy)(PROCESS_MITIGATION_POLICY MitigationPolicy, PVOID lpBuffer, SIZE_T dwLength);
typedef int(WINAPI* lpGetThreadPriority)(HANDLE hThread);
typedef BOOL(WINAPI* lpSetThreadPriority)(HANDLE hThread, int nPriority);
typedef HINTERNET(WINAPI* lpInternetConnectA)(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword,
	DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
typedef HINTERNET(WINAPI* lpHttpOpenRequestA)(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer,
	LPCSTR FAR * lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
typedef BOOL(WINAPI* lpHttpAddRequestHeadersA)(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwModifiers);
typedef BOOL(WINAPI* lpHttpSendRequestExA)(HINTERNET hRequest, LPINTERNET_BUFFERSA lpBuffersIn, LPINTERNET_BUFFERSA lpBuffersOut, DWORD dwFlags, DWORD_PTR dwContext);
typedef BOOL(WINAPI* lpInternetWriteFile)(HINTERNET hFile, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten);
typedef BOOL(WINAPI* lpHttpEndRequestA)(HINTERNET hRequest, LPINTERNET_BUFFERSA lpBuffersOut, DWORD dwFlags, DWORD_PTR dwContext);
typedef NTSTATUS(NTAPI* lpNtProtectVirtualMemory)(IN HANDLE ProcessHandle, IN PVOID *BaseAddress, IN SIZE_T *NumberOfBytesToProtect, IN ULONG NewAccessProtection, OUT PULONG OldAccessProtection);
typedef BOOL(WINAPI* lpVirtualProtectEx)(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
typedef HRESULT(WINAPI* lpCoInitializeEx)(LPVOID pvReserved, DWORD  dwCoInit);
typedef HRESULT(WINAPI* lpCoInitializeSecurity)(PSECURITY_DESCRIPTOR pSecDesc, LONG cAuthSvc, SOLE_AUTHENTICATION_SERVICE *asAuthSvc, void *pReserved1,
	DWORD dwAuthnLevel, DWORD dwImpLevel, void *pAuthList, DWORD dwCapabilities, void *pReserved3);
typedef HRESULT(WINAPI* lpCoCreateInstance)(REFCLSID  rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv);
typedef HRESULT(WINAPI* lpCoSetProxyBlanket)(IUnknown *pProxy, DWORD dwAuthnSvc, DWORD dwAuthzSvc, OLECHAR *pServerPrincName, DWORD dwAuthnLevel,
	DWORD dwImpLevel, RPC_AUTH_IDENTITY_HANDLE pAuthInfo, DWORD dwCapabilities);
typedef UINT_PTR(WINAPI* lpSetTimer)(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc);
typedef BOOL(WINAPI* lpKillTimer)(HWND hWnd, UINT_PTR uIDEvent);
typedef DWORD(NTAPI* lpRtlComputeCrc32)(DWORD dwInitial, const BYTE* pData, int iLen);
typedef NTSTATUS(NTAPI* lpNtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
	PVOID* Parameters, HARDERROR_RESPONSE_OPTION ResponseOption, PHARDERROR_RESPONSE Response);
typedef HWINEVENTHOOK(WINAPI* lpSetWinEventHook)(DWORD eventMin, DWORD eventMax, HMODULE hmodWinEventProc, WINEVENTPROC pfnWinEventProc,
	DWORD idProcess, DWORD idThread, DWORD dwFlags);
typedef BOOL(WINAPI* lpGetWindowInfo)(HWND hwnd, PWINDOWINFO pwi);
typedef LPWSTR*(WINAPI* lpCommandLineToArgvW)(LPCWSTR lpCmdLine, int* pNumArgs);
typedef LPWSTR(WINAPI* lpGetCommandLineW)(VOID);
typedef NTSTATUS(NTAPI* lpNtGetNextThread)(HANDLE ProcessHandle, HANDLE ThreadHandle, ACCESS_MASK DesiredAccess, ULONG HandleAttributes, ULONG Flags, PHANDLE NewThreadHandle);
typedef NTSTATUS(NTAPI* lpNtGetNextProcess)(HANDLE ProcessHandle, ACCESS_MASK DesiredAccess, ULONG HandleAttributes, ULONG Flags, PHANDLE NewProcessHandle);
typedef BOOL(WINAPI* lpProcessIdToSessionId)(DWORD dwProcessId, DWORD *pSessionId);
typedef HANDLE(WINAPI* lpCreateMutexA)(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCTSTR lpName);
typedef BOOL(WINAPI* lpSetHandleInformation)(HANDLE hObject, DWORD dwMask, DWORD dwFlags);
typedef UINT(WINAPI* lpSetErrorMode)(UINT uMode);
typedef BOOL(WINAPI *lpIsWow64Process) (HANDLE, PBOOL);
typedef LSTATUS(WINAPI* lpRegOpenKeyExA)(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
typedef LSTATUS(WINAPI* lpRegQueryValueExA)(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
typedef LSTATUS(WINAPI* lpRegCloseKey)(HKEY hKey);
typedef NTSTATUS(NTAPI* lpNtSuspendProcess)(IN HANDLE ProcessHandle);
typedef NTSTATUS(NTAPI* lpNtResumeThread)(IN HANDLE ThreadHandle, OUT PULONG SuspendCount OPTIONAL);
typedef NTSTATUS(NTAPI* lpNtGetContextThread)(IN HANDLE ThreadHandle, OUT PCONTEXT pContext);
typedef NTSTATUS(NTAPI* lpNtSetContextThread)(IN HANDLE ThreadHandle, IN PCONTEXT ThreadContext);
typedef NTSTATUS(NTAPI* lpNtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded);
typedef NTSTATUS(NTAPI* lpNtWaitForSingleObject)(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout);
typedef LPVOID(WINAPI* lpVirtualAllocEx)(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
typedef HWND(WINAPI* lpGetConsoleWindow)(VOID);
typedef BOOL(WINAPI* lpShowWindow)(HWND hWnd, int nCmdShow);
typedef BOOL(WINAPI* lpQueryWorkingSetEx)(HANDLE hProcess, PVOID pv, DWORD cb);
typedef NTSTATUS(NTAPI* lpNtAdjustPrivilegesToken)(HANDLE TokenHandle, BOOLEAN DisableAllPrivileges, PTOKEN_PRIVILEGES NewState, ULONG BufferLength, PTOKEN_PRIVILEGES PreviousState, PULONG ReturnLength);
typedef NTSTATUS(NTAPI* lpNtOpenProcessToken)(HANDLE ProcessHandle, ACCESS_MASK DesiredAccess, PHANDLE TokenHandle);
typedef HANDLE(WINAPI* lpOpenMutexA)(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName);
typedef BOOL(WINAPI* lpInternetCheckConnectionA)(LPCSTR lpszUrl, DWORD dwFlags, DWORD dwReserved);
typedef BOOL(WINAPI* lpFtpPutFileA)(HINTERNET hConnect, LPCTSTR lpszLocalFile, LPCTSTR lpszNewRemoteFile, DWORD dwFlags, DWORD_PTR dwContext);
typedef VOID(WINAPI* lpGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
typedef HWND(WINAPI* lpGetWindow)(HWND hWnd, UINT uCmd);
typedef EVT_HANDLE(WINAPI* lpEvtCreateRenderContext)(DWORD ValuePathsCount, LPCWSTR* ValuePaths, DWORD Flags);
typedef EVT_HANDLE(WINAPI* lpEvtQuery)(EVT_HANDLE Session, LPCWSTR Path, LPCWSTR Query, DWORD Flags);
typedef BOOL(WINAPI* lpEvtNext)(EVT_HANDLE ResultSet, DWORD EventsSize, PEVT_HANDLE Events, DWORD Timeout, DWORD Flags, PDWORD Returned);
typedef BOOL(WINAPI* lpEvtRender)(EVT_HANDLE Context, EVT_HANDLE Fragment, DWORD Flags, DWORD BufferSize, PVOID Buffer,
	PDWORD BufferUsed, PDWORD PropertyCount);
typedef EVT_HANDLE(WINAPI* lpEvtOpenPublisherMetadata)(EVT_HANDLE Session, LPCWSTR PublisherId, LPCWSTR LogFilePath, LCID Locale, DWORD Flags);
typedef BOOL(WINAPI* lpEvtFormatMessage)(EVT_HANDLE PublisherMetadata, EVT_HANDLE Event, DWORD MessageId, DWORD ValueCount,
	PEVT_VARIANT Values, DWORD Flags, DWORD BufferSize, LPWSTR Buffer, PDWORD BufferUsed);
typedef BOOL(WINAPI* lpEvtClose)(EVT_HANDLE Object);
typedef HGLOBAL(WINAPI* lpGlobalFree)(HGLOBAL hMem);
typedef PVOID(NTAPI* lpRtlAllocateHeap)(__in PVOID HeapHandle, __in_opt ULONG Flags, __in SIZE_T Size);
typedef BOOLEAN(NTAPI* lpRtlFreeHeap)(__in PVOID HeapHandle, __in_opt ULONG Flags, __in PVOID HeapBase);
typedef NTSTATUS(NTAPI* lpLdrLoadDll)(PWCHAR PathToFile, ULONG *Flags, UNICODE_STRING *ModuleFileName, HMODULE *ModuleHandle);
typedef HANDLE(WINAPI* lpCreateRemoteThread)(HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, 
	LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
typedef SIZE_T(WINAPI* lpVirtualQueryEx)(HANDLE hProcess, LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
typedef NTSTATUS(NTAPI* lpNtCreateDebugObject)(PHANDLE DebugObjectHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, ULONG Flags);
typedef NTSTATUS(NTAPI* lpNtDebugActiveProcess)(HANDLE ProcessHandle, HANDLE DebugObjectHandle);
typedef void(WINAPI* lpDnsApiFree)(PVOID pData);
typedef int(WINAPI* lpDnsGetCacheDataTable)(PDNS_CACHE_ENTRY);
typedef BOOLEAN(WINAPI* lpRtlAddFunctionTable)(PRUNTIME_FUNCTION FunctionTable, DWORD EntryCount, DWORD_PTR BaseAddress);
typedef BOOL(WINAPI* lpDLLMain)(HMODULE, DWORD, PVOID);
typedef NTSTATUS(NTAPI* lpRtlAddAccessDeniedAce)(PACL	pAcl,DWORD	dwAceRevision,DWORD	AccessMask,PSID		pSid);
typedef PVOID(NTAPI* lpRtlImageDirectoryEntryToData)(PVOID ImageBase, BOOLEAN MappedAsImage, USHORT DirectoryEntry, PULONG Size);
typedef BOOL(WINAPI* lpWow64DisableWow64FsRedirection)(PVOID *OldValue);
typedef BOOL(WINAPI* lpWow64RevertWow64FsRedirection)(PVOID OldValue);
typedef PRTL_DEBUG_INFORMATION(NTAPI* lpRtlCreateQueryDebugBuffer)(IN ULONG  Size, IN BOOLEAN  EventPair);
typedef NTSTATUS(NTAPI* lpRtlQueryProcessDebugInformation)(IN ULONG  ProcessId, IN ULONG  DebugInfoClassMask, IN OUT PRTL_DEBUG_INFORMATION   DebugBuffer);
typedef NTSTATUS(NTAPI* lpRtlDestroyQueryDebugBuffer)(IN PRTL_DEBUG_INFORMATION   DebugBuffer);
typedef VOID(NTAPI* lpLdrShutdownProcess)(VOID);
typedef DWORD(WINAPI* lpPssCaptureSnapshot)(HANDLE ProcessHandle, PSS_CAPTURE_FLAGS CaptureFlags, DWORD ThreadContextFlags, HPSS* SnapshotHandle);
typedef DWORD(WINAPI* lpPssFreeSnapshot)(HANDLE ProcessHandle, HPSS SnapshotHandle);
typedef DWORD(WINAPI* lpPssQuerySnapshot)(HPSS SnapshotHandle, PSS_QUERY_INFORMATION_CLASS InformationClass, void* Buffer, DWORD BufferLength);
typedef DWORD(WINAPI* lpPssWalkSnapshot)(HPSS SnapshotHandle, PSS_WALK_INFORMATION_CLASS InformationClass, HPSSWALK WalkMarkerHandle, void* Buffer, DWORD BufferLength);
typedef DWORD(WINAPI* lpPssDuplicateSnapshot)(HANDLE SourceProcessHandle, HPSS SnapshotHandle, HANDLE TargetProcessHandle, HPSS* TargetSnapshotHandle, PSS_DUPLICATE_FLAGS Flags);
typedef DWORD(WINAPI* lpPssWalkMarkerCreate)(PSS_ALLOCATOR const *Allocator, HPSSWALK* WalkMarkerHandle);
typedef DWORD(WINAPI* lpPssWalkMarkerFree)(HPSSWALK WalkMarkerHandle);
typedef NTSTATUS(NTAPI* lpNtCreateSection)(PHANDLE SectionHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PLARGE_INTEGER MaximumSize,
	ULONG SectionPageProtection, ULONG AllocationAttributes, HANDLE FileHandle);
typedef NTSTATUS(NTAPI* lpNtMapViewOfSection)(HANDLE SectionHandle, HANDLE ProcessHandle, PVOID *BaseAddress, ULONG_PTR ZeroBits, SIZE_T CommitSize, PLARGE_INTEGER SectionOffset,
	PSIZE_T ViewSize, SECTION_INHERIT InheritDisposition, ULONG AllocationType, ULONG Win32Protect);
typedef NTSTATUS(NTAPI* lpRtlAcquirePrivilege)(PULONG Privilege, ULONG NumPriv, ULONG Flags, PVOID * ReturnedState);
typedef NTSTATUS(NTAPI* lpNtCreateSemaphore)(OUT PHANDLE SemaphoreHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ULONG InitialCount, IN ULONG MaximumCount);
typedef NTSTATUS(NTAPI* lpNtOpenSemaphore)(OUT PHANDLE SemaphoreHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes);
typedef NTSTATUS(NTAPI* lpNtQuerySemaphore)(IN HANDLE SemaphoreHandle, IN SEMAPHORE_INFORMATION_CLASS SemaphoreInformationClass, OUT PVOID SemaphoreInformation,
	IN ULONG SemaphoreInformationLength, OUT PULONG ReturnLength OPTIONAL);
typedef NTSTATUS(NTAPI* lpRtlAllocateAndInitializeSid)(IN PSID_IDENTIFIER_AUTHORITY IdentifierAuthority, IN UCHAR SubAuthorityCount, IN ULONG SubAuthority0, IN ULONG SubAuthority1,
	IN ULONG SubAuthority2, IN ULONG SubAuthority3, IN ULONG SubAuthority4, IN ULONG SubAuthority5, IN ULONG SubAuthority6, IN ULONG SubAuthority7, OUT PSID* Sid);
typedef NTSTATUS(NTAPI* lpRtlCreateUserThread)(HANDLE ProcessHandle, PSECURITY_DESCRIPTOR SecurityDescriptor, BOOLEAN CreateSuspended, ULONG StackZeroBits,
 SIZE_T StackReserve, SIZE_T StackCommit, PTHREAD_START_ROUTINE StartAddress, PVOID Parameter, PHANDLE ThreadHandle, PCLIENT_ID	ClientId);
typedef NTSTATUS(NTAPI* lpNtCreateFile)(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, 
	PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
typedef NTSTATUS(NTAPI* lpNtOpenProcess)(PHANDLE ProcessHandle, ACCESS_MASK AccessMask, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientID);
typedef NTSTATUS(NTAPI* lpNtOpenThread)(PHANDLE ThreadHandle, ACCESS_MASK AccessMask, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
typedef NTSTATUS(NTAPI* lpRtlCreateProcessParametersEx)(PRTL_USER_PROCESS_PARAMETERS *pProcessParameters, PUNICODE_STRING ImagePathName, PUNICODE_STRING DllPath,
	PUNICODE_STRING CurrentDirectory, UNICODE_STRING CommandLine, PVOID Environment, PUNICODE_STRING WindowTitle, PUNICODE_STRING DesktopInfo, PUNICODE_STRING ShellInfo,
	PUNICODE_STRING RuntimeData, ULONG Flags);
typedef NTSTATUS(NTAPI* lpNtWow64ReadVirtualMemory64)(HANDLE ProcessHandle, PVOID64 BaseAddress, PVOID Buffer, ULONG64 Size, PULONG64 NumberOfBytesRead);
typedef NTSTATUS(NTAPI* lpNtWow64QueryInformationProcess64) (HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength,
	PULONG ReturnLength);
typedef BOOL(WINAPI* lpRegisterWaitForSingleObject)(PHANDLE phNewWaitObject, HANDLE hObject, WAITORTIMERCALLBACK Callback, PVOID Context, ULONG dwMilliseconds, ULONG dwFlags);
typedef ULONG(WINAPI* lpRtlNtStatusToDosError)(NTSTATUS Status);
typedef NTSTATUS(NTAPI* lpNtOpenThreadToken)(HANDLE ThreadHandle, ACCESS_MASK DesiredAccess, BOOLEAN OpenAsSelf, PHANDLE TokenHandle);
typedef NTSTATUS(NTAPI* lpNtQueryInformationToken)(HANDLE TokenHandle, TOKEN_INFORMATION_CLASS TokenInformationClass, PVOID TokenInformation, ULONG TokenInformationLength, PULONG ReturnLength);
typedef BOOLEAN(WINAPI* lpWinStationSendMessageW)(HANDLE hServer, ULONG SessionId, PWSTR Title, ULONG TitleLength, PWSTR Message, ULONG MessageLength, ULONG Style,
	ULONG Timeout, PULONG Response, BOOLEAN DoNotWait);
typedef BOOL(WINAPI* lpGetHandleInformation)(HANDLE hObject, LPDWORD lpdwFlags);
typedef BOOL(WINAPI* lpCreateDirectoryA)(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
typedef LONG(NTAPI* lpNtFlushInstructionCache)(HANDLE ProcessHandle, PVOID BaseAddress, ULONG NumberOfBytesToFlush);
typedef WORD(NTAPI* lpRtlCaptureStackBackTrace)(DWORD, DWORD, PVOID *, PDWORD);
typedef NTSTATUS(NTAPI* lpNtWow64AllocateVirtualMemory64)(HANDLE ProcessHandle, PULONG64 BaseAddress, ULONG64 ZeroBits, PULONG64 Size, ULONG AllocationType, ULONG Protection);
typedef NTSTATUS(NTAPI* lpNtWow64WriteVirtualMemory64)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG BufferLength, PULONG ReturnLength);
typedef NTSTATUS(NTAPI* lpNtSuspendThread)(HANDLE ThreadHandle, PULONG PreviousSuspendCount);
typedef NTSTATUS(NTAPI* lpNtTerminateThread)(HANDLE ThreadHandle, NTSTATUS ExitStatus);
typedef HANDLE(WINAPI* lpGetProcessHeap)();
typedef NTSTATUS(NTAPI* lpNtDelayExecution)(BOOL Alertable, PLARGE_INTEGER DelayInterval);
typedef NTSTATUS(NTAPI* lpNtFreeVirtualMemory)(HANDLE ProcessHandle, PVOID *BaseAddress, PSIZE_T RegionSize, ULONG FreeType);
typedef NTSTATUS(NTAPI* lpNtSystemDebugControl)(IN SYSDBG_COMMAND Command, IN PVOID InputBuffer OPTIONAL, IN ULONG InputBufferLength, OUT PVOID OutputBuffer OPTIONAL, IN ULONG OutputBufferLength, OUT PULONG ReturnLength OPTIONAL);
typedef NTSTATUS(NTAPI* lpNtSetSystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength);
