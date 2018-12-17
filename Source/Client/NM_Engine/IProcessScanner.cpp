#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "Defines.h"
#include "SafeProcessHandle.h"
#include "TempFile.h"
#include "IconHelper.h"
#include "ProcessFunctions.h"

#include <rewolf-wow64ext/src/wow64ext.h>

std::vector <std::string> GetIconHashList(const std::string & szTargetProcess)
{
	auto vIconList = std::vector<std::string>();

	HICON hIconLarge;
	HICON hIconSmall;

	int nIconCount = (int)ExtractIconExA(szTargetProcess.c_str(), -1, NULL, NULL, 0);
	for (auto t = 0; t < nIconCount; t++)
	{
		if (ExtractIconExA(szTargetProcess.c_str(), t, &hIconLarge, &hIconSmall, 1) == 0) 
		{
			SCANNER_LOG(LL_ERR, "ExtractIconExA fail! Error: %u", g_winapiApiTable->GetLastError());
			continue;
		}

		auto szTempFileName = g_nmApp->DirFunctionsInstance()->CreateTempFileName("gdic");
		SCANNER_LOG(LL_SYS, "Temp file created! File: %s", szTempFileName.c_str());
				
		if (SaveIcon(szTempFileName.c_str(), &hIconSmall, 1) == 0)
		{
			SCANNER_LOG(LL_ERR, "SaveIcon fail! Last Error: %u", g_winapiApiTable->GetLastError());

			DestroyIcon(hIconLarge);
			DestroyIcon(hIconSmall);
			continue;
		}

		auto szHash = g_nmApp->DirFunctionsInstance()->GetFileSHA256(szTempFileName);
		if (szHash.empty())
		{
			if (g_winapiApiTable->GetLastError() != ERROR_RESOURCE_TYPE_NOT_FOUND)
			{
				SCANNER_LOG(LL_ERR, "Hash can NOT created! Last Error: %u", g_winapiApiTable->GetLastError());
			}

			DestroyIcon(hIconLarge);
			DestroyIcon(hIconSmall);
			continue;
		}
		vIconList.push_back(szHash);

		DestroyIcon(hIconLarge);
		DestroyIcon(hIconSmall);
		g_winapiApiTable->DeleteFileA(szTempFileName.c_str());
	}

	return vIconList;
}

bool IScanner::CheckProcessIconHash(HANDLE hProcess)
{
	auto szProcessName = CProcessFunctions::GetProcessName(hProcess);
	if (szProcessName.empty())
	{
		SCANNER_LOG(LL_ERR, "Process name read fail! Target process: %p Error: %u", hProcess, g_winapiApiTable->GetLastError());
		return false;
	}
	SCANNER_LOG(LL_SYS, "Process image name: %s", szProcessName.c_str());

	auto vIconHashList = GetIconHashList(szProcessName);
	if (vIconHashList.empty())
	{
#if 0
		if (g_winapiApiTable->GetLastError() != ERROR_RESOURCE_TYPE_NOT_FOUND)
		{
			SCANNER_LOG(LL_ERR, "Icon hash list is empty! Last error: %u", g_winapiApiTable->GetLastError());
		}
#endif
		return false;
	}
//	SCANNER_LOG(LL_SYS, "Process icon hash list created! Size: %u", vIconHashList.size());

	for (const auto & szCurrHash : vIconHashList)
	{
		SCANNER_LOG(LL_SYS, "Current hash: %s", szCurrHash.c_str());
		g_nmApp->QuarentineInstance()->CheckProcessIconHash(szProcessName, szCurrHash);
	}
	return true;
}

