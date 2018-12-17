#include "NM_Index.h"
#include "NM_Main.h"
#include "DynamicWinapi.h"

void CDynamicWinapi::BindAPIs_4()
{
	g_winapiApiTable->RtlQueryProcessDebugInformation = (lpRtlQueryProcessDebugInformation)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlQueryProcessDebugInformation").crypt_get());
	g_winapiApiTable->LdrShutdownProcess = (lpLdrShutdownProcess)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("LdrShutdownProcess").crypt_get());
	g_winapiApiTable->NtCreateSection = (lpNtCreateSection)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtCreateSection").crypt_get());
	g_winapiApiTable->NtMapViewOfSection = (lpNtMapViewOfSection)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtMapViewOfSection").crypt_get());
	g_winapiApiTable->RtlAcquirePrivilege = (lpRtlAcquirePrivilege)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlAcquirePrivilege").crypt_get());
	g_winapiApiTable->NtCreateSemaphore = (lpNtCreateSemaphore)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtCreateSemaphore").crypt_get());
	g_winapiApiTable->NtOpenSemaphore = (lpNtOpenSemaphore)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtOpenSemaphore").crypt_get());
	g_winapiApiTable->NtQuerySemaphore = (lpNtQuerySemaphore)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtQuerySemaphore").crypt_get());
	g_winapiApiTable->RtlAllocateAndInitializeSid = (lpRtlAllocateAndInitializeSid)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlAllocateAndInitializeSid").crypt_get());
	g_winapiApiTable->RtlCreateUserThread = (lpRtlCreateUserThread)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlCreateUserThread").crypt_get());
	g_winapiApiTable->NtCreateFile = (lpNtCreateFile)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtCreateFile").crypt_get());
	g_winapiApiTable->NtOpenProcess = (lpNtOpenProcess)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtOpenProcess").crypt_get());
	g_winapiApiTable->NtOpenThread = (lpNtOpenThread)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtOpenThread").crypt_get());
	g_winapiApiTable->RtlCreateProcessParametersEx = (lpRtlCreateProcessParametersEx)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlCreateProcessParametersEx").crypt_get());
	g_winapiApiTable->RegisterWaitForSingleObject = (lpRegisterWaitForSingleObject)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("RegisterWaitForSingleObject").crypt_get());
	g_winapiApiTable->RtlNtStatusToDosError = (lpRtlNtStatusToDosError)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlNtStatusToDosError").crypt_get());
	g_winapiApiTable->NtOpenThreadToken = (lpNtOpenThreadToken)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtOpenThreadToken").crypt_get());
	g_winapiApiTable->GetProcessHeap = (lpGetProcessHeap)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetProcessHeap").crypt_get());
	g_winapiApiTable->NtQueryInformationToken = (lpNtQueryInformationToken)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtQueryInformationToken").crypt_get());
	g_winapiApiTable->WinStationSendMessageW = (lpWinStationSendMessageW)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hWinsta, xorstr("WinStationSendMessageW").crypt_get());
	g_winapiApiTable->NtFlushInstructionCache = (lpNtFlushInstructionCache)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtFlushInstructionCache").crypt_get());
	g_winapiApiTable->NtSuspendThread = (lpNtSuspendThread)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtSuspendThread").crypt_get());
	g_winapiApiTable->NtTerminateThread = (lpNtTerminateThread)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtTerminateThread").crypt_get());
	g_winapiApiTable->NtDelayExecution = (lpNtDelayExecution)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtDelayExecution").crypt_get());
	g_winapiApiTable->NtFreeVirtualMemory = (lpNtFreeVirtualMemory)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtFreeVirtualMemory").crypt_get());
	g_winapiApiTable->NtSystemDebugControl = (lpNtSystemDebugControl)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtSystemDebugControl").crypt_get());
	g_winapiApiTable->RtlCaptureStackBackTrace = (lpRtlCaptureStackBackTrace)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlCaptureStackBackTrace").crypt_get());
	g_winapiApiTable->NtSetSystemInformation = (lpNtSetSystemInformation)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtSetSystemInformation").crypt_get());
	g_winapiApiTable->RegGetValueA = (lpRegGetValueA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("RegGetValueA").crypt_get());
	g_winapiApiTable->QueryFullProcessImageNameW = (lpQueryFullProcessImageNameW)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("QueryFullProcessImageNameW").crypt_get());
	g_winapiApiTable->SetWinEventHook = (lpSetWinEventHook)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("SetWinEventHook").crypt_get());
	g_winapiApiTable->UnhookWinEvent = (lpUnhookWinEvent)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("UnhookWinEvent").crypt_get());
	g_winapiApiTable->IsWindow = (lpIsWindow)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("IsWindow").crypt_get());
	g_winapiApiTable->GetWindowDisplayAffinity = (lpGetWindowDisplayAffinity)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("GetWindowDisplayAffinity").crypt_get());
	g_winapiApiTable->SetWindowDisplayAffinity = (lpSetWindowDisplayAffinity)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("SetWindowDisplayAffinity").crypt_get());
	g_winapiApiTable->RtlCreateTimerQueue = (lpRtlCreateTimerQueue)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlCreateTimerQueue").crypt_get());
	g_winapiApiTable->RtlDeleteTimerQueue = (lpRtlDeleteTimerQueue)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlDeleteTimerQueue").crypt_get());
	g_winapiApiTable->RtlCreateTimer = (lpRtlCreateTimer)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlCreateTimer").crypt_get());
	g_winapiApiTable->RtlDeleteTimer = (lpRtlDeleteTimer)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("RtlDeleteTimer").crypt_get());
	g_winapiApiTable->GetSystemDirectoryA = (lpGetSystemDirectoryA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetSystemDirectoryA").crypt_get());
	g_winapiApiTable->GetNamedPipeServerProcessId = (lpGetNamedPipeServerProcessId)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("GetNamedPipeServerProcessId").crypt_get());
	g_winapiApiTable->NtYieldExecution = (lpNtYieldExecution)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hNtdll, xorstr("NtYieldExecution").crypt_get());
	g_winapiApiTable->CreateEventA = (lpCreateEventA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("CreateEventA").crypt_get());
	g_winapiApiTable->ResetEvent = (lpResetEvent)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("ResetEvent").crypt_get());
	g_winapiApiTable->SetEvent = (lpSetEvent)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("SetEvent").crypt_get());
	g_winapiApiTable->DuplicateTokenEx = (lpDuplicateTokenEx)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("DuplicateTokenEx").crypt_get());
	g_winapiApiTable->CreateEnvironmentBlock = (lpCreateEnvironmentBlock)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUserEnv, xorstr("CreateEnvironmentBlock").crypt_get());
	g_winapiApiTable->CreateProcessAsUserA = (lpCreateProcessAsUserA)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hAdvapi32, xorstr("CreateProcessAsUserA").crypt_get());
	g_winapiApiTable->DestroyEnvironmentBlock = (lpDestroyEnvironmentBlock)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUserEnv, xorstr("DestroyEnvironmentBlock").crypt_get());
	g_winapiApiTable->HeapSetInformation = (lpHeapSetInformation)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hKernel32, xorstr("HeapSetInformation").crypt_get());
	g_winapiApiTable->CloseWindow = (lpCloseWindow)g_winapiApiTable->GetProcAddress(g_winapiModuleTable->hUser32, xorstr("CloseWindow").crypt_get());
}

