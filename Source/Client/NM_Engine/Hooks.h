#pragma once
#include <Windows.h>
#include "WorkQueue.h"
#include <NktHookLib.h>

enum EHookBlacklistedShellCodes
{
	ShellCodeNull,
	LdrLoadShellCode1,
	ManualMapShellCode1,
	ManualMapShellCode2,
	ManualMapShellCode3,
	ReflectiveShellCode1,
	ManualLoadShellCode1,
	ThreadHijackShellCode1,
	ThreadHijackShellCode2,
	ThreadHijackShellCode3,
	CreateRemoteThreadExShellCode1,
	CodeInjectionShellCode1,
	AutoInjectorLalakerShellCode,
	LalakerMetin2HackV110,
	SHELLCODEMAXITEM
};

enum EHookBlacklistedApis
{
	HookApiNull=100,
	EaxLoadLibraryA,
	EaxLoadLibraryW,
	EaxLoadLibraryExA,
	EaxLoadLibraryExW,
	EaxLoadLibraryA_KBase,
	EaxLoadLibraryW_KBase,
	EaxLoadLibraryExA_KBase,
	EaxLoadLibraryExW_KBase,
	EaxFreeLibrary,
	EaxLdrLoadDll,
	EaxLdrUnloadDll,
	EaxSetWindowHookEx,
	EaxPython,
	EaxRtlUserThreadStart,
	EaxSetWindowHookEx2,
	EaxNtCreateThread,
	EaxNtCreateThreadEx,
	EaxRtlCreateUserThread,
	EaxCodeInjectionType,
	EaxUnknownState,
	EaxBadPointerType,
	EaxBadAllocatedProtectType,
	EaxBadProtectType,
	QueryWorkingSetExFail,
	QueryWorkingSetExNotValid,
	EaxMainProcess,
	EaxMappedCode,
	EaxMappedCode2,
	NullCharacteristics,
	EaxMaxType
};

enum EHookCustomFailList
{
	HOOK_CHECK_GETMODULEINFO_FAIL = 100001,
	HOOK_CHECK_GETMODULEINFO_PYTHON_FAIL = 100002,
	HOOK_CHECK_VIRTUALQUERY_FAIL = 100003,
	HOOK_CHECK_GETMAPPEDFILENAME_FAIL = 100004
};

enum EHookBlockTypes
{
	RET_HOOK = 1,
	NOP_HOOK,
};

typedef int(WINAPI* connectHook)(UINT_PTR s, const struct sockaddr* name, int namelen);
static struct {
	SIZE_T Id;
	connectHook Call;
} hconnect = { 0, 0 };

typedef NTSTATUS(NTAPI* NtMapViewOfSectionHook)(HANDLE SectionHandle, HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits, SIZE_T CommitSize, PLARGE_INTEGER SectionOffset, PSIZE_T ViewSize, SECTION_INHERIT InheritDisposition, ULONG AllocationType, ULONG Protect);
static struct {
	SIZE_T Id;
	NtMapViewOfSectionHook Call;
} hNtMapViewOfSection = { 0, 0 };

typedef void(NTAPI* LdrInitializeThunkHook)(PCONTEXT NormalContext, PVOID SystemArgument1, PVOID SystemArgument2);
static struct {
	SIZE_T Id;
	LdrInitializeThunkHook Call;
} hLdrInitializeThunk = { 0, 0 };

typedef ULONG(NTAPI* RtlGetFullPathName_UHook)(PCWSTR FileName, ULONG Size, PWSTR Buffer, PWSTR* ShortName);
static struct {
	SIZE_T Id;
	RtlGetFullPathName_UHook Call;
} hRtlGetFullPathName_U = { 0, 0 };

typedef NTSTATUS(NTAPI* LdrGetDllHandleExHook)(IN ULONG Flags, IN PWSTR DllPath OPTIONAL, IN PULONG DllCharacteristics OPTIONAL, IN PUNICODE_STRING DllName, OUT PVOID *DllHandle OPTIONAL);
static struct {
	SIZE_T Id;
	LdrGetDllHandleExHook Call;
} hLdrGetDllHandleEx = { 0, 0 };

