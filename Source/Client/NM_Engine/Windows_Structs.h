#pragma once
#include <Windows.h>
#include <ReflectiveDLLRefresher/src/ReflectiveLoader.h>

namespace
{
	typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
	typedef NTSTATUS *PNTSTATUS;
	typedef CONST char *PCSZ;
	typedef void** PPVOID;
	typedef DWORD64 KAFFINITY64;
	typedef LONG KPRIORITY;

#define GDI_HANDLE_BUFFER_SIZE32	34
#define GDI_HANDLE_BUFFER_SIZE		GDI_HANDLE_BUFFER_SIZE32

	typedef ULONG GDI_HANDLE_BUFFER[GDI_HANDLE_BUFFER_SIZE];

	enum EGameCodes 
	{
		METIN2_GAME = 101,
	};

	typedef struct _module_data
	{
		BYTE* bMemory;
		BYTE* bFileCopy;
	}SMODULE_DATA, *PMODULE_DATA;

	enum SECTION_INHERIT
	{
		ViewShare = 1,
		ViewUnmap = 2
	};

	enum {
		MEM_EXECUTE_OPTION_DISABLE = 0x01,
		MEM_EXECUTE_OPTION_ENABLE = 0x02,
		MEM_EXECUTE_OPTION_PERMANENT = 0x08
	};

	enum THREAD_STATE
	{
		Running = 2,
		Waiting = 5,
	};

	struct handle_data {
		unsigned long process_id;
		HWND best_handle;
	};

	typedef struct WSAData2 {
		WORD                    wVersion;
		WORD                    wHighVersion;
		char                    szDescription[256 + 1];
		char                    szSystemStatus[128 + 1];
		unsigned short          iMaxSockets;
		unsigned short          iMaxUdpDg;
		char FAR *              lpVendorInfo;
	} WSADATA2, FAR * LPWSADATA2;

	typedef struct _CLIENT_ID
	{
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	} CLIENT_ID, *PCLIENT_ID;

	typedef struct _CLIENT_ID64
	{
		PVOID64 UniqueProcess;
		PVOID64 UniqueThread;
	} CLIENT_ID64, *PCLIENT_ID64;

	typedef struct _CLIENT_ID32
	{
		DWORD UniqueProcess;
		DWORD UniqueThread;
	} CLIENT_ID32, *PCLIENT_ID32;