bool IScanner::CheckProcessBase(HANDLE hProcess)
{
	SCANNER_LOG(LL_SYS, "Process base check routine has been started!");

	BYTE pBaseMem[12] = { 0 };
	DWORD64 pImageBase = NULL;

#ifdef _M_IX86
	if (g_nmApp->FunctionsInstance()->IsWow64Process(NtCurrentProcess))
	{
		PROCESS_BASIC_INFORMATION_WOW64 pPBI = { 0 };
		DWORD64 ntStat = g_winapiApiTable->NtWow64QueryInformationProcess64(hProcess, ProcessBasicInformation, &pPBI, sizeof(pPBI), NULL);
		if (!NT_SUCCESS(ntStat))
		{
			SCANNER_LOG(LL_ERR, "NtWow64QueryInformationProcess64(ProcessBasicInformation) fail! Target process: %p Status: %p", hProcess, ntStat);
			return false;
		}
		SCANNER_LOG(LL_SYS, "PBI data(%p) succesfully found for: %p(%u)", &pPBI, hProcess, g_winapiApiTable->GetProcessId(hProcess));

		PEB64 pPEB;
		auto dwReadByteCount = 0UL;

		ntStat = ReadProcessMemory64(hProcess, (DWORD64)pPBI.PebBaseAddress, &pPEB, sizeof(pPEB), &dwReadByteCount);
		if (!NT_SUCCESS(ntStat) || dwReadByteCount != sizeof(pPEB))
		{
			SCANNER_LOG(LL_ERR, "ReadProcessMemory64(1) fail! Target process: %p Status: %p", hProcess, ntStat);
			return false;
		}
		SCANNER_LOG(LL_SYS, "PEB data(%p) read succesfully completed", &pPEB);

		pImageBase = (DWORD64)pPEB.ImageBaseAddress;
		if (!pImageBase)
		{
			SCANNER_LOG(LL_ERR, "Process base not found! Target process: %p", hProcess);
			return false;
		}
		SCANNER_LOG(LL_SYS, "Process image base: %p", pImageBase);

		ntStat = ReadProcessMemory64(hProcess, (DWORD64)pImageBase, &pBaseMem, sizeof(pBaseMem), &dwReadByteCount);
		if (!NT_SUCCESS(ntStat) || dwReadByteCount != sizeof(pBaseMem))
		{
			SCANNER_LOG(LL_ERR, "Process base read fail! Target process: %p Status: %p", hProcess, ntStat);
			return false;
		}
	}
	else // non wow64
	{
		TODO_MSG;
	}
#elif _M_X64
	TODO_MSG
#else
#error "architecture unsupported"
#endif

	SCANNER_LOG(LL_SYS, "Process image base memory: %0x%X, %0x%X, %0x%X, %0x%X, %0x%X, %0x%X, %0x%X, %0x%X, %0x%X, %0x%X, %0x%X, %0x%X",
		pBaseMem[0], pBaseMem[1], pBaseMem[2], pBaseMem[3], pBaseMem[4], pBaseMem[5], pBaseMem[6], pBaseMem[7], pBaseMem[8], pBaseMem[9], pBaseMem[10], pBaseMem[11]);

	char szImageName[MAX_PATH];
	if (GetProcessImageFileNameA(hProcess, szImageName, MAX_PATH) <= 0)
	{
		SCANNER_LOG(LL_ERR, "Process name read fail! Target process: %p Error: %u", hProcess, GetLastError());
		return false;
	}
	SCANNER_LOG(LL_SYS, "Process image name: %s", szImageName);

	g_nmApp->QuarentineInstance()->CheckProcessBaseMem(szImageName, (PVOID64)pImageBase, pBaseMem);
	return true;
}

bool IScanner::IsScannedProcess(DWORD dwProcessId)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	return HasInVector(m_vScannedProcessIDs, dwProcessId);
}

