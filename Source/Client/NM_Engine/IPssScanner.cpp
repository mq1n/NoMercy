#include "NM_Index.h"
#include "NM_Main.h"
#include "Defines.h"
#include "ProcessSnapshot.h"
#include "WinVerHelper.h"

#if 0

static VOID PrintUnixEpoch32(ULONG Value)
{
	int rc;
	struct tm Time;


	rc = _gmtime32_s(&Time, (__time32_t *)&Value);

	if (rc != 0) {
		return;
	}

	wprintf(L"%04u-%02u-%02u %02u:%02u:%02u",
		Time.tm_year + 1900, Time.tm_mon + 1, Time.tm_mday,
		Time.tm_hour, Time.tm_min, Time.tm_sec);

	return;
}

static PCWSTR LookupDictionary(ULONG Value, PCKEY_VALUE_PAIR Dictionary)
{
	int i;

	for (i = 0; Dictionary[i].String; ++i) {
		if (Dictionary[i].Value == Value) {
			return Dictionary[i].String;
		}
	}

	return NULL;
}

static VOID PrintBitmap(ULONG Value, PCKEY_VALUE_PAIR Dictionary)
{
	int i;

	for (i = 0; Dictionary[i].Value; ++i) {
		if ((Value & Dictionary[i].Value) == Dictionary[i].Value) {
			wprintf(L"  %08X %s\n", Dictionary[i].Value, Dictionary[i].String);
		}
	}

	return;
}

static VOID PrintMemoryBasicInformation(PMEMORY_BASIC_INFORMATION MemBasicInfo)
{
	int i;

	wprintf(L"MEMORY_BASIC_INFORMATION:\n");
	wprintf(L"        BaseAddress: %p\n", MemBasicInfo->BaseAddress);
	wprintf(L"     AllocationBase: %p\n", MemBasicInfo->AllocationBase);

	wprintf(L"  AllocationProtect: %08X\n", MemBasicInfo->AllocationProtect);
	PRINT_MBI_BITMAP(i, PageProtections, MemBasicInfo->AllocationProtect);

	wprintf(L"         RegionSize: %08X (%u) bytes.\n",
		(ULONG)MemBasicInfo->RegionSize, (ULONG)MemBasicInfo->RegionSize);

	wprintf(L"              State: %08X\n", MemBasicInfo->State);
	PRINT_MBI_BITMAP(i, PageStates, MemBasicInfo->State);

	wprintf(L"            Protect: %08X\n", MemBasicInfo->Protect);
	PRINT_MBI_BITMAP(i, PageProtections, MemBasicInfo->Protect);

	wprintf(L"               Type: %08X\n", MemBasicInfo->Type);
	PRINT_MBI_BITMAP(i, PageTypes, MemBasicInfo->Type);

	return;
}

auto CreateSnapshot(HANDLE hProcess, HPSS * phSnapshopHandle)
{
	static const DWORD CaptureFlags =
		/* PSS_CAPTURE_VA_CLONE | */
		PSS_CAPTURE_VA_SPACE |
		PSS_CAPTURE_VA_SPACE_SECTION_INFORMATION |
		PSS_CAPTURE_HANDLE_TRACE |
		PSS_CAPTURE_HANDLES |
		PSS_CAPTURE_HANDLE_BASIC_INFORMATION |
		PSS_CAPTURE_HANDLE_TYPE_SPECIFIC_INFORMATION |
		PSS_CAPTURE_HANDLE_NAME_INFORMATION |
		PSS_CAPTURE_THREADS |
		PSS_CAPTURE_THREAD_CONTEXT |
		PSS_CREATE_MEASURE_PERFORMANCE;

	HPSS SnapshotHandle;
	auto rc = g_winapiApiTable->PssCaptureSnapshot(hProcess, (PSS_CAPTURE_FLAGS)CaptureFlags, CONTEXT_ALL, &SnapshotHandle);
	if (rc != ERROR_SUCCESS) {
		printf("PssCaptureSnapshot fail! Result: %u Error: %u", rc, GetLastError());
		return false;
	}

	if (phSnapshopHandle)
		*phSnapshopHandle = SnapshotHandle;

	return true;
}

