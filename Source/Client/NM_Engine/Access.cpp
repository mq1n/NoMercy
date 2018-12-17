#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Access.h"
#include "WinVerHelper.h"
#include "Defines.h"

#pragma optimize("", off )
bool CAccess::EnablePermanentDep()
{
	/* Set up proper flags, call NtSetInformationProcess to disble RW memory execution and make it permanent */
	ULONG ExecuteFlags = MEM_EXECUTE_OPTION_DISABLE | MEM_EXECUTE_OPTION_PERMANENT;
	auto ntStatus = g_winapiApiTable->NtSetInformationProcess(NtCurrentProcess, ProcessExecuteFlags, &ExecuteFlags, sizeof(ExecuteFlags));

	if (NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_SYS, "Permanent DEP enabled!");
		return true;
	}
	
	DEBUG_LOG(LL_ERR, "Permanent DEP can NOT enabled! Ntstat: %p", ntStatus);
	return false;
}

bool CAccess::EnableNullPageProtection()
{
	/* Allocate null page and first 0x1000 bytes proceeding it */
	SIZE_T RegionSize = 0x1000;
	LPVOID lpBaseAddress = (PVOID)0x1;

	auto ntStatus = g_winapiApiTable->NtAllocateVirtualMemory(NtCurrentProcess, &lpBaseAddress, 0L, &RegionSize, MEM_COMMIT | MEM_RESERVE, PAGE_NOACCESS);
	if (NT_SUCCESS(ntStatus)) 
	{
		DEBUG_LOG(LL_SYS, "NULL Page Allocation Prevention enabled!");
		return true;
	}

	DEBUG_LOG(LL_ERR, "NULL Page Allocation Prevention can NOT enabled! Ntstat: %p", ntStatus);
	return false;
}

bool CAccess::EnableDebugPrivileges()
{
	static auto iCount = 0;

	BOOLEAN bPrevStat = TRUE;
	auto ntStatus = g_winapiApiTable->RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, TRUE, FALSE, &bPrevStat);

	if (!bPrevStat)
		iCount++;

	if (iCount > 3)
		g_nmApp->OnCloseRequest(EXIT_ERR_DEBUG_PRIV_LIMIT_EXCEED, g_winapiApiTable->GetLastError());

	return NT_SUCCESS(ntStatus);
}

#pragma optimize("", on )