	typedef struct _UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWSTR  Buffer;
	} UNICODE_STRING, *PUNICODE_STRING;

	typedef struct _UNICODE_STRING32
	{
		USHORT Length;
		USHORT MaximumLength;
		DWORD  Buffer;
	} UNICODE_STRING32, *PUNICODE_STRING32;

	typedef struct _module_info {
		PVOID                   BaseAddress;
		PVOID                   EntryPoint;
		ULONG                   SizeOfImage;
		UNICODE_STRING          FullDllName;
		UNICODE_STRING          BaseDllName;
	} ANTI_MODULE_INFO, *PANTI_MODULE_INFO;

	typedef enum _SYSDBG_COMMAND
	{
		SysDbgQueryModuleInformation = 0,
		SysDbgQueryTraceInformation = 1,
		SysDbgSetTracepoint = 2,
		SysDbgSetSpecialCall = 3,
		SysDbgClearSpecialCalls = 4,
		SysDbgQuerySpecialCalls = 5,
		SysDbgBreakPoint = 6,
		SysDbgQueryVersion = 7,
		SysDbgReadVirtual = 8,
		SysDbgWriteVirtual = 9,
		SysDbgReadPhysical = 10,
		SysDbgWritePhysical = 11,
		SysDbgReadControlSpace = 12,
		SysDbgWriteControlSpace = 13,
		SysDbgReadIoSpace = 14,
		SysDbgWriteIoSpace = 15,
		SysDbgReadMsr = 16,
		SysDbgWriteMsr = 17,
		SysDbgReadBusData = 18,
		SysDbgWriteBusData = 19,
		SysDbgCheckLowMemory = 20,
		SysDbgEnableKernelDebugger = 21,
		SysDbgDisableKernelDebugger = 22,
		SysDbgGetAutoKdEnable = 23,
		SysDbgSetAutoKdEnable = 24,
		SysDbgGetPrintBufferSize = 25,
		SysDbgSetPrintBufferSize = 26,
		SysDbgGetKdUmExceptionEnable = 27,
		SysDbgSetKdUmExceptionEnable = 28,
		SysDbgGetTriageDump = 29,
		SysDbgGetKdBlockEnable = 30,
		SysDbgSetKdBlockEnable = 31,
		SysDbgRegisterForUmBreakInfo = 32,
		SysDbgGetUmBreakPid = 33,
		SysDbgClearUmBreakPid = 34,
		SysDbgGetUmAttachPid = 35,
		SysDbgClearUmAttachPid = 36,
	} SYSDBG_COMMAND;


	typedef enum _KWAIT_REASON
	{
		Executive = 0,
		FreePage = 1,
		PageIn = 2,
		PoolAllocation = 3,
		DelayExecution = 4,
		Suspended = 5,
		UserRequest = 6,
		WrExecutive = 7,
		WrFreePage = 8,
		WrPageIn = 9,
		WrPoolAllocation = 10,
		WrDelayExecution = 11,
		WrSuspended = 12,
		WrUserRequest = 13,
		WrEventPair = 14,
		WrQueue = 15,
		WrLpcReceive = 16,
		WrLpcReply = 17,
		WrVirtualMemory = 18,
		WrPageOut = 19,
		WrRendezvous = 20,
		Spare2 = 21,
		Spare3 = 22,
		Spare4 = 23,
		Spare5 = 24,
		WrCalloutStack = 25,
		WrKernel = 26,
		WrResource = 27,
		WrPushLock = 28,
		WrMutex = 29,
		WrQuantumEnd = 30,
		WrDispatchInt = 31,
		WrPreempted = 32,
		WrYieldExecution = 33,
		WrFastMutex = 34,
		WrGuardedMutex = 35,
		WrRundown = 36,
		MaximumWaitReason = 37
	} KWAIT_REASON;

	typedef enum _THREADINFOCLASS
	{
		ThreadBasicInformation, // q: THREAD_BASIC_INFORMATION
		ThreadTimes, // q: KERNEL_USER_TIMES
		ThreadPriority, // s: KPRIORITY
		ThreadBasePriority, // s: LONG
		ThreadAffinityMask, // s: KAFFINITY
		ThreadImpersonationToken, // s: HANDLE
		ThreadDescriptorTableEntry,
		ThreadEnableAlignmentFaultFixup, // s: BOOLEAN
		ThreadEventPair,
		ThreadQuerySetWin32StartAddress, // q: PVOID
		ThreadZeroTlsCell, // 10
		ThreadPerformanceCount, // q: LARGE_INTEGER
		ThreadAmILastThread, // q: ULONG
		ThreadIdealProcessor, // s: ULONG
		ThreadPriorityBoost, // qs: ULONG
		ThreadSetTlsArrayAddress,
		ThreadIsIoPending, // q: ULONG
		ThreadHideFromDebugger, // s: void, BOOLEAN
		ThreadBreakOnTermination, // qs: ULONG
		ThreadSwitchLegacyState,
		ThreadIsTerminated, // 20, q: ULONG
		ThreadLastSystemCall, // q: THREAD_LAST_SYSCALL_INFORMATION
		ThreadIoPriority, // qs: ULONG
		ThreadCycleTime, // q: THREAD_CYCLE_TIME_INFORMATION
		ThreadPagePriority, // q: ULONG
		ThreadActualBasePriority,
		ThreadTebInformation, // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_SET_CONTEXT)
		ThreadCSwitchMon,
		ThreadCSwitchPmu,
		ThreadWow64Context, // q: WOW64_CONTEXT
		ThreadGroupInformation, // 30, q: GROUP_AFFINITY
		ThreadUmsInformation,
		ThreadCounterProfiling,
		ThreadIdealProcessorEx, // q: PROCESSOR_NUMBER
		ThreadCpuAccountingInformation, // since WIN8
		ThreadSuspendCount, // since WINBLUE
		MaxThreadInfoClass
	} THREADINFOCLASS;

	typedef enum _SYSTEM_INFORMATION_CLASS
	{
		SystemBasicInformation,                // q: SYSTEM_BASIC_INFORMATION
		SystemProcessorInformation,            // q: SYSTEM_PROCESSOR_INFORMATION
		SystemPerformanceInformation,          // q: SYSTEM_PERFORMANCE_INFORMATION
		SystemTimeOfDayInformation,            // q: SYSTEM_TIMEOFDAY_INFORMATION
		SystemPathInformation,                 // not implemented
		SystemProcessInformation,              // q: SYSTEM_PROCESS_INFORMATION
		SystemCallCountInformation,            // q: SYSTEM_CALL_COUNT_INFORMATION
		SystemDeviceInformation,               // q: SYSTEM_DEVICE_INFORMATION
		SystemProcessorPerformanceInformation, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
		SystemFlagsInformation,                // q: SYSTEM_FLAGS_INFORMATION
		SystemCallTimeInformation,             // not implemented // 10
		SystemModuleInformation,               // q: RTL_PROCESS_MODULES
		SystemLocksInformation,
		SystemStackTraceInformation,
		SystemPagedPoolInformation,            // not implemented
		SystemNonPagedPoolInformation,         // not implemented
		SystemHandleInformation,               // q: SYSTEM_HANDLE_INFORMATION
		SystemObjectInformation,               // q: SYSTEM_OBJECTTYPE_INFORMATION mixed with SYSTEM_OBJECT_INFORMATION
		SystemPageFileInformation,             // q: SYSTEM_PAGEFILE_INFORMATION
		SystemVdmInstemulInformation,          // q
		SystemVdmBopInformation,               // not implemented // 20
		SystemFileCacheInformation,            // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemCache)
		SystemPoolTagInformation,              // q: SYSTEM_POOLTAG_INFORMATION
		SystemInterruptInformation,            // q: SYSTEM_INTERRUPT_INFORMATION
		SystemDpcBehaviorInformation,          // q: SYSTEM_DPC_BEHAVIOR_INFORMATION; s: SYSTEM_DPC_BEHAVIOR_INFORMATION (requires SeLoadDriverPrivilege)
		SystemFullMemoryInformation,           // not implemented
		SystemLoadGdiDriverInformation,        // s (kernel-mode only)
		SystemUnloadGdiDriverInformation,      // s (kernel-mode only)
		SystemTimeAdjustmentInformation,       // q: SYSTEM_QUERY_TIME_ADJUST_INFORMATION; s: SYSTEM_SET_TIME_ADJUST_INFORMATION (requires SeSystemtimePrivilege)
		SystemSummaryMemoryInformation,        // not implemented
		SystemMirrorMemoryInformation,         // s (requires license value "Kernel-MemoryMirroringSupported") (requires SeShutdownPrivilege) // 30
		SystemPerformanceTraceInformation,     // s
		SystemObsolete0,                       // not implemented
		SystemExceptionInformation,            // q: SYSTEM_EXCEPTION_INFORMATION
		SystemCrashDumpStateInformation,       // s (requires SeDebugPrivilege)
		SystemKernelDebuggerInformation,       // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION
		SystemContextSwitchInformation,        // q: SYSTEM_CONTEXT_SWITCH_INFORMATION
		SystemRegistryQuotaInformation,        // q: SYSTEM_REGISTRY_QUOTA_INFORMATION; s (requires SeIncreaseQuotaPrivilege)
		SystemExtendServiceTableInformation,   // s (requires SeLoadDriverPrivilege) // loads win32k only
		SystemPrioritySeperation,              // s (requires SeTcbPrivilege)
		SystemVerifierAddDriverInformation,    // s (requires SeDebugPrivilege) // 40
		SystemVerifierRemoveDriverInformation, // s (requires SeDebugPrivilege)
		SystemProcessorIdleInformation,        // q: SYSTEM_PROCESSOR_IDLE_INFORMATION
		SystemLegacyDriverInformation,         // q: SYSTEM_LEGACY_DRIVER_INFORMATION
		SystemCurrentTimeZoneInformation,      // q
		SystemLookasideInformation,            // q: SYSTEM_LOOKASIDE_INFORMATION
		SystemTimeSlipNotification,            // s (requires SeSystemtimePrivilege)
		SystemSessionCreate,                   // not implemented
		SystemSessionDetach,                   // not implemented
		SystemSessionInformation,              // not implemented
		SystemRangeStartInformation,           // q // 50
		SystemVerifierInformation,             // q: SYSTEM_VERIFIER_INFORMATION; s (requires SeDebugPrivilege)
		SystemVerifierThunkExtend,             // s (kernel-mode only)
		SystemSessionProcessInformation,       // q: SYSTEM_SESSION_PROCESS_INFORMATION
		SystemLoadGdiDriverInSystemSpace,      // s (kernel-mode only) (same as SystemLoadGdiDriverInformation)
		SystemNumaProcessorMap,                // q
		SystemPrefetcherInformation,           // q: PREFETCHER_INFORMATION; s: PREFETCHER_INFORMATION // PfSnQueryPrefetcherInformation
		SystemExtendedProcessInformation,      // q: SYSTEM_PROCESS_INFORMATION
		SystemRecommendedSharedDataAlignment,  // q
		SystemComPlusPackage,                  // q; s
		SystemNumaAvailableMemory,             // 60
		SystemProcessorPowerInformation,       // q: SYSTEM_PROCESSOR_POWER_INFORMATION
		SystemEmulationBasicInformation,       // q
		SystemEmulationProcessorInformation,
		SystemExtendedHandleInformation,       // q: SYSTEM_HANDLE_INFORMATION_EX
		SystemLostDelayedWriteInformation,     // q: ULONG
		SystemBigPoolInformation,              // q: SYSTEM_BIGPOOL_INFORMATION
		SystemSessionPoolTagInformation,       // q: SYSTEM_SESSION_POOLTAG_INFORMATION
		SystemSessionMappedViewInformation,    // q: SYSTEM_SESSION_MAPPED_VIEW_INFORMATION
		SystemHotpatchInformation,             // q; s
		SystemObjectSecurityMode,              // q // 70
		SystemWatchdogTimerHandler,            // s (kernel-mode only)
		SystemWatchdogTimerInformation,        // q (kernel-mode only); s (kernel-mode only)
		SystemLogicalProcessorInformation,     // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION
		SystemWow64SharedInformationObsolete,  // not implemented
		SystemRegisterFirmwareTableInformationHandler, // s (kernel-mode only)
		SystemFirmwareTableInformation,        // not implemented
		SystemModuleInformationEx,             // q: RTL_PROCESS_MODULE_INFORMATION_EX
		SystemVerifierTriageInformation,       // not implemented
		SystemSuperfetchInformation,           // q: SUPERFETCH_INFORMATION; s: SUPERFETCH_INFORMATION // PfQuerySuperfetchInformation
		SystemMemoryListInformation,           // q: SYSTEM_MEMORY_LIST_INFORMATION; s: SYSTEM_MEMORY_LIST_COMMAND (requires SeProfileSingleProcessPrivilege) // 80
		SystemFileCacheInformationEx,          // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (same as SystemFileCacheInformation)
		SystemThreadPriorityClientIdInformation, // s: SYSTEM_THREAD_CID_PRIORITY_INFORMATION (requires SeIncreaseBasePriorityPrivilege)
		SystemProcessorIdleCycleTimeInformation, // q: SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION[]
		SystemVerifierCancellationInformation,   // not implemented // name:wow64:whNT32QuerySystemVerifierCancellationInformation
		SystemProcessorPowerInformationEx,       // not implemented
		SystemRefTraceInformation,               // q; s // ObQueryRefTraceInformation
		SystemSpecialPoolInformation,            // q; s (requires SeDebugPrivilege) // MmSpecialPoolTag, then MmSpecialPoolCatchOverruns != 0
		SystemProcessIdInformation,              // q: SYSTEM_PROCESS_ID_INFORMATION
		SystemErrorPortInformation,              // s (requires SeTcbPrivilege)
		SystemBootEnvironmentInformation,        // q: SYSTEM_BOOT_ENVIRONMENT_INFORMATION // 90
		SystemHypervisorInformation,             // q; s (kernel-mode only)
		SystemVerifierInformationEx,             // q; s
		SystemTimeZoneInformation,               // s (requires SeTimeZonePrivilege)
		SystemImageFileExecutionOptionsInformation, // s: SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION (requires SeTcbPrivilege)
		SystemCoverageInformation,                  // q; s // name:wow64:whNT32QuerySystemCoverageInformation; ExpCovQueryInformation
		SystemPrefetchPatchInformation,             // not implemented
		SystemVerifierFaultsInformation,            // s (requires SeDebugPrivilege)
		SystemSystemPartitionInformation,           // q: SYSTEM_SYSTEM_PARTITION_INFORMATION
		SystemSystemDiskInformation,                // q: SYSTEM_SYSTEM_DISK_INFORMATION
		SystemProcessorPerformanceDistribution,     // q: SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION // 100
		SystemNumaProximityNodeInformation,         // q
		SystemDynamicTimeZoneInformation,           // q; s (requires SeTimeZonePrivilege)
		SystemCodeIntegrityInformation,             // q // SeCodeIntegrityQueryInformation
		SystemProcessorMicrocodeUpdateInformation,  // s
		SystemProcessorBrandString,                 // q // HaliQuerySystemInformation -> HalpGetProcessorBrandString, info class 23
		SystemVirtualAddressInformation,            // q: SYSTEM_VA_LIST_INFORMATION[]; s: SYSTEM_VA_LIST_INFORMATION[] (requires SeIncreaseQuotaPrivilege) // MmQuerySystemVaInformation
		SystemLogicalProcessorAndGroupInformation,  // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX // since WIN7 // KeQueryLogicalProcessorRelationship
		SystemProcessorCycleTimeInformation,        // q: SYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION[]
		SystemStoreInformation,                     // q; s // SmQueryStoreInformation
		SystemRegistryAppendString,                 // s: SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS // 110
		SystemAitSamplingValue,                     // s: ULONG (requires SeProfileSingleProcessPrivilege)
		SystemVhdBootInformation,                   // q: SYSTEM_VHD_BOOT_INFORMATION
		SystemCpuQuotaInformation,                  // q; s // PsQueryCpuQuotaInformation
		SystemNativeBasicInformation,               // not implemented
		SystemSpare1,                               // not implemented
		SystemLowPriorityIoInformation,             // q: SYSTEM_LOW_PRIORITY_IO_INFORMATION
		SystemTpmBootEntropyInformation,            // q: TPM_BOOT_ENTROPY_NT_RESULT // ExQueryTpmBootEntropyInformation
		SystemVerifierCountersInformation,          // q: SYSTEM_VERIFIER_COUNTERS_INFORMATION
		SystemPagedPoolInformationEx,               // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypePagedPool)
		SystemSystemPtesInformationEx,              // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemPtes) // 120
		SystemNodeDistanceInformation,              // q
		SystemAcpiAuditInformation,                 // q: SYSTEM_ACPI_AUDIT_INFORMATION // HaliQuerySystemInformation -> HalpAuditQueryResults, info class 26
		SystemBasicPerformanceInformation,          // q: SYSTEM_BASIC_PERFORMANCE_INFORMATION // name:wow64:whNtQuerySystemInformation_SystemBasicPerformanceInformation
		SystemQueryPerformanceCounterInformation,   // q: SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION // since WIN7 SP1
		SystemSessionBigPoolInformation,            // since WIN8
		SystemBootGraphicsInformation,
		SystemScrubPhysicalMemoryInformation,
		SystemBadPageInformation,
		SystemProcessorProfileControlArea,
		SystemCombinePhysicalMemoryInformation,     // 130
		SystemEntropyInterruptTimingCallback,
		SystemConsoleInformation,
		SystemPlatformBinaryInformation,
		SystemThrottleNotificationInformation,
		SystemHypervisorProcessorCountInformation,
		SystemDeviceDataInformation,
		SystemDeviceDataEnumerationInformation,
		SystemMemoryTopologyInformation,
		SystemMemoryChannelInformation,
		SystemBootLogoInformation,               // 140
		SystemProcessorPerformanceInformationEx, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX // since WINBLUE
		SystemSpare0,
		SystemSecureBootPolicyInformation,
		SystemPageFileInformationEx,             // q: SYSTEM_PAGEFILE_INFORMATION_EX
		SystemSecureBootInformation,
		SystemEntropyInterruptTimingRawInformation,
		SystemPortableWorkspaceEfiLauncherInformation,
		SystemFullProcessInformation,           // q: SYSTEM_PROCESS_INFORMATION with SYSTEM_PROCESS_INFORMATION_EXTENSION (requires admin)
		SystemKernelDebuggerInformationEx,      // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX
		SystemBootMetadataInformation,          // 150
		SystemSoftRebootInformation,
		SystemElamCertificateInformation,
		SystemOfflineDumpConfigInformation,
		SystemProcessorFeaturesInformation,     // q: SYSTEM_PROCESSOR_FEATURES_INFORMATION
		SystemRegistryReconciliationInformation,
		SystemEdidInformation,
		SystemManufacturingInformation,            // q: SYSTEM_MANUFACTURING_INFORMATION // since THRESHOLD
		SystemEnergyEstimationConfigInformation,   // q: SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION
		SystemHypervisorDetailInformation,         // q: SYSTEM_HYPERVISOR_DETAIL_INFORMATION
		SystemProcessorCycleStatsInformation,      // q: SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION // 160
		SystemVmGenerationCountInformation,
		SystemTrustedPlatformModuleInformation,    // q: SYSTEM_TPM_INFORMATION
		SystemKernelDebuggerFlags,
		SystemCodeIntegrityPolicyInformation,
		SystemIsolatedUserModeInformation,
		SystemHardwareSecurityTestInterfaceResultsInformation,
		SystemSingleModuleInformation,             // q: SYSTEM_SINGLE_MODULE_INFORMATION
		SystemAllowedCpuSetsInformation,
		SystemDmaProtectionInformation,            // q: SYSTEM_DMA_PROTECTION_INFORMATION
		SystemInterruptCpuSetsInformation,
		SystemSecureBootPolicyFullInformation,
		SystemCodeIntegrityPolicyFullInformation,
		SystemAffinitizedInterruptProcessorInformation,
		SystemRootSiloInformation,                // q: SYSTEM_ROOT_SILO_INFORMATION
		SystemCpuSetInformation,                  // q: SYSTEM_CPU_SET_INFORMATION // since THRESHOLD2
		SystemCpuSetTagInformation,               // q: SYSTEM_CPU_SET_TAG_INFORMATION
		SystemWin32WerStartCallout,
		SystemSecureKernelProfileInformation,
		MaxSystemInfoClass
	} SYSTEM_INFORMATION_CLASS;

	typedef enum _MEMORY_INFORMATION_CLASS
	{
		MemoryBasicInformation,
		MemoryWorkingSetInformation,
		MemoryMappedFilenameInformation,
		MemoryRegionInformation,
		MemoryWorkingSetExInformation,
		MemorySharedCommitInformation
	} MEMORY_INFORMATION_CLASS;

	typedef enum _PROCESSINFOCLASS
	{
		ProcessBasicInformation, // 0, q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
		ProcessQuotaLimits, // qs: QUOTA_LIMITS, QUOTA_LIMITS_EX
		ProcessIoCounters, // q: IO_COUNTERS
		ProcessVmCounters, // q: VM_COUNTERS, VM_COUNTERS_EX
		ProcessTimes, // q: KERNEL_USER_TIMES
		ProcessBasePriority, // s: KPRIORITY
		ProcessRaisePriority, // s: ULONG
		ProcessDebugPort, // q: HANDLE
		ProcessExceptionPort, // s: HANDLE
		ProcessAccessToken, // s: PROCESS_ACCESS_TOKEN
		ProcessLdtInformation, // 10
		ProcessLdtSize,
		ProcessDefaultHardErrorMode, // qs: ULONG
		ProcessIoPortHandlers, // (kernel-mode only)
		ProcessPooledUsageAndLimits, // q: POOLED_USAGE_AND_LIMITS
		ProcessWorkingSetWatch, // q: PROCESS_WS_WATCH_INFORMATION[]; s: void
		ProcessUserModeIOPL,
		ProcessEnableAlignmentFaultFixup, // s: BOOLEAN
		ProcessPriorityClass, // qs: PROCESS_PRIORITY_CLASS
		ProcessWx86Information,
		ProcessHandleCount, // 20, q: ULONG, PROCESS_HANDLE_INFORMATION
		ProcessAffinityMask, // s: KAFFINITY
		ProcessPriorityBoost, // qs: ULONG
		ProcessDeviceMap, // qs: PROCESS_DEVICEMAP_INFORMATION, PROCESS_DEVICEMAP_INFORMATION_EX
		ProcessSessionInformation, // q: PROCESS_SESSION_INFORMATION
		ProcessForegroundInformation, // s: PROCESS_FOREGROUND_BACKGROUND
		ProcessWow64Information, // q: ULONG_PTR
		ProcessImageFileName, // q: UNICODE_STRING
		ProcessLUIDDeviceMapsEnabled, // q: ULONG
		ProcessBreakOnTermination, // qs: ULONG
		ProcessDebugObjectHandle, // 30, q: HANDLE
		ProcessDebugFlags, // qs: ULONG
		ProcessHandleTracing, // q: PROCESS_HANDLE_TRACING_QUERY; s: size 0 disables, otherwise enables
		ProcessIoPriority, // qs: ULONG
		ProcessExecuteFlags, // qs: ULONG
		ProcessResourceManagement,
		ProcessCookie, // q: ULONG
		ProcessImageInformation, // q: SECTION_IMAGE_INFORMATION
		ProcessCycleTime, // q: PROCESS_CYCLE_TIME_INFORMATION // since VISTA
		ProcessPagePriority, // q: ULONG
		ProcessInstrumentationCallback, // 40
		ProcessThreadStackAllocation, // s: PROCESS_STACK_ALLOCATION_INFORMATION, PROCESS_STACK_ALLOCATION_INFORMATION_EX
		ProcessWorkingSetWatchEx, // q: PROCESS_WS_WATCH_INFORMATION_EX[]
		ProcessImageFileNameWin32, // q: UNICODE_STRING
		ProcessImageFileMapping, // q: HANDLE (input)
		ProcessAffinityUpdateMode, // qs: PROCESS_AFFINITY_UPDATE_MODE
		ProcessMemoryAllocationMode, // qs: PROCESS_MEMORY_ALLOCATION_MODE
		ProcessGroupInformation, // q: USHORT[]
		ProcessTokenVirtualizationEnabled, // s: ULONG
		ProcessConsoleHostProcess, // q: ULONG_PTR
		ProcessWindowInformation, // 50, q: PROCESS_WINDOW_INFORMATION
		ProcessHandleInformation, // q: PROCESS_HANDLE_SNAPSHOT_INFORMATION // since WIN8
		ProcessMitigationPolicy, // s: PROCESS_MITIGATION_POLICY_INFORMATION
		ProcessDynamicFunctionTableInformation,
		ProcessHandleCheckingMode,
		ProcessKeepAliveCount, // q: PROCESS_KEEPALIVE_COUNT_INFORMATION
		ProcessRevokeFileHandles, // s: PROCESS_REVOKE_FILE_HANDLES_INFORMATION
		ProcessWorkingSetControl, // s: PROCESS_WORKING_SET_CONTROL
		ProcessHandleTable, // since WINBLUE
		ProcessCheckStackExtentsMode,
		ProcessCommandLineInformation, // 60, q: UNICODE_STRING
		ProcessProtectionInformation, // q: PS_PROTECTION
		MaxProcessInfoClass
	} PROCESSINFOCLASS;

	typedef struct _DEBUG_HEAP_INFORMATION
	{
		ULONG Base; // 0x00
		ULONG Flags; // 0x04
		USHORT Granularity; // 0x08
		USHORT Unknown; // 0x0A
		ULONG Allocated; // 0x0C
		ULONG Committed; // 0x10
		ULONG TagCount; // 0x14
		ULONG BlockCount; // 0x18
		ULONG Reserved[7]; // 0x1C
		PVOID Tags; // 0x38
		PVOID Blocks; // 0x3C Heap block pointer for this node.
	} DEBUG_HEAP_INFORMATION, *PDEBUG_HEAP_INFORMATION;
	
	typedef struct _SYSTEM_HANDLE
	{
		ULONG ProcessId;
		BYTE ObjectTypeNumber;
		BYTE Flags;
		USHORT Handle;
		PVOID Object;
		ACCESS_MASK GrantedAccess;
	} SYSTEM_HANDLE, *PSYSTEM_HANDLE;
	
	typedef struct _SYSTEM_HANDLE_INFORMATION
	{
		ULONG HandleCount;
		SYSTEM_HANDLE Handles[1];
	} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
