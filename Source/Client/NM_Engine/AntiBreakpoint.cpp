#include "NM_Index.h"
#include "NM_Main.h"
#include "AntiBreakpoint.h"
#include "DynamicWinapi.h"

#pragma optimize("", off )
bool CAntiBreakpoint::HasHardwareBreakpoint()
{
	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

	if (g_winapiApiTable->GetThreadContext(NtCurrentThread, &ctx))
	{
		if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3 || ctx.Dr6 || ctx.Dr7) 
		{
			return true;
		}
	}

	return false;
}

bool CAntiBreakpoint::HasEntrypointBreakpoint()
{
	auto pIDH = reinterpret_cast<PIMAGE_DOS_HEADER>(g_winapiModuleTable->hBaseModule);
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pINH = reinterpret_cast<PIMAGE_NT_HEADERS>((reinterpret_cast<DWORD_PTR>(pIDH) + pIDH->e_lfanew));
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return false;

	auto pEntryPoint = reinterpret_cast<PBYTE>((pINH->OptionalHeader.AddressOfEntryPoint + reinterpret_cast<DWORD_PTR>(pIDH)));
	return (pEntryPoint[0] == 0xCC);
}

bool CAntiBreakpoint::HasMemoryBreakpoint()
{
	SYSTEM_INFO SystemInfo = { 0 };
	g_winapiApiTable->GetSystemInfo(&SystemInfo);

	auto pAllocation = g_winapiApiTable->VirtualAlloc(NULL, SystemInfo.dwPageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (pAllocation == NULL)
		return false;

	RtlFillMemory(pAllocation, 1, 0xC3);

	DWORD OldProtect = 0;
	if (g_winapiApiTable->VirtualProtect(pAllocation, SystemInfo.dwPageSize, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &OldProtect) == 0)
		return false;

	__try
	{
		((void(*)())pAllocation)();
	}
	__except (GetExceptionCode() == STATUS_GUARD_PAGE_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		g_winapiApiTable->VirtualFree(pAllocation, NULL, MEM_RELEASE);
		return false;
	}

	g_winapiApiTable->VirtualFree(pAllocation, NULL, MEM_RELEASE);
	return true;
}

#pragma optimize("", on )