void IScanner::OnScanTerminatedProcess(HANDLE hProcess)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	SCANNER_LOG(LL_SYS, "Terminated process scanner has been started!");

	if (!IS_VALID_HANDLE(hProcess) || !g_nmApp->DynamicWinapiInstance()->IsValidHandle(hProcess))
		return;

	auto dwProcessId = g_winapiApiTable->GetProcessId(hProcess);
	if (!dwProcessId)
		return;

	SCANNER_LOG(LL_SYS, "Target process id: %u", dwProcessId);

	if (IsScannedProcess(dwProcessId))
	{
		SCANNER_LOG(LL_SYS, "Process already scanned!");
		return;
	}

	// Add to checked list
	m_vScannedProcessIDs.push_back(dwProcessId);

	// Quick filter for system process
	if (dwProcessId <= 4)
	{
		SCANNER_LOG(LL_SYS, "System process scan passed!");
		return;
	}

	// Check target pid is it our pid
	if (g_winapiApiTable->GetCurrentProcessId() == dwProcessId)
	{
		SCANNER_LOG(LL_SYS, "Itself scan passed!");
		return;
	}

	// Check target pid is protected from ourself
	auto vLauncherProcesses = g_nmApp->InitMgrInstance()->m_vLaunchedProcesses;
	if (vLauncherProcesses.empty() == false)
	{
		for (const auto & pCurrProc : vLauncherProcesses)
		{
			if (IS_VALID_SMART_PTR(pCurrProc))
			{
				if (pCurrProc->dwProcessId == dwProcessId)
				{
					SCANNER_LOG(LL_SYS, "Target process is protected by ourself!");
					return;
				}
			}
		}
	}

	// Scan routine

	/// 1
	// Icon hash
	auto bRet = CheckProcessIconHash(hProcess);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process icon hash scan completed! Result: %d", bRet);

	/// 2
	// Scan process file
	bRet = ScanFile(hProcess, FILE_TYPE_TERMINATED_PROCESS);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process file scan routine completed! Result: %d", bRet);

	return;
}

void IScanner::OnScanProcess(DWORD dwProcessId)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	SCANNER_LOG(LL_SYS, "Process scanner has been started! Target process id: %u", dwProcessId);

	if (IsScannedProcess(dwProcessId))
	{
		SCANNER_LOG(LL_SYS, "Process already scanned!");
		return;
	}

	// Add to checked list
	m_vScannedProcessIDs.push_back(dwProcessId);

	// Quick filter for system process
	if (dwProcessId <= 4)
	{
		SCANNER_LOG(LL_SYS, "System process scan passed!");
		return;
	}

	// Check target pid is it our pid
	if (g_winapiApiTable->GetCurrentProcessId() == dwProcessId)
	{
		SCANNER_LOG(LL_SYS, "Itself scan passed!");
		return;
	}

	// Check target pid is protected from ourself
	auto vLauncherProcesses = g_nmApp->InitMgrInstance()->m_vLaunchedProcesses;
	if (vLauncherProcesses.empty() == false)
	{
		for (const auto & pCurrProc : vLauncherProcesses)
		{
			if (IS_VALID_SMART_PTR(pCurrProc))
			{
				if (pCurrProc->dwProcessId == dwProcessId)
				{
					SCANNER_LOG(LL_SYS, "Target process is protected by ourself!");
					return;
				}
			}
		}
	}

	 // todo: get handle from kernelside

	// Check quick alive status
	auto hProcessSync = g_nmApp->DynamicWinapiInstance()->NTHelper()->OpenProcess(SYNCHRONIZE, dwProcessId);
	if (!IS_VALID_HANDLE(hProcessSync))
	{
		SCANNER_LOG(LL_ERR, "Target process is not alive! Last error: %p", g_winapiApiTable->GetLastError());
		return;
	}
	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcessSync);

	// Create handle access
	auto hProcess = g_nmApp->DynamicWinapiInstance()->NTHelper()->OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_OPERATION, dwProcessId);
	if (!IS_VALID_HANDLE(hProcess))
	{
		SCANNER_LOG(LL_ERR, "Handle can NOT created for target process! Error: %p", g_winapiApiTable->GetLastError());
		return; // TODO: Send information to master server(pid, process name, directory, error code)
	}
	SCANNER_LOG(LL_SYS, "Handle: %p created for target process: %u", hProcess, dwProcessId);

	auto bRet = false;
	
	// Decrease debug priv of target process
#if 0
	bRet = g_nmApp->AccessHelperInstance()->DecreaseProcessDebugAccess(dwProcessId);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Decrease debug access completed! Result: %d", bRet);
