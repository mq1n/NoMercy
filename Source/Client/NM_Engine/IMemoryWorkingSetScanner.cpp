#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "Defines.h"
#include "WinVerHelper.h"

inline LPVOID CreateMemoryPage(DWORD dwRegionSize, DWORD dwProtection)
{
	LPVOID pMemBase = nullptr;

	__try 
	{
		pMemBase = g_winapiApiTable->VirtualAlloc(0, dwRegionSize, MEM_COMMIT | MEM_RESERVE, dwProtection);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { }

	return pMemBase;
}

auto IScanner::GetProtectedMemoryRegions()
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	return m_pMemoryWatchdogs;
};

bool IScanner::InitializeMemoryWatchdogs()
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	DEBUG_LOG(LL_SYS, "Memory watchdog create routine has been started!");
	m_pMemoryWatchdogs.clear();

	auto iRandomNumber = g_nmApp->FunctionsInstance()->GetRandomInt(1, 15);
	for (auto i = 0UL; i <= iRandomNumber; i++) // Create fake pages pre
	{
		auto pCurrMem = CreateMemoryPage(0x10000, PAGE_READWRITE);
		if (pCurrMem) m_pMemoryDummyPages.push_back(pCurrMem);
	}

	auto pFirstWatchdog = CreateMemoryPage(0x10000, PAGE_READWRITE); // Create watchdog page
	if (!pFirstWatchdog)
	{
		DEBUG_LOG(LL_ERR, "First watchdog create fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	m_pMemoryWatchdogs.push_back(pFirstWatchdog);
	DEBUG_LOG(LL_SYS, "First watchdog created at: %p", pFirstWatchdog);

	iRandomNumber = g_nmApp->FunctionsInstance()->GetRandomInt(1, 15);
	for (auto i = 0UL; i <= iRandomNumber; i++) // Create fake pages post
	{
		auto pCurrMem = CreateMemoryPage(0x10000, PAGE_READWRITE);
		if (pCurrMem) m_pMemoryDummyPages.push_back(pCurrMem);
	}


	iRandomNumber = g_nmApp->FunctionsInstance()->GetRandomInt(1, 15);
	for (auto i = 0UL; i <= iRandomNumber; i++) // Create fake pages pre
	{
		auto pCurrMem = CreateMemoryPage(0x10000, PAGE_READONLY);
		if (pCurrMem) m_pMemoryDummyPages.push_back(pCurrMem);
	}

	auto pSecondWatchdog = CreateMemoryPage(0x10000, PAGE_READONLY); // Create watchdog page
	if (!pSecondWatchdog)
	{
		DEBUG_LOG(LL_ERR, "Second watchdog create fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	m_pMemoryWatchdogs.push_back(pSecondWatchdog);
	DEBUG_LOG(LL_SYS, "Second watchdog created at: %p", pSecondWatchdog);

	iRandomNumber = g_nmApp->FunctionsInstance()->GetRandomInt(1, 15);
	for (auto i = 0UL; i <= iRandomNumber; i++) // Create fake pages post
	{
		auto pCurrMem = CreateMemoryPage(0x10000, PAGE_READONLY);
		if (pCurrMem) m_pMemoryDummyPages.push_back(pCurrMem);
	}


	iRandomNumber = g_nmApp->FunctionsInstance()->GetRandomInt(1, 15);
	for (auto i = 0UL; i <= iRandomNumber; i++) // Create fake pages pre
	{
		auto pCurrMem = CreateMemoryPage(0x10000, PAGE_NOACCESS); 
		if (pCurrMem) m_pMemoryDummyPages.push_back(pCurrMem);
	}

	auto pThirdWatchdog = CreateMemoryPage(0x10000, PAGE_NOACCESS); // Create watchdog page
	if (!pThirdWatchdog)
	{
		DEBUG_LOG(LL_ERR, "Third watchdog create fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}
	m_pMemoryWatchdogs.push_back(pThirdWatchdog);
	DEBUG_LOG(LL_SYS, "Third watchdog created at: %p", pThirdWatchdog);

	iRandomNumber = g_nmApp->FunctionsInstance()->GetRandomInt(1, 15);
	for (auto i = 0UL; i <= iRandomNumber; i++) // Create fake pages post
	{
		auto pCurrMem = CreateMemoryPage(0x10000, PAGE_NOACCESS);
		if (pCurrMem) m_pMemoryDummyPages.push_back(pCurrMem);
	}

	DEBUG_LOG(LL_SYS, "Memory watchdog pages succesfully created! Watchdog count: %u Dummy page count: %u", m_pMemoryWatchdogs.size(), m_pMemoryDummyPages.size());
	return true;
}

bool IScanner::CheckMemoryWatchdogs()
{
	std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

	DEBUG_LOG(LL_SYS, "Memory watchdog check routine started!");

	if (m_pMemoryWatchdogs.empty())
	{
		DEBUG_LOG(LL_ERR, "Any watchdog page is NOT exist yet");
		return false;
	}

	for (const auto & pCurrWatchdog : m_pMemoryWatchdogs)
	{
		DEBUG_LOG(LL_SYS, "Current watchdog page: %p", pCurrWatchdog);

		if (IsBadCodePtr((FARPROC)pCurrWatchdog))
		{
			DEBUG_LOG(LL_ERR, "Memory page is corrupted!");
			return false;
		}

		MEMORY_BASIC_INFORMATION mbi = { 0 };
		if (!g_winapiApiTable->VirtualQuery(pCurrWatchdog, &mbi, sizeof(mbi)))
		{
			DEBUG_LOG(LL_ERR, "VirtualQuery fail! Error: %u", g_winapiApiTable->GetLastError());
			return false;
		}

		if (mbi.State == MEM_FREE)
		{
			DEBUG_LOG(LL_ERR, "Memory page is free'd!");
			return false;
		}

		PSAPI_WORKING_SET_EX_INFORMATION pworkingSetExInformation = { pCurrWatchdog, NULL };

		auto ntStatus = g_winapiApiTable->NtQueryVirtualMemory(NtCurrentProcess, NULL, MemoryWorkingSetExInformation, &pworkingSetExInformation, sizeof(pworkingSetExInformation), NULL);
		if (!NT_SUCCESS(ntStatus))
		{
			DEBUG_LOG(LL_ERR, "NtQueryVirtualMemory fail! Status: %p", ntStatus);
			if (IsWindowsVistaOrGreater() && !g_winapiApiTable->QueryWorkingSetEx(NtCurrentProcess, &pworkingSetExInformation, sizeof(pworkingSetExInformation)))
			{
				DEBUG_LOG(LL_ERR, "QueryWorkingSetEx fail! Error: %u", g_winapiApiTable->GetLastError());
				return false;
			}
		}

		if (pworkingSetExInformation.VirtualAttributes.Valid)
		{
			DEBUG_LOG(LL_ERR, "Memory scan detected at watchdog: %p", pCurrWatchdog);
			return false;
		}
	}
	return true;
}

