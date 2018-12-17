#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ThreadExitCallback.h"
#include "Defines.h"

bool CThreadExitWatcher::InitializeExitCallback(HANDLE hThread, TThreadExitCallbackTemplate pCallback, DWORD dwTimeout, PVOID pContext)
{
	if (!hThread || !g_nmApp->DynamicWinapiInstance()->IsValidHandle(hThread))
		return false;

	auto hWaitObj = HANDLE(nullptr);
	auto bWaitRet = g_winapiApiTable->RegisterWaitForSingleObject(&hWaitObj, hThread, pCallback, pContext, dwTimeout, WT_EXECUTEONLYONCE);
	if (!bWaitRet)
		return false;

	m_hWaitObj	= hWaitObj;
	m_hThread	= hThread;
	m_pCallback = pCallback;
	return true;
}


