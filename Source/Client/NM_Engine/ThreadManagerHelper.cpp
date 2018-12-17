#include "NM_Index.h"
#include "NM_Main.h"
#include "ThreadManagerHelper.h"
#include "ThreadHelper.h"
#include "ThreadFunctions.h"
#include "Defines.h"
#include "PEHelper.h"
#include "ThreadExitCallback.h"
#include "WinVerHelper.h"


static void NTAPI ThreadTerminateWatcher(PVOID pCtx, BOOLEAN)
{
	g_nmApp->ThreadManagerInstance()->OnThreadTerminated(reinterpret_cast<DWORD>(pCtx));
}


CThreadManager::CThreadManager()
{
	m_vThreadList.clear();
	m_vSelfThreads.clear();
}

CThreadManager::~CThreadManager()
{
	m_vThreadList.clear();
}

std::shared_ptr <CThread> CThreadManager::CreateCustomThread(int iThreadCode, LPTHREAD_START_ROUTINE pFunc, std::size_t ulFuncSize, LPVOID lpParam, DWORD dwMaxDelay, bool bIsTemporaryThread /* TODO: LPDWORD pdwErrorStep, bool bCreateSuspended = false */)
{
	DEBUG_LOG(LL_SYS, "Custom thread(%u) create has been started!", iThreadCode);

	if (GetThreadFromThreadCode(iThreadCode))
	{
		DEBUG_LOG(LL_CRI, "Already exist thread with code(%d)!", iThreadCode);

		g_nmApp->OnCloseRequest(EXIT_ERR_CUSTOM_THREAD_ALREADY_EXIST, g_winapiApiTable->GetLastError());
		return nullptr;
	}

	auto dwThreadId = 0UL;
	auto hThread = CThreadFunctions::CreateThread(iThreadCode, pFunc, lpParam, &dwThreadId);
	if (!IS_VALID_HANDLE(hThread))
	{
		DEBUG_LOG(LL_CRI, "Thread: %d can NOT created! Error: %u", iThreadCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_CUSTOM_THREAD_CREATE_FAIL, g_winapiApiTable->GetLastError());
		return nullptr;
	}

	if (g_nmApp->AccessHelperInstance()->BlockAccess(hThread) == false)
	{
		DEBUG_LOG(LL_CRI, "Thread access rules(1) can NOT adjusted for: %d Error: %u", iThreadCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_CUSTOM_THREAD_ACCESS_1_FAIL, g_winapiApiTable->GetLastError());
		return nullptr;
	}

	if (g_nmApp->AccessHelperInstance()->DenyThreadAccess(hThread) == false)
	{
		DEBUG_LOG(LL_CRI, "Thread access rules(2) can NOT adjusted for: %d Error: %u", iThreadCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_CUSTOM_THREAD_ACCESS_2_FAIL, g_winapiApiTable->GetLastError());
		return nullptr;
	}

	auto customThread = std::make_shared<CThread>(hThread, dwThreadId, iThreadCode);
	if (!IS_VALID_SMART_PTR(customThread))
	{
		DEBUG_LOG(LL_CRI, "Thread class can NOT created! Thread: %d Error: %u", iThreadCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_CUSTOM_THREAD_ALLOC_CLASS_FAIL, g_winapiApiTable->GetLastError());
		return nullptr;
	}

	auto threadInfos = std::make_shared<SSelfThreads>();
	if (!IS_VALID_SMART_PTR(threadInfos))
	{
		DEBUG_LOG(LL_CRI, "Thread data container can NOT created! Thread: %d Error: %u", iThreadCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_CUSTOM_THREAD_ALLOC_CONTAINER_FAIL, g_winapiApiTable->GetLastError());
		return nullptr;
	}

	auto pExitCallbackHelper = std::make_shared<CThreadExitWatcher>();
	if (!IS_VALID_SMART_PTR(pExitCallbackHelper))
	{
		DEBUG_LOG(LL_CRI, "Thread exit watcher can NOT created! Thread: %d Error: %u", iThreadCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_CUSTOM_THREAD_EXIT_WATCHER_ALLOC_FAIL, g_winapiApiTable->GetLastError());
		return nullptr;
	}

	if (!pExitCallbackHelper->InitializeExitCallback(hThread, ThreadTerminateWatcher, INFINITE, (PVOID)dwThreadId))
	{
		DEBUG_LOG(LL_CRI, "Thread exit watcher can NOT initialize! Thread: %d Error: %u", iThreadCode, g_winapiApiTable->GetLastError());

		g_nmApp->OnCloseRequest(EXIT_ERR_CUSTOM_THREAD_EXIT_WATCHER_INIT_FAIL, g_winapiApiTable->GetLastError());
		return nullptr;
	}

	threadInfos->iThreadCode		= iThreadCode;
	threadInfos->dwThreadAddress	= reinterpret_cast<DWORD_PTR>(pFunc);
	threadInfos->hThread			= hThread;
	threadInfos->customThread		= customThread;
	threadInfos->dwThreadId			= dwThreadId;
	threadInfos->dwMaxDelay			= dwMaxDelay;
	threadInfos->bIsTemporaryThread = bIsTemporaryThread;
	threadInfos->ulFuncSize			= ulFuncSize;
	threadInfos->ulFuncHash			= CPEFunctions::CalculateChecksum((DWORD)pFunc, ulFuncSize);

	m_vThreadList.push_back(threadInfos);

#ifdef _DEBUG
	if (IsWindows10OrGreater() && GetWindowsBuildNumber() >= 14393)
	{
//		auto szThreadName = customThread->GetThreadCustomName();
//		auto wszThreadName = std::wstring(szThreadName.begin(), szThreadName.end());

		std::wstring wszThreadName = L"Custom thread: " + std::to_wstring(iThreadCode);

//		LI_FIND(SetThreadDescription)(hThread, wszThreadName.c_str());
	}
#endif

	DEBUG_LOG(LL_SYS, "Custom thread created! %d) %p (%u) Adr: %p Len: %u Sum: %p", iThreadCode, hThread, dwThreadId, pFunc, ulFuncSize, threadInfos->ulFuncHash);
	return customThread;
}