#if 0
	typedef struct _PEB_LDR_DATA
	{
		ULONG           Length;
		BOOLEAN         Initialized;
		PVOID           SsHandle;
		LIST_ENTRY      InLoadOrderModuleList;
		LIST_ENTRY      InMemoryOrderModuleList;
		LIST_ENTRY      InInitializationOrderModuleList;
	} PEB_LDR_DATA, *PPEB_LDR_DATA;
#endif
	typedef struct _LDR_MODULE
	{
		LIST_ENTRY            InLoadOrderModuleList;
		LIST_ENTRY            InMemoryOrderModuleList;
		LIST_ENTRY            InInitializationOrderModuleList;
		PVOID                 BaseAddress;
		PVOID                 EntryPoint;
		ULONG                 SizeOfImage;
		UNICODE_STRING        FullDllName;
		UNICODE_STRING        BaseDllName;
		ULONG                 Flags;
		SHORT                 LoadCount;
		SHORT                 TlsIndex;
		LIST_ENTRY            HashTableEntry;
		ULONG                 TimeDateStamp;
	} LDR_MODULE, *PLDR_MODULE;

	typedef struct _LDR_MODULE32
	{
		LIST_ENTRY32          InLoadOrderModuleList;
		LIST_ENTRY32          InMemoryOrderModuleList;
		LIST_ENTRY32          InInitializationOrderModuleList;
		DWORD                 BaseAddress;
		DWORD                 EntryPoint;
		ULONG                 SizeOfImage;
		UNICODE_STRING32      FullDllName;
		UNICODE_STRING32      BaseDllName;
		ULONG                 Flags;
		SHORT                 LoadCount;
		SHORT                 TlsIndex;
		LIST_ENTRY32          HashTableEntry;
		ULONG                 TimeDateStamp;
	} LDR_MODULE32, *PLDR_MODULE32;

