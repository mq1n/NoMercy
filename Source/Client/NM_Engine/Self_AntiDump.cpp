#include "NM_Index.h"
#include "NM_Main.h"
#include "SelfProtection.h"
#include "DynamicWinapi.h"
#include "Defines.h"

#define BUFFER_SIZE 0x1000

static LPVOID s_pGuardMem = nullptr;

inline LPVOID CreateSafeMemoryPage(DWORD dwRegionSize, DWORD dwProtection)
{
	LPVOID pMemBase = nullptr;

	__try
	{
		pMemBase = g_winapiApiTable->VirtualAlloc(0, dwRegionSize, MEM_COMMIT | MEM_RESERVE, dwProtection);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { }

	return pMemBase;
}

bool CSelfProtection::InitializeAntiDump(HMODULE hModule)
{
	for (std::size_t i = 0; i < g_nmApp->FunctionsInstance()->GetRandomInt(20, 50); i++)
		CreateSafeMemoryPage(BUFFER_SIZE, PAGE_READWRITE);

	s_pGuardMem = CreateSafeMemoryPage(BUFFER_SIZE, PAGE_READWRITE);

	for (std::size_t i = 0; i < g_nmApp->FunctionsInstance()->GetRandomInt(20, 50); i++)
		CreateSafeMemoryPage(BUFFER_SIZE, PAGE_READWRITE);

//	auto hTargetModule = g_winapiModuleTable->hBaseModule;
	auto hTargetModule = hModule;

	auto pDOS = (IMAGE_DOS_HEADER *)hTargetModule;
	if (pDOS->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pINH = (IMAGE_NT_HEADERS *)(pDOS + pDOS->e_lfanew);
//	if (pINH->Signature != IMAGE_NT_SIGNATURE)
//		return false;

	auto pISH = (PIMAGE_SECTION_HEADER)(pINH + 1);
	if (!pISH)
		return false;

	auto dwOldProtect = 0UL;
	VirtualProtect((LPVOID)pISH, sizeof(LPVOID), PAGE_READWRITE, &dwOldProtect);

	pISH[0].VirtualAddress = reinterpret_cast<DWORD_PTR>(s_pGuardMem);

	VirtualProtect((LPVOID)pISH, sizeof(LPVOID), dwOldProtect, &dwOldProtect);

	return true;
}

bool CSelfProtection::IsDumpTriggered()
{
	if (!s_pGuardMem)
	{
		DEBUG_LOG(LL_ERR, "Null guard ptr!");
		return true;
	}

	PSAPI_WORKING_SET_EX_INFORMATION pworkingSetExInformation = { s_pGuardMem, NULL };

	auto ntStatus = g_winapiApiTable->NtQueryVirtualMemory(NtCurrentProcess, NULL, MemoryWorkingSetExInformation, &pworkingSetExInformation, sizeof(pworkingSetExInformation), NULL);
	if (NT_SUCCESS(ntStatus))
	{
		if (pworkingSetExInformation.VirtualAttributes.Valid)
			return true;
	}

	return false;
}


