#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "Access.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "SafeProcessHandle.h"
#include "ProcessFunctions.h"
#include "StackTrace.h"
#include "ExceptionHandlers.h"

bool SetTokenPrivilege(HANDLE TokenHandle, PWSTR PrivilegeName, PLUID PrivilegeLuid, ULONG Attributes)
{
	TOKEN_PRIVILEGES privileges;
	privileges.PrivilegeCount = 1;
	privileges.Privileges[0].Attributes = Attributes;
	privileges.Privileges[0].Luid = *PrivilegeLuid;

	auto ntStat = g_winapiApiTable->NtAdjustPrivilegesToken(TokenHandle, FALSE, &privileges, 0 /* sizeof(privileges) */, NULL, NULL);
	if (!NT_SUCCESS(ntStat)) 
	{
		DEBUG_LOG(LL_ERR, "NtAdjustPrivilegesToken fail! Ntstat: %p", ntStat);
	}
#if 0
	if (ntStat == STATUS_NOT_ALL_ASSIGNED)
	{
		DEBUG_LOG(LL_ERR, "NtAdjustPrivilegesToken returned with: STATUS_NOT_ALL_ASSIGNED");
	}
#endif
	return true;
}

inline bool DecreasePrivilegeEx(HANDLE hProcess)
{
	HANDLE hToken;
	auto ntStat = g_winapiApiTable->NtOpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	if (!NT_SUCCESS(ntStat))
	{
		DEBUG_LOG(LL_ERR, "NtOpenProcessToken fail! Status: %p", ntStat);
		return false;
	}

	LUID luid;
	if (!g_winapiApiTable->LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &luid))
	{
		DEBUG_LOG(LL_ERR, "LookupPrivilegeValueW fail! Error: %u", g_winapiApiTable->GetLastError());
		g_winapiApiTable->CloseHandle(hToken);
		return false;
	}

	if (SetTokenPrivilege(hToken, NULL, &luid, SE_PRIVILEGE_REMOVED) == false)  
	{
		g_winapiApiTable->CloseHandle(hToken);
		return false;
	}

	g_winapiApiTable->CloseHandle(hToken);
	return true;
}

bool CAccess::DecreasePrivilege(HANDLE hProcess)
{
	__try
	{
		if (DecreasePrivilegeEx(hProcess) == false)
			return false;
	}
	__except (CExceptionHandlers::OnExceptionThrowed(GetExceptionInformation()))
	{
//		Logf(CUSTOM_LOG_FILENAME, xorstr("DecreasePrivilege fail!\n").crypt_get());
	}
	return true;
}

bool CAccess::RemoveProcessDebugPriv(DWORD dwProcessId, HANDLE hProcess)
{
	if (HasInVector(m_vUnopenedProcessIds, dwProcessId) || HasInVector(m_vBlockedProcessIds, dwProcessId))
		return true;

	if (dwProcessId == g_winapiApiTable->GetCurrentProcessId())
	{
		m_vBlockedProcessIds.push_back(dwProcessId);
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
		return true;
	}

	auto hShadowProcess		= g_nmApp->DataInstance()->GetShadowProcessHandle();
	auto dwShadowProcessId	= g_winapiApiTable->GetProcessId(hShadowProcess);
	if (dwProcessId == dwShadowProcessId)
	{
		m_vBlockedProcessIds.push_back(dwProcessId);
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
		return true;
	}

	auto szProcessName = CProcessFunctions::GetProcessName(hProcess);
	if (szProcessName.empty() == false)
	{
		auto szLowerProcessName		= g_nmApp->FunctionsInstance()->szLower(szProcessName);

		auto szExeNameWithPath		= g_nmApp->DirFunctionsInstance()->ExeNameWithPath();
		auto szLowerExeNameWithPath = g_nmApp->FunctionsInstance()->szLower(szExeNameWithPath);
		if (!strcmp(szLowerProcessName.c_str(), szLowerExeNameWithPath.c_str()))
		{
			DEBUG_LOG(LL_SYS, "Itself access adjust passed! %u", dwProcessId);

			m_vBlockedProcessIds.push_back(dwProcessId);
			g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
			return true;
		}

#ifdef _DEBUG
		if ((strstr(szLowerProcessName.c_str(), xorstr("conhost.exe").crypt_get()) || strstr(szLowerProcessName.c_str(), xorstr("devenv.exe").crypt_get())))
		{
			DEBUG_LOG(LL_SYS, "Console access adjust passed! %u", dwProcessId);

			m_vBlockedProcessIds.push_back(dwProcessId);
			g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
			return true;
		}
#endif

		if (strstr(szLowerProcessName.c_str(), xorstr("crashsender1402.exe").crypt_get()))
		{
			DEBUG_LOG(LL_SYS, "Whitelist access adjust passed! %u:%s", dwProcessId, szLowerProcessName.c_str());

			m_vBlockedProcessIds.push_back(dwProcessId);
			g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
			return true;
		}
	}
#if 0
	else
	{
		DEBUG_LOG(LL_ERR, "Process: %u(%p) file name not found! Last error: %u", hProcess, dwProcessId, g_winapiApiTable->GetLastError());
		m_vUnopenedProcessIds.push_back(dwProcessId);
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
		return false;
	}
#endif

	if (g_nmApp->AccessHelperInstance()->DecreasePrivilege(hProcess) == false)
	{
		DEBUG_LOG(LL_ERR, "Process decrease debug privilege fail! Target PID: %u Last error: %u", dwProcessId, g_winapiApiTable->GetLastError());

		m_vUnopenedProcessIds.push_back(dwProcessId);
		g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
		return false;
	}

	DEBUG_LOG(LL_SYS, "Access rules adjusted to PID: %u Name: %s", dwProcessId, szProcessName.c_str());
	m_vBlockedProcessIds.push_back(dwProcessId);

	g_nmApp->DynamicWinapiInstance()->SafeCloseHandle(hProcess);
	return true;
}