#if 0
	typedef struct _RTL_USER_PROCESS_PARAMETERS {
		BYTE           Reserved1[16];
		PVOID          Reserved2[10];
	} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;
#endif

	typedef struct _CURDIR
	{
		UNICODE_STRING DosPath;
		PVOID Handle;
	} CURDIR, *PCURDIR;

	typedef struct _RTL_DRIVE_LETTER_CURDIR
	{
		WORD Flags;
		WORD Length;
		ULONG TimeStamp;
		UNICODE_STRING DosPath;
	} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

	typedef struct _RTL_USER_PROCESS_PARAMETERS
	{
		ULONG MaximumLength;
		ULONG Length;
		ULONG Flags;
		ULONG DebugFlags;
		PVOID ConsoleHandle;
		ULONG ConsoleFlags;
		PVOID StandardInput;
		PVOID StandardOutput;
		PVOID StandardError;
		CURDIR CurrentDirectory;
		UNICODE_STRING DllPath;
		UNICODE_STRING ImagePathName;
		UNICODE_STRING CommandLine;
		PVOID Environment;
		ULONG StartingX;
		ULONG StartingY;
		ULONG CountX;
		ULONG CountY;
		ULONG CountCharsX;
		ULONG CountCharsY;
		ULONG FillAttribute;
		ULONG WindowFlags;
		ULONG ShowWindowFlags;
		UNICODE_STRING WindowTitle;
		UNICODE_STRING DesktopInfo;
		UNICODE_STRING ShellInfo;
		UNICODE_STRING RuntimeData;
		RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
	} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;
	typedef struct _PEB_FREE_BLOCK {
		_PEB_FREE_BLOCK          *Next;
		ULONG                   Size;
	} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

	typedef void(*PPEBLOCKROUTINE)(
		PVOID PebLock
		);

	typedef struct _PEB
	{
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;

		union
		{
			BOOLEAN BitField;
			struct
			{
				BOOLEAN ImageUsesLargePages : 1;
				BOOLEAN IsProtectedProcess : 1;
				BOOLEAN IsLegacyProcess : 1;
				BOOLEAN IsImageDynamicallyRelocated : 1;
				BOOLEAN SkipPatchingUser32Forwarders : 1;
				BOOLEAN IsPackagedProcess : 1;
				BOOLEAN IsAppContainer : 1;
				BOOLEAN SpareBits : 1;
			};
		};

		HANDLE Mutant;
		PVOID ImageBaseAddress;
		PPEB_LDR_DATA LoaderData;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
		PVOID SubSystemData;
		PVOID ProcessHeap;
		PRTL_CRITICAL_SECTION FastPebLock;
		PVOID AtlThunkSListPtr;
		PVOID IFEOKey;

		union
		{
			ULONG CrossProcessFlags;
			struct
			{
				ULONG ProcessInJob : 1;
				ULONG ProcessInitializing : 1;
				ULONG ProcessUsingVEH : 1;
				ULONG ProcessUsingVCH : 1;
				ULONG ProcessUsingFTH : 1;
				ULONG ReservedBits0 : 27;
			};

			ULONG EnvironmentUpdateCount;
		};

		union
		{
			PVOID KernelCallbackTable;
			PVOID UserSharedInfoPtr;
		};

		ULONG SystemReserved[1];
		ULONG AtlThunkSListPtr32;
		PVOID ApiSetMap;
		ULONG TlsExpansionCounter;
		PVOID TlsBitmap;
		ULONG TlsBitmapBits[2];
		PVOID ReadOnlySharedMemoryBase;
		PVOID HotpatchInformation;
		PPVOID ReadOnlyStaticServerData;
		PVOID AnsiCodePageData;
		PVOID OemCodePageData;
		PVOID UnicodeCaseTableData;
		ULONG NumberOfProcessors;
		ULONG NtGlobalFlag;
		LARGE_INTEGER CriticalSectionTimeout;
		SIZE_T HeapSegmentReserve;
		SIZE_T HeapSegmentCommit;
		SIZE_T HeapDeCommitTotalFreeThreshold;
		SIZE_T HeapDeCommitFreeBlockThreshold;
		ULONG NumberOfHeaps;
		ULONG MaximumNumberOfHeaps;
		PPVOID ProcessHeaps;
		PVOID GdiSharedHandleTable;
		PVOID ProcessStarterHelper;
		ULONG GdiDCAttributeList;
		PRTL_CRITICAL_SECTION LoaderLock;
		ULONG OSMajorVersion;
		ULONG OSMinorVersion;
		USHORT OSBuildNumber;
		USHORT OSCSDVersion;
		ULONG OSPlatformId;
		ULONG ImageSubsystem;
		ULONG ImageSubsystemMajorVersion;
		ULONG ImageSubsystemMinorVersion;
		ULONG_PTR ImageProcessAffinityMask;
		GDI_HANDLE_BUFFER GdiHandleBuffer;
		PVOID PostProcessInitRoutine;
		PVOID TlsExpansionBitmap;
		ULONG TlsExpansionBitmapBits[32];
		ULONG SessionId;
		ULARGE_INTEGER AppCompatFlags;
		ULARGE_INTEGER AppCompatFlagsUser;
		PVOID pShimData;
		PVOID AppCompatInfo;
		UNICODE_STRING CSDVersion;
		PVOID ActivationContextData;
		PVOID ProcessAssemblyStorageMap;
		PVOID SystemDefaultActivationContextData;
		PVOID SystemAssemblyStorageMap;
		SIZE_T MinimumStackCommit;
		PPVOID FlsCallback;
		LIST_ENTRY FlsListHead;
		PVOID FlsBitmap;
		ULONG FlsBitmapBits[FLS_MAXIMUM_AVAILABLE / (sizeof(ULONG) * 8)];
		ULONG FlsHighIndex;
		PVOID WerRegistrationData;
		PVOID WerShipAssertPtr;
		PVOID pContextData;
		PVOID pImageHeaderHash;
		union
		{
			ULONG TracingFlags;
			struct
			{
				ULONG HeapTracingEnabled : 1;
				ULONG CritSecTracingEnabled : 1;
				ULONG LibLoaderTracingEnabled : 1;
				ULONG SpareTracingBits : 29;
			};
		};

		ULONGLONG CsrServerReadOnlySharedMemoryBase;
	} PEB, *PPEB;

	typedef struct _STRING32
	{
		USHORT Length;
		USHORT MaximumLength;
		ULONG Buffer;
	} STRING32, *PSTRING32;

	typedef struct _CURDIR32
	{
		UNICODE_STRING32 DosPath;
		ULONG Handle;
	} CURDIR32, *PCURDIR32;

	typedef struct _RTL_DRIVE_LETTER_CURDIR32
	{
		USHORT Flags;
		USHORT Length;
		ULONG TimeStamp;
		STRING32 DosPath;
	} RTL_DRIVE_LETTER_CURDIR32, *PRTL_DRIVE_LETTER_CURDIR32;

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001
	typedef struct _RTL_USER_PROCESS_PARAMETERS32
	{
		ULONG MaximumLength;
		ULONG Length;
		ULONG Flags;
		ULONG DebugFlags;
		ULONG ConsoleHandle;
		ULONG ConsoleFlags;
		ULONG StandardInput;
		ULONG StandardOutput;
		ULONG StandardError;
		CURDIR32 CurrentDirectory;
		UNICODE_STRING32 DllPath;
		UNICODE_STRING32 ImagePathName;
		UNICODE_STRING32 CommandLine;
		ULONG Environment;
		ULONG StartingX;
		ULONG StartingY;
		ULONG CountX;
		ULONG CountY;
		ULONG CountCharsX;
		ULONG CountCharsY;
		ULONG FillAttribute;
		ULONG WindowFlags;
		ULONG ShowWindowFlags;
		UNICODE_STRING32 WindowTitle;
		UNICODE_STRING32 DesktopInfo;
		UNICODE_STRING32 ShellInfo;
		UNICODE_STRING32 RuntimeData;
		RTL_DRIVE_LETTER_CURDIR32 CurrentDirectories[RTL_MAX_DRIVE_LETTERS];
		ULONG EnvironmentSize;
		ULONG EnvironmentVersion;
		ULONG PackageDependencyData;
		ULONG ProcessGroupId;
	} RTL_USER_PROCESS_PARAMETERS32, *PRTL_USER_PROCESS_PARAMETERS32;

	typedef struct _PROCESS_BASIC_INFORMATION
	{
		NTSTATUS ExitStatus;
		PPEB PebBaseAddress;
		ULONG_PTR AffinityMask;
		KPRIORITY BasePriority;
		HANDLE UniqueProcessId;
		HANDLE InheritedFromUniqueProcessId;
	} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

	typedef struct _PROCESS_BASIC_INFORMATION_WOW64
	{
		PVOID     Reserved1[2];
		PVOID64   PebBaseAddress;
		PVOID     Reserved2[4];
		ULONG_PTR UniqueProcessId[2];
		PVOID     Reserved3[2];
	} PROCESS_BASIC_INFORMATION_WOW64;

	typedef struct _THREAD_BASIC_INFORMATION {
		NTSTATUS                ExitStatus;
		PVOID                   TebBaseAddress;
		CLIENT_ID               ClientId;
		KAFFINITY               AffinityMask;
		KPRIORITY               Priority;
		KPRIORITY               BasePriority;
	} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

	typedef struct BASE_RELOCATION_BLOCK {
		DWORD PageAddress;
		DWORD BlockSize;
	} BASE_RELOCATION_BLOCK, *PBASE_RELOCATION_BLOCK;

	typedef struct BASE_RELOCATION_ENTRY {
		USHORT Offset : 12;
		USHORT Type : 4;
	} BASE_RELOCATION_ENTRY, *PBASE_RELOCATION_ENTRY;

	typedef struct _OBJECT_ATTRIBUTES {
		ULONG           Length;
		HANDLE          RootDirectory;
		PUNICODE_STRING ObjectName;
		ULONG           Attributes;
		PVOID           SecurityDescriptor;
		PVOID           SecurityQualityOfService;
	}  OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

	typedef enum _DEBUGOBJECTINFOCLASS {
		DebugObjectFlags = 1,
		MaxDebugObjectInfoClass
	} DEBUGOBJECTINFOCLASS, *PDEBUGOBJECTINFOCLASS;

	typedef enum _HARDERROR_RESPONSE_OPTION {
		OptionAbortRetryIgnore,
		OptionOk,
		OptionOkCancel,
		OptionRetryCancel,
		OptionYesNo,
		OptionYesNoCancel,
		OptionShutdownSystem,
		OptionExplorerTrayBaloon,
		OptionCancelTryAgainContinue
	} HARDERROR_RESPONSE_OPTION, *PHARDERROR_RESPONSE_OPTION;

	typedef enum _HARDERROR_RESPONSE {
		ResponseReturnToCaller,
		ResponseNotHandled,
		ResponseAbort,
		ResponseCancel,
		ResponseIgnore,
		ResponseNo,
		ResponseOk,
		ResponseRetry,
		ResponseYes,
		ResponseTryAgain,
		ResponseContinue
	} HARDERROR_RESPONSE, *PHARDERROR_RESPONSE;

	typedef struct RTL_ACTIVATION_CONTEXT_STACK_FRAME*
		PRTL_ACTIVATION_CONTEXT_STACK_FRAME;

	struct RTL_ACTIVATION_CONTEXT_STACK_FRAME
	{
		PRTL_ACTIVATION_CONTEXT_STACK_FRAME Previous;
		_ACTIVATION_CONTEXT* ActivationContext;
		ULONG Flags;
	};

	struct ACTIVATION_CONTEXT_STACK
	{
		PRTL_ACTIVATION_CONTEXT_STACK_FRAME ActiveFrame;
		LIST_ENTRY FrameListCache;
		ULONG Flags;
		ULONG NextCookieSequenceNumber;
		ULONG StackId;
	};

	typedef ACTIVATION_CONTEXT_STACK* PACTIVATION_CONTEXT_STACK;

	typedef struct TEB_ACTIVE_FRAME_CONTEXT
	{
		/* 0x000 */ ULONG   Flags;
		/* 0x004 */ PSTR    FrameName;

	} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT;

	typedef struct TEB_ACTIVE_FRAME
	{
		/* 0x000 */ ULONG                       Context;  // Flags;
		/* 0x004 */ struct TEB_ACTIVE_FRAME    *Previous;
		/* 0x008 */	PTEB_ACTIVE_FRAME_CONTEXT   pContext;
	} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;

	struct LOAD_MEM_DLL_INFO : public TEB_ACTIVE_FRAME
	{
		ULONG           Flags;
		PVOID           MappedBase;
		PVOID           MemDllBase;
		SIZE_T          DllBufferSize;
		SIZE_T          ViewSize;
		UNICODE_STRING  Lz32Path;

		union
		{
			HANDLE DllFileHandle;
			HANDLE SectionHandle;
		};

		UNICODE_STRING  MemDllFullPath;
	};

