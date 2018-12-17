#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "ServiceHelper.h"
#include "Defines.h"

CServiceHelper::CServiceHelper()
{
}

CServiceHelper::CServiceHelper(const std::string & szServiceName, const std::string & szDisplayName, const std::string & szServicePath) :
	m_szServiceName(szServiceName), m_szDisplayName(szDisplayName), m_szServicePath(szServicePath)
{
}

inline void PrintDetailedLog(SC_HANDLE shServiceHandle)
{
	DWORD bytesNeeded;
	SERVICE_STATUS_PROCESS ssStatus;
	if (QueryServiceStatusEx(shServiceHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded) == FALSE)
	{
		DEBUG_LOG(LL_ERR, "QueryServiceStatusEx fail! Error: %u", g_winapiApiTable->GetLastError());
		return;
	}

	DEBUG_LOG(LL_SYS, "Service detailed logs handled.\n\tCurrent State: %d\n\tExit Code: %d\n\tCheck Point: %d\n\tWait Hint: %d",
		ssStatus.dwCurrentState, ssStatus.dwWin32ExitCode, ssStatus.dwCheckPoint, ssStatus.dwWaitHint);
}

DWORD CServiceHelper::GetServiceStatus()
{
	auto dwResult	= 0UL;
	auto hSCManager = SC_HANDLE(nullptr);
	auto hService	= SC_HANDLE(nullptr);
	auto sStatus	= SERVICE_STATUS { 0 };

	hSCManager = OpenSCManagerA(0, 0, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		DEBUG_LOG(LL_ERR, "OpenSCManagerA fail! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}
	hService = OpenServiceA(hSCManager, m_szServiceName.c_str(), SERVICE_QUERY_STATUS);
	if (hService == NULL)
	{
		DEBUG_LOG(LL_ERR, "OpenServiceA fail! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}
	if (!QueryServiceStatus(hService, &sStatus))
	{
		DEBUG_LOG(LL_ERR, "QueryServiceStatus fail! Error: %u", g_winapiApiTable->GetLastError());
		goto _Complete;
	}

	dwResult = sStatus.dwCurrentState;

_Complete:
	if (dwResult == 0 && hService)
		PrintDetailedLog(hService);

	if (hSCManager)
	{
		CloseServiceHandle(hSCManager);
		hSCManager = nullptr;
	}
	if (hService)
	{
		CloseServiceHandle(hService);
		hService = nullptr;
	}

	return dwResult;
}

bool CServiceHelper::LoadService(DWORD dwServiceType, LPDWORD pdwErrorCode)
{
	auto bRet		= false;
	auto hSCManager = SC_HANDLE(nullptr);
	auto hService	= SC_HANDLE(nullptr);

	if (g_nmApp->DirFunctionsInstance()->IsFileExist(m_szServicePath) == false)
	{
		DEBUG_LOG(LL_ERR, "Target file: %s is not exist!", m_szServicePath.c_str());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}
 
	hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCManager)
	{
		DEBUG_LOG(LL_ERR, "OpenSCManager fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}
 
	hService = CreateServiceA(hSCManager, m_szServiceName.c_str(), m_szDisplayName.c_str(), SERVICE_ALL_ACCESS, dwServiceType,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, m_szServicePath.c_str(), dwServiceType == SERVICE_FILE_SYSTEM_DRIVER ? "FSFilter Activity Monitor" : NULL, 
		NULL, NULL, NULL, NULL
	);
	if (!hService)
	{
		auto dwError = g_winapiApiTable->GetLastError();
		DEBUG_LOG(LL_ERR, "CreateServiceA fail! Error: %u", dwError);
		if (pdwErrorCode) *pdwErrorCode = dwError;
		goto _Complete;
	}

	bRet = true;

_Complete:
	if (bRet == false && hService)
		PrintDetailedLog(hService);

	if (hSCManager)
	{
		CloseServiceHandle(hSCManager);
		hSCManager = nullptr;
	}
	if (hService)
	{
		CloseServiceHandle(hService);
		hService = nullptr;
	}

	return bRet;
}

bool CServiceHelper::UnloadDriver(LPDWORD pdwErrorCode)
{
	auto bRet		= false;
	auto hSCManager = SC_HANDLE(nullptr);
	auto hService	= SC_HANDLE(nullptr);

	hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		DEBUG_LOG(LL_ERR, "OpenSCManager fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}

	hService = OpenServiceA(hSCManager, m_szServiceName.c_str(), DELETE | SERVICE_QUERY_STATUS);
	if (!hService)
	{
		DEBUG_LOG(LL_ERR, "OpenServiceA fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}
 
	if (DeleteService(hService) == FALSE)
	{
		DEBUG_LOG(LL_ERR, "DeleteService fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}
 
	bRet = true;

_Complete:
	if (bRet == false && hService)
		PrintDetailedLog(hService);

	if (hSCManager)
	{
		CloseServiceHandle(hSCManager);
		hSCManager = nullptr;
	}
	if (hService)
	{
		CloseServiceHandle(hService);
		hService = nullptr;
	}

	return bRet;
}

bool CServiceHelper::StartDriver(LPDWORD pdwErrorCode)
{
	auto bRet		= false;
	auto hSCManager = SC_HANDLE(nullptr);
	auto hService	= SC_HANDLE(nullptr);
  
	hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		DEBUG_LOG(LL_ERR, "OpenSCManager fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}

	hService = OpenServiceA(hSCManager, m_szServiceName.c_str(), SERVICE_START | SERVICE_QUERY_STATUS);
	if (!hService)
	{
		DEBUG_LOG(LL_ERR, "OpenServiceA fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}
 
	if (StartServiceA(hService, 0, NULL) == FALSE)
	{
		DEBUG_LOG(LL_ERR, "StartServiceA fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}
 
	bRet = true;

_Complete:
	if (bRet == false && hService)
		PrintDetailedLog(hService);

	if (hSCManager)
	{
		CloseServiceHandle(hSCManager);
		hSCManager = nullptr;
	}
	if (hService)
	{
		CloseServiceHandle(hService);
		hService = nullptr;
	}

	return bRet;
}

bool CServiceHelper::StopDriver(LPDWORD pdwErrorCode)
{
	auto bRet		= false;
	auto hSCManager = SC_HANDLE(nullptr);
	auto hService	= SC_HANDLE(nullptr);
	auto sStatus	= SERVICE_STATUS { 0 };

	hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		DEBUG_LOG(LL_ERR, "OpenSCManager fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}

	hService = OpenServiceA(hSCManager, m_szServiceName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
	if (!hService)
	{
		DEBUG_LOG(LL_ERR, "OpenServiceA fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}

	if (ControlService(hService, SERVICE_CONTROL_STOP, &sStatus))
	{
		DEBUG_LOG(LL_SYS, "Stopping %s ...", m_szServiceName.c_str());
		g_winapiApiTable->Sleep(500);

		auto pTimer = CTimer<std::chrono::milliseconds>();
		while (QueryServiceStatus(hService, &sStatus))
		{
			if (pTimer.diff() > 5000)
				break;

			if (sStatus.dwCurrentState != SERVICE_STOP_PENDING)
				break;

			DEBUG_LOG(LL_SYS, "Stopping pending %s ...", m_szServiceName.c_str());
			g_winapiApiTable->Sleep(500);
		}

		if (sStatus.dwCurrentState == SERVICE_STOPPED)
		{
			DEBUG_LOG(LL_SYS, "%s Has Successfully Stopped", m_szServiceName.c_str());
		}
		else
		{
			DEBUG_LOG(LL_ERR, "%s Could Not Be Stopped. Status: %u Last error: %u", m_szServiceName.c_str(), sStatus.dwCurrentState, g_winapiApiTable->GetLastError());
			if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
			goto _Complete;
		}
	}
	
	bRet = true;

_Complete:
	if (bRet == false && hService)
		PrintDetailedLog(hService);

	if (hSCManager)
	{
		CloseServiceHandle(hSCManager);
		hSCManager = nullptr;
	}
	if (hService)
	{
		CloseServiceHandle(hService);
		hService = nullptr;
	}

	return bRet;
}
 
bool CServiceHelper::IsInstalled(LPDWORD pdwErrorCode)
{
	auto bRet		= false;
	auto hSCManager = SC_HANDLE(nullptr);
	auto hService	= SC_HANDLE(nullptr);
  
	hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		DEBUG_LOG(LL_ERR, "OpenSCManager fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}

	hService = OpenServiceA(hSCManager, m_szServiceName.c_str(), SERVICE_QUERY_CONFIG);
	if (!hService)
	{
		DEBUG_LOG(LL_ERR, "OpenServiceA fail! Error: %u", g_winapiApiTable->GetLastError());
		if (pdwErrorCode) *pdwErrorCode = g_winapiApiTable->GetLastError();
		goto _Complete;
	}

	bRet = true;

_Complete:
	if (hSCManager)
	{
		CloseServiceHandle(hSCManager);
		hSCManager = nullptr;
	}
	if (hService)
	{
		CloseServiceHandle(hService);
		hService = nullptr;
	}

	return bRet;
}

bool CServiceHelper::SetupFilterInstance(const std::string & szDriverName, const std::string & szInstanceName, const std::string & szAltitude, DWORD Flags, bool SetAsDefaultInstance)
{
	std::string szPath = "System\\CurrentControlSet\\Services\\" + szDriverName;

	// Registering an instance with specified flags and altitude:
	HKEY hKey = NULL;
	LSTATUS RegStatus = RegOpenKeyExA(HKEY_LOCAL_MACHINE, szPath.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (RegStatus != ERROR_SUCCESS) 
		return false;

	HKEY hInstancesKey = NULL;
	DWORD Disposition = 0;
	RegStatus = RegCreateKeyExA(hKey, "Instances", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hInstancesKey, &Disposition);
	RegCloseKey(hKey);

	if (RegStatus != ERROR_SUCCESS)
		return false;

	if (SetAsDefaultInstance) 
	{
		RegStatus = RegSetValueExA(hInstancesKey, "DefaultInstance", 0, REG_SZ, reinterpret_cast<const BYTE*>(szInstanceName.c_str()), szInstanceName.size());
		if (RegStatus != ERROR_SUCCESS) 
		{
			RegCloseKey(hInstancesKey);
			return false;
		}
	}

	HKEY hInstanceKey = NULL;
	RegStatus = RegCreateKeyExA(hInstancesKey, szInstanceName.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hInstanceKey, &Disposition);
	if (RegStatus != ERROR_SUCCESS)
	{
		RegCloseKey(hInstancesKey);
		return false;
	}
	RegCloseKey(hInstancesKey);

	RegStatus = RegSetValueExA(hInstanceKey, "Altitude", 0, REG_SZ, reinterpret_cast<const BYTE*>(szAltitude.c_str()), szAltitude.size());
	if (RegStatus != ERROR_SUCCESS) 
	{
		RegCloseKey(hInstanceKey);
		return false;
	}

	RegStatus = RegSetValueExA(hInstanceKey, "Flags", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&Flags), sizeof(Flags));

	RegCloseKey(hInstanceKey);

	return RegStatus == ERROR_SUCCESS;
}


