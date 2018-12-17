#include "NM_Index.h"
#include "NM_Main.h"
#include "Defines.h"
#include "PatternScan.h"
#include "MemoryHelper.h"

// TODO: Multiple scan like DWORD CMemoryScanner::Scan(std::vector < CMemoryPattern* > memPatterns)

CMemoryScanner::CMemoryScanner(HANDLE hProcess, DWORD dwFlags) :
	m_hProcess(hProcess), m_dwFlags(dwFlags), m_dwLowAddr(0), m_dwHighAddr(0), m_cMemHelper(nullptr), m_bSafeSearch(false), m_bExtra(false),
	m_dwExtraCount(0)
{
	m_cMemHelper = std::make_unique<CMemoryHelper>(m_hProcess);
}

CMemoryScanner::CMemoryScanner(HANDLE hProcess, DWORD dwFlags, DWORD dwLowAddr, DWORD dwHighAddr) :
	m_hProcess(hProcess), m_dwFlags(dwFlags), m_dwLowAddr(dwLowAddr), m_dwHighAddr(dwHighAddr), m_cMemHelper(nullptr), m_bSafeSearch(false),
	m_bExtra(false), m_dwExtraCount(0)
{
	m_cMemHelper = std::make_unique<CMemoryHelper>(m_hProcess);
}

CMemoryScanner::~CMemoryScanner()
{
	m_hProcess = INVALID_HANDLE_VALUE;
	m_dwFlags = 0;
	m_dwLowAddr = 0;
	m_dwHighAddr = 0;
	m_bSafeSearch = false;
	m_bExtra = false;
	m_dwExtraCount = 0;
}


void CMemoryScanner::SetSafeSearch(bool bSafe)
{
	m_bSafeSearch = bSafe;
}

void CMemoryScanner::SetExtraFlag(bool bExtra, DWORD dwExtraCount)
{
	m_bExtra = bExtra;
	m_dwExtraCount = dwExtraCount;
}

void CMemoryScanner::SetFlags(DWORD dwFlags)
{
	m_dwFlags = dwFlags;
}

DWORD CMemoryScanner::ProcessPatternScan(PBYTE pCurAddr, std::size_t uiSize, const CMemoryPattern& memPattern) const
{
	auto vBuffer = std::vector <BYTE> (uiSize);

	if (m_cMemHelper->ReadAddressSafe(pCurAddr, &vBuffer[0], uiSize) == false) 
	{
		DEBUG_LOG(LL_ERR, "Read fail ||| %p - %u(real) - %u(read) | %p", pCurAddr, uiSize, vBuffer.size(), memPattern.GetBytes());
		return 0UL;
	}

	DWORD dwResult = 0;
	for (DWORD i = 0UL; i + memPattern.GetLength() < vBuffer.size(); ++i)
	{
		bool bFound = true;
		for (DWORD j = 0UL; j < memPattern.GetLength() && bFound; ++j)
		{
			if (memPattern.GetMask(j) == '?') continue;
			if (memPattern.GetByte(j) == vBuffer[i + j]) continue;
			bFound = false;
		}

		if (bFound) 
		{
			dwResult = (DWORD)pCurAddr + i + memPattern.GetPatternOffset();
			
			// printf("w/o extra: %p\n", dwResult);
			if (m_dwFlags & EScanFlags::MEM_SCAN_WITHEXTRA)
				dwResult += m_dwExtraCount;

			// printf("w/o read: %p\n", dwResult);
			if (m_dwFlags & EScanFlags::MEM_SCAN_READ)
				m_cMemHelper->ReadAddress((LPVOID)dwResult, &dwResult, sizeof(dwResult));

			// printf("w/o subtract: %p\n", dwResult);
			if (m_dwFlags & EScanFlags::MEM_SCAN_SUBTRACT)
				dwResult -= m_dwLowAddr;

			// printf("w/o adjust offset: %p\n", dwResult);
			if (memPattern.GetAddrOffset())
				dwResult += memPattern.GetAddrOffset();

			break;
		}
	}

	//vBuffer.clear();
	return dwResult;
}

int CMemoryScanner::ProcessSectionScan(PBYTE pCurAddr, const CMemoryPattern& memPattern, MEMORY_BASIC_INFORMATION mbi, LPDWORD pdwResult) const
{
	// Check module range
	if (m_dwLowAddr && m_dwHighAddr)
		if ( (DWORD)pCurAddr < m_dwLowAddr || (DWORD)pCurAddr > m_dwHighAddr )
			return 0;

	// Check page states
	if (!(mbi.State & MEM_COMMIT))
		return -1;

	if (mbi.State & MEM_RELEASE)
		return -2;

	// Access check
	if (mbi.Protect == PAGE_NOACCESS || mbi.Protect & PAGE_GUARD)
		return -3;

	if (mbi.Protect != PAGE_READONLY && mbi.Protect != PAGE_READWRITE && mbi.Protect != PAGE_EXECUTE_READ && mbi.Protect != PAGE_EXECUTE_READWRITE)
		return -4;

	if (m_bSafeSearch)
	{
		// Only valid sections
		PSAPI_WORKING_SET_EX_INFORMATION pworkingSetExInformation = { 0 };
		if (!g_winapiApiTable->QueryWorkingSetEx(m_hProcess, &pworkingSetExInformation, sizeof(pworkingSetExInformation)))
			return -5;

		if (!pworkingSetExInformation.VirtualAttributes.Valid)
			return -6;
	}

	// Scan patterns
	DWORD dwLocalRet = ProcessPatternScan(pCurAddr, mbi.RegionSize, memPattern);
	if (!dwLocalRet)
		return -7;

	*pdwResult = dwLocalRet;
	return 1;
}

DWORD CMemoryScanner::Scan(const CMemoryPattern& memPattern) const
{
	SYSTEM_INFO sysINFO;
	g_winapiApiTable->GetSystemInfo(&sysINFO);

	PBYTE pCurAddr = (PBYTE)sysINFO.lpMinimumApplicationAddress;
	PBYTE pMaxAddr = (PBYTE)sysINFO.lpMaximumApplicationAddress;

	DWORD dwResult = 0;
	MEMORY_BASIC_INFORMATION mbi;
	while (pCurAddr < pMaxAddr)
	{
		if (g_winapiApiTable->VirtualQueryEx(m_hProcess, pCurAddr, &mbi, sizeof(mbi)))
			if (ProcessSectionScan(pCurAddr, memPattern, mbi, &dwResult) == 1)
				break;

		pCurAddr += mbi.RegionSize;
	}
	return dwResult;
}

