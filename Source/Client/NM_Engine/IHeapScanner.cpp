#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Quarentine.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "ProcessFunctions.h"

#if 0
* Base, size, flags, block count
* Pattern
* Hash
#endif

bool IScanner::IsScannedHeap(HANDLE hProcess, PVOID64 dwBase, DWORD dwAllocatedSize)
{
	return true;
}

void IScanner::OnScanHeap(HANDLE hProcess, PVOID64 dwBase, SIZE_T dwAllocatedSize, SIZE_T dwComittedSize, DWORD dwFlags, DWORD dwBlockCount)
{

}

bool IScanner::EnumerateHeaps(HANDLE hProcess)
{
	auto dwProcessId = g_winapiApiTable->GetProcessId(hProcess);

	SCANNER_LOG(LL_SYS, "Section enumerator has been started! Target process: %p(%u)", hProcess, dwProcessId);

	auto db = g_winapiApiTable->RtlCreateQueryDebugBuffer(0, FALSE);
	if (!db)
	{
		SCANNER_LOG(LL_ERR, "RtlCreateQueryDebugBuffer fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto ntStatus = g_winapiApiTable->RtlQueryProcessDebugInformation(dwProcessId, PDI_HEAPS | PDI_HEAP_BLOCKS, db);
	if (!NT_SUCCESS(ntStatus))
	{
		SCANNER_LOG(LL_ERR, "RtlQueryProcessDebugInformation fail! Error: %u", g_winapiApiTable->GetLastError());

		g_winapiApiTable->RtlDestroyQueryDebugBuffer(db);
		return false;
	}

	for (auto i = 0UL; i < db->Heaps->NumberOfHeaps; ++i)
	{
		auto curHeap = &db->Heaps->Heaps[i];

		auto VirtualAddress = (PVOID64)curHeap->BaseAddress;
		auto BlockCount = curHeap->NumberOfEntries;
		auto CommittedSize = curHeap->BytesCommitted;
		auto AllocatedSize = curHeap->BytesAllocated;
		auto Flags = curHeap->Flags;

		DEBUG_LOG(LL_SYS, "Address: %X Block count: %u Size: %u-%u Flags: %u", VirtualAddress, BlockCount, CommittedSize, AllocatedSize, Flags);
		OnScanHeap(hProcess, VirtualAddress, AllocatedSize, CommittedSize, Flags, BlockCount);
	}

	g_winapiApiTable->RtlDestroyQueryDebugBuffer(db);
	return true;
}

bool IScanner::ScanProcessHeaps(HANDLE hProcess)
{
	SCANNER_LOG(LL_SYS, "Heap scanner has been started! Target process: %u(%p)", g_winapiApiTable->GetProcessId(hProcess), hProcess);

	if (!IS_VALID_HANDLE(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Target handle is NOT valid!");
		return true;
	}

	if (!g_nmApp->DynamicWinapiInstance()->IsValidHandle(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Target process is NOT active!");
		return true;
	}

	auto szProcessName = CProcessFunctions::GetProcessName(hProcess);
	if (szProcessName.empty())
	{
		SCANNER_LOG(LL_ERR, "Process name read fail! Target process: %p Error: %u", hProcess, g_winapiApiTable->GetLastError());
		return false;
	}
	SCANNER_LOG(LL_SYS, "Process image name: %s", szProcessName.c_str());

	return EnumerateHeaps(hProcess);
}

