#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "SyscallHelper.h"
#include "SyscallIndexHelper.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "PEHelper.h"

#include <rewolf-wow64ext/src/wow64ext.h>


CSyscallHelper::CSyscallHelper()
{
	m_pSyscallIndexHelper = std::make_shared<CSyscallIndexHelper>();
}

CSyscallHelper::~CSyscallHelper()
{
}


bool CSyscallHelper::BuildSyscalls()
{
	DEBUG_LOG(LL_SYS, "Syscall index searcher has been started!");

	if (!IS_VALID_SMART_PTR(m_pSyscallIndexHelper))
	{
		DEBUG_LOG(LL_ERR, "Syscall index helper can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	m_dwNtSuspendProcessIndex = m_pSyscallIndexHelper->GetSyscallId(xorstr("NtSuspendProcess").crypt_get());
	if (!m_dwNtSuspendProcessIndex)
	{
		DEBUG_LOG(LL_ERR, "NtSuspendProcess syscall index NOT found! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "NtSuspendProcess syscall index: %p", m_dwNtSuspendProcessIndex);

	m_dwNtWriteVirtualMemoryIndex = m_pSyscallIndexHelper->GetSyscallId(xorstr("NtWriteVirtualMemory").crypt_get());
	if (!m_dwNtWriteVirtualMemoryIndex)
	{
		DEBUG_LOG(LL_ERR, "NtWriteVirtualMemory syscall index NOT found! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "NtWriteVirtualMemory syscall index: %p", m_dwNtWriteVirtualMemoryIndex);

	m_dwNtProtectVirtualMemoryIndex = m_pSyscallIndexHelper->GetSyscallId(xorstr("NtProtectVirtualMemory").crypt_get());
	if (!m_dwNtProtectVirtualMemoryIndex)
	{
		DEBUG_LOG(LL_ERR, "NtProtectVirtualMemory syscall index NOT found! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	DEBUG_LOG(LL_SYS, "NtProtectVirtualMemory syscall index: %p", m_dwNtProtectVirtualMemoryIndex);


	DEBUG_LOG(LL_SYS, "Syscall index searcher completed!");
	return true;
}

ULONG_PTR CSyscallHelper::BuildSyscall(DWORD dwIdx, LPDWORD pdwHash)
{
	auto dwRegionSize	= (SIZE_T)0x1000;
	auto pMemory		= (PVOID)0x0;

	auto ntStatus = g_winapiApiTable->NtAllocateVirtualMemory(NtCurrentProcess, &pMemory, 0L, &dwRegionSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "Memory can NOT allocated! Status: %p", ntStatus);
		return NULL;
	}

	auto pSysCall = (ULONG_PTR)pMemory;

	if (g_nmApp->FunctionsInstance()->IsX64System() == false)
	{
		BYTE pSyscall32[] =
		{
			0xB8, 0x0, 0x0, 0x0, 0x0,	// mov eax, dwIdx
			0xE8, 0x3, 0x0, 0x0, 0x0,	// call sysentry
			0xC2, 0x14, 0x0,			// ret 0x14
// sysenter:
			0x8B, 0xD4,					// mov edx,esp
			0x0F, 0x34,					// sysenter
			0xC3						// retn
		};
		memcpy(&pSyscall32[1], &dwIdx, 4);

		// copy 32 bit opcode
		memcpy((void*)pSysCall, pSyscall32, sizeof(pSyscall32));
		pSysCall += sizeof(pSyscall32);
	}
	else
	{
		BYTE pSyscall64[] =
		{
			0xB8, 0x0, 0x0, 0x0, 0x0,	// mov eax, dwIdx
			0x4C, 0x8B, 0xD1,			// mov r10,rcx
			0x0F, 0x05,					// syscall
			0xC3						// retn
		};
		memcpy(&pSyscall64[1], &dwIdx, 4);

		//copy 64 bit opcode
		memcpy((void*)pSysCall, pSyscall64, sizeof(pSyscall64));
	}

	if (pdwHash) *pdwHash = CPEFunctions::CalculateChecksum((DWORD)pMemory, 0x1000);
	return pSysCall;
}


NTSTATUS CSyscallHelper::NtSuspendProcess(IN HANDLE ProcessHandle)
{
	DEBUG_LOG(LL_SYS, "Custom NtSuspendProcess has been started!");

	auto dwCorrectSum = 0UL;
	auto pSyscall = BuildSyscall(m_dwNtSuspendProcessIndex, &dwCorrectSum);
	if (!pSyscall)
	{
		DEBUG_LOG(LL_ERR, "Syscall function can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return STATUS_INVALID_HANDLE;
	}

	auto pCurrSum = CPEFunctions::CalculateChecksum(pSyscall, 0x1000);
	if (pCurrSum != dwCorrectSum)
	{
		DEBUG_LOG(LL_ERR, "Syscall function corrupted! Sum: %p/%p", pCurrSum, dwCorrectSum);
		return STATUS_BAD_TOKEN_TYPE;
	}

#ifndef _WIN64
	auto ntStatus = static_cast<NTSTATUS>(X64Call((DWORD64)pSyscall, 1, (DWORD64)ProcessHandle));
#else
	auto ntStatus = lpNtSuspendProcess(pSyscall)(ProcessHandle);
#endif

	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "Syscall function fail! Status: %p", ntStatus);
		return ntStatus;
	}

	SIZE_T RegionSize = 0;
	ntStatus = g_winapiApiTable->NtFreeVirtualMemory(NtCurrentProcess, (PVOID*)&pSyscall, &RegionSize, MEM_RELEASE);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "Syscall function can NOT free'd! Error: %u", g_winapiApiTable->GetLastError());
		return ntStatus;
	}

	DEBUG_LOG(LL_SYS, "Custom NtSuspendProcess completed!");
	return ntStatus;
}

NTSTATUS CSyscallHelper::NtWriteVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, LPCVOID Buffer, SIZE_T NumberOfBytesToWrite, PSIZE_T NumberOfBytesWritten)
{
	DEBUG_LOG(LL_SYS, "Custom NtWriteVirtualMemory has been started!");

	auto dwCorrectSum = 0UL;
	auto pSyscall = BuildSyscall(m_dwNtWriteVirtualMemoryIndex, &dwCorrectSum);
	if (!pSyscall)
	{
		DEBUG_LOG(LL_ERR, "Syscall function can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return STATUS_INVALID_HANDLE;
	}

	auto pCurrSum = CPEFunctions::CalculateChecksum(pSyscall, 0x1000);
	if (pCurrSum != dwCorrectSum)
	{
		DEBUG_LOG(LL_ERR, "Syscall function corrupted! Sum: %p/%p", pCurrSum, dwCorrectSum);
		return STATUS_BAD_TOKEN_TYPE;
	}

#ifndef _WIN64
	auto ntStatus = static_cast<NTSTATUS>(X64Call((DWORD64)pSyscall, 5, (DWORD64)ProcessHandle, (DWORD64)BaseAddress, (DWORD64)Buffer, (DWORD64)NumberOfBytesToWrite, (DWORD64)NumberOfBytesWritten));
#else
	auto ntStatus = lpNtWriteVirtualMemory(pSyscall)(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten);
#endif

	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "Syscall function fail! Status: %p", ntStatus);
		return ntStatus;
	}

	SIZE_T RegionSize = 0;
	ntStatus = g_winapiApiTable->NtFreeVirtualMemory(NtCurrentProcess, (PVOID*)&pSyscall, &RegionSize, MEM_RELEASE);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "Syscall function can NOT free'd! Error: %u", g_winapiApiTable->GetLastError());
		return ntStatus;
	}

	DEBUG_LOG(LL_SYS, "Custom NtWriteVirtualMemory completed!");
	return ntStatus;
}

