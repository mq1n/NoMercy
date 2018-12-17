#pragma once
#include <Windows.h>
#include <vector>
#include <memory>
#include <map>
#include "Windows_Structs.h"
#include "Spdlog_Wrapper.h"

enum EFileScanTypes
{
	FILE_TYPE_NULL,
	FILE_TYPE_PROCESS,
	FILE_TYPE_TERMINATED_PROCESS,
	FILE_TYPE_MODULE,
	FILE_TYPE_SECTION
};

enum EScanIDs
{
	SCAN_NULL,
	SCAN_TEST,
	SCAN_PROCESS_BASE,
	SCAN_PROCESS_ICON_HASH,
	SCAN_FILE_EXIST_CHECK,
	SCAN_BASIC_FILE_INFORMATIONS,
	SCAN_FILE_INFORMATIONS,
	SCAN_FILE_CERTIFICATE,
	SCAN_MAPPED_FILE_PE,
	SCAN_MAPPED_FILE_SECTION_NAME,
	SCAN_MAPPED_FILE_EAT,
	SCAN_MAPPED_FILE_IAT,
	SCAN_MAPPED_FILE_REGION_HASH
};

typedef struct _PROCESS_ENUM_INFO
{
	DWORD dwProcessId;
//	wchar_t wszProcessName[MAX_PATH];
} SProcEnumInfo, *PProcEnumInfo;

typedef struct _SECTION_SCAN_CONTEXT
{
	DWORD	dwProcessId;
	DWORD64 dwBase;
	DWORD64	dwSize;
} SSectionScanContext, *PSectionScanContext;

typedef struct _MODULE_ENUM_CONTEXT
{
	DWORD64 ullBaseAddress;
	SIZE_T	uiModuleSize;
	wchar_t	wszModuleName[MAX_PATH];
} SModuleEnumContext, *PModuleEnumContext;

typedef struct _SECTION_ENUM_CONTEXT
{
	DWORD64		BaseAddress;
	DWORD64		AllocationBase;
	DWORD64		RegionSize;
	DWORD		State;
	DWORD		Protect;
	DWORD		BaseProtect;
	DWORD		Type;
} SSectionEnumContext, *PSectionEnumContext;

typedef struct _SECTION_CHECKSUM_INFORMATIONS
{
	DWORD		dwBase;
	DWORD		dwTextHash;
	DWORD		dwRDataHash;
	bool		bStatus;
} SSectionChecksumInfos, *PSectionChecksumInfos;


typedef struct _PROCESS_CALLBACK_CONTEXT
{
	DWORD	dwProcessId;
	int		iSID;
	int		iThreadCount;
	char	szFileAndPathname[MAX_PATH + 1];
	char	szFilename[MAX_PATH + 1];
	char	szClassname[128];
	char	szCommandline[512];
} SProcessCallbackCtx, *PProcessCallbackCtx;

typedef struct _THREAD_CALLBACK_CONTEXT
{
	DWORD	dwTID;
	DWORD	dwProcessId;
	DWORD	dwWaitMode;
	DWORD	dwStartAddress;
} SThreadCallbackCtx, *PThreadCallbackCtx;

typedef struct _MODULE_CALLBACK_CONTEXT
{
	DWORD	dwProcessId;
	DWORD	dwBaseAddress;
	DWORD	dwImageSize;
	char	szFilename[MAX_PATH + 1];
	char	szExecutable[MAX_PATH * 2 + 1];
} SModuleCallbackCtx, *PModuleCallbackCtx;

typedef struct _DRIVER_CALLBACK_CONTEXT
{
	char	szName[MAX_PATH + 1];
	char	szPath[512];
	char	szState[64];
	char	szType[64];
	bool	bStarted;
} SDriverCallbackCtx, *PDriverCallbackCtx;


class IScanner
{
	public:
		// Constructor & destructor

		IScanner();
		~IScanner();

		// Initilization & Finalization

		bool InitializeScanner();
		bool FinalizeScanner();

		// Local DB

		bool BuildLocalDB();
		bool CheckLocalDB();

		// Log

#ifdef _DEBUG
		auto LogInstance() { return m_logHelper; };
#endif

		// Common 
		bool RunFirstTimeScans();
		bool SendViolationMessageToMasterServer(DWORD dwViolationID, DWORD dwSystemErrorCode, const std::string & szMessage, bool bWait = false);