bool CThreadManager::DestroyThread(const std::shared_ptr <CThread> & thread)
{
	if (!IS_VALID_SMART_PTR(thread) || thread->IsCorrect() == false)
	{
		DEBUG_LOG(LL_CRI, "Unknown thread! Tid: %u", IS_VALID_SMART_PTR(thread) ? thread->GetId() : (DWORD)-1);
		return false;
	}

	if (m_vThreadList.empty())
		return false;

	for (const auto & pCurrThread : m_vThreadList)
	{
		if (pCurrThread->dwThreadId == thread->GetId())
		{
			auto threadInfo = GetThreadInfo(pCurrThread->iThreadCode);
			if (IS_VALID_SMART_PTR(threadInfo))
				m_vThreadList.erase(std::remove(m_vThreadList.begin(), m_vThreadList.end(), threadInfo), m_vThreadList.end());
			
			thread->Terminate();

			if (IS_VALID_SMART_PTR(pCurrThread->customThread))
			{
				pCurrThread->customThread.reset();
				pCurrThread->customThread = nullptr;
			}
			return true;
		}
	}

	return false;
}

void CThreadManager::OnThreadTerminated(DWORD dwThreadId) // TODO: Implement routine
{
//	DEBUG_LOG(LL_CRI, "Thread: %u terminated!", dwThreadId);

	if (!dwThreadId)
		return;

	auto pThread = GetThreadFromId(dwThreadId);
	if (!IS_VALID_SMART_PTR(pThread))
		return;

	auto bIsCompleted = pThread->IsRoutineCompleted();
	DEBUG_LOG(LL_CRI, "Thread: %u terminated. Complete status: %d", bIsCompleted);

	// if is termporary thread and bIsCompleted is not completed
	// or if it s not temporary thread
	// ...
}

std::size_t CThreadManager::GetThreadFuncSize(LPBYTE lpFunc)
{
	return 10; // temp

#if 0 // FIXME
	auto ulLength = std::size_t(0);
#ifdef _M_IX86
	auto dwStart = (DWORD)lpFunc;
	while (*(DWORD *)dwStart != 0xAAAAAAAA)
	{
		dwStart++;
		ulLength++;
	}
#endif
	return ulLength;
#endif
}

const std::vector < std::shared_ptr <SSelfThreads> > & CThreadManager::GetThreadList()
{
	return m_vThreadList;
}

