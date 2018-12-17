#include "NM_Index.h"
#include "NM_Main.h"
#include "ThreadHelper.h"
#include "ThreadEnumerator.h"
#include "ThreadFunctions.h"
#include "Defines.h"
#include "SafeThreadHandle.h"
#include "ProcessFunctions.h"

#include <lazy_importer.hpp>

CThread::CThread(DWORD dwThreadId) :
	m_dwThreadId(dwThreadId), m_hThread(INVALID_HANDLE_VALUE), m_iThreadCode(0), m_bOpenedThread(false), m_bRoutineCompleted(false)
{
	auto bFail = false;
	auto dwErr = 0UL;

	auto customThread = g_nmApp->ThreadManagerInstance()->GetThreadFromId(dwThreadId);
	if (IS_VALID_SMART_PTR(customThread)) 
	{
		m_hThread		= customThread->GetHandle();
		m_iThreadCode	= customThread->GetCustomCode();
	}
	else 
	{
		m_hThread = g_winapiApiTable->OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);
		if (!IS_VALID_HANDLE(m_hThread))
		{
			bFail = true;
			dwErr = g_winapiApiTable->GetLastError();

			auto threadEnumerator = std::make_unique<CSafeThreadHandle>();
			if (IS_VALID_SMART_PTR(threadEnumerator))
			{
				auto vThreads = threadEnumerator->EnumerateThreads(NtCurrentProcess);
				if (vThreads.empty() == false)
				{
					for (const auto & hCurrThread : vThreads)
					{
						auto dwCurrThreadId = g_winapiApiTable->GetThreadId(hCurrThread);
						if (dwCurrThreadId == dwThreadId)
						{
							bFail = false;
							break;
						}
					}
				}
			}
		}

		if (bFail)
		{
			DEBUG_LOG(LL_CRI, "CThread class can not created for: %u Error: %u Alive: %d", dwThreadId, dwErr, CThreadFunctions::ThreadIsItAlive(dwThreadId));
			g_nmApp->OnCloseRequest(EXIT_ERR_CUSTOM_THREAD_OPEN_FAIL, g_winapiApiTable->GetLastError());
		}

		m_bOpenedThread = true;
	}
}
CThread::CThread(HANDLE hThread) :
	m_hThread(hThread), m_dwThreadId(0), m_iThreadCode(-1), m_bOpenedThread(false), m_bRoutineCompleted(false)
{
	auto customThread = g_nmApp->ThreadManagerInstance()->GetThreadFromHandle(m_hThread);
	if (IS_VALID_SMART_PTR(customThread)) {
		m_dwThreadId	= customThread->GetId();
		m_iThreadCode	= customThread->GetCustomCode();
	}
	else {
		m_dwThreadId	= CThreadFunctions::GetThreadID(hThread);
		m_bOpenedThread = true;
	}
}
CThread::CThread(HANDLE hThread, int iThreadCode) :
	m_hThread(hThread), m_iThreadCode(iThreadCode), m_dwThreadId(0), m_bOpenedThread(false), m_bRoutineCompleted(false)
{
	m_dwThreadId = CThreadFunctions::GetThreadID(hThread);
}
CThread::CThread(HANDLE hThread, DWORD dwThreadId, int iThreadCode) :
	m_hThread(hThread), m_iThreadCode(iThreadCode), m_dwThreadId(dwThreadId), m_bOpenedThread(false), m_bRoutineCompleted(false)
{
}

CThread::~CThread()
{
	m_hThread = INVALID_HANDLE_VALUE;

	m_iThreadCode	= -2;
	m_dwThreadId	= 0;
	m_bOpenedThread = false;
}

void CThread::Terminate()
{
	if (!IS_VALID_HANDLE(m_hThread))
		return;

	if (!g_nmApp->DynamicWinapiInstance()->IsValidHandle(m_hThread))
		return;

	g_winapiApiTable->TerminateThread(m_hThread, EXIT_SUCCESS);
	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(m_hThread);
}

