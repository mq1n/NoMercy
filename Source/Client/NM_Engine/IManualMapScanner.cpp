#include "NM_Main.h"
#include "NM_Index.h"
#include "Defines.h"
#include "SafeThreadHandle.h"
#include "PatternScan.h"
#include "ProcessFunctions.h"

#if 0

auto s_vThreadRegionScanList = std::vector<MEMORY_BASIC_INFORMATION>();

bool IsAddedRegion(LPVOID lpBase)
{
	for (const auto & pCurrRegion : s_vThreadRegionScanList)
	{
		if (pCurrRegion.AllocationBase == lpBase)
			return true;
	}
	return false;
}

bool ValidateHeaders(LPVOID lpData)
{
	if (!lpData)
		return false;

	auto pIDH = (PIMAGE_DOS_HEADER)lpData;
	if (!pIDH || pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pINH = (PIMAGE_NT_HEADERS)((DWORD)pIDH + (DWORD)pIDH->e_lfanew);
	if (!pINH || pINH->Signature != IMAGE_NT_SIGNATURE)
		return false;

	if (pINH->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC && pINH->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		return false;

	return true;
}

bool ValidateImage(HANDLE hProcess, MEMORY_BASIC_INFORMATION mbi, LPDWORD pdwErrorCode)
{
	if (!g_nmApp->DynamicWinapiInstance()->IsLoadedAddress((DWORD)mbi.AllocationBase)) {
		if (pdwErrorCode) *pdwErrorCode = 1;
		return false;
	}

	SIZE_T pBytesRead = 0;
	auto vBuffer = std::vector<BYTE>(mbi.RegionSize);
	if (!g_winapiApiTable->ReadProcessMemory(hProcess, (LPCVOID)mbi.AllocationBase, &vBuffer[0], mbi.RegionSize, &pBytesRead) || pBytesRead != mbi.RegionSize) {
		if (pdwErrorCode) *pdwErrorCode = 2;
		return false;
	}

	if (!ValidateHeaders(vBuffer.data())) {
		if (pdwErrorCode) *pdwErrorCode = 3;
		return false;
	}

	bool validAllocationFlags = mbi.AllocationProtect == PAGE_EXECUTE_WRITECOPY || mbi.AllocationProtect == PAGE_READONLY;
	if (mbi.Type != MEM_IMAGE || !validAllocationFlags) {
		if (pdwErrorCode) *pdwErrorCode = 4;
		return false;
	}

	return true;
}

DWORD GetInstructionPointer(HANDLE hThread)
{
	auto pThread = std::make_unique<CThread>(hThread);
	if (!pThread || !pThread.get() || !pThread->IsCorrect())
		return 0;

	auto pContext = pThread->GetContext();
	if (!pContext || !pContext.get())
		return 0;

	return pContext->Eip;
}

void IScanner::ScanForMMappedModules(HANDLE hProcess)
{
	s_vThreadRegionScanList.clear();

	auto dwPID = g_winapiApiTable->GetProcessId(hProcess);
	if (!CProcessFunctions::ProcessIsItAlive(dwPID)) {
		DEBUG_LOG(LL_ERR, "Process: %p(%u) is not running!", hProcess, dwPID);
		return;
	}

	auto threadEnumerator = std::make_unique<CSafeThreadHandle>();
	if (!threadEnumerator || !threadEnumerator.get()) {
		DEBUG_LOG(LL_ERR, "threadEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return;
	}

	auto pEnumerator = std::make_unique<CEnumerator>(dwPID);
	if (!pEnumerator || !pEnumerator.get()) {
		DEBUG_LOG(LL_ERR, "pEnumerator allocation failed! Last error: %u", g_winapiApiTable->GetLastError());
		return;
	}

	auto vSections = pEnumerator->EnumerateSections();
	if (vSections.empty()) {
		DEBUG_LOG(LL_ERR, "Section list is null!");
		return;
	}

	auto vModules = pEnumerator->EnumerateModules();
	if (vModules.empty()) {
		DEBUG_LOG(LL_ERR, "Module list is null!");
		return;
	}

	auto vThreads = threadEnumerator->EnumerateThreads(hProcess);
	if (vThreads.empty()) {
		DEBUG_LOG(LL_ERR, "Thread list is null!");
		return;
	}

	MEMORY_BASIC_INFORMATION mbi;
	for (const auto & hThread : vThreads)
	{
		auto pThread = std::make_unique<CThread>(hThread);
		if (!pThread || !pThread.get() || !pThread->IsCorrect())
			continue;

		// Eax
		auto bQueryRet = g_winapiApiTable->VirtualQuery((LPCVOID)pThread->GetStartAddress(), &mbi, sizeof(mbi));
		if (!bQueryRet)
			continue;
		
		if (mbi.AllocationBase > 0 && !IsAddedRegion(mbi.AllocationBase))
			s_vThreadRegionScanList.push_back(mbi);

		// Eip
		auto pCurrentEIP = GetInstructionPointer(hThread);

		bQueryRet = g_winapiApiTable->VirtualQuery((LPCVOID)pCurrentEIP, &mbi, sizeof(mbi));
		if (!bQueryRet)
			continue;
		
		if (mbi.AllocationBase > 0 && !IsAddedRegion(mbi.AllocationBase))
			s_vThreadRegionScanList.push_back(mbi);
	}

	DEBUG_LOG(LL_SYS, "Finished iterating threads - Scanning %u address(es)", s_vThreadRegionScanList.size());

	DWORD dwErrorCode = 0;
	for (const auto & pCurrRegion : s_vThreadRegionScanList)
	{
		if (!ValidateImage(hProcess, pCurrRegion, &dwErrorCode)) // kernel32yi yakalýyor loga error type eklendi, kontrol et 
		{
			char szFileName[2048] = { 0 };
			if (g_winapiApiTable->GetMappedFileNameA(hProcess, (LPVOID)pCurrRegion.AllocationBase, szFileName, 2048) && strlen(szFileName))
			{
				if (!strstr(szFileName, xorstr("kernel32.dll")))
				{
					DEBUG_LOG(LL_ERR, "Region: %p can not validated! Owner: %s Result: %u Error code: %u", pCurrRegion.AllocationBase, szFileName, dwErrorCode, g_winapiApiTable->GetLastError());

					auto szLog = g_nmApp->FunctionsInstance()->CreateString(xorstr("Unvalidated memory block detected! Owner: %s Type: %u Error: %u"), szFileName, dwErrorCode, g_winapiApiTable->GetLastError());
					Logf(CUSTOM_LOG_FILENAME, szLog.c_str());

					auto pCheatDetectQueueData = std::make_shared<SCheatDetectQueueData>();
					pCheatDetectQueueData->dwViolation = 999998;
					pCheatDetectQueueData->dwErrorCode = g_winapiApiTable->GetLastError();
					strcpy(pCheatDetectQueueData->szMessage, szLog.c_str());
					g_pkUnprocessedCheatObjects->InsertObject(pCheatDetectQueueData);

					//	g_nmApp->FunctionsInstance()->Close Process(szLog.c_str(), false, "");
				}
			}
		}
	}

	DEBUG_LOG(LL_SYS, "Iterating virtual pages");

	DWORD dwLastCheckedBase = 0;
	for (const auto & pCurrentSectionInfos : vSections)
	{
		// DEBUG_LOG(LL_SYS, "Current region: %p-%p Size: %p State: %p Protect: %p",
		// 	pCurrentSectionInfos->dwAllocationBase, pCurrentSectionInfos->dwBase, pCurrentSectionInfos->ulSize, pCurrentSectionInfos->dwState, pCurrentSectionInfos->dwProtect);
		
		if (!(pCurrentSectionInfos->dwState & MEM_COMMIT) || pCurrentSectionInfos->dwState == MEM_FREE)
			continue;

		if (pCurrentSectionInfos->dwProtect & PAGE_EXECUTE_READWRITE || pCurrentSectionInfos->dwProtect & PAGE_EXECUTE_WRITECOPY || 
			pCurrentSectionInfos->dwProtect & PAGE_EXECUTE_READ		 || !(pCurrentSectionInfos->dwProtect & PAGE_EXECUTE))
		{
			continue;
		}

		//if (pCurrentSectionInfos->dwType != MEM_IMAGE)
		//	continue;

		if (pCurrentSectionInfos->dwAllocationBase == 0)
			continue;

		//if already checked this address 
		if (pCurrentSectionInfos->dwAllocationBase == dwLastCheckedBase)
			continue;

		bool bResult = true;
		for (const auto & pCurrModule : vModules)
		{
			bool bSkip = false;
			auto dwCurrentBase = (DWORD)pCurrentSectionInfos->dwBase;

			if (pCurrModule->dwDllBase == pCurrentSectionInfos->dwAllocationBase)
				bSkip = true;

			if (dwCurrentBase > pCurrModule->dwDllBase && dwCurrentBase <= (pCurrModule->dwDllBase + pCurrModule->ulSizeOfImage))
				bSkip = true;

			if (bSkip) 
			{
				bResult = false;
				dwLastCheckedBase = pCurrentSectionInfos->dwAllocationBase;
				break;
			}
		}

		//DLL not found in PEB
		if (!bResult)
		{
			char szAllocFileName[2048] = { 0 };
			g_winapiApiTable->GetMappedFileNameA(hProcess, (LPVOID)pCurrentSectionInfos->dwAllocationBase, szAllocFileName, 2048);

			char szFileName[2048] = { 0 };
			g_winapiApiTable->GetMappedFileNameA(hProcess, (LPVOID)pCurrentSectionInfos->dwBase, szFileName, 2048);

			DEBUG_LOG(LL_SYS, "Base: %p(%p) - Size: %p Type: %u IsImage: %d Filename: %s(%s)",
				pCurrentSectionInfos->dwBase, pCurrentSectionInfos->dwAllocationBase, pCurrentSectionInfos->ulSize, pCurrentSectionInfos->dwType, pCurrentSectionInfos->dwType == MEM_IMAGE, szFileName, szAllocFileName);

			// whitelist; wow64cpu.dll, wow64.dll, wow64win.dll

			if (mbi.RegionSize > 400000) // 40kb
			{
				auto memScanner = std::make_unique<CMemoryScanner>(hProcess, EScanFlags::MEM_SCAN_NORMAL, pCurrentSectionInfos->dwAllocationBase, pCurrentSectionInfos->ulSize);
				if (memScanner && memScanner.get())
				{
					BYTE pBuffer1[] = { 0x73, 0x6E, 0x78, 0x68, 0x6B, 0x36, 0x34, 0x2E, 0x64, 0x6C, 0x6C };
					auto dwPattern1Ret = memScanner->Scan(CMemoryPattern(pBuffer1, xorstr("xxxxxxxxxxx"), 0x0, 0x0));

					BYTE pBuffer2[] = { 0x55, 0x8b, 0xec, 0x83, 0x7d, 0x0c, 0x01, 0x75, 0x0 };
					auto dwPattern2Ret = memScanner->Scan(CMemoryPattern(pBuffer2, xorstr("xxxxxxxx?"), 0x0, 0x0));

					BYTE pBuffer3[] = { 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x42, 0x6F, 0x78, 0x41, 0x0, 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x42, 0x6F, 0x78, 0x57 };
					auto dwPattern3Ret = memScanner->Scan(CMemoryPattern(pBuffer3, xorstr("xxxxxxxxxxxxxxxxxxxxxxx"), 0x0, 0x0));

					DEBUG_LOG(LL_ERR, "Pattern ret: %p-%p-%p", dwPattern1Ret, dwPattern2Ret, dwPattern3Ret);

					auto iPatternCode = 0;
					if (dwPattern1Ret)
						iPatternCode = 1;
					if (dwPattern2Ret)
						iPatternCode = 2;
					if (dwPattern3Ret)
						iPatternCode = 3;

					if (iPatternCode)
					{
						auto szLog = g_nmApp->FunctionsInstance()->CreateString(xorstr("Unknown memory block detected! Owner: %s Error: %d"), szFileName, iPatternCode);
						Logf(CUSTOM_LOG_FILENAME, szLog.c_str());

						auto pCheatDetectQueueData = std::make_shared<SCheatDetectQueueData>();
						pCheatDetectQueueData->dwViolation = 999996;
						pCheatDetectQueueData->dwErrorCode = g_winapiApiTable->GetLastError();
						strcpy(pCheatDetectQueueData->szMessage, szLog.c_str());
						g_pkUnprocessedCheatObjects->InsertObject(pCheatDetectQueueData);

//						g_nmApp->FunctionsInstance()->Close Process(szLog.c_str(), false, "");
					}
				}
			}

			if (pCurrentSectionInfos->dwAllocationBase != pCurrentSectionInfos->dwBase)
			{
				DEBUG_LOG(LL_ERR, "Current base: %p(%s) Allocated base: %p(%s)", 
					pCurrentSectionInfos->dwBase, szFileName, pCurrentSectionInfos->dwAllocationBase, szAllocFileName);

				auto szRealName = CProcessFunctions::DosDevicePath2LogicalPath(szFileName);
				if (szRealName.empty() == false)
				{
					DWORD dwErrorType = 0;
					if (g_nmApp->AnalyserInstance()->OnModuleLoaded(szRealName.c_str(), g_winapiApiTable->GetCurrentThreadId(), CHECK_TYPE_MANIFEST_PROBER, &dwErrorType) == false)
					{
						auto szLog = g_nmApp->FunctionsInstance()->CreateString(xorstr("Illegal Module initilization detected! Error code: 5-%u Module: %s"), dwErrorType, szRealName.c_str());
						g_nmApp->FunctionsInstance()->Close Process(szLog.c_str(), false, "");
					}
				}
			}
		}
	}
}

#endif