		/// Major

		// Process scanner

		bool ScanProcesses();
		void OnScanProcess(DWORD dwProcessId);
		void OnScanTerminatedProcess(HANDLE hProcess);
		bool IsScannedProcess(DWORD dwProcessId);

		// Window scanner

		bool ScanWindows();
		bool ScanProcessWindows(HANDLE hProcess);
		void OnScanWindow(HWND hWnd);
		bool IsScannedWindow(HWND hWnd);

		// Service scanner

		bool ScanServices();
		void OnScanService(const std::string & szServiceName, const std::string & szServiceDisplayName, DWORD dwServiceStatus);
		bool IsScannedService(const std::string & szServiceName);

		// Driver scanner

		bool ScanDrivers();
		void OnScanDriver(const std::string & szDriverPathName);
		bool IsScannedDriver(const std::string & szDriverPathName);

		// Handle scanner

		bool ScanHandles();
		void OnScanHandle(HANDLE hSourcePid, HANDLE hHandle, PVOID pObject, USHORT uTypeIndex);
		bool IsScannedHandle(HANDLE hHandle);

		// Object directory scanner
		// ...

		// Folder scanner
		// ...
		
		/// Minor

		// File Scanner

		bool ScanFile(HANDLE hProcess, EFileScanTypes fileType);
		bool ScanFile(const std::string & szFileName, EFileScanTypes fileType);
		bool IsScannedFile(const std::string & szFileName);

		// Module scanner

		bool ScanProcessModules(HANDLE hProcess);
		void ScanModule(HANDLE hProcess, const std::string & szModuleName, DWORD64 dwModuleBase, DWORD dwModuleSize);
		bool IsScannedModule(const std::string & szModuleName);

		// Section scanner

		bool ScanProcessSections(HANDLE hProcess);
		bool OnScanSection(HANDLE hProcess, DWORD64 dwBaseAddress, DWORD64 dwAllocationBase, DWORD64 dwRegionSize, DWORD dwState, DWORD dwProtect, DWORD dwBaseProtect, DWORD dwType);
		bool IsScannedSection(DWORD dwProcessId, DWORD64 dwBase, DWORD64 dwSize);

		// Thread scanner

		bool ScanProcessThreads(HANDLE hProcess);
		void OnScanThread(HANDLE hProcess, SYSTEM_EXTENDED_THREAD_INFORMATION * pCurrThread);
		bool IsScannedThread(DWORD_PTR dwThreadId);

		// Heap scanner

		bool ScanProcessHeaps(HANDLE hProcess);
		void OnScanHeap(HANDLE hProcess, PVOID64 dwBase, SIZE_T dwAllocatedSize, SIZE_T dwComittedSize, DWORD dwFlags, DWORD dwBlockCount);
		bool IsScannedHeap(HANDLE hProcess, PVOID64 dwBase, DWORD dwAllocatedSize);


		/// Extendeds
		// Big pool
		// Firmware
		// Job
		// Pss
		// Window hook

		/// Other

		bool CheckHiddenProcessWindows();
		void CheckWow32ReservedHook();
		void InitializeEventLogCheck();
		void ScanForMMappedModules(HANDLE hProcess);
		bool InitializePssSnapshotScan(HANDLE hProcess);
		bool InitializeContextScan(DWORD dwThreadId);

		/// Routines

		// Memory watchdog(Memory working set)

		bool InitializeMemoryWatchdogs();
		bool CheckMemoryWatchdogs();
		auto GetProtectedMemoryRegions();

		// Section checksum via hash

		bool InitSectionHash(DWORD dwBase); // non-thread
		bool InitSectionHashes(); // non-thread
		bool CheckSectionHashes(); // non-thread
		bool InitChecksumScan(); // thread

		/// Scan methods, master server callbacks

