#pragma once
#include "Windows_Structs.h"
#include <string>
#include <mutex>
#include <vector>

#undef CreateFile

typedef struct _HEAP_BLOCK_CONTEXT
{
	PVOID pBase;
	char szName[255];
} SHeapBlockContext, *PHeapBlockContext;

class CNT
{
	public:
		CNT() = default;
		~CNT() = default;

		PVOID Alloc(SIZE_T ulSize, const std::string & szName = "");
		bool Free(PVOID pMemBase);
		PVOID ReAlloc(PVOID pMemBase, SIZE_T ulSize, const std::string & szName = "");
		PVOID SmartAlloc(SIZE_T ulSize, const std::string & szName = ""); // TODO: Use smart pointers with default ::Free func deleter

		HANDLE OpenProcess(DWORD dwDesiredAccess, DWORD dwProcessId);
		bool SuspendProcess(HANDLE hThread);
		bool ResumeProcess(HANDLE hProcess);
		HANDLE OpenProcessToken(HANDLE processHandle, ACCESS_MASK desiredAccess);
		bool TerminateProcess(HANDLE hProcess, NTSTATUS ulExitStatus);

		HANDLE OpenThread(DWORD dwDesiredAccess, DWORD dwThreadId);
		HANDLE CreateThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParam, PDWORD_PTR pdwThreadId);
		bool CreateThreadAndWait(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParam, DWORD dwDelay, LPDWORD pdwExitCode);
		bool SuspendThread(HANDLE hThread);
		bool ResumeThread(HANDLE hThread, bool bLoop);
		bool TerminateThread(HANDLE hThread, NTSTATUS ulExitStatus);

		bool CloseHandle(HANDLE hTarget);
		bool WaitObject(HANDLE hTarget, DWORD dwDelay);
		bool Sleep(DWORD dwDelay);
		void YieldCPU();

		NTSTATUS CreateFile(PHANDLE hFile, LPWSTR FilePath, ACCESS_MASK AccessMask, ULONG FileAttributes, ULONG ShareAccess, ULONG DispositionFlags, ULONG CreateOptions);
		NTSTATUS QueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);

#if 0
		NTSTATUS AllocateMemory(HANDLE ProcessHandle, PULONG64 BaseAddress, ULONG64 ZeroBits, PULONG64 Size, ULONG AllocationType, ULONG Protection);
		NTSTATUS WriteMemory(HANDLE ProcessHandle, PVOID64 BaseAddress, PVOID Buffer, ULONG64 BufferLength, PULONG ReturnLength);
		NTSTATUS QueryMemory(HANDLE ProcessHandle, ULONG64 BaseAddress, MEMORY_BASIC_INFORMATION64* MemoryInformationClass, PVOID64 Buffer, PVOID64 Length, PULONG ResultLength);
		NTSTATUS ReadMemory(HANDLE ProcessHandle, PVOID64 BaseAddress, PVOID Buffer, ULONG64 Size, PULONG64 NumberOfBytesRead);
		NTSTATUS ProtectMemory(HANDLE ProcessHandle, PVOID64 BaseAddress, DWORD64 NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
#endif

	protected:
		auto GetHeapBlocks();

	private:
		mutable std::recursive_mutex						m_Mutex;
		std::vector < std::shared_ptr <SHeapBlockContext> > m_vHeapBlockPool;
};