typedef ULONG(WINAPI* pfnRtlDispatchException)(PEXCEPTION_RECORD pExcptRec, CONTEXT * pContext);
static pfnRtlDispatchException m_fnRtlDispatchException = NULL;

typedef NTSTATUS(NTAPI* NtDelayExecutionHook)(BOOL Alertable, PLARGE_INTEGER DelayInterval);
static struct {
	SIZE_T Id;
	NtDelayExecutionHook Call;
} hNtDelayExecution = { 0, 0 };

typedef LONG(WINAPI* SetWindowLongAHook)(HWND hWnd, int nIndex, LONG dwNewLong);
static struct {
	SIZE_T Id;
	SetWindowLongAHook Call;
} hSetWindowLongA = { 0, 0 };

typedef LONG(WINAPI* SetWindowLongWHook)(HWND hWnd, int nIndex, LONG dwNewLong);
static struct {
	SIZE_T Id;
	SetWindowLongWHook Call;
} hSetWindowLongW = { 0, 0 };

// ---------------

// ***
#if 0
IsNLSDefinedString
DbgUiDebugActiveProcess
DbgUiSetThreadDebugObject
RtlRbInsertNodeEx
CallWindowProcA
CallNextHookEx
DefWindowProcW
CreateFont

RtlDispatchAPC
NtQueryValueKey

GetCursor
GetCursorInfo
GetCursorPos
SetCursor
SetCursorPos
ShowCursor
ClipCursor
GetClipCursor
SetPhysicalCursorPos
GetPhysicalCursorPos
ShowCursor

GetAsyncKeyState
GetKeyState
GetKeyboardState

GetRawInputBuffer
GetRawInputData
GetRawInputDeviceInfoA/W
GetRawInputDeviceList
GetRegisteredRawInputDevices
RegisterRawInputDevice

ReleaseCapture
GetCapture
SetCapture

GetMessageA/W
PeekMessageA/W
DispatchMessageA/W

ShellExecuteA/W

RtlInitializeSListHead
RtlPcToFileHeader
RtlRestoreContext

-- win32u --
NtUserUserHandleGrantAccess
NtUserSetWindowLongPtr
NtUserSetWindowLong
NtUserSetTimer
NtUserKillTimer
NtUserClipCursor
NtUserRegisterRawInputDevices
#endif
// ***

typedef BOOL(WINAPI* ClientThreadSetupHook)(VOID);
static struct {
	SIZE_T Id;
	ClientThreadSetupHook Call;
} hClientThreadSetup = { 0, 0 };

// TODO: CreateWindowInternalW
typedef HWND(WINAPI* CreateWindowExWHook)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent,
	HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static struct {
	SIZE_T Id;
	CreateWindowExWHook Call;
} hCreateWindowExW = { 0, 0 };

typedef VOID(NTAPI* KiUserApcDispatcherHook)(PVOID Unused1, PVOID Unused2, PVOID Unused3, PVOID ContextStart, PVOID ContextBody);
static struct {
	SIZE_T Id;
	KiUserApcDispatcherHook Call;
} hKiUserApcDispatcher = { 0, 0 };

typedef VOID(NTAPI* KiUserCallbackDispatcherHook)(ULONG Index, PVOID Argument, ULONG ArgumentLength);
static struct {
	SIZE_T Id;
	KiUserCallbackDispatcherHook Call;
} hKiUserCallbackDispatcher = { 0, 0 };

typedef NTSTATUS(NTAPI* NtUnmapViewOfSectionHook)(HANDLE ProcessHandle, PVOID BaseAddress);
static struct {
	SIZE_T Id;
	NtUnmapViewOfSectionHook Call;
} hNtUnmapViewOfSection = { 0, 0 };