NTSTATUS CSyscallHelper::NtProtectVirtualMemory(HANDLE ProcessHandle, PVOID *BaseAddress, SIZE_T *NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection)
{
	DEBUG_LOG(LL_SYS, "Custom NtProtectVirtualMemory has been started!");

	auto dwCorrectSum = 0UL;
	auto pSyscall = BuildSyscall(m_dwNtProtectVirtualMemoryIndex, &dwCorrectSum);
	if (!pSyscall)
	{
		DEBUG_LOG(LL_ERR, "Syscall function can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return STATUS_INVALID_HANDLE;
	}

	auto pCurrSum = CPEFunctions::CalculateChecksum(pSyscall, 0x1000);
	if (pCurrSum != dwCorrectSum)
	{
		DEBUG_LOG(LL_ERR, "Syscall function corrupted! Sum: %p/%p", pCurrSum, dwCorrectSum);
		return STATUS_BAD_TOKEN_TYPE;
	}

#ifndef _WIN64
	auto ntStatus = static_cast<NTSTATUS>(X64Call((DWORD64)pSyscall, 5, (DWORD64)ProcessHandle, (DWORD64)BaseAddress, (DWORD64)NumberOfBytesToProtect, (DWORD64)NewAccessProtection, (DWORD64)OldAccessProtection));
#else
	auto ntStatus = lpNtProtectVirtualMemory(pSyscall)(ProcessHandle, BaseAddress, NumberOfBytesToProtect, NewAccessProtection, OldAccessProtection);
#endif

	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "Syscall function fail! Status: %p", ntStatus);
		return ntStatus;
	}

	SIZE_T RegionSize = 0;
	ntStatus = g_winapiApiTable->NtFreeVirtualMemory(NtCurrentProcess, (PVOID*)&pSyscall, &RegionSize, MEM_RELEASE);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, "Syscall function can NOT free'd! Error: %u", g_winapiApiTable->GetLastError());
		return ntStatus;
	}

	DEBUG_LOG(LL_SYS, "Custom NtProtectVirtualMemory completed!");
	return ntStatus;
}


