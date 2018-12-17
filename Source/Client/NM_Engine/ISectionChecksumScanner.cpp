#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ScannerInterface.h"
#include "Defines.h"
#include "PEHelper.h"
#include "ProcessFunctions.h"
#include "ScannerInterface.h"


inline bool GetSectionHash(DWORD dwModuleBase, const std::string & szSectionName, LPDWORD pdwSectionHash)
{
	DEBUG_LOG(LL_SYS, "Section hash calculate function has been started! Target module base: %p Target session: %s", dwModuleBase, szSectionName.c_str());

	auto dwSectionBase = 0UL, dwSectionSize = 0UL;
	bool bFindTextSection = CPEFunctions::GetSectionInformation(szSectionName, dwModuleBase, &dwSectionBase, &dwSectionSize);
	if (!bFindTextSection)
	{
		DEBUG_LOG(LL_ERR, "Section: %s can not found for: %p", szSectionName.c_str(), dwModuleBase);
		return false;
	}

	auto szModuleOwner = g_nmApp->FunctionsInstance()->GetModuleOwnerName(reinterpret_cast<LPVOID>(dwModuleBase));
	if (szModuleOwner.empty()) 
	{
		DEBUG_LOG(LL_ERR, "Module: %p informations not found! Last error: %u", dwModuleBase, g_winapiApiTable->GetLastError());
		return false;
	}

	if (!dwSectionBase || !dwSectionSize) 
	{
		DEBUG_LOG(LL_ERR, "Section base address or size is null! Target Module base: %p Section: %s Module: %s IsPacked result: %d",
			dwModuleBase, szSectionName.c_str(), szModuleOwner.c_str(), g_nmApp->DirFunctionsInstance()->IsPackedExecutable(szModuleOwner));	
		return false;
	}

	if (IsBadReadPtr((LPCVOID)dwSectionBase, dwSectionSize))
	{
		DEBUG_LOG(LL_ERR, "Section is not at readable area! Base: %p Size: %p", dwSectionBase, dwSectionSize);
		return false;
	}

	MEMORY_BASIC_INFORMATION mbi;
	if (!g_winapiApiTable->VirtualQuery((LPCVOID)dwSectionBase, &mbi, sizeof(mbi)))
	{
		DEBUG_LOG(LL_ERR, "VirtualQuery fail! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	if (!(mbi.State & MEM_COMMIT) || (mbi.State & MEM_RELEASE) || (mbi.Protect == PAGE_NOACCESS) || (mbi.Protect & PAGE_GUARD))
	{
		DEBUG_LOG(LL_ERR, "Section is not a accesible memory!");
		return false;
	}

	auto dwOldProtect = 0UL;
	if (!g_winapiApiTable->VirtualProtect((LPVOID)dwSectionBase, dwSectionSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		DEBUG_LOG(LL_ERR, "VirtualProtect(pre) fail! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	auto dwChecksum = CPEFunctions::CalculateChecksum(dwSectionBase, dwSectionSize);
	if (!dwChecksum)
	{
		DEBUG_LOG(LL_ERR, "Section checksum can not calculated! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	if (!g_winapiApiTable->VirtualProtect((LPVOID)dwSectionBase, dwSectionSize, dwOldProtect, &dwOldProtect))
	{
		DEBUG_LOG(LL_ERR, "VirtualProtect(post) fail! Last error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Section: %s checksum calculated: %p - %p -> (%p)", szSectionName.c_str(), dwSectionBase, dwSectionSize, dwChecksum);
	if (pdwSectionHash) *pdwSectionHash = dwChecksum;
	return true;
}

bool IScanner::InitSectionHash(DWORD dwBase)
{
	DEBUG_LOG(LL_SYS, "Section checksum create init started! Target: %p", dwBase);

	auto	bCompleted		= false;
	auto	iAttemptCount	= 0;
	auto	dwTextHash		= 0UL;
	auto	dwRDataHash		= 0UL;
	do
	{
		// TODO: PE Header, .edata, .rsrc, .reloc

		if (!GetSectionHash(dwBase, xorstr(".text").crypt_get(), &dwTextHash))
		{
			DEBUG_LOG(LL_ERR, "GetSectionHash(.text) fail!");
			break;
		}

		if (!GetSectionHash(dwBase, xorstr(".rdata").crypt_get(), &dwRDataHash))
		{
			DEBUG_LOG(LL_ERR, "GetSectionHash(.rdata) fail!");
			break;
		}

		if (dwTextHash && dwRDataHash) 
		{
			DEBUG_LOG(LL_SYS, "Section checksum calculate completed! result: %p|%p", dwTextHash, dwRDataHash);

			bCompleted = true;
			break;
		}

		DEBUG_LOG(LL_ERR, "Section checksum can not calculated! result: %p|%p", dwTextHash, dwRDataHash);

		auto szModuleOwner = g_nmApp->FunctionsInstance()->GetModuleOwnerName(reinterpret_cast<LPVOID>(dwBase));
		if (g_nmApp->DirFunctionsInstance()->IsPackedExecutable(szModuleOwner))
		{
			DEBUG_LOG(LL_ERR, "Section checksum fail! Executable: %s is probably packed!", szModuleOwner.c_str());
			return false;
		}
		else
		{
			Sleep(2000);
			iAttemptCount++;
		}
	} while (iAttemptCount < 3);

	auto pSectionChecksumInfos = std::make_shared<SSectionChecksumInfos>();
	if (IS_VALID_SMART_PTR(pSectionChecksumInfos))
	{
		pSectionChecksumInfos->dwBase = dwBase;
		pSectionChecksumInfos->dwTextHash = dwTextHash;
		pSectionChecksumInfos->dwRDataHash = dwRDataHash;
		pSectionChecksumInfos->bStatus = bCompleted;
		m_mSectionChecksumInfoMap[dwBase] = pSectionChecksumInfos;
	}
	return bCompleted;
}

bool IScanner::InitSectionHashes()
{
	DEBUG_LOG(LL_SYS, "Section checksum check init started!");

	// Clear old hash list
	m_mSectionChecksumInfoMap.clear();

	/*
	auto pEnumerator = std::make_unique<CEnumerator>(g_winapiApiTable->GetCurrentProcessId());
	if (!pEnumerator || !pEnumerator.get())
		return;

	auto vModuleList = pEnumerator->EnumerateModules();
	if (vModuleList.empty())
		return;

	for (const auto & hModule : vModuleList)
	{
		if (!InitSectionHash((DWORD)hModule, true))
		{
			DEBUG_LOG(LL_ERR, "Section hash checker can not created! Target module base: %p Name: %s", hModule, GetModuleOwnerName(hModule).c_str());
			// close process
		}
	}
	*/

	InitSectionHash((DWORD)g_winapiModuleTable->hBaseModule);
	return true;
}
bool IScanner::CheckSectionHashes()
{
	DEBUG_LOG(LL_SYS, "Section checksum check started!");

	/*
	auto vModuleList=createmodulelist();
	for (const auto & hModule : vModuleList)
	{
		DEBUG_LOG(LL_SYS, "Module: %p section checksum check started!", hModule);

		// get old informations
		auto pOldSectionInformations = m_sectionChecksumInfoMap[hModule];
		if (!pOldSectionInformations)
		{

		}

		// clear old informations
		m_sectionChecksumInfoMap.erase(pOldSectionInformations);

		// create current hashes
		auto bHashResult = InitSectionHash(hModule);
		if (!bHashResult)
		{

		}

		// get new informations
		auto pCurrentSectionInformations = m_sectionChecksumInfoMap[hModule];
		if (!pCurrentSectionInformations)
		{

		}

		// check
		if (pOldSectionInformations != pCurrentSectionInformations)
		{

		}
	}
	*/
	return true;
}

DWORD WINAPI CheckSectionsHashRoutine(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Section hash check thread event has been started");

	auto pSectionCheckThreadTimer = CTimer<std::chrono::milliseconds>();

	auto currentThread = std::shared_ptr<CThread>();
	do
	{
		if (pSectionCheckThreadTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_SECTION_HASH_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_SECTIONCHECKSUM);

	} while (!IS_VALID_SMART_PTR(currentThread));

	// ---

	g_nmApp->ScannerInstance()->InitSectionHashes();

	while (1)
	{
		g_nmApp->ScannerInstance()->CheckSectionHashes();

		// Update ticks
		g_nmApp->SelfThreadMgrInstance()->IncreaseThreadTick(SELF_THREAD_SECTIONCHECKSUM);
		g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(SELF_THREAD_SECTIONCHECKSUM, GetCurrentTime());

		g_winapiApiTable->Sleep(6000);
	}

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc8()
{
	__asm
	{
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
		_emit 0xAA
	}
}
#endif

bool IScanner::InitChecksumScan()
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)CheckSectionsHashRoutine);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		DEBUG_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_SECTIONCHECKSUM, CheckSectionsHashRoutine, ulFuncSize, nullptr, 6000 + 1000, false);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}