typedef NTSTATUS(NTAPI* NtProtectVirtualMemoryHook)(HANDLE ProcessHandle, PVOID *BaseAddress, PULONG NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
static struct {
	SIZE_T Id;
	NtProtectVirtualMemoryHook Call;
} hNtProtectVirtualMemory = { 0, 0 };

typedef NTSTATUS(NTAPI* NtFlushInstructionCacheHook)(HANDLE ProcessHandle, PVOID BaseAddress, ULONG NumberOfBytesToFlush);
static struct {
	SIZE_T Id;
	NtFlushInstructionCacheHook Call;
} hNtFlushInstructionCache = { 0, 0 };

typedef NTSTATUS(NTAPI* LdrAccessResourceHook)(PVOID BaseAddress, PIMAGE_RESOURCE_DATA_ENTRY ResourceDataEntry, PVOID *Resource, PULONG Size);
static struct {
	SIZE_T Id;
	LdrAccessResourceHook Call;
} hLdrAccessResource = { 0, 0 };

typedef NTSTATUS(NTAPI*	LdrLockLoaderLockHook)(ULONG Flags, PULONG Disposition, PULONG_PTR Cookie);
static struct {
	SIZE_T Id;
	LdrLockLoaderLockHook Call;
} hLdrLockLoaderLock = { 0, 0 };

typedef NTSTATUS(NTAPI* LdrDisableThreadCalloutsForDllHook)(PVOID DllImageBase);
static struct {
	SIZE_T Id;
	LdrDisableThreadCalloutsForDllHook Call;
} hLdrDisableThreadCalloutsForDll = { 0, 0 };

typedef NTSTATUS(NTAPI* NtLockVirtualMemoryHook)(HANDLE ProcessHandle, PVOID *BaseAddress, PULONG NumberOfBytesToLock, ULONG LockOption);
static struct {
	SIZE_T Id;
	NtLockVirtualMemoryHook Call;
} hNtLockVirtualMemory = { 0, 0 };

typedef PVOID(NTAPI* RtlAllocateHeapHook)(PVOID HeapHandle, ULONG Flags, SIZE_T Size);
static struct {
	SIZE_T Id;
	RtlAllocateHeapHook Call;
} hRtlAllocateHeap = { 0, 0 };

typedef SHORT(NTAPI* NtUserGetAsyncKeyStateHook)(INT Key);
static struct {
	SIZE_T Id;
	NtUserGetAsyncKeyStateHook Call;
} hNtUserGetAsyncKeyState = { 0, 0 };

typedef VOID(NTAPI* RtlRaiseExceptionHook)(PEXCEPTION_RECORD ExceptionRecord);
static struct {
	SIZE_T Id;
	RtlRaiseExceptionHook Call;
} hRtlRaiseException = { 0, 0 };

typedef NTSTATUS(NTAPI* RtlActivateActivationContextHook)(IN ULONG Unknown, IN HANDLE Handle, OUT PULONG_PTR Cookie);
static struct {
	SIZE_T Id;
	RtlActivateActivationContextHook Call;
} hRtlActivateActivationContext = { 0, 0 };

typedef PVOID(NTAPI* RtlAddVectoredExceptionHandlerHook)(IN ULONG FirstHandler, IN PVECTORED_EXCEPTION_HANDLER VectoredHandler);
static struct {
	SIZE_T Id;
	RtlAddVectoredExceptionHandlerHook Call;
} hRtlAddVectoredExceptionHandler = { 0, 0 };

typedef PVOID(NTAPI* RtlAddVectoredContinueHandlerHook)(IN ULONG FirstHandler, IN PVECTORED_EXCEPTION_HANDLER VectoredHandler);
static struct {
	SIZE_T Id;
	RtlAddVectoredContinueHandlerHook Call;
} hRtlAddVectoredContinueHandler = { 0, 0 };

typedef LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI* SetUnhandledExceptionFilterHook)(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
static struct {
	SIZE_T Id;
	SetUnhandledExceptionFilterHook Call;
} hSetUnhandledExceptionFilter = { 0, 0 };

typedef PVOID(WINAPI* AddVectoredExceptionHandlerHook)(ULONG FirstHandler, PVECTORED_EXCEPTION_HANDLER VectoredHandler);
static struct {
	SIZE_T Id;
	AddVectoredExceptionHandlerHook Call;
} hAddVectoredExceptionHandler = { 0, 0 };

typedef NTSTATUS(NTAPI* RtlInitAnsiStringExHook)(PANSI_STRING target, PCSZ source);
static struct {
	SIZE_T Id;
	RtlInitAnsiStringExHook Call;
} hRtlInitAnsiStringEx = { 0, 0 };

typedef void(NTAPI* RtlInitUnicodeStringHook)(PUNICODE_STRING target, PCWSTR source);
static struct {
	SIZE_T Id;
	RtlInitUnicodeStringHook Call;
} hRtlInitUnicodeString = { 0, 0 };

typedef int(WINAPI* MultiByteToWideCharHook)(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
typedef int(WINAPI* WideCharToMultiByteHook)(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);

typedef NTSTATUS(NTAPI* NtCreateSectionHook)(PHANDLE SectionHandle, ULONG DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PLARGE_INTEGER MaximumSize, ULONG PageAttributess,
	ULONG SectionAttributes, HANDLE FileHandle);
static struct {
	SIZE_T Id;
	NtCreateSectionHook Call;
} hNtCreateSection = { 0, 0 };

typedef NTSTATUS(NTAPI* NtCreateFileHook)(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
static struct {
	SIZE_T Id;
	NtCreateFileHook Call;
} hNtCreateFile = { 0, 0 };

typedef NTSTATUS(NTAPI* NtQueryAttributesFileHook)(POBJECT_ATTRIBUTES ObjectAttributes, PFILE_BASIC_INFORMATION FileAttributes);
static struct {
	SIZE_T Id;
	NtQueryAttributesFileHook Call;
} hNtQueryAttributesFile = { 0, 0 };

typedef HANDLE(WINAPI* CreateFileMappingAHook)(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD  flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCTSTR lpName);
static struct {
	SIZE_T Id;
	CreateFileMappingAHook Call;
} hCreateFileMappingA = { 0, 0 };

typedef UINT(WINAPI* GetRawInputDataHook)(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader);
static struct {
	SIZE_T Id;
	GetRawInputDataHook Call;
} hGetRawInputData = { 0, 0 };

typedef BOOL(WINAPI* CryptGetOIDFunctionAddressHook)(HCRYPTOIDFUNCSET hFuncSet, DWORD dwEncodingType, LPCSTR pszOID, DWORD dwFlags, void **ppvFuncAddr, HCRYPTOIDFUNCADDR *phFuncAddr);
static struct {
	SIZE_T Id;
	CryptGetOIDFunctionAddressHook Call;
} hCryptGetOIDFunctionAddress = { 0, 0 };

typedef BOOL(WINAPI* IsBadReadPtrHook)(const VOID *lp, UINT_PTR ucb);
static struct {
	SIZE_T Id;
	IsBadReadPtrHook Call;
} hIsBadReadPtr = { 0, 0 };

typedef BOOL(WINAPI* CreateProcessInternalWHook)(HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, 
	LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken);
static struct {
	SIZE_T Id;
	CreateProcessInternalWHook Call;
} hCreateProcessInternalW = { 0, 0 };

// --------------------------------

class CSelfApiHooks
{
	public:
		CSelfApiHooks();
		~CSelfApiHooks() = default;

	public:
		bool InitializeSelfAPIHooks();
		bool HooksIsInitialized();

#ifdef _DEBUG
		auto LogInstance() { return m_logHelper; };
#endif

	protected:
		bool BlockAPI(LPCSTR lpModule, LPCSTR lpFuncName, int iType);

		bool InitializePatchs();
		bool InitializeDetours();

	private:
		mutable std::recursive_mutex	m_mutex;
#ifdef _DEBUG
		std::shared_ptr <CLogger>		m_logHelper;
#endif
		CNktHookLib *					m_pNktHook;
		bool							m_bHooksIsInitialized;

	public:
		void ThreadHookCallback(std::shared_ptr <SThreadHookQueueData> pData);
		void ModuleHookCallback(std::shared_ptr <SModuleHookQueueData> pData);
		void ModuleRequestHookCallback(std::shared_ptr <SModuleReqHookQueueData> pData);
		void SectionHookCallback(std::shared_ptr <SSectionHookQueueData> pData);
		void WndProcHookCallback(std::shared_ptr <SWndProcHookQueueData> pData);
};

