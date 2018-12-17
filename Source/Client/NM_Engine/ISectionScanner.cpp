#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Quarentine.h"
#include "Defines.h"
#include "WinVerHelper.h"
#include "ProcessFunctions.h"

#include <rewolf-wow64ext/src/wow64ext.h>

// TODO: Check segment count
// TODO: Check PE informations

bool ScanSectionBase(DWORD64 dwBaseAddress, DWORD64 dwRegionSize, DWORD dwProtect)
{
	return true;
}

bool ScanSectionHash(DWORD64 dwBaseAddress, DWORD64 dwRegionSize, DWORD dwProtect)
{
	return true;
}

bool ScanSharedPage(DWORD64 dwBaseAddress, DWORD64 dwRegionSize, DWORD dwProtect)
{
	return true;
}

bool ScanPattern(DWORD64 dwBaseAddress, DWORD64 dwRegionSize, DWORD dwProtect)
{
	return true;
}

bool ScanProtectViolation(DWORD64 dwBaseAddress, DWORD64 dwRegionSize, DWORD dwBaseProtect, DWORD dwProtect)
{
	// * protection violation(current protection != base protection) ret Size + current protection + base protection
	// check is page_execute_read_Write
	return true;
}


bool IScanner::IsScannedSection(DWORD dwProcessId, DWORD64 dwBase, DWORD64 dwSize)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	for (const auto & pCurrSection : m_vScannedSections)
	{
		if (pCurrSection->dwProcessId == dwProcessId && pCurrSection->dwBase == dwBase && pCurrSection->dwSize)
		{
			return true;
		}
	}

	return false;
}

bool IScanner::OnScanSection(HANDLE hProcess, DWORD64 dwBaseAddress, DWORD64 dwAllocationBase, DWORD64 dwRegionSize, DWORD dwState, DWORD dwProtect, DWORD dwBaseProtect, DWORD dwType)
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	auto bRet							= false;
	auto pSectionContext				= std::shared_ptr<SSectionScanContext>();
	auto pSectionOwnerName				= PMEMORY_SECTION_NAME(nullptr);
	auto wszSectionName					= std::wstring(L"");
	auto szSectionName					= std::string("");
	auto dwSectionOwnerNameMaxLength	= 512UL;
	auto dwProcessId					= g_winapiApiTable->GetProcessId(hProcess);
	auto ntStatus						= 0UL;
	char szFileName[2048] = { 0 };

	SCANNER_LOG(LL_SYS, "Section scanner has been started! Target section: %p(%p) - %p Own process: %p(%u)",
		dwBaseAddress, dwAllocationBase, dwRegionSize, hProcess, dwProcessId);

	if (IsScannedSection(dwProcessId, dwBaseAddress, dwRegionSize))
	{
		SCANNER_LOG(LL_SYS, "Section already scanned!");
		goto _Complete;
	}

	// Add to checked list
	pSectionContext = std::make_shared<SSectionScanContext>();
	if (!IS_VALID_SMART_PTR(pSectionContext))
	{
		SCANNER_LOG(LL_ERR, "Section scan context can NOT allocated!");
		goto _Complete;
	}

	pSectionContext->dwProcessId	= dwProcessId;
	pSectionContext->dwBase			= dwBaseAddress;
	pSectionContext->dwSize			= dwRegionSize;

	m_vScannedSections.push_back(pSectionContext);

	// Check state
	if (!(dwState & MEM_COMMIT))
	{
		goto _Complete;
	}

//	if (mbi.State & MEM_RELEASE)
//		return;

	// Check protect
	if (dwProtect == PAGE_NOACCESS || dwProtect & PAGE_GUARD)
	{
		goto _Complete;
	}

	if (dwProtect != PAGE_READONLY && dwProtect != PAGE_READWRITE && dwProtect != PAGE_EXECUTE_READ && dwProtect != PAGE_EXECUTE_READWRITE)
	{
		goto _Complete;
	}

	// TODO: Fix section name handle fail

	// Allocate memory for owner name
	pSectionOwnerName = (PMEMORY_SECTION_NAME)g_nmApp->DynamicWinapiInstance()->NTHelper()->Alloc(dwSectionOwnerNameMaxLength, __FUNCTION__);
	if (!pSectionOwnerName)
	{
		SCANNER_LOG(LL_ERR, "Memory allocation fail for section name! Target process: %p Status: %p", hProcess, ntStatus);
		goto _Complete;
	}

	// Query owner name
	if (!VirtualQueryEx64(hProcess, dwAllocationBase, MemoryMappedFilenameInformation, (DWORD64)pSectionOwnerName, dwSectionOwnerNameMaxLength))
	{
		SCANNER_LOG(LL_ERR, "VirtualQueryEx64 fail! Target process: %p Status: %p", hProcess, ntStatus);
		goto _Complete;
	}

	if (pSectionOwnerName->SectionFileName.Length && pSectionOwnerName->SectionFileName.Buffer)
		wszSectionName = pSectionOwnerName->SectionFileName.Buffer;

	if (wszSectionName.size() > 0)
		szSectionName = std::string(wszSectionName.begin(), wszSectionName.end());

	// Scan routine

	/*
	get modulefilename on section iterator, compare with memorysectionname and module name
	GetModuleFileNameExA GetMappedFileNameA
	*/

	/// 1
	// Scan section file
	bRet = ScanFile(szSectionName, FILE_TYPE_SECTION);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Section file scan routine completed! Result: %d", bRet);

	/// 2
	// Scan section file
	bRet = ScanSectionBase(dwBaseAddress, dwRegionSize, dwProtect);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Section base scan routine completed! Result: %d", bRet);

	/// 3
	// Scan section file
	bRet = ScanSectionHash(dwBaseAddress, dwRegionSize, dwProtect);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Section hash scan routine completed! Result: %d", bRet);

	/// 4
	// Scan section file
	bRet = ScanSharedPage(dwBaseAddress, dwRegionSize, dwProtect);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Section shared page scan routine completed! Result: %d", bRet);

	/// 5
	// Scan section file
	bRet = ScanPattern(dwBaseAddress, dwRegionSize, dwProtect);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Section pattern scan routine completed! Result: %d", bRet);

	/// 6
	// Scan section file
	bRet = ScanProtectViolation(dwBaseAddress, dwRegionSize, dwBaseProtect, dwProtect);
	SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Section protect violation scan routine completed! Result: %d", bRet);

	/// 7
	// Scan module links
	if (dwType == MEM_IMAGE)
	{
		if (dwAllocationBase != dwBaseAddress)
		{
			// log
		}

		bRet = ScanModuleLinks(hProcess, dwBaseAddress);
		SCANNER_LOG(bRet ? LL_SYS : LL_ERR, "Section file scan routine completed! Result: %d", bRet);
	}

	bRet = true;

