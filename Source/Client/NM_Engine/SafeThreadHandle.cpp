#include "NM_Index.h"
#include "NM_Main.h"
#include "SafeThreadHandle.h"
#include "SafeProcessHandle.h"
#include "DynamicWinAPI.h"
#include "WinVerHelper.h"
#include "Defines.h"

CSafeThreadHandle::CSafeThreadHandle(DWORD dwAccess) :
	m_dwAccess(dwAccess)
{
	CloseUselessHandles();
}
CSafeThreadHandle::~CSafeThreadHandle()
{
	CloseUselessHandles();
}

void CSafeThreadHandle::CloseUselessHandles()
{
	if (m_vHandleList.empty())
		return;

	for (std::size_t i = 0UL; i < m_vHandleList.size(); ++i)
	{
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(m_vHandleList[i]);
	}
	m_vHandleList.clear();
}

HANDLE CSafeThreadHandle::FindThread(HANDLE hOwnerProcess, DWORD dwTargetTID)
{
	if (!IsWindowsVistaOrGreater())
		return INVALID_HANDLE_VALUE;

	HANDLE hCurrent = nullptr;

	while (g_winapiApiTable->NtGetNextThread(hOwnerProcess, hCurrent, m_dwAccess, 0, 0, &hCurrent) == STATUS_SUCCESS)
	{
		if (g_winapiApiTable->GetThreadId(hCurrent) == dwTargetTID)
			return hCurrent;
		
		m_vHandleList.push_back(hCurrent);
	}

	return INVALID_HANDLE_VALUE;
}

std::vector <HANDLE> CSafeThreadHandle::EnumerateThreads(HANDLE hOwnerProcess)
{
	if (!IsWindowsVistaOrGreater())
		return m_vHandleList;

	HANDLE hCurrent = nullptr;

	while (g_winapiApiTable->NtGetNextThread(hOwnerProcess, hCurrent, m_dwAccess, 0, 0, &hCurrent) == STATUS_SUCCESS)
	{
		m_vHandleList.push_back(hCurrent);
	}

	return m_vHandleList;
}

std::vector <HANDLE> CSafeThreadHandle::EnumerateThreads()
{
	if (!IsWindowsVistaOrGreater())
		return m_vHandleList;

	auto processEnumerator = std::make_unique<CSafeProcessHandle>();
	auto vProcesses = processEnumerator->EnumerateProcesses(true);
	if (vProcesses.empty())
		return m_vHandleList;

	HANDLE hCurrent = nullptr;
	for (const auto & hCurrentProcess : vProcesses)
	{
		while (g_winapiApiTable->NtGetNextThread(hCurrentProcess, hCurrent, m_dwAccess, 0, 0, &hCurrent) == STATUS_SUCCESS)
		{
			m_vHandleList.push_back(hCurrent);
		}
	}
	return m_vHandleList;
}