#endif

	// Scan routine

	// TODO: Use asyncpp or concurrency for each job
	// TODO: Check process hollowing
	// TODO: Check PE manipulation 

	/// 1
	// Base scan, Entrypoint + first 12 byte
	bRet = CheckProcessBase(hProcess);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process base scan completed! Result: %d", bRet);

	/// 2
	// Icon hash
	bRet = CheckProcessIconHash(hProcess);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process icon hash scan completed! Result: %d", bRet);

	/// 3
	// Scan process file
	bRet = ScanFile(hProcess, FILE_TYPE_PROCESS);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process file scan routine completed! Result: %d", bRet);

#if 0 // deativited, ready for use
	/// 4
	// Scan process modules
	bRet = ScanProcessModules(hProcess);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process module scan routine completed! Result: %d", bRet);

	/// 5
	// Scan sections
	bRet = ScanProcessSections(hProcess);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process section scan routine completed! Result: %d", bRet);


	/// 6
	// Scan threads
	bRet = ScanProcessThreads(hProcess);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process thread scan routine completed! Result: %d", bRet);
#endif

	/// 7
	// Scan heaps
	bRet = ScanProcessHeaps(hProcess);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process heap scan routine completed! Result: %d", bRet);

	/// 8
	// Scan own windows
	bRet = ScanProcessWindows(hProcess);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Process window scan routine completed! Result: %d", bRet);

	// Drop access'd handle
	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
	return;
}

bool IScanner::ScanProcesses()
{
	SCANNER_LOG(LL_SYS, "Process scanner routine started!");

	// Check SystemExtendedProcessInformation and SystemProcessInformation and compare both of them
	auto vDifferentProcesses = GetDifferentProcessList();
	if (vDifferentProcesses.empty() == false)
	{
		SCANNER_LOG(LL_ERR, "Unknown process(es) found! Size: %u", vDifferentProcesses.size());

		for (const auto & pCurrProc : vDifferentProcesses)
		{
			SCANNER_LOG(LL_CRI, "Different process id: %u", pCurrProc->dwProcessId);
			// TODO: Send to quarentine
		}
	}

	// Scan alive processes
	auto vProcesses = ListProcessesM1();
	if (vProcesses.empty())
	{
		return false;
	}

	for (const auto & pCurrProc : vProcesses)
	{
		if (IS_VALID_SMART_PTR(pCurrProc))
		{
			OnScanProcess(pCurrProc->dwProcessId);
		}
	}

	// Check terminated processes with NtGetNextProcess
	auto processEnumerator = std::make_unique<CSafeProcessHandle>(PROCESS_QUERY_INFORMATION);
	if (!IS_VALID_SMART_PTR(processEnumerator))
	{
		return false;
	}

	auto vTerminatedProcesses = processEnumerator->EnumerateProcesses(false);
	if (vTerminatedProcesses.empty())
	{
		return false;
	}

	auto dwExitCode = 0UL;
	for (const auto & hCurrProc : vTerminatedProcesses)
	{
		if (g_winapiApiTable->GetExitCodeProcess(hCurrProc, &dwExitCode) && dwExitCode == STILL_ACTIVE)
			continue;

		OnScanTerminatedProcess(hCurrProc);
	}

	return true;
}

// ----------------------------------------------------------------------

