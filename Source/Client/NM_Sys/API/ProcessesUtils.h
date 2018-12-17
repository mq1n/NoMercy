#pragma once

#define PROCESS_TERMINATE					(0x0001)  
#define PROCESS_CREATE_THREAD				(0x0002)  
#define PROCESS_SET_SESSIONID				(0x0004)  
#define PROCESS_VM_OPERATION				(0x0008)  
#define PROCESS_VM_READ						(0x0010)  
#define PROCESS_VM_WRITE					(0x0020)  
#define PROCESS_DUP_HANDLE					(0x0040)  
#define PROCESS_CREATE_PROCESS				(0x0080)  
#define PROCESS_SET_QUOTA					(0x0100)  
#define PROCESS_SET_INFORMATION				(0x0200)  
#define PROCESS_QUERY_INFORMATION			(0x0400)  
#define PROCESS_SUSPEND_RESUME				(0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION	(0x1000)
#define PROCESS_SET_LIMITED_INFORMATION		(0x2000)  

#define THREAD_TERMINATE					(0x0001)
#define THREAD_SUSPEND_RESUME				(0x0002)
#define THREAD_GET_CONTEXT					(0x0008)
#define THREAD_SET_CONTEXT					(0x0010)
#define THREAD_SET_INFORMATION				(0x0020)
#define THREAD_QUERY_INFORMATION			(0x0040)
#define THREAD_SET_THREAD_TOKEN				(0x0080)
#define THREAD_IMPERSONATE					(0x0100)
#define THREAD_DIRECT_IMPERSONATION			(0x0200)
#define THREAD_SET_LIMITED_INFORMATION		(0x0400)
#define THREAD_QUERY_LIMITED_INFORMATION	(0x0800)
#define THREAD_RESUME						(0x1000)

extern "C"
{
	NTKERNELAPI UCHAR* PsGetProcessImageFileName(IN PEPROCESS Process);
	NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(IN PEPROCESS Process);
	NTKERNELAPI PPEB PsGetProcessPeb(PEPROCESS Process);
};

namespace Processes 
{
	namespace Descriptors 
	{
		// Queried PEPROCESS must be dereferenced by ObDereferenceObject:
		_IRQL_requires_max_(APC_LEVEL)
		PEPROCESS GetEPROCESS(HANDLE ProcessId);

		// Queried PETHREAD must be dereferenced by ObDereferenceObject:
		_IRQL_requires_max_(APC_LEVEL)
		PETHREAD GetETHREAD(HANDLE ThreadId);