void CThread::ClearDebugRegisters()
{
	auto registerCleaner = [](LPVOID lpParam) -> DWORD
	{
		auto hThread = (HANDLE)lpParam;

		CONTEXT ctx = { 0 };
		ctx.ContextFlags = CONTEXT_ALL;

		if (LI_FIND(SuspendThread)(hThread) == (DWORD)-1) 
		{
			DEBUG_LOG(LL_ERR, xorstr("C!g_winapiApiTable->SuspendThread").crypt_get());
			return 0;
		}

		if (!LI_FIND(GetThreadContext)(hThread, &ctx))
		{
			DEBUG_LOG(LL_ERR, xorstr("C!g_winapiApiTable->GetThreadContext").crypt_get());
			return 0;
		}

		if (ctx.Dr0) 
		{
			DEBUG_LOG(LL_DEV, "Cctx.Dr0: %p cleaned!", ctx.Dr0);
			ctx.Dr0 = 0;
		}
		if (ctx.Dr1) 
		{
			DEBUG_LOG(LL_DEV, "Cctx.Dr1: %p cleaned!", ctx.Dr1);
			ctx.Dr1 = 0;
		}
		if (ctx.Dr2) 
		{
			DEBUG_LOG(LL_DEV, xorstr("Cctx.Dr2: %p cleaned!").crypt_get(), ctx.Dr2);
			ctx.Dr2 = 0;
		}
		if (ctx.Dr3) 
		{
			DEBUG_LOG(LL_DEV, xorstr("Cctx.Dr3: %p cleaned!").crypt_get(), ctx.Dr3);
			ctx.Dr3 = 0;
		}
		if (ctx.Dr7) 
		{
			DEBUG_LOG(LL_DEV, xorstr("Cctx.Dr7: %p cleaned!").crypt_get(), ctx.Dr7);
			ctx.Dr7 = 0;
		}

		if (!LI_FIND(SetThreadContext)(hThread, &ctx))
		{
			DEBUG_LOG(LL_ERR, xorstr("C!g_winapiApiTable->SetThreadContext").crypt_get());
			return 0;
		}

		if (LI_FIND(ResumeThread)(hThread) == (DWORD)-1)
		{
			DEBUG_LOG(LL_ERR, xorstr("!g_winapiApiTable->ResumeThread").crypt_get());
			return 0;
		}

		return 0;
	};

	if (HasDebugRegisters() == false) 
	{
		DEBUG_LOG(LL_SYS, "Has not debug registers!");
		return;
	}

	if (NtCurrentThread != m_hThread) 
	{
		registerCleaner(m_hThread);
	}
#if 0
	else // TESTME
	{
		auto hCleanerThead = g_winapiApiTable->CreateThread(nullptr, 0, registerCleaner, m_hThread, 0, 0);
		if (!hCleanerThead || hCleanerThead == INVALID_HANDLE_VALUE) {
			DEBUG_LOG(LL_SYS, xorstr("Cleaner thread can not created!"));
			return;
		}

		auto dwWaitRet = WaitForSingleObject(hCleanerThead, 5000);
		if (dwWaitRet == WAIT_TIMEOUT) {
			DEBUG_LOG(LL_SYS, xorstr("Cleaner thread wait timeout!"));
			return;
		}
	}
#endif

	if (HasDebugRegisters() == true) {
		DEBUG_LOG(LL_ERR, "Debug registers can not cleaned");
	}
	else {
		DEBUG_LOG(LL_SYS, "Debug registers cleaned!");
	}
}

void CThread::Wait(DWORD dwMSDelay)
{
	auto dwWaitRet = g_winapiApiTable->WaitForSingleObject(m_hThread, dwMSDelay);
	DEBUG_LOG(LL_SYS, "Wait completed for: %u, Delay: %u, Ret: %u", m_dwThreadId, dwMSDelay, dwWaitRet);
}


void CThread::SetPriority(int iPriority)
{
	auto bPriorityRet = g_winapiApiTable->SetThreadPriority(m_hThread, iPriority);
	DEBUG_LOG(LL_SYS, "Set Priority completed for: %u, New: %u, Ret: %u", m_dwThreadId, iPriority, bPriorityRet ? 1 : g_winapiApiTable->GetLastError());
}

void CThread::SetRoutineCompleted(bool bCompleted)
{
	m_bRoutineCompleted = bCompleted;
}


HANDLE CThread::GetHandle()
{
	return m_hThread;
}

DWORD CThread::GetId()
{
	return m_dwThreadId;
}

int CThread::GetCustomCode()
{
	return m_iThreadCode;
}

std::string CThread::GetThreadCustomName() // TODO: Add other threads
{
	switch (m_iThreadCode)
	{
		case SELF_THREAD_DIRFUNCS:
			return xorstr("Dir check thread").crypt_get();
	}

	return std::string(xorstr("Unknown thread: ").crypt_get()) + std::to_string(m_iThreadCode);
}

DWORD CThread::GetStartAddress()
{
	auto dwThreadStartAddress = 0UL;
	auto ntStatus = g_winapiApiTable->NtQueryInformationThread(m_hThread, ThreadQuerySetWin32StartAddress, &dwThreadStartAddress, sizeof(dwThreadStartAddress), NULL);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, xorstr("NtQueryInformationThread fail! Thread: %p Status: %p").crypt_get(), m_hThread, ntStatus);
	}
	return dwThreadStartAddress;
}

int CThread::GetPriority()
{
	return g_winapiApiTable->GetThreadPriority(m_hThread);
}

DWORD CThread::GetProcessId()
{
	THREAD_BASIC_INFORMATION ThreadInfo;
	auto ntStatus = g_winapiApiTable->NtQueryInformationThread(m_hThread, ThreadBasicInformation, &ThreadInfo, sizeof(ThreadInfo), NULL);
	if (!NT_SUCCESS(ntStatus))
	{
		DEBUG_LOG(LL_ERR, xorstr("NtQueryInformationThread fail! Thread: %p Status: %p").crypt_get(), m_hThread, ntStatus);
		return g_winapiApiTable->GetCurrentProcessId();
	}
	return (DWORD)ThreadInfo.ClientId.UniqueProcess;
}

