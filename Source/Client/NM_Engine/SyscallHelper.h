#pragma once
#include "SyscallIndexHelper.h"

class CSyscallHelper
{
	public:
		CSyscallHelper();
		~CSyscallHelper();

		bool BuildSyscalls();
		ULONG_PTR BuildSyscall(DWORD dwIdx, LPDWORD pdwHash);

		NTSTATUS NtSuspendProcess(HANDLE);
		NTSTATUS NtWriteVirtualMemory(HANDLE, PVOID, CONST VOID *, SIZE_T, PSIZE_T);
		NTSTATUS NtProtectVirtualMemory(HANDLE, PVOID *, SIZE_T *, ULONG, PULONG);

	private:
		std::shared_ptr <CSyscallIndexHelper>	m_pSyscallIndexHelper;

		DWORD									m_dwNtSuspendProcessIndex;
		DWORD									m_dwNtWriteVirtualMemoryIndex;
		DWORD									m_dwNtProtectVirtualMemoryIndex;
};

