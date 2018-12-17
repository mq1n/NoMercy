#include "NM_Index.h"
#include "NM_Main.h"
#include "AntiBreakpoint.h"
#include "Defines.h"
#include "PEHelper.h"

typedef struct _SOFT_BP_CHECK_INFORMATIONS
{
	PVOID pBase;
	DWORD dwSize;
	DWORD dwChecksum;
}SSoftBPCheckInfos, *PSoftBPCheckInfos;

static auto s_vCheckList = std::vector < std::shared_ptr <SSoftBPCheckInfos> > ();

bool IsAddedRegion(PVOID lpSectionBase, std::size_t ulSize)
{
	for (const auto & pCurrRegion : s_vCheckList)
	{
		if (pCurrRegion->pBase == lpSectionBase && pCurrRegion->dwSize == ulSize)
			return true;
	}
	return false;
}

void AddToCheckList(PVOID lpSectionBase, std::size_t ulSize)
{
	auto dwChecksum = CPEFunctions::CalculateChecksum((DWORD)lpSectionBase, ulSize);
	if (!dwChecksum)
		return;

	if (IsAddedRegion(lpSectionBase, ulSize))
		return;

	DEBUG_LOG(LL_SYS, "Section added to check list! Base: %p Size: %u Sum: %p", lpSectionBase, ulSize, dwChecksum);

	auto softBPInfos		= std::make_shared<SSoftBPCheckInfos>();
	softBPInfos->pBase		= lpSectionBase;
	softBPInfos->dwSize		= ulSize;
	softBPInfos->dwChecksum = dwChecksum;

	s_vCheckList.push_back(softBPInfos);
}