#define GDI_BATCH_BUFFER_SIZE 310
	typedef struct _GDI_TEB_BATCH
	{
		ULONG Offset;
		ULONG_PTR HDC;
		ULONG Buffer[GDI_BATCH_BUFFER_SIZE];
	} GDI_TEB_BATCH, *PGDI_TEB_BATCH;

	typedef struct _GDI_TEB_BATCH32
	{
		ULONG Offset;
		DWORD HDC;
		ULONG Buffer[GDI_BATCH_BUFFER_SIZE];
	} GDI_TEB_BATCH32, *PGDI_TEB_BATCH32;

	typedef struct _TEB
	{
		NT_TIB NtTib;
		PVOID EnvironmentPointer;
		CLIENT_ID ClientId;
		PVOID ActiveRpcHandle;
		PVOID ThreadLocalStoragePointer;
		PPEB ProcessEnvironmentBlock;
		ULONG LastErrorValue;
		ULONG CountOfOwnedCriticalSections;
		PVOID CsrClientThread;
		PVOID Win32ThreadInfo;
		ULONG User32Reserved[26];
		ULONG UserReserved[5];
		PVOID WOW32Reserved;
		LCID CurrentLocale;
		ULONG FpSoftwareStatusRegister;
		PVOID SystemReserved1[54];
		NTSTATUS ExceptionCode;
		PVOID ActivationContextStackPointer;

#ifdef _WIN64
		UCHAR SpareBytes[24];
#else
		UCHAR SpareBytes[36];
#endif

		ULONG TxFsContext;
		GDI_TEB_BATCH GdiTebBatch;
		CLIENT_ID RealClientId;
		HANDLE GdiCachedProcessHandle;
		ULONG GdiClientPID;
		ULONG GdiClientTID;
		PVOID GdiThreadLocaleInfo;
		ULONG_PTR Win32ClientInfo[62];
		PVOID glDispatchTable[233];
		ULONG_PTR glReserved1[29];
		PVOID glReserved2;
		PVOID glSectionInfo;
		PVOID glSection;
		PVOID glTable;
		PVOID glCurrentRC;
		PVOID glContext;
		NTSTATUS LastStatusValue;
		UNICODE_STRING StaticUnicodeString;
		WCHAR StaticUnicodeBuffer[261];
		PVOID DeallocationStack;
		PVOID TlsSlots[64];
		LIST_ENTRY TlsLinks;
		PVOID Vdm;
		PVOID ReservedForNtRpc;
		PVOID DbgSsReserved[2];
		ULONG HardErrorMode;

#ifdef _WIN64
		PVOID Instrumentation[11];
#else
		PVOID Instrumentation[9];
#endif

		GUID ActivityId;
		PVOID SubProcessTag;
		PVOID EtwLocalData;
		PVOID EtwTraceData;
		PVOID WinSockData;
		ULONG GdiBatchCount;

		union
		{
			PROCESSOR_NUMBER CurrentIdealProcessor;
			ULONG IdealProcessorValue;

			struct
			{
				UCHAR ReservedPad0;
				UCHAR ReservedPad1;
				UCHAR ReservedPad2;
				UCHAR IdealProcessor;
			};
		};

		ULONG GuaranteedStackBytes;
		PVOID ReservedForPerf;
		PVOID ReservedForOle;
		ULONG WaitingOnLoaderLock;
		PVOID SavedPriorityState;
		ULONG_PTR SoftPatchPtr1;
		PVOID ThreadPoolData;
		PPVOID TlsExpansionSlots;

#ifdef _WIN64
		PVOID DeallocationBStore;
		PVOID BStoreLimit;
#endif

		ULONG MuiGeneration;
		ULONG IsImpersonating;
		PVOID NlsCache;
		PVOID pShimData;
		ULONG HeapVirtualAffinity;
		HANDLE CurrentTransactionHandle;
		PTEB_ACTIVE_FRAME ActiveFrame;
		PVOID FlsData;
		PVOID PreferredLanguages;
		PVOID UserPrefLanguages;
		PVOID MergedPrefLanguages;
		ULONG MuiImpersonation;

		union
		{
			USHORT CrossTebFlags;
			USHORT SpareCrossTebBits : 16;
		};

		union
		{
			USHORT SameTebFlags;

			struct
			{
				USHORT SafeThunkCall : 1;
				USHORT InDebugPrint : 1;
				USHORT HasFiberData : 1;
				USHORT SkipThreadAttach : 1;
				USHORT WerInShipAssertCode : 1;
				USHORT RanProcessInit : 1;
				USHORT ClonedThread : 1;
				USHORT SuppressDebugMsg : 1;
				USHORT DisableUserStackWalk : 1;
				USHORT RtlExceptionAttached : 1;
				USHORT InitialThread : 1;
				USHORT SessionAware : 1;
				USHORT SpareSameTebBits : 4;
			};
		};

		PVOID TxnScopeEnterCallback;
		PVOID TxnScopeExitCallback;
		PVOID TxnScopeContext;
		ULONG LockCount;
		ULONG SpareUlong0;
		PVOID ResourceRetValue;
		PVOID ReservedForWdf;
	} TEB, *PTEB;