std::shared_ptr <CThread> CThreadManager::GetThreadFromThreadCode(int iThreadCode)
{
	for (const auto & thread : m_vThreadList)
		if (thread->iThreadCode == iThreadCode)
			return thread->customThread;

	return nullptr;
}

std::shared_ptr <CThread> CThreadManager::GetThreadFromId(DWORD dwThreadId)
{
	for (const auto & thread : m_vThreadList)
		if (thread->dwThreadId == dwThreadId)
			return thread->customThread;

	return nullptr;
}

std::shared_ptr <CThread> CThreadManager::GetThreadFromAddress(DWORD dwThreadAddress)
{
	for (const auto & thread : m_vThreadList)
		if (thread->dwThreadAddress == dwThreadAddress)
			return thread->customThread;

	return nullptr;
}

std::shared_ptr <CThread> CThreadManager::GetThreadFromHandle(HANDLE hThread)
{
	for (const auto & thread : m_vThreadList)
		if (thread->hThread == hThread)
			return thread->customThread;

	return nullptr;
}

std::shared_ptr <SSelfThreads> CThreadManager::GetThreadInfo(int iThreadCode)
{
	for (const auto & thread : m_vThreadList)
		if (thread->iThreadCode == iThreadCode)
			return thread;

	return nullptr;
}


std::size_t CThreadManager::GetThreadCount()
{
	return m_vThreadList.size();
}

int CThreadManager::GetSuspendedThreadCount()
{
	auto iCount = 0;

	for (const auto & thread : m_vThreadList)
		if (thread->customThread->HasSuspend())
			iCount++;

	return iCount;
}

bool CThreadManager::HasSuspendedThread()
{
	for (const auto & thread : m_vThreadList)
		if (thread->customThread->HasSuspend())
			return true;

	return false;
}

bool CThreadManager::SetAntiTrace(const std::shared_ptr <CThread> & targetThread)
{
	BOOL bCheckStat = FALSE;

	if (!IS_VALID_SMART_PTR(targetThread) || !IS_VALID_HANDLE(targetThread->GetHandle())) 
	{
		DEBUG_LOG(LL_SYS, "Unknown target thread data!");
		return false;
	}
	DEBUG_LOG(LL_SYS, "Anti trace started for: %u[%p]", targetThread->GetId(), targetThread->GetHandle());

	auto ntReturnStat = g_winapiApiTable->ZwSetInformationThread(targetThread->GetHandle(), ThreadHideFromDebugger, &bCheckStat, sizeof(ULONG));
	if (NT_SUCCESS(ntReturnStat)) 
	{
		DEBUG_LOG(LL_SYS, "Anti trace failed on Step1!");
		return false;
	}

	auto ntFakeStat1 = g_winapiApiTable->ZwSetInformationThread(targetThread->GetHandle(), ThreadHideFromDebugger, &bCheckStat, sizeof(UINT));
	if (NT_SUCCESS(ntFakeStat1))
	{
		DEBUG_LOG(LL_SYS, "Anti trace failed on Step2!");
		return false;
	}

	auto ntFakeStat2 = g_winapiApiTable->ZwSetInformationThread((HANDLE)0xFFFFF, ThreadHideFromDebugger, 0, 0);
	if (NT_SUCCESS(ntFakeStat2)) 
	{
		DEBUG_LOG(LL_SYS, "Anti trace failed on Step3!");
		return false;
	}

	auto ntCorrectStat = g_winapiApiTable->ZwSetInformationThread(targetThread->GetHandle(), ThreadHideFromDebugger, 0, 0);
	if (!NT_SUCCESS(ntCorrectStat)) 
	{
		DEBUG_LOG(LL_SYS, "Anti trace failed on Step4, Status: %p", ntCorrectStat);
		return false;
	}

	auto ntCorrectStat2 = g_winapiApiTable->NtQueryInformationThread(targetThread->GetHandle(), ThreadHideFromDebugger, &bCheckStat, sizeof(BOOLEAN), 0);
	if (!NT_SUCCESS(ntCorrectStat))
	{
		DEBUG_LOG(LL_SYS, "Anti trace failed on Step5, Status: %p", ntCorrectStat);
		return false;
	}

	if (!bCheckStat) 
	{
		DEBUG_LOG(LL_SYS, "Anti trace failed on Step6!");
		return false;
	}

	return true;
}

