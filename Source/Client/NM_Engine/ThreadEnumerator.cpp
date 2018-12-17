#include "NM_Index.h"
#include "NM_Main.h"
#include "ThreadEnumerator.h"
#include "DynamicWinapi.h"

CThreadEnumerator::CThreadEnumerator(DWORD dwProcessId) :
	m_dwProcessId(dwProcessId)
{
	m_Cap = InitializeQuery();
}

CThreadEnumerator::~CThreadEnumerator()
{
	m_dwProcessId = 0;

	if (m_Cap)
		free(m_Cap);
	m_Cap = nullptr;
}


BYTE * CThreadEnumerator::InitializeQuery()
{
	BYTE * mp_Data;
	DWORD mu32_DataSize = 1024 * 1024;

	while (true)
	{
		mp_Data = (BYTE*)malloc(mu32_DataSize);
		if (!mp_Data)
			break;

		ULONG ntNeeded = 0;
		auto ntStat = g_winapiApiTable->NtQuerySystemInformation(SystemProcessInformation, mp_Data, mu32_DataSize, &ntNeeded);

		if (ntStat == STATUS_INFO_LENGTH_MISMATCH)
		{
			mu32_DataSize *= 2;
			mp_Data = (BYTE*)realloc((PVOID)mp_Data, mu32_DataSize);
			continue;
		}

		return mp_Data;
	}

	g_nmApp->OnCloseRequest(EXIT_ERR_THREAD_ENUM_QUERY_FAIL, g_winapiApiTable->GetLastError());
	return nullptr;
}

LPVOID CThreadEnumerator::GetProcInfo()
{
	auto pk_Proc = (SYSTEM_PROCESS_INFORMATION*)m_Cap;

	while (true)
	{
		if (reinterpret_cast<DWORD_PTR>(pk_Proc->UniqueProcessId) == m_dwProcessId)
			return pk_Proc;

		if (!pk_Proc->NextEntryOffset)
			return nullptr;

		pk_Proc = (SYSTEM_PROCESS_INFORMATION*)((BYTE*)pk_Proc + pk_Proc->NextEntryOffset);
	}

	return nullptr;
}

LPVOID CThreadEnumerator::GetThreadList(LPVOID procInfo)
{
	auto pProcInfo = (SYSTEM_PROCESS_INFORMATION *)procInfo;
	auto pk_Thread = pProcInfo->Threads;
	return pk_Thread;
}

DWORD CThreadEnumerator::GetThreadCount(LPVOID procInfo)
{
	auto pProcInfo = (SYSTEM_PROCESS_INFORMATION *)procInfo;
	return pProcInfo->NumberOfThreads;
}

LPVOID CThreadEnumerator::FindThread(LPVOID procInfo, DWORD dwThreadId)
{
	auto pProcInfo = (SYSTEM_PROCESS_INFORMATION *)procInfo;
	auto pk_Thread = pProcInfo->Threads;
	if (!pk_Thread)
		return nullptr;

	for (DWORD i = 0; i < pProcInfo->NumberOfThreads; i++)
	{
		if (reinterpret_cast<DWORD_PTR>(pk_Thread->ClientId.UniqueThread) == dwThreadId)
			return pk_Thread;

		pk_Thread++;
	}

	return nullptr;
}