#if 0
	typedef struct _LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID DllBase;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		ULONG Flags;
		SHORT LoadCount;
		SHORT TlsIndex;
		LIST_ENTRY HashTableEntry;
		ULONG TimeDateStamp;
	} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;
#endif

	typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION
	{
		BOOLEAN KernelDebuggerEnabled;
		BOOLEAN KernelDebuggerNotPresent;
	} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

	typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX
	{
		BOOLEAN DebuggerAllowed;
		BOOLEAN DebuggerEnabled;
		BOOLEAN DebuggerPresent;
	} SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION_EX;

	typedef struct _SYSTEM_KERNEL_DEBUGGER_FLAGS
	{
		BOOLEAN KernelDebuggerIgnoreUmExceptions;
	} SYSTEM_KERNEL_DEBUGGER_FLAGS, *PSYSTEM_KERNEL_DEBUGGER_FLAGS;

	typedef struct _OBJECT_TYPE_INFORMATION
	{
		UNICODE_STRING TypeName;
		ULONG TotalNumberOfObjects;
		ULONG TotalNumberOfHandles;
		ULONG TotalPagedPoolUsage;
		ULONG TotalNonPagedPoolUsage;
		ULONG TotalNamePoolUsage;
		ULONG TotalHandleTableUsage;
		ULONG HighWaterNumberOfObjects;
		ULONG HighWaterNumberOfHandles;
		ULONG HighWaterPagedPoolUsage;
		ULONG HighWaterNonPagedPoolUsage;
		ULONG HighWaterNamePoolUsage;
		ULONG HighWaterHandleTableUsage;
		ULONG InvalidAttributes;
		GENERIC_MAPPING GenericMapping;
		ULONG ValidAccessMask;
		BOOLEAN SecurityRequired;
		BOOLEAN MaintainHandleCount;
		ULONG PoolType;
		ULONG DefaultPagedPoolCharge;
		ULONG DefaultNonPagedPoolCharge;
	} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;
	typedef struct _OBJECT_TYPES_INFORMATION
	{
		ULONG NumberOfTypes;
		OBJECT_TYPE_INFORMATION TypeInformation[1];
	} OBJECT_TYPES_INFORMATION, *POBJECT_TYPES_INFORMATION;

	typedef enum _OBJECT_INFORMATION_CLASS
	{
		ObjectBasicInformation,
		ObjectNameInformation,
		ObjectTypeInformation,
		ObjectTypesInformation, //OBJECT_TYPES_INFORMATION
		ObjectHandleFlagInformation, //OBJECT_HANDLE_FLAG_INFORMATION
		ObjectSessionInformation,
		MaxObjectInfoClass  // MaxObjectInfoClass should always be the last enum
	} OBJECT_INFORMATION_CLASS;

	typedef struct _RTL_PROCESS_MODULE_INFORMATION
	{
		HANDLE Section;
		PVOID MappedBase;
		PVOID ImageBase;
		ULONG ImageSize;
		ULONG Flags;
		USHORT LoadOrderIndex;
		USHORT InitOrderIndex;
		USHORT LoadCount;
		USHORT OffsetToFileName;
		UCHAR FullPathName[256];
	} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

	typedef struct _RTL_PROCESS_MODULES
	{
		ULONG NumberOfModules;
		RTL_PROCESS_MODULE_INFORMATION Modules[1];
	} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

	typedef struct _OBJECT_NAME_INFORMATION
	{
		UNICODE_STRING Name;
	} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

	typedef struct _DNS_CACHE_ENTRY
	{
		struct _DNS_CACHE_ENTRY* Next;  // Pointer to next entry
		PCWSTR Name;                    // DNS Record Name
		USHORT Type;                    // DNS Record Type
		USHORT DataLength;              // Not referenced
		ULONG Flags;                    // DNS Record Flags
	} DNS_CACHE_ENTRY, *PDNS_CACHE_ENTRY;

	typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
		ULONG Flags;                    //Reserved.  
		PUNICODE_STRING FullDllName;   //The full path name of the DLL module.  
		PUNICODE_STRING BaseDllName;   //The base file name of the DLL module.  
		PVOID DllBase;                  //A pointer to the base address for the DLL in memory.  
		ULONG SizeOfImage;              //The size of the DLL image, in bytes.  
	} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

	typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
		ULONG Flags;                    //Reserved.  
		PUNICODE_STRING FullDllName;   //The full path name of the DLL module.  
		PUNICODE_STRING BaseDllName;   //The base file name of the DLL module.  
		PVOID DllBase;                  //A pointer to the base address for the DLL in memory.  
		ULONG SizeOfImage;              //The size of the DLL image, in bytes.  
	} LDR_DLL_UNLOADED_NOTIFICATION_DATA, *PLDR_DLL_UNLOADED_NOTIFICATION_DATA;

	typedef union _LDR_DLL_NOTIFICATION_DATA {
		LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
		LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
	} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;
	typedef const PLDR_DLL_NOTIFICATION_DATA PCLDR_DLL_NOTIFICATION_DATA;

#ifndef _WIN64
	typedef struct _RUNTIME_FUNCTION
	{
		ULONG BeginAddress;
		ULONG EndAddress;
		ULONG UnwindData;
	} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;