std::string CThread::GetThreadOwnerFullName()
{
	auto dwStartAddress = GetStartAddress();
	if (!dwStartAddress)
		return std::string("");

	char cFileName[2048] = { 0 };
	if (!g_winapiApiTable->GetMappedFileNameA(NtCurrentProcess, (LPVOID)dwStartAddress, cFileName, 2048))
		return std::string("");

	std::string szFileName = cFileName;
	std::transform(szFileName.begin(), szFileName.end(), szFileName.begin(), tolower);

	return szFileName;
}

std::string CThread::GetThreadOwnerFileName()
{
	auto szFullName = GetThreadOwnerFullName();
	if (szFullName.empty())
		return std::string("");

	auto szFileName = g_nmApp->DirFunctionsInstance()->GetNameFromPath(szFullName);
	if (szFileName.empty())
		return std::string("");

	return szFileName;
}

std::shared_ptr <CONTEXT> CThread::GetContext()
{
	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_ALL;

	if (!g_winapiApiTable->GetThreadContext(m_hThread, &ctx)) 
	{
		DEBUG_LOG(LL_ERR, xorstr("GetThreadContext fail! Error: %u").crypt_get(), g_winapiApiTable->GetLastError());
		return nullptr;
	}

	return std::make_shared<CONTEXT>(ctx);
}

DWORD CThread::GetModuleBaseAddress()
{
	auto dwStartAddress = GetStartAddress();
	if (!dwStartAddress)
		return 0;

	auto pOwnModule = (LDR_MODULE *)g_nmApp->DynamicWinapiInstance()->FindModuleFromAddress(dwStartAddress);
	if (!pOwnModule)
		return 0;

	return (DWORD)pOwnModule->BaseAddress;
}

std::size_t CThread::GetModuleSize()
{
	auto dwStartAddress = GetStartAddress();
	if (!dwStartAddress)
		return 0;

	auto pOwnModule = (LDR_MODULE *)g_nmApp->DynamicWinapiInstance()->FindModuleFromAddress(dwStartAddress);
	if (!pOwnModule)
		return 0;

	return pOwnModule->SizeOfImage;
}


bool CThread::IsOpenedThread()
{
	return m_bOpenedThread;
}

bool CThread::IsCorrect()
{
	return ( m_hThread && m_hThread != INVALID_HANDLE_VALUE );
}

bool CThread::IsItAlive()
{
	auto dwWaitRet = g_winapiApiTable->WaitForSingleObject(m_hThread, 0);
	return ( dwWaitRet == WAIT_TIMEOUT );
}

bool CThread::IsRemoteThread()
{
	if (this->GetProcessId() != g_winapiApiTable->GetCurrentProcessId()) // PEB
		return true;
	return (CThreadFunctions::GetThreadOwnerProcessId(m_dwThreadId) != g_winapiApiTable->GetCurrentProcessId()); // toolhelp32
}

bool CThread::IsSelfThread()
{
	return (g_nmApp->ThreadManagerInstance()->GetThreadFromId(m_dwThreadId) != nullptr);
}

bool CThread::IsGoodPriority()
{
	return (GetPriority() >= 0);
}

bool CThread::IsRoutineCompleted()
{
	return m_bRoutineCompleted;
}


bool CThread::HasSuspend()
{
	auto dwOwnerPID = CThreadFunctions::GetThreadOwnerProcessId(m_dwThreadId);
	if (dwOwnerPID == 0) 
	{
		DEBUG_LOG(LL_ERR, xorstr("dwOwnerPID null!").crypt_get());
		return true;
	}

	auto threadEnumerator = std::make_shared<CThreadEnumerator>(dwOwnerPID);
	if (!IS_VALID_SMART_PTR(threadEnumerator)) 
	{
		DEBUG_LOG(LL_ERR, xorstr("threadEnumerator allocation failed!").crypt_get());
		return true;
	}

	auto systemThreadOwnerProcInfo = (SYSTEM_PROCESS_INFORMATION*)threadEnumerator->GetProcInfo();
	if (systemThreadOwnerProcInfo == nullptr) 
	{
		DEBUG_LOG(LL_ERR, xorstr("systemThreadOwnerProcInfo failed!").crypt_get());
		return true;
	}

	auto systemThreadInfo = (SYSTEM_THREAD_INFORMATION*)threadEnumerator->FindThread(systemThreadOwnerProcInfo, m_dwThreadId);
	if (systemThreadInfo == nullptr) 
	{
		DEBUG_LOG(LL_ERR, xorstr("systemThreadInfo failed!").crypt_get());
		return true;
	}

	if (systemThreadInfo->ThreadState == Waiting && systemThreadInfo->WaitReason == Suspended)
		return true;
	
	return false;
}

bool CThread::HasDebugRegisters()
{
	auto ctx = GetContext();
	return (ctx->Dr0 || ctx->Dr1 || ctx->Dr2 || ctx->Dr3 || ctx->Dr7);
}

bool CThread::TrySuspend()
{
	return (g_winapiApiTable->SuspendThread(m_hThread) != (DWORD)-1);
}

bool CThread::TryResume()
{
	return (g_winapiApiTable->ResumeThread(m_hThread) != (DWORD)-1);
}


