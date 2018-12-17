#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Hooks.h"
#include "Defines.h"
#include "StackTrace.h"
#include "WorkQueue.h"

void CSelfApiHooks::ThreadHookCallback(std::shared_ptr <SThreadHookQueueData> pData)
{
	if (!IS_VALID_SMART_PTR(pData))
		return;

	DEBUG_LOG(LL_SYS, "Thread callback works! TID: %u Start address: %p", pData->dwThreadID, pData->lpStartAddress);

	auto dwErrorType = 0UL;
	if (g_nmApp->AnalyserInstance()->OnThreadCreated(pData->dwThreadID, pData->lpStartAddress, pData->lpEIP, &dwErrorType) == false)
	{
		DEBUG_LOG(LL_ERR, "Illegal Thread initilization detected! Error code: %u", dwErrorType);
		g_nmApp->OnCloseRequest(EXIT_ERR_THREADLOAD_HOOK, g_winapiApiTable->GetLastError(), (void*)dwErrorType);
	}
}

void CSelfApiHooks::ModuleHookCallback(std::shared_ptr<SModuleHookQueueData> pData)
{
	if (!IS_VALID_SMART_PTR(pData))
		return;

	DEBUG_LOG(LL_SYS, "Module callback works! TID: %u Check type: %d Target: %ls", pData->dwThreadID, pData->iCheckType, pData->wszModuleName.c_str());

	// Process source string
	auto szSourceString = std::string(pData->wszModuleName.begin(), pData->wszModuleName.end());

	if (pData->iCheckType == CHECK_TYPE_RtlGetFullPathName_U)
	{
		auto lpModuleBase = g_nmApp->DynamicWinapiInstance()->GetModuleAddressFromName(pData->wszModuleName.c_str());
		if (lpModuleBase)
		{
			DEBUG_LOG(LL_ERR, "Illegal Module initilization detected! Error code: 1 Module: %s", szSourceString.c_str());
			g_nmApp->OnCloseRequest(EXIT_ERR_MODULELOAD_HOOK_1, g_winapiApiTable->GetLastError(), (void*)szSourceString.c_str());
			return;
		}
	}

	auto dwErrorType = 0UL;
	if (g_nmApp->AnalyserInstance()->OnModuleLoaded(szSourceString.c_str(), g_winapiApiTable->GetCurrentThreadId(), CHECK_TYPE_RtlGetFullPathName_U, &dwErrorType) == false)
	{
		DEBUG_LOG(LL_ERR, "Illegal Module initilization detected! Error code: 2-%u", dwErrorType);
		g_nmApp->OnCloseRequest(EXIT_ERR_MODULELOAD_HOOK_2, g_winapiApiTable->GetLastError(), (void*)dwErrorType);
	}
}

void CSelfApiHooks::ModuleRequestHookCallback(std::shared_ptr<SModuleReqHookQueueData> pData)
{
	if (!IS_VALID_SMART_PTR(pData))
		return;

	DEBUG_LOG(LL_SYS, "Module request callback works! Target: %ls", pData->wszModuleName.c_str());

	auto szModuleName = std::string(pData->wszModuleName.begin(), pData->wszModuleName.end());

	auto dwErrorType = 0UL;
	if (g_nmApp->AnalyserInstance()->OnModuleRequested(szModuleName.c_str(), &dwErrorType) == false)
	{
		DEBUG_LOG(LL_ERR, "Illegal module request detected! Error code: %u", dwErrorType);
		g_nmApp->OnCloseRequest(EXIT_ERR_MODULEHANDLE_HOOK, g_winapiApiTable->GetLastError(), (void*)dwErrorType);
	}
}

void CSelfApiHooks::SectionHookCallback(std::shared_ptr<SSectionHookQueueData> pData)
{
	if (!IS_VALID_SMART_PTR(pData))
		return;

	//if (g_nmApp->ManualMapIsReady() == false)
	//	return;

	// DEBUG_LOG(LL_SYS, "Section callback works! Base: %p pArbitraryUserPointer: %p", pData->dwBase, pData->lpArbitraryUserPointer);

	auto dwErrorType = 0UL;
	if (g_nmApp->AnalyserInstance()->OnSectionMapped(pData->dwBase, pData->lpArbitraryUserPointer, &dwErrorType) == false)
	{
		DEBUG_LOG(LL_ERR, "Illegal memory block detected! Error code: %u", dwErrorType);
		g_nmApp->OnCloseRequest(EXIT_ERR_MAPVIEWSECTION_HOOK, g_winapiApiTable->GetLastError(), (void*)dwErrorType);
	}
}

void CSelfApiHooks::WndProcHookCallback(std::shared_ptr<SWndProcHookQueueData> pData)
{
	if (!IS_VALID_SMART_PTR(pData))
		return;

	DEBUG_LOG(LL_SYS, "WndProc callback works! Current hWnd: %p Index: %d NewLong: %p", pData->hWnd, pData->nIndex, pData->dwNewLong);

	auto dwErrorType = 0UL;
	if (g_nmApp->AnalyserInstance()->OnWndProcHooked(pData->hWnd, pData->nIndex, pData->dwNewLong, &dwErrorType) == false)
	{
		DEBUG_LOG(LL_ERR, "Illegal window access detected! Error code: %d-%u", pData->iType, dwErrorType);
		g_nmApp->OnCloseRequest(EXIT_ERR_WNDPROC_HOOK, g_winapiApiTable->GetLastError(), (void*)dwErrorType);
	}
}