#endif

	enum EPDITypes
	{
		PDI_MODULES = 0x01,
		PDI_BACKTRACE = 0x02,
		PDI_HEAPS = 0x04,
		PDI_HEAP_TAGS = 0x08,
		PDI_HEAP_BLOCKS = 0x10,
		PDI_LOCKS = 0x20
	};

	typedef struct _RTL_PROCESS_BACKTRACE_INFORMATION
	{
		PVOID SymbolicBackTrace;
		ULONG TraceCount;
		USHORT Index;
		USHORT Depth;
		PVOID BackTrace[16];
	} RTL_PROCESS_BACKTRACE_INFORMATION, *PRTL_PROCESS_BACKTRACE_INFORMATION;

	// Added in version 2.05.
	typedef struct _RTL_PROCESS_BACKTRACES
	{
		ULONG CommittedMemory;
		ULONG ReservedMemory;
		ULONG NumberOfBackTraceLookups;
		ULONG NumberOfBackTraces;
		RTL_PROCESS_BACKTRACE_INFORMATION BackTraces[1];
	} RTL_PROCESS_BACKTRACES, *PRTL_PROCESS_BACKTRACES;

	// Added in version 2.05.
	typedef struct _RTL_PROCESS_LOCK_INFORMATION
	{
		PVOID Address;
		USHORT Type;
		USHORT CreatorBackTraceIndex;
		ULONG OwnerThreadId;
		ULONG ActiveCount;
		ULONG ContentionCount;
		ULONG EntryCount;
		ULONG RecursionCount;
		ULONG NumberOfSharedWaiters;
		ULONG NumberOfExclusiveWaiters;
	} RTL_PROCESS_LOCK_INFORMATION, *PRTL_PROCESS_LOCK_INFORMATION;

	// Added in version 2.05.
	typedef struct _RTL_PROCESS_LOCKS
	{
		ULONG NumberOfLocks;
		RTL_PROCESS_LOCK_INFORMATION Locks[1];
	} RTL_PROCESS_LOCKS, *PRTL_PROCESS_LOCKS;

	// Added in version 2.05, source: Process Hacker.
	typedef struct _RTL_PROCESS_VERIFIER_OPTIONS
	{
		ULONG SizeStruct;
		ULONG Option;
		UCHAR OptionData[1];
	} RTL_PROCESS_VERIFIER_OPTIONS, *PRTL_PROCESS_VERIFIER_OPTIONS;


	typedef struct _RTL_HEAP_INFORMATION
	{
		PVOID BaseAddress;
		ULONG Flags;
		USHORT EntryOverhead;
		USHORT CreatorBackTraceIndex;
		SIZE_T BytesAllocated;
		SIZE_T BytesCommitted;
		ULONG NumberOfTags;
		ULONG NumberOfEntries;
		ULONG NumberOfPseudoTags;
		ULONG PseudoTagGranularity;
		ULONG Reserved[5];
		PVOID Tags;
		PVOID Entries;
	} RTL_HEAP_INFORMATION, *PRTL_HEAP_INFORMATION;

	struct Win32HeapInformation
	{
		LONG_PTR VirtualAddress;
		LONG BlockCount;
		LONG CommittedSize;
		LONG AllocatedSize;
		LONG Flags;
	};

	typedef struct _RTL_PROCESS_HEAPS
	{
		ULONG NumberOfHeaps;
		RTL_HEAP_INFORMATION Heaps[1];
	} RTL_PROCESS_HEAPS, *PRTL_PROCESS_HEAPS;

	typedef struct _DEBUG_MODULE_INFORMATION
	{
		HANDLE Section;
		PVOID MappedBase;
		PVOID ImageBase;
		ULONG ImageSize;
		ULONG ImageFlags;
		USHORT LoadOrderIndex;
		USHORT InitOrderIndex;
		USHORT LoadCount;
		USHORT ModuleNameOffset;
		CHAR ImageName[256];
	} DEBUG_MODULE_INFORMATION, *PDEBUG_MODULE_INFORMATION;

	// Changed name in version 2.05 because of a bug.
	typedef struct _DEBUG_MODULES_STRUCT
	{
		ULONG Count;
		DEBUG_MODULE_INFORMATION DbgModInfo[1];
	} DEBUG_MODULES_STRUCT, *PDEBUG_MODULES_STRUCT;

	typedef struct _RTL_PROCESS_MODULE_INFORMATION_EX
	{
		USHORT NextOffset;
		DEBUG_MODULE_INFORMATION BaseInfo;
		ULONG ImageChecksum;
		ULONG TimeDateStamp;
		PVOID DefaultBase;
	} RTL_PROCESS_MODULE_INFORMATION_EX, *PRTL_PROCESS_MODULE_INFORMATION_EX;

	typedef struct _RTL_DEBUG_INFORMATION
	{
		HANDLE SectionHandleClient;
		PVOID ViewBaseClient;
		PVOID ViewBaseTarget;
		ULONG_PTR ViewBaseDelta;
		HANDLE EventPairClient;
		HANDLE EventPairTarget;
		HANDLE TargetProcessId;
		HANDLE TargetThreadHandle;
		ULONG Flags;
		SIZE_T OffsetFree;
		SIZE_T CommitSize;
		SIZE_T ViewSize;

		// We have two types module information structures.
		union
		{
			PDEBUG_MODULES_STRUCT Modules;
			PRTL_PROCESS_MODULE_INFORMATION_EX ModulesEx;
		};

		PRTL_PROCESS_BACKTRACES BackTraces;
		PRTL_PROCESS_HEAPS Heaps; // x86 offset should be 0x38, x64 offset should be 0x70.
		PRTL_PROCESS_LOCKS Locks;
		PVOID SpecificHeap;
		HANDLE TargetProcessHandle;
		PRTL_PROCESS_VERIFIER_OPTIONS VerifierOptions;
		PVOID ProcessHeap;
		HANDLE CriticalSectionHandle;
		HANDLE CriticalSectionOwnerThread;
		PVOID Reserved[4];
	} RTL_DEBUG_INFORMATION, *PRTL_DEBUG_INFORMATION;

	typedef enum _SEMAPHORE_INFORMATION_CLASS {
		SemaphoreBasicInformation
	} SEMAPHORE_INFORMATION_CLASS, *PSEMAPHORE_INFORMATION_CLASS;

	typedef struct _IO_STATUS_BLOCK {
		union {
			NTSTATUS Status;
			PVOID    Pointer;
		};
		ULONG_PTR Information;
	} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

	typedef struct _STRING {
		USHORT Length;
		USHORT MaximumLength;
		PCHAR  Buffer;
	} ANSI_STRING, *PANSI_STRING;

	typedef struct _FILE_BASIC_INFORMATION
	{
		LARGE_INTEGER CreationTime;
		LARGE_INTEGER LastAccessTime;
		LARGE_INTEGER LastWriteTime;
		LARGE_INTEGER ChangeTime;
		ULONG FileAttributes;
	} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

	typedef struct _SECTION_IMAGE_INFORMATION {
		PVOID					EntryPoint;
		ULONG					StackZeroBits;
		ULONG					StackReserved;
		ULONG					StackCommit;
		ULONG					ImageSubsystem;
		WORD					SubsystemVersionLow;
		WORD					SubsystemVersionHigh;
		ULONG					Unknown1;
		ULONG					ImageCharacteristics;
		ULONG					ImageMachineType;
		ULONG					Unknown2[3];
	} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

	typedef struct _RTL_USER_PROCESS_INFORMATION {
		ULONG					Size;
		HANDLE					ProcessHandle;
		HANDLE					ThreadHandle;
		CLIENT_ID				ClientId;
		SECTION_IMAGE_INFORMATION ImageInformation;
	} RTL_USER_PROCESS_INFORMATION, *PRTL_USER_PROCESS_INFORMATION;

	typedef struct _SECTION_BASIC_INFORMATION {
		PVOID         Base;
		ULONG         Attributes;
		LARGE_INTEGER Size;
	} SECTION_BASIC_INFORMATION, *PSECTION_BASIC_INFORMATION;

	typedef enum _SECTION_INFORMATION_CLASS {
		SectionBasicInformation,
		SectionImageInformation
	} SECTION_INFORMATION_CLASS, *PSECTION_INFORMATION_CLASS;

	typedef struct _VM_COUNTERS {
		// the following was inferred by painful reverse engineering
		SIZE_T		   PeakVirtualSize;	// not actually
		SIZE_T         PageFaultCount;
		SIZE_T         PeakWorkingSetSize;
		SIZE_T         WorkingSetSize;
		SIZE_T         QuotaPeakPagedPoolUsage;
		SIZE_T         QuotaPagedPoolUsage;
		SIZE_T         QuotaPeakNonPagedPoolUsage;
		SIZE_T         QuotaNonPagedPoolUsage;
		SIZE_T         PagefileUsage;
		SIZE_T         PeakPagefileUsage;
		SIZE_T         VirtualSize;		// not actually
	} VM_COUNTERS;

	typedef struct _SYSDBG_TRIAGE_DUMP
	{
		ULONG Flags;
		ULONG BugCheckCode;
		ULONG_PTR BugCheckParam1;
		ULONG_PTR BugCheckParam2;
		ULONG_PTR BugCheckParam3;
		ULONG_PTR BugCheckParam4;
		ULONG ProcessHandles;
		ULONG ThreadHandles;
		PHANDLE Handles;
	} SYSDBG_TRIAGE_DUMP, *PSYSDBG_TRIAGE_DUMP;

	typedef struct _SYSTEM_THREAD_INFORMATION {
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER CreateTime;
		ULONG WaitTime;
		PVOID StartAddress;
		CLIENT_ID ClientId;
		KPRIORITY Priority;
		LONG BasePriority;
		ULONG ContextSwitchCount;
		ULONG ThreadState;
		KWAIT_REASON WaitReason;
#ifdef _WIN64
		ULONG Reserved[4];
#endif
	}SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

	typedef struct _SYSTEM_PROCESS_INFORMATION
	{
		ULONG NextEntryOffset;
		ULONG NumberOfThreads;
		LARGE_INTEGER SpareLi1;
		LARGE_INTEGER SpareLi2;
		LARGE_INTEGER SpareLi3;
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ImageName;
		KPRIORITY BasePriority;
		HANDLE UniqueProcessId;
		HANDLE InheritedFromUniqueProcessId;
		ULONG HandleCount;
		ULONG SessionId;
		ULONG_PTR PageDirectoryBase;
		SIZE_T PeakVirtualSize;
		SIZE_T VirtualSize;
		ULONG PageFaultCount;
		SIZE_T PeakWorkingSetSize;
		SIZE_T WorkingSetSize;
		SIZE_T QuotaPeakPagedPoolUsage;
		SIZE_T QuotaPagedPoolUsage;
		SIZE_T QuotaPeakNonPagedPoolUsage;
		SIZE_T QuotaNonPagedPoolUsage;
		SIZE_T PagefileUsage;
		SIZE_T PeakPagefileUsage;
		SIZE_T PrivatePageCount;
		LARGE_INTEGER ReadOperationCount;
		LARGE_INTEGER WriteOperationCount;
		LARGE_INTEGER OtherOperationCount;
		LARGE_INTEGER ReadTransferCount;
		LARGE_INTEGER WriteTransferCount;
		LARGE_INTEGER OtherTransferCount;
		SYSTEM_THREAD_INFORMATION Threads[1];
	} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

	typedef struct _SYSTEM_EXTENDED_THREAD_INFORMATION
	{
		SYSTEM_THREAD_INFORMATION ThreadInfo;
		PVOID StackBase;
		PVOID StackLimit;
		PVOID Win32StartAddress;
		PVOID TebAddress; /* This is only filled in on Vista and above */
		ULONG Reserved1;
		ULONG Reserved2;
		ULONG Reserved3;
	} SYSTEM_EXTENDED_THREAD_INFORMATION, *PSYSTEM_EXTENDED_THREAD_INFORMATION;

	typedef struct _SYSTEM_EXTENDED_PROCESS_INFORMATION
	{
		ULONG NextEntryOffset;
		ULONG NumberOfThreads;
		LARGE_INTEGER SpareLi1;
		LARGE_INTEGER SpareLi2;
		LARGE_INTEGER SpareLi3;
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ImageName;
		KPRIORITY BasePriority;
		ULONG ProcessId;
		ULONG InheritedFromUniqueProcessId;
		ULONG HandleCount;
		ULONG SessionId;
		PVOID PageDirectoryBase;
		VM_COUNTERS VirtualMemoryCounters;
		SIZE_T PrivatePageCount;
		IO_COUNTERS IoCounters;
		SYSTEM_EXTENDED_THREAD_INFORMATION Threads[1];
	} SYSTEM_EXTENDED_PROCESS_INFORMATION, *PSYSTEM_EXTENDED_PROCESS_INFORMATION;

	typedef struct _SYSTEM_THREADS {
		LARGE_INTEGER  KernelTime;
		LARGE_INTEGER  UserTime;
		LARGE_INTEGER  CreateTime;
		ULONG          WaitTime;
		PVOID          StartAddress;
		CLIENT_ID      ClientId;
		KPRIORITY      Priority;
		KPRIORITY      BasePriority;
		ULONG          ContextSwitchCount;
		LONG           State;
		LONG           WaitReason;
	} SYSTEM_THREADS, *PSYSTEM_THREADS;

	typedef struct _SYSTEM_PROCESSES {
		ULONG			NextEntryDelta;
		ULONG			ThreadCount;
		ULONG			Reserved1[6];
		LARGE_INTEGER   CreateTime;
		LARGE_INTEGER   UserTime;
		LARGE_INTEGER   KernelTime;
		UNICODE_STRING  ProcessName;
		KPRIORITY	    BasePriority;
		ULONG			ProcessId;
		ULONG			InheritedFromProcessId;
		ULONG			HandleCount;
		ULONG			Reserved2[2];
		VM_COUNTERS	    VmCounters;
#if _WIN32_WINNT >= 0x500
		IO_COUNTERS	    IoCounters;
#endif
		SYSTEM_THREADS  Threads[1];
	} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;

	typedef struct _SYSTEM_SESSION_PROCESS_INFORMATION
	{
		ULONG SessionId;
		ULONG BufferLength;
		PVOID Buffer;
	} SYSTEM_SESSION_PROCESS_INFORMATION, *PSYSTEM_SESSION_PROCESS_INFORMATION;

	typedef struct _SYSTEM_HANDLE_EX
	{
		PVOID Object;
		HANDLE ProcessId;
		HANDLE Handle;
		ULONG GrantedAccess;
		USHORT CreatorBackTraceIndex;
		USHORT ObjectTypeIndex;
		ULONG HandleAttributes;
		ULONG Reserved;
	} SYSTEM_HANDLE_EX, *PSYSTEM_HANDLE_EX;

	typedef struct _SYSTEM_HANDLE_INFORMATION_EX
	{
		ULONG_PTR HandleCount;
		ULONG_PTR Reserved;
		PSYSTEM_HANDLE_EX Handles;
	} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

	typedef struct _SYSTEM_CODEINTEGRITY_INFORMATION
	{
		ULONG Length;
		ULONG CodeIntegrityOptions;
	} SYSTEM_CODEINTEGRITY_INFORMATION, *PSYSTEM_CODEINTEGRITY_INFORMATION;

	typedef struct _SYSTEM_SECUREBOOT_INFORMATION
	{
		BOOLEAN SecureBootEnabled;
		BOOLEAN SecureBootCapable;
	} SYSTEM_SECUREBOOT_INFORMATION, *PSYSTEM_SECUREBOOT_INFORMATION;

	typedef struct _SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION
	{
		union
		{
			ULONG Flags;
			struct
			{
				ULONG Locked : 1;
				ULONG Unlockable : 1;
				ULONG UnlockApplied : 1;
				ULONG Reserved : 29;
			};
		};
	} SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION, *PSYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION;

	typedef struct _MEMORY_SECTION_NAME
	{
		UNICODE_STRING	SectionFileName;
	} MEMORY_SECTION_NAME, *PMEMORY_SECTION_NAME;

	typedef struct _SYSTEM_BASIC_INFORMATION
	{
		ULONG Reserved;
		ULONG TimerResolution;
		ULONG PageSize;
		ULONG NumberOfPhysicalPages;
		ULONG LowestPhysicalPageNumber;
		ULONG HighestPhysicalPageNumber;
		ULONG AllocationGranularity;
		ULONG_PTR MinimumUserModeAddress;
	    ULONG_PTR MaximumUserModeAddress;
		ULONG_PTR ActiveProcessorsAffinityMask;
		CCHAR NumberOfProcessors;
	} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

	typedef struct _KSYSTEM_TIME
	{
		ULONG LowPart;
		LONG High1Time;
		LONG High2Time;
	} KSYSTEM_TIME, *PKSYSTEM_TIME;

	typedef enum _NT_PRODUCT_TYPE
	{
		NtProductWinNt = 1,
		NtProductLanManNt = 2,
		NtProductServer = 3
	} NT_PRODUCT_TYPE;

	typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE
	{
		StandardDesign = 0,
		NEC98x86 = 1,
		EndAlternatives = 2
	} ALTERNATIVE_ARCHITECTURE_TYPE;

	typedef struct _KUSER_SHARED_DATA
	{
		ULONG TickCountLowDeprecated;
		ULONG TickCountMultiplier;
		KSYSTEM_TIME InterruptTime;
		KSYSTEM_TIME SystemTime;
		KSYSTEM_TIME TimeZoneBias;
		WORD ImageNumberLow;
		WORD ImageNumberHigh;
		WCHAR NtSystemRoot[260];
		ULONG MaxStackTraceDepth;
		ULONG CryptoExponent;
		ULONG TimeZoneId;
		ULONG LargePageMinimum;
		ULONG Reserved2[7];
		NT_PRODUCT_TYPE NtProductType;
		UCHAR ProductTypeIsValid;
		ULONG NtMajorVersion;
		ULONG NtMinorVersion;
		UCHAR ProcessorFeatures[64];
		ULONG Reserved1;
		ULONG Reserved3;
		ULONG TimeSlip;
		ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;
		LARGE_INTEGER SystemExpirationDate;
		ULONG SuiteMask;
		UCHAR KdDebuggerEnabled;
		UCHAR NXSupportPolicy;
		ULONG ActiveConsoleId;
		ULONG DismountCount;
		ULONG ComPlusPackage;
		ULONG LastSystemRITEventTickCount;
		ULONG NumberOfPhysicalPages;
		UCHAR SafeBootMode;
		ULONG SharedDataFlags;
		ULONG DbgErrorPortPresent : 1;
		ULONG DbgElevationEnabled : 1;
		ULONG DbgVirtEnabled : 1;
		ULONG DbgInstallerDetectEnabled : 1;
		ULONG SystemDllRelocated : 1;
		ULONG SpareBits : 27;
		UINT64 TestRetInstruction;
		ULONG SystemCall;
		ULONG SystemCallReturn;
		UINT64 SystemCallPad[3];
		union
		{
			KSYSTEM_TIME TickCount;
			UINT64 TickCountQuad;
		};
		ULONG Cookie;
		INT64 ConsoleSessionForegroundProcessId;
		ULONG Wow64SharedInformation[16];
		WORD UserModeGlobalLogger[8];
		ULONG HeapTracingPid[2];
		ULONG CritSecTracingPid[2];
		ULONG ImageFileExecutionOptions;
		union
		{
			UINT64 AffinityPad;
			ULONG ActiveProcessorAffinity;
		};
		UINT64 InterruptTimeBias;
	} KUSER_SHARED_DATA, *PKUSER_SHARED_DATA;
};