		void IsFileMappingExist(const std::string & szReqId, const std::string & szTargetName);
		void IsMutexExist(const std::string & szReqId, const std::string & szTargetName);
		void IsEventExist(const std::string & szReqId, const std::string & szTargetName);
		void IsSemaphoreExist(const std::string & szReqId, const std::string & szTargetName);
		void IsJobObjectExist(const std::string & szReqId, const std::string & szTargetName);
		void IsSymLinkExist(const std::string & szReqId, const std::string & szTargetName);
		void IsModuleExist(const std::string & szReqId, const std::string & szTargetName);
		void IsWindowNameExist(const std::string & szReqId, const std::string & szTargetName);
		void IsClassNameExist(const std::string & szReqId, const std::string & szTargetName);
		void IsFileExist(const std::string & szReqId, const std::string & szTargetName);
		void IsMemChecksumCorrupted(const std::string & szReqId, LPVOID lpAddress, DWORD cbSize, const std::string & szCorrectHash);
		void IsProcessRunning(const std::string & szReqId, const std::string & szTargetName);
		void IsMemCorrupted(const std::string & szReqId, LPVOID lpAddress, DWORD dwOffsetList[], std::size_t cbOffsetListSize, const std::string & szCorrectChecksum);
		void IsEbpContextCorrupted(const std::string & szReqId, DWORD dwOffset, const std::string & szCorrectChecksum);
		void CheckForegroundWindowOwner(const std::string & szReqId);
		void CheckAPIModuleBound(const std::string & szReqId, const std::string & szModuleName, const std::string & szAPIName);
		void CheckTimeManipulation(const std::string & szReqId);
		void CheckGameAlteration(const std::string & szReqId);
		void CheckSystemModuleHashs(const std::string & szReqId);

		/// Watcher callbacks

		// Window
		void OnWatcherWindowScan(HWND hWnd);															// Window watcher from WindowCallback.cpp

		// Process
		void OnWatcherProcessScan(std::shared_ptr <SProcessCallbackCtx> pProcessCtx, bool bCreated);	// Process watcher from WMI.cpp

		// Module
		void OnWatcherModuleScan(std::shared_ptr <SModuleCallbackCtx> pModuleCtx);						// Module watcher from WMI.cpp

		// Thread
		void OnWatcherThreadScan(std::shared_ptr <SThreadCallbackCtx> pThreadCtx);						// Thread watcher from WMI.cpp

		// Driver
		void OnWatcherDriverScan(std::shared_ptr <SDriverCallbackCtx> pDriverCtx);						// Driver watcher from WMI.cpp

		// Driver
		void OnWatcherDriverScan(const std::string & szDriverPathName);									// Driver watcher from kernel image callback


		/// Check methods

		bool IsFileDigitalSigned(const std::wstring & wszFileName);
		bool isValidFileCertificate(const std::wstring & wszFileName);
		bool IsTestSignEnabled(LPDWORD pdwReturnCode);
		bool IsSecureBootEnabled();

	protected:
		std::vector < std::shared_ptr <SProcEnumInfo> > ListProcessesM1(); // SystemExtendedProcessInformation
		std::vector < std::shared_ptr <SProcEnumInfo> > ListProcessesM2(); // SystemProcessInformation

		std::vector < std::shared_ptr <SProcEnumInfo> > GetDifferentProcessList();

		bool CheckProcessBase(HANDLE hProcess);
		bool CheckProcessIconHash(HANDLE hProcess);

		bool EnumerateModules(HANDLE hProcess);
		bool ScanModuleLinks(HANDLE hProcess, DWORD64 dwModuleBase);
		std::vector < std::shared_ptr <SModuleEnumContext> > GetModuleList(HANDLE hProcess);

		bool EnumerateSections(HANDLE hProcess);
		std::vector < std::shared_ptr <SSectionEnumContext> > GetSectionList(HANDLE hProcess);

		bool EnumerateThreads(HANDLE hProcess);

		bool EnumerateHeaps(HANDLE hProcess);

	private:
		mutable std::recursive_mutex	m_Mutex;
#ifdef _DEBUG
		std::shared_ptr <CLogger>		m_logHelper;
#endif

		std::vector < LPVOID >											m_pMemoryDummyPages;
		std::vector < LPVOID >											m_pMemoryWatchdogs;
		std::map	< DWORD, std::shared_ptr <SSectionChecksumInfos> >	m_mSectionChecksumInfoMap;
		std::vector < std::wstring >									m_vSignCheckedFiles;

		std::vector < DWORD >									m_vScannedProcessIDs;
		std::vector < std::string >								m_vScannedFileNames;
		std::vector < std::string >								m_vScannedModuleNames;
		std::vector < std::shared_ptr <SSectionScanContext> >	m_vScannedSections;
		std::vector < DWORD_PTR >								m_vScannedThreadIDs;
};