void ParsePerformanceInformation(HPSS SnapshotHandle)
{
	PSS_PERFORMANCE_COUNTERS PerformanceCounters;

	//
	// Query snapshot: performance information.
	//
	auto rc = g_winapiApiTable->PssQuerySnapshot(SnapshotHandle, PSS_QUERY_PERFORMANCE_COUNTERS, &PerformanceCounters, sizeof(PSS_PERFORMANCE_COUNTERS));
	if (rc != ERROR_SUCCESS)
	{
		wprintf(L"PssQuerySnapshot/PSS_QUERY_PERFORMANCE_COUNTERS failed: Win32 error %u (%s).\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}
	else {
		wprintf(L"PSS_PERFORMANCE_COUNTERS:\n"
			L"TotalCycleCount: %I64u cycles.\n"
			L"VaCloneCycleCount: %I64u cycles.\n"
			L"VaSpaceCycleCount: %I64u cycles.\n"
			L"AuxPagesCycleCount: %I64u cycles.\n"
			L"HandlesCycleCount: %I64u cycles.\n"
			L"ThreadsCycleCount: %I64u cycles.\n"
			L"\n"
			L"TotalWallClockPeriod: %I64u us.\n"
			L"VaCloneWallClockPeriod: %I64u us.\n"
			L"VaSpaceWallClockPeriod: %I64u us.\n"
			L"AuxPagesWallClockPeriod: %I64u us.\n"
			L"HandlesWallClockPeriod: %I64u us.\n"
			L"ThreadsWallClockPeriod: %I64u us.\n\n",
			PerformanceCounters.TotalCycleCount,
			PerformanceCounters.VaCloneCycleCount,
			PerformanceCounters.VaSpaceCycleCount,
			PerformanceCounters.AuxPagesCycleCount,
			PerformanceCounters.HandlesCycleCount,
			PerformanceCounters.ThreadsCycleCount,
			PerformanceCounters.TotalWallClockPeriod,
			PerformanceCounters.VaCloneWallClockPeriod,
			PerformanceCounters.VaSpaceWallClockPeriod,
			PerformanceCounters.AuxPagesWallClockPeriod,
			PerformanceCounters.HandlesWallClockPeriod,
			PerformanceCounters.ThreadsWallClockPeriod);
	}
}

void ParseProcessInformation(HPSS SnapshotHandle)
{
	PSS_PROCESS_INFORMATION Process;

	//
	// Query snapshot: process information.
	//
	auto rc = g_winapiApiTable->PssQuerySnapshot(SnapshotHandle, PSS_QUERY_PROCESS_INFORMATION, &Process, sizeof(PSS_PROCESS_INFORMATION));
	if (rc != ERROR_SUCCESS)
	{
		wprintf(L"PssQuerySnapshot/PSS_QUERY_PROCESS_INFORMATION with PSS_PROCESS_INFORMATION failed: Win32 error %u (%s).\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}
	else {
		wprintf(L"PSS_PROCESS_INFORMATION:\n"
			L"           Process ID: %4u (%4x)\n"
			L"                 Path: %s\n"
			L"        Base priority: %d %s.\n"
			L"       Priority class: %08X %s.\n"
			L"        Execute flags: %08X.\n"
			L"   IsProtectedProcess: %u.\n"
			L"       IsWow64Process: %u.\n"
			L"    IsProcessDeleting: N/A.\n"
			L" IsCrossSessionCreate: N/A.\n"
			L"             IsFrozen: N/A.\n"
			L"         IsBackground: N/A.\n"
			L"      IsStronglyNamed: N/A.\n",
			(ULONG)Process.ProcessId,
			(ULONG)Process.ProcessId,
			Process.ImageFileName,
			Process.BasePriority,
			LookupDictionary(Process.BasePriority, PriorityValueDictionary),
			Process.PriorityClass,
			LookupDictionary(Process.PriorityClass, PriorityClassDictionary),
			Process.ExecuteFlags,
			((Process.Flags & PSS_PROCESS_FLAGS_PROTECTED) ? 1 : 0),
			((Process.Flags & PSS_PROCESS_FLAGS_WOW64) ? 1 : 0));
		wprintf(L"\n");
	}

}

void ParseSpaceInformation(HPSS SnapshotHandle)
{
	PSS_VA_SPACE_INFORMATION VaSpace;

	//
	// Query snapshot: VA space information.
	//
	auto rc = g_winapiApiTable->PssQuerySnapshot(SnapshotHandle, PSS_QUERY_VA_SPACE_INFORMATION, &VaSpace, sizeof(PSS_VA_SPACE_INFORMATION));
	if (rc != ERROR_SUCCESS)
	{
		wprintf(L"PssQuerySnapshot/PSS_QUERY_VA_SPACE_INFORMATION failed: Win32 error %u (%s).\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}
	else {
		wprintf(L"PSS_VA_SPACE_INFORMATION:\n"
			L"  Region count: %u.\n\n",
			VaSpace.RegionCount);
	}
}

void ParseHandleInformation(HPSS SnapshotHandle)
{
	PSS_HANDLE_INFORMATION Handle;

	//
	// Query snapshot: handle information.
	//
	auto rc = g_winapiApiTable->PssQuerySnapshot(SnapshotHandle, PSS_QUERY_HANDLE_INFORMATION, &Handle, sizeof(PSS_HANDLE_INFORMATION));

	if (rc != ERROR_SUCCESS) {
		wprintf(L"PssQuerySnapshot/PSS_QUERY_HANDLE_INFORMATION failed: Win32 error %u (%s).\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}
	else {
		wprintf(L"PSS_HANDLE_INFORMATION:\n"
			L"  Handles captured: %u.\n\n",
			Handle.HandlesCaptured);
	}
}

void ParseThreadInformation(HPSS SnapshotHandle)
{
	PSS_THREAD_INFORMATION Thread;

	//
	// Query snapshot: thread information.
	//
	auto rc = g_winapiApiTable->PssQuerySnapshot(SnapshotHandle, PSS_QUERY_THREAD_INFORMATION, &Thread, sizeof(PSS_THREAD_INFORMATION));

	if (rc != ERROR_SUCCESS) {
		wprintf(L"PssQuerySnapshot/PSS_QUERY_THREAD_INFORMATION failed: Win32 error %u (%s).\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}
	else {
		wprintf(L"PSS_THREAD_INFORMATION:\n"
			L"  Threads captured: %u.\n"
			L"    Context length: %u (0x%x) bytes.\n\n",
			Thread.ThreadsCaptured,
			Thread.ContextLength, Thread.ContextLength);
	}
}

void ParseHandleTraceInformation(HPSS SnapshotHandle)
{
	PSS_HANDLE_TRACE_INFORMATION HandleTrace;

	//
	// Query snapshot: handle trace information.
	//
	auto rc = g_winapiApiTable->PssQuerySnapshot(SnapshotHandle, PSS_QUERY_HANDLE_TRACE_INFORMATION, &HandleTrace, sizeof(PSS_HANDLE_TRACE_INFORMATION));

	if (rc != ERROR_SUCCESS) {
		wprintf(L"PssQuerySnapshot/PSS_QUERY_HANDLE_TRACE_INFORMATION failed: Win32 error %u (%s).\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}
	else {
		wprintf(L"PSS_HANDLE_TRACE_INFORMATION:\n"
			L"  Section handle: %p.\n",
			L"            Size: %4u bytes.\n\n",
			HandleTrace.SectionHandle,
			HandleTrace.Size);
	}
}

void ParseSpaceEntry(HPSS SnapshotHandle)
{
	HPSSWALK WalkMarkerHandle;
	PSS_VA_SPACE_ENTRY VaSpaceEntry;
	MEMORY_BASIC_INFORMATION MemBasicInfo;

	//
	// Walk the snapshot: VA space.
	//
	auto rc = g_winapiApiTable->PssWalkMarkerCreate(NULL, &WalkMarkerHandle);
	if (rc != ERROR_SUCCESS) {
		wprintf(L"PssWalkMarkerCreate failed: Win32 error %u (%s).\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}

	rc = g_winapiApiTable->PssWalkSnapshot(SnapshotHandle, PSS_WALK_VA_SPACE, WalkMarkerHandle, &VaSpaceEntry, sizeof(PSS_VA_SPACE_ENTRY));

	wprintf(L"PSS_VA_SPACE_ENTRY walk:\n");

	DWORD i = 0;
	while (rc == ERROR_SUCCESS)
	{
		++i;

		wprintf(L"  %2u) Descriptor:\n", i);

		ZeroMemory(&MemBasicInfo, sizeof(MEMORY_BASIC_INFORMATION));
		MemBasicInfo.BaseAddress = VaSpaceEntry.BaseAddress;
		MemBasicInfo.AllocationBase = VaSpaceEntry.AllocationBase;
		MemBasicInfo.AllocationProtect = VaSpaceEntry.AllocationProtect;
		MemBasicInfo.RegionSize = VaSpaceEntry.RegionSize;
		MemBasicInfo.State = VaSpaceEntry.State;
		MemBasicInfo.Protect = VaSpaceEntry.Protect;
		MemBasicInfo.Type = VaSpaceEntry.Type;

		PrintMemoryBasicInformation(&MemBasicInfo);
		wprintf(L"\n");

		wprintf(L"  SizeOfImage:%08X       ImageBase: %p\n"
			L"    CheckSum: %08X   TimeDateStamp: %08X\n",
			VaSpaceEntry.SizeOfImage,
			VaSpaceEntry.ImageBase,
			VaSpaceEntry.CheckSum,
			VaSpaceEntry.TimeDateStamp);

		wprintf(L"                                        ");
		PrintUnixEpoch32(VaSpaceEntry.TimeDateStamp);
		wprintf(L"\n");

		if (VaSpaceEntry.MappedFileNameLength) {
			//NT_ASSERT (VaSpaceEntry.MappedFileName);
			wprintf(L"\n    Mapped file: %s\n", VaSpaceEntry.MappedFileName);
		}

		wprintf(L"\n");

		rc = g_winapiApiTable->PssWalkSnapshot(SnapshotHandle, PSS_WALK_VA_SPACE, WalkMarkerHandle, &VaSpaceEntry, sizeof(PSS_VA_SPACE_ENTRY));
	}

	if (rc == ERROR_NO_MORE_ITEMS) {
		wprintf(L"  Reached end of VA space stream.\n\n");

	}
	else if (rc != ERROR_SUCCESS) {
		wprintf(L"PssQuerySnapshot/PSS_WALK_VA_SPACE failed: Win32 error %u (%s).\n\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}

	g_winapiApiTable->PssWalkMarkerFree(WalkMarkerHandle);
}

void ParseHandleEntry(HPSS SnapshotHandle)
{
	HPSSWALK WalkMarkerHandle;
	PSS_HANDLE_ENTRY HandleEntry;

	//
	// Walk the snapshot: handles.
	//
	auto rc = g_winapiApiTable->PssWalkMarkerCreate(NULL, &WalkMarkerHandle);
	if (rc != ERROR_SUCCESS)
	{
		wprintf(L"PssWalkMarkerCreate failed: Win32 error %u (%s).\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}

	rc = g_winapiApiTable->PssWalkSnapshot(SnapshotHandle, PSS_WALK_HANDLES, WalkMarkerHandle, &HandleEntry, sizeof(PSS_HANDLE_ENTRY));

	wprintf(L"PSS_HANDLE_ENTRY walk:\n");

	DWORD i = 0;
	while (rc == ERROR_SUCCESS)
	{
		++i;
		wprintf(L"  %2u) Handle value: %4u (%4x)  %20s (%24s) %s\n",
			i,
			(ULONG)HandleEntry.Handle,
			(ULONG)HandleEntry.Handle,
			(HandleEntry.TypeName ? HandleEntry.TypeName : L"UNKNOWN"),
			LookupDictionary(HandleEntry.ObjectType, PssObjectTypes),
			(HandleEntry.ObjectName ? HandleEntry.ObjectName : L"(nameless)"));

		switch (HandleEntry.ObjectType) {
		case PSS_OBJECT_TYPE_EVENT:
			wprintf(L"       Event: ManualReset %u   Signaled %u\n",
				HandleEntry.TypeSpecificInformation.Event.ManualReset,
				HandleEntry.TypeSpecificInformation.Event.Signaled);
			break;
		}

		rc = g_winapiApiTable->PssWalkSnapshot(SnapshotHandle, PSS_WALK_HANDLES, WalkMarkerHandle, &HandleEntry, sizeof(PSS_HANDLE_ENTRY));
	}

	if (rc == ERROR_NO_MORE_ITEMS) {
		wprintf(L"  Reached end of handle stream.\n\n");

	}
	else if (rc != ERROR_SUCCESS) {
		wprintf(L"PssQuerySnapshot/PSS_WALK_HANDLES failed: Win32 error %u (%s).\n\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}

	g_winapiApiTable->PssWalkMarkerFree(WalkMarkerHandle);
}

void ParseThreadEntry(HPSS SnapshotHandle)
{
	HPSSWALK WalkMarkerHandle;
	PSS_THREAD_ENTRY ThreadEntry;

	//
	// Walk the snapshot: threads.
	//
	auto rc = g_winapiApiTable->PssWalkMarkerCreate(NULL, &WalkMarkerHandle);
	if (rc != ERROR_SUCCESS)
	{
		wprintf(L"PssWalkMarkerCreate failed: Win32 error %u (%s).\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}

	rc = g_winapiApiTable->PssWalkSnapshot(SnapshotHandle, PSS_WALK_THREADS, WalkMarkerHandle, &ThreadEntry, sizeof(PSS_THREAD_ENTRY));

	wprintf(L"PSS_THREAD_ENTRY walk:\n");

	DWORD i = 0;
	while (rc == ERROR_SUCCESS)
	{
		++i;

		wprintf(L"  %2u) Thread ID: %4u (%4x)  suspend: %u  teb: %p  w32sa: %p  pc: %p  %s\n"
			L"      priority %d (%s)    base priority %d (%s)\n",
			i,
			(ULONG)ThreadEntry.ThreadId,
			(ULONG)ThreadEntry.ThreadId,
			ThreadEntry.SuspendCount,
			ThreadEntry.TebBaseAddress,
			ThreadEntry.Win32StartAddress,
			(ThreadEntry.ContextRecord ? CONTEXT_TO_PROGRAM_COUNTER(ThreadEntry.ContextRecord) : 0xDEADBEEF),
			((ThreadEntry.Flags & PSS_THREAD_FLAGS_TERMINATED) ? 1 : 0),
			ThreadEntry.Priority,
			LookupDictionary(ThreadEntry.Priority, PriorityValueDictionary),
			ThreadEntry.BasePriority,
			LookupDictionary(ThreadEntry.BasePriority, PriorityValueDictionary));

		rc = g_winapiApiTable->PssWalkSnapshot(SnapshotHandle, PSS_WALK_THREADS, WalkMarkerHandle, &ThreadEntry, sizeof(PSS_THREAD_ENTRY));
	}

	if (rc == ERROR_NO_MORE_ITEMS) {
		wprintf(L"  Reached end of thread stream.\n\n");

	}
	else if (rc != ERROR_SUCCESS) {
		wprintf(L"PssQuerySnapshot/PSS_WALK_THREADS failed: Win32 error %u (%s).\n\n",
			rc, LookupDictionary(rc, Win32ErrorCodes));
	}

	g_winapiApiTable->PssWalkMarkerFree(WalkMarkerHandle);
}

static void QuerySnapshot(HPSS SnapshotHandle)
{
	ParsePerformanceInformation(SnapshotHandle);
	ParseProcessInformation(SnapshotHandle);
	ParseSpaceInformation(SnapshotHandle);
	ParseHandleInformation(SnapshotHandle);
	ParseThreadInformation(SnapshotHandle);
	ParseHandleTraceInformation(SnapshotHandle);

	ParseSpaceEntry(SnapshotHandle);
	ParseHandleEntry(SnapshotHandle);
	ParseThreadEntry(SnapshotHandle);
}

bool IScanner::InitializePssSnapshotScan(HANDLE hProcess)
{
	if (IsWindows8Point1OrGreater() == false)
		return false;

	HPSS hSnapshot;
	auto bSnapshotRet = CreateSnapshot(hProcess, &hSnapshot);
	if (!bSnapshotRet)
		return false;

	QuerySnapshot(hSnapshot);

	auto rc = g_winapiApiTable->PssFreeSnapshot(hProcess, hSnapshot);
	if (rc != ERROR_SUCCESS) 
	{
		printf(xorstr("PssFreeSnapshot fail! Result: %u Error: %u").crypt_get(), rc, GetLastError());
		return false;
	}

	return true;
}

#endif