bool ScanModuleCodecaveSuitableSection(DWORD dwModuleBase, const std::wstring & wszBaseName)
{
	DEBUG_LOG(LL_SYS, "Routine started for: %p -> %ls", dwModuleBase, wszBaseName.c_str());

	auto PIDH = (PIMAGE_DOS_HEADER)dwModuleBase;
	if (!PIDH || PIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	auto pINH = (PIMAGE_NT_HEADERS)((DWORD)PIDH + PIDH->e_lfanew);
	if (!pINH || pINH->Signature != IMAGE_NT_SIGNATURE)
		return false;

	auto pIFH = (PIMAGE_FILE_HEADER)&pINH->FileHeader;
	if (!pIFH)
		return false;

	auto pImageSectionHeader = IMAGE_FIRST_SECTION(pINH);
	if (!pImageSectionHeader)
		return false;

	auto nSectionNumber = pIFH->NumberOfSections;
	DEBUG_LOG(LL_SYS, "%u section found!", nSectionNumber);

	for (std::size_t i = 0; i < nSectionNumber; ++i)
	{
		auto pCurrSection = pImageSectionHeader[i];
		DEBUG_LOG(LL_SYS, "Current section: %s Base: %p Size: %u",
			(char*)pCurrSection.Name, dwModuleBase + pCurrSection.VirtualAddress, pCurrSection.SizeOfRawData);

		auto IsMonitored =
			(pCurrSection.Characteristics & IMAGE_SCN_MEM_EXECUTE) && (pCurrSection.Characteristics & IMAGE_SCN_MEM_READ) &&
			(pCurrSection.Characteristics & IMAGE_SCN_CNT_CODE) && !(pCurrSection.Characteristics & IMAGE_SCN_MEM_DISCARDABLE);

		if (IsMonitored)
		{
			DEBUG_LOG(LL_SYS, "Section: %s suitable for check!", (char*)pCurrSection.Name);
			AddToCheckList((VOID *)((ULONG_PTR)PIDH + pCurrSection.VirtualAddress), pCurrSection.Misc.VirtualSize);
		}
	}
	return true;
}

DWORD WINAPI SoftBPCheckRoutine(LPVOID)
{
	DEBUG_LOG(LL_SYS, "Software BP check event has been started");

	auto pFolderThreadTimer = CTimer<std::chrono::milliseconds>();

	auto currentThread = std::shared_ptr<CThread>();
	do
	{
		if (pFolderThreadTimer.diff() > 5000)
			g_nmApp->OnCloseRequest(EXIT_ERR_SOFTBP_THREAD_TIMEOUT, g_winapiApiTable->GetLastError());

		g_winapiApiTable->Sleep(10);
		currentThread = g_nmApp->ThreadManagerInstance()->GetThreadFromThreadCode(SELF_THREAD_ANTI_SOFTBP);

	} while (!IS_VALID_SMART_PTR(currentThread));

	while (1)
	{	
		// Update ticks
		g_nmApp->SelfThreadMgrInstance()->IncreaseThreadTick(SELF_THREAD_ANTI_SOFTBP);
		g_nmApp->SelfThreadMgrInstance()->SetLastCheckTime(SELF_THREAD_ANTI_SOFTBP, GetCurrentTime());

		Sleep(10000);
	}

#if 0
	auto pEnumerator = std::make_unique<CEnumerator>(g_winapiApiTable->GetCurrentProcessId());
	if (!pEnumerator || !pEnumerator.get())
		return 0;

	// clear last list
	s_vCheckList.clear();

	// create new scan list
	auto vModuleList = pEnumerator->EnumerateModules();
	if (vModuleList.empty() == false)
	{
		for (const auto & pModule : vModuleList)
		{
			ScanModuleCodecaveSuitableSection(pModule->dwDllBase, pModule->wszBaseDllName);
		}
	}

	while (1)
	{
		// check listed objects
		if (s_vCheckList.empty() == false)
		{
			for (const auto & pCurrInfo : s_vCheckList)
			{
				auto dwCurrChecksum = CPEFunctions::CalculateChecksum((DWORD)pCurrInfo->pBase, pCurrInfo->dwSize);
				DEBUG_LOG(pCurrInfo->dwChecksum == dwCurrChecksum ? LL_SYS : LL_CRI, "Current base: %p Size: %u Current sum: %p Corrent sum: %p Eq: %d",
					pCurrInfo->pBase, pCurrInfo->dwSize, dwCurrChecksum, pCurrInfo->dwChecksum, pCurrInfo->dwChecksum == dwCurrChecksum);

				if (pCurrInfo->dwChecksum != dwCurrChecksum)
				{
					auto szOwner = g_nmApp->FunctionsInstance()->GetModuleOwnerName(pCurrInfo->pBase);
					//if (!strstr(szOwner.c_str(), xorstr("ntdll.dll"))) // whitelist
					{
						auto szLog = g_nmApp->FunctionsInstance()->CreateString(xorstr("Corrupted memory data detected! Owner: %s"), szOwner.c_str());
						g_nmApp->FunctionsInstance()->CloseProcess(szLog.c_str(), false, "");
					}
				}
			}
		}

		g_winapiApiTable->Sleep(10000);
	}
#endif

	g_nmApp->ThreadManagerInstance()->DestroyThread(currentThread);
	return 0;
}

#ifndef _M_X64
__declspec(naked) void DummyFunc3()
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

bool CAntiBreakpoint::InitAntiSoftBP()
{
	DEBUG_LOG(LL_SYS, "Thread creation has been started!");

	auto ulFuncSize = g_nmApp->ThreadManagerInstance()->GetThreadFuncSize((LPBYTE)SoftBPCheckRoutine);
	if (!ulFuncSize || ulFuncSize > 1024)
	{
		DEBUG_LOG(LL_ERR, "Not valid thread func size: %u", ulFuncSize);
		return false;
	}

	auto thread = g_nmApp->ThreadManagerInstance()->CreateCustomThread(SELF_THREAD_ANTI_SOFTBP, SoftBPCheckRoutine, ulFuncSize, nullptr, 10000 + 5000, false);
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_ERR, "Thread can NOT created! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Info - %u[%p->%p][%d-%s] - Completed! Shared:%p Custom:%p",
		thread->GetId(), thread->GetHandle(), thread->GetStartAddress(), thread->GetCustomCode(), thread->GetThreadCustomName().c_str(), thread, thread.get());

	return true;
}

