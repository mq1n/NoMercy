#pragma once
#include "PatternTemplate.h"
#include "MemoryHelper.h"

enum EScanFlags
{
	MEM_SCAN_NORMAL		= (1 << 0), // normal
	MEM_SCAN_READ		= (1 << 1), // rpm at pattern
	MEM_SCAN_SUBTRACT	= (1 << 2), // subtract img base
	MEM_SCAN_WITHEXTRA	= (1 << 3), // add extra data
};

class CMemoryScanner
{
	public:
		CMemoryScanner(HANDLE hProcess, DWORD dwFlags);
		CMemoryScanner(HANDLE hProcess, DWORD dwFlags, DWORD dwLowAddr, DWORD dwHighAddr);
		~CMemoryScanner();

		void	SetSafeSearch(bool bSafe);
		void	SetExtraFlag(bool bExtra, DWORD dwExtraCount = 0);
		void	SetFlags(DWORD dwFlags);

		DWORD	Scan(const CMemoryPattern& memPattern) const;

	protected:
		DWORD	ProcessPatternScan(PBYTE pCurAddr, std::size_t uiSize, const CMemoryPattern& memPattern) const;
		int		ProcessSectionScan(PBYTE pCurAddr, const CMemoryPattern& memPattern, MEMORY_BASIC_INFORMATION mbi, LPDWORD pdwResult) const;

	private:
		std::unique_ptr <CMemoryHelper>	m_cMemHelper;
		HANDLE							m_hProcess;

		DWORD							m_dwLowAddr;
		DWORD							m_dwHighAddr;

		bool							m_bSafeSearch;
		bool							m_bExtra;

		DWORD							m_dwFlags;
		DWORD							m_dwExtraCount;
};