std::vector < std::shared_ptr <SProcEnumInfo> > IScanner::ListProcessesM1()
{
	auto vOutput			= std::vector<std::shared_ptr<SProcEnumInfo>>();
	auto dwProcessInfoSize	= 2000UL;
	auto ntStat				= NTSTATUS(0x0);

	auto pProcessInfo = (PSYSTEM_EXTENDED_PROCESS_INFORMATION)malloc(dwProcessInfoSize);
	ZeroMemory(pProcessInfo, dwProcessInfoSize);

	while ((ntStat = g_winapiApiTable->NtQuerySystemInformation(SystemExtendedProcessInformation, pProcessInfo, dwProcessInfoSize, NULL)) == STATUS_INFO_LENGTH_MISMATCH)
	{
		dwProcessInfoSize *= 2;
		pProcessInfo = (PSYSTEM_EXTENDED_PROCESS_INFORMATION)realloc(pProcessInfo, dwProcessInfoSize);
	}

	if (!NT_SUCCESS(ntStat))
	{
		SCANNER_LOG(LL_ERR, "NtQuerySystemInformation failed! Error code: %u Ntstatus: %u", g_winapiApiTable->GetLastError(), ntStat);

		free(pProcessInfo);
		return vOutput;
	}

	auto pIterator = pProcessInfo;
	while (pIterator->NextEntryOffset)
	{
//		printf("%u -> %ls\n", pProcessInfo->ProcessId, pProcessInfo->ImageName.Buffer);

		auto pCurrProc = std::make_shared<SProcEnumInfo>();
		if (IS_VALID_SMART_PTR(pCurrProc))
		{
			pCurrProc->dwProcessId = pIterator->ProcessId;
//			wcscpy(pCurrProc->wszProcessName, pProcessInfo->ImageName.Buffer);

			vOutput.push_back(pCurrProc);
		}

		if (pIterator->NextEntryOffset == 0)
			break;

		pIterator = (PSYSTEM_EXTENDED_PROCESS_INFORMATION)((ULONG_PTR)pIterator + pIterator->NextEntryOffset);
	}

	free(pProcessInfo);

	return vOutput;
}

std::vector < std::shared_ptr <SProcEnumInfo> > IScanner::ListProcessesM2()
{
	auto vOutput = std::vector<std::shared_ptr<SProcEnumInfo>>();
	auto dwProcessInfoSize = 2000UL;
	auto ntStat = NTSTATUS(0x0);

	auto pProcessInfo = (PSYSTEM_PROCESSES)malloc(dwProcessInfoSize);
	ZeroMemory(pProcessInfo, dwProcessInfoSize);

	while ((ntStat = g_winapiApiTable->NtQuerySystemInformation(SystemProcessInformation, pProcessInfo, dwProcessInfoSize, NULL)) == STATUS_INFO_LENGTH_MISMATCH)
	{
		dwProcessInfoSize *= 2;
		pProcessInfo = (PSYSTEM_PROCESSES)realloc(pProcessInfo, dwProcessInfoSize);
	}

	if (!NT_SUCCESS(ntStat))
	{
		SCANNER_LOG(LL_ERR, "NtQuerySystemInformation failed! Error code: %u Ntstatus: %u", g_winapiApiTable->GetLastError(), ntStat);

		free(pProcessInfo);
		return vOutput;
	}

	auto pIterator = pProcessInfo;
	while (pIterator->NextEntryDelta)
	{
//		printf("%u -> %ls\n", pProcessInfo->ProcessId, pProcessInfo->ProcessName.Buffer);

		auto pCurrProc = std::make_shared<SProcEnumInfo>();
		if (IS_VALID_SMART_PTR(pCurrProc))
		{
			pCurrProc->dwProcessId = pIterator->ProcessId;
//			wcscpy(pCurrProc->wszProcessName, pProcessInfo->ProcessName.Buffer);

			vOutput.push_back(pCurrProc);
		}

		if (pIterator->NextEntryDelta == 0)
			break;

		pIterator = (PSYSTEM_PROCESSES)((ULONG_PTR)pIterator + pIterator->NextEntryDelta);
	}

	free(pProcessInfo);

	return vOutput;
}

std::vector < std::shared_ptr <SProcEnumInfo> > IScanner::GetDifferentProcessList()
{
	auto vOutput = std::vector<std::shared_ptr<SProcEnumInfo>>();

	auto pProcessList1 = ListProcessesM1();
	if (pProcessList1.empty())
		return vOutput;

	auto pProcessList2 = ListProcessesM2();
	if (pProcessList2.empty())
		return vOutput;

	std::sort(pProcessList1.begin(), pProcessList1.end());
	std::sort(pProcessList2.begin(), pProcessList2.end());

	auto vDifferentProcessIds = std::vector <std::shared_ptr <SProcEnumInfo> >();
	std::set_intersection
	(
		pProcessList1.begin(), pProcessList1.end(),
		pProcessList2.begin(), pProcessList2.end(),
		std::back_inserter(vDifferentProcessIds)
	);

	return vDifferentProcessIds;
}