		// Queried hProcess must be closed by ZwClose:
		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS OpenProcess(HANDLE ProcessId, OUT PHANDLE hProcess, ACCESS_MASK AccessMask = PROCESS_ALL_ACCESS, ULONG Attributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS OpenProcessByPointer(PEPROCESS Process, OUT PHANDLE hProcess, ACCESS_MASK AccessMask = PROCESS_ALL_ACCESS, ULONG Attributes = OBJ_KERNEL_HANDLE, KPROCESSOR_MODE ProcessorMode = KernelMode);

		// Queried hThread must be closed by ZwClose:
		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS OpenThread(HANDLE ThreadId, OUT PHANDLE hThread, ACCESS_MASK AccessMask = THREAD_ALL_ACCESS, ULONG Attributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS OpenThreadByPointer(PETHREAD Thread, OUT PHANDLE hThread, ACCESS_MASK AccessMask = THREAD_ALL_ACCESS, ULONG Attributes = OBJ_KERNEL_HANDLE, KPROCESSOR_MODE ProcessorMode = KernelMode);

		// It doesn't requires dereference!
		inline PEPROCESS ThreadToProcess(PETHREAD Thread)
		{
			return IoThreadToProcess(Thread);
		};
	}

	namespace AddressSpace 
	{
		// These functions switches your thread's address space
		// to address space of target process, so you can
		// access usermode memory of another process you want.

		// !!! NOTICE !!!
		// After switching of address space your own usermode memory
		// and usermode handles becomes INVALID in context of another process!

		_IRQL_requires_max_(APC_LEVEL)
		BOOLEAN AttachToProcessByPid(HANDLE ProcessId, OUT PKAPC_STATE ApcState);

		_IRQL_requires_max_(APC_LEVEL)
		BOOLEAN AttachToProcess(PEPROCESS Process, OUT PKAPC_STATE ApcState);

		_IRQL_requires_max_(APC_LEVEL)
		VOID DetachFromProcess(IN PKAPC_STATE ApcState);
	}

	namespace Terminator 
	{
		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS TerminateProcessByPid(HANDLE ProcessId, NTSTATUS ExitStatus);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS TerminateProcess(HANDLE hProcess, NTSTATUS ExitStatus);
	}

	namespace Threads 
	{
		_IRQL_requires_max_(APC_LEVEL)
		NTSTATUS GetContextThread(IN PETHREAD Thread, OUT PCONTEXT Context, IN KPROCESSOR_MODE PreviousMode);

		_IRQL_requires_max_(APC_LEVEL)
		NTSTATUS SetContextThread(IN PETHREAD Thread, IN PCONTEXT Context, IN KPROCESSOR_MODE PreviousMode);

		_IRQL_requires_max_(APC_LEVEL)
		NTSTATUS SuspendProcess(IN PEPROCESS Process);

		_IRQL_requires_max_(APC_LEVEL)
		NTSTATUS ResumeProcess(IN PEPROCESS Process);


		using _UserThreadRoutine = NTSTATUS (NTAPI*)(PVOID Argument);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS CreateUserThread(HANDLE hProcess, IN _UserThreadRoutine StartAddress,IN PVOID Argument, BOOLEAN CreateSuspended, OUT PHANDLE hThread, OUT PCLIENT_ID ClientId);


		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS CreateSystemThread(OPTIONAL HANDLE hProcess, PKSTART_ROUTINE StartAddress, PVOID Argument, OUT PHANDLE hThread, OUT PCLIENT_ID ClientId);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS CreateSystemThread(PKSTART_ROUTINE StartAddress, PVOID Argument, OUT PHANDLE hThread);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS QueryInformationThread(HANDLE hThread, THREADINFOCLASS ThreadInformationClass, OUT PVOID ThreadInformation, ULONG ThreadInformationLength, OUT PULONG ReturnLength);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS SetInformationThread(HANDLE hThread, THREADINFOCLASS ThreadInformationClass, IN PVOID ThreadInformation, ULONG ThreadInformationLength);
	}

	namespace Information 
	{
		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS QueryInformationProcess(HANDLE hProcess, PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, ULONG ProcessInformationLength, OUT PULONG ReturnLength);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS SetInformationProcess(HANDLE hProcess, PROCESSINFOCLASS ProcessInformationClass, IN PVOID ProcessInformation, ULONG ProcessInformationLength);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		BOOLEAN Is32BitProcess(HANDLE hProcess = ZwCurrentProcess());
	}

	namespace MemoryManagement 
	{
		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS AllocateVirtualMemory(HANDLE hProcess, SIZE_T Size, ULONG Protect, IN OUT PVOID* BaseAddress);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		PVOID AllocateVirtualMemory(HANDLE hProcess, SIZE_T Size, ULONG Protect);

		_IRQL_requires_max_(PASSIVE_LEVEL)
		NTSTATUS FreeVirtualMemory(HANDLE hProcess, PVOID BaseAddress);

		_IRQL_requires_max_(APC_LEVEL)
		NTSTATUS ReadProcessMemory(PEPROCESS Process, __in_data_source(USER_MODE) IN PVOID BaseAddress, OUT PVOID Buffer, ULONG Size);

		_IRQL_requires_max_(APC_LEVEL)
		NTSTATUS WriteProcessMemory(PEPROCESS Process, __in_data_source(USER_MODE) OUT PVOID BaseAddress, IN PVOID Buffer, ULONG Size);
	}

	namespace Apc 
	{
		enum KAPC_ENVIRONMENT 
		{
			OriginalApcEnvironment,
			AttachedApcEnvironment,
			CurrentApcEnvironment,
			InsertApcEnvironment
		};

		using PKRUNDOWN_ROUTINE = VOID(NTAPI*)(PRKAPC Apc);

		using PKNORMAL_ROUTINE = VOID(NTAPI*)(PVOID NormalContext, PVOID SyatemArgument1, PVOID SystemArgument2);
		using PKKERNEL_ROUTINE = VOID(NTAPI*)(PRKAPC Apc, PKNORMAL_ROUTINE NormalRoutine, PVOID NormalContext, PVOID SystemArgument1, PVOID SystemArgument2);


		using _KeInitializeApc = VOID(NTAPI*)(IN PRKAPC Apc, IN PRKTHREAD Thread, IN KAPC_ENVIRONMENT Environment, IN PKKERNEL_ROUTINE KernelRoutine, IN PKRUNDOWN_ROUTINE RundownRoutine OPTIONAL,
			IN PKNORMAL_ROUTINE NormalRoutine OPTIONAL, IN KPROCESSOR_MODE ApcMode OPTIONAL, IN PVOID NormalContext OPTIONAL);

		using _KeInsertQueueApc = BOOLEAN(NTAPI*)(PRKAPC Apc, PVOID SystemArgument1, PVOID SystemArgument2, KPRIORITY PriorityBoost);

		using _KeTestAlertThread = BOOLEAN(NTAPI*)(KPROCESSOR_MODE AlertMode);


		_IRQL_requires_max_(APC_LEVEL)
		NTSTATUS QueueUserApc(PETHREAD Thread, PKNORMAL_ROUTINE NormalRoutine, PVOID Argument);
	}
}

