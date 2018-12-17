#include "NM_Index.h"
#include "NM_Main.h"
#include "MemoryHelper.h"
#include "Defines.h"
#include "WinVerHelper.h"

CMemoryHelper::CMemoryHelper(HANDLE hProcess) :
	m_hProcess(hProcess)
{
}

LPVOID CMemoryHelper::Alloc(std::size_t size, DWORD dwProtection, LPVOID pPreferredLoc)
{
	return VirtualAllocEx(m_hProcess, pPreferredLoc, size, MEM_COMMIT | MEM_RESERVE, dwProtection);
}

LPVOID CMemoryHelper::Commit(LPVOID data, std::size_t size)
{
	LPVOID AllocatedPointer = Alloc(size);
	if (!AllocatedPointer)
		return nullptr;

	if (WriteProcessMemory(m_hProcess, AllocatedPointer, data, size, NULL))
		return AllocatedPointer;

	Free(AllocatedPointer, size);
	return nullptr;
}

bool CMemoryHelper::Protect(LPVOID address, std::size_t size, DWORD dwProtectFlag)
{
	DWORD dwOldProtect = 0;
	return (VirtualProtectEx(m_hProcess, address, size, dwProtectFlag, &dwOldProtect) == TRUE);
}

bool CMemoryHelper::Free(LPVOID address, std::size_t size)
{
	return (VirtualFreeEx(m_hProcess, address, NULL, MEM_RELEASE) == TRUE);
}

bool CMemoryHelper::ReadAddress(LPVOID address, LPVOID buffer, std::size_t size)
{
	SIZE_T dwReadByteCount = 0;
	if (!ReadProcessMemory(m_hProcess, address, buffer, size, &dwReadByteCount))
		dwReadByteCount = 0;
	return dwReadByteCount != 0;
}
bool CMemoryHelper::ReadAddressSafe(LPVOID address, LPVOID buffer, std::size_t size)
{
	SIZE_T dwReadByteCount = 0;
	__try {
		if (!ReadProcessMemory(m_hProcess, address, buffer, size, &dwReadByteCount))
			dwReadByteCount = 0;
	}
	__except(1) { }
	return (dwReadByteCount && dwReadByteCount == size);
}

bool CMemoryHelper::WriteAddress(LPVOID address, LPVOID buffer, std::size_t size)
{
	SIZE_T dwWriteByteCount = 0;
	if (!WriteProcessMemory(m_hProcess, address, buffer, size, &dwWriteByteCount))
		dwWriteByteCount = 0;
	return (dwWriteByteCount != 0);
}
bool CMemoryHelper::WriteAddressSafe(LPVOID address, LPVOID buffer, std::size_t size)
{
	SIZE_T dwWriteByteCount = 0;
	__try {
		if (!WriteProcessMemory(m_hProcess, address, buffer, size, &dwWriteByteCount))
			dwWriteByteCount = 0;
	}
	__except (1) { }
	return (dwWriteByteCount && dwWriteByteCount == size);
}

bool CMemoryHelper::Patch(LPVOID address, LPVOID buffer, std::size_t size)
{
	DWORD dwOldProtect = 0;
	if (VirtualProtectEx(m_hProcess, address, size, PAGE_EXECUTE_READWRITE, &dwOldProtect) == FALSE)
		return false;

	if (!WriteAddressSafe(address, buffer, size))
		return false;

	if (VirtualProtectEx(m_hProcess, address, size, dwOldProtect, &dwOldProtect) == FALSE)
		return false;

	return true;
}

bool CMemoryHelper::IsSafePage(LPVOID address)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQueryEx(m_hProcess, address, &mbi, sizeof(mbi)))
		return false;

	if (!(mbi.State & MEM_COMMIT))
		return false;

	if (mbi.State & MEM_RELEASE)
		return false;

	if (mbi.Protect == PAGE_NOACCESS || mbi.Protect & PAGE_GUARD)
		return false;

	if (mbi.Protect != PAGE_READONLY && mbi.Protect != PAGE_READWRITE && mbi.Protect != PAGE_EXECUTE_READ && mbi.Protect != PAGE_EXECUTE_READWRITE)
		return false;

	return true;
}

