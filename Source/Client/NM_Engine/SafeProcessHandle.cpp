#include "NM_Index.h"
#include "NM_Main.h"
#include "SafeProcessHandle.h"
#include "DynamicWinAPI.h"
#include "WinVerHelper.h"
#include "Defines.h"

CSafeProcessHandle::CSafeProcessHandle(DWORD dwAccess) :
	m_dwAccess(dwAccess)
{
	CloseUselessHandles();
}
CSafeProcessHandle::~CSafeProcessHandle()
{
	CloseUselessHandles();
}

void CSafeProcessHandle::CloseUselessHandles()
{
	if (m_vHandleList.empty())
		return;

	for (std::size_t i = 0UL; i < m_vHandleList.size(); ++i)
	{
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(m_vHandleList[i]);
	}
	m_vHandleList.clear();
}

HANDLE CSafeProcessHandle::FindProcessFromPID(DWORD dwProcessID, bool bAliveOnly)
{
	if (!IsWindowsVistaOrGreater())
		return INVALID_HANDLE_VALUE;

	auto dwExitCode = 0UL;
	HANDLE hCurrent = nullptr;

	while (g_winapiApiTable->NtGetNextProcess(hCurrent, m_dwAccess, 0, 0, &hCurrent) == STATUS_SUCCESS)
	{
		if (bAliveOnly && (!g_winapiApiTable->GetExitCodeProcess(hCurrent, &dwExitCode) || dwExitCode != STILL_ACTIVE))
			continue;

		if (dwProcessID == g_winapiApiTable->GetProcessId(hCurrent))
			return hCurrent;
		
		m_vHandleList.push_back(hCurrent);
	}

	return INVALID_HANDLE_VALUE;
}


std::vector <HANDLE> CSafeProcessHandle::EnumerateProcesses(bool bAliveOnly)
{
	if (!IsWindowsVistaOrGreater())
		return m_vHandleList;

	auto dwExitCode = 0UL;
	HANDLE hCurrent = nullptr;

	while (g_winapiApiTable->NtGetNextProcess(hCurrent, m_dwAccess, 0, 0, &hCurrent) == STATUS_SUCCESS)
	{
		if (bAliveOnly && (!g_winapiApiTable->GetExitCodeProcess(hCurrent, &dwExitCode) || dwExitCode != STILL_ACTIVE))
			continue;

		m_vHandleList.push_back(hCurrent);
	}

	return m_vHandleList;
}