namespace NMitigationStructs
{
	typedef struct _PROCESS_MITIGATION_ASLR_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD EnableBottomUpRandomization : 1;
				DWORD EnableForceRelocateImages : 1;
				DWORD EnableHighEntropy : 1;
				DWORD DisallowStrippedImages : 1;
				DWORD ReservedFlags : 28;
			};
		};
	} PROCESS_MITIGATION_ASLR_POLICY, *PPROCESS_MITIGATION_ASLR_POLICY;
	typedef struct _PROCESS_MITIGATION_DEP_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD Enable : 1;
				DWORD DisableAtlThunkEmulation : 1;
				DWORD ReservedFlags : 30;
			};
		};
		BOOLEAN Permanent;
	} PROCESS_MITIGATION_DEP_POLICY, *PPROCESS_MITIGATION_DEP_POLICY;

	typedef struct _PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD RaiseExceptionOnInvalidHandleReference : 1;
				DWORD HandleExceptionsPermanentlyEnabled : 1;
				DWORD ReservedFlags : 30;
			};
		};
	} PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY, *PPROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY;
	typedef struct _PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD DisallowWin32kSystemCalls : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY, *PPROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY;
	typedef struct _PROCESS_MITIGATION_DYNAMIC_CODE_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD ProhibitDynamicCode : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_DYNAMIC_CODE_POLICY, *PPROCESS_MITIGATION_DYNAMIC_CODE_POLICY;
	typedef struct _PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY {
		union {
			DWORD Flags;
			struct {
				DWORD MicrosoftSignedOnly : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY, *PPROCESS_MITIGATION_BINARY_SIGNATURE_POLICY;
	typedef struct _PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY {
		union {
			DWORD  Flags;
			struct {
				DWORD EnableControlFlowGuard : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY, *PPROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY;
	typedef struct _PROCESS_MITIGATION_IMAGE_LOAD_POLICY {
		union {
			DWORD  Flags;
			struct {
				DWORD NoRemoteImages : 1;
				DWORD NoLowMandatoryLabelImages : 1;
				DWORD PreferSystem32Images : 1;
				DWORD ReservedFlags : 29;
			};
		};
	} PROCESS_MITIGATION_IMAGE_LOAD_POLICY, *PPROCESS_MITIGATION_IMAGE_LOAD_POLICY;
	typedef struct _PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY {
		union {
			DWORD  Flags;
			struct {
				DWORD DisableExtensionPoints : 1;
				DWORD ReservedFlags : 31;
			};
		};
	} PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY, *PPROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY;


	typedef enum _PROCESS_MITIGATION_POLICY {
		ProcessDEPPolicy,
		ProcessASLRPolicy,
		ProcessDynamicCodePolicy,
		ProcessStrictHandleCheckPolicy,
		ProcessSystemCallDisablePolicy,
		ProcessMitigationOptionsMask,
		ProcessExtensionPointDisablePolicy,
		ProcessControlFlowGuardPolicy,
		ProcessSignaturePolicy,
		ProcessFontDisablePolicy,
		ProcessImageLoadPolicy,
		MaxProcessMitigationPolicy
	} PROCESS_MITIGATION_POLICY, *PPROCESS_MITIGATION_POLICY;

};