_Complete:
	if (pSectionOwnerName)
	{
		g_nmApp->DynamicWinapiInstance()->NTHelper()->Free(pSectionOwnerName);
	}
	return bRet;
}


std::vector < std::shared_ptr <SSectionEnumContext> > IScanner::GetSectionList(HANDLE hProcess)
{
	auto vOutput = std::vector<std::shared_ptr<SSectionEnumContext>>();

	DWORD64 baseAddress = 0;
	MEMORY_BASIC_INFORMATION64 basicInfo = { 0 };

	auto bufferSize = 0ULL;
	while (bufferSize = VirtualQueryEx64(hProcess, baseAddress, &basicInfo, sizeof(basicInfo)))
	{
		auto dwReadSize			= 0ULL;
		auto pCurrSectionCtx	= std::shared_ptr<SSectionEnumContext>();

#if 0
		if (basicInfo.State != MEM_COMMIT)
			goto ContinueLoop;
		if (basicInfo.Protect == PAGE_NOACCESS)
			goto ContinueLoop;
		if (basicInfo.Protect & PAGE_GUARD)
			goto ContinueLoop;
#endif

		dwReadSize = basicInfo.RegionSize;

		if (basicInfo.RegionSize > bufferSize && basicInfo.RegionSize > 16 * 1024 * 1024) // 16 MB
		{
			SCANNER_LOG(LL_SYS, "Huge section found! Size: %lld Read size: %ld", basicInfo.RegionSize, bufferSize);
			dwReadSize = bufferSize;
		}

		pCurrSectionCtx = std::make_shared<SSectionEnumContext>();
		if (IS_VALID_SMART_PTR(pCurrSectionCtx))
		{
			pCurrSectionCtx->AllocationBase = basicInfo.AllocationBase;
			pCurrSectionCtx->BaseAddress	= basicInfo.BaseAddress;
			pCurrSectionCtx->Protect		= basicInfo.Protect;
			pCurrSectionCtx->RegionSize		= dwReadSize;
			pCurrSectionCtx->State			= basicInfo.State;
			pCurrSectionCtx->BaseProtect	= basicInfo.AllocationProtect;
			pCurrSectionCtx->Type			= basicInfo.Type;

			vOutput.push_back(pCurrSectionCtx);
		}

//ContinueLoop:
		baseAddress += basicInfo.RegionSize;
	}

	return vOutput;
}

bool IScanner::EnumerateSections(HANDLE hProcess)
{
	SCANNER_LOG(LL_SYS, "Section enumerator has been started!");

	auto vSectionList = GetSectionList(hProcess);
	if (vSectionList.empty())
	{
		SCANNER_LOG(LL_ERR, "Section list is NULL!");
		return false;
	}

	for (const auto & pCurrSection : vSectionList)
	{
		if (IS_VALID_SMART_PTR(pCurrSection))
		{
			OnScanSection
			(
				hProcess, pCurrSection->BaseAddress, pCurrSection->AllocationBase, pCurrSection->RegionSize, 
				pCurrSection->State, pCurrSection->Protect, pCurrSection->BaseProtect, pCurrSection->Type
			);
		}
	}

	return true;
}

bool IScanner::ScanProcessSections(HANDLE hProcess)
{
	SCANNER_LOG(LL_SYS, "Section scanner has been started! Target process: %u(%p)", g_winapiApiTable->GetProcessId(hProcess), hProcess);

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

	// Check dig. sign informations, if signed pass it, it can be take so much time

	return EnumerateSections(hProcess);
}